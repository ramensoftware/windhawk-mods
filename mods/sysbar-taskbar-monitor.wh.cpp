// ==WindhawkMod==
// @id              sysbar-taskbar-monitor
// @name            SysBar - Taskbar Monitor
// @description     CPU and GPU load and temperature, RAM usage and network speed with icons, embedded into the Windows 11 taskbar
// @name:ru-RU      SysBar — монитор панели задач
// @description:ru-RU Загрузка и температура CPU и GPU, использование памяти и скорость сети в панели задач Windows 11.
// @name:ar         SysBar — مراقب شريط المهام
// @description:ar  يعرض حمل المعالج ومعالج الرسومات ودرجة حرارتهما، واستخدام الذاكرة وسرعة الشبكة في شريط مهام Windows 11.
// @name:bn         SysBar — টাস্কবার মনিটর
// @description:bn  Windows 11-এর টাস্কবারে CPU ও GPU-এর ব্যবহার ও তাপমাত্রা, RAM-এর ব্যবহার এবং নেটওয়ার্কের গতি দেখায়।
// @name:de         SysBar – Taskleistenmonitor
// @description:de  Zeigt Auslastung und Temperatur von CPU und GPU, RAM-Auslastung und Netzwerkgeschwindigkeit in der Windows-11-Taskleiste an.
// @name:es         SysBar — Monitor de la barra de tareas
// @description:es  Muestra el uso y la temperatura de la CPU y la GPU, el uso de RAM y la velocidad de red en la barra de tareas de Windows 11.
// @name:fr         SysBar — Moniteur de la barre des tâches
// @description:fr  Affiche la charge et la température du CPU et du GPU, la mémoire utilisée et le débit réseau dans la barre des tâches de Windows 11.
// @name:hi         SysBar — टास्कबार मॉनिटर
// @description:hi  Windows 11 के टास्कबार पर CPU और GPU का उपयोग और तापमान, RAM का उपयोग और नेटवर्क की गति दिखाता है।
// @name:id         SysBar — Monitor bilah tugas
// @description:id  Menampilkan beban dan suhu CPU serta GPU, penggunaan RAM, dan kecepatan jaringan di bilah tugas Windows 11.
// @name:it         SysBar — Monitor della barra delle applicazioni
// @description:it  Mostra il carico e la temperatura di CPU e GPU, l’utilizzo della RAM e la velocità di rete nella barra delle applicazioni di Windows 11.
// @name:ja         SysBar — タスクバーモニター
// @description:ja  Windows 11 のタスクバーに CPU・GPU の使用率と温度、メモリ使用率、ネットワーク速度を表示します。
// @name:ko         SysBar — 작업 표시줄 모니터
// @description:ko  Windows 11 작업 표시줄에 CPU와 GPU의 사용률 및 온도, 메모리 사용률, 네트워크 속도를 표시합니다.
// @name:nl         SysBar — Taakbalkmonitor
// @description:nl  Toont de belasting en temperatuur van CPU en GPU, het RAM-gebruik en de netwerksnelheid op de taakbalk van Windows 11.
// @name:pl         SysBar — Monitor paska zadań
// @description:pl  Wyświetla obciążenie i temperaturę CPU i GPU, użycie pamięci RAM oraz prędkość sieci na pasku zadań Windows 11.
// @name:pt-BR      SysBar — Monitor da barra de tarefas
// @description:pt-BR Exibe o uso e a temperatura da CPU e da GPU, o uso de RAM e a velocidade da rede na barra de tarefas do Windows 11.
// @name:pt-PT      SysBar — Monitor da barra de tarefas
// @description:pt-PT Mostra a utilização e a temperatura do CPU e do GPU, a utilização da RAM e a velocidade da rede na barra de tarefas do Windows 11.
// @name:tr         SysBar — Görev çubuğu izleyicisi
// @description:tr  Windows 11 görev çubuğunda CPU ve GPU kullanımını ve sıcaklıklarını, RAM kullanımını ve ağ hızını gösterir.
// @name:uk         SysBar — Монітор панелі завдань
// @description:uk  Показує завантаження й температуру процесора та відеокарти, використання оперативної пам’яті й швидкість мережі на панелі завдань Windows 11.
// @name:vi         SysBar — Trình theo dõi trên thanh tác vụ
// @description:vi  Hiển thị mức sử dụng và nhiệt độ của CPU và GPU, mức sử dụng RAM và tốc độ mạng trên thanh tác vụ Windows 11.
// @name:zh-CN      SysBar — 任务栏监视器
// @description:zh-CN 在 Windows 11 任务栏上显示 CPU 和 GPU 使用率与温度、内存使用率以及网络速度。
// @name:zh-TW      SysBar — 工作列監視器
// @description:zh-TW 在 Windows 11 工作列上顯示 CPU 和 GPU 使用率與溫度、記憶體使用率及網路速度。
// @version         1.2.1
// @author          Brov3r
// @github          https://github.com/Brov3r
// @homepage        https://github.com/Brov3r/SysBar
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -liphlpapi -lpdh -lwbemuuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# SysBar
Displays CPU, GPU, memory and network readings on the primary Windows 11
taskbar. CPU/GPU groups show **load · temperature**; yellow and red mark the
configured temperature thresholds. Unavailable temperatures are hidden.

GPU load uses NVIDIA NVML or Windows performance counters. If the optional
SysBar Sensor is unavailable, SysBar tries Open Hardware Monitor, then the
built-in Windows ACPI and thermal-zone performance providers. A thermal-zone
reading is not necessarily the CPU package temperature. If Windows exposes no
usable reading, the temperature stays hidden.

![SysBar on the Windows 11 taskbar](https://raw.githubusercontent.com/Brov3r/SysBar/v1.1.1/docs/sysbar-demo.png)

On Windows 11 x64, enable the mod and open **Settings** to choose which
readings to display. Memory can show percentage used, used GiB or available
GiB. Enable uptime to see hours and minutes since the last restart.

[Taskbar System Info](https://windhawk.net/mods/taskbar-system-info) already
provides CPU, GPU, memory and network statistics. SysBar is an alternative
for users who want uptime alongside those readings, scalable drawn icons,
and access to an optional, separately installed thermal-zone sensor through
read-only shared memory. It overlays the area left of the notification tray;
it does not reserve space for taskbar buttons.

The [optional sensor and installation instructions](https://github.com/Brov3r/SysBar#-quick-start)
are only needed if you want to try the helper for temperature readings.
For help or bug reports, use the [issue tracker](https://github.com/Brov3r/SysBar/issues).

License: MIT. Icon reference: [Nieobie's game-icon-pack](https://github.com/Nieobie/game-icon-pack) (CC0-1.0).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- metrics:
  - hideCpu: false
    $name: Hide CPU
    $name:ru-RU: Скрыть процессор (CPU)
    $name:de: "CPU ausblenden"
    $name:es: "Ocultar CPU"
    $name:fr: "Masquer le CPU"
    $name:it: "Nascondi CPU"
    $name:pt-BR: "Ocultar CPU"
    $name:pt-PT: "Ocultar CPU"
    $name:nl: "CPU verbergen"
    $name:pl: "Ukryj CPU"
    $name:uk: "Приховати процесор"
    $name:tr: "CPU’yu gizle"
    $name:id: "Sembunyikan CPU"
    $name:vi: "Ẩn CPU"
    $name:ja: "CPU を非表示にする"
    $name:ko: "CPU 숨기기"
    $name:zh-CN: "隐藏 CPU"
    $name:zh-TW: "隱藏 CPU"
    $name:ar: "إخفاء المعالج"
    $name:hi: "CPU छिपाएँ"
    $name:bn: "CPU লুকান"
  - hideGpu: false
    $name: Hide GPU
    $name:ru-RU: Скрыть видеокарту (GPU)
    $name:de: "GPU ausblenden"
    $name:es: "Ocultar GPU"
    $name:fr: "Masquer le GPU"
    $name:it: "Nascondi GPU"
    $name:pt-BR: "Ocultar GPU"
    $name:pt-PT: "Ocultar GPU"
    $name:nl: "GPU verbergen"
    $name:pl: "Ukryj GPU"
    $name:uk: "Приховати відеокарту"
    $name:tr: "GPU’yu gizle"
    $name:id: "Sembunyikan GPU"
    $name:vi: "Ẩn GPU"
    $name:ja: "GPU を非表示にする"
    $name:ko: "GPU 숨기기"
    $name:zh-CN: "隐藏 GPU"
    $name:zh-TW: "隱藏 GPU"
    $name:ar: "إخفاء معالج الرسومات"
    $name:hi: "GPU छिपाएँ"
    $name:bn: "GPU লুকান"
  - hideRam: false
    $name: Hide memory
    $name:ru-RU: Скрыть оперативную память
    $name:de: "Arbeitsspeicher ausblenden"
    $name:es: "Ocultar memoria"
    $name:fr: "Masquer la mémoire"
    $name:it: "Nascondi memoria"
    $name:pt-BR: "Ocultar memória"
    $name:pt-PT: "Ocultar memória"
    $name:nl: "Geheugen verbergen"
    $name:pl: "Ukryj pamięć"
    $name:uk: "Приховати оперативну пам’ять"
    $name:tr: "Belleği gizle"
    $name:id: "Sembunyikan memori"
    $name:vi: "Ẩn bộ nhớ"
    $name:ja: "メモリを非表示にする"
    $name:ko: "메모리 숨기기"
    $name:zh-CN: "隐藏内存"
    $name:zh-TW: "隱藏記憶體"
    $name:ar: "إخفاء الذاكرة"
    $name:hi: "मेमोरी छिपाएँ"
    $name:bn: "মেমরি লুকান"
  - ramDisplay: percent
    $name: Memory reading
    $name:ru-RU: Что показывать для памяти
    $name:de: "Speicheranzeige"
    $name:es: "Indicador de memoria"
    $name:fr: "Indicateur de mémoire"
    $name:it: "Indicatore di memoria"
    $name:pt-BR: "Indicador de memória"
    $name:pt-PT: "Indicador de memória"
    $name:nl: "Geheugenweergave"
    $name:pl: "Wskaźnik pamięci"
    $name:uk: "Показник пам’яті"
    $name:tr: "Bellek göstergesi"
    $name:id: "Indikator memori"
    $name:vi: "Chỉ số bộ nhớ"
    $name:ja: "メモリの表示内容"
    $name:ko: "메모리 표시 방식"
    $name:zh-CN: "内存显示方式"
    $name:zh-TW: "記憶體顯示方式"
    $name:ar: "مؤشر الذاكرة"
    $name:hi: "मेमोरी संकेतक"
    $name:bn: "মেমরি সূচক"
    $options:
    - percent: Usage (%)
    - used: Used (GiB)
    - free: Available (GiB)
    $options:ru-RU:
    - percent: Занято (%)
    - used: Занято (ГиБ)
    - free: Доступно (ГиБ)
    $options:de:
    - percent: "Belegt (%)"
    - used: "Belegt (GiB)"
    - free: "Verfügbar (GiB)"
    $options:es:
    - percent: "En uso (%)"
    - used: "En uso (GiB)"
    - free: "Disponible (GiB)"
    $options:fr:
    - percent: "Utilisée (%)"
    - used: "Utilisée (Gio)"
    - free: "Disponible (Gio)"
    $options:it:
    - percent: "In uso (%)"
    - used: "In uso (GiB)"
    - free: "Disponibile (GiB)"
    $options:pt-BR:
    - percent: "Em uso (%)"
    - used: "Em uso (GiB)"
    - free: "Disponível (GiB)"
    $options:pt-PT:
    - percent: "Utilizada (%)"
    - used: "Utilizada (GiB)"
    - free: "Disponível (GiB)"
    $options:nl:
    - percent: "In gebruik (%)"
    - used: "In gebruik (GiB)"
    - free: "Beschikbaar (GiB)"
    $options:pl:
    - percent: "Zajęta (%)"
    - used: "Zajęta (GiB)"
    - free: "Dostępna (GiB)"
    $options:uk:
    - percent: "Зайнято (%)"
    - used: "Зайнято (ГіБ)"
    - free: "Доступно (ГіБ)"
    $options:tr:
    - percent: "Kullanılan (%)"
    - used: "Kullanılan (GiB)"
    - free: "Kullanılabilir (GiB)"
    $options:id:
    - percent: "Terpakai (%)"
    - used: "Terpakai (GiB)"
    - free: "Tersedia (GiB)"
    $options:vi:
    - percent: "Đang dùng (%)"
    - used: "Đang dùng (GiB)"
    - free: "Khả dụng (GiB)"
    $options:ja:
    - percent: "使用率（%）"
    - used: "使用中（GiB）"
    - free: "利用可能（GiB）"
    $options:ko:
    - percent: "사용률(%)"
    - used: "사용 중(GiB)"
    - free: "사용 가능(GiB)"
    $options:zh-CN:
    - percent: "已用百分比 (%)"
    - used: "已用容量 (GiB)"
    - free: "可用容量 (GiB)"
    $options:zh-TW:
    - percent: "已用百分比 (%)"
    - used: "已用容量 (GiB)"
    - free: "可用容量 (GiB)"
    $options:ar:
    - percent: "المستخدم (%)"
    - used: "المستخدم (GiB)"
    - free: "المتاح (GiB)"
    $options:hi:
    - percent: "उपयोग (%)"
    - used: "उपयोग में (GiB)"
    - free: "उपलब्ध (GiB)"
    $options:bn:
    - percent: "ব্যবহৃত (%)"
    - used: "ব্যবহৃত (GiB)"
    - free: "উপলব্ধ (GiB)"
  - showNet: true
    $name: Show download and upload speeds
    $name:ru-RU: Показывать скорость скачивания и отдачи
    $name:de: "Download- und Uploadrate anzeigen"
    $name:es: "Mostrar velocidades de descarga y subida"
    $name:fr: "Afficher les débits descendant et montant"
    $name:it: "Mostra velocità di download e upload"
    $name:pt-BR: "Mostrar velocidades de download e upload"
    $name:pt-PT: "Mostrar velocidades de receção e envio"
    $name:nl: "Download- en uploadsnelheid tonen"
    $name:pl: "Pokazuj prędkość pobierania i wysyłania"
    $name:uk: "Показувати швидкість завантаження й віддачі"
    $name:tr: "İndirme ve yükleme hızlarını göster"
    $name:id: "Tampilkan kecepatan unduh dan unggah"
    $name:vi: "Hiện tốc độ tải xuống và tải lên"
    $name:ja: "ダウンロード速度とアップロード速度を表示"
    $name:ko: "다운로드 및 업로드 속도 표시"
    $name:zh-CN: "显示下载和上传速度"
    $name:zh-TW: "顯示下載與上傳速度"
    $name:ar: "عرض سرعتي التنزيل والرفع"
    $name:hi: "डाउनलोड और अपलोड की गति दिखाएँ"
    $name:bn: "ডাউনলোড ও আপলোডের গতি দেখান"
  - showUptime: false
    $name: Show uptime (hours:minutes)
    $name:ru-RU: Показывать время после перезагрузки (часы:минуты)
    $name:de: "Laufzeit anzeigen (Stunden:Minuten)"
    $name:es: "Mostrar tiempo de actividad (horas:minutos)"
    $name:fr: "Afficher le temps depuis le démarrage (heures:minutes)"
    $name:it: "Mostra tempo di attività (ore:minuti)"
    $name:pt-BR: "Mostrar tempo de atividade (horas:minutos)"
    $name:pt-PT: "Mostrar tempo de atividade (horas:minutos)"
    $name:nl: "Tijd sinds opstarten tonen (uren:minuten)"
    $name:pl: "Pokazuj czas pracy (godziny:minuty)"
    $name:uk: "Показувати час роботи (години:хвилини)"
    $name:tr: "Çalışma süresini göster (saat:dakika)"
    $name:id: "Tampilkan waktu aktif (jam:menit)"
    $name:vi: "Hiện thời gian hoạt động (giờ:phút)"
    $name:ja: "稼働時間を表示（時間:分）"
    $name:ko: "가동 시간 표시(시간:분)"
    $name:zh-CN: "显示系统运行时间（小时:分钟）"
    $name:zh-TW: "顯示系統運作時間（小時:分鐘）"
    $name:ar: "عرض مدة التشغيل (ساعات:دقائق)"
    $name:hi: "चालू रहने की अवधि दिखाएँ (घंटे:मिनट)"
    $name:bn: "চালু থাকার সময় দেখান (ঘণ্টা:মিনিট)"
  $name: Readings
  $name:ru-RU: Показатели
  $name:de: "Messwerte"
  $name:es: "Indicadores"
  $name:fr: "Indicateurs"
  $name:it: "Indicatori"
  $name:pt-BR: "Indicadores"
  $name:pt-PT: "Indicadores"
  $name:nl: "Meetwaarden"
  $name:pl: "Wskaźniki"
  $name:uk: "Показники"
  $name:tr: "Göstergeler"
  $name:id: "Indikator"
  $name:vi: "Chỉ số"
  $name:ja: "表示項目"
  $name:ko: "표시 항목"
  $name:zh-CN: "显示指标"
  $name:zh-TW: "顯示指標"
  $name:ar: "المؤشرات"
  $name:hi: "संकेतक"
  $name:bn: "সূচক"
- appearance:
  - fontSize: 12
    $name: Text size (8–24 DIPs)
    $name:ru-RU: Размер текста (8–24 логических пикселя)
    $name:de: "Schriftgröße (8–24 DIP)"
    $name:es: "Tamaño del texto (8–24 DIP)"
    $name:fr: "Taille du texte (8–24 DIP)"
    $name:it: "Dimensione testo (8–24 DIP)"
    $name:pt-BR: "Tamanho do texto (8–24 DIP)"
    $name:pt-PT: "Tamanho do texto (8–24 DIP)"
    $name:nl: "Tekstgrootte (8–24 DIP)"
    $name:pl: "Rozmiar tekstu (8–24 DIP)"
    $name:uk: "Розмір тексту (8–24 DIP)"
    $name:tr: "Yazı boyutu (8–24 DIP)"
    $name:id: "Ukuran teks (8–24 DIP)"
    $name:vi: "Cỡ chữ (8–24 DIP)"
    $name:ja: "文字サイズ（8～24 DIP）"
    $name:ko: "글자 크기(8~24 DIP)"
    $name:zh-CN: "文字大小（8–24 DIP）"
    $name:zh-TW: "文字大小（8–24 DIP）"
    $name:ar: "حجم النص (8–24 DIP)"
    $name:hi: "टेक्स्ट का आकार (8–24 DIP)"
    $name:bn: "লেখার আকার (8–24 DIP)"
  - showIcons: true
    $name: Show metric icons and network arrows
    $name:ru-RU: Показывать значки показателей и стрелки сети
    $name:de: "Symbole und Netzwerkpfeile anzeigen"
    $name:es: "Mostrar iconos y flechas de red"
    $name:fr: "Afficher les icônes et les flèches réseau"
    $name:it: "Mostra icone e frecce di rete"
    $name:pt-BR: "Mostrar ícones e setas de rede"
    $name:pt-PT: "Mostrar ícones e setas de rede"
    $name:nl: "Pictogrammen en netwerkpijlen tonen"
    $name:pl: "Pokazuj ikony i strzałki sieci"
    $name:uk: "Показувати піктограми й мережеві стрілки"
    $name:tr: "Simgeleri ve ağ oklarını göster"
    $name:id: "Tampilkan ikon dan panah jaringan"
    $name:vi: "Hiện biểu tượng và mũi tên mạng"
    $name:ja: "アイコンと通信方向の矢印を表示"
    $name:ko: "아이콘과 네트워크 화살표 표시"
    $name:zh-CN: "显示指标图标和网络箭头"
    $name:zh-TW: "顯示指標圖示與網路箭頭"
    $name:ar: "عرض الرموز وأسهم الشبكة"
    $name:hi: "आइकन और नेटवर्क के तीर दिखाएँ"
    $name:bn: "আইকন ও নেটওয়ার্কের তীর দেখান"
  - metricGap: 13
    $name: Space between visible groups (0–64 DIPs)
    $name:ru-RU: Интервал между видимыми группами (0–64 логических пикселя)
    $name:de: "Abstand zwischen sichtbaren Gruppen (0–64 DIP)"
    $name:es: "Espacio entre grupos visibles (0–64 DIP)"
    $name:fr: "Espace entre groupes visibles (0–64 DIP)"
    $name:it: "Spazio tra gruppi visibili (0–64 DIP)"
    $name:pt-BR: "Espaço entre grupos visíveis (0–64 DIP)"
    $name:pt-PT: "Espaço entre grupos visíveis (0–64 DIP)"
    $name:nl: "Afstand tussen zichtbare groepen (0–64 DIP)"
    $name:pl: "Odstęp między widocznymi grupami (0–64 DIP)"
    $name:uk: "Відстань між видимими групами (0–64 DIP)"
    $name:tr: "Görünür gruplar arasındaki boşluk (0–64 DIP)"
    $name:id: "Jarak antarkelompok yang terlihat (0–64 DIP)"
    $name:vi: "Khoảng cách giữa các nhóm hiển thị (0–64 DIP)"
    $name:ja: "表示グループ間の余白（0～64 DIP）"
    $name:ko: "표시 그룹 사이 간격(0~64 DIP)"
    $name:zh-CN: "可见分组之间的间距（0–64 DIP）"
    $name:zh-TW: "可見群組之間的間距（0–64 DIP）"
    $name:ar: "المسافة بين المجموعات الظاهرة (0–64 DIP)"
    $name:hi: "दिख रहे समूहों के बीच अंतर (0–64 DIP)"
    $name:bn: "দৃশ্যমান গোষ্ঠীগুলির মধ্যের ফাঁক (0–64 DIP)"
  - trayGap: 6
    $name: Gap to the notification area (0–128 DIPs)
    $name:ru-RU: Отступ от системного трея справа (0–128 логических пикселей)
    $name:de: "Abstand zum Infobereich (0–128 DIP)"
    $name:es: "Distancia al área de notificación (0–128 DIP)"
    $name:fr: "Distance à la zone de notification (0–128 DIP)"
    $name:it: "Distanza dall’area di notifica (0–128 DIP)"
    $name:pt-BR: "Distância até a área de notificação (0–128 DIP)"
    $name:pt-PT: "Distância à área de notificação (0–128 DIP)"
    $name:nl: "Afstand tot het systeemvak (0–128 DIP)"
    $name:pl: "Odstęp od obszaru powiadomień (0–128 DIP)"
    $name:uk: "Відступ від області сповіщень (0–128 DIP)"
    $name:tr: "Bildirim alanına uzaklık (0–128 DIP)"
    $name:id: "Jarak ke area notifikasi (0–128 DIP)"
    $name:vi: "Khoảng cách đến vùng thông báo (0–128 DIP)"
    $name:ja: "通知領域との間隔（0～128 DIP）"
    $name:ko: "알림 영역까지의 간격(0~128 DIP)"
    $name:zh-CN: "距通知区域的间距（0–128 DIP）"
    $name:zh-TW: "與通知區域的間距（0–128 DIP）"
    $name:ar: "المسافة إلى منطقة الإعلام (0–128 DIP)"
    $name:hi: "सूचना क्षेत्र से दूरी (0–128 DIP)"
    $name:bn: "বিজ্ঞপ্তি এলাকা থেকে দূরত্ব (0–128 DIP)"
  $name: Appearance and spacing
  $name:ru-RU: Оформление и отступы
  $name:de: "Darstellung und Abstände"
  $name:es: "Aspecto y espaciado"
  $name:fr: "Apparence et espacement"
  $name:it: "Aspetto e spaziatura"
  $name:pt-BR: "Aparência e espaçamento"
  $name:pt-PT: "Aspeto e espaçamento"
  $name:nl: "Weergave en afstanden"
  $name:pl: "Wygląd i odstępy"
  $name:uk: "Оформлення та відступи"
  $name:tr: "Görünüm ve aralıklar"
  $name:id: "Tampilan dan jarak"
  $name:vi: "Giao diện và khoảng cách"
  $name:ja: "外観と間隔"
  $name:ko: "모양 및 간격"
  $name:zh-CN: "外观与间距"
  $name:zh-TW: "外觀與間距"
  $name:ar: "المظهر والتباعد"
  $name:hi: "दिखावट और अंतर"
  $name:bn: "চেহারা ও ব্যবধান"
- temperature:
  - showTemps: true
    $name: Show available temperatures next to CPU and GPU load
    $name:ru-RU: Показывать доступную температуру рядом с загрузкой CPU и GPU
    $name:de: "Verfügbare Temperaturen neben der CPU- und GPU-Auslastung anzeigen"
    $name:es: "Mostrar las temperaturas disponibles junto al uso de CPU y GPU"
    $name:fr: "Afficher les températures disponibles à côté de la charge CPU et GPU"
    $name:it: "Mostra le temperature disponibili accanto al carico di CPU e GPU"
    $name:pt-BR: "Mostrar temperaturas disponíveis ao lado do uso da CPU e da GPU"
    $name:pt-PT: "Mostrar as temperaturas disponíveis junto à utilização do CPU e do GPU"
    $name:nl: "Beschikbare temperaturen naast CPU- en GPU-belasting tonen"
    $name:pl: "Pokazuj dostępne temperatury obok obciążenia CPU i GPU"
    $name:uk: "Показувати доступні температури поруч із завантаженням CPU і GPU"
    $name:tr: "CPU ve GPU kullanımının yanında mevcut sıcaklıkları göster"
    $name:id: "Tampilkan suhu yang tersedia di samping beban CPU dan GPU"
    $name:vi: "Hiện nhiệt độ hiện có bên cạnh mức sử dụng CPU và GPU"
    $name:ja: "取得できた温度を CPU・GPU 使用率の横に表示"
    $name:ko: "CPU 및 GPU 사용률 옆에 확인 가능한 온도 표시"
    $name:zh-CN: "在 CPU 和 GPU 使用率旁显示可用的温度读数"
    $name:zh-TW: "在 CPU 和 GPU 使用率旁顯示可取得的溫度"
    $name:ar: "عرض درجات الحرارة المتاحة بجوار حمل المعالج ومعالج الرسومات"
    $name:hi: "CPU और GPU के उपयोग के पास उपलब्ध तापमान दिखाएँ"
    $name:bn: "CPU ও GPU ব্যবহারের পাশে উপলব্ধ তাপমাত্রা দেখান"
  - warnTemp: 75
    $name: Warning color from (30–120 °C)
    $name:ru-RU: Порог предупреждения (30–120 °C)
    $name:de: "Warnschwelle (30–120 °C)"
    $name:es: "Umbral de aviso (30–120 °C)"
    $name:fr: "Seuil d’avertissement (30–120 °C)"
    $name:it: "Soglia di avviso (30–120 °C)"
    $name:pt-BR: "Limite de aviso (30–120 °C)"
    $name:pt-PT: "Limiar de aviso (30–120 °C)"
    $name:nl: "Waarschuwingsdrempel (30–120 °C)"
    $name:pl: "Próg ostrzeżenia (30–120 °C)"
    $name:uk: "Поріг попередження (30–120 °C)"
    $name:tr: "Uyarı eşiği (30–120 °C)"
    $name:id: "Ambang peringatan (30–120 °C)"
    $name:vi: "Ngưỡng cảnh báo (30–120 °C)"
    $name:ja: "警告温度（30～120 °C）"
    $name:ko: "경고 온도(30~120 °C)"
    $name:zh-CN: "警告温度（30–120 °C）"
    $name:zh-TW: "警示溫度（30–120 °C）"
    $name:ar: "حد التحذير (30–120 °C)"
    $name:hi: "चेतावनी की सीमा (30–120 °C)"
    $name:bn: "সতর্কতার সীমা (30–120 °C)"
    $description: Amber in dark theme, brown in light theme.
    $description:ru-RU: Янтарный цвет в тёмной теме, коричневый в светлой.
    $description:de: "Bernsteinfarben im dunklen Design, braun im hellen."
    $description:es: "Ámbar en el tema oscuro y marrón en el claro."
    $description:fr: "Ambre en thème sombre, brun en thème clair."
    $description:it: "Ambra nel tema scuro, marrone in quello chiaro."
    $description:pt-BR: "Âmbar no tema escuro e marrom no tema claro."
    $description:pt-PT: "Âmbar no tema escuro e castanho no tema claro."
    $description:nl: "Amber in het donkere thema, bruin in het lichte."
    $description:pl: "Bursztynowy w ciemnym motywie, brązowy w jasnym."
    $description:uk: "Бурштиновий у темній темі, коричневий у світлій."
    $description:tr: "Koyu temada kehribar, açık temada kahverengi."
    $description:id: "Kuning ambar pada tema gelap, cokelat pada tema terang."
    $description:vi: "Màu hổ phách trong giao diện tối, màu nâu trong giao diện sáng."
    $description:ja: "ダークテーマでは琥珀色、ライトテーマでは茶色になります。"
    $description:ko: "어두운 테마에서는 호박색, 밝은 테마에서는 갈색입니다."
    $description:zh-CN: "深色主题下为琥珀色，浅色主题下为棕色。"
    $description:zh-TW: "深色佈景主題中為琥珀色，淺色佈景主題中為棕色。"
    $description:ar: "كهرماني في النسق الداكن وبني في النسق الفاتح."
    $description:hi: "गहरे थीम में अंबर और हल्के थीम में भूरा रंग।"
    $description:bn: "গাঢ় থিমে অ্যাম্বার, হালকা থিমে বাদামি।"
  - critTemp: 88
    $name: Red color from (°C)
    $name:ru-RU: Порог красной подсветки (°C)
    $name:de: "Rote Anzeige ab (°C)"
    $name:es: "Mostrar en rojo a partir de (°C)"
    $name:fr: "Seuil d’affichage en rouge (°C)"
    $name:it: "Soglia del colore rosso (°C)"
    $name:pt-BR: "Exibir em vermelho a partir de (°C)"
    $name:pt-PT: "Mostrar a vermelho a partir de (°C)"
    $name:nl: "Rood vanaf (°C)"
    $name:pl: "Próg koloru czerwonego (°C)"
    $name:uk: "Поріг червоного кольору (°C)"
    $name:tr: "Kırmızı renk eşiği (°C)"
    $name:id: "Ambang warna merah (°C)"
    $name:vi: "Ngưỡng chuyển sang màu đỏ (°C)"
    $name:ja: "赤く表示する温度（°C）"
    $name:ko: "빨간색으로 표시할 온도(°C)"
    $name:zh-CN: "标红温度（°C）"
    $name:zh-TW: "顯示為紅色的溫度（°C）"
    $name:ar: "حد العرض باللون الأحمر (°C)"
    $name:hi: "लाल रंग की सीमा (°C)"
    $name:bn: "লাল রঙের সীমা (°C)"
    $description: At least 1 degree above the warning threshold, up to 130 °C.
    $description:ru-RU: Минимум на 1 °C выше предупреждения, максимум 130 °C.
    $description:de: "Mindestens 1 °C über der Warnschwelle, höchstens 130 °C."
    $description:es: "Al menos 1 °C por encima del aviso, hasta 130 °C."
    $description:fr: "Au moins 1 °C au-dessus du seuil d’avertissement, jusqu’à 130 °C."
    $description:it: "Almeno 1 °C sopra la soglia di avviso, fino a 130 °C."
    $description:pt-BR: "Pelo menos 1 °C acima do limite de aviso, até 130 °C."
    $description:pt-PT: "Pelo menos 1 °C acima do limiar de aviso, até 130 °C."
    $description:nl: "Minstens 1 °C boven de waarschuwingsdrempel, maximaal 130 °C."
    $description:pl: "Co najmniej 1 °C powyżej progu ostrzeżenia, maksymalnie 130 °C."
    $description:uk: "Принаймні на 1 °C вище порога попередження, максимум 130 °C."
    $description:tr: "Uyarı eşiğinin en az 1 °C üzerinde, en fazla 130 °C."
    $description:id: "Minimal 1 °C di atas ambang peringatan, maksimal 130 °C."
    $description:vi: "Cao hơn ngưỡng cảnh báo ít nhất 1 °C, tối đa 130 °C."
    $description:ja: "警告温度より 1 °C 以上高く、上限は 130 °C です。"
    $description:ko: "경고 온도보다 1 °C 이상 높아야 하며 최대 130 °C입니다."
    $description:zh-CN: "至少比警告温度高 1 °C，最高 130 °C。"
    $description:zh-TW: "至少比警示溫度高 1 °C，最高 130 °C。"
    $description:ar: "أعلى من حد التحذير بدرجة مئوية واحدة على الأقل، وبحد أقصى 130 °C."
    $description:hi: "चेतावनी की सीमा से कम से कम 1 °C अधिक, अधिकतम 130 °C।"
    $description:bn: "সতর্কতার সীমার চেয়ে অন্তত 1 °C বেশি, সর্বোচ্চ 130 °C।"
  $name: Temperatures and alerts
  $name:ru-RU: Температуры и предупреждения
  $name:de: "Temperaturen und Warnungen"
  $name:es: "Temperaturas y avisos"
  $name:fr: "Températures et alertes"
  $name:it: "Temperature e avvisi"
  $name:pt-BR: "Temperaturas e alertas"
  $name:pt-PT: "Temperaturas e avisos"
  $name:nl: "Temperaturen en waarschuwingen"
  $name:pl: "Temperatury i ostrzeżenia"
  $name:uk: "Температури й попередження"
  $name:tr: "Sıcaklıklar ve uyarılar"
  $name:id: "Suhu dan peringatan"
  $name:vi: "Nhiệt độ và cảnh báo"
  $name:ja: "温度と警告"
  $name:ko: "온도 및 경고"
  $name:zh-CN: "温度与警告"
  $name:zh-TW: "溫度與警示"
  $name:ar: "درجات الحرارة والتنبيهات"
  $name:hi: "तापमान और चेतावनियाँ"
  $name:bn: "তাপমাত্রা ও সতর্কতা"
- operation:
  - intervalSeconds: 1
    $name: Refresh interval (1–10 seconds)
    $name:ru-RU: Интервал обновления (1–10 секунд)
    $name:de: "Aktualisierungsintervall (1–10 Sekunden)"
    $name:es: "Intervalo de actualización (1–10 segundos)"
    $name:fr: "Intervalle d’actualisation (1–10 secondes)"
    $name:it: "Intervallo di aggiornamento (1–10 secondi)"
    $name:pt-BR: "Intervalo de atualização (1–10 segundos)"
    $name:pt-PT: "Intervalo de atualização (1–10 segundos)"
    $name:nl: "Verversingsinterval (1–10 seconden)"
    $name:pl: "Odstęp odświeżania (1–10 sekund)"
    $name:uk: "Інтервал оновлення (1–10 секунд)"
    $name:tr: "Yenileme aralığı (1–10 saniye)"
    $name:id: "Interval pembaruan (1–10 detik)"
    $name:vi: "Chu kỳ làm mới (1–10 giây)"
    $name:ja: "更新間隔（1～10 秒）"
    $name:ko: "새로 고침 간격(1~10초)"
    $name:zh-CN: "刷新间隔（1–10 秒）"
    $name:zh-TW: "更新間隔（1–10 秒）"
    $name:ar: "فترة التحديث (1–10 ثوانٍ)"
    $name:hi: "अपडेट का अंतराल (1–10 सेकंड)"
    $name:bn: "হালনাগাদের ব্যবধান (1–10 সেকেন্ড)"
    $description: WMI temperature readings are queried at most once every 5 seconds.
    $description:ru-RU: Температура через WMI опрашивается не чаще раза в 5 секунд.
    $description:de: "WMI-Temperaturen werden höchstens alle 5 Sekunden abgefragt."
    $description:es: "Las temperaturas por WMI se consultan como máximo una vez cada 5 segundos."
    $description:fr: "Les températures WMI sont relevées au plus une fois toutes les 5 secondes."
    $description:it: "Le temperature WMI vengono lette al massimo una volta ogni 5 secondi."
    $description:pt-BR: "As temperaturas via WMI são consultadas no máximo uma vez a cada 5 segundos."
    $description:pt-PT: "As temperaturas por WMI são consultadas no máximo uma vez a cada 5 segundos."
    $description:nl: "WMI-temperaturen worden hoogstens eens per 5 seconden opgevraagd."
    $description:pl: "Temperatury przez WMI są odczytywane najwyżej raz na 5 sekund."
    $description:uk: "Температури через WMI зчитуються не частіше ніж раз на 5 секунд."
    $description:tr: "WMI sıcaklıkları 5 saniyede en fazla bir kez okunur."
    $description:id: "Suhu melalui WMI dibaca paling sering sekali setiap 5 detik."
    $description:vi: "Nhiệt độ qua WMI được đọc tối đa một lần mỗi 5 giây."
    $description:ja: "WMI による温度取得は最短でも 5 秒間隔です。"
    $description:ko: "WMI 온도는 5초에 한 번 이하로 조회합니다."
    $description:zh-CN: "通过 WMI 获取温度的间隔至少为 5 秒。"
    $description:zh-TW: "透過 WMI 讀取溫度的間隔至少為 5 秒。"
    $description:ar: "تُقرأ درجات الحرارة عبر WMI مرة واحدة كل 5 ثوانٍ على الأكثر."
    $description:hi: "WMI से तापमान पाँच सेकंड में अधिकतम एक बार पढ़ा जाता है।"
    $description:bn: "WMI দিয়ে প্রতি 5 সেকেন্ডে সর্বোচ্চ একবার তাপমাত্রা পড়া হয়।"
  $name: Refresh
  $name:ru-RU: Обновление
  $name:de: Aktualisierung
  $name:es: Actualización
  $name:fr: Actualisation
  $name:it: Aggiornamento
  $name:pt-BR: Atualização
  $name:pt-PT: Atualização
  $name:nl: Vernieuwen
  $name:pl: Odświeżanie
  $name:uk: Оновлення
  $name:tr: Yenileme
  $name:id: Pembaruan
  $name:vi: Làm mới
  $name:ja: 更新
  $name:ko: 새로 고침
  $name:zh-CN: 刷新
  $name:zh-TW: 更新
  $name:ar: التحديث
  $name:hi: अपडेट
  $name:bn: হালনাগাদ
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <wbemidl.h>
#include <oleauto.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cwchar>
#include <limits>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;

using winrt::Windows::UI::Color;

namespace {

constexpr wchar_t kHostName[] = L"SysBarMonitorWidget";
constexpr UINT kRunOnTaskbarThreadMessage = WM_APP + 0x5B1;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class RamDisplay { Percent, Used, Free };
struct Settings {
    bool showCpu = true, showGpu = true, showRam = true, showUptime = false;
    RamDisplay ramDisplay = RamDisplay::Percent;
    int fontSize = 12;
    bool showTemps = true;
    bool showNet = true;
    bool showIcons = true;
    int metricGap = 13, trayGap = 6;
    int warnTemp = 75;
    int critTemp = 88;
    int intervalSeconds = 1;
};

Settings g_settings;
std::mutex g_settingsMutex;
Settings CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}


void LoadSettings() {
    // Values are zero when the mod has never been configured (for example,
    // when installed manually). Fall back to sane defaults in that case.
    const int fontSize = Wh_GetIntSetting(L"appearance.fontSize");
    const int showTemps = Wh_GetIntSetting(L"temperature.showTemps");
    const int showNet = Wh_GetIntSetting(L"metrics.showNet");
    const int showIcons = Wh_GetIntSetting(L"appearance.showIcons");
    const int warnTemp = Wh_GetIntSetting(L"temperature.warnTemp");
    const int critTemp = Wh_GetIntSetting(L"temperature.critTemp");

    const bool unconfigured = fontSize == 0 && showTemps == 0 && showNet == 0 &&
                              showIcons == 0 && warnTemp == 0 && critTemp == 0;

    Settings s;
    s.showCpu = !Wh_GetIntSetting(L"metrics.hideCpu");
    s.showGpu = !Wh_GetIntSetting(L"metrics.hideGpu");
    s.showRam = !Wh_GetIntSetting(L"metrics.hideRam");
    s.showUptime = Wh_GetIntSetting(L"metrics.showUptime") != 0;
    if (auto mode = Wh_GetStringSetting(L"metrics.ramDisplay")) {
        if (!wcscmp(mode, L"used")) s.ramDisplay = RamDisplay::Used;
        if (!wcscmp(mode, L"free")) s.ramDisplay = RamDisplay::Free;
        Wh_FreeStringSetting(mode);
    }
    s.fontSize = std::clamp(unconfigured ? 12 : fontSize, 8, 24);
    s.showTemps = unconfigured || showTemps != 0;
    s.showNet = unconfigured || showNet != 0;
    s.showIcons = unconfigured || showIcons != 0;
    s.metricGap = std::clamp(unconfigured ? 13 : Wh_GetIntSetting(L"appearance.metricGap"), 0, 64);
    s.trayGap = std::clamp(unconfigured ? 6 : Wh_GetIntSetting(L"appearance.trayGap"), 0, 128);
    s.warnTemp = std::clamp(unconfigured ? 75 : warnTemp, 30, 120);
    s.critTemp = std::clamp(unconfigured ? 88 : critTemp, s.warnTemp + 1, 130);
    s.intervalSeconds = std::clamp(Wh_GetIntSetting(L"operation.intervalSeconds"), 1, 10);
    std::lock_guard lock(g_settingsMutex);
    g_settings = s;
}

// ---------------------------------------------------------------------------
// Metrics snapshot (produced by a worker thread, consumed by the taskbar UI)
// ---------------------------------------------------------------------------

struct Snapshot {
    double cpuPct = -1;
    double cpuTemp = -1;
    double gpuPct = -1;
    double gpuTemp = -1;
    double ramPct = -1;
    double usedGiB = -1, freeGiB = -1;
    double netDown = -1;
    double netUp = -1;
    bool gpuAvailable = false;
    bool netAvailable = false;
};

std::mutex g_snapMutex;
Snapshot g_snap;

[[clang::no_destroy]] std::optional<std::thread> g_worker;
std::atomic<bool> g_workerStop{false};
std::mutex g_workerWakeMutex;
std::condition_variable g_workerWake;

// ---------------------------------------------------------------------------
// Win32 helpers
// ---------------------------------------------------------------------------

uint64_t FileTimeToU64(const FILETIME& ft) {
    return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

std::optional<double> ReadCpuLoad() {
    static bool initialized = false;
    static uint64_t prevIdle = 0, prevKernel = 0, prevUser = 0;

    FILETIME idle{}, kernel{}, user{};
    if (!GetSystemTimes(&idle, &kernel, &user)) {
        return std::nullopt;
    }

    const uint64_t idleNow = FileTimeToU64(idle);
    const uint64_t kernelNow = FileTimeToU64(kernel);
    const uint64_t userNow = FileTimeToU64(user);

    if (!initialized) {
        initialized = true;
        prevIdle = idleNow;
        prevKernel = kernelNow;
        prevUser = userNow;
        return std::nullopt;
    }

    const uint64_t idleDelta = idleNow - prevIdle;
    const uint64_t kernelDelta = kernelNow - prevKernel;
    const uint64_t userDelta = userNow - prevUser;
    prevIdle = idleNow;
    prevKernel = kernelNow;
    prevUser = userNow;

    const uint64_t total = kernelDelta + userDelta;
    if (!total || idleDelta > total) {
        return std::nullopt;
    }
    return std::clamp(100.0 * static_cast<double>(total - idleDelta) /
                          static_cast<double>(total),
                      0.0, 100.0);
}

void ReadRam(Snapshot& snapshot) {
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (!GlobalMemoryStatusEx(&memory)) {
        return;
    }
    snapshot.ramPct = memory.dwMemoryLoad;
    constexpr double gib = 1024.0 * 1024 * 1024;
    snapshot.freeGiB = memory.ullAvailPhys / gib;
    snapshot.usedGiB = (memory.ullTotalPhys - memory.ullAvailPhys) / gib;
}

// Published by the tiny SYSTEM helper (tools/SysBarSensor.cs) through a
// global shared-memory section, because the ACPI thermal zone can only be
// queried with administrative rights.
struct SensorSharedMemory {
    volatile LONG sequence;
    volatile LONG tick;
    double temperature;
};

struct SensorReader {
    winrt::handle mapping;
    SensorSharedMemory* view = nullptr;
    ~SensorReader() { if (view) UnmapViewOfFile(view); }

    std::optional<double> Read() {
        if (!view) {
            mapping.attach(OpenFileMappingW(FILE_MAP_READ, FALSE, L"Global\\SysBarSensors"));
            if (!mapping) return std::nullopt;
            view = static_cast<SensorSharedMemory*>(
                MapViewOfFile(mapping.get(), FILE_MAP_READ, 0, 0, sizeof(SensorSharedMemory)));
            if (!view) {
                mapping.close();
                return std::nullopt;
            }
        }

        const LONG before = view->sequence;
        MemoryBarrier();
        const LONG tick = view->tick;
        const double value = view->temperature;
        MemoryBarrier();
        const LONG after = view->sequence;

        const DWORD age = GetTickCount() - static_cast<DWORD>(tick);
        if (age > 15000) {
            UnmapViewOfFile(view);
            mapping.close();
            view = nullptr;
            return std::nullopt;
        }
        if (before != after || (before & 1) || !(value >= 1 && value <= 150))
            return std::nullopt;
        return value;
    }
};

struct NetCounters {
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> previous;
    std::chrono::steady_clock::time_point time{};
};

bool IsRealNetworkAdapter(const MIB_IF_ROW2& row) {
    if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK || row.Type == IF_TYPE_TUNNEL) {
        return false;
    }
    if (row.OperStatus != IfOperStatusUp) {
        return false;
    }
    return true;
}

std::pair<double, double> ReadNetwork(NetCounters& state, bool& available) {
    available = false;

    MIB_IF_TABLE2* table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR || !table) {
        state = {};
        return {};
    }

    const auto now = std::chrono::steady_clock::now();
    std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> current;
    uint64_t received = 0, sent = 0;
    bool comparable = false;

    for (ULONG i = 0; i < table->NumEntries; i++) {
        const MIB_IF_ROW2& row = table->Table[i];
        if (!IsRealNetworkAdapter(row)) {
            continue;
        }
        const uint64_t luid = row.InterfaceLuid.Value;
        current[luid] = {row.InOctets, row.OutOctets};

        auto it = state.previous.find(luid);
        if (it != state.previous.end() && row.InOctets >= it->second.first &&
            row.OutOctets >= it->second.second) {
            comparable = true;
            received += row.InOctets - it->second.first;
            sent += row.OutOctets - it->second.second;
        }
    }
    FreeMibTable(table);

    const double seconds =
        std::chrono::duration<double>(now - state.time).count();
    if (comparable && state.time != std::chrono::steady_clock::time_point{} &&
        seconds >= 0.2) {
        available = true;
        state.previous = std::move(current);
        state.time = now;
        return {static_cast<double>(received) / seconds,
                static_cast<double>(sent) / seconds};
    }

    state.previous = std::move(current);
    state.time = now;
    return {};
}

// ---------------------------------------------------------------------------
// GPU metrics: NVML first, PDH "\GPU Engine(*)\Utilization Percentage" fallback
// ---------------------------------------------------------------------------

struct NvmlGpu {
    HMODULE lib = nullptr;
    void* device = nullptr;
    using InitFn = int (*)();
    using CountFn = int (*)(unsigned*);
    using HandleFn = int (*)(unsigned, void**);
    using TempFn = int (*)(void*, int, int*);
    using UtilFn = int (*)(void*, void*);

    InitFn init = nullptr;
    InitFn shutdown = nullptr;
    bool initialized = false;
    ~NvmlGpu() {
        if (initialized) shutdown();
        if (lib) FreeLibrary(lib);
    }
    CountFn count = nullptr;
    HandleFn handle = nullptr;
    TempFn temperature = nullptr;
    UtilFn utilization = nullptr;
    bool failed = false;
    std::chrono::steady_clock::time_point nextRetry{};
    char name[96]{};

    bool Ensure() {
        if (device) {
            return true;
        }
        if (failed && std::chrono::steady_clock::now() < nextRetry) {
            return false;
        }
        if (!lib) {
            lib = LoadLibraryExW(L"nvml.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        if (!lib) {
            Wh_Log(L"nvml: LoadLibrary failed, error=%lu", GetLastError());
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        init = reinterpret_cast<InitFn>(GetProcAddress(lib, "nvmlInit_v2"));
        shutdown = reinterpret_cast<InitFn>(GetProcAddress(lib, "nvmlShutdown"));
        count =
            reinterpret_cast<CountFn>(GetProcAddress(lib, "nvmlDeviceGetCount_v2"));
        handle = reinterpret_cast<HandleFn>(
            GetProcAddress(lib, "nvmlDeviceGetHandleByIndex_v2"));
        temperature = reinterpret_cast<TempFn>(
            GetProcAddress(lib, "nvmlDeviceGetTemperature"));
        utilization = reinterpret_cast<UtilFn>(
            GetProcAddress(lib, "nvmlDeviceGetUtilizationRates"));
        if (!init || !shutdown || !count || !handle || !temperature || !utilization) {
            Wh_Log(L"nvml: missing exports init=%d count=%d handle=%d temp=%d util=%d",
                     init != nullptr, count != nullptr, handle != nullptr,
                     temperature != nullptr, utilization != nullptr);
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        const int initResult = initialized ? 0 : init();
        if (initResult != 0) {
            Wh_Log(L"nvml: init failed, code=%d", initResult);
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        unsigned deviceCount = 0;
        initialized = true;
        const int countResult = count(&deviceCount);
        if (countResult != 0 || deviceCount == 0) {
            Wh_Log(L"nvml: count failed, code=%d count=%u", countResult,
                     deviceCount);
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        const int handleResult = handle(0, &device);
        if (handleResult != 0) {
            Wh_Log(L"nvml: handle failed, code=%d", handleResult);
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        using NameFn = int (*)(void*, char*, unsigned);
        if (auto getName = reinterpret_cast<NameFn>(
                GetProcAddress(lib, "nvmlDeviceGetName"))) {
            getName(device, name, sizeof(name));
        }
        Wh_Log(L"nvml: ready, gpu=%S", name);
        return true;
    }

    struct Util {
        unsigned gpu;
        unsigned memory;
    };

    bool Sample(double& util, double& temp, bool readTemp) {
        if (!device) {
            return false;
        }
        int t = -1;
        Util u{};
        const int tempResult = readTemp ? temperature(device, 0, &t) : -1;
        const int utilResult = utilization(device, &u);

        const bool tempValid = tempResult == 0 && t > 0 && t <= 150;
        const bool utilValid = utilResult == 0;

        if (!tempValid && !utilValid) {
            const auto now = std::chrono::steady_clock::now();
            if (now >= lastLog) {
                lastLog = now + std::chrono::seconds(30);
                Wh_Log(L"nvml: sample failed temp=%d/%d util=%d",
                         tempResult, t, utilResult);
            }
        }
        if (tempValid) {
            temp = static_cast<double>(t);
        }
        if (utilValid) {
            util = std::clamp(static_cast<double>(u.gpu), 0.0, 100.0);
        }
        return tempValid || utilValid;
    }

    std::chrono::steady_clock::time_point lastLog{};
};

struct PdhGpuLoad {
    PDH_HQUERY query = nullptr;
    ~PdhGpuLoad() { if (query) PdhCloseQuery(query); }
    std::vector<BYTE> buffer;
    PDH_HCOUNTER counter = nullptr;
    bool failed = false;
    std::chrono::steady_clock::time_point nextRetry{};

    bool Ensure() {
        if (counter) {
            return true;
        }
        if (failed && std::chrono::steady_clock::now() < nextRetry) {
            return false;
        }
        failed = false;
        if (query) {
            PdhCloseQuery(query);
            query = nullptr;
        }
        if (PdhOpenQueryW(nullptr, 0, &query) != ERROR_SUCCESS) {
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        if (PdhAddEnglishCounterW(query,
                                  L"\\GPU Engine(*)\\Utilization Percentage", 0,
                                  &counter) != ERROR_SUCCESS) {
            PdhCloseQuery(query);
            query = nullptr;
            counter = nullptr;
            failed = true;
            nextRetry = std::chrono::steady_clock::now() +
                        std::chrono::seconds(60);
            return false;
        }
        PdhCollectQueryData(query);
        return true;
    }

    std::optional<double> Sample() {
        if (!Ensure() || PdhCollectQueryData(query) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        DWORD bufferSize = 0, itemCount = 0;
        PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize,
                                     &itemCount, nullptr);
        if (!bufferSize || !itemCount) {
            return std::nullopt;
        }

        buffer.resize(bufferSize);
        auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
        if (PdhGetFormattedCounterArrayW(counter, PDH_FMT_DOUBLE, &bufferSize,
                                         &itemCount, items) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        std::unordered_map<std::wstring, double> engines;
        double busiest = -1;
        for (DWORD i = 0; i < itemCount; i++) {
            const wchar_t* instance = items[i].szName;
            const auto engine = instance ? wcsstr(instance, L"luid_") : nullptr;
            if (!engine) {
                continue;
            }
            if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA &&
                items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) {
                continue;
            }
            auto& load = engines[engine];  // Same adapter/engine across process IDs.
            load += items[i].FmtValue.doubleValue;
            busiest = std::max(busiest, load);
        }
        if (busiest < 0) return std::nullopt;
        return std::clamp(busiest, 0.0, 100.0);
    }
};

// ---------------------------------------------------------------------------
// Temperature sources: LibreHardwareMonitor / OpenHardwareMonitor WMI, ACPI
// ---------------------------------------------------------------------------

class WmiConnection {
   public:
    WmiConnection() = default;
    ~WmiConnection() { Close(); }

    WmiConnection(const WmiConnection&) = delete;
    WmiConnection& operator=(const WmiConnection&) = delete;

    bool Ensure(const wchar_t* ns) {
        if (services_) {
            return true;
        }
        if (failed_ && std::chrono::steady_clock::now() < nextRetry_) {
            return false;
        }

        IWbemLocator* locator = nullptr;
        if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr,
                                    CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                    reinterpret_cast<void**>(&locator)))) {
            MarkFailed();
            return false;
        }
        BSTR path = SysAllocString(ns);
        HRESULT hr = locator->ConnectServer(
            path, nullptr, nullptr, nullptr, 0, nullptr, nullptr, &services_);
        SysFreeString(path);
        locator->Release();
        if (FAILED(hr) || !services_) {
            Close();
            MarkFailed();
            return false;
        }
        CoSetProxyBlanket(services_, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                          nullptr, RPC_C_AUTHN_LEVEL_CALL,
                          RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
        failed_ = false;
        return true;
    }

    // Returns hottest CPU sensor, if any sensor name looks like a CPU one.
    std::optional<double> QueryCpuTemperature() {
        if (!services_) {
            return std::nullopt;
        }
        IEnumWbemClassObject* enumerator = nullptr;
        BSTR language = SysAllocString(L"WQL");
        BSTR query = SysAllocString(
            L"SELECT Name, SensorType, Value FROM Sensor WHERE SensorType='Temperature'");
        HRESULT hr = services_->ExecQuery(
            language, query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
            &enumerator);
        SysFreeString(language);
        SysFreeString(query);
        if (FAILED(hr) || !enumerator) {
            Close();
            MarkFailed();
            return std::nullopt;
        }

        std::optional<double> best;
        IWbemClassObject* object = nullptr;
        ULONG returned = 0;
        while (!g_workerStop && enumerator->Next(500, 1, &object, &returned) == S_OK &&
               returned == 1) {
            VARIANT nameVar, typeVar, valueVar;
            VariantInit(&nameVar);
            VariantInit(&typeVar);
            VariantInit(&valueVar);
            object->Get(L"Name", 0, &nameVar, nullptr, nullptr);
            object->Get(L"SensorType", 0, &typeVar, nullptr, nullptr);
            object->Get(L"Value", 0, &valueVar, nullptr, nullptr);

            std::wstring name = nameVar.vt == VT_BSTR && nameVar.bstrVal
                                    ? nameVar.bstrVal
                                    : L"";
            std::wstring type = typeVar.vt == VT_BSTR && typeVar.bstrVal
                                    ? typeVar.bstrVal
                                    : L"";
            double value = -1;
            if (valueVar.vt == VT_R4) {
                value = valueVar.fltVal;
            } else if (valueVar.vt == VT_R8) {
                value = valueVar.dblVal;
            }

            VariantClear(&nameVar);
            VariantClear(&typeVar);
            VariantClear(&valueVar);
            object->Release();

            if (type != L"Temperature" || !(value > 0 && value <= 150)) {
                continue;
            }
            std::wstring lower = name;
            CharLowerW(lower.data());
            if (lower.find(L"gpu") != std::wstring::npos) {
                continue;
            }
            const bool cpuLike =
                lower.find(L"cpu") != std::wstring::npos ||
                lower.find(L"tctl") != std::wstring::npos ||
                lower.find(L"tdie") != std::wstring::npos ||
                lower.find(L"ccd") != std::wstring::npos ||
                lower.find(L"core (") != std::wstring::npos;
            if (!cpuLike) {
                continue;
            }
            if (!best || value > *best) {
                best = value;
            }
        }
        enumerator->Release();
        return best;
    }

    std::optional<double> QueryZoneTemperature(bool performanceCounters) {
        if (!services_) {
            return std::nullopt;
        }
        IEnumWbemClassObject* enumerator = nullptr;
        BSTR language = SysAllocString(L"WQL");
        BSTR query = SysAllocString(performanceCounters
            ? L"SELECT * FROM Win32_PerfFormattedData_Counters_ThermalZoneInformation"
            : L"SELECT CurrentTemperature FROM MSAcpi_ThermalZoneTemperature");
        HRESULT hr = services_->ExecQuery(
            language, query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
            &enumerator);
        SysFreeString(language);
        SysFreeString(query);
        if (FAILED(hr) || !enumerator) {
            Close();
            MarkFailed();
            return std::nullopt;
        }

        std::optional<double> best;
        IWbemClassObject* object = nullptr;
        ULONG returned = 0;
        while (!g_workerStop && enumerator->Next(500, 1, &object, &returned) == S_OK &&
               returned == 1) {
            auto readKelvin = [&](const wchar_t* property, double divisor) {
                VARIANT raw{};
                object->Get(property, 0, &raw, nullptr, nullptr);
                double kelvin = 0;
                if (raw.vt == VT_I4) kelvin = raw.lVal / divisor;
                if (raw.vt == VT_UI4) kelvin = raw.ulVal / divisor;
                VariantClear(&raw);
                double c = kelvin - 273.15;
                return c > 0 && c < 150 ? c : -1.0;
            };
            double celsius = readKelvin(performanceCounters
                ? L"HighPrecisionTemperature" : L"CurrentTemperature", 10);
            if (performanceCounters && celsius < 0) {
                celsius = readKelvin(L"Temperature", 1);
            }
            object->Release();
            if (celsius > 0 && celsius < 150 &&
                (!best || celsius > *best)) {
                best = celsius;
            }
        }
        enumerator->Release();
        return best;
    }

    void ResetTimeout() {
        failed_ = false;
        nextRetry_ = {};
    }

   private:
    void MarkFailed() {
        failed_ = true;
        nextRetry_ =
            std::chrono::steady_clock::now() + std::chrono::seconds(60);
    }

    void Close() {
        if (services_) {
            services_->Release();
            services_ = nullptr;
        }
    }

    IWbemServices* services_ = nullptr;
    bool failed_ = false;
    std::chrono::steady_clock::time_point nextRetry_{};
};

struct TemperatureSources {
    WmiConnection ohm;
    WmiConnection acpi;
    WmiConnection perfZones;

    double Read() {
        if (ohm.Ensure(L"ROOT\\OpenHardwareMonitor")) {
            if (auto t = ohm.QueryCpuTemperature()) {
                return *t;
            }
        }
        if (acpi.Ensure(L"ROOT\\WMI")) {
            if (auto t = acpi.QueryZoneTemperature(false)) {
                return *t;
            }
        }
        if (perfZones.Ensure(L"ROOT\\CIMV2")) {
            if (auto t = perfZones.QueryZoneTemperature(true)) return *t;
        }
        return -1;
    }
};

// Worker thread
// ---------------------------------------------------------------------------

void WorkerMain() {
    struct ComApartment {
        HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        ~ComApartment() { if (SUCCEEDED(result)) CoUninitialize(); }
    } com;

    NetCounters network;
    NvmlGpu nvml;
    PdhGpuLoad pdhGpu;
    TemperatureSources temps;
    SensorReader sensor;
    double lastCpuTemp = -1;
    unsigned tick = 0;
    std::chrono::steady_clock::time_point nextWmi{};

    while (!g_workerStop.load()) {
        Snapshot snapshot;
        const auto settings = CurrentSettings();

        if (auto cpu = ReadCpuLoad()) {
            snapshot.cpuPct = *cpu;
        }
        if (settings.showRam) ReadRam(snapshot);

        if (settings.showNet) {
            const auto [down, up] = ReadNetwork(network, snapshot.netAvailable);
            snapshot.netDown = down;
            snapshot.netUp = up;
        } else if (!network.previous.empty()) {
            network = {};
        }

        if (settings.showGpu && nvml.Ensure()) {
            double util = -1, temp = -1;
            if (nvml.Sample(util, temp, settings.showTemps)) {
                if (util >= 0) {
                    snapshot.gpuPct = util;
                }
                if (temp > 0) {
                    snapshot.gpuTemp = temp;
                }
                snapshot.gpuAvailable = true;
            }
        }
        if (settings.showGpu && snapshot.gpuPct < 0) {
            if (auto util = pdhGpu.Sample()) {
                snapshot.gpuPct = *util;
                snapshot.gpuAvailable = true;
            }
        }

        if (settings.showCpu && settings.showTemps) {
            if (auto sharedTemp = sensor.Read()) {
                lastCpuTemp = *sharedTemp;
                nextWmi = {};
            } else if (SUCCEEDED(com.result) && std::chrono::steady_clock::now() >= nextWmi) {
                lastCpuTemp = temps.Read();
                nextWmi = std::chrono::steady_clock::now() + std::chrono::seconds(5);
            }
            snapshot.cpuTemp = lastCpuTemp;
        }

        if (tick % 10 == 0) {
            Wh_Log(
                L"metrics cpu=%.0f cpuTemp=%.1f gpu=%.0f gpuTemp=%.1f "
                L"ram=%.0f down=%.0f up=%.0f",
                snapshot.cpuPct, snapshot.cpuTemp, snapshot.gpuPct,
                snapshot.gpuTemp, snapshot.ramPct, snapshot.netDown,
                snapshot.netUp);
        }

        {
            std::lock_guard lock(g_snapMutex);
            g_snap = snapshot;
        }

        tick++;
        std::unique_lock lock(g_workerWakeMutex);
        g_workerWake.wait_for(lock, std::chrono::seconds(settings.intervalSeconds),
                              [] { return g_workerStop.load(); });
    }

}

void StartWorker() {
    g_workerStop = false;
    g_worker.emplace(WorkerMain);
}

void StopWorker() {
    if (!g_worker) {
        return;
    }
    {
        std::lock_guard lock(g_workerWakeMutex);
        g_workerStop = true;
    }
    g_workerWake.notify_all();
    g_worker->join();
    g_worker.reset();
}

// ---------------------------------------------------------------------------
// Formatting
// ---------------------------------------------------------------------------

std::wstring FormatPercent(double value) {
    if (value < 0) {
        return L"-";
    }
    wchar_t buffer[16];
    swprintf(buffer, 16, L"%.0f%%", value);
    return buffer;
}

std::wstring FormatTemp(double value) {
    if (value < 0) {
        return L"-";
    }
    wchar_t buffer[16];
    swprintf(buffer, 16, L"\u00B7\u2002%.0f\u00B0C", value);
    return buffer;
}

std::wstring FormatRate(double bytesPerSecond) {
    wchar_t buffer[32];
    if (bytesPerSecond < 1024) {
        swprintf(buffer, 32, L"%.0fB/s", bytesPerSecond);
    } else if (bytesPerSecond < 1024.0 * 1024) {
        swprintf(buffer, 32, L"%.0fKB/s", bytesPerSecond / 1024.0);
    } else if (bytesPerSecond < 1024.0 * 1024 * 1024) {
        swprintf(buffer, 32, L"%.1fMB/s", bytesPerSecond / (1024.0 * 1024));
    } else {
        swprintf(buffer, 32, L"%.1fGB/s",
                 bytesPerSecond / (1024.0 * 1024 * 1024));
    }
    return buffer;
}

// ---------------------------------------------------------------------------
// UI
// ---------------------------------------------------------------------------

enum class IconKind { Cpu, Gpu, Ram, ArrowDown, ArrowUp };

struct Palette {
    Color text;
    Color subtle;
    Color warn;
    Color crit;
};

Palette CurrentPalette(bool dark) {
    Palette p;
    if (dark) {
        p.text = Color{255, 238, 238, 238};
        p.subtle = Color{235, 200, 200, 200};
        p.warn = Color{255, 255, 196, 87};
        p.crit = Color{255, 255, 107, 107};
    } else {
        p.text = Color{255, 24, 24, 24};
        p.subtle = Color{235, 88, 88, 88};
        p.warn = Color{255, 150, 84, 0};
        p.crit = Color{255, 196, 43, 28};
    }
    return p;
}

[[clang::no_destroy]] wuxc::Grid g_rootGrid{nullptr};
[[clang::no_destroy]] wuxc::Grid g_host{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_row{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_cpuGroup{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_gpuGroup{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_ramGroup{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_netGroup{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_cpuValue{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_cpuTemp{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_gpuValue{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_gpuTemp{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_ramValue{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_uptime{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_netDown{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_netUp{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_textBrush{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_warnBrush{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_critBrush{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_timer{nullptr};
[[clang::no_destroy]] wux::DispatcherTimer g_retryTimer{nullptr};
[[clang::no_destroy]] wux::FrameworkElement g_pendingFrame{nullptr};
[[clang::no_destroy]] wux::FrameworkElement g_measuredFrame{nullptr};
std::atomic<bool> g_attached{false};
// Only the primary taskbar thread subscribes and revokes these handlers.
[[clang::no_destroy]] std::vector<std::pair<winrt::weak_ref<wux::FrameworkElement>,
                                           winrt::event_token>> g_loadedHandlers;
[[clang::no_destroy]] wux::XamlRoot g_xamlRoot{nullptr};

bool g_dark = true;
std::atomic<bool> g_unloading{false};
double g_rightMargin = 250;
int g_retryCount = 0;
std::atomic<HWND> g_msgWindow{nullptr};
HWND g_taskbarWindow = nullptr;

void EnsureMessageWindow();

HWND FindPrimaryTaskbarOnCurrentThread() {
    struct Search {
        HWND found;
        DWORD threadId;
    } search{nullptr, GetCurrentThreadId()};

    EnumWindows(
        [](HWND window, LPARAM param) -> BOOL {
            auto* s = reinterpret_cast<Search*>(param);
            wchar_t className[64];
            if (!GetClassNameW(window, className, 64)) {
                return TRUE;
            }
            if (wcscmp(className, L"Shell_TrayWnd") != 0) {
                return TRUE;
            }
            if (GetWindowThreadProcessId(window, nullptr) != s->threadId) {
                return TRUE;
            }
            s->found = window;
            return FALSE;
        },
        reinterpret_cast<LPARAM>(&search));
    return search.found;
}

bool IsDarkTheme() {
    if (g_host) {
        const auto theme = g_host.ActualTheme();
        if (theme == wux::ElementTheme::Dark) {
            return true;
        }
        if (theme == wux::ElementTheme::Light) {
            return false;
        }
    }
    auto application = wux::Application::Current();
    return !application ||
           application.RequestedTheme() == wux::ApplicationTheme::Dark;
}

wuxc::TextBlock MakeTextBlock(double size, Color color) {
    wuxc::TextBlock text;
    text.FontSize(size);
    text.FontFamily(
        wuxm::FontFamily(L"Segoe UI Variable Text"));
    text.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
    text.TextLineBounds(wux::TextLineBounds::Tight);
    text.Foreground(wuxm::SolidColorBrush(color));
    text.VerticalAlignment(wux::VerticalAlignment::Center);
    return text;
}

// Icons are redrawn from "game-icon-pack" by Nieobie (CC0-1.0) with XAML
// primitives and geometry built in code:
// https://github.com/Nieobie/game-icon-pack
constexpr double kIconBox = 16.0;

void AppendRoundedRect(wuxm::PathGeometry const& geometry, float left,
                       float top, float width, float height, float radius) {
    wuxm::PathFigure figure;
    figure.StartPoint(wf::Point{left + radius, top});
    auto segments = figure.Segments();

    auto lineTo = [&](float x, float y) {
        wuxm::LineSegment segment;
        segment.Point(wf::Point{x, y});
        segments.Append(segment);
    };
    auto arcTo = [&](float x, float y) {
        wuxm::ArcSegment segment;
        segment.Point(wf::Point{x, y});
        segment.Size(wf::Size{radius, radius});
        segment.SweepDirection(wuxm::SweepDirection::Clockwise);
        segment.IsLargeArc(false);
        segments.Append(segment);
    };

    lineTo(left + width - radius, top);
    arcTo(left + width, top + radius);
    lineTo(left + width, top + height - radius);
    arcTo(left + width - radius, top + height);
    lineTo(left + radius, top + height);
    arcTo(left, top + height - radius);
    lineTo(left, top + radius);
    arcTo(left + radius, top);
    figure.IsClosed(true);
    geometry.Figures().Append(figure);
}

wuxs::Path MakePath(wuxm::PathGeometry geometry,
                    wuxm::SolidColorBrush const& brush) {
    wuxs::Path path;
    path.Data(geometry);
    path.Fill(brush);
    geometry.FillRule(wuxm::FillRule::EvenOdd);
    path.Stretch(wuxm::Stretch::None);
    path.Width(kIconBox);
    path.Height(kIconBox);
    path.VerticalAlignment(wux::VerticalAlignment::Center);
    return path;
}

wuxs::Rectangle MakeFilledRect(const wuxm::SolidColorBrush& brush, double left,
                               double top, double width, double height,
                               double radius) {
    wuxs::Rectangle shape;
    shape.Width(width);
    shape.Height(height);
    shape.RadiusX(radius);
    shape.RadiusY(radius);
    shape.Fill(brush);
    wuxc::Canvas::SetLeft(shape, left);
    wuxc::Canvas::SetTop(shape, top);
    return shape;
}

wuxs::Polygon MakeFilledPolygon(
    const wuxm::SolidColorBrush& brush,
    std::initializer_list<wf::Point> points) {
    wuxs::Polygon shape;
    shape.Fill(brush);
    for (const auto& point : points) {
        shape.Points().Append(point);
    }
    return shape;
}

wux::FrameworkElement MakeCpuIcon(const wuxm::SolidColorBrush& brush) {
    wuxc::Canvas canvas;
    canvas.Width(kIconBox);
    canvas.Height(kIconBox);
    canvas.VerticalAlignment(wux::VerticalAlignment::Center);

    // Chip body: a rounded ring with a die in the middle.
    wuxm::PathGeometry ring;
    AppendRoundedRect(ring, 3.4, 3.4, 9.2, 9.2, 2.0);
    AppendRoundedRect(ring, 5.4, 5.4, 5.2, 5.2, 1.2);
    canvas.Children().Append(MakePath(ring, brush));
    canvas.Children().Append(
        MakeFilledRect(brush, 6.7, 6.7, 2.6, 2.6, 0.6));

    // Pins on four sides.
    for (double x : {5.6, 8.0, 10.4}) {
        canvas.Children().Append(
            MakeFilledRect(brush, x - 0.7, 1.4, 1.4, 2.1, 0.7));
        canvas.Children().Append(
            MakeFilledRect(brush, x - 0.7, 12.5, 1.4, 2.1, 0.7));
    }
    for (double y : {5.6, 8.0, 10.4}) {
        canvas.Children().Append(
            MakeFilledRect(brush, 1.4, y - 0.7, 2.1, 1.4, 0.7));
        canvas.Children().Append(
            MakeFilledRect(brush, 12.5, y - 0.7, 2.1, 1.4, 0.7));
    }
    return canvas;
}

wux::FrameworkElement MakeGpuIcon(const wuxm::SolidColorBrush& brush) {
    wuxc::Canvas canvas;
    canvas.Width(kIconBox);
    canvas.Height(kIconBox);
    canvas.VerticalAlignment(wux::VerticalAlignment::Center);

    // Monitor: screen ring, stand and base.
    wuxm::PathGeometry ring;
    AppendRoundedRect(ring, 1.6, 2.3, 12.8, 8.0, 1.6);
    AppendRoundedRect(ring, 2.8, 3.5, 10.4, 5.0, 0.8);
    canvas.Children().Append(MakePath(ring, brush));
    canvas.Children().Append(MakeFilledRect(brush, 7.2, 10.3, 1.6, 1.9, 0.3));
    canvas.Children().Append(MakeFilledRect(brush, 5.6, 12.2, 4.8, 1.4, 0.7));
    return canvas;
}

wux::FrameworkElement MakeRamIcon(const wuxm::SolidColorBrush& brush) {
    wuxc::Canvas canvas;
    canvas.Width(kIconBox);
    canvas.Height(kIconBox);
    canvas.VerticalAlignment(wux::VerticalAlignment::Center);

    // Memory card with a chamfered corner and three contact slots.
    wuxm::PathGeometry geometry;

    wuxm::PathFigure card;
    card.StartPoint(wf::Point{5.0, 2.3});
    auto segments = card.Segments();
    auto lineTo = [&](float x, float y) {
        wuxm::LineSegment segment;
        segment.Point(wf::Point{x, y});
        segments.Append(segment);
    };
    auto arcTo = [&](float x, float y, float radius) {
        wuxm::ArcSegment segment;
        segment.Point(wf::Point{x, y});
        segment.Size(wf::Size{radius, radius});
        segment.SweepDirection(wuxm::SweepDirection::Clockwise);
        segment.IsLargeArc(false);
        segments.Append(segment);
    };
    lineTo(12.8, 2.3);
    arcTo(14.6, 4.1, 1.8);
    lineTo(14.6, 10.5);
    arcTo(12.8, 12.3, 1.8);
    lineTo(5.2, 12.3);
    arcTo(3.4, 10.5, 1.8);
    lineTo(3.4, 3.9);
    lineTo(5.0, 2.3);
    card.IsClosed(true);
    geometry.Figures().Append(card);

    AppendRoundedRect(geometry, 5.3, 3.7, 1.5, 2.5, 0.3);
    AppendRoundedRect(geometry, 7.6, 3.7, 1.5, 2.5, 0.3);
    AppendRoundedRect(geometry, 9.9, 3.7, 1.5, 2.5, 0.3);
    canvas.Children().Append(MakePath(geometry, brush));

    // Contacts at the bottom edge.
    for (double x : {4.0, 6.1, 8.2, 10.3, 12.4}) {
        canvas.Children().Append(MakeFilledRect(brush, x, 12.4, 1.2, 1.2, 0.3));
    }
    return canvas;
}

wux::FrameworkElement MakeArrowIcon(const wuxm::SolidColorBrush& brush, bool up) {
    wuxc::Canvas canvas;
    canvas.Width(kIconBox);
    canvas.Height(kIconBox);
    canvas.VerticalAlignment(wux::VerticalAlignment::Center);

    if (up) {
        canvas.Children().Append(
            MakeFilledPolygon(brush, {{4.2, 7.3}, {8.0, 3.1}, {11.8, 7.3}}));
        canvas.Children().Append(MakeFilledRect(brush, 7.15, 6.1, 1.7, 7.4, 0.2));
    } else {
        canvas.Children().Append(
            MakeFilledPolygon(brush, {{4.2, 8.5}, {8.0, 12.7}, {11.8, 8.5}}));
        canvas.Children().Append(MakeFilledRect(brush, 7.15, 1.7, 1.7, 7.4, 0.2));
    }
    canvas.Children().Append(MakeFilledRect(brush, 3.8, 13.0, 8.4, 1.4, 0.7));
    return canvas;
}

wux::FrameworkElement MakeIcon(IconKind kind, Color color) {
    auto brush = wuxm::SolidColorBrush(color);
    switch (kind) {
        case IconKind::Cpu:
            return MakeCpuIcon(brush);
        case IconKind::Gpu:
            return MakeGpuIcon(brush);
        case IconKind::Ram:
            return MakeRamIcon(brush);
        case IconKind::ArrowDown:
            return MakeArrowIcon(brush, false);
        case IconKind::ArrowUp:
            return MakeArrowIcon(brush, true);
    }
    return MakeCpuIcon(brush);
}

void SetText(wuxc::TextBlock const& block, const std::wstring& value) {
    if (!block) {
        return;
    }
    auto text = winrt::hstring(value);
    if (block.Text() != text) {
        block.Text(text);
    }
}

void UpdateRightMargin() {
    if (!g_host) {
        return;
    }
    double scale = 1;
    if (g_xamlRoot) {
        scale = g_xamlRoot.RasterizationScale();
    }
    if (scale <= 0) {
        scale = 1;
    }

    if (!g_taskbarWindow || !IsWindow(g_taskbarWindow)) {
        g_taskbarWindow = FindWindowW(L"Shell_TrayWnd", nullptr);
    }

    const int gap = CurrentSettings().trayGap;
    double margin = 244 + gap;
    if (g_taskbarWindow) {
        if (HWND tray = FindWindowExW(g_taskbarWindow, nullptr,
                                      L"TrayNotifyWnd", nullptr)) {
            RECT taskbarRect{};
            RECT trayRect{};
            if (GetWindowRect(g_taskbarWindow, &taskbarRect) &&
                GetWindowRect(tray, &trayRect)) {
                // Align the widget's right edge with the tray's left edge in
                // the RootGrid's coordinate space, whatever its padding is.
                const double rootWidth =
                    g_rootGrid ? g_rootGrid.ActualWidth() : 0;
                const double trayLeft =
                    (trayRect.left - taskbarRect.left) / scale;
                margin = (rootWidth > 0 ? rootWidth - trayLeft
                                        : (trayRect.right - trayRect.left) / scale) + gap;
            }
        }
    }
    if (margin < 0) {
        margin = 0;
    }
    g_rightMargin = margin;
    const auto current = g_host.Margin();
    if (std::abs(current.Right - margin) >= 0.5) {
        g_host.Margin(wux::Thickness{0, 0, margin, 0});
    }
}

void ApplyWidthLimits(const Snapshot& snapshot, const Settings& settings) {
    if (!g_rootGrid || !g_host || !g_row) {
        return;
    }
    const double rootWidth = g_rootGrid.ActualWidth();
    if (rootWidth <= 0) {
        return;
    }
    const double available = std::max(80.0, rootWidth - g_rightMargin - 14);
    if (g_uptime) g_uptime.Visibility(wux::Visibility::Visible);

    if (g_netGroup) {
        g_netGroup.Visibility(settings.showNet && snapshot.netAvailable ? wux::Visibility::Visible
                                                 : wux::Visibility::Collapsed);
    }
    if (g_cpuTemp) {
        g_cpuTemp.Visibility(settings.showTemps && snapshot.cpuTemp >= 0 ? wux::Visibility::Visible
                                                  : wux::Visibility::Collapsed);
    }
    if (g_gpuTemp) {
        g_gpuTemp.Visibility(settings.showTemps && snapshot.gpuTemp >= 0 ? wux::Visibility::Visible
                                                  : wux::Visibility::Collapsed);
    }
    if (g_gpuGroup) {
        g_gpuGroup.Visibility(snapshot.gpuAvailable ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }

    auto desiredWidth = [&]() {
        g_row.Measure(wf::Size{std::numeric_limits<float>::infinity(), 64});
        return g_row.DesiredSize().Width;
    };

    if (desiredWidth() <= available) {
        return;
    }
    if (g_uptime) {
        g_uptime.Visibility(wux::Visibility::Collapsed);
        if (desiredWidth() <= available) return;
    }
    if (g_netGroup) {
        g_netGroup.Visibility(wux::Visibility::Collapsed);
    }
    if (desiredWidth() <= available) {
        return;
    }
    if (g_cpuTemp) {
        g_cpuTemp.Visibility(wux::Visibility::Collapsed);
    }
    if (g_gpuTemp) {
        g_gpuTemp.Visibility(wux::Visibility::Collapsed);
    }
    if (desiredWidth() <= available) {
        return;
    }
    if (g_gpuGroup) {
        g_gpuGroup.Visibility(wux::Visibility::Collapsed);
    }
}

void UpdateTexts() {
    if (!g_host) {
        return;
    }

    Snapshot snapshot;
    {
        std::lock_guard lock(g_snapMutex);
        snapshot = g_snap;
    }

    SetText(g_cpuValue, FormatPercent(snapshot.cpuPct));
    const auto settings = CurrentSettings();
    if (settings.ramDisplay == RamDisplay::Percent) {
        SetText(g_ramValue, FormatPercent(snapshot.ramPct));
    } else {
        const double gib = settings.ramDisplay == RamDisplay::Used ? snapshot.usedGiB : snapshot.freeGiB;
        wchar_t value[32] = L"-";
        if (gib >= 0) swprintf(value, 32, L"%.1f GiB", gib);
        SetText(g_ramValue, value);
    }
    if (g_uptime) {
        const auto minutes = GetTickCount64() / 60000;
        wchar_t value[40];
        swprintf(value, 40, settings.showIcons ? L"\u2191 %llu:%02llu" : L"%llu:%02llu",
                 minutes / 60, minutes % 60);
        SetText(g_uptime, value);
    }

    if (g_cpuTemp) {
        SetText(g_cpuTemp, settings.showTemps ? FormatTemp(snapshot.cpuTemp)
                                                : L"");
        if (settings.showTemps && snapshot.cpuTemp >= 0) {
            g_cpuTemp.Foreground(
                snapshot.cpuTemp >= settings.critTemp
                    ? g_critBrush
                    : (snapshot.cpuTemp >= settings.warnTemp ? g_warnBrush
                                                              : g_textBrush));
        } else {
            g_cpuTemp.Foreground(g_textBrush);
        }
    }

    if (snapshot.gpuAvailable) {
        SetText(g_gpuValue, FormatPercent(snapshot.gpuPct));
        if (g_gpuTemp) {
            SetText(g_gpuTemp,
                    settings.showTemps ? FormatTemp(snapshot.gpuTemp) : L"");
            if (settings.showTemps && snapshot.gpuTemp >= 0) {
                g_gpuTemp.Foreground(
                    snapshot.gpuTemp >= settings.critTemp
                        ? g_critBrush
                        : (snapshot.gpuTemp >= settings.warnTemp
                               ? g_warnBrush
                               : g_textBrush));
            } else {
                g_gpuTemp.Foreground(g_textBrush);
            }
        }
    }

    if (g_netGroup && settings.showNet && snapshot.netAvailable) {
        SetText(g_netDown, FormatRate(snapshot.netDown));
        SetText(g_netUp, FormatRate(snapshot.netUp));
    }

    ApplyWidthLimits(snapshot, settings);
}

void BuildContent();
void Tick() {
    if (g_unloading || !g_host) {
        return;
    }
    const bool dark = IsDarkTheme();
    if (dark != g_dark) {
        BuildContent();
    }
    if (!g_row) {
        return;
    }
    UpdateRightMargin();
    UpdateTexts();
}

void StartTimer() {
    if (g_timer) {
        return;
    }
    g_timer = wux::DispatcherTimer{};
    g_timer.Interval(std::chrono::seconds(CurrentSettings().intervalSeconds));
    g_timer.Tick([](auto&&, auto&&) { Tick(); });
    g_timer.Start();
}

void BuildContent() {
    if (!g_host) {
        return;
    }
    if (g_timer) {
        g_timer.Stop();
        g_timer = nullptr;
    }
    if (g_retryTimer) {
        g_retryTimer.Stop();
        g_retryTimer = nullptr;
    }
    g_host.Children().Clear();
    g_row = nullptr;
    g_cpuGroup = nullptr;
    g_gpuGroup = nullptr;
    g_ramGroup = nullptr;
    g_netGroup = nullptr;
    g_cpuValue = nullptr;
    g_cpuTemp = nullptr;
    g_gpuValue = nullptr;
    g_gpuTemp = nullptr;
    g_ramValue = nullptr;
    g_uptime = nullptr;
    g_netDown = nullptr;
    g_netUp = nullptr;

    g_dark = IsDarkTheme();
    const auto palette = CurrentPalette(g_dark);
    const auto settings = CurrentSettings();
    const double size = static_cast<double>(settings.fontSize);

    g_textBrush = wuxm::SolidColorBrush(palette.text);
    g_warnBrush = wuxm::SolidColorBrush(palette.warn);
    g_critBrush = wuxm::SolidColorBrush(palette.crit);

    g_row = wuxc::StackPanel{};
    g_row.Orientation(wuxc::Orientation::Horizontal);
    g_row.Spacing(settings.metricGap);
    g_row.VerticalAlignment(wux::VerticalAlignment::Center);
    auto makeIcon = [&](IconKind icon) {
        wuxc::Viewbox box;
        box.Child(MakeIcon(icon, palette.subtle));
        box.Width(size * kIconBox / 12);
        box.Height(box.Width());
        box.VerticalAlignment(wux::VerticalAlignment::Center);
        box.Margin(wux::Thickness{0, 0, 4, 0});
        return box;
    };
    auto makeGroup = [&](IconKind icon, const wchar_t* name) {
        wuxc::StackPanel group;
        group.Orientation(wuxc::Orientation::Horizontal);
        group.VerticalAlignment(wux::VerticalAlignment::Center);
        if (settings.showIcons) {
            group.Children().Append(makeIcon(icon));
        }
        wux::Automation::AutomationProperties::SetName(group, name);
        return group;
    };

    // CPU
    g_cpuGroup = makeGroup(IconKind::Cpu, L"CPU");
    g_cpuValue = MakeTextBlock(size, palette.text);
    g_cpuGroup.Children().Append(g_cpuValue);
    if (settings.showTemps) {
        g_cpuTemp = MakeTextBlock(size, palette.text);
        g_cpuTemp.Margin(wux::Thickness{size / 2, 0, 0, 0});
        g_cpuGroup.Children().Append(g_cpuTemp);
    }
    if (settings.showCpu) g_row.Children().Append(g_cpuGroup);

    // GPU
    g_gpuGroup = makeGroup(IconKind::Gpu, L"GPU");
    g_gpuValue = MakeTextBlock(size, palette.text);
    g_gpuGroup.Children().Append(g_gpuValue);
    if (settings.showTemps) {
        g_gpuTemp = MakeTextBlock(size, palette.text);
        g_gpuTemp.Margin(wux::Thickness{size / 2, 0, 0, 0});
        g_gpuGroup.Children().Append(g_gpuTemp);
    }
    if (settings.showGpu) g_row.Children().Append(g_gpuGroup);

    // RAM
    g_ramGroup = makeGroup(IconKind::Ram, L"RAM");
    g_ramValue = MakeTextBlock(size, palette.text);
    g_ramGroup.Children().Append(g_ramValue);
    if (settings.showRam) g_row.Children().Append(g_ramGroup);
    if (settings.showUptime) {
        g_uptime = MakeTextBlock(size, palette.text);
        g_row.Children().Append(g_uptime);
    }

    // Network
    if (settings.showNet) {
        g_netGroup = wuxc::StackPanel{};
        g_netGroup.Orientation(wuxc::Orientation::Horizontal);
        g_netGroup.VerticalAlignment(wux::VerticalAlignment::Center);

        auto downPanel = wuxc::StackPanel{};
        downPanel.Orientation(wuxc::Orientation::Horizontal);
        downPanel.VerticalAlignment(wux::VerticalAlignment::Center);
        downPanel.Margin(wux::Thickness{0, 0, 9, 0});
        if (settings.showIcons) downPanel.Children().Append(makeIcon(IconKind::ArrowDown));
        g_netDown = MakeTextBlock(size, palette.text);
        downPanel.Children().Append(g_netDown);

        auto upPanel = wuxc::StackPanel{};
        upPanel.Orientation(wuxc::Orientation::Horizontal);
        upPanel.VerticalAlignment(wux::VerticalAlignment::Center);
        if (settings.showIcons) upPanel.Children().Append(makeIcon(IconKind::ArrowUp));
        g_netUp = MakeTextBlock(size, palette.text);
        upPanel.Children().Append(g_netUp);

        g_netGroup.Children().Append(downPanel);
        g_netGroup.Children().Append(upPanel);
        g_row.Children().Append(g_netGroup);
    }

    g_host.Children().Append(g_row);
    SetText(g_cpuValue, L"-");
    SetText(g_gpuValue, L"-");
    SetText(g_ramValue, L"-");
    SetText(g_cpuTemp, L"-");
    SetText(g_gpuTemp, L"-");
    StartTimer();
}

void CreateUi() {
    if (!g_rootGrid || g_unloading) {
        return;
    }

    if (!g_host) {
        g_host = wuxc::Grid{};
        g_host.Name(kHostName);
        g_host.IsHitTestVisible(false);
        g_host.HorizontalAlignment(wux::HorizontalAlignment::Right);
        g_host.VerticalAlignment(wux::VerticalAlignment::Center);
        wuxc::Canvas::SetZIndex(g_host, 10000);
        wuxc::Grid::SetColumnSpan(
            g_host, static_cast<int>(std::max(
                        1u, g_rootGrid.ColumnDefinitions().Size())));
    }

    uint32_t index = 0;
    if (!g_rootGrid.Children().IndexOf(g_host, index)) {
        g_rootGrid.Children().Append(g_host);
    }

    BuildContent();
    UpdateRightMargin();
    UpdateTexts();
}

wux::FrameworkElement FindDirectChildByName(wux::DependencyObject const& parent,
                                            const wchar_t* name) {
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        auto element = child.try_as<wux::FrameworkElement>();
        if (element && element.Name() == name) {
            return element;
        }
    }
    return nullptr;
}

wux::FrameworkElement GetTaskbarHostElement(HWND tray);

bool IsPrimaryFrame(HWND tray, wux::FrameworkElement const& frame) noexcept {
    try {
        auto host = GetTaskbarHostElement(tray);
        auto primaryRoot = host ? host.XamlRoot() : nullptr;
        return primaryRoot && frame.XamlRoot() == primaryRoot;
    } catch (const winrt::hresult_error&) {
        return false;  // The shell can be replacing its XAML tree.
    }
}

void DetachHost() {
    uint32_t index;
    if (g_rootGrid && g_host && g_rootGrid.Children().IndexOf(g_host, index))
        g_rootGrid.Children().RemoveAt(index);
}

void AttachToTaskbarFrame(wux::FrameworkElement frame) {
    if (!frame || g_unloading) {
        return;
    }
    HWND primary = FindPrimaryTaskbarOnCurrentThread();
    if (!primary || !IsPrimaryFrame(primary, frame)) {
        return;
    }
    g_taskbarWindow = primary;
    EnsureMessageWindow();

    auto rootElement = FindDirectChildByName(frame, L"RootGrid");
    auto root = rootElement ? rootElement.try_as<wuxc::Grid>() : nullptr;
    if (!root) {
        g_pendingFrame = frame;
        if (!g_retryTimer) {
            g_retryCount = 0;
            g_retryTimer = wux::DispatcherTimer{};
            g_retryTimer.Interval(std::chrono::milliseconds(400));
            g_retryTimer.Tick([](auto&&, auto&&) {
                if (g_unloading) {
                    if (g_retryTimer) {
                        g_retryTimer.Stop();
                        g_retryTimer = nullptr;
                    }
                    return;
                }
                if (++g_retryCount > 25 || !g_pendingFrame) {
                    Wh_Log(L"SysBar: RootGrid not found, giving up");
                    if (g_retryTimer) {
                        g_retryTimer.Stop();
                        g_retryTimer = nullptr;
                    }
                    g_pendingFrame = nullptr;
                    return;
                }
                AttachToTaskbarFrame(g_pendingFrame);
            });
            g_retryTimer.Start();
        }
        return;
    }

    g_pendingFrame = nullptr;
    if (g_retryTimer) {
        g_retryTimer.Stop();
        g_retryTimer = nullptr;
    }

    if (g_rootGrid && winrt::get_abi(g_rootGrid) != winrt::get_abi(root)) {
        // A new taskbar tree was created (explorer restart or theme reload).
        DetachHost();
        g_host = nullptr;
        g_row = nullptr;
        g_cpuGroup = nullptr;
        g_gpuGroup = nullptr;
        g_ramGroup = nullptr;
        g_netGroup = nullptr;
        g_cpuValue = nullptr;
        g_cpuTemp = nullptr;
        g_gpuValue = nullptr;
        g_gpuTemp = nullptr;
        g_ramValue = nullptr;
        g_uptime = nullptr;
        g_netDown = nullptr;
        g_netUp = nullptr;
        if (g_timer) {
            g_timer.Stop();
            g_timer = nullptr;
        }
    }

    g_rootGrid = root;
    g_xamlRoot = frame.XamlRoot();
    CreateUi();
    g_attached = true;
    Wh_Log(L"widget attached: root=%.0fx%.0f margin=%.0f scale=%.2f",
             g_rootGrid.ActualWidth(), g_rootGrid.ActualHeight(), g_rightMargin,
             g_xamlRoot ? g_xamlRoot.RasterizationScale() : -1.0);
    Wh_Log(L"SysBar: widget attached");
}

// ---------------------------------------------------------------------------
// Taskbar-frame hook
// ---------------------------------------------------------------------------

using TaskbarFrame_Constructor_t = void*(WINAPI*)(void*);
TaskbarFrame_Constructor_t TaskbarFrame_Constructor_Original = nullptr;

void* WINAPI TaskbarFrame_Constructor_Hook(void* pThis) {
    void* result = TaskbarFrame_Constructor_Original(pThis);
    if (g_unloading) {
        return result;
    }

    try {
        wux::FrameworkElement frame{nullptr};
        reinterpret_cast<::IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        if (frame && FindPrimaryTaskbarOnCurrentThread()) {
            EnsureMessageWindow();
            if (!g_msgWindow || g_unloading) return result;
            std::erase_if(g_loadedHandlers, [](const auto& entry) { return !entry.first.get(); });
            auto token = frame.Loaded([](wf::IInspectable const& sender, auto&&) {
                if (g_unloading) {
                    return;
                }
                auto element = sender.try_as<wux::FrameworkElement>();
                if (element) {
                    AttachToTaskbarFrame(element);
                }
            });
            g_loadedHandlers.emplace_back(winrt::make_weak(frame), token);
        }
    } catch (...) {
        Wh_Log(L"SysBar: Loaded hook failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    return result;
}

// The constructor and Loaded event can both precede Windhawk at sign-in.
// Recover on the next layout pass, but modify the tree outside MeasureOverride.
using MeasureFrame_t = int(WINAPI*)(void*, wf::Size, wf::Size*);
MeasureFrame_t MeasureFrame_Original = nullptr;
int WINAPI MeasureFrame_Hook(void* self, wf::Size size, wf::Size* resultSize) {
    int result = MeasureFrame_Original(self, size, resultSize);
    if (SUCCEEDED(result) && !g_unloading && !g_attached) {
        HWND primary = FindPrimaryTaskbarOnCurrentThread();
        if (!primary || g_measuredFrame || g_retryTimer) return result;
        wux::FrameworkElement frame{nullptr};
        static_cast<IUnknown*>(self)->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        if (frame && IsPrimaryFrame(primary, frame)) {
            EnsureMessageWindow();
            if (HWND window = g_msgWindow.load()) {
                g_measuredFrame = frame;
                if (!PostMessageW(window, kRunOnTaskbarThreadMessage, 2, 0)) {
                    g_measuredFrame = nullptr;
                }
            }
        }
    }
    return result;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
        &TaskbarFrame_Constructor_Original,
        TaskbarFrame_Constructor_Hook,
    }, {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
        &MeasureFrame_Original,
        MeasureFrame_Hook,
        true,  // Keep constructor support on builds without this symbol.
    }};
    return WindhawkUtils::HookSymbols(module, hooks, std::size(hooks));
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

std::atomic<bool> g_taskbarViewHooked{false};

bool HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR fileName,
                                    bool applyImmediately) {
    if (!module || GetTaskbarViewModule() != module) {
        return false;
    }
    if (g_taskbarViewHooked.exchange(true)) {
        return true;
    }
    Wh_Log(L"SysBar: taskbar view loaded (%s)",
           fileName ? fileName : L"already loaded");
    if (!HookTaskbarViewSymbols(module)) {
        Wh_Log(L"SysBar: taskbar view symbols unavailable");
        g_taskbarViewHooked = false;
        return false;
    }
    if (applyImmediately) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    const DWORD lastError = GetLastError();
    if (module && !g_taskbarViewHooked) {
        HandleLoadedModuleIfTaskbarView(module, fileName, true);
    }
    SetLastError(lastError);
    return module;
}

// ---------------------------------------------------------------------------
// Cross-thread runner for the taskbar UI thread
// ---------------------------------------------------------------------------

// When Explorer already owns a fully laid out taskbar, no constructor or
// measure callback is guaranteed. Resolve the existing TaskbarHost as in
// Windhawk's taskbar-notification-icon-spacing and taskbar-multirow mods.
void* g_siteVtable = nullptr;
void* (WINAPI* g_getHost)(void*, void**) = nullptr;
BYTE* g_frameHeight = nullptr;
void (WINAPI* g_releaseHost)(void*) = nullptr;
UINT g_attachMessage = 0;

wux::FrameworkElement GetTaskbarHostElement(HWND tray) {
    HWND band = static_cast<HWND>(GetPropW(tray, L"TaskbandHWND"));
    auto site = band ? reinterpret_cast<void**>(GetWindowLongPtrW(band, 0)) : nullptr;
    if (!site) return nullptr;
    int i = 0;
    while (i < 20 && site[i] != g_siteVtable) ++i;
    if (i == 20) return nullptr;
#if defined(__x86_64__)
    const BYTE* b = g_frameHeight;
    if (b[0] != 0x48 || b[1] != 0x83 || b[2] != 0xec ||
        b[4] != 0x48 || b[5] != 0x83 || b[6] != 0xc1 || b[7] > 0x7f) return nullptr;
    size_t offset = b[7];
#elif defined(__aarch64__)
    // pacibsp; stp fp,lr,[sp,#imm]!; mov fp,sp; ldr x8,[x0,#imm]!
    const DWORD* p = reinterpret_cast<const DWORD*>(g_frameHeight);
    if (p[0] != 0xD503237F || (p[1] & 0xFFC07FFF) != 0xA9807BFD ||
        p[2] != 0x910003FD || (p[3] & 0xFFE00FFF) != 0xF8400C08 ||
        (p[3] & 0x00100000)) return nullptr;
    size_t offset = (p[3] >> 12) & 0xff;
#else
#error Unsupported architecture
#endif
    void* host[2]{};
    g_getHost(site + i, host);
    wux::FrameworkElement element{nullptr};
    if (host[0] && host[1]) {
        auto unk = *reinterpret_cast<IUnknown**>(static_cast<BYTE*>(host[0]) + offset);
        if (unk) unk->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                     winrt::put_abi(element));
    }
    if (host[1]) g_releaseHost(host[1]);
    return element;
}

void AttachExistingFrame(HWND tray) {
    if (g_attached || g_unloading) return;
    auto element = GetTaskbarHostElement(tray);
    if (element) {
        auto root = element.XamlRoot();
        auto frame = root && root.Content() ?
            root.Content().try_as<wux::FrameworkElement>() : nullptr;
        if (frame && winrt::get_class_name(frame) == L"Taskbar.TaskbarFrame")
            AttachToTaskbarFrame(frame);
        else if (winrt::get_class_name(element) == L"Taskbar.TaskbarFrame")
            AttachToTaskbarFrame(element);
    }
}

HMODULE g_taskbarDll = nullptr;
struct ModuleTraits {
    using type = HMODULE;
    static type invalid() noexcept { return nullptr; }
    static void close(type module) noexcept { FreeLibrary(module); }
};
bool ResolveTaskbarSymbols(HMODULE module) {
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_siteVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_releaseHost},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, std::size(taskbarDllHooks));
}

void RecoverExistingTaskbar() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(tray, &pid);
    if (!tid || pid != GetCurrentProcessId() || g_attached) return;
    g_attachMessage = RegisterWindowMessageW(L"SysBar.AttachExistingTaskbar");
    if (!g_attachMessage) return;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC,
        [](int code, WPARAM wp, LPARAM lp) -> LRESULT {
            if (code == HC_ACTION) {
                auto msg = reinterpret_cast<const CWPSTRUCT*>(lp);
                if (msg->message == g_attachMessage) {
                    try { AttachExistingFrame(msg->hwnd); }
                    catch (...) { Wh_Log(L"SysBar: existing taskbar unavailable"); }
                }
            }
            return CallNextHookEx(nullptr, code, wp, lp);
        }, nullptr, tid);
    if (hook) {
        SendMessageW(tray, g_attachMessage, 0, 0);
        UnhookWindowsHookEx(hook);
    }
}

void DestroyUi();

LRESULT CALLBACK MessageWindowProc(HWND window, UINT message, WPARAM wparam,
                                   LPARAM lparam) {
    if (message == kRunOnTaskbarThreadMessage) {
        if (g_unloading) {
            if (wparam == 1) DestroyUi();
        } else if (wparam == 2 && g_measuredFrame) {
            auto frame = std::move(g_measuredFrame);
            try {
                if (!g_attached && frame.XamlRoot()) AttachToTaskbarFrame(frame);
            } catch (...) {
                Wh_Log(L"SysBar: layout attach failed: %08X",
                       static_cast<unsigned>(winrt::to_hresult()));
            }
        } else if (wparam == 0 && g_host) {
            BuildContent();
            UpdateRightMargin();
            UpdateTexts();
        }
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

void EnsureMessageWindow() {
    if (g_unloading || g_msgWindow) {
        return;
    }
    static bool registered = false;
    if (!registered) {
        WNDCLASSW windowClass{};
        windowClass.lpfnWndProc = MessageWindowProc;
        windowClass.hInstance = GetModuleHandleW(nullptr);
        windowClass.lpszClassName = L"SysBarMonitorMessageWindow";
        registered = RegisterClassW(&windowClass) != 0;
    }
    if (!registered) {
        return;
    }
    g_msgWindow = CreateWindowExW(0, L"SysBarMonitorMessageWindow", L"", 0, 0,
                                  0, 0, 0, HWND_MESSAGE, nullptr,
                                  GetModuleHandleW(nullptr), nullptr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"SysBar: init");
    g_unloading = false;
    g_taskbarViewHooked = false;
    LoadSettings();
    winrt::handle_type<ModuleTraits> library{
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)};
    if (!ResolveTaskbarSymbols(library.get())) return FALSE;

    if (HMODULE module = GetTaskbarViewModule()) {
        if (!HandleLoadedModuleIfTaskbarView(module, nullptr, false)) {
            Wh_Log(L"SysBar: taskbar view already loaded, symbols missing");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            kernelBase = GetModuleHandleW(L"kernel32.dll");
        }
        auto loadLibraryEx =
            kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
                             GetProcAddress(kernelBase, "LoadLibraryExW"))
                       : nullptr;
        if (!loadLibraryEx ||
            !WindhawkUtils::SetFunctionHook(loadLibraryEx, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            Wh_Log(L"SysBar: LoadLibraryExW hook failed");
            return FALSE;
        }
    }
    g_taskbarDll = library.detach();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"after init");
    StartWorker();
    if (!g_taskbarViewHooked) {
        if (HMODULE module = GetTaskbarViewModule()) {
            HandleLoadedModuleIfTaskbarView(module, nullptr, true);
        }
    }
    RecoverExistingTaskbar();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND window = g_msgWindow.load()) {
        PostMessageW(window, kRunOnTaskbarThreadMessage, 0, 0);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"SysBar: before uninit");
    g_unloading = true;
    StopWorker();

    if (HWND window = g_msgWindow.load()) {
        SendMessageW(window, kRunOnTaskbarThreadMessage, 1, 0);
    }
}

namespace {
void DestroyUi() {
        for (auto& [weakFrame, token] : g_loadedHandlers) {
            if (auto frame = weakFrame.get()) frame.Loaded(token);
        }
        g_loadedHandlers.clear();
        if (g_timer) {
            g_timer.Stop();
            g_timer = nullptr;
        }
        if (g_retryTimer) {
            g_retryTimer.Stop();
            g_retryTimer = nullptr;
        }
        DetachHost();
        g_host = nullptr;
        g_row = nullptr;
        g_cpuGroup = nullptr;
        g_gpuGroup = nullptr;
        g_ramGroup = nullptr;
        g_netGroup = nullptr;
        g_cpuValue = nullptr;
        g_cpuTemp = nullptr;
        g_gpuValue = nullptr;
        g_gpuTemp = nullptr;
        g_ramValue = nullptr;
        g_uptime = nullptr;
        g_netDown = nullptr;
        g_netUp = nullptr;
        g_textBrush = nullptr;
        g_warnBrush = nullptr;
        g_critBrush = nullptr;
        g_rootGrid = nullptr;
        g_attached = false;
        g_measuredFrame = nullptr;
        g_pendingFrame = nullptr;
        g_xamlRoot = nullptr;
        if (g_msgWindow) {
            DestroyWindow(g_msgWindow);
            g_msgWindow = nullptr;
            UnregisterClassW(L"SysBarMonitorMessageWindow",
                             GetModuleHandleW(nullptr));
        }
}
}  // namespace

void Wh_ModUninit() {
    Wh_Log(L"SysBar: uninit");
    StopWorker();
    if (g_taskbarDll) FreeLibrary(g_taskbarDll);
}



