// ==WindhawkMod==
// @id archive-preview
// @name Archive Preview
// @description Browse, open, copy and extract archives in File Explorer's Preview Pane, with sortable columns and optional extraction using 7-Zip, NanaZip or WinRAR.
// @description:es Explora, abre, copia y extrae archivos comprimidos desde el panel de vista previa del Explorador, con columnas ordenables y extracción opcional mediante 7-Zip, NanaZip o WinRAR.
// @version 1.1.0
// @author Luiz Bizzio
// @github https://github.com/luizbizzio
// @homepage https://github.com/luizbizzio/archive-preview
// @donateUrl https://github.com/sponsors/luizbizzio
// @license Apache-2.0
// @architecture x86-64
// @include explorer.exe
// @include prevhost.exe
// @compilerOptions -lole32 -loleaut32 -luuid -luser32 -lgdi32 -ladvapi32 -lshell32 -lshlwapi -lcomctl32 -luxtheme -lpropsys -ldwmapi -lwindowscodecs -lmsimg32 -lpsapi -lurlmon
// ==/WindhawkMod==

// Archive Preview v1.1.0: Windows Shell browsing with optional external GUI extraction.
// Native transfers use Windows Shell without mod-managed disk staging.
// Optional external dragging extracts into a private folder in the mod's storage.
// Temporary data is retained while transfer objects and streams use it;
// the broker retries cleanup after restart when it can verify the folder is idle.
// Windows Shell providers may also create their own temporary data.
// Native extraction does not enforce a mod-defined expanded-size limit.
// Updating the mod never terminates Explorer or Preview Host processes.
// An optional setting requests closure of Explorer windows containing an
// Archive Preview during unload. The DLL waits for COM references to drain.

// ==WindhawkModReadme==
/*
# Archive Preview

![Archive Preview](https://raw.githubusercontent.com/luizbizzio/archive-preview/main/assets/archive-preview-cover.png)

**Browse, open, copy and extract archives directly in Windows File Explorer.**

Archive Preview is a [Windhawk](https://windhawk.net/) mod that adds archive browsing to File Explorer's **Preview Pane (Alt + P)**.

Browsing uses Windows Shell archive support. Extraction can use Windows or a compatible external program.

&nbsp;

---

&nbsp;

## Features

- Browse folders inside supported archives.
- Open compatible files with their default Windows app. Direct opening of certain potentially dangerous file types is blocked.
- Copy selected items with **Ctrl + C** and paste them into a destination folder with **Ctrl + V**.
- Drag files and folders into File Explorer or a compatible app.
- Use **Extract here**, **Extract to…** or **Extract selected**, including selection actions in the item context menu.
- Choose Windows, automatic external-program detection, 7-Zip, NanaZip or WinRAR for extraction.
- Click column headers to sort. Click again to reverse the order. Sorting starts by name, keeps folders first, and compares dates and sizes by value.
- Drag a column separator to resize it. Name gets priority when the pane is narrow.
- Right-click the headers to show or hide Modified, Type and Size. Width and visibility changes are saved automatically.
- Use light or dark themes, high contrast support and the Windows accent color.
- Follow the Windows display language or choose a language for the main preview controls.

&nbsp;

---

&nbsp;

## Getting started

1. Install and enable the mod in Windhawk.
2. Open File Explorer and press **Alt + P**.
3. Select a supported archive.
4. Browse its contents and choose an action.

Double-click to open a folder or compatible file. Use **Back** to return to the parent folder and **Ctrl** or **Shift** to select multiple items.

**Ctrl + Shift + E** runs **Extract selected here** while the list has focus. You can set a shortcut for each extract action in Windhawk.

**Extract to…** initially opens the folder containing the archive.

&nbsp;

---

&nbsp;

## Supported formats

The default extensions are **`.zip`, `.rar`, `.7z` and `.tar`**.

Additional extensions can be configured. Experimental options include `.tar.gz`, `.tgz`, `.tbz` and `.tzst`. Their availability depends on Windows and its archive providers.

Adding an extension or choosing an external extractor does not install a Windows Shell browsing provider. Some compound extensions also require a compatible preview association.

&nbsp;

---

&nbsp;

## External extraction and dragging

Windows is the default extraction backend. You can instead select automatic external-program detection, 7-Zip, NanaZip or WinRAR.

Install the program first. Leave its path empty for automatic detection or specify a compatible graphical executable such as `7zG.exe`, `NanaZipG.exe`, `NanaZip.Universal.Windows.exe` or `WinRAR.exe`.

Extraction actions use the selected backend. External programs provide their own progress, password and overwrite dialogs as supported. External extraction preserves archive paths, including for selected items.

With external dragging enabled and an external backend selected, dragged items are extracted into a private temporary folder in the mod's storage before Explorer transfers them to the destination. This requires additional temporary disk space.

The mod keeps temporary files while the transfer uses them and tries to remove them when the transfer releases them. The broker checks for leftovers the next time it starts. It keeps folders that are still in use or whose status it cannot confirm.

Disable external dragging to restore native dragging. **Clipboard copy and paste always continue using Windows.** Explorer may still display its own copy window.

&nbsp;

---

&nbsp;

## Settings

| Group | What you can change |
| --- | --- |
| **Files and security** | Archive extensions and the optional preview exception for local downloads. The exception is off by default and keeps Mark of the Web. |
| **Extraction** | Extraction program, optional program path, and external dragging. |
| **Appearance** | Theme and preview language. Automatic follows the Windows display language. The preview offers 105 languages and variants. Some messages use English when a translation is unavailable. |
| **Columns** | Visibility, horizontal scrollbar, and date and size lookups for archives of 256 MiB or more. Windhawk settings take priority over saved column visibility choices. |
| **Shortcuts** | Four extraction shortcuts. Enter `disabled` to turn a shortcut off. If two actions use the same keys, both shortcuts are disabled until you change one. |
| **Updates and disabling** | Optionally close Explorer windows that still contain an archive preview when the mod updates, recompiles, or is disabled. This is off by default. Enabling it closes all tabs in each affected window. |
| **Diagnostics** | Enable detailed logs when reporting a preview or extraction issue. Turn on Windhawk logging to see them. |

Translated setting labels follow Windhawk's language. The preview language setting applies to the mod's controls. Windows and external extractor dialogs use their own language settings.

&nbsp;

---

&nbsp;

## Important limitations

- Browsing and native operations depend on Windows Shell providers. Some archives, compression methods and password-protected files may not work. Native browsing has no mod-provided password dialog.
- The preview bridge requires the built-in Windows text Preview Handler and compatible associations. It redirects previews at runtime without creating COM preview registrations.
- Broker operations require a local drive reported as fixed and a path without reparse points, such as symbolic links or junctions. Network locations and drives reported as removable are rejected. A recovery list may sometimes display their contents without supporting open, copy, drag or extraction.
- During native extraction, Windows may report 0% until the operation finishes. External programs show progress in their own window.
- Large archives and stalled providers can delay loading, progress, cancellation or input.
- Transfers depend on the provider and destination app. Some apps do not accept virtual files.
- Native extraction has no mod-defined expanded-size limit. Extract only trusted archives.
- ZIP folder dates may be derived from contained items when no folder date is stored.
- Transfer requests support up to **4,096 selected items**.
- Disabling or updating the mod may wait for workers and preview objects to finish. A stalled provider can prolong the wait indefinitely. The mod never forces a process restart.
- Components running inside the same Preview Host share that context; providers are not fully isolated from one another.

&nbsp;

---

&nbsp;

## Troubleshooting

### The preview does not appear

Press **Alt + P**, check the configured extensions, then select another file and retry the archive.

### Windhawk stays on Uninitializing

Close the Explorer window that showed the preview. You may need to do this manually if the automatic option is off or the pane is already blank.

### An action fails

If an operation fails, try the archive directly in File Explorer. For external extraction, check the selected program and executable path. An external extractor does not replace a missing Windows browsing provider.

### Dates or sizes are missing

Check column visibility and the large-archive metadata settings. The provider may not supply every value.

### Report a problem

When reporting a problem, include your Windows version, archive format, selected backend, steps and result. Enable Windhawk logging and turn on **Diagnostics > Detailed logs** before reproducing it. Include `[AP lifecycle]` for a slow update or shutdown, `[AP list]` for a list recovery, or `[AP external drag]` for temporary folder cleanup. Remove private names and paths before sharing logs.

&nbsp;

---

&nbsp;

## Author and license

Created by **[Luiz Bizzio](https://github.com/luizbizzio)** and licensed under **Apache License 2.0**.

[Source code](https://github.com/luizbizzio/archive-preview) · [Report an issue](https://github.com/luizbizzio/archive-preview/issues) · [License](https://github.com/luizbizzio/archive-preview/blob/main/LICENSE)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FilesAndSecurity:
    - ArchiveExtensions: ".zip,.rar,.7z,.tar"
      $name: Archive extensions
      $name:es: Extensiones de archivos comprimidos
      $description: Comma-separated extensions. A compatible Windows Shell provider is required.
      $description:es: Extensiones separadas por comas. Requieren un proveedor compatible del Shell de Windows.
    - AutoAllowInternetArchivePreview: false
      $name: Allow previews of archives marked as downloaded from the Internet
      $name:es: Permitir vista previa de archivos marcados como procedentes de Internet
      $description: Optional scoped exception for local archives. Disabled by default.
      $description:es: Excepción opcional y limitada para archivos comprimidos locales. Desactivada de forma predeterminada.
  $name: Files and security
  $name:es: Archivos y seguridad
  $name:pt: Arquivos e segurança
- Extract:
    - ExtractionBackend: windows
      $name: Extraction program
      $name:es: Programa de extracción
      $description: External programs show their own progress, password and overwrite dialogs. Install the selected program first. External extraction preserves archive paths, including when extracting selected items.
      $description:es: Los programas externos muestran sus propias ventanas de progreso, contraseñas y sobrescritura. Instala primero el programa elegido. La extracción externa conserva las rutas del archivo, también al extraer una selección.
      $options:
      - windows: Windows (original)
      - auto: External - automatic detection
      - 7zip: 7-Zip
      - nanazip: NanaZip
      - winrar: WinRAR
      $options:es:
      - windows: Windows (original)
      - auto: Externo - detectar automáticamente
      - 7zip: 7-Zip
      - nanazip: NanaZip
      - winrar: WinRAR
    - ExternalExtractorPath: ""
      $name: External program path (optional)
      $name:es: Ruta del programa externo (opcional)
      $description: Leave empty for automatic detection. Otherwise enter the full path to 7zG.exe, NanaZipG.exe, NanaZip.Universal.Windows.exe or WinRAR.exe. Use the graphical extraction executable, not the file manager or console program.
      $description:es: Déjalo vacío para detectar el programa. También puedes indicar la ruta completa a 7zG.exe, NanaZipG.exe, NanaZip.Universal.Windows.exe o WinRAR.exe. Usa el ejecutable gráfico de extracción, no el gestor de archivos ni el de consola.
      #! $hideIf: {ExtractionBackend: windows}
    - ExternalDragExtraction: true
      $name: Use the selected external program when dragging files
      $name:es: Usar el programa externo al arrastrar archivos
      $description: Extracts the dragged selection to a temporary folder, then Explorer copies it to the drop destination. Requires extra temporary disk space. Clipboard copy/paste continues using Windows. Disable to restore native dragging.
      $description:es: Extrae la selección a una carpeta temporal y después el Explorador la copia al destino donde se suelte. Necesita espacio temporal adicional. Copiar y pegar sigue usando Windows. Desactívalo para recuperar el arrastre original.
      #! $hideIf: {ExtractionBackend: windows}
  $name: Extraction
  $name:es: Extracción
  $name:pt: Extração
- Preview:
    - ThemeMode: auto
      $name: Theme
      $name:es: Tema
      $options:
      - auto: Automatic
      - light: Light
      - dark: Dark
      $options:es:
      - auto: Automático
      - light: Claro
      - dark: Oscuro
    - Language: auto
      $name: Preview language
      $name:es: Idioma de la vista previa
      $name:pt: Idioma do painel de visualização
      $description: Automatic uses the Windows display language when available.
      $description:es: Automático usa el idioma de pantalla de Windows cuando está disponible.
      $description:pt: O modo automático usa o idioma de exibição do Windows quando disponível.
      $options:
      - auto: Automatic (Windows)
      - af: "Afrikaans"
      - am: "አማርኛ"
      - ar: "العربية"
      - as: "অসমীয়া"
      - az: "Azərbaycanca"
      - be: "Беларуская"
      - bg: "Български"
      - bn: "বাংলা"
      - bs: "Bosanski"
      - ca: "Català"
      - chr: "ᏣᎳᎩ"
      - cs: "Čeština"
      - cy: "Cymraeg"
      - da: "Dansk"
      - de: "Deutsch"
      - el: "Ελληνικά"
      - en: "English"
      - es: "Español"
      - et: "Eesti"
      - eu: "Euskara"
      - fa: "فارسی"
      - fi: "Suomi"
      - fil: "Filipino"
      - fr: "Français"
      - ga: "Gaeilge"
      - gd: "Gàidhlig"
      - gl: "Galego"
      - gu: "ગુજરાતી"
      - ha: "Hausa"
      - he: "עברית"
      - hi: "हिन्दी"
      - hr: "Hrvatski"
      - hu: "Magyar"
      - hy: "Հայերեն"
      - id: "Bahasa Indonesia"
      - ig: "Igbo"
      - is: "Íslenska"
      - it: "Italiano"
      - iu-Latn: "Inuktitut (Latin)"
      - ja: "日本語"
      - ka: "ქართული"
      - kk: "Қазақша"
      - km: "ខ្មែរ"
      - kn: "ಕನ್ನಡ"
      - ko: "한국어"
      - kok: "कोंकणी"
      - ku-Arab: "کوردی (سۆرانی)"
      - ky: "Кыргызча"
      - lb: "Lëtzebuergesch"
      - lo: "ລາວ"
      - lt: "Lietuvių"
      - lv: "Latviešu"
      - mi: "Māori"
      - mk: "Македонски"
      - ml: "മലയാളം"
      - mn: "Монгол"
      - mr: "मराठी"
      - ms: "Bahasa Melayu"
      - mt: "Malti"
      - nb: "Norsk bokmål"
      - ne: "नेपाली"
      - nl: "Nederlands"
      - nn: "Norsk nynorsk"
      - nso: "Sesotho sa Leboa"
      - or: "ଓଡ଼ିଆ"
      - pa: "ਪੰਜਾਬੀ (ਗੁਰਮੁਖੀ)"
      - pa-Arab: "پنجابی (شاہ مکھی)"
      - pl: "Polski"
      - prs: "دری"
      - pt-BR: "Português (Brasil)"
      - pt-PT: "Português (Portugal)"
      - quc: "K’iche’"
      - quz: "Runasimi"
      - ro: "Română"
      - ru: "Русский"
      - rw: "Ikinyarwanda"
      - sd-Arab: "سنڌي"
      - si: "සිංහල"
      - sk: "Slovenčina"
      - sl: "Slovenščina"
      - sq: "Shqip"
      - sr-Cyrl: "Српски (ћирилица)"
      - sr-Latn: "Srpski (latinica)"
      - sv: "Svenska"
      - sw: "Kiswahili"
      - ta: "தமிழ்"
      - te: "తెలుగు"
      - tg: "Тоҷикӣ"
      - th: "ไทย"
      - ti: "ትግርኛ"
      - tk: "Türkmençe"
      - tn: "Setswana"
      - tr: "Türkçe"
      - tt: "Татарча"
      - ug: "ئۇيغۇرچە"
      - uk: "Українська"
      - ur: "اردو"
      - uz: "O‘zbekcha"
      - vi: "Tiếng Việt"
      - wo: "Wolof"
      - xh: "isiXhosa"
      - yo: "Yorùbá"
      - zh-Hans: "简体中文"
      - zh-Hant: "繁體中文"
      - zu: "isiZulu"
  $name: Appearance
  $name:es: Apariencia
  $name:pt: Aparência
- DisplayColumns:
    - HideDateModified: false
      $name: Hide the Date modified column
      $name:es: Ocultar la columna de fecha de modificación
    - HideType: false
      $name: Hide the Type column
      $name:es: Ocultar la columna de tipo
    - HideSize: false
      $name: Hide the Size column
      $name:es: Ocultar la columna de tamaño
    - HideHorizontalScrollbar: false
      $name: Hide horizontal scrollbar
      $name:es: Ocultar la barra de desplazamiento horizontal
      $description: Show fewer columns when the pane is too narrow to fit them.
      $description:es: Muestra menos columnas cuando el panel es demasiado estrecho.
    - ReadLargeArchiveDates: false
      $name: Read modification dates in large archives
      $name:es: Consultar fechas en comprimidos grandes
      $description: Applies from 256 MiB of compressed archive size. Enable if you need to see dates for these archives. Queries can slow some providers. The date column must also be visible.
      $description:es: Se aplica a comprimidos de 256 MiB o más. Actívalo si necesitas ver los datos de fechas. Las consultas pueden ralentizar algunos proveedores. La columna de modificación también debe estar visible.
    - ReadLargeArchiveSizes: false
      $name: Read file sizes in large archives
      $name:es: Consultar tamaños en comprimidos grandes
      $description: Applies from 256 MiB of compressed archive size. Enable if you need to see sizes for these archives. Queries can slow some providers. The size column must also be visible.
      $description:es: Se aplica a comprimidos de 256 MiB o más. Actívalo si necesitas ver los datos de tamaños. Las consultas pueden ralentizar algunos proveedores. La columna de tamaño también debe estar visible.
  $name: Columns
  $name:es: Columnas
  $name:pt: Colunas
- KeyBindings:
    - ExtractHereShortcut: "disabled"
      $name: Extract here shortcut
      $name:es: Atajo para extraer aquí
      $description: Enter a shortcut such as ctrl+alt+e, or use disabled to turn it off.
      $description:es: Escribe un atajo como ctrl+alt+e o usa disabled para desactivarlo.
    - ExtractToShortcut: "disabled"
      $name: Extract to shortcut
      $name:es: Atajo para extraer en una carpeta
      $description: Enter a shortcut such as ctrl+alt+t, or use disabled to turn it off.
      $description:es: Escribe un atajo como ctrl+alt+t o usa disabled para desactivarlo.
    - ExtractSelectedShortcut: "ctrl+shift+e"
      $name: Extract selected here shortcut
      $name:es: Atajo para extraer la selección aquí
      $description: Enter a shortcut such as ctrl+shift+e, or use disabled to turn it off.
      $description:es: Escribe un atajo como ctrl+shift+e o usa disabled para desactivarlo.
    - ExtractSelectedToShortcut: "disabled"
      $name: Extract selected to shortcut
      $name:es: Atajo para extraer la selección en una carpeta
      $description: Enter a shortcut such as ctrl+shift+t, or use disabled to turn it off.
      $description:es: Escribe un atajo como ctrl+shift+t o usa disabled para desactivarlo.
  $name: Shortcuts
  $name:es: Atajos
  $name:pt: Atalhos
- ModLifecycle:
    - CloseExplorerWindowOnUnload: false
      $name: Close Explorer windows with an archive preview
      $name:es: Cerrar ventanas del Explorador con una vista previa
      $name:pt: Fechar janelas do Explorador com uma prévia
      $description: When you update, recompile or disable the mod, Explorer closes windows that still contain an Archive Preview, including all tabs in those windows.
      $description:es: Al actualizar, recompilar o desactivar el mod, el Explorador cierra las ventanas que aún contienen una vista previa del mod, incluidas todas sus pestañas.
      $description:pt: Ao atualizar, recompilar ou desativar o mod, o Explorador fecha as janelas que ainda contêm uma prévia do mod, incluindo todas as abas dessas janelas.
  $name: Updates and disabling
  $name:es: Actualizaciones y desactivación
  $name:pt: Atualizações e desativação
- Diagnostics:
    - DetailedLogs: false
      $name: Detailed logs
      $name:es: Registros detallados
      $name:pt: Logs detalhados
      $description: Collect extra details about previews and extraction. Enable Windhawk logging to see them.
      $description:es: Recopila más detalles sobre las vistas previas y la extracción. Activa los registros de Windhawk para verlos.
      $description:pt: Coleta mais detalhes sobre prévias e extração. Ative os logs do Windhawk para vê-los.
  $name: Diagnostics
  $name:es: Diagnóstico
  $name:pt: Diagnóstico
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wincrypt.h>
#include <windowsx.h>
#include <ole2.h>
#include <ocidl.h>
#include <objbase.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shldisp.h>
#include <exdisp.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <urlmon.h>
#include <iterator>
#include <utility>
#include <dwmapi.h>
#include <wincodec.h>
#include <propkey.h>
#include <psapi.h>
#include <sddl.h>
#include <aclapi.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <vector>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <limits>
#include <cstdio>

// Lifetime management for short IPC workers and background Shell enumerators.
// Creation and shutdown share a mutex, so Wh_ModUninit cannot miss a newly
// created thread. Do not detach or FreeLibraryAndExitThread a worker.
namespace ArchivePreviewWorkerLifetime {
struct Record { HANDLE thread{}; Record * next{}; };
static std::mutex mutex;
static Record * records = nullptr;
static bool stopping = false;
static HANDLE Start(LPTHREAD_START_ROUTINE callback,void * context) {
    auto record = std::unique_ptr < Record > (new(std::nothrow) Record());
    if(!record){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return nullptr;}
    std::lock_guard < std::mutex > lock(mutex);
    if(stopping){SetLastError(ERROR_SHUTDOWN_IN_PROGRESS);return nullptr;}
    // A poll may run every 500 ms for hours. Reap completed handles to avoid
    // accumulating thousands of terminated worker records in Explorer.
    Record *  * cursor =  & records;
    while( * cursor){
        Record * active =  * cursor;
        if(WaitForSingleObject(active->thread,0) == WAIT_OBJECT_0){
            * cursor = active->next;
            CloseHandle(active->thread);
            delete active;
        }else cursor =  & active->next;
    }
    HANDLE thread = CreateThread(nullptr,0,callback,context,0,nullptr);
    if(!thread)return nullptr;
    record->thread = thread;
    record->next = records;
    records = record.release();
    return thread;
}
static size_t CountRecords(const Record * record) {
    size_t count = 0;
    for (; record; record = record->next) ++count;
    return count;
}
static void JoinAll() {
    Record * current = nullptr;
    {
        std::lock_guard < std::mutex > lock(mutex);
        stopping = true;
        current = records;
        records = nullptr;
    }
    const ULONGLONG started = GetTickCount64();
    Wh_Log(L"[AP lifecycle] phase=workers-join-begin tracked=%zu",CountRecords(current));
    while(current){
        Record * next = current->next;
        // Synchronous pipe reads are cancellable. Third-party synchronous
        // COM calls may not be, so never assume a timeout means completion.
        CancelSynchronousIo(current->thread);
        for(;;){
            const DWORD result = WaitForSingleObject(current->thread,30000);
            if(result == WAIT_OBJECT_0)break;
            if(result == WAIT_TIMEOUT){
                Wh_Log(L"[AP lifecycle] phase=worker-wait worker_tid=%lu elapsed_ms=%llu",
                    GetThreadId(current->thread),GetTickCount64() - started);
                CancelSynchronousIo(current->thread);
                continue;
            }
            // A failed wait is not proof the worker has ended. Preserve code
            // safety rather than returning to Windhawk with live callbacks.
            const DWORD error = GetLastError();
            Wh_Log(L"[AP lifecycle] phase=worker-wait-failed win32=%lu worker_tid=%lu",
                error,GetThreadId(current->thread));
            Sleep(100);
        }
        CloseHandle(current->thread);
        delete current;
        current = next;
    }
    Wh_Log(L"[AP lifecycle] phase=workers-joined elapsed_ms=%llu",GetTickCount64() - started);
}
} // namespace ArchivePreviewWorkerLifetime

namespace ArchivePreviewText {
    static void LoadLanguage();
    static UINT TextDirection();
}

// Shared Windows 11-inspired button paint and hover handling.  These are
// owner-drawn Win32 buttons, not WinUI controls: keep native keyboard focus,
// touch/mouse input, high contrast, and per-monitor DPI without a new runtime.
namespace ArchivePreviewButtons {
    static constexpr wchar_t kHoverProperty[] = L"ArchivePreview.Button.Hover";
    static constexpr UINT_PTR kSubclassId = 0xCFB091;

    static LRESULT CALLBACK HoverSubclass(HWND hwnd, UINT message, WPARAM wp,
                                          LPARAM lp, UINT_PTR id, DWORD_PTR) {
        switch (message) {
          case WM_MOUSEMOVE:
            if (!GetPropW(hwnd,kHoverProperty)) {
                if (SetPropW(hwnd,kHoverProperty,reinterpret_cast < HANDLE > (1))) {
                    TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};
                    TrackMouseEvent( & track);
                    InvalidateRect(hwnd,nullptr,FALSE);
                }
            }
            break;
          case WM_MOUSELEAVE:
            RemovePropW(hwnd,kHoverProperty);
            InvalidateRect(hwnd,nullptr,FALSE);
            break;
          case WM_ENABLE:
          case WM_SETFOCUS:
          case WM_KILLFOCUS:
          case WM_THEMECHANGED:
            InvalidateRect(hwnd,nullptr,FALSE);
            break;
          case WM_NCDESTROY:
            RemovePropW(hwnd,kHoverProperty);
            RemoveWindowSubclass(hwnd,HoverSubclass,id);
            break;
        }
        return DefSubclassProc(hwnd,message,wp,lp);
    }

    static void Attach(HWND hwnd) {
        if (hwnd) SetWindowSubclass(hwnd,HoverSubclass,kSubclassId,0);
    }

    static void Paint(HDC dc, HWND hwnd, const RECT & area, UINT state,
                      HFONT font, PCWSTR label, COLORREF parentBackground,
                      bool dark, bool highContrast, UINT dpi,
                      bool backArrow = false) {
        if (!dc) return;
        const int width = static_cast < int > (area.right - area.left);
        const int height = static_cast < int > (area.bottom - area.top);
        if (width <= 0 || height <= 0) return;
        const bool enabled = (state & ODS_DISABLED) == 0;
        const bool pressed = (state & ODS_SELECTED) != 0;
        const bool hot = enabled && GetPropW(hwnd,kHoverProperty) != nullptr;
        const bool focused = (state & ODS_FOCUS) != 0;
        const int scale = static_cast < int > (dpi ? dpi : 96);
        const int radius = std::max(2,std::min(MulDiv(6,scale,96),
            std::min(width,height) - 1)); // 3 DIP corner radius, ellipse diameter 6 DIP.

        COLORREF face = dark?RGB(53,53,53):RGB(252,252,252);
        COLORREF edge = dark?RGB(100,100,100):RGB(205,205,205);
        COLORREF text = dark?RGB(239,239,239):RGB(35,35,35);
        COLORREF focus = GetSysColor(COLOR_HIGHLIGHT);
        if (!enabled) {
            face = dark?RGB(44,44,44):RGB(246,246,246);
            edge = dark?RGB(70,70,70):RGB(224,224,224);
            text = dark?RGB(135,135,135):RGB(150,150,150);
        } else if (pressed) {
            face = dark?RGB(44,44,44):RGB(228,228,228);
            edge = dark?RGB(117,117,117):RGB(174,174,174);
        } else if (hot) {
            face = dark?RGB(66,66,66):RGB(245,245,245);
            edge = dark?RGB(129,129,129):RGB(173,173,173);
        }
        if (highContrast) {
            face = hot && enabled ? GetSysColor(COLOR_HIGHLIGHT):GetSysColor(COLOR_BTNFACE);
            edge = GetSysColor(COLOR_BTNTEXT);
            text = !enabled?GetSysColor(COLOR_GRAYTEXT):
                (hot?GetSysColor(COLOR_HIGHLIGHTTEXT):GetSysColor(COLOR_BTNTEXT));
            focus = GetSysColor(COLOR_WINDOWTEXT);
        }

        // Clearing the complete owner-draw rectangle with the parent's color
        // makes rounded corners blend, without rectangular fill artifacts.
        HBRUSH parent = CreateSolidBrush(parentBackground);
        if (parent) {FillRect(dc, & area,parent);DeleteObject(parent);}
        HBRUSH fill = CreateSolidBrush(face);
        HPEN pen = CreatePen(PS_SOLID,1,edge);
        if (fill && pen) {
            HGDIOBJ oldFill = SelectObject(dc,fill);
            HGDIOBJ oldPen = SelectObject(dc,pen);
            RoundRect(dc,area.left,area.top,area.right,area.bottom,radius,radius);
            SelectObject(dc,oldPen);
            SelectObject(dc,oldFill);
        } else if (fill) {
            // Out-of-resource fallback: keep the button visible and clickable.
            FillRect(dc, & area,fill);
        }
        if (pen) DeleteObject(pen);
        if (fill) DeleteObject(fill);

        HFONT oldFont = font?static_cast < HFONT > (SelectObject(dc,font)):nullptr;
        const int oldMode = SetBkMode(dc,TRANSPARENT);
        const COLORREF oldText = SetTextColor(dc,text);
        RECT caption = area;
        caption.left += MulDiv(3,scale,96);
        caption.right -= MulDiv(3,scale,96);
        if (backArrow) {
            const int cx = static_cast < int > ((area.left + area.right) / 2);
            const int cy = static_cast < int > ((area.top + area.bottom) / 2);
            const int half = std::max(2,std::min(MulDiv(6,scale,96),
                std::min(width,height) / 3));
            HPEN arrow = CreatePen(PS_SOLID,std::max(1,MulDiv(2,scale,96)),text);
            if (arrow) {
                HGDIOBJ previous = SelectObject(dc,arrow);
                MoveToEx(dc,cx + half,cy,nullptr);
                LineTo(dc,cx - half,cy);
                POINT head[] = {{cx,cy - half},{cx - half,cy},{cx,cy + half}};
                Polyline(dc,head,3);
                SelectObject(dc,previous);
                DeleteObject(arrow);
            }
        } else if (caption.right > caption.left) {
            DrawTextW(dc,label, - 1, & caption,DT_SINGLELINE | DT_VCENTER |
                DT_CENTER | DT_END_ELLIPSIS | DT_NOPREFIX | ArchivePreviewText::TextDirection());
        }
        SetTextColor(dc,oldText);
        SetBkMode(dc,oldMode);
        if (oldFont) SelectObject(dc,oldFont);

        // Draw a subtle rounded, high-contrast keyboard focus indicator.
        if (focused) {
            const int inset = std::max(1,MulDiv(2,scale,96));
            RECT frame = area;
            InflateRect( & frame, - inset, - inset);
            if (frame.right > frame.left + 2 && frame.bottom > frame.top + 2) {
                HPEN ring = CreatePen(PS_SOLID,1,focus);
                if (ring) {
                    HGDIOBJ oldPen = SelectObject(dc,ring);
                    HGDIOBJ oldFill = SelectObject(dc,GetStockObject(NULL_BRUSH));
                    RoundRect(dc,frame.left,frame.top,frame.right,frame.bottom,
                        std::max(2,radius - inset),std::max(2,radius - inset));
                    SelectObject(dc,oldFill);
                    SelectObject(dc,oldPen);
                    DeleteObject(ring);
                }
            }
        }
    }
}

namespace ArchivePreviewDiagnostics {
    static std::atomic<bool> detailedLogs{false};

    static void Load() {
        detailedLogs.store(Wh_GetIntSetting(L"Diagnostics.DetailedLogs") != 0,
                           std::memory_order_relaxed);
    }

    static bool Enabled() {
        return detailedLogs.load(std::memory_order_relaxed);
    }
}

namespace ArchivePreviewSettings {
    static std::wstring ReadText(PCWSTR key) {
        PCWSTR setting = Wh_GetStringSetting(key);
        if (!setting) return {};
        std::wstring value(setting);
        Wh_FreeStringSetting(setting);
        return value;
    }
}

// Each injected process loads its own settings.
namespace ArchivePreviewOptions {
    static std::atomic < bool > autoAllowInternetArchivePreview{false};
    static std::atomic < int > themeMode{0}; // 0 auto, 1 light, 2 dark
    static std::atomic<unsigned> visibleColumns{0xF};
    static std::atomic<bool> hideHorizontalScrollbar{false};
    static std::atomic<bool> closeExplorerWindowOnUnload{false};
    static bool ShowColumn(int kind) {
        return (visibleColumns.load() & (1u << kind)) != 0;
    }
    static std::atomic<unsigned> largeMetadataMask{0};
    static constexpr ULONGLONG kLargeMetadataThreshold = 256ULL * 1024 * 1024;
    struct MetadataPolicy {
        unsigned visibleMask;
        unsigned largeMask;
        unsigned ForArchive(ULONGLONG archiveBytes) const {
            return archiveBytes >= kLargeMetadataThreshold ? visibleMask & largeMask : visibleMask;
        }
    };
    static MetadataPolicy MetadataSnapshot() {
        const unsigned columns = visibleColumns.load();
        return {((columns & 0b0010) ? 1u : 0u) | ((columns & 0b1000) ? 2u : 0u),
            largeMetadataMask.load()};
    }
    static unsigned MetadataMask(ULONGLONG archiveBytes) {
        return MetadataSnapshot().ForArchive(archiveBytes);
    }
    struct ExtractionOptions {
        int backend{}; // 0 Windows, 1 auto, 2 7-Zip, 3 NanaZip, 4 WinRAR
        bool externalDrag{true};
        std::wstring executable;
    };
    static std::mutex extractionOptionsMutex;
    static ExtractionOptions extractionOptions;
    static ExtractionOptions ExtractionSnapshot() {
        std::lock_guard<std::mutex> lock(extractionOptionsMutex);
        return extractionOptions;
    }
    // Hotkey settings use [ctrl+][alt+][shift+][win+]<VK decimal>.
    // Accept readable key names and shortcuts entered as text as well.
    // No global hotkey is registered. Matching happens only in the focused list.
    static constexpr unsigned kShortcutCtrl = 0x100;
    static constexpr unsigned kShortcutAlt = 0x200;
    static constexpr unsigned kShortcutShift = 0x400;
    static constexpr unsigned kShortcutWin = 0x800;
    static constexpr unsigned kShortcutMods = 0xF00;
    enum class ExtractAction { Here, To, SelectedHere, SelectedTo, None };
    static constexpr std::array<PCWSTR,4> kExtractShortcutSettings{
        L"KeyBindings.ExtractHereShortcut",
        L"KeyBindings.ExtractToShortcut",
        L"KeyBindings.ExtractSelectedShortcut",
        L"KeyBindings.ExtractSelectedToShortcut",
    };
    static std::array<unsigned,kExtractShortcutSettings.size()> extractShortcuts{};
    static std::mutex extractShortcutsMutex;

    static std::wstring ShortcutTrimLower(std::wstring value) {
        const size_t first = value.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) return {};
        const size_t last = value.find_last_not_of(L" \t\r\n");
        value = value.substr(first,last - first + 1);
        std::transform(value.begin(),value.end(),value.begin(),
            [](wchar_t ch){return static_cast < wchar_t > (std::towlower(ch));});
        return value;
    }
    // Returns 0 on disabled OR invalid input. Invalid input never silently
    // reverts to the default and triggers a shortcut the user did not choose.
    static unsigned ParseExtractShortcut(std::wstring text) {
        text = ShortcutTrimLower(std::move(text));
        if (text.empty() || text == L"disabled" || text == L"none") return 0;
        if (text == L"ctrl_shift_e") return kShortcutCtrl | kShortcutShift | 'E';
        if (text == L"ctrl_alt_e") return kShortcutCtrl | kShortcutAlt | 'E';
        if (text == L"ctrl_shift_x") return kShortcutCtrl | kShortcutShift | 'X';

        unsigned modifiers = 0,virtualKey = 0;
        size_t at = 0;
        while (at < text.size()) {
            const size_t end = text.find(L'+',at);
            if (end == at) return 0;
            const std::wstring part = ShortcutTrimLower(text.substr(at,
                end == std::wstring::npos ? std::wstring::npos : end - at));
            if (part.empty()) return 0;
            const bool last = end == std::wstring::npos;
            unsigned bit = 0;
            if (part == L"ctrl" || part == L"control") bit = kShortcutCtrl;
            else if (part == L"alt") bit = kShortcutAlt;
            else if (part == L"shift") bit = kShortcutShift;
            else if (part == L"win" || part == L"windows") bit = kShortcutWin;
            if (bit) {
                if (last || (modifiers & bit) != 0) return 0;
                modifiers |= bit;
            } else {
                if (!last) return 0;
                if (part.size() == 1 && part[0] >= L'a' && part[0] <= L'z')
                    virtualKey = static_cast < unsigned > (part[0] - L'a' + L'A');
                else if (part.size() == 1 && part[0] >= L'0' && part[0] <= L'9')
                    virtualKey = static_cast < unsigned > (part[0]);
                else if (part.size() >= 2 && part[0] == L'f') {
                    const std::wstring number = part.substr(1);
                    if (number.empty() || number.find_first_not_of(L"0123456789") != std::wstring::npos) return 0;
                    const auto index = std::wcstoul(number.c_str(),nullptr,10);
                    if (index >= 1 && index <= 24) virtualKey = static_cast<unsigned>(VK_F1 + index - 1);
                } else if (part == L"space") virtualKey = VK_SPACE;
                else if (part == L"tab") virtualKey = VK_TAB;
                else if (part == L"enter" || part == L"return") virtualKey = VK_RETURN;
                else if (part == L"esc" || part == L"escape") virtualKey = VK_ESCAPE;
                else if (part == L"insert" || part == L"ins") virtualKey = VK_INSERT;
                else if (part == L"delete" || part == L"del") virtualKey = VK_DELETE;
                else if (part == L"home") virtualKey = VK_HOME;
                else if (part == L"end") virtualKey = VK_END;
                else if (part == L"pageup" || part == L"pgup") virtualKey = VK_PRIOR;
                else if (part == L"pagedown" || part == L"pgdn") virtualKey = VK_NEXT;
                else if (part == L"up") virtualKey = VK_UP;
                else if (part == L"down") virtualKey = VK_DOWN;
                else if (part == L"left") virtualKey = VK_LEFT;
                else if (part == L"right") virtualKey = VK_RIGHT;
                else if (part.find_first_not_of(L"0123456789") == std::wstring::npos) {
                    const unsigned long v = std::wcstoul(part.c_str(),nullptr,10);
                    if (v <= 255) virtualKey = static_cast < unsigned > (v);
                }
                if (!virtualKey) return 0;
            }
            if (last) break;
            at = end + 1;
        }
        // Avoid bare character keys, modifier-only shortcuts and potentially
        // destructive system keys. Windows can intercept Win-key combos.
        if (virtualKey < 8 || virtualKey > 255 || virtualKey == VK_SHIFT ||
            virtualKey == VK_CONTROL || virtualKey == VK_MENU ||
            virtualKey == VK_LWIN || virtualKey == VK_RWIN) return 0;
        if (!(modifiers & kShortcutMods) && !(virtualKey >= VK_F1 && virtualKey <= VK_F24))
            return 0;
        return modifiers | virtualKey;
    }
    static bool ConflictsWithPreviewShortcut(unsigned binding) {
        const unsigned key = binding & 0xFF;
        const unsigned mods = binding & kShortcutMods;
        if (key == VK_BACK || key == VK_RETURN || key == VK_RIGHT || key == VK_APPS) return true;
        if (key == VK_LEFT && (mods & kShortcutAlt)) return true;
        if (key == VK_F10 && mods == kShortcutShift) return true;
        if ((mods & kShortcutCtrl) && (key == 'A' || key == 'C' || key == VK_NEXT || key == VK_PRIOR))
            return true;
        return key == 'G' && (mods & (kShortcutCtrl | kShortcutShift)) == (kShortcutCtrl | kShortcutShift);
    }
    static void LoadExtractShortcuts() {
        std::array<unsigned,kExtractShortcutSettings.size()> shortcuts{};
        for (size_t i = 0; i < shortcuts.size(); ++i) {
            const auto setting = kExtractShortcutSettings[i];
            const auto text = ShortcutTrimLower(ArchivePreviewSettings::ReadText(setting));
            shortcuts[i] = ParseExtractShortcut(text);
            if (!shortcuts[i] && !text.empty() && text != L"disabled" && text != L"none")
                Wh_Log(L"Invalid %s setting; shortcut disabled",setting);
            if (shortcuts[i] && ConflictsWithPreviewShortcut(shortcuts[i])) {
                Wh_Log(L"%s conflicts with a preview shortcut; extraction shortcut disabled",setting);
                shortcuts[i] = 0;
            }
        }
        auto resolved = shortcuts;
        for (size_t i = 0; i < shortcuts.size(); ++i) {
            if (!shortcuts[i]) continue;
            for (size_t j = i + 1; j < shortcuts.size(); ++j) {
                if (shortcuts[i] != shortcuts[j]) continue;
                Wh_Log(L"%s and %s use the same shortcut; both disabled",
                    kExtractShortcutSettings[i],kExtractShortcutSettings[j]);
                resolved[i] = resolved[j] = 0;
            }
        }
        std::lock_guard<std::mutex> lock(extractShortcutsMutex);
        extractShortcuts = resolved;
    }
    static ExtractAction ExtractShortcutForKey(WPARAM key) {
        if (key > 255) return ExtractAction::None;
        unsigned current = static_cast<unsigned>(key);
        if (GetKeyState(VK_CONTROL) & 0x8000) current |= kShortcutCtrl;
        if (GetKeyState(VK_MENU) & 0x8000) current |= kShortcutAlt;
        if (GetKeyState(VK_SHIFT) & 0x8000) current |= kShortcutShift;
        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
            current |= kShortcutWin;
        std::lock_guard<std::mutex> lock(extractShortcutsMutex);
        const auto match = std::find(extractShortcuts.begin(),extractShortcuts.end(),current);
        return current && match != extractShortcuts.end() ?
            static_cast<ExtractAction>(match - extractShortcuts.begin()) : ExtractAction::None;
    }
    static bool IsRepeatedKey(LPARAM keyState) {
        return (keyState & (1L << 30)) != 0;
    }
    // Extraction status delay; the Shell, not this mod, prepares file content.
    static constexpr ULONGLONG kExtractionProgressDelayMs = 2000;
    static std::mutex extensionsMutex;
    static std::vector < std::wstring > extensions = {L".zip", L".rar", L".7z", L".tar"};
    // Keep the list intentionally small. A custom extension only enables the
    // Windows Shell preview provider; it does not install a decompressor.
    static bool ValidExtension(const std::wstring & ext) {
        if(ext.size() < 2 || ext.size() > 24 || ext[0] != L'.' || ext.back() == L'.')
            return false;
        bool previousDot = true;
        for(size_t i = 1;i < ext.size();++i) {
            const wchar_t c = ext[i];
            const bool letter = (c >= L'a' && c <= L'z');
            const bool digit = (c >= L'0' && c <= L'9');
            if (!(letter || digit || (c == L'.' && !previousDot))) return false;
            previousDot = c == L'.';
        }
        // Do not accidentally hijack common executable/system extensions.
        static constexpr const wchar_t * blocked[] = {
            L".exe",L".dll",L".sys",L".com",L".scr",L".msi",L".msp",
            L".bat",L".cmd",L".ps1",L".vbs",L".js",L".lnk",L".url",
            L".reg",L".cpl",L".pif",L".hta"
        };
        for(const wchar_t * b:blocked) if(ext == b) return false;
        // This setting changes which preview associations are redirected at runtime.
        // It must not accept arbitrary document, image or system extensions.
        static constexpr const wchar_t * archiveSuffixes[] = {
            L".zip",L".rar",L".7z",L".tar",L".gz",L".tgz",
            L".bz2",L".tbz",L".tbz2",L".xz",L".txz",
            L".zst",L".tzst",L".zstd",L".tar.gz",L".tar.bz2",
            L".tar.xz",L".tar.zst",L".tar.zstd",L".cab",L".iso",
            L".cpio",L".ar",L".lz4",L".tar.lz4"};
        for(const wchar_t * allowed:archiveSuffixes) if(ext == allowed) return true;
        return false;
    }
    static std::vector < std::wstring > ParseExtensions(const std::wstring & text) {
        std::vector < std::wstring > parsed;
        std::wstring token;
        bool overflow = false;
        auto append = [ & ]() {
            if(overflow) {
                Wh_Log(L"ArchivePreview: ignored extension token longer than 64 characters");
                token.clear(); overflow = false; return;
            }
            if(token.empty()) return;
            for(auto & c:token) c = static_cast < wchar_t > (towlower(c));
            if(token[0] != L'.') token.insert(token.begin(),L'.');
            if(ValidExtension(token) &&
               std::find(parsed.begin(),parsed.end(),token) == parsed.end() &&
               parsed.size() < 48) parsed.push_back(token);
            else if(!ValidExtension(token))
                Wh_Log(L"ArchivePreview: ignored invalid/unsafe extension: %s",token.c_str());
            token.clear();
        };
        for(wchar_t c:text) {
            if(c == L',' || c == L';' || iswspace(c)) append();
            else if(token.size() < 64 && !overflow) token += c;
            else overflow = true;
        }
        append();
        return parsed;
    }
    static std::vector < std::wstring > ExtensionSnapshot() {
        std::lock_guard < std::mutex > lock(extensionsMutex);
        return extensions;
    }
    static bool MatchArchive(const std::wstring & path) {
        std::lock_guard < std::mutex > lock(extensionsMutex);
        for(const auto & ext:extensions) {
            if(path.size() > ext.size() &&
               _wcsicmp(path.c_str() + path.size() - ext.size(),ext.c_str()) == 0)
                return true;
        }
        return false;
    }
    static void Load() {
        ExtractionOptions extraction;
        extraction.externalDrag = Wh_GetIntSetting(L"Extract.ExternalDragExtraction") != 0;
        const auto backend = ArchivePreviewSettings::ReadText(L"Extract.ExtractionBackend");
        if (_wcsicmp(backend.c_str(),L"auto") == 0) extraction.backend = 1;
        else if (_wcsicmp(backend.c_str(),L"7zip") == 0) extraction.backend = 2;
        else if (_wcsicmp(backend.c_str(),L"nanazip") == 0) extraction.backend = 3;
        else if (_wcsicmp(backend.c_str(),L"winrar") == 0) extraction.backend = 4;
        extraction.executable = ArchivePreviewSettings::ReadText(L"Extract.ExternalExtractorPath");
        {
            std::lock_guard<std::mutex> lock(extractionOptionsMutex);
            extractionOptions = std::move(extraction);
        }
        const auto text = ArchivePreviewSettings::ReadText(L"FilesAndSecurity.ArchiveExtensions");
        auto parsed = ParseExtensions(text);
        // A nonempty setting containing only invalid extensions must not
        // replace the last valid snapshot. An empty list is intentional.
        if(parsed.empty() && !text.empty() && text.find_first_not_of(L" \t\r\n,;") != std::wstring::npos) {
            Wh_Log(L"ArchivePreview: ignored entirely invalid extension setting; keeping last valid snapshot");
        } else {
            std::lock_guard < std::mutex > lock(extensionsMutex);
            extensions = std::move(parsed);
        }
        autoAllowInternetArchivePreview.store(
            Wh_GetIntSetting(L"FilesAndSecurity.AutoAllowInternetArchivePreview") != 0,std::memory_order_relaxed);
        hideHorizontalScrollbar.store(Wh_GetIntSetting(L"DisplayColumns.HideHorizontalScrollbar") != 0);
        const unsigned largeDates = Wh_GetIntSetting(L"DisplayColumns.ReadLargeArchiveDates") ? 1u : 0u;
        const unsigned largeSizes = Wh_GetIntSetting(L"DisplayColumns.ReadLargeArchiveSizes") ? 2u : 0u;
        largeMetadataMask.store(largeDates | largeSizes);
        const auto theme = ArchivePreviewSettings::ReadText(L"Preview.ThemeMode");
        int newMode = 0;
        if (_wcsicmp(theme.c_str(),L"light") == 0) newMode = 1;
        else if (_wcsicmp(theme.c_str(),L"dark") == 0) newMode = 2;
        themeMode.store(newMode);
        closeExplorerWindowOnUnload.store(
            Wh_GetIntSetting(L"ModLifecycle.CloseExplorerWindowOnUnload") != 0);
        ArchivePreviewText::LoadLanguage();
        LoadExtractShortcuts();
    }
    static bool Dark(bool systemDark) {
        const int mode = themeMode.load();
        return mode == 2 ? true : (mode == 1 ? false : systemDark);
    }
}

namespace ArchivePreviewText {
enum class Id : size_t {
    ColumnName, ColumnDate, ColumnType, ColumnSize, Back, ExtractHere, ExtractTo,
    ExtractSelected, Selected, Cancel, SelectedHere, SelectedTo, PickFolder,
    MainCount,
    Extracting = MainCount, PreparingFiles, StartingExtraction, Cancelling,
    ExternalProgress, CancelInExternal, ExtractorMissing, Estimate,
    ExtractionComplete, ExtractionCancelled, Count
};
using MainStrings = std::array<PCWSTR,static_cast<size_t>(Id::MainCount)>;
using NoticeStrings = std::array<PCWSTR,static_cast<size_t>(Id::Count) - static_cast<size_t>(Id::MainCount)>;

static constexpr NoticeStrings kNotices_en = {
    L"Extracting…", L"Preparing files…",
    L"Starting extraction…", L"Cancelling…",
    L"Progress in external application", L"Cancel extraction in the external application's window.",
    L"Extraction program not found. Check the mod settings.", L"estimate",
    L"Extraction completed.", L"Extraction cancelled."
};

static constexpr NoticeStrings kNotices_es = {
    L"Extrayendo…", L"Preparando archivos…",
    L"Iniciando extracción…", L"Cancelando…",
    L"Progreso en el programa externo", L"Cancela la extracción desde la ventana del programa externo.",
    L"No se encuentra el programa de extracción. Revisa los ajustes del mod.", L"estimación",
    L"Extracción completada.", L"Extracción cancelada."
};

static constexpr NoticeStrings kNotices_pt_BR = {
    L"Extraindo…", L"Preparando arquivos…",
    L"Iniciando extração…", L"Cancelando…",
    L"Progresso no programa externo", L"Cancele a extração pela janela do programa externo.",
    L"Programa de extração não encontrado. Confira as configurações do mod.", L"estimativa",
    L"Extração concluída.", L"Extração cancelada."
};

static constexpr NoticeStrings kNotices_pt_PT = {
    L"A extrair…", L"A preparar ficheiros…",
    L"A iniciar a extração…", L"A cancelar…",
    L"Progresso no programa externo", L"Cancele a extração na janela do programa externo.",
    L"Programa de extração não encontrado. Verifique as definições do mod.", L"estimativa",
    L"Extração concluída.", L"Extração cancelada."
};

static constexpr NoticeStrings kNotices_de = {
    L"Wird entpackt…", L"Dateien werden vorbereitet…",
    L"Entpacken wird gestartet…", L"Wird abgebrochen…",
    L"Fortschritt im externen Programm", L"Brechen Sie den Vorgang im Fenster des externen Programms ab.",
    L"Entpackprogramm nicht gefunden. Prüfen Sie die Mod-Einstellungen.", L"Schätzung",
    L"Entpacken abgeschlossen.", L"Entpacken abgebrochen."
};

static constexpr NoticeStrings kNotices_fr = {
    L"Extraction en cours…", L"Préparation des fichiers…",
    L"Démarrage de l’extraction…", L"Annulation en cours…",
    L"Progression dans le programme externe", L"Annulez l’extraction dans la fenêtre du programme externe.",
    L"Programme d’extraction introuvable. Vérifiez les paramètres du mod.", L"estimation",
    L"Extraction terminée.", L"Extraction annulée."
};

static constexpr NoticeStrings kNotices_it = {
    L"Estrazione in corso…", L"Preparazione dei file…",
    L"Avvio dell’estrazione…", L"Annullamento in corso…",
    L"Avanzamento nel programma esterno", L"Annulla l’estrazione dalla finestra del programma esterno.",
    L"Programma di estrazione non trovato. Controlla le impostazioni del mod.", L"stima",
    L"Estrazione completata.", L"Estrazione annullata."
};

static constexpr NoticeStrings kNotices_nl = {
    L"Bezig met uitpakken…", L"Bestanden voorbereiden…",
    L"Uitpakken starten…", L"Bezig met annuleren…",
    L"Voortgang in extern programma", L"Annuleer het uitpakken in het venster van het externe programma.",
    L"Uitpakprogramma niet gevonden. Controleer de modinstellingen.", L"schatting",
    L"Uitpakken voltooid.", L"Uitpakken geannuleerd."
};

static constexpr NoticeStrings kNotices_ja = {
    L"展開中…", L"ファイルを準備中…",
    L"展開を開始中…", L"キャンセル中…",
    L"外部アプリでの進行状況", L"外部アプリのウィンドウで展開をキャンセルしてください。",
    L"展開アプリが見つかりません。Modの設定を確認してください。", L"推定",
    L"展開が完了しました。", L"展開をキャンセルしました。"
};

static constexpr NoticeStrings kNotices_ko = {
    L"압축 푸는 중…", L"파일 준비 중…",
    L"압축 풀기 시작 중…", L"취소 중…",
    L"외부 프로그램에서 진행 중", L"외부 프로그램 창에서 압축 풀기를 취소하세요.",
    L"압축 풀기 프로그램을 찾을 수 없습니다. 모드 설정을 확인하세요.", L"추정",
    L"압축 풀기가 완료되었습니다.", L"압축 풀기가 취소되었습니다."
};

static constexpr NoticeStrings kNotices_zh_Hans = {
    L"正在解压…", L"正在准备文件…",
    L"正在开始解压…", L"正在取消…",
    L"进度在外部程序中显示", L"请在外部程序的窗口中取消解压。",
    L"找不到解压程序。请检查模组设置。", L"估计",
    L"解压完成。", L"解压已取消。"
};

static constexpr NoticeStrings kNotices_zh_Hant = {
    L"正在解壓縮…", L"正在準備檔案…",
    L"正在開始解壓縮…", L"正在取消…",
    L"進度顯示於外部程式", L"請在外部程式的視窗中取消解壓縮。",
    L"找不到解壓縮程式。請檢查模組設定。", L"估計",
    L"解壓縮完成。", L"解壓縮已取消。"
};

static constexpr NoticeStrings kNotices_ru = {
    L"Извлечение…", L"Подготовка файлов…",
    L"Запуск извлечения…", L"Отмена…",
    L"Ход работы во внешней программе", L"Отмените извлечение в окне внешней программы.",
    L"Программа для извлечения не найдена. Проверьте настройки мода.", L"оценка",
    L"Извлечение завершено.", L"Извлечение отменено."
};

static constexpr NoticeStrings kNotices_uk = {
    L"Видобування…", L"Підготовка файлів…",
    L"Початок видобування…", L"Скасування…",
    L"Перебіг у зовнішній програмі", L"Скасуйте видобування у вікні зовнішньої програми.",
    L"Програму для видобування не знайдено. Перевірте налаштування моду.", L"оцінка",
    L"Видобування завершено.", L"Видобування скасовано."
};

static constexpr NoticeStrings kNotices_ar = {
    L"جارٍ الاستخراج…", L"جارٍ تجهيز الملفات…",
    L"جارٍ بدء الاستخراج…", L"جارٍ الإلغاء…",
    L"التقدم في البرنامج الخارجي", L"ألغِ الاستخراج من نافذة البرنامج الخارجي.",
    L"لم يتم العثور على برنامج الاستخراج. تحقّق من إعدادات الإضافة.", L"تقدير",
    L"اكتمل الاستخراج.", L"تم إلغاء الاستخراج."
};

static constexpr NoticeStrings kNotices_he = {
    L"מחלץ…", L"מכין קבצים…",
    L"מתחיל חילוץ…", L"מבטל…",
    L"ההתקדמות מוצגת ביישום החיצוני", L"בטל את החילוץ בחלון היישום החיצוני.",
    L"תוכנת החילוץ לא נמצאה. בדוק את הגדרות המוד.", L"הערכה",
    L"החילוץ הושלם.", L"החילוץ בוטל."
};

static constexpr NoticeStrings kNotices_tr = {
    L"Çıkarılıyor…", L"Dosyalar hazırlanıyor…",
    L"Çıkarma başlatılıyor…", L"İptal ediliyor…",
    L"İlerleme harici programda gösteriliyor", L"Çıkarmayı harici programın penceresinden iptal edin.",
    L"Çıkarma programı bulunamadı. Mod ayarlarını kontrol edin.", L"tahmini",
    L"Çıkarma tamamlandı.", L"Çıkarma iptal edildi."
};

static constexpr NoticeStrings kNotices_pl = {
    L"Rozpakowywanie…", L"Przygotowywanie plików…",
    L"Rozpoczynanie rozpakowywania…", L"Anulowanie…",
    L"Postęp w programie zewnętrznym", L"Anuluj rozpakowywanie w oknie programu zewnętrznego.",
    L"Nie znaleziono programu do rozpakowywania. Sprawdź ustawienia moda.", L"szacunek",
    L"Rozpakowywanie zakończone.", L"Rozpakowywanie anulowane."
};

static constexpr NoticeStrings kNotices_hi = {
    L"निकाला जा रहा है…", L"फ़ाइलें तैयार हो रही हैं…",
    L"निकालना शुरू हो रहा है…", L"रद्द किया जा रहा है…",
    L"बाहरी प्रोग्राम में प्रगति", L"बाहरी प्रोग्राम की विंडो में निकालना रद्द करें।",
    L"निकालने वाला प्रोग्राम नहीं मिला। मॉड की सेटिंग देखें।", L"अनुमान",
    L"निकालना पूरा हुआ।", L"निकालना रद्द हुआ।"
};

static constexpr NoticeStrings kNotices_id = {
    L"Mengekstrak…", L"Menyiapkan file…",
    L"Memulai ekstraksi…", L"Membatalkan…",
    L"Progres di program eksternal", L"Batalkan ekstraksi di jendela program eksternal.",
    L"Program ekstraksi tidak ditemukan. Periksa pengaturan mod.", L"perkiraan",
    L"Ekstraksi selesai.", L"Ekstraksi dibatalkan."
};

static constexpr NoticeStrings kNotices_vi = {
    L"Đang giải nén…", L"Đang chuẩn bị tệp…",
    L"Đang bắt đầu giải nén…", L"Đang hủy…",
    L"Tiến độ trong chương trình bên ngoài", L"Hủy giải nén trong cửa sổ chương trình bên ngoài.",
    L"Không tìm thấy chương trình giải nén. Hãy kiểm tra cài đặt của mod.", L"ước tính",
    L"Đã giải nén xong.", L"Đã hủy giải nén."
};

static constexpr NoticeStrings kNotices_th = {
    L"กำลังแตกไฟล์…", L"กำลังเตรียมไฟล์…",
    L"กำลังเริ่มแตกไฟล์…", L"กำลังยกเลิก…",
    L"ดูความคืบหน้าในโปรแกรมภายนอก", L"ยกเลิกการแตกไฟล์ในหน้าต่างของโปรแกรมภายนอก",
    L"ไม่พบโปรแกรมแตกไฟล์ โปรดตรวจสอบการตั้งค่าม็อด", L"โดยประมาณ",
    L"แตกไฟล์เสร็จแล้ว", L"ยกเลิกการแตกไฟล์แล้ว"
};

static constexpr NoticeStrings kNotices_fa = {
    L"در حال استخراج…", L"در حال آماده\u200Cسازی فایل\u200Cها…",
    L"در حال شروع استخراج…", L"در حال لغو…",
    L"پیشرفت در برنامهٔ خارجی", L"استخراج را از پنجرهٔ برنامهٔ خارجی لغو کنید.",
    L"برنامهٔ استخراج پیدا نشد. تنظیمات افزونه را بررسی کنید.", L"برآورد",
    L"استخراج کامل شد.", L"استخراج لغو شد."
};

struct Catalog {
    PCWSTR tag;
    bool rightToLeft;
    MainStrings controls;
    const NoticeStrings * notices;
};

// Cherokee, Latin Inuktitut, and K'iche' use terms from the Microsoft
// Terminology Collection. Keep the settings choices and catalog tags in sync.
// Windows variants with shared wording resolve to the same catalog.
static constexpr Catalog kCatalogs[] = {
    {L"en", false, {
        L"Name", L"Modified", L"Type", L"Size",
        L"Back", L"Extract here", L"Extract to…", L"Extract selected",
        L"Selected", L"Cancel", L"Extract selected here", L"Extract selected to…",
        L"Extract archive to…"
    }, &kNotices_en},
    {L"ar", true, {
        L"الاسم", L"تاريخ التعديل", L"النوع", L"الحجم",
        L"رجوع", L"استخراج هنا", L"استخراج إلى…", L"استخراج المحدد",
        L"المحدد", L"إلغاء", L"استخراج المحدد هنا", L"استخراج المحدد إلى…",
        L"استخراج الأرشيف إلى…"
    }, &kNotices_ar},
    {L"bg", false, {
        L"Име", L"Променен на", L"Тип", L"Размер",
        L"Назад", L"Разархивирай тук", L"Разархивирай в…", L"Разархивирай избраното",
        L"Избрано", L"Отказ", L"Разархивирай избраното тук", L"Разархивирай избраното в…",
        L"Разархивирай архива в…"
    }, nullptr},
    {L"ca", false, {
        L"Nom", L"Data de modificació", L"Tipus", L"Mida",
        L"Enrere", L"Extreu aquí", L"Extreu a…", L"Extreu la selecció",
        L"Selecció", L"Cancel·la", L"Extreu la selecció aquí", L"Extreu la selecció a…",
        L"Extreu el fitxer comprimit a…"
    }, nullptr},
    {L"cs", false, {
        L"Název", L"Změněno", L"Typ", L"Velikost",
        L"Zpět", L"Rozbalit zde", L"Rozbalit do…", L"Rozbalit výběr",
        L"Výběr", L"Zrušit", L"Rozbalit výběr zde", L"Rozbalit výběr do…",
        L"Rozbalit archiv do…"
    }, nullptr},
    {L"da", false, {
        L"Navn", L"Ændret", L"Type", L"Størrelse",
        L"Tilbage", L"Udpak her", L"Udpak til…", L"Udpak valgte",
        L"Valgte", L"Annuller", L"Udpak valgte her", L"Udpak valgte til…",
        L"Udpak arkiv til…"
    }, nullptr},
    {L"de", false, {
        L"Name", L"Geändert am", L"Typ", L"Größe",
        L"Zurück", L"Hier entpacken", L"Entpacken nach…", L"Auswahl entpacken",
        L"Auswahl", L"Abbrechen", L"Auswahl hier entpacken", L"Auswahl entpacken nach…",
        L"Archiv entpacken nach…"
    }, &kNotices_de},
    {L"el", false, {
        L"Όνομα", L"Τροποποιήθηκε", L"Τύπος", L"Μέγεθος",
        L"Πίσω", L"Εξαγωγή εδώ", L"Εξαγωγή σε…", L"Εξαγωγή επιλογής",
        L"Επιλογή", L"Ακύρωση", L"Εξαγωγή επιλογής εδώ", L"Εξαγωγή επιλογής σε…",
        L"Εξαγωγή αρχείου σε…"
    }, nullptr},
    {L"es", false, {
        L"Nombre", L"Modificación", L"Tipo", L"Tamaño",
        L"Atrás", L"Extraer aquí", L"Extraer en…", L"Extraer selección",
        L"Selección", L"Cancelar", L"Extraer selección aquí", L"Extraer selección en…",
        L"Extraer archivo en…"
    }, &kNotices_es},
    {L"et", false, {
        L"Nimi", L"Muudetud", L"Tüüp", L"Suurus",
        L"Tagasi", L"Paki siia lahti", L"Paki lahti asukohta…", L"Paki valitud lahti",
        L"Valitud", L"Loobu", L"Paki valitud siia lahti", L"Paki valitud lahti asukohta…",
        L"Paki arhiiv lahti asukohta…"
    }, nullptr},
    {L"eu", false, {
        L"Izena", L"Aldatze-data", L"Mota", L"Tamaina",
        L"Atzera", L"Atera hemen", L"Atera hona…", L"Atera hautatutakoa",
        L"Hautatua", L"Utzi", L"Atera hautatutakoa hemen", L"Atera hautatutakoa hona…",
        L"Atera artxiboa hona…"
    }, nullptr},
    {L"fi", false, {
        L"Nimi", L"Muokattu", L"Tyyppi", L"Koko",
        L"Takaisin", L"Pura tähän", L"Pura kohteeseen…", L"Pura valitut",
        L"Valitut", L"Peruuta", L"Pura valitut tähän", L"Pura valitut kohteeseen…",
        L"Pura arkisto kohteeseen…"
    }, nullptr},
    {L"fr", false, {
        L"Nom", L"Modifié le", L"Type", L"Taille",
        L"Retour", L"Extraire ici", L"Extraire vers…", L"Extraire la sélection",
        L"Sélection", L"Annuler", L"Extraire la sélection ici", L"Extraire la sélection vers…",
        L"Extraire l’archive vers…"
    }, &kNotices_fr},
    {L"gl", false, {
        L"Nome", L"Data de modificación", L"Tipo", L"Tamaño",
        L"Atrás", L"Extraer aquí", L"Extraer en…", L"Extraer a selección",
        L"Selección", L"Cancelar", L"Extraer a selección aquí", L"Extraer a selección en…",
        L"Extraer o arquivo en…"
    }, nullptr},
    {L"he", true, {
        L"שם", L"תאריך שינוי", L"סוג", L"גודל",
        L"חזרה", L"חלץ כאן", L"חלץ אל…", L"חלץ את הבחירה",
        L"בחירה", L"ביטול", L"חלץ את הבחירה כאן", L"חלץ את הבחירה אל…",
        L"חלץ את הארכיון אל…"
    }, &kNotices_he},
    {L"hr", false, {
        L"Naziv", L"Izmijenjeno", L"Vrsta", L"Veličina",
        L"Natrag", L"Raspakiraj ovdje", L"Raspakiraj u…", L"Raspakiraj odabrano",
        L"Odabrano", L"Odustani", L"Raspakiraj odabrano ovdje", L"Raspakiraj odabrano u…",
        L"Raspakiraj arhivu u…"
    }, nullptr},
    {L"hu", false, {
        L"Név", L"Módosítva", L"Típus", L"Méret",
        L"Vissza", L"Kibontás ide", L"Kibontás ide: …", L"Kijelöltek kibontása",
        L"Kijelöltek", L"Mégse", L"Kijelöltek kibontása ide", L"Kijelöltek kibontása ide: …",
        L"Archívum kibontása ide: …"
    }, nullptr},
    {L"id", false, {
        L"Nama", L"Diubah", L"Jenis", L"Ukuran",
        L"Kembali", L"Ekstrak di sini", L"Ekstrak ke…", L"Ekstrak pilihan",
        L"Pilihan", L"Batal", L"Ekstrak pilihan di sini", L"Ekstrak pilihan ke…",
        L"Ekstrak arsip ke…"
    }, &kNotices_id},
    {L"it", false, {
        L"Nome", L"Data di modifica", L"Tipo", L"Dimensione",
        L"Indietro", L"Estrai qui", L"Estrai in…", L"Estrai selezione",
        L"Selezione", L"Annulla", L"Estrai selezione qui", L"Estrai selezione in…",
        L"Estrai archivio in…"
    }, &kNotices_it},
    {L"ja", false, {
        L"名前", L"更新日時", L"種類", L"サイズ",
        L"戻る", L"ここに展開", L"展開先を選択…", L"選択項目を展開",
        L"選択項目", L"キャンセル", L"選択項目をここに展開", L"選択項目の展開先を選択…",
        L"アーカイブの展開先を選択…"
    }, &kNotices_ja},
    {L"ko", false, {
        L"이름", L"수정한 날짜", L"유형", L"크기",
        L"뒤로", L"여기에 풀기", L"풀 위치 선택…", L"선택 항목 풀기",
        L"선택 항목", L"취소", L"선택 항목을 여기에 풀기", L"선택 항목을 풀 위치 선택…",
        L"압축을 풀 위치 선택…"
    }, &kNotices_ko},
    {L"lt", false, {
        L"Pavadinimas", L"Keitimo data", L"Tipas", L"Dydis",
        L"Atgal", L"Išskleisti čia", L"Išskleisti į…", L"Išskleisti pažymėtus",
        L"Pažymėti", L"Atšaukti", L"Išskleisti pažymėtus čia", L"Išskleisti pažymėtus į…",
        L"Išskleisti archyvą į…"
    }, nullptr},
    {L"lv", false, {
        L"Nosaukums", L"Mainīšanas datums", L"Tips", L"Lielums",
        L"Atpakaļ", L"Atspiest šeit", L"Atspiest uz…", L"Atspiest atlasīto",
        L"Atlasīts", L"Atcelt", L"Atspiest atlasīto šeit", L"Atspiest atlasīto uz…",
        L"Atspiest arhīvu uz…"
    }, nullptr},
    {L"nb", false, {
        L"Navn", L"Endret", L"Type", L"Størrelse",
        L"Tilbake", L"Pakk ut her", L"Pakk ut til…", L"Pakk ut valgte",
        L"Valgte", L"Avbryt", L"Pakk ut valgte her", L"Pakk ut valgte til…",
        L"Pakk ut arkiv til…"
    }, nullptr},
    {L"nl", false, {
        L"Naam", L"Gewijzigd op", L"Type", L"Grootte",
        L"Terug", L"Hier uitpakken", L"Uitpakken naar…", L"Selectie uitpakken",
        L"Selectie", L"Annuleren", L"Selectie hier uitpakken", L"Selectie uitpakken naar…",
        L"Archief uitpakken naar…"
    }, &kNotices_nl},
    {L"pl", false, {
        L"Nazwa", L"Data modyfikacji", L"Typ", L"Rozmiar",
        L"Wstecz", L"Rozpakuj tutaj", L"Rozpakuj do…", L"Rozpakuj zaznaczone",
        L"Zaznaczone", L"Anuluj", L"Rozpakuj zaznaczone tutaj", L"Rozpakuj zaznaczone do…",
        L"Rozpakuj archiwum do…"
    }, &kNotices_pl},
    {L"pt-BR", false, {
        L"Nome", L"Data de modificação", L"Tipo", L"Tamanho",
        L"Voltar", L"Extrair aqui", L"Extrair para…", L"Extrair seleção",
        L"Seleção", L"Cancelar", L"Extrair seleção aqui", L"Extrair seleção para…",
        L"Extrair arquivo para…"
    }, &kNotices_pt_BR},
    {L"pt-PT", false, {
        L"Nome", L"Data de modificação", L"Tipo", L"Tamanho",
        L"Voltar", L"Extrair aqui", L"Extrair para…", L"Extrair seleção",
        L"Seleção", L"Cancelar", L"Extrair seleção aqui", L"Extrair seleção para…",
        L"Extrair ficheiro para…"
    }, &kNotices_pt_PT},
    {L"ro", false, {
        L"Nume", L"Data modificării", L"Tip", L"Dimensiune",
        L"Înapoi", L"Extrage aici", L"Extrage în…", L"Extrage selecția",
        L"Selecție", L"Anulează", L"Extrage selecția aici", L"Extrage selecția în…",
        L"Extrage arhiva în…"
    }, nullptr},
    {L"ru", false, {
        L"Имя", L"Дата изменения", L"Тип", L"Размер",
        L"Назад", L"Извлечь здесь", L"Извлечь в…", L"Извлечь выбранное",
        L"Выбранное", L"Отмена", L"Извлечь выбранное здесь", L"Извлечь выбранное в…",
        L"Извлечь архив в…"
    }, &kNotices_ru},
    {L"sk", false, {
        L"Názov", L"Dátum úpravy", L"Typ", L"Veľkosť",
        L"Späť", L"Rozbaliť sem", L"Rozbaliť do…", L"Rozbaliť výber",
        L"Výber", L"Zrušiť", L"Rozbaliť výber sem", L"Rozbaliť výber do…",
        L"Rozbaliť archív do…"
    }, nullptr},
    {L"sl", false, {
        L"Ime", L"Spremenjeno", L"Vrsta", L"Velikost",
        L"Nazaj", L"Razširi sem", L"Razširi v…", L"Razširi izbor",
        L"Izbor", L"Prekliči", L"Razširi izbor sem", L"Razširi izbor v…",
        L"Razširi arhiv v…"
    }, nullptr},
    {L"sr-Latn", false, {
        L"Ime", L"Datum izmene", L"Tip", L"Veličina",
        L"Nazad", L"Raspakuj ovde", L"Raspakuj u…", L"Raspakuj izabrano",
        L"Izabrano", L"Otkaži", L"Raspakuj izabrano ovde", L"Raspakuj izabrano u…",
        L"Raspakuj arhivu u…"
    }, nullptr},
    {L"sr-Cyrl", false, {
        L"Име", L"Датум измене", L"Тип", L"Величина",
        L"Назад", L"Распакуј овде", L"Распакуј у…", L"Распакуј изабрано",
        L"Изабрано", L"Откажи", L"Распакуј изабрано овде", L"Распакуј изабрано у…",
        L"Распакуј архиву у…"
    }, nullptr},
    {L"sv", false, {
        L"Namn", L"Ändrad", L"Typ", L"Storlek",
        L"Tillbaka", L"Packa upp här", L"Packa upp till…", L"Packa upp markerade",
        L"Markerade", L"Avbryt", L"Packa upp markerade här", L"Packa upp markerade till…",
        L"Packa upp arkiv till…"
    }, nullptr},
    {L"th", false, {
        L"ชื่อ", L"วันที่แก้ไข", L"ชนิด", L"ขนาด",
        L"ย้อนกลับ", L"แตกไฟล์ที่นี่", L"แตกไฟล์ไปยัง…", L"แตกไฟล์ที่เลือก",
        L"ที่เลือก", L"ยกเลิก", L"แตกไฟล์ที่เลือกที่นี่", L"แตกไฟล์ที่เลือกไปยัง…",
        L"แตกไฟล์เก็บถาวรไปยัง…"
    }, &kNotices_th},
    {L"tr", false, {
        L"Ad", L"Değiştirme tarihi", L"Tür", L"Boyut",
        L"Geri", L"Buraya çıkar", L"Konuma çıkar…", L"Seçilenleri çıkar",
        L"Seçilenler", L"İptal", L"Seçilenleri buraya çıkar", L"Seçilenleri konuma çıkar…",
        L"Arşivi konuma çıkar…"
    }, &kNotices_tr},
    {L"uk", false, {
        L"Ім’я", L"Дата змінення", L"Тип", L"Розмір",
        L"Назад", L"Видобути сюди", L"Видобути до…", L"Видобути вибране",
        L"Вибране", L"Скасувати", L"Видобути вибране сюди", L"Видобути вибране до…",
        L"Видобути архів до…"
    }, &kNotices_uk},
    {L"vi", false, {
        L"Tên", L"Ngày sửa đổi", L"Loại", L"Kích thước",
        L"Quay lại", L"Giải nén tại đây", L"Giải nén vào…", L"Giải nén mục đã chọn",
        L"Đã chọn", L"Hủy", L"Giải nén mục đã chọn tại đây", L"Giải nén mục đã chọn vào…",
        L"Giải nén tệp nén vào…"
    }, &kNotices_vi},
    {L"zh-Hans", false, {
        L"名称", L"修改日期", L"类型", L"大小",
        L"返回", L"解压到此处", L"解压到…", L"解压所选项",
        L"所选项", L"取消", L"将所选项解压到此处", L"将所选项解压到…",
        L"将压缩包解压到…"
    }, &kNotices_zh_Hans},
    {L"zh-Hant", false, {
        L"名稱", L"修改日期", L"類型", L"大小",
        L"返回", L"解壓縮至此處", L"解壓縮至…", L"解壓縮所選項目",
        L"所選項目", L"取消", L"將所選項目解壓縮至此處", L"將所選項目解壓縮至…",
        L"將壓縮檔解壓縮至…"
    }, &kNotices_zh_Hant},
    {L"af", false, {
        L"Naam", L"Gewysig", L"Tipe", L"Grootte",
        L"Terug", L"Pak hier uit", L"Pak uit na…", L"Pak gekose items uit",
        L"Gekies", L"Kanselleer", L"Pak gekose items hier uit", L"Pak gekose items uit na…",
        L"Pak argief uit na…"
    }, nullptr},
    {L"sq", false, {
        L"Emri", L"Data e ndryshimit", L"Lloji", L"Madhësia",
        L"Prapa", L"Nxirr këtu", L"Nxirr te…", L"Nxirr përzgjedhjen",
        L"Përzgjedhja", L"Anulo", L"Nxirr përzgjedhjen këtu", L"Nxirr përzgjedhjen te…",
        L"Nxirr arkivin te…"
    }, nullptr},
    {L"am", false, {
        L"ስም", L"የተቀየረበት ቀን", L"ዓይነት", L"መጠን",
        L"ተመለስ", L"እዚህ አውጣ", L"ወደ… አውጣ", L"የተመረጡትን አውጣ",
        L"የተመረጡ", L"ሰርዝ", L"የተመረጡትን እዚህ አውጣ", L"የተመረጡትን ወደ… አውጣ",
        L"የታመቀውን ፋይል ወደ… አውጣ"
    }, nullptr},
    {L"hy", false, {
        L"Անուն", L"Փոփոխման ամսաթիվ", L"Տեսակ", L"Չափ",
        L"Հետ", L"Ապափաթեթավորել այստեղ", L"Ապափաթեթավորել դեպի…", L"Ապափաթեթավորել ընտրվածը",
        L"Ընտրված", L"Չեղարկել", L"Ապափաթեթավորել ընտրվածն այստեղ", L"Ապափաթեթավորել ընտրվածը դեպի…",
        L"Ապափաթեթավորել արխիվը դեպի…"
    }, nullptr},
    {L"as", false, {
        L"নাম", L"সলনিৰ তাৰিখ", L"প্ৰকাৰ", L"আকাৰ",
        L"পিছলৈ", L"ইয়াত উলিয়াওক", L"ইয়ালৈ উলিয়াওক…", L"নিৰ্বাচিত বস্তু উলিয়াওক",
        L"নিৰ্বাচিত", L"বাতিল কৰক", L"নিৰ্বাচিত বস্তু ইয়াত উলিয়াওক", L"নিৰ্বাচিত বস্তু ইয়ালৈ উলিয়াওক…",
        L"সংকুচিত ফাইল ইয়ালৈ উলিয়াওক…"
    }, nullptr},
    {L"az", false, {
        L"Ad", L"Dəyişiklik tarixi", L"Növ", L"Ölçü",
        L"Geri", L"Buraya çıxar", L"Qovluğa çıxar…", L"Seçilənləri çıxar",
        L"Seçilənlər", L"Ləğv et", L"Seçilənləri buraya çıxar", L"Seçilənləri qovluğa çıxar…",
        L"Arxivi qovluğa çıxar…"
    }, nullptr},
    {L"be", false, {
        L"Імя", L"Дата змянення", L"Тып", L"Памер",
        L"Назад", L"Выняць сюды", L"Выняць у…", L"Выняць выбранае",
        L"Выбранае", L"Скасаваць", L"Выняць выбранае сюды", L"Выняць выбранае ў…",
        L"Выняць архіў у…"
    }, nullptr},
    {L"bn", false, {
        L"নাম", L"পরিবর্তনের তারিখ", L"ধরন", L"আকার",
        L"ফিরে যান", L"এখানে বের করুন", L"গন্তব্যে বের করুন…", L"নির্বাচিতগুলো বের করুন",
        L"নির্বাচিত", L"বাতিল", L"নির্বাচিতগুলো এখানে বের করুন", L"নির্বাচিতগুলো গন্তব্যে বের করুন…",
        L"আর্কাইভ গন্তব্যে বের করুন…"
    }, nullptr},
    {L"bs", false, {
        L"Naziv", L"Datum izmjene", L"Vrsta", L"Veličina",
        L"Nazad", L"Raspakuj ovdje", L"Raspakuj u…", L"Raspakuj odabrano",
        L"Odabrano", L"Otkaži", L"Raspakuj odabrano ovdje", L"Raspakuj odabrano u…",
        L"Raspakuj arhivu u…"
    }, nullptr},
    {L"fil", false, {
        L"Pangalan", L"Binago", L"Uri", L"Laki",
        L"Bumalik", L"I-extract dito", L"I-extract sa…", L"I-extract ang napili",
        L"Napili", L"Kanselahin", L"I-extract ang napili dito", L"I-extract ang napili sa…",
        L"I-extract ang archive sa…"
    }, nullptr},
    {L"ka", false, {
        L"სახელი", L"შეცვლის თარიღი", L"ტიპი", L"ზომა",
        L"უკან", L"აქ ამოარქივება", L"ამოარქივება საქაღალდეში…", L"არჩეულის ამოარქივება",
        L"არჩეული", L"გაუქმება", L"არჩეულის აქ ამოარქივება", L"არჩეულის ამოარქივება საქაღალდეში…",
        L"არქივის ამოარქივება საქაღალდეში…"
    }, nullptr},
    {L"gu", false, {
        L"નામ", L"ફેરફાર તારીખ", L"પ્રકાર", L"કદ",
        L"પાછા", L"અહીં કાઢો", L"આ ફોલ્ડરમાં કાઢો…", L"પસંદ કરેલ કાઢો",
        L"પસંદ કરેલ", L"રદ કરો", L"પસંદ કરેલ અહીં કાઢો", L"પસંદ કરેલ ફોલ્ડરમાં કાઢો…",
        L"આર્કાઇવને ફોલ્ડરમાં કાઢો…"
    }, nullptr},
    {L"hi", false, {
        L"नाम", L"संशोधन की तारीख", L"प्रकार", L"आकार",
        L"वापस", L"यहाँ निकालें", L"फ़ोल्डर में निकालें…", L"चुने हुए आइटम निकालें",
        L"चुने हुए", L"रद्द करें", L"चुने हुए आइटम यहाँ निकालें", L"चुने हुए आइटम फ़ोल्डर में निकालें…",
        L"संग्रह को फ़ोल्डर में निकालें…"
    }, &kNotices_hi},
    {L"is", false, {
        L"Heiti", L"Breytt", L"Gerð", L"Stærð",
        L"Til baka", L"Afþjappa hér", L"Afþjappa í…", L"Afþjappa völdu",
        L"Valið", L"Hætta við", L"Afþjappa völdu hér", L"Afþjappa völdu í…",
        L"Afþjappa safnskrá í…"
    }, nullptr},
    {L"ga", false, {
        L"Ainm", L"Dáta athraithe", L"Cineál", L"Méid",
        L"Ar ais", L"Bain amach anseo", L"Bain amach go…", L"Bain amach na míreanna roghnaithe",
        L"Roghnaithe", L"Cealaigh", L"Bain amach na míreanna roghnaithe anseo", L"Bain amach na míreanna roghnaithe go…",
        L"Bain amach an chartlann go…"
    }, nullptr},
    {L"kn", false, {
        L"ಹೆಸರು", L"ಬದಲಾಯಿಸಿದ ದಿನಾಂಕ", L"ವಿಧ", L"ಗಾತ್ರ",
        L"ಹಿಂದೆ", L"ಇಲ್ಲಿ ಹೊರತೆಗೆಯಿರಿ", L"ಈ ಫೋಲ್ಡರ್\u200Cಗೆ ಹೊರತೆಗೆಯಿರಿ…", L"ಆಯ್ದವುಗಳನ್ನು ಹೊರತೆಗೆಯಿರಿ",
        L"ಆಯ್ದವು", L"ರದ್ದುಮಾಡಿ", L"ಆಯ್ದವುಗಳನ್ನು ಇಲ್ಲಿ ಹೊರತೆಗೆಯಿರಿ", L"ಆಯ್ದವುಗಳನ್ನು ಫೋಲ್ಡರ್\u200Cಗೆ ಹೊರತೆಗೆಯಿರಿ…",
        L"ಆರ್ಕೈವ್ ಅನ್ನು ಫೋಲ್ಡರ್\u200Cಗೆ ಹೊರತೆಗೆಯಿರಿ…"
    }, nullptr},
    {L"kk", false, {
        L"Атауы", L"Өзгертілген күні", L"Түрі", L"Өлшемі",
        L"Артқа", L"Осы жерге шығару", L"Қалтаға шығару…", L"Таңдалғандарды шығару",
        L"Таңдалғандар", L"Болдырмау", L"Таңдалғандарды осы жерге шығару", L"Таңдалғандарды қалтаға шығару…",
        L"Мұрағатты қалтаға шығару…"
    }, nullptr},
    {L"km", false, {
        L"ឈ្មោះ", L"កាលបរិច្ឆេទកែប្រែ", L"ប្រភេទ", L"ទំហំ",
        L"ថយក្រោយ", L"ពន្លានៅទីនេះ", L"ពន្លាទៅ…", L"ពន្លាធាតុដែលបានជ្រើស",
        L"បានជ្រើស", L"បោះបង់", L"ពន្លាធាតុដែលបានជ្រើសនៅទីនេះ", L"ពន្លាធាតុដែលបានជ្រើសទៅ…",
        L"ពន្លាឯកសារបង្ហាប់ទៅ…"
    }, nullptr},
    {L"kok", false, {
        L"नांव", L"बदलाची तारीख", L"प्रकार", L"आकार",
        L"फाटीं", L"हांगा काडात", L"फोल्डरांत काडात…", L"वेंचिल्लें काडात",
        L"वेंचिल्लें", L"रद्द करात", L"वेंचिल्लें हांगा काडात", L"वेंचिल्लें फोल्डरांत काडात…",
        L"संकुचीत फायल फोल्डरांत काडात…"
    }, nullptr},
    {L"lo", false, {
        L"ຊື່", L"ວັນທີແກ້ໄຂ", L"ປະເພດ", L"ຂະໜາດ",
        L"ກັບຄືນ", L"ແຕກໄຟລ໌ບ່ອນນີ້", L"ແຕກໄຟລ໌ໄປທີ່…", L"ແຕກໄຟລ໌ທີ່ເລືອກ",
        L"ທີ່ເລືອກ", L"ຍົກເລີກ", L"ແຕກໄຟລ໌ທີ່ເລືອກບ່ອນນີ້", L"ແຕກໄຟລ໌ທີ່ເລືອກໄປທີ່…",
        L"ແຕກໄຟລ໌ບີບອັດໄປທີ່…"
    }, nullptr},
    {L"lb", false, {
        L"Numm", L"Geännert", L"Typ", L"Gréisst",
        L"Zeréck", L"Hei auspaken", L"Auspacken op…", L"Auswiel auspaken",
        L"Auswiel", L"Ofbriechen", L"Auswiel hei auspaken", L"Auswiel auspaken op…",
        L"Archiv auspaken op…"
    }, nullptr},
    {L"mk", false, {
        L"Име", L"Датум на промена", L"Тип", L"Големина",
        L"Назад", L"Отпакувај тука", L"Отпакувај во…", L"Отпакувај го избраното",
        L"Избрано", L"Откажи", L"Отпакувај го избраното тука", L"Отпакувај го избраното во…",
        L"Отпакувај ја архивата во…"
    }, nullptr},
    {L"ms", false, {
        L"Nama", L"Tarikh diubah", L"Jenis", L"Saiz",
        L"Kembali", L"Ekstrak di sini", L"Ekstrak ke…", L"Ekstrak pilihan",
        L"Pilihan", L"Batal", L"Ekstrak pilihan di sini", L"Ekstrak pilihan ke…",
        L"Ekstrak arkib ke…"
    }, nullptr},
    {L"ml", false, {
        L"പേര്", L"മാറ്റിയ തീയതി", L"തരം", L"വലുപ്പം",
        L"തിരികെ", L"ഇവിടെ പുറത്തെടുക്കുക", L"ഫോൾഡറിലേക്ക് പുറത്തെടുക്കുക…", L"തിരഞ്ഞെടുത്തവ പുറത്തെടുക്കുക",
        L"തിരഞ്ഞെടുത്തവ", L"റദ്ദാക്കുക", L"തിരഞ്ഞെടുത്തവ ഇവിടെ പുറത്തെടുക്കുക", L"തിരഞ്ഞെടുത്തവ ഫോൾഡറിലേക്ക് പുറത്തെടുക്കുക…",
        L"ആർക്കൈവ് ഫോൾഡറിലേക്ക് പുറത്തെടുക്കുക…"
    }, nullptr},
    {L"mt", false, {
        L"Isem", L"Data tal-bidla", L"Tip", L"Daqs",
        L"Lura", L"Oħroġ hawn", L"Oħroġ lejn…", L"Oħroġ l-għażla",
        L"Magħżula", L"Ikkanċella", L"Oħroġ l-għażla hawn", L"Oħroġ l-għażla lejn…",
        L"Oħroġ l-arkivju lejn…"
    }, nullptr},
    {L"mi", false, {
        L"Ingoa", L"Rā i panonihia", L"Momo", L"Rahi",
        L"Hoki", L"Tangohia ki konei", L"Tangohia ki…", L"Tangohia ngā mea kua tīpakohia",
        L"Kua tīpakohia", L"Whakakore", L"Tangohia ngā mea kua tīpakohia ki konei", L"Tangohia ngā mea kua tīpakohia ki…",
        L"Tangohia te pūranga ki…"
    }, nullptr},
    {L"mr", false, {
        L"नाव", L"बदलाची तारीख", L"प्रकार", L"आकार",
        L"मागे", L"येथे काढा", L"फोल्डरमध्ये काढा…", L"निवडलेले काढा",
        L"निवडलेले", L"रद्द करा", L"निवडलेले येथे काढा", L"निवडलेले फोल्डरमध्ये काढा…",
        L"संग्रह फोल्डरमध्ये काढा…"
    }, nullptr},
    {L"ne", false, {
        L"नाम", L"परिमार्जन मिति", L"प्रकार", L"आकार",
        L"पछाडि", L"यहाँ निकाल्नुहोस्", L"फोल्डरमा निकाल्नुहोस्…", L"छानिएका वस्तु निकाल्नुहोस्",
        L"छानिएका", L"रद्द गर्नुहोस्", L"छानिएका वस्तु यहाँ निकाल्नुहोस्", L"छानिएका वस्तु फोल्डरमा निकाल्नुहोस्…",
        L"सङ्ग्रह फोल्डरमा निकाल्नुहोस्…"
    }, nullptr},
    {L"nn", false, {
        L"Namn", L"Endra", L"Type", L"Storleik",
        L"Tilbake", L"Pakk ut her", L"Pakk ut til…", L"Pakk ut valde",
        L"Valde", L"Avbryt", L"Pakk ut valde her", L"Pakk ut valde til…",
        L"Pakk ut arkiv til…"
    }, nullptr},
    {L"or", false, {
        L"ନାମ", L"ପରିବର୍ତ୍ତନ ତାରିଖ", L"ପ୍ରକାର", L"ଆକାର",
        L"ପଛକୁ", L"ଏଠାରେ ବାହାର କରନ୍ତୁ", L"ଫୋଲ୍ଡରକୁ ବାହାର କରନ୍ତୁ…", L"ବଛାଯାଇଥିବା ଫାଇଲ ବାହାର କରନ୍ତୁ",
        L"ବଛାଯାଇଛି", L"ବାତିଲ କରନ୍ତୁ", L"ବଛାଯାଇଥିବା ଫାଇଲ ଏଠାରେ ବାହାର କରନ୍ତୁ", L"ବଛାଯାଇଥିବା ଫାଇଲ ଫୋଲ୍ଡରକୁ ବାହାର କରନ୍ତୁ…",
        L"ସଂକୁଚିତ ଫାଇଲ ଫୋଲ୍ଡରକୁ ବାହାର କରନ୍ତୁ…"
    }, nullptr},
    {L"fa", true, {
        L"نام", L"تاریخ تغییر", L"نوع", L"اندازه",
        L"بازگشت", L"استخراج در اینجا", L"استخراج در…", L"استخراج موارد انتخاب\u200Cشده",
        L"انتخاب\u200Cشده", L"لغو", L"استخراج موارد انتخاب\u200Cشده در اینجا", L"استخراج موارد انتخاب\u200Cشده در…",
        L"استخراج بایگانی در…"
    }, &kNotices_fa},
    {L"pa", false, {
        L"ਨਾਂ", L"ਸੋਧਣ ਦੀ ਮਿਤੀ", L"ਕਿਸਮ", L"ਆਕਾਰ",
        L"ਪਿੱਛੇ", L"ਇੱਥੇ ਕੱਢੋ", L"ਫੋਲਡਰ ਵਿੱਚ ਕੱਢੋ…", L"ਚੁਣੀਆਂ ਚੀਜ਼ਾਂ ਕੱਢੋ",
        L"ਚੁਣੀਆਂ", L"ਰੱਦ ਕਰੋ", L"ਚੁਣੀਆਂ ਚੀਜ਼ਾਂ ਇੱਥੇ ਕੱਢੋ", L"ਚੁਣੀਆਂ ਚੀਜ਼ਾਂ ਫੋਲਡਰ ਵਿੱਚ ਕੱਢੋ…",
        L"ਆਰਕਾਈਵ ਫੋਲਡਰ ਵਿੱਚ ਕੱਢੋ…"
    }, nullptr},
    {L"gd", false, {
        L"Ainm", L"Ceann-là an atharrachaidh", L"Seòrsa", L"Meud",
        L"Air ais", L"Dì-phacaich an-seo", L"Dì-phacaich gu…", L"Dì-phacaich na thagh thu",
        L"Taghta", L"Sguir dheth", L"Dì-phacaich na thagh thu an-seo", L"Dì-phacaich na thagh thu gu…",
        L"Dì-phacaich an tasglann gu…"
    }, nullptr},
    {L"ta", false, {
        L"பெயர்", L"மாற்றிய தேதி", L"வகை", L"அளவு",
        L"பின்னே", L"இங்கே பிரித்தெடு", L"கோப்புறைக்குப் பிரித்தெடு…", L"தேர்ந்தெடுத்தவற்றைப் பிரித்தெடு",
        L"தேர்ந்தெடுத்தவை", L"ரத்துசெய்", L"தேர்ந்தெடுத்தவற்றை இங்கே பிரித்தெடு", L"தேர்ந்தெடுத்தவற்றைக் கோப்புறைக்குப் பிரித்தெடு…",
        L"காப்பகத்தைக் கோப்புறைக்குப் பிரித்தெடு…"
    }, nullptr},
    {L"tt", false, {
        L"Исем", L"Үзгәртү көне", L"Төр", L"Зурлык",
        L"Артка", L"Монда чыгару", L"Папкага чыгару…", L"Сайланганнарны чыгару",
        L"Сайланганнар", L"Баш тарту", L"Сайланганнарны монда чыгару", L"Сайланганнарны папкага чыгару…",
        L"Архивны папкага чыгару…"
    }, nullptr},
    {L"te", false, {
        L"పేరు", L"మార్చిన తేదీ", L"రకం", L"పరిమాణం",
        L"వెనుకకు", L"ఇక్కడ వెలికితీయండి", L"ఫోల్డర్\u200Cకు వెలికితీయండి…", L"ఎంచుకున్న వాటిని వెలికితీయండి",
        L"ఎంచుకున్నవి", L"రద్దు చేయండి", L"ఎంచుకున్న వాటిని ఇక్కడ వెలికితీయండి", L"ఎంచుకున్న వాటిని ఫోల్డర్\u200Cకు వెలికితీయండి…",
        L"ఆర్కైవ్\u200Cను ఫోల్డర్\u200Cకు వెలికితీయండి…"
    }, nullptr},
    {L"ur", true, {
        L"نام", L"ترمیم کی تاریخ", L"قسم", L"حجم",
        L"واپس", L"یہاں نکالیں", L"فولڈر میں نکالیں…", L"منتخب اشیا نکالیں",
        L"منتخب", L"منسوخ کریں", L"منتخب اشیا یہاں نکالیں", L"منتخب اشیا فولڈر میں نکالیں…",
        L"آرکائیو فولڈر میں نکالیں…"
    }, nullptr},
    {L"ug", true, {
        L"ئاتى", L"ئۆزگەرتىلگەن چېسلا", L"تۈرى", L"چوڭلۇقى",
        L"قايتىش", L"بۇ يەرگە يېشىش", L"مۇنداق ئورۇنغا يېشىش…", L"تاللانغاننى يېشىش",
        L"تاللانغان", L"بىكار قىلىش", L"تاللانغاننى بۇ يەرگە يېشىش", L"تاللانغاننى قىسقۇچقا يېشىش…",
        L"ئارخىپنى قىسقۇچقا يېشىش…"
    }, nullptr},
    {L"uz", false, {
        L"Nomi", L"O‘zgartirilgan sana", L"Turi", L"Hajmi",
        L"Orqaga", L"Shu yerga chiqarish", L"Jildga chiqarish…", L"Tanlanganlarni chiqarish",
        L"Tanlanganlar", L"Bekor qilish", L"Tanlanganlarni shu yerga chiqarish", L"Tanlanganlarni jildga chiqarish…",
        L"Arxivni jildga chiqarish…"
    }, nullptr},
    {L"cy", false, {
        L"Enw", L"Dyddiad addasu", L"Math", L"Maint",
        L"Yn ôl", L"Echdynnu yma", L"Echdynnu i…", L"Echdynnu'r dewis",
        L"Dewis", L"Canslo", L"Echdynnu'r dewis yma", L"Echdynnu'r dewis i…",
        L"Echdynnu'r archif i…"
    }, nullptr},
    {L"quz", false, {
        L"Suti", L"Hukchasqa p'unchaw", L"Laya", L"Sayay",
        L"Kutiy", L"Kaypi hurquy", L"Mayman hurquyta akllay…", L"Akllasqata hurquy",
        L"Akllasqa", L"Sayachiy", L"Akllasqata kaypi hurquy", L"Akllasqata mayman hurquyta akllay…",
        L"Waqaychasqata mayman hurquyta akllay…"
    }, nullptr},
    {L"ku-Arab", true, {
        L"ناو", L"بەرواری دەستکاری", L"جۆر", L"قەبارە",
        L"گەڕانەوە", L"لێرە دەریبکە", L"دەریبکە بۆ…", L"هەڵبژێردراوەکان دەربکە",
        L"هەڵبژێردراوەکان", L"هەڵوەشاندنەوە", L"هەڵبژێردراوەکان لێرە دەربکە", L"هەڵبژێردراوەکان دەربکە بۆ…",
        L"ئەرشیفەکە دەربکە بۆ…"
    }, nullptr},
    {L"ha", false, {
        L"Suna", L"Ranar gyara", L"Nau’i", L"Girma",
        L"Koma", L"Cire a nan", L"Cire zuwa…", L"Cire waɗanda aka zaɓa",
        L"Zaɓaɓɓu", L"Soke", L"Cire waɗanda aka zaɓa a nan", L"Cire waɗanda aka zaɓa zuwa…",
        L"Cire fayilolin da aka matse zuwa…"
    }, nullptr},
    {L"ig", false, {
        L"Aha", L"Ụbọchị mgbanwe", L"Ụdị", L"Nha",
        L"Laghachi", L"Wepụta ebe a", L"Wepụta gaa na…", L"Wepụta ihe ahọpụtara",
        L"Ahọpụtara", L"Kagbuo", L"Wepụta ihe ahọpụtara ebe a", L"Wepụta ihe ahọpụtara gaa na…",
        L"Wepụta faịlụ ndị akpakọtara gaa na…"
    }, nullptr},
    {L"xh", false, {
        L"Igama", L"Umhla wokulungiswa", L"Uhlobo", L"Ubungakanani",
        L"Emva", L"Khupha apha", L"Khuphela kwi…", L"Khupha ezikhethiweyo",
        L"Ezikhethiweyo", L"Rhoxisa", L"Khupha ezikhethiweyo apha", L"Khuphela ezikhethiweyo kwi…",
        L"Khuphela ifayile ecinezelweyo kwi…"
    }, nullptr},
    {L"zu", false, {
        L"Igama", L"Idethi yokulungiswa", L"Uhlobo", L"Usayizi",
        L"Emuva", L"Khipha lapha", L"Khiphela ku…", L"Khipha okukhethiwe",
        L"Okukhethiwe", L"Khansela", L"Khipha okukhethiwe lapha", L"Khiphela okukhethiwe ku…",
        L"Khiphela ifayela elicindezelwe ku…"
    }, nullptr},
    {L"rw", false, {
        L"Izina", L"Itariki yahinduriweho", L"Ubwoko", L"Ingano",
        L"Subira inyuma", L"Kura dosiye hano", L"Kura dosiye ujye muri…", L"Kura dosiye zatoranyijwe",
        L"Izatoranyijwe", L"Hagarika", L"Kura dosiye zatoranyijwe hano", L"Kura dosiye zatoranyijwe ujye muri…",
        L"Kura dosiye muri arikive ujye muri…"
    }, nullptr},
    {L"sw", false, {
        L"Jina", L"Tarehe ya kubadilishwa", L"Aina", L"Ukubwa",
        L"Rudi", L"Toa hapa", L"Toa kwenye…", L"Toa zilizochaguliwa",
        L"Zilizochaguliwa", L"Ghairi", L"Toa zilizochaguliwa hapa", L"Toa zilizochaguliwa kwenye…",
        L"Toa faili za kumbukumbu kwenye…"
    }, nullptr},
    {L"ky", false, {
        L"Аты", L"Өзгөртүлгөн күнү", L"Түрү", L"Өлчөмү",
        L"Артка", L"Бул жерге чыгаруу", L"Папкага чыгаруу…", L"Тандалгандарды чыгаруу",
        L"Тандалгандар", L"Жокко чыгаруу", L"Тандалгандарды бул жерге чыгаруу", L"Тандалгандарды папкага чыгаруу…",
        L"Архивди папкага чыгаруу…"
    }, nullptr},
    {L"mn", false, {
        L"Нэр", L"Өөрчилсөн огноо", L"Төрөл", L"Хэмжээ",
        L"Буцах", L"Энд задлах", L"Хавтас руу задлах…", L"Сонгосныг задлах",
        L"Сонгосон", L"Цуцлах", L"Сонгосныг энд задлах", L"Сонгосныг хавтас руу задлах…",
        L"Архивыг хавтас руу задлах…"
    }, nullptr},
    {L"pa-Arab", true, {
        L"ناں", L"بدلن دی تاریخ", L"قسم", L"حجم",
        L"پچھے", L"ایتھے کڈھو", L"فولڈر وچ کڈھو…", L"چُنے ہوئے کڈھو",
        L"چُنے ہوئے", L"رد کرو", L"چُنے ہوئے ایتھے کڈھو", L"چُنے ہوئے فولڈر وچ کڈھو…",
        L"آرکائیو فولڈر وچ کڈھو…"
    }, nullptr},
    {L"nso", false, {
        L"Leina", L"Letšatši la phetogo", L"Mohuta", L"Bogolo",
        L"Morago", L"Ntšha mo", L"Ntšhetša go…", L"Ntšha tše di kgethilwego",
        L"Di kgethilwe", L"Khansela", L"Ntšha tše di kgethilwego mo", L"Ntšhetša tše di kgethilwego go…",
        L"Ntšhetša faele ye e gateletšwego go…"
    }, nullptr},
    {L"tn", false, {
        L"Leina", L"Letsatsi la phetogo", L"Mofuta", L"Bogolo",
        L"Morago", L"Ntsha fano", L"Ntshetsa kwa…", L"Ntsha tse di tlhophilweng",
        L"Tse di tlhophilweng", L"Khansela", L"Ntsha tse di tlhophilweng fano", L"Ntshetsa tse di tlhophilweng kwa…",
        L"Ntshetsa faele e e pitlagantsweng kwa…"
    }, nullptr},
    {L"sd-Arab", true, {
        L"نالو", L"تبديليءَ جي تاريخ", L"قسم", L"ماپ",
        L"واپس", L"هتي ڪڍو", L"فولڊر ۾ ڪڍو…", L"چونڊيل شيون ڪڍو",
        L"چونڊيل", L"رد ڪريو", L"چونڊيل شيون هتي ڪڍو", L"چونڊيل شيون فولڊر ۾ ڪڍو…",
        L"آرڪائيو فولڊر ۾ ڪڍو…"
    }, nullptr},
    {L"si", false, {
        L"නම", L"වෙනස් කළ දිනය", L"වර්ගය", L"ප්\u200Dරමාණය",
        L"ආපසු", L"මෙතැනට උපුටා ගන්න", L"ෆෝල්ඩරයකට උපුටා ගන්න…", L"තෝරාගත් දේ උපුටා ගන්න",
        L"තෝරාගත් දේ", L"අවලංගු කරන්න", L"තෝරාගත් දේ මෙතැනට උපුටා ගන්න", L"තෝරාගත් දේ ෆෝල්ඩරයකට උපුටා ගන්න…",
        L"සංරක්ෂිතය ෆෝල්ඩරයකට උපුටා ගන්න…"
    }, nullptr},
    {L"tg", false, {
        L"Ном", L"Санаи тағйир", L"Навъ", L"Андоза",
        L"Бозгашт", L"Ба ин ҷо баровардан", L"Ба ҷузвдон баровардан…", L"Интихобшударо баровардан",
        L"Интихобшуда", L"Бекор кардан", L"Интихобшударо ба ин ҷо баровардан", L"Интихобшударо ба ҷузвдон баровардан…",
        L"Бойгониро ба ҷузвдон баровардан…"
    }, nullptr},
    {L"ti", false, {
        L"ስም", L"ዝተቐየረሉ ዕለት", L"ዓይነት", L"መጠን",
        L"ተመለስ", L"ኣብዚ ኣውጽእ", L"ናብ… ኣውጽእ", L"ዝተመርጹ ኣውጽእ",
        L"ዝተመርጹ", L"ሰርዝ", L"ዝተመርጹ ኣብዚ ኣውጽእ", L"ዝተመርጹ ናብ… ኣውጽእ",
        L"ዝተጸቕጠ ፋይል ናብ… ኣውጽእ"
    }, nullptr},
    {L"tk", false, {
        L"Ady", L"Üýtgedilen senesi", L"Görnüşi", L"Ölçegi",
        L"Yza", L"Şu ýere çykar", L"Bukja çykar…", L"Saýlananlary çykar",
        L"Saýlananlar", L"Ýatyr", L"Saýlananlary şu ýere çykar", L"Saýlananlary bukja çykar…",
        L"Arhiwi bukja çykar…"
    }, nullptr},
    {L"wo", false, {
        L"Tur", L"Bés bu ñu ko soppee", L"Xeet", L"Dayo",
        L"Dellu", L"Génne fii", L"Génne ci…", L"Génne yi ñu tànn",
        L"Yi ñu tànn", L"Neenal", L"Génne yi ñu tànn fii", L"Génne yi ñu tànn ci…",
        L"Génne dencukaay bi ci…"
    }, nullptr},
    {L"yo", false, {
        L"Orúkọ", L"Ọjọ́ àtúnṣe", L"Irú", L"Ìwọ̀n",
        L"Padà", L"Yọ jáde síbí", L"Yọ jáde sí…", L"Yọ àwọn tí a yàn jáde",
        L"Tí a yàn", L"Fagilé", L"Yọ àwọn tí a yàn jáde síbí", L"Yọ àwọn tí a yàn jáde sí…",
        L"Yọ àkójọpọ̀ fáìlì jáde sí…"
    }, nullptr},
    {L"chr", false, {
        L"ᏚᏙᎥᎢ", L"ᎢᎦ ᎯᏁᏟᏴᎾ", L"ᎢᏳᏍᏗ", L"ᎢᎩᏓ",
        L"ᎠᏏᎾᎦᏘ", L"ᎠᎭᏂ ᏱᎬᏅᏕᏍᏗ", L"ᏱᎬᏅᏕᏍᏗ…", L"ᏗᏑᏰᏓ ᏱᎬᏅᏕᏍᏗ",
        L"ᏗᏑᏰᏓ", L"ᎠᏎᏒᏗ", L"ᏗᏑᏰᏓ ᎠᎭᏂ ᏱᎬᏅᏕᏍᏗ", L"ᏗᏑᏰᏓ ᏱᎬᏅᏕᏍᏗ…",
        L"ᏗᏴᏈᏛᎥᏍᎩ ᎭᏑᏯᎩ"
    }, nullptr},
    {L"iu-Latn", false, {
        L"Atiq", L"Ulluq asijjiqtuq", L"Qanuittuuninga", L"Anininga",
        L"Utimut", L"Maani angillitirli", L"Puurvimmut angillitirli…", L"Nirulussat angillitirli",
        L"Nirulussat", L"Qujanaarli", L"Nirulussat maani angillitirli", L"Nirulussat puurvimmut angillitirli…",
        L"Puurvik niruarli"
    }, nullptr},
    {L"quc", false, {
        L"Okb’i’aj", L"Uq’ijol uk’exanik", L"Ub’antajik", L"Unimal",
        L"Tzalijsab’al", L"Resaxik waral", L"Resaxik pa k’olkemchak…", L"Resaxik le cha’om",
        L"Cha’om", L"Uq’atexik", L"Resaxik le cha’om waral", L"Resaxik le cha’om pa k’olkemchak…",
        L"Ucha’ik k’olkemchak"
    }, nullptr},
    {L"prs", true, {
        L"نام", L"تاریخ تغییر", L"نوع", L"اندازه",
        L"برگشت", L"استخراج در اینجا", L"استخراج در فولدر…", L"استخراج موارد انتخاب\u200Cشده",
        L"انتخاب\u200Cشده", L"لغو", L"استخراج موارد انتخاب\u200Cشده در اینجا", L"استخراج موارد انتخاب\u200Cشده در فولدر…",
        L"استخراج آرشیف در فولدر…"
    }, nullptr},
};
static std::atomic<int> language{0}; // English fallback.

static bool SameTag(std::wstring_view tag, PCWSTR other) {
    const size_t length = wcslen(other);
    return tag.size() == length &&
        CompareStringOrdinal(tag.data(),static_cast<int>(tag.size()),other,
            static_cast<int>(length),TRUE) == CSTR_EQUAL;
}
static int FindCatalog(std::wstring_view tag) {
    for (int i = 0; i < static_cast<int>(_countof(kCatalogs)); ++i)
        if (SameTag(tag,kCatalogs[i].tag)) return i;
    return -1;
}
static int FindLanguage(std::wstring_view tag) {
    struct Alias { PCWSTR source; PCWSTR catalog; };
    static constexpr Alias aliases[] = {
        {L"zh-CN",L"zh-Hans"}, {L"zh-SG",L"zh-Hans"}, {L"zh-CHS",L"zh-Hans"},
        {L"zh-TW",L"zh-Hant"}, {L"zh-HK",L"zh-Hant"}, {L"zh-MO",L"zh-Hant"},
        {L"zh-CHT",L"zh-Hant"}, {L"zh",L"zh-Hans"},
        {L"pt",L"pt-BR"}, {L"sr",L"sr-Cyrl"}, {L"no",L"nb"},
        {L"iw",L"he"}, {L"in",L"id"}, {L"tl",L"fil"},
        {L"pa-PK",L"pa-Arab"}, {L"sd",L"sd-Arab"}, {L"ku-IQ",L"ku-Arab"},
        {L"ckb",L"ku-Arab"}, {L"fa-AF",L"prs"}, {L"qu",L"quz"},
        {L"iu",L"iu-Latn"}, {L"qut",L"quc"},
    };
    while (!tag.empty()) {
        const int found = FindCatalog(tag);
        if (found >= 0) return found;
        for (const auto & alias : aliases)
            if (SameTag(tag,alias.source)) return FindCatalog(alias.catalog);
        const size_t separator = tag.rfind(L'-');
        if (separator == std::wstring_view::npos) break;
        tag = tag.substr(0,separator);
    }
    return -1;
}
static int WindowsLanguage() {
    ULONG count = 0, chars = 0;
    if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME,&count,nullptr,&chars) && chars > 1) {
        std::vector<wchar_t> names(chars);
        if (GetUserPreferredUILanguages(MUI_LANGUAGE_NAME,&count,names.data(),&chars)) {
            // Win32 returns a double-NUL terminated list in preference order.
            for (PCWSTR tag = names.data(); *tag; tag += wcslen(tag) + 1) {
                const int match = FindLanguage(tag);
                if (match >= 0) return match;
            }
        }
    }
    // The UI language ID is a fallback when the preferred-language API fails.
    wchar_t name[LOCALE_NAME_MAX_LENGTH]{};
    const LANGID id = GetUserDefaultUILanguage();
    if (LCIDToLocaleName(MAKELCID(id,SORT_DEFAULT),name,_countof(name),0)) {
        const int match = FindLanguage(name);
        if (match >= 0) return match;
    }
    return 0;
}
static void LoadLanguage() {
    const auto setting = ArchivePreviewSettings::ReadText(L"Preview.Language");
    const int manual = FindLanguage(setting);
    language.store(manual >= 0 ? manual : WindowsLanguage(),std::memory_order_relaxed);
}
static int Language() {
    return language.load(std::memory_order_relaxed);
}
static PCWSTR LanguageTag() {
    return kCatalogs[Language()].tag;
}
static UINT TextDirection() {
    return kCatalogs[Language()].rightToLeft ? DT_RTLREADING : 0;
}
static UINT MenuDirection() {
    return kCatalogs[Language()].rightToLeft ? TPM_LAYOUTRTL : 0;
}
static PCWSTR Get(Id id) {
    const auto & catalog = kCatalogs[Language()];
    const size_t index = static_cast<size_t>(id);
    if (index < catalog.controls.size()) return catalog.controls[index];
    if (index < static_cast<size_t>(Id::Count))
        return (catalog.notices ? *catalog.notices : kNotices_en)[index - catalog.controls.size()];
    return L"";
}
}

// Own icon copies rather than retaining only indices into a borrowed Shell
// image list. Copies can survive parking, cache hits and worker COM teardown.
// Deduplicate within each model and bound the live copies in each process.
namespace ArchivePreviewIcons {
static std::atomic < unsigned > liveCopies{0};
static constexpr unsigned kMaxCopies = 512;
// The cap includes in-flight reservations. Count owned handles separately so
// a failed capture or a reservation alone cannot raise the measured peak.
static std::atomic<unsigned> ownedCopies{0};
static std::atomic<unsigned> peakCopies{0};
static std::atomic<uint64_t> rejectedAtCap{0};
static std::atomic<uint64_t> captureFailures{0};
struct Icon {
    HICON handle{};
    explicit Icon(HICON value):handle(value) {
        const unsigned owned = ownedCopies.fetch_add(1,std::memory_order_relaxed) + 1;
        if (!ArchivePreviewDiagnostics::Enabled()) return;
        unsigned peak = peakCopies.load(std::memory_order_relaxed);
        while (peak < owned &&
            !peakCopies.compare_exchange_weak(peak,owned,std::memory_order_relaxed)) {}
    }
    Icon(const Icon &) = delete;
    Icon & operator=(const Icon &) = delete;
    ~Icon() {
        if (handle) DestroyIcon(handle);
        ownedCopies.fetch_sub(1,std::memory_order_relaxed);
        liveCopies.fetch_sub(1,std::memory_order_relaxed);
    }
};
static std::shared_ptr < Icon > Capture(HIMAGELIST list,int index) {
    if (!list || index < 0) return {};
    if (liveCopies.fetch_add(1,std::memory_order_relaxed) >= kMaxCopies) {
        liveCopies.fetch_sub(1,std::memory_order_relaxed);
        if (ArchivePreviewDiagnostics::Enabled())
            rejectedAtCap.fetch_add(1,std::memory_order_relaxed);
        return {};
    }
    HICON handle = ImageList_GetIcon(list,index,ILD_TRANSPARENT);
    if (!handle) {
        liveCopies.fetch_sub(1,std::memory_order_relaxed);
        if (ArchivePreviewDiagnostics::Enabled())
            captureFailures.fetch_add(1,std::memory_order_relaxed);
        return {};
    }
    auto * image = new(std::nothrow) Icon(handle);
    if (!image) {
        DestroyIcon(handle);
        liveCopies.fetch_sub(1,std::memory_order_relaxed);
        if (ArchivePreviewDiagnostics::Enabled())
            captureFailures.fetch_add(1,std::memory_order_relaxed);
        return {};
    }
    return std::shared_ptr < Icon >(image);
}
static void LogStats(PCWSTR source) {
    if (!ArchivePreviewDiagnostics::Enabled()) return;
    const unsigned owned = ownedCopies.load(std::memory_order_relaxed);
    Wh_Log(L"[AP icons] source=%s pid=%lu live=%u peak=%u cap=%u rejected_at_cap=%llu capture_failures=%llu",
        source,GetCurrentProcessId(),owned,
        std::max(owned,peakCopies.load(std::memory_order_relaxed)),kMaxCopies,
        static_cast<unsigned long long>(rejectedAtCap.load(std::memory_order_relaxed)),
        static_cast<unsigned long long>(captureFailures.load(std::memory_order_relaxed)));
}
}

namespace ArchivePreviewColumns {
constexpr std::array<int,4> kMinimumDips{150,100,64,60};
constexpr std::array<int,4> kDefaultDips{150,150,120,94};
constexpr int kMaxWidthDips = 4096;
constexpr PCWSTR kWidthKeys[] = {L"NameWidth",L"DateWidth",L"TypeWidth",L"SizeWidth"};
constexpr PCWSTR kVisibilityKeys[] = {nullptr,L"DateLayoutVisibility",L"TypeLayoutVisibility",L"SizeLayoutVisibility"};
constexpr PCWSTR kHideSettings[] = {nullptr,L"DisplayColumns.HideDateModified",L"DisplayColumns.HideType",L"DisplayColumns.HideSize"};

struct Change {
    DWORD column{};
    int value{};
    DWORD visibility{};
};
static bool Valid(const Change & change) {
    return change.column < kMinimumDips.size() &&
        (change.visibility == 1 ? change.column != 0 && (change.value == 0 || change.value == 1) :
         change.visibility == 0 && change.value >= kMinimumDips[change.column] && change.value <= kMaxWidthDips);
}

static std::mutex preferencesMutex;
static std::array<int,4> preferredDips{};
// Keep local changes usable if both the storage API and the broker are unavailable.
static std::array<bool,4> unsavedWidth{};
static std::array<bool,4> unsavedVisibility{};
struct ReloadResult { bool widths{}; bool visibility{}; };

static ReloadResult Load(bool settingsChanged = false) {
    std::lock_guard<std::mutex> lock(preferencesMutex);
    ReloadResult result;
    unsigned visible = 1;
    const unsigned current = ArchivePreviewOptions::visibleColumns.load();
    for (unsigned i = 0; i < preferredDips.size(); ++i) {
        if (!unsavedWidth[i]) {
            const int saved = Wh_GetIntValue(kWidthKeys[i],0);
            const int width = Valid({i,saved,0}) ? saved : 0;
            result.widths |= std::exchange(preferredDips[i],width) != width;
        }
        if (i == 0) continue;
        const unsigned bit = 1u << i;
        if (unsavedVisibility[i] && !settingsChanged) {
            visible |= current & bit;
            continue;
        }
        const bool defaultVisible = Wh_GetIntSetting(kHideSettings[i]) == 0;
        const int saved = Wh_GetIntValue(kVisibilityKeys[i],0);
        // A header override is relative to the Windhawk setting at the time.
        // Changing that setting takes precedence over the previous override.
        const int overrideForDefault = defaultVisible ? 2 : 1;
        if (settingsChanged && saved && saved != overrideForDefault)
            Wh_DeleteValue(kVisibilityKeys[i]);
        unsavedVisibility[i] = false;
        if (saved == overrideForDefault ? !defaultVisible : defaultVisible) visible |= bit;
    }
    result.visibility = ArchivePreviewOptions::visibleColumns.exchange(visible) != visible;
    return result;
}

static void ApplyLocked(const Change & change, bool saved) {
    const auto i = change.column;
    if (change.visibility) {
        const unsigned bit = 1u << i;
        if (change.value) ArchivePreviewOptions::visibleColumns.fetch_or(bit);
        else ArchivePreviewOptions::visibleColumns.fetch_and(~bit);
        unsavedVisibility[i] = !saved;
    } else {
        preferredDips[i] = change.value;
        unsavedWidth[i] = !saved;
    }
}

static bool Save(const Change & change) {
    if (!Valid(change)) return false;
    std::lock_guard<std::mutex> lock(preferencesMutex);
    const auto i = change.column;
    int value = change.value;
    PCWSTR key = kWidthKeys[i];
    if (change.visibility) {
        const bool defaultVisible = Wh_GetIntSetting(kHideSettings[i]) == 0;
        key = kVisibilityKeys[i];
        value = value == defaultVisible ? 0 : (value ? 1 : 2);
    }
    if (!Wh_SetIntValue(key,value)) return false;
    ApplyLocked(change,true);
    return true;
}

static void UseForSession(const Change & change) {
    if (!Valid(change)) return;
    std::lock_guard<std::mutex> lock(preferencesMutex);
    ApplyLocked(change,false);
    Wh_Log(L"Archive Preview could not save the column preference; using it for this session");
}

static bool VisibilityMenu(HWND owner, POINT point, Change & change) {
    if (point.x == -1 && point.y == -1) {
        RECT bounds{};
        GetWindowRect(owner,&bounds);
        point = {bounds.left + 8,bounds.bottom};
    }
    HMENU menu = CreatePopupMenu();
    if (!menu) return false;
    using Id = ArchivePreviewText::Id;
    const Id labels[] = {Id::ColumnDate,Id::ColumnType,Id::ColumnSize};
    for (unsigned i = 1; i <= 3; ++i)
        AppendMenuW(menu,MF_STRING | (ArchivePreviewOptions::ShowColumn(i) ? MF_CHECKED : MF_UNCHECKED),
            i,ArchivePreviewText::Get(labels[i - 1]));
    const UINT command = TrackPopupMenuEx(menu,TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON | ArchivePreviewText::MenuDirection(),
        point.x,point.y,owner,nullptr);
    DestroyMenu(menu);
    if (command < 1 || command > 3) return false;
    change = {command,!ArchivePreviewOptions::ShowColumn(command),1};
    return true;
}

static int Px(UINT dpi, int dips) {
    return MulDiv(dips,dpi ? dpi : 96,96);
}
static int ViewportWidth(HWND list) {
    RECT client{};
    GetClientRect(list,&client);
    return std::max(0,static_cast<int>(client.right - client.left));
}
static int HorizontalOffset(HWND list) {
    SCROLLINFO scroll{sizeof(scroll),SIF_POS};
    return GetScrollInfo(list,SB_HORZ,&scroll) ? scroll.nPos : 0;
}
static void HideScrollbar(HWND list) {
    if (GetWindowLongPtrW(list,GWL_STYLE) & WS_HSCROLL)
        ShowScrollBar(list,SB_HORZ,FALSE);
}
static int DragWidth(HWND list, UINT dpi, unsigned kind, int width) {
    const int minimum = Px(dpi,kMinimumDips[kind]);
    // A compressed column may already be narrower than its drag minimum.
    // Wait until the pane can fit the minimum before changing that width.
    const int maximum = ArchivePreviewOptions::hideHorizontalScrollbar.load() ?
        std::min(ViewportWidth(list),Px(dpi,kMaxWidthDips)) : Px(dpi,kMaxWidthDips);
    return maximum < minimum ? -1 : std::clamp(width,minimum,maximum);
}

static std::array<int,4> Fit(int available, const std::array<int,4> & preferred,
                             const std::array<int,4> & minimum, bool hideScroll, bool autoName) {
    std::array<int,4> widths{};
    widths[0] = std::min(preferred[0],available);
    if (!hideScroll) widths[0] = std::max(widths[0],minimum[0]);
    int remaining = available - widths[0];
    int expandable = 0;
    bool collapsed = false;
    for (size_t i = 1; i < widths.size(); ++i) {
        if (minimum[i] == 0) continue;
        if (hideScroll && minimum[i] > remaining) {
            collapsed = true;
            continue;
        }
        widths[i] = minimum[i];
        remaining -= widths[i];
        expandable += preferred[i] - minimum[i];
    }
    int growth = std::clamp(remaining,0,expandable);
    remaining -= growth;
    for (size_t i = 1; i < widths.size() && expandable > 0; ++i) {
        if (!widths[i]) continue;
        const int extra = preferred[i] - minimum[i];
        const int add = growth * extra / expandable;
        widths[i] += add;
        growth -= add;
        expandable -= extra;
    }
    if ((autoName || collapsed) && remaining > 0) widths[0] += remaining;
    return widths;
}

template<typename Kind>
static void Layout(HWND list, UINT dpi, const std::vector<Kind> & columns) {
    static thread_local bool layingOut = false;
    if (layingOut || columns.empty()) return;
    layingOut = true;
    std::array<int,4> saved;
    {
        std::lock_guard<std::mutex> lock(preferencesMutex);
        saved = preferredDips;
    }
    std::array<int,4> preferred{}, minimum{};
    for (const auto kind : columns) {
        const auto i = static_cast<unsigned>(kind);
        minimum[i] = Px(dpi,kMinimumDips[i]);
        preferred[i] = Px(dpi,saved[i] ? saved[i] : kDefaultDips[i]);
    }
    const bool hideScroll = ArchivePreviewOptions::hideHorizontalScrollbar.load();
    if (hideScroll) {
        const int offset = HorizontalOffset(list);
        if (offset) ListView_Scroll(list,-offset,0);
    }
    // A horizontal bar can change whether a vertical bar is needed.
    for (int pass = 0; pass < 2; ++pass) {
        const int available = ViewportWidth(list);
        const auto widths = Fit(available,preferred,minimum,hideScroll,!saved[0]);
        // Shrink existing columns before expanding any to avoid a transient bar.
        for (const bool grow : {false,true}) {
            for (int i = 0; i < static_cast<int>(columns.size()); ++i) {
                const int width = widths[static_cast<unsigned>(columns[i])];
                const int current = ListView_GetColumnWidth(list,i);
                if (width != current && (width > current) == grow)
                    ListView_SetColumnWidth(list,i,width);
            }
        }
        if (hideScroll) HideScrollbar(list);
        if (ViewportWidth(list) == available) break;
    }
    layingOut = false;
}
}

namespace ArchivePreviewSort {
struct Values {
    bool folder{};
    const std::wstring & name;
    const std::wstring & type;
    ULONGLONG bytes{};
    bool hasSize{};
    ULONGLONG modified{};
};
static int Text(const std::wstring & a,const std::wstring & b) {
    const int result = CompareStringOrdinal(a.c_str(),-1,b.c_str(),-1,TRUE);
    return result == CSTR_LESS_THAN?-1:(result == CSTR_GREATER_THAN?1:0);
}
template<typename T,typename GetValues>
static void Rows(std::vector<T> & rows,int column,bool ascending,GetValues values) {
    std::stable_sort(rows.begin(),rows.end(),[&](const T & a,const T & b) {
        const auto x = values(a),y = values(b);
        if (x.folder != y.folder) return x.folder; // folders stay first in both directions
        int compared = 0;
        if (column == 1) {
            if ((x.modified != 0) != (y.modified != 0)) return x.modified != 0;
            compared = x.modified < y.modified?-1:(x.modified > y.modified?1:0);
        } else if (column == 2) compared = Text(x.type,y.type);
        else if (column == 3) {
            if (x.hasSize != y.hasSize) return x.hasSize;
            compared = x.bytes < y.bytes?-1:(x.bytes > y.bytes?1:0);
        } else compared = Text(x.name,y.name);
        if (compared) return ascending?compared < 0:compared > 0;
        return column != 0 && Text(x.name,y.name) < 0;
    });
}
static ULONGLONG FileTimeValue(const FILETIME & time) {
    return (static_cast<ULONGLONG>(time.dwHighDateTime) << 32) | time.dwLowDateTime;
}
}

// Strict path validation for sensitive local paths and optional URLMon
// handling. Reject redirecting reparse points rather than trusting drive letters.
// A same-user adversary may still race nontransactional Win32 path operations.
namespace ArchivePreviewPathSafety {
    static bool PlainDrivePath(PCWSTR value) {
        if (!value || !((value[0] >= L'A' && value[0] <= L'Z') ||
                         (value[0] >= L'a' && value[0] <= L'z')) ||
            value[1] != L':' || value[2] != L'\\') return false;
        if (!value[3]) return true;
        for (const wchar_t * ch = value + 2; * ch;++ch)
            if ( * ch == L':' || * ch == L'/') return false;
        return true;
    }
    // Call only for absolute local drive paths whose components are validated.
    static std::wstring ExtendedLocalPath(const std::wstring & path) {
        return L"\\\\?\\"+path;
    }
    static HRESULT CheckPathComponents(const std::filesystem::path & path,
                                       bool lastMustBeDirectory,
                                       bool lockAncestors,
                                       std::vector < HANDLE >  * retained,
                                       bool extendedLength) {
        const std::wstring value = path.wstring();
        if (!PlainDrivePath(value.c_str()) || !path.is_absolute()) return E_ACCESSDENIED;
        const wchar_t root[4] = {value[0],L':',L'\\',L'\0'};
        if (GetDriveTypeW(root) != DRIVE_FIXED) return E_ACCESSDENIED;
        std::filesystem::path cursor = path.root_path();
        auto parts = path.relative_path();
        if (parts.empty()) return E_ACCESSDENIED;
        const size_t count = static_cast < size_t > (std::distance(parts.begin(),parts.end()));
        size_t index = 0;
        for (const auto & component:parts) {
            const std::wstring name = component.wstring();
            if (name.empty() || name == L"." || name == L"..") return E_ACCESSDENIED;
            cursor /= component;
            const bool isLast = (++index == count);
            const std::wstring extended = extendedLength ? ExtendedLocalPath(cursor.native()) :
                std::wstring{};
            const PCWSTR openPath = extendedLength ? extended.c_str() : cursor.c_str();
            HANDLE handle = CreateFileW(openPath,FILE_READ_ATTRIBUTES,
                FILE_SHARE_READ | FILE_SHARE_WRITE | (lockAncestors?0:FILE_SHARE_DELETE),
                nullptr,OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
            if (handle == INVALID_HANDLE_VALUE) return HRESULT_FROM_WIN32(GetLastError());
            FILE_ATTRIBUTE_TAG_INFO info{};
            HRESULT result = E_ACCESSDENIED;
            if (!GetFileInformationByHandleEx(handle,FileAttributeTagInfo,&info,sizeof(info)))
                result = HRESULT_FROM_WIN32(GetLastError());
            else if (!(info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                ((!isLast || lastMustBeDirectory)
                    ? (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
                    : (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
                result = S_OK;
            if (FAILED(result)) {
                CloseHandle(handle);
                return result;
            }
            if (lockAncestors && retained) retained->push_back(handle);
            else CloseHandle(handle);
        }
        return S_OK;
    }
    static bool ValidPathComponents(const std::filesystem::path & path,
                                    bool lastMustBeDirectory,
                                    bool lockAncestors,
                                    std::vector < HANDLE >  * retained = nullptr) {
        return SUCCEEDED(CheckPathComponents(path,lastMustBeDirectory,lockAncestors,retained,false));
    }
}

// Optional, per-process exception to URLACTION_SHELL_PREVIEW for this mod's
// local archive types. Do NOT edit Zone.Identifier or system-wide zone policies.
// This targets URLMon's standard security-manager implementation. Explorer can
// choose another security-check route, so this option is best-effort.
namespace ArchivePreviewSecurityPolicy {
    using ProcessUrlActionFn = HRESULT (STDMETHODCALLTYPE * )(
        IInternetSecurityManager * , LPCWSTR, DWORD, BYTE * , DWORD,
        BYTE * , DWORD, DWORD, DWORD);
    static ProcessUrlActionFn originalProcessUrlAction = nullptr;
    static thread_local bool insideSecurityProbe = false;

    static bool IsEligibleLocalArchive(LPCWSTR url) {
        if (!url) return false;
        // Accept only local file URLs or an absolute drive path. Reject network
        // shares, web URLs, relative paths and any unsupported extension.
        std::unique_ptr<WCHAR[]> decoded;
        const wchar_t * path = url;
        if (_wcsnicmp(url,L"file:",5) == 0) {
            // Allocate without throwing across the Explorer hook boundary.
            // URLMon may return long paths, so don't reserve 64 KB on the stack.
            decoded.reset(new(std::nothrow) WCHAR[32768]{});
            if (!decoded) return false;
            DWORD chars = 32768;
            if (FAILED(PathCreateFromUrlW(url,decoded.get(),&chars,0))) return false;
            path = decoded.get();
        }
        if (wcslen(path) < 4) return false;
        if (!((path[0] >= L'A' && path[0] <= L'Z') ||
              (path[0] >= L'a' && path[0] <= L'z')) ||
            path[1] != L':' || (path[2] != L'\\' && path[2] != L'/'))
            return false;
        if (!ArchivePreviewOptions::MatchArchive(path) ||
            !ArchivePreviewPathSafety::PlainDrivePath(path)) return false;
        // Strictly reject reparse ancestors and alternate-data-stream paths.
        // Opening the final file also blocks an obvious path-to-network redirect.
        return ArchivePreviewPathSafety::ValidPathComponents(std::filesystem::path(path),
                                                  false,false);
    }

    static HRESULT STDMETHODCALLTYPE ProcessUrlActionHook(
        IInternetSecurityManager * manager, LPCWSTR url, DWORD action,
        BYTE * policy, DWORD policyBytes, BYTE * context, DWORD contextBytes,
        DWORD flags, DWORD reserved) {
        const auto original = originalProcessUrlAction;
        if (!original) return E_FAIL;
        if (action == URLACTION_SHELL_PREVIEW && manager && url && policy &&
            policyBytes >= sizeof(DWORD) && !insideSecurityProbe &&
            ArchivePreviewOptions::autoAllowInternetArchivePreview.load(
                std::memory_order_relaxed) && IsEligibleLocalArchive(url)) {
            insideSecurityProbe = true;
            DWORD zone = URLZONE_INVALID;
            // A Restricted Sites policy (zone 4), managed zone or an unknown
            // security classification must NEVER be overridden here.
            const bool internet = SUCCEEDED(manager->MapUrlToZone(url, & zone,0)) &&
                zone == URLZONE_INTERNET;
            insideSecurityProbe = false;
            if (internet) {
                const DWORD allow = URLPOLICY_ALLOW;
                std::memcpy(policy, & allow,sizeof(allow));
                return S_OK;
            }
        }
        return original(manager,url,action,policy,policyBytes,context,
                        contextBytes,flags,reserved);
    }

    static bool InstallHook() {
        // Installing the hook even when disabled supports changing the toggle
        // without repeatedly patching/unpatching code in Explorer threads.
        const HRESULT coinit = CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
        if (FAILED(coinit) && coinit != RPC_E_CHANGED_MODE) {
            Wh_Log(L"ArchivePreview security policy: COM unavailable (0x%08lX)",
                   static_cast < unsigned long > (coinit));
            return false;
        }
        IInternetSecurityManager * manager = nullptr;
        const HRESULT hr = CoInternetCreateSecurityManager(nullptr, & manager,0);
        if (FAILED(hr) || !manager) {
            Wh_Log(L"ArchivePreview security policy: could not create URLMon security manager (0x%08lX)",
                   static_cast < unsigned long > (hr));
            if (SUCCEEDED(coinit)) CoUninitialize();
            return false;
        }
        // IUnknown = entries 0-2; Set/GetSecuritySite=3/4;
        // MapUrlToZone=5; GetSecurityId=6; ProcessUrlAction=7.
        auto *  * functions =  * reinterpret_cast < void *  *  *  > (manager);
        void * target = functions ? functions[7] : nullptr;
        const bool installed = target && Wh_SetFunctionHook(
            target,reinterpret_cast < void *  > (ProcessUrlActionHook),
            reinterpret_cast < void *  *  > ( & originalProcessUrlAction));
        manager->Release();
        if (SUCCEEDED(coinit)) CoUninitialize();
        if (installed)
            Wh_Log(L"ArchivePreview security policy: scoped URLMon hook registered; disabled by default");
        else
            Wh_Log(L"ArchivePreview security policy: URLMon hook not available; Windows default stays active");
        return installed;
    }
} // namespace ArchivePreviewSecurityPolicy

// Wire protocol for the per-user transfer broker. No arbitrary executables/commands.
static constexpr DWORD ARCHIVE_BROKER_MAGIC = 0x42505041; // APPB
static constexpr DWORD ARCHIVE_BROKER_EXTRACT_HERE = 1;
static constexpr DWORD ARCHIVE_BROKER_EXTRACT_TO = 2;
static constexpr DWORD ARCHIVE_BROKER_COPY_SELECTION = 6;
static constexpr DWORD ARCHIVE_BROKER_EXTRACT_SELECTED_TO = 7;
static constexpr DWORD ARCHIVE_BROKER_GRIP_UPDATE = 8; // attach broker-owned ListView to low-IL host
static constexpr DWORD ARCHIVE_BROKER_GRIP_CLEAR = 9; // detach broker-owned ListView
// The medium-integrity broker owns the actual interactive ListView.
// Only a HWND token, command ID and navigation depth cross the IPC/window boundary.
// Never pass pointers through cross-process window messages.
static constexpr DWORD ARCHIVE_BROKER_BROWSER_BACK = 10;
static constexpr DWORD ARCHIVE_BROKER_PING = 11; // actual authenticated IPC health check
static constexpr DWORD ARCHIVE_BROKER_QUERY_STATUS = 12; // request-ID completion query
static constexpr DWORD ARCHIVE_BROKER_OPEN_FILE = 13; // explicit file open, never auto-execute
static constexpr DWORD ARCHIVE_BROKER_CANCEL_OPERATION = 14; // authenticated request-scoped cancel
static constexpr DWORD ARCHIVE_BROKER_EXTRACT_SELECTED_HERE = 15;
static constexpr DWORD ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO = 16;
static constexpr DWORD ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE = 17;
// R5: Read-only request to get the absolute path of the one file selected in
// the matching Explorer window. Some Windows stream previewers pass only a
// basename through STATSTG.pwcsName, never a filesystem path.
static constexpr DWORD ARCHIVE_BROKER_RESOLVE_SELECTED_PATH = 18;
static constexpr DWORD ARCHIVE_BROKER_SAVE_COLUMN = 19;
// Shared popup IDs are local HWND menu IDs, never sent over the named pipe.
static constexpr UINT kSelectedMenuHere = 1101;
static constexpr UINT kSelectedMenuTo = 1102;
static constexpr DWORD ARCHIVE_OPERATION_UNKNOWN = 0;
static constexpr DWORD ARCHIVE_OPERATION_QUEUED = 1;
static constexpr DWORD ARCHIVE_OPERATION_RUNNING = 2;
static constexpr DWORD ARCHIVE_OPERATION_DONE = 3;
// Progress is tracked by request ID. The Shell can report work units,
// but it does not guarantee byte-level progress or immediate cancellation.
static constexpr DWORD ARCHIVE_PROGRESS_NONE = 0;
static constexpr DWORD ARCHIVE_PROGRESS_SCANNING = 1;
static constexpr DWORD ARCHIVE_PROGRESS_EXTRACTING = 2;
static constexpr DWORD ARCHIVE_PROGRESS_PICKING = 4;
static constexpr DWORD ARCHIVE_PROGRESS_STARTING = 5; // native Shell operation accepted, percent not yet available
static constexpr DWORD ARCHIVE_PROGRESS_EXTERNAL = 6; // progress/cancellation belong to the external application's dialog
// An explicit client acknowledgement keeps server responses alive until consumed.
static constexpr DWORD ARCHIVE_BROKER_REPLY_ACK = 0x4B434150; // PACK
static constexpr UINT ARCHIVE_BROWSER_READY = WM_APP + 194;
static constexpr UINT ARCHIVE_BROWSER_DEPTH = WM_APP + 195;
static constexpr UINT ARCHIVE_BROWSER_FAILED = WM_APP + 196;
// No cross-process pointer data: broker asks the Preview Host to refresh its
// toolbar after OLE drag finishes and Explorer may have repainted the parent.
static constexpr UINT ARCHIVE_BROWSER_DRAG_FINISHED = WM_APP + 197;
// Explicit repaint after manual extraction; unlike drag, no delayed second pass.
static constexpr UINT ARCHIVE_BROWSER_CHROME_REFRESH = WM_APP + 204;
// A medium-integrity observer confirmed that Explorer selected another item.
// Only a notification; the preview owner is responsible for hiding its HWND.
static constexpr UINT ARCHIVE_BROWSER_SELECTION_LOST = WM_APP + 198;
// Broker-owned Cancel button sends only a scalar request fingerprint back to
// the low-integrity preview. No cross-process pointer or callback is used.
static constexpr UINT ARCHIVE_BROWSER_CANCEL_FROM_FOOTER = WM_APP + 199;
// Broker sends only a scalar selection count and matching anchor HWND.
static constexpr UINT ARCHIVE_BROWSER_SELECTED_COUNT = WM_APP + 200;
// Whole-archive hotkeys use the same Preview Host actions as the toolbar.
static constexpr UINT ARCHIVE_BROWSER_EXTRACT_ARCHIVE = WM_APP + 209;
static WPARAM ArchiveRequestUiToken(const GUID & id) {
    const auto * data = reinterpret_cast < const BYTE *  > ( & id);
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0;i < sizeof(GUID);++i) {
        hash ^= data[i];hash *= 1099511628211ULL;
    }
    return static_cast < WPARAM > (hash);
}
// Low-integrity list input messages support the fallback preview.
static constexpr UINT ARCHIVE_PROXY_CLICK = WM_APP + 190;
static constexpr UINT ARCHIVE_PROXY_DOUBLECLICK = WM_APP + 191;
static constexpr UINT ARCHIVE_PROXY_WHEEL = WM_APP + 192;
static constexpr UINT ARCHIVE_PROXY_RIGHTCLICK = WM_APP + 193;
// Consistent selection cap for native Shell operations and IPC.
// Never accept an unbounded payload from a named pipe.
static constexpr DWORD ARCHIVE_BROKER_MAX_PATHS = 4096;
static constexpr ULONGLONG ARCHIVE_BROKER_MAX_WIRE_BYTES = 64ULL * 1024 * 1024;
static constexpr DWORD ARCHIVE_BROKER_MAX_CHARS = 32700;

struct ArchiveBrokerWireHeader {
    DWORD magic;
    DWORD command;
    DWORD archiveChars;
    DWORD itemCount;
    GUID requestId{};  // generated once; retries must reuse this identifier
};
struct ArchiveBrokerColumnChange {
    uint64_t previewWindow{};
    ArchivePreviewColumns::Change change{};
    DWORD reserved{};
};
static_assert(sizeof(ArchiveBrokerColumnChange) == 24);
struct ArchiveBrokerOperationStatusReply {
    HRESULT lookup;
    HRESULT result;
    DWORD phase;
    DWORD progressStage;
    DWORD progressPercent; // Windows Shell IFileOperation work-unit estimate
    DWORD progressKnown;   // 0 = indeterminate
    ULONGLONG reservedBytesDone;  // wire compatibility; always zero in native mode
    ULONGLONG reservedBytesTotal; // wire compatibility; always zero in native mode
};

inline HRESULT ArchiveGetUserSid(std::wstring & text) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, & token)) return HRESULT_FROM_WIN32(GetLastError());
    DWORD cb = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, & cb);
    if (cb == 0 || cb > 16384) { CloseHandle(token); return E_FAIL; }
    std::vector < BYTE > buffer(cb);
    BOOL ok = GetTokenInformation(token, TokenUser, buffer.data(), cb, & cb);
    CloseHandle(token);
    if (!ok) return HRESULT_FROM_WIN32(GetLastError());
    LPWSTR sidText = nullptr;
    if (!ConvertSidToStringSidW(reinterpret_cast < TOKEN_USER *  > (buffer.data())->User.Sid, & sidText))
        return HRESULT_FROM_WIN32(GetLastError());
    text.assign(sidText);
    LocalFree(sidText);
    return S_OK;
}
inline HRESULT ArchiveBrokerPipeName(std::wstring & name) {
    std::wstring sid;
    HRESULT hr = ArchiveGetUserSid(sid);
    if (FAILED(hr)) return hr;
    name = L"\\\\.\\pipe\\ArchivePreview.Broker.v804." + sid;
    return S_OK;
}
inline HRESULT ArchiveWriteAll(HANDLE h, const void * bytes, DWORD size) {
    const BYTE * p = static_cast < const BYTE *  > (bytes);
    while (size) {
        DWORD wrote = 0;
        if (!WriteFile(h,p,size, & wrote,nullptr))
            return HRESULT_FROM_WIN32(GetLastError());
        if (wrote == 0) return HRESULT_FROM_WIN32(ERROR_BROKEN_PIPE);
        p += wrote;
        size -= wrote;
    }
    return S_OK;
}
// Read a byte-stream response without waiting forever if a broker worker is
// stalled. Never call blocking ReadFile unless PeekNamedPipe reports bytes.
// A timeout closes only this client's connection, not the running broker.
inline HRESULT ArchiveReadAllWithDeadline(HANDLE pipe, void * output, DWORD count,
                                           ULONGLONG deadline) {
    BYTE * dst = static_cast < BYTE *  > (output);
    while (count) {
        DWORD available = 0;
        if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, & available, nullptr))
            return HRESULT_FROM_WIN32(GetLastError());
        if (!available) {
            if (GetTickCount64() >= deadline) return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
            Sleep(10);
            continue;
        }
        DWORD received = 0;
        if (!ReadFile(pipe, dst, std::min(count, available), & received, nullptr) || !received)
            return HRESULT_FROM_WIN32(GetLastError() ? GetLastError() : ERROR_BROKEN_PIPE);
        count -= received;
        dst += received;
    }
    return S_OK;
}
// Preview Host can read Windhawk settings but a low-integrity token may be
// unable to write them. Send only a bounded column change to the same broker.
static void ArchiveSaveFallbackColumn(HWND preview, const ArchivePreviewColumns::Change & change) {
    using namespace ArchivePreviewColumns;
    if (!Valid(change) || Save(change)) return;
    std::wstring pipeName;
    if (SUCCEEDED(ArchiveBrokerPipeName(pipeName))) {
        HANDLE pipe = CreateFileW(pipeName.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (pipe != INVALID_HANDLE_VALUE) {
            const ArchiveBrokerWireHeader header{ARCHIVE_BROKER_MAGIC,ARCHIVE_BROKER_SAVE_COLUMN,0,0,{}};
            const ArchiveBrokerColumnChange request{reinterpret_cast<UINT_PTR>(preview),change};
            HRESULT result = E_FAIL;
            const bool received = SUCCEEDED(ArchiveWriteAll(pipe,&header,sizeof(header))) &&
                SUCCEEDED(ArchiveWriteAll(pipe,&request,sizeof(request))) &&
                SUCCEEDED(ArchiveReadAllWithDeadline(pipe,&result,sizeof(result),GetTickCount64() + 300));
            if (received) {
                const DWORD ack = ARCHIVE_BROKER_REPLY_ACK;
                ArchiveWriteAll(pipe,&ack,sizeof(ack));
            }
            CloseHandle(pipe);
            if (received && SUCCEEDED(result)) {
                std::lock_guard<std::mutex> lock(preferencesMutex);
                ApplyLocked(change,true);
                return;
            }
        }
    }
    UseForSession(change);
}

inline HRESULT ArchiveSendBrokerCommand(DWORD command, const std::wstring & archive,
                                        const std::vector < std::wstring >  & paths,
                                        GUID * acceptedRequestId = nullptr) {
    if ((command != ARCHIVE_BROKER_EXTRACT_HERE && command != ARCHIVE_BROKER_EXTRACT_TO &&
         command != ARCHIVE_BROKER_COPY_SELECTION &&
         command != ARCHIVE_BROKER_EXTRACT_SELECTED_TO &&
         command != ARCHIVE_BROKER_EXTRACT_SELECTED_HERE &&
         command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO &&
         command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE &&
         command != ARCHIVE_BROKER_OPEN_FILE &&
         command != ARCHIVE_BROKER_GRIP_UPDATE && command != ARCHIVE_BROKER_GRIP_CLEAR &&
         command != ARCHIVE_BROKER_BROWSER_BACK) ||
        archive.empty() || archive.size() > ARCHIVE_BROKER_MAX_CHARS ||
        paths.size() > ARCHIVE_BROKER_MAX_PATHS) return E_INVALIDARG;
    for (const auto & p : paths)
        if (p.empty() || p.size() > 2048 || p.find(L'\0') != std::wstring::npos) return E_INVALIDARG;
    ULONGLONG wireBytes = static_cast < ULONGLONG > (archive.size()) * sizeof(wchar_t);
    for (const auto & p:paths) {
        wireBytes += sizeof(DWORD) + static_cast < ULONGLONG > (p.size()) * sizeof(wchar_t);
        if (wireBytes > ARCHIVE_BROKER_MAX_WIRE_BYTES)
            return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }
    std::wstring name;
    HRESULT hr = ArchiveBrokerPipeName(name);
    if (FAILED(hr)) return hr;
    // All retries for one user gesture reuse the same GUID. The server keeps
    // accepted/completed request IDs for deduplication after a lost reply.
    GUID id{};
    hr = CoCreateGuid( & id);
    if (FAILED(hr)) return hr;
    // The broker must parse thousands of paths before acknowledging a large
    // selection. This is an admission timeout, not an extraction timeout.
    const DWORD deadlineMs = paths.size() > 1024 ? 60000 :
        (command == ARCHIVE_BROKER_COPY_SELECTION ? 30000 : 3000);
    HRESULT last = E_FAIL;
    const bool canRetry = command == ARCHIVE_BROKER_EXTRACT_HERE ||
                          command == ARCHIVE_BROKER_EXTRACT_TO ||
                          command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
                          command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE ||
                          command == ARCHIVE_BROKER_OPEN_FILE;
    for (unsigned attempt = 0;attempt < (canRetry?2u:1u);++attempt) {
        HANDLE h = CreateFileW(name.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                             OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (h == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY &&
            WaitNamedPipeW(name.c_str(),1500))
            h = CreateFileW(name.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                          OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (h == INVALID_HANDLE_VALUE) return HRESULT_FROM_WIN32(GetLastError());
        ArchiveBrokerWireHeader header{ARCHIVE_BROKER_MAGIC,command,
            static_cast < DWORD > (archive.size()),static_cast < DWORD > (paths.size()),id};
        last = ArchiveWriteAll(h, & header,sizeof(header));
        if (SUCCEEDED(last)) last = ArchiveWriteAll(h,archive.data(),header.archiveChars * sizeof(wchar_t));
        // One WriteFile per path does not scale to 40,000 selected entries.
        // Preserve the wire framing while sending 256-KiB chunks instead.
        std::vector < BYTE > frame;
        frame.reserve(256 * 1024 + 4096);
        auto flush = [ & ]() -> HRESULT {
            if (frame.empty()) return S_OK;
            const HRESULT result = ArchiveWriteAll(h,frame.data(),static_cast < DWORD > (frame.size()));
            frame.clear();
            return result;
        };
        for (const auto & path:paths) {
            if (FAILED(last)) break;
            const DWORD length = static_cast < DWORD > (path.size());
            const BYTE * lengthBytes = reinterpret_cast < const BYTE *  > ( & length);
            const BYTE * textBytes = reinterpret_cast < const BYTE *  > (path.data());
            const size_t record = sizeof(length) + path.size() * sizeof(wchar_t);
            if (frame.size() + record > 256 * 1024) last = flush();
            if (FAILED(last)) break;
            frame.insert(frame.end(),lengthBytes,lengthBytes + sizeof(length));
            frame.insert(frame.end(),textBytes,textBytes + path.size() * sizeof(wchar_t));
        }
        if (SUCCEEDED(last)) last = flush();
        if (SUCCEEDED(last)) {
            HRESULT reply = E_FAIL;
            last = ArchiveReadAllWithDeadline(h, & reply,sizeof(reply),GetTickCount64() + deadlineMs);
            if (SUCCEEDED(last)) {
                const DWORD ack = ARCHIVE_BROKER_REPLY_ACK;
                last = ArchiveWriteAll(h, & ack,sizeof(ack));
                if (SUCCEEDED(last)) last = reply;
            }
        }
        CloseHandle(h);
        if (SUCCEEDED(last) && acceptedRequestId) * acceptedRequestId = id;
        // Retry only a read timeout after a complete request was written.
        // Never retry an explicit server rejection or a partially sent frame.
        if (!canRetry || last != HRESULT_FROM_WIN32(ERROR_TIMEOUT)) break;
    }
    return last;
}

// R5: The stock Windows TXT handler sometimes receives a named IStream with
// only the archive's filename. Resolve it using the selected Explorer item,
// through the existing medium-integrity broker. This IPC request is read-only,
// requires an actual live preview HWND owned by the caller, and returns only
// a selected local archive path. No guessed directory or file search.
inline HRESULT ArchiveResolveSelectedPath(const std::wstring & leaf,HWND preview,
                                          std::wstring & path) {
    path.clear();
    if(!preview || !IsWindow(preview) || leaf.empty() || leaf.size() > 255 ||
       leaf.find_first_of(L"\\/:") != std::wstring::npos ||
       !ArchivePreviewOptions::MatchArchive(leaf)) return E_INVALIDARG;
    std::wstring pipeName;
    HRESULT hr = ArchiveBrokerPipeName(pipeName);
    if(FAILED(hr))return hr;
    HANDLE pipe = CreateFileW(pipeName.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(pipe == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY &&
       WaitNamedPipeW(pipeName.c_str(),1200))
        pipe = CreateFileW(pipeName.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                         OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(pipe == INVALID_HANDLE_VALUE)return HRESULT_FROM_WIN32(GetLastError());
    GUID id{};CoCreateGuid( & id);
    const std::wstring anchor = std::to_wstring(static_cast < unsigned long long > (
        reinterpret_cast < uintptr_t > (preview)));
    const ArchiveBrokerWireHeader header{ARCHIVE_BROKER_MAGIC,
        ARCHIVE_BROKER_RESOLVE_SELECTED_PATH,static_cast < DWORD > (leaf.size()),1,id};
    hr = ArchiveWriteAll(pipe, & header,sizeof(header));
    if(SUCCEEDED(hr))hr = ArchiveWriteAll(pipe,leaf.data(),
        static_cast < DWORD > (leaf.size() * sizeof(wchar_t)));
    const DWORD anchorChars = static_cast < DWORD > (anchor.size());
    if(SUCCEEDED(hr))hr = ArchiveWriteAll(pipe, & anchorChars,sizeof(anchorChars));
    if(SUCCEEDED(hr))hr = ArchiveWriteAll(pipe,anchor.data(),
        anchorChars * sizeof(wchar_t));
    const ULONGLONG deadline = GetTickCount64() + 5500;
    HRESULT result = E_FAIL;DWORD chars = 0;
    if(SUCCEEDED(hr))hr = ArchiveReadAllWithDeadline(pipe, & result,sizeof(result),deadline);
    if(SUCCEEDED(hr))hr = ArchiveReadAllWithDeadline(pipe, & chars,sizeof(chars),deadline);
    if(SUCCEEDED(hr)) {
        if(chars > 0 && chars < 32700 && SUCCEEDED(result)) {
            path.resize(chars);
            hr = ArchiveReadAllWithDeadline(pipe,path.data(),
                chars * sizeof(wchar_t),deadline);
            if(SUCCEEDED(hr) && (path.find(L'\0') != std::wstring::npos ||
                path.find_last_of(L"\\/") == std::wstring::npos ||
                _wcsicmp(path.c_str() + path.find_last_of(L"\\/") + 1,
                          leaf.c_str()) != 0))hr = E_INVALIDARG;
        } else if(chars != 0) hr = E_INVALIDARG;
    }
    if(SUCCEEDED(hr)) {
        const DWORD ack = ARCHIVE_BROKER_REPLY_ACK;
        ArchiveWriteAll(pipe, & ack,sizeof(ack));
        if(FAILED(result))hr = result;
    }
    CloseHandle(pipe);
    if(FAILED(hr))path.clear();
    return hr;
}

// Query the actual completion state, not merely whether a request was queued.
// This is intentionally short and retried by an ordinary UI timer.
inline HRESULT ArchiveQueryBrokerOperation(const GUID & requestId,
                                          ArchiveBrokerOperationStatusReply & out) {
    const GUID zero{};
    if (InlineIsEqualGUID(requestId,zero)) return E_INVALIDARG;
    std::wstring name;
    HRESULT hr = ArchiveBrokerPipeName(name);
    if (FAILED(hr)) return hr;
    HANDLE pipe = CreateFileW(name.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (pipe == INVALID_HANDLE_VALUE) return HRESULT_FROM_WIN32(GetLastError());
    const ArchiveBrokerWireHeader header{ARCHIVE_BROKER_MAGIC,
        ARCHIVE_BROKER_QUERY_STATUS,0,0,requestId};
    hr = ArchiveWriteAll(pipe, & header,sizeof(header));
    if (SUCCEEDED(hr))
        hr = ArchiveReadAllWithDeadline(pipe, & out,sizeof(out),GetTickCount64() + 300);
    if (SUCCEEDED(hr)) {
        const DWORD ack = ARCHIVE_BROKER_REPLY_ACK;
        hr = ArchiveWriteAll(pipe, & ack,sizeof(ack));
        if (SUCCEEDED(hr)) hr = out.lookup;
    }
    CloseHandle(pipe);
    return hr;
}

// Cancellation uses a normal archive-scoped pipe request, never a naked GUID.
// The broker verifies that this preview is authorized for the archive and
// matches the operation's origin PID before modifying its state.
inline HRESULT ArchiveCancelBrokerOperation(const GUID & id,const std::wstring & archive) {
    const GUID zero{};
    if (InlineIsEqualGUID(id,zero) || archive.empty() || archive.size() > ARCHIVE_BROKER_MAX_CHARS)
        return E_INVALIDARG;
    std::wstring pipeName;
    HRESULT hr = ArchiveBrokerPipeName(pipeName);
    if (FAILED(hr)) return hr;
    HANDLE pipe = CreateFileW(pipeName.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (pipe == INVALID_HANDLE_VALUE && GetLastError() == ERROR_PIPE_BUSY &&
        WaitNamedPipeW(pipeName.c_str(),1000))
        pipe = CreateFileW(pipeName.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                         OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (pipe == INVALID_HANDLE_VALUE) return HRESULT_FROM_WIN32(GetLastError());
    const ArchiveBrokerWireHeader header{ARCHIVE_BROKER_MAGIC,
        ARCHIVE_BROKER_CANCEL_OPERATION,static_cast < DWORD > (archive.size()),0,id};
    hr = ArchiveWriteAll(pipe, & header,sizeof(header));
    if (SUCCEEDED(hr))
        hr = ArchiveWriteAll(pipe,archive.data(),header.archiveChars * sizeof(wchar_t));
    if (SUCCEEDED(hr)) {
        HRESULT response = E_FAIL;
        hr = ArchiveReadAllWithDeadline(pipe, & response,sizeof(response),GetTickCount64() + 2500);
        if (SUCCEEDED(hr)) {
            const DWORD ack = ARCHIVE_BROKER_REPLY_ACK;
            hr = ArchiveWriteAll(pipe, & ack,sizeof(ack));
            if (SUCCEEDED(hr)) hr = response;
        }
    }
    CloseHandle(pipe);
    return hr;
}

// Stream-only compression filters have one payload, not a tar directory.
// Their Shell provider may report a placeholder zero instead of the real
// expanded byte count. Do not treat that zero as a trustworthy size.
static bool ArchivePreviewIsSingleStreamArchive(const std::wstring & path) {
    static constexpr const wchar_t * tarWrappers[] = {
        L".tar.gz",L".tar.bz2",L".tar.xz",L".tar.zst"};
    for (const wchar_t * suffix:tarWrappers) {
        const size_t length = wcslen(suffix);
        if (path.size() > length &&
            _wcsicmp(path.c_str() + path.size() - length,suffix) == 0) return false;
    }
    static constexpr const wchar_t * streams[] = {L".gz",L".bz2",L".xz",L".zst"};
    for (const wchar_t * suffix:streams) {
        const size_t length = wcslen(suffix);
        if (path.size() > length &&
            _wcsicmp(path.c_str() + path.size() - length,suffix) == 0) return true;
    }
    return false;
}

namespace ArchivePreviewHost {
static constexpr wchar_t kBackgroundProperty[] = L"ArchivePreview.Background.1.1";
static constexpr wchar_t kForegroundProperty[] = L"ArchivePreview.Foreground.1.1";
// Archive Preview 1.1.0
// Copyright (c) 2026 Luiz Bizzio. SPDX-License-Identifier: Apache-2.0
// One archive browser with the original compact toolbar and owner-drawn rows.
// Theme contrast approach adapted from Explorer Column View by ItsMikesch (MIT):
// Upstream MIT attribution is preserved; Apache-2.0 applies to the mod's own code.
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-column-view.wh.cpp
// Unlike a HWND overlay, this is a real IPreviewHandler hosted in Alt+P by Windows.
// To make dark/light reliable even under prevhost.exe, the VISIBLE column rows are
// OWNER DRAWN; archive contents still come from Windows' native Shell namespace.


static constexpr wchar_t kWindowClass[] = L"ArchivePreview.RealHost.0607";
static constexpr wchar_t kToolbarClass[] = L"ArchivePreview.Toolbar.0607";
static constexpr wchar_t kListClass[] = WC_LISTVIEWW;
static constexpr UINT_PTR kThemePollTimer = 177;
static constexpr UINT_PTR kOperationStatusTimer = 179;
static constexpr UINT_PTR kChromeRepairTimer = 180;
static constexpr UINT kOperationPollResultMessage = WM_APP + 201;
static constexpr UINT kOperationPollProgressMessage = WM_APP + 202;
static constexpr UINT kOperationCancelResultMessage = WM_APP + 203;
// Async broker commands and path resolution never wait on the preview UI STA.
static constexpr UINT kAsyncCommandResultMessage = WM_APP + 205;
static constexpr UINT kAsyncGripResultMessage = WM_APP + 206;
static constexpr UINT kAsyncPathResultMessage = WM_APP + 207;
static constexpr UINT kAsyncFallbackResultMessage = WM_APP + 208;
// Compile-time regression: duplicate WM_TIMER IDs disable one handler branch.
static_assert(kThemePollTimer != kOperationStatusTimer &&
    kThemePollTimer != kChromeRepairTimer &&
    kOperationStatusTimer != kChromeRepairTimer,"WM_TIMER IDs must be unique");
static constexpr int kBackButtonId = 301;
static constexpr int kExtractButtonId = 302;
static constexpr int kCancelButtonId = 304;
static constexpr int kExtractToButtonId = 303;
static constexpr int kExtractSelectedButtonId = 305;
static constexpr int kMaxDepth = 16;
static constexpr int kMaxItems = 5000; // fallback page size, not a total archive limit
static constexpr size_t kMaxFallbackPages = 40; // 200k entries, bounded recovery mode

static HINSTANCE g_module = nullptr;
static std::atomic < long > g_objects{0};
static std::atomic < long > g_liveArchiveHandlers{0};
static std::atomic < ULONGLONG > g_nextPollSerial{1};
// Serializing only within one handler is insufficient: Unload's old worker
// can finish after the next handler attaches the same archive to a reused HWND.
// The UI takes only the short owner lock; pipe I/O is serialized on workers.
static std::mutex g_gripIpcMutex;
static std::mutex g_gripOwnerMutex;
static std::unordered_map < HWND,ULONGLONG > g_gripOwners;
// Wh_ModBeforeUninit must also stop an active fallback enumerator. Preview
// hosts can retain COM adapters while Windhawk waits for a safe unload.
static std::atomic<bool> g_hostShutdown{false};
static INIT_ONCE g_classInit = INIT_ONCE_STATIC_INIT;

static COLORREF Mix(COLORREF front, COLORREF back, int opacity) {
    opacity = std::clamp(opacity, 0, 100);
    return RGB((GetRValue(front) * opacity + GetRValue(back) * (100 - opacity)) / 100,
               (GetGValue(front) * opacity + GetGValue(back) * (100 - opacity)) / 100,
               (GetBValue(front) * opacity + GetBValue(back) * (100 - opacity)) / 100);
}
static double Luminance(COLORREF c) {
    const auto calc = [](BYTE channel) {
        double v = channel / 255.0;
        return v <= 0.04045 ? v / 12.92 : std::pow((v + 0.055) / 1.055,2.4);
    };
    return 0.2126 * calc(GetRValue(c)) + 0.7152 * calc(GetGValue(c)) + 0.0722 * calc(GetBValue(c));
}
static double Contrast(COLORREF a, COLORREF b) {
    const double x = Luminance(a), y = Luminance(b);
    return (std::max(x,y) + 0.05) / (std::min(x,y) + 0.05);
}
static COLORREF Readable(COLORREF bg, COLORREF suggested) {
    if (Contrast(bg,suggested) >= 4.5) return suggested;
    const bool white = Contrast(bg,RGB(255,255,255)) >= Contrast(bg,RGB(0,0,0));
    COLORREF soft = white?RGB(238,238,238):RGB(28,28,28);
    return Contrast(bg,soft) >= 4.5?soft:(white?RGB(255,255,255):RGB(0,0,0));
}
struct Palette {
    COLORREF bg{}, panel{}, fg{}, muted{}, hairline{}, selected{}, selectedText{}, accent{};
    bool dark = false;
    bool highContrast = false;
};
// The preview host can supply colors for an unrelated (often light) host frame.
// Prefer the current app theme, then the window frame hint, then consistent
// colors supplied by IPreviewHandlerVisuals. Never force dark globally.
static bool ReadLightThemeSetting(DWORD & light) {
    DWORD bytes = sizeof(light);
    const wchar_t * path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    if(RegGetValueW(HKEY_CURRENT_USER,path,L"AppsUseLightTheme",RRF_RT_REG_DWORD,
                    nullptr, & light, & bytes) == ERROR_SUCCESS)return true;
    bytes = sizeof(light);
    return RegGetValueW(HKEY_CURRENT_USER,path,L"SystemUsesLightTheme",RRF_RT_REG_DWORD,
                        nullptr, & light, & bytes) == ERROR_SUCCESS;
}
static bool WindowRequestsDark(HWND host) {
    if(!host)return false;
    HWND top = GetAncestor(host,GA_ROOT);
    if(!top)return false;
    BOOL dark = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(top,DWMWA_USE_IMMERSIVE_DARK_MODE,
                                          & dark,sizeof(dark))) && dark;
}
static COLORREF SystemAccentColor(bool highContrast) {
    if (!highContrast) {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&color,&opaque)))
            return RGB((color >> 16) & 0xff,(color >> 8) & 0xff,color & 0xff);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}
static Palette ResolvePalette(HWND host, COLORREF hostBg, COLORREF hostText,
                              bool hasBg, bool hasText, int brokerDark =  - 1) {
    Palette p{};
    HIGHCONTRASTW hc{};hc.cbSize = sizeof(hc);
    p.highContrast = SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(hc), & hc,0) &&
        (hc.dwFlags & HCF_HIGHCONTRASTON);
    DWORD light = 1;
    const bool known = ReadLightThemeSetting(light);
    // The app theme setting is the primary source of truth for Explorer.
    // A host dark hint can help when the registry value is absent.
    p.dark = !p.highContrast && (brokerDark >= 0 ? brokerDark != 0 :
        ArchivePreviewOptions::Dark(known ? light == 0 : WindowRequestsDark(host)));
    p.bg = p.dark?RGB(28,28,28):RGB(255,255,255);
    p.fg = p.dark?RGB(238,238,238):RGB(28,28,28);
    if(p.highContrast){
        p.bg = GetSysColor(COLOR_WINDOW);
        p.fg = GetSysColor(COLOR_WINDOWTEXT);
    }else if(hasBg && hasText){
        // prevhost often sends a white default even when Explorer is dark.
        // Use host colors only when they agree with the effective app theme.
        const bool hostDark = Luminance(hostBg) < 0.25;
        const bool hostLight = Luminance(hostBg) > 0.65;
        if((p.dark && hostDark) || (!p.dark && hostLight)){
            p.bg = hostBg;
            p.fg = hostText;
        }
    }
    p.fg = Readable(p.bg,p.fg);
    p.panel = Mix(p.fg,p.bg,p.dark?7:4);
    p.muted = Readable(p.bg,Mix(p.fg,p.bg,76));
    p.hairline = Mix(p.fg,p.bg,p.dark?21:17);
    p.accent = SystemAccentColor(p.highContrast);
    p.selected = p.accent;
    p.selectedText = p.highContrast?GetSysColor(COLOR_HIGHLIGHTTEXT):
        Readable(p.selected,p.fg);
    return p;
}


static bool IsArchive(const std::wstring & path) {
    return ArchivePreviewOptions::MatchArchive(path);
}
static std::wstring DisplayName(IShellItem * item, SIGDN which = SIGDN_NORMALDISPLAY) {
    PWSTR p = nullptr;
    HRESULT hr = item?item->GetDisplayName(which, & p):E_INVALIDARG;
    std::wstring s = SUCCEEDED(hr) && p?p:L"";
    CoTaskMemFree(p);return s;
}
static std::wstring Filename(const std::wstring & s) {
    auto pos = s.find_last_of(L"\\/");return pos == std::wstring::npos?s:s.substr(pos + 1);
}
static std::wstring HumanSize(ULONGLONG n) {
    wchar_t out[72]{};
    return StrFormatByteSizeW(static_cast < LONGLONG > (n),out,72)?out:L"";
}
static bool Within(PIDLIST_ABSOLUTE root, PIDLIST_ABSOLUTE target) {
    return root && target && (ILIsEqual(root,target) || ILIsParent(root,target,FALSE));
}
static int Scale(HWND hwnd,int x) {
    UINT dpi = hwnd?GetDpiForWindow(hwnd):96;
    return MulDiv(x,dpi?dpi:96,96);
}
static void Fill(HDC dc,const RECT & rect,COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    if(brush){FillRect(dc, & rect,brush);DeleteObject(brush);}
}
static void Label(HDC dc,RECT r,const std::wstring & text,HFONT font,COLORREF color,UINT fmt) {
    if(text.empty())return;
    HGDIOBJ old = font?SelectObject(dc,font):nullptr;
    ::SetBkMode(dc,TRANSPARENT);::SetTextColor(dc,color);
    DrawTextW(dc,text.c_str(), - 1, & r,fmt | DT_NOPREFIX);
    if(old)SelectObject(dc,old);
}

struct Entry {
    PIDLIST_ABSOLUTE id{};
    std::wstring name;
    std::wstring kind;
    std::wstring modifiedLabel;
    ULONGLONG modifiedValue{};
    ULONGLONG bytes{};
    bool hasSize{};
    bool folder{};
    int iconIndex =  - 1;
    std::shared_ptr < ArchivePreviewIcons::Icon > iconImage;
    ~Entry(){CoTaskMemFree(id);}
    Entry() = default;
    Entry(const Entry & ) = delete;Entry & operator = (const Entry & ) = delete;
    Entry(Entry && x) noexcept : id(x.id),name(std::move(x.name)),kind(std::move(x.kind)),
        modifiedLabel(std::move(x.modifiedLabel)),modifiedValue(x.modifiedValue),
        bytes(x.bytes),hasSize(x.hasSize),folder(x.folder),iconIndex(x.iconIndex),
        iconImage(std::move(x.iconImage)){x.id = nullptr;}
    Entry & operator = (Entry && x) noexcept {if(this !=  & x){CoTaskMemFree(id);id = x.id;x.id = nullptr;
        name = std::move(x.name);kind = std::move(x.kind);
        modifiedLabel = std::move(x.modifiedLabel);modifiedValue = x.modifiedValue;
        bytes = x.bytes;hasSize = x.hasSize;
        folder = x.folder;iconIndex = x.iconIndex;iconImage = std::move(x.iconImage);}return * this;}
};
struct Column {
    HWND list{};
    PIDLIST_ABSOLUTE folderId{};
    std::wstring heading;
    std::vector < Entry > entries;
    std::vector < int > visibleColumns; // 0 Name, 1 Date modified, 2 Type, 3 Size
    int appliedLanguage{-1};
    int sortColumn{};
    bool sortAscending{true};
    bool populated{};
    ULONGLONG archiveBytes{};
    unsigned metadataMask{~0u};
    unsigned failedMetadataMask{~0u};
    size_t page{};
    bool hasNextPage{};
    Column() = default;
    Column(const Column & ) = delete;Column & operator = (const Column & ) = delete;
    ~Column(){CoTaskMemFree(folderId);}
};

class ArchiveHandler final : public IPreviewHandler, public IInitializeWithFile,
    public IOleWindow, public IObjectWithSite, public IPreviewHandlerVisuals {
    std::atomic < ULONG > refs_{1};
    IUnknown * site_{};
    HWND parent_{}, window_{}, toolbar_{}, backButton_{}, extractButton_{}, extractToButton_{}, extractSelectedButton_{}, cancelButton_{};
    HWND brokerBoundList_{};
    bool brokerBrowserReady_{};
    // A corrupt/unsupported archive must not start a new model + fallback
    // enumeration on every theme timer tick. These flags are per selection.
    bool brokerLoadFailed_{};
    bool fallbackLoadFailed_{};
    bool terminalLoadFailure_{};
    // Only startup pipe errors receive a bounded grace period. A rejected
    // archive or failed Shell enumeration must never reattach on a timer.
    ULONGLONG brokerStartupFailureAt_{};
    bool brokerThemeKnown_{};
    bool brokerThemeDark_{};
    bool brokerCanGoBack_{};
    int brokerSelectionCount_{};
    ULONGLONG brokerAttachAttempt_{};
    ULONGLONG previewStartedAt_{};
    // Only an explicit, verified Explorer selection change retires a preview.
    // Ancestors may be temporarily invisible while Explorer builds Alt+P.
    bool selectionRetired_{};
    RECT area_{};
    std::wstring archive_, error_, status_;
    ULONGLONG statusUntil_{}; // Fallback notices expire; operation progress does not.
    int fallbackNoticeLaidOut_{}; // Remember last actual height across timer expiry.
    POINT mouseDown_{};
    int mouseDownIndex_ = - 1;
    bool mouseDownCustomHit_ = false;
    std::vector < int > mouseDownSelection_;
    bool dragStarted_ = false;
    bool dragInProgress_ = false;
    GUID pendingExtraction_{};
    bool extractionPending_{};
    ULONGLONG extractionAcceptedAt_{};
    bool fallbackProgressVisible_{};
    bool cancelRequested_{};
    DWORD extractionStage_{ARCHIVE_PROGRESS_NONE};
    DWORD extractionPercent_{};
    bool extractionPercentKnown_{};
    // A worker performs pipe I/O. No 300 ms IPC reads on prevhost's UI STA.
    ULONGLONG statusPollSerial_{};
    bool statusPollRunning_{};
    unsigned statusPollFailures_{};
    struct OperationCancelJob {
        HWND hwnd{};
        GUID requestId{};
        std::wstring archive;
        ULONGLONG serial{};
    };
    static DWORD WINAPI OperationCancelWorker(LPVOID raw) {
        std::unique_ptr < OperationCancelJob > job(static_cast < OperationCancelJob *  > (raw));
        const HRESULT hr = ArchiveCancelBrokerOperation(job->requestId,job->archive);
        PostMessageW(job->hwnd,kOperationCancelResultMessage,
                     static_cast < WPARAM > (job->serial),
                     static_cast < LPARAM > (static_cast < DWORD > (hr)));
        return 0;
    }
    struct OperationPollJob {
        HWND hwnd{};
        GUID requestId{};
        ULONGLONG serial{};
    };
    static DWORD WINAPI OperationPollWorker(LPVOID raw) {
        std::unique_ptr < OperationPollJob > job(static_cast < OperationPollJob *  > (raw));
        ArchiveBrokerOperationStatusReply reply{};
        const HRESULT query = ArchiveQueryBrokerOperation(job->requestId,reply);
        const DWORD phase = SUCCEEDED(query)?reply.phase:ARCHIVE_OPERATION_UNKNOWN;
        const DWORD result = static_cast < DWORD > (SUCCEEDED(query)?reply.result:query);
        // Scalar messages only. A pointer payload could leak if the preview
        // HWND is destroyed while the IPC worker is still running.
        if (SUCCEEDED(query)) {
            const DWORD progressBits = ((reply.progressStage & 0xffu) << 24) |
                ((reply.progressKnown?1u:0u) << 23) |
                (std::min < DWORD > (reply.progressPercent,100u) & 0x7fu);
            PostMessageW(job->hwnd,kOperationPollProgressMessage,
                static_cast < WPARAM > (job->serial),static_cast < LPARAM > (progressBits));
        }
        const ULONGLONG packed = (static_cast < ULONGLONG > (phase) << 32) | result;
        PostMessageW(job->hwnd,kOperationPollResultMessage,
                     static_cast < WPARAM > (job->serial),static_cast < LPARAM > (packed));
        return 0;
    }
    PIDLIST_ABSOLUTE archiveId_{};
    std::vector < std::unique_ptr < Column >> cols_; // navigation history, only the last list is visible
    HIMAGELIST icons_{}; // Shell owns this image list
    Palette palette_{};
    COLORREF hostBg_{}, hostText_{};
    bool hasHostBg_{}, hasHostText_{};
    LOGFONTW hostFont_{};
    bool hasHostFont_{};
    // Preview handler lifecycle: OLE must be paired with OleUninitialize,
    // and DoPreview must not create a second child window on repeat calls.
    bool oleInitialized_{};
    // Each request owns its inputs and result independently of ArchiveHandler.
    // The UI compares the globally unique serial before reading any completion.
    struct CommandJob {
        HWND hwnd{};
        ULONGLONG serial{};
        DWORD command{};
        std::wstring archive;
        std::vector<std::wstring> paths;
        GUID requestId{};
        HRESULT result{E_PENDING};
        std::atomic<bool> done{false};
    };
    std::shared_ptr<CommandJob> commandJob_;
    static DWORD WINAPI CommandWorker(LPVOID raw) {
        std::unique_ptr<std::shared_ptr<CommandJob>> holder(
            static_cast<std::shared_ptr<CommandJob>*>(raw));
        const auto job = *holder;
        GUID requestId{};
        job->result = ArchiveSendBrokerCommand(job->command, job->archive,
                                                job->paths, &requestId);
        job->requestId = requestId;
        job->done.store(true, std::memory_order_release);
        PostMessageW(job->hwnd, kAsyncCommandResultMessage,
                     static_cast<WPARAM>(job->serial), 0);
        return 0;
    }
    bool QueueCommand(DWORD command, std::vector<std::wstring> paths,
                      const wchar_t* status) {
        if (!window_ || commandJob_ || archive_.empty() || resolveJob_ ||
            archive_.find_first_of(L"\\/:") == std::wstring::npos) return false;
        auto job = std::make_shared<CommandJob>();
        job->hwnd = window_;
        job->serial = g_nextPollSerial.fetch_add(1);
        job->command = command;
        job->archive = archive_;
        job->paths = std::move(paths);
        auto* holder = new(std::nothrow) std::shared_ptr<CommandJob>(job);
        if (!holder) return false;
        commandJob_ = job;
        if (!ArchivePreviewWorkerLifetime::Start(CommandWorker, holder)) {
            commandJob_.reset();
            delete holder;
            return false;
        }
        if (status) SetStatus(status);
        Layout();
        return true;
    }
    void CompleteCommand(ULONGLONG serial) {
        auto job = commandJob_;
        if (!job || job->serial != serial ||
            !job->done.load(std::memory_order_acquire)) return;
        commandJob_.reset();
        const HRESULT hr = job->result;
        if (SUCCEEDED(hr)) {
            switch (job->command) {
                case ARCHIVE_BROKER_EXTRACT_HERE:
                    TrackExtraction(job->requestId, L"Extracting archive...");
                    break;
                case ARCHIVE_BROKER_EXTRACT_TO:
                    TrackExtraction(job->requestId, L"Choose an extraction folder...");
                    break;
                case ARCHIVE_BROKER_EXTRACT_SELECTED_HERE:
                    TrackExtraction(job->requestId, L"Extracting selected items here...");
                    break;
                case ARCHIVE_BROKER_EXTRACT_SELECTED_TO:
                    TrackExtraction(job->requestId, L"Choose a folder to extract selected items.");
                    break;
                case ARCHIVE_BROKER_COPY_SELECTION:
                    SetStatus(L"Files copied. Paste in Explorer with Ctrl+V.");
                    break;
                default:
                    SetStatus(L"");
                    break;
            }
        } else {
            wchar_t message[160]{};
            swprintf_s(message, L"Broker command failed (0x%08lX).",
                       static_cast<unsigned long>(hr));
            SetStatus(message);
            if (job->command == ARCHIVE_BROKER_BROWSER_BACK) {
                brokerBrowserReady_ = false;
                brokerCanGoBack_ = false;
                EnsureFallbackContent();
            }
        }
        Layout();
    }
    // One worker serializes each preview's desired transitions. The process-
    // wide IPC gate also orders old and new sessions without blocking the UI.
    struct GripState {
        const ULONGLONG ownerToken = g_nextPollSerial.fetch_add(1);
        std::mutex mutex;
        HWND hwnd{};
        HWND desired{};
        HWND applied{};
        std::wstring desiredArchive;
        std::wstring appliedArchive;
        bool running{};
        ULONGLONG serial{};
        ~GripState() {
            std::lock_guard<std::mutex> lock(g_gripOwnerMutex);
            std::erase_if(g_gripOwners,[this](const auto & entry) {
                return entry.second == ownerToken;
            });
        }
    };
    std::shared_ptr<GripState> gripState_;
    static DWORD WINAPI GripWorker(LPVOID raw) {
        std::unique_ptr<std::shared_ptr<GripState>> holder(
            static_cast<std::shared_ptr<GripState>*>(raw));
        auto state = *holder;
        for (;;) {
            HWND desired{}, applied{}, callback{};
            std::wstring desiredArchive, appliedArchive;
            ULONGLONG serial{};
            {
                std::lock_guard<std::mutex> lock(state->mutex);
                desired = state->desired;
                applied = state->applied;
                desiredArchive = state->desiredArchive;
                appliedArchive = state->appliedArchive;
                callback = state->hwnd;
                serial = state->serial;
                if (applied == desired &&
                    (!desired || appliedArchive == desiredArchive)) {
                    state->running = false;
                    return 0;
                }
            }
            HRESULT hr = S_OK;
            {
                std::lock_guard<std::mutex> ipcLock(g_gripIpcMutex);
                // A desired transition may have changed while another preview's
                // request held the IPC gate. Never send that stale snapshot.
                {
                    std::lock_guard<std::mutex> lock(state->mutex);
                    if (state->serial != serial) continue;
                }
                const HWND anchor = applied?applied:desired;
                bool ownsAnchor = false;
                {
                    std::lock_guard<std::mutex> ownerLock(g_gripOwnerMutex);
                    const auto owner = g_gripOwners.find(anchor);
                    ownsAnchor = owner != g_gripOwners.end() &&
                        owner->second == state->ownerToken;
                }
                if (ownsAnchor) {
                    if (applied) {
                        hr = ArchiveSendBrokerCommand(ARCHIVE_BROKER_GRIP_CLEAR,
                            appliedArchive, {std::to_wstring(static_cast<unsigned long long>(
                                reinterpret_cast<uintptr_t>(applied)))});
                    } else if (desired) {
                        hr = ArchiveSendBrokerCommand(ARCHIVE_BROKER_GRIP_UPDATE,
                            desiredArchive, {std::to_wstring(static_cast<unsigned long long>(
                                reinterpret_cast<uintptr_t>(desired)))});
                    }
                } else {
                    // A newer session owns this HWND. Its update either already
                    // ran or will follow this gate. Do not clear its UI or its
                    // broker authorization, even when the archive path matches.
                    Wh_Log(L"ArchivePreview: skipped obsolete preview binding (anchor=%p)",anchor);
                }
            }
            {
                std::lock_guard<std::mutex> lock(state->mutex);
                if (SUCCEEDED(hr)) {
                    state->applied = applied ? nullptr : desired;
                    state->appliedArchive = applied ? L"" : desiredArchive;
                } else if (applied) {
                    // A missing old broker no longer owns the previous HWND.
                    // Forget a failed clear, then try attaching the new view.
                    state->applied = nullptr;
                    state->appliedArchive.clear();
                    continue;
                } else {
                    state->running = false;
                }
            }
            if (FAILED(hr)) {
                PostMessageW(callback, kAsyncGripResultMessage,
                    static_cast<WPARAM>(serial), static_cast<LPARAM>(hr));
                return 0;
            }
        }
    }
    struct ResolveJob {
        HWND hwnd{};
        ULONGLONG serial{};
        std::wstring leaf;
        std::wstring resultPath;
        HRESULT result{E_PENDING};
        std::atomic<bool> cancelled{false};
        std::atomic<bool> done{false};
    };
    std::shared_ptr<ResolveJob> resolveJob_;
    static DWORD WINAPI ResolveWorker(LPVOID raw) {
        std::unique_ptr<std::shared_ptr<ResolveJob>> holder(
            static_cast<std::shared_ptr<ResolveJob>*>(raw));
        auto job = *holder;
        // The Explorer broker can be starting on demand. Retry only startup
        // connection failures, off the UI thread, while the HWND still exists.
        for (unsigned attempt = 0; attempt < 18; ++attempt) {
            if (job->cancelled.load(std::memory_order_acquire)) break;
            job->result = ArchiveResolveSelectedPath(job->leaf, job->hwnd,
                                                     job->resultPath);
            if (job->result != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) &&
                job->result != HRESULT_FROM_WIN32(ERROR_PIPE_BUSY)) break;
            if (!IsWindow(job->hwnd)) break;
            Sleep(150);
        }
        job->done.store(true, std::memory_order_release);
        PostMessageW(job->hwnd, kAsyncPathResultMessage,
                     static_cast<WPARAM>(job->serial), 0);
        return 0;
    }
    bool StartPathResolution() {
        if (!window_ || resolveJob_) return false;
        auto job = std::make_shared<ResolveJob>();
        job->hwnd = window_;
        job->serial = g_nextPollSerial.fetch_add(1);
        job->leaf = archive_;
        auto* holder = new(std::nothrow) std::shared_ptr<ResolveJob>(job);
        if (!holder) return false;
        resolveJob_ = job;
        if (!ArchivePreviewWorkerLifetime::Start(ResolveWorker, holder)) {
            resolveJob_.reset();
            delete holder;
            return false;
        }
        SetStatus(L"Locating selected archive...");
        return true;
    }
    // The non-IPC Shell initialization stays on the host STA. No pipe wait.
    // Data-only recovery enumeration. Shell COM objects belong to a worker STA;
    // Only cloned PIDLs, strings, numeric metadata and owned icon copies cross
    // back to the UI STA; no Shell COM interface is passed between apartments.
    struct FallbackJob {
        HWND hwnd{};
        HWND list{};
        ULONGLONG serial{};
        std::wstring archive;
        PIDLIST_ABSOLUTE archiveId{};
        PIDLIST_ABSOLUTE folderId{};
        size_t page{};
        size_t previousPage{SIZE_MAX};
        ArchivePreviewOptions::MetadataPolicy metadataPolicy{};
        ULONGLONG archiveBytes{};
        unsigned metadataMask{};
        bool refreshMetadata{};
        std::vector<Entry> entries;
        HIMAGELIST icons{}; // system-owned image list, never destroy
        bool hasNextPage{};
        HRESULT result{E_PENDING};
        std::atomic<bool> cancelled{false};
        std::atomic<bool> done{false};
        ~FallbackJob() {
            CoTaskMemFree(archiveId);
            CoTaskMemFree(folderId);
        }
    };
    std::shared_ptr<FallbackJob> fallbackJob_;
    void StopRetryingFailedArchive() {
        if (terminalLoadFailure_ || !brokerLoadFailed_ ||
            !fallbackLoadFailed_) return;
        terminalLoadFailure_ = true;
        // Both providers failed for this selection. Do not retry on timer ticks.
        SyncGrip(true);
        error_ = L"Windows cannot read this archive. Select another file to try again.";
        SetStatus(error_);
        Layout();
    }
    void MarkBrokerLoadFailed() {
        if (selectionRetired_ || !uiReady_ || brokerLoadFailed_) return;
        brokerLoadFailed_ = true;
        brokerStartupFailureAt_ = 0;
        brokerBrowserReady_ = false;
        brokerCanGoBack_ = false;
        // Remove the failed broker list, even when fallback enumeration works.
        // In particular, no subsequent timer may create another broker model.
        SyncGrip(true);
        StopRetryingFailedArchive();
        if (!terminalLoadFailure_) EnsureFallbackContent();
    }
    static DWORD WINAPI FallbackWorker(LPVOID raw) {
        std::unique_ptr<std::shared_ptr<FallbackJob>> holder(
            static_cast<std::shared_ptr<FallbackJob>*>(raw));
        auto job = *holder;
        const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (SUCCEEDED(com)) {
            job->result = CollectFallback(*job);
            CoUninitialize();
        } else {
            job->result = com;
        }
        job->done.store(true, std::memory_order_release);
        PostMessageW(job->hwnd, kAsyncFallbackResultMessage,
                     static_cast<WPARAM>(job->serial), 0);
        return 0;
    }
    void QueueFallback(Column& col, size_t previousPage = SIZE_MAX, bool refreshMetadata = false) {
        if (terminalLoadFailure_ || fallbackLoadFailed_ ||
            g_hostShutdown.load(std::memory_order_acquire) ||
            (col.populated && !refreshMetadata) || !window_ || !archiveId_ || !col.folderId) return;
        if (fallbackJob_ && fallbackJob_->list == col.list &&
            fallbackJob_->page == col.page) return;
        if (fallbackJob_) fallbackJob_->cancelled.store(true, std::memory_order_release);
        auto job = std::make_shared<FallbackJob>();
        job->hwnd = window_;
        job->list = col.list;
        job->serial = g_nextPollSerial.fetch_add(1);
        job->archive = archive_;
        job->archiveId = ILCloneFull(archiveId_);
        job->folderId = ILCloneFull(col.folderId);
        job->page = col.page;
        job->previousPage = previousPage;
        job->metadataPolicy = ArchivePreviewOptions::MetadataSnapshot();
        job->refreshMetadata = refreshMetadata;
        job->metadataMask = job->metadataPolicy.ForArchive(col.archiveBytes);
        if (!job->archiveId || !job->folderId) {
            fallbackLoadFailed_ = true;
            error_ = L"Not enough memory for recovery enumeration.";
            SetStatus(error_);
            StopRetryingFailedArchive();
            return;
        }
        auto* holder = new(std::nothrow) std::shared_ptr<FallbackJob>(job);
        if (!holder) {
            fallbackLoadFailed_ = true;
            error_ = L"Could not allocate recovery worker state.";
            SetStatus(error_);
            StopRetryingFailedArchive();
            return;
        }
        fallbackJob_ = job;
        if (!ArchivePreviewWorkerLifetime::Start(FallbackWorker, holder)) {
            fallbackJob_.reset();
            delete holder;
            fallbackLoadFailed_ = true;
            error_ = L"Could not start recovery enumeration.";
            SetStatus(error_);
            StopRetryingFailedArchive();
            return;
        }
        SetStatus(L"Loading archive items...");
    }
    void CompleteFallback(ULONGLONG serial) {
        auto job = fallbackJob_;
        if (!job || job->serial != serial ||
            !job->done.load(std::memory_order_acquire)) return;
        fallbackJob_.reset();
        if (job->cancelled.load(std::memory_order_acquire) ||
            g_hostShutdown.load(std::memory_order_acquire) ||
            brokerBrowserReady_ || selectionRetired_) return;
        Column* col = nullptr;
        for (auto& c : cols_) {
            if (c && c->list == job->list && c->page == job->page) {
                col = c.get();
                break;
            }
        }
        if (!col || cols_.empty() || cols_.back().get() != col) return;
        if (SUCCEEDED(job->result) &&
            job->metadataMask != ArchivePreviewOptions::MetadataMask(job->archiveBytes)) {
            QueueFallback(*col,job->previousPage,job->refreshMetadata);
            return;
        }
        if (FAILED(job->result)) {
            // Cancellation is not an archive-format failure.
            if (job->result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) return;
            if (job->refreshMetadata) {
                col->failedMetadataMask = job->metadataMask;
                col->populated = true;
                SetStatus(L"Could not reload archive details.");
            } else if (job->previousPage != SIZE_MAX) {
                col->page = job->previousPage;
                col->populated = true;
                SetStatus(L"Could not load this recovery page.");
                if (col->list) ShowWindow(col->list, SW_SHOWNOACTIVATE);
            } else {
                fallbackLoadFailed_ = true;
                error_ = L"The Shell cannot enumerate this archive format.";
                SetStatus(error_);
                StopRetryingFailedArchive();
            }
            Layout();
            return;
        }
        Wh_Log(L"[AP diag/host] fallback completed pid=%lu self=%p list=%p rows=%zu",
            GetCurrentProcessId(),this,col->list,job->entries.size());
        diagnosticPending_ = true;
        fallbackLoadFailed_ = false;
        std::unordered_set<std::wstring> selected;
        std::wstring focused, top;
        if (job->refreshMetadata && col->list) {
            const auto nameAt = [&](int row) -> std::wstring {
                return row >= 0 && static_cast<size_t>(row) < col->entries.size() ?
                    col->entries[row].name : std::wstring{};
            };
            for (int row : SelectedIndices()) selected.insert(nameAt(row));
            focused = nameAt(ListView_GetNextItem(col->list,-1,LVNI_FOCUSED));
            top = nameAt(ListView_GetTopIndex(col->list));
        }
        col->archiveBytes = job->archiveBytes;
        col->metadataMask = job->metadataMask;
        col->failedMetadataMask = ~0u;
        col->entries = std::move(job->entries);
        OrderFallbackRows(*col);
        col->hasNextPage = job->hasNextPage;
        col->populated = true;
        if (job->icons) icons_ = job->icons;
        if (col->list && IsWindow(col->list)) {
            SendMessageW(col->list, WM_SETREDRAW, FALSE, 0);
            ListView_DeleteAllItems(col->list);
            int topRow = -1;
            for (int i = 0; i < static_cast<int>(col->entries.size()); ++i) {
                LVITEMW row{};
                row.mask = LVIF_TEXT | LVIF_PARAM;
                row.iItem = i;
                row.pszText = const_cast<LPWSTR>(col->entries[i].name.c_str());
                row.lParam = i;
                ListView_InsertItem(col->list, &row);
                if (job->refreshMetadata) {
                    const auto & name = col->entries[i].name;
                    const UINT state = (selected.count(name) ? LVIS_SELECTED : 0) |
                        (name == focused ? LVIS_FOCUSED : 0);
                    if (state) ListView_SetItemState(col->list,i,state,LVIS_SELECTED | LVIS_FOCUSED);
                    if (name == top) topRow = i;
                }
            }
            if (topRow >= 0) ListView_EnsureVisible(col->list,topRow,FALSE);
            SendMessageW(col->list, WM_SETREDRAW, TRUE, 0);
            ShowWindow(col->list, SW_SHOWNOACTIVATE);
            InvalidateRect(col->list, nullptr, FALSE);
        }
        if (col->page || col->hasNextPage) {
            std::wstring info = L"Recovery page " + std::to_wstring(col->page + 1) +
                L". Ctrl+PgDown: next, Ctrl+PgUp: previous.";
            if (col->page + 1 == kMaxFallbackPages &&
                col->entries.size() == kMaxItems)
                info = L"Recovery reached its 200,000-entry safety limit.";
            SetStatus(info);
        } else SetStatus(L"");
        if (job->previousPage != SIZE_MAX && col->list && !col->entries.empty()) {
            ListView_EnsureVisible(col->list, 0, FALSE);
            ListView_SetItemState(col->list, 0, LVIS_FOCUSED, LVIS_FOCUSED);
        }
        Layout();
        ArchivePreviewIcons::LogStats(L"recovery");
    }
    bool uiReady_{};
    unsigned diagnosticFallbackPaints_{};
    unsigned diagnosticFallbackIcons_{};
    unsigned diagnosticFallbackMisses_{};
    bool diagnosticPending_{true};
    int fallbackResizeColumn_{-1};
    int fallbackResizeStartWidth_{};
    bool themeApplied_{};
    int appliedLanguage_{-1};
    HFONT font_{};
    UINT dpi_{};

    static BOOL CALLBACK InitClass(PINIT_ONCE, PVOID, PVOID * ) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = g_module;
        wc.lpszClassName = kWindowClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.style = CS_DBLCLKS;
        if (!g_module || !RegisterClassExW( & wc)) return FALSE;
        WNDCLASSEXW bar{};
        bar.cbSize = sizeof(bar);
        bar.lpfnWndProc = ToolbarProc;
        bar.hInstance = g_module;
        bar.lpszClassName = kToolbarClass;
        bar.hCursor = LoadCursorW(nullptr,IDC_ARROW);
        bar.style = 0; // Explicit size invalidation instead of full-surface erase
        if (!RegisterClassExW( & bar)) {
            // Only unregister what this DLL instance registered successfully.
            UnregisterClassW(kWindowClass, g_module);
            return FALSE;
        }
        return TRUE;
    }
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        if (msg == WM_NCCREATE) {
            auto * cs = reinterpret_cast < CREATESTRUCTW *  > (lp);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast < LONG_PTR > (cs->lpCreateParams));
        }
        auto * self = reinterpret_cast < ArchiveHandler *  > (GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!self) return DefWindowProcW(hwnd, msg, wp, lp);
        switch (msg) {
            case kAsyncCommandResultMessage:
                self->CompleteCommand(static_cast<ULONGLONG>(wp));
                return 0;
            case kAsyncGripResultMessage: {
                bool currentFailure = false;
                if (self->gripState_) {
                    std::lock_guard<std::mutex> lock(self->gripState_->mutex);
                    currentFailure = self->gripState_->serial ==
                        static_cast<ULONGLONG>(wp);
                }
                if (!currentFailure || self->selectionRetired_ || !self->uiReady_ ||
                    self->brokerLoadFailed_) return 0;
                const HRESULT result = static_cast<HRESULT>(static_cast<DWORD>(lp));
                const bool starting = result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) ||
                    result == HRESULT_FROM_WIN32(ERROR_PIPE_BUSY);
                if (starting) {
                    const ULONGLONG now = GetTickCount64();
                    if (!self->brokerStartupFailureAt_) self->brokerStartupFailureAt_ = now;
                    // Give an on-demand broker a limited chance to initialize.
                    // On persistent connection failures, keep the recovery view
                    // instead of retrying forever (including offline brokers).
                    if (now - self->brokerStartupFailureAt_ < 8000) return 0;
                }
                // E_ACCESSDENIED, broken pipe, invalid paths and model failures
                // are not startup errors. Only a new selection can retry them.
                self->MarkBrokerLoadFailed();
                return 0;
            }
            case kAsyncFallbackResultMessage:
                self->CompleteFallback(static_cast<ULONGLONG>(wp));
                return 0;
            case kAsyncPathResultMessage: {
                auto job = self->resolveJob_;
                if (!job || job->serial != static_cast<ULONGLONG>(wp) ||
                    !job->done.load(std::memory_order_acquire)) return 0;
                self->resolveJob_.reset();
                if (FAILED(job->result)) {
                    self->error_ = L"Cannot identify the selected archive path.";
                    self->SetStatus(self->error_);
                } else {
                    self->archive_ = std::move(job->resultPath);
                    self->SetStatus(L"");
                    self->FinishStart();
                    if (self->archiveId_) self->SyncGrip();
                }
                self->Layout();
                return 0;
            }
            case ARCHIVE_BROWSER_CANCEL_FROM_FOOTER:
                // A broker HWND may be shared by multiple Explorer previews.
                // Only the Preview Handler owning this exact operation accepts it.
                if (self->extractionPending_ &&
                    wp == ArchiveRequestUiToken(self->pendingExtraction_))
                    self->CancelExtraction();
                return 0;
            case kOperationCancelResultMessage: {
                if (!self->extractionPending_ ||
                    static_cast < ULONGLONG > (wp) != self->statusPollSerial_) return 0;
                const HRESULT result = static_cast < HRESULT > (static_cast < DWORD > (lp));
                if (FAILED(result)) {
                    self->cancelRequested_ = false;
                    if (self->cancelButton_) EnableWindow(self->cancelButton_,TRUE);
                    wchar_t warning[128]{};
                    swprintf_s(warning,L"Cancel request failed (0x%08lX).",
                               static_cast < unsigned long > (result));
                    self->SetStatus(warning);
                } else self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::Cancelling));
                return 0;
            }
            case kOperationPollProgressMessage: {
                if (!self->extractionPending_ ||
                    static_cast < ULONGLONG > (wp) != self->statusPollSerial_) return 0;
                const DWORD bits = static_cast < DWORD > (static_cast < ULONG_PTR > (lp));
                const DWORD stage = (bits >> 24) & 0xffu;
                const bool known = (bits & (1u << 23)) != 0;
                const DWORD percent = bits & 0x7fu;
                if (stage == self->extractionStage_ && known == self->extractionPercentKnown_ &&
                    percent == self->extractionPercent_) return 0;
                self->extractionStage_ = stage;
                self->extractionPercentKnown_ = known;
                self->extractionPercent_ = percent;
                if (self->cancelRequested_) self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::Cancelling));
                else if (stage == ARCHIVE_PROGRESS_SCANNING) self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::PreparingFiles));
                else if (stage == ARCHIVE_PROGRESS_PICKING) self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractTo));
                else if (stage == ARCHIVE_PROGRESS_EXTRACTING) {
                    if (known) {
                        self->SetStatus(std::wstring(ArchivePreviewText::Get(ArchivePreviewText::Id::Extracting)) +
                            L" " + std::to_wstring(percent) + L"%");
                    } else self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::Extracting));
                } else if (stage == ARCHIVE_PROGRESS_EXTERNAL) {
                    self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::CancelInExternal));
                    self->Layout();
                } else if (stage == ARCHIVE_PROGRESS_NONE) self->SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::StartingExtraction));
                return 0;
            }
            case kOperationPollResultMessage: {
                // The result is scalar: no heap-owned message can leak if the
                // window disappears while a pipe request is in flight.
                if (!self->extractionPending_ ||
                    static_cast < ULONGLONG > (wp) != self->statusPollSerial_) return 0;
                self->statusPollRunning_ = false;
                const ULONGLONG bits = static_cast < ULONGLONG > (static_cast < ULONG_PTR > (lp));
                const DWORD phase = static_cast < DWORD > (bits >> 32);
                const HRESULT result = static_cast < HRESULT > (static_cast < DWORD > (bits));
                if (phase == ARCHIVE_OPERATION_UNKNOWN) {
                    if (++self->statusPollFailures_ == 15)
                        self->SetStatus(L"Transfer broker not responding. Still checking operation status...");
                    return 0;
                }
                self->statusPollFailures_ = 0;
                if (phase == ARCHIVE_OPERATION_DONE) {
                    self->extractionPending_ = false;
                    self->extractionAcceptedAt_ = 0;
                    self->fallbackProgressVisible_ = false;
                    self->cancelRequested_ = false;
                    if (self->cancelButton_) ShowWindow(self->cancelButton_,SW_HIDE);
                    KillTimer(hwnd,kOperationStatusTimer);
                    self->extractionStage_ = ARCHIVE_PROGRESS_NONE;
                    self->ApplyExtractionResult(result);
                    self->Layout();
                }
                return 0;
            }
            case ARCHIVE_BROWSER_SELECTION_LOST:
                // A confirmed change of Explorer selection is different from
                // temporary host invisibility during preview initialization.
                // Retire the old content, but retain our dark-painted child:
                // hiding the entire child exposes prevhost's white background.
                if (self->window_ && IsWindow(self->window_) && !self->selectionRetired_) {
                    self->selectionRetired_ = true;
                    KillTimer(hwnd, kChromeRepairTimer);
                    self->SyncGrip(true);
                    if (self->toolbar_) ShowWindow(self->toolbar_, SW_HIDE);
                    for (auto & col : self->cols_)
                        if (col && col->list) ShowWindow(col->list, SW_HIDE);
                    RedrawWindow(hwnd, nullptr, nullptr,
                        RDW_INVALIDATE | RDW_NOERASE);
                }
                return 0;
            case ARCHIVE_BROWSER_SELECTED_COUNT:
                if (self->brokerBoundList_ &&
                    lp == reinterpret_cast < LPARAM > (self->brokerBoundList_)) {
                    self->brokerSelectionCount_ = static_cast < int > (std::min < WPARAM > (wp,40001));
                    self->Layout();
                }
                return 0;
            case ARCHIVE_BROWSER_EXTRACT_ARCHIVE:
                if (self->brokerBrowserReady_ && !self->selectionRetired_ &&
                    self->brokerBoundList_ && lp == reinterpret_cast<LPARAM>(self->brokerBoundList_)) {
                    using Action = ArchivePreviewOptions::ExtractAction;
                    if (wp == static_cast<WPARAM>(Action::Here)) self->ExtractHere();
                    if (wp == static_cast<WPARAM>(Action::To)) self->ExtractTo();
                }
                return 0;
            case ARCHIVE_BROWSER_CHROME_REFRESH:
                // Repair the toolbar twice, once now and once after
                // Explorer finishes its own short-lived parent repaint.
                self->RefreshChrome();
                SetTimer(hwnd,kChromeRepairTimer,240,nullptr);
                return 0;
            case ARCHIVE_BROWSER_DRAG_FINISHED:
                // The drop is complete in the medium-IL broker. Repaint the
                // low-IL toolbar now, and once again after Explorer finishes
                // any parent-window refresh triggered by the dropped files.
                self->RefreshChrome();
                SetTimer(hwnd, kChromeRepairTimer, 240, nullptr);
                return 0;
            case ARCHIVE_BROWSER_READY:
                self->Diagnostic(L"READY received");
                self->diagnosticPending_ = true;
                // The medium-IL browser can be ready before Explorer makes
                // the preview's ancestor visible. Do NOT tear it down then.
                // Only a confirmed selection change or Unload invalidates it.
                if (!self->uiReady_ || self->selectionRetired_ ||
                    self->brokerLoadFailed_ || self->terminalLoadFailure_ ||
                    !self->window_ || !self->brokerBoundList_) return 0;
                if (!self->cols_.empty() && self->cols_.back()->list) {
                    self->brokerBrowserReady_ = true;
                    self->brokerStartupFailureAt_ = 0;
                    // READY only means the broker's HWND exists. It is NOT
                    // proof that the Shell model has enumerated successfully.
                    // A slow first attach can start a fallback enumeration.
                    // It is no longer needed after the broker is ready.
                    if (self->fallbackJob_)
                        self->fallbackJob_->cancelled.store(true, std::memory_order_release);
                    self->fallbackJob_.reset();
                    self->error_.clear();
                    if (!self->extractionPending_) self->SetStatus(L"");
                    self->brokerThemeKnown_ = true;
                    self->brokerThemeDark_ = wp != 0;
                    self->Theme();
                    ShowWindow(self->cols_.back()->list, SW_HIDE);
                    self->Layout();
                }
                return 0;
            case ARCHIVE_BROWSER_DEPTH:
                if (self->brokerBrowserReady_) {
                    self->brokerCanGoBack_ = (wp > 0);
                    if (self->backButton_) EnableWindow(self->backButton_, self->brokerCanGoBack_);
                }
                return 0;
            case ARCHIVE_BROWSER_FAILED:
                self->Diagnostic(L"FAILED received");
                self->diagnosticPending_ = true;
                // A model failure is definitive for this selection. If a
                // recovery page works, keep it and do NOT recreate the broker.
                self->MarkBrokerLoadFailed();
                return 0;
            case ARCHIVE_PROXY_CLICK:
            case ARCHIVE_PROXY_DOUBLECLICK:
            case ARCHIVE_PROXY_RIGHTCLICK: {
                if (self->cols_.empty() || !self->cols_.back()->list) return 0;
                HWND list = self->cols_.back()->list;
                const POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                RECT bounds{}; GetClientRect(list, & bounds);
                if (pt.x < 0 || pt.y < 0 || pt.x >= bounds.right || pt.y >= bounds.bottom) return 0;
                LVHITTESTINFO hit{}; hit.pt = pt;
                const int item = ListView_HitTest(list, & hit);
                const bool keepMulti = msg == ARCHIVE_PROXY_CLICK && item >= 0 &&
                    !(wp & (MK_CONTROL | MK_SHIFT)) &&
                    (ListView_GetItemState(list, item, LVIS_SELECTED) & LVIS_SELECTED);
                const auto old = keepMulti ? self->SelectedIndices() : std::vector < int > {};
                const UINT down = msg == ARCHIVE_PROXY_DOUBLECLICK ? WM_LBUTTONDBLCLK :
                    (msg == ARCHIVE_PROXY_RIGHTCLICK ? WM_RBUTTONDOWN : WM_LBUTTONDOWN);
                const UINT up = msg == ARCHIVE_PROXY_RIGHTCLICK ? WM_RBUTTONUP : WM_LBUTTONUP;
                SendMessageW(list, down, wp | (msg == ARCHIVE_PROXY_RIGHTCLICK ? MK_RBUTTON : MK_LBUTTON), lp);
                if (IsWindow(list)) SendMessageW(list, up, wp, lp);
                // Preserve a multi-selection when beginning a drag on one selected row.
                if (keepMulti && old.size() > 1 && IsWindow(list)) {
                    ListView_SetItemState(list, - 1, 0, LVIS_SELECTED);
                    for (int index : old)
                        ListView_SetItemState(list, index, LVIS_SELECTED, LVIS_SELECTED);
                }
                if (IsWindow(list)) ::SetFocus(list);
                self->SyncGrip();
                return static_cast < LRESULT > (item + 1);
            }
            case ARCHIVE_PROXY_WHEEL:
                if (!self->cols_.empty() && self->cols_.back()->list)
                    return SendMessageW(self->cols_.back()->list, WM_MOUSEWHEEL, wp, lp);
                return 0;
            case WM_SHOWWINDOW:
                // Explorer frequently hides ancestors while creating/reparenting
                // the preview. Visibility changes alone must not retire the
                // browser or reset its cache, selection and native header.
                if (!wp) {
                    KillTimer(hwnd, kChromeRepairTimer);
                } else if (!self->selectionRetired_) {
                    self->SyncGrip();
                    SetTimer(hwnd, kChromeRepairTimer, 120, nullptr);
                }
                return DefWindowProcW(hwnd,msg,wp,lp);
            case WM_SIZE: self->Layout(); return 0;
            case WM_DPICHANGED_AFTERPARENT: self->UpdateFont(); self->Layout(); return 0;
            case WM_THEMECHANGED:
            case WM_SYSCOLORCHANGE:
            case WM_SETTINGCHANGE: self->Theme(); return 0;
            case WM_TIMER:
                if (wp == kChromeRepairTimer) {
                    KillTimer(hwnd, kChromeRepairTimer);
                    if (!self->selectionRetired_ && !self->brokerBrowserReady_ &&
                        !self->terminalLoadFailure_)
                        self->SyncGrip();
                    self->RefreshChrome();
                    return 0;
                }
                if (wp == kOperationStatusTimer) {
                    self->PollExtractionStatus();
                    const bool fallbackVisible = self->extractionPending_ &&
                        !self->brokerBrowserReady_ && self->extractionAcceptedAt_ &&
                        GetTickCount64() - self->extractionAcceptedAt_ >= ArchivePreviewOptions::kExtractionProgressDelayMs &&
                        self->extractionStage_ != ARCHIVE_PROGRESS_PICKING;
                    if (fallbackVisible != self->fallbackProgressVisible_) {
                        self->fallbackProgressVisible_ = fallbackVisible;
                        self->Layout();
                    }
                    // Animate indeterminate progress without blocking UI/IPC.
                    if (self->window_ && !self->extractionPercentKnown_)
                        InvalidateRect(self->window_,nullptr,FALSE);
                    return 0;
                }
                if (wp == kThemePollTimer) {
                    self->UpdateButtonLabels();
                    if (!self->brokerBrowserReady_ && !self->cols_.empty()) {
                        ArchivePreviewColumns::Load();
                        self->ApplyFallbackColumnPreferences();
                    }
                    if (self->statusUntil_ && GetTickCount64() >= self->statusUntil_) {
                        self->SetStatus(L"");
                    }
                    // If the medium-integrity Broker exits, its cross-process
                    // child HWND disappears. Restore the in-process native list
                    // and retry attaching when a new Broker is available.
                    if (self->brokerBrowserReady_ &&
                        !FindWindowExW(hwnd,nullptr,
                            L"ArchivePreview.BrokerListHost.0607",nullptr)) {
                        self->Diagnostic(L"broker child disappeared");
                        self->diagnosticPending_ = true;
                        self->brokerBrowserReady_ = false;
                        self->brokerCanGoBack_ = false;
                        self->EnsureFallbackContent();
                    }
                    if (!self->selectionRetired_ && !self->brokerBrowserReady_ &&
                        !self->terminalLoadFailure_ &&
                        !self->cols_.empty() && !self->cols_.back()->populated &&
                        self->previewStartedAt_ && GetTickCount64() - self->previewStartedAt_ > 6500)
                        self->EnsureFallbackContent();
                    const auto now = ResolvePalette(self->parent_, self->hostBg_, self->hostText_,
                        self->hasHostBg_, self->hasHostText_,
                        (self->brokerThemeKnown_ && self->brokerBrowserReady_) ?
                            (self->brokerThemeDark_ ? 1 : 0) : - 1);
                    if (now.bg != self->palette_.bg || now.fg != self->palette_.fg ||
                        now.dark != self->palette_.dark || now.selected != self->palette_.selected)
                        self->Theme();
                    if (!self->selectionRetired_ && !self->brokerBrowserReady_ &&
                        !self->terminalLoadFailure_)
                        self->SyncGrip();
                    if (self->diagnosticPending_) {
                        self->diagnosticPending_ = false;
                        self->Diagnostic(L"settled");
                    }
                    // Small independent child surface (~38 logical px high).
                    // Recover from host/Explorer invalidations without
                    // recreating the browser or modifying its selection.
                    // Theme() and drag/drop completion already invalidate the
                    // toolbar when necessary. No idle forced repaint here.
                    return 0;
                }
                break;
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORSTATIC: {
                HDC dc = reinterpret_cast < HDC > (wp);
                if (dc) {
                    SetBkColor(dc, self->palette_.panel);
                    ::SetTextColor(dc, self->palette_.fg);
                    SetDCBrushColor(dc, self->palette_.panel);
                }
                return reinterpret_cast < LRESULT > (GetStockObject(DC_BRUSH));
            }
            case WM_NOTIFY: return self->Notify(reinterpret_cast < NMHDR *  > (lp));
            case WM_COMMAND:
                if (HIWORD(wp) == BN_CLICKED) {
                    if (LOWORD(wp) == kBackButtonId) { self->GoBack(); return 0; }
                    if (LOWORD(wp) == kExtractButtonId) { self->ExtractHere(); return 0; }
                    if (LOWORD(wp) == kExtractToButtonId) { self->ExtractTo(); return 0; }
                    if (LOWORD(wp) == kExtractSelectedButtonId) { self->ExtractSelectedHere(); return 0; }
                    if (LOWORD(wp) == kCancelButtonId) { self->CancelExtraction(); return 0; }
                }
                break;
            case WM_DRAWITEM: return self->DrawItem(reinterpret_cast < DRAWITEMSTRUCT *  > (lp));
            case WM_MEASUREITEM: {
                auto * mi = reinterpret_cast < MEASUREITEMSTRUCT *  > (lp);
                if (mi && mi->CtlType == ODT_LISTVIEW)
                    mi->itemHeight = static_cast < UINT > (Scale(hwnd, 31));
                return TRUE;
            }
            case WM_ERASEBKGND: {
                HDC dc = reinterpret_cast < HDC > (wp);
                if (dc) {
                    RECT rc{}; GetClientRect(hwnd, & rc);
                    Fill(dc, rc, self->palette_.bg);
                    // Toolbar is now an independently repainted child HWND.
                }
                return 1;
            }
            case WM_PAINT: self->Paint(); return 0;
            case WM_PRINT:
            case WM_PRINTCLIENT: {
                // Explorer's synchronous snapshot path may skip WM_PAINT.
                // The preview host must always contribute its themed surface.
                HDC dc = reinterpret_cast < HDC > (wp);
                if (dc && (msg == WM_PRINTCLIENT || (lp & PRF_CLIENT))) {
                    RECT r{};GetClientRect(hwnd, & r);
                    Fill(dc,r,self->palette_.bg);
                }
                // Do not swallow the children requested by the
                // Explorer WM_PRINT snapshot. Without forwarding PRF_CHILDREN,
                // the toolbar and its owner-drawn buttons were absent from
                // the snapshot even though the Broker ListView was visible.
                // Do not let DefWindowProc erase our themed client again.
                if (msg == WM_PRINT && (lp & PRF_CHILDREN))
                    return DefWindowProcW(hwnd,msg,wp,
                        lp & ~(static_cast < LPARAM > (PRF_CLIENT) | PRF_ERASEBKGND));
                return 0;
            }
            case WM_NCDESTROY:
                RemovePropW(hwnd,kBackgroundProperty);RemovePropW(hwnd,kForegroundProperty);
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
                return DefWindowProcW(hwnd, msg, wp, lp);
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    static LRESULT CALLBACK ToolbarProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        if (msg == WM_NCCREATE) {
            auto * cs = reinterpret_cast < CREATESTRUCTW *  > (lp);
            SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast < LONG_PTR > (cs->lpCreateParams));
        }
        auto * self = reinterpret_cast < ArchiveHandler *  > (GetWindowLongPtrW(hwnd,GWLP_USERDATA));
        if (!self) return DefWindowProcW(hwnd,msg,wp,lp);
        switch(msg) {
            case WM_ERASEBKGND: {
                HDC dc = reinterpret_cast < HDC > (wp);
                RECT rc{};GetClientRect(hwnd, & rc);
                if(dc) Fill(dc,rc,self->palette_.panel);
                return 1;
            }
            case WM_PAINT: self->PaintToolbar(hwnd); return 0;
            case WM_PRINT:
            case WM_PRINTCLIENT:
                if (wp && (msg == WM_PRINTCLIENT || (lp & PRF_CLIENT)))
                    self->DrawToolbarContent(hwnd,reinterpret_cast < HDC > (wp));
                // Allow Windows to print the owner-drawn button children
                // without asking it to re-erase the dark toolbar surface.
                if (msg == WM_PRINT && (lp & PRF_CHILDREN))
                    return DefWindowProcW(hwnd,msg,wp,
                        lp & ~(static_cast < LPARAM > (PRF_CLIENT) | PRF_ERASEBKGND));
                return 0;
            case WM_SHOWWINDOW:
                if (wp) InvalidateRect(hwnd, nullptr, FALSE);
                return DefWindowProcW(hwnd,msg,wp,lp);
            case WM_DRAWITEM: return self->DrawItem(reinterpret_cast < DRAWITEMSTRUCT *  > (lp));
            case WM_COMMAND: return SendMessageW(self->window_,msg,wp,lp);
            case WM_CTLCOLORBTN:
            case WM_CTLCOLORSTATIC: {
                HDC dc = reinterpret_cast < HDC > (wp);
                if (dc) {
                    SetBkColor(dc,self->palette_.panel);
                    ::SetTextColor(dc,self->palette_.fg);
                    SetDCBrushColor(dc,self->palette_.panel);
                }
                return reinterpret_cast < LRESULT > (GetStockObject(DC_BRUSH));
            }
            case WM_NCDESTROY: SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);break;
        }
        return DefWindowProcW(hwnd,msg,wp,lp);
    }
    void DrawToolbarContent(HWND hwnd, HDC dc) {
        if (!dc) return;
        RECT rc{};GetClientRect(hwnd, & rc);
        Fill(dc,rc,palette_.panel);
        RECT line{rc.left, rc.bottom > 0?rc.bottom - 1:rc.bottom,rc.right,rc.bottom};
        Fill(dc,line,palette_.hairline);
        // Keep navigation controls separate from status and progress. The
        // fallback preview renders both in a bottom strip instead.
    }
    void PaintToolbar(HWND hwnd) {
        // only a solid panel and a one-pixel rule are painted here.
        // GDI backbuffers were pure overhead and could delay first visible
        // dark pixels while Explorer rapidly replaced preview handlers.
        PAINTSTRUCT ps{};
        HDC target = BeginPaint(hwnd, & ps);
        if (target) DrawToolbarContent(hwnd,target);
        EndPaint(hwnd, & ps);
    }
    static int FallbackRowAtPoint(HWND list, POINT point) {
        RECT client{};
        if (!GetClientRect(list, & client) || !PtInRect( & client,point)) return - 1;
        LVHITTESTINFO info{};info.pt = point;
        const int native = ListView_HitTest(list, & info);
        if (native >= 0) return native;
        const int total = ListView_GetItemCount(list);
        const int first = std::max(0,ListView_GetTopIndex(list));
        const int visible = std::max(1,ListView_GetCountPerPage(list));
        for (int row = first;row < total && row < first + visible + 2;++row) {
            RECT bounds{};
            if (ListView_GetItemRect(list,row, & bounds,LVIR_BOUNDS) &&
                point.y >= bounds.top && point.y < bounds.bottom)
                return row;
            if (bounds.top > point.y) break;
        }
        return - 1;
    }
    static LRESULT CALLBACK ColumnHeaderProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,
                                             UINT_PTR id,DWORD_PTR ref) {
        auto * self=reinterpret_cast<ArchiveHandler*>(ref);
        if(self && (msg==WM_RBUTTONUP || msg==WM_CONTEXTMENU)){
            POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(msg==WM_RBUTTONUP)ClientToScreen(hwnd,&point);
            ArchivePreviewColumns::Change change;
            if (ArchivePreviewColumns::VisibilityMenu(hwnd,point,change)) {
                ArchiveSaveFallbackColumn(self->window_,change);
                self->ApplyFallbackColumnPreferences();
            }
            return 0;
        }
        if (self && (msg == WM_CANCELMODE || msg == WM_CAPTURECHANGED))
            PostMessageW(self->window_,WM_SIZE,0,0);
        if(msg==WM_NCDESTROY)RemoveWindowSubclass(hwnd,ColumnHeaderProc,id);
        return DefSubclassProc(hwnd,msg,wp,lp);
    }
    static LRESULT CALLBACK ListProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                    UINT_PTR id, DWORD_PTR ref) {
        auto * self = reinterpret_cast < ArchiveHandler *  > (ref);
        if (self) {
            if (ArchivePreviewOptions::hideHorizontalScrollbar.load() &&
                (msg == WM_HSCROLL || msg == WM_MOUSEHWHEEL)) return 0;
            if (msg == WM_NOTIFY && lp && !self->cols_.empty() && self->cols_.back()->list == hwnd) {
                auto * notice = reinterpret_cast<NMHDR*>(lp);
                if (notice->hwndFrom == ListView_GetHeader(hwnd)) {
                    const auto code = notice->code;
                    auto * track = reinterpret_cast<NMHEADERW*>(lp);
                    const auto & columns = self->cols_.back()->visibleColumns;
                    if (code == HDN_BEGINTRACKW || code == HDN_BEGINTRACKA) {
                        if (track->iItem < 0 || static_cast<size_t>(track->iItem) >= columns.size()) return TRUE;
                        const int width = ListView_GetColumnWidth(hwnd,track->iItem);
                        if (!width || ArchivePreviewColumns::DragWidth(hwnd,self->dpi_,columns[track->iItem],width) < 0)
                            return TRUE;
                        const LRESULT result = DefSubclassProc(hwnd,msg,wp,lp);
                        if (!result) {
                            self->fallbackResizeStartWidth_ = width;
                            self->fallbackResizeColumn_ = track->iItem;
                        }
                        return result;
                    }
                    if ((code == HDN_ITEMCHANGEDW || code == HDN_ITEMCHANGEDA) &&
                        self->fallbackResizeColumn_ >= 0 &&
                        ArchivePreviewOptions::hideHorizontalScrollbar.load()) {
                        const LRESULT result = DefSubclassProc(hwnd,msg,wp,lp);
                        ArchivePreviewColumns::HideScrollbar(hwnd);
                        return result;
                    }
                    if ((code == HDN_ITEMCHANGINGW || code == HDN_ITEMCHANGINGA ||
                         code == HDN_TRACKW || code == HDN_TRACKA ||
                         code == HDN_ENDTRACKW || code == HDN_ENDTRACKA) &&
                        self->fallbackResizeColumn_ >= 0 && track->iItem == self->fallbackResizeColumn_ &&
                        static_cast<size_t>(track->iItem) < columns.size()) {
                        const int kind = columns[track->iItem];
                        const bool end = code == HDN_ENDTRACKW || code == HDN_ENDTRACKA;
                        int width = -1;
                        if (track->pitem && (track->pitem->mask & HDI_WIDTH)) {
                            width = track->pitem->cxy == self->fallbackResizeStartWidth_ ? track->pitem->cxy :
                                ArchivePreviewColumns::DragWidth(hwnd,self->dpi_,kind,track->pitem->cxy);
                            if (width >= 0) track->pitem->cxy = width;
                        }
                        if (!end && width < 0) return TRUE;
                        const LRESULT result = DefSubclassProc(hwnd,msg,wp,lp);
                        if (end) {
                            self->fallbackResizeColumn_ = -1;
                            if (!result && width >= 0 && width != self->fallbackResizeStartWidth_) {
                                const int dips = MulDiv(width,96,self->dpi_ ? self->dpi_ : 96);
                                ArchiveSaveFallbackColumn(self->window_,{static_cast<DWORD>(kind),dips,0});
                            }
                            PostMessageW(self->window_,WM_SIZE,0,0);
                        }
                        return result;
                    }
                }
            }
            if (msg == WM_LBUTTONDOWN) {
                LVHITTESTINFO hit{};
                hit.pt.x = GET_X_LPARAM(lp);
                hit.pt.y = GET_Y_LPARAM(lp);
                const int native = ListView_HitTest(hwnd, & hit);
                self->mouseDownIndex_ = FallbackRowAtPoint(hwnd,hit.pt);
                self->mouseDownCustomHit_ = self->mouseDownIndex_ >= 0 && native < 0;
                self->mouseDown_ = hit.pt;
                // Capture only a selected multi-selection, before native click
                // processing collapses it. Avoid allocating on every click.
                self->mouseDownSelection_.clear();
                if (self->mouseDownIndex_ >= 0 &&
                    (ListView_GetItemState(hwnd,self->mouseDownIndex_,LVIS_SELECTED) & LVIS_SELECTED) &&
                    ListView_GetSelectedCount(hwnd) > 1)
                    self->mouseDownSelection_ = self->SelectedIndices();
                self->dragStarted_ = false;
                if (self->mouseDownCustomHit_) {
                    ::SetFocus(hwnd);
                    const bool ctrl = (wp & MK_CONTROL) != 0;
                    const bool shift = (wp & MK_SHIFT) != 0;
                    const int row = self->mouseDownIndex_;
                    const bool wasSelected = (ListView_GetItemState(hwnd,row,LVIS_SELECTED) & LVIS_SELECTED) != 0;
                    if (shift) {
                        const int focus = ListView_GetNextItem(hwnd, - 1,LVNI_FOCUSED);
                        if (!ctrl) ListView_SetItemState(hwnd, - 1,0,LVIS_SELECTED);
                        const int first = focus >= 0?std::min(focus,row):row;
                        const int last = focus >= 0?std::max(focus,row):row;
                        for (int i = first;i <= last;++i)
                            ListView_SetItemState(hwnd,i,LVIS_SELECTED,LVIS_SELECTED);
                    } else if (ctrl) {
                        ListView_SetItemState(hwnd,row,wasSelected?0:LVIS_SELECTED,LVIS_SELECTED);
                    } else if (!wasSelected || ListView_GetSelectedCount(hwnd) <= 1) {
                        ListView_SetItemState(hwnd, - 1,0,LVIS_SELECTED);
                        ListView_SetItemState(hwnd,row,LVIS_SELECTED,LVIS_SELECTED);
                    }
                    ListView_SetItemState(hwnd,row,LVIS_FOCUSED,LVIS_FOCUSED);
                    SetCapture(hwnd);
                    return 0;
                }
            } else if (msg == WM_MOUSEMOVE && (wp & MK_LBUTTON) != 0 &&
                       self->mouseDownIndex_ >= 0 && !self->dragStarted_ &&
                       !self->dragInProgress_) {
                const POINT current{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
                if (std::abs(current.x - self->mouseDown_.x) >= GetSystemMetrics(SM_CXDRAG) ||
                    std::abs(current.y - self->mouseDown_.y) >= GetSystemMetrics(SM_CYDRAG)) {
                    const int idx = self->mouseDownIndex_;
                    self->dragStarted_ = true;
                    self->mouseDownIndex_ = - 1;
                    self->mouseDownCustomHit_ = false;
                    Wh_Log(L"ArchivePreview: drag initiated by pointer threshold");
                    if (GetCapture() == hwnd) ReleaseCapture();
                    self->DragItem(idx, self->mouseDownSelection_);
                    return 0;
                }
                if (self->mouseDownCustomHit_) return 0;
            } else if (msg == WM_LBUTTONUP || msg == WM_CANCELMODE) {
                const bool custom = self->mouseDownCustomHit_;
                self->mouseDownCustomHit_ = false;
                self->mouseDownIndex_ =  - 1;
                self->dragStarted_ = false;
                self->mouseDownSelection_.clear();
                if (custom) {
                    if (GetCapture() == hwnd) ReleaseCapture();
                    return 0;
                }
            } else if (msg == WM_CAPTURECHANGED && self->mouseDownCustomHit_) {
                self->mouseDownCustomHit_ = false;
                self->mouseDownIndex_ =  - 1;
                self->mouseDownSelection_.clear();
            }
            if (msg == WM_RBUTTONDOWN) {
                POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
                const int row = FallbackRowAtPoint(hwnd,pt);
                if (row >= 0) {
                    ::SetFocus(hwnd);
                    if (!(ListView_GetItemState(hwnd,row,LVIS_SELECTED) & LVIS_SELECTED)) {
                        ListView_SetItemState(hwnd, - 1,0,LVIS_SELECTED);
                        ListView_SetItemState(hwnd,row,LVIS_SELECTED | LVIS_FOCUSED,
                            LVIS_SELECTED | LVIS_FOCUSED);
                    }
                }
                return 0; // Do not let native RMB collapse a multi-selection.
            }
            if (msg == WM_RBUTTONUP) {
                POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
                ClientToScreen(hwnd, & point);
                self->SelectedContextMenu(hwnd,point);
                return 0;
            }
            if (msg == WM_CONTEXTMENU) {
                // Shift+F10 / keyboard Menu key. Mouse is handled on RMB up.
                const POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
                if (point.x ==  - 1 && point.y ==  - 1) self->SelectedContextMenu(hwnd,point);
                return 0;
            }
            if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
                // Recovery ListView: 5,000 entries per page. No permanent
                // truncation, and no extra buttons in the approved UI.
                if ((GetKeyState(VK_CONTROL) & 0x8000) && wp == VK_NEXT) {
                    self->ChangeFallbackPage(1); return 0;
                }
                if ((GetKeyState(VK_CONTROL) & 0x8000) && wp == VK_PRIOR) {
                    self->ChangeFallbackPage( - 1); return 0;
                }
                if (wp == VK_RIGHT || wp == VK_RETURN) { self->EnterFolder(); return 0; }
                if (wp == VK_BACK || (wp == VK_LEFT && (GetKeyState(VK_MENU) & 0x8000))) {
                    self->GoBack(); return 0;
                }
                if (wp == 'C' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                    self->CopyItem(); return 0;
                }
                if (wp == 'G' && (GetKeyState(VK_CONTROL) & 0x8000) &&
                    (GetKeyState(VK_SHIFT) & 0x8000)) {
                    const auto selected = self->SelectedIndices();
                    if (!selected.empty()) self->DragItem(selected.front(), selected);
                    return 0;
                }
                using Action = ArchivePreviewOptions::ExtractAction;
                const auto action = ArchivePreviewOptions::ExtractShortcutForKey(wp);
                if (action != Action::None) {
                    if (!ArchivePreviewOptions::IsRepeatedKey(lp) && !self->selectionRetired_) {
                        switch (action) {
                          case Action::Here: self->ExtractHere(); break;
                          case Action::To: self->ExtractTo(); break;
                          case Action::SelectedHere: self->ExtractSelectedHere(); break;
                          case Action::SelectedTo: self->ExtractSelectedTo(); break;
                          case Action::None: break;
                        }
                    }
                    return 0;
                }
                if (wp == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
                    ListView_SetItemState(hwnd, - 1, LVIS_SELECTED, LVIS_SELECTED);
                    return 0;
                }
            }
        }
        if (msg == WM_NCDESTROY) RemoveWindowSubclass(hwnd, ListProc, id);
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    void UpdateFont() {
        if (!window_) return;
        UINT now = GetDpiForWindow(window_);
        if (!now) now = 96;
        if (dpi_ == now && font_) return;
        // Keep the prior DPI when font creation fails; the next change
        // message must be able to retry instead of keeping a stale font.
        LOGFONTW lf{};
        if (hasHostFont_) lf = hostFont_;
        else { lf.lfHeight = - MulDiv(13, now, 96); wcscpy_s(lf.lfFaceName, L"Segoe UI"); }
        HFONT replacement = CreateFontIndirectW( & lf);
        if (!replacement) return; // Keep the original selected font on failure.
        HFONT old = font_;
        font_ = replacement;
        dpi_ = now;
        for (auto & c : cols_) if (c && c->list && IsWindow(c->list))
            SendMessageW(c->list,WM_SETFONT,reinterpret_cast < WPARAM > (font_),FALSE);
        // cached font is not selected by any controlled ListView now.
        if (old) DeleteObject(old);
        for (auto & c : cols_) if (c && c->list && IsWindow(c->list))
            InvalidateRect(c->list,nullptr,FALSE);
    }
    void UpdateButtonLabels() {
        const int language = ArchivePreviewText::Language();
        if (appliedLanguage_ == language) return;
        appliedLanguage_ = language;
        using TextId = ArchivePreviewText::Id;
        const std::pair < HWND,TextId > buttons[] = {
            {backButton_,TextId::Back},{extractButton_,TextId::ExtractHere},
            {extractToButton_,TextId::ExtractTo},{extractSelectedButton_,TextId::ExtractSelected},
            {cancelButton_,TextId::Cancel}};
        for (const auto & button:buttons) if (button.first && IsWindow(button.first)) {
            SetWindowTextW(button.first,ArchivePreviewText::Get(button.second));
            InvalidateRect(button.first,nullptr,FALSE);
        }
    }
    void Theme() {
        UpdateButtonLabels();
        const Palette next = ResolvePalette(parent_,hostBg_,hostText_,hasHostBg_,hasHostText_,
            (brokerThemeKnown_ && brokerBrowserReady_) ? (brokerThemeDark_ ? 1 : 0) : - 1);
        // Theme messages and broker-ready notifications are frequently repeated
        // after a fast ZIP extraction. Invalidate only if colors really change.
        const bool changed = !themeApplied_ || next.bg != palette_.bg ||
            next.panel != palette_.panel || next.fg != palette_.fg ||
            next.hairline != palette_.hairline || next.dark != palette_.dark ||
            next.highContrast != palette_.highContrast ||
            next.accent != palette_.accent || next.selected != palette_.selected ||
            next.selectedText != palette_.selectedText;
        palette_ = next;
        // Publish scalar colors on our own low-IL window. The broker can read
        // them without cross-process pointers, synchronous messages or relaxing UIPI.
        if(window_){
            SetPropW(window_,kBackgroundProperty,reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(palette_.bg)+1));
            SetPropW(window_,kForegroundProperty,reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(palette_.fg)+1));
        }
        if (!changed) return;
        themeApplied_ = true;
        for (auto & c : cols_) {
            if (!c->list) continue;
            ListView_SetBkColor(c->list, palette_.bg);
            ListView_SetTextBkColor(c->list, palette_.bg);
            ListView_SetTextColor(c->list, palette_.fg);
            SetWindowTheme(c->list, palette_.dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
            InvalidateRect(c->list, nullptr, FALSE);
        }
        if (backButton_) {
            SetWindowTheme(backButton_, L"", L"");
            InvalidateRect(backButton_, nullptr, FALSE);
        }
        if (extractButton_) {
            SetWindowTheme(extractButton_, L"", L"");
            InvalidateRect(extractButton_, nullptr, FALSE);
        }
        if (extractToButton_) {
            SetWindowTheme(extractToButton_, L"", L"");
            InvalidateRect(extractToButton_, nullptr, FALSE);
        }
        if (extractSelectedButton_) {
            SetWindowTheme(extractSelectedButton_,L"",L"");
            InvalidateRect(extractSelectedButton_,nullptr,FALSE);
        }
        if (toolbar_) RedrawWindow(toolbar_,nullptr,nullptr,
            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_NOERASE);
        if (window_) RedrawWindow(window_,nullptr,nullptr,
            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_NOERASE);
    }
    void RefreshChrome() {
        if (!window_ || !toolbar_ || !IsWindow(window_) ||
            !IsWindow(toolbar_) || !IsWindowVisible(window_)) return;
        // Re-evaluate effective system app theme. The broker-owned ListView is
        // intentionally not rebuilt, resized, or reinitialized here.
        const Palette expected = ResolvePalette(parent_,hostBg_,hostText_,
            hasHostBg_,hasHostText_,
            (brokerThemeKnown_ && brokerBrowserReady_) ? (brokerThemeDark_ ? 1 : 0) : - 1);
        if (expected.dark != palette_.dark || expected.bg != palette_.bg ||
            expected.panel != palette_.panel || expected.fg != palette_.fg)
            Theme();
        // Put the independent toolbar above any sibling refreshed by Explorer.
        SetWindowPos(toolbar_,HWND_TOP,0,0,0,0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        // Let the message loop coalesce Explorer's consecutive invalidations.
        // RDW_UPDATENOW/RDW_ERASE used to expose the light parent between
        // the broker ListView and the low-IL preview toolbar.
        RedrawWindow(toolbar_,nullptr,nullptr,
            RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_NOERASE);
    }
    int FallbackNoticeHeight() const {
        if (brokerBrowserReady_ || selectionRetired_) return 0;
        if (extractionPending_ && fallbackProgressVisible_) return Scale(window_,40);
        return !status_.empty() && (!statusUntil_ || GetTickCount64() < statusUntil_)
            ? Scale(window_,32) : 0;
    }
    void SetStatus(const std::wstring & text) {
        const int oldHeight = fallbackNoticeLaidOut_;
        status_ = text;
        const bool error = text.find(L"failed") != std::wstring::npos ||
            text.find(L"Failed") != std::wstring::npos ||
            text.find(L"Could not") != std::wstring::npos ||
            text.find(L"unavailable") != std::wstring::npos ||
            text.find(L"not responding") != std::wstring::npos;
        statusUntil_ = text.empty() || extractionPending_ ? 0 :
            GetTickCount64() + (error ? 8000 : 5000);
        if (window_) {
            if (oldHeight != FallbackNoticeHeight()) Layout();
            InvalidateRect(window_,nullptr,FALSE);
        }
    }
    void ApplyFallbackColumnPreferences() {
        if (cols_.empty()) return;
        auto & column = *cols_.back();
        Layout();
        if (fallbackResizeColumn_ >= 0 || !column.populated || fallbackJob_) return;
        const unsigned wanted = ArchivePreviewOptions::MetadataMask(column.archiveBytes);
        if (wanted == column.failedMetadataMask) return;
        column.failedMetadataMask = ~0u;
        if (wanted != column.metadataMask) QueueFallback(column,SIZE_MAX,true);
    }
    void LayoutFallbackColumns(Column & column) {
        if (fallbackResizeColumn_ >= 0) {
            if (GetCapture() == ListView_GetHeader(column.list)) return;
            fallbackResizeColumn_ = -1;
        }
        const std::vector < int > requested = [&]() {
            std::vector < int > result{0};
            if (ArchivePreviewOptions::ShowColumn(1)) result.push_back(1);
            if (ArchivePreviewOptions::ShowColumn(2)) result.push_back(2);
            if (ArchivePreviewOptions::ShowColumn(3)) result.push_back(3);
            return result;
        }();
        const int language = ArchivePreviewText::Language();
        if (column.visibleColumns != requested || column.appliedLanguage != language) {
            for (int i = 0;i < 4;++i)
                if (!ListView_DeleteColumn(column.list,0)) break;
            using Id = ArchivePreviewText::Id;
            const wchar_t * titles[] = {ArchivePreviewText::Get(Id::ColumnName),
                ArchivePreviewText::Get(Id::ColumnDate),ArchivePreviewText::Get(Id::ColumnType),
                ArchivePreviewText::Get(Id::ColumnSize)};
            for (size_t i = 0;i < requested.size();++i) {
                LVCOLUMNW value{};
                value.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
                value.pszText = const_cast < LPWSTR > (titles[requested[i]]);
                value.cx = 0;
                value.fmt = requested[i] == 3?LVCFMT_RIGHT:LVCFMT_LEFT;
                ListView_InsertColumn(column.list,static_cast < int > (i),&value);
            }
            column.visibleColumns = requested;
            column.appliedLanguage = language;
            InvalidateRect(column.list,nullptr,FALSE);
        }
        ArchivePreviewColumns::Layout(column.list,dpi_,requested);
        HWND header = ListView_GetHeader(column.list);
        for (size_t i=0;header && i<requested.size();++i) {
            HDITEMW item{};item.mask = HDI_FORMAT;
            if (!Header_GetItem(header,static_cast<int>(i),&item)) continue;
            item.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN | HDF_RTLREADING);
            if (ArchivePreviewText::TextDirection()) item.fmt |= HDF_RTLREADING;
            if (requested[i] == column.sortColumn)
                item.fmt |= column.sortAscending?HDF_SORTUP:HDF_SORTDOWN;
            Header_SetItem(header,static_cast<int>(i),&item);
        }
    }
    void Layout() {
        if (!window_) return;
        RECT rc{};
        GetClientRect(window_, & rc);
        const int width = std::max(1, static_cast < int > (rc.right - rc.left));
        const int height = std::max(1, static_cast < int > (rc.bottom - rc.top));
        const bool fallbackProgress = extractionPending_ && fallbackProgressVisible_ &&
            !brokerBrowserReady_;
        const int toolbar = Scale(window_,38);
        const int noticeHeight = FallbackNoticeHeight();
        fallbackNoticeLaidOut_ = noticeHeight;
        const int pad = Scale(window_, 7);
        const int buttonHeight = Scale(window_, 26);
        const int y = std::max(0, (Scale(window_,38) - buttonHeight) / 2);
        // Browser selection/count messages can call Layout repeatedly
        // without a geometry change. Do not reposition unchanged toolbar and
        // owner-draw buttons: Windows would unnecessarily invalidate their
        // chrome while a fast ZIP extraction completes.
        auto placeChild = [ & ](HWND child,int x,int y,int w,int h,UINT extra = 0) {
            if (!child || !IsWindow(child)) return;
            RECT old{};
            const bool measured = GetWindowRect(child, & old) != 0;
            if (measured) {
                HWND p = GetParent(child);
                if (p) MapWindowPoints(nullptr,p,reinterpret_cast < POINT *  > ( & old),2);
            }
            const bool same = measured && old.left == x && old.top == y &&
                old.right - old.left == w && old.bottom - old.top == h;
            const bool visibleStyle = (GetWindowLongPtrW(child,GWL_STYLE) & WS_VISIBLE) != 0;
            const bool visibilityMatches = !(extra & SWP_SHOWWINDOW) || visibleStyle;
            if (same && visibilityMatches) return;
            SetWindowPos(child,nullptr,x,y,w,h,
                SWP_NOACTIVATE | SWP_NOZORDER | extra);
        };
        // A dedicated child window owns the toolbar paint, preventing white strips.
        placeChild(toolbar_,0,0,width,toolbar);
        const int gap = Scale(window_, 5);
        // Keep Back + the two whole-archive controls usable at narrow widths.
        // The extra selected control is shown when enough room exists; the
        // keyboard shortcut and right-click menu remain available otherwise.
        const bool selectedFits = width >= Scale(window_,315);
        const bool selectedCompact = width < Scale(window_,410);
        const int gaps = selectedFits?3:2;
        const int available = std::max(3,width - 2 * pad - gaps * gap);
        const int selectedWidth = selectedFits?Scale(window_,selectedCompact?75:111):0;
        const int primaryWidth = std::max(3,available - selectedWidth);
        const int backWidth = std::min(Scale(window_,32),primaryWidth * 23 / 100);
        const int toWidth = std::min(Scale(window_,88),primaryWidth * 35 / 100);
        const int hereWidth = std::min(Scale(window_,101),
            std::max(1,primaryWidth - backWidth - toWidth));
        // Navigation and extraction actions live in the separate Preview Host toolbar.
        if (backButton_) {
            placeChild(backButton_,pad,y,backWidth,buttonHeight);
            EnableWindow(backButton_, !commandJob_ && (brokerBrowserReady_ ? brokerCanGoBack_ : cols_.size() > 1));
        }
        const int selectedX = width - pad - selectedWidth;
        if (extractSelectedButton_) {
            if (selectedFits) {
                placeChild(extractSelectedButton_,selectedX,y,
                    selectedWidth,buttonHeight,SWP_SHOWWINDOW);
                const bool hasSelection = brokerBrowserReady_ ? brokerSelectionCount_ > 0 :
                    (!cols_.empty() && cols_.back()->list &&
                     ListView_GetSelectedCount(cols_.back()->list) > 0);
                EnableWindow(extractSelectedButton_,hasSelection && !extractionPending_ && !commandJob_);
            } else if (GetWindowLongPtrW(extractSelectedButton_,GWL_STYLE) & WS_VISIBLE)
                ShowWindow(extractSelectedButton_,SW_HIDE);
        }
        const int toX = selectedFits?selectedX - gap - toWidth:width - pad - toWidth;
        placeChild(extractToButton_,toX,y,toWidth,buttonHeight);
        placeChild(extractButton_,toX - gap - hereWidth,y,hereWidth,buttonHeight);
        if (extractButton_) EnableWindow(extractButton_, !extractionPending_ && !commandJob_);
        if (extractToButton_) EnableWindow(extractToButton_, !extractionPending_ && !commandJob_);
        if (cancelButton_) {
            const int cancelWidth = Scale(window_,67);
            const int cancelHeight = Scale(window_,19);
            placeChild(cancelButton_,std::max(0,width - pad - cancelWidth),
                std::max(0,height - noticeHeight + Scale(window_,6)),cancelWidth,cancelHeight);
            const bool cancelVisible = (GetWindowLongPtrW(cancelButton_,GWL_STYLE) & WS_VISIBLE) != 0;
            if (cancelVisible != fallbackProgress)
                ShowWindow(cancelButton_,fallbackProgress?SW_SHOWNA:SW_HIDE);
            EnableWindow(cancelButton_,fallbackProgress && !cancelRequested_ &&
                extractionStage_ != ARCHIVE_PROGRESS_EXTERNAL);
        }
        if (cols_.empty() || !cols_.back()->list) return;
        HWND list = cols_.back()->list;
        placeChild(list,0,toolbar,width,std::max(1,height - toolbar - noticeHeight));
        LayoutFallbackColumns(*cols_.back());
        // The parent has no newly exposed pixels if the layout is identical.
        // Repaints are requested by theme/status/child windows themselves.
    }
    void Paint() {
        PAINTSTRUCT ps{};
        HDC target = BeginPaint(window_, & ps);
        RECT rc{}; GetClientRect(window_, & rc);
        // Preserve double-buffering, but allocate the bitmap only for the
        // invalidated region rather than the whole Preview Pane on each paint.
        const RECT dirty = ps.rcPaint;
        const int w = std::max(0,static_cast < int > (dirty.right - dirty.left));
        const int h = std::max(0,static_cast < int > (dirty.bottom - dirty.top));
        HDC memory = (target && w && h) ? CreateCompatibleDC(target) : nullptr;
        HBITMAP bitmap = memory ? CreateCompatibleBitmap(target,w,h) : nullptr;
        HGDIOBJ oldBitmap = bitmap ? SelectObject(memory,bitmap) : nullptr;
        HDC dc = bitmap ? memory : target;
        if (bitmap) SetViewportOrgEx(memory, - dirty.left, - dirty.top,nullptr);
        Fill(dc,bitmap ? rc : dirty,palette_.bg);
        const int bottom = std::min(static_cast < int > (rc.bottom),Scale(window_,38));
        // Fallback mode has no Broker footer. Put messages/progress at the
        // bottom, never between Back and Extract here.
        const int noticeHeight = FallbackNoticeHeight();
        if (noticeHeight > 0) {
            RECT strip{rc.left,std::max(rc.top,rc.bottom - noticeHeight),rc.right,rc.bottom};
            Fill(dc,strip,palette_.panel);
            RECT rule{strip.left,strip.top,strip.right,std::min(strip.bottom,strip.top + 1)};
            Fill(dc,rule,palette_.hairline);
            const bool progress = extractionPending_ && fallbackProgressVisible_;
            const bool reserveCancel = progress && cancelButton_ && IsWindowVisible(cancelButton_);
            RECT label{strip.left + Scale(window_,11),strip.top + Scale(window_,1),
                std::max(strip.left + Scale(window_,20),strip.right - Scale(window_,reserveCancel?82:12)),
                progress?strip.bottom - Scale(window_,8):strip.bottom};
            const std::wstring message = status_.empty() ?
                ArchivePreviewText::Get(ArchivePreviewText::Id::Extracting) : status_;
            Label(dc,label,message,font_,palette_.fg,
                DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | ArchivePreviewText::TextDirection());
            if (progress) {
                RECT track{strip.left + Scale(window_,9),strip.bottom - Scale(window_,7),
                    strip.right - Scale(window_,9),strip.bottom - Scale(window_,4)};
                Fill(dc,track,palette_.hairline);
                RECT segment = track;
                if (extractionStage_ == ARCHIVE_PROGRESS_EXTRACTING && extractionPercentKnown_) {
                    segment.right = segment.left + MulDiv(track.right - track.left,
                        static_cast < int > (std::min < DWORD > (extractionPercent_,100)),100);
                } else {
                    const int span = std::max(1,static_cast < int > (track.right - track.left) / 5);
                    const int travel = (track.right - track.left) + span;
                    segment.left = track.left + static_cast < int > ((GetTickCount64() % 1800) * travel / 1800) - span;
                    segment.right = segment.left + span;
                    segment.left = std::max(segment.left,track.left);
                    segment.right = std::min(segment.right,track.right);
                }
                if (segment.right > segment.left) Fill(dc,segment,palette_.accent);
            }
        }
        // The toolbar has its own child window and paints independently.
        if (!error_.empty()) {
            RECT area{Scale(window_,12),bottom + Scale(window_,12),
                      rc.right - Scale(window_,12),rc.bottom - Scale(window_,12)};
            Label(dc,area,error_,font_,palette_.muted,DT_WORDBREAK);
        }
        if (bitmap) {
            SetViewportOrgEx(memory,0,0,nullptr);
            BitBlt(target,dirty.left,dirty.top,w,h,memory,0,0,SRCCOPY);
            SelectObject(memory,oldBitmap);
            DeleteObject(bitmap);
        }
        if (memory) DeleteDC(memory);
        EndPaint(window_, & ps);
    }
    LRESULT DrawItem(DRAWITEMSTRUCT * di) {
        if (!di) return FALSE;
        if (di->CtlType == ODT_BUTTON &&
            (di->CtlID == kBackButtonId || di->CtlID == kExtractButtonId ||
             di->CtlID == kExtractToButtonId || di->CtlID == kExtractSelectedButtonId ||
             di->CtlID == kCancelButtonId)) {
            RECT buttonRect{};
            GetClientRect(di->hwndItem, & buttonRect);
            const bool compactSelected = (di->CtlID == kExtractSelectedButtonId &&
                buttonRect.right - buttonRect.left < Scale(window_,95));
            using TextId = ArchivePreviewText::Id;
            const TextId textId = di->CtlID == kBackButtonId?TextId::Back:
                (di->CtlID == kExtractButtonId?TextId::ExtractHere:
                (di->CtlID == kCancelButtonId?TextId::Cancel:
                (di->CtlID == kExtractSelectedButtonId?
                    (compactSelected?TextId::Selected:TextId::ExtractSelected):TextId::ExtractTo)));
            PCWSTR title = ArchivePreviewText::Get(textId);
            ArchivePreviewButtons::Paint(di->hDC,di->hwndItem,di->rcItem,
                di->itemState,font_,title,palette_.panel,palette_.dark,
                palette_.highContrast,GetDpiForWindow(di->hwndItem),
                di->CtlID == kBackButtonId);
            return TRUE;
        }
        if (di->CtlType != ODT_LISTVIEW || cols_.empty() ||
            di->hwndItem != cols_.back()->list) return FALSE;
        Column & col = * cols_.back();
        if (di->itemID >= col.entries.size()) return TRUE;
        const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
        if (diagnostics) ++diagnosticFallbackPaints_;
        const Entry & e = col.entries[di->itemID];
        RECT r = di->rcItem;
        r.left = 0;
        r.right = ArchivePreviewColumns::ViewportWidth(col.list);
        const int origin = -ArchivePreviewColumns::HorizontalOffset(col.list);
        // Selection comes from the current ListView; Column no longer stores
        // any selection state after the UI was reduced to one visible list.
        const bool chosen = (di->itemState & ODS_SELECTED) != 0 ||
            (ListView_GetItemState(col.list, static_cast < int > (di->itemID), LVIS_SELECTED) & LVIS_SELECTED) != 0;
        Fill(di->hDC, r, chosen ? palette_.selected : palette_.bg);
        if (chosen) {
            RECT stripe{r.left, r.top, r.left + Scale(window_, 3), r.bottom};
            Fill(di->hDC, stripe, palette_.accent);
        }
        const int ix = origin + Scale(window_,7);
        const int iy = r.top + (r.bottom - r.top - Scale(window_, 16)) / 2;
        if (e.iconImage || (icons_ && e.iconIndex >= 0)) {
            // Use the icon alpha/mask, not ImageList_Draw's default background.
            // This avoids bright tiles around icons in dark Explorer previews.
            HICON icon = e.iconImage?e.iconImage->handle:
                ImageList_GetIcon(icons_,e.iconIndex,ILD_TRANSPARENT);
            if (icon) {
                const BOOL drawn = DrawIconEx(di->hDC, ix, iy, icon, Scale(window_, 16),
                           Scale(window_, 16), 0, nullptr, DI_NORMAL);
                if (diagnostics && drawn) ++diagnosticFallbackIcons_;
                else if (diagnostics) ++diagnosticFallbackMisses_;
                if (!e.iconImage) DestroyIcon(icon);
            } else if (diagnostics) ++diagnosticFallbackMisses_;
        } else if (diagnostics) ++diagnosticFallbackMisses_;
        const int nameWidth = ListView_GetColumnWidth(col.list,0);
        RECT name{ix + Scale(window_,23),r.top,
            origin + nameWidth - Scale(window_,8),r.bottom};
        if (name.right > name.left)
            Label(di->hDC,name,e.name,font_,chosen?palette_.selectedText:palette_.fg,
                DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_LEFT);
        int columnLeft = origin + nameWidth;
        for (size_t i = 1;i < col.visibleColumns.size();++i) {
            const int kind = col.visibleColumns[i];
            const int width = ListView_GetColumnWidth(col.list,static_cast < int > (i));
            const std::wstring text = kind == 1?e.modifiedLabel:
                (kind == 2?e.kind:(e.hasSize?HumanSize(e.bytes):L""));
            RECT cell{columnLeft + Scale(window_,6),r.top,
                columnLeft + width - Scale(window_,10),r.bottom};
            if (cell.right > cell.left)
                Label(di->hDC,cell,text,font_,chosen?palette_.selectedText:palette_.fg,
                    DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER |
                    (kind == 3?DT_RIGHT:DT_LEFT));
            columnLeft += width;
        }
        if ((di->itemState & ODS_FOCUS) != 0) {
            RECT focus = r;
            InflateRect( & focus, - 1, - 1);
            DrawFocusRect(di->hDC, & focus);
        }
        return TRUE;
    }
    static void OrderFallbackRows(Column & column) {
        ArchivePreviewSort::Rows(column.entries,column.sortColumn,column.sortAscending,
            [](const Entry & e)->ArchivePreviewSort::Values {
                return {e.folder,e.name,e.kind,e.bytes,e.hasSize,e.modifiedValue};
            });
    }
    void SortFallbackRows(Column & column,int kind) {
        std::unordered_set<std::wstring> selected;
        std::wstring focused;
        const int focus = ListView_GetNextItem(column.list,-1,LVNI_FOCUSED);
        if (focus >= 0 && static_cast<size_t>(focus) < column.entries.size()) focused = column.entries[focus].name;
        for (int i=ListView_GetNextItem(column.list,-1,LVNI_SELECTED);i>=0;
            i=ListView_GetNextItem(column.list,i,LVNI_SELECTED))
            if (static_cast<size_t>(i)<column.entries.size()) selected.insert(column.entries[i].name);
        if (column.sortColumn == kind) column.sortAscending = !column.sortAscending;
        else {column.sortColumn = kind;column.sortAscending = true;}
        OrderFallbackRows(column);
        SendMessageW(column.list,WM_SETREDRAW,FALSE,0);
        ListView_DeleteAllItems(column.list);
        for (size_t i=0;i<column.entries.size();++i) {
            LVITEMW row{};row.mask = LVIF_TEXT | LVIF_PARAM;
            row.iItem = static_cast<int>(i);row.pszText = const_cast<LPWSTR>(column.entries[i].name.c_str());row.lParam = i;
            ListView_InsertItem(column.list,&row);
            UINT state = selected.count(column.entries[i].name)?LVIS_SELECTED:0;
            if (column.entries[i].name == focused) state |= LVIS_FOCUSED;
            if (state) ListView_SetItemState(column.list,static_cast<int>(i),state,LVIS_SELECTED | LVIS_FOCUSED);
        }
        SendMessageW(column.list,WM_SETREDRAW,TRUE,0);
        InvalidateRect(column.list,nullptr,FALSE);Layout();
    }
    LRESULT Notify(NMHDR * n) {
        if(!n || cols_.empty())return 0;
        auto & activeColumn=*cols_.back();
        if(n->hwndFrom!=activeColumn.list)return 0;
        if (n->code == LVN_COLUMNCLICK && !brokerBrowserReady_ && cols_.back()->populated) {
            const auto * click = reinterpret_cast<const NMLISTVIEW*>(n);
            auto & column = *cols_.back();
            if (click->iSubItem >= 0 && static_cast<size_t>(click->iSubItem) < column.visibleColumns.size())
                SortFallbackRows(column,column.visibleColumns[click->iSubItem]);
            return 0;
        }
        if (n->code == LVN_MARQUEEBEGIN)
            return mouseDownIndex_ >= 0 ? TRUE : FALSE;
        if (n->code == NM_DBLCLK) EnterFolder();
        else if (n->code == LVN_ITEMCHANGED) {
            const auto * change = reinterpret_cast < const NMLISTVIEW *  > (n);
            if (change && (change->uChanged & LVIF_STATE) &&
                ((change->uOldState ^ change->uNewState) & LVIS_SELECTED))
                {
                    if (extractSelectedButton_ && !brokerBrowserReady_)
                        EnableWindow(extractSelectedButton_,
                            ListView_GetSelectedCount(cols_.back()->list) > 0 && !extractionPending_ && !commandJob_);
                }
        }
        else if (n->code == LVN_BEGINDRAG && !dragStarted_ && !dragInProgress_) {
            const auto * item = reinterpret_cast < const NMLISTVIEW *  > (n);
            if (item && item->iItem >= 0) {
                dragStarted_ = true;
                mouseDownIndex_ = - 1;
                Wh_Log(L"ArchivePreview: drag initiated by list view");
                if (GetCapture() == n->hwndFrom) ReleaseCapture();
                DragItem(item->iItem, mouseDownSelection_);
            }
        }
        return 0;
    }
    void EnterFolder() {
        if (cols_.empty()) return;
        Column & c = * cols_.back();
        int idx = ListView_GetNextItem(c.list, - 1, LVNI_SELECTED);
        Wh_Log(L"[AP diag/host] EnterFolder pid=%lu self=%p row=%d rows=%zu isFolder=%d",
            GetCurrentProcessId(),this,idx,c.entries.size(),
            idx >= 0 && static_cast<size_t>(idx) < c.entries.size() && c.entries[idx].folder?1:0);
        if (idx < 0 || idx >= static_cast < int > (c.entries.size()) ||
            !c.entries[idx].folder || cols_.size() >= kMaxDepth) return;
        PIDLIST_ABSOLUTE id = c.entries[idx].id;
        std::wstring name = c.entries[idx].name;
        if (SUCCEEDED(AddColumn(id, std::move(name)))) {
            Layout();
            SyncGrip();
            SetStatus(L"");
            ::SetFocus(cols_.back()->list);
        } else {
            Wh_Log(L"ArchivePreview: Shell provider could not open folder");
        }
    }
    void GoBack() {
        Diagnostic(L"Back requested");
        if (commandJob_) return;
        if (brokerBrowserReady_ && brokerBoundList_) {
            if (!QueueCommand(ARCHIVE_BROKER_BROWSER_BACK,
                {std::to_wstring(static_cast<unsigned long long>(
                    reinterpret_cast<uintptr_t>(brokerBoundList_)))},
                L"Going back...")) SetStatus(L"Cannot queue navigation.");
            return;
        }
        if (cols_.size() <= 1) return;
        auto & child = cols_.back();
        if (child->list) DestroyWindow(child->list);
        cols_.pop_back();
        ShowWindow(cols_.back()->list, SW_SHOWNA);
        SyncGrip();
        ListView_SetItemState(cols_.back()->list, - 1, 0, LVIS_SELECTED | LVIS_FOCUSED);
        Layout();
        SetStatus(L"");
        ::SetFocus(cols_.back()->list);
    }
    std::vector < int > SelectedIndices() const {
        std::vector < int > selected;
        if (cols_.empty() || !cols_.back()->list) return selected;
        HWND list = cols_.back()->list;
        for (int i = - 1; (i = ListView_GetNextItem(list, i, LVNI_SELECTED)) >= 0; )
            selected.push_back(i);
        return selected;
    }
    // Transfers are delegated to the normal-integrity Broker.
    // The fallback does not stage files within the low-integrity Preview Host.
    void ApplyExtractionResult(HRESULT outcome) {
        if (extractButton_) EnableWindow(extractButton_,TRUE);
        if (extractToButton_) EnableWindow(extractToButton_,TRUE);
        Layout();
        if (SUCCEEDED(outcome)) SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractionComplete));
        else if (outcome == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) &&
            ArchivePreviewOptions::ExtractionSnapshot().backend != 0)
            SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractorMissing));
        else if (outcome == HRESULT_FROM_WIN32(ERROR_CANCELLED))
            SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractionCancelled));
        else if (outcome == HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY))
            SetStatus(L"Extraction incomplete. Some files remain in the destination.");
        else {
            wchar_t status[160]{};
            swprintf_s(status,L"Extraction failed (0x%08lX). Check Windhawk logs.",
                static_cast < unsigned long > (outcome));
            SetStatus(status);
        }
        // Do not call Theme() and force an immediate toolbar repaint back to
        // back: the extraction completion also invalidates the broker header.
        // Both windows already own an opaque dark-aware paint surface.
        if (window_ && IsWindow(window_)) {
            // Explorer may still be repainting its folder after extraction.
            // One deferred correction is cheaper and avoids an intermediate
            // exposed parent frame between two toolbar paints.
            SetTimer(window_,kChromeRepairTimer,240,nullptr);
        }
    }
    void TrackExtraction(const GUID & id,const wchar_t * initial) {
        pendingExtraction_ = id;
        extractionPending_ = true;
        extractionAcceptedAt_ = GetTickCount64();
        fallbackProgressVisible_ = false;
        cancelRequested_ = false;
        if (cancelButton_) ShowWindow(cancelButton_,SW_HIDE);
        if (extractButton_) EnableWindow(extractButton_,FALSE);
        if (extractToButton_) EnableWindow(extractToButton_,FALSE);
        if (extractSelectedButton_) EnableWindow(extractSelectedButton_,FALSE);
        extractionStage_ = ARCHIVE_PROGRESS_NONE;
        extractionPercent_ = 0;
        extractionPercentKnown_ = false;
        statusPollSerial_ = g_nextPollSerial.fetch_add(1); // unique across handler instances
        statusPollRunning_ = false;
        statusPollFailures_ = 0;
        SetStatus(initial);
        Layout();
        if (window_) SetTimer(window_,kOperationStatusTimer,500,nullptr);
    }
    void CancelExtraction() {
        if (!extractionPending_ || cancelRequested_ || !window_) return;
        if (extractionStage_ == ARCHIVE_PROGRESS_EXTERNAL) {
            SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::CancelInExternal));
            return;
        }
        auto * job = new(std::nothrow) OperationCancelJob;
        if (!job)return;
        job->hwnd = window_;
        job->requestId = pendingExtraction_;
        job->archive = archive_;
        job->serial = statusPollSerial_;
        HANDLE thread = ArchivePreviewWorkerLifetime::Start(OperationCancelWorker,job);
        if (!thread) {delete job;return;}
        cancelRequested_ = true;
        if (cancelButton_) EnableWindow(cancelButton_,FALSE);
        SetStatus(ArchivePreviewText::Get(ArchivePreviewText::Id::Cancelling));
    }
    void PollExtractionStatus() {
        if (!extractionPending_ || !window_ || statusPollRunning_) return;
        // Track the worker until it terminates. Shutdown cancels pipe I/O
        // and waits for the callback before Windhawk unloads the DLL.
        auto * job = new(std::nothrow) OperationPollJob;
        if (!job)return;
        job->hwnd = window_;
        job->requestId = pendingExtraction_;
        job->serial = statusPollSerial_;
        HANDLE thread = ArchivePreviewWorkerLifetime::Start(OperationPollWorker,job);
        if (!thread) { delete job; return; }
        statusPollRunning_ = true;
    }
    void ExtractSelectedTo() { ExtractSelected(false); }
    void ExtractSelectedHere() { ExtractSelected(true); }
    void ExtractSelected(bool here) {
        if (extractionPending_ || commandJob_) return;
        if (brokerBrowserReady_ && brokerBoundList_) {
            const DWORD cmd = here ? ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE :
                ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO;
            if (!QueueCommand(cmd, {std::to_wstring(static_cast<unsigned long long>(
                    reinterpret_cast<uintptr_t>(brokerBoundList_)))},
                    L"Sending selected extraction..."))
                SetStatus(L"Could not queue selected extraction.");
            return;
        }
        const auto selected = SelectedIndices();
        if (selected.empty()) { SetStatus(L"Select one or more files first."); return; }
        if (selected.size() > ARCHIVE_BROKER_MAX_PATHS) {
            SetStatus(L"Too many selected items; limit is 4,096.");
            return;
        }
        const DWORD cmd = here ? ARCHIVE_BROKER_EXTRACT_SELECTED_HERE :
            ARCHIVE_BROKER_EXTRACT_SELECTED_TO;
        if (!QueueCommand(cmd, SelectedArchivePaths(selected),
                          L"Starting selected extraction..."))
            SetStatus(L"Could not queue selected extraction.");
    }
    void SelectedContextMenu(HWND list, POINT screenPoint) {
        if (!list || cols_.empty() || !window_) return;
        POINT local = screenPoint;
        if (screenPoint.x ==  - 1 && screenPoint.y ==  - 1) {
            int row = ListView_GetNextItem(list, - 1,LVNI_FOCUSED);
            if (row < 0) row = ListView_GetNextItem(list, - 1,LVNI_SELECTED);
            if (row < 0) return;
            RECT item{};
            if (!ListView_GetItemRect(list,row, & item,LVIR_BOUNDS)) return;
            local = {item.left + 16,item.top + 16};
            screenPoint = local;
            ClientToScreen(list, & screenPoint);
        } else ScreenToClient(list, & local);
        const int row = FallbackRowAtPoint(list,local);
        if (row < 0 || row >= ListView_GetItemCount(list)) return;
        const bool selected = (ListView_GetItemState(list,row,LVIS_SELECTED) & LVIS_SELECTED) != 0;
        if (!selected) {
            ListView_SetItemState(list, - 1,0,LVIS_SELECTED);
            ListView_SetItemState(list,row,LVIS_SELECTED | LVIS_FOCUSED,
                LVIS_SELECTED | LVIS_FOCUSED);
        }
        HMENU menu = CreatePopupMenu();
        if (!menu) return;
        AppendMenuW(menu,MF_STRING,kSelectedMenuHere,ArchivePreviewText::Get(ArchivePreviewText::Id::SelectedHere));
        AppendMenuW(menu,MF_STRING,kSelectedMenuTo,ArchivePreviewText::Get(ArchivePreviewText::Id::SelectedTo));

        const UINT choice = TrackPopupMenu(menu,TPM_RETURNCMD | TPM_NONOTIFY |
            TPM_RIGHTBUTTON | ArchivePreviewText::MenuDirection(),screenPoint.x,screenPoint.y,0,window_,nullptr);
        DestroyMenu(menu);
        if (choice == kSelectedMenuHere) ExtractSelectedHere();
        else if (choice == kSelectedMenuTo) ExtractSelectedTo();
    }
    void CopyItem() {
        if (commandJob_) return;
        const auto indices = SelectedIndices();
        if (indices.empty()) { SetStatus(L"Select one or more files first."); return; }
        if (indices.size() > ARCHIVE_BROKER_MAX_PATHS) {
            SetStatus(L"Too many selected items; limit is 4,096.");
            return;
        }
        if (!QueueCommand(ARCHIVE_BROKER_COPY_SELECTION,
                          SelectedArchivePaths(indices), L"Copying selection..."))
            SetStatus(L"Could not queue copy operation.");
    }
    std::vector < std::wstring > SelectedArchivePaths(const std::vector < int >  & indices) {
        std::vector < std::wstring > result;
        if (cols_.empty()) return result;
        const Column & column = * cols_.back();
        for (int idx : indices) {
            if (idx < 0 || idx >= static_cast < int > (column.entries.size())) continue;
            std::wstring name;
            for (size_t level = 1; level < cols_.size(); ++level) {
                if (!name.empty()) name += L"\\";
                name += cols_[level]->heading;
            }
            if (!name.empty()) name += L"\\";
            name += column.entries[idx].name;
            result.emplace_back(std::move(name));
        }
        return result;
    }
    // Attach/detach runs on a tracked IPC worker so the preview UI does not
    // block on the broker. The broker still owns the native transfer ListView.
    void Diagnostic(PCWSTR reason) {
        if (!ArchivePreviewDiagnostics::Enabled()) return;
        HWND list = cols_.empty()?nullptr:cols_.back()->list;
        HWND broker = window_?FindWindowExW(window_,nullptr,
            L"ArchivePreview.BrokerListHost.0607",nullptr):nullptr;
        Wh_Log(L"[AP diag/host] %s pid=%lu self=%p window=%p anchor=%p bound=%p "
               L"brokerChild=%p ready=%d failed=%d terminal=%d retired=%d back=%d "
               L"history=%zu rows=%zu listVisible=%d paints=%u icons=%u misses=%u dpi=%u",
            reason,GetCurrentProcessId(),this,window_,list,brokerBoundList_,broker,
            brokerBrowserReady_?1:0,brokerLoadFailed_?1:0,terminalLoadFailure_?1:0,
            selectionRetired_?1:0,brokerCanGoBack_?1:0,cols_.size(),
            cols_.empty()?size_t(0):cols_.back()->entries.size(),
            list && IsWindowVisible(list)?1:0,diagnosticFallbackPaints_,
            diagnosticFallbackIcons_,diagnosticFallbackMisses_,dpi_);
    }
    void SyncGrip(bool clear = false) {
        if ((terminalLoadFailure_ || brokerLoadFailed_) && !clear) return;
        HWND current = (!clear && !cols_.empty()) ? cols_.back()->list : nullptr;
        if (!window_ || archive_.empty() ||
            (archive_.find_first_of(L"\\/:") == std::wstring::npos)) current = nullptr;
        if (selectionRetired_) current = nullptr;
        if (clear || current != brokerBoundList_) {
            brokerBrowserReady_ = false;
            brokerCanGoBack_ = false;
            brokerBoundList_ = current;
        }
        if (current && (!parent_ || !IsWindow(parent_))) return;
        if (!current && !gripState_) return;
        if (current && brokerBrowserReady_) return;
        const ULONGLONG now = GetTickCount64();
        if (current && now - brokerAttachAttempt_ < 2000) return;
        if (current) brokerAttachAttempt_ = now;
        if (!gripState_) gripState_ = std::make_shared<GripState>();
        auto state = gripState_;
        bool start = false;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            // A timer must not invalidate an in-flight worker's serial for
            // the exact same request. Otherwise the UI discards a real failure
            // as stale and can keep issuing GRIP_UPDATE indefinitely.
            if (state->running && state->desired == current &&
                state->desiredArchive == archive_ && state->hwnd == window_)
                return;
            state->desired = current;
            state->desiredArchive = archive_;
            state->hwnd = window_;
            state->serial = g_nextPollSerial.fetch_add(1);
            if (current) {
                std::lock_guard<std::mutex> ownerLock(g_gripOwnerMutex);
                g_gripOwners[current] = state->ownerToken;
            }
            // A pipe acknowledgement is not proof that the broker created its
            // cross-process HWND. Resend after the 2s retry interval until the
            // UI receives ARCHIVE_BROWSER_READY.
            if (current && !brokerBrowserReady_ && !state->running &&
                state->applied == current && state->appliedArchive == archive_) {
                state->applied = nullptr;
                state->appliedArchive.clear();
            }
            if (!state->running) {
                state->running = true;
                start = true;
            }
        }
        if (!start) return;
        auto* holder = new(std::nothrow) std::shared_ptr<GripState>(state);
        if (!holder || !ArchivePreviewWorkerLifetime::Start(GripWorker, holder)) {
            delete holder;
            std::lock_guard<std::mutex> lock(state->mutex);
            state->running = false;
        }
    }
    void DragItem(int idx, const std::vector < int >  & pressedSelection) {
        // When the transparent overlay is active, its medium-integrity HWND
        // receives the REAL WM_LBUTTONDOWN. This low-IL fallback path must not
        // start DoDragDrop or spawn another floating window.
        (void)idx; (void)pressedSelection;
        SyncGrip();
    }
    // The Broker performs native extraction; the UI only queues the command.
    void ExtractHere() {
        if (extractionPending_ || commandJob_) return;
        if (!QueueCommand(ARCHIVE_BROKER_EXTRACT_HERE, {},
                          L"Starting extraction..."))
            SetStatus(L"Could not queue extraction.");
    }
    void ExtractTo() {
        if (extractionPending_ || commandJob_) return;
        if (!QueueCommand(ARCHIVE_BROKER_EXTRACT_TO, {},
                          L"Opening extraction destination..."))
            SetStatus(L"Could not queue extraction.");
    }
    static HRESULT CollectFallback(FallbackJob& job) {
        if (job.cancelled.load(std::memory_order_acquire) ||
            g_hostShutdown.load(std::memory_order_acquire))
            return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        WIN32_FILE_ATTRIBUTE_DATA archiveInfo{};
        ULONGLONG archiveBytes=ArchivePreviewOptions::kLargeMetadataThreshold;
        if(GetFileAttributesExW(job.archive.c_str(),GetFileExInfoStandard,&archiveInfo))
            archiveBytes=(static_cast<ULONGLONG>(archiveInfo.nFileSizeHigh)<<32) | archiveInfo.nFileSizeLow;
        job.archiveBytes = archiveBytes;
        job.metadataMask = job.metadataPolicy.ForArchive(archiveBytes);
        const unsigned metadataMask = job.metadataMask;
        IShellItem * folder = nullptr;
        HRESULT hr = SHCreateItemFromIDList(job.folderId, IID_PPV_ARGS( & folder));
        if (FAILED(hr)) return hr;
        IEnumShellItems * enumerator = nullptr;
        hr = folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS( & enumerator));
        folder->Release();
        if (FAILED(hr)) return hr;
        HRESULT enumerateResult = S_OK;
        job.hasNextPage = false;
        // IEnumShellItems::Skip may be unsupported by third-party providers.
        // Fall back to Next() but never scan beyond the recovery-mode limit.
        const ULONG offset = static_cast < ULONG > (job.page * kMaxItems);
        if (offset) {
            HRESULT skipped = enumerator->Skip(offset);
            if (skipped == S_FALSE) enumerateResult = HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
            else if (FAILED(skipped)) {
                // Reset by reopening; a provider can partially advance before
                // returning E_NOTIMPL, so continuing the same enumerator is unsafe.
                enumerator->Release();enumerator = nullptr;
                IShellItem * freshFolder = nullptr;
                enumerateResult = SHCreateItemFromIDList(job.folderId,
                    IID_PPV_ARGS( & freshFolder));
                if (SUCCEEDED(enumerateResult))
                    enumerateResult = freshFolder->BindToHandler(nullptr,BHID_EnumItems,
                        IID_PPV_ARGS( & enumerator));
                if (freshFolder) freshFolder->Release();
                if (SUCCEEDED(enumerateResult)) for (ULONG i = 0;i < offset;++i) {
                    if (job.cancelled.load(std::memory_order_acquire) ||
                        g_hostShutdown.load(std::memory_order_acquire)) {
                        enumerateResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                        break;
                    }
                    IShellItem * discard = nullptr;ULONG got = 0;
                    const HRESULT next = enumerator->Next(1, & discard, & got);
                    if (discard) discard->Release();
                    if (next != S_OK || !got) {
                        enumerateResult = FAILED(next)?next:HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES);
                        break;
                    }
                }
            }
        }
        if (FAILED(enumerateResult)) {
            if (enumerator) enumerator->Release();return enumerateResult;
        }
        std::unordered_map < int,std::shared_ptr < ArchivePreviewIcons::Icon >> iconImages;
        for (int i = 0; i < kMaxItems; ++i) {
            if (job.cancelled.load(std::memory_order_acquire) ||
                g_hostShutdown.load(std::memory_order_acquire)) {
                enumerateResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                break;
            }
            IShellItem * item = nullptr;
            ULONG fetched = 0;
            hr = enumerator->Next(1, & item, & fetched);
            if (hr != S_OK || !item) {
                if (item) item->Release();
                if (FAILED(hr)) enumerateResult = hr;
                break;
            }
            Entry e;
            e.name = DisplayName(item);
            if (e.name.empty()) { item->Release(); continue; }
            hr = SHGetIDListFromObject(item, & e.id);
            if (FAILED(hr) || !Within(job.archiveId, e.id)) { item->Release(); continue; }
            SFGAOF attrs = SFGAO_FOLDER;
            e.folder = SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, & attrs)) &&
                       ((attrs & SFGAO_FOLDER) != 0);
            IShellItem2 * props = nullptr;
            const bool readSize = (metadataMask & 2u) && !e.folder;
            if (((metadataMask & 1u) || readSize) &&
                SUCCEEDED(item->QueryInterface(IID_PPV_ARGS( & props))) && props) {
                e.hasSize = readSize && SUCCEEDED(props->GetUInt64(PKEY_Size, & e.bytes));
                if (e.hasSize && !e.bytes && ArchivePreviewIsSingleStreamArchive(job.archive))
                    e.hasSize = false; // Shell's zero may be an unknown size.
                if (metadataMask & 1u) {
                    FILETIME utc{},local{};
                    SYSTEMTIME stamp{};
                    wchar_t date[80]{},time[80]{};
                    if (SUCCEEDED(props->GetFileTime(PKEY_DateModified,&utc)) &&
                        (utc.dwHighDateTime || utc.dwLowDateTime) &&
                        (e.modifiedValue = ArchivePreviewSort::FileTimeValue(utc)) &&
                        FileTimeToLocalFileTime(&utc,&local) &&
                        FileTimeToSystemTime(&local,&stamp) &&
                        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT,DATE_SHORTDATE,
                            &stamp,nullptr,date,static_cast < int > (_countof(date)),nullptr) &&
                        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,TIME_NOSECONDS,
                            &stamp,nullptr,time,static_cast < int > (_countof(time))))
                        e.modifiedLabel = std::wstring(date) + L" " + time;
                }
                props->Release();
            }
            SHFILEINFOW fi{};
            UINT flags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME;
            HIMAGELIST imageList = reinterpret_cast < HIMAGELIST > (SHGetFileInfoW(
                reinterpret_cast < LPCWSTR > (e.id), 0, & fi, sizeof(fi), flags));
            if (!imageList) {
                // A low-integrity archive provider may not resolve PIDL icons.
                // Keep its file-specific icon when available; otherwise use the
                // same attribute-based Shell icon lookup as the broker.
                fi = {};
                imageList = reinterpret_cast < HIMAGELIST > (SHGetFileInfoW(
                    e.folder ? L"folder" : e.name.c_str(),
                    e.folder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
                    &fi,sizeof(fi),SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX |
                        SHGFI_SMALLICON | SHGFI_TYPENAME));
            }
            if (imageList && !job.icons) job.icons = imageList;
            e.iconIndex = imageList ? fi.iIcon : - 1;
            if (imageList) {
                const auto known = iconImages.find(e.iconIndex);
                if (known != iconImages.end()) e.iconImage = known->second;
                else {
                    e.iconImage = ArchivePreviewIcons::Capture(imageList,e.iconIndex);
                    if (iconImages.size() < 1024) iconImages.emplace(e.iconIndex,e.iconImage);
                }
            }
            e.kind = fi.szTypeName[0] ? fi.szTypeName : (e.folder ? L"Folder" : L"File");
            job.entries.emplace_back(std::move(e));
            item->Release();
        }
        // Only count NEXT as available when a real item exists beyond this
        // page. Paging uses Ctrl+PageDown / Ctrl+PageUp in recovery mode.
        if (job.entries.size() >= static_cast < size_t > (kMaxItems) &&
            job.page + 1 < kMaxFallbackPages) {
            IShellItem * extra = nullptr;ULONG got = 0;
            const HRESULT more = enumerator->Next(1, & extra, & got);
            if (extra) extra->Release();
            job.hasNextPage = (more == S_OK && got > 0);
            if (FAILED(more)) enumerateResult = more;
        }
        enumerator->Release();
        if (FAILED(enumerateResult)) {job.entries.clear();return enumerateResult;}
        std::stable_sort(job.entries.begin(), job.entries.end(), [](const Entry & a, const Entry & b) {
            if (a.folder != b.folder) return a.folder > b.folder;
            return CompareStringOrdinal(a.name.c_str(), - 1, b.name.c_str(), - 1, TRUE) == CSTR_LESS_THAN;
        });
        return S_OK;
    }
    void ChangeFallbackPage(int direction) {
        if (brokerBrowserReady_ || cols_.empty()) return;
        Column& col = *cols_.back();
        if (!col.populated || (direction > 0 && !col.hasNextPage) ||
            (direction < 0 && col.page == 0) ||
            (direction > 0 && col.page + 1 >= kMaxFallbackPages)) return;
        const size_t previousPage = col.page;
        col.page = direction > 0 ? col.page + 1 : col.page - 1;
        col.populated = false;
        if (col.list) ShowWindow(col.list, SW_HIDE);
        QueueFallback(col, previousPage);
    }
    void EnsureFallbackContent() {
        if (cols_.empty() || brokerBrowserReady_ || selectionRetired_ ||
            terminalLoadFailure_ || fallbackLoadFailed_ ||
            g_hostShutdown.load(std::memory_order_acquire)) return;
        Column& col = *cols_.back();
        if (!col.populated) QueueFallback(col);
        else if (col.list && IsWindow(col.list))
            ShowWindow(col.list, SW_SHOWNOACTIVATE);
        Layout();
    }
    HRESULT AddColumn(PIDLIST_ABSOLUTE folderId, std::wstring heading, bool deferEntries = false) {

        if (!folderId || !Within(archiveId_, folderId) || cols_.size() >= kMaxDepth)
            return E_ACCESSDENIED;
        ArchivePreviewColumns::Load();
        auto col = std::make_unique < Column > ();
        col->heading = std::move(heading);
        col->folderId = ILCloneFull(folderId);
        if (!col->folderId) return E_OUTOFMEMORY;
        HWND list = CreateWindowExW(0, kListClass, L"",
            WS_CHILD | WS_VSCROLL | WS_TABSTOP | LVS_REPORT |
            LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS,
            0, 0, 1, 1, window_, nullptr, g_module, nullptr);
        if (!list) return HRESULT_FROM_WIN32(GetLastError());
        col->list = list;
        SendMessageW(list, WM_SETFONT, (WPARAM)font_, TRUE);
        ListView_SetExtendedListViewStyleEx(list, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT,
            LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);
        LVCOLUMNW lvc{};
        lvc.mask = LVCF_WIDTH | LVCF_TEXT;
        RECT listRect{}; GetClientRect(window_, & listRect);
        lvc.cx = std::max(1, static_cast < int > (listRect.right - listRect.left));
        lvc.pszText = const_cast < LPWSTR > (L"Name");
        ListView_InsertColumn(list, 0, & lvc);
        SetWindowSubclass(list, ListProc, 1, reinterpret_cast < DWORD_PTR > (this));
        if(HWND header=ListView_GetHeader(list))
            SetWindowSubclass(header,ColumnHeaderProc,1,reinterpret_cast<DWORD_PTR>(this));
        if (!cols_.empty() && cols_.back()->list) ShowWindow(cols_.back()->list, SW_HIDE);
        cols_.emplace_back(std::move(col));
        themeApplied_ = false; // Apply the palette to the newly created list too.
        Theme(); Layout();
        if (!deferEntries && !brokerBrowserReady_)
            QueueFallback(*cols_.back());
        return S_OK;
    }
    HRESULT Start() {
        if (!IsArchive(archive_)) {
            error_ = L"This extension is not enabled in the mod settings.";
            return S_OK;
        }
        if (archive_.find_first_of(L"\\/:") == std::wstring::npos) {
            if (!StartPathResolution())
                error_ = L"Could not start selected path resolution.";
            return S_OK;
        }
        return FinishStart();
    }
    HRESULT FinishStart() {
        DWORD attr = GetFileAttributesW(archive_.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"[AP bridge/prevhost] R5 GetFileAttributesW failed: error=%lu",GetLastError());
            error_ = L"The archive is not accessible.";
            return S_OK;
        }
        HRESULT hr = SHParseDisplayName(archive_.c_str(), nullptr, & archiveId_, 0, nullptr);
        if (FAILED(hr) || !archiveId_) {
            error_ = L"The Windows Shell could not locate the archive.";
            return S_OK;
        }
        // The list HWND is a broker anchor. Avoid enumerating the ZIP twice.
        // Populate the fallback only if the broker cannot attach.
        hr = AddColumn(archiveId_, Filename(archive_), true);
        if (FAILED(hr)) error_ = L"The Windows Shell cannot enumerate this archive format.";
        Layout();
        return S_OK;
    }
    void PositionHostIfChanged() {
        if (!window_ || !IsWindow(window_)) return;
        const int wantedWidth = std::max(1L, area_.right - area_.left);
        const int wantedHeight = std::max(1L, area_.bottom - area_.top);
        RECT actual{};
        if (!GetWindowRect(window_, & actual)) return;
        POINT current{actual.left, actual.top};
        HWND parent = GetParent(window_);
        if (parent) ScreenToClient(parent, & current);
        const bool positionChanged = current.x != area_.left || current.y != area_.top;
        const bool sizeChanged = actual.right - actual.left != wantedWidth ||
                                 actual.bottom - actual.top != wantedHeight;
        if (!positionChanged && !sizeChanged) return;
        SetWindowPos(window_, nullptr, area_.left, area_.top, wantedWidth, wantedHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    void Stop() {
        if (window_ || uiReady_) Diagnostic(L"Stop");
        // Do not let a broker HWND keep painting after the host unloads the
        // preview because Explorer changed the selection after an extraction.
        if (window_) ShowWindow(window_,SW_HIDE);
        SyncGrip(true);
        commandJob_.reset();
        if (resolveJob_) resolveJob_->cancelled.store(true, std::memory_order_release);
        resolveJob_.reset();
        if (fallbackJob_) fallbackJob_->cancelled.store(true, std::memory_order_release);
        fallbackJob_.reset();
        gripState_.reset();
        while (!cols_.empty()) {
            if (cols_.back()->list) DestroyWindow(cols_.back()->list);
            cols_.pop_back();
        }
        CoTaskMemFree(archiveId_); archiveId_ = nullptr;
        icons_ = nullptr;
        toolbar_ = nullptr;
        backButton_ = nullptr;
        extractButton_ = nullptr;
        extractToButton_ = nullptr;
        extractSelectedButton_ = nullptr;
        cancelButton_ = nullptr;
        cancelRequested_ = false;
        mouseDownSelection_.clear();
        brokerBrowserReady_ = false;
        brokerLoadFailed_ = false;
        fallbackLoadFailed_ = false;
        terminalLoadFailure_ = false;
        brokerStartupFailureAt_ = 0;
        brokerThemeKnown_ = false;
        brokerCanGoBack_ = false;
        brokerSelectionCount_ = 0;
        brokerAttachAttempt_ = 0;
        previewStartedAt_ = 0;
        selectionRetired_ = false;
        mouseDownIndex_ = - 1;
        dragStarted_ = false;
        dragInProgress_ = false;
        status_.clear();
        statusUntil_ = 0;
        fallbackNoticeLaidOut_ = 0;
        extractionPercent_ = 0;
        extractionPercentKnown_ = false;
        if (window_) {
            KillTimer(window_, kThemePollTimer);
            KillTimer(window_, kOperationStatusTimer);
            extractionPending_ = false;
            extractionStage_ = ARCHIVE_PROGRESS_NONE;
            extractionPercentKnown_ = false;
            KillTimer(window_, kChromeRepairTimer);
            DestroyWindow(window_);
            window_ = nullptr;
        }
        if (font_) { DeleteObject(font_); font_ = nullptr; }
        if (oleInitialized_) { OleUninitialize(); oleInitialized_ = false; }
        uiReady_ = false;
        diagnosticFallbackPaints_ = diagnosticFallbackIcons_ = diagnosticFallbackMisses_ = 0;
        diagnosticPending_ = true;
        themeApplied_ = false;
        appliedLanguage_ = -1;
        error_.clear();
    }
public:
    ArchiveHandler() {
        ++g_objects;
        ++g_liveArchiveHandlers;
        Wh_Log(L"[AP lifecycle] object=archive-handler event=created handler=%p tid=%lu",
            static_cast<IPreviewHandler*>(this),GetCurrentThreadId());
    }
    ~ArchiveHandler() {
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] object=archive-handler event=final-release-begin handler=%p tid=%lu hwnd=%p site=%d",
            static_cast<IPreviewHandler*>(this),GetCurrentThreadId(),window_,site_ ? 1 : 0);
        Stop();
        if (site_) site_->Release();
        Wh_Log(L"[AP lifecycle] object=archive-handler event=final-release-end handler=%p elapsed_ms=%llu",
            static_cast<IPreviewHandler*>(this),GetTickCount64() - started);
        --g_liveArchiveHandlers;
        --g_objects;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void *  * out) override {
        if (!out) return E_POINTER;
        * out = nullptr;
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IPreviewHandler))
            * out = static_cast < IPreviewHandler *  > (this);
        else if (IsEqualIID(iid, IID_IInitializeWithFile))
            * out = static_cast < IInitializeWithFile *  > (this);
        else if (IsEqualIID(iid, IID_IOleWindow))
            * out = static_cast < IOleWindow *  > (this);
        else if (IsEqualIID(iid, IID_IObjectWithSite))
            * out = static_cast < IObjectWithSite *  > (this);
        else if (IsEqualIID(iid, IID_IPreviewHandlerVisuals))
            * out = static_cast < IPreviewHandlerVisuals *  > (this);
        if (! * out) return E_NOINTERFACE;
        AddRef(); return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return refs_.fetch_add(1) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = refs_.fetch_sub(1) - 1;
        if (!n) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE Initialize(LPCWSTR file, DWORD) override {
        if (!file || !file[0]) return E_INVALIDARG;
        // Existing Windows Shell associations can dispatch compound .tar.* files through their
        // final suffix; this full-name check rejects unenabled standalone streams.
        if (!ArchivePreviewOptions::MatchArchive(file)) return E_NOTIMPL;
        Stop(); archive_ = file;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetWindow(HWND hwnd, const RECT * rc) override {
        if (!hwnd || !rc) return E_INVALIDARG;
        parent_ = hwnd; area_ = * rc;
        // Explorer can expose the parent before calling DoPreview. Create a
        // correctly themed opaque child NOW instead of showing prevhost white.
        // The same HWND is reused by DoPreview; no extra temporary overlay.
        if (!window_) {
            if (!InitOnceExecuteOnce( & g_classInit, InitClass, nullptr, nullptr)) return E_FAIL;
            palette_ = ResolvePalette(parent_, hostBg_, hostText_, hasHostBg_, hasHostText_);
            window_ = CreateWindowExW(0, kWindowClass, L"",
                WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                area_.left, area_.top,
                std::max(1L, area_.right - area_.left),
                std::max(1L, area_.bottom - area_.top), parent_, nullptr, g_module, this);
            if (!window_) return HRESULT_FROM_WIN32(GetLastError());
            ShowWindow(window_, SW_SHOWNOACTIVATE);
            RedrawWindow(window_, nullptr, nullptr,
                RDW_INVALIDATE | RDW_NOERASE | RDW_NOCHILDREN);
        }
        // SetWindow may be called repeatedly by Explorer. Reparenting the
        // existing HWND to the SAME parent and forcing an unchanged size
        // needlessly invalidates the surface during ZIP selection.
        if (window_) {
            if (GetParent(window_) != hwnd) {
                SetLastError(0);
                HWND oldParent = SetParent(window_, hwnd);
                if (!oldParent && GetLastError() != 0)
                    return HRESULT_FROM_WIN32(GetLastError());
            }
            PositionHostIfChanged();
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetRect(const RECT * rc) override {
        if (!rc) return E_INVALIDARG;
        area_ = * rc;
        PositionHostIfChanged();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DoPreview() override {
        if (uiReady_) {
            // A genuine new DoPreview can revive a previously retired ZIP.
            // An implicit ancestor visibility change cannot.
            if (selectionRetired_) {
                selectionRetired_ = false;
                // A real new preview gesture, unlike a visibility timer,
                // allows retrying a previously failed archive.
                brokerLoadFailed_ = false;
                fallbackLoadFailed_ = false;
                terminalLoadFailure_ = false;
                brokerStartupFailureAt_ = 0;
                brokerAttachAttempt_ = 0;
                error_.clear();
                if (toolbar_) ShowWindow(toolbar_, SW_SHOWNOACTIVATE);
                if (!cols_.empty() && cols_.back()->list)
                    ShowWindow(cols_.back()->list, SW_SHOWNOACTIVATE);
            }
            if (window_ && !IsWindowVisible(window_))
                ShowWindow(window_, SW_SHOWNOACTIVATE);
            Layout();
            SyncGrip();
            if (window_) InvalidateRect(window_, nullptr, FALSE);
            return S_OK;
        }
        if (!parent_) return E_UNEXPECTED;
        if (!InitOnceExecuteOnce( & g_classInit, InitClass, nullptr, nullptr)) return E_FAIL;
        INITCOMMONCONTROLSEX ic{};
        ic.dwSize = sizeof(ic); ic.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx( & ic);
        HRESULT ole = OleInitialize(nullptr);
        oleInitialized_ = SUCCEEDED(ole);
        // Establish the host palette before any child can receive WM_ERASEBKGND.
        // The first exposed frame is a themed surface, not the default white DC.
        palette_ = ResolvePalette(parent_, hostBg_, hostText_, hasHostBg_, hasHostText_);
        // SetWindow already created the themed surface on the usual Explorer
        // path. Support hosts that call DoPreview without first using SetWindow
        // to paint it, but never recreate an existing HWND (avoids a white flash).
        if (!window_) window_ = CreateWindowExW(0, kWindowClass, L"",
            WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            area_.left, area_.top,
            std::max(1L, area_.right - area_.left),
            std::max(1L, area_.bottom - area_.top), parent_, nullptr, g_module, this);
        if (!window_) {
            if (oleInitialized_) { OleUninitialize(); oleInitialized_ = false; }
            return HRESULT_FROM_WIN32(GetLastError());
        }
        toolbar_ = CreateWindowExW(0,kToolbarClass,L"",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0,0,1,1,window_,nullptr,g_module,this);
        if (!toolbar_) {DWORD error = GetLastError();Stop();return HRESULT_FROM_WIN32(error);}
        backButton_ = CreateWindowExW(0, L"BUTTON", ArchivePreviewText::Get(ArchivePreviewText::Id::Back),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
            0, 0, 1, 1, toolbar_,
            reinterpret_cast < HMENU > (static_cast < INT_PTR > (kBackButtonId)), g_module, nullptr);
        extractButton_ = CreateWindowExW(0, L"BUTTON", ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractHere),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
            0, 0, 1, 1, toolbar_,
            reinterpret_cast < HMENU > (static_cast < INT_PTR > (kExtractButtonId)), g_module, nullptr);
        cancelButton_ = CreateWindowExW(0,L"BUTTON",ArchivePreviewText::Get(ArchivePreviewText::Id::Cancel),
            WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
            0,0,1,1,window_,
            reinterpret_cast < HMENU > (static_cast < INT_PTR > (kCancelButtonId)),g_module,nullptr);
        extractToButton_ = CreateWindowExW(0, L"BUTTON", ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractTo),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
            0, 0, 1, 1, toolbar_,
            reinterpret_cast < HMENU > (static_cast < INT_PTR > (kExtractToButtonId)), g_module, nullptr);
        extractSelectedButton_ = CreateWindowExW(0,L"BUTTON",ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractSelected),
            WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
            0,0,1,1,toolbar_,
            reinterpret_cast < HMENU > (static_cast < INT_PTR > (kExtractSelectedButtonId)),
            g_module,nullptr);
        if (!backButton_ || !extractButton_ || !extractToButton_ ||
            !extractSelectedButton_ || !cancelButton_) {
            DWORD err = GetLastError();
            Stop();
            return HRESULT_FROM_WIN32(err ? err : ERROR_NOT_ENOUGH_MEMORY);
        }
        ArchivePreviewButtons::Attach(backButton_);
        ArchivePreviewButtons::Attach(extractButton_);
        ArchivePreviewButtons::Attach(extractToButton_);
        ArchivePreviewButtons::Attach(extractSelectedButton_);
        ArchivePreviewButtons::Attach(cancelButton_);
        uiReady_ = true;
        UpdateFont(); Theme();
        // Show the already-themed host before any expensive Shell enumeration.
        // The root list starts hidden and is only populated if the broker fails.
        ShowWindow(window_, SW_SHOWNOACTIVATE);
        RedrawWindow(window_, nullptr, nullptr,
            RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW | RDW_NOCHILDREN);
        SetTimer(window_, kThemePollTimer, 2000, nullptr);
        previewStartedAt_ = GetTickCount64();
        HRESULT hr = Start();
        SyncGrip();
        Wh_Log(L"[AP lifecycle] object=archive-handler event=preview-started handler=%p hwnd=%p dpi=%u hr=0x%08lX elapsed_ms=%llu",
            static_cast<IPreviewHandler*>(this),window_,dpi_,static_cast<unsigned long>(hr),
            GetTickCount64() - previewStartedAt_);
        InvalidateRect(window_, nullptr, FALSE);
        return hr;
    }
    HRESULT STDMETHODCALLTYPE Unload() override {
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] object=archive-handler event=unload-begin handler=%p tid=%lu hwnd=%p broker_ready=%d",
            static_cast<IPreviewHandler*>(this),GetCurrentThreadId(),window_,brokerBrowserReady_ ? 1 : 0);
        Stop();
        archive_.clear();
        Wh_Log(L"[AP lifecycle] object=archive-handler event=unload-end handler=%p elapsed_ms=%llu",
            static_cast<IPreviewHandler*>(this),GetTickCount64() - started);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetFocus() override {
        if (!cols_.empty() && cols_.back()->list) {
            ::SetFocus(cols_.back()->list); return S_OK;
        }
        if (window_) { ::SetFocus(window_); return S_OK; }
        return S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE QueryFocus(HWND * out) override {
        if (!out) return E_POINTER;
        * out = GetFocus(); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG * ) override { return S_FALSE; }
    HRESULT STDMETHODCALLTYPE GetWindow(HWND * out) override {
        if (!out) return E_POINTER;
        * out = window_; return window_ ? S_OK : E_FAIL;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown * site) override {
        if (site) site->AddRef();
        if (site_) site_->Release();
        site_ = site; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void *  * out) override {
        if (!out) return E_POINTER;
        * out = nullptr;
        return site_ ? site_->QueryInterface(iid, out) : E_FAIL;
    }
    HRESULT STDMETHODCALLTYPE SetBackgroundColor(COLORREF color) override {
        hostBg_ = color; hasHostBg_ = true; Theme(); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetTextColor(COLORREF color) override {
        hostText_ = color; hasHostText_ = true; Theme(); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetFont(const LOGFONTW * lf) override {
        if (!lf) return E_INVALIDARG;
        hostFont_ = * lf; hasHostFont_ = true;
        dpi_ = 0; UpdateFont(); Layout(); return S_OK;
    }
};

} // namespace ArchivePreviewHost


namespace ArchivePreviewBroker {
static HINSTANCE g_module = nullptr; // The Windhawk DLL, never explorer.exe.
// Archive Preview transfer broker v1.1.0. Runs as the current user at
// ordinary integrity, NOT elevated. Transfer operations stay out of prevhost.
// SPDX-License-Identifier: Apache-2.0

// ComCtl32 v6 enables Windows-native alpha-blended ListView marquee with LVS_EX_DOUBLEBUFFER.
namespace fs = std::filesystem;
// Shutdown is cooperative. Never terminate a thread while it can be inside
// a Windows Shell/COM provider; join worker handles before DLL unload.
static std::atomic < bool > g_cancelForShutdown{false};
// The service owner keeps this event alive until the service and UI have joined.
static HANDLE g_brokerStopEvent = nullptr;
static thread_local const std::atomic < bool >  * t_operationCancelFlag = nullptr;
static void SignalBrokerShutdown() {
    g_cancelForShutdown.store(true,std::memory_order_release);
    if (g_brokerStopEvent) SetEvent(g_brokerStopEvent);
}
static bool CancelledForCurrentOperation(const std::atomic < bool >  * overrideFlag = nullptr) {
    return g_cancelForShutdown.load(std::memory_order_acquire) ||
        (t_operationCancelFlag && t_operationCancelFlag->load(std::memory_order_acquire)) ||
        (overrideFlag && overrideFlag->load(std::memory_order_acquire));
}

struct ScopedOperationCancellation {
    const std::atomic < bool >  * previous = t_operationCancelFlag;
    explicit ScopedOperationCancellation(const std::atomic < bool >  * flag) {
        if(flag) t_operationCancelFlag = flag;
    }
    ~ScopedOperationCancellation(){t_operationCancelFlag = previous;}
};
static constexpr size_t kBrowserFolderMaxEntries = 200000;

struct Request {
    DWORD command{};
    GUID requestId{};
    std::wstring archive;
    std::vector < std::wstring > items;
    ArchiveBrokerColumnChange columnChange{};
};

static bool IsArchiveExtension(const std::wstring & filename) {
    return ArchivePreviewOptions::MatchArchive(filename);
}
// Broker is a medium-integrity service, so never let a low-IL requester
// resolve an ambiguous or device namespace path at the broker's privilege.
// Normal local paths only (e.g. C:\Users\...\archive.zip).
static bool SafeArchivePath(const std::wstring & path, bool mustExist) {
    if (path.size() < 7 || path.size() > 32700 ||
        !((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z')) ||
        path[1] != L':' || path[2] != L'\\' ||
        path.find(L'/') != std::wstring::npos ||
        path.find(L'|') != std::wstring::npos ||
        path.find(L':',2) != std::wstring::npos) return false;
    size_t pos = 3;
    while (pos < path.size()) {
        const auto end = path.find(L'\\',pos);
        const auto length = (end == std::wstring::npos ? path.size() : end) - pos;
        if (!length || (length == 1 && path[pos] == L'.') ||
            (length == 2 && path[pos] == L'.' && path[pos + 1] == L'.')) return false;
        if (end == std::wstring::npos) break;
        pos = end + 1;
    }
    if (!IsArchiveExtension(path)) return false;
    if (mustExist) {
        // Check *every* existing component by an opened handle, including the
        // archive itself. GetFileAttributesW on only the leaf would follow an
        // ancestor junction/symlink to a different volume or network share.
        // The helper also rejects non-fixed drives and alternate streams.
        if (!ArchivePreviewPathSafety::ValidPathComponents(
                fs::path(path),false,false)) return false;
    }
    // GRIP_CLEAR and cancellation can legitimately reference a deleted file.
    // Those commands never reopen or execute filesystem content for this path.
    return true;
}
static bool SafeArchiveLeaf(const std::wstring & name);
static bool SafeRelativePath(const std::wstring & path) {
    if (path.empty() || path.size() > 2048 || path[0] == L'\\' || path[0] == L'/' ||
        path.back() == L'\\' ||
        path.find(L':') != std::wstring::npos || path.find(L'/') != std::wstring::npos) return false;
    size_t pos = 0;
    while (pos < path.size()) {
        size_t end = path.find(L'\\',pos);
        if (end == std::wstring::npos) end = path.size();
        if (end == pos || !SafeArchiveLeaf(path.substr(pos,end - pos))) return false;
        pos = end + 1;
    }
    return true;
}
// A half-written request must not block the only named-pipe listener forever.
// PeekNamedPipe is non-blocking. Read only bytes already reported available,
// giving the peer five seconds since its last progress to finish each frame.
static HRESULT ReadPipeWithDeadline(HANDLE pipe,void * output,DWORD bytes,ULONGLONG deadline) {
    auto * position = static_cast < BYTE *  > (output);
    ULONGLONG idleDeadline = GetTickCount64() + 5000;
    while (bytes) {
        if (g_cancelForShutdown.load(std::memory_order_acquire))
            return HRESULT_FROM_WIN32(ERROR_OPERATION_ABORTED);
        DWORD available = 0;
        if (!PeekNamedPipe(pipe,nullptr,0,nullptr, & available,nullptr))
            return HRESULT_FROM_WIN32(GetLastError());
        if (!available) {
            if (GetTickCount64() >= deadline || GetTickCount64() >= idleDeadline)
                return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
            Sleep(5);
            continue;
        }
        const DWORD count = std::min(bytes,available);
        DWORD received = 0;
        if (!ReadFile(pipe,position,count, & received,nullptr) || !received)
            return HRESULT_FROM_WIN32(GetLastError()?GetLastError():ERROR_BROKEN_PIPE);
        position += received;
        bytes -= received;
        idleDeadline = GetTickCount64() + 5000;
    }
    return S_OK;
}
static HRESULT Receive(HANDLE pipe, Request & request) {
    const ULONGLONG headerDeadline = GetTickCount64() + 5000;
    ArchiveBrokerWireHeader header{};
    HRESULT hr = ReadPipeWithDeadline(pipe, & header,sizeof(header),headerDeadline);
    if (FAILED(hr)) return hr;
    if (header.magic != ARCHIVE_BROKER_MAGIC ||
        (header.command != ARCHIVE_BROKER_EXTRACT_HERE &&
         header.command != ARCHIVE_BROKER_EXTRACT_TO &&
         header.command != ARCHIVE_BROKER_COPY_SELECTION &&
         header.command != ARCHIVE_BROKER_EXTRACT_SELECTED_TO &&
         header.command != ARCHIVE_BROKER_EXTRACT_SELECTED_HERE &&
         header.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO &&
         header.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE &&
         /* ARCHIVE_BROKER_OPEN_FILE is broker UI only, never an IPC request. */
         header.command != ARCHIVE_BROKER_GRIP_UPDATE && header.command != ARCHIVE_BROKER_GRIP_CLEAR &&
         header.command != ARCHIVE_BROKER_BROWSER_BACK &&
         header.command != ARCHIVE_BROKER_PING &&
         header.command != ARCHIVE_BROKER_QUERY_STATUS &&
         header.command != ARCHIVE_BROKER_CANCEL_OPERATION &&
         header.command != ARCHIVE_BROKER_RESOLVE_SELECTED_PATH &&
         header.command != ARCHIVE_BROKER_SAVE_COLUMN) ||
        header.archiveChars > ARCHIVE_BROKER_MAX_CHARS ||
        header.itemCount > ARCHIVE_BROKER_MAX_PATHS) return E_INVALIDARG;
    // Count and total byte budget are checked before the broker allocates
    // each path; the client has the same bounds.
    ULONGLONG receivedBytes = static_cast < ULONGLONG > (header.archiveChars) * sizeof(wchar_t);
    if (receivedBytes > ARCHIVE_BROKER_MAX_WIRE_BYTES) return E_INVALIDARG;
    const ULONGLONG deadline = GetTickCount64() + 60000; // absolute wire budget, 5-second idle timeout
    request.command = header.command;
    request.requestId = header.requestId;
    if (header.command == ARCHIVE_BROKER_SAVE_COLUMN) {
        if (header.archiveChars || header.itemCount) return E_INVALIDARG;
        hr = ReadPipeWithDeadline(pipe,&request.columnChange,sizeof(request.columnChange),headerDeadline);
        if (FAILED(hr)) return hr;
        return request.columnChange.previewWindow && !request.columnChange.reserved &&
            ArchivePreviewColumns::Valid(request.columnChange.change)
            ? S_OK : E_INVALIDARG;
    }
    request.items.reserve(header.itemCount);
    if (header.command == ARCHIVE_BROKER_QUERY_STATUS) {
        const GUID zero{};
        return !InlineIsEqualGUID(header.requestId,zero) &&
            header.archiveChars == 0 && header.itemCount == 0 ? S_OK : E_INVALIDARG;
    }
    if (header.command == ARCHIVE_BROKER_PING)
        return header.archiveChars == 0 && header.itemCount == 0 ? S_OK : E_INVALIDARG;
    if (header.archiveChars == 0) return E_INVALIDARG;
    request.archive.resize(header.archiveChars);
    hr = ReadPipeWithDeadline(pipe, & request.archive[0],header.archiveChars * sizeof(wchar_t),deadline);
    if (FAILED(hr) || request.archive.find(L'\0') != std::wstring::npos ||
        !IsArchiveExtension(request.archive)) return E_INVALIDARG;
    // CLEAR can arrive after Explorer removed an archive; its syntax must
    // remain valid, while every new operation must target an existing file.
    const bool mustExist = request.command != ARCHIVE_BROKER_GRIP_CLEAR &&
        request.command != ARCHIVE_BROKER_CANCEL_OPERATION;
    // The original archive can have been moved/deleted while a long extraction
    // is running. Cancel authenticates against the queued request and preview,
    // not the current filesystem state of its archive path.
    if (request.command == ARCHIVE_BROKER_RESOLVE_SELECTED_PATH) {
        if (!SafeArchiveLeaf(request.archive)) return E_INVALIDARG;
    } else if (!SafeArchivePath(request.archive,mustExist)) return E_INVALIDARG;
    for (DWORD i = 0;i < header.itemCount;i++) {
        DWORD length{};
        hr = ReadPipeWithDeadline(pipe, & length,sizeof(length),deadline);
        if (FAILED(hr) || length == 0 || length > 2048) return E_INVALIDARG;
        const ULONGLONG entryBytes = sizeof(length) + static_cast < ULONGLONG > (length) * sizeof(wchar_t);
        if (entryBytes > ARCHIVE_BROKER_MAX_WIRE_BYTES - receivedBytes) return E_INVALIDARG;
        receivedBytes += entryBytes;
        std::wstring path(length,L'\0');
        hr = ReadPipeWithDeadline(pipe, & path[0],length * sizeof(wchar_t),deadline);
        if (FAILED(hr) || path.find(L'\0') != std::wstring::npos || !SafeRelativePath(path))
            return E_INVALIDARG;
        request.items.push_back(std::move(path));
    }
    const bool requiresItems = request.command == ARCHIVE_BROKER_COPY_SELECTION ||
        request.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
        request.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE ||
        request.command == ARCHIVE_BROKER_OPEN_FILE;
    if (request.command == ARCHIVE_BROKER_OPEN_FILE && request.items.size() != 1)
        return E_INVALIDARG;
    if (request.command == ARCHIVE_BROKER_GRIP_UPDATE ||
        request.command == ARCHIVE_BROKER_GRIP_CLEAR ||
        request.command == ARCHIVE_BROKER_BROWSER_BACK ||
        request.command == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO ||
        request.command == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE ||
        request.command == ARCHIVE_BROKER_RESOLVE_SELECTED_PATH) {
        if (request.items.size() != 1) return E_INVALIDARG;
    } else if (requiresItems != !request.items.empty()) return E_INVALIDARG;
    return S_OK;
}
static std::wstring GetName(IShellItem * item) {
    PWSTR p = nullptr;
    std::wstring result;
    if (item && SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, & p)) && p) result = p;
    CoTaskMemFree(p);
    return result;
}
static HRESULT Enumerate(IShellItem * folder, std::vector < IShellItem *  >  & items,
                         size_t maxEntries = kBrowserFolderMaxEntries) {
    IEnumShellItems * enumeration = nullptr;
    HRESULT hr = folder->BindToHandler(nullptr,BHID_EnumItems,IID_PPV_ARGS( & enumeration));
    if (FAILED(hr)) return hr;
    for (;;) {
        if(CancelledForCurrentOperation()){hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;}
        IShellItem * item = nullptr;
        ULONG fetched{};
        hr = enumeration->Next(1, & item, & fetched);
        if (hr == S_FALSE) { hr = S_OK; break; }
        if (FAILED(hr)) { if (item) item->Release(); break; }
        if (!item) { hr = E_FAIL; break; }
        // Do not fail entire ZIP folders at 2,000 entries. A larger safety
        // cap prevents unbounded allocations from hostile namespace providers.
        if (items.size() >= maxEntries) { item->Release(); hr = HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE); break; }
        items.push_back(item);
    }
    enumeration->Release();
    return hr;
}
static void ReleaseItems(std::vector < IShellItem *  >  & items) {
    for (auto * p : items) if (p) p->Release();
    items.clear();
}
static HRESULT FindItem(IShellItem * root, const std::wstring & relative, IShellItem *  * output) {
    if (!output) return E_POINTER;
    * output = nullptr;
    if (!SafeRelativePath(relative)) return E_INVALIDARG;
    root->AddRef();
    IShellItem * current = root;
    size_t pos = 0;
    HRESULT hr = S_OK;
    while (pos < relative.size()) {
        size_t end = relative.find(L'\\',pos);
        if (end == std::wstring::npos) end = relative.size();
        const auto segment = relative.substr(pos,end - pos);
        std::vector < IShellItem *  > children;
        hr = Enumerate(current,children,kBrowserFolderMaxEntries);
        IShellItem * found = nullptr;
        if (SUCCEEDED(hr)) for (auto * child:children) {
            if (!_wcsicmp(GetName(child).c_str(),segment.c_str())) {
                found = child;
                found->AddRef();
                break;
            }
        }
        ReleaseItems(children);
        current->Release();
        current = nullptr;
        if (FAILED(hr) || !found) { if (found) found->Release(); return FAILED(hr)?hr:HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND); }
        current = found;
        pos = end + 1;
    }
    * output = current;
    return S_OK;
}
static HRESULT OpenArchive(const std::wstring & path, IShellItem *  * out) {
    if (!out) return E_POINTER;
    * out = nullptr;
    if (!SafeArchivePath(path,true)) return E_INVALIDARG;
    // Do not manufacture .tbz filename aliases or temporary archives.
    // Use the native Windows Shell provider, if available.
    return SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(out));
}
// Resolve a complete selection with ONE Shell enumeration per parent directory.
// All COM pointers stay within their originating STA; the lookup is request-local.
// This replaces the per-selected-item FindItem() loop (M*N enumerations).
static std::wstring ShellLookupKey(const std::wstring & value) {
    std::wstring key = value;
    std::transform(key.begin(),key.end(),key.begin(),[](wchar_t ch) {
        return static_cast < wchar_t > (towlower(ch));
    });
    return key;
}
// retain only requested Shell items. A large folder can contain 200,000
// children; keeping every COM item alive for a 1-file selection is unnecessary.
// This trie also enumerates each requested parent at most once.
struct SelectedPathNode {
    std::unordered_map < std::wstring,std::unique_ptr < SelectedPathNode >> children;
    std::wstring relative;
    bool selected{};
};
static HRESULT ResolveSelectedNode(IShellItem * folder,SelectedPathNode & node,
                                   std::unordered_map < std::wstring,IShellItem *  >  & found,
                                   unsigned depth,unsigned maxDepth = 16) {
    if (!folder || depth > maxDepth) return E_INVALIDARG;
    IEnumShellItems * enumeration = nullptr;
    HRESULT hr = folder->BindToHandler(nullptr,BHID_EnumItems,IID_PPV_ARGS( & enumeration));
    if (FAILED(hr)) return hr;
    size_t visited = 0,matched = 0;
    for (;;) {
        if(CancelledForCurrentOperation()){hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;}
        IShellItem * item = nullptr;
        ULONG fetched = 0;
        hr = enumeration->Next(1, & item, & fetched);
        if (hr == S_FALSE) {hr = S_OK;break;}
        if (FAILED(hr) || !item || !fetched) {
            if (item) item->Release();
            hr = FAILED(hr)?hr:E_FAIL;break;
        }
        if (++visited > kBrowserFolderMaxEntries) {
            item->Release();hr = HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);break;
        }
        const auto match = node.children.find(ShellLookupKey(GetName(item)));
        if (match != node.children.end()) {
            ++matched;
            SelectedPathNode & child =  * match->second;
            if (child.selected) {
                item->AddRef();
                found.emplace(ShellLookupKey(child.relative),item);
            }
            if (!child.children.empty()) {
                hr = ResolveSelectedNode(item,child,found,depth + 1,maxDepth);
                if (FAILED(hr)) {item->Release();break;}
            }
        }
        item->Release();
        if (matched == node.children.size()) {hr = S_OK;break;}
    }
    enumeration->Release();
    if (SUCCEEDED(hr) && matched != node.children.size())
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    return hr;
}
static HRESULT ResolveSelectedItems(IShellItem * root,
                                    const std::vector < std::wstring >  & names,
                                    std::vector < IShellItem *  >  & result,
                                    unsigned maxDepth = 16) {
    result.clear();
    if (!root) return E_POINTER;
    if (names.empty()) return Enumerate(root,result,kBrowserFolderMaxEntries);
    SelectedPathNode tree;
    for (const auto & relative:names) {
        if (!SafeRelativePath(relative)) return E_INVALIDARG;
        SelectedPathNode * node =  & tree;
        size_t start = 0;
        while (start < relative.size()) {
            const size_t end = relative.find(L'\\',start);
            const size_t stop = end == std::wstring::npos?relative.size():end;
            const auto key = ShellLookupKey(relative.substr(start,stop - start));
            auto & slot = node->children[key];
            if (!slot) slot = std::make_unique < SelectedPathNode > ();
            node = slot.get();
            start = stop + 1;
        }
        node->relative = relative;
        node->selected = true;
    }
    std::unordered_map < std::wstring,IShellItem *  > found;
    HRESULT hr = ResolveSelectedNode(root,tree,found,0,maxDepth);
    if (SUCCEEDED(hr)) {
        result.reserve(names.size());
        for (const auto & relative:names) {
            const auto it = found.find(ShellLookupKey(relative));
            if (it == found.end()) {
                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);break;
            }
            it->second->AddRef();
            result.push_back(it->second);
        }
    }
    for (auto & entry:found) entry.second->Release();
    if (FAILED(hr)) ReleaseItems(result);
    return hr;
}
// Forward declarations for Shell helpers defined below.
static HRESULT DirectoryItem(const std::wstring & path,IShellItem *  * target);
static std::wstring ParentFolder(const std::wstring & path);
static HRESULT PickFolder(IShellItem *  * selected,const std::wstring & initialDirectory);
static std::wstring OpenLowerExtension(const std::wstring & path);
static bool IsDangerousOpenExtension(const std::wstring & ext);
// === NATIVE SHELL BACKEND (NO MOD-MANAGED DISK STAGING) ===
// Every Shell interface remains in its originating STA. Keep this separate
// from the existing visual components and their layout calculations.
static std::wstring NativeShellChildName(IShellFolder * folder, PCUITEMID_CHILD child) {
    if (!folder || !child) return {};
    STRRET str{};
    if (FAILED(folder->GetDisplayNameOf(child,SHGDN_INFOLDER,&str))) return {};
    // Allocate the name through Shell, rather than reserving 64 KB on
    // a potentially reentrant broker STA stack frame.
    PWSTR name = nullptr;
    if (FAILED(StrRetToStrW(&str,child,&name)) || !name) {
        CoTaskMemFree(name);
        return {};
    }
    std::wstring result(name);
    CoTaskMemFree(name);
    return result;
}
static HRESULT NativeShellBindFolder(const std::wstring & archive,
                                     const std::wstring & relative,IShellFolder *  * out) {
    if (!out) return E_POINTER;
    * out = nullptr;
    PIDLIST_ABSOLUTE root = nullptr;
    HRESULT hr = SHParseDisplayName(archive.c_str(),nullptr, & root,0,nullptr);
    if (FAILED(hr) || !root) return FAILED(hr)?hr:E_FAIL;
    IShellFolder * folder = nullptr;
    hr = SHBindToObject(nullptr,root,nullptr,IID_IShellFolder,
                      reinterpret_cast < void *  *  > ( & folder));
    CoTaskMemFree(root);
    if (FAILED(hr) || !folder) return FAILED(hr)?hr:E_FAIL;
    if (relative.empty()) { * out = folder;return S_OK;}
    if (!SafeRelativePath(relative)) {folder->Release();return E_INVALIDARG;}
    size_t begin = 0;
    while (begin < relative.size()) {
        const size_t end = relative.find(L'\\',begin);
        const std::wstring target = relative.substr(begin,
            end == std::wstring::npos?std::wstring::npos:end - begin);
        if (target.empty()) {folder->Release();return E_INVALIDARG;}
        IEnumIDList * children = nullptr;
        hr = folder->EnumObjects(nullptr,static_cast < SHCONTF > (SHCONTF_FOLDERS | SHCONTF_NONFOLDERS), & children);
        if (FAILED(hr) || !children) {folder->Release();return FAILED(hr)?hr:E_FAIL;}
        PITEMID_CHILD child = nullptr;
        HRESULT nextResult = S_FALSE;
        size_t scanned = 0;
        for (;;) {
            if (CancelledForCurrentOperation()) {
                nextResult = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;
            }
            nextResult = children->Next(1, & child,nullptr);
            if (nextResult != S_OK) break;
            if (!child) {nextResult = E_FAIL;break;}
            if (++scanned > kBrowserFolderMaxEntries) {
                nextResult = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);break;
            }
            if (NativeShellChildName(folder,child) == target) break;
            CoTaskMemFree(child);child = nullptr;
        }
        children->Release();
        if (nextResult != S_OK || !child) {
            if (child) CoTaskMemFree(child);
            folder->Release();
            return FAILED(nextResult)?nextResult:
                HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
        }
        IShellFolder * next = nullptr;
        hr = folder->BindToObject(child,nullptr,IID_IShellFolder,
                                reinterpret_cast < void *  *  > ( & next));
        CoTaskMemFree(child);
        folder->Release();
        if (FAILED(hr) || !next) return FAILED(hr)?hr:E_FAIL;
        folder = next;
        if (end == std::wstring::npos) break;
        begin = end + 1;
    }
    * out = folder;
    return S_OK;
}
// Native clipboard/drag use the provider's original IDataObject. Native
// extraction uses Shell items. External drag has a separate virtual-file data
// object and does not advertise the original compressed namespace to targets.
static HRESULT NativeShellUIObject(const Request & req,HWND owner,
                                    REFIID iid,void *  * out) {
    if (!out) return E_POINTER;
    * out = nullptr;
    if (!SafeArchivePath(req.archive,true)) return E_INVALIDARG;
    if (req.items.size() > 4096) return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    std::wstring commonParent;
    std::vector < std::wstring > basenames;
    basenames.reserve(req.items.size());
    for (const auto & relative:req.items) {
        if (!SafeRelativePath(relative)) return E_INVALIDARG;
        const size_t split = relative.find_last_of(L'\\');
        const std::wstring parent = split == std::wstring::npos?L"":relative.substr(0,split);
        const std::wstring base = relative.substr(split == std::wstring::npos?0:split + 1);
        if (base.empty()) return E_INVALIDARG;
        if (!basenames.empty() && parent != commonParent) return E_INVALIDARG;
        commonParent = parent;
        basenames.push_back(base);
    }
    IShellFolder * folder = nullptr;
    HRESULT hr = NativeShellBindFolder(req.archive,commonParent, & folder);
    if (FAILED(hr)) return hr;
    IEnumIDList * items = nullptr;
    hr = folder->EnumObjects(owner,static_cast < SHCONTF > (SHCONTF_FOLDERS | SHCONTF_NONFOLDERS), & items);
    if (FAILED(hr) || !items) {folder->Release();return FAILED(hr)?hr:E_FAIL;}
    std::vector < PITEMID_CHILD > selected;
    std::unordered_set < std::wstring > wanted;
    for (const auto & name:basenames) wanted.insert(ShellLookupKey(name));
    PITEMID_CHILD child = nullptr;
    size_t examined = 0;
    for (;;) {
        if (CancelledForCurrentOperation()) {
            hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;
        }
        const HRESULT next = items->Next(1, & child,nullptr);
        if (next == S_FALSE) {hr = S_OK;break;}
        if (next != S_OK || !child) {
            if (child) {CoTaskMemFree(child);child = nullptr;}
            hr = FAILED(next)?next:E_FAIL;
            break;
        }
        if (++examined > kBrowserFolderMaxEntries || selected.size() >= ARCHIVE_BROKER_MAX_PATHS) {
            CoTaskMemFree(child);
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            child = nullptr;
            break;
        }
        const std::wstring name = NativeShellChildName(folder,child);
        if (name.empty()) {
            // A nameless Shell item is not a valid omission during an entire
            // archive extraction: fail instead of reporting partial success.
            CoTaskMemFree(child);
            hr = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
            child = nullptr;
            break;
        }
        const bool chosen = basenames.empty() ||
            wanted.erase(ShellLookupKey(name)) != 0;
        if (chosen) selected.push_back(child);
        else CoTaskMemFree(child);
        child = nullptr;
        if (!basenames.empty() && wanted.empty()) {hr = S_OK;break;}
    }
    items->Release();
    if (SUCCEEDED(hr)) {
        if (selected.empty() || (!basenames.empty() && !wanted.empty()))
            hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        else {
            std::vector < PCUITEMID_CHILD > pointers(selected.begin(),selected.end());
            if (IsEqualIID(iid,IID_IShellItemArray)) {
                IShellItemArray * array = nullptr;
                hr = SHCreateShellItemArray(nullptr,folder,
                    static_cast < UINT > (pointers.size()),pointers.data(),&array);
                if (SUCCEEDED(hr)) *out = array;
                else if (array) array->Release();
            } else {
                hr = folder->GetUIObjectOf(owner,static_cast < UINT > (pointers.size()),
                    pointers.data(),iid,nullptr,out);
            }
        }
    }
    for (PITEMID_CHILD pidl:selected) CoTaskMemFree(pidl);
    folder->Release();
    return hr;
}
static HRESULT NativeShellSelection(const Request & req,HWND owner,IDataObject *  * out) {
    return NativeShellUIObject(req,owner,IID_IDataObject,
                               reinterpret_cast < void *  *  > (out));
}
// First ask the provider to perform its native Copy context-menu verb. Some
// archive namespace providers publish a clipboard IDataObject different from
// their drag IDataObject (e.g. additional preferred-effect formats).
static HRESULT NativeShellContextCopy(const Request & req,HWND owner) {
    IContextMenu * menu = nullptr;
    HRESULT hr = NativeShellUIObject(req,owner,IID_IContextMenu,
                                    reinterpret_cast < void *  *  > ( & menu));
    if (FAILED(hr) || !menu) return FAILED(hr)?hr:E_NOINTERFACE;
    CMINVOKECOMMANDINFOEX cmd{};
    cmd.cbSize = sizeof(cmd);
    cmd.fMask = CMIC_MASK_UNICODE | CMIC_MASK_FLAG_NO_UI;
    cmd.hwnd = owner;
    cmd.lpVerb = "copy";
    cmd.lpVerbW = L"copy";
    cmd.nShow = SW_SHOWNORMAL;
    const DWORD before = GetClipboardSequenceNumber();
    hr = menu->InvokeCommand(reinterpret_cast < LPCMINVOKECOMMANDINFO > ( & cmd));
    menu->Release();
    if (FAILED(hr)) return hr;
    // Do not accept a successful InvokeCommand that did not publish anything.
    // The fallback will use the same provider's original IDataObject.
    const DWORD after = GetClipboardSequenceNumber();
    return after && after != before ? S_OK : HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
}
// Add the standard COPY drop effect if the native IDataObject accepts SetData.
// This is metadata in the Windows clipboard, not a custom transfer or cache.
static void NativeShellMarkClipboardCopy(IDataObject * data) {
    if (!data) return;
    const UINT format = RegisterClipboardFormatW(L"Preferred DropEffect");
    if (!format) return;
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE,sizeof(DWORD));
    if (!memory) return;
    void * value = GlobalLock(memory);
    if (!value) {GlobalFree(memory);return;}
    * static_cast < DWORD *  > (value) = DROPEFFECT_COPY;
    GlobalUnlock(memory);
    FORMATETC kind{static_cast < CLIPFORMAT > (format),nullptr,DVASPECT_CONTENT, - 1,TYMED_HGLOBAL};
    STGMEDIUM medium{};
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = memory;
    if (FAILED(data->SetData( & kind, & medium,TRUE))) GlobalFree(memory);
}
static HRESULT NativeShellClipboard(const Request & req,HWND owner) {
    if (CancelledForCurrentOperation())
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    HRESULT hr = NativeShellContextCopy(req,owner);
    if (SUCCEEDED(hr)) return hr;
    IDataObject * data = nullptr;
    hr = NativeShellSelection(req,owner, & data);
    if (FAILED(hr) || !data) return FAILED(hr)?hr:E_FAIL;
    if (CancelledForCurrentOperation()) {
        data->Release();
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    NativeShellMarkClipboardCopy(data);
    if (CancelledForCurrentOperation()) {
        data->Release();
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    hr = OleSetClipboard(data);
    // The broker has persistent STA workers: do NOT call OleFlushClipboard.
    // Flushing a large virtual archive selection can force complete and
    // unbounded materialization before the user even presses Ctrl+V.
    // Data stays available while this broker apartment remains alive. On mod
    // unload the deferred clipboard data may become unavailable, as documented.
    data->Release();
    return hr;
}

// These callbacks write only short, thread-safe progress snapshots; no Shell
// interfaces cross worker apartments. Implementation is below the operation
// record map to keep its ownership/lifetime unchanged.
// The two persistent STA workers set these thread-local pointers only while
// executing a request. Native IFileOperation callbacks normally run on that
// STA, so the watchdog sees genuine Shell activity, not a wall-clock timer.
static thread_local std::atomic < ULONGLONG >  * t_workerHeartbeat = nullptr;
static thread_local std::atomic < DWORD >  * t_workerProgressStage = nullptr;
static bool NativeProgressRecord(const GUID & id,DWORD stage,
    bool known = false,DWORD percent = 0,bool begin = false,
    bool updateEstimate = false);

// Source items still belong to the Shell namespace, not a mod-created
// cache or extractor. The sink only monitors IFileOperation notifications.
class NativeExtractionProgressSink final : public IFileOperationProgressSink {
    std::atomic < ULONG > refs_{1};
    GUID requestId_{};
    const std::atomic < bool >  * cancellation_{};
    HRESULT firstFailure_{S_OK};
    const bool diagnostics_{ArchivePreviewDiagnostics::Enabled()};
    ULONGLONG started_{diagnostics_?GetTickCount64():0};
    ULONGLONG lastCallbackLog_{};
    ULONGLONG preCopyCount_{};
    ULONGLONG postCopyCount_{};
    wchar_t requestText_[48]{};
    void LogCallback(PCWSTR name,ULONGLONG count,HRESULT result = S_OK) {
        const ULONGLONG now = GetTickCount64();
        if (count != 1 && now - lastCallbackLog_ < 2000) return;
        lastCallbackLog_ = now;
        Wh_Log(L"[AP extract] id=%s callback=%s count=%llu elapsed=%llu ms hr=0x%08lX",
            requestText_,name,static_cast<unsigned long long>(count),
            static_cast<unsigned long long>(now-started_),static_cast<unsigned long>(result));
    }
    HRESULT Check() const {
        return CancelledForCurrentOperation(cancellation_) ?
            HRESULT_FROM_WIN32(ERROR_CANCELLED) : S_OK;
    }
    HRESULT PostResult(HRESULT result) {
        NativeProgressRecord(requestId_,ARCHIVE_PROGRESS_EXTRACTING);
        if (FAILED(result) && SUCCEEDED(firstFailure_)) firstFailure_ = result;
        return FAILED(result)?result:Check();
    }
public:
    NativeExtractionProgressSink(const GUID & id,
        const std::atomic < bool >  * cancellation):
        requestId_(id),cancellation_(cancellation){
        if (diagnostics_) StringFromGUID2(id,requestText_,_countof(requestText_));
    }
    HRESULT Failure() const {return firstFailure_;}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void *  * out) override {
        if (!out) return E_POINTER;
        * out = nullptr;
        if (IsEqualIID(iid,IID_IUnknown) ||
            IsEqualIID(iid,IID_IFileOperationProgressSink)) {
            * out = static_cast < IFileOperationProgressSink *  > (this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {return refs_.fetch_add(1) + 1;}
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG remaining = refs_.fetch_sub(1) - 1;
        if (!remaining) delete this;
        return remaining;
    }
    HRESULT STDMETHODCALLTYPE StartOperations() override {
        if (diagnostics_) LogCallback(L"StartOperations",1);
        NativeProgressRecord(requestId_,ARCHIVE_PROGRESS_EXTRACTING);
        return Check();
    }
    HRESULT STDMETHODCALLTYPE FinishOperations(HRESULT result) override {
        if (diagnostics_) {
            LogCallback(L"FinishOperations",1,result);
            Wh_Log(L"[AP extract] id=%s copy callbacks pre=%llu post=%llu",
                requestText_,static_cast<unsigned long long>(preCopyCount_),
                static_cast<unsigned long long>(postCopyCount_));
        }
        if (FAILED(result) && SUCCEEDED(firstFailure_)) firstFailure_ = result;
        return S_OK; // do not overwrite the original Shell operation result
    }
    HRESULT STDMETHODCALLTYPE PreRenameItem(DWORD,IShellItem * ,LPCWSTR) override {return Check();}
    HRESULT STDMETHODCALLTYPE PostRenameItem(DWORD,IShellItem * ,LPCWSTR,HRESULT hr,IShellItem * ) override {return PostResult(hr);}
    HRESULT STDMETHODCALLTYPE PreMoveItem(DWORD,IShellItem * ,IShellItem * ,LPCWSTR) override {return Check();}
    HRESULT STDMETHODCALLTYPE PostMoveItem(DWORD,IShellItem * ,IShellItem * ,LPCWSTR,HRESULT hr,IShellItem * ) override {return PostResult(hr);}
    HRESULT STDMETHODCALLTYPE PreCopyItem(DWORD,IShellItem * ,IShellItem * ,LPCWSTR) override {
        if (diagnostics_) LogCallback(L"PreCopyItem",++preCopyCount_);
        NativeProgressRecord(requestId_,ARCHIVE_PROGRESS_EXTRACTING);
        return Check();
    }
    HRESULT STDMETHODCALLTYPE PostCopyItem(DWORD,IShellItem * ,IShellItem * ,LPCWSTR,HRESULT hr,IShellItem * ) override {
        if (diagnostics_) LogCallback(L"PostCopyItem",++postCopyCount_,hr);
        return PostResult(hr);
    }
    HRESULT STDMETHODCALLTYPE PreDeleteItem(DWORD,IShellItem * ) override {return Check();}
    HRESULT STDMETHODCALLTYPE PostDeleteItem(DWORD,IShellItem * ,HRESULT hr,IShellItem * ) override {return PostResult(hr);}
    HRESULT STDMETHODCALLTYPE PreNewItem(DWORD,IShellItem * ,LPCWSTR) override {return Check();}
    HRESULT STDMETHODCALLTYPE PostNewItem(DWORD,IShellItem * ,LPCWSTR,LPCWSTR,DWORD,HRESULT hr,IShellItem * ) override {return PostResult(hr);}
    HRESULT STDMETHODCALLTYPE UpdateProgress(UINT total,UINT soFar) override {
        // Windows reports proportional WORK UNITS, not byte counts. A zero
        // total is indeterminate. Do not invent a percentage or byte count.
        const DWORD percent = total ? static_cast < DWORD > (
            std::min < ULONGLONG > (100,(static_cast < ULONGLONG > (soFar) * 100) / total)) : 0;
        const ULONGLONG now = diagnostics_?GetTickCount64():0;
        if (diagnostics_ && (!lastCallbackLog_ || now - lastCallbackLog_ >= 2000)) {
            lastCallbackLog_ = now;
            Wh_Log(L"[AP extract] id=%s callback=UpdateProgress total=%u done=%u percent=%lu elapsed=%llu ms",
                requestText_,total,soFar,percent,static_cast<unsigned long long>(now-started_));
        }
        NativeProgressRecord(requestId_,ARCHIVE_PROGRESS_EXTRACTING,
                             total != 0,percent,false,true);
        return Check();
    }
    HRESULT STDMETHODCALLTYPE ResetTimer() override {return S_OK;}
    HRESULT STDMETHODCALLTYPE PauseTimer() override {return S_OK;}
    HRESULT STDMETHODCALLTYPE ResumeTimer() override {return S_OK;}
};

static HRESULT NativeShellExtract(const Request & req) {
    if (!SafeArchivePath(req.archive,true)) return E_INVALIDARG;
    const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
    wchar_t traceId[48]{};
    if (diagnostics) StringFromGUID2(req.requestId,traceId,_countof(traceId));
    const ULONGLONG extractionStarted = diagnostics?GetTickCount64():0;
    auto step = [&](PCWSTR name,auto action)->HRESULT {
        if (!diagnostics) return action();
        const ULONGLONG began = GetTickCount64();
        Wh_Log(L"[AP extract] id=%s step=%s begin elapsed=%llu ms",
            traceId,name,static_cast<unsigned long long>(began-extractionStarted));
        const HRESULT result = action();
        Wh_Log(L"[AP extract] id=%s step=%s end duration=%llu ms hr=0x%08lX",
            traceId,name,static_cast<unsigned long long>(GetTickCount64()-began),
            static_cast<unsigned long>(result));
        return result;
    };
    const bool choosingDestination = req.command == ARCHIVE_BROKER_EXTRACT_TO ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO;
    if (choosingDestination)
        NativeProgressRecord(req.requestId,ARCHIVE_PROGRESS_PICKING);
    IShellItem * dest = nullptr;
    HRESULT hr = E_FAIL;
    if (req.command == ARCHIVE_BROKER_EXTRACT_HERE ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE) {
        const std::wstring directory = ParentFolder(req.archive);
        hr = directory.empty()?E_INVALIDARG:DirectoryItem(directory, & dest);
    } else hr = PickFolder( & dest,ParentFolder(req.archive));
    if (FAILED(hr) || !dest) {if(dest)dest->Release();return FAILED(hr)?hr:E_FAIL;}
    // No confirmation dialog. The chosen Windows provider performs the copy.
    // Notify the existing footer only after the folder picker has returned.
    NativeProgressRecord(req.requestId,ARCHIVE_PROGRESS_SCANNING,false,0,true);
    if (CancelledForCurrentOperation()) {
        dest->Release();return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    IShellItemArray * data = nullptr;
    hr = step(L"PrepareShellItems",[&] {
        return NativeShellUIObject(req,nullptr,IID_IShellItemArray,
            reinterpret_cast < void *  * > (&data));
    });
    if (SUCCEEDED(hr) && !data) hr = E_FAIL;
    if (SUCCEEDED(hr)) {
        if (CancelledForCurrentOperation()) hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
        else {
            IFileOperation * operation = nullptr;
            hr = CoCreateInstance(CLSID_FileOperation,nullptr,CLSCTX_ALL,
                                IID_IFileOperation,reinterpret_cast < void *  *  > ( & operation));
            if (SUCCEEDED(hr) && !operation) hr = E_FAIL;
            if (SUCCEEDED(hr)) {
                NativeExtractionProgressSink * sink = new(std::nothrow)
                    NativeExtractionProgressSink(req.requestId,
                                                  t_operationCancelFlag);
                if (!sink) hr = E_OUTOFMEMORY;
                else {
                    DWORD cookie = 0;
                    hr = operation->Advise(sink, & cookie);
                    if (SUCCEEDED(hr)) {
                        hr = operation->SetOperationFlags(FOF_ALLOWUNDO);
                        if (SUCCEEDED(hr)) hr = step(L"QueueCopyItems",[&] {
                            return operation->CopyItems(data,dest);
                        });
                        if (SUCCEEDED(hr) && CancelledForCurrentOperation())
                            hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                        if (SUCCEEDED(hr)) {
                            NativeProgressRecord(req.requestId,ARCHIVE_PROGRESS_STARTING,
                                false,0,false,true);
                            hr = step(L"PerformOperations",[&] {
                                return operation->PerformOperations();
                            });
                            // Per Microsoft, query aborted even if PerformOperations
                            // failed. S_OK is not proof that every file copied.
                            BOOL aborted = FALSE;
                            const HRESULT abortedHr =
                                operation->GetAnyOperationsAborted( & aborted);
                            if (SUCCEEDED(hr) && FAILED(sink->Failure()))
                                hr = sink->Failure();
                            if (SUCCEEDED(abortedHr) && aborted && SUCCEEDED(hr))
                                hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                            if (CancelledForCurrentOperation() && SUCCEEDED(hr))
                                hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                        }
                        operation->Unadvise(cookie);
                    }
                    sink->Release();
                }
                operation->Release();
            }
        }
    }
    if (data) data->Release();
    dest->Release();
    return hr;
}
// External extraction runs directly as the normal user. Arguments are never
// passed through cmd.exe, a command template or Shell association lookup.
static std::wstring ExternalQuoteArgument(const std::wstring & value) {
    std::wstring result = L"\"";
    size_t slashes = 0;
    for (wchar_t ch:value) {
        if (ch == L'\\') {++slashes;continue;}
        if (ch == L'"') {
            result.append(slashes * 2 + 1,L'\\');
            result += ch;
        } else {
            result.append(slashes,L'\\');
            result += ch;
        }
        slashes = 0;
    }
    result.append(slashes * 2,L'\\');
    result += L'"';
    return result;
}
static bool ExternalAbsolutePath(const std::wstring & path) {
    return path.size() >= 3 && ((path[0] >= L'A' && path[0] <= L'Z') ||
        (path[0] >= L'a' && path[0] <= L'z')) && path[1] == L':' &&
        (path[2] == L'\\' || path[2] == L'/') &&
        path.find(L'\0') == std::wstring::npos;
}
static int ExternalExecutableKind(const std::wstring & path) {
    const wchar_t * name = PathFindFileNameW(path.c_str());
    if (_wcsicmp(name,L"7zG.exe") == 0) return 2;
    if (_wcsicmp(name,L"NanaZipG.exe") == 0 ||
        _wcsicmp(name,L"NanaZip.Universal.Windows.exe") == 0) return 3;
    if (_wcsicmp(name,L"WinRAR.exe") == 0) return 4;
    return 0;
}
static std::wstring ExternalExpandedPath(std::wstring path) {
    const size_t first = path.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    path = path.substr(first,path.find_last_not_of(L" \t\r\n")-first+1);
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"')
        path = path.substr(1,path.size()-2);
    const DWORD needed = ExpandEnvironmentStringsW(path.c_str(),nullptr,0);
    if (!needed || needed > 32700) return {};
    std::vector<wchar_t> expanded(needed);
    const DWORD written = ExpandEnvironmentStringsW(path.c_str(),expanded.data(),needed);
    if (!written || written > needed) return {};
    return expanded.data();
}
static bool ExternalExistingExecutable(const std::wstring & path,int kind) {
    if (!ExternalAbsolutePath(path) || ExternalExecutableKind(path) != kind) return false;
    const DWORD attrs = GetFileAttributesW(path.c_str());
    // App execution aliases are reparse points. Do not resolve them into a
    // protected WindowsApps package path or mistake them for directories.
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}
static std::wstring ExternalFindExecutable(int kind) {
    std::vector<std::wstring> candidates;
    const wchar_t * relative = kind == 2?L"7-Zip\\7zG.exe":L"WinRAR\\WinRAR.exe";
    if (kind == 2 || kind == 4) {
        for (const KNOWNFOLDERID * folder:{&FOLDERID_ProgramFiles,&FOLDERID_ProgramFilesX86}) {
            PWSTR base = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(*folder,KF_FLAG_DEFAULT,nullptr,&base)) && base)
                candidates.push_back(std::wstring(base)+L"\\"+relative);
            CoTaskMemFree(base);
        }
        // Read existing installation registrations; never modify them.
        if (kind == 2) {
            for (DWORD view:{KEY_WOW64_64KEY,KEY_WOW64_32KEY}) {
                HKEY key = nullptr;
                if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\7-Zip",0,KEY_QUERY_VALUE | view,&key) != ERROR_SUCCESS) continue;
                wchar_t value[32768]{};DWORD bytes = sizeof(value),type = 0;
                if (RegQueryValueExW(key,L"Path",nullptr,&type,reinterpret_cast<BYTE*>(value),&bytes) == ERROR_SUCCESS &&
                    (type == REG_SZ || type == REG_EXPAND_SZ) && bytes < sizeof(value)) {
                    std::wstring directory = ExternalExpandedPath(value);
                    if (!directory.empty()) candidates.push_back(directory+L"\\7zG.exe");
                }
                RegCloseKey(key);
            }
        } else {
            for (HKEY root:{HKEY_CURRENT_USER,HKEY_LOCAL_MACHINE}) {
                wchar_t value[32768]{};DWORD bytes = sizeof(value);
                if (RegGetValueW(root,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\WinRAR.exe",
                    nullptr,RRF_RT_REG_SZ,nullptr,value,&bytes) == ERROR_SUCCESS && bytes < sizeof(value)) {
                    auto path = ExternalExpandedPath(value);
                    // App Paths can register the executable without its .exe suffix.
                    if (_wcsicmp(PathFindFileNameW(path.c_str()),L"WinRAR") == 0) path += L".exe";
                    candidates.push_back(std::move(path));
                }
            }
        }
    } else if (kind == 3) {
        candidates.push_back(ExternalExpandedPath(L"%LOCALAPPDATA%\\Microsoft\\WindowsApps\\NanaZipG.exe"));
    }
    for (const auto & candidate:candidates)
        if (ExternalExistingExecutable(candidate,kind)) return candidate;
    return {};
}
enum class ExternalDragLeaseStatus : DWORD { Idle, Launching, Running };
static constexpr DWORD kExternalDragLeaseSignature = 0x31445041; // APD1
struct ExternalDragLeaseRecord {
    DWORD signature{};
    ExternalDragLeaseStatus status{};
    DWORD pid{};
    FILETIME created{};
};
static bool WriteExternalDragLease(HANDLE file,ExternalDragLeaseStatus status,
                                   DWORD pid = 0,FILETIME created = {}) {
    const ExternalDragLeaseRecord record{kExternalDragLeaseSignature,status,pid,created};
    LARGE_INTEGER start{};
    DWORD written = 0;
    return SetFilePointerEx(file,start,nullptr,FILE_BEGIN) &&
        WriteFile(file,&record,sizeof(record),&written,nullptr) &&
        written == sizeof(record) && FlushFileBuffers(file);
}
static HRESULT ExternalShellExtract(const Request & req,
    const ArchivePreviewOptions::ExtractionOptions & options,
    const std::wstring & forcedDestination = {},HANDLE dragLease = nullptr) {
    if (!SafeArchivePath(req.archive,true) || req.items.size() > ARCHIVE_BROKER_MAX_PATHS)
        return E_INVALIDARG;
    for (const auto & item:req.items) if (!SafeRelativePath(item)) return E_INVALIDARG;
    int kind = options.backend;
    const PCWSTR resolution = !options.executable.empty() ? L"manual" : kind == 1 ? L"auto" : L"installed";
    std::wstring executable;
    if (!options.executable.empty()) {
        executable = ExternalExpandedPath(options.executable);
        const int detected = ExternalExecutableKind(executable);
        if (!detected || (kind != 1 && kind != detected) ||
            !ExternalExistingExecutable(executable,detected)) {
            Wh_Log(L"[AP external] manual path rejected configured_program=%d detected_program=%d",kind,detected);
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
        kind = detected;
    } else if (kind == 1) {
        for (int candidate:{2,3,4}) {
            executable = ExternalFindExecutable(candidate);
            if (!executable.empty()) {kind = candidate;break;}
        }
    } else executable = ExternalFindExecutable(kind);
    if (executable.empty()) {
        Wh_Log(L"[AP external] program not found resolution=%s configured_program=%d",resolution,options.backend);
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    const PCWSTR program = kind == 2 ? L"7-Zip" : kind == 3 ? L"NanaZip" : L"WinRAR";
    const bool choose = forcedDestination.empty() && (req.command == ARCHIVE_BROKER_EXTRACT_TO ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO);
    std::wstring destination = forcedDestination.empty()?ParentFolder(req.archive):forcedDestination;
    if (choose) {
        NativeProgressRecord(req.requestId,ARCHIVE_PROGRESS_PICKING);
        IShellItem * folder = nullptr;
        HRESULT picked = PickFolder(&folder,destination);
        if (SUCCEEDED(picked) && folder) {
            PWSTR path = nullptr;
            picked = folder->GetDisplayName(SIGDN_FILESYSPATH,&path);
            if (SUCCEEDED(picked) && path) destination = path;
            else if (SUCCEEDED(picked)) picked = E_FAIL;
            CoTaskMemFree(path);
        } else if (SUCCEEDED(picked)) picked = E_FAIL;
        if (folder) folder->Release();
        if (FAILED(picked)) return picked;
    }
    if (destination.empty() || CancelledForCurrentOperation())
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    std::vector<std::wstring> arguments{executable,L"x"};
    if (kind == 4) {
        // Include filters are full archive-relative paths. Also include a
        // selected directory's descendants without broadening to other roots.
        for (const auto & item:req.items) {
            if (item.front() == L'@') return E_INVALIDARG; // WinRAR listfile syntax
            arguments.push_back(L"-n"+item);
            arguments.push_back(L"-n"+item+L"\\*");
        }
        arguments.push_back(L"--");
        arguments.push_back(req.archive);
        if (destination.back() != L'\\') destination += L'\\';
        arguments.push_back(destination);
    } else {
        arguments.push_back(L"-o"+destination);
        arguments.push_back(L"-spd"); // interpret selection names literally
        for (const auto & item:req.items) arguments.push_back(L"-i!"+item);
        arguments.push_back(L"--");
        arguments.push_back(req.archive);
    }
    std::wstring command;
    for (const auto & arg:arguments) {
        if (!command.empty()) command += L' ';
        command += ExternalQuoteArgument(arg);
    }
    // Do not split a selection into multiple partial extraction operations.
    if (command.size() >= 32700) return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    if (CancelledForCurrentOperation()) return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    STARTUPINFOW startup{};startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;startup.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION child{};
    const std::wstring working = ParentFolder(executable);
    // Commit the handoff under the same lock as broker cancellation. An
    // accepted cancel must prevent launch. After the handoff the extractor owns it.
    if (!NativeProgressRecord(req.requestId,ARCHIVE_PROGRESS_EXTERNAL,false,0,true,true))
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    // Persist an uncertain state before launch. Recovery must retain a stage
    // if Explorer exits before the extractor's identity can be recorded.
    if (dragLease && !WriteExternalDragLease(dragLease,ExternalDragLeaseStatus::Launching))
        return E_FAIL;
    if (CancelledForCurrentOperation()) {
        if (dragLease) WriteExternalDragLease(dragLease,ExternalDragLeaseStatus::Idle);
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    // Pin the executable in lpApplicationName and do not inherit broker handles.
    if (!CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,0,
        nullptr,working.empty()?nullptr:working.c_str(),&startup,&child)) {
        const DWORD error = GetLastError();
        if (dragLease) WriteExternalDragLease(dragLease,ExternalDragLeaseStatus::Idle);
        Wh_Log(L"[AP external] launch failed program=%s resolution=%s Win32=%lu",program,resolution,error);
        return HRESULT_FROM_WIN32(error);
    }
    CloseHandle(child.hThread);
    if (dragLease) {
        FILETIME created{},exited{},kernel{},user{};
        if (GetProcessTimes(child.hProcess,&created,&exited,&kernel,&user))
            WriteExternalDragLease(dragLease,ExternalDragLeaseStatus::Running,child.dwProcessId,created);
    }
    Wh_Log(L"[AP external] started program=%s resolution=%s pid=%lu selected=%zu",program,resolution,child.dwProcessId,req.items.size());
    HRESULT result = E_FAIL;
    for (;;) {
        const DWORD wake = MsgWaitForMultipleObjects(1,&child.hProcess,FALSE,250,QS_ALLINPUT);
        if (wake == WAIT_OBJECT_0) {
            DWORD code = 0;
            if (dragLease) WriteExternalDragLease(dragLease,ExternalDragLeaseStatus::Idle);
            if (!GetExitCodeProcess(child.hProcess,&code)) result = HRESULT_FROM_WIN32(GetLastError());
            else {
                Wh_Log(L"[AP external] finished pid=%lu exit=%lu",child.dwProcessId,code);
                result = code == 0?S_OK:(code == 255?HRESULT_FROM_WIN32(ERROR_CANCELLED):
                    (code == 1?HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY):E_FAIL));
            }
            break;
        }
        if (wake == WAIT_FAILED) {result = HRESULT_FROM_WIN32(GetLastError());break;}
        if (wake == WAIT_OBJECT_0 + 1) {
            MSG message{};
            while (!CancelledForCurrentOperation() &&
                   PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) {
                TranslateMessage(&message);DispatchMessageW(&message);
            }
        }
        if (g_cancelForShutdown.load(std::memory_order_acquire)) {
            // Independent user application continues safely after mod unload.
            // Never terminate the extractor while it is writing user files.
            Wh_Log(L"[AP external] mod shutdown; extractor pid=%lu continues independently",child.dwProcessId);
            result = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;
        }
    }
    CloseHandle(child.hProcess);
    return result;
}
static HRESULT NativeShellOpen(const Request & req) {
    if (req.items.size() != 1 || !SafeArchivePath(req.archive,true) ||
        !SafeRelativePath(req.items.front())) return E_INVALIDARG;
    // The Shell's verb may internally prepare files; avoid executing active
    // content directly from a compressed namespace.
    const std::wstring extension = OpenLowerExtension(req.items.front());
    if (IsDangerousOpenExtension(extension) || extension == L".exe" ||
        extension == L".msi" || extension == L".bat" || extension == L".cmd" ||
        extension == L".ps1" || extension == L".scr" || extension == L".lnk" ||
        extension == L".vbs" || extension == L".js")
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    IShellItem * root = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(req.archive.c_str(),nullptr,
                    IID_IShellItem,reinterpret_cast < void *  *  > ( & root));
    if (FAILED(hr) || !root) return FAILED(hr)?hr:E_FAIL;
    std::vector < IShellItem *  > selected;
    hr = ResolveSelectedItems(root,req.items,selected);
    if (SUCCEEDED(hr) && selected.size() == 1) {
        PIDLIST_ABSOLUTE absolute = nullptr;
        hr = SHGetIDListFromObject(selected[0], & absolute);
        if (SUCCEEDED(hr) && absolute) {
            SHELLEXECUTEINFOW run{};run.cbSize = sizeof(run);
            run.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
            run.lpIDList = absolute;run.nShow = SW_SHOWNORMAL;
            if (!ShellExecuteExW( & run)) hr = HRESULT_FROM_WIN32(GetLastError());
            CoTaskMemFree(absolute);
        } else if (SUCCEEDED(hr)) hr = E_FAIL;
    }
    ReleaseItems(selected);
    root->Release();
    return hr;
}
// === END NATIVE SHELL BACKEND ===

// Minimal path, selection, archive-identity and OLE-drag helpers retained.
static HRESULT DirectoryItem(const std::wstring & path,IShellItem *  * target) {
    return SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(target));
}
static std::wstring ParentFolder(const std::wstring & path) {
    const auto sep = path.find_last_of(L"\\/");
    if (sep == std::wstring::npos) return {};
    if (sep == 2 && path[1] == L':') return path.substr(0,sep + 1);
    return path.substr(0,sep);
}
static HRESULT PickFolder(IShellItem *  * selected,const std::wstring & initialDirectory) {
    * selected = nullptr;
    IFileOpenDialog * picker = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS( & picker));
    if (FAILED(hr)) return hr;
    FILEOPENDIALOGOPTIONS opts{};
    hr = picker->GetOptions( & opts);
    if (SUCCEEDED(hr)) hr = picker->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
    if (SUCCEEDED(hr)) hr = picker->SetTitle(ArchivePreviewText::Get(ArchivePreviewText::Id::PickFolder));
    IShellItem * initial = nullptr;
    if (SUCCEEDED(hr)) hr = DirectoryItem(initialDirectory,&initial);
    if (SUCCEEDED(hr)) hr = initial?picker->SetFolder(initial):E_FAIL;
    if (initial) initial->Release();
    // A topmost, non-activating modal owner keeps the picker visible when the
    // broker is running in the background without a taskbar window.
    HWND owner = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"STATIC",
        L"Archive Preview", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (SUCCEEDED(hr)) hr = picker->Show(owner);
    if (SUCCEEDED(hr)) hr = picker->GetResult(selected);
    if (owner) DestroyWindow(owner);
    picker->Release();
    return hr;
}
struct ArchiveFileIdentity {
    DWORD volume{};
    DWORD indexHigh{};
    DWORD indexLow{};
    // NTFS change time normally advances even if a writer restores mtime.
    // A zero value means the filesystem/API did not expose this signal.
    ULONGLONG changeTime{};
    bool operator == (const ArchiveFileIdentity & ) const = default;
};
static bool GetArchiveSignature(const std::wstring & path,FILETIME & modified,
                                ULONGLONG & bytes,ArchiveFileIdentity * identity = nullptr) {
    // Read metadata and file identity through the same handle, rather than
    // separate path-based queries that could race a file replacement.
    HANDLE handle = CreateFileW(path.c_str(),FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
    if(handle == INVALID_HANDLE_VALUE)return false;
    BY_HANDLE_FILE_INFORMATION stat{};
    const bool ok = GetFileInformationByHandle(handle, & stat) != FALSE &&
        !(stat.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
    ULONGLONG changeTime = 0;
    if(ok && identity) {
        FILE_BASIC_INFO basic{};
        if(GetFileInformationByHandleEx(handle,FileBasicInfo, & basic,sizeof(basic)))
            changeTime = static_cast < ULONGLONG > (basic.ChangeTime.QuadPart);
    }
    CloseHandle(handle);
    if(!ok)return false;
    modified = stat.ftLastWriteTime;
    bytes = (static_cast < ULONGLONG > (stat.nFileSizeHigh) << 32) | stat.nFileSizeLow;
    if(identity) * identity = {stat.dwVolumeSerialNumber,stat.nFileIndexHigh,stat.nFileIndexLow,changeTime};
    return true;
}
static bool SafeArchiveLeaf(const std::wstring & name) {
    if (name.empty() || name.size() > 255 || name == L"." || name == L".." ||
        name.back() == L'.' || name.back() == L' ' ||
        name.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos) return false;
    for (wchar_t c:name) {
        // Windows control codes and Unicode bidi controls can spoof or alias
        // an executable name; reject them rather than misrepresent it.
        if (c < 32 || (c >= 127 && c <= 159) || c == 0x061c ||
            (c >= 0x200e && c <= 0x200f) || (c >= 0x202a && c <= 0x202e) ||
            (c >= 0x2066 && c <= 0x2069)) return false;
    }
    std::wstring upper = ShellLookupKey(name);
    // Win32 reserves COM/LPT device names with superscript 1/2/3 too.
    for (wchar_t & c:upper) {
        if (c == 0x00b9) c = L'1';
        else if (c == 0x00b2) c = L'2';
        else if (c == 0x00b3) c = L'3';
    }
    auto dot = upper.find(L'.');
    if (dot != std::wstring::npos) upper.resize(dot);
    if (upper == L"con" || upper == L"prn" || upper == L"aux" || upper == L"nul" ||
        upper == L"conin$" || upper == L"conout$" || upper == L"clock$") return false;
    if (upper.size() == 4 && (upper.rfind(L"com",0) == 0 || upper.rfind(L"lpt",0) == 0) &&
        upper[3] >= L'0' && upper[3] <= L'9') return false;
    return true;
}
static std::wstring OpenLowerExtension(const std::wstring & path) {
    const auto dot = path.find_last_of(L'.');
    const auto slash = path.find_last_of(L"\\/");
    if (dot == std::wstring::npos || (slash != std::wstring::npos && dot < slash)) return {};
    std::wstring ext = path.substr(dot);
    std::transform(ext.begin(),ext.end(),ext.begin(),[](wchar_t c){return static_cast < wchar_t > (towlower(c));});
    return ext;
}
static bool IsDangerousOpenExtension(const std::wstring & ext) {
    // Block executable content, scripts, shortcuts, installers and registration
    // files from being launched via their default Windows Shell verb.
    static constexpr const wchar_t * blocked[] = {
        L".bat",L".cmd",L".ps1",L".psm1",L".vbs",L".vbe",L".js",L".jse",
        L".wsf",L".wsh",L".hta",L".msi",L".msp",L".msix",L".appx",
        L".scr",L".com",L".pif",L".lnk",L".url",L".reg",L".cpl",
        L".msc",L".dll",L".sys",L".jar",L".chm",L".iso",L".vhd",
        L".vhdx",L".scf",L".sct",L".inf"};
    for (auto * entry:blocked) if (ext == entry) return true;
    return false;
}
class MediumDragSource final : public IDropSource {
    std::atomic < ULONG > refs_{1};
    const ULONGLONG started_ = GetTickCount64();
public:
    DWORD queries{};
    DWORD feedback{};
    DWORD firstKeys{};
    DWORD lastKeys{};
    DWORD lastFeedback{};
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void *  * out) override {
        if (!out) return E_POINTER;
        * out = nullptr;
        if (iid == IID_IUnknown || iid == IID_IDropSource) {
            * out = static_cast < IDropSource *  > (this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return refs_.fetch_add(1) + 1; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG n = refs_.fetch_sub(1) - 1;
        if (!n) delete this;
        return n;
    }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD keys) override {
        if (!queries) firstKeys = keys;
        ++queries;
        lastKeys = keys;
        if (escape || CancelledForCurrentOperation() ||
            GetTickCount64() - started_ >= 5ULL * 60 * 1000)
            return DRAGDROP_S_CANCEL;
        // The popup owns the physical gesture, so OLE's button state is valid.
        // Never combine this with stale cross-process state.
        return (keys & MK_LBUTTON) ? S_OK : DRAGDROP_S_DROP;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override {
        ++feedback;
        lastFeedback = effect;
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
};


// Broker-owned real ListView, reparented inside the existing low-integrity
// preview container. No transparent overlay or separate drag button.
static HRESULT DispatchAsync(const Request & req,DWORD ownerPid = 0);
static constexpr UINT WM_BROWSER_CHANGE = WM_APP + 67;
static constexpr UINT WM_BROWSER_REAP = WM_APP + 68;
static constexpr UINT_PTR kBrowserTimer = 2;
static constexpr UINT_PTR kBrowserSaveTimer = 4; // coalesce selection changes before preview unload
static constexpr UINT_PTR kBrowserHeaderRepairTimer = 5; // one-shot after Shell refresh
static constexpr UINT_PTR kBrowserListSettleTimer = 13; // one-shot repaint after ListView owner-data churn
static constexpr UINT_PTR kBrowserModelRetryTimer = 7; // queue backpressure, not fallback
static constexpr UINT_PTR kBrowserHeavyDebounceTimer = 11; // coalesce rapid 10 GiB selection changes
static constexpr UINT kBrowserHeavyDebounceMs = 550;
static constexpr wchar_t kBrowserManagerClass[] = L"ArchivePreview.BrowserManager.0607";
static constexpr wchar_t kBrowserHostClass[] = L"ArchivePreview.BrokerListHost.0607";
static constexpr wchar_t kBrowserHeaderClass[] = L"ArchivePreview.BrokerHeader.0701";
static constexpr wchar_t kBrowserFooterClass[] = L"ArchivePreview.BrokerFooter.0701";
static constexpr UINT WM_BROWSER_HEADER_REFRESH = WM_APP + 69;
static constexpr UINT WM_BROWSER_MODEL_READY = WM_APP + 70;
static constexpr UINT WM_BROWSER_LOCAL_EXTRACT_DONE = WM_APP + 71;
static constexpr UINT WM_BROWSER_SETTINGS_CHANGED = WM_APP + 72;
static constexpr UINT WM_BROWSER_MODEL_PREVIEW = WM_APP + 74; // early, partial rows
static constexpr UINT WM_BROWSER_COLUMN_PREFERENCES_CHANGED = WM_APP + 75;
static constexpr UINT_PTR kBrowserExtractStatusTimer = 8;
static constexpr UINT_PTR kBrowserProgressTimer = 9;
static constexpr UINT_PTR kBrowserParkSweepTimer = 10;
static constexpr ULONGLONG kBrowserParkTtlMs = 4000;
static constexpr size_t kBrowserParkMax = 4;
static constexpr int kBrowserCancelButtonId = 405;
// A 2-second delay starts only after the broker begins real processing.
// Folder picker time and time waiting in the queue do not count.
struct BrowserProgressSnapshot {
    bool active{};
    DWORD stage{ARCHIVE_PROGRESS_NONE};
    DWORD percent{};
    bool known{};
    bool cancelling{};
    ULONGLONG workStarted{};
    GUID requestId{};
};
static BrowserProgressSnapshot BrowserQueryProgress(const std::wstring & archive,DWORD ownerPid);
struct BrowserLocalExtractResult { GUID requestId{}; HRESULT outcome{E_FAIL}; DWORD command{}; };
// Completion is queried by the originating request GUID, never broadcast.
// Stable case-insensitive archive key for notifications without a payload.
static WPARAM BrowserArchiveKey(const std::wstring & path) {
    uint64_t hash = 14695981039346656037ULL;
    for (wchar_t c:path) {
        hash ^= static_cast < uint64_t > (towlower(c));
        hash *= 1099511628211ULL;
    }
    return static_cast < WPARAM > (hash);
}
static constexpr wchar_t kPreviewHostClass[] = L"ArchivePreview.RealHost.0607";
static std::atomic < HWND > g_gripManager{nullptr};
static std::atomic < bool > g_gripReady{false};
static std::mutex g_browserPostMutex;
template<typename T>
static HRESULT PostBrowserPayload(UINT message,WPARAM param,std::unique_ptr<T> payload) {
    std::lock_guard<std::mutex> lock(g_browserPostMutex);
    if (g_cancelForShutdown.load(std::memory_order_acquire))
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    const HWND manager = g_gripManager.load(std::memory_order_acquire);
    if (!manager) return HRESULT_FROM_WIN32(ERROR_NOT_READY);
    if (!PostMessageW(manager,message,param,reinterpret_cast<LPARAM>(payload.get())))
        return HRESULT_FROM_WIN32(GetLastError());
    payload.release();
    return S_OK;
}

struct BrowserEntry {
    std::wstring name;
    std::wstring relative;
    bool folder{};
    int icon{ - 1};
    std::shared_ptr < ArchivePreviewIcons::Icon > iconImage;
    std::wstring sizeLabel;
    std::wstring typeLabel;
    std::wstring modifiedLabel;
    ULONGLONG modifiedValue{};
    ULONGLONG sizeBytes{};
    bool hasSize{};
};
// Column order is stable even when any of the three optional columns is hidden.
enum class BrowserColumnKind : unsigned char { Name, DateModified, Type, Size };
// Cached GDI objects for the row/header palette.
// One shared set per browser replaces per-row Create/DeleteObject calls.
// The RGB values, custom-draw geometry and selection behavior are unchanged.
// Reuse the small header/footer GDI backing stores across WM_PAINT.
// All create/paint/destroy calls run on the same broker UI thread.
struct BrowserSurfaceBuffer {
    HDC memory{};
    HBITMAP bitmap{};
    HGDIOBJ original{};
    int width{};
    int height{};
    BrowserSurfaceBuffer() = default;
    BrowserSurfaceBuffer(const BrowserSurfaceBuffer & ) = delete;
    BrowserSurfaceBuffer & operator = (const BrowserSurfaceBuffer & ) = delete;
    ~BrowserSurfaceBuffer() { Reset(); }
    void Reset() {
        if (memory) {
            if (original && original != HGDI_ERROR) SelectObject(memory,original);
            if (bitmap) DeleteObject(bitmap);
            DeleteDC(memory);
        }
        memory = nullptr; bitmap = nullptr; original = nullptr;
        width = height = 0;
    }
    HDC Ensure(HDC target,int w,int h) {
        if (!target || w <= 0 || h <= 0) return nullptr;
        if (memory && bitmap && width == w && height == h) return memory;
        Reset();
        memory = CreateCompatibleDC(target);
        if (memory) bitmap = CreateCompatibleBitmap(target,w,h);
        if (bitmap) original = SelectObject(memory,bitmap);
        if (!memory || !bitmap || !original || original == HGDI_ERROR) {
            Reset();return nullptr;
        }
        width = w;height = h;
        return memory;
    }
};
struct BrowserPaintCache {
    HBRUSH background{};
    HBRUSH selection{};
    HBRUSH hover{};
    HBRUSH header{};
    HBRUSH footer{}; // no GDI brush allocation on every footer paint
    HPEN selectionPen{};
    HPEN hoverPen{};
    HPEN headerDivider{};
    HPEN footerDivider{};
    bool dark{};
    bool highContrast{};
    bool ready{};
    COLORREF systemWindow{},systemHighlight{},systemButtonFace{},systemWindowText{};
    COLORREF accent{};
    COLORREF sourceBackground{CLR_INVALID},sourceForeground{CLR_INVALID};
    COLORREF backgroundColor{},foregroundColor{},panelColor{},mutedColor{},borderColor{},selectedText{};
    void Reset() {
        if (background) DeleteObject(background);
        if (selection) DeleteObject(selection);
        if (hover) DeleteObject(hover);
        if (header) DeleteObject(header);
        if (footer) DeleteObject(footer);
        if (selectionPen) DeleteObject(selectionPen);
        if (hoverPen) DeleteObject(hoverPen);
        if (headerDivider) DeleteObject(headerDivider);
        if (footerDivider) DeleteObject(footerDivider);
        background = selection = hover = header = footer = nullptr;
        selectionPen = hoverPen = headerDivider = footerDivider = nullptr;
        ready = false;
    }
    ~BrowserPaintCache() { Reset(); }
    BrowserPaintCache() = default;
    BrowserPaintCache(const BrowserPaintCache & ) = delete;
    BrowserPaintCache & operator = (const BrowserPaintCache & ) = delete;
};
static void BrowserCachePaintColors(BrowserPaintCache & p,bool dark,bool highContrast=false,
    COLORREF hostBg=CLR_INVALID,COLORREF hostText=CLR_INVALID) {
    const COLORREF accent=ArchivePreviewHost::SystemAccentColor(highContrast);
    if(p.ready && p.dark==dark && p.highContrast==highContrast && p.accent==accent &&
        p.sourceBackground==hostBg && p.sourceForeground==hostText &&
        (!highContrast || (p.systemWindow==GetSysColor(COLOR_WINDOW) &&
            p.systemHighlight==GetSysColor(COLOR_HIGHLIGHT) &&
            p.systemButtonFace==GetSysColor(COLOR_BTNFACE) && p.systemWindowText==GetSysColor(COLOR_WINDOWTEXT))))return;
    p.Reset();
    const bool supplied=hostBg!=CLR_INVALID && hostText!=CLR_INVALID;
    const auto palette=ArchivePreviewHost::ResolvePalette(nullptr,hostBg,hostText,supplied,supplied,dark?1:0);
    p.sourceBackground=hostBg;p.sourceForeground=hostText;
    p.backgroundColor=palette.bg;p.foregroundColor=palette.fg;
    p.panelColor=highContrast?GetSysColor(COLOR_WINDOW):palette.panel;
    p.mutedColor=highContrast?palette.fg:palette.muted;
    p.borderColor=highContrast?palette.fg:palette.hairline;p.selectedText=palette.selectedText;
    p.accent=accent;
    if(highContrast){
        p.systemWindow=GetSysColor(COLOR_WINDOW);p.systemHighlight=GetSysColor(COLOR_HIGHLIGHT);
        p.systemButtonFace=GetSysColor(COLOR_BTNFACE);p.systemWindowText=GetSysColor(COLOR_WINDOWTEXT);
    }
    p.background=CreateSolidBrush(p.backgroundColor);
    p.selection=CreateSolidBrush(accent);
    p.hover=CreateSolidBrush(highContrast?GetSysColor(COLOR_BTNFACE):p.panelColor);
    p.header=CreateSolidBrush(p.panelColor);p.footer=CreateSolidBrush(p.panelColor);
    p.selectionPen=CreatePen(PS_SOLID,1,accent);
    p.hoverPen=CreatePen(PS_SOLID,1,highContrast?GetSysColor(COLOR_BTNFACE):p.panelColor);
    p.headerDivider=CreatePen(PS_SOLID,1,p.borderColor);p.footerDivider=CreatePen(PS_SOLID,1,p.borderColor);
    p.dark=dark;p.highContrast=highContrast;
    p.ready=p.background && p.selection && p.hover && p.header && p.footer &&
        p.selectionPen && p.hoverPen && p.headerDivider && p.footerDivider;
    if(!p.ready)p.Reset();
}
struct BrowserState {
    HWND anchor{};
    HWND parent{};
    HWND host{};
    HWND header{}; // independent broker-owned surface for Name / Size
    HWND footer{}; // opaque broker-owned child avoids Explorer-white parent footer
    HWND list{};
    // HWNDs survive a short Explorer Unload only while parked under a
    // broker-owned top-level window. Never reuse across unrelated Explorer roots.
    bool parked{};
    HWND parkedExplorerRoot{};
    ULONGLONG parkedSince{};
    // Preview rows may survive only a short reattach to the exact same,
    // unchanged archive. Never reuse the rows across a path replacement.
    FILETIME parkedSignatureModified{};
    ULONGLONG parkedSignatureBytes{};
    ArchiveFileIdentity parkedSignatureIdentity{};
    bool parkedSignatureValid{};
    bool parkedWasEnumerating{};
    std::wstring archive;
    std::wstring folder;
    std::vector < BrowserEntry > entries;
    std::vector < BrowserColumnKind > visibleColumns;
    BrowserColumnKind sortColumn{BrowserColumnKind::Name};
    bool sortAscending{true};
    int appliedLanguage{-1};
    unsigned displayedMetadataMask{~0u};
    int resizingColumn{-1};
    int resizeStartX{},resizeStartWidth{};
    int pressedHeader{-1};
    HIMAGELIST shellIcons{}; // borrowed system small-icon list, never destroy
    HIMAGELIST rowSpacingIcons{}; // private list used only to set row height
    HFONT rowFont{};
    HFONT headerFont{};
    UINT currentDpi{};
    int iconPixels{16};
    int rowPixels{24};
    int headerPixels{25};
    int hoverIndex{ - 1};
    bool trackingMouse{};
    bool dragging{};
    // A newer ZIP preview took over this Explorer pane. Keep a broker state
    // alive during a nested OLE drag/copy loop, but never paint it again.
    bool retireWhenIdle{};
    GUID pendingExtract{};
    HWND progressCancelButton{};
    bool progressVisible{};
    UINT progressPollMs{900}; // use 250ms only during active operations
    DWORD progressStage{ARCHIVE_PROGRESS_NONE};
    DWORD progressPercent{};
    bool progressKnown{};
    ULONGLONG progressElapsedSeconds{};
    bool progressCancelling{};
    GUID progressRequestId{};
    ULONGLONG progressClickAt{};
    std::wstring extractStatus;
    ULONGLONG extractStatusUntil{};
    int statusStripPixels{}; // Tracked to reflow the ListView only on visibility changes.
    bool dark{};
    bool highContrast{};
    bool updating{};
    bool hasRenderedFolder{};
    bool partialModelPreview{}; // first rows shown before full enumeration
    uint64_t instanceId{};
    uint64_t modelGeneration{};
    std::shared_ptr < std::atomic < bool >> modelCancelled;
    bool modelPending{};
    std::wstring modelPendingFolder;
    bool heavyModelDeferred{}; // timer has not launched COM work yet
    // Set only by the explicit Refresh Preview action. Worker completion must
    // apply a freshly enumerated model even when the folder did not change.
    bool manualRefreshPending{};
    std::wstring manualRefreshFolder;
    // After a successful drop, verify that the archive is still the item
    // selected in its own Explorer window. Never guess if Shell COM fails.
    ULONGLONG selectionGuardDeadline{};
    ULONGLONG selectionGuardNext{};
    unsigned selectionMismatchCount{};
    bool selectionGuardLogged{};
    ULONGLONG lastThemeCheck{};
    std::wstring renderedFolder;
    BrowserPaintCache paint; // original 0.6.7 colors, cached GDI handles
    BrowserSurfaceBuffer headerSurface;
    BrowserSurfaceBuffer footerSurface;
    // paint counters sampled on the broker UI thread, never logged in
    // WM_PAINT. Logging there would itself produce severe paint contention.
    unsigned headerPaints{};
    unsigned footerPaints{};
    unsigned headerErases{};
    unsigned footerErases{};
    unsigned hostPaints{};
    unsigned hostErases{};
    unsigned diagnosticRowPaints{};
    unsigned diagnosticIconDraws{};
    unsigned diagnosticIconMisses{};
    unsigned diagnosticInvalidRows{};
    unsigned repairs{};
    std::vector < int > pressSelectionRows;
    bool pressCustomHit{};
    POINT pressPoint{};
    bool pressSelectionOversized{};
    bool pressWasSelected{};
    ULONGLONG lastSelectionLimitWarning{};
    int pressIndex{ - 1};
    ~BrowserState() {
        if (rowSpacingIcons) ImageList_Destroy(rowSpacingIcons);
        if (rowFont) DeleteObject(rowFont);
        if (headerFont) DeleteObject(headerFont);
    }
};
static COLORREF BrowserHostColor(HWND parent,PCWSTR property){
    const ULONG_PTR value=reinterpret_cast<ULONG_PTR>(parent?GetPropW(parent,property):nullptr);
    return value && value<=0x1000000?static_cast<COLORREF>(value-1):CLR_INVALID;
}
static bool BrowserHostColorsChanged(const BrowserState * s){
    return s->paint.sourceBackground!=BrowserHostColor(s->parent,ArchivePreviewHost::kBackgroundProperty) ||
        s->paint.sourceForeground!=BrowserHostColor(s->parent,ArchivePreviewHost::kForegroundProperty);
}
static void BrowserRefreshPaintColors(BrowserState * s){
    BrowserCachePaintColors(s->paint,s->dark,s->highContrast,
        BrowserHostColor(s->parent,ArchivePreviewHost::kBackgroundProperty),
        BrowserHostColor(s->parent,ArchivePreviewHost::kForegroundProperty));
}
static COLORREF BrowserBackground(const BrowserState * s){
    return s && s->paint.ready?s->paint.backgroundColor:
        (s && s->highContrast?GetSysColor(COLOR_WINDOW):(s && s->dark?RGB(28,28,28):RGB(255,255,255)));
}
static COLORREF BrowserForeground(const BrowserState * s){
    return s && s->paint.ready?s->paint.foregroundColor:
        (s && s->highContrast?GetSysColor(COLOR_WINDOWTEXT):(s && s->dark?RGB(238,238,238):RGB(28,28,28)));
}
static COLORREF BrowserPanel(const BrowserState * s){return s && s->paint.ready?s->paint.panelColor:BrowserBackground(s);}
static COLORREF BrowserMuted(const BrowserState * s){return s && s->paint.ready?s->paint.mutedColor:BrowserForeground(s);}
static COLORREF BrowserBorder(const BrowserState * s){return s && s->paint.ready?s->paint.borderColor:BrowserForeground(s);}
static void BrowserDiagnosticSnapshot(BrowserState * s,PCWSTR reason) {
    if (!s || !ArchivePreviewDiagnostics::Enabled()) return;
    const size_t sampled = std::min<size_t>(64,s->entries.size());
    size_t owned = 0,folders = 0;
    for (size_t i = 0;i < sampled;++i) {
        if (s->entries[i].iconImage && s->entries[i].iconImage->handle) ++owned;
        if (s->entries[i].folder) ++folders;
    }
    RECT inputRect{};
    if (s->list) GetWindowRect(s->list,&inputRect);
    POINT inputPoint{inputRect.left + (inputRect.right-inputRect.left)/2,
        inputRect.top + std::min<LONG>(12,std::max<LONG>(0,(inputRect.bottom-inputRect.top)/2))};
    HWND inputHit = WindowFromPoint(inputPoint);
    wchar_t inputClass[80]{};
    if (inputHit) GetClassNameW(inputHit,inputClass,_countof(inputClass));
    Wh_Log(L"[AP input] %s list=%p enabled=%d hostEnabled=%d parentEnabled=%d "
           L"width=%ld height=%ld hit=%p hitClass=%s above=%p",
        reason,s->list,s->list?IsWindowEnabled(s->list):0,
        s->host?IsWindowEnabled(s->host):0,s->parent?IsWindowEnabled(s->parent):0,
        inputRect.right-inputRect.left,inputRect.bottom-inputRect.top,inputHit,inputClass,
        s->host?GetWindow(s->host,GW_HWNDPREV):nullptr);
    wchar_t klass[80]{};
    if (s->list) GetClassNameW(s->list,klass,_countof(klass));
    Wh_Log(L"[AP diag/broker] %s pid=%lu instance=%llu anchor=%p host=%p list=%p "
           L"parent=%p listParent=%p class=%s style=%llX parked=%d visible=%d "
           L"rows=%zu controlRows=%d sample=%zu owned=%zu folders=%zu pending=%d partial=%d "
           L"paints=%u icons=%u misses=%u invalidRows=%u dpi=%u root_folder=%d",
        reason,GetCurrentProcessId(),static_cast<unsigned long long>(s->instanceId),
        s->anchor,s->host,s->list,s->parent,s->list?GetParent(s->list):nullptr,klass,
        static_cast<unsigned long long>(s->list?GetWindowLongPtrW(s->list,GWL_STYLE):0),
        s->parked?1:0,s->list && IsWindowVisible(s->list)?1:0,s->entries.size(),
        s->list && IsWindow(s->list)?ListView_GetItemCount(s->list):0,sampled,owned,folders,
        s->modelPending?1:0,s->partialModelPreview?1:0,s->diagnosticRowPaints,
        s->diagnosticIconDraws,s->diagnosticIconMisses,s->diagnosticInvalidRows,
        s->currentDpi,s->folder.empty()?1:0);
}
[[clang::no_destroy]] static std::unordered_map < HWND,std::shared_ptr < BrowserState >> g_browsers;
static std::atomic < uint64_t > g_nextBrowserId{1};
static std::atomic < unsigned > g_activeQueueOperations{0};
static void BrowserInvalidateFooter(BrowserState * s);

static void BrowserScheduleListSettle(BrowserState * s) {
    if (s && !s->parked && s->host && IsWindow(s->host))
        SetTimer(s->host,kBrowserListSettleTimer,135,nullptr);
}
static HWND g_browserParkHost = nullptr; // UI-thread-owned, hidden top-level parking parent
static std::atomic < unsigned > g_activeModelWorkers{0};
// Heavy Shell COM enumerations cannot always be cancelled while a provider is
// inside a synchronous call. Never let four hung large archives consume all
// model slots, otherwise even a tiny ZIP stops previewing.
// Two of the four slots remain available for normal (small) archives.
static std::atomic < unsigned > g_activeHeavyModelWorkers{0};
static constexpr ULONGLONG kBrowserHeavyArchiveBytes = ArchivePreviewOptions::kLargeMetadataThreshold;
static constexpr unsigned kBrowserMaxModelWorkers = 4;
static constexpr unsigned kBrowserMaxHeavyModelWorkers = 2;
// A tiny .tgz can contain several GiB of decompressed TAR data. Physical
// archive size alone does not predict expensive sequential enumeration.
static bool BrowserHeavyModelRequest(const std::wstring & path,ULONGLONG bytes) {
    if (bytes >= kBrowserHeavyArchiveBytes) return true;
    static constexpr const wchar_t * sequential[] = {
        L".tgz",L".tbz",L".tbz2",L".txz",L".tzst",
        L".tar.gz",L".tar.bz2",L".tar.xz",L".tar.zst",
        L".tar.zstd",L".tar.lz4"};
    for (const wchar_t * extension:sequential) {
        const size_t length = wcslen(extension);
        if (path.size() > length &&
            _wcsicmp(path.c_str() + path.size() - length,extension) == 0)
            return true;
    }
    return false;
}
static bool BrowserReserveModelWorker(bool heavy) {
    if (heavy && g_activeHeavyModelWorkers.fetch_add(1,
            std::memory_order_acq_rel) >= kBrowserMaxHeavyModelWorkers) {
        g_activeHeavyModelWorkers.fetch_sub(1,std::memory_order_acq_rel);
        return false;
    }
    if (g_activeModelWorkers.fetch_add(1,
            std::memory_order_acq_rel) >= kBrowserMaxModelWorkers) {
        g_activeModelWorkers.fetch_sub(1,std::memory_order_acq_rel);
        if (heavy) g_activeHeavyModelWorkers.fetch_sub(1,
            std::memory_order_acq_rel);
        return false;
    }
    return true;
}
static void BrowserReleaseModelWorker(bool heavy) {
    if (heavy) g_activeHeavyModelWorkers.fetch_sub(1,
        std::memory_order_acq_rel);
    g_activeModelWorkers.fetch_sub(1,std::memory_order_acq_rel);
}

// Cache metadata and the last selection per archive, bounded to six archives
// and 32 folders each. The cache is process-local and keyed by the archive's
// last-write timestamp/size, so reopening the same ZIP is fast but edits are
// not served stale metadata. It never stores extracted file contents.
struct BrowserSavedView {
    std::vector < std::wstring > selectedPaths;
    int firstVisible{};
    int focused{ - 1};
    std::wstring focusedPath;
    std::wstring topPath;
};
struct BrowserArchiveCache {
    FILETIME modified{};
    ULONGLONG bytes{};
    ArchiveFileIdentity identity{};
    bool signatureValid{};
    unsigned metadataMask{~0u};
    ULONGLONG touched{};
    std::unordered_map < std::wstring,std::vector < BrowserEntry >> folders;
    std::unordered_map < std::wstring,BrowserSavedView > views;
    std::unordered_map < std::wstring,ULONGLONG > folderTouched;
};
static std::unordered_map < std::wstring,BrowserArchiveCache > g_browserCache;
static BrowserArchiveCache & BrowserCache(const std::wstring & archive) {
    auto & cache = g_browserCache[archive];
    cache.touched = GetTickCount64();
    ArchiveFileIdentity identity{};
    FILETIME stamp{};ULONGLONG bytes{};
    if(GetArchiveSignature(archive,stamp,bytes, & identity)) {
        if(!cache.signatureValid || cache.identity != identity ||
           CompareFileTime( & cache.modified, & stamp) != 0 || cache.bytes != bytes) {
            cache.folders.clear();cache.views.clear();cache.folderTouched.clear();
            cache.modified = stamp;cache.bytes = bytes;cache.identity = identity;
            cache.signatureValid = true;
        }
    } else {
        cache.folders.clear();cache.views.clear();cache.folderTouched.clear();cache.signatureValid = false;
    }
    const unsigned mask=ArchivePreviewOptions::MetadataMask(cache.bytes);
    if(cache.metadataMask!=mask){cache.folders.clear();cache.folderTouched.clear();cache.metadataMask=mask;}
    while (g_browserCache.size() > 6) {
        auto oldest = g_browserCache.end();
        for (auto it = g_browserCache.begin();it != g_browserCache.end();++it) {
            if (it->first == archive) continue;
            if (oldest == g_browserCache.end() || it->second.touched < oldest->second.touched)
                oldest = it;
        }
        if (oldest == g_browserCache.end()) break;
        g_browserCache.erase(oldest);
    }
    return cache;
}

// Bound total cached rows across all archives, not just the number of folders.
// A normal 2,000-entry folder stays cached, while old folders are evicted first.
static void BrowserTrimRowCache(const std::wstring & activeArchive,const std::wstring & activeFolder) {
    constexpr size_t maxCachedRows = 12000;
    constexpr size_t maxCachedBytes = 24ULL * 1024 * 1024;
    const auto entryBytes = [](const std::vector < BrowserEntry >  & entries) {
        size_t sum = entries.size() * sizeof(BrowserEntry);
        for (const auto & entry:entries)
            sum += (entry.name.size() + entry.relative.size() + entry.sizeLabel.size()) * sizeof(wchar_t);
        return sum;
    };
    size_t rows = 0, bytes = 0;
    for (const auto & [archive,cache]:g_browserCache)
        for (const auto & [folder,entries]:cache.folders) {
            rows += entries.size();bytes += entryBytes(entries);
        }
    while (rows > maxCachedRows || bytes > maxCachedBytes) {
        auto oldestArchive = g_browserCache.end();
        std::wstring oldestFolder;
        ULONGLONG oldestTick = std::numeric_limits < ULONGLONG > ::max();
        for (auto it = g_browserCache.begin();it != g_browserCache.end();++it) {
            for (const auto & pair:it->second.folders) {
                if (it->first == activeArchive && pair.first == activeFolder) continue;
                const auto touch = it->second.folderTouched.find(pair.first);
                const ULONGLONG stamp = touch == it->second.folderTouched.end()?0:touch->second;
                if (oldestArchive == g_browserCache.end() || stamp < oldestTick) {
                    oldestArchive = it;oldestFolder = pair.first;oldestTick = stamp;
                }
            }
        }
        if (oldestArchive == g_browserCache.end()) break;
        auto & c = oldestArchive->second;
        const auto hit = c.folders.find(oldestFolder);
        if (hit == c.folders.end()) break;
        rows -= hit->second.size();
        bytes -= std::min(bytes,entryBytes(hit->second));
        c.folders.erase(hit);c.views.erase(oldestFolder);c.folderTouched.erase(oldestFolder);
            }
}

static bool ValidBrowserAnchor(HWND anchor) {
    if (!anchor || !IsWindow(anchor)) return false;
    wchar_t klass[128]{};
    GetClassNameW(anchor,klass,_countof(klass));
    if (wcscmp(klass,WC_LISTVIEWW)) return false;
    HWND parent = GetParent(anchor);
    if (!parent || !IsWindow(parent)) return false;
    GetClassNameW(parent,klass,_countof(klass));
    return wcscmp(klass,kPreviewHostClass) == 0;
}
// The Explorer pane can retain an old preview window for a short time after
// a drag-out selects the extracted folder. The broker is medium integrity,
// so it can query the corresponding Explorer folder view via Shell automation.
// Only an unambiguous mismatch is actionable. Unknown = never hide.
enum class ExplorerSelection { Unknown, ArchiveSelected, OtherSelected };
static ExplorerSelection SelectedItemInOwningExplorer(const BrowserState * s) {
    if (!s || !s->parent || !IsWindow(s->parent)) return ExplorerSelection::Unknown;
    const HWND owner = GetAncestor(s->parent, GA_ROOT);
    if (!owner || !IsWindow(owner)) return ExplorerSelection::Unknown;

    IShellWindows * windows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_LOCAL_SERVER,
        IID_PPV_ARGS( & windows))) || !windows) return ExplorerSelection::Unknown;
    long count = 0;
    if (FAILED(windows->get_Count( & count))) { windows->Release(); return ExplorerSelection::Unknown; }
    bool sawDifferent = false;
    bool sawArchive = false;
    // Bound enumeration to avoid a broken shell extension monopolizing the UI.
    for (long i = 0;i < count && i < 64;i++) {
        VARIANT index{}; V_VT( & index) = VT_I4; V_I4( & index) = i;
        IDispatch * dispatch = nullptr;
        if (FAILED(windows->Item(index, & dispatch)) || !dispatch) continue;
        IWebBrowser2 * browser = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS( & browser))) && browser) {
            SHANDLE_PTR shellHandle = 0;
            if (SUCCEEDED(browser->get_HWND( & shellHandle))) {
                const HWND window = reinterpret_cast < HWND > (static_cast < INT_PTR > (shellHandle));
                if (window && IsWindow(window) && GetAncestor(window,GA_ROOT) == owner) {
                    IDispatch * document = nullptr;
                    if (SUCCEEDED(browser->get_Document( & document)) && document) {
                        IShellFolderViewDual * view = nullptr;
                        if (SUCCEEDED(document->QueryInterface(IID_PPV_ARGS( & view))) && view) {
                            FolderItems * selected = nullptr;
                            if (SUCCEEDED(view->SelectedItems( & selected)) && selected) {
                                long selectedCount = 0;
                                if (SUCCEEDED(selected->get_Count( & selectedCount)) && selectedCount > 0) {
                                    if (selectedCount == 1) {
                                        VARIANT first{}; V_VT( & first) = VT_I4; V_I4( & first) = 0;
                                        FolderItem * item = nullptr;
                                        if (SUCCEEDED(selected->Item(first, & item)) && item) {
                                            BSTR path = nullptr;
                                            if (SUCCEEDED(item->get_Path( & path)) && path) {
                                                if (_wcsicmp(path,s->archive.c_str()) == 0) sawArchive = true;
                                                else sawDifferent = true;
                                            }
                                            SysFreeString(path);
                                            item->Release();
                                        }
                                    } else sawDifferent = true;
                                }
                                selected->Release();
                            }
                            view->Release();
                        }
                        document->Release();
                    }
                }
            }
            browser->Release();
        }
        dispatch->Release();
    }
    windows->Release();
    // Multiple tabs/windows may report the same top-level HWND. Do not
    // discard a preview if any matching view still selects its archive.
    if (sawArchive) return ExplorerSelection::ArchiveSelected;
    if (sawDifferent) return ExplorerSelection::OtherSelected;
    return ExplorerSelection::Unknown;
}
static bool BrowserHighContrast() {
    HIGHCONTRASTW hc{};
    hc.cbSize = sizeof(hc);
    return SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(hc), & hc,0) &&
           (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}
static bool BrowserDarkTheme() {
    DWORD light = 1,cb = sizeof(light);
    if (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",RRF_RT_REG_DWORD,nullptr, & light, & cb) != ERROR_SUCCESS)
        return ArchivePreviewOptions::Dark(false);
    return ArchivePreviewOptions::Dark(light == 0);
}
// Keep type arithmetic in int: Win32 RECT dimensions are LONG on x64.
static int BrowserPx(const BrowserState * s, int logical) {
    // DPI is already sampled and updated on WM_DPICHANGED; avoid a Win32
    // query for every spacing calculation on every painted row.
    UINT dpi = s ? s->currentDpi : 0;
    if (!dpi) {
        const HWND hwnd = s ? (s->host ? s->host : s->list) : nullptr;
        dpi = hwnd ? GetDpiForWindow(hwnd) : 96;
    }
    return MulDiv(logical, static_cast < int > (dpi ? dpi : 96), 96);
}
static int BrowserHeaderHeight(const BrowserState * s) {
    return s && s->headerPixels > 0 ? s->headerPixels : BrowserPx(s, 25);
}
static int BrowserFooterHeight(const BrowserState * s) {
    return BrowserPx(s, 28); // three extra DIP for footer breathing room.
}
static bool BrowserStatusActive(const BrowserState * s) {
    return s && !s->extractStatus.empty() &&
        (!s->extractStatusUntil || GetTickCount64() < s->extractStatusUntil);
}
static int BrowserProgressHeight(const BrowserState * s) {
    if (!s) return 0;
    // completion messages live in the existing footer. A short ZIP
    // extract must not insert/remove a 32px strip and resize the ListView.
    return s->progressVisible ? BrowserPx(s,40) : 0;
}
static RECT BrowserProgressRect(BrowserState * s,const RECT & area) {
    RECT result = area;
    result.bottom = std::max(result.top,area.bottom - BrowserFooterHeight(s));
    result.top = std::max(result.top,result.bottom - BrowserProgressHeight(s));
    return result;
}
static void BrowserLayoutProgress(BrowserState * s) {
    if (!s || !s->host || !s->list) return;
    RECT area{};GetClientRect(s->host, & area);
    const int width = std::max(1,static_cast < int > (area.right - area.left));
    const int height = std::max(1,static_cast < int > (area.bottom - area.top));
    const int top = BrowserHeaderHeight(s);
    const int notice = BrowserProgressHeight(s);
    s->statusStripPixels = notice;
    const int used = BrowserFooterHeight(s) + notice;
    // Only move children when their rectangles changed. Repeated progress
    // polls must not invalidate the entire dark ListView or footer.
    auto positionChild = [ & ](HWND child,int x,int y,int w,int h) {
        if (!child || !IsWindow(child)) return;
        RECT previous{};
        if (GetWindowRect(child, & previous)) {
            MapWindowPoints(nullptr,s->host,reinterpret_cast < POINT *  > ( & previous),2);
            if (previous.left == x && previous.top == y &&
                previous.right - previous.left == w &&
                previous.bottom - previous.top == h) return;
        }
        SetWindowPos(child,nullptr,x,y,w,h,SWP_NOACTIVATE | SWP_NOZORDER);
    };
    positionChild(s->list,0,top,width,std::max(1,height - top - used));
    if (s->footer && IsWindow(s->footer)) {
        const int footerHeight = BrowserFooterHeight(s);
        positionChild(s->footer,0,std::max(0,height - footerHeight),
            width,footerHeight);
    }
    if (s->progressCancelButton) {
        const int buttonWidth = BrowserPx(s,66);
        const int buttonHeight = BrowserPx(s,24);
        const RECT progress = BrowserProgressRect(s,area);
        positionChild(s->progressCancelButton,
            std::max(0,width - BrowserPx(s,8) - buttonWidth),
            static_cast < int > (progress.top) + BrowserPx(s,5),
            buttonWidth,buttonHeight);
        if (!!(GetWindowLongPtrW(s->progressCancelButton,GWL_STYLE) & WS_VISIBLE) != s->progressVisible)
            ShowWindow(s->progressCancelButton,
                s->progressVisible?SW_SHOWNA:SW_HIDE);
        EnableWindow(s->progressCancelButton,
            s->progressVisible && !s->progressCancelling &&
                s->progressStage != ARCHIVE_PROGRESS_EXTERNAL);
    }
}
static void BrowserPaintProgress(BrowserState * s,HDC dc,const RECT & bounds) {
    if (!s || !dc || !s->progressVisible) return;
    const RECT panel = BrowserProgressRect(s,bounds);
    const COLORREF background = BrowserPanel(s);
    const COLORREF foreground = BrowserForeground(s);
    const COLORREF border = BrowserBorder(s);
    const COLORREF accent = s->paint.accent;
    HBRUSH bg = CreateSolidBrush(background);
    if (bg) {FillRect(dc, & panel,bg);DeleteObject(bg);}
    HPEN pen = CreatePen(PS_SOLID,1,border);
    if (pen) {
        HGDIOBJ old = SelectObject(dc,pen);
        MoveToEx(dc,panel.left,panel.top,nullptr);
        LineTo(dc,panel.right,panel.top);
        SelectObject(dc,old);DeleteObject(pen);
    }
    using Text = ArchivePreviewText::Id;
    Text message = Text::Extracting;
    if (s->progressStage == ARCHIVE_PROGRESS_EXTERNAL) message = Text::ExternalProgress;
    else if (s->progressCancelling) message = Text::Cancelling;
    else if (s->progressStage == ARCHIVE_PROGRESS_SCANNING) message = Text::PreparingFiles;
    else if (s->progressStage == ARCHIVE_PROGRESS_PICKING) message = Text::ExtractTo;
    else if (s->progressStage == ARCHIVE_PROGRESS_STARTING) message = Text::StartingExtraction;
    std::wstring title = ArchivePreviewText::Get(message);
    if (!s->progressCancelling && s->progressStage == ARCHIVE_PROGRESS_EXTRACTING &&
        s->progressKnown && s->progressPercent > 0) {
        title += L" " + std::to_wstring(s->progressPercent) + L"%";
        if (s->progressPercent >= 100)
            title += L" (" + std::wstring(ArchivePreviewText::Get(Text::Estimate)) + L")";
    }
    title += L" · " + std::to_wstring(s->progressElapsedSeconds) + L" s";
    RECT label = panel;
    label.left += BrowserPx(s,10);
    label.right = std::max(label.left + 1,label.right - BrowserPx(s,80));
    label.bottom = panel.top + BrowserPx(s,29);
    HFONT oldFont = static_cast < HFONT > (SelectObject(dc,
        s->rowFont?s->rowFont:GetStockObject(DEFAULT_GUI_FONT)));
    const int oldMode = SetBkMode(dc,TRANSPARENT);
    const COLORREF oldColor = SetTextColor(dc,foreground);
    DrawTextW(dc,title.c_str(), - 1, & label,
        DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | ArchivePreviewText::TextDirection());
    SetTextColor(dc,oldColor);SetBkMode(dc,oldMode);SelectObject(dc,oldFont);
    RECT rail{panel.left + BrowserPx(s,9),panel.bottom - BrowserPx(s,8),
        panel.right - BrowserPx(s,9),panel.bottom - BrowserPx(s,4)};
    if (rail.right <= rail.left || rail.bottom <= rail.top) return;
    HBRUSH track = CreateSolidBrush(border);
    if (track) {FillRect(dc, & rail,track);DeleteObject(track);}
    RECT fill = rail;
    if (s->progressStage == ARCHIVE_PROGRESS_EXTRACTING && s->progressKnown &&
        s->progressPercent > 0 && s->progressPercent < 100) {
        fill.right = fill.left + MulDiv(static_cast < int > (rail.right - rail.left),
            static_cast < int > (std::min < DWORD > (s->progressPercent,100)),100);
    } else {
        const int span = std::max(1,static_cast < int > (rail.right - rail.left) / 5);
        const int travel = static_cast < int > (rail.right - rail.left) + span;
        fill.left = rail.left + static_cast < int > (GetTickCount64() % 1800 * travel / 1800) - span;
        fill.right = fill.left + span;
        fill.left = std::max(fill.left,rail.left);
        fill.right = std::min(fill.right,rail.right);
    }
    if (fill.right > fill.left) {
        HBRUSH marker = CreateSolidBrush(accent);
        if (marker) {FillRect(dc, & fill,marker);DeleteObject(marker);}
    }
}
static void BrowserInvalidateFooter(BrowserState * s) {
    if (!s) return;
    if (s->footer && IsWindow(s->footer)) {
        InvalidateRect(s->footer,nullptr,FALSE);
        return;
    }
    if (!s->host || !IsWindow(s->host)) return;
    RECT r{}; GetClientRect(s->host, & r);
    r.top = std::max(r.top,r.bottom - BrowserFooterHeight(s));
    InvalidateRect(s->host, & r,FALSE);
}
static void BrowserPaintFooter(BrowserState * s,HDC dc,const RECT & bounds) {
    if (!s || !dc) return;
    RECT foot = bounds;
    foot.top = std::max(foot.top,foot.bottom - BrowserFooterHeight(s));
    const COLORREF background = BrowserPanel(s);
    const COLORREF foreground = BrowserMuted(s);
    HBRUSH brush = s->paint.footer?s->paint.footer:CreateSolidBrush(background);
    if (brush) {
        FillRect(dc, & foot,brush);
        if (brush != s->paint.footer) DeleteObject(brush);
    }
    HPEN line = s->paint.footerDivider?s->paint.footerDivider:
        CreatePen(PS_SOLID,1,BrowserBorder(s));
    if (line) {
        HGDIOBJ previous = SelectObject(dc,line);
        MoveToEx(dc,foot.left,foot.top,nullptr);LineTo(dc,foot.right,foot.top);
        SelectObject(dc,previous);
        if (line != s->paint.footerDivider) DeleteObject(line);
    }
    const int selected = s->list && IsWindow(s->list)?ListView_GetSelectedCount(s->list):0;
    const size_t total = s->entries.size();
    std::wstring summary = ArchivePreviewText::Get(ArchivePreviewText::Id::Selected);
    summary += L" " + std::to_wstring(selected) + L" / " + std::to_wstring(total);
    if (selected == 1 && !BrowserStatusActive(s)) {
        const int index = ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);
        if (index >= 0 && static_cast < size_t > (index) < s->entries.size()) {
            summary += L"    |    ";
            summary += s->entries[static_cast < size_t > (index)].name;
        }
    }
    // Reuse the footer itself for extraction/open status: no extra child,
    // no vertical relayout, and no Explorer-white surface on fast completion.
    if (BrowserStatusActive(s)) summary = s->extractStatus;
    RECT text = foot;
    text.left += BrowserPx(s,10);
    text.right -= BrowserPx(s,10); // Full-width footer summary without cache button.
    HFONT previous = static_cast < HFONT > (SelectObject(dc,s->rowFont?s->rowFont:GetStockObject(DEFAULT_GUI_FONT)));
    const int oldMode = SetBkMode(dc,TRANSPARENT);
    const COLORREF oldText = SetTextColor(dc,foreground);
    DrawTextW(dc,summary.c_str(), - 1, & text,DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS |
        DT_NOPREFIX | ArchivePreviewText::TextDirection());
    SetTextColor(dc,oldText);SetBkMode(dc,oldMode);SelectObject(dc,previous);
}
// Keep the entire footer in an opaque child window,
// just as the column header has its own HWND.
// Explorer can invalidate the cross-process parent while a fast ZIP extract
// finishes; it cannot erase an independently painted child surface.
static LRESULT CALLBACK BrowserFooterProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto * create = reinterpret_cast < CREATESTRUCTW *  > (lp);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,
            reinterpret_cast < LONG_PTR > (create->lpCreateParams));
    }
    auto * s = reinterpret_cast < BrowserState *  > (GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    switch(msg) {
      case WM_ERASEBKGND: {
          if (s && ArchivePreviewDiagnostics::Enabled()) ++s->footerErases;
          if (s && wp) {
              RECT area{};GetClientRect(hwnd, & area);
              BrowserPaintFooter(s,reinterpret_cast < HDC > (wp),area);
          }
          return 1;
      }
      case WM_PAINT: {
          if (s && ArchivePreviewDiagnostics::Enabled()) ++s->footerPaints;
          PAINTSTRUCT paint{};
          HDC target = BeginPaint(hwnd, & paint);
          if (target && s) {
              RECT area{};GetClientRect(hwnd, & area);
              const int width = std::max(0,static_cast < int > (area.right - area.left));
              const int height = std::max(0,static_cast < int > (area.bottom - area.top));
              HDC memory = s->footerSurface.Ensure(target,width,height);
              BrowserPaintFooter(s,memory?memory:target,area);
              if (memory) BitBlt(target,0,0,width,height,memory,0,0,SRCCOPY);
          }
          EndPaint(hwnd, & paint);
          return 0;
      }
      case WM_PRINT:
      case WM_PRINTCLIENT:
          if (wp && s && (msg == WM_PRINTCLIENT || (lp & PRF_CLIENT))) {
              RECT area{};GetClientRect(hwnd, & area);
              BrowserPaintFooter(s,reinterpret_cast < HDC > (wp),area);
          }
          return 0;
      case WM_THEMECHANGED:
      case WM_SYSCOLORCHANGE:
      case WM_SETTINGCHANGE:
      case WM_DPICHANGED_AFTERPARENT:
          InvalidateRect(hwnd,nullptr,FALSE);
          return 0;
      case WM_NCDESTROY:
          if (s) s->footerSurface.Reset();
          if (s && s->footer == hwnd) s->footer = nullptr;
          SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
          return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
static void BrowserRedrawHeader(BrowserState * s, bool immediate = false);
static const wchar_t * BrowserColumnTitle(BrowserColumnKind column) {
    using Id = ArchivePreviewText::Id;
    switch(column) {
      case BrowserColumnKind::Name: return ArchivePreviewText::Get(Id::ColumnName);
      case BrowserColumnKind::DateModified: return ArchivePreviewText::Get(Id::ColumnDate);
      case BrowserColumnKind::Type: return ArchivePreviewText::Get(Id::ColumnType);
      case BrowserColumnKind::Size: return ArchivePreviewText::Get(Id::ColumnSize);
    }
    return L"";
}
static void BrowserOrderRows(std::vector<BrowserEntry> & rows,BrowserColumnKind column,bool ascending) {
    ArchivePreviewSort::Rows(rows,static_cast<int>(column),ascending,
        [](const BrowserEntry & e)->ArchivePreviewSort::Values {
            return {e.folder,e.name,e.typeLabel,e.sizeBytes,e.hasSize,e.modifiedValue};
        });
}
static void BrowserSortByColumn(BrowserState * s,BrowserColumnKind kind);
static std::wstring BrowserHeaderCaption(const BrowserState * s,BrowserColumnKind kind) {
    std::wstring text = BrowserColumnTitle(kind);
    if (s && s->sortColumn == kind) text += s->sortAscending?L" ▲":L" ▼";
    return text;
}
static const std::wstring & BrowserEntryColumn(const BrowserEntry & entry, BrowserColumnKind column) {
    switch(column) {
      case BrowserColumnKind::DateModified: return entry.modifiedLabel;
      case BrowserColumnKind::Type: return entry.typeLabel;
      case BrowserColumnKind::Size: return entry.sizeLabel;
      default: return entry.name;
    }
}
static void BrowserLayoutColumns(BrowserState * s) {
    if (!s || !s->list || s->resizingColumn >= 0) return;
    ArchivePreviewColumns::Layout(s->list,s->currentDpi,s->visibleColumns);
    BrowserRedrawHeader(s);
}
static bool BrowserRebuildColumns(BrowserState * s) {
    if(!s || !s->list || !IsWindow(s->list)) return false;
    s->resizingColumn=-1;s->pressedHeader=-1;
    if(GetCapture()==s->header)ReleaseCapture();
    s->appliedLanguage = ArchivePreviewText::Language();
    s->visibleColumns.clear();
    s->visibleColumns.push_back(BrowserColumnKind::Name);
    if(ArchivePreviewOptions::ShowColumn(1)) s->visibleColumns.push_back(BrowserColumnKind::DateModified);
    if(ArchivePreviewOptions::ShowColumn(2)) s->visibleColumns.push_back(BrowserColumnKind::Type);
    if(ArchivePreviewOptions::ShowColumn(3)) s->visibleColumns.push_back(BrowserColumnKind::Size);
    for(int i = 0;i < 4;++i) if(!ListView_DeleteColumn(s->list,0)) break;
    LVCOLUMNW column{};column.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT;
    for(size_t i = 0;i < s->visibleColumns.size();++i) {
        const BrowserColumnKind kind = s->visibleColumns[i];
        column.pszText = const_cast < LPWSTR > (BrowserColumnTitle(kind));
        column.cx = 0;
        column.fmt = kind == BrowserColumnKind::Size ? LVCFMT_RIGHT : LVCFMT_LEFT;
        if (ListView_InsertColumn(s->list,static_cast<int>(i), & column) < 0) return false;
    }
    BrowserLayoutColumns(s);
    InvalidateRect(s->list,nullptr,FALSE);
    return true;
}

// Use the Windows message font and small-icon metrics, not fixed 11 pt / 22 px.
// SystemParametersInfoForDpi already returns a LOGFONT scaled for this DPI.
// Do not scale its lfHeight a second time.
static void BrowserApplyMetrics(BrowserState * s, bool force = false) {
    if (!s || !s->host || !s->list) return;
    UINT dpi = GetDpiForWindow(s->host);
    if (!dpi) dpi = GetDpiForWindow(s->list);
    if (!dpi) dpi = 96;
    if (!force && s->currentDpi == dpi && s->rowFont && s->rowSpacingIcons) return;

    NONCLIENTMETRICSW nc{};
    nc.cbSize = sizeof(nc);
    LOGFONTW lf{};
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(nc), & nc, 0, dpi)) {
        lf = nc.lfMessageFont;
    } else {
        lf.lfHeight = - MulDiv(9, static_cast < int > (dpi), 72);
        lf.lfWeight = FW_NORMAL;
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
    }
    HFONT row = CreateFontIndirectW( & lf);
    if (!row) return;
    LOGFONTW heading = lf;
    heading.lfWeight = FW_SEMIBOLD;
    HFONT header = CreateFontIndirectW( & heading);
    if (!header) { DeleteObject(row); return; }

    const int iconSize = std::max(12, GetSystemMetricsForDpi(SM_CXSMICON, dpi));
    int textHeight = std::max(1, MulDiv(12, static_cast < int > (dpi), 96));
    HDC dc = GetDC(s->list);
    if (dc) {
        HFONT old = static_cast < HFONT > (SelectObject(dc, row));
        TEXTMETRICW tm{};
        if (GetTextMetricsW(dc, & tm)) textHeight = tm.tmHeight;
        SelectObject(dc, old);
        ReleaseDC(s->list, dc);
    }
    const int pad = MulDiv(7, static_cast < int > (dpi), 96);
    const int rowHeight = std::max(textHeight, iconSize) + std::max(5, pad);
    const int headerHeight = std::max(textHeight + MulDiv(10, static_cast < int > (dpi), 96),
                                      MulDiv(24, static_cast < int > (dpi), 96));
    HIMAGELIST spacing = ImageList_Create(1, rowHeight, ILC_COLOR32 | ILC_MASK, 1, 1);
    if (!spacing) { DeleteObject(row); DeleteObject(header); return; }

    // Assign new GDI resources before releasing the previous ones.
    HFONT oldRow = s->rowFont, oldHeader = s->headerFont;
    HIMAGELIST oldSpacing = s->rowSpacingIcons;
    s->rowFont = row;
    s->headerFont = header;
    s->rowSpacingIcons = spacing;
    s->currentDpi = dpi;
    s->iconPixels = iconSize;
    s->rowPixels = rowHeight;
    s->headerPixels = headerHeight;
    SendMessageW(s->list, WM_SETFONT, reinterpret_cast < WPARAM > (row), TRUE);
    ListView_SetImageList(s->list, spacing, LVSIL_SMALL);
    if (oldSpacing) ImageList_Destroy(oldSpacing);
    if (oldRow) DeleteObject(oldRow);
    if (oldHeader) DeleteObject(oldHeader);
    RECT bounds{};
    GetClientRect(s->host, & bounds);
    const int width = std::max(1, static_cast < int > (bounds.right));
    if (s->header && IsWindow(s->header))
        MoveWindow(s->header,0,0,width,headerHeight,TRUE);
    BrowserLayoutProgress(s);
    BrowserLayoutColumns(s);
    InvalidateRect(s->list, nullptr, FALSE);
    BrowserRedrawHeader(s);
    InvalidateRect(s->host, nullptr, FALSE);
}

static std::wstring BrowserFormatSize(ULONGLONG size) {
    if (size < 1024) return std::to_wstring(size) + L" B";
    constexpr const wchar_t * units[] = {L"KB", L"MB", L"GB", L"TB"};
    double n = static_cast < double > (size) / 1024.0;
    unsigned index = 0;
    while (n >= 1024.0 && index < 3) { n /= 1024.0; ++index; }
    wchar_t output[48]{};
    if (n < 10.0) swprintf_s(output,L"%.1f %ls",n,units[index]);
    else swprintf_s(output,L"%.0f %ls",n,units[index]);
    return output;
}
// Keep metadata collection on the model's COM STA worker. A single lookup
// per extension supplies Windows' localized type name for many archive entries.
static std::wstring BrowserTypeLabel(const std::wstring & name, bool folder,
                                     std::unordered_map < std::wstring,std::wstring >  & cache) {
    if (folder) return L"File folder";
    const size_t dot = name.find_last_of(L'.');
    std::wstring key = dot != std::wstring::npos && dot > 0 ? name.substr(dot) : L"";
    std::transform(key.begin(),key.end(),key.begin(),[](wchar_t c) {
        return static_cast < wchar_t > (std::towlower(c));
    });
    const auto known = cache.find(key);
    if (known != cache.end()) return known->second;
    SHFILEINFOW info{};
    const DWORD_PTR result = SHGetFileInfoW(name.c_str(),FILE_ATTRIBUTE_NORMAL,
        & info,sizeof(info),SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
    const std::wstring type = result && info.szTypeName[0] ? info.szTypeName : L"File";
    if (cache.size() < 1024) cache.emplace(key,type);
    return type;
}
static std::wstring BrowserModifiedLabel(IShellItem * item,ULONGLONG & value) {
    value = 0;
    if (!item) return {};
    IShellItem2 * item2 = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS( & item2)))) return {};
    FILETIME utc{},local{};
    const HRESULT hr = item2->GetFileTime(PKEY_DateModified, & utc);
    item2->Release();
    if (FAILED(hr) || (!utc.dwHighDateTime && !utc.dwLowDateTime) ||
        !FileTimeToLocalFileTime( & utc, & local)) return {};
    value = ArchivePreviewSort::FileTimeValue(utc);
    SYSTEMTIME st{};
    if (!FileTimeToSystemTime( & local, & st)) return {};
    wchar_t date[80]{},time[80]{};
    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT,DATE_SHORTDATE, & st,nullptr,
                         date,static_cast < int > (_countof(date)),nullptr) ||
        !GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,TIME_NOSECONDS, & st,nullptr,
                         time,static_cast < int > (_countof(time)))) return {};
    return std::wstring(date) + L" " + time;
}

// ZIP files can omit directory entries entirely. Some Shell ZIP providers also
// return no PKEY_DateModified for explicit folders. Use the metadata in the ZIP
// central directory as a *read-only* fallback for smaller archives. An implicit
// directory gets the newest descendant timestamp, which is an approximation,
// not a stored directory mtime. No decompression or disk staging occurs.
struct BrowserZipFolderDate {
    SYSTEMTIME local{};
    bool explicitFolder{};
};
using BrowserZipFolderDates = std::unordered_map < std::wstring,BrowserZipFolderDate > ;
static WORD BrowserZipU16(const BYTE * p) {
    return static_cast < WORD > (p[0] | (static_cast < WORD > (p[1]) << 8));
}
static DWORD BrowserZipU32(const BYTE * p) {
    return static_cast < DWORD > (p[0]) | (static_cast < DWORD > (p[1]) << 8) |
        (static_cast < DWORD > (p[2]) << 16) | (static_cast < DWORD > (p[3]) << 24);
}
static bool BrowserZipDosTime(WORD date, WORD time, SYSTEMTIME & output) {
    SYSTEMTIME st{};
    st.wYear = static_cast < WORD > (1980 + (date >> 9));
    st.wMonth = static_cast < WORD > ((date >> 5) & 15);
    st.wDay = static_cast < WORD > (date & 31);
    st.wHour = static_cast < WORD > (time >> 11);
    st.wMinute = static_cast < WORD > ((time >> 5) & 63);
    st.wSecond = static_cast < WORD > ((time & 31) * 2);
    if (st.wMonth < 1 || st.wMonth > 12 || st.wDay < 1 || st.wDay > 31 ||
        st.wHour > 23 || st.wMinute > 59 || st.wSecond > 59) return false;
    FILETIME check{};
    if (!SystemTimeToFileTime( & st, & check)) return false;
    output = st;
    return true;
}
static ULONGLONG BrowserZipDateOrder(const SYSTEMTIME & t) {
    return (((((static_cast < ULONGLONG > (t.wYear) * 13 + t.wMonth) * 32 + t.wDay)
        * 24 + t.wHour) * 60 + t.wMinute) * 60 + t.wSecond);
}
static std::wstring BrowserZipDateLabel(const SYSTEMTIME & local) {
    wchar_t date[80]{},time[80]{};
    if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT,DATE_SHORTDATE, & local,nullptr,
                         date,static_cast < int > (_countof(date)),nullptr) ||
        !GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,TIME_NOSECONDS, & local,nullptr,
                         time,static_cast < int > (_countof(time)))) return {};
    return std::wstring(date) + L" " + time;
}
static std::wstring BrowserZipFolderKey(std::wstring path) {
    for (auto & ch:path) {
        if (ch == L'/') ch = L'\\';
        else ch = static_cast < wchar_t > (std::towlower(ch));
    }
    while (!path.empty() && path.back() == L'\\') path.pop_back();
    return path;
}
static void BrowserZipUpdateFolder(BrowserZipFolderDates & dates,
        std::wstring_view folderPrefix,const std::wstring & name,bool directory,
        WORD dosDate,WORD dosTime) {
    if (!name.starts_with(folderPrefix)) return;
    const std::wstring_view relative = std::wstring_view(name).substr(folderPrefix.size());
    const size_t separator = relative.find(L'\\');
    const bool stored = separator == std::wstring_view::npos;
    if (relative.empty() || (stored && !directory) || !SafeRelativePath(name)) return;
    SYSTEMTIME st{};
    if (!BrowserZipDosTime(dosDate,dosTime,st)) return;

    // SafeRelativePath limits each leaf to 255 characters. Store only the
    // direct child name, regardless of the depth of the central directory entry.
    constexpr size_t kMaxFolderDates = 8192;
    std::wstring child(relative.substr(0,separator));
    auto it = dates.find(child);
    if (it == dates.end()) {
        if (dates.size() < kMaxFolderDates)
            dates.emplace(std::move(child),BrowserZipFolderDate{st,stored});
        return;
    }
    // Real directory timestamps outrank those derived from children.
    if (stored && !it->second.explicitFolder) {it->second = {st,true};return;}
    if (stored == it->second.explicitFolder &&
        BrowserZipDateOrder(st) > BrowserZipDateOrder(it->second.local))
        it->second.local = st;
}
static bool BrowserReadZipFolderDates(const std::wstring & archive,
                                     const std::wstring & folder,
                                     BrowserZipFolderDates & dates) try {
    dates.clear();
    if (CancelledForCurrentOperation() || archive.size() < 4 ||
        _wcsicmp(archive.c_str() + archive.size() - 4,L".zip") != 0)
        return false;
    std::wstring folderPrefix = BrowserZipFolderKey(folder);
    if (!folderPrefix.empty()) folderPrefix += L'\\';
    HANDLE file = CreateFileW(archive.c_str(),GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    const std::unique_ptr<void,decltype(&CloseHandle)> fileGuard(file,CloseHandle);
    // The end-of-central-directory record is at most 65,557 bytes from EOF.
    LARGE_INTEGER fileLength{};
    constexpr DWORD kTailMax = 65557;
    constexpr DWORD kDirectoryMax = 32 * 1024 * 1024;
    bool ok = false;
    do {
        if (!GetFileSizeEx(file, & fileLength) || fileLength.QuadPart < 22) break;
        const DWORD tailLength = static_cast < DWORD > (std::min < LONGLONG > (fileLength.QuadPart,kTailMax));
        std::vector < BYTE > tail(tailLength);
        LARGE_INTEGER from{};from.QuadPart = fileLength.QuadPart - tailLength;
        DWORD got = 0;
        if (!SetFilePointerEx(file,from,nullptr,FILE_BEGIN) ||
            !ReadFile(file,tail.data(),tailLength, & got,nullptr) || got != tailLength) break;
        size_t eocd = tailLength;
        for (size_t i = tailLength - 22;;--i) {
            if (BrowserZipU32(tail.data() + i) == 0x06054b50 &&
                i + 22 + BrowserZipU16(tail.data() + i + 20) == tailLength) {eocd = i;break;}
            if (!i) break;
        }
        if (eocd == tailLength) break;
        const BYTE * end = tail.data() + eocd;
        if (BrowserZipU16(end + 4) != 0 || BrowserZipU16(end + 6) != 0 ||
            BrowserZipU16(end + 8) != BrowserZipU16(end + 10)) break; // multi-disk
        const DWORD records = BrowserZipU16(end + 10);
        const DWORD dirLength = BrowserZipU32(end + 12);
        const DWORD dirOffset = BrowserZipU32(end + 16);
        if (records == 0xFFFF || dirLength == 0xFFFFFFFF ||
            dirOffset == 0xFFFFFFFF || dirLength > kDirectoryMax ||
            static_cast < ULONGLONG > (dirOffset) + dirLength >
                static_cast < ULONGLONG > (fileLength.QuadPart)) break; // Zip64: skip
        if (CancelledForCurrentOperation()) break;
        std::vector < BYTE > directory(dirLength);
        from.QuadPart = dirOffset;
        got = 0;
        if (!SetFilePointerEx(file,from,nullptr,FILE_BEGIN) ||
            (dirLength && (!ReadFile(file,directory.data(),dirLength, & got,nullptr) ||
                got != dirLength))) break;
        size_t at = 0;
        bool malformed = false;
        for (DWORD i = 0;i < records;++i) {
            if (i % 128 == 0 && CancelledForCurrentOperation()) {
                dates.clear();
                return false;
            }
            if (at > directory.size() || directory.size() - at < 46 ||
                BrowserZipU32(directory.data() + at) != 0x02014b50) {malformed = true;break;}
            const BYTE * record = directory.data() + at;
            const WORD nameBytes = BrowserZipU16(record + 28);
            const size_t advance = 46ULL + nameBytes +
                BrowserZipU16(record + 30) + BrowserZipU16(record + 32);
            if (advance > directory.size() - at) {malformed = true;break;}
            const bool utf8 = (BrowserZipU16(record + 8) & 0x0800) != 0;
            const UINT page = utf8?CP_UTF8:437;
            const DWORD flags = utf8?MB_ERR_INVALID_CHARS:0;
            const char * raw = reinterpret_cast < const char *  > (record + 46);
            const int count = MultiByteToWideChar(page,flags,raw,nameBytes,nullptr,0);
            if (count > 0 && count <= 2048) {
                std::wstring name(static_cast < size_t > (count),L'\0');
                if (MultiByteToWideChar(page,flags,raw,nameBytes,name.data(),count) == count) {
                    const bool directoryEntry = name.back() == L'/' || name.back() == L'\\';
                    name = BrowserZipFolderKey(std::move(name));
                    BrowserZipUpdateFolder(dates,folderPrefix,name,directoryEntry,
                        BrowserZipU16(record + 14),BrowserZipU16(record + 12));
                }
            }
            at += advance;
        }
        ok = !malformed && !CancelledForCurrentOperation();
    } while(false);
    if (!ok) dates.clear();
    return ok;
} catch (const std::bad_alloc &) {
    dates.clear();
    return false;
}

static std::wstring BrowserItemSize(IShellItem * item, ULONGLONG & bytes, bool & hasSize) {
    bytes = 0;
    hasSize = false;
    IShellItem2 * info = nullptr;
    if (!item || FAILED(item->QueryInterface(IID_PPV_ARGS( & info))) || !info) return {};
    hasSize = SUCCEEDED(info->GetUInt64(PKEY_Size, & bytes));
    info->Release();
    return hasSize ? BrowserFormatSize(bytes) : L"";
}

// Native marquee is preserved on genuinely blank space. Row-width hit tests
// and a targeted fallback drag gesture prevent false marquee starts.
// LVS_EX_DOUBLEBUFFER keeps the native alpha-blended rectangle.
// Draw the selected state ourselves, because a medium-integrity ListView
// embedded into the low-integrity Preview Host can lose the themed highlight.
static LRESULT BrowserCustomDraw(BrowserState * s, NMLVCUSTOMDRAW * draw) {
    if (!s || !draw) return CDRF_DODEFAULT;
    if (draw->nmcd.dwDrawStage == CDDS_PREPAINT)
        return CDRF_NOTIFYITEMDRAW;
    if (draw->nmcd.dwDrawStage != CDDS_ITEMPREPAINT) return CDRF_DODEFAULT;
    const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
    if (diagnostics) ++s->diagnosticRowPaints;
    const size_t index = static_cast < size_t > (draw->nmcd.dwItemSpec);
    if (index >= s->entries.size()) {
        if (diagnostics) ++s->diagnosticInvalidRows;
        return CDRF_DODEFAULT;
    }
    HDC dc = draw->nmcd.hdc;
    RECT row{};
    if (!ListView_GetItemRect(s->list,static_cast < int > (index), & row,LVIR_BOUNDS))
        return CDRF_DODEFAULT;
    RECT client{};GetClientRect(s->list, & client);
    row.left = 0;row.right = client.right;
    // The native owner-data ListView may reuse the HDC when moving between
    // clipped paint regions. Restrict all custom icon/text drawing to this row.
    const int savedDc = SaveDC(dc);
    if (savedDc) IntersectClipRect(dc,row.left,row.top,row.right,row.bottom);
    const bool selected = (ListView_GetItemState(s->list,static_cast < int > (index),LVIS_SELECTED) & LVIS_SELECTED) != 0;
    const COLORREF background = BrowserBackground(s);
    const COLORREF chosen = s->paint.ready?s->paint.accent:
        ArchivePreviewHost::SystemAccentColor(s->highContrast);
    const bool hovered = !s->highContrast && !s->dragging &&
        s->hoverIndex == static_cast < int > (index);
    const COLORREF foreground = s->highContrast ?
        GetSysColor(selected?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT):
        (selected?s->paint.selectedText:BrowserForeground(s));
    HBRUSH base = s->paint.background ? s->paint.background : CreateSolidBrush(background);
    if (base) FillRect(dc, & row,base);
    if (base && base != s->paint.background) DeleteObject(base);
    if (selected || hovered) {
        const COLORREF shade = selected ? chosen : BrowserPanel(s);
        const int inset = BrowserPx(s,2);
        HPEN cachedPen = selected ? s->paint.selectionPen : s->paint.hoverPen;
        HBRUSH cachedFill = selected ? s->paint.selection : s->paint.hover;
        HPEN pen = cachedPen ? cachedPen : CreatePen(PS_SOLID,1,shade);
        HBRUSH fill = cachedFill ? cachedFill : CreateSolidBrush(shade);
        if (pen && fill) {
            HGDIOBJ oldPen = SelectObject(dc,pen),oldBrush = SelectObject(dc,fill);
            RoundRect(dc,inset,row.top + BrowserPx(s,1),row.right - inset,
                row.bottom - BrowserPx(s,1),BrowserPx(s,5),BrowserPx(s,5));
            SelectObject(dc,oldBrush);SelectObject(dc,oldPen);
        }
        if (pen && pen != cachedPen) DeleteObject(pen);
        if (fill && fill != cachedFill) DeleteObject(fill);
    }
    HFONT oldFont = static_cast < HFONT > (SelectObject(dc,s->rowFont ? s->rowFont : GetStockObject(DEFAULT_GUI_FONT)));
    const int oldMode = SetBkMode(dc,TRANSPARENT);
    const COLORREF oldColor = SetTextColor(dc,foreground);
    const int origin = -ArchivePreviewColumns::HorizontalOffset(s->list);
    const int left = origin + BrowserPx(s,8);
    const int iconSize = s->iconPixels;
    const int rowHeight = static_cast < int > (row.bottom - row.top);
    const int iconY = static_cast < int > (row.top) + std::max(0, (rowHeight - iconSize) / 2);
    const auto & entry = s->entries[index];
    if (entry.iconImage || (s->shellIcons && entry.icon >= 0)) {
        // ImageList_DrawEx(cx,cy) crops a source icon; it does NOT resize it.
        // Draw the retained icon copy at the DPI-adjusted size. Only the
        // allocation-limit fallback below needs a temporary HICON.
        HICON icon = entry.iconImage?entry.iconImage->handle:
            ImageList_GetIcon(s->shellIcons,entry.icon,ILD_TRANSPARENT);
        if (icon) {
            const BOOL drawn = DrawIconEx(dc,left,iconY,icon,iconSize,iconSize,0,nullptr,DI_NORMAL);
            if (diagnostics && drawn) ++s->diagnosticIconDraws;
            else if (diagnostics) ++s->diagnosticIconMisses;
            if (!entry.iconImage) DestroyIcon(icon);
        } else if (diagnostics) ++s->diagnosticIconMisses;
    } else if (diagnostics) ++s->diagnosticIconMisses;
    const int nameWidth = ListView_GetColumnWidth(s->list,0);
    RECT name{left + iconSize + BrowserPx(s,7),row.top,
        origin + nameWidth - BrowserPx(s,8),row.bottom};
    if (name.right > name.left)
        DrawTextW(dc,entry.name.c_str(),-1,&name,DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    int columnLeft = origin + nameWidth;
    for(size_t col = 1;col < s->visibleColumns.size();++col) {
        const BrowserColumnKind kind = s->visibleColumns[col];
        const int width = ListView_GetColumnWidth(s->list,static_cast < int > (col));
        const std::wstring & value = BrowserEntryColumn(entry,kind);
        RECT cell{columnLeft + BrowserPx(s,6),row.top,
            columnLeft + width - BrowserPx(s,10),row.bottom};
        if(!value.empty() && cell.right > cell.left)
            DrawTextW(dc,value.c_str(), - 1, & cell,DT_SINGLELINE | DT_VCENTER |
                DT_END_ELLIPSIS | DT_NOPREFIX |
                (kind == BrowserColumnKind::Size ? DT_RIGHT : DT_LEFT));
        columnLeft += width;
    }
    SetTextColor(dc,oldColor);SetBkMode(dc,oldMode);
    SelectObject(dc,oldFont);
    if (savedDc) RestoreDC(dc,savedDc);
    return CDRF_SKIPDEFAULT;
}
static void BrowserPaintHeader(BrowserState * s, HDC dc, const RECT & client) {
    if (!s) return;
    const int height = BrowserHeaderHeight(s);
    RECT header = client;header.bottom = height;
    HBRUSH fill = s->paint.header ? s->paint.header :
        CreateSolidBrush(BrowserPanel(s));
    if (fill) FillRect(dc, & header,fill);
    if (fill && fill != s->paint.header) DeleteObject(fill);
    HFONT oldFont = static_cast < HFONT > (SelectObject(dc,s->headerFont ? s->headerFont : GetStockObject(DEFAULT_GUI_FONT)));
    const COLORREF oldColor = SetTextColor(dc,BrowserMuted(s));
    const int oldMode = SetBkMode(dc,TRANSPARENT);
    const int origin = s->list ? -ArchivePreviewColumns::HorizontalOffset(s->list) : 0;
    const int nameWidth = s->list?ListView_GetColumnWidth(s->list,0):client.right;
    const int textDc = SaveDC(dc);
    if (textDc && s->list)
        IntersectClipRect(dc,0,0,ArchivePreviewColumns::ViewportWidth(s->list),height);
    RECT name{origin + BrowserPx(s,13),0,origin + nameWidth - BrowserPx(s,6),height};
    const std::wstring nameCaption = BrowserHeaderCaption(s,BrowserColumnKind::Name);
    if (name.right > name.left)
        DrawTextW(dc,nameCaption.c_str(),-1,&name,
            DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | ArchivePreviewText::TextDirection());
    int columnLeft = origin + nameWidth;
    for(size_t col = 1;col < s->visibleColumns.size();++col) {
        const BrowserColumnKind kind = s->visibleColumns[col];
        const int width = ListView_GetColumnWidth(s->list,static_cast < int > (col));
        RECT cell{columnLeft + BrowserPx(s,6),0,columnLeft + width - BrowserPx(s,10),height};
        if(cell.right > cell.left) {
            const std::wstring caption = BrowserHeaderCaption(s,kind);
            DrawTextW(dc,caption.c_str(), - 1, & cell,
                DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX | ArchivePreviewText::TextDirection() |
                (kind == BrowserColumnKind::Size ? DT_RIGHT : DT_LEFT));
        }
        columnLeft += width;
    }
    if (textDc) RestoreDC(dc,textDc);
    // No vertical separators between columns. Keep only the bottom rule.
    HPEN divider = s->paint.headerDivider ? s->paint.headerDivider :
        CreatePen(PS_SOLID,1,BrowserBorder(s));
    if (divider) {
        HGDIOBJ oldPen = SelectObject(dc,divider);
        MoveToEx(dc,0,height - 1,nullptr);LineTo(dc,client.right,height - 1);
        SelectObject(dc,oldPen);
        if (divider != s->paint.headerDivider) DeleteObject(divider);
    }
    SetTextColor(dc,oldColor);SetBkMode(dc,oldMode);SelectObject(dc,oldFont);
}
// The Broker owns both the rows and the header. The header has its own HWND,
// so Explorer / prevhost parent invalidations cannot leave a white unpainted
// strip behind the list. No custom painting is added to the marquee or rows.
static int BrowserHeaderColumnAt(BrowserState * s,int x,bool edges) {
    if (!s || !s->list || x < 0 || x >= ArchivePreviewColumns::ViewportWidth(s->list)) return -1;
    x += ArchivePreviewColumns::HorizontalOffset(s->list);
    int left = 0;
    for (int i = 0; i < static_cast<int>(s->visibleColumns.size()); ++i) {
        const int width = ListView_GetColumnWidth(s->list,i);
        const int right = left + width;
        if (edges) {
            if (width > 0 && std::abs(x - right) <= BrowserPx(s,5) &&
                ArchivePreviewColumns::DragWidth(s->list,s->currentDpi,
                    static_cast<unsigned>(s->visibleColumns[i]),width) >= 0) return i;
        } else if (x >= left && x < right) return i;
        left = right;
    }
    return -1;
}
static void BrowserSaveColumnPreference(const ArchivePreviewColumns::Change & change) {
    if (!ArchivePreviewColumns::Save(change)) ArchivePreviewColumns::UseForSession(change);
    if (HWND manager = g_gripManager.load())
        PostMessageW(manager,WM_BROWSER_COLUMN_PREFERENCES_CHANGED,change.visibility,0);
}
static LRESULT CALLBACK BrowserHeaderProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto * cs = reinterpret_cast < CREATESTRUCTW *  > (lp);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast < LONG_PTR > (cs->lpCreateParams));
    }
    auto * s = reinterpret_cast < BrowserState *  > (GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    switch(msg) {
      case WM_RBUTTONUP:
      case WM_CONTEXTMENU:
        if(s){
            POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(msg==WM_RBUTTONUP)ClientToScreen(hwnd,&point);
            ArchivePreviewColumns::Change change;
            if (ArchivePreviewColumns::VisibilityMenu(hwnd,point,change))
                BrowserSaveColumnPreference(change);
        }
        return 0;

      case WM_SETCURSOR:
        if(s && LOWORD(lp)==HTCLIENT){
            POINT point{};GetCursorPos(&point);ScreenToClient(hwnd,&point);
            if(s->resizingColumn>=0 || BrowserHeaderColumnAt(s,point.x,true)>=0){
                SetCursor(LoadCursorW(nullptr,IDC_SIZEWE));return TRUE;
            }
        }
        break;
      case WM_LBUTTONDOWN:
        if(s && !s->dragging){
            const int x=GET_X_LPARAM(lp);
            const int edge=BrowserHeaderColumnAt(s,x,true);
            s->pressedHeader=-1;
            if(edge>=0){
                s->resizingColumn=edge;s->resizeStartX=x;
                s->resizeStartWidth=ListView_GetColumnWidth(s->list,edge);
                SetCapture(hwnd);
            }else{s->pressedHeader=BrowserHeaderColumnAt(s,x,false);SetCapture(hwnd);}
        }
        return 0;
      case WM_MOUSEMOVE:
        if(s && s->resizingColumn>=0 && GetCapture()==hwnd){
            const int column = s->resizingColumn;
            const int delta = GET_X_LPARAM(lp) - s->resizeStartX;
            const unsigned kind = static_cast<unsigned>(s->visibleColumns[column]);
            const int width = delta ? ArchivePreviewColumns::DragWidth(s->list,s->currentDpi,
                kind,s->resizeStartWidth + delta) : s->resizeStartWidth;
            if (width >= 0 && width != ListView_GetColumnWidth(s->list,column)) {
                ListView_SetColumnWidth(s->list,column,width);
                if (ArchivePreviewOptions::hideHorizontalScrollbar.load())
                    ArchivePreviewColumns::HideScrollbar(s->list);
            }
            InvalidateRect(hwnd,nullptr,FALSE);InvalidateRect(s->list,nullptr,FALSE);
            SetCursor(LoadCursorW(nullptr,IDC_SIZEWE));
        }
        return 0;
      case WM_LBUTTONUP:
        if(s){
            const bool resized=s->resizingColumn>=0;
            const int pressed=s->pressedHeader;
            if (resized) {
                const int column = s->resizingColumn;
                const unsigned kind = static_cast<unsigned>(s->visibleColumns[column]);
                const int width = ListView_GetColumnWidth(s->list,column);
                if (width != s->resizeStartWidth) {
                    const int dips = MulDiv(width,96,s->currentDpi ? s->currentDpi : 96);
                    BrowserSaveColumnPreference({kind,dips,0});
                }
            }
            s->resizingColumn=-1;s->pressedHeader=-1;
            if(GetCapture()==hwnd)ReleaseCapture();
            if (resized) BrowserLayoutColumns(s);
            if(!resized && pressed>=0 && !s->modelPending && !s->dragging && !s->updating &&
                pressed==BrowserHeaderColumnAt(s,GET_X_LPARAM(lp),false))
                BrowserSortByColumn(s,s->visibleColumns[static_cast<size_t>(pressed)]);
        }
        return 0;
      case WM_CANCELMODE:
      case WM_CAPTURECHANGED:
        if (s) {
            const bool resized = s->resizingColumn >= 0;
            s->resizingColumn = -1;
            s->pressedHeader = -1;
            if (GetCapture() == hwnd) ReleaseCapture();
            if (resized) BrowserLayoutColumns(s);
        }
        return 0;
      case WM_ERASEBKGND:
          if (s && ArchivePreviewDiagnostics::Enabled()) ++s->headerErases;
          // WM_ERASEBKGND is sometimes sent without a subsequent WM_PAINT
          // during prevhost/Explorer transitions. Supply the dark content now.
          if (s && wp) {
              RECT area{};GetClientRect(hwnd, & area);
              BrowserPaintHeader(s,reinterpret_cast < HDC > (wp),area);
          }
          return 1;
      case WM_PAINT: {
          if (s && ArchivePreviewDiagnostics::Enabled()) ++s->headerPaints;
          PAINTSTRUCT ps{};
          HDC target = BeginPaint(hwnd, & ps);
          if (target && s) {
              RECT r{};GetClientRect(hwnd, & r);
              const int width = std::max(0,static_cast < int > (r.right - r.left));
              const int height = std::max(0,static_cast < int > (r.bottom - r.top));
              HDC memory = s->headerSurface.Ensure(target,width,height);
              BrowserPaintHeader(s,memory?memory:target,r);
              if (memory) BitBlt(target,0,0,width,height,memory,0,0,SRCCOPY);
          }
          EndPaint(hwnd, & ps);
          return 0;
      }
      case WM_PRINT:
      case WM_PRINTCLIENT:
          if (wp && s && (msg == WM_PRINTCLIENT || (lp & PRF_CLIENT))) {
              RECT r{};GetClientRect(hwnd, & r);
              BrowserPaintHeader(s,reinterpret_cast < HDC > (wp),r);
          }
          return 0;
      case WM_THEMECHANGED:
      case WM_SYSCOLORCHANGE:
      case WM_SETTINGCHANGE:
      case WM_DPICHANGED_AFTERPARENT:
          InvalidateRect(hwnd,nullptr,FALSE);
          return 0;
      case WM_NCDESTROY:
          if (s) s->headerSurface.Reset();
          if (s && s->header == hwnd) s->header = nullptr;
          SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
          return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
static void BrowserRedrawHeader(BrowserState * s, bool immediate) {
    // Most immediate=true callers only change the footer/status. Skip a full
    // header repaint then; actual column/layout/theme changes use false.
    if (!s) return;
    // Existing status call sites still request a header redraw. Reuse that
    // notification to reflow the status/progress strip and keep the header
    // reserved for actual column names. Do not relayout on every repaint.
    if (s->host && IsWindow(s->host)) {
        const int next = BrowserProgressHeight(s);
        if (next != s->statusStripPixels) {
            BrowserLayoutProgress(s);
            InvalidateRect(s->host,nullptr,FALSE);
        } else if (next) {
            RECT area{};GetClientRect(s->host, & area);
            RECT strip = BrowserProgressRect(s,area);
            InvalidateRect(s->host, & strip,FALSE);
        }
    }
    // the status text is in the footer; the header only needs one
    // invalidation. Never erase into the parent's default (white) brush.
    BrowserInvalidateFooter(s);
    if (!immediate && s->header && IsWindow(s->header))
        RedrawWindow(s->header,nullptr,nullptr,RDW_INVALIDATE | RDW_NOERASE);
}
// a precise, bounded paint diagnostic after extraction or host reuse.
// GetPixel is observation only; CLR_INVALID (occlusion/headless) is ignored.
static COLORREF BrowserChromeCorner(HWND child) {
    if (!child || !IsWindow(child) || !IsWindowVisible(child)) return CLR_INVALID;
    RECT r{};GetClientRect(child, & r);
    if (r.right < 8 || r.bottom < 8) return CLR_INVALID;
    HDC dc = GetDC(child);
    if (!dc) return CLR_INVALID;
    const COLORREF value = GetPixel(dc,3,3);
    ReleaseDC(child,dc);
    return value;
}
static bool BrowserChromeLooksWhite(COLORREF color) {
    return color != CLR_INVALID && GetRValue(color) > 220 &&
           GetGValue(color) > 220 && GetBValue(color) > 220;
}
static void BrowserChromeAudit(BrowserState * s, const wchar_t * reason,
                               bool repairIfWhite = false) {
    if (!s || s->parked || !s->host || !IsWindow(s->host)) return;
    const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
    const bool canRepair = repairIfWhite && s->dark && !s->highContrast;
    if (!canRepair && !diagnostics) return;
    const COLORREF header = BrowserChromeCorner(s->header);
    const COLORREF footer = BrowserChromeCorner(s->footer);
    const bool badHeader = s->dark && !s->highContrast && BrowserChromeLooksWhite(header);
    const bool badFooter = s->dark && !s->highContrast && BrowserChromeLooksWhite(footer);
    Wh_Log(L"ArchivePreview: chrome audit (%ls): dark=%d host=%d header=%d footer=%d "
        L"pixelH=%06lX pixelF=%06lX paintH=%u paintF=%u eraseH=%u eraseF=%u "
        L"paintHost=%u eraseHost=%u repairs=%u suspect=%d/%d",
        reason?reason:L"unspecified",s->dark?1:0,
        IsWindowVisible(s->host)?1:0,
        s->header && IsWindowVisible(s->header)?1:0,
        s->footer && IsWindowVisible(s->footer)?1:0,
        static_cast < unsigned long > (header),static_cast < unsigned long > (footer),
        s->headerPaints,s->footerPaints,s->headerErases,s->footerErases,
        s->hostPaints,s->hostErases,s->repairs,badHeader?1:0,badFooter?1:0);
    if (!repairIfWhite || (!badHeader && !badFooter)) return;
    // No parent/whole-list erase. Repaint only the offending child; never
    // move/recreate the ListView or footer controls.
    if (badHeader && s->header && IsWindow(s->header))
        RedrawWindow(s->header,nullptr,nullptr,RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    if (badFooter && s->footer && IsWindow(s->footer))
        RedrawWindow(s->footer,nullptr,nullptr,RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
}
static void BrowserApplyTheme(BrowserState * s) {
    if (!s || !s->list) return;
    s->highContrast = BrowserHighContrast();
    s->dark = !s->highContrast && BrowserDarkTheme();
    BrowserRefreshPaintColors(s);
    const COLORREF bg = BrowserBackground(s);
    const COLORREF fg = BrowserForeground(s);
    ListView_SetBkColor(s->list,bg);
    ListView_SetTextBkColor(s->list,bg);
    ListView_SetTextColor(s->list,fg);
    SetWindowTheme(s->list,s->highContrast?L"":(s->dark?L"DarkMode_Explorer":L"Explorer"),nullptr);
    // Rows and background already have explicit dark/light colors. Do not
    // force an extra default erase immediately before the owner-drawn frame.
    InvalidateRect(s->list,nullptr,FALSE);
    BrowserRedrawHeader(s,false);
    if (s->host) InvalidateRect(s->host,nullptr,FALSE);
}
static void BrowserNotifyDepth(BrowserState * s) {
    if (s && s->parent && IsWindow(s->parent))
        PostMessageW(s->parent,ARCHIVE_BROWSER_DEPTH,s->folder.empty()?0:1,0);
}
// Gather at most MAX_PATHS+1. The extra entry is a deliberate overflow marker.
// Callers MUST reject an oversized selection before sending any transfer request.
// This avoids both silent partial copies and unbounded temporary allocations.
static std::vector < std::wstring > BrowserSelection(BrowserState * s) {
    std::vector < std::wstring > selected;
    if (!s || !s->list) return selected;
    for (int i = ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);
         i >= 0 && selected.size() <= ARCHIVE_BROKER_MAX_PATHS;
         i = ListView_GetNextItem(s->list,i,LVNI_SELECTED)) {
        if (static_cast < size_t > (i) < s->entries.size())
            selected.push_back(s->entries[static_cast < size_t > (i)].relative);
    }
    return selected;
}
static void BrowserNotifySelectionCount(BrowserState * s) {
    if (s && s->list && s->parent && IsWindow(s->parent))
        PostMessageW(s->parent,ARCHIVE_BROWSER_SELECTED_COUNT,
            static_cast < WPARAM > (ListView_GetSelectedCount(s->list)),
            reinterpret_cast < LPARAM > (s->anchor));
}
static void BrowserWarnSelectionLimit(BrowserState * s) {
    if (!s) return;
    const ULONGLONG now = GetTickCount64();
    // Mouse move can attempt a drag again after the dialog closes. Avoid a
    // cascade of identical modal warnings while the user still holds the mouse.
    if (s->lastSelectionLimitWarning && now - s->lastSelectionLimitWarning < 3000) return;
    s->lastSelectionLimitWarning = now;
    wchar_t warning[280]{};
    swprintf_s(warning,
        L"Too many items selected. The limit is %lu items per transfer.\n"
        L"Nothing was copied or extracted. Please select fewer items.",
        static_cast < unsigned long > (ARCHIVE_BROKER_MAX_PATHS));
    Wh_Log(L"%s", warning);
    if (s->host && IsWindow(s->host))
        MessageBoxW(s->host,warning,L"Archive Preview",MB_OK | MB_ICONWARNING);
}
// All selected-extraction entry points go through the SAME validated selection.
// This prevents the low-integrity toolbar from reading a stale hidden ListView.
static HRESULT BrowserExtractSelected(BrowserState * s,bool here) {
    if (!s || !s->list || !IsWindow(s->list)) return E_INVALIDARG;
    if (s->dragging || s->modelPending || s->progressVisible)
        return HRESULT_FROM_WIN32(ERROR_BUSY);
    const GUID zero{};
    if (!InlineIsEqualGUID(s->pendingExtract,zero)) {
        s->extractStatus = L"An extraction is already running.";
        s->extractStatusUntil = GetTickCount64() + 3000;
        SetTimer(s->host,kBrowserExtractStatusTimer,3000,nullptr);
        BrowserRedrawHeader(s,true);
        return HRESULT_FROM_WIN32(ERROR_BUSY);
    }
    Request req{};
    req.command = here ? ARCHIVE_BROKER_EXTRACT_SELECTED_HERE :
        ARCHIVE_BROKER_EXTRACT_SELECTED_TO;
    req.archive = s->archive;
    req.items = BrowserSelection(s);
    if (req.items.empty()) {
        s->extractStatus = L"Select one or more items first.";
        s->extractStatusUntil = GetTickCount64() + 4500;
        SetTimer(s->host,kBrowserExtractStatusTimer,4500,nullptr);
        BrowserRedrawHeader(s,true);
        return E_INVALIDARG;
    }
    if (req.items.size() > ARCHIVE_BROKER_MAX_PATHS) {
        BrowserWarnSelectionLimit(s);
        return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
    }
    const HRESULT id = CoCreateGuid( & req.requestId);
    if (FAILED(id)) return id;
    DWORD ownerPid = 0;
    if (s->parent) GetWindowThreadProcessId(s->parent, & ownerPid);
    // Match the owner PID used by BrowserQueryProgress and cancellable IPC.
    const HRESULT queued = DispatchAsync(req,ownerPid);
    if (SUCCEEDED(queued)) {
        s->pendingExtract = req.requestId;
        s->extractStatus = here ? L"Extracting selected items here..." :
            L"Choose extraction folder...";
        s->extractStatusUntil = 0;
    } else {
        wchar_t message[110]{};
        swprintf_s(message,L"Selected extraction rejected: 0x%08lX",
            static_cast < unsigned long > (queued));
        s->extractStatus = message;
        s->extractStatusUntil = GetTickCount64() + 8000;
        SetTimer(s->host,kBrowserExtractStatusTimer,8000,nullptr);
    }
    BrowserRedrawHeader(s,true);
    return queued;
}
static int BrowserRowAtPoint(HWND list,POINT point);
static void BrowserShowSelectionMenu(BrowserState * s,POINT screenPoint) {
    if (!s || !s->list) return;
    POINT pt = screenPoint;
    if (screenPoint.x ==  - 1 && screenPoint.y ==  - 1) {
        int row = ListView_GetNextItem(s->list, - 1,LVNI_FOCUSED);
        if (row < 0) row = ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);
        if (row < 0) return;
        RECT bounds{};
        if (!ListView_GetItemRect(s->list,row, & bounds,LVIR_BOUNDS)) return;
        pt = {bounds.left + 12,bounds.top + 12};
        screenPoint = pt;
        ClientToScreen(s->list, & screenPoint);
    } else ScreenToClient(s->list, & pt);
    const int row = BrowserRowAtPoint(s->list,pt);
    if (row < 0 || static_cast < size_t > (row) >= s->entries.size()) return;
    if (!(ListView_GetItemState(s->list,row,LVIS_SELECTED) & LVIS_SELECTED)) {
        ListView_SetItemState(s->list, - 1,0,LVIS_SELECTED);
        ListView_SetItemState(s->list,row,LVIS_SELECTED | LVIS_FOCUSED,
            LVIS_SELECTED | LVIS_FOCUSED);
    }
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu,MF_STRING,kSelectedMenuHere,ArchivePreviewText::Get(ArchivePreviewText::Id::SelectedHere));
    AppendMenuW(menu,MF_STRING,kSelectedMenuTo,ArchivePreviewText::Get(ArchivePreviewText::Id::SelectedTo));

    const UINT selection = TrackPopupMenu(menu,TPM_RETURNCMD | TPM_NONOTIFY |
        TPM_RIGHTBUTTON | ArchivePreviewText::MenuDirection(),screenPoint.x,screenPoint.y,0,s->host,nullptr);
    DestroyMenu(menu);
    if (selection == kSelectedMenuHere) BrowserExtractSelected(s,true);
    else if (selection == kSelectedMenuTo) BrowserExtractSelected(s,false);
}
// Record the real ListView state before navigating or detaching. These values
// are independent of the HWND and survive a short Explorer preview reload.
static void BrowserSaveView(BrowserState * s) {
    if (!s || !s->list || !IsWindow(s->list) || !s->hasRenderedFolder ||
        s->partialModelPreview || s->updating) return;
    if (ListView_GetItemCount(s->list) != static_cast<int>(s->entries.size())) return;
    auto & cache = BrowserCache(s->archive);
    BrowserSavedView view{};
    view.firstVisible = ListView_GetTopIndex(s->list);
    view.focused = ListView_GetNextItem(s->list, - 1,LVNI_FOCUSED);
    if (view.focused >= 0 && static_cast<size_t>(view.focused) < s->entries.size())
        view.focusedPath = s->entries[view.focused].relative;
    if (view.firstVisible >= 0 && static_cast<size_t>(view.firstVisible) < s->entries.size())
        view.topPath = s->entries[view.firstVisible].relative;
    // View persistence is independent of transfer limits. Preserve ALL rows
    // (bounded by the 200,000-row folder model), including oversized selections.
    for (int i = ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);
         i >= 0;
         i = ListView_GetNextItem(s->list,i,LVNI_SELECTED))
        if (static_cast < size_t > (i) < s->entries.size())
            view.selectedPaths.push_back(s->entries[static_cast < size_t > (i)].relative);
    cache.views[s->renderedFolder] = std::move(view);
}
static bool BrowserSameEntries(const std::vector < BrowserEntry >  & a,const std::vector < BrowserEntry >  & b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0;i < a.size();++i)
        if (a[i].name != b[i].name || a[i].relative != b[i].relative ||
            a[i].folder != b[i].folder || a[i].sizeLabel != b[i].sizeLabel ||
            a[i].icon != b[i].icon || a[i].sizeBytes != b[i].sizeBytes ||
            a[i].typeLabel != b[i].typeLabel || a[i].modifiedLabel != b[i].modifiedLabel ||
            a[i].hasSize != b[i].hasSize || a[i].modifiedValue != b[i].modifiedValue)
            return false;
    return true;
}
// Shell namespace enumeration must stay in one initialized COM apartment.
// The UI thread never receives an IShellItem* from this worker, only row data.
struct BrowserModelJob {
    std::shared_ptr < std::atomic < bool >> cancelled;
    ArchiveFileIdentity archiveIdentity{};
    HWND anchor{};
    uint64_t instance{};
    uint64_t generation{};
    std::wstring archive;
    std::wstring folder;
    FILETIME modified{};
    ULONGLONG archiveBytes{};
    HRESULT result{E_FAIL};
    unsigned metadataMask{};
    bool heavyModelSlot{}; // worker owns a reserved heavy and global slot
    std::vector < BrowserEntry > rows;
};
// Publish bounded progressive snapshots at 1, 128 and 1024 entries.
// Large compressed archives may contain only one huge file; waiting for 128
// entries, or for tar -tf to finish decompressing 10 GiB, delays the UI.
// Actions remain disabled until the model is fully validated and committed.
struct BrowserModelPreview {
    std::shared_ptr < std::atomic < bool >> ticket;
    HWND anchor{};
    uint64_t instance{};
    uint64_t generation{};
    std::wstring archive;
    std::wstring folder;
    ULONGLONG archiveBytes{};
    unsigned metadataMask{};
    std::vector < BrowserEntry > rows;
};
static void BrowserPublishFirstRows(const BrowserModelJob & job,
                                    const std::vector < BrowserEntry >  & rows) {
    if ((rows.size() != 1 && rows.size() != 128 && rows.size() != 1024) ||
        CancelledForCurrentOperation()) return;
    // The receiver adopts this pointer into unique_ptr when it processes the
    // posted message. Delete it here if posting is not possible or fails.
    std::unique_ptr<BrowserModelPreview> preview(new(std::nothrow) BrowserModelPreview);
    if (!preview) return;
    preview->ticket = job.cancelled;
    preview->anchor = job.anchor;
    preview->instance = job.instance;
    preview->generation = job.generation;
    preview->archive = job.archive;
    preview->folder = job.folder;
    preview->archiveBytes = job.archiveBytes;
    preview->metadataMask = job.metadataMask;
    preview->rows = rows;
    PostBrowserPayload(WM_BROWSER_MODEL_PREVIEW,0,std::move(preview));
}

// Query the original archive's URL zone before invoking an independent
// provider. A provider failure must never bypass Mark-of-the-Web preview
// restrictions when Allow preview for downloaded archives is disabled.
static HRESULT BrowserBuildModel(BrowserModelJob & job) {
        const auto & archive = job.archive;
        const auto & folder = job.folder;
        auto & fresh = job.rows;
        const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
        const ULONGLONG started = diagnostics?GetTickCount64():0;
        ULONGLONG firstNameMs = 0;
        ULONGLONG metadataMs = 0;
        ULONGLONG nextMs = 0;
        ULONGLONG namesMs = 0;
        ULONGLONG attrsMs = 0;
        ULONGLONG propertiesMs = 0;
        // Shell providers may decompress members just to answer PKEY_Size or
        // PKEY_DateModified. On large physical archives, show file names and
        // folder structure. Request optional properties only for visible
        // columns, including dates when enabled on large archives.
        // This does NOT extract files, and leaves all transfer/extract safety
        // checks in their own code paths unchanged. Unavailable values stay blank.
        BrowserZipFolderDates zipFolderDates;
        const bool wantDate = (job.metadataMask & 1u) != 0;
        const bool wantSize = (job.metadataMask & 2u) != 0;
        if (wantDate) BrowserReadZipFolderDates(archive,folder,zipFolderDates);
        if (CancelledForCurrentOperation()) return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        if (diagnostics && job.heavyModelSlot) {
            wchar_t startedLine[160]{};
            swprintf_s(startedLine,L"ArchivePreview model start: archiveMiB=%llu active=%u heavy=%u",
                job.archiveBytes / (1024ULL * 1024),
                g_activeModelWorkers.load(std::memory_order_relaxed),
                g_activeHeavyModelWorkers.load(std::memory_order_relaxed));
            Wh_Log(L"%s", startedLine);
        }
        IShellItem * root = nullptr;
        HRESULT hr = OpenArchive(archive, & root);
        if (diagnostics && job.heavyModelSlot) {
            wchar_t openLine[140]{};
            swprintf_s(openLine,L"ArchivePreview model OpenArchive: hr=0x%08lX ms=%llu",
                static_cast < unsigned long > (hr),GetTickCount64() - started);
            Wh_Log(L"%s", openLine);
        }
        IShellItem * current = nullptr;
        if (SUCCEEDED(hr)) {
            if (folder.empty()) {current = root;current->AddRef();}
            else hr = FindItem(root,folder, & current);
        }
        IEnumShellItems * enumeration = nullptr;
        if (SUCCEEDED(hr))
            hr = current->BindToHandler(nullptr,BHID_EnumItems,IID_PPV_ARGS( & enumeration));
        const ULONGLONG shellReadyMs = diagnostics?GetTickCount64() - started:0;
        if (diagnostics && job.heavyModelSlot) {
            wchar_t bindLine[160]{};
            swprintf_s(bindLine,L"ArchivePreview model BindToHandler: hr=0x%08lX ms=%llu",
                static_cast < unsigned long > (hr),shellReadyMs);
            Wh_Log(L"%s", bindLine);
        }
        // The Shell icon index is shared by ordinary files with the same
        // extension. Avoid repeating expensive PIDL/icon lookups for each row.
        // Executables, shortcuts and icon files can have file-specific icons.
        using ModelIcon = std::pair < int,std::shared_ptr < ArchivePreviewIcons::Icon >>;
        std::unordered_map < std::wstring,ModelIcon > iconByType;
        std::unordered_map < std::wstring,std::wstring > typeByExtension;
        bool firstNextLogSent = false;
        if (SUCCEEDED(hr)) for (;;) {
            if(CancelledForCurrentOperation()){hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);break;}
            if (diagnostics && job.heavyModelSlot && !firstNextLogSent) {
                Wh_Log(L"ArchivePreview model: waiting for first EnumItems::Next");
                firstNextLogSent = true;
            }
            IShellItem * item = nullptr;ULONG fetched = 0;
            const ULONGLONG nextStart = diagnostics?GetTickCount64():0;
            const HRESULT next = enumeration->Next(1, & item, & fetched);
            if (diagnostics) nextMs += GetTickCount64() - nextStart;
            if (next == S_FALSE) {hr = S_OK;break;}
            if (FAILED(next) || !item || !fetched) {
                if (item) item->Release();
                hr = FAILED(next)?next:E_FAIL;break;
            }
            if (fresh.size() >= kBrowserFolderMaxEntries) {
                item->Release();
                hr = HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
                Wh_Log(L"Archive folder exceeds 200,000 entries; use paged recovery view");
                break;
            }
            const ULONGLONG itemStart = diagnostics?GetTickCount64():0;
            BrowserEntry entry{};
            const ULONGLONG nameStart = diagnostics?GetTickCount64():0;
            entry.name = GetName(item);
            if (diagnostics) namesMs += GetTickCount64() - nameStart;
            if (entry.name.empty()) {item->Release();continue;}
            if (diagnostics && !firstNameMs) firstNameMs = GetTickCount64() - started;
            entry.relative = folder.empty()?entry.name:folder + L"\\" + entry.name;
            if (!SafeRelativePath(entry.relative)) {item->Release();continue;}
            // Publish the first safe name even before SFGAO_FOLDER. Some
            // providers scan an archive during GetAttributes, so waiting for
            // this flag can delay the entire preview. A partial row is
            // deliberately non-actionable until enumeration completes.
            if (fresh.empty()) {
                std::vector < BrowserEntry > first{entry};
                BrowserPublishFirstRows(job,first);
            }
            SFGAOF attrs = SFGAO_FOLDER;
            const ULONGLONG attrsStart = diagnostics?GetTickCount64():0;
            entry.folder = SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, & attrs)) && (attrs & SFGAO_FOLDER);
            if (diagnostics) attrsMs += GetTickCount64() - attrsStart;
            if (wantSize || wantDate) {
                const ULONGLONG propertiesStart = diagnostics?GetTickCount64():0;
                if (!entry.folder && wantSize) {
                    entry.sizeLabel = BrowserItemSize(item,entry.sizeBytes,entry.hasSize);
                    if (entry.hasSize && entry.sizeBytes == 0 &&
                        ArchivePreviewIsSingleStreamArchive(archive)) {
                        entry.hasSize = false;
                        entry.sizeLabel.clear();
                    }
                }
                if (wantDate) {
                    entry.modifiedLabel = BrowserModifiedLabel(item,entry.modifiedValue);
                    if (entry.folder && entry.modifiedLabel.empty() && !zipFolderDates.empty()) {
                        const auto found = zipFolderDates.find(BrowserZipFolderKey(entry.name));
                        if (found != zipFolderDates.end()) {
                            entry.modifiedLabel = BrowserZipDateLabel(found->second.local);
                            FILETIME local{},utc{};
                            if (SystemTimeToFileTime(&found->second.local,&local) && LocalFileTimeToFileTime(&local,&utc))
                                entry.modifiedValue = ArchivePreviewSort::FileTimeValue(utc);
                        }
                    }
                }
                if (diagnostics) propertiesMs += GetTickCount64() - propertiesStart;
            }
            entry.typeLabel = BrowserTypeLabel(entry.name,entry.folder,typeByExtension);
            // Query a synthetic file attribute only once per extension.
            // PIDL-based GetFileInfo can trigger a costly archive-provider lookup
            // for EACH member (and can hang while scanning a large RAR/7z).
            std::wstring typeKey = entry.folder?L"<folder>":L"<file>";
            if (!entry.folder) {
                const size_t dot = entry.name.find_last_of(L'.');
                if (dot != std::wstring::npos && dot + 1 < entry.name.size()) {
                    typeKey = entry.name.substr(dot);
                    std::transform(typeKey.begin(),typeKey.end(),typeKey.begin(),
                        [](wchar_t c){return static_cast < wchar_t > (std::towlower(c));});
                }
            }
            const auto cached = iconByType.find(typeKey);
            if (cached != iconByType.end()) {
                entry.icon = cached->second.first;
                entry.iconImage = cached->second.second;
            }
            else {
                SHFILEINFOW fi{};
                const DWORD flags = entry.folder?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_NORMAL;
                const DWORD_PTR icon = SHGetFileInfoW(entry.folder?L"folder":entry.name.c_str(),
                    flags, & fi,sizeof(fi),SHGFI_USEFILEATTRIBUTES |
                    SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
                if (icon) {
                    entry.icon = fi.iIcon;
                    entry.iconImage = ArchivePreviewIcons::Capture(
                        reinterpret_cast < HIMAGELIST >(icon),entry.icon);
                    if (iconByType.size() < 1024)
                        iconByType.emplace(typeKey,ModelIcon{entry.icon,entry.iconImage});
                }
            }
            fresh.push_back(std::move(entry));
            if (diagnostics) metadataMs += GetTickCount64() - itemStart;
            if (fresh.size() == 128 || fresh.size() == 1024)
                BrowserPublishFirstRows(job,fresh);
            item->Release();
        }
        if (enumeration) enumeration->Release();
        if (current) current->Release();
        if (root) root->Release();
        if (FAILED(hr)) return hr;
        std::stable_sort(fresh.begin(),fresh.end(),[](const BrowserEntry & a,const BrowserEntry & b){
            if (a.folder != b.folder) return a.folder > b.folder;
            return CompareStringOrdinal(a.name.c_str(), - 1,b.name.c_str(), - 1,TRUE) == CSTR_LESS_THAN;
        });
    if (diagnostics)
        Wh_Log(L"[AP model] archive_mib=%llu shell_ms=%llu first_name_ms=%llu total_ms=%llu next_ms=%llu name_ms=%llu attributes_ms=%llu properties_ms=%llu item_ms=%llu rows=%zu",
            job.archiveBytes / (1024ULL * 1024),shellReadyMs,firstNameMs,GetTickCount64() - started,
            nextMs,namesMs,attrsMs,propertiesMs,metadataMs,fresh.size());
    return S_OK;
}
// A single expensive archive/folder/signature may be requested by many
// short-lived Explorer preview instances. Coalesce those requests into ONE
// ongoing COM enumeration. This map is broker-UI-thread-owned; no worker ever
// mutates it. The token is a lifetime identity, not a shared cancellation right
// for arbitrary subscribers.
struct BrowserModelInflight {
    std::shared_ptr < std::atomic < bool >> ticket;
    std::wstring archive;
    std::wstring folder;
    FILETIME stamp{};
    ULONGLONG bytes{};
    ArchiveFileIdentity identity{};
    unsigned metadataMask{};
};
static std::vector < BrowserModelInflight > g_browserModelInflight;
static bool BrowserSameInflight(const BrowserModelInflight & record,
        const std::wstring & archive,const std::wstring & folder,
        const FILETIME & stamp,ULONGLONG bytes,const ArchiveFileIdentity & identity,
        unsigned metadataMask) {
    return record.metadataMask == metadataMask && !record.ticket->load(std::memory_order_acquire) &&
        _wcsicmp(record.archive.c_str(),archive.c_str()) == 0 &&
        _wcsicmp(record.folder.c_str(),folder.c_str()) == 0 &&
        record.bytes == bytes && record.identity == identity &&
        CompareFileTime( & record.stamp, & stamp) == 0;
}
static void BrowserCancelOutdatedMetadata() {
    for (const auto & active : g_browserModelInflight)
        if (active.metadataMask != ArchivePreviewOptions::MetadataMask(active.bytes))
            active.ticket->store(true,std::memory_order_release);
}
static void BrowserForgetInflight(const std::shared_ptr < std::atomic < bool >>  & ticket) {
    std::erase_if(g_browserModelInflight,[ & ](const BrowserModelInflight & r) {
        return r.ticket == ticket;
    });
}
static BrowserState * BrowserFindModelSubscriber(
        const std::shared_ptr < std::atomic < bool >>  & ticket,
        const std::wstring & archive,const std::wstring & folder) {
    for (auto & [anchor,browser]:g_browsers) {
        BrowserState * state = browser.get();
        if (!state || state->parked || !state->modelPending ||
            state->modelCancelled != ticket ||
            _wcsicmp(state->archive.c_str(),archive.c_str()) != 0 ||
            _wcsicmp(state->folder.c_str(),folder.c_str()) != 0) continue;
        return state;
    }
    return nullptr;
}
static DWORD WINAPI BrowserModelWorker(LPVOID raw) {
    // Global shutdown owns the worker handle and joins before unloading.
    // The UI manager consumes this pointer on a successful PostMessageW.
    // Otherwise the worker deletes it. Neither path ignores an ownership result.
    std::unique_ptr<BrowserModelJob> job(static_cast<BrowserModelJob*>(raw));
    const HRESULT initialized = OleInitialize(nullptr);
    if (SUCCEEDED(initialized)) {
        ScopedOperationCancellation scope(job->cancelled.get());
        job->result = BrowserBuildModel( * job);
        OleUninitialize();
    } else job->result = initialized;
    // Snapshot before transferring ownership of job to the UI thread.
    const bool heavyModelSlot = job->heavyModelSlot;
    if (FAILED(PostBrowserPayload(WM_BROWSER_MODEL_READY,0,std::move(job))))
        Wh_Log(L"Background archive model discarded: browser manager exited");
    BrowserReleaseModelWorker(heavyModelSlot);
    return 0;
}
static HRESULT BrowserScheduleModel(BrowserState * s,bool bypassDebounce = false) {
    if (!s || !s->host) return E_POINTER;
    FILETIME archiveStamp{};
    ULONGLONG archiveBytes{};
    ArchiveFileIdentity identity{};
    if (!GetArchiveSignature(s->archive,archiveStamp,archiveBytes, & identity))
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    const bool heavy = BrowserHeavyModelRequest(s->archive,archiveBytes);
    const unsigned metadataMask = ArchivePreviewOptions::MetadataMask(archiveBytes);
    // A reattached pane subscribes to the same worker instead of requesting
    // another blocking IEnumShellItems::Next on the same 10 GiB archive.
    for (const auto & active:g_browserModelInflight) {
        if (BrowserSameInflight(active,s->archive,s->folder,archiveStamp,
                                archiveBytes,identity,metadataMask)) {
            if (s->modelPending && s->modelCancelled == active.ticket) return S_OK;
            KillTimer(s->host,kBrowserHeavyDebounceTimer);
            KillTimer(s->host,kBrowserModelRetryTimer);
            s->heavyModelDeferred = false;
            s->modelCancelled = active.ticket;
            s->modelPending = true;
            s->modelPendingFolder = s->folder;
            Wh_Log(L"ArchivePreview: reusing in-flight archive enumeration; no extra COM thread");
            return S_OK;
        }
    }
    s->modelPending = false;
    s->modelPendingFolder = s->folder;
    s->modelCancelled.reset();
    // A quick sequence of selections should not start COM work for every
    // large archive the cursor passes. A stable selection starts after 550ms.
    if (heavy && !bypassDebounce) {
        if (!SetTimer(s->host,kBrowserHeavyDebounceTimer,
                      kBrowserHeavyDebounceMs,nullptr))
            return HRESULT_FROM_WIN32(GetLastError());
        s->heavyModelDeferred = true;
        return S_OK;
    }
    s->heavyModelDeferred = false;
    KillTimer(s->host,kBrowserHeavyDebounceTimer);
    // Reserve a heavy slot *before* a global slot. Both reservations are
    // released only once the real worker has returned from COM and exited.
    // Never pretend that a hung COM call has finished just to free capacity.
    if (!BrowserReserveModelWorker(heavy)) {
        // Retries do not spawn more threads while old providers are blocked.
        // A small archive can still consume either reserved normal slot.
        static std::atomic < ULONGLONG > lastCapacityLog{0};
        const ULONGLONG now = GetTickCount64();
        ULONGLONG previous = lastCapacityLog.load(std::memory_order_relaxed);
        if (ArchivePreviewDiagnostics::Enabled() && (previous == 0 || now - previous >= 5000) &&
            lastCapacityLog.compare_exchange_strong(previous,now,
                std::memory_order_relaxed)) {
            wchar_t line[180]{};
            swprintf_s(line,L"ArchivePreview model capacity: heavy=%u total=%u requestedHeavy=%u",
                g_activeHeavyModelWorkers.load(std::memory_order_relaxed),
                g_activeModelWorkers.load(std::memory_order_relaxed),
                heavy?1u:0u);
            Wh_Log(L"%s", line);
        }
        if (!s->hasRenderedFolder && s->extractStatus.empty()) {
            s->extractStatus = L"Archive provider busy. Waiting for a free worker...";
            s->extractStatusUntil = 0;
            BrowserRedrawHeader(s,true);
        }
        SetTimer(s->host,kBrowserModelRetryTimer,2000,nullptr);
        return S_OK;
    }
    KillTimer(s->host,kBrowserModelRetryTimer);
    auto * job = new(std::nothrow) BrowserModelJob;
    if (!job) {
        BrowserReleaseModelWorker(heavy);
        return E_OUTOFMEMORY;
    }
    job->cancelled = std::make_shared < std::atomic < bool >> (false);
    // A worker's ticket is independent from the old pane's lifetime.
    s->modelCancelled = job->cancelled;
    job->anchor = s->anchor;
    job->instance = s->instanceId;
    job->generation = ++s->modelGeneration;
    job->archive = s->archive;
    job->folder = s->folder;
    job->modified = archiveStamp;
    job->archiveBytes = archiveBytes;
    job->archiveIdentity = identity;
    job->heavyModelSlot = heavy;
    job->metadataMask = metadataMask;
    // The worker cannot outlive Wh_ModUninit: every handle is joined.
    // Register before the thread starts: completion can happen immediately.
    g_browserModelInflight.push_back({job->cancelled,s->archive,s->folder,
                                     archiveStamp,archiveBytes,identity,metadataMask});
    // After a successful CreateThread the worker owns and eventually deletes job.
    HANDLE thread = ArchivePreviewWorkerLifetime::Start(BrowserModelWorker,job);
    if (!thread) {
        const DWORD error = GetLastError();
        BrowserForgetInflight(job->cancelled);
        delete job;
        BrowserReleaseModelWorker(heavy);
        return HRESULT_FROM_WIN32(error);
    }
    s->modelPending = true;
    s->modelPendingFolder = s->folder;
    return S_OK;
}
static void BrowserRestoreView(BrowserState * s,const BrowserSavedView & view) {
    std::unordered_set < std::wstring > selectedLookup;
    const bool manySelected = view.selectedPaths.size() > 8;
    if (manySelected) {
        selectedLookup.reserve(view.selectedPaths.size());
        selectedLookup.insert(view.selectedPaths.begin(),view.selectedPaths.end());
    }
    for (int i = 0;i < static_cast < int > (s->entries.size());++i) {
        const auto & entry = s->entries[static_cast < size_t > (i)];
        const bool selected = manySelected
            ? selectedLookup.find(entry.relative) != selectedLookup.end()
            : std::find(view.selectedPaths.begin(),view.selectedPaths.end(),entry.relative) != view.selectedPaths.end();
        if (selected) ListView_SetItemState(s->list,i,LVIS_SELECTED,LVIS_SELECTED);
    }
    int focus = view.focused,top = view.firstVisible;
    for (size_t i=0;i<s->entries.size();++i) {
        if (!view.focusedPath.empty() && s->entries[i].relative == view.focusedPath) focus = static_cast<int>(i);
        if (!view.topPath.empty() && s->entries[i].relative == view.topPath) top = static_cast<int>(i);
    }
    if (focus >= 0 && focus < ListView_GetItemCount(s->list))
        ListView_SetItemState(s->list,focus,LVIS_FOCUSED,LVIS_FOCUSED);
    if (top >= 0 && top < ListView_GetItemCount(s->list)) {
        const int delta = top - ListView_GetTopIndex(s->list);
        RECT row{};
        if (delta && ListView_GetItemRect(s->list,top,&row,LVIR_BOUNDS) && row.bottom > row.top)
            ListView_Scroll(s->list,0,delta * (row.bottom - row.top));
        else if (delta)
            ListView_EnsureVisible(s->list,top,FALSE);
    }
}
static bool BrowserEnsureList(BrowserState * s,bool restoreView = true);
static bool BrowserListHasRows(const BrowserState * s) {
    if (!s->list || !IsWindow(s->list) || GetParent(s->list) != s->host) return false;
    const LONG_PTR style = GetWindowLongPtrW(s->list,GWL_STYLE);
    if ((style & (LVS_TYPEMASK | LVS_OWNERDATA)) != (LVS_REPORT | LVS_OWNERDATA) ||
        ListView_GetItemCount(s->list) != static_cast<int>(s->entries.size()) ||
        s->visibleColumns.empty()) return false;
    LVCOLUMNW last{};
    last.mask = LVCF_WIDTH;
    return ListView_GetColumn(s->list,static_cast<int>(s->visibleColumns.size())-1,&last) != FALSE;
}
static bool BrowserApplyListRows(BrowserState * s) {
    if (!s->list || !IsWindow(s->list) || GetParent(s->list) != s->host ||
        !ListView_SetItemCountEx(s->list,0,LVSICF_NOSCROLL) ||
        !ListView_SetItemCountEx(s->list,static_cast<int>(s->entries.size()),LVSICF_NOSCROLL) ||
        !BrowserListHasRows(s)) return BrowserEnsureList(s,false);
    return true;
}
static void BrowserSortByColumn(BrowserState * s,BrowserColumnKind kind) {
    if (!s || !s->list || !s->hasRenderedFolder || s->partialModelPreview || s->modelPending || s->dragging) return;
    BrowserSaveView(s);
    const auto view = BrowserCache(s->archive).views[s->renderedFolder];
    const auto previousColumn = s->sortColumn;
    const bool previousAscending = s->sortAscending;
    if (s->sortColumn == kind) s->sortAscending = !s->sortAscending;
    else {s->sortColumn = kind;s->sortAscending = true;}
    s->updating = true;
    SendMessageW(s->list,WM_SETREDRAW,FALSE,0);
    BrowserOrderRows(s->entries,s->sortColumn,s->sortAscending);
    bool ready = BrowserApplyListRows(s);
    if (!ready) {
        s->sortColumn = previousColumn;
        s->sortAscending = previousAscending;
        BrowserOrderRows(s->entries,previousColumn,previousAscending);
        ready = ListView_SetItemCountEx(s->list,static_cast<int>(s->entries.size()),LVSICF_NOSCROLL) &&
            BrowserListHasRows(s);
    }
    if (ready) BrowserRestoreView(s,view);
    s->hoverIndex = s->pressIndex = -1;s->pressSelectionRows.clear();
    s->pressCustomHit = false;s->pressSelectionOversized = false;s->pressWasSelected = false;
    s->updating = false;
    SendMessageW(s->list,WM_SETREDRAW,TRUE,0);InvalidateRect(s->list,nullptr,FALSE);
    BrowserRedrawHeader(s,false);
    if (ready) {
        BrowserNotifySelectionCount(s);
        BrowserSaveView(s);
    } else BrowserScheduleListSettle(s);
}
static HRESULT BrowserRefresh(BrowserState * s, bool forceRebuild = false) {
    if (!s || !s->host) return E_POINTER;
    const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
    const ULONGLONG refreshStarted = diagnostics?GetTickCount64():0;
    BrowserSaveView(s);
    if (s->manualRefreshPending && s->folder != s->manualRefreshFolder) {
        // Navigation supersedes the explicit refresh. Ignore its old status.
        s->manualRefreshPending = false;
        s->manualRefreshFolder.clear();
        if (s->extractStatus == L"Refreshing preview...") {
            s->extractStatus.clear();
            s->extractStatusUntil = 0;
            BrowserRedrawHeader(s,true);
        }
    }
    if(s->modelPending && s->modelPendingFolder != s->folder) {
        ++s->modelGeneration;
        s->modelCancelled.reset();
        s->modelPending = false;
        s->modelPendingFolder.clear();
    }
    if (s->heavyModelDeferred && s->modelPendingFolder != s->folder) {
        KillTimer(s->host,kBrowserHeavyDebounceTimer);
        s->heavyModelDeferred = false;
    }
    auto & cache = BrowserCache(s->archive);
    // A cache hit skips SHGetFileInfo for every row; initialize the borrowed
    // process-wide image list once so cached icon indices still render.
    if (!s->shellIcons) {
        SHFILEINFOW info{};
        // Never ask the UI thread to resolve a real 10 GiB archive file.
        // SHGFI_USEFILEATTRIBUTES returns the same shared system icon list
        // without opening the archive or invoking its property handlers.
        s->shellIcons = reinterpret_cast < HIMAGELIST > (SHGetFileInfoW(
            L"ArchivePreview.zip",FILE_ATTRIBUTE_NORMAL, & info,sizeof(info),
            SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
    }
    std::vector < BrowserEntry > fresh;
    bool fromCache = false;
    cache.folderTouched[s->folder] = GetTickCount64();
    auto hit = cache.folders.find(s->folder);
    if (hit != cache.folders.end()) {
        fromCache = true;
        // A no-op refresh does not need to copy up to 2,000 cached rows.
        if (!forceRebuild && !s->partialModelPreview && s->hasRenderedFolder &&
            s->renderedFolder == s->folder && s->displayedMetadataMask == cache.metadataMask)
            return BrowserEnsureList(s)?S_OK:E_FAIL;
        if (hit->second.size() > 12000) {
            fresh = std::move(hit->second);
            cache.folders.erase(hit);
            cache.folderTouched.erase(s->folder);
                    } else fresh = hit->second;
    } else {
        // Never block the broker UI thread on a cache miss.
        return BrowserScheduleModel(s);
    }
    BrowserOrderRows(fresh,s->sortColumn,s->sortAscending);
    const ULONGLONG modelReady = diagnostics?GetTickCount64():0;
    // Navigating to the same unchanged folder must never clear the selection or
    // scroll position. Skip model replacement when cached metadata is identical,
    // but still validate the retained native control.
    if (!forceRebuild && !s->partialModelPreview && s->hasRenderedFolder &&
        s->renderedFolder == s->folder && s->displayedMetadataMask == cache.metadataMask &&
        BrowserSameEntries(s->entries,fresh))
        return BrowserEnsureList(s)?S_OK:E_FAIL;

    BrowserSavedView view{};
    auto saved = cache.views.find(s->folder);
    if (saved != cache.views.end()) view = saved->second;
    s->updating = true;
    SendMessageW(s->list,WM_SETREDRAW,FALSE,0);
    s->entries.swap(fresh);
    // LVS_OWNERDATA renders visible rows only: replacing 100,000 per-row
    // LVM_INSERTITEM calls with a single item count update avoids UI stalls.
    // Selection state is managed by the control; reset it before restore.
    if (!BrowserApplyListRows(s)) {
        // Keep the previous model/folder coherent if recreating the control
        // also failed. Do not leave child rows labelled as the parent folder.
        s->entries.swap(fresh);
        if (ListView_SetItemCountEx(s->list,static_cast<int>(s->entries.size()),LVSICF_NOSCROLL) &&
            BrowserListHasRows(s)) {
            if (const auto old = cache.views.find(s->renderedFolder); old != cache.views.end())
                BrowserRestoreView(s,old->second);
        }
        s->updating = false;
        SendMessageW(s->list,WM_SETREDRAW,TRUE,0);
        BrowserScheduleListSettle(s);
        Wh_Log(L"[AP list] model update failed expected=%zu",s->entries.size());
        return E_FAIL;
    }
    BrowserRestoreView(s,view);
    s->renderedFolder = s->folder;
    s->hasRenderedFolder = true;
    s->displayedMetadataMask = cache.metadataMask;
    s->partialModelPreview = false; // Only validated cache rows reach this commit.
    s->hoverIndex =  - 1;
    s->pressIndex =  - 1;
    s->pressSelectionRows.clear();
    s->updating = false;
    SendMessageW(s->list,WM_SETREDRAW,TRUE,0);
    // Redraw every cell and its icon, even if a reused model has the same
    // row count. Suppressing erase left stale pixels until mouse hover.
    RedrawWindow(s->list,nullptr,nullptr,
        RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
    BrowserScheduleListSettle(s);
    BrowserNotifyDepth(s);
    BrowserInvalidateFooter(s);
    if (diagnostics)
        Wh_Log(L"[AP list] model_ms=%llu ui_ms=%llu total_ms=%llu rows=%zu cache=%u saved_selection=%zu",
            modelReady - refreshStarted,GetTickCount64() - modelReady,
            GetTickCount64() - refreshStarted,s->entries.size(),fromCache?1u:0u,view.selectedPaths.size());
    return S_OK;
}
static void BrowserBack(BrowserState * s) {
    if (s) BrowserDiagnosticSnapshot(s,L"Back requested");
    if (!s || s->folder.empty()) return;
    const auto slash = s->folder.find_last_of(L'\\');
    const std::wstring previous = s->folder;
    s->folder = slash == std::wstring::npos?L"":s->folder.substr(0,slash);
    if (FAILED(BrowserRefresh(s))) s->folder = previous;
}
static void BrowserOpenSelected(BrowserState * s,int clickedRow =  - 1) {
    if (s) {
        Wh_Log(L"[AP diag/broker] Open requested instance=%llu anchor=%p row=%d pending=%d rows=%zu root_folder=%d",
            static_cast<unsigned long long>(s->instanceId),s->anchor,clickedRow,
            s->modelPending?1:0,s->entries.size(),s->folder.empty()?1:0);
    }
    if (!s || s->modelPending) return;
    int i = clickedRow >= 0 ? clickedRow : ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);
    if (i < 0 || static_cast < size_t > (i) >= s->entries.size()) return;
    const auto & entry = s->entries[static_cast < size_t > (i)];
    if (!entry.folder) {
        Request req{};
        req.command = ARCHIVE_BROKER_OPEN_FILE;
        req.archive = s->archive;
        req.items = {entry.relative};
        if (FAILED(CoCreateGuid( & req.requestId))) return;
        const HRESULT queued = DispatchAsync(req);
        if (SUCCEEDED(queued)) {
            s->pendingExtract = req.requestId;
            s->extractStatus = L"Preparing file to open...";
            s->extractStatusUntil = 0;
        } else {
            wchar_t msg[90]{};
            swprintf_s(msg,L"Open request rejected: 0x%08lX",
                       static_cast < unsigned long > (queued));
            s->extractStatus = msg;
            s->extractStatusUntil = GetTickCount64() + 8000;
        }
        BrowserRedrawHeader(s,true);
        return;
    }
    if (std::count(entry.relative.begin(),entry.relative.end(),L'\\') >= 24) return;
    const std::wstring previous = s->folder;
    s->folder = entry.relative;
    if (FAILED(BrowserRefresh(s))) s->folder = previous;
}
static int BrowserRowAtPoint(HWND list, POINT point) {
    if (!list) return - 1;
    RECT client{};
    if (!GetClientRect(list, & client) || !PtInRect( & client,point)) return - 1;
    LVHITTESTINFO hit{}; hit.pt = point;
    const int native = ListView_HitTest(list, & hit);
    if (native >= 0 && native < ListView_GetItemCount(list)) return native;
    const int total = ListView_GetItemCount(list);
    if (!total) return - 1;
    const int first = std::max(0,ListView_GetTopIndex(list));
    const int visible = std::max(1,ListView_GetCountPerPage(list));
    for (int row = first; row < total && row < first + visible + 2; ++row) {
        RECT bounds{};
        if (ListView_GetItemRect(list,row, & bounds,LVIR_BOUNDS) &&
            point.y >= bounds.top && point.y < bounds.bottom)
            return row;
        if (bounds.top > point.y) break;
    }
    return - 1;
}
// Save indices only when a multi-selection actually needs to survive a native
// click. Full paths are resolved only if the user really starts a drag.
static void BrowserCapturePressSelection(BrowserState * s,int row) {
    if (!s || !s->list) return;
    s->pressIndex = row;
    s->pressWasSelected = row >= 0 &&
        (ListView_GetItemState(s->list,row,LVIS_SELECTED) & LVIS_SELECTED) != 0;
    const UINT count = ListView_GetSelectedCount(s->list);
    s->pressSelectionOversized = s->pressWasSelected && count > ARCHIVE_BROKER_MAX_PATHS;
    s->pressSelectionRows.clear();
    if (!s->pressWasSelected || count <= 1 || s->pressSelectionOversized) return;
    s->pressSelectionRows.reserve(count);
    for (int item = ListView_GetNextItem(s->list, - 1,LVNI_SELECTED);item >= 0;
         item = ListView_GetNextItem(s->list,item,LVNI_SELECTED))
        s->pressSelectionRows.push_back(item);
}
// When a custom-drawn row extends beyond the native hit-test bounds, select
// it explicitly and manage drag threshold without synthesizing mouse events.
static void BrowserSelectFullRowHit(BrowserState * s,int row,WPARAM modifiers) {
    if (!s || !s->list || row < 0) return;
    SetFocus(s->list);
    const bool control = (modifiers & MK_CONTROL) != 0;
    const bool shift = (modifiers & MK_SHIFT) != 0;
    const bool selected = (ListView_GetItemState(s->list,row,LVIS_SELECTED) & LVIS_SELECTED) != 0;
    if (shift) {
        const int focus = ListView_GetNextItem(s->list, - 1,LVNI_FOCUSED);
        if (!control) ListView_SetItemState(s->list, - 1,0,LVIS_SELECTED);
        const int first = focus >= 0 ? std::min(focus,row) : row;
        const int last = focus >= 0 ? std::max(focus,row) : row;
        for (int i = first;i <= last;++i)
            ListView_SetItemState(s->list,i,LVIS_SELECTED,LVIS_SELECTED);
    } else if (control) {
        ListView_SetItemState(s->list,row,selected?0:LVIS_SELECTED,LVIS_SELECTED);
    } else if (!selected || ListView_GetSelectedCount(s->list) <= 1) {
        ListView_SetItemState(s->list, - 1,0,LVIS_SELECTED);
        ListView_SetItemState(s->list,row,LVIS_SELECTED,LVIS_SELECTED);
    }
    ListView_SetItemState(s->list,row,LVIS_FOCUSED,LVIS_FOCUSED);
}
static void BrowserSyncProgress(BrowserState * s);
static std::atomic<unsigned> g_externalDragInterfaces{0};
// Delayed virtual-file drag. Metadata is advertised during drag-over; the
// selected extractor runs only when a target requests file contents. Explorer
// owns the final destination and copies the resulting streams there.
struct ExternalDragEntry {
    std::wstring archivePath;
    std::wstring displayPath;
    bool directory{};
};
using ExternalDragHandle = std::unique_ptr<void,decltype(&CloseHandle)>;
static constexpr wchar_t kExternalDragPrefix[] = L"ArchivePreview-Drag-";
static constexpr wchar_t kExternalDragLeaseName[] = L"\\lease.bin";
static constexpr wchar_t kExternalDragFilesName[] = L"\\files";

struct ExternalDragPathLocks {
    std::vector<HANDLE> retained;
    ~ExternalDragPathLocks() {
        for (HANDLE handle:retained) CloseHandle(handle);
    }
    bool Lock(const fs::path & path) {
        // Reserve before opening handles so allocation failure leaves no untracked handle.
        const auto relative = path.relative_path();
        retained.reserve(static_cast<size_t>(std::distance(relative.begin(),relative.end())));
        return SUCCEEDED(ArchivePreviewPathSafety::CheckPathComponents(path,true,true,&retained,true));
    }
};
static ExternalDragHandle OpenExternalDragEntry(const std::wstring & path,DWORD access,
                                                DWORD share,DWORD creation = OPEN_EXISTING) {
    const std::wstring openPath = ArchivePreviewPathSafety::ExtendedLocalPath(path);
    HANDLE handle = CreateFileW(openPath.c_str(),access,share,nullptr,creation,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
    return ExternalDragHandle(handle == INVALID_HANDLE_VALUE ? nullptr : handle,CloseHandle);
}
static bool ExternalDragIsDirectory(HANDLE handle) {
    FILE_ATTRIBUTE_TAG_INFO info{};
    return GetFileInformationByHandleEx(handle,FileAttributeTagInfo,&info,sizeof(info)) &&
        (info.FileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ==
            FILE_ATTRIBUTE_DIRECTORY;
}
static bool SameExternalDragEntry(HANDLE first,HANDLE second) {
    BY_HANDLE_FILE_INFORMATION a{},b{};
    return GetFileInformationByHandle(first,&a) && GetFileInformationByHandle(second,&b) &&
        (a.nFileIndexHigh || a.nFileIndexLow) && a.dwVolumeSerialNumber == b.dwVolumeSerialNumber &&
        a.nFileIndexHigh == b.nFileIndexHigh && a.nFileIndexLow == b.nFileIndexLow;
}
static HRESULT ExternalDragRoot(std::wstring & path,ExternalDragPathLocks & rootLocks,bool create) {
    std::vector<wchar_t> storage(32768);
    if (!Wh_GetModStoragePath(storage.data(),storage.size())) return E_ACCESSDENIED;
    std::wstring sid;
    const HRESULT sidResult = ArchiveGetUserSid(sid);
    if (FAILED(sidResult)) return sidResult;
    const fs::path base = fs::path(storage.data()).lexically_normal();
    const fs::path parent = base.has_filename() ? base : base.parent_path();
    ExternalDragPathLocks locks;
    if (!locks.Lock(parent)) return E_ACCESSDENIED;
    path = (parent / (std::wstring(L"ArchivePreview-ExternalDrag-")+sid)).wstring();
    if (create) {
        const std::wstring createPath = ArchivePreviewPathSafety::ExtendedLocalPath(path);
        const std::wstring acl = L"D:P(A;OICI;FA;;;SY)(A;OICI;FA;;;" + sid + L")";
        PSECURITY_DESCRIPTOR descriptor = nullptr;
        if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
                acl.c_str(),SDDL_REVISION_1,&descriptor,nullptr))
            return HRESULT_FROM_WIN32(GetLastError());
        SECURITY_ATTRIBUTES attributes{sizeof(attributes),descriptor,FALSE};
        const BOOL created = CreateDirectoryW(createPath.c_str(),&attributes);
        const DWORD error = created ? ERROR_SUCCESS : GetLastError();
        LocalFree(descriptor);
        if (error != ERROR_SUCCESS && error != ERROR_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(error);
    }
    const auto root = OpenExternalDragEntry(path,FILE_READ_ATTRIBUTES | READ_CONTROL,
        FILE_SHARE_READ | FILE_SHARE_WRITE);
    if (!root) return HRESULT_FROM_WIN32(GetLastError());
    if (!ExternalDragIsDirectory(root.get()) || !rootLocks.Lock(fs::path(path)))
        return E_ACCESSDENIED;
    PSID expected = nullptr;
    if (!ConvertStringSidToSidW(sid.c_str(),&expected)) return E_ACCESSDENIED;
    PSID owner = nullptr;
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    const DWORD error = GetSecurityInfo(root.get(),SE_FILE_OBJECT,OWNER_SECURITY_INFORMATION,
        &owner,nullptr,nullptr,nullptr,&descriptor);
    const bool allowed = error == ERROR_SUCCESS && owner && EqualSid(owner,expected);
    LocalFree(descriptor);
    LocalFree(expected);
    return allowed ? S_OK : E_ACCESSDENIED;
}
static bool ExternalDragLeaseIsIdle(HANDLE file) {
    FILE_ATTRIBUTE_TAG_INFO attrs{};
    LARGE_INTEGER start{},size{};
    ExternalDragLeaseRecord record{};
    DWORD read = 0;
    if (!GetFileInformationByHandleEx(file,FileAttributeTagInfo,&attrs,sizeof(attrs)) ||
        (attrs.FileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ||
        !GetFileSizeEx(file,&size) || size.QuadPart != sizeof(record) ||
        !SetFilePointerEx(file,start,nullptr,FILE_BEGIN) ||
        !ReadFile(file,&record,sizeof(record),&read,nullptr) || read != sizeof(record) ||
        record.signature != kExternalDragLeaseSignature)
        return false;

    const bool hasCreationTime = record.created.dwHighDateTime || record.created.dwLowDateTime;
    if (record.status == ExternalDragLeaseStatus::Idle) return !record.pid && !hasCreationTime;
    if (record.status != ExternalDragLeaseStatus::Running || !record.pid || !hasCreationTime)
        return false;
    const ExternalDragHandle process(OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,record.pid),CloseHandle);
    if (!process) return GetLastError() == ERROR_INVALID_PARAMETER;
    if (WaitForSingleObject(process.get(),0) == WAIT_OBJECT_0) return true;
    FILETIME created{},exited{},kernel{},user{};
    return GetProcessTimes(process.get(),&created,&exited,&kernel,&user) &&
        CompareFileTime(&record.created,&created) != 0;
}
static bool DeleteExternalDragEntry(HANDLE file) {
    FILE_DISPOSITION_INFO remove{TRUE};
    if (SetFileInformationByHandle(file,FileDispositionInfo,&remove,sizeof(remove))) return true;
    if (GetLastError() != ERROR_ACCESS_DENIED) return false;
    FILE_DISPOSITION_INFO_EX readOnly{
        FILE_DISPOSITION_FLAG_DELETE | FILE_DISPOSITION_FLAG_IGNORE_READONLY_ATTRIBUTE};
    return SetFileInformationByHandle(file,FileDispositionInfoEx,&readOnly,sizeof(readOnly)) != FALSE;
}
static bool ExternalDragRemoveTree(const std::wstring & path,bool cancellable,unsigned depth = 0) {
    if (depth >= 128 || (cancellable && CancelledForCurrentOperation())) return false;
    // Open the entry itself and keep it pinned during enumeration. Reparse
    // points are removed by handle without visiting their targets.
    const auto file = OpenExternalDragEntry(path,DELETE | FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE);
    if (!file) {
        const DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    FILE_ATTRIBUTE_TAG_INFO info{};
    if (!GetFileInformationByHandleEx(file.get(),FileAttributeTagInfo,&info,sizeof(info)))
        return false;
    if ((info.FileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) ==
            FILE_ATTRIBUTE_DIRECTORY) {
        WIN32_FIND_DATAW entry{};
        const std::wstring searchPath = ArchivePreviewPathSafety::ExtendedLocalPath(path+L"\\*");
        HANDLE found = FindFirstFileW(searchPath.c_str(),&entry);
        if (found == INVALID_HANDLE_VALUE) {
            if (GetLastError() != ERROR_FILE_NOT_FOUND) return false;
        } else {
            const std::unique_ptr<void,decltype(&FindClose)> find(found,FindClose);
            bool removed = true;
            do {
                if (cancellable && CancelledForCurrentOperation()) return false;
                if (wcscmp(entry.cFileName,L".") == 0 || wcscmp(entry.cFileName,L"..") == 0) continue;
                if (!ExternalDragRemoveTree(path+L"\\"+entry.cFileName,cancellable,depth+1))
                    removed = false;
            } while (FindNextFileW(find.get(),&entry));
            if (GetLastError() != ERROR_NO_MORE_FILES || !removed) return false;
        }
    }
    return DeleteExternalDragEntry(file.get());
}
static bool ReclaimExternalDragStage(const std::wstring & path,HANDLE directory,
                                     ExternalDragHandle & lease,bool cancellable) {
    if (!lease || !ExternalDragLeaseIsIdle(lease.get()) ||
        !ExternalDragRemoveTree(path+kExternalDragFilesName,cancellable) ||
        !DeleteExternalDragEntry(lease.get())) return false;
    lease.reset();
    return DeleteExternalDragEntry(directory);
}
static DWORD WINAPI RecoverExternalDragStages(LPVOID) try {
    if (CancelledForCurrentOperation()) return 0;
    std::wstring root;
    ExternalDragPathLocks locks;
    if (FAILED(ExternalDragRoot(root,locks,false))) return 0;
    const std::wstring prefix(kExternalDragPrefix);
    WIN32_FIND_DATAW entry{};
    const std::wstring searchPath = ArchivePreviewPathSafety::ExtendedLocalPath(root+L"\\"+prefix+L"*");
    HANDLE found = FindFirstFileW(searchPath.c_str(),&entry);
    if (found == INVALID_HANDLE_VALUE) return 0;
    const std::unique_ptr<void,decltype(&FindClose)> find(found,FindClose);
    do {
        if (CancelledForCurrentOperation()) break;
        const std::wstring name(entry.cFileName);
        if (name.size() != prefix.size()+38 || !name.starts_with(prefix) ||
            name[prefix.size()] != L'{' || name.back() != L'}') continue;
        GUID id{};
        if (FAILED(CLSIDFromString(name.c_str()+prefix.size(),&id))) continue;
        const std::wstring path = root+L"\\"+name;
        auto directory = OpenExternalDragEntry(path,DELETE | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE);
        if (!directory || !ExternalDragIsDirectory(directory.get())) continue;
        auto lease = OpenExternalDragEntry(path+kExternalDragLeaseName,GENERIC_READ | DELETE,0);
        if (ReclaimExternalDragStage(path,directory.get(),lease,true)) {
            directory.reset();
            Wh_Log(L"[AP external drag] phase=recovery-cleanup result=removed stage=%s",name.c_str());
        }
    } while (FindNextFileW(find.get(),&entry));
    return 0;
} catch (const std::bad_alloc &) {
    Wh_Log(L"[AP external drag] recovery cleanup stopped: out of memory");
    return 1;
}
struct ExternalDragStage : public std::enable_shared_from_this<ExternalDragStage> {
    Request request;
    ArchivePreviewOptions::ExtractionOptions options;
    FILETIME modified{};
    ULONGLONG archiveBytes{};
    ArchiveFileIdentity identity{};
    std::wstring directory;
    ExternalDragHandle directoryHandle{nullptr,CloseHandle};
    ExternalDragHandle lease{nullptr,CloseHandle};
    std::mutex mutex;
    HANDLE ready{CreateEventW(nullptr,TRUE,FALSE,nullptr)};
    bool attempted{};
    HRESULT outcome{E_PENDING};
    bool MatchesArchive() const {
        FILETIME stamp{};ULONGLONG bytes{};ArchiveFileIdentity current{};
        return GetArchiveSignature(request.archive,stamp,bytes,&current) &&
            identity == current && bytes == archiveBytes && CompareFileTime(&stamp,&modified) == 0;
    }
    HRESULT Extract() {
        if (!MatchesArchive()) return HRESULT_FROM_WIN32(ERROR_FILE_INVALID);
        GUID unique{};
        HRESULT hr = CoCreateGuid(&unique);
        if (FAILED(hr)) return hr;
        wchar_t text[48]{};StringFromGUID2(unique,text,_countof(text));
        std::wstring files;
        {
            std::wstring root;
            ExternalDragPathLocks locks;
            hr = ExternalDragRoot(root,locks,true);
            if (FAILED(hr)) return hr;
            directory = root+L"\\"+kExternalDragPrefix+text;
            files = directory+kExternalDragFilesName;
            if (!CreateDirectoryW(ArchivePreviewPathSafety::ExtendedLocalPath(directory).c_str(),nullptr))
                return HRESULT_FROM_WIN32(GetLastError());
            directoryHandle = OpenExternalDragEntry(directory,DELETE | FILE_READ_ATTRIBUTES,
                FILE_SHARE_READ | FILE_SHARE_WRITE);
            if (!directoryHandle || !ExternalDragIsDirectory(directoryHandle.get()))
                return E_ACCESSDENIED;
            lease = OpenExternalDragEntry(directory+kExternalDragLeaseName,
                GENERIC_READ | GENERIC_WRITE | DELETE,0,CREATE_NEW);
            if (!lease) return HRESULT_FROM_WIN32(GetLastError());
            if (!WriteExternalDragLease(lease.get(),ExternalDragLeaseStatus::Idle)) return E_FAIL;
            if (!CreateDirectoryW(ArchivePreviewPathSafety::ExtendedLocalPath(files).c_str(),nullptr))
                return HRESULT_FROM_WIN32(GetLastError());
        }
        Request extraction = request;
        extraction.command = ARCHIVE_BROKER_EXTRACT_SELECTED_HERE;
        extraction.requestId = unique;
        Wh_Log(L"[AP external drag] phase=extract items=%zu stage=ArchivePreview-Drag-%s",
            extraction.items.size(),text);
        hr = ExternalShellExtract(extraction,options,files,lease.get());
        if (FAILED(hr)) return hr;
        return MatchesArchive()?S_OK:HRESULT_FROM_WIN32(ERROR_FILE_INVALID);
    }
    static DWORD WINAPI Worker(LPVOID raw) {
        std::unique_ptr<std::shared_ptr<ExternalDragStage>> holder(
            static_cast<std::shared_ptr<ExternalDragStage>*>(raw));
        const auto stage = *holder;
        const HRESULT initialized = OleInitialize(nullptr);
        try {
            stage->outcome = SUCCEEDED(initialized)?stage->Extract():initialized;
        } catch (const std::bad_alloc &) {
            stage->outcome = E_OUTOFMEMORY;
        }
        if (SUCCEEDED(initialized)) OleUninitialize();
        Wh_Log(L"[AP external drag] preparation result=0x%08lX",static_cast<unsigned long>(stage->outcome));
        SetEvent(stage->ready);
        return 0;
    }
    HRESULT Prepare() {
        if (!ready) return E_OUTOFMEMORY;
        if (CancelledForCurrentOperation()) return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!attempted) {
                attempted = true;
                auto * holder = new(std::nothrow) std::shared_ptr<ExternalDragStage>(shared_from_this());
                if (!holder || !ArchivePreviewWorkerLifetime::Start(Worker,holder)) {
                    delete holder;outcome = E_OUTOFMEMORY;SetEvent(ready);
                }
            }
        }
        // Concurrent/reentrant content requests wait for the SAME extraction.
        // The extractor runs on a separate STA, so dispatching an incoming OLE
        // request cannot deadlock an extraction suspended on this UI thread.
        const HANDLE events[] = {g_brokerStopEvent,ready};
        while (!CancelledForCurrentOperation()) {
            const DWORD wake = MsgWaitForMultipleObjectsEx(_countof(events),events,INFINITE,
                QS_ALLINPUT,MWMO_INPUTAVAILABLE);
            if (wake == WAIT_OBJECT_0) break;
            if (wake == WAIT_OBJECT_0 + 1) return outcome;
            if (wake == WAIT_FAILED) return HRESULT_FROM_WIN32(GetLastError());
            if (wake == WAIT_OBJECT_0 + _countof(events)) {
                MSG pending{};
                while (!CancelledForCurrentOperation() &&
                       PeekMessageW(&pending,nullptr,0,0,PM_REMOVE)) {
                    if (pending.message == WM_QUIT) {
                        PostQuitMessage(static_cast<int>(pending.wParam));
                        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
                    }
                    TranslateMessage(&pending);DispatchMessageW(&pending);
                }
            }
        }
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }
    ~ExternalDragStage() {
        if (ready) CloseHandle(ready);
        if (!directoryHandle) return;
        bool removed = false;
        try {
            ExternalDragPathLocks locks;
            if (locks.Lock(fs::path(directory).parent_path()) &&
                ExternalDragIsDirectory(directoryHandle.get())) {
                // The path may have changed when no ancestor handles were held.
                const auto current = OpenExternalDragEntry(directory,FILE_READ_ATTRIBUTES,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
                if (current && SameExternalDragEntry(directoryHandle.get(),current.get())) {
                    removed = lease ? ReclaimExternalDragStage(directory,directoryHandle.get(),lease,false) :
                        DeleteExternalDragEntry(directoryHandle.get());
                }
            }
        } catch (const std::bad_alloc &) {
            // The next broker session can retry using the persistent record.
        }
        directoryHandle.reset();
        if (ArchivePreviewDiagnostics::Enabled()) {
            const wchar_t * stageName = wcsrchr(directory.c_str(),L'\\');
            Wh_Log(L"[AP external drag] phase=cleanup result=%s stage=%s",
                removed?L"removed":L"retained",
                stageName?stageName+1:directory.c_str());
        }
    }
};
// Retain the stage until every stream/clone has been released by the target.
// The IDataObject can be released before a target finishes reading its stream.
class ExternalDragStream final : public IStream {
    std::atomic<ULONG> refs_{1};
    IStream * stream_{};
    std::shared_ptr<ExternalDragStage> stage_;
    const LONG index_;
public:
    ExternalDragStream(IStream * stream,std::shared_ptr<ExternalDragStage> stage,LONG index):
        stream_(stream),stage_(std::move(stage)),index_(index) {
        ++ArchivePreviewHost::g_objects;
        ++g_externalDragInterfaces;
    }
    ~ExternalDragStream(){stream_->Release();stage_.reset();--ArchivePreviewHost::g_objects;--g_externalDragInterfaces;}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void ** out) override {
        if (!out) return E_POINTER;*out = nullptr;
        if (iid != IID_IUnknown && iid != IID_ISequentialStream && iid != IID_IStream) return E_NOINTERFACE;
        *out = static_cast<IStream*>(this);AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {return refs_.fetch_add(1)+1;}
    ULONG STDMETHODCALLTYPE Release() override {const ULONG n = refs_.fetch_sub(1)-1;if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE Read(void * p,ULONG n,ULONG * read) override {
        const HRESULT hr = stream_->Read(p,n,read);
        if (FAILED(hr))
            Wh_Log(L"[AP external drag] phase=stream-read index=%ld hr=0x%08lX",
                index_,static_cast<unsigned long>(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE Write(const void *,ULONG,ULONG * written) override {if(written)*written=0;return STG_E_ACCESSDENIED;}
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER move,DWORD origin,ULARGE_INTEGER * position) override {return stream_->Seek(move,origin,position);}
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER) override {return STG_E_ACCESSDENIED;}
    HRESULT STDMETHODCALLTYPE CopyTo(IStream * target,ULARGE_INTEGER n,
                                     ULARGE_INTEGER * read,ULARGE_INTEGER * written) override {
        const HRESULT hr = stream_->CopyTo(target,n,read,written);
        if (FAILED(hr))
            Wh_Log(L"[AP external drag] phase=stream-copy index=%ld hr=0x%08lX",
                index_,static_cast<unsigned long>(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE Commit(DWORD flags) override {return stream_->Commit(flags);}
    HRESULT STDMETHODCALLTYPE Revert() override {return stream_->Revert();}
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER at,ULARGE_INTEGER n,DWORD type) override {return stream_->LockRegion(at,n,type);}
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER at,ULARGE_INTEGER n,DWORD type) override {return stream_->UnlockRegion(at,n,type);}
    HRESULT STDMETHODCALLTYPE Stat(STATSTG * stat,DWORD flags) override {return stream_->Stat(stat,flags);}
    HRESULT STDMETHODCALLTYPE Clone(IStream ** out) override {
        if (!out) return E_POINTER;*out = nullptr;
        IStream * copy = nullptr;HRESULT hr = stream_->Clone(&copy);
        if (FAILED(hr)) return hr;
        auto * wrapper = new(std::nothrow) ExternalDragStream(copy,stage_,index_);
        if (!wrapper) {copy->Release();return E_OUTOFMEMORY;}
        *out = wrapper;return S_OK;
    }
};
class ExternalDragData final : public IDataObject,public IDataObjectAsyncCapability {
    std::atomic<ULONG> refs_{1};
    std::shared_ptr<ExternalDragStage> stage_;
    std::vector<ExternalDragEntry> entries_;
    std::unordered_set<std::wstring> names_;
    struct Extra {CLIPFORMAT format{};LONG index{};HGLOBAL memory{};};
    std::vector<Extra> extras_;
    bool async_{true};
    bool inOperation_{};
    const CLIPFORMAT descriptors_ = static_cast<CLIPFORMAT>(RegisterClipboardFormatW(L"FileGroupDescriptorW"));
    const CLIPFORMAT contents_ = static_cast<CLIPFORMAT>(RegisterClipboardFormatW(L"FileContents"));
    const CLIPFORMAT preferred_ = static_cast<CLIPFORMAT>(RegisterClipboardFormatW(L"Preferred DropEffect"));
    static constexpr size_t kMaxEntries = 32768;
    HRESULT AddItem(IShellItem * item,const std::wstring & archivePath,const std::wstring & displayPath,unsigned depth) {
        if (CancelledForCurrentOperation()) return HRESULT_FROM_WIN32(ERROR_CANCELLED);
        if (depth > 64 || !SafeRelativePath(archivePath) || !SafeRelativePath(displayPath)) return E_INVALIDARG;
        if (displayPath.size() >= MAX_PATH) return HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
        if (entries_.size() >= kMaxEntries) return HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        if (!names_.insert(ShellLookupKey(displayPath)).second) return HRESULT_FROM_WIN32(ERROR_DUP_NAME);
        SFGAOF attrs{};
        HRESULT hr = item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM,&attrs);
        if (FAILED(hr)) return hr;
        const bool directory = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
        entries_.push_back({archivePath,displayPath,directory});
        if (!directory) return S_OK;
        IEnumShellItems * enumerator = nullptr;
        hr = item->BindToHandler(nullptr,BHID_EnumItems,IID_PPV_ARGS(&enumerator));
        if (FAILED(hr)) return hr;
        for (;;) {
            IShellItem * child = nullptr;ULONG fetched = 0;
            hr = enumerator->Next(1,&child,&fetched);
            if (hr == S_FALSE) {if(child)child->Release();hr = S_OK;break;}
            if (hr != S_OK || !child) {if(child)child->Release();if(SUCCEEDED(hr))hr=E_FAIL;break;}
            PWSTR raw = nullptr;std::wstring name;
            IShellItem2 * details = nullptr;
            if (SUCCEEDED(child->QueryInterface(IID_PPV_ARGS(&details)))) {
                if (SUCCEEDED(details->GetString(PKEY_FileName,&raw)) && raw) name = raw;
                CoTaskMemFree(raw);raw = nullptr;details->Release();
            }
            if (name.empty() && SUCCEEDED(child->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,&raw)) && raw)
                name = raw;
            CoTaskMemFree(raw);
            if (!SafeArchiveLeaf(name)) hr = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
            else hr = AddItem(child,archivePath+L"\\"+name,displayPath+L"\\"+name,depth+1);
            child->Release();
            if (FAILED(hr)) break;
        }
        enumerator->Release();return hr;
    }
    static HRESULT CopyMemoryMedium(HGLOBAL source,STGMEDIUM * out) {
        const SIZE_T bytes = GlobalSize(source);
        if (!bytes || bytes > 32ULL*1024*1024) return DV_E_TYMED;
        HGLOBAL copy = GlobalAlloc(GMEM_MOVEABLE,bytes);
        if (!copy) return E_OUTOFMEMORY;
        void * from = GlobalLock(source);void * to = GlobalLock(copy);
        if (!from || !to) {
            if(from)GlobalUnlock(source);if(to)GlobalUnlock(copy);GlobalFree(copy);return E_OUTOFMEMORY;
        }
        memcpy(to,from,bytes);GlobalUnlock(copy);GlobalUnlock(source);
        out->tymed = TYMED_HGLOBAL;out->hGlobal = copy;out->pUnkForRelease = nullptr;return S_OK;
    }
public:
    ExternalDragData(const Request & request,const ArchivePreviewOptions::ExtractionOptions & options):
        stage_(std::make_shared<ExternalDragStage>()) {
        stage_->request = request;stage_->options = options;++ArchivePreviewHost::g_objects;++g_externalDragInterfaces;
    }
    ~ExternalDragData(){
        for(const auto & extra:extras_)GlobalFree(extra.memory);
        stage_.reset();--ArchivePreviewHost::g_objects;--g_externalDragInterfaces;
    }
    HRESULT Initialize() {
        if (!descriptors_ || !contents_ || !preferred_ || stage_->request.items.empty()) return E_INVALIDARG;
        if (!GetArchiveSignature(stage_->request.archive,stage_->modified,stage_->archiveBytes,&stage_->identity))
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        // Items from one preview list share a parent. Strip that parent from
        // drop names while preserving the selection's own directory trees.
        const std::wstring parent = ParentFolder(stage_->request.items.front());
        const size_t prefix = parent.empty()?0:parent.size()+1;
        for(const auto & name:stage_->request.items)
            if (ParentFolder(name) != parent) return E_INVALIDARG;
        IShellItem * archive = nullptr;
        HRESULT hr = OpenArchive(stage_->request.archive,&archive);
        if (FAILED(hr)) return hr;
        std::vector<IShellItem*> selected;
        hr = ResolveSelectedItems(archive,stage_->request.items,selected);
        if (SUCCEEDED(hr)) {
            for(size_t i=0;i<selected.size();++i) {
                hr = AddItem(selected[i],stage_->request.items[i],stage_->request.items[i].substr(prefix),0);
                if (FAILED(hr)) break;
            }
        }
        ReleaseItems(selected);archive->Release();
        if (SUCCEEDED(hr) && !stage_->MatchesArchive()) hr = HRESULT_FROM_WIN32(ERROR_FILE_INVALID);
        Wh_Log(L"[AP external drag] descriptors=%zu result=0x%08lX",entries_.size(),static_cast<unsigned long>(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void ** out) override {
        if(!out)return E_POINTER;*out=nullptr;
        if(iid == IID_IUnknown || iid == IID_IDataObject)*out=static_cast<IDataObject*>(this);
        else if(iid == IID_IDataObjectAsyncCapability)*out=static_cast<IDataObjectAsyncCapability*>(this);
        else return E_NOINTERFACE;
        AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {return refs_.fetch_add(1)+1;}
    ULONG STDMETHODCALLTYPE Release() override {const ULONG n=refs_.fetch_sub(1)-1;if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC * format) override {
        if(!format)return E_POINTER;
        if(format->dwAspect != DVASPECT_CONTENT)return DV_E_DVASPECT;
        if(format->cfFormat == contents_) {
            if(!(format->tymed & TYMED_ISTREAM))return DV_E_TYMED;
            return format->lindex >= -1 && (format->lindex == -1 || static_cast<size_t>(format->lindex)<entries_.size())?S_OK:DV_E_LINDEX;
        }
        if(!(format->tymed & TYMED_HGLOBAL))return DV_E_TYMED;
        if(format->cfFormat == descriptors_ || format->cfFormat == preferred_)return S_OK;
        for(const auto & extra:extras_)
            if(extra.format == format->cfFormat && extra.index == format->lindex)return S_OK;
        return DV_E_FORMATETC;
    }
    HRESULT STDMETHODCALLTYPE GetData(FORMATETC * format,STGMEDIUM * out) override {
        if(!out)return E_POINTER;*out={};
        HRESULT hr=QueryGetData(format);if(FAILED(hr))return hr;
        if(format->cfFormat == descriptors_) {
            const size_t bytes=FIELD_OFFSET(FILEGROUPDESCRIPTORW,fgd)+entries_.size()*sizeof(FILEDESCRIPTORW);
            HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,bytes);
            if(!memory)return E_OUTOFMEMORY;
            auto * group=static_cast<FILEGROUPDESCRIPTORW*>(GlobalLock(memory));
            if(!group){GlobalFree(memory);return E_OUTOFMEMORY;}
            group->cItems=static_cast<UINT>(entries_.size());
            for(size_t i=0;i<entries_.size();++i) {
                auto & descriptor=group->fgd[i];
                descriptor.dwFlags=FD_ATTRIBUTES | FD_UNICODE;
                descriptor.dwFileAttributes=entries_[i].directory?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_NORMAL;
                wcscpy_s(descriptor.cFileName,entries_[i].displayPath.c_str());
            }
            GlobalUnlock(memory);out->tymed=TYMED_HGLOBAL;out->hGlobal=memory;return S_OK;
        }
        if(format->cfFormat == preferred_) {
            HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,sizeof(DWORD));
            if(!memory)return E_OUTOFMEMORY;
            auto * value=static_cast<DWORD*>(GlobalLock(memory));
            if(!value){GlobalFree(memory);return E_OUTOFMEMORY;}
            *value=DROPEFFECT_COPY;GlobalUnlock(memory);
            out->tymed=TYMED_HGLOBAL;out->hGlobal=memory;return S_OK;
        }
        if(format->cfFormat == contents_) {
            if(format->lindex<0)return DV_E_LINDEX;
            const auto & entry=entries_[static_cast<size_t>(format->lindex)];
            if(entry.directory) {
                out->tymed=TYMED_ISTREAM;
                hr=CreateStreamOnHGlobal(nullptr,TRUE,&out->pstm);
                if(FAILED(hr))*out={};return hr;
            }
            hr=stage_->Prepare();if(FAILED(hr))return hr;
            const std::wstring path=stage_->directory+kExternalDragFilesName+L"\\"+entry.archivePath;
            hr = ArchivePreviewPathSafety::CheckPathComponents(fs::path(path),false,false,nullptr,true);
            if (FAILED(hr)) {
                Wh_Log(L"[AP external drag] phase=stream-validate index=%ld path_chars=%zu hr=0x%08lX",
                    format->lindex,path.size(),static_cast<unsigned long>(hr));
                return hr;
            }
            const std::wstring streamPath = path.size() >= MAX_PATH ?
                ArchivePreviewPathSafety::ExtendedLocalPath(path) : path;
            IStream * file=nullptr;
            hr=SHCreateStreamOnFileEx(streamPath.c_str(),STGM_READ | STGM_SHARE_DENY_WRITE,
                FILE_ATTRIBUTE_NORMAL,FALSE,nullptr,&file);
            if (FAILED(hr)) {
                Wh_Log(L"[AP external drag] phase=stream-open index=%ld path_chars=%zu hr=0x%08lX",
                    format->lindex,path.size(),static_cast<unsigned long>(hr));
                return hr;
            }
            auto * wrapper=new(std::nothrow)ExternalDragStream(file,stage_,format->lindex);
            if(!wrapper){file->Release();return E_OUTOFMEMORY;}
            out->tymed=TYMED_ISTREAM;out->pstm=wrapper;return S_OK;
        }
        for(const auto & extra:extras_)
            if(extra.format == format->cfFormat && extra.index == format->lindex)return CopyMemoryMedium(extra.memory,out);
        return DV_E_FORMATETC;
    }
    HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *,STGMEDIUM *) override {return DV_E_FORMATETC;}
    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC *,FORMATETC * out) override {if(out)out->ptd=nullptr;return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetData(FORMATETC * format,STGMEDIUM * medium,BOOL release) override {
        if(!format || !medium)return E_POINTER;
        if(format->dwAspect != DVASPECT_CONTENT)return DV_E_DVASPECT;
        if(medium->tymed != TYMED_HGLOBAL || !(format->tymed & TYMED_HGLOBAL))return DV_E_TYMED;
        if(format->cfFormat == descriptors_ || format->cfFormat == contents_ || format->cfFormat == preferred_)return E_ACCESSDENIED;
        STGMEDIUM copy{};HRESULT hr=CopyMemoryMedium(medium->hGlobal,&copy);
        if(FAILED(hr))return hr;
        Extra * target=nullptr;
        for(auto & extra:extras_)if(extra.format==format->cfFormat && extra.index==format->lindex){target=&extra;break;}
        if(!target) {
            if(extras_.size()>=32){GlobalFree(copy.hGlobal);return E_OUTOFMEMORY;}
            extras_.push_back({format->cfFormat,format->lindex,nullptr});target=&extras_.back();
        }
        if(target->memory)GlobalFree(target->memory);target->memory=copy.hGlobal;
        if(release)ReleaseStgMedium(medium);return S_OK;
    }
    HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD direction,IEnumFORMATETC ** out) override {
        if(!out)return E_POINTER;*out=nullptr;if(direction!=DATADIR_GET)return E_NOTIMPL;
        const FORMATETC formats[]={
            {descriptors_,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL},
            {contents_,nullptr,DVASPECT_CONTENT,-1,TYMED_ISTREAM},
            {preferred_,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL}};
        return SHCreateStdEnumFmtEtc(_countof(formats),formats,out);
    }
    HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC *,DWORD,IAdviseSink *,DWORD *) override {return OLE_E_ADVISENOTSUPPORTED;}
    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD) override {return OLE_E_ADVISENOTSUPPORTED;}
    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA ** out) override {if(out)*out=nullptr;return OLE_E_ADVISENOTSUPPORTED;}
    HRESULT STDMETHODCALLTYPE SetAsyncMode(BOOL mode) override {async_=mode!=FALSE;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetAsyncMode(BOOL * out) override {if(!out)return E_POINTER;*out=async_?TRUE:FALSE;return S_OK;}
    HRESULT STDMETHODCALLTYPE StartOperation(IBindCtx *) override {inOperation_=true;return S_OK;}
    HRESULT STDMETHODCALLTYPE InOperation(BOOL * out) override {if(!out)return E_POINTER;*out=inOperation_?TRUE:FALSE;return S_OK;}
    HRESULT STDMETHODCALLTYPE EndOperation(HRESULT result,IBindCtx *,DWORD effect) override {
        Wh_Log(L"[AP external drag] target completed hr=0x%08lX effect=%lu",static_cast<unsigned long>(result),effect);
        inOperation_=false;return S_OK;
    }
};
static HRESULT CreateExternalDragData(const Request & request,
    const ArchivePreviewOptions::ExtractionOptions & options,IDataObject ** out) {
    if(!out)return E_POINTER;*out=nullptr;
    auto * data=new(std::nothrow)ExternalDragData(request,options);
    if(!data)return E_OUTOFMEMORY;
    const HRESULT hr=data->Initialize();
    if(FAILED(hr)){data->Release();return hr;}
    *out=static_cast<IDataObject*>(data);return S_OK;
}
// Native mode and clipboard retain the original Shell IDataObject.
// External drag uses delayed virtual-file streams backed by a private stage.
static bool BrowserRejectCollapsedOversizeDrag(const BrowserState * s,int draggedIndex) {
    return s && draggedIndex == s->pressIndex && s->pressSelectionOversized &&
           s->pressWasSelected;
}
static void BrowserTransfer(BrowserState * s,bool drag,int draggedIndex =  - 1) {
    if (!s || s->dragging || s->modelPending) return;
    std::shared_ptr < BrowserState > lifetime;
    const auto owner = g_browsers.find(s->anchor);
    if (owner != g_browsers.end() && owner->second.get() == s) lifetime = owner->second;
    auto selection = BrowserSelection(s);
    // The native ListView may collapse a large selection on mouse-down before
    // LVN_BEGINDRAG. If the pressed row was part of an oversized group, never
    // misinterpret that collapse as an intentional single-file drag.
    if (drag && BrowserRejectCollapsedOversizeDrag(s,draggedIndex)) {
        BrowserWarnSelectionLimit(s);
        return;
    }
    if (drag && draggedIndex >= 0 && static_cast < size_t > (draggedIndex) < s->entries.size()) {
        const auto & dragged = s->entries[static_cast < size_t > (draggedIndex)].relative;
        // When dragging one row in an already multi-selected group, preserve
        // the selection snapshot captured before ListView processed the click.
        if (s->pressIndex == draggedIndex && s->pressSelectionRows.size() > 1 &&
            std::find(s->pressSelectionRows.begin(),s->pressSelectionRows.end(),draggedIndex) != s->pressSelectionRows.end()) {
            selection.clear();
            selection.reserve(s->pressSelectionRows.size());
            for (int row:s->pressSelectionRows)
                if (row >= 0 && static_cast < size_t > (row) < s->entries.size())
                    selection.push_back(s->entries[static_cast < size_t > (row)].relative);
        } else if (std::find(selection.begin(),selection.end(),dragged) == selection.end())
            selection = {dragged};
    }
    if (selection.empty()) return;
    if (selection.size() > ARCHIVE_BROKER_MAX_PATHS) {
        BrowserWarnSelectionLimit(s);
        return;  // never start a partial selection
    }
    Request req{};
    req.archive = s->archive;
    req.items = selection;
    const auto extractionOptions = ArchivePreviewOptions::ExtractionSnapshot();
    const bool externalDrag = drag && extractionOptions.backend != 0 && extractionOptions.externalDrag;
    // Capture settings for the complete gesture; later changes must not switch
    // the extractor halfway through serving this data object.
    s->dragging = true;
    // A Shell context-menu Copy verb can publish more clipboard formats than
    // the IDataObject supplied for drag. Avoid enumerating the selection twice
    // when the provider implements that verb.
    HRESULT nativeHr = drag ? E_NOTIMPL : NativeShellContextCopy(req,s->host);
    if (drag || FAILED(nativeHr)) {
        IDataObject * nativeData = nullptr;
        nativeHr = externalDrag?CreateExternalDragData(req,extractionOptions,&nativeData):
            NativeShellSelection(req,s->host, & nativeData);
        if (SUCCEEDED(nativeHr) && nativeData) {
            if (!drag) {
                NativeShellMarkClipboardCopy(nativeData);
                nativeHr = OleSetClipboard(nativeData);
            } else {
                if (GetCapture() == s->list) ReleaseCapture();
                auto * nativeSource = new(std::nothrow) MediumDragSource();
                if (nativeSource) {
                    DWORD effect = DROPEFFECT_NONE;
                    nativeHr = ::DoDragDrop(nativeData,nativeSource,DROPEFFECT_COPY, & effect);
                    nativeSource->Release();
                } else nativeHr = E_OUTOFMEMORY;
            }
            nativeData->Release();
        }
    }
    if (FAILED(nativeHr)) {
        wchar_t status[176]{};
        swprintf_s(status,externalDrag?L"External drag failed: 0x%08lX. Check Windhawk logs.":
            L"Native Shell operation unavailable: 0x%08lX",
            static_cast < unsigned long > (nativeHr));
        s->extractStatus = status;
        s->extractStatusUntil = GetTickCount64() + 8000;
        SetTimer(s->host,kBrowserExtractStatusTimer,8000,nullptr);
    } else {
        s->extractStatus = externalDrag?
            (nativeHr == DRAGDROP_S_CANCEL?L"Drag cancelled.":L"External drag handed to destination."):
            (drag?L"Drag sent to Windows Shell.":L"Copied using Windows Shell.");
        s->extractStatusUntil = GetTickCount64() + 3500;
        SetTimer(s->host,kBrowserExtractStatusTimer,3500,nullptr);
    }
    s->dragging = false;
    if (s->retireWhenIdle) {
        HWND manager = g_gripManager.load();
        if (manager) PostMessageW(manager,WM_BROWSER_REAP,
            reinterpret_cast < WPARAM > (s->anchor),1);
    } else BrowserInvalidateFooter(s);
    return;
}

static LRESULT CALLBACK BrowserListSubclass(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp,
                                             UINT_PTR, DWORD_PTR ref) {
    BrowserState * s = reinterpret_cast < BrowserState *  > (ref);
    if (!s) return DefSubclassProc(hwnd,msg,wp,lp);
    if (ArchivePreviewOptions::hideHorizontalScrollbar.load() &&
        (msg == WM_HSCROLL || msg == WM_MOUSEHWHEEL)) return 0;
    if (msg == WM_SIZE || msg == LVM_SETITEMCOUNT) {
        const LRESULT result = DefSubclassProc(hwnd,msg,wp,lp);
        BrowserLayoutColumns(s);
        InvalidateRect(hwnd,nullptr,FALSE);
        return result;
    }
    if (msg == WM_HSCROLL || msg == WM_MOUSEHWHEEL || msg == LVM_SCROLL) {
        const int offset = ArchivePreviewColumns::HorizontalOffset(hwnd);
        const LRESULT result = DefSubclassProc(hwnd,msg,wp,lp);
        if (offset != ArchivePreviewColumns::HorizontalOffset(hwnd)) {
            BrowserRedrawHeader(s);
            InvalidateRect(hwnd,nullptr,FALSE);
        }
        return result;
    }

    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDOWN)
        Wh_Log(L"[AP input] click list=%p msg=%u x=%d y=%d pending=%d updating=%d",
            hwnd,msg,GET_X_LPARAM(lp),GET_Y_LPARAM(lp),
            s->modelPending?1:0,s->updating?1:0);
    if (msg == WM_ERASEBKGND) {
        HDC dc = reinterpret_cast < HDC > (wp);
        if (dc) {
            RECT r{};GetClientRect(hwnd, & r);
            HBRUSH background = s->paint.background ? s->paint.background :
                CreateSolidBrush(BrowserBackground(s));
            if (background) FillRect(dc, & r,background);
            if (background && background != s->paint.background) DeleteObject(background);
        }
        return 1;
    }
    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) {
        POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        LVHITTESTINFO hit{};hit.pt = pt;
        const int native = ListView_HitTest(hwnd, & hit);
        const int row = BrowserRowAtPoint(hwnd,pt);
        if (row < 0) {
            // Genuine empty space retains native marquee selection.
            s->pressIndex =  - 1;
            s->pressCustomHit = false;
            s->pressSelectionRows.clear();
            s->pressSelectionOversized = false;
            s->pressWasSelected = false;
            return DefSubclassProc(hwnd,msg,wp,lp);
        }
        if (msg == WM_LBUTTONDBLCLK && native < 0) {
            BrowserSelectFullRowHit(s,row,wp);
            BrowserOpenSelected(s);
            return 0;
        }
        if (msg == WM_LBUTTONDOWN) {
            BrowserCapturePressSelection(s,row);
            s->pressPoint = pt;
            s->pressCustomHit = (native < 0);
            if (s->pressCustomHit) {
                BrowserSelectFullRowHit(s,row,wp);
                SetCapture(hwnd);
                return 0;
            }
        }
    }
    if (msg == WM_RBUTTONDOWN) {
        const POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        const int row = BrowserRowAtPoint(hwnd,point);
        if (row >= 0) {
            SetFocus(hwnd);
            if (!(ListView_GetItemState(hwnd,row,LVIS_SELECTED) & LVIS_SELECTED)) {
                ListView_SetItemState(hwnd, - 1,0,LVIS_SELECTED);
                ListView_SetItemState(hwnd,row,LVIS_SELECTED | LVIS_FOCUSED,
                    LVIS_SELECTED | LVIS_FOCUSED);
            }
        }
        return 0; // Native list must not collapse a multi-selection.
    }
    if (msg == WM_RBUTTONUP) {
        POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        ClientToScreen(hwnd, & pt);
        BrowserShowSelectionMenu(s,pt);
        return 0;
    }
    if (msg == WM_CONTEXTMENU) {
        const POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        if (pt.x ==  - 1 && pt.y ==  - 1) BrowserShowSelectionMenu(s,pt);
        return 0;
    }
    if (msg == WM_MOUSEMOVE) {
        POINT pt{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        if (s->pressCustomHit && (wp & MK_LBUTTON)) {
            const int dx = std::abs(pt.x - s->pressPoint.x);
            const int dy = std::abs(pt.y - s->pressPoint.y);
            if (dx >= GetSystemMetrics(SM_CXDRAG) || dy >= GetSystemMetrics(SM_CYDRAG)) {
                const int row = s->pressIndex;
                s->pressCustomHit = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                BrowserTransfer(s,true,row);
                return 0;
            }
            return 0;
        }
        // Avoid repainting the whole ListView while its native marquee
        // owns mouse capture. This preserves the system alpha-blended rectangle.
        if ((wp & MK_LBUTTON) && s->pressIndex < 0)
            return DefSubclassProc(hwnd,msg,wp,lp);
        LVHITTESTINFO hit{};hit.pt = pt;
        const int current = ListView_HitTest(hwnd, & hit);
        if (current != s->hoverIndex) {
            const int previous = s->hoverIndex;
            s->hoverIndex = current;
            if (previous >= 0 && previous < ListView_GetItemCount(hwnd))
                ListView_RedrawItems(hwnd,previous,previous);
            if (current >= 0 && current < ListView_GetItemCount(hwnd))
                ListView_RedrawItems(hwnd,current,current);
        }
        if (!s->trackingMouse) {
            TRACKMOUSEEVENT track{};
            track.cbSize = sizeof(track);track.dwFlags = TME_LEAVE;track.hwndTrack = hwnd;
            s->trackingMouse = TrackMouseEvent( & track) != FALSE;
        }
    }
    if (msg == WM_LBUTTONUP) {
        if (s->pressCustomHit) {
            s->pressCustomHit = false;
            if (GetCapture() == hwnd) ReleaseCapture();
            s->pressIndex =  - 1;
            s->pressSelectionRows.clear();
            return 0;
        }
        s->pressIndex =  - 1;
    }
    if (msg == WM_CAPTURECHANGED && s->pressCustomHit) {
        s->pressCustomHit = false;
        s->pressIndex =  - 1;
        s->pressSelectionRows.clear();
    }
    if (msg == WM_MOUSELEAVE) {
        const int previous = s->hoverIndex;
        s->hoverIndex =  - 1;
        s->trackingMouse = false;
        if (previous >= 0 && previous < ListView_GetItemCount(hwnd))
            ListView_RedrawItems(hwnd,previous,previous);
    }
    if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
        if ((GetKeyState(VK_CONTROL) & 0x8000) && wp == 'A') {
            ListView_SetItemState(hwnd, - 1,LVIS_SELECTED,LVIS_SELECTED);
            return 0;
        }
        if ((GetKeyState(VK_CONTROL) & 0x8000) && (wp == 'C')) {
            BrowserTransfer(s,false);return 0;
        }
        using Action = ArchivePreviewOptions::ExtractAction;
        const auto action = ArchivePreviewOptions::ExtractShortcutForKey(wp);
        if (action != Action::None) {
            if (!ArchivePreviewOptions::IsRepeatedKey(lp) && !s->parked && !s->retireWhenIdle) {
                if (action == Action::SelectedHere || action == Action::SelectedTo)
                    BrowserExtractSelected(s,action == Action::SelectedHere);
                else if (!s->dragging && s->parent && IsWindow(s->parent))
                    PostMessageW(s->parent,ARCHIVE_BROWSER_EXTRACT_ARCHIVE,static_cast<WPARAM>(action),
                        reinterpret_cast<LPARAM>(s->anchor));
            }
            return 0;
        }
        if (wp == VK_BACK) {BrowserBack(s);return 0;}
        if (wp == VK_RETURN) {BrowserOpenSelected(s);return 0;}
    }
    return DefSubclassProc(hwnd,msg,wp,lp);
}
// Both BeginPaint and synchronous WM_PRINTCLIENT must render the same host
// background/status. Child HWNDs still paint independently in their own proc.
static void BrowserPaintHostContent(BrowserState * s,HWND hwnd,HDC dc) {
    if (!dc) return;
    RECT area{};
    GetClientRect(hwnd, & area);
    const COLORREF background = BrowserBackground(s);
    HBRUSH brush = s && s->paint.background?s->paint.background:CreateSolidBrush(background);
    if (brush) {
        FillRect(dc, & area,brush);
        if (!s || brush != s->paint.background) DeleteObject(brush);
    }
    if (s) {
        BrowserPaintProgress(s,dc,area);
        if (!s->footer || !IsWindow(s->footer)) BrowserPaintFooter(s,dc,area);
    }
}
static LRESULT CALLBACK BrowserHostProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto * cs = reinterpret_cast < CREATESTRUCTW *  > (lp);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast < LONG_PTR > (cs->lpCreateParams));
    }
    auto * s = reinterpret_cast < BrowserState *  > (GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    switch(msg) {
      case WM_SIZE:
        if (s && s->list) {
            RECT rc{};GetClientRect(hwnd, & rc);
            const int w = std::max(1,static_cast < int > (rc.right));
            const int header = BrowserHeaderHeight(s);
            if (s->header && IsWindow(s->header)) {
                RECT old{};
                if (GetWindowRect(s->header, & old))
                    MapWindowPoints(nullptr,hwnd,reinterpret_cast < POINT *  > ( & old),2);
                if (old.left != 0 || old.top != 0 ||
                    old.right - old.left != w || old.bottom - old.top != header)
                    SetWindowPos(s->header,nullptr,0,0,w,header,
                        SWP_NOZORDER | SWP_NOACTIVATE);
            }
            BrowserLayoutProgress(s);
            BrowserLayoutColumns(s);
            InvalidateRect(hwnd,nullptr,FALSE);
        }
        return 0;
      case WM_DPICHANGED:
      case WM_DPICHANGED_AFTERPARENT:
      case WM_SETTINGCHANGE:
      case WM_THEMECHANGED:
      case WM_SYSCOLORCHANGE:
        if (s && s->list) {
            s->paint.Reset(); // system palette may change without a flag change
            if (msg == WM_DPICHANGED || msg == WM_DPICHANGED_AFTERPARENT)
                BrowserApplyMetrics(s,true);
            BrowserApplyTheme(s);
        }
        return 0;
      case WM_COMMAND:
        if (s && LOWORD(wp) == kBrowserCancelButtonId && HIWORD(wp) == BN_CLICKED &&
            s->progressVisible && !s->progressCancelling &&
            s->progressStage != ARCHIVE_PROGRESS_EXTERNAL && s->parent &&
            IsWindow(s->parent)) {
            if (PostMessageW(s->parent,ARCHIVE_BROWSER_CANCEL_FROM_FOOTER,
                ArchiveRequestUiToken(s->progressRequestId),0)) {
                s->progressClickAt = GetTickCount64();
                s->progressCancelling = true;
                EnableWindow(s->progressCancelButton,FALSE);
                InvalidateRect(hwnd,nullptr,FALSE);
            }
            return 0;
        }
        break;
      case WM_DRAWITEM:
        if (s && static_cast < int > (wp) == kBrowserCancelButtonId) {
            const auto * di = reinterpret_cast < DRAWITEMSTRUCT *  > (lp);
            if (!di) return TRUE;
            const COLORREF parent = BrowserPanel(s);
            ArchivePreviewButtons::Paint(di->hDC,di->hwndItem,di->rcItem,
                di->itemState,s->rowFont,ArchivePreviewText::Get(ArchivePreviewText::Id::Cancel),parent,
                s->dark,s->highContrast,s->currentDpi);
            return TRUE;
        }
        break;
      case WM_PAINT: {
        if (s && ArchivePreviewDiagnostics::Enabled()) ++s->hostPaints;
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(hwnd, & paint);
        BrowserPaintHostContent(s,hwnd,dc);
        EndPaint(hwnd, & paint);
        return 0;
      }
      case WM_PRINTCLIENT:
        // Explorer requests an immediate host frame during reparenting.
        BrowserPaintHostContent(s,hwnd,reinterpret_cast < HDC > (wp));
        return 0;
      case WM_NOTIFY:
        if (s && reinterpret_cast < NMHDR *  > (lp) && reinterpret_cast < NMHDR *  > (lp)->hwndFrom == s->list) {
            NMHDR * n = reinterpret_cast < NMHDR *  > (lp);
            if (n->code == LVN_GETDISPINFOW) {
                auto * d = reinterpret_cast < NMLVDISPINFOW *  > (lp);
                const int i = d->item.iItem;
                if (i >= 0 && static_cast < size_t > (i) < s->entries.size() &&
                    (d->item.mask & LVIF_TEXT) && d->item.pszText && d->item.cchTextMax > 0) {
                    const auto & e = s->entries[static_cast < size_t > (i)];
                    const int sub = d->item.iSubItem;
                    const BrowserColumnKind kind = sub >= 0 && static_cast < size_t > (sub) < s->visibleColumns.size()
                        ? s->visibleColumns[static_cast < size_t > (sub)] : BrowserColumnKind::Name;
                    const std::wstring & source = BrowserEntryColumn(e,kind);
                    wcsncpy_s(d->item.pszText,static_cast < size_t > (d->item.cchTextMax),
                              source.c_str(),_TRUNCATE);
                }
                return 0;
            }
            if (n->code == LVN_ODFINDITEMW) {
                const auto * find = reinterpret_cast < const NMLVFINDITEMW *  > (lp);
                if (!find || !(find->lvfi.flags & LVFI_STRING) || !find->lvfi.psz || s->entries.empty())
                    return - 1;
                const size_t first = static_cast < size_t > (std::max(0,find->iStart));
                for (size_t jump = 0;jump < s->entries.size();++jump) {
                    const size_t i = (first + jump) % s->entries.size();
                    const auto & name = s->entries[i].name;
                    const size_t chars = wcslen(find->lvfi.psz);
                    if (name.size() >= chars && _wcsnicmp(name.c_str(),find->lvfi.psz,chars) == 0)
                        return static_cast < LRESULT > (i);
                }
                return - 1;
            }
            if (n->code == NM_CUSTOMDRAW)
                return BrowserCustomDraw(s,reinterpret_cast < NMLVCUSTOMDRAW *  > (lp));
            // Virtual ListView reports bulk selection through
            // LVN_ODSTATECHANGED, not necessarily LVN_ITEMCHANGED.
            if (n->code == LVN_ODSTATECHANGED && !s->updating) {
                const auto * change = reinterpret_cast < const NMLVODSTATECHANGE *  > (lp);
                if (change && ((change->uNewState ^ change->uOldState) & LVIS_SELECTED)) {
                    if (s->host) {
                        SetTimer(s->host,kBrowserSaveTimer,160,nullptr);
                        BrowserInvalidateFooter(s);
                        BrowserNotifySelectionCount(s);
                    }
                    InvalidateRect(s->list,nullptr,FALSE);
                }
                return 0;
            }
            if (n->code == LVN_ITEMCHANGED && !s->updating) {
                const auto * changed = reinterpret_cast < const NMLISTVIEW *  > (lp);
                if (changed && (changed->uChanged & LVIF_STATE) &&
                    ((changed->uNewState ^ changed->uOldState) & LVIS_SELECTED)) {
                    // Save a snapshot soon after selection changes so an Explorer
                    // refresh after extraction cannot destroy it first.
                    if (s->host) {
                        SetTimer(s->host,kBrowserSaveTimer,160,nullptr);
                        BrowserInvalidateFooter(s);
                        BrowserNotifySelectionCount(s);
                    }
                    if (changed->iItem >= 0 && changed->iItem < ListView_GetItemCount(s->list))
                        ListView_RedrawItems(s->list,changed->iItem,changed->iItem);
                    else InvalidateRect(s->list,nullptr,FALSE);
                }
            }
            if (n->code == LVN_MARQUEEBEGIN) {
                // TRUE cancels the native rectangle when a row initiated the
                // gesture. Blank space still gets the Windows marquee.
                return s->pressIndex >= 0 ? TRUE : FALSE;
            }
            if (n->code == NM_DBLCLK) {
                const auto * click = reinterpret_cast < const NMITEMACTIVATE *  > (lp);
                BrowserOpenSelected(s,click?click->iItem: - 1);
                return 0;
            }
            if (n->code == LVN_BEGINDRAG) {
                const auto * item = reinterpret_cast < const NMLISTVIEW *  > (lp);
                BrowserTransfer(s,true,item?item->iItem: - 1);
                return 0;
            }
        }
        break;
      case WM_ERASEBKGND: {
        if (s && ArchivePreviewDiagnostics::Enabled()) ++s->hostErases;
        HDC dc = reinterpret_cast < HDC > (wp);
        if (dc) {RECT r{};GetClientRect(hwnd, & r);
            HBRUSH fill = s && s->paint.background ? s->paint.background :
                CreateSolidBrush(BrowserBackground(s));
            if (fill) FillRect(dc, & r,fill);
            if (fill && (!s || fill != s->paint.background)) DeleteObject(fill);
        }
        return 1;
      }
      case WM_NCDESTROY:
        if (s && g_gripManager.load() && s->anchor)
            PostMessageW(g_gripManager.load(),WM_BROWSER_REAP,
                         reinterpret_cast < WPARAM > (s->anchor),0);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
        return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
// Repair only a preview branch occluded inside the same Explorer window.
// Raising the broker child alone cannot raise its parent above a sibling
// preview container. Never hide/destroy a foreign preview or activate Explorer.
static void BrowserRepairInputOrder(BrowserState * s) {
    if (!s || s->parked || s->retireWhenIdle || !s->host || !s->list ||
        !IsWindowVisible(s->host) || !IsWindow(s->list)) return;
    RECT bounds{};
    if (!GetWindowRect(s->list,&bounds) || bounds.right <= bounds.left ||
        bounds.bottom <= bounds.top) return;
    POINT point{bounds.left + (bounds.right-bounds.left)/2,
        bounds.top + std::min<LONG>(12,(bounds.bottom-bounds.top-1)/2)};
    POINT cursor{};
    if (GetCursorPos(&cursor) && PtInRect(&bounds,cursor)) point = cursor;
    const HWND hit = WindowFromPoint(point);
    if (!hit || hit == s->host || IsChild(s->host,hit)) return;
    const HWND root = GetAncestor(s->host,GA_ROOT);
    wchar_t hitClass[96]{};
    GetClassNameW(hit,hitClass,_countof(hitClass));
    // Restrict repair to the Shell preview wrapper observed in the FBX log.
    if (wcscmp(hitClass,L"Shell Preview Extension Host Previewer") != 0) return;
    const HWND hitRoot = GetAncestor(hit,GA_ROOT);
    Wh_Log(L"[AP input2] blocked host=%p parent=%p hit=%p class=%s "
           L"root=%p hitRoot=%p hitStyle=%llX hitExStyle=%llX ancestor=%d",
        s->host,s->parent,hit,hitClass,root,hitRoot,
        static_cast<unsigned long long>(GetWindowLongPtrW(hit,GWL_STYLE)),
        static_cast<unsigned long long>(GetWindowLongPtrW(hit,GWL_EXSTYLE)),
        IsChild(hit,s->host)?1:0);
    for (HWND node = ArchivePreviewDiagnostics::Enabled()?s->host:nullptr;node;) {
        wchar_t name[96]{};
        GetClassNameW(node,name,_countof(name));
        Wh_Log(L"[AP input2] chain node=%p class=%s parent=%p enabled=%d "
               L"style=%llX exStyle=%llX",node,name,GetAncestor(node,GA_PARENT),
            IsWindowEnabled(node)?1:0,
            static_cast<unsigned long long>(GetWindowLongPtrW(node,GWL_STYLE)),
            static_cast<unsigned long long>(GetWindowLongPtrW(node,GWL_EXSTYLE)));
        if (node == root) break;
        HWND next = GetAncestor(node,GA_PARENT);
        if (next == node) break;
        node = next;
    }
    if (!root || hitRoot != root || IsChild(hit,s->host)) return;
    // Find the lowest common parent, then raise only our branch below it.
    HWND branch = s->host;
    for (unsigned depth = 0;depth < 32 && branch && branch != root;++depth) {
        const HWND parent = GetAncestor(branch,GA_PARENT);
        if (!parent || parent == branch) return;
        if (parent == hit || IsChild(parent,hit)) {
            if (!(GetWindowLongPtrW(branch,GWL_STYLE) & WS_CHILD)) return;
            UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
            // A foreign UI thread must never hold up the broker's input loop.
            if (GetWindowThreadProcessId(branch,nullptr) != GetCurrentThreadId())
                flags |= SWP_ASYNCWINDOWPOS;
            SetLastError(0);
            const BOOL result = SetWindowPos(branch,HWND_TOP,0,0,0,0,flags);
            const DWORD error = result?0:GetLastError();
            Wh_Log(L"[AP input2] raise branch=%p commonParent=%p result=%d "
                   L"error=%lu async=%d hitAfter=%p",branch,parent,result?1:0,error,
                (flags & SWP_ASYNCWINDOWPOS)?1:0,WindowFromPoint(point));
            return;
        }
        branch = parent;
    }
}
static bool PositionBrowser(BrowserState * s) {
    if (!s || s->parked || !s->host || !ValidBrowserAnchor(s->anchor) ||
        GetParent(s->anchor) != s->parent) return false;
    if (s->retireWhenIdle) {
        if (IsWindowVisible(s->host)) ShowWindow(s->host,SW_HIDE);
        return true;
    }
    // The in-process anchor is deliberately hidden after a successful attach.
    // Follow the preview *parent*, not the anchor: Explorer can deselect the
    // archive after an extraction while an embedded broker HWND still exists.
    if (!IsWindowVisible(s->parent)) {
        if (IsWindowVisible(s->host)) ShowWindow(s->host,SW_HIDE);
        return true;
    }
    RECT r{};GetWindowRect(s->anchor, & r);
    POINT a{r.left,r.top},b{r.right,r.bottom};
    if (!ScreenToClient(s->parent, & a) || !ScreenToClient(s->parent, & b)) return false;
    RECT existing{};GetWindowRect(s->host, & existing);
    const int w = std::max(1,static_cast < int > (b.x - a.x)),h = std::max(1,static_cast < int > (b.y - a.y));
    POINT topLeft{existing.left,existing.top};
    ScreenToClient(s->parent, & topLeft);
    const bool changed = topLeft.x != a.x || topLeft.y != a.y ||
        existing.right - existing.left != w || existing.bottom - existing.top != h;
    if (changed) {
        SetWindowPos(s->host,HWND_TOP,a.x,a.y,w,h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        BrowserRedrawHeader(s);
    } else if (!IsWindowVisible(s->host)) {
        // Re-show without a redundant size/layout notification. The opaque
        // chrome children will paint normally on WM_SHOWWINDOW/WM_PAINT.
        ShowWindow(s->host,SW_SHOWNOACTIVATE);
        if (s->header) InvalidateRect(s->header,nullptr,FALSE);
        BrowserInvalidateFooter(s);
    }
    // Reattachment can keep identical bounds while another preview child
    // remains above our host. Geometry equality does not imply correct Z order.
    if (IsWindowVisible(s->host) && GetWindow(s->host,GW_HWNDPREV)) {
        HWND above = GetWindow(s->host,GW_HWNDPREV);
        const BOOL raised = SetWindowPos(s->host,HWND_TOP,0,0,0,0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        Wh_Log(L"[AP input] restore order host=%p previousAbove=%p result=%d",
            s->host,above,raised?1:0);
    }
    BrowserRepairInputOrder(s);
    return true;
}
static HWND BrowserCreateListControl(HWND host,DWORD visibility = 0) {
    return CreateWindowExW(0,WC_LISTVIEWW,L"",WS_CHILD | visibility | WS_VSCROLL |
        WS_TABSTOP | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHOWSELALWAYS |
        LVS_SHAREIMAGELISTS | LVS_OWNERDATA,0,0,1,1,host,nullptr,g_module,nullptr);
}
static bool BrowserConfigureList(BrowserState * s) {
    ListView_SetExtendedListViewStyleEx(s->list,
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP | LVS_EX_GRIDLINES,
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
    return BrowserRebuildColumns(s) &&
        SetWindowSubclass(s->list,BrowserListSubclass,1,reinterpret_cast<DWORD_PTR>(s));
}
static bool BrowserEnsureList(BrowserState * s,bool restoreView) {
    if (!s || !s->host || !IsWindow(s->host)) return false;
    if (restoreView && BrowserListHasRows(s)) return true;
    const int expected = static_cast<int>(s->entries.size());
    const HWND previous = s->list && IsWindow(s->list) && GetParent(s->list) == s->host ?
        s->list : nullptr;
    Wh_Log(L"[AP list] rebuild-begin instance=%llu expected=%d actual=%d",
        static_cast<unsigned long long>(s->instanceId),expected,
        previous?ListView_GetItemCount(previous):-1);
    const HWND replacement = BrowserCreateListControl(s->host);
    if (!replacement) {
        Wh_Log(L"[AP list] rebuild-create-failed win32=%lu",GetLastError());
        return false;
    }
    const bool hadFocus = previous && GetFocus() == previous;
    const bool wasUpdating = s->updating;
    auto oldColumns = std::move(s->visibleColumns);
    const int oldLanguage = s->appliedLanguage;
    s->updating = true;
    s->list = replacement;
    if (s->rowFont)
        SendMessageW(replacement,WM_SETFONT,reinterpret_cast<WPARAM>(s->rowFont),FALSE);
    if (s->rowSpacingIcons) ListView_SetImageList(replacement,s->rowSpacingIcons,LVSIL_SMALL);
    if (!BrowserConfigureList(s) || !ListView_SetItemCountEx(replacement,expected,LVSICF_NOSCROLL) ||
        !BrowserListHasRows(s)) {
        s->list = previous;
        s->visibleColumns = std::move(oldColumns);
        s->appliedLanguage = oldLanguage;
        s->updating = wasUpdating;
        DestroyWindow(replacement);
        Wh_Log(L"[AP list] rebuild-setup-failed expected=%d",expected);
        return false;
    }
    BrowserApplyTheme(s);
    BrowserLayoutProgress(s);
    BrowserLayoutColumns(s);
    bool restored = false;
    if (restoreView && s->hasRenderedFolder && !s->partialModelPreview) {
        const auto & views = BrowserCache(s->archive).views;
        if (const auto saved = views.find(s->renderedFolder); saved != views.end()) {
            BrowserRestoreView(s,saved->second);
            restored = true;
        }
    }
    if (previous) {
        RemoveWindowSubclass(previous,BrowserListSubclass,1);
        DestroyWindow(previous);
    }
    ShowWindow(replacement,SW_SHOWNA);
    if (hadFocus) SetFocus(replacement);
    s->hoverIndex = -1;
    s->pressIndex = -1;
    s->pressSelectionRows.clear();
    s->trackingMouse = false;
    s->pressCustomHit = false;
    s->pressSelectionOversized = false;
    s->pressWasSelected = false;
    s->updating = wasUpdating;
    InvalidateRect(s->list,nullptr,FALSE);
    BrowserScheduleListSettle(s);
    Wh_Log(L"[AP list] rebuild-complete rows=%d restored_view=%d",expected,restored?1:0);
    return true;
}
static bool CreateBrowser(BrowserState * s) {
    if (!s || !ValidBrowserAnchor(s->anchor)) return false;
    const bool diagnostics = ArchivePreviewDiagnostics::Enabled();
    const ULONGLONG createdAt = diagnostics?GetTickCount64():0;
    s->parent = GetParent(s->anchor);
    // Create the actual ListView *in the medium-integrity broker*.
    // It becomes a true child of the preview window using SetParent, not a
    // transparent overlay or an input-forwarding proxy.
    HWND host = CreateWindowExW(WS_EX_NOPARENTNOTIFY,kBrowserHostClass,L"",
        WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0,0,1,1,nullptr,nullptr,g_module,s);
    if (!host) return false;
    LONG_PTR style = GetWindowLongPtrW(host,GWL_STYLE);
    SetWindowLongPtrW(host,GWL_STYLE,
        (style & ~WS_POPUP) | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetLastError(0);
    HWND oldParent = SetParent(host,s->parent);
    if ((!oldParent && GetLastError() != 0) || GetParent(host) != s->parent) {
        DWORD e = GetLastError();
        wchar_t line[120]{};
        swprintf_s(line,L"SetParent cross-process failed: Win32=%lu",e);Wh_Log(L"%s", line);
        DestroyWindow(host);return false;
    }
    s->host = host;
    // Initialize the correct palette before Windows can send the first paint.
    s->highContrast = BrowserHighContrast();
    s->dark = !s->highContrast && BrowserDarkTheme();
    BrowserRefreshPaintColors(s);
    s->header = CreateWindowExW(WS_EX_NOPARENTNOTIFY,kBrowserHeaderClass,L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0,0,1,1,host,nullptr,g_module,s);
    if (!s->header) {DestroyWindow(host);s->host = nullptr;return false;}
    s->list = BrowserCreateListControl(host,WS_VISIBLE);
    if (!s->list) {DestroyWindow(host);s->host = nullptr;return false;}
    s->footer = CreateWindowExW(WS_EX_NOPARENTNOTIFY,kBrowserFooterClass,L"",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,0,0,1,1,host,nullptr,
        g_module,s);
    if (!s->footer) {DestroyWindow(host);s->host = nullptr;s->list = nullptr;return false;}
    s->progressCancelButton = CreateWindowExW(0,L"BUTTON",ArchivePreviewText::Get(ArchivePreviewText::Id::Cancel),
        WS_CHILD | WS_TABSTOP | BS_OWNERDRAW,
        0,0,1,1,host,reinterpret_cast < HMENU > (static_cast < INT_PTR > (kBrowserCancelButtonId)),
        g_module,nullptr);
    if (!s->progressCancelButton) {
        DestroyWindow(host);s->host = nullptr;s->list = nullptr;return false;
    }
    ArchivePreviewButtons::Attach(s->progressCancelButton);
    // Follow the effective DPI and Windows message font, including during
    // per-monitor DPI transitions. The system small-icon size sets the row scale.
    BrowserApplyMetrics(s, true);
    if (!BrowserConfigureList(s)) {
        DestroyWindow(host);s->host = nullptr;s->list = nullptr;return false;
    }
    BrowserApplyTheme(s);
    if (FAILED(BrowserRefresh(s))) {
        RemoveWindowSubclass(s->list,BrowserListSubclass,1);
        DestroyWindow(host);s->host = nullptr;s->list = nullptr;return false;
    }
    BrowserNotifySelectionCount(s);
    if (!PositionBrowser(s)) {
        RemoveWindowSubclass(s->list,BrowserListSubclass,1);
        DestroyWindow(host);s->host = nullptr;s->list = nullptr;return false;
    }
    RedrawWindow(s->host,nullptr,nullptr,
        RDW_INVALIDATE | RDW_NOERASE | RDW_ALLCHILDREN);
    SetTimer(s->host,kBrowserTimer,1100,nullptr);
    SetTimer(s->host,kBrowserProgressTimer,s->progressPollMs,nullptr);
    // Share the medium-IL ListView palette with the low-IL toolbar/footer.
    // Prevhost may report a light host color after Extract Here in dark mode.
    PostMessageW(s->parent,ARCHIVE_BROWSER_READY,s->dark?1:0,0);
    BrowserNotifyDepth(s);
    if (diagnostics)
        Wh_Log(L"[AP list] attached pid=%lu list=%p rows=%zu ready_ms=%llu dpi=%u",
            GetCurrentProcessId(),s->list,s->entries.size(),GetTickCount64() - createdAt,s->currentDpi);
    return true;
}
static void BrowserAdjustProgressPoll(BrowserState * s,bool active) {
    if (!s || !s->host || !IsWindow(s->host)) return;
    const UINT wanted = active?250u:900u;
    if (s->progressPollMs == wanted) return;
    s->progressPollMs = wanted;
    SetTimer(s->host,kBrowserProgressTimer,wanted,nullptr);
}
static void BrowserSyncProgress(BrowserState * s) {
    if (!s || !s->host || !IsWindow(s->host)) return;
    DWORD ownerPid = 0;
    if (s->parent) GetWindowThreadProcessId(s->parent, & ownerPid);
    const auto state = ownerPid?BrowserQueryProgress(s->archive,ownerPid):BrowserProgressSnapshot{};
    BrowserAdjustProgressPoll(s,state.active);
    const ULONGLONG now = GetTickCount64();
    const bool show = state.active && state.workStarted &&
        now >= state.workStarted && now - state.workStarted >= ArchivePreviewOptions::kExtractionProgressDelayMs;
    const bool same = state.active &&
        InlineIsEqualGUID(state.requestId,s->progressRequestId);
    if (!same) s->progressClickAt = 0;
    const bool cancelling = state.cancelling ||
        (same && s->progressClickAt && now >= s->progressClickAt &&
         now - s->progressClickAt < 2500);
    const ULONGLONG elapsed = state.workStarted && now >= state.workStarted
        ? (now-state.workStarted)/1000 : 0;
    const bool animate = state.stage != ARCHIVE_PROGRESS_EXTRACTING ||
        !state.known || state.percent == 0 || state.percent >= 100;
    const bool layoutChanged = s->progressVisible != show;
    const bool repaint = layoutChanged ||
        (show && (s->progressStage != state.stage ||
            s->progressKnown != state.known ||
            s->progressPercent != state.percent ||
            s->progressCancelling != cancelling || !same || animate ||
            s->progressElapsedSeconds != elapsed));
    s->progressVisible = show;
    s->progressStage = state.stage;
    s->progressKnown = state.known;
    s->progressElapsedSeconds = elapsed;
    s->progressPercent = state.percent;
    s->progressCancelling = cancelling;
    s->progressRequestId = state.requestId;
    if (layoutChanged) BrowserLayoutProgress(s);
    else if (s->progressCancelButton && show)
        EnableWindow(s->progressCancelButton,!cancelling && state.stage != ARCHIVE_PROGRESS_EXTERNAL);
    if (repaint) {
        RECT area{};GetClientRect(s->host, & area);
        RECT dirty = BrowserProgressRect(s,area);
        // When hiding, repaint the previous strip and the newly exposed list.
        if (layoutChanged) InvalidateRect(s->host,nullptr,FALSE);
        else InvalidateRect(s->host, & dirty,FALSE);
    }
}
static LRESULT CALLBACK BrowserHostWindowProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto * s = reinterpret_cast < BrowserState *  > (GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if (msg == WM_TIMER && wp == kBrowserListSettleTimer && s) {
        KillTimer(hwnd,kBrowserListSettleTimer);
        if (!s->parked && !s->retireWhenIdle && !s->updating && !s->dragging) {
            if (!BrowserEnsureList(s)) {
                Wh_Log(L"[AP list] settle failed; restoring Shell fallback");
                s->retireWhenIdle = true;
                ShowWindow(s->host,SW_HIDE);
                if (s->parent) PostMessageW(s->parent,ARCHIVE_BROWSER_FAILED,0,0);
                if (HWND manager = g_gripManager.load())
                    PostMessageW(manager,WM_BROWSER_REAP,reinterpret_cast<WPARAM>(s->anchor),1);
                return 0;
            }
            if (IsWindowVisible(s->list))
                RedrawWindow(s->list,nullptr,nullptr,
                    RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
            BrowserNotifySelectionCount(s);
        }
        BrowserRepairInputOrder(s);
        BrowserDiagnosticSnapshot(s,L"settled");
        ArchivePreviewIcons::LogStats(L"preview");
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserProgressTimer && s) {
        BrowserSyncProgress(s);
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserExtractStatusTimer && s) {
        KillTimer(hwnd,kBrowserExtractStatusTimer);
        if (s->extractStatusUntil && GetTickCount64() >= s->extractStatusUntil) {
            s->extractStatus.clear();
            s->extractStatusUntil = 0;
            BrowserRedrawHeader(s,true);
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserHeavyDebounceTimer && s) {
        KillTimer(hwnd,kBrowserHeavyDebounceTimer);
        if (s->heavyModelDeferred && !s->parked) {
            s->heavyModelDeferred = false;
            // Recheck the file signature and in-flight registry at execution.
            BrowserScheduleModel(s,true);
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserModelRetryTimer && s) {
        KillTimer(hwnd,kBrowserModelRetryTimer);
        if (!s->modelPending || s->modelPendingFolder != s->folder) {
            // A manual refresh must bypass cached data even after a worker-
            // capacity retry. A retained partial view is not a complete cache
            // hit: resume its enumeration instead of leaving it incomplete.
            const bool manual = s->manualRefreshPending &&
                s->manualRefreshFolder == s->folder;
            const HRESULT hr = (manual || s->partialModelPreview)
                ? BrowserScheduleModel(s,true) : BrowserRefresh(s);
            if (manual && FAILED(hr)) {
                s->manualRefreshPending = false;
                s->manualRefreshFolder.clear();
                wchar_t message[100]{};
                swprintf_s(message,L"Refresh failed: 0x%08lX",
                    static_cast < unsigned long > (hr));
                s->extractStatus = message;
                s->extractStatusUntil = GetTickCount64() + 5000;
                SetTimer(s->host,kBrowserExtractStatusTimer,5200,nullptr);
                BrowserRedrawHeader(s,true);
            }
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserHeaderRepairTimer && s) {
        KillTimer(hwnd,kBrowserHeaderRepairTimer);
        if (ArchivePreviewDiagnostics::Enabled()) ++s->repairs;
        // Immediate-only refresh can skip the required header paint.
        // Request a new frame from both independent opaque child surfaces.
        if (s->header && IsWindowVisible(s->header))
            RedrawWindow(s->header,nullptr,nullptr,RDW_INVALIDATE | RDW_NOERASE);
        BrowserInvalidateFooter(s);
        // Ask the compositor for its current corner pixels before any
        // forced synchronous painting; this isolates the residual white ZIP.
        BrowserChromeAudit(s,L"after-extract",true);
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserSaveTimer && s) {
        KillTimer(hwnd,kBrowserSaveTimer);
        if (!s->updating && !s->dragging) BrowserSaveView(s);
        return 0;
    }
    if (msg == WM_TIMER && wp == kBrowserTimer && s) {
        if (s->retireWhenIdle) {
            if (s->host && IsWindowVisible(s->host)) ShowWindow(s->host,SW_HIDE);
            if (!s->dragging) {
                HWND manager = g_gripManager.load();
                if (manager) PostMessageW(manager,WM_BROWSER_REAP,
                    reinterpret_cast < WPARAM > (s->anchor),1);
            }
            return 0;
        }
        if (s->dragging) return 0;
        if (!s->updating && !BrowserListHasRows(s)) BrowserScheduleListSettle(s);
        if (!ValidBrowserAnchor(s->anchor) || !PositionBrowser(s)) {
            PostMessageW(g_gripManager.load(),WM_BROWSER_REAP,
                reinterpret_cast < WPARAM > (s->anchor),0);
        } else {
            const ULONGLONG now = GetTickCount64();
            static ULONGLONG nextColumnPreferencesCheck = 0;
            if (now >= nextColumnPreferencesCheck) {
                nextColumnPreferencesCheck = now + 2000;
                const auto changed = ArchivePreviewColumns::Load();
                if (changed.widths || changed.visibility)
                    if (HWND manager = g_gripManager.load())
                        PostMessageW(manager,WM_BROWSER_COLUMN_PREFERENCES_CHANGED,changed.visibility,0);
            }
            if (s->selectionGuardDeadline && now >= s->selectionGuardNext) {
                s->selectionGuardNext = now + 380;
                if (now > s->selectionGuardDeadline) {
                    s->selectionGuardDeadline = 0;
                    s->selectionMismatchCount = 0;
                } else {
                    const auto selection = SelectedItemInOwningExplorer(s);
                    if (!s->selectionGuardLogged) {
                        const wchar_t * state = selection == ExplorerSelection::OtherSelected
                            ? L"different item" : (selection == ExplorerSelection::ArchiveSelected
                                ? L"archive still selected" : L"unavailable");
                        Wh_Log(L"%s", std::wstring(std::wstring(L"Post-drop Explorer selection: ") + state).c_str());
                        s->selectionGuardLogged = true;
                    }
                    if (selection == ExplorerSelection::OtherSelected) {
                        if (++s->selectionMismatchCount >= 2) {
                            Wh_Log(L"Explorer selected another item; detaching stale archive preview");
                            s->selectionGuardDeadline = 0;
                            if (s->parent && IsWindow(s->parent))
                                PostMessageW(s->parent,ARCHIVE_BROWSER_SELECTION_LOST,0,0);
                            HWND manager = g_gripManager.load();
                            if (manager) PostMessageW(manager,WM_BROWSER_REAP,
                                reinterpret_cast < WPARAM > (s->anchor),1);
                            return 0;
                        }
                    } else s->selectionMismatchCount = 0;
                }
            }
            const UINT dpi = GetDpiForWindow(s->host);
            // Keep post-drop selection detection fast, but reduce polling
            // while the preview is idle (no selection-change guard active).
            SetTimer(hwnd,kBrowserTimer,s->selectionGuardDeadline?300:1100,nullptr);
            if (dpi && dpi != s->currentDpi) BrowserApplyMetrics(s,true);
            if (!s->lastThemeCheck || now - s->lastThemeCheck >= 1800) {
                s->lastThemeCheck = now;
                const bool hc = BrowserHighContrast();
                const bool dark = !hc && BrowserDarkTheme();
                if (dark != s->dark || hc != s->highContrast ||
                    s->paint.accent != ArchivePreviewHost::SystemAccentColor(hc) || BrowserHostColorsChanged(s)) {
                    BrowserApplyTheme(s);
                    // Keep low-IL toolbar/footer consistent with this ListView
                    // when the user changes the Windows color mode at runtime.
                    if (s->parent && IsWindow(s->parent))
                        PostMessageW(s->parent,ARCHIVE_BROWSER_READY,s->dark?1:0,0);
                }
            }
        }
        return 0;
    }
    return BrowserHostProc(hwnd,msg,wp,lp);
}
static void BrowserDispose(std::unordered_map < HWND,std::shared_ptr < BrowserState >> ::iterator it);
// The Preview Handler must still honor Unload and destroy *its* HWNDs.
// Only the independent, medium-integrity broker may keep its own controls
// briefly. Reparent before the old Explorer preview container is destroyed.
static void BrowserSweepParked() {
    const ULONGLONG now = GetTickCount64();
    for (auto it = g_browsers.begin();it != g_browsers.end();) {
        BrowserState * s = it->second.get();
        if (!s || !s->parked || (now >= s->parkedSince &&
                now - s->parkedSince < kBrowserParkTtlMs)) { ++it;continue; }
        auto old = it++;
        BrowserDispose(old);
    }
}
static void BrowserDispose(std::unordered_map < HWND,std::shared_ptr < BrowserState >> ::iterator it) {
    if (it == g_browsers.end()) return;
    BrowserState * s = it->second.get();
    // A model ticket may be shared by a newly attached preview. Removing
    // this view must not invalidate the worker for the other subscriber.
    s->modelCancelled.reset();
    s->heavyModelDeferred = false;
    if (s->host && IsWindow(s->host)) {
        KillTimer(s->host,kBrowserSaveTimer);
        KillTimer(s->host,kBrowserTimer);
        KillTimer(s->host,kBrowserHeaderRepairTimer);
        KillTimer(s->host,kBrowserListSettleTimer);
        KillTimer(s->host,kBrowserModelRetryTimer);
        KillTimer(s->host,kBrowserHeavyDebounceTimer);
        KillTimer(s->host,kBrowserExtractStatusTimer);
        KillTimer(s->host,kBrowserProgressTimer);
    }
    BrowserSaveView(s);
    if (s->list && IsWindow(s->list))
        RemoveWindowSubclass(s->list,BrowserListSubclass,1);
    if (s->host && IsWindow(s->host)) {
        ShowWindow(s->host,SW_HIDE);
        DestroyWindow(s->host);
    }
    g_browsers.erase(it);
}

// Park controls only on the broker UI thread; there is no active transfer to
// retain, no active extraction to steal, and the pane belongs to a live root.
static bool BrowserPark(std::unordered_map < HWND,std::shared_ptr < BrowserState >> ::iterator it) {
    if (it == g_browsers.end() || !g_browserParkHost || !IsWindow(g_browserParkHost)) return false;
    BrowserState * s = it->second.get();
    const GUID zero{};
    if (!s || s->parked || s->dragging ||
        s->progressVisible || !InlineIsEqualGUID(s->pendingExtract,zero) ||
        !s->host || !IsWindow(s->host) || !s->parent || !IsWindow(s->parent)) return false;
    HWND root = GetAncestor(s->parent,GA_ROOT);
    if (!root || !IsWindow(root)) return false;
    BrowserSaveView(s);
    // Use an anchored file identity, not just size/path. A rewritten or
    // replaced archive must never receive rows from an old preview instance.
    s->parkedSignatureValid = GetArchiveSignature(s->archive,
        s->parkedSignatureModified,s->parkedSignatureBytes,
        & s->parkedSignatureIdentity);
    s->parkedWasEnumerating = s->modelPending || s->heavyModelDeferred;
    // DO NOT cancel a running COM enumeration on a transient Explorer Unload.
    // The worker remains tracked and can be reattached by ticket. Otherwise
    // every reattach launches another 10 GiB scan while the old scan is stuck.
    s->heavyModelDeferred = false;
    ++s->modelGeneration; // UI generations are not used for worker identity
    s->manualRefreshPending = false;
    s->manualRefreshFolder.clear();
    ShowWindow(s->host,SW_HIDE);
    // SetParent can fail on some Explorer/DPI configurations. The caller then
    // performs normal disposal, never keeping a child of a dying parent.
    SetLastError(0);
    HWND previous = SetParent(s->host,g_browserParkHost);
    if ((!previous && GetLastError() != 0) || GetParent(s->host) != g_browserParkHost)
        return false;
    for (UINT_PTR timer : {kBrowserTimer,kBrowserSaveTimer,kBrowserHeaderRepairTimer,
        kBrowserListSettleTimer,kBrowserModelRetryTimer,kBrowserHeavyDebounceTimer,
        kBrowserExtractStatusTimer,kBrowserProgressTimer}) KillTimer(s->host,timer);
    s->parked = true;
    s->parkedExplorerRoot = root;
    s->parkedSince = GetTickCount64();
    s->parent = nullptr;
    s->selectionGuardDeadline = 0;
    s->selectionGuardNext = 0;
    s->selectionMismatchCount = 0;
    if (HWND manager = g_gripManager.load())
        SetTimer(manager,kBrowserParkSweepTimer,500,nullptr);
    BrowserDiagnosticSnapshot(s,L"parked");
    Wh_Log(L"ArchivePreview: broker chrome parked after preview Unload");
    return true;
}
static bool BrowserAdopt(BrowserState * s,HWND newAnchor,const std::wstring & newArchive) {
    if (!s || !s->parked || !ValidBrowserAnchor(newAnchor) ||
        !s->host || !IsWindow(s->host) || !s->header || !IsWindow(s->header) ||
        !s->footer || !IsWindow(s->footer) || !s->list || !IsWindow(s->list) ||
        GetParent(s->host) != g_browserParkHost) return false;
    HWND newParent = GetParent(newAnchor);
    if (!newParent || !IsWindow(newParent) ||
        GetAncestor(newParent,GA_ROOT) != s->parkedExplorerRoot ||
        !ArchivePreviewOptions::MatchArchive(newArchive)) return false;
    FILETIME stamp{};ULONGLONG bytes{};
    ArchiveFileIdentity identity{};
    if (!GetArchiveSignature(newArchive,stamp,bytes, & identity)) return false;
    const bool sameArchive = s->parkedSignatureValid &&
        _wcsicmp(s->archive.c_str(),newArchive.c_str()) == 0 &&
        s->parkedSignatureIdentity == identity &&
        s->parkedSignatureBytes == bytes &&
        CompareFileTime( & s->parkedSignatureModified, & stamp) == 0;
    const bool retainedPending = sameArchive && s->parkedWasEnumerating;
    // A finished task may have posted its completion while this view was
    // parked. The manager can cache it without showing it to any HWND.
    // Never preserve a pending state when its inflight ticket has retired.
    bool inflightStillRunning = false;
    if (sameArchive && s->modelCancelled) {
        for (const auto & active:g_browserModelInflight)
            if (active.ticket == s->modelCancelled &&
                active.metadataMask == ArchivePreviewOptions::MetadataMask(bytes) &&
                !active.ticket->load(std::memory_order_acquire)) {
                inflightStillRunning = true; break;
            }
    }
    ShowWindow(s->host,SW_HIDE);
    SetLastError(0);
    HWND previous = SetParent(s->host,newParent);
    if ((!previous && GetLastError() != 0) || GetParent(s->host) != newParent)
        return false;
    // The COM handler can have been recreated while the broker ListView and
    // chrome remain alive. Never carry file-specific selection or worker IDs.
    s->anchor = newAnchor;
    s->parent = newParent;
    s->parked = false;
    s->parkedExplorerRoot = nullptr;
    s->parkedSince = 0;
    s->instanceId = g_nextBrowserId.fetch_add(1);
    ++s->modelGeneration;
    s->archive = newArchive;
    s->parkedSignatureValid = false;
    s->parkedWasEnumerating = false;
    if (!sameArchive) {
        // Switching archives, or changing the file during Unload, always
        // discards old data and resets the owner-data ListView row count.
        s->folder.clear();
        s->sortColumn = BrowserColumnKind::Name;s->sortAscending = true;
        s->renderedFolder.clear();
        s->hasRenderedFolder = false;
        s->partialModelPreview = false;
        s->entries.clear();
        s->updating = true;
        SendMessageW(s->list,WM_SETREDRAW,FALSE,0);
        ListView_SetItemCountEx(s->list,0,LVSICF_NOSCROLL);
        SendMessageW(s->list,WM_SETREDRAW,TRUE,0);
        s->updating = false;
    }
    if (!sameArchive || !inflightStillRunning) {
        s->modelCancelled.reset();
        s->modelPending = false;
        s->modelPendingFolder.clear();
    }
    s->heavyModelDeferred = false;
    s->pendingExtract = GUID{};
    s->extractStatus.clear();
    s->extractStatusUntil = 0;
    s->progressVisible = false;
    s->progressStage = ARCHIVE_PROGRESS_NONE;
    s->progressKnown = false;
    s->progressPercent = 0;
    s->progressCancelling = false;
    s->progressRequestId = GUID{};
    s->progressPollMs = 900;
    s->selectionGuardDeadline = 0;
    s->selectionMismatchCount = 0;
    s->selectionGuardLogged = false;
    s->hoverIndex =  - 1;
    s->pressIndex =  - 1;
    s->pressSelectionRows.clear();
    s->retireWhenIdle = false;
    s->diagnosticRowPaints = s->diagnosticIconDraws = 0;
    s->diagnosticIconMisses = s->diagnosticInvalidRows = 0;
    // Keep the borrowed Shell image list for the allocation-limit fallback.
    // Normal cached rows retain their own icon copies and do not need this
    // handle to survive a Preview Host recreation. The fallback indices refer
    // to the broker's system list, not to an archive-specific allocation.
    if (!s->shellIcons) {
        SHFILEINFOW iconInfo{};
        s->shellIcons = reinterpret_cast < HIMAGELIST > (SHGetFileInfoW(
            L"ArchivePreview.zip",FILE_ATTRIBUTE_NORMAL, & iconInfo,sizeof(iconInfo),
            SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
    }
    const UINT dpi = GetDpiForWindow(s->host);
    if (dpi && dpi != s->currentDpi) BrowserApplyMetrics(s,true);
    std::vector < BrowserColumnKind > requested{BrowserColumnKind::Name};
    if (ArchivePreviewOptions::ShowColumn(1)) requested.push_back(BrowserColumnKind::DateModified);
    if (ArchivePreviewOptions::ShowColumn(2)) requested.push_back(BrowserColumnKind::Type);
    if (ArchivePreviewOptions::ShowColumn(3)) requested.push_back(BrowserColumnKind::Size);
    if (requested != s->visibleColumns || s->appliedLanguage != ArchivePreviewText::Language())
        BrowserRebuildColumns(s);
    BrowserApplyTheme(s);
    HRESULT loaded = S_OK;
    if (sameArchive && retainedPending && inflightStillRunning) {
        // This work is still running; the token survives the reattach.
        Wh_Log(L"ArchivePreview reattach: same unchanged archive; retained active COM task");
    } else if (sameArchive && retainedPending) {
        // It completed while parked; use its cached validated result if
        // available or schedule one new operation after checking signature.
        loaded = BrowserRefresh(s);
        Wh_Log(L"ArchivePreview reattach: completed while parked; cache inspected");
    } else if (sameArchive && s->hasRenderedFolder && !s->partialModelPreview &&
        s->displayedMetadataMask == ArchivePreviewOptions::MetadataMask(bytes)) {
        // Reuse the completed metadata after checking the file identity.
        // The native control is validated below: a surviving HWND can have
        // lost its virtual item state even while all model rows remain intact.
        Wh_Log(L"ArchivePreview reattach: same unchanged archive; reused completed model");
    } else {
        loaded = BrowserRefresh(s);
    }
    if (FAILED(loaded)) return false;
    if (!PositionBrowser(s)) return false;
    // paint the retained chrome once after the new anchor is attached.
    // Do not block the OLE transfer loop or disturb the footer z-order.
    if (s->header && IsWindow(s->header)) InvalidateRect(s->header,nullptr,FALSE);
    BrowserInvalidateFooter(s);
    SetTimer(s->host,kBrowserTimer,1100,nullptr);
    SetTimer(s->host,kBrowserProgressTimer,s->progressPollMs,nullptr);
    // A surviving HWND/style is not proof that its virtual item state survived
    // reparenting. Validate and repair before declaring the retained view ready.
    if (!BrowserEnsureList(s)) return false;
    PostMessageW(newParent,ARCHIVE_BROWSER_READY,s->dark?1:0,0);
    BrowserNotifyDepth(s);
    BrowserNotifySelectionCount(s);
    // SetParent/reparenting may leave stale ListView pixels. Paint the
    // already loaded rows (including their icons) once on reattach; do not
    // restart a potentially expensive 10 GiB Shell enumeration.
    if (s->list && IsWindowVisible(s->list))
        RedrawWindow(s->list,nullptr,nullptr,
            RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW);
    BrowserScheduleListSettle(s);
    BrowserDiagnosticSnapshot(s,sameArchive?L"adopt same archive":L"adopt different archive");
    Wh_Log(L"%s", std::wstring(sameArchive
        ? L"ArchivePreview: broker chrome reused; unchanged archive rows preserved"
        : L"ArchivePreview: broker chrome reused; different archive model scheduled").c_str());
    return true;
}

// Only replace another binding inside the exact same preview container.
// Different live containers can share an Explorer root (tabs, retained views
// or separate panes) and occupy identical screen rectangles. Treating those
// rectangles as ownership caused their 2-second retry timers to repeatedly
// park/adopt the same broker HWND, alternating native and broker designs.
static bool BrowserOverlapsIncomingAnchor(const BrowserState * oldBrowser,HWND anchor) {
    if (!oldBrowser || oldBrowser->parked || !oldBrowser->parent ||
        !IsWindow(oldBrowser->parent) || !ValidBrowserAnchor(anchor)) return false;
    const HWND newParent = GetParent(anchor);
    return newParent && newParent == oldBrowser->parent;
}
static void BrowserRetireOverlappingPreviews(HWND newAnchor) {
    for (auto it = g_browsers.begin();it != g_browsers.end();) {
        if (it->first == newAnchor ||
            !BrowserOverlapsIncomingAnchor(it->second.get(),newAnchor)) {
            ++it;
            continue;
        }
        BrowserState * old = it->second.get();
        Wh_Log(L"[AP ownership] replacing binding within parent=%p oldAnchor=%p newAnchor=%p",
            old->parent,old->anchor,newAnchor);
        // DoDragDrop pumps nested messages. A callback may still reference
        // this BrowserState, so defer destruction until the drag completes.
        if (old->dragging) {
            old->retireWhenIdle = true;
            if (old->host && IsWindow(old->host)) ShowWindow(old->host,SW_HIDE);
            ++it;
        } else {
            auto obsolete = it++;
            if (!BrowserPark(obsolete)) BrowserDispose(obsolete);
        }
        Wh_Log(L"ArchivePreview: parked or retired replaced binding in the same preview container");
    }
}
static LRESULT CALLBACK BrowserManagerProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    // The UI thread destroys this internal window after reclaiming its queue.
    if (msg == WM_CLOSE) return 0;
    if (msg == WM_TIMER && wp == kBrowserParkSweepTimer) {
        BrowserSweepParked();
        size_t parkedCount = 0;
        for (const auto & item:g_browsers) if (item.second->parked) ++parkedCount;
        // Never allow unbounded hidden control trees, even if many Explorer
        // windows churn their preview panes simultaneously.
        while (parkedCount > kBrowserParkMax) {
            auto oldest = g_browsers.end();
            for (auto it = g_browsers.begin();it != g_browsers.end();++it)
                if (it->second->parked && (oldest == g_browsers.end() ||
                    it->second->parkedSince < oldest->second->parkedSince)) oldest = it;
            if (oldest == g_browsers.end()) break;
            BrowserDispose(oldest);
            --parkedCount;
        }
        if (!parkedCount) KillTimer(hwnd,kBrowserParkSweepTimer);
        return 0;
    }
    if (msg == WM_BROWSER_COLUMN_PREFERENCES_CHANGED) {
        if (wp) BrowserCancelOutdatedMetadata();
        for (auto & [anchor,browser] : g_browsers) {
            if (browser->parked || !browser->list || !IsWindow(browser->list)) continue;
            if (wp) {
                BrowserRebuildColumns(browser.get());
                BrowserRefresh(browser.get());
            } else BrowserLayoutColumns(browser.get());
        }
        return 0;
    }
    if (msg == WM_BROWSER_SETTINGS_CHANGED) {
        // Runs on the Broker UI thread: never manipulate the ListView from
        // Windhawk's settings callback, which can be another Explorer thread.
        BrowserCancelOutdatedMetadata();
        for(auto & [anchor,browser]:g_browsers) {
            BrowserState * state = browser.get();
            if (state->parked) continue;
            if(!state || !state->list || !IsWindow(state->list)) continue;
            BrowserRebuildColumns(state);
            if (state->progressCancelButton)
                SetWindowTextW(state->progressCancelButton,
                    ArchivePreviewText::Get(ArchivePreviewText::Id::Cancel));
            state->paint.Reset();
            BrowserApplyTheme(state);
            BrowserRefresh(state);
            BrowserLayoutProgress(state);
        }
        return 0;
    }
    if (msg == WM_BROWSER_MODEL_PREVIEW) {
        std::unique_ptr < BrowserModelPreview > preview(
            reinterpret_cast < BrowserModelPreview *  > (lp));
        if (!preview || preview->ticket->load(std::memory_order_acquire) ||
            preview->metadataMask != ArchivePreviewOptions::MetadataMask(preview->archiveBytes)) return 0;
        // Follow the subscribed work ticket, not an obsolete Explorer HWND.
        // A transient Unload/reattach does not invalidate the actual worker.
        BrowserState * s = BrowserFindModelSubscriber(preview->ticket,
            preview->archive,preview->folder);
        if (!s || (s->hasRenderedFolder && !s->partialModelPreview)) return 0;
        s->updating = true;
        SendMessageW(s->list,WM_SETREDRAW,FALSE,0);
        s->entries.swap(preview->rows);
        const bool applied = BrowserApplyListRows(s);
        if (!applied) {
            s->entries.swap(preview->rows);
            ListView_SetItemCountEx(s->list,static_cast<int>(s->entries.size()),LVSICF_NOSCROLL);
        }
        s->updating = false;
        SendMessageW(s->list,WM_SETREDRAW,TRUE,0);
        if (applied) {
            s->renderedFolder = s->folder;
            s->hasRenderedFolder = true;
            s->partialModelPreview = true;
            s->extractStatus = L"Loading archive contents...";
            s->extractStatusUntil = 0;
            BrowserNotifyDepth(s);
            BrowserRedrawHeader(s,true);
            RedrawWindow(s->list,nullptr,nullptr,RDW_INVALIDATE | RDW_NOERASE);
        }
        BrowserScheduleListSettle(s);
        return 0;
    }
    if (msg == WM_BROWSER_MODEL_READY) {
        std::unique_ptr < BrowserModelJob > job(reinterpret_cast < BrowserModelJob *  > (lp));
        if (!job) return 0;
        BrowserForgetInflight(job->cancelled);
        auto & cache = BrowserCache(job->archive);
        if (job->cancelled->load(std::memory_order_acquire) || job->metadataMask != cache.metadataMask) {
            for (auto & [key,browser]:g_browsers) {
                if (browser->modelCancelled != job->cancelled) continue;
                browser->modelPending = false;browser->modelPendingFolder.clear();
                browser->modelCancelled.reset();
                if (!browser->parked) BrowserRefresh(browser.get(),true);
            }
            return 0;
        }
        BrowserState * s = BrowserFindModelSubscriber(job->cancelled,
            job->archive,job->folder);
        if (!s) {
            // The user selected a different file while COM was running.
            // Keep a small fully validated model so selecting the old
            // archive again does not scan it for a second time.
            if (SUCCEEDED(job->result) && !job->rows.empty() &&
                job->rows.size() <= 12000 && cache.signatureValid &&
                cache.identity == job->archiveIdentity && cache.bytes == job->archiveBytes &&
                CompareFileTime(&cache.modified,&job->modified) == 0) {
                if (cache.folders.size() >= 32 &&
                    cache.folders.find(job->folder) == cache.folders.end()) {
                    auto oldest = cache.folderTouched.end();
                    for (auto ci = cache.folderTouched.begin();
                         ci != cache.folderTouched.end();++ci)
                        if (oldest == cache.folderTouched.end() ||
                            ci->second < oldest->second) oldest = ci;
                    if (oldest != cache.folderTouched.end()) {
                        cache.folders.erase(oldest->first);
                        cache.views.erase(oldest->first);
                                                cache.folderTouched.erase(oldest);
                    }
                }
                cache.folders[job->folder] = std::move(job->rows);
                cache.folderTouched[job->folder] = GetTickCount64();
                BrowserTrimRowCache(job->archive,job->folder);
                Wh_Log(L"ArchivePreview model: completed while detached; cached validated names");
            }
            return 0;
        }
        // Multiple previews may subscribe to the same work. The main
        // subscriber updates the cache first; others then redraw from it.
        std::vector < std::shared_ptr < BrowserState >> additionalSubscribers;
        for (const auto & [otherAnchor,other]:g_browsers)
            if (other.get() != s && !other->parked && other->modelPending &&
                other->modelCancelled == job->cancelled &&
                _wcsicmp(other->archive.c_str(),job->archive.c_str()) == 0 &&
                _wcsicmp(other->folder.c_str(),job->folder.c_str()) == 0)
                additionalSubscribers.push_back(other);
        const auto it = g_browsers.find(s->anchor);
        if (it == g_browsers.end() || it->second.get() != s) return 0;
        s->modelPending = false;
        s->modelPendingFolder.clear();
        if (FAILED(job->result)) {
            for (auto & other:additionalSubscribers) {
                other->modelPending = false;
                other->modelPendingFolder.clear();
                other->modelCancelled.reset();
                if (other->partialModelPreview) {
                    other->partialModelPreview = false;
                    other->hasRenderedFolder = false;
                    other->entries.clear();
                    if (other->list && IsWindow(other->list))
                        ListView_SetItemCountEx(other->list,0,LVSICF_NOSCROLL);
                }
                other->extractStatus = L"Archive listing failed; choose Refresh to retry.";
                other->extractStatusUntil = GetTickCount64() + 7000;
                if (other->host && IsWindow(other->host)) {
                    SetTimer(other->host,kBrowserExtractStatusTimer,7200,nullptr);
                    BrowserRedrawHeader(other.get(),true);
                }
            }
            if (s->partialModelPreview) {
                s->partialModelPreview = false;
                s->hasRenderedFolder = false;
                s->entries.clear();
                if (s->list && IsWindow(s->list))
                    ListView_SetItemCountEx(s->list,0,LVSICF_NOSCROLL);
                s->extractStatus.clear();
            }
            wchar_t code[120]{};
            swprintf_s(code,L"Background archive enumeration failed: 0x%08lX",
                       static_cast < unsigned long > (job->result));
            Wh_Log(L"%s", code);
            if (s->manualRefreshPending && s->manualRefreshFolder == s->folder) {
                s->manualRefreshPending = false;
                s->manualRefreshFolder.clear();
                wchar_t message[100]{};
                swprintf_s(message,L"Refresh failed: 0x%08lX",
                    static_cast < unsigned long > (job->result));
                s->extractStatus = message;
                s->extractStatusUntil = GetTickCount64() + 5000;
                SetTimer(s->host,kBrowserExtractStatusTimer,5200,nullptr);
                BrowserRedrawHeader(s,true);
            }
            if (s->hasRenderedFolder) s->folder = s->renderedFolder;
            else {
                if (s->parent && IsWindow(s->parent))
                    PostMessageW(s->parent,ARCHIVE_BROWSER_FAILED,0,0);
                // Failed first load must not leave a broker window covering
                // the low-integrity fallback list.
                BrowserDispose(it);
            }
            return 0;
        }
        if (!cache.signatureValid || cache.identity != job->archiveIdentity ||
            CompareFileTime(&cache.modified,&job->modified) != 0 || cache.bytes != job->archiveBytes) {
            // Source changed while the worker was enumerating; restart.
            s->partialModelPreview = false;
            s->hasRenderedFolder = false;
            s->entries.clear();
            if (s->list && IsWindow(s->list))
                ListView_SetItemCountEx(s->list,0,LVSICF_NOSCROLL);
            BrowserRefresh(s);
            for (auto & other:additionalSubscribers) {
                if (other->parked || !other->host || !IsWindow(other->host))
                    continue;
                other->modelPending = false;
                other->modelPendingFolder.clear();
                other->modelCancelled.reset();
                other->partialModelPreview = false;
                BrowserRefresh(other.get());
            }
            return 0;
        }
        if (cache.folders.size() >= 32) {
            auto oldest = cache.folderTouched.end();
            for (auto ci = cache.folderTouched.begin();ci != cache.folderTouched.end();++ci)
                if (ci->first != s->folder &&
                    (oldest == cache.folderTouched.end() || ci->second < oldest->second)) oldest = ci;
            if (oldest != cache.folderTouched.end()) {
                cache.folders.erase(oldest->first);
                cache.views.erase(oldest->first);
                cache.folderTouched.erase(oldest);
            }
        }
        // Huge active folders live in BrowserState only, rather than doubling
        // their strings in the metadata cache and exhausting the 24 MiB cap.
        cache.folders[s->folder] = std::move(job->rows);
        cache.folderTouched[s->folder] = GetTickCount64();
        BrowserTrimRowCache(s->archive,s->folder);
        const bool manual = s->manualRefreshPending && s->manualRefreshFolder == s->folder;
        // Force replacement after model completion. The
        // previous same-folder cache fast-path silently kept stale rows.
        const HRESULT display = BrowserRefresh(s,true);
        if (SUCCEEDED(display)) s->partialModelPreview = false;
        else if (s->hasRenderedFolder) {
            s->folder = s->renderedFolder;
            BrowserNotifyDepth(s);
        }
        if (SUCCEEDED(display)) BrowserNotifySelectionCount(s);
        if (SUCCEEDED(display) && !manual) {
            if (s->extractStatus == L"Loading archive contents...") {
                s->extractStatus.clear();
                s->extractStatusUntil = 0;
            }
            BrowserRedrawHeader(s,true);
        }
        if (manual) {
            s->manualRefreshPending = false;
            s->manualRefreshFolder.clear();
            if (SUCCEEDED(display)) s->extractStatus = L"Preview refreshed.";
            else {
                wchar_t message[100]{};
                swprintf_s(message,L"Refresh failed: 0x%08lX",
                    static_cast < unsigned long > (display));
                s->extractStatus = message;
            }
            s->extractStatusUntil = GetTickCount64() + 5000;
            SetTimer(s->host,kBrowserExtractStatusTimer,5200,nullptr);
            BrowserRedrawHeader(s,true);
        }
        if (FAILED(display) && !s->hasRenderedFolder && s->parent && IsWindow(s->parent))
            PostMessageW(s->parent,ARCHIVE_BROWSER_FAILED,0,0);
        for (auto & other:additionalSubscribers) {
            if (other->parked || !other->host || !IsWindow(other->host))
                continue;
            other->modelPending = false;
            other->modelPendingFolder.clear();
            other->modelCancelled.reset();
            // The main subscriber has already published the validated cache.
            // This does not start another COM worker.
            BrowserRefresh(other.get(),true);
        }
        return 0;
    }
    if (msg == WM_BROWSER_LOCAL_EXTRACT_DONE) {
        std::unique_ptr < BrowserLocalExtractResult > result(
            reinterpret_cast < BrowserLocalExtractResult *  > (lp));
        if (!result) return 0;
        for (auto & entry:g_browsers) {
            BrowserState * s = entry.second.get();
            const GUID zero{};
            if (!s || s->parked || InlineIsEqualGUID(s->pendingExtract,zero) ||
                !InlineIsEqualGUID(s->pendingExtract,result->requestId)) continue;
            s->pendingExtract = GUID{};
            if (SUCCEEDED(result->outcome)) s->extractStatus =
                result->command == ARCHIVE_BROKER_OPEN_FILE ? L"File opened." : ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractionComplete);
            else if (result->outcome == HRESULT_FROM_WIN32(ERROR_CANCELLED))
                s->extractStatus = result->command == ARCHIVE_BROKER_OPEN_FILE ? L"Open cancelled." : ArchivePreviewText::Get(ArchivePreviewText::Id::ExtractionCancelled);
            else if (result->outcome == HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY))
                s->extractStatus = L"Extraction incomplete: check destination files.";
            else {
                wchar_t message[96]{};
                swprintf_s(message,result->command == ARCHIVE_BROKER_OPEN_FILE
                    ? L"Open failed: 0x%08lX" : L"Extraction failed: 0x%08lX",
                    static_cast < unsigned long > (result->outcome));
                s->extractStatus = message;
            }
            s->extractStatusUntil = GetTickCount64() + 8000;
            if (s->host && IsWindow(s->host)) {
                SetTimer(s->host,kBrowserExtractStatusTimer,8000,nullptr);
                BrowserInvalidateFooter(s);
            }
            break; // Never route completion by archive path.
        }
        return 0;
    }
    if (msg == WM_BROWSER_HEADER_REFRESH) {
        for (auto & [anchor,browser]:g_browsers) {
            if (browser && !browser->parked && BrowserArchiveKey(browser->archive) == wp) {
                // The Shell can send this immediately after a small archive
                // finishes. Queue one paint rather than synchronously erasing
                // the entire cross-process chrome twice within 220 ms.
                BrowserInvalidateFooter(browser.get());
                // keep a single delayed chrome repair. Repainting the
                // header immediately and again 240 ms later was redundant and
                // could race Explorer's snapshot/reparent sequence.
                // The host has its own delayed repair timer. Do not also
                // post CHROME_REFRESH here: that forced two cross-process
                // redraws while Explorer was updating the destination folder.
                Wh_Log(L"ArchivePreview: chrome post-extract: dark=%d highContrast=%d header=%d footer=%d",
                    browser->dark?1:0,browser->highContrast?1:0,
                    browser->header && IsWindowVisible(browser->header)?1:0,
                    browser->footer && IsWindowVisible(browser->footer)?1:0);
                if (browser->host && IsWindow(browser->host)) {
                    // Explorer can erase this cross-process chrome after
                    // our immediate paint; issue a coalesced delayed repair.
                    SetTimer(browser->host,kBrowserHeaderRepairTimer,240,nullptr);
                    // Extract-to / extract-here can change Explorer selection,
                    // just like dropping a file. Verify it without clearing the UI.
                    SetTimer(browser->host,kBrowserTimer,300,nullptr);
                    browser->selectionGuardDeadline = GetTickCount64() + 6500;
                    browser->selectionGuardNext = GetTickCount64() + 380;
                    browser->selectionMismatchCount = 0;
                    browser->selectionGuardLogged = false;
                }
            }
        }
        return 0;
    }
    if (msg == WM_BROWSER_CHANGE) {
        std::unique_ptr < Request > req(reinterpret_cast < Request *  > (lp));
        if (!req || req->items.empty()) return 0;
        wchar_t * end = nullptr;
        const unsigned long long val = wcstoull(req->items[0].c_str(), & end,10);
        if (!val || !end || * end) return 0;
        HWND anchor = reinterpret_cast < HWND > (static_cast < uintptr_t > (val));
        auto it = g_browsers.find(anchor);
        if (wp == ARCHIVE_BROKER_GRIP_CLEAR) {
            // A stale Unload for an earlier archive must not tear down a newly
            // attached browser if Windows reused the same child HWND.
            if (it != g_browsers.end() && !it->second->parked &&
                !_wcsicmp(it->second->archive.c_str(),req->archive.c_str())) {
                if (!BrowserPark(it)) BrowserDispose(it);
                Wh_Log(L"ArchivePreview: broker preview unloaded; controls parked when safe");
            }
            return 0;
        }
        if (wp == ARCHIVE_BROKER_BROWSER_BACK) {
            if (it != g_browsers.end() && !it->second->parked)
                BrowserBack(it->second.get());
            return 0;
        }
        if (wp == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO ||
            wp == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE) {
            if (it != g_browsers.end() && !it->second->parked &&
                _wcsicmp(it->second->archive.c_str(),req->archive.c_str()) == 0 &&
                ValidBrowserAnchor(anchor) && !it->second->retireWhenIdle)
                BrowserExtractSelected(it->second.get(),
                    wp == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE);
            return 0;
        }
        if (!ValidBrowserAnchor(anchor)) return 0;
        // An old GRIP_UPDATE may arrive after its newer replacement. When
        // another browser already owns this region, discard a positively
        // stale archive rather than reattaching it on top of the new one.
        bool overlap = false;
        for (const auto & [otherAnchor,other]:g_browsers) {
            if ((otherAnchor != anchor ||
                 _wcsicmp(other->archive.c_str(),req->archive.c_str()) != 0) &&
                BrowserOverlapsIncomingAnchor(other.get(),anchor)) {
                overlap = true;
                break;
            }
        }
        if (overlap) {
            BrowserState candidate{};
            candidate.parent = GetParent(anchor);
            candidate.archive = req->archive;
            if (SelectedItemInOwningExplorer( & candidate) == ExplorerSelection::OtherSelected) {
                Wh_Log(L"ArchivePreview: ignored stale archive attach after Explorer selection changed");
                return 0;
            }
        }
        // The map key can still reference a parked old HWND. The correct
        // owner is chosen below by Explorer-root, never by a stale HWND value.
        if (it != g_browsers.end() && it->second->parked) it = g_browsers.end();
        if (it != g_browsers.end() &&
            (_wcsicmp(it->second->archive.c_str(),req->archive.c_str()) != 0 ||
             !IsWindow(it->second->host) || GetParent(it->second->host) != GetParent(anchor))) {
            if (it->second->dragging) {
                it->second->retireWhenIdle = true;
                Wh_Log(L"Transfer cancellation diagnostic: preview attachment replaced while transfer was active");
                if (it->second->host && IsWindow(it->second->host))
                    ShowWindow(it->second->host,SW_HIDE);
                return 0;
            }
            if (!BrowserPark(it)) BrowserDispose(it);
            it = g_browsers.end();
        }
        BrowserRetireOverlappingPreviews(anchor);
        if (it == g_browsers.end()) {
            HWND newRoot = GetAncestor(GetParent(anchor),GA_ROOT);
            auto best = g_browsers.end();
            for (auto old = g_browsers.begin();old != g_browsers.end();++old) {
                auto * candidate = old->second.get();
                if (candidate->parked && candidate->parkedExplorerRoot == newRoot &&
                    GetTickCount64() - candidate->parkedSince <= kBrowserParkTtlMs &&
                    (best == g_browsers.end() ||
                     candidate->parkedSince > best->second->parkedSince)) best = old;
            }
            if (best != g_browsers.end()) {
                const auto duplicate = g_browsers.find(anchor);
                if (duplicate != g_browsers.end() && !duplicate->second->parked) {
                    // Never overwrite a live browser that won a rapid race.
                    return 0;
                }
                BrowserState check{};
                check.parent = GetParent(anchor);
                check.archive = req->archive;
                if (SelectedItemInOwningExplorer( & check) == ExplorerSelection::OtherSelected) {
                    Wh_Log(L"ArchivePreview: stale archive attach rejected during broker reuse");
                    return 0;
                }
                auto recycled = best->second;
                g_browsers.erase(best);
                // Windows may recycle the same anchor value while another
                // parked instance still has that map key. Never let a stale
                // parked entry defeat the new insertion or leak its HWNDs.
                auto staleDuplicate = g_browsers.find(anchor);
                if (staleDuplicate != g_browsers.end() && staleDuplicate->second->parked)
                    BrowserDispose(staleDuplicate);
                // No other active preview may already own the new anchor.
                auto inserted = g_browsers.emplace(anchor,recycled);
                if (inserted.second && BrowserAdopt(recycled.get(),anchor,req->archive))
                    return 0;
                if (inserted.second) BrowserDispose(inserted.first);
                Wh_Log(L"ArchivePreview: broker chrome recycle unavailable; creating fresh preview");
            }
            // A parked browser with a recycled HWND but a different Explorer
            // root cannot be adopted. Remove its map entry before creating a
            // new instance, otherwise emplace would fail and leave an orphan.
            auto stale = g_browsers.find(anchor);
            if (stale != g_browsers.end() && stale->second->parked)
                BrowserDispose(stale);
            auto item = std::make_shared < BrowserState > ();
            item->anchor = anchor;
            item->instanceId = g_nextBrowserId.fetch_add(1);
            item->archive = req->archive;
            auto * raw = item.get();
            // Publish the instance before starting a background model worker.
            // The manager can then match even a very fast result by instance ID.
            g_browsers.emplace(anchor,item);
            if (!CreateBrowser(raw)) {
                HWND parent = GetParent(anchor);
                if (parent) PostMessageW(parent,ARCHIVE_BROWSER_FAILED,0,0);
                const auto failed = g_browsers.find(anchor);
                if (failed != g_browsers.end()) BrowserDispose(failed);
                Wh_Log(L"Broker-owned ListView attach failed; native low-IL fallback retained");
                return 0;
            }
        } else {
            // A retried attach can refer to an already live browser. Re-send
            // readiness and depth instead of leaving prevhost in recovery mode.
            BrowserState * current = it->second.get();
            if (PositionBrowser(current)) {
                PostMessageW(current->parent,ARCHIVE_BROWSER_READY,current->dark?1:0,0);
                BrowserNotifyDepth(current);
                BrowserNotifySelectionCount(current);
            }
        }
        return 0;
    }
    if (msg == WM_BROWSER_REAP) {
        HWND anchor = reinterpret_cast < HWND > (wp);
        auto it = g_browsers.find(anchor);
        if (it != g_browsers.end() && !it->second->parked &&
            (lp == 1 || !ValidBrowserAnchor(anchor) || !IsWindow(it->second->host))) {
            if (it->second->dragging) {
                it->second->retireWhenIdle = true;
                Wh_Log(L"Transfer cancellation diagnostic: preview reaped while transfer was active");
                if (it->second->host && IsWindow(it->second->host))
                    ShowWindow(it->second->host,SW_HIDE);
            } else BrowserDispose(it);
        }
        return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
static void RunBrokerMessageLoop() {
    while (!CancelledForCurrentOperation()) {
        const DWORD wake = MsgWaitForMultipleObjectsEx(1,&g_brokerStopEvent,INFINITE,
            QS_ALLINPUT,MWMO_INPUTAVAILABLE);
        if (wake != WAIT_OBJECT_0 + 1) return;
        MSG message{};
        while (!CancelledForCurrentOperation() &&
               PeekMessageW(&message,nullptr,0,0,PM_REMOVE)) {
            if (message.message == WM_QUIT) return;
            TranslateMessage(&message);DispatchMessageW(&message);
        }
    }
}
template<typename T>
static void DiscardBrowserPayloads(HWND manager,UINT message) {
    MSG stranded{};
    while (PeekMessageW(&stranded,manager,message,message,PM_REMOVE)) {
        if (stranded.message == message)
            delete reinterpret_cast<T*>(stranded.lParam);
    }
}
static DWORD WINAPI GripUiThread(LPVOID) {
    if (CancelledForCurrentOperation()) return 0;
    HRESULT init = OleInitialize(nullptr);
    if (FAILED(init)) return 1;
    INITCOMMONCONTROLSEX cc{};cc.dwSize = sizeof(cc);cc.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx( & cc);
    // Use the Windhawk DLL instance, not explorer.exe, for all of our classes.
    // Never reuse an older copy's WndProc by accepting CLASS_ALREADY_EXISTS.
    if (!g_module) { OleUninitialize(); return 2; }
    const HINSTANCE instance = g_module;
    WNDCLASSEXW wc{};wc.cbSize = sizeof(wc);wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr,IDC_ARROW);
    const wchar_t * registered[4]{};
    size_t registeredCount = 0;
    auto addClass = [ & ](const wchar_t * name,WNDPROC proc) -> bool {
        wc.lpfnWndProc = proc;
        wc.lpszClassName = name;
        wc.style = 0;
        if (!RegisterClassExW( & wc)) return false;
        registered[registeredCount++] = name;
        return true;
    };
    const bool registeredAll =
        addClass(kBrowserHostClass,BrowserHostWindowProc) &&
        addClass(kBrowserHeaderClass,BrowserHeaderProc) &&
        addClass(kBrowserFooterClass,BrowserFooterProc) &&
        addClass(kBrowserManagerClass,BrowserManagerProc);
    if (!registeredAll) {
        const DWORD registerError = GetLastError();
        // Roll back our own successful registrations on any failure.
        while (registeredCount) UnregisterClassW(registered[--registeredCount],instance);
        Wh_Log(L"ArchivePreview: broker window class registration failed (Win32=%lu)",registerError);
        OleUninitialize();return 3;
    }
    HWND manager = CreateWindowExW(0,kBrowserManagerClass,L"",0,0,0,0,0,
        HWND_MESSAGE,nullptr,instance,nullptr);
    // A hidden broker-owned parent lets the ListView survive destruction of
    // the low-integrity Preview Handler HWND without becoming an orphan.
    g_browserParkHost = CreateWindowExW(WS_EX_TOOLWINDOW,L"STATIC",L"",
        WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        - 32000, - 32000,1,1,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    if (manager && !CancelledForCurrentOperation()) {
        g_gripManager.store(manager,std::memory_order_release);
        g_gripReady.store(true,std::memory_order_release);
    }
    Wh_Log(L"%s", std::wstring(manager?L"Broker-owned ListView manager ready":L"Broker-owned ListView manager failed").c_str());
    if (manager) RunBrokerMessageLoop();
    {
        std::lock_guard<std::mutex> lock(g_browserPostMutex);
        g_gripManager.store(nullptr,std::memory_order_release);
        g_gripReady.store(false,std::memory_order_release);
    }
    if (manager) {
        DiscardBrowserPayloads<BrowserModelJob>(manager,WM_BROWSER_MODEL_READY);
        DiscardBrowserPayloads<BrowserModelPreview>(manager,WM_BROWSER_MODEL_PREVIEW);
        DiscardBrowserPayloads<Request>(manager,WM_BROWSER_CHANGE);
        DiscardBrowserPayloads<BrowserLocalExtractResult>(manager,WM_BROWSER_LOCAL_EXTRACT_DONE);
    }
    // No new browser may subscribe after manager shutdown. The worker's own
    // token remains valid until it returns; this map holds no thread handles.
    for (auto & work:g_browserModelInflight)
        work.ticket->store(true,std::memory_order_release);
    g_browserModelInflight.clear();
    // Windhawk can unload this module while Explorer remains alive. Destroy
    // every broker-owned HWND before allowing the module to be released.
    while(!g_browsers.empty()) BrowserDispose(g_browsers.begin());
    if (g_browserParkHost && IsWindow(g_browserParkHost)) DestroyWindow(g_browserParkHost);
    g_browserParkHost = nullptr;
    if(manager && IsWindow(manager)) DestroyWindow(manager);
    // Model workers are globally tracked. The broker UI may exit first,
    // but Wh_ModUninit must still join every model worker before DLL unload.
    const ULONGLONG modelsStopStarted = GetTickCount64();
    while(g_activeModelWorkers.load(std::memory_order_acquire) != 0 &&
          GetTickCount64() - modelsStopStarted < 3000) Sleep(80);
    if (g_activeModelWorkers.load(std::memory_order_acquire) != 0)
        Wh_Log(L"ArchivePreview: model workers still active; shutdown join will wait for them");
    // All broker-owned windows have been destroyed before these calls.
    UnregisterClassW(kBrowserManagerClass,instance);
    UnregisterClassW(kBrowserHeaderClass,instance);
    UnregisterClassW(kBrowserFooterClass,instance);
    UnregisterClassW(kBrowserHostClass,instance);
    while (g_externalDragInterfaces.load(std::memory_order_acquire)) {
        MSG pending{};
        while (PeekMessageW(&pending,nullptr,0,0,PM_REMOVE)) {
            if (pending.message == WM_QUIT) continue;
            TranslateMessage(&pending);DispatchMessageW(&pending);
        }
        if (g_externalDragInterfaces.load(std::memory_order_acquire))
            MsgWaitForMultipleObjects(0,nullptr,FALSE,100,QS_ALLINPUT);
    }
    OleUninitialize();return 0;
}
static HRESULT DispatchGripUpdate(const Request & req) {
    std::unique_ptr<Request> copy(new(std::nothrow) Request(req));
    if (!copy) return E_OUTOFMEMORY;
    return PostBrowserPayload(WM_BROWSER_CHANGE,req.command,std::move(copy));
}

static void Report(const Request & req,HRESULT hr) {
    if (!ArchivePreviewDiagnostics::Enabled()) return;
    const wchar_t * operation =
        req.command == ARCHIVE_BROKER_COPY_SELECTION ? L"Copy selection" :
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ? L"Extract selected to" :
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE ? L"Extract selected here" :
        req.command == ARCHIVE_BROKER_OPEN_FILE ? L"Open archive file" :
        req.command == ARCHIVE_BROKER_EXTRACT_TO ? L"Extract to" : L"Extract here";
    wchar_t guidText[48]{};
    StringFromGUID2(req.requestId,guidText,_countof(guidText));
    Wh_Log(L"[AP broker] operation=%s hr=0x%08lX request=%s items=%zu",
        operation,static_cast<unsigned long>(hr),guidText,req.items.size());
    // Do not display MessageBox here. Process() runs on a worker apartment:
    // a modal dialog stalls the queue and suppresses the completion result.
    // The handler reads the final HRESULT by request GUID.

}



static HRESULT Process(const Request & req) {
    HRESULT hr = E_INVALIDARG;
    if (req.command == ARCHIVE_BROKER_COPY_SELECTION)
        hr = NativeShellClipboard(req,nullptr);
    else if (req.command == ARCHIVE_BROKER_OPEN_FILE)
        hr = NativeShellOpen(req);
    else if (req.command == ARCHIVE_BROKER_EXTRACT_HERE ||
             req.command == ARCHIVE_BROKER_EXTRACT_TO ||
             req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE ||
             req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO) {
        const auto options = ArchivePreviewOptions::ExtractionSnapshot();
        hr = options.backend == 0?NativeShellExtract(req):ExternalShellExtract(req,options);
    }
    else return E_INVALIDARG;
    Report(req, hr);
    if (req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE ||
        req.command == ARCHIVE_BROKER_OPEN_FILE) {
        const GUID zero{};
        if (!InlineIsEqualGUID(req.requestId,zero)) {
            std::unique_ptr<BrowserLocalExtractResult> result(
                new(std::nothrow) BrowserLocalExtractResult{req.requestId,hr,req.command});
            if (result) PostBrowserPayload(WM_BROWSER_LOCAL_EXTRACT_DONE,0,std::move(result));
        }
    }
    if (req.command == ARCHIVE_BROKER_EXTRACT_HERE ||
        req.command == ARCHIVE_BROKER_EXTRACT_TO ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
        req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE) {
        HWND manager = g_gripManager.load();
        if (manager && IsWindow(manager))
            PostMessageW(manager,WM_BROWSER_HEADER_REFRESH,
                         BrowserArchiveKey(req.archive),0);
    }
    return hr;
}
// Two persistent STA workers process file operations without creating a thread
// for each click. Only explicit, on-demand operations enter the bounded FIFO.
static std::mutex g_workMutex;
// Manual-reset work signal also lets OLE STAs dispatch clipboard COM calls.
static HANDLE g_workEvent = nullptr;
static std::deque < Request > g_workQueue;
// The OLE clipboard provider needs its own apartment. The two extraction
// workers may be inside long Shell calls and cannot pump clipboard messages.
// Both event handles are published, signalled and closed under g_workMutex.
static HANDLE g_clipboardEvent = nullptr;
static HANDLE g_clipboardThread = nullptr;
static std::atomic<bool> g_clipboardReady{false};
static std::deque<Request> g_clipboardQueue;
static bool g_clipboardBusy = false; // guarded by g_workMutex
static std::atomic < bool > g_stopWorkers{false};
// A completed operation can contain thousands of archive paths. A stable
// SHA-256 fingerprint bounds retry records without retaining those arrays.
using BrokerRequestHash = std::array < BYTE,32 > ;
static bool HashBrokerRequest(const Request & req,BrokerRequestHash & result) {
    HCRYPTPROV provider = 0;
    if (!CryptAcquireContextW( & provider,nullptr,nullptr,PROV_RSA_AES,
            CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) return false;
    HCRYPTHASH hash = 0;
    if (!CryptCreateHash(provider,CALG_SHA_256,0,0, & hash)) {
        CryptReleaseContext(provider,0);
        return false;
    }
    // Domain-separated and length-framed: paths with different boundaries
    // cannot hash like the same concatenation of characters.
    const DWORD tag = 0x37375041; // APP7
    const ULONGLONG itemCount = static_cast < ULONGLONG > (req.items.size());
    auto chunk = [ & ](const void * p,size_t count)->bool {
        return count <= std::numeric_limits < DWORD > ::max() &&
            CryptHashData(hash,reinterpret_cast < const BYTE *  > (p),
                static_cast < DWORD > (count),0) != FALSE;
    };
    auto stringChunk = [ & ](const std::wstring & value)->bool {
        const ULONGLONG chars = static_cast < ULONGLONG > (value.size());
        return chunk( & chars,sizeof(chars)) && chunk(value.data(),value.size() * sizeof(wchar_t));
    };
    bool ok = chunk( & tag,sizeof(tag)) && chunk( & req.command,sizeof(req.command)) &&
        stringChunk(req.archive) && chunk( & itemCount,sizeof(itemCount));
    for(const auto & item:req.items) {
        if(!ok)break;
        ok = stringChunk(item);
    }
    DWORD bytes = static_cast < DWORD > (result.size());
    if(ok)ok = CryptGetHashParam(hash,HP_HASHVAL,result.data(), & bytes,0) != FALSE &&
              bytes == result.size();
    CryptDestroyHash(hash);
    CryptReleaseContext(provider,0);
    return ok;
}
struct BrokerOperationRecord {
    DWORD phase{ARCHIVE_OPERATION_QUEUED};
    HRESULT result{S_OK};
    BrokerRequestHash fingerprint{};
    ULONGLONG updated{};
    DWORD progressStage{ARCHIVE_PROGRESS_NONE};
    DWORD progressPercent{};
    DWORD progressKnown{};
    ULONGLONG workStarted{}; // first actual scan/extraction; excludes picker/queue
    GUID requestId{};
    bool isExtraction{};
    DWORD ownerPid{};           // set for low-IL requests only
    std::wstring archive;       // archive-bound cancellation authorization
    std::shared_ptr < std::atomic < bool >> cancelled;
};
static std::unordered_map < std::wstring,BrokerOperationRecord > g_operationRecords;
// Read a small, fixed-size snapshot; never enumerate the archive or block on COM
// from the broker UI thread. Multiple previews cannot cancel each other's IDs.
static BrowserProgressSnapshot BrowserQueryProgress(const std::wstring & archive,DWORD ownerPid) {
    BrowserProgressSnapshot view{};
    if (!ownerPid || archive.empty()) return view;
    std::lock_guard < std::mutex > lock(g_workMutex);
    ULONGLONG latest = 0;
    for (const auto & entry:g_operationRecords) {
        const auto & record = entry.second;
        if (!record.isExtraction || record.phase == ARCHIVE_OPERATION_DONE ||
            record.ownerPid != ownerPid ||
            _wcsicmp(record.archive.c_str(),archive.c_str()) != 0) continue;
        if (view.active && record.updated < latest) continue;
        latest = record.updated;
        view.active = true;
        view.stage = record.progressStage;
        view.percent = record.progressPercent;
        view.known = record.progressKnown != 0;
        view.cancelling = record.cancelled && record.cancelled->load(std::memory_order_acquire);
        view.workStarted = record.workStarted;
        view.requestId = record.requestId;
    }
    return view;
}
static std::wstring BrokerRequestKey(const GUID & id) {
    const GUID zero{};
    if (InlineIsEqualGUID(id,zero)) return {};
    wchar_t buffer[48]{};
    return StringFromGUID2(id,buffer,_countof(buffer)) ? buffer : L"";
}

static bool NativeProgressRecord(const GUID & id,DWORD stage,
                                 bool known,DWORD percent,bool begin,
                                 bool updateEstimate) {
    const std::wstring key = BrokerRequestKey(id);
    if (key.empty()) return stage != ARCHIVE_PROGRESS_EXTERNAL || !CancelledForCurrentOperation();
    const ULONGLONG now = GetTickCount64();
    std::lock_guard < std::mutex > lock(g_workMutex);
    if (stage == ARCHIVE_PROGRESS_EXTERNAL && CancelledForCurrentOperation()) return false;
    const auto it = g_operationRecords.find(key);
    if (it == g_operationRecords.end() || !it->second.isExtraction ||
        it->second.phase != ARCHIVE_OPERATION_RUNNING) return true;
    auto & record = it->second;
    if (begin && !record.workStarted) record.workStarted = now;
    if (stage != ARCHIVE_PROGRESS_NONE) record.progressStage = stage;
    if (updateEstimate || begin) {
        record.progressKnown = known?1:0;
        record.progressPercent = known?std::min < DWORD > (percent,100):0;
    }
    // The Shell returns work units (not bytes). Update a worker heartbeat
    // only when an actual provider callback or a stage transition arrives.
    record.updated = now;
    if (t_workerHeartbeat && t_workerProgressStage) {
        t_workerProgressStage->store(record.progressStage,std::memory_order_release);
        t_workerHeartbeat->store(now,std::memory_order_release);
    }
    return true;
}

static ArchiveBrokerOperationStatusReply QueryOperationStatus(const GUID & id,DWORD callerPid) {
    const std::wstring key = BrokerRequestKey(id);
    const std::lock_guard < std::mutex > guard(g_workMutex);
    const auto found = g_operationRecords.find(key);
    if (key.empty() || found == g_operationRecords.end() || !callerPid ||
        found->second.ownerPid != callerPid)
        return {HRESULT_FROM_WIN32(ERROR_NOT_FOUND),E_FAIL,ARCHIVE_OPERATION_UNKNOWN,
                ARCHIVE_PROGRESS_NONE,0,0,0,0};
    const auto & record = found->second;
    return {S_OK,record.result,record.phase,record.progressStage,
            record.progressPercent,record.progressKnown,0,0};
}
// Fingerprints make a larger completion-history limit inexpensive; this reduces
// the chance of evicting an unpolled completion during a busy extraction batch.
static constexpr size_t kMaxTrackedOperations = 1024;
static constexpr size_t kMaxQueuedOperations = 16;
static constexpr ULONGLONG kMaxQueuedPayloadBytes = 256ULL * 1024 * 1024;
static ULONGLONG QueuedPayloadBytes(const Request & req) {
    ULONGLONG size = sizeof(Request) + req.archive.size() * sizeof(wchar_t) +
        static_cast < ULONGLONG > (req.items.size()) * sizeof(std::wstring);
    for(const auto & entry:req.items)
        size += entry.size() * sizeof(wchar_t);
    return size;
}
static bool HasWorkQueueSpace(const Request & incoming) {
    if(g_workQueue.size() >= kMaxQueuedOperations)return false;
    const ULONGLONG added = QueuedPayloadBytes(incoming);
    if(added > kMaxQueuedPayloadBytes)return false;
    ULONGLONG used = 0;
    for(const auto & req:g_workQueue) {
        const ULONGLONG size = QueuedPayloadBytes(req);
        if(size > kMaxQueuedPayloadBytes - used)return false;
        used += size;
    }
    return added <= kMaxQueuedPayloadBytes - used;
}
static HRESULT CancelTrackedExtraction(const Request & req,DWORD clientPid) {
    if (!clientPid || !req.items.empty()) return E_ACCESSDENIED;
    const std::wstring key = BrokerRequestKey(req.requestId);
    if (key.empty()) return E_INVALIDARG;
    std::lock_guard < std::mutex > lock(g_workMutex);
    const auto it = g_operationRecords.find(key);
    if (it == g_operationRecords.end() || !it->second.isExtraction ||
        it->second.ownerPid != clientPid ||
        _wcsicmp(it->second.archive.c_str(),req.archive.c_str()) != 0 ||
        !it->second.cancelled) return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    if (it->second.phase == ARCHIVE_OPERATION_DONE) return S_OK;
    if (it->second.progressStage == ARCHIVE_PROGRESS_EXTERNAL)
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    // Only extraction requests from the live Preview Pane may be cancelled.
    if (it->second.phase == ARCHIVE_OPERATION_QUEUED) {
        for (auto pending = g_workQueue.begin();pending != g_workQueue.end();++pending) {
            if (InlineIsEqualGUID(pending->requestId,req.requestId)) {
                if (pending->command != ARCHIVE_BROKER_EXTRACT_HERE &&
                    pending->command != ARCHIVE_BROKER_EXTRACT_TO &&
                    pending->command != ARCHIVE_BROKER_EXTRACT_SELECTED_TO &&
                    pending->command != ARCHIVE_BROKER_EXTRACT_SELECTED_HERE)
                    return E_ACCESSDENIED;
                g_workQueue.erase(pending);
                it->second.cancelled->store(true,std::memory_order_release);
                it->second.result = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                it->second.phase = ARCHIVE_OPERATION_DONE;
                it->second.updated = GetTickCount64();
                return S_OK;
            }
        }
        // Worker removed the queue entry and is transitioning to RUNNING.
        // Signal it regardless; it checks before entering Process().
    }
    it->second.cancelled->store(true,std::memory_order_release);
    return S_OK;
}
static constexpr int kWorkerCount = 2;
static HANDLE g_workerThreads[kWorkerCount]{};
static std::atomic < ULONGLONG > g_workerStarted[kWorkerCount]{};
static std::atomic < ULONGLONG > g_workerProgressHeartbeat[kWorkerCount]{};
static std::atomic < DWORD > g_workerProgressStage[kWorkerCount]{};
static std::atomic < bool > g_loggedProviderStall{false};
static constexpr ULONGLONG kProviderStallMs = 10ULL * 60 * 1000;
// This gate is for exhausted worker capacity, not an individual slow file.
// One stalled archive must not prevent the other STA worker from accepting
// independent requests. Native callbacks refresh the per-worker heartbeat.
static bool ProviderStalled() {
    const ULONGLONG now = GetTickCount64();
    unsigned stalledWorkers = 0;
    for (size_t index = 0;index < kWorkerCount;++index) {
        const ULONGLONG begin = g_workerStarted[index].load(std::memory_order_acquire);
        const ULONGLONG heartbeat = g_workerProgressHeartbeat[index].load(std::memory_order_acquire);
        const DWORD stage = g_workerProgressStage[index].load(std::memory_order_acquire);
        if (!begin || stage == ARCHIVE_PROGRESS_PICKING || stage == ARCHIVE_PROGRESS_EXTERNAL) continue;
        const ULONGLONG activity = std::max(begin,heartbeat);
        if (now >= activity && now - activity > kProviderStallMs) ++stalledWorkers;
    }
    const bool exhausted = stalledWorkers == kWorkerCount;
    if (exhausted) {
        if (!g_loggedProviderStall.exchange(true,std::memory_order_acq_rel))
            Wh_Log(L"ArchivePreview: watchdog: both Shell workers have had no callbacks for 10 minutes; deferring new work");
    } else {
        g_loggedProviderStall.store(false,std::memory_order_release);
    }
    return exhausted;
}
static DWORD WINAPI WorkQueueThread(LPVOID raw) {
    const size_t workerIndex = static_cast < size_t > (reinterpret_cast < uintptr_t > (raw));
    const HRESULT initialized = OleInitialize(nullptr);
    if (FAILED(initialized)) { Wh_Log(L"Worker STA initialization failed"); return 1; }
    for (;;) {
        Request req;
        std::shared_ptr < std::atomic < bool >> operationCancel;
        {
            std::unique_lock < std::mutex > lock(g_workMutex);
            // An STA that owns a virtual-file OLE clipboard object must pump
            // COM window messages even while no broker operation is queued.
            // The manual-reset event avoids periodic wakeups while idle.
            while (!g_stopWorkers.load() && g_workQueue.empty()) {
                ResetEvent(g_workEvent); // Protected by g_workMutex: no lost wake.
                lock.unlock();
                const DWORD wake = MsgWaitForMultipleObjects(
                    1, &g_workEvent, FALSE, INFINITE, QS_ALLINPUT);
                if (wake == WAIT_OBJECT_0 + 1) {
                    MSG message{};
                    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&message);
                        DispatchMessageW(&message);
                    }
                } else if (wake == WAIT_FAILED) {
                    Sleep(50);
                }
                lock.lock();
            }
            if (g_stopWorkers.load() && g_workQueue.empty()) break;
            req = std::move(g_workQueue.front());
            g_workQueue.pop_front();
            g_activeQueueOperations.fetch_add(1,std::memory_order_release);
            const auto key = BrokerRequestKey(req.requestId);
            const auto it = g_operationRecords.find(key);
            if (!key.empty() && it != g_operationRecords.end()) {
                it->second.phase = ARCHIVE_OPERATION_RUNNING;
                it->second.updated = GetTickCount64();
                operationCancel = it->second.cancelled;
            }
        }
        const bool isExtraction = req.command == ARCHIVE_BROKER_EXTRACT_HERE ||
            req.command == ARCHIVE_BROKER_EXTRACT_TO ||
            req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
            req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE;
        t_workerHeartbeat =  & g_workerProgressHeartbeat[workerIndex];
        t_workerProgressStage =  & g_workerProgressStage[workerIndex];
        g_workerProgressHeartbeat[workerIndex].store(0,std::memory_order_release);
        g_workerProgressStage[workerIndex].store(isExtraction ? ARCHIVE_PROGRESS_STARTING : ARCHIVE_PROGRESS_NONE,
                                                  std::memory_order_release);
        g_workerStarted[workerIndex].store(GetTickCount64(),std::memory_order_release);
        t_operationCancelFlag = operationCancel.get();
        const HRESULT completed = CancelledForCurrentOperation() ?
            HRESULT_FROM_WIN32(ERROR_CANCELLED) : Process(req);
        t_operationCancelFlag = nullptr;
        t_workerHeartbeat = nullptr;
        t_workerProgressStage = nullptr;
        g_workerStarted[workerIndex].store(0,std::memory_order_release);
        g_workerProgressHeartbeat[workerIndex].store(0,std::memory_order_release);
        g_workerProgressStage[workerIndex].store(ARCHIVE_PROGRESS_NONE,std::memory_order_release);
        // No archive-wide completion notifications: multiple previews may
        // display the same ZIP. The originating handler queries by GUID.
        const auto key = BrokerRequestKey(req.requestId);
        if (!key.empty()) {
            std::lock_guard < std::mutex > lock(g_workMutex);
            const auto it = g_operationRecords.find(key);
            if (it != g_operationRecords.end()) {
                it->second.phase = ARCHIVE_OPERATION_DONE;
                it->second.result = completed;
                it->second.updated = GetTickCount64();
            }
        }
        g_activeQueueOperations.fetch_sub(1,std::memory_order_release);
    }
    OleUninitialize();
    return 0;
}
// Dedicated OLE clipboard STA: never run extractions on this apartment.
static DWORD WINAPI ClipboardQueueThread(LPVOID) {
    const HRESULT initialized = OleInitialize(nullptr);
    if (FAILED(initialized)) {
        Wh_Log(L"ArchivePreview: clipboard STA initialization failed");
        return 1;
    }
    g_clipboardReady.store(true, std::memory_order_release);
    for (;;) {
        Request req;
        std::shared_ptr<std::atomic<bool>> cancellation;
        {
            std::unique_lock<std::mutex> lock(g_workMutex);
            while (!g_stopWorkers.load(std::memory_order_acquire) &&
                   g_clipboardQueue.empty()) {
                ResetEvent(g_clipboardEvent);
                lock.unlock();
                const DWORD wake = MsgWaitForMultipleObjects(
                    1, &g_clipboardEvent, FALSE, INFINITE, QS_ALLINPUT);
                if (wake == WAIT_OBJECT_0 + 1) {
                    MSG msg{};
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                } else if (wake == WAIT_FAILED) {
                    Sleep(50);
                }
                lock.lock();
            }
            if (g_stopWorkers.load(std::memory_order_acquire) &&
                g_clipboardQueue.empty()) break;
            req = std::move(g_clipboardQueue.front());
            g_clipboardQueue.pop_front();
            g_clipboardBusy = true;
            g_activeQueueOperations.fetch_add(1, std::memory_order_release);
            const auto key = BrokerRequestKey(req.requestId);
            const auto it = g_operationRecords.find(key);
            if (!key.empty() && it != g_operationRecords.end()) {
                it->second.phase = ARCHIVE_OPERATION_RUNNING;
                it->second.updated = GetTickCount64();
                cancellation = it->second.cancelled;
            }
        }
        t_operationCancelFlag = cancellation.get();
        const HRESULT completed = CancelledForCurrentOperation() ?
            HRESULT_FROM_WIN32(ERROR_CANCELLED) : Process(req);
        t_operationCancelFlag = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_workMutex);
            const auto key = BrokerRequestKey(req.requestId);
            const auto it = g_operationRecords.find(key);
            if (!key.empty() && it != g_operationRecords.end()) {
                it->second.phase = ARCHIVE_OPERATION_DONE;
                it->second.result = completed;
                it->second.updated = GetTickCount64();
            }
            g_clipboardBusy = false;
        }
        g_activeQueueOperations.fetch_sub(1, std::memory_order_release);
    }
    g_clipboardReady.store(false, std::memory_order_release);
    OleUninitialize();
    return 0;
}
static bool StartWorkQueue() {
    // StopService may run concurrently with broker startup. Publish both
    // handles under the same mutex that guards every SetEvent and CloseHandle.
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_workEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_workEvent) return false;
        g_clipboardEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_clipboardEvent) {
            CloseHandle(g_workEvent);
            g_workEvent = nullptr;
            return false;
        }
    }
    g_clipboardReady.store(false, std::memory_order_release);
    for (int i = 0;i < kWorkerCount;++i) {
        g_workerThreads[i] = CreateThread(nullptr,0,WorkQueueThread,
            reinterpret_cast < LPVOID > (static_cast < uintptr_t > (i)),0,nullptr);
        if (!g_workerThreads[i]) {
            g_stopWorkers.store(true);
            {
                std::lock_guard<std::mutex> lock(g_workMutex);
                SetEvent(g_workEvent);
                SetEvent(g_clipboardEvent);
            }
            for (int j = 0;j < i;++j) {
                // Keep each STA handle until it has joined. Closing a live
                // handle loses the reliable join point during DLL unload.
                while (WaitForSingleObject(g_workerThreads[j],30000) == WAIT_TIMEOUT)
                    Wh_Log(L"ArchivePreview: waiting for STA worker after partial startup failure");
                CloseHandle(g_workerThreads[j]);
                g_workerThreads[j] = nullptr;
            }
            {
                std::lock_guard<std::mutex> lock(g_workMutex);
                CloseHandle(g_clipboardEvent);
                g_clipboardEvent = nullptr;
                CloseHandle(g_workEvent);
                g_workEvent = nullptr;
            }
            return false;
        }
    }
    g_clipboardThread = CreateThread(nullptr, 0, ClipboardQueueThread,
                                     nullptr, 0, nullptr);
    if (g_clipboardThread) {
        // Wait on the broker service thread, never an Explorer UI thread.
        // A created Win32 thread is not proof its OLE apartment started.
        for (int attempt = 0; attempt < 200 &&
             !g_clipboardReady.load(std::memory_order_acquire); ++attempt) {
            if (WaitForSingleObject(g_clipboardThread, 0) == WAIT_OBJECT_0) break;
            Sleep(5);
        }
    }
    if (!g_clipboardThread ||
        !g_clipboardReady.load(std::memory_order_acquire)) {
        Wh_Log(L"ArchivePreview: clipboard worker unavailable");
        g_stopWorkers.store(true, std::memory_order_release);
        {
            std::lock_guard<std::mutex> lock(g_workMutex);
            SetEvent(g_workEvent);
            SetEvent(g_clipboardEvent);
        }
        for (HANDLE& worker : g_workerThreads) {
            if (!worker) continue;
            while (WaitForSingleObject(worker, 30000) == WAIT_TIMEOUT)
                Wh_Log(L"ArchivePreview: waiting for STA worker after clipboard startup failure");
            CloseHandle(worker);
            worker = nullptr;
        }
        if (g_clipboardThread) {
            while (WaitForSingleObject(g_clipboardThread, 30000) == WAIT_TIMEOUT)
                Wh_Log(L"ArchivePreview: waiting for failed clipboard worker");
            CloseHandle(g_clipboardThread);
            g_clipboardThread = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(g_workMutex);
            CloseHandle(g_clipboardEvent);
            g_clipboardEvent = nullptr;
            CloseHandle(g_workEvent);
            g_workEvent = nullptr;
        }
        return false;
    }
    return true;
}
static HRESULT DispatchAsync(const Request & req,DWORD ownerPid) {
    if (g_cancelForShutdown.load(std::memory_order_acquire) || g_stopWorkers.load())
        return HRESULT_FROM_WIN32(ERROR_SHUTDOWN_IN_PROGRESS);
    // Preserve strict admission: a stalled provider cannot accept new work.
    if (ProviderStalled()) return HRESULT_FROM_WIN32(ERROR_BUSY);
    // Compute this outside g_workMutex. Hashing a 64-MiB request must not block
    // other workers completing status updates or admitting small commands.
    const auto id = BrokerRequestKey(req.requestId);
    BrokerRequestHash fingerprint{};
    if(!id.empty() && !HashBrokerRequest(req,fingerprint)) return E_FAIL;
    {
        std::lock_guard < std::mutex > lock(g_workMutex);
        if (g_stopWorkers.load()) return HRESULT_FROM_WIN32(ERROR_SHUTDOWN_IN_PROGRESS);
        if (!id.empty()) {
            const auto found = g_operationRecords.find(id);
            if (found != g_operationRecords.end()) {
                // A matching request ID is not permission to reuse another
                // Preview Host's queued or completed operation.
                if (found->second.ownerPid != ownerPid) return E_ACCESSDENIED;
                if (found->second.fingerprint != fingerprint) return E_INVALIDARG;
                return S_OK; // lost reply to this client's same operation
            }
        }
        // Clipboard work must never wait behind large extractions. Its own
        // STA must remain free to serve deferred IDataObject COM requests.
        const bool clipboard = req.command == ARCHIVE_BROKER_COPY_SELECTION;
        if (clipboard ? (g_clipboardBusy || !g_clipboardQueue.empty() ||
                         !g_clipboardEvent ||
                         !g_clipboardReady.load(std::memory_order_acquire))
                      : !HasWorkQueueSpace(req))
            return HRESULT_FROM_WIN32(ERROR_BUSY);
        (clipboard ? g_clipboardQueue : g_workQueue).push_back(req);
        if (!id.empty()) {
            // Only finished records may be aged out; pending and running
            // records must remain deduplicated for their entire lifetime.
            if (g_operationRecords.size() >= kMaxTrackedOperations) {
                auto oldest = g_operationRecords.end();
                for (auto it = g_operationRecords.begin();it != g_operationRecords.end();++it)
                    if (it->second.phase == ARCHIVE_OPERATION_DONE &&
                        (oldest == g_operationRecords.end() ||
                         it->second.updated < oldest->second.updated)) oldest = it;
                if (oldest != g_operationRecords.end()) g_operationRecords.erase(oldest);
                else {
                    // Cannot evict an in-flight id. Remove only our newly queued task.
                    auto& queue = clipboard ? g_clipboardQueue : g_workQueue;
                    for (auto it = queue.begin(); it != queue.end(); ++it)
                        if (InlineIsEqualGUID(it->requestId, req.requestId)) {
                            queue.erase(it);
                            break;
                        }
                    return HRESULT_FROM_WIN32(ERROR_BUSY);
                }
            }
            BrokerOperationRecord record{};
            record.phase = ARCHIVE_OPERATION_QUEUED;
            record.fingerprint = fingerprint;
            record.updated = GetTickCount64();
            record.requestId = req.requestId;
            record.isExtraction = req.command == ARCHIVE_BROKER_EXTRACT_HERE ||
                req.command == ARCHIVE_BROKER_EXTRACT_TO ||
                req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_TO ||
            req.command == ARCHIVE_BROKER_EXTRACT_SELECTED_HERE;
            record.ownerPid = ownerPid;
            record.archive = req.archive;
            record.cancelled = std::make_shared < std::atomic < bool >> (false);
            g_operationRecords.emplace(id,std::move(record));
        }
        // Signal while holding the same mutex used to close both handles.
        HANDLE signal = clipboard ? g_clipboardEvent : g_workEvent;
        if (signal) SetEvent(signal);
    }
    return S_OK;
}

static void CancelTimedOutClipboard(const GUID& id, DWORD ownerPid) {
    const std::wstring key = BrokerRequestKey(id);
    if (key.empty() || !ownerPid) return;
    std::lock_guard<std::mutex> lock(g_workMutex);
    auto it = g_operationRecords.find(key);
    if (it == g_operationRecords.end() ||
        it->second.ownerPid != ownerPid ||
        it->second.phase == ARCHIVE_OPERATION_DONE) return;
    if (it->second.cancelled)
        it->second.cancelled->store(true, std::memory_order_release);
    if (it->second.phase != ARCHIVE_OPERATION_QUEUED) return;
    for (auto pending = g_clipboardQueue.begin();
         pending != g_clipboardQueue.end(); ++pending) {
        if (InlineIsEqualGUID(pending->requestId, id)) {
            g_clipboardQueue.erase(pending);
            it->second.result = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
            it->second.phase = ARCHIVE_OPERATION_DONE;
            it->second.updated = GetTickCount64();
            return;
        }
    }
}

// A per-user named-pipe ACL alone does not identify the Preview Handler: any
// process under the same account can connect. Verify the client token/session
// and the location of its executable before accepting privileged commands.
// Does not disable or elevate the Preview Handler isolation.
static bool IsSameSessionUser(HANDLE client) {
    DWORD currentSession = 0,clientSession = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), & currentSession) ||
        !ProcessIdToSessionId(GetProcessId(client), & clientSession) ||
        currentSession != clientSession) return false;
    HANDLE localToken = nullptr,peerToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY, & localToken)) return false;
    if (!OpenProcessToken(client,TOKEN_QUERY, & peerToken)) {CloseHandle(localToken);return false;}
    auto tokenSid = [](HANDLE token,std::vector < BYTE >  & data)->PSID {
        DWORD need = 0;
        GetTokenInformation(token,TokenUser,nullptr,0, & need);
        if (!need || need > 16384) return nullptr;
        data.resize(need);
        return GetTokenInformation(token,TokenUser,data.data(),need, & need)
            ? reinterpret_cast < TOKEN_USER *  > (data.data())->User.Sid : nullptr;
    };
    std::vector < BYTE > a,b;
    PSID as = tokenSid(localToken,a),bs = tokenSid(peerToken,b);
    const bool match = as && bs && EqualSid(as,bs);
    CloseHandle(peerToken);CloseHandle(localToken);
    return match;
}
static bool VerifyBrokerClient(HANDLE pipe) {
    ULONG pid = 0;
    if (!GetNamedPipeClientProcessId(pipe, & pid) || !pid) return false;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if (!process) return false;
    bool permitted = false;
    if (IsSameSessionUser(process)) {
        // Size the path buffer dynamically. QueryFullProcessImageNameW uses
        // the supplied character count and reports insufficient capacity.
        std::vector<wchar_t> path(1024);
        DWORD chars = 0;
        for (;;) {
            chars = static_cast<DWORD>(path.size());
            if (QueryFullProcessImageNameW(process,0,path.data(),&chars)) break;
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || path.size() >= 32768) {
                chars = 0;
                break;
            }
            path.resize(std::min<size_t>(32768,path.size()*2));
        }
        if (chars != 0) {
            const std::wstring imagePath(path.data(),chars);
            wchar_t winDir[MAX_PATH]{};
            const UINT winCount = GetWindowsDirectoryW(winDir,_countof(winDir));
            const std::wstring prefix = winCount > 0 && winCount < _countof(winDir)
                ? std::wstring(winDir) + L"\\" : std::wstring();
            // A same-user process is not sufficient: require the actual
            // Windows Preview Host path, or this broker's own process ID.
            const bool windowsProcess = !prefix.empty() &&
                _wcsicmp(imagePath.c_str(),
                         (prefix + L"System32\\prevhost.exe").c_str()) == 0;
            permitted = windowsProcess || pid == GetCurrentProcessId();
        }
    }
    CloseHandle(process);
    return permitted;
}

// R5: Only use an Explorer selection, never a filename search. Prefer the
// Explorer window containing the caller's preview host, and refuse ambiguous
// matches. This runs on the broker's already-initialized IPC STA.
static HRESULT ResolveSelectedExplorerArchive(HWND preview,const std::wstring & leaf,
                                              std::wstring & resolved) {
    resolved.clear();
    HWND owningWindow = GetAncestor(preview,GA_ROOT);
    if(!owningWindow || !IsWindow(owningWindow))return E_INVALIDARG;
    IShellWindows * windows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_LOCAL_SERVER,
                                IID_PPV_ARGS( & windows));
    if(FAILED(hr) || !windows)return FAILED(hr)?hr:E_FAIL;
    long count = 0;
    hr = windows->get_Count( & count);
    if(FAILED(hr)){windows->Release();return hr;}
    std::wstring ownerMatch,globalMatch;
    unsigned ownerCount = 0,globalCount = 0;
    for(long i = 0;i < count && i < 64;i++) {
        VARIANT index{};V_VT( & index) = VT_I4;V_I4( & index) = i;
        IDispatch * dispatch = nullptr;
        if(FAILED(windows->Item(index, & dispatch)) || !dispatch)continue;
        IWebBrowser2 * browser = nullptr;
        if(SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS( & browser))) && browser){
            SHANDLE_PTR browserHandle = 0;
            const bool sameWindow = SUCCEEDED(browser->get_HWND( & browserHandle)) &&
                GetAncestor(reinterpret_cast < HWND > (static_cast < INT_PTR > (browserHandle)),
                            GA_ROOT) == owningWindow;
            IDispatch * document = nullptr;
            if(SUCCEEDED(browser->get_Document( & document)) && document){
                IShellFolderViewDual * view = nullptr;
                if(SUCCEEDED(document->QueryInterface(IID_PPV_ARGS( & view))) && view){
                    FolderItems * selected = nullptr;
                    if(SUCCEEDED(view->SelectedItems( & selected)) && selected){
                        long n = 0;
                        if(SUCCEEDED(selected->get_Count( & n)) && n == 1){
                            VARIANT zero{};V_VT( & zero) = VT_I4;V_I4( & zero) = 0;
                            FolderItem * item = nullptr;
                            if(SUCCEEDED(selected->Item(zero, & item)) && item){
                                BSTR text = nullptr;
                                if(SUCCEEDED(item->get_Path( & text)) && text){
                                    const std::wstring candidate(text,SysStringLen(text));
                                    const size_t slash = candidate.find_last_of(L"\\/");
                                    if(slash != std::wstring::npos &&
                                        _wcsicmp(candidate.c_str() + slash + 1,leaf.c_str()) == 0 &&
                                        SafeArchivePath(candidate,true)) {
                                        ++globalCount;globalMatch = candidate;
                                        if(sameWindow){++ownerCount;ownerMatch = candidate;}
                                    }
                                }
                                SysFreeString(text);item->Release();
                            }
                        }
                        selected->Release();
                    }
                    view->Release();
                }
                document->Release();
            }
            browser->Release();
        }
        dispatch->Release();
    }
    windows->Release();
    if(ownerCount == 1)resolved = std::move(ownerMatch);
    else if(ownerCount == 0 && globalCount == 1)resolved = std::move(globalMatch);
    Wh_Log(L"[AP bridge/explorer] R5 selected path matches owner=%u all=%u resolved=%u",
        ownerCount,globalCount,resolved.empty()?0U:1U);
    if(resolved.empty())return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    return S_OK;
}

static bool VerifyBrowserWindowOwner(HANDLE pipe,const Request & request) {
    if (request.command != ARCHIVE_BROKER_GRIP_UPDATE &&
        request.command != ARCHIVE_BROKER_GRIP_CLEAR &&
        request.command != ARCHIVE_BROKER_BROWSER_BACK &&
        request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO &&
        request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE &&
        request.command != ARCHIVE_BROKER_RESOLVE_SELECTED_PATH &&
        request.command != ARCHIVE_BROKER_SAVE_COLUMN) return true;
    UINT_PTR number = request.columnChange.previewWindow;
    if (request.command != ARCHIVE_BROKER_SAVE_COLUMN) {
        if (request.items.size() != 1) return false;
        wchar_t * finish = nullptr;
        number = static_cast<UINT_PTR>(wcstoull(request.items[0].c_str(),&finish,10));
        if (!number || !finish || *finish) return false;
    }
    HWND anchor = reinterpret_cast<HWND>(number);
    ULONG peerPid = 0;
    DWORD windowPid = 0;
    if (!GetNamedPipeClientProcessId(pipe, & peerPid) || !peerPid) return false;
    if (!IsWindow(anchor)) return request.command == ARCHIVE_BROKER_GRIP_CLEAR;
    GetWindowThreadProcessId(anchor, & windowPid);
    if(windowPid != peerPid)return false;
    if (request.command == ARCHIVE_BROKER_RESOLVE_SELECTED_PATH ||
        request.command == ARCHIVE_BROKER_SAVE_COLUMN) {
        wchar_t klass[128]{};
        GetClassNameW(anchor,klass,_countof(klass));
        return wcscmp(klass,ArchivePreviewHost::kWindowClass) == 0 &&
               IsWindow(GetParent(anchor));
    }
    return ValidBrowserAnchor(anchor);
}
// Authorize privileged low-IL requests only for a currently attached preview.
// The attach operation proves the archive is bound to a live window owned by
// the requesting preview-host PID. The broker rechecks window identity on each
// privileged request; merely knowing the pipe name is not sufficient.
struct AuthorizedPreview {
    DWORD pid{};
    HWND anchor{};
    std::wstring archive;
};
static std::mutex g_authorizedMutex;
static std::vector < AuthorizedPreview > g_authorizedPreviews;
static HWND PreviewAnchor(const Request & request) {
    if(request.items.size() != 1)return nullptr;
    wchar_t * end = nullptr;
    const unsigned long long val = wcstoull(request.items[0].c_str(), & end,10);
    if(!val || !end || * end)return nullptr;
    return reinterpret_cast < HWND > (static_cast < uintptr_t > (val));
}
static bool IsAuthorizedArchiveOperation(HANDLE pipe,const Request & request){
    if(request.command == ARCHIVE_BROKER_GRIP_UPDATE ||
       request.command == ARCHIVE_BROKER_GRIP_CLEAR ||
       request.command == ARCHIVE_BROKER_PING ||
       request.command == ARCHIVE_BROKER_QUERY_STATUS ||
       request.command == ARCHIVE_BROKER_RESOLVE_SELECTED_PATH ||
       request.command == ARCHIVE_BROKER_SAVE_COLUMN)return true;
    ULONG clientPid = 0;
    if(!GetNamedPipeClientProcessId(pipe, & clientPid) || !clientPid)return false;
    std::lock_guard < std::mutex > lock(g_authorizedMutex);
    bool authorized = false;
    for(auto it = g_authorizedPreviews.begin();it != g_authorizedPreviews.end();){
        DWORD windowPid = 0;
        if(!ValidBrowserAnchor(it->anchor) ||
           !GetWindowThreadProcessId(it->anchor, & windowPid) ||
           windowPid != it->pid){it = g_authorizedPreviews.erase(it);continue;}
        if(it->pid == clientPid &&
           _wcsicmp(it->archive.c_str(),request.archive.c_str()) == 0 &&
           ((request.command != ARCHIVE_BROKER_BROWSER_BACK &&
             request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO &&
             request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE) ||
            it->anchor == PreviewAnchor(request)))authorized = true;
        ++it;
    }
    return authorized;
}
static void TrackAuthorizedPreview(HANDLE pipe,const Request & request){
    if(request.command != ARCHIVE_BROKER_GRIP_UPDATE &&
       request.command != ARCHIVE_BROKER_GRIP_CLEAR)return;
    ULONG pid = 0;
    if(!GetNamedPipeClientProcessId(pipe, & pid) || !pid)return;
    HWND anchor = PreviewAnchor(request);
    if(!anchor)return;
    std::lock_guard < std::mutex > lock(g_authorizedMutex);
    for(auto it = g_authorizedPreviews.begin();it != g_authorizedPreviews.end();){
        if(it->pid == pid && it->anchor == anchor)it = g_authorizedPreviews.erase(it);
        else ++it;
    }
    if(request.command == ARCHIVE_BROKER_GRIP_UPDATE)
        g_authorizedPreviews.push_back({static_cast < DWORD > (pid),anchor,request.archive});
}
// Bound concurrent pipe readers: a slow or incomplete low-IL client may
// consume one connection for at most five seconds, but not block all other
// commands (notably Extract and Ctrl+C). Each reader owns its STA context.
static std::atomic < unsigned > g_activePipeReaders{0};
static std::mutex g_readerHandlesMutex;
static std::vector < HANDLE > g_readerHandles;
static constexpr unsigned kMaxPipeReaders = 6;
static void ReapFinishedPipeReaders() {
    // Serve() is the sole owner and caller. Join every live reader before
    // returning from the broker service so the DLL can be safely unloaded.
    std::lock_guard < std::mutex > guard(g_readerHandlesMutex);
    for(auto it = g_readerHandles.begin();it != g_readerHandles.end();) {
        const DWORD status = WaitForSingleObject( * it,0);
        if(status == WAIT_OBJECT_0) {
            CloseHandle( * it);
            it = g_readerHandles.erase(it);
        } else ++it;
    }
}
static DWORD WINAPI ServeOnePipeClient(LPVOID state) {
    // Reader concurrency is bounded separately from the queued STA workers.
    // Do not treat elapsed IPC-reader lifetime as provider progress.
    std::unique_ptr < HANDLE > pipeHolder(static_cast < HANDLE *  > (state));
    if (!pipeHolder) { g_activePipeReaders.fetch_sub(1); return 1; }
    HANDLE pipe =  * pipeHolder;
    const HRESULT initialized = OleInitialize(nullptr);
    if (SUCCEEDED(initialized)) {
            if (!VerifyBrokerClient(pipe)) {
                ULONG peer = 0;
                GetNamedPipeClientProcessId(pipe, & peer);
                wchar_t detail[160]{};
                swprintf_s(detail,L"Rejected broker IPC client PID %lu: executable, token or session mismatch",peer);
                Wh_Log(L"%s", detail);
                // Do not interpret any frames from an unauthorized client.
                DisconnectNamedPipe(pipe);
                CloseHandle(pipe);
                OleUninitialize();
                g_activePipeReaders.fetch_sub(1);
                return 0;
            }
            Request request;
            HRESULT hr = Receive(pipe,request);
            const bool requestValid = SUCCEEDED(hr) &&
                VerifyBrowserWindowOwner(pipe,request) &&
                IsAuthorizedArchiveOperation(pipe,request);
            if (SUCCEEDED(hr) && !requestValid) hr = E_ACCESSDENIED;
            // This must remain true until the peer confirms consuming the reply.
            // DisconnectNamedPipe discards unread bytes (ERROR_PIPE_NOT_CONNECTED).
            bool replyWritten = false;
            if (requestValid && request.command == ARCHIVE_BROKER_RESOLVE_SELECTED_PATH) {
                std::wstring resolved;
                hr = ResolveSelectedExplorerArchive(PreviewAnchor(request),
                                                  request.archive,resolved);
                const DWORD chars = SUCCEEDED(hr)
                    ? static_cast < DWORD > (resolved.size()) : 0;
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & hr,sizeof(hr))) &&
                    SUCCEEDED(ArchiveWriteAll(pipe, & chars,sizeof(chars)));
                if(replyWritten && chars)
                    replyWritten = SUCCEEDED(ArchiveWriteAll(pipe,resolved.data(),
                        chars * sizeof(wchar_t)));
            }
            if (requestValid && request.command == ARCHIVE_BROKER_SAVE_COLUMN) {
                const auto & change = request.columnChange.change;
                hr = ArchivePreviewColumns::Save(change) ? S_OK : E_FAIL;
                if (SUCCEEDED(hr)) {
                    if (HWND manager = g_gripManager.load())
                        PostMessageW(manager,WM_BROWSER_COLUMN_PREFERENCES_CHANGED,change.visibility,0);
                }
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe,&hr,sizeof(hr)));
            }
            if (requestValid && request.command == ARCHIVE_BROKER_PING) {
                const DWORD alive = ARCHIVE_BROKER_MAGIC;
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & alive,sizeof(alive)));
            }
            if (requestValid && request.command == ARCHIVE_BROKER_CANCEL_OPERATION) {
                ULONG clientPid = 0;
                GetNamedPipeClientProcessId(pipe, & clientPid);
                hr = CancelTrackedExtraction(request,clientPid);
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & hr,sizeof(hr)));
            }
            if (requestValid && request.command == ARCHIVE_BROKER_QUERY_STATUS) {
                ULONG callerPid = 0;
                GetNamedPipeClientProcessId(pipe, & callerPid);
                const auto status = QueryOperationStatus(request.requestId,callerPid);
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & status,sizeof(status)));
            }
            if (requestValid && (request.command == ARCHIVE_BROKER_GRIP_UPDATE ||
                                 request.command == ARCHIVE_BROKER_GRIP_CLEAR ||
                                 request.command == ARCHIVE_BROKER_BROWSER_BACK ||
                                 request.command == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO ||
                                 request.command == ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE)) {
                hr = DispatchGripUpdate(request);
                if(SUCCEEDED(hr))TrackAuthorizedPreview(pipe,request);
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & hr,sizeof(hr)));
            }
            if (requestValid && request.command == ARCHIVE_BROKER_COPY_SELECTION &&
                !g_cancelForShutdown.load(std::memory_order_acquire)) {
                // The clipboard IDataObject must belong to a persistent STA.
                // The IPC reader's OLE apartment is temporary, so running the
                // provider copy verb here can drop virtual files immediately.
                // Wait only on this broker reader thread, never in prevhost UI.
                ULONG clientPid = 0;
                GetNamedPipeClientProcessId(pipe, &clientPid);
                hr = DispatchAsync(request, clientPid);
                if (SUCCEEDED(hr)) {
                    const ULONGLONG timeout = request.items.size() > 1024 ? 53000 : 27000;
                    const ULONGLONG deadline = GetTickCount64() + timeout;
                    for (;;) {
                        const auto result = QueryOperationStatus(request.requestId, clientPid);
                        if (SUCCEEDED(result.lookup) &&
                            result.phase == ARCHIVE_OPERATION_DONE) {
                            hr = result.result;
                            break;
                        }
                        if (g_cancelForShutdown.load(std::memory_order_acquire)) {
                            hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                            break;
                        }
                        if (GetTickCount64() >= deadline) {
                            CancelTimedOutClipboard(request.requestId, clientPid);
                            hr = HRESULT_FROM_WIN32(ERROR_TIMEOUT);
                            break;
                        }
                        Sleep(30);
                    }
                }
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, &hr, sizeof(hr)));
            }
            else if (requestValid && request.command != ARCHIVE_BROKER_COPY_SELECTION &&
                       request.command != ARCHIVE_BROKER_GRIP_UPDATE &&
                       request.command != ARCHIVE_BROKER_GRIP_CLEAR &&
                       request.command != ARCHIVE_BROKER_BROWSER_BACK &&
                       request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_TO &&
                       request.command != ARCHIVE_BROKER_BROWSER_EXTRACT_SELECTED_HERE &&
                       request.command != ARCHIVE_BROKER_PING &&
                       request.command != ARCHIVE_BROKER_QUERY_STATUS &&
                       request.command != ARCHIVE_BROKER_CANCEL_OPERATION &&
                       request.command != ARCHIVE_BROKER_RESOLVE_SELECTED_PATH &&
                       request.command != ARCHIVE_BROKER_SAVE_COLUMN) {
                // Return queue admission status before the worker runs.
                // Completion (including failures) remains reported by Report().
                ULONG clientPid = 0;
                GetNamedPipeClientProcessId(pipe, & clientPid);
                hr = DispatchAsync(request,clientPid);
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & hr,sizeof(hr)));
                if (FAILED(hr)) {
                    wchar_t status[110]{};
                    swprintf_s(status, L"Could not queue broker command %lu: 0x%08lX",
                        request.command, static_cast < unsigned long > (hr));
                    Wh_Log(L"%s", status);
                }
            }
            if (requestValid && request.command == ARCHIVE_BROKER_COPY_SELECTION &&
                !replyWritten && g_cancelForShutdown.load(std::memory_order_acquire)) {
                const HRESULT unavailable = HRESULT_FROM_WIN32(ERROR_BUSY);
                replyWritten = SUCCEEDED(ArchiveWriteAll(pipe, & unavailable,sizeof(unavailable)));
            }
            if (!requestValid) Wh_Log(L"Rejected malformed or incomplete broker request");
            if (replyWritten) {
                // Bounded client ACK instead of an unbounded FlushFileBuffers() or
                // an immediate DisconnectNamedPipe(), which loses unread replies.
                DWORD ack = 0;
                const HRESULT acknowledged = ReadPipeWithDeadline(
                    pipe, & ack,sizeof(ack),GetTickCount64() + 2000);
                if (FAILED(acknowledged) || ack != ARCHIVE_BROKER_REPLY_ACK)
                    Wh_Log(L"Broker reply not acknowledged; closing timed-out client");
            }
    } else {
        Wh_Log(L"Could not initialize COM for IPC reader");
    }
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);
    if (SUCCEEDED(initialized)) OleUninitialize();
    g_activePipeReaders.fetch_sub(1);
    return 0;
}

static std::atomic < bool > g_stopBroker{false};
static std::atomic < bool > g_serviceReady{false};
static void JoinBrokerThread(HANDLE thread,PCWSTR name) {
    for (;;) {
        const DWORD result = WaitForSingleObject(thread,30000);
        if (result == WAIT_OBJECT_0) return;
        if (result == WAIT_TIMEOUT) {
            Wh_Log(L"Shutdown waiting for %s",name);
        } else {
            Wh_Log(L"Shutdown wait failed for %s (Win32=%lu)",name,GetLastError());
            Sleep(100);
        }
    }
}
static int Serve() {
    if(g_stopBroker.load(std::memory_order_acquire))return 0;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY, & token)) {
        TOKEN_ELEVATION elevation{}; DWORD cb = 0;
        if (GetTokenInformation(token,TokenElevation, & elevation,sizeof(elevation), & cb) &&
            elevation.TokenIsElevated) {
            CloseHandle(token);
            Wh_Log(L"Refusing to run broker as Administrator");
            return 7;
        }
        CloseHandle(token);
    }
    std::wstring pipeName,userSid;
    if (FAILED(ArchiveBrokerPipeName(pipeName)) || FAILED(ArchiveGetUserSid(userSid))) return 2;
    const std::wstring sddl = L"D:P(A;;GA;;;SY)(A;;GA;;;" + userSid + L")S:(ML;;NW;;;LW)";
    PSECURITY_DESCRIPTOR descriptor = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl.c_str(),SDDL_REVISION_1, & descriptor,nullptr)) return 3;
    SECURITY_ATTRIBUTES attrs{};
    attrs.nLength = sizeof(attrs);
    attrs.lpSecurityDescriptor = descriptor;
    Wh_Log(L"Broker started: normal-user transfer mode");
    if (!StartWorkQueue()) { LocalFree(descriptor); Wh_Log(L"Worker queue startup failed"); return 8; }
    HANDLE gripWorkerThread = CreateThread(nullptr,0,GripUiThread,nullptr,0,nullptr);
    // Maintenance never delays preview startup. The service joins the worker
    // before allowing another service instance to reuse the cancellation state.
    const ExternalDragHandle cleanupThread(gripWorkerThread ?
        CreateThread(nullptr,0,RecoverExternalDragStages,nullptr,0,nullptr) : nullptr,CloseHandle);
    if(!gripWorkerThread) {
        Wh_Log(L"Cannot launch broker UI apartment; stopping work queue");
        g_stopBroker.store(true,std::memory_order_release);
    }
    // Never publish the service as healthy before its UI manager exists.
    for (int attempt = 0; attempt < 300 && !g_stopBroker.load() &&
         !g_gripReady.load(); ++attempt) {
        if(gripWorkerThread && WaitForSingleObject(gripWorkerThread,0) == WAIT_OBJECT_0) break;
        Sleep(10);
    }
    if(!g_gripReady.load()) {
        Wh_Log(L"Broker UI not ready; pipe registration aborted");
        g_stopBroker.store(true,std::memory_order_release);
    }
    while (!g_stopBroker.load(std::memory_order_acquire)) {
        // Long Explorer sessions can serve many thousands of connections.
        // Reclaim terminated reader handles before accepting more work.
        ReapFinishedPipeReaders();
        HANDLE pipe = CreateNamedPipeW(pipeName.c_str(),PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
            kMaxPipeReaders + 1,65536,65536,0, & attrs);
        if (pipe == INVALID_HANDLE_VALUE) {Wh_Log(L"CreateNamedPipe failed");break;}
        if (g_stopBroker.load(std::memory_order_acquire)) {
            CloseHandle(pipe);
            break;
        }
        g_serviceReady.store(true,std::memory_order_release);
        if (ConnectNamedPipe(pipe,nullptr) || GetLastError() == ERROR_PIPE_CONNECTED) {
            if (g_stopBroker.load(std::memory_order_acquire)) { DisconnectNamedPipe(pipe); CloseHandle(pipe); break; }
            if (g_activePipeReaders.load() >= kMaxPipeReaders) {
                DisconnectNamedPipe(pipe);CloseHandle(pipe);
                continue;
            }
            auto * readerPipe = new (std::nothrow) HANDLE(pipe);
            if (!readerPipe) {DisconnectNamedPipe(pipe);CloseHandle(pipe);continue;}
            g_activePipeReaders.fetch_add(1);
            HANDLE thread = CreateThread(nullptr,0,ServeOnePipeClient,readerPipe,0,nullptr);
            if (!thread) {
                g_activePipeReaders.fetch_sub(1);
                delete readerPipe;
                DisconnectNamedPipe(pipe);CloseHandle(pipe);
            } else {
                // Retain the handle; shutdown must join every IPC reader
                // before Windhawk unloads the DLL.
                std::lock_guard < std::mutex > lock(g_readerHandlesMutex);
                g_readerHandles.push_back(thread);
            }
            ReapFinishedPipeReaders();
        } else CloseHandle(pipe);
    }
    SignalBrokerShutdown();
    // Draining is deliberate: outstanding Shell operations may need time.
    // If they hang, unload stays blocked until the live workers can exit.
    std::vector < HANDLE > readers;
    {
        std::lock_guard < std::mutex > lock(g_readerHandlesMutex);
        readers.swap(g_readerHandles);
    }
    for(HANDLE reader:readers) {
        // A stalled synchronous named-pipe read/write can be cancelled on
        // shutdown. This does NOT terminate the thread or bypass COM release.
        CancelSynchronousIo(reader);
        while(WaitForSingleObject(reader,30000) == WAIT_TIMEOUT)
            Wh_Log(L"Shutdown waiting for a stalled IPC/COM reader");
        CloseHandle(reader);
    }
    g_stopWorkers.store(true);
    { std::lock_guard < std::mutex > lock(g_authorizedMutex);g_authorizedPreviews.clear(); }
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_workQueue.clear();
        g_clipboardQueue.clear();
        if (g_workEvent) SetEvent(g_workEvent);
        if (g_clipboardEvent) SetEvent(g_clipboardEvent);
    }
    for (HANDLE & worker:g_workerThreads) {
        if (worker) {
            while(WaitForSingleObject(worker,30000) == WAIT_TIMEOUT)
                Wh_Log(L"Shutdown waiting for an active COM worker; not forcing termination");
            CloseHandle(worker);worker = nullptr;
        }
    }
    if (gripWorkerThread) {
        JoinBrokerThread(gripWorkerThread,L"Broker UI thread");
        CloseHandle(gripWorkerThread);
    }
    if (cleanupThread) JoinBrokerThread(cleanupThread.get(),L"external drag cleanup");
    if (g_clipboardThread) {
        while (WaitForSingleObject(g_clipboardThread, 30000) == WAIT_TIMEOUT)
            Wh_Log(L"Shutdown waiting for clipboard STA");
        CloseHandle(g_clipboardThread);
        g_clipboardThread = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        if (g_clipboardEvent) {
            CloseHandle(g_clipboardEvent);
            g_clipboardEvent = nullptr;
        }
        if (g_workEvent) {
            CloseHandle(g_workEvent);
            g_workEvent = nullptr;
        }
    }
    g_serviceReady.store(false,std::memory_order_release);
    LocalFree(descriptor);
    return 4;
}
// Service shutdown preserves COM-worker lifetime until callbacks return.
static void StopService() {
    g_stopBroker.store(true, std::memory_order_release);
    SignalBrokerShutdown();
    g_stopWorkers.store(true, std::memory_order_release);
    {
        std::lock_guard<std::mutex> lock(g_workMutex);
        g_workQueue.clear();
        g_clipboardQueue.clear();
        if (g_workEvent) SetEvent(g_workEvent);
        if (g_clipboardEvent) SetEvent(g_clipboardEvent);
    }
    std::wstring pipe;
    if (SUCCEEDED(ArchiveBrokerPipeName(pipe))) {
        HANDLE handle = CreateFileW(pipe.c_str(),GENERIC_READ | GENERIC_WRITE,0,nullptr,
                    OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    }
}

} // namespace ArchivePreviewBroker


namespace ArchivePreviewRuntime {
static std::atomic < HANDLE > g_brokerOwner{nullptr};
static HANDLE g_serviceThread = nullptr;
static std::atomic < bool > g_stopping{false};
static DWORD WINAPI ServiceThread(void * context){
    HANDLE owningEvent = static_cast < HANDLE > (context);
    const HRESULT init = OleInitialize(nullptr);
    const int result = SUCCEEDED(init)?ArchivePreviewBroker::Serve():10;
    if(SUCCEEDED(init))OleUninitialize();
    Wh_Log(L"ArchivePreview: in-process broker exited, code=%d",result);
    HANDLE expected = owningEvent;
    g_brokerOwner.compare_exchange_strong(expected,nullptr);
    CloseHandle(owningEvent);
    return result;
}
static bool StartService(){
    // Only one explorer process owns the broker named pipe at a time.
    HANDLE own = CreateEventW(nullptr,TRUE,FALSE,L"Local\\ArchivePreview_BrokerOwner");
    if(!own || GetLastError() == ERROR_ALREADY_EXISTS){
        if(own)CloseHandle(own);
        Wh_Log(L"ArchivePreview: another instance still owns the broker; retry pending");
        return false;
    }
    ArchivePreviewBroker::g_brokerStopEvent = CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if (!ArchivePreviewBroker::g_brokerStopEvent) {
        CloseHandle(own);
        return false;
    }
    g_brokerOwner.store(own);
    ArchivePreviewBroker::g_cancelForShutdown.store(false,std::memory_order_release);
    ArchivePreviewBroker::g_stopBroker.store(false, std::memory_order_release);
    ArchivePreviewBroker::g_stopWorkers.store(false, std::memory_order_release);
    ArchivePreviewBroker::g_gripReady.store(false, std::memory_order_release);
    ArchivePreviewBroker::g_serviceReady.store(false,std::memory_order_release);
    g_serviceThread = CreateThread(nullptr,0,ServiceThread,own,0,nullptr);
    if(!g_serviceThread){
        g_brokerOwner.store(nullptr);
        CloseHandle(std::exchange(ArchivePreviewBroker::g_brokerStopEvent,nullptr));
        CloseHandle(own);
        return false;
    }
    // The broker creates its own UI apartment, operation workers and pipe.
    // Do not wait here: this function can run from Explorer's association
    // hooks on the UI thread. The Preview Host retries its attach until ready.
    return true;
}
static std::atomic_flag g_brokerAttempt = ATOMIC_FLAG_INIT;
static std::atomic < ULONGLONG > g_brokerLastAttempt{0};
static void ReleaseServiceHandles() {
    CloseHandle(std::exchange(g_serviceThread,nullptr));
    CloseHandle(std::exchange(ArchivePreviewBroker::g_brokerStopEvent,nullptr));
}
static void StopService(){
    // Do not race a concurrent MaybeStartService inside an Explorer hook.
    while(g_brokerAttempt.test(std::memory_order_acquire))Sleep(1);
    if(!g_serviceThread)return;
    ArchivePreviewBroker::StopService();
    CancelSynchronousIo(g_serviceThread); // wake a blocked ConnectNamedPipe
    // A timeout is NOT a safe indication that executable code can unload.
    // Windhawk calls FreeLibrary immediately after Wh_ModUninit. Do not return
    // while the service thread, its COM workers or its HWND callbacks are live.
    ULONGLONG lastLog = GetTickCount64();
    for(;;){
        const DWORD result = WaitForSingleObject(g_serviceThread,250);
        if(result == WAIT_OBJECT_0)break;
        if(result == WAIT_TIMEOUT){
            // The first cancellation may have raced the start of ConnectNamedPipe.
            CancelSynchronousIo(g_serviceThread);
            const ULONGLONG now = GetTickCount64();
            if (now - lastLog >= 30000) {
                Wh_Log(L"ArchivePreview unload: waiting for broker/Shell provider to exit");
                lastLog = now;
            }
            continue;
        }
        Wh_Log(L"ArchivePreview unload: broker wait failed (Win32=%lu)",GetLastError());
        Sleep(100);
    }
    ReleaseServiceHandles();
}

static void MaybeStartService(){
    if(g_stopping.load(std::memory_order_acquire))return;
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG last = g_brokerLastAttempt.load(std::memory_order_relaxed);
    // A zero timestamp means no attempt yet. Never suppress the first preview
    // just because the system booted less than three seconds ago.
    if(last != 0 && now - last < 3000)return;
    if(g_brokerAttempt.test_and_set(std::memory_order_acquire))return;
    g_brokerLastAttempt.store(now,std::memory_order_relaxed);
    // If an earlier broker shutdown has completed, reap the thread handle.
    // The old owner event has already been closed by ServiceThread.
    if(g_serviceThread && WaitForSingleObject(g_serviceThread,0) == WAIT_OBJECT_0){
        ReleaseServiceHandles();
    }
    if(!g_serviceThread && !g_stopping.load(std::memory_order_acquire)){
        if(StartService())
            Wh_Log(L"[AP bridge/explorer] broker ownership acquired");
        else
            Wh_Log(L"[AP bridge/explorer] association hooks remain active; broker owned elsewhere or starting");
    }
    g_brokerAttempt.clear(std::memory_order_release);
}

} // namespace ArchivePreviewRuntime

namespace ArchivePreviewVirtual {
static constexpr wchar_t kVirtualClsidText[] =
    L"{1531D583-8375-4D3F-B5FB-D23BBD169F22}";
static constexpr wchar_t kPreviewAssociation[] =
    L"{8895B1C6-B41F-4C1C-A562-0D564250836F}";
static std::atomic < unsigned > g_associationHits{0};
static std::atomic < unsigned > g_registryHits{0};
static std::atomic < unsigned > g_regOpenHits{0};
static std::atomic < unsigned > g_queryPreviewCalls{0};
static std::atomic < unsigned > g_queryVirtualHits{0};
static std::atomic < bool > g_stopping{false};

using AssocQueryStringWFn = decltype( & AssocQueryStringW);
using RegGetValueWFn = decltype( & RegGetValueW);
using RegOpenKeyExWFn = decltype( & RegOpenKeyExW);
using QueryAssocGetStringFn = HRESULT (STDMETHODCALLTYPE * )(IQueryAssociations * ,ASSOCF,ASSOCSTR,LPCWSTR,LPWSTR,DWORD * );
static QueryAssocGetStringFn g_queryStringOriginal = nullptr;
// Archive associations can arrive as configured extensions, ProgIDs or HKEYs.
// Build these aliases read-only from the current user's associations.
static std::mutex g_aliasMutex;
static std::unordered_set < std::wstring > g_archiveProgIds;
static AssocQueryStringWFn g_assocOriginal = nullptr;
static RegGetValueWFn g_regOriginal = nullptr;
static RegOpenKeyExWFn g_regOpenOriginal = nullptr;
static std::wstring g_existingTextPreviewKey; // actual HKCR key, opened read-only

static std::wstring Lower(std::wstring s) {
    std::transform(s.begin(),s.end(),s.begin(),[](wchar_t c) {
        return static_cast < wchar_t > (towlower(c));
    });
    return s;
}
static bool EndsWith(const std::wstring & text,const std::wstring & suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(),suffix.size(),suffix) == 0;
}
// Read association labels without installing/altering any Windows key.
// Windows may initialize IQueryAssociations using a ProgID from a file type,
// such as CompressedFolder, rather than the literal .zip extension.
static bool ReadAssocString(HKEY root, const std::wstring & path,
                            const wchar_t * value, std::wstring * result) {
    wchar_t buffer[260]{};
    DWORD bytes = sizeof(buffer), type = 0;
    if (RegGetValueW(root,path.c_str(),value,RRF_RT_REG_SZ, & type,
                     buffer, & bytes) != ERROR_SUCCESS || bytes < sizeof(wchar_t))
        return false;
    buffer[_countof(buffer) - 1] = L'\0';
    std::wstring text = Lower(buffer);
    if (text.empty() || text.size() >= _countof(buffer) - 1) return false;
    * result = std::move(text);
    return true;
}
static void RefreshArchiveProgIds() {
    std::unordered_set < std::wstring > ids;
    const auto extensions = ArchivePreviewOptions::ExtensionSnapshot();
    for (const auto & ext:extensions) {
        std::wstring id;
        if (ReadAssocString(HKEY_CLASSES_ROOT,ext,nullptr, & id)) ids.insert(id);
        const std::wstring choice =
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\" +
            ext + L"\\UserChoice";
        if (ReadAssocString(HKEY_CURRENT_USER,choice,L"ProgId", & id)) ids.insert(id);
    }
    if (std::find(extensions.begin(),extensions.end(),L".zip") != extensions.end())
        ids.insert(L"compressedfolder");
    const size_t count = ids.size();
    {
        std::lock_guard < std::mutex > lock(g_aliasMutex);
        g_archiveProgIds.swap(ids);
    }
    Wh_Log(L"[AP bridge/explorer] R4 loaded %u archive ProgID aliases",
        static_cast < unsigned > (count));
}
static bool SupportedAssoc(const std::wstring & name) {
    if (name.empty()) return false;
    std::wstring low = Lower(name);
    // Some Shell code requests associations under SystemFileAssociations.
    constexpr wchar_t prefix[] = L"systemfileassociations\\";
    if (low.rfind(prefix,0) == 0)
        low.erase(0,_countof(prefix) - 1);
    const auto extensions = ArchivePreviewOptions::ExtensionSnapshot();
    for (const auto & extension:extensions) {
        if (low == extension) return true;
        // The actual filename is safe evidence; a shared ProgID is not.
        if ((low.find(L'\\') != std::wstring::npos ||
             low.find(L'/') != std::wstring::npos) && EndsWith(low,extension))
            return true;
    }
    return false;
}
static bool KnownArchiveProgId(const std::wstring & name) {
    std::lock_guard < std::mutex > lock(g_aliasMutex);
    return g_archiveProgIds.find(Lower(name)) != g_archiveProgIds.end();
}
static bool FileHasClass(const std::wstring & path,const std::wstring & cls) {
    const auto extensions = ArchivePreviewOptions::ExtensionSnapshot();
    for (const auto & ext:extensions) {
        if (!EndsWith(Lower(path),ext))continue;
        if (cls == L"compressedfolder" && ext == L".zip")return true;
        if (cls == L"7zg.exe" && ext == L".7z")return true;
        std::wstring actual;
        if (ReadAssocString(HKEY_CLASSES_ROOT,ext,nullptr, & actual) && actual == cls)
            return true;
        const std::wstring choice =
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\" + ext + L"\\UserChoice";
        if (ReadAssocString(HKEY_CURRENT_USER,choice,L"ProgId", & actual) && actual == cls)
            return true;
    }
    return false;
}
// An alias is not an extension: refuse it unless a *unique*, real Explorer
// selection in the same process has this class and an enabled suffix. If
// another view selects a file with this class but a disabled suffix, fail
// closed rather than redirect that view to the TXT previewer.
static bool SelectedArchiveForClass(const std::wstring & cls) {
    HWND foreground = GetAncestor(GetForegroundWindow(),GA_ROOT);
    if(!foreground)return false;
    DWORD foregroundPid = 0;
    GetWindowThreadProcessId(foreground, & foregroundPid);
    if(foregroundPid != GetCurrentProcessId())return false;
    IShellWindows * windows = nullptr;
    if(FAILED(CoCreateInstance(CLSID_ShellWindows,nullptr,CLSCTX_LOCAL_SERVER,
        IID_PPV_ARGS( & windows))) || !windows)return false;
    long count = 0;
    unsigned matchingArchives = 0,conflictingFiles = 0;
    bool foregroundArchive = false;
    if(SUCCEEDED(windows->get_Count( & count)))for(long i = 0;i < count && i < 64;++i){
        VARIANT index{};V_VT( & index) = VT_I4;V_I4( & index) = i;
        IDispatch * dispatch = nullptr;
        if(FAILED(windows->Item(index, & dispatch)) || !dispatch)continue;
        IWebBrowser2 * browser = nullptr;
        if(SUCCEEDED(dispatch->QueryInterface(IID_PPV_ARGS( & browser))) && browser){
            SHANDLE_PTR browserHandle = 0;
            HWND browserHwnd = nullptr;
            DWORD browserPid = 0;
            if(SUCCEEDED(browser->get_HWND( & browserHandle))){
                browserHwnd = GetAncestor(reinterpret_cast < HWND > (
                    static_cast < INT_PTR > (browserHandle)),GA_ROOT);
                GetWindowThreadProcessId(browserHwnd, & browserPid);
            }
            if(browserPid == GetCurrentProcessId()){
                IDispatch * document = nullptr;
                if(SUCCEEDED(browser->get_Document( & document)) && document){
                    IShellFolderViewDual * view = nullptr;
                    if(SUCCEEDED(document->QueryInterface(IID_PPV_ARGS( & view))) && view){
                        FolderItems * selected = nullptr;
                        if(SUCCEEDED(view->SelectedItems( & selected)) && selected){
                            long number = 0;
                            if(SUCCEEDED(selected->get_Count( & number)) && number == 1){
                                VARIANT zero{};V_VT( & zero) = VT_I4;V_I4( & zero) = 0;
                                FolderItem * item = nullptr;
                                if(SUCCEEDED(selected->Item(zero, & item)) && item){
                                    BSTR text = nullptr;
                                    if(SUCCEEDED(item->get_Path( & text)) && text){
                                        std::wstring path(text,SysStringLen(text));
                                        // Consider only actual local filenames, not shell objects.
                                        if(path.size() > 3 && path[1] == L':' && path[2] == L'\\'){
                                            const bool isArchive = ArchivePreviewOptions::MatchArchive(path);
                                            const wchar_t * suffix = PathFindExtensionW(path.c_str());
                                            std::wstring actual;
                                            if(suffix && * suffix){
                                                ReadAssocString(HKEY_CLASSES_ROOT,suffix,nullptr, & actual);
                                                const std::wstring choice =
                                                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\" +
                                                    std::wstring(suffix) + L"\\UserChoice";
                                                std::wstring userChoice;
                                                if(ReadAssocString(HKEY_CURRENT_USER,choice,L"ProgId", & userChoice))
                                                    actual = std::move(userChoice);
                                            }
                                            const bool sameClass = actual == cls ||
                                                (cls == L"compressedfolder" && _wcsicmp(suffix,L".zip") == 0) ||
                                                (cls == L"7zg.exe" && _wcsicmp(suffix,L".7z") == 0);
                                            if(sameClass){
                                                if(isArchive && FileHasClass(path,cls)){
                                                    ++matchingArchives;
                                                    if(browserHwnd == foreground)foregroundArchive = true;
                                                } else ++conflictingFiles;
                                            }
                                        }
                                    }
                                    SysFreeString(text);item->Release();
                                }
                            }
                            selected->Release();
                        }
                        view->Release();
                    }
                    document->Release();
                }
            }
            browser->Release();
        }
        dispatch->Release();
    }
    windows->Release();
    return foregroundArchive && matchingArchives != 0 && conflictingFiles == 0;
}

// Resolve the backing ProgID of a Shell-created IQueryAssociations instance.
// A Shell folder can hand out an already-initialized interface without ever
// calling the Init method on the instance we hook. Never classify such a
// preview lookup as an archive without checking its actual association.
// NtQueryKey is used only for read-only inspection of a handle returned by the
// documented IQueryAssociations::GetKey method. It does not register a handler.
static std::wstring RegistryClassKeyName(HKEY key) {
    if (!key || key == HKEY_CLASSES_ROOT || key == HKEY_CURRENT_USER ||
        key == HKEY_LOCAL_MACHINE || key == HKEY_USERS) return {};
    using NtQueryKeyFn = LONG (NTAPI * )(HANDLE,ULONG,PVOID,ULONG,PULONG);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto ntQueryKey = ntdll ? reinterpret_cast < NtQueryKeyFn > (
        GetProcAddress(ntdll,"NtQueryKey")) : nullptr;
    if (!ntQueryKey) return {};
    constexpr ULONG kKeyNameInformation = 3;
    ULONG required = 0;
    ntQueryKey(reinterpret_cast < HANDLE > (key),kKeyNameInformation,
               nullptr,0, & required);
    if (required < sizeof(ULONG) + sizeof(wchar_t) || required > 16384) return {};
    std::vector < unsigned char > bytes(required + sizeof(wchar_t));
    ULONG returned = 0;
    if (ntQueryKey(reinterpret_cast < HANDLE > (key),kKeyNameInformation,
                   bytes.data(),static_cast < ULONG > (bytes.size()), & returned) < 0)
        return {};
    ULONG length = 0;
    memcpy( & length,bytes.data(),sizeof(length));
    if (!length || (length % sizeof(wchar_t)) ||
        length > bytes.size() - sizeof(ULONG)) return {};
    std::wstring full(length / sizeof(wchar_t),L'\0');
    memcpy(full.data(),bytes.data() + sizeof(ULONG),length);
    full = Lower(std::move(full));
    // Only treat an actual Classes key as a file association. Reject other
    // registry locations even when their last component looks like a ProgID.
    if (full.find(L"\\software\\classes\\") == std::wstring::npos &&
        full.find(L"_classes\\") == std::wstring::npos) return {};
    const size_t last = full.find_last_of(L'\\');
    if (last == std::wstring::npos || last + 1 == full.size()) return {};
    return full.substr(last + 1);
}
// A shared 7-Zip application class must be bound to a selected .7z item.
static bool QueryObjectArchive(IQueryAssociations * self,std::wstring * source) {
    if (!self) return false;
    // First use the public API to ask the existing object which ProgID it
    // represents. Call the original trampoline so we do not reenter our hook.
    wchar_t progId[260]{};
    DWORD chars = static_cast < DWORD > (_countof(progId));
    const HRESULT idHr = g_queryStringOriginal(self,static_cast < ASSOCF > (0),
        ASSOCSTR_PROGID,nullptr,progId, & chars);
    if (idHr == S_OK && chars > 0 && chars <= _countof(progId) &&
        (SupportedAssoc(progId) ||
         (KnownArchiveProgId(progId) && SelectedArchiveForClass(Lower(progId))))) {
        * source = L"ProgID:" + Lower(progId);
        return true;
    }
    // For objects supplied by IShellFolder, Init may never be called. GetKey
    // returns the association's actual class key without guessing from the
    // selected window or another object's last Init call.
    HKEY classKey = nullptr;
    const HRESULT keyHr = self->GetKey(static_cast < ASSOCF > (0),
        ASSOCKEY_CLASS,nullptr, & classKey);
    if (SUCCEEDED(keyHr) && classKey) {
        std::wstring name = RegistryClassKeyName(classKey);
        if (classKey != HKEY_CLASSES_ROOT && classKey != HKEY_CURRENT_USER &&
            classKey != HKEY_LOCAL_MACHINE && classKey != HKEY_USERS)
            RegCloseKey(classKey);
        if (!name.empty()) {
            * source = L"class:" + name;
            if(SupportedAssoc(name))return true;
            if((name == L"7zg.exe" || KnownArchiveProgId(name)) &&
               SelectedArchiveForClass(name)) {
                * source = L"class:" + name + L"; selected enabled archive";
                return true;
            }
            return false;
        }
    }
    if (idHr == S_OK && chars > 0 && chars <= _countof(progId)) {
        * source = L"other:" + Lower(progId);
    } else {
        * source = L"unresolved";
    }
    return false;
}
static bool PreviewAssociation(LPCWSTR extra) {
    return extra && _wcsicmp(extra,kPreviewAssociation) == 0;
}
static bool VirtualRegistryPath(HKEY root,LPCWSTR subkey,LPCWSTR value) {
    if (!subkey || (value && *value) ||
        (root != HKEY_CLASSES_ROOT && root != HKEY_CURRENT_USER)) return false;
    constexpr wchar_t suffix[] =
        L"\\shellex\\{8895B1C6-B41F-4C1C-A562-0D564250836F}";
    constexpr wchar_t userPrefix[] = L"software\\classes\\";
    const wchar_t *association = subkey;
    if (root == HKEY_CURRENT_USER) {
        if (_wcsnicmp(association,userPrefix,_countof(userPrefix)-1) != 0)
            return false;
        association += _countof(userPrefix)-1;
    }
    const size_t length = wcslen(association);
    constexpr size_t suffixLength = _countof(suffix)-1;
    if (length <= suffixLength ||
        _wcsicmp(association + length - suffixLength,suffix) != 0)
        return false;
    // Only a preview-association lookup reaches these allocations.
    return SupportedAssoc(Lower(std::wstring(association,length-suffixLength)));
}
static void LogHit(const wchar_t * label,unsigned count) {
    if (count == 1 || count == 5 || count == 20)
        Wh_Log(L"[AP bridge/explorer] %s: %u calls",label,count);
}
static HRESULT WriteClsid(WCHAR * output,DWORD * chars) {
    if (!chars) return E_POINTER;
    constexpr DWORD needed = static_cast < DWORD > (_countof(kVirtualClsidText));
    if (!output) { * chars = needed; return S_FALSE; }
    if ( * chars < needed) { * chars = needed; return E_POINTER; }
    memcpy(output,kVirtualClsidText,sizeof(kVirtualClsidText));
    * chars = needed;
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE QueryGetStringHook(IQueryAssociations * self,
    ASSOCF flags,ASSOCSTR kind,LPCWSTR extra,LPWSTR output,DWORD * chars) {
    if (kind == ASSOCSTR_SHELLEXTENSION && PreviewAssociation(extra)) {
        const unsigned previews = ++g_queryPreviewCalls;
        bool archive = false;
        // Validate the live association rather than trusting stale COM
        // pointer identity from an unrelated Init call.
        std::wstring detected = L"unresolved";
        static thread_local bool probing = false;
        if (!probing && !g_stopping.load(std::memory_order_relaxed)) {
            probing = true;
            archive = QueryObjectArchive(self, & detected);
            probing = false;
        }
        if (previews == 1 || previews == 20)
            Wh_Log(L"[AP bridge/explorer] preview lookups=%u archive=%u",
                   previews,archive ? 1U : 0U);
        if (archive && !g_stopping.load(std::memory_order_relaxed)) {
            // Preserve an existing third-party handler for ambiguous ProgIDs.
            // The enabled extension is not known from this COM object alone.
            if(detected.rfind(L"ProgID:",0) == 0 || detected.rfind(L"class:",0) == 0){
                wchar_t existing[96]{};DWORD existingChars = _countof(existing);
                if(g_queryStringOriginal(self,flags,kind,extra,existing, & existingChars) == S_OK &&
                   existing[0] && _wcsicmp(existing,kVirtualClsidText) != 0) {
                    return g_queryStringOriginal(self,flags,kind,extra,output,chars);
                }
            }
            ArchivePreviewRuntime::MaybeStartService();
            ++g_queryVirtualHits;
            return WriteClsid(output,chars);
        }
    }
    return g_queryStringOriginal(self,flags,kind,extra,output,chars);
}
static bool InstallQueryAssociationHooks() {
    // AssocCreate is the documented constructor. The vtable method slots are
    // IUnknown (0..2), Init (3), GetString (4).
    HRESULT coInit = CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    IQueryAssociations * query = nullptr;
    // Some Windhawk MinGW builds declare IQueryAssociations without exporting
    // __mingw_uuidof<IQueryAssociations>(). Use its published COM IID directly.
    // IID_IQueryAssociations = C46CA590-3C3F-11D2-BEE6-0000F805CA57.
    static const IID kQueryAssociationsIid =
        {0xC46CA590,0x3C3F,0x11D2,{0xBE,0xE6,0x00,0x00,0xF8,0x05,0xCA,0x57}};
    const HRESULT hr = AssocCreate(CLSID_QueryAssociations,
        kQueryAssociationsIid,reinterpret_cast < void *  *  > ( & query));
    if (FAILED(hr) || !query) {
        Wh_Log(L"[AP bridge/explorer] IQueryAssociations provider unavailable hr=0x%08lX",
            static_cast < unsigned long > (hr));
        if (SUCCEEDED(coInit)) CoUninitialize();
        return false;
    }
    void *  * table =  * reinterpret_cast < void *  *  *  > (query);
    // The Init callback is untouched. We only need to intercept live
    // preview-handler queries in GetString (vtable index 4).
    const bool getStringHook = Wh_SetFunctionHook(table[4],
        reinterpret_cast < void *  > ( & QueryGetStringHook),
        reinterpret_cast < void *  *  > ( & g_queryStringOriginal));
    query->Release();
    if (SUCCEEDED(coInit)) CoUninitialize();
    Wh_Log(L"[AP bridge/explorer] IQueryAssociations GetString hook=%u",
           getStringHook ? 1U : 0U);
    return getStringHook;
}
static HRESULT WINAPI AssocHook(ASSOCF flags,ASSOCSTR kind,LPCWSTR association,
                                LPCWSTR extra,LPWSTR output,DWORD * chars) {
    if (!g_stopping.load(std::memory_order_relaxed) &&
        kind == ASSOCSTR_SHELLEXTENSION && association &&
        PreviewAssociation(extra) && SupportedAssoc(association)) {
        ArchivePreviewRuntime::MaybeStartService();
        LogHit(L"AssocQueryStringW",++g_associationHits);
        return WriteClsid(output,chars);
    }
    return g_assocOriginal(flags,kind,association,extra,output,chars);
}
static LSTATUS WINAPI RegistryHook(HKEY root,LPCWSTR subkey,LPCWSTR value,
                                   DWORD flags,LPDWORD type,PVOID output,LPDWORD size) {
    if (!g_stopping.load(std::memory_order_relaxed) &&
        VirtualRegistryPath(root,subkey,value) &&
        (!(flags & RRF_RT_ANY) || (flags & RRF_RT_REG_SZ))) {
        ArchivePreviewRuntime::MaybeStartService();
        LogHit(L"RegGetValueW",++g_registryHits);
        if (!size) return ERROR_INVALID_PARAMETER;
        constexpr DWORD needed = sizeof(kVirtualClsidText);
        if (type) * type = REG_SZ;
        if (!output) { * size = needed; return ERROR_SUCCESS; }
        if ( * size < needed) {
            if (flags & RRF_ZEROONFAILURE) memset(output,0, * size);
            * size = needed;
            return ERROR_MORE_DATA;
        }
        memcpy(output,kVirtualClsidText,needed);
        * size = needed;
        return ERROR_SUCCESS;
    }
    return g_regOriginal(root,subkey,value,flags,type,output,size);
}
static LSTATUS WINAPI RegistryOpenProbe(HKEY root,LPCWSTR subkey,DWORD options,
                                         REGSAM access,PHKEY result) {
    if (!g_stopping.load(std::memory_order_relaxed) &&
        VirtualRegistryPath(root,subkey,nullptr)) {
        ArchivePreviewRuntime::MaybeStartService();
        LogHit(L"RegOpenKeyExW (redirect to real TXT association)",++g_regOpenHits);
        // Return a genuine read-only HKEY to the *existing* Windows TXT
        // preview association, never a fabricated handle or registry write.
        if(!g_existingTextPreviewKey.empty() &&
           (access & (KEY_SET_VALUE | KEY_CREATE_SUB_KEY | DELETE | WRITE_DAC | WRITE_OWNER)) == 0) {
            const LSTATUS rc = g_regOpenOriginal(HKEY_CLASSES_ROOT,
                g_existingTextPreviewKey.c_str(),options,access,result);
            if(rc == ERROR_SUCCESS)return rc;
        }
    }
    return g_regOpenOriginal(root,subkey,options,access,result);
}
static bool AddHook(HMODULE module,const char * name,void * replacement,void *  * original) {
    FARPROC target = module?GetProcAddress(module,name):nullptr;
    if (!target || !Wh_SetFunctionHook(reinterpret_cast < void *  > (target),replacement,original)) {
        Wh_Log(L"[AP bridge/explorer] Hook unavailable: %S",name);
        return false;
    }
    return true;
}
static bool Install() {
    HMODULE shlwapi = GetModuleHandleW(L"shlwapi.dll");
    HMODULE advapi = GetModuleHandleW(L"advapi32.dll");
    if (!shlwapi || !advapi) {
        Wh_Log(L"[AP bridge/explorer] Missing association libraries");
        return false;
    }
    const bool assoc = AddHook(shlwapi,"AssocQueryStringW",
        reinterpret_cast < void *  > (AssocHook),reinterpret_cast < void *  *  > ( & g_assocOriginal));
    const bool reg = AddHook(advapi,"RegGetValueW",
        reinterpret_cast < void *  > (RegistryHook),reinterpret_cast < void *  *  > ( & g_regOriginal));
    const bool regOpen = AddHook(advapi,"RegOpenKeyExW",
        reinterpret_cast < void *  > (RegistryOpenProbe),
        reinterpret_cast < void *  *  > ( & g_regOpenOriginal));
    // Discover a real registered TXT preview association to reuse its HKEY.
    // The text association may be on .txt or on txtfile depending on Windows.
    for (const wchar_t * path : {
        L".txt\\shellex\\{8895B1C6-B41F-4C1C-A562-0D564250836F}",
        L"txtfile\\shellex\\{8895B1C6-B41F-4C1C-A562-0D564250836F}",
        L"SystemFileAssociations\\text\\shellex\\{8895B1C6-B41F-4C1C-A562-0D564250836F}"}) {
        wchar_t val[100]{};DWORD cb = sizeof(val),type = 0;
        if(RegGetValueW(HKEY_CLASSES_ROOT,path,nullptr,RRF_RT_REG_SZ,
                & type,val, & cb) == ERROR_SUCCESS &&
           _wcsicmp(val,kVirtualClsidText) == 0) {
            g_existingTextPreviewKey = path;
            Wh_Log(L"[AP bridge/explorer] real TXT preview HKEY source: %s",path);
            break;
        }
    }
    if(g_existingTextPreviewKey.empty())
        Wh_Log(L"[AP bridge/explorer] no TXT preview HKEY found; only direct association queries are virtualized");
    // Populate aliases before hooking GetString. WinRAR.ZIP, WinRAR and
    // CompressedFolder are often what the Shell uses instead of .zip/.rar.
    RefreshArchiveProgIds();
    const bool queryAssoc = InstallQueryAssociationHooks();
    Wh_Log(L"[AP bridge/explorer] assoc=%u reg=%u open=%u query=%u; virtual target=Windows TXT Previewer",
        assoc?1U:0U,reg?1U:0U,regOpen?1U:0U,queryAssoc?1U:0U);
    return assoc && reg;
}
} // namespace ArchivePreviewVirtual
// Registry-free reuse of the existing Windows TXT preview COM
// surrogate. This does not modify HKCU/HKLM. The original TXT handler is
// retained for ordinary text previews. Some Windows builds may bypass
// shell32!DllGetClassObject; the bridge must fail closed in that case.
namespace ArchivePreviewBridge {
static const CLSID kTextHandler = {0x1531d583,0x8375,0x4d3f,{0xb5,0xfb,0xd2,0x3b,0xbd,0x16,0x9f,0x22}};
using DllGetClassObjectFn = HRESULT (STDAPICALLTYPE * )(REFCLSID,REFIID,LPVOID * );
static DllGetClassObjectFn originalDllGetClassObject = nullptr;
static std::atomic < unsigned > classFactoryHits{0};
static std::atomic < unsigned > createdObjects{0};
static std::atomic < unsigned > archiveObjects{0};
// Count outstanding host-visible COM objects separately from archive objects.
// These are diagnostic only: never try to Release() a reference owned by COM.
static std::atomic < long > liveAdapters{0};
static std::atomic < long > liveFactories{0};
static std::atomic < bool > stopping{false};

class PreviewAdapter final : public IPreviewHandler, public IInitializeWithFile,
    public IInitializeWithStream, public IInitializeWithItem, public IObjectWithSite,
    public IOleWindow, public IPreviewHandlerVisuals {
    std::atomic < ULONG > refs_{1};
    IUnknown * native_{};
    IUnknown * archive_{};
    IUnknown * originalForFile_{}; // non-archive's real Windows handler if redirected
    IUnknown * site_{};
    bool archiveSelected_{};
    bool initialized_{};
    bool windowKnown_{};
    HWND window_{};
    RECT area_{};
    bool backgroundKnown_{},foregroundKnown_{},fontKnown_{};
    COLORREF background_{},foreground_{};
    LOGFONTW font_{};

    IUnknown * Active() const {
        return archiveSelected_ && archive_ ? archive_:
            (originalForFile_ ? originalForFile_:native_);
    }
    template < typename T,typename F > HRESULT Call(REFIID iid,F fn) {
        IUnknown * active = Active();
        if (!active) return E_UNEXPECTED;
        T * api = nullptr;
        HRESULT hr = active->QueryInterface(iid,reinterpret_cast < void *  *  > ( & api));
        if(SUCCEEDED(hr) && api){hr = fn(api);api->Release();}
        return hr;
    }
    HRESULT EnsureArchive() {
        if(stopping.load(std::memory_order_acquire))return CO_E_SERVER_STOPPING;
        if(archive_)return S_OK;
        auto * obj = new(std::nothrow) ArchivePreviewHost::ArchiveHandler();
        if(!obj)return E_OUTOFMEMORY;
        archive_ = static_cast < IPreviewHandler *  > (obj);
        Wh_Log(L"[AP lifecycle] object=adapter event=archive-handler-attached adapter=%p handler=%p",
            this,archive_);
        return S_OK;
    }
    HRESULT SelectOriginalForNonArchive(LPCWSTR file) {
        if(originalForFile_){originalForFile_->Release();originalForFile_ = nullptr;}
        if(!file || ! * file)return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        const wchar_t * ext = PathFindExtensionW(file);
        if(!ext || ! * ext)return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        // Never change the behavior of the genuine Windows .txt previewer.
        if(_wcsicmp(ext,L".txt") == 0)return S_OK;
        wchar_t handler[96]{};DWORD chars = _countof(handler);
        const HRESULT found = AssocQueryStringW(static_cast < ASSOCF > (0),
            ASSOCSTR_SHELLEXTENSION,ext,
            L"{8895B1C6-B41F-4C1C-A562-0D564250836F}",handler, & chars);
        if(found != S_OK || !handler[0]){
            // An unrelated binary type that shares an archive ProgID must
            // never fall back to the Windows text handler.
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }
        CLSID clsid{};
        if(FAILED(CLSIDFromString(handler, & clsid)))return E_INVALIDARG;
        if(IsEqualCLSID(clsid,kTextHandler))return S_OK;
        // The original handler may be a third-party COM server; if activation
        // fails, show no preview rather than interpreting binary data as TXT.
        const HRESULT opened = CoCreateInstance(clsid,nullptr,
            CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,IID_IUnknown,
            reinterpret_cast < void *  *  > ( & originalForFile_));
        if(FAILED(opened))
            Wh_Log(L"[AP bridge/prevhost] original handler not available for %s hr=0x%08lX",
                ext,static_cast < unsigned long > (opened));
        return opened;
    }
    void CopyStateToCurrent() {
        if(site_) Call < IObjectWithSite > (IID_IObjectWithSite,[ & ](auto * obj){return obj->SetSite(site_);});
        if(backgroundKnown_) Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetBackgroundColor(background_);});
        if(foregroundKnown_) Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetTextColor(foreground_);});
        if(fontKnown_) Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetFont( & font_);});
        if(windowKnown_) Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->SetWindow(window_, & area_);});
    }
    HRESULT RouteFile(LPCWSTR file,DWORD mode){
        if(stopping.load(std::memory_order_acquire))return CO_E_SERVER_STOPPING;
        if(!file || ! * file)return E_INVALIDARG;
        const bool archive = ArchivePreviewOptions::MatchArchive(file);
        Wh_Log(L"[AP diag/bridge] RouteFile pid=%lu adapter=%p oldArchive=%d initialized=%d newArchive=%d file=%s",
            GetCurrentProcessId(),this,archiveSelected_?1:0,initialized_?1:0,archive?1:0,file);
        if(archive){
            const HRESULT ready = EnsureArchive();if(FAILED(ready))return ready;
        }else{
            const HRESULT ready = SelectOriginalForNonArchive(file);
            if(FAILED(ready))return ready;
        }
        archiveSelected_ = archive;
        initialized_ = true;
        Wh_Log(L"[AP FBX diagnostic] Initialize entering adapter=%p archive=%d file=%s",
            this,archive?1:0,file);
        const HRESULT hr = Call < IInitializeWithFile > (IID_IInitializeWithFile,
            [ & ](auto * obj){return obj->Initialize(file,mode);});
        Wh_Log(L"[AP FBX diagnostic] Initialize returned adapter=%p hr=0x%08lX",
            this,static_cast<unsigned long>(hr));
        if(archive && SUCCEEDED(hr)){
            ++archiveObjects;
            Wh_Log(L"[AP bridge/prevhost] archive selected: %s",file);
            CopyStateToCurrent();
        }
        if(!archive && hr == E_NOINTERFACE){
            IStream * stream = nullptr;
            HRESULT opened = SHCreateStreamOnFileEx(file,STGM_READ | STGM_SHARE_DENY_NONE,
                FILE_ATTRIBUTE_NORMAL,FALSE,nullptr, & stream);
            if(SUCCEEDED(opened) && stream){
                opened = Call < IInitializeWithStream > (IID_IInitializeWithStream,
                    [ & ](auto * obj){return obj->Initialize(stream,mode);});
                stream->Release();
            }
            if(SUCCEEDED(opened))CopyStateToCurrent();
            return opened;
        }
        if(!archive && SUCCEEDED(hr))CopyStateToCurrent();
        return hr;
    }
public:
    explicit PreviewAdapter(IUnknown * native):native_(native){
        ++ArchivePreviewHost::g_objects;
        ++liveAdapters;
        Wh_Log(L"[AP lifecycle] object=adapter event=created adapter=%p tid=%lu",
            this,GetCurrentThreadId());
    }
    ~PreviewAdapter(){
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] object=adapter event=final-release-begin adapter=%p tid=%lu handler=%p initialized=%d archive_active=%d",
            this,GetCurrentThreadId(),archive_,initialized_ ? 1 : 0,archiveSelected_ ? 1 : 0);
        if(archive_)archive_->Release();
        if(originalForFile_)originalForFile_->Release();
        if(native_)native_->Release();
        if(site_)site_->Release();
        Wh_Log(L"[AP lifecycle] object=adapter event=final-release-end adapter=%p elapsed_ms=%llu",
            this,GetTickCount64() - started);
        --liveAdapters;
        --ArchivePreviewHost::g_objects;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void *  * out) override {
        if(!out)return E_POINTER; * out = nullptr;
        if(IsEqualIID(iid,IID_IUnknown) || IsEqualIID(iid,IID_IPreviewHandler))
            * out = static_cast < IPreviewHandler *  > (this);
        else if(IsEqualIID(iid,IID_IInitializeWithFile))
            * out = static_cast < IInitializeWithFile *  > (this);
        else if(IsEqualIID(iid,IID_IInitializeWithStream))
            * out = static_cast < IInitializeWithStream *  > (this);
        else if(IsEqualIID(iid,IID_IInitializeWithItem))
            * out = static_cast < IInitializeWithItem *  > (this);
        else if(IsEqualIID(iid,IID_IObjectWithSite))
            * out = static_cast < IObjectWithSite *  > (this);
        else if(IsEqualIID(iid,IID_IOleWindow))
            * out = static_cast < IOleWindow *  > (this);
        else if(IsEqualIID(iid,IID_IPreviewHandlerVisuals))
            * out = static_cast < IPreviewHandlerVisuals *  > (this);
        if(! * out)return E_NOINTERFACE;
        AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override{return refs_.fetch_add(1) + 1;}
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG next = refs_.fetch_sub(1) - 1;
        if(!next)delete this;
        return next;
    }
    // IInitializeWithFile
    HRESULT STDMETHODCALLTYPE Initialize(LPCWSTR file,DWORD mode) override {
        return RouteFile(file,mode);
    }
    // IInitializeWithStream: the Windows Shell may provide a named file stream.
    HRESULT STDMETHODCALLTYPE Initialize(IStream * stream,DWORD mode) override {
        if(stopping.load(std::memory_order_acquire))return CO_E_SERVER_STOPPING;
        if(!stream)return E_POINTER;
        STATSTG stat{};
        const HRESULT hr = stream->Stat( & stat,STATFLAG_DEFAULT);
        std::wstring path;
        if(SUCCEEDED(hr) && stat.pwcsName)path = stat.pwcsName;
        CoTaskMemFree(stat.pwcsName);
        if(!path.empty() && ArchivePreviewOptions::MatchArchive(path))
            return RouteFile(path.c_str(),mode);
        if(!path.empty()){
            const HRESULT ready = SelectOriginalForNonArchive(path.c_str());
            if(FAILED(ready))return ready;
        }
        Wh_Log(L"[AP bridge/prevhost] stream init: hr=0x%08lX name=%s; using original handler",
            static_cast < unsigned long > (hr),path.empty()?L"(none)":path.c_str());
        archiveSelected_ = false;
        initialized_ = true;
        const HRESULT forwarded = Call < IInitializeWithStream > (IID_IInitializeWithStream,
            [ & ](auto * obj){return obj->Initialize(stream,mode);});
        if(SUCCEEDED(forwarded))CopyStateToCurrent();
        return forwarded;
    }
    // IInitializeWithItem supports direct filesystem paths without a stream name.
    HRESULT STDMETHODCALLTYPE Initialize(IShellItem * item,DWORD mode) override {
        if(stopping.load(std::memory_order_acquire))return CO_E_SERVER_STOPPING;
        if(!item)return E_POINTER;
        PWSTR path = nullptr;
        const HRESULT hr = item->GetDisplayName(SIGDN_FILESYSPATH, & path);
        if(SUCCEEDED(hr) && path && ArchivePreviewOptions::MatchArchive(path)){
            const HRESULT routed = RouteFile(path,mode);
            CoTaskMemFree(path);
            return routed;
        }
        if(SUCCEEDED(hr) && path){
            const HRESULT ready = SelectOriginalForNonArchive(path);
            if(FAILED(ready)){CoTaskMemFree(path);return ready;}
        }
        CoTaskMemFree(path);
        archiveSelected_ = false;
        initialized_ = true;
        HRESULT forwarded = Call < IInitializeWithItem > (IID_IInitializeWithItem,
            [ & ](auto * obj){return obj->Initialize(item,mode);});
        if(forwarded == E_NOINTERFACE){
            IStream * stream = nullptr;
            HRESULT bind = item->BindToHandler(nullptr,BHID_Stream,IID_PPV_ARGS( & stream));
            if(SUCCEEDED(bind) && stream){
                forwarded = Call < IInitializeWithStream > (IID_IInitializeWithStream,
                    [ & ](auto * obj){return obj->Initialize(stream,mode);});
                stream->Release();
            }else forwarded = bind;
        }
        if(SUCCEEDED(forwarded))CopyStateToCurrent();
        return forwarded;
    }
    // IPreviewHandler and IOleWindow
    HRESULT STDMETHODCALLTYPE SetWindow(HWND hwnd,const RECT * rc) override {
        if(!hwnd || !rc)return E_INVALIDARG;
        window_ = hwnd;area_ =  * rc;windowKnown_ = true;
        // Normally Init precedes SetWindow. If not, cache without creating a
        // native TXT window which would remain behind the archive window.
        if(!initialized_)return S_OK;
        return Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->SetWindow(hwnd,rc);});
    }
    HRESULT STDMETHODCALLTYPE SetRect(const RECT * rc) override {
        if(!rc)return E_INVALIDARG;
        area_ =  * rc;
        return Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->SetRect(rc);});
    }
    HRESULT STDMETHODCALLTYPE DoPreview() override {
        Wh_Log(L"[AP diag/bridge] DoPreview pid=%lu adapter=%p archive=%d initialized=%d active=%p window=%p",
            GetCurrentProcessId(),this,archiveSelected_?1:0,initialized_?1:0,Active(),window_);
        const HRESULT result = Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->DoPreview();});
        Wh_Log(L"[AP FBX diagnostic] DoPreview returned adapter=%p archive=%d hr=0x%08lX",
            this,archiveSelected_?1:0,static_cast<unsigned long>(result));
        return result;
    }
    HRESULT STDMETHODCALLTYPE Unload() override {
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] object=adapter event=unload-begin adapter=%p tid=%lu handler=%p hwnd=%p initialized=%d archive_active=%d refs=%lu",
            this,GetCurrentThreadId(),archive_,window_,initialized_ ? 1 : 0,archiveSelected_ ? 1 : 0,
            refs_.load(std::memory_order_relaxed));
        const HRESULT hr = Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->Unload();});
        archiveSelected_ = false;
        initialized_ = false;
        Wh_Log(L"[AP lifecycle] object=adapter event=unload-end adapter=%p elapsed_ms=%llu hr=0x%08lX",
            this,GetTickCount64() - started,static_cast<unsigned long>(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE SetFocus() override {
        return Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->SetFocus();});
    }
    HRESULT STDMETHODCALLTYPE QueryFocus(HWND * hwnd) override {
        return Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->QueryFocus(hwnd);});
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG * msg) override {
        return Call < IPreviewHandler > (IID_IPreviewHandler,
            [ & ](auto * obj){return obj->TranslateAccelerator(msg);});
    }
    HRESULT STDMETHODCALLTYPE GetWindow(HWND * hwnd) override {
        return Call < IOleWindow > (IID_IOleWindow,
            [ & ](auto * obj){return obj->GetWindow(hwnd);});
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL enter) override {
        return Call < IOleWindow > (IID_IOleWindow,
            [ & ](auto * obj){return obj->ContextSensitiveHelp(enter);});
    }
    // IObjectWithSite
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown * site) override {
        if(site)site->AddRef();if(site_)site_->Release();site_ = site;
        return Call < IObjectWithSite > (IID_IObjectWithSite,
            [ & ](auto * obj){return obj->SetSite(site);});
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid,void *  * out) override {
        if(!out)return E_POINTER; * out = nullptr;
        if(site_)return site_->QueryInterface(iid,out);
        return Call < IObjectWithSite > (IID_IObjectWithSite,
            [ & ](auto * obj){return obj->GetSite(iid,out);});
    }
    // IPreviewHandlerVisuals
    HRESULT STDMETHODCALLTYPE SetBackgroundColor(COLORREF c) override {
        backgroundKnown_ = true;background_ = c;
        return Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetBackgroundColor(c);});
    }
    HRESULT STDMETHODCALLTYPE SetTextColor(COLORREF c) override {
        foregroundKnown_ = true;foreground_ = c;
        return Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetTextColor(c);});
    }
    HRESULT STDMETHODCALLTYPE SetFont(const LOGFONTW * lf) override {
        if(!lf)return E_POINTER;
        fontKnown_ = true;font_ =  * lf;
        return Call < IPreviewHandlerVisuals > (IID_IPreviewHandlerVisuals,
            [ & ](auto * obj){return obj->SetFont(lf);});
    }
};

class BridgeFactory final : public IClassFactory {
    std::atomic < ULONG > refs_{1};
    IClassFactory * nativeFactory_{};
public:
    explicit BridgeFactory(IClassFactory * factory):nativeFactory_(factory){
        ++ArchivePreviewHost::g_objects;
        ++liveFactories;
        Wh_Log(L"[AP lifecycle] object=factory event=created factory=%p tid=%lu",
            this,GetCurrentThreadId());
    }
    ~BridgeFactory(){
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] object=factory event=final-release-begin factory=%p tid=%lu",
            this,GetCurrentThreadId());
        if(nativeFactory_)nativeFactory_->Release();
        Wh_Log(L"[AP lifecycle] object=factory event=final-release-end factory=%p elapsed_ms=%llu",
            this,GetTickCount64() - started);
        --liveFactories;
        --ArchivePreviewHost::g_objects;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void *  * out) override {
        if(!out)return E_POINTER; * out = nullptr;
        if(IsEqualIID(iid,IID_IUnknown) || IsEqualIID(iid,IID_IClassFactory))
            * out = static_cast < IClassFactory *  > (this);
        if(! * out)return E_NOINTERFACE;
        AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef()override{return refs_.fetch_add(1) + 1;}
    ULONG STDMETHODCALLTYPE Release()override{
        ULONG next = refs_.fetch_sub(1) - 1;if(!next)delete this;return next;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown * outer,REFIID iid,void *  * out) override {
        if(!out)return E_POINTER; * out = nullptr;
        if(stopping.load(std::memory_order_acquire))return CO_E_SERVER_STOPPING;
        if(outer)return CLASS_E_NOAGGREGATION;
        IUnknown * native = nullptr;
        HRESULT hr = nativeFactory_->CreateInstance(nullptr,IID_IUnknown,
            reinterpret_cast < void *  *  > ( & native));
        if(FAILED(hr) || !native)return FAILED(hr)?hr:E_FAIL;
        auto * adapter = new(std::nothrow) PreviewAdapter(native); // adopts native reference
        if(!adapter){native->Release();return E_OUTOFMEMORY;}
        hr = adapter->QueryInterface(iid,out);
        adapter->Release();
        ++createdObjects;
        if(createdObjects.load() <= 12)
            Wh_Log(L"[AP bridge/prevhost] TXT factory wrapped; IID request hr=0x%08lX",
                   static_cast < unsigned long > (hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL lock) override {
        return nativeFactory_->LockServer(lock);
    }
};

static HRESULT STDAPICALLTYPE DllFactoryHook(REFCLSID clsid,REFIID iid,LPVOID * out){
    if(!originalDllGetClassObject)return E_UNEXPECTED;
    if(!stopping.load() && IsEqualCLSID(clsid,kTextHandler) &&
       (IsEqualIID(iid,IID_IClassFactory) || IsEqualIID(iid,IID_IUnknown))){
        IClassFactory * native = nullptr;
        HRESULT hr = originalDllGetClassObject(clsid,IID_IClassFactory,
            reinterpret_cast < void *  *  > ( & native));
        if(FAILED(hr) || !native)return FAILED(hr)?hr:E_FAIL;
        auto * adapter = new(std::nothrow) BridgeFactory(native);
        if(!adapter){native->Release();return E_OUTOFMEMORY;}
        hr = adapter->QueryInterface(iid,out);
        adapter->Release();
        ++classFactoryHits;
        Wh_Log(L"[AP bridge/prevhost] intercept shell32 TXT COM factory hit=%u hr=0x%08lX",
               classFactoryHits.load(),static_cast < unsigned long > (hr));
        return hr;
    }
    return originalDllGetClassObject(clsid,iid,out);
}

static bool Install(){
    // Require the actual Microsoft text preview server. Do not replace a
    // third-party CLSID registration or arbitrary handler DLL.
    std::wstring clsidPath = L"CLSID\\{1531D583-8375-4D3F-B5FB-D23BBD169F22}\\InprocServer32";
    wchar_t server[1024]{};
    DWORD bytes = sizeof(server),type = 0;
    LONG rc = RegGetValueW(HKEY_CLASSES_ROOT,clsidPath.c_str(),nullptr,
        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, & type,server, & bytes);
    if(rc != ERROR_SUCCESS){
        Wh_Log(L"[AP bridge/prevhost] Windows TXT COM registration missing rc=%ld",rc);
        return false;
    }
    std::wstring name(server);
    if(_wcsicmp(PathFindFileNameW(name.c_str()),L"shell32.dll") != 0){
        Wh_Log(L"[AP bridge/prevhost] TXT server is not shell32.dll: %s; bridge disabled",name.c_str());
        return false;
    }
    HMODULE shell = GetModuleHandleW(L"shell32.dll");
    if(!shell)shell = LoadLibraryExW(L"shell32.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!shell)return false;
    auto target = GetProcAddress(shell,"DllGetClassObject");
    if(!target || !Wh_SetFunctionHook(reinterpret_cast < void *  > (target),
        reinterpret_cast < void *  > ( & DllFactoryHook),
        reinterpret_cast < void *  *  > ( & originalDllGetClassObject))){
        Wh_Log(L"[AP bridge/prevhost] shell32 DllGetClassObject hook unavailable");
        return false;
    }
    Wh_Log(L"[AP bridge/prevhost] TXT bridge hook installed, no registry writes");
    return true;
}
} // namespace ArchivePreviewBridge


static bool IsExplorer(){
    wchar_t path[MAX_PATH]{};
    if(!GetModuleFileNameW(nullptr,path,MAX_PATH))return false;
    const wchar_t * name = wcsrchr(path,L'\\');name = name?name + 1:path;
    return _wcsicmp(name,L"explorer.exe") == 0;
}
static bool IsPrevhost(){
    wchar_t path[MAX_PATH]{};
    if(!GetModuleFileNameW(nullptr,path,MAX_PATH))return false;
    const wchar_t * name = wcsrchr(path,L'\\');name = name?name + 1:path;
    return _wcsicmp(name,L"prevhost.exe") == 0;
}

static bool HasWindowClass(HWND window, PCWSTR expected) {
    wchar_t name[64]{};
    return GetClassNameW(window, name, _countof(name)) != 0 &&
           wcscmp(name, expected) == 0;
}

static bool IsWindowsPreviewHost(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[MAX_PATH]{};
    DWORD length = _countof(path);
    const BOOL found = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!found) return false;

    wchar_t windows[MAX_PATH]{};
    const UINT prefixLength = GetWindowsDirectoryW(windows, _countof(windows));
    return prefixLength > 0 && prefixLength < _countof(windows) &&
           length > prefixLength &&
           _wcsnicmp(path, windows, prefixLength) == 0 &&
           _wcsicmp(path + prefixLength, L"\\System32\\prevhost.exe") == 0;
}

static BOOL CALLBACK FindArchivePreviewChild(HWND child, LPARAM data) {
    if (!HasWindowClass(child, ArchivePreviewHost::kWindowClass)) return TRUE;
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(child, &pid) || !IsWindowsPreviewHost(pid))
        return TRUE;
    *reinterpret_cast<HWND*>(data) = child;
    return FALSE;
}

static BOOL CALLBACK CloseExplorerWindowWithPreview(HWND window, LPARAM data) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId() ||
        (!HasWindowClass(window, L"CabinetWClass") &&
         !HasWindowClass(window, L"ExploreWClass")))
        return TRUE;

    HWND preview = nullptr;
    EnumChildWindows(window, FindArchivePreviewChild, reinterpret_cast<LPARAM>(&preview));
    if (!preview || GetAncestor(preview, GA_ROOT) != window) return TRUE;

    // Let Explorer handle the normal close on its UI thread.
    if (PostMessageW(window, WM_CLOSE, 0, 0)) {
        ++*reinterpret_cast<unsigned*>(data);
        Wh_Log(L"[AP lifecycle] phase=preview-window-close-queued window=%p", window);
    } else {
        Wh_Log(L"[AP lifecycle] phase=preview-window-close-failed window=%p win32=%lu",
               window, GetLastError());
    }
    return TRUE;
}

static void RequestArchivePreviewWindowClosure() {
    unsigned queued = 0;
    EnumWindows(CloseExplorerWindowWithPreview, reinterpret_cast<LPARAM>(&queued));
    Wh_Log(L"[AP lifecycle] phase=preview-window-close-scan queued=%u", queued);
}

static DWORD WindowsBuildForLog() {
    using GetVersionFn = LONG (NTAPI*)(OSVERSIONINFOW*);
    const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return 0;
    const auto getVersion = reinterpret_cast<GetVersionFn>(GetProcAddress(ntdll,"RtlGetVersion"));
    OSVERSIONINFOW version{};
    version.dwOSVersionInfoSize = sizeof(version);
    return getVersion && getVersion(&version) == 0 ? version.dwBuildNumber : 0;
}
static std::atomic<ULONGLONG> shutdownRequestedAt{0};
static void LogShutdownState(PCWSTR phase, ULONGLONG elapsed) {
    Wh_Log(L"[AP lifecycle] phase=%s elapsed_ms=%llu tid=%lu objects=%ld adapters=%ld archive_handlers=%ld factories=%ld drag_interfaces=%u",
        phase,elapsed,GetCurrentThreadId(),ArchivePreviewHost::g_objects.load(),
        ArchivePreviewBridge::liveAdapters.load(),ArchivePreviewHost::g_liveArchiveHandlers.load(),
        ArchivePreviewBridge::liveFactories.load(),ArchivePreviewBroker::g_externalDragInterfaces.load());
}
BOOL Wh_ModInit(){
    ArchivePreviewOptions::Load();
    ArchivePreviewDiagnostics::Load();
    ArchivePreviewColumns::Load();
    Wh_Log(L"[AP lifecycle] phase=init version=%s pid=%lu windows_build=%lu mod_arch=x64 windows_ui_lang=0x%04X mod_lang=%s",
        WH_MOD_VERSION,GetCurrentProcessId(),WindowsBuildForLog(),
        static_cast<unsigned>(GetUserDefaultUILanguage()),ArchivePreviewText::LanguageTag());
    HMODULE module{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast < LPCWSTR > ( & Wh_ModInit), & module))return FALSE;
    ArchivePreviewHost::g_module = module;
    ArchivePreviewBroker::g_module = module;
    if(IsExplorer()){
        ArchivePreviewSecurityPolicy::InstallHook();
        // Every Explorer process must install its own association hooks.
        // Broker ownership is a session-wide singleton and must not gate them.
        if(!ArchivePreviewVirtual::Install()){
            Wh_Log(L"[AP bridge/explorer] association hooks unavailable");
            return FALSE;
        }
        // Start the broker only when an archive preview association is queried.
        // Starting synchronously here can stall Explorer during sign-in.
        Wh_Log(L"[AP bridge/explorer] virtual associations active; broker starts on demand, no registry writes");
    }else if(IsPrevhost()){
        Wh_Log(L"[AP bridge/prevhost] Windhawk injected into Preview Host, installing bridge");
        if(!ArchivePreviewBridge::Install()){
            Wh_Log(L"[AP bridge/prevhost] bootstrap hook not installed");
            return FALSE;
        }
    }
    return TRUE;
}
void Wh_ModSettingsChanged(){
    ArchivePreviewOptions::Load();
    ArchivePreviewDiagnostics::Load();
    ArchivePreviewColumns::Load(true);
    Wh_Log(L"[AP lifecycle] phase=settings-changed mod_lang=%s",ArchivePreviewText::LanguageTag());
    if(IsExplorer()){
        ArchivePreviewVirtual::RefreshArchiveProgIds();
        HWND manager = ArchivePreviewBroker::g_gripManager.load();
        if(manager && IsWindow(manager))
            PostMessageW(manager,ArchivePreviewBroker::WM_BROWSER_SETTINGS_CHANGED,0,0);
        Wh_Log(L"[AP bridge/explorer] options changed, associations remain virtual");
    }
}
// Windhawk runs this callback while the hooks are still installed. Stop new
// COM wrappers before the engine detaches the hooks, instead of only stopping
// after that point in Wh_ModUninit. Existing host references remain valid.
void Wh_ModBeforeUninit(){
    ArchivePreviewHost::g_hostShutdown.store(true,std::memory_order_release);
    ArchivePreviewRuntime::g_stopping.store(true,std::memory_order_release);
    ArchivePreviewVirtual::g_stopping.store(true,std::memory_order_release);
    ArchivePreviewBridge::stopping.store(true,std::memory_order_release);
    shutdownRequestedAt.store(GetTickCount64(),std::memory_order_release);
    LogShutdownState(L"before-uninit",0);
    if (ArchivePreviewOptions::closeExplorerWindowOnUnload.load() && IsExplorer())
        RequestArchivePreviewWindowClosure();
}
// Windhawk performs FreeLibrary after this callback returns. Never pin the
// module or return while COM can still invoke an object/factory vtable in it.
// The Preview Host owns those references, not Windhawk: only their owners can
// release them. The deliberate unbounded wait follows Windhawk's unload policy.
static void WaitForPreviewComRelease(){
    const ULONGLONG started = GetTickCount64();
    ULONGLONG lastLog = started;
    LogShutdownState(L"com-wait-begin",0);
    for(;;){
        const long objects = ArchivePreviewHost::g_objects.load(std::memory_order_acquire);
        if(objects == 0)break;
        const ULONGLONG now = GetTickCount64();
        if(now - lastLog >= 5000){
            LogShutdownState(L"com-wait",now - started);
            lastLog = now;
        }
        // Do not force Release() of a COM reference owned by the host.
        // This may block indefinitely if Windows never releases its objects.
        Sleep(100);
    }
    LogShutdownState(L"com-drained",GetTickCount64() - started);
}
// The COM object count alone is not enough if a Windows HWND outlives its
// owner. A class still registered with one of our window procs must not be
// left behind when Windhawk releases the DLL.
static void UnregisterPreviewClassesSafely(){
    const wchar_t * const classes[] = {
        ArchivePreviewHost::kToolbarClass,
        ArchivePreviewHost::kWindowClass,
    };
    for(const wchar_t * name:classes){
        const ULONGLONG started = GetTickCount64();
        Wh_Log(L"[AP lifecycle] phase=unregister-window-class-begin class=%s",name);
        ULONGLONG lastLog = 0;
        for(;;){
            if(UnregisterClassW(name,ArchivePreviewHost::g_module))break;
            const DWORD error = GetLastError();
            if(error == ERROR_CLASS_DOES_NOT_EXIST)break;
            const ULONGLONG now = GetTickCount64();
            if(lastLog == 0 || now - lastLog >= 5000){
                Wh_Log(L"[AP lifecycle] phase=unregister-window-class-wait class=%s win32=%lu elapsed_ms=%llu",
                    name,error,now - started);
                lastLog = now;
            }
            Sleep(100);
        }
        Wh_Log(L"[AP lifecycle] phase=unregister-window-class-end class=%s elapsed_ms=%llu",
            name,GetTickCount64() - started);
    }
}
void Wh_ModUninit(){
    const ULONGLONG started = GetTickCount64();
    const ULONGLONG requested = shutdownRequestedAt.load(std::memory_order_acquire);
    ArchivePreviewHost::g_hostShutdown.store(true,std::memory_order_release);
    ArchivePreviewRuntime::g_stopping.store(true,std::memory_order_release);
    ArchivePreviewVirtual::g_stopping.store(true,std::memory_order_release);
    ArchivePreviewBridge::stopping.store(true,std::memory_order_release);
    LogShutdownState(L"uninit-begin",requested && requested <= started ? started - requested : 0);
    if(IsExplorer()){
        Wh_Log(L"[AP bridge/explorer] shutdown virtualQueries=%u regQueries=%u",
            ArchivePreviewVirtual::g_queryVirtualHits.load(),
            ArchivePreviewVirtual::g_registryHits.load());
        const ULONGLONG brokerStarted = GetTickCount64();
        LogShutdownState(L"broker-stop-begin",0);
        ArchivePreviewRuntime::StopService();
        LogShutdownState(L"broker-stopped",GetTickCount64() - brokerStarted);
    }
    if(IsPrevhost()){
        Wh_Log(L"[AP bridge/prevhost] shutdown factories=%u objects=%u archives=%u",
            ArchivePreviewBridge::classFactoryHits.load(),
            ArchivePreviewBridge::createdObjects.load(),
            ArchivePreviewBridge::archiveObjects.load());
    }
    // The managed background workers cannot execute mod code after this join.
    // The COM object references below are a separate issue: prevhost may own
    // those references and the DLL cannot release them unilaterally.
    ArchivePreviewWorkerLifetime::JoinAll();
    ArchivePreviewIcons::LogStats(L"shutdown");
    // Once the last host object and factory have been released, no preview
    // callback can safely start; our COM creation paths were stopped in
    // Wh_ModBeforeUninit. Apply the same safety check in every injected process.
    WaitForPreviewComRelease();
    UnregisterPreviewClassesSafely();
    LogShutdownState(L"uninit-end",GetTickCount64() - started);
}
