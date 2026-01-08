// ==WindhawkMod==
// @id              taskbar-app-volume
// @name            Taskbar APP Volume Control
// @description     Control the system volume and apps volume (SOME APPS) by scrolling .
// @version         1.0
// @author          tz39
// @github          https://github.com/tz39
// @include         explorer.exe
// @compilerOptions -DWINVER=0x0A00 -lcomctl32 -ldwmapi -lgdi32 -lole32 -lversion -luiautomationcore -loleaut32 -luuid
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*Control the system volume and apps volume (SOME APPS) by scrolling over the taskbar for system volume and at icon to change the app volume, Optimized for Windows 10 and 11 taskbars. Supports popular apps like Chrome, Firefox, Spotify, Discord,...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// - name: scrollArea
//   description: Scroll area
//   type: string
//   defaultValue: taskbar
//   options:
//     - label: Taskbar
//       value: taskbar
//     - label: Notification area
//       value: notification_area
//     - label: Taskbar without notification area
//       value: taskbarWithoutNotificationArea
// - name: middleClickToMute
//   description: Middle click on the icon to mute
//   type: boolean
//   defaultValue: true
// - name: ctrlScrollVolumeChange
//   description: Change volume with Ctrl+Scroll
//   type: boolean
//   defaultValue: true
// - name: noAutomaticMuteToggle
//   description: Don't toggle mute automatically
//   type: boolean
//   defaultValue: false
// - name: volumeChangeStep
//   description: Volume change step
//   type: integer
//   defaultValue: 2
// - name: oldTaskbarOnWin11
//   description: Optimize for the old taskbar on Windows 11
//   type: boolean
//   defaultValue: false
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <audiopolicy.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <psapi.h>
#include <uiautomation.h>
#include <windowsx.h>
#include <string>
#include <cwctype>
#include <algorithm>

#include <atomic>
#include <unordered_set>
#include <vector>

enum class VolumeIndicator {
    None,
    Classic,
    Modern,
    Win11,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
};

struct {
    VolumeIndicator volumeIndicator;
    ScrollArea scrollArea;
    bool middleClickToMute;
    bool ctrlScrollVolumeChange;
    bool noAutomaticMuteToggle;
    int volumeChangeStep;
    bool oldTaskbarOnWin11;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
bool g_inputSiteProcHooked;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

// --- App Mapping & Optimization ---

struct AppMapping {
    const wchar_t* keyword;
    const wchar_t* exeName;
    bool isPartial;
};

const AppMapping g_appMappings[] = {
    // Browsers
    { L"Firefox", L"firefox", true },
    { L"Chrome", L"chrome", true },
    { L"Edge", L"msedge", true },
    { L"Brave", L"brave", true },
    { L"Opera", L"opera", true },
    { L"Vivaldi", L"vivaldi", true },
    
    // Media
    { L"Spotify", L"Spotify", true },
    { L"Netflix", L"Netflix", true },
    { L"iTunes", L"iTunes", true },
    { L"VLC", L"vlc", true },
    
    // Social
    { L"Discord", L"Discord", true }, // Fuzzy match handles DiscordCanary, etc.
    { L"WhatsApp", L"WhatsApp", true },
    { L"Telegram", L"Telegram", true },
    { L"Unigram", L"Unigram", true },
    { L"Teams", L"Teams", true },
    
    // Gaming
    { L"Steam", L"steam", true },
    { L"Epic Games", L"EpicGamesLauncher", true },
    { L"Minecraft", L"Minecraft.Windows", true },
    { L"Roblox", L"RobloxPlayerBeta", true }
};

struct AudioSessionCache {
    DWORD pid;
    std::vector<ISimpleAudioVolume*> pVolumes;
    DWORD lastUsedTick;
};

// Global cache object
static AudioSessionCache g_audioCache = { 0, {}, 0 };

void ClearAudioCache() {
    for (auto* pVol : g_audioCache.pVolumes) {
        if (pVol) pVol->Release();
    }
    g_audioCache.pVolumes.clear();
    g_audioCache.pid = 0;
}

// PID Cache
#include <unordered_map>
static std::unordered_map<std::wstring, DWORD> g_pidCache;

void ClearPidCache() {
    g_pidCache.clear();
}

// Simple case-insensitive substring helper
bool HasSubstringCaseInsensitive(const std::wstring& str, const std::wstring& sub) {
    if (sub.empty()) return true;
    if (str.length() < sub.length()) return false;
    
    auto it = std::search(
        str.begin(), str.end(),
        sub.begin(), sub.end(),
        [](wchar_t c1, wchar_t c2) {
            return std::towlower(c1) == std::towlower(c2);
        }
    );
    return it != str.end();
}

// Universal Smart Match Helpers
std::wstring NormalizeString(const std::wstring& input) {
    std::wstring result;
    std::wstring strLower = input;
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), towlower);

    // Strip .exe suffix if present
    size_t exePos = strLower.rfind(L".exe");
    if (exePos != std::wstring::npos && exePos == strLower.length() - 4) {
        strLower = strLower.substr(0, exePos);
    }
    
    result.reserve(strLower.length());
    for (wchar_t c : strLower) {
        // Keep only alphanumeric characters
        if (iswalnum(c)) {
            result.push_back(c);
        }
    }
    return result;
}

bool SmartFuzzyMatch(const std::wstring& sessionName, const std::wstring& targetName) {
    if (targetName.empty()) return false;
    std::wstring nSession = NormalizeString(sessionName);
    std::wstring nTarget = NormalizeString(targetName);
    
    // Bidirectional Containment Check
    // "vlcmediaplayer" contains "vlc" -> Match
    // "rocketleague" contains "rocketleague" -> Match
    if (nSession.empty() || nTarget.empty()) return false;
    
    return (nSession.find(nTarget) != std::wstring::npos) || 
           (nTarget.find(nSession) != std::wstring::npos);
}
// ----------------------------------

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_7,
    WIN_VERSION_8,
    WIN_VERSION_81,
    WIN_VERSION_811,
    WIN_VERSION_10_T1,        // 1507
    WIN_VERSION_10_T2,        // 1511
    WIN_VERSION_10_R1,        // 1607
    WIN_VERSION_10_R2,        // 1703
    WIN_VERSION_10_R3,        // 1709
    WIN_VERSION_10_R4,        // 1803
    WIN_VERSION_10_R5,        // 1809
    WIN_VERSION_10_19H1,      // 1903, 1909
    WIN_VERSION_10_20H1,      // 2004, 20H2, 21H1, 21H2
    WIN_VERSION_SERVER_2022,  // Server 2022
    WIN_VERSION_11_21H2,
    WIN_VERSION_11_22H2,
};

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

static int g_nWinVersion;
static int g_nExplorerVersion;
static HWND g_hTaskbarWnd;
static DWORD g_dwTaskbarThreadId;

IUIAutomation* g_pAutomation = nullptr;

struct WindowSearchData {
    std::wstring targetName;
    DWORD foundPid;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    WindowSearchData* data = (WindowSearchData*)lParam;
    
    if (!IsWindowVisible(hwnd)) return TRUE;

    WCHAR szTitle[256];
    GetWindowText(hwnd, szTitle, sizeof(szTitle) / sizeof(WCHAR));
    
    if (wcslen(szTitle) == 0) return TRUE;

    std::wstring windowTitle(szTitle);
    
    // Check for exact match
    if (_wcsicmp(windowTitle.c_str(), data->targetName.c_str()) == 0) {
        GetWindowThreadProcessId(hwnd, &data->foundPid);
        return FALSE; // Stop enumeration
    }
    
    // Check if Window Title contains the Button Name (e.g. "YouTube - Chrome" contains "Chrome" logic? No, usually "Chrome" is Button Name)
    // Actually, on Win11 grouped, Button Name = "Google Chrome". Window Title = "Tab - Google Chrome".
    // So Window Title contains Button Name.
    if (windowTitle.find(data->targetName) != std::wstring::npos) {
         GetWindowThreadProcessId(hwnd, &data->foundPid);
         // Don't stop, keep looking for exact match? No, good enough for now.
         return FALSE; 
    }

    return TRUE;
}

std::wstring GetProcessName(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess) {
        WCHAR szProcessName[MAX_PATH];
        DWORD dwSize = MAX_PATH;
        if (QueryFullProcessImageName(hProcess, 0, szProcessName, &dwSize)) {
            CloseHandle(hProcess);
            // Extract just the file name from full path
            std::wstring fullPath(szProcessName);
            size_t lastSlash = fullPath.find_last_of(L"\\");
            if (lastSlash != std::wstring::npos) {
                return fullPath.substr(lastSlash + 1);
            }
            return fullPath;
        }
        CloseHandle(hProcess);
    }
    return L"";
}

BOOL SetAppVolume(DWORD pid, int nDelta, const std::wstring& processNameOverride = L"", bool isPartialMatch = false, bool useSmartMatch = false) {
    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioSessionManager2* pSessionManager = NULL;
    IAudioSessionEnumerator* pSessionEnumerator = NULL;
    int sessionCount = 0;
    BOOL bFound = FALSE;

    std::wstring targetProcessName;
    if (!processNameOverride.empty()) {
        targetProcessName = processNameOverride;
    } else {
        targetProcessName = GetProcessName(pid);
        if (targetProcessName.empty()) {
             // Wh_Log(L"Could not get process name for PID: %d", pid);
             return FALSE;
        }
    }

    // --- Optimization: Check Cache ---
    if (g_audioCache.pid == pid && !g_audioCache.pVolumes.empty() && processNameOverride.empty()) {
        BOOL anySuccess = FALSE;
        for (auto* pVol : g_audioCache.pVolumes) {
            float level = 0.0f;
            if (SUCCEEDED(pVol->GetMasterVolume(&level))) {
                 float step = g_settings.volumeChangeStep ? (float)g_settings.volumeChangeStep / 100.0f : 0.02f;
                 float change = (nDelta > 0 ? step : -step);
                 level += change;
                 if (level < 0.0f) level = 0.0f;
                 if (level > 1.0f) level = 1.0f;
                 
                 if (SUCCEEDED(pVol->SetMasterVolume(level, NULL))) {
                     anySuccess = TRUE;
                 }
            }
        }

        if (anySuccess) {
             g_audioCache.lastUsedTick = GetTickCount();
             Wh_Log(L"CACHE HIT. PID: %d", g_audioCache.pid);
             return TRUE;
        }
        
        // If all failed, clear cache.
        ClearAudioCache();
    }
    // --------------------------------

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return FALSE;

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) { pEnumerator->Release(); return FALSE; }

    hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pSessionManager);
    if (FAILED(hr)) { pDevice->Release(); pEnumerator->Release(); return FALSE; }

    hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
    if (FAILED(hr)) { pSessionManager->Release(); pDevice->Release(); pEnumerator->Release(); return FALSE; }

    hr = pSessionEnumerator->GetCount(&sessionCount);
    
    for (int i = 0; i < sessionCount; i++) {
        IAudioSessionControl* pSessionControl = NULL;
        IAudioSessionControl2* pSessionControl2 = NULL;
        
        if (SUCCEEDED(pSessionEnumerator->GetSession(i, &pSessionControl))) {
            if (SUCCEEDED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2))) {
                DWORD sessionPid = 0;
                if (SUCCEEDED(pSessionControl2->GetProcessId(&sessionPid))) {
                     std::wstring sessionProcessName = GetProcessName(sessionPid);
                     
                     // Check if PID matches OR if Process Name matches (handles Chrome/Spotify helper processes)
                     // If isPartialMatch is true, we check if sessionProcessName contains targetProcessName
                     // If useSmartMatch is true, we use the aggressive normalization check.
                     // Otherwise exact match.
                     bool nameMatch = false;
                     if (useSmartMatch) {
                         nameMatch = SmartFuzzyMatch(sessionProcessName, targetProcessName);
                     } else if (isPartialMatch) {
                         nameMatch = HasSubstringCaseInsensitive(sessionProcessName, targetProcessName);
                     } else {
                         nameMatch = (_wcsicmp(targetProcessName.c_str(), sessionProcessName.c_str()) == 0);
                     }

                     if (sessionPid == pid || nameMatch) {
                          // Wh_Log(L"FOUND MATCH: %s (PID: %d) for Target: %s", sessionProcessName.c_str(), sessionPid, targetProcessName.c_str());
                          
                          ISimpleAudioVolume* pVol = NULL;
                          if (SUCCEEDED(pSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pVol))) {
                              float level = 0.0f;
                              if (SUCCEEDED(pVol->GetMasterVolume(&level))) {
                                   float step = g_settings.volumeChangeStep ? (float)g_settings.volumeChangeStep / 100.0f : 0.02f;
                                   float change = (nDelta > 0 ? step : -step);
                                   level += change;
                                   if (level < 0.0f) level = 0.0f;
                                   if (level > 1.0f) level = 1.0f;
                                   
                                   if (SUCCEEDED(pVol->SetMasterVolume(level, NULL))) {
                                       Wh_Log(L"Adjusted Volume for: %s (PID: %d)", sessionProcessName.c_str(), sessionPid);
                                       bFound = TRUE;
                                       
                                       // --- Update Cache ---
                                       if (processNameOverride.empty()) { // Only cache pure PID lookups
                                           if (g_audioCache.pid != pid) {
                                               ClearAudioCache();
                                               g_audioCache.pid = pid;
                                           }
                                           pVol->AddRef();
                                           g_audioCache.pVolumes.push_back(pVol);
                                           g_audioCache.lastUsedTick = GetTickCount();
                                       }
                                       // --------------------
                                       // Do NOT release pVol, it is stored in cache
                                   } else {
                                       pVol->Release();
                                   }
                              } else {
                                  pVol->Release();
                              }
                          }
                     }
                }
                pSessionControl2->Release();
            }
            pSessionControl->Release();
        }
        // REMOVED break; to allow finding multiple sessions (e.g. Discord Voice + Sounds)
        // if (bFound) break; 
    }

    pSessionEnumerator->Release();
    pSessionManager->Release();
    pDevice->Release();
    pEnumerator->Release();
    
    return bFound;
}

DWORD GetPidFromTaskbarPoint(POINT pt, BOOL* pIsItem, std::wstring* pNameOut) {
    *pIsItem = FALSE;
    if (!g_pAutomation) {
        CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&g_pAutomation);
    }
    if (!g_pAutomation) return 0;

    IUIAutomationElement* pElement = nullptr;
    if (FAILED(g_pAutomation->ElementFromPoint(pt, &pElement)) || !pElement) return 0;

    CONTROLTYPEID controlType = 0;
    pElement->get_CurrentControlType(&controlType);
    
    // Wh_Log(L"Element found. ControlType: %d", controlType);

    // Check if it's a button (50000) or list item (50007) - typical for taskbar apps
    // Also Header (50034) or Pane (50033) sometimes wrap the icon in Win11?
    if (controlType == 50000 || controlType == 50007) {
        *pIsItem = TRUE;
    }

    DWORD targetPid = 0;
    UIA_HWND uiaHwnd = NULL;
    
    // Try to get the window handle directly from the element
    if (SUCCEEDED(pElement->get_CurrentNativeWindowHandle(&uiaHwnd)) && uiaHwnd) {
        GetWindowThreadProcessId((HWND)uiaHwnd, &targetPid);
        
        // If the PID belongs to Explorer (which owns the taskbar), it's not the target app.
        // We should ignore it and fallback to name matching.
        DWORD explorerPid = 0;
        GetWindowThreadProcessId(g_hTaskbarWnd, &explorerPid);
        if (targetPid == explorerPid) {
             targetPid = 0;
        } else if (targetPid) {
             // If we found a PID (that isn't explorer), it's definitely an item
             *pIsItem = TRUE;
        }
    }

    // Fallback to name matching if PID not found (or was Explorer)
    if (!targetPid && pElement) {
        BSTR nameBstr = nullptr;
        pElement->get_CurrentName(&nameBstr);
        
        // Fallback: Try LegacyIAccessible Pattern if Name is empty
        if (!nameBstr || wcslen(nameBstr) == 0) {
            if (nameBstr) SysFreeString(nameBstr);
            nameBstr = nullptr;
            
            IUIAutomationLegacyIAccessiblePattern* pLegacy = nullptr;
            if (SUCCEEDED(pElement->GetCurrentPattern(UIA_LegacyIAccessiblePatternId, (IUnknown**)&pLegacy)) && pLegacy) {
                pLegacy->get_CurrentName(&nameBstr);
                pLegacy->Release();
            }
        }

        if (nameBstr) {
            std::wstring foundName(nameBstr);
            if (!foundName.empty()) {
                *pIsItem = TRUE; // If it has a name, treat it as an item
                if (pNameOut) *pNameOut = foundName;
            }
            
            // --- PID Cache Check ---
            // If we have seen this name ("Spotify", "Chrome") before, allow fast lookup.
            if (g_pidCache.find(foundName) != g_pidCache.end()) {
                DWORD cachedPid = g_pidCache[foundName];
                // Verify PID is still alive to avoid recycling issues (basic check)
                HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, cachedPid);
                if (hProc) {
                    CloseHandle(hProc);
                    targetPid = cachedPid;
                    // Wh_Log(L"PID Cache Hit: %s -> %d", foundName.c_str(), targetPid);
                } else {
                    // Invalid PID, remove from cache
                    g_pidCache.erase(foundName);
                }
            }
            // -----------------------

            if (!targetPid) {
                // Use fuzzy search logic
                WindowSearchData searchData;
                searchData.targetName = foundName;
                searchData.foundPid = 0;
                
                EnumWindows(EnumWindowsProc, (LPARAM)&searchData);
                
                // If failed, try to "clean" the name. 
                // Taskbar sometimes says "Brave - 1 running window pinned". 
                // We want just "Brave".
                if (!searchData.foundPid) {
                    std::wstring fullName(nameBstr);
                    size_t dashPos = fullName.find(L" - ");
                    if (dashPos != std::wstring::npos) {
                        std::wstring cleanedName = fullName.substr(0, dashPos);
                        searchData.targetName = cleanedName;
                        EnumWindows(EnumWindowsProc, (LPARAM)&searchData);
                        
                        if (pNameOut) *pNameOut = cleanedName; // Return the cleaned name if we used it
                        
                        // If clean name worked, maybe cache that? 
                        // Complex because key was foundName. 
                        // Let's just update the cache with the ORIGINAL key mapping to the FOUND pid.
                    }
                }
                
                if (searchData.foundPid) {
                    targetPid = searchData.foundPid;
                    // Update Cache
                    g_pidCache[foundName] = targetPid;
                }
            }
            SysFreeString(nameBstr);
        } else {
            // Name detection failed. Check ClassName to see if it's an Item anyway.
            // This prevents Global Volume fallthrough for unidentified items.
            BSTR classBstr;
            if (SUCCEEDED(pElement->get_CurrentClassName(&classBstr)) && classBstr) {
                // Wh_Log(L"Item ClassName: %s", classBstr);
                if (wcscmp(classBstr, L"Taskbar.TaskListButton") == 0) {
                    *pIsItem = TRUE;
                }
                SysFreeString(classBstr);
            }
        }
    }
    
    pElement->Release();
    return targetPid;
}

#pragma region functions

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    int iDpi = 96;
    if (pGetDpiForWindow) {
        iDpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }

    return iDpi;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (!hTrayNotifyWnd) {
            return false;
        }

        return GetWindowRect(hTrayNotifyWnd, rcResult);
    }

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            return false;
        }

        HWND hBridgeWnd = FindWindowEx(
            hMMTaskbarWnd, NULL,
            L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
        while (hBridgeWnd) {
            RECT rcBridge;
            if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                break;
            }

            if (rcBridge.left != rcTaskbar.left ||
                rcBridge.top != rcTaskbar.top ||
                rcBridge.right != rcTaskbar.right ||
                rcBridge.bottom != rcTaskbar.bottom) {
                CopyRect(rcResult, &rcBridge);
                return true;
            }

            hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, hBridgeWnd,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
        }

        // On newer Win11 versions, the clock on secondary taskbars is difficult
        // to detect without either UI Automation or UWP UI APIs. Just consider
        // the last pixels, not accurate, but better than nothing.
        int lastPixels =
            MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);
        CopyRect(rcResult, &rcTaskbar);
        if (rcResult->right - rcResult->left > lastPixels) {
            if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
                rcResult->right = rcResult->left + lastPixels;
            } else {
                rcResult->left = rcResult->right - lastPixels;
            }
        }

        return true;
    }

    if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (!hClockButtonWnd) {
            return false;
        }

        return GetWindowRect(hClockButtonWnd, rcResult);
    }

    SetRectEmpty(rcResult);
    return true;
}

bool IsPointInsideScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    // Optimization: If the window passed IS the taskbar, check it directly. 
    // But for robustness, we always check against the known global taskbar handle.
    // This allows child windows (like the Comet button) to pass the check even if they haven't been passed in as the root.
    
    RECT rcTaskbar;
    if (GetWindowRect(g_hTaskbarWnd, &rcTaskbar) && PtInRect(&rcTaskbar, pt)) {
         return true;
    }

    // TODO: support secondary taskbars if needed, iterating g_secondaryTaskbarWindows
    // For now, checking the primary global taskbar handle should fix the Comet issue 
    // if it lives on the main bar.
    
    return false;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL) {
            pData = LockResource(hGlobal);
            if (pData != NULL) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
        return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

    switch (nMajor) {
        case 6:
            switch (nMinor) {
                case 1:
                    g_nWinVersion = WIN_VERSION_7;
                    break;

                case 2:
                    g_nWinVersion = WIN_VERSION_8;
                    break;

                case 3:
                    if (nQFE < 17000)
                        g_nWinVersion = WIN_VERSION_81;
                    else
                        g_nWinVersion = WIN_VERSION_811;
                    break;

                case 4:
                    g_nWinVersion = WIN_VERSION_10_T1;
                    break;
            }
            break;

        case 10:
            if (nBuild <= 10240)
                g_nWinVersion = WIN_VERSION_10_T1;
            else if (nBuild <= 10586)
                g_nWinVersion = WIN_VERSION_10_T2;
            else if (nBuild <= 14393)
                g_nWinVersion = WIN_VERSION_10_R1;
            else if (nBuild <= 15063)
                g_nWinVersion = WIN_VERSION_10_R2;
            else if (nBuild <= 16299)
                g_nWinVersion = WIN_VERSION_10_R3;
            else if (nBuild <= 17134)
                g_nWinVersion = WIN_VERSION_10_R4;
            else if (nBuild <= 17763)
                g_nWinVersion = WIN_VERSION_10_R5;
            else if (nBuild <= 18362)
                g_nWinVersion = WIN_VERSION_10_19H1;
            else if (nBuild <= 19041)
                g_nWinVersion = WIN_VERSION_10_20H1;
            else if (nBuild <= 20348)
                g_nWinVersion = WIN_VERSION_SERVER_2022;
            else if (nBuild <= 22000)
                g_nWinVersion = WIN_VERSION_11_21H2;
            else
                g_nWinVersion = WIN_VERSION_11_22H2;
            break;
    }

    if (g_nWinVersion == WIN_VERSION_UNSUPPORTED)
        return FALSE;

    return TRUE;
}

#pragma endregion  // functions

#pragma region volume_functions

const static GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const static GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const static GUID XIID_IAudioEndpointVolume = {
    0x5CDF2C82,
    0x841E,
    0x4546,
    {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

static IMMDeviceEnumerator* g_pDeviceEnumerator;

BOOL IsDefaultAudioEndpointAvailable() {
    IMMDevice* defaultDevice = NULL;
    HRESULT hr;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            bSuccess = TRUE;

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL IsVolMuted(BOOL* pbMuted) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMute(pbMuted)))
                    bSuccess = TRUE;

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL ToggleVolMuted() {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    BOOL bMuted;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMute(&bMuted))) {
                    if (SUCCEEDED(endpointVolume->SetMute(!bMuted, NULL)))
                        bSuccess = TRUE;
                }

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

BOOL AddMasterVolumeLevelScalar(float fMasterVolumeAdd) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    float fMasterVolume;
    BOOL bSuccess = FALSE;

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(
                        &fMasterVolume))) {
                    fMasterVolume += fMasterVolumeAdd;

                    if (fMasterVolume < 0.0)
                        fMasterVolume = 0.0;
                    else if (fMasterVolume > 1.0)
                        fMasterVolume = 1.0;

                    if (SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(
                            fMasterVolume, NULL))) {
                        bSuccess = TRUE;

                        if (!g_settings.noAutomaticMuteToggle) {
                            // Windows displays the volume rounded to the
                            // nearest percentage. The range [0, 0.005) is
                            // displayed as 0%, [0.005, 0.015) as 1%, etc. It
                            // also mutes the volume when it becomes zero, we do
                            // the same.

                            if (fMasterVolume < 0.005)
                                endpointVolume->SetMute(TRUE, NULL);
                            else
                                endpointVolume->SetMute(FALSE, NULL);
                        }
                    }
                }

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

void SndVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr))
        g_pDeviceEnumerator = NULL;
}

void SndVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

#pragma endregion  // volume_functions

#pragma region sndvol_win11

UINT g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
DWORD g_lastScrollTime;
short g_lastScrollDeltaRemainder;

bool PostAppCommand(SHORT appCommand, int count) {
    if (!g_hTaskbarWnd) {
        return false;
    }

    HWND hReBarWindow32 =
        FindWindowEx(g_hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return false;
    }

    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return false;
    }

    for (int i = 0; i < count; i++) {
        PostMessage(hMSTaskSwWClass, g_uShellHookMsg, HSHELL_APPCOMMAND,
                    MAKELPARAM(0, appCommand));
    }

    return true;
}

bool Win11IndicatorAdjustVolumeLevelWithMouseWheel(short delta) {
    BOOL muted;
    if (g_settings.noAutomaticMuteToggle && IsVolMuted(&muted) && muted) {
        return true;
    }

    if (GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    SHORT appCommand = APPCOMMAND_VOLUME_UP;
    if (clicks < 0) {
        clicks = -clicks;
        appCommand = APPCOMMAND_VOLUME_DOWN;
    }

    if (g_settings.volumeChangeStep) {
        clicks *= g_settings.volumeChangeStep / 2;
    }

    if (!PostAppCommand(appCommand, clicks)) {
        return false;
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return true;
}

#pragma endregion  // sndvol_win11

#pragma region sndvol

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam);
BOOL ScrollSndVol(WPARAM wParam, LPARAM lMousePosParam);
void SetSndVolTimer();
void KillSndVolTimer();
void CleanupSndVol();

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta, int nStep);
static BOOL OpenScrollSndVolInternal(WPARAM wParam,
                                     LPARAM lMousePosParam,
                                     HWND hVolumeAppWnd,
                                     BOOL* pbOpened);
static BOOL ValidateSndVolProcess();
static BOOL ValidateSndVolWnd();
static void CALLBACK CloseSndVolTimerProc(HWND hWnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime);
static HWND GetSndVolDlg(HWND hVolumeAppWnd);
static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam);
static BOOL IsSndVolWndInitialized(HWND hWnd);
static BOOL MoveSndVolCenterMouse(HWND hWnd);

// Modern indicator functions
static BOOL CanUseModernIndicator();
static BOOL ShowSndVolModernIndicator();
static BOOL HideSndVolModernIndicator();
static void EndSndVolModernIndicatorSession();
static HWND GetOpenSndVolModernIndicatorWnd();
static HWND GetSndVolTrayControlWnd();
static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd,
                                                        LPARAM lParam);

static HANDLE hSndVolProcess;
static HWND hSndVolWnd;
static UINT_PTR nCloseSndVolTimer;
static int nCloseSndVolTimerCount;
static HWND hSndVolModernPreviousForegroundWnd;
static BOOL bSndVolModernLaunched;
static BOOL bSndVolModernAppeared;

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam) {
    HANDLE hMutex;
    HWND hVolumeAppWnd;
    DWORD dwProcessId;
    WCHAR szCommandLine[sizeof("SndVol.exe -f 4294967295")];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if (g_settings.volumeIndicator == VolumeIndicator::Win11 &&
        g_nWinVersion >= WIN_VERSION_11_22H2) {
        return Win11IndicatorAdjustVolumeLevelWithMouseWheel(
            GET_WHEEL_DELTA_WPARAM(wParam));
    }

    if (g_settings.volumeIndicator == VolumeIndicator::None) {
        return AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam),
                                               0);
    }

    if (CanUseModernIndicator()) {
        if (!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), 2))
            return FALSE;

        ShowSndVolModernIndicator();
        SetSndVolTimer();
        return TRUE;
    }

    if (!IsDefaultAudioEndpointAvailable())
        return FALSE;

    if (ValidateSndVolProcess()) {
        if (WaitForInputIdle(hSndVolProcess, 0) == 0)  // If not initializing
        {
            if (ValidateSndVolWnd()) {
                ScrollSndVol(wParam, lMousePosParam);

                return FALSE;  // False because we didn't open it, it was open
            } else {
                hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                           L"Windows Volume App Window");
                if (hVolumeAppWnd) {
                    GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

                    if (GetProcessId(hSndVolProcess) == dwProcessId) {
                        BOOL bOpened;
                        if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                     hVolumeAppWnd, &bOpened)) {
                            if (bOpened)
                                SetSndVolTimer();

                            return bOpened;
                        }
                    }
                }
            }
        }

        return FALSE;
    }

    hMutex = OpenMutex(SYNCHRONIZE, FALSE, L"Windows Volume App Window");
    if (hMutex) {
        CloseHandle(hMutex);

        hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                   L"Windows Volume App Window");
        if (hVolumeAppWnd) {
            GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);

            hSndVolProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                            FALSE, dwProcessId);
            if (hSndVolProcess) {
                if (WaitForInputIdle(hSndVolProcess, 0) ==
                    0)  // if not initializing
                {
                    if (ValidateSndVolWnd()) {
                        ScrollSndVol(wParam, lMousePosParam);

                        return FALSE;  // False because we didn't open it, it
                                       // was open
                    } else {
                        BOOL bOpened;
                        if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                     hVolumeAppWnd, &bOpened)) {
                            if (bOpened)
                                SetSndVolTimer();

                            return bOpened;
                        }
                    }
                }
            }
        }

        return FALSE;
    }

    wsprintf(szCommandLine, L"SndVol.exe -f %u", (DWORD)lMousePosParam);

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE,
                       ABOVE_NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL,
                       NULL, &si, &pi))
        return FALSE;

    if (g_nExplorerVersion <= WIN_VERSION_7)
        SendMessage(g_hTaskbarWnd, WM_USER + 12, 0, 0);  // Close start menu

    AllowSetForegroundWindow(pi.dwProcessId);
    ResumeThread(pi.hThread);

    CloseHandle(pi.hThread);
    hSndVolProcess = pi.hProcess;

    SetSndVolTimer();

    return TRUE;
}

BOOL ScrollSndVol(WPARAM wParam, LPARAM lMousePosParam) {
    GUITHREADINFO guithreadinfo;

    guithreadinfo.cbSize = sizeof(GUITHREADINFO);

    if (!GetGUIThreadInfo(GetWindowThreadProcessId(hSndVolWnd, NULL),
                          &guithreadinfo))
        return FALSE;

    PostMessage(guithreadinfo.hwndFocus, WM_MOUSEWHEEL, wParam, lMousePosParam);
    return TRUE;
}

void SetSndVolTimer() {
    nCloseSndVolTimer =
        SetTimer(NULL, nCloseSndVolTimer, 100, CloseSndVolTimerProc);
    nCloseSndVolTimerCount = 0;
}

void KillSndVolTimer() {
    if (nCloseSndVolTimer != 0) {
        KillTimer(NULL, nCloseSndVolTimer);
        nCloseSndVolTimer = 0;
    }
}

void CleanupSndVol() {
    KillSndVolTimer();

    if (hSndVolProcess) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;
    }
}

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta, int nStep) {
    if (!nStep) {
        nStep = g_settings.volumeChangeStep;
        if (!nStep)
            nStep = 2;
    }

    return AddMasterVolumeLevelScalar((float)nWheelDelta * nStep *
                                      ((float)0.01 / 120));
}

static BOOL OpenScrollSndVolInternal(WPARAM wParam,
                                     LPARAM lMousePosParam,
                                     HWND hVolumeAppWnd,
                                     BOOL* pbOpened) {
    HWND hSndVolDlg = GetSndVolDlg(hVolumeAppWnd);
    if (hSndVolDlg) {
        if (GetWindowTextLength(hSndVolDlg) == 0)  // Volume control
        {
            if (IsSndVolWndInitialized(hSndVolDlg) &&
                MoveSndVolCenterMouse(hSndVolDlg)) {
                if (g_nExplorerVersion <= WIN_VERSION_7)
                    SendMessage(g_hTaskbarWnd, WM_USER + 12, 0,
                                0);  // Close start menu

                SetForegroundWindow(hVolumeAppWnd);
                PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

                *pbOpened = TRUE;
                return TRUE;
            }
        } else if (IsWindowVisible(
                       hSndVolDlg))  // Another dialog, e.g. volume mixer
        {
            if (g_nExplorerVersion <= WIN_VERSION_7)
                SendMessage(g_hTaskbarWnd, WM_USER + 12, 0,
                            0);  // Close start menu

            SetForegroundWindow(hVolumeAppWnd);
            PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);

            *pbOpened = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

static BOOL ValidateSndVolProcess() {
    if (!hSndVolProcess)
        return FALSE;

    if (WaitForSingleObject(hSndVolProcess, 0) != WAIT_TIMEOUT) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;

        return FALSE;
    }

    return TRUE;
}

static BOOL ValidateSndVolWnd() {
    HWND hForegroundWnd;
    DWORD dwProcessId;
    WCHAR szClass[sizeof("#32770") + 1];

    hForegroundWnd = GetForegroundWindow();

    if (hSndVolWnd == hForegroundWnd)
        return TRUE;

    GetWindowThreadProcessId(hForegroundWnd, &dwProcessId);

    if (GetProcessId(hSndVolProcess) == dwProcessId) {
        GetClassName(hForegroundWnd, szClass, sizeof("#32770") + 1);

        if (lstrcmp(szClass, L"#32770") == 0) {
            hSndVolWnd = hForegroundWnd;

            return TRUE;
        }
    }

    hSndVolWnd = NULL;

    return FALSE;
}

static void CALLBACK CloseSndVolTimerProc(HWND hWnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime) {
    if (CanUseModernIndicator()) {
        HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
        if (!bSndVolModernAppeared) {
            if (hSndVolModernIndicatorWnd) {
                bSndVolModernAppeared = TRUE;
                nCloseSndVolTimerCount = 1;

                // Windows 11 shows an ugly focus border. Make it go away by
                // making the window think it becomes inactive.
                if (g_nWinVersion >= WIN_VERSION_11_21H2) {
                    PostMessage(hSndVolModernIndicatorWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), 0);
                }

                return;
            } else {
                nCloseSndVolTimerCount++;
                if (nCloseSndVolTimerCount < 10)
                    return;
            }
        } else {
            if (hSndVolModernIndicatorWnd) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);

                if (!hPointWnd)
                    nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolModernIndicatorWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;

                if (nCloseSndVolTimerCount < 10)
                    return;

                HideSndVolModernIndicator();
            }
        }

        EndSndVolModernIndicatorSession();
    } else {
        if (ValidateSndVolProcess()) {
            if (WaitForInputIdle(hSndVolProcess, 0) != 0)
                return;

            if (ValidateSndVolWnd()) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);

                if (!hPointWnd)
                    nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;

                if (nCloseSndVolTimerCount < 10)
                    return;

                if (hPointWnd != hSndVolWnd)
                    PostMessage(hSndVolWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), (LPARAM)NULL);
            }
        }
    }

    KillTimer(NULL, nCloseSndVolTimer);
    nCloseSndVolTimer = 0;
}

static HWND GetSndVolDlg(HWND hVolumeAppWnd) {
    HWND hWnd = NULL;
    EnumThreadWindows(GetWindowThreadProcessId(hVolumeAppWnd, NULL),
                      EnumThreadFindSndVolWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam) {
    WCHAR szClass[16];

    GetClassName(hWnd, szClass, _countof(szClass));
    if (lstrcmp(szClass, L"#32770") == 0) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }

    return TRUE;
}

static BOOL IsSndVolWndInitialized(HWND hWnd) {
    HWND hChildDlg;

    hChildDlg = FindWindowEx(hWnd, NULL, L"#32770", NULL);
    if (!hChildDlg)
        return FALSE;

    if (!(GetWindowLong(hChildDlg, GWL_STYLE) & WS_VISIBLE))
        return FALSE;

    return TRUE;
}

static BOOL MoveSndVolCenterMouse(HWND hWnd) {
    NOTIFYICONIDENTIFIER notifyiconidentifier;
    BOOL bCompositionEnabled;
    POINT pt;
    SIZE size;
    RECT rc, rcExclude, rcInflate;
    int nInflate;

    ZeroMemory(&notifyiconidentifier, sizeof(NOTIFYICONIDENTIFIER));
    notifyiconidentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
    memcpy(&notifyiconidentifier.guidItem,
           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4\x1C\xB6\x7D\x5B\x9C",
           sizeof(GUID));

    if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcExclude) != S_OK)
        SetRectEmpty(&rcExclude);

    GetCursorPos(&pt);
    GetWindowRect(hWnd, &rc);

    nInflate = 0;

    if (DwmIsCompositionEnabled(&bCompositionEnabled) == S_OK &&
        bCompositionEnabled) {
        memcpy(
            &notifyiconidentifier.guidItem,
            "\x43\x65\x4B\x96\xAD\xBB\xEE\x44\x84\x8A\x3A\x95\xD8\x59\x51\xEA",
            sizeof(GUID));

        if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcInflate) ==
            S_OK) {
            nInflate = rcInflate.bottom - rcInflate.top;
            InflateRect(&rc, nInflate, nInflate);
        }
    }

    size.cx = rc.right - rc.left;
    size.cy = rc.bottom - rc.top;

    if (!CalculatePopupWindowPosition(
            &pt, &size,
            TPM_CENTERALIGN | TPM_VCENTERALIGN | TPM_VERTICAL | TPM_WORKAREA,
            &rcExclude, &rc))
        return FALSE;

    SetWindowPos(
        hWnd, NULL, rc.left + nInflate, rc.top + nInflate, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);

    return TRUE;
}

// Modern indicator functions

static BOOL CanUseModernIndicator() {
    if (g_nWinVersion < WIN_VERSION_10_T1 ||
        g_settings.volumeIndicator == VolumeIndicator::Classic)
        return FALSE;

    DWORD dwEnabled = 1;
    DWORD dwValueSize = sizeof(dwEnabled);
    DWORD dwError = RegGetValue(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\MTCUVC",
        L"EnableMTCUVC", RRF_RT_REG_DWORD, NULL, &dwEnabled, &dwValueSize);

    // We don't check dwError just like Microsoft doesn't at SndVolSSO.dll.
    if (!dwError)
        Wh_Log(L"%u", dwError);

    return dwEnabled != 0;
}

static BOOL ShowSndVolModernIndicator() {
    if (bSndVolModernLaunched)
        return TRUE;  // already launched

    HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
    if (hSndVolModernIndicatorWnd)
        return TRUE;  // already shown

    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd && hForegroundWnd != g_hTaskbarWnd)
        hSndVolModernPreviousForegroundWnd = hForegroundWnd;

    HWND hSndVolTrayControlWnd = GetSndVolTrayControlWnd();
    if (!hSndVolTrayControlWnd)
        return FALSE;

    if (!PostMessage(hSndVolTrayControlWnd, 0x460, 0,
                     MAKELPARAM(NIN_SELECT, 100)))
        return FALSE;

    bSndVolModernLaunched = TRUE;
    return TRUE;
}

static BOOL HideSndVolModernIndicator() {
    HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
    if (hSndVolModernIndicatorWnd) {
        if (!hSndVolModernPreviousForegroundWnd ||
            !SetForegroundWindow(hSndVolModernPreviousForegroundWnd))
            SetForegroundWindow(g_hTaskbarWnd);
    }

    return TRUE;
}

static void EndSndVolModernIndicatorSession() {
    hSndVolModernPreviousForegroundWnd = NULL;
    bSndVolModernLaunched = FALSE;
    bSndVolModernAppeared = FALSE;
}

static HWND GetOpenSndVolModernIndicatorWnd() {
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd)
        return NULL;

    // Check class name
    WCHAR szBuffer[32];
    if (!GetClassName(hForegroundWnd, szBuffer, 32) ||
        wcscmp(szBuffer, L"Windows.UI.Core.CoreWindow") != 0)
        return NULL;

    // Check that the MtcUvc prop exists
    WCHAR szVerifyPropName[sizeof(
        "ApplicationView_CustomWindowTitle#1234567890#MtcUvc")];
    wsprintf(szVerifyPropName, L"ApplicationView_CustomWindowTitle#%u#MtcUvc",
             (DWORD)(DWORD_PTR)hForegroundWnd);

    SetLastError(0);
    GetProp(hForegroundWnd, szVerifyPropName);
    if (GetLastError() != 0)
        return NULL;

    return hForegroundWnd;
}

static HWND GetSndVolTrayControlWnd() {
    // The window we're looking for has a class name similar to
    // "ATL:00007FFAECBBD280". It shares a thread with the bluetooth window,
    // which is easier to find by class, so we use that.

    HWND hBluetoothNotificationWnd =
        FindWindow(L"BluetoothNotificationAreaIconWindowClass", NULL);
    if (!hBluetoothNotificationWnd)
        return NULL;

    HWND hWnd = NULL;
    EnumThreadWindows(GetWindowThreadProcessId(hBluetoothNotificationWnd, NULL),
                      EnumThreadFindSndVolTrayControlWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd,
                                                        LPARAM lParam) {
    HMODULE hInstance = (HMODULE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    if (hInstance && hInstance == GetModuleHandle(L"sndvolsso.dll")) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }

    return TRUE;
}

#pragma endregion  // sndvol

////////////////////////////////////////////////////////////

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_taskbar-volume-control");

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam,
           LPARAM lParam) WINAPI_LAMBDA_RETURN(LRESULT) {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                        (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
                    param->result =
                        SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                          param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture() != NULL) {
        Wh_Log(L"OnMouseWheel: Rejected (Capture is set)");
        return false;
    }

    if (g_settings.ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
        // Wh_Log(L"OnMouseWheel: Rejected (Ctrl not pressed)"); // Optional
        return false;
    }

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    if (!IsPointInsideScrollArea(hWnd, pt)) {
        Wh_Log(L"OnMouseWheel: Rejected (Point %d,%d not inside scroll area of HWND %p)", pt.x, pt.y, hWnd);
        return false;
    }

    // Allows to steal focus
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    BOOL isItem = FALSE;
    std::wstring appName;
    DWORD pid = GetPidFromTaskbarPoint(pt, &isItem, &appName);
    
    // Allow if PID found OR if we identified it as an Item and have a Name (Smart Match fallback)
    if (pid || (isItem && !appName.empty())) {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        
        std::wstring processNameOverride = L"";
        
        // App-specific overrides via Mapping Table
        bool isPartial = false;
        bool useSmartMatch = false;
        
        if (!appName.empty()) {
            bool mappingFound = false;
            for (const auto& mapping : g_appMappings) {
                if (HasSubstringCaseInsensitive(appName, mapping.keyword)) {
                    processNameOverride = mapping.exeName;
                    isPartial = mapping.isPartial;
                    mappingFound = true;
                    break;
                }
            }
            
            // If no manual mapping found, enable SMART MATCH using the Taskbar Name
            if (!mappingFound) {
                processNameOverride = appName;
                useSmartMatch = true;
                Wh_Log(L"Smart Match fallback for: %s", appName.c_str());
            }
        }

        if (SetAppVolume(pid, delta, processNameOverride, isPartial, useSmartMatch)) {
            Wh_Log(L"Success for PID: %d, Name: %s", pid, appName.c_str());
            return true;
        } else {
            Wh_Log(L"Failed for PID: %d, Name: %s", pid, appName.c_str());
        }
    } else {
        Wh_Log(L"No Target. PID: %d, Name: %s, isItem: %d", pid, appName.c_str(), isItem);
    }
    
    // If we are over a taskbar item (like an app icon), but failed to change its volume
    // (e.g. no audio session or pid lookup failed), we should STOP here and NOT change
    // the system master volume.
    if (isItem) {
        Wh_Log(L"Blocked Global Change. isItem: %d", isItem);
        return true;
    }

    Wh_Log(L"OnMouseWheel: Rejected (No PID and no App Name, fallback to global)");
    Wh_Log(L"Falling back to Global Volume change.");
    OpenScrollSndVol(wParam, lParam);

    return true;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass,
                                           _In_ DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    LRESULT result = 0;

    switch (uMsg) {
        case WM_COPYDATA: {
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            typedef struct _notifyiconidentifier_internal {
                DWORD dwMagic;    // 0x34753423
                DWORD dwRequest;  // 1 for (x,y) | 2 for (w,h)
                DWORD cbSize;     // 0x20
                DWORD hWndHigh;
                DWORD hWndLow;
                UINT uID;
                GUID guidItem;
            } NOTIFYICONIDENTIFIER_INTERNAL;

            COPYDATASTRUCT* p_copydata = (COPYDATASTRUCT*)lParam;

            // Change Shell_NotifyIconGetRect handling result for the volume
            // icon. In case it's not visible, or in Windows 11, it returns 0,
            // which causes sndvol.exe to ignore the command line position.
            if (result == 0 && p_copydata->dwData == 0x03 &&
                p_copydata->cbData == sizeof(NOTIFYICONIDENTIFIER_INTERNAL)) {
                NOTIFYICONIDENTIFIER_INTERNAL* p_icon_ident =
                    (NOTIFYICONIDENTIFIER_INTERNAL*)p_copydata->lpData;
                if (p_icon_ident->dwMagic == 0x34753423 &&
                    (p_icon_ident->dwRequest == 0x01 ||
                     p_icon_ident->dwRequest == 0x02) &&
                    p_icon_ident->cbSize == 0x20 &&
                    memcmp(&p_icon_ident->guidItem,
                           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4"
                           "\x1C\xB6\x7D\x5B\x9C",
                           sizeof(GUID)) == 0) {
                    RECT rc;
                    GetWindowRect(hWnd, &rc);

                    if (p_icon_ident->dwRequest == 0x01)
                        result = MAKEWORD(rc.left, rc.top);
                    else
                        result =
                            MAKEWORD(rc.right - rc.left, rc.bottom - rc.top);
                }
            }
            break;
        }

        case WM_MOUSEWHEEL:
            if (g_nExplorerVersion < WIN_VERSION_11_21H2 &&
                OnMouseWheel(hWnd, wParam, lParam)) {
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (hWnd != g_hTaskbarWnd) {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            // Relaxed Check: We don't strictly require the root to be our known taskbar window.
            // If OnMouseWheel (which checks screen coordinates) handles it, we eat it.
            if (/*HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) && */ OnMouseWheel(hWnd, wParam, lParam)) {
                Wh_Log(L"InputSiteHook: EATING MESSAGE");
                return 0; // Handled, eat the message
            }
            Wh_Log(L"InputSiteHook: Not handled, falling through for uMsg: %d", uMsg);
            break; // Not handled, fall through to default processing
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) {
    SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    void* wndProc = (void*)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    Wh_SetFunctionHook(wndProc, (void*)InputSiteWindowProc_Hook,
                       (void**)&InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SndVolInit();
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
            !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

void LoadSettings() {
    PCWSTR volumeIndicator = Wh_GetStringSetting(L"volumeIndicator");
    g_settings.volumeIndicator = VolumeIndicator::Win11;
    if (wcscmp(volumeIndicator, L"modern") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Modern;
    } else if (wcscmp(volumeIndicator, L"classic") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Classic;
    } else if (wcscmp(volumeIndicator, L"none") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::None;
    } else {
        // Old option for compatibility.
        if (wcscmp(volumeIndicator, L"tooltip") == 0) {
            g_settings.volumeIndicator = VolumeIndicator::None;
        }
    }
    Wh_FreeStringSetting(volumeIndicator);

    PCWSTR scrollArea = Wh_GetStringSetting(L"scrollArea");
    g_settings.scrollArea = ScrollArea::taskbar;
    if (wcscmp(scrollArea, L"notification_area") == 0) {
        g_settings.scrollArea = ScrollArea::notificationArea;
    } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
        g_settings.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
    }
    Wh_FreeStringSetting(scrollArea);

    g_settings.middleClickToMute = Wh_GetIntSetting(L"middleClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    g_settings.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"noAutomaticMuteToggle");
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

using VolumeSystemTrayIconDataModel_OnIconClicked_t =
    void(WINAPI*)(void* pThis, void* iconClickedEventArgs);
VolumeSystemTrayIconDataModel_OnIconClicked_t
    VolumeSystemTrayIconDataModel_OnIconClicked_Original;
void WINAPI
VolumeSystemTrayIconDataModel_OnIconClicked_Hook(void* pThis,
                                                 void* iconClickedEventArgs) {
    Wh_Log(L">");

    if (g_settings.middleClickToMute && GetKeyState(VK_MBUTTON) < 0) {
        ToggleVolMuted();
        return;
    }

    VolumeSystemTrayIconDataModel_OnIconClicked_Original(pThis,
                                                         iconClickedEventArgs);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))"},
            &VolumeSystemTrayIconDataModel_OnIconClicked_Original,
            VolumeSystemTrayIconDataModel_OnIconClicked_Hook,
            true,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                    g_nExplorerVersion = WIN_VERSION_10_20H1;
                }
                break;
            }
        }
    }
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3)) {
        if (IsExplorerPatcherModule(module)) {
            if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                g_nExplorerVersion = WIN_VERSION_10_20H1;
            }
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    if (g_nWinVersion >= WIN_VERSION_11_22H2 && g_settings.middleClickToMute) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Taskbar view module not loaded yet");

            HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
            auto pKernelBaseLoadLibraryExW =
                (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                          "LoadLibraryExW");
            WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
        }
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    CleanupSndVol();
    SndVolUninit();
    ClearAudioCache();
    ClearPidCache(); // Cleaning up cache
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    bool prevMiddleClickToMute = g_settings.middleClickToMute;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               g_settings.middleClickToMute != prevMiddleClickToMute;

    return TRUE;
}

