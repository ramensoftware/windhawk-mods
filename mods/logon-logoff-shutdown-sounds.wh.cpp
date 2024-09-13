// ==WindhawkMod==
// @id              logon-logoff-shutdown-sounds
// @name            Logon, Logoff & Shutdown Sounds Restored
// @description     Restores the logon, logoff and shutdown sounds from earlier versions of Windows
// @version         1.0.1
// @author          Toru the Red Fox
// @github          https://github.com/TorutheRedFox
// @twitter         https://twitter.com/TorutheRedFox
// @include         explorer.exe
// @compilerOptions -lcomdlg32 -lshlwapi -lwinmm -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Logon Sounds Restored
Restores the logon, logoff and shutdown sounds from earlier versions of Windows, simple as.

It is recommended to use [these reg files](https://www.howtogeek.com/wp-content/uploads/2016/09/Shutdown-Logoff-Logon-Sound-Hacks.zip) to restore the sound events to the Sound control panel applet.

Note: Likely redundant with explorer7 due to 7's explorer having this code in it already.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- xpmode: false
  $name: XP Startup behavior
  $description: Plays the startup sound on logon. Resuming will still play the logon sound. It is recommended to untick the box to play the startup sound in the control panel to prevent it from playing before you log on.

*/
// ==/WindhawkModSettings==

#include <gdiplus.h>
#include <handleapi.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <shlwapi.h>
#include <winerror.h>
#include <winuser.h>

// settings
struct {
    bool    bXpMode;
} settings;

// the various global handles used 

HMODULE g_hShlwapi;
HWND g_hwSound;
HANDLE g_hSoundThread;
HMODULE g_hExplorer;

// NOTE: sligtly different versions of this exist in...
//      SHPlaySound() -> shell32
//      IEPlaySound() -> shdocvw/browseui

STDAPI_(void) ExplorerPlaySound(LPCTSTR pszSound)
{
    // note, we have access only to global system sounds here as we use "Apps\.Default"
    TCHAR szKey[256];
    wsprintf(szKey, TEXT("AppEvents\\Schemes\\Apps\\.Default\\%s\\.current"), pszSound);

    TCHAR szFileName[MAX_PATH];
    szFileName[0] = 0;
    LONG cbSize = sizeof(szFileName);

    // test for an empty string, PlaySound will play the Default Sound if we
    // give it a sound it cannot find...

    if ((RegQueryValue(HKEY_CURRENT_USER, szKey, szFileName, &cbSize) == ERROR_SUCCESS)
        && szFileName[0])
    {
        // flags are relevant, we try to not stomp currently playing sounds
        PlaySound(szFileName, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
    }
}

typedef LRESULT WINAPI (* _OnSessionChange_t)(void* _this, WPARAM wParam, LPARAM lParam);
_OnSessionChange_t _OnSessionChange_orig;
LRESULT WINAPI _OnSessionChange_hook(void* _this, WPARAM wParam, LPARAM lParam)
{
    if ((WTS_SESSION_LOCK == wParam) || (WTS_SESSION_UNLOCK == wParam))
    {
        if (wParam == WTS_SESSION_LOCK)
        {
            ExplorerPlaySound(TEXT("WindowsLogoff"));
        }
        else if (wParam == WTS_SESSION_UNLOCK)
        {
            ExplorerPlaySound(TEXT("WindowsLogon"));
        }
    }

    return _OnSessionChange_orig(_this, wParam, lParam);
}

#ifndef SND_SYSTEM // because this is sometimes missing???
#define SND_SYSTEM 0x00200000
#endif

DWORD WINAPI PlaySoundFileThreadProc( LPVOID lpParam ) 
{
    PlaySoundW((WCHAR*)lpParam, 0, SND_NODEFAULT | SND_MEMORY | SND_SYSTEM);
    LocalFree(lpParam);
    return 0;
}

DWORD WINAPI GetLastErrorError()
{
    DWORD hr = GetLastError();
    if ( hr == S_OK )
        return 1;
    return hr;
}

HRESULT WINAPI HRESULTFromLastErrorError()
{
    HRESULT hr = GetLastErrorError();
    if ( FAILED(hr) )
        return HRESULT_FROM_WIN32((USHORT)hr);
    return hr;
}

HRESULT WINAPI PlaySoundFile(HANDLE* hCurrentThread, LPCWSTR lpszFileName) {
    DWORD szSoundFile;           // eax
    signed int hr = 0;    // esi
    DWORD NumberOfBytesRead;  // [esp+8h] [ebp-Ch] BYREF
    HANDLE hFile;             // [esp+Ch] [ebp-8h]
    LPVOID lpSndBuf = 0;       // [esp+10h] [ebp-4h]

    hFile = CreateFile(
        lpszFileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL
    );

    if (hFile != INVALID_HANDLE_VALUE) {
        szSoundFile = GetFileSize(hFile, 0);
        hr = E_OUTOFMEMORY;
        if (szSoundFile != INVALID_FILE_SIZE && szSoundFile >= 0 && szSoundFile < 0x400000) {
            lpSndBuf = LocalAlloc(0, szSoundFile);
            if (lpSndBuf && ReadFile(hFile, lpSndBuf, szSoundFile, &NumberOfBytesRead, 0)) {
                hr = szSoundFile != NumberOfBytesRead ? HRESULT_FROM_WIN32(ERROR_IO_PENDING) : 0;
            } else {
                hr = HRESULTFromLastErrorError();
            }
        }
        CloseHandle(hFile);
    } else {
        hr = HRESULTFromLastErrorError();
    }

    if (SUCCEEDED(hr)) {
        HANDLE hPlaySndThread = CreateThread(0, 0, PlaySoundFileThreadProc, lpSndBuf, 0, 0);
        if (hPlaySndThread) {
            if (hCurrentThread)
                *hCurrentThread = hPlaySndThread;
            else
                CloseHandle(hPlaySndThread);
            return hr;
        }
        hr = HRESULTFromLastErrorError();
    }

    if (lpSndBuf)
        LocalFree(lpSndBuf);
    
    return hr;
}

typedef enum _ELOGONLOGOFFSOUNDTYPE
{
    ST_LOGON,
    ST_LOGOFF,
    ST_EXIT,
    ST_START,
} ELOGONLOGOFFSOUNDTYPE;

HRESULT PlayLogonLogoffSound(HANDLE* hThread, ELOGONLOGOFFSOUNDTYPE enSoundType)
{
    const wchar_t *szSoundType;
    HRESULT hr;
    DWORD pcbData[4];
    WCHAR szSubKey[264];
    WCHAR pvData[264];

    switch (enSoundType) {
        case ST_LOGON:
            szSoundType = TEXT("WindowsLogon");
            break;
        case ST_LOGOFF:
            szSoundType = TEXT("WindowsLogoff");
            break;
        case ST_EXIT:
            szSoundType = TEXT("SystemExit");
            break;
        case ST_START:
            szSoundType = TEXT("SystemStart");
            break;
    }
    
    pcbData[0] = 520;
    
    hr = wsprintf(szSubKey, TEXT("AppEvents\\Schemes\\Apps\\.Default\\%ws\\.Current"), szSoundType);

    if (SUCCEEDED(hr))
    {
        hr = RegGetValue(HKEY_CURRENT_USER, szSubKey, 0, 2u, 0, pvData, pcbData);
        if (hr == S_OK)
            return PlaySoundFile(hThread, pvData);
        else
            return HRESULT_FROM_WIN32((USHORT)hr);
    }
    return hr;
}

LRESULT SHDefWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (IsWindowUnicode(hwnd))
    {
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
}

STDAPI_(BOOL) SHRegisterClassW(const WNDCLASSW* pwc)
{
    WNDCLASSW wc;
    if (!GetClassInfo(pwc->hInstance, pwc->lpszClassName, &wc)) 
    {
        return RegisterClass(pwc);
    }
    return TRUE;
}

HWND WINAPI SHCreateWorkerWindowW(WNDPROC pfnWndProc, HWND hwndParent, DWORD dwExStyle, DWORD dwFlags, HMENU hmenu, void * p)
{
    WNDCLASSW wc = {0};

    wc.lpfnWndProc      = DefWindowProcW;
    wc.cbWndExtra       = sizeof(void *);
    wc.hInstance        = g_hShlwapi;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszClassName    = L"WorkerW";
    //dwExStyle |= IsBiDiLocalizedSystem() ? /*dwExStyleRTLMirrorWnd*/0L : 0L;

    SHRegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(dwExStyle, L"WorkerW", NULL, dwFlags,
                                  0, 0, 0, 0, hwndParent,
                                  (HMENU)hmenu, g_hShlwapi, NULL);
    if (hwnd) 
    {
        SetWindowLongPtr(hwnd, 0, (LPARAM)(p));

        // Note: Must explicitly use W version to avoid charset thunks
        if (pfnWndProc)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnWndProc);

    }

    return hwnd;
}

class CSoundWnd {
    public:
        WINAPI CSoundWnd();
        LONG m_refCount;
        HWND m_hwndSound;
        HANDLE m_thread;
        BOOL WINAPI Init();
        static DWORD CALLBACK s_ThreadProc(void* lpParam);
        LONG WINAPI Release();
        static DWORD CALLBACK s_CreateWindow(void* lpParam);
        static LRESULT CALLBACK s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam);
        LRESULT WINAPI v_WndProc(HWND hWnd, UINT mst, WPARAM wParam, LPARAM lParam);
};

WINAPI CSoundWnd::CSoundWnd()
    : m_refCount(1)
    , m_hwndSound(nullptr)
    , m_thread(nullptr)
{
}

BOOL WINAPI CSoundWnd::Init()
{
    if (!SHCreateThread(s_ThreadProc, this, CTF_COINIT_STA | CTF_PROCESS_REF, s_CreateWindow)) {
        DWORD dwLastError = GetLastError();
        LPWSTR pszMessageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pszMessageBuffer, 0, NULL);
    
        Wh_Log(TEXT("CSoundWnd::Init Failed to create thread %s"), pszMessageBuffer);
    }
    g_hwSound = this->m_hwndSound;
    SetProcessShutdownParameters(0x4FF, NULL);
    return this->m_hwndSound != NULL;
}

DWORD CALLBACK CSoundWnd::s_CreateWindow(void* lpParam)
{
    InterlockedIncrement(&((CSoundWnd*)lpParam)->m_refCount);
    
    ((CSoundWnd*)lpParam)->m_hwndSound = SHCreateWorkerWindowW(s_WndProc, 0, 0, 0, 0, lpParam);
    return 0;
}

__inline void * GetWindowPtr0(HWND hWnd) {
    return (void *)GetWindowLongPtrA(hWnd, 0);
}

DWORD CALLBACK CSoundWnd::s_ThreadProc(void* lpParam)
{
    struct tagMSG Msg; // [esp+8h] [ebp-1Ch] BYREF
    CSoundWnd* _this = static_cast<CSoundWnd*>(lpParam);

    if ( _this->m_hwndSound )
    {
        while ( GetMessageW(&Msg, 0, 0, 0) )
        {
            TranslateMessage(&Msg);
            DispatchMessageW(&Msg);
        }
    }
    _this->Release();
    return 0;
}

LRESULT CALLBACK CSoundWnd::s_WndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    CSoundWnd* soundWnd = (CSoundWnd*)GetWindowPtr0(hWnd);
    if (soundWnd)
        return soundWnd->v_WndProc(hWnd, msg, wParam, lParam);
    else
        return SHDefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI CSoundWnd::v_WndProc(HWND hWnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam) {
    WCHAR szShutdownReason[256];
    
    switch (msg) {
        case WM_QUERYENDSESSION:
            if ((lParam & ENDSESSION_CRITICAL) == FALSE) {
                LoadString(GetModuleHandle(NULL), 0x2DBu, szShutdownReason, ARRAYSIZE(szShutdownReason));
                ShutdownBlockReasonCreate(this->m_hwndSound, szShutdownReason);
                PlayLogonLogoffSound(&this->m_thread, (lParam & ENDSESSION_LOGOFF) != 0 ? ST_LOGOFF : ST_EXIT);
                if (this->m_thread) {
                    g_hSoundThread = this->m_thread;
                    WaitForSingleObject(this->m_thread, INFINITE);
                    CloseHandle(this->m_thread);
                }
            }
            return 1;
        case WM_ENDSESSION:
            if (wParam && (lParam & ENDSESSION_CRITICAL) == FALSE && this->m_thread) {
                WaitForSingleObject(this->m_thread, INFINITE);
                CloseHandle(this->m_thread);
            }
            DestroyWindow(this->m_hwndSound);
            return SHDefWindowProc(hWnd, msg, wParam, lParam);
        case WM_NCDESTROY:
            SetWindowLong(hWnd, 0, 0);
            g_hwSound = NULL;
            this->m_hwndSound = NULL;
            PostQuitMessage(0);
            return 0;
        default:
            return SHDefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LONG WINAPI CSoundWnd::Release()
{
    LONG lRefCount = InterlockedDecrement(&this->m_refCount);
    if ( !lRefCount )
        operator delete((void *)this);
    return lRefCount;
}

BOOL WINAPI InitSoundWindow()
{
    BOOL bSuccess = FALSE;
    CSoundWnd* soundWnd = new CSoundWnd();
    if (soundWnd)
    {
        bSuccess = soundWnd->Init();
        soundWnd->Release();
    }
    return bSuccess;
}

DWORD WINAPI GetCurrentSessionId()
{
    DWORD pSessionId;
    DWORD CurrentProcessId = GetCurrentProcessId();
    ProcessIdToSessionId(CurrentProcessId, &pSessionId);
    return pSessionId;
}

HRESULT WINAPI SHCreateSessionKey(REGSAM samDesired, PHKEY phKey)
{
    HRESULT hr = S_OK;
    static WCHAR wszSessionKey[256];
    LONG Error;
 
    if (DWORD dwSessionId = GetCurrentSessionId())
    {
        swprintf(wszSessionKey,
                      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%d",
                      dwSessionId);
    }
    else
        hr = HRESULT_FROM_WIN32(GetLastError());
 
    if(SUCCEEDED(hr))
    {
        Error = RegCreateKeyEx(HKEY_CURRENT_USER, wszSessionKey, 0, NULL,
                                REG_OPTION_VOLATILE, samDesired, NULL, phKey, NULL);
        if (Error != ERROR_SUCCESS)
            hr = HRESULT_FROM_WIN32(Error);
    }
    
    return hr;
}

//  a three state boolean for bools that need initialization
typedef enum 
{
    TRIBIT_UNDEFINED = 0,
    TRIBIT_TRUE,
    TRIBIT_FALSE,
} TRIBIT;

TRIBIT g_tbPlayedStartupSound;
DWORD g_dwCurProcId;

BOOL WINAPI HasPerLogonActionBeenDone(LPCWSTR lpSubKey, TRIBIT* out_tbDone)
{
    DWORD dwDisposition;
    HKEY hKey;
    HKEY phkResult;

    *out_tbDone = TRIBIT_TRUE;
    if ( SUCCEEDED(SHCreateSessionKey(KEY_WRITE, &hKey)) )
    {
        if ( RegCreateKeyExW(hKey, lpSubKey, NULL, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &phkResult, &dwDisposition) == ERROR_SUCCESS )
        {
            RegCloseKey(phkResult);
            if ( dwDisposition == REG_CREATED_NEW_KEY )
                *out_tbDone = TRIBIT_FALSE;
        }
        RegCloseKey(hKey);
    }
    return *out_tbDone == TRIBIT_TRUE;
}

BOOL WINAPI HasLogonSoundBeenPlayed()
{
    return HasPerLogonActionBeenDone(L"LogonSoundHasBeenPlayed", &g_tbPlayedStartupSound);
}

BOOL WINAPI IsDesktopProcess() {
    DWORD dwShellProcId = NULL;
    DWORD dwCurProcId = GetCurrentProcessId();
    HWND hShellWindow = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    if (!hShellWindow) // no desktop present yet, assume it's main explorer starting up
    {
        g_dwCurProcId = dwCurProcId;
        return TRUE;
    }
    GetWindowThreadProcessId(hShellWindow, &dwShellProcId);
    if (dwCurProcId == dwShellProcId || !hShellWindow) {
        if (dwCurProcId != g_dwCurProcId) // because the dll handle is shared, the boolean needs to be reset when the explorer process changes
        {
            g_tbPlayedStartupSound = TRIBIT_UNDEFINED;
            g_dwCurProcId = dwCurProcId;
        }
        return TRUE;
    } else {
        g_dwCurProcId = dwCurProcId;
        return FALSE;
    }
}

// Load the settings for the mod
void LoadSettings(void)
{
    settings.bXpMode = (BOOL)Wh_GetIntSetting(L"xpmode");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hExplorer = GetModuleHandleW(NULL);
    if (!g_hExplorer)
        Wh_Log(L"Failed to get Explorer's handle");

    g_hShlwapi = LoadLibrary(TEXT("shlwapi.dll"));

    WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
        {
            {
                #ifdef _WIN64
                L"protected: __int64 __cdecl CTray::_OnSessionChange(unsigned __int64,__int64)"
                #else
                L"protected: long __thiscall CTray::_OnSessionChange(unsigned int,long)"
                #endif
            },
            (void **)&_OnSessionChange_orig,
            (void *)_OnSessionChange_hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(
        g_hExplorer,
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook _OnSessionChange");
        return FALSE;
    }

    if (IsDesktopProcess()) {
        InitSoundWindow();
        SetProcessShutdownParameters(0x4FF, NULL);
        if (!HasLogonSoundBeenPlayed())
            PlayLogonLogoffSound(NULL, settings.bXpMode ? ST_START : ST_LOGON);
        else
            Wh_Log(TEXT("Logon sound already played in this session."));
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}

VOID Wh_ModUninit()
{
    Wh_Log(TEXT("Exiting"));

    if (g_hSoundThread) // make sure DLL stays alive until we finish playing
        WaitForSingleObject(g_hSoundThread, INFINITE);
    
    if (g_hwSound) {
        PostMessage(g_hwSound, WM_CLOSE, NULL, NULL);
        g_hwSound = NULL;
    }

    if (g_hExplorer) {
        CloseHandle(g_hExplorer);
    }

    if (g_hShlwapi) {
        CloseHandle(g_hShlwapi);
    }
}
