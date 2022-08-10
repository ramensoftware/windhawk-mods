// ==WindhawkMod==
// @id           lm-vs-solution-icon
// @name         Visual Studio Solution Icon
// @description  Add an icon overlay on the Visual Studio task bar icon
// @version      0.7
// @author       Mark Jansen
// @github       https://github.com/learn-more
// @twitter      https://twitter.com/learn_more
// @include      devenv.exe
// @compilerOptions -lole32 -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Visual Studio Solution Icon
Add an icon overlay on the Visual Studio task bar icon.

If the file .editoricon.ico exists, this will be used as overlay icon.
When this file does not exist, an icon will be generated from the solution filename.

![Example](https://i.imgur.com/PKKHt5a.png)

Tested on:
- Visual Studio 2017
- Visual Studio 2019
- Visual Studio 2022

Inspired by [SolutionIcon](https://github.com/ashmind/SolutionIcon/blob/master/SolutionIcon/Implementation/IconGenerator.cs)

## Releases:
- 0.7:
  - Try to use/load .editoricon.ico
- 0.6:
  - Reload the icon when the taskbar button is refreshed
- 0.5:
  - Initial release
*/
// ==/WindhawkModReadme==


#include <string>
#include <filesystem>
#include <shobjidl.h>


// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_lm-vs-solution-icon");
UINT g_taskbarButtonCreatedMsg = RegisterWindowMessage(L"TaskbarButtonCreated");
UINT g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

constexpr int kImageWidth = 16;
constexpr int kImageHeight = 16;

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
  SUBCLASSPROC pfnSubclass;
  UINT_PTR uIdSubclass;
  DWORD_PTR dwRefData;
  BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode, WPARAM wParam,
                                              LPARAM lParam) {
  if (nCode == HC_ACTION) {
    const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
    if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
      SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *param =
          (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM *)cwp->lParam;
      param->result = ::SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                        param->uIdSubclass, param->dwRefData);
    }
  }

  return ::CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd, SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
  DWORD dwThreadId = ::GetWindowThreadProcessId(hWnd, nullptr);
  if (dwThreadId == 0) {
    return FALSE;
  }

  if (dwThreadId == ::GetCurrentThreadId()) {
    return ::SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
  }

  HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass,
                                nullptr, dwThreadId);
  if (!hook) {
    return FALSE;
  }

  SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
  param.pfnSubclass = pfnSubclass;
  param.uIdSubclass = uIdSubclass;
  param.dwRefData = dwRefData;
  param.result = FALSE;
  SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (WPARAM)&param);

  UnhookWindowsHookEx(hook);

  return param.result;
}

static void ApplyIconAfterReload(void);
LRESULT CALLBACK VSWindowSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
                                      _In_ WPARAM wParam, _In_ LPARAM lParam,
                                      _In_ UINT_PTR uIdSubclass,
                                      _In_ DWORD_PTR dwRefData) {
  if (uMsg == g_taskbarCreatedMsg) {
    Wh_Log("TaskbarCreated");
    ApplyIconAfterReload();
  }
  if (uMsg == g_taskbarButtonCreatedMsg) {
    Wh_Log("TaskbarButtonCreated");
    ApplyIconAfterReload();
  } else if (uMsg == g_subclassRegisteredMsg && !wParam) {
    RemoveWindowSubclass(hWnd, VSWindowSubclassProc, 0);
  }

  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void SubclassVSWindow(HWND hWnd) {
  SetWindowSubclassFromAnyThread(hWnd, VSWindowSubclassProc, 0, 0);
}

void UnsubclassVSWindow(HWND hWnd) {
  SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

HRESULT g_CoInit = E_FAIL;
ITaskbarList3* g_TaskbarList3 = nullptr;

struct Utils
{
static std::wstring LettersFromFilename(const std::wstring& str);
static bool Selftest();
static HWND MainWindow();
static HICON CreateCustomIcon(const std::wstring& full_path, const std::wstring& text);
};

static void ApplyIcon(const std::wstring& commandline)
{
    Wh_Log(L"Got %s", commandline.c_str());
    std::wstring::size_type restartManager = commandline.find(L"/restartManager");
    if (restartManager != std::wstring::npos)
    {
        std::wstring tmp = commandline.substr(0, restartManager);
        // Erase trailing whitespace
        tmp.erase(tmp.find_last_not_of(L" \t\n\r\f\v") + 1);
        // Remove quotes
        if (!tmp.empty() && tmp[0] == '"' && tmp[tmp.size()-1] == '"')
        {
            tmp = tmp.substr(1, tmp.size() - 2);
        }

        HICON icon = NULL;
        if (!tmp.empty())
        {
            std::filesystem::path path = tmp;
            path.replace_filename(".editoricon.ico");
            if (std::filesystem::exists(path))
            {
                icon = (HICON)::LoadImageW(NULL, path.c_str(), IMAGE_ICON, kImageWidth, kImageHeight, LR_LOADFROMFILE);
                Wh_Log(L"Found %s (loaded:%p)", path.c_str(), icon);
            }

            // No custom icon found yet?
            if (icon == NULL)
            {
                // Convert a full path into a 2-letter abbreviation
                std::wstring ico = Utils::LettersFromFilename(tmp);
                Wh_Log(L"'%s' => '%s'", tmp.c_str(), ico.c_str());
                // Generate an icon
                icon = Utils::CreateCustomIcon(tmp, ico);
            }
        }

        // Did we find something to show as icon?
        if (icon == NULL)
        {
            g_TaskbarList3->SetOverlayIcon(Utils::MainWindow(), NULL, NULL);
        }
        else
        {
            g_TaskbarList3->SetOverlayIcon(Utils::MainWindow(), icon, NULL);
            ::DestroyIcon(icon);
        }
    }
}

// Either the module is reloaded, or the taskbar button is recreated
static void ApplyIconAfterReload()
{
    wchar_t Buffer[RESTART_MAX_CMD_LINE+1] = {};
    DWORD dwBufferSize = _countof(Buffer);

    HRESULT hr = ::GetApplicationRestartSettings(::GetCurrentProcess(), Buffer, &dwBufferSize, NULL);
    if (SUCCEEDED(hr))
    {
        ApplyIcon(Buffer);
    }
    else
    {
        Wh_Log(L"GetApplicationRestartSettings failed: 0x%x", hr);
    }
}

typedef decltype(&RegisterApplicationRestart) REGISTERAPPLICATIONRESTART;

REGISTERAPPLICATIONRESTART oRegisterApplicationRestart;
HRESULT WINAPI hkRegisterApplicationRestart(PCWSTR pwzCommandline, DWORD dwFlags)
{
    if (pwzCommandline)
    {
        ApplyIcon(pwzCommandline);
    }
    return oRegisterApplicationRestart(pwzCommandline, dwFlags);
}

BOOL Wh_ModInit(void)
{
    if (!Utils::Selftest())
        return FALSE;

    g_CoInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!SUCCEEDED(g_CoInit))
    {
        Wh_Log(L"CoInitializeEx failed: 0x%x", g_CoInit);
        return FALSE;
    }

    HRESULT hr = ::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_TaskbarList3));
    if (!SUCCEEDED(hr))
    {
        Wh_Log(L"CoCreateInstance failed: 0x%x", hr);
        return FALSE;
    }
    g_TaskbarList3->HrInit();

    Wh_SetFunctionHook((void*)::RegisterApplicationRestart, (void*)hkRegisterApplicationRestart, (void**)&oRegisterApplicationRestart);

    return TRUE;
}

void Wh_ModAfterInit(void)
{
    ApplyIconAfterReload();
    SubclassVSWindow(Utils::MainWindow());
}

void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit");

    UnsubclassVSWindow(Utils::MainWindow());

    if (g_TaskbarList3)
    {
        g_TaskbarList3->SetOverlayIcon(Utils::MainWindow(), NULL, NULL);
        g_TaskbarList3->Release();
    }
    g_TaskbarList3 = nullptr;

    if (SUCCEEDED(g_CoInit))
        CoUninitialize();
}

std::wstring Utils::LettersFromFilename(const std::wstring& str)
{
    // Cut off the path, keeping the filename (without extension)
    std::wstring tmp = std::filesystem::path(str).stem();

    // Cut off 'CompanyName.'
    std::wstring::size_type pos = tmp.find_last_of('.');
    if (pos != std::wstring::npos)
        tmp = tmp.substr(pos+1);

    std::wstring result;
    if (tmp.size() > 0u)
    {
        result = tmp[0];
        auto it = std::find_if(tmp.begin() + 1, tmp.end(), [](wchar_t c) { return std::isupper(c) != 0; } );
        if (it != tmp.end())
        {
            result += *it;
        }
        else if (tmp.size() > 1u)
        {
            result += tmp[1];
        }
        while (result.length() < 2u)
            result += ' ';
    }
    return result;
}

bool Utils::Selftest()
{
    bool pass = true;
    #define EXPECT_LETTERS(input, output) \
        do { \
            if (LettersFromFilename(input) != output) {\
                Wh_Log(L"Error: '%s' resulted in '%s' instead of '%s'", input, LettersFromFilename(input).c_str(), output); \
                pass = false; \
            } \
        } while (0)

    EXPECT_LETTERS(L"c:\\TestSolution.sln", L"TS");
    EXPECT_LETTERS(L"c:/testSolution.sln", L"tS");
    EXPECT_LETTERS(L"c:/whatever\\.\\testsolution.sln", L"te");
    EXPECT_LETTERS(L"Testsolution.sln", L"Te");
    EXPECT_LETTERS(L"T.sln", L"T ");
    EXPECT_LETTERS(L".sln", L"sl");
    EXPECT_LETTERS(L"\\\\network\\something.TestSolution.sln", L"TS");

    return pass;
}

struct handle_data
{
    HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
    return ::GetWindow(handle, GW_OWNER) == (HWND)0 && ::IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (GetCurrentProcessId() != process_id || !is_main_window(handle))
        return TRUE;
    data.window_handle = handle;
    return FALSE;
}

// Adapted from https://stackoverflow.com/a/21767578/4928207
HWND Utils::MainWindow()
{
    handle_data data;
    data.window_handle = 0;
    EnumWindows(enum_windows_callback, (LPARAM)&data);
    return data.window_handle;
}

// Mostly ported from https://github.com/ashmind/SolutionIcon/blob/master/SolutionIcon/Implementation/IconGenerator.cs
static COLORREF GetColor(const std::wstring& txt)
{
    static COLORREF colors[] = {
        RGB(0x1a, 0xbc, 0x9c), RGB(0x2e, 0xcc, 0x71), RGB(0x34, 0x98, 0xdb), RGB(0x9b, 0x59, 0xb6), RGB(0x34, 0x49, 0x5e),
        RGB(0x16, 0xa0, 0x85), RGB(0x27, 0xae, 0x60), RGB(0x29, 0x80, 0xb9), RGB(0x8e, 0x44, 0xad), RGB(0x2c, 0x3e, 0x50),
        RGB(0xf1, 0xc4, 0x0f), RGB(0xe6, 0x7e, 0x22), RGB(0x95, 0xa5, 0xa6), RGB(0xf3, 0x9c, 0x12), RGB(0x7f, 0x8c, 0x8d),
    };

    UINT hash = 0;
    for( wchar_t c : txt) {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    hash %= _countof(colors);
    return colors[hash];
}

HICON Utils::CreateCustomIcon(const std::wstring& full_path, const std::wstring& text)
{
    HDC hMem = ::CreateCompatibleDC(NULL);
    HBITMAP hbm = ::CreateBitmap(kImageWidth, kImageHeight, 1, 32, NULL);

    HGDIOBJ hOldBM = ::SelectObject(hMem, hbm);

    if (hOldBM)
    {
        LOGFONTW lf = {0};

        lf.lfHeight = -12;
        lf.lfWeight = FW_BOLD;
        lf.lfQuality = PROOF_QUALITY;
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        HFONT font = CreateFontIndirect(&lf);
        if (font)
        {
            HGDIOBJ hOldFont = ::SelectObject(hMem, font);

            RECT rc = {0, 0, kImageWidth, kImageHeight};

            HBRUSH hBG = ::CreateSolidBrush(::GetColor(full_path));
            ::FillRect(hMem, &rc, hBG);
            ::DeleteObject(hBG);

            ::SetTextColor(hMem, RGB(255, 255, 255));
            ::SetBkMode(hMem, TRANSPARENT);

            ::DrawTextW(hMem, text.c_str(), text.length(), &rc, DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_NOCLIP);

            ::SelectObject(hMem, hOldFont);
            ::DeleteObject(font);
        }

        ::SelectObject(hMem, hOldBM);
    }

    HBITMAP hbmpMask = ::CreateCompatibleBitmap(hMem, kImageWidth, kImageHeight);
    ICONINFO ii;
    ii.fIcon = TRUE;
    ii.hbmMask = hbmpMask;
    ii.hbmColor = hbm;
    HICON hIcon = ::CreateIconIndirect(&ii);
    ::DeleteObject(hbmpMask);
    ::DeleteObject(hbm);
    ::DeleteDC(hMem);

    return hIcon;
}
