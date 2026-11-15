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
#include <stdlib.h>
#include <string.h>

HANDLE g_hStopEvent = NULL;
HANDLE g_hThread = NULL;

#define WINDOW_METRICS_DEFAULT_PATH L".DEFAULT\\Control Panel\\Desktop\\WindowMetrics"
#define BACKUP_MUTEX_NAME           L"Global\\ClassicThemeMetricsFix_DefaultBackupMutex"
#define BACKUP_BLOB_VALUE_NAME      L"DefaultWindowMetricsBackup"
#define BACKUP_SIZE_VALUE_NAME      L"DefaultWindowMetricsBackupSize"

#pragma pack(push, 1)
typedef struct {
    DWORD nameLenBytes; // including terminating null, in bytes
    DWORD type;
    DWORD dataLen;
} BackupEntryHeader;
#pragma pack(pop)

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

// Копирование параметров WindowMetrics из профиля пользователя в .DEFAULT
void CopyWindowMetricsFromSid(LPCWSTR szSid) {
    WCHAR szSrcPath[MAX_PATH];
    wsprintfW(szSrcPath, L"%s\\Control Panel\\Desktop\\WindowMetrics", szSid);
    LPCWSTR szDestPath = WINDOW_METRICS_DEFAULT_PATH;

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

// Сериализация текущих значений HKEY_USERS\.DEFAULT\...\WindowMetrics в бинарный блоб
BYTE* SerializeDefaultWindowMetrics(DWORD* pOutSize) {
    *pOutSize = 0;

    HKEY hKey = NULL;
    if (RegOpenKeyExW(HKEY_USERS, WINDOW_METRICS_DEFAULT_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return NULL;
    }

    DWORD cValues = 0, cchMaxName = 0, cbMaxData = 0;
    if (RegQueryInfoKeyW(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &cValues, &cchMaxName, &cbMaxData, NULL, NULL) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return NULL;
    }

    cchMaxName += 2;
    cbMaxData += 2;

    DWORD capacity = (DWORD)sizeof(DWORD) +
        cValues * ((DWORD)sizeof(BackupEntryHeader) + (cchMaxName * (DWORD)sizeof(WCHAR)) + cbMaxData);
    if (capacity < sizeof(DWORD)) capacity = sizeof(DWORD);

    BYTE* buffer = (BYTE*)malloc(capacity);
    if (!buffer) {
        RegCloseKey(hKey);
        return NULL;
    }

    DWORD offset = sizeof(DWORD); // место под счётчик
    DWORD actualCount = 0;

    WCHAR* valueName = (WCHAR*)malloc(cchMaxName * sizeof(WCHAR));
    BYTE* valueData = (BYTE*)malloc(cbMaxData);

    if (valueName && valueData) {
        for (DWORD i = 0; i < cValues; i++) {
            DWORD cchName = cchMaxName;
            DWORD cbData = cbMaxData;
            DWORD dwType = 0;

            if (RegEnumValueW(hKey, i, valueName, &cchName, NULL, &dwType, valueData, &cbData) == ERROR_SUCCESS) {
                DWORD nameLenBytes = (cchName + 1) * sizeof(WCHAR);

                BackupEntryHeader header;
                header.nameLenBytes = nameLenBytes;
                header.type = dwType;
                header.dataLen = cbData;

                memcpy(buffer + offset, &header, sizeof(header));
                offset += sizeof(header);
                memcpy(buffer + offset, valueName, nameLenBytes);
                offset += nameLenBytes;
                memcpy(buffer + offset, valueData, cbData);
                offset += cbData;

                actualCount++;
            }
        }
    }

    free(valueName);
    free(valueData);
    RegCloseKey(hKey);

    memcpy(buffer, &actualCount, sizeof(DWORD));

    *pOutSize = offset;
    return buffer;
}

// Восстановление значений из блоба обратно в HKEY_USERS\.DEFAULT\...\WindowMetrics
void RestoreDefaultWindowMetricsFromBlob(const BYTE* blob, DWORD size) {
    if (!blob || size < sizeof(DWORD)) return;

    HKEY hKey = NULL;
    if (RegCreateKeyExW(HKEY_USERS, WINDOW_METRICS_DEFAULT_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        return;
    }

    DWORD count = 0;
    memcpy(&count, blob, sizeof(DWORD));
    DWORD offset = sizeof(DWORD);

    for (DWORD i = 0; i < count && offset + sizeof(BackupEntryHeader) <= size; i++) {
        BackupEntryHeader header;
        memcpy(&header, blob + offset, sizeof(header));
        offset += sizeof(header);

        if ((unsigned long long)offset + header.nameLenBytes + header.dataLen > size) break;

        const WCHAR* name = (const WCHAR*)(blob + offset);
        offset += header.nameLenBytes;
        const BYTE* data = blob + offset;
        offset += header.dataLen;

        RegSetValueExW(hKey, name, 0, header.type, data, header.dataLen);
    }

    RegCloseKey(hKey);
}

// Одноразовый бэкап .DEFAULT, защищённый именованным мьютексом от гонки между
// несколькими копиями мода (по одному winlogon.exe на сессию)
void BackupDefaultIfNeeded() {
    HANDLE hMutex = CreateMutexW(NULL, FALSE, BACKUP_MUTEX_NAME);
    if (!hMutex) {
        return;
    }
    WaitForSingleObject(hMutex, INFINITE);

    int existingSize = Wh_GetIntValue(BACKUP_SIZE_VALUE_NAME, 0);
    if (existingSize <= 0) {
        DWORD blobSize = 0;
        BYTE* blob = SerializeDefaultWindowMetrics(&blobSize);
        if (blob && blobSize > 0) {
            if (Wh_SetBinaryValue(BACKUP_BLOB_VALUE_NAME, blob, blobSize)) {
                Wh_SetIntValue(BACKUP_SIZE_VALUE_NAME, (int)blobSize);
            }
        }
        if (blob) free(blob);
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

// Восстановление оригинальных значений .DEFAULT и удаление бэкапа
void RestoreDefaultBackup() {
    HANDLE hMutex = CreateMutexW(NULL, FALSE, BACKUP_MUTEX_NAME);
    if (!hMutex) {
        return;
    }
    WaitForSingleObject(hMutex, INFINITE);

    int size = Wh_GetIntValue(BACKUP_SIZE_VALUE_NAME, 0);
    if (size > 0) {
        BYTE* blob = (BYTE*)malloc((size_t)size);
        if (blob) {
            if (Wh_GetBinaryValue(BACKUP_BLOB_VALUE_NAME, blob, (size_t)size)) {
                RestoreDefaultWindowMetricsFromBlob(blob, (DWORD)size);
            }
            free(blob);
        }
        Wh_DeleteValue(BACKUP_BLOB_VALUE_NAME);
        Wh_DeleteValue(BACKUP_SIZE_VALUE_NAME);
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
}

// Фоновый поток мониторинга изменений
DWORD WINAPI MetricsMonitorThread(LPVOID lpParam) {
    HANDLE hRegEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!hRegEvent) return 0;

    // Бэкапим оригинальные значения .DEFAULT до первой перезаписи
    BackupDefaultIfNeeded();

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
                    // Короткий таймаут, чтобы быстро заметить выход пользователя
                    // и отпустить куст реестра
                    DWORD dwWait = WaitForMultipleObjects(2, handles, FALSE, 1000);

                    if (dwWait == WAIT_OBJECT_0) {
                        break; // Выход из мода
                    } else if (dwWait == WAIT_OBJECT_0 + 1) {
                        // Реестр изменился (пользователь сменил тему / метрики)
                        CopyWindowMetricsFromSid(szSid);
                    } else {
                        // Проверяем, не сменился ли активный пользователь
                        // (в том числе случай выхода из системы — нет активного
                        // пользователя считаем как "сменился")
                        LPWSTR szCurrentSid = NULL;
                        BOOL bSame = FALSE;
                        if (GetActiveUserSidString(&szCurrentSid)) {
                            bSame = (lstrcmpiW(szSid, szCurrentSid) == 0);
                            LocalFree(szCurrentSid);
                        }
                        if (!bSame) break; // пользователь сменился или вышел -> отпускаем куст
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
        // Все ожидания в потоке следят за g_hStopEvent, поэтому
        // бесконечное ожидание безопасно и не приведёт к выгрузке DLL
        // из-под ещё работающего потока в критическом процессе.
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    // Восстанавливаем оригинальные значения .DEFAULT (вступит в силу
    // при следующем входе в систему) и удаляем бэкап
    RestoreDefaultBackup();
}
