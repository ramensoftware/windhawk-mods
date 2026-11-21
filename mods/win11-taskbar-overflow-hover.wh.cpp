// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar & System Tray Overflow on Hover
// @name:fr         Débordement de la barre des tâches et de la barre d'état au survol
// @name:ar         فتح القوائم المخفية لشريط المهام ومنطقة الإشعارات عند التمرير
// @name:es         Desbordamiento de barra de tareas y bandeja del sistema al pasar el ratón
// @name:de         Taskleisten- & System-Tray-Überlauf beim Darüberfahren
// @name:it         Overflow della barra delle applicazioni e della barra di sistema al passaggio del mouse
// @name:pt         Menu oculto da barra de tarefas e bandeja do sistema ao passar o mouse
// @name:ru         Открытие скрытых значков панели задач и трея при наведении
// @name:zh-CN      悬停打开任务栏和系统托盘溢出菜单
// @name:ja         ホバーでタスクバーとシステムトレイのオーバーフローを表示
// @description     Opens the Windows 11 taskbar app overflow and (optionally) system tray hidden icons menu on hover, auto-closing on exit or click.
// @description:fr  Ouvre automatiquement le menu de débordement des applications (et en option les icônes cachées de la barre d'état) de Windows 11 au survol de la souris.
// @description:ar  يفتح قائمة التطبيقات الإضافية (واختيارياً الأيقونات المخفية في منطقة الإشعارات) في شريط مهام ويندوز 11 تلقائياً عند تمرير مؤشر الفأرة.
// @description:es  Abre automáticamente el menú de aplicaciones desbordadas (y opcionalmente los iconos ocultos de la bandeja) en Windows 11 al pasar el cursor.
// @description:de  Öffnet das Überlaufmenü der Taskleiste (und optional die ausgeblendeten Symbole im System-Tray) von Windows 11 beim Darüberfahren mit der Maus.
// @description:it  Apre automaticamente il menu di overflow delle app (e opzionalmente le icone nascoste della barra di sistema) di Windows 11 al passaggio del mouse.
// @description:pt  Abre automaticamente o menu de aplicativos excedentes da barra de tarefas (e opcionalmente os ícones ocultos da bandeja) no Windows 11 ao passar o mouse.
// @description:ru  Автоматически открывает меню переполнения панели задач (и опционально скрытые значки системного трея) Windows 11 при наведении курсора.
// @description:zh-CN 鼠标悬停时自动打开 Windows 11 任务栏应用溢出菜单（以及可选的系统托盘隐藏图标菜单），移开或点击时自动关闭。
// @description:ja  マウスホバーで Windows 11 のタスクバーアプリアイコンのオーバーフロー（およびオプションでシステムトレイの隠れているインジケーター）を自動的に開きます。
// @version         1.2.0
// @author          Cougar16
// @github          https://github.com/Cougar16
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar & System Tray Overflow on Hover 🚀

A zero-click QoL upgrade for Windows 11. Hover over the taskbar app overflow button
(and optionally the system tray chevron) to instantly expand hidden items without clicking.

![Hover overflow menu](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/Assets/Hover-Overflow-Menu.png)

✨ Features:
- Zero-Click App Overflow Open: Auto-expands the running app overflow menu ("...") simply by hovering.
- Optional System Tray Chevron Support: Can also auto-expand the system tray hidden icons flyout (the "^" chevron) in a single unified process (disabled by default in Settings to avoid conflicts with existing tray mods).
- Configurable Toggles: Individually enable or disable hover activation for either menu in the mod settings.
- Auto-Dismiss: Smoothly closes the active menu the moment your cursor leaves the button and flyout hit area, complete with debounced overshoot protection so quick mouse sweeps won't cause flickering.
- Instant Item Launch: Automatically dismisses the flyout the millisecond you launch an app or click an item inside it, bypassing native Windows UI delay while keeping context menus intact.
- Multi-DPI & Multi-Language Support: Per-Monitor V2 DPI awareness guarantees pixel-accurate coordinate mapping on high-scale displays, with native localization across Windows editions.

## Compatibility & Comparison with `tray-hover-expand`
For the system tray hidden icons chevron (`^`), the Windhawk catalog also offers **[Tray hover expand](https://windhawk.net/mods/tray-hover-expand)** (`tray-hover-expand`), which provides additional tray-specific options such as fullscreen suppression, tooltip hiding, and custom keyword lists.

**How this mod's optional tray handler differs:**
- **Unified Single-Process Polling:** Handles both the taskbar app overflow (`...`) and the system tray chevron (`^`) in a single UIA monitor thread, cleanly coordinating transitions when moving directly between the two buttons.
- **Strict Chevron Disambiguation (Build 26220+ Safe):** On newer Windows 11 builds, all tray action buttons (Touch Keyboard, Emoji/IME, and Language Switcher) share `AutomationId="SystemTrayIcon"` and `ClassName="SystemTray.NormalButton"`. Rather than matching on class and ID alone, this mod inspects the button's cached accessible name and raw XAML subtree for Segoe Fluent chevron glyphs (`\uE70D`, `\uE70E`, etc.) so neighboring tray buttons are never accidentally triggered.

> ⚠️ **Important:** Do **not** enable **"Enable System Tray Chevron on Hover"** in this mod while `tray-hover-expand` is also enabled. Because the Windows 11 system tray chevron uses `IUIAutomationInvokePattern` (which acts as a toggle), running two tray hover mods simultaneously will cause them to fight and re-open the flyout when trying to close it. For this reason, the system tray chevron option is **disabled by default**.

Developed by Cougar16 for the Windows customization community.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAppOverflow: true
  $name: Enable App Overflow on Hover
  $name:fr: Activer le débordement des applications au survol
  $name:ar: تفعيل قائمة التطبيقات الإضافية عند التمرير
  $name:es: Activar menú de aplicaciones desbordadas al pasar el ratón
  $name:de: App-Überlaufmenü beim Darüberfahren aktivieren
  $name:it: Attiva menu di overflow delle app al passaggio del mouse
  $name:pt: Ativar menu de aplicativos excedentes ao passar o mouse
  $name:ru: Открывать меню переполнения приложений при наведении
  $name:zh-CN: 悬停时打开任务栏应用溢出菜单
  $name:ja: ホバーでアプリのオーバーフローメニューを開く
  $description: Auto-open the running taskbar apps overflow menu ("...") when hovering.
  $description:fr: Ouvre automatiquement le menu des applications supplémentaires (« ... ») de la barre des tâches au survol.
  $description:ar: فتح قائمة تطبيقات شريط المهام الإضافية ("...") تلقائياً عند تمرير الفأرة فوقها.
  $description:es: Abre automáticamente el menú de aplicaciones adicionales ("...") en la barra de tareas al pasar el cursor.
  $description:de: Öffnet das Überlaufmenü für laufende Taskleisten-Apps ("...") automatisch beim Darüberfahren.
  $description:it: Apre automaticamente il menu delle app aggiuntive ("...") sulla barra delle applicazioni al passaggio del mouse.
  $description:pt: Abre automaticamente o menu de aplicativos em execução ("...") na barra de tarefas ao passar o cursor.
  $description:ru: Автоматически открывать меню дополнительных запущенных приложений («...») при наведении курсора.
  $description:zh-CN: 鼠标悬停在任务栏“...”按钮上时自动展开隐藏的运行中应用。
  $description:ja: タスクバーの「...」ボタンにカーソルを合わせたときに実行中のアプリ一覧を自動的に開きます。
- enableTrayChevron: false
  $name: Enable System Tray Chevron on Hover
  $name:fr: Activer le chevron des icônes cachées au survol
  $name:ar: تفعيل سهم الأيقونات المخفية في منطقة الإشعارات عند التمرير
  $name:es: Activar flecha de iconos ocultos de la bandeja al pasar el ratón
  $name:de: System-Tray-Pfeil (Ausgeblendete Symbole) beim Darüberfahren aktivieren
  $name:it: Attiva freccia delle icone nascoste della barra di sistema al passaggio del mouse
  $name:pt: Ativar seta de ícones ocultos da bandeja do sistema ao passar o mouse
  $name:ru: Открывать скрытые значки системного трея при наведении
  $name:zh-CN: 悬停时打开系统托盘隐藏图标菜单
  $name:ja: ホバーでシステムトレイの隠れているアイコンを開く
  $description: Auto-open the system tray hidden icons flyout ("^") when hovering. Do not enable if you already use the "Tray hover expand" mod.
  $description:fr: Ouvre automatiquement le menu des icônes cachées (« ^ ») au survol. Ne pas activer si vous utilisez déjà le mod « Tray hover expand ».
  $description:ar: فتح نافذة الأيقونات المخفية ("^") في منطقة الإشعارات تلقائياً عند التمرير. لا تقم بتفعيله إذا كنت تستخدم إضافة "Tray hover expand".
  $description:es: Abre automáticamente el menú de iconos ocultos ("^") al pasar el cursor. No activar si ya usas el mod "Tray hover expand".
  $description:de: Öffnet das Flyout für ausgeblendete Symbole ("^") beim Darüberfahren. Nicht aktivieren, wenn bereits "Tray hover expand" genutzt wird.
  $description:it: Apre automaticamente il riquadro delle icone nascoste ("^") al passaggio del mouse. Non attivare se usi già la mod "Tray hover expand".
  $description:pt: Abre automaticamente o menu de ícones ocultos ("^") ao passar o cursor. Não ative se você já usa o mod "Tray hover expand".
  $description:ru: Автоматически открывать всплывающее окно скрытых значков трея («^») при наведении. Не включайте, если уже используете мод «Tray hover expand».
  $description:zh-CN: 鼠标悬停在系统托盘“^”按钮上时自动展开隐藏图标菜单。如果已启用“Tray hover expand”模组，请勿同时开启。
  $description:ja: システムトレイの「^」ボタンにカーソルを合わせたときに隠れているインジケーターを自動的に開きます。「Tray hover expand」を使用中の場合は無効にしてください。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>

#include <cwchar>

// Time the cursor must rest on the overflow button before the menu opens.
constexpr int kHoverDelayMs = 100;

// Time the cursor must stay outside the button + menu area before the menu
// closes (a tiny debounce so a brief overshoot does not make it flicker).
constexpr int kCloseDelayMs = 150;

// Signalled on unload to stop the monitor thread (also interrupts its sleep).
HANDLE g_stopEvent = nullptr;
HANDLE g_hThread = nullptr;

// Mod settings state
struct ModSettings {
    bool enableAppOverflow = true;
    bool enableTrayChevron = false;
} g_settings;

void LoadSettings() {
    g_settings.enableAppOverflow = Wh_GetIntSetting(L"enableAppOverflow") != 0;
    g_settings.enableTrayChevron = Wh_GetIntSetting(L"enableTrayChevron") != 0;
}

// After toggling the button the flyout needs a moment to appear. During this
// grace period the cursor being away from it does not close it.
constexpr ULONGLONG kOpenGraceMs = 600;

// After a click inside the flyout, wait this long before deciding whether the
// click opened a popup menu (keep the flyout) or not (dismiss the flyout).
constexpr ULONGLONG kClickSettleMs = 100;

// The app overflow ("...") flyout and the thumbnail previews are each hosted in
// a window of this class on the taskbar's thread. The tray's "Show hidden icons"
// popup is a different window, TopLevelWindowForOverflowXamlIsland.
constexpr wchar_t kXamlHostClass[] = L"XamlExplorerHostIslandWindow";
constexpr wchar_t kTrayOverflowHostClass[] = L"TopLevelWindowForOverflowXamlIsland";
constexpr wchar_t kLegacyTrayOverflowHostClass[] = L"NotifyIconOverflowWindow";

// Return true only when the point belongs to a taskbar top-level window
// owned by the same process as the primary taskbar.
bool IsTaskbarWindowAtPoint(POINT pt, DWORD* taskbarPidOut = nullptr) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return false;
    }

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) {
        return false;
    }

    wchar_t className[256] = {};
    if (!GetClassNameW(hRoot, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return false;
    }

    HWND hPrimaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hPrimaryTaskbar) {
        return false;
    }

    DWORD primaryPid = 0;
    DWORD pointPid = 0;
    GetWindowThreadProcessId(hPrimaryTaskbar, &primaryPid);
    GetWindowThreadProcessId(hRoot, &pointPid);

    if (!primaryPid || pointPid != primaryPid) {
        return false;
    }

    if (taskbarPidOut) {
        *taskbarPidOut = primaryPid;
    }

    return true;
}

// Gets the on-screen bounds of a visible XAML host. A host can span the whole
// monitor with a window region limiting it to its UI, so its window rect alone
// says nothing about where that UI is.
bool GetXamlHostBounds(HWND hWnd, RECT* bounds) {
    RECT windowRect = {};
    if (!IsWindowVisible(hWnd) || !GetWindowRect(hWnd, &windowRect)) {
        return false;
    }

    *bounds = windowRect;

    HRGN region = CreateRectRgn(0, 0, 0, 0);
    if (region) {
        // GetWindowRgn fails for a window without a region.
        if (GetWindowRgn(hWnd, region) != ERROR &&
            GetRgnBox(region, bounds) != ERROR) {
            OffsetRect(bounds, windowRect.left, windowRect.top);
        }
        DeleteObject(region);
    }

    return !IsRectEmpty(bounds);
}

// Returns true if the system tray's "Show hidden icons" flyout window
// (TopLevelWindowForOverflowXamlIsland) is currently open and visible.
bool IsTrayOverflowWindowOpen(DWORD taskbarPid, RECT* boundsOut = nullptr) {
    const wchar_t* classes[] = { kTrayOverflowHostClass, kLegacyTrayOverflowHostClass };
    for (const wchar_t* cls : classes) {
        HWND hWnd = nullptr;
        while ((hWnd = FindWindowExW(nullptr, hWnd, cls, nullptr)) != nullptr) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid != taskbarPid) {
                continue;
            }
            RECT bounds = {};
            if (GetXamlHostBounds(hWnd, &bounds)) {
                if (boundsOut) {
                    *boundsOut = bounds;
                }
                return true;
            }
        }
    }
    return false;
}

// Scans the visible XAML hosts on the taskbar's UI thread. The thread match
// excludes the other XAML islands that live in Explorer (Alt+Tab, Task View...).
// Returns true if the point is over one of them. flyoutBounds receives the
// bounds of the host nearest the overflow button: that is the flyout, since the
// preview of an icon inside the flyout appears beyond it.
bool QueryXamlHosts(HWND hTaskbar,
                    const RECT& chevronRect,
                    POINT pt,
                    RECT* flyoutBounds) {
    *flyoutBounds = {};

    DWORD taskbarPid = 0;
    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbar, &taskbarPid);
    if (!taskbarPid || !taskbarThreadId) {
        return false;
    }

    const LONGLONG chevronCenterX = (chevronRect.left + chevronRect.right) / 2;
    const LONGLONG chevronCenterY = (chevronRect.top + chevronRect.bottom) / 2;

    bool overHost = false;
    LONGLONG nearestDistance = -1;

    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, kXamlHostClass, nullptr)) !=
           nullptr) {
        DWORD pid = 0;
        DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
        if (pid != taskbarPid || threadId != taskbarThreadId) {
            continue;
        }

        RECT bounds = {};
        if (!GetXamlHostBounds(hWnd, &bounds)) {
            continue;
        }

        overHost = overHost || PtInRect(&bounds, pt);

        const LONGLONG dx = (bounds.left + bounds.right) / 2 - chevronCenterX;
        const LONGLONG dy = (bounds.top + bounds.bottom) / 2 - chevronCenterY;
        const LONGLONG distance = dx * dx + dy * dy;
        if (nearestDistance < 0 || distance < nearestDistance) {
            nearestDistance = distance;
            *flyoutBounds = bounds;
        }
    }

    // Also scan the system tray hidden-icons flyout window
    // (TopLevelWindowForOverflowXamlIsland / NotifyIconOverflowWindow).
    const wchar_t* trayClasses[] = { kTrayOverflowHostClass, kLegacyTrayOverflowHostClass };
    for (const wchar_t* cls : trayClasses) {
        HWND hTrayWnd = nullptr;
        while ((hTrayWnd = FindWindowExW(nullptr, hTrayWnd, cls, nullptr)) != nullptr) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hTrayWnd, &pid);
            if (pid != taskbarPid) {
                continue;
            }

            RECT bounds = {};
            if (!GetXamlHostBounds(hTrayWnd, &bounds)) {
                continue;
            }

            overHost = overHost || PtInRect(&bounds, pt);

            const LONGLONG dx = (bounds.left + bounds.right) / 2 - chevronCenterX;
            const LONGLONG dy = (bounds.top + bounds.bottom) / 2 - chevronCenterY;
            const LONGLONG distance = dx * dx + dy * dy;
            if (nearestDistance < 0 || distance < nearestDistance) {
                nearestDistance = distance;
                *flyoutBounds = bounds;
            }
        }
    }

    return overHost;
}

// True while a popup menu (for example a tray icon's context menu) is showing.
bool IsPopupMenuOpen() {
    GUITHREADINFO gti = {};
    gti.cbSize = sizeof(gti);
    if (GetGUIThreadInfo(0, &gti) &&
        (gti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE))) {
        return true;
    }

    HWND hMenu = nullptr;
    while ((hMenu = FindWindowExW(nullptr, hMenu, L"#32768", nullptr)) != nullptr) {
        if (IsWindowVisible(hMenu)) {
            return true;
        }
    }

    return false;
}

// Case-insensitive substring helper for fallback localized name matching.
bool ContainsInsensitive(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle) return false;
    size_t hLen = wcslen(haystack);
    size_t nLen = wcslen(needle);
    if (nLen == 0 || nLen > hLen) return false;
    for (size_t i = 0; i <= hLen - nLen; ++i) {
        if (_wcsnicmp(haystack + i, needle, nLen) == 0) {
            return true;
        }
    }
    return false;
}

// Returns true if the unicode codepoint is one of the Segoe Fluent Icons / MDL2
// chevron arrow glyphs used by the Windows 11 system tray hidden-icons button.
bool IsChevronGlyphChar(wchar_t ch) {
    switch (ch) {
        case 0xE70D: // ChevronDown
        case 0xE70E: // ChevronUp
        case 0xE971: // ChevronUpSmall
        case 0xE972: // ChevronDownSmall
        case 0xE96D: // ChevronUpMed
        case 0xE96E: // ChevronDownMed
        case 0xE014: // Legacy ChevronUp
        case 0xE015: // Legacy ChevronDown
        case 0xE018:
        case 0xE019:
            return true;
        default:
            return false;
    }
}

// Inspects the raw UIA subtree of a SystemTray.NormalButton to see if its icon
// glyph is the chevron arrow (and NOT the Touch Keyboard \uE765, Emoji, or Language text "FRA").
bool HasChevronGlyphInSubtree(IUIAutomation* automation,
                              IUIAutomationElement* buttonElement,
                              IUIAutomationCacheRequest* cacheReq) {
    if (!automation || !buttonElement) {
        return false;
    }

    IUIAutomationTreeWalker* rawWalker = nullptr;
    if (FAILED(automation->get_RawViewWalker(&rawWalker)) || !rawWalker) {
        return false;
    }

    bool foundChevronGlyph = false;

    auto getNameFast = [&](IUIAutomationElement* el, BSTR* outName) -> HRESULT {
        if (cacheReq && SUCCEEDED(el->get_CachedName(outName)) && *outName) {
            return S_OK;
        }
        return el->get_CurrentName(outName);
    };

    auto getFirstChildFast = [&](IUIAutomationElement* parent, IUIAutomationElement** child) {
        if (cacheReq) {
            return rawWalker->GetFirstChildElementBuildCache(parent, cacheReq, child);
        }
        return rawWalker->GetFirstChildElement(parent, child);
    };

    auto getNextSiblingFast = [&](IUIAutomationElement* node, IUIAutomationElement** next) {
        if (cacheReq) {
            return rawWalker->GetNextSiblingElementBuildCache(node, cacheReq, next);
        }
        return rawWalker->GetNextSiblingElement(node, next);
    };

    IUIAutomationElement* child1 = nullptr;
    getFirstChildFast(buttonElement, &child1);
    while (child1 && !foundChevronGlyph) {
        BSTR name1 = nullptr;
        if (SUCCEEDED(getNameFast(child1, &name1)) && name1) {
            if (wcslen(name1) == 1 && IsChevronGlyphChar(name1[0])) {
                foundChevronGlyph = true;
            }
            SysFreeString(name1);
        }

        if (!foundChevronGlyph) {
            IUIAutomationElement* child2 = nullptr;
            getFirstChildFast(child1, &child2);
            while (child2 && !foundChevronGlyph) {
                BSTR name2 = nullptr;
                if (SUCCEEDED(getNameFast(child2, &name2)) && name2) {
                    if (wcslen(name2) == 1 && IsChevronGlyphChar(name2[0])) {
                        foundChevronGlyph = true;
                    }
                    SysFreeString(name2);
                }

                if (!foundChevronGlyph) {
                    IUIAutomationElement* child3 = nullptr;
                    getFirstChildFast(child2, &child3);
                    while (child3 && !foundChevronGlyph) {
                        BSTR name3 = nullptr;
                        if (SUCCEEDED(getNameFast(child3, &name3)) && name3) {
                            if (wcslen(name3) == 1 && IsChevronGlyphChar(name3[0])) {
                                foundChevronGlyph = true;
                            }
                            SysFreeString(name3);
                        }
                        IUIAutomationElement* next3 = nullptr;
                        getNextSiblingFast(child3, &next3);
                        child3->Release();
                        child3 = next3;
                    }
                }

                IUIAutomationElement* next2 = nullptr;
                getNextSiblingFast(child2, &next2);
                child2->Release();
                child2 = next2;
            }
        }

        IUIAutomationElement* next1 = nullptr;
        getNextSiblingFast(child1, &next1);
        child1->Release();
        child1 = next1;
    }

    rawWalker->Release();
    return foundChevronGlyph;
}

// Checks if the element is the taskbar's app overflow button ("Chevron" / "OverflowButton")
// OR strictly the system tray's hidden icons chevron ("Afficher les icônes cachées" / "Show hidden icons").
// Excludes Touch Keyboard, Emoji/IME, Pen menu, and Language switcher ("FRA") buttons.
bool IsOverflowAutomationId(IUIAutomation* automation,
                            IUIAutomationElement* element,
                            IUIAutomationCacheRequest* cacheReq,
                            bool useCachedProps,
                            bool* isTrayChevronOut = nullptr) {
    if (isTrayChevronOut) {
        *isTrayChevronOut = false;
    }
    if (!element) {
        return false;
    }

    BSTR autoId = nullptr;
    BSTR className = nullptr;
    if (useCachedProps) {
        element->get_CachedAutomationId(&autoId);
        element->get_CachedClassName(&className);
    } else {
        element->get_CurrentAutomationId(&autoId);
        element->get_CurrentClassName(&className);
    }

    bool isAppOverflow = false;
    bool isTrayChevron = false;

    // 1. Original app overflow check (subject to user setting)
    if (autoId &&
        (wcscmp(autoId, L"Chevron") == 0 ||
         wcscmp(autoId, L"OverflowButton") == 0)) {
        if (g_settings.enableAppOverflow) {
            isAppOverflow = true;
        }
    }

    // 2. Strictly System Tray Hidden Icons Chevron check (subject to user setting):
    // Note: Touch Keyboard, Emoji, and Language Switcher ALSO have AutomationId="SystemTrayIcon"
    // and ClassName="SystemTray.NormalButton", so we must NEVER match on ClassName alone!
    if (!isAppOverflow && g_settings.enableTrayChevron) {
        if (autoId &&
            (wcscmp(autoId, L"ChevronButton") == 0 ||
             wcscmp(autoId, L"NotificationChevron") == 0)) {
            isTrayChevron = true;
        } else if (className && wcscmp(className, L"SystemTray.ChevronButton") == 0) {
            isTrayChevron = true;
        } else if (autoId && wcscmp(autoId, L"SystemTrayIcon") == 0 &&
                   className && wcscmp(className, L"SystemTray.NormalButton") == 0) {
            // Check A (Fastest - zero extra IPC calls!): Multi-language accessible Name check
            BSTR name = nullptr;
            HRESULT hrName = useCachedProps ? element->get_CachedName(&name)
                                            : element->get_CurrentName(&name);
            if (SUCCEEDED(hrName) && name) {
                if (ContainsInsensitive(name, L"icônes cachées") ||      // French
                    ContainsInsensitive(name, L"icones cachees") ||      // French (unaccented)
                    ContainsInsensitive(name, L"hidden icons") ||        // English
                    ContainsInsensitive(name, L"الأيقونات المخفية") ||    // Arabic
                    ContainsInsensitive(name, L"الرموز المخفية") ||       // Arabic (alt)
                    ContainsInsensitive(name, L"ausgeblendete Symbole") || // German
                    ContainsInsensitive(name, L"iconos ocultos") ||      // Spanish
                    ContainsInsensitive(name, L"icone nascoste") ||      // Italian
                    ContainsInsensitive(name, L"ícones ocultos") ||      // Portuguese
                    ContainsInsensitive(name, L"verborgen pictogrammen") || // Dutch
                    ContainsInsensitive(name, L"ukryte ikony") ||        // Polish
                    ContainsInsensitive(name, L"gizli simgeleri") ||     // Turkish
                    ContainsInsensitive(name, L"скрытые значки") ||      // Russian
                    ContainsInsensitive(name, L"隠れているインジケーター") || // Japanese
                    ContainsInsensitive(name, L"숨겨진 아이콘") ||         // Korean
                    ContainsInsensitive(name, L"隐藏的图标") ||           // Chinese (Simplified)
                    ContainsInsensitive(name, L"隱藏的圖示") ||           // Chinese (Traditional)
                    wcscmp(name, L"Masquer") == 0 ||
                    wcscmp(name, L"Hide") == 0 ||
                    wcscmp(name, L"إخفاء") == 0 ||
                    wcscmp(name, L"Ocultar") == 0 ||
                    wcscmp(name, L"Ausblenden") == 0 ||
                    wcscmp(name, L"Nascondi") == 0 ||
                    wcscmp(name, L"Скрыть") == 0) {
                    isTrayChevron = true;
                }
                SysFreeString(name);
            }

            // Check B: Universal Segoe Fluent Chevron glyph check for any other Windows language
            if (!isTrayChevron && HasChevronGlyphInSubtree(automation, element, cacheReq)) {
                isTrayChevron = true;
            }
        }
    }

    if (className) {
        SysFreeString(className);
    }
    if (autoId) {
        SysFreeString(autoId);
    }

    if (isTrayChevronOut) {
        *isTrayChevronOut = isTrayChevron;
    }

    return isAppOverflow || isTrayChevron;
}

// Walk the UIA control tree only after the Win32 taskbar check passed.
// Uses a batched CacheRequest so all properties are fetched in a single cross-process call.
bool FindTaskbarOverflowElement(
    IUIAutomation* automation,
    IUIAutomationCacheRequest* cacheReq,
    POINT pt,
    DWORD taskbarPid,
    IUIAutomationElement** elementOut,
    IUIAutomationTogglePattern** toggleOut,
    IUIAutomationInvokePattern** invokeOut = nullptr,
    IUIAutomationExpandCollapsePattern** expandCollapseOut = nullptr,
    bool* isTrayChevronOut = nullptr) {

    *elementOut = nullptr;
    *toggleOut = nullptr;
    if (invokeOut) *invokeOut = nullptr;
    if (expandCollapseOut) *expandCollapseOut = nullptr;
    if (isTrayChevronOut) *isTrayChevronOut = false;

    IUIAutomationElement* pointElement = nullptr;
    bool usedCache = false;
    HRESULT hr = E_FAIL;

    if (cacheReq) {
        hr = automation->ElementFromPointBuildCache(pt, cacheReq, &pointElement);
        if (SUCCEEDED(hr) && pointElement) {
            usedCache = true;
        }
    }
    if (!pointElement) {
        hr = automation->ElementFromPoint(pt, &pointElement);
    }
    if (FAILED(hr) || !pointElement) {
        return false;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    hr = automation->get_ControlViewWalker(&walker);
    if (FAILED(hr) || !walker) {
        pointElement->Release();
        return false;
    }

    IUIAutomationElement* node = pointElement;
    node->AddRef();

    bool found = false;

    for (int i = 0; i < 10 && node; ++i) {
        int elementPid = 0;
        hr = usedCache ? node->get_CachedProcessId(&elementPid)
                       : node->get_CurrentProcessId(&elementPid);

        // Do not inspect or interact with elements from another process.
        if (FAILED(hr) || elementPid != static_cast<int>(taskbarPid)) {
            node->Release();
            node = nullptr;
            break;
        }

        bool isTrayChevron = false;
        if (IsOverflowAutomationId(automation, node, cacheReq, usedCache, &isTrayChevron)) {
            IUIAutomationTogglePattern* togglePattern = nullptr;
            node->GetCurrentPatternAs(
                UIA_TogglePatternId, IID_PPV_ARGS(&togglePattern));

            IUIAutomationInvokePattern* invokePattern = nullptr;
            IUIAutomationExpandCollapsePattern* expandPattern = nullptr;

            // The system tray chevron does NOT expose TogglePattern on Windows 11;
            // it uses InvokePattern (or ExpandCollapsePattern).
            if (isTrayChevron) {
                if (invokeOut) {
                    node->GetCurrentPatternAs(
                        UIA_InvokePatternId, IID_PPV_ARGS(&invokePattern));
                }
                if (expandCollapseOut) {
                    node->GetCurrentPatternAs(
                        UIA_ExpandCollapsePatternId, IID_PPV_ARGS(&expandPattern));
                }
            }

            if (togglePattern || invokePattern || expandPattern) {
                *elementOut = node;
                *toggleOut = togglePattern;
                if (invokeOut) *invokeOut = invokePattern;
                if (expandCollapseOut) *expandCollapseOut = expandPattern;
                if (isTrayChevronOut) *isTrayChevronOut = isTrayChevron;
                node = nullptr; // Ownership transferred to elementOut.
                found = true;
                break;
            }
        }

        IUIAutomationElement* parent = nullptr;
        if (usedCache && cacheReq) {
            hr = walker->GetParentElementBuildCache(node, cacheReq, &parent);
        } else {
            hr = walker->GetParentElement(node, &parent);
        }
        node->Release();
        node = SUCCEEDED(hr) ? parent : nullptr;
    }

    if (node) {
        node->Release();
    }

    walker->Release();
    pointElement->Release();
    return found;
}

// The app overflow flyout is open while its TogglePattern state is On.
// The system tray chevron flyout is open while TopLevelWindowForOverflowXamlIsland is visible.
bool IsOverflowOpen(IUIAutomationTogglePattern* toggle,
                    bool isTrayChevron = false,
                    DWORD taskbarPid = 0) {
    if (isTrayChevron && taskbarPid != 0) {
        return IsTrayOverflowWindowOpen(taskbarPid);
    }
    if (!toggle) {
        return false;
    }
    ToggleState state = ToggleState_Off;
    return SUCCEEDED(toggle->get_CurrentToggleState(&state)) &&
           state == ToggleState_On;
}

DWORD WINAPI HoverMonitorThread(LPVOID) {
    // 1. Ensure physical screen coordinates from GetCursorPos match UIA ElementFromPoint
    // on DPI-scaled displays (125%, 150%, etc.).
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // 2. Use COINIT_MULTITHREADED (MTA) so COM never creates a hidden STA window
    // whose unpumped message queue would stall cross-process UIA calls after a few minutes.
    HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(coHr)) {
        coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(coHr)) {
            Wh_Log(L"CoInitializeEx failed: 0x%08X", coHr);
            return 1;
        }
    }

    IUIAutomation* automation = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_IUIAutomation, reinterpret_cast<void**>(&automation));

    if (FAILED(hr) || !automation) {
        Wh_Log(L"Failed to initialize UI Automation: 0x%08X", hr);
        CoUninitialize();
        return 1;
    }

    // Build a reusable UIA CacheRequest so ProcessId, AutomationId, ClassName,
    // Name, and BoundingRectangle are fetched in ONE single IPC round-trip.
    IUIAutomationCacheRequest* cacheReq = nullptr;
    if (SUCCEEDED(automation->CreateCacheRequest(&cacheReq)) && cacheReq) {
        cacheReq->AddProperty(UIA_ProcessIdPropertyId);
        cacheReq->AddProperty(UIA_AutomationIdPropertyId);
        cacheReq->AddProperty(UIA_ClassNamePropertyId);
        cacheReq->AddProperty(UIA_NamePropertyId);
        cacheReq->AddProperty(UIA_BoundingRectanglePropertyId);
    }

    int currentHoverTime = 0;
    int currentCloseHoverTime = 0;
    bool menuOpened = false;      // True only while the mod owns an open flyout.
    bool hoverConsumed = false;   // No opening until the cursor leaves the button.
    bool wasMouseDown = false;
    bool clickPending = false;    // A click inside the flyout is waiting to dismiss it.
    ULONGLONG openedAt = 0;
    ULONGLONG clickAt = 0;

    // Cursor tracking optimization: avoid hammering UIA ElementFromPoint when
    // the cursor hasn't moved and isn't currently hovering an overflow button.
    POINT lastQueriedPt = { -10000, -10000 };
    bool lastPtWasOverflow = false;

    IUIAutomationTogglePattern* chevron = nullptr;               // Set while menuOpened (app overflow).
    IUIAutomationInvokePattern* chevronInvoke = nullptr;         // Set while menuOpened (tray chevron).
    IUIAutomationExpandCollapsePattern* chevronExpand = nullptr; // Optional tray chevron pattern.
    bool activeIsTrayChevron = false;
    DWORD activeTaskbarPid = 0;
    RECT chevronRect = {};                                       // Button bounds, captured when opened.

    // Forget everything about the flyout the mod opened.
    auto resetOwnership = [&]() {
        menuOpened = false;
        clickPending = false;
        currentHoverTime = 0;
        currentCloseHoverTime = 0;
        activeIsTrayChevron = false;
        activeTaskbarPid = 0;
        if (chevron) {
            chevron->Release();
            chevron = nullptr;
        }
        if (chevronInvoke) {
            chevronInvoke->Release();
            chevronInvoke = nullptr;
        }
        if (chevronExpand) {
            chevronExpand->Release();
            chevronExpand = nullptr;
        }
    };

    auto triggerAction = [&](IUIAutomationTogglePattern* tog,
                             IUIAutomationInvokePattern* inv,
                             IUIAutomationExpandCollapsePattern* exp,
                             bool opening) -> HRESULT {
        if (tog) {
            return tog->Toggle();
        }
        if (exp) {
            HRESULT hrExp = opening ? exp->Expand() : exp->Collapse();
            if (SUCCEEDED(hrExp)) {
                return hrExp;
            }
        }
        if (inv) {
            return inv->Invoke();
        }
        return E_FAIL;
    };

    auto closeFlyout = [&]() {
        // Guard against the Invoke-toggle hazard: only fire the close action if
        // the flyout is still actually open right now.
        if (IsOverflowOpen(chevron, activeIsTrayChevron, activeTaskbarPid)) {
            HRESULT closeHr = triggerAction(chevron, chevronInvoke, chevronExpand, false);
            if (FAILED(closeHr)) {
                Wh_Log(L"Failed to close the overflow flyout: 0x%08X", closeHr);
            }
        }
        resetOwnership();
    };

    // The 50 ms wait doubles as the poll interval and is interrupted by the
    // stop event, so unloading never has to wait for a sleep to finish.
    while (WaitForSingleObject(g_stopEvent, 50) == WAIT_TIMEOUT) {
        const ULONGLONG now = GetTickCount64();

        // The flyout was closed some other way (Win key, Alt+Tab, Escape typed
        // by the user, click elsewhere...). Stop considering it ours.
        // For the tray chevron, wait until kOpenGraceMs has passed before
        // declaring it externally closed so the XAML island has time to appear.
        if (menuOpened) {
            bool pastGrace = !activeIsTrayChevron || (now - openedAt >= kOpenGraceMs);
            if (pastGrace && !IsOverflowOpen(chevron, activeIsTrayChevron, activeTaskbarPid)) {
                resetOwnership();
            }
        }

        SHORT lButtonState = GetAsyncKeyState(VK_LBUTTON);
        bool isMouseDown = (lButtonState & 0x8000) != 0;
        // Also catch clicks that began and ended between two 50 ms polls (bit 0).
        bool mouseClicked = (wasMouseDown && !isMouseDown) || (!isMouseDown && (lButtonState & 0x0001) != 0);
        wasMouseDown = isMouseDown;

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
        }

        RECT flyoutBounds = {};
        bool overXamlHost =
            menuOpened && QueryXamlHosts(FindWindowW(L"Shell_TrayWnd", nullptr),
                                         chevronRect, pt, &flyoutBounds);

        // Only a click inside the flyout counts as "clicked an icon".
        if (mouseClicked && overXamlHost) {
            clickPending = true;
            clickAt = now;
        }

        // A clicked icon that opened a popup menu keeps the flyout open until
        // the menu is gone.
        if (clickPending && now - clickAt >= kClickSettleMs && !IsPopupMenuOpen()) {
            closeFlyout();
            continue;
        }

        DWORD taskbarPid = 0;
        bool overTaskbar = IsTaskbarWindowAtPoint(pt, &taskbarPid);
        bool isOverflow = false;

        if (overTaskbar) {
            bool cursorMoved = (pt.x != lastQueriedPt.x || pt.y != lastQueriedPt.y);

            // Only perform cross-process UIA hit-testing if the cursor moved
            // OR if it is resting on a valid overflow button.
            if (cursorMoved || lastPtWasOverflow) {
                lastQueriedPt = pt;

                IUIAutomationElement* overflowElement = nullptr;
                IUIAutomationTogglePattern* togglePattern = nullptr;
                IUIAutomationInvokePattern* invokePattern = nullptr;
                IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
                bool isTrayChevron = false;

                isOverflow = FindTaskbarOverflowElement(
                    automation, cacheReq, pt, taskbarPid,
                    &overflowElement, &togglePattern, &invokePattern, &expandPattern, &isTrayChevron);

                lastPtWasOverflow = isOverflow;

                // If the mod currently has one flyout open (e.g. App Overflow) and
                // the user moves onto the other button (e.g. Tray Chevron), close the first.
                if (isOverflow && menuOpened && isTrayChevron != activeIsTrayChevron) {
                    closeFlyout();
                    hoverConsumed = false;
                }

                // Do not toggle a flyout that is already open (for example one the
                // user opened with a click), and do not reopen it for this hover
                // if the user closes it by clicking the button.
                if (isOverflow && IsOverflowOpen(togglePattern, isTrayChevron, taskbarPid)) {
                    hoverConsumed = true;
                }

                if (isOverflow && !hoverConsumed) {
                    currentHoverTime += 50;

                    if (currentHoverTime >= kHoverDelayMs) {
                        HRESULT actionHr = triggerAction(
                            togglePattern, invokePattern, expandPattern, true);

                        if (SUCCEEDED(actionHr)) {
                            menuOpened = true;
                            hoverConsumed = true;
                            openedAt = now;
                            currentCloseHoverTime = 0;
                            clickPending = false;
                            activeIsTrayChevron = isTrayChevron;
                            activeTaskbarPid = taskbarPid;

                            // Keep the button patterns so the flyout can be closed through them.
                            if (chevron) {
                                chevron->Release();
                                chevron = nullptr;
                            }
                            if (chevronInvoke) {
                                chevronInvoke->Release();
                                chevronInvoke = nullptr;
                            }
                            if (chevronExpand) {
                                chevronExpand->Release();
                                chevronExpand = nullptr;
                            }

                            if (togglePattern) {
                                chevron = togglePattern;
                                chevron->AddRef();
                            }
                            if (invokePattern) {
                                chevronInvoke = invokePattern;
                                chevronInvoke->AddRef();
                            }
                            if (expandPattern) {
                                chevronExpand = expandPattern;
                                chevronExpand->AddRef();
                            }

                            // Remember where the button is, so the area between the
                            // button and the menu counts as "still hovering".
                            if (FAILED(overflowElement->get_CachedBoundingRectangle(&chevronRect)) &&
                                FAILED(overflowElement->get_CurrentBoundingRectangle(&chevronRect))) {
                                chevronRect = {};
                            }
                        } else {
                            Wh_Log(L"Overflow action failed: 0x%08X", actionHr);
                        }

                        currentHoverTime = 0;
                    }
                } else {
                    currentHoverTime = 0;
                }

                if (expandPattern) expandPattern->Release();
                if (invokePattern) invokePattern->Release();
                if (togglePattern) togglePattern->Release();
                if (overflowElement) overflowElement->Release();
            } else {
                currentHoverTime = 0;
            }
        } else {
            lastQueriedPt = { -10000, -10000 };
            lastPtWasOverflow = false;
            currentHoverTime = 0;
        }

        if (!isOverflow) {
            hoverConsumed = false;
        }

        // Closing logic: while the flyout is open, the safe cursor area is the
        // overflow button, the flyout, and the space between them (their
        // combined bounding box), plus a thumbnail preview that is showing.
        // Nothing is closed while a tray icon's popup menu is showing.
        if (menuOpened) {
            bool flyoutSettled =
                !IsRectEmpty(&flyoutBounds) || now - openedAt >= kOpenGraceMs;

            RECT area = {};
            UnionRect(&area, &chevronRect, &flyoutBounds);
            bool inArea = isOverflow || overXamlHost || PtInRect(&area, pt);

            if (inArea || !flyoutSettled || IsPopupMenuOpen()) {
                currentCloseHoverTime = 0;
            } else {
                currentCloseHoverTime += 50;

                if (currentCloseHoverTime >= kCloseDelayMs) {
                    closeFlyout();
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }
    }

    if (chevron) {
        chevron->Release();
        chevron = nullptr;
    }
    if (chevronInvoke) {
        chevronInvoke->Release();
        chevronInvoke = nullptr;
    }
    if (chevronExpand) {
        chevronExpand->Release();
        chevronExpand = nullptr;
    }
    if (cacheReq) {
        cacheReq->Release();
        cacheReq = nullptr;
    }

    automation->Release();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Taskbar & System Tray Overflow on Hover v1.1.0");

    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, HoverMonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading Taskbar & System Tray Overflow on Hover");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Wait without a timeout: the thread must be fully finished before the
    // process continues to tear down.
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// Reload mod settings dynamically when changed in the Windhawk UI.
void WhTool_ModSettingsChanged() {
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                             ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
