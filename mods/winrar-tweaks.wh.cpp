// ==WindhawkMod==
// @id              winrar-tweaks
// @name            WinRAR Tweaks
// @description     Closes WinRAR on Esc (dialog-aware) and converts Size and Packed columns into readable units (B, KB, MB, GB, TB).
// @version         1.0.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @homepage        https://github.com/ArvindSaini978/my-windhawk-mods
// @license         MIT
// @include         WinRAR.exe
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# WinRAR Tweaks

All-in-one quality-of-life enhancements for **WinRAR** that modernize navigation and file size readability without altering archive behavior.

---

## ✨ Features

* ⌨️ **Smart Close on `Esc`:** Quickly close WinRAR with the `Escape` key—just like modern utilities and viewers. Intelligently respects active context menus, menu bars, dropdowns, inline file renaming, and child dialogs (closing open popups first before closing the main window).
* 📊 **Human-Readable File Sizes:** Automatically converts hard-to-read raw byte counts (e.g., `121815`) in the **Size** and **Packed** columns into clean, readable units (`B`, `KB`, `MB`, `GB`, `TB`).
* ⚙️ **Modular Settings:** Toggle either tweak independently at any time in the mod's Settings tab.


## 🖼️ Previews

### Human-Readable File Sizes
![WinRAR File Size Before and After Preview](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_preview_1.png)

### Smart Close on Esc
![WinRAR Close on Esc Demo Webp](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/master/assets/wt_preview_2.webp)

---

## 🛠️ How It Works

* **Modal & Input Guarding:** Queries live Win32 thread GUI state (`GetGUIThreadInfo`) so `Esc` never interrupts active menu loops, drag/capture operations, dropdowns, or inline `Edit` rename boxes before sending `WM_CLOSE`.
* **In-Place ListView Formatting:** Subclasses WinRAR's `SysListView32` file list (both newly created and already-open windows) and formats raw byte counts on the fly as items are populated or refreshed.

---

## ⚠️ Compatibility & Notes

* **Refreshing After Changing Settings or Disabling:** If you toggle the *Human-Readable File Sizes* setting on or off, or disable/remove the mod while WinRAR is open, please press **`F5`** or navigate to another folder and back to refresh the file list with the updated size format.
* **Supported Column Languages:** Out-of-the-box header detection supports English (`Size`, `Packed`) as well as common localized headers (`Размер`, `Сжатый`, `Größe`, `Gepackt`, `Taille`, `Compressé`, `Tamaño`, `Comprimido`).
* **Sorting Behavior:** Because WinRAR sorts internal archive records prior to display formatting, sorting by Size or Packed continues to sort accurately by exact byte count.

---

### 🧩 More from this author
* **[Drag to Scroll](https://windhawk.net/mods/drag-to-scroll)** – Hold `Ctrl + Right-Click` and drag to scroll smoothly across desktop applications.
* **[Explorer Custom Shortcuts](https://windhawk.net/mods/explorer-custom-shortcuts)** – Customizable keyboard shortcuts for File Explorer: open terminal, copy paths, create files/folders, toggle hidden items, and launch apps with hotkeys.

### Author & Attribution
* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- close_on_esc: true
  $name: "Close on Esc"
  $description: "Pressing Escape closes the active WinRAR dialog or main window while respecting menus, dropdowns, and inline text editing."
- readable_file_sizes: true
  $name: "Human-Readable File Sizes"
  $description: "Formats raw byte values in the Size and Packed columns into B, KB, MB, GB, and TB."
*/
// ==/WindhawkModSettings==

// clang-format on

#include <commctrl.h>
#include <windhawk_utils.h>
#include <climits>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <unordered_set>
#include <vector>

struct ModSettings {
    bool close_on_esc = true;
    bool readable_file_sizes = true;
};

static ModSettings g_settings;
static std::mutex g_listViewMutex;
static std::unordered_set<HWND> g_subclassedListViews;

// Reads user configuration from Windhawk settings
void LoadSettings() {
    g_settings.close_on_esc = Wh_GetIntSetting(L"close_on_esc") != 0;
    g_settings.readable_file_sizes =
        Wh_GetIntSetting(L"readable_file_sizes") != 0;
}

// ---------------------------------------------------------------------------
// 1. Smart Esc-to-Close Logic
// ---------------------------------------------------------------------------

// Returns true if keyboard focus is inside a text input or combobox control
static bool IsFocusInEditableControl(HWND hFocus) {
    if (!hFocus)
        return false;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hFocus, cls, ARRAYSIZE(cls)))
        return false;

    return (_wcsicmp(cls, L"Edit") == 0 || _wcsicmp(cls, L"ComboBox") == 0 ||
            _wcsicmp(cls, L"ComboBoxEx32") == 0 ||
            wcsstr(cls, L"RichEdit") != nullptr ||
            wcsstr(cls, L"RICHEDIT") != nullptr);
}

// Intercepts Escape keydown to close the active WinRAR dialog or main window
static bool ProcessEscapeKey(MSG* pMsg) {
    if (!g_settings.close_on_esc || !pMsg)
        return false;

    if (pMsg->message != WM_KEYDOWN || pMsg->wParam != VK_ESCAPE)
        return false;

    if ((pMsg->lParam & (1LL << 30)) != 0)
        return false;

    GUITHREADINFO gti = {};
    gti.cbSize = sizeof(gti);
    if (GetGUIThreadInfo(GetCurrentThreadId(), &gti)) {
        constexpr DWORD kModalFlags = GUI_INMENUMODE | GUI_POPUPMENUMODE |
                                      GUI_SYSTEMMENUMODE | GUI_INMOVESIZE;
        if ((gti.flags & kModalFlags) != 0 || gti.hwndCapture != nullptr ||
            gti.hwndMenuOwner != nullptr) {
            return false;
        }

        if (IsFocusInEditableControl(gti.hwndFocus))
            return false;
    } else if (GetCapture() != nullptr) {
        return false;
    }

    HWND hActive = gti.hwndActive ? gti.hwndActive : GetActiveWindow();
    if (!hActive)
        return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hActive, &pid);
    if (pid != GetCurrentProcessId())
        return false;

    HWND hTarget = GetAncestor(hActive, GA_ROOT);
    if (!hTarget)
        hTarget = hActive;

    PostMessageW(hTarget, WM_CLOSE, 0, 0);
    pMsg->message = WM_NULL;
    return true;
}

using GetMessageW_t = decltype(&GetMessageW);
static GetMessageW_t pfnGetMessageW_Original = nullptr;

BOOL WINAPI GetMessageW_Hook(LPMSG lpMsg,
                             HWND hWnd,
                             UINT wMsgFilterMin,
                             UINT wMsgFilterMax) {
    BOOL res =
        pfnGetMessageW_Original(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    if (res > 0 && lpMsg) {
        ProcessEscapeKey(lpMsg);
    }
    return res;
}

using PeekMessageW_t = decltype(&PeekMessageW);
static PeekMessageW_t pfnPeekMessageW_Original = nullptr;

BOOL WINAPI PeekMessageW_Hook(LPMSG lpMsg,
                              HWND hWnd,
                              UINT wMsgFilterMin,
                              UINT wMsgFilterMax,
                              UINT wRemoveMsg) {
    BOOL res = pfnPeekMessageW_Original(lpMsg, hWnd, wMsgFilterMin,
                                        wMsgFilterMax, wRemoveMsg);
    if (res && lpMsg && (wRemoveMsg & PM_REMOVE)) {
        ProcessEscapeKey(lpMsg);
    }
    return res;
}

// ---------------------------------------------------------------------------
// 2. Human-Readable File Size Logic
// ---------------------------------------------------------------------------

// Converts a raw byte count into a 2-decimal human-readable string (B to TB)
static void FormatBytesReadable(ULONGLONG bytes,
                                WCHAR* outBuf,
                                size_t outSize) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes < 1024ULL) {
        swprintf_s(outBuf, outSize, L"%llu B", bytes);
        return;
    }

    double dBytes = static_cast<double>(bytes);

    if (dBytes >= GB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f TB", dBytes / TB);
    } else if (dBytes >= MB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f GB", dBytes / GB);
    } else if (dBytes >= KB * 1023.995) {
        swprintf_s(outBuf, outSize, L"%.2f MB", dBytes / MB);
    } else {
        swprintf_s(outBuf, outSize, L"%.2f KB", dBytes / KB);
    }
}

// Parses a formatted numeric byte string while rejecting dates, ratios, or text
static bool ParseNumericBytes(const WCHAR* str, ULONGLONG* outBytes) {
    if (!str || str == LPSTR_TEXTCALLBACKW || !*str || !outBytes)
        return false;

    for (const WCHAR* p = str; *p; ++p) {
        if (*p == L':' || *p == L'-' || *p == L'/' || *p == L'%' ||
            iswalpha(*p)) {
            return false;
        }
    }

    ULONGLONG val = 0;
    bool hasDigits = false;

    for (const WCHAR* p = str; *p; ++p) {
        WCHAR ch = *p;

        if (iswspace(ch) || ch == L',' || ch == L'.' || ch == L'\'' ||
            ch == 0x00A0 || ch == 0x202F || ch == 0x200E || ch == 0x200F) {
            continue;
        }

        if (ch >= L'0' && ch <= L'9') {
            ULONGLONG digit = static_cast<ULONGLONG>(ch - L'0');
            if (val > (ULLONG_MAX - digit) / 10ULL)
                return false;
            val = (val * 10ULL) + digit;
            hasDigits = true;
        } else {
            return false;
        }
    }

    if (!hasDigits)
        return false;

    *outBytes = val;
    return true;
}

// Checks whether the given ListView column index corresponds to Size or Packed
static bool IsTargetSizeColumn(HWND hListView, int subItem) {
    if (subItem < 0)
        return false;

    HWND hHeader = ListView_GetHeader(hListView);
    if (!hHeader)
        return false;

    WCHAR textBuf[64] = {};
    HDITEMW hdi = {};
    hdi.mask = HDI_TEXT;
    hdi.pszText = textBuf;
    hdi.cchTextMax = ARRAYSIZE(textBuf);

    if (!SendMessageW(hHeader, HDM_GETITEMW, static_cast<WPARAM>(subItem),
                      reinterpret_cast<LPARAM>(&hdi))) {
        return false;
    }

    static const WCHAR* const kKeywords[] = {
        L"Size",    L"Packed", L"Размер",    L"Сжатый", L"Größe",
        L"Gepackt", L"Taille", L"Compressé", L"Tamaño", L"Comprimido",
    };

    for (const WCHAR* kw : kKeywords) {
        if (wcsstr(textBuf, kw) != nullptr)
            return true;
    }
    return false;
}

// Formats raw byte cells already present in an open ListView
static void FormatExistingListViewItems(HWND hListView) {
    if (!g_settings.readable_file_sizes || !IsWindow(hListView))
        return;

    if (GetWindowLongPtrW(hListView, GWL_STYLE) & LVS_OWNERDATA)
        return;

    HWND hHeader = ListView_GetHeader(hListView);
    if (!hHeader)
        return;

    int colCount =
        static_cast<int>(SendMessageW(hHeader, HDM_GETITEMCOUNT, 0, 0));
    int itemCount = ListView_GetItemCount(hListView);
    if (colCount <= 0 || itemCount <= 0)
        return;

    for (int subItem = 0; subItem < colCount; ++subItem) {
        if (!IsTargetSizeColumn(hListView, subItem))
            continue;

        for (int i = 0; i < itemCount; ++i) {
            WCHAR rawText[64] = {};
            LVITEMW lvi = {};
            lvi.iSubItem = subItem;
            lvi.pszText = rawText;
            lvi.cchTextMax = ARRAYSIZE(rawText);

            if (SendMessageW(hListView, LVM_GETITEMTEXTW,
                             static_cast<WPARAM>(i),
                             reinterpret_cast<LPARAM>(&lvi)) > 0) {
                ULONGLONG bytes = 0;
                if (ParseNumericBytes(rawText, &bytes)) {
                    WCHAR formatted[64] = {};
                    FormatBytesReadable(bytes, formatted, ARRAYSIZE(formatted));

                    lvi.pszText = formatted;
                    SendMessageW(hListView, LVM_SETITEMTEXTW,
                                 static_cast<WPARAM>(i),
                                 reinterpret_cast<LPARAM>(&lvi));
                }
            }
        }
    }
}

// Intercepts ListView item text updates to format Size and Packed values
static LRESULT CALLBACK ListViewSubclassProc(HWND hWnd,
                                             UINT uMsg,
                                             WPARAM wParam,
                                             LPARAM lParam,
                                             DWORD_PTR /*dwRefData*/) {
    if (g_settings.readable_file_sizes && lParam != 0) {
        if (uMsg == LVM_SETITEMTEXTW || uMsg == LVM_SETITEMW ||
            uMsg == LVM_INSERTITEMW) {
            LVITEMW* pItem = reinterpret_cast<LVITEMW*>(lParam);
            bool hasText =
                (uMsg == LVM_SETITEMTEXTW) || ((pItem->mask & LVIF_TEXT) != 0);

            if (hasText && pItem->pszText &&
                pItem->pszText != LPSTR_TEXTCALLBACKW) {
                ULONGLONG byteCount = 0;
                if (ParseNumericBytes(pItem->pszText, &byteCount) &&
                    IsTargetSizeColumn(hWnd, pItem->iSubItem)) {
                    WCHAR szFormatted[64] = {};
                    FormatBytesReadable(byteCount, szFormatted,
                                        ARRAYSIZE(szFormatted));

                    LPWSTR pOld = pItem->pszText;
                    pItem->pszText = szFormatted;
                    LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    pItem->pszText = pOld;
                    return res;
                }
            }
        }
    }

    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        g_subclassedListViews.erase(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclasses a SysListView32 window and tracks it for safe teardown
static void AttachListViewSubclass(HWND hWnd, bool formatExisting) {
    if (!hWnd || !IsWindow(hWnd))
        return;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, WC_LISTVIEWW) != 0) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        if (g_subclassedListViews.find(hWnd) != g_subclassedListViews.end())
            return;
    }

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, ListViewSubclassProc, 0)) {
        {
            std::lock_guard<std::mutex> lock(g_listViewMutex);
            g_subclassedListViews.insert(hWnd);
        }
        if (formatExisting) {
            FormatExistingListViewItems(hWnd);
        }
    }
}

static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM /*lParam*/) {
    AttachListViewSubclass(hWnd, true);
    return TRUE;
}

static BOOL CALLBACK EnumTopWindowsProc(HWND hWnd, LPARAM /*lParam*/) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        AttachListViewSubclass(hWnd, true);
        EnumChildWindows(hWnd, EnumChildProc, 0);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// 3. Window Creation Hook & Windhawk Lifecycle
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t pfnCreateWindowExW_Original = nullptr;

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
    HWND hWnd = pfnCreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hWnd) {
        AttachListViewSubclass(hWnd, false);
    }

    return hWnd;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing WinRAR Tweaks");
    LoadSettings();

    WindhawkUtils::SetFunctionHook(GetMessageW, GetMessageW_Hook,
                                   &pfnGetMessageW_Original);
    WindhawkUtils::SetFunctionHook(PeekMessageW, PeekMessageW_Hook,
                                   &pfnPeekMessageW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &pfnCreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumTopWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_settings.readable_file_sizes) {
        EnumWindows(EnumTopWindowsProc, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading WinRAR Tweaks");

    std::vector<HWND> listViews;
    {
        std::lock_guard<std::mutex> lock(g_listViewMutex);
        listViews.assign(g_subclassedListViews.begin(),
                         g_subclassedListViews.end());
        g_subclassedListViews.clear();
    }

    for (HWND hWnd : listViews) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hWnd, ListViewSubclassProc);
        }
    }
}
