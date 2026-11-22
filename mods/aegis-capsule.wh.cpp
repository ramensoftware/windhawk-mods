// ==WindhawkMod==
// @id              aegis-capsule
// @name            aegisCapsule
// @description     Direct2D & WASAPI top-bar status capsule for Windows 10/11 with media controls, voice recording, Pomodoro timer, quick notes, and hardware telemetry.
// @version         0.1.0
// @author          MrSpy00
// @github          https://github.com/MrSpy00
// @homepage        https://github.com/MrSpy00/aegisCapsule
// @include         explorer.exe
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshcore -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lruntimeobject -lwindowscodecs -lavrt -lsetupapi -lwinhttp -lpdh -lwinmm
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# aegisCapsule v0.1.0

*Dynamic Liquid Glass Companion & Productivity Capsule for Windows Desktop*  
*Windows Masaüstü için Yeni Nesil Dinamik Liquid Glass Kapsülü ve Üretkenlik Asistanı*

---

## 🇹🇷 Türkçe Açıklama & Özellikler

**aegisCapsule**, Apple Vision Pro estetiği ve Liquid Glass optik teknolojisi ile geliştirilmiş, donanım hızlandırmalı (Direct2D/DirectWrite), ultra akıcı ve çok amaçlı bir dinamik masaüstü kapsülüdür. 60-240 FPS aralığında neredeyse sıfır CPU kullanımıyla çalışır.

### ✨ Temel Özellikler
- **💎 Liquid Glass Optik Motoru:** Çok katmanlı sıvı damla cam yansımaları, hareketli kaustik ışık dalgaları, çevresel ambiyans aydınlatması ve kristal berraklığında kenar kırılmaları.
- **🎮 Tam Ekran Otomatik Gizleme (Full-Screen Auto-Hide):** Oyunlarda, tam ekran video oynatıcılarda (YouTube, Netflix, VLC) ve tam ekran uygulamalarda kapsül kendiliğinden tamamen kapanır; masaüstüne dönüldüğünde pürüzsüzce geri gelir.
- **📢 Dinamik Bildirim Genişlemesi:** Ekran alıntısı, bildirim veya kopyalama işlemlerinde kapsül metnin uzunluğuna göre sağa-sola dinamik olarak genişler; yazılar asla kesilmez.
- **🔕 Sistem Bildirimlerini Gizleme (Opsiyonel):** Windows'un standart açılır bildirim pencerelerini susturup tüm bildirimleri yalnızca kapsülde zarifçe gösterme desteği.
- **🌿 Sağlık & Yaşam Asistanı (Living Wellness):** Sürekli PC açık kalma sayacı, ayarlanabilir ve hazır süreli su içme hatırlatıcıları (15/30/45/60/90/120 dk), 20-20-20 göz dinlendirme ve saatlik esneme molaları.
- **🎙️ Akıllı Ses Kaydedici:** Kapsül üzerinden tek tıkla mikrofon kaydı başlatma, canlı dalga formu görselleştirmesi ve masaüstüne yüksek kaliteli WAV kaydı.
- **📝 Hızlı Notlar & Karalama:** Panodaki metinleri veya notları anında kapsüle kaydetme ve görüntüleme.
- **🍅 Pomodoro Odak Sayacı:** 15/25/45/60 dakikalık odaklanma seansları, kısa/uzun molalar ve dairesel ilerleme halkası.
- **⛅ Canlı Hava Durumu & Atmosfer:** Şehir, sıcaklık, hissedilen, rüzgar, nem ve durum ikonları.
- **📊 7 Sekmeli Gelişmiş Dashboard:** Yaşayan Asistan & Günlük Özet, Takvim & Saat, Hava Durumu, Donanım & Oyun Overlay, Hızlı Notlar, Pomodoro, Sağlık Asistanı.
- **🖱️ Doğal Kaydırma (Natural Scroll):** Fare tekerleği aşağı = Sonraki Sekme, yukarı = Önceki Sekme.
- **🌐 Çift Dilli Yapı:** Hem 🇹🇷 Türkçe hem 🇬🇧 İngilizce tam arayüz ve sağ tık menü desteği.

---

## 🇬🇧 English Description & Features

**aegisCapsule** is an ultra-fluid, hardware-accelerated dynamic capsule companion for Windows, featuring Apple Vision Pro aesthetics, pristine Liquid Glass optics, and an intelligent living assistant.

### ✨ Key Capabilities
- **💎 Liquid Glass Optical Engine:** Multi-layer liquid droplet glass refractions, animated caustic light waves, ambient accent glow, and crisp dual-rim specular highlights.
- **🎮 Full-Screen Auto-Hide:** Automatically hides completely when games, full-screen video players (YouTube, Netflix, VLC), or full-screen apps are active. Restores seamlessly upon returning to desktop.
- **📢 Dynamic Island Notification Expansion:** Dynamically adjusts capsule width to fit notifications, screenshot alerts, and clipboard events with zero clipping.
- **🔕 Suppress Native Toast Popups (Optional):** Suppress default Windows toast banners so all alerts display exclusively inside the capsule island.
- **🌿 Living Assistant & Wellness:** Continuous PC uptime tracking, customizable hydration intervals (15/30/45/60/90/120 min presets), 20-20-20 eye strain breaks, and posture stretch alerts.
- **🎙️ Smart Voice Recorder:** One-click WASAPI microphone recording saving crystal-clear WAV audio to Desktop with live animated audio waveforms.
- **📝 Quick Notes & Scratchpad:** Instant clipboard note capturing and storage directly on the capsule.
- **🍅 Pomodoro Productivity Timer:** 15/25/45/60 min focus intervals, short/long breaks, and live circular progress rings.
- **⛅ Real-Time Weather Hub:** Location-aware temperature, feels-like, wind speed, humidity, and condition glyphs.
- **📊 7-Tab Expanded Dashboard:** Living Companion & Daily Briefing, Calendar, Weather, Hardware/Game Overlay (FPS, CPU, RAM, GPU, Disk), Notes, Pomodoro, Living Assistant.
- **🖱️ Natural Wheel Scroll:** Mouse wheel down = next tab, mouse wheel up = previous tab.
- **🌐 Full Bilingual Support:** Seamless 🇹🇷 Turkish and 🇬🇧 English localization across all cards, alerts, and settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
  - Language: auto
    $name: "Language / Dil Seçimi"
    $description: "Display language for all dashboard cards, health assistant, alerts, and menus. / Tüm panel kartları, sağlık asistanı, uyarılar ve menüler için dil seçimi."
    $options:
      - auto: "🌐 Auto (Match Windows System) / Otomatik (Windows Sistem Dili)"
      - tr: "🇹🇷 Türkçe (Turkish)"
      - en: "🇬🇧 English (İngilizce)"
  - SuppressSystemToasts: false
    $name: "Suppress Windows Native Toast Popups / Windows Sistem Bildirim Barlarını Gizle"
    $description: "Hide default Windows toast banners so all notifications display exclusively in the aegisCapsule island. / Windows'un açılır bildirim pencerelerini gizler; tüm bildirimler yalnızca kapsülde gösterilir."
  $name: "General & Language / Genel & Dil"
- Appearance:
  - Position: top-center
    $name: "Position / Konum"
    $description: "Where aegisCapsule should appear on your screen. / Kapsülün ekranda nerede görüneceğini seçin."
    $options:
      - top-center: "Top Center / Üst Orta"
      - top-left: "Top Left / Üst Sol"
      - top-right: "Top Right / Üst Sağ"
      - bottom-center: "Bottom Center / Alt Orta"
      - bottom-left: "Bottom Left / Alt Sol"
      - bottom-right: "Bottom Right / Alt Sağ"
      - center: "Screen Center / Ekran Ortası"
  - TargetMonitor: primary
    $name: "Target Monitor / Hedef Monitör"
    $description: "Select the screen to display the capsule. / Kapsülün gösterileceği monitörü seçin."
    $options:
      - 'primary': "Primary Monitor / Ana Monitör"
      - '1': "Display 1 / Monitör 1"
      - '2': "Display 2 / Monitör 2"
      - '3': "Display 3 / Monitör 3"
      - '4': "Display 4 / Monitör 4"
      - '5': "Display 5 / Monitör 5"
      - 'follow': "Follow Mouse (Active Monitor) / Fareyi Takip Et"
  - FullScreenDetection: true
    $name: "Full-screen Auto-Hide / Tam Ekran Otomatik Gizleme"
    $description: "Automatically hide capsule completely during full-screen games, movies, and apps. / Oyun veya film gibi tam ekran uygulamalarda kapsülü tamamen gizler."
  - OffsetX: 0
    $name: "Offset X (px) / X Ekseni Kaydırma"
    $description: "Horizontal fine-tuning in pixels. / Yatay konum ince ayarı (piksel)."
  - OffsetY: 0
    $name: "Offset Y (px) / Y Ekseni Kaydırma"
    $description: "Vertical fine-tuning in pixels. / Dikey konum ince ayarı (piksel)."
  - EdgeMargin: 8
    $name: "Screen Edge Margin (px) / Ekran Kenar Boşluğu"
    $description: "Distance from the edge of the monitor. / Ekran kenarından uzaklık."
  - SizeScale: '1.0'
    $name: "Overall Size Scale / Genel Boyut Ölçeği"
    $description: "Adjust global UI size scale multiplier. / Genel arayüz boyut ölçeği çarpanı."
    $options:
      - '0.5': "0.5x (Mini)"
      - '0.6': "0.6x"
      - '0.7': "0.7x"
      - '0.8': "0.8x (Compact)"
      - '0.9': "0.9x"
      - '1.0': "1.0x (Standard)"
      - '1.1': "1.1x"
      - '1.2': "1.2x (Medium)"
      - '1.3': "1.3x"
      - '1.5': "1.5x (Large)"
      - '1.8': "1.8x"
      - '2.0': "2.0x (Extra Large)"
  - CornerStyle: pill
    $name: "Corner Roundness & Shape / Kenar Yuvarlaklığı & Şekil"
    $description: "Choose how round or sharp the capsule edges appear. / Kapsül kenarlarının biçimini seçin."
    $options:
      - pill: "Full Pill (Continuous Round Capsule) / Tam Hap (Yuvarlak Kapsül)"
      - squircle: "Vision Squircle (Smooth Rounded Rectangle) / Vision Squircle (Yumuşak Dikdörtgen)"
      - modern-box: "Modern Box (8px Corner Radius) / Modern Kutu (8px)"
      - sharp: "Sharp Box (0px Corner Radius) / Keskin Kutu (0px)"
      - custom: "Custom Radius / Özel Yarıçap"
  - CustomCornerRadius: 18
    $name: "Custom Corner Radius (px) / Özel Köşe Yarıçapı"
    $description: "Applied when Corner Roundness is set to Custom. / Şekil 'Özel' seçildiğinde geçerlidir."
  - IdleDisplayMode: '0'
    $name: "Default Idle Display Style / Varsayılan Bekleme Modu Görünümü"
    $description: "Choose what the capsule shows by default when collapsed. / Kapsül beklemedeyken varsayılan olarak ne göstereceğini seçin."
    $options:
      - '0': "Smart Dynamic (Auto-adapts to Pomodoro / Weather) / Akıllı Dinamik"
      - '1': "Clock & Weather Duo (Centered 50/50 Classic) / Saat ve Hava Durumu"
      - '2': "Clock Only (Minimalist Large Time) / Sadece Saat"
      - '3': "System Telemetry HUD (Clock + CPU + RAM) / Sistem Durum Bilgisi"
      - '4': "Weather & Atmosphere Focus / Hava Durumu Odaklı"
      - '5': "Live Pomodoro Focus / Canlı Pomodoro Sayacı"
  - IdleTextScale: 100
    $name: "Collapsed Idle Font Scale (%) / Kapalı Kapsül Yazı Boyutu (%)"
    $description: "Adjust the text and clock size in the collapsed capsule (50% to 200%). / Kapalı kapsül içindeki yazı ve saat boyutunu ölçekleyin (%50 - %200)."
  - IdleClockShowSeconds: false
    $name: "Show Seconds in Clock / Saatte Saniyeleri Göster"
    $description: "Display seconds in the minimal clock format (e.g. 14:32:08). / Minimal saatte saniyeleri göster (Örn. 14:32:08)."
  - IdleClockTimeFormat: auto
    $name: "Clock Time Format / Saat Zaman Formatı"
    $description: "Choose between 24-Hour, 12-Hour (AM/PM), or System Locale default. / 24 Saat, 12 Saat (ÖÖ/ÖS) veya Sistem varsayılanı."
    $options:
      - auto: "Auto (System Locale Default) / Otomatik (Sistem Dili)"
      - 24h: "24-Hour (14:30) / 24 Saatlik"
      - 12h: "12-Hour (2:30 PM) / 12 Saatlik"
  - IdleClockBold: true
    $name: "Bold Idle Typography / Kalın Yazı Tipi"
    $description: "Use SemiBold font weight for crisp visibility in the minimal capsule. / Kapalı kapsülde daha net okunabilirlik için yarı kalın yazı tipi kullan."
  - CollapsedWidth: 200
    $name: "Collapsed Idle Width (px) / Kapalı Kapsül Genişliği"
    $description: "Width of the minimal idle capsule before size scaling. Default is 200. / Standart kapalı kapsül genişliği."
  - CollapsedHeight: 38
    $name: "Collapsed Idle Height (px) / Kapalı Kapsül Yüksekliği"
    $description: "Height of the minimal idle capsule before size scaling. Default is 38. / Standart kapalı kapsül yüksekliği."
  - ExpandedWidth: 480
    $name: "Expanded Dashboard Width (px) / Açık Dashboard Genişliği"
    $description: "Width of the expanded dashboard before size scaling. Default is 480. / Genişletilmiş panel genişliği."
  - ExpandedHeight: 190
    $name: "Expanded Dashboard Height (px) / Açık Dashboard Yüksekliği"
    $description: "Height of the expanded dashboard before size scaling. Default is 190. / Genişletilmiş panel yüksekliği."
  - AutoHideIdleSeconds: '0'
    $name: "Auto-hide idle capsule / Boşta Otomatik Gizleme"
    $description: "Hide the idle capsule after inactivity. 0 keeps it always visible. / Hareketsizlik sonrası kapsülü gizleme süresi."
    $options:
      - '-1': "Hide instantly / Anında Gizle"
      - '0': "Never hide (Always visible) / Asla Gizleme (Sürekli Görünür)"
      - '3': "Hide after 3 seconds / 3 saniye sonra gizle"
      - '5': "Hide after 5 seconds / 5 saniye sonra gizle"
      - '10': "Hide after 10 seconds / 10 saniye sonra gizle"
      - '15': "Hide after 15 seconds / 15 saniye sonra gizle"
      - '30': "Hide after 30 seconds / 30 saniye sonra gizle"
  - UnhideOnHover: true
    $name: "Unhide on hover / Üzerine Gelince Göster"
    $description: "Allow a hidden capsule to reappear when hovering near its anchor. / Fare yaklaştığında gizli kapsülü açar."
  - ExpandOnHover: true
    $name: "Expand on hover / Üzerine Gelince Genişlet"
    $description: "Expand the capsule automatically when hovered. / Fare üzerine gelince paneli otomatik açar."
  - AlwaysOnTop: true
    $name: "Always on top / Her Zaman Üstte"
    $description: "Keeps aegisCapsule above all other windows. / Kapsülü tüm pencerelerin üstünde tutar."
  - AutoDpiScale: true
    $name: "Auto DPI scaling / Otomatik DPI Ölçekleme"
    $description: "Automatically scales font sizes and metrics to match monitor DPI. / Monitör DPI değerine göre otomatik ölçekler."
  - ClickThroughIdle: false
    $name: "Click-through when idle / Boştayken Tıklamayı İlet"
    $description: "Let mouse clicks pass through the minimal idle capsule. / Kapalıyken tıklamaların arkadaki pencereye geçmesini sağlar."
  $name: "Appearance & Placement / Görünüm & Konum"
- Optics:
  - LiquidGlass: true
    $name: "Liquid Glass Optical Engine / Liquid Glass Optik Motoru"
    $description: "Master switch for multi-layer frosted glass, caustics, and refraction. / Çok katmanlı buzlu cam ve kırılma efektleri."
  - LiquidGlassIntensity: 80
    $name: "Liquid Glass Intensity / Cam Efekti Yoğunluğu"
    $description: "0 to 100. Controls the prominence of the glass sheen. / 0-100 arası cam parlaklık yoğunluğu."
  - GlassSheen: true
    $name: "Frosted Glass Gradient Sheen / Gradyan Cam Parlaması"
    $description: "Renders a smooth vertical ambient light sheen across the capsule face. / Kapsül yüzeyinde dikey ışık parlaması."
  - GlassCaustics: true
    $name: "Animated Caustic Light Wave / Hareketli Kaustik Işık Dalgası"
    $description: "Renders an ultra-subtle animated sinusoidal light shimmer. / Yüzeyde akıcı sinüzoidal ışık dalgası oluşturur."
  - GlassSpecular: true
    $name: "1px Top Specular Highlight / Üst Kenar Yansıma Çizgisi"
    $description: "Adds a crisp, elegant top rim reflection line. / Üst kenara zarif 1px yansıma çizgisi ekler."
  - GlassRefractionRim: true
    $name: "Edge Refraction Border / Kenar Kırılma Sınırı"
    $description: "Simulates optical refraction along the outer capsule perimeter. / Kapsül kenarlarında optik kırılma sınırı çizer."
  - AccentGlow: true
    $name: "Ambient Accent Glow / Çevresel Vurgu Işığı"
    $description: "Dynamic breathing ambient colored halo behind the capsule. / Kapsül arkasında nefes alan renkli ambiyans halesi."
  - AccentGlowIntensity: 80
    $name: "Accent Glow Intensity / Vurgu Işığı Yoğunluğu"
    $description: "0 to 100. Controls the brightness of the ambient glow. / Çevresel ışığın parlaklığı (0-100)."
  - AccentGlowRadius: 28
    $name: "Accent Glow Radius (px) / Vurgu Işığı Yarıçapı"
    $description: "Spread distance of the ambient colored halo. / Renk halesinin yayılma mesafesi (px)."
  - SoftShadow: true
    $name: "Multi-Pass Soft Drop Shadow / Yumuşak Gölge"
    $description: "Progressive soft atmospheric drop shadow beneath the capsule. / Kapsül altında yumuşak atmosferik gölge oluşturur."
  - ShadowSoftness: 70
    $name: "Shadow Softness / Gölge Yumuşaklığı"
    $description: "0 to 100. Controls shadow blur and depth. / Gölge derinliği ve yumuşaklığı (0-100)."
  $name: "Optics & Liquid Glass / Optik & Liquid Glass"
- Assistant:
  - EnableWellnessReminders: true
    $name: "Living Capsule & Wellness Alerts / Yaşayan Kapsül & Sağlık Uyarıları"
    $description: "Periodic smart health reminders for hydration, eye rest, and posture. / Su içme, 20-20-20 göz dinlendirme ve mola hatırlatıcıları."
  - HydrationPreset: '45'
    $name: "Hydration Reminder Timing / Su İçme Hatırlatıcı Aralığı"
    $description: "Choose a preset interval or custom timing. / Hazır süre seçin veya özel dakika girin."
    $options:
      - '15': "15 min / 15 dakika"
      - '30': "30 min / 30 dakika"
      - '45': "45 min (Standard) / 45 dakika (Standart)"
      - '60': "60 min (1 Hour) / 1 Saat"
      - '90': "90 min (1.5 Hours) / 1.5 Saat"
      - '120': "120 min (2 Hours) / 2 Saat"
      - 'custom': "Custom Minutes (below) / Özel Dakika"
  - HydrationIntervalMinutes: 45
    $name: "Hydration Custom Minutes / Su İçme Özel Dakika (Özel Seçildiğinde)"
    $description: "Custom interval in minutes (5-360 min). / Özel su içme periyodu (5-360 dk)."
  - EyeRestReminder: true
    $name: "20-20-20 Eye Strain Break / 20-20-20 Göz Dinlendirme Kuralı"
    $description: "Reminds you to look 20 feet away for 20 seconds. / Düzenli aralıklarla 20 saniye uzağa bakma molası verir."
  - EyeRestMinutes: 20
    $name: "Eye Rest Interval (min) / Göz Dinlendirme Süresi (dk)"
    $description: "Interval in minutes (10-60 min). / Göz dinlendirme periyodu (dakika)."
  - PostureReminder: true
    $name: "Posture & Walk Reminder / Duruş & Yürüyüş Hatırlatıcısı"
    $description: "Reminds you to stretch and check posture. / Duruş düzeltme ve esneme uyarısı gösterir."
  - PosturePreset: '60'
    $name: "Posture Reminder Timing / Duruş & Esneme Süresi"
    $description: "Choose a preset interval or custom timing. / Hazır süre seçin veya özel dakika girin."
    $options:
      - '30': "30 min / 30 dakika"
      - '45': "45 min / 45 dakika"
      - '60': "60 min (1 Hour) / 1 Saat"
      - '90': "90 min (1.5 Hours) / 1.5 Saat"
      - '120': "120 min (2 Hours) / 2 Saat"
      - 'custom': "Custom Minutes (below) / Özel Dakika"
  - PostureIntervalMinutes: 60
    $name: "Posture Custom Minutes / Duruş Özel Dakika (Özel Seçildiğinde)"
    $description: "Custom interval in minutes (15-360 min). / Özel duruş esneme periyodu (15-360 dk)."
  - UptimeAlertHours: 2
    $name: "Continuous PC Uptime Alert (hours) / Sürekli Kullanım Uyarısı (saat)"
    $description: "Alert when at PC for extended hours (1-12 hours, 0 to disable). / Bilgisayar başında uzun süre kalındığında uyarı verir (0 = kapalı)."
  $name: "Living Assistant & Health / Akıllı Asistan & Sağlık"
- Tools:
  - PomodoroWorkMinutes: 25
    $name: "Pomodoro Focus Duration (min) / Pomodoro Çalışma Süresi (dk)"
    $description: "Work session duration in minutes (10-60). / Odaklanma çalışma periyodu süresi."
  - PomodoroBreakMinutes: 5
    $name: "Pomodoro Break Duration (min) / Pomodoro Mola Süresi (dk)"
    $description: "Short break duration in minutes (2-15). / Kısa mola süresi."
  - VoiceRecordingFolder: ""
    $name: "Voice Recording Folder / Ses Kayıt Klasörü"
    $description: "Custom folder for voice notes (blank defaults to Desktop/aegisCapsule_Recordings). / Ses kayıtlarının kaydedileceği özel klasör."
  $name: "Productivity & Tools / Üretkenlik & Araçlar"
- Themes:
  - AccentColorMode: auto
    $name: "Accent Color Mode / Vurgu Rengi Modu"
    $description: "How the glowing accent color is chosen. / Vurgu renginin nasıl belirleneceğini seçin."
    $options:
      - auto: "Auto (from album art / dynamic) / Otomatik (Albüm kapağından)"
      - system: "Windows System Accent / Windows Sistem Vurgusu"
      - custom: "Custom Hex Color / Özel Hex Rengi"
  - CustomAccentHex: "#4cc9f0"
    $name: "Custom Accent Hex / Özel Vurgu Hex Kodu"
    $description: "Hex color for accent mode (#RRGGBB or #RRGGBBAA). / Özel renk hex kodu."
  - PillBgColor: "#0D0D0F"
    $name: "Capsule Background Color / Kapsül Arka Plan Rengi"
    $description: "Hex color for background (#RRGGBB or #RRGGBBAA). / Arka plan hex rengi."
  - TextPrimaryColor: "#F7F7F7"
    $name: "Primary Text Color / Ana Metin Rengi"
    $description: "Hex color for track titles and primary metrics. / Başlık ve ana metin rengi."
  - TextSecondaryColor: "#888888"
    $name: "Secondary Text Color / İkincil Metin Rengi"
    $description: "Hex color for artist names and muted labels. / Sanatçı ve ikincil etiket rengi."
  - TintIntensity: 72
    $name: "Background Tint Depth / Arka Plan Ton Derinliği"
    $description: "0 to 100. Controls the depth of the dark base tint. / Koyu baz ton derinliği."
  - PillOpacity: 96
    $name: "Capsule Transparency / Kapsül Saydamlığı"
    $description: "10 to 100. Lower values make the capsule glass more translucent. / Kapsül cam saydamlığı (10-100)."
  $name: "Colors & Theming / Renkler & Temalar"
- Performance:
  - AnimationSpeed: normal
    $name: "Animation Speed / Animasyon Hızı"
    $description: "Transition speed multiplier. / Geçiş hızı çarpanı."
    $options:
      - slow: "Slow (0.65x) / Yavaş"
      - normal: "Normal (1.0x) / Normal"
      - fast: "Fast (1.35x) / Hızlı"
      - instant: "Instant (5.0x Snappy) / Anlık"
  - SpringStyle: bouncy
    $name: "Spring Physics Dynamic / Yay Fiziği Karakteri"
    $description: "Motion curve for capsule expansions and morphs. / Kapsül açılma ve şekil değiştirme yay eğrisi."
    $options:
      - bouncy: "Bouncy (Apple Vision fluid bounce) / Akıcı Yaylı"
      - smooth: "Smooth (Critically damped gentle glide) / Yumuşak Kayma"
      - snappy: "Snappy (High tension responsive snap) / Seri & Atik"
      - stiff: "Stiff (Minimal overshoot) / Sert"
  - FrameRateTarget: '60'
    $name: "Target Refresh Rate / Hedef Yenileme Hızı"
    $description: "Maximum render loop refresh rate. / Maksimum kare hızı (FPS)."
    $options:
      - '30': "30 FPS (Power Saver) / 30 FPS (Güç Tasarrufu)"
      - '60': "60 FPS (Standard Smooth) / 60 FPS (Standart Akıcı)"
      - '120': "120 FPS (High Refresh) / 120 FPS (Yüksek Akıcılık)"
      - '144': "144 FPS (Ultra Refresh) / 144 FPS (Ultra)"
      - '240': "240 FPS (Extreme Esports) / 240 FPS (Ekstrem)"
  - LowPowerMode: false
    $name: "Adaptive Power Saver / Uyarlanabilir Güç Tasarrufu"
    $description: "Automatically throttles idle rendering when on battery. / Pildeyken render hızını otomatik düşürür."
  $name: "Physics & Performance / Fizik & Performans"
- Modules:
  - NotificationAutoExpand: true
    $name: "Auto-Expand Notification Pill / Bildirimde Barı Otomatik Genişlet"
    $description: "Dynamically widen capsule bar horizontally to display full text without truncation. / Bildirimlerde kayan yazı yerine barı sağa sola uzatıp tam metni gösterir."
  - Media: true
    $name: "Media module / Medya Modülü"
    $description: "Shows album art, song info, scrubber, and playback controls. / Şarkı bilgisi, albüm kapağı ve medya kontrolleri."
  - Notification: true
    $name: "Notification module / Bildirim Modülü"
    $description: "Shows rich Windows toast notifications and banner popups. / Windows bildirimleri ve uyarılar."
  - Clipboard: true
    $name: "Clipboard module / Pano Modülü"
    $description: "Shows a quick preview of copied text or screenshot images. / Kopyalanan metin ve ekran görüntüsü önizlemesi."
  - Battery: true
    $name: "Battery module / Pil Modülü"
    $description: "Shows alerts when battery is low or charger is connected. / Düşük pil ve şarj durumu uyarıları."
  - Progress: true
    $name: "Progress module / İlerleme Modülü"
    $description: "Shows a progress ring for active downloads or tasks. / İndirme ve aktarımlar için ilerleme halkası."
  - Volume: true
    $name: "Volume OSD module / Ses OSD Modülü"
    $description: "Shows instant volume bar and mute status on system volume change. / Ses seviyesi değiştiğinde anlık OSD çubuğu."
  - CapsLock: true
    $name: "Caps/Num Lock OSD module / Tuş Kilidi OSD Modülü"
    $description: "Shows status indicator when Caps Lock or Num Lock is toggled. / Caps Lock / Num Lock basıldığında durum göstergesi."
  - Device: true
    $name: "Device connection OSD / Aygıt Bağlantı OSD"
    $description: "Shows alert when USB or Bluetooth devices are plugged in. / USB ve Bluetooth cihaz bağlantı bildirimleri."
  - GameOverlay: false
    $name: "Enable Game Overlay Mode / Oyun Overlay Modu"
    $description: "Replaces the clock with live stats like FPS, CPU, GPU, and RAM. / FPS, CPU, GPU ve RAM kullanımını gösterir."
  - ShowMetricText: true
    $name: "Show labels in metric chips / Metrik Etiketlerini Göster"
    $description: "Adds text labels (like 'CPU') inside metric bars. / Metrik çubuklarında yazı etiketlerini gösterir."
  - ShowMetricsInIdle: false
    $name: "Show micro-metrics in idle / Kapalı Kapsülde CPU/RAM Göster"
    $description: "Displays small CPU and RAM chips in the collapsed idle capsule. / Kapalı hap görünümünde küçük CPU/RAM çipi gösterir."
  - WeatherCity: ""
    $name: "Weather City (Optional) / Hava Durumu Şehri (İsteğe Bağlı)"
    $description: "Enter your city (e.g. London, Istanbul). Leave blank for auto IP. / Şehir adı yazın. Boş bırakırsanız otomatik konum kullanılır."
  - WeatherFahrenheit: false
    $name: "Use Fahrenheit / Fahrenheit Kullan"
    $description: "Display weather temperature in Fahrenheit. / Sıcaklığı Fahrenheit cinsinden gösterir."
  - NotificationDuration: "4.0"
    $name: "Notification Duration (sec) / Bildirim Gösterim Süresi (sn)"
    $description: "How long to display new notifications. / Bildirimlerin ekranda kalma süresi."
    $options:
      - '2.0': "2.0s"
      - '3.0': "3.0s"
      - '4.0': "4.0s"
      - '6.0': "6.0s"
      - '8.0': "8.0s"
  - ClipboardDuration: "2.5"
    $name: "Clipboard Duration (sec) / Pano Gösterim Süresi (sn)"
    $description: "How long to display clipboard popups. / Pano kopyalama bildiriminin süresi."
    $options:
      - '1.5': "1.5s"
      - '2.5': "2.5s"
      - '4.0': "4.0s"
  - VolumeDuration: "1.8"
    $name: "Volume Duration (sec) / Ses Gösterim Süresi (sn)"
    $description: "How long to display volume popups. / Ses OSD süresi."
  - CapsLockDuration: "1.5"
    $name: "Caps Lock Duration (sec) / Tuş Kilidi Süresi (sn)"
    $description: "How long to display Caps/Num Lock popups. / Tuş kilidi OSD süresi."
  - DeviceDuration: "3.0"
    $name: "Device Duration (sec) / Cihaz Bildirim Süresi (sn)"
    $description: "How long to display device alerts. / Aygıt bağlantı bildirimi süresi."
  $name: "Modules & Features / Modüller & Özellikler"
- FineTuning:
  - SplitGap: 12
    $name: "Split pill gap (px) / Ayrık Kapsül Boşluğu"
    $description: "Gap between two pills in Split mode. / Ayrık modda iki hap arası boşluk (px)."
  - HoverScalePercent: 102
    $name: "Hover scale (%) / Fare Üzerindeyken Büyüme Oranı (%)"
    $description: "Pill scale on hover/pinned (100 = 1.0x, 105 = 1.05x). / Fare üzerindeyken büyüme yüzdesi."
  - NudgeOffsetPx: 8
    $name: "Nudge distance (px) / Bildirim Zıplama Mesafesi (px)"
    $description: "Vertical bounce distance on new event (0 disables). / Yeni bildirim geldiğinde dikey sekme mesafesi."
  - WaveformBarCount: 22
    $name: "Waveform bar count / Dalga Çubuğu Sayısı"
    $description: "Number of bars in audio waveform (8-32). / Ses dalgasındaki çubuk sayısı."
  - WaveformGapPx: '2.5'
    $name: "Waveform bar gap (px) / Dalga Çubukları Arası Boşluk"
    $description: "Gap between waveform bars. / Dalga çubukları arasındaki mesafe."
  - ProgressRingThickness: '2.8'
    $name: "Progress ring thickness (px) / İlerleme Halkası Kalınlığı"
    $description: "Thickness of progress ring stroke. / İlerleme halkasının çizgi kalınlığı."
  - PaginationDotSize: '2.5'
    $name: "Pagination dot radius (px) / Sayfa Noktası Yarıçapı"
    $description: "Radius of pagination dots. / Panel geçiş noktalarının boyutu."
  - PrivacyDotSize: '3.8'
    $name: "Privacy dot radius (px) / Gizlilik Noktası Yarıçapı"
    $description: "Radius of mic/camera privacy dots. / Mikrofon ve kamera gizlilik noktalarının boyutu."
  - TopHighlightOpacity: 35
    $name: "Top highlight opacity (%) / Üst Yansıma Saydamlığı (%)"
    $description: "Opacity of 1px top rim highlight (0-100). / Üst kenar yansımasının opaklığı."
  - RimThickness: '0.9'
    $name: "Refraction rim thickness (px) / Kırılma Kenarlığı Kalınlığı"
    $description: "Stroke thickness of outer refraction border. / Dış kırılma kenarlığının kalınlığı."
  - ShadowSpreadPx: 10
    $name: "Shadow spread (px) / Gölge Yayılımı (px)"
    $description: "Maximum spread of soft drop shadow (0-24). / Gölgenin yayılma genişliği."
  - ShadowYOffsetPx: 4
    $name: "Shadow Y offset (px) / Gölge Dikey Kaydırma"
    $description: "Vertical offset of drop shadow (0-12). / Gölgenin dikey uzaklığı."
  - CausticsSpeed: '1.3'
    $name: "Caustics animation speed / Kaustik Animasyon Hızı"
    $description: "Speed multiplier for glass caustics wave. / Cam yüzeyindeki ışık dalgası hareket hızı."
  - ContentFadeOnResize: true
    $name: "Content fade on resize / Boyut Değişirken İçeriği Yumuşat"
    $description: "Fade pill content alpha during morph. / Şekil değişirken içerik saydamlığını yumuşatır."
  - UseDwmBlurBehind: false
    $name: "DWM blur behind / DWM Gerçek Arka Plan Bulanıklığı"
    $description: "Enable real backdrop blur via DwmEnableBlurBehindWindow. / DWM üzerinden gerçek arka plan bulanıklığı uygular."
  - WeatherUpdateMinutes: 10
    $name: "Weather update interval (min) / Hava Durumu Güncelleme Aralığı (dk)"
    $description: "Weather fetch interval in minutes (5-60). / Hava durumu verisi çekme sıklığı."
  $name: "Fine Tuning & Advanced / İnce Ayarlar & Gelişmiş"
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <unknwn.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <setupapi.h>
#include <dbt.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shcore.h>
#include <windowsx.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <objbase.h>
#include <wrl/client.h>
#include <uiautomation.h>
#include <winhttp.h>
#include <pdh.h>
#include <pdhmsg.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#if __has_include(<winrt/Windows.UI.Notifications.Management.h>) && \
    __has_include(<winrt/Windows.UI.Notifications.h>)
#define AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER 1
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.Notifications.Management.h>
#else
#define AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER 0
#endif

using Microsoft::WRL::ComPtr;
using namespace std::chrono_literals;

namespace {

constexpr wchar_t kWindowClass[] = L"aegisCapsule_Overlay";
[[maybe_unused]] constexpr UINT WM_APP_LAYOUT_CHANGED = WM_APP + 0x442;
constexpr UINT WM_APP_NEW_EVENT = WM_APP + 0x443;
constexpr float kRenderPadX = 32.0f;
constexpr float kRenderPadY = 24.0f;
constexpr int kTotalTabs = 7;

enum class IslandKind {
    Idle,
    Media,
    Progress,
    Clipboard,
    Notification,
    Volume,
    BatteryLow,
    CapsLock,
    Device,
    Recording,
    Split,
};

enum class Language {
    Auto = 0,
    Turkish = 1,
    English = 2,
};

inline bool IsTurkish(Language lang) {
    if (lang == Language::Turkish) return true;
    if (lang == Language::English) return false;
    LANGID langId = GetUserDefaultUILanguage();
    return (PRIMARYLANGID(langId) == LANG_TURKISH);
}

enum class CornerStyle {
    Pill,
    Squircle,
    ModernBox,
    Sharp,
    Custom,
};

enum class Position {
    TopCenter,
    TopLeft,
    TopRight,
    BottomCenter,
    BottomLeft,
    BottomRight,
    Center,
};

enum class SpringStyle {
    Bouncy,
    Smooth,
    Snappy,
    Stiff,
};

enum class AccentMode {
    Auto,
    System,
    Custom,
};

enum class PomodoroState {
    Stopped,
    Working,
    Break,
    LongBreak,
};

struct Settings {
    Language language = Language::Auto;
    Position position = Position::TopCenter;
    int targetMonitor = 0;
    bool fullScreenDetection = true;
    int offsetX = 0;
    int offsetY = 0;
    int edgeMargin = 8;
    float sizeScale = 1.0f;
    CornerStyle cornerStyle = CornerStyle::Pill;
    float customCornerRadius = 18.0f;
    int idleDisplayMode = 0;
    float idleTextScale = 1.0f;
    bool idleClockShowSeconds = false;
    int idleClockTimeFormat = 0; // 0 = Auto, 1 = 24h, 2 = 12h
    bool idleClockBold = true;
    float collapsedWidth = 200.0f;
    float collapsedHeight = 38.0f;
    float expandedWidth = 480.0f;
    float expandedHeight = 190.0f;
    int autoHideIdleSeconds = 0;
    bool unhideOnHover = true;
    bool alwaysOnTop = true;
    bool expandOnHover = true;
    bool autoDpiScale = true;
    bool clickThroughIdle = false;

    // Optics & Liquid Glass
    bool liquidGlass = true;
    float liquidGlassIntensity = 0.80f;
    bool glassSheen = true;
    bool glassCaustics = true;
    bool glassSpecular = true;
    bool glassRefractionRim = true;
    bool accentGlow = true;
    float accentGlowIntensity = 0.80f;
    float accentGlowRadius = 28.0f;
    bool softShadow = true;
    float shadowSoftness = 0.70f;

    // Living Assistant & Wellness
    bool enableWellnessReminders = true;
    int hydrationIntervalMinutes = 45;
    bool eyeRestReminder = true;
    int eyeRestMinutes = 20;
    bool postureReminder = true;
    int postureIntervalMinutes = 60;
    int uptimeAlertHours = 2;

    // Notifications & Toast Suppression
    bool suppressSystemToasts = false;

    // Productivity & Tools
    int pomodoroWorkMinutes = 25;
    int pomodoroBreakMinutes = 5;
    std::wstring voiceRecordingFolder;

    // Colors & Theming
    D2D1_COLOR_F pillBgColor = D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f); // #0D0D0F
    D2D1_COLOR_F textPrimaryColor = D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f); // #F7F7F7
    D2D1_COLOR_F textSecondaryColor = D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f); // #888888
    AccentMode accentMode = AccentMode::Auto;
    D2D1_COLOR_F customAccent = D2D1::ColorF(0x4cc9f0);
    float tintOpacity = 0.72f;
    float pillOpacity = 0.96f;

    // Physics & Performance
    float animationSpeed = 1.0f;
    SpringStyle springStyle = SpringStyle::Bouncy;
    int targetFps = 60;
    bool lowPowerMode = false;

    // Modules & Smart Features
    bool notificationAutoExpand = true;
    bool media = true;
    bool notification = true;
    bool clipboard = true;
    bool battery = true;
    bool progress = true;
    bool volume = true;
    bool capsLock = true;
    bool device = true;
    bool gameOverlay = false;
    bool showMetricText = true;
    bool showMetricsInIdle = false;
    std::wstring weatherCity;
    bool weatherFahrenheit = false;
    float notificationDuration = 4.0f;
    float clipboardDuration = 2.5f;
    float volumeDuration = 1.8f;
    float capsLockDuration = 1.5f;
    float deviceDuration = 3.0f;

    // Fine Tuning & Advanced
    float splitGap = 12.0f;
    float hoverScalePercent = 1.02f;
    float nudgeOffsetPx = 8.0f;
    int waveformBarCount = 22;
    float waveformGapPx = 2.5f;
    float progressRingThickness = 2.8f;
    float paginationDotSize = 2.5f;
    float privacyDotSize = 3.8f;
    float topHighlightOpacity = 0.35f;
    float rimThickness = 0.9f;
    float shadowSpreadPx = 10.0f;
    float shadowYOffsetPx = 4.0f;
    float causticsSpeed = 1.3f;
    bool contentFadeOnResize = true;
    bool useDwmBlurBehind = false;
    int weatherUpdateMinutes = 10;
};

struct BitmapPixels {
    std::vector<uint8_t> bgra;
    UINT width = 0;
    UINT height = 0;
    uint64_t generation = 0;
    D2D1_COLOR_F sampledAccent = D2D1::ColorF(0x4cc9f0);
};

struct MediaSnapshot {
    bool available = false;
    bool playing = false;
    std::wstring title;
    std::wstring artist;
    std::wstring sourceAppUserModelId;
    std::wstring sourceName;
    std::wstring sourceBadge;
    BitmapPixels art;
    BitmapPixels sourceIcon;
    uint64_t artGeneration = 0;
    uint64_t sourceIconGeneration = 0;
    double artChangedAt = 0.0;
    int64_t positionTicks = 0;
    int64_t endTicks = 0;
    int64_t lastUpdatedTicks = 0;
};

struct ClipboardSnapshot {
    bool active = false;
    bool image = false;
    std::wstring text;
    std::wstring appName;
    BitmapPixels appIcon;
    double expiresAt = 0.0;
};

struct BatterySnapshot {
    bool active = false;
    bool low = false;
    bool charging = false;
    int percent = 100;
    DWORD secondsRemaining = BATTERY_LIFE_UNKNOWN;
    double expiresAt = 0.0;
};

struct ProgressSnapshot {
    bool active = false;
    int percent = 0;
    std::wstring appName;
};

struct NotificationSnapshot {
    bool active = false;
    std::wstring app;
    std::wstring title;
    std::wstring body;
    BitmapPixels icon;
    double expiresAt = 0.0;
};

struct VolumeSnapshot {
    bool active = false;
    int percent = 0;
    bool muted = false;
    std::wstring deviceName;
    double expiresAt = 0.0;
};

struct CapsLockSnapshot {
    bool active = false;
    bool capsOn = false;
    bool numOn = false;
    bool isNumEvent = false;
    double expiresAt = 0.0;
};

enum class DeviceEventType {
    Connected,
    Disconnected,
};

struct DeviceSnapshot {
    bool active = false;
    DeviceEventType eventType = DeviceEventType::Connected;
    std::wstring deviceName;
    bool isBluetoothLike = false;
    double expiresAt = 0.0;
};

struct SystemSnapshot {
    int volumePercent = 0;
    bool volumeMuted = false;
    int cpuPercent = 0;
    int memoryPercent = 0;
    int diskFreePercent = 0;
    int renderFps = 0;
    int gpuPercent = -1;
    bool charging = false;
    bool micActive = false;      // orange dot: microphone in use
    bool cameraActive = false;   // green dot: camera in use
    std::wstring foregroundTitle;
    uint64_t uptimeSeconds = 0;
    double activeSessionStart = 0.0;
};

struct Activity {
    IslandKind kind = IslandKind::Idle;
    float width = 170.0f;
    float height = 36.0f;
};

struct WeatherSnapshot {
    bool hasData = false;
    float temperature = 0.0f;
    int weatherCode = 0;
    std::wstring city;
    std::wstring weatherDesc;
    std::wstring windSpeed;
    std::wstring windDir;
    std::wstring humidity;
    std::wstring feelsLike;
    double lastUpdated = 0.0;
};

struct PomodoroSnapshot {
    PomodoroState state = PomodoroState::Stopped;
    int remainingSeconds = 25 * 60;
    int totalSeconds = 25 * 60;
    int completedSessions = 0;
    double lastTick = 0.0;
};

struct RecordingSnapshot {
    bool active = false;
    double startedAt = 0.0;
    int seconds = 0;
    std::wstring filePath;
};

struct AssistantSnapshot {
    double nextHydrationTime = 0.0;
    double nextEyeRestTime = 0.0;
    double nextPostureTime = 0.0;
    std::wstring lastReminder;
    double lastReminderTime = 0.0;
};

struct SharedState {
    MediaSnapshot media;
    ClipboardSnapshot clipboard;
    NotificationSnapshot notification;
    VolumeSnapshot volume;
    CapsLockSnapshot capsLock;
    DeviceSnapshot device;
    BatterySnapshot battery;
    ProgressSnapshot progress;
    SystemSnapshot system;
    WeatherSnapshot weather;
    PomodoroSnapshot pomodoro;
    RecordingSnapshot recording;
    AssistantSnapshot assistant;
    std::vector<std::wstring> quickNotes;
    std::array<float, 64> waveform{};
    size_t waveformWrite = 0;
    bool muted = false;
};

// Global synchronization primitives
std::mutex g_stateMutex;
SharedState g_state;
std::mutex g_settingsMutex;
Settings g_settings;

std::atomic<bool> g_running = false;
std::atomic<bool> g_layoutDirty = true;
std::atomic<int> g_idleTab = 0;
std::atomic<int> g_pressedMediaButton = -1;
std::atomic<double> g_lastNudgeTime = 0.0;
std::atomic<uint64_t> g_artGenerationCounter = 1;
std::atomic<bool> g_isFullScreenActive = false;
std::atomic<bool> g_isRecording = false;
std::mutex g_recordingMutex;

std::atomic<HWND> g_hwnd = nullptr; // atomic: written by render thread, read by TriggerNudge/others
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsChangedEvent = nullptr;
HANDLE g_renderThread = nullptr;
HANDLE g_mediaThread = nullptr;
HANDLE g_audioThread = nullptr;
HANDLE g_weatherThread = nullptr;
HANDLE g_notificationThread = nullptr;
HANDLE g_keyboardThread = nullptr;
HANDLE g_assistantThread = nullptr;
HANDLE g_recordingThread = nullptr;
DWORD g_renderThreadId = 0;
DWORD g_keyboardThreadId = 0;
UINT g_shellHookMessage = 0;

HHOOK g_keyboardHook = nullptr;
FILETIME g_prevIdleTime{};
FILETIME g_prevKernelTime{};
FILETIME g_prevUserTime{};

constexpr GUID kSubTypeIeeeFloat = {
    0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

void TriggerNudge();
void PushAudioChunks(BYTE* data, UINT32 frames, WAVEFORMATEX* format);
void UpdateSystemMetrics();
void UpdateBatteryMetrics();
void UpdatePrivacyIndicators();
void DismissTransientState();
void ShowContextMenu(HWND hwnd, POINT screenPoint);
void StartVoiceRecording();
void StopVoiceRecording();
void TriggerCustomAlert(const std::wstring& app, const std::wstring& title, const std::wstring& body, double duration);

// Mathematical & helper routines
inline float Clamp(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

inline int ClampInt(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline int Modulo(int a, int b) {
    if (b <= 0) return 0;
    return ((a % b) + b) % b;
}

double NowSeconds() {
    using namespace std::chrono;
    return duration_cast<duration<double>>(steady_clock::now().time_since_epoch()).count();
}

Settings GetSettingsSnapshot() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

D2D1_COLOR_F ColorFromHex(std::wstring text, D2D1_COLOR_F fallback) {
    if (!text.empty() && text[0] == L'#') {
        text.erase(text.begin());
    }

    if (text.size() == 3) {
        wchar_t expanded[7] = {text[0], text[0], text[1], text[1], text[2], text[2], 0};
        text = expanded;
    }

    if (text.size() == 6) {
        wchar_t* end = nullptr;
        const unsigned long rgb = wcstoul(text.c_str(), &end, 16);
        if (!end || *end != L'\0') {
            return fallback;
        }

        return D2D1::ColorF(
            ((rgb >> 16) & 0xff) / 255.0f,
            ((rgb >> 8) & 0xff) / 255.0f,
            (rgb & 0xff) / 255.0f,
            1.0f);
    } else if (text.size() == 8) {
        wchar_t* end = nullptr;
        const unsigned long rgba = wcstoul(text.c_str(), &end, 16);
        if (!end || *end != L'\0') {
            return fallback;
        }

        return D2D1::ColorF(
            ((rgba >> 24) & 0xff) / 255.0f,
            ((rgba >> 16) & 0xff) / 255.0f,
            ((rgba >> 8) & 0xff) / 255.0f,
            (rgba & 0xff) / 255.0f);
    }

    return fallback;
}

float GetPrimaryMonitorDpiScale() {
    POINT pt = {0, 0};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
        GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor"));
    if (pGetDpiForMonitor) {
        pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY);
    }
    return dpiX / 96.0f;
}

float CalculateCornerRadius(float height, const Settings& settings) {
    switch (settings.cornerStyle) {
        case CornerStyle::Sharp:
            return 0.0f;
        case CornerStyle::ModernBox:
            return std::min(height * 0.5f, 8.0f * settings.sizeScale);
        case CornerStyle::Squircle:
            return std::min(height * 0.5f, 22.0f * settings.sizeScale);
        case CornerStyle::Custom:
            return std::min(height * 0.5f, std::max(0.0f, settings.customCornerRadius) * settings.sizeScale);
        case CornerStyle::Pill:
        default:
            return height * 0.5f;
    }
}

class SpringValue {
   public:
    void Reset(float value) {
        position_ = value;
        target_ = value;
        velocity_ = 0.0f;
    }

    void SetTarget(float target) {
        target_ = target;
    }

    float GetPosition() const {
        return position_;
    }

    float GetTarget() const {
        return target_;
    }

    bool IsSettled(float eps = 0.05f) const {
        return std::abs(target_ - position_) < eps && std::abs(velocity_) < eps;
    }

    void UpdateWith(float target, float stiffness, float damping, float dt) {
        target_ = target;
        dt = std::min(dt, 0.05f);
        const float force = -stiffness * (position_ - target_);
        const float dampingForce = -damping * velocity_;
        const float acceleration = force + dampingForce;
        velocity_ += acceleration * dt;
        position_ += velocity_ * dt;
    }

    void Update(float target, SpringStyle style, float speedMultiplier, float dt) {
        float stiffness = 320.0f;
        float damping = 22.0f;

        switch (style) {
            case SpringStyle::Smooth:
                stiffness = 240.0f;
                damping = 31.0f;
                break;
            case SpringStyle::Snappy:
                stiffness = 460.0f;
                damping = 34.0f;
                break;
            case SpringStyle::Stiff:
                stiffness = 380.0f;
                damping = 40.0f;
                break;
            case SpringStyle::Bouncy:
            default:
                stiffness = 320.0f;
                damping = 22.0f;
                break;
        }

        stiffness *= (speedMultiplier * speedMultiplier);
        damping *= speedMultiplier;

        UpdateWith(target, stiffness, damping, dt);
    }

   private:
    float position_ = 0.0f;
    float target_ = 0.0f;
    float velocity_ = 0.0f;
};

bool EqualsNoCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                      [](wchar_t ca, wchar_t cb) { return towlower(ca) == towlower(cb); });
}

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    Settings next;

    // Language
    const int localLang = Wh_GetIntValue(L"LanguageOverride", -1);
    if (localLang >= 0) {
        if (localLang == 1) next.language = Language::Turkish;
        else if (localLang == 2) next.language = Language::English;
        else next.language = Language::Auto;
    } else {
        std::wstring langStr = GetStringSettingCopy(L"General.Language");
        if (EqualsNoCase(langStr, L"tr") || EqualsNoCase(langStr, L"1") || EqualsNoCase(langStr, L"turkish")) {
            next.language = Language::Turkish;
        } else if (EqualsNoCase(langStr, L"en") || EqualsNoCase(langStr, L"2") || EqualsNoCase(langStr, L"english")) {
            next.language = Language::English;
        } else {
            const int langInt = Wh_GetIntSetting(L"General.Language");
            if (langInt == 1) next.language = Language::Turkish;
            else if (langInt == 2) next.language = Language::English;
            else next.language = Language::Auto;
        }
    }

    // Suppress System Toast Popups
    next.suppressSystemToasts = Wh_GetIntSetting(L"General.SuppressSystemToasts") != 0;

    // Position
    const std::wstring position = GetStringSettingCopy(L"Appearance.Position");
    if (EqualsNoCase(position, L"top-left")) next.position = Position::TopLeft;
    else if (EqualsNoCase(position, L"top-right")) next.position = Position::TopRight;
    else if (EqualsNoCase(position, L"bottom-center")) next.position = Position::BottomCenter;
    else if (EqualsNoCase(position, L"bottom-left")) next.position = Position::BottomLeft;
    else if (EqualsNoCase(position, L"bottom-right")) next.position = Position::BottomRight;
    else if (EqualsNoCase(position, L"center")) next.position = Position::Center;
    else next.position = Position::TopCenter;

    // Monitor
    std::wstring mon = GetStringSettingCopy(L"Appearance.TargetMonitor");
    if (mon == L"primary" || mon.empty()) next.targetMonitor = 0;
    else if (mon == L"follow") next.targetMonitor = -1;
    else next.targetMonitor = _wtoi(mon.c_str());

    // Offsets & Margins
    next.offsetX = Wh_GetIntSetting(L"Appearance.OffsetX");
    next.offsetY = Wh_GetIntSetting(L"Appearance.OffsetY");
    const int marginSetting = Wh_GetIntSetting(L"Appearance.EdgeMargin");
    next.edgeMargin = marginSetting != 0 ? marginSetting : 8;

    // Size Scale
    const std::wstring scale = GetStringSettingCopy(L"Appearance.SizeScale");
    if (!scale.empty()) {
        wchar_t* end = nullptr;
        float parsedScale = wcstof(scale.c_str(), &end);
        if (end != scale.c_str() && parsedScale > 0.1f && parsedScale < 10.0f) {
            next.sizeScale = parsedScale;
        }
    }
    if (Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0) {
        next.sizeScale *= GetPrimaryMonitorDpiScale();
    }

    // Corner Style & Radius
    const std::wstring cornerStr = GetStringSettingCopy(L"Appearance.CornerStyle");
    if (EqualsNoCase(cornerStr, L"squircle")) next.cornerStyle = CornerStyle::Squircle;
    else if (EqualsNoCase(cornerStr, L"modern-box")) next.cornerStyle = CornerStyle::ModernBox;
    else if (EqualsNoCase(cornerStr, L"sharp")) next.cornerStyle = CornerStyle::Sharp;
    else if (EqualsNoCase(cornerStr, L"custom")) next.cornerStyle = CornerStyle::Custom;
    else next.cornerStyle = CornerStyle::Pill;
    
    const int customRad = Wh_GetIntSetting(L"Appearance.CustomCornerRadius");
    next.customCornerRadius = customRad > 0 ? static_cast<float>(customRad) : 18.0f;

    const int localCorner = Wh_GetIntValue(L"CornerStyleOverride", -1);
    if (localCorner >= 0 && localCorner <= 4) {
        next.cornerStyle = static_cast<CornerStyle>(localCorner);
    }

    // Default Idle Display Style
    const int localIdleMode = Wh_GetIntValue(L"IdleDisplayModeOverride", -1);
    next.idleDisplayMode = localIdleMode >= 0 ? localIdleMode : Wh_GetIntSetting(L"Appearance.IdleDisplayMode");

    // Idle Typography & Clock Customization
    const int idleScaleSetting = Wh_GetIntSetting(L"Appearance.IdleTextScale");
    next.idleTextScale = idleScaleSetting > 0 ? Clamp(idleScaleSetting / 100.0f, 0.5f, 2.5f) : 1.0f;
    const int localSecOverride = Wh_GetIntValue(L"IdleClockSecondsOverride", -1);
    next.idleClockShowSeconds = localSecOverride >= 0 ? (localSecOverride != 0) : (Wh_GetIntSetting(L"Appearance.IdleClockShowSeconds") != 0);
    const std::wstring timeFmt = GetStringSettingCopy(L"Appearance.IdleClockTimeFormat");
    if (timeFmt == L"24h") next.idleClockTimeFormat = 1;
    else if (timeFmt == L"12h") next.idleClockTimeFormat = 2;
    else next.idleClockTimeFormat = 0;
    next.idleClockBold = Wh_GetIntSetting(L"Appearance.IdleClockBold") != 0;

    // Full Screen Auto-Hide
    next.fullScreenDetection = Wh_GetIntSetting(L"Appearance.FullScreenDetection") != 0;

    // Dimensions
    const int colW = Wh_GetIntSetting(L"Appearance.CollapsedWidth");
    next.collapsedWidth = colW >= 60 ? static_cast<float>(colW) : 200.0f;
    const int colH = Wh_GetIntSetting(L"Appearance.CollapsedHeight");
    next.collapsedHeight = colH >= 20 ? static_cast<float>(colH) : 38.0f;
    const int expW = Wh_GetIntSetting(L"Appearance.ExpandedWidth");
    next.expandedWidth = expW >= 200 ? static_cast<float>(expW) : 480.0f;
    const int expH = Wh_GetIntSetting(L"Appearance.ExpandedHeight");
    next.expandedHeight = expH >= 80 ? static_cast<float>(expH) : 190.0f;

    // Auto-Hide & Interactions
    const std::wstring hideSec = GetStringSettingCopy(L"Appearance.AutoHideIdleSeconds");
    next.autoHideIdleSeconds = hideSec.empty() ? 0 : _wtoi(hideSec.c_str());
    next.unhideOnHover = Wh_GetIntSetting(L"Appearance.UnhideOnHover") != 0;
    next.alwaysOnTop = Wh_GetIntSetting(L"Appearance.AlwaysOnTop") != 0;
    const int localExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1);
    next.expandOnHover = localExpandOnHover >= 0 ? (localExpandOnHover != 0) : (Wh_GetIntSetting(L"Appearance.ExpandOnHover") != 0);
    next.autoDpiScale = Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0;
    next.clickThroughIdle = Wh_GetIntSetting(L"Appearance.ClickThroughIdle") != 0;

    // Optics & Liquid Glass
    const int localLiquidGlass = Wh_GetIntValue(L"LiquidGlassOverride", -1);
    next.liquidGlass = localLiquidGlass >= 0 ? (localLiquidGlass != 0) : (Wh_GetIntSetting(L"Optics.LiquidGlass") != 0 || Wh_GetIntSetting(L"Themes.LiquidGlass") != 0);
    const int glassIntensitySetting = Wh_GetIntSetting(L"Optics.LiquidGlassIntensity");
    const int localGlassIntensity = Wh_GetIntValue(L"LiquidGlassIntensityOverride", -1);
    next.liquidGlassIntensity = Clamp((localGlassIntensity >= 0 ? localGlassIntensity : (glassIntensitySetting ? glassIntensitySetting : 80)) / 100.0f, 0.0f, 1.0f);
    
    next.glassSheen = Wh_GetIntSetting(L"Optics.GlassSheen") != 0;
    next.glassCaustics = Wh_GetIntSetting(L"Optics.GlassCaustics") != 0;
    next.glassSpecular = Wh_GetIntSetting(L"Optics.GlassSpecular") != 0;
    next.glassRefractionRim = Wh_GetIntSetting(L"Optics.GlassRefractionRim") != 0;

    const int localAccentGlow = Wh_GetIntValue(L"AccentGlowOverride", -1);
    next.accentGlow = localAccentGlow >= 0 ? (localAccentGlow != 0) : (Wh_GetIntSetting(L"Optics.AccentGlow") != 0 || Wh_GetIntSetting(L"Themes.AccentGlow") != 0);
    const int glowIntensitySetting = Wh_GetIntSetting(L"Optics.AccentGlowIntensity");
    const int glowRadiusOverride = Wh_GetIntValue(L"AccentGlowRadiusOverride", -1);
    next.accentGlowIntensity = Clamp((glowIntensitySetting ? glowIntensitySetting : 80) / 100.0f, 0.0f, 1.0f);
    const int glowRadSetting = glowRadiusOverride >= 5 ? glowRadiusOverride : Wh_GetIntSetting(L"Optics.AccentGlowRadius");
    next.accentGlowRadius = glowRadSetting >= 5 ? static_cast<float>(glowRadSetting) : 28.0f;

    next.softShadow = Wh_GetIntSetting(L"Optics.SoftShadow") != 0;
    const int shadowSoft = Wh_GetIntSetting(L"Optics.ShadowSoftness");
    next.shadowSoftness = Clamp((shadowSoft ? shadowSoft : 70) / 100.0f, 0.0f, 1.0f);

    // Living Assistant & Wellness
    next.enableWellnessReminders = Wh_GetIntSetting(L"Assistant.EnableWellnessReminders") != 0;

    // Hydration
    const std::wstring hydPreset = GetStringSettingCopy(L"Assistant.HydrationPreset");
    const int localHydOverride = Wh_GetIntValue(L"HydrationIntervalOverride", -1);
    if (localHydOverride >= 0) {
        next.hydrationIntervalMinutes = localHydOverride;
    } else if (!hydPreset.empty() && hydPreset != L"custom") {
        next.hydrationIntervalMinutes = _wtoi(hydPreset.c_str());
    } else {
        const int hydrInt = Wh_GetIntSetting(L"Assistant.HydrationIntervalMinutes");
        next.hydrationIntervalMinutes = hydrInt > 0 ? hydrInt : 45;
    }

    // Eye rest
    const int localEyeOverride = Wh_GetIntValue(L"EyeRestIntervalOverride", -1);
    if (localEyeOverride >= 0) {
        next.eyeRestReminder = localEyeOverride > 0;
        next.eyeRestMinutes = localEyeOverride > 0 ? localEyeOverride : 20;
    } else {
        next.eyeRestReminder = Wh_GetIntSetting(L"Assistant.EyeRestReminder") != 0;
        const int eyeMin = Wh_GetIntSetting(L"Assistant.EyeRestMinutes");
        next.eyeRestMinutes = eyeMin > 0 ? eyeMin : 20;
    }

    // Posture
    const std::wstring postPreset = GetStringSettingCopy(L"Assistant.PosturePreset");
    const int localPostOverride = Wh_GetIntValue(L"PostureIntervalOverride", -1);
    if (localPostOverride >= 0) {
        next.postureReminder = localPostOverride > 0;
        next.postureIntervalMinutes = localPostOverride > 0 ? localPostOverride : 60;
    } else {
        next.postureReminder = Wh_GetIntSetting(L"Assistant.PostureReminder") != 0;
        if (!postPreset.empty() && postPreset != L"custom") {
            next.postureIntervalMinutes = _wtoi(postPreset.c_str());
        } else {
            const int postInt = Wh_GetIntSetting(L"Assistant.PostureIntervalMinutes");
            next.postureIntervalMinutes = postInt > 0 ? postInt : 60;
        }
    }

    // Uptime Alert
    const int localUptimeOverride = Wh_GetIntValue(L"UptimeAlertOverride", -1);
    if (localUptimeOverride >= 0) {
        next.uptimeAlertHours = localUptimeOverride;
    } else {
        const int upAlert = Wh_GetIntSetting(L"Assistant.UptimeAlertHours");
        next.uptimeAlertHours = upAlert >= 0 ? upAlert : 2;
    }

    // Productivity & Tools
    const int pomWork = Wh_GetIntSetting(L"Tools.PomodoroWorkMinutes");
    next.pomodoroWorkMinutes = pomWork >= 5 ? pomWork : 25;
    const int pomBreak = Wh_GetIntSetting(L"Tools.PomodoroBreakMinutes");
    next.pomodoroBreakMinutes = pomBreak >= 1 ? pomBreak : 5;
    next.voiceRecordingFolder = GetStringSettingCopy(L"Tools.VoiceRecordingFolder");

    // Accent Colors & Modes
    const std::wstring accentMode = GetStringSettingCopy(L"Themes.AccentColorMode");
    if (EqualsNoCase(accentMode, L"system")) next.accentMode = AccentMode::System;
    else if (EqualsNoCase(accentMode, L"custom")) next.accentMode = AccentMode::Custom;
    else next.accentMode = AccentMode::Auto;
    
    next.customAccent = ColorFromHex(GetStringSettingCopy(L"Themes.CustomAccentHex"), next.customAccent);

    // Base Colors & Opacities
    next.tintOpacity = Clamp((Wh_GetIntSetting(L"Themes.TintIntensity") ? Wh_GetIntSetting(L"Themes.TintIntensity") : 72) / 100.0f, 0.0f, 1.0f);
    const int settingOpacity = Wh_GetIntSetting(L"Themes.PillOpacity");
    const int localOpacity = Wh_GetIntValue(L"PillOpacityOverride", -1);
    next.pillOpacity = Clamp((localOpacity >= 0 ? localOpacity : (settingOpacity ? settingOpacity : 96)) / 100.0f, 0.10f, 1.0f);

    // Curated color theme presets
    struct ThemeColors { const wchar_t* bg; const wchar_t* fg; const wchar_t* sec; };
    static constexpr ThemeColors kThemes[] = {
        {L"#0D0D0F", L"#F7F7F7", L"#888888"},  // 0: OLED Black (default)
        {L"#1C1C1E", L"#FFFFFF", L"#999999"},  // 1: Dark Gray
        {L"#050D1A", L"#E8F0FF", L"#7090BB"},  // 2: Midnight Blue
        {L"#0E0014", L"#F0E8FF", L"#9060BB"},  // 3: Deep Purple
        {L"#1F1F1F", L"#FFFFFF", L"#A0A0A0"},  // 4: Fluent Design
        {L"#080808", L"#FFFFFF", L"#8E8E93"},  // 5: Liquid Black
        {L"#24272C", L"#FFFFFF", L"#A2ACB8"},  // 6: Liquid Silver
        {L"#050518", L"#E0E6FF", L"#7B8AB8"},  // 7: Liquid Night
        {L"#040F18", L"#E0FFF8", L"#62B8A8"},  // 8: Liquid Aurora
        {L"#000000", L"#FFFFFF", L"#CCCCCC"},  // 9: Pure Glass / Crystal Clear
    };
    const int theme = Wh_GetIntValue(L"ColorTheme", -1);
    if (theme >= 0 && theme < static_cast<int>(ARRAYSIZE(kThemes))) {
        next.pillBgColor = ColorFromHex(kThemes[theme].bg, D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f));
        next.textPrimaryColor = ColorFromHex(kThemes[theme].fg, D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f));
        next.textSecondaryColor = ColorFromHex(kThemes[theme].sec, D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f));
    } else {
        next.pillBgColor = ColorFromHex(GetStringSettingCopy(L"Themes.PillBgColor"), D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f));
        next.textPrimaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextPrimaryColor"), D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f));
        next.textSecondaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextSecondaryColor"), D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f));
    }

    // Animation & Physics
    const std::wstring speed = GetStringSettingCopy(L"Performance.AnimationSpeed");
    if (EqualsNoCase(speed, L"slow")) next.animationSpeed = 0.65f;
    else if (EqualsNoCase(speed, L"fast")) next.animationSpeed = 1.35f;
    else if (EqualsNoCase(speed, L"instant")) next.animationSpeed = 5.0f;
    else next.animationSpeed = 1.0f;

    const std::wstring springStr = GetStringSettingCopy(L"Performance.SpringStyle");
    if (EqualsNoCase(springStr, L"smooth")) next.springStyle = SpringStyle::Smooth;
    else if (EqualsNoCase(springStr, L"snappy")) next.springStyle = SpringStyle::Snappy;
    else if (EqualsNoCase(springStr, L"stiff")) next.springStyle = SpringStyle::Stiff;
    else next.springStyle = SpringStyle::Bouncy;

    const std::wstring fpsStr = GetStringSettingCopy(L"Performance.FrameRateTarget");
    next.targetFps = fpsStr.empty() ? 60 : _wtoi(fpsStr.c_str());
    if (next.targetFps < 15 || next.targetFps > 360) next.targetFps = 60;
    next.lowPowerMode = Wh_GetIntSetting(L"Performance.LowPowerMode") != 0;

    // Modules
    next.notificationAutoExpand = Wh_GetIntSetting(L"Modules.NotificationAutoExpand") != 0;
    next.media = Wh_GetIntSetting(L"Modules.Media") != 0;
    next.notification = Wh_GetIntSetting(L"Modules.Notification") != 0;
    next.clipboard = Wh_GetIntSetting(L"Modules.Clipboard") != 0;
    next.battery = Wh_GetIntSetting(L"Modules.Battery") != 0;
    next.progress = Wh_GetIntSetting(L"Modules.Progress") != 0;
    next.volume = Wh_GetIntSetting(L"Modules.Volume") != 0;
    next.capsLock = Wh_GetIntSetting(L"Modules.CapsLock") != 0;
    next.device = Wh_GetIntSetting(L"Modules.Device") != 0;
    next.gameOverlay = Wh_GetIntSetting(L"Modules.GameOverlay") != 0;
    next.showMetricText = Wh_GetIntSetting(L"Modules.ShowMetricText") != 0;
    const int localShowMetricsInIdle = Wh_GetIntValue(L"ShowMetricsInIdleOverride", -1);
    next.showMetricsInIdle = localShowMetricsInIdle >= 0 ? (localShowMetricsInIdle != 0) : (Wh_GetIntSetting(L"Modules.ShowMetricsInIdle") != 0);
    next.weatherCity = GetStringSettingCopy(L"Modules.WeatherCity");
    next.weatherFahrenheit = Wh_GetIntSetting(L"Modules.WeatherFahrenheit") != 0;

    const std::wstring notifDur = GetStringSettingCopy(L"Modules.NotificationDuration");
    if (!notifDur.empty()) {
        float d = wcstof(notifDur.c_str(), nullptr);
        if (d > 0.5f && d < 30.0f) next.notificationDuration = d;
    }
    const std::wstring clipDur = GetStringSettingCopy(L"Modules.ClipboardDuration");
    if (!clipDur.empty()) {
        float d = wcstof(clipDur.c_str(), nullptr);
        if (d > 0.5f && d < 30.0f) next.clipboardDuration = d;
    }
    const std::wstring volDur = GetStringSettingCopy(L"Modules.VolumeDuration");
    if (!volDur.empty()) {
        float d = wcstof(volDur.c_str(), nullptr);
        if (d > 0.5f && d < 30.0f) next.volumeDuration = d;
    }
    const std::wstring capsDur = GetStringSettingCopy(L"Modules.CapsLockDuration");
    if (!capsDur.empty()) {
        float d = wcstof(capsDur.c_str(), nullptr);
        if (d > 0.5f && d < 30.0f) next.capsLockDuration = d;
    }
    const std::wstring devDur = GetStringSettingCopy(L"Modules.DeviceDuration");
    if (!devDur.empty()) {
        float d = wcstof(devDur.c_str(), nullptr);
        if (d > 0.5f && d < 30.0f) next.deviceDuration = d;
    }

    // Fine Tuning & Advanced
    {
        int v = Wh_GetIntSetting(L"FineTuning.SplitGap");
        next.splitGap = Clamp(v ? static_cast<float>(v) : 12.0f, 4.0f, 32.0f);
        int hs = Wh_GetIntSetting(L"FineTuning.HoverScalePercent");
        next.hoverScalePercent = Clamp((hs ? hs : 102) / 100.0f, 1.0f, 1.15f);
        int nudge = Wh_GetIntSetting(L"FineTuning.NudgeOffsetPx");
        next.nudgeOffsetPx = Clamp(nudge >= 0 ? static_cast<float>(nudge) : 8.0f, 0.0f, 20.0f);
        int wbc = Wh_GetIntSetting(L"FineTuning.WaveformBarCount");
        next.waveformBarCount = ClampInt(wbc ? wbc : 22, 8, 32);
        std::wstring wGap = GetStringSettingCopy(L"FineTuning.WaveformGapPx");
        if (!wGap.empty()) {
            float f = wcstof(wGap.c_str(), nullptr);
            if (f >= 1.0f && f <= 6.0f) next.waveformGapPx = f;
        }
        std::wstring prt = GetStringSettingCopy(L"FineTuning.ProgressRingThickness");
        if (!prt.empty()) {
            float f = wcstof(prt.c_str(), nullptr);
            if (f >= 1.0f && f <= 6.0f) next.progressRingThickness = f;
        }
        std::wstring pds = GetStringSettingCopy(L"FineTuning.PaginationDotSize");
        if (!pds.empty()) {
            float f = wcstof(pds.c_str(), nullptr);
            if (f >= 1.0f && f <= 6.0f) next.paginationDotSize = f;
        }
        std::wstring priv = GetStringSettingCopy(L"FineTuning.PrivacyDotSize");
        if (!priv.empty()) {
            float f = wcstof(priv.c_str(), nullptr);
            if (f >= 2.0f && f <= 8.0f) next.privacyDotSize = f;
        }
        int tho = Wh_GetIntSetting(L"FineTuning.TopHighlightOpacity");
        next.topHighlightOpacity = Clamp((tho ? tho : 35) / 100.0f, 0.0f, 1.0f);
        std::wstring rimT = GetStringSettingCopy(L"FineTuning.RimThickness");
        if (!rimT.empty()) {
            float f = wcstof(rimT.c_str(), nullptr);
            if (f >= 0.2f && f <= 3.0f) next.rimThickness = f;
        }
        int ss = Wh_GetIntSetting(L"FineTuning.ShadowSpreadPx");
        next.shadowSpreadPx = Clamp(ss >= 0 ? static_cast<float>(ss) : 10.0f, 0.0f, 24.0f);
        int sy = Wh_GetIntSetting(L"FineTuning.ShadowYOffsetPx");
        next.shadowYOffsetPx = Clamp(sy >= 0 ? static_cast<float>(sy) : 4.0f, 0.0f, 12.0f);
        std::wstring cs = GetStringSettingCopy(L"FineTuning.CausticsSpeed");
        if (!cs.empty()) {
            float f = wcstof(cs.c_str(), nullptr);
            if (f >= 0.0f && f <= 5.0f) next.causticsSpeed = f;
        }
        {
            std::wstring cf = GetStringSettingCopy(L"FineTuning.ContentFadeOnResize");
            if (cf.empty()) next.contentFadeOnResize = true;
            else next.contentFadeOnResize = Wh_GetIntSetting(L"FineTuning.ContentFadeOnResize") != 0;
        }
        next.useDwmBlurBehind = Wh_GetIntSetting(L"FineTuning.UseDwmBlurBehind") != 0;
        int wum = Wh_GetIntSetting(L"FineTuning.WeatherUpdateMinutes");
        next.weatherUpdateMinutes = ClampInt(wum ? wum : 10, 5, 60);
    }

    bool cityChanged = false;
    {
        std::lock_guard lock(g_settingsMutex);
        cityChanged = (next.weatherCity != g_settings.weatherCity);
        g_settings = next;
    }
    g_layoutDirty = true;
    if (cityChanged && g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
}

void EnableBlurBehind(HWND hwnd) {
    Settings s = GetSettingsSnapshot();
    DWM_BLURBEHIND blur = {};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = s.useDwmBlurBehind ? TRUE : FALSE;
    if (s.useDwmBlurBehind) {
        blur.hRgnBlur = nullptr;
        blur.dwFlags |= DWM_BB_BLURREGION;
    }
    DwmEnableBlurBehindWindow(hwnd, &blur);
}

struct MonitorEnumData {
    std::vector<HMONITOR> monitors;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) {
    auto* data = reinterpret_cast<MonitorEnumData*>(dwData);
    data->monitors.push_back(hMonitor);
    return TRUE;
}

HMONITOR GetAnchorMonitor() {
    HMONITOR selectedMonitor = nullptr;
    Settings settings = GetSettingsSnapshot();

    if (settings.targetMonitor == -1) {
        POINT pt = {0, 0};
        GetCursorPos(&pt);
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    } else if (settings.targetMonitor > 0) {
        MonitorEnumData data;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&data));
        
        int index = settings.targetMonitor - 1;
        if (index >= 0 && index < static_cast<int>(data.monitors.size())) {
            selectedMonitor = data.monitors[index];
        }
    }

    if (!selectedMonitor) {
        POINT pt = {0, 0};
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }
    return selectedMonitor;
}

RECT GetAnchorWorkRect() {
    HMONITOR selectedMonitor = GetAnchorMonitor();
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(selectedMonitor, &mi);
    return mi.rcWork;
}

static int s_lastOverlayX = INT_MIN, s_lastOverlayY = INT_MIN, s_lastOverlayW = INT_MIN, s_lastOverlayH = INT_MIN;
static bool s_lastOverlayTopMost = false;

void ResetPositionOverlayCache() {
    s_lastOverlayX = INT_MIN;
    s_lastOverlayY = INT_MIN;
    s_lastOverlayW = INT_MIN;
    s_lastOverlayH = INT_MIN;
    s_lastOverlayTopMost = false;
}

POINT ComputeOverlayPosition(int width, int height, const Settings& settings) {
    RECT work = GetAnchorWorkRect();
    const int margin = settings.edgeMargin;
    int x = work.left + (work.right - work.left - width) / 2;
    int y = work.top + margin;

    switch (settings.position) {
        case Position::TopLeft:
            x = work.left + margin;
            y = work.top + margin;
            break;
        case Position::TopRight:
            x = work.right - width - margin;
            y = work.top + margin;
            break;
        case Position::BottomCenter:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.bottom - height - margin;
            break;
        case Position::BottomLeft:
            x = work.left + margin;
            y = work.bottom - height - margin;
            break;
        case Position::BottomRight:
            x = work.right - width - margin;
            y = work.bottom - height - margin;
            break;
        case Position::Center:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.top + (work.bottom - work.top - height) / 2;
            break;
        case Position::TopCenter:
        default:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.top + margin;
            break;
    }

    x += settings.offsetX;
    y += settings.offsetY;
    return POINT{x, y};
}

void PositionOverlayWindow(HWND hwnd, int width, int height, bool force = false) {
    if (!hwnd || !IsWindow(hwnd)) return;
    Settings settings = GetSettingsSnapshot();
    POINT pt = ComputeOverlayPosition(width, height, settings);
    int x = pt.x;
    int y = pt.y;

    bool topMost = settings.alwaysOnTop;
    if (!force && x == s_lastOverlayX && y == s_lastOverlayY && width == s_lastOverlayW && height == s_lastOverlayH && topMost == s_lastOverlayTopMost) {
        return;
    }
    s_lastOverlayX = x;
    s_lastOverlayY = y;
    s_lastOverlayW = width;
    s_lastOverlayH = height;
    s_lastOverlayTopMost = topMost;

    SetWindowPos(hwnd, topMost ? HWND_TOPMOST : HWND_NOTOPMOST,
                 x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSENDCHANGING);
}

bool CheckIsFullScreenActive(HMONITOR targetMonitor) {
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_NOT_PRESENT;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
        if (quns == QUNS_RUNNING_D3D_FULL_SCREEN || quns == QUNS_PRESENTATION_MODE || quns == QUNS_BUSY) {
            return true;
        }
    }

    HWND fg = GetForegroundWindow();
    if (fg && fg != g_hwnd.load(std::memory_order_relaxed) && IsWindow(fg) && IsWindowVisible(fg) && !IsIconic(fg)) {
        wchar_t cls[128] = {};
        GetClassNameW(fg, cls, ARRAYSIZE(cls));
        if (_wcsicmp(cls, L"Progman") != 0 && _wcsicmp(cls, L"WorkerW") != 0 &&
            _wcsicmp(cls, L"Shell_TrayWnd") != 0 && _wcsicmp(cls, L"Shell_SecondaryTrayWnd") != 0 &&
            _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") != 0) {
            
            HMONITOR fgMon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
            if (targetMonitor == nullptr || fgMon == targetMonitor) {
                RECT rcFg;
                if (GetWindowRect(fg, &rcFg)) {
                    MONITORINFO mi = {sizeof(mi)};
                    if (GetMonitorInfoW(fgMon, &mi)) {
                        if (rcFg.left <= mi.rcMonitor.left && rcFg.top <= mi.rcMonitor.top &&
                            rcFg.right >= mi.rcMonitor.right && rcFg.bottom >= mi.rcMonitor.bottom) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

std::wstring GetNotesFilePath() {
    wchar_t appData[MAX_PATH] = {};
    if (GetEnvironmentVariableW(L"APPDATA", appData, ARRAYSIZE(appData)) > 0) {
        std::wstring dir = std::wstring(appData) + L"\\aegisCapsule";
        CreateDirectoryW(dir.c_str(), nullptr);
        return dir + L"\\notes.txt";
    }
    return L"C:\\aegisCapsule_notes.txt";
}

void LoadQuickNotes() {
    std::wstring path = GetNotesFilePath();
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD size = GetFileSize(hFile, nullptr);
        if (size > 0 && size < 1024 * 1024) {
            std::vector<char> buf(size + 1, 0);
            DWORD read = 0;
            ReadFile(hFile, buf.data(), size, &read, nullptr);
            std::string content(buf.data(), read);

            // Skip UTF-8 BOM if present
            if (content.size() >= 3 &&
                static_cast<unsigned char>(content[0]) == 0xEF &&
                static_cast<unsigned char>(content[1]) == 0xBB &&
                static_cast<unsigned char>(content[2]) == 0xBF) {
                content.erase(0, 3);
            }

            std::vector<std::wstring> notes;
            std::istringstream stream(content);
            std::string line;
            while (std::getline(stream, line)) {
                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (!line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos) {
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, line.data(), static_cast<int>(line.size()), nullptr, 0);
                    if (wlen > 0) {
                        std::wstring wline(wlen, 0);
                        MultiByteToWideChar(CP_UTF8, 0, line.data(), static_cast<int>(line.size()), &wline[0], wlen);
                        notes.push_back(wline);
                    }
                }
            }
            std::lock_guard lock(g_stateMutex);
            g_state.quickNotes = std::move(notes);
        }
        CloseHandle(hFile);
    }
}

void SaveQuickNote(const std::wstring& note) {
    if (note.empty()) return;
    std::wstring formattedNote = note;
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t timeBuf[32] = {};
    swprintf_s(timeBuf, L"[%02d:%02d] ", st.wHour, st.wMinute);
    if (formattedNote.find(L"[") != 0) {
        formattedNote = timeBuf + formattedNote;
    }

    std::wstring path = GetNotesFilePath();
    HANDLE hFile = CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        int u8len = WideCharToMultiByte(CP_UTF8, 0, formattedNote.c_str(), static_cast<int>(formattedNote.size()), nullptr, 0, nullptr, nullptr);
        if (u8len > 0) {
            std::string utf8(u8len, 0);
            WideCharToMultiByte(CP_UTF8, 0, formattedNote.c_str(), static_cast<int>(formattedNote.size()), &utf8[0], u8len, nullptr, nullptr);
            utf8 += "\r\n";
            DWORD written = 0;
            WriteFile(hFile, utf8.data(), static_cast<DWORD>(utf8.size()), &written, nullptr);
        }
        CloseHandle(hFile);
    }
    LoadQuickNotes();
}

void TriggerCustomAlert(const std::wstring& app, const std::wstring& title, const std::wstring& body, double duration) {
    NotificationSnapshot snap;
    snap.active = true;
    snap.app = app;
    snap.title = title;
    snap.body = body;
    snap.expiresAt = NowSeconds() + duration;
    {
        std::lock_guard lock(g_stateMutex);
        g_state.notification = snap;
    }
    TriggerNudge();
}

std::wstring GetRecordingsDirectory() {
    Settings s = GetSettingsSnapshot();
    if (!s.voiceRecordingFolder.empty()) {
        CreateDirectoryW(s.voiceRecordingFolder.c_str(), nullptr);
        return s.voiceRecordingFolder;
    }
    wchar_t userProfile[MAX_PATH] = {};
    if (GetEnvironmentVariableW(L"USERPROFILE", userProfile, ARRAYSIZE(userProfile)) > 0) {
        std::wstring dir = std::wstring(userProfile) + L"\\Desktop\\aegisCapsule_Recordings";
        CreateDirectoryW(dir.c_str(), nullptr);
        return dir;
    }
    return L"C:\\aegisCapsule_Recordings";
}

DWORD WINAPI RecordingThreadProc(LPVOID) {
    try { winrt::init_apartment(); } catch (...) {}

    ComPtr<IMMDeviceEnumerator> enumerator;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (!enumerator) return 1;

    ComPtr<IMMDevice> device;
    enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device);
    if (!device) return 1;

    ComPtr<IAudioClient> audioClient;
    device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient);
    if (!audioClient) return 1;

    WAVEFORMATEX* pwfx = nullptr;
    audioClient->GetMixFormat(&pwfx);
    if (!pwfx) return 1;

    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t filename[128] = {};
    swprintf_s(filename, L"VoiceNote_%04d%02d%02d_%02d%02d%02d.wav", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    std::wstring fullPath = GetRecordingsDirectory() + L"\\" + filename;

    HANDLE hFile = CreateFileW(fullPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        CoTaskMemFree(pwfx);
        return 1;
    }

    BYTE wavHeader[44] = {};
    memcpy(&wavHeader[0], "RIFF", 4);
    memcpy(&wavHeader[8], "WAVE", 4);
    memcpy(&wavHeader[12], "fmt ", 4);
    DWORD subchunk1Size = 16;
    WORD audioFormat = 1;
    WORD numChannels = pwfx->nChannels;
    DWORD sampleRate = pwfx->nSamplesPerSec;
    WORD bitsPerSample = 16;
    WORD blockAlign = numChannels * (bitsPerSample / 8);
    DWORD byteRate = sampleRate * blockAlign;

    memcpy(&wavHeader[16], &subchunk1Size, 4);
    memcpy(&wavHeader[20], &audioFormat, 2);
    memcpy(&wavHeader[22], &numChannels, 2);
    memcpy(&wavHeader[24], &sampleRate, 4);
    memcpy(&wavHeader[28], &byteRate, 4);
    memcpy(&wavHeader[32], &blockAlign, 2);
    memcpy(&wavHeader[34], &bitsPerSample, 2);
    memcpy(&wavHeader[36], "data", 4);

    DWORD bytesWritten = 0;
    WriteFile(hFile, wavHeader, sizeof(wavHeader), &bytesWritten, nullptr);

    HRESULT hrInit = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 1000000, 0, pwfx, nullptr);
    if (FAILED(hrInit)) {
        CloseHandle(hFile);
        CoTaskMemFree(pwfx);
        return 1;
    }
    ComPtr<IAudioCaptureClient> captureClient;
    HRESULT hrService = audioClient->GetService(IID_PPV_ARGS(&captureClient));
    if (FAILED(hrService) || !captureClient) {
        CloseHandle(hFile);
        CoTaskMemFree(pwfx);
        return 1;
    }
    HRESULT hrStart = audioClient->Start();
    if (FAILED(hrStart)) {
        CloseHandle(hFile);
        CoTaskMemFree(pwfx);
        return 1;
    }

    DWORD totalPcmBytes = 0;
    double startTime = NowSeconds();

    {
        std::lock_guard lock(g_stateMutex);
        g_state.recording.active = true;
        g_state.recording.startedAt = startTime;
        g_state.recording.filePath = fullPath;
    }

    while (g_isRecording && g_running) {
        double elapsedRec = NowSeconds() - startTime;
        if (elapsedRec >= 3600.0) break; // 60 min max auto-stop

        UINT32 nextPacketSize = 0;
        HRESULT hr = captureClient->GetNextPacketSize(&nextPacketSize);
        if (SUCCEEDED(hr) && nextPacketSize > 0) {
            BYTE* data = nullptr;
            UINT32 numFramesRead = 0;
            DWORD flags = 0;
            hr = captureClient->GetBuffer(&data, &numFramesRead, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr)) {
                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                    size_t samples = static_cast<size_t>(numFramesRead) * pwfx->nChannels;
                    std::vector<int16_t> pcm(samples);
                    if (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
                        (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE && pwfx->cbSize >= 22 &&
                         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat, kSubTypeIeeeFloat))) {
                        float* f = reinterpret_cast<float*>(data);
                        for (size_t i = 0; i < samples; ++i) {
                            float s = Clamp(f[i], -1.0f, 1.0f);
                            pcm[i] = static_cast<int16_t>(s * 32767.0f);
                        }
                    } else {
                        memcpy(pcm.data(), data, samples * sizeof(int16_t));
                    }

                    DWORD pcmBytes = static_cast<DWORD>(pcm.size() * sizeof(int16_t));
                    WriteFile(hFile, pcm.data(), pcmBytes, &bytesWritten, nullptr);
                    totalPcmBytes += bytesWritten;

                    PushAudioChunks(data, numFramesRead, pwfx);
                }
                captureClient->ReleaseBuffer(numFramesRead);
            }
        }

        {
            std::lock_guard lock(g_stateMutex);
            g_state.recording.seconds = static_cast<int>(elapsedRec);
        }

        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 15) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(15);
    }

    audioClient->Stop();

    DWORD chunkSize = 36 + totalPcmBytes;
    SetFilePointer(hFile, 4, nullptr, FILE_BEGIN);
    WriteFile(hFile, &chunkSize, 4, &bytesWritten, nullptr);
    SetFilePointer(hFile, 40, nullptr, FILE_BEGIN);
    WriteFile(hFile, &totalPcmBytes, 4, &bytesWritten, nullptr);
    CloseHandle(hFile);

    if (pwfx) CoTaskMemFree(pwfx);

    {
        std::lock_guard lock(g_stateMutex);
        g_state.recording.active = false;
    }

    TriggerCustomAlert(L"Voice Recorder", L"Voice Note Saved 🎙️", filename, 5.0);
    return 0;
}

void StartVoiceRecording() {
    std::lock_guard lock(g_recordingMutex);
    if (!g_isRecording) {
        if (g_recordingThread) {
            WaitForSingleObject(g_recordingThread, 1000);
            CloseHandle(g_recordingThread);
            g_recordingThread = nullptr;
        }
        g_isRecording = true;
        g_recordingThread = CreateThread(nullptr, 0, RecordingThreadProc, nullptr, 0, nullptr);
    }
}

void StopVoiceRecording() {
    // Extract thread handle under lock, then join outside lock to avoid deadlock.
    // RecordingThreadProc must not hold g_recordingMutex — joining with mutex held
    // would deadlock if the thread tried to re-acquire it on cleanup.
    HANDLE threadToJoin = nullptr;
    {
        std::lock_guard lock(g_recordingMutex);
        if (!g_isRecording) return;
        g_isRecording = false;
        threadToJoin = g_recordingThread;
        g_recordingThread = nullptr;
    }
    if (threadToJoin) {
        WaitForSingleObject(threadToJoin, 2000);
        CloseHandle(threadToJoin);
    }
}

// Activity Prioritization
std::vector<IslandKind> ChooseActivities(const SharedState& state, const Settings& settings,
                                         double now) {
    std::vector<IslandKind> chosen;

    if (state.recording.active) {
        chosen.push_back(IslandKind::Recording);
        return chosen;
    }

    if (settings.device && state.device.active && state.device.expiresAt > now) {
        chosen.push_back(IslandKind::Device);
        return chosen;
    }

    if (settings.capsLock && state.capsLock.active && state.capsLock.expiresAt > now) {
        chosen.push_back(IslandKind::CapsLock);
        return chosen;
    }

    if (settings.volume && state.volume.active && state.volume.expiresAt > now) {
        chosen.push_back(IslandKind::Volume);
        return chosen;
    }

    if (settings.battery && state.battery.active && state.battery.expiresAt > now) {
        chosen.push_back(IslandKind::BatteryLow);
        return chosen;
    }

    if (settings.notification && state.notification.active && state.notification.expiresAt > now) {
        chosen.push_back(IslandKind::Notification);
        return chosen;
    }

    if (settings.clipboard && state.clipboard.active && state.clipboard.expiresAt > now) {
        chosen.push_back(IslandKind::Clipboard);
        return chosen;
    }

    if (settings.media && state.media.available) {
        chosen.push_back(IslandKind::Media);
    }

    if (settings.progress && state.progress.active) {
        chosen.push_back(IslandKind::Progress);
    }

    if (chosen.empty()) {
        chosen.push_back(IslandKind::Idle);
    }

    return chosen;
}

bool DecodeImageBytesToPixels(const std::vector<uint8_t>& bytes, BitmapPixels* outPixels) {
    if (!outPixels || bytes.empty()) {
        return false;
    }

    ComPtr<IWICImagingFactory> factory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (FAILED(hr)) return false;

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) return false;

    void* locked = GlobalLock(mem);
    memcpy(locked, bytes.data(), bytes.size());
    GlobalUnlock(mem);

    ComPtr<IStream> stream;
    hr = CreateStreamOnHGlobal(mem, TRUE, &stream);
    if (FAILED(hr)) {
        GlobalFree(mem);
        return false;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return false;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return false;

    ComPtr<IWICFormatConverter> converter;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return false;

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
                               WICBitmapDitherTypeNone, nullptr, 0.0,
                               WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return false;

    UINT width = 0, height = 0;
    converter->GetSize(&width, &height);
    if (!width || !height || width > 2048 || height > 2048) return false;

    BitmapPixels pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.bgra.resize(static_cast<size_t>(width) * height * 4);

    hr = converter->CopyPixels(nullptr, width * 4,
                               static_cast<UINT>(pixels.bgra.size()),
                               pixels.bgra.data());
    if (FAILED(hr)) return false;

    struct Bucket {
        uint32_t count = 0;
        uint32_t r = 0, g = 0, b = 0;
    };

    std::array<Bucket, 16 * 16 * 16> buckets{};
    for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
        const uint8_t alpha = pixels.bgra[i + 3];
        const uint8_t blue = pixels.bgra[i + 0];
        const uint8_t green = pixels.bgra[i + 1];
        const uint8_t red = pixels.bgra[i + 2];
        if (alpha < 32) continue;

        const int maxc = std::max({red, green, blue});
        const int minc = std::min({red, green, blue});
        const int luminance = (54 * red + 183 * green + 19 * blue) / 256;
        const int saturation = maxc - minc;
        if (luminance < 28 || luminance > 232 || saturation < 24) continue;

        const size_t bucketIndex = ((red >> 4) << 8) | ((green >> 4) << 4) | (blue >> 4);
        Bucket& bucket = buckets[bucketIndex];
        const uint32_t weight = 1 + static_cast<uint32_t>(saturation / 48);
        bucket.count += weight;
        bucket.r += red * weight;
        bucket.g += green * weight;
        bucket.b += blue * weight;
    }

    const Bucket* best = nullptr;
    for (const Bucket& bucket : buckets) {
        if (!best || bucket.count > best->count) {
            best = &bucket;
        }
    }

    if (best && best->count > 0) {
        pixels.sampledAccent = D2D1::ColorF(
            Clamp((best->r / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            Clamp((best->g / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            Clamp((best->b / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            1.0f);
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);
    return true;
}

bool IconToPixels(HICON icon, UINT size, BitmapPixels* outPixels) {
    if (!icon || !outPixels || !size) return false;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) return false;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(size);
    bi.bmiHeader.biHeight = -static_cast<LONG>(size);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return false;
    }

    HGDIOBJ old = SelectObject(dc, bitmap);
    RECT fill = {0, 0, static_cast<LONG>(size), static_cast<LONG>(size)};
    FillRect(dc, &fill, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
    ZeroMemory(bits, static_cast<size_t>(size) * size * 4);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);

    BitmapPixels pixels;
    pixels.width = size;
    pixels.height = size;
    pixels.bgra.resize(static_cast<size_t>(size) * size * 4);
    memcpy(pixels.bgra.data(), bits, pixels.bgra.size());

    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.bgra.size(); i += 4) {
        if (pixels.bgra[i] != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
            const bool black = pixels.bgra[i] < 4 && pixels.bgra[i + 1] < 4 && pixels.bgra[i + 2] < 4;
            pixels.bgra[i + 3] = black ? 0 : 255;
        }
    } else {
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
            const uint8_t a = pixels.bgra[i + 3];
            if (a < 255 && a > 0) {
                pixels.bgra[i + 0] = static_cast<uint8_t>(pixels.bgra[i + 0] * a / 255);
                pixels.bgra[i + 1] = static_cast<uint8_t>(pixels.bgra[i + 1] * a / 255);
                pixels.bgra[i + 2] = static_cast<uint8_t>(pixels.bgra[i + 2] * a / 255);
            }
        }
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);

    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return true;
}

bool ProcessImageNameForPid(DWORD pid, std::wstring* imageName) {
    if (!pid || !imageName) return false;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) *imageName = path;
    return ok;
}

HICON CopyWindowIcon(HWND hwnd, WPARAM iconType) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 80, &result);
    return result ? CopyIcon(reinterpret_cast<HICON>(result)) : nullptr;
}

HICON getProcessIcon(DWORD pid) {
    std::wstring path;
    if (ProcessImageNameForPid(pid, &path) && !path.empty()) {
        HICON hIcon = nullptr;
        UINT iconId = 0;
        using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
        static auto pPrivateExtractIconsW = reinterpret_cast<PrivateExtractIconsW_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));
        if (pPrivateExtractIconsW && pPrivateExtractIconsW(path.c_str(), 0, 64, 64, &hIcon, &iconId, 1, 0) == 1 && hIcon) {
            return hIcon;
        }

        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
            return sfi.hIcon;
        }

        HICON hLarge = nullptr;
        HICON hSmall = nullptr;
        if (ExtractIconExW(path.c_str(), 0, &hLarge, &hSmall, 1) > 0) {
            if (hSmall) DestroyIcon(hSmall);
            if (hLarge) return hLarge;
        }
    }

    return CopyIcon(LoadIconW(nullptr, MAKEINTRESOURCEW(32512)));
}

HICON getWindowIcon(HWND hwnd) {
    if (!hwnd) return CopyIcon(LoadIconW(nullptr, MAKEINTRESOURCEW(32512)));

    if (HICON icon = CopyWindowIcon(hwnd, ICON_BIG)) return icon;
    if (HICON icon = CopyWindowIcon(hwnd, ICON_SMALL)) return icon;

    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON))) return CopyIcon(icon);
    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM))) return CopyIcon(icon);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return getProcessIcon(pid);
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring BaseNameFromPath(std::wstring path) {
    const size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) path.erase(0, slash + 1);
    const size_t dot = path.find_last_of(L'.');
    if (dot != std::wstring::npos) path.erase(dot);
    return path;
}

std::wstring FriendlyMediaSourceName(const std::wstring& aumid) {
    if (aumid.empty()) return L"Now Playing";
    std::wstring lower = ToLowerCopy(aumid);

    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"applemusic") != std::wstring::npos || lower.find(L"apple music") != std::wstring::npos) return L"Apple Music";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Google Chrome";
    if (lower.find(L"msedge") != std::wstring::npos || lower.find(L"edge") != std::wstring::npos) return L"Microsoft Edge";
    if (lower.find(L"firefox") != std::wstring::npos) return L"Mozilla Firefox";
    if (lower.find(L"brave") != std::wstring::npos) return L"Brave";
    if (lower.find(L"opera") != std::wstring::npos) return L"Opera";
    if (lower.find(L"tidal") != std::wstring::npos) return L"TIDAL";
    if (lower.find(L"deezer") != std::wstring::npos) return L"Deezer";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC Media Player";
    if (lower.find(L"foobar") != std::wstring::npos) return L"foobar2000";
    if (lower.find(L"aimp") != std::wstring::npos) return L"AIMP";
    if (lower.find(L"wmplayer") != std::wstring::npos || lower.find(L"zune") != std::wstring::npos) return L"Windows Media Player";

    return BaseNameFromPath(aumid);
}

std::wstring MediaSourceBadge(const std::wstring& friendlyName) {
    std::wstring lower = ToLowerCopy(friendlyName);
    if (lower == L"spotify") return L"\u25b6 Spotify";
    if (lower == L"apple music") return L"\u266b Apple Music";
    if (lower == L"tidal") return L"\u25c8 TIDAL";
    if (lower == L"deezer") return L"\u2261 Deezer";
    if (lower == L"google chrome" || lower == L"microsoft edge" || lower == L"mozilla firefox" || lower == L"brave") return L"\U0001f310 Web";
    return L"\u25b6 " + friendlyName;
}

[[maybe_unused]] bool IsBrowserMediaSource(const std::wstring& aumid) {
    const std::wstring lower = ToLowerCopy(aumid);
    return lower.find(L"chrome") != std::wstring::npos ||
           lower.find(L"msedge") != std::wstring::npos ||
           lower.find(L"edge") != std::wstring::npos ||
           lower.find(L"firefox") != std::wstring::npos ||
           lower.find(L"brave") != std::wstring::npos ||
           lower.find(L"opera") != std::wstring::npos;
}

BitmapPixels GetWindowIconPixels(HWND hwnd, UINT size = 64) {
    BitmapPixels pixels;
    HICON icon = getWindowIcon(hwnd);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }
    return pixels;
}

[[maybe_unused]] BitmapPixels GetProcessIconPixels(DWORD pid, UINT size = 64) {
    BitmapPixels pixels;
    HICON icon = getProcessIcon(pid);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }
    return pixels;
}

struct AppIconSearchData {
    std::wstring targetName;
    HWND bestHwnd = nullptr;
};

BOOL CALLBACK FindAppIconWindowProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    auto* data = reinterpret_cast<AppIconSearchData*>(lParam);

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    std::wstring path;
    if (ProcessImageNameForPid(pid, &path)) {
        std::wstring base = ToLowerCopy(BaseNameFromPath(path));
        std::wstring target = ToLowerCopy(data->targetName);
        if (base.find(target) != std::wstring::npos || target.find(base) != std::wstring::npos) {
            data->bestHwnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

BitmapPixels FindAppIconByName(const std::wstring& appName, UINT size = 64) {
    BitmapPixels pixels;
    if (appName.empty()) return pixels;

    AppIconSearchData data;
    data.targetName = appName;
    EnumWindows(FindAppIconWindowProc, reinterpret_cast<LPARAM>(&data));

    if (data.bestHwnd) {
        pixels = GetWindowIconPixels(data.bestHwnd, size);
    }
    return pixels;
}

BitmapPixels FindMediaSourceIcon(const std::wstring& aumid, UINT size = 64) {
    BitmapPixels pixels;
    if (aumid.empty()) return pixels;

    std::wstring exeName = aumid;
    const size_t bang = exeName.find(L'!');
    if (bang != std::wstring::npos) exeName.erase(bang);

    pixels = FindAppIconByName(BaseNameFromPath(exeName), size);
    return pixels;
}

std::vector<uint8_t> ReadWinRtStreamBytes(
    const winrt::Windows::Storage::Streams::IRandomAccessStreamReference& reference) {
    std::vector<uint8_t> bytes;
    if (!reference) return bytes;

    try {
        auto stream = reference.OpenReadAsync().get();
        if (!stream) return bytes;

        const uint64_t size64 = stream.Size();
        if (size64 == 0 || size64 > 8 * 1024 * 1024) return bytes;

        const uint32_t size = static_cast<uint32_t>(size64);
        winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
        reader.LoadAsync(size).get();
        bytes.resize(size);
        reader.ReadBytes(winrt::array_view<uint8_t>(bytes.data(), bytes.data() + bytes.size()));
    } catch (...) {}
    return bytes;
}

void TriggerNudge() {
    const double now = NowSeconds();
    const double previous = g_lastNudgeTime.load();
    if (now - previous < 0.45) return;
    g_lastNudgeTime = now;
    HWND hwnd = g_hwnd.load(std::memory_order_relaxed);
    if (hwnd) PostMessageW(hwnd, WM_APP_NEW_EVENT, 0, 0);
}

float SampleAudioAmplitude(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format || !format->nChannels) return 0.0f;

    double sum = 0.0;
    size_t samples = static_cast<size_t>(frames) * format->nChannels;

    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX) &&
         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat, kSubTypeIeeeFloat))) {
        auto* f = reinterpret_cast<float*>(data);
        for (size_t i = 0; i < samples; ++i) sum += f[i] * f[i];
    } else if (format->wBitsPerSample == 16) {
        auto* s = reinterpret_cast<int16_t*>(data);
        for (size_t i = 0; i < samples; ++i) {
            float norm = s[i] / 32768.0f;
            sum += norm * norm;
        }
    } else {
        return 0.0f;
    }

    return static_cast<float>(std::sqrt(sum / samples));
}

void PushWaveformSample(float amplitude) {
    std::lock_guard lock(g_stateMutex);
    g_state.waveform[g_state.waveformWrite] = amplitude;
    g_state.waveformWrite = (g_state.waveformWrite + 1) % g_state.waveform.size();
}

void PushAudioChunks(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format) return;
    const UINT32 chunkSize = 480; // ~10ms chunks
    for (UINT32 offset = 0; offset < frames; offset += chunkSize) {
        UINT32 count = std::min(chunkSize, frames - offset);
        BYTE* chunkData = data + offset * format->nBlockAlign;
        float amp = SampleAudioAmplitude(chunkData, count, format);
        PushWaveformSample(amp);
    }
}

// PDH GPU queries
static PDH_HQUERY g_gpuQuery = NULL;
static PDH_HCOUNTER g_gpuCounter = NULL;

static void InitGpuQuery() {
    if (g_gpuQuery == NULL) {
        if (PdhOpenQueryW(NULL, 0, &g_gpuQuery) == ERROR_SUCCESS) {
            PdhAddEnglishCounterW(g_gpuQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_gpuCounter);
            PdhCollectQueryData(g_gpuQuery);
        }
    }
}

static int GetGpuUsage() {
    InitGpuQuery();
    if (!g_gpuQuery || !g_gpuCounter) return 0;
    
    PdhCollectQueryData(g_gpuQuery);
    
    DWORD bufferSize = 0;
    DWORD itemCount = 0;
    PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, NULL);
    
    if (bufferSize > 0) {
        std::vector<BYTE> buffer(bufferSize);
        auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
        
        if (PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items) == ERROR_SUCCESS) {
            double total = 0;
            for (DWORD i = 0; i < itemCount; i++) {
                if (items[i].szName && wcsstr(items[i].szName, L"engtype_3D")) {
                    total += items[i].FmtValue.doubleValue;
                }
            }
            return ClampInt(static_cast<int>(total), 0, 100);
        }
    }
    return 0;
}

// Privacy Registry consent status
bool IsDeviceActiveViaRegistry(const wchar_t* capability) {
    bool isActive = false;
    std::wstring basePath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\";
    basePath += capability;

    auto CheckSubkeys = [](HKEY hKeyParent) -> bool {
        DWORD index = 0;
        wchar_t subKeyName[256];
        DWORD nameLen = ARRAYSIZE(subKeyName);
        while (RegEnumKeyExW(hKeyParent, index, subKeyName, &nameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
            HKEY hSub;
            if (RegOpenKeyExW(hKeyParent, subKeyName, 0, KEY_READ, &hSub) == ERROR_SUCCESS) {
                if (_wcsicmp(subKeyName, L"NonPackaged") == 0) {
                    DWORD npIndex = 0;
                    wchar_t npSubKeyName[256];
                    DWORD npNameLen = ARRAYSIZE(npSubKeyName);
                    while (RegEnumKeyExW(hSub, npIndex, npSubKeyName, &npNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                        HKEY hNpSub;
                        if (RegOpenKeyExW(hSub, npSubKeyName, 0, KEY_READ, &hNpSub) == ERROR_SUCCESS) {
                            uint64_t stopTime = 1;
                            DWORD dataSize = sizeof(stopTime);
                            if (RegQueryValueExW(hNpSub, L"LastUsedTimeStop", nullptr, nullptr, reinterpret_cast<LPBYTE>(&stopTime), &dataSize) == ERROR_SUCCESS) {
                                if (stopTime == 0) {
                                    RegCloseKey(hNpSub);
                                    RegCloseKey(hSub);
                                    return true;
                                }
                            }
                            RegCloseKey(hNpSub);
                        }
                        npIndex++;
                        npNameLen = ARRAYSIZE(npSubKeyName);
                    }
                } else {
                    uint64_t stopTime = 1;
                    DWORD dataSize = sizeof(stopTime);
                    if (RegQueryValueExW(hSub, L"LastUsedTimeStop", nullptr, nullptr, reinterpret_cast<LPBYTE>(&stopTime), &dataSize) == ERROR_SUCCESS) {
                        if (stopTime == 0) {
                            RegCloseKey(hSub);
                            return true;
                        }
                    }
                }
                RegCloseKey(hSub);
            }
            index++;
            nameLen = ARRAYSIZE(subKeyName);
        }
        return false;
    };

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, basePath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        isActive = CheckSubkeys(hKey);
        RegCloseKey(hKey);
    }
    return isActive;
}

void UpdatePrivacyIndicators() {
    static double s_lastPrivacyCheck = 0.0;
    double now = NowSeconds();
    if (now - s_lastPrivacyCheck < 5.0) return;
    s_lastPrivacyCheck = now;

    bool mic = IsDeviceActiveViaRegistry(L"microphone");
    bool cam = IsDeviceActiveViaRegistry(L"webcam");

    std::lock_guard lock(g_stateMutex);
    g_state.system.micActive = mic;
    g_state.system.cameraActive = cam;
}

void SetSystemMasterVolume(float level) {
    level = Clamp(level, 0.0f, 1.0f);
    ComPtr<IMMDeviceEnumerator> enumerator;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator)))) {
        ComPtr<IMMDevice> device;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
            ComPtr<IAudioEndpointVolume> endpointVolume;
            if (SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, &endpointVolume))) {
                endpointVolume->SetMasterVolumeLevelScalar(level, nullptr);
                endpointVolume->SetMute(level <= 0.001f, nullptr);
                {
                    std::lock_guard lock(g_stateMutex);
                    g_state.volume.percent = static_cast<int>(level * 100.0f);
                    g_state.volume.muted = (level <= 0.001f);
                    g_state.volume.active = true;
                    g_state.volume.expiresAt = NowSeconds() + 2.5;
                }
                TriggerNudge();
            }
        }
    }
}

void SetSystemToastSuppression(bool suppress) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\PushNotifications",
                        0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        DWORD val = suppress ? 0 : 1;
        RegSetValueExW(hKey, L"ToastEnabled", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&val), sizeof(val));
        RegCloseKey(hKey);
    }
}

bool HttpGet(const std::wstring& host, const std::wstring& path, std::string& responseBody) {
    HINTERNET hSession = WinHttpOpen(L"aegisCapsule/0.1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), nullptr,
                                            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD timeout = 8000;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    bool success = false;
    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, nullptr)) {
        DWORD size = 0;
        responseBody.clear();
        while (WinHttpQueryDataAvailable(hRequest, &size) && size > 0) {
            std::vector<char> buffer(size);
            DWORD read = 0;
            if (WinHttpReadData(hRequest, buffer.data(), size, &read) && read > 0) {
                responseBody.append(buffer.data(), read);
            }
        }
        success = !responseBody.empty();
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return success;
}

void DismissTransientState() {
    std::lock_guard lock(g_stateMutex);
    g_state.clipboard.active = false;
    g_state.notification.active = false;
    g_state.volume.active = false;
    g_state.battery.active = false;
    g_state.capsLock.active = false;
    g_state.device.active = false;
}

void ShowContextMenu(HWND hwnd, POINT screenPoint) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    Settings currentSettings = GetSettingsSnapshot();
    bool tr = IsTurkish(currentSettings.language);

    // Root Action 1: Dismiss
    AppendMenuW(menu, MF_STRING, 1, tr ? L"✕ Uyarıyı / OSD'yi Kapat" : L"✕ Dismiss current alert / OSD");
    
    // Root Action 2: Pin / Expand / Game switches
    const int pinned = Wh_GetIntValue(L"PinnedExpanded", 0);
    AppendMenuW(menu, MF_STRING | (pinned ? MF_CHECKED : 0), 2, tr ? (pinned ? L"📌 Kapsülü Açık Sabitle: AÇIK ✓" : L"📌 Kapsülü Açık Sabitle: KAPALI") : (pinned ? L"📌 Pinned Expanded: ON ✓" : L"📌 Pinned Expanded: OFF"));

    const int gameOverlayPinned = Wh_GetIntValue(L"GameOverlayPinned", 0);
    AppendMenuW(menu, MF_STRING | (gameOverlayPinned ? MF_CHECKED : 0), 3, tr ? (gameOverlayPinned ? L"🎮 Oyun Overlay Modu: AÇIK ✓" : L"🎮 Oyun Overlay Modu: KAPALI") : (gameOverlayPinned ? L"🎮 Game Overlay Mode: ON ✓" : L"🎮 Game Overlay Mode: OFF"));
    
    const int activeExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0 
                                  ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0) 
                                  : Wh_GetIntSetting(L"Appearance.ExpandOnHover");
    AppendMenuW(menu, MF_STRING | (activeExpandOnHover ? MF_CHECKED : 0), 11, tr ? (activeExpandOnHover ? L"👆 Üzerine Gelince Aç: AÇIK ✓" : L"👆 Üzerine Gelince Aç: KAPALI") : (activeExpandOnHover ? L"👆 Expand on Hover: ON ✓" : L"👆 Expand on Hover: OFF"));

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    // Submenu 1: Pomodoro Focus
    HMENU hPomoMenu = CreatePopupMenu();
    AppendMenuW(hPomoMenu, MF_STRING, 72, tr ? L"▶️ Başlat / Duraklat" : L"▶️ Start / Pause");
    AppendMenuW(hPomoMenu, MF_STRING, 73, tr ? L"🔄 Sıfırla" : L"🔄 Reset");
    AppendMenuW(hPomoMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hPomoMenu, MF_STRING, 90, tr ? L"⏱️ 15 Dakika (Hızlı Odak)" : L"⏱️ 15 min (Quick Focus)");
    AppendMenuW(hPomoMenu, MF_STRING, 91, tr ? L"⏱️ 25 Dakika (Standart)" : L"⏱️ 25 min (Standard)");
    AppendMenuW(hPomoMenu, MF_STRING, 92, tr ? L"⏱️ 45 Dakika (Derin Çalışma)" : L"⏱️ 45 min (Deep Work)");
    AppendMenuW(hPomoMenu, MF_STRING, 93, tr ? L"⏱️ 60 Dakika (Geniş Seans)" : L"⏱️ 60 min (Extended)");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hPomoMenu), tr ? L"🍅 Odaklanma & Pomodoro" : L"🍅 Pomodoro & Focus");

    // Submenu 2: Living Wellness & Health
    HMENU hWellnessMenu = CreatePopupMenu();
    AppendMenuW(hWellnessMenu, MF_STRING, 74, tr ? L"💧 Anlık Su İçme Uyarısı Gönder" : L"💧 Trigger Hydration Alert Now");
    AppendMenuW(hWellnessMenu, MF_STRING, 75, tr ? L"👁️ Anlık 20-20-20 Göz Molası Gönder" : L"👁️ Trigger 20-20-20 Eye Rest");
    AppendMenuW(hWellnessMenu, MF_STRING, 76, tr ? L"🧘 Anlık Duruş & Esneme Uyarısı Gönder" : L"🧘 Trigger Posture & Stretch Alert");
    AppendMenuW(hWellnessMenu, MF_SEPARATOR, 0, nullptr);

    // Hydration Timing Submenu
    HMENU hHydMenu = CreatePopupMenu();
    const int curHyd = currentSettings.hydrationIntervalMinutes;
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 15 ? MF_CHECKED : 0), 110, tr ? L"⏱️ 15 Dakika" : L"⏱️ 15 Minutes");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 30 ? MF_CHECKED : 0), 111, tr ? L"⏱️ 30 Dakika" : L"⏱️ 30 Minutes");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 45 ? MF_CHECKED : 0), 112, tr ? L"⏱️ 45 Dakika (Standart) ✓" : L"⏱️ 45 Minutes (Standard) ✓");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 60 ? MF_CHECKED : 0), 113, tr ? L"⏱️ 60 Dakika (1 Saat)" : L"⏱️ 60 Minutes (1 Hour)");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 90 ? MF_CHECKED : 0), 114, tr ? L"⏱️ 90 Dakika (1.5 Saat)" : L"⏱️ 90 Minutes (1.5 Hours)");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 120 ? MF_CHECKED : 0), 115, tr ? L"⏱️ 120 Dakika (2 Saat)" : L"⏱️ 120 Minutes (2 Hours)");
    AppendMenuW(hHydMenu, MF_STRING | (curHyd == 0 ? MF_CHECKED : 0), 116, tr ? L"❌ Hatırlatıcıyı Kapat" : L"❌ Disable Hydration Alert");
    AppendMenuW(hWellnessMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hHydMenu), tr ? L"💧 Su İçme Aralığı" : L"💧 Hydration Interval");

    // Eye Rest Timing Submenu
    HMENU hEyeMenu = CreatePopupMenu();
    const int curEye = currentSettings.eyeRestReminder ? currentSettings.eyeRestMinutes : 0;
    AppendMenuW(hEyeMenu, MF_STRING | (curEye == 10 ? MF_CHECKED : 0), 120, tr ? L"⏱️ 10 Dakika" : L"⏱️ 10 Minutes");
    AppendMenuW(hEyeMenu, MF_STRING | (curEye == 15 ? MF_CHECKED : 0), 121, tr ? L"⏱️ 15 Dakika" : L"⏱️ 15 Minutes");
    AppendMenuW(hEyeMenu, MF_STRING | (curEye == 20 ? MF_CHECKED : 0), 122, tr ? L"⏱️ 20 Dakika (Standart 20-20-20) ✓" : L"⏱️ 20 Minutes (Standard) ✓");
    AppendMenuW(hEyeMenu, MF_STRING | (curEye == 30 ? MF_CHECKED : 0), 123, tr ? L"⏱️ 30 Dakika" : L"⏱️ 30 Minutes");
    AppendMenuW(hEyeMenu, MF_STRING | (curEye == 0 ? MF_CHECKED : 0), 124, tr ? L"❌ Göz Molasını Kapat" : L"❌ Disable Eye Strain Break");
    AppendMenuW(hWellnessMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hEyeMenu), tr ? L"👁️ 20-20-20 Göz Dinlendirme" : L"👁️ 20-20-20 Eye Strain Break");

    // Posture Timing Submenu
    HMENU hPostMenu = CreatePopupMenu();
    const int curPost = currentSettings.postureReminder ? currentSettings.postureIntervalMinutes : 0;
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 30 ? MF_CHECKED : 0), 130, tr ? L"⏱️ 30 Dakika" : L"⏱️ 30 Minutes");
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 45 ? MF_CHECKED : 0), 131, tr ? L"⏱️ 45 Dakika" : L"⏱️ 45 Minutes");
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 60 ? MF_CHECKED : 0), 132, tr ? L"⏱️ 60 Dakika (1 Saat Standart) ✓" : L"⏱️ 60 Minutes (1 Hour) ✓");
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 90 ? MF_CHECKED : 0), 133, tr ? L"⏱️ 90 Dakika (1.5 Saat)" : L"⏱️ 90 Minutes (1.5 Hours)");
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 120 ? MF_CHECKED : 0), 134, tr ? L"⏱️ 120 Dakika (2 Saat)" : L"⏱️ 120 Minutes (2 Hours)");
    AppendMenuW(hPostMenu, MF_STRING | (curPost == 0 ? MF_CHECKED : 0), 135, tr ? L"❌ Duruş Uyarısını Kapat" : L"❌ Disable Posture Alert");
    AppendMenuW(hWellnessMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hPostMenu), tr ? L"🧘 Duruş & Esneme Süresi" : L"🧘 Posture & Stretch Interval");

    // Uptime Alert Submenu
    HMENU hUpMenu = CreatePopupMenu();
    const int curUp = currentSettings.uptimeAlertHours;
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 1 ? MF_CHECKED : 0), 140, tr ? L"⏱️ 1 Saat" : L"⏱️ 1 Hour");
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 2 ? MF_CHECKED : 0), 141, tr ? L"⏱️ 2 Saat (Standart) ✓" : L"⏱️ 2 Hours (Standard) ✓");
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 3 ? MF_CHECKED : 0), 142, tr ? L"⏱️ 3 Saat" : L"⏱️ 3 Hours");
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 4 ? MF_CHECKED : 0), 143, tr ? L"⏱️ 4 Saat" : L"⏱️ 4 Hours");
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 6 ? MF_CHECKED : 0), 144, tr ? L"⏱️ 6 Saat" : L"⏱️ 6 Hours");
    AppendMenuW(hUpMenu, MF_STRING | (curUp == 0 ? MF_CHECKED : 0), 145, tr ? L"❌ Oturum Uyarısını Kapat" : L"❌ Disable Uptime Alert");
    AppendMenuW(hWellnessMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hUpMenu), tr ? L"⏱️ Uzun Oturum Uyarısı" : L"⏱️ Active PC Session Alert");

    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hWellnessMenu), tr ? L"🌿 Sağlık & Yaşam Asistanı" : L"🌿 Living Wellness & Health");

    // Submenu 3: Quick Notes
    HMENU hNotesMenu = CreatePopupMenu();
    AppendMenuW(hNotesMenu, MF_STRING, 71, tr ? L"📋 Panodan Not Ekle" : L"📋 Add Note from Clipboard");
    AppendMenuW(hNotesMenu, MF_STRING, 77, tr ? L"🗑️ Tüm Notları Temizle" : L"🗑️ Clear All Notes");
    AppendMenuW(hNotesMenu, MF_STRING, 78, tr ? L"📂 Notlar Dosyasını Aç" : L"📂 Open Notes File");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hNotesMenu), tr ? L"📝 Hızlı Notlar" : L"📝 Quick Notes");

    // Submenu 4: Voice Recorder
    HMENU hVoiceMenu = CreatePopupMenu();
    AppendMenuW(hVoiceMenu, MF_STRING, 70, g_isRecording ? (tr ? L"⏹️ Ses Kaydını DURDUR" : L"⏹️ STOP Voice Recording") : (tr ? L"🎙️ Ses Kaydını BAŞLAT" : L"🎙️ START Voice Recording"));
    AppendMenuW(hVoiceMenu, MF_STRING, 79, tr ? L"📂 Kayıt Klasörünü Aç" : L"📂 Open Recordings Folder");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hVoiceMenu), tr ? L"🎙️ Akıllı Ses Kaydedici" : L"🎙️ Voice Recorder");

    // Submenu 5: Idle Style
    HMENU hIdleMenu = CreatePopupMenu();
    const int currentIdleStyle = Wh_GetIntValue(L"IdleDisplayModeOverride", -1) >= 0
                               ? Wh_GetIntValue(L"IdleDisplayModeOverride", 0)
                               : currentSettings.idleDisplayMode;
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 0 ? MF_CHECKED : 0), 80, tr ? L"⚡ Akıllı Dinamik (Otomatik)" : L"⚡ Smart Dynamic (Auto)");
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 1 ? MF_CHECKED : 0), 81, tr ? L"⛅ Saat & Hava Durumu İkili" : L"⛅ Clock & Weather Duo");
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 2 ? MF_CHECKED : 0), 82, tr ? L"🕒 Minimalist Sadece Saat" : L"🕒 Minimalist Clock Only");
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 3 ? MF_CHECKED : 0), 83, tr ? L"📊 Sistem Durumu HUD (Saat/CPU/RAM)" : L"📊 System Telemetry HUD");
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 4 ? MF_CHECKED : 0), 84, tr ? L"☀️ Hava Durumu Odaklı" : L"☀️ Weather & Atmosphere Focus");
    AppendMenuW(hIdleMenu, MF_STRING | (currentIdleStyle == 5 ? MF_CHECKED : 0), 85, tr ? L"🍅 Canlı Pomodoro Sayacı" : L"🍅 Live Pomodoro Focus");
    AppendMenuW(hIdleMenu, MF_SEPARATOR, 0, nullptr);
    const int curSec = Wh_GetIntValue(L"IdleClockSecondsOverride", -1) >= 0
                     ? Wh_GetIntValue(L"IdleClockSecondsOverride", 0)
                     : (currentSettings.idleClockShowSeconds ? 1 : 0);
    AppendMenuW(hIdleMenu, MF_STRING | (curSec ? MF_CHECKED : 0), 86, tr ? L"⏱️ Saatte Saniyeleri Göster" : L"⏱️ Show Seconds in Clock");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hIdleMenu), tr ? L"🖥️ Bekleme Görünümü" : L"🖥️ Idle Display Style");

    // Submenu 6: Themes & Optics
    HMENU hThemesMenu = CreatePopupMenu();
    AppendMenuW(hThemesMenu, MF_STRING, 20, L"🖤 OLED Black (Varsayılan / Default)");
    AppendMenuW(hThemesMenu, MF_STRING, 25, L"💎 Liquid Black Glass");
    AppendMenuW(hThemesMenu, MF_STRING, 26, L"✨ Liquid Silver Glass");
    AppendMenuW(hThemesMenu, MF_STRING, 27, L"🌌 Liquid Midnight");
    AppendMenuW(hThemesMenu, MF_STRING, 28, L"🌠 Liquid Aurora");
    AppendMenuW(hThemesMenu, MF_STRING, 29, L"🪟 Pure Crystal Glass");
    AppendMenuW(hThemesMenu, MF_STRING, 22, L"🌊 Midnight Blue");
    AppendMenuW(hThemesMenu, MF_STRING, 23, L"🔮 Deep Purple");
    AppendMenuW(hThemesMenu, MF_STRING, 24, L"🪟 Fluent Design");
    AppendMenuW(hThemesMenu, MF_SEPARATOR, 0, nullptr);
    const int activeLiquidGlass = Wh_GetIntValue(L"LiquidGlassOverride", -1) >= 0
                                ? Wh_GetIntValue(L"LiquidGlassOverride", 0)
                                : (Wh_GetIntSetting(L"Optics.LiquidGlass") || Wh_GetIntSetting(L"Themes.LiquidGlass"));
    AppendMenuW(hThemesMenu, MF_STRING | (activeLiquidGlass ? MF_CHECKED : 0), 30, tr ? L"💎 Liquid Glass Efekti" : L"💎 Liquid Glass Optics");
    const int activeAccentGlow = Wh_GetIntValue(L"AccentGlowOverride", -1) >= 0
                               ? Wh_GetIntValue(L"AccentGlowOverride", 0)
                               : (Wh_GetIntSetting(L"Optics.AccentGlow") || Wh_GetIntSetting(L"Themes.AccentGlow"));
    AppendMenuW(hThemesMenu, MF_STRING | (activeAccentGlow ? MF_CHECKED : 0), 31, tr ? L"✨ Çevresel Vurgu Işığı" : L"✨ Ambient Accent Glow");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hThemesMenu), tr ? L"🎨 Temalar & Cam Optikleri" : L"🎨 Themes & Liquid Glass");

    // Submenu 7: Shape
    HMENU hShapeMenu = CreatePopupMenu();
    const int currentCorner = Wh_GetIntValue(L"CornerStyleOverride", -1) >= 0
                            ? Wh_GetIntValue(L"CornerStyleOverride", 0)
                            : static_cast<int>(currentSettings.cornerStyle);
    AppendMenuW(hShapeMenu, MF_STRING | (currentCorner == static_cast<int>(CornerStyle::Pill) ? MF_CHECKED : 0), 40, tr ? L"💊 Tam Yuvarlak Hap (Full Pill)" : L"💊 Full Round Pill");
    AppendMenuW(hShapeMenu, MF_STRING | (currentCorner == static_cast<int>(CornerStyle::Squircle) ? MF_CHECKED : 0), 41, L"🔲 Vision Squircle");
    AppendMenuW(hShapeMenu, MF_STRING | (currentCorner == static_cast<int>(CornerStyle::ModernBox) ? MF_CHECKED : 0), 42, tr ? L"📦 Modern 8px Kutu" : L"📦 Modern 8px Box");
    AppendMenuW(hShapeMenu, MF_STRING | (currentCorner == static_cast<int>(CornerStyle::Sharp) ? MF_CHECKED : 0), 43, tr ? L"📐 Keskin Minimal Kutu" : L"📐 Sharp Minimal Box");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hShapeMenu), tr ? L"📐 Kapsül Şekli" : L"📐 Shape & Geometry");

    // Submenu 8: Language with full flag icons
    HMENU hLangMenu = CreatePopupMenu();
    const int curLang = Wh_GetIntValue(L"LanguageOverride", -1) >= 0
                      ? Wh_GetIntValue(L"LanguageOverride", 0)
                      : static_cast<int>(currentSettings.language);
    AppendMenuW(hLangMenu, MF_STRING | (curLang == 0 ? MF_CHECKED : 0), 100, L"🌐 Otomatik (Sistem Dili) / Auto");
    AppendMenuW(hLangMenu, MF_STRING | (curLang == 1 ? MF_CHECKED : 0), 101, L"🇹🇷 Türkçe (Turkish)");
    AppendMenuW(hLangMenu, MF_STRING | (curLang == 2 ? MF_CHECKED : 0), 102, L"🇬🇧 English (İngilizce)");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hLangMenu), tr ? L"🌐 Dil / Language" : L"🌐 Language / Dil");

    // Submenu 9: Opacity
    HMENU hOpacityMenu = CreatePopupMenu();
    AppendMenuW(hOpacityMenu, MF_STRING, 4, L"100% (Opaque)");
    AppendMenuW(hOpacityMenu, MF_STRING, 5, L"85%");
    AppendMenuW(hOpacityMenu, MF_STRING, 6, L"70%");
    AppendMenuW(hOpacityMenu, MF_STRING, 7, L"55%");
    AppendMenuW(hOpacityMenu, MF_STRING, 8, tr ? L"Sıfırla" : L"Reset to Default");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hOpacityMenu), tr ? L"🔆 Saydamlık" : L"🔆 Transparency");

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 9, tr ? L"⚙️ Windhawk Ayarlarını Aç" : L"⚙️ Open Windhawk settings");

    SetForegroundWindow(hwnd);
    const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                   screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    switch (cmd) {
        case 1:
            DismissTransientState();
            break;
        case 2:
            Wh_SetIntValue(L"PinnedExpanded", Wh_GetIntValue(L"PinnedExpanded", 0) ? 0 : 1);
            g_layoutDirty = true;
            break;
        case 3:
            Wh_SetIntValue(L"GameOverlayPinned", Wh_GetIntValue(L"GameOverlayPinned", 0) ? 0 : 1);
            g_layoutDirty = true;
            break;
        case 4: Wh_SetIntValue(L"PillOpacityOverride", 100); LoadSettings(); break;
        case 5: Wh_SetIntValue(L"PillOpacityOverride", 85); LoadSettings(); break;
        case 6: Wh_SetIntValue(L"PillOpacityOverride", 70); LoadSettings(); break;
        case 7: Wh_SetIntValue(L"PillOpacityOverride", 55); LoadSettings(); break;
        case 8: Wh_SetIntValue(L"PillOpacityOverride", -1); LoadSettings(); break;
        case 9: {
            wchar_t currentProcessPath[MAX_PATH] = {};
            GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
            HINSTANCE result = ShellExecuteW(nullptr, L"open", currentProcessPath, nullptr, nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(result) <= 32) {
                Wh_Log(L"Failed to open Windhawk settings.");
            }
            break;
        }
        case 70:
            if (g_isRecording) StopVoiceRecording();
            else StartVoiceRecording();
            break;
        case 71: {
            if (OpenClipboard(hwnd)) {
                HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                if (hData) {
                    wchar_t* text = static_cast<wchar_t*>(GlobalLock(hData));
                    if (text) {
                        SaveQuickNote(text);
                        GlobalUnlock(hData);
                        TriggerCustomAlert(L"Quick Notes", tr ? L"Not Kaydedildi 📝" : L"Note Saved 📝", text, 3.5);
                    }
                }
                CloseClipboard();
            }
            break;
        }
        case 72: {
            std::lock_guard lock(g_stateMutex);
            if (g_state.pomodoro.state == PomodoroState::Stopped || g_state.pomodoro.state == PomodoroState::Break) {
                g_state.pomodoro.state = PomodoroState::Working;
                g_state.pomodoro.lastTick = NowSeconds();
            } else {
                g_state.pomodoro.state = PomodoroState::Stopped;
            }
            break;
        }
        case 73: {
            std::lock_guard lock(g_stateMutex);
            Settings s = GetSettingsSnapshot();
            g_state.pomodoro.state = PomodoroState::Stopped;
            g_state.pomodoro.remainingSeconds = s.pomodoroWorkMinutes * 60;
            g_state.pomodoro.totalSeconds = s.pomodoroWorkMinutes * 60;
            break;
        }
        case 74:
            TriggerCustomAlert(L"Living Assistant", tr ? L"Su İçme Vakti 💧" : L"Time to Drink Water 💧", tr ? L"Vücudunuzu susuz bırakmayın! Bir bardak su için ve biraz esneyin." : L"Stay hydrated! Take a sip of water and stretch.", 6.0);
            break;
        case 75:
            TriggerCustomAlert(L"Living Assistant", tr ? L"20-20-20 Kuralı 👁️" : L"20-20-20 Rule 👁️", tr ? L"Gözlerinizi dinlendirin! 20 saniye boyunca 20 fit (6 metre) uzağa bakın." : L"Rest your eyes! Look at something 20 feet away for 20 seconds.", 6.0);
            break;
        case 76:
            TriggerCustomAlert(L"Living Assistant", tr ? L"Duruş & Esneme 🧘" : L"Posture & Stretch 🧘", tr ? L"Ayağa kalkın, omuzlarınızı geriye alın ve derin bir nefes alıp esneyin." : L"Stand up, roll your shoulders, and do a quick 30s stretch.", 6.0);
            break;
        case 77: {
            std::lock_guard lock(g_stateMutex);
            g_state.quickNotes.clear();
            std::wstring path = GetNotesFilePath();
            DeleteFileW(path.c_str());
            TriggerCustomAlert(L"Quick Notes", tr ? L"Tüm Notlar Temizlendi 🗑️" : L"Notes Cleared 🗑️", tr ? L"Tüm notlarınız başarıyla silindi." : L"All notes have been removed.", 3.0);
            break;
        }
        case 78: {
            std::wstring path = GetNotesFilePath();
            ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        }
        case 79: {
            std::wstring folder = GetRecordingsDirectory();
            ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        }
        case 80: Wh_SetIntValue(L"IdleDisplayModeOverride", 0); LoadSettings(); g_layoutDirty = true; break;
        case 81: Wh_SetIntValue(L"IdleDisplayModeOverride", 1); LoadSettings(); g_layoutDirty = true; break;
        case 82: Wh_SetIntValue(L"IdleDisplayModeOverride", 2); LoadSettings(); g_layoutDirty = true; break;
        case 83: Wh_SetIntValue(L"IdleDisplayModeOverride", 3); LoadSettings(); g_layoutDirty = true; break;
        case 84: Wh_SetIntValue(L"IdleDisplayModeOverride", 4); LoadSettings(); g_layoutDirty = true; break;
        case 85: Wh_SetIntValue(L"IdleDisplayModeOverride", 5); LoadSettings(); g_layoutDirty = true; break;
        case 86: {
            const int curSecVal = Wh_GetIntValue(L"IdleClockSecondsOverride", -1) >= 0
                                ? Wh_GetIntValue(L"IdleClockSecondsOverride", 0)
                                : (currentSettings.idleClockShowSeconds ? 1 : 0);
            Wh_SetIntValue(L"IdleClockSecondsOverride", curSecVal ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 90: {
            std::lock_guard lock(g_stateMutex);
            g_state.pomodoro.state = PomodoroState::Stopped;
            g_state.pomodoro.remainingSeconds = 15 * 60;
            g_state.pomodoro.totalSeconds = 15 * 60;
            TriggerCustomAlert(L"Pomodoro Timer", tr ? L"15 Dk Odak Ayarlandı ⏱️" : L"15 Min Focus Set ⏱️", tr ? L"Pomodoro seansı 15 dakikaya ayarlandı." : L"Pomodoro session set to 15 minutes.", 3.0);
            break;
        }
        case 91: {
            std::lock_guard lock(g_stateMutex);
            g_state.pomodoro.state = PomodoroState::Stopped;
            g_state.pomodoro.remainingSeconds = 25 * 60;
            g_state.pomodoro.totalSeconds = 25 * 60;
            TriggerCustomAlert(L"Pomodoro Timer", tr ? L"25 Dk Standart Odak ⏱️" : L"25 Min Focus Set ⏱️", tr ? L"Standart 25 dakikalık seans ayarlandı." : L"Standard Pomodoro session set to 25 minutes.", 3.0);
            break;
        }
        case 92: {
            std::lock_guard lock(g_stateMutex);
            g_state.pomodoro.state = PomodoroState::Stopped;
            g_state.pomodoro.remainingSeconds = 45 * 60;
            g_state.pomodoro.totalSeconds = 45 * 60;
            TriggerCustomAlert(L"Pomodoro Timer", tr ? L"45 Dk Derin Çalışma ⏱️" : L"45 Min Focus Set ⏱️", tr ? L"45 dakikalık derin odaklanma seansı ayarlandı." : L"Deep work session set to 45 minutes.", 3.0);
            break;
        }
        case 93: {
            std::lock_guard lock(g_stateMutex);
            g_state.pomodoro.state = PomodoroState::Stopped;
            g_state.pomodoro.remainingSeconds = 60 * 60;
            g_state.pomodoro.totalSeconds = 60 * 60;
            TriggerCustomAlert(L"Pomodoro Timer", tr ? L"60 Dk Genişletilmiş Odak ⏱️" : L"60 Min Focus Set ⏱️", tr ? L"60 dakikalık uzun çalışma seansı ayarlandı." : L"Extended deep work session set to 60 minutes.", 3.0);
            break;
        }
        case 100: Wh_SetIntValue(L"LanguageOverride", 0); LoadSettings(); g_layoutDirty = true; break;
        case 101: Wh_SetIntValue(L"LanguageOverride", 1); LoadSettings(); g_layoutDirty = true; break;
        case 102: Wh_SetIntValue(L"LanguageOverride", 2); LoadSettings(); g_layoutDirty = true; break;

        // Hydration timing overrides
        case 110: Wh_SetIntValue(L"HydrationIntervalOverride", 15); LoadSettings(); break;
        case 111: Wh_SetIntValue(L"HydrationIntervalOverride", 30); LoadSettings(); break;
        case 112: Wh_SetIntValue(L"HydrationIntervalOverride", 45); LoadSettings(); break;
        case 113: Wh_SetIntValue(L"HydrationIntervalOverride", 60); LoadSettings(); break;
        case 114: Wh_SetIntValue(L"HydrationIntervalOverride", 90); LoadSettings(); break;
        case 115: Wh_SetIntValue(L"HydrationIntervalOverride", 120); LoadSettings(); break;
        case 116: Wh_SetIntValue(L"HydrationIntervalOverride", 0); LoadSettings(); break;

        // Eye rest timing overrides
        case 120: Wh_SetIntValue(L"EyeRestIntervalOverride", 10); LoadSettings(); break;
        case 121: Wh_SetIntValue(L"EyeRestIntervalOverride", 15); LoadSettings(); break;
        case 122: Wh_SetIntValue(L"EyeRestIntervalOverride", 20); LoadSettings(); break;
        case 123: Wh_SetIntValue(L"EyeRestIntervalOverride", 30); LoadSettings(); break;
        case 124: Wh_SetIntValue(L"EyeRestIntervalOverride", 0); LoadSettings(); break;

        // Posture timing overrides
        case 130: Wh_SetIntValue(L"PostureIntervalOverride", 30); LoadSettings(); break;
        case 131: Wh_SetIntValue(L"PostureIntervalOverride", 45); LoadSettings(); break;
        case 132: Wh_SetIntValue(L"PostureIntervalOverride", 60); LoadSettings(); break;
        case 133: Wh_SetIntValue(L"PostureIntervalOverride", 90); LoadSettings(); break;
        case 134: Wh_SetIntValue(L"PostureIntervalOverride", 120); LoadSettings(); break;
        case 135: Wh_SetIntValue(L"PostureIntervalOverride", 0); LoadSettings(); break;

        // Uptime alert overrides
        case 140: Wh_SetIntValue(L"UptimeAlertOverride", 1); LoadSettings(); break;
        case 141: Wh_SetIntValue(L"UptimeAlertOverride", 2); LoadSettings(); break;
        case 142: Wh_SetIntValue(L"UptimeAlertOverride", 3); LoadSettings(); break;
        case 143: Wh_SetIntValue(L"UptimeAlertOverride", 4); LoadSettings(); break;
        case 144: Wh_SetIntValue(L"UptimeAlertOverride", 6); LoadSettings(); break;
        case 145: Wh_SetIntValue(L"UptimeAlertOverride", 0); LoadSettings(); break;

        case 40: // Pill
            Wh_SetIntValue(L"CornerStyleOverride", static_cast<int>(CornerStyle::Pill));
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 41: // Squircle
            Wh_SetIntValue(L"CornerStyleOverride", static_cast<int>(CornerStyle::Squircle));
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 42: // ModernBox
            Wh_SetIntValue(L"CornerStyleOverride", static_cast<int>(CornerStyle::ModernBox));
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 43: // Sharp
            Wh_SetIntValue(L"CornerStyleOverride", static_cast<int>(CornerStyle::Sharp));
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 11: {
            const int activeExpandVal = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0
                                      ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0)
                                      : Wh_GetIntSetting(L"Appearance.ExpandOnHover");
            Wh_SetIntValue(L"ExpandOnHoverOverride", activeExpandVal ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 30: {
            const int activeLgVal = Wh_GetIntValue(L"LiquidGlassOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"LiquidGlassOverride", 0)
                                  : (Wh_GetIntSetting(L"Optics.LiquidGlass") || Wh_GetIntSetting(L"Themes.LiquidGlass"));
            Wh_SetIntValue(L"LiquidGlassOverride", activeLgVal ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 31: {
            const int activeAgVal = Wh_GetIntValue(L"AccentGlowOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"AccentGlowOverride", 0)
                                  : (Wh_GetIntSetting(L"Optics.AccentGlow") || Wh_GetIntSetting(L"Themes.AccentGlow"));
            Wh_SetIntValue(L"AccentGlowOverride", activeAgVal ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 32: {
            const int activeMetricsVal = Wh_GetIntValue(L"ShowMetricsInIdleOverride", -1) >= 0
                                       ? Wh_GetIntValue(L"ShowMetricsInIdleOverride", 0)
                                       : Wh_GetIntSetting(L"Modules.ShowMetricsInIdle");
            Wh_SetIntValue(L"ShowMetricsInIdleOverride", activeMetricsVal ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        // Color theme presets
        case 20: Wh_SetIntValue(L"ColorTheme", 0); LoadSettings(); break;
        case 21: Wh_SetIntValue(L"ColorTheme", 1); LoadSettings(); break;
        case 22: Wh_SetIntValue(L"ColorTheme", 2); LoadSettings(); break;
        case 23: Wh_SetIntValue(L"ColorTheme", 3); LoadSettings(); break;
        case 24: Wh_SetIntValue(L"ColorTheme", 4); LoadSettings(); break;
        case 25: Wh_SetIntValue(L"ColorTheme", 5); LoadSettings(); break;
        case 26: Wh_SetIntValue(L"ColorTheme", 6); LoadSettings(); break;
        case 27: Wh_SetIntValue(L"ColorTheme", 7); LoadSettings(); break;
        case 28: Wh_SetIntValue(L"ColorTheme", 8); LoadSettings(); break;
        case 29: Wh_SetIntValue(L"ColorTheme", 9); LoadSettings(); break;
        case 50: Wh_SetIntValue(L"LiquidGlassIntensityOverride", 25); LoadSettings(); break;
        case 51: Wh_SetIntValue(L"LiquidGlassIntensityOverride", 50); LoadSettings(); break;
        case 52: Wh_SetIntValue(L"LiquidGlassIntensityOverride", 75); LoadSettings(); break;
        case 53: Wh_SetIntValue(L"LiquidGlassIntensityOverride", 100); LoadSettings(); break;
        case 60: Wh_SetIntValue(L"AccentGlowRadiusOverride", 16); LoadSettings(); break;
        case 61: Wh_SetIntValue(L"AccentGlowRadiusOverride", 24); LoadSettings(); break;
        case 62: Wh_SetIntValue(L"AccentGlowRadiusOverride", 32); LoadSettings(); break;
        case 63: Wh_SetIntValue(L"AccentGlowRadiusOverride", 48); LoadSettings(); break;
    }
}

class Renderer {
   private:
    HWND hwnd_ = nullptr;
    HDC memDc_ = nullptr;
    HBITMAP memBitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    int bitmapWidth_ = 0;
    int bitmapHeight_ = 0;
    int bitmapCapacityW_ = 0;
    int bitmapCapacityH_ = 0;
    float settingsOpacity_ = 0.96f;

    ComPtr<ID2D1Factory> d2dFactory_;
    ComPtr<IDWriteFactory> dwriteFactory_;
    ComPtr<ID2D1DCRenderTarget> target_;

    ComPtr<ID2D1SolidColorBrush> pillBgBrush_;
    ComPtr<ID2D1SolidColorBrush> tintBrush_;
    ComPtr<ID2D1SolidColorBrush> accentBrush_;
    ComPtr<ID2D1SolidColorBrush> textBrush_;
    ComPtr<ID2D1SolidColorBrush> mutedBrush_;
    ComPtr<ID2D1SolidColorBrush> whiteBrush_;
    ComPtr<ID2D1SolidColorBrush> redBrush_;
    ComPtr<ID2D1SolidColorBrush> greenBrush_;
    ComPtr<ID2D1SolidColorBrush> yellowBrush_;
    ComPtr<ID2D1SolidColorBrush> cyanBrush_;
    ComPtr<ID2D1SolidColorBrush> orangeBrush_;
    ComPtr<ID2D1SolidColorBrush> magentaBrush_;
    ComPtr<ID2D1SolidColorBrush> waveformBrush_;
    ComPtr<ID2D1SolidColorBrush> cardBgBrush_;
    ComPtr<ID2D1SolidColorBrush> cardBorderBrush_;
    ComPtr<ID2D1SolidColorBrush> dividerBrush_;
    ComPtr<ID2D1SolidColorBrush> weekendBrush_;
    ComPtr<ID2D1SolidColorBrush> calHeaderBrush_;
    ComPtr<ID2D1SolidColorBrush> glassBorderBrush_;
    ComPtr<ID2D1SolidColorBrush> glassGlossBrush_;
    ComPtr<ID2D1SolidColorBrush> shadowBrushes_[6];

    ComPtr<IDWriteTextFormat> textFormat_;
    ComPtr<IDWriteTextFormat> smallTextFormat_;
    ComPtr<IDWriteTextFormat> clockFormat_;
    ComPtr<IDWriteTextFormat> boldTextFormat_;
    ComPtr<IDWriteTextFormat> hugeTextFormat_;
    ComPtr<IDWriteTextFormat> iconFormat_;
    ComPtr<IDWriteTextFormat> emojiFormat_;

    float tabTransitionPos_ = 0.0f;

    ComPtr<ID2D1Bitmap> artBitmap_;
    uint64_t artGeneration_ = 0;
    ComPtr<ID2D1Bitmap> mediaSourceIconBitmap_;
    uint64_t mediaSourceIconGeneration_ = 0;
    ComPtr<ID2D1Bitmap> notificationIconBitmap_;
    uint64_t notificationIconGeneration_ = 0;
    ComPtr<ID2D1Bitmap> clipboardIconBitmap_;
    uint64_t clipboardIconGeneration_ = 0;

    // Gradient brush cache — avoids per-frame StopCollection + Brush allocation
    ComPtr<ID2D1GradientStopCollection> sheenStops_;
    ComPtr<ID2D1LinearGradientBrush> sheenBrush_;
    float cachedSheenIntensity_ = -1.0f;
    float cachedSheenOpacity_ = -1.0f;

    ComPtr<ID2D1GradientStopCollection> shimmerStops_;
    ComPtr<ID2D1LinearGradientBrush> shimmerBrush_;
    float cachedShimmerIntensity_ = -1.0f;
    float cachedShimmerOpacity_ = -1.0f;

    ComPtr<ID2D1SolidColorBrush> glowBorderBrush_;

    // Text layout cache — avoids per-frame IDWriteTextLayout allocation for marquee
    std::wstring cachedMarqueeText_;
    float cachedMarqueeHeight_ = 0.0f;
    ComPtr<IDWriteTextLayout> cachedMarqueeLayout_;

    float lastFontScale_ = 0.0f; // tracks current font scale to avoid redundant format recreation

   public:
    bool Initialize(HWND hwnd) {
        hwnd_ = hwnd;

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                       __uuidof(ID2D1Factory),
                                       reinterpret_cast<void**>(d2dFactory_.GetAddressOf()));
        if (FAILED(hr)) return false;

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                 reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf()));
        if (FAILED(hr)) return false;

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f,
            D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

        hr = d2dFactory_->CreateDCRenderTarget(&props, &target_);
        if (FAILED(hr)) return false;

        EnsureTextFormats(1.0f);
        return EnsureBackingBitmap(640, 200);
    }

    bool EnsureBackingBitmap(int width, int height) {
        if (width <= 0 || height <= 0) return false;
        
        int targetCapW = ((width + 127) / 128) * 128;
        int targetCapH = ((height + 63) / 64) * 64;

        if (bitmapCapacityW_ >= targetCapW && bitmapCapacityH_ >= targetCapH && memDc_ && memBitmap_) {
            bitmapWidth_ = width;
            bitmapHeight_ = height;
            return true;
        }

        int newCapW = std::max(bitmapCapacityW_, targetCapW);
        int newCapH = std::max(bitmapCapacityH_, targetCapH);

        HDC screen = GetDC(nullptr);
        if (!memDc_) memDc_ = CreateCompatibleDC(screen);

        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = newCapW;
        bi.bmiHeader.biHeight = -newCapH;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP nextBitmap = CreateDIBSection(memDc_, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        ReleaseDC(nullptr, screen);

        if (!nextBitmap) return false;

        if (oldBitmap_) SelectObject(memDc_, oldBitmap_);
        if (memBitmap_) DeleteObject(memBitmap_);

        memBitmap_ = nextBitmap;
        oldBitmap_ = SelectObject(memDc_, memBitmap_);
        bitmapCapacityW_ = newCapW;
        bitmapCapacityH_ = newCapH;
        bitmapWidth_ = width;
        bitmapHeight_ = height;
        return true;
    }

    void EnsureTextFormats(float scale) {
        if (std::abs(scale - lastFontScale_) < 0.001f && textFormat_) {
            return;
        }
        lastFontScale_ = scale;

        textFormat_ = nullptr;
        smallTextFormat_ = nullptr;
        clockFormat_ = nullptr;
        boldTextFormat_ = nullptr;
        hugeTextFormat_ = nullptr;
        iconFormat_ = nullptr;

        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         13.5f * scale, L"", &textFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Small", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         11.0f * scale, L"", &smallTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         18.0f * scale, L"", &clockFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         12.0f * scale, L"", &boldTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         38.0f * scale, L"", &hugeTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe Fluent Icons", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         16.0f * scale, L"", &iconFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Emoji", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         15.0f * scale, L"", &emojiFormat_);

        if (textFormat_) textFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (smallTextFormat_) smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (boldTextFormat_) {
            boldTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            boldTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (hugeTextFormat_) {
            hugeTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            hugeTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (clockFormat_) {
            clockFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            clockFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            clockFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (iconFormat_) iconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (emojiFormat_) {
            emojiFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            emojiFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            emojiFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    void EnsureBrushes(const Settings& settings, const SharedState& state) {
        if (!target_) return;

        D2D1_COLOR_F pillColor = settings.pillBgColor;
        pillColor.a *= settings.pillOpacity;
        if (!pillBgBrush_) target_->CreateSolidColorBrush(pillColor, &pillBgBrush_);
        else pillBgBrush_->SetColor(pillColor);

        D2D1_COLOR_F tintColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, settings.tintOpacity * settings.pillOpacity);
        if (!tintBrush_) target_->CreateSolidColorBrush(tintColor, &tintBrush_);
        else tintBrush_->SetColor(tintColor);

        D2D1_COLOR_F accent = settings.customAccent;
        if (settings.accentMode == AccentMode::Auto) {
            if (!state.media.art.bgra.empty()) accent = state.media.art.sampledAccent;
            else accent = settings.customAccent;
        } else if (settings.accentMode == AccentMode::System) {
            DWORD color = 0;
            BOOL opaque = TRUE;
            if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
                // DwmGetColorizationColor returns 0xAARRGGBB. We ignore the alpha byte
                // and force full opacity since accent is used for glow/highlight effects.
                accent = D2D1::ColorF(
                    ((color >> 16) & 0xff) / 255.0f,
                    ((color >> 8) & 0xff) / 255.0f,
                    (color & 0xff) / 255.0f,
                    1.0f); // Always opaque — do not use the alpha byte from DWM
            }
        }
        if (!accentBrush_) target_->CreateSolidColorBrush(accent, &accentBrush_);
        else accentBrush_->SetColor(accent);

        if (!textBrush_) target_->CreateSolidColorBrush(settings.textPrimaryColor, &textBrush_);
        else textBrush_->SetColor(settings.textPrimaryColor);

        if (!mutedBrush_) target_->CreateSolidColorBrush(settings.textSecondaryColor, &mutedBrush_);
        else mutedBrush_->SetColor(settings.textSecondaryColor);

        if (!whiteBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &whiteBrush_);
        if (!redBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &redBrush_);
        if (!greenBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.90f, 0.46f, 1.0f), &greenBrush_);
        if (!yellowBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.85f, 0.0f, 1.0f), &yellowBrush_);
        if (!cyanBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.82f, 1.0f, 1.0f), &cyanBrush_);
        if (!orangeBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.57f, 0.0f, 1.0f), &orangeBrush_);
        if (!magentaBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(0.83f, 0.0f, 1.0f, 1.0f), &magentaBrush_);
        if (!waveformBrush_) target_->CreateSolidColorBrush(accent, &waveformBrush_);

        D2D1_COLOR_F cardBgCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.080f * settingsOpacity_);
        if (!cardBgBrush_) target_->CreateSolidColorBrush(cardBgCol, &cardBgBrush_);
        else cardBgBrush_->SetColor(cardBgCol);

        D2D1_COLOR_F cardBorderCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.060f * settingsOpacity_);
        if (!cardBorderBrush_) target_->CreateSolidColorBrush(cardBorderCol, &cardBorderBrush_);
        else cardBorderBrush_->SetColor(cardBorderCol);

        D2D1_COLOR_F divCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f * settingsOpacity_);
        if (!dividerBrush_) target_->CreateSolidColorBrush(divCol, &dividerBrush_);
        else dividerBrush_->SetColor(divCol);

        D2D1_COLOR_F weekendCol = D2D1::ColorF(0.85f, 0.25f, 0.20f, 0.65f * settingsOpacity_);
        if (!weekendBrush_) target_->CreateSolidColorBrush(weekendCol, &weekendBrush_);
        else weekendBrush_->SetColor(weekendCol);

        D2D1_COLOR_F calHeadCol = D2D1::ColorF(0.85f, 0.25f, 0.20f, 0.90f * settingsOpacity_);
        if (!calHeaderBrush_) target_->CreateSolidColorBrush(calHeadCol, &calHeaderBrush_);
        else calHeaderBrush_->SetColor(calHeadCol);

        D2D1_COLOR_F rimCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.16f * settings.pillOpacity);
        if (!glassBorderBrush_) target_->CreateSolidColorBrush(rimCol, &glassBorderBrush_);
        else glassBorderBrush_->SetColor(rimCol);

        D2D1_COLOR_F glossCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.35f * settings.pillOpacity);
        if (!glassGlossBrush_) target_->CreateSolidColorBrush(glossCol, &glassGlossBrush_);
        else glassGlossBrush_->SetColor(glossCol);

        if (!shimmerBrush_ || !shimmerStops_) {
            D2D1_GRADIENT_STOP stops[3];
            stops[0] = {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)};
            stops[1] = {0.5f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.09f)};
            stops[2] = {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)};
            if (SUCCEEDED(target_->CreateGradientStopCollection(stops, 3, &shimmerStops_))) {
                target_->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(100, 100)),
                    shimmerStops_.Get(), &shimmerBrush_);
            }
        }
    }

    IDWriteFactory* GetDWriteFactory() const { return dwriteFactory_.Get(); }
    IDWriteTextFormat* GetTextFormat() const { return textFormat_.Get(); }

    float MeasureNotificationWidth(const std::wstring& title, const std::wstring& body) {
        auto SanitizeString = [](const std::wstring& in) -> std::wstring {
            std::wstring out;
            out.reserve(in.size());
            bool lastSpace = true;
            for (wchar_t c : in) {
                if (c == L'\r' || c == L'\n' || c == L'\t' || c == L' ') {
                    if (!lastSpace) {
                        out.push_back(L' ');
                        lastSpace = true;
                    }
                } else {
                    out.push_back(c);
                    lastSpace = false;
                }
            }
            while (!out.empty() && out.back() == L' ') out.pop_back();
            return out;
        };

        std::wstring cleanTitle = SanitizeString(title);
        std::wstring cleanBody = SanitizeString(body);

        if (!dwriteFactory_ || !textFormat_) {
            return 80.0f + static_cast<float>(std::max(cleanTitle.size(), cleanBody.size())) * 8.0f;
        }

        float maxW = 0.0f;
        if (!cleanTitle.empty()) {
            ComPtr<IDWriteTextLayout> layout;
            if (SUCCEEDED(dwriteFactory_->CreateTextLayout(cleanTitle.c_str(), static_cast<UINT32>(cleanTitle.size()), textFormat_.Get(), 4000.0f, 50.0f, &layout)) && layout) {
                DWRITE_TEXT_METRICS m = {};
                layout->GetMetrics(&m);
                if (m.widthIncludingTrailingWhitespace > maxW) maxW = m.widthIncludingTrailingWhitespace;
            }
        }
        if (!cleanBody.empty() && cleanBody != cleanTitle) {
            ComPtr<IDWriteTextLayout> layout;
            if (SUCCEEDED(dwriteFactory_->CreateTextLayout(cleanBody.c_str(), static_cast<UINT32>(cleanBody.size()), smallTextFormat_.Get() ? smallTextFormat_.Get() : textFormat_.Get(), 4000.0f, 50.0f, &layout)) && layout) {
                DWRITE_TEXT_METRICS m = {};
                layout->GetMetrics(&m);
                if (m.widthIncludingTrailingWhitespace > maxW) maxW = m.widthIncludingTrailingWhitespace;
            }
        }
        return maxW + 72.0f;
    }

    void DrawSoftShadow(D2D1_RECT_F rect, float radius, float softness, const Settings& settings) {
        if (!target_) return;
        const int steps = 5;
        float maxSpread = settings.shadowSpreadPx > 0.1f ? settings.shadowSpreadPx : 10.0f * Clamp(softness, 0.1f, 2.0f);
        const float baseAlpha = 0.045f * softness * settingsOpacity_;
        const float yBase = settings.shadowYOffsetPx;

        for (int i = steps; i >= 1; --i) {
            const float spread = (static_cast<float>(i) / steps) * maxSpread;
            const float yOffset = spread * 0.30f + yBase;
            const float alpha = baseAlpha * (1.0f - static_cast<float>(i - 1) / steps * 0.55f);

            D2D1_RECT_F sRect = D2D1::RectF(
                rect.left - spread * 0.5f,
                rect.top - spread * 0.2f + yOffset,
                rect.right + spread * 0.5f,
                rect.bottom + spread * 0.8f + yOffset
            );

            if (!shadowBrushes_[i]) {
                target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha), &shadowBrushes_[i]);
            } else {
                shadowBrushes_[i]->SetOpacity(alpha);
            }

            if (shadowBrushes_[i]) {
                target_->FillRoundedRectangle(
                    D2D1::RoundedRect(sRect, radius + spread * 0.5f, radius + spread * 0.5f),
                    shadowBrushes_[i].Get());
            }
        }
    }

    void DrawAccentGlow(D2D1_RECT_F rect, const Activity& activity, const Settings& settings, double now) {
        if (!accentBrush_ || !target_) return;

        float opacity = (activity.kind == IslandKind::Media ? 0.22f : 0.12f) * settings.accentGlowIntensity;
        if (activity.kind == IslandKind::BatteryLow) {
            if (redBrush_) {
                const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 4.0));
                redBrush_->SetOpacity((0.20f + 0.15f * pulse) * settingsOpacity_);
                D2D1_RECT_F glowRect = D2D1::RectF(rect.left - 2.0f, rect.top - 1.5f, rect.right + 2.0f, rect.bottom + 2.0f);
                float rad = (rect.bottom - rect.top) * 0.5f + 1.0f;
                target_->DrawRoundedRectangle(D2D1::RoundedRect(glowRect, rad, rad), redBrush_.Get(), 2.0f);
                redBrush_->SetOpacity(1.0f);
            }
            return;
        }

        if (activity.kind == IslandKind::Recording) {
            if (redBrush_) {
                const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 5.0));
                redBrush_->SetOpacity((0.25f + 0.20f * pulse) * settingsOpacity_);
                D2D1_RECT_F glowRect = D2D1::RectF(rect.left - 2.5f, rect.top - 2.0f, rect.right + 2.5f, rect.bottom + 2.5f);
                float rad = (rect.bottom - rect.top) * 0.5f + 1.5f;
                target_->DrawRoundedRectangle(D2D1::RoundedRect(glowRect, rad, rad), redBrush_.Get(), 2.5f);
                redBrush_->SetOpacity(1.0f);
            }
            return;
        }

        const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 1.8));
        opacity = (opacity + 0.05f * pulse) * settingsOpacity_;

        // Crisp, elegant outer ambient glow perfectly conforming to the capsule border
        accentBrush_->SetOpacity(opacity * 0.45f);
        D2D1_RECT_F haloRect = D2D1::RectF(rect.left - 2.0f, rect.top - 1.5f, rect.right + 2.0f, rect.bottom + 2.0f);
        float rad = (rect.bottom - rect.top) * 0.5f + 1.0f;
        target_->DrawRoundedRectangle(D2D1::RoundedRect(haloRect, rad, rad), accentBrush_.Get(), 2.0f);
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawPillSurface(D2D1_RECT_F rect, float radius, IslandKind kind, const Settings& settings, double now) {
        (void)kind;

        // 1. Dark Base Tint
        if (tintBrush_) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), tintBrush_.Get());
        }

        // 2. Base Capsule Color
        if (pillBgBrush_) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), pillBgBrush_.Get());
        }

        // 3. Liquid Glass Multi-Layer Optics Pipeline
        if (settings.liquidGlass && target_) {
            const float intensity = Clamp(settings.liquidGlassIntensity, 0.0f, 1.0f);
            const float iOp = intensity * settingsOpacity_;

            // A. Surface Meniscus Curvature Sheen (Curved vertical droplet reflection)
            if (settings.glassSheen) {
                if (!sheenBrush_ || cachedSheenIntensity_ != intensity || cachedSheenOpacity_ != settingsOpacity_) {
                    D2D1_GRADIENT_STOP stops[4];
                    stops[0] = {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f * iOp)};
                    stops[1] = {0.35f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f * iOp)};
                    stops[2] = {0.80f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.02f * iOp)};
                    stops[3] = {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f * iOp)};
                    sheenStops_ = nullptr;
                    if (SUCCEEDED(target_->CreateGradientStopCollection(stops, 4, &sheenStops_))) {
                        sheenBrush_ = nullptr;
                        target_->CreateLinearGradientBrush(
                            D2D1::LinearGradientBrushProperties(
                                D2D1::Point2F((rect.left + rect.right) * 0.5f, rect.top),
                                D2D1::Point2F((rect.left + rect.right) * 0.5f, rect.bottom)),
                            sheenStops_.Get(), &sheenBrush_);
                        cachedSheenIntensity_ = intensity;
                        cachedSheenOpacity_ = settingsOpacity_;
                    }
                }
                if (sheenBrush_) {
                    target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), sheenBrush_.Get());
                }
            }

            // B. Animated Organic Liquid Caustic Light Waves
            if (settings.glassCaustics && !settings.lowPowerMode && settings.causticsSpeed > 0.01f && shimmerBrush_) {
                const float pillW = rect.right - rect.left;
                if (pillW > 40.0f) {
                    const float wavePhase = static_cast<float>(std::sin(now * settings.causticsSpeed * 1.2));
                    const float waveCenterX = rect.left + pillW * 0.5f + wavePhase * (pillW * 0.38f);
                    const float waveW = std::min(75.0f, pillW * 0.40f);
                    const float sOp = intensity * settingsOpacity_;

                    shimmerBrush_->SetStartPoint(D2D1::Point2F(waveCenterX - waveW, rect.top));
                    shimmerBrush_->SetEndPoint(D2D1::Point2F(waveCenterX + waveW, rect.bottom));
                    shimmerBrush_->SetOpacity(sOp);
                    target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), shimmerBrush_.Get());
                }
            }

            // C. Crisp Dual Top & Bottom Liquid Surface Specular Rims
            if (settings.glassSpecular && glassGlossBrush_) {
                // Top rim reflection
                glassGlossBrush_->SetOpacity(settings.topHighlightOpacity * intensity * settingsOpacity_);
                D2D1_RECT_F topGloss = D2D1::RectF(rect.left + radius * 0.75f, rect.top + 0.5f,
                                                  rect.right - radius * 0.75f, rect.top + 1.5f);
                target_->FillRectangle(topGloss, glassGlossBrush_.Get());

                // Bottom subtle meniscus reflection
                glassGlossBrush_->SetOpacity(settings.topHighlightOpacity * 0.35f * intensity * settingsOpacity_);
                D2D1_RECT_F botGloss = D2D1::RectF(rect.left + radius * 0.85f, rect.bottom - 1.5f,
                                                  rect.right - radius * 0.85f, rect.bottom - 0.5f);
                target_->FillRectangle(botGloss, glassGlossBrush_.Get());
                glassGlossBrush_->SetOpacity(1.0f);
            }

            // D. Sub-Pixel Edge Refraction Border
            if (settings.glassRefractionRim && glassBorderBrush_) {
                target_->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f,
                                                  rect.right - 0.5f, rect.bottom - 0.5f),
                                      radius, radius),
                    glassBorderBrush_.Get(), settings.rimThickness);
            }
        } else {
            if (glassBorderBrush_) {
                target_->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f,
                                                  rect.right - 0.5f, rect.bottom - 0.5f),
                                      radius, radius),
                    glassBorderBrush_.Get(), settings.rimThickness * 0.9f);
            }
        }
    }

    void DrawPrivacyDots(const SharedState& state, D2D1_RECT_F rect, double now, const Settings& settings) {
        if (!target_ || !d2dFactory_) return;
        const bool micActive = state.system.micActive;
        const bool camActive = state.system.cameraActive;
        if (!micActive && !camActive) return;

        const float dotRadius = settings.privacyDotSize;
        const float spacing = dotRadius * 2.9f;
        const float rightMargin = 14.0f;
        const float cy = (rect.top + rect.bottom) * 0.5f;

        D2D1_POINT_2F camCenter = D2D1::Point2F(rect.right - rightMargin, cy);
        D2D1_POINT_2F micCenter = camActive 
            ? D2D1::Point2F(rect.right - rightMargin - spacing, cy)
            : D2D1::Point2F(rect.right - rightMargin, cy);

        const float pulsePhase = static_cast<float>(std::sin(now * 3.5));
        const float pulseAlpha = 0.6f + 0.4f * pulsePhase;
        const float pulseRadius = dotRadius + 2.5f + 1.5f * pulsePhase;

        if (camActive) {
            D2D1_COLOR_F camColor = D2D1::ColorF(0.0f, 0.90f, 0.46f, 1.0f);
            if (!greenBrush_) target_->CreateSolidColorBrush(camColor, &greenBrush_);
            
            greenBrush_->SetOpacity(0.28f * pulseAlpha * settingsOpacity_);
            target_->FillEllipse(D2D1::Ellipse(camCenter, pulseRadius, pulseRadius), greenBrush_.Get());
            
            greenBrush_->SetOpacity(0.55f * settingsOpacity_);
            target_->FillEllipse(D2D1::Ellipse(camCenter, dotRadius + 1.2f, dotRadius + 1.2f), greenBrush_.Get());
            
            greenBrush_->SetOpacity(1.0f);
            target_->FillEllipse(D2D1::Ellipse(camCenter, dotRadius, dotRadius), greenBrush_.Get());
            
            if (whiteBrush_) {
                whiteBrush_->SetOpacity(0.85f);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(camCenter.x - 1.0f, camCenter.y - 1.0f), 1.2f, 1.2f), whiteBrush_.Get());
                whiteBrush_->SetOpacity(1.0f);
            }
        }

        if (micActive) {
            D2D1_COLOR_F micColor = D2D1::ColorF(1.0f, 0.57f, 0.0f, 1.0f);
            if (!orangeBrush_) target_->CreateSolidColorBrush(micColor, &orangeBrush_);
            
            orangeBrush_->SetOpacity(0.28f * pulseAlpha * settingsOpacity_);
            target_->FillEllipse(D2D1::Ellipse(micCenter, pulseRadius, pulseRadius), orangeBrush_.Get());
            
            orangeBrush_->SetOpacity(0.55f * settingsOpacity_);
            target_->FillEllipse(D2D1::Ellipse(micCenter, dotRadius + 1.2f, dotRadius + 1.2f), orangeBrush_.Get());
            
            orangeBrush_->SetOpacity(1.0f);
            target_->FillEllipse(D2D1::Ellipse(micCenter, dotRadius, dotRadius), orangeBrush_.Get());
            
            if (whiteBrush_) {
                whiteBrush_->SetOpacity(0.85f);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(micCenter.x - 1.0f, micCenter.y - 1.0f), 1.2f, 1.2f), whiteBrush_.Get());
                whiteBrush_->SetOpacity(1.0f);
            }
        }
    }

    void DrawPill(const SharedState& state, const Settings& settings, const Activity& activity,
                  D2D1_RECT_F rect, float scale, double now) {
        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float w = (rect.right - rect.left) * scale;
        const float h = (rect.bottom - rect.top) * scale;
        rect = D2D1::RectF(cx - w * 0.5f, cy - h * 0.5f, cx + w * 0.5f, cy + h * 0.5f);

        float radius = CalculateCornerRadius(rect.bottom - rect.top, settings);
        
        // Progressive multi-layer soft drop shadow
        if (settings.softShadow && settings.shadowSpreadPx > 0.01f) {
            DrawSoftShadow(rect, radius, settings.shadowSoftness, settings);
        }

        // Dynamic ambient accent glow behind the capsule
        if (settings.accentGlow) {
            DrawAccentGlow(rect, activity, settings, now);
        }

        D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(rect, radius, radius);
        DrawPillSurface(rect, radius, activity.kind, settings, now);

        float contentAlpha = 1.0f;

        if (activity.kind == IslandKind::Progress) {
            DrawProgressRing(rect, state.progress.percent, settings);
        }

        if (activity.kind == IslandKind::BatteryLow) {
            const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 2.0 * 3.14159265 * 2.1));
            redBrush_->SetOpacity(0.45f + 0.45f * pulse);
            target_->DrawRoundedRectangle(pill, redBrush_.Get(), 2.0f);
            redBrush_->SetOpacity(1.0f);
        } else {
            accentBrush_->SetOpacity(activity.kind == IslandKind::Idle ? 0.18f : 0.34f);
            target_->DrawRoundedRectangle(pill, accentBrush_.Get(), 1.0f);
            accentBrush_->SetOpacity(1.0f);
        }

        if (radius > 2.0f) {
            if (whiteBrush_) {
                whiteBrush_->SetOpacity(0.08f * settingsOpacity_);
                target_->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(rect.left + 1, rect.top + 1, rect.right - 1,
                                                  rect.bottom - 1),
                                      std::max(0.0f, radius - 1.0f), std::max(0.0f, radius - 1.0f)),
                    whiteBrush_.Get(), 1.0f);
                whiteBrush_->SetOpacity(1.0f);
            }
        }

        bool isExpanded = (rect.bottom - rect.top > (settings.collapsedHeight * settings.sizeScale + std::max(12.0f, (settings.expandedHeight - settings.collapsedHeight) * settings.sizeScale * 0.30f)));
        if (isExpanded) {
            if (activity.kind == IslandKind::Media && state.media.available) {
                DrawMedia(state, rect, now);
            } else {
                DrawExpandedDashboard(state, rect, settings, now, scale);
            }
        } else {
            switch (activity.kind) {
                case IslandKind::Media:
                    DrawMedia(state, rect, now);
                    break;
                case IslandKind::Clipboard:
                    DrawClipboard(state, rect, settings);
                    break;
                case IslandKind::Notification:
                    DrawNotification(state, rect, settings);
                    break;
                case IslandKind::Volume:
                    DrawVolume(state, rect, settings);
                    break;
                case IslandKind::CapsLock:
                    DrawCapsLock(state, rect, settings);
                    break;
                case IslandKind::Device:
                    DrawDevice(state, rect, settings);
                    break;
                case IslandKind::BatteryLow:
                    DrawBattery(state, rect, settings);
                    break;
                case IslandKind::Progress:
                    DrawProgress(state, rect, settings);
                    break;
                case IslandKind::Recording:
                    DrawVoiceRecording(state, rect, now, settings);
                    break;
                case IslandKind::Idle:
                default:
                    DrawCollapsedIdle(state, rect, settings, now);
                    break;
            }
        }

        // Apple-style privacy indicator dots
        DrawPrivacyDots(state, rect, now, settings);

        if (contentAlpha < 0.99f && textBrush_ && mutedBrush_) {
            textBrush_->SetOpacity(0.90f);
            mutedBrush_->SetOpacity(0.58f);
        }
    }

    void DrawFittedTitle(const std::wstring& text, D2D1_RECT_F rect, ID2D1Brush* brush, float baseFontSize, float scale) {
        if (text.empty() || !dwriteFactory_ || !target_ || !brush) return;
        float availW = rect.right - rect.left;
        float availH = rect.bottom - rect.top;
        if (availW <= 10.0f || availH <= 10.0f) return;

        float fontSize = baseFontSize * scale;
        ComPtr<IDWriteTextFormat> fmt;
        HRESULT hr = dwriteFactory_->CreateTextFormat(
            L"Segoe UI Variable Display", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &fmt);
        if (FAILED(hr) || !fmt) return;

        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        ComPtr<IDWriteTextLayout> layout;
        hr = dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.length()), fmt.Get(), 2000.0f, availH, &layout);
        if (SUCCEEDED(hr) && layout) {
            DWRITE_TEXT_METRICS metrics;
            if (SUCCEEDED(layout->GetMetrics(&metrics))) {
                if (metrics.widthIncludingTrailingWhitespace > availW && metrics.widthIncludingTrailingWhitespace > 1.0f) {
                    float factor = availW / metrics.widthIncludingTrailingWhitespace;
                    fontSize = std::max(fontSize * factor * 0.96f, 10.0f * scale);
                    dwriteFactory_->CreateTextFormat(
                        L"Segoe UI Variable Display", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                        DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &fmt);
                    if (fmt) {
                        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    }
                }
            }
        }
        if (fmt) {
            target_->DrawText(text.c_str(), static_cast<UINT32>(text.length()), fmt.Get(), rect, brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }

    void DrawCenteredGlyph(const std::wstring& glyph, D2D1_RECT_F badgeRect, IDWriteTextFormat* format, ID2D1Brush* brush, bool isEmoji = true) {
        if (!target_ || !dwriteFactory_ || !format || !brush || glyph.empty()) return;
        const float w = badgeRect.right - badgeRect.left;
        const float h = badgeRect.bottom - badgeRect.top;
        if (w <= 0.0f || h <= 0.0f) return;

        ComPtr<IDWriteTextLayout> layout;
        HRESULT hr = dwriteFactory_->CreateTextLayout(glyph.c_str(), static_cast<UINT32>(glyph.length()), format, w, h, &layout);
        if (SUCCEEDED(hr) && layout) {
            layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            DWRITE_TEXT_METRICS tm = {};
            layout->GetMetrics(&tm);

            // Compute exact geometric center inside the badge
            float glyphCenterX = tm.left + tm.width * 0.5f;
            float glyphCenterY = tm.top + tm.height * 0.5f;
            float targetCenterX = w * 0.5f;
            float targetCenterY = h * 0.5f;

            float offsetX = targetCenterX - glyphCenterX;
            float offsetY = targetCenterY - glyphCenterY;

            // Optical compensation for Segoe UI Emoji glyphs
            if (isEmoji) {
                offsetY += 0.5f;
            }

            target_->PushAxisAlignedClip(badgeRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
            D2D1_POINT_2F pt = D2D1::Point2F(badgeRect.left + offsetX, badgeRect.top + offsetY);
            target_->DrawTextLayout(pt, layout.Get(), brush, isEmoji ? (D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP) : D2D1_DRAW_TEXT_OPTIONS_CLIP);
            target_->PopAxisAlignedClip();
        }
    }

    void DrawFittedLine(const std::wstring& text, D2D1_RECT_F rect, ID2D1Brush* brush, float baseFontSize, float scale,
                        DWRITE_TEXT_ALIGNMENT align,
                        DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL, bool wrap = false) {
        if (text.empty() || !dwriteFactory_ || !target_ || !brush) return;
        float availW = rect.right - rect.left;
        float availH = rect.bottom - rect.top;
        if (availW <= 4.0f || availH <= 4.0f) return;

        float fontSize = baseFontSize * scale;
        ComPtr<IDWriteTextFormat> fmt;
        HRESULT hr = dwriteFactory_->CreateTextFormat(
            L"Segoe UI Variable Text", nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &fmt);
        if (FAILED(hr) || !fmt) return;

        if (wrap) {
            fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
            fmt->SetTextAlignment(align);
            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        } else {
            fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            fmt->SetTextAlignment(align);
            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

            ComPtr<IDWriteTextLayout> layout;
            hr = dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.length()), fmt.Get(), 2000.0f, availH, &layout);
            if (SUCCEEDED(hr) && layout) {
                DWRITE_TEXT_METRICS metrics;
                if (SUCCEEDED(layout->GetMetrics(&metrics))) {
                    if (metrics.widthIncludingTrailingWhitespace > availW && metrics.widthIncludingTrailingWhitespace > 1.0f) {
                        float factor = availW / metrics.widthIncludingTrailingWhitespace;
                        fontSize = std::max(fontSize * factor * 0.95f, 7.5f * scale);
                        dwriteFactory_->CreateTextFormat(
                            L"Segoe UI Variable Text", nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                            DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &fmt);
                        if (fmt) {
                            fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                            fmt->SetTextAlignment(align);
                            fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                        }
                    }
                }
            }
        }
        if (fmt) {
            target_->DrawText(text.c_str(), static_cast<UINT32>(text.length()), fmt.Get(), rect, brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }

    void DrawFittedLine(const std::wstring& text, D2D1_RECT_F rect, ID2D1Brush* brush, float baseFontSize, float scale, bool wrap = false) {
        DrawFittedLine(text, rect, brush, baseFontSize, scale, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_FONT_WEIGHT_NORMAL, wrap);
    }

    static void GetWeatherIconAndText(int code, std::wstring& icon, std::wstring& text, bool isTurkish = false) {
        switch (code) {
            case 113: icon = L"☀️"; text = isTurkish ? L"Güneşli" : L"Sunny"; break;
            case 116: icon = L"⛅"; text = isTurkish ? L"Parçalı Bulutlu" : L"Partly Cloudy"; break;
            case 119: case 122: icon = L"☁️"; text = isTurkish ? L"Bulutlu" : L"Cloudy"; break;
            case 143: case 248: case 260: icon = L"🌫️"; text = isTurkish ? L"Sisli" : L"Foggy"; break;
            case 200: case 386: case 389: case 392: case 395: icon = L"⛈️"; text = isTurkish ? L"Fırtına" : L"Thunderstorm"; break;
            case 176: case 263: case 266: case 281: case 284: case 293: case 296: case 299:
            case 302: case 305: case 308: case 311: case 314: case 353: case 356: case 359:
                icon = L"🌧️";
                text = isTurkish ? L"Yağmurlu" : L"Rain";
                break;
            case 179: case 182: case 185: case 227: case 230: case 317: case 320: case 323:
            case 326: case 329: case 332: case 335: case 338: case 350: case 362: case 365:
            case 368: case 371:
                icon = L"❄️";
                text = isTurkish ? L"Karlı" : L"Snow";
                break;
            default: icon = L"🌡️"; text = isTurkish ? L"Açık" : L"Clear"; break;
        }
    }

    static int GetDaysInMonth(int year, int month) {
        if (month == 2) {
            bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            return leap ? 29 : 28;
        }
        if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
        return 31;
    }

    static int GetDayOfWeek(int year, int month, int day) {
        if (month < 3) { month += 12; year -= 1; }
        int k = year % 100;
        int j = year / 100;
        int h = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
        return (h + 6) % 7;
    }

    static std::wstring GetSystemUserName() {
        wchar_t nameBuf[256] = {};
        DWORD size = ARRAYSIZE(nameBuf);
        if (GetUserNameW(nameBuf, &size) && wcslen(nameBuf) > 0) {
            std::wstring name = nameBuf;
            if (!name.empty() && name[0] >= L'a' && name[0] <= L'z') {
                name[0] = towupper(name[0]);
            }
            return name;
        }
        wchar_t envBuf[256] = {};
        if (GetEnvironmentVariableW(L"USERNAME", envBuf, ARRAYSIZE(envBuf)) > 0) {
            std::wstring name = envBuf;
            if (!name.empty() && name[0] >= L'a' && name[0] <= L'z') {
                name[0] = towupper(name[0]);
            }
            return name;
        }
        return L"User";
    }

    static void GetLivingInsight(const SharedState& state, const Settings& settings, double now,
                                 const SYSTEMTIME& local, bool hasWeather,
                                 std::wstring& outTag, std::wstring& outIcon, std::wstring& outMsg) {
        bool tr = IsTurkish(settings.language);
        int hour = local.wHour;
        uint64_t uptimeMin = (GetTickCount64() / 1000) / 60;
        int weatherCode = state.weather.weatherCode;
        double remHyd = std::max(0.0, state.assistant.nextHydrationTime - now);
        double remEye = std::max(0.0, state.assistant.nextEyeRestTime - now);

        // Smart dynamic seed that rotates gracefully
        uint32_t seed = static_cast<uint32_t>(now / 25.0) + static_cast<uint32_t>(local.wDay * 100 + local.wHour * 10);

        // Priority 1: Late Night Context
        if (hour >= 23 || hour <= 4) {
            outTag = tr ? L"🌙 GECE VAKTİ" : L"🌙 LATE HOURS";
            outIcon = L"🦉";
            if (seed % 2 == 0) {
                outMsg = tr ? L"Gece mesaisi mi yapıyorsun? Gözlerini dinlendirmeyi ve uyku düzenini korumayı unutma 💤"
                            : L"Late night work session? Remember to rest your eyes and protect your sleep 💤";
            } else {
                outMsg = tr ? L"Günün tüm yorgunluğunu geride bırakma vakti. İyi geceler ve huzurlu dinlenmeler! ✨"
                            : L"Time to wind down today's efforts. Good night and rest well! ✨";
            }
            return;
        }

        // Priority 2: Extended PC Uptime Alert
        if (uptimeMin >= 150) {
            outTag = tr ? L"🧘 UZUN OTURUM UYARISI" : L"🧘 EXTENDED SESSION";
            outIcon = L"🚶";
            outMsg = tr ? L"2.5 saatten fazladır kesintisiz bilgisayar başındasın! Kısa bir kalkış, esneme ve su molası harika gelir 🌿"
                        : L"You've been at your PC for over 2.5 hours! A 5-minute stretch and water break will refresh you 🌿";
            return;
        }

        // Priority 3: Weather-Driven Intelligent Contexts
        if (hasWeather && (seed % 3 == 0)) {
            if (weatherCode == 176 || weatherCode == 263 || weatherCode == 266 || weatherCode == 293 ||
                weatherCode == 296 || weatherCode == 299 || weatherCode == 302 || weatherCode == 305 ||
                weatherCode == 308 || weatherCode == 311 || weatherCode == 314 || weatherCode == 353 ||
                weatherCode == 356 || weatherCode == 359) {
                outTag = tr ? L"🌧️ YAĞMURLU GÜN" : L"🌧️ RAINY DAY";
                outIcon = L"☔";
                outMsg = tr ? L"Dışarıda yağmur var! Sıcak bir fincan kahve eşliğinde üretkenliğin ve odaklanmanın tadını çıkar ☕"
                            : L"Rain is falling outside! Enjoy a cozy, productive session with a warm cup of coffee ☕";
                return;
            }
            if (weatherCode == 179 || weatherCode == 182 || weatherCode == 185 || weatherCode == 227 ||
                weatherCode == 230 || weatherCode == 317 || weatherCode == 320 || weatherCode == 323 ||
                weatherCode == 326 || weatherCode == 329 || weatherCode == 332 || weatherCode == 335 ||
                weatherCode == 338 || weatherCode == 350 || weatherCode == 362 || weatherCode == 365 ||
                weatherCode == 368 || weatherCode == 371) {
                outTag = tr ? L"❄️ KAR VE KIŞ" : L"❄️ WINTER CHILL";
                outIcon = L"🧣";
                outMsg = tr ? L"Dışarısı karlı ve serin! Sıcak kalmaya dikkat et ve ortamı ferah tutmayı unutma ❄️"
                            : L"It's chilly and snowy outside! Stay warm and keep your focus environment fresh ❄️";
                return;
            }
            if (weatherCode == 113 && hour >= 8 && hour <= 17) {
                outTag = tr ? L"☀️ GÜNEŞLİ ATMOSFER" : L"☀️ SUNNY VIBES";
                outIcon = L"☀️";
                outMsg = tr ? L"Güneşli ve pırıl pırıl bir hava var! Enerjini yüksek tut, harika bir gün seni bekliyor ✨"
                            : L"Crisp sunny weather today! Keep your spirits high, a wonderful day is unfolding ✨";
                return;
            }
        }

        // Priority 4: Health & Wellness Timers
        if (settings.hydrationIntervalMinutes > 0 && remHyd < 180.0 && (seed % 3 == 1)) {
            outTag = tr ? L"💧 SAĞLIK & SU TAKİBİ" : L"💧 HYDRATION CHECK";
            outIcon = L"💧";
            outMsg = tr ? L"Su içme vaktin yaklaşıyor! Masanda suyun hazır mı? Yeterli su içmek zihni daima keskin tutar 🌿"
                        : L"Time for a sip of water is approaching! Hydration keeps your brain sharp and energized 🌿";
            return;
        }

        if (settings.eyeRestReminder && remEye < 120.0 && (seed % 3 == 2)) {
            outTag = tr ? L"👁️ 20-20-20 GÖZ SAĞLIĞI" : L"👁️ 20-20-20 EYE REST";
            outIcon = L"👁️";
            outMsg = tr ? L"20-20-20 Kuralı: 20 fit (6m) uzağa 20 saniye bakarak göz kaslarını gevşetmeyi unutma ✨"
                        : L"20-20-20 Rule: Look 20 feet away for 20 seconds to release optical strain ✨";
            return;
        }

        // Priority 5: Time-of-Day & Conversational Companion Motivation
        if (hour >= 5 && hour < 12) {
            // Morning
            struct MsgItem { const wchar_t* tagTr; const wchar_t* msgTr; const wchar_t* tagEn; const wchar_t* msgEn; const wchar_t* icon; };
            static const MsgItem kMorning[] = {
                {L"🌅 SABAH MOTİVASYONU", L"Güne harika bir başlangıç yap! Bugünkü en önemli hedefini belirledin mi? 🎯",
                 L"🌅 MORNING FOCUS", L"Start your day strong! Have you chosen your main milestone for today? 🎯", L"🌅"},
                {L"⚡ GÜNE BAŞLARKEN", L"Sabahın ferahlığı zihnini açsın. Derin bir nefes al ve yapacaklarına odaklan 🚀",
                 L"⚡ MORNING FRESHNESS", L"Let the morning clarity guide you. Breathe deep and focus on your goals 🚀", L"⚡"},
                {L"💎 YAŞAYAN KAPSÜL", L"Kapsül arka planda tüm sistemini senin için hazır tutuyor. Harika bir gün olsun! ✨",
                 L"💎 LIVING COMPANION", L"aegisCapsule is running smoothly in sync with your desktop. Have a great day! ✨", L"💎"},
                {L"🎯 GÜNLÜK HEDEF", L"Küçük adımlar büyük başarılar getirir. İlk işini bitirmek için harika bir an 💡",
                 L"🎯 DAILY GOAL", L"Small steady steps lead to big victories. A perfect moment to tackle task #1 💡", L"🎯"}
            };
            const auto& item = kMorning[seed % ARRAYSIZE(kMorning)];
            outTag = tr ? item.tagTr : item.tagEn;
            outMsg = tr ? item.msgTr : item.msgEn;
            outIcon = item.icon;
            return;
        } else if (hour >= 12 && hour < 18) {
            // Midday & Afternoon
            struct MsgItem { const wchar_t* tagTr; const wchar_t* msgTr; const wchar_t* tagEn; const wchar_t* msgEn; const wchar_t* icon; };
            static const MsgItem kAfternoon[] = {
                {L"🌿 GÜNÜN TEMPOSU", L"Harika bir ivme yakaladın! Omuzlarını serbest bırak ve dik duruşunu koru 🧘",
                 L"🌿 MIDDAY MOMENTUM", L"Great momentum today! Roll your shoulders back and maintain good posture 🧘", L"🧘"},
                {L"☕ ENERJİ MOLASI", L"Öğleden sonra zihnini tazelemek için 2 dakikalık kısa bir mola ve su çok iyi gelir ☕",
                 L"☕ RECHARGE BREAK", L"A quick 2-minute stretch and water break will give you a crisp afternoon boost ☕", L"☕"},
                {L"🍅 ODAKLANMA ZAMANI", L"Derin çalışmaya girmek için 25 dakikalık Pomodoro sayacını tek tıkla başlatabilirsin 🍅",
                 L"🍅 FOCUS SESSION", L"Ready for uninterrupted deep work? Start a 25-minute Pomodoro focus session anytime 🍅", L"🍅"},
                {L"✨ KİŞİSEL ASİSTAN", L"Nasılsın, her şey yolunda mı? İşlerin tıkırında ilerliyor, tempoyu koru! 💫",
                 L"✨ COMPANION PULSE", L"How is your workflow going? Everything is on track, keep up the great pace! 💫", L"✨"},
                {L"📝 HIZLI NOTLAR", L"Aklına gelen harika fikirleri panodan tek tıkla Hızlı Notlar sekmesine kaydedebilirsin 📝",
                 L"📝 QUICK NOTES", L"Capture instant ideas and clipboard snippets right into Quick Notes anytime 📝", L"📝"}
            };
            const auto& item = kAfternoon[seed % ARRAYSIZE(kAfternoon)];
            outTag = tr ? item.tagTr : item.tagEn;
            outMsg = tr ? item.msgTr : item.msgEn;
            outIcon = item.icon;
            return;
        } else {
            // Evening
            struct MsgItem { const wchar_t* tagTr; const wchar_t* msgTr; const wchar_t* tagEn; const wchar_t* msgEn; const wchar_t* icon; };
            static const MsgItem kEvening[] = {
                {L"🌆 AKŞAM REFLEKSİYONU", L"Günün kapanışına doğru ilerliyorsun. Bugün başardığın güzel işler için kendine teşekkür et 🌟",
                 L"🌆 EVENING REFLECTION", L"Heading into the evening! Take a moment to acknowledge all you accomplished today 🌟", L"🌟"},
                {L"🌿 HUZURLU AKŞAMLAR", L"Masanı ve zihnini rahatlat. Dinlenmek de üretkenliğin vazgeçilmez bir parçasıdır 🌿",
                 L"🌿 EVENING CALM", L"Unwind your desk and mind. Rest is an essential pillar of true productivity 🌿", L"🌿"},
                {L"🎯 YARINA HAZIRLIK", L"Yarının planını kafanda netleştirmek geceyi huzurlu ve zihnini berrak kılar 💡",
                 L"🎯 TOMORROW'S VISION", L"A quick mental outline for tomorrow lets you relax with complete peace of mind 💡", L"💡"},
                {L"💎 YAŞAYAN ASİSTAN", L"Kapsül her an seninle. Sağlığın ve üretkenliğin daima en yüksek standartta 💎",
                 L"💎 LIVING COMPANION", L"Your companion is always by your side, keeping your workflow effortlessly fluid 💎", L"💎"}
            };
            const auto& item = kEvening[seed % ARRAYSIZE(kEvening)];
            outTag = tr ? item.tagTr : item.tagEn;
            outMsg = tr ? item.msgTr : item.msgEn;
            outIcon = item.icon;
            return;
        }
    }

    void DrawLivingBriefingDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings,
                                     double now, float scale, const SYSTEMTIME& local, bool hasWeather,
                                     const std::wstring& wIcon, const std::wstring& wText) {
        bool tr = IsTurkish(settings.language);
        std::wstring userName = GetSystemUserName();
        const float totalW = (rect.right - rect.left);
        const float totalH = (rect.bottom - rect.top);

        // 1. Top Header Bar: Personalized Greeting (Left) + Glass Clock & Date Tag (Right)
        std::wstring greeting;
        int hour = local.wHour;
        if (hour >= 5 && hour < 12) {
            greeting = tr ? (L"Günaydın, " + userName + L" ☀️") : (L"Good morning, " + userName + L" ☀️");
        } else if (hour >= 12 && hour < 18) {
            greeting = tr ? (L"Tünaydın, " + userName + L" 🌤️") : (L"Good afternoon, " + userName + L" 🌤️");
        } else if (hour >= 18 && hour < 23) {
            greeting = tr ? (L"İyi akşamlar, " + userName + L" 🌆") : (L"Good evening, " + userName + L" 🌆");
        } else {
            greeting = tr ? (L"İyi geceler, " + userName + L" 🌙") : (L"Good night, " + userName + L" 🌙");
        }

        wchar_t dateBuf[64] = {};
        if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"ddd, d MMMM", dateBuf, ARRAYSIZE(dateBuf), nullptr)) {
            swprintf_s(dateBuf, L"%02d/%02d", local.wDay, local.wMonth);
        }
        wchar_t timeBuf[16] = {};
        swprintf_s(timeBuf, L"%02d:%02d", local.wHour, local.wMinute);
        std::wstring dateTimeTag = std::wstring(timeBuf) + L" \u2022 " + dateBuf;

        const float headerTop = rect.top + 12.0f * scale;
        const float headerH = 24.0f * scale;
        const float datePillW = std::clamp(totalW * 0.38f, 130.0f * scale, 190.0f * scale);

        D2D1_RECT_F greetingRect = D2D1::RectF(rect.left + 18.0f * scale, headerTop,
                                               rect.right - datePillW - 40.0f * scale, headerTop + headerH);
        textBrush_->SetOpacity(0.96f);
        DrawFittedTitle(greeting, greetingRect, textBrush_.Get(), 14.5f, scale);

        D2D1_RECT_F datePillRect = D2D1::RectF(rect.right - 36.0f * scale - datePillW, headerTop,
                                              rect.right - 36.0f * scale, headerTop + headerH);
        ID2D1SolidColorBrush* cardB = cardBgBrush_ ? cardBgBrush_.Get() : textBrush_.Get();
        ID2D1SolidColorBrush* borderB = cardBorderBrush_ ? cardBorderBrush_.Get() : mutedBrush_.Get();
        target_->FillRoundedRectangle(D2D1::RoundedRect(datePillRect, 6.0f * scale, 6.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(datePillRect, 6.0f * scale, 6.0f * scale), borderB, 1.0f * scale);

        mutedBrush_->SetOpacity(0.85f);
        DrawFittedLine(dateTimeTag, datePillRect, mutedBrush_.Get(), 10.5f, scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD, false);
        mutedBrush_->SetOpacity(1.0f);

        // 2. Middle Hero Section: Living Companion Intelligent Dynamic Insight Bubble
        std::wstring outTag, outIcon, outMsg;
        GetLivingInsight(state, settings, now, local, hasWeather, outTag, outIcon, outMsg);

        const float bubbleTop = headerTop + headerH + 8.0f * scale;
        const float bubbleH = std::clamp(totalH * 0.42f, 54.0f * scale, 76.0f * scale);
        D2D1_RECT_F bubbleRect = D2D1::RectF(rect.left + 16.0f * scale, bubbleTop,
                                             rect.right - 36.0f * scale, bubbleTop + bubbleH);

        target_->FillRoundedRectangle(D2D1::RoundedRect(bubbleRect, 10.0f * scale, 10.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(bubbleRect, 10.0f * scale, 10.0f * scale), borderB, 1.0f * scale);

        if (accentBrush_) {
            accentBrush_->SetOpacity(0.20f * settingsOpacity_);
            target_->DrawRoundedRectangle(D2D1::RoundedRect(bubbleRect, 10.0f * scale, 10.0f * scale), accentBrush_.Get(), 1.0f * scale);
            accentBrush_->SetOpacity(1.0f);
        }

        const float iconBoxSz = bubbleH - 14.0f * scale;
        D2D1_RECT_F iconBox = D2D1::RectF(bubbleRect.left + 8.0f * scale, bubbleRect.top + 7.0f * scale,
                                          bubbleRect.left + 8.0f * scale + iconBoxSz, bubbleRect.top + 7.0f * scale + iconBoxSz);
        DrawCenteredGlyph(outIcon, iconBox, hugeTextFormat_.Get(), textBrush_.Get(), true);

        const float textStartX = iconBox.right + 10.0f * scale;
        const float textEndX = bubbleRect.right - 10.0f * scale;

        D2D1_RECT_F tagRect = D2D1::RectF(textStartX, bubbleRect.top + 7.0f * scale, textEndX, bubbleRect.top + 23.0f * scale);
        if (accentBrush_) {
            accentBrush_->SetOpacity(0.90f);
            DrawFittedLine(outTag, tagRect, accentBrush_.Get(), 10.5f, scale, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_FONT_WEIGHT_BOLD, false);
            accentBrush_->SetOpacity(1.0f);
        }

        D2D1_RECT_F msgRect = D2D1::RectF(textStartX, bubbleRect.top + 23.0f * scale, textEndX, bubbleRect.bottom - 6.0f * scale);
        textBrush_->SetOpacity(0.94f);
        DrawFittedLine(outMsg, msgRect, textBrush_.Get(), 11.5f, scale, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_FONT_WEIGHT_NORMAL, true);
        textBrush_->SetOpacity(1.0f);

        // 3. Bottom Row: 3 Quick-Glance Interactive Micro Chips
        const float chipsTop = bubbleRect.bottom + 8.0f * scale;
        const float chipsBottom = rect.bottom - 12.0f * scale;
        const float availW = (rect.right - 36.0f * scale) - (rect.left + 16.0f * scale);
        const float gap = 8.0f * scale;
        const float chipW = (availW - gap * 2.0f) / 3.0f;

        // Chip 1: Weather Glance
        D2D1_RECT_F chip1Rect = D2D1::RectF(rect.left + 16.0f * scale, chipsTop,
                                            rect.left + 16.0f * scale + chipW, chipsBottom);
        target_->FillRoundedRectangle(D2D1::RoundedRect(chip1Rect, 8.0f * scale, 8.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(chip1Rect, 8.0f * scale, 8.0f * scale), borderB, 1.0f * scale);

        wchar_t wTempStr[32] = {};
        if (hasWeather) swprintf_s(wTempStr, L"%.0f\x00B0", state.weather.temperature);
        else wcscpy_s(wTempStr, L"--\x00B0");
        std::wstring chip1Text = wIcon + L" " + wTempStr + L" " + (wText.empty() ? (tr ? L"Açık" : L"Clear") : wText);
        mutedBrush_->SetOpacity(0.90f);
        DrawFittedLine(chip1Text, chip1Rect, mutedBrush_.Get(), 10.5f, scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD, false);

        // Chip 2: Active PC Uptime Glance
        D2D1_RECT_F chip2Rect = D2D1::RectF(rect.left + 16.0f * scale + chipW + gap, chipsTop,
                                            rect.left + 16.0f * scale + chipW * 2.0f + gap, chipsBottom);
        target_->FillRoundedRectangle(D2D1::RoundedRect(chip2Rect, 8.0f * scale, 8.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(chip2Rect, 8.0f * scale, 8.0f * scale), borderB, 1.0f * scale);

        uint64_t upMin = (GetTickCount64() / 1000) / 60;
        uint64_t upH = upMin / 60;
        uint64_t upM = upMin % 60;
        wchar_t upBuf[64] = {};
        if (tr) swprintf_s(upBuf, L"⏱️ %llusa %lludk Aktif", upH, upM);
        else swprintf_s(upBuf, L"⏱️ %lluh %llum Active", upH, upM);
        DrawFittedLine(upBuf, chip2Rect, mutedBrush_.Get(), 10.5f, scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD, false);

        // Chip 3: Wellness / Focus Quick Status
        D2D1_RECT_F chip3Rect = D2D1::RectF(rect.left + 16.0f * scale + (chipW + gap) * 2.0f, chipsTop,
                                            rect.right - 36.0f * scale, chipsBottom);
        target_->FillRoundedRectangle(D2D1::RoundedRect(chip3Rect, 8.0f * scale, 8.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(chip3Rect, 8.0f * scale, 8.0f * scale), borderB, 1.0f * scale);

        std::wstring chip3Text;
        if (state.pomodoro.state == PomodoroState::Working) {
            int rM = state.pomodoro.remainingSeconds / 60;
            int rS = state.pomodoro.remainingSeconds % 60;
            wchar_t pBuf[32] = {};
            swprintf_s(pBuf, L"🍅 %02d:%02d", rM, rS);
            chip3Text = pBuf;
        } else if (settings.hydrationIntervalMinutes > 0) {
            int remMin = static_cast<int>(std::max(0.0, state.assistant.nextHydrationTime - now) / 60.0);
            wchar_t hydBuf[32] = {};
            if (tr) swprintf_s(hydBuf, L"💧 Su: %ddk", remMin);
            else swprintf_s(hydBuf, L"💧 Water: %dm", remMin);
            chip3Text = hydBuf;
        } else {
            chip3Text = tr ? L"🌿 Kapsül Canlı" : L"🌿 Living Capsule";
        }
        DrawFittedLine(chip3Text, chip3Rect, mutedBrush_.Get(), 10.5f, scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD, false);
        mutedBrush_->SetOpacity(1.0f);
    }

    void DrawCalendarDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, const SYSTEMTIME& local) {
        (void)state; (void)now; // state and now unused in calendar; settings IS used below
        const float totalW = (rect.right - rect.left);
        const float totalH = (rect.bottom - rect.top);
        const float leftW = std::clamp(totalW * 0.24f, 75.0f, 130.0f);
        
        ID2D1SolidColorBrush* calBg = cardBgBrush_ ? cardBgBrush_.Get() : textBrush_.Get();
        D2D1_RECT_F leftBlock = D2D1::RectF(rect.left + 16.0f * scale, rect.top + 14.0f * scale,
                                            rect.left + 16.0f * scale + leftW, rect.bottom - 16.0f * scale);
        target_->FillRoundedRectangle(D2D1::RoundedRect(leftBlock, 12.0f * scale, 12.0f * scale), calBg);
        
        ID2D1SolidColorBrush* calHeader = calHeaderBrush_ ? calHeaderBrush_.Get() : textBrush_.Get();
        
        wchar_t monthName[32] = {};
        if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"MMMM", monthName, ARRAYSIZE(monthName), nullptr)) {
            // Fallback: manual month names if locale API fails
            bool isTr = IsTurkish(settings.language);
            static const wchar_t* kMonthsTr[] = {L"OCAK",L"ŞUBAT",L"MART",L"NİSAN",L"MAYIS",L"HAZİRAN",
                                                   L"TEMMUZ",L"AĞUSTOS",L"EYLÜL",L"EKİM",L"KASIM",L"ARALIK"};
            static const wchar_t* kMonthsEn[] = {L"JANUARY",L"FEBRUARY",L"MARCH",L"APRIL",L"MAY",L"JUNE",
                                                   L"JULY",L"AUGUST",L"SEPTEMBER",L"OCTOBER",L"NOVEMBER",L"DECEMBER"};
            int mIdx = std::max(1, std::min(12, static_cast<int>(local.wMonth))) - 1;
            wcscpy_s(monthName, isTr ? kMonthsTr[mIdx] : kMonthsEn[mIdx]);
        } else {
            for (int i = 0; monthName[i]; ++i) monthName[i] = towupper(monthName[i]);
        }
        
        if (wcslen(monthName) > 0 && boldTextFormat_ && calHeader) {
            target_->DrawText(monthName, static_cast<UINT32>(wcslen(monthName)), boldTextFormat_.Get(),
                               D2D1::RectF(leftBlock.left, leftBlock.top + 6.0f * scale, leftBlock.right, leftBlock.top + 24.0f * scale),
                               calHeader, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        wchar_t yearStr[16] = {};
        swprintf_s(yearStr, L"%d", local.wYear);
        if (wcslen(yearStr) > 0 && boldTextFormat_ && mutedBrush_) {
            mutedBrush_->SetOpacity(0.45f);
            target_->DrawText(yearStr, static_cast<UINT32>(wcslen(yearStr)), boldTextFormat_.Get(),
                               D2D1::RectF(leftBlock.left, leftBlock.top + 20.0f * scale, leftBlock.right, leftBlock.top + 38.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            mutedBrush_->SetOpacity(1.0f);
        }

        wchar_t dayStr[16] = {};
        swprintf_s(dayStr, L"%d", local.wDay);
        if (wcslen(dayStr) > 0 && hugeTextFormat_ && textBrush_) {
            textBrush_->SetOpacity(0.96f);
            target_->DrawText(dayStr, static_cast<UINT32>(wcslen(dayStr)), hugeTextFormat_.Get(),
                               D2D1::RectF(leftBlock.left, leftBlock.top + 32.0f * scale, leftBlock.right, leftBlock.top + 82.0f * scale),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            textBrush_->SetOpacity(1.0f);
        }

        wchar_t weekdayName[32] = {};
        if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"dddd", weekdayName, ARRAYSIZE(weekdayName), nullptr)) {
            // Fallback: manual weekday names if locale API fails
            bool isTr = IsTurkish(settings.language);
            static const wchar_t* kDaysTr[] = {L"Pazar",L"Pazartesi",L"Salı",L"Çarşamba",L"Perşembe",L"Cuma",L"Cumartesi"};
            static const wchar_t* kDaysEn[] = {L"Sunday",L"Monday",L"Tuesday",L"Wednesday",L"Thursday",L"Friday",L"Saturday"};
            wcscpy_s(weekdayName, isTr ? kDaysTr[local.wDayOfWeek % 7] : kDaysEn[local.wDayOfWeek % 7]);
        }
        mutedBrush_->SetOpacity(0.75f);
        if (wcslen(weekdayName) > 0 && boldTextFormat_) {
            target_->DrawText(weekdayName, static_cast<UINT32>(wcslen(weekdayName)), boldTextFormat_.Get(),
                               D2D1::RectF(leftBlock.left, leftBlock.bottom - 22.0f * scale, leftBlock.right, leftBlock.bottom),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        mutedBrush_->SetOpacity(1.0f);

        // Right Grid (Fully Dynamic Sizing & Overflow Protection)
        int startDay = GetDayOfWeek(local.wYear, local.wMonth, 1);
        int totalDays = GetDaysInMonth(local.wYear, local.wMonth);
        int totalRows = std::max(1, 1 + (startDay + totalDays + 6) / 7); // guard against zero for rowH division
        const float gridStart = leftBlock.right + 18.0f * scale;
        const float gridEnd = rect.right - 36.0f * scale;
        const float colW = (gridEnd > gridStart + 70.0f) ? (gridEnd - gridStart) / 7.0f : 28.0f * scale;
        const float gridTop = rect.top + 14.0f * scale;
        const float gridBottom = rect.bottom - 14.0f * scale;
        const float rowH = (gridBottom > gridTop + 50.0f) ? (gridBottom - gridTop) / static_cast<float>(totalRows) : 16.0f * scale;

        bool tr = IsTurkish(settings.language);
        const wchar_t* daysEn[] = {L"Su", L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa"};
        // Standard Turkish day abbreviations: Paz/Pzt/Sal/Çar/Per/Cum/Cmt
        const wchar_t* daysTr[] = {L"Paz", L"Pzt", L"Sal", L"Çar", L"Per", L"Cum", L"Cmt"};
        const wchar_t** days = tr ? daysTr : daysEn;
        
        for (int i = 0; i < 7; ++i) {
            D2D1_RECT_F cell = D2D1::RectF(gridStart + i * colW, gridTop, gridStart + (i+1)*colW, gridTop + rowH);
            ID2D1SolidColorBrush* brush = (i == 0 || i == 6) ? calHeader : mutedBrush_.Get();
            target_->DrawText(days[i], static_cast<UINT32>(wcslen(days[i])), boldTextFormat_.Get(), cell, brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        
        int row = 1;
        int col = startDay;
        ID2D1SolidColorBrush* weekendB = weekendBrush_ ? weekendBrush_.Get() : calHeader;

        for (int d = 1; d <= totalDays; ++d) {
            D2D1_RECT_F cell = D2D1::RectF(gridStart + col * colW, gridTop + row * rowH + 1.0f * scale, 
                                           gridStart + (col+1)*colW, gridTop + (row+1)*rowH + 1.0f * scale);
            
            wchar_t dBuf[8] = {};
            swprintf_s(dBuf, L"%d", d);
            UINT32 dLen = static_cast<UINT32>(wcslen(dBuf));

            if (d == local.wDay) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cell.left + colW*0.5f, cell.top + rowH*0.5f), std::min(colW, rowH)*0.46f, std::min(colW, rowH)*0.46f), calHeader);
                textBrush_->SetOpacity(1.0f);
                target_->DrawText(dBuf, dLen, boldTextFormat_.Get(), cell, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            } else {
                if (col == 0 || col == 6) {
                    target_->DrawText(dBuf, dLen, boldTextFormat_.Get(), cell, weekendB, D2D1_DRAW_TEXT_OPTIONS_NONE);
                } else {
                    textBrush_->SetOpacity(0.85f);
                    target_->DrawText(dBuf, dLen, boldTextFormat_.Get(), cell, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
                }
            }
            
            col++;
            if (col > 6) { col = 0; row++; }
        }
        textBrush_->SetOpacity(1.0f);
    }

    void DrawWeatherDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, bool hasWeather, const std::wstring& wIcon, const std::wstring& wText) {
        (void)now;
        bool tr = IsTurkish(settings.language);
        wchar_t wTemp[32] = {};
        if (hasWeather) swprintf_s(wTemp, L"%.0f\x00B0", state.weather.temperature);
        else wcscpy_s(wTemp, L"--\x00B0");

        std::wstring city = (hasWeather && !state.weather.city.empty() && state.weather.city != L"[")
            ? state.weather.city
            : (tr ? L"Mevcut Konum" : L"Current Location");
        std::wstring desc = wText.empty() ? (tr ? L"Açık" : L"Clear") : wText;

        const float midX = rect.left + (rect.right - rect.left) * 0.44f;

        textBrush_->SetOpacity(0.96f);
        D2D1_RECT_F cityRect = D2D1::RectF(rect.left + 24.0f * scale, rect.top + 18.0f * scale, midX - 10.0f * scale, rect.top + 40.0f * scale);
        DrawFittedTitle(city, cityRect, textBrush_.Get(), 15.0f, scale);

        D2D1_RECT_F iconRect = D2D1::RectF(rect.left + 24.0f * scale, rect.top + 44.0f * scale, rect.left + 78.0f * scale, rect.top + 104.0f * scale);
        target_->DrawText(wIcon.c_str(), static_cast<UINT32>(wIcon.length()), hugeTextFormat_.Get(),
                           iconRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                           
        D2D1_RECT_F tempRect = D2D1::RectF(rect.left + 80.0f * scale, rect.top + 44.0f * scale, midX - 10.0f * scale, rect.top + 104.0f * scale);
        target_->DrawText(wTemp, static_cast<UINT32>(wcslen(wTemp)), hugeTextFormat_.Get(),
                           tempRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        mutedBrush_->SetOpacity(0.85f);
        D2D1_RECT_F descRect = D2D1::RectF(rect.left + 24.0f * scale, rect.top + 108.0f * scale, midX - 10.0f * scale, rect.bottom - 16.0f * scale);
        DrawFittedLine(desc, descRect, mutedBrush_.Get(), 12.5f, scale, false);

        ID2D1SolidColorBrush* divB = dividerBrush_ ? dividerBrush_.Get() : textBrush_.Get();
        target_->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(midX, rect.top + 24.0f * scale,
                                           midX + 1.5f * scale, rect.bottom - 24.0f * scale),
                              0.5f * scale, 0.5f * scale), divB);

        std::wstring line3 = hasWeather
            ? (tr ? L"💨 Rüzgar: " : L"💨 Wind: ") + state.weather.windSpeed + (settings.weatherFahrenheit ? L" mph " : L" km/h ") + state.weather.windDir
            : (tr ? L"💨 Rüzgar: --" : L"💨 Wind: --");
        std::wstring line4 = hasWeather
            ? (tr ? L"🌡️ Hissedilen: " : L"🌡️ Feels Like: ") + state.weather.feelsLike + L"\x00B0"
            : (tr ? L"🌡️ Hissedilen: --" : L"🌡️ Feels Like: --");
        std::wstring line5 = hasWeather
            ? (tr ? L"💧 Nem: " : L"💧 Humidity: ") + state.weather.humidity + L"%"
            : (tr ? L"💧 Nem: --" : L"💧 Humidity: --");

        const float rightX = midX + 18.0f * scale;
        const float rightMaxX = rect.right - 36.0f * scale;

        mutedBrush_->SetOpacity(0.85f);
        D2D1_RECT_F rightLine3 = D2D1::RectF(rightX, rect.top + 36.0f * scale, rightMaxX, rect.top + 62.0f * scale);
        DrawFittedLine(line3, rightLine3, mutedBrush_.Get(), 12.0f, scale, false);
                           
        D2D1_RECT_F rightLine4 = D2D1::RectF(rightX, rect.top + 68.0f * scale, rightMaxX, rect.top + 94.0f * scale);
        DrawFittedLine(line4, rightLine4, mutedBrush_.Get(), 12.0f, scale, false);
                           
        D2D1_RECT_F rightLine5 = D2D1::RectF(rightX, rect.top + 100.0f * scale, rightMaxX, rect.top + 126.0f * scale);
        DrawFittedLine(line5, rightLine5, mutedBrush_.Get(), 12.0f, scale, false);
        mutedBrush_->SetOpacity(1.0f);
        textBrush_->SetOpacity(1.0f);
    }

    void DrawNotesDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, float scale) {
        bool tr = IsTurkish(settings.language);
        textBrush_->SetOpacity(0.96f);
        D2D1_RECT_F titleRect = D2D1::RectF(rect.left + 20.0f * scale, rect.top + 14.0f * scale, rect.right - 36.0f * scale, rect.top + 38.0f * scale);
        std::wstring title = tr ? L"📝 Hızlı Notlar & Karalama" : L"📝 Quick Notes & Scratchpad";
        DrawFittedTitle(title, titleRect, textBrush_.Get(), 15.0f, scale);

        if (state.quickNotes.empty()) {
            mutedBrush_->SetOpacity(0.70f);
            std::wstring emptyMsg = tr
                ? L"Henüz not eklenmedi.\nPanodaki metni kaydetmek için kapsüle sağ tıklayın!"
                : L"No notes yet.\nRight-click capsule > 'Add Note from Clipboard' to capture your thoughts!";
            D2D1_RECT_F emptyRect = D2D1::RectF(rect.left + 20.0f * scale, rect.top + 44.0f * scale, rect.right - 36.0f * scale, rect.bottom - 14.0f * scale);
            DrawFittedLine(emptyMsg, emptyRect, mutedBrush_.Get(), 12.0f, scale, true);
            mutedBrush_->SetOpacity(1.0f);
        } else {
            const float startY = rect.top + 42.0f * scale;
            const float availH = (rect.bottom - 14.0f * scale) - startY;
            size_t count = std::min(state.quickNotes.size(), size_t(3));
            const float lineH = (count > 0 && availH > 20.0f) ? std::clamp(availH / static_cast<float>(count), 18.0f * scale, 34.0f * scale) : 26.0f * scale;
            for (size_t i = 0; i < count; ++i) {
                std::wstring item = L"• " + state.quickNotes[state.quickNotes.size() - 1 - i];
                textBrush_->SetOpacity(0.90f);
                D2D1_RECT_F lineRect = D2D1::RectF(rect.left + 20.0f * scale, startY + i * lineH, rect.right - 36.0f * scale, startY + (i + 1) * lineH - 2.0f * scale);
                DrawFittedLine(item, lineRect, textBrush_.Get(), 12.0f, scale, false);
            }
        }
        textBrush_->SetOpacity(1.0f);
    }

    void DrawPomodoroDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, float scale) {
        bool tr = IsTurkish(settings.language);
        const float totalW = (rect.right - rect.left);
        const float totalH = (rect.bottom - rect.top);
        const float cx = rect.left + std::clamp(totalW * 0.22f, 50.0f * scale, 95.0f * scale);
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float r = std::clamp(totalH * 0.26f, 22.0f * scale, 48.0f * scale);

        // Background Track Ring (Reuse whiteBrush_ to avoid per-frame allocation)
        if (whiteBrush_) {
            whiteBrush_->SetOpacity(0.10f * settingsOpacity_);
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), whiteBrush_.Get(), 5.0f * scale);
            whiteBrush_->SetOpacity(1.0f);
        }

        // Progress Arc (360° Smooth Path Geometry)
        float progress = state.pomodoro.totalSeconds > 0 ? static_cast<float>(state.pomodoro.remainingSeconds) / state.pomodoro.totalSeconds : 0.0f;
        progress = Clamp(progress, 0.0f, 1.0f);
        
        ComPtr<ID2D1SolidColorBrush> pomoBrush = (state.pomodoro.state == PomodoroState::Working) ? redBrush_ : greenBrush_;
        if (!pomoBrush) pomoBrush = accentBrush_;

        if (progress > 0.005f) {
            ComPtr<ID2D1PathGeometry> geometry;
            if (SUCCEEDED(d2dFactory_->CreatePathGeometry(&geometry))) {
                ComPtr<ID2D1GeometrySink> sink;
                if (SUCCEEDED(geometry->Open(&sink))) {
                    const float start = -3.14159265f * 0.5f;
                    const float sweep = 2.0f * 3.14159265f * progress;
                    const int segments = std::max(6, static_cast<int>(64 * progress));

                    auto pointAt = [&](float a) {
                        return D2D1::Point2F(cx + std::cos(a) * r, cy + std::sin(a) * r);
                    };

                    sink->BeginFigure(pointAt(start), D2D1_FIGURE_BEGIN_HOLLOW);
                    for (int i = 1; i <= segments; ++i) {
                        const float a = start + sweep * static_cast<float>(i) / segments;
                        sink->AddLine(pointAt(a));
                    }
                    sink->EndFigure(D2D1_FIGURE_END_OPEN);
                    sink->Close();

                    pomoBrush->SetOpacity(0.95f * settingsOpacity_);
                    target_->DrawGeometry(geometry.Get(), pomoBrush.Get(), 5.0f * scale);
                    pomoBrush->SetOpacity(1.0f);
                }
            }
        }

        int remMins = state.pomodoro.remainingSeconds / 60;
        int remSecs = state.pomodoro.remainingSeconds % 60;
        wchar_t timeStr[32] = {};
        swprintf_s(timeStr, L"%02d:%02d", remMins, remSecs);

        textBrush_->SetOpacity(0.96f);
        target_->DrawText(timeStr, static_cast<UINT32>(wcslen(timeStr)), boldTextFormat_.Get(),
                           D2D1::RectF(cx - 36.0f * scale, cy - 14.0f * scale, cx + 36.0f * scale, cy + 14.0f * scale),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        // Right side info & buttons
        const float rightLeft = cx + r + 18.0f * scale;
        std::wstring modeName;
        if (state.pomodoro.state == PomodoroState::Working) modeName = tr ? L"🍅 Odaklanma Seansı" : L"🍅 Focus Session";
        else if (state.pomodoro.state == PomodoroState::Break) modeName = tr ? L"☕ Kısa Mola" : L"☕ Short Break";
        else if (state.pomodoro.state == PomodoroState::LongBreak) modeName = tr ? L"🌿 Uzun Mola" : L"🌿 Long Break";
        else modeName = tr ? L"🍅 Pomodoro Hazır" : L"🍅 Pomodoro Ready";
        
        textBrush_->SetOpacity(0.96f);
        D2D1_RECT_F modeRect = D2D1::RectF(rightLeft, rect.top + 20.0f * scale, rect.right - 36.0f * scale, rect.top + 44.0f * scale);
        DrawFittedTitle(modeName, modeRect, textBrush_.Get(), 15.0f, scale);

        wchar_t sessStr[128] = {};
        if (tr) {
            swprintf_s(sessStr, L"Tamamlanan: %d seans | Hedef: %d dk", state.pomodoro.completedSessions, settings.pomodoroWorkMinutes);
        } else {
            swprintf_s(sessStr, L"Completed: %d sessions | Target: %dm", state.pomodoro.completedSessions, settings.pomodoroWorkMinutes);
        }
        mutedBrush_->SetOpacity(0.80f);
        D2D1_RECT_F sessRect = D2D1::RectF(rightLeft, rect.top + 48.0f * scale, rect.right - 36.0f * scale, rect.top + 72.0f * scale);
        DrawFittedLine(sessStr, sessRect, mutedBrush_.Get(), 11.5f, scale, false);

        std::wstring hint = tr ? L"Başlatmak / Duraklatmak için tıklayın" : L"Click or Right-Click to Start / Pause / Reset";
        mutedBrush_->SetOpacity(0.60f);
        D2D1_RECT_F hintRect = D2D1::RectF(rightLeft, rect.top + 76.0f * scale, rect.right - 36.0f * scale, rect.bottom - 12.0f * scale);
        DrawFittedLine(hint, hintRect, mutedBrush_.Get(), 11.0f, scale, true);
        mutedBrush_->SetOpacity(1.0f);
    }

    void DrawAssistantDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, float scale) {
        bool tr = IsTurkish(settings.language);
        textBrush_->SetOpacity(0.96f);
        D2D1_RECT_F titleRect = D2D1::RectF(rect.left + 20.0f * scale, rect.top + 14.0f * scale, rect.right - 36.0f * scale, rect.top + 38.0f * scale);
        std::wstring title = tr ? L"🌿 Yaşam Asistanı & Sağlık" : L"🌿 Living Assistant & Wellness";
        DrawFittedTitle(title, titleRect, textBrush_.Get(), 15.0f, scale);

        uint64_t uptimeMin = (GetTickCount64() / 1000) / 60;
        uint64_t upHours = uptimeMin / 60;
        uint64_t upM = uptimeMin % 60;

        wchar_t line1[128] = {};
        if (tr) {
            swprintf_s(line1, L"⏱️ PC Açık Kalma: %llu sa %llu dk", upHours, upM);
        } else {
            swprintf_s(line1, L"⏱️ PC Active Session: %llu hr %llu min", upHours, upM);
        }
        
        const float startY = rect.top + 42.0f * scale;
        const float rowH = std::clamp(((rect.bottom - 12.0f * scale) - startY) / 3.0f, 20.0f * scale, 34.0f * scale);

        textBrush_->SetOpacity(0.90f);
        D2D1_RECT_F l1Rect = D2D1::RectF(rect.left + 20.0f * scale, startY, rect.right - 36.0f * scale, startY + rowH);
        DrawFittedLine(line1, l1Rect, textBrush_.Get(), 12.0f, scale, false);

        double now = NowSeconds();
        wchar_t line2[128] = {};
        if (settings.hydrationIntervalMinutes > 0) {
            int remMin = static_cast<int>(std::max(0.0, state.assistant.nextHydrationTime - now) / 60.0);
            if (tr) swprintf_s(line2, L"💧 Su Takibi: Sıradaki uyarı %d dk sonra (Aralık: %d dk)", remMin, settings.hydrationIntervalMinutes);
            else swprintf_s(line2, L"💧 Hydration: Next alert in %dm (Interval: %dm)", remMin, settings.hydrationIntervalMinutes);
        } else {
            if (tr) wcscpy_s(line2, L"💧 Su Takibi: Kapalı");
            else wcscpy_s(line2, L"💧 Hydration: Disabled");
        }

        mutedBrush_->SetOpacity(0.80f);
        D2D1_RECT_F l2Rect = D2D1::RectF(rect.left + 20.0f * scale, startY + rowH, rect.right - 36.0f * scale, startY + rowH * 2.0f);
        DrawFittedLine(line2, l2Rect, mutedBrush_.Get(), 12.0f, scale, false);

        wchar_t line3[128] = {};
        if (settings.eyeRestReminder && settings.eyeRestMinutes > 0) {
            int remEye = static_cast<int>(std::max(0.0, state.assistant.nextEyeRestTime - now) / 60.0);
            if (tr) swprintf_s(line3, L"👁️ 20-20-20 Göz Molası: %d dk sonra (20 sn uzağa bakın)", remEye);
            else swprintf_s(line3, L"👁️ 20-20-20 Eye Rest: in %dm (Look 20ft away for 20s)", remEye);
        } else if (settings.postureReminder && settings.postureIntervalMinutes > 0) {
            int remPosture = static_cast<int>(std::max(0.0, state.assistant.nextPostureTime - now) / 60.0);
            if (tr) swprintf_s(line3, L"🧘 Duruş & Esneme: %d dk sonra (Dik oturun)", remPosture);
            else swprintf_s(line3, L"🧘 Posture & Stretch: in %dm (Straighten back)", remPosture);
        } else {
            std::wstring l3 = tr ? L"🧘 Duruş & Esneme: Dik oturun ve derin nefes alın" : L"🧘 Posture: Straighten back & breathe deep";
            wcscpy_s(line3, l3.c_str());
        }

        D2D1_RECT_F l3Rect = D2D1::RectF(rect.left + 20.0f * scale, startY + rowH * 2.0f, rect.right - 36.0f * scale, startY + rowH * 3.0f);
        DrawFittedLine(line3, l3Rect, mutedBrush_.Get(), 12.0f, scale, false);
        mutedBrush_->SetOpacity(1.0f);
        textBrush_->SetOpacity(1.0f);
    }

    void DrawSingleTab(int tabIndex, const SharedState& state, D2D1_RECT_F rect, const Settings& settings,
                       double now, float scale, const SYSTEMTIME& local, bool hasWeather,
                       const std::wstring& wIcon, const std::wstring& wText) {
        switch (tabIndex) {
            case 0: DrawLivingBriefingDashboard(state, rect, settings, now, scale, local, hasWeather, wIcon, wText); break;
            case 1: DrawCalendarDashboard(state, rect, settings, now, scale, local); break;
            case 2: DrawWeatherDashboard(state, rect, settings, now, scale, hasWeather, wIcon, wText); break;
            case 3: DrawGameOverlay(state, rect, scale); break;
            case 4: DrawNotesDashboard(state, rect, settings, scale); break;
            case 5: DrawPomodoroDashboard(state, rect, settings, scale); break;
            case 6:
            default: DrawAssistantDashboard(state, rect, settings, scale); break;
        }
    }

    void ResetTabTransition() {
        tabTransitionPos_ = 0.0f;
    }

    void DrawCollapsedIdle(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now) {
        const float width = rect.right - rect.left;
        const float height = rect.bottom - rect.top;
        if (width < 20.0f || height < 10.0f) return;

        target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        SYSTEMTIME local = {};
        GetLocalTime(&local);
        wchar_t timeBuf[32] = {};
        DWORD timeFlags = settings.idleClockShowSeconds ? 0 : TIME_NOSECONDS;
        if (settings.idleClockTimeFormat == 1) {
            if (settings.idleClockShowSeconds) {
                swprintf_s(timeBuf, L"%02d:%02d:%02d", local.wHour, local.wMinute, local.wSecond);
            } else {
                swprintf_s(timeBuf, L"%02d:%02d", local.wHour, local.wMinute);
            }
        } else if (settings.idleClockTimeFormat == 2) {
            int h12 = local.wHour % 12;
            if (h12 == 0) h12 = 12;
            const wchar_t* ampm = local.wHour >= 12 ? L"PM" : L"AM";
            if (settings.idleClockShowSeconds) {
                swprintf_s(timeBuf, L"%d:%02d:%02d %s", h12, local.wMinute, local.wSecond, ampm);
            } else {
                swprintf_s(timeBuf, L"%d:%02d %s", h12, local.wMinute, ampm);
            }
        } else {
            if (!GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, timeFlags, &local, nullptr, timeBuf, ARRAYSIZE(timeBuf))) {
                if (settings.idleClockShowSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", local.wHour, local.wMinute, local.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", local.wHour, local.wMinute);
                }
            }
        }

        bool hasWeather = state.weather.hasData && (now - state.weather.lastUpdated < 3600.0);
        std::wstring wIcon = L"🌡️";
        std::wstring wText = L"Clear";
        if (hasWeather) {
            wText = state.weather.weatherDesc;
            GetWeatherIconAndText(state.weather.weatherCode, wIcon, wText, IsTurkish(settings.language));
        }

        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        bool tr = IsTurkish(settings.language);

        bool pomoActive = (state.pomodoro.state == PomodoroState::Working || state.pomodoro.state == PomodoroState::Break);
        int displayMode = settings.idleDisplayMode;

        // Auto-adapt for Smart Dynamic (0)
        if (displayMode == 0) {
            if (pomoActive) displayMode = 5;
            else if (settings.showMetricsInIdle && state.system.cpuPercent >= 0) displayMode = 3;
            else if (hasWeather) displayMode = 1;
            else displayMode = 2;
        }

        const float tScale = settings.idleTextScale;
        const DWRITE_FONT_WEIGHT clockWeight = settings.idleClockBold ? DWRITE_FONT_WEIGHT_SEMI_BOLD : DWRITE_FONT_WEIGHT_NORMAL;

        const float halfH = std::clamp((rect.bottom - rect.top) * 0.38f, 8.0f, 16.0f);
        const float divH = std::clamp(halfH * 0.65f, 5.0f, 12.0f);

        ID2D1SolidColorBrush* divB = dividerBrush_ ? dividerBrush_.Get() : mutedBrush_.Get();

        if (pomoActive || displayMode == 5) {
            // Live Pomodoro Focus (Tomato + MM:SS | Time)
            int remMins = state.pomodoro.remainingSeconds / 60;
            int remSecs = state.pomodoro.remainingSeconds % 60;
            wchar_t pomoText[32] = {};
            swprintf_s(pomoText, L"🍅 %02d:%02d", remMins, remSecs);

            ID2D1SolidColorBrush* pBrush = (state.pomodoro.state == PomodoroState::Working) ? redBrush_.Get() : greenBrush_.Get();

            D2D1_RECT_F leftRect = D2D1::RectF(rect.left + 6.0f, cy - halfH, cx - 3.0f, cy + halfH);
            DrawFittedLine(pomoText, leftRect, pBrush ? pBrush : textBrush_.Get(), 12.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD);

            // Center Divider
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 0.5f, cy - divH, cx + 0.5f, cy + divH), 0.5f, 0.5f), divB);

            // Right: Time
            D2D1_RECT_F rightRect = D2D1::RectF(cx + 3.0f, cy - halfH, rect.right - 6.0f, cy + halfH);
            DrawFittedLine(timeBuf, rightRect, textBrush_.Get(), 12.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, clockWeight);
        } else if (displayMode == 2) {
            // Minimalist Clock Only (Centered)
            D2D1_RECT_F fullRect = D2D1::RectF(rect.left + 6.0f, cy - halfH, rect.right - 6.0f, cy + halfH);
            DrawFittedLine(timeBuf, fullRect, textBrush_.Get(), 14.5f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, clockWeight);
        } else if (displayMode == 3) {
            // System Telemetry HUD with high contrast, 3 equal columns and zero overlap
            const float pad = 6.0f;
            const float totalW = (rect.right - rect.left) - pad * 2.0f;
            const float colW = totalW / 3.0f;
            const float d1 = rect.left + pad + colW;
            const float d2 = rect.left + pad + colW * 2.0f;

            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(d1 - 0.5f, cy - divH, d1 + 0.5f, cy + divH), 0.5f, 0.5f), divB);
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(d2 - 0.5f, cy - divH, d2 + 0.5f, cy + divH), 0.5f, 0.5f), divB);

            // Col 1: Time
            D2D1_RECT_F col1 = D2D1::RectF(rect.left + pad, cy - halfH, d1 - 2.0f, cy + halfH);
            DrawFittedLine(timeBuf, col1, textBrush_.Get(), 11.5f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, clockWeight);

            // Col 2: CPU %
            wchar_t cpuLabel[32] = {};
            swprintf_s(cpuLabel, L"CPU %d%%", state.system.cpuPercent >= 0 ? state.system.cpuPercent : 0);
            D2D1_RECT_F col2 = D2D1::RectF(d1 + 2.0f, cy - halfH, d2 - 2.0f, cy + halfH);
            DrawFittedLine(cpuLabel, col2, cyanBrush_ ? cyanBrush_.Get() : textBrush_.Get(), 11.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD);

            // Col 3: RAM %
            wchar_t memLabel[32] = {};
            swprintf_s(memLabel, L"RAM %d%%", state.system.memoryPercent >= 0 ? state.system.memoryPercent : 0);
            D2D1_RECT_F col3 = D2D1::RectF(d2 + 2.0f, cy - halfH, rect.right - pad, cy + halfH);
            DrawFittedLine(memLabel, col3, orangeBrush_ ? orangeBrush_.Get() : textBrush_.Get(), 11.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        } else if (displayMode == 4) {
            // Weather Focus: [ ☀️ Istanbul 28° | 17:09 ]
            wchar_t weatherLabel[64] = {};
            std::wstring locName = (!state.weather.city.empty() && state.weather.city != L"[") ? state.weather.city : (tr ? L"Hava" : L"Weather");
            if (hasWeather) swprintf_s(weatherLabel, L"%s %s %.0f\x00B0", wIcon.c_str(), locName.c_str(), state.weather.temperature);
            else swprintf_s(weatherLabel, L"☀️ %s 24\x00B0", locName.c_str());

            D2D1_RECT_F leftRect = D2D1::RectF(rect.left + 6.0f, cy - halfH, cx - 3.0f, cy + halfH);
            DrawFittedLine(weatherLabel, leftRect, textBrush_.Get(), 11.5f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD);

            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 0.5f, cy - divH, cx + 0.5f, cy + divH), 0.5f, 0.5f), divB);

            D2D1_RECT_F rightRect = D2D1::RectF(cx + 3.0f, cy - halfH, rect.right - 6.0f, cy + halfH);
            DrawFittedLine(timeBuf, rightRect, textBrush_.Get(), 11.5f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, clockWeight);
        } else {
            // Classic 50/50 Duo: [ Time | Weather ] (Style 1 & default)
            D2D1_RECT_F leftRect = D2D1::RectF(rect.left + 6.0f, cy - halfH, cx - 3.0f, cy + halfH);
            DrawFittedLine(timeBuf, leftRect, textBrush_.Get(), 12.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, clockWeight);

            // Center Divider
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 0.5f, cy - divH, cx + 0.5f, cy + divH), 0.5f, 0.5f), divB);

            // Weather: [ ☀️ 32° ]
            wchar_t weatherLabel[32] = {};
            if (hasWeather) swprintf_s(weatherLabel, L"%s %.0f\x00B0", wIcon.c_str(), state.weather.temperature);
            else wcscpy_s(weatherLabel, ARRAYSIZE(weatherLabel), L"☀️ 24\x00B0");

            D2D1_RECT_F rightRect = D2D1::RectF(cx + 3.0f, cy - halfH, rect.right - 6.0f, cy + halfH);
            DrawFittedLine(weatherLabel, rightRect, textBrush_.Get(), 12.0f * tScale, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        }

        target_->PopAxisAlignedClip();
    }

    void DrawExpandedDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale) {
        if (settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0) {
            DrawGameOverlay(state, rect, scale);
            return;
        }

        target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        SYSTEMTIME local = {};
        GetLocalTime(&local);

        bool hasWeather = state.weather.hasData && (now - state.weather.lastUpdated < 3600.0);
        std::wstring wIcon = L"🌡️";
        std::wstring wText = L"Clear";
        if (hasWeather) {
            wText = state.weather.weatherDesc;
            GetWeatherIconAndText(state.weather.weatherCode, wIcon, wText, IsTurkish(settings.language));
        }

        // Expanded Mode (7 Interactive Tabs with Smooth Sliding Carousel)
        int tab = Modulo(g_idleTab.load(), kTotalTabs);

        // Animate tab transition position with wrap-around support
        float targetPos = static_cast<float>(tab);
        float diff = targetPos - tabTransitionPos_;
        if (diff > kTotalTabs * 0.5f) tabTransitionPos_ += static_cast<float>(kTotalTabs);
        else if (diff < -kTotalTabs * 0.5f) tabTransitionPos_ -= static_cast<float>(kTotalTabs);
        diff = targetPos - tabTransitionPos_;

        if (std::abs(diff) > 0.002f) {
            tabTransitionPos_ += diff * Clamp(settings.animationSpeed * 0.22f, 0.08f, 0.45f);
        } else {
            tabTransitionPos_ = targetPos;
        }

        const float totalW = rect.right - rect.left;
        if (std::abs(tabTransitionPos_ - targetPos) <= 0.005f) {
            tabTransitionPos_ = targetPos;
            DrawSingleTab(tab, state, rect, settings, now, scale, local, hasWeather, wIcon, wText);
        } else {
            int floorTab = static_cast<int>(std::floor(tabTransitionPos_));
            float frac = tabTransitionPos_ - static_cast<float>(floorTab);
            int tab1 = Modulo(floorTab, kTotalTabs);
            int tab2 = Modulo(floorTab + 1, kTotalTabs);
            float offset1 = -frac * totalW;
            float offset2 = (1.0f - frac) * totalW;

            D2D1_RECT_F r1 = D2D1::RectF(rect.left + offset1, rect.top, rect.right + offset1, rect.bottom);
            D2D1_RECT_F r2 = D2D1::RectF(rect.left + offset2, rect.top, rect.right + offset2, rect.bottom);

            // Strict per-tab clipping: Tab 1 only renders in its visible bounds
            float clipLeft1 = std::max(rect.left, r1.left);
            float clipRight1 = std::min(rect.right, r1.right);
            if (clipRight1 > clipLeft1 + 1.0f) {
                D2D1_RECT_F clip1 = D2D1::RectF(clipLeft1, rect.top, clipRight1, rect.bottom);
                target_->PushAxisAlignedClip(clip1, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                DrawSingleTab(tab1, state, r1, settings, now, scale, local, hasWeather, wIcon, wText);
                target_->PopAxisAlignedClip();
            }

            // Strict per-tab clipping: Tab 2 only renders in its visible bounds
            float clipLeft2 = std::max(rect.left, r2.left);
            float clipRight2 = std::min(rect.right, r2.right);
            if (clipRight2 > clipLeft2 + 1.0f) {
                D2D1_RECT_F clip2 = D2D1::RectF(clipLeft2, rect.top, clipRight2, rect.bottom);
                target_->PushAxisAlignedClip(clip2, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
                DrawSingleTab(tab2, state, r2, settings, now, scale, local, hasWeather, wIcon, wText);
                target_->PopAxisAlignedClip();
            }
        }

        // 7 Pagination Dots (Vertical on the right edge)
        float shiftX = 0.0f;
        if (state.system.micActive && state.system.cameraActive) shiftX = 26.0f * scale;
        else if (state.system.micActive || state.system.cameraActive) shiftX = 14.0f * scale;

        const float dotX = rect.right - 12.0f * scale - shiftX;
        const float dotY = (rect.top + rect.bottom) * 0.5f;
        const float spacing = 7.0f * scale;
        const float r = settings.paginationDotSize * scale;

        // Reuse whiteBrush_ with opacity instead of allocating new brushes every frame (perf fix BUG-10)
        const float startY = dotY - (kTotalTabs - 1) * 0.5f * spacing;
        for (int i = 0; i < kTotalTabs; ++i) {
            if (whiteBrush_) {
                whiteBrush_->SetOpacity((tab == i) ? 0.90f * settingsOpacity_ : 0.22f * settingsOpacity_);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, startY + i * spacing), r, r), whiteBrush_.Get());
            }
        }
        if (whiteBrush_) whiteBrush_->SetOpacity(1.0f);

        target_->PopAxisAlignedClip();
    }

    void DrawGameOverlay(const SharedState& state, D2D1_RECT_F rect, float scale) {
        Settings settings = GetSettingsSnapshot();
        const float padL = rect.left + 16.0f * scale;
        const float padR = rect.right - 36.0f * scale;
        const float availW = padR - padL;
        const float gap = 8.0f * scale;
        const float cardW = (availW - gap * 4.0f) / 5.0f;
        const float cardTop = rect.top + 14.0f * scale;
        const float cardBottom = rect.bottom - 14.0f * scale;

        DrawGameMetricCard(D2D1::RectF(padL, cardTop, padL + cardW, cardBottom),
                           L"FPS", state.system.renderFps, 0, scale, true);
        DrawGameMetricCard(D2D1::RectF(padL + (cardW + gap), cardTop, padL + (cardW + gap) + cardW, cardBottom),
                           L"CPU", state.system.cpuPercent, 1, scale, false);
        DrawGameMetricCard(D2D1::RectF(padL + (cardW + gap) * 2.0f, cardTop, padL + (cardW + gap) * 2.0f + cardW, cardBottom),
                           L"RAM", state.system.memoryPercent, 2, scale, false);
        DrawGameMetricCard(D2D1::RectF(padL + (cardW + gap) * 3.0f, cardTop, padL + (cardW + gap) * 3.0f + cardW, cardBottom),
                           L"GPU", state.system.gpuPercent, 3, scale, false);
        DrawGameMetricCard(D2D1::RectF(padL + (cardW + gap) * 4.0f, cardTop, padL + (cardW + gap) * 4.0f + cardW, cardBottom),
                           L"DSK", 100 - state.system.diskFreePercent, 4, scale, false);
    }

    void DrawGameMetricCard(D2D1_RECT_F rect, const wchar_t* label, int percent, int iconKind, float scale, bool isFps = false) {
        Settings settings = GetSettingsSnapshot();
        ID2D1SolidColorBrush* metricBrush = cyanBrush_ ? cyanBrush_.Get() : textBrush_.Get();
        switch (iconKind) {
            case 0: metricBrush = greenBrush_ ? greenBrush_.Get() : textBrush_.Get(); break; // FPS Green
            case 1: metricBrush = cyanBrush_ ? cyanBrush_.Get() : textBrush_.Get(); break; // CPU Cyan
            case 2: metricBrush = magentaBrush_ ? magentaBrush_.Get() : textBrush_.Get(); break; // RAM Magenta
            case 3: metricBrush = greenBrush_ ? greenBrush_.Get() : textBrush_.Get(); break; // GPU Green
            case 4: metricBrush = orangeBrush_ ? orangeBrush_.Get() : textBrush_.Get(); break; // DSK Orange
            default: break;
        }

        ID2D1SolidColorBrush* cardB = cardBgBrush_ ? cardBgBrush_.Get() : textBrush_.Get();
        ID2D1SolidColorBrush* borderB = cardBorderBrush_ ? cardBorderBrush_.Get() : mutedBrush_.Get();

        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, 14.0f * scale, 14.0f * scale), cardB);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 14.0f * scale, 14.0f * scale), borderB, 1.0f * scale);

        metricBrush->SetOpacity(0.24f * settingsOpacity_);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 14.0f * scale, 14.0f * scale), metricBrush, 1.2f * scale);
        metricBrush->SetOpacity(1.0f);

        // Icon + Label
        DrawGameIcon(D2D1::Point2F(rect.left + 16.0f * scale, rect.top + 18.0f * scale), 7.0f * scale, iconKind, metricBrush, scale);

        if (settings.showMetricText) {
            mutedBrush_->SetOpacity(0.60f);
            target_->DrawText(label, static_cast<UINT32>(wcslen(label)), smallTextFormat_.Get(),
                               D2D1::RectF(rect.left + 28.0f * scale, rect.top + 8.0f * scale,
                                           rect.right - 6.0f * scale, rect.top + 26.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        // Value
        wchar_t value[16] = {};
        if (isFps) {
            swprintf_s(value, L"%d", percent);
        } else {
            if (percent < 0) wcscpy_s(value, ARRAYSIZE(value), L"--");
            else swprintf_s(value, L"%d%%", percent);
        }
        textBrush_->SetOpacity(0.96f);
        const float cardH = rect.bottom - rect.top;
        D2D1_RECT_F valueRect = D2D1::RectF(rect.left + 4.0f * scale, rect.top + cardH * 0.35f, rect.right - 4.0f * scale, rect.bottom - 18.0f * scale);
        DrawFittedLine(value, valueRect, textBrush_.Get(), 14.0f, scale, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_FONT_WEIGHT_BOLD);

        // Progress Bar
        const float pct = isFps ? Clamp(percent / 144.0f, 0.0f, 1.0f) : (percent < 0 ? 0.0f : Clamp(percent / 100.0f, 0.0f, 1.0f));
        D2D1_RECT_F track = D2D1::RectF(rect.left + 10.0f * scale, rect.bottom - 10.0f * scale,
                                       rect.right - 10.0f * scale, rect.bottom - 5.0f * scale);
        borderB->SetOpacity(0.12f * settingsOpacity_);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2.5f * scale, 2.5f * scale), borderB);
        D2D1_RECT_F fillRect = D2D1::RectF(track.left, track.top,
                                          track.left + (track.right - track.left) * pct,
                                          track.bottom);
        metricBrush->SetOpacity(0.88f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, 2.5f * scale, 2.5f * scale), metricBrush);
        metricBrush->SetOpacity(1.0f);
    }

    void DrawGameIcon(D2D1_POINT_2F center, float radius, int kind, ID2D1SolidColorBrush* customBrush, float scale) {
        ID2D1SolidColorBrush* brush = customBrush ? customBrush : accentBrush_.Get();
        if (!brush) return;
        brush->SetOpacity(0.88f);
        switch (kind) {
            case 1: { // CPU die
                const float size = radius * 0.82f;
                D2D1_RECT_F outerRect = D2D1::RectF(center.x - size, center.y - size, center.x + size, center.y + size);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(outerRect, 2.0f * scale, 2.0f * scale), brush, 1.4f * scale);
                const float dieSize = size * 0.45f;
                D2D1_RECT_F dieRect = D2D1::RectF(center.x - dieSize, center.y - dieSize, center.x + dieSize, center.y + dieSize);
                target_->FillRoundedRectangle(D2D1::RoundedRect(dieRect, 1.0f * scale, 1.0f * scale), brush);
                break;
            }
            case 2: { // RAM
                const float w = radius * 1.15f;
                const float h = radius * 0.45f;
                D2D1_RECT_F pcb = D2D1::RectF(center.x - w, center.y - h, center.x + w, center.y + h);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(pcb, 1.0f * scale, 1.0f * scale), brush, 1.3f * scale);
                break;
            }
            case 3: { // GPU Fan
                const float fanR = radius * 0.75f;
                target_->DrawEllipse(D2D1::Ellipse(center, fanR, fanR), brush, 1.2f * scale);
                target_->FillEllipse(D2D1::Ellipse(center, 1.8f * scale, 1.8f * scale), brush);
                break;
            }
            case 4: { // Disk
                const float w = radius * 0.85f;
                const float h = radius * 1.05f;
                D2D1_RECT_F enc = D2D1::RectF(center.x - w, center.y - h, center.x + w, center.y + h);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(enc, 2.0f * scale, 2.0f * scale), brush, 1.3f * scale);
                break;
            }
            case 0:
            default: { // Speedometer
                const float dialR = radius * 1.05f;
                target_->DrawEllipse(D2D1::Ellipse(center, dialR, dialR), brush, 1.3f * scale);
                target_->FillEllipse(D2D1::Ellipse(center, 1.6f * scale, 1.6f * scale), brush);
                break;
            }
        }
        brush->SetOpacity(1.0f);
    }

    void DrawMetricChip(const SharedState& state, D2D1_RECT_F rect, const wchar_t* label, int percent, int iconKind) {
        Settings settings = GetSettingsSnapshot();
        const float scale = 1.0f;
        ID2D1SolidColorBrush* metricBrush = cyanBrush_ ? cyanBrush_.Get() : textBrush_.Get();
        switch (iconKind) {
            case 1: {
                if (state.battery.low) metricBrush = redBrush_ ? redBrush_.Get() : textBrush_.Get();
                else if (state.system.charging) metricBrush = yellowBrush_ ? yellowBrush_.Get() : textBrush_.Get();
                else metricBrush = greenBrush_ ? greenBrush_.Get() : textBrush_.Get();
                break;
            }
            case 2: metricBrush = cyanBrush_ ? cyanBrush_.Get() : textBrush_.Get(); break;
            case 3: default: metricBrush = magentaBrush_ ? magentaBrush_.Get() : textBrush_.Get(); break;
        }

        ID2D1SolidColorBrush* chipBg = cardBgBrush_ ? cardBgBrush_.Get() : textBrush_.Get();
        ID2D1SolidColorBrush* chipBorder = cardBorderBrush_ ? cardBorderBrush_.Get() : mutedBrush_.Get();

        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), chipBg);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), chipBorder, 1.0f * scale);

        metricBrush->SetOpacity(0.24f * settingsOpacity_);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), metricBrush, 1.2f * scale);
        metricBrush->SetOpacity(1.0f);

        if (settings.showMetricText) {
            D2D1_RECT_F labelRect = D2D1::RectF(rect.left + 23.0f * scale, rect.top + 3.0f * scale, rect.right - 2.0f * scale, rect.top + 20.0f * scale);
            mutedBrush_->SetOpacity(0.48f);
            target_->DrawText(label, static_cast<UINT32>(wcslen(label)), smallTextFormat_.Get(), labelRect,
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        wchar_t value[16] = {};
        if (percent < 0) wcscpy_s(value, ARRAYSIZE(value), L"--");
        else swprintf_s(value, L"%d%%", percent);
        textBrush_->SetOpacity(0.90f);
        D2D1_RECT_F valueRect = settings.showMetricText
            ? D2D1::RectF(rect.left + 7.5f * scale, rect.top + 14.5f * scale, rect.right - 2.0f * scale, rect.bottom - 8.0f * scale)
            : D2D1::RectF(rect.left + 23.0f * scale, rect.top + 8.0f * scale, rect.right - 2.0f * scale, rect.bottom - 8.0f * scale);
        target_->DrawText(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        const float clamped = percent < 0 ? 0.0f : Clamp(percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F meterTrack = D2D1::RectF(rect.left + 8.0f * scale, rect.bottom - 6.0f * scale, rect.right - 8.0f * scale, rect.bottom - 4.0f * scale);
        chipBorder->SetOpacity(0.12f * settingsOpacity_);
        target_->FillRoundedRectangle(D2D1::RoundedRect(meterTrack, 1.0f * scale, 1.0f * scale), chipBorder);
        
        D2D1_RECT_F meterFill = D2D1::RectF(meterTrack.left, meterTrack.top,
                                           meterTrack.left + (meterTrack.right - meterTrack.left) * clamped,
                                           meterTrack.bottom);
        metricBrush->SetOpacity(0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(meterFill, 1.0f * scale, 1.0f * scale), metricBrush);
        
        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawMedia(const SharedState& state, D2D1_RECT_F rect, double now) {
        Settings settings = GetSettingsSnapshot();
        const float height = rect.bottom - rect.top;

        float radius = CalculateCornerRadius(height, settings);
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(&layer);

        float expandedAlpha = std::clamp((height - 60.0f) / 60.0f, 0.0f, 1.0f);
        float collapsedAlpha = std::clamp((80.0f - height) / 30.0f, 0.0f, 1.0f);

        // Expanded UI
        if (expandedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), expandedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());
            
            const float totalH = rect.bottom - rect.top;
            const float artSize = std::clamp(totalH * 0.36f, 36.0f, 68.0f);
            const float artTop = rect.top + 16.0f;
            D2D1_RECT_F artRect = D2D1::RectF(rect.left + 20.0f, artTop,
                                              rect.left + 20.0f + artSize, artTop + artSize);
            DrawAlbumArt(state.media, artRect, now, 14.0f, true);

            float shiftX = 0.0f;
            if (state.system.micActive && state.system.cameraActive) shiftX = 30.0f;
            else if (state.system.micActive || state.system.cameraActive) shiftX = 16.0f;

            const float waveW = 32.0f;
            const float waveH = 20.0f;
            D2D1_RECT_F waveRect = D2D1::RectF(rect.right - 20.0f - shiftX - waveW,
                                               artTop + (artSize - waveH) * 0.5f,
                                               rect.right - 20.0f - shiftX,
                                               artTop + (artSize + waveH) * 0.5f);

            const float textLeft = artRect.right + 14.0f;
            const float textRight = waveRect.left - 12.0f;
            
            D2D1_RECT_F titleRect = D2D1::RectF(textLeft, artTop + 4.0f, textRight, artTop + 24.0f);
            DrawMarqueeText(state.media.title.empty() ? L"Unknown Track" : state.media.title,
                            titleRect, textFormat_.Get(), textBrush_.Get(), now, 42.0f);

            D2D1_RECT_F artistRect = D2D1::RectF(textLeft, artTop + 26.0f, textRight, artTop + 46.0f);
            mutedBrush_->SetOpacity(0.65f);
            DrawMarqueeText(state.media.artist.empty() ? L"Unknown Artist" : state.media.artist,
                            artistRect, smallTextFormat_.Get(), mutedBrush_.Get(), now, 30.0f);
            mutedBrush_->SetOpacity(0.50f);

            if (state.media.playing) {
                DrawWaveform(state, waveRect, settings);
            } else {
                mutedBrush_->SetOpacity(0.5f);
                for (int i = 0; i < 4; ++i) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(waveRect.left + i * 4.0f + 2.0f, (waveRect.top + waveRect.bottom) * 0.5f), 1.2f, 1.2f), mutedBrush_.Get());
                }
            }

            // Timeline Scrubber
            const float scrubberY = artTop + artSize + 16.0f;
            double currentPosition = state.media.positionTicks / 10000000.0;
            double duration = state.media.endTicks / 10000000.0;
            if (state.media.playing && state.media.lastUpdatedTicks > 0) {
                currentPosition += (GetTickCount64() - state.media.lastUpdatedTicks) / 1000.0;
            }
            currentPosition = std::max(0.0, std::min(currentPosition, duration));

            auto FormatTime = [](double seconds) -> std::wstring {
                if (seconds <= 0.0 || _isnan(seconds)) return L"0:00";
                int m = static_cast<int>(seconds) / 60;
                int s = static_cast<int>(seconds) % 60;
                wchar_t buf[16] = {};
                swprintf_s(buf, L"%d:%02d", m, s);
                return buf;
            };

            std::wstring elapsedStr = FormatTime(currentPosition);
            std::wstring remainStr = L"-" + FormatTime(duration - currentPosition);

            const float scrubLeft = rect.left + 20.0f;
            const float scrubRight = rect.right - 20.0f;
            
            mutedBrush_->SetOpacity(0.85f);
            D2D1_RECT_F elRect = D2D1::RectF(scrubLeft, scrubberY - 8.0f, scrubLeft + 40.0f, scrubberY + 8.0f);
            target_->DrawText(elapsedStr.c_str(), static_cast<UINT32>(elapsedStr.size()), smallTextFormat_.Get(), elRect, mutedBrush_.Get());
            
            D2D1_RECT_F remRect = D2D1::RectF(scrubRight - 40.0f, scrubberY - 8.0f, scrubRight, scrubberY + 8.0f);
            target_->DrawText(remainStr.c_str(), static_cast<UINT32>(remainStr.size()), smallTextFormat_.Get(), remRect, mutedBrush_.Get());

            const float barLeft = scrubLeft + 44.0f;
            const float barRight = scrubRight - 44.0f;
            const float progress = duration > 0.0 ? static_cast<float>(currentPosition / duration) : 0.0f;

            ComPtr<ID2D1SolidColorBrush> scrubBg;
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.15f), &scrubBg);
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(barLeft, scrubberY - 2.5f, barRight, scrubberY + 2.5f), 2.5f, 2.5f), scrubBg.Get());

            ComPtr<ID2D1SolidColorBrush> scrubFg;
            target_->CreateSolidColorBrush(state.media.art.bgra.empty() ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.9f) : state.media.art.sampledAccent, &scrubFg);
            const float scrubW = (barRight - barLeft) * progress;
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(barLeft, scrubberY - 2.5f, barLeft + scrubW, scrubberY + 2.5f), 2.5f, 2.5f), scrubFg.Get());

            // Controls
            const float cy = scrubberY + (rect.bottom - scrubberY) * 0.48f;
            const float cx = (rect.left + rect.right) * 0.5f;
            DrawMediaControls(state.media.playing, 
                              D2D1::Point2F(cx - 64.0f, cy),
                              D2D1::Point2F(cx, cy),
                              D2D1::Point2F(cx + 64.0f, cy));

            target_->PopLayer();
        }

        // Collapsed UI
        if (collapsedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), collapsedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());
            
            const float cy = (rect.top + rect.bottom) * 0.5f;
            const float artPadding = 6.0f;
            const float artSize = height - artPadding * 2.0f;
            
            D2D1_RECT_F artRect = D2D1::RectF(rect.left + 8.0f, cy - artSize * 0.5f,
                                              rect.left + 8.0f + artSize, cy + artSize * 0.5f);
            DrawAlbumArt(state.media, artRect, now, artSize * 0.5f, false);

            float shiftX = 0.0f;
            if (state.system.micActive && state.system.cameraActive) shiftX = 30.0f * settings.sizeScale;
            else if (state.system.micActive || state.system.cameraActive) shiftX = 16.0f * settings.sizeScale;

            const float waveW = std::clamp(42.0f * settings.sizeScale, 28.0f, 60.0f);
            D2D1_RECT_F waveRect = D2D1::RectF(rect.right - waveW - shiftX, cy - 10.0f,
                                               rect.right - 14.0f * settings.sizeScale - shiftX, cy + 10.0f);

            const float titleLeft = artRect.right + 8.0f;
            const float titleRight = waveRect.left - 8.0f;
            if (titleRight > titleLeft + 20.0f && !state.media.title.empty()) {
                D2D1_RECT_F textRect = D2D1::RectF(titleLeft, cy - 8.0f, titleRight, cy + 8.0f);
                std::wstring displayMedia = state.media.title;
                if (!state.media.artist.empty()) {
                    displayMedia += L" \u2022 " + state.media.artist;
                }
                textBrush_->SetOpacity(0.92f);
                DrawMarqueeText(displayMedia, textRect, smallTextFormat_.Get(), textBrush_.Get(), now, 32.0f);
                textBrush_->SetOpacity(1.0f);
            }

            if (state.media.playing) {
                DrawWaveform(state, waveRect, settings);
            } else {
                mutedBrush_->SetOpacity(0.5f);
                for (int i = 0; i < 4; ++i) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(waveRect.left + i * 6.0f + 3.0f, cy), 1.5f, 1.5f), mutedBrush_.Get());
                }
            }

            target_->PopLayer();
        }
    }

    void DrawMediaControls(bool playing, D2D1_POINT_2F prev, D2D1_POINT_2F play, D2D1_POINT_2F next) {
        DrawMediaButton(prev, 16.0f, 0, false);
        DrawMediaButton(play, 22.0f, playing ? 1 : 2, true);
        DrawMediaButton(next, 16.0f, 3, false);
    }

    void DrawMediaButton(D2D1_POINT_2F center, float radius, int kind, bool primary) {
        int buttonCmd = (kind == 0) ? 0 : ((kind == 1 || kind == 2) ? 1 : 2);
        bool isPressed = (g_pressedMediaButton.load() == buttonCmd);

        if (isPressed) radius *= 0.88f;

        ComPtr<ID2D1SolidColorBrush> bg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, primary ? (isPressed ? 0.16f : 0.080f) : (isPressed ? 0.10f : 0.040f)), &bg);
        target_->FillEllipse(D2D1::Ellipse(center, radius, radius), bg.Get());
        accentBrush_->SetOpacity(primary ? (isPressed ? 1.0f : 0.88f) : (isPressed ? 0.80f : 0.62f));

        if (kind == 1) {  // pause
            const float h = radius * 0.72f;
            target_->FillRoundedRectangle(D2D1::RoundedRect(
                                             D2D1::RectF(center.x - 4.0f, center.y - h * 0.5f,
                                                         center.x - 1.5f, center.y + h * 0.5f),
                                             1.0f, 1.0f),
                                         accentBrush_.Get());
            target_->FillRoundedRectangle(D2D1::RoundedRect(
                                             D2D1::RectF(center.x + 1.5f, center.y - h * 0.5f,
                                                         center.x + 4.0f, center.y + h * 0.5f),
                                             1.0f, 1.0f),
                                         accentBrush_.Get());
        } else {
            const float dir = kind == 0 ? -1.0f : 1.0f;
            const float tri = radius * (primary ? 0.70f : 0.62f);
            const float cx = (kind == 0 || kind == 3) ? center.x - dir * radius * 0.16f : center.x;

            D2D1_POINT_2F p1 = D2D1::Point2F(cx - dir * tri * 0.35f, center.y - tri * 0.58f);
            D2D1_POINT_2F p2 = D2D1::Point2F(cx - dir * tri * 0.35f, center.y + tri * 0.58f);
            D2D1_POINT_2F p3 = D2D1::Point2F(cx + dir * tri * 0.55f, center.y);
            ComPtr<ID2D1PathGeometry> geom;
            d2dFactory_->CreatePathGeometry(&geom);
            ComPtr<ID2D1GeometrySink> sink;
            if (SUCCEEDED(geom->Open(&sink))) {
                sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(p2);
                sink->AddLine(p3);
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                target_->FillGeometry(geom.Get(), accentBrush_.Get());
            }

            if (kind == 0 || kind == 3) {
                const float x = cx + dir * radius * 0.55f;
                target_->DrawLine(D2D1::Point2F(x, center.y - radius * 0.45f),
                                  D2D1::Point2F(x, center.y + radius * 0.45f),
                                  accentBrush_.Get(), 1.5f);
            }
        }

        accentBrush_->SetOpacity(1.0f);
    }

    void DrawAlbumArt(const MediaSnapshot& media, D2D1_RECT_F rect, double now, float radius, bool drawBadge) {
        (void)now;
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        HRESULT hrMask = d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        HRESULT hrLayer = target_->CreateLayer(nullptr, &layer);
        const bool roundedClip = SUCCEEDED(hrMask) && SUCCEEDED(hrLayer) && mask && layer;
        if (roundedClip) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get()), layer.Get());
        } else {
            target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        }

        if (!media.art.bgra.empty()) {
            if (artGeneration_ != media.art.generation || !artBitmap_) {
                D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                target_->CreateBitmap(D2D1::SizeU(media.art.width, media.art.height),
                                      media.art.bgra.data(), media.art.width * 4,
                                      &props, &artBitmap_);
                artGeneration_ = media.art.generation;
            }

            D2D1_RECT_F dst = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
            if (artBitmap_) {
                target_->DrawBitmap(artBitmap_.Get(), dst, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        } else {
            accentBrush_->SetOpacity(0.24f);
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
            if (!media.sourceIcon.bgra.empty()) {
                D2D1_RECT_F iconRect = D2D1::RectF(rect.left + 11, rect.top + 11, rect.right - 11, rect.bottom - 11);
                DrawBitmapPixels(media.sourceIcon, iconRect, mediaSourceIconBitmap_, mediaSourceIconGeneration_, 0.95f);
            } else {
                target_->DrawText(media.sourceBadge.empty() ? L"\u25b6" : media.sourceBadge.c_str(),
                                   static_cast<UINT32>(media.sourceBadge.empty() ? 1 : media.sourceBadge.size()),
                                   textFormat_.Get(), rect, textBrush_.Get());
            }
        }

        if (drawBadge && !media.sourceIcon.bgra.empty()) {
            D2D1_RECT_F badge = D2D1::RectF(rect.right - 24, rect.bottom - 22, rect.right - 3, rect.bottom - 3);
            DrawCircularBitmapPixels(media.sourceIcon,
                                     D2D1::Point2F((badge.left + badge.right) * 0.5f, (badge.top + badge.bottom) * 0.5f),
                                     9.5f, mediaSourceIconBitmap_, mediaSourceIconGeneration_, 0.98f);
        }

        if (roundedClip) target_->PopLayer();
        else target_->PopAxisAlignedClip();
    }

    void DrawBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F rect,
                          ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                          float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;

        if (cachedGeneration != pixels.generation || !cache) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            target_->CreateBitmap(D2D1::SizeU(pixels.width, pixels.height),
                                  pixels.bgra.data(), pixels.width * 4,
                                  &props, &cache);
            cachedGeneration = pixels.generation;
        }

        if (cache) {
            target_->DrawBitmap(cache.Get(), rect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    void DrawRoundedBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F badge,
                                 float cornerRadius,
                                 ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                 float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;

        const float pad = 2.0f;
        D2D1_RECT_F iconRect = D2D1::RectF(badge.left + pad, badge.top + pad, badge.right - pad, badge.bottom - pad);
        const float innerR = std::max(0.0f, cornerRadius - pad);

        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(iconRect, innerR, innerR), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(iconRect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), layer.Get());
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
        }
    }

    void DrawCircularBitmapPixels(const BitmapPixels& pixels, D2D1_POINT_2F center, float radius,
                                  ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                  float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;

        D2D1_RECT_F rect = D2D1::RectF(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
        ComPtr<ID2D1EllipseGeometry> ellipse;
        d2dFactory_->CreateEllipseGeometry(D2D1::Ellipse(center, radius, radius), &ellipse);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (ellipse && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, ellipse.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), layer.Get());
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
        }

        if (!glowBorderBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &glowBorderBrush_);
        glowBorderBrush_->SetOpacity(0.12f);
        if (glowBorderBrush_) target_->DrawEllipse(D2D1::Ellipse(center, radius, radius), glowBorderBrush_.Get(), 1.0f);
    }

    void DrawMarqueeText(const std::wstring& text, D2D1_RECT_F rect, IDWriteTextFormat* format,
                         ID2D1Brush* brush, double now, float speed) {
        if (!format || !brush || text.empty() || !dwriteFactory_ || !target_) return;

        const float layoutHeight = rect.bottom - rect.top;
        const float available = rect.right - rect.left;
        if (layoutHeight <= 2.0f || available <= 2.0f) return;

        ComPtr<IDWriteTextLayout> layout;
        HRESULT hr = dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()),
                                                      format, 3000.0f, layoutHeight, &layout);
        if (FAILED(hr) || !layout) return;

        layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        DWRITE_TEXT_METRICS metrics = {};
        layout->GetMetrics(&metrics);

        D2D1_RECT_F clipRect = rect;
        clipRect.top -= 1.0f;
        clipRect.bottom += 1.0f;
        target_->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        if (metrics.widthIncludingTrailingWhitespace <= available) {
            target_->DrawTextLayout(D2D1::Point2F(rect.left, rect.top), layout.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
        } else {
            const float scrollW = metrics.widthIncludingTrailingWhitespace + 38.0f;
            const float pauseSecs = 1.5f;
            const float scrollSecs = (speed > 0.1f) ? (scrollW / speed) : 5.0f;
            const float cycleSecs = pauseSecs + scrollSecs;
            const float tInCycle = std::fmod(static_cast<float>(now), cycleSecs);
            const float offset = (tInCycle < pauseSecs) ? 0.0f : (tInCycle - pauseSecs) * speed;

            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset, rect.top), layout.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            if (offset > 0.0f) {
                target_->DrawTextLayout(D2D1::Point2F(rect.left - offset + scrollW, rect.top), layout.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            }
        }
        target_->PopAxisAlignedClip();
    }

    void DrawWaveform(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        const float gap = settings.waveformGapPx;
        const float minBarWidth = 2.0f;
        const float availableW = rect.right - rect.left;
        size_t targetCount = static_cast<size_t>(ClampInt(settings.waveformBarCount, 8, 32));
        size_t count = std::max<size_t>(1, static_cast<size_t>((availableW + gap) / (minBarWidth + gap)));
        count = std::min<size_t>(count, targetCount);

        const float barWidth = (availableW - gap * (count - 1)) / count;
        const float centerY = (rect.top + rect.bottom) * 0.5f;
        const float maxH = (rect.bottom - rect.top) * 0.86f;
        const size_t step = 4;

        for (size_t i = 0; i < count; ++i) {
            const size_t offset = (count - i) * step;
            const size_t source = (state.waveformWrite + state.waveform.size() - offset) % state.waveform.size();
            const float amp = Clamp(state.waveform[source], 0.03f, 1.0f);
            const float h = std::max(3.0f, amp * maxH);
            const float x = rect.left + i * (barWidth + gap);

            const float t = (count > 1) ? static_cast<float>(i) / static_cast<float>(count - 1) : 0.0f;
            const float freqBlend = t * 0.7f;
            D2D1::ColorF accentCol(0.42f, 0.79f, 0.94f);
            D2D1::ColorF barColor(
                Lerp(accentCol.r, 1.0f, freqBlend),
                Lerp(accentCol.g, 1.0f, freqBlend),
                Lerp(accentCol.b, 1.0f, freqBlend),
                0.45f + 0.5f * amp);

            D2D1_RECT_F bar = D2D1::RectF(x, centerY - h * 0.5f, x + barWidth, centerY + h * 0.5f);
            if (!waveformBrush_) target_->CreateSolidColorBrush(barColor, &waveformBrush_);
            else waveformBrush_->SetColor(barColor);
            target_->FillRoundedRectangle(D2D1::RoundedRect(bar, barWidth * 0.5f, barWidth * 0.5f), waveformBrush_.Get());
        }
    }

    void DrawVoiceRecording(const SharedState& state, D2D1_RECT_F rect, double now, const Settings& settings) {
        if (rect.bottom - rect.top < 18.0f || rect.right - rect.left < 50.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float iconSz = std::min((rect.bottom - rect.top) - 10.0f, 26.0f);
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 10.0f, cy - iconSz * 0.5f, rect.left + 10.0f + iconSz, cy + iconSz * 0.5f);
        const float br = iconSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 0.25f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        if (emojiFormat_) {
            DrawCenteredGlyph(L"🎙️", badge, emojiFormat_.Get(), textBrush_.Get(), true);
        }

        D2D1_RECT_F waveRect = D2D1::RectF(badge.right + 8.0f, rect.top + 4.0f, rect.right - 10.0f, rect.bottom - 4.0f);
        DrawWaveform(state, waveRect, settings);
    }

    void DrawClipboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 100.0f) return;
        const double now = NowSeconds();
        const float ttl = settings.clipboardDuration > 0.1f ? settings.clipboardDuration : 2.5f;
        const float remaining = Clamp(static_cast<float>(state.clipboard.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;
        bool tr = IsTurkish(settings.language);

        D2D1_RECT_F badge = D2D1::RectF(rect.left + 12, rect.top + 10, rect.left + 50, rect.bottom - 10);
        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.070f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), badgeBg.Get());
        target_->DrawRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), accentBrush_.Get(), 1.0f);

        if (!state.clipboard.appIcon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.clipboard.appIcon, badge, 10.0f, clipboardIconBitmap_, clipboardIconGeneration_, 0.96f);
        } else {
            const wchar_t* glyph = state.clipboard.image ? L"\uE114" : L"\uE8C8";
            ComPtr<ID2D1SolidColorBrush> iconBg;
            const D2D1_COLOR_F iconColor = state.clipboard.image
                ? D2D1::ColorF(0.3f, 0.3f, 0.35f, 0.90f)
                : D2D1::ColorF(1, 1, 1, 0.055f * settingsOpacity_);
            target_->CreateSolidColorBrush(iconColor, &iconBg);
            target_->FillRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), iconBg.Get());
            textBrush_->SetOpacity(0.95f);
            
            if (iconFormat_) {
                DrawCenteredGlyph(glyph, badge, iconFormat_.Get(), textBrush_.Get(), false);
            }
            textBrush_->SetOpacity(0.90f);
        }

        D2D1_RECT_F titleRect = D2D1::RectF(badge.right + 11, rect.top + 9, rect.right - 18, rect.top + 25);
        mutedBrush_->SetOpacity(0.48f);
        const std::wstring clipTitle = state.clipboard.appName.empty()
            ? (state.clipboard.image ? (tr ? std::wstring(L"Resim kopyalandı") : std::wstring(L"Image copied")) : (tr ? std::wstring(L"Pano") : std::wstring(L"Clipboard")))
            : state.clipboard.appName + (tr ? L"  \u00b7  Pano" : L"  \u00b7  Clipboard");
        target_->DrawText(clipTitle.c_str(), static_cast<UINT32>(clipTitle.size()), smallTextFormat_.Get(), titleRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);

        D2D1_RECT_F textRect = D2D1::RectF(badge.right + 11, rect.top + 25, rect.right - 18, rect.bottom - 12);
        DrawMarqueeText(state.clipboard.text.empty() ? (tr ? L"Kopyalandı" : L"Copied") : state.clipboard.text, textRect, textFormat_.Get(), textBrush_.Get(), now, 34.0f);

        D2D1_RECT_F track = D2D1::RectF(badge.right + 11, rect.bottom - 8, rect.right - 20, rect.bottom - 6);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1, 1), trackBrush.Get());
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top, track.left + (track.right - track.left) * progress, track.bottom);
        accentBrush_->SetOpacity(0.55f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 1, 1), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawNotification(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 20.0f || rect.right - rect.left < 60.0f) return;
        const double now = NowSeconds();
        const float ttl = settings.notificationDuration > 0.1f ? settings.notificationDuration : 4.0f;
        const float remaining = Clamp(static_cast<float>(state.notification.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;
        bool tr = IsTurkish(settings.language);

        auto SanitizeString = [](const std::wstring& in) -> std::wstring {
            std::wstring out;
            out.reserve(in.size());
            bool lastSpace = true;
            for (wchar_t c : in) {
                if (c == L'\r' || c == L'\n' || c == L'\t' || c == L' ') {
                    if (!lastSpace) {
                        out.push_back(L' ');
                        lastSpace = true;
                    }
                } else {
                    out.push_back(c);
                    lastSpace = false;
                }
            }
            while (!out.empty() && out.back() == L' ') out.pop_back();
            return out;
        };

        std::wstring cleanApp = SanitizeString(state.notification.app);
        std::wstring cleanTitle = SanitizeString(state.notification.title);
        std::wstring cleanBody = SanitizeString(state.notification.body);

        if (cleanTitle.empty()) cleanTitle = tr ? L"Bildirim" : L"Notification";

        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float iconSz = std::clamp((rect.bottom - rect.top) - 16.0f, 20.0f, 28.0f);
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 12.0f, cy - iconSz * 0.5f, rect.left + 12.0f + iconSz, cy + iconSz * 0.5f);
        const float br = iconSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> plateBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &plateBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), plateBrush.Get());

        if (!state.notification.icon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.notification.icon, badge, br, notificationIconBitmap_, notificationIconGeneration_, 1.0f);

            ComPtr<ID2D1SolidColorBrush> badgeColor;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &badgeColor);
            const float dotR = iconSz * 0.14f;
            const float dotX = badge.right - dotR * 0.5f;
            const float dotY = badge.top + dotR * 0.5f;
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeColor.Get());

            ComPtr<ID2D1SolidColorBrush> badgeBorder;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &badgeBorder);
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeBorder.Get(), 0.9f);
        } else {
            textBrush_->SetOpacity(0.95f);
            std::wstring glyph = L"🔔";
            if (cleanTitle.find(L"Screen") != std::wstring::npos || cleanTitle.find(L"Ekran") != std::wstring::npos || cleanTitle.find(L"Snip") != std::wstring::npos || cleanTitle.find(L"Alıntı") != std::wstring::npos) {
                glyph = L"📸";
            } else if (cleanTitle.find(L"Pomodoro") != std::wstring::npos) {
                glyph = L"🍅";
            } else if (cleanTitle.find(L"Hydrat") != std::wstring::npos || cleanTitle.find(L"Water") != std::wstring::npos || cleanTitle.find(L"Su") != std::wstring::npos) {
                glyph = L"💧";
            } else if (cleanTitle.find(L"Note") != std::wstring::npos || cleanTitle.find(L"Not") != std::wstring::npos) {
                glyph = L"📝";
            } else if (cleanApp.find(L"Mail") != std::wstring::npos || cleanApp.find(L"Outlook") != std::wstring::npos || cleanTitle.find(L"Mail") != std::wstring::npos || cleanTitle.find(L"E-posta") != std::wstring::npos) {
                glyph = L"✉️";
            }

            if (emojiFormat_) {
                DrawCenteredGlyph(glyph, badge, emojiFormat_.Get(), textBrush_.Get(), true);
            } else if (iconFormat_) {
                DrawCenteredGlyph(L"\uEA8F", badge, iconFormat_.Get(), textBrush_.Get(), false);
            }
        }

        const float tx = badge.right + 11.0f;
        const float rightPad = 14.0f;
        const float totalH = rect.bottom - rect.top;

        if (!cleanBody.empty() && cleanBody != cleanTitle && totalH >= 40.0f) {
            // Two-row display
            std::wstring headerText = cleanApp.empty() ? cleanTitle : (cleanApp + L" \u2022 " + cleanTitle);
            D2D1_RECT_F titleRect = D2D1::RectF(tx, cy - 17.0f, rect.right - rightPad, cy - 1.0f);
            textBrush_->SetOpacity(0.96f);
            DrawMarqueeText(headerText, titleRect, textFormat_.Get(), textBrush_.Get(), now, 32.0f);

            D2D1_RECT_F bodyRect = D2D1::RectF(tx, cy + 1.0f, rect.right - rightPad, cy + 17.0f);
            mutedBrush_->SetOpacity(0.85f);
            DrawMarqueeText(cleanBody, bodyRect, smallTextFormat_.Get() ? smallTextFormat_.Get() : textFormat_.Get(), mutedBrush_.Get(), now, 32.0f);
            mutedBrush_->SetOpacity(0.58f);
        } else {
            // Single-row combined marquee display
            std::wstring singleText = cleanTitle;
            if (!cleanBody.empty() && cleanBody != cleanTitle) {
                singleText += L" \u2022 " + cleanBody;
            }
            D2D1_RECT_F textRect = D2D1::RectF(tx, cy - 11.0f, rect.right - rightPad, cy + 11.0f);
            textBrush_->SetOpacity(0.96f);
            DrawMarqueeText(singleText, textRect, textFormat_.Get(), textBrush_.Get(), now, 32.0f);
        }

        D2D1_RECT_F track = D2D1::RectF(tx, rect.bottom - 4.5f, rect.right - rightPad, rect.bottom - 2.5f);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1.5f, 1.5f), trackBrush.Get());
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top, track.left + (track.right - track.left) * progress, track.bottom);
        accentBrush_->SetOpacity(0.80f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 1.5f, 1.5f), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawVolume(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const bool muted = state.volume.muted || state.volume.percent == 0;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f, rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;
        bool tr = IsTurkish(settings.language);

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const wchar_t* glyph = muted ? L"\uE74F" : L"\uE993";
        textBrush_->SetOpacity(0.95f);
        if (iconFormat_) {
            DrawCenteredGlyph(glyph, badge, iconFormat_.Get(), textBrush_.Get(), false);
        }

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 58, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        const std::wstring deviceLabel = state.volume.deviceName.empty() ? (tr ? std::wstring(L"Ses Seviyesi") : std::wstring(L"Volume")) : state.volume.deviceName;
        if (!deviceLabel.empty() && smallTextFormat_ && mutedBrush_) {
            target_->DrawText(deviceLabel.c_str(), static_cast<UINT32>(deviceLabel.size()), smallTextFormat_.Get(), labelRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        wchar_t value[16] = {};
        if (muted) wcscpy_s(value, ARRAYSIZE(value), tr ? L"Sessiz" : L"Muted");
        else swprintf_s(value, L"%d%%", state.volume.percent);
        D2D1_RECT_F valueRect = D2D1::RectF(rect.right - 58, cy - 22, rect.right - 14, cy - 6);
        if (wcslen(value) > 0 && smallTextFormat_ && textBrush_) {
            target_->DrawText(value, static_cast<UINT32>(wcslen(value)), smallTextFormat_.Get(), valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        textBrush_->SetOpacity(0.90f);

        D2D1_RECT_F track = D2D1::RectF(tx, cy + 2, rect.right - 14, cy + 6);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2, 2), trackBrush.Get());
        const float pct = Clamp(state.volume.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top, track.left + (track.right - track.left) * pct, track.bottom);
        accentBrush_->SetOpacity(muted ? 0.24f : 0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 2, 2), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawCapsLock(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 110.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f, rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;
        bool tr = IsTurkish(settings.language);

        ComPtr<ID2D1SolidColorBrush> badgeBg, badgeBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.16f), &badgeBorder);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBorder.Get(), 1.0f);

        const wchar_t* glyph = state.capsLock.isNumEvent ? L"1" : L"A";
        std::wstring label = state.capsLock.isNumEvent ? (tr ? L"Sayı Kilidi" : L"Num Lock") : (tr ? L"Büyük Harf" : L"Caps Lock");
        bool isOn = state.capsLock.isNumEvent ? state.capsLock.numOn : state.capsLock.capsOn;

        textBrush_->SetOpacity(0.95f);
        if (clockFormat_) {
            DrawCenteredGlyph(glyph, badge, clockFormat_.Get(), textBrush_.Get(), false);
        }

        ComPtr<ID2D1SolidColorBrush> ledBrush;
        D2D1_COLOR_F ledColor = isOn ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.22f);
        target_->CreateSolidColorBrush(ledColor, &ledBrush);
        D2D1_POINT_2F ledCenter = D2D1::Point2F(badge.right - 5.0f, badge.bottom - 5.0f);
        target_->FillEllipse(D2D1::Ellipse(ledCenter, 3.0f, 3.0f), ledBrush.Get());

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 10, rect.right - 44, cy + 10);
        if (!label.empty() && smallTextFormat_ && textBrush_) {
            target_->DrawText(label.c_str(), static_cast<UINT32>(label.size()), smallTextFormat_.Get(), labelRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        std::wstring status = isOn ? (tr ? L"AÇIK" : L"ON") : (tr ? L"KAPALI" : L"OFF");
        D2D1_RECT_F statusRect = D2D1::RectF(rect.right - 44, cy - 10, rect.right - 14, cy + 10);
        mutedBrush_->SetOpacity(0.80f);
        if (!status.empty() && smallTextFormat_ && mutedBrush_) {
            target_->DrawText(status.c_str(), static_cast<UINT32>(status.size()), smallTextFormat_.Get(), statusRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }

    void DrawDevice(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 100.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const bool connected = (state.device.eventType == DeviceEventType::Connected);
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f, rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;
        bool tr = IsTurkish(settings.language);

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        if (iconFormat_) {
            DrawCenteredGlyph(connected ? L"\uE88E" : L"\uE711", badge, iconFormat_.Get(), textBrush_.Get(), false);
        }

        ComPtr<ID2D1SolidColorBrush> dotBrush;
        D2D1_COLOR_F dotColor = connected ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f) : D2D1::ColorF(1.0f, 0.27f, 0.22f, 1.0f);
        target_->CreateSolidColorBrush(dotColor, &dotBrush);

        D2D1_POINT_2F dotCenter = D2D1::Point2F(badge.right - 4.5f, badge.bottom - 4.5f);
        target_->FillEllipse(D2D1::Ellipse(dotCenter, 4.5f, 4.5f), dotBrush.Get());

        const float tx = badge.right + 14;
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = connected ? (tr ? L"Cihaz Bağlandı" : L"Device Connected") : (tr ? L"Cihaz Çıkarıldı" : L"Device Removed");
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 14, cy - 5);
        if (!label.empty() && smallTextFormat_ && mutedBrush_) {
            target_->DrawText(label.c_str(), static_cast<UINT32>(label.size()), smallTextFormat_.Get(), labelRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        textBrush_->SetOpacity(0.95f);
        const std::wstring& name = state.device.deviceName.empty()
            ? (state.device.isBluetoothLike ? (tr ? std::wstring(L"Bluetooth Cihazı") : std::wstring(L"Bluetooth Device")) : (tr ? std::wstring(L"USB Cihazı") : std::wstring(L"USB Device")))
            : state.device.deviceName;
        D2D1_RECT_F nameRect = D2D1::RectF(tx, cy - 3, rect.right - 14, cy + 17);
        if (!name.empty() && textFormat_ && textBrush_) {
            target_->DrawText(name.c_str(), static_cast<UINT32>(name.size()), textFormat_.Get(), nameRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawBattery(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f, rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;
        bool tr = IsTurkish(settings.language);

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const float bx = badge.left + badgeSz * 0.25f;
        const float by = cy - badgeSz * 0.22f;
        const float bw = badgeSz * 0.45f;
        const float bh = badgeSz * 0.44f;
        D2D1_RECT_F batRect = D2D1::RectF(bx, by, bx + bw, by + bh);
        
        ComPtr<ID2D1SolidColorBrush> batBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.85f), &batBorder);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(batRect, 2, 2), batBorder.Get(), 1.5f);
        
        D2D1_RECT_F nubRect = D2D1::RectF(batRect.right, cy - 3, batRect.right + 2.5f, cy + 3);
        target_->FillRectangle(nubRect, batBorder.Get());

        const float pct = Clamp(state.battery.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fillRect = D2D1::RectF(batRect.left + 2, batRect.top + 2, batRect.left + 2 + (bw - 4) * pct, batRect.bottom - 2);
        
        ComPtr<ID2D1SolidColorBrush> batFill;
        if (state.battery.low) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &batFill);
        else if (state.battery.charging) target_->CreateSolidColorBrush(D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f), &batFill);
        else target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.95f), &batFill);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, 1, 1), batFill.Get());

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 14, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = state.battery.charging ? (tr ? L"Güç Bağlandı" : L"Power Connected") : (tr ? L"Pil Uyarısı" : L"Battery Alert");
        if (!label.empty() && smallTextFormat_ && mutedBrush_) {
            target_->DrawText(label.c_str(), static_cast<UINT32>(label.size()), smallTextFormat_.Get(), labelRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        wchar_t value[128] = {};
        if (state.battery.secondsRemaining != BATTERY_LIFE_UNKNOWN && !state.battery.charging) {
            const DWORD minutes = state.battery.secondsRemaining / 60;
            if (tr) swprintf_s(value, ARRAYSIZE(value), L"%%%d \u2022 %lusa %02ldk kaldı", state.battery.percent, minutes / 60, minutes % 60);
            else swprintf_s(value, ARRAYSIZE(value), L"%d%% \u2022 %luh %02lum left", state.battery.percent, minutes / 60, minutes % 60);
        } else {
            swprintf_s(value, ARRAYSIZE(value), L"%d%%", state.battery.percent);
        }

        D2D1_RECT_F valueRect = D2D1::RectF(tx, cy - 4, rect.right - 14, cy + 17);
        textBrush_->SetOpacity(0.95f);
        if (wcslen(value) > 0 && textFormat_ && textBrush_) {
            target_->DrawText(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(), valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        textBrush_->SetOpacity(0.90f);
    }

    void DrawProgress(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 100.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f, rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;
        bool tr = IsTurkish(settings.language);

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        // Progress icon glyph or ring
        if (iconFormat_) {
            DrawCenteredGlyph(L"\uE896", badge, iconFormat_.Get(), textBrush_.Get(), false);
        }

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 58, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        const std::wstring procTitle = state.progress.appName.empty() ? (tr ? std::wstring(L"İşlem Sürüyor") : std::wstring(L"Progress")) : state.progress.appName;
        if (!procTitle.empty() && smallTextFormat_ && mutedBrush_) {
            target_->DrawText(procTitle.c_str(), static_cast<UINT32>(procTitle.size()), smallTextFormat_.Get(), labelRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }

        wchar_t value[16] = {};
        swprintf_s(value, L"%d%%", state.progress.percent);
        D2D1_RECT_F valueRect = D2D1::RectF(rect.right - 58, cy - 22, rect.right - 14, cy - 6);
        if (wcslen(value) > 0 && smallTextFormat_ && textBrush_) {
            target_->DrawText(value, static_cast<UINT32>(wcslen(value)), smallTextFormat_.Get(), valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        textBrush_->SetOpacity(0.90f);

        D2D1_RECT_F track = D2D1::RectF(tx, cy + 2, rect.right - 14, cy + 6);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2, 2), trackBrush.Get());
        const float pct = Clamp(state.progress.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top, track.left + (track.right - track.left) * pct, track.bottom);
        accentBrush_->SetOpacity(0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 2, 2), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawProgressRing(D2D1_RECT_F rect, int percent, const Settings& settings) {
        if (!target_ || !d2dFactory_) return;

        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float rx = (rect.right - rect.left) * 0.5f + 4.0f;
        const float ry = (rect.bottom - rect.top) * 0.5f + 4.0f;

        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f * settingsOpacity_), &trackBrush);
        if (trackBrush) {
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry), trackBrush.Get(), settings.progressRingThickness * 0.71f);
        }

        const float clampedPct = Clamp(percent / 100.0f, 0.0f, 1.0f);
        if (clampedPct <= 0.01f) return;

        ComPtr<ID2D1PathGeometry> geometry;
        if (FAILED(d2dFactory_->CreatePathGeometry(&geometry))) return;
        ComPtr<ID2D1GeometrySink> sink;
        if (FAILED(geometry->Open(&sink))) return;

        const float start = -3.14159265f * 0.5f;
        const float sweep = 2.0f * 3.14159265f * clampedPct;
        const int segments = std::max(6, static_cast<int>(64 * clampedPct));

        auto pointAt = [&](float a) {
            return D2D1::Point2F(cx + std::cos(a) * rx, cy + std::sin(a) * ry);
        };

        sink->BeginFigure(pointAt(start), D2D1_FIGURE_BEGIN_HOLLOW);
        for (int i = 1; i <= segments; ++i) {
            const float a = start + sweep * static_cast<float>(i) / segments;
            sink->AddLine(pointAt(a));
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        accentBrush_->SetOpacity(0.95f * settingsOpacity_);
        target_->DrawGeometry(geometry.Get(), accentBrush_.Get(), settings.progressRingThickness);
        accentBrush_->SetOpacity(1.0f);
    }

    bool Render(const SharedState& state, const Settings& settings, const Activity& primary,
                const std::optional<Activity>& secondary, float width, float height,
                float nudge, bool hover, bool pinned, double now) {
        EnsureTextFormats(settings.sizeScale);
        const int pixelWidth = std::max(1, static_cast<int>(std::ceil(width + kRenderPadX * 2.0f)));
        const int pixelHeight = std::max(1, static_cast<int>(std::ceil(height + kRenderPadY * 2.0f)));

        if (!EnsureBackingBitmap(pixelWidth, pixelHeight)) return false;

        RECT rc = {0, 0, pixelWidth, pixelHeight};
        HRESULT hr = target_->BindDC(memDc_, &rc);
        if (FAILED(hr)) return false;

        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));

        EnsureBrushes(settings, state);
        settingsOpacity_ = settings.pillOpacity;

        const float hoverScale = hover || pinned ? settings.hoverScalePercent : 1.0f;
        const float scale = hoverScale;

        const float top = kRenderPadY + nudge;
        const float left = kRenderPadX;
        
        if (width >= 2.0f && height >= 2.0f) {
            if (secondary) {
                const float gap = settings.splitGap * settings.sizeScale;
                const float maxH = std::max(primary.height, secondary->height);
                const float pTop = top + (maxH - primary.height) * 0.5f;
                const float sTop = top + (maxH - secondary->height) * 0.5f;

                DrawPill(state, settings, primary,
                         D2D1::RectF(left, pTop, left + primary.width, pTop + primary.height),
                         scale, now);
                DrawPill(state, settings, *secondary,
                         D2D1::RectF(left + primary.width + gap, sTop,
                                      left + primary.width + gap + secondary->width,
                                      sTop + secondary->height),
                         scale, now);
            } else {
                DrawPill(state, settings, primary,
                         D2D1::RectF(left, top, left + width, top + height), scale, now);
            }
        }

        hr = target_->EndDraw();
        if (FAILED(hr)) return false;

        POINT src = {0, 0};
        POINT dst = ComputeOverlayPosition(pixelWidth, pixelHeight, settings);
        SIZE size = {pixelWidth, pixelHeight};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        return UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend, ULW_ALPHA) != FALSE;
    }
};

void DispatchMediaCommand(int action, int64_t targetTicks = -1) {
    std::thread([action, targetTicks]() {
        try {
            winrt::init_apartment();
            auto sessionManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (sessionManager) {
                auto session = sessionManager.GetCurrentSession();
                if (session) {
                    if (action == 0) session.TrySkipPreviousAsync();
                    else if (action == 1) session.TryTogglePlayPauseAsync();
                    else if (action == 2) session.TrySkipNextAsync();
                    else if (action == 3 && targetTicks >= 0) session.TryChangePlaybackPositionAsync(targetTicks);
                }
            }
        } catch (...) {}
    }).detach();
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool tracking = false;
    static bool isDraggingScrubber = false;

    switch (msg) {
        case WM_MOUSEMOVE: {
            if (!tracking) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                tracking = true;
            }

            int px = GET_X_LPARAM(lParam);

            if (isDraggingScrubber) {
                Settings settings = GetSettingsSnapshot();
                SharedState state;
                {
                    std::lock_guard lock(g_stateMutex);
                    state = g_state;
                }

                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
                float pillW = static_cast<float>(rcClient.right - rcClient.left) - kRenderPadX * 2.0f;
                float pillCenterX = kRenderPadX + pillW * 0.5f;
                float unscaledX = pillCenterX + (px - pillCenterX) / settings.sizeScale;
                float unscaledLeft = pillCenterX - (pillW / settings.sizeScale) * 0.5f;
                float unscaledRight = pillCenterX + (pillW / settings.sizeScale) * 0.5f;

                float barLeft = unscaledLeft + 64.0f;
                float barRight = unscaledRight - 64.0f;
                if (barRight > barLeft && state.media.endTicks > 0) {
                    float pct = Clamp((unscaledX - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
                    int64_t targetTicks = static_cast<int64_t>(pct * state.media.endTicks);
                    DispatchMediaCommand(3, targetTicks);
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            tracking = false;
            isDraggingScrubber = false;
            g_pressedMediaButton = -1;
            return 0;
        }
        case WM_LBUTTONDOWN: {
            // Feature-4: CTRL+Drag to freely move capsule window
            if ((wParam & MK_CONTROL) || (GetKeyState(VK_CONTROL) & 0x8000)) {
                ReleaseCapture();
                SendMessage(hwnd, WM_SYSCOMMAND, 0xF012 /* SC_DRAGMOVE */, 0);
                return 0;
            }

            int px = GET_X_LPARAM(lParam);
            int py = GET_Y_LPARAM(lParam);

            Settings settings = GetSettingsSnapshot();
            SharedState state;
            {
                std::lock_guard lock(g_stateMutex);
                state = g_state;
            }

            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            float pillW = static_cast<float>(rcClient.right - rcClient.left) - kRenderPadX * 2.0f;
            float pillH = static_cast<float>(rcClient.bottom - rcClient.top) - kRenderPadY * 2.0f;
            float pillCenterX = kRenderPadX + pillW * 0.5f;
            float pillCenterY = kRenderPadY + pillH * 0.5f;

            float unscaledX = pillCenterX + (px - pillCenterX) / settings.sizeScale;
            float unscaledY = pillCenterY + (py - pillCenterY) / settings.sizeScale;
            float unscaledLeft = pillCenterX - (pillW / settings.sizeScale) * 0.5f;
            float unscaledRight = pillCenterX + (pillW / settings.sizeScale) * 0.5f;
            float unscaledTop = pillCenterY - (pillH / settings.sizeScale) * 0.5f;

            float expandThreshold = settings.collapsedHeight + std::max(12.0f, (settings.expandedHeight - settings.collapsedHeight) * 0.30f);
            bool isExpandedPill = (pillH / settings.sizeScale > expandThreshold);

            // Feature-5: Interactive Volume Click/Drag
            if (state.volume.expiresAt > NowSeconds()) {
                const float badgeSz = pillH / settings.sizeScale - 16.0f;
                const float tx = unscaledLeft + 14.0f + badgeSz + 14.0f;
                const float trackR = unscaledRight - 14.0f;
                if (unscaledX >= tx && unscaledX <= trackR && trackR > tx) {
                    float newVol = Clamp((unscaledX - tx) / (trackR - tx), 0.0f, 1.0f);
                    SetSystemMasterVolume(newVol);
                    return 0;
                }
            }

            // Expanded Media Transport Controls & Scrubber Hit-Testing
            if (state.media.available && isExpandedPill) {
                float totalH = pillH / settings.sizeScale;
                float artSize = std::clamp(totalH * 0.36f, 36.0f, 68.0f);
                float artTop = unscaledTop + 16.0f;
                float scrubberY = artTop + artSize + 16.0f;
                float cy = scrubberY + (unscaledTop + totalH - scrubberY) * 0.48f;
                float cx = pillCenterX;

                // Prev button (kind = 0)
                if (std::hypot(unscaledX - (cx - 64.0f), unscaledY - cy) <= 22.0f) {
                    g_pressedMediaButton = 0;
                    SetCapture(hwnd);
                    return 0;
                }
                // Play/Pause button (kind = 1)
                if (std::hypot(unscaledX - cx, unscaledY - cy) <= 26.0f) {
                    g_pressedMediaButton = 1;
                    SetCapture(hwnd);
                    return 0;
                }
                // Next button (kind = 2)
                if (std::hypot(unscaledX - (cx + 64.0f), unscaledY - cy) <= 22.0f) {
                    g_pressedMediaButton = 2;
                    SetCapture(hwnd);
                    return 0;
                }

                // Scrubber bar
                float barLeft = unscaledLeft + 64.0f;
                float barRight = unscaledRight - 64.0f;
                if (unscaledY >= scrubberY - 14.0f && unscaledY <= scrubberY + 14.0f &&
                    unscaledX >= barLeft - 6.0f && unscaledX <= barRight + 6.0f) {
                    isDraggingScrubber = true;
                    SetCapture(hwnd);
                    
                    if (barRight > barLeft && state.media.endTicks > 0) {
                        float pct = Clamp((unscaledX - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
                        int64_t targetTicks = static_cast<int64_t>(pct * state.media.endTicks);
                        DispatchMediaCommand(3, targetTicks);
                    }
                    return 0;
                }
            }

            // Collapsed Click-To-Expand
            if (!settings.expandOnHover && !isExpandedPill) {
                Wh_SetIntValue(L"PinnedExpanded", 1);
                g_layoutDirty = true;
                return 0;
            }

            return 0;
        }
        case WM_LBUTTONUP: {
            if (isDraggingScrubber) {
                isDraggingScrubber = false;
                ReleaseCapture();
                return 0;
            }

            int pressed = g_pressedMediaButton.exchange(-1);
            if (pressed != -1) {
                ReleaseCapture();
                DispatchMediaCommand(pressed);
                return 0;
            }

            // Interactive Tab & Widget Click Handling
            Settings settings = GetSettingsSnapshot();
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            float pillH = static_cast<float>(rcClient.bottom - rcClient.top) - kRenderPadY * 2.0f;
            
            if (g_isRecording) {
                StopVoiceRecording();
                return 0;
            }

            float expandThreshold = settings.collapsedHeight + std::max(12.0f, (settings.expandedHeight - settings.collapsedHeight) * 0.30f);
            bool isExpandedPill = (pillH / settings.sizeScale > expandThreshold);

            int currentTab = Modulo(g_idleTab.load(), kTotalTabs);
            if (currentTab == 5 && isExpandedPill) {
                std::lock_guard lock(g_stateMutex);
                if (g_state.pomodoro.state == PomodoroState::Stopped || g_state.pomodoro.state == PomodoroState::Break) {
                    g_state.pomodoro.state = PomodoroState::Working;
                    g_state.pomodoro.lastTick = NowSeconds();
                } else {
                    g_state.pomodoro.state = PomodoroState::Stopped;
                }
                return 0;
            }

            if (isExpandedPill) {
                g_idleTab = Modulo(g_idleTab.load() + 1, kTotalTabs);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt;
            GetCursorPos(&pt);
            ShowContextMenu(hwnd, pt);
            return 0;
        }
        case WM_LBUTTONDBLCLK: {
            int pinned = Wh_GetIntValue(L"PinnedExpanded", 0);
            Wh_SetIntValue(L"PinnedExpanded", pinned ? 0 : 1);
            g_layoutDirty = true;
            return 0;
        }
        case WM_MBUTTONUP: {
            DispatchMediaCommand(1);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);
            int tab = g_idleTab.load();
            if (delta < 0) tab = Modulo(tab + 1, kTotalTabs);
            else if (delta > 0) tab = Modulo(tab - 1, kTotalTabs);
            g_idleTab = tab;
            return 0;
        }
        case WM_APP_NEW_EVENT:
            return 0;
        default:
            if (msg == g_shellHookMessage && g_shellHookMessage != 0) {
                if (wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED) {
                    HWND active = reinterpret_cast<HWND>(lParam);
                    if (active && IsWindow(active)) {
                        wchar_t title[256] = {};
                        GetWindowTextW(active, title, ARRAYSIZE(title));
                        std::lock_guard lock(g_stateMutex);
                        g_state.system.foregroundTitle = title;
                    }
                }
                return 0;
            }
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

// Background Worker Threads

DWORD WINAPI RenderThreadProc(LPVOID) {
    Settings initSettings = GetSettingsSnapshot();
    RECT work = GetAnchorWorkRect();
    const int initialW = static_cast<int>(std::ceil(initSettings.collapsedWidth * initSettings.sizeScale + kRenderPadX * 2.0f));
    const int initialH = static_cast<int>(std::ceil(initSettings.collapsedHeight * initSettings.sizeScale + kRenderPadY * 2.0f));
    int initialX = work.left + (work.right - work.left - initialW) / 2;
    int initialY = work.top + initSettings.edgeMargin;

    // Clean unregister previous class if lingering
    UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWindowClass, L"aegisCapsule",
        WS_POPUP, initialX, initialY, initialW, initialH,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_hwnd) {
        UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
        return 1;
    }

    PositionOverlayWindow(g_hwnd, initialW, initialH, true);
    EnableBlurBehind(g_hwnd);
    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_hwnd);
    g_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(g_hwnd);

    Renderer renderer;
    if (!renderer.Initialize(g_hwnd)) {
        DeregisterShellHookWindow(g_hwnd);
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
        UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
        return 1;
    }

    SpringValue widthSpring;
    SpringValue heightSpring;
    SpringValue nudgeSpring;
    widthSpring.Reset(initSettings.collapsedWidth * initSettings.sizeScale);
    heightSpring.Reset(initSettings.collapsedHeight * initSettings.sizeScale);
    nudgeSpring.Reset(0.0f);

    double lastTime = NowSeconds();
    int frameCount = 0;
    double lastFpsTime = lastTime;
    double lastActiveTime = lastTime;

    while (g_running) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (!g_running) break;

        double now = NowSeconds();
        float dt = static_cast<float>(now - lastTime);
        lastTime = now;

        Settings settings = GetSettingsSnapshot();
        SharedState state;
        {
            std::lock_guard lock(g_stateMutex);
            state = g_state;
        }

        bool isHover = false;
        POINT pt;
        if (GetCursorPos(&pt)) {
            RECT rcWindow;
            GetWindowRect(g_hwnd, &rcWindow);
            if (PtInRect(&rcWindow, pt)) {
                isHover = true;
                lastActiveTime = now;
            }
        }

        static double s_lastFsCheck = 0.0;
        bool isFullScreen = g_isFullScreenActive.load();
        if (settings.fullScreenDetection && (now - s_lastFsCheck >= 0.50)) {
            s_lastFsCheck = now;
            isFullScreen = CheckIsFullScreenActive(GetAnchorMonitor());
            g_isFullScreenActive.store(isFullScreen);
        }

        std::vector<IslandKind> activities = ChooseActivities(state, settings, now);
        IslandKind primaryKind = activities.front();
        bool isAlert = (primaryKind == IslandKind::Clipboard ||
                        primaryKind == IslandKind::Notification ||
                        primaryKind == IslandKind::Volume ||
                        primaryKind == IslandKind::CapsLock ||
                        primaryKind == IslandKind::Device ||
                        primaryKind == IslandKind::BatteryLow);

        if (isAlert || (primaryKind == IslandKind::Media && state.media.playing) || primaryKind == IslandKind::Recording) {
            lastActiveTime = now;
        }

        bool shouldHide = isFullScreen;
        if (!shouldHide) {
            if (settings.autoHideIdleSeconds > 0) {
                if (primaryKind == IslandKind::Idle && !isHover && (now - lastActiveTime > settings.autoHideIdleSeconds)) {
                    shouldHide = true;
                }
            } else if (settings.autoHideIdleSeconds < 0) {
                if (primaryKind == IslandKind::Idle && !isHover) {
                    shouldHide = true;
                }
            }
        }

        bool pinned = Wh_GetIntValue(L"PinnedExpanded", 0) != 0;
        bool isGameOverlayPinned = Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
        bool expand = (isHover && settings.expandOnHover) || pinned || isGameOverlayPinned;

        Activity primary;
        primary.kind = primaryKind;

        if (shouldHide) {
            primary.width = 0.0f;
            primary.height = 0.0f;
        } else if (expand) {
            primary.width = settings.expandedWidth * settings.sizeScale;
            primary.height = settings.expandedHeight * settings.sizeScale;
        } else if (primaryKind == IslandKind::Recording) {
            primary.width = std::max(220.0f, settings.collapsedWidth + 40.0f) * settings.sizeScale;
            primary.height = settings.collapsedHeight * settings.sizeScale;
        } else if (primaryKind == IslandKind::Media) {
            primary.width = (settings.collapsedWidth + 50.0f) * settings.sizeScale;
            primary.height = settings.collapsedHeight * settings.sizeScale;
        } else if (primaryKind == IslandKind::Notification) {
            float baseW = (settings.collapsedWidth + 80.0f) * settings.sizeScale;
            if (settings.notificationAutoExpand) {
                float measuredW = renderer.MeasureNotificationWidth(state.notification.title, state.notification.body);
                baseW = std::clamp(measuredW * settings.sizeScale, baseW, (settings.expandedWidth + 140.0f) * settings.sizeScale);
            }
            primary.width = baseW;
            primary.height = (settings.collapsedHeight + 14.0f) * settings.sizeScale;
        } else if (isAlert) {
            primary.width = (settings.collapsedWidth + 80.0f) * settings.sizeScale;
            primary.height = (settings.collapsedHeight + 14.0f) * settings.sizeScale;
        } else {
            int idleMode = settings.idleDisplayMode;
            bool pomoActive = (state.pomodoro.state == PomodoroState::Working || state.pomodoro.state == PomodoroState::Break);
            if (idleMode == 0) {
                if (pomoActive) idleMode = 5;
                else if (settings.showMetricsInIdle && state.system.cpuPercent >= 0) idleMode = 3;
                else if (state.weather.hasData && (now - state.weather.lastUpdated < 3600.0)) idleMode = 1;
                else idleMode = 2;
            }

            float idleWidth = settings.collapsedWidth;
            if (pomoActive || idleMode == 5) {
                idleWidth = std::max(settings.collapsedWidth + 24.0f, 194.0f);
            } else if (idleMode == 2) {
                idleWidth = std::max(settings.collapsedWidth - 28.0f, 130.0f);
            } else if (idleMode == 3) {
                idleWidth = std::max(settings.collapsedWidth + 48.0f, 218.0f);
            } else if (idleMode == 4) {
                idleWidth = std::max(settings.collapsedWidth + 44.0f, 214.0f);
            } else {
                idleWidth = settings.collapsedWidth;
            }

            primary.width = idleWidth * settings.sizeScale;
            primary.height = settings.collapsedHeight * settings.sizeScale;
        }

        // Pomodoro Timer Tick
        {
            std::lock_guard lock(g_stateMutex);
            if (g_state.pomodoro.state == PomodoroState::Working || g_state.pomodoro.state == PomodoroState::Break) {
                if (now - g_state.pomodoro.lastTick >= 1.0) {
                    int elapsed = static_cast<int>(now - g_state.pomodoro.lastTick);
                    g_state.pomodoro.lastTick = now;
                    g_state.pomodoro.remainingSeconds -= elapsed;
                    if (g_state.pomodoro.remainingSeconds <= 0) {
                        bool tr = IsTurkish(settings.language);
                        if (g_state.pomodoro.state == PomodoroState::Working) {
                            g_state.pomodoro.completedSessions++;
                            g_state.pomodoro.state = PomodoroState::Break;
                            g_state.pomodoro.remainingSeconds = settings.pomodoroBreakMinutes * 60;
                            g_state.pomodoro.totalSeconds = settings.pomodoroBreakMinutes * 60;
                            TriggerCustomAlert(L"Pomodoro Focus", tr ? L"Odaklanma Seansı Bitti! 🎉" : L"Session Complete! 🎉", tr ? L"Mola zamanı. Harika iş çıkardınız!" : L"Time for a break. Great work!", 6.0);
                        } else {
                            g_state.pomodoro.state = PomodoroState::Working;
                            g_state.pomodoro.remainingSeconds = settings.pomodoroWorkMinutes * 60;
                            g_state.pomodoro.totalSeconds = settings.pomodoroWorkMinutes * 60;
                            TriggerCustomAlert(L"Pomodoro Focus", tr ? L"Mola Bitti 🍅" : L"Break Finished 🍅", tr ? L"Tekrar odaklanmaya hazır mısınız? Başlayalım!" : L"Ready to focus again? Let's go!", 6.0);
                        }
                    }
                }
            }
        }

        // Spring Physics Update
        widthSpring.Update(primary.width, settings.springStyle, settings.animationSpeed, dt);
        heightSpring.Update(primary.height, settings.springStyle, settings.animationSpeed, dt);
        {
            double lastNudge = g_lastNudgeTime.load();
            float nudgeTarget = 0.0f;
            if (settings.nudgeOffsetPx > 0.1f && (now - lastNudge) < 0.55) {
                float t = static_cast<float>((now - lastNudge) / 0.55);
                float bounce = std::sin(t * 3.14159f) * (1.0f - t);
                nudgeTarget = -settings.nudgeOffsetPx * bounce;
            }
            nudgeSpring.Update(nudgeTarget, SpringStyle::Bouncy, settings.animationSpeed, dt);
        }

        float curW = widthSpring.GetPosition();
        float curH = heightSpring.GetPosition();
        float curNudge = nudgeSpring.GetPosition();

        if (!expand && curH <= (settings.collapsedHeight * settings.sizeScale + 3.0f)) {
            g_idleTab.store(0);
            renderer.ResetTabTransition();
        }

        renderer.Render(state, settings, primary, std::nullopt, curW, curH, curNudge, isHover, pinned, now);

        frameCount++;
        if (now - lastFpsTime >= 1.0) {
            {
                std::lock_guard lock(g_stateMutex);
                g_state.system.renderFps = frameCount;
            }
            frameCount = 0;
            lastFpsTime = now;
            UpdateSystemMetrics();
            UpdateBatteryMetrics();
        }

        int targetFps = settings.targetFps;
        if (settings.lowPowerMode && !isHover && !state.media.playing) {
            targetFps = 30;
        }
        int frameMs = std::max(1, 1000 / targetFps);
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, frameMs) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(frameMs);
    }

    if (g_hwnd) {
        DeregisterShellHookWindow(g_hwnd);
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
    UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
    ResetPositionOverlayCache();
    return 0;
}

DWORD WINAPI MediaThreadProc(LPVOID) {
    try {
        winrt::init_apartment();
    } catch (...) {}

    static std::wstring s_cachedAumid;
    static BitmapPixels s_cachedSourceIcon;
    static std::wstring s_lastMediaTrackId;
    static BitmapPixels s_cachedAlbumArt;

    while (g_running) {
        try {
            auto sessionManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (sessionManager) {
                auto session = sessionManager.GetCurrentSession();
                if (session) {
                    auto mediaProps = session.TryGetMediaPropertiesAsync().get();
                    auto timeline = session.GetTimelineProperties();
                    auto playbackInfo = session.GetPlaybackInfo();

                    MediaSnapshot snapshot;
                    snapshot.available = true;
                    snapshot.playing = (playbackInfo && playbackInfo.PlaybackStatus() == winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                    
                    if (mediaProps) {
                        snapshot.title = mediaProps.Title().c_str();
                        snapshot.artist = mediaProps.Artist().c_str();
                        
                        std::wstring currentTrackId = snapshot.title + L"|" + snapshot.artist;
                        if (currentTrackId != s_lastMediaTrackId) {
                            s_lastMediaTrackId = currentTrackId;
                            auto thumbnail = mediaProps.Thumbnail();
                            if (thumbnail) {
                                std::vector<uint8_t> bytes = ReadWinRtStreamBytes(thumbnail);
                                if (!bytes.empty()) {
                                    DecodeImageBytesToPixels(bytes, &snapshot.art);
                                    s_cachedAlbumArt = snapshot.art;
                                }
                            }
                        } else {
                            snapshot.art = s_cachedAlbumArt;
                        }
                    }

                    snapshot.sourceAppUserModelId = session.SourceAppUserModelId().c_str();
                    snapshot.sourceName = FriendlyMediaSourceName(snapshot.sourceAppUserModelId);
                    snapshot.sourceBadge = MediaSourceBadge(snapshot.sourceName);

                    if (snapshot.sourceAppUserModelId == s_cachedAumid && !s_cachedSourceIcon.bgra.empty()) {
                        snapshot.sourceIcon = s_cachedSourceIcon;
                    } else {
                        snapshot.sourceIcon = FindMediaSourceIcon(snapshot.sourceAppUserModelId);
                        s_cachedAumid = snapshot.sourceAppUserModelId;
                        s_cachedSourceIcon = snapshot.sourceIcon;
                    }

                    snapshot.positionTicks = timeline.Position().count();
                    snapshot.endTicks = timeline.EndTime().count();
                    snapshot.lastUpdatedTicks = GetTickCount64();

                    {
                        std::lock_guard lock(g_stateMutex);
                        if (g_state.media.title != snapshot.title || g_state.media.artist != snapshot.artist) {
                            snapshot.artChangedAt = NowSeconds();
                        } else {
                            snapshot.artChangedAt = g_state.media.artChangedAt;
                        }
                        g_state.media = snapshot;
                    }
                } else {
                    std::lock_guard lock(g_stateMutex);
                    g_state.media.available = false;
                    g_state.media.playing = false;
                }
            }
        } catch (...) {}

        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(500);
    }
    return 0;
}

DWORD WINAPI AudioThreadProc(LPVOID) {
    try {
        winrt::init_apartment();
    } catch (...) {}

    ComPtr<IMMDeviceEnumerator> enumerator;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (!enumerator) return 1;

    ComPtr<IMMDevice> device;
    enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (!device) return 1;

    ComPtr<IAudioClient> audioClient;
    device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, &audioClient);
    if (!audioClient) return 1;

    WAVEFORMATEX* pwfx = nullptr;
    audioClient->GetMixFormat(&pwfx);
    if (!pwfx) return 1;

    HRESULT hrInit = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 1000000, 0, pwfx, nullptr);
    if (FAILED(hrInit)) {
        if (pwfx) CoTaskMemFree(pwfx);
        return 1;
    }

    ComPtr<IAudioCaptureClient> captureClient;
    HRESULT hrSvc = audioClient->GetService(IID_PPV_ARGS(&captureClient));
    if (FAILED(hrSvc) || !captureClient) {
        if (pwfx) CoTaskMemFree(pwfx);
        return 1;
    }

    HRESULT hrStart = audioClient->Start();
    if (FAILED(hrStart)) {
        if (pwfx) CoTaskMemFree(pwfx);
        return 1;
    }

    while (g_running) {
        UINT32 nextPacketSize = 0;
        HRESULT hr = captureClient->GetNextPacketSize(&nextPacketSize);
        if (SUCCEEDED(hr) && nextPacketSize > 0) {
            BYTE* data = nullptr;
            UINT32 numFramesRead = 0;
            DWORD flags = 0;
            hr = captureClient->GetBuffer(&data, &numFramesRead, &flags, nullptr, nullptr);
            if (SUCCEEDED(hr)) {
                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                    PushAudioChunks(data, numFramesRead, pwfx);
                }
                captureClient->ReleaseBuffer(numFramesRead);
            }
        }
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 15) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(15);
    }

    if (audioClient) audioClient->Stop();
    if (pwfx) CoTaskMemFree(pwfx);
    return 0;
}

DWORD WINAPI WeatherThreadProc(LPVOID) {
    int backoffMins = 0;
    while (g_running) {
        std::wstring cityQuery;
        bool isFahrenheit = false;
        {
            std::lock_guard lock(g_settingsMutex);
            cityQuery = g_settings.weatherCity;
            isFahrenheit = g_settings.weatherFahrenheit;
        }

        std::wstring path = L"/";
        if (!cityQuery.empty()) path += cityQuery;
        path += isFahrenheit ? L"?format=j1&u" : L"?format=j1&m";

        auto Utf8ToWide = [](const std::string& str) -> std::wstring {
            if (str.empty()) return L"";
            int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
            if (wlen <= 0) return L"";
            std::wstring w(wlen, 0);
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &w[0], wlen);
            return w;
        };

        std::string json;
        if (HttpGet(L"wttr.in", path, json)) {
            backoffMins = 0;
            WeatherSnapshot snap;
            snap.hasData = true;
            snap.lastUpdated = NowSeconds();

            auto FindNestedValue = [](const std::string& j, const std::string& sectionKey) -> std::string {
                size_t p = j.find("\"" + sectionKey + "\"");
                if (p == std::string::npos) return "";
                size_t vPos = j.find("\"value\"", p);
                if (vPos == std::string::npos || vPos - p > 200) return "";
                size_t colon = j.find(":", vPos);
                if (colon == std::string::npos) return "";
                size_t startQuote = j.find("\"", colon);
                if (startQuote == std::string::npos) return "";
                size_t endQuote = j.find("\"", startQuote + 1);
                if (endQuote == std::string::npos) return "";
                return j.substr(startQuote + 1, endQuote - startQuote - 1);
            };

            auto FindSimpleValue = [](const std::string& j, const std::string& key) -> std::string {
                size_t p = j.find("\"" + key + "\"");
                if (p == std::string::npos) return "";
                size_t colon = j.find(":", p);
                if (colon == std::string::npos) return "";
                size_t startQuote = j.find("\"", colon);
                if (startQuote != std::string::npos && startQuote - colon < 5) {
                    size_t endQuote = j.find("\"", startQuote + 1);
                    if (endQuote != std::string::npos) {
                        return j.substr(startQuote + 1, endQuote - startQuote - 1);
                    }
                }
                size_t valStart = j.find_first_not_of(" \t\r\n\"", colon + 1);
                if (valStart == std::string::npos) return "";
                size_t valEnd = j.find_first_of(",}\"\r\n", valStart);
                if (valEnd == std::string::npos) return "";
                return j.substr(valStart, valEnd - valStart);
            };

            std::string tempStr = isFahrenheit ? FindSimpleValue(json, "temp_F") : FindSimpleValue(json, "temp_C");
            if (!tempStr.empty()) snap.temperature = static_cast<float>(atof(tempStr.c_str()));

            std::string codeStr = FindSimpleValue(json, "weatherCode");
            if (!codeStr.empty()) snap.weatherCode = atoi(codeStr.c_str());

            std::string descStr = FindNestedValue(json, "weatherDesc");
            if (descStr.empty()) descStr = FindSimpleValue(json, "value");
            if (!descStr.empty()) snap.weatherDesc = Utf8ToWide(descStr);

            std::string windStr = isFahrenheit ? FindSimpleValue(json, "windspeedMiles") : FindSimpleValue(json, "windspeedKmph");
            if (!windStr.empty()) snap.windSpeed = Utf8ToWide(windStr);

            std::string windDirStr = FindSimpleValue(json, "winddir16Point");
            if (!windDirStr.empty()) snap.windDir = Utf8ToWide(windDirStr);

            std::string humStr = FindSimpleValue(json, "humidity");
            if (!humStr.empty()) snap.humidity = Utf8ToWide(humStr);

            std::string feelsStr = isFahrenheit ? FindSimpleValue(json, "FeelsLikeF") : FindSimpleValue(json, "FeelsLikeC");
            if (!feelsStr.empty()) snap.feelsLike = Utf8ToWide(feelsStr);

            std::string areaStr = FindNestedValue(json, "areaName");
            if (!areaStr.empty()) snap.city = Utf8ToWide(areaStr);
            else snap.city = cityQuery.empty() ? L"Current Location" : cityQuery;

            {
                std::lock_guard lock(g_stateMutex);
                g_state.weather = snap;
            }
        } else {
            backoffMins = (backoffMins == 0) ? 1 : std::min(backoffMins * 2, 8);
        }

        int mins = 10;
        {
            std::lock_guard lock(g_settingsMutex);
            mins = g_settings.weatherUpdateMinutes;
        }
        if (backoffMins > 0) mins = backoffMins;

        DWORD waitMs = static_cast<DWORD>(ClampInt(mins, 1, 60) * 60000);
        if (g_stopEvent && g_settingsChangedEvent) {
            HANDLE events[] = { g_stopEvent, g_settingsChangedEvent };
            DWORD waitRes = WaitForMultipleObjects(2, events, FALSE, waitMs);
            if (waitRes == WAIT_OBJECT_0) break;
        } else {
            Sleep(1000);
        }
    }
    return 0;
}

DWORD WINAPI AssistantThreadProc(LPVOID) {
    double lastHydrationTime = NowSeconds();
    double lastEyeRestTime = NowSeconds();
    double lastPostureTime = NowSeconds();
    double lastUptimeAlertTime = NowSeconds();

    LoadQuickNotes();

    while (g_running) {
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 2000) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(2000);
        if (!g_running) break;

        Settings s = GetSettingsSnapshot();

        double now = NowSeconds();
        bool tr = IsTurkish(s.language);

        // Sync upcoming countdown timestamps to state
        {
            std::lock_guard lock(g_stateMutex);
            g_state.assistant.nextHydrationTime = lastHydrationTime + (s.hydrationIntervalMinutes > 0 ? s.hydrationIntervalMinutes * 60.0 : 0.0);
            g_state.assistant.nextEyeRestTime = lastEyeRestTime + (s.eyeRestMinutes > 0 ? s.eyeRestMinutes * 60.0 : 0.0);
            g_state.assistant.nextPostureTime = lastPostureTime + (s.postureIntervalMinutes > 0 ? s.postureIntervalMinutes * 60.0 : 0.0);
        }

        if (!s.enableWellnessReminders) continue;

        // 1. Hydration Reminder
        if (s.hydrationIntervalMinutes > 0 && (now - lastHydrationTime >= s.hydrationIntervalMinutes * 60.0)) {
            lastHydrationTime = now;
            TriggerCustomAlert(L"Living Assistant", tr ? L"Su İçme Kontrolü 💧" : L"Hydration Check 💧",
                               tr ? L"Bir bardak su içme ve vücudunuzu nemlendirme vakti!" : L"Time to drink a glass of water and hydrate!", 6.0);
        }

        // 2. 20-20-20 Eye Rest Reminder
        if (s.eyeRestReminder && s.eyeRestMinutes > 0 && (now - lastEyeRestTime >= s.eyeRestMinutes * 60.0)) {
            lastEyeRestTime = now;
            TriggerCustomAlert(L"Living Assistant", tr ? L"Göz Dinlendirme (20-20-20) 👁️" : L"Eye Rest (20-20-20) 👁️",
                               tr ? L"Gözlerinizi dinlendirmek için 20 saniye boyunca 6 metre uzağa bakın." : L"Look at something 20 feet away for 20 seconds to relax your eyes.", 6.0);
        }

        // 3. Posture Reminder
        if (s.postureReminder && s.postureIntervalMinutes > 0 && (now - lastPostureTime >= s.postureIntervalMinutes * 60.0)) {
            lastPostureTime = now;
            TriggerCustomAlert(L"Living Assistant", tr ? L"Duruş & Esneme 🧘" : L"Posture & Stretch 🧘",
                               tr ? L"Sırtınızı dikleştirin, omuzlarınızı geriye alın ve derin bir nefes alın." : L"Straighten your back, roll your shoulders, and take a deep breath.", 6.0);
        }

        // 4. Long Continuous PC Session Uptime Alert
        if (s.uptimeAlertHours > 0 && (now - lastUptimeAlertTime >= s.uptimeAlertHours * 3600.0)) {
            uint64_t uptimeHours = (GetTickCount64() / 1000) / 3600;
            if (uptimeHours >= static_cast<uint64_t>(s.uptimeAlertHours)) {
                lastUptimeAlertTime = now;
                wchar_t upMsg[128] = {};
                if (tr) {
                    swprintf_s(upMsg, L"%llu saattir kesintisiz bilgisayar başındasınız! Kısa bir yürüyüş yapın ve dinlenin.", uptimeHours);
                } else {
                    swprintf_s(upMsg, L"You've been active for %llu hours! Take a short walk and rest a bit.", uptimeHours);
                }
                TriggerCustomAlert(L"Living Assistant", tr ? L"Uzun Oturum Uyarısı ⏱️" : L"Active Session Alert ⏱️", upMsg, 7.0);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (kbd->vkCode == VK_CAPITAL || kbd->vkCode == VK_NUMLOCK) {
            CapsLockSnapshot snap;
            snap.active = true;
            snap.capsOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
            snap.numOn = (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
            snap.isNumEvent = (kbd->vkCode == VK_NUMLOCK);
            snap.expiresAt = NowSeconds() + GetSettingsSnapshot().capsLockDuration;

            {
                std::lock_guard lock(g_stateMutex);
                g_state.capsLock = snap;
            }
            TriggerNudge();
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyboardThreadProc(LPVOID) {
    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleW(nullptr), 0);
    MSG msg;
    while (g_running && GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

#if AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER
DWORD WINAPI NotificationThreadProc(LPVOID) {
    try { winrt::init_apartment(); } catch (...) {}
    winrt::Windows::UI::Notifications::Management::UserNotificationListener listener = nullptr;
    try {
        listener = winrt::Windows::UI::Notifications::Management::UserNotificationListener::Current();
    } catch (...) {}
    if (!listener) return 1;

    bool accessGranted = false;
    try { accessGranted = (listener.GetAccessStatus() == winrt::Windows::UI::Notifications::Management::UserNotificationListenerAccessStatus::Allowed); } catch (...) {}
    if (!accessGranted) return 1;

    uint32_t lastSeenId = 0;
    while (g_running) {
        try {
            auto notifications = listener.GetNotificationsAsync(winrt::Windows::UI::Notifications::NotificationKinds::Toast).get();
            if (notifications) {
                uint32_t count = notifications.Size();
                for (uint32_t i = 0; i < count; ++i) {
                    auto notif = notifications.GetAt(i);
                    if (!notif) continue;
                    uint32_t nid = notif.Id();
                    if (nid <= lastSeenId) continue;
                    lastSeenId = nid;

                    auto details = notif.Notification();
                    if (!details) continue;

                    std::wstring appTitle, notifTitle, notifBody;
                    try {
                        if (notif.AppInfo() && notif.AppInfo().DisplayInfo()) {
                            appTitle = notif.AppInfo().DisplayInfo().DisplayName().c_str();
                        }
                    } catch (...) {}

                    auto binding = details.Visual().GetBinding(winrt::Windows::UI::Notifications::KnownNotificationBindings::ToastGeneric());
                    if (binding) {
                        auto headLines = binding.GetTextElements();
                        if (headLines && headLines.Size() > 0) notifTitle = headLines.GetAt(0).Text().c_str();
                        if (headLines && headLines.Size() > 1) notifBody = headLines.GetAt(1).Text().c_str();
                    }

                    BitmapPixels iconPx;
                    try {
                        if (notif.AppInfo() && notif.AppInfo().DisplayInfo()) {
                            auto logo = notif.AppInfo().DisplayInfo().GetLogo(winrt::Windows::Foundation::Size(48.0f, 48.0f));
                            if (logo) {
                                auto bytes = ReadWinRtStreamBytes(logo);
                                if (!bytes.empty()) DecodeImageBytesToPixels(bytes, &iconPx);
                            }
                        }
                    } catch (...) {}

                    Settings s = GetSettingsSnapshot();
                    double dur = s.notificationDuration;
                    NotificationSnapshot snap;
                    snap.active = true;
                    snap.app = appTitle.empty() ? L"Notification" : appTitle;
                    snap.title = notifTitle.empty() ? L"New notification" : notifTitle;
                    snap.body = notifBody;
                    snap.icon = iconPx;
                    snap.expiresAt = NowSeconds() + dur;

                    {
                        std::lock_guard lock(g_stateMutex);
                        g_state.notification = snap;
                    }
                    TriggerNudge();
                    break;
                }
            }
        } catch (...) {}
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 2000) == WAIT_OBJECT_0) break;
        else if (!g_stopEvent) Sleep(2000);
    }
    return 0;
}
#endif

void UpdateSystemMetrics() {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto ToInt64 = [](const FILETIME& ft) -> uint64_t {
            return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
        };

        uint64_t idle = ToInt64(idleTime) - ToInt64(g_prevIdleTime);
        uint64_t kernel = ToInt64(kernelTime) - ToInt64(g_prevKernelTime);
        uint64_t user = ToInt64(userTime) - ToInt64(g_prevUserTime);
        uint64_t total = kernel + user;

        if (total > 0) {
            int cpu = static_cast<int>((total - idle) * 100 / total);
            std::lock_guard lock(g_stateMutex);
            g_state.system.cpuPercent = ClampInt(cpu, 0, 100);
        }

        g_prevIdleTime = idleTime;
        g_prevKernelTime = kernelTime;
        g_prevUserTime = userTime;
    }

    MEMORYSTATUSEX mem = {};
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        std::lock_guard lock(g_stateMutex);
        g_state.system.memoryPercent = static_cast<int>(mem.dwMemoryLoad);
    }

    // Detect the actual Windows system drive and query free space using uint64_t to avoid type mismatch
    uint64_t freeBytes = 0;
    uint64_t totalBytes = 0;
    wchar_t sysDir[MAX_PATH] = {};
    const wchar_t* diskRoot = L"C:\\"; // fallback
    if (GetWindowsDirectoryW(sysDir, ARRAYSIZE(sysDir)) > 0 && sysDir[0] && sysDir[1] == L':') {
        sysDir[2] = L'\\'; sysDir[3] = L'\0'; // ensure trailing backslash
        diskRoot = sysDir;
    }
    using GetDiskFreeSpaceExW_t = BOOL(WINAPI*)(LPCWSTR, void*, void*, void*);
    static auto pGetDiskFreeSpaceExW = reinterpret_cast<GetDiskFreeSpaceExW_t>(
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetDiskFreeSpaceExW"));
    if (pGetDiskFreeSpaceExW && pGetDiskFreeSpaceExW(diskRoot, &freeBytes, &totalBytes, nullptr) && totalBytes > 0) {
        int freePct = static_cast<int>(freeBytes * 100 / totalBytes);
        std::lock_guard lock(g_stateMutex);
        g_state.system.diskFreePercent = freePct;
    }

    int gpu = GetGpuUsage();
    {
        std::lock_guard lock(g_stateMutex);
        g_state.system.gpuPercent = gpu;
    }

    UpdatePrivacyIndicators();
}

void UpdateBatteryMetrics() {
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        bool charging = (sps.ACLineStatus == 1);
        bool low = (sps.BatteryFlag & 1) || (sps.BatteryLifePercent <= 20 && !charging);
        
        // Lock once for all g_state writes to avoid data race on battery fields
        std::lock_guard lock(g_stateMutex);
        g_state.system.charging = charging;

        if (sps.BatteryLifePercent <= 100) {
            g_state.battery.percent = sps.BatteryLifePercent;
            g_state.battery.charging = charging;
            g_state.battery.low = low;
            g_state.battery.secondsRemaining = sps.BatteryLifeTime;
        }
    }
}

// Windhawk Module Lifecycle

std::atomic<bool> g_modInitialized{false};

BOOL WhTool_ModInit() {
    g_running = true;
    if (!g_stopEvent) g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    else ResetEvent(g_stopEvent);

    if (!g_settingsChangedEvent) g_settingsChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    else ResetEvent(g_settingsChangedEvent);

    LoadSettings();
    Settings initSettings = GetSettingsSnapshot();
    if (initSettings.suppressSystemToasts) {
        SetSystemToastSuppression(true);
    }

    g_renderThread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0, &g_renderThreadId);
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0, nullptr);
    g_weatherThread = CreateThread(nullptr, 0, WeatherThreadProc, nullptr, 0, nullptr);
    g_assistantThread = CreateThread(nullptr, 0, AssistantThreadProc, nullptr, 0, nullptr);
    g_keyboardThread = CreateThread(nullptr, 0, KeyboardThreadProc, nullptr, 0, &g_keyboardThreadId);
#if AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER
    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
#endif
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    Settings currentSettings = GetSettingsSnapshot();
    SetSystemToastSuppression(currentSettings.suppressSystemToasts);
    if (g_hwnd) EnableBlurBehind(g_hwnd);
    if (g_settingsChangedEvent) SetEvent(g_settingsChangedEvent);
    g_layoutDirty = true;
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_settingsChangedEvent) SetEvent(g_settingsChangedEvent);

    // Restore Windows native toast popups on unload
    SetSystemToastSuppression(false);

    if (g_renderThreadId) {
        PostThreadMessageW(g_renderThreadId, WM_QUIT, 0, 0);
    }
    if (g_keyboardThreadId) {
        PostThreadMessageW(g_keyboardThreadId, WM_QUIT, 0, 0);
    }

    StopVoiceRecording();

    HANDLE threads[] = {
        g_renderThread, g_mediaThread, g_audioThread, g_weatherThread,
        g_assistantThread, g_recordingThread, g_keyboardThread
#if AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER
        , g_notificationThread
#endif
    };

    for (HANDLE& t : threads) {
        if (t) {
            WaitForSingleObject(t, 2000);
            CloseHandle(t);
            t = nullptr;
        }
    }
    g_renderThread = nullptr;
    g_renderThreadId = 0;
    g_mediaThread = nullptr;
    g_audioThread = nullptr;
    g_weatherThread = nullptr;
    g_assistantThread = nullptr;
    g_recordingThread = nullptr;
    g_keyboardThread = nullptr;
    g_keyboardThreadId = 0;
#if AEGIS_CAPSULE_HAS_USER_NOTIFICATION_LISTENER
    g_notificationThread = nullptr;
#endif

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_settingsChangedEvent) {
        CloseHandle(g_settingsChangedEvent);
        g_settingsChangedEvent = nullptr;
    }
    if (g_gpuQuery) {
        PdhCloseQuery(g_gpuQuery);
        g_gpuQuery = NULL;
    }
}

void StartMod() {
    bool expected = false;
    if (!g_modInitialized.compare_exchange_strong(expected, true)) {
        return;
    }
    WhTool_ModInit();
}

void StopMod() {
    bool expected = true;
    if (!g_modInitialized.compare_exchange_strong(expected, false)) {
        return;
    }
    WhTool_ModUninit();
}

} // namespace

// Windhawk Entry Points

BOOL Wh_ModInit() {
    Wh_Log(L"aegisCapsule: Initializing module...");
    StartMod();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"aegisCapsule: ModAfterInit called.");
    // Windhawk calls AfterInit after all hooks are applied. StartMod() is safe to call
    // again here — g_modInitialized (atomic) prevents double initialization.
    StartMod();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"aegisCapsule: Settings updated.");
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    Wh_Log(L"aegisCapsule: Shutting down module.");
    StopMod();
}
