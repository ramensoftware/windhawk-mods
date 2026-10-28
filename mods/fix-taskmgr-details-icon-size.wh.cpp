// ==WindhawkMod==
// @id                  fix-taskmgr-details-icon-size
// @name                Fix Task Manager Details Icon Size
// @name:zh-CN          修复任务管理器详细信息页图标大小
// @description         Fix task manager details icon size to normal.
// @description:zh-CN   修复任务管理器“详细信息”和“服务”页面的图标大小。
// @version             0.1
// @author              GF-Huang
// @github              https://github.com/GF-Huang
// @include             Taskmgr.exe
// @architecture        x86-64
// @compilerOptions     -lcomctl32 -lgdi32 -luser32
// @license             MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Task Manager Icon Sizes

![compared](https://i.imgur.com/3l0CqCH.png)

This mod lets you configure the Task Manager Details/Services icon sizes. You need to restart the task manager after modifying the settings.

I don't know from which version of Windows 11 onwards, the icons on the details page have become as small as ants.

------------------------------------------------------------------------------

此模组允许您配置任务管理器中“详细信息/服务”图标的尺寸。修改设置后需要重启任务管理器。

不知道 Windows 11 从哪个版本起，详细信息页面的图标小得跟蚂蚁一样。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cellSize: 32
  $name: Cell size
  $name:zh-CN: 单元格大小
  $description: The size of the cell occupied by the icon (in pixels, ranging from 24 to 256) is used as the margin around the icon.
  $description:zh-CN: 图标所占单元格的大小（像素，24-256），用作图标四周的留空。
  #! $min: 24
  #! $max: 256
- iconSize: 24
  $name: Icon size
  $name:zh-CN: 图标大小
  $description: Icon size in pixels; cannot exceed cell size (16-256)
  $description:zh-CN: 图标大小（像素，16-256），不能超过单元格大小
  #! $min: 16
  #! $max: 256
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <windhawk_utils.h>

namespace {

constexpr int kOldCellSize = 24;
int g_cellSize = 32;
constexpr int kOldIconSize = 16;
int g_iconSize = 24;

int GetIconOffset() {
    return (g_cellSize - g_iconSize) / 2;
}

thread_local unsigned g_wdcImageListCreationDepth;
thread_local unsigned g_wdcIconResizeDepth;

class ScopedCounter {
public:
    explicit ScopedCounter(unsigned& counter) : m_counter(counter) {
        ++m_counter;
    }

    ~ScopedCounter() {
        --m_counter;
    }

    ScopedCounter(const ScopedCounter&) = delete;
    ScopedCounter& operator=(const ScopedCounter&) = delete;

private:
    unsigned& m_counter;
};

using WdcCreateImageList_t = HRESULT(__fastcall*)(void* monitor);
using WdcAddResizedIcon_t = HRESULT(__fastcall*)(void* monitor, HICON icon,
                                                  int* imageIndex);
using ImageListCreate_t = HIMAGELIST(WINAPI*)(int, int, UINT, int, int);
using CreateCompatibleBitmap_t = HBITMAP(WINAPI*)(HDC, int, int);
using FillRect_t = int(WINAPI*)(HDC, const RECT*, HBRUSH);
using CreateBitmap_t = HBITMAP(WINAPI*)(int, int, UINT, UINT, const void*);
using PatBlt_t = BOOL(WINAPI*)(HDC, int, int, int, int, DWORD);
using DrawIconEx_t = BOOL(WINAPI*)(HDC, int, int, HICON, int, int, UINT,
                                   HBRUSH, UINT);

WdcCreateImageList_t WdcCreateImageList_Original;
WdcAddResizedIcon_t WdcAddResizedIcon_Original;
ImageListCreate_t ImageListCreate_Original;
CreateCompatibleBitmap_t CreateCompatibleBitmap_Original;
FillRect_t FillRect_Original;
CreateBitmap_t CreateBitmap_Original;
PatBlt_t PatBlt_Original;
DrawIconEx_t DrawIconEx_Original;

HRESULT __fastcall WdcCreateImageList_Hook(void* monitor) {
    ScopedCounter scope(g_wdcImageListCreationDepth);
    return WdcCreateImageList_Original(monitor);
}

HRESULT __fastcall WdcAddResizedIcon_Hook(void* monitor, HICON icon,
                                           int* imageIndex) {
    ScopedCounter scope(g_wdcIconResizeDepth);
    return WdcAddResizedIcon_Original(monitor, icon, imageIndex);
}

HIMAGELIST WINAPI ImageListCreate_Hook(int cx, int cy, UINT flags,
                                      int initialCount, int growCount) {
    if (g_wdcImageListCreationDepth && cx == kOldCellSize &&
        cy == kOldCellSize) {
        cx = g_cellSize;
        cy = g_cellSize;
    }

    return ImageListCreate_Original(cx, cy, flags, initialCount, growCount);
}

HBITMAP WINAPI CreateCompatibleBitmap_Hook(HDC hdc, int cx, int cy) {
    if (g_wdcIconResizeDepth && cx == kOldCellSize &&
        cy == kOldCellSize) {
        cx = g_cellSize;
        cy = g_cellSize;
    }

    return CreateCompatibleBitmap_Original(hdc, cx, cy);
}

int WINAPI FillRect_Hook(HDC hdc, const RECT* rect, HBRUSH brush) {
    if (g_wdcIconResizeDepth && rect && rect->right == kOldCellSize &&
        rect->bottom == kOldCellSize) {
        RECT adjusted = *rect;
        adjusted.right = g_cellSize;
        adjusted.bottom = g_cellSize;
        return FillRect_Original(hdc, &adjusted, brush);
    }

    return FillRect_Original(hdc, rect, brush);
}

HBITMAP WINAPI CreateBitmap_Hook(int width, int height, UINT planes,
                                 UINT bitCount, const void* bits) {
    if (g_wdcIconResizeDepth && width == kOldCellSize &&
        height == kOldCellSize) {
        width = g_cellSize;
        height = g_cellSize;
    }

    return CreateBitmap_Original(width, height, planes, bitCount, bits);
}

BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int width, int height,
                        DWORD rop) {
    if (g_wdcIconResizeDepth && width == kOldCellSize &&
        height == kOldCellSize) {
        width = g_cellSize;
        height = g_cellSize;
    }

    return PatBlt_Original(hdc, x, y, width, height, rop);
}

BOOL WINAPI DrawIconEx_Hook(HDC hdc, int x, int y, HICON icon, int cx, int cy,
                            UINT step, HBRUSH flickerFree, UINT flags) {
    if (g_wdcIconResizeDepth && x == 4 && y == 4 &&
        cx == kOldIconSize && cy == kOldIconSize) {
        x = GetIconOffset();
        y = GetIconOffset();
        cx = g_iconSize;
        cy = g_iconSize;
    }

    return DrawIconEx_Original(hdc, x, y, icon, cx, cy, step, flickerFree,
                               flags);
}

bool InstallApiHook(HMODULE module, const char* name, void* hook,
                    void** original) {
    if (!module) {
        Wh_Log(L"A required API module isn't loaded");
        return false;
    }

    FARPROC target = GetProcAddress(module, name);
    if (!target) {
        Wh_Log(L"Couldn't resolve a required API");
        return false;
    }

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(target), hook, original)) {
        Wh_Log(L"Couldn't install a required API hook");
        return false;
    }

    return true;
}

} // namespace

BOOL Wh_ModInit() {
    g_cellSize = std::clamp(Wh_GetIntSetting(L"cellSize"), 24, 256);
    g_iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 16, g_cellSize);
    HMODULE taskmgr = GetModuleHandleW(nullptr);
    if (!taskmgr) {
        Wh_Log(L"Couldn't get the Taskmgr.exe module handle");
        return FALSE;
    }

    // taskmgr.exe
    const WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            { L"public: long __cdecl WdcDataMonitor::CreateImageListWithResizedImage(void)" },
            &WdcCreateImageList_Original,
            WdcCreateImageList_Hook,
        },
        {
            { L"public: long __cdecl WdcDataMonitor::AddResizedIconToImageList(struct HICON__ *,int *)" },
            &WdcAddResizedIcon_Original,
            WdcAddResizedIcon_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(taskmgr, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Could not resolve WDC helpers; check the Taskmgr PDB symbols");
        return FALSE;
    }
    HMODULE comctl32 = GetModuleHandleW(L"comctl32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");

    bool ok = true;
    ok &= InstallApiHook(comctl32, "ImageList_Create",
                         reinterpret_cast<void*>(ImageListCreate_Hook),
                         reinterpret_cast<void**>(&ImageListCreate_Original));
    ok &= InstallApiHook(gdi32, "CreateCompatibleBitmap",
                         reinterpret_cast<void*>(CreateCompatibleBitmap_Hook),
                         reinterpret_cast<void**>(
                             &CreateCompatibleBitmap_Original));
    ok &= InstallApiHook(user32, "FillRect",
                         reinterpret_cast<void*>(FillRect_Hook),
                         reinterpret_cast<void**>(&FillRect_Original));
    ok &= InstallApiHook(gdi32, "CreateBitmap",
                         reinterpret_cast<void*>(CreateBitmap_Hook),
                         reinterpret_cast<void**>(&CreateBitmap_Original));
    ok &= InstallApiHook(gdi32, "PatBlt",
                         reinterpret_cast<void*>(PatBlt_Hook),
                         reinterpret_cast<void**>(&PatBlt_Original));
    ok &= InstallApiHook(user32, "DrawIconEx",
                         reinterpret_cast<void*>(DrawIconEx_Hook),
                         reinterpret_cast<void**>(&DrawIconEx_Original));

    if (!ok) {
        Wh_Log(L"One or more WDC image-size API hooks failed");
        return FALSE;
    }

    Wh_Log(L"PDB-symbol WDC hooks installed: %dpx cells, %dpx icons",
           g_cellSize, g_iconSize);
    return TRUE;
}
