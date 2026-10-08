// ==WindhawkMod==
// @id              win7-on-screen-keyboard-restorer
// @name            Windows 7 On Screen Keyboard Restorer
// @name:it Ripristino della tastiera su schermo di Windows 7
// @name:ru Восстановление экранной клавиатуры Windows 7
// @name:es Restaurador del teclado en pantalla de Windows 7
// @name:pt-BR Restaurador do Teclado Virtual do Windows 7
// @name:fr Restaurateur du clavier visuel de Windows 7
// @name:de Windows 7 Bildschirmtastatur-Wiederherstellung
// @name:pl Przywracanie klawiatury ekranowej z Windows 7
// @name:zh-CN Windows 7 屏幕键盘还原
// @name:ja Windows 7 スクリーン キーボード復元
// @name:ko Windows 7 화상 키보드 복원
// @description     This mod redirects the Windows 10/11 On-Screen Keyboard (osk.exe) to the original Windows 7 one 
// @description:it Questo mod reindirizza la tastiera su schermo (osk.exe) di Windows 10/11 a quella originale di Windows 7
// @description:ru Этот мод перенаправляет экранную клавиатуру Windows 10/11 (osk.exe) на оригинальную клавиатуру Windows 7
// @description:es Este mod redirige el teclado en pantalla (osk.exe) de Windows 10/11 al original de Windows 7
// @description:pt-BR Este mod redireciona o Teclado Virtual do Windows 10/11 (osk.exe) para o original do Windows 7
// @description:fr Ce mod redirige le clavier visuel (osk.exe) de Windows 10/11 vers celui d'origine de Windows 7
// @description:de Dieses Mod leitet die Bildschirmtastatur (osk.exe) von Windows 10/11 auf die ursprüngliche von Windows 7 um
// @description:pl Ten mod przekierowuje klawiaturę ekranową (osk.exe) z Windows 10/11 do oryginalnej klawiatury z Windows 7
// @description:zh-CN 此模组将 Windows 10/11 的屏幕键盘 (osk.exe) 重定向为 Windows 7 的原始版本
// @description:ja この mod は、Windows 10/11 のスクリーン キーボード (osk.exe) を Windows 7 のオリジナル版にリダイレクトします
// @description:ko 이 모드는 Windows 10/11의 화상 키보드(osk.exe)를 원래의 Windows 7 버전으로 리디렉션합니다
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         osk.exe
// @include         control.exe
// @architecture    x86-64
// @compilerOptions -lwinhttp -lshlwapi -luser32 -lgdi32 -lshell32 -ladvapi32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 On Screen Keyboard Restorer

This modification tries to restore the original Windows 7 On-Screen Keyboard on Windows 10 and
Windows 11: whenever the system `osk.exe` is about to start, the Windows 7
`osk.exe` is started instead. This is a best effort restoration. The Windows 7
files are downloaded from Microsoft's public symbol server and verified against
pinned SHA-256 digests before they are used, and nothing is written to
`System32`, to `Common Files`, or to Image File Execution Options. The files are
kept in the mod's own storage folder (or in the folder of the `installFolder`
setting), which Windhawk removes together with the mod, and no registry key is
left behind.

This modification has been tested on Windows 10 21H2 and Windows 11 24H2.

## Screenshot (Before)

![Before](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/beforekeyboard.PNG)

## Screenshot (After)

![After](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/afterkeyboard.PNG)

## How it works

The mod catches the keyboard before it opens and swaps it for
the Windows 7 one. It does this in two ways: first, it watches the programs
that usually launch the keyboard (Explorer, Start/Search, `control.exe` and a
few others) and redirects the launch so the Windows 7 keyboard starts instead.
Second, if the system keyboard still manages to open through some path that
cannot be watched, the mod lets it open, then quietly closes it and starts the
Windows 7 one in its place.

Once the Windows 7 keyboard is running, the mod gives it the files it expects
but cannot find on Windows 10/11: `TabSKB.dll` and `DUI70.dll` are pointed to
local copies, the English texts and dialogs are supplied from inside the mod
when the `.mui` file is missing, and the seven special key faces (Windows,
menu, Enter and the four arrows) are drawn by the mod itself, because the
modern `TipResX.dll` on this system has no pictures for them.

## Preparation

Before the Windows 7 keyboard can run outside `System32`, two small edits are
needed. First, `uiAccess="true"` is changed to `false` in its manifest: Windows
only honors that flag for signed programs in protected folders, and otherwise
refuses to start the program with error 740. Second, `DUI70.dll` is renamed to
`DUI71.dll` so it does not collide with the system's own DirectUI, which has
the same name but is incompatible. The trade-off is that, since the UIAccess
flag is gone, the Windows 7 keyboard cannot type into elevated (administrator)
windows.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- installFolder: ""
  $name: Install folder
  $name:it: Cartella di installazione
  $name:ru: Папка установки
  $name:es: Carpeta de instalación
  $name:pt-BR: Pasta de instalação
  $name:fr: Dossier d'installation
  $name:de: Installationsordner
  $name:pl: Folder instalacyjny
  $name:zh-CN: 安装文件夹
  $name:ja: インストール フォルダー
  $name:ko: 설치 폴더
  $description: >-
    This setting sets the folder that holds the Windows 7 files; environment
    variables are expanded. Leave it empty to use the mod's own storage folder,
    which Windhawk removes together with the mod. The mod downloads the pinned
    Windows 7 files into this folder; a file there that is not the pinned build
    is first renamed to <name>.user-backup, never overwritten.
  $description:it: >-
    Questa impostazione indica la cartella che contiene i file di Windows 7; le
    variabili d'ambiente vengono espanse. Lasciala vuota per usare la cartella di
    archiviazione del mod, che Windhawk rimuove insieme al mod. Il mod scarica i
    file di Windows 7 previsti in questa cartella; un file già presente che non
    è la build prevista viene prima rinominato in <nome>.user-backup, mai
    sovrascritto.
  $description:ru: >-
    Этот параметр задаёт папку с файлами Windows 7; переменные среды раскрываются.
    Оставьте его пустым, чтобы использовать собственную папку хранения мода,
    которую Windhawk удаляет вместе с модом. Мод загружает в эту папку нужные
    файлы Windows 7; существующий файл, не совпадающий с нужной сборкой, сначала
    переименовывается в <имя>.user-backup и никогда не перезаписывается.
  $description:es: >-
    Esta opción indica la carpeta que contiene los archivos de Windows 7; las
    variables de entorno se expanden. Déjala vacía para usar la carpeta de
    almacenamiento del mod, que Windhawk elimina junto con el mod. El mod
    descarga los archivos de Windows 7 en esta carpeta; un archivo existente que
    no sea la compilación esperada se renombra antes a <nombre>.user-backup y
    nunca se sobrescribe.
  $description:pt-BR: >-
    Esta configuração indica a pasta que contém os arquivos do Windows 7; as
    variáveis de ambiente são expandidas. Deixe-a vazia para usar a pasta de
    armazenamento do mod, que o Windhawk remove junto com o mod. O mod baixa os
    arquivos do Windows 7 para esta pasta; um arquivo existente que não seja a
    compilação esperada é antes renomeado para <nome>.user-backup e nunca é
    sobrescrito.
  $description:fr: >-
    Ce paramètre indique le dossier qui contient les fichiers de Windows 7 ; les
    variables d'environnement sont développées. Laissez-le vide pour utiliser le
    dossier de stockage du mod, que Windhawk supprime avec le mod. Le mod
    télécharge les fichiers de Windows 7 dans ce dossier ; un fichier existant
    qui n'est pas la build attendue est d'abord renommé en <nom>.user-backup,
    jamais écrasé.
  $description:de: >-
    Diese Einstellung legt den Ordner mit den Windows 7-Dateien fest;
    Umgebungsvariablen werden erweitert. Leer lassen, um den eigenen
    Speicherordner des Mods zu verwenden, den Windhawk zusammen mit dem Mod
    entfernt. Das Mod lädt die Windows 7-Dateien in diesen Ordner herunter; eine
    vorhandene Datei, die nicht der erwarteten Version entspricht, wird zuerst in
    <Name>.user-backup umbenannt und nie überschrieben.
  $description:pl: >-
    To ustawienie wskazuje folder z plikami Windows 7; zmienne środowiskowe są
    rozwijane. Pozostaw je puste, aby użyć własnego folderu magazynu moda, który
    Windhawk usuwa razem z modem. Mod pobiera pliki Windows 7 do tego folderu;
    istniejący plik, który nie jest oczekiwaną wersją, jest najpierw zmieniany na
    <nazwa>.user-backup i nigdy nie jest nadpisywany.
  $description:zh-CN: >-
    此设置指定存放 Windows 7 文件的文件夹；环境变量会被展开。留空则使用模组自己的存储文件夹，卸载模组时 Windhawk 会一并删除。模组会把所需的 Windows 7 文件下载到此文件夹；其中与预期版本不符的现有文件会先被重命名为 <名称>.user-backup，绝不会被覆盖。
  $description:ja: >-
    この設定は、Windows 7 のファイルを置くフォルダーを指定します。環境変数は展開されます。空にすると、mod 専用の保存フォルダーを使用します（mod の削除時に Windhawk が削除します）。mod は必要な Windows 7 のファイルをこのフォルダーにダウンロードします。想定したビルドと異なる既存のファイルは、上書きされず、先に <名前>.user-backup に名前変更されます。
  $description:ko: >-
    이 설정은 Windows 7 파일이 들어 있는 폴더를 지정합니다. 환경 변수는 확장됩니다. 비워 두면 모드 전용 저장 폴더를 사용하며, 모드를 제거할 때 Windhawk가 함께 삭제합니다. 모드는 필요한 Windows 7 파일을 이 폴더에 다운로드하며, 예상한 빌드가 아닌 기존 파일은 덮어쓰지 않고 먼저 <이름>.user-backup으로 이름이 바뀝니다.
- preloadLocalDlls: true
  $name: Preload the DLLs from the install folder first
  $name:it: Precarica prima le DLL dalla cartella di installazione
  $name:ru: Сначала предзагружать DLL из папки установки
  $name:es: Precargar primero las DLL de la carpeta de instalación
  $name:pt-BR: Pré-carregar primeiro as DLLs da pasta de instalação
  $name:fr: Précharger d'abord les DLL du dossier d'installation
  $name:de: Die DLLs zuerst aus dem Installationsordner vorladen
  $name:pl: Wczytaj najpierw biblioteki DLL z folderu instalacyjnego
  $name:zh-CN: 优先从安装文件夹预加载 DLL
  $name:ja: インストール フォルダーの DLL を先にプリロードする
  $name:ko: 설치 폴더의 DLL을 먼저 미리 로드
  $description: >-
    This setting loads dui71.dll and msswch.dll from the install folder before the
    system copies with the same names. Keep it on: with the system copies, the
    keys that are not plain text come out blank.
  $description:it: >-
    Questa impostazione carica dui71.dll e msswch.dll dalla cartella di
    installazione prima delle copie di sistema con lo stesso nome. Lasciala
    attiva: con le copie di sistema i tasti che non sono testo semplice restano
    vuoti.
  $description:ru: >-
    Этот параметр загружает dui71.dll и msswch.dll из папки установки раньше
    системных копий с теми же именами. Оставьте его включённым: с системными
    копиями клавиши, которые не являются обычным текстом, остаются пустыми.
  $description:es: >-
    Esta opción carga dui71.dll y msswch.dll desde la carpeta de instalación antes
    que las copias del sistema con los mismos nombres. Déjala activada: con las
    copias del sistema, las teclas que no son texto simple salen vacías.
  $description:pt-BR: >-
    Esta configuração carrega dui71.dll e msswch.dll da pasta de instalação antes
    das cópias do sistema com os mesmos nomes. Mantenha-a ativada: com as cópias
    do sistema, as teclas que não são texto simples ficam vazias.
  $description:fr: >-
    Ce paramètre charge dui71.dll et msswch.dll depuis le dossier d'installation
    avant les copies système portant les mêmes noms. Gardez-le activé : avec les
    copies système, les touches qui ne sont pas du texte simple restent vides.
  $description:de: >-
    Diese Einstellung lädt dui71.dll und msswch.dll aus dem Installationsordner
    vor den Systemkopien mit denselben Namen. Eingeschaltet lassen: Mit den
    Systemkopien bleiben die Tasten, die kein reiner Text sind, leer.
  $description:pl: >-
    To ustawienie wczytuje dui71.dll i msswch.dll z folderu instalacyjnego przed
    systemowymi kopiami o tych samych nazwach. Pozostaw je włączone: z systemowymi
    kopiami klawisze, które nie są zwykłym tekstem, pozostają puste.
  $description:zh-CN: >-
    此设置先从安装文件夹加载 dui71.dll 和 msswch.dll，早于同名的系统副本。请保持开启：使用系统副本时，非纯文本的按键会显示为空白。
  $description:ja: >-
    この設定は、同名のシステム側コピーより先に、インストール フォルダーから dui71.dll と msswch.dll を読み込みます。オンのままにしてください。システム側コピーでは、通常のテキストではないキーが空白になります。
  $description:ko: >-
    이 설정은 같은 이름의 시스템 복사본보다 먼저 설치 폴더에서 dui71.dll과 msswch.dll을 로드합니다. 켜 둔 상태로 유지하세요. 시스템 복사본을 쓰면 일반 텍스트가 아닌 키가 비어 있게 됩니다.
- windowsKeyFace: "box"
  $name: Face of the Windows key
  $name:it: Aspetto del tasto Windows
  $name:ru: Вид клавиши Windows
  $name:es: Aspecto de la tecla Windows
  $name:pt-BR: Aparência da tecla Windows
  $name:fr: Apparence de la touche Windows
  $name:de: Aussehen der Windows-Taste
  $name:pl: Wygląd klawisza Windows
  $name:zh-CN: Windows 键的外观
  $name:ja: Windows キーの図柄
  $name:ko: Windows 키 모양
  $description: >-
    This setting chooses how the Windows key is drawn: "box" (the squared plus
    sign U+229E, closest to the classic flag), "win" (the word Win) or "keep" (the
    glyph of the system font, the current logo).
  $description:it: >-
    Questa impostazione sceglie come disegnare il tasto Windows: "box" (il segno
    più quadrato U+229E, il più vicino alla bandiera classica), "win" (la parola
    Win) oppure "keep" (il glifo del font di sistema, il logo attuale).
  $description:ru: >-
    Этот параметр выбирает вид клавиши Windows: "box" (квадратный знак плюса
    U+229E, ближайший к классическому флагу), "win" (слово Win) или "keep" (глиф
    системного шрифта, текущий логотип).
  $description:es: >-
    Esta opción elige cómo se dibuja la tecla Windows: "box" (el signo más
    cuadrado U+229E, lo más parecido a la bandera clásica), "win" (la palabra Win)
    o "keep" (el glifo de la fuente del sistema, el logotipo actual).
  $description:pt-BR: >-
    Esta configuração escolhe como a tecla Windows é desenhada: "box" (o sinal de
    mais quadrado U+229E, o mais próximo da bandeira clássica), "win" (a palavra
    Win) ou "keep" (o glifo da fonte do sistema, o logotipo atual).
  $description:fr: >-
    Ce paramètre choisit l'aspect de la touche Windows : "box" (le signe plus
    encadré U+229E, le plus proche du drapeau classique), "win" (le mot Win) ou
    "keep" (le glyphe de la police système, le logo actuel).
  $description:de: >-
    Diese Einstellung wählt das Aussehen der Windows-Taste: "box" (das eingerahmte
    Pluszeichen U+229E, am nächsten zur klassischen Flagge), "win" (das Wort Win)
    oder "keep" (der Glyph der Systemschriftart, das aktuelle Logo).
  $description:pl: >-
    To ustawienie wybiera wygląd klawisza Windows: "box" (kwadratowy znak plus
    U+229E, najbliższy klasycznej fladze), "win" (słowo Win) lub "keep" (glif
    czcionki systemowej, obecne logo).
  $description:zh-CN: >-
    此设置选择 Windows 键的绘制方式："box"（方框加号 U+229E，最接近经典旗帜）、"win"（单词 Win）或 "keep"（系统字体字形，即当前的徽标）。
  $description:ja: >-
    この設定は Windows キーの図柄を選びます。"box"（四角いプラス記号 U+229E、従来の旗に最も近い）、"win"（Win という語）、"keep"（システム フォントのグリフ、現在のロゴ）です。
  $description:ko: >-
    이 설정은 Windows 키의 모양을 선택합니다. "box"(네모 안 더하기 기호 U+229E, 고전적인 깃발에 가장 가까움), "win"(Win이라는 단어), "keep"(시스템 글꼴의 글리프, 현재 로고)입니다.
- keyFaces: true
  $name: Supply the faces of the keys that are not text
  $name:it: Fornisci l'aspetto dei tasti che non sono testo
  $name:ru: Подставлять изображения клавиш, которые не являются текстом
  $name:es: Proporcionar el aspecto de las teclas que no son texto
  $name:pt-BR: Fornecer a aparência das teclas que não são texto
  $name:fr: Fournir l'apparence des touches qui ne sont pas du texte
  $name:de: Die Darstellung der Tasten bereitstellen, die kein Text sind
  $name:pl: Dostarczaj wygląd klawiszy, które nie są tekstem
  $name:zh-CN: 提供非文本按键的外观
  $name:ja: テキストではないキーの図柄を提供する
  $name:ko: 텍스트가 아닌 키의 모양 제공
  $description: >-
    This setting supplies the faces of the keys that are not text (Windows, menu,
    Enter and the four arrows), drawn inside the mod: the system has no pictures
    for them, and five keys would come out empty.
  $description:it: >-
    Questa impostazione fornisce l'aspetto dei tasti che non sono testo (Windows,
    menu, Invio e le quattro frecce), disegnato dentro il mod: il sistema non ha
    immagini per loro e cinque tasti risulterebbero vuoti.
  $description:ru: >-
    Этот параметр подставляет изображения клавиш, которые не являются текстом
    (Windows, меню, Enter и четыре стрелки), нарисованные внутри мода: у системы
    нет для них картинок, и пять клавиш остались бы пустыми.
  $description:es: >-
    Esta opción proporciona las caras de las teclas que no son texto (Windows,
    menú, Intro y las cuatro flechas), dibujadas dentro del mod: el sistema no
    tiene imágenes para ellas y cinco teclas saldrían vacías.
  $description:pt-BR: >-
    Esta configuração fornece as faces das teclas que não são texto (Windows,
    menu, Enter e as quatro setas), desenhadas dentro do mod: o sistema não tem
    imagens para elas e cinco teclas ficariam vazias.
  $description:fr: >-
    Ce paramètre fournit l'apparence des touches qui ne sont pas du texte
    (Windows, menu, Entrée et les quatre flèches), dessinée dans le mod : le
    système n'a pas d'images pour elles et cinq touches resteraient vides.
  $description:de: >-
    Diese Einstellung liefert die Darstellung der Tasten, die kein Text sind
    (Windows, Menü, Enter und die vier Pfeile), im Mod gezeichnet: Das System hat
    keine Bilder dafür, und fünf Tasten blieben leer.
  $description:pl: >-
    To ustawienie dostarcza wygląd klawiszy, które nie są tekstem (Windows, menu,
    Enter i cztery strzałki), rysowany w modzie: system nie ma dla nich obrazów i
    pięć klawiszy byłoby pustych.
  $description:zh-CN: >-
    此设置提供非文本按键（Windows、菜单、Enter 和四个方向键）的外观，由 mod 自行绘制：系统中没有它们的图像，否则五个按键会显示为空白。
  $description:ja: >-
    この設定は、テキストではないキー（Windows、メニュー、Enter、4 つの矢印キー）の図柄を mod 内で描画して渡します。システムにはこれらの画像がないため、オフにすると 5 つのキーが空白になります。
  $description:ko: >-
    이 설정은 텍스트가 아닌 키(Windows, 메뉴, Enter 및 네 개의 화살표)의 모양을 mod 안에서 그려서 제공합니다. 시스템에는 이들의 그림이 없어, 끄면 다섯 개의 키가 비어 있게 됩니다.
- keyFaceColor: "white"
  $name: Colour of the faces that are supplied
  $name:it: Colore degli aspetti forniti
  $name:ru: Цвет подставляемых изображений
  $name:es: Color de las imágenes proporcionadas
  $name:pt-BR: Cor da aparência fornecida
  $name:fr: Couleur des apparences fournies
  $name:de: Farbe der bereitgestellten Darstellungen
  $name:pl: Kolor dostarczanych wyglądów
  $name:zh-CN: 所提供外观的颜色
  $name:ja: 提供する図柄の色
  $name:ko: 제공되는 모양의 색
  $description: >-
    This setting sets the colour of the supplied faces: "white" (the colour of the
    originals), "black" or a colour such as "#RRGGBB". Changes apply at once,
    without restarting the keyboard.
  $description:it: >-
    Questa impostazione imposta il colore degli aspetti forniti: "white" (il
    colore degli originali), "black" o un colore come "#RRGGBB". Le modifiche si
    applicano subito, senza riavviare la tastiera.
  $description:ru: >-
    Этот параметр задаёт цвет подставляемых изображений: "white" (цвет
    оригиналов), "black" или цвет вида "#RRGGBB". Изменения применяются сразу, без
    перезапуска клавиатуры.
  $description:es: >-
    Esta opción fija el color de las caras proporcionadas: "white" (el color de
    las originales), "black" o un color como "#RRGGBB". Los cambios se aplican al
    momento, sin reiniciar el teclado.
  $description:pt-BR: >-
    Esta configuração define a cor das faces fornecidas: "white" (a cor das
    originais), "black" ou uma cor como "#RRGGBB". As alterações são aplicadas na
    hora, sem reiniciar o teclado.
  $description:fr: >-
    Ce paramètre règle la couleur des apparences fournies : "white" (la couleur
    des originales), "black" ou une couleur au format "#RRGGBB". Les changements
    s'appliquent immédiatement, sans redémarrer le clavier.
  $description:de: >-
    Diese Einstellung legt die Farbe der gelieferten Darstellungen fest: "white"
    (die Farbe der Originale), "black" oder eine Farbe wie "#RRGGBB". Änderungen
    wirken sofort, ohne die Tastatur neu zu starten.
  $description:pl: >-
    To ustawienie ustawia kolor dostarczanych wyglądów: "white" (kolor
    oryginałów), "black" lub kolor w formacie "#RRGGBB". Zmiany działają od razu,
    bez ponownego uruchamiania klawiatury.
  $description:zh-CN: >-
    此设置设定所提供外观的颜色："white"（原始图像的颜色）、"black" 或 "#RRGGBB" 形式的颜色。更改会立即生效，无需重启键盘。
  $description:ja: >-
    この設定は、提供する図柄の色を指定します。"white"（元の画像の色）、"black"、または "#RRGGBB" 形式の色です。変更はキーボードを再起動せずにすぐ適用されます。
  $description:ko: >-
    이 설정은 제공되는 모양의 색을 지정합니다. "white"(원본 그림의 색), "black" 또는 "#RRGGBB" 형식의 색입니다. 변경 사항은 키보드를 다시 시작하지 않고 즉시 적용됩니다.
- diagnostics: true
  $name: Verbose diagnostic logging
  $name:it: Registrazione diagnostica dettagliata
  $name:ru: Подробный диагностический журнал
  $name:es: Registro de diagnóstico detallado
  $name:pt-BR: Log de diagnóstico detalhado
  $name:fr: Journal de diagnostic détaillé
  $name:de: Ausführliche Diagnoseprotokollierung
  $name:pl: Szczegółowe rejestrowanie diagnostyczne
  $name:zh-CN: 详细诊断日志
  $name:ja: 詳細な診断ログ
  $name:ko: 상세 진단 로깅
  $description: >-
    This setting logs the work of the mod in detail: the hook results, the file
    preparation, the DLL and string fallbacks, and every osk-related process start
    with its caller.
  $description:it: >-
    Questa impostazione registra in dettaglio il lavoro del mod: risultati degli
    hook, preparazione dei file, ripieghi di DLL e stringhe e ogni avvio di
    processo legato a osk con il suo chiamante.
  $description:ru: >-
    Этот параметр подробно записывает работу мода: результаты хуков, подготовку
    файлов, резервные варианты для DLL и строк, а также каждый запуск процесса,
    связанного с osk, и его вызывающий модуль.
  $description:es: >-
    Esta opción registra con detalle el trabajo del mod: los resultados de los
    enganches, la preparación de archivos, los respaldos de DLL y cadenas y cada
    inicio de proceso relacionado con osk y su llamador.
  $description:pt-BR: >-
    Esta configuração registra detalhadamente o trabalho do mod: os resultados dos
    hooks, a preparação dos arquivos, os fallbacks de DLL e strings e cada início
    de processo relacionado ao osk com o seu chamador.
  $description:fr: >-
    Ce paramètre journalise en détail le travail du mod : les résultats des hooks,
    la préparation des fichiers, les solutions de repli pour les DLL et les
    chaînes, et chaque lancement de processus lié à osk avec son appelant.
  $description:de: >-
    Diese Einstellung protokolliert die Arbeit des Mods ausführlich:
    Hook-Ergebnisse, Dateivorbereitung, die DLL- und Zeichenfolgen-Ersatzlösungen
    und jeden osk-bezogenen Prozessstart mit seinem Aufrufer.
  $description:pl: >-
    To ustawienie szczegółowo rejestruje pracę moda: wyniki hooków, przygotowanie
    plików, mechanizmy zastępcze dla DLL i ciągów oraz każde uruchomienie procesu
    związanego z osk wraz z modułem wywołującym.
  $description:zh-CN: >-
    此设置详细记录 mod 的工作情况：挂钩结果、文件准备、DLL 和字符串回退，以及每个与 osk 相关的进程启动及其调用模块。
  $description:ja: >-
    この設定は mod の動作を詳しく記録します。フックの結果、ファイルの準備、DLL と文字列のフォールバック、そして osk に関連する各プロセスの起動と呼び出し元です。
  $description:ko: >-
    이 설정은 mod의 작업을 자세히 기록합니다. 후킹 결과, 파일 준비, DLL 및 문자열 폴백, 그리고 osk 관련 프로세스 시작과 호출 모듈입니다.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <winhttp.h>
#include <winver.h>

#include <atomic>
#include <exception>
#include <map>
#include <mutex>
#include <new>
#include <string>
#include <vector>

// Address of the caller of the current function. Windhawk's compiler is clang
// based: the MSVC intrinsic _ReturnAddress() is not declared there, so the
// compiler builtin is used instead.
#if defined(__clang__) || defined(__GNUC__)
#define MOD_CALLER_ADDRESS() __builtin_return_address(0)
#else
#include <intrin.h>
#define MOD_CALLER_ADDRESS() _ReturnAddress()
#endif

namespace {

// ---------------------------------------------------------------------------
// Settings and global state
// ---------------------------------------------------------------------------

std::mutex g_stateLock;
std::wstring g_installDir;
std::atomic<bool> g_autoDownload{true};
std::atomic<bool> g_prepareFiles{true};
std::atomic<bool> g_redirectLaunches{true};
std::atomic<bool> g_handoff{true};
std::atomic<bool> g_preloadLocalDlls{true};
std::wstring g_windowsKeyFace = L"box";  // touched only by LoadSettings
std::atomic<bool> g_keyFaces{true};
std::wstring g_keyFaceColor = L"white";  // touched only by LoadSettings
std::atomic<bool> g_diagnostics{true};

std::wstring GetInstallDir() {
    std::lock_guard<std::mutex> lock(g_stateLock);
    return g_installDir;
}// ---------------------------------------------------------------------------
// Data served to the keyboard outlives this mod
//
// A resource handle is valid for the lifetime of its module, so the keyboard
// is free to cache the handles and the pointers it is given. Anything this mod
// serves therefore has to stay readable after Windhawk has unmapped the
// module: the handles and the bytes live in blocks of the process heap that
// are deliberately never freed (a few KB in all). A copy is reused while the
// bytes do not change, so a keyboard layout switch adds one small block at
// most.
// ---------------------------------------------------------------------------

std::mutex g_servedLock;

struct ServedBlock {
    const void* key;  // what the bytes were served for
    const BYTE* data;
    size_t size;
};

std::vector<ServedBlock> g_servedBlocks;

const BYTE* ServePersistentBytes(const void* key, const std::vector<BYTE>& bytes,
                                size_t* size) {
    std::lock_guard<std::mutex> guard(g_servedLock);
    for (const ServedBlock& block : g_servedBlocks) {
        if (block.key == key && block.size == bytes.size() &&
            memcmp(block.data, bytes.data(), bytes.size()) == 0) {
            *size = block.size;
            return block.data;
        }
    }

    BYTE* copy = (BYTE*)HeapAlloc(GetProcessHeap(), 0, bytes.size() + 1);
    if (!copy) {
        return nullptr;
    }
    memcpy(copy, bytes.data(), bytes.size());
    g_servedBlocks.push_back({key, copy, bytes.size()});
    *size = bytes.size();
    return copy;
}

// The same for a string that is handed out by address: LoadStringW with a
// length of zero writes the address of the string into the caller's buffer,
// and that pointer must not point into this module either.
PCWSTR ServePersistentString(PCWSTR source) {
    if (!source) {
        return nullptr;
    }
    std::vector<BYTE> bytes((wcslen(source) + 1) * sizeof(wchar_t));
    memcpy(bytes.data(), source, bytes.size());
    size_t size = 0;
    const BYTE* copy = ServePersistentBytes(source, bytes, &size);
    return copy ? (PCWSTR)copy : source;
}

HMODULE g_exeModule = nullptr;
// True in the Windows 7 keyboard process (set in Wh_ModInit).
bool g_oskProcess = false;

// Signalled by Wh_ModBeforeUninit so that worker threads finish quickly.
HANDLE g_stopEvent = nullptr;

// Worker threads. Every thread that runs mod code is registered here so that
// Wh_ModUninit can wait for it: mod code must never outlive the mod.
std::mutex g_workersLock;
std::vector<HANDLE> g_workers;

// Every worker starts here. A C++ exception that leaves a thread routine ends
// in std::terminate, which would take the host process (explorer.exe) down, so
// the routine runs inside a try/catch. The log calls in the handlers use string
// literals only: nothing there can allocate and throw again.
struct WorkerStart {
    LPTHREAD_START_ROUTINE routine;
    void* parameter;
    const wchar_t* what;  // a string literal: it outlives the thread
};

DWORD WINAPI WorkerTrampoline(LPVOID context) {
    WorkerStart start = *static_cast<WorkerStart*>(context);
    delete static_cast<WorkerStart*>(context);

    try {
        return start.routine(start.parameter);
    } catch (const std::exception&) {
        Wh_Log(L"A worker thread ended with a C++ exception (std::exception); "
               L"the host process is kept alive");
    } catch (...) {
        Wh_Log(L"A worker thread ended with an unknown C++ exception; the host "
               L"process is kept alive");
    }
    return 1;
}

bool SpawnWorker(LPTHREAD_START_ROUTINE routine, void* parameter,
                 const wchar_t* what) {
    // The check and the registration happen under the same lock as the swap in
    // JoinWorkers below: a thread is either tracked (and waited for) or
    // refused. A thread started after the shutdown began would run code of
    // this mod after Windhawk has unmapped it.
    std::lock_guard<std::mutex> lock(g_workersLock);
    if (g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
        Wh_Log(L"The %s thread is not started: the mod is shutting down", what);
        return false;
    }

    // The room for the handle is reserved before the thread exists: a
    // push_back that failed afterwards would leave a running thread that
    // JoinWorkers does not know about.
    try {
        g_workers.reserve(g_workers.size() + 1);
    } catch (...) {
        Wh_Log(L"Could not create the %s thread (out of memory)", what);
        return false;
    }

    WorkerStart* start = new (std::nothrow) WorkerStart{routine, parameter, what};
    if (!start) {
        Wh_Log(L"Could not create the %s thread (out of memory)", what);
        return false;
    }

    HANDLE thread =
        CreateThread(nullptr, 0, WorkerTrampoline, start, 0, nullptr);
    if (!thread) {
        Wh_Log(L"Could not create the %s thread (error %lu)", what,
               GetLastError());
        delete start;
        return false;
    }

    g_workers.push_back(thread);  // cannot throw: the capacity is reserved
    return true;
}

void JoinWorkers() {
    // Waits without a deadline: Windhawk unmaps this module as soon as
    // Wh_ModUninit returns, so a thread that is still running in it would take
    // the host process down with it. The waits inside the workers are
    // cancellable (g_stopEvent, and CancelHttp for the network), so this
    // returns quickly in practice. The loop is what catches a thread that was
    // started while the handles were being swapped out.
    for (;;) {
        std::vector<HANDLE> threads;
        {
            std::lock_guard<std::mutex> lock(g_workersLock);
            threads.swap(g_workers);
        }
        if (threads.empty()) {
            return;
        }
        for (HANDLE thread : threads) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
        }
    }
}

// Expands environment variables in a string.
std::wstring ExpandVariables(const std::wstring& text) {
    DWORD needed = ExpandEnvironmentStringsW(text.c_str(), nullptr, 0);
    if (needed <= 1) {
        return text;
    }
    std::wstring expanded(needed, L'\0');
    DWORD written =
        ExpandEnvironmentStringsW(text.c_str(), expanded.data(), needed);
    if (written == 0 || written > needed) {
        return text;
    }
    expanded.resize(written - 1);
    return expanded;
}

void LoadSettings() {
    PCWSTR value = Wh_GetStringSetting(L"installFolder");
    std::wstring folder = value ? value : L"";
    Wh_FreeStringSetting(value);

    if (folder.empty()) {
        // The mod's own storage folder: Windhawk creates it next to the mod's
        // settings, every process of the mod gets the same path, and removing
        // the mod removes the files with it. None of the processes this mod is
        // loaded into is packaged, so the folder is seen the same everywhere.
        wchar_t path[32768] = {};
        size_t length = Wh_GetModStoragePath(path, ARRAYSIZE(path));
        if (length == 0 || length >= ARRAYSIZE(path)) {
            Wh_Log(L"Windhawk did not provide the mod storage path; the folder "
                   L"of the input files has to be set by hand");
            folder = L"%USERPROFILE%\\AppData\\Local\\Win7OnScreenKeyboard";
        } else {
            folder = path;
        }
    }
    folder = ExpandVariables(folder);

    while (folder.size() > 3 &&
           (folder.back() == L'\\' || folder.back() == L'/')) {
        folder.pop_back();
    }

    {
        std::lock_guard<std::mutex> lock(g_stateLock);
        g_installDir = folder;
    }

    // These have no setting on purpose: the mod must work on its own, so the
    // files are always downloaded from the symbol server, always verified
    // against the pinned SHA-256 digests, always prepared, and the Windows 7
    // keyboard is always used as soon as its files are there. The globals keep
    // their initial value of true.
    g_preloadLocalDlls = Wh_GetIntSetting(L"preloadLocalDlls") != 0;

    {
        PCWSTR face = Wh_GetStringSetting(L"windowsKeyFace");
        if (face && *face) {
            g_windowsKeyFace = face;
        } else {
            g_windowsKeyFace = L"box";
        }
        Wh_FreeStringSetting(face);
    }

    g_keyFaces = Wh_GetIntSetting(L"keyFaces") != 0;

    {
        PCWSTR colour = Wh_GetStringSetting(L"keyFaceColor");
        if (colour && *colour) {
            g_keyFaceColor = colour;
        } else {
            g_keyFaceColor = L"white";
        }
        Wh_FreeStringSetting(colour);
    }
    g_diagnostics = Wh_GetIntSetting(L"diagnostics") != 0;
}

// ---------------------------------------------------------------------------
// Logging helpers
// ---------------------------------------------------------------------------

// At most one message per interval; suppressed events are counted.
struct RateLimit {
    std::atomic<ULONGLONG> nextTick{0};

    bool Allow(DWORD intervalMs) {
        ULONGLONG now = GetTickCount64();
        ULONGLONG next = nextTick.load(std::memory_order_relaxed);
        if (now < next) {
            return false;
        }
        return nextTick.compare_exchange_strong(next, now + intervalMs);
    }
};

// Describes the module that made a call, as "module.dll+0x1234". Used to find
// out which DLL inside explorer.exe / SearchHost.exe actually starts osk.exe.
std::wstring DescribeCallerModule(void* returnAddress) {
    if (!returnAddress) {
        return L"(no return address)";
    }

    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(returnAddress), &module) ||
        !module) {
        return L"(unmapped address)";
    }

    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return L"(unknown module)";
    }

    wchar_t text[MAX_PATH + 32];
    swprintf(text, ARRAYSIZE(text), L"%s+0x%llX", PathFindFileNameW(path),
             (unsigned long long)((ULONG_PTR)returnAddress - (ULONG_PTR)module));
    return text;
}

std::wstring DescribeExitCode(DWORD code) {
    wchar_t text[160];
    PCWSTR name = nullptr;
    switch (code) {
        case 0xC0000135:
            name = L"STATUS_DLL_NOT_FOUND (a required DLL is missing)";
            break;
        case 0xC0000139:
            name = L"STATUS_ENTRYPOINT_NOT_FOUND (DLL version mismatch)";
            break;
        case 0xC0000142:
            name = L"STATUS_DLL_INIT_FAILED";
            break;
        case 0xC000007B:
            name = L"STATUS_INVALID_IMAGE_FORMAT";
            break;
        case 0xC0000005:
            name = L"STATUS_ACCESS_VIOLATION";
            break;
        case 0xC000013A:
            name = L"STATUS_CONTROL_C_EXIT";
            break;
        case 0:
            name = L"exit code 0 (normal)";
            break;
        default:
            if (code == 1 && g_oskProcess) {
                // Observed behaviour of the Windows 7 keyboard: closing its
                // window makes it quit with code 1, so this is its normal exit
                // path, not a failure.
                name = L"exit code 1 (the Windows 7 keyboard quits with this "
                       L"code when its window is closed)";
            } else if (code == 1) {
                name = L"exit code 1 (the program reported a startup error)";
            }
            break;
    }

    if (name) {
        swprintf(text, ARRAYSIZE(text), L"0x%08lX - %s", code, name);
    } else {
        swprintf(text, ARRAYSIZE(text), L"0x%08lX", code);
    }
    return text;
}

// Explains the errors that matter for this mod.
std::wstring DescribeLaunchError(DWORD error) {
    switch (error) {
        case 740:
            return L"740 ERROR_ELEVATION_REQUIRED - the program declares "
                   L"uiAccess=\"true\" and Windows only starts such programs "
                   L"from a protected location (Program Files, System32). The "
                   L"copy in the install folder is not prepared yet: the setup "
                   L"prepares it on its own";
        case 2:
            return L"2 ERROR_FILE_NOT_FOUND - the file is missing";
        case 5:
            return L"5 ERROR_ACCESS_DENIED";
        case 193:
            return L"193 ERROR_BAD_EXE_FORMAT";
        case 216:
            return L"216 ERROR_EXE_MACHINE_TYPE_MISMATCH - 32 bit copy on a "
                   L"64 bit system?";
        default:
            return L"";
    }
}

// ---------------------------------------------------------------------------
// Path helpers
// ---------------------------------------------------------------------------

bool EqualsNoCase(PCWSTR a, PCWSTR b) {
    return a && b && _wcsicmp(a, b) == 0;
}

std::wstring BaseName(PCWSTR path) {
    if (!path) {
        return L"";
    }
    PCWSTR base = PathFindFileNameW(path);
    return base ? base : L"";
}

std::wstring JoinPath(const std::wstring& dir, PCWSTR name) {
    std::wstring result = dir;
    if (!result.empty() && result.back() != L'\\' && result.back() != L'/') {
        result += L'\\';
    }
    result += name;
    return result;
}

bool FileExists(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring GetProcessImagePath() {
    std::wstring path(MAX_PATH, L'\0');
    for (;;) {
        DWORD length =
            GetModuleFileNameW(nullptr, path.data(), (DWORD)path.size());
        if (length == 0) {
            return L"";
        }
        if (length < path.size() - 1) {
            path.resize(length);
            return path;
        }
        if (path.size() >= 32768) {
            return L"";
        }
        path.resize(path.size() * 2);
    }
}

std::wstring DirectoryPart(PCWSTR path) {
    if (!path) {
        return L"";
    }
    PCWSTR base = PathFindFileNameW(path);
    if (!base || base == path) {
        return L"";
    }
    std::wstring dir(path, (size_t)(base - path));
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }
    return dir;
}

std::wstring NormalizePath(PCWSTR path) {
    if (!path) {
        return L"";
    }

    std::wstring input = path;
    if (input.find(L'%') != std::wstring::npos) {
        input = ExpandVariables(input);
    }

    wchar_t full[32768];
    DWORD length =
        GetFullPathNameW(input.c_str(), ARRAYSIZE(full), full, nullptr);
    if (length == 0 || length >= ARRAYSIZE(full)) {
        return input;
    }

    wchar_t longPath[32768];
    DWORD longLength = GetLongPathNameW(full, longPath, ARRAYSIZE(longPath));
    if (longLength > 0 && longLength < ARRAYSIZE(longPath)) {
        std::wstring result(longPath, longLength);
        if (result.rfind(L"\\\\?\\", 0) == 0) {
            return result.substr(4);
        }
        return result;
    }

    std::wstring result(full, length);
    if (result.rfind(L"\\\\?\\", 0) == 0) {
        return result.substr(4);
    }
    return result;
}

bool PathsEqual(const std::wstring& a, const std::wstring& b) {
    return !a.empty() && !b.empty() && _wcsicmp(a.c_str(), b.c_str()) == 0;
}

// ---------------------------------------------------------------------------
// The Windows 7 files: verification
// ---------------------------------------------------------------------------

// SHA-256 digests (64 uppercase hex characters) of the four files, exactly as
// the Microsoft symbol server serves them. The digest is what pins a file: the
// timestamp and the image size below are two header fields that any x64 PE can
// carry, so they are only used for the diagnostics.
//
// osk.exe is the only file this mod edits, and the preparation is a
// deterministic edit of its bytes, so the digest of the prepared form is
// pinned as well. CheckPinnedFile below also accepts a prepared file that is
// exactly the pinned edit of the pinned original (see PrepareBytes), which is
// the same guarantee computed rather than stored.
constexpr PCWSTR kShaOskOriginal =
    L"E1F7612086C2D01F15F2E74F1C22BC6ABEB56F18E6BDA058EDCE8D780AEBB353";
constexpr PCWSTR kShaOskPrepared =
    L"E72F9DC42FF825542F3753BE3C424166BDCBF6B3889E0EE759D028198D993159";
constexpr PCWSTR kShaDui70 =
    L"98D21EFFF511E407336A226420701E82554DA01FA05661303836B6860D63749D";
constexpr PCWSTR kShaMsswch =
    L"F8802FC97CC07102731542EF65B42871D01B9C2CDEAD1A3DEB7071E807C1EBFB";
constexpr PCWSTR kShaTabskb =
    L"92B399510B50B0FD5AB3CE3ADD9642A0D4E02D5295FCDBC283027EA62FC632FE";

struct RemoteFile {
    PCWSTR name;            // name in the install folder
    PCWSTR urlName;         // name on the symbol server (dui71.dll <- dui70.dll)
    DWORD timestamp;
    DWORD sizeOfImage;
    PCWSTR role;
    PCWSTR sha256;          // digest of the file as downloaded
    PCWSTR sha256Prepared;  // digest of its prepared form, when it has one
};

const RemoteFile kFiles[] = {
    {L"osk.exe", L"osk.exe", 0x4A5BD272, 0xAC000, L"the keyboard",
     kShaOskOriginal, kShaOskPrepared},
    {L"dui71.dll", L"dui70.dll", 0x4A5BDF25, 0xF2000,
     L"Windows 7 DirectUI, renamed so it does not collide with the system one",
     kShaDui70, nullptr},
    {L"msswch.dll", L"msswch.dll", 0x4A5BDFAD, 0x9000,
     L"static import of osk.exe", kShaMsswch, nullptr},
    {L"tabskb.dll", L"tabskb.dll", 0x4CE7C9CF, 0x72000,
     L"the keyboard layout", kShaTabskb, nullptr},
};

enum class PeCheck { Invalid, ValidButOther, Exact };

PeCheck CheckPeFile(const std::wstring& path, const RemoteFile& expected) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return PeCheck::Invalid;
    }

    BYTE buffer[4096];
    DWORD read = 0;
    PeCheck result = PeCheck::Invalid;
    if (ReadFile(file, buffer, sizeof(buffer), &read, nullptr) &&
        read >= sizeof(IMAGE_DOS_HEADER)) {
        auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(buffer);
        if (dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew > 0 &&
            (size_t)dos->e_lfanew + sizeof(IMAGE_NT_HEADERS64) <= read) {
            auto nt =
                reinterpret_cast<IMAGE_NT_HEADERS64*>(buffer + dos->e_lfanew);
            if (nt->Signature == IMAGE_NT_SIGNATURE &&
                nt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
                result = (nt->FileHeader.TimeDateStamp == expected.timestamp &&
                          nt->OptionalHeader.SizeOfImage ==
                              expected.sizeOfImage)
                             ? PeCheck::Exact
                             : PeCheck::ValidButOther;
            }
        }
    }

    CloseHandle(file);
    return result;
}

// The two helpers live with the install-folder code further down, after this
// block: declared here because the check of the prepared osk.exe needs them.
bool ReadFileBytes(const std::wstring& path, std::vector<BYTE>& out);
bool PrepareBytes(const std::vector<BYTE>& original, std::vector<BYTE>& out);

// ---------------------------------------------------------------------------
// SHA-256 of a file, through the CryptoAPI (advapi32 is already linked).
//
// CheckPeFile above only compares the timestamp and the image size, and any
// x64 PE can be built with the same two values: the digest is what proves that
// a file is the pinned build.
// ---------------------------------------------------------------------------

bool Sha256OfFile(const std::wstring& path, BYTE digest[32]) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    HCRYPTPROV provider = 0;
    HCRYPTHASH hash = 0;
    bool ok = false;
    if (CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_AES,
                             CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash)) {
            BYTE buffer[64 * 1024];
            DWORD read = 0;
            ok = true;
            for (;;) {
                if (!ReadFile(file, buffer, sizeof(buffer), &read, nullptr)) {
                    ok = false;
                    break;
                }
                if (read == 0) {
                    break;
                }
                if (!CryptHashData(hash, buffer, read, 0)) {
                    ok = false;
                    break;
                }
            }
            DWORD length = 32;
            if (ok &&
                !CryptGetHashParam(hash, HP_HASHVAL, digest, &length, 0)) {
                ok = false;
            }
            CryptDestroyHash(hash);
        }
        CryptReleaseContext(provider, 0);
    }

    CloseHandle(file);
    return ok;
}

wchar_t HexUpperChar(BYTE value) {
    return value < 10 ? (wchar_t)(L'0' + value) : (wchar_t)(L'A' + value - 10);
}

bool Sha256MatchesHex(const BYTE digest[32], PCWSTR expectedHex) {
    if (!expectedHex) {
        return false;
    }
    for (int i = 0; i < 32; i++) {
        if (expectedHex[i * 2] != HexUpperChar(digest[i] >> 4) ||
            expectedHex[i * 2 + 1] != HexUpperChar(digest[i] & 0xF)) {
            return false;
        }
    }
    return true;
}

bool Sha256MatchesFile(const std::wstring& path, PCWSTR expectedHex) {
    BYTE digest[32];
    return Sha256OfFile(path, digest) && Sha256MatchesHex(digest, expectedHex);
}

constexpr PCWSTR kOskBackupSuffix = L".original";

// The prepared osk.exe is the pinned file with the two documented edits, and
// the edits are deterministic, so the prepared image can be recomputed from
// the pinned original and compared byte by byte with the file on disk. This
// accepts a prepared file whose digest is not the pinned one without ever
// accepting a file that is not the pinned image.
bool IsPinnedPreparation(const std::wstring& preparedPath,
                         const std::wstring& originalPath) {
    std::vector<BYTE> original;
    if (!ReadFileBytes(originalPath, original)) {
        return false;
    }
    if (!Sha256MatchesFile(originalPath, kShaOskOriginal)) {
        return false;
    }

    std::vector<BYTE> expected;
    if (!PrepareBytes(original, expected)) {
        return false;
    }

    std::vector<BYTE> prepared;
    if (!ReadFileBytes(preparedPath, prepared)) {
        return false;
    }
    return prepared.size() == expected.size() &&
           memcmp(prepared.data(), expected.data(), prepared.size()) == 0;
}

// ---------------------------------------------------------------------------
// The verification of a file in the install folder.
//
// Exact means: the bytes are the pinned Windows 7 build. osk.exe is accepted
// in its prepared form as well: either its digest is the pinned one, or the
// prepared file is exactly the deterministic edit of the pinned original.
// ValidButOther means a readable x64 image that is not the pinned build: it is
// not used, and EnsureFiles replaces it.
// ---------------------------------------------------------------------------

PeCheck CheckPinnedFile(const std::wstring& path, const RemoteFile& expected) {
    PeCheck pe = CheckPeFile(path, expected);
    if (pe == PeCheck::Invalid) {
        return PeCheck::Invalid;
    }

    BYTE digest[32];
    if (!Sha256OfFile(path, digest)) {
        return PeCheck::ValidButOther;
    }
    if (Sha256MatchesHex(digest, expected.sha256)) {
        return PeCheck::Exact;
    }
    if (expected.sha256Prepared &&
        Sha256MatchesHex(digest, expected.sha256Prepared)) {
        return PeCheck::Exact;
    }
    if (expected.sha256Prepared &&
        IsPinnedPreparation(path, path + kOskBackupSuffix)) {
        return PeCheck::Exact;
    }
    return PeCheck::ValidButOther;
}

bool ReadFileBytes(const std::wstring& path, std::vector<BYTE>& out) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 64 * 1024 * 1024) {
        CloseHandle(file);
        return false;
    }

    out.resize((size_t)size.QuadPart);
    DWORD read = 0;
    bool ok = ReadFile(file, out.data(), (DWORD)out.size(), &read, nullptr) &&
              read == out.size();
    CloseHandle(file);
    return ok;
}

bool WriteFileBytes(const std::wstring& path, const std::vector<BYTE>& bytes) {
    std::wstring temp = path + L".windhawk-tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written = 0;
    bool ok = WriteFile(file, bytes.data(), (DWORD)bytes.size(), &written,
                        nullptr) &&
              written == bytes.size();
    if (ok) {
        FlushFileBuffers(file);
    }
    CloseHandle(file);

    if (ok && !MoveFileExW(temp.c_str(), path.c_str(),
                           MOVEFILE_REPLACE_EXISTING)) {
        ok = false;
    }
    if (!ok) {
        DeleteFileW(temp.c_str());
    }
    return ok;
}

// ---------------------------------------------------------------------------
// Minimal PE reader: just enough to locate the RT_MANIFEST resource, which is
// needed to grow the manifest by one byte without moving anything else in the
// file (the resource size field in the resource directory is bumped instead).
// ---------------------------------------------------------------------------

struct PeSection {
    DWORD virtualAddress;
    DWORD virtualSize;
    DWORD rawOffset;
    DWORD rawSize;
};

struct PeManifest {
    size_t dataOffset = 0;      // file offset of the manifest bytes
    DWORD dataSize = 0;         // resource size
    size_t dataEntryOffset = 0; // file offset of the data entry (RVA + Size)
};

bool PeReadWord(const std::vector<BYTE>& data, size_t offset, WORD* value) {
    if (offset + sizeof(WORD) > data.size()) {
        return false;
    }
    memcpy(value, data.data() + offset, sizeof(WORD));  // never assume alignment
    return true;
}

bool PeReadDword(const std::vector<BYTE>& data, size_t offset, DWORD* value) {
    if (offset + sizeof(DWORD) > data.size()) {
        return false;
    }
    memcpy(value, data.data() + offset, sizeof(DWORD));
    return true;
}

bool PeRvaToOffset(const std::vector<PeSection>& sections, DWORD rva,
                   size_t* offset) {
    for (const auto& section : sections) {
        DWORD size = section.virtualSize > section.rawSize ? section.virtualSize
                                                           : section.rawSize;
        if (rva >= section.virtualAddress &&
            rva < section.virtualAddress + size) {
            *offset = (size_t)section.rawOffset + (rva - section.virtualAddress);
            return true;
        }
    }
    return false;
}

// Reads the type/name/language directories of a resource directory entry list
// and looks for the first leaf of type |typeId|.
bool PeFindResourceLeaf(const std::vector<BYTE>& data,
                        const std::vector<PeSection>& sections, size_t dirBase,
                        size_t dirSize, WORD typeId, DWORD* dataRva,
                        DWORD* dataSize, size_t* leafEntryOffset) {
    size_t dirEnd = dirBase + dirSize;

    auto readEntry = [&](size_t entryOffset, DWORD* nameId, size_t* next) {
        DWORD name = 0;
        DWORD sub = 0;
        if (!PeReadDword(data, entryOffset, &name) ||
            !PeReadDword(data, entryOffset + 4, &sub)) {
            return false;
        }
        size_t target = dirBase + (sub & 0x7FFFFFFF);
        if (target >= dirEnd) {
            return false;
        }
        if (nameId) {
            *nameId = name;
        }
        *next = target;
        return true;
    };

    auto countEntries = [&](size_t dirOffset, WORD* count) {
        WORD named = 0;
        WORD ids = 0;
        if (!PeReadWord(data, dirOffset + 12, &named) ||
            !PeReadWord(data, dirOffset + 14, &ids)) {
            return false;
        }
        *count = (WORD)(named + ids);
        return true;
    };

    WORD typeCount = 0;
    if (!countEntries(dirBase, &typeCount)) {
        return false;
    }

    for (WORD t = 0; t < typeCount; t++) {
        DWORD typeName = 0;
        size_t typeDir = 0;
        if (!readEntry(dirBase + 16 + (size_t)t * 8, &typeName, &typeDir)) {
            continue;
        }
        if ((typeName & 0x80000000) || (WORD)typeName != typeId) {
            continue;  // only integer type ids are of interest here
        }

        WORD nameCount = 0;
        if (!countEntries(typeDir, &nameCount)) {
            continue;
        }
        for (WORD n = 0; n < nameCount; n++) {
            DWORD nameId = 0;
            size_t nameDir = 0;
            if (!readEntry(typeDir + 16 + (size_t)n * 8, &nameId, &nameDir)) {
                continue;
            }

            WORD languageCount = 0;
            if (!countEntries(nameDir, &languageCount)) {
                continue;
            }
            for (WORD l = 0; l < languageCount; l++) {
                size_t entry = nameDir + 16 + (size_t)l * 8;
                DWORD entryName = 0;
                DWORD sub = 0;
                if (!PeReadDword(data, entry, &entryName) ||
                    !PeReadDword(data, entry + 4, &sub)) {
                    continue;
                }

                // As at every other level, the offset points inside the
                // resource directory; it is the data entry that holds the RVA
                // and the size of the bytes.
                size_t dataEntry = dirBase + (sub & 0x7FFFFFFF);
                if (dataEntry < dirBase || dataEntry + 8 > data.size()) {
                    continue;
                }

                DWORD rva = 0;
                DWORD size = 0;
                if (!PeReadDword(data, dataEntry, &rva) ||
                    !PeReadDword(data, dataEntry + 4, &size)) {
                    continue;
                }

                size_t offset = 0;
                if (!PeRvaToOffset(sections, rva, &offset) ||
                    offset + size > data.size()) {
                    continue;
                }

                *dataRva = rva;
                *dataSize = size;
                *leafEntryOffset = dataEntry;  // Size field at +4
                return true;
            }
        }
    }
    return false;
}

// Finds the RT_MANIFEST resource of a PE image.
bool PeFindManifest(const std::vector<BYTE>& data, PeManifest* manifest) {
    if (data.size() < 0x40 || data[0] != 'M' || data[1] != 'Z') {
        return false;
    }

    DWORD e_lfanew = 0;
    if (!PeReadDword(data, 0x3C, &e_lfanew) || e_lfanew + 24 > data.size() ||
        memcmp(data.data() + e_lfanew, "PE\0\0", 4) != 0) {
        return false;
    }

    size_t fileHeader = (size_t)e_lfanew + 4;
    WORD numberOfSections = 0;
    WORD sizeOfOptionalHeader = 0;
    if (!PeReadWord(data, fileHeader + 2, &numberOfSections) ||
        !PeReadWord(data, fileHeader + 16, &sizeOfOptionalHeader)) {
        return false;
    }

    size_t optionalHeader = fileHeader + 20;
    WORD magic = 0;
    if (!PeReadWord(data, optionalHeader, &magic) || magic != 0x20B) {
        return false;  // x64 images only
    }

    // Data directory 2 = resource table (PE32+: 112 bytes into the optional
    // header).
    DWORD resourceRva = 0;
    DWORD resourceSize = 0;
    if (!PeReadDword(data, optionalHeader + 112 + 2 * 8, &resourceRva) ||
        !PeReadDword(data, optionalHeader + 112 + 2 * 8 + 4, &resourceSize) ||
        !resourceRva || resourceSize < 16) {
        return false;
    }

    std::vector<PeSection> sections;
    sections.reserve(numberOfSections);
    size_t sectionOffset = optionalHeader + sizeOfOptionalHeader;
    for (WORD i = 0; i < numberOfSections; i++) {
        size_t entry = sectionOffset + (size_t)i * 40;
        PeSection section{};
        if (!PeReadDword(data, entry + 8, &section.virtualSize) ||
            !PeReadDword(data, entry + 12, &section.virtualAddress) ||
            !PeReadDword(data, entry + 16, &section.rawSize) ||
            !PeReadDword(data, entry + 20, &section.rawOffset)) {
            return false;
        }
        sections.push_back(section);
    }

    size_t directoryOffset = 0;
    if (!PeRvaToOffset(sections, resourceRva, &directoryOffset) ||
        directoryOffset + resourceSize > data.size()) {
        return false;
    }

    DWORD manifestRva = 0;
    DWORD manifestSize = 0;
    size_t leafEntryOffset = 0;
    if (!PeFindResourceLeaf(data, sections, directoryOffset, resourceSize, 24,
                            &manifestRva, &manifestSize, &leafEntryOffset)) {
        return false;
    }

    size_t manifestOffset = 0;
    if (!PeRvaToOffset(sections, manifestRva, &manifestOffset)) {
        return false;
    }

    manifest->dataOffset = manifestOffset;
    manifest->dataSize = manifestSize;
    manifest->dataEntryOffset = leafEntryOffset;
    return true;
}

size_t FindBytes(const std::vector<BYTE>& data, PCSTR needle, size_t from = 0) {
    size_t needleLength = strlen(needle);
    if (needleLength == 0 || data.size() < needleLength) {
        return (size_t)-1;
    }
    for (size_t i = from; i + needleLength <= data.size(); i++) {
        if (memcmp(data.data() + i, needle, needleLength) == 0) {
            return i;
        }
    }
    return (size_t)-1;
}

// Search limited to a byte range (used to look for the uiAccess attribute
// inside the manifest only).
size_t FindBytesInRange(const std::vector<BYTE>& data, PCSTR needle, size_t from,
                        size_t to) {
    size_t needleLength = strlen(needle);
    if (needleLength == 0 || to > data.size() || from >= to ||
        to - from < needleLength) {
        return (size_t)-1;
    }
    for (size_t i = from; i + needleLength <= to; i++) {
        if (memcmp(data.data() + i, needle, needleLength) == 0) {
            return i;
        }
    }
    return (size_t)-1;
}

size_t CountBytes(const std::vector<BYTE>& data, PCSTR needle) {
    size_t needleLength = strlen(needle);
    size_t count = 0;
    size_t pos = FindBytes(data, needle);
    while (pos != (size_t)-1) {
        count++;
        pos = FindBytes(data, needle, pos + needleLength);
    }
    return count;
}

// ---------------------------------------------------------------------------
// File preparation: the two same-length edits
// ---------------------------------------------------------------------------

// State of a local osk.exe with respect to the preparation.
struct OskState {
    bool delayLoadPatched = false;  // DUI71.dll in the delay-load table
    bool delayLoadPristine = false; // DUI70.dll in the delay-load table
    bool manifestPatched = false;   // no uiAccess="true" in the manifest
    bool manifestPristine = false;  // uiAccess="true" still there
    bool succeeded = false;         // file written
};

// Rewrites uiAccess="true" into uiAccess="false".
//
// The replacement is one byte longer, and the file must not change length: the
// PE section table keeps describing the very same offsets. The manifest
// resource is always padded to a 4 byte boundary, so the manifest is allowed to
// grow by up to (4 - size % 4) bytes without touching anything else: the
// attribute value grows, the following bytes of the manifest slide right by one
// and the last one lands on the resource's own padding byte. The resource size
// field in the resource directory is then incremented, exactly like the
// community package for Windows 10/11 does it.
bool PatchManifestUiAccess(std::vector<BYTE>& data, size_t* patchedAt,
                           bool quiet = false) {
    PeManifest manifest;
    if (!PeFindManifest(data, &manifest)) {
        if (!quiet) {
            Wh_Log(L"Preparation: RT_MANIFEST not found, the manifest is left "
                   L"untouched");
        }
        return false;
    }

    const size_t manifestEnd = manifest.dataOffset + manifest.dataSize;
    if (manifestEnd > data.size()) {
        if (!quiet) {
            Wh_Log(
                L"Preparation: the manifest resource does not fit in the file");
        }
        return false;
    }

    size_t position = FindBytesInRange(data, "uiAccess=\"true\"",
                                       manifest.dataOffset, manifestEnd);
    if (position == (size_t)-1) {
        if (!quiet) {
            Wh_Log(L"Preparation: the manifest carries no uiAccess=\"true\" "
                   L"(already prepared or a different manifest)");
        }
        return false;
    }

    DWORD padding = 4 - (manifest.dataSize % 4);
    if (padding < 1 || manifestEnd + 1 > data.size()) {
        if (!quiet) {
            Wh_Log(L"Preparation: no padding byte after the manifest resource, "
                   L"the manifest is left untouched");
        }
        return false;
    }

    const char kOldText[] = "uiAccess=\"true\"";
    const char kNewText[] = "uiAccess=\"false\"";
    const size_t oldLength = sizeof(kOldText) - 1;
    const size_t newLength = sizeof(kNewText) - 1;
    static_assert(sizeof(kNewText) == sizeof(kOldText) + 1, "one byte longer");

    // Slide the rest of the manifest one byte to the right (it ends on the
    // resource's padding byte, which is part of the file).
    size_t tailStart = position + oldLength;
    if (manifestEnd > tailStart) {
        memmove(data.data() + tailStart + 1, data.data() + tailStart,
                manifestEnd - tailStart);
    }
    memcpy(data.data() + position, kNewText, newLength);

    // The manifest now covers the padding byte: extend the resource.
    DWORD newSize = manifest.dataSize + 1;
    memcpy(data.data() + manifest.dataEntryOffset + 4, &newSize, sizeof(newSize));

    if (patchedAt) {
        *patchedAt = position;
    }
    if (!quiet) {
        Wh_Log(L"Preparation: manifest uiAccess=\"true\" -> \"false\" at "
               L"offset 0x%llX; the manifest resource grows from %lu to %lu "
               L"bytes (no other byte of the file moves)",
               (unsigned long long)position, (unsigned long)manifest.dataSize,
               (unsigned long)newSize);
    }
    return true;
}

// Applies the preparation to one file. Only called for files that verified as
// the pinned Windows 7 build and only when the user asked for it.
// The two preparation edits, applied to a copy of the bytes. PrepareOskFile
// below performs the same edits on the file; they are kept in one place so
// that a prepared osk.exe can also be verified by recomputing it.
bool PrepareBytes(const std::vector<BYTE>& original, std::vector<BYTE>& out) {
    out = original;

    size_t position = FindBytes(out, "DUI70.dll");
    if (position == (size_t)-1 || CountBytes(out, "DUI70.dll") != 1) {
        return false;
    }
    memcpy(out.data() + position, "DUI71.dll", 9);

    if (FindBytes(out, "uiAccess=\"true\"") == (size_t)-1) {
        return true;  // nothing more to do: the manifest is already prepared
    }
    return PatchManifestUiAccess(out, nullptr, true);
}

OskState PrepareOskFile(const std::wstring& path) {
    OskState state;
    std::vector<BYTE> data;
    if (!ReadFileBytes(path, data)) {
        Wh_Log(L"Preparation: cannot read %s (error %lu)", path.c_str(),
               GetLastError());
        return state;
    }

    const size_t originalSize = data.size();
    bool changed = false;

    // 1) Delay-load table: DUI70.dll -> DUI71.dll (exactly the same length).
    size_t duiPos = FindBytes(data, "DUI70.dll");
    size_t duiCount = CountBytes(data, "DUI70.dll");
    if (duiPos == (size_t)-1) {
        state.delayLoadPatched = FindBytes(data, "DUI71.dll") != (size_t)-1;
        state.delayLoadPristine = false;
    } else if (duiCount == 1) {
        memcpy(data.data() + duiPos, "DUI71.dll", 9);
        state.delayLoadPatched = true;
        state.delayLoadPristine = true;
        changed = true;
        Wh_Log(L"Preparation: delay-load name DUI70.dll -> DUI71.dll at offset "
               L"0x%llX",
               (unsigned long long)duiPos);
    } else {
        Wh_Log(L"Preparation: \"DUI70.dll\" appears %llu times, the delay-load "
               L"table is not patched (unexpected file)",
               (unsigned long long)duiCount);
        state.delayLoadPristine = true;
    }

    // 2) Manifest: uiAccess="true" -> uiAccess="false", file length kept.
    size_t uiPos = 0;
    if (FindBytes(data, "uiAccess=\"true\"") == (size_t)-1) {
        state.manifestPatched = true;
        state.manifestPristine = false;
    } else {
        state.manifestPristine = true;
        if (PatchManifestUiAccess(data, &uiPos)) {
            state.manifestPatched = true;
            state.manifestPristine = false;
            changed = true;
        }
    }

    if (!changed) {
        Wh_Log(L"Preparation: %s needs no change (already prepared)",
               path.c_str());
        state.succeeded = true;
        return state;
    }

    // The file must keep its identity: same length, same PE timestamp/size.
    if (data.size() != originalSize) {
        Wh_Log(L"Preparation: aborted, the file length changed");
        return state;
    }

    std::wstring temp = path + L".prepared";
    if (!WriteFileBytes(temp, data)) {
        Wh_Log(L"Preparation: cannot write %s (error %lu)", temp.c_str(),
               GetLastError());
        return state;
    }

    // The backup is the un-prepared copy from the symbol server: it is
    // replaced when an older one is already there, so the preparation stays
    // repeatable (the token and image size above prove what it is).
    std::wstring backup = path + L".original";
    if (!MoveFileExW(path.c_str(), backup.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"Preparation: cannot back up the original file (error %lu)",
               GetLastError());
        DeleteFileW(temp.c_str());
        return state;
    }

    if (!MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"Preparation: cannot replace %s (error %lu)", path.c_str(),
               GetLastError());
        MoveFileExW(backup.c_str(), path.c_str(), 0);
        DeleteFileW(temp.c_str());
        return state;
    }

    // Verify: the prepared file must still be the Windows 7 image, both edits
    // must be visible and the manifest resource must still parse as the XML it
    // is (size field extended, text intact).
    std::vector<BYTE> check;
    const RemoteFile& expected = kFiles[0];
    bool manifestOk = false;
    if (ReadFileBytes(path, check) && check.size() == originalSize) {
        PeManifest manifest;
        if (PeFindManifest(check, &manifest) &&
            manifest.dataOffset + manifest.dataSize <= check.size()) {
            const size_t end = manifest.dataOffset + manifest.dataSize;
            manifestOk =
                FindBytesInRange(check, "<assembly", manifest.dataOffset, end) !=
                    (size_t)-1 &&
                FindBytesInRange(check, "</assembly>", manifest.dataOffset, end) !=
                    (size_t)-1 &&
                FindBytesInRange(check, "uiAccess=\"false\"", manifest.dataOffset,
                                 end) != (size_t)-1;
        }
    }
    bool verified = manifestOk &&
                    FindBytes(check, "DUI71.dll") != (size_t)-1 &&
                    FindBytes(check, "uiAccess=\"true\"") == (size_t)-1 &&
                    CheckPinnedFile(path, expected) == PeCheck::Exact;

    if (!verified) {
        Wh_Log(L"Preparation: verification failed, restoring the original file");
        MoveFileExW(backup.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING);
        return state;
    }

    Wh_Log(L"Preparation: %s prepared (original kept as %s)", path.c_str(),
           backup.c_str());
    state.succeeded = true;
    return state;
}

// Reads the preparation state without changing anything.
OskState InspectOskFile(const std::wstring& path) {
    OskState state;
    std::vector<BYTE> data;
    if (!ReadFileBytes(path, data)) {
        return state;
    }
    state.delayLoadPristine = FindBytes(data, "DUI70.dll") != (size_t)-1;
    state.delayLoadPatched = FindBytes(data, "DUI71.dll") != (size_t)-1;
    state.manifestPristine = FindBytes(data, "uiAccess=\"true\"") != (size_t)-1;
    state.manifestPatched = !state.manifestPristine;
    return state;
}

// The mod keeps nothing outside its own storage folder: the state of the
// install folder is recomputed when it is needed (CheckInstallFolder) and
// logged, and a running Windows 7 keyboard is found back through its image
// path, so no note is left in the registry for the user to clean up.

void LogFolderAccess(const wchar_t* who) {
    std::wstring dir = GetInstallDir();
    std::wstring osk = JoinPath(dir, L"osk.exe");

    DWORD dirAttributes = GetFileAttributesW(dir.c_str());
    DWORD oskAttributes = GetFileAttributesW(osk.c_str());

    Wh_Log(L"%s: folder probe for %s", who, dir.c_str());
    Wh_Log(L"  %%USERPROFILE%%=%s",
           ExpandVariables(L"%USERPROFILE%").c_str());
    Wh_Log(L"  %%LOCALAPPDATA%%=%s",
           ExpandVariables(L"%LOCALAPPDATA%").c_str());
    Wh_Log(L"  folder attributes=%s, osk.exe attributes=%s",
           dirAttributes == INVALID_FILE_ATTRIBUTES ? L"not visible" : L"visible",
           oskAttributes == INVALID_FILE_ATTRIBUTES ? L"not visible" : L"visible");
    if (dirAttributes == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"  folder is not visible to this process (error %lu)",
               GetLastError());
    }
}

struct InstallState {
    bool filesUsable = false;    // every file is the pinned Windows 7 build
    bool canStart = false;       // no uiAccess requirement left in osk.exe
    bool fullyPrepared = false;  // canStart and the delay-load name is patched
    std::wstring missing;
    std::wstring otherBuild;
};

InstallState CheckInstallFolder() {
    InstallState state;
    std::wstring dir = GetInstallDir();

    for (const auto& file : kFiles) {
        std::wstring path = JoinPath(dir, file.name);
        PeCheck check = CheckPinnedFile(path, file);
        if (check == PeCheck::Invalid) {
            if (!state.missing.empty()) {
                state.missing += L", ";
            }
            state.missing += file.name;
        } else if (check == PeCheck::ValidButOther) {
            if (!state.otherBuild.empty()) {
                state.otherBuild += L", ";
            }
            state.otherBuild += file.name;
        }
    }

    // A file that is present but is not the pinned build is not usable: the
    // keyboard is started by Explorer, which does not check anything.
    state.filesUsable = state.missing.empty() && state.otherBuild.empty();

    if (state.filesUsable) {
        OskState osk = InspectOskFile(JoinPath(dir, L"osk.exe"));
        state.canStart = !osk.manifestPristine;
        state.fullyPrepared = state.canStart && !osk.delayLoadPristine;
    }
    return state;
}

// Cached answer to "can we start the Windows 7 keyboard right now?". The cache
// is invalidated by the setup thread and when the settings change.
ULONGLONG g_startTick = 0;

std::atomic<int> g_readyCache{-1};
std::atomic<ULONGLONG> g_readyCheckTick{0};

bool ComputeReady() {
    InstallState state = CheckInstallFolder();
    if (!state.filesUsable) {
        return false;
    }
    // Ready means: every file is there and the local osk.exe no longer asks for
    // UIAccess, otherwise Windows refuses to start it from a user folder with
    // error 740. The delay-load edit is not required to start: the LoadLibrary
    // hooks cover the unpatched case.
    if (!state.canStart) {
        // The files are there but osk.exe is not prepared yet (UIAccess):
        // prepare it right now instead of waiting for the setup thread.
        std::wstring oskPath = JoinPath(GetInstallDir(), L"osk.exe");
        if (CheckPinnedFile(oskPath, kFiles[0]) == PeCheck::Exact) {
            PrepareOskFile(oskPath);
            state = CheckInstallFolder();
        }
    }
    if (!state.canStart) {
        Wh_Log(L"The Windows 7 osk.exe in the install folder still declares "
               L"uiAccess=\"true\": Windows refuses to start it (error 740) "
               L"until the setup prepares the file");
        return false;
    }
    return true;
}

bool IsReady() {
    int cached = g_readyCache.load(std::memory_order_relaxed);

    // A "not ready" answer is re-checked periodically, so that copying the
    // files into the folder by hand is picked up without restarting anything.
    bool stale = cached < 0 ||
                 (cached == 0 &&
                  GetTickCount64() - g_readyCheckTick.load(
                                         std::memory_order_relaxed) > 10000);
    if (stale) {
        g_readyCheckTick.store(GetTickCount64(), std::memory_order_relaxed);
        cached = ComputeReady() ? 1 : 0;
        g_readyCache.store(cached, std::memory_order_relaxed);
    }
    return cached != 0;
}

void InvalidateReadyCache() { g_readyCache.store(-1, std::memory_order_relaxed); }

// Renames a leftover dui70.dll (the previous layout / a hand-made copy) to
// dui71.dll. A local dui70.dll would be picked up by the system DUser.dll and
// break it, so this also protects against a stale folder.
void MigrateLegacyDui70() {
    std::wstring dir = GetInstallDir();
    std::wstring legacy = JoinPath(dir, L"dui70.dll");
    std::wstring target = JoinPath(dir, L"dui71.dll");

    if (!FileExists(legacy)) {
        return;
    }

    // Whatever it is, a dui70.dll in the folder of the keyboard is a hazard:
    // the system DUser.dll resolves that name and would load a foreign
    // implementation into itself. It is moved aside, never deleted.
    std::wstring disabled = legacy + L".unused";
    if (MoveFileExW(legacy.c_str(), disabled.c_str(),
                    MOVEFILE_REPLACE_EXISTING)) {
        Wh_Log(L"%s moved to %s: a dui70.dll next to the keyboard would be "
               L"loaded by the system DUser.dll. The Windows 7 DirectUI is used "
               L"as dui71.dll and is untouched",
               legacy.c_str(), disabled.c_str());
    } else {
        Wh_Log(L"Warning: %s cannot be moved aside (error %lu) and it must not "
               L"stay there: the system DUser.dll loads that name",
               legacy.c_str(), GetLastError());
        return;
    }

    // First run with the old layout: the file that has just been moved aside is
    // the Windows 7 DirectUI, so it becomes dui71.dll if that is missing.
    if (FileExists(target)) {
        return;
    }

    RemoteFile expected = kFiles[1];
    if (CheckPinnedFile(disabled, expected) != PeCheck::Exact) {
        if (FileExists(disabled)) {
            Wh_Log(L"%s is not the Windows 7 DUI70.dll, so it is not used as "
                   L"dui71.dll (it stays as %s)",
                   legacy.c_str(), disabled.c_str());
        }
        return;
    }

    if (MoveFileExW(disabled.c_str(), target.c_str(), 0)) {
        Wh_Log(L"%s renamed to %s (the Windows 7 DirectUI must load under a "
               L"name of its own)",
               disabled.c_str(), target.c_str());
    } else {
        Wh_Log(L"Could not rename %s to %s (error %lu)", disabled.c_str(),
               target.c_str(), GetLastError());
    }
}

void LogInstallFolderState() {
    std::wstring dir = GetInstallDir();
    InstallState state = CheckInstallFolder();
    Wh_Log(L"Install folder: %s", dir.c_str());
    Wh_Log(L"  all files are the pinned Windows 7 build: %d, osk.exe can "
           L"start: %d, fully prepared: %d",
           state.filesUsable ? 1 : 0, state.canStart ? 1 : 0,
           state.fullyPrepared ? 1 : 0);
    if (!state.missing.empty()) {
        Wh_Log(L"  missing or invalid: %s", state.missing.c_str());
    }
    if (!state.otherBuild.empty()) {
        Wh_Log(L"  present but not the pinned build: %s", state.otherBuild.c_str());
    }

    for (const auto& file : kFiles) {
        std::wstring path = JoinPath(dir, file.name);
        PeCheck check = CheckPinnedFile(path, file);
        Wh_Log(L"  %-12s %s (%s)", file.name,
               check == PeCheck::Exact     ? L"ok, verified"
               : check == PeCheck::ValidButOther ? L"present, not the pinned "
                                                   L"build"
                                                 : L"MISSING",
               file.role);
    }

    std::wstring oskPath = JoinPath(dir, L"osk.exe");
    if (FileExists(oskPath)) {
        OskState osk = InspectOskFile(oskPath);
        Wh_Log(L"  osk.exe: delay-load %s, manifest %s",
               osk.delayLoadPatched    ? L"DUI71.dll (prepared)"
               : osk.delayLoadPristine ? L"DUI70.dll (not prepared)"
                                      : L"unknown",
               osk.manifestPristine ? L"uiAccess=\"true\" (not prepared: "
                                      L"Windows will refuse to start it, error "
                                      L"740)"
                                    : L"no uiAccess requirement (prepared)");
    }
}

// ---------------------------------------------------------------------------
// Download
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// The download in progress, as handles the shutdown can close
//
// WinHTTP documents that closing a handle makes the calls that are blocked on
// it return, so Wh_ModBeforeUninit closes these handles to cancel a download
// instead of letting the shutdown wait for a network timeout. Each handle is
// closed exactly once: this list is the only place a handle is given up.
// ---------------------------------------------------------------------------

std::mutex g_httpLock;
std::vector<HINTERNET> g_httpHandles;

void TrackHttpHandle(HINTERNET handle) {
    if (!handle) {
        return;
    }
    std::lock_guard<std::mutex> guard(g_httpLock);
    g_httpHandles.push_back(handle);
}

// Hands the handle back to be closed, or nullptr when CancelHttp already
// closed it.
HINTERNET UntrackHttpHandle(HINTERNET handle) {
    if (!handle) {
        return nullptr;
    }
    std::lock_guard<std::mutex> guard(g_httpLock);
    for (size_t i = 0; i < g_httpHandles.size(); i++) {
        if (g_httpHandles[i] == handle) {
            g_httpHandles.erase(g_httpHandles.begin() + i);
            return handle;
        }
    }
    return nullptr;
}

void CancelHttp() {
    std::vector<HINTERNET> handles;
    {
        std::lock_guard<std::mutex> guard(g_httpLock);
        handles.swap(g_httpHandles);
    }
    for (HINTERNET handle : handles) {
        WinHttpCloseHandle(handle);
    }
}

bool DownloadToFile(const std::wstring& urlPath, const std::wstring& dest) {
    constexpr DWORD kMaxSize = 16 * 1024 * 1024;
    bool ok = false;

    // A user agent that names this mod: the mod is not Microsoft's symbol
    // tool and must not pretend to be it.
    HINTERNET session = WinHttpOpen(L"Win7OnScreenKeyboardRestorer/1.0.0 "
                                    L"(Windhawk mod; "
                                    L"https://github.com/babamohammed2022)",
                                    WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                    WINHTTP_NO_PROXY_NAME,
                                    WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET connection = nullptr;
    HINTERNET request = nullptr;
    HANDLE out = INVALID_HANDLE_VALUE;

    if (session) {
        TrackHttpHandle(session);
        WinHttpSetTimeouts(session, 15000, 15000, 30000, 60000);
        connection = WinHttpConnect(session, L"msdl.microsoft.com",
                                    INTERNET_DEFAULT_HTTPS_PORT, 0);
        TrackHttpHandle(connection);
    }
    if (connection) {
        request = WinHttpOpenRequest(connection, L"GET", urlPath.c_str(),
                                     nullptr, WINHTTP_NO_REFERER,
                                     WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     WINHTTP_FLAG_SECURE);
        TrackHttpHandle(request);
    }
    if (!request) {
        Wh_Log(L"WinHTTP open failed (%lu)", GetLastError());
    }

    if (request &&
        WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                           WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(request, nullptr)) {
        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        WinHttpQueryHeaders(request,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize,
                            WINHTTP_NO_HEADER_INDEX);

        if (status != 200) {
            Wh_Log(L"HTTP status %lu for %s", status, urlPath.c_str());
        } else {
            out = CreateFileW(dest.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }

        DWORD expectedLength = 0;
        DWORD lengthSize = sizeof(expectedLength);
        if (status == 200 &&
            !WinHttpQueryHeaders(request,
                                 WINHTTP_QUERY_CONTENT_LENGTH |
                                     WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX, &expectedLength,
                                 &lengthSize, WINHTTP_NO_HEADER_INDEX)) {
            expectedLength = 0;  // unknown: only the PE check applies
        }

        if (out != INVALID_HANDLE_VALUE) {
            DWORD total = 0;
            ok = true;
            for (;;) {
                if (g_stopEvent &&
                    WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                    ok = false;
                    break;
                }

                DWORD available = 0;
                if (!WinHttpQueryDataAvailable(request, &available)) {
                    ok = false;
                    break;
                }
                if (available == 0) {
                    break;
                }

                std::vector<BYTE> chunk(available);
                DWORD got = 0;
                if (!WinHttpReadData(request, chunk.data(), available, &got)) {
                    ok = false;
                    break;
                }

                total += got;
                DWORD written = 0;
                if (total > kMaxSize ||
                    !WriteFile(out, chunk.data(), got, &written, nullptr) ||
                    written != got) {
                    ok = false;
                    break;
                }
            }
            if (ok && expectedLength != 0 && total != expectedLength) {
                Wh_Log(L"Download truncated: %lu of %lu bytes for %s",
                       total, expectedLength, urlPath.c_str());
                ok = false;
            }
        }
    }

    if (out != INVALID_HANDLE_VALUE) {
        CloseHandle(out);
    }
    // Each handle is closed here unless CancelHttp got to it first.
    if (HINTERNET handle = UntrackHttpHandle(request)) {
        WinHttpCloseHandle(handle);
    }
    if (HINTERNET handle = UntrackHttpHandle(connection)) {
        WinHttpCloseHandle(handle);
    }
    if (HINTERNET handle = UntrackHttpHandle(session)) {
        WinHttpCloseHandle(handle);
    }
    return ok;
}

// Moves a file that is not the pinned build out of the way instead of letting
// the replacement overwrite it: the install folder may be a folder of the
// user's own (installFolder setting), and a file there, such as a copy from a
// Windows 7 SP1 install, belongs to the user. The name never clashes with an
// earlier backup, so a second pass cannot destroy the first one.
bool MoveAsideUserFile(const std::wstring& path, std::wstring* movedTo) {
    for (int n = 0; n < 100; n++) {
        std::wstring target = path + L".user-backup";
        if (n > 0) {
            target += L"." + std::to_wstring(n);
        }
        if (GetFileAttributesW(target.c_str()) != INVALID_FILE_ATTRIBUTES) {
            continue;
        }
        // No MOVEFILE_REPLACE_EXISTING: an existing backup is never touched.
        if (MoveFileExW(path.c_str(), target.c_str(), 0)) {
            if (movedTo) {
                *movedTo = target;
            }
            return true;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS &&
            GetLastError() != ERROR_FILE_EXISTS) {
            return false;
        }
    }
    return false;
}

bool EnsureFiles() {
    bool allPresent = true;
    std::wstring dir = GetInstallDir();
    int result = SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS &&
        result != ERROR_FILE_EXISTS) {
        Wh_Log(L"Could not create the install folder %s (error %d)",
               dir.c_str(), result);
    }

    for (const auto& file : kFiles) {
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            return false;
        }

        std::wstring dest = JoinPath(dir, file.name);
        PeCheck existing = CheckPinnedFile(dest, file);
        if (existing == PeCheck::Exact) {
            continue;
        }
        if (existing == PeCheck::ValidButOther) {
            Wh_Log(L"%s: present but not the pinned Windows 7 build (the "
                   L"digest does not match); it is moved aside as "
                   L"%s.user-backup and the pinned file replaces it",
                   file.name, file.name);
        }

        wchar_t urlPath[512];
        swprintf(urlPath, ARRAYSIZE(urlPath), L"/download/symbols/%s/%08X%x/%s",
                 file.urlName, file.timestamp, file.sizeOfImage, file.urlName);

        // The three steps are reported separately: "error 2" from the download
        // step and "error 2" from the move step mean completely different
        // things (no folder to write into in this process, or no folder to
        // move into).
        std::wstring temp = dest + L".tmp";
        bool downloaded = false;
        for (int attempt = 1; attempt <= 3 && !downloaded; attempt++) {
            if (attempt > 1 && g_stopEvent &&
                WaitForSingleObject(g_stopEvent, 2000 * attempt) ==
                    WAIT_OBJECT_0) {
                return false;
            }
            downloaded = DownloadToFile(urlPath, temp);
            if (!downloaded) {
                DWORD error = GetLastError();
                DeleteFileW(temp.c_str());
                Wh_Log(L"%s: download attempt %d of 3 failed (error %lu); "
                       L"the folder is %s",
                       file.name, attempt, error, dir.c_str());
            }
        }
        if (!downloaded) {
            allPresent = false;
            continue;
        }
        if (CheckPinnedFile(temp, file) != PeCheck::Exact) {
            DeleteFileW(temp.c_str());
            Wh_Log(L"%s: the downloaded file does not have the pinned SHA-256 "
                   L"digest and was discarded (the download may have been "
                   L"tampered with, or the symbol server serves another file)",
                   file.name);
            allPresent = false;
            continue;
        }
        // The download is verified: only now is the user's own file moved
        // aside (never before, so a failed download leaves it untouched). If
        // it cannot be moved, it is kept and nothing is replaced.
        if (existing == PeCheck::ValidButOther) {
            std::wstring backup;
            if (!MoveAsideUserFile(dest, &backup)) {
                Wh_Log(L"%s: the existing file cannot be moved aside (error "
                       L"%lu), so it is kept and not replaced",
                       file.name, GetLastError());
                DeleteFileW(temp.c_str());
                allPresent = false;
                continue;
            }
            Wh_Log(L"%s: the existing file was moved to %s", file.name,
                   backup.c_str());
        }
        if (!MoveFileExW(temp.c_str(), dest.c_str(),
                         MOVEFILE_REPLACE_EXISTING)) {
            Wh_Log(L"%s: the verified file cannot be moved to %s (error %lu)%s",
                   file.name, dest.c_str(), GetLastError(),
                   GetFileAttributesW(dir.c_str()) == INVALID_FILE_ATTRIBUTES
                       ? L"; this process cannot see the install folder at all"
                       : L"");
            DeleteFileW(temp.c_str());
            allPresent = false;
            continue;
        }
        Wh_Log(L"%s: downloaded and verified", file.name);
    }
    return allPresent;
}

// ---------------------------------------------------------------------------
// Setup thread
// ---------------------------------------------------------------------------

// One pass of the setup: download what is missing (with retries when
// `retries` is true), move a legacy dui70.dll aside and prepare osk.exe. Only
// one process at a time does the work (session-wide mutex).
void RunSetupSteps(bool retries) {
    HANDLE mutex = CreateMutexW(nullptr, FALSE, L"Local\\Win7OskRestorerSetup");
    bool owner = mutex && GetLastError() != ERROR_ALREADY_EXISTS;

    if (owner) {
        if (g_autoDownload) {
            // The network is often not up yet when Explorer starts, and a
            // single failed download used to leave the mod "not ready" until
            // the next restart: retry with growing pauses.
            static const DWORD kPauses[] = {0, 5000, 15000, 30000, 60000};
            for (DWORD pause : kPauses) {
                if (pause) {
                    if (!retries) {
                        break;
                    }
                    Wh_Log(L"Some Windows 7 files are still missing: next "
                           L"attempt in %lu s", pause / 1000);
                    if (g_stopEvent &&
                        WaitForSingleObject(g_stopEvent, pause) ==
                            WAIT_OBJECT_0) {
                        break;
                    }
                }
                if (EnsureFiles()) {
                    break;
                }
            }
        }
        MigrateLegacyDui70();

        std::wstring oskPath = JoinPath(GetInstallDir(), L"osk.exe");
        RemoteFile expected = kFiles[0];
        if (FileExists(oskPath)) {
            if (g_prepareFiles) {
                if (CheckPinnedFile(oskPath, expected) == PeCheck::Exact) {
                    PrepareOskFile(oskPath);
                } else {
                    Wh_Log(L"Preparation skipped: %s is not the pinned Windows 7 "
                           L"build; it is not used and the setup replaces it "
                           L"with the pinned file",
                           oskPath.c_str());
                }
            } else {
                OskState osk = InspectOskFile(oskPath);
                if (osk.manifestPristine) {
                    Wh_Log(L"Warning: %s still declares uiAccess=\"true\" and "
                           L"the preparation is disabled: Windows will refuse "
                           L"to start it (error 740)",
                           oskPath.c_str());
                }
            }
        }
    }

    if (mutex) {
        CloseHandle(mutex);
    }
}

std::atomic<bool> g_setupRunning{false};

DWORD WINAPI SetupThread(LPVOID) {
    RunSetupSteps(true);

    LogInstallFolderState();
    LogFolderAccess(L"Setup");
    InvalidateReadyCache();
    IsReady();  // recompute and cache for the hooks
    g_setupRunning = false;
    return 0;
}

void StartSetup() {
    if (g_setupRunning.exchange(true)) {
        return;  // a setup pass is already running in this process
    }
    Wh_Log(L"Install folder: %s (auto download: %d, prepare: %d)",
           GetInstallDir().c_str(), g_autoDownload ? 1 : 0,
           g_prepareFiles ? 1 : 0);
    SpawnWorker(SetupThread, nullptr, L"setup");
}

void LogSystemDependencies() {
    wchar_t systemDir[MAX_PATH];
    if (!GetSystemDirectoryW(systemDir, ARRAYSIZE(systemDir))) {
        return;
    }
    for (PCWSTR dll : {L"DUser.dll", L"WMsgAPI.dll", L"MSSWCH.dll"}) {
        std::wstring path = JoinPath(systemDir, dll);
        Wh_Log(L"Dependency %s: %s", dll,
               FileExists(path)
                   ? L"present in System32"
                   : L"not in System32 (a copy in the install folder is used "
                     L"for the static imports, otherwise osk.exe cannot start)");
    }
}

// ---------------------------------------------------------------------------
// Module diagnostics (Windows 7 osk.exe side)
// ---------------------------------------------------------------------------

std::wstring GetModuleVersionText(HMODULE module) {
    wchar_t path[32768];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return L"";
    }

    DWORD size = GetFileVersionInfoSizeW(path, nullptr);
    if (!size) {
        return L"(no version resource)";
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoW(path, 0, size, data.data())) {
        return L"(unreadable version resource)";
    }

    VS_FIXEDFILEINFO* info = nullptr;
    UINT infoSize = 0;
    if (!VerQueryValueW(data.data(), L"\\", (void**)&info, &infoSize) ||
        !info || infoSize < sizeof(VS_FIXEDFILEINFO)) {
        return L"(no fixed file info)";
    }

    wchar_t text[128];
    swprintf(text, ARRAYSIZE(text), L"%u.%u.%u.%u",
             HIWORD(info->dwFileVersionMS), LOWORD(info->dwFileVersionMS),
             HIWORD(info->dwFileVersionLS), LOWORD(info->dwFileVersionLS));
    return text;
}

void LogModuleInfo(PCWSTR name) {
    HMODULE module = GetModuleHandleW(name);
    if (!module) {
        Wh_Log(L"Module %s: not loaded", name);
        return;
    }

    wchar_t path[32768];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        path[0] = L'\0';
    }
    Wh_Log(L"Module %s: %s (version %s)", name, path,
           GetModuleVersionText(module).c_str());
}

// ---------------------------------------------------------------------------
// Is this the stock osk.exe / our own copy?
// ---------------------------------------------------------------------------

bool IsOurOskPath(PCWSTR path) {
    if (!path) {
        return false;
    }
    std::wstring local =
        NormalizePath(JoinPath(GetInstallDir(), L"osk.exe").c_str());
    return PathsEqual(NormalizePath(path), local);
}

bool IsStockOskPath(PCWSTR path, std::wstring* reason) {
    auto setReason = [reason](PCWSTR text) {
        if (reason) {
            *reason = text;
        }
    };

    if (!path || !*path) {
        setReason(L"empty target");
        return false;
    }

    std::wstring trimmed = path;
    while (!trimmed.empty() &&
           (trimmed.back() == L' ' || trimmed.back() == L'\t')) {
        trimmed.pop_back();
    }

    std::wstring base = BaseName(trimmed.c_str());
    if (!EqualsNoCase(base.c_str(), L"osk.exe") &&
        !EqualsNoCase(base.c_str(), L"osk")) {
        setReason(L"target is not osk");
        return false;
    }

    if (IsOurOskPath(trimmed.c_str())) {
        setReason(L"already the Windows 7 copy");
        return false;
    }

    std::wstring dir = DirectoryPart(trimmed.c_str());
    if (dir.empty()) {
        setReason(L"bare name, treated as the system osk.exe");
        return true;
    }

    std::wstring normalizedDir = NormalizePath(dir.c_str());

    wchar_t buffer[MAX_PATH];
    std::vector<std::wstring> systemDirs;
    if (GetSystemDirectoryW(buffer, ARRAYSIZE(buffer))) {
        systemDirs.push_back(NormalizePath(buffer));
    }
    if (GetSystemWow64DirectoryW(buffer, ARRAYSIZE(buffer))) {
        systemDirs.push_back(NormalizePath(buffer));
    }
    if (GetWindowsDirectoryW(buffer, ARRAYSIZE(buffer))) {
        std::wstring windowsDir = NormalizePath(buffer);
        systemDirs.push_back(windowsDir);
        systemDirs.push_back(
            NormalizePath(JoinPath(windowsDir, L"Sysnative").c_str()));
    }
    for (const std::wstring& candidate : systemDirs) {
        if (PathsEqual(normalizedDir, candidate)) {
            setReason(L"system folder");
            return true;
        }
    }

    setReason(L"osk.exe outside the system folders (left untouched)");
    return false;
}

bool IsOskImageName() {
    return EqualsNoCase(BaseName(GetProcessImagePath().c_str()).c_str(),
                        L"osk.exe");
}

bool IsOurOskProcess() {
    return IsOskImageName() && IsOurOskPath(GetProcessImagePath().c_str());
}

// ---------------------------------------------------------------------------
// Child process watchdog (diagnostics)
// ---------------------------------------------------------------------------

DWORD WINAPI ChildWatchThread(LPVOID parameter) {
    HANDLE child = (HANDLE)parameter;
    if (!child) {
        return 0;
    }

    // Ten seconds: long enough to catch a keyboard that dies at startup, short
    // enough not to hold the thread for a keyboard that is simply in use. The
    // worker is also joined at uninstall time through g_stopEvent.
    HANDLE handles[2] = {child, g_stopEvent};
    DWORD wait = WaitForMultipleObjects(g_stopEvent ? 2 : 1, handles, FALSE,
                                        10000);

    if (wait == WAIT_OBJECT_0) {
        DWORD exitCode = 0;
        if (GetExitCodeProcess(child, &exitCode) && exitCode != STILL_ACTIVE) {
            if (exitCode == 0) {
                Wh_Log(L"The Windows 7 keyboard (osk.exe) exited normally");
            } else if (exitCode == 1) {
                // Observed: the Windows 7 keyboard quits with 1 when its window
                // is closed by the user, so this is its normal quit path.
                Wh_Log(L"The Windows 7 keyboard (osk.exe) exited with 0x%08lX; "
                       L"this is the code it returns when its window is closed",
                       exitCode);
            } else {
                Wh_Log(L"The Windows 7 keyboard (osk.exe) exited early with %s",
                       DescribeExitCode(exitCode).c_str());
            }
        }
    } else if (wait == WAIT_TIMEOUT) {
        Wh_Log(L"The Windows 7 keyboard (osk.exe) is still running after 10 "
               L"seconds (good sign)");
    }

    CloseHandle(child);
    return 0;
}

// Watches a just-created osk.exe without stealing the handle the caller owns:
// the handle is duplicated, the caller keeps the original.
void WatchChild(HANDLE child) {
    if (!g_diagnostics || !child) {
        return;
    }

    HANDLE duplicate = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), child, GetCurrentProcess(),
                         &duplicate,
                         SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                         FALSE, 0)) {
        return;
    }

    if (!SpawnWorker(ChildWatchThread, duplicate, L"watchdog")) {
        CloseHandle(duplicate);
    }
}

std::wstring GetProcessImagePathById(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }
    wchar_t buffer[32768];
    DWORD size = ARRAYSIZE(buffer);
    std::wstring path;
    if (QueryFullProcessImageNameW(process, 0, buffer, &size)) {
        path.assign(buffer, size);
    }
    CloseHandle(process);
    return path;
}

void FindRunningOsk(bool* stockSeen, bool* ourSeen) {
    *stockSeen = false;
    *ourSeen = false;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    std::wstring ourExe =
        NormalizePath(JoinPath(GetInstallDir(), L"osk.exe").c_str());

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (!EqualsNoCase(entry.szExeFile, L"osk.exe") ||
                entry.th32ProcessID == GetCurrentProcessId()) {
                continue;
            }

            std::wstring path = GetProcessImagePathById(entry.th32ProcessID);
            bool isOurs = !path.empty() &&
                          PathsEqual(NormalizePath(path.c_str()), ourExe);
            if (isOurs) {
                *ourSeen = true;
            } else {
                *stockSeen = true;
            }

            Wh_Log(L"%s osk.exe already running: pid %lu, %s",
                   isOurs ? L"Windows 7" : L"System", entry.th32ProcessID,
                   path.empty() ? L"(path unknown)" : path.c_str());
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
}

// ---------------------------------------------------------------------------
// Launcher side: redirect osk.exe launches
// ---------------------------------------------------------------------------

bool BuildRedirectedCommandLine(PCWSTR commandLine, size_t tokenEnd,
                                const std::wstring& localExe,
                                std::wstring& result) {
    result = L"\"";
    result += localExe;
    result += L"\"";
    if (commandLine && commandLine[tokenEnd]) {
        result += (commandLine + tokenEnd);
    }
    return true;
}

void ParseFirstToken(PCWSTR command, std::wstring& token, size_t& tokenEnd) {
    token.clear();
    tokenEnd = 0;
    if (!command) {
        return;
    }

    size_t i = 0;
    while (command[i] == L' ' || command[i] == L'\t') {
        i++;
    }

    if (command[i] == L'"') {
        i++;
        size_t start = i;
        while (command[i] && command[i] != L'"') {
            i++;
        }
        token.assign(command + start, i - start);
        if (command[i] == L'"') {
            i++;
        }
    } else {
        size_t start = i;
        while (command[i] && command[i] != L' ' && command[i] != L'\t') {
            i++;
        }
        token.assign(command + start, i - start);
    }
    tokenEnd = i;
}

struct LaunchDecision {
    bool redirect = false;
    std::wstring newApplication;
    std::wstring newCommandLine;
};

LaunchDecision DecideLaunch(PCWSTR api, LPCWSTR applicationName,
                            LPCWSTR commandLine, void* returnAddress) {
    LaunchDecision decision;

    bool mentionsOsk = (applicationName &&
                        StrStrIW(applicationName, L"osk") != nullptr) ||
                       (commandLine && StrStrIW(commandLine, L"osk") != nullptr);
    if (!mentionsOsk) {
        return decision;
    }

    std::wstring reason;
    bool targetIsOsk = false;
    bool viaCommandLine = false;
    std::wstring token;
    size_t tokenEnd = 0;

    if (applicationName && *applicationName) {
        targetIsOsk = IsStockOskPath(applicationName, &reason);
    } else if (commandLine && *commandLine) {
        ParseFirstToken(commandLine, token, tokenEnd);
        targetIsOsk = IsStockOskPath(token.c_str(), &reason);
        viaCommandLine = true;
    } else {
        reason = L"no target at all";
    }

    Wh_Log(L"%s: %s (application=\"%s\", command line=\"%s\")", api,
           reason.c_str(), applicationName ? applicationName : L"(null)",
           commandLine ? commandLine : L"(null)");
    if (g_diagnostics) {
        Wh_Log(L"    called from %s, thread %lu",
               DescribeCallerModule(returnAddress).c_str(),
               GetCurrentThreadId());
    }

    if (!targetIsOsk || !g_redirectLaunches) {
        return decision;
    }

    if (!IsReady()) {
        InstallState state = CheckInstallFolder();
        Wh_Log(L"Not redirecting: the Windows 7 keyboard is not ready in %s "
               L"(missing: %s; can start: %d). The system keyboard is used; the "
               L"mod's setup thread completes the folder in the background",
               GetInstallDir().c_str(),
               state.missing.empty() ? L"nothing" : state.missing.c_str(),
               state.canStart ? 1 : 0);
        StartSetup();  // self-healing: try to complete the files again
        return decision;
    }

    std::wstring localExe = JoinPath(GetInstallDir(), L"osk.exe");
    if (viaCommandLine) {
        BuildRedirectedCommandLine(commandLine, tokenEnd, localExe,
                                   decision.newCommandLine);
        Wh_Log(L"Redirecting (command line): %s",
               decision.newCommandLine.c_str());
    } else {
        decision.newApplication = localExe;

        std::wstring commandToken;
        size_t commandTokenEnd = 0;
        ParseFirstToken(commandLine, commandToken, commandTokenEnd);
        if (commandTokenEnd > 0 &&
            IsStockOskPath(commandToken.c_str(), nullptr)) {
            BuildRedirectedCommandLine(commandLine, commandTokenEnd, localExe,
                                       decision.newCommandLine);
            Wh_Log(L"Redirecting (application name + command line): %s",
                   decision.newCommandLine.c_str());
        } else {
            Wh_Log(L"Redirecting (application name): %s", localExe.c_str());
        }
    }

    decision.redirect = true;
    return decision;
}

// The hooks call this instead of DecideLaunch. The decision builds several
// strings, so it can throw (std::bad_alloc); an exception must not leave a hook
// that runs inside the host process. On failure the launch is simply not
// redirected, so the behaviour is the one of Windows without the mod. Only the
// code of the mod is inside the try: the original function is called by the
// hook afterwards, outside it.
LaunchDecision SafeDecideLaunch(PCWSTR api, LPCWSTR applicationName,
                                LPCWSTR commandLine, void* returnAddress) {
    try {
        return DecideLaunch(api, applicationName, commandLine, returnAddress);
    } catch (...) {
        Wh_Log(L"DecideLaunch failed with a C++ exception; the launch is not "
               L"redirected");
    }
    return LaunchDecision{};
}

void LogRedirectFailure(DWORD error) {
    std::wstring explanation = DescribeLaunchError(error);
    if (!explanation.empty()) {
        Wh_Log(L"Redirect failed: error %s", explanation.c_str());
    } else {
        Wh_Log(L"Redirect failed: error %lu", error);
    }
    Wh_Log(L"Falling back to the original launch (the system keyboard is used "
           L"and, if the catch-all is enabled, the Windows 7 one takes over)");
}

using CreateProcessInternalW_t = BOOL(WINAPI*)(
    HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL,
    DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
CreateProcessInternalW_t CreateProcessInternalW_Original;

BOOL WINAPI CreateProcessInternalW_Hook(
    HANDLE token, LPCWSTR applicationName, LPWSTR commandLine,
    LPSECURITY_ATTRIBUTES processAttributes,
    LPSECURITY_ATTRIBUTES threadAttributes, BOOL inheritHandles,
    DWORD creationFlags, LPVOID environment, LPCWSTR currentDirectory,
    LPSTARTUPINFOW startupInfo, LPPROCESS_INFORMATION processInformation,
    PHANDLE restrictedToken) {
    void* caller = MOD_CALLER_ADDRESS();
    LaunchDecision decision =
        SafeDecideLaunch(L"CreateProcess", applicationName, commandLine, caller);

    if (!decision.redirect) {
        return CreateProcessInternalW_Original(
            token, applicationName, commandLine, processAttributes,
            threadAttributes, inheritHandles, creationFlags, environment,
            currentDirectory, startupInfo, processInformation, restrictedToken);
    }

    LPCWSTR newApplication = decision.newApplication.empty()
                                 ? applicationName
                                 : decision.newApplication.c_str();
    LPWSTR newCommandLine = decision.newCommandLine.empty()
                                ? commandLine
                                : decision.newCommandLine.data();

    SetLastError(ERROR_SUCCESS);
    BOOL result = CreateProcessInternalW_Original(
        token, newApplication, newCommandLine, processAttributes,
        threadAttributes, inheritHandles, creationFlags, environment,
        currentDirectory, startupInfo, processInformation, restrictedToken);

    if (result) {
        Wh_Log(L"Redirect succeeded, osk.exe pid %lu",
               processInformation ? processInformation->dwProcessId : 0);
        if (processInformation) {
            WatchChild(processInformation->hProcess);
        }
        return result;
    }

    DWORD error = GetLastError();
    LogRedirectFailure(error);

    // Never worse than the stock behaviour: run the original request.
    SetLastError(ERROR_SUCCESS);
    result = CreateProcessInternalW_Original(
        token, applicationName, commandLine, processAttributes,
        threadAttributes, inheritHandles, creationFlags, environment,
        currentDirectory, startupInfo, processInformation, restrictedToken);
    if (!result) {
        SetLastError(error);
    }
    return result;
}

using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                                       LPSECURITY_ATTRIBUTES, BOOL, DWORD,
                                       LPVOID, LPCWSTR, LPSTARTUPINFOW,
                                       LPPROCESS_INFORMATION);
CreateProcessW_t CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR applicationName, LPWSTR commandLine,
                                LPSECURITY_ATTRIBUTES processAttributes,
                                LPSECURITY_ATTRIBUTES threadAttributes,
                                BOOL inheritHandles, DWORD creationFlags,
                                LPVOID environment, LPCWSTR currentDirectory,
                                LPSTARTUPINFOW startupInfo,
                                LPPROCESS_INFORMATION processInformation) {
    void* caller = MOD_CALLER_ADDRESS();
    LaunchDecision decision =
        SafeDecideLaunch(L"CreateProcessW", applicationName, commandLine, caller);

    if (!decision.redirect) {
        return CreateProcessW_Original(
            applicationName, commandLine, processAttributes, threadAttributes,
            inheritHandles, creationFlags, environment, currentDirectory,
            startupInfo, processInformation);
    }

    SetLastError(ERROR_SUCCESS);
    BOOL result = CreateProcessW_Original(
        decision.newApplication.empty() ? applicationName
                                        : decision.newApplication.c_str(),
        decision.newCommandLine.empty() ? commandLine
                                        : decision.newCommandLine.data(),
        processAttributes, threadAttributes, inheritHandles, creationFlags,
        environment, currentDirectory, startupInfo, processInformation);

    if (result) {
        Wh_Log(L"Redirect succeeded, osk.exe pid %lu",
               processInformation ? processInformation->dwProcessId : 0);
        if (processInformation) {
            WatchChild(processInformation->hProcess);
        }
        return result;
    }

    DWORD error = GetLastError();
    LogRedirectFailure(error);

    SetLastError(ERROR_SUCCESS);
    result = CreateProcessW_Original(
        applicationName, commandLine, processAttributes, threadAttributes,
        inheritHandles, creationFlags, environment, currentDirectory,
        startupInfo, processInformation);
    if (!result) {
        SetLastError(error);
    }
    return result;
}

RateLimit g_idListLogLimit;

// Resolves a shell item (PIDL) to a path, when it points at the file system.
std::wstring ResolveIdListTarget(void* pidl) {
    using SHGetPathFromIDListW_t = BOOL(WINAPI*)(const void*, LPWSTR);
    static SHGetPathFromIDListW_t getPath = []() -> SHGetPathFromIDListW_t {
        HMODULE shell32 = GetModuleHandleW(L"shell32.dll");
        if (!shell32) {
            shell32 = LoadLibraryW(L"shell32.dll");
        }
        return shell32 ? reinterpret_cast<SHGetPathFromIDListW_t>(
                             GetProcAddress(shell32, "SHGetPathFromIDListW"))
                       : nullptr;
    }();

    if (!getPath || !pidl) {
        return L"";
    }
    wchar_t path[32768];
    path[0] = L'\0';
    return getPath(pidl, path) ? std::wstring(path) : std::wstring();
}

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
ShellExecuteExW_t ShellExecuteExW_Original;

BOOL WINAPI ShellExecuteExW_Hook(SHELLEXECUTEINFOW* info) {
    if (!info || info->cbSize < sizeof(SHELLEXECUTEINFOW)) {
        return ShellExecuteExW_Original(info);
    }

    void* caller = MOD_CALLER_ADDRESS();

    if (info->fMask & (SEE_MASK_IDLIST | SEE_MASK_INVOKEIDLIST)) {
        if (g_diagnostics && g_idListLogLimit.Allow(2000)) {
            std::wstring item = ResolveIdListTarget(info->lpIDList);
            Wh_Log(L"ShellExecuteEx on a shell item (mask 0x%lX, item: %s, "
                   L"verb: %s), called from %s; the hand-off covers this launch",
                   (unsigned long)info->fMask,
                   item.empty() ? L"not a file system item" : item.c_str(),
                   info->lpVerb ? info->lpVerb : L"(default)",
                   DescribeCallerModule(caller).c_str());
        }
        return ShellExecuteExW_Original(info);
    }

    LaunchDecision decision =
        SafeDecideLaunch(L"ShellExecuteEx", info->lpFile, nullptr, caller);
    if (!decision.redirect) {
        return ShellExecuteExW_Original(info);
    }

    std::wstring localExe = JoinPath(GetInstallDir(), L"osk.exe");
    SHELLEXECUTEINFOW copy = *info;
    copy.lpFile = localExe.c_str();
    copy.lpParameters = info->lpParameters;
    copy.lpDirectory = info->lpDirectory;

    BOOL result = ShellExecuteExW_Original(&copy);
    if (result) {
        info->hInstApp = copy.hInstApp;
        info->hProcess = copy.hProcess;
        DWORD childPid = copy.hProcess ? GetProcessId(copy.hProcess) : 0;
        Wh_Log(L"Redirect succeeded, osk.exe pid %lu", childPid);
        if (copy.hProcess && (copy.fMask & SEE_MASK_NOCLOSEPROCESS)) {
            WatchChild(copy.hProcess);
        }
        return result;
    }

    DWORD error = GetLastError();
    LogRedirectFailure(error);
    SetLastError(ERROR_SUCCESS);
    result = ShellExecuteExW_Original(info);
    if (!result) {
        SetLastError(error);
    }
    return result;
}

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR,
                                           LPCWSTR, INT);
ShellExecuteW_t ShellExecuteW_Original;

HINSTANCE WINAPI ShellExecuteW_Hook(HWND window, LPCWSTR operation,
                                    LPCWSTR file, LPCWSTR parameters,
                                    LPCWSTR directory, INT showCommand) {
    void* caller = MOD_CALLER_ADDRESS();
    LaunchDecision decision = SafeDecideLaunch(L"ShellExecute", file, nullptr, caller);

    if (!decision.redirect) {
        return ShellExecuteW_Original(window, operation, file, parameters,
                                      directory, showCommand);
    }

    std::wstring localExe = JoinPath(GetInstallDir(), L"osk.exe");
    HINSTANCE result = ShellExecuteW_Original(
        window, operation, localExe.c_str(), parameters, directory, showCommand);
    if ((ULONG_PTR)result > 32) {
        Wh_Log(L"Redirected ShellExecute to %s (ok)", localExe.c_str());
        return result;
    }

    Wh_Log(L"Redirected ShellExecute failed (code %llu), falling back to the "
           L"original target",
           (unsigned long long)(ULONG_PTR)result);
    return ShellExecuteW_Original(window, operation, file, parameters, directory,
                                  showCommand);
}

bool HookExport(PCWSTR module, PCSTR name, void* hook, void** original) {
    HMODULE handle = GetModuleHandleW(module);
    if (!handle) {
        handle = LoadLibraryW(module);
    }
    if (!handle) {
        Wh_Log(L"Hook %S: module %s is not available", name, module);
        return false;
    }

    FARPROC target = GetProcAddress(handle, name);
    if (!target) {
        Wh_Log(L"Hook %S: export not found in %s", name, module);
        return false;
    }

    bool ok = Wh_SetFunctionHook(reinterpret_cast<void*>(target), hook, original);
    Wh_Log(L"Hook %s!%S: %s", module, name, ok ? L"installed" : L"FAILED");
    return ok;
}

// Several APIs moved from kernel32.dll to kernelbase.dll.
bool HookExportWithFallback(PCWSTR primary, PCWSTR secondary, PCSTR name,
                            void* hook, void** original) {
    if (HookExport(primary, name, hook, original)) {
        return true;
    }
    return secondary && HookExport(secondary, name, hook, original);
}

void HookLauncherProcess() {
    if (!HookExport(L"kernelbase.dll", "CreateProcessInternalW",
                    reinterpret_cast<void*>(CreateProcessInternalW_Hook),
                    reinterpret_cast<void**>(
                        &CreateProcessInternalW_Original))) {
        HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll",
                               "CreateProcessW",
                               reinterpret_cast<void*>(CreateProcessW_Hook),
                               reinterpret_cast<void**>(&CreateProcessW_Original));
    }

    HookExport(L"shell32.dll", "ShellExecuteExW",
               reinterpret_cast<void*>(ShellExecuteExW_Hook),
               reinterpret_cast<void**>(&ShellExecuteExW_Original));
    HookExport(L"shell32.dll", "ShellExecuteW",
               reinterpret_cast<void*>(ShellExecuteW_Hook),
               reinterpret_cast<void**>(&ShellExecuteW_Original));
}

// ---------------------------------------------------------------------------
// Stock osk.exe side: hand-off to the Windows 7 copy
// ---------------------------------------------------------------------------

std::atomic<bool> g_handoffAttempted{false};

bool IsSystemToken() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }

    BYTE buffer[512];
    DWORD size = 0;
    bool isSystem = false;
    if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size)) {
        auto user = reinterpret_cast<TOKEN_USER*>(buffer);
        BYTE systemSid[SECURITY_MAX_SID_SIZE];
        DWORD sidSize = sizeof(systemSid);
        if (CreateWellKnownSid(WinLocalSystemSid, nullptr, systemSid, &sidSize)) {
            isSystem = EqualSid(user->User.Sid, systemSid) != FALSE;
        }
    }
    CloseHandle(token);
    return isSystem;
}

std::wstring GetCurrentDesktopName() {
    HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
    if (!desktop) {
        return L"";
    }
    wchar_t name[256];
    DWORD needed = 0;
    if (!GetUserObjectInformationW(desktop, UOI_NAME, name, sizeof(name),
                                   &needed)) {
        return L"";
    }
    return name;
}

bool ShouldSkipHandoff(std::wstring& why) {
    std::wstring desktop = GetCurrentDesktopName();
    if (!desktop.empty() && _wcsicmp(desktop.c_str(), L"Default") != 0) {
        why = L"the process runs on the \"" + desktop + L"\" desktop";
        return true;
    }

    if (IsSystemToken()) {
        why = L"the process runs as SYSTEM";
        return true;
    }

    DWORD session = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &session)) {
        DWORD consoleSession = WTSGetActiveConsoleSessionId();
        if (consoleSession != 0xFFFFFFFF && session != consoleSession) {
            wchar_t text[128];
            swprintf(text, ARRAYSIZE(text),
                     L"the process runs in session %lu, the console session is "
                     L"%lu",
                     session, consoleSession);
            why = text;
            return true;
        }
    }
    return false;
}

BOOL CALLBACK HideWindowsEnumProc(HWND window, LPARAM parameter) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid == (DWORD)parameter) {
        ShowWindow(window, SW_HIDE);
    }
    return TRUE;
}

void HideOwnWindowsBriefly(int rounds) {
    DWORD pid = GetCurrentProcessId();
    for (int i = 0; i < rounds; i++) {
        EnumWindows(HideWindowsEnumProc, (LPARAM)pid);
        if (g_stopEvent &&
            WaitForSingleObject(g_stopEvent, 20) == WAIT_OBJECT_0) {
            break;
        }
    }
}

DWORD WINAPI HandoffThread(LPVOID) {
    std::wstring localExe = JoinPath(GetInstallDir(), L"osk.exe");
    if (!FileExists(localExe)) {
        Wh_Log(L"Hand-off skipped: %s does not exist", localExe.c_str());
        return 0;
    }

    std::wstring why;
    if (ShouldSkipHandoff(why)) {
        Wh_Log(L"Hand-off skipped: %s", why.c_str());
        return 0;
    }

    if (!IsReady()) {
        // The launcher side did not finish the setup (download failed or still
        // running): complete it from here, then wait a little for another
        // process that may be doing it.
        Wh_Log(L"Hand-off: the Windows 7 files are not ready, completing the "
               L"setup from the system osk.exe");
        RunSetupSteps(false);
        InvalidateReadyCache();
        for (int i = 0; i < 40 && !IsReady(); i++) {
            if (g_stopEvent &&
                WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
                return 0;
            }
            InvalidateReadyCache();
        }
    }

    if (!IsReady()) {
        InstallState state = CheckInstallFolder();
        Wh_Log(L"Hand-off skipped: the Windows 7 files are not ready yet "
               L"(missing: %s, can start: %d); the system keyboard stays",
               state.missing.empty() ? L"nothing" : state.missing.c_str(),
               state.canStart ? 1 : 0);
        return 0;
    }

    // A Windows 7 keyboard is already there: this is what a second
    // Win+Ctrl+O press looks like. Close the system keyboard that was just
    // started instead of opening a duplicate.
    bool stockSeen = false;
    bool ourSeen = false;
    FindRunningOsk(&stockSeen, &ourSeen);
    if (ourSeen) {
        Wh_Log(L"Hand-off: the Windows 7 keyboard is already running, closing "
               L"this system osk.exe instead of opening a second keyboard");
        HideOwnWindowsBriefly(10);
        TerminateProcess(GetCurrentProcess(), 0);
        return 0;
    }

    std::wstring arguments;
    size_t tokenEnd = 0;
    std::wstring token;
    ParseFirstToken(GetCommandLineW(), token, tokenEnd);
    if (GetCommandLineW() && GetCommandLineW()[tokenEnd]) {
        arguments = GetCommandLineW() + tokenEnd;
    }

    std::wstring commandLine = L"\"";
    commandLine += localExe;
    commandLine += L"\"";
    commandLine += arguments;

    std::wstring directory = GetInstallDir();
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo{};

    Wh_Log(L"Hand-off: starting \"%s\"", commandLine.c_str());
    bool started = CreateProcessW(localExe.c_str(), commandLine.data(), nullptr,
                                  nullptr, FALSE, 0, nullptr, directory.c_str(),
                                  &startupInfo, &processInfo) != FALSE;
    if (!started) {
        DWORD error = GetLastError();
        std::wstring explanation = DescribeLaunchError(error);
        Wh_Log(L"Hand-off: CreateProcess failed with %s",
               explanation.empty() ? L"a generic error" : explanation.c_str());
        if (explanation.empty()) {
            Wh_Log(L"Hand-off: CreateProcess error %lu", error);
        }

        // Second attempt through the shell, which can elevate when it has to.
        SHELLEXECUTEINFOW execInfo{};
        execInfo.cbSize = sizeof(execInfo);
        execInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
        execInfo.lpFile = localExe.c_str();
        execInfo.lpParameters = arguments.empty() ? nullptr : arguments.c_str();
        execInfo.lpDirectory = directory.c_str();
        execInfo.nShow = SW_SHOWNORMAL;

        if (ShellExecuteExW(&execInfo) && execInfo.hProcess) {
            Wh_Log(L"Hand-off succeeded through the shell, Windows 7 osk.exe "
                   L"pid %lu; closing pid %lu",
                   GetProcessId(execInfo.hProcess), GetCurrentProcessId());
            WatchChild(execInfo.hProcess);
            HideOwnWindowsBriefly(30);
            TerminateProcess(GetCurrentProcess(), 0);
            return 0;
        }

        Wh_Log(L"Hand-off failed, the system keyboard is kept");
        return 0;
    }

    Wh_Log(L"Hand-off succeeded, Windows 7 osk.exe pid %lu; closing pid %lu",
           processInfo.dwProcessId, GetCurrentProcessId());
    if (processInfo.hThread) {
        CloseHandle(processInfo.hThread);
    }
    WatchChild(processInfo.hProcess);

    HideOwnWindowsBriefly(30);
    TerminateProcess(GetCurrentProcess(), 0);
    return 0;
}

void RunStockOskHandoff() {
    if (!g_handoff) {
        Wh_Log(L"Running as the system osk.exe; the hand-off is disabled in "
               L"the settings");
        return;
    }
    if (IsOurOskProcess()) {
        return;
    }
    if (g_handoffAttempted.exchange(true)) {
        return;
    }

    Wh_Log(L"Running as the system osk.exe (%s); handing off to the Windows 7 "
           L"copy",
           GetProcessImagePath().c_str());
    SpawnWorker(HandoffThread, nullptr, L"hand-off");
}

// ---------------------------------------------------------------------------
// Windows 7 osk.exe side: DLL redirection
// ---------------------------------------------------------------------------

bool CallerIsThisModule(void* returnAddress) {
    if (!returnAddress || !g_exeModule) {
        return false;
    }
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(returnAddress),
                            &module)) {
        return false;
    }
    return module == g_exeModule;
}

RateLimit g_loadFailLimit;
RateLimit g_loadRedirectLimit;

std::wstring GetLocalDllPath(PCWSTR name) {
    std::wstring installDir = GetInstallDir();
    std::wstring base = BaseName(name);

    if (EqualsNoCase(base.c_str(), L"tabskb.dll")) {
        return JoinPath(installDir, L"tabskb.dll");
    }
    if (EqualsNoCase(base.c_str(), L"dui70.dll")) {
        // The Windows 7 keyboard delay-loads DUI70.dll; the system one is
        // already loaded in this process through the system DUser.dll, so the
        // local copy lives under a different name.
        return JoinPath(installDir, L"dui71.dll");
    }
    if (EqualsNoCase(base.c_str(), L"dui71.dll")) {
        return JoinPath(installDir, L"dui71.dll");
    }
    return L"";
}

// Returns true and fills |redirected| when the load must be served from the
// install folder instead. |callerAddress| is the return address of the
// LoadLibrary call, used to serve the DUI70 request only when it comes from
// the Windows 7 keyboard itself.
bool RedirectDll(PCWSTR name, void* callerAddress, DWORD flags,
                 std::wstring& redirected) {
    if (!name || !*name) {
        return false;
    }

    std::wstring local = GetLocalDllPath(name);
    if (local.empty() || !FileExists(local)) {
        return false;
    }

    std::wstring requested = NormalizePath(name);
    if (PathsEqual(requested, NormalizePath(local.c_str()))) {
        return false;  // already the local file
    }

    std::wstring base = BaseName(name);
    if (EqualsNoCase(base.c_str(), L"dui70.dll") ||
        EqualsNoCase(base.c_str(), L"dui71.dll")) {
        // Only the Windows 7 keyboard may get the Windows 7 DirectUI: the
        // system DUser.dll must keep the system one.
        if (!CallerIsThisModule(callerAddress)) {
            return false;
        }
        // Never override an explicit LOAD_WITH_ALTERED_SEARCH_PATH or an image
        // section request.
        if (flags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE |
                     LOAD_WITH_ALTERED_SEARCH_PATH)) {
            return false;
        }
    }

    (void)name;
    redirected = local;
    return true;
}

// The LoadLibrary hooks call this instead of RedirectDll, for the same reason
// as SafeDecideLaunch: on an exception the load is simply not redirected.
bool SafeRedirectDll(PCWSTR name, void* callerAddress, DWORD flags,
                     std::wstring& redirected) {
    try {
        return RedirectDll(name, callerAddress, flags, redirected);
    } catch (...) {
        Wh_Log(L"RedirectDll failed with a C++ exception; the load is not "
               L"redirected");
    }
    return false;
}

using LoadLibraryW_t = HMODULE(WINAPI*)(LPCWSTR);
LoadLibraryW_t LoadLibraryW_Original;

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR name) {
    void* caller = MOD_CALLER_ADDRESS();
    std::wstring redirected;
    if (SafeRedirectDll(name, caller, 0, redirected)) {
        HMODULE module = LoadLibraryW_Original(redirected.c_str());
        if (g_loadRedirectLimit.Allow(200)) {
            Wh_Log(L"LoadLibraryW: %s -> %s (%s)", name, redirected.c_str(),
                   module ? L"ok" : L"failed");
        }
        if (!module) {
            Wh_Log(L"LoadLibraryW of the redirected %s failed (error %lu)",
                   redirected.c_str(), GetLastError());
        }
        return module;
    }

    HMODULE module = LoadLibraryW_Original(name);
    if (!module && name && *name && g_loadFailLimit.Allow(200)) {
        Wh_Log(L"LoadLibraryW failed: %s (error %lu)", name, GetLastError());
    }
    return module;
}

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    void* caller = MOD_CALLER_ADDRESS();
    std::wstring redirected;
    if (SafeRedirectDll(name, caller, flags, redirected)) {
        HMODULE module = LoadLibraryExW_Original(redirected.c_str(), nullptr, flags);
        if (g_loadRedirectLimit.Allow(200)) {
            Wh_Log(L"LoadLibraryExW: %s -> %s (%s)", name, redirected.c_str(),
                   module ? L"ok" : L"failed");
        }
        if (!module) {
            Wh_Log(L"LoadLibraryExW of the redirected %s failed (error %lu)",
                   redirected.c_str(), GetLastError());
        }
        return module;
    }

    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (!module && name && *name && g_loadFailLimit.Allow(200)) {
        Wh_Log(L"LoadLibraryExW failed: %s (error %lu)", name, GetLastError());
    }
    return module;
}

// The MSVC delay-load helper calls LoadLibraryA, so an A variant hook is what
// actually sees the Windows 7 DirectUI being loaded (and lets it be redirected
// to dui71.dll when needed).
// ANSI -> wide. The buffer is sized exactly: the terminating NUL of the input
// is not copied (the length is passed explicitly), so nothing can be written
// past the end of the string.
bool WideStringFromAnsi(PCSTR text, std::wstring* wide) {
    if (!text || !*text) {
        return false;
    }

    int characters =
        MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
    if (characters <= 1) {
        return false;
    }

    wide->resize((size_t)characters - 1);
    if (MultiByteToWideChar(CP_ACP, 0, text, (int)strlen(text), wide->data(),
                            characters - 1) != characters - 1) {
        wide->clear();
        return false;
    }
    return true;
}

std::string NarrowString(const std::wstring& text) {
    std::string narrow;
    if (text.empty()) {
        return narrow;
    }
    int needed = WideCharToMultiByte(CP_ACP, 0, text.c_str(), (int)text.size(),
                                     nullptr, 0, nullptr, nullptr);
    if (needed <= 0) {
        return narrow;
    }
    narrow.resize((size_t)needed);
    WideCharToMultiByte(CP_ACP, 0, text.c_str(), (int)text.size(),
                        narrow.data(), needed, nullptr, nullptr);
    return narrow;
}

using LoadLibraryA_t = HMODULE(WINAPI*)(LPCSTR);
LoadLibraryA_t LoadLibraryA_Original;

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR name) {
    void* caller = MOD_CALLER_ADDRESS();
    std::wstring wide;

    WideStringFromAnsi(name, &wide);

    std::wstring redirected;
    if (!wide.empty() && SafeRedirectDll(wide.c_str(), caller, 0, redirected)) {
        std::string narrow = NarrowString(redirected);
        HMODULE module = LoadLibraryA_Original(narrow.c_str());
        Wh_Log(L"LoadLibraryA: %s -> %s (%s)", wide.c_str(), redirected.c_str(),
               module ? L"ok" : L"failed");
        return module;
    }

    HMODULE module = LoadLibraryA_Original(name);
    if (module && !wide.empty() && g_loadRedirectLimit.Allow(200)) {
        std::wstring base = BaseName(wide.c_str());
        if (EqualsNoCase(base.c_str(), L"dui71.dll")) {
            Wh_Log(L"LoadLibraryA: the Windows 7 DirectUI (dui71.dll) is now "
                   L"loaded in this process");
        }
    }
    return module;
}

using LoadLibraryExA_t = HMODULE(WINAPI*)(LPCSTR, HANDLE, DWORD);
LoadLibraryExA_t LoadLibraryExA_Original;

HMODULE WINAPI LoadLibraryExA_Hook(LPCSTR name, HANDLE file, DWORD flags) {
    void* caller = MOD_CALLER_ADDRESS();
    std::wstring wide;
    WideStringFromAnsi(name, &wide);

    std::wstring redirected;
    if (!wide.empty() && SafeRedirectDll(wide.c_str(), caller, flags, redirected)) {
        std::string narrow = NarrowString(redirected);
        HMODULE module = LoadLibraryExA_Original(narrow.c_str(), file, flags);
        Wh_Log(L"LoadLibraryExA: %s -> %s (%s)", wide.c_str(),
               redirected.c_str(), module ? L"ok" : L"failed");
        return module;
    }

    return LoadLibraryExA_Original(name, file, flags);
}

// ---------------------------------------------------------------------------
// Resource requests: the keys that are not plain text (Enter, the arrows, the
// Windows key, the menu key) take their face from a resource of the Windows 7
// keyboard. Knowing exactly which resource is asked for, and whether it is
// found, is what tells a missing file from a missing resource.
// ---------------------------------------------------------------------------

HRSRC FaceResourceHandleFor(LPCWSTR type, LPCWSTR name);
HRSRC CaptionBlockFor(HMODULE module, LPCWSTR type, LPCWSTR name, HRSRC original);
bool IsCaptionBlockHandle(const void* handle);
const std::vector<BYTE>* CaptionBlockBits(const void* handle);

std::wstring DescribeResourceIdentifier(LPCWSTR identifier) {
    if (!identifier) {
        return L"(null)";
    }
    if (IS_INTRESOURCE(identifier)) {
        wchar_t text[32];
        swprintf(text, ARRAYSIZE(text), L"#%u", (unsigned int)(ULONG_PTR)identifier);
        return text;
    }
    return identifier;
}

std::wstring DescribeModuleBase(HMODULE module) {
    wchar_t text[32];
    swprintf(text, ARRAYSIZE(text), L"base 0x%llX",
             (unsigned long long)(ULONG_PTR)module);
    return text;
}

std::wstring DescribeResourceModule(HMODULE module) {
    if (!module) {
        return L"(no module)";
    }

    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return std::wstring(PathFindFileNameW(path)) + L" (" +
               DescribeModuleBase(module) + L")";
    }

    // A module that was loaded as a data file has no file name here, but it is
    // a mapped file, and the mapping does tell which one it is.
    using GetMappedFileNameW_t = DWORD(WINAPI*)(HANDLE, LPVOID, LPWSTR, DWORD);
    static GetMappedFileNameW_t getMappedFileName = []() -> GetMappedFileNameW_t {
        HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
        if (!kernel32) {
            return nullptr;
        }
        return reinterpret_cast<GetMappedFileNameW_t>(
            GetProcAddress(kernel32, "K32GetMappedFileNameW"));
    }();

    if (getMappedFileName) {
        wchar_t mapped[MAX_PATH];
        DWORD length = getMappedFileName(GetCurrentProcess(), module, mapped,
                                         ARRAYSIZE(mapped));
        if (length > 0 && length < ARRAYSIZE(mapped)) {
            return std::wstring(PathFindFileNameW(mapped)) + L" (" +
                   DescribeModuleBase(module) + L", loaded as a data file)";
        }
    }

    return L"(unnamed module, " + DescribeModuleBase(module) + L")";
}

void LogResourceRequest(const wchar_t* api, HMODULE module, LPCWSTR type,
                        LPCWSTR name, HANDLE result) {
    static std::atomic<int> limit{0};
    if (limit.fetch_add(1, std::memory_order_relaxed) >= 400) {
        return;
    }

    std::wstring details;
    wchar_t text[128];
    if (result) {
        HRSRC resource = (HRSRC)result;
        DWORD size = SizeofResource(module, resource);
        swprintf(text, ARRAYSIZE(text), L"found, %lu bytes", (unsigned long)size);
        details = text;
    } else {
        swprintf(text, ARRAYSIZE(text), L"NOT FOUND, error %lu",
                 (unsigned long)GetLastError());
        details = text;
    }

    Wh_Log(L"%s: type=%s id=%s of %s -> %s", api,
           DescribeResourceIdentifier(type).c_str(),
           DescribeResourceIdentifier(name).c_str(),
           DescribeResourceModule(module).c_str(), details.c_str());
}

using FindResourceW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR);
FindResourceW_t FindResourceW_Original;

HRSRC WINAPI FindResourceW_Hook(HMODULE module, LPCWSTR name, LPCWSTR type) {
    if (HRSRC face = FaceResourceHandleFor(type, name)) {
        return face;
    }

    HRSRC result = FindResourceW_Original(module, name, type);
    if (result) {
        if (HRSRC block = CaptionBlockFor(module, type, name, result)) {
            return block;
        }
    }
    if (g_diagnostics) {
        LogResourceRequest(L"FindResourceW", module, type, name, result);
    }
    return result;
}

using FindResourceExW_t = HRSRC(WINAPI*)(HMODULE, LPCWSTR, LPCWSTR, WORD);
FindResourceExW_t FindResourceExW_Original;

HRSRC WINAPI FindResourceExW_Hook(HMODULE module, LPCWSTR type, LPCWSTR name,
                                  WORD language) {
    if (HRSRC face = FaceResourceHandleFor(type, name)) {
        return face;
    }

    HRSRC result = FindResourceExW_Original(module, type, name, language);
    if (result) {
        if (HRSRC block = CaptionBlockFor(module, type, name, result)) {
            return block;
        }
    }
    if (g_diagnostics) {
        LogResourceRequest(L"FindResourceExW", module, type, name, result);
    }
    return result;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// The faces of the keys that are not text.
//
// The Windows 7 layout marks those keys with an image id (15501 the Windows
// key, 15502 the menu key, 15503 Enter, 15504 to 15507 the four arrows, all
// listed in the UIFILE resources of tabskb.dll) and the keyboard asks for the
// picture with
//
//     LoadImageW(instance, MAKEINTRESOURCEW(id), IMAGE_ICON, cx, cy, flags)
//
// The pictures behind those ids are the ones of `TipResX.dll`, the resource
// module of the keyboard: the Windows 7 osk.exe loads it by name - the string
// "TipResX.dll" sits in its code next to "TabSKB.dll" and "msswch.dll" - and it
// is where the captions of the keys come from as well (`RT_STRING` blocks 938
// to 966, which is why they are numbered 15003 and up).
//
// On Windows 10 and 11 that name resolves to the resource module of the modern
// keyboard, `%CommonProgramFiles%\microsoft shared\ink\TipResX.dll`. That
// file still answers the caption strings of the Windows 7 ids - the ids were
// inherited - but it has no picture at all for 15503 to 15507, and a modern
// logo where the Windows flag used to be. That is the whole reason why Enter
// and the four arrows come out empty and why the Windows key is wrong.
//
// The pictures below are therefore drawn by this mod, from the geometry of the
// original ones: a one pixel stroke, an Enter that is twice as wide as a key
// and bends upwards at its right end, four sheared panes for the flag, a
// window with two menu lines for the menu key. They are stored as the bits of
// an icon (header, pixels, mask) in base64. Only the coverage is stored here:
// the colour is applied when the picture is built (white by default, like the
// original pictures; dev/gen_faces.py holds the drawing).
// ---------------------------------------------------------------------------

// #15501: 16x16, 32 bits per pixel, the ink in the alpha channel.
static const char kFaceBase64_15501[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wv///8j////Dv///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////Bf///5/////4//////////3////I"
    "////G////wD///8A////AP///wD///9U////F////wD///8V////ef///0T////g////////"
    "/////////////////2z///8A////AP///wD///8A////rv/////////4//////////////+U"
    "////lv////////////////////////+2////AP///wD///8A////AP///2X/////////////"
    "////////////2////0z/////////////////////////9v///wn///8A////AP///wD///8b"
    "/////v////////////////////////8w////8P///63///91////ev///6T///81////AP//"
    "/wD///8A////AP///9D/////////////////////////Z////yH///91////wP///8f///+Y"
    "////Nf///wD///8A////AP///wD///8h////dP///6T///+h////W////yL///9l////////"
    "/////////////////9b///8A////AP///wD///8A////Qv///77///+F////fP///7P////1"
    "////Kv//////////////////////////////If///wD///8A////AP///wj////1////////"
    "/////////////////1H////U/////////////////////////23///8A////AP///wD///8A"
    "////tP////////////////////////+b////jv/////////y////3f////b///+2////AP//"
    "/wD///8A////AP///2r/////////////////////////5P///zr///9Y////BP///wD///8H"
    "////P////wD///8A////AP///wD///8a////v/////r/////////9f///5j///8F////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8I////Gv///wf///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// #15502: 16x16, 32 bits per pixel, the ink in the alpha channel.
static const char kFaceBase64_15502[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///0j///9g////YP///2D///9g////YP///2D///9g////YP///2D///9g////SP//"
    "/wD///8A////AP///wD////7////x////7////+/////v////7////+/////v////7////+/"
    "////x/////v///8A////AP///wD///8A/////////yD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///yD/////////AP///wD///8A////AP////////8g////AP///1T///+A"
    "////gP///4D///+A////fP///wj///8g/////////wD///8A////AP///wD/////////IP//"
    "/wD///9U////gP///4D///+A////gP///3z///8I////IP////////8A////AP///wD///8A"
    "/////////yD///8A////EP///yD///8g////IP///yD///8c////AP///yD/////////AP//"
    "/wD///8A////AP////////8g////AP///6f/////////////////////////9////xT///8g"
    "/////////wD///8A////AP///wD/////////IP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////IP////////8A////AP///wD///8A/////////zz///8g////IP///yD///8g"
    "////IP///yD///8g////IP///zz/////////AP///wD///8A////AP//////////////////"
    "/////////////////////////////////////////////////wD///8A////AP///wD/////"
    "////IP///wD///8A////AP///wD///8A////AP///wD///8A////IP////////8A////AP//"
    "/wD///8A/////////yD///8A////AP///wD///8A////AP///wD///8A////AP///yD/////"
    "////AP///wD///8A////AP////v////H////v////7////+/////v////7////+/////v///"
    "/7/////H////+////wD///8A////AP///wD///9I////YP///2D///9g////YP///2D///9g"
    "////YP///2D///9g////YP///0j///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// #15503: 34x16, 32 bits per pixel, the ink in the alpha channel.
static const char kFaceBase64_15503[] =
    "KAAAACIAAAAgAAAAAQAgAAAAAACACAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////g////0j///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////j////9f///8Y////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////j////9f///8Y////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////i/////////+X////gP///4D///+A////gP//"
    "/4D///+A////gP///4D///+A////gP///4D///+A////gP///4D///+A////gP///4D///+A"
    "////gP///4D///+A////gP///4D///+A////gP///4D///+A////MP///wD///8A////AP//"
    "/7//////////q////5////+f////n////5////+f////n////5////+f////n////5////+f"
    "////n////5////+f////n////5////+f////n////5////+f////n////5////+f////n///"
    "/5////+f////z////4D///8A////AP///wD///8M////w////8P///8M////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///4D///+A////AP///wD///8A"
    "////AP///wz////D////w////wz///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///+A////gP///wD///8A////AP///wD///8A////DP///7////9Y////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////gP///4D///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///3z///98////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8I////CP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==";

// #15504: 16x16, 32 bits per pixel, the ink in the alpha channel (redrawn in 0.3.0: up arrow: the head points up, the shaft runs down from its tip).
static const char kFaceBase64_15504[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP////f///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD/////////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "/////////wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP////////8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD/////////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A/////////wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP////////8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP////P///9U////AP///wD/////"
    "////AP///wD///9U////8////wD///8A////AP///wD///8A////AP///wD///9U////9///"
    "/1T///8A/////////wD///9U////9////1T///8A////AP///wD///8A////AP///wD///8A"
    "////AP///1T////3////VP////////9U////9////1T///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////VP////f/////////9////1T///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///9U////9////1T///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// #15505: 16x16, 32 bits per pixel, the ink in the alpha channel (redrawn in 0.3.0: down arrow: the head points down, the shaft runs up from its tip).
static const char kFaceBase64_15505[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////VP////f///9U////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////VP////f/////////9///"
    "/1T///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////VP////f///9U"
    "/////////1T////3////VP///wD///8A////AP///wD///8A////AP///wD///8A////VP//"
    "//f///9U////AP////////8A////VP////f///9U////AP///wD///8A////AP///wD///8A"
    "////AP////P///9U////AP///wD/////////AP///wD///9U////8////wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A/////////wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP////////8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD/////"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A/////////wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP////////8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD/////////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////9////wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// #15506: 16x16, 32 bits per pixel, the ink in the alpha channel.
static const char kFaceBase64_15506[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///1T////z"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/1T////3////VP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///1T////3////VP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///1T////3////VP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD////3////////////////////////////////////////"
    "//////////////////f///8A////AP///wD///8A////VP////f///9U////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///9U////9///"
    "/1T///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///1T////3////VP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////VP////P///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// #15507: 16x16, 32 bits per pixel, the ink in the alpha channel.
static const char kFaceBase64_15507[] =
    "KAAAABAAAAAgAAAAAQAgAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD////z////VP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////VP////f///9U////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///9U////9////1T///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///1T////3"
    "////VP///wD///8A////AP///wD////3////////////////////////////////////////"
    "//////////////////f///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////VP////f///9U////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////VP////f///9U////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////VP////f///9U////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP////P///9U////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A"
    "////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP//"
    "/wD///8A////AP///wD///8A////AP///wD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

// The bytes of the bits of one face: the header, the pixels (32 bits per
// pixel, as many rows as the height) and the mask (one bit per pixel, the rows
// of the mask are four byte aligned).
size_t FaceDibBytes(int width, int height) {
    int stride = ((width + 31) / 32) * 4;
    return 40 + (size_t)width * height * 4 + (size_t)stride * height;
}

struct KeyFace {
    UINT id;
    PCWSTR what;
    int width;  // the natural size of the drawing: the Enter of the original
    int height; // keyboard is twice as wide as one key, everything else 16x16
    const char* base64;
};

const KeyFace kKeyFaces[] = {
    {15501, L"the Windows key", 16, 16, kFaceBase64_15501},
    {15502, L"the menu key", 16, 16, kFaceBase64_15502},
    {15503, L"Enter", 34, 16, kFaceBase64_15503},
    {15504, L"the up arrow", 16, 16, kFaceBase64_15504},
    {15505, L"the down arrow", 16, 16, kFaceBase64_15505},
    {15506, L"the left arrow", 16, 16, kFaceBase64_15506},
    {15507, L"the right arrow", 16, 16, kFaceBase64_15507},
};

size_t FaceIndexFromId(UINT id) {
    for (size_t i = 0; i < ARRAYSIZE(kKeyFaces); i++) {
        if (kKeyFaces[i].id == id) {
            return i;
        }
    }
    return (size_t)-1;
}

int Base64Value(char character) {
    if (character >= 'A' && character <= 'Z') {
        return character - 'A';
    }
    if (character >= 'a' && character <= 'z') {
        return character - 'a' + 26;
    }
    if (character >= '0' && character <= '9') {
        return character - '0' + 52;
    }
    if (character == '+') {
        return 62;
    }
    if (character == '/') {
        return 63;
    }
    return -1;
}

bool Base64Decode(const char* text, std::vector<BYTE>& out) {
    out.clear();
    unsigned int buffer = 0;
    int bits = 0;

    for (const char* p = text; *p; p++) {
        if (*p == '=') {
            break;
        }
        int value = Base64Value(*p);
        if (value < 0) {
            continue;  // line breaks and stray characters are ignored
        }
        buffer = (buffer << 6) | (unsigned int)value;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((BYTE)((buffer >> bits) & 0xFF));
        }
    }

    return !out.empty();
}

// The face of a key, decoded once: the bits as they are stored here and the
// coverage, that is the alpha channel top-down, which is all the drawing needs.
bool FaceDib(size_t index, std::vector<BYTE>& out, std::vector<BYTE>& coverage) {
    static std::mutex lock;
    static std::vector<BYTE> bitsCache[ARRAYSIZE(kKeyFaces)];
    static std::vector<BYTE> coverageCache[ARRAYSIZE(kKeyFaces)];

    std::lock_guard<std::mutex> guard(lock);
    if (bitsCache[index].empty()) {
        const KeyFace& face = kKeyFaces[index];
        std::vector<BYTE> decoded;
        if (!Base64Decode(face.base64, decoded)) {
            Wh_Log(L"Face #%u (%s): the built-in picture cannot be decoded",
                   face.id, face.what);
            return false;
        }

        const BITMAPINFOHEADER* header =
            reinterpret_cast<const BITMAPINFOHEADER*>(decoded.data());
        if (decoded.size() != FaceDibBytes(face.width, face.height) ||
            header->biWidth != face.width ||
            header->biHeight != face.height * 2 || header->biBitCount != 32 ||
            header->biCompression != BI_RGB) {
            Wh_Log(L"Face #%u (%s): the built-in picture is not a %dx%d 32 bit "
                   L"icon (%zu bytes)",
                   face.id, face.what, face.width, face.height, decoded.size());
            return false;
        }

        // The pixels are bottom-up in the file; the colour is flat, so the
        // alpha channel is all the drawing needs.
        std::vector<BYTE> alpha((size_t)face.width * face.height);
        const BYTE* pixels = decoded.data() + 40;
        for (int y = 0; y < face.height; y++) {
            for (int x = 0; x < face.width; x++) {
                const BYTE* pixel =
                    pixels +
                    (size_t)(face.height - 1 - y) * face.width * 4 + x * 4;
                alpha[(size_t)y * face.width + x] = pixel[3];
            }
        }

        bitsCache[index] = decoded;
        coverageCache[index] = alpha;
    }

    out = bitsCache[index];
    coverage = coverageCache[index];
    return true;
}

// ---------------------------------------------------------------------------
// The colour of the faces: white, like the original pictures, unless the
// settings ask for another colour. "auto" is white as well. The visual style
// of the system is only the last resort for a value the setting does not name.
//
// Microsoft documents the two sources used for "auto" and for a value that does
// not name a colour:
//   * GetThemeColor(UxTheme.dll) with TMT_TEXTCOLOR (3803) of the normal state
//     of a push button (BP_PUSHBUTTON 1, PBS_NORMAL 1): "the colour of the
//     text" of the control of the current visual style;
//   * the "default app mode" of the system, `AppsUseLightTheme` below
//     `Software\Microsoft\Windows\CurrentVersion\Themes\Personalize`. The
//     keyboard, like any window, is drawn dark when that value is 0. (The
//     value next to it, `SystemUsesLightTheme`, is the taskbar and the Start
//     menu: it is *not* what colours an application, and reading it was the
//     mistake that left the faces black on the dark keys of this system.)
// ---------------------------------------------------------------------------

struct ThemeApi {
    using OpenThemeData_t = HANDLE(WINAPI*)(HWND, LPCWSTR);
    using GetThemeColor_t = HRESULT(WINAPI*)(HANDLE, int, int, int, COLORREF*);
    using CloseThemeData_t = HRESULT(WINAPI*)(HANDLE);

    OpenThemeData_t openThemeData = nullptr;
    GetThemeColor_t getThemeColor = nullptr;
    CloseThemeData_t closeThemeData = nullptr;
    bool available = false;

    ThemeApi() {
        HMODULE theme = LoadLibraryW(L"uxtheme.dll");
        if (!theme) {
            return;
        }
        openThemeData = reinterpret_cast<OpenThemeData_t>(
            GetProcAddress(theme, "OpenThemeData"));
        getThemeColor = reinterpret_cast<GetThemeColor_t>(
            GetProcAddress(theme, "GetThemeColor"));
        closeThemeData = reinterpret_cast<CloseThemeData_t>(
            GetProcAddress(theme, "CloseThemeData"));
        available = openThemeData && getThemeColor && closeThemeData;
    }
};

ThemeApi& Theme() {
    static ThemeApi api;
    return api;
}

constexpr int kPbsNormal = 1;      // PBS_NORMAL
constexpr int kBpPushButton = 1;   // BP_PUSHBUTTON
constexpr int kTmtTextColor = 3803;  // TMT_TEXTCOLOR

bool ThemeTextColour(COLORREF* colour) {
    ThemeApi& api = Theme();
    if (!api.available) {
        return false;
    }
    HANDLE theme = api.openThemeData(nullptr, L"Button");
    if (!theme) {
        return false;
    }
    COLORREF value = 0;
    HRESULT result = api.getThemeColor(theme, kBpPushButton, kPbsNormal,
                                       kTmtTextColor, &value);
    api.closeThemeData(theme);
    if (FAILED(result)) {
        return false;
    }
    *colour = value;
    return true;
}

int ReadAppMode() {
    // 0 = the applications are drawn dark, 1 = drawn light, -1 = no answer.
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                      L"Personalize",
                      0, KEY_READ, &key) != ERROR_SUCCESS) {
        return -1;
    }
    DWORD value = 0;
    DWORD size = sizeof(value);
    bool read = RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                                 (LPBYTE)&value, &size) == ERROR_SUCCESS;
    RegCloseKey(key);
    return read ? (value ? 1 : 0) : -1;
}

int Luminance(COLORREF colour) {
    // Rec. 601 luma, in per mille.
    return (GetRValue(colour) * 299 + GetGValue(colour) * 587 +
            GetBValue(colour) * 114) / 1000;
}

COLORREF ResolveKeyFaceColour(std::wstring* source) {
    auto set = [source](PCWSTR text) {
        if (source) {
            *source = text;
        }
    };

    if (EqualsNoCase(g_keyFaceColor.c_str(), L"white")) {
        set(L"the keyFaceColor setting");
        return RGB(255, 255, 255);
    }
    if (EqualsNoCase(g_keyFaceColor.c_str(), L"black")) {
        set(L"the keyFaceColor setting");
        return RGB(0, 0, 0);
    }
    if (g_keyFaceColor.size() == 7 && g_keyFaceColor[0] == L'#') {
        unsigned int red = 0, green = 0, blue = 0;
        if (swscanf_s(g_keyFaceColor.c_str() + 1, L"%2X%2X%2X", &red, &green,
                      &blue) == 3) {
            set(L"the keyFaceColor setting");
            return RGB(red, green, blue);
        }
    }

    if (EqualsNoCase(g_keyFaceColor.c_str(), L"auto")) {
        // The keys of the Windows 7 keyboard are dark whatever the theme says
        // (the layout and its style draw them), and the faces of the original
        // are white: "auto" therefore means white. The captions of this system
        // are NOT a good guide: they are black on a light theme, and black
        // faces on dark keys are invisible (this was the v0.2.8 log).
        set(L"the dark keys of the Windows 7 keyboard (white, as the original pictures)");
        return RGB(255, 255, 255);
    }

    COLORREF colour = 0;
    bool haveColour = ThemeTextColour(&colour);
    int appMode = ReadAppMode();  // 0 = dark applications, 1 = light, -1 = ?

    // This is the last resort, for the values of the setting that do not name
    // a colour. The colour of the captions of the theme comes first; but on a
    // system whose visual style is the light one while the applications are
    // drawn dark (a combination Windows allows), that colour is the black of
    // the light style, which would be invisible on the dark keys. The app mode
    // is what colours the keys, so it has the last word then.
    if (appMode == 0 && haveColour && Luminance(colour) < 450) {
        set(L"the dark applications of this system");
        return RGB(255, 255, 255);
    }
    if (appMode == 1 && haveColour && Luminance(colour) >= 450) {
        set(L"the light applications of this system");
        return RGB(0, 0, 0);
    }
    if (haveColour) {
        set(L"the text colour of the visual style");
        return colour;
    }
    if (appMode == 0) {
        set(L"the dark applications of this system");
        return RGB(255, 255, 255);
    }
    if (appMode == 1) {
        set(L"the light applications of this system");
        return RGB(0, 0, 0);
    }
    set(L"the colour of the original pictures (white)");
    return RGB(255, 255, 255);
}

// Only the coverage is resampled: the colour of a face is flat, and the Enter
// picture is not square, so the target is not either.
void ResampleFaceCoverage(const BYTE* source, int sourceWidth,
                          int sourceHeight, int targetWidth, int targetHeight,
                          BYTE* target) {
    for (int y = 0; y < targetHeight; y++) {
        double sourceY =
            ((double)y + 0.5) * sourceHeight / targetHeight - 0.5;
        int y0 = (int)sourceY;
        double yFraction = sourceY - (double)y0;
        int y1 = y0 + 1;
        if (y0 < 0) {
            y0 = 0;
            yFraction = 0;
        }
        if (y1 > sourceHeight - 1) {
            y1 = sourceHeight - 1;
        }

        for (int x = 0; x < targetWidth; x++) {
            double sourceX =
                ((double)x + 0.5) * sourceWidth / targetWidth - 0.5;
            int x0 = (int)sourceX;
            double xFraction = sourceX - (double)x0;
            int x1 = x0 + 1;
            if (x0 < 0) {
                x0 = 0;
                xFraction = 0;
            }
            if (x1 > sourceWidth - 1) {
                x1 = sourceWidth - 1;
            }

            double top =
                source[(size_t)y0 * sourceWidth + x0] * (1 - xFraction) +
                source[(size_t)y0 * sourceWidth + x1] * xFraction;
            double bottom =
                source[(size_t)y1 * sourceWidth + x0] * (1 - xFraction) +
                source[(size_t)y1 * sourceWidth + x1] * xFraction;
            double value = top * (1 - yFraction) + bottom * yFraction;
            target[(size_t)y * targetWidth + x] = (BYTE)(value + 0.5);
        }
    }
}

// The icon of a face, in the size the caller asks for. The picture is resampled
// to that size, exactly as the system does with the icon of a resource, so a
// wide Enter key gets a wide Enter face.
bool BuildFaceIcon(size_t index, int width, int height, COLORREF colour,
                   std::vector<BYTE>& out, int* solidPixels) {
    std::vector<BYTE> decoded;
    std::vector<BYTE> coverage;
    if (!FaceDib(index, decoded, coverage)) {
        return false;
    }

    int stride = ((width + 31) / 32) * 4;
    size_t pixelBytes = (size_t)width * height * 4;
    out.assign(40 + pixelBytes + (size_t)stride * height, 0);

    BITMAPINFOHEADER header{};
    header.biSize = sizeof(BITMAPINFOHEADER);
    header.biWidth = width;
    header.biHeight = height * 2;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;
    header.biSizeImage = (DWORD)pixelBytes;
    memcpy(out.data(), &header, sizeof(header));

    std::vector<BYTE> scaled((size_t)width * height);
    ResampleFaceCoverage(coverage.data(), kKeyFaces[index].width,
                         kKeyFaces[index].height, width, height,
                         scaled.data());

    BYTE blue = GetBValue(colour);
    BYTE green = GetGValue(colour);
    BYTE red = GetRValue(colour);
    int solid = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            BYTE alpha = scaled[(size_t)y * width + x];
            if (alpha >= 128) {
                solid++;
            }
            BYTE* pixel =
                out.data() + 40 + (size_t)(height - 1 - y) * width * 4 + x * 4;
            pixel[0] = blue;
            pixel[1] = green;
            pixel[2] = red;
            pixel[3] = alpha;
        }
    }

    // The mask of the icon, written the way the original pictures carry it: the
    // bits of the glyph are 0 ("put the colour of the picture down") and the
    // background is 1 ("leave the key alone"). It has to describe the same shape
    // as the alpha channel, so it is written from the same coverage, row by row
    // from the bottom, like the pixels above it.
    BYTE* mask = out.data() + 40 + pixelBytes;
    for (int y = 0; y < height; y++) {
        BYTE* row = mask + (size_t)(height - 1 - y) * stride;
        for (int x = 0; x < width; x++) {
            if (scaled[(size_t)y * width + x] == 0) {
                row[x / 8] |= (BYTE)(0x80u >> (x % 8));
            }
        }
    }

    if (solidPixels) {
        *solidPixels = solid;
    }
    return true;
}

// ---------------------------------------------------------------------------
// The faces of the current colour, once: the bits of the seven icons and the
// seven groups that point at them. The groups are needed by the resource path
// below; the icons are needed by both paths.
// ---------------------------------------------------------------------------

constexpr int kFaceGroupKind = 1;  // RT_GROUP_ICON
constexpr int kFaceIconKind = 2;   // RT_ICON

struct FaceArtwork {
    std::mutex lock;
    bool built = false;
    COLORREF colour = 0;
    std::wstring colourSource;
    std::vector<BYTE> icons[ARRAYSIZE(kKeyFaces)];
    std::vector<BYTE> groups[ARRAYSIZE(kKeyFaces)];
    // The bytes of the previous colour are moved here when the colour changes:
    // they have been handed to the system (LockResource) and must stay valid,
    // so they are kept instead of being freed.
    std::vector<std::vector<BYTE>> retired;
};

FaceArtwork& Artwork() {
    static FaceArtwork artwork;
    return artwork;
}

// A colour change rebuilds the pictures; the ones that were given to the system
// are kept alive, and the icons that were built for them are not freed either
// (an icon built with LR_SHARED belongs to the system). It costs a few
// kilobytes for every change of the setting, and it is what makes the setting
// work without restarting the keyboard.
void InvalidateArtwork() {
    FaceArtwork& artwork = Artwork();
    std::lock_guard<std::mutex> guard(artwork.lock);
    if (!artwork.built) {
        return;
    }
    for (size_t i = 0; i < ARRAYSIZE(kKeyFaces); i++) {
        artwork.retired.emplace_back();
        artwork.retired.back().swap(artwork.icons[i]);
        artwork.retired.emplace_back();
        artwork.retired.back().swap(artwork.groups[i]);
    }
    artwork.built = false;
}

// A GRPICONDIR with a single entry, which is what an icon of one size is: see
// Microsoft Learn, *Resource File Formats*, "RT_GROUP_ICON".
void BuildFaceGroup(const KeyFace& face, const std::vector<BYTE>& icon,
                    std::vector<BYTE>& out) {
    out.assign(6 + 14, 0);
    out[2] = 1;  // idType: icon
    out[4] = 1;  // idCount: one image
    BYTE* entry = out.data() + 6;
    entry[0] = (BYTE)face.width;   // bWidth, 0 means 256
    entry[1] = (BYTE)face.height;  // bHeight
    entry[4] = 1;                  // wPlanes
    entry[6] = 32;                 // wBitCount
    DWORD size = (DWORD)icon.size();
    memcpy(entry + 8, &size, sizeof(size));
    WORD id = (WORD)face.id;
    memcpy(entry + 12, &id, sizeof(id));
}

// The pictures are built once per process and never rebuilt: the bytes are
// handed to the system (LockResource) and must stay valid. A theme change in
// the middle of a session therefore keeps the pictures of the resource path,
// while the ones built for LoadImage follow the colour of the moment.
bool EnsureArtwork(std::wstring* colourSource) {
    FaceArtwork& artwork = Artwork();
    std::lock_guard<std::mutex> guard(artwork.lock);

    if (artwork.built) {
        if (colourSource) {
            *colourSource = artwork.colourSource;
        }
        return true;
    }

    std::wstring source;
    COLORREF colour = ResolveKeyFaceColour(&source);

    std::vector<BYTE> icons[ARRAYSIZE(kKeyFaces)];
    std::vector<BYTE> groups[ARRAYSIZE(kKeyFaces)];
    for (size_t i = 0; i < ARRAYSIZE(kKeyFaces); i++) {
        if (!BuildFaceIcon(i, kKeyFaces[i].width, kKeyFaces[i].height, colour,
                           icons[i], nullptr)) {
            return false;
        }
        BuildFaceGroup(kKeyFaces[i], icons[i], groups[i]);
    }

    for (size_t i = 0; i < ARRAYSIZE(kKeyFaces); i++) {
        artwork.icons[i].swap(icons[i]);
        artwork.groups[i].swap(groups[i]);
    }
    artwork.colour = colour;
    artwork.colourSource = source;
    artwork.built = true;
    if (colourSource) {
        *colourSource = source;
    }
    return true;
}

// The icon of a face, as a handle the keyboard can draw. Icons asked for with
// LR_SHARED are kept: their owner must not free them, and they are asked for
// more than once.
HICON CreateFaceIcon(size_t index, int cx, int cy, UINT flags) {
    int width = cx;
    int height = cy;
    if (width <= 0 && height <= 0) {
        if (flags & LR_DEFAULTSIZE) {
            width = GetSystemMetrics(SM_CXICON);
            height = GetSystemMetrics(SM_CYICON);
        } else {
            width = kKeyFaces[index].width;
            height = kKeyFaces[index].height;
        }
    } else if (width <= 0) {
        width = height * kKeyFaces[index].width / kKeyFaces[index].height;
    } else if (height <= 0) {
        height = width * kKeyFaces[index].height / kKeyFaces[index].width;
    }
    if (width < 4) {
        width = kKeyFaces[index].width;
    }
    if (height < 4) {
        height = kKeyFaces[index].height;
    }
    if (width > 512) {
        width = 512;
    }
    if (height > 512) {
        height = 512;
    }

    COLORREF colour = ResolveKeyFaceColour(nullptr);

    struct CacheEntry {
        size_t index;
        int width;
        int height;
        COLORREF colour;
        HICON icon;
    };
    static std::mutex lock;
    static CacheEntry cache[16]{};

    if (flags & LR_SHARED) {
        std::lock_guard<std::mutex> guard(lock);
        for (const CacheEntry& entry : cache) {
            if (entry.icon && entry.index == index && entry.width == width &&
                entry.height == height && entry.colour == colour) {
                return entry.icon;
            }
        }
    }

    int solid = 0;
    std::vector<BYTE> dib;
    if (!BuildFaceIcon(index, width, height, colour, dib, &solid)) {
        return nullptr;
    }

    HICON icon = CreateIconFromResourceEx(dib.data(), (DWORD)dib.size(), TRUE,
                                          0x00030000, width, height,
                                          LR_DEFAULTCOLOR);
    if (!icon) {
        Wh_Log(L"Face #%u (%s): CreateIconFromResourceEx refused the built "
               L"picture (%dx%d, %zu bytes, error %u)",
               kKeyFaces[index].id, kKeyFaces[index].what, width, height,
               dib.size(), GetLastError());
        return nullptr;
    }

    {
        static std::atomic<int> limit{0};
        if (limit.fetch_add(1, std::memory_order_relaxed) < 20) {
            Wh_Log(L"Face #%u (%s): %dx%d icon built (%d of %d pixels are "
                   L"solid, asked %dx%d, flags 0x%X)",
                   kKeyFaces[index].id, kKeyFaces[index].what, width, height,
                   solid, width * height, cx, cy, flags);
        }
    }

    {
        // The first icons are asked what Windows itself reads in them: it is
        // what the keyboard is given, and it is worth having it in the log when
        // a face does not come out on the screen.
        static std::atomic<int> checks{0};
        if (checks.fetch_add(1, std::memory_order_relaxed) < 3) {
            ICONINFO info{};
            if (GetIconInfo(icon, &info)) {
                BITMAP colourInfo{};
                BITMAP maskInfo{};
                GetObjectW(info.hbmColor, sizeof(colourInfo), &colourInfo);
                GetObjectW(info.hbmMask, sizeof(maskInfo), &maskInfo);
                Wh_Log(L"Face #%u: the system reads the icon as %dx%d, the "
                       L"colour bitmap as %ldx%ld %u bpp and the mask as %ldx%ld "
                       L"%u bpp",
                       kKeyFaces[index].id, width, height, colourInfo.bmWidth,
                       colourInfo.bmHeight, (unsigned)colourInfo.bmBitsPixel,
                       maskInfo.bmWidth, maskInfo.bmHeight,
                       (unsigned)maskInfo.bmBitsPixel);
                if (info.hbmColor) {
                    DeleteObject(info.hbmColor);
                }
                if (info.hbmMask) {
                    DeleteObject(info.hbmMask);
                }
            } else {
                Wh_Log(L"Face #%u: GetIconInfo refused the icon (error %u)",
                       kKeyFaces[index].id, GetLastError());
            }
        }
    }

    if (flags & LR_SHARED) {
        std::lock_guard<std::mutex> guard(lock);
        for (CacheEntry& entry : cache) {
            if (!entry.icon) {
                entry.index = index;
                entry.width = width;
                entry.height = height;
                entry.colour = colour;
                entry.icon = icon;
                break;
            }
        }
    }

    return icon;
}

std::wstring DescribeKeyFaceColour() {
    std::wstring source;
    COLORREF colour = ResolveKeyFaceColour(&source);
    wchar_t text[64];
    swprintf(text, ARRAYSIZE(text), L"#%02X%02X%02X, from %s",
             GetRValue(colour), GetGValue(colour), GetBValue(colour),
             source.c_str());
    return text;
}

// ---------------------------------------------------------------------------
// The same pictures, served where the keyboard looks its resources up.
//
// The captions prove how the keyboard reads the module of its resources: it
// asks `FindResourceExW(module, #6, block, language)` and then takes the bytes
// with LoadResource/LockResource (the log of this mod shows exactly that for
// `TipResX.dll`). A picture that is missing *there* cannot be loaded by any
// other means: the lookup itself fails, and a mod that only answers LoadImage
// is walked past. The two resource types of an icon are therefore served here
// as well, with the same pixels:
//
//     FindResourceExW(module, RT_GROUP_ICON, MAKEINTRESOURCE(15503), ...)
//     SizeofResource / LoadResource / LockResource on that handle
//
// The resource handle of this mod is the address of a description below, which
// is recognised by the four hooked functions: any other handle is passed to the
// original function untouched, and a handle that is not one of ours is never
// dereferenced.
// ---------------------------------------------------------------------------

// The two resource types of an icon, as the numbers they are: in the headers
// they are declared as MAKEINTRESOURCE(3) and MAKEINTRESOURCE(14), which cannot
// be compared with the id of a type without a cast.
constexpr UINT kRtIcon = 3;        // RT_ICON
constexpr UINT kRtGroupIcon = 14;  // RT_GROUP_ICON

constexpr DWORD kFaceResourceMagic = 0x304B4246;  // "FBK0"

struct FaceResource {
    DWORD magic;
    WORD id;
    WORD kind;  // kFaceGroupKind or kFaceIconKind
};

// The handle arrays live in blocks of the process heap that are never freed:
// the keyboard caches the handles it is given, and they have to stay readable
// after this mod is unloaded (see ServePersistentBytes for the bytes).
FaceResource* g_faceGroupHandles = nullptr;
FaceResource* g_faceIconHandles = nullptr;

void InitialiseFaceHandles() {
    static std::once_flag once;
    std::call_once(once, []() {
        g_faceGroupHandles =
            (FaceResource*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                     sizeof(FaceResource) *
                                         ARRAYSIZE(kKeyFaces));
        g_faceIconHandles =
            (FaceResource*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                     sizeof(FaceResource) *
                                         ARRAYSIZE(kKeyFaces));
        if (!g_faceGroupHandles || !g_faceIconHandles) {
            Wh_Log(L"Could not allocate the face handles; the pictures of the "
                   L"keys are not served");
            return;
        }
        for (size_t i = 0; i < ARRAYSIZE(kKeyFaces); i++) {
            g_faceGroupHandles[i] = {kFaceResourceMagic, (WORD)kKeyFaces[i].id,
                                     (WORD)kFaceGroupKind};
            g_faceIconHandles[i] = {kFaceResourceMagic, (WORD)kKeyFaces[i].id,
                                    (WORD)kFaceIconKind};
        }
    });
}

// Recognises a handle of this mod without ever looking at foreign memory: the
// address has to be inside the two arrays above.
const FaceResource* AsFaceResource(const void* handle) {
    InitialiseFaceHandles();
    if (!g_faceGroupHandles || !g_faceIconHandles) {
        return nullptr;
    }

    auto inside = [handle](const FaceResource* first) {
        ULONG_PTR value = (ULONG_PTR)handle;
        ULONG_PTR begin = (ULONG_PTR)first;
        ULONG_PTR end = begin + sizeof(FaceResource) * ARRAYSIZE(kKeyFaces);
        if (value < begin || value >= end ||
            ((value - begin) % sizeof(FaceResource)) != 0) {
            return false;
        }
        return true;
    };

    const FaceResource* resource = nullptr;
    if (inside(g_faceGroupHandles)) {
        resource = reinterpret_cast<const FaceResource*>(handle);
    } else if (inside(g_faceIconHandles)) {
        resource = reinterpret_cast<const FaceResource*>(handle);
    } else {
        return nullptr;
    }
    return resource->magic == kFaceResourceMagic ? resource : nullptr;
}

const std::vector<BYTE>* FaceResourceBits(const FaceResource* resource) {
    if (!EnsureArtwork(nullptr)) {
        return nullptr;
    }
    size_t index = FaceIndexFromId(resource->id);
    if (index == (size_t)-1) {
        return nullptr;
    }
    FaceArtwork& artwork = Artwork();
    std::lock_guard<std::mutex> guard(artwork.lock);
    return resource->kind == kFaceGroupKind ? &artwork.groups[index]
                                            : &artwork.icons[index];
}

// The handle served for a resource request, or nullptr when the request is not
// one of the faces of the Windows 7 keyboard.
HRSRC FaceResourceHandleFor(LPCWSTR type, LPCWSTR name) {
    if (!g_keyFaces || !g_oskProcess || !type || !name ||
        !IS_INTRESOURCE(type) || !IS_INTRESOURCE(name)) {
        return nullptr;
    }

    UINT typeId = (UINT)(ULONG_PTR)type;
    UINT id = (UINT)(ULONG_PTR)name;
    size_t index = FaceIndexFromId(id);
    if (index == (size_t)-1) {
        return nullptr;
    }

    InitialiseFaceHandles();
    if (!g_faceGroupHandles || !g_faceIconHandles) {
        return nullptr;
    }
    const FaceResource* resource = nullptr;
    if (typeId == kRtGroupIcon) {
        resource = &g_faceGroupHandles[index];
    } else if (typeId == kRtIcon) {
        resource = &g_faceIconHandles[index];
    } else {
        return nullptr;
    }

    static std::atomic<int> limit{0};
    if (limit.fetch_add(1, std::memory_order_relaxed) < 20) {
        Wh_Log(L"FindResource: type=#%u id=#%u (%s) is served by the mod (%s)",
               typeId, id, kKeyFaces[index].what,
               typeId == kRtGroupIcon ? L"the group" : L"the picture");
    }
    return (HRSRC)const_cast<FaceResource*>(resource);
}

using LoadImageW_t = HANDLE(WINAPI*)(HINSTANCE, LPCWSTR, UINT, int, int, UINT);
LoadImageW_t LoadImageW_Original;

HANDLE WINAPI LoadImageW_Hook(HINSTANCE instance, LPCWSTR name, UINT type,
                              int width, int height, UINT flags) {
    // The faces of the keys that are not text: the picture of this system is
    // replaced with the one drawn by this mod.
    if (g_keyFaces && g_oskProcess && type == IMAGE_ICON && name &&
        IS_INTRESOURCE(name)) {
        UINT id = (UINT)(ULONG_PTR)name;
        size_t index = FaceIndexFromId(id);
        if (index != (size_t)-1) {
            HICON icon = CreateFaceIcon(index, width, height, flags);
            if (icon) {
                static std::atomic<int> faceLogLimit{0};
                if (faceLogLimit.fetch_add(1, std::memory_order_relaxed) < 20) {
                    Wh_Log(L"LoadImageW: id=#%u (%s) -> the face drawn by the "
                           L"mod is used (asked %dx%d, colour %s)",
                           id, kKeyFaces[index].what, width, height,
                           DescribeKeyFaceColour().c_str());
                }
                return icon;
            }
            Wh_Log(L"LoadImageW: id=#%u (%s): the face could not be built; "
                   L"the picture of this system is used instead",
                   id, kKeyFaces[index].what);
        }
    }

    HANDLE result =
        LoadImageW_Original(instance, name, type, width, height, flags);
    if (g_diagnostics) {
        static std::atomic<int> limit{0};
        if (limit.fetch_add(1, std::memory_order_relaxed) < 200) {
            Wh_Log(L"LoadImageW: type=%u id=%s of %s (%dx%d, flags 0x%X) -> %s",
                   type, DescribeResourceIdentifier(name).c_str(),
                   DescribeResourceModule(
                       (HMODULE)GetModuleHandleW(nullptr))
                       .c_str(),
                   width, height, flags, result ? L"ok" : L"failed");
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// The resources the keyboard draws its keys with (the pictures of the keys and
// the captions) are served by this mod instead of the files on disk: the
// resource calls below answer for the handles this mod handed out.
// ---------------------------------------------------------------------------

using SizeofResource_t = DWORD(WINAPI*)(HMODULE, HRSRC);
SizeofResource_t SizeofResource_Original;

DWORD WINAPI SizeofResource_Hook(HMODULE module, HRSRC resource) {
    if (const FaceResource* face = AsFaceResource(resource)) {
        if (const std::vector<BYTE>* bits = FaceResourceBits(face)) {
            return (DWORD)bits->size();
        }
    }
    if (const std::vector<BYTE>* bytes = CaptionBlockBits(resource)) {
        return (DWORD)bytes->size();
    }
    return SizeofResource_Original(module, resource);
}

using LoadResource_t = HGLOBAL(WINAPI*)(HMODULE, HRSRC);
LoadResource_t LoadResource_Original;

HGLOBAL WINAPI LoadResource_Hook(HMODULE module, HRSRC resource) {
    // The bytes are handed out by LockResource below, which knows the handle.
    if (AsFaceResource(resource) || IsCaptionBlockHandle(resource)) {
        return (HGLOBAL)resource;
    }
    return LoadResource_Original(module, resource);
}

using LockResource_t = LPVOID(WINAPI*)(HGLOBAL);
LockResource_t LockResource_Original;

LPVOID WINAPI LockResource_Hook(HGLOBAL data) {
    // The keyboard keeps the pointer it gets here, so the bytes come from
    // blocks that are never freed: see ServePersistentBytes.
    size_t size = 0;
    if (const FaceResource* face = AsFaceResource((const void*)data)) {
        if (const std::vector<BYTE>* bits = FaceResourceBits(face)) {
            return (LPVOID)ServePersistentBytes(bits, *bits, &size);
        }
        return nullptr;
    }
    if (const std::vector<BYTE>* bytes = CaptionBlockBits((const void*)data)) {
        return (LPVOID)ServePersistentBytes(bytes, *bytes, &size);
    }
    return LockResource_Original(data);
}


// ---------------------------------------------------------------------------
// A snapshot of the modules a few seconds after the keyboard was started: it
// is what tells whether the Windows 7 DirectUI is really in use, which is the
// difference between the authentic drawing and the system one.
// ---------------------------------------------------------------------------

DWORD WINAPI ModuleSnapshotThread(LPVOID) {
    if (g_stopEvent) {
        WaitForSingleObject(g_stopEvent, 5000);
    }
    if (g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
        return 0;
    }

    Wh_Log(L"Module snapshot, 5 seconds after the keyboard started:");
    for (PCWSTR name : {L"DUser.dll", L"dui70.dll", L"dui71.dll", L"MSSWCH.dll",
                        L"TabSKB.dll", L"UxTheme.dll", L"GdiPlus.dll"}) {
        LogModuleInfo(name);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Windows 7 osk.exe side: fallback for the missing osk.exe.mui
// ---------------------------------------------------------------------------

// String ids and texts taken from the official Windows 7 (6.1.7600.16385)
// osk.exe.mui, language 1033. Note that the table starts at 1001: ids 1000 and
// 1004 are empty in the original MUI.
struct StringEntry {
    UINT id;
    PCWSTR text;
};

const StringEntry kStrings[] = {
    {1001, L"On-Screen Keyboard"},
    {1005, L"Could not start On-Screen Keyboard."},
    {1013, L"Select the device you want to use to scan through the keys."},
    {1014,
     L"A:0:0:0.5 second:1:0.75 second:2:1 second:3:1.5 seconds:4:2 "
     L"seconds:5:2.5 seconds:6:3 seconds:"},
    {1021, L"Space Bar Key"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Move the cursor to where you want to enter text."},
};

// ---------------------------------------------------------------------------
// The same string ids in the languages of the settings block. The English
// table above stays the reference; the table of the Windows interface
// language (GetUserDefaultUILanguage) is served instead, and a language or
// an id without a translation falls back to English. The texts are the ones
// the localized Windows 7 keyboard shows, so the window title and the
// strings of the keyboard read in the language of the system.
// ---------------------------------------------------------------------------

// Italian (it).
const StringEntry kStringsIt[] = {
    {1001, L"Tastiera su schermo"},
    {1005, L"Impossibile avviare la tastiera su schermo."},
    {1013, L"Selezionare il dispositivo da usare per scorrere i tasti."},
    {1014,
     L"A:0:0,5 secondi:1:0,75 secondi:2:1 secondo:3:1,5 secondi:4:2 "
     L"secondi:5:2,5 secondi:6:3 secondi:"},
    {1021, L"Tasto barra spaziatrice"},
    {1022, L"Invio"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041,
     L"Spostare il cursore nel punto in cui si desidera immettere il "
     L"testo."},
};

// Russian (ru).
const StringEntry kStringsRu[] = {
    {1001, L"Экранная клавиатура"},
    {1005, L"Не удалось запустить экранную клавиатуру."},
    {1013, L"Выберите устройство, с помощью которого нужно перебирать клавиши."},
    {1014,
     L"A:0:0,5 секунды:1:0,75 секунды:2:1 секунда:3:1,5 секунды:4:2 "
     L"секунды:5:2,5 секунды:6:3 секунды:"},
    {1021, L"Клавиша пробела"},
    {1022, L"Ввод"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Переместите курсор туда, куда нужно ввести текст."},
};

// Spanish (es).
const StringEntry kStringsEs[] = {
    {1001, L"Teclado en pantalla"},
    {1005, L"No se pudo iniciar el teclado en pantalla."},
    {1013, L"Seleccione el dispositivo que desea usar para recorrer las teclas."},
    {1014,
     L"A:0:0,5 segundos:1:0,75 segundos:2:1 segundo:3:1,5 segundos:4:2 "
     L"segundos:5:2,5 segundos:6:3 segundos:"},
    {1021, L"Tecla de la barra espaciadora"},
    {1022, L"Intro"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Mueva el cursor al lugar donde desea escribir texto."},
};

// Portuguese (Brazil) (pt-BR).
const StringEntry kStringsPtBr[] = {
    {1001, L"Teclado Virtual"},
    {1005, L"Não foi possível iniciar o Teclado Virtual."},
    {1013, L"Selecione o dispositivo que deseja usar para percorrer as teclas."},
    {1014,
     L"A:0:0,5 segundo:1:0,75 segundo:2:1 segundo:3:1,5 segundos:4:2 "
     L"segundos:5:2,5 segundos:6:3 segundos:"},
    {1021, L"Tecla de barra de espaço"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Mova o cursor para onde deseja inserir texto."},
};

// French (fr).
const StringEntry kStringsFr[] = {
    {1001, L"Clavier visuel"},
    {1005, L"Impossible de démarrer le clavier visuel."},
    {1013,
     L"Sélectionnez le périphérique à utiliser pour parcourir les "
     L"touches."},
    {1014,
     L"A:0:0,5 seconde:1:0,75 seconde:2:1 seconde:3:1,5 seconde:4:2 "
     L"secondes:5:2,5 secondes:6:3 secondes:"},
    {1021, L"Touche barre d'espace"},
    {1022, L"Entrée"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Déplacez le curseur à l'endroit où vous voulez saisir du texte."},
};

// German (de).
const StringEntry kStringsDe[] = {
    {1001, L"Bildschirmtastatur"},
    {1005, L"Die Bildschirmtastatur konnte nicht gestartet werden."},
    {1013,
     L"Wählen Sie das Gerät aus, mit dem Sie die Tasten durchlaufen "
     L"möchten."},
    {1014,
     L"A:0:0,5 Sekunde:1:0,75 Sekunde:2:1 Sekunde:3:1,5 Sekunden:4:2 "
     L"Sekunden:5:2,5 Sekunden:6:3 Sekunden:"},
    {1021, L"Leertaste"},
    {1022, L"Eingabe"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041,
     L"Bewegen Sie den Cursor an die Stelle, an der Sie Text eingeben "
     L"möchten."},
};

// Polish (pl).
const StringEntry kStringsPl[] = {
    {1001, L"Klawiatura ekranowa"},
    {1005, L"Nie można uruchomić klawiatury ekranowej."},
    {1013,
     L"Wybierz urządzenie, którego chcesz użyć do przechodzenia po "
     L"klawiszach."},
    {1014,
     L"A:0:0,5 sekundy:1:0,75 sekundy:2:1 sekunda:3:1,5 sekundy:4:2 "
     L"sekundy:5:2,5 sekundy:6:3 sekundy:"},
    {1021, L"Klawisz spacji"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"Przenieś kursor w miejsce, w którym chcesz wprowadzić tekst."},
};

// Chinese (Simplified) (zh-CN).
const StringEntry kStringsZhCn[] = {
    {1001, L"屏幕键盘"},
    {1005, L"无法启动屏幕键盘。"},
    {1013, L"选择要用于扫描按键的设备。"},
    {1014, L"A:0:0.5 秒:1:0.75 秒:2:1 秒:3:1.5 秒:4:2 秒:5:2.5 秒:6:3 秒:"},
    {1021, L"空格键"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"将光标移到要输入文本的位置。"},
};

// Japanese (ja).
const StringEntry kStringsJa[] = {
    {1001, L"スクリーン キーボード"},
    {1005, L"スクリーン キーボードを起動できませんでした。"},
    {1013, L"キーのスキャンに使用するデバイスを選択してください。"},
    {1014, L"A:0:0.5 秒:1:0.75 秒:2:1 秒:3:1.5 秒:4:2 秒:5:2.5 秒:6:3 秒:"},
    {1021, L"スペース キー"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"テキストを入力する位置にカーソルを移動してください。"},
};

// Korean (ko).
const StringEntry kStringsKo[] = {
    {1001, L"화상 키보드"},
    {1005, L"화상 키보드를 시작할 수 없습니다."},
    {1013, L"키를 스캔하는 데 사용할 장치를 선택하세요."},
    {1014, L"A:0:0.5초:1:0.75초:2:1초:3:1.5초:4:2초:5:2.5초:6:3초:"},
    {1021, L"스페이스바 키"},
    {1022, L"Enter"},
    {1023, L"F2"},
    {1024, L"F3"},
    {1025, L"F4"},
    {1026, L"F5"},
    {1027, L"F6"},
    {1028, L"F7"},
    {1029, L"F8"},
    {1030, L"F9"},
    {1031, L"F12"},
    {1041, L"텍스트를 입력할 위치로 커서를 이동하세요."},
};

struct LocalizedStringTable {
    LANGID language;
    const StringEntry* entries;
    size_t count;
};

const LocalizedStringTable kLocalizedStringTables[] = {
    {MAKELANGID(LANG_ITALIAN, SUBLANG_NEUTRAL),
     kStringsIt, ARRAYSIZE(kStringsIt)},
    {MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL),
     kStringsRu, ARRAYSIZE(kStringsRu)},
    {MAKELANGID(LANG_SPANISH, SUBLANG_NEUTRAL),
     kStringsEs, ARRAYSIZE(kStringsEs)},
    {MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN),
     kStringsPtBr, ARRAYSIZE(kStringsPtBr)},
    {MAKELANGID(LANG_FRENCH, SUBLANG_NEUTRAL),
     kStringsFr, ARRAYSIZE(kStringsFr)},
    {MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL),
     kStringsDe, ARRAYSIZE(kStringsDe)},
    {MAKELANGID(LANG_POLISH, SUBLANG_NEUTRAL),
     kStringsPl, ARRAYSIZE(kStringsPl)},
    {MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
     kStringsZhCn, ARRAYSIZE(kStringsZhCn)},
    {MAKELANGID(LANG_JAPANESE, SUBLANG_NEUTRAL),
     kStringsJa, ARRAYSIZE(kStringsJa)},
    {MAKELANGID(LANG_KOREAN, SUBLANG_NEUTRAL),
     kStringsKo, ARRAYSIZE(kStringsKo)},
};

// The table in use: English until SelectStringTable picks another one.
const StringEntry* g_strings = kStrings;
size_t g_stringsCount = ARRAYSIZE(kStrings);

// Picks the table of the Windows interface language: an exact language and
// sublanguage first, then the table of the language whatever the
// sublanguage (Portuguese without the Brazilian sublanguage, for example),
// and English for a language the mod does not translate.
void SelectStringTable() {
    LANGID language = GetUserDefaultUILanguage();
    const LocalizedStringTable* match = nullptr;
    bool exact = false;
    for (const LocalizedStringTable& candidate : kLocalizedStringTables) {
        if (PRIMARYLANGID(candidate.language) != PRIMARYLANGID(language)) {
            continue;
        }
        if (SUBLANGID(candidate.language) == SUBLANGID(language)) {
            match = &candidate;
            exact = true;
            break;
        }
        if (!match) {
            match = &candidate;
        }
    }
    if (match) {
        g_strings = match->entries;
        g_stringsCount = match->count;
    } else {
        g_strings = kStrings;
        g_stringsCount = ARRAYSIZE(kStrings);
    }
    Wh_Log(L"Strings: interface language 0x%04X, %s", language,
           match ? (exact ? L"translated" : L"translated (language match)")
                 : L"English (not translated)");
}

// The text of a string id in the selected language, with the English table
// of the official MUI as the fallback. A null return means the id is not a
// string this mod serves at all, and the caller keeps its own result.
PCWSTR LocalizedString(UINT id) {
    for (size_t i = 0; i < g_stringsCount; i++) {
        if (g_strings[i].id == id) {
            return g_strings[i].text;
        }
    }
    for (const StringEntry& entry : kStrings) {
        if (entry.id == id) {
            return entry.text;
        }
    }
    return nullptr;
}



// The names and the captions of the keys, from the same official resource file
// as the strings above (the Windows 7 `TipResX.dll`, blocks 938 to 966, the
// source of the ids 15003 and up). The resource module of the modern keyboard
// leaves holes in that table - the log of this mod shows them as
// `FindResourceExW: type=#6 id=#941 of tipresx.dll -> NOT FOUND` - and a key
// whose caption is missing is a key with nothing written on it, which is what
// is left of the extra keys of the keyboard when the layout is switched. They
// are served only when the module of the system has no string for the id.
struct KeyboardStringEntry {
    UINT id;
    PCWSTR text;
};

const KeyboardStringEntry kKeyboardStrings[] = {
    {15002, L"Esc"},
    {15003, L"Bksp"},
    {15004, L"AltGr"},
    {15005, L"Caps"},
    {15006, L"Shift"},
    {15007, L"Ctrl"},
    {15008, L"Alt"},
    {15009, L"Tab"},
    {15010, L"Home"},
    {15011, L"PgUp"},
    {15012, L"End"},
    {15013, L"PgDn"},
    {15014, L"Del"},
    {15015, L"PrtScn"},
    {15016, L"Insert"},
    {15017, L"Pause"},
    {15018, L"Fn"},
    {15019, L"ScrLk"},
    {15020, L"Convert"},
    {15022, L"Space"},
    {15023, L"Num"},
    {15026, L"Enter"},
    {15027, L"Options"},
    {15028, L"Help"},
    {15029, L"Escape"},
    {15030, L"Backspace"},
    {15031, L"Tab"},
    {15032, L"Enter"},
    {15033, L"Left Shift"},
    {15034, L"Right Shift"},
    {15035, L"Caps Lock"},
    {15036, L"Control"},
    {15037, L"Right Control"},
    {15038, L"Alt"},
    {15039, L"Right Alt"},
    {15040, L"AltGr"},
    {15041, L"Space"},
    {15042, L"Delete"},
    {15043, L"Context Menu"},
    {15044, L"Windows Key"},
    {15045, L"Down Arrow"},
    {15046, L"Up Arrow"},
    {15047, L"Left Arrow"},
    {15048, L"Right Arrow"},
    {15052, L"Function Mode"},
    {15053, L"Convert"},
    {15054, L"Korean"},
    {15055, L"Home"},
};

// ---------------------------------------------------------------------------
// The captions of the keys, corrected where they are blank.
//
// The keyboard does not ask LoadStringW for its captions. It reads the RT_STRING
// blocks 938 to 942 of TipResX.dll directly (sixteen captions per block: block N
// holds the ids (N - 1) * 16 to (N - 1) * 16 + 15, so 938 starts at 14992 and
// the keys use 15002 to 15071). Two things leave keys blank:
//  * the caption of Backspace (id 15003, scancode 0x0E) is a private-use
//    glyph that the keyboard font does not draw (on the modern module it shows
//    as a small box). The key always shows the Unicode sign ERASE TO THE LEFT
//    (U+232B) instead, which Windows draws with the system font fallback;
//  * a caption that is empty in the module is given the name of
//    kKeyboardStrings, when there is one.
// The blocks that still have blank captions are written to the log, with their
// ids, so that the next report shows which keys remain empty and why.
// ---------------------------------------------------------------------------

constexpr UINT kRtString = 6;
constexpr UINT kCaptionFirstBlock = 938;
constexpr UINT kCaptionLastBlock = 942;
constexpr UINT kCaptionBlockCount = kCaptionLastBlock - kCaptionFirstBlock + 1;
constexpr UINT kCaptionsPerBlock = 16;
constexpr UINT kBackspaceCaption = 15003;
// The sign of the Backspace key: U+232B ERASE TO THE LEFT. Segoe Fluent Icons
// draws the same key as BackSpaceQWERTY (E750, Microsoft Learn), but that code
// point is private use, which the keyboard font cannot draw in a caption.
constexpr wchar_t kBackspaceIcon = L'\u232B';

struct CaptionBlock {
    UINT block;
    bool built;
    bool ok;
    std::vector<BYTE> bytes;
};

// Allocated on the process heap and never freed, for the same reason as the
// face handles above: the handles served to the keyboard outlive this module.
CaptionBlock* g_captionBlocks = nullptr;

bool EnsureCaptionBlocks() {
    static std::once_flag once;
    std::call_once(once, []() {
        CaptionBlock* blocks =
            (CaptionBlock*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                     sizeof(CaptionBlock) * kCaptionBlockCount);
        if (!blocks) {
            Wh_Log(L"Could not allocate the caption blocks");
            return;
        }
        for (UINT i = 0; i < kCaptionBlockCount; i++) {
            new (&blocks[i]) CaptionBlock();
        }
        g_captionBlocks = blocks;
    });
    return g_captionBlocks != nullptr;
}
std::mutex g_captionMutex;

bool IsPrivateUseCaption(wchar_t c) {
    return c >= 0xE000 && c <= 0xF8FF;
}

// A caption that shows something: text that is not blank and not a
// private-use glyph.
bool HasVisibleCaption(const std::wstring& text) {
    for (wchar_t c : text) {
        if (!IsPrivateUseCaption(c) && c != L' ' && c != L'\t' && c != 0) {
            return true;
        }
    }
    return false;
}

// The names of the AltGr key, taken from Windows itself. The key is the right
// Alt key (scan code 0x38, extended), and the keyboard layout that has an AltGr
// names it in the language of the layout (GetKeyNameTextW, "the key name is
// localized to the keyboard layout of the calling thread"). The English name of
// kKeyboardStrings is only the last resort, for a layout that gives no name.
constexpr UINT kAltGrCaptionShort = 15004;
constexpr UINT kAltGrCaptionLong = 15040;

bool IsAltGrCaption(UINT id) {
    return id == kAltGrCaptionShort || id == kAltGrCaptionLong;
}

// A pointer that stays valid: std::map nodes never move, and a name is
// computed once per keyboard layout.
const wchar_t* AltGrNameFromWindows() {
    static std::mutex lock;
    static std::map<HKL, std::wstring> names;

    HKL layout = GetKeyboardLayout(0);
    std::lock_guard<std::mutex> guard(lock);
    auto found = names.find(layout);
    if (found == names.end()) {
        std::wstring name;
        wchar_t buffer[64] = {};
        // lParam of GetKeyNameText: scan code in bits 16-23, bit 24 = extended.
        const LONG lParam = (0x38 << 16) | (1 << 24);
        int length = GetKeyNameTextW(lParam, buffer, ARRAYSIZE(buffer));
        if (length > 0) {
            name.assign(buffer, (size_t)length);
        }
        if (!HasVisibleCaption(name)) {
            name.clear();
        }
        Wh_Log(L"AltGr: Windows names the key \"%s\" for this layout",
               name.empty() ? L"(nothing)" : name.c_str());
        found = names.emplace(layout, name).first;
    }
    return found->second.empty() ? nullptr : found->second.c_str();
}

const wchar_t* KeyboardCaptionName(UINT id) {
    if (IsAltGrCaption(id)) {
        if (const wchar_t* fromWindows = AltGrNameFromWindows()) {
            return fromWindows;
        }
    }
    for (const KeyboardStringEntry& entry : kKeyboardStrings) {
        if (entry.id == id) {
            return entry.text;
        }
    }
    return nullptr;
}

const CaptionBlock* AsCaptionBlock(const void* handle) {
    if (!EnsureCaptionBlocks()) {
        return nullptr;
    }
    uintptr_t address = (uintptr_t)handle;
    uintptr_t first = (uintptr_t)&g_captionBlocks[0];
    uintptr_t last = (uintptr_t)&g_captionBlocks[kCaptionBlockCount];
    if (address < first || address >= last) {
        return nullptr;
    }
    return (const CaptionBlock*)handle;
}

bool IsCaptionBlockHandle(const void* handle) {
    const CaptionBlock* block = AsCaptionBlock(handle);
    return block && block->ok;
}

const std::vector<BYTE>* CaptionBlockBits(const void* handle) {
    const CaptionBlock* block = AsCaptionBlock(handle);
    return block && block->ok ? &block->bytes : nullptr;
}

// TipResX.dll, wherever the keyboard finds it.
bool IsTipResModule(HMODULE module) {
    if (!module) {
        return false;
    }
    wchar_t path[32768];
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) {
        return false;
    }
    const wchar_t suffix[] = L"\\tipresx.dll";
    size_t length = wcslen(path);
    size_t suffixLength = ARRAYSIZE(suffix) - 1;
    return length >= suffixLength &&
           _wcsicmp(path + length - suffixLength, suffix) == 0;
}

// Reads the sixteen captions of a block: a count of UTF-16 units, then the text.
bool ReadCaptions(const std::vector<BYTE>& bytes,
                  std::vector<std::wstring>& captions) {
    size_t pos = 0;
    for (UINT i = 0; i < kCaptionsPerBlock; i++) {
        if (pos + 2 > bytes.size()) {
            return false;
        }
        size_t count = bytes[pos] | (bytes[pos + 1] << 8);
        pos += 2;
        if (pos + count * 2 > bytes.size()) {
            return false;
        }
        std::wstring text;
        for (size_t k = 0; k < count; k++) {
            text += (wchar_t)(bytes[pos + k * 2] | (bytes[pos + k * 2 + 1] << 8));
        }
        captions.push_back(text);
        pos += count * 2;
    }
    return true;
}

std::vector<BYTE> WriteCaptions(const std::vector<std::wstring>& captions) {
    std::vector<BYTE> out;
    for (const std::wstring& text : captions) {
        out.push_back((BYTE)(text.size() & 0xFF));
        out.push_back((BYTE)((text.size() >> 8) & 0xFF));
        for (wchar_t c : text) {
            out.push_back((BYTE)((unsigned)c & 0xFF));
            out.push_back((BYTE)(((unsigned)c >> 8) & 0xFF));
        }
    }
    return out;
}

// Copies the block of the keyboard module and corrects it. Called with
// g_captionMutex held.
bool BuildCaptionBlock(HMODULE module, HRSRC original, UINT block,
                       CaptionBlock& target) {
    DWORD size = SizeofResource_Original(module, original);
    HGLOBAL data = size ? LoadResource_Original(module, original) : nullptr;
    const BYTE* source = data ? (const BYTE*)LockResource_Original(data) : nullptr;
    if (!source) {
        Wh_Log(L"Captions: block %u could not be read (size %lu)", block,
               (unsigned long)size);
        return false;
    }
    std::vector<BYTE> bytes(source, source + size);
    std::vector<std::wstring> captions;
    if (!ReadCaptions(bytes, captions)) {
        Wh_Log(L"Captions: block %u does not have sixteen captions", block);
        return false;
    }

    const UINT first = (block - 1) * kCaptionsPerBlock;
    UINT named = 0;
    UINT backspace = 0;
    std::wstring blankIds;
    for (UINT i = 0; i < captions.size(); i++) {
        UINT id = first + i;
        std::wstring& text = captions[i];
        if (id == kBackspaceCaption) {
            text = std::wstring(1, kBackspaceIcon);
            backspace++;
            continue;
        }
        if (HasVisibleCaption(text)) {
            continue;
        }
        if (const wchar_t* name = KeyboardCaptionName(id)) {
            text = name;
            named++;
            continue;
        }
        if (id >= 15002 && id <= 15071) {
            if (!blankIds.empty()) {
                blankIds += L", ";
            }
            blankIds += std::to_wstring(id);
        }
    }

    target.bytes = WriteCaptions(captions);
    target.ok = true;
    Wh_Log(L"Captions: block %u (%u-%u): %u blank caption(s) named, Backspace "
           L"arrow %u, still blank: %s",
           block, first, first + kCaptionsPerBlock - 1, named, backspace,
           blankIds.empty() ? L"none" : blankIds.c_str());
    return true;
}

// The handle for a caption block of the keyboard module, or nullptr to keep
// the resource the system found.
HRSRC CaptionBlockFor(HMODULE module, LPCWSTR type, LPCWSTR name,
                      HRSRC original) {
    if (!g_oskProcess || !original) {
        return nullptr;
    }
    if ((ULONG_PTR)type >= 0x10000 || (UINT)(ULONG_PTR)type != kRtString) {
        return nullptr;
    }
    if ((ULONG_PTR)name >= 0x10000) {
        return nullptr;
    }
    UINT block = (UINT)(ULONG_PTR)name;
    if (block < kCaptionFirstBlock || block > kCaptionLastBlock) {
        return nullptr;
    }
    if (!IsTipResModule(module)) {
        return nullptr;
    }

    if (!EnsureCaptionBlocks()) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_captionMutex);
    CaptionBlock& slot = g_captionBlocks[block - kCaptionFirstBlock];
    if (!slot.built) {
        slot.built = true;
        slot.block = block;
        slot.ok = BuildCaptionBlock(module, original, block, slot);
    }
    return slot.ok ? (HRSRC)&slot : nullptr;
}

bool NormalizeKeyCaption(std::wstring& text);

// Writes a fallback string into the caller's buffer, with the behaviour
// Microsoft documents for LoadStringW: a buffer size of zero asks for the
// address of the string itself, and a buffer that is too small is filled and
// terminated. Returns the number of characters, or -1 when the id is unknown.
int WriteFallbackString(PCWSTR source, LPWSTR buffer, int bufferMax) {
    int length = (int)wcslen(source);
    if (bufferMax == 0) {
        if (!buffer) {
            return 0;
        }
        // The caller keeps the address it is given here (this is how
        // LoadStringW hands out a string), so it must not point into this
        // module: see ServePersistentString.
        *reinterpret_cast<PCWSTR*>(buffer) = ServePersistentString(source);
        return length;
    }
    if (!buffer || bufferMax < 0) {
        return 0;
    }

    std::wstring text(source);
    NormalizeKeyCaption(text);
    length = (int)text.size();

    int copied = length < bufferMax - 1 ? length : bufferMax - 1;
    wmemcpy(buffer, text.c_str(), (size_t)copied);
    buffer[copied] = L'\0';
    return copied;
}

using LoadStringW_t = int(WINAPI*)(HINSTANCE, UINT, LPWSTR, int);
LoadStringW_t LoadStringW_Original;

RateLimit g_missingStringLimit;

// ---------------------------------------------------------------------------
// Key captions: the Windows 7 keyboard draws the face of the keys that are not
// plain letters with a glyph taken from the font installed on the system. On
// Windows 10 and 11 some of those code points are private-use characters of the
// current icon font (the Windows logo, for instance) or characters that the
// font of this system does not draw at all, and the key comes out empty.
//
// Two things are done here:
//   * a private-use character is never passed through: the Windows key gets the
//     face chosen in the settings, every other one is dropped;
//   * every caption that reaches the keyboard is logged once, with its id, so
//     that the exact id of each special key can be read from the log.
// ---------------------------------------------------------------------------

bool IsPrivateUseCharacter(wchar_t character) {
    unsigned int code = (unsigned int)character;
    return (code >= 0xE000 && code <= 0xF8FF) ||
           (code >= 0xF0000 && code <= 0xFFFFD);
}

std::wstring WindowsKeyFaceText() {
    if (EqualsNoCase(g_windowsKeyFace.c_str(), L"keep")) {
        return L"";  // empty means: leave this character alone
    }
    if (EqualsNoCase(g_windowsKeyFace.c_str(), L"win")) {
        return L"Win";
    }
    return L"\u229E";  // squared plus, the classic placeholder for this key
}

// Replaces the private-use characters of a caption. Returns false when the
// text was not changed.
bool NormalizeKeyCaption(std::wstring& text) {
    bool changed = false;
    std::wstring result;
    result.reserve(text.size() + 4);

    for (wchar_t character : text) {
        if (!IsPrivateUseCharacter(character)) {
            result.push_back(character);
            continue;
        }

        // The Windows logo and its neighbours: this is the glyph of the
        // Windows key (0xE15A) and the ones the icon fonts use around it.
        bool isWindowsKey =
            (unsigned int)character == 0xE15A || (unsigned int)character == 0xE7AD;
        if (isWindowsKey) {
            std::wstring replacement = WindowsKeyFaceText();
            if (!replacement.empty()) {
                result += replacement;
                changed = true;
                continue;
            }
            result.push_back(character);
            continue;
        }

        // Anything else cannot be drawn by the fonts of this system.
        changed = true;
        static std::atomic<int> logLimit{0};
        if (logLimit.fetch_add(1, std::memory_order_relaxed) < 20) {
            Wh_Log(L"Caption: the private-use character U+%04X cannot be drawn "
                   L"by the fonts of this system and was dropped",
                   (unsigned int)character);
        }
    }

    if (changed) {
        text.swap(result);
    }
    return changed;
}

// Logs each caption of the keyboard once, so that the id of every special key
// can be read straight from the log (the table of the fallback strings alone
// does not say which id belongs to which key).
void LogCaptionOnce(UINT id, PCWSTR text, int length) {
    constexpr int kMaxLogged = 64;
    static std::atomic<int> loggedCount{0};
    static UINT loggedIds[kMaxLogged];
    static std::atomic<int> loggedSize{0};

    int size = loggedSize.load(std::memory_order_relaxed);
    for (int i = 0; i < size; i++) {
        if (loggedIds[i] == id) {
            return;
        }
    }
    if (loggedCount.fetch_add(1, std::memory_order_relaxed) >= kMaxLogged) {
        return;
    }
    if (size < kMaxLogged) {
        loggedIds[size] = id;
        loggedSize.store(size + 1, std::memory_order_relaxed);
    }

    std::wstring escaped;
    escaped.reserve((size_t)length * 8 + 8);
    for (int i = 0; i < length && text && text[i]; i++) {
        wchar_t c = text[i];
        wchar_t buffer[16];
        if (c >= 0x20 && c < 0x7F) {
            swprintf(buffer, ARRAYSIZE(buffer), L"%c", c);
        } else {
            swprintf(buffer, ARRAYSIZE(buffer), L"[U+%04X]", (unsigned int)c);
        }
        escaped += buffer;
    }

    if (length > 0) {
        Wh_Log(L"Caption %u: \"%s\"", id, escaped.c_str());
    } else {
        Wh_Log(L"Caption %u: (empty)", id);
    }
}

// A string is rewritten only when it comes from the modules of the keyboard:
// dropping a glyph of some other component's text would be out of place.
bool IsKeyboardModule(HINSTANCE instance) {
    if (!instance) {
        return false;
    }
    if (instance == g_exeModule) {
        return true;
    }

    wchar_t path[32768];
    if (!GetModuleFileNameW((HMODULE)instance, path, ARRAYSIZE(path))) {
        return false;
    }

    std::wstring moduleDir = GetInstallDir();
    size_t length = moduleDir.size();
    if (length == 0 || _wcsnicmp(path, moduleDir.c_str(), length) != 0) {
        return false;
    }
    // Only the files of the install folder, not a longer name that starts the
    // same way.
    return path[length] == L'\\' || path[length] == L'/';
}

int WINAPI LoadStringW_Hook(HINSTANCE instance, UINT id, LPWSTR buffer,
                            int bufferMax) {
    int result = LoadStringW_Original(instance, id, buffer, bufferMax);
    if (result > 0) {
        if (g_oskProcess && buffer && bufferMax != 0 && g_diagnostics) {
            LogCaptionOnce(id, buffer, result);
        }
        if (g_oskProcess && buffer && bufferMax != 0 &&
            IsKeyboardModule(instance)) {
            // The caption comes from the resources of this system (usually the
            // strings of the keyboard of Windows 10/11): replace the glyphs
            // this system cannot draw.
            std::wstring text(buffer, (size_t)result);
            if (NormalizeKeyCaption(text)) {
                int copied = (int)text.size();
                if (copied > bufferMax - 1) {
                    copied = bufferMax - 1;
                }
                wmemcpy(buffer, text.c_str(), (size_t)copied);
                buffer[copied] = L'\0';
                return copied;
            }
        }
        return result;
    }

    // Nothing in the resources of this system: the captions of the keys live in
    // the resource module of the keyboard (`TipResX.dll`), which on Windows 10
    // and 11 is the module of the modern keyboard and does not define all of
    // the ids. Writing nothing on a key is worse than writing the name the
    // original keyboard uses.
    if (g_oskProcess && id >= kKeyboardStrings[0].id) {
        for (const KeyboardStringEntry& entry : kKeyboardStrings) {
            if (entry.id != id) {
                continue;
            }
            static std::mutex servedLock;
            static bool served[64]{};
            bool first = false;
            {
                std::lock_guard<std::mutex> guard(servedLock);
                size_t slot = (size_t)(id - kKeyboardStrings[0].id);
                if (slot < ARRAYSIZE(served) && !served[slot]) {
                    served[slot] = true;
                    first = true;
                }
            }
            PCWSTR text = KeyboardCaptionName(id);
            if (!text) {
                text = entry.text;
            }
            if (first) {
                Wh_Log(L"String %u is not in the resources of this system; "
                       L"the name \"%s\" is used",
                       id, text);
            }
            return WriteFallbackString(text, buffer, bufferMax);
        }
    }

    if (PCWSTR text = LocalizedString(id)) {
        return WriteFallbackString(text, buffer, bufferMax);
    }

    if (instance && instance != g_exeModule) {
        return result;
    }

    if (g_missingStringLimit.Allow(500)) {
        Wh_Log(L"String %u is not available: the official osk.exe.mui does not "
               L"define it either (its table covers 1009 to 1072, the first "
               L"non-empty string is 1017), so this is normal",
               id);
    }
    return result;
}

// The three dialog templates, byte for byte from the official Windows 7
// osk.exe.mui: 5000 (Options), 5001 (About), 5002 (Ctrl+Alt+Del message).
// They are passed straight to DialogBoxIndirectParamW.
// Dialog resource 5000, 2096 bytes, extracted verbatim from the
// Windows 7 osk.exe.mui (RT_DIALOG). Used with DialogBoxIndirectParamW.
alignas(4) const BYTE kDialog5000[] = {
    0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0xC8, 0x80, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x00,
    0x40, 0x01, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x00, 0x70, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x4D, 0x00, 0x53, 0x00, 0x20, 0x00, 0x53, 0x00,
    0x68, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x44, 0x00,
    0x6C, 0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x50, 0x10, 0x00, 0x05, 0x00,
    0x54, 0x00, 0x0A, 0x00, 0x93, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00,
    0x26, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x20, 0x00, 0x63, 0x00,
    0x6C, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x20, 0x00, 0x73, 0x00,
    0x6F, 0x00, 0x75, 0x00, 0x6E, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x50,
    0x10, 0x00, 0x11, 0x00, 0x78, 0x00, 0x0A, 0x00, 0xA9, 0x13, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x54, 0x00, 0x75, 0x00, 0x72, 0x00, 0x6E, 0x00,
    0x20, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x6E, 0x00, 0x75, 0x00,
    0x6D, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x63, 0x00, 0x20, 0x00,
    0x6B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x20, 0x00, 0x70, 0x00, 0x61, 0x00,
    0x26, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x50, 0x08, 0x00, 0x20, 0x00,
    0xCF, 0x00, 0xBE, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00,
    0x54, 0x00, 0x6F, 0x00, 0x20, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x20, 0x00, 0x4F, 0x00,
    0x6E, 0x00, 0x2D, 0x00, 0x53, 0x00, 0x63, 0x00, 0x72, 0x00, 0x65, 0x00,
    0x65, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00,
    0x62, 0x00, 0x6F, 0x00, 0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x3A, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x03, 0x50, 0x14, 0x00, 0x2B, 0x00, 0x54, 0x00, 0x0A, 0x00,
    0x94, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x26, 0x00, 0x43, 0x00,
    0x6C, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x20, 0x00, 0x6F, 0x00,
    0x6E, 0x00, 0x20, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x73, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x01, 0x50, 0x14, 0x00, 0x38, 0x00, 0x54, 0x00, 0x0A, 0x00,
    0x95, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x26, 0x00, 0x48, 0x00,
    0x6F, 0x00, 0x76, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x6F, 0x00,
    0x76, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x6B, 0x00, 0x65, 0x00,
    0x79, 0x00, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0x50, 0x14, 0x00, 0x69, 0x00,
    0x54, 0x00, 0x0A, 0x00, 0x96, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00,
    0x26, 0x00, 0x53, 0x00, 0x63, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x20, 0x00,
    0x74, 0x00, 0x68, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x67, 0x00,
    0x68, 0x00, 0x20, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x73, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x20, 0x00, 0x47, 0x00, 0x3C, 0x00, 0x08, 0x00,
    0x97, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x48, 0x00, 0x26, 0x00,
    0x6F, 0x00, 0x76, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x64, 0x00,
    0x75, 0x00, 0x72, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00,
    0x6E, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x58, 0x3B, 0x00, 0x50, 0x00,
    0x79, 0x00, 0x14, 0x00, 0x99, 0x13, 0x00, 0x00, 0x6D, 0x00, 0x73, 0x00,
    0x63, 0x00, 0x74, 0x00, 0x6C, 0x00, 0x73, 0x00, 0x5F, 0x00, 0x74, 0x00,
    0x72, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6B, 0x00, 0x62, 0x00, 0x61, 0x00,
    0x72, 0x00, 0x33, 0x00, 0x32, 0x00, 0x00, 0x00, 0x48, 0x00, 0x6F, 0x00,
    0x76, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x64, 0x00, 0x75, 0x00,
    0x72, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x20, 0x00, 0x54, 0x00, 0x1A, 0x00, 0x08, 0x00,
    0xEF, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x53, 0x00, 0x68, 0x00,
    0x6F, 0x00, 0x72, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x6C, 0x00, 0x64, 0x00, 0x32, 0x00, 0x08, 0x00,
    0x98, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50,
    0xB8, 0x00, 0x54, 0x00, 0x1A, 0x00, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x82, 0x00, 0x4C, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x67, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x20, 0x00, 0x78, 0x00,
    0x3C, 0x00, 0x08, 0x00, 0x9A, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00,
    0x53, 0x00, 0x63, 0x00, 0x61, 0x00, 0x26, 0x00, 0x6E, 0x00, 0x6E, 0x00,
    0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20, 0x00, 0x73, 0x00, 0x70, 0x00,
    0x65, 0x00, 0x65, 0x00, 0x64, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x58,
    0x3B, 0x00, 0x81, 0x00, 0x79, 0x00, 0x14, 0x00, 0x9C, 0x13, 0x00, 0x00,
    0x6D, 0x00, 0x73, 0x00, 0x63, 0x00, 0x74, 0x00, 0x6C, 0x00, 0x73, 0x00,
    0x5F, 0x00, 0x74, 0x00, 0x72, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6B, 0x00,
    0x62, 0x00, 0x61, 0x00, 0x72, 0x00, 0x33, 0x00, 0x32, 0x00, 0x00, 0x00,
    0x53, 0x00, 0x63, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x6E, 0x00, 0x69, 0x00,
    0x6E, 0x00, 0x67, 0x00, 0x20, 0x00, 0x73, 0x00, 0x70, 0x00, 0x65, 0x00,
    0x65, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x20, 0x00, 0x85, 0x00,
    0x1A, 0x00, 0x08, 0x00, 0xF1, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00,
    0x46, 0x00, 0x61, 0x00, 0x73, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x6C, 0x00, 0x97, 0x00, 0x32, 0x00, 0x08, 0x00,
    0x9B, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50,
    0xB8, 0x00, 0x85, 0x00, 0x1A, 0x00, 0x08, 0x00, 0xF2, 0x03, 0x00, 0x00,
    0xFF, 0xFF, 0x82, 0x00, 0x53, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x77, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x20, 0x00, 0xA2, 0x00,
    0x3C, 0x00, 0x08, 0x00, 0xF3, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00,
    0x54, 0x00, 0x6F, 0x00, 0x20, 0x00, 0x73, 0x00, 0x65, 0x00, 0x6C, 0x00,
    0x65, 0x00, 0x63, 0x00, 0x74, 0x00, 0x20, 0x00, 0x61, 0x00, 0x20, 0x00,
    0x6B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x50,
    0x20, 0x00, 0xAE, 0x00, 0xA0, 0x00, 0x0A, 0x00, 0x9F, 0x13, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x20, 0x00,
    0x6A, 0x00, 0x6F, 0x00, 0x79, 0x00, 0x73, 0x00, 0x74, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x6B, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x67, 0x00, 0x61, 0x00,
    0x26, 0x00, 0x6D, 0x00, 0x65, 0x00, 0x20, 0x00, 0x70, 0x00, 0x61, 0x00,
    0x64, 0x00, 0x20, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x20, 0x00, 0x6F, 0x00,
    0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x67, 0x00,
    0x61, 0x00, 0x6D, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20, 0x00,
    0x64, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x01, 0x50, 0x20, 0x00, 0xBD, 0x00, 0x46, 0x00, 0x0A, 0x00,
    0xA0, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x55, 0x00, 0x73, 0x00,
    0x65, 0x00, 0x20, 0x00, 0x26, 0x00, 0x6B, 0x00, 0x65, 0x00, 0x79, 0x00,
    0x62, 0x00, 0x6F, 0x00, 0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x20, 0x00,
    0x6B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40,
    0x6C, 0x00, 0xBB, 0x00, 0x01, 0x00, 0x0C, 0x00, 0x9D, 0x13, 0x00, 0x00,
    0xFF, 0xFF, 0x82, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x62, 0x00,
    0x6F, 0x00, 0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x20, 0x00, 0x73, 0x00,
    0x63, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x6B, 0x00, 0x65, 0x00,
    0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x21, 0x50, 0x6E, 0x00, 0xBB, 0x00,
    0x48, 0x00, 0x35, 0x00, 0x9E, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x01, 0x50, 0x20, 0x00, 0xCC, 0x00, 0x46, 0x00, 0x0A, 0x00,
    0xA6, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x55, 0x00, 0x73, 0x00,
    0x65, 0x00, 0x20, 0x00, 0x6D, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x73, 0x00,
    0x26, 0x00, 0x65, 0x00, 0x20, 0x00, 0x63, 0x00, 0x6C, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x02, 0x50, 0x08, 0x00, 0xE4, 0x00,
    0xCF, 0x00, 0x29, 0x00, 0xA5, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00,
    0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x20, 0x00, 0x70, 0x00,
    0x72, 0x00, 0x65, 0x00, 0x64, 0x00, 0x69, 0x00, 0x63, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x50,
    0x12, 0x00, 0xF0, 0x00, 0xA0, 0x00, 0x0A, 0x00, 0xA3, 0x13, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x20, 0x00,
    0x26, 0x00, 0x54, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x20, 0x00,
    0x50, 0x00, 0x72, 0x00, 0x65, 0x00, 0x64, 0x00, 0x69, 0x00, 0x63, 0x00,
    0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x50,
    0x1C, 0x00, 0xFE, 0x00, 0xA0, 0x00, 0x0A, 0x00, 0xA4, 0x13, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x73, 0x00, 0x65, 0x00,
    0x72, 0x00, 0x74, 0x00, 0x20, 0x00, 0x73, 0x00, 0x70, 0x00, 0x61, 0x00,
    0x63, 0x00, 0x65, 0x00, 0x20, 0x00, 0x61, 0x00, 0x66, 0x00, 0x74, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65, 0x00,
    0x64, 0x00, 0x69, 0x00, 0x63, 0x00, 0x74, 0x00, 0x65, 0x00, 0x64, 0x00,
    0x20, 0x00, 0x26, 0x00, 0x77, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x64, 0x00,
    0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x50, 0x08, 0x00, 0x14, 0x01,
    0xC8, 0x00, 0x0A, 0x00, 0xA7, 0x13, 0x00, 0x00, 0x53, 0x00, 0x79, 0x00,
    0x73, 0x00, 0x4C, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x6B, 0x00, 0x00, 0x00,
    0x3C, 0x00, 0x41, 0x00, 0x3E, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6E, 0x00,
    0x74, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x77, 0x00,
    0x68, 0x00, 0x65, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x72, 0x00,
    0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x20, 0x00, 0x4F, 0x00,
    0x6E, 0x00, 0x2D, 0x00, 0x53, 0x00, 0x63, 0x00, 0x72, 0x00, 0x65, 0x00,
    0x65, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00,
    0x62, 0x00, 0x6F, 0x00, 0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x20, 0x00,
    0x73, 0x00, 0x74, 0x00, 0x61, 0x00, 0x72, 0x00, 0x74, 0x00, 0x73, 0x00,
    0x20, 0x00, 0x77, 0x00, 0x68, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x20, 0x00,
    0x49, 0x00, 0x20, 0x00, 0x6C, 0x00, 0x6F, 0x00, 0x67, 0x00, 0x20, 0x00,
    0x6F, 0x00, 0x6E, 0x00, 0x3C, 0x00, 0x2F, 0x00, 0x41, 0x00, 0x3E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x50, 0x08, 0x00, 0x1F, 0x01, 0xC8, 0x00, 0x0A, 0x00,
    0xA1, 0x13, 0x00, 0x00, 0x53, 0x00, 0x79, 0x00, 0x73, 0x00, 0x4C, 0x00,
    0x69, 0x00, 0x6E, 0x00, 0x6B, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x41, 0x00,
    0x3E, 0x00, 0x41, 0x00, 0x62, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x74, 0x00,
    0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x20, 0x00, 0x4F, 0x00,
    0x6E, 0x00, 0x2D, 0x00, 0x53, 0x00, 0x63, 0x00, 0x72, 0x00, 0x65, 0x00,
    0x65, 0x00, 0x6E, 0x00, 0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00,
    0x62, 0x00, 0x6F, 0x00, 0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x3C, 0x00,
    0x2F, 0x00, 0x41, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x50,
    0x6F, 0x00, 0x2D, 0x01, 0x32, 0x00, 0x0E, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x4F, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x50,
    0xA6, 0x00, 0x2D, 0x01, 0x32, 0x00, 0x0E, 0x00, 0x02, 0x00, 0x00, 0x00,
    0xFF, 0xFF, 0x80, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x63, 0x00,
    0x65, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Dialog resource 5001, 662 bytes, extracted verbatim from the
// Windows 7 osk.exe.mui (RT_DIALOG). Used with DialogBoxIndirectParamW.
alignas(4) const BYTE kDialog5001[] = {
    0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0xC8, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x01,
    0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x00, 0x62, 0x00, 0x6F, 0x00,
    0x75, 0x00, 0x74, 0x00, 0x20, 0x00, 0x4F, 0x00, 0x6E, 0x00, 0x2D, 0x00,
    0x53, 0x00, 0x63, 0x00, 0x72, 0x00, 0x65, 0x00, 0x65, 0x00, 0x6E, 0x00,
    0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x62, 0x00, 0x6F, 0x00,
    0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x4D, 0x00, 0x53, 0x00, 0x20, 0x00, 0x53, 0x00, 0x68, 0x00,
    0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6C, 0x00,
    0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x50, 0xD0, 0x00, 0x44, 0x00, 0x2E, 0x00, 0x0E, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x4F, 0x00, 0x4B, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x0A, 0x00, 0x0A, 0x00, 0xA4, 0x00, 0x08, 0x00,
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x4D, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x6F, 0x00, 0x66, 0x00,
    0x74, 0x00, 0x20, 0x00, 0x4F, 0x00, 0x6E, 0x00, 0x2D, 0x00, 0x53, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x65, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x20, 0x00,
    0x4B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x62, 0x00, 0x6F, 0x00, 0x61, 0x00,
    0x72, 0x00, 0x64, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50,
    0x0A, 0x00, 0x14, 0x00, 0xEF, 0x00, 0x08, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0xFF, 0xFF, 0x82, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x70, 0x00, 0x79, 0x00,
    0x72, 0x00, 0x69, 0x00, 0x67, 0x00, 0x68, 0x00, 0x74, 0x00, 0x20, 0x00,
    0xA9, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x69, 0x00, 0x63, 0x00, 0x72, 0x00,
    0x6F, 0x00, 0x73, 0x00, 0x6F, 0x00, 0x66, 0x00, 0x74, 0x00, 0x20, 0x00,
    0x43, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x70, 0x00, 0x6F, 0x00, 0x72, 0x00,
    0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x0A, 0x00, 0x28, 0x00, 0xEF, 0x00, 0x10, 0x00,
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x50, 0x00, 0x6F, 0x00,
    0x72, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x73, 0x00,
    0x20, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x70, 0x00, 0x79, 0x00, 0x72, 0x00,
    0x69, 0x00, 0x67, 0x00, 0x68, 0x00, 0x74, 0x00, 0x20, 0x00, 0x4D, 0x00,
    0x61, 0x00, 0x64, 0x00, 0x65, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x65, 0x00,
    0x63, 0x00, 0x20, 0x00, 0x4C, 0x00, 0x69, 0x00, 0x6D, 0x00, 0x69, 0x00,
    0x74, 0x00, 0x65, 0x00, 0x64, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x46, 0x00,
    0x6F, 0x00, 0x72, 0x00, 0x20, 0x00, 0x6D, 0x00, 0x6F, 0x00, 0x72, 0x00,
    0x65, 0x00, 0x20, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x66, 0x00, 0x6F, 0x00,
    0x72, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00, 0x6F, 0x00,
    0x6E, 0x00, 0x20, 0x00, 0x61, 0x00, 0x62, 0x00, 0x6F, 0x00, 0x75, 0x00,
    0x74, 0x00, 0x20, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x64, 0x00, 0x65, 0x00,
    0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x63, 0x00, 0x20, 0x00, 0x61, 0x00,
    0x6E, 0x00, 0x64, 0x00, 0x20, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x74, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x74, 0x00, 0x69, 0x00,
    0x76, 0x00, 0x65, 0x00, 0x20, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x70, 0x00,
    0x75, 0x00, 0x74, 0x00, 0x20, 0x00, 0x64, 0x00, 0x65, 0x00, 0x76, 0x00,
    0x69, 0x00, 0x63, 0x00, 0x65, 0x00, 0x73, 0x00, 0x2C, 0x00, 0x20, 0x00,
    0x73, 0x00, 0x65, 0x00, 0x65, 0x00, 0x3A, 0x00, 0x20, 0x00, 0x68, 0x00,
    0x74, 0x00, 0x74, 0x00, 0x70, 0x00, 0x3A, 0x00, 0x2F, 0x00, 0x2F, 0x00,
    0x77, 0x00, 0x77, 0x00, 0x77, 0x00, 0x2E, 0x00, 0x6D, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x6F, 0x00, 0x66, 0x00,
    0x74, 0x00, 0x2E, 0x00, 0x63, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x2F, 0x00,
    0x65, 0x00, 0x6E, 0x00, 0x61, 0x00, 0x62, 0x00, 0x6C, 0x00, 0x65, 0x00,
    0x2F, 0x00, 0x6F, 0x00, 0x73, 0x00, 0x6B, 0x00, 0x2E, 0x00, 0x00, 0x00,
    0x00, 0x00,
};

// Dialog resource 5002, 324 bytes, extracted verbatim from the
// Windows 7 osk.exe.mui (RT_DIALOG). Used with DialogBoxIndirectParamW.
alignas(4) const BYTE kDialog5002[] = {
    0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x00, 0xC8, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01,
    0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x00, 0x6E, 0x00, 0x2D, 0x00,
    0x53, 0x00, 0x63, 0x00, 0x72, 0x00, 0x65, 0x00, 0x65, 0x00, 0x6E, 0x00,
    0x20, 0x00, 0x4B, 0x00, 0x65, 0x00, 0x79, 0x00, 0x62, 0x00, 0x6F, 0x00,
    0x61, 0x00, 0x72, 0x00, 0x64, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x4D, 0x00, 0x53, 0x00, 0x20, 0x00, 0x53, 0x00, 0x68, 0x00,
    0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6C, 0x00,
    0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x50, 0xC8, 0x00, 0x28, 0x00, 0x32, 0x00, 0x0E, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x4F, 0x00, 0x4B, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x50, 0x0A, 0x00, 0x0A, 0x00, 0xE6, 0x00, 0x14, 0x00,
    0xA2, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x54, 0x00, 0x6F, 0x00,
    0x20, 0x00, 0x61, 0x00, 0x63, 0x00, 0x63, 0x00, 0x65, 0x00, 0x73, 0x00,
    0x73, 0x00, 0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x20, 0x00,
    0x63, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x6D, 0x00, 0x61, 0x00, 0x6E, 0x00,
    0x64, 0x00, 0x73, 0x00, 0x20, 0x00, 0x61, 0x00, 0x76, 0x00, 0x61, 0x00,
    0x69, 0x00, 0x6C, 0x00, 0x61, 0x00, 0x62, 0x00, 0x6C, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x62, 0x00, 0x79, 0x00, 0x20, 0x00, 0x70, 0x00, 0x72, 0x00,
    0x65, 0x00, 0x73, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00,
    0x20, 0x00, 0x43, 0x00, 0x54, 0x00, 0x52, 0x00, 0x4C, 0x00, 0x2B, 0x00,
    0x41, 0x00, 0x4C, 0x00, 0x54, 0x00, 0x2B, 0x00, 0x44, 0x00, 0x45, 0x00,
    0x4C, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x75, 0x00, 0x73, 0x00, 0x65, 0x00,
    0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00, 0x20, 0x00, 0x53, 0x00,
    0x74, 0x00, 0x61, 0x00, 0x72, 0x00, 0x74, 0x00, 0x20, 0x00, 0x6D, 0x00,
    0x65, 0x00, 0x6E, 0x00, 0x75, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00,
};

struct FallbackDialog {
    WORD id;
    const BYTE* data;
    DWORD size;
};

const FallbackDialog kFallbackDialogs[] = {
    {5000, kDialog5000, (DWORD)sizeof(kDialog5000)},
    {5001, kDialog5001, (DWORD)sizeof(kDialog5001)},
    {5002, kDialog5002, (DWORD)sizeof(kDialog5002)},
};

const FallbackDialog* FindFallbackDialog(WORD id) {
    for (const auto& dialog : kFallbackDialogs) {
        if (dialog.id == id) {
            return &dialog;
        }
    }
    return nullptr;
}

bool NeedsFallbackDialog(HINSTANCE instance, PCWSTR templateName) {
    if (!IS_INTRESOURCE(templateName) || (instance && instance != g_exeModule)) {
        return false;
    }
    if (FindResourceW(g_exeModule, templateName, RT_DIALOG)) {
        return false;  // a real .mui is present
    }
    return FindFallbackDialog((WORD)(ULONG_PTR)templateName) != nullptr;
}

using DialogBoxParamW_t = INT_PTR(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC,
                                           LPARAM);
DialogBoxParamW_t DialogBoxParamW_Original;

INT_PTR WINAPI DialogBoxParamW_Hook(HINSTANCE instance, LPCWSTR templateName,
                                    HWND parent, DLGPROC proc, LPARAM param) {
    if (NeedsFallbackDialog(instance, templateName)) {
        const FallbackDialog* dialog =
            FindFallbackDialog((WORD)(ULONG_PTR)templateName);
        Wh_Log(L"DialogBoxParam: using the built-in dialog %u (%lu bytes)",
               dialog->id, dialog->size);
        return DialogBoxIndirectParamW(
            g_exeModule, reinterpret_cast<LPCDLGTEMPLATEW>(
                             const_cast<BYTE*>(dialog->data)),
            parent, proc, param);
    }
    return DialogBoxParamW_Original(instance, templateName, parent, proc, param);
}

using CreateDialogParamW_t = HWND(WINAPI*)(HINSTANCE, LPCWSTR, HWND, DLGPROC,
                                           LPARAM);
CreateDialogParamW_t CreateDialogParamW_Original;

HWND WINAPI CreateDialogParamW_Hook(HINSTANCE instance, LPCWSTR templateName,
                                    HWND parent, DLGPROC proc, LPARAM param) {
    if (NeedsFallbackDialog(instance, templateName)) {
        const FallbackDialog* dialog =
            FindFallbackDialog((WORD)(ULONG_PTR)templateName);
        Wh_Log(L"CreateDialogParam: using the built-in dialog %u (%lu bytes)",
               dialog->id, dialog->size);
        return CreateDialogIndirectParamW(
            g_exeModule, reinterpret_cast<LPCDLGTEMPLATEW>(
                             const_cast<BYTE*>(dialog->data)),
            parent, proc, param);
    }
    return CreateDialogParamW_Original(instance, templateName, parent, proc,
                                       param);
}

using MessageBoxW_t = int(WINAPI*)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t MessageBoxW_Original;

int WINAPI MessageBoxW_Hook(HWND window, LPCWSTR text, LPCWSTR caption,
                            UINT type) {
    Wh_Log(L"MessageBox: %s - %s", caption ? caption : L"",
           text ? text : L"");
    return MessageBoxW_Original(window, text, caption, type);
}

using ExitProcess_t = void(WINAPI*)(UINT);
ExitProcess_t ExitProcess_Original;

void WINAPI ExitProcess_Hook(UINT exitCode) {
    Wh_Log(L"ExitProcess(%s) after %llu ms, called from %s",
           DescribeExitCode(exitCode).c_str(),
           (unsigned long long)(GetTickCount64() - g_startTick),
           DescribeCallerModule(MOD_CALLER_ADDRESS()).c_str());
    ExitProcess_Original(exitCode);
}

using TerminateProcess_t = BOOL(WINAPI*)(HANDLE, UINT);
TerminateProcess_t TerminateProcess_Original;

BOOL WINAPI TerminateProcess_Hook(HANDLE process, UINT exitCode) {
    if (process == GetCurrentProcess()) {
        Wh_Log(L"TerminateProcess(self, %s) after %llu ms, called from %s",
               DescribeExitCode(exitCode).c_str(),
               (unsigned long long)(GetTickCount64() - g_startTick),
               DescribeCallerModule(MOD_CALLER_ADDRESS()).c_str());
    }
    return TerminateProcess_Original(process, exitCode);
}

void PreloadLocalDlls() {
    std::wstring installDir = GetInstallDir();
    for (PCWSTR name : {L"dui71.dll", L"msswch.dll"}) {
        std::wstring path = JoinPath(installDir, name);
        if (!FileExists(path)) {
            continue;
        }
        if (GetModuleHandleW(name)) {
            Wh_Log(L"Preload: %s is already loaded, keeping the loaded copy",
                   name);
            continue;
        }
        HMODULE module = LoadLibraryW(path.c_str());
        Wh_Log(L"Preload: %s -> %s", path.c_str(), module ? L"loaded" : L"failed");
    }
}

// ---------------------------------------------------------------------------
// Windows 7 osk.exe side: the title of the main window
// ---------------------------------------------------------------------------

// The Windows 7 keyboard reads the title of its window from osk.exe.mui, which
// is not on the symbol server, and it does not always do it through a call
// that the string fallback above can answer. Whatever the way it asks, a main
// window that ends up with an empty caption gets the original title (string
// 1001 of the official MUI).
//
// The window is created once, so the title is filled in when it is created
// (CreateWindowExW below) instead of polling for it: the thread that asked
// every window of the process for its caption every 300 ms, for the whole life
// of the keyboard, woke the keyboard up for nothing. Only top-level,
// captioned, unowned windows of this process with no title at all are touched:
// nothing that has a title is changed.
void FixWindowTitle(HWND window) {
    if (!window) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId() ||
        GetWindow(window, GW_OWNER) != nullptr) {
        return;
    }
    LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION || (style & WS_CHILD)) {
        return;
    }
    DWORD_PTR length = 0;
    if (!SendMessageTimeoutW(window, WM_GETTEXTLENGTH, 0, 0,
                             SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, &length) ||
        length != 0) {
        return;
    }

    PCWSTR title = LocalizedString(1001);
    if (!title) {
        return;
    }
    DWORD_PTR ignored = 0;
    if (SendMessageTimeoutW(window, WM_SETTEXT, 0,
                            reinterpret_cast<LPARAM>(title),
                            SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, &ignored)) {
        static std::atomic<int> logged{0};
        if (logged.fetch_add(1, std::memory_order_relaxed) == 0) {
            Wh_Log(L"Window title: the main window had no title, \"%s\" was "
                   L"set",
                   title);
        }
    }
}

// The windows that already exist when the hooks are installed: the keyboard
// may have created its main window before this mod was loaded into it.
BOOL CALLBACK TitleFixEnumProc(HWND window, LPARAM) {
    FixWindowTitle(window);
    return TRUE;
}

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int,
                                        int, int, int, HWND, HMENU, HINSTANCE,
                                        LPVOID);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent,
                                 HMENU menu, HINSTANCE instance,
                                 LPVOID param) {
    HWND window =
        CreateWindowExW_Original(exStyle, className, windowName, style, x, y,
                                 width, height, parent, menu, instance, param);
    FixWindowTitle(window);
    return window;
}

void HookOskProcess() {
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LoadLibraryW",
                           reinterpret_cast<void*>(LoadLibraryW_Hook),
                           reinterpret_cast<void**>(&LoadLibraryW_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LoadLibraryExW",
                           reinterpret_cast<void*>(LoadLibraryExW_Hook),
                           reinterpret_cast<void**>(&LoadLibraryExW_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LoadLibraryA",
                           reinterpret_cast<void*>(LoadLibraryA_Hook),
                           reinterpret_cast<void**>(&LoadLibraryA_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LoadLibraryExA",
                           reinterpret_cast<void*>(LoadLibraryExA_Hook),
                           reinterpret_cast<void**>(&LoadLibraryExA_Original));
    HookExport(L"user32.dll", "LoadStringW",
               reinterpret_cast<void*>(LoadStringW_Hook),
               reinterpret_cast<void**>(&LoadStringW_Original));
    HookExport(L"user32.dll", "DialogBoxParamW",
               reinterpret_cast<void*>(DialogBoxParamW_Hook),
               reinterpret_cast<void**>(&DialogBoxParamW_Original));
    HookExport(L"user32.dll", "CreateDialogParamW",
               reinterpret_cast<void*>(CreateDialogParamW_Hook),
               reinterpret_cast<void**>(&CreateDialogParamW_Original));
    HookExport(L"user32.dll", "MessageBoxW",
               reinterpret_cast<void*>(MessageBoxW_Hook),
               reinterpret_cast<void**>(&MessageBoxW_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "FindResourceW",
                           reinterpret_cast<void*>(FindResourceW_Hook),
                           reinterpret_cast<void**>(&FindResourceW_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "FindResourceExW",
                           reinterpret_cast<void*>(FindResourceExW_Hook),
                           reinterpret_cast<void**>(&FindResourceExW_Original));
    HookExport(L"user32.dll", "LoadImageW",
               reinterpret_cast<void*>(LoadImageW_Hook),
               reinterpret_cast<void**>(&LoadImageW_Original));
    // The pictures are also served where the keyboard looks its resources up:
    // a picture that is missing in the resource module cannot be loaded at all,
    // whichever icon function is used.
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll",
                           "SizeofResource",
                           reinterpret_cast<void*>(SizeofResource_Hook),
                           reinterpret_cast<void**>(&SizeofResource_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LoadResource",
                           reinterpret_cast<void*>(LoadResource_Hook),
                           reinterpret_cast<void**>(&LoadResource_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "LockResource",
                           reinterpret_cast<void*>(LockResource_Hook),
                           reinterpret_cast<void**>(&LockResource_Original));
    HookExport(L"user32.dll", "CreateWindowExW",
               reinterpret_cast<void*>(CreateWindowExW_Hook),
               reinterpret_cast<void**>(&CreateWindowExW_Original));
    // The keyboard may have created its main window before the hooks above
    // were installed; a window that appears later is caught by the hook.
    EnumWindows(TitleFixEnumProc, 0);
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll", "ExitProcess",
                           reinterpret_cast<void*>(ExitProcess_Hook),
                           reinterpret_cast<void**>(&ExitProcess_Original));
    HookExportWithFallback(L"kernelbase.dll", L"kernel32.dll",
                           "TerminateProcess",
                           reinterpret_cast<void*>(TerminateProcess_Hook),
                           reinterpret_cast<void**>(&TerminateProcess_Original));
}

void LogOskProcessEnvironment() {
    std::wstring exe = GetProcessImagePath();
    std::wstring installDir = GetInstallDir();

    Wh_Log(L"Windows 7 osk.exe running from %s", exe.c_str());
    Wh_Log(L"Install folder: %s", installDir.c_str());

    std::wstring muiDir = JoinPath(installDir, L"en-US");
    Wh_Log(L"osk.exe.mui: %s",
           FileExists(JoinPath(muiDir, L"osk.exe.mui"))
               ? L"present (the built-in strings/dialogs are not used)"
               : L"missing (using the built-in strings/dialogs)");

    wchar_t systemDir[MAX_PATH];
    if (GetSystemDirectoryW(systemDir, ARRAYSIZE(systemDir))) {
        for (PCWSTR dll : {L"DUser.dll", L"WMsgAPI.dll", L"MSSWCH.dll",
                           L"dui70.dll", L"dui71.dll"}) {
            std::wstring inSystem = JoinPath(systemDir, dll);
            std::wstring inInstall = JoinPath(installDir, dll);
            Wh_Log(L"%s: System32=%s, install folder=%s", dll,
                   FileExists(inSystem) ? L"yes" : L"no",
                   FileExists(inInstall) ? L"yes" : L"no");
        }
    }

    if (g_keyFaces) {
        Wh_Log(L"Key faces: the faces of the keys that are not text are "
               L"supplied by the mod (colour %s)",
               DescribeKeyFaceColour().c_str());
    }

    for (PCWSTR name : {L"DUser.dll", L"dui70.dll", L"dui71.dll", L"MSSWCH.dll",
                        L"TabSKB.dll", L"TipResX.dll"}) {
        LogModuleInfo(name);
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    g_exeModule = GetModuleHandleW(nullptr);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_startTick = GetTickCount64();

    LoadSettings();
    Wh_Log(L"Init in %s", GetProcessImagePath().c_str());

    if (IsOskImageName()) {
        if (IsOurOskProcess()) {
            g_oskProcess = true;
            SelectStringTable();
            LogOskProcessEnvironment();
            if (g_diagnostics) {
                LogFolderAccess(L"osk.exe");
            }
            HookOskProcess();
            if (g_preloadLocalDlls) {
                PreloadLocalDlls();
            }
            SpawnWorker(ModuleSnapshotThread, nullptr, L"module snapshot");
        } else {
            RunStockOskHandoff();
        }
        return TRUE;
    }

    Wh_Log(L"Launcher side: redirect=%d, hand-off=%d, prepare=%d, "
           L"diagnostics=%d",
           g_redirectLaunches ? 1 : 0, g_handoff ? 1 : 0,
           g_prepareFiles ? 1 : 0, g_diagnostics ? 1 : 0);
    HookLauncherProcess();
    LogSystemDependencies();
    StartSetup();
    return TRUE;
}

void Wh_ModBeforeUninit() {
    // Hooks are still active here: tell the workers to stop before anything is
    // removed, and close the handles of the download in progress. WinHTTP
    // documents that closing a handle makes the calls that are blocked on it
    // return, which is what keeps the shutdown below from waiting for a
    // network timeout.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    CancelHttp();
}

void Wh_ModUninit() {
    JoinWorkers();
    // Nothing here frees the data the hooks served to the keyboard: resource
    // handles, resource bytes and strings live in blocks of the process heap
    // that are never freed, so they stay valid after the unload (see
    // ServePersistentBytes and the handles served by FindResourceExW). The
    // dialog templates are copies inside the dialogs the keyboard created, and
    // the settings strings are only read by the hooks, which are gone by now.
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    InvalidateArtwork();
    InvalidateReadyCache();
    Wh_Log(L"Settings reloaded (install folder: %s)", GetInstallDir().c_str());
    if (!IsOskImageName()) {
        StartSetup();
    }
}
