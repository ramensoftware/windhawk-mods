// ==WindhawkMod==
// @id              classic-theme-metrics-fix
// @name            Fix for Classic theme metrics on 24H2, 25H2, 26H2 builds 9444+
// @description     Syncs WindowMetrics of current user with default user at theme change and logon.
// @version         1.0
// @author          Anixx
// @github 			https://github.com/Anixx
// @include         winlogon.exe
// @compilerOptions -ladvapi32 -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod is intended to workaround the bug with broken titlebar and scrollbar buttons in Classic theme on Windows 11 24H2, 25H2, 26H2 with build number 9444 and newer or version 26H1. If you are on build 8875 or below, you do not need this mod.

On the newest builds of Windows Microsoft broke the very core of its operating system, the essential part of the kernel,
the module win32k.sys. This led to the titlebar buttons and scrollbars in unthemed applications being broken.

How it works? On the affected build the actual size of the titlebar and scrollbar buttons is taken from the user's registry
section, while their visible size is takes from the default user. This makes the buttons to be painted with different size than their actual size, creating garbage and visual glitches.

This mod copies the user's window metrics data from the current user (HKEY_CURRENT_USER\Control Panel\Desktop\WindowMetrics) to the default user (HKEY_USERS\.DEFAULT\Control Panel\Desktop\WindowMetrics) upon theme change, thus
forcing them being in sync. 

Unfortunately, the values from the default user's registry are read only once during the login screen. This makes it impossible to intercept the query during the interactive user's session.
After a theme change, re-login for the changes to take effect.

Before mod:

![before](https://i.imgur.com/MSXt5Wt.png)

After mod:

![after](https://i.imgur.com/Y8GreLt.png)
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <wtsapi32.h>
#include <sddl.h>

HANDLE g_hStopEvent = NULL;
HANDLE g_hThread = NULL;

// Получение строкового SID активного пользователя сессии
BOOL GetActiveUserSidString(LPWSTR* ppszSid) {
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    if (sessionId == 0xFFFFFFFF) {
        return FALSE;
    }

    HANDLE hToken = NULL;
    if (!WTSQueryUserToken(sessionId, &hToken)) {
        return FALSE;
    }

    BOOL bSuccess = FALSE;
    DWORD dwLen = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &dwLen);
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        PTOKEN_USER pUser = (PTOKEN_USER)malloc(dwLen);
        if (pUser) {
            if (GetTokenInformation(hToken, TokenUser, pUser, dwLen, &dwLen)) {
                bSuccess = ConvertSidToStringSidW(pUser->User.Sid, ppszSid);
            }
            free(pUser);
        }
    }

    CloseHandle(hToken);
    return bSuccess;
}

// Резервное копирование оригинальных значений .DEFAULT (до первой перезаписи)
void BackupDefaultMetrics() {
    HANDLE hMutex = CreateMutexW(NULL, FALSE, L"Global\\Windhawk_ClassicThemeMetrics_BackupMutex");
    if (!hMutex) return;
    
    WaitForSingleObject(hMutex, INFINITE);

    // Сначала проверяем, есть ли уже бэкап
    size_t existingSize = Wh_GetBinaryValue(L"Backup", NULL, 0);
    if (existingSize > 0) {
        // Бэкап уже сделан другой копией мода (или при предыдущем срабатывании), пропускаем
    } else {
        HKEY hKey = NULL;
        if (RegOpenKeyExW(HKEY_USERS, L".DEFAULT\\Control Panel\\Desktop\\WindowMetrics", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            DWORD cValues = 0, cchMaxName = 0, cbMaxData = 0;
            if (RegQueryInfoKeyW(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cchMaxName, &cbMaxData, NULL, NULL) == ERROR_SUCCESS) {
                cchMaxName++;
                WCHAR* nameBuf = (WCHAR*)malloc(cchMaxName * sizeof(WCHAR));
                BYTE* dataBuf = (BYTE*)malloc(cbMaxData);
                
                DWORD bufCap = 8192; // 8KB должно хватить
                BYTE* blob = (BYTE*)malloc(bufCap);
                
                if (nameBuf && dataBuf && blob) {
                    DWORD blobOffset = sizeof(DWORD);
                    DWORD actualCount = 0;

                    for (DWORD i = 0; i < cValues; i++) {
                        DWORD cchName = cchMaxName;
                        DWORD cbData = cbMaxData;
                        DWORD dwType = 0;
                        if (RegEnumValueW(hKey, i, nameBuf, &cchName, NULL, &dwType, dataBuf, &cbData) == ERROR_SUCCESS) {
                            DWORD nameBytes = (cchName + 1) * sizeof(WCHAR);
                            DWORD entrySize = sizeof(DWORD) * 3 + nameBytes + cbData;
                            
                            if (blobOffset + entrySize > bufCap) {
                                bufCap *= 2;
                                BYTE* newBlob = (BYTE*)realloc(blob, bufCap);
                                if (newBlob) {
                                    blob = newBlob;
                                } else {
                                    break; // Ошибка выделения памяти
                                }
                            }
                            
                            *((DWORD*)(blob + blobOffset)) = nameBytes; blobOffset += sizeof(DWORD);
                            *((DWORD*)(blob + blobOffset)) = dwType;    blobOffset += sizeof(DWORD);
                            *((DWORD*)(blob + blobOffset)) = cbData;    blobOffset += sizeof(DWORD);
                            
                            memcpy(blob + blobOffset, nameBuf, nameBytes); blobOffset += nameBytes;
                            memcpy(blob + blobOffset, dataBuf, cbData);    blobOffset += cbData;
                            
                            actualCount++;
                        }
                    }
                    *((DWORD*)blob) = actualCount; // Записываем общее число элементов в начало блоба
                    Wh_SetBinaryValue(L"Backup", blob, blobOffset);
                }
                
                if (nameBuf) free(nameBuf);
                if (dataBuf) free(dataBuf);
                if (blob) free(blob);
            }
            RegCloseKey(hKey);
        }
    }
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

// Восстановление оригинальных значений при выгрузке мода
void RestoreDefaultMetrics() {
    HANDLE hMutex = CreateMutexW(NULL, FALSE, L"Global\\Windhawk_ClassicThemeMetrics_BackupMutex");
    if (!hMutex) return;
    
    WaitForSingleObject(hMutex, INFINITE);

    size_t blobSize = Wh_GetBinaryValue(L"Backup", NULL, 0);
    if (blobSize > 0) {
        BYTE* blob = (BYTE*)malloc(blobSize);
        if (blob) {
            if (Wh_GetBinaryValue(L"Backup", blob, blobSize) == blobSize) {
                HKEY hKey = NULL;
                if (RegOpenKeyExW(HKEY_USERS, L".DEFAULT\\Control Panel\\Desktop\\WindowMetrics", 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
                    DWORD count = *((DWORD*)blob);
                    DWORD offset = sizeof(DWORD);
                    for (DWORD i = 0; i < count; i++) {
                        // Защита от чтения за пределами буфера
                        if (offset + sizeof(DWORD)*3 > blobSize) break;
                        
                        DWORD nameBytes = *((DWORD*)(blob + offset)); offset += sizeof(DWORD);
                        DWORD dwType    = *((DWORD*)(blob + offset)); offset += sizeof(DWORD);
                        DWORD cbData    = *((DWORD*)(blob + offset)); offset += sizeof(DWORD);
                        
                        if (offset + nameBytes + cbData > blobSize) break;
                        
                        WCHAR* pName = (WCHAR*)(blob + offset); offset += nameBytes;
                        BYTE* pData  = (BYTE*)(blob + offset);  offset += cbData;
                        
                        RegSetValueExW(hKey, pName, 0, dwType, pData, cbData);
                    }
                    RegCloseKey(hKey);
                }
            }
            free(blob); // Освобождаем выделенную через malloc память
        }
        Wh_DeleteValue(L"Backup"); // Удаляем бэкап из хранилища мода
    }
    
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

// Копирование параметров WindowMetrics из профиля пользователя в .DEFAULT
void CopyWindowMetricsFromSid(LPCWSTR szSid) {
    BackupDefaultMetrics(); // Гарантируем, что оригинальные значения сохранены перед первой перезаписью

    WCHAR szSrcPath[MAX_PATH];
    wsprintfW(szSrcPath, L"%s\\Control Panel\\Desktop\\WindowMetrics", szSid);
    LPCWSTR szDestPath = L".DEFAULT\\Control Panel\\Desktop\\WindowMetrics";

    HKEY hKeySrc = NULL;
    HKEY hKeyDest = NULL;

    if (RegOpenKeyExW(HKEY_USERS, szSrcPath, 0, KEY_READ, &hKeySrc) != ERROR_SUCCESS) {
        return;
    }

    if (RegCreateKeyExW(HKEY_USERS, szDestPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeyDest, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hKeySrc);
        return;
    }

    DWORD cValues = 0;
    DWORD cchMaxValueNameLen = 0;
    DWORD cbMaxValueLen = 0;

    if (RegQueryInfoKeyW(hKeySrc, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cchMaxValueNameLen, &cbMaxValueLen, NULL, NULL) == ERROR_SUCCESS && cValues > 0) {
        cchMaxValueNameLen += 2;
        cbMaxValueLen += 2;

        WCHAR* valueName = (WCHAR*)malloc(cchMaxValueNameLen * sizeof(WCHAR));
        BYTE* valueData = (BYTE*)malloc(cbMaxValueLen);

        if (valueName && valueData) {
            for (DWORD i = 0; i < cValues; i++) {
                DWORD cchName = cchMaxValueNameLen;
                DWORD cbData = cbMaxValueLen;
                DWORD dwType = 0;

                if (RegEnumValueW(hKeySrc, i, valueName, &cchName, NULL, &dwType, valueData, &cbData) == ERROR_SUCCESS) {
                    RegSetValueExW(hKeyDest, valueName, 0, dwType, valueData, cbData);
                }
            }
        }

        if (valueName) free(valueName);
        if (valueData) free(valueData);
    }

    RegCloseKey(hKeySrc);
    RegCloseKey(hKeyDest);
}

// Фоновый поток мониторинга изменений
DWORD WINAPI MetricsMonitorThread(LPVOID lpParam) {
    HANDLE hRegEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!hRegEvent) return 0;

    while (WaitForSingleObject(g_hStopEvent, 0) == WAIT_TIMEOUT) {
        LPWSTR szSid = NULL;
        
        if (GetActiveUserSidString(&szSid)) {
            WCHAR szSrcPath[MAX_PATH];
            wsprintfW(szSrcPath, L"%s\\Control Panel\\Desktop\\WindowMetrics", szSid);

            HKEY hKeySrc = NULL;
            if (RegOpenKeyExW(HKEY_USERS, szSrcPath, 0, KEY_READ | KEY_NOTIFY, &hKeySrc) == ERROR_SUCCESS) {
                // 1. Начальная синхронизация при обнаружении сессии
                CopyWindowMetricsFromSid(szSid);

                // 2. Ожидание изменений (смена темы / метрик)
                while (WaitForSingleObject(g_hStopEvent, 0) == WAIT_TIMEOUT) {
                    if (RegNotifyChangeKeyValue(hKeySrc, FALSE, REG_NOTIFY_CHANGE_LAST_SET, hRegEvent, TRUE) != ERROR_SUCCESS) {
                        break;
                    }

                    HANDLE handles[2] = { g_hStopEvent, hRegEvent };
                    DWORD dwWait = WaitForMultipleObjects(2, handles, FALSE, 1000); // Сокращенный таймаут до 1 секунды

                    if (dwWait == WAIT_OBJECT_0) {
                        break; // Выход из мода
                    } else if (dwWait == WAIT_OBJECT_0 + 1) {
                        // Реестр изменился (пользователь сменил тему / метрики)
                        CopyWindowMetricsFromSid(szSid);
                    } else if (dwWait == WAIT_TIMEOUT) {
                        // Проверяем, не сменился ли активный пользователь (в т.ч. при логауте, когда нет пользователя)
                        LPWSTR szCurrentSid = NULL;
                        BOOL bSame = FALSE;
                        if (GetActiveUserSidString(&szCurrentSid)) {
                            bSame = (lstrcmpiW(szSid, szCurrentSid) == 0);
                            LocalFree(szCurrentSid);
                        }
                        if (!bSame) break; // Пользователь сменился или вышел -> немедленно освобождаем дескриптор улья (hive)
                    }
                }
                RegCloseKey(hKeySrc);
            } else {
                WaitForSingleObject(g_hStopEvent, 2000);
            }
            LocalFree(szSid);
        } else {
            // Пользователь еще не вошел в систему
            WaitForSingleObject(g_hStopEvent, 2000);
        }
    }

    CloseHandle(hRegEvent);
    return 0;
}

BOOL Wh_ModInit() {
    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) {
        return FALSE;
    }

    g_hThread = CreateThread(NULL, 0, MetricsMonitorThread, NULL, 0, NULL);
    if (!g_hThread) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE); // INFINITE безопасен т.к. поток проверяет событие остановки
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }
    
    // Восстанавливаем оригинальные метрики при выгрузке
    RestoreDefaultMetrics();
}
