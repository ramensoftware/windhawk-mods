// ==WindhawkMod==
// @id              lock-screen-clock-styler
// @name            Lock Screen Clock Styler
// @name:tr-TR      Kilit Ekranı Saat Özelleştirici
// @name:zh-CN      锁屏时钟样式定制
// @name:zh-TW      鎖定畫面時鐘樣式自訂
// @name:ja-JP      ロック画面の時計スタイラー
// @name:ko-KR      잠금 화면 시계 스타일러
// @name:de-DE      Sperrbildschirm-Uhr-Styler
// @name:es-ES      Personalizador del reloj de la pantalla de bloqueo
// @name:pt-BR      Personalizador de Relógio da Tela de Bloqueio
// @name:fr-FR      Personnaliseur d'horloge d'écran de verrouillage
// @name:it-IT      Personalizzatore orologio schermata di blocco
// @name:pl-PL      Stylizator zegara ekranu blokady
// @name:ru-RU      Стилизатор часов экрана блокировки
// @name:uk-UA      Стилізатор годинника екрана блокування
// @description     Customize the Windows 11 lock screen clock with modern styles, date display, and widget controls.
// @description:tr-TR Windows 11 kilit ekranı saatini modern stiller, tarih göstergesi ve widget denetimleriyle özelleştirin.
// @description:zh-CN 使用现代样式、日期显示和小组件控制来自定义 Windows 11 锁屏时钟。
// @description:zh-TW 使用現代樣式、日期顯示與小工具控制來自訂 Windows 11 鎖定畫面時鐘。
// @description:ja-JP モダンなスタイル、日付表示、ウィジェット制御で Windows 11 のロック画面の時計をカスタマイズします。
// @description:ko-KR 모던한 스타일, 날짜 표시 및 위젯 제어로 Windows 11 잠금 화면 시계를 맞춤 설정합니다.
// @description:de-DE Passen Sie die Windows 11-Sperrbildschirmuhr mit modernen Stilen, Datumsanzeige und Widget-Steuerung an.
// @description:es-ES Personaliza el reloj de la pantalla de bloqueo de Windows 11 con estilos modernos, visualización de fecha y control de widgets.
// @description:pt-BR Personalize o relógio da tela de bloqueio do Windows 11 com estilos modernos, exibição de data e controle de widgets.
// @description:fr-FR Personnalisez l'horloge de l'écran de verrouillage de Windows 11 avec des styles modernes, l'affichage de la date et le contrôle des widgets.
// @description:it-IT Personalizza l'orologio della schermata di blocco di Windows 11 con stili moderni, visualizzazione della data e controllo dei widget.
// @description:pl-PL Dostosuj zegar ekranu blokady systemu Windows 11 za pomocą nowoczesnych stylów, wyświetlania daty i kontroli widżetów.
// @description:ru-RU Настройте часы экрана блокировки Windows 11 с современными стилями, отображением даты и управлением виджетами.
// @description:uk-UA Налаштуйте годинник екрана блокування Windows 11 за допомогою сучасних стилів, відображення дати та керування віджетами.
// @version         3.0.0
// @author          ali0zdas
// @github          https://github.com/ali0zdas
// @twitter         https://twitter.com/ali0zdas
// @license         GPL-3.0-only
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Clock Styler

Customize and enhance the Windows 11 lock screen clock with modern designs, frosted glass panels, dynamic date capsules, and clean layout controls.

## What's New:
- **✨ NEW in v3.0.0** 
- **ANSI Shadow Style:** Retro terminal ASCII block-art clock with 4 phosphor color themes, prompt header bar, and glow effect.
- **🎨 Per-Theme Customizations:** Dedicated settings groups so every style can be tuned independently:
  - **Colors & Typography:** 14 curated color palettes, 8 built-in Windows fonts, font weight, and drop-shadow toggle.
  - **Retro Pixel & Flip Cards:** LED dot shapes, frosted backplate opacity, Flip card corner radius, and center split seam.
  - **ANSI Shadow & Cyberpunk HUD:** 4 terminal/HUD color presets, header/telemetry bar toggles, phosphor glow, and card/chassis opacity.
- **✨ NEW — 3 VERTICAL CLOCK STYLES:** Pixel, Dual Weight & Impact.

## Screenshots:
| Style | Preview |
| :---: | :---: |
| Retro Pixel / LED Matrix | ![Retro Pixel](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/retro%20pixel%20new.png) |
| Vertical Stack Styles (Pixel, Modern, Poster) | ![Vertical Stack Styles](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/vertical%20styles.png) |
| ANSI Shadow (Terminal ASCII) | ![ANSI Shadow](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/ansi%20shadow.png) |
| Modern Flip Clock | ![Modern Flip Clock](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/modern%20flipclock%20new.png) |
| Classic Flip Clock | ![Classic Flip Clock](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/classical%20flipclock%20new.png) |
| Minimalist Sonoma | ![Minimalist Sonoma](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/sonoma%20new.png) |
| Cyberpunk Sci-Fi HUD | ![Cyberpunk Sci-Fi HUD](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/cyberpunk%20hud.png) |

## Features:
- **Multiple Distinct Clock Styles:** 9 distinct styles including Retro Pixel, Vertical Stacks, ANSI Shadow, Flip Clocks, Minimalist Sonoma, and Cyberpunk HUD.
- **Lightweight & Thread-Safe:** Native XAML injection without XAML Diagnostics TAP, ensuring minimal CPU overhead.
- **Show Seconds Toggle:** Theme-aware live seconds display (`Automatic`, `Always Show`, or `Always Hide` — supported across all non-flip and non-vertical styles).
- **Extensive Customization:** Card opacity, font size, vertical offsets, and screen alignments are fully customizable via settings.

## Security Note:
This mod exclusively targets `LockApp.exe`. It does not inject into or interfere with `LogonUI.exe` or `winlogon.exe` in the Secure Desktop, preventing any system lockup or logon security risks.

## 🤝 Contributing & Feedback:
Feedback, bug reports, and theme suggestions are warmly welcomed! If you'd like to suggest a new clock style, report an issue, or contribute code, feel free to visit the [GitHub repository](https://github.com/ali0zdas/windhawk-mod-lock-screen-clock-styler) and open an issue or pull request.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- clockStyle: pixel
  $name: "🎨 Clock Style"
  $name:tr-TR: "🎨 Saat Tasarım Stili"
  $name:zh-CN: "🎨 时钟样式"
  $name:zh-TW: "🎨 時鐘樣式"
  $name:ru-RU: "🎨 Стиль часов"
  $name:de-DE: "🎨 Uhrstil"
  $name:es-ES: "🎨 Estilo de reloj"
  $name:pt-BR: "🎨 Estilo do relógio"
  $name:fr-FR: "🎨 Style d'horloge"
  $name:it-IT: "🎨 Stile orologio"
  $name:pl-PL: "🎨 Styl zegara"
  $name:ja-JP: "🎨 時計スタイル"
  $name:ko-KR: "🎨 시계 스타일"
  $name:uk-UA: "🎨 Стиль годинника"
  $description: "The clock style to display on the lock screen."
  $description:tr-TR: "Kilit ekranında görüntülenecek saat stili."
  $description:zh-CN: "要在锁屏上显示的时钟样式。"
  $description:zh-TW: "要在鎖定畫面上顯示的時鐘樣式。"
  $description:ru-RU: "Стиль часов для отображения на экране блокировки."
  $description:de-DE: "Der auf dem Sperrbildschirm anzuzeigende Uhrstil."
  $description:es-ES: "El estilo de reloj que se mostrará en la pantalla de bloqueo."
  $description:pt-BR: "O estilo de relógio a ser exibido na tela de bloqueio."
  $description:fr-FR: "Le style d'horloge à afficher sur l'écran de verrouillage."
  $description:it-IT: "Lo stile dell'orologio da visualizzare nella schermata di blocco."
  $description:pl-PL: "Styl zegara do wyświetlenia na ekranie blokady."
  $description:ja-JP: "ロック画面に表示する時計のスタイル。"
  $description:ko-KR: "잠금 화면에 표시할 시계 스타일입니다."
  $description:uk-UA: "Стиль годинника для відображення на екрані блокування."
  $options:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Default Clock Only (No Custom Clock)"
  $options:tr-TR:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Yalnızca Varsayılan Saat (Özel Saat Yok)"
  $options:zh-CN:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "仅默认时钟（无自定义时钟）"
  $options:zh-TW:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "僅預設時鐘（無自訂時鐘）"
  $options:ru-RU:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Только стандартные часы (без кастомных)"
  $options:de-DE:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Nur Standard-Uhr (keine benutzerdefinierte Uhr)"
  $options:es-ES:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Solo reloj predeterminado (sin reloj personalizado)"
  $options:pt-BR:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Apenas relógio padrão (sem relógio personalizado)"
  $options:fr-FR:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Horloge par défaut uniquement (aucune personnalisation)"
  $options:it-IT:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Solo orologio predefinito (nessun orologio personalizzato)"
  $options:pl-PL:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Tylko domyślny zegar (brak własnego zegara)"
  $options:ja-JP:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "デフォルトの時計のみ（カスタム時計なし）"
  $options:ko-KR:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "기본 시계만 사용 (사용자 지정 시계 없음)"
  $options:uk-UA:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - verticalBahnschrift: "Vertical Pixel / Nothing OS (Bahnschrift)"
    - verticalSegoe: "Vertical Modern / Dual Weight (Segoe UI)"
    - verticalImpact: "Vertical Poster / Heavy Impact (Impact)"
    - ansiShadow: "ANSI Shadow (Terminal ASCII)"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - defaultOnly: "Лише стандартний годинник (без власного)"
- timeFormat: 24h
  $name: "🕒 Time Format"
  $name:tr-TR: "🕒 Saat Formatı"
  $name:zh-CN: "🕒 时间格式"
  $name:zh-TW: "🕒 時間格式"
  $name:ru-RU: "🕒 Формат времени"
  $name:de-DE: "🕒 Zeitformat"
  $name:es-ES: "🕒 Formato de hora"
  $name:pt-BR: "🕒 Formato de hora"
  $name:fr-FR: "🕒 Format de l'heure"
  $name:it-IT: "🕒 Formato ora"
  $name:pl-PL: "🕒 Format czasu"
  $name:ja-JP: "🕒 時刻の形式"
  $name:ko-KR: "🕒 시간 형식"
  $name:uk-UA: "🕒 Формат часу"
  $description: "Select 24-hour format or 12-hour format with AM/PM indicator."
  $description:tr-TR: "24 saat formatı veya AM/PM göstergeli 12 saat formatı seçimi."
  $description:zh-CN: "选择 24 小时制或带有 AM/PM 指示器的 12 小时制。"
  $description:zh-TW: "選擇 24 小時制或帶有 AM/PM 指示器的 12 小時制。"
  $description:ru-RU: "Выберите 24-часовой формат или 12-часовой формат с индикатором AM/PM."
  $description:de-DE: "Wählen Sie das 24-Stunden-Format oder das 12-Stunden-Format mit AM/PM-Anzeige."
  $description:es-ES: "Selecciona el formato de 24 horas o el formato de 12 horas con indicador AM/PM."
  $description:pt-BR: "Selecione o formato de 24 horas ou o formato de 12 horas com indicador AM/PM."
  $description:fr-FR: "Sélectionnez le format 24 heures ou 12 heures avec indicateur AM/PM."
  $description:it-IT: "Seleziona il formato a 24 ore o a 12 ore con indicatore AM/PM."
  $description:pl-PL: "Wybierz format 24-godzinny lub 12-godzinny ze wskaźnikiem AM/PM."
  $description:ja-JP: "24時間表示または午前/午後（AM/PM）表示の12時間表示を選択します。"
  $description:ko-KR: "24시간 형식 또는 오전/오후(AM/PM) 표시가 있는 12시간 형식을 선택합니다."
  $description:uk-UA: "Виберіть 24-годинний формат або 12-годинний формат з індикатором AM/PM."
  $options:
    - 24h: "24-Hour (14:30)"
    - 12h: "12-Hour AM/PM (02:30 PM)"
  $options:tr-TR:
    - 24h: "24 Saat (14:30)"
    - 12h: "12 Saat AM/PM (02:30 PM)"
  $options:zh-CN:
    - 24h: "24 小时制 (14:30)"
    - 12h: "12 小时制 AM/PM (02:30 PM)"
  $options:zh-TW:
    - 24h: "24 小時制 (14:30)"
    - 12h: "12 小時制 AM/PM (02:30 PM)"
  $options:ru-RU:
    - 24h: "24-часовой (14:30)"
    - 12h: "12-часовой AM/PM (02:30 PM)"
  $options:de-DE:
    - 24h: "24-Stunden (14:30)"
    - 12h: "12-Stunden AM/PM (02:30 PM)"
  $options:es-ES:
    - 24h: "24 horas (14:30)"
    - 12h: "12 horas AM/PM (02:30 PM)"
  $options:pt-BR:
    - 24h: "24 horas (14:30)"
    - 12h: "12 horas AM/PM (02:30 PM)"
  $options:fr-FR:
    - 24h: "24 heures (14:30)"
    - 12h: "12 heures AM/PM (02:30 PM)"
  $options:it-IT:
    - 24h: "24 ore (14:30)"
    - 12h: "12 ore AM/PM (02:30 PM)"
  $options:pl-PL:
    - 24h: "24-godzinny (14:30)"
    - 12h: "12-godzinny AM/PM (02:30 PM)"
  $options:ja-JP:
    - 24h: "24時間表示 (14:30)"
    - 12h: "12時間表示 AM/PM (02:30 PM)"
  $options:ko-KR:
    - 24h: "24시간 형식 (14:30)"
    - 12h: "12시간 형식 AM/PM (02:30 PM)"
  $options:uk-UA:
    - 24h: "24-годинний (14:30)"
    - 12h: "12-годинний AM/PM (02:30 PM)"
- showSeconds: auto
  $name: "⏱️ Show Seconds"
  $name:tr-TR: "⏱️ Saniyeyi Göster"
  $name:zh-CN: "⏱️ 显示秒数"
  $name:zh-TW: "⏱️ 顯示秒數"
  $name:ru-RU: "⏱️ Показывать секунды"
  $name:de-DE: "⏱️ Sekunden anzeigen"
  $name:es-ES: "⏱️ Mostrar segundos"
  $name:pt-BR: "⏱️ Mostrar segundos"
  $name:fr-FR: "⏱️ Afficher les secondes"
  $name:it-IT: "⏱️ Mostra secondi"
  $name:pl-PL: "⏱️ Pokaż sekundy"
  $name:ja-JP: "⏱️ 秒を表示"
  $name:ko-KR: "⏱️ 초 표시"
  $name:uk-UA: "⏱️ Показувати секунди"
  $description: "Controls live seconds display. Automatic follows the theme default (off for Minimalist Sonoma, flip clocks, and vertical stack styles; on for Retro Pixel, ANSI Shadow, and Cyberpunk HUD)."
  $description:tr-TR: "Canlı saniye görünümünü ayarlar. Otomatik seçeneği tema varsayılanını kullanır (Minimalist Sonoma, flip saatler ve dikey yığın stilleri için kapalı; Retro Pixel, ANSI Shadow ve Cyberpunk HUD için açık)."
  $description:zh-CN: "控制实时秒数显示。“自动”将使用主题默认值（Minimalist Sonoma、翻页时钟和垂直堆叠样式默认关闭；Retro Pixel、ANSI Shadow 和 Cyberpunk HUD 默认开启）。"
  $description:zh-TW: "控制即時秒數顯示。「自動」將使用佈景主題預設值（Minimalist Sonoma、翻頁時鐘與垂直堆疊樣式預設關閉；Retro Pixel、ANSI Shadow 與 Cyberpunk HUD 預設開啟）。"
  $description:ru-RU: "Управляет отображением секунд в реальном времени. «Автоматически» использует значение темы (выкл. для Minimalist Sonoma, перекидных и вертикальных часов; вкл. для Retro Pixel, ANSI Shadow и Cyberpunk HUD)."
  $description:de-DE: "Steuert die Live-Sekundenanzeige. „Automatisch“ verwendet den Design-Standard (aus für Minimalist Sonoma, Klappuhren und vertikale Stile; ein für Retro Pixel, ANSI Shadow und Cyberpunk HUD)."
  $description:es-ES: "Controla la visualización de segundos en vivo. Automático usa el valor predeterminado del tema (desactivado para Minimalist Sonoma, relojes flip y estilos verticales; activado para Retro Pixel, ANSI Shadow y Cyberpunk HUD)."
  $description:pt-BR: "Controla a exibição de segundos ao vivo. Automático usa o padrão do tema (desativado para Minimalist Sonoma, relógios flip e estilos verticais; ativado para Retro Pixel, ANSI Shadow e Cyberpunk HUD)."
  $description:fr-FR: "Contrôle l'affichage des secondes en direct. Automatique utilise le paramètre par défaut du thème (désactivé pour Minimalist Sonoma, les horloges à volets et les styles verticaux ; activé pour Retro Pixel, ANSI Shadow et Cyberpunk HUD)."
  $description:it-IT: "Controlla la visualizzazione dei secondi in tempo reale. Automatico usa il valore predefinito del tema (disattivato per Minimalist Sonoma, orologi flip e stili verticali; attivato per Retro Pixel, ANSI Shadow e Cyberpunk HUD)."
  $description:pl-PL: "Steruje wyświetlaniem sekund na żywo. Opcja „Automatycznie” używa ustawienia domyślnego motywu (wyłączone dla Minimalist Sonoma, zegarów klapkowych i stylów pionowych; włączone dla Retro Pixel, ANSI Shadow i Cyberpunk HUD)."
  $description:ja-JP: "リアルタイムの秒表示を制御します。「自動」はテーマの既定値を使用します（Minimalist Sonoma、フリップ時計、垂直スタックスタイルではオフ、Retro Pixel、ANSI Shadow、Cyberpunk HUD ではオン）。"
  $description:ko-KR: "실시간 초 표시를 제어합니다. '자동'은 테마 기본값을 사용합니다(Minimalist Sonoma, 플립 시계, 세로 스택 스타일은 꺼짐, Retro Pixel, ANSI Shadow, Cyberpunk HUD는 켜짐)."
  $description:uk-UA: "Керує відображенням секунд у реальном часі. «Автоматично» використовує налаштування теми (вимк. для Minimalist Sonoma, перекидних і вертикальних годинників; увімк. для Retro Pixel, ANSI Shadow та Cyberpunk HUD)."
  $options:
    - auto: "Automatic (Theme Default)"
    - on: "Always Show"
    - off: "Always Hide"
  $options:tr-TR:
    - auto: "Otomatik (Tema Varsayılanı)"
    - on: "Her Zaman Göster"
    - off: "Her Zaman Gizle"
  $options:zh-CN:
    - auto: "自动（主题默认）"
    - on: "始终显示"
    - off: "始终隐藏"
  $options:zh-TW:
    - auto: "自動（佈景主題預設）"
    - on: "一律顯示"
    - off: "一律隱藏"
  $options:ru-RU:
    - auto: "Автоматически (по умолчанию темы)"
    - on: "Всегда показывать"
    - off: "Всегда скрывать"
  $options:de-DE:
    - auto: "Automatisch (Design-Standard)"
    - on: "Immer anzeigen"
    - off: "Immer ausblenden"
  $options:es-ES:
    - auto: "Automático (Predeterminado del tema)"
    - on: "Mostrar siempre"
    - off: "Ocultar siempre"
  $options:pt-BR:
    - auto: "Automático (Padrão do tema)"
    - on: "Sempre mostrar"
    - off: "Sempre ocultar"
  $options:fr-FR:
    - auto: "Automatique (Par défaut du thème)"
    - on: "Toujours afficher"
    - off: "Toujours masquer"
  $options:it-IT:
    - auto: "Automatico (Predefinito del tema)"
    - on: "Mostra sempre"
    - off: "Nascondi sempre"
  $options:pl-PL:
    - auto: "Automatycznie (Domyślne motywu)"
    - on: "Zawsze pokazuj"
    - off: "Zawsze ukrywaj"
  $options:ja-JP:
    - auto: "自動（テーマの既定値）"
    - on: "常に表示"
    - off: "常に非表示"
  $options:ko-KR:
    - auto: "자동 (테마 기본값)"
    - on: "항상 표시"
    - off: "항상 숨기기"
  $options:uk-UA:
    - auto: "Автоматично (За замовчуванням теми)"
    - on: "Завжди показувати"
    - off: "Завжди приховувати"
- clockFontSize: 165
  $name: "📏 Digit Font Size"
  $name:tr-TR: "📏 Rakam Yazı Tipi Boyutu"
  $name:zh-CN: "📏 数字字体大小"
  $name:zh-TW: "📏 數字字型大小"
  $name:ru-RU: "📏 Размер шрифта цифр"
  $name:de-DE: "📏 Ziffernschriftgröße"
  $name:es-ES: "📏 Tamaño de fuente de dígitos"
  $name:pt-BR: "📏 Tamanho da fonte dos dígitos"
  $name:fr-FR: "📏 Taille de police des chiffres"
  $name:it-IT: "📏 Dimensione carattere cifre"
  $name:pl-PL: "📏 Rozmiar czcionki cyfr"
  $name:ja-JP: "📏 数字のフォントサイズ"
  $name:ko-KR: "📏 숫자 글꼴 크기"
  $name:uk-UA: "📏 Розмір шрифту цифр"
  $description: "Base font size for the clock digits (20 to 300). Each style scales this value for its own layout."
  $description:tr-TR: "Saat rakamları için temel punto büyüklüğü (20 ile 300 arası). Her stil bu değeri kendi yerleşimine göre ölçekler."
  $description:zh-CN: "时钟数字的基本字体大小（20 到 300）。每个样式会根据自身布局缩放此值。"
  $description:zh-TW: "時鐘數字的基本字型大小（20 到 300）。每個樣式會根據自身版面配置縮放此值。"
  $description:ru-RU: "Базовый размер шрифта для цифр часов (от 20 до 300). Каждый стиль масштабирует это значение под собственный макет."
  $description:de-DE: "Basis-Schriftgröße für die Ziffern (20 bis 300). Jeder Stil skaliert diesen Wert für sein eigenes Layout."
  $description:es-ES: "Tamaño de fuente base para los dígitos del reloj (20 a 300). Cada estilo escala este valor para su propio diseño."
  $description:pt-BR: "Tamanho base da fonte para os dígitos do relógio (20 a 300). Cada estilo dimensiona esse valor para seu próprio layout."
  $description:fr-FR: "Taille de police de base pour les chiffres de l'horloge (20 à 300). Chaque style ajuste cette valeur à sa propre disposition."
  $description:it-IT: "Dimensione di base del carattere per le cifre dell'orologio (da 20 a 300). Ogni stile ridimensiona questo valore in base al proprio layout."
  $description:pl-PL: "Podstawowy rozmiar czcionki cyfr zegara (od 20 do 300). Każdy styl skaluje tę wartość do własnego układu."
  $description:ja-JP: "時計の数字の基本フォントサイズ（20〜300）。各スタイルは独自のレイアウトに合わせてこの値を調整します。"
  $description:ko-KR: "시계 숫자의 기본 글꼴 크기입니다(20~300). 각 스타일마다 고유한 레이아웃에 맞춰 이 값을 조정합니다."
  $description:uk-UA: "Базовий розмір шрифту для цифр годинника (від 20 до 300). Кожен стиль масштабує це значення для свого макета."
- verticalOffset: 0
  $name: "↕️ Vertical Offset (px)"
  $name:tr-TR: "↕️ Dikey Konum Ofseti (px)"
  $name:zh-CN: "↕️ 垂直偏移 (px)"
  $name:zh-TW: "↕️ 垂直位移 (px)"
  $name:ru-RU: "↕️ Вертикальное смещение (пикс.)"
  $name:de-DE: "↕️ Vertikaler Versatz (px)"
  $name:es-ES: "↕️ Desplazamiento vertical (px)"
  $name:pt-BR: "↕️ Deslocamento vertical (px)"
  $name:fr-FR: "↕️ Décalage vertical (px)"
  $name:it-IT: "↕️ Offset verticale (px)"
  $name:pl-PL: "↕️ Przesunięcie pionowe (px)"
  $name:ja-JP: "↕️ 垂直オフセット (px)"
  $name:ko-KR: "↕️ 세로 오프셋 (px)"
  $name:uk-UA: "↕️ Вертикальний зсув (пікс.)"
  $description: "Shifts the clock vertically (-200 to +400). Out-of-range values are clamped."
  $description:tr-TR: "Saati dikey olarak kaydırır (-200 ile +400 arası). Aralık dışı değerler sınırlandırılır."
  $description:zh-CN: "在垂直方向上移动时钟（-200 到 +400）。超出范围的值将被限制。"
  $description:zh-TW: "在垂直方向上位移時鐘（-200 到 +400）。超出範圍的數值將被限制。"
  $description:ru-RU: "Смещает часы по вертикали (от -200 до +400). Значения вне диапазона ограничиваются."
  $description:de-DE: "Verschiebt die Uhr vertikal (-200 bis +400). Werte außerhalb des Bereichs werden begrenzt."
  $description:es-ES: "Desplaza el reloj verticalmente (-200 a +400). Los valores fuera de rango se limitan."
  $description:pt-BR: "Desloca o relógio verticalmente (-200 a +400). Valores fora do intervalo são ajustados."
  $description:fr-FR: "Décale l'horloge verticalement (-200 à +400). Les valeurs hors limites sont ajustées."
  $description:it-IT: "Sposta l'orologio verticalmente (da -200 a +400). I valori fuori intervallo vengono limitati."
  $description:pl-PL: "Przesuwa zegar w pionie (od -200 do +400). Wartości spoza zakresu są ograniczane."
  $description:ja-JP: "時計を垂直方向に移動します（-200〜+400）。範囲外の値は制限されます。"
  $description:ko-KR: "시계를 세로 방향으로 이동합니다(-200~+400). 범위를 벗어난 값은 제한됩니다."
  $description:uk-UA: "Зсуває годинник по вертикалі (від -200 до +400). Значення за межами діапазону обмежуються."
- clockAlignment: auto
  $name: "↔️ Clock Horizontal Alignment"
  $name:tr-TR: "↔️ Saat Yatay Hizalaması"
  $name:zh-CN: "↔️ 时钟水平对齐"
  $name:zh-TW: "↔️ 時鐘水平對齊"
  $name:ru-RU: "↔️ Горизонтальное выравнивание часов"
  $name:de-DE: "↔️ Horizontale Ausrichtung der Uhr"
  $name:es-ES: "↔️ Alineación horizontal del reloj"
  $name:pt-BR: "↔️ Alinhamento horizontal do relógio"
  $name:fr-FR: "↔️ Alignement horizontal de l'horloge"
  $name:it-IT: "↔️ Allineamento orizzontale orologio"
  $name:pl-PL: "↔️ Wyrównanie poziome zegara"
  $name:ja-JP: "↔️ 時計の水平方向の配置"
  $name:ko-KR: "↔️ 시계 가로 정렬"
  $name:uk-UA: "↔️ Горизонтальне вирівнювання годинника"
  $description: "Horizontal alignment and layout of the clock on screen. Automatic uses the theme default (bottom-left for Minimalist Sonoma, center for other styles)."
  $description:tr-TR: "Saatin ekrandaki yerleşim ve hizalaması. Otomatik seçeneği tema varsayılanını kullanır (Minimalist Sonoma için sol alt, diğer stiller için merkez)."
  $description:zh-CN: "时钟在屏幕上的水平对齐与布局。“自动”将使用主题默认值（Minimalist Sonoma 为左下角，其他样式为居中）。"
  $description:zh-TW: "時鐘在螢幕上的水平對齊與版面配置。「自動」將使用佈景主題預設值（Minimalist Sonoma 為左下角，其他樣式為置中）。"
  $description:ru-RU: "Горизонтальное выравнивание и расположение часов на экране. «Автоматически» использует настройки темы по умолчанию (слева внизу для Minimalist Sonoma, по центру для остальных стилей)."
  $description:de-DE: "Horizontale Ausrichtung und Anordnung der Uhr auf dem Bildschirm. „Automatisch“ verwendet den Design-Standard (unten links für Minimalist Sonoma, mittig für andere Stile)."
  $description:es-ES: "Alineación horizontal y diseño del reloj en pantalla. Automático utiliza el valor predeterminado del tema (abajo a la izquierda para Minimalist Sonoma, centrado para otros estilos)."
  $description:pt-BR: "Alinhamento horizontal e layout do relógio na tela. Automático usa o padrão do tema (canto inferior esquerdo para Minimalist Sonoma, centro para outros estilos)."
  $description:fr-FR: "Alignement horizontal et disposition de l'horloge à l'écran. Automatique utilise le style par défaut (en bas à gauche pour Minimalist Sonoma, centré pour les autres)."
  $description:it-IT: "Allineamento orizzontale e layout dell'orologio sullo schermo. Automatico usa il valore predefinito del tema (in basso a sinistra per Minimalist Sonoma, centrato per gli altri)."
  $description:pl-PL: "Wyrównanie poziome i układ zegara na ekranie. Opcja „Automatycznie” używa domyślnego motywu (w lewym dolnym rogu dla Minimalist Sonoma, na środku dla pozostałych)."
  $description:ja-JP: "画面上の時計の水平方向の配置とレイアウト。「自動」はテーマのデフォルトを使用します（Minimalist Sonoma は左下、その他のスタイルは中央）。"
  $description:ko-KR: "화면에서 시계의 가로 정렬 및 레이아웃입니다. '자동'은 테마 기본값을 사용합니다(Minimalist Sonoma는 왼쪽 하단, 기타 스타일은 중앙)."
  $description:uk-UA: "Горизонтальне вирівнювання та макет годинника на екрані. «Автоматично» використовує значення теми за замовчуванням (внизу ліворуч для Minimalist Sonoma, по центру для інших стилів)."
  $options:
    - auto: "Automatic (Theme Default)"
    - center: "Center of Screen"
    - bottomLeft: "Bottom-Left (Sonoma Style)"
    - left: "Left Side"
    - right: "Right Side"
  $options:tr-TR:
    - auto: "Otomatik (Tema Varsayılanı)"
    - center: "Ekranın Ortası"
    - bottomLeft: "Sol Alt (Sonoma Stili)"
    - left: "Sol Taraf"
    - right: "Sağ Taraf"
  $options:zh-CN:
    - auto: "自动（主题默认）"
    - center: "屏幕居中"
    - bottomLeft: "左下角（Sonoma 样式）"
    - left: "靠左"
    - right: "靠右"
  $options:zh-TW:
    - auto: "自動（佈景主題預設）"
    - center: "螢幕置中"
    - bottomLeft: "左下角（Sonoma 樣式）"
    - left: "靠左"
    - right: "靠右"
  $options:ru-RU:
    - auto: "Автоматически (по умолчанию темы)"
    - center: "По центру экрана"
    - bottomLeft: "Слева внизу (стиль Sonoma)"
    - left: "Слева"
    - right: "Справа"
  $options:de-DE:
    - auto: "Automatisch (Design-Standard)"
    - center: "Bildschirmmitte"
    - bottomLeft: "Unten links (Sonoma-Stil)"
    - left: "Links"
    - right: "Rechts"
  $options:es-ES:
    - auto: "Automático (predeterminado del tema)"
    - center: "Centro de la pantalla"
    - bottomLeft: "Abajo a la izquierda (estilo Sonoma)"
    - left: "Lado izquierdo"
    - right: "Lado derecho"
  $options:pt-BR:
    - auto: "Automático (padrão do tema)"
    - center: "Centro da tela"
    - bottomLeft: "Canto inferior esquerdo (estilo Sonoma)"
    - left: "Lado esquerdo"
    - right: "Lado direito"
  $options:fr-FR:
    - auto: "Automatique (par défaut)"
    - center: "Centre de l'écran"
    - bottomLeft: "En bas à gauche (style Sonoma)"
    - left: "Côté gauche"
    - right: "Côté droit"
  $options:it-IT:
    - auto: "Automatico (predefinito del tema)"
    - center: "Centro dello schermo"
    - bottomLeft: "In basso a sinistra (stile Sonoma)"
    - left: "Lato sinistro"
    - right: "Lato destro"
  $options:pl-PL:
    - auto: "Automatycznie (domyślne motywu)"
    - center: "Środek ekranu"
    - bottomLeft: "Lewy dolny róg (styl Sonoma)"
    - left: "Po lewej stronie"
    - right: "Po prawej stronie"
  $options:ja-JP:
    - auto: "自動（テーマの既定値）"
    - center: "画面中央"
    - bottomLeft: "左下（Sonoma スタイル）"
    - left: "左寄せ"
    - right: "右寄せ"
  $options:ko-KR:
    - auto: "자동 (테마 기본값)"
    - center: "화면 중앙"
    - bottomLeft: "왼쪽 하단 (Sonoma 스타일)"
    - left: "왼쪽"
    - right: "오른쪽"
  $options:uk-UA:
    - auto: "Автоматично (за замовчуванням теми)"
    - center: "По центру екрана"
    - bottomLeft: "Внизу ліворуч (стиль Sonoma)"
    - left: "Ліворуч"
    - right: "Праворуч"
- showDateCapsule: true
  $name: "📅 Show Date Display"
  $name:tr-TR: "📅 Tarih Göstergesini Göster"
  $name:zh-CN: "📅 显示日期"
  $name:zh-TW: "📅 顯示日期"
  $name:ru-RU: "📅 Показывать дату"
  $name:de-DE: "📅 Datum anzeigen"
  $name:es-ES: "📅 Mostrar fecha"
  $name:pt-BR: "📅 Mostrar data"
  $name:fr-FR: "📅 Afficher la date"
  $name:it-IT: "📅 Mostra data"
  $name:pl-PL: "📅 Pokaż datę"
  $name:ja-JP: "📅 日付を表示"
  $name:ko-KR: "📅 날짜 표시"
  $name:uk-UA: "📅 Показувати дату"
  $description: "Displays the date and weekday (above or below the clock)."
  $description:tr-TR: "Tarih ve gün göstergesini görüntüler (saatin üstünde veya altında)."
  $description:zh-CN: "显示日期和星期几（位于时钟上方或下方）。"
  $description:zh-TW: "顯示日期與星期（位於時鐘上方或下方）。"
  $description:ru-RU: "Отображает дату и день недели (над или под часами)."
  $description:de-DE: "Zeigt das Datum und den Wochentag an (über oder unter der Uhr)."
  $description:es-ES: "Muestra la fecha y el día de la semana (encima o debajo del reloj)."
  $description:pt-BR: "Exibe a data e o dia da semana (acima ou abaixo do relógio)."
  $description:fr-FR: "Affiche la date et le jour de la semaine (au-dessus ou en dessous de l'horloge)."
  $description:it-IT: "Visualizza la data e il giorno della settimana (sopra o sotto l'orologio)."
  $description:pl-PL: "Wyświetla datę i dzień tygodnia (nad lub pod zegarem)."
  $description:ja-JP: "日付と曜日を表示します（時計の上または下）。"
  $description:ko-KR: "날짜와 요일을 표시합니다(시계 위 또는 아래)."
  $description:uk-UA: "Відображає дату та день тижня (над або під годинником)."
- hideDefaultWidgets: false
  $name: "🧩 Hide Default Widgets"
  $name:tr-TR: "🧩 Varsayılan Widget'ları Gizle"
  $name:zh-CN: "🧩 隐藏默认小组件"
  $name:zh-TW: "🧩 隱藏預設小工具"
  $name:ru-RU: "🧩 Скрыть стандартные виджеты"
  $name:de-DE: "🧩 Standard-Widgets ausblenden"
  $name:es-ES: "🧩 Ocultar widgets predeterminados"
  $name:pt-BR: "🧩 Ocultar widgets padrão"
  $name:fr-FR: "🧩 Masquer les widgets par défaut"
  $name:it-IT: "🧩 Nascondi widget predefiniti"
  $name:pl-PL: "🧩 Ukryj domyślne widżety"
  $name:ja-JP: "🧩 デフォルトのウィジェットを非表示にする"
  $name:ko-KR: "🧩 기본 위젯 숨기기"
  $name:uk-UA: "🧩 Приховати стандартні віджети"
  $description: "Hides MSN weather, sports, and news widget areas on the lock screen. Disabled by default so the mod only restyles the clock unless asked otherwise."
  $description:tr-TR: "Kilit ekranındaki MSN hava durumu, spor ve haber widget alanını gizler. Mod varsayılan olarak yalnızca saati yeniden stillendirir; widget'ları gizlemek için bu seçeneği açın."
  $description:zh-CN: "隐藏锁屏上的 MSN 天气、体育和新闻小组件区域。默认禁用，以便模组仅重新设计时钟样式，除非另有指定。"
  $description:zh-TW: "隱藏鎖定畫面上的 MSN 天氣、運動與新聞小工具區域。預設為停用，除非另有指定，否則模組僅變更時鐘樣式。"
  $description:ru-RU: "Скрывает области виджетов погоды MSN, спорта и новостей на экране блокировки. Отключено по умолчанию, чтобы мод изменял только часы, если не указано иное."
  $description:de-DE: "Blendet MSN-Wetter-, Sport- und Nachrichten-Widgets auf dem Sperrbildschirm aus. Standardmäßig deaktiviert, damit der Mod nur die Uhr anpasst, sofern nicht anders gewünscht."
  $description:es-ES: "Oculta las áreas de widgets de clima, deportes y noticias de MSN en la pantalla de bloqueo. Desactivado de forma predeterminada para que el mod solo rediseñe el reloj a menos que se indique lo contrario."
  $description:pt-BR: "Oculta as áreas de widgets de clima, esportes e notícias do MSN na tela de bloqueio. Desativado por padrão para que o mod apenas estilize o relógio, a menos que solicitado o contrário."
  $description:fr-FR: "Masque les zones de widgets météo, sports et actualités MSN sur l'écran de verrouillage. Désactivé par défaut."
  $description:it-IT: "Nasconde le aree dei widget meteo, sport e notizie MSN nella schermata di blocco. Disabilitato per impostazione predefinita."
  $description:pl-PL: "Ukrywa obszary widżetów pogody MSN, wiadomości i sportu na ekranie blokady. Domyślnie wyłączone."
  $description:ja-JP: "ロック画面上の MSN 天気、スポーツ、ニュースのウィジェット領域を非表示にします。指定がない限り時計のスタイル変更のみを行うよう、デフォルトでは無効になっています。"
  $description:ko-KR: "잠금 화면의 MSN 날씨, 스포츠, 뉴스 위젯 영역을 숨깁니다. 별도로 설정하지 않는 한 시계 스타일만 변경되도록 기본적으로 비활성화되어 있습니다."
  $description:uk-UA: "Приховує області віджетів погоди MSN, спорту та новин на екрані блокування. За замовчуванням вимкнено, щоб мод змінював лише стиль годинника, якщо не вказано інше."
- showBatteryStatus: false
  $name: "🔋 Show Battery & Charging Capsule"
  $name:tr-TR: "🔋 Pil ve Şarj Kapsülünü Göster"
  $name:zh-CN: "🔋 显示电池与充电胶囊"
  $name:zh-TW: "🔋 顯示電池與充電膠囊"
  $name:ru-RU: "🔋 Показывать капсулу батареи и зарядки"
  $name:de-DE: "🔋 Akku- und Ladekapsel anzeigen"
  $name:es-ES: "🔋 Mostrar cápsula de batería y carga"
  $name:pt-BR: "🔋 Mostrar cápsula de bateria e carregamento"
  $name:fr-FR: "🔋 Afficher la capsule de batterie et de charge"
  $name:it-IT: "🔋 Mostra capsula batteria e ricarica"
  $name:pl-PL: "🔋 Pokaż kapsułę baterii i ładowania"
  $name:ja-JP: "🔋 バッテリーと充電のカプセルを表示"
  $name:ko-KR: "🔋 배터리 및 충전 캡슐 표시"
  $name:uk-UA: "🔋 Показувати капсулу батареї та заряджання"
  $description: "Displays battery percentage and charging status on laptops (automatically hidden on desktop PCs)."
  $description:tr-TR: "Dizüstü bilgisayarlarda pil yüzdesi ve şarj durumu kapsülünü görüntüler (masaüstü bilgisayarlarda otomatik gizlenir)."
  $description:zh-CN: "在笔记本电脑上显示电池百分比和充电状态（在台式机上自动隐藏）。"
  $description:zh-TW: "在筆記型電腦上顯示電池百分比與充電狀態（在桌上型電腦上會自動隱藏）。"
  $description:ru-RU: "Отображает процент заряда батареи и статус зарядки на ноутбуках (на настольных ПК скрывается автоматически)."
  $description:de-DE: "Zeigt den Akkustand und Ladestatus auf Laptops an (auf Desktop-PCs automatisch ausgeblendet)."
  $description:es-ES: "Muestra el porcentaje de batería y el estado de carga en portátiles (se oculta automáticamente en PC de escritorio)."
  $description:pt-BR: "Exibe a porcentagem da bateria e o status de carregamento em notebooks (oculto automaticamente em PCs desktop)."
  $description:fr-FR: "Affiche le pourcentage de batterie et l'état de charge sur les ordinateurs portables (automatiquement masqué sur les PC fixes)."
  $description:it-IT: "Visualizza la percentuale della batteria e lo stato di ricarica sui laptop (nascosto automaticamente sui PC desktop)."
  $description:pl-PL: "Wyświetla procent baterii i stan ładowania na laptopach (automatycznie ukrywane na komputerach stacjonarnych)."
  $description:ja-JP: "ノートパソコンでバッテリー残量と充電状態を表示します（デスクトップ PC では自動的に非表示になります）。"
  $description:ko-KR: "노트북에서 배터리 백분율과 충전 상태를 표시합니다(데스크톱 PC에서는 자동으로 숨겨집니다)."
  $description:uk-UA: "Відображає відсоток заряду батареї та статус заряджання на ноутбуках (на настільних ПК автоматично приховується)."
- pixelSettings:
  - dotShape: circle
    $name: "🟦 Dot Shape"
    $name:tr-TR: "🟦 Nokta Şekli"
    $name:zh-CN: "🟦 点阵形状"
    $name:zh-TW: "🟦 點陣形狀"
    $name:ru-RU: "🟦 Форма точек"
    $name:de-DE: "🟦 Punktform"
    $name:es-ES: "🟦 Forma de los puntos"
    $name:pt-BR: "🟦 Formato dos pontos"
    $name:fr-FR: "🟦 Forme des points"
    $name:it-IT: "🟦 Forma dei punti"
    $name:pl-PL: "🟦 Kształt punktów"
    $name:ja-JP: "🟦 ドット形状"
    $name:ko-KR: "🟦 도트 모양"
    $name:uk-UA: "🟦 Форма точок"
    $description: "Shape of each LED dot in the Retro Pixel matrix."
    $description:tr-TR: "Retro Pixel matrisindeki her bir LED noktanın şekli."
    $description:zh-CN: "Retro Pixel 点阵中每个 LED 点的形状。"
    $description:zh-TW: "Retro Pixel 點陣中每個 LED 點的形狀。"
    $description:ru-RU: "Форма каждого светодиодного элемента в матрице Retro Pixel."
    $description:de-DE: "Form der einzelnen LED-Punkte in der Retro Pixel-Matrix."
    $description:es-ES: "Forma de cada punto LED en la matriz Retro Pixel."
    $description:pt-BR: "Formato de cada ponto LED na matriz Retro Pixel."
    $description:fr-FR: "Forme de chaque point LED dans la matrice Retro Pixel."
    $description:it-IT: "Forma di ciascun punto LED nella matrice Retro Pixel."
    $description:pl-PL: "Kształt każdego punktu LED w matrycy Retro Pixel."
    $description:ja-JP: "Retro Pixel マトリックス内の各 LED ドットの形状。"
    $description:ko-KR: "Retro Pixel 매트릭스의 각 LED 도트 모양입니다."
    $description:uk-UA: "Форма кожної світлодіодної точки у матриці Retro Pixel."
    $options:
      - circle: "Circle (Default)"
      - square: "Square"
      - rounded: "Rounded Square"
    $options:tr-TR:
      - circle: "Yuvarlak (Varsayılan)"
      - square: "Kare"
      - rounded: "Yuvarlatılmış Kare"
    $options:zh-CN:
      - circle: "圆形（默认）"
      - square: "方形"
      - rounded: "圆角方形"
    $options:zh-TW:
      - circle: "圓形（預設）"
      - square: "方形"
      - rounded: "圓角方形"
    $options:ru-RU:
      - circle: "Круг (по умолчанию)"
      - square: "Квадрат"
      - rounded: "Скруглённый квадрат"
    $options:de-DE:
      - circle: "Kreis (Standard)"
      - square: "Quadrat"
      - rounded: "Abgerundetes Quadrat"
    $options:es-ES:
      - circle: "Círculo (Predeterminado)"
      - square: "Cuadrado"
      - rounded: "Cuadrado redondeado"
    $options:pt-BR:
      - circle: "Círculo (Padrão)"
      - square: "Quadrado"
      - rounded: "Quadrado arredondado"
    $options:fr-FR:
      - circle: "Cercle (Par défaut)"
      - square: "Carré"
      - rounded: "Carré arrondi"
    $options:it-IT:
      - circle: "Cerchio (Predefinito)"
      - square: "Quadrato"
      - rounded: "Quadrato arrotondato"
    $options:pl-PL:
      - circle: "Koło (Domyślne)"
      - square: "Kwadrat"
      - rounded: "Zaokrąglony kwadrat"
    $options:ja-JP:
      - circle: "円形（既定）"
      - square: "正方形"
      - rounded: "角丸正方形"
    $options:ko-KR:
      - circle: "원형 (기본값)"
      - square: "정사각형"
      - rounded: "둥근 사각형"
    $options:uk-UA:
      - circle: "Коло (За замовчуванням)"
      - square: "Квадрат"
      - rounded: "Заокруглений квадрат"
  - ledColor: default
    $name: "💡 Active LED Color"
    $name:tr-TR: "💡 Aktif LED Rengi"
    $name:zh-CN: "💡 激活 LED 颜色"
    $name:zh-TW: "💡 亮起 LED 色彩"
    $name:ru-RU: "💡 Цвет активных LED"
    $name:de-DE: "💡 Aktive LED-Farbe"
    $name:es-ES: "💡 Color de LED activo"
    $name:pt-BR: "💡 Cor do LED ativo"
    $name:fr-FR: "💡 Couleur des LED actives"
    $name:it-IT: "💡 Colore LED attivo"
    $name:pl-PL: "💡 Kolor aktywnych LED"
    $name:ja-JP: "💡 アクティブ LED カラー"
    $name:ko-KR: "💡 활성 LED 색상"
    $name:uk-UA: "💡 Колір активних LED"
    $description: "Color of the active LEDs in the Retro Pixel matrix."
    $description:tr-TR: "Retro Pixel matrisindeki aktif LED'lerin rengi."
    $description:zh-CN: "Retro Pixel 点阵中激活 LED 的颜色。"
    $description:zh-TW: "Retro Pixel 點陣中亮起 LED 的色彩。"
    $description:ru-RU: "Цвет активных светодиодов в матрице Retro Pixel."
    $description:de-DE: "Farbe der aktiven LEDs in der Retro Pixel-Matrix."
    $description:es-ES: "Color de los LED activos en la matriz Retro Pixel."
    $description:pt-BR: "Cor dos LEDs ativos na matriz Retro Pixel."
    $description:fr-FR: "Couleur des LED actives dans la matrice Retro Pixel."
    $description:it-IT: "Colore dei LED attivi nella matrice Retro Pixel."
    $description:pl-PL: "Kolor aktywnych diod LED w matrycy Retro Pixel."
    $description:ja-JP: "Retro Pixel マトリックス内のアクティブ LED の色。"
    $description:ko-KR: "Retro Pixel 매트릭스의 활성 LED 색상입니다."
    $description:uk-UA: "Колір активних світлодіодів у матриці Retro Pixel."
    $options:
      - default: "🎨 Theme Default (White)"
      - white: "⚪ Pure White"
      - black: "⚫ Black"
      - red: "🔴 Red"
      - orange: "🟠 Orange"
      - yellow: "🟡 Yellow"
      - silver: "🩶 Soft Silver"
      - cyan: "🩵 Electric Cyan"
      - blue: "🔵 Sky Blue"
      - green: "🟢 Matrix Green"
      - amber: "🔶 Warm Amber"
      - coral: "🪸 Coral Red"
      - magenta: "🩷 Neon Magenta"
      - violet: "🟣 Soft Violet"
    $options:tr-TR:
      - default: "🎨 Tema Varsayılanı (Beyaz)"
      - white: "⚪ Saf Beyaz"
      - black: "⚫ Siyah"
      - red: "🔴 Kırmızı"
      - orange: "🟠 Turuncu"
      - yellow: "🟡 Sarı"
      - silver: "🩶 Yumuşak Gümüş"
      - cyan: "🩵 Elektrik Camgöbeği (Cyan)"
      - blue: "🔵 Gök Mavisi"
      - green: "🟢 Matrix Yeşili"
      - amber: "🔶 Sıcak Kehribar"
      - coral: "🪸 Mercan Kırmızısı"
      - magenta: "🩷 Neon Eflatun (Magenta)"
      - violet: "🟣 Yumuşak Mor"
    $options:zh-CN:
      - default: "🎨 主题默认（白色）"
      - white: "⚪ 纯白"
      - black: "⚫ 黑色"
      - red: "🔴 红色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黄色"
      - silver: "🩶 柔和银"
      - cyan: "🩵 电光青"
      - blue: "🔵 天蓝"
      - green: "🟢 矩阵绿"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚红"
      - magenta: "🩷 霓虹品红"
      - violet: "🟣 柔紫"
    $options:zh-TW:
      - default: "🎨 佈景主題預設（白色）"
      - white: "⚪ 純白"
      - black: "⚫ 黑色"
      - red: "🔴 紅色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黃色"
      - silver: "🩶 柔和銀"
      - cyan: "🩵 電光青"
      - blue: "🔵 天藍"
      - green: "🟢 矩陣綠"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚紅"
      - magenta: "🩷 霓虹洋紅"
      - violet: "🟣 柔紫"
    $options:ru-RU:
      - default: "🎨 По умолчанию темы (белый)"
      - white: "⚪ Чистый белый"
      - black: "⚫ Чёрный"
      - red: "🔴 Красный"
      - orange: "🟠 Оранжевый"
      - yellow: "🟡 Жёлтый"
      - silver: "🩶 Мягкое серебро"
      - cyan: "🩵 Электрический циан"
      - blue: "🔵 Небесно-голубой"
      - green: "🟢 Матричный зелёный"
      - amber: "🔶 Тёплый янтарный"
      - coral: "🪸 Кораллово-красный"
      - magenta: "🩷 Неоновая маджента"
      - violet: "🟣 Мягкий фиолетовый"
    $options:de-DE:
      - default: "🎨 Design-Standard (Weiß)"
      - white: "⚪ Reinweiß"
      - black: "⚫ Schwarz"
      - red: "🔴 Rot"
      - orange: "🟠 Orange"
      - yellow: "🟡 Gelb"
      - silver: "🩶 Sanftes Silber"
      - cyan: "🩵 Elektrisches Cyan"
      - blue: "🔵 Himmelblau"
      - green: "🟢 Matrix-Grün"
      - amber: "🔶 Warmes Bernstein"
      - coral: "🪸 Korallenrot"
      - magenta: "🩷 Neon-Magenta"
      - violet: "🟣 Sanftes Violett"
    $options:es-ES:
      - default: "🎨 Predeterminado del tema (Blanco)"
      - white: "⚪ Blanco puro"
      - black: "⚫ Negro"
      - red: "🔴 Rojo"
      - orange: "🟠 Naranja"
      - yellow: "🟡 Amarillo"
      - silver: "🩶 Plata suave"
      - cyan: "🩵 Cian eléctrico"
      - blue: "🔵 Azul cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ámbar cálido"
      - coral: "🪸 Rojo coral"
      - magenta: "🩷 Magenta neón"
      - violet: "🟣 Violeta suave"
    $options:pt-BR:
      - default: "🎨 Padrão do tema (Branco)"
      - white: "⚪ Branco puro"
      - black: "⚫ Preto"
      - red: "🔴 Vermelho"
      - orange: "🟠 Laranja"
      - yellow: "🟡 Amarelo"
      - silver: "🩶 Prata suave"
      - cyan: "🩵 Ciano elétrico"
      - blue: "🔵 Azul celeste"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Âmbar quente"
      - coral: "🪸 Vermelho coral"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Violeta suave"
    $options:fr-FR:
      - default: "🎨 Par défaut du thème (Blanc)"
      - white: "⚪ Blanc pur"
      - black: "⚫ Noir"
      - red: "🔴 Rouge"
      - orange: "🟠 Orange"
      - yellow: "🟡 Jaune"
      - silver: "🩶 Argent doux"
      - cyan: "🩵 Cyan électrique"
      - blue: "🔵 Bleu ciel"
      - green: "🟢 Vert Matrix"
      - amber: "🔶 Ambre chaud"
      - coral: "🪸 Rouge corail"
      - magenta: "🩷 Magenta néon"
      - violet: "🟣 Violet doux"
    $options:it-IT:
      - default: "🎨 Predefinito del tema (Bianco)"
      - white: "⚪ Bianco puro"
      - black: "⚫ Nero"
      - red: "🔴 Rosso"
      - orange: "🟠 Arancione"
      - yellow: "🟡 Giallo"
      - silver: "🩶 Argento tenue"
      - cyan: "🩵 Ciano elettrico"
      - blue: "🔵 Blu cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ambra caldo"
      - coral: "🪸 Rosso corallo"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Viola tenue"
    $options:pl-PL:
      - default: "🎨 Domyślny motywu (Biel)"
      - white: "⚪ Czysta biel"
      - black: "⚫ Czarny"
      - red: "🔴 Czerwony"
      - orange: "🟠 Pomarańczowy"
      - yellow: "🟡 Żółty"
      - silver: "🩶 Miękkie srebro"
      - cyan: "🩵 Elektryczny cyjan"
      - blue: "🔵 Błękit nieba"
      - green: "🟢 Zieleń Matrix"
      - amber: "🔶 Ciepły bursztyn"
      - coral: "🪸 Koralowa czerwień"
      - magenta: "🩷 Neonowa magenta"
      - violet: "🟣 Delikatny fiolet"
    $options:ja-JP:
      - default: "🎨 テーマ既定値（白）"
      - white: "⚪ ピュアホワイト"
      - black: "⚫ ブラック"
      - red: "🔴 レッド"
      - orange: "🟠 オレンジ"
      - yellow: "🟡 イエロー"
      - silver: "🩶 ソフトシルバー"
      - cyan: "🩵 エレクトリックシアン"
      - blue: "🔵 スカイブルー"
      - green: "🟢 マトリックスグリーン"
      - amber: "🔶 ウォームアンバー"
      - coral: "🪸 コーラルレッド"
      - magenta: "🩷 ネオンマゼンタ"
      - violet: "🟣 ソフトバイオレット"
    $options:ko-KR:
      - default: "🎨 테마 기본값 (흰색)"
      - white: "⚪ 순수 흰색"
      - black: "⚫ 블랙"
      - red: "🔴 레드"
      - orange: "🟠 오렌지"
      - yellow: "🟡 옐로우"
      - silver: "🩶 부드러운 실버"
      - cyan: "🩵 일렉트릭 시안"
      - blue: "🔵 스카이 블루"
      - green: "🟢 매트릭스 그린"
      - amber: "🔶 따뜻한 앰버"
      - coral: "🪸 코랄 레드"
      - magenta: "🩷 네온 마젠타"
      - violet: "🟣 부드러운 바이올렛"
    $options:uk-UA:
      - default: "🎨 За замовчуванням теми (Білий)"
      - white: "⚪ Чистий білий"
      - black: "⚫ Чорний"
      - red: "🔴 Червоний"
      - orange: "🟠 Помаранчевий"
      - yellow: "🟡 Жовтий"
      - silver: "🩶 М'яке срібло"
      - cyan: "🩵 Електричний ціан"
      - blue: "🔵 Небесно-блакитний"
      - green: "🟢 Матричний зелений"
      - amber: "🔶 Теплий бурштиновий"
      - coral: "🪸 Коралово-червоний"
      - magenta: "🩷 Неонова маджента"
      - violet: "🟣 М'який фіолетовий"
  - showBackplate: true
    $name: "✨ Show Frosted Backplate"
    $name:tr-TR: "✨ Buzlu Arka Paneli Göster"
    $name:zh-CN: "✨ 显示磨砂背板"
    $name:zh-TW: "✨ 顯示霧面背板"
    $name:ru-RU: "✨ Показывать матовую подложку"
    $name:de-DE: "✨ Mattierte Rückwand anzeigen"
    $name:es-ES: "✨ Mostrar placa trasera esmerilada"
    $name:pt-BR: "✨ Mostrar placa traseira fosca"
    $name:fr-FR: "✨ Afficher la plaque arrière dépolie"
    $name:it-IT: "✨ Mostra piastra posteriore satinata"
    $name:pl-PL: "✨ Pokaż matowe tło"
    $name:ja-JP: "✨ フロストバックプレートを表示"
    $name:ko-KR: "✨ 반투명 백플레이트 표시"
    $name:uk-UA: "✨ Показувати матову підкладку"
    $description: "Displays the subtle dark frosted acrylic container behind the LED matrix."
    $description:tr-TR: "LED matrisin arkasındaki koyu buzlu akrilik çerçeveyi gösterir."
    $description:zh-CN: "在 LED 点阵后方显示深色磨砂亚克力背景框。"
    $description:zh-TW: "在 LED 點陣後方顯示深色霧面壓克力背景框。"
    $description:ru-RU: "Отображает тёмную матовую акриловую подложку за LED-матрицей."
    $description:de-DE: "Zeigt den dezenten dunklen mattierten Acryl-Hintergrund hinter der LED-Matrix an."
    $description:es-ES: "Muestra el contenedor acrílico esmerilado oscuro detrás de la matriz LED."
    $description:pt-BR: "Exibe o painel acrílico fosco escuro atrás da matriz de LED."
    $description:fr-FR: "Affiche le panneau acrylique dépoli sombre derrière la matrice LED."
    $description:it-IT: "Mostra il pannello acrilico satinato scuro dietro la matrice LED."
    $description:pl-PL: "Wyświetla ciemne, matowe tło akrylowe za matrycą LED."
    $description:ja-JP: "LED マトリックスの背後にダークフロストのアクリルパネルを表示します。"
    $description:ko-KR: "LED 매트릭스 뒤에 어두운 반투명 아크릴 배경을 표시합니다."
    $description:uk-UA: "Відображає темну матову акрилову підкладку за LED-матрицею."
  - backplateOpacity: 32
    $name: "🪟 Backplate Opacity"
    $name:tr-TR: "🪟 Arka Panel Opaklığı"
    $name:zh-CN: "🪟 背板不透明度"
    $name:zh-TW: "🪟 背板不透明度"
    $name:ru-RU: "🪟 Непрозрачность подложки"
    $name:de-DE: "🪟 Rückwand-Deckkraft"
    $name:es-ES: "🪟 Opacidad de placa trasera"
    $name:pt-BR: "🪟 Opacidade da placa traseira"
    $name:fr-FR: "🪟 Opacité de la plaque arrière"
    $name:it-IT: "🪟 Opacità piastra posteriore"
    $name:pl-PL: "🪟 Przezroczystość tła"
    $name:ja-JP: "🪟 バックプレート不透明度"
    $name:ko-KR: "🪟 백플레이트 불투명도"
    $name:uk-UA: "🪟 Непрозорість підкладки"
    $description: "Background opacity of the dark backplate (0 to 255; default 32)."
    $description:tr-TR: "Koyu arka panelin arka plan opaklığı (0 ile 255 arası; varsayılan 32)."
    $description:zh-CN: "深色背板的背景不透明度（0 到 255；默认 32）。"
    $description:zh-TW: "深色背板的背景不透明度（0 到 255；預設 32）。"
    $description:ru-RU: "Непрозрачность фона тёмной подложки (от 0 до 255; по умолчанию 32)."
    $description:de-DE: "Hintergrunddeckkraft der dunklen Rückwand (0 bis 255; Standard 32)."
    $description:es-ES: "Opacidad de fondo de la placa trasera oscura (0 a 255; predeterminado 32)."
    $description:pt-BR: "Opacidade de fundo da placa traseira escura (0 a 255; padrão 32)."
    $description:fr-FR: "Opacité d'arrière-plan de la plaque arrière sombre (0 à 255 ; par défaut 32)."
    $description:it-IT: "Opacità dello sfondo della piastra posteriore scura (da 0 a 255; predefinito 32)."
    $description:pl-PL: "Krycie ciemnego tła (od 0 do 255; domyślnie 32)."
    $description:ja-JP: "ダークバックプレートの背景不透明度（0〜255、既定値 32）。"
    $description:ko-KR: "어두운 백플레이트의 배경 불투명도입니다(0~255, 기본값 32)."
    $description:uk-UA: "Непрозорість фону темної підкладки (від 0 до 255; за замовчуванням 32)."
  $name: "👾 Retro Pixel / LED Matrix"
  $name:tr-TR: "👾 Retro Pixel / LED Matrix"
  $name:zh-CN: "👾 Retro Pixel / LED Matrix"
  $name:zh-TW: "👾 Retro Pixel / LED Matrix"
  $name:ru-RU: "👾 Retro Pixel / LED Matrix"
  $name:de-DE: "👾 Retro Pixel / LED Matrix"
  $name:es-ES: "👾 Retro Pixel / LED Matrix"
  $name:pt-BR: "👾 Retro Pixel / LED Matrix"
  $name:fr-FR: "👾 Retro Pixel / LED Matrix"
  $name:it-IT: "👾 Retro Pixel / LED Matrix"
  $name:pl-PL: "👾 Retro Pixel / LED Matrix"
  $name:ja-JP: "👾 Retro Pixel / LED Matrix"
  $name:ko-KR: "👾 Retro Pixel / LED Matrix"
  $name:uk-UA: "👾 Retro Pixel / LED Matrix"
- verticalSettings:
  - rowSpacing: 0
    $name: "↕️ Row Spacing Offset (px)"
    $name:tr-TR: "↕️ Satır Aralığı Ofseti (px)"
    $name:zh-CN: "↕️ 行间距偏移 (px)"
    $name:zh-TW: "↕️ 行距位移 (px)"
    $name:ru-RU: "↕️ Смещение межстрочного интервала (пикс.)"
    $name:de-DE: "↕️ Zeilenabstand-Versatz (px)"
    $name:es-ES: "↕️ Ajuste de espacio entre filas (px)"
    $name:pt-BR: "↕️ Ajuste de espaçamento entre linhas (px)"
    $name:fr-FR: "↕️ Décalage d'espacement des lignes (px)"
    $name:it-IT: "↕️ Offset spaziatura righe (px)"
    $name:pl-PL: "↕️ Przesunięcie odstępu wierszy (px)"
    $name:ja-JP: "↕️ 行間オフセット (px)"
    $name:ko-KR: "↕️ 행 간격 오프셋 (px)"
    $name:uk-UA: "↕️ Зсув міжрядкового інтервалу (пікс.)"
    $description: "Adjusts vertical gap between hour and minute rows (-60 to +120 px; 0 is theme default)."
    $description:tr-TR: "Saat ve dakika satırları arasındaki dikey boşluğu ayarlar (-60 ile +120 px; 0 tema varsayılanıdır)."
    $description:zh-CN: "调整小时行与分钟行之间的垂直间距（-60 到 +120 px；0 为主题默认值）。"
    $description:zh-TW: "調整小時列與分鐘列之間的垂直間距（-60 到 +120 px；0 為佈景主題預設值）。"
    $description:ru-RU: "Регулирует вертикальный зазор между строками часов и минут (от -60 до +120 пикс.; 0 — по умолчанию темы)."
    $description:de-DE: "Passt den vertikalen Abstand zwischen Stunden- und Minutenzeile an (-60 bis +120 px; 0 ist Design-Standard)."
    $description:es-ES: "Ajusta el espacio vertical entre las filas de hora y minuto (-60 a +120 px; 0 es el valor predeterminado)."
    $description:pt-BR: "Ajusta o espaço vertical entre as linhas de hora e minuto (-60 a +120 px; 0 é o padrão do tema)."
    $description:fr-FR: "Ajuste l'espace vertical entre les lignes des heures et des minutes (-60 à +120 px ; 0 par défaut)."
    $description:it-IT: "Regola la distanza verticale tra le righe di ore e minuti (da -60 a +120 px; 0 è il valore predefinito)."
    $description:pl-PL: "Dostosowuje pionowy odstęp między wierszami godzin i minut (od -60 do +120 px; 0 to domyślny odstęp)."
    $description:ja-JP: "時と分の行間の垂直スペースを調整します（-60〜+120 px、0 でテーマ既定値）。"
    $description:ko-KR: "시와 분 행 사이의 세로 간격을 조정합니다(-60~+120 px, 0은 테마 기본값)."
    $description:uk-UA: "Регулює вертикальний проміжок між рядками годин і хвилин (від -60 до +120 пікс.; 0 — за замовчуванням)."
  - minuteOpacity: 0
    $name: "🌗 Minute Opacity (%)"
    $name:tr-TR: "🌗 Dakika Opaklığı (%)"
    $name:zh-CN: "🌗 分钟不透明度 (%)"
    $name:zh-TW: "🌗 分鐘不透明度 (%)"
    $name:ru-RU: "🌗 Прозрачность минут (%)"
    $name:de-DE: "🌗 Minutendeckkraft (%)"
    $name:es-ES: "🌗 Opacidad de los minutos (%)"
    $name:pt-BR: "🌗 Opacidade dos minutos (%)"
    $name:fr-FR: "🌗 Opacité des minutes (%)"
    $name:it-IT: "🌗 Opacità minuti (%)"
    $name:pl-PL: "🌗 Przezroczystość minut (%)"
    $name:ja-JP: "🌗 分の不透明度 (%)"
    $name:ko-KR: "🌗 분 불투명도 (%)"
    $name:uk-UA: "🌗 Прозорість хвилин (%)"
    $description: "Overrides minute row opacity (10 to 100%; 0 uses the theme default)."
    $description:tr-TR: "Dakika satırının opaklığını belirler (%10 ile %100 arası; 0 tema varsayılanını kullanır)."
    $description:zh-CN: "覆盖分钟行的不透明度（10 到 100%；0 使用主题默认值）。"
    $description:zh-TW: "覆寫分鐘列的不透明度（10 到 100%；0 使用佈景主題預設值）。"
    $description:ru-RU: "Переопределяет непрозрачность строки минут (от 10 до 100%; 0 — по умолчанию темы)."
    $description:de-DE: "Überschreibt die Deckkraft der Minutenzeile (10 bis 100 %; 0 verwendet den Design-Standard)."
    $description:es-ES: "Anula la opacidad de la fila de minutos (10 a 100%; 0 usa el valor predeterminado)."
    $description:pt-BR: "Substitui a opacidade da linha dos minutos (10 a 100%; 0 usa o padrão do tema)."
    $description:fr-FR: "Remplace l'opacité de la ligne des minutes (10 à 100 % ; 0 utilise la valeur par défaut)."
    $description:it-IT: "Sostituisce l'opacità della riga dei minuti (10–100%; 0 usa il valore predefinito)."
    $description:pl-PL: "Nadpisuje krycie wiersza minut (10–100%; 0 używa wartości domyślnej)."
    $description:ja-JP: "分の行の不透明度を上書きします（10〜100%、0 でテーマ既定値）。"
    $description:ko-KR: "분 행의 불투명도를 재정의합니다(10~100%, 0은 테마 기본값)."
    $description:uk-UA: "Перевизначає непрозорість рядка хвилин (10–100%; 0 — за замовчуванням)."
  - textColor: default
    $name: "🎨 Digit Color"
    $name:tr-TR: "🎨 Rakam Rengi"
    $name:zh-CN: "🎨 数字颜色"
    $name:zh-TW: "🎨 數字色彩"
    $name:ru-RU: "🎨 Цвет цифр"
    $name:de-DE: "🎨 Ziffernfarbe"
    $name:es-ES: "🎨 Color de los dígitos"
    $name:pt-BR: "🎨 Cor dos dígitos"
    $name:fr-FR: "🎨 Couleur des chiffres"
    $name:it-IT: "🎨 Colore cifre"
    $name:pl-PL: "🎨 Kolor cyfr"
    $name:ja-JP: "🎨 数字カラー"
    $name:ko-KR: "🎨 숫자 색상"
    $name:uk-UA: "🎨 Колір цифр"
    $description: "Color for the vertical clock digits."
    $description:tr-TR: "Dikey saat rakamlarının rengi."
    $description:zh-CN: "垂直时钟数字的颜色。"
    $description:zh-TW: "垂直時鐘數字的色彩。"
    $description:ru-RU: "Цвет цифр вертикальных часов."
    $description:de-DE: "Farbe der vertikalen Uhrziffern."
    $description:es-ES: "Color de los dígitos del reloj vertical."
    $description:pt-BR: "Cor dos dígitos do relógio vertical."
    $description:fr-FR: "Couleur des chiffres de l'horloge verticale."
    $description:it-IT: "Colore delle cifre dell'orologio verticale."
    $description:pl-PL: "Kolor cyfr zegara pionowego."
    $description:ja-JP: "縦型時計の数字の色。"
    $description:ko-KR: "세로 시계 숫자의 색상입니다."
    $description:uk-UA: "Колір цифр вертикального годинника."
    $options:
      - default: "🎨 Theme Default (White)"
      - white: "⚪ Pure White"
      - black: "⚫ Black"
      - red: "🔴 Red"
      - orange: "🟠 Orange"
      - yellow: "🟡 Yellow"
      - silver: "🩶 Soft Silver"
      - cyan: "🩵 Electric Cyan"
      - blue: "🔵 Sky Blue"
      - green: "🟢 Matrix Green"
      - amber: "🔶 Warm Amber"
      - coral: "🪸 Coral Red"
      - magenta: "🩷 Neon Magenta"
      - violet: "🟣 Soft Violet"
    $options:tr-TR:
      - default: "🎨 Tema Varsayılanı (Beyaz)"
      - white: "⚪ Saf Beyaz"
      - black: "⚫ Siyah"
      - red: "🔴 Kırmızı"
      - orange: "🟠 Turuncu"
      - yellow: "🟡 Sarı"
      - silver: "🩶 Yumuşak Gümüş"
      - cyan: "🩵 Elektrik Camgöbeği (Cyan)"
      - blue: "🔵 Gök Mavisi"
      - green: "🟢 Matrix Yeşili"
      - amber: "🔶 Sıcak Kehribar"
      - coral: "🪸 Mercan Kırmızısı"
      - magenta: "🩷 Neon Eflatun (Magenta)"
      - violet: "🟣 Yumuşak Mor"
    $options:zh-CN:
      - default: "🎨 主题默认（白色）"
      - white: "⚪ 纯白"
      - black: "⚫ 黑色"
      - red: "🔴 红色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黄色"
      - silver: "🩶 柔和银"
      - cyan: "🩵 电光青"
      - blue: "🔵 天蓝"
      - green: "🟢 矩阵绿"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚红"
      - magenta: "🩷 霓虹品红"
      - violet: "🟣 柔紫"
    $options:zh-TW:
      - default: "🎨 佈景主題預設（白色）"
      - white: "⚪ 純白"
      - black: "⚫ 黑色"
      - red: "🔴 紅色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黃色"
      - silver: "🩶 柔和銀"
      - cyan: "🩵 電光青"
      - blue: "🔵 天藍"
      - green: "🟢 矩陣綠"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚紅"
      - magenta: "🩷 霓虹洋紅"
      - violet: "🟣 柔紫"
    $options:ru-RU:
      - default: "🎨 По умолчанию темы (белый)"
      - white: "⚪ Чистый белый"
      - black: "⚫ Чёрный"
      - red: "🔴 Красный"
      - orange: "🟠 Оранжевый"
      - yellow: "🟡 Жёлтый"
      - silver: "🩶 Мягкое серебро"
      - cyan: "🩵 Электрический циан"
      - blue: "🔵 Небесно-голубой"
      - green: "🟢 Матричный зелёный"
      - amber: "🔶 Тёплый янтарный"
      - coral: "🪸 Кораллово-красный"
      - magenta: "🩷 Неоновая маджента"
      - violet: "🟣 Мягкий фиолетовый"
    $options:de-DE:
      - default: "🎨 Design-Standard (Weiß)"
      - white: "⚪ Reinweiß"
      - black: "⚫ Schwarz"
      - red: "🔴 Rot"
      - orange: "🟠 Orange"
      - yellow: "🟡 Gelb"
      - silver: "🩶 Sanftes Silber"
      - cyan: "🩵 Elektrisches Cyan"
      - blue: "🔵 Himmelblau"
      - green: "🟢 Matrix-Grün"
      - amber: "🔶 Warmes Bernstein"
      - coral: "🪸 Korallenrot"
      - magenta: "🩷 Neon-Magenta"
      - violet: "🟣 Sanftes Violett"
    $options:es-ES:
      - default: "🎨 Predeterminado del tema (Blanco)"
      - white: "⚪ Blanco puro"
      - black: "⚫ Negro"
      - red: "🔴 Rojo"
      - orange: "🟠 Naranja"
      - yellow: "🟡 Amarillo"
      - silver: "🩶 Plata suave"
      - cyan: "🩵 Cian eléctrico"
      - blue: "🔵 Azul cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ámbar cálido"
      - coral: "🪸 Rojo coral"
      - magenta: "🩷 Magenta neón"
      - violet: "🟣 Violeta suave"
    $options:pt-BR:
      - default: "🎨 Padrão do tema (Branco)"
      - white: "⚪ Branco puro"
      - black: "⚫ Preto"
      - red: "🔴 Vermelho"
      - orange: "🟠 Laranja"
      - yellow: "🟡 Amarelo"
      - silver: "🩶 Prata suave"
      - cyan: "🩵 Ciano elétrico"
      - blue: "🔵 Azul celeste"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Âmbar quente"
      - coral: "🪸 Vermelho coral"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Violeta suave"
    $options:fr-FR:
      - default: "🎨 Par défaut du thème (Blanc)"
      - white: "⚪ Blanc pur"
      - black: "⚫ Noir"
      - red: "🔴 Rouge"
      - orange: "🟠 Orange"
      - yellow: "🟡 Jaune"
      - silver: "🩶 Argent doux"
      - cyan: "🩵 Cyan électrique"
      - blue: "🔵 Bleu ciel"
      - green: "🟢 Vert Matrix"
      - amber: "🔶 Ambre chaud"
      - coral: "🪸 Rouge corail"
      - magenta: "🩷 Magenta néon"
      - violet: "🟣 Violet doux"
    $options:it-IT:
      - default: "🎨 Predefinito del tema (Bianco)"
      - white: "⚪ Bianco puro"
      - black: "⚫ Nero"
      - red: "🔴 Rosso"
      - orange: "🟠 Arancione"
      - yellow: "🟡 Giallo"
      - silver: "🩶 Argento tenue"
      - cyan: "🩵 Ciano elettrico"
      - blue: "🔵 Blu cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ambra caldo"
      - coral: "🪸 Rosso corallo"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Viola tenue"
    $options:pl-PL:
      - default: "🎨 Domyślny motywu (Biel)"
      - white: "⚪ Czysta biel"
      - black: "⚫ Czarny"
      - red: "🔴 Czerwony"
      - orange: "🟠 Pomarańczowy"
      - yellow: "🟡 Żółty"
      - silver: "🩶 Miękkie srebro"
      - cyan: "🩵 Elektryczny cyjan"
      - blue: "🔵 Błękit nieba"
      - green: "🟢 Zieleń Matrix"
      - amber: "🔶 Ciepły bursztyn"
      - coral: "🪸 Koralowa czerwień"
      - magenta: "🩷 Neonowa magenta"
      - violet: "🟣 Delikatny fiolet"
    $options:ja-JP:
      - default: "🎨 テーマ既定値（白）"
      - white: "⚪ ピュアホワイト"
      - black: "⚫ ブラック"
      - red: "🔴 レッド"
      - orange: "🟠 オレンジ"
      - yellow: "🟡 イエロー"
      - silver: "🩶 ソフトシルバー"
      - cyan: "🩵 エレクトリックシアン"
      - blue: "🔵 スカイブルー"
      - green: "🟢 マトリックスグリーン"
      - amber: "🔶 ウォームアンバー"
      - coral: "🪸 コーラルレッド"
      - magenta: "🩷 ネオンマゼンタ"
      - violet: "🟣 ソフトバイオレット"
    $options:ko-KR:
      - default: "🎨 테마 기본값 (흰색)"
      - white: "⚪ 순수 흰색"
      - black: "⚫ 블랙"
      - red: "🔴 레드"
      - orange: "🟠 오렌지"
      - yellow: "🟡 옐로우"
      - silver: "🩶 부드러운 실버"
      - cyan: "🩵 일렉트릭 시안"
      - blue: "🔵 스카이 블루"
      - green: "🟢 매트릭스 그린"
      - amber: "🔶 따뜻한 앰버"
      - coral: "🪸 코랄 레드"
      - magenta: "🩷 네온 마젠타"
      - violet: "🟣 부드러운 바이올렛"
    $options:uk-UA:
      - default: "🎨 За замовчуванням теми (Білий)"
      - white: "⚪ Чистий білий"
      - black: "⚫ Чорний"
      - red: "🔴 Червоний"
      - orange: "🟠 Помаранчевий"
      - yellow: "🟡 Жовтий"
      - silver: "🩶 М'яке срібло"
      - cyan: "🩵 Електричний ціан"
      - blue: "🔵 Небесно-блакитний"
      - green: "🟢 Матричний зелений"
      - amber: "🔶 Теплий бурштиновий"
      - coral: "🪸 Коралово-червоний"
      - magenta: "🩷 Неонова маджента"
      - violet: "🟣 М'який фіолетовий"
  - fontFamily: default
    $name: "🔤 Font Family"
    $name:tr-TR: "🔤 Yazı Tipi"
    $name:zh-CN: "🔤 字体"
    $name:zh-TW: "🔤 字型"
    $name:ru-RU: "🔤 Шрифт"
    $name:de-DE: "🔤 Schriftart"
    $name:es-ES: "🔤 Fuente"
    $name:pt-BR: "🔤 Fonte"
    $name:fr-FR: "🔤 Police"
    $name:it-IT: "🔤 Carattere"
    $name:pl-PL: "🔤 Czcionka"
    $name:ja-JP: "🔤 フォント"
    $name:ko-KR: "🔤 글꼴"
    $name:uk-UA: "🔤 Шрифт"
    $description: "Font used for the vertical clock digits."
    $description:tr-TR: "Dikey saat rakamları için kullanılacak yazı tipi."
    $description:zh-CN: "垂直时钟数字使用的字体。"
    $description:zh-TW: "垂直時鐘數字使用的字型。"
    $description:ru-RU: "Шрифт для цифр вертикальных часов."
    $description:de-DE: "Schriftart für die vertikalen Uhrziffern."
    $description:es-ES: "Fuente utilizada para los dígitos del reloj vertical."
    $description:pt-BR: "Fonte usada para os dígitos do relógio vertical."
    $description:fr-FR: "Police utilisée pour les chiffres de l'horloge verticale."
    $description:it-IT: "Carattere usato per le cifre dell'orologio verticale."
    $description:pl-PL: "Czcionka używana dla cyfr zegara pionowego."
    $description:ja-JP: "縦型時計の数字に使用するフォント。"
    $description:ko-KR: "세로 시계 숫자에 사용할 글꼴입니다."
    $description:uk-UA: "Шрифт для цифр вертикального годинника."
    $options:
      - default: "Theme Default"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:tr-TR:
      - default: "Tema Varsayılanı"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:zh-CN:
      - default: "主题默认"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:zh-TW:
      - default: "佈景主題預設"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ru-RU:
      - default: "По умолчанию темы"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:de-DE:
      - default: "Design-Standard"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:es-ES:
      - default: "Predeterminado del tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:pt-BR:
      - default: "Padrão do tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:fr-FR:
      - default: "Par défaut du thème"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:it-IT:
      - default: "Predefinito del tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:pl-PL:
      - default: "Domyślna motywu"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ja-JP:
      - default: "テーマ既定値"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ko-KR:
      - default: "테마 기본값"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:uk-UA:
      - default: "За замовчуванням теми"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
  - showShadow: true
    $name: "🌫️ Show Drop Shadow"
    $name:tr-TR: "🌫️ Alt Gölgeyi Göster"
    $name:zh-CN: "🌫️ 显示投影"
    $name:zh-TW: "🌫️ 顯示陰影"
    $name:ru-RU: "🌫️ Показывать тень"
    $name:de-DE: "🌫️ Schlagschatten anzeigen"
    $name:es-ES: "🌫️ Mostrar sombra"
    $name:pt-BR: "🌫️ Mostrar sombra"
    $name:fr-FR: "🌫️ Afficher l'ombre portée"
    $name:it-IT: "🌫️ Mostra ombra"
    $name:pl-PL: "🌫️ Pokaż cień"
    $name:ja-JP: "🌫️ ドロップシャドウを表示"
    $name:ko-KR: "🌫️ 그림자 표시"
    $name:uk-UA: "🌫️ Показувати тінь"
    $description: "Enables the ambient drop-shadow layer behind vertical digits."
    $description:tr-TR: "Dikey rakamların arkasındaki yumuşak gölge katmanını etkinleştirir."
    $description:zh-CN: "启用垂直数字后方的柔和投影层。"
    $description:zh-TW: "啟用垂直數字後方的柔和陰影層。"
    $description:ru-RU: "Включает мягкую тень позади вертикальных цифр."
    $description:de-DE: "Aktiviert die dezente Schattenebene hinter den vertikalen Ziffern."
    $description:es-ES: "Activa la capa de sombra suave detrás de los dígitos verticales."
    $description:pt-BR: "Ativa a camada de sombra suave atrás dos dígitos verticais."
    $description:fr-FR: "Active l'ombre portée douce derrière les chiffres verticaux."
    $description:it-IT: "Attiva il livello d'ombra dietro le cifre verticali."
    $description:pl-PL: "Włącza delikatny cień za pionowymi cyframi."
    $description:ja-JP: "縦型数字の背後のドロップシャドウを有効にします。"
    $description:ko-KR: "세로 숫자 뒤의 그림자 효과를 활성화합니다."
    $description:uk-UA: "Вмикає м'яку тінь позаду вертикальних цифр."
  $name: "📱 Vertical Stack Clocks"
  $name:tr-TR: "📱 Dikey Yığın Saatler (Vertical Stack)"
  $name:zh-CN: "📱 垂直堆叠时钟 (Vertical Stack)"
  $name:zh-TW: "📱 垂直堆疊時鐘 (Vertical Stack)"
  $name:ru-RU: "📱 Вертикальные часы (Vertical Stack)"
  $name:de-DE: "📱 Vertikale Uhren (Vertical Stack)"
  $name:es-ES: "📱 Relojes verticales (Vertical Stack)"
  $name:pt-BR: "📱 Relógios verticais (Vertical Stack)"
  $name:fr-FR: "📱 Horloges empilées (Vertical Stack)"
  $name:it-IT: "📱 Orologi verticali (Vertical Stack)"
  $name:pl-PL: "📱 Zegary pionowe (Vertical Stack)"
  $name:ja-JP: "📱 縦型スタック時計 (Vertical Stack)"
  $name:ko-KR: "📱 세로 스택 시계 (Vertical Stack)"
  $name:uk-UA: "📱 Вертикальні годинники (Vertical Stack)"
- ansiSettings:
  - colorPreset: amber
    $name: "🟩 Terminal Color Theme"
    $name:tr-TR: "🟩 Terminal Renk Teması"
    $name:zh-CN: "🟩 终端配色方案"
    $name:zh-TW: "🟩 終端機色彩配置"
    $name:ru-RU: "🟩 Цветовая схема терминала"
    $name:de-DE: "🟩 Terminal-Farbschema"
    $name:es-ES: "🟩 Tema de color de terminal"
    $name:pt-BR: "🟩 Tema de cores do terminal"
    $name:fr-FR: "🟩 Thème de couleur du terminal"
    $name:it-IT: "🟩 Tema colore terminale"
    $name:pl-PL: "🟩 Motyw kolorystyczny terminala"
    $name:ja-JP: "🟩 ターミナルカラーテーマ"
    $name:ko-KR: "🟩 터미널 색상 테마"
    $name:uk-UA: "🟩 Колірна тема термінала"
    $description: "Phosphor color palette for the ANSI Shadow ASCII banner."
    $description:tr-TR: "ANSI Shadow ASCII saati için fosfor renk paleti."
    $description:zh-CN: "ANSI Shadow ASCII 时钟的荧光配色方案。"
    $description:zh-TW: "ANSI Shadow ASCII 時鐘的螢光色彩配置。"
    $description:ru-RU: "Фосфорная цветовая палитра для ASCII-часов ANSI Shadow."
    $description:de-DE: "Phosphor-Farbpalette für die ANSI Shadow ASCII-Uhr."
    $description:es-ES: "Paleta de fósforo para el reloj ASCII ANSI Shadow."
    $description:pt-BR: "Paleta de fósforo para o relógio ASCII ANSI Shadow."
    $description:fr-FR: "Palette phosphore pour l'horloge ASCII ANSI Shadow."
    $description:it-IT: "Tavolozza fosforo per l'orologio ASCII ANSI Shadow."
    $description:pl-PL: "Paleta fosforowa dla zegara ASCII ANSI Shadow."
    $description:ja-JP: "ANSI Shadow ASCII 時計の蛍光カラーパレット。"
    $description:ko-KR: "ANSI Shadow ASCII 시계의 인광 색상 팔레트입니다."
    $description:uk-UA: "Фосфорна колірна палітра для ASCII-годинника ANSI Shadow."
    $options:
      - amber: "🔶 Amber CRT (Default)"
      - green: "🟢 Matrix Green"
      - cyan: "🩵 Cyber Cyan"
      - white: "⚪ Classic Monochrome"
    $options:tr-TR:
      - amber: "🔶 Kehribar CRT (Varsayılan)"
      - green: "🟢 Matrix Yeşili"
      - cyan: "🩵 Siber Camgöbeği"
      - white: "⚪ Klasik Beyaz"
    $options:zh-CN:
      - amber: "🔶 琥珀 CRT（默认）"
      - green: "🟢 矩阵绿"
      - cyan: "🩵 赛博青"
      - white: "⚪ 经典单色白"
    $options:zh-TW:
      - amber: "🔶 琥珀 CRT（預設）"
      - green: "🟢 矩陣綠"
      - cyan: "🩵 賽博青"
      - white: "⚪ 經典單色白"
    $options:ru-RU:
      - amber: "🔶 Янтарный ЭЛТ (по умолчанию)"
      - green: "🟢 Матричный зелёный"
      - cyan: "🩵 Кибер-циан"
      - white: "⚪ Классический белый"
    $options:de-DE:
      - amber: "🔶 Bernstein-CRT (Standard)"
      - green: "🟢 Matrix-Grün"
      - cyan: "🩵 Cyber-Cyan"
      - white: "⚪ Klassisch Weiß"
    $options:es-ES:
      - amber: "🔶 CRT ámbar (Predeterminado)"
      - green: "🟢 Verde Matrix"
      - cyan: "🩵 Cian cibernético"
      - white: "⚪ Blanco clásico"
    $options:pt-BR:
      - amber: "🔶 CRT âmbar (Padrão)"
      - green: "🟢 Verde Matrix"
      - cyan: "🩵 Ciano cibernético"
      - white: "⚪ Branco clássico"
    $options:fr-FR:
      - amber: "🔶 CRT ambre (Par défaut)"
      - green: "🟢 Vert Matrix"
      - cyan: "🩵 Cyan cyber"
      - white: "⚪ Monochrome classique"
    $options:it-IT:
      - amber: "🔶 CRT ambra (Predefinito)"
      - green: "🟢 Verde Matrix"
      - cyan: "🩵 Ciano cyber"
      - white: "⚪ Bianco classico"
    $options:pl-PL:
      - amber: "🔶 Bursztynowy CRT (Domyślne)"
      - green: "🟢 Zieleń Matrix"
      - cyan: "🩵 Cybernetyczny cyjan"
      - white: "⚪ Klasyczna biel"
    $options:ja-JP:
      - amber: "🔶 アンバー CRT（既定）"
      - green: "🟢 マトリックスグリーン"
      - cyan: "🩵 サイバーシアン"
      - white: "⚪ クラシックモノクロ"
    $options:ko-KR:
      - amber: "🔶 앰버 CRT (기본값)"
      - green: "🟢 매트릭스 그린"
      - cyan: "🩵 사이버 시안"
      - white: "⚪ 클래식 모노크롬"
    $options:uk-UA:
      - amber: "🔶 Бурштиновий ЕПТ (За замовчуванням)"
      - green: "🟢 Матричний зелений"
      - cyan: "🩵 Кібер-ціан"
      - white: "⚪ Класичний білий"
  - showPromptBar: true
    $name: "💻 Show Terminal Prompt Header"
    $name:tr-TR: "💻 Terminal Komut Satırı Başlığını Göster"
    $name:zh-CN: "💻 显示终端提示符栏"
    $name:zh-TW: "💻 顯示終端機提示字元列"
    $name:ru-RU: "💻 Показывать строку терминала"
    $name:de-DE: "💻 Terminal-Eingabeaufforderung anzeigen"
    $name:es-ES: "💻 Mostrar encabezado de terminal"
    $name:pt-BR: "💻 Mostrar cabeçalho do terminal"
    $name:fr-FR: "💻 Afficher l'en-tête d'invite du terminal"
    $name:it-IT: "💻 Mostra intestazione prompt terminale"
    $name:pl-PL: "💻 Pokaż wiersz poleceń terminala"
    $name:ja-JP: "💻 ターミナルプロンプト行を表示"
    $name:ko-KR: "💻 터미널 프롬프트 헤더 표시"
    $name:uk-UA: "💻 Показувати рядок термінала"
    $description: "Displays the PowerShell prompt line above the ASCII clock."
    $description:tr-TR: "ASCII saatin üzerinde PowerShell komut satırını görüntüler."
    $description:zh-CN: "在 ASCII 时钟上方显示 PowerShell 提示符行。"
    $description:zh-TW: "在 ASCII 時鐘上方顯示 PowerShell 提示字元列。"
    $description:ru-RU: "Отображает строку приглашения PowerShell над ASCII-часами."
    $description:de-DE: "Zeigt die PowerShell-Eingabezeile über der ASCII-Uhr an."
    $description:es-ES: "Muestra la línea de comandos de PowerShell sobre el reloj ASCII."
    $description:pt-BR: "Exibe a linha de comando do PowerShell acima do relógio ASCII."
    $description:fr-FR: "Affiche la ligne d'invite PowerShell au-dessus de l'horloge ASCII."
    $description:it-IT: "Mostra la riga del prompt di PowerShell sopra l'orologio ASCII."
    $description:pl-PL: "Wyświetla wiersz poleceń PowerShell nad zegarem ASCII."
    $description:ja-JP: "ASCII 時計の上に PowerShell プロンプト行を表示します。"
    $description:ko-KR: "ASCII 시계 위에 PowerShell 프롬프트 줄을 표시합니다."
    $description:uk-UA: "Відображає рядок запиту PowerShell над ASCII-годинником."
  - showGlow: true
    $name: "✨ Phosphor Glow Effect"
    $name:tr-TR: "✨ Fosfor Parlama Efekti"
    $name:zh-CN: "✨ 荧光发光效果"
    $name:zh-TW: "✨ 螢光發光效果"
    $name:ru-RU: "✨ Эффект фосфорного свечения"
    $name:de-DE: "✨ Phosphor-Leuchteffekt"
    $name:es-ES: "✨ Efecto de brillo de fósforo"
    $name:pt-BR: "✨ Efeito de brilho de fósforo"
    $name:fr-FR: "✨ Effet de lueur phosphore"
    $name:it-IT: "✨ Effetto bagliore fosforo"
    $name:pl-PL: "✨ Efekt fosforowej poświaty"
    $name:ja-JP: "✨ フォスファーグロー効果"
    $name:ko-KR: "✨ 인광 글로우 효과"
    $name:uk-UA: "✨ Ефект фосфорного світіння"
    $description: "Adds a soft phosphor neon glow around the ASCII digits."
    $description:tr-TR: "ASCII rakamların etrafına yumuşak bir fosfor parlama efekti ekler."
    $description:zh-CN: "在 ASCII 数字周围添加柔和的荧光发光效果。"
    $description:zh-TW: "在 ASCII 數字周圍加入柔和的螢光發光效果。"
    $description:ru-RU: "Добавляет мягкое фосфорное свечение вокруг ASCII-цифр."
    $description:de-DE: "Fügt ein weiches Phosphor-Leuchten um die ASCII-Ziffern hinzu."
    $description:es-ES: "Añade un suave brillo de fósforo alrededor de los dígitos ASCII."
    $description:pt-BR: "Adiciona um brilho suave de fósforo ao redor dos dígitos ASCII."
    $description:fr-FR: "Ajoute une douce lueur phosphore autour des chiffres ASCII."
    $description:it-IT: "Aggiunge un morbido bagliore al fosforo attorno alle cifre ASCII."
    $description:pl-PL: "Dodaje miękką fosforową poświatę wokół cyfr ASCII."
    $description:ja-JP: "ASCII 数字の周囲に柔らかな蛍光グローを追加します。"
    $description:ko-KR: "ASCII 숫자 주위에 부드러운 인광 발광 효과를 추가합니다."
    $description:uk-UA: "Додає м'яке фосфорне світіння навколо ASCII-цифр."
  - cardOpacity: 165
    $name: "🪟 Terminal Card Opacity"
    $name:tr-TR: "🪟 Terminal Kartı Opaklığı"
    $name:zh-CN: "🪟 终端卡片不透明度"
    $name:zh-TW: "🪟 終端機卡片不透明度"
    $name:ru-RU: "🪟 Непрозрачность карточки терминала"
    $name:de-DE: "🪟 Terminalkarten-Deckkraft"
    $name:es-ES: "🪟 Opacidad de tarjeta del terminal"
    $name:pt-BR: "🪟 Opacidade do cartão do terminal"
    $name:fr-FR: "🪟 Opacité de la carte du terminal"
    $name:it-IT: "🪟 Opacità scheda del terminale"
    $name:pl-PL: "🪟 Przezroczystość karty terminala"
    $name:ja-JP: "🪟 ターミナルカードの不透明度"
    $name:ko-KR: "🪟 터미널 카드 불투명도"
    $name:uk-UA: "🪟 Непрозорість картки термінала"
    $description: "Background opacity of the dark terminal card (0 to 255; set 0 to hide the card, default 165)."
    $description:tr-TR: "Koyu terminal kartının arka plan opaklığı (0 ile 255 arası; kartı gizlemek için 0 yapın, varsayılan 165)."
    $description:zh-CN: "深色终端卡片的背景不透明度（0 到 255；设为 0 可隐藏卡片，默认 165）。"
    $description:zh-TW: "深色終端機卡片的背景不透明度（0 到 255；設為 0 可隱藏卡片，預設 165）。"
    $description:ru-RU: "Непрозрачность фона тёмной карточки терминала (от 0 до 255; 0 скрывает карточку, по умолчанию 165)."
    $description:de-DE: "Hintergrunddeckkraft der dunklen Terminalkarte (0 bis 255; 0 blendet die Karte aus, Standard 165)."
    $description:es-ES: "Opacidad de fondo de la tarjeta oscura del terminal (0 a 255; 0 oculta la tarjeta, predeterminado 165)."
    $description:pt-BR: "Opacidade de fundo do cartão escuro do terminal (0 a 255; 0 oculta o cartão, padrão 165)."
    $description:fr-FR: "Opacité d'arrière-plan de la carte sombre du terminal (0 à 255 ; 0 masque la carte, par défaut 165)."
    $description:it-IT: "Opacità dello sfondo della scheda scura del terminale (da 0 a 255; 0 nasconde la scheda, predefinito 165)."
    $description:pl-PL: "Krycie tła ciemnej karty terminala (od 0 do 255; 0 ukrywa kartę, domyślnie 165)."
    $description:ja-JP: "ダークターミナルカードの背景不透明度（0〜255、0 でカードを非表示、既定値 165）。"
    $description:ko-KR: "다크 터미널 카드의 배경 불투명도입니다(0~255, 0으로 설정 시 카드 숨김, 기본값 165)."
    $description:uk-UA: "Непрозорість фону темної картки термінала (від 0 до 255; 0 приховує картку, за замовчуванням 165)."
  $name: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:tr-TR: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:zh-CN: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:zh-TW: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:ru-RU: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:de-DE: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:es-ES: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:pt-BR: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:fr-FR: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:it-IT: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:pl-PL: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:ja-JP: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:ko-KR: "🖥️ ANSI Shadow (Terminal ASCII)"
  $name:uk-UA: "🖥️ ANSI Shadow (Terminal ASCII)"
- flipSettings:
  - glassOpacity: 45
    $name: "🪟 Glass Opacity (Modern Flip Cards)"
    $name:tr-TR: "🪟 Cam Opaklığı (Modern Flip Kartları)"
    $name:zh-CN: "🪟 玻璃不透明度（Modern Flip 卡片）"
    $name:zh-TW: "🪟 玻璃不透明度（Modern Flip 卡片）"
    $name:ru-RU: "🪟 Непрозрачность стекла (карточки Modern Flip)"
    $name:de-DE: "🪟 Glas-Deckkraft (Modern Flip-Karten)"
    $name:es-ES: "🪟 Opacidad del cristal (Tarjetas Modern Flip)"
    $name:pt-BR: "🪟 Opacidade do vidro (Cartões Modern Flip)"
    $name:fr-FR: "🪟 Opacité du verre (Cartes Modern Flip)"
    $name:it-IT: "🪟 Opacità vetro (schede Modern Flip)"
    $name:pl-PL: "🪟 Nieprzezroczystość szkła (karty Modern Flip)"
    $name:ja-JP: "🪟 ガラスの不透明度（Modern Flip カード）"
    $name:ko-KR: "🪟 유리 불투명도 (Modern Flip 카드)"
    $name:uk-UA: "🪟 Непрозорість скла (картки Modern Flip)"
    $description: "Translucency of the Modern Flip clock cards (0 to 255; 0 is completely transparent, 255 is fully opaque). Other styles are not affected."
    $description:tr-TR: "Modern Flip saat kartlarının yarı saydamlığı (0 ile 255 arası; 0 tamamen saydam, 255 tam opak). Diğer stiller etkilenmez."
    $description:zh-CN: "Modern Flip 时钟卡片的半透明度（0 到 255；0 完全透明，255 完全不透明）。其他样式不受影响。"
    $description:zh-TW: "Modern Flip 時鐘卡片的半透明度（0 到 255；0 完全透明，255 完全不透明）。其他樣式不受影響。"
    $description:ru-RU: "Полупрозрачность карточек часов Modern Flip (от 0 до 255; 0 — полностью прозрачные, 255 — полностью непрозрачные). Другие стили не затрагиваются."
    $description:de-DE: "Lichtdurchlässigkeit der Modern Flip-Uhrenkarten (0 bis 255; 0 ist vollständig transparent, 255 vollständig deckend). Andere Stile sind nicht betroffen."
    $description:es-ES: "Translucidez de las tarjetas de reloj Modern Flip (0 a 255; 0 es completamente transparente, 255 es totalmente opaco). Otros estilos no se ven afectados."
    $description:pt-BR: "Translucidez dos cartões do relógio Modern Flip (0 a 255; 0 é totalmente transparente, 255 é totalmente opaco). Outros estilos não são afetados."
    $description:fr-FR: "Translucidité des cartes d'horloge Modern Flip (0 à 255 ; 0 est totalement transparent, 255 est totalement opaque). Les autres styles ne sont pas affectés."
    $description:it-IT: "Trasparenza delle schede dell'orologio Modern Flip (da 0 a 255; 0 è completamente trasparente, 255 è completamente opaco). Gli altri stili non sono interessati."
    $description:pl-PL: "Półprzezroczystość kart zegara Modern Flip (od 0 do 255; 0 to całkowita przezroczystość, 255 to pełne krycie). Inne style nie ulegają zmianie."
    $description:ja-JP: "Modern Flip 時計カードの半透明度（0〜255。0は完全に透明、255は完全に不透明）。他のスタイルには影響しません。"
    $description:ko-KR: "Modern Flip 시계 카드의 반투명도입니다(0~255, 0은 완전 투명, 255는 완전 불투명). 다른 스타일에는 영향을 주지 않습니다."
    $description:uk-UA: "Напівпрозорість карток годинника Modern Flip (від 0 до 255; 0 — повністю прозорі, 255 — повністю непрозорі). Інші стилі не змінюються."
  - cornerRadius: -1
    $name: "🔘 Card Corner Radius (px)"
    $name:tr-TR: "🔘 Kart Köşe Yuvarlaklığı (px)"
    $name:zh-CN: "🔘 卡片圆角半径 (px)"
    $name:zh-TW: "🔘 卡片圓角半徑 (px)"
    $name:ru-RU: "🔘 Скругление углов карточек (пикс.)"
    $name:de-DE: "🔘 Karten-Eckenradius (px)"
    $name:es-ES: "🔘 Radio de esquina de tarjetas (px)"
    $name:pt-BR: "🔘 Raio dos cantos dos cartões (px)"
    $name:fr-FR: "🔘 Rayon des coins des cartes (px)"
    $name:it-IT: "🔘 Raggio angoli schede (px)"
    $name:pl-PL: "🔘 Zaokrąglenie rogów kart (px)"
    $name:ja-JP: "🔘 カードの角丸半径 (px)"
    $name:ko-KR: "🔘 카드 모서리 반경 (px)"
    $name:uk-UA: "🔘 Заокруглення кутів карток (пікс.)"
    $description: "Corner radius for Modern & Classic Flip cards (0 to 48 px; -1 uses theme default)."
    $description:tr-TR: "Modern ve Classic Flip kartları için köşe yuvarlaklığı (0 ile 48 px; -1 tema varsayılanını kullanır)."
    $description:zh-CN: "Modern 与 Classic Flip 卡片的圆角半径（0 到 48 px；-1 使用主题默认值）。"
    $description:zh-TW: "Modern 與 Classic Flip 卡片的圓角半徑（0 到 48 px；-1 使用佈景主題預設值）。"
    $description:ru-RU: "Радиус скругления углов карточек Modern и Classic Flip (от 0 до 48 пикс.; -1 — по умолчанию темы)."
    $description:de-DE: "Eckenradius für Modern & Classic Flip-Karten (0 bis 48 px; -1 verwendet den Design-Standard)."
    $description:es-ES: "Radio de esquina para las tarjetas Modern y Classic Flip (0 a 48 px; -1 usa el valor predeterminado)."
    $description:pt-BR: "Raio dos cantos dos cartões Modern e Classic Flip (0 a 48 px; -1 usa o padrão do tema)."
    $description:fr-FR: "Rayon des coins des cartes Modern et Classic Flip (0 à 48 px ; -1 utilise la valeur par défaut)."
    $description:it-IT: "Raggio degli angoli per le schede Modern e Classic Flip (0–48 px; -1 usa il valore predefinito)."
    $description:pl-PL: "Zaokrąglenie rogów kart Modern i Classic Flip (0–48 px; -1 używa wartości domyślnej)."
    $description:ja-JP: "Modern & Classic Flip カードの角丸半径（0〜48 px、-1 でテーマ既定値）。"
    $description:ko-KR: "Modern & Classic Flip 카드의 모서리 반경입니다(0~48 px, -1은 테마 기본값)."
    $description:uk-UA: "Радіус заокруглення кутів карток Modern та Classic Flip (0–48 пікс.; -1 — за замовчуванням)."
  - showSplitSeam: true
    $name: "✂️ Show Center Split Seam"
    $name:tr-TR: "✂️ Orta Bölme Çizgisini Göster"
    $name:zh-CN: "✂️ 显示中间分割线"
    $name:zh-TW: "✂️ 顯示中間分割線"
    $name:ru-RU: "✂️ Показывать центральный разделитель"
    $name:de-DE: "✂️ Mittlere Trennlinie anzeigen"
    $name:es-ES: "✂️ Mostrar línea divisoria central"
    $name:pt-BR: "✂️ Mostrar linha divisória central"
    $name:fr-FR: "✂️ Afficher la ligne de séparation centrale"
    $name:it-IT: "✂️ Mostra linea di divisione centrale"
    $name:pl-PL: "✂️ Pokaż środkową linię podziału"
    $name:ja-JP: "✂️ 中央の分割線を表示"
    $name:ko-KR: "✂️ 중앙 분할선 표시"
    $name:uk-UA: "✂️ Показувати центральний роздільник"
    $description: "Displays the horizontal mechanical split gap/seam across the flip cards."
    $description:tr-TR: "Flip kartlarının ortasındaki yatay bölme boşluğunu/çizgisini gösterir."
    $description:zh-CN: "在翻页卡片中间显示水平机械分割缝/线。"
    $description:zh-TW: "在翻頁卡片中間顯示水平機械分割縫/線。"
    $description:ru-RU: "Отображает горизонтальный разделительный шов на перекидных карточках."
    $description:de-DE: "Zeigt die horizontale mechanische Trennfuge auf den Klappkarten an."
    $description:es-ES: "Muestra la separación horizontal mecánica en el centro de las tarjetas flip."
    $description:pt-BR: "Exibe a divisória horizontal mecânica no centro dos cartões flip."
    $description:fr-FR: "Affiche la séparation mécanique horizontale au centre des cartes à volets."
    $description:it-IT: "Mostra la fessura di separazione orizzontale al centro delle schede flip."
    $description:pl-PL: "Wyświetla poziomą szczelinę podziału na środku kart klapkowych."
    $description:ja-JP: "フリップカード中央の水平分割線を表示します。"
    $description:ko-KR: "플립 카드 중앙의 가로 분할선을 표시합니다."
    $description:uk-UA: "Відображає горизонтальний розділювальний шов посередині перекидних карток."
  - digitColor: default
    $name: "🎨 Digit Color"
    $name:tr-TR: "🎨 Rakam Rengi"
    $name:zh-CN: "🎨 数字颜色"
    $name:zh-TW: "🎨 數字色彩"
    $name:ru-RU: "🎨 Цвет цифр"
    $name:de-DE: "🎨 Ziffernfarbe"
    $name:es-ES: "🎨 Color de los dígitos"
    $name:pt-BR: "🎨 Cor dos dígitos"
    $name:fr-FR: "🎨 Couleur des chiffres"
    $name:it-IT: "🎨 Colore cifre"
    $name:pl-PL: "🎨 Kolor cyfr"
    $name:ja-JP: "🎨 数字カラー"
    $name:ko-KR: "🎨 숫자 색상"
    $name:uk-UA: "🎨 Колір цифр"
    $description: "Color for the Modern & Classic Flip clock digits."
    $description:tr-TR: "Modern ve Classic Flip saat rakamlarının rengi."
    $description:zh-CN: "Modern 与 Classic Flip 时钟数字的颜色。"
    $description:zh-TW: "Modern 與 Classic Flip 時鐘數字的色彩。"
    $description:ru-RU: "Цвет цифр перекидных часов Modern и Classic Flip."
    $description:de-DE: "Farbe der Ziffern für Modern & Classic Flip-Uhren."
    $description:es-ES: "Color de los dígitos de los relojes Modern y Classic Flip."
    $description:pt-BR: "Cor dos dígitos dos relógios Modern e Classic Flip."
    $description:fr-FR: "Couleur des chiffres des horloges Modern et Classic Flip."
    $description:it-IT: "Colore delle cifre per gli orologi Modern e Classic Flip."
    $description:pl-PL: "Kolor cyfr zegarów Modern i Classic Flip."
    $description:ja-JP: "Modern & Classic Flip 時計の数字の色。"
    $description:ko-KR: "Modern & Classic Flip 시계 숫자의 색상입니다."
    $description:uk-UA: "Колір цифр годинників Modern та Classic Flip."
    $options:
      - default: "🎨 Theme Default"
      - white: "⚪ Pure White"
      - black: "⚫ Black"
      - red: "🔴 Red"
      - orange: "🟠 Orange"
      - yellow: "🟡 Yellow"
      - silver: "🩶 Soft Silver"
      - cyan: "🩵 Electric Cyan"
      - blue: "🔵 Sky Blue"
      - green: "🟢 Matrix Green"
      - amber: "🔶 Warm Amber"
      - coral: "🪸 Coral Red"
      - magenta: "🩷 Neon Magenta"
      - violet: "🟣 Soft Violet"
    $options:tr-TR:
      - default: "🎨 Tema Varsayılanı"
      - white: "⚪ Saf Beyaz"
      - black: "⚫ Siyah"
      - red: "🔴 Kırmızı"
      - orange: "🟠 Turuncu"
      - yellow: "🟡 Sarı"
      - silver: "🩶 Yumuşak Gümüş"
      - cyan: "🩵 Elektrik Camgöbeği (Cyan)"
      - blue: "🔵 Gök Mavisi"
      - green: "🟢 Matrix Yeşili"
      - amber: "🔶 Sıcak Kehribar"
      - coral: "🪸 Mercan Kırmızısı"
      - magenta: "🩷 Neon Eflatun (Magenta)"
      - violet: "🟣 Yumuşak Mor"
    $options:zh-CN:
      - default: "🎨 主题默认"
      - white: "⚪ 纯白"
      - black: "⚫ 黑色"
      - red: "🔴 红色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黄色"
      - silver: "🩶 柔和银"
      - cyan: "🩵 电光青"
      - blue: "🔵 天蓝"
      - green: "🟢 矩阵绿"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚红"
      - magenta: "🩷 霓虹品红"
      - violet: "🟣 柔紫"
    $options:zh-TW:
      - default: "🎨 佈景主題預設"
      - white: "⚪ 純白"
      - black: "⚫ 黑色"
      - red: "🔴 紅色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黃色"
      - silver: "🩶 柔和銀"
      - cyan: "🩵 電光青"
      - blue: "🔵 天藍"
      - green: "🟢 矩陣綠"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚紅"
      - magenta: "🩷 霓虹洋紅"
      - violet: "🟣 柔紫"
    $options:ru-RU:
      - default: "🎨 По умолчанию темы"
      - white: "⚪ Чистый белый"
      - black: "⚫ Чёрный"
      - red: "🔴 Красный"
      - orange: "🟠 Оранжевый"
      - yellow: "🟡 Жёлтый"
      - silver: "🩶 Мягкое серебро"
      - cyan: "🩵 Электрический циан"
      - blue: "🔵 Небесно-голубой"
      - green: "🟢 Матричный зелёный"
      - amber: "🔶 Тёплый янтарный"
      - coral: "🪸 Кораллово-красный"
      - magenta: "🩷 Неоновая маджента"
      - violet: "🟣 Мягкий фиолетовый"
    $options:de-DE:
      - default: "🎨 Design-Standard"
      - white: "⚪ Reinweiß"
      - black: "⚫ Schwarz"
      - red: "🔴 Rot"
      - orange: "🟠 Orange"
      - yellow: "🟡 Gelb"
      - silver: "🩶 Sanftes Silber"
      - cyan: "🩵 Elektrisches Cyan"
      - blue: "🔵 Himmelblau"
      - green: "🟢 Matrix-Grün"
      - amber: "🔶 Warmes Bernstein"
      - coral: "🪸 Korallenrot"
      - magenta: "🩷 Neon-Magenta"
      - violet: "🟣 Sanftes Violett"
    $options:es-ES:
      - default: "🎨 Predeterminado del tema"
      - white: "⚪ Blanco puro"
      - black: "⚫ Negro"
      - red: "🔴 Rojo"
      - orange: "🟠 Naranja"
      - yellow: "🟡 Amarillo"
      - silver: "🩶 Plata suave"
      - cyan: "🩵 Cian eléctrico"
      - blue: "🔵 Azul cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ámbar cálido"
      - coral: "🪸 Rojo coral"
      - magenta: "🩷 Magenta neón"
      - violet: "🟣 Violeta suave"
    $options:pt-BR:
      - default: "🎨 Padrão do tema"
      - white: "⚪ Branco puro"
      - black: "⚫ Preto"
      - red: "🔴 Vermelho"
      - orange: "🟠 Laranja"
      - yellow: "🟡 Amarelo"
      - silver: "🩶 Prata suave"
      - cyan: "🩵 Ciano elétrico"
      - blue: "🔵 Azul celeste"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Âmbar quente"
      - coral: "🪸 Vermelho coral"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Violeta suave"
    $options:fr-FR:
      - default: "🎨 Par défaut du thème"
      - white: "⚪ Blanc pur"
      - black: "⚫ Noir"
      - red: "🔴 Rouge"
      - orange: "🟠 Orange"
      - yellow: "🟡 Jaune"
      - silver: "🩶 Argent doux"
      - cyan: "🩵 Cyan électrique"
      - blue: "🔵 Bleu ciel"
      - green: "🟢 Vert Matrix"
      - amber: "🔶 Ambre chaud"
      - coral: "🪸 Rouge corail"
      - magenta: "🩷 Magenta néon"
      - violet: "🟣 Violet doux"
    $options:it-IT:
      - default: "🎨 Predefinito del tema"
      - white: "⚪ Bianco puro"
      - black: "⚫ Nero"
      - red: "🔴 Rosso"
      - orange: "🟠 Arancione"
      - yellow: "🟡 Giallo"
      - silver: "🩶 Argento tenue"
      - cyan: "🩵 Ciano elettrico"
      - blue: "🔵 Blu cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ambra caldo"
      - coral: "🪸 Rosso corallo"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Viola tenue"
    $options:pl-PL:
      - default: "🎨 Domyślny motywu"
      - white: "⚪ Czysta biel"
      - black: "⚫ Czarny"
      - red: "🔴 Czerwony"
      - orange: "🟠 Pomarańczowy"
      - yellow: "🟡 Żółty"
      - silver: "🩶 Miękkie srebro"
      - cyan: "🩵 Elektryczny cyjan"
      - blue: "🔵 Błękit nieba"
      - green: "🟢 Zieleń Matrix"
      - amber: "🔶 Ciepły bursztyn"
      - coral: "🪸 Koralowa czerwień"
      - magenta: "🩷 Neonowa magenta"
      - violet: "🟣 Delikatny fiolet"
    $options:ja-JP:
      - default: "🎨 テーマ既定値"
      - white: "⚪ ピュアホワイト"
      - black: "⚫ ブラック"
      - red: "🔴 レッド"
      - orange: "🟠 オレンジ"
      - yellow: "🟡 イエロー"
      - silver: "🩶 ソフトシルバー"
      - cyan: "🩵 エレクトリックシアン"
      - blue: "🔵 スカイブルー"
      - green: "🟢 マトリックスグリーン"
      - amber: "🔶 ウォームアンバー"
      - coral: "🪸 コーラルレッド"
      - magenta: "🩷 ネオンマゼンタ"
      - violet: "🟣 ソフトバイオレット"
    $options:ko-KR:
      - default: "🎨 테마 기본값"
      - white: "⚪ 순수 흰색"
      - black: "⚫ 블랙"
      - red: "🔴 레드"
      - orange: "🟠 오렌지"
      - yellow: "🟡 옐로우"
      - silver: "🩶 부드러운 실버"
      - cyan: "🩵 일렉트릭 시안"
      - blue: "🔵 스카이 블루"
      - green: "🟢 매트릭스 그린"
      - amber: "🔶 따뜻한 앰버"
      - coral: "🪸 코랄 레드"
      - magenta: "🩷 네온 마젠타"
      - violet: "🟣 부드러운 바이올렛"
    $options:uk-UA:
      - default: "🎨 За замовчуванням теми"
      - white: "⚪ Чистий білий"
      - black: "⚫ Чорний"
      - red: "🔴 Червоний"
      - orange: "🟠 Помаранчевий"
      - yellow: "🟡 Жовтий"
      - silver: "🩶 М'яке срібло"
      - cyan: "🩵 Електричний ціан"
      - blue: "🔵 Небесно-блакитний"
      - green: "🟢 Матричний зелений"
      - amber: "🔶 Теплий бурштиновий"
      - coral: "🪸 Коралово-червоний"
      - magenta: "🩷 Неонова маджента"
      - violet: "🟣 М'який фіолетовий"
  $name: "⏳ Flip Clocks (Modern & Classic)"
  $name:tr-TR: "⏳ Flip Saatler (Modern & Classic)"
  $name:zh-CN: "⏳ Flip 时钟（Modern & Classic）"
  $name:zh-TW: "⏳ Flip 時鐘（Modern & Classic）"
  $name:ru-RU: "⏳ Перекидные часы (Modern & Classic)"
  $name:de-DE: "⏳ Flip-Uhren (Modern & Classic)"
  $name:es-ES: "⏳ Relojes Flip (Modern & Classic)"
  $name:pt-BR: "⏳ Relógios Flip (Modern & Classic)"
  $name:fr-FR: "⏳ Horloges Flip (Modern & Classic)"
  $name:it-IT: "⏳ Orologi Flip (Modern & Classic)"
  $name:pl-PL: "⏳ Zegary Flip (Modern & Classic)"
  $name:ja-JP: "⏳ Flip 時計（Modern & Classic）"
  $name:ko-KR: "⏳ Flip 시계 (Modern & Classic)"
  $name:uk-UA: "⏳ Перекидні годинники (Modern & Classic)"
- minimalistSettings:
  - fontWeight: default
    $name: "🔤 Font Weight"
    $name:tr-TR: "🔤 Yazı Kalınlığı"
    $name:zh-CN: "🔤 字体粗细"
    $name:zh-TW: "🔤 字型粗細"
    $name:ru-RU: "🔤 Насыщенность шрифта"
    $name:de-DE: "🔤 Schriftstärke"
    $name:es-ES: "🔤 Grosor de fuente"
    $name:pt-BR: "🔤 Espessura da fonte"
    $name:fr-FR: "🔤 Épaisseur de police"
    $name:it-IT: "🔤 Spessore carattere"
    $name:pl-PL: "🔤 Grubość czcionki"
    $name:ja-JP: "🔤 フォントの太さ"
    $name:ko-KR: "🔤 글꼴 두께"
    $name:uk-UA: "🔤 Товщина шрифту"
    $description: "Overrides digit weight for Minimalist Sonoma."
    $description:tr-TR: "Minimalist Sonoma için rakam yazı kalınlığını belirler."
    $description:zh-CN: "覆盖 Minimalist Sonoma 的数字粗细。"
    $description:zh-TW: "覆寫 Minimalist Sonoma 的數字粗細。"
    $description:ru-RU: "Переопределяет насыщенность цифр для Minimalist Sonoma."
    $description:de-DE: "Überschreibt die Ziffernschriftstärke für Minimalist Sonoma."
    $description:es-ES: "Anula el grosor de los dígitos para Minimalist Sonoma."
    $description:pt-BR: "Substitui a espessura dos dígitos para Minimalist Sonoma."
    $description:fr-FR: "Remplace l'épaisseur des chiffres pour Minimalist Sonoma."
    $description:it-IT: "Sostituisce lo spessore delle cifre per Minimalist Sonoma."
    $description:pl-PL: "Nadpisuje grubość cyfr dla Minimalist Sonoma."
    $description:ja-JP: "Minimalist Sonoma の数字の太さを上書きします。"
    $description:ko-KR: "Minimalist Sonoma의 숫자 두께를 재정의합니다."
    $description:uk-UA: "Перевизначає товщину цифр для Minimalist Sonoma."
    $options:
      - default: "Theme Default"
      - thin: "Thin"
      - light: "Light"
      - normal: "Regular"
      - semibold: "SemiBold"
      - bold: "Bold"
    $options:tr-TR:
      - default: "Tema Varsayılanı"
      - thin: "İnce (Thin)"
      - light: "Hafif (Light)"
      - normal: "Normal (Regular)"
      - semibold: "Yarı Kalın (SemiBold)"
      - bold: "Kalın (Bold)"
    $options:zh-CN:
      - default: "主题默认"
      - thin: "极细 (Thin)"
      - light: "细体 (Light)"
      - normal: "常规 (Regular)"
      - semibold: "半粗体 (SemiBold)"
      - bold: "粗体 (Bold)"
    $options:zh-TW:
      - default: "佈景主題預設"
      - thin: "極細 (Thin)"
      - light: "細體 (Light)"
      - normal: "標準 (Regular)"
      - semibold: "半粗體 (SemiBold)"
      - bold: "粗體 (Bold)"
    $options:ru-RU:
      - default: "По умолчанию темы"
      - thin: "Тонкий (Thin)"
      - light: "Светлый (Light)"
      - normal: "Обычный (Regular)"
      - semibold: "Полужирный (SemiBold)"
      - bold: "Жирный (Bold)"
    $options:de-DE:
      - default: "Design-Standard"
      - thin: "Dünn (Thin)"
      - light: "Leicht (Light)"
      - normal: "Normal (Regular)"
      - semibold: "Halbfett (SemiBold)"
      - bold: "Fett (Bold)"
    $options:es-ES:
      - default: "Predeterminado del tema"
      - thin: "Fina (Thin)"
      - light: "Ligera (Light)"
      - normal: "Normal (Regular)"
      - semibold: "Seminegrita (SemiBold)"
      - bold: "Negrita (Bold)"
    $options:pt-BR:
      - default: "Padrão do tema"
      - thin: "Fina (Thin)"
      - light: "Leve (Light)"
      - normal: "Normal (Regular)"
      - semibold: "Seminegrito (SemiBold)"
      - bold: "Negrito (Bold)"
    $options:fr-FR:
      - default: "Par défaut du thème"
      - thin: "Fin (Thin)"
      - light: "Léger (Light)"
      - normal: "Normal (Regular)"
      - semibold: "Semi-gras (SemiBold)"
      - bold: "Gras (Bold)"
    $options:it-IT:
      - default: "Predefinito del tema"
      - thin: "Sottile (Thin)"
      - light: "Leggero (Light)"
      - normal: "Normale (Regular)"
      - semibold: "Semigrassetto (SemiBold)"
      - bold: "Grassetto (Bold)"
    $options:pl-PL:
      - default: "Domyślna motywu"
      - thin: "Cienka (Thin)"
      - light: "Lekka (Light)"
      - normal: "Zwykła (Regular)"
      - semibold: "Półgruba (SemiBold)"
      - bold: "Pogrubiona (Bold)"
    $options:ja-JP:
      - default: "テーマ既定値"
      - thin: "極細 (Thin)"
      - light: "細字 (Light)"
      - normal: "標準 (Regular)"
      - semibold: "中太 (SemiBold)"
      - bold: "太字 (Bold)"
    $options:ko-KR:
      - default: "테마 기본값"
      - thin: "가는체 (Thin)"
      - light: "가벼운체 (Light)"
      - normal: "보통 (Regular)"
      - semibold: "중간 굵기 (SemiBold)"
      - bold: "굵게 (Bold)"
    $options:uk-UA:
      - default: "За замовчуванням теми"
      - thin: "Тонкий (Thin)"
      - light: "Світлий (Light)"
      - normal: "Звичайний (Regular)"
      - semibold: "Напівжирний (SemiBold)"
      - bold: "Жирний (Bold)"
  - textColor: default
    $name: "🎨 Text Color"
    $name:tr-TR: "🎨 Metin Rengi"
    $name:zh-CN: "🎨 文本颜色"
    $name:zh-TW: "🎨 文字色彩"
    $name:ru-RU: "🎨 Цвет текста"
    $name:de-DE: "🎨 Textfarbe"
    $name:es-ES: "🎨 Color del texto"
    $name:pt-BR: "🎨 Cor do texto"
    $name:fr-FR: "🎨 Couleur du texte"
    $name:it-IT: "🎨 Colore testo"
    $name:pl-PL: "🎨 Kolor tekstu"
    $name:ja-JP: "🎨 テキストカラー"
    $name:ko-KR: "🎨 텍스트 색상"
    $name:uk-UA: "🎨 Колір тексту"
    $description: "Color for Minimalist Sonoma clock text."
    $description:tr-TR: "Minimalist Sonoma saat metninin rengi."
    $description:zh-CN: "Minimalist Sonoma 时钟文本的颜色。"
    $description:zh-TW: "Minimalist Sonoma 時鐘文字的色彩。"
    $description:ru-RU: "Цвет текста часов Minimalist Sonoma."
    $description:de-DE: "Textfarbe für Minimalist Sonoma."
    $description:es-ES: "Color del texto de los relojes Minimalist Sonoma."
    $description:pt-BR: "Cor do texto dos relógios Minimalist Sonoma."
    $description:fr-FR: "Couleur du texte des horloges Minimalist Sonoma."
    $description:it-IT: "Colore del testo degli orologi Minimalist Sonoma."
    $description:pl-PL: "Kolor tekstu zegarów Minimalist Sonoma."
    $description:ja-JP: "Minimalist Sonoma の時計テキストの色。"
    $description:ko-KR: "Minimalist Sonoma 시계 텍스트의 색상입니다."
    $description:uk-UA: "Колір тексту годинників Minimalist Sonoma."
    $options:
      - default: "🎨 Theme Default (White)"
      - white: "⚪ Pure White"
      - black: "⚫ Black"
      - red: "🔴 Red"
      - orange: "🟠 Orange"
      - yellow: "🟡 Yellow"
      - silver: "🩶 Soft Silver"
      - cyan: "🩵 Electric Cyan"
      - blue: "🔵 Sky Blue"
      - green: "🟢 Matrix Green"
      - amber: "🔶 Warm Amber"
      - coral: "🪸 Coral Red"
      - magenta: "🩷 Neon Magenta"
      - violet: "🟣 Soft Violet"
    $options:tr-TR:
      - default: "🎨 Tema Varsayılanı (Beyaz)"
      - white: "⚪ Saf Beyaz"
      - black: "⚫ Siyah"
      - red: "🔴 Kırmızı"
      - orange: "🟠 Turuncu"
      - yellow: "🟡 Sarı"
      - silver: "🩶 Yumuşak Gümüş"
      - cyan: "🩵 Elektrik Camgöbeği (Cyan)"
      - blue: "🔵 Gök Mavisi"
      - green: "🟢 Matrix Yeşili"
      - amber: "🔶 Sıcak Kehribar"
      - coral: "🪸 Mercan Kırmızısı"
      - magenta: "🩷 Neon Eflatun (Magenta)"
      - violet: "🟣 Yumuşak Mor"
    $options:zh-CN:
      - default: "🎨 主题默认（白色）"
      - white: "⚪ 纯白"
      - black: "⚫ 黑色"
      - red: "🔴 红色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黄色"
      - silver: "🩶 柔和银"
      - cyan: "🩵 电光青"
      - blue: "🔵 天蓝"
      - green: "🟢 矩阵绿"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚红"
      - magenta: "🩷 霓虹品红"
      - violet: "🟣 柔紫"
    $options:zh-TW:
      - default: "🎨 佈景主題預設（白色）"
      - white: "⚪ 純白"
      - black: "⚫ 黑色"
      - red: "🔴 紅色"
      - orange: "🟠 橙色"
      - yellow: "🟡 黃色"
      - silver: "🩶 柔和銀"
      - cyan: "🩵 電光青"
      - blue: "🔵 天藍"
      - green: "🟢 矩陣綠"
      - amber: "🔶 暖琥珀"
      - coral: "🪸 珊瑚紅"
      - magenta: "🩷 霓虹洋紅"
      - violet: "🟣 柔紫"
    $options:ru-RU:
      - default: "🎨 По умолчанию темы (белый)"
      - white: "⚪ Чистый белый"
      - black: "⚫ Чёрный"
      - red: "🔴 Красный"
      - orange: "🟠 Оранжевый"
      - yellow: "🟡 Жёлтый"
      - silver: "🩶 Мягкое серебро"
      - cyan: "🩵 Электрический циан"
      - blue: "🔵 Небесно-голубой"
      - green: "🟢 Матричный зелёный"
      - amber: "🔶 Тёплый янтарный"
      - coral: "🪸 Кораллово-красный"
      - magenta: "🩷 Неоновая маджента"
      - violet: "🟣 Мягкий фиолетовый"
    $options:de-DE:
      - default: "🎨 Design-Standard (Weiß)"
      - white: "⚪ Reinweiß"
      - black: "⚫ Schwarz"
      - red: "🔴 Rot"
      - orange: "🟠 Orange"
      - yellow: "🟡 Gelb"
      - silver: "🩶 Sanftes Silber"
      - cyan: "🩵 Elektrisches Cyan"
      - blue: "🔵 Himmelblau"
      - green: "🟢 Matrix-Grün"
      - amber: "🔶 Warmes Bernstein"
      - coral: "🪸 Korallenrot"
      - magenta: "🩷 Neon-Magenta"
      - violet: "🟣 Sanftes Violett"
    $options:es-ES:
      - default: "🎨 Predeterminado del tema (Blanco)"
      - white: "⚪ Blanco puro"
      - black: "⚫ Negro"
      - red: "🔴 Rojo"
      - orange: "🟠 Naranja"
      - yellow: "🟡 Amarillo"
      - silver: "🩶 Plata suave"
      - cyan: "🩵 Cian eléctrico"
      - blue: "🔵 Azul cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ámbar cálido"
      - coral: "🪸 Rojo coral"
      - magenta: "🩷 Magenta neón"
      - violet: "🟣 Violeta suave"
    $options:pt-BR:
      - default: "🎨 Padrão do tema (Branco)"
      - white: "⚪ Branco puro"
      - black: "⚫ Preto"
      - red: "🔴 Vermelho"
      - orange: "🟠 Laranja"
      - yellow: "🟡 Amarelo"
      - silver: "🩶 Prata suave"
      - cyan: "🩵 Ciano elétrico"
      - blue: "🔵 Azul celeste"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Âmbar quente"
      - coral: "🪸 Vermelho coral"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Violeta suave"
    $options:fr-FR:
      - default: "🎨 Par défaut du thème (Blanc)"
      - white: "⚪ Blanc pur"
      - black: "⚫ Noir"
      - red: "🔴 Rouge"
      - orange: "🟠 Orange"
      - yellow: "🟡 Jaune"
      - silver: "🩶 Argent doux"
      - cyan: "🩵 Cyan électrique"
      - blue: "🔵 Bleu ciel"
      - green: "🟢 Vert Matrix"
      - amber: "🔶 Ambre chaud"
      - coral: "🪸 Rouge corail"
      - magenta: "🩷 Magenta néon"
      - violet: "🟣 Violet doux"
    $options:it-IT:
      - default: "🎨 Predefinito del tema (Bianco)"
      - white: "⚪ Bianco puro"
      - black: "⚫ Nero"
      - red: "🔴 Rosso"
      - orange: "🟠 Arancione"
      - yellow: "🟡 Giallo"
      - silver: "🩶 Argento tenue"
      - cyan: "🩵 Ciano elettrico"
      - blue: "🔵 Blu cielo"
      - green: "🟢 Verde Matrix"
      - amber: "🔶 Ambra caldo"
      - coral: "🪸 Rosso corallo"
      - magenta: "🩷 Magenta neon"
      - violet: "🟣 Viola tenue"
    $options:pl-PL:
      - default: "🎨 Domyślny motywu (Biel)"
      - white: "⚪ Czysta biel"
      - black: "⚫ Czarny"
      - red: "🔴 Czerwony"
      - orange: "🟠 Pomarańczowy"
      - yellow: "🟡 Żółty"
      - silver: "🩶 Miękkie srebro"
      - cyan: "🩵 Elektryczny cyjan"
      - blue: "🔵 Błękit nieba"
      - green: "🟢 Zieleń Matrix"
      - amber: "🔶 Ciepły bursztyn"
      - coral: "🪸 Koralowa czerwień"
      - magenta: "🩷 Neonowa magenta"
      - violet: "🟣 Delikatny fiolet"
    $options:ja-JP:
      - default: "🎨 テーマ既定値（白）"
      - white: "⚪ ピュアホワイト"
      - black: "⚫ ブラック"
      - red: "🔴 レッド"
      - orange: "🟠 オレンジ"
      - yellow: "🟡 イエロー"
      - silver: "🩶 ソフトシルバー"
      - cyan: "🩵 エレクトリックシアン"
      - blue: "🔵 スカイブルー"
      - green: "🟢 マトリックスグリーン"
      - amber: "🔶 ウォームアンバー"
      - coral: "🪸 コーラルレッド"
      - magenta: "🩷 ネオンマゼンタ"
      - violet: "🟣 ソフトバイオレット"
    $options:ko-KR:
      - default: "🎨 테마 기본값 (흰색)"
      - white: "⚪ 순수 흰색"
      - black: "⚫ 블랙"
      - red: "🔴 레드"
      - orange: "🟠 오렌지"
      - yellow: "🟡 옐로우"
      - silver: "🩶 부드러운 실버"
      - cyan: "🩵 일렉트릭 시안"
      - blue: "🔵 스카이 블루"
      - green: "🟢 매트릭스 그린"
      - amber: "🔶 따뜻한 앰버"
      - coral: "🪸 코랄 레드"
      - magenta: "🩷 네온 마젠타"
      - violet: "🟣 부드러운 바이올렛"
    $options:uk-UA:
      - default: "🎨 За замовчуванням теми (Білий)"
      - white: "⚪ Чистий білий"
      - black: "⚫ Чорний"
      - red: "🔴 Червоний"
      - orange: "🟠 Помаранчевий"
      - yellow: "🟡 Жовтий"
      - silver: "🩶 М'яке срібло"
      - cyan: "🩵 Електричний ціан"
      - blue: "🔵 Небесно-блакитний"
      - green: "🟢 Матричний зелений"
      - amber: "🔶 Теплий бурштиновий"
      - coral: "🪸 Коралово-червоний"
      - magenta: "🩷 Неонова маджента"
      - violet: "🟣 М'який фіолетовий"
  - fontFamily: default
    $name: "🔤 Font Family"
    $name:tr-TR: "🔤 Yazı Tipi"
    $name:zh-CN: "🔤 字体"
    $name:zh-TW: "🔤 字型"
    $name:ru-RU: "🔤 Шрифт"
    $name:de-DE: "🔤 Schriftart"
    $name:es-ES: "🔤 Fuente"
    $name:pt-BR: "🔤 Fonte"
    $name:fr-FR: "🔤 Police"
    $name:it-IT: "🔤 Carattere"
    $name:pl-PL: "🔤 Czcionka"
    $name:ja-JP: "🔤 フォント"
    $name:ko-KR: "🔤 글꼴"
    $name:uk-UA: "🔤 Шрифт"
    $description: "Font used for Minimalist Sonoma."
    $description:tr-TR: "Minimalist Sonoma için kullanılacak yazı tipi."
    $description:zh-CN: "Minimalist Sonoma 使用的字体。"
    $description:zh-TW: "Minimalist Sonoma 使用的字型。"
    $description:ru-RU: "Шрифт для Minimalist Sonoma."
    $description:de-DE: "Schriftart für Minimalist Sonoma."
    $description:es-ES: "Fuente utilizada para Minimalist Sonoma."
    $description:pt-BR: "Fonte usada para Minimalist Sonoma."
    $description:fr-FR: "Police utilisée pour Minimalist Sonoma."
    $description:it-IT: "Carattere usato per Minimalist Sonoma."
    $description:pl-PL: "Czcionka używana dla Minimalist Sonoma."
    $description:ja-JP: "Minimalist Sonoma に使用するフォント。"
    $description:ko-KR: "Minimalist Sonoma에 사용할 글꼴입니다."
    $description:uk-UA: "Шрифт для Minimalist Sonoma."
    $options:
      - default: "Theme Default"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:tr-TR:
      - default: "Tema Varsayılanı"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:zh-CN:
      - default: "主题默认"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:zh-TW:
      - default: "佈景主題預設"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ru-RU:
      - default: "По умолчанию темы"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:de-DE:
      - default: "Design-Standard"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:es-ES:
      - default: "Predeterminado del tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:pt-BR:
      - default: "Padrão do tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:fr-FR:
      - default: "Par défaut du thème"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:it-IT:
      - default: "Predefinito del tema"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:pl-PL:
      - default: "Domyślna motywu"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ja-JP:
      - default: "テーマ既定値"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:ko-KR:
      - default: "테마 기본값"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
    $options:uk-UA:
      - default: "За замовчуванням теми"
      - Segoe UI Variable Display: "Segoe UI Variable Display"
      - Segoe UI Light: "Segoe UI Light"
      - Bahnschrift: "Bahnschrift"
      - Impact: "Impact"
      - Consolas: "Consolas"
      - Calibri Light: "Calibri Light"
      - Georgia: "Georgia"
      - Trebuchet MS: "Trebuchet MS"
  - showShadow: true
    $name: "🌫️ Show Drop Shadow"
    $name:tr-TR: "🌫️ Alt Gölgeyi Göster"
    $name:zh-CN: "🌫️ 显示投影"
    $name:zh-TW: "🌫️ 顯示陰影"
    $name:ru-RU: "🌫️ Показывать тень"
    $name:de-DE: "🌫️ Schlagschatten anzeigen"
    $name:es-ES: "🌫️ Mostrar sombra"
    $name:pt-BR: "🌫️ Mostrar sombra"
    $name:fr-FR: "🌫️ Afficher l'ombre portée"
    $name:it-IT: "🌫️ Mostra ombra"
    $name:pl-PL: "🌫️ Pokaż cień"
    $name:ja-JP: "🌫️ ドロップシャドウを表示"
    $name:ko-KR: "🌫️ 그림자 표시"
    $name:uk-UA: "🌫️ Показувати тінь"
    $description: "Enables the atmospheric drop-shadow layer for better legibility."
    $description:tr-TR: "Okunabilirliği artıran atmosferik gölge katmanını etkinleştirir."
    $description:zh-CN: "启用可提升可读性的柔和投影层。"
    $description:zh-TW: "啟用可提升可讀性的柔和陰影層。"
    $description:ru-RU: "Включает мягкий слой тени для лучшей читаемости."
    $description:de-DE: "Aktiviert die dezente Schattenebene für bessere Lesbarkeit."
    $description:es-ES: "Activa la capa de sombra atmosférica para mejorar la legibilidad."
    $description:pt-BR: "Ativa a camada de sombra atmosférica para melhor legibilidade."
    $description:fr-FR: "Active l'ombre portée atmosphérique pour une meilleure lisibilité."
    $description:it-IT: "Attiva il livello d'ombra atmosferica per una migliore leggibilità."
    $description:pl-PL: "Włącza warstwę miękkiego cienia dla lepszej czytelności."
    $description:ja-JP: "視認性を高めるドロップシャドウ層を有効にします。"
    $description:ko-KR: "가독성을 높여주는 그림자 레이어를 활성화합니다."
    $description:uk-UA: "Вмикає м'який шар тіні для кращої читабельності."
  $name: "🪐 Minimalist Sonoma"
  $name:tr-TR: "🪐 Minimalist Sonoma"
  $name:zh-CN: "🪐 Minimalist Sonoma"
  $name:zh-TW: "🪐 Minimalist Sonoma"
  $name:ru-RU: "🪐 Minimalist Sonoma"
  $name:de-DE: "🪐 Minimalist Sonoma"
  $name:es-ES: "🪐 Minimalist Sonoma"
  $name:pt-BR: "🪐 Minimalist Sonoma"
  $name:fr-FR: "🪐 Minimalist Sonoma"
  $name:it-IT: "🪐 Minimalist Sonoma"
  $name:pl-PL: "🪐 Minimalist Sonoma"
  $name:ja-JP: "🪐 Minimalist Sonoma"
  $name:ko-KR: "🪐 Minimalist Sonoma"
  $name:uk-UA: "🪐 Minimalist Sonoma"
- cyberpunkSettings:
  - colorPreset: default
    $name: "🎯 HUD Color Preset"
    $name:tr-TR: "🎯 HUD Renk Teması"
    $name:zh-CN: "🎯 HUD 配色方案"
    $name:zh-TW: "🎯 HUD 色彩配置"
    $name:ru-RU: "🎯 Цветовая схема HUD"
    $name:de-DE: "🎯 HUD-Farbschema"
    $name:es-ES: "🎯 Paleta de color HUD"
    $name:pt-BR: "🎯 Esquema de cores HUD"
    $name:fr-FR: "🎯 Palette de couleurs HUD"
    $name:it-IT: "🎯 Schema colori HUD"
    $name:pl-PL: "🎯 Motyw kolorystyczny HUD"
    $name:ja-JP: "🎯 HUD カラープリセット"
    $name:ko-KR: "🎯 HUD 색상 프리셋"
    $name:uk-UA: "🎯 Колірна схема HUD"
    $description: "Accent palette for the Cyberpunk Sci-Fi HUD terminal."
    $description:tr-TR: "Cyberpunk Sci-Fi HUD terminali için vurgu renk paleti."
    $description:zh-CN: "Cyberpunk Sci-Fi HUD 终端的强调色方案。"
    $description:zh-TW: "Cyberpunk Sci-Fi HUD 終端機的強調色配置。"
    $description:ru-RU: "Акцентная цветовая палитра для терминала Cyberpunk Sci-Fi HUD."
    $description:de-DE: "Akzent-Farbpalette für das Cyberpunk Sci-Fi HUD-Terminal."
    $description:es-ES: "Paleta de acento para la terminal Cyberpunk Sci-Fi HUD."
    $description:pt-BR: "Paleta de destaque para o terminal Cyberpunk Sci-Fi HUD."
    $description:fr-FR: "Palette d'accentuation pour le terminal Cyberpunk Sci-Fi HUD."
    $description:it-IT: "Tavolozza di colori per il terminale Cyberpunk Sci-Fi HUD."
    $description:pl-PL: "Paleta kolorów akcentu dla terminala Cyberpunk Sci-Fi HUD."
    $description:ja-JP: "Cyberpunk Sci-Fi HUD ターミナルのアクセントカラー。"
    $description:ko-KR: "Cyberpunk Sci-Fi HUD 터미널의 강조 색상 팔레트입니다."
    $description:uk-UA: "Акцентна колірна палітра для термінала Cyberpunk Sci-Fi HUD."
    $options:
      - default: "🩵🟡 Cyan & Acid Yellow (Default)"
      - matrix: "🟢 Matrix Neon Green"
      - crimson: "🔴🔶 Crimson & Amber"
      - violet: "🟣🩵 Neon Violet & Cyan"
    $options:tr-TR:
      - default: "🩵🟡 Cyan & Acid Yellow (Varsayılan)"
      - matrix: "🟢 Matrix Neon Yeşili"
      - crimson: "🔴🔶 Kızıl & Kehribar"
      - violet: "🟣🩵 Neon Mor & Cyan"
    $options:zh-CN:
      - default: "🩵🟡 青色与酸性黄（默认）"
      - matrix: "🟢 矩阵霓虹绿"
      - crimson: "🔴🔶 绯红与琥珀"
      - violet: "🟣🩵 霓虹紫与青"
    $options:zh-TW:
      - default: "🩵🟡 青色與酸性黃（預設）"
      - matrix: "🟢 矩陣霓虹綠"
      - crimson: "🔴🔶 緋紅與琥珀"
      - violet: "🟣🩵 霓虹紫與青"
    $options:ru-RU:
      - default: "🩵🟡 Циан и кислотно-жёлтый (по умолчанию)"
      - matrix: "🟢 Матричный неоново-зелёный"
      - crimson: "🔴🔶 Багровый и янтарный"
      - violet: "🟣🩵 Неоновый фиолетовый и циан"
    $options:de-DE:
      - default: "🩵🟡 Cyan & Acid Yellow (Standard)"
      - matrix: "🟢 Matrix-Neongrün"
      - crimson: "🔴🔶 Karmesinrot & Bernstein"
      - violet: "🟣🩵 Neonviolett & Cyan"
    $options:es-ES:
      - default: "🩵🟡 Cian y amarillo ácido (Predeterminado)"
      - matrix: "🟢 Verde neón Matrix"
      - crimson: "🔴🔶 Carmesí y ámbar"
      - violet: "🟣🩵 Violeta neón y cian"
    $options:pt-BR:
      - default: "🩵🟡 Ciano e amarelo ácido (Padrão)"
      - matrix: "🟢 Verde neon Matrix"
      - crimson: "🔴🔶 Carmesim e âmbar"
      - violet: "🟣🩵 Violeta neon e ciano"
    $options:fr-FR:
      - default: "🩵🟡 Cyan et jaune acide (Par défaut)"
      - matrix: "🟢 Vert néon Matrix"
      - crimson: "🔴🔶 Cramoisi et ambre"
      - violet: "🟣🩵 Violet néon et cyan"
    $options:it-IT:
      - default: "🩵🟡 Ciano e giallo acido (Predefinito)"
      - matrix: "🟢 Verde neon Matrix"
      - crimson: "🔴🔶 Cremisi e ambra"
      - violet: "🟣🩵 Viola neon e ciano"
    $options:pl-PL:
      - default: "🩵🟡 Cyjan i kwasowy żółty (Domyślne)"
      - matrix: "🟢 Neonowa zieleń Matrix"
      - crimson: "🔴🔶 Karmazyn i bursztyn"
      - violet: "🟣🩵 Neonowy fiolet i cyjan"
    $options:ja-JP:
      - default: "🩵🟡 シアン & アシッドイエロー（既定）"
      - matrix: "🟢 マトリックスネオングリーン"
      - crimson: "🔴🔶 クリムゾン & アンバー"
      - violet: "🟣🩵 ネオンバイオレット & シアン"
    $options:ko-KR:
      - default: "🩵🟡 시안 & 애시드 옐로우 (기본값)"
      - matrix: "🟢 매트릭스 네온 그린"
      - crimson: "🔴🔶 크림슨 & 앰버"
      - violet: "🟣🩵 네온 바이올렛 & 시안"
    $options:uk-UA:
      - default: "🩵🟡 Ціан і кислотно-жовтий (За замовчуванням)"
      - matrix: "🟢 Матричний неоново-зелений"
      - crimson: "🔴🔶 Багряний і бурштиновий"
      - violet: "🟣🩵 Неоновий фіолетовий і ціан"
  - showHeaderBar: true
    $name: "📊 Show Top Telemetry Bar"
    $name:tr-TR: "📊 Üst Telemetri Çubuğunu Göster"
    $name:zh-CN: "📊 显示顶部遥测栏"
    $name:zh-TW: "📊 顯示頂部遙測列"
    $name:ru-RU: "📊 Показывать верхнюю панель телеметрии"
    $name:de-DE: "📊 Obere Telemetrieleiste anzeigen"
    $name:es-ES: "📊 Mostrar barra de telemetría superior"
    $name:pt-BR: "📊 Mostrar barra de telemetria superior"
    $name:fr-FR: "📊 Afficher la barre de télémétrie supérieure"
    $name:it-IT: "📊 Mostra barra di telemetria superiore"
    $name:pl-PL: "📊 Pokaż górny pasek telemetrii"
    $name:ja-JP: "📊 上部テレメトリバーを表示"
    $name:ko-KR: "📊 상단 텔레메트리 바 표시"
    $name:uk-UA: "📊 Показувати верхню панель телеметрії"
    $description: "Displays the SYS.LOC // TIME_SYNC header bar and status indicators."
    $description:tr-TR: "SYS.LOC // TIME_SYNC üst bilgi çubuğunu ve durum göstergelerini görüntüler."
    $description:zh-CN: "显示 SYS.LOC // TIME_SYNC 顶部信息栏及状态指示器。"
    $description:zh-TW: "顯示 SYS.LOC // TIME_SYNC 頂部資訊列及狀態指示器。"
    $description:ru-RU: "Отображает верхнюю строку SYS.LOC // TIME_SYNC и индикаторы состояния."
    $description:de-DE: "Zeigt die SYS.LOC // TIME_SYNC-Kopfzeile und Statusanzeigen an."
    $description:es-ES: "Muestra la barra superior SYS.LOC // TIME_SYNC y los indicadores de estado."
    $description:pt-BR: "Exibe a barra superior SYS.LOC // TIME_SYNC e os indicadores de status."
    $description:fr-FR: "Affiche la barre d'en-tête SYS.LOC // TIME_SYNC et les indicateurs d'état."
    $description:it-IT: "Mostra la barra superiore SYS.LOC // TIME_SYNC e gli indicatori di stato."
    $description:pl-PL: "Wyświetla górny pasek SYS.LOC // TIME_SYNC oraz wskaźniki stanu."
    $description:ja-JP: "SYS.LOC // TIME_SYNC ヘッダーバーとステータス表示を表示します。"
    $description:ko-KR: "SYS.LOC // TIME_SYNC 상단 바 및 상태 표시기를 표시합니다."
    $description:uk-UA: "Відображає верхній рядок SYS.LOC // TIME_SYNC та індикатори стану."
  - chassisOpacity: 170
    $name: "🪟 Chassis Opacity"
    $name:tr-TR: "🪟 Gövde Opaklığı"
    $name:zh-CN: "🪟 外框不透明度"
    $name:zh-TW: "🪟 外框不透明度"
    $name:ru-RU: "🪟 Непрозрачность корпуса"
    $name:de-DE: "🪟 Gehäuse-Deckkraft"
    $name:es-ES: "🪟 Opacidad del chasis"
    $name:pt-BR: "🪟 Opacidade do chassi"
    $name:fr-FR: "🪟 Opacité du châssis"
    $name:it-IT: "🪟 Opacità chassis"
    $name:pl-PL: "🪟 Przezroczystość obudowy"
    $name:ja-JP: "🪟 シャーシの不透明度"
    $name:ko-KR: "🪟 섀시 불투명도"
    $name:uk-UA: "🪟 Непрозорість корпусу"
    $description: "Background opacity of the HUD chassis (0 to 255; default 170)."
    $description:tr-TR: "HUD gövdesinin arka plan opaklığı (0 ile 255 arası; varsayılan 170)."
    $description:zh-CN: "HUD 外框的背景不透明度（0 到 255；默认 170）。"
    $description:zh-TW: "HUD 外框的背景不透明度（0 到 255；預設 170）。"
    $description:ru-RU: "Непрозрачность фона корпуса HUD (от 0 до 255; по умолчанию 170)."
    $description:de-DE: "Hintergrunddeckkraft des HUD-Gehäuses (0 bis 255; Standard 170)."
    $description:es-ES: "Opacidad de fondo del chasis del HUD (0 a 255; predeterminado 170)."
    $description:pt-BR: "Opacidade de fundo do chassi do HUD (0 a 255; padrão 170)."
    $description:fr-FR: "Opacité d'arrière-plan du châssis HUD (0 à 255 ; par défaut 170)."
    $description:it-IT: "Opacità dello sfondo dello chassis HUD (da 0 a 255; predefinito 170)."
    $description:pl-PL: "Krycie tła obudowy HUD (od 0 do 255; domyślnie 170)."
    $description:ja-JP: "HUD シャーシの背景不透明度（0〜255、既定値 170）。"
    $description:ko-KR: "HUD 섀시의 배경 불투명도입니다(0~255, 기본값 170)."
    $description:uk-UA: "Непрозорість фону корпусу HUD (від 0 до 255; за замовчуванням 170)."
  $name: "⚡ Cyberpunk Sci-Fi HUD"
  $name:tr-TR: "⚡ Cyberpunk Sci-Fi HUD"
  $name:zh-CN: "⚡ Cyberpunk Sci-Fi HUD"
  $name:zh-TW: "⚡ Cyberpunk Sci-Fi HUD"
  $name:ru-RU: "⚡ Cyberpunk Sci-Fi HUD"
  $name:de-DE: "⚡ Cyberpunk Sci-Fi HUD"
  $name:es-ES: "⚡ Cyberpunk Sci-Fi HUD"
  $name:pt-BR: "⚡ Cyberpunk Sci-Fi HUD"
  $name:fr-FR: "⚡ Cyberpunk Sci-Fi HUD"
  $name:it-IT: "⚡ Cyberpunk Sci-Fi HUD"
  $name:pl-PL: "⚡ Cyberpunk Sci-Fi HUD"
  $name:ja-JP: "⚡ Cyberpunk Sci-Fi HUD"
  $name:ko-KR: "⚡ Cyberpunk Sci-Fi HUD"
  $name:uk-UA: "⚡ Cyberpunk Sci-Fi HUD"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#include <powrprof.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Documents;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wu = winrt::Windows::UI;

struct PixelThemeSettings {
    std::wstring dotShape;
    wu::Color ledColor;
    bool showBackplate;
    int backplateOpacity;
};

struct VerticalThemeSettings {
    int rowSpacing;
    int minuteOpacity;
    bool hasCustomTextColor;
    wu::Color textColor;
    std::wstring fontFamily;
    bool showShadow;
};

struct FlipThemeSettings {
    int cornerRadius;
    bool showSplitSeam;
    bool hasCustomDigitColor;
    wu::Color digitColor;
};

struct MinimalistThemeSettings {
    std::wstring fontWeight;
    bool hasCustomTextColor;
    wu::Color textColor;
    std::wstring fontFamily;
    bool showShadow;
};

struct CyberpunkThemeSettings {
    std::wstring colorPreset;
    bool showHeaderBar;
    int chassisOpacity;
};

struct AnsiThemeSettings {
    std::wstring colorPreset;
    bool showPromptBar;
    bool showGlow;
    int cardOpacity;
};

// Mod Configuration Settings
struct ModSettings {
    std::wstring clockStyle;
    std::wstring timeFormat;
    bool showSeconds;
    int glassOpacity;
    int clockFontSize;
    int verticalOffset;
    std::wstring clockAlignment;
    bool hideDefaultWidgets;
    bool showDateCapsule;
    bool showBatteryStatus;
    PixelThemeSettings pixel;
    VerticalThemeSettings vertical;
    FlipThemeSettings flip;
    MinimalistThemeSettings minimalist;
    CyberpunkThemeSettings cyberpunk;
    AnsiThemeSettings ansi;
} g_settings;

inline bool IsVerticalStyle(const std::wstring& style) {
    return style == L"verticalBahnschrift" ||
           style == L"verticalSegoe" ||
           style == L"verticalImpact";
}

inline std::wstring ReadStringSetting(const wchar_t* name, const wchar_t* fallback = L"") {
    auto val = WindhawkUtils::StringSetting::make(name);
    if (!val.get() || !*val.get()) return fallback;
    std::wstring s(val.get());
    size_t start = s.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return fallback;
    size_t end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

inline int HexCharToInt(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}

inline bool TryParseHexColor(const std::wstring& input, wu::Color& outColor) {
    if (input.empty()) return false;
    std::wstring hex = (input[0] == L'#') ? input.substr(1) : input;
    if (hex.size() == 3) {
        int r = HexCharToInt(hex[0]);
        int g = HexCharToInt(hex[1]);
        int b = HexCharToInt(hex[2]);
        if (r < 0 || g < 0 || b < 0) return false;
        outColor = wu::ColorHelper::FromArgb(255, (uint8_t)(r * 17), (uint8_t)(g * 17), (uint8_t)(b * 17));
        return true;
    }
    if (hex.size() != 6 && hex.size() != 8) return false;
    uint8_t bytes[4] = { 255, 0, 0, 0 };
    size_t offset = (hex.size() == 8) ? 0 : 1;
    for (size_t i = 0; i < hex.size(); i += 2) {
        int hi = HexCharToInt(hex[i]);
        int lo = HexCharToInt(hex[i + 1]);
        if (hi < 0 || lo < 0) return false;
        bytes[offset + (i / 2)] = (uint8_t)((hi << 4) | lo);
    }
    outColor = wu::ColorHelper::FromArgb(bytes[0], bytes[1], bytes[2], bytes[3]);
    return true;
}

inline bool ResolveColorSetting(const std::wstring& input, wu::Color& outColor) {
    if (input.empty() || input == L"default") return false;
    if (input == L"white")   { outColor = wu::Colors::White(); return true; }
    if (input == L"black")   { outColor = wu::ColorHelper::FromArgb(255, 18, 18, 18); return true; }
    if (input == L"silver")  { outColor = wu::ColorHelper::FromArgb(255, 200, 208, 216); return true; }
    if (input == L"red")     { outColor = wu::ColorHelper::FromArgb(255, 239, 68, 68); return true; }
    if (input == L"orange")  { outColor = wu::ColorHelper::FromArgb(255, 249, 115, 22); return true; }
    if (input == L"yellow")  { outColor = wu::ColorHelper::FromArgb(255, 250, 204, 21); return true; }
    if (input == L"cyan")    { outColor = wu::ColorHelper::FromArgb(255, 0, 245, 255); return true; }
    if (input == L"blue")    { outColor = wu::ColorHelper::FromArgb(255, 96, 165, 250); return true; }
    if (input == L"green")   { outColor = wu::ColorHelper::FromArgb(255, 0, 255, 136); return true; }
    if (input == L"amber")   { outColor = wu::ColorHelper::FromArgb(255, 255, 176, 0); return true; }
    if (input == L"coral")   { outColor = wu::ColorHelper::FromArgb(255, 255, 90, 95); return true; }
    if (input == L"magenta") { outColor = wu::ColorHelper::FromArgb(255, 255, 59, 154); return true; }
    if (input == L"violet")  { outColor = wu::ColorHelper::FromArgb(255, 167, 139, 250); return true; }
    return TryParseHexColor(input, outColor);
}

inline winrt::Windows::UI::Text::FontWeight ResolveFontWeight(
    const std::wstring& key,
    winrt::Windows::UI::Text::FontWeight fallback)
{
    if (key == L"thin") return winrt::Windows::UI::Text::FontWeights::Thin();
    if (key == L"light") return winrt::Windows::UI::Text::FontWeights::Light();
    if (key == L"normal") return winrt::Windows::UI::Text::FontWeights::Normal();
    if (key == L"semibold") return winrt::Windows::UI::Text::FontWeights::SemiBold();
    if (key == L"bold") return winrt::Windows::UI::Text::FontWeights::Bold();
    return fallback;
}

// Loads every setting. Before the UI hook is installed this may run on any
// thread; afterwards it must run on the lock screen UI thread.
void LoadSettings() {
    auto style = WindhawkUtils::StringSetting::make(L"clockStyle");
    g_settings.clockStyle = (style.get() && *style.get()) ? style.get() : L"pixel";
    // Migrate legacy "aura" users seamlessly to Minimalist Sonoma ("macos")
    if (g_settings.clockStyle == L"aura") {
        g_settings.clockStyle = L"macos";
    }
    if (g_settings.clockStyle != L"pixel" && g_settings.clockStyle != L"flip" &&
        g_settings.clockStyle != L"vintage" && g_settings.clockStyle != L"macos" &&
        g_settings.clockStyle != L"cyberpunk" && g_settings.clockStyle != L"ansiShadow" &&
        g_settings.clockStyle != L"verticalBahnschrift" &&
        g_settings.clockStyle != L"verticalSegoe" &&
        g_settings.clockStyle != L"verticalImpact" &&
        g_settings.clockStyle != L"defaultOnly") {
        g_settings.clockStyle = L"pixel";
    }

    auto fmt = WindhawkUtils::StringSetting::make(L"timeFormat");
    g_settings.timeFormat = (fmt.get() && *fmt.get()) ? fmt.get() : L"24h";
    if (g_settings.timeFormat != L"24h" && g_settings.timeFormat != L"12h") {
        g_settings.timeFormat = L"24h";
    }

    // Flip clocks and 2-row vertical stack clocks have no seconds display.
    // "auto" follows the theme default (off for Minimalist Sonoma, on for other supported styles).
    bool isVertical = IsVerticalStyle(g_settings.clockStyle);
    bool supportsSeconds =
        (g_settings.clockStyle != L"flip" && g_settings.clockStyle != L"vintage" && !isVertical);
    std::wstring rawShowSeconds = ReadStringSetting(L"showSeconds", L"auto");
    if (rawShowSeconds == L"on") {
        g_settings.showSeconds = supportsSeconds;
    } else if (rawShowSeconds == L"off") {
        g_settings.showSeconds = false;
    } else {
        g_settings.showSeconds = supportsSeconds && (g_settings.clockStyle != L"macos");
    }

    g_settings.glassOpacity = Wh_GetIntSetting(L"flipSettings.glassOpacity");
    if (g_settings.glassOpacity == 45) {
        int legacyOpacity = Wh_GetIntSetting(L"glassOpacity");
        if (legacyOpacity > 0 && legacyOpacity != 45) {
            g_settings.glassOpacity = legacyOpacity;
        }
    }
    if (g_settings.glassOpacity < 0) g_settings.glassOpacity = 0;
    if (g_settings.glassOpacity > 255) g_settings.glassOpacity = 255;

    int fontSetting = Wh_GetIntSetting(L"clockFontSize");
    if (fontSetting == 0) fontSetting = 140;
    if (fontSetting < 20) fontSetting = 20;
    if (fontSetting > 300) fontSetting = 300;
    g_settings.clockFontSize = fontSetting;

    g_settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");
    if (g_settings.verticalOffset < -200) g_settings.verticalOffset = -200;
    if (g_settings.verticalOffset > 400) g_settings.verticalOffset = 400;

    auto align = WindhawkUtils::StringSetting::make(L"clockAlignment");
    std::wstring rawAlign = (align.get() && *align.get()) ? align.get() : L"auto";
    if (rawAlign != L"auto" && rawAlign != L"center" && rawAlign != L"bottomLeft" &&
        rawAlign != L"left" && rawAlign != L"right") {
        rawAlign = L"auto";
    }

    // "auto" maps to the theme default; an explicit choice is always honored.
    g_settings.clockAlignment = (rawAlign == L"auto")
        ? (g_settings.clockStyle == L"macos" ? L"bottomLeft" : L"center")
        : rawAlign;

    g_settings.hideDefaultWidgets = Wh_GetIntSetting(L"hideDefaultWidgets") != 0;
    g_settings.showDateCapsule = Wh_GetIntSetting(L"showDateCapsule") != 0;
    g_settings.showBatteryStatus = Wh_GetIntSetting(L"showBatteryStatus") != 0;

    // 1. Retro Pixel settings group
    g_settings.pixel.dotShape = ReadStringSetting(L"pixelSettings.dotShape", L"circle");
    if (g_settings.pixel.dotShape != L"circle" &&
        g_settings.pixel.dotShape != L"square" &&
        g_settings.pixel.dotShape != L"rounded") {
        g_settings.pixel.dotShape = L"circle";
    }
    if (!ResolveColorSetting(ReadStringSetting(L"pixelSettings.ledColor", L"default"), g_settings.pixel.ledColor)) {
        g_settings.pixel.ledColor = wu::Colors::White();
    }
    g_settings.pixel.showBackplate = Wh_GetIntSetting(L"pixelSettings.showBackplate") != 0;
    g_settings.pixel.backplateOpacity = Wh_GetIntSetting(L"pixelSettings.backplateOpacity");
    if (g_settings.pixel.backplateOpacity < 0) g_settings.pixel.backplateOpacity = 0;
    if (g_settings.pixel.backplateOpacity > 255) g_settings.pixel.backplateOpacity = 255;

    // 2. Vertical Stack settings group
    g_settings.vertical.rowSpacing = Wh_GetIntSetting(L"verticalSettings.rowSpacing");
    if (g_settings.vertical.rowSpacing < -60) g_settings.vertical.rowSpacing = -60;
    if (g_settings.vertical.rowSpacing > 120) g_settings.vertical.rowSpacing = 120;
    g_settings.vertical.minuteOpacity = Wh_GetIntSetting(L"verticalSettings.minuteOpacity");
    if (g_settings.vertical.minuteOpacity != 0) {
        if (g_settings.vertical.minuteOpacity < 10) g_settings.vertical.minuteOpacity = 10;
        if (g_settings.vertical.minuteOpacity > 100) g_settings.vertical.minuteOpacity = 100;
    }
    g_settings.vertical.hasCustomTextColor =
        ResolveColorSetting(ReadStringSetting(L"verticalSettings.textColor", L"default"), g_settings.vertical.textColor);
    g_settings.vertical.fontFamily = ReadStringSetting(L"verticalSettings.fontFamily", L"default");
    if (g_settings.vertical.fontFamily == L"default") g_settings.vertical.fontFamily.clear();
    g_settings.vertical.showShadow = Wh_GetIntSetting(L"verticalSettings.showShadow") != 0;

    // 3. Flip Clock settings group
    g_settings.flip.cornerRadius = Wh_GetIntSetting(L"flipSettings.cornerRadius");
    if (g_settings.flip.cornerRadius < -1) g_settings.flip.cornerRadius = -1;
    if (g_settings.flip.cornerRadius > 48) g_settings.flip.cornerRadius = 48;
    g_settings.flip.showSplitSeam = Wh_GetIntSetting(L"flipSettings.showSplitSeam") != 0;
    g_settings.flip.hasCustomDigitColor =
        ResolveColorSetting(ReadStringSetting(L"flipSettings.digitColor", L"default"), g_settings.flip.digitColor);

    // 4. Minimalist Sonoma & Aura settings group
    g_settings.minimalist.fontWeight = ReadStringSetting(L"minimalistSettings.fontWeight", L"default");
    if (g_settings.minimalist.fontWeight != L"default" &&
        g_settings.minimalist.fontWeight != L"thin" &&
        g_settings.minimalist.fontWeight != L"light" &&
        g_settings.minimalist.fontWeight != L"normal" &&
        g_settings.minimalist.fontWeight != L"semibold" &&
        g_settings.minimalist.fontWeight != L"bold") {
        g_settings.minimalist.fontWeight = L"default";
    }
    g_settings.minimalist.hasCustomTextColor =
        ResolveColorSetting(ReadStringSetting(L"minimalistSettings.textColor", L"default"), g_settings.minimalist.textColor);
    g_settings.minimalist.fontFamily = ReadStringSetting(L"minimalistSettings.fontFamily", L"default");
    if (g_settings.minimalist.fontFamily == L"default") g_settings.minimalist.fontFamily.clear();
    g_settings.minimalist.showShadow = Wh_GetIntSetting(L"minimalistSettings.showShadow") != 0;

    // 5. Cyberpunk HUD settings group
    g_settings.cyberpunk.colorPreset = ReadStringSetting(L"cyberpunkSettings.colorPreset", L"default");
    if (g_settings.cyberpunk.colorPreset != L"default" &&
        g_settings.cyberpunk.colorPreset != L"matrix" &&
        g_settings.cyberpunk.colorPreset != L"crimson" &&
        g_settings.cyberpunk.colorPreset != L"violet") {
        g_settings.cyberpunk.colorPreset = L"default";
    }
    g_settings.cyberpunk.showHeaderBar = Wh_GetIntSetting(L"cyberpunkSettings.showHeaderBar") != 0;
    g_settings.cyberpunk.chassisOpacity = Wh_GetIntSetting(L"cyberpunkSettings.chassisOpacity");
    if (g_settings.cyberpunk.chassisOpacity < 0) g_settings.cyberpunk.chassisOpacity = 0;
    if (g_settings.cyberpunk.chassisOpacity > 255) g_settings.cyberpunk.chassisOpacity = 255;

    // 6. ANSI Shadow (Terminal ASCII) settings group
    g_settings.ansi.colorPreset = ReadStringSetting(L"ansiSettings.colorPreset", L"amber");
    if (g_settings.ansi.colorPreset != L"amber" &&
        g_settings.ansi.colorPreset != L"green" &&
        g_settings.ansi.colorPreset != L"cyan" &&
        g_settings.ansi.colorPreset != L"white") {
        g_settings.ansi.colorPreset = L"amber";
    }
    g_settings.ansi.showPromptBar = Wh_GetIntSetting(L"ansiSettings.showPromptBar") != 0;
    g_settings.ansi.showGlow = Wh_GetIntSetting(L"ansiSettings.showGlow") != 0;
    g_settings.ansi.cardOpacity = Wh_GetIntSetting(L"ansiSettings.cardOpacity");
    if (g_settings.ansi.cardOpacity < 0) g_settings.ansi.cardOpacity = 0;
    if (g_settings.ansi.cardOpacity > 255) g_settings.ansi.cardOpacity = 255;

    Wh_Log(L"Settings loaded: style=%s, fmt=%s, showSec=%d, opacity=%d, font=%d, vertOffset=%d, align=%s, hideWidgets=%d, showBattery=%d",
           g_settings.clockStyle.c_str(), g_settings.timeFormat.c_str(), g_settings.showSeconds, g_settings.glassOpacity,
           g_settings.clockFontSize, g_settings.verticalOffset, g_settings.clockAlignment.c_str(), g_settings.hideDefaultWidgets, g_settings.showBatteryStatus);
}

// Strong XAML references. These belong to the lock screen UI thread and must
// never be released by CRT static destructors at process exit. They are marked
// no_destroy and released manually from the UI thread in TeardownOnUiThread().
[[clang::no_destroy]] wuxc::Grid g_lockRootGrid{ nullptr };
[[clang::no_destroy]] wux::UIElement g_activeClockElement{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_secTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_ampmTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_dateTextBlock{ nullptr };

// Minimalist Sonoma (Clean Typography) Theme State
[[clang::no_destroy]] wuxc::TextBlock g_macosTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateShadowBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosAmPmShadowBlock{ nullptr };

// Vertical Stack Clock Themes State
[[clang::no_destroy]] wuxc::TextBlock g_verticalHourText{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_verticalHourShadow{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_verticalMinText{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_verticalMinShadow{ nullptr };

// Battery Capsule State
[[clang::no_destroy]] wuxc::Border g_batteryCapsule{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryIconText{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryStatusText{ nullptr };

// Cyberpunk HUD Pulsing State
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash1{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash2{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberLiveLabel{ nullptr };

// ANSI Shadow (Terminal ASCII) State
[[clang::no_destroy]] wuxc::TextBlock g_ansiArtTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_ansiShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_ansiCursorTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::Grid g_ansiShadowHost{ nullptr };
[[clang::no_destroy]] wuxm::SolidColorBrush g_ansiPrimaryBrush{ nullptr };
[[clang::no_destroy]] wuxm::SolidColorBrush g_ansiShadowBrush{ nullptr };

// Elements whose Visibility this mod changed. The original local value is kept
// so it can be restored without overwriting Windows' own bindings/state.
struct TrackedElement {
    wux::FrameworkElement element{ nullptr };
    winrt::Windows::Foundation::IInspectable originalValue{ nullptr };
    bool hadLocalValue = false;
};

// no_destroy: releasing these vectors in a CRT destructor at process exit would
// release XAML objects off the UI thread. They are cleared on the UI thread.
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenClockElements;
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenWidgetElements;

// Dirty-checking time cache for high-efficiency rendering (eliminates redundant DirectWrite invalidations)
struct TimeCache {
    int lastDisplayHour = -1;
    int lastMinute = -1;
    int lastSecond = -1;
    int lastDay = -1;
    int lastMonth = -1;
    int lastYear = -1;
    std::wstring lastAmPm;
    std::wstring lastAuraTime;
    std::wstring lastDateStr;
    std::wstring lastMacosTime;
    std::wstring lastMacosDate;
    std::wstring lastVerticalHour;
    std::wstring lastVerticalMin;
    std::wstring lastAnsiBanner;
    int lastBatteryPct = -1;
    bool lastIsCharging = false;

    void Invalidate() {
        lastDisplayHour = -1;
        lastMinute = -1;
        lastSecond = -1;
        lastDay = -1;
        lastMonth = -1;
        lastYear = -1;
        lastAmPm.clear();
        lastAuraTime.clear();
        lastDateStr.clear();
        lastMacosTime.clear();
        lastMacosDate.clear();
        lastVerticalHour.clear();
        lastVerticalMin.clear();
        lastAnsiBanner.clear();
        lastBatteryPct = -1;
        lastIsCharging = false;
    }
} g_timeCache;

// 7x11 Dot Matrix Font Table for Digits 0 to 9 with 2-Dot Stroke Thickness
// Each digit consists of 11 rows; each row uses the lower 7 bits (bit 6 = col 0, bit 0 = col 6).
static const uint8_t FONT_7x11[10][11] = {
    // 0
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 1
    { 0b0011100,
      0b0111100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0111110,
      0b0111110 },
    // 2
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0111111,
      0b1111110,
      0b1100000,
      0b1100000,
      0b1111111,
      0b1111111 },
    // 3
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0011110,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 4
    { 0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0000011 },
    // 5
    { 0b1111111,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b0111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 6
    { 0b0111110,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 7
    { 0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000110,
      0b0001100,
      0b0011000,
      0b0011000,
      0b0110000,
      0b0110000,
      0b0110000 },
    // 8
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b0111110,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 9
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111111,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 }
};

// Pixel Clock State
struct PixelDigitGrid {
    std::vector<wuxs::Shape> dots; // 77 dots per digit (7 cols x 11 rows)
    int lastVal = -1; // Cached last rendered digit to avoid redundant updates
};
// no_destroy: the six dot vectors hold XAML Ellipse references and must only be
// cleared from the UI thread (see ResetClockElementReferences()).
[[clang::no_destroy]] PixelDigitGrid g_pixelDigits[6]; // H1, H2, M1, M2, S1, S2
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushActiveDot{ nullptr };
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushInactiveDot{ nullptr };

// Win32 Battery Status Helper
struct BatteryInfo {
    bool hasBattery = false;
    bool isCharging = false;
    int percentage = -1;
};

BatteryInfo GetCurrentBatteryInfo() {
    BatteryInfo info;
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        // BatteryFlag 128 means "no system battery"; the unknown value (255)
        // also has that bit set. BatteryFlag 8 means "charging".
        if ((sps.BatteryFlag & 128) == 0 && sps.BatteryLifePercent <= 100) {
            info.hasBattery = true;
            info.percentage = (int)sps.BatteryLifePercent;
            info.isCharging = (sps.BatteryFlag & 8) != 0;
        }
    }
    return info;
}

// Build Universal Frosted Battery & Power Status Capsule
wux::UIElement CreateBatteryCapsule(bool alignLeft, bool isDarkTheme = false) {
    auto batteryInfo = GetCurrentBatteryInfo();
    if (!batteryInfo.hasBattery) {
        return nullptr; // Desktop PC without battery: do not render
    }

    wuxc::Border capsule;
    capsule.Name(L"WindhawkBatteryCapsule");
    if (isDarkTheme) {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(210, 14, 14, 14)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    } else {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 255, 255, 255)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(55, 255, 255, 255)));
    }
    capsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    capsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(10.0));
    capsule.Padding(wux::Thickness{ 12.0, 4.0, 14.0, 4.0 });
    capsule.Margin(wux::Thickness{ alignLeft ? 6.0 : 0.0, 12.0, 0.0, 0.0 });
    capsule.HorizontalAlignment(alignLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    wuxc::StackPanel panel;
    panel.Orientation(wuxc::Orientation::Horizontal);
    panel.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock iconText;
    iconText.Name(L"WindhawkBatteryIconText");
    iconText.Text(batteryInfo.isCharging ? L"⚡ " : L"🔋 ");
    iconText.FontSize(12.5);
    iconText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    iconText.Foreground(batteryInfo.isCharging 
        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)) // Gold / Yellow
        : wuxm::SolidColorBrush(wu::Colors::White()));
    iconText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(iconText);

    wuxc::TextBlock statusText;
    statusText.Name(L"WindhawkBatteryStatusText");
    WCHAR buf[32];
    swprintf_s(buf, L"%d%%", batteryInfo.percentage);
    statusText.Text(buf);
    statusText.FontSize(12.5);
    statusText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
    statusText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
    statusText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
    statusText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(statusText);

    capsule.Child(panel);
    g_batteryCapsule = capsule;
    g_batteryIconText = iconText;
    g_batteryStatusText = statusText;
    return capsule;
}

// Helper to apply root horizontal and vertical alignments (supports bottomLeft Sonoma style)
void ApplyAlignmentAndMargin(wuxc::Grid& rootContainer) {
    if (g_settings.clockAlignment == L"left") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 80.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    } else if (g_settings.clockAlignment == L"right") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Right);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 80.0, 0.0 });
    } else if (g_settings.clockAlignment == L"bottomLeft") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Bottom);
        rootContainer.Margin(wux::Thickness{ 80.0, 0.0, 0.0, 80.0 + (double)g_settings.verticalOffset });
    } else {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Center);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    }
}

// ANSI Shadow 6-row box-drawing glyph table for digits '0'..'9' (index 0..9) and ':' (index 10)
static const wchar_t* const ANSI_SHADOW_GLYPHS[11][6] = {
    // 0
    { L" ██████╗ ",
      L"██╔═████╗",
      L"██║██╔██║",
      L"████╔╝██║",
      L"╚██████╔╝",
      L" ╚═════╝ " },
    // 1
    { L" ██╗",
      L"███║",
      L"╚██║",
      L" ██║",
      L" ██║",
      L" ╚═╝" },
    // 2
    { L"██████╗ ",
      L"╚════██╗",
      L" █████╔╝",
      L"██╔═══╝ ",
      L"███████╗",
      L"╚══════╝" },
    // 3
    { L"██████╗ ",
      L"╚════██╗",
      L" █████╔╝",
      L" ╚═══██╗",
      L"██████╔╝",
      L"╚═════╝ " },
    // 4
    { L"██╗  ██╗",
      L"██║  ██║",
      L"███████║",
      L"╚════██║",
      L"     ██║",
      L"     ╚═╝" },
    // 5
    { L"███████╗",
      L"██╔════╝",
      L"███████╗",
      L"╚════██║",
      L"███████║",
      L"╚══════╝" },
    // 6
    { L" ██████╗ ",
      L"██╔════╝ ",
      L"███████╗ ",
      L"██╔═══██╗",
      L"╚██████╔╝",
      L" ╚═════╝ " },
    // 7
    { L"███████╗",
      L"╚════██║",
      L"    ██╔╝",
      L"   ██╔╝ ",
      L"   ██║  ",
      L"   ╚═╝  " },
    // 8
    { L" █████╗ ",
      L"██╔══██╗",
      L"╚█████╔╝",
      L"██╔══██╗",
      L"╚█████╔╝",
      L" ╚════╝ " },
    // 9
    { L" █████╗ ",
      L"██╔══██╗",
      L"╚██████║",
      L" ╚═══██║",
      L" █████╔╝",
      L" ╚════╝ " },
    // :
    { L"   ",
      L"██╗",
      L"╚═╝",
      L"██╗",
      L"╚═╝",
      L"   " }
};

inline std::wstring BuildAnsiShadowBanner(const std::wstring& timeStr) {
    std::wstring result;
    result.reserve(320);
    for (int row = 0; row < 6; row++) {
        if (row > 0) result += L"\n";
        bool firstGlyph = true;
        for (wchar_t ch : timeStr) {
            int idx = -1;
            if (ch >= L'0' && ch <= L'9') idx = ch - L'0';
            else if (ch == L':') idx = 10;
            if (idx < 0) continue;
            if (!firstGlyph) result += L"\u00A0";
            for (const wchar_t* p = ANSI_SHADOW_GLYPHS[idx][row]; *p; ++p) {
                result += (*p == L' ') ? L'\u00A0' : *p;
            }
            firstGlyph = false;
        }
    }
    return result;
}

// Populate ANSI Shadow TextBlock with two-tone Runs: solid blocks (█) use primaryBrush,
// box-drawing shadow strokes (╔ ╗ ║ ╚ ╝ ═) use shadowBrush for crisp 3D depth.
inline void SetAnsiShadowRuns(wuxc::TextBlock& tb, const std::wstring& banner,
                              const wuxm::SolidColorBrush& primaryBrush,
                              const wuxm::SolidColorBrush& shadowBrush) {
    auto inlines = tb.Inlines();
    inlines.Clear();
    if (!primaryBrush || !shadowBrush) {
        tb.Text(banner);
        return;
    }

    auto isPrimaryChar = [](wchar_t ch) {
        return ch == L'█' || ch == L' ' || ch == L'\u00A0' || ch == L'\n';
    };

    size_t i = 0;
    const size_t n = banner.size();
    while (i < n) {
        bool primary = isPrimaryChar(banner[i]);
        size_t start = i;
        while (i < n && isPrimaryChar(banner[i]) == primary) {
            i++;
        }
        wuxd::Run run;
        run.Text(banner.substr(start, i - start));
        run.Foreground(primary ? primaryBrush : shadowBrush);
        inlines.Append(run);
    }
}

// Update single pixel digit dots (7x11 matrix) with dirty-checking
void SetPixelDigitValue(PixelDigitGrid& digitRef, int val) {
    if (val < 0 || val > 9 || digitRef.dots.size() != 77 || !g_brushActiveDot || !g_brushInactiveDot) {
        return;
    }
    if (digitRef.lastVal == val) {
        return; // Value unchanged; skip re-applying brushes to 77 Ellipses
    }
    digitRef.lastVal = val;

    const uint8_t* rows = FONT_7x11[val];
    for (int r = 0; r < 11; r++) {
        uint8_t rowBits = rows[r];
        for (int c = 0; c < 7; c++) {
            bool active = (rowBits & (1 << (6 - c))) != 0;
            int idx = r * 7 + c;
            digitRef.dots[idx].Fill(active ? g_brushActiveDot : g_brushInactiveDot);
        }
    }
}

// Update Time and Date with dirty-checking cache (eliminates 95%+ redundant DirectWrite passes)
void UpdateTimeAndDate() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    bool is12h = (g_settings.timeFormat == L"12h");
    int rawHour = st.wHour;
    int displayHour = rawHour;
    bool isPM = (rawHour >= 12);
    const wchar_t* ampmStr = isPM ? L"PM" : L"AM";
    if (is12h) {
        displayHour = rawHour % 12;
        if (displayHour == 0) displayHour = 12;
    }

    bool hourChanged = (displayHour != g_timeCache.lastDisplayHour);
    bool minuteChanged = (st.wMinute != g_timeCache.lastMinute);
    bool secondChanged = (st.wSecond != g_timeCache.lastSecond);
    bool ampmChanged = (g_timeCache.lastAmPm != ampmStr);
    bool dateChanged = (st.wDay != g_timeCache.lastDay || st.wMonth != g_timeCache.lastMonth || st.wYear != g_timeCache.lastYear);

    if (g_ampmTextBlock && ampmChanged) {
        g_ampmTextBlock.Text(ampmStr);
        if (g_macosAmPmShadowBlock) {
            g_macosAmPmShadowBlock.Text(ampmStr);
        }
    }
    // Update the cache even when the current style renders AM/PM inside its
    // main text. Otherwise "ampmChanged" stays true and defeats dirty checking.
    g_timeCache.lastAmPm = ampmStr;

    if (g_settings.clockStyle == L"pixel") {
        int h1 = displayHour / 10;
        int h2 = displayHour % 10;
        int m1 = st.wMinute / 10;
        int m2 = st.wMinute % 10;

        SetPixelDigitValue(g_pixelDigits[0], h1);
        SetPixelDigitValue(g_pixelDigits[1], h2);
        SetPixelDigitValue(g_pixelDigits[2], m1);
        SetPixelDigitValue(g_pixelDigits[3], m2);

        if (g_settings.showSeconds) {
            int s1 = st.wSecond / 10;
            int s2 = st.wSecond % 10;
            SetPixelDigitValue(g_pixelDigits[4], s1);
            SetPixelDigitValue(g_pixelDigits[5], s2);
        }
    } else if (g_settings.clockStyle == L"macos") {
        bool shouldUpdateTime = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged);
        if (g_macosTimeTextBlock && (shouldUpdateTime || g_timeCache.lastMacosTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d", displayHour, st.wMinute);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_macosTimeTextBlock.Text(timeBuf);
            if (g_macosShadowTextBlock) {
                g_macosShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastMacosTime = timeBuf;
        }

        if (g_macosDateTextBlock && (dateChanged || g_timeCache.lastMacosDate.empty())) {
            WCHAR dateBuf[128];
            if (GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"dddd, d MMMM", dateBuf, ARRAYSIZE(dateBuf)) == 0) {
                GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf));
            }
            g_macosDateTextBlock.Text(dateBuf);
            if (g_macosDateShadowBlock) {
                g_macosDateShadowBlock.Text(dateBuf);
            }
            g_timeCache.lastMacosDate = dateBuf;
        }
    } else if (g_settings.clockStyle == L"aura") {
        bool shouldUpdateAura = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged || ampmChanged);
        if (g_auraTimeTextBlock && (shouldUpdateAura || g_timeCache.lastAuraTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d %s", displayHour, st.wMinute, ampmStr);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_auraTimeTextBlock.Text(timeBuf);
            if (g_auraShadowTextBlock) {
                g_auraShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastAuraTime = timeBuf;
        }
    } else if (IsVerticalStyle(g_settings.clockStyle)) {
        if (g_verticalHourText && (hourChanged || g_timeCache.lastVerticalHour.empty())) {
            WCHAR hourBuf[8];
            swprintf_s(hourBuf, L"%02d", displayHour);
            g_verticalHourText.Text(hourBuf);
            if (g_verticalHourShadow) {
                g_verticalHourShadow.Text(hourBuf);
            }
            g_timeCache.lastVerticalHour = hourBuf;
        }

        if (g_verticalMinText && (minuteChanged || g_timeCache.lastVerticalMin.empty())) {
            WCHAR minBuf[8];
            swprintf_s(minBuf, L"%02d", st.wMinute);
            g_verticalMinText.Text(minBuf);
            if (g_verticalMinShadow) {
                g_verticalMinShadow.Text(minBuf);
            }
            g_timeCache.lastVerticalMin = minBuf;
        }
    } else if (g_settings.clockStyle == L"ansiShadow") {
        if (g_ansiArtTextBlock && (hourChanged || minuteChanged || g_timeCache.lastAnsiBanner.empty())) {
            WCHAR timeBuf[16];
            if (is12h) {
                swprintf_s(timeBuf, L"%d:%02d", displayHour, st.wMinute);
            } else {
                swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
            }
            std::wstring banner = BuildAnsiShadowBanner(timeBuf);
            SetAnsiShadowRuns(g_ansiArtTextBlock, banner, g_ansiPrimaryBrush, g_ansiShadowBrush);
            if (g_ansiShadowTextBlock) {
                g_ansiShadowTextBlock.Text(banner);
            }
            g_timeCache.lastAnsiBanner = banner;
        }

        if (g_secTextBlock && g_settings.showSeconds && secondChanged) {
            WCHAR secBuf[8];
            swprintf_s(secBuf, L":%02d", st.wSecond);
            g_secTextBlock.Text(secBuf);
        }

        if (g_ansiCursorTextBlock && secondChanged) {
            g_ansiCursorTextBlock.Opacity((st.wSecond % 2 == 0) ? 0.95 : 0.15);
        }
    } else {
        // Flip Clock, Retro Vintage, Cyberpunk HUD
        if (g_hourTextBlock && hourChanged) {
            WCHAR hourBuf[8];
            // Text-based 12-hour clocks omit the leading zero consistently.
            if (is12h) {
                swprintf_s(hourBuf, L"%d", displayHour);
            } else {
                swprintf_s(hourBuf, L"%02d", displayHour);
            }
            g_hourTextBlock.Text(hourBuf);
            if (g_hourBotTextBlock) {
                g_hourBotTextBlock.Text(hourBuf);
            }
        }

        if (g_minTextBlock && minuteChanged) {
            WCHAR minBuf[8];
            swprintf_s(minBuf, L"%02d", st.wMinute);
            g_minTextBlock.Text(minBuf);
            if (g_minBotTextBlock) {
                g_minBotTextBlock.Text(minBuf);
            }
        }

        if (g_secTextBlock && g_settings.showSeconds && secondChanged) {
            WCHAR secBuf[8];
            swprintf_s(secBuf, L"%02d", st.wSecond);
            g_secTextBlock.Text(secBuf);
        }

        // Cyberpunk HUD Tactical Separator Pulse
        if (g_settings.clockStyle == L"cyberpunk" && secondChanged) {
            bool pulseState = (st.wSecond % 2 == 0);
            if (g_cyberSlash1) {
                g_cyberSlash1.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberSlash2) {
                g_cyberSlash2.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberLiveLabel) {
                g_cyberLiveLabel.Opacity(pulseState ? 1.0 : 0.55);
            }
        }
    }

    // Update the date cache even when the current style has no date capsule.
    // Otherwise "dateChanged" stays true forever for the Sonoma style, which
    // would format and assign the date on every tick.
    if (dateChanged) {
        g_timeCache.lastDay = st.wDay;
        g_timeCache.lastMonth = st.wMonth;
        g_timeCache.lastYear = st.wYear;
    }

    if (g_dateTextBlock && g_settings.showDateCapsule && (dateChanged || g_timeCache.lastDateStr.empty())) {
        WCHAR dateBuf[128] = {0};
        bool isVertical = IsVerticalStyle(g_settings.clockStyle);
        bool formatted = false;
        if (isVertical) {
            // Preserves natural locale word order (e.g. "31 Ekim Cumartesi", "Saturday, October 31", "10月31日 星期六")
            // by taking system long date and removing the redundant 4-digit year.
            WCHAR rawDate[128];
            if (GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, rawDate, ARRAYSIZE(rawDate)) > 0) {
                WCHAR yearStr[16];
                swprintf_s(yearStr, L"%d", st.wYear);
                std::wstring s(rawDate);
                size_t pos = s.find(yearStr);
                if (pos != std::wstring::npos) {
                    size_t eraseStart = pos;
                    size_t eraseCount = wcslen(yearStr);
                    if (eraseStart + eraseCount < s.size() && (s[eraseStart + eraseCount] == L'年' || s[eraseStart + eraseCount] == L'년')) {
                        eraseCount++;
                    }
                    if (eraseStart + eraseCount + 3 <= s.size()) {
                        if (s.substr(eraseStart + eraseCount, 3) == L" г." || s.substr(eraseStart + eraseCount, 3) == L" р.") {
                            eraseCount += 3;
                        }
                    }
                    if (eraseStart >= 2 && s[eraseStart - 2] == L',' && s[eraseStart - 1] == L' ') {
                        eraseStart -= 2;
                        eraseCount += 2;
                    } else if (eraseStart >= 1 && s[eraseStart - 1] == L' ') {
                        eraseStart -= 1;
                        eraseCount += 1;
                    } else if (eraseStart + eraseCount < s.size() && s[eraseStart + eraseCount] == L' ') {
                        eraseCount += 1;
                    }
                    s.erase(eraseStart, eraseCount);
                    while (!s.empty() && (s.front() == L' ' || s.front() == L',')) s.erase(s.begin());
                    while (!s.empty() && (s.back() == L' ' || s.back() == L',')) s.pop_back();
                }
                if (!s.empty()) {
                    wcsncpy_s(dateBuf, s.c_str(), _TRUNCATE);
                    formatted = true;
                }
            }
        }
        if (formatted ||
            GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf)) > 0) {
            g_dateTextBlock.Text(dateBuf);
            g_timeCache.lastDateStr = dateBuf;
        }
    }

    // Battery percentage and power status update
    if (g_batteryStatusText && g_settings.showBatteryStatus && (minuteChanged || g_timeCache.lastBatteryPct == -1)) {
        auto bInfo = GetCurrentBatteryInfo();
        if (bInfo.hasBattery) {
            if (bInfo.percentage != g_timeCache.lastBatteryPct || bInfo.isCharging != g_timeCache.lastIsCharging) {
                WCHAR bBuf[32];
                if (g_settings.clockStyle == L"cyberpunk") {
                    swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
                } else {
                    swprintf_s(bBuf, L"%d%%", bInfo.percentage);
                }
                g_batteryStatusText.Text(bBuf);
                if (g_batteryIconText) {
                    g_batteryIconText.Text(bInfo.isCharging ? L"⚡ " : L"🔋 ");
                    g_batteryIconText.Foreground(bInfo.isCharging 
                        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10))
                        : wuxm::SolidColorBrush(wu::Colors::White()));
                }
                g_timeCache.lastBatteryPct = bInfo.percentage;
                g_timeCache.lastIsCharging = bInfo.isCharging;
            }
        }
    }

    g_timeCache.lastDisplayHour = displayHour;
    g_timeCache.lastMinute = st.wMinute;
    g_timeCache.lastSecond = st.wSecond;
}

// Build Single Flip Card Element (Two physically split upper/lower flaps with clean scissors-cut gap)
wuxc::Grid CreateFlipCard(double width, double height, double cornerRadius, double fontSize,
                          wuxc::TextBlock& outTopText, wuxc::TextBlock& outBotText,
                          const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    double seamGap = g_settings.flip.showSplitSeam ? 4.0 : 0.0;
    double halfHeight = (height - seamGap) / 2.0;
    double innerCutRadius = g_settings.flip.showSplitSeam ? 2.0 : 0.0;
    wu::Color digitColor = g_settings.flip.hasCustomDigitColor
        ? g_settings.flip.digitColor
        : wu::Colors::White();

    // Optical centering offset for Segoe UI Variable ExtraBold: aligns the
    // digit to the split line so both halves are visually 50/50.
    double opticalOffset = fontSize * 0.045;

    // --- 1. TOP FLAP (upper half card & digits) ---
    wuxc::Grid topContainer;
    topContainer.Width(width);
    topContainer.Height(height);

    wuxm::RectangleGeometry topClip;
    topClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, 0.0f, (float)width, (float)halfHeight });
    topContainer.Clip(topClip);

    // Top flap acrylic body & border: rounded top corners, 2 px inner cut,
    // open bottom edge.
    wuxc::Border topFlap;
    topFlap.Width(width);
    topFlap.Height(halfHeight);
    topFlap.VerticalAlignment(wux::VerticalAlignment::Top);
    topFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    topFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    topFlap.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 0.0 });
    topFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(cornerRadius, cornerRadius, innerCutRadius, innerCutRadius));
    topContainer.Children().Append(topFlap);

    // Top digit: centered in the card; the lower half is clipped at the split.
    wuxc::TextBlock topText;
    topText.Name(textName);
    topText.FontSize(fontSize);
    topText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    topText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    topText.Foreground(wuxm::SolidColorBrush(digitColor));
    topText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    topText.VerticalAlignment(wux::VerticalAlignment::Center);
    topText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(topText, wux::FontNumeralAlignment::Tabular);
    topContainer.Children().Append(topText);

    // AM/PM badge on the clock card (12-hour format, top-left of the top flap).
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 14.0, 12.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(45, 255, 255, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(75, 255, 255, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
        ampmBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkFlipAmPmText");
        ampmText.FontSize(12.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        topContainer.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    // --- 2. BOTTOM FLAP (lower half card & digits) ---
    wuxc::Grid botContainer;
    botContainer.Width(width);
    botContainer.Height(height);

    wuxm::RectangleGeometry botClip;
    botClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, (float)(halfHeight + seamGap), (float)width, (float)halfHeight });
    botContainer.Clip(botClip);

    // Bottom flap acrylic body & border: 2 px inner cut, rounded bottom
    // corners, open top edge.
    wuxc::Border bottomFlap;
    bottomFlap.Width(width);
    bottomFlap.Height(halfHeight);
    bottomFlap.VerticalAlignment(wux::VerticalAlignment::Bottom);
    bottomFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    bottomFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    bottomFlap.BorderThickness(wux::Thickness{ 1.5, 0.0, 1.5, 1.5 });
    bottomFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(innerCutRadius, innerCutRadius, cornerRadius, cornerRadius));
    botContainer.Children().Append(bottomFlap);

    // Bottom digit: same center coordinates as the top digit; the upper half
    // is clipped at the split.
    std::wstring botName = std::wstring(textName) + L"_Bot";
    wuxc::TextBlock botText;
    botText.Name(botName);
    botText.FontSize(fontSize);
    botText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    botText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    botText.Foreground(wuxm::SolidColorBrush(digitColor));
    botText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    botText.VerticalAlignment(wux::VerticalAlignment::Center);
    botText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(botText, wux::FontNumeralAlignment::Tabular);
    botContainer.Children().Append(botText);

    card.Children().Append(topContainer);
    card.Children().Append(botContainer);

    outTopText = topText;
    outBotText = botText;
    return card;
}

// Build Flip Clock UI Element
wux::UIElement BuildFlipClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkFlipClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(22.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = (g_settings.flip.cornerRadius >= 0) ? (double)g_settings.flip.cornerRadius : 22.0;

    // 1. Hour Card
    auto hourCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, g_hourBotTextBlock, L"WindhawkFlipHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, g_minBotTextBlock, L"WindhawkFlipMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Date Capsule
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(35, 255, 255, 255)));
        dateCapsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(60, 255, 255, 255)));
        dateCapsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(12.0));
        dateCapsule.Padding(wux::Thickness{ 20.0, 8.0, 20.0, 8.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkFlipDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Single Vintage Flip Card Element (Retro Mechanical Airport/Station Solari Style)
wuxc::Grid CreateVintageFlipCard(double width, double height, double cornerRadius, double fontSize, wuxc::TextBlock& outText, const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    // Fliqlo matte charcoal black flap body (#0e0e0e) - frameless matte finish.
    wuxc::Border flapBackplate;
    flapBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(242, 14, 14, 14)));
    flapBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(cornerRadius));

    // Fliqlo Original Matte Light Silver Digit Typography (#b9b9b9 / rgb(185, 185, 185))
    wu::Color vintageDigitColor = g_settings.flip.hasCustomDigitColor
        ? g_settings.flip.digitColor
        : wu::ColorHelper::FromArgb(255, 185, 185, 185);
    wuxc::TextBlock cardText;
    cardText.Name(textName);
    cardText.FontSize(fontSize);
    cardText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    cardText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    cardText.Foreground(wuxm::SolidColorBrush(vintageDigitColor));
    cardText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardText.VerticalAlignment(wux::VerticalAlignment::Center);
    double opticalOffset = fontSize * 0.045;
    cardText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(cardText, wux::FontNumeralAlignment::Tabular);

    // Center Mechanical Split: Upper shadow + Lower light catch bevel + Side axle hinge clips
    wuxc::Grid splitSeam;
    splitSeam.Height(10.0);
    splitSeam.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    splitSeam.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::StackPanel seamLines;
    seamLines.Orientation(wuxc::Orientation::Vertical);
    seamLines.VerticalAlignment(wux::VerticalAlignment::Center);

    // Deep shadow groove between top and bottom flaps
    wuxs::Rectangle shadowGroove;
    shadowGroove.Height(2.0);
    shadowGroove.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    shadowGroove.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 0, 0)));
    shadowGroove.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    // Subtle edge highlight on bottom flap catching ambient overhead light
    wuxs::Rectangle bevelHighlight;
    bevelHighlight.Height(1.0);
    bevelHighlight.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    bevelHighlight.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    bevelHighlight.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    seamLines.Children().Append(shadowGroove);
    seamLines.Children().Append(bevelHighlight);
    splitSeam.Children().Append(seamLines);

    // Left and Right Axle Clips (Mechanical hinge pins)
    wuxs::Rectangle leftClip;
    leftClip.Width(4.0);
    leftClip.Height(9.0);
    leftClip.RadiusX(1.0);
    leftClip.RadiusY(1.0);
    leftClip.HorizontalAlignment(wux::HorizontalAlignment::Left);
    leftClip.VerticalAlignment(wux::VerticalAlignment::Center);
    leftClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    wuxs::Rectangle rightClip;
    rightClip.Width(4.0);
    rightClip.Height(9.0);
    rightClip.RadiusX(1.0);
    rightClip.RadiusY(1.0);
    rightClip.HorizontalAlignment(wux::HorizontalAlignment::Right);
    rightClip.VerticalAlignment(wux::VerticalAlignment::Center);
    rightClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    splitSeam.Children().Append(leftClip);
    splitSeam.Children().Append(rightClip);

    card.Children().Append(flapBackplate);
    card.Children().Append(cardText);
    if (g_settings.flip.showSplitSeam) {
        card.Children().Append(splitSeam);
    }
    outText = cardText;

    // Vintage Mechanical AM/PM Tag on Hour Card (12h format)
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 12.0, 10.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 14, 14, 14)));
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
        ampmBadge.Padding(wux::Thickness{ 6.0, 1.5, 6.0, 1.5 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkVintageAmPmText");
        ampmText.FontSize(11.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        card.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    return card;
}

// Build Retro Vintage Flip Clock UI Element
wux::UIElement BuildVintageClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkVintageClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(20.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = (g_settings.flip.cornerRadius >= 0) ? (double)g_settings.flip.cornerRadius : 18.0;

    // 1. Hour Card
    auto hourCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, L"WindhawkVintageHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, L"WindhawkVintageMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Vintage Date Plate (Fliqlo Matte Dark Style)
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 14, 14, 14)));
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(8.0));
        dateCapsule.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkVintageDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 185, 185, 185)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false, true);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Cyberpunk Sci-Fi HUD Terminal Clock Element (Authentic Netrunner / Tactical Cyberpunk HUD)
wux::UIElement BuildCyberpunkClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkCyberpunkHUDContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Resolve tactical palette based on user preset
    wu::Color colPrimary = wu::ColorHelper::FromArgb(255, 0, 245, 255);    // Electric Neon Cyan
    wu::Color colAccent1 = wu::ColorHelper::FromArgb(255, 252, 238, 10);   // Acid Yellow
    wu::Color colAccent2 = wu::ColorHelper::FromArgb(255, 255, 0, 128);    // Hot Magenta
    wu::Color colDigits  = wu::ColorHelper::FromArgb(255, 235, 252, 255);  // Electric White-Cyan
    if (g_settings.cyberpunk.colorPreset == L"matrix") {
        colPrimary = wu::ColorHelper::FromArgb(255, 0, 255, 136);
        colAccent1 = wu::ColorHelper::FromArgb(255, 180, 255, 120);
        colAccent2 = wu::ColorHelper::FromArgb(255, 0, 210, 106);
        colDigits  = wu::ColorHelper::FromArgb(255, 220, 255, 235);
    } else if (g_settings.cyberpunk.colorPreset == L"crimson") {
        colPrimary = wu::ColorHelper::FromArgb(255, 255, 176, 0);
        colAccent1 = wu::ColorHelper::FromArgb(255, 255, 45, 85);
        colAccent2 = wu::ColorHelper::FromArgb(255, 255, 120, 0);
        colDigits  = wu::ColorHelper::FromArgb(255, 255, 235, 235);
    } else if (g_settings.cyberpunk.colorPreset == L"violet") {
        colPrimary = wu::ColorHelper::FromArgb(255, 0, 245, 255);
        colAccent1 = wu::ColorHelper::FromArgb(255, 191, 90, 242);
        colAccent2 = wu::ColorHelper::FromArgb(255, 168, 85, 247);
        colDigits  = wu::ColorHelper::FromArgb(255, 242, 235, 255);
    }
    auto withAlpha = [](wu::Color c, uint8_t a) {
        return wu::ColorHelper::FromArgb(a, c.R, c.G, c.B);
    };

    // Outer Tactical HUD Bracket Frame
    wuxc::Grid hudFrame;
    hudFrame.HorizontalAlignment(wux::HorizontalAlignment::Center);
    hudFrame.VerticalAlignment(wux::VerticalAlignment::Center);

    // 1. Dark Obsidian Glass Chassis with Neon Edge
    wuxc::Border chassis;
    chassis.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.cyberpunk.chassisOpacity, 8, 10, 18)));
    chassis.BorderBrush(wuxm::SolidColorBrush(withAlpha(colPrimary, 130)));
    chassis.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 1.5 });
    chassis.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
    chassis.Padding(wux::Thickness{ 32.0, 18.0, 32.0, 20.0 });

    wuxc::StackPanel hudContent;
    hudContent.Orientation(wuxc::Orientation::Vertical);
    hudContent.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // --- TOP CYBER HEADER BAR ---
    if (g_settings.cyberpunk.showHeaderBar) {
        wuxc::Grid topBar;
        topBar.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        topBar.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 14.0 });

        // Left Tag: [ SYS.LOC // TIME_SYNC ]
        wuxc::TextBlock topTag;
        topTag.Text(L"SYS.LOC // TIME_SYNC");
        topTag.FontSize(11.0);
        topTag.FontFamily(wuxm::FontFamily(L"Consolas"));
        topTag.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        topTag.Foreground(wuxm::SolidColorBrush(withAlpha(colPrimary, 220)));
        topTag.HorizontalAlignment(wux::HorizontalAlignment::Left);
        topTag.VerticalAlignment(wux::VerticalAlignment::Center);

        // Right Telemetry Blocks: 3 Primary + 1 Accent1
        wuxc::StackPanel telemetryPills;
        telemetryPills.Orientation(wuxc::Orientation::Horizontal);
        telemetryPills.HorizontalAlignment(wux::HorizontalAlignment::Right);
        telemetryPills.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock liveLabel;
        liveLabel.Text(L"ONLINE ");
        liveLabel.FontSize(10.0);
        liveLabel.FontFamily(wuxm::FontFamily(L"Consolas"));
        liveLabel.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        liveLabel.Foreground(wuxm::SolidColorBrush(withAlpha(colAccent1, 220)));
        liveLabel.VerticalAlignment(wux::VerticalAlignment::Center);
        telemetryPills.Children().Append(liveLabel);
        g_cyberLiveLabel = liveLabel;

        for (int i = 0; i < 3; i++) {
            wuxs::Rectangle pill;
            pill.Width(6.0);
            pill.Height(9.0);
            pill.RadiusX(1.0);
            pill.RadiusY(1.0);
            pill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
            pill.Fill(wuxm::SolidColorBrush(withAlpha(colPrimary, 230)));
            telemetryPills.Children().Append(pill);
        }
        wuxs::Rectangle yellowPill;
        yellowPill.Width(6.0);
        yellowPill.Height(9.0);
        yellowPill.RadiusX(1.0);
        yellowPill.RadiusY(1.0);
        yellowPill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
        yellowPill.Fill(wuxm::SolidColorBrush(withAlpha(colAccent1, 250)));
        telemetryPills.Children().Append(yellowPill);

        // Tactical laptop battery telemetry (if battery present)
        auto bInfo = GetCurrentBatteryInfo();
        if (g_settings.showBatteryStatus && bInfo.hasBattery) {
            wuxc::Border cyberBat;
            cyberBat.Background(wuxm::SolidColorBrush(withAlpha(colPrimary, 40)));
            cyberBat.BorderBrush(wuxm::SolidColorBrush(withAlpha(colPrimary, 90)));
            cyberBat.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
            cyberBat.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
            cyberBat.Padding(wux::Thickness{ 6.0, 1.0, 6.0, 1.0 });
            cyberBat.Margin(wux::Thickness{ 6.0, 0.0, 0.0, 0.0 });
            cyberBat.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::TextBlock batText;
            batText.Name(L"WindhawkCyberBatteryText");
            WCHAR bBuf[32];
            swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
            batText.Text(bBuf);
            batText.FontSize(9.5);
            batText.FontFamily(wuxm::FontFamily(L"Consolas"));
            batText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
            batText.Foreground(wuxm::SolidColorBrush(withAlpha(colPrimary, 240)));
            cyberBat.Child(batText);
            telemetryPills.Children().Append(cyberBat);
            g_batteryStatusText = batText;
        }

        topBar.Children().Append(topTag);
        topBar.Children().Append(telemetryPills);
        hudContent.Children().Append(topBar);

        // Top Header Divider
        wuxs::Rectangle headerDivider;
        headerDivider.Height(1.0);
        headerDivider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        headerDivider.Fill(wuxm::SolidColorBrush(withAlpha(colPrimary, 70)));
        headerDivider.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 16.0 });
        hudContent.Children().Append(headerDivider);
    }

    // --- CENTER DIGITS DISPLAY ---
    wuxc::StackPanel digitsPanel;
    digitsPanel.Orientation(wuxc::Orientation::Horizontal);
    digitsPanel.HorizontalAlignment(wux::HorizontalAlignment::Center);
    digitsPanel.VerticalAlignment(wux::VerticalAlignment::Center);

    double numFontSize = (double)g_settings.clockFontSize * 1.05;
    double digitBlockWidth = numFontSize * 1.28;

    // Hour
    wuxc::TextBlock hourText;
    hourText.Name(L"WindhawkCyberHourText");
    hourText.FontSize(numFontSize);
    hourText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    hourText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    hourText.Foreground(wuxm::SolidColorBrush(colDigits));
    hourText.Width(digitBlockWidth);
    hourText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(hourText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(hourText);
    g_hourTextBlock = hourText;

    // Slashes 1 (//)
    wuxc::TextBlock slash1;
    slash1.Text(L" // ");
    slash1.FontSize(numFontSize * 0.7);
    slash1.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    slash1.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    slash1.Foreground(wuxm::SolidColorBrush(withAlpha(colAccent1, 230)));
    slash1.VerticalAlignment(wux::VerticalAlignment::Center);
    slash1.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
    digitsPanel.Children().Append(slash1);
    g_cyberSlash1 = slash1;

    // Minute
    wuxc::TextBlock minText;
    minText.Name(L"WindhawkCyberMinText");
    minText.FontSize(numFontSize);
    minText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    minText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    minText.Foreground(wuxm::SolidColorBrush(colDigits));
    minText.Width(digitBlockWidth);
    minText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(minText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(minText);
    g_minTextBlock = minText;

    // Seconds (if enabled)
    if (g_settings.showSeconds) {
        wuxc::TextBlock slash2;
        slash2.Text(L" // ");
        slash2.FontSize(numFontSize * 0.7);
        slash2.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        slash2.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
        slash2.Foreground(wuxm::SolidColorBrush(withAlpha(colAccent2, 240)));
        slash2.VerticalAlignment(wux::VerticalAlignment::Center);
        slash2.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
        digitsPanel.Children().Append(slash2);
        g_cyberSlash2 = slash2;

        wuxc::TextBlock secText;
        secText.Name(L"WindhawkCyberSecText");
        secText.FontSize(numFontSize);
        secText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        secText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        secText.Foreground(wuxm::SolidColorBrush(colDigits));
        secText.Width(digitBlockWidth);
        secText.TextAlignment(wux::TextAlignment::Center);
        wuxd::Typography::SetNumeralAlignment(secText, wux::FontNumeralAlignment::Tabular);
        digitsPanel.Children().Append(secText);
        g_secTextBlock = secText;
    }

    hudContent.Children().Append(digitsPanel);

    // Bottom Scanline Bar (Dotted / Segmented Laser Track)
    wuxs::Rectangle scanline;
    scanline.Height(2.0);
    scanline.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    scanline.Fill(wuxm::SolidColorBrush(withAlpha(colPrimary, 90)));
    scanline.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 14.0 });
    hudContent.Children().Append(scanline);

    // --- BOTTOM TELEMETRY DATE & AM/PM BAR ---
    bool is12h = (g_settings.timeFormat == L"12h");
    if (g_settings.showDateCapsule || is12h) {
        wuxc::StackPanel bottomBar;
        bottomBar.Orientation(wuxc::Orientation::Horizontal);
        bottomBar.HorizontalAlignment(wux::HorizontalAlignment::Center);
        bottomBar.VerticalAlignment(wux::VerticalAlignment::Center);

        if (g_settings.showDateCapsule) {
            // Telemetry Tag Badge
            wuxc::Border tagBadge;
            tagBadge.Background(wuxm::SolidColorBrush(withAlpha(colAccent1, 235)));
            tagBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
            tagBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });
            tagBadge.Margin(wux::Thickness{ 0.0, 0.0, 10.0, 0.0 });
            tagBadge.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::TextBlock tagText;
            tagText.Text(L"DATE.LOG");
            tagText.FontSize(11.0);
            tagText.FontFamily(wuxm::FontFamily(L"Consolas"));
            tagText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
            tagText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 10, 12, 18))); // Deep Black
            tagBadge.Child(tagText);
            bottomBar.Children().Append(tagBadge);

            // Date String
            wuxc::TextBlock dateText;
            dateText.Name(L"WindhawkCyberDateText");
            dateText.FontSize(15.0);
            dateText.FontFamily(wuxm::FontFamily(L"Consolas"));
            dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
            dateText.Foreground(wuxm::SolidColorBrush(withAlpha(colPrimary, 240)));
            dateText.VerticalAlignment(wux::VerticalAlignment::Center);
            bottomBar.Children().Append(dateText);
            g_dateTextBlock = dateText;
        }

        if (is12h) {
            wuxc::Border ampmBadge;
            ampmBadge.Background(wuxm::SolidColorBrush(withAlpha(colPrimary, 50)));
            ampmBadge.BorderBrush(wuxm::SolidColorBrush(withAlpha(colPrimary, 180)));
            ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
            ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
            ampmBadge.Padding(wux::Thickness{ 8.0, 2.5, 8.0, 2.5 });
            ampmBadge.Margin(wux::Thickness{ g_settings.showDateCapsule ? 12.0 : 0.0, 0.0, 0.0, 0.0 });
            ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::TextBlock ampmText;
            ampmText.Name(L"WindhawkCyberAmPmText");
            ampmText.FontSize(12.0);
            ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
            ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
            ampmText.Foreground(wuxm::SolidColorBrush(colPrimary));
            ampmBadge.Child(ampmText);
            bottomBar.Children().Append(ampmBadge);
            g_ampmTextBlock = ampmText;
        }

        hudContent.Children().Append(bottomBar);
    }

    chassis.Child(hudContent);
    hudFrame.Children().Append(chassis);

    // --- TACTICAL HUD CORNER ACCENTS (4 Tactical Chamfer Brackets) ---
    // 1. Top-Left L-bracket
    wuxc::Grid cornerTL;
    cornerTL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerTL.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Margin(wux::Thickness{ -2.0, -2.0, 0.0, 0.0 });
    wuxs::Rectangle tlH;
    tlH.Width(16.0); tlH.Height(3.0);
    tlH.Fill(wuxm::SolidColorBrush(colAccent1));
    tlH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle tlV;
    tlV.Width(3.0); tlV.Height(16.0);
    tlV.Fill(wuxm::SolidColorBrush(colAccent1));
    tlV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Children().Append(tlH);
    cornerTL.Children().Append(tlV);
    hudFrame.Children().Append(cornerTL);

    // 2. Top-Right Accent
    wuxc::Grid cornerTR;
    cornerTR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerTR.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Margin(wux::Thickness{ 0.0, -2.0, -2.0, 0.0 });
    wuxs::Rectangle trH;
    trH.Width(16.0); trH.Height(3.0);
    trH.Fill(wuxm::SolidColorBrush(colAccent2));
    trH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle trV;
    trV.Width(3.0); trV.Height(16.0);
    trV.Fill(wuxm::SolidColorBrush(colAccent2));
    trV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Children().Append(trH);
    cornerTR.Children().Append(trV);
    hudFrame.Children().Append(cornerTR);

    // 3. Bottom-Left Accent
    wuxc::Grid cornerBL;
    cornerBL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerBL.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Margin(wux::Thickness{ -2.0, 0.0, 0.0, -2.0 });
    wuxs::Rectangle blH;
    blH.Width(16.0); blH.Height(3.0);
    blH.Fill(wuxm::SolidColorBrush(colPrimary));
    blH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle blV;
    blV.Width(3.0); blV.Height(16.0);
    blV.Fill(wuxm::SolidColorBrush(colPrimary));
    blV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Children().Append(blH);
    cornerBL.Children().Append(blV);
    hudFrame.Children().Append(cornerBL);

    // 4. Bottom-Right Accent
    wuxc::Grid cornerBR;
    cornerBR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerBR.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Margin(wux::Thickness{ 0.0, 0.0, -2.0, -2.0 });
    wuxs::Rectangle brH;
    brH.Width(16.0); brH.Height(3.0);
    brH.Fill(wuxm::SolidColorBrush(colPrimary));
    brH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle brV;
    brV.Width(3.0); brV.Height(16.0);
    brV.Fill(wuxm::SolidColorBrush(colPrimary));
    brV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Children().Append(brH);
    cornerBR.Children().Append(brV);
    hudFrame.Children().Append(cornerBR);

    verticalLayout.Children().Append(hudFrame);
    rootContainer.Children().Append(verticalLayout);

    UpdateTimeAndDate();

    return rootContainer;
}

// Create a single LED dot shape (circle, square, or rounded square)
wuxs::Shape CreatePixelDot(double dotSize, const wuxm::Brush& fill) {
    if (g_settings.pixel.dotShape == L"square" || g_settings.pixel.dotShape == L"rounded") {
        wuxs::Rectangle rect;
        rect.Width(dotSize);
        rect.Height(dotSize);
        double rad = (g_settings.pixel.dotShape == L"rounded") ? (dotSize * 0.25) : 0.0;
        rect.RadiusX(rad);
        rect.RadiusY(rad);
        rect.Fill(fill);
        rect.HorizontalAlignment(wux::HorizontalAlignment::Center);
        rect.VerticalAlignment(wux::VerticalAlignment::Center);
        return rect;
    }
    wuxs::Ellipse dot;
    dot.Width(dotSize);
    dot.Height(dotSize);
    dot.Fill(fill);
    dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
    dot.VerticalAlignment(wux::VerticalAlignment::Center);
    return dot;
}

// Build Pixel Digit Grid Helper (7 cols x 11 rows, 77 dots)
wuxc::Grid CreatePixelDigitGrid(PixelDigitGrid& digitRef, double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 7; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    digitRef.dots.clear();
    digitRef.dots.reserve(77);

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 7; c++) {
            wuxs::Shape dot = CreatePixelDot(dotSize, g_brushInactiveDot);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
            digitRef.dots.push_back(dot);
        }
    }
    return grid;
}

// Build Pixel Colon Helper (2 cols x 11 rows, 4 rows x 2 dots active per colon point)
wuxc::Grid CreatePixelColon(double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 2; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 2; c++) {
            bool isColonDot = (r == 2 || r == 3 || r == 7 || r == 8);
            wuxs::Shape dot = CreatePixelDot(dotSize, isColonDot ? g_brushActiveDot : g_brushInactiveDot);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
        }
    }
    return grid;
}

// Build Retro Pixel / LED Dot Matrix Clock Element
wux::UIElement BuildPixelClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkPixelClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    // High-contrast brushes: 100% active LED color, subtle 6% ambient inactive dots
    wu::Color ledCol = g_settings.pixel.ledColor;
    g_brushActiveDot = wuxm::SolidColorBrush(ledCol);
    g_brushInactiveDot = wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, ledCol.R, ledCol.G, ledCol.B));

    // Dark frosted acrylic backplate for perfect contrast on any wallpaper
    wuxc::Border darkBackplate;
    darkBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.pixel.backplateOpacity, 10, 14, 22)));
    darkBackplate.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, 255, 255, 255)));
    darkBackplate.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    darkBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(20.0));
    darkBackplate.Padding(wux::Thickness{ 26.0, 24.0, 26.0, 24.0 });
    darkBackplate.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel matrixLayout;
    matrixLayout.Orientation(wuxc::Orientation::Horizontal);
    matrixLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    matrixLayout.VerticalAlignment(wux::VerticalAlignment::Center);

    // Scale dot size appropriately for 7x11 double-pixel thickness
    double dotSize = (double)g_settings.clockFontSize * 0.100;
    double spacing = dotSize * 0.35;
    double digitGap = dotSize * 1.3;
    double colonGap = dotSize * 1.0;

    // H1 & H2
    auto h1 = CreatePixelDigitGrid(g_pixelDigits[0], dotSize, spacing);
    auto h2 = CreatePixelDigitGrid(g_pixelDigits[1], dotSize, spacing);
    h1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    h2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(h1);
    matrixLayout.Children().Append(h2);

    // Colon 1 (between hours and minutes)
    auto colon1 = CreatePixelColon(dotSize, spacing);
    colon1.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(colon1);

    // M1 & M2
    auto m1 = CreatePixelDigitGrid(g_pixelDigits[2], dotSize, spacing);
    auto m2 = CreatePixelDigitGrid(g_pixelDigits[3], dotSize, spacing);
    m1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    matrixLayout.Children().Append(m1);
    matrixLayout.Children().Append(m2);

    // Optional Seconds (HH:MM:SS format at the same scale and alignment)
    if (g_settings.showSeconds) {
        m2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });

        // Colon 2 (between minutes and seconds)
        auto colon2 = CreatePixelColon(dotSize, spacing);
        colon2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
        matrixLayout.Children().Append(colon2);

        // S1 & S2
        auto s1 = CreatePixelDigitGrid(g_pixelDigits[4], dotSize, spacing);
        auto s2 = CreatePixelDigitGrid(g_pixelDigits[5], dotSize, spacing);
        s1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
        matrixLayout.Children().Append(s1);
        matrixLayout.Children().Append(s2);
    }

    verticalLayout.Children().Append(matrixLayout);

    // Date & 12-Hour AM/PM Indicator Row (keeps LED dot matrix centered and symmetrical)
    bool is12h = (g_settings.timeFormat == L"12h");
    if (g_settings.showDateCapsule || is12h) {
        wuxc::StackPanel bottomInfoRow;
        bottomInfoRow.Orientation(wuxc::Orientation::Horizontal);
        bottomInfoRow.HorizontalAlignment(wux::HorizontalAlignment::Center);
        bottomInfoRow.VerticalAlignment(wux::VerticalAlignment::Center);
        bottomInfoRow.Margin(wux::Thickness{ 0.0, 22.0, 0.0, 0.0 });

        if (g_settings.showDateCapsule) {
            wuxc::TextBlock dateText;
            dateText.Name(L"WindhawkPixelDateText");
            dateText.FontSize(19.0);
            dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
            dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
            dateText.VerticalAlignment(wux::VerticalAlignment::Center);

            bottomInfoRow.Children().Append(dateText);
            g_dateTextBlock = dateText;
        }

        if (is12h) {
            wuxc::Border ampmBadge;
            ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, ledCol.R, ledCol.G, ledCol.B)));
            ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(115, ledCol.R, ledCol.G, ledCol.B)));
            ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
            ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
            ampmBadge.Padding(wux::Thickness{ 8.0, 2.5, 8.0, 2.5 });
            ampmBadge.Margin(wux::Thickness{ g_settings.showDateCapsule ? 12.0 : 0.0, 0.0, 0.0, 0.0 });
            ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

            wuxc::TextBlock ampmText;
            ampmText.Name(L"WindhawkPixelAmPmText");
            ampmText.FontSize(13.0);
            ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
            ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
            ampmText.Foreground(wuxm::SolidColorBrush(ledCol));
            ampmBadge.Child(ampmText);
            bottomInfoRow.Children().Append(ampmBadge);
            g_ampmTextBlock = ampmText;
        }

        verticalLayout.Children().Append(bottomInfoRow);
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    if (g_settings.pixel.showBackplate) {
        darkBackplate.Child(verticalLayout);
        rootContainer.Children().Append(darkBackplate);
    } else {
        rootContainer.Children().Append(verticalLayout);
    }

    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Aura / Floating Clock Element
wux::UIElement BuildAuraClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkAuraClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Layered floating time container with soft ambient drop-shadow
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(wux::HorizontalAlignment::Center);

    double fontSize = (double)(g_settings.clockFontSize * 1.35);
    auto auraWeight = ResolveFontWeight(g_settings.minimalist.fontWeight, winrt::Windows::UI::Text::FontWeights::SemiBold());
    const wchar_t* auraFontFam = g_settings.minimalist.fontFamily.empty()
        ? L"Segoe UI Variable Display"
        : g_settings.minimalist.fontFamily.c_str();
    wu::Color auraTextColor = g_settings.minimalist.hasCustomTextColor
        ? g_settings.minimalist.textColor
        : wu::Colors::White();

    // 1. Ambient Drop-Shadow Aura Layer (Behind foreground text for contrast on any wallpaper)
    if (g_settings.minimalist.showShadow) {
        wuxc::TextBlock shadowText;
        shadowText.FontSize(fontSize);
        shadowText.FontFamily(wuxm::FontFamily(auraFontFam));
        shadowText.FontWeight(auraWeight);
        shadowText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(140, 0, 0, 0)));
        shadowText.HorizontalAlignment(wux::HorizontalAlignment::Center);
        shadowText.TextAlignment(wux::TextAlignment::Center);
        shadowText.Margin(wux::Thickness{ 0.0, 4.0, 0.0, 0.0 });
        wuxd::Typography::SetNumeralAlignment(shadowText, wux::FontNumeralAlignment::Tabular);
        timeLayerGrid.Children().Append(shadowText);
        g_auraShadowTextBlock = shadowText;
    }

    // 2. Foreground Crisp Floating Text Layer
    wuxc::TextBlock timeText;
    timeText.FontSize(fontSize);
    timeText.FontFamily(wuxm::FontFamily(auraFontFam));
    timeText.FontWeight(auraWeight);
    timeText.Foreground(wuxm::SolidColorBrush(auraTextColor));
    timeText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    timeText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_auraTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // Floating Aura Date Pill
    if (g_settings.showDateCapsule) {
        wuxc::Border auraDatePill;
        auraDatePill.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(38, 255, 255, 255)));
        auraDatePill.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(65, 255, 255, 255)));
        auraDatePill.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        auraDatePill.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(18.0));
        auraDatePill.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        auraDatePill.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 0.0 });
        auraDatePill.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkAuraDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        auraDatePill.Child(dateText);
        verticalLayout.Children().Append(auraDatePill);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Sonoma (Clean Typography) Clock Element
wux::UIElement BuildMacOSClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkMacOSClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    bool isBottomLeft = (g_settings.clockAlignment == L"bottomLeft");

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    const wchar_t* macosFontFam = g_settings.minimalist.fontFamily.empty()
        ? L"Segoe UI Variable Display"
        : g_settings.minimalist.fontFamily.c_str();
    auto macosTimeWeight = ResolveFontWeight(g_settings.minimalist.fontWeight, winrt::Windows::UI::Text::FontWeights::Light());
    wu::Color macosTextColor = g_settings.minimalist.hasCustomTextColor
        ? g_settings.minimalist.textColor
        : wu::Colors::White();
    wu::Color macosDateColor = g_settings.minimalist.hasCustomTextColor
        ? wu::ColorHelper::FromArgb(240, macosTextColor.R, macosTextColor.G, macosTextColor.B)
        : wu::ColorHelper::FromArgb(240, 255, 255, 255);

    // 1. macOS Date & 12-Hour AM/PM Header (Positioned elegantly ABOVE the time)
    bool is12h = (g_settings.timeFormat == L"12h");
    if (g_settings.showDateCapsule || is12h) {
        wuxc::Grid dateLayerGrid;
        dateLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateLayerGrid.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 6.0 });

        double dateFontSize = (double)g_settings.clockFontSize * 0.23;
        if (dateFontSize < 18.0) dateFontSize = 18.0;

        auto buildHeaderRow = [&](bool isShadow) {
            wuxc::StackPanel row;
            row.Orientation(wuxc::Orientation::Horizontal);
            row.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
            row.VerticalAlignment(wux::VerticalAlignment::Center);
            if (isShadow) {
                row.Margin(wux::Thickness{ 0.0, 1.5, 0.0, 0.0 });
            }

            if (g_settings.showDateCapsule) {
                wuxc::TextBlock dateText;
                if (!isShadow) {
                    dateText.Name(L"WindhawkMacOSDateText");
                }
                dateText.FontSize(dateFontSize);
                dateText.FontFamily(wuxm::FontFamily(macosFontFam));
                dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
                dateText.Foreground(wuxm::SolidColorBrush(
                    isShadow ? wu::ColorHelper::FromArgb(70, 0, 0, 0) : macosDateColor));
                dateText.VerticalAlignment(wux::VerticalAlignment::Center);
                row.Children().Append(dateText);

                if (isShadow) {
                    g_macosDateShadowBlock = dateText;
                } else {
                    g_macosDateTextBlock = dateText;
                }
            }

            if (g_settings.showDateCapsule && is12h) {
                wuxc::TextBlock sepText;
                sepText.Text(L"  •  ");
                sepText.FontSize(dateFontSize * 0.92);
                sepText.FontFamily(wuxm::FontFamily(macosFontFam));
                sepText.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
                sepText.Foreground(wuxm::SolidColorBrush(
                    isShadow
                        ? wu::ColorHelper::FromArgb(50, 0, 0, 0)
                        : wu::ColorHelper::FromArgb(150, macosTextColor.R, macosTextColor.G, macosTextColor.B)));
                sepText.VerticalAlignment(wux::VerticalAlignment::Center);
                row.Children().Append(sepText);
            }

            if (is12h) {
                wuxc::TextBlock ampmText;
                if (!isShadow) {
                    ampmText.Name(L"WindhawkMacOSAmPmText");
                }
                ampmText.FontSize(dateFontSize * 0.92);
                ampmText.FontFamily(wuxm::FontFamily(macosFontFam));
                ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
                ampmText.Foreground(wuxm::SolidColorBrush(
                    isShadow ? wu::ColorHelper::FromArgb(70, 0, 0, 0) : macosDateColor));
                ampmText.VerticalAlignment(wux::VerticalAlignment::Center);
                row.Children().Append(ampmText);

                if (isShadow) {
                    g_macosAmPmShadowBlock = ampmText;
                } else {
                    g_ampmTextBlock = ampmText;
                }
            }

            return row;
        };

        // Atmospheric Shadow Layer for Date / AM-PM Header
        if (g_settings.minimalist.showShadow) {
            dateLayerGrid.Children().Append(buildHeaderRow(true));
        }

        // Foreground Date / AM-PM Header
        dateLayerGrid.Children().Append(buildHeaderRow(false));

        verticalLayout.Children().Append(dateLayerGrid);
    }

    // 2. Giant macOS Typography Time Display (Thin/Light, San Francisco aesthetic via Segoe UI Variable Display Light)
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    double timeFontSize = (double)g_settings.clockFontSize * 1.55;

    // Atmospheric Soft Shadow Layer for Time
    if (g_settings.minimalist.showShadow) {
        wuxc::TextBlock timeShadow;
        timeShadow.FontSize(timeFontSize);
        timeShadow.FontFamily(wuxm::FontFamily(macosFontFam));
        timeShadow.FontWeight(macosTimeWeight);
        timeShadow.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 0, 0, 0)));
        timeShadow.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        timeShadow.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
        timeShadow.Margin(wux::Thickness{ 0.0, 3.5, 0.0, 0.0 });
        wuxd::Typography::SetNumeralAlignment(timeShadow, wux::FontNumeralAlignment::Tabular);
        timeLayerGrid.Children().Append(timeShadow);
        g_macosShadowTextBlock = timeShadow;
    }

    // Foreground Clean Time Text
    wuxc::TextBlock timeText;
    timeText.Name(L"WindhawkMacOSTimeText");
    timeText.FontSize(timeFontSize);
    timeText.FontFamily(wuxm::FontFamily(macosFontFam));
    timeText.FontWeight(macosTimeWeight);
    timeText.Foreground(wuxm::SolidColorBrush(macosTextColor));
    timeText.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
    timeText.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_macosTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // 3. macOS Sleek Battery Capsule (Underneath time if enabled)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(isBottomLeft);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// ---------------------------------------------------------------------------
// Vertical Stack Clock Builder (Bahnschrift, Segoe UI Dual Weight, Impact)
// ---------------------------------------------------------------------------

wux::UIElement BuildVerticalClockHelper(
    const wchar_t* containerName,
    const wchar_t* fontFamilyName,
    double fontScaleFactor,
    winrt::Windows::UI::Text::FontWeight hourWeight,
    winrt::Windows::UI::Text::FontWeight minWeight,
    wu::Color hourColor,
    wu::Color minColor,
    wu::Color shadowHourColor,
    wu::Color shadowMinColor,
    double shadowOffsetY,
    double spacingOverlapFactor,
    int charSpacing)
{
    wuxc::Grid rootContainer;
    rootContainer.Name(containerName);
    ApplyAlignmentAndMargin(rootContainer);

    bool isLeftAligned = (g_settings.clockAlignment == L"left" || g_settings.clockAlignment == L"bottomLeft");
    bool isRightAligned = (g_settings.clockAlignment == L"right");
    auto horizAlignment = isLeftAligned ? wux::HorizontalAlignment::Left
                        : (isRightAligned ? wux::HorizontalAlignment::Right : wux::HorizontalAlignment::Center);
    auto textAlignment = isLeftAligned ? wux::TextAlignment::Left
                       : (isRightAligned ? wux::TextAlignment::Right : wux::TextAlignment::Center);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(horizAlignment);

    // 1. Date Capsule (Positioned above vertical clock digits)
    if (g_settings.showDateCapsule) {
        wuxc::Border capsule;
        capsule.Name(L"WindhawkVerticalDateCapsule");
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 255, 255, 255)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(52, 255, 255, 255)));
        capsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        capsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(18.0));
        capsule.Padding(wux::Thickness{ 18.0, 5.0, 18.0, 5.0 });
        capsule.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 14.0 });
        capsule.HorizontalAlignment(horizAlignment);

        wuxc::StackPanel panel;
        panel.Orientation(wuxc::Orientation::Horizontal);
        panel.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkVerticalDateText");
        dateText.FontSize(16.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(250, 255, 255, 255)));
        panel.Children().Append(dateText);
        g_dateTextBlock = dateText;

        if (g_settings.timeFormat == L"12h") {
            wuxc::TextBlock sepText;
            sepText.FontSize(15.0);
            sepText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
            sepText.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
            sepText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(160, 255, 255, 255)));
            sepText.Text(L"  •  ");
            panel.Children().Append(sepText);

            wuxc::TextBlock ampmText;
            ampmText.Name(L"WindhawkVerticalAmPmText");
            ampmText.FontSize(15.0);
            ampmText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
            ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
            panel.Children().Append(ampmText);
            g_ampmTextBlock = ampmText;
        }

        capsule.Child(panel);
        verticalLayout.Children().Append(capsule);
    } else if (g_settings.timeFormat == L"12h") {
        // Dedicated AM/PM capsule if date capsule is disabled
        wuxc::Border ampmCapsule;
        ampmCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 255, 255, 255)));
        ampmCapsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(52, 255, 255, 255)));
        ampmCapsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(12.0));
        ampmCapsule.Padding(wux::Thickness{ 14.0, 4.0, 14.0, 4.0 });
        ampmCapsule.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 12.0 });
        ampmCapsule.HorizontalAlignment(horizAlignment);

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkVerticalAmPmText");
        ampmText.FontSize(14.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmCapsule.Child(ampmText);
        verticalLayout.Children().Append(ampmCapsule);
        g_ampmTextBlock = ampmText;
    }

    // 2. Vertical Clock Digits (Row 1: Hour, Row 2: Minute)
    double fontSize = (double)g_settings.clockFontSize * fontScaleFactor;
    const wchar_t* effectiveFont = g_settings.vertical.fontFamily.empty()
        ? fontFamilyName
        : g_settings.vertical.fontFamily.c_str();
    wu::Color effHourColor = g_settings.vertical.hasCustomTextColor
        ? g_settings.vertical.textColor
        : hourColor;
    wu::Color effMinColor = g_settings.vertical.hasCustomTextColor
        ? g_settings.vertical.textColor
        : minColor;
    if (g_settings.vertical.minuteOpacity > 0) {
        effMinColor.A = (uint8_t)((g_settings.vertical.minuteOpacity * 255 + 50) / 100);
    }

    // Hour Row Layer Grid
    wuxc::Grid hourGrid;
    hourGrid.HorizontalAlignment(horizAlignment);

    if (g_settings.vertical.showShadow) {
        wuxc::TextBlock hourShadow;
        hourShadow.FontSize(fontSize);
        hourShadow.FontFamily(wuxm::FontFamily(effectiveFont));
        hourShadow.FontWeight(hourWeight);
        hourShadow.Foreground(wuxm::SolidColorBrush(shadowHourColor));
        hourShadow.HorizontalAlignment(horizAlignment);
        hourShadow.TextAlignment(textAlignment);
        hourShadow.Margin(wux::Thickness{ 0.0, shadowOffsetY, 0.0, 0.0 });
        if (charSpacing != 0) hourShadow.CharacterSpacing(charSpacing);
        wuxd::Typography::SetNumeralAlignment(hourShadow, wux::FontNumeralAlignment::Tabular);
        hourGrid.Children().Append(hourShadow);
        g_verticalHourShadow = hourShadow;
    }

    wuxc::TextBlock hourText;
    hourText.Name(L"WindhawkVerticalHourText");
    hourText.FontSize(fontSize);
    hourText.FontFamily(wuxm::FontFamily(effectiveFont));
    hourText.FontWeight(hourWeight);
    hourText.Foreground(wuxm::SolidColorBrush(effHourColor));
    hourText.HorizontalAlignment(horizAlignment);
    hourText.TextAlignment(textAlignment);
    if (charSpacing != 0) hourText.CharacterSpacing(charSpacing);
    wuxd::Typography::SetNumeralAlignment(hourText, wux::FontNumeralAlignment::Tabular);
    hourGrid.Children().Append(hourText);
    g_verticalHourText = hourText;

    verticalLayout.Children().Append(hourGrid);

    // Minute Row Layer Grid (with overlap factor + optional user spacing offset)
    wuxc::Grid minGrid;
    minGrid.HorizontalAlignment(horizAlignment);
    double minMarginTop = (-fontSize * spacingOverlapFactor) + (double)g_settings.vertical.rowSpacing;
    minGrid.Margin(wux::Thickness{ 0.0, minMarginTop, 0.0, 0.0 });

    if (g_settings.vertical.showShadow) {
        wuxc::TextBlock minShadow;
        minShadow.FontSize(fontSize);
        minShadow.FontFamily(wuxm::FontFamily(effectiveFont));
        minShadow.FontWeight(minWeight);
        minShadow.Foreground(wuxm::SolidColorBrush(shadowMinColor));
        minShadow.HorizontalAlignment(horizAlignment);
        minShadow.TextAlignment(textAlignment);
        minShadow.Margin(wux::Thickness{ 0.0, shadowOffsetY, 0.0, 0.0 });
        if (charSpacing != 0) minShadow.CharacterSpacing(charSpacing);
        wuxd::Typography::SetNumeralAlignment(minShadow, wux::FontNumeralAlignment::Tabular);
        minGrid.Children().Append(minShadow);
        g_verticalMinShadow = minShadow;
    }

    wuxc::TextBlock minText;
    minText.Name(L"WindhawkVerticalMinText");
    minText.FontSize(fontSize);
    minText.FontFamily(wuxm::FontFamily(effectiveFont));
    minText.FontWeight(minWeight);
    minText.Foreground(wuxm::SolidColorBrush(effMinColor));
    minText.HorizontalAlignment(horizAlignment);
    minText.TextAlignment(textAlignment);
    if (charSpacing != 0) minText.CharacterSpacing(charSpacing);
    wuxd::Typography::SetNumeralAlignment(minText, wux::FontNumeralAlignment::Tabular);
    minGrid.Children().Append(minText);
    g_verticalMinText = minText;

    verticalLayout.Children().Append(minGrid);

    // 3. Battery Capsule (Below minutes if enabled)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(isLeftAligned);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// 1. Vertical Pixel / Nothing OS (Bahnschrift DIN Norm)
wux::UIElement BuildVerticalBahnschriftClockElement() {
    return BuildVerticalClockHelper(
        L"WindhawkVerticalBahnschriftContainer",
        L"Bahnschrift",
        1.25, // Scaled for prominent vertical 2-row presence
        winrt::Windows::UI::Text::FontWeights::SemiBold(),
        winrt::Windows::UI::Text::FontWeights::SemiBold(),
        wu::Colors::White(),
        wu::ColorHelper::FromArgb(192, 255, 255, 255), // Subtle contrast for minutes (75% opacity)
        wu::ColorHelper::FromArgb(100, 0, 0, 0),
        wu::ColorHelper::FromArgb(80, 0, 0, 0),
        4.0,
        0.18, // Compact vertical line spacing
        -25   // -2.5% em letter-spacing
    );
}

// 2. Vertical Modern / Dual Weight (Segoe UI Variable)
wux::UIElement BuildVerticalSegoeClockElement() {
    return BuildVerticalClockHelper(
        L"WindhawkVerticalSegoeContainer",
        L"Segoe UI Variable Display",
        1.25, // Scaled for prominent vertical 2-row presence
        winrt::Windows::UI::Text::FontWeights::SemiBold(),   // Hour bold
        winrt::Windows::UI::Text::FontWeights::ExtraLight(), // Minute light (high-contrast dual-weight matching preview font-weight: 200)
        wu::Colors::White(),
        wu::ColorHelper::FromArgb(215, 255, 255, 255),
        wu::ColorHelper::FromArgb(90, 0, 0, 0),
        wu::ColorHelper::FromArgb(70, 0, 0, 0),
        4.0,
        0.16,
        -30
    );
}

// 3. Vertical Poster / Heavy Impact (Impact)
wux::UIElement BuildVerticalImpactClockElement() {
    return BuildVerticalClockHelper(
        L"WindhawkVerticalImpactContainer",
        L"Impact",
        1.30, // Proportional scaling for condensed poster glyphs
        winrt::Windows::UI::Text::FontWeights::Normal(),
        winrt::Windows::UI::Text::FontWeights::Normal(),
        wu::Colors::White(),
        wu::Colors::White(),
        wu::ColorHelper::FromArgb(130, 0, 0, 0),
        wu::ColorHelper::FromArgb(130, 0, 0, 0),
        5.0,
        0.22, // Tight line spacing for poster aesthetic
        10    // Slight positive tracking
    );
}

// Build ANSI Shadow (Terminal ASCII) Clock Element
wux::UIElement BuildAnsiShadowClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkAnsiShadowContainer");
    ApplyAlignmentAndMargin(rootContainer);

    // Resolve terminal color palette
    wu::Color colPrimary = wu::ColorHelper::FromArgb(255, 251, 191, 36);   // Amber CRT (Default)
    wu::Color colAccent  = wu::ColorHelper::FromArgb(255, 253, 230, 138);
    if (g_settings.ansi.colorPreset == L"green") {
        colPrimary = wu::ColorHelper::FromArgb(255, 0, 255, 0);            // Matrix Green (Classic Windows CMD)
        colAccent  = wu::ColorHelper::FromArgb(255, 128, 255, 128);
    } else if (g_settings.ansi.colorPreset == L"cyan") {
        colPrimary = wu::ColorHelper::FromArgb(255, 34, 211, 238);         // Cyber Cyan
        colAccent  = wu::ColorHelper::FromArgb(255, 165, 243, 252);
    } else if (g_settings.ansi.colorPreset == L"white") {
        colPrimary = wu::ColorHelper::FromArgb(255, 248, 250, 252);        // Classic Monochrome
        colAccent  = wu::ColorHelper::FromArgb(255, 203, 213, 225);
    }

    auto withAlpha = [](wu::Color c, uint8_t a) {
        return wu::ColorHelper::FromArgb(a, c.R, c.G, c.B);
    };

    wuxc::StackPanel outerLayout;
    outerLayout.Orientation(wuxc::Orientation::Vertical);
    outerLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardContent;
    cardContent.Orientation(wuxc::Orientation::Vertical);
    cardContent.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxm::FontFamily monoFont(L"Cascadia Mono, Consolas");
    double asciiFontSize = std::max(8.0, (double)g_settings.clockFontSize * 0.175);
    double scale = std::clamp(asciiFontSize / 28.875, 0.7, 1.8);

    // Two-tone phosphor brushes for 3D ANSI Shadow depth (solid blocks vs box-drawing strokes)
    g_ansiPrimaryBrush = wuxm::SolidColorBrush(colPrimary);
    g_ansiShadowBrush = wuxm::SolidColorBrush(withAlpha(colPrimary, 145));

    // Top terminal prompt bar (plus right-side AM/PM & :SS indicators)
    bool hasRightIndicators = (g_settings.timeFormat == L"12h") || g_settings.showSeconds;
    if (g_settings.ansi.showPromptBar || hasRightIndicators) {
        wuxc::Grid headerBar;
        headerBar.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        headerBar.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 11.0 * scale });

        if (g_settings.ansi.showPromptBar) {
            wuxc::ColumnDefinition colLeft;
            colLeft.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
            wuxc::ColumnDefinition colRight;
            colRight.Width(wux::GridLengthHelper::Auto());
            headerBar.ColumnDefinitions().Append(colLeft);
            headerBar.ColumnDefinitions().Append(colRight);

            wuxc::StackPanel promptRow;
            promptRow.Orientation(wuxc::Orientation::Horizontal);
            promptRow.HorizontalAlignment(wux::HorizontalAlignment::Left);
            promptRow.VerticalAlignment(wux::VerticalAlignment::Center);
            promptRow.Margin(wux::Thickness{ 0.0, 0.0, hasRightIndicators ? (20.0 * scale) : 0.0, 0.0 });

            wuxc::TextBlock promptText;
            promptText.Name(L"WindhawkAnsiPromptText");
            promptText.Text(L"PS C:\\Windows\\LockApp> clock --ansi ");
            promptText.FontSize(13.0 * scale);
            promptText.FontFamily(monoFont);
            promptText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(145, 255, 255, 255)));
            promptText.VerticalAlignment(wux::VerticalAlignment::Center);
            promptRow.Children().Append(promptText);

            wuxc::TextBlock cursorText;
            cursorText.Name(L"WindhawkAnsiCursorText");
            cursorText.Text(L"█");
            cursorText.FontSize(12.5 * scale);
            cursorText.FontFamily(monoFont);
            cursorText.Foreground(wuxm::SolidColorBrush(colPrimary));
            cursorText.VerticalAlignment(wux::VerticalAlignment::Center);
            promptRow.Children().Append(cursorText);
            g_ansiCursorTextBlock = cursorText;

            wuxc::Grid::SetColumn(promptRow, 0);
            headerBar.Children().Append(promptRow);
        }

        if (hasRightIndicators) {
            wuxc::StackPanel rightStatus;
            rightStatus.Orientation(wuxc::Orientation::Horizontal);
            rightStatus.HorizontalAlignment(wux::HorizontalAlignment::Right);
            rightStatus.VerticalAlignment(wux::VerticalAlignment::Center);

            if (g_settings.timeFormat == L"12h") {
                wuxc::Border ampmPill;
                ampmPill.Background(wuxm::SolidColorBrush(withAlpha(colPrimary, 36)));
                ampmPill.BorderBrush(wuxm::SolidColorBrush(withAlpha(colPrimary, 110)));
                ampmPill.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
                ampmPill.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
                ampmPill.Padding(wux::Thickness{ 6.0 * scale, 1.0 * scale, 6.0 * scale, 1.0 * scale });
                ampmPill.Margin(wux::Thickness{ 0.0, 0.0, g_settings.showSeconds ? (8.0 * scale) : 0.0, 0.0 });
                ampmPill.VerticalAlignment(wux::VerticalAlignment::Center);

                wuxc::TextBlock ampmText;
                ampmText.Name(L"WindhawkAnsiAmPmText");
                ampmText.FontSize(11.5 * scale);
                ampmText.FontFamily(monoFont);
                ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
                ampmText.Foreground(wuxm::SolidColorBrush(colAccent));
                ampmPill.Child(ampmText);
                rightStatus.Children().Append(ampmPill);
                g_ampmTextBlock = ampmText;
            }

            if (g_settings.showSeconds) {
                wuxc::TextBlock secText;
                secText.Name(L"WindhawkAnsiSecText");
                secText.FontSize(13.5 * scale);
                secText.FontFamily(monoFont);
                secText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
                secText.Foreground(wuxm::SolidColorBrush(colAccent));
                secText.VerticalAlignment(wux::VerticalAlignment::Center);
                rightStatus.Children().Append(secText);
                g_secTextBlock = secText;
            }

            if (g_settings.ansi.showPromptBar) {
                wuxc::Grid::SetColumn(rightStatus, 1);
            }
            headerBar.Children().Append(rightStatus);
        }

        cardContent.Children().Append(headerBar);
    }

    bool hasCard = g_settings.ansi.cardOpacity > 0;

    // Center 6-line ANSI Shadow ASCII banner with optional Composition phosphor glow
    // Native Composition Visual.Scale(1.0, 1.10, 1.0) avoids XAML RenderTransform surface
    // clipping of Composition DropShadow while preserving gapless box-drawing continuity.
    wuxc::Grid artGrid;
    artGrid.HorizontalAlignment(wux::HorizontalAlignment::Center);
    artGrid.Margin(wux::Thickness{ 0.0, 0.0, 0.0, asciiFontSize * 0.60 });
    try {
        auto artVisual = wuxh::ElementCompositionPreview::GetElementVisual(artGrid);
        artVisual.Scale(winrt::Windows::Foundation::Numerics::float3{ 1.0f, 1.10f, 1.0f });
    } catch (...) {
        Wh_Log(L"Failed to apply Composition vertical scale for ANSI Shadow");
    }

    wuxc::Grid shadowHost;
    shadowHost.Name(L"WindhawkAnsiGlowHost");
    shadowHost.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    shadowHost.VerticalAlignment(wux::VerticalAlignment::Stretch);
    shadowHost.IsHitTestVisible(false);
    if (g_settings.ansi.showGlow) {
        artGrid.Children().Append(shadowHost);
        g_ansiShadowHost = shadowHost;
    }

    // Contrast shadow whenever floating without card (preserves legibility on bright wallpapers even with glow)
    if (!hasCard) {
        wuxc::TextBlock shadowArt;
        shadowArt.Name(L"WindhawkAnsiShadowGlowText");
        shadowArt.FontSize(asciiFontSize);
        shadowArt.FontFamily(monoFont);
        shadowArt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        shadowArt.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(g_settings.ansi.showGlow ? 120 : 155, 0, 0, 0)));
        shadowArt.LineStackingStrategy(wux::LineStackingStrategy::BlockLineHeight);
        shadowArt.LineHeight(asciiFontSize * 1.0);
        shadowArt.TextAlignment(wux::TextAlignment::Left);
        shadowArt.HorizontalAlignment(wux::HorizontalAlignment::Center);
        shadowArt.Margin(wux::Thickness{ 0.0, 2.0, 0.0, 0.0 });
        artGrid.Children().Append(shadowArt);
        g_ansiShadowTextBlock = shadowArt;
    }

    wuxc::TextBlock mainArt;
    mainArt.Name(L"WindhawkAnsiMainText");
    mainArt.FontSize(asciiFontSize);
    mainArt.FontFamily(monoFont);
    mainArt.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
    mainArt.Foreground(g_ansiPrimaryBrush);
    mainArt.LineStackingStrategy(wux::LineStackingStrategy::BlockLineHeight);
    mainArt.LineHeight(asciiFontSize * 1.0);
    mainArt.TextAlignment(wux::TextAlignment::Left);
    mainArt.HorizontalAlignment(wux::HorizontalAlignment::Center);
    artGrid.Children().Append(mainArt);
    g_ansiArtTextBlock = mainArt;

    if (g_settings.ansi.showGlow) {
        try {
            auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(shadowHost);
            auto compositor = hostVisual.Compositor();

            auto dropShadow = compositor.CreateDropShadow();
            dropShadow.Color(colPrimary);
            dropShadow.BlurRadius(16.0f);
            dropShadow.Opacity(0.75f);
            dropShadow.Offset(winrt::Windows::Foundation::Numerics::float3{ 0.0f, 0.0f, 0.0f });
            dropShadow.Mask(mainArt.GetAlphaMask());

            auto shadowVisual = compositor.CreateSpriteVisual();
            shadowVisual.RelativeSizeAdjustment(winrt::Windows::Foundation::Numerics::float2{ 1.0f, 1.0f });
            shadowVisual.Shadow(dropShadow);
            wuxh::ElementCompositionPreview::SetElementChildVisual(shadowHost, shadowVisual);
        } catch (...) {
            Wh_Log(L"Failed to attach Composition phosphor glow for ANSI Shadow");
        }
    }

    cardContent.Children().Append(artGrid);

    // Date display (inside terminal footer when card is active, or centered below)
    if (g_settings.showDateCapsule) {
        if (hasCard) {
            wuxs::Rectangle divider;
            divider.Height(1.0);
            divider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
            divider.Fill(wuxm::SolidColorBrush(withAlpha(colPrimary, (uint8_t)std::min(45, g_settings.ansi.cardOpacity))));
            divider.Margin(wux::Thickness{ 0.0, 15.4 * scale, 0.0, 11.0 * scale });
            cardContent.Children().Append(divider);
        }

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkAnsiDateText");
        dateText.FontSize(14.5 * scale);
        dateText.FontFamily(monoFont);
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(withAlpha(colAccent, 220)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);
        if (!hasCard) {
            dateText.Margin(wux::Thickness{ 0.0, 17.6 * scale, 0.0, 0.0 });
        }
        cardContent.Children().Append(dateText);
        g_dateTextBlock = dateText;
    }

    if (hasCard) {
        // Reserve full 4-digit clock width so narrow digits like '1' never shrink or jitter the card
        cardContent.MinWidth(asciiFontSize * 23.5);

        wuxc::Border terminalCard;
        terminalCard.Name(L"WindhawkAnsiTerminalCard");
        terminalCard.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.ansi.cardOpacity, 8, 12, 20)));
        terminalCard.BorderBrush(wuxm::SolidColorBrush(withAlpha(colPrimary, (uint8_t)std::min(65, g_settings.ansi.cardOpacity))));
        terminalCard.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        terminalCard.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(12.0));
        terminalCard.Padding(wux::Thickness{ 26.0 * scale, 19.8 * scale, 26.0 * scale, 19.8 * scale });
        terminalCard.HorizontalAlignment(wux::HorizontalAlignment::Center);
        terminalCard.Child(cardContent);
        outerLayout.Children().Append(terminalCard);
    } else {
        outerLayout.Children().Append(cardContent);
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false, true);
        if (batteryCapsule) {
            outerLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(outerLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Factory function for selected clock style
wux::UIElement BuildClockElement() {
    if (g_settings.clockStyle == L"macos") {
        return BuildMacOSClockElement();
    } else if (g_settings.clockStyle == L"pixel") {
        return BuildPixelClockElement();
    } else if (g_settings.clockStyle == L"flip") {
        return BuildFlipClockElement();
    } else if (g_settings.clockStyle == L"vintage") {
        return BuildVintageClockElement();
    } else if (g_settings.clockStyle == L"cyberpunk") {
        return BuildCyberpunkClockElement();
    } else if (g_settings.clockStyle == L"ansiShadow") {
        return BuildAnsiShadowClockElement();
    } else if (g_settings.clockStyle == L"aura") {
        return BuildAuraClockElement();
    } else if (g_settings.clockStyle == L"verticalBahnschrift") {
        return BuildVerticalBahnschriftClockElement();
    } else if (g_settings.clockStyle == L"verticalSegoe") {
        return BuildVerticalSegoeClockElement();
    } else if (g_settings.clockStyle == L"verticalImpact") {
        return BuildVerticalImpactClockElement();
    }
    return BuildPixelClockElement();
}

// ---------------------------------------------------------------------------
// UI thread discovery and synchronous dispatch
// ---------------------------------------------------------------------------

std::atomic<HWND> g_coreWindow{ nullptr };
std::atomic<HHOOK> g_getMessageHook{ nullptr };
std::atomic<bool> g_settingsDirty{ false };
HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;

// ---------------------------------------------------------------------------
// Power management and display state tracking
// ---------------------------------------------------------------------------

#ifndef DEVICE_NOTIFY_CALLBACK
#define DEVICE_NOTIFY_CALLBACK 2
#endif

#ifndef PBT_APMSUSPEND
#define PBT_APMSUSPEND 0x0004
#endif

#ifndef PBT_APMRESUMEAUTOMATIC
#define PBT_APMRESUMEAUTOMATIC 0x0012
#endif

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif

#ifndef PBT_POWERSETTINGCHANGE
#define PBT_POWERSETTINGCHANGE 0x8013
#endif

typedef ULONG(CALLBACK* DEVICE_NOTIFY_CALLBACK_ROUTINE)(PVOID Context,
                                                        ULONG Type,
                                                        PVOID Setting);

typedef struct _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS {
    DEVICE_NOTIFY_CALLBACK_ROUTINE Callback;
    PVOID Context;
} DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS, *PDEVICE_NOTIFY_SUBSCRIBE_PARAMETERS;

// GUID_CONSOLE_DISPLAY_STATE {6FE69556-704A-47A0-8F24-C28D936FDA47}
static constexpr GUID GUID_CONSOLE_DISPLAY_STATE_LOCAL = {
    0x6fe69556,
    0x704a,
    0x47a0,
    {0x8f, 0x24, 0xc2, 0x8d, 0x93, 0x6f, 0xda, 0x47}};

std::atomic<bool> g_isDisplayOn{true};
std::atomic<bool> g_isSystemAwake{true};
std::atomic<ULONGLONG> g_resumeTimeMs{0};
HPOWERNOTIFY g_hDisplayNotify = nullptr;
HPOWERNOTIFY g_hSuspendResumeNotify = nullptr;

ULONG CALLBACK SuspendResumeCallback(PVOID /*context*/,
                                     ULONG type,
                                     PVOID /*setting*/) {
    if (type == PBT_APMSUSPEND) {
        Wh_Log(L"Power notification: System suspending (PBT_APMSUSPEND).");
        g_isSystemAwake.store(false);
    } else if (type == PBT_APMRESUMEAUTOMATIC || type == PBT_APMRESUMESUSPEND) {
        Wh_Log(L"Power notification: System resumed (type=0x%x).", type);
        g_isSystemAwake.store(true);
        g_resumeTimeMs.store(GetTickCount64());
        g_settingsDirty.store(true);
    }
    return 0;
}

ULONG CALLBACK DisplaySettingCallback(PVOID /*context*/,
                                      ULONG type,
                                      PVOID setting) {
    if (type == PBT_POWERSETTINGCHANGE && setting) {
        auto* pbs = reinterpret_cast<const POWERBROADCAST_SETTING*>(setting);
        if (pbs->DataLength >= sizeof(DWORD)) {
            DWORD state = *reinterpret_cast<const DWORD*>(pbs->Data);
            if (state == 0) {
                Wh_Log(L"Display notification: Console display is OFF.");
                g_isDisplayOn.store(false);
            } else {
                Wh_Log(L"Display notification: Console display is ON (state=%u).",
                       state);
                g_isDisplayOn.store(true);
                g_resumeTimeMs.store(GetTickCount64());
                g_settingsDirty.store(true);
            }
        }
    }
    return 0;
}

UINT g_runOnUiThreadMessage = 0;

using RunOnUiThreadProc = void(WINAPI*)(PVOID);

struct RunOnUiThreadParam {
    RunOnUiThreadProc proc;
    PVOID param;
    bool ran;
};

// WH_CALLWNDPROC hook used by RunOnUiThread(). It runs on the target UI thread
// before the window procedure sees the message.
LRESULT CALLBACK RunOnUiThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == g_runOnUiThreadMessage) {
            auto* p = reinterpret_cast<RunOnUiThreadParam*>(cwp->lParam);
            if (p && p->proc) {
                try {
                    p->proc(p->param);
                    p->ran = true;
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"UI task failed: %s", e.message().c_str());
                } catch (...) {
                    Wh_Log(L"UI task failed with an unknown error.");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Runs proc on the thread that owns hWnd and returns only after it has
// finished. Mixing this with queued dispatcher callbacks is not allowed: the
// synchronous path is the only way to guarantee that no mod code is left
// behind when the DLL is unloaded.
bool RunOnUiThread(HWND hWnd, RunOnUiThreadProc proc, PVOID param) {
    if (!hWnd || !proc) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    static std::mutex s_runMutex;
    std::lock_guard<std::mutex> lock(s_runMutex);

    if (!g_runOnUiThreadMessage) {
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnUiThreadHookProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_CALLWNDPROC) failed: %u", GetLastError());
        return false;
    }

    RunOnUiThreadParam p{ proc, param, false };
    SendMessageW(hWnd, g_runOnUiThreadMessage, 0, reinterpret_cast<LPARAM>(&p));
    UnhookWindowsHookEx(hook);
    return p.ran;
}

// ---------------------------------------------------------------------------
// Visual tree helpers
// ---------------------------------------------------------------------------

bool IsClockAttached() {
    if (!g_activeClockElement) {
        return false;
    }
    try {
        return wuxm::VisualTreeHelper::GetParent(g_activeClockElement) != nullptr;
    } catch (...) {
        return false;
    }
}

// Restores the local Visibility value that existed before this mod hid an
// element. ClearValue() is used when the element had no local value, which
// keeps Windows' own bindings and visual states intact.
void RestoreTrackedElements(std::vector<TrackedElement>& tracked) {
    auto prop = wux::UIElement::VisibilityProperty();
    for (auto& trackedElement : tracked) {
        if (!trackedElement.element) {
            continue;
        }
        try {
            if (trackedElement.hadLocalValue) {
                trackedElement.element.SetValue(prop, trackedElement.originalValue);
            } else {
                trackedElement.element.ClearValue(prop);
            }
        } catch (...) {
            // If restoring the saved value fails, at least remove our local
            // value so Windows can re-evaluate its own bindings and states.
            try {
                trackedElement.element.ClearValue(prop);
            } catch (...) {}
        }
    }
    tracked.clear();
    tracked.shrink_to_fit();
}

// Keeps the tracking list in sync with the current tree and hides only the
// elements that are actually supposed to be hidden.
void SyncTrackedElements(std::vector<TrackedElement>& tracked,
                         std::vector<wux::FrameworkElement> const& found,
                         bool shouldHide) {
    tracked.erase(
        std::remove_if(tracked.begin(), tracked.end(),
                       [&found](TrackedElement const& trackedElement) {
                           if (!trackedElement.element) {
                               return true;
                           }
                           return std::none_of(
                               found.begin(), found.end(),
                               [&trackedElement](wux::FrameworkElement const& element) {
                                   return element == trackedElement.element;
                               });
                       }),
        tracked.end());

    if (!shouldHide) {
        RestoreTrackedElements(tracked);
        return;
    }

    auto prop = wux::UIElement::VisibilityProperty();
    for (auto const& element : found) {
        if (!element) {
            continue;
        }

        bool alreadyTracked = std::any_of(
            tracked.begin(), tracked.end(),
            [&element](TrackedElement const& trackedElement) {
                return trackedElement.element == element;
            });
        if (alreadyTracked) {
            continue;
        }

        TrackedElement trackedElement;
        trackedElement.element = element;
        try {
            auto localValue = element.ReadLocalValue(prop);
            trackedElement.hadLocalValue =
                !(localValue == wux::DependencyProperty::UnsetValue());
            if (trackedElement.hadLocalValue) {
                trackedElement.originalValue = localValue;
            }
        } catch (...) {
            trackedElement.hadLocalValue = false;
        }

        tracked.push_back(trackedElement);
        try {
            element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }

    // Re-assert the value on every scan. Local values normally win over visual
    // states, but re-applying is cheap and keeps the element hidden if Windows
    // replaces the tree or resets the property.
    for (auto& trackedElement : tracked) {
        try {
            trackedElement.element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }
}

// ---------------------------------------------------------------------------
// Clock element management
// ---------------------------------------------------------------------------

void RemoveActiveClockElement() {
    if (!g_activeClockElement) {
        return;
    }
    if (g_lockRootGrid) {
        uint32_t index = 0;
        try {
            if (g_lockRootGrid.Children().IndexOf(g_activeClockElement, index)) {
                g_lockRootGrid.Children().RemoveAt(index);
            }
        } catch (...) {}
    }
    g_activeClockElement = nullptr;
}

// Releases every strong XAML reference. Must run on the lock screen UI thread.
void ResetClockElementReferences() {
    g_hourTextBlock = nullptr;
    g_hourBotTextBlock = nullptr;
    g_minTextBlock = nullptr;
    g_minBotTextBlock = nullptr;
    g_secTextBlock = nullptr;
    g_ampmTextBlock = nullptr;
    g_auraTimeTextBlock = nullptr;
    g_auraShadowTextBlock = nullptr;
    g_macosTimeTextBlock = nullptr;
    g_macosShadowTextBlock = nullptr;
    g_macosDateTextBlock = nullptr;
    g_macosDateShadowBlock = nullptr;
    g_macosAmPmShadowBlock = nullptr;
    g_verticalHourText = nullptr;
    g_verticalHourShadow = nullptr;
    g_verticalMinText = nullptr;
    g_verticalMinShadow = nullptr;
    g_batteryCapsule = nullptr;
    g_batteryIconText = nullptr;
    g_batteryStatusText = nullptr;
    g_cyberSlash1 = nullptr;
    g_cyberSlash2 = nullptr;
    g_cyberLiveLabel = nullptr;
    if (g_ansiShadowHost) {
        try {
            wuxh::ElementCompositionPreview::SetElementChildVisual(g_ansiShadowHost, nullptr);
        } catch (...) {}
        g_ansiShadowHost = nullptr;
    }
    g_ansiArtTextBlock = nullptr;
    g_ansiShadowTextBlock = nullptr;
    g_ansiCursorTextBlock = nullptr;
    g_ansiPrimaryBrush = nullptr;
    g_ansiShadowBrush = nullptr;
    g_dateTextBlock = nullptr;

    for (int i = 0; i < 6; i++) {
        g_pixelDigits[i].dots.clear();
        g_pixelDigits[i].dots.shrink_to_fit();
        g_pixelDigits[i].lastVal = -1;
    }
    g_brushActiveDot = nullptr;
    g_brushInactiveDot = nullptr;
}

void RestoreDefaultElements() {
    RestoreTrackedElements(g_hiddenClockElements);
    RestoreTrackedElements(g_hiddenWidgetElements);
}

struct ScanResult {
    wuxc::Grid root{ nullptr };
    bool hasLockRoot = false;
    bool hasMainPage = false;
    std::vector<wux::FrameworkElement> clockElements;
    std::vector<wux::FrameworkElement> widgetElements;
};

void ScanNode(wux::DependencyObject const& node, ScanResult& result, int depth) {
    if (!node || depth > 96) {
        return;
    }

    // Our own clock contains hundreds of Ellipses in the pixel style. There is
    // nothing to find inside it, so skip the whole subtree.
    if (g_activeClockElement && node == g_activeClockElement) {
        return;
    }

    if (auto element = node.try_as<wux::FrameworkElement>()) {
        auto name = element.Name();

        if (name == L"LockRootGrid") {
            if (!result.hasLockRoot) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    result.root = grid;
                    result.hasLockRoot = true;
                }
            }
        } else if (name == L"MainPageContainer") {
            if (!result.hasMainPage) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    if (!result.hasLockRoot) {
                        result.root = grid;
                    }
                    result.hasMainPage = true;
                }
            }
        } else if (name == L"TimeAndDatePanel" || name == L"Time" || name == L"Date") {
            result.clockElements.push_back(element);
        } else if (name == L"AdaptiveCardDetailContainer" ||
                   name == L"DetailTextPanel" || name == L"LockWidgetCanvas") {
            result.widgetElements.push_back(element);
        }
    }

    int childCount = 0;
    try {
        childCount = wuxm::VisualTreeHelper::GetChildrenCount(node);
    } catch (...) {
        return;
    }
    for (int i = 0; i < childCount; i++) {
        try {
            ScanNode(wuxm::VisualTreeHelper::GetChild(node, i), result, depth + 1);
        } catch (...) {}
    }
}

// Finds the lock screen root and the stock clock/widget elements, then injects
// or refreshes the selected clock. Must run on the lock screen UI thread.
void AttachToTree() {
    if (!g_isSystemAwake.load() || !g_isDisplayOn.load()) {
        return;
    }

    wux::Window currentWindow{nullptr};
    try {
        currentWindow = wux::Window::Current();
        if (!currentWindow || !currentWindow.Visible()) {
            return;
        }
    } catch (...) {
        return;
    }

    wux::UIElement content{nullptr};
    try {
        content = currentWindow.Content();
    } catch (...) {
        return;
    }
    if (!content) {
        return;
    }

    ScanResult scan;
    ScanNode(content, scan, 0);
    if (!scan.root) {
        // The tree is still being built; the next tick will retry.
        return;
    }

    bool sameRoot = (g_lockRootGrid == scan.root);
    if (!sameRoot) {
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_lockRootGrid = scan.root;
        g_timeCache.Invalidate();
        Wh_Log(L"Attached to the lock screen visual tree.");
    }

    bool hideClock = (g_settings.clockStyle != L"defaultOnly");
    SyncTrackedElements(g_hiddenClockElements, scan.clockElements, hideClock);
    SyncTrackedElements(g_hiddenWidgetElements, scan.widgetElements,
                        g_settings.hideDefaultWidgets);

    if (g_settings.clockStyle == L"defaultOnly") {
        RemoveActiveClockElement();
        g_timeCache.Invalidate();
        return;
    }

    if (!g_activeClockElement || !IsClockAttached()) {
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_timeCache.Invalidate();

        try {
            g_activeClockElement = BuildClockElement();
            if (g_activeClockElement) {
                g_lockRootGrid.Children().Append(g_activeClockElement);
                Wh_Log(L"Clock injected: style=%s, seconds=%d",
                       g_settings.clockStyle.c_str(), g_settings.showSeconds);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Failed to inject clock: %s", e.message().c_str());
            g_activeClockElement = nullptr;
        } catch (...) {
            Wh_Log(L"Failed to inject clock: unknown error.");
            g_activeClockElement = nullptr;
        }
    }

    UpdateTimeAndDate();
}

// Reloads settings and rebuilds the clock on the UI thread.
void WINAPI RebuildOnUiThread(PVOID) {
    g_settingsDirty = false;
    LoadSettings();
    RestoreDefaultElements();
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_timeCache.Invalidate();
    g_lockRootGrid = nullptr;
    AttachToTree();
    Wh_Log(L"Settings applied on the lock screen UI thread.");
}

// ---------------------------------------------------------------------------
// Periodic UI work
// ---------------------------------------------------------------------------

ULONGLONG g_lastUiTick = 0;
ULONGLONG g_lastFullScan = 0;

void OnUiThreadTick() {
    if (!g_isSystemAwake.load() || !g_isDisplayOn.load()) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    int intervalMs = g_settings.showSeconds ? 500 : 1000;
    if ((now - g_lastUiTick) < static_cast<ULONGLONG>(intervalMs)) {
        return;
    }

    HWND coreWnd = g_coreWindow.load();
    if (!coreWnd || !IsWindow(coreWnd) || !IsWindowVisible(coreWnd)) {
        return;
    }

    try {
        auto currentWindow = wux::Window::Current();
        if (!currentWindow || !currentWindow.Visible()) {
            return;
        }
    } catch (...) {
        return;
    }

    g_lastUiTick = now;

    if (g_settingsDirty.exchange(false)) {
        try {
            RebuildOnUiThread(nullptr);
        } catch (...) {
            // Keep the request alive so the next tick retries the rebuild.
            g_settingsDirty = true;
            throw;
        }
        return;
    }

    bool hasClockStyle = (g_settings.clockStyle != L"defaultOnly");
    bool needScan = (!g_lockRootGrid || (hasClockStyle && !g_activeClockElement) ||
                     (now - g_lastFullScan) >= 3000);
    if (needScan) {
        g_lastFullScan = now;
        AttachToTree();
    } else if (hasClockStyle) {
        UpdateTimeAndDate();
    }
}

LRESULT CALLBACK GetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        try {
            OnUiThreadTick();
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"UI tick failed: %s", e.message().c_str());
        } catch (...) {
            Wh_Log(L"UI tick failed with an unknown error.");
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Teardown and worker thread
// ---------------------------------------------------------------------------

void WINAPI TeardownOnUiThread(PVOID) {
    // Unhook from the UI thread: no WH_GETMESSAGE callback can be executing at
    // the same time on this thread, so it is safe for the DLL to unload next.
    HHOOK hook = g_getMessageHook.exchange(nullptr);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }

    RestoreDefaultElements();
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_lockRootGrid = nullptr;
    g_timeCache.Invalidate();
    Wh_Log(L"UI teardown complete.");
}

BOOL CALLBACK FindCoreWindowProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    WCHAR className[128];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0 &&
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindCoreWindow() {
    HWND found = nullptr;
    EnumWindows(FindCoreWindowProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Wait for LockApp's CoreWindow, then install the message hook on its
    // thread. Retry until the window exists or the mod is unloaded.
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        if (g_getMessageHook.load()) {
            break;
        }

        if (!g_isSystemAwake.load() || !g_isDisplayOn.load()) {
            continue;
        }

        HWND window = FindCoreWindow();
        if (!window || !IsWindowVisible(window)) {
            continue;
        }

        DWORD threadId = GetWindowThreadProcessId(window, nullptr);
        HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageHookProc, nullptr, threadId);
        if (hook) {
            g_coreWindow = window;
            g_getMessageHook = hook;
            Wh_Log(L"Installed WH_GETMESSAGE hook on UI thread %u (hwnd=%p).",
                   threadId, window);
            break;
        }
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
    }

    // Wake the UI thread periodically. The hook itself throttles updates to
    // 500 ms when seconds are shown and to 1000 ms otherwise.
    while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {
        if (!g_isSystemAwake.load() || !g_isDisplayOn.load()) {
            continue;
        }

        ULONGLONG resumeTime = g_resumeTimeMs.load();
        if (resumeTime != 0 && (GetTickCount64() - resumeTime) < 1000) {
            continue;
        }

        HWND window = g_coreWindow.load();
        if (!window) {
            continue;
        }
        if (!IsWindow(window)) {
            Wh_Log(L"The lock screen window disappeared; stopping the worker.");
            break;
        }
        if (!IsWindowVisible(window)) {
            continue;
        }
        PostMessageW(window, WM_NULL, 0, 0);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Lock Screen Clock Styler initializing (Wh_ModInit)...");

    LoadSettings();

    g_runOnUiThreadMessage =
        RegisterWindowMessageW(L"Windhawk_LockScreenClockStyler_RunOnUiThread_v1");
    if (!g_runOnUiThreadMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: %u", GetLastError());
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_coreWindow = nullptr;
    g_getMessageHook = nullptr;
    g_settingsDirty = false;
    g_isDisplayOn = true;
    g_isSystemAwake = true;
    g_resumeTimeMs = 0;

    // Register for console display state (on/off) notifications
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS displayParams = {DisplaySettingCallback, nullptr};
    DWORD dispRes = PowerSettingRegisterNotification(
        &GUID_CONSOLE_DISPLAY_STATE_LOCAL,
        DEVICE_NOTIFY_CALLBACK,
        reinterpret_cast<HANDLE>(&displayParams),
        &g_hDisplayNotify);
    if (dispRes != ERROR_SUCCESS) {
        Wh_Log(L"PowerSettingRegisterNotification failed: %u", dispRes);
        g_hDisplayNotify = nullptr;
    }

    // Register for system suspend / resume notifications
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS suspendParams = {SuspendResumeCallback, nullptr};
    g_hSuspendResumeNotify = RegisterSuspendResumeNotification(
        reinterpret_cast<HANDLE>(&suspendParams),
        DEVICE_NOTIFY_CALLBACK);
    if (!g_hSuspendResumeNotify) {
        Wh_Log(L"RegisterSuspendResumeNotification failed: %u", GetLastError());
        g_hSuspendResumeNotify = nullptr;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        if (g_hDisplayNotify) {
            PowerSettingUnregisterNotification(g_hDisplayNotify);
            g_hDisplayNotify = nullptr;
        }
        if (g_hSuspendResumeNotify) {
            UnregisterSuspendResumeNotification(g_hSuspendResumeNotify);
            g_hSuspendResumeNotify = nullptr;
        }
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Lock Screen Clock Styler uninitializing (Wh_ModUninit)...");

    if (g_hDisplayNotify) {
        PowerSettingUnregisterNotification(g_hDisplayNotify);
        g_hDisplayNotify = nullptr;
    }
    if (g_hSuspendResumeNotify) {
        UnregisterSuspendResumeNotification(g_hSuspendResumeNotify);
        g_hSuspendResumeNotify = nullptr;
    }

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    HWND window = g_coreWindow.load();
    HHOOK hook = g_getMessageHook.load();
    if (window && hook && IsWindow(window)) {
        // Synchronous. When this returns, TeardownOnUiThread has finished and
        // the WH_GETMESSAGE hook has been removed from the UI thread.
        if (!RunOnUiThread(window, TeardownOnUiThread, nullptr)) {
            Wh_Log(L"Failed to run teardown on the UI thread.");
            HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
            if (remainingHook) {
                UnhookWindowsHookEx(remainingHook);
            }
            // XAML objects are intentionally left alone here. They belong to
            // the UI thread and releasing them from this thread would be unsafe.
        }
    } else {
        HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
        if (remainingHook) {
            UnhookWindowsHookEx(remainingHook);
        }
    }

    g_coreWindow = nullptr;
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    Wh_Log(L"Lock Screen Clock Styler uninitialized.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Mod settings changed (Wh_ModSettingsChanged)...");

    HWND window = g_coreWindow.load();
    HHOOK hook = g_getMessageHook.load();
    if (hook) {
        // The UI hook is active, so g_settings must only be modified on the
        // lock screen UI thread.
        if (window && IsWindow(window) &&
            RunOnUiThread(window, RebuildOnUiThread, nullptr)) {
            return;
        }
        // The UI thread did not process the synchronous task. Let the next
        // tick retry instead of touching XAML state or settings from here.
        Wh_Log(L"Deferring settings change to the UI thread.");
        g_settingsDirty = true;
    } else {
        // Not attached yet: the first UI tick reloads settings on the UI
        // thread (RebuildOnUiThread), so don't touch g_settings from here.
        g_settingsDirty = true;
    }
}
