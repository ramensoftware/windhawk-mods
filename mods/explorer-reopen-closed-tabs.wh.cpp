// ==WindhawkMod==
// @id              explorer-reopen-closed-tabs
// @name            Explorer Tab History (Shift+Tab) & Reopen Closed Tabs (Ctrl+Shift+T)
// @name:vi-VN      Lịch sử Tab Explorer (Shift+Tab) & Khôi phục Tab đã đóng (Ctrl+Shift+T)
// @name:en-US      Explorer Tab History (Shift+Tab) & Reopen Closed Tabs (Ctrl+Shift+T)
// @name:zh-CN      资源管理器标签历史 (Shift+Tab) 与恢复关闭的标签 (Ctrl+Shift+T)
// @name:zh-TW      檔案總管分頁歷史 (Shift+Tab) 與復原已關閉分頁 (Ctrl+Shift+T)
// @name:ja-JP      エクスプローラーのタブ履歴 (Shift+Tab) と閉じたタブの復元 (Ctrl+Shift+T)
// @name:ko-KR      파일 탐색기 탭 기록 (Shift+Tab) 및 닫은 탭 다시 열기 (Ctrl+Shift+T)
// @name:es-ES      Historial de pestañas del Explorador (Shift+Tab) y reabrir cerradas (Ctrl+Shift+T)
// @name:fr-FR      Historique des onglets de l'Explorateur (Shift+Tab) et rouvrir (Ctrl+Shift+T)
// @name:de-DE      Explorer-Tab-Verlauf (Shift+Tab) & geschlossene Tabs öffnen (Ctrl+Shift+T)
// @name:ru-RU      История вкладок Проводника (Shift+Tab) и восстановление (Ctrl+Shift+T)
// @name:pt-BR      Histórico de abas do Explorer (Shift+Tab) e reabrir fechadas (Ctrl+Shift+T)
// @name:it-IT      Cronologia schede Esplora file (Shift+Tab) e riapertura (Ctrl+Shift+T)
// @name:ar-SA      سجل علامات تبويب مستكشف الملفات (Shift+Tab) وإعادة فتح المغلقة (Ctrl+Shift+T)
// @name:hi-IN      एक्सप्लोरर टैब इतिहास (Shift+Tab) और बंद टैब फिर से खोलें (Ctrl+Shift+T)
// @name:id-ID      Riwayat Tab Explorer (Shift+Tab) & Buka Kembali Tab Tertutup (Ctrl+Shift+T)
// @name:ms-MY      Sejarah Tab Explorer (Shift+Tab) & Buka Semula Tab Ditutup (Ctrl+Shift+T)
// @name:th-TH      ประวัติแท็บ File Explorer (Shift+Tab) และกู้คืนแท็บที่ปิด (Ctrl+Shift+T)
// @name:tr-TR      Gezgin Sekme Geçmişi (Shift+Tab) ve Kapatılan Sekmeleri Açma (Ctrl+Shift+T)
// @name:pl-PL      Historia kart Eksploratora (Shift+Tab) i przywracanie zamkniętych (Ctrl+Shift+T)
// @name:nl-NL      Verkenner-tabbladgeschiedenis (Shift+Tab) & gesloten heropenen (Ctrl+Shift+T)
// @name:uk-UA      Історія вкладок Провідника (Shift+Tab) та відновлення закритих (Ctrl+Shift+T)
// @name:cs-CZ      Historie karet Průzkumníka (Shift+Tab) a obnovení zavřených (Ctrl+Shift+T)
// @name:sv-SE      Flikhistorik i Utforskaren (Shift+Tab) & återöppna stängda (Ctrl+Shift+T)
// @name:ro-RO      Istoric file Explorer (Shift+Tab) și redeschidere file închise (Ctrl+Shift+T)
// @name:el-GR      Ιστορικό καρτελών Εξερεύνησης (Shift+Tab) & επαναφορά κλειστών (Ctrl+Shift+T)
// @name:he-IL      היסטוריית כרטיסיות סייר הקבצים (Shift+Tab) ושחזור כרטיסיות שנסגרו (Ctrl+Shift+T)
// @description     Reopen closed Explorer tabs/windows with Ctrl+Shift+T and open a modern WinUI 3 Tab History table with Shift+Tab (Windows 11 & Windows 10, 26 languages)
// @description:vi-VN Khôi phục tab/cửa sổ Explorer đã đóng bằng Ctrl+Shift+T và mở bảng Lịch sử mở Tab giao diện WinUI 3 hiện đại bằng Shift+Tab (hỗ trợ Windows 11 & 10, 26 ngôn ngữ)
// @description:zh-CN 使用 Ctrl+Shift+T 恢复已关闭的资源管理器标签页/窗口，并使用 Shift+Tab 打开现代 WinUI 3 风格的标签历史记录表格（支持 Win11 和 Win10）
// @description:zh-TW 使用 Ctrl+Shift+T 復原已關閉的檔案總管分頁/視窗，並使用 Shift+Tab 開啟現代 WinUI 3 風格的分頁歷史記錄表格（支援 Win11 與 Win10）
// @description:ja-JP Ctrl+Shift+T で閉じたエクスプローラーのタブやウィンドウを復元し、Shift+Tab で WinUI 3 風のタブ履歴テーブルを表示します（Win11/Win10 対応）
// @description:ko-KR Ctrl+Shift+T로 닫은 파일 탐색기 탭/창을 다시 열고 Shift+Tab으로 모던 WinUI 3 스타일의 탭 기록 표를 엽니다 (Win11 및 Win10 지원)
// @description:es-ES Reabre pestañas o ventanas cerradas del Explorador con Ctrl+Shift+T y abre una tabla moderna WinUI 3 de historial con Shift+Tab (Win11 y Win10)
// @description:fr-FR Rouvrez les onglets/fenêtres fermés de l'Explorateur avec Ctrl+Shift+T et affichez l'historique WinUI 3 avec Shift+Tab (Win11 et Win10)
// @description:de-DE Geschlossene Explorer-Tabs/-Fenster mit Strg+Umschalt+T wiederherstellen und modernen WinUI 3-Tab-Verlauf mit Umschalt+Tab öffnen (Win11 & Win10)
// @description:ru-RU Восстанавливайте закрытые вкладки/окна Проводника через Ctrl+Shift+T и открывайте таблицу истории в стиле WinUI 3 по Shift+Tab (Win11 и Win10)
// @description:pt-BR Reabra abas/janelas fechadas do Explorer com Ctrl+Shift+T e abra a tabela de histórico WinUI 3 com Shift+Tab (suporta Win11 e Win10)
// @description:it-IT Riapri le schede/finestre chiuse di Esplora file con Ctrl+Shift+T e apri la tabella cronologia WinUI 3 con Shift+Tab (Win11 e Win10)
// @description:ar-SA إعادة فتح علامات التبويب/النوافذ المغلقة عبر Ctrl+Shift+T وفتح جدول سجل التبويبات بتصميم WinUI 3 الحديث عبر Shift+Tab (يدعم Win11 و Win10)
// @description:hi-IN Ctrl+Shift+T से बंद एक्सप्लोरर टैब/विंडो फिर से खोलें और Shift+Tab से आधुनिक WinUI 3 टैब इतिहास तालिका खोलें (Win11 और Win10 समर्थित)
// @description:id-ID Buka kembali tab/jendela Explorer yang ditutup dengan Ctrl+Shift+T dan tampilkan tabel riwayat tab WinUI 3 dengan Shift+Tab (Win11 & Win10)
// @description:ms-MY Buka semula tab/tetingkap Explorer yang ditutup dengan Ctrl+Shift+T dan buka jadual sejarah tab WinUI 3 dengan Shift+Tab (Win11 & Win10)
// @description:th-TH กู้คืนแท็บหรือหน้าต่าง Explorer ที่ปิดด้วย Ctrl+Shift+T และเปิดตารางประวัติแท็บสไตล์ WinUI 3 ด้วย Shift+Tab (รองรับทั้ง Win11 และ Win10)
// @description:tr-TR Kapatılan Gezgin sekmelerini/pencerelerini Ctrl+Shift+T ile yeniden açın ve Shift+Tab ile modern WinUI 3 sekme geçmişi tablosunu görüntüleyin
// @description:pl-PL Przywracaj zamknięte karty/okna Eksploratora skrótem Ctrl+Shift+T i otwieraj tabelę historii WinUI 3 skrótem Shift+Tab (Win11 i Win10)
// @description:nl-NL Heropen gesloten Verkenner-tabbladen/vensters met Ctrl+Shift+T en open de moderne WinUI 3-geschiedenistabel met Shift+Tab (Win11 & Win10)
// @description:uk-UA Відновлюйте закриті вкладки/вікна Провідника за допомогою Ctrl+Shift+T та відкривайте таблицю історії WinUI 3 через Shift+Tab (Win11 і Win10)
// @description:cs-CZ Obnovte zavřené karty/okna Průzkumníka pomocí Ctrl+Shift+T a otevřete tabulku historie ve stylu WinUI 3 pomocí Shift+Tab (Win11 a Win10)
// @description:sv-SE Återöppna stängda flikar/fönster i Utforskaren med Ctrl+Shift+T och öppna en modern WinUI 3-historiktabell med Shift+Tab (Win11 & Win10)
// @description:ro-RO Redeschideți filele/ferestrele Explorer închise cu Ctrl+Shift+T și deschideți tabelul de istoric WinUI 3 cu Shift+Tab (Win11 și Win10)
// @description:el-GR Επαναφέρετε κλειστές καρτέλες/παράθυρα με Ctrl+Shift+T και ανοίξτε τον πίνακα ιστορικού WinUI 3 με Shift+Tab (υποστηρίζει Win11 & Win10)
// @description:he-IL שחזר כרטיסיות או חלונות שנסגרו בסייר הקבצים עם Ctrl+Shift+T ופתח טבלת היסטוריה מודרנית בסגנון WinUI 3 עם Shift+Tab (נתמך ב-Win11 ו-Win10)
// @version         2.0.202
// @author          caotu2005
// @github          https://github.com/caotu2005
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lshell32 -lshlwapi -luuid -lgdi32 -lgdiplus -ldwmapi -luxtheme -limm32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lịch sử Tab Explorer (`Shift+Tab`) & Khôi phục Tab đã đóng (`Ctrl+Shift+T`)

Mod nâng cấp **File Explorer** trên cả **Windows 11** và **Windows 10** với giao diện **WinUI 3** hiện đại và hỗ trợ **26 ngôn ngữ phổ biến trên thế giới**:

## Điểm mới ở phiên bản 2.0.202
- **Khắc phục triệt để lỗi treo `Uninitializing...` khi cập nhật hoặc tắt mod**: Loại bỏ hoàn toàn việc hook các hàm chặn luồng (`GetMessageW` / `DispatchMessageW`), thay bằng cơ chế bắt sự kiện không chặn (`TranslateAcceleratorW` / `IsDialogMessageW` / `TranslateMessage`) kết hợp đánh thức luồng tức thì (`PostThreadMessageW`), giúp Windhawk nạp lại hoặc gỡ mod trong tích tắc.
- **Đa ngôn ngữ (26 ngôn ngữ)**: Tự động nhận diện ngôn ngữ hệ điều hành hoặc chọn trực tiếp trong phần Cài đặt (Tiếng Việt, English, 简体中文, 繁體中文, 日本語, 한국어, Español, Français, Deutsch, Русский, Português, Italiano, العربية, हिन्दी, Bahasa Indonesia, Bahasa Melayu, ไทย, Türkçe, Polski, Nederlands, Українська, Čeština, Svenska, Română, Ελληνικά, עברית).
- **Ô tìm kiếm chuẩn Win32 Edit (TSF/IME)**: Có con trỏ nhấp nháy trực quan và gõ tiếng Việt mượt mà trên cả bàn phím Tiếng Việt mặc định của Windows lẫn UniKey/EVKey.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Language: "auto"
  $name: Language / Ngôn ngữ hiển thị
  $description: Chọn ngôn ngữ cho bảng Lịch sử Tab (Mặc định Tự động theo Windows / Auto-detect system language).
  $options:
  - "auto": Auto (Tự động theo hệ thống)
  - "vi": Tiếng Việt (Vietnamese)
  - "en": English
  - "zh-CN": 简体中文 (Chinese Simplified)
  - "zh-TW": 繁體中文 (Chinese Traditional)
  - "ja": 日本語 (Japanese)
  - "ko": 한국어 (Korean)
  - "es": Español (Spanish)
  - "fr": Français (French)
  - "de": Deutsch (German)
  - "ru": Русский (Russian)
  - "pt": Português (Portuguese)
  - "it": Italiano (Italian)
  - "ar": العربية (Arabic)
  - "hi": हिन्दी (Hindi)
  - "id": Bahasa Indonesia (Indonesian)
  - "ms": Bahasa Melayu (Malay)
  - "th": ไทย (Thai)
  - "tr": Türkçe (Turkish)
  - "pl": Polski (Polish)
  - "nl": Nederlands (Dutch)
  - "uk": Українська (Ukrainian)
  - "cs": Čeština (Czech)
  - "sv": Svenska (Swedish)
  - "ro": Română (Romanian)
  - "el": Ελληνικά (Greek)
  - "he": עברית (Hebrew)
- ReopenHotkey: "ctrl+shift+84"
  $name: Phím tắt khôi phục tab đã đóng (Mặc định Ctrl+Shift+T)
  $description: Mặc định là ctrl+shift+84 (Ctrl+Shift+T). Có thể nhập dạng "Ctrl+Shift+T" hoặc mã VK.
  #! $format: hotkey
- EnableShiftTabHistory: true
  $name: Mở Bảng lịch sử Tab (WinUI 3) bằng phím tắt (Mặc định Shift+Tab)
  $description: Khi bật, nhấn Shift+Tab trong Explorer sẽ mở giao diện bảng lịch sử 4 cột (giữ nguyên Ctrl+Tab để chuyển nhanh giữa các tab).
- HistoryPanelHotkey: "shift+9"
  $name: Phím tắt mở Bảng lịch sử Tab (Mặc định Shift+Tab)
  $description: Mặc định là shift+9 (VK_TAB = 9).
  #! $format: hotkey
- MaxHistory: 100
  $name: Số lượng mục lưu tối đa trong lịch sử
  $description: Giới hạn số lượng tab/thư mục lưu trong bảng lịch sử và danh sách khôi phục (10 - 500).
  #! $min: 10
  #! $max: 500
- DeduplicateHistory: true
  $name: Gộp các thư mục trùng nhau theo thời gian mở gần nhất
  $description: Nếu bật, khi mở lại một thư mục đã có trong bảng lịch sử, mục đó sẽ được cập nhật thời gian mở gần đây nhất và đưa lên đầu bảng (STT = 1).
- Win10OpenInNewWindow: false
  $name: Windows 10 - Mở mục từ lịch sử trong cửa sổ Explorer mới
  $description: Trên Windows 10 (không có tab gốc), chọn bật để mở trong cửa sổ Explorer mới hoặc tắt để điều hướng ngay trong cửa sổ hiện tại.
- PerWindowClosedHistory: false
  $name: Chỉ khôi phục tab đã đóng thuộc cùng cửa sổ (Ctrl+Shift+T)
  $description: Nếu tắt, bạn có thể khôi phục cả các tab từ những cửa sổ Explorer đã đóng hoàn toàn trước đó.
- ThemeMode: "auto"
  $name: Giao diện màu sắc (WinUI 3 Theme)
  $description: Chọn chế độ màu cho hộp thoại Lịch sử Tab.
  $options:
  - "auto": Tự động theo Windows (Dark / Light)
  - "dark": Luôn dùng giao diện Tối (WinUI 3 Dark)
  - "light": Luôn dùng giao diện Sáng (WinUI 3 Light)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <gdiplus.h>
#include <imm.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <uxtheme.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef WM_GETISHELLBROWSER
#define WM_GETISHELLBROWSER (WM_USER + 7)
#endif

// Các hằng số DWM tương thích cho cả Windows 10 và Windows 11
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_OLD
#define DWMWA_USE_IMMERSIVE_DARK_MODE_OLD 19
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE_NEW
#define DWMWA_USE_IMMERSIVE_DARK_MODE_NEW 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE_VAL
#define DWMWA_WINDOW_CORNER_PREFERENCE_VAL 33
#endif
#ifndef DWMWA_BORDER_COLOR_VAL
#define DWMWA_BORDER_COLOR_VAL 34
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE_VAL
#define DWMWA_SYSTEMBACKDROP_TYPE_VAL 38
#endif

constexpr WPARAM IDM_EXPLORER_NEW_TAB = 0xA21B;

constexpr UINT_PTR IDT_RESTORE_TAB_NAVIGATE = 0x57485254;
constexpr UINT RESTORE_TIMER_INTERVAL_MS = 35;
constexpr int MAX_RESTORE_TICKS = 30;

constexpr UINT WM_APP_SHOW_HISTORY_UI = WM_APP + 0x341;
constexpr UINT WM_APP_TRIGGER_REOPEN = WM_APP + 0x342;
constexpr UINT WM_APP_DEFERRED_CLOSE_CHECK = WM_APP + 0x343;

constexpr int IDC_HISTORY_SEARCH_EDIT = 1001;
constexpr PCWSTR HISTORY_DIALOG_CLASS_NAME = L"WindhawkWinUI3ExplorerHistoryDlg";

// Cờ nguyên tử và mutex toàn cục được khai báo trước mọi hàm sử dụng
std::atomic<bool> g_uninitializing{false};
std::mutex g_mutex;

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ===========================================================================
// BẢNG DỊCH ĐA NGÔN NGỮ (26 NGÔN NGỮ PHỔ BIẾN TRÊN THẾ GIỚI)
// ===========================================================================
struct LocalizedStrings {
    PCWSTR code;
    PCWSTR windowTitle;
    PCWSTR subtitleFormat; // Chứa %d cho số lượng mục
    PCWSTR searchPlaceholder;
    PCWSTR colIndex;
    PCWSTR colName;
    PCWSTR colPath;
    PCWSTR colRecentTime;
    PCWSTR emptyHistory;
    PCWSTR emptySearchResult;
    PCWSTR btnDeleteOne;
    PCWSTR btnClearAll;
    PCWSTR btnOpenSameTab;
    PCWSTR btnOpenNewTab;
};

constexpr LocalizedStrings kTranslations[] = {
    // 1. Tiếng Việt (vi)
    {L"vi",
     L"Lịch sử mở các Tab Explorer",
     L"%d mục  •  Shift+Tab / Lên / Xuống: chọn  •  Enter: mở Tab  •  Shift+Enter: mở tại chỗ",
     L"Gõ để lọc theo tên hoặc đường dẫn...",
     L"STT",
     L"Tên folder (hoặc tên file,..)",
     L"Đường dẫn",
     L"Thời gian mở gần đây nhất",
     L"Chưa có lịch sử tab nào được ghi nhận.",
     L"Không tìm thấy thư mục nào khớp với từ khóa tìm kiếm.",
     L"Xóa mục chọn (Ctrl+Del)",
     L"Xóa toàn bộ lịch sử",
     L"Mở tại Tab hiện tại (Shift+Enter)",
     L"Mở trong Tab / Cửa sổ mới (Enter)"},

    // 2. English (en)
    {L"en",
     L"Explorer Tab History",
     L"%d items  •  Shift+Tab / Up / Down: select  •  Enter: new Tab  •  Shift+Enter: current Tab",
     L"Type to filter by name or path...",
     L"#",
     L"Folder / Item Name",
     L"Full Path",
     L"Last Opened Time",
     L"No tab history has been recorded yet.",
     L"No folders match your search query.",
     L"Delete Selected (Ctrl+Del)",
     L"Clear All History",
     L"Open in Current Tab (Shift+Enter)",
     L"Open in New Tab / Window (Enter)"},

    // 3. Chinese Simplified - 简体中文 (zh-CN)
    {L"zh-CN",
     L"文件资源管理器标签页历史记录",
     L"%d 项  •  Shift+Tab / 上下键: 选择  •  Enter: 新建标签页  •  Shift+Enter: 当前页打开",
     L"输入名称或路径进行筛选...",
     L"序号",
     L"文件夹 / 项目名称",
     L"完整路径",
     L"最近打开时间",
     L"暂无已记录的标签页历史。",
     L"未找到与搜索词匹配的文件夹。",
     L"删除所选 (Ctrl+Del)",
     L"清空全部历史",
     L"在当前标签页打开 (Shift+Enter)",
     L"在新标签页/窗口打开 (Enter)"},

    // 4. Chinese Traditional - 繁體中文 (zh-TW)
    {L"zh-TW",
     L"檔案總管分頁歷史記錄",
     L"%d 項  •  Shift+Tab / 上下鍵: 選擇  •  Enter: 新分頁開啟  •  Shift+Enter: 目前分頁開啟",
     L"輸入名稱或路徑以篩選...",
     L"序號",
     L"資料夾 / 項目名稱",
     L"完整路徑",
     L"最近開啟時間",
     L"尚無已記錄的分頁歷史。",
     L"找不到符合搜尋條件的資料夾。",
     L"刪除所選 (Ctrl+Del)",
     L"清除全部歷史",
     L"在目前分頁開啟 (Shift+Enter)",
     L"在新分頁/視窗開啟 (Enter)"},

    // 5. Japanese - 日本語 (ja)
    {L"ja",
     L"エクスプローラーのタブ履歴",
     L"%d 件  •  Shift+Tab / 上下キー: 選択  •  Enter: 新しいタブ  •  Shift+Enter: 現在のタブ",
     L"名前またはパスでフィルタリング...",
     L"番号",
     L"フォルダー / 項目名",
     L"パス",
     L"最終アクセス日時",
     L"タブの履歴はまだ記録されていません。",
     L"検索条件に一致するフォルダーが見つかりません。",
     L"選択項目を削除 (Ctrl+Del)",
     L"履歴をすべて消去",
     L"現在のタブで開く (Shift+Enter)",
     L"新しいタブ / ウィンドウで開く (Enter)"},

    // 6. Korean - 한국어 (ko)
    {L"ko",
     L"파일 탐색기 탭 기록",
     L"%d개 항목  •  Shift+Tab / 위/아래: 선택  •  Enter: 새 탭  •  Shift+Enter: 현재 탭",
     L"이름 또는 경로로 검색...",
     L"번호",
     L"폴더 / 항목 이름",
     L"전체 경로",
     L"최근 연 시간",
     L"기록된 탭 내역이 아직 없습니다.",
     L"검색어와 일치하는 폴더가 없습니다.",
     L"선택 항목 삭제 (Ctrl+Del)",
     L"전체 기록 지우기",
     L"현재 탭에서 열기 (Shift+Enter)",
     L"새 탭 / 창에서 열기 (Enter)"},

    // 7. Spanish - Español (es)
    {L"es",
     L"Historial de pestañas del Explorador",
     L"%d elementos  •  Shift+Tab / Arriba/Abajo: elegir  •  Enter: nueva pestaña  •  Shift+Enter: actual",
     L"Escriba para filtrar por nombre o ruta...",
     L"N.º",
     L"Nombre de carpeta / elemento",
     L"Ruta completa",
     L"Última apertura",
     L"Aún no se ha registrado historial de pestañas.",
     L"Ninguna carpeta coincide con su búsqueda.",
     L"Eliminar selección (Ctrl+Del)",
     L"Borrar todo el historial",
     L"Abrir en pestaña actual (Shift+Enter)",
     L"Abrir en nueva pestaña / ventana (Enter)"},

    // 8. French - Français (fr)
    {L"fr",
     L"Historique des onglets de l'Explorateur",
     L"%d éléments  •  Shift+Tab / Haut/Bas : choisir  •  Entrée : nouvel onglet  •  Shift+Entrée : actuel",
     L"Tapez pour filtrer par nom ou chemin...",
     L"N°",
     L"Nom du dossier / élément",
     L"Chemin complet",
     L"Dernière ouverture",
     L"Aucun historique d'onglet enregistré pour le moment.",
     L"Aucun dossier ne correspond à votre recherche.",
     L"Supprimer la sélection (Ctrl+Suppr)",
     L"Effacer tout l'historique",
     L"Ouvrir dans l'onglet actif (Shift+Entrée)",
     L"Nouvel onglet / fenêtre (Entrée)"},

    // 9. German - Deutsch (de)
    {L"de",
     L"Explorer-Tab-Verlauf",
     L"%d Einträge  •  Shift+Tab / Auf/Ab: Auswahl  •  Enter: neuer Tab  •  Shift+Enter: aktueller Tab",
     L"Nach Name oder Pfad filtern...",
     L"Nr.",
     L"Ordner- / Elementname",
     L"Vollständiger Pfad",
     L"Zuletzt geöffnet",
     L"Noch kein Tab-Verlauf aufgezeichnet.",
     L"Keine Ordner entsprechen Ihrer Suchanfrage.",
     L"Auswahl löschen (Strg+Entf)",
     L"Gesamten Verlauf löschen",
     L"Im aktuellen Tab öffnen (Shift+Enter)",
     L"In neuem Tab / Fenster öffnen (Enter)"},

    // 10. Russian - Русский (ru)
    {L"ru",
     L"История вкладок Проводника",
     L"%d элем.  •  Shift+Tab / Вверх/Вниз: выбор  •  Enter: новая вкладка  •  Shift+Enter: текущая",
     L"Введите имя или путь для фильтрации...",
     L"№",
     L"Имя папки / элемента",
     L"Полный путь",
     L"Время последнего открытия",
     L"История вкладок пока пуста.",
     L"Папки по вашему запросу не найдены.",
     L"Удалить выбранное (Ctrl+Del)",
     L"Очистить всю историю",
     L"Открыть в текущей вкладке (Shift+Enter)",
     L"Открыть в новой вкладке / окне (Enter)"},

    // 11. Portuguese - Português (pt)
    {L"pt",
     L"Histórico de Abas do Explorer",
     L"%d itens  •  Shift+Tab / Cima/Baixo: selecionar  •  Enter: nova aba  •  Shift+Enter: aba atual",
     L"Digite para filtrar por nome ou caminho...",
     L"Nº",
     L"Nome da pasta / item",
     L"Caminho completo",
     L"Última abertura",
     L"Nenhum histórico de abas registrado ainda.",
     L"Nenhuma pasta corresponde à sua pesquisa.",
     L"Excluir selecionado (Ctrl+Del)",
     L"Limpar todo o histórico",
     L"Abrir na aba atual (Shift+Enter)",
     L"Abrir em nova aba / janela (Enter)"},

    // 12. Italian - Italiano (it)
    {L"it",
     L"Cronologia schede di Esplora file",
     L"%d elementi  •  Shift+Tab / Su/Giù: seleziona  •  Invio: nuova scheda  •  Shift+Invio: scheda attuale",
     L"Digita per filtrare per nome o percorso...",
     L"N.",
     L"Nome cartella / elemento",
     L"Percorso completo",
     L"Ultima apertura",
     L"Nessuna cronologia schede registrata.",
     L"Nessuna cartella corrisponde alla ricerca.",
     L"Elimina selezionato (Ctrl+Canc)",
     L"Cancella tutta la cronologia",
     L"Apri nella scheda attuale (Shift+Invio)",
     L"Apri in nuova scheda / finestra (Invio)"},

    // 13. Arabic - العربية (ar)
    {L"ar",
     L"سجل علامات تبويب مستكشف الملفات",
     L"%d عنصر  •  Shift+Tab / أعلى/أسفل: تحديد  •  Enter: تبويب جديد  •  Shift+Enter: التبويب الحالي",
     L"اكتب للتصفية حسب الاسم أو المسار...",
     L"#",
     L"اسم المجلد / العنصر",
     L"المسار الكامل",
     L"وقت آخر فتح",
     L"لم يتم تسجيل أي سجل لعلامات التبويب بعد.",
     L"لا توجد مجلدات تطابق بحثك.",
     L"حذف المحدد (Ctrl+Del)",
     L"مسح كل السجل",
     L"فتح في التبويب الحالي (Shift+Enter)",
     L"فتح في تبويب / نافذة جديدة (Enter)"},

    // 14. Hindi - हिन्दी (hi)
    {L"hi",
     L"एक्सप्लोरर टैब इतिहास",
     L"%d आइटम  •  Shift+Tab / ऊपर/नीचे: चुनें  •  Enter: नया टैब  •  Shift+Enter: वर्तमान टैब",
     L"नाम या पथ से फ़िल्टर करने के लिए टाइप करें...",
     L"क्र.",
     L"फ़ोल्डर / आइटम का नाम",
     L"पूरा पथ",
     L"अंतिम बार खोलने का समय",
     L"अभी तक कोई टैब इतिहास दर्ज नहीं किया गया है।",
     L"आपकी खोज से मेल खाने वाला कोई फ़ोल्डर नहीं मिला।",
     L"चयनित हटाएं (Ctrl+Del)",
     L"पूरा इतिहास साफ़ करें",
     L"वर्तमान टैब में खोलें (Shift+Enter)",
     L"नए टैब / विंडो में खोलें (Enter)"},

    // 15. Indonesian - Bahasa Indonesia (id)
    {L"id",
     L"Riwayat Tab File Explorer",
     L"%d item  •  Shift+Tab / Atas/Bawah: pilih  •  Enter: tab baru  •  Shift+Enter: tab saat ini",
     L"Ketik untuk memfilter nama atau jalur...",
     L"No",
     L"Nama Folder / Item",
     L"Jalur Lengkap",
     L"Terakhir Dibuka",
     L"Belum ada riwayat tab yang tercatat.",
     L"Tidak ada folder yang cocok dengan pencarian Anda.",
     L"Hapus Pilihan (Ctrl+Del)",
     L"Hapus Semua Riwayat",
     L"Buka di Tab Saat Ini (Shift+Enter)",
     L"Buka di Tab / Jendela Baru (Enter)"},

    // 16. Malay - Bahasa Melayu (ms)
    {L"ms",
     L"Sejarah Tab File Explorer",
     L"%d item  •  Shift+Tab / Atas/Bawah: pilih  •  Enter: tab baharu  •  Shift+Enter: tab semasa",
     L"Taip untuk menapis mengikut nama atau laluan...",
     L"Bil",
     L"Nama Folder / Item",
     L"Laluan Penuh",
     L"Masa Terakhir Dibuka",
     L"Tiada sejarah tab direkodkan lagi.",
     L"Tiada folder sepadan dengan carian anda.",
     L"Padam Pilihan (Ctrl+Del)",
     L"Kosongkan Semua Sejarah",
     L"Buka dalam Tab Semasa (Shift+Enter)",
     L"Buka dalam Tab / Tetingkap Baharu (Enter)"},

    // 17. Thai - ไทย (th)
    {L"th",
     L"ประวัติการเปิดแท็บ File Explorer",
     L"%d รายการ  •  Shift+Tab / ขึ้น/ลง: เลือก  •  Enter: แท็บใหม่  •  Shift+Enter: แท็บเดิม",
     L"พิมพ์เพื่อกรองตามชื่อหรือเส้นทาง...",
     L"ลำดับ",
     L"ชื่อโฟลเดอร์ / รายการ",
     L"เส้นทางเต็ม",
     L"เวลาที่เปิดล่าสุด",
     L"ยังไม่มีประวัติแท็บที่บันทึกไว้",
     L"ไม่พบโฟลเดอร์ที่ตรงกับคำค้นหา",
     L"ลบรายการที่เลือก (Ctrl+Del)",
     L"ล้างประวัติทั้งหมด",
     L"เปิดในแท็บปัจจุบัน (Shift+Enter)",
     L"เปิดในแท็บ / หน้าต่างใหม่ (Enter)"},

    // 18. Turkish - Türkçe (tr)
    {L"tr",
     L"Dosya Gezgini Sekme Geçmişi",
     L"%d öğe  •  Shift+Tab / Yukarı/Aşağı: seç  •  Enter: yeni sekme  •  Shift+Enter: mevcut sekme",
     L"Ada veya yola göre filtrelemek için yazın...",
     L"No",
     L"Klasör / Öğe Adı",
     L"Tam Yol",
     L"Son Açılış Zamanı",
     L"Henüz kaydedilmiş sekme geçmişi yok.",
     L"Aramanızla eşleşen klasör bulunamadı.",
     L"Seçileni Sil (Ctrl+Del)",
     L"Tüm Geçmişi Temizle",
     L"Mevcut Sekmede Aç (Shift+Enter)",
     L"Yeni Sekmede / Pencerede Aç (Enter)"},

    // 19. Polish - Polski (pl)
    {L"pl",
     L"Historia kart Eksploratora plików",
     L"%d poz.  •  Shift+Tab / Góra/Dół: wybierz  •  Enter: nowa karta  •  Shift+Enter: bieżąca karta",
     L"Wpisz, aby filtrować według nazwy lub ścieżki...",
     L"Lp.",
     L"Nazwa folderu / elementu",
     L"Pełna ścieżka",
     L"Ostatnio otwarto",
     L"Brak zapisanej historii kart.",
     L"Brak folderów pasujących do wyszukiwania.",
     L"Usuń zaznaczone (Ctrl+Del)",
     L"Wyczyść całą historię",
     L"Otwórz w bieżącej karcie (Shift+Enter)",
     L"Otwórz w nowej karcie / oknie (Enter)"},

    // 20. Dutch - Nederlands (nl)
    {L"nl",
     L"Verkenner-tabbladgeschiedenis",
     L"%d items  •  Shift+Tab / Omhoog/Omlaag: kies  •  Enter: nieuw tabblad  •  Shift+Enter: huidig",
     L"Typ om te filteren op naam of pad...",
     L"Nr.",
     L"Map- / itemnaam",
     L"Volledig pad",
     L"Laatst geopend",
     L"Nog geen tabbladgeschiedenis vastgelegd.",
     L"Geen mappen gevonden voor uw zoekopdracht.",
     L"Selectie verwijderen (Ctrl+Del)",
     L"Alle geschiedenis wissen",
     L"Openen in huidig tabblad (Shift+Enter)",
     L"Openen in nieuw tabblad / venster (Enter)"},

    // 21. Ukrainian - Українська (uk)
    {L"uk",
     L"Історія вкладок Провідника",
     L"%d елем.  •  Shift+Tab / Вгору/Вниз: вибір  •  Enter: нова вкладка  •  Shift+Enter: поточна",
     L"Введіть назву або шлях для фільтрації...",
     L"№",
     L"Назва папки / елемента",
     L"Повний шлях",
     L"Час останнього відкриття",
     L"Історія вкладок поки що порожня.",
     L"Папок за вашим запитом не знайдено.",
     L"Видалити вибране (Ctrl+Del)",
     L"Очистити всю історію",
     L"Відкрити в поточній вкладці (Shift+Enter)",
     L"Відкрити в новій вкладці / вікні (Enter)"},

    // 22. Czech - Čeština (cs)
    {L"cs",
     L"Historie karet Průzkumníka",
     L"%d položek  •  Shift+Tab / Nahoru/Dolů: vybrat  •  Enter: nová karta  •  Shift+Enter: aktuální",
     L"Pište pro filtrování podle názvu nebo cesty...",
     L"Č.",
     L"Název složky / položky",
     L"Celá cesta",
     L"Naposledy otevřeno",
     L"Zatím nebyla zaznamenána žádná historie karet.",
     L"Vašemu hledání neodpovídají žádné složky.",
     L"Smazat vybrané (Ctrl+Del)",
     L"Vymazat celou historii",
     L"Otevřít v aktuální kartě (Shift+Enter)",
     L"Otevřít v nové kartě / okně (Enter)"},

    // 23. Swedish - Svenska (sv)
    {L"sv",
     L"Flikhistorik i Utforskaren",
     L"%d objekt  •  Shift+Tab / Upp/Ner: välj  •  Enter: ny flik  •  Shift+Enter: aktuell flik",
     L"Skriv för att filtrera efter namn eller sökväg...",
     L"Nr",
     L"Mapp- / objektnamn",
     L"Fullständig sökväg",
     L"Senast öppnad",
     L"Ingen flikhistorik har sparats ännu.",
     L"Inga mappar matchar din sökning.",
     L"Ta bort markerad (Ctrl+Del)",
     L"Rensa all historik",
     L"Öppna i aktuell flik (Shift+Enter)",
     L"Öppna i ny flik / nytt fönster (Enter)"},

    // 24. Romanian - Română (ro)
    {L"ro",
     L"Istoric file File Explorer",
     L"%d elemente  •  Shift+Tab / Sus/Jos: selectare  •  Enter: filă nouă  •  Shift+Enter: fila curentă",
     L"Tastați pentru a filtra după nume sau cale...",
     L"Nr.",
     L"Nume dosar / element",
     L"Cale completă",
     L"Ultima deschidere",
     L"Nu a fost înregistrat încă niciun istoric al filelor.",
     L"Niciun dosar nu corespunde căutării dvs.",
     L"Șterge selecția (Ctrl+Del)",
     L"Șterge tot istoricul",
     L"Deschide în fila curentă (Shift+Enter)",
     L"Deschide în filă / fereastră nouă (Enter)"},

    // 25. Greek - Ελληνικά (el)
    {L"el",
     L"Ιστορικό καρτελών Εξερεύνησης αρχείων",
     L"%d στοιχεία  •  Shift+Tab / Πάνω/Κάτω: επιλογή  •  Enter: νέα καρτέλα  •  Shift+Enter: τρέχουσα",
     L"Πληκτρολογήστε για φιλτράρισμα ονόματος ή διαδρομής...",
     L"Α/Α",
     L"Όνομα φακέλου / στοιχείου",
     L"Πλήρης διαδρομή",
     L"Τελευταίο άνοιγμα",
     L"Δεν έχει καταγραφεί ακόμη ιστορικό καρτελών.",
     L"Δεν βρέθηκαν φάκελοι που να ταιριάζουν.",
     L"Διαγραφή επιλογής (Ctrl+Del)",
     L"Εκκαθάριση ιστορικού",
     L"Άνοιγμα στην τρέχουσα καρτέλα (Shift+Enter)",
     L"Άνοιγμα σε νέα καρτέλα / παράθυρο (Enter)"},

    // 26. Hebrew - עברית (he)
    {L"he",
     L"היסטוריית כרטיסיות בסייר הקבצים",
     L"%d פריטים  •  Shift+Tab / למעלה/למטה: בחירה  •  Enter: כרטיסייה חדשה  •  Shift+Enter: נוכחית",
     L"הקלד לסינון לפי שם או נתיב...",
     L"מס'",
     L"שם תיקייה / פריט",
     L"נתיב מלא",
     L"זמן פתיחה אחרון",
     L"טרם נרשמה היסטוריית כרטיסיות.",
     L"לא נמצאו תיקיות התואמות לחיפוש שלך.",
     L"מחק בחירה (Ctrl+Del)",
     L"נקה את כל ההיסטוריה",
     L"פתח בכרטיסייה הנוכחית (Shift+Enter)",
     L"פתח בכרטיסייה / חלון חדש (Enter)"}
};

const LocalizedStrings* FindTranslationByCode(PCWSTR code) {
    if (!code || !*code) {
        return &kTranslations[0];
    }
    for (const auto& item : kTranslations) {
        if (_wcsicmp(item.code, code) == 0) {
            return &item;
        }
    }
    return nullptr;
}

const LocalizedStrings* DetectSystemLanguageStrings() {
    LANGID langId = GetUserDefaultUILanguage();
    WORD primary = PRIMARYLANGID(langId);
    WORD sub = SUBLANGID(langId);

    switch (primary) {
        case LANG_VIETNAMESE: return FindTranslationByCode(L"vi");
        case LANG_ENGLISH:    return FindTranslationByCode(L"en");
        case LANG_CHINESE:
            if (sub == SUBLANG_CHINESE_TRADITIONAL ||
                sub == SUBLANG_CHINESE_HONGKONG ||
                sub == SUBLANG_CHINESE_MACAU) {
                return FindTranslationByCode(L"zh-TW");
            }
            return FindTranslationByCode(L"zh-CN");
        case LANG_JAPANESE:   return FindTranslationByCode(L"ja");
        case LANG_KOREAN:     return FindTranslationByCode(L"ko");
        case LANG_SPANISH:    return FindTranslationByCode(L"es");
        case LANG_FRENCH:     return FindTranslationByCode(L"fr");
        case LANG_GERMAN:     return FindTranslationByCode(L"de");
        case LANG_RUSSIAN:    return FindTranslationByCode(L"ru");
        case LANG_PORTUGUESE: return FindTranslationByCode(L"pt");
        case LANG_ITALIAN:    return FindTranslationByCode(L"it");
        case LANG_ARABIC:     return FindTranslationByCode(L"ar");
        case LANG_HINDI:      return FindTranslationByCode(L"hi");
        case LANG_INDONESIAN: return FindTranslationByCode(L"id");
        case LANG_MALAY:      return FindTranslationByCode(L"ms");
        case LANG_THAI:       return FindTranslationByCode(L"th");
        case LANG_TURKISH:    return FindTranslationByCode(L"tr");
        case LANG_POLISH:     return FindTranslationByCode(L"pl");
        case LANG_DUTCH:      return FindTranslationByCode(L"nl");
        case LANG_UKRAINIAN:  return FindTranslationByCode(L"uk");
        case LANG_CZECH:      return FindTranslationByCode(L"cs");
        case LANG_SWEDISH:    return FindTranslationByCode(L"sv");
        case LANG_ROMANIAN:   return FindTranslationByCode(L"ro");
        case LANG_GREEK:      return FindTranslationByCode(L"el");
        case LANG_HEBREW:     return FindTranslationByCode(L"he");
        default:              return FindTranslationByCode(L"en");
    }
}

struct HotkeyConfig {
    bool ctrl = false;
    bool shift = true;
    bool alt = false;
    bool win = false;
    UINT vk = VK_TAB;
};

struct Settings {
    const LocalizedStrings* lang = &kTranslations[0];
    HotkeyConfig reopenHotkey = {true, true, false, false, 'T'};
    bool enableShiftTabHistory = true;
    HotkeyConfig historyHotkey = {false, true, false, false, VK_TAB}; // Mặc định: Shift+Tab
    int maxHistory = 100;
    bool deduplicateHistory = true;
    bool win10OpenInNewWindow = false;
    bool perWindowClosedHistory = false;
    int themeMode = 0; // 0 = auto, 1 = dark, 2 = light
} g_settings;

const LocalizedStrings& GetLoc() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_settings.lang ? *g_settings.lang : kTranslations[0];
}

// Cấu trúc lưu mục trong Bảng lịch sử 4 cột (Shift+Tab)
struct HistoryRecord {
    LPITEMIDLIST pidl = nullptr;
    std::wstring itemName;        // Cột 2: Tên folder (hoặc tên file,..)
    std::wstring fullPath;        // Cột 3: Đường dẫn
    std::wstring lastOpenedTime;  // Cột 4: Thời gian mở gần đây nhất
    FILETIME lastOpenedFt = {};

    HistoryRecord() = default;

    HistoryRecord(LPITEMIDLIST inPidl,
                  std::wstring inName,
                  std::wstring inPath,
                  std::wstring inTime,
                  FILETIME inFt)
        : pidl(inPidl),
          itemName(std::move(inName)),
          fullPath(std::move(inPath)),
          lastOpenedTime(std::move(inTime)),
          lastOpenedFt(inFt) {}

    HistoryRecord(const HistoryRecord&) = delete;
    HistoryRecord& operator=(const HistoryRecord&) = delete;

    HistoryRecord(HistoryRecord&& other) noexcept
        : pidl(other.pidl),
          itemName(std::move(other.itemName)),
          fullPath(std::move(other.fullPath)),
          lastOpenedTime(std::move(other.lastOpenedTime)),
          lastOpenedFt(other.lastOpenedFt) {
        other.pidl = nullptr;
    }

    HistoryRecord& operator=(HistoryRecord&& other) noexcept {
        if (this != &other) {
            if (pidl) {
                ILFree(pidl);
            }
            pidl = other.pidl;
            itemName = std::move(other.itemName);
            fullPath = std::move(other.fullPath);
            lastOpenedTime = std::move(other.lastOpenedTime);
            lastOpenedFt = other.lastOpenedFt;
            other.pidl = nullptr;
        }
        return *this;
    }

    ~HistoryRecord() {
        if (pidl) {
            ILFree(pidl);
            pidl = nullptr;
        }
    }
};

// Cấu trúc lưu Tab đã đóng cho Ctrl+Shift+T
struct ClosedTabEntry {
    LPITEMIDLIST pidl = nullptr;
    HWND hOwnerCabinet = nullptr;
    std::wstring displayName;

    ClosedTabEntry() = default;

    ClosedTabEntry(LPITEMIDLIST inPidl, HWND inCabinet, std::wstring inName)
        : pidl(inPidl), hOwnerCabinet(inCabinet), displayName(std::move(inName)) {}

    ClosedTabEntry(const ClosedTabEntry&) = delete;
    ClosedTabEntry& operator=(const ClosedTabEntry&) = delete;

    ClosedTabEntry(ClosedTabEntry&& other) noexcept
        : pidl(other.pidl),
          hOwnerCabinet(other.hOwnerCabinet),
          displayName(std::move(other.displayName)) {
        other.pidl = nullptr;
    }

    ClosedTabEntry& operator=(ClosedTabEntry&& other) noexcept {
        if (this != &other) {
            if (pidl) {
                ILFree(pidl);
            }
            pidl = other.pidl;
            hOwnerCabinet = other.hOwnerCabinet;
            displayName = std::move(other.displayName);
            other.pidl = nullptr;
        }
        return *this;
    }

    ~ClosedTabEntry() {
        if (pidl) {
            ILFree(pidl);
            pidl = nullptr;
        }
    }
};

struct ActiveTabState {
    HWND hCabinet = nullptr;
    LPITEMIDLIST currentPidl = nullptr;
    std::wstring displayName;
    std::wstring fullPath;
    bool savedToHistory = false;
    bool isRestoring = false;
    LPITEMIDLIST restoreTargetPidl = nullptr;
    int restoreTickCount = 0;
    int matchedTicks = 0;

    ~ActiveTabState() {
        if (currentPidl) {
            ILFree(currentPidl);
            currentPidl = nullptr;
        }
        if (restoreTargetPidl) {
            ILFree(restoreTargetPidl);
            restoreTargetPidl = nullptr;
        }
    }
};

struct PendingRestoreItem {
    LPITEMIDLIST pidl = nullptr;
    ULONGLONG timestamp = 0;
};

std::unordered_map<HWND, ActiveTabState> g_activeTabs;
std::unordered_set<HWND> g_cabinetWindows;
std::deque<ClosedTabEntry> g_closedTabs;
std::deque<HistoryRecord> g_openHistory;
std::unordered_map<HWND, std::deque<PendingRestoreItem>> g_pendingRestores;
std::unordered_map<DWORD, HHOOK> g_threadKeyboardHooks;
ULONGLONG g_lastShortcutTriggerTick = 0;
ULONG_PTR g_gdiplusToken = 0;
HWND g_hHistoryDialog = nullptr;
ATOM g_historyDialogAtom = 0;

void ShowOrCycleHistoryDialog(HWND hCabinet);
bool RestoreLastClosedTab(HWND hCabinet);
bool OpenFolderPidlInCabinet(HWND hCabinet, LPCITEMIDLIST pidl, bool forceSameTab);

// ---------------------------------------------------------------------------
// Phân tích cấu hình phím tắt & cài đặt
// ---------------------------------------------------------------------------
HotkeyConfig ParseHotkeyString(PCWSTR text, HotkeyConfig defaultCfg) {
    if (!text || !*text) {
        return defaultCfg;
    }

    HotkeyConfig cfg;
    std::wstring lower;
    for (PCWSTR p = text; *p; ++p) {
        lower.push_back(std::towlower(*p));
    }

    cfg.ctrl = (lower.find(L"ctrl") != std::wstring::npos) ||
               (lower.find(L"control") != std::wstring::npos);
    cfg.shift = (lower.find(L"shift") != std::wstring::npos);
    cfg.alt = (lower.find(L"alt") != std::wstring::npos) ||
              (lower.find(L"menu") != std::wstring::npos);
    cfg.win = (lower.find(L"win") != std::wstring::npos);

    size_t lastPlus = lower.rfind(L'+');
    std::wstring keyPart =
        (lastPlus != std::wstring::npos) ? lower.substr(lastPlus + 1) : lower;

    while (!keyPart.empty() && std::iswspace(keyPart.front())) {
        keyPart.erase(keyPart.begin());
    }
    while (!keyPart.empty() && std::iswspace(keyPart.back())) {
        keyPart.pop_back();
    }

    if (keyPart.empty()) {
        return defaultCfg;
    }

    if (keyPart == L"tab") {
        cfg.vk = VK_TAB;
        return cfg;
    }
    if (keyPart == L"space") {
        cfg.vk = VK_SPACE;
        return cfg;
    }
    if (keyPart == L"tilde" || keyPart == L"`") {
        cfg.vk = VK_OEM_3;
        return cfg;
    }

    if (keyPart.size() == 1 && keyPart[0] >= L'a' && keyPart[0] <= L'z') {
        cfg.vk = static_cast<UINT>(std::towupper(keyPart[0]));
        return cfg;
    }

    bool allDigits = true;
    for (wchar_t ch : keyPart) {
        if (!std::iswdigit(ch)) {
            allDigits = false;
            break;
        }
    }
    if (allDigits) {
        UINT parsedVk = static_cast<UINT>(_wtoi(keyPart.c_str()));
        if (parsedVk != 0) {
            cfg.vk = parsedVk;
            return cfg;
        }
    }

    return defaultCfg;
}

void LoadSettings() {
    const LocalizedStrings* chosenLang = nullptr;
    PCWSTR langStr = Wh_GetStringSetting(L"Language");
    if (langStr) {
        if (_wcsicmp(langStr, L"auto") == 0 || !*langStr) {
            chosenLang = DetectSystemLanguageStrings();
        } else {
            chosenLang = FindTranslationByCode(langStr);
        }
        Wh_FreeStringSetting(langStr);
    }
    if (!chosenLang) {
        chosenLang = DetectSystemLanguageStrings();
    }

    HotkeyConfig defaultReopen = {true, true, false, false, 'T'};
    PCWSTR reopenStr = Wh_GetStringSetting(L"ReopenHotkey");
    HotkeyConfig reopenHk = ParseHotkeyString(reopenStr, defaultReopen);
    Wh_FreeStringSetting(reopenStr);

    bool enableShiftTab = Wh_GetIntSetting(L"EnableShiftTabHistory") != 0;

    HotkeyConfig defaultHist = {false, true, false, false, VK_TAB};
    PCWSTR histStr = Wh_GetStringSetting(L"HistoryPanelHotkey");
    HotkeyConfig histHk = ParseHotkeyString(histStr, defaultHist);
    Wh_FreeStringSetting(histStr);

    int maxHist = Wh_GetIntSetting(L"MaxHistory");
    if (maxHist < 10) {
        maxHist = 10;
    } else if (maxHist > 500) {
        maxHist = 500;
    }

    bool dedup = Wh_GetIntSetting(L"DeduplicateHistory") != 0;
    bool win10NewWin = Wh_GetIntSetting(L"Win10OpenInNewWindow") != 0;
    bool perWin = Wh_GetIntSetting(L"PerWindowClosedHistory") != 0;

    PCWSTR themeStr = Wh_GetStringSetting(L"ThemeMode");
    int themeMode = 0;
    if (themeStr) {
        if (_wcsicmp(themeStr, L"dark") == 0) {
            themeMode = 1;
        } else if (_wcsicmp(themeStr, L"light") == 0) {
            themeMode = 2;
        }
        Wh_FreeStringSetting(themeStr);
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    g_settings.lang = chosenLang;
    g_settings.reopenHotkey = reopenHk;
    g_settings.enableShiftTabHistory = enableShiftTab;
    g_settings.historyHotkey = histHk;
    g_settings.maxHistory = maxHist;
    g_settings.deduplicateHistory = dedup;
    g_settings.win10OpenInNewWindow = win10NewWin;
    g_settings.perWindowClosedHistory = perWin;
    g_settings.themeMode = themeMode;

    while (static_cast<int>(g_closedTabs.size()) > g_settings.maxHistory) {
        g_closedTabs.pop_front();
    }
    while (static_cast<int>(g_openHistory.size()) > g_settings.maxHistory) {
        g_openHistory.pop_back();
    }
}

// ---------------------------------------------------------------------------
// Kiểm tra chế độ Dark Mode & hỗ trợ Tab của Windows 11 / Windows 10
// ---------------------------------------------------------------------------
bool IsSystemDarkMode() {
    int mode = 0;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        mode = g_settings.themeMode;
    }
    if (mode == 1) {
        return true;
    }
    if (mode == 2) {
        return false;
    }

    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value == 0;
    }
    return true;
}

bool IsWindowOfClass(HWND hWnd, PCWSTR className) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    WCHAR szClass[96];
    if (GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass)) == 0) {
        return false;
    }
    return _wcsicmp(szClass, className) == 0;
}

bool IsHistoryDialogOrChild(HWND hWnd) {
    HWND hDlg = g_hHistoryDialog;
    if (!hDlg || !hWnd || !IsWindow(hDlg)) {
        return false;
    }
    return (hWnd == hDlg) || IsChild(hDlg, hWnd);
}

bool IsImeOrTsfPopupWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    WCHAR szClass[128] = {};
    if (GetClassNameW(hWnd, szClass, ARRAYSIZE(szClass)) == 0) {
        return false;
    }
    return (_wcsicmp(szClass, L"IME") == 0) ||
           (_wcsicmp(szClass, L"MSCTFIME UI") == 0) ||
           (_wcsicmp(szClass, L"CiceroUIWndFrame") == 0) ||
           (wcsstr(szClass, L"InputIndicator") != nullptr) ||
           (wcsstr(szClass, L"Candidate") != nullptr);
}

HWND FindParentCabinetWindow(HWND hWnd) {
    HWND hCurr = hWnd;
    while (hCurr) {
        if (IsWindowOfClass(hCurr, L"CabinetWClass")) {
            return hCurr;
        }
        hCurr = GetParent(hCurr);
    }
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (IsWindowOfClass(hRoot, L"CabinetWClass")) {
        return hRoot;
    }
    return nullptr;
}

HWND FindActiveShellTabWindow(HWND hCabinet) {
    if (!hCabinet || !IsWindow(hCabinet)) {
        return nullptr;
    }

    HWND hFirstTab = nullptr;
    HWND hChild = nullptr;
    while ((hChild = FindWindowExW(hCabinet, hChild, L"ShellTabWindowClass", nullptr)) != nullptr) {
        if (!hFirstTab) {
            hFirstTab = hChild;
        }
        if (IsWindowVisible(hChild)) {
            return hChild;
        }
    }
    return hFirstTab;
}

bool CabinetSupportsNativeWin11Tabs(HWND hCabinet) {
    if (!hCabinet || !IsWindow(hCabinet)) {
        return false;
    }
    HWND hBridge = FindWindowExW(hCabinet, nullptr,
                                 L"Microsoft.UI.Content.DesktopChildSiteBridge", nullptr);
    if (hBridge) {
        return true;
    }
    HWND hTab1 = FindWindowExW(hCabinet, nullptr, L"ShellTabWindowClass", nullptr);
    if (hTab1 && FindWindowExW(hCabinet, hTab1, L"ShellTabWindowClass", nullptr)) {
        return true;
    }
    return false;
}

std::wstring FormatCurrentTimestamp(FILETIME* outFt = nullptr) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    if (outFt) {
        SystemTimeToFileTime(&st, outFt);
    }
    WCHAR buf[64];
    wsprintfW(buf, L"%02u:%02u:%02u - %02u/%02u/%04u",
              st.wHour, st.wMinute, st.wSecond,
              st.wDay, st.wMonth, st.wYear);
    return buf;
}

// ---------------------------------------------------------------------------
// Làm việc với Shell COM API để lấy PIDL, Tên hiển thị và Đường dẫn
// ---------------------------------------------------------------------------
IShellBrowser* GetShellBrowserForTab(HWND hTab) {
    if (!hTab || !IsWindow(hTab) || g_uninitializing.load(std::memory_order_relaxed)) {
        return nullptr;
    }

    IShellBrowser* pShellBrowser =
        reinterpret_cast<IShellBrowser*>(SendMessageW(hTab, WM_GETISHELLBROWSER, 0, 0));
    if (pShellBrowser) {
        pShellBrowser->AddRef();
        return pShellBrowser;
    }

    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr,
                                CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pShellWindows))) ||
        !pShellWindows) {
        return nullptr;
    }

    long count = 0;
    pShellWindows->get_Count(&count);
    for (long i = 0; i < count; ++i) {
        VARIANT vIndex;
        VariantInit(&vIndex);
        vIndex.vt = VT_I4;
        vIndex.lVal = i;

        IDispatch* pDisp = nullptr;
        if (SUCCEEDED(pShellWindows->Item(vIndex, &pDisp)) && pDisp) {
            IServiceProvider* pSp = nullptr;
            if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pSp))) && pSp) {
                IShellBrowser* pSb = nullptr;
                if (SUCCEEDED(pSp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&pSb))) && pSb) {
                    HWND hBrowserTab = nullptr;
                    if (SUCCEEDED(pSb->GetWindow(&hBrowserTab)) && hBrowserTab == hTab) {
                        pShellBrowser = pSb;
                        pSp->Release();
                        pDisp->Release();
                        break;
                    }
                    pSb->Release();
                }
                pSp->Release();
            }
            pDisp->Release();
        }
    }

    pShellWindows->Release();
    return pShellBrowser;
}

void ExtractPidlDisplayInfo(LPCITEMIDLIST pidl,
                            std::wstring* outItemName,
                            std::wstring* outFullPath) {
    if (!pidl) {
        return;
    }

    std::wstring displayName;
    std::wstring fullPath;

    PWSTR pszNormal = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &pszNormal)) && pszNormal) {
        displayName = pszNormal;
        CoTaskMemFree(pszNormal);
    }

    PWSTR pszFileSys = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &pszFileSys)) && pszFileSys) {
        fullPath = pszFileSys;
        CoTaskMemFree(pszFileSys);
    }

    if (fullPath.empty()) {
        PWSTR pszAddressBar = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEEDITING, &pszAddressBar)) &&
            pszAddressBar) {
            fullPath = pszAddressBar;
            CoTaskMemFree(pszAddressBar);
        }
    }

    if (fullPath.empty()) {
        PWSTR pszParsing = nullptr;
        if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &pszParsing)) &&
            pszParsing) {
            fullPath = pszParsing;
            CoTaskMemFree(pszParsing);
        }
    }

    if (displayName.empty()) {
        displayName = fullPath;
    }
    if (fullPath.empty()) {
        fullPath = displayName;
    }

    if (outItemName) {
        *outItemName = std::move(displayName);
    }
    if (outFullPath) {
        *outFullPath = std::move(fullPath);
    }
}

LPITEMIDLIST QueryTabCurrentPidl(HWND hTab,
                                 std::wstring* outDisplayName = nullptr,
                                 std::wstring* outFullPath = nullptr) {
    IShellBrowser* pShellBrowser = GetShellBrowserForTab(hTab);
    if (!pShellBrowser) {
        return nullptr;
    }

    LPITEMIDLIST clonedPidl = nullptr;
    IShellView* pShellView = nullptr;
    if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
        IFolderView* pFolderView = nullptr;
        if (SUCCEEDED(pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView))) && pFolderView) {
            IPersistFolder2* pPersistFolder2 = nullptr;
            if (SUCCEEDED(pFolderView->GetFolder(IID_PPV_ARGS(&pPersistFolder2))) &&
                pPersistFolder2) {
                LPITEMIDLIST rawPidl = nullptr;
                if (SUCCEEDED(pPersistFolder2->GetCurFolder(&rawPidl)) && rawPidl) {
                    clonedPidl = ILClone(rawPidl);
                    ExtractPidlDisplayInfo(rawPidl, outDisplayName, outFullPath);
                    CoTaskMemFree(rawPidl);
                }
                pPersistFolder2->Release();
            }
            pFolderView->Release();
        }
        pShellView->Release();
    }

    pShellBrowser->Release();
    return clonedPidl;
}

void RecordOpenedFolderToHistoryLocked(LPCITEMIDLIST pidl,
                                       const std::wstring& itemName,
                                       const std::wstring& fullPath) {
    if (!pidl || itemName.empty()) {
        return;
    }

    FILETIME ftNow = {};
    std::wstring timeStr = FormatCurrentTimestamp(&ftNow);

    if (g_settings.deduplicateHistory) {
        for (auto it = g_openHistory.begin(); it != g_openHistory.end(); ++it) {
            bool same = false;
            if (it->pidl && ILIsEqual(it->pidl, pidl)) {
                same = true;
            } else if (!fullPath.empty() && _wcsicmp(it->fullPath.c_str(), fullPath.c_str()) == 0) {
                same = true;
            }
            if (same) {
                g_openHistory.erase(it);
                break;
            }
        }
    } else if (!g_openHistory.empty()) {
        if ((g_openHistory.front().pidl && ILIsEqual(g_openHistory.front().pidl, pidl)) ||
            (_wcsicmp(g_openHistory.front().fullPath.c_str(), fullPath.c_str()) == 0)) {
            g_openHistory.front().lastOpenedTime = timeStr;
            g_openHistory.front().lastOpenedFt = ftNow;
            return;
        }
    }

    LPITEMIDLIST cloned = ILClone(pidl);
    if (!cloned) {
        return;
    }

    g_openHistory.emplace_front(cloned, itemName, fullPath, timeStr, ftNow);
    while (static_cast<int>(g_openHistory.size()) > g_settings.maxHistory) {
        g_openHistory.pop_back();
    }
}

void UpdateTabLocationSnapshot(HWND hTab) {
    if (!hTab || !IsWindow(hTab) || g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_activeTabs.find(hTab);
        if (it != g_activeTabs.end() && it->second.isRestoring) {
            return;
        }
    }

    std::wstring name;
    std::wstring path;
    LPITEMIDLIST newPidl = QueryTabCurrentPidl(hTab, &name, &path);
    if (!newPidl) {
        return;
    }

    HWND hCabinet = FindParentCabinetWindow(hTab);
    bool shouldRefreshDialog = false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto& state = g_activeTabs[hTab];
        if (hCabinet) {
            state.hCabinet = hCabinet;
        }

        bool locationChanged = !state.currentPidl || !ILIsEqual(state.currentPidl, newPidl);
        if (state.currentPidl) {
            ILFree(state.currentPidl);
        }
        state.currentPidl = newPidl;
        if (!name.empty()) {
            state.displayName = name;
        }
        if (!path.empty()) {
            state.fullPath = path;
        }

        if (locationChanged && !state.displayName.empty()) {
            RecordOpenedFolderToHistoryLocked(newPidl, state.displayName, state.fullPath);
            shouldRefreshDialog = (g_hHistoryDialog && IsWindow(g_hHistoryDialog));
        }
    }

    if (shouldRefreshDialog) {
        InvalidateRect(g_hHistoryDialog, nullptr, FALSE);
    }
}

void RecordTabClosed(HWND hTab) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    UpdateTabLocationSnapshot(hTab);

    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_activeTabs.find(hTab);
    if (it == g_activeTabs.end()) {
        return;
    }

    ActiveTabState& state = it->second;
    if (state.savedToHistory || state.isRestoring || !state.currentPidl) {
        return;
    }

    state.savedToHistory = true;
    LPITEMIDLIST cloned = ILClone(state.currentPidl);
    if (!cloned) {
        return;
    }

    g_closedTabs.emplace_back(cloned, state.hCabinet, state.displayName);
    while (static_cast<int>(g_closedTabs.size()) > g_settings.maxHistory) {
        g_closedTabs.pop_front();
    }
}

// ---------------------------------------------------------------------------
// Mở một thư mục (PIDL) trong Tab mới (Win11) hoặc Cửa sổ (Win10)
// ---------------------------------------------------------------------------
bool OpenPidlInNewExplorerWindow(LPCITEMIDLIST pidl) {
    if (!pidl) {
        return false;
    }
    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_IDLIST;
    sei.lpVerb = L"open";
    sei.lpIDList = const_cast<LPITEMIDLIST>(pidl);
    sei.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&sei) != FALSE;
}

bool OpenFolderPidlInCabinet(HWND hCabinet, LPCITEMIDLIST pidl, bool forceSameTab) {
    if (!pidl) {
        return false;
    }

    if (!hCabinet || !IsWindow(hCabinet)) {
        return OpenPidlInNewExplorerWindow(pidl);
    }

    HWND hActiveTab = FindActiveShellTabWindow(hCabinet);
    bool hasWin11Tabs = CabinetSupportsNativeWin11Tabs(hCabinet);
    bool win10NewWindow = false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        win10NewWindow = g_settings.win10OpenInNewWindow;
    }

    if (forceSameTab || (!hasWin11Tabs && !win10NewWindow)) {
        if (hActiveTab) {
            IShellBrowser* pSb = GetShellBrowserForTab(hActiveTab);
            if (pSb) {
                HRESULT hr = pSb->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                pSb->Release();
                if (SUCCEEDED(hr)) {
                    SetForegroundWindow(hCabinet);
                    return true;
                }
            }
        }
        return OpenPidlInNewExplorerWindow(pidl);
    }

    if (!hasWin11Tabs && win10NewWindow) {
        return OpenPidlInNewExplorerWindow(pidl);
    }

    LPITEMIDLIST cloned = ILClone(pidl);
    if (!cloned) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto& queue = g_pendingRestores[hCabinet];
        ULONGLONG now = GetTickCount64();
        while (!queue.empty() && (now - queue.front().timestamp > 5000)) {
            if (queue.front().pidl) {
                ILFree(queue.front().pidl);
            }
            queue.pop_front();
        }
        queue.push_back(PendingRestoreItem{cloned, now});
    }

    SetForegroundWindow(hCabinet);
    HWND hCommandTarget = hActiveTab ? hActiveTab : hCabinet;
    PostMessageW(hCommandTarget, WM_COMMAND, IDM_EXPLORER_NEW_TAB, 0);
    return true;
}

bool RestoreLastClosedTab(HWND hCabinet) {
    LPITEMIDLIST targetPidl = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_closedTabs.empty()) {
            return false;
        }

        for (auto it = g_closedTabs.rbegin(); it != g_closedTabs.rend(); ++it) {
            if (g_settings.perWindowClosedHistory && hCabinet && it->hOwnerCabinet != hCabinet) {
                continue;
            }
            if (it->pidl) {
                targetPidl = ILClone(it->pidl);
                g_closedTabs.erase(std::next(it).base());
                break;
            }
        }
    }

    if (!targetPidl) {
        return false;
    }

    bool ok = OpenFolderPidlInCabinet(hCabinet, targetPidl, false);
    ILFree(targetPidl);
    return ok;
}

// ---------------------------------------------------------------------------
// Điều hướng Tab mới tạo về PIDL đích (dành cho Windows 11 Tabs)
// ---------------------------------------------------------------------------
void TryNavigateRestoringTab(HWND hTab) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        KillTimer(hTab, IDT_RESTORE_TAB_NAVIGATE);
        return;
    }

    LPITEMIDLIST targetClone = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_activeTabs.find(hTab);
        if (it == g_activeTabs.end() || !it->second.isRestoring ||
            !it->second.restoreTargetPidl) {
            KillTimer(hTab, IDT_RESTORE_TAB_NAVIGATE);
            return;
        }

        it->second.restoreTickCount++;
        if (it->second.restoreTickCount > MAX_RESTORE_TICKS) {
            it->second.isRestoring = false;
            if (it->second.currentPidl) {
                ILFree(it->second.currentPidl);
            }
            it->second.currentPidl = it->second.restoreTargetPidl;
            it->second.restoreTargetPidl = nullptr;
            ExtractPidlDisplayInfo(it->second.currentPidl,
                                   &it->second.displayName,
                                   &it->second.fullPath);
            RecordOpenedFolderToHistoryLocked(it->second.currentPidl,
                                              it->second.displayName,
                                              it->second.fullPath);
            KillTimer(hTab, IDT_RESTORE_TAB_NAVIGATE);
            return;
        }

        targetClone = ILClone(it->second.restoreTargetPidl);
    }

    if (!targetClone) {
        return;
    }

    IShellBrowser* pShellBrowser = GetShellBrowserForTab(hTab);
    if (pShellBrowser) {
        std::wstring currentName;
        std::wstring currentPath;
        LPITEMIDLIST currentPidl = QueryTabCurrentPidl(hTab, &currentName, &currentPath);
        if (currentPidl) {
            bool isAlreadyAtTarget = ILIsEqual(currentPidl, targetClone);
            if (!isAlreadyAtTarget) {
                pShellBrowser->BrowseObject(targetClone, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_activeTabs.find(hTab);
                if (it != g_activeTabs.end()) {
                    it->second.matchedTicks = 0;
                }
            } else {
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_activeTabs.find(hTab);
                if (it != g_activeTabs.end()) {
                    it->second.matchedTicks++;
                    if (it->second.matchedTicks >= 3 && it->second.restoreTickCount >= 4) {
                        it->second.isRestoring = false;
                        if (it->second.currentPidl) {
                            ILFree(it->second.currentPidl);
                        }
                        it->second.currentPidl = ILClone(targetClone);
                        it->second.displayName = currentName;
                        it->second.fullPath = currentPath;
                        RecordOpenedFolderToHistoryLocked(targetClone, currentName, currentPath);
                        if (it->second.restoreTargetPidl) {
                            ILFree(it->second.restoreTargetPidl);
                            it->second.restoreTargetPidl = nullptr;
                        }
                        KillTimer(hTab, IDT_RESTORE_TAB_NAVIGATE);
                    }
                }
            }
            ILFree(currentPidl);
        } else {
            pShellBrowser->BrowseObject(targetClone, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        }
        pShellBrowser->Release();
    }

    ILFree(targetClone);
}

// ===========================================================================
// GIAO DIỆN BẢNG LỊCH SỬ MỞ TAB EXPLORER — PHONG CÁCH WINUI 3 (WIN 11 & WIN 10)
// ===========================================================================
struct HistoryDialogRowView {
    size_t originalIndex = 0;
    int stt = 0;
    std::wstring itemName;
    std::wstring fullPath;
    std::wstring lastOpenedTime;
    LPITEMIDLIST pidl = nullptr;
};

struct HistoryDialogState {
    HWND hWnd = nullptr;
    HWND hTargetCabinet = nullptr;
    HWND hSearchEdit = nullptr;
    HFONT hSearchFont = nullptr;
    HBRUSH hSearchBgBrush = nullptr;
    UINT dpi = 96;
    bool darkMode = true;
    std::wstring searchQuery;
    int selectedRow = 0;
    int hoverRow = -1;
    int scrollOffsetRows = 0;
    int hoverButtonId = 0; // 1=CloseX, 2=OpenNewTab, 3=OpenCurTab, 4=DeleteRow, 5=ClearAll

    RECT rcCloseBtn = {};
    RECT rcSearchBox = {};
    RECT rcSearchEditInner = {};
    RECT rcTableCard = {};
    RECT rcTableHeader = {};
    RECT rcTableBody = {};
    RECT rcBtnOpenNew = {};
    RECT rcBtnOpenSame = {};
    RECT rcBtnDeleteOne = {};
    RECT rcBtnClearAll = {};

    ~HistoryDialogState() {
        if (hSearchFont) {
            DeleteObject(hSearchFont);
            hSearchFont = nullptr;
        }
        if (hSearchBgBrush) {
            DeleteObject(hSearchBgBrush);
            hSearchBgBrush = nullptr;
        }
    }
};

int ScaleForDpi(int val, UINT dpi) {
    return MulDiv(val, static_cast<int>(dpi), 96);
}

UINT GetWindowDpiSafe(HWND hWnd) {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto pGetDpiForWindow =
            reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(hUser32, "GetDpiForWindow"));
        if (pGetDpiForWindow && hWnd) {
            UINT d = pGetDpiForWindow(hWnd);
            if (d > 0) {
                return d;
            }
        }
    }
    HDC hdc = GetDC(hWnd);
    UINT dpi = 96;
    if (hdc) {
        dpi = static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX));
        ReleaseDC(hWnd, hdc);
    }
    return dpi ? dpi : 96;
}

void ApplyWinUI3WindowBackdrop(HWND hWnd, bool darkMode) {
    BOOL darkVal = darkMode ? TRUE : FALSE;
    if (FAILED(DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_NEW,
                                     &darkVal, sizeof(darkVal)))) {
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_OLD,
                              &darkVal, sizeof(darkVal));
    }

    DWORD cornerPref = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE_VAL,
                          &cornerPref, sizeof(cornerPref));

    COLORREF borderClr = darkMode ? RGB(58, 58, 58) : RGB(215, 215, 215);
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR_VAL, &borderClr, sizeof(borderClr));

    DWORD backdropType = 3; // DWMSBT_TRANSIENTWINDOW
    HRESULT hrBackdrop = DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE_VAL,
                                               &backdropType, sizeof(backdropType));

    if (FAILED(hrBackdrop)) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (hUser32) {
            auto pSetWindowCompositionAttribute =
                reinterpret_cast<SetWindowCompositionAttribute_t>(
                    GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
            if (pSetWindowCompositionAttribute) {
                ACCENT_POLICY policy = {};
                policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
                policy.AccentFlags = 2;
                policy.GradientColor = darkMode ? 0xE61F1F1F : 0xE6F4F4F4;
                WINDOWCOMPOSITIONATTRIBDATA data = {};
                data.Attrib = 19; // WCA_ACCENT_POLICY
                data.pvData = &policy;
                data.cbData = sizeof(policy);
                pSetWindowCompositionAttribute(hWnd, &data);
            }
        }
    }
}

bool ContainsCaseInsensitive(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) {
        return true;
    }
    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](wchar_t ch1, wchar_t ch2) {
            return std::towlower(ch1) == std::towlower(ch2);
        });
    return it != haystack.end();
}

std::vector<HistoryDialogRowView> BuildFilteredHistoryRows(const std::wstring& query) {
    std::vector<HistoryDialogRowView> result;
    std::lock_guard<std::mutex> lock(g_mutex);
    int sttCounter = 1;
    for (size_t i = 0; i < g_openHistory.size(); ++i) {
        const auto& rec = g_openHistory[i];
        if (!query.empty()) {
            if (!ContainsCaseInsensitive(rec.itemName, query) &&
                !ContainsCaseInsensitive(rec.fullPath, query)) {
                continue;
            }
        }
        HistoryDialogRowView row;
        row.originalIndex = i;
        row.stt = sttCounter++;
        row.itemName = rec.itemName;
        row.fullPath = rec.fullPath;
        row.lastOpenedTime = rec.lastOpenedTime;
        row.pidl = rec.pidl;
        result.push_back(std::move(row));
    }
    return result;
}

void AddRoundedRectPath(Gdiplus::GraphicsPath& path,
                        const Gdiplus::RectF& rc,
                        float radius) {
    if (radius <= 0.5f) {
        path.AddRectangle(rc);
        return;
    }
    float d = radius * 2.0f;
    if (d > rc.Width) d = rc.Width;
    if (d > rc.Height) d = rc.Height;

    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void FillRoundRect(Gdiplus::Graphics& g,
                   const Gdiplus::RectF& rc,
                   float radius,
                   const Gdiplus::Color& fillColor) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rc, radius);
    Gdiplus::SolidBrush brush(fillColor);
    g.FillPath(&brush, &path);
}

void DrawRoundRect(Gdiplus::Graphics& g,
                   const Gdiplus::RectF& rc,
                   float radius,
                   const Gdiplus::Color& borderColor,
                   float strokeWidth = 1.0f) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rc, radius);
    Gdiplus::Pen pen(borderColor, strokeWidth);
    g.DrawPath(&pen, &path);
}

void DrawSearchMagnifierIcon(Gdiplus::Graphics& g,
                             float centerX,
                             float centerY,
                             float size,
                             const Gdiplus::Color& color,
                             float strokeWidth) {
    Gdiplus::Pen pen(color, strokeWidth);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);

    float radius = size * 0.36f;
    float circleCx = centerX - size * 0.08f;
    float circleCy = centerY - size * 0.08f;

    g.DrawEllipse(&pen,
                  circleCx - radius,
                  circleCy - radius,
                  radius * 2.0f,
                  radius * 2.0f);

    float handleStartOffset = radius * 0.72f;
    float handleEndOffset = size * 0.44f;
    g.DrawLine(&pen,
               circleCx + handleStartOffset,
               circleCy + handleStartOffset,
               centerX + handleEndOffset,
               centerY + handleEndOffset);
}

void DrawCloseCrossIcon(Gdiplus::Graphics& g,
                        const Gdiplus::RectF& rcBtn,
                        float halfExtent,
                        const Gdiplus::Color& color,
                        float strokeWidth) {
    Gdiplus::Pen pen(color, strokeWidth);
    pen.SetStartCap(Gdiplus::LineCapRound);
    pen.SetEndCap(Gdiplus::LineCapRound);

    float cx = rcBtn.X + rcBtn.Width * 0.5f;
    float cy = rcBtn.Y + rcBtn.Height * 0.5f;
    g.DrawLine(&pen, cx - halfExtent, cy - halfExtent, cx + halfExtent, cy + halfExtent);
    g.DrawLine(&pen, cx + halfExtent, cy - halfExtent, cx - halfExtent, cy + halfExtent);
}

void DrawFallbackFolderIcon(Gdiplus::Graphics& g,
                            int x,
                            int y,
                            int size,
                            const Gdiplus::Color& accentColor) {
    Gdiplus::RectF body(static_cast<float>(x + 1),
                        static_cast<float>(y + size * 0.28f),
                        static_cast<float>(size - 2),
                        static_cast<float>(size * 0.58f));
    Gdiplus::RectF tab(static_cast<float>(x + 1),
                       static_cast<float>(y + size * 0.16f),
                       static_cast<float>(size * 0.42f),
                       static_cast<float>(size * 0.22f));
    FillRoundRect(g, tab, 2.0f, accentColor);
    FillRoundRect(g, body, 2.5f, accentColor);
}

void UpdateDialogLayoutRects(HistoryDialogState* st, int clientW, int clientH) {
    UINT dpi = st->dpi;
    int pad = ScaleForDpi(18, dpi);
    int headerH = ScaleForDpi(68, dpi);
    int footerH = ScaleForDpi(62, dpi);
    int tableHeaderH = ScaleForDpi(38, dpi);

    int closeW = ScaleForDpi(44, dpi);
    int closeH = ScaleForDpi(32, dpi);
    st->rcCloseBtn = {clientW - closeW - ScaleForDpi(6, dpi),
                      ScaleForDpi(6, dpi),
                      clientW - ScaleForDpi(6, dpi),
                      ScaleForDpi(6, dpi) + closeH};

    int searchW = ScaleForDpi(290, dpi);
    int searchH = ScaleForDpi(34, dpi);
    int searchRight = st->rcCloseBtn.left - ScaleForDpi(12, dpi);
    int searchTop = ScaleForDpi(17, dpi);
    st->rcSearchBox = {searchRight - searchW, searchTop, searchRight, searchTop + searchH};

    int editLeft = st->rcSearchBox.left + ScaleForDpi(32, dpi);
    int editRight = st->rcSearchBox.right - ScaleForDpi(8, dpi);
    int editHeight = ScaleForDpi(20, dpi);
    int editTop = st->rcSearchBox.top + (searchH - editHeight) / 2;
    st->rcSearchEditInner = {editLeft, editTop, editRight, editTop + editHeight};

    st->rcTableCard = {pad, headerH, clientW - pad, clientH - footerH};
    st->rcTableHeader = {st->rcTableCard.left,
                         st->rcTableCard.top,
                         st->rcTableCard.right,
                         st->rcTableCard.top + tableHeaderH};
    st->rcTableBody = {st->rcTableCard.left,
                       st->rcTableHeader.bottom,
                       st->rcTableCard.right,
                       st->rcTableCard.bottom};

    int btnH = ScaleForDpi(34, dpi);
    int btnY = clientH - footerH + (footerH - btnH) / 2;

    int delOneW = ScaleForDpi(165, dpi);
    int clearAllW = ScaleForDpi(155, dpi);
    st->rcBtnDeleteOne = {pad, btnY, pad + delOneW, btnY + btnH};
    st->rcBtnClearAll = {st->rcBtnDeleteOne.right + ScaleForDpi(10, dpi),
                         btnY,
                         st->rcBtnDeleteOne.right + ScaleForDpi(10, dpi) + clearAllW,
                         btnY + btnH};

    int openNewW = ScaleForDpi(220, dpi);
    int openSameW = ScaleForDpi(200, dpi);
    st->rcBtnOpenNew = {clientW - pad - openNewW, btnY, clientW - pad, btnY + btnH};
    st->rcBtnOpenSame = {st->rcBtnOpenNew.left - ScaleForDpi(10, dpi) - openSameW,
                         btnY,
                         st->rcBtnOpenNew.left - ScaleForDpi(10, dpi),
                         btnY + btnH};
}

int GetVisibleRowCapacity(const HistoryDialogState* st) {
    int rowH = ScaleForDpi(40, st->dpi);
    int bodyH = st->rcTableBody.bottom - st->rcTableBody.top - ScaleForDpi(6, st->dpi);
    int cap = bodyH / rowH;
    return (cap > 1) ? cap : 1;
}

void EnsureSelectedRowVisible(HistoryDialogState* st, int totalRows) {
    if (totalRows <= 0) {
        st->selectedRow = 0;
        st->scrollOffsetRows = 0;
        return;
    }
    if (st->selectedRow < 0) {
        st->selectedRow = 0;
    }
    if (st->selectedRow >= totalRows) {
        st->selectedRow = totalRows - 1;
    }

    int visibleCap = GetVisibleRowCapacity(st);
    if (st->selectedRow < st->scrollOffsetRows) {
        st->scrollOffsetRows = st->selectedRow;
    } else if (st->selectedRow >= st->scrollOffsetRows + visibleCap) {
        st->scrollOffsetRows = st->selectedRow - visibleCap + 1;
    }

    int maxScroll = std::max(0, totalRows - visibleCap);
    if (st->scrollOffsetRows > maxScroll) {
        st->scrollOffsetRows = maxScroll;
    }
    if (st->scrollOffsetRows < 0) {
        st->scrollOffsetRows = 0;
    }
}

void ExecuteOpenSelectedHistoryRow(HistoryDialogState* st, bool forceSameTab) {
    auto rows = BuildFilteredHistoryRows(st->searchQuery);
    if (rows.empty()) {
        return;
    }
    EnsureSelectedRowVisible(st, static_cast<int>(rows.size()));
    const auto& chosen = rows[st->selectedRow];

    LPITEMIDLIST targetPidl = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (chosen.originalIndex < g_openHistory.size() &&
            g_openHistory[chosen.originalIndex].pidl) {
            targetPidl = ILClone(g_openHistory[chosen.originalIndex].pidl);
        }
    }

    HWND hCabinet = st->hTargetCabinet;
    DestroyWindow(st->hWnd);

    if (targetPidl) {
        OpenFolderPidlInCabinet(hCabinet, targetPidl, forceSameTab);
        ILFree(targetPidl);
    }
}

void DeleteSelectedHistoryRow(HistoryDialogState* st) {
    auto rows = BuildFilteredHistoryRows(st->searchQuery);
    if (rows.empty()) {
        return;
    }
    EnsureSelectedRowVisible(st, static_cast<int>(rows.size()));
    size_t origIdx = rows[st->selectedRow].originalIndex;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (origIdx < g_openHistory.size()) {
            g_openHistory.erase(g_openHistory.begin() + static_cast<ptrdiff_t>(origIdx));
        }
    }

    auto updatedRows = BuildFilteredHistoryRows(st->searchQuery);
    EnsureSelectedRowVisible(st, static_cast<int>(updatedRows.size()));
    InvalidateRect(st->hWnd, nullptr, FALSE);
}

void ClearAllHistoryRows(HistoryDialogState* st) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_openHistory.clear();
    }
    st->selectedRow = 0;
    st->scrollOffsetRows = 0;
    InvalidateRect(st->hWnd, nullptr, FALSE);
}

LRESULT CALLBACK SearchEditSubclassProc(HWND hEdit,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return DefSubclassProc(hEdit, uMsg, wParam, lParam);
    }

    auto* st = reinterpret_cast<HistoryDialogState*>(dwRefData);

    switch (uMsg) {
        case WM_GETDLGCODE:
            return DLGC_WANTALLKEYS | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTCHARS;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            if (wParam == VK_UP || wParam == VK_DOWN ||
                wParam == VK_PRIOR || wParam == VK_NEXT ||
                wParam == VK_RETURN || wParam == VK_ESCAPE ||
                wParam == VK_TAB ||
                (wParam == VK_DELETE && (ctrlDown || GetWindowTextLengthW(hEdit) == 0))) {
                if (st && st->hWnd) {
                    SendMessageW(st->hWnd, uMsg, wParam, lParam);
                }
                return 0;
            }
            break;
        }

        case WM_CHAR: {
            if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_TAB) {
                return 0;
            }
            break;
        }

        case WM_PAINT: {
            LRESULT res = DefSubclassProc(hEdit, uMsg, wParam, lParam);

            if (st && GetWindowTextLengthW(hEdit) == 0) {
                bool isComposing = false;
                HIMC hImc = ImmGetContext(hEdit);
                if (hImc) {
                    LONG compLen = ImmGetCompositionStringW(hImc, GCS_COMPSTR, nullptr, 0);
                    if (compLen > 0) {
                        isComposing = true;
                    }
                    ImmReleaseContext(hEdit, hImc);
                }

                if (!isComposing) {
                    const LocalizedStrings& loc = GetLoc();
                    HideCaret(hEdit);
                    HDC hdc = GetDC(hEdit);
                    if (hdc) {
                        RECT rcEdit;
                        GetClientRect(hEdit, &rcEdit);
                        rcEdit.left += ScaleForDpi(3, st->dpi);

                        SetBkMode(hdc, TRANSPARENT);
                        SetTextColor(hdc, st->darkMode ? RGB(155, 155, 155) : RGB(115, 115, 115));
                        HGDIOBJ hOldFont = SelectObject(
                            hdc, st->hSearchFont ? st->hSearchFont : GetStockObject(DEFAULT_GUI_FONT));

                        DrawTextW(hdc, loc.searchPlaceholder, -1, &rcEdit,
                                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);

                        SelectObject(hdc, hOldFont);
                        ReleaseDC(hEdit, hdc);
                    }
                    ShowCaret(hEdit);
                }
            }
            return res;
        }
    }

    return DefSubclassProc(hEdit, uMsg, wParam, lParam);
}

void DrawWinUI3Button(Gdiplus::Graphics& g,
                      const RECT& rc,
                      PCWSTR text,
                      bool isPrimary,
                      bool isHovered,
                      bool darkMode,
                      Gdiplus::Font* font) {
    Gdiplus::RectF rf(static_cast<float>(rc.left),
                      static_cast<float>(rc.top),
                      static_cast<float>(rc.right - rc.left),
                      static_cast<float>(rc.bottom - rc.top));

    Gdiplus::Color bgColor;
    Gdiplus::Color borderColor;
    Gdiplus::Color textColor;

    if (isPrimary) {
        if (darkMode) {
            bgColor = isHovered ? Gdiplus::Color(255, 118, 214, 255)
                                : Gdiplus::Color(255, 96, 205, 255);
            borderColor = Gdiplus::Color(255, 130, 218, 255);
            textColor = Gdiplus::Color(255, 16, 16, 16);
        } else {
            bgColor = isHovered ? Gdiplus::Color(255, 20, 115, 205)
                                : Gdiplus::Color(255, 0, 103, 192);
            borderColor = Gdiplus::Color(255, 0, 90, 175);
            textColor = Gdiplus::Color(255, 255, 255, 255);
        }
    } else {
        if (darkMode) {
            bgColor = isHovered ? Gdiplus::Color(255, 62, 62, 62)
                                : Gdiplus::Color(255, 48, 48, 48);
            borderColor = Gdiplus::Color(255, 70, 70, 70);
            textColor = Gdiplus::Color(255, 242, 242, 242);
        } else {
            bgColor = isHovered ? Gdiplus::Color(255, 238, 238, 238)
                                : Gdiplus::Color(255, 251, 251, 251);
            borderColor = Gdiplus::Color(255, 212, 212, 212);
            textColor = Gdiplus::Color(255, 28, 28, 28);
        }
    }

    FillRoundRect(g, rf, 6.0f, bgColor);
    DrawRoundRect(g, rf, 6.0f, borderColor, 1.0f);

    Gdiplus::SolidBrush textBrush(textColor);
    Gdiplus::StringFormat sf;
    sf.SetAlignment(Gdiplus::StringAlignmentCenter);
    sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    sf.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    sf.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    g.DrawString(text, -1, font, rf, &sf, &textBrush);
}

void PaintWinUI3HistoryDialog(HWND hWnd, HDC hdc, HistoryDialogState* st) {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int clientW = rcClient.right - rcClient.left;
    int clientH = rcClient.bottom - rcClient.top;
    if (clientW <= 0 || clientH <= 0) {
        return;
    }

    UpdateDialogLayoutRects(st, clientW, clientH);
    const LocalizedStrings& loc = GetLoc();

    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateCompatibleBitmap(hdc, clientW, clientH);
    HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hBmp));

    Gdiplus::Graphics g(hMemDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    bool dark = st->darkMode;
    UINT dpi = st->dpi;

    Gdiplus::Color colBg = dark ? Gdiplus::Color(245, 31, 31, 31)
                                : Gdiplus::Color(248, 243, 243, 243);
    Gdiplus::Color colWindowBorder = dark ? Gdiplus::Color(255, 62, 62, 62)
                                          : Gdiplus::Color(255, 206, 206, 206);
    Gdiplus::Color colCardBg = dark ? Gdiplus::Color(255, 40, 40, 40)
                                    : Gdiplus::Color(255, 255, 255, 255);
    Gdiplus::Color colCardBorder = dark ? Gdiplus::Color(255, 58, 58, 58)
                                        : Gdiplus::Color(255, 226, 226, 226);
    Gdiplus::Color colTableHeaderBg = dark ? Gdiplus::Color(255, 48, 48, 48)
                                           : Gdiplus::Color(255, 247, 247, 247);
    Gdiplus::Color colPrimaryText = dark ? Gdiplus::Color(255, 245, 245, 245)
                                         : Gdiplus::Color(255, 24, 24, 24);
    Gdiplus::Color colSecondaryText = dark ? Gdiplus::Color(255, 175, 175, 175)
                                           : Gdiplus::Color(255, 98, 98, 98);
    Gdiplus::Color colAccent = dark ? Gdiplus::Color(255, 96, 205, 255)
                                    : Gdiplus::Color(255, 0, 103, 192);
    Gdiplus::Color colRowHover = dark ? Gdiplus::Color(255, 52, 52, 52)
                                      : Gdiplus::Color(255, 241, 244, 249);
    Gdiplus::Color colRowSelected = dark ? Gdiplus::Color(255, 58, 63, 70)
                                         : Gdiplus::Color(255, 232, 242, 252);
    Gdiplus::Color colRowAlt = dark ? Gdiplus::Color(110, 44, 44, 44)
                                    : Gdiplus::Color(160, 250, 250, 250);

    Gdiplus::SolidBrush bgBrush(colBg);
    g.FillRectangle(&bgBrush, 0, 0, clientW, clientH);

    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font fontTitle(&fontFamily, static_cast<float>(ScaleForDpi(15, dpi)),
                            Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontSubtitle(&fontFamily, static_cast<float>(ScaleForDpi(12, dpi)),
                               Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font fontHeader(&fontFamily, static_cast<float>(ScaleForDpi(12, dpi)),
                             Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontCellBold(&fontFamily, static_cast<float>(ScaleForDpi(13, dpi)),
                               Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontCell(&fontFamily, static_cast<float>(ScaleForDpi(13, dpi)),
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font fontMono(&fontFamily, static_cast<float>(ScaleForDpi(12, dpi)),
                           Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    auto rows = BuildFilteredHistoryRows(st->searchQuery);
    EnsureSelectedRowVisible(st, static_cast<int>(rows.size()));

    int pad = ScaleForDpi(18, dpi);
    Gdiplus::SolidBrush primaryBrush(colPrimaryText);
    Gdiplus::SolidBrush secondaryBrush(colSecondaryText);
    Gdiplus::SolidBrush accentBrush(colAccent);

    Gdiplus::RectF rcTitleBadge(static_cast<float>(pad),
                                static_cast<float>(ScaleForDpi(16, dpi)),
                                static_cast<float>(ScaleForDpi(4, dpi)),
                                static_cast<float>(ScaleForDpi(20, dpi)));
    FillRoundRect(g, rcTitleBadge, 2.0f, colAccent);

    Gdiplus::StringFormat sfLeftCenter;
    sfLeftCenter.SetAlignment(Gdiplus::StringAlignmentNear);
    sfLeftCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    sfLeftCenter.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    sfLeftCenter.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);

    float titleLeft = static_cast<float>(pad + ScaleForDpi(12, dpi));
    float maxTitleWidth = static_cast<float>(st->rcSearchBox.left - ScaleForDpi(14, dpi)) - titleLeft;
    if (maxTitleWidth < 80.0f) {
        maxTitleWidth = 80.0f;
    }

    Gdiplus::RectF rfTitle(titleLeft,
                           static_cast<float>(ScaleForDpi(12, dpi)),
                           maxTitleWidth,
                           static_cast<float>(ScaleForDpi(24, dpi)));
    g.DrawString(loc.windowTitle, -1, &fontTitle, rfTitle, &sfLeftCenter, &primaryBrush);

    WCHAR subtitleBuf[256];
    wsprintfW(subtitleBuf, loc.subtitleFormat, static_cast<int>(rows.size()));
    Gdiplus::RectF rfSub(titleLeft,
                         static_cast<float>(ScaleForDpi(37, dpi)),
                         maxTitleWidth,
                         static_cast<float>(ScaleForDpi(20, dpi)));
    g.DrawString(subtitleBuf, -1, &fontSubtitle, rfSub, &sfLeftCenter, &secondaryBrush);

    // Khung bao ngoài của ô Tìm kiếm chuẩn WinUI 3
    Gdiplus::RectF rfSearch(static_cast<float>(st->rcSearchBox.left),
                            static_cast<float>(st->rcSearchBox.top),
                            static_cast<float>(st->rcSearchBox.right - st->rcSearchBox.left),
                            static_cast<float>(st->rcSearchBox.bottom - st->rcSearchBox.top));
    Gdiplus::Color searchBg = dark ? Gdiplus::Color(255, 45, 45, 45)
                                   : Gdiplus::Color(255, 255, 255, 255);
    FillRoundRect(g, rfSearch, 6.0f, searchBg);
    DrawRoundRect(g, rfSearch, 6.0f, colCardBorder, 1.0f);

    Gdiplus::RectF rfSearchAccent(rfSearch.X + 4.0f,
                                  rfSearch.Y + rfSearch.Height - 2.0f,
                                  rfSearch.Width - 8.0f,
                                  2.0f);
    FillRoundRect(g, rfSearchAccent, 1.0f, colAccent);

    float magSize = static_cast<float>(ScaleForDpi(13, dpi));
    float magCenterX = rfSearch.X + static_cast<float>(ScaleForDpi(16, dpi));
    float magCenterY = rfSearch.Y + rfSearch.Height * 0.5f;
    float magStroke = std::max(1.4f, static_cast<float>(dpi) / 64.0f);
    DrawSearchMagnifierIcon(g, magCenterX, magCenterY, magSize,
                            st->searchQuery.empty() ? colSecondaryText : colAccent,
                            magStroke);

    // Nút Đóng (X) góc phải trên
    Gdiplus::RectF rfClose(static_cast<float>(st->rcCloseBtn.left),
                           static_cast<float>(st->rcCloseBtn.top),
                           static_cast<float>(st->rcCloseBtn.right - st->rcCloseBtn.left),
                           static_cast<float>(st->rcCloseBtn.bottom - st->rcCloseBtn.top));
    if (st->hoverButtonId == 1) {
        FillRoundRect(g, rfClose, 6.0f, Gdiplus::Color(255, 196, 43, 28));
    }
    Gdiplus::Color closeIconColor =
        (st->hoverButtonId == 1) ? Gdiplus::Color(255, 255, 255, 255) : colPrimaryText;
    DrawCloseCrossIcon(g, rfClose,
                       static_cast<float>(ScaleForDpi(5, dpi)),
                       closeIconColor,
                       std::max(1.5f, static_cast<float>(dpi) / 64.0f));

    Gdiplus::StringFormat sfCenter;
    sfCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
    sfCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // Khung Card chứa Bảng 4 cột
    Gdiplus::RectF rfCard(static_cast<float>(st->rcTableCard.left),
                          static_cast<float>(st->rcTableCard.top),
                          static_cast<float>(st->rcTableCard.right - st->rcTableCard.left),
                          static_cast<float>(st->rcTableCard.bottom - st->rcTableCard.top));
    FillRoundRect(g, rfCard, 8.0f, colCardBg);

    Gdiplus::RectF rfHeader(static_cast<float>(st->rcTableHeader.left),
                            static_cast<float>(st->rcTableHeader.top),
                            static_cast<float>(st->rcTableHeader.right - st->rcTableHeader.left),
                            static_cast<float>(st->rcTableHeader.bottom - st->rcTableHeader.top));
    FillRoundRect(g, rfHeader, 8.0f, colTableHeaderBg);

    int tableLeft = st->rcTableCard.left;
    int tableW = st->rcTableCard.right - st->rcTableCard.left - ScaleForDpi(10, dpi);
    int col1W = ScaleForDpi(58, dpi);
    int col2W = ScaleForDpi(230, dpi);
    int col4W = ScaleForDpi(175, dpi);
    int col3W = std::max(ScaleForDpi(180, dpi), tableW - col1W - col2W - col4W);

    int col1X = tableLeft;
    int col2X = col1X + col1W;
    int col3X = col2X + col2W;
    int col4X = col3X + col3W;

    float headerY = static_cast<float>(st->rcTableHeader.top);
    float headerH = static_cast<float>(st->rcTableHeader.bottom - st->rcTableHeader.top);

    g.DrawString(loc.colIndex, -1, &fontHeader,
                 Gdiplus::RectF(static_cast<float>(col1X), headerY,
                                static_cast<float>(col1W), headerH),
                 &sfCenter, &secondaryBrush);

    g.DrawString(loc.colName, -1, &fontHeader,
                 Gdiplus::RectF(static_cast<float>(col2X + ScaleForDpi(8, dpi)), headerY,
                                static_cast<float>(col2W - ScaleForDpi(12, dpi)), headerH),
                 &sfLeftCenter, &secondaryBrush);

    g.DrawString(loc.colPath, -1, &fontHeader,
                 Gdiplus::RectF(static_cast<float>(col3X + ScaleForDpi(8, dpi)), headerY,
                                static_cast<float>(col3W - ScaleForDpi(12, dpi)), headerH),
                 &sfLeftCenter, &secondaryBrush);

    g.DrawString(loc.colRecentTime, -1, &fontHeader,
                 Gdiplus::RectF(static_cast<float>(col4X + ScaleForDpi(8, dpi)), headerY,
                                static_cast<float>(col4W - ScaleForDpi(12, dpi)), headerH),
                 &sfLeftCenter, &secondaryBrush);

    Gdiplus::Pen dividerPen(colCardBorder, 1.0f);
    g.DrawLine(&dividerPen,
               static_cast<float>(st->rcTableHeader.left),
               static_cast<float>(st->rcTableHeader.bottom),
               static_cast<float>(st->rcTableHeader.right),
               static_cast<float>(st->rcTableHeader.bottom));
    g.DrawLine(&dividerPen, static_cast<float>(col2X), headerY + 8.0f,
               static_cast<float>(col2X), headerY + headerH - 8.0f);
    g.DrawLine(&dividerPen, static_cast<float>(col3X), headerY + 8.0f,
               static_cast<float>(col3X), headerY + headerH - 8.0f);
    g.DrawLine(&dividerPen, static_cast<float>(col4X), headerY + 8.0f,
               static_cast<float>(col4X), headerY + headerH - 8.0f);

    int rowH = ScaleForDpi(40, dpi);
    int visibleCap = GetVisibleRowCapacity(st);
    int bodyTop = st->rcTableBody.top + ScaleForDpi(3, dpi);

    if (rows.empty()) {
        Gdiplus::RectF rfEmpty(static_cast<float>(st->rcTableBody.left),
                               static_cast<float>(st->rcTableBody.top),
                               static_cast<float>(st->rcTableBody.right - st->rcTableBody.left),
                               static_cast<float>(st->rcTableBody.bottom - st->rcTableBody.top));
        PCWSTR emptyMsg = st->searchQuery.empty() ? loc.emptyHistory : loc.emptySearchResult;
        g.DrawString(emptyMsg, -1, &fontCell, rfEmpty, &sfCenter, &secondaryBrush);
    } else {
        int endIdx = std::min(static_cast<int>(rows.size()), st->scrollOffsetRows + visibleCap);
        int iconSize = ScaleForDpi(18, dpi);

        for (int idx = st->scrollOffsetRows; idx < endIdx; ++idx) {
            int visIdx = idx - st->scrollOffsetRows;
            int rowTop = bodyTop + visIdx * rowH;
            const auto& row = rows[idx];

            Gdiplus::RectF rfRow(static_cast<float>(st->rcTableCard.left + ScaleForDpi(4, dpi)),
                                 static_cast<float>(rowTop + 1),
                                 static_cast<float>(st->rcTableCard.right - st->rcTableCard.left -
                                                    ScaleForDpi(14, dpi)),
                                 static_cast<float>(rowH - 2));

            bool isSelected = (idx == st->selectedRow);
            bool isHovered = (idx == st->hoverRow);

            if (isSelected) {
                FillRoundRect(g, rfRow, 6.0f, colRowSelected);
                Gdiplus::RectF rfPill(rfRow.X + 2.0f,
                                      rfRow.Y + static_cast<float>(ScaleForDpi(8, dpi)),
                                      static_cast<float>(ScaleForDpi(3, dpi)),
                                      rfRow.Height - static_cast<float>(ScaleForDpi(16, dpi)));
                FillRoundRect(g, rfPill, 1.5f, colAccent);
            } else if (isHovered) {
                FillRoundRect(g, rfRow, 6.0f, colRowHover);
            } else if (idx % 2 == 1) {
                FillRoundRect(g, rfRow, 4.0f, colRowAlt);
            }

            WCHAR sttText[16];
            wsprintfW(sttText, L"%d", row.stt);
            g.DrawString(sttText, -1, isSelected ? &fontCellBold : &fontCell,
                         Gdiplus::RectF(static_cast<float>(col1X),
                                        static_cast<float>(rowTop),
                                        static_cast<float>(col1W),
                                        static_cast<float>(rowH)),
                         &sfCenter, isSelected ? &accentBrush : &secondaryBrush);

            int iconX = col2X + ScaleForDpi(10, dpi);
            int iconY = rowTop + (rowH - iconSize) / 2;
            bool iconDrawn = false;
            if (row.pidl) {
                SHFILEINFOW sfi = {};
                if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(row.pidl), 0, &sfi, sizeof(sfi),
                                   SHGFI_PIDL | SHGFI_ICON | SHGFI_SMALLICON) &&
                    sfi.hIcon) {
                    DrawIconEx(hMemDC, iconX, iconY, sfi.hIcon, iconSize, iconSize,
                               0, nullptr, DI_NORMAL);
                    DestroyIcon(sfi.hIcon);
                    iconDrawn = true;
                }
            }
            if (!iconDrawn) {
                DrawFallbackFolderIcon(g, iconX, iconY, iconSize, colAccent);
            }

            int nameTextX = iconX + iconSize + ScaleForDpi(8, dpi);
            int nameTextW = col2X + col2W - nameTextX - ScaleForDpi(6, dpi);
            g.DrawString(row.itemName.c_str(), -1,
                         isSelected ? &fontCellBold : &fontCell,
                         Gdiplus::RectF(static_cast<float>(nameTextX),
                                        static_cast<float>(rowTop),
                                        static_cast<float>(nameTextW),
                                        static_cast<float>(rowH)),
                         &sfLeftCenter, &primaryBrush);

            g.DrawString(row.fullPath.c_str(), -1, &fontMono,
                         Gdiplus::RectF(static_cast<float>(col3X + ScaleForDpi(8, dpi)),
                                        static_cast<float>(rowTop),
                                        static_cast<float>(col3W - ScaleForDpi(14, dpi)),
                                        static_cast<float>(rowH)),
                         &sfLeftCenter, &secondaryBrush);

            g.DrawString(row.lastOpenedTime.c_str(), -1, &fontMono,
                         Gdiplus::RectF(static_cast<float>(col4X + ScaleForDpi(8, dpi)),
                                        static_cast<float>(rowTop),
                                        static_cast<float>(col4W - ScaleForDpi(12, dpi)),
                                        static_cast<float>(rowH)),
                         &sfLeftCenter, isSelected ? &primaryBrush : &secondaryBrush);
        }

        if (static_cast<int>(rows.size()) > visibleCap) {
            float trackX = static_cast<float>(st->rcTableCard.right - ScaleForDpi(7, dpi));
            float trackY = static_cast<float>(st->rcTableBody.top + ScaleForDpi(4, dpi));
            float trackH = static_cast<float>(st->rcTableBody.bottom - st->rcTableBody.top -
                                              ScaleForDpi(8, dpi));
            float ratio = static_cast<float>(visibleCap) / static_cast<float>(rows.size());
            float thumbH = std::max(static_cast<float>(ScaleForDpi(24, dpi)), trackH * ratio);
            int maxScroll = static_cast<int>(rows.size()) - visibleCap;
            float scrollRatio = (maxScroll > 0)
                                    ? (static_cast<float>(st->scrollOffsetRows) /
                                       static_cast<float>(maxScroll))
                                    : 0.0f;
            float thumbY = trackY + (trackH - thumbH) * scrollRatio;

            Gdiplus::RectF rfThumb(trackX, thumbY,
                                   static_cast<float>(ScaleForDpi(4, dpi)), thumbH);
            FillRoundRect(g, rfThumb, 2.0f,
                          dark ? Gdiplus::Color(160, 150, 150, 150)
                               : Gdiplus::Color(160, 120, 120, 120));
        }
    }

    DrawRoundRect(g, rfCard, 8.0f, colCardBorder, 1.0f);

    DrawWinUI3Button(g, st->rcBtnDeleteOne, loc.btnDeleteOne,
                     false, st->hoverButtonId == 4, dark, &fontSubtitle);
    DrawWinUI3Button(g, st->rcBtnClearAll, loc.btnClearAll,
                     false, st->hoverButtonId == 5, dark, &fontSubtitle);
    DrawWinUI3Button(g, st->rcBtnOpenSame, loc.btnOpenSameTab,
                     false, st->hoverButtonId == 3, dark, &fontSubtitle);
    DrawWinUI3Button(g, st->rcBtnOpenNew, loc.btnOpenNewTab,
                     true, st->hoverButtonId == 2, dark, &fontHeader);

    Gdiplus::RectF rfOuter(0.5f, 0.5f,
                           static_cast<float>(clientW - 1),
                           static_cast<float>(clientH - 1));
    DrawRoundRect(g, rfOuter, 8.0f, colWindowBorder, 1.0f);

    BitBlt(hdc, 0, 0, clientW, clientH, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);
}

int HitTestTableRow(const HistoryDialogState* st, POINT pt, int totalRows) {
    if (!PtInRect(&st->rcTableBody, pt)) {
        return -1;
    }
    int rowH = ScaleForDpi(40, st->dpi);
    int bodyTop = st->rcTableBody.top + ScaleForDpi(3, st->dpi);
    if (pt.y < bodyTop) {
        return -1;
    }
    int visIdx = (pt.y - bodyTop) / rowH;
    int visibleCap = GetVisibleRowCapacity(st);
    if (visIdx < 0 || visIdx >= visibleCap) {
        return -1;
    }
    int actualIdx = st->scrollOffsetRows + visIdx;
    if (actualIdx < 0 || actualIdx >= totalRows) {
        return -1;
    }
    return actualIdx;
}

int HitTestDialogButtons(const HistoryDialogState* st, POINT pt) {
    if (PtInRect(&st->rcCloseBtn, pt)) return 1;
    if (PtInRect(&st->rcBtnOpenNew, pt)) return 2;
    if (PtInRect(&st->rcBtnOpenSame, pt)) return 3;
    if (PtInRect(&st->rcBtnDeleteOne, pt)) return 4;
    if (PtInRect(&st->rcBtnClearAll, pt)) return 5;
    return 0;
}

LRESULT CALLBACK HistoryDialogWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto* st = reinterpret_cast<HistoryDialogState*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_NCCREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto* initSt = reinterpret_cast<HistoryDialogState*>(cs->lpCreateParams);
            initSt->hWnd = hWnd;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(initSt));
            return TRUE;
        }

        case WM_CREATE: {
            if (st) {
                st->dpi = GetWindowDpiSafe(hWnd);
                st->darkMode = IsSystemDarkMode();
                ApplyWinUI3WindowBackdrop(hWnd, st->darkMode);

                RECT rcClient = {};
                GetClientRect(hWnd, &rcClient);
                UpdateDialogLayoutRects(st, rcClient.right - rcClient.left,
                                        rcClient.bottom - rcClient.top);

                st->hSearchFont = CreateFontW(
                    -ScaleForDpi(13, st->dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

                st->hSearchBgBrush = CreateSolidBrush(
                    st->darkMode ? RGB(45, 45, 45) : RGB(255, 255, 255));

                int editW = st->rcSearchEditInner.right - st->rcSearchEditInner.left;
                int editH = st->rcSearchEditInner.bottom - st->rcSearchEditInner.top;
                st->hSearchEdit = CreateWindowExW(
                    0, L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
                    st->rcSearchEditInner.left,
                    st->rcSearchEditInner.top,
                    editW, editH,
                    hWnd,
                    reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_HISTORY_SEARCH_EDIT)),
                    GetModuleHandleW(nullptr),
                    nullptr);

                if (st->hSearchEdit) {
                    if (st->hSearchFont) {
                        SendMessageW(st->hSearchEdit, WM_SETFONT,
                                     reinterpret_cast<WPARAM>(st->hSearchFont), TRUE);
                    }
                    SetWindowTheme(st->hSearchEdit,
                                   st->darkMode ? L"DarkMode_CFD" : L"Explorer",
                                   nullptr);
                    SendMessageW(st->hSearchEdit, EM_SETMARGINS,
                                 EC_LEFTMARGIN | EC_RIGHTMARGIN,
                                 MAKELONG(ScaleForDpi(2, st->dpi), ScaleForDpi(2, st->dpi)));
                    WindhawkUtils::SetWindowSubclassFromAnyThread(
                        st->hSearchEdit, SearchEditSubclassProc,
                        reinterpret_cast<DWORD_PTR>(st));
                }
            }
            return 0;
        }

        case WM_SIZE: {
            if (st) {
                int w = LOWORD(lParam);
                int h = HIWORD(lParam);
                UpdateDialogLayoutRects(st, w, h);
                if (st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                    SetWindowPos(st->hSearchEdit, nullptr,
                                 st->rcSearchEditInner.left,
                                 st->rcSearchEditInner.top,
                                 st->rcSearchEditInner.right - st->rcSearchEditInner.left,
                                 st->rcSearchEditInner.bottom - st->rcSearchEditInner.top,
                                 SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
            return 0;
        }

        case WM_CTLCOLOREDIT: {
            if (st && reinterpret_cast<HWND>(lParam) == st->hSearchEdit) {
                HDC hdcEdit = reinterpret_cast<HDC>(wParam);
                SetTextColor(hdcEdit, st->darkMode ? RGB(245, 245, 245) : RGB(24, 24, 24));
                SetBkColor(hdcEdit, st->darkMode ? RGB(45, 45, 45) : RGB(255, 255, 255));
                return reinterpret_cast<LRESULT>(st->hSearchBgBrush);
            }
            break;
        }

        case WM_COMMAND: {
            if (st && LOWORD(wParam) == IDC_HISTORY_SEARCH_EDIT &&
                HIWORD(wParam) == EN_CHANGE) {
                int len = GetWindowTextLengthW(st->hSearchEdit);
                std::wstring newText;
                if (len > 0) {
                    newText.resize(static_cast<size_t>(len));
                    GetWindowTextW(st->hSearchEdit, newText.data(), len + 1);
                }
                if (newText != st->searchQuery) {
                    st->searchQuery = std::move(newText);
                    st->selectedRow = 0;
                    st->scrollOffsetRows = 0;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    if (st->hSearchEdit) {
                        InvalidateRect(st->hSearchEdit, nullptr, FALSE);
                    }
                }
                return 0;
            }
            break;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (st) {
                PaintWinUI3HistoryDialog(hWnd, hdc, st);
            }
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!st) break;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            auto rows = BuildFilteredHistoryRows(st->searchQuery);
            int newHoverRow = HitTestTableRow(st, pt, static_cast<int>(rows.size()));
            int newHoverBtn = HitTestDialogButtons(st, pt);

            if (newHoverRow != st->hoverRow || newHoverBtn != st->hoverButtonId) {
                st->hoverRow = newHoverRow;
                st->hoverButtonId = newHoverBtn;
                InvalidateRect(hWnd, nullptr, FALSE);
            }

            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (st && (st->hoverRow != -1 || st->hoverButtonId != 0)) {
                st->hoverRow = -1;
                st->hoverButtonId = 0;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (!st) break;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

            if (PtInRect(&st->rcSearchBox, pt)) {
                if (st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                    SetFocus(st->hSearchEdit);
                }
                return 0;
            }

            int btnId = HitTestDialogButtons(st, pt);
            if (btnId == 1) {
                DestroyWindow(hWnd);
                return 0;
            }
            if (btnId == 2) {
                ExecuteOpenSelectedHistoryRow(st, false);
                return 0;
            }
            if (btnId == 3) {
                ExecuteOpenSelectedHistoryRow(st, true);
                return 0;
            }
            if (btnId == 4) {
                DeleteSelectedHistoryRow(st);
                if (st->hSearchEdit) SetFocus(st->hSearchEdit);
                return 0;
            }
            if (btnId == 5) {
                ClearAllHistoryRows(st);
                if (st->hSearchEdit) SetFocus(st->hSearchEdit);
                return 0;
            }

            auto rows = BuildFilteredHistoryRows(st->searchQuery);
            int clickedRow = HitTestTableRow(st, pt, static_cast<int>(rows.size()));
            if (clickedRow >= 0) {
                st->selectedRow = clickedRow;
                InvalidateRect(hWnd, nullptr, FALSE);
                if (st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                    SetFocus(st->hSearchEdit);
                }
                return 0;
            }

            if (pt.y < st->rcTableCard.top && !PtInRect(&st->rcSearchBox, pt)) {
                ReleaseCapture();
                SendMessageW(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                if (st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                    SetFocus(st->hSearchEdit);
                }
                return 0;
            }
            return 0;
        }

        case WM_LBUTTONDBLCLK: {
            if (!st) break;
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            auto rows = BuildFilteredHistoryRows(st->searchQuery);
            int clickedRow = HitTestTableRow(st, pt, static_cast<int>(rows.size()));
            if (clickedRow >= 0) {
                st->selectedRow = clickedRow;
                bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                ExecuteOpenSelectedHistoryRow(st, shiftDown);
                return 0;
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            if (!st) break;
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            auto rows = BuildFilteredHistoryRows(st->searchQuery);
            int visibleCap = GetVisibleRowCapacity(st);
            int maxScroll = std::max(0, static_cast<int>(rows.size()) - visibleCap);
            if (delta < 0) {
                st->scrollOffsetRows = std::min(maxScroll, st->scrollOffsetRows + 2);
            } else if (delta > 0) {
                st->scrollOffsetRows = std::max(0, st->scrollOffsetRows - 2);
            }
            InvalidateRect(hWnd, nullptr, FALSE);
            return 0;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (!st) break;
            auto rows = BuildFilteredHistoryRows(st->searchQuery);
            int total = static_cast<int>(rows.size());
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            if (wParam == VK_ESCAPE) {
                if (!st->searchQuery.empty() && st->hSearchEdit) {
                    SetWindowTextW(st->hSearchEdit, L"");
                    st->searchQuery.clear();
                    st->selectedRow = 0;
                    st->scrollOffsetRows = 0;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    InvalidateRect(st->hSearchEdit, nullptr, FALSE);
                } else {
                    DestroyWindow(hWnd);
                }
                return 0;
            }

            if (wParam == VK_RETURN) {
                ExecuteOpenSelectedHistoryRow(st, shiftDown);
                return 0;
            }

            if (wParam == VK_DELETE) {
                DeleteSelectedHistoryRow(st);
                return 0;
            }

            if (wParam == VK_TAB) {
                if (total > 0) {
                    st->selectedRow = (st->selectedRow + 1) % total;
                    EnsureSelectedRowVisible(st, total);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            if (wParam == VK_DOWN) {
                if (total > 0) {
                    st->selectedRow = (st->selectedRow + 1) % total;
                    EnsureSelectedRowVisible(st, total);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            if (wParam == VK_UP) {
                if (total > 0) {
                    st->selectedRow = (st->selectedRow - 1 + total) % total;
                    EnsureSelectedRowVisible(st, total);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            if (wParam == VK_NEXT) {
                if (total > 0) {
                    st->selectedRow = std::min(total - 1,
                                               st->selectedRow + GetVisibleRowCapacity(st));
                    EnsureSelectedRowVisible(st, total);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            if (wParam == VK_PRIOR) {
                if (total > 0) {
                    st->selectedRow = std::max(0,
                                               st->selectedRow - GetVisibleRowCapacity(st));
                    EnsureSelectedRowVisible(st, total);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }
            break;
        }

        case WM_SETFOCUS: {
            if (st && st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                SetFocus(st->hSearchEdit);
            }
            return 0;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                HWND hNewActive = reinterpret_cast<HWND>(lParam);
                if (hNewActive && (IsHistoryDialogOrChild(hNewActive) ||
                                   IsImeOrTsfPopupWindow(hNewActive))) {
                    return 0;
                }
                PostMessageW(hWnd, WM_APP_DEFERRED_CLOSE_CHECK, 0, 0);
            } else if (st && st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                SetFocus(st->hSearchEdit);
            }
            return 0;
        }

        case WM_APP_DEFERRED_CLOSE_CHECK: {
            HWND hFg = GetForegroundWindow();
            HWND hFocus = GetFocus();
            if (!IsHistoryDialogOrChild(hFg) && !IsHistoryDialogOrChild(hFocus) &&
                !IsImeOrTsfPopupWindow(hFg)) {
                DestroyWindow(hWnd);
            }
            return 0;
        }

        case WM_DESTROY: {
            if (st && st->hSearchEdit && IsWindow(st->hSearchEdit)) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    st->hSearchEdit, SearchEditSubclassProc);
            }
            break;
        }

        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                if (g_hHistoryDialog == hWnd) {
                    g_hHistoryDialog = nullptr;
                }
            }
            delete st;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool EnsureHistoryDialogClassRegistered() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_historyDialogAtom != 0) {
        return true;
    }

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = HistoryDialogWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = HISTORY_DIALOG_CLASS_NAME;

    g_historyDialogAtom = RegisterClassExW(&wc);
    return g_historyDialogAtom != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

void ShowOrCycleHistoryDialog(HWND hCabinet) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    if (hCabinet && IsWindow(hCabinet)) {
        HWND hActiveTab = FindActiveShellTabWindow(hCabinet);
        if (hActiveTab) {
            UpdateTabLocationSnapshot(hActiveTab);
        }
    }

    HWND hExisting = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        hExisting = g_hHistoryDialog;
    }

    if (hExisting && IsWindow(hExisting)) {
        SetForegroundWindow(hExisting);
        SendMessageW(hExisting, WM_KEYDOWN, VK_TAB, 0);
        return;
    }

    if (!EnsureHistoryDialogClassRegistered()) {
        return;
    }

    UINT dpi = GetWindowDpiSafe(hCabinet);
    int dlgW = ScaleForDpi(860, dpi);
    int dlgH = ScaleForDpi(520, dpi);

    RECT rcParent = {};
    if (hCabinet && IsWindow(hCabinet) && GetWindowRect(hCabinet, &rcParent)) {
        // Căn giữa hộp thoại bên trên cửa sổ Explorer hiện tại
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcParent, 0);
    }

    int posX = rcParent.left + ((rcParent.right - rcParent.left) - dlgW) / 2;
    int posY = rcParent.top + ((rcParent.bottom - rcParent.top) - dlgH) / 2;

    auto* st = new HistoryDialogState();
    st->hTargetCabinet = hCabinet;
    st->dpi = dpi;
    st->darkMode = IsSystemDarkMode();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        st->selectedRow = (g_openHistory.size() > 1) ? 1 : 0;
    }

    const LocalizedStrings& loc = GetLoc();
    HWND hDlg = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        HISTORY_DIALOG_CLASS_NAME,
        loc.windowTitle,
        WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN,
        posX, posY, dlgW, dlgH,
        hCabinet, nullptr, GetModuleHandleW(nullptr), st);

    if (!hDlg) {
        delete st;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_hHistoryDialog = hDlg;
    }

    ShowWindow(hDlg, SW_SHOW);
    SetForegroundWindow(hDlg);
    if (st->hSearchEdit && IsWindow(st->hSearchEdit)) {
        SetFocus(st->hSearchEdit);
    } else {
        SetFocus(hDlg);
    }
}

// ---------------------------------------------------------------------------
// Subclass cho ShellTabWindowClass và CabinetWClass
// ---------------------------------------------------------------------------
LRESULT CALLBACK ShellTabSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_SETTEXT: {
            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            bool restoring = false;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_activeTabs.find(hWnd);
                if (it != g_activeTabs.end()) {
                    restoring = it->second.isRestoring;
                }
            }
            if (restoring) {
                TryNavigateRestoringTab(hWnd);
            } else {
                UpdateTabLocationSnapshot(hWnd);
            }
            return res;
        }

        case WM_TIMER:
            if (wParam == IDT_RESTORE_TAB_NAVIGATE) {
                TryNavigateRestoringTab(hWnd);
                return 0;
            }
            break;

        case WM_CLOSE:
            RecordTabClosed(hWnd);
            break;

        case WM_DESTROY:
            KillTimer(hWnd, IDT_RESTORE_TAB_NAVIGATE);
            RecordTabClosed(hWnd);
            break;

        case WM_NCDESTROY: {
            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            std::lock_guard<std::mutex> lock(g_mutex);
            g_activeTabs.erase(hWnd);
            return res;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CabinetSubclassProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR dwRefData) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_APP_SHOW_HISTORY_UI:
            ShowOrCycleHistoryDialog(hWnd);
            return 0;

        case WM_APP_TRIGGER_REOPEN:
            RestoreLastClosedTab(hWnd);
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                HWND hActiveTab = FindActiveShellTabWindow(hWnd);
                if (hActiveTab) {
                    UpdateTabLocationSnapshot(hActiveTab);
                }
            }
            break;

        case WM_CLOSE: {
            HWND hChild = nullptr;
            while ((hChild = FindWindowExW(hWnd, hChild, L"ShellTabWindowClass", nullptr)) !=
                   nullptr) {
                UpdateTabLocationSnapshot(hChild);
            }
            break;
        }

        case WM_NCDESTROY: {
            LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            std::lock_guard<std::mutex> lock(g_mutex);
            g_cabinetWindows.erase(hWnd);
            auto it = g_pendingRestores.find(hWnd);
            if (it != g_pendingRestores.end()) {
                for (auto& item : it->second) {
                    if (item.pidl) {
                        ILFree(item.pidl);
                    }
                }
                g_pendingRestores.erase(it);
            }
            return res;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Kiểm tra và chặn phím tắt Shift+Tab và Ctrl+Shift+T (Không dùng hàm chặn luồng)
// ---------------------------------------------------------------------------
enum class ShortcutAction {
    None = 0,
    OpenHistoryDialog,
    ReopenClosedTab
};

ShortcutAction MatchShortcutKey(UINT vk) {
    HotkeyConfig reopenHk;
    HotkeyConfig histHk;
    bool enableShiftTab = true;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        reopenHk = g_settings.reopenHotkey;
        histHk = g_settings.historyHotkey;
        enableShiftTab = g_settings.enableShiftTabHistory;
    }

    bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    bool altDown = (GetKeyState(VK_MENU) & 0x8000) != 0;
    bool winDown = ((GetKeyState(VK_LWIN) & 0x8000) != 0) ||
                   ((GetKeyState(VK_RWIN) & 0x8000) != 0);

    if (enableShiftTab && vk == histHk.vk &&
        ctrlDown == histHk.ctrl && shiftDown == histHk.shift &&
        altDown == histHk.alt && winDown == histHk.win) {
        return ShortcutAction::OpenHistoryDialog;
    }

    if (vk == reopenHk.vk &&
        ctrlDown == reopenHk.ctrl && shiftDown == reopenHk.shift &&
        altDown == reopenHk.alt && winDown == reopenHk.win) {
        return ShortcutAction::ReopenClosedTab;
    }

    return ShortcutAction::None;
}

bool TryHandleExplorerShortcut(HWND hTargetWnd, UINT vk, bool isRepeat) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return false;
    }

    if (IsHistoryDialogOrChild(hTargetWnd) || IsHistoryDialogOrChild(GetFocus())) {
        return false;
    }

    ShortcutAction action = MatchShortcutKey(vk);
    if (action == ShortcutAction::None) {
        return false;
    }

    if (isRepeat && action == ShortcutAction::ReopenClosedTab) {
        return true;
    }

    HWND hCabinet = FindParentCabinetWindow(hTargetWnd);
    if (!hCabinet) {
        HWND hForeground = GetForegroundWindow();
        if (IsWindowOfClass(hForeground, L"CabinetWClass")) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hForeground, &pid);
            if (pid == GetCurrentProcessId()) {
                hCabinet = hForeground;
            }
        }
    }

    if (!hCabinet) {
        return false;
    }

    ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (now - g_lastShortcutTriggerTick < 90) {
            return true;
        }
        g_lastShortcutTriggerTick = now;
    }

    if (action == ShortcutAction::OpenHistoryDialog) {
        PostMessageW(hCabinet, WM_APP_SHOW_HISTORY_UI, 0, 0);
    } else if (action == ShortcutAction::ReopenClosedTab) {
        PostMessageW(hCabinet, WM_APP_TRIGGER_REOPEN, 0, 0);
    }
    return true;
}

LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && !g_uninitializing.load(std::memory_order_relaxed)) {
        bool isKeyDown = (lParam & (1ULL << 31)) == 0;
        bool wasDownBefore = (lParam & (1ULL << 30)) != 0;
        if (isKeyDown) {
            HWND hFocus = GetFocus();
            if (!IsHistoryDialogOrChild(hFocus)) {
                HWND hActive = hFocus ? hFocus : GetActiveWindow();
                if (TryHandleExplorerShortcut(hActive, static_cast<UINT>(wParam), wasDownBefore)) {
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void EnsureThreadKeyboardHook(HWND hWnd) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_threadKeyboardHooks.find(threadId) == g_threadKeyboardHooks.end()) {
        HHOOK hHook = SetWindowsHookExW(WH_KEYBOARD, KeyboardHookProc, nullptr, threadId);
        if (hHook) {
            g_threadKeyboardHooks[threadId] = hHook;
        }
    }
}

// ---------------------------------------------------------------------------
// Gắn theo dõi cửa sổ CabinetWClass và ShellTabWindowClass
// ---------------------------------------------------------------------------
void AttachShellTabWindow(HWND hTab) {
    if (!hTab || !IsWindow(hTab) || g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    HWND hCabinet = FindParentCabinetWindow(hTab);
    bool shouldStartRestoreTimer = false;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_activeTabs.find(hTab) != g_activeTabs.end()) {
            return;
        }

        ActiveTabState& state = g_activeTabs[hTab];
        state.hCabinet = hCabinet;

        if (hCabinet) {
            auto itQueue = g_pendingRestores.find(hCabinet);
            if (itQueue != g_pendingRestores.end()) {
                ULONGLONG now = GetTickCount64();
                while (!itQueue->second.empty()) {
                    PendingRestoreItem item = itQueue->second.front();
                    itQueue->second.pop_front();
                    if (now - item.timestamp <= 5000 && item.pidl) {
                        state.isRestoring = true;
                        state.restoreTargetPidl = item.pidl;
                        shouldStartRestoreTimer = true;
                        break;
                    } else if (item.pidl) {
                        ILFree(item.pidl);
                    }
                }
            }
        }
    }

    WindhawkUtils::SetWindowSubclassFromAnyThread(hTab, ShellTabSubclassProc, 0);
    EnsureThreadKeyboardHook(hTab);

    if (shouldStartRestoreTimer) {
        SetTimer(hTab, IDT_RESTORE_TAB_NAVIGATE, RESTORE_TIMER_INTERVAL_MS, nullptr);
    } else {
        UpdateTabLocationSnapshot(hTab);
    }
}

void AttachCabinetWindow(HWND hCabinet) {
    if (!hCabinet || !IsWindow(hCabinet) || g_uninitializing.load(std::memory_order_relaxed)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_cabinetWindows.insert(hCabinet).second) {
            return;
        }
    }

    WindhawkUtils::SetWindowSubclassFromAnyThread(hCabinet, CabinetSubclassProc, 0);
    EnsureThreadKeyboardHook(hCabinet);

    HWND hChild = nullptr;
    while ((hChild = FindWindowExW(hCabinet, hChild, L"ShellTabWindowClass", nullptr)) != nullptr) {
        AttachShellTabWindow(hChild);
    }
}

// ---------------------------------------------------------------------------
// Các hàm Hook Win32 API KHÔNG CHẶN LUỒNG (Tránh 100% lỗi treo Uninitializing...)
// ---------------------------------------------------------------------------
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

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
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                         X, Y, nWidth, nHeight, hWndParent, hMenu,
                                         hInstance, lpParam);
    if (!hWnd || g_uninitializing.load(std::memory_order_relaxed)) {
        return hWnd;
    }

    BOOL isTextClass = ((reinterpret_cast<ULONG_PTR>(lpClassName) &
                         ~static_cast<ULONG_PTR>(0xFFFF)) != 0);
    if (isTextClass) {
        if (_wcsicmp(lpClassName, L"CabinetWClass") == 0) {
            AttachCabinetWindow(hWnd);
        } else if (_wcsicmp(lpClassName, L"ShellTabWindowClass") == 0) {
            AttachShellTabWindow(hWnd);
        } else if (_wcsicmp(lpClassName, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0 ||
                   _wcsicmp(lpClassName, L"InputSiteWindowClass") == 0 ||
                   _wcsicmp(lpClassName, L"DirectUIHWND") == 0) {
            if (FindParentCabinetWindow(hWnd)) {
                EnsureThreadKeyboardHook(hWnd);
            }
        }
    }

    return hWnd;
}

using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
TranslateAcceleratorW_t TranslateAcceleratorW_Original = nullptr;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (!g_uninitializing.load(std::memory_order_relaxed) && lpMsg &&
        (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (!IsHistoryDialogOrChild(lpMsg->hwnd)) {
            bool wasDownBefore = (lpMsg->lParam & (1ULL << 30)) != 0;
            if (TryHandleExplorerShortcut(lpMsg->hwnd ? lpMsg->hwnd : hWnd,
                                          static_cast<UINT>(lpMsg->wParam),
                                          wasDownBefore)) {
                lpMsg->message = WM_NULL;
                return 1;
            }
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, lpMsg);
}

using IsDialogMessageW_t = decltype(&IsDialogMessageW);
IsDialogMessageW_t IsDialogMessageW_Original = nullptr;

BOOL WINAPI IsDialogMessageW_Hook(HWND hDlg, LPMSG lpMsg) {
    if (!g_uninitializing.load(std::memory_order_relaxed) && lpMsg &&
        (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (!IsHistoryDialogOrChild(lpMsg->hwnd)) {
            bool wasDownBefore = (lpMsg->lParam & (1ULL << 30)) != 0;
            if (TryHandleExplorerShortcut(lpMsg->hwnd ? lpMsg->hwnd : hDlg,
                                          static_cast<UINT>(lpMsg->wParam),
                                          wasDownBefore)) {
                lpMsg->message = WM_NULL;
                return TRUE;
            }
        }
    }
    return IsDialogMessageW_Original(hDlg, lpMsg);
}

using TranslateMessage_t = decltype(&TranslateMessage);
TranslateMessage_t TranslateMessage_Original = nullptr;

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_uninitializing.load(std::memory_order_relaxed) && lpMsg &&
        (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (!IsHistoryDialogOrChild(lpMsg->hwnd)) {
            bool wasDownBefore = (lpMsg->lParam & (1ULL << 30)) != 0;
            if (TryHandleExplorerShortcut(lpMsg->hwnd,
                                          static_cast<UINT>(lpMsg->wParam),
                                          wasDownBefore)) {
                MSG* mutableMsg = const_cast<MSG*>(lpMsg);
                mutableMsg->message = WM_NULL;
                mutableMsg->wParam = 0;
                mutableMsg->lParam = 0;
                return TRUE;
            }
        }
    }
    return TranslateMessage_Original(lpMsg);
}

// ---------------------------------------------------------------------------
// Vòng đời của Mod Windhawk
// ---------------------------------------------------------------------------
void ScanExistingExplorerWindows() {
    EnumWindows(
        [](HWND hWnd, LPARAM) WINAPI -> BOOL {
            if (g_uninitializing.load(std::memory_order_relaxed)) {
                return FALSE;
            }
            DWORD pid = 0;
            if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId()) {
                return TRUE;
            }
            if (IsWindowOfClass(hWnd, L"CabinetWClass")) {
                AttachCabinetWindow(hWnd);
            }
            return TRUE;
        },
        0);
}

BOOL Wh_ModInit() {
    g_uninitializing.store(false, std::memory_order_seq_cst);
    Wh_Log(L"Initializing explorer-reopen-closed-tabs v2.0.202 (Non-blocking hooks + 26 languages)");

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    LoadSettings();

    Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                       reinterpret_cast<void*>(CreateWindowExW_Hook),
                       reinterpret_cast<void**>(&CreateWindowExW_Original));

    Wh_SetFunctionHook(reinterpret_cast<void*>(TranslateAcceleratorW),
                       reinterpret_cast<void*>(TranslateAcceleratorW_Hook),
                       reinterpret_cast<void**>(&TranslateAcceleratorW_Original));

    Wh_SetFunctionHook(reinterpret_cast<void*>(IsDialogMessageW),
                       reinterpret_cast<void*>(IsDialogMessageW_Hook),
                       reinterpret_cast<void**>(&IsDialogMessageW_Original));

    Wh_SetFunctionHook(reinterpret_cast<void*>(TranslateMessage),
                       reinterpret_cast<void*>(TranslateMessage_Hook),
                       reinterpret_cast<void**>(&TranslateMessage_Original));

    return TRUE;
}

void Wh_ModAfterInit() {
    ScanExistingExplorerWindows();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    HWND hDlg = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        hDlg = g_hHistoryDialog;
    }
    if (hDlg && IsWindow(hDlg)) {
        InvalidateRect(hDlg, nullptr, FALSE);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing explorer-reopen-closed-tabs v2.0.202");
    g_uninitializing.store(true, std::memory_order_seq_cst);

    HWND hDlg = nullptr;
    std::vector<HWND> tabsToDetach;
    std::vector<HWND> cabinetsToDetach;
    std::vector<std::pair<DWORD, HHOOK>> hooksToRemove;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        hDlg = g_hHistoryDialog;
        g_hHistoryDialog = nullptr;

        for (const auto& [hTab, _] : g_activeTabs) {
            tabsToDetach.push_back(hTab);
        }
        for (HWND hCab : g_cabinetWindows) {
            cabinetsToDetach.push_back(hCab);
        }
        for (const auto& [tid, hHook] : g_threadKeyboardHooks) {
            if (hHook) {
                hooksToRemove.emplace_back(tid, hHook);
            }
        }
        g_threadKeyboardHooks.clear();
    }

    // Đóng hộp thoại không chặn luồng và đánh thức luồng chứa hộp thoại
    if (hDlg && IsWindow(hDlg)) {
        DWORD dlgTid = GetWindowThreadProcessId(hDlg, nullptr);
        if (dlgTid == GetCurrentThreadId()) {
            DestroyWindow(hDlg);
        } else {
            PostMessageW(hDlg, WM_CLOSE, 0, 0);
            if (dlgTid) {
                PostThreadMessageW(dlgTid, WM_NULL, 0, 0);
            }
            for (int i = 0; i < 15 && IsWindow(hDlg); ++i) {
                Sleep(10);
            }
        }
    }

    // Gỡ toàn bộ WH_KEYBOARD hook và gửi WM_NULL đánh thức từng luồng để Windows giải phóng DLL ngay lập tức
    for (const auto& [tid, hHook] : hooksToRemove) {
        UnhookWindowsHookEx(hHook);
        if (tid) {
            PostThreadMessageW(tid, WM_NULL, 0, 0);
        }
    }

    for (HWND hTab : tabsToDetach) {
        if (IsWindow(hTab)) {
            KillTimer(hTab, IDT_RESTORE_TAB_NAVIGATE);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTab, ShellTabSubclassProc);
        }
    }

    for (HWND hCab : cabinetsToDetach) {
        if (IsWindow(hCab)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hCab, CabinetSubclassProc);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_activeTabs.clear();
        g_cabinetWindows.clear();
        g_closedTabs.clear();
        g_openHistory.clear();
        for (auto& [_, queue] : g_pendingRestores) {
            for (auto& item : queue) {
                if (item.pidl) {
                    ILFree(item.pidl);
                }
            }
        }
        g_pendingRestores.clear();

        if (g_historyDialogAtom != 0) {
            UnregisterClassW(HISTORY_DIALOG_CLASS_NAME, GetModuleHandleW(nullptr));
            g_historyDialogAtom = 0;
        }
    }

    if (g_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
