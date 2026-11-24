// ==WindhawkMod==
// @id              network-speedtest-native
// @name            Network Menu Speed Test
// @description     Replaces the web speed test in the Windows 11 network menu with a native flyout: ping, jitter, download and upload measured against Cloudflare, live gauge, results chart with copy, keyboard accessible. No browser, no separate app.
// @version         1.1
// @author          Alessandro-Vecchio
// @github          https://github.com/Alessandro-Vecchio
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @include         SystemSettings.exe
// @include         RuntimeBroker.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lshlwapi -lshell32 -lole32 -ladvapi32 -luser32 -ld2d1 -ld3d11 -ldcomp -ldwrite -ldwmapi -lwinhttp -liphlpapi -lshcore -luuid -loleacc -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Network Menu Speed Test

Windows 11's network menu ("Run internet speed test") normally opens

    https://go.microsoft.com/fwlink/?linkid=2324916

in the default browser, which redirects to a Bing search page. This mod
intercepts that URL in the shell processes and replaces it with a native
flyout: the window, the gauge, the result cards and the measurement all live
inside the mod, with no browser, external app or protocol handler. The test
uses the public Cloudflare endpoints on `speed.cloudflare.com`, the same ones
the website uses.

### Screenshots

| Light theme | Dark theme |
| --- | --- |
| ![Light theme](https://raw.githubusercontent.com/Alessandro-Vecchio/windhawk-mods/assets/light.png) | ![Dark theme](https://raw.githubusercontent.com/Alessandro-Vecchio/windhawk-mods/assets/dark.png) |

| Acrylic | Results card |
| --- | --- |
| ![Acrylic](https://raw.githubusercontent.com/Alessandro-Vecchio/windhawk-mods/assets/acrylic.png) | ![Results card](https://raw.githubusercontent.com/Alessandro-Vecchio/windhawk-mods/assets/results.png) |

### Data usage

The test moves real data. The **Data limit per direction** setting caps each
phase, download and upload separately; with the default limit a whole test
stays under ~1 GB. Lines up to ~340 Mbps complete the full 12 s window; faster
lines end the phase when the cap is reached, and the result divides the bytes
by the time data was actually moving. Lower limits shorten the measurement on
fast lines, while removing the cap can move several GB per test. The results
card shows the bytes used ("Data used: 762 MB"), and the same figure is
included in the text copied with Copy.

### Settings

- **Extra URL patterns**: additional URL substrings separated by `|`. Leave
  it empty when the network menu opens the flyout; add patterns only if the
  menu starts using a different URL.
- **Fall back to the web page**: run the original URL when the flyout cannot
  start.
- **Also intercept browser process launches**: catch launches made directly
  through `CreateProcessW`. The launch returns `ERROR_CANCELLED` because no
  browser process is created; turn this off to open the browser normally when
  the shell skips the shell-execute path.
- **Start the test automatically**: `auto` (default) starts the test when the
  flyout opens, but waits for a click on metered connections; `always` starts
  unconditionally; `never` always waits for a click.
- **Data limit per direction**: 128 MB, 256 MB, 512 MB (default), 1 GB or
  Unlimited.

### Notes

- The flyout opens above the taskbar on the monitor under the mouse and
  closes on an outside click, but never while a measurement is running.
- Keyboard accessible (Tab / Enter / Esc with a WinUI-style focus ring) and
  exposed to screen readers through MSAA; it honours the "Animation effects"
  setting, so transitions are instant when animations are off.
- The flyout texts follow the Windows display language: English, Italian,
  Spanish, French, German, Portuguese, Russian, Japanese, Korean, Chinese
  (Simplified and Traditional), Dutch, Polish and Turkish; other languages
  fall back to English.
- [perform-speedtest-redirect](https://windhawk.net/mods/perform-speedtest-redirect)
  intercepts the same URL launches; enable only one of the two mods.
- Diagnostics go to the Windhawk mod log; enable logging for this mod from
  its Details page when needed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraPatterns: ""
  $name: Extra URL patterns
  $description: Additional substrings separated by | (example speedtest.net|ookla.com). Leave empty if the speed test opens correctly; add patterns only if the network menu starts using a different URL.
- fallbackToOriginal: true
  $name: Fall back to the web page
  $description: If the native flyout cannot start, open the original web page instead of doing nothing.
- interceptProcessLaunch: true
  $name: Also intercept browser process launches
  $description: Catch direct browser launches from the network menu. The launch returns ERROR_CANCELLED because no browser process is created.
- autoStart: auto
  $name: Start the test automatically
  $description: Starts the test when the flyout opens. Auto waits for a click on metered connections. Never requires a click every time.
  $options:
  - auto: Auto (waits on metered connections)
  - always: Always
  - never: Never (click to start)
- dataBudget: '512'
  $name: Data limit per direction
  $description: Maximum data used by each phase of the test, download and upload separately. Set to Unlimited for no cap (a fast line can then use several GB per test).
  $options:
  - '128': 128 MB
  - '256': 256 MB
  - '512': 512 MB (default)
  - '1000': 1 GB
  - unlimited: Unlimited
*/
// ==/WindhawkModSettings==

#include <windows.h>
// WIN32_LEAN_AND_MEAN (set in @compilerOptions) makes windows.h skip these:
// - shellapi.h: the hooks use SHELLEXECUTEINFOW/ShellExecuteW.
// - winsock2.h/ws2tcpip.h: iptypes.h (IP_ADAPTER_ADDRESSES) is built on the
//   Winsock address types, and the adapter matching uses SOCKADDR_IN6. The mod
//   itself never calls Winsock functions.
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shellscalingapi.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <oleacc.h>
#include <winhttp.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <netlistmgr.h>
#include <cmath>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <new>
#include <atomic>

#include <windhawk_utils.h>

using ShellExecuteExW_t = decltype(&ShellExecuteExW);
using ShellExecuteW_t = decltype(&ShellExecuteW);
using CreateProcessW_t = decltype(&CreateProcessW);

static ShellExecuteExW_t ShellExecuteExW_Original;
static ShellExecuteW_t ShellExecuteW_Original;
static CreateProcessW_t CreateProcessW_Original;

static std::vector<std::wstring> g_patterns;
// Settings are reloaded by Windhawk on its own thread while the shell hooks
// read the patterns from arbitrary threads. Swapping the vector under a lock
// prevents a hook from walking a vector that is being rebuilt.
static SRWLOCK g_patternsLock = SRWLOCK_INIT;
static std::atomic<bool> g_fallbackToOriginal = true;
static std::atomic<bool> g_interceptProcessLaunch = true;
// Start mode for the flyout: 0 = auto (start unless the connection is
// metered), 1 = always start, 2 = never start (wait for the Start button).
static std::atomic<int> g_autoStart = 0;
// Default and lowest accepted per-direction data budget, in bytes.
static constexpr LONGLONG kDefaultTransferBudgetBytes = 512'000'000;
static constexpr LONGLONG kMinTransferBudgetBytes = 128'000'000;
// Maximum data per direction in bytes, from settings; 0 means unlimited.
static std::atomic<LONGLONG> g_transferBudgetBytes =
    kDefaultTransferBudgetBytes;

// ---------------------------------------------------------------------------
// UI strings in the main Windows display languages, chosen from the user's
// language with a fallback to English.
// ---------------------------------------------------------------------------

struct UiStrings {
    PCWSTR ready;
    PCWSTR starting;
    PCWSTR connecting;
    PCWSTR completed;
    PCWSTR canceled;
    PCWSTR stopping;
    PCWSTR error;
    PCWSTR ping;
    PCWSTR download;
    PCWSTR upload;
    PCWSTR jitter;
    PCWSTR dataUsed;
    PCWSTR copy;
    PCWSTR copied;
    PCWSTR start;
    PCWSTR stop;
    PCWSTR repeat;
    PCWSTR resultsTitle;
    PCWSTR failedTitle;
    PCWSTR failedHint;
    PCWSTR retry;
    PCWSTR closeName;
    PCWSTR server;
    PCWSTR errConnect;
    PCWSTR errPing;
    PCWSTR errCanceled;
    PCWSTR errUpload;
    PCWSTR errDownload;
    PCWSTR errUnexpected;
};

static const UiStrings kStringsEn = {
    L"Ready", L"Starting", L"Connecting", L"Completed", L"Canceled",
    L"Stopping\x2026", L"Error", L"Ping", L"Download", L"Upload", L"Jitter",
    L"Data used", L"Copy", L"Copied", L"Start", L"Stop", L"Repeat",
    L"Test results", L"Test failed",
    L"Try again in a moment.", L"Retry", L"Close", L"Server",
    L"Could not connect to the server.",
    L"Could not measure the ping: requests failed.",
    L"Test canceled.",
    L"Upload: the server received no data. Try again in a few minutes.",
    L"Download: the server returned no data. Try again in a few minutes.",
    L"Unexpected error during the test.",
};

// Italian
static const UiStrings kStringsIt = {
    L"Pronto", L"Avvio", L"Connessione", L"Completato", L"Annullato",
    L"Arresto\x2026", L"Errore", L"Ping", L"Download", L"Upload", L"Jitter",
    L"Dati usati", L"Copia", L"Copiato", L"Avvia", L"Interrompi", L"Ripeti",
    L"Risultati del test", L"Test non riuscito",
    L"Riprova tra qualche istante.", L"Riprova", L"Chiudi", L"Server",
    L"Impossibile collegarsi al server.",
    L"Impossibile misurare il ping: richieste fallite.",
    L"Test annullato.",
    L"Upload: il server non ha ricevuto dati. Riprova tra qualche minuto.",
    L"Download: il server non ha restituito dati. Riprova tra qualche minuto.",
    L"Errore imprevisto durante il test.",
};

// Spanish
static const UiStrings kStringsEs = {
    L"Listo", L"Iniciando", L"Conectando", L"Completado", L"Cancelado",
    L"Deteniendo\x2026", L"Error", L"Ping", L"Descarga", L"Carga", L"Jitter",
    L"Datos usados", L"Copiar", L"Copiado", L"Iniciar", L"Detener", L"Repetir",
    L"Resultados de la prueba", L"Error en la prueba",
    L"Inténtalo de nuevo en un momento.", L"Reintentar", L"Cerrar", L"Servidor",
    L"No se pudo conectar al servidor.",
    L"No se pudo medir el ping: error en las solicitudes.",
    L"Prueba cancelada.",
    L"Carga: el servidor no recibió datos. Inténtalo de nuevo en unos minutos.",
    L"Descarga: el servidor no devolvió datos. Inténtalo de nuevo en unos minutos.",
    L"Error inesperado durante la prueba.",
};

// French
static const UiStrings kStringsFr = {
    L"Prêt", L"Démarrage", L"Connexion", L"Terminé", L"Annulé",
    L"Arrêt\x2026", L"Erreur", L"Ping", L"Téléchargement", L"Chargement",
    L"Jitter", L"Données utilisées", L"Copier", L"Copié", L"Démarrer",
    L"Arrêter", L"Répéter", L"Résultats du test", L"Échec du test",
    L"Réessayez dans un instant.", L"Réessayer", L"Fermer", L"Serveur",
    L"Impossible de se connecter au serveur.",
    L"Impossible de mesurer le ping\x202F: échec des requêtes.",
    L"Test annulé.",
    L"Chargement\x202F: le serveur n'a reçu aucune donnée. Réessayez dans "
    L"quelques minutes.",
    L"Téléchargement\x202F: le serveur n'a renvoyé aucune donnée. Réessayez "
    L"dans quelques minutes.",
    L"Erreur inattendue pendant le test.",
};

// German
static const UiStrings kStringsDe = {
    L"Bereit", L"Wird gestartet", L"Verbindung wird hergestellt", L"Fertig",
    L"Abgebrochen", L"Wird beendet\x2026", L"Fehler", L"Ping",
    L"Herunterladen", L"Hochladen", L"Jitter", L"Verwendete Daten",
    L"Kopieren", L"Kopiert", L"Starten", L"Beenden", L"Wiederholen",
    L"Testergebnisse", L"Test fehlgeschlagen",
    L"Versuchen Sie es gleich noch einmal.", L"Erneut versuchen", L"Schließen",
    L"Server", L"Verbindung zum Server nicht möglich.",
    L"Ping konnte nicht gemessen werden: Anforderungen fehlgeschlagen.",
    L"Test abgebrochen.",
    L"Hochladen: Der Server hat keine Daten empfangen. Versuchen Sie es in "
    L"einigen Minuten erneut.",
    L"Herunterladen: Der Server hat keine Daten zurückgegeben. Versuchen Sie "
    L"es in einigen Minuten erneut.",
    L"Unerwarteter Fehler beim Test.",
};

// Portuguese (Brazil)
static const UiStrings kStringsPt = {
    L"Pronto", L"Iniciando", L"Conectando", L"Concluído", L"Cancelado",
    L"Interrompendo\x2026", L"Erro", L"Ping", L"Download", L"Upload", L"Jitter",
    L"Dados usados", L"Copiar", L"Copiado", L"Iniciar", L"Parar", L"Repetir",
    L"Resultados do teste", L"Falha no teste",
    L"Tente novamente em instantes.", L"Tentar novamente", L"Fechar",
    L"Servidor", L"Não foi possível conectar ao servidor.",
    L"Não foi possível medir o ping: falha nas solicitações.",
    L"Teste cancelado.",
    L"Upload: o servidor não recebeu dados. Tente novamente em alguns minutos.",
    L"Download: o servidor não retornou dados. Tente novamente em alguns "
    L"minutos.",
    L"Erro inesperado durante o teste.",
};

// Russian
static const UiStrings kStringsRu = {
    L"Готово", L"Запуск", L"Подключение", L"Завершено", L"Отменено",
    L"Остановка\x2026", L"Ошибка", L"Пинг", L"Скачивание", L"Отправка",
    L"Джиттер", L"Использовано данных", L"Копировать", L"Скопировано",
    L"Запустить", L"Остановить", L"Повторить", L"Результаты теста",
    L"Тест не выполнен",
    L"Повторите попытку через некоторое время.", L"Повторить попытку",
    L"Закрыть", L"Сервер",
    L"Не удалось подключиться к серверу.",
    L"Не удалось измерить пинг: запросы завершились ошибкой.",
    L"Тест отменен.",
    L"Отправка: сервер не получил данные. Повторите попытку через несколько "
    L"минут.",
    L"Скачивание: сервер не вернул данные. Повторите попытку через несколько "
    L"минут.",
    L"Непредвиденная ошибка во время теста.",
};

// Japanese
static const UiStrings kStringsJa = {
    L"準備完了", L"開始中", L"接続中", L"完了", L"キャンセルされました",
    L"停止中\x2026", L"エラー", L"Ping", L"ダウンロード", L"アップロード",
    L"ジッター", L"使用データ", L"コピー", L"コピーしました", L"開始", L"停止",
    L"再実行", L"テスト結果", L"テストに失敗しました",
    L"しばらくしてからもう一度お試しください。", L"再試行", L"閉じる",
    L"サーバー", L"サーバーに接続できませんでした。",
    L"Ping を測定できませんでした: 要求が失敗しました。",
    L"テストがキャンセルされました。",
    L"アップロード: サーバーがデータを受信しませんでした。数分後にもう一度お試しください。",
    L"ダウンロード: サーバーからデータが返されませんでした。数分後にもう一度お試しください。",
    L"テスト中に予期しないエラーが発生しました。",
};

// Korean
static const UiStrings kStringsKo = {
    L"준비됨", L"시작 중", L"연결 중", L"완료됨", L"취소됨",
    L"중지 중\x2026", L"오류", L"핑", L"다운로드", L"업로드", L"지터",
    L"사용된 데이터", L"복사", L"복사됨", L"시작", L"중지", L"다시 실행",
    L"테스트 결과", L"테스트 실패",
    L"잠시 후 다시 시도하세요.", L"재시도", L"닫기", L"서버",
    L"서버에 연결할 수 없습니다.",
    L"핑을 측정할 수 없음: 요청이 실패했습니다.",
    L"테스트가 취소되었습니다.",
    L"업로드: 서버가 데이터를 받지 못했습니다. 몇 분 후 다시 시도하세요.",
    L"다운로드: 서버가 데이터를 반환하지 않았습니다. 몇 분 후 다시 시도하세요.",
    L"테스트 중 예기치 않은 오류가 발생했습니다.",
};

// Chinese (Simplified)
static const UiStrings kStringsZhHans = {
    L"就绪", L"正在启动", L"正在连接", L"已完成", L"已取消",
    L"正在停止\x2026", L"错误", L"Ping", L"下载", L"上传", L"抖动",
    L"已用数据", L"复制", L"已复制", L"开始", L"停止", L"再测一次",
    L"测试结果", L"测试失败",
    L"请稍后重试。", L"重试", L"关闭", L"服务器",
    L"无法连接到服务器。",
    L"无法测量 Ping：请求失败。",
    L"测试已取消。",
    L"上传：服务器未收到数据。请在几分钟后重试。",
    L"下载：服务器未返回数据。请在几分钟后重试。",
    L"测试期间发生意外错误。",
};

// Chinese (Traditional)
static const UiStrings kStringsZhHant = {
    L"就緒", L"正在啟動", L"正在連線", L"已完成", L"已取消",
    L"正在停止\x2026", L"錯誤", L"Ping", L"下載", L"上傳", L"抖動",
    L"已用資料", L"複製", L"已複製", L"開始", L"停止", L"再測一次",
    L"測試結果", L"測試失敗",
    L"請稍後重試。", L"重試", L"關閉", L"伺服器",
    L"無法連線到伺服器。",
    L"無法測量 Ping：要求失敗。",
    L"測試已取消。",
    L"上傳：伺服器未收到資料。請在幾分鐘後重試。",
    L"下載：伺服器未傳回資料。請在幾分鐘後重試。",
    L"測試期間發生意外錯誤。",
};

// Dutch
static const UiStrings kStringsNl = {
    L"Gereed", L"Bezig met starten", L"Bezig met verbinden", L"Voltooid",
    L"Geannuleerd", L"Bezig met stoppen\x2026", L"Fout", L"Ping",
    L"Downloaden", L"Uploaden", L"Jitter", L"Gebruikte gegevens", L"Kopiëren",
    L"Gekopieerd", L"Starten", L"Stoppen", L"Herhalen", L"Testresultaten",
    L"Test mislukt",
    L"Probeer het zo opnieuw.", L"Opnieuw proberen", L"Sluiten", L"Server",
    L"Kan geen verbinding maken met de server.",
    L"De ping kan niet worden gemeten: aanvragen zijn mislukt.",
    L"Test geannuleerd.",
    L"Uploaden: de server heeft geen gegevens ontvangen. Probeer het over "
    L"enkele minuten opnieuw.",
    L"Downloaden: de server heeft geen gegevens geretourneerd. Probeer het "
    L"over enkele minuten opnieuw.",
    L"Onverwachte fout tijdens de test.",
};

// Polish
static const UiStrings kStringsPl = {
    L"Gotowe", L"Uruchamianie", L"Łączenie", L"Ukończono", L"Anulowano",
    L"Zatrzymywanie\x2026", L"Błąd", L"Ping", L"Pobieranie", L"Wysyłanie",
    L"Jitter", L"Wykorzystane dane", L"Kopiuj", L"Skopiowano", L"Uruchom",
    L"Zatrzymaj", L"Powtórz", L"Wyniki testu", L"Test nie powiódł się",
    L"Spróbuj ponownie za chwilę.", L"Spróbuj ponownie", L"Zamknij", L"Serwer",
    L"Nie można połączyć się z serwerem.",
    L"Nie można zmierzyć pinga: żądania nie powiodły się.",
    L"Test anulowany.",
    L"Wysyłanie: serwer nie odebrał danych. Spróbuj ponownie za kilka minut.",
    L"Pobieranie: serwer nie zwrócił danych. Spróbuj ponownie za kilka minut.",
    L"Nieoczekiwany błąd podczas testu.",
};

// Turkish
static const UiStrings kStringsTr = {
    L"Hazır", L"Başlatılıyor", L"Bağlanıyor", L"Tamamlandı", L"İptal edildi",
    L"Durduruluyor\x2026", L"Hata", L"Ping", L"İndirme", L"Yükleme", L"Jitter",
    L"Kullanılan veri", L"Kopyala", L"Kopyalandı", L"Başlat", L"Durdur",
    L"Tekrarla", L"Test sonuçları", L"Test başarısız",
    L"Birazdan yeniden deneyin.", L"Yeniden dene", L"Kapat", L"Sunucu",
    L"Sunucuya bağlanılamadı.",
    L"Ping ölçülemedi: istekler başarısız oldu.",
    L"Test iptal edildi.",
    L"Yükleme: sunucu veri almadı. Birkaç dakika sonra yeniden deneyin.",
    L"İndirme: sunucu veri döndürmedi. Birkaç dakika sonra yeniden deneyin.",
    L"Test sırasında beklenmeyen bir hata oluştu.",
};

// The languages the flyout strings are available in, matched by the primary
// language identifier of the Windows display language. Languages outside the
// table fall back to English; adding one is a table row plus a strings block.
// Chinese is matched below because its variants split on the sublanguage.
struct UiStringsLang {
    LANGID lang;
    const UiStrings* strings;
};

static const UiStringsLang kStringsByLang[] = {
    {LANG_ITALIAN, &kStringsIt},    {LANG_SPANISH, &kStringsEs},
    {LANG_FRENCH, &kStringsFr},     {LANG_GERMAN, &kStringsDe},
    {LANG_PORTUGUESE, &kStringsPt}, {LANG_RUSSIAN, &kStringsRu},
    {LANG_JAPANESE, &kStringsJa},   {LANG_KOREAN, &kStringsKo},
    {LANG_DUTCH, &kStringsNl},      {LANG_POLISH, &kStringsPl},
    {LANG_TURKISH, &kStringsTr},
};

class UiStringsRef {
public:
    explicit UiStringsRef(const UiStrings* strings) : strings_(strings) {}

    const UiStrings* operator->() const {
        return strings_.load(std::memory_order_relaxed);
    }

    UiStringsRef& operator=(const UiStrings* strings) {
        strings_.store(strings, std::memory_order_relaxed);
        return *this;
    }

    bool operator==(const UiStrings* strings) const {
        return strings_.load(std::memory_order_relaxed) == strings;
    }

private:
    std::atomic<const UiStrings*> strings_;
};

static UiStringsRef g_str(&kStringsEn);

static void SelectLanguage() {
    LANGID lang = GetUserDefaultUILanguage();
    const UiStrings* strings = &kStringsEn;
    if (PRIMARYLANGID(lang) == LANG_CHINESE) {
        // Simplified covers the mainland and Singapore variants; the rest are
        // Traditional.
        strings = SUBLANGID(lang) == SUBLANG_CHINESE_SIMPLIFIED ||
                          SUBLANGID(lang) == SUBLANG_CHINESE_SINGAPORE
                      ? &kStringsZhHans
                      : &kStringsZhHant;
    } else {
        for (const auto& entry : kStringsByLang) {
            if (PRIMARYLANGID(lang) == entry.lang) {
                strings = entry.strings;
                break;
            }
        }
    }
    g_str = strings;
}

static std::vector<std::wstring> SplitPatterns(PCWSTR text) {
    std::vector<std::wstring> result;
    std::wstring current;
    for (PCWSTR p = text; p && *p; p++) {
        if (*p == L'|') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else if (*p != L' ' && *p != L'\t') {
            current.push_back(*p);
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

static void LoadSettings() {
    auto extra = WindhawkUtils::StringSetting::make(L"extraPatterns");
    std::vector<std::wstring> patterns = SplitPatterns(extra.get());
    AcquireSRWLockExclusive(&g_patternsLock);
    g_patterns.swap(patterns);
    ReleaseSRWLockExclusive(&g_patternsLock);

    g_fallbackToOriginal.store(Wh_GetIntSetting(L"fallbackToOriginal") != 0,
                               std::memory_order_relaxed);
    g_interceptProcessLaunch.store(
        Wh_GetIntSetting(L"interceptProcessLaunch") != 0,
        std::memory_order_relaxed);
    auto autoStart = WindhawkUtils::StringSetting::make(L"autoStart");
    int startMode = 0;
    if (wcscmp(autoStart.get(), L"always") == 0) {
        startMode = 1;
    } else if (wcscmp(autoStart.get(), L"never") == 0) {
        startMode = 2;
    }
    g_autoStart.store(startMode, std::memory_order_relaxed);
    auto dataBudget = WindhawkUtils::StringSetting::make(L"dataBudget");
    LONGLONG budgetBytes = kDefaultTransferBudgetBytes;
    if (_wcsicmp(dataBudget.get(), L"unlimited") == 0) {
        budgetBytes = 0;
    } else if (*dataBudget.get()) {
        long long mb = _wcstoi64(dataBudget.get(), nullptr, 10);
        if (mb > 0) {
            budgetBytes = mb * 1'000'000;
            if (budgetBytes < kMinTransferBudgetBytes) {
                budgetBytes = kMinTransferBudgetBytes;
            }
        }
    }
    g_transferBudgetBytes.store(budgetBytes, std::memory_order_relaxed);
    SelectLanguage();
}

// ---------------------------------------------------------------------------
// URL matching
// ---------------------------------------------------------------------------

static bool IsSpeedTestUrl(PCWSTR url) {
    if (!url || !*url) {
        return false;
    }

    // The links shipped by the Windows 11 network flyout/tray menu and the
    // page they redirect to. Keep this list narrow: the hook must never touch
    // ordinary web searches.
    static const PCWSTR kDefaultPatterns[] = {
        L"linkid=2325015",
        L"linkid=2324916",
        L"form=wspeed",
    };
    for (PCWSTR pattern : kDefaultPatterns) {
        if (StrStrIW(url, pattern)) {
            return true;
        }
    }

    // User-supplied extra patterns only apply to actual web URLs: the hooks
    // also see file names and command lines (a document, a launcher, ...), and
    // a file whose name contains a pattern must not be cancelled. The empty
    // check above already returned, so only the scheme matters here.
    if (!(url[0] == L'h' || url[0] == L'H')) {
        return false;
    }
    if (!StrStrIW(url, L"://")) {
        return false;
    }

    bool matched = false;
    AcquireSRWLockShared(&g_patternsLock);
    for (const auto& pattern : g_patterns) {
        if (StrStrIW(url, pattern.c_str())) {
            matched = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_patternsLock);
    return matched;
}

// ---------------------------------------------------------------------------
// Flyout activation across the shell processes
// ---------------------------------------------------------------------------

static constexpr PCWSTR kFlyoutClass = L"SpeedTestFlyout_Native";
static UINT g_showMessage = 0;

static BOOL CALLBACK FindFlyoutCallback(HWND hwnd, LPARAM param) {
    wchar_t className[64] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, kFlyoutClass) == 0) {
        *(HWND*)param = hwnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindFlyoutHost(DWORD* hostPid = nullptr) {
    // FindWindow is unreliable for this window class across shell processes,
    // so walk the top-level windows instead.
    HWND hwnd = nullptr;
    EnumWindows(FindFlyoutCallback, (LPARAM)&hwnd);
    if (hwnd && hostPid) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        *hostPid = pid;
    }
    return hwnd;
}

static bool ShowNativeFlyout() {
    // This runs inside a shell or process-launch hook, on whatever thread the
    // caller uses to launch the browser. Never wait for the UI here: a
    // blocking hook can re-enter the shell's XAML while it is starting a
    // process. If the host isn't there yet, fail fast and let the caller fall
    // back to the original URL.
    if (!g_showMessage) {
        Wh_Log(L"Registered flyout message is unavailable");
        return false;
    }

    DWORD hostPid = 0;
    HWND hwnd = FindFlyoutHost(&hostPid);
    if (!hwnd) {
        Wh_Log(L"Flyout host window not found");
        return false;
    }
    // The foreground grant belongs to the process the click happened in; the
    // flyout window lives in the host process (usually explorer.exe). Grant it
    // permission to take the foreground before posting the show request,
    // otherwise SetForegroundWindow inside the flyout can fail and the window
    // opens without focus (Esc/Tab dead).
    if (hostPid) {
        AllowSetForegroundWindow(hostPid);
    }
    if (!PostMessageW(hwnd, g_showMessage, 0, 0)) {
        Wh_Log(L"Flyout message post failed (error %u)", GetLastError());
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

static BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* sei) {
    if (sei && IsSpeedTestUrl(sei->lpFile)) {
        if (ShowNativeFlyout()) {
            if (sei->fMask & SEE_MASK_NOCLOSEPROCESS) {
                sei->hProcess = nullptr;
            }
            sei->hInstApp = (HINSTANCE)33;
            return TRUE;
        }

        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return FALSE;
        }
    }

    return ShellExecuteExW_Original(sei);
}

static HINSTANCE WINAPI ShellExecuteW_Hook(HWND hwnd, LPCWSTR lpOperation,
                                           LPCWSTR lpFile, LPCWSTR lpParameters,
                                           LPCWSTR lpDirectory, INT nShowCmd) {
    if (IsSpeedTestUrl(lpFile)) {
        if (ShowNativeFlyout()) {
            return (HINSTANCE)33;
        }
        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return (HINSTANCE)ERROR_FILE_NOT_FOUND;
        }
    }

    return ShellExecuteW_Original(hwnd, lpOperation, lpFile, lpParameters,
                                  lpDirectory, nShowCmd);
}

static BOOL WINAPI CreateProcessW_Hook(LPCWSTR lpApplicationName,
                                       LPWSTR lpCommandLine,
                                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                       BOOL bInheritHandles, DWORD dwCreationFlags,
                                       LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
                                       LPSTARTUPINFOW lpStartupInfo,
                                       LPPROCESS_INFORMATION lpProcessInformation) {
    if (g_interceptProcessLaunch.load(std::memory_order_relaxed) &&
        (IsSpeedTestUrl(lpCommandLine) || IsSpeedTestUrl(lpApplicationName))) {
        if (ShowNativeFlyout()) {
            if (lpProcessInformation) {
                *lpProcessInformation = {};
            }
            SetLastError(ERROR_CANCELLED);
            return FALSE;
        }

        if (!g_fallbackToOriginal.load(std::memory_order_relaxed)) {
            SetLastError(ERROR_FILE_NOT_FOUND);
            return FALSE;
        }
    }

    return CreateProcessW_Original(lpApplicationName, lpCommandLine,
                                   lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment,
                                   lpCurrentDirectory, lpStartupInfo,
                                   lpProcessInformation);
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

static constexpr float kWidth = 400.0f;
// Gauge and results share a height; the error card is much shorter, so the
// window animates between the two layouts instead of leaving an empty gap.
static constexpr float kGaugeHeight = 456.0f;
static constexpr float kErrorHeight = 340.0f;
static constexpr wchar_t kTextFamily[] = L"Segoe UI Variable Text";
static constexpr wchar_t kDisplayFamily[] = L"Segoe UI Variable Display";
static constexpr wchar_t kIconFamily[] = L"Segoe Fluent Icons";

static ULONGLONG Now() {
    return GetTickCount64();
}

static float Clamp01(float v) {
    return v < 0 ? 0 : (v > 1 ? 1 : v);
}

static double Clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static std::wstring FormatNumber(double value) {
    wchar_t buffer[48];
    if (value >= 100) {
        swprintf(buffer, 48, L"%.0f", value);
    } else {
        swprintf(buffer, 48, L"%.1f", value);
    }
    wchar_t decimalSeparator = g_str == &kStringsEn ? L'.' : L',';
    for (wchar_t* p = buffer; *p; p++) {
        if (*p == L'.') {
            *p = decimalSeparator;
        }
    }
    return buffer;
}

static std::wstring FormatScale(double value) {
    wchar_t buffer[48];
    swprintf(buffer, 48, L"%.0f", value);
    return buffer;
}

static std::wstring FormatClock(const SYSTEMTIME& t) {
    wchar_t buffer[16];
    swprintf(buffer, 16, L"%02u:%02u", t.wHour, t.wMinute);
    return buffer;
}

static std::wstring FormatDate(const SYSTEMTIME& t) {
    wchar_t buffer[24];
    if (g_str == &kStringsEn) {
        swprintf(buffer, 24, L"%02u/%02u/%04u %02u:%02u", t.wMonth, t.wDay,
                 t.wYear, t.wHour, t.wMinute);
    } else {
        swprintf(buffer, 24, L"%02u/%02u/%04u %02u:%02u", t.wDay, t.wMonth,
                 t.wYear, t.wHour, t.wMinute);
    }
    return buffer;
}

static unsigned NextRandom() {
    static volatile LONG seed = (LONG)GetTickCount64();
    unsigned x = (unsigned)InterlockedIncrement(&seed);
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x ? x : 0x9E3779B9u;
}

static double Percentile(std::vector<double> values, double percentile) {
    if (values.empty()) {
        return 0;
    }
    std::sort(values.begin(), values.end());
    if (values.size() == 1) {
        return values[0];
    }
    double rank = percentile / 100.0 * (values.size() - 1);
    size_t low = (size_t)std::floor(rank);
    size_t high = (size_t)std::ceil(rank);
    if (low == high) {
        return values[low];
    }
    return values[low] + (values[high] - values[low]) * (rank - low);
}

static double ChartAxisMax(double peak) {
    double value = std::max(peak * 1.04, 10.0);
    double step;
    if (value > 2000) {
        step = 500;
    } else if (value > 1000) {
        step = 250;
    } else if (value > 500) {
        step = 100;
    } else if (value > 200) {
        step = 50;
    } else if (value > 100) {
        step = 25;
    } else {
        step = 10;
    }
    return std::ceil(value / step) * step;
}

// ---------------------------------------------------------------------------
// Palette
// ---------------------------------------------------------------------------

struct Palette {
    D2D1_COLOR_F background;
    D2D1_COLOR_F text;
    D2D1_COLOR_F secondary;
    D2D1_COLOR_F tertiary;
    D2D1_COLOR_F cardFill;
    D2D1_COLOR_F cardStroke;
    D2D1_COLOR_F controlFill;
    D2D1_COLOR_F track;
    D2D1_COLOR_F download;
    D2D1_COLOR_F upload;
    D2D1_COLOR_F jitter;
    D2D1_COLOR_F positive;
    D2D1_COLOR_F negative;
    D2D1_COLOR_F accent;
    D2D1_COLOR_F accentText;
    D2D1_COLOR_F label;
};

static D2D1_COLOR_F Col(UINT32 rgb, float alpha = 1.0f) {
    D2D1_COLOR_F c;
    c.r = ((rgb >> 16) & 0xFF) / 255.0f;
    c.g = ((rgb >> 8) & 0xFF) / 255.0f;
    c.b = (rgb & 0xFF) / 255.0f;
    c.a = alpha;
    return c;
}

static D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float alpha) {
    c.a *= alpha;
    return c;
}

static D2D1_COLOR_F Lighten(D2D1_COLOR_F c, float amount) {
    c.r += (1.0f - c.r) * amount;
    c.g += (1.0f - c.g) * amount;
    c.b += (1.0f - c.b) * amount;
    return c;
}

// Pressed controls get a darker fill in Fluent; Lighten() cannot do that.
static D2D1_COLOR_F Shade(D2D1_COLOR_F c, float amount) {
    c.r *= (1.0f - amount);
    c.g *= (1.0f - amount);
    c.b *= (1.0f - amount);
    return c;
}

// The shell flyouts follow the *system* theme, and the DWM backdrop material is
// tinted from the system palette as well. Reading AppsUseLightTheme here would
// mix a dark surface with a light material (or the other way round).
static bool ReadSystemLightTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

static bool ReadTransparencyEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}

static DWORD WindowsBuild() {
    static DWORD build = 0;
    if (!build) {
        using RtlGetVersion_t = LONG(WINAPI*)(OSVERSIONINFOW*);
        auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
        OSVERSIONINFOW info = {};
        info.dwOSVersionInfoSize = sizeof(info);
        build = (rtlGetVersion && rtlGetVersion(&info) == 0 && info.dwBuildNumber)
                    ? info.dwBuildNumber
                    : 1;
    }
    return build;
}

// DWMWA_SYSTEMBACKDROP_TYPE only exists from build 22621, and even there DWM
// draws no material when composition is off, on remote sessions or in high
// contrast. Painting a translucent scrim in those cases would leave a
// see-through window with no blur behind it, which looks broken; the caller
// keeps the solid surface instead.
static bool MaterialSupported() {
    if (WindowsBuild() < 22621) {
        return false;
    }
    BOOL composition = FALSE;
    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition) {
        return false;
    }
    if (GetSystemMetrics(SM_REMOTESESSION)) {
        return false;
    }
    HIGHCONTRASTW contrast = {sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        return false;
    }
    return true;
}

static Palette MakePalette(bool light) {
    Palette p = {};
    if (light) {
        p.background = Col(0xF3F3F3);
        p.text = Col(0x1A1A1A);
        p.secondary = Col(0x5D5D5D);
        p.tertiary = Col(0x767676);
        p.cardFill = Col(0xFFFFFF, 0.70f);
        p.cardStroke = Col(0x000000, 0.06f);
        p.controlFill = Col(0x000000, 0.04f);
        p.track = Col(0xE4E4E4);
        p.download = Col(0x0B6BCB);
        p.upload = Col(0x6B3FA0);
        p.jitter = Col(0x0F6E66);
        p.positive = Col(0x0F7B0F);
        p.negative = Col(0xC42B1C);
        p.accent = Col(0x005FB8);
        p.accentText = Col(0xFFFFFF);
        p.label = Col(0x6B6B6B);
    } else {
        p.background = Col(0x2C2C2C);
        p.text = Col(0xFFFFFF);
        p.secondary = Col(0xC5C5C5);
        p.tertiary = Col(0x8B8B8B);
        p.cardFill = Col(0xFFFFFF, 0.053f);
        p.cardStroke = Col(0xFFFFFF, 0.086f);
        p.controlFill = Col(0xFFFFFF, 0.06f);
        p.track = Col(0x3A3A3A);
        p.download = Col(0x6CB4FF);
        p.upload = Col(0xC9A0FF);
        p.jitter = Col(0x5ED8CC);
        p.positive = Col(0x6CCB5F);
        p.negative = Col(0xFF99A4);
        p.accent = Col(0x4CC2FF);
        p.accentText = Col(0x000000);
        p.label = Col(0xA0A0A0);
    }
    return p;
}

// ---------------------------------------------------------------------------
// Network name for the footline
// ---------------------------------------------------------------------------

struct NetworkInfo {
    std::wstring name;
    const wchar_t* glyph = L"\xE774";
};

static PIP_ADAPTER_ADDRESSES GetAdapterAddressesList(
    std::vector<BYTE>* buffer) {
    ULONG size = (ULONG)buffer->size();
    ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST |
                  GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    for (int attempt = 0; attempt < 3; attempt++) {
        buffer->resize(size);
        ULONG result = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr,
                                            (PIP_ADAPTER_ADDRESSES)buffer->data(),
                                            &size);
        if (result == ERROR_SUCCESS) {
            return (PIP_ADAPTER_ADDRESSES)buffer->data();
        }
        if (result != ERROR_BUFFER_OVERFLOW) {
            return nullptr;
        }
    }
    return nullptr;
}

// Profile name for the footline: on Wi-Fi this is the SSID, on Ethernet the
// user-visible connection name - what the Windows network flyout shows. The
// Network List Manager knows it; the match to the adapter is by interface
// GUID. An empty result means the caller falls back to the adapter
// description.
static std::wstring NetworkProfileName(PIP_ADAPTER_ADDRESSES adapter) {
    INetworkListManager* manager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&manager));
    if (FAILED(hr)) {
        return L"";
    }

    std::wstring name;
    // IP_ADAPTER_ADDRESSES::AdapterName is the interface GUID as an ANSI
    // string, e.g. "{B49A9A0D-...}".
    wchar_t adapterName[64] = {};
    GUID adapterGuid = {};
    if (adapter->AdapterName &&
        MultiByteToWideChar(CP_ACP, 0, adapter->AdapterName, -1, adapterName,
                            64) > 0 &&
        SUCCEEDED(CLSIDFromString(adapterName, &adapterGuid))) {
        IEnumNetworkConnections* enumerator = nullptr;
        if (SUCCEEDED(manager->GetNetworkConnections(&enumerator))) {
            INetworkConnection* connection = nullptr;
            while (enumerator->Next(1, &connection, nullptr) == S_OK) {
                VARIANT_BOOL connected = VARIANT_FALSE;
                GUID id = {};
                if (connection->get_IsConnected(&connected) == S_OK &&
                    connected != VARIANT_FALSE &&
                    connection->GetAdapterId(&id) == S_OK &&
                    IsEqualGUID(id, adapterGuid)) {
                    INetwork* network = nullptr;
                    BSTR connectionName = nullptr;
                    if (connection->GetNetwork(&network) == S_OK && network) {
                        if (network->GetName(&connectionName) == S_OK &&
                            connectionName) {
                            name = connectionName;
                        }
                        if (connectionName) {
                            SysFreeString(connectionName);
                        }
                        network->Release();
                    }
                }
                connection->Release();
                if (!name.empty()) {
                    break;
                }
            }
            enumerator->Release();
        }
    }
    manager->Release();
    return name;
}

static NetworkInfo NetworkInfoFromAdapter(PIP_ADAPTER_ADDRESSES adapter) {
    NetworkInfo info;
    info.name = NetworkProfileName(adapter);
    if (info.name.empty()) {
        info.name = adapter->Description ? adapter->Description : L"";
    }
    info.glyph =
        adapter->IfType == IF_TYPE_IEEE80211 ? L"\xE701" : L"\xE839";
    return info;
}

static NetworkInfo ReadNetwork() {
    NetworkInfo info;
    std::vector<BYTE> buffer(16 * 1024);
    PIP_ADAPTER_ADDRESSES addresses = GetAdapterAddressesList(&buffer);
    if (!addresses) {
        return info;
    }

    PIP_ADAPTER_ADDRESSES best = nullptr;
    PIP_ADAPTER_ADDRESSES fallback = nullptr;
    for (auto* a = addresses; a; a = a->Next) {
        if (a->OperStatus != IfOperStatusUp ||
            a->IfType == IF_TYPE_SOFTWARE_LOOPBACK ||
            a->IfType == IF_TYPE_TUNNEL) {
            continue;
        }
        if (!fallback) {
            fallback = a;
        }
        if (a->FirstGatewayAddress) {
            best = a;
            break;
        }
    }
    if (!best) {
        best = fallback;
    }
    if (!best) {
        return info;
    }

    return NetworkInfoFromAdapter(best);
}

static NetworkInfo ReadNetworkByIndex(ULONG interfaceIndex,
                                      ADDRESS_FAMILY addressFamily) {
    NetworkInfo info;
    std::vector<BYTE> buffer(16 * 1024);
    PIP_ADAPTER_ADDRESSES addresses = GetAdapterAddressesList(&buffer);
    if (!addresses) {
        return info;
    }

    for (auto* adapter = addresses; adapter; adapter = adapter->Next) {
        bool matches = addressFamily == AF_INET
                           ? adapter->IfIndex == interfaceIndex
                           : addressFamily == AF_INET6 &&
                                 adapter->Ipv6IfIndex == interfaceIndex;
        if (adapter->OperStatus == IfOperStatusUp &&
            adapter->IfType != IF_TYPE_SOFTWARE_LOOPBACK && matches) {
            return NetworkInfoFromAdapter(adapter);
        }
    }
    return info;
}

static bool SameAddress(const SOCKADDR_STORAGE& address,
                        const SOCKADDR* candidate) {
    if (!candidate || address.ss_family != candidate->sa_family) {
        return false;
    }
    if (address.ss_family == AF_INET) {
        const auto* a = (const SOCKADDR_IN*)&address;
        const auto* b = (const SOCKADDR_IN*)candidate;
        return a->sin_addr.s_addr == b->sin_addr.s_addr;
    }
    if (address.ss_family == AF_INET6) {
        const auto* a = (const SOCKADDR_IN6*)&address;
        const auto* b = (const SOCKADDR_IN6*)candidate;
        return memcmp(&a->sin6_addr, &b->sin6_addr, sizeof(IN6_ADDR)) == 0 &&
               (!a->sin6_scope_id || !b->sin6_scope_id ||
                a->sin6_scope_id == b->sin6_scope_id);
    }
    return false;
}

static ULONG InterfaceIndexForLocalAddress(
    const SOCKADDR_STORAGE& localAddress, ADDRESS_FAMILY* addressFamily) {
    if (addressFamily) {
        *addressFamily = AF_UNSPEC;
    }
    std::vector<BYTE> buffer(16 * 1024);
    PIP_ADAPTER_ADDRESSES addresses = GetAdapterAddressesList(&buffer);
    if (!addresses) {
        return 0;
    }

    for (auto* adapter = addresses; adapter; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp ||
            adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK) {
            continue;
        }
        for (auto* unicast = adapter->FirstUnicastAddress; unicast;
             unicast = unicast->Next) {
            if (SameAddress(localAddress, unicast->Address.lpSockaddr)) {
                 if (addressFamily) {
                     *addressFamily = localAddress.ss_family;
                 }
                return localAddress.ss_family == AF_INET ? adapter->IfIndex
                                                          : adapter->Ipv6IfIndex;
            }
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Test state shared between the worker and the UI thread
// ---------------------------------------------------------------------------

enum class Phase {
    Idle,
    Warmup,
    Latency,
    Download,
    Upload,
    Stopping,
    Done,
    Error,
    Canceled,
};

struct Sample {
    double value;
    bool upload;
};

struct TestState {
    Phase phase = Phase::Idle;
    double currentMbps = 0;
    double download = 0;
    double upload = 0;
    double ping = 0;
    double jitter = 0;
    bool hasPing = false;
    bool hasJitter = false;
    bool hasResult = false;
    std::wstring caption;
    std::wstring error;
    std::wstring server = L"Cloudflare";
    SYSTEMTIME when = {};
    // Total bytes moved by the test, warm-up included, across both phases.
    LONGLONG dataUsed = 0;
    std::vector<Sample> samples;
};

static SRWLOCK g_stateLock = SRWLOCK_INIT;
static TestState g_state;
static std::atomic<LONG> g_cancelRequested = 0;

static TestState CopyState() {
    TestState copy;
    AcquireSRWLockShared(&g_stateLock);
    copy = g_state;
    ReleaseSRWLockShared(&g_stateLock);
    return copy;
}

static bool StateRunning(const TestState& s) {
    return s.phase == Phase::Warmup || s.phase == Phase::Latency ||
           s.phase == Phase::Download || s.phase == Phase::Upload ||
           s.phase == Phase::Stopping;
}

static bool TestRunning() {
    return StateRunning(CopyState());
}

static void StateReset() {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state = TestState();
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetPhase(Phase phase, PCWSTR caption) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.phase = phase;
        g_state.caption = caption ? caption : L"";
        g_state.currentMbps = 0;
        g_state.error.clear();
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetCaption(PCWSTR caption) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.caption = caption ? caption : L"";
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetCurrent(double mbps) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.currentMbps = mbps;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetLatency(double ping, bool hasPing, double jitter, bool hasJitter) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        g_state.ping = ping;
        g_state.hasPing = hasPing;
        g_state.jitter = jitter;
        g_state.hasJitter = hasJitter;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateAddSample(double value, bool upload) {
    if (!(value >= 0) || value > 1000000) {
        return;
    }
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        if (g_state.samples.size() >= 240) {
            g_state.samples.erase(g_state.samples.begin());
        }
        g_state.samples.push_back({value, upload});
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetMeasured(bool upload, double mbps) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase != Phase::Stopping) {
        if (upload) {
            g_state.upload = mbps;
        } else {
            g_state.download = mbps;
        }
        g_state.currentMbps = mbps;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

// Called from the test thread after each transfer phase: warm-up included,
// because the user pays for every byte, not just the measured ones.
static void StateAddDataUsed(LONGLONG bytes) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.dataUsed += bytes;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateSetServer(const std::wstring& server) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.server = server.empty() ? L"Cloudflare" : server;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateFinish(double download, double upload, double ping, double jitter,
                        bool hasLatency) {
    AcquireSRWLockExclusive(&g_stateLock);
    if (g_state.phase == Phase::Stopping) {
        g_state.phase = Phase::Canceled;
        g_state.caption = g_str->canceled;
        g_state.currentMbps = 0;
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }
    g_state.phase = Phase::Done;
    g_state.caption = g_str->completed;
    g_state.download = download;
    g_state.upload = upload;
    g_state.ping = ping;
    g_state.jitter = jitter;
    g_state.hasPing = hasLatency;
    g_state.hasJitter = hasLatency;
    g_state.hasResult = true;
    GetLocalTime(&g_state.when);
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateFail(const std::wstring& message) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.phase = Phase::Error;
    g_state.caption = g_str->error;
    g_state.error = message;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void StateCanceled() {
    AcquireSRWLockExclusive(&g_stateLock);
    g_state.phase = Phase::Canceled;
    g_state.caption = g_str->canceled;
    g_state.currentMbps = 0;
    ReleaseSRWLockExclusive(&g_stateLock);
}

static void RequestTestCancel() {
    g_cancelRequested.store(1, std::memory_order_relaxed);
    AcquireSRWLockExclusive(&g_stateLock);
    if (StateRunning(g_state)) {
        g_state.phase = Phase::Stopping;
        g_state.caption = g_str->stopping;
        g_state.currentMbps = 0;
    }
    ReleaseSRWLockExclusive(&g_stateLock);
}

// ---------------------------------------------------------------------------
// Cloudflare measurement engine (WinHTTP)
// ---------------------------------------------------------------------------

static constexpr PCWSTR kHost = L"speed.cloudflare.com";

// UI host window and the running measurement thread (only in Explorer).
static HWND g_flyoutHwnd = nullptr;
static HANDLE g_testThread = nullptr;

static bool CancelRequested(const std::atomic<LONG>* stop = nullptr) {
    return g_cancelRequested.load(std::memory_order_relaxed) != 0 ||
           (stop && stop->load(std::memory_order_relaxed) != 0);
}

struct TransferCounter {
    std::atomic<LONGLONG> bytes = 0;
    // Per-phase data budget in bytes (0 = unlimited). Workers stop requesting
    // new transfers once it is spent, and read/write loops cut in-flight
    // transfers short when they notice; the partial bytes still count, so the
    // total never overshoots the budget by much. Lines that don't reach the
    // budget keep the full window; faster lines end the phase early and report
    // bytes over the actual elapsed time.
    LONGLONG budget = 0;

    bool BudgetExceeded() const {
        return budget > 0 &&
               bytes.load(std::memory_order_relaxed) >= budget;
    }
};

static void AddBytes(TransferCounter* counter, long long count) {
    counter->bytes.fetch_add(count, std::memory_order_relaxed);
}

class HttpClient {
public:
    HttpClient() = default;
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    ~HttpClient() { Close(); }

    bool Open() {
        Close();
        session_ = WinHttpOpen(L"NetworkMenuSpeedTest/1.0",
                               WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                               WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!session_) {
            return false;
        }
        // Resolve/connect stay short so a cancel or a mod unload can unwind a
        // blocked worker promptly. The send timeout is long because it bounds
        // WinHttpWriteData when the TCP window is full: with 8 parallel upload
        // workers on a slow line a 256 KB block can legitimately take tens of
        // seconds, and a 3 s limit failed the upload incorrectly. Cancellation
        // is signalled through the stop flag; workers check it between blocks.
        if (!WinHttpSetTimeouts(session_, 3000, 3000, 30000, 3000)) {
            DWORD error = GetLastError();
            Close();
            SetLastError(error);
            return false;
        }
        connect_ = WinHttpConnect(session_, kHost, INTERNET_DEFAULT_HTTPS_PORT, 0);
        return connect_ != nullptr;
    }

    void Close() {
        if (connect_) {
            WinHttpCloseHandle(connect_);
            connect_ = nullptr;
        }
        if (session_) {
            WinHttpCloseHandle(session_);
            session_ = nullptr;
        }
    }

    bool Get(PCWSTR path, std::wstring* body,
             const std::atomic<LONG>* stop,
             WINHTTP_CONNECTION_INFO* connectionInfo = nullptr) {
        DWORD status = 0;
        HINTERNET request = OpenRequest(L"GET", path);
        if (!request) {
            return false;
        }
        bool ok = SendAndReceive(request, 0, stop, &status);
        if (ok && connectionInfo) {
            *connectionInfo = {};
            connectionInfo->cbSize = sizeof(*connectionInfo);
            DWORD size = sizeof(*connectionInfo);
            if (!WinHttpQueryOption(request, WINHTTP_OPTION_CONNECTION_INFO,
                                    connectionInfo, &size)) {
                Wh_Log(L"WinHTTP connection info query failed (error %u)",
                       GetLastError());
            }
        }
        if (ok && body) {
            ok = ReadAll(request, body, stop);
        }
        WinHttpCloseHandle(request);
        return ok && status >= 200 && status < 300;
    }

    bool Download(PCWSTR path, TransferCounter* counter,
                  const std::atomic<LONG>* stop,
                  DWORD* statusOut) {
        DWORD status = 0;
        HINTERNET request = OpenRequest(L"GET", path);
        if (!request) {
            return false;
        }
        bool ok = SendAndReceive(request, 0, stop, &status);
        if (ok) {
            char buffer[64 * 1024];
            for (;;) {
                if (CancelRequested(stop) ||
                    counter->BudgetExceeded()) {
                    ok = false;
                    break;
                }
                DWORD read = 0;
                if (!WinHttpReadData(request, buffer, sizeof(buffer), &read)) {
                    ok = false;
                    break;
                }
                if (read == 0) {
                    break;
                }
                AddBytes(counter, read);
            }
        }
        WinHttpCloseHandle(request);
        if (statusOut) {
            *statusOut = status;
        }
        bool success = ok && status >= 200 && status < 300;
        return success;
    }

    bool Upload(PCWSTR path, const char* data, DWORD size,
                TransferCounter* counter, const std::atomic<LONG>* stop,
                DWORD* statusOut) {
        DWORD status = 0;
        HINTERNET request = OpenRequest(L"POST", path);
        if (!request) {
            return false;
        }

        wchar_t headers[] = L"Content-Type: application/octet-stream\r\n";
        bool ok = !CancelRequested(stop) &&
                  WinHttpSendRequest(request, headers, (DWORD)-1L,
                                     WINHTTP_NO_REQUEST_DATA, 0, size, 0) != FALSE;

        DWORD sentTotal = 0;
        const DWORD chunk = 32 * 1024;
        while (ok && sentTotal < size) {
            if (CancelRequested(stop) || counter->BudgetExceeded()) {
                ok = false;
                break;
            }
            DWORD part = std::min(chunk, size - sentTotal);
            DWORD written = 0;
            if (!WinHttpWriteData(request, data + sentTotal, part, &written) ||
                written == 0) {
                ok = false;
                break;
            }
            AddBytes(counter, written);
            sentTotal += written;
        }

        if (ok) {
            ok = WinHttpReceiveResponse(request, nullptr) != FALSE;
            if (ok) {
                DWORD sizeOfStatus = sizeof(status);
                ok = WinHttpQueryHeaders(
                         request,
                         WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                         WINHTTP_HEADER_NAME_BY_INDEX, &status, &sizeOfStatus,
                         WINHTTP_NO_HEADER_INDEX) != FALSE;
            }
        }
        if (CancelRequested(stop)) {
            ok = false;
        }

        WinHttpCloseHandle(request);
        if (statusOut) {
            *statusOut = status;
        }
        bool success = ok && status >= 200 && status < 300;
        return success;
    }

private:
    HINTERNET OpenRequest(PCWSTR method, PCWSTR path) {
        if (!connect_) {
            return nullptr;
        }
        return WinHttpOpenRequest(connect_, method, path, nullptr, WINHTTP_NO_REFERER,
                                  WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    }

    bool SendAndReceive(HINTERNET request, DWORD bodySize,
                        const std::atomic<LONG>* stop,
                        DWORD* statusOut) {
        if (CancelRequested(stop)) {
            return false;
        }
        wchar_t headers[] = L"Cache-Control: no-cache\r\n";
        if (!WinHttpSendRequest(request, headers, (DWORD)-1L, WINHTTP_NO_REQUEST_DATA,
                                bodySize, 0, 0)) {
            return false;
        }
        if (CancelRequested(stop)) {
            return false;
        }
        if (!WinHttpReceiveResponse(request, nullptr)) {
            return false;
        }
        if (CancelRequested(stop)) {
            return false;
        }
        DWORD status = 0;
        DWORD size = sizeof(status);
        if (!WinHttpQueryHeaders(
                request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size,
                WINHTTP_NO_HEADER_INDEX)) {
            return false;
        }
        if (statusOut) {
            *statusOut = status;
        }
        return true;
    }

    bool ReadAll(HINTERNET request, std::wstring* body,
                 const std::atomic<LONG>* stop) {
        std::string bytes;
        char buffer[16 * 1024];
        for (;;) {
            if (CancelRequested(stop)) {
                return false;
            }
            DWORD read = 0;
            if (!WinHttpReadData(request, buffer, sizeof(buffer), &read)) {
                return false;
            }
            if (read == 0) {
                break;
            }
            bytes.append(buffer, read);
        }
        body->assign(bytes.begin(), bytes.end());
        return true;
    }

    HINTERNET session_ = nullptr;
    HINTERNET connect_ = nullptr;
};

// ---------------------------------------------------------------------------
// Measurement phases
// ---------------------------------------------------------------------------

static constexpr int kDownloadChunk = 8'000'000;
static constexpr int kUploadChunk = 4'000'000;
static constexpr int kDownloadWorkers = 12;
static constexpr int kUploadWorkers = 8;
static constexpr int kTransferWarmupMs = 2000;
static constexpr int kTransferMeasureMs = 10000;
// Warm-up data is counted but excluded from the measurement with a byte
// baseline. Its cap is a quarter of the per-direction budget (128 MB at the
// 512 MB default), leaving the rest for a meaningful measured window. When
// the budget is unlimited, keep the 128 MB warm-up cap so a fast line still
// enters the measured window quickly.
static constexpr LONGLONG kWarmupBudgetBytes = 128'000'000;
// Even when the budget ends a phase early, keep the phase alive for at least
// this long while the in-flight transfers drain. The reported average still
// covers only the time bytes actually moved, so a lower budget shortens the
// measured interval on fast lines.
static constexpr int kTransferMinPhaseMs = 4000;
static constexpr int kLatencySamples = 12;
static constexpr int kLatencyWarmup = 2;

static std::wstring Widen(const std::string& text) {
    std::wstring result;
    result.reserve(text.size());
    for (unsigned char c : text) {
        result.push_back((wchar_t)c);
    }
    return result;
}

static void ReadTrace(std::wstring* server, ULONG* interfaceIndex,
                      ADDRESS_FAMILY* interfaceFamily) {
    if (interfaceIndex) {
        *interfaceIndex = 0;
    }
    if (interfaceFamily) {
        *interfaceFamily = AF_UNSPEC;
    }
    HttpClient client;
    if (!client.Open()) {
        return;
    }
    std::wstring body;
    WINHTTP_CONNECTION_INFO connectionInfo = {};
    if (!client.Get(L"/cdn-cgi/trace", &body, nullptr, &connectionInfo)) {
        return;
    }
    if (interfaceIndex) {
        *interfaceIndex = InterfaceIndexForLocalAddress(
            connectionInfo.LocalAddress, interfaceFamily);
    }

    std::wstring colo, loc;
    size_t start = 0;
    while (start < body.size()) {
        size_t end = body.find(L'\n', start);
        if (end == std::wstring::npos) {
            end = body.size();
        }
        std::wstring line = body.substr(start, end - start);
        size_t eq = line.find(L'=');
        if (eq != std::wstring::npos) {
            std::wstring key = line.substr(0, eq);
            std::wstring value = line.substr(eq + 1);
            if (key == L"colo") {
                colo = value;
            } else if (key == L"loc") {
                loc = value;
            }
        }
        start = end + 1;
    }

    if (!colo.empty()) {
        *server = loc.empty() ? colo : (colo + L" (" + loc + L")");
    }
}

// Time a small uncached request through WinHTTP so the latency probe follows
// the same proxy, DNS, IPv4/IPv6, and routing path as the transfer test.
static bool ProbeHttp(HttpClient& client, double* milliseconds) {
    LARGE_INTEGER frequency = {};
    LARGE_INTEGER started = {};
    if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0 ||
        !QueryPerformanceCounter(&started)) {
        return false;
    }

    wchar_t path[96];
    swprintf(path, 96, L"/cdn-cgi/trace?ping=%u", NextRandom());
    std::wstring body;
    bool ok = client.Get(path, &body, nullptr);

    LARGE_INTEGER finished = {};
    if (!ok || !QueryPerformanceCounter(&finished) ||
        finished.QuadPart <= started.QuadPart) {
        return false;
    }
    *milliseconds = (finished.QuadPart - started.QuadPart) * 1000.0 /
                    frequency.QuadPart;
    return true;
}

static void MeasureLatency(double* pingOut, double* jitterOut) {
    HttpClient client;
    if (!client.Open()) {
        DWORD error = GetLastError();
        Wh_Log(L"WinHTTP open failed for latency probe (error %u)", error);
        throw std::wstring(g_str->errConnect);
    }

    std::vector<double> samples;
    samples.reserve(kLatencySamples);
    int failures = 0;

    for (int i = 0; i < kLatencySamples; i++) {
        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }

        double milliseconds = 0;
        bool ok = ProbeHttp(client, &milliseconds);
        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }
        if (ok && i >= kLatencyWarmup) {
            samples.push_back(milliseconds);
        } else if (!ok) {
            failures++;
        }

        if (!samples.empty()) {
            double ping = Percentile(samples, 50);
            StateSetLatency(ping, true, 0, false);
        }
        StateSetCaption(g_str->ping);

        Sleep(30);
    }

    if (samples.empty()) {
        throw std::wstring(g_str->errPing);
    }

    double ping = Percentile(samples, 50);
    double jitter = 0;
    if (samples.size() > 1) {
        double total = 0;
        for (size_t i = 1; i < samples.size(); i++) {
            total += std::fabs(samples[i] - samples[i - 1]);
        }
        jitter = total / (samples.size() - 1);
    }

    if (failures > 0) {
        Wh_Log(L"%d latency probes failed", failures);
    }
    *pingOut = ping;
    *jitterOut = jitter;
}

struct DownloadJob {
    TransferCounter* counter;
    const std::atomic<LONG>* stop;
    DWORD requests;
    DWORD failures;
};

struct UploadJob {
    TransferCounter* counter;
    const std::atomic<LONG>* stop;
    const char* data;
    DWORD size;
    DWORD requests;
    DWORD failures;
};

static DWORD WINAPI DownloadWorker(LPVOID parameter) {
    auto* job = (DownloadJob*)parameter;
    HttpClient client;
    if (!client.Open()) {
        DWORD error = GetLastError();
        job->failures++;
        Wh_Log(L"Download worker could not open WinHTTP session (error %u)",
               error);
        return 0;
    }

    while (!CancelRequested(job->stop) && !job->counter->BudgetExceeded()) {
        wchar_t path[96];
        swprintf(path, 96, L"/__down?bytes=%d&r=%u", kDownloadChunk, NextRandom());
        DWORD status = 0;
        job->requests++;
        bool ok = client.Download(path, job->counter, job->stop, &status);
        if (!ok && !CancelRequested(job->stop)) {
            // A transfer cut short by the data budget is not a failure: the
            // request would have succeeded.
            if (job->counter->BudgetExceeded()) {
                break;
            }
            job->failures++;
            Sleep(status == 429 ? 500 : 250);
        }
    }
    return 0;
}

static DWORD WINAPI UploadWorker(LPVOID parameter) {
    auto* job = (UploadJob*)parameter;
    HttpClient client;
    if (!client.Open()) {
        DWORD error = GetLastError();
        job->failures++;
        Wh_Log(L"Upload worker could not open WinHTTP session (error %u)",
               error);
        return 0;
    }

    while (!CancelRequested(job->stop) && !job->counter->BudgetExceeded()) {
        DWORD status = 0;
        job->requests++;
        bool ok = client.Upload(L"/__up", job->data, job->size, job->counter,
                                job->stop, &status);
        if (!ok && !CancelRequested(job->stop)) {
            // A transfer cut short by the data budget is not a failure: the
            // request would have succeeded.
            if (job->counter->BudgetExceeded()) {
                break;
            }
            job->failures++;
            Sleep(status == 429 ? 500 : 250);
        }
    }
    return 0;
}

struct WorkerGroup {
    HANDLE* handles;
    int count;
    std::atomic<LONG>* stop;

    void Stop() {
        stop->store(1, std::memory_order_relaxed);
        for (int i = 0; i < count; i++) {
            if (handles[i]) {
                DWORD result = WaitForSingleObject(handles[i], INFINITE);
                if (result != WAIT_OBJECT_0) {
                    Wh_Log(L"Transfer worker wait failed (error %u)",
                           GetLastError());
                }
                CloseHandle(handles[i]);
                handles[i] = nullptr;
            }
        }
    }

    ~WorkerGroup() { Stop(); }
};

static bool TransferWorkerRunning(HANDLE* workers, int count) {
    for (int i = 0; i < count; i++) {
        if (WaitForSingleObject(workers[i], 0) == WAIT_TIMEOUT) {
            return true;
        }
    }
    return false;
}

static double MeasureTransfer(bool upload) {
    LONGLONG budget = g_transferBudgetBytes.load(std::memory_order_relaxed);
    LONGLONG warmupBudget =
        budget > 0 ? budget / 4 : kWarmupBudgetBytes;
    TransferCounter counter;
    counter.budget = budget;
    std::atomic<LONG> stop = 0;

    const int workerCount = upload ? kUploadWorkers : kDownloadWorkers;
    HANDLE workers[kDownloadWorkers > kUploadWorkers ? kDownloadWorkers
                                                      : kUploadWorkers] = {};
    DownloadJob downloadJobs[kDownloadWorkers] = {};
    UploadJob uploadJobs[kUploadWorkers] = {};
    std::vector<char> uploadData;

    if (upload) {
        uploadData.resize(kUploadChunk);
        for (size_t i = 0; i < uploadData.size(); i++) {
            uploadData[i] = (char)(NextRandom() & 0xFF);
        }
    }

    WorkerGroup workerGroup = {workers, workerCount, &stop};
    for (int i = 0; i < workerCount; i++) {
        if (upload) {
            uploadJobs[i] = {&counter, &stop, uploadData.data(),
                             (DWORD)uploadData.size(), 0, 0};
            workers[i] = CreateThread(nullptr, 0, UploadWorker, &uploadJobs[i], 0, nullptr);
        } else {
            downloadJobs[i] = {&counter, &stop, 0, 0};
            workers[i] = CreateThread(nullptr, 0, DownloadWorker, &downloadJobs[i], 0, nullptr);
        }
        if (!workers[i]) {
            DWORD error = GetLastError();
            workerGroup.Stop();
            Wh_Log(L"Unable to create %s worker %d (error %u)",
                   upload ? L"upload" : L"download", i, error);
            throw std::wstring(g_str->errUnexpected);
        }
    }

    // Warm-up data is counted as it arrives, then excluded by taking a byte
    // baseline immediately before the measured interval. Warm-up also stops
    // early once its own budget is spent: the workers keep running and the
    // in-flight data flows into the measured window instead of exhausting the
    // total budget before it starts.
    ULONGLONG warmupStart = Now();
    while (Now() - warmupStart < kTransferWarmupMs) {
        if (CancelRequested()) {
            workerGroup.Stop();
            throw std::wstring(g_str->errCanceled);
        }
        bool anyWorkerRunning = TransferWorkerRunning(workers, workerCount);
        if (!anyWorkerRunning) {
            if (counter.BudgetExceeded()) {
                break;
            }
            workerGroup.Stop();
            throw std::wstring(upload ? g_str->errUpload : g_str->errDownload);
        }
        if (counter.BudgetExceeded() ||
            counter.bytes.load(std::memory_order_relaxed) >= warmupBudget) {
            break;
        }
        Sleep(100);
    }

    LONGLONG base = counter.bytes.load(std::memory_order_relaxed);
    ULONGLONG measuredStart = Now();
    // Time of the last byte that arrived: the result divides by the time the
    // transfer was actually moving data, not by the full window, so a phase
    // ended early by the budget is not diluted with idle tail time.
    ULONGLONG lastTraffic = measuredStart;
    LONGLONG prevTotal = base;
    std::deque<std::pair<ULONGLONG, LONGLONG>> window;
    double current = 0;
    // Once the budget is spent the display stops updating: the tail after the
    // in-flight transfers drain is idle time and would otherwise paint a
    // meaningless drop toward zero on the gauge and the chart.
    bool budgetSpent = counter.BudgetExceeded();

    while (Now() - measuredStart < kTransferMeasureMs) {
        if (CancelRequested()) {
            workerGroup.Stop();
            throw std::wstring(g_str->errCanceled);
        }
        Sleep(100);

        ULONGLONG now = Now();
        LONGLONG total = counter.bytes.load(std::memory_order_relaxed);
        if (total > prevTotal) {
            lastTraffic = now;
            prevTotal = total;
        }
        if (!budgetSpent && counter.BudgetExceeded()) {
            budgetSpent = true;
        }
        window.push_back({now, total});
        while (window.size() > 2 && now - window.front().first > 1000) {
            window.pop_front();
        }
        if (window.size() >= 2) {
            double seconds = (window.back().first - window.front().first) / 1000.0;
            if (seconds > 0.05) {
                current = (window.back().second - window.front().second) * 8.0 /
                          seconds / 1'000'000.0;
            }
        }

        if (!budgetSpent) {
            StateSetCurrent(current);
            StateSetCaption(upload ? g_str->upload : g_str->download);
            StateAddSample(current, upload);
        }

        // The budget can end the phase early on fast lines: once it is spent,
        // keep sampling while the in-flight transfers drain, and never end
        // before the minimum window so the average is not just the ramp.
        if (counter.BudgetExceeded() &&
            !TransferWorkerRunning(workers, workerCount) &&
            Now() - measuredStart >= kTransferMinPhaseMs) {
            break;
        }
    }

    LONGLONG total = counter.bytes.load(std::memory_order_relaxed);
    workerGroup.Stop();

    if (CancelRequested()) {
        throw std::wstring(g_str->errCanceled);
    }

    // Divide by the time the transfer was actually moving data. On the full
    // 10 s window this is effectively the same; on budget-shortened phases it
    // excludes the idle tail left after the in-flight transfers drained.
    double seconds = (lastTraffic - measuredStart) / 1000.0;
    long long bytes = total - base;
    if (bytes <= 0 || seconds <= 0) {
        throw std::wstring(upload
                               ? g_str->errUpload
                               : g_str->errDownload);
    }

    // Report exactly what the connection moved: total bytes over the measured
    // window. No percentile uplift and no multiplier, so the figure cannot be
    // inflated.
    double mbps = bytes * 8.0 / seconds / 1'000'000.0;

    DWORD requests = 0;
    DWORD failures = 0;
    for (int i = 0; i < workerCount; i++) {
        requests += upload ? uploadJobs[i].requests : downloadJobs[i].requests;
        failures += upload ? uploadJobs[i].failures : downloadJobs[i].failures;
    }
    Wh_Log(L"%s: %.1f Mbps, %u requests, %u failures (window %.1f s)",
           upload ? L"upload" : L"download", mbps, requests, failures, seconds);

    StateSetMeasured(upload, mbps);
    StateAddDataUsed(counter.bytes.load(std::memory_order_relaxed));
    // Do not append the window average to the chart: the chart shows the
    // 100 ms samples, and the average would appear as an artificial last point.
    return mbps;
}

static void RunTestWorker() {
    try {
        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }
        StateSetPhase(Phase::Warmup, g_str->connecting);

        std::wstring server = L"Cloudflare";
        ULONG interfaceIndex = 0;
        ADDRESS_FAMILY interfaceFamily = AF_UNSPEC;
        ReadTrace(&server, &interfaceIndex, &interfaceFamily);
        StateSetServer(server);
        if (interfaceIndex && g_flyoutHwnd) {
            PostMessageW(g_flyoutHwnd, WM_APP + 4, interfaceIndex,
                         interfaceFamily);
        }

        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }

        StateSetPhase(Phase::Latency, g_str->ping);
        double ping = 0;
        double jitter = 0;
        bool hasLatency = false;
        try {
            MeasureLatency(&ping, &jitter);
            hasLatency = true;
            StateSetLatency(ping, true, jitter, true);
        } catch (const std::wstring& error) {
            if (CancelRequested()) {
                throw;
            }
            Wh_Log(L"Latency unavailable; continuing with throughput test: %s",
                   error.c_str());
            StateSetLatency(0, false, 0, false);
        }

        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }

        StateSetPhase(Phase::Download, g_str->download);
        double download = MeasureTransfer(false);

        if (CancelRequested()) {
            throw std::wstring(g_str->errCanceled);
        }

        StateSetPhase(Phase::Upload, g_str->upload);
        double upload = MeasureTransfer(true);

        StateFinish(download, upload, ping, jitter, hasLatency);
        if (g_flyoutHwnd) {
            PostMessageW(g_flyoutHwnd, WM_APP + 1, 0, 0);
        }
    } catch (const std::wstring& error) {
        if (CancelRequested()) {
            StateCanceled();
        } else {
            StateFail(error);
        }
        if (g_flyoutHwnd) {
            PostMessageW(g_flyoutHwnd, WM_APP + 1, 0, 0);
        }
    } catch (...) {
        if (CancelRequested()) {
            StateCanceled();
        } else {
            StateFail(g_str->errUnexpected);
        }
        if (g_flyoutHwnd) {
            PostMessageW(g_flyoutHwnd, WM_APP + 1, 0, 0);
        }
    }
}

static DWORD WINAPI TestWorkerThread(LPVOID) {
    RunTestWorker();
    return 0;
}

// ---------------------------------------------------------------------------
// Direct2D surface
// ---------------------------------------------------------------------------

static ID2D1Factory* g_d2dFactory = nullptr;
static IDWriteFactory* g_dwriteFactory = nullptr;
static ID2D1RenderTarget* g_renderTarget = nullptr;
static ID2D1HwndRenderTarget* g_hwndTarget = nullptr;
static ID2D1StrokeStyle* g_roundStroke = nullptr;

// DirectComposition path. It is always attempted first because the flyout
// window keeps WS_EX_NOREDIRECTIONBITMAP, so an ID2D1HwndRenderTarget has no
// redirection surface to present into; the HWND target below is only reached
// if D3D/DComp is unavailable (e.g. no GPU and no WARP). When the system
// transparency setting is off, the composition surface is painted with a
// solid background instead of showing the DWM backdrop through.
static bool g_composition = false;
static ID3D11Device* g_d3dDevice = nullptr;
static IDXGIDevice* g_dxgiDevice = nullptr;
static IDXGISwapChain1* g_swapChain = nullptr;
static IDCompositionDevice* g_dcompDevice = nullptr;
static IDCompositionTarget* g_dcompTarget = nullptr;
static IDCompositionVisual* g_dcompVisual = nullptr;
static ID2D1DeviceContext* g_context = nullptr;
static ID2D1Bitmap1* g_contextBitmap = nullptr;
static UINT g_targetWidth = 0;
static UINT g_targetHeight = 0;
static float g_targetDpi = 0;

// Acrylic grain. Windows paints a very faint noise texture over the material to
// hide banding in the blur. DWM's backdrop carries its own, but the dark tint
// flattens it, so an equivalent texture is drawn here (below the content).
static ID2D1Bitmap* g_noiseBitmap = nullptr;
static ID2D1BitmapBrush* g_noiseBrush = nullptr;
static constexpr int kNoiseSize = 128;
static constexpr float kNoiseOpacity = 0.06f;

static Palette g_palette = {};
static bool g_dark = true;
static bool g_systemLight = false;
static bool g_transparencyEnabled = false;
static bool g_materialSupported = false;
static bool g_translucent = false;
static float g_dpi = 96.0f;
static float g_scale = 1.0f;

// Re-reads everything that decides the surface look. Called when the window is
// opened and when Windows reports a theme/transparency change, never from the
// render loop.
static void RefreshAppearance() {
    g_systemLight = ReadSystemLightTheme();
    g_dark = !g_systemLight;
    g_palette = MakePalette(g_systemLight);
    g_transparencyEnabled = ReadTransparencyEnabled();
    g_materialSupported = MaterialSupported();
    g_translucent = g_materialSupported && g_transparencyEnabled;
}

struct TextKey {
    float size;
    int weight;
    int align;
    int valign;
    bool wrap;
    int family;

    bool operator==(const TextKey& other) const {
        return size == other.size && weight == other.weight &&
               align == other.align && valign == other.valign &&
               wrap == other.wrap && family == other.family;
    }
};

static std::vector<std::pair<TextKey, IDWriteTextFormat*>> g_textFormats;

static PCWSTR FamilyName(int family) {
    switch (family) {
        case 1:
            return kDisplayFamily;
        case 2:
            return kIconFamily;
        default:
            return kTextFamily;
    }
}

// Locale name for text shaping (digit forms, line breaking, bidi): follows the
// user's regional format, independent of the flyout language.
static PCWSTR LocaleName() {
    static wchar_t name[LOCALE_NAME_MAX_LENGTH];
    if (!name[0] &&
        GetUserDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH) <= 0) {
        wcscpy_s(name, L"en-US");
    }
    return name;
}

static IDWriteTextFormat* GetTextFormat(float size, DWRITE_FONT_WEIGHT weight,
                                        DWRITE_TEXT_ALIGNMENT align,
                                        DWRITE_PARAGRAPH_ALIGNMENT valign,
                                        bool wrap, int family) {
    TextKey key = {size, (int)weight, (int)align, (int)valign, wrap, family};
    for (const auto& entry : g_textFormats) {
        if (entry.first == key) {
            return entry.second;
        }
    }
    if (!g_dwriteFactory) {
        return nullptr;
    }

    IDWriteTextFormat* format = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextFormat(
            FamilyName(family), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, LocaleName(), &format))) {
        return nullptr;
    }
    format->SetTextAlignment(align);
    format->SetParagraphAlignment(valign);
    format->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP
                                 : DWRITE_WORD_WRAPPING_NO_WRAP);
    g_textFormats.push_back({key, format});
    return format;
}

static D2D1_RECT_F Rect(float left, float top, float right, float bottom) {
    D2D1_RECT_F r;
    r.left = left;
    r.top = top;
    r.right = right;
    r.bottom = bottom;
    return r;
}

struct BrushEntry {
    D2D1_COLOR_F color;
    ID2D1SolidColorBrush* brush;
};

static std::vector<BrushEntry> g_brushes;

static bool SameColor(D2D1_COLOR_F a, D2D1_COLOR_F b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

static void ReleaseBrushes() {
    for (auto& entry : g_brushes) {
        entry.brush->Release();
    }
    g_brushes.clear();
}

// Brushes are cached per colour and handed out with an extra AddRef, so the
// call sites can keep releasing the returned brush.
static ID2D1SolidColorBrush* Brush(D2D1_COLOR_F color) {
    if (!g_renderTarget) {
        return nullptr;
    }
    for (const auto& entry : g_brushes) {
        if (SameColor(entry.color, color)) {
            entry.brush->AddRef();
            return entry.brush;
        }
    }
    if (g_brushes.size() >= 128) {
        ReleaseBrushes();
    }
    ID2D1SolidColorBrush* brush = nullptr;
    if (FAILED(g_renderTarget->CreateSolidColorBrush(color, &brush))) {
        return nullptr;
    }
    g_brushes.push_back({color, brush});
    brush->AddRef();
    return brush;
}

static void FillRounded(D2D1_RECT_F rect, float radius, D2D1_COLOR_F color) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    D2D1_ROUNDED_RECT rr = {rect, radius, radius};
    g_renderTarget->FillRoundedRectangle(rr, brush);
    brush->Release();
}

static void StrokeRounded(D2D1_RECT_F rect, float radius, D2D1_COLOR_F color,
                          float width = 1.0f) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    D2D1_ROUNDED_RECT rr = {rect, radius, radius};
    g_renderTarget->DrawRoundedRectangle(rr, brush, width);
    brush->Release();
}

// WinUI focus visual: a 2 px neutral ring around the control, offset by 1 px
// so it never sits on the control's own border, plus a 1 px inner stroke in
// the opposite tone, as in the system focus visuals. The ring is deliberately
// neutral (light-on-dark / dark-on-light), not the accent color.
static void DrawFocusRing(D2D1_RECT_F rect, float radius, float alpha) {
    D2D1_RECT_F outer = Rect(rect.left - 3.0f, rect.top - 3.0f,
                             rect.right + 3.0f, rect.bottom + 3.0f);
    D2D1_COLOR_F ring = WithAlpha(g_palette.text, alpha);
    D2D1_COLOR_F inner = g_systemLight ? Col(0xFFFFFF, alpha)
                                       : Col(0x000000, alpha);
    StrokeRounded(outer, radius + 3.0f, ring, 2.0f);
    StrokeRounded(Rect(outer.left + 1.0f, outer.top + 1.0f, outer.right - 1.0f,
                       outer.bottom - 1.0f),
                  radius + 2.0f, inner, 1.0f);
}

static void FillRect(D2D1_RECT_F rect, D2D1_COLOR_F color) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    g_renderTarget->FillRectangle(rect, brush);
    brush->Release();
}

static void ReleaseNoise() {
    if (g_noiseBrush) {
        g_noiseBrush->Release();
        g_noiseBrush = nullptr;
    }
    if (g_noiseBitmap) {
        g_noiseBitmap->Release();
        g_noiseBitmap = nullptr;
    }
}

// Builds the grain once per render target. Values are centred on the dark
// palette colour and painted at a very low opacity, so the average colour does
// not shift: only ~2 levels of texture, like the shell material.
static bool EnsureNoise() {
    if (g_noiseBrush) {
        return true;
    }
    if (!g_renderTarget) {
        return false;
    }

    std::vector<UINT32> pixels((size_t)kNoiseSize * kNoiseSize);
    UINT32 state = 0x9E3779B9u;
    for (auto& pixel : pixels) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        int level = 0x2C + (int)(state % 61) - 30;
        if (level < 0) {
            level = 0;
        } else if (level > 255) {
            level = 255;
        }
        pixel = 0xFF000000u | ((UINT32)level << 16) | ((UINT32)level << 8) |
                (UINT32)level;
    }

    D2D1_BITMAP_PROPERTIES props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    if (FAILED(g_renderTarget->CreateBitmap(
            D2D1::SizeU(kNoiseSize, kNoiseSize), pixels.data(),
            kNoiseSize * sizeof(UINT32), &props, &g_noiseBitmap))) {
        return false;
    }

    D2D1_BITMAP_BRUSH_PROPERTIES brushProps = {};
    brushProps.extendModeX = D2D1_EXTEND_MODE_WRAP;
    brushProps.extendModeY = D2D1_EXTEND_MODE_WRAP;
    brushProps.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
    if (FAILED(g_renderTarget->CreateBitmapBrush(g_noiseBitmap, &brushProps,
                                                nullptr, &g_noiseBrush))) {
        ReleaseNoise();
        return false;
    }
    return true;
}

static void DrawLine(float x1, float y1, float x2, float y2, D2D1_COLOR_F color,
                     float width = 1.0f) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    g_renderTarget->DrawLine({x1, y1}, {x2, y2}, brush, width);
    brush->Release();
}

static void FillCircle(float cx, float cy, float radius, D2D1_COLOR_F color) {
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!brush) {
        return;
    }
    D2D1_ELLIPSE ellipse = {{cx, cy}, radius, radius};
    g_renderTarget->FillEllipse(ellipse, brush);
    brush->Release();
}

static void Text(const std::wstring& text, D2D1_RECT_F rect, float size,
                 D2D1_COLOR_F color, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
                 DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING,
                 DWRITE_PARAGRAPH_ALIGNMENT valign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                 int family = 0, bool wrap = false, bool tabular = false) {
    if (text.empty()) {
        return;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight, align, valign, wrap, family);
    ID2D1SolidColorBrush* brush = Brush(color);
    if (!format || !brush) {
        if (brush) {
            brush->Release();
        }
        return;
    }

    if (!tabular || !g_dwriteFactory) {
        g_renderTarget->DrawTextW(text.c_str(), (UINT32)text.size(), format, rect, brush,
                                  D2D1_DRAW_TEXT_OPTIONS_NONE);
        brush->Release();
        return;
    }

    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(
            text.c_str(), (UINT32)text.size(), format,
            std::max(1.0f, rect.right - rect.left), std::max(1.0f, rect.bottom - rect.top),
            &layout))) {
        brush->Release();
        return;
    }
    // Tabular figures keep the width of every digit identical, so a live value
    // does not jitter while it changes. The layout is drawn from the rect's
    // top-left corner; alignment still comes from the text format.
    IDWriteTypography* typography = nullptr;
    if (SUCCEEDED(g_dwriteFactory->CreateTypography(&typography))) {
        DWRITE_FONT_FEATURE feature = {DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1};
        typography->AddFontFeature(feature);
        layout->SetTypography(typography, {0, (UINT32)text.size()});
        typography->Release();
    }
    g_renderTarget->DrawTextLayout({rect.left, rect.top}, layout, brush,
                                   D2D1_DRAW_TEXT_OPTIONS_NONE);
    layout->Release();
    brush->Release();
}

static float MeasureWidth(const std::wstring& text, float size,
                          DWRITE_FONT_WEIGHT weight, int family = 0,
                          bool tabular = false) {
    if (!g_dwriteFactory || text.empty()) {
        return 0;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight,
                                              DWRITE_TEXT_ALIGNMENT_LEADING,
                                              DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                                              false, family);
    if (!format) {
        return 0;
    }
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(
            text.c_str(), (UINT32)text.size(), format, 1000, 1000, &layout))) {
        return 0;
    }
    // A tabular measure must use the same typography as the drawing, or the
    // measured width drifts away from the rendered one (proportional digits
    // such as "1" are narrower than their tabular cell).
    if (tabular) {
        IDWriteTypography* typography = nullptr;
        if (SUCCEEDED(g_dwriteFactory->CreateTypography(&typography))) {
            DWRITE_FONT_FEATURE feature = {DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1};
            typography->AddFontFeature(feature);
            layout->SetTypography(typography, {0, (UINT32)text.size()});
            typography->Release();
        }
    }
    DWRITE_TEXT_METRICS metrics = {};
    layout->GetMetrics(&metrics);
    layout->Release();
    return metrics.width;
}

// Distance between the top of a single-line layout and its baseline. Used to
// align two texts of different sizes on the same baseline (value + unit)
// instead of aligning their box bottoms, which made the unit drift down under
// short glyphs such as the em dash placeholder.
static float BaselineOffset(float size, int family = 0) {
    if (!g_dwriteFactory) {
        return size * 0.9f;
    }
    IDWriteTextFormat* format = GetTextFormat(size, DWRITE_FONT_WEIGHT_NORMAL,
                                              DWRITE_TEXT_ALIGNMENT_LEADING,
                                              DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                                              false, family);
    if (!format) {
        return size * 0.9f;
    }
    float baseline = size * 0.9f;
    IDWriteTextLayout* layout = nullptr;
    if (SUCCEEDED(g_dwriteFactory->CreateTextLayout(L"0", 1, format, 1000, 1000,
                                                    &layout))) {
        DWRITE_LINE_METRICS metrics = {};
        UINT32 count = 0;
        if (SUCCEEDED(layout->GetLineMetrics(&metrics, 1, &count)) && count > 0) {
            baseline = metrics.baseline;
        }
        layout->Release();
    }
    return baseline;
}

// Fits a single line of text into maxWidth, appending an ellipsis only when the
// full string does not fit: SSIDs and adapter descriptions can be longer than
// the footline, and cutting them at a fixed character count would hide part of
// the name.
static std::wstring Ellipsize(const std::wstring& text, float maxWidth, float size,
                              DWRITE_FONT_WEIGHT weight, int family = 0) {
    if (text.empty() || MeasureWidth(text, size, weight, family) <= maxWidth) {
        return text;
    }
    std::wstring result = text;
    while (!result.empty() &&
           MeasureWidth(result + L"\x2026", size, weight, family) > maxWidth) {
        result.pop_back();
    }
    return result + L"\x2026";
}

static ID2D1PathGeometry* CreateArcPath(float cx, float cy, float radius,
                                        float startDegrees, float sweepDegrees) {
    if (!g_d2dFactory || sweepDegrees <= 0.1f) {
        return nullptr;
    }

    ID2D1PathGeometry* geometry = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry))) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geometry->Open(&sink))) {
        geometry->Release();
        return nullptr;
    }

    const float pi = 3.14159265358979323846f;
    auto point = [&](float angle) {
        float radians = angle * pi / 180.0f;
        return D2D1_POINT_2F{cx + radius * cosf(radians), cy + radius * sinf(radians)};
    };

    sink->BeginFigure(point(startDegrees), D2D1_FIGURE_BEGIN_HOLLOW);
    D2D1_ARC_SEGMENT arc = {};
    arc.point = point(startDegrees + sweepDegrees);
    arc.size = {radius, radius};
    arc.rotationAngle = 0;
    arc.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc.arcSize = sweepDegrees > 180.0f ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc);
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    sink->Release();
    return geometry;
}

static void DrawArc(float cx, float cy, float radius, float fraction,
                    D2D1_COLOR_F color, float width) {
    ID2D1PathGeometry* path = CreateArcPath(cx, cy, radius, 135.0f, 270.0f * fraction);
    if (!path) {
        return;
    }
    ID2D1SolidColorBrush* brush = Brush(color);
    if (brush) {
        g_renderTarget->DrawGeometry(path, brush, width, g_roundStroke);
        brush->Release();
    }
    path->Release();
}

// ---------------------------------------------------------------------------
// Chart
// ---------------------------------------------------------------------------

static ID2D1PathGeometry* BuildSeriesPath(const std::vector<double>& values,
                                          float left, float right, float axisMax,
                                          float baseline, float top, bool area) {
    if (values.size() < 2 || right - left < 2 || !g_d2dFactory) {
        return nullptr;
    }

    ID2D1PathGeometry* geometry = nullptr;
    if (FAILED(g_d2dFactory->CreatePathGeometry(&geometry))) {
        return nullptr;
    }
    ID2D1GeometrySink* sink = nullptr;
    if (FAILED(geometry->Open(&sink))) {
        geometry->Release();
        return nullptr;
    }

    float span = right - left;
    auto y = [&](double value) {
        double fraction = Clamp(value / axisMax, 0, 1);
        return baseline - (float)fraction * (baseline - top);
    };

    if (area) {
        sink->BeginFigure({left, baseline}, D2D1_FIGURE_BEGIN_FILLED);
        for (size_t i = 0; i < values.size(); i++) {
            float t = (float)i / (float)(values.size() - 1);
            sink->AddLine({left + span * t, y(values[i])});
        }
        sink->AddLine({right, baseline});
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    } else {
        sink->BeginFigure({left, y(values[0])}, D2D1_FIGURE_BEGIN_HOLLOW);
        for (size_t i = 1; i < values.size(); i++) {
            float t = (float)i / (float)(values.size() - 1);
            sink->AddLine({left + span * t, y(values[i])});
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
    }

    sink->Close();
    sink->Release();
    return geometry;
}

static void DrawChart(const std::vector<Sample>& samples,
                      D2D1_RECT_F area, float alpha) {
    if (samples.size() < 2) {
        return;
    }

    std::vector<double> down;
    std::vector<double> up;
    double downPeak = 0;
    double upPeak = 0;
    for (const auto& sample : samples) {
        if (sample.upload) {
            upPeak = std::max(upPeak, sample.value);
            up.push_back(sample.value);
        } else {
            downPeak = std::max(downPeak, sample.value);
            down.push_back(sample.value);
        }
    }
    // Both curves share one scale so the two halves can be compared at a
    // glance; with two independent axes a 200 Mbps upload looked as tall as a
    // 900 Mbps download.
    double axis = ChartAxisMax(std::max(std::max(downPeak, upPeak), 1.0));

    const float top = area.top + 17;
    const float baseline = area.bottom - 3;
    const float width = area.right - area.left;

    DrawLine(area.left, top, area.right, top,
             WithAlpha(g_palette.cardStroke, alpha * 2.0f));
    DrawLine(area.left, (top + baseline) / 2, area.right, (top + baseline) / 2,
             WithAlpha(g_palette.cardStroke, alpha * 2.0f));
    DrawLine(area.left, baseline, area.right, baseline,
             WithAlpha(g_palette.cardStroke, alpha * 2.4f));

    bool hasDown = down.size() >= 2;
    bool hasUp = up.size() >= 2;
    if (!hasDown && !hasUp) {
        return;
    }

    float downLeft = area.left;
    float downRight = area.right;
    float upLeft = area.left;
    float upRight = area.right;
    if (hasDown && hasUp) {
        // Download and upload have very different ranges. Each series keeps its
        // own half of the chart, but both use the shared `axis` above so their
        // heights are comparable.
        float zone = (width - 16) / 2;
        downRight = area.left + zone;
        upLeft = area.left + zone + 16;
    } else if (!hasUp) {
        upLeft = area.right;
    } else {
        downRight = area.left;
    }

    if (hasDown) {
        ID2D1PathGeometry* fill = BuildSeriesPath(down, downLeft, downRight, (float)axis,
                                                  baseline, top, true);
        if (fill) {
            ID2D1SolidColorBrush* brush = Brush(WithAlpha(g_palette.download, alpha * 0.10f));
            if (brush) {
                g_renderTarget->FillGeometry(fill, brush);
                brush->Release();
            }
            fill->Release();
        }
        ID2D1PathGeometry* line = BuildSeriesPath(down, downLeft, downRight, (float)axis,
                                                  baseline, top, false);
        if (line) {
            ID2D1SolidColorBrush* brush = Brush(WithAlpha(g_palette.download, alpha));
            if (brush) {
                g_renderTarget->DrawGeometry(line, brush, 2.0f, g_roundStroke);
                brush->Release();
            }
            line->Release();
        }
    }

    if (hasUp) {
        ID2D1PathGeometry* fill = BuildSeriesPath(up, upLeft, upRight, (float)axis,
                                                  baseline, top, true);
        if (fill) {
            ID2D1SolidColorBrush* brush = Brush(WithAlpha(g_palette.upload, alpha * 0.10f));
            if (brush) {
                g_renderTarget->FillGeometry(fill, brush);
                brush->Release();
            }
            fill->Release();
        }
        ID2D1PathGeometry* line = BuildSeriesPath(up, upLeft, upRight, (float)axis,
                                                  baseline, top, false);
        if (line) {
            ID2D1SolidColorBrush* brush = Brush(WithAlpha(g_palette.upload, alpha));
            if (brush) {
                g_renderTarget->DrawGeometry(line, brush, 2.0f, g_roundStroke);
                brush->Release();
            }
            line->Release();
        }
    }

    if (hasDown || hasUp) {
        Text(FormatScale(axis) + L" Mbps",
             Rect(area.left, area.top - 4, area.right, area.top + 12), 10,
             WithAlpha(g_palette.tertiary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
             DWRITE_TEXT_ALIGNMENT_LEADING);
    }
}

// ---------------------------------------------------------------------------
// Flyout UI state and drawing
// ---------------------------------------------------------------------------

enum class View {
    Gauge,
    Results,
    Error,
};

enum class Hot {
    None,
    Start,
    Copy,
    Close,
    Retry,
};

static View g_view = View::Gauge;
static View g_animFrom = View::Gauge;
static bool g_animating = false;
static ULONGLONG g_animStart = 0;
// Material state. DWM applies DWMWA_SYSTEMBACKDROP_TYPE asynchronously and
// there is no query for "the material is attached", so the first frames are
// painted with an opaque scrim that then fades out. The attribute is always
// set to NONE while the window is hidden, so showing it is a real state change
// and DWM is guaranteed to recompute the material.
static ULONGLONG g_scrimFadeAt = 0;

static constexpr float kScrimSettleMs = 60.0f;
static constexpr float kScrimFadeMs = 200.0f;

// Theme-specific tints over DWM's Desktop Acrylic. The light tint softens
// colorful backdrops; the stronger dark tint matches the shell's dark flyouts.
// Both preserve the material's blur and wallpaper response.
static constexpr float kLightTint = 0.60f;
static constexpr float kDarkTint = 0.42f;

static bool ScrimFading() {
    // The extra 50 ms forces one final frame at alpha 0 after the fade, so the
    // surface is never left with the last interpolated (non-zero) alpha.
    return g_scrimFadeAt &&
           Now() - g_scrimFadeAt <
               (ULONGLONG)(kScrimSettleMs + kScrimFadeMs + 50.0f);
}

static float ScrimAlpha() {
    if (!g_scrimFadeAt) {
        return 1.0f;
    }
    float elapsed = (float)(Now() - g_scrimFadeAt);
    if (elapsed <= kScrimSettleMs) {
        return 1.0f;
    }
    float t = (elapsed - kScrimSettleMs) / kScrimFadeMs;
    if (t >= 1.0f) {
        return 0.0f;
    }
    // Ease-out: the material is already there, the scrim just dissolves.
    float eased = 1.0f - (1.0f - t) * (1.0f - t);
    return 1.0f - eased;
}

static ULONGLONG g_copyUntil = 0;
static double g_gaugeShown = 0;
static Hot g_hot = Hot::None;
static Hot g_pressed = Hot::None;
static Hot g_focus = Hot::None;
static NetworkInfo g_network;
static bool g_uiVisible = false;
static HWND g_previousForeground = nullptr;
static UINT g_mouseButtonsDown = 0;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static volatile LONG g_shutdown = 0;

static UINT MouseButtonsDown() {
    constexpr int buttons[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1,
                               VK_XBUTTON2};
    UINT state = 0;
    for (size_t i = 0; i < ARRAYSIZE(buttons); i++) {
        if (GetAsyncKeyState(buttons[i]) & 0x8000) {
            state |= 1u << i;
        }
    }
    return state;
}

// Fluent motion is 167-250 ms; 250 ms keeps the transitions readable while
// staying in the range of the shell flyouts. Users who turn "Animation
// effects" off get an instant swap.
static constexpr float kAnimMs = 250.0f;
static constexpr D2D1_RECT_F kCloseRect = {352.0f, 9.0f, 384.0f, 39.0f};
static constexpr D2D1_RECT_F kCopyRect = {292.0f, 60.0f, 364.0f, 86.0f};
static constexpr D2D1_RECT_F kCardRect = {20.0f, 48.0f, 380.0f, 372.0f};
// Error layout, authored for the 340 px window: the card ends 44 px above the
// bottom so the footline keeps its usual place underneath.
static constexpr D2D1_RECT_F kErrorRect = {20.0f, 56.0f, 380.0f, 296.0f};
static constexpr D2D1_RECT_F kRetryRect = {144.0f, 230.0f, 256.0f, 262.0f};

// Height the window is laid out at right now. It animates between
// ViewHeight() values; every rect below the content top reads it.
static float g_layoutHeight = kGaugeHeight;
static float g_heightFrom = kGaugeHeight;

static float ViewHeight(View view) {
    return view == View::Error ? kErrorHeight : kGaugeHeight;
}

static D2D1_RECT_F FootRect() {
    return Rect(20.0f, g_layoutHeight - 32.0f, 384.0f, g_layoutHeight - 14.0f);
}

static bool AnimationsEnabled() {
    BOOL enabled = TRUE;
    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0)) {
        return true;
    }
    return enabled != FALSE;
}

// Keyboard focus is only drawn while the window really has it, so the ring
// does not linger after the flyout loses activation.
static bool FocusVisible() {
    return g_focus != Hot::None && g_flyoutHwnd && GetFocus() == g_flyoutHwnd;
}

static float AnimT() {
    return Clamp01((float)(Now() - g_animStart) / kAnimMs);
}

static float AnimOut() {
    return 1.0f - Clamp01(AnimT() / 0.38f);
}

static float AnimIn() {
    return Clamp01((AnimT() - 0.38f) / 0.62f);
}

static bool InRect(D2D1_RECT_F rect, float x, float y) {
    return x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom;
}

struct GaugeTick {
    PCWSTR label;
    double mbps;
    double fraction;
    float width;
};

static constexpr GaugeTick kGaugeTicks[] = {
    {L"0", 0, 0.0, 28},       {L"5", 5, 0.12, 28},
    {L"10", 10, 0.24, 32},   {L"50", 50, 0.36, 32},
    {L"100", 100, 0.50, 38}, {L"250", 250, 0.625, 38},
    {L"500", 500, 0.75, 38}, {L"750", 750, 0.88, 38},
    {L"1000+", 1000, 1.0, 48},
};

static double GaugeFraction(double mbps) {
    constexpr double kGaugeMaxMbps = 1000.0;
    double value = Clamp(mbps, 0.0, kGaugeMaxMbps);
    if (!std::isfinite(value)) {
        return 0;
    }
    if (value <= 0) {
        return 0;
    }

    for (size_t i = 1; i < ARRAYSIZE(kGaugeTicks); i++) {
        const GaugeTick& lower = kGaugeTicks[i - 1];
        const GaugeTick& upper = kGaugeTicks[i];
        if (value <= upper.mbps) {
            double progress = lower.mbps == 0
                                  ? std::log1p(value) / std::log1p(upper.mbps)
                                  : std::log(value / lower.mbps) /
                                        std::log(upper.mbps / lower.mbps);
            return lower.fraction +
                   (upper.fraction - lower.fraction) * progress;
        }
    }
    return 1.0;
}

static PCWSTR StartButtonLabel(const TestState& state) {
    if (StateRunning(state)) {
        return g_str->stop;
    }
    if (state.phase == Phase::Done || state.phase == Phase::Canceled ||
        g_view == View::Results) {
        return g_str->repeat;
    }
    return g_str->start;
}

static bool IsRepeatButton(const TestState& state) {
    return !StateRunning(state) &&
           (state.phase == Phase::Done || state.phase == Phase::Canceled ||
            g_view == View::Results);
}

static D2D1_RECT_F StartButtonRect(const TestState& state) {
    PCWSTR label = StartButtonLabel(state);
    DWRITE_FONT_WEIGHT weight = StateRunning(state)
                                   ? DWRITE_FONT_WEIGHT_NORMAL
                                   : DWRITE_FONT_WEIGHT_SEMI_BOLD;
    float width = MeasureWidth(label, 13, weight) + 32.0f;
    if (IsRepeatButton(state)) {
        width += 22.0f;
    }
    width = std::max(width, 76.0f);
    float left = (kWidth - width) / 2.0f;
    return Rect(left, 382.0f, left + width, 416.0f);
}

static Hot HitTest(float x, float y) {
    if (InRect(kCloseRect, x, y)) {
        return Hot::Close;
    }
    if (g_view == View::Error) {
        return InRect(kRetryRect, x, y) ? Hot::Retry : Hot::None;
    }
    if (g_view == View::Results && InRect(kCopyRect, x, y)) {
        return Hot::Copy;
    }
    if (InRect(StartButtonRect(CopyState()), x, y)) {
        return Hot::Start;
    }
    return Hot::None;
}

static void DrawGauge(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }

    const float cx = kWidth / 2;
    const float cy = 162.0f;
    const float radius = 96.0f;
    const float stroke = 13.0f;

    DrawArc(cx, cy, radius, 1.0f, WithAlpha(g_palette.track, alpha), stroke);
    if (g_gaugeShown > 0.003) {
        D2D1_COLOR_F color =
            state.phase == Phase::Upload ? g_palette.upload : g_palette.download;
        DrawArc(cx, cy, radius, (float)g_gaugeShown, WithAlpha(color, alpha), stroke);
    }

    std::wstring value = L"\x2014";
    std::wstring unit = L"Mbps";
    if (state.phase != Phase::Latency && state.phase != Phase::Warmup &&
        state.phase != Phase::Stopping && state.phase != Phase::Canceled &&
        state.phase != Phase::Idle) {
        value = FormatNumber(state.currentMbps);
    }

    Text(value, Rect(20, 124, 380, 178), 44, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 1, false, true);
    Text(unit, Rect(20, 178, 380, 198), 15, WithAlpha(g_palette.secondary, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER);
    Text(state.caption, Rect(20, 202, 380, 220), 13,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_CENTER);

    // Tick labels follow the same logarithmic scale as the arc.
    const float labelRadius = radius + 22.0f;
    for (const GaugeTick& tick : kGaugeTicks) {
        double angle = (135.0 + 270.0 * tick.fraction) *
                       3.14159265358979323846 / 180.0;
        float x = cx + labelRadius * (float)std::cos(angle);
        float y = cy + labelRadius * (float)std::sin(angle);
        Text(tick.label, Rect(x - tick.width / 2, y - 9,
                              x + tick.width / 2, y + 9),
             11, WithAlpha(g_palette.tertiary, alpha),
             DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
             DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

static void DrawMetricLabel(float x, float y, PCWSTR glyph, D2D1_COLOR_F color,
                            PCWSTR label, float alpha) {
    Text(glyph, Rect(x, y - 1, x + 18, y + 17), 13, WithAlpha(color, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(label, Rect(x + 19, y, x + 160, y + 17), 12,
         WithAlpha(g_palette.secondary, alpha));
}

static void DrawValue(float x, float y, bool hasValue, double value, PCWSTR unit,
                      float fontSize, float alpha) {
    std::wstring text = hasValue ? FormatNumber(value) : L"\x2014";
    Text(text, Rect(x, y, x + 170, y + fontSize * 1.35f), fontSize,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_SEMI_BOLD,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, 1, false, true);
    // Tabular measure: the drawn value uses tabular figures, so the unit must
    // be placed from the same width or it can touch the last digit.
    float width = MeasureWidth(text, fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD, 1, true);
    // The unit rides the value's baseline: box-bottom alignment drags the unit
    // down under glyphs without descenders, such as the em dash placeholder.
    float unitTop = y + BaselineOffset(fontSize, 1) - BaselineOffset(12, 0);
    Text(unit, Rect(x + width + 5, unitTop, x + width + 80, unitTop + 18), 12,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

static void DrawLatency(float x, float y, PCWSTR label, bool hasValue, double value,
                        float alpha) {
    Text(label, Rect(x, y + 3, x + 60, y + 20), 12,
         WithAlpha(g_palette.secondary, alpha));
    std::wstring text = hasValue ? FormatNumber(value) : L"\x2014";
    Text(text, Rect(x + 42, y, x + 120, y + 20), 13,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_MEDIUM,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, 0, false, true);
    float width = MeasureWidth(text, 13, DWRITE_FONT_WEIGHT_MEDIUM, 0, true);
    float unitTop = y + BaselineOffset(13, 0) - BaselineOffset(11, 0);
    Text(L"ms", Rect(x + 42 + width + 4, unitTop, x + 150, unitTop + 17), 11,
         WithAlpha(g_palette.secondary, alpha));
}

static void DrawLiveStats(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }

    DrawLine(20, 282, 380, 282, WithAlpha(g_palette.cardStroke, alpha * 1.6f));

    bool hasDown = state.phase == Phase::Download || state.phase == Phase::Upload ||
                   state.phase == Phase::Done;
    bool hasUp = state.phase == Phase::Upload || state.phase == Phase::Done;

    double down = state.phase == Phase::Download ? state.currentMbps : state.download;
    double up = state.phase == Phase::Upload ? state.currentMbps : state.upload;

    DrawMetricLabel(20, 288, L"\xE896", g_palette.download, g_str->download, alpha);
    DrawMetricLabel(200, 288, L"\xE898", g_palette.upload, g_str->upload, alpha);
    DrawValue(20, 304, hasDown, down, L"Mbps", 32, alpha);
    DrawValue(200, 304, hasUp, up, L"Mbps", 32, alpha);

    DrawLatency(20, 352, g_str->ping, state.hasPing, state.ping, alpha);
    DrawLatency(200, 352, g_str->jitter, state.hasJitter, state.jitter, alpha);
}

static void DrawCopyButton(float alpha) {
    bool copied = Now() < g_copyUntil;
    bool hover = g_hot == Hot::Copy;
    bool pressed = g_pressed == Hot::Copy;
    D2D1_RECT_F rect = kCopyRect;

    FillRounded(rect, 4,
                WithAlpha(g_palette.controlFill, alpha * (pressed ? 2.4f : (hover ? 1.8f : 1.0f))));
    StrokeRounded(rect, 4, WithAlpha(g_palette.cardStroke, alpha * 1.8f));

    PCWSTR glyph = copied ? L"\xE73E" : L"\xE8C8";
    PCWSTR label = copied ? g_str->copied : g_str->copy;
    Text(glyph, Rect(rect.left + 9, rect.top, rect.left + 24, rect.bottom), 12,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(label, Rect(rect.left + 26, rect.top, rect.right - 4, rect.bottom), 12,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (g_focus == Hot::Copy && FocusVisible()) {
        DrawFocusRing(rect, 4, alpha);
    }
}

static void DrawResults(float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }

    FillRounded(kCardRect, 8, WithAlpha(g_palette.cardFill, alpha));
    StrokeRounded(kCardRect, 8, WithAlpha(g_palette.cardStroke, alpha));

    Text(g_str->resultsTitle, Rect(36, 62, 260, 82), 13,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_SEMI_BOLD);
    DrawCopyButton(alpha);

    DrawChart(state.samples, Rect(36, 94, 364, 198), alpha);

    // Caption row: server and time on the left, data used on the right.
    std::wstring caption = std::wstring(g_str->server) + L" " + state.server + L" \x00B7 " + FormatClock(state.when);
    Text(caption, Rect(36, 214, 248, 230), 11, WithAlpha(g_palette.secondary, alpha));

    wchar_t dataText[64];
    swprintf(dataText, 64, L"%s: %.0f MB",
             g_str->dataUsed, state.dataUsed / 1'000'000.0);
    Text(std::wstring(dataText), Rect(248, 214, 364, 230), 11,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_TRAILING);

    DrawMetricLabel(36, 236, L"\xE896", g_palette.download, g_str->download, alpha);
    DrawMetricLabel(216, 236, L"\xE898", g_palette.upload, g_str->upload, alpha);
    DrawValue(36, 252, true, state.download, L"Mbps", 30, alpha);
    DrawValue(216, 252, true, state.upload, L"Mbps", 30, alpha);

    DrawLatency(36, 310, g_str->ping, state.hasPing, state.ping, alpha);
    DrawLatency(216, 310, g_str->jitter, state.hasJitter, state.jitter, alpha);
}

static void DrawError(float alpha, const TestState& state) {
    FillRounded(kErrorRect, 8, WithAlpha(g_palette.cardFill, alpha));
    StrokeRounded(kErrorRect, 8, WithAlpha(g_palette.cardStroke, alpha));

    Text(L"\xEA39", Rect(20, 74, 380, 106), 26, WithAlpha(g_palette.negative, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    Text(g_str->failedTitle, Rect(20, 112, 380, 134), 15,
         WithAlpha(g_palette.text, alpha), DWRITE_FONT_WEIGHT_SEMI_BOLD,
         DWRITE_TEXT_ALIGNMENT_CENTER);
    Text(state.error.empty() ? g_str->failedHint : state.error,
         Rect(52, 142, 348, 218), 12, WithAlpha(g_palette.secondary, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 0, true);

    bool hover = g_hot == Hot::Retry;
    bool pressed = g_pressed == Hot::Retry;
    FillRounded(kRetryRect, 4,
                WithAlpha(pressed ? Shade(g_palette.accent, 0.18f)
                                  : Lighten(g_palette.accent, hover ? 0.18f : 0.0f),
                          alpha));
    Text(g_str->retry, kRetryRect, 13, WithAlpha(g_palette.accentText, alpha),
         DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (g_focus == Hot::Retry && FocusVisible()) {
        DrawFocusRing(kRetryRect, 4, alpha);
    }
}

static void DrawButton(float alpha, const TestState& state) {
    bool running = StateRunning(state);
    PCWSTR label = StartButtonLabel(state);
    bool repeat = IsRepeatButton(state);
    bool hover = g_hot == Hot::Start;
    bool pressed = g_pressed == Hot::Start;

    D2D1_RECT_F rect = StartButtonRect(state);
    DWRITE_FONT_WEIGHT weight = running ? DWRITE_FONT_WEIGHT_NORMAL
                                        : DWRITE_FONT_WEIGHT_SEMI_BOLD;
    if (running) {
        float fill = pressed ? 3.0f : (hover ? 2.2f : 1.2f);
        FillRounded(rect, 4, WithAlpha(g_palette.controlFill, alpha * fill));
        StrokeRounded(rect, 4, WithAlpha(g_palette.cardStroke, alpha * 2.0f));
    } else {
        D2D1_COLOR_F fill = pressed ? Shade(g_palette.accent, 0.18f)
                                    : Lighten(g_palette.accent, hover ? 0.18f : 0.0f);
        FillRounded(rect, 4, WithAlpha(fill, alpha));
    }
    D2D1_COLOR_F textColor =
        running ? g_palette.text : g_palette.accentText;
    if (repeat) {
        float labelWidth = MeasureWidth(label, 13, weight);
        float contentWidth = 16.0f + 6.0f + labelWidth;
        float contentLeft = (rect.left + rect.right - contentWidth) / 2.0f;
        Text(L"\xE72C", Rect(contentLeft, rect.top, contentLeft + 16.0f, rect.bottom),
             14, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
        Text(label,
             Rect(contentLeft + 22.0f, rect.top,
                  contentLeft + contentWidth, rect.bottom),
             13, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    } else {
        Text(label, rect, 13, WithAlpha(textColor, alpha), weight,
             DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    if (g_focus == Hot::Start && FocusVisible()) {
        DrawFocusRing(rect, 4, alpha);
    }
}

static void DrawTitle(float alpha) {
    Text(L"Speedtest", Rect(16, 15, 200, 33), 12,
         WithAlpha(g_palette.text, alpha * 0.85f));

    bool hover = g_hot == Hot::Close;
    bool pressed = g_pressed == Hot::Close;
    if (hover || pressed) {
        FillRounded(kCloseRect, 4,
                    WithAlpha(g_palette.controlFill, alpha * (pressed ? 2.4f : 1.6f)));
    }
    Text(L"\xE711", kCloseRect, 11, WithAlpha(g_palette.text, alpha),
         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    if (g_focus == Hot::Close && FocusVisible()) {
        DrawFocusRing(kCloseRect, 4, alpha);
    }
}

static void DrawNetwork(float alpha) {
    D2D1_RECT_F row = FootRect();
    Text(g_network.glyph, Rect(row.left, row.top, row.left + 16, row.bottom), 11,
         WithAlpha(g_palette.secondary, alpha), DWRITE_FONT_WEIGHT_NORMAL,
         DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, 2);
    if (!g_network.name.empty()) {
        std::wstring name = Ellipsize(g_network.name, row.right - row.left - 17.0f, 11,
                                      DWRITE_FONT_WEIGHT_NORMAL);
        Text(name, Rect(row.left + 17, row.top, row.right, row.bottom), 11,
             WithAlpha(g_palette.secondary, alpha));
    }
}

static void DrawView(View view, float alpha, const TestState& state) {
    if (alpha <= 0.001f) {
        return;
    }
    switch (view) {
        case View::Gauge:
            DrawGauge(alpha, state);
            DrawLiveStats(alpha, state);
            break;
        case View::Results:
            DrawResults(alpha, state);
            break;
        case View::Error:
            DrawError(alpha, state);
            break;
    }
}

static void DrawFrame(const TestState& state) {
    if (!g_renderTarget) {
        return;
    }

    // The material itself is drawn by DWM behind the composition surface: the
    // surface stays transparent so the Desktop Acrylic tint and blur are the
    // only background. Painting the palette colour on top as well would stack
    // two tints, flatten the blur and make the flyout look like flat grey glass
    // instead of the shell material.
    bool translucent = g_composition && g_translucent;
    D2D1_COLOR_F background = g_palette.background;
    float scrim = translucent ? ScrimAlpha() : 1.0f;
    background.a = scrim;
    g_renderTarget->Clear(background);

    // DWM's Desktop Acrylic is much lighter than the material the shell paints
    // in its dark flyouts. A black tint restores that tone; a lighter white
    // tint in light mode reduces backdrop show-through without removing blur.
    if (translucent && scrim < 1.0f) {
        float tintAlpha =
            (g_dark ? kDarkTint : kLightTint) * (1.0f - scrim);
        D2D1_COLOR_F tint =
            g_dark ? Col(0x000000, tintAlpha) : Col(0xFFFFFF, tintAlpha);
        FillRect(Rect(-2.0f, -2.0f, kWidth + 2.0f, g_layoutHeight + 2.0f), tint);

        // The stronger dark tint flattens DWM's grain, so restore a faint
        // texture there to keep banding from showing in the blur.
        if (g_dark && EnsureNoise()) {
            g_noiseBrush->SetOpacity(kNoiseOpacity * (1.0f - scrim));
            g_renderTarget->FillRectangle(
                Rect(-2.0f, -2.0f, kWidth + 2.0f, g_layoutHeight + 2.0f),
                g_noiseBrush);
        }
    }

    if (g_animating) {
        DrawView(g_animFrom, AnimOut(), state);
        DrawView(g_view, AnimIn(), state);
    } else {
        DrawView(g_view, 1.0f, state);
    }

    float buttonAlpha = g_animating ? AnimIn() : 1.0f;
    if (g_view != View::Error || (g_animating && g_animFrom != View::Error)) {
        DrawButton(buttonAlpha, state);
    }

    DrawNetwork(1.0f);
    DrawTitle(1.0f);
}

// ---------------------------------------------------------------------------
// Direct2D lifetime
// ---------------------------------------------------------------------------

static void ReleaseTargets() {
    // Brushes belong to the render target: drop them before the target goes.
    ReleaseBrushes();
    ReleaseNoise();
    if (g_contextBitmap) {
        g_contextBitmap->Release();
        g_contextBitmap = nullptr;
    }
    if (g_context) {
        g_context->SetTarget(nullptr);
        g_context->Release();
        g_context = nullptr;
    }
    if (g_renderTarget) {
        g_renderTarget->Release();
        g_renderTarget = nullptr;
    }
    if (g_hwndTarget) {
        g_hwndTarget->Release();
        g_hwndTarget = nullptr;
    }
    if (g_dcompVisual) {
        g_dcompVisual->Release();
        g_dcompVisual = nullptr;
    }
    if (g_dcompTarget) {
        g_dcompTarget->Release();
        g_dcompTarget = nullptr;
    }
    if (g_dcompDevice) {
        g_dcompDevice->Release();
        g_dcompDevice = nullptr;
    }
    if (g_swapChain) {
        g_swapChain->Release();
        g_swapChain = nullptr;
    }
    if (g_dxgiDevice) {
        g_dxgiDevice->Release();
        g_dxgiDevice = nullptr;
    }
    if (g_d3dDevice) {
        g_d3dDevice->Release();
        g_d3dDevice = nullptr;
    }
    g_composition = false;
    g_targetWidth = 0;
    g_targetHeight = 0;
    g_targetDpi = 0;
}

static void ReleaseGraphics() {
    ReleaseTargets();
    if (g_roundStroke) {
        g_roundStroke->Release();
        g_roundStroke = nullptr;
    }
    for (auto& entry : g_textFormats) {
        entry.second->Release();
    }
    g_textFormats.clear();
    if (g_dwriteFactory) {
        g_dwriteFactory->Release();
        g_dwriteFactory = nullptr;
    }
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
}

static bool BindCompositionTarget() {
    if (!g_context || !g_swapChain) {
        return false;
    }
    if (g_contextBitmap) {
        g_contextBitmap->Release();
        g_contextBitmap = nullptr;
    }

    IDXGISurface* surface = nullptr;
    if (FAILED(g_swapChain->GetBuffer(0, __uuidof(IDXGISurface),
                                      (void**)&surface))) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 properties = {};
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.dpiX = g_dpi;
    properties.dpiY = g_dpi;
    properties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    HRESULT hr = g_context->CreateBitmapFromDxgiSurface(surface, &properties,
                                                        &g_contextBitmap);
    surface->Release();
    if (FAILED(hr)) {
        return false;
    }

    g_context->SetTarget(g_contextBitmap);
    g_context->SetDpi(g_dpi, g_dpi);
    g_context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    g_context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_targetDpi = g_dpi;
    return true;
}

// Builds a DirectComposition swapchain with an alpha channel. The flyout keeps
// painting the same way, but the background can now be fully transparent so
// the DWM backdrop (Desktop Acrylic) is the only background.
static bool CreateCompositionTarget(HWND hwnd, UINT width, UINT height) {
    D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0,
                                  D3D_FEATURE_LEVEL_10_1};
    // Hardware first; retry with WARP so VMs, RDP sessions and machines
    // without usable graphics still get the composition surface. Without this
    // fallback those sessions would fall through to the (in this window
    // shape invisible) HwndRenderTarget path below.
    HRESULT d3dHr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 2,
                                   D3D11_SDK_VERSION, &g_d3dDevice, nullptr,
                                   nullptr);
    if (FAILED(d3dHr)) {
        d3dHr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, levels, 2,
                               D3D11_SDK_VERSION, &g_d3dDevice, nullptr,
                               nullptr);
    }
    if (FAILED(d3dHr)) {
        return false;
    }
    if (FAILED(g_d3dDevice->QueryInterface(__uuidof(IDXGIDevice),
                                           (void**)&g_dxgiDevice))) {
        return false;
    }

    ID2D1Device* d2dDevice = nullptr;
    ID2D1Factory1* factory1 = nullptr;
    if (FAILED(g_d2dFactory->QueryInterface(__uuidof(ID2D1Factory1),
                                            (void**)&factory1))) {
        return false;
    }
    // The device must come from the same factory that created the stroke style
    // and path geometries, otherwise every draw call fails with WRONG_FACTORY.
    HRESULT hr = factory1->CreateDevice(g_dxgiDevice, &d2dDevice);
    factory1->Release();
    if (FAILED(hr)) {
        return false;
    }
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                        &g_context);
    d2dDevice->Release();
    if (FAILED(hr)) {
        return false;
    }

    IDXGIAdapter* adapter = nullptr;
    if (FAILED(g_dxgiDevice->GetAdapter(&adapter))) {
        return false;
    }
    IDXGIFactory2* factory = nullptr;
    hr = adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory);
    adapter->Release();
    if (FAILED(hr)) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 description = {};
    description.Width = width;
    description.Height = height;
    description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.SampleDesc.Count = 1;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = 2;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    hr = factory->CreateSwapChainForComposition(g_d3dDevice, &description,
                                                nullptr, &g_swapChain);
    factory->Release();
    if (FAILED(hr)) {
        return false;
    }

    if (FAILED(DCompositionCreateDevice(g_dxgiDevice,
                                        __uuidof(IDCompositionDevice),
                                        (void**)&g_dcompDevice))) {
        return false;
    }
    if (FAILED(g_dcompDevice->CreateTargetForHwnd(hwnd, TRUE, &g_dcompTarget)) ||
        FAILED(g_dcompDevice->CreateVisual(&g_dcompVisual))) {
        return false;
    }
    if (FAILED(g_dcompVisual->SetContent(g_swapChain)) ||
        FAILED(g_dcompTarget->SetRoot(g_dcompVisual)) ||
        FAILED(g_dcompDevice->Commit())) {
        return false;
    }

    g_composition = true;
    g_targetWidth = width;
    g_targetHeight = height;
    if (!BindCompositionTarget()) {
        return false;
    }

    // g_renderTarget is a separate reference so the painting code keeps using
    // one interface whatever path produced it.
    return SUCCEEDED(g_context->QueryInterface(__uuidof(ID2D1RenderTarget),
                                               (void**)&g_renderTarget));
}

static bool EnsureRenderTarget() {
    if (g_renderTarget) {
        return true;
    }
    if (!g_d2dFactory || !g_flyoutHwnd) {
        return false;
    }

    RECT client = {};
    GetClientRect(g_flyoutHwnd, &client);
    UINT width = (UINT)std::max<LONG>(1, client.right - client.left);
    UINT height = (UINT)std::max<LONG>(1, client.bottom - client.top);

    // The material needs per-pixel alpha, so the composition surface is the
    // default path; the HWND target stays as a fallback if D3D is unavailable.
    if (CreateCompositionTarget(g_flyoutHwnd, width, height)) {
        return true;
    }
    ReleaseTargets();

    D2D1_RENDER_TARGET_PROPERTIES properties = {};
    properties.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    properties.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    properties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
    properties.dpiX = g_dpi;
    properties.dpiY = g_dpi;

    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProperties = {};
    hwndProperties.hwnd = g_flyoutHwnd;
    hwndProperties.pixelSize = {width, height};
    hwndProperties.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

    if (FAILED(g_d2dFactory->CreateHwndRenderTarget(properties, hwndProperties,
                                                    &g_hwndTarget))) {
        g_hwndTarget = nullptr;
        Wh_Log(L"Unable to create a render target");
        return false;
    }
    if (FAILED(g_hwndTarget->QueryInterface(__uuidof(ID2D1RenderTarget),
                                            (void**)&g_renderTarget))) {
        return false;
    }
    g_targetWidth = width;
    g_targetHeight = height;
    return true;
}

static void ResizeRenderTarget() {
    if (!g_renderTarget || !g_flyoutHwnd) {
        return;
    }
    RECT client = {};
    GetClientRect(g_flyoutHwnd, &client);
    UINT width = (UINT)std::max<LONG>(1, client.right - client.left);
    UINT height = (UINT)std::max<LONG>(1, client.bottom - client.top);

    if (g_composition && g_context && g_swapChain) {
        if (width == g_targetWidth && height == g_targetHeight &&
            g_dpi == g_targetDpi) {
            return;
        }
        g_context->SetTarget(nullptr);
        if (g_contextBitmap) {
            g_contextBitmap->Release();
            g_contextBitmap = nullptr;
        }
        if (SUCCEEDED(g_swapChain->ResizeBuffers(0, width, height,
                                                 DXGI_FORMAT_UNKNOWN, 0)) &&
            BindCompositionTarget()) {
            g_targetWidth = width;
            g_targetHeight = height;
            return;
        }
        // The swapchain is gone: fall back to the HWND path on the next paint.
        ReleaseTargets();
        return;
    }

    if (g_hwndTarget) {
        g_hwndTarget->Resize({width, height});
        g_hwndTarget->SetDpi(g_dpi, g_dpi);
        g_targetWidth = width;
        g_targetHeight = height;
        g_targetDpi = g_dpi;
    }
}

// ---------------------------------------------------------------------------
// Actions
// ---------------------------------------------------------------------------

static void BeginView(View target, bool animate = true) {
    if (target == g_view && !g_animating) {
        return;
    }
    View previous = g_animating ? g_animFrom : g_view;
    g_animFrom = previous;
    g_view = target;
    g_heightFrom = g_layoutHeight;
    g_animStart = Now();
    g_animating = animate && previous != target && AnimationsEnabled();
    if (g_view != previous) {
        g_focus = Hot::None;
        if (g_flyoutHwnd) {
            NotifyWinEvent(EVENT_OBJECT_REORDER, g_flyoutHwnd, OBJID_CLIENT,
                           CHILDID_SELF);
        }
    }
}

static void CopyResult(HWND hwnd, const TestState& state) {
    if (!state.hasResult) {
        return;
    }
    std::wstring text =
        L"Speedtest \x00B7 " + FormatNumber(state.download) + L" Mbps download \x00B7 " +
        FormatNumber(state.upload) + L" Mbps upload \x00B7 " +
        (state.hasPing ? FormatNumber(state.ping) : L"\x2014") + L" ms ping \x00B7 " +
        (state.hasJitter ? FormatNumber(state.jitter) : L"\x2014") + L" ms jitter \x00B7 " +
        std::to_wstring((state.dataUsed + 500000) / 1000000) + L" MB data used";
    text += L"\r\n";
    text += state.server + L" \x00B7 " + FormatDate(state.when);

    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) {
        Wh_Log(L"Clipboard allocation failed (error %u)", GetLastError());
        return;
    }

    void* target = GlobalLock(memory);
    if (!target) {
        Wh_Log(L"Clipboard memory lock failed (error %u)", GetLastError());
        GlobalFree(memory);
        return;
    }
    memcpy(target, text.c_str(), bytes);
    GlobalUnlock(memory);

    bool copied = false;
    DWORD error = ERROR_SUCCESS;
    for (int attempt = 0; attempt < 5; attempt++) {
        if (OpenClipboard(hwnd)) {
            if (EmptyClipboard()) {
                if (SetClipboardData(CF_UNICODETEXT, memory)) {
                    memory = nullptr;
                    copied = true;
                } else {
                    error = GetLastError();
                }
            } else {
                error = GetLastError();
            }
            CloseClipboard();
            break;
        }
        error = GetLastError();
        Sleep(20);
    }
    if (memory) {
        GlobalFree(memory);
    }
    if (copied) {
        g_copyUntil = Now() + 1600;
    } else {
        Wh_Log(L"Copying test results to the clipboard failed (error %u)",
               error);
    }
    InvalidateRect(hwnd, nullptr, FALSE);
}

static void StartOrCancelTest(HWND hwnd) {
    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        return;
    }

    TestState state = CopyState();
    if (StateRunning(state)) {
        RequestTestCancel();
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    if (g_testThread) {
        // The previous worker may still be unwinding (it already set
        // Canceled/Done/Error but has not returned yet). Wait briefly for it
        // so the handle is not lost and Wh_ModUninit can join it; at that
        // point the thread only unwinds, so the wait is short. Keep the
        // bounded wait small: this runs on the UI thread.
        DWORD wait = WaitForSingleObject(g_testThread, 2000);
        if (wait != WAIT_OBJECT_0) {
            Wh_Log(L"Previous test thread wait result %lu (error %u)", wait,
                   GetLastError());
        }
        CloseHandle(g_testThread);
        g_testThread = nullptr;
    }

    StateReset();
    StateSetCaption(g_str->starting);
    // Mark the run as active before the worker starts, so a focus change in the
    // first few hundred milliseconds cannot dismiss the flyout mid-test.
    StateSetPhase(Phase::Warmup, g_str->starting);
    g_cancelRequested.store(0, std::memory_order_relaxed);
    g_copyUntil = 0;
    g_gaugeShown = 0;
    g_network = ReadNetwork();
    BeginView(View::Gauge);

    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        g_cancelRequested.store(1, std::memory_order_relaxed);
        StateCanceled();
        return;
    }

    g_testThread = CreateThread(nullptr, 0, TestWorkerThread, nullptr, 0, nullptr);
    if (!g_testThread) {
        DWORD error = GetLastError();
        Wh_Log(L"Speed test worker creation failed (error %u)", error);
        StateFail(g_str->errUnexpected);
        SetTimer(hwnd, 1, 16, nullptr);
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }
    SetTimer(hwnd, 1, 16, nullptr);
    InvalidateRect(hwnd, nullptr, FALSE);
}

// ---------------------------------------------------------------------------
// Show / hide / paint
// ---------------------------------------------------------------------------

// DWM backdrop. Windows 11 flyouts use Desktop Acrylic: DWM blurs whatever is
// behind the window and the premultiplied swap chain lets it show through.
// When the user turns transparency effects off, the plain solid surface is
// used instead (Windows does the same in its own apps).
//
// The frame is extended once at creation time (see UiThreadProc): on a
// captionless popup with zero margins DWM keeps the solid fallback. Runtime
// calls only flip the attribute, which must *change* for DWM to recompute the
// material -- that is why it is always set to NONE while hidden.
static void ApplyBackdrop(HWND hwnd, bool enable) {
    BOOL dark = g_dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark,
                          sizeof(dark));

    int backdrop = enable ? 3 /* DWMSBT_TRANSIENTWINDOW */
                          : 1 /* DWMSBT_NONE */;
    DwmSetWindowAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop,
                          sizeof(backdrop));
}

// Re-reads theme and transparency and re-applies the material. DWM ignores a
// write with the same value, so a state flip is applied as NONE -> ON.
static void RefreshBackdrop(HWND hwnd) {
    bool wasTranslucent = g_translucent;
    RefreshAppearance();
    if (g_translucent == wasTranslucent) {
        ApplyBackdrop(hwnd, g_translucent);
        return;
    }
    ApplyBackdrop(hwnd, false);
    if (g_translucent) {
        ApplyBackdrop(hwnd, true);
        g_scrimFadeAt = Now();
    } else {
        g_scrimFadeAt = 0;
    }
    InvalidateRect(hwnd, nullptr, FALSE);
}

static HMONITOR MonitorUnderCursor() {
    POINT cursor = {};
    GetCursorPos(&cursor);
    return MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
}

static void PositionFlyout(HWND hwnd, HMONITOR monitor) {
    MONITORINFO info = {sizeof(info)};
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return;
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    g_dpi = (float)(dpiX ? dpiX : 96);
    g_scale = g_dpi / 96.0f;

    int width = (int)std::lround(kWidth * g_scale);
    int height = (int)std::lround(g_layoutHeight * g_scale);
    int margin = (int)std::lround(12 * g_scale);
    int x = info.rcWork.right - width - margin;
    int y = info.rcWork.bottom - height - margin;

    // Position and size only: visibility is controlled by ShowFlyoutWindow so
    // the first frame can be presented before the window becomes visible.
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE);
    ResizeRenderTarget();
}

// True when Windows reports the current connection as metered (Fixed or
// Variable cost). Windows knows this from the user's "metered connection"
// setting or a cellular profile; unknown costs and query failures count as
// unmetered so a detection problem never blocks the auto path.
static bool IsConnectionMetered() {
    INetworkCostManager* costManager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&costManager));
    if (FAILED(hr)) {
        Wh_Log(L"NetworkCostManager unavailable (hr 0x%08X)", (unsigned)hr);
        return false;
    }
    DWORD cost = NLM_CONNECTION_COST_UNKNOWN;
    hr = costManager->GetCost(&cost, nullptr);
    costManager->Release();
    if (FAILED(hr)) {
        Wh_Log(L"GetCost failed (hr 0x%08X)", (unsigned)hr);
        return false;
    }
    bool metered = (cost & (NLM_CONNECTION_COST_FIXED |
                            NLM_CONNECTION_COST_VARIABLE |
                            NLM_CONNECTION_COST_ROAMING)) != 0;
    return metered;
}

static void ShowFlyoutWindow(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    bool startTest = !TestRunning();
    if (startTest) {
        StateReset();
        g_gaugeShown = 0;
        g_view = View::Gauge;
        g_animating = false;
        g_copyUntil = 0;
        g_hot = Hot::None;
        g_pressed = Hot::None;
        g_focus = Hot::None;
        g_network = ReadNetwork();

        // Opening the flyout is the request to measure: no need to press
        // Start. The preference and the metered-connection flag gate it: with
        // "auto" a metered connection (Fixed/Variable cost in Windows) opens
        // the flyout idle so the user starts the transfer consciously, and
        // "never" always does. The idle view already carries the Start button.
        int startMode = g_autoStart.load(std::memory_order_relaxed);
        if (startMode == 0 && IsConnectionMetered()) {
            startMode = 2;
        }
        if (startMode == 2) {
            StateSetCaption(g_str->ready);
            g_focus = Hot::Start;
            startTest = false;
        }
    }
    g_layoutHeight = ViewHeight(g_view);
    g_heightFrom = g_layoutHeight;

    RefreshAppearance();

    // Start from the solid surface and present one fully opaque frame before
    // the window becomes visible. The material is turned on only afterwards:
    // before that DWM does not attach it.
    ApplyBackdrop(hwnd, false);
    g_scrimFadeAt = 0;

    PositionFlyout(hwnd, MonitorUnderCursor());
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd);

    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_SHOWWINDOW);
    g_previousForeground = GetForegroundWindow();
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    g_uiVisible = true;
    g_mouseButtonsDown = MouseButtonsDown();
    SetTimer(hwnd, 1, 16, nullptr);
    InvalidateRect(hwnd, nullptr, FALSE);
    UpdateWindow(hwnd);

    // Now that the window is visible, setting the attribute is a real change.
    // The scrim stays up for the first frames and then fades to transparent.
    if (g_translucent) {
        ApplyBackdrop(hwnd, true);
        g_scrimFadeAt = Now();
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    if (startTest) {
        StartOrCancelTest(hwnd);
    }
}

static void HideFlyoutWindow(HWND hwnd, bool restoreForeground = true) {
    if (!hwnd) {
        return;
    }
    ShowWindow(hwnd, SW_HIDE);
    KillTimer(hwnd, 1);
    // Leave the material off so the next show is a state change DWM honours.
    ApplyBackdrop(hwnd, false);
    g_scrimFadeAt = 0;
    g_uiVisible = false;
    g_hot = Hot::None;
    g_pressed = Hot::None;
    g_focus = Hot::None;
    // A flyout hands focus back to the window it was opened from.
    if (restoreForeground && g_previousForeground && g_previousForeground != hwnd &&
        IsWindow(g_previousForeground)) {
        SetForegroundWindow(g_previousForeground);
    }
    g_previousForeground = nullptr;
}

// ---------------------------------------------------------------------------
// Actions and keyboard focus
// ---------------------------------------------------------------------------

// One entry point for mouse clicks, keyboard activation and MSAA, so all three
// paths behave identically.
static void ActivateHot(HWND hwnd, Hot hot) {
    switch (hot) {
        case Hot::Close:
            if (TestRunning()) {
                RequestTestCancel();
            }
            HideFlyoutWindow(hwnd);
            break;
        case Hot::Start:
        case Hot::Retry:
            StartOrCancelTest(hwnd);
            break;
        case Hot::Copy:
            CopyResult(hwnd, CopyState());
            break;
        default:
            break;
    }
}

static int AccChildId(Hot hot) {
    switch (hot) {
        case Hot::Close:
            return 1;
        case Hot::Start:
        case Hot::Retry:
            return 2;
        case Hot::Copy:
            return 3;
        default:
            return 0;
    }
}

static Hot HotFromAccChild(int id) {
    switch (id) {
        case 1:
            return Hot::Close;
        case 2:
            return g_view == View::Error ? Hot::Retry : Hot::Start;
        case 3:
            return Hot::Copy;
        default:
            return Hot::None;
    }
}

static std::vector<Hot> FocusOrder() {
    std::vector<Hot> order;
    order.push_back(Hot::Close);
    if (g_view == View::Error) {
        order.push_back(Hot::Retry);
        return order;
    }
    if (g_view == View::Results) {
        order.push_back(Hot::Copy);
    }
    order.push_back(Hot::Start);
    return order;
}

static void MoveFocus(int delta) {
    std::vector<Hot> order = FocusOrder();
    if (order.empty()) {
        return;
    }
    int index = -1;
    for (size_t i = 0; i < order.size(); i++) {
        if (order[i] == g_focus) {
            index = (int)i;
        }
    }
    int next = index < 0 ? (delta > 0 ? 0 : (int)order.size() - 1)
                         : (index + delta + (int)order.size()) % (int)order.size();
    if (order[next] == g_focus) {
        return;
    }
    g_focus = order[next];
    NotifyWinEvent(EVENT_OBJECT_FOCUS, g_flyoutHwnd, OBJID_CLIENT,
                   AccChildId(g_focus));
    InvalidateRect(g_flyoutHwnd, nullptr, FALSE);
}

static bool IsAvailableHot(Hot hot) {
    switch (hot) {
        case Hot::Close:
            return true;
        case Hot::Start:
            return g_view != View::Error;
        case Hot::Copy:
            return g_view == View::Results;
        case Hot::Retry:
            return g_view == View::Error;
        default:
            return false;
    }
}

static D2D1_RECT_F HotRect(Hot hot) {
    switch (hot) {
        case Hot::Start:
            return StartButtonRect(CopyState());
        case Hot::Copy:
            return kCopyRect;
        case Hot::Close:
            return kCloseRect;
        case Hot::Retry:
            return kRetryRect;
        default:
            return Rect(0, 0, 0, 0);
    }
}

class FlyoutAccessible final : public IAccessible {
public:
    explicit FlyoutAccessible(HWND hwnd) : hwnd_(hwnd) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }
        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IAccessible) {
            *object = static_cast<IAccessible*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return (ULONG)InterlockedIncrement(&references_);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG remaining = InterlockedDecrement(&references_);
        if (remaining == 0) {
            delete this;
        }
        return (ULONG)remaining;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* count) override {
        if (!count) {
            return E_POINTER;
        }
        *count = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID,
                                             DISPID*) override {
        return DISP_E_UNKNOWNNAME;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*,
                                     VARIANT*, EXCEPINFO*, UINT*) override {
        return DISP_E_MEMBERNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE get_accParent(IDispatch** parent) override {
        if (!parent) {
            return E_POINTER;
        }
        *parent = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accChildCount(long* count) override {
        if (!count) {
            return E_POINTER;
        }
        *count = (long)FocusOrder().size();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accChild(VARIANT, IDispatch** child) override {
        if (!child) {
            return E_POINTER;
        }
        *child = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accName(VARIANT child, BSTR* name) override {
        if (!name) {
            return E_POINTER;
        }
        PCWSTR value = nullptr;
        if (IsSelf(child)) {
            value = L"Speedtest";
        } else {
            Hot hot = ChildHot(child);
            if (!IsAvailableHot(hot)) {
                return E_INVALIDARG;
            }
            switch (hot) {
                case Hot::Close:
                    value = g_str->closeName;
                    break;
                case Hot::Copy:
                    value = g_str->copy;
                    break;
                case Hot::Retry:
                    value = g_str->retry;
                    break;
                case Hot::Start: {
                    TestState state = CopyState();
                    value = StartButtonLabel(state);
                    break;
                }
                default:
                    return E_INVALIDARG;
            }
        }
        *name = SysAllocString(value);
        return *name ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE get_accValue(VARIANT child, BSTR* value) override {
        if (!value) {
            return E_POINTER;
        }
        if (!IsSelf(child)) {
            *value = nullptr;
            return S_FALSE;
        }
        TestState state = CopyState();
        std::wstring status = state.caption;
        if (state.phase == Phase::Download || state.phase == Phase::Upload) {
            status += L" ";
            status += FormatNumber(state.currentMbps);
            status += L" Mbps";
        }
        *value = SysAllocString(status.c_str());
        return *value ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE put_accValue(VARIANT, BSTR) override {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE get_accDescription(VARIANT child,
                                                  BSTR* description) override {
        if (!description) {
            return E_POINTER;
        }
        *description = nullptr;
        return IsValidChild(child) ? S_FALSE : E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE get_accRole(VARIANT child, VARIANT* role) override {
        if (!role) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }
        VariantInit(role);
        role->vt = VT_I4;
        role->lVal = IsSelf(child) ? ROLE_SYSTEM_WINDOW : ROLE_SYSTEM_PUSHBUTTON;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accState(VARIANT child, VARIANT* state) override {
        if (!state) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }
        VariantInit(state);
        state->vt = VT_I4;
        if (!IsSelf(child)) {
            Hot hot = ChildHot(child);
            state->lVal = STATE_SYSTEM_FOCUSABLE;
            if (hot == g_focus && GetFocus() == hwnd_) {
                state->lVal |= STATE_SYSTEM_FOCUSED;
            }
        } else {
            state->lVal = 0;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accHelp(VARIANT, BSTR* help) override {
        if (!help) {
            return E_POINTER;
        }
        *help = nullptr;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accHelpTopic(BSTR* helpFile, VARIANT,
                                                long* topic) override {
        if (!helpFile || !topic) {
            return E_POINTER;
        }
        *helpFile = nullptr;
        *topic = 0;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut(VARIANT child,
                                                       BSTR* shortcut) override {
        if (!shortcut) {
            return E_POINTER;
        }
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        *shortcut = SysAllocString(g_str == &kStringsIt ? L"Tab / Invio"
                                                        : L"Tab / Enter");
        return *shortcut ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE get_accFocus(VARIANT* child) override {
        if (!child) {
            return E_POINTER;
        }
        VariantInit(child);
        if (g_focus == Hot::None || !IsAvailableHot(g_focus)) {
            child->vt = VT_EMPTY;
        } else {
            child->vt = VT_I4;
            child->lVal = AccChildId(g_focus);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_accSelection(VARIANT* selection) override {
        return get_accFocus(selection);
    }

    HRESULT STDMETHODCALLTYPE get_accDefaultAction(VARIANT child,
                                                    BSTR* action) override {
        if (!action) {
            return E_POINTER;
        }
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        *action = SysAllocString(g_str == &kStringsIt ? L"Attiva" : L"Activate");
        return *action ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT STDMETHODCALLTYPE accSelect(long flags, VARIANT child) override {
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        if (flags & SELFLAG_TAKEFOCUS) {
            Hot hot = ChildHot(child);
            g_focus = hot;
            SetFocus(hwnd_);
            NotifyWinEvent(EVENT_OBJECT_FOCUS, hwnd_, OBJID_CLIENT, AccChildId(hot));
            InvalidateRect(hwnd_, nullptr, FALSE);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accLocation(long* left, long* top, long* width,
                                          long* height, VARIANT child) override {
        if (!left || !top || !width || !height) {
            return E_POINTER;
        }
        if (!IsValidChild(child)) {
            return E_INVALIDARG;
        }

        RECT windowRect = {};
        if (!GetWindowRect(hwnd_, &windowRect)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        if (IsSelf(child)) {
            *left = windowRect.left;
            *top = windowRect.top;
            *width = windowRect.right - windowRect.left;
            *height = windowRect.bottom - windowRect.top;
            return S_OK;
        }

        D2D1_RECT_F rect = HotRect(ChildHot(child));
        *left = windowRect.left + (long)std::lround(rect.left * g_scale);
        *top = windowRect.top + (long)std::lround(rect.top * g_scale);
        *width = (long)std::lround((rect.right - rect.left) * g_scale);
        *height = (long)std::lround((rect.bottom - rect.top) * g_scale);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accNavigate(long direction, VARIANT start,
                                           VARIANT* end) override {
        if (!end) {
            return E_POINTER;
        }
        VariantInit(end);
        std::vector<Hot> order = FocusOrder();
        if (order.empty()) {
            return S_FALSE;
        }
        int index = -1;
        if (start.vt == VT_I4 && start.lVal != CHILDID_SELF) {
            Hot current = HotFromAccChild(start.lVal);
            for (size_t i = 0; i < order.size(); i++) {
                if (order[i] == current) {
                    index = (int)i;
                    break;
                }
            }
        }
        int next = -1;
        if (direction == NAVDIR_FIRSTCHILD) {
            next = 0;
        } else if (direction == NAVDIR_LASTCHILD) {
            next = (int)order.size() - 1;
        } else if (direction == NAVDIR_NEXT && index >= 0 &&
                   index + 1 < (int)order.size()) {
            next = index + 1;
        } else if (direction == NAVDIR_PREVIOUS && index > 0) {
            next = index - 1;
        }
        if (next < 0) {
            return S_FALSE;
        }
        end->vt = VT_I4;
        end->lVal = AccChildId(order[(size_t)next]);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accHitTest(long x, long y, VARIANT* child) override {
        if (!child) {
            return E_POINTER;
        }
        VariantInit(child);
        POINT point = {x, y};
        if (!ScreenToClient(hwnd_, &point)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        Hot hot = HitTest((float)point.x / g_scale, (float)point.y / g_scale);
        child->vt = VT_I4;
        child->lVal = hot == Hot::None ? CHILDID_SELF : AccChildId(hot);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE accDoDefaultAction(VARIANT child) override {
        if (!IsValidChild(child) || IsSelf(child)) {
            return E_INVALIDARG;
        }
        return PostMessageW(hwnd_, WM_APP + 3, AccChildId(ChildHot(child)), 0)
                   ? S_OK
                   : HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT STDMETHODCALLTYPE put_accName(VARIANT, BSTR) override {
        return E_NOTIMPL;
    }

private:
    bool IsSelf(const VARIANT& child) const {
        return child.vt == VT_I4 && child.lVal == CHILDID_SELF;
    }

    Hot ChildHot(const VARIANT& child) const {
        return child.vt == VT_I4 ? HotFromAccChild(child.lVal) : Hot::None;
    }

    bool IsValidChild(const VARIANT& child) const {
        return IsSelf(child) || IsAvailableHot(ChildHot(child));
    }

    volatile LONG references_ = 1;
    HWND hwnd_;
};

static FlyoutAccessible* g_accessible = nullptr;

static void TickFlyout(HWND hwnd) {
    // Repaint only when something actually changes: idle frames would make
    // DWM re-composite the acrylic material for nothing and stutter the
    // scrim fade.
    bool repaint = ScrimFading();

    TestState state = CopyState();
    UINT mouseButtonsDown = MouseButtonsDown();
    UINT newlyPressedButtons = mouseButtonsDown & ~g_mouseButtonsDown;
    g_mouseButtonsDown = mouseButtonsDown;
    if (g_uiVisible && newlyPressedButtons && !StateRunning(state)) {
        POINT cursor = {};
        RECT flyout = {};
        if (GetCursorPos(&cursor) && GetWindowRect(hwnd, &flyout) &&
            !PtInRect(&flyout, cursor)) {
            HideFlyoutWindow(hwnd, false);
            return;
        }
    }

    // The ring shows throughput. Start each transfer phase from an empty ring
    // instead of letting the small latency arc decay into a visible blob while
    // the download figure is still 0.0.
    static Phase lastPhase = Phase::Idle;
    if (state.phase != lastPhase) {
        NotifyWinEvent(EVENT_OBJECT_VALUECHANGE, hwnd, OBJID_CLIENT, CHILDID_SELF);
        if (state.phase == Phase::Warmup || state.phase == Phase::Download ||
            state.phase == Phase::Upload || state.phase == Phase::Stopping ||
            state.phase == Phase::Idle || state.phase == Phase::Canceled) {
            g_gaugeShown = 0;
            repaint = true;
        }
        lastPhase = state.phase;
    }

    if (g_testThread && !StateRunning(state)) {
        if (WaitForSingleObject(g_testThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_testThread);
            g_testThread = nullptr;
        }
    }

    View previousView = g_view;
    bool previousAnimating = g_animating;
    if (state.phase == Phase::Done && g_view != View::Results && !g_animating) {
        BeginView(View::Results);
    } else if (state.phase == Phase::Error && g_view != View::Error) {
        BeginView(View::Error);
        g_hot = Hot::None;
    } else if (state.phase == Phase::Canceled && g_view == View::Results) {
        BeginView(View::Gauge);
    }

    float previousHeight = g_layoutHeight;
    if (g_animating) {
        float t = AnimT();
        float eased = t * t * (3.0f - 2.0f * t);
        float targetHeight = ViewHeight(g_view);
        g_layoutHeight = g_heightFrom + (targetHeight - g_heightFrom) * eased;
    } else {
        g_layoutHeight = ViewHeight(g_view);
    }
    if (std::fabs(g_layoutHeight - previousHeight) > 0.25f) {
        PositionFlyout(hwnd, MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
        repaint = true;
    }

    if (g_view != previousView || g_animating != previousAnimating) {
        repaint = true;
    }

    double target = 0;
    if (state.phase == Phase::Latency) {
        // Show idle latency only in the lower stats and keep the speed ring empty.
        target = 0;
    } else if (state.phase == Phase::Download || state.phase == Phase::Upload) {
        target = state.currentMbps;
    } else if (state.phase == Phase::Done) {
        target = state.download;
    }
    double targetFraction = GaugeFraction(target);
    double previousGauge = g_gaugeShown;
    g_gaugeShown += (targetFraction - g_gaugeShown) * 0.18;
    if (std::fabs(targetFraction - g_gaugeShown) < 0.0015) {
        g_gaugeShown = targetFraction;
    }
    if (std::fabs(g_gaugeShown - previousGauge) > 0.0005) {
        repaint = true;
    }

    if (g_animating && AnimT() >= 1.0f) {
        g_animating = false;
        repaint = true;
    }

    // A running test updates live figures every tick; the copy toast has a
    // deadline that needs a final repaint to disappear.
    if (StateRunning(state) || g_animating) {
        repaint = true;
    }
    if (g_copyUntil) {
        if (Now() < g_copyUntil) {
            repaint = true;
        } else {
            g_copyUntil = 0;
            repaint = true;
        }
    }

    if (repaint) {
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

static LRESULT CALLBACK FlyoutWndProc(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;

        case WM_GETOBJECT:
            if ((LONG)lParam == OBJID_CLIENT) {
                if (!g_accessible) {
                    g_accessible = new (std::nothrow) FlyoutAccessible(hwnd);
                }
                return g_accessible
                           ? LresultFromObject(IID_IAccessible, wParam, g_accessible)
                           : 0;
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            HDC dc = BeginPaint(hwnd, &paint);
            if (EnsureRenderTarget()) {
                g_renderTarget->BeginDraw();
                TestState state = CopyState();
                DrawFrame(state);
                HRESULT result = g_renderTarget->EndDraw();
                if (result == D2DERR_RECREATE_TARGET) {
                    ReleaseTargets();
                } else if (FAILED(result)) {
                    static int endDrawFailures = 0;
                    if (endDrawFailures++ < 3) {
                        Wh_Log(L"EndDraw failed 0x%08X", (unsigned)result);
                    }
                    ReleaseTargets();
                } else if (g_composition && g_swapChain) {
                    // Sync interval 0: the swap chain is composed by DWM on
                    // its own clock, so waiting for a vblank here only blocks
                    // the UI thread inside WM_PAINT.
                    HRESULT present = g_swapChain->Present(0, 0);
                    if (FAILED(present)) {
                        Wh_Log(L"Present failed 0x%08X", (unsigned)present);
                        ReleaseTargets();
                    }
                }
            }
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_SIZE:
            if (g_renderTarget) {
                ResizeRenderTarget();
            }
            return 0;

        case WM_TIMER:
            if (wParam == 1) {
                TickFlyout(hwnd);
            }
            return 0;

        case WM_MOUSEMOVE: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            Hot hot = HitTest(x, y);
            bool focusChanged = g_focus != Hot::None;
            if (hot != g_hot || focusChanged) {
                g_hot = hot;
                g_focus = Hot::None;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hot != Hot::None) {
                g_hot = Hot::None;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT point = {};
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                Hot hot = HitTest((float)point.x / g_scale, (float)point.y / g_scale);
                if (hot != Hot::None) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            g_pressed = HitTest(x, y);
            if (g_pressed != Hot::None) {
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            float x = (float)GET_X_LPARAM(lParam) / g_scale;
            float y = (float)GET_Y_LPARAM(lParam) / g_scale;
            Hot hot = HitTest(x, y);
            Hot pressed = g_pressed;
            g_pressed = Hot::None;
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            if (hot != Hot::None && hot == pressed) {
                ActivateHot(hwnd, hot);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_TAB) {
                MoveFocus((GetKeyState(VK_SHIFT) & 0x8000) ? -1 : 1);
                return 0;
            }
            if ((wParam == VK_RETURN || wParam == VK_SPACE) &&
                g_focus != Hot::None) {
                ActivateHot(hwnd, g_focus);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                if (TestRunning()) {
                    RequestTestCancel();
                } else {
                    HideFlyoutWindow(hwnd);
                }
                return 0;
            }
            break;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_uiVisible &&
                !TestRunning()) {
                HideFlyoutWindow(hwnd, false);
            }
            // Let DefWindowProc run so the click-back focus handling continues;
            // returning 0 here left the previously active window without
            // restored focus.
            break;

        case WM_DPICHANGED: {
            UINT dpi = HIWORD(wParam);
            if (dpi == 0) {
                dpi = LOWORD(wParam);
            }
            if (dpi) {
                g_dpi = (float)dpi;
                g_scale = g_dpi / 96.0f;
            }
            // Re-anchor to the work area instead of the suggested rect: the
            // flyout is positioned manually above the taskbar.
            PositionFlyout(hwnd, MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_SETTINGCHANGE:
            // Follow a light/dark switch or a transparency toggle without
            // waiting for the next time the flyout is opened.
            if (wParam == 0 && lParam &&
                _wcsicmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                RefreshBackdrop(hwnd);
            }
            return 0;

        case WM_THEMECHANGED:
            // High contrast and theme switches arrive here as well; the
            // capability check decides whether the material may stay on.
            RefreshBackdrop(hwnd);
            return 0;

        case WM_DWMCOMPOSITIONCHANGED:
            RefreshBackdrop(hwnd);
            return 0;

        case WM_CLOSE:
            if (TestRunning()) {
                RequestTestCancel();
            }
            HideFlyoutWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_accessible) {
                // A client (Narrator, Inspect, ...) may still hold a
                // reference; disconnect it from this object's vtable so a
                // late call after the DLL is unloaded cannot crash the host.
                CoDisconnectObject(g_accessible, 0);
                g_accessible->Release();
                g_accessible = nullptr;
            }
            g_flyoutHwnd = nullptr;
            PostQuitMessage(0);
            return 0;

        default:
            if (message == WM_APP + 2) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (message == g_showMessage) {
                ShowFlyoutWindow(hwnd);
                return 0;
            }
            if (message == WM_APP + 1) {
                TickFlyout(hwnd);
                return 0;
            }
            if (message == WM_APP + 4) {
                NetworkInfo network = ReadNetworkByIndex(
                    (ULONG)wParam, (ADDRESS_FAMILY)lParam);
                if (!network.name.empty()) {
                    g_network = network;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            if (message == WM_APP + 3) {
                Hot hot = HotFromAccChild((int)wParam);
                if (IsAvailableHot(hot)) {
                    ActivateHot(hwnd, hot);
                }
                return 0;
            }
            break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// ---------------------------------------------------------------------------
// UI thread and module lifecycle
// ---------------------------------------------------------------------------

// Returns the module of the calling code (the mod DLL), not the host. Used as
// the window class instance so registration/unregistration is scoped to the
// mod's own module, as Windhawk conventions expect.
// The module handle of the mod image: the class name then belongs to the
// mod and disappears with it, not with the host.
extern "C" IMAGE_DOS_HEADER __ImageBase;

static DWORD WINAPI UiThreadProc(LPVOID) {
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        Wh_Log(L"COM initialization failed (error 0x%08X)", (unsigned)comResult);
    }

    RefreshAppearance();

    WNDCLASSEXW windowClass = {sizeof(windowClass)};
    // No CS_DROPSHADOW: it draws a square shadow around the window rect, which
    // stays square while DWM clips the material to rounded corners.
    windowClass.style = 0;
    windowClass.lpfnWndProc = FlyoutWndProc;
    windowClass.hInstance = (HINSTANCE)&__ImageBase;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = nullptr;
    windowClass.lpszClassName = kFlyoutClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed (error %u)", GetLastError());
        if (comInitialized) {
            CoUninitialize();
        }
        return 1;
    }

    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                                    WS_EX_NOREDIRECTIONBITMAP,
                                kFlyoutClass, L"Speedtest", WS_POPUP, 0, 0,
                                (int)std::lround(kWidth), (int)std::lround(kGaugeHeight),
                                nullptr, nullptr, windowClass.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed (error %u)", GetLastError());
        UnregisterClassW(kFlyoutClass, windowClass.hInstance);
        if (comInitialized) {
            CoUninitialize();
        }
        return 1;
    }
    g_flyoutHwnd = hwnd;

    // Rounded corners and a solid frame on Windows 11.
    int corner = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner,
                          sizeof(corner));

    // Desktop Acrylic is composed over the whole window only when the frame is
    // extended: on a captionless popup with zero margins DWM keeps the solid
    // fallback. This is a one-time setup; the runtime state is driven by
    // ApplyBackdrop().
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    ApplyBackdrop(hwnd, false);

    D2D1_FACTORY_OPTIONS options = {};
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory), &options,
                                 (void**)&g_d2dFactory))) {
        g_d2dFactory = nullptr;
    }
    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                  __uuidof(IDWriteFactory),
                                  (IUnknown**)&g_dwriteFactory))) {
        g_dwriteFactory = nullptr;
    }

    D2D1_STROKE_STYLE_PROPERTIES strokeProperties = {};
    strokeProperties.startCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.endCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.dashCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
    if (g_d2dFactory) {
        g_d2dFactory->CreateStrokeStyle(strokeProperties, nullptr, 0, &g_roundStroke);
    }

    // A mod unload can arrive while this thread is still starting up. In that
    // case the message loop would never receive WM_QUIT, so check the flag
    // before entering it.
    if (InterlockedCompareExchange(&g_shutdown, 0, 0)) {
        DestroyWindow(hwnd);
    } else {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }
    g_cancelRequested.store(1, std::memory_order_relaxed);
    // The test thread is owned (and waited for) by Wh_ModUninit, which runs
    // before the DLL is unloaded. Waiting here as well could keep the UI
    // thread alive past that point.
    ReleaseGraphics();
    g_flyoutHwnd = nullptr;
    UnregisterClassW(kFlyoutClass, windowClass.hInstance);
    if (comInitialized) {
        CoUninitialize();
    }
    return 0;
}

static bool IsPrimaryShellProcess() {
    // Every explorer.exe instance (and every process loading the mod) would
    // otherwise start its own UI thread and register the same flyout class.
    // With "launch folder windows in a separate process" several explorer
    // instances coexist; only the one owning the taskbar window should host
    // the flyout. If the taskbar does not exist yet anywhere (the mod can be
    // injected very early in the primary explorer), fall back to the process
    // name so the first explorer still hosts it.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD trayPid = 0;
        GetWindowThreadProcessId(tray, &trayPid);
        return trayPid == GetCurrentProcessId();
    }
    wchar_t path[MAX_PATH * 2] = {};
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"explorer.exe") == 0;
}

BOOL Wh_ModInit(void) {
    LoadSettings();
    InterlockedExchange(&g_shutdown, 0);

    g_showMessage = RegisterWindowMessageW(L"SpeedTestFlyout.Show");
    if (!g_showMessage) {
        Wh_Log(L"RegisterWindowMessage failed (error %u)", GetLastError());
    }

    if (IsPrimaryShellProcess() && g_showMessage) {
        g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0,
                                  &g_uiThreadId);
        if (!g_uiThread) {
            Wh_Log(L"UI thread creation failed (error %u)", GetLastError());
        }
    }

    // Import-form hooks: referencing ShellExecuteExW/ShellExecuteW makes the
    // mod import shell32 (a KnownDLL, so it is loaded with the mod), and the
    // two hooks can no longer be silently missing in a host that loads
    // shell32 later.
    if (!WindhawkUtils::SetFunctionHook(ShellExecuteExW, ShellExecuteExW_Hook,
                                        &ShellExecuteExW_Original)) {
        Wh_Log(L"Unable to hook ShellExecuteExW");
    }
    if (!WindhawkUtils::SetFunctionHook(ShellExecuteW, ShellExecuteW_Hook,
                                        &ShellExecuteW_Original)) {
        Wh_Log(L"Unable to hook ShellExecuteW");
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE kernel = kernelBase ? kernelBase : kernel32;
    auto createProcessW = kernel
                              ? (CreateProcessW_t)GetProcAddress(kernel, "CreateProcessW")
                              : nullptr;
    if (createProcessW &&
        !WindhawkUtils::SetFunctionHook(createProcessW, CreateProcessW_Hook,
                                        &CreateProcessW_Original)) {
        Wh_Log(L"Unable to hook CreateProcessW");
    }

    return TRUE;
}

void Wh_ModUninit(void) {
    g_cancelRequested.store(1, std::memory_order_relaxed);
    InterlockedExchange(&g_shutdown, 1);

    if (g_flyoutHwnd && !PostMessageW(g_flyoutHwnd, WM_APP + 2, 0, 0)) {
        Wh_Log(L"Posting flyout shutdown message failed (error %u)",
               GetLastError());
    }

    if (g_uiThread && g_uiThreadId) {
        // The thread queue may not exist yet; retry until it is ready or the
        // startup path observes g_shutdown and exits.
        while (WaitForSingleObject(g_uiThread, 0) == WAIT_TIMEOUT) {
            if (PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0)) {
                break;
            }
            Sleep(10);
        }
        // Never unload code while either thread can still be executing it.
        DWORD result = WaitForSingleObject(g_uiThread, INFINITE);
        if (result != WAIT_OBJECT_0) {
            Wh_Log(L"Waiting for UI thread failed (error %u)", GetLastError());
            return;
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        g_uiThreadId = 0;
    }

    if (g_testThread) {
        DWORD result = WaitForSingleObject(g_testThread, INFINITE);
        if (result != WAIT_OBJECT_0) {
            Wh_Log(L"Waiting for test thread failed (error %u)", GetLastError());
            return;
        }
        CloseHandle(g_testThread);
        g_testThread = nullptr;
    }
}

void Wh_ModSettingsChanged(void) {
    LoadSettings();
}


