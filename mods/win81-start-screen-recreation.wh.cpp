// ==WindhawkMod==
// @id              win81-start-screen-recreation
// @name            Windows 8.1 Start Screen Recreation
// @name:it         Ricreazione della schermata Start di Windows 8.1
// @name:es         Recreación de la pantalla de inicio de Windows 8.1
// @name:fr         Recréation de l'écran d'accueil de Windows 8.1
// @name:pt         Recriação da Tela Inicial do Windows 8.1
// @description     This mod recreates the Windows 8.1 Start Screen on Windows 10 and Windows 11
// @description:it  Questa mod ricrea la schermata Start di Windows 8.1 su Windows 10 e Windows 11
// @description:es  Este mod recrea la pantalla de inicio de Windows 8.1 en Windows 10 y Windows 11
// @description:fr  Ce mod recrée l'écran d'accueil de Windows 8.1 sur Windows 10 et Windows 11
// @description:pt  Este mod recria a Tela Inicial do Windows 8.1 no Windows 10 e no Windows 11
// @version         1.0.0
// @license         GPL-3.0-or-later
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32 -lmsimg32 -lshell32 -ladvapi32 -lole32 -ld2d1 -ldwrite -ldwmapi -lgdiplus -luuid -lshlwapi -lwtsapi32 -lpowrprof -lshcore
// ==/WindhawkMod==
//
// ==WindhawkModReadme==
/*
# Windows 8.1 Start Screen Recreation

This mod recreates the Windows 8.1 Start Screen on Windows 10 and Windows 11.

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/startscreen.png)

## About the mod
The mod replaces the Start experience with a full-screen Start screen modeled after
Windows 8.1: tiles arranged in groups, an All Apps view, type-to-search, and the
corresponding menus and animations. Feedback and suggestions are accepted. The mod
has been tested on Windows 10 21H2, Windows 10 22H2, Windows 11 24H2 and Windows 11 25H2.

Note: This is a best-effort modification. Layout, colors, icons and animations are
inspired by Windows 8.1. Tiles display the real Windows 10/11 applications, so
certain original elements (such as Live Tiles and Metro apps) cannot be restored.
The mod does **not** replace system files.

## Features
- **Tiles**: all four sizes (small, medium, wide, large). Tiles can be dragged,
  apps can be pinned and unpinned, and groups can be named via right-click on empty
  space and **Name groups**.
- **8.1 menus**: right-clicking a tile opens the Windows 8.1 Update menu with the
  cascading **Resize** submenu. The bottom command bar (touch / keyboard) provides
  Advanced, Run, Resize and Unpin.
- **Search**: typing starts a search. The search charm covers apps, settings and
  files, with the Everywhere / Apps / Settings / Files scopes.
- **All Apps view**: accessed via the circle arrow at the bottom left; sortable by
  name, date installed or category.
- **Real tile colors**: packaged apps use their own tile color and artwork; classic
  programs that declare a tile color (such as Chrome) use it as well.
- **Background options**: the classic petal artwork, a solid color, the accent
  color, the desktop wallpaper, or a custom image.
- **Optional 8.1-style layout**: modern versions of the old Windows 8.1 apps
  (Outlook, Calendar, Photos, Weather, and others) can be pinned automatically when
  installed. Disabled by default; it can be enabled in the settings.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| Windows key / Start button | Opens or closes Start |
| Typing | Starts a search |
| Ctrl+Tab | Opens the All Apps view |
| Arrow keys + Enter | Navigates and opens apps |
| Esc | Closes menus / Start |

## Additional information
- The mod is compatible with the **Windows 8/8.1 Charms Bar Recreation** and the
  **Windows 8/8.1 Search Charm Recreation** mods for a complete Windows 8.1
  experience.
- The mod is available in English, Italian, Spanish, French and Portuguese. The language
  follows the Windows display language.
- All options are documented on the mod's settings page in Windhawk.
- If the Start hook cannot be installed (for example on an unsupported Windows build), the
  mod falls back to keyboard interception. A lone Windows key press is then detected by a
  keyboard hook, and the mod sends a synthetic key press of an unassigned key (VK 0xE8) so
  that Explorer does not open its own Start menu as well.

## Credits

- **AdmnistratoX** - for the original Windows 8 (8045) Start Screen mod on which this
  mod is based.
- **WinBoeing 777** - Testing on Windows 10 22H2
- All testers who reported bugs and provided feedback.

## Notes 

- This modification is not affiliated with, endorsed by, or sponsored by Microsoft. This project represents a community contribution to help users improve their Windows experience by recreating a legacy component of an old operating system. Windows is a trademark owned by Microsoft Corporation. This modification is an independent companion project intended to complement, not replace, the standard Windows experience, and is provided free of charge for personal, non-commercial use.
- For problems, please report an issue on GitHub.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- winKey: true
  $name: "Open with the Windows key and the Start button"
  $name:it: "Apri con il tasto Windows e il pulsante Start"
  $name:es: "Abrir con la tecla Windows y el botón Inicio"
  $name:fr: "Ouvrir avec la touche Windows et le bouton Démarrer"
  $name:pt: "Abrir com a tecla Windows e o botão Iniciar"
  $description: "This setting makes pressing the Windows key on its own, or clicking the taskbar Start button, open this Start screen instead of the Windows Start menu (Win+E, Win+D and the other shortcuts keep working)."
  $description:it: "Questa impostazione fa aprire questa schermata Start, al posto del menu Start di Windows, quando premi il tasto Windows da solo o fai clic sul pulsante Start della barra (Win+E, Win+D e le altre combinazioni continuano a funzionare)."
  $description:es: "Esta configuración hace que, al pulsar la tecla Windows sola o hacer clic en el botón Inicio de la barra, se abra esta pantalla de inicio en lugar del menú Inicio de Windows (Win+E, Win+D y el resto de accesos siguen funcionando)."
  $description:fr: "Ce paramètre fait ouvrir cet écran d'accueil, au lieu du menu Démarrer de Windows, quand vous appuyez sur la touche Windows seule ou cliquez sur le bouton Démarrer de la barre (Win+E, Win+D et les autres raccourcis continuent de fonctionner)."
  $description:pt: "Esta configuração faz com que, ao pressionar a tecla Windows sozinha ou clicar no botão Iniciar da barra, esta Tela Inicial seja aberta em vez do menu Iniciar do Windows (Win+E, Win+D e os outros atalhos continuam funcionando)."
- searchOnType: true
  $name: "Typing on Start opens Search"
  $name:it: "Digitare su Start apre la ricerca"
  $name:es: "Escribir en Inicio abre la búsqueda"
  $name:fr: "Taper sur l'écran d'accueil ouvre la recherche"
  $name:pt: "Digitar na Tela Inicial abre a pesquisa"
  $description: "This setting makes typing while the Start screen is open search your apps (programs, Store apps, and system commands such as winver). A sidebar slides in; click outside it to hide it, and press the Windows key to go back to Start."
  $description:it: "Questa impostazione fa sì che, digitando mentre la schermata Start è aperta, vengano cercate le tue app (programmi, app dello Store e comandi di sistema come winver). Una barra laterale scorre dentro; fai clic fuori per nasconderla e premi il tasto Windows per tornare a Start."
  $description:es: "Esta configuración hace que, al escribir con la pantalla de inicio abierta, se busquen tus aplicaciones (programas, aplicaciones de la Tienda y comandos del sistema como winver). Una barra lateral se desliza; haz clic fuera para ocultarla y pulsa la tecla Windows para volver a Inicio."
  $description:fr: "Ce paramètre fait en sorte que, lorsque vous tapez alors que l'écran d'accueil est ouvert, vos applications soient recherchées (programmes, applications Store et commandes système comme winver). Un panneau latéral glisse ; cliquez à l'extérieur pour le masquer et appuyez sur la touche Windows pour revenir à l'accueil."
  $description:pt: "Esta configuração faz com que, ao digitar com a Tela Inicial aberta, seus aplicativos sejam pesquisados (programas, aplicativos da Loja e comandos do sistema como winver). Uma barra lateral desliza; clique fora para ocultá-la e pressione a tecla Windows para voltar à Tela Inicial."
- interceptMode: auto
  $name: "How the Windows key / Start button are caught"
  $name:it: "Come vengono intercettati il tasto Windows e il pulsante Start"
  $name:es: "Cómo se capturan la tecla Windows y el botón Inicio"
  $name:fr: "Comment la touche Windows et le bouton Démarrer sont capturés"
  $name:pt: "Como a tecla Windows e o botão Iniciar são capturados"
  $description: "This setting chooses how the Windows key and the Start button are intercepted. Changing this needs the mod to be disabled and enabled again."
  $description:it: "Questa impostazione sceglie come vengono intercettati il tasto Windows e il pulsante Start. Per modificarla è necessario disabilitare e riabilitare la mod."
  $description:es: "Esta configuración elige cómo se interceptan la tecla Windows y el botón Inicio. Cambiarla requiere desactivar y volver a activar la mod."
  $description:fr: "Ce paramètre choisit comment la touche Windows et le bouton Démarrer sont interceptés. Le modifier nécessite de désactiver puis réactiver le mod."
  $description:pt: "Esta configuração escolhe como a tecla Windows e o botão Iniciar são interceptados. Alterá-la exige desativar e ativar o mod novamente."
  $options:
  - auto: "Hook the Start menu inside Explorer (recommended), plus the keyboard hook as a backup"
  - keyboard: "Keyboard hook only (the taskbar Start button is not caught)"
  $options:it:
  - auto: "Aggancia il menu Start dentro Explorer (consigliato), più l'hook da tastiera come riserva"
  - keyboard: "Solo hook da tastiera (il pulsante Start della barra non viene intercettato)"
  $options:es:
  - auto: "Enganchar el menú Inicio dentro de Explorer (recomendado), más el hook de teclado como reserva"
  - keyboard: "Solo hook de teclado (el botón Inicio de la barra no se captura)"
  $options:fr:
  - auto: "Ancrer le menu Démarrer dans Explorer (recommandé), plus le hook clavier en secours"
  - keyboard: "Hook clavier uniquement (le bouton Démarrer de la barre n'est pas capturé)"
  $options:pt:
  - auto: "Ancorar o menu Iniciar dentro do Explorer (recomendado), mais o hook de teclado como reserva"
  - keyboard: "Somente hook de teclado (o botão Iniciar da barra não é capturado)"
- openSlideMs: 360
  $name: "Opening desktop slide (milliseconds)"
  $name:it: "Scorrimento del desktop in apertura (millisecondi)"
  $name:es: "Deslizamiento del escritorio al abrir (milisegundos)"
  $name:fr: "Glissement du bureau à l'ouverture (millisecondes)"
  $name:pt: "Deslizamento da área de trabalho ao abrir (milissegundos)"
  $description: "This setting controls the duration of the desktop moving left when Start opens. Smaller is faster. Allowed range 80 to 3000; default 360."
  $description:it: "Questa impostazione controlla la durata dello spostamento del desktop verso sinistra quando Start si apre. Un valore più piccolo è più veloce. Intervallo consentito da 80 a 3000; predefinito 360."
  $description:es: "Esta configuración controla la duración del movimiento del escritorio hacia la izquierda cuando se abre Inicio. Un valor menor es más rápido. Rango permitido de 80 a 3000; predeterminado 360."
  $description:fr: "Ce paramètre contrôle la durée du déplacement du bureau vers la gauche quand l'écran d'accueil s'ouvre. Plus petit = plus rapide. Plage autorisée de 80 à 3000 ; défaut 360."
  $description:pt: "Esta configuração controla a duração do movimento da área de trabalho para a esquerda quando a Tela Inicial abre. Menor é mais rápido. Faixa permitida de 80 a 3000; padrão 360."
- closeSlideMs: 600
  $name: "Closing desktop slide (milliseconds)"
  $name:it: "Scorrimento del desktop in chiusura (millisecondi)"
  $name:es: "Deslizamiento del escritorio al cerrar (milisegundos)"
  $name:fr: "Glissement du bureau à la fermeture (millisecondes)"
  $name:pt: "Deslizamento da área de trabalho ao fechar (milissegundos)"
  $description: "This setting controls the duration of the desktop returning from the left when Start closes. Smaller is faster. Allowed range 80 to 3000; default 600."
  $description:it: "Questa impostazione controlla la durata del ritorno del desktop da sinistra quando Start si chiude. Un valore più piccolo è più veloce. Intervallo consentito da 80 a 3000; predefinito 600."
  $description:es: "Esta configuración controla la duración del regreso del escritorio desde la izquierda cuando se cierra Inicio. Un valor menor es más rápido. Rango permitido de 80 a 3000; predeterminado 600."
  $description:fr: "Ce paramètre contrôle la durée du retour du bureau depuis la gauche quand l'écran d'accueil se ferme. Plus petit = plus rapide. Plage autorisée de 80 à 3000 ; défaut 600."
  $description:pt: "Esta configuração controla a duração do retorno da área de trabalho vindo da esquerda quando a Tela Inicial fecha. Menor é mais rápido. Faixa permitida de 80 a 3000; padrão 600."
- groupColumns: 2
  $name: "Tiles per group (horizontal)"
  $name:it: "Tile per gruppo (orizzontali)"
  $name:es: "Iconos por grupo (horizontales)"
  $name:fr: "Vignettes par groupe (horizontales)"
  $name:pt: "Blocos por grupo (horizontais)"
  $description: "This setting chooses the number of Medium-tile columns in each Start group. Allowed range 2 to 8; default 2. A Small tile takes a quarter of a Medium slot, a Wide tile two slots, a Large tile four. Changes apply immediately."
  $description:it: "Questa impostazione sceglie il numero di colonne di tile medie in ogni gruppo di Start. Intervallo consentito da 2 a 8; predefinito 2. Una tile piccola occupa un quarto di posto medio, una larga due posti, una grande quattro. Le modifiche si applicano subito."
  $description:es: "Esta configuración elige el número de columnas de iconos medianos de cada grupo de Inicio. Rango permitido de 2 a 8; predeterminado 2. Un icono pequeño ocupa un cuarto de hueco mediano, uno ancho dos huecos y uno grande cuatro. Los cambios se aplican al momento."
  $description:fr: "Ce paramètre choisit le nombre de colonnes de vignettes moyennes dans chaque groupe de l'écran d'accueil. Plage autorisée de 2 à 8 ; défaut 2. Une petite vignette occupe un quart d'emplacement moyen, une large deux emplacements, une grande quatre. Les changements s'appliquent immédiatement."
  $description:pt: "Esta configuração escolhe o número de colunas de blocos médios em cada grupo da Tela Inicial. Faixa permitida de 2 a 8; padrão 2. Um bloco pequeno ocupa um quarto de espaço médio, um largo dois espaços, um grande quatro. As mudanças valem na hora."
- groupRows: 3
  $name: "Tiles per group (vertical)"
  $name:it: "Tile per gruppo (verticali)"
  $name:es: "Iconos por grupo (verticales)"
  $name:fr: "Vignettes par groupe (verticales)"
  $name:pt: "Blocos por grupo (verticais)"
  $description: "This setting chooses the number of tile rows in each Start group. Allowed range 1 to 8; default 3. On shorter displays the visible row count is limited to fit above the bottom command bar. Changes apply immediately."
  $description:it: "Questa impostazione sceglie il numero di righe di tile in ogni gruppo di Start. Intervallo consentito da 1 a 8; predefinito 3. Sugli schermi più bassi il numero di righe visibili è limitato per restare sopra la barra dei comandi. Le modifiche si applicano subito."
  $description:es: "Esta configuración elige el número de filas de iconos de cada grupo de Inicio. Rango permitido de 1 a 8; predeterminado 3. En pantallas bajas el número de filas visibles se limita para caber sobre la barra de comandos inferior. Los cambios se aplican al momento."
  $description:fr: "Ce paramètre choisit le nombre de rangées de vignettes dans chaque groupe de l'écran d'accueil. Plage autorisée de 1 à 8 ; défaut 3. Sur les écrans peu hauts, le nombre de rangées visibles est limité pour rester au-dessus de la barre de commandes. Les changements s'appliquent immédiatement."
  $description:pt: "Esta configuração escolhe o número de fileiras de blocos em cada grupo da Tela Inicial. Faixa permitida de 1 a 8; padrão 3. Em telas mais baixas, o número de fileiras visíveis é limitado para caber acima da barra de comandos inferior. As mudanças valem na hora."
- backgroundMode: petals
  $name: "Start screen background"
  $name:it: "Sfondo della schermata Start"
  $name:es: "Fondo de la pantalla de inicio"
  $name:fr: "Arrière-plan de l'écran d'accueil"
  $name:pt: "Plano de fundo da Tela Inicial"
  $description: "This setting chooses what the Start screen uses as its background, like the Windows 8.1 personalization options: the petal artwork drawn as vector shapes, a solid colour, the system accent colour, the desktop wallpaper with a translucent colour overlay, or your own image file from the background image path setting. Whenever a provided image cannot be loaded or decoded, the petal artwork is used as fallback."
  $description:it: "Questa impostazione sceglie cosa usa come sfondo la schermata Start, come le opzioni di personalizzazione di Windows 8.1: l'arte a petali disegnata come forme vettoriali, un colore pieno, il colore accent di sistema, lo sfondo del desktop con una velatura colorata traslucida oppure un tuo file immagine dal percorso dell'immagine di sfondo. Quando un'immagine fornita non può essere caricata o decodificata, viene usata come riserva l'arte a petali."
  $description:es: "Esta configuración elige qué usa como fondo la pantalla de inicio, como las opciones de personalización de Windows 8.1: el arte de pétalos dibujado como formas vectoriales, un color sólido, el color de acento del sistema, el fondo de escritorio con una veladura translúcida o tu propio archivo de imagen desde la ruta de imagen de fondo. Cuando una imagen proporcionada no puede cargarse o descodificarse, se usa como reserva el arte de pétalos."
  $description:fr: "Ce paramètre choisit ce que l'écran d'accueil utilise comme arrière-plan, comme les options de personnalisation de Windows 8.1 : l'art à pétales dessiné en formes vectorielles, une couleur unie, la couleur d'accent du système, le fond d'écran du bureau avec un voile coloré translucide ou votre propre fichier image depuis le chemin d'image d'arrière-plan. Lorsqu'une image fournie ne peut pas être chargée ou décodée, l'art à pétales sert de repli."
  $description:pt: "Esta configuração escolhe o que a Tela Inicial usa como plano de fundo, como as opções de personalização do Windows 8.1: a arte de pétalas desenhada como formas vetoriais, uma cor sólida, a cor de destaque do sistema, o papel de parede da área de trabalho com uma véu colorido translúcido ou seu próprio arquivo de imagem do caminho de imagem de fundo. Quando uma imagem fornecida não pode ser carregada ou decodificada, a arte de pétalas é usada como reserva."
  $options:
  - petals: "Petal background drawn as vector artwork"
  - solid: "Solid colour from the background colour setting"
  - accent: "Solid colour tied to the system accent colour"
  - wallpaper: "Desktop wallpaper with a translucent colour overlay, like Windows 8.1"
  - image: "Custom image from the background image path setting"
  $options:it:
  - petals: "Sfondo a petali disegnato come arte vettoriale"
  - solid: "Colore pieno dall'impostazione del colore di sfondo"
  - accent: "Colore pieno legato al colore accent di sistema"
  - wallpaper: "Sfondo del desktop con velatura colorata traslucida, come Windows 8.1"
  - image: "Immagine personalizzata dal percorso dell'immagine di sfondo"
  $options:es:
  - petals: "Fondo de pétalos dibujado como arte vectorial"
  - solid: "Color sólido del ajuste de color de fondo"
  - accent: "Color sólido ligado al color de acento del sistema"
  - wallpaper: "Fondo de escritorio con veladura translúcida, como Windows 8.1"
  - image: "Imagen personalizada desde la ruta de imagen de fondo"
  $options:fr:
  - petals: "Fond à pétales dessiné en art vectoriel"
  - solid: "Couleur unie du paramètre de couleur d'arrière-plan"
  - accent: "Couleur unie liée à la couleur d'accent du système"
  - wallpaper: "Fond d'écran du bureau avec voile translucide, comme Windows 8.1"
  - image: "Image personnalisée depuis le chemin d'image d'arrière-plan"
  $options:pt:
  - petals: "Fundo de pétalas desenhado como arte vetorial"
  - solid: "Cor sólida da configuração de cor de fundo"
  - accent: "Cor sólida ligada à cor de destaque do sistema"
  - wallpaper: "Papel de parede da área de trabalho com véu translúcido, como o Windows 8.1"
  - image: "Imagem personalizada do caminho de imagem de fundo"
- backgroundImagePath: ""
  $name: "Custom background image path"
  $name:it: "Percorso dell'immagine di sfondo personalizzata"
  $name:es: "Ruta de la imagen de fondo personalizada"
  $name:fr: "Chemin de l'image d'arrière-plan personnalisée"
  $name:pt: "Caminho da imagem de fundo personalizada"
  $description: "This setting is the full path of the image file used when the Start screen background is set to the custom image mode, for example C:\\Users\\YourName\\Pictures\\start.png. Environment variables such as %USERPROFILE% are expanded. The image is cover-cropped to fill the screen, like a wallpaper set to Fill. If the path is empty or the image cannot be loaded, the petal artwork is used instead."
  $description:it: "Questa impostazione è il percorso completo del file immagine usato quando lo sfondo della schermata Start è in modalità immagine personalizzata, per esempio C:\\Users\\TuoNome\\Immagini\\start.png. Le variabili d'ambiente come %USERPROFILE% vengono espanse. L'immagine viene ritagliata per riempire lo schermo, come uno sfondo impostato su Riempimento. Se il percorso è vuoto o l'immagine non può essere caricata, viene usata l'arte a petali."
  $description:es: "Esta configuración es la ruta completa del archivo de imagen usado cuando el fondo de la pantalla de inicio está en modo de imagen personalizada, por ejemplo C:\\Users\\TuNombre\\Imágenes\\start.png. Las variables de entorno como %USERPROFILE% se expanden. La imagen se recorta para llenar la pantalla, como un fondo en modo Rellenar. Si la ruta está vacía o la imagen no puede cargarse, se usa el arte de pétalos."
  $description:fr: "Ce paramètre est le chemin complet du fichier image utilisé quand l'arrière-plan de l'écran d'accueil est en mode image personnalisée, par exemple C:\\Users\\VotreNom\\Images\\start.png. Les variables d'environnement comme %USERPROFILE% sont développées. L'image est recadrée pour remplir l'écran, comme un fond d'écran en mode Remplir. Si le chemin est vide ou l'image illisible, l'art à pétales est utilisé."
  $description:pt: "Esta configuração é o caminho completo do arquivo de imagem usado quando o plano de fundo da Tela Inicial está no modo de imagem personalizada, por exemplo C:\\Users\\SeuNome\\Imagens\\start.png. Variáveis de ambiente como %USERPROFILE% são expandidas. A imagem é cortada para preencher a tela, como um papel de parede no modo Preencher. Se o caminho estiver vazio ou a imagem não puder ser carregada, a arte de pétalas é usada."
- backgroundColor: "#4617b4"
  $name: "Start background and overlay colour (hex)"
  $name:it: "Colore di sfondo Start e di velatura (esadecimale)"
  $name:es: "Color de fondo de Inicio y de veladura (hex)"
  $name:fr: "Couleur d'arrière-plan Start et de voile (hex)"
  $name:pt: "Cor de fundo da Tela Inicial e do véu (hex)"
  $description: "This setting is the solid Windows 8.1 style background colour used by the solid and desktop-wallpaper background modes (in wallpaper mode it is the translucent overlay tint, since in Windows 8.1 the Start background colour was independent of the accent colour). Accepts #RRGGBB; invalid values fall back to the Windows 8.1 default purple #4617b4."
  $description:it: "Questa impostazione è il colore di sfondo pieno in stile Windows 8.1 usato dalle modalità colore pieno e sfondo del desktop (in modalità sfondo del desktop è la tinta traslucida di velatura, perché in Windows 8.1 il colore di sfondo di Start era indipendente dal colore accent). Accetta #RRGGBB; i valori non validi ripiegano sul viola predefinito di Windows 8.1 #4617b4."
  $description:es: "Esta configuración es el color de fondo sólido al estilo de Windows 8.1 que usan los modos color sólido y fondo de escritorio (en modo fondo de escritorio es el tinte translúcido, ya que en Windows 8.1 el color de fondo de Inicio era independiente del color de acento). Acepta #RRGGBB; los valores no válidos usan el púrpura predeterminado de Windows 8.1 #4617b4."
  $description:fr: "Ce paramètre est la couleur d'arrière-plan unie de style Windows 8.1 utilisée par les modes couleur unie et fond d'écran du bureau (en mode fond d'écran, c'est la teinte translucide du voile, car sous Windows 8.1 la couleur d'arrière-plan de l'écran d'accueil était indépendante de la couleur d'accent). Accepte #RRGGBB ; les valeurs invalides reviennent au violet par défaut de Windows 8.1 #4617b4."
  $description:pt: "Esta configuração é a cor de fundo sólida no estilo do Windows 8.1 usada pelos modos cor sólida e papel de parede da área de trabalho (no modo papel de parede, é o tom translúcido do véu, pois no Windows 8.1 a cor de fundo da Tela Inicial era independente da cor de destaque). Aceita #RRGGBB; valores inválidos usam o roxo padrão do Windows 8.1 #4617b4."
- privacyMode: false
  $name: "Privacy mode (generic account name)"
  $name:it: "Modalità privacy (nome account generico)"
  $name:es: "Modo de privacidad (nombre de cuenta genérico)"
  $name:fr: "Mode confidentialité (nom de compte générique)"
  $name:pt: "Modo de privacidade (nome de conta genérico)"
  $description: "This setting replaces your Windows account name with a localized generic label (for example User) in the Start screen header, for screenshots, recordings and streams. The account picture and its menu keep working normally. Ported from the Windows 10 1507 Start Menu Recreation mod."
  $description:it: "Questa impostazione sostituisce il nome del tuo account Windows con un'etichetta generica localizzata (per esempio Utente) nell'intestazione della schermata Start, per schermate, registrazioni e streaming. L'immagine account e il suo menu continuano a funzionare normalmente. Portata dalla mod Windows 10 1507 Start Menu Recreation."
  $description:es: "Esta configuración sustituye el nombre de tu cuenta de Windows por una etiqueta genérica localizada (por ejemplo Usuario) en el encabezado de la pantalla de inicio, para capturas, grabaciones y directos. La imagen de cuenta y su menú siguen funcionando con normalidad. Portada de la mod Windows 10 1507 Start Menu Recreation."
  $description:fr: "Ce paramètre remplace le nom de votre compte Windows par un libellé générique localisé (par exemple Utilisateur) dans l'en-tête de l'écran d'accueil, pour captures, enregistrements et streams. L'image du compte et son menu continuent de fonctionner normalement. Porté du mod Windows 10 1507 Start Menu Recreation."
  $description:pt: "Esta configuração substitui o nome da sua conta do Windows por um rótulo genérico localizado (por exemplo Usuário) no cabeçalho da Tela Inicial, para capturas, gravações e transmissões. A imagem da conta e seu menu continuam funcionando normalmente. Portado da mod Windows 10 1507 Start Menu Recreation."
- appTileColors: false
  $name: "Sample tile colours from the app icons (Windows 10 style)"
  $name:it: "Campiona i colori delle tile dalle icone delle app (stile Windows 10)"
  $name:es: "Muestrear los colores de los iconos de las aplicaciones (estilo Windows 10)"
  $name:fr: "Échantillonner les couleurs des vignettes depuis les icônes (style Windows 10)"
  $name:pt: "Amstrar as cores dos blocos a partir dos ícones dos aplicativos (estilo Windows 10)"
  $description: "This setting gives every tile that has an icon a flat fill sampled from the dominant hue of that icon, like the Windows 10 Start menu option, instead of the fixed Windows 8.1 palette colours. Tiles without an icon keep their palette colour. Ported from the Windows 10 1507 Start Menu Recreation mod. Packaged apps always use their own tile colour, as in Windows 8.1; this setting extends icon sampling to every tile."
  $description:it: "Questa impostazione dà a ogni tile con icona un riempimento pieno campionato dalla tinta dominante dell'icona, come l'opzione del menu Start di Windows 10, al posto dei colori fissi della palette di Windows 8.1. Le tile senza icona mantengono il colore di palette. Portata dalla mod Windows 10 1507 Start Menu Recreation. Le app in pacchetto usano sempre il colore della loro tile, come in Windows 8.1; questa impostazione estende il campionamento a ogni tile."
  $description:es: "Esta configuración da a cada icono con imagen un relleno plano muestreado del tono dominante del icono, como la opción del menú Inicio de Windows 10, en lugar de los colores fijos de la paleta de Windows 8.1. Los iconos sin imagen mantienen su color de paleta. Portada de la mod Windows 10 1507 Start Menu Recreation. Las aplicaciones empaquetadas siempre usan el color de su propio icono, como en Windows 8.1; esta configuración extiende el muestreo a todos los iconos."
  $description:fr: "Ce paramètre donne à chaque vignette avec icône un fond uni échantillonné depuis la teinte dominante de l'icône, comme l'option du menu Démarrer de Windows 10, au lieu des couleurs fixes de la palette Windows 8.1. Les vignettes sans icône gardent leur couleur de palette. Porté du mod Windows 10 1507 Start Menu Recreation. Les applications packagées utilisent toujours la couleur de leur propre vignette, comme Windows 8.1 ; ce paramètre étend l'échantillonnage à toutes les vignettes."
  $description:pt: "Esta configuração dá a cada bloco com ícone um preenchimento sólido amostrado do tom dominante do ícone, como a opção do menu Iniciar do Windows 10, em vez das cores fixas da paleta do Windows 8.1. Blocos sem ícone mantêm a cor da paleta. Portado da mod Windows 10 1507 Start Menu Recreation. Os aplicativos empacotados sempre usam a cor do seu próprio bloco, como no Windows 8.1; esta configuração estende a amostragem a todos os blocos."
- seedStartLayout: false
  $name: "Populate Start with a Windows 8.1 style layout from installed apps"
  $name:it: "Popola Start con un layout stile Windows 8.1 dalle app installate"
  $name:es: "Rellenar Inicio con un diseño estilo Windows 8.1 desde las apps instaladas"
  $name:fr: "Peupler l'écran d'accueil façon Windows 8.1 depuis les applications installées"
  $name:pt: "Preencher a Tela Inicial com um layout estilo Windows 8.1 dos apps instalados"
  $description: "This setting is optional and off by default. There are no real Metro apps left to download on Windows 10/11, so to spare you pinning dozens of apps by hand, turning this on pins the modern counterparts of the Windows 8.1 inbox apps that are actually installed (Mail/Outlook, Calendar, Weather, Photos, Edge, News, Camera, Xbox, Music, Movies, OneDrive, Maps, Settings, Calculator, Clock, Notepad, Paint) in Windows 8.1 sizes and groups - Wide Mail, Large Weather and so on. It only adds groups after your current tiles, never removes or moves anything, skips apps that are already pinned or not installed, and the added tiles are ordinary tiles you can resize, move or unpin. Turning it off again changes nothing that was added."
  $description:it: "Questa impostazione è opzionale e disattivata in partenza. Su Windows 10/11 non restano vere app Metro da scaricare, quindi per risparmiarti di pinnare decine di app a mano, attivandola vengono pinnate le controparti moderne delle app di sistema di Windows 8.1 che sono davvero installate (Posta/Outlook, Calendario, Meteo, Foto, Edge, Notizie, Fotocamera, Xbox, Musica, Film, OneDrive, Mappe, Impostazioni, Calcolatrice, Orologio, Blocco note, Paint) con dimensioni e gruppi in stile 8.1: Posta Larga, Meteo Grande e così via. Aggiunge solo gruppi dopo le tue tile attuali, non rimuove né sposta nulla, salta le app già pinnate o non installate, e le tile aggiunte sono tile normali da ridimensionare, spostare o rimuovere. Disattivandola non cambia nulla di ciò che è stato aggiunto."
  $description:es: "Esta configuración es opcional y viene desactivada. En Windows 10/11 ya no quedan apps Metro reales para descargar, así que, para ahorrarte fijar docenas de apps a mano, al activarla se fijan las equivalentes modernas de las apps del sistema de Windows 8.1 que estén instaladas (Correo/Outlook, Calendario, Tiempo, Fotos, Edge, Noticias, Cámara, Xbox, Música, Películas, OneDrive, Mapas, Configuración, Calculadora, Reloj, Bloc de notas, Paint) con tamaños y grupos estilo 8.1: Correo Ancho, Tiempo Grande, etc. Solo añade grupos tras tus iconos actuales, nunca quita ni mueve nada, salta las apps ya fijadas o no instaladas, y los iconos añadidos son normales: puedes redimensionarlos, moverlos o quitarlos. Desactivarla no cambia lo ya añadido."
  $description:fr: "Ce paramètre est optionnel et désactivé par défaut. Sur Windows 10/11, il ne reste plus de vraies applications Metro à télécharger ; pour vous épargner d'épingler des dizaines d'applications à la main, l'activer épingle les équivalents modernes des applications système de Windows 8.1 réellement installées (Courrier/Outlook, Calendrier, Météo, Photos, Edge, Actualités, Caméra, Xbox, Musique, Films, OneDrive, Cartes, Paramètres, Calculatrice, Horloge, Bloc-notes, Paint) aux tailles et groupes de Windows 8.1 : Courrier Large, Météo Grand, etc. Il ajoute seulement des groupes après vos vignettes actuelles, ne retire ni ne déplace rien, ignore les applications déjà épinglées ou absentes, et les vignettes ajoutées sont ordinaires : redimensionnables, déplaçables, retirables. Le désactiver ne change rien à ce qui a été ajouté."
  $description:pt: "Esta configuração é opcional e vem desativada. No Windows 10/11 não restam apps Metro de verdade para baixar, então, para poupar você de fixar dezenas de apps manualmente, ao ativá-la são fixados os equivalentes modernos dos apps de sistema do Windows 8.1 que estiverem instalados (Email/Outlook, Calendário, Clima, Fotos, Edge, Notícias, Câmera, Xbox, Música, Filmes, OneDrive, Mapas, Configurações, Calculadora, Relógio, Bloco de Notas, Paint) com tamanhos e grupos no estilo 8.1: Email Largo, Clima Grande e assim por diante. Ela apenas adiciona grupos depois dos seus blocos atuais, nunca remove nem move nada, pula apps já fixados ou não instalados, e os blocos adicionados são comuns: redimensionáveis, móveis e removíveis. Desativá-la não muda o que foi adicionado."
- openAtLogon: true
  $name: "Open Start once at sign-in"
  $name:it: "Apri Start una volta all'accesso"
  $name:es: "Abrir Inicio una vez al iniciar sesión"
  $name:fr: "Ouvrir l'écran d'accueil une fois à l'ouverture de session"
  $name:pt: "Abrir a Tela Inicial uma vez ao entrar"
  $description: "This setting shows Start after the Explorer desktop is ready on a new sign-in, using the slower 8102-style tile reveal. Does not reopen Start when this mod is edited or Explorer is restarted later in the same sign-in. Turn off to keep the desktop at sign-in."
  $description:it: "Questa impostazione mostra Start dopo che il desktop di Explorer è pronto a un nuovo accesso, usando la rivelazione delle tile più lenta in stile 8102. Non riapre Start se questa mod viene modificata o se Explorer viene riavviato più tardi nella stessa sessione. Disattivala per mantenere il desktop all'accesso."
  $description:es: "Esta configuración muestra Inicio cuando el escritorio de Explorer está listo tras un nuevo inicio de sesión, con la revelación de iconos más lenta al estilo 8102. No reabre Inicio si se edita esta mod o se reinicia Explorer más tarde en la misma sesión. Desactívala para mantener el escritorio al iniciar sesión."
  $description:fr: "Ce paramètre affiche l'écran d'accueil dès que le bureau d'Explorer est prêt après une nouvelle ouverture de session, avec la révélation des vignettes plus lente de style 8102. Il ne rouvre pas l'écran d'accueil si le mod est modifié ou si Explorer redémarre plus tard dans la même session. Désactivez-le pour garder le bureau à l'ouverture de session."
  $description:pt: "Esta configuração mostra a Tela Inicial quando a área de trabalho do Explorer está pronta após um novo logon, usando a revelação de blocos mais lenta no estilo 8102. Não reabre a Tela Inicial se este mod for editado ou se o Explorer reiniciar depois no mesmo logon. Desative para manter a área de trabalho ao entrar."
*/
// ==/WindhawkModSettings==

// Includes
#include <windhawk_utils.h>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shobjidl.h>
#include <objbase.h>
#include <shellapi.h>
#include <commoncontrols.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wtsapi32.h>
#include <shellscalingapi.h>
#include <powrprof.h>
#include <sddl.h>
#include <knownfolders.h>
#include <new>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <math.h>
#include <exception>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <optional>
#include <cmath>

using namespace Gdiplus;

class SrwGuard {
    SRWLOCK* l_; bool excl_;
public:
    SrwGuard(SRWLOCK* l, bool exclusive = false) : l_(l), excl_(exclusive) {
        if (excl_) AcquireSRWLockExclusive(l_); else AcquireSRWLockShared(l_);
    }
    ~SrwGuard() { if (excl_) ReleaseSRWLockExclusive(l_); else ReleaseSRWLockShared(l_); }
    SrwGuard(const SrwGuard&) = delete; SrwGuard& operator=(const SrwGuard&) = delete;
};

template <typename T> class GdiObj {
    T h_;
public:
    explicit GdiObj(T h = NULL) : h_(h) {}
    ~GdiObj() { if (h_) DeleteObject(h_); }
    GdiObj(const GdiObj&) = delete; GdiObj& operator=(const GdiObj&) = delete;
    GdiObj(GdiObj&& o) noexcept : h_(o.h_) { o.h_ = NULL; }
    GdiObj& operator=(GdiObj&& o) noexcept { if (this != &o) { if (h_) DeleteObject(h_); h_ = o.h_; o.h_ = NULL; } return *this; }
    T get() const { return h_; } operator T() const { return h_; }
    T release() { T t = h_; h_ = NULL; return t; }
    void reset(T h = NULL) { if (h_) DeleteObject(h_); h_ = h; }
};

class ScopedWindowDc {
    HWND wnd_;
    HDC dc_;
public:
    explicit ScopedWindowDc(HWND wnd = NULL) : wnd_(wnd), dc_(GetDC(wnd)) {}
    ~ScopedWindowDc() { if (dc_) ReleaseDC(wnd_, dc_); }
    ScopedWindowDc(const ScopedWindowDc&) = delete;
    ScopedWindowDc& operator=(const ScopedWindowDc&) = delete;
    HDC get() const { return dc_; }
    operator HDC() const { return dc_; }
};

class ScopedPaint {
    HWND wnd_;
    PAINTSTRUCT ps_{};
    HDC dc_;
public:
    explicit ScopedPaint(HWND wnd) : wnd_(wnd), dc_(BeginPaint(wnd, &ps_)) {}
    ~ScopedPaint() { if (wnd_) EndPaint(wnd_, &ps_); }
    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;
    HDC get() const { return dc_; }
    operator HDC() const { return dc_; }
};

struct ScopedSelectedObject {
    HDC dc = nullptr;
    HGDIOBJ previous = nullptr;
    ScopedSelectedObject(HDC target, HGDIOBJ object) : dc(target) {
        if (dc && object) previous = SelectObject(dc, object);
    }
    ~ScopedSelectedObject() {
        if (dc && previous && previous != HGDI_ERROR) SelectObject(dc, previous);
    }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
};

struct ScopedTextColor {
    HDC dc = nullptr;
    COLORREF previous = 0;
    ScopedTextColor(HDC target, COLORREF color) : dc(target) {
        if (dc) previous = SetTextColor(dc, color);
    }
    ~ScopedTextColor() { if (dc) SetTextColor(dc, previous); }
    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

struct ScopedBkMode {
    HDC dc = nullptr;
    int previous = 0;
    ScopedBkMode(HDC target, int mode) : dc(target) {
        if (dc) previous = SetBkMode(dc, mode);
    }
    ~ScopedBkMode() { if (dc) SetBkMode(dc, previous); }
    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};

static std::atomic<unsigned> g_exceptionCount{0};

static void LogCaughtException(const wchar_t* step, const char* what) {
    const unsigned count = g_exceptionCount.fetch_add(1) + 1;
    if (count <= 8) {
        if (what && *what)
            Wh_Log(L"[Guard] std::exception while %s: %S", step, what);
        else
            Wh_Log(L"[Guard] unknown exception while %s", step);
    }
}

template <typename Function>
static bool RunGuarded(const wchar_t* step, Function&& function) {
    try {
        function();
        return true;
    } catch (const std::exception& e) {
        LogCaughtException(step, e.what());
        return false;
    } catch (...) {
        LogCaughtException(step, nullptr);
        return false;
    }
}

template <typename T, typename Function>
static T RunGuardedValue(const wchar_t* step, T fallback, Function&& function) {
    try {
        return function();
    } catch (const std::exception& e) {
        LogCaughtException(step, e.what());
        return fallback;
    } catch (...) {
        LogCaughtException(step, nullptr);
        return fallback;
    }
}

enum StrId {
    STR_START, STR_DESKTOP,
    STR_TILE_EXPLORER, STR_TILE_COMPUTER, STR_TILE_CONTROLPANEL, STR_TILE_STORE,
    STR_APPS, STR_RESULTS_FOR, STR_NO_RESULTS, STR_SEARCH_APPS, STR_SEARCH_FOR, STR_SEARCH_IN,
    STR_ROW_APPS, STR_ROW_SETTINGS, STR_ROW_FILES, STR_ROW_STORE,
    STR_BAR_ADVANCED, STR_BAR_UNPIN, STR_BAR_PIN_START, STR_BAR_SMALLER, STR_BAR_LARGER,
    STR_ADV_MANAGE, STR_ADV_MAP, STR_ADV_DISCONNECT, STR_ADV_PROPERTIES, STR_ADV_RUNAS, STR_ADV_OPENFILE,
    ERR_COMPMGMT, ERR_MAPDRIVE, ERR_DISCONNECTDRIVE, ERR_RUNAS, ERR_NOFILELOC,
    STR_MENU_PICTURE, STR_MENU_LOCK, STR_MENU_SIGNOUT,
    STR_GENERIC_USER,
    STR_PWR_SLEEP, STR_PWR_HIBERNATE, STR_PWR_SHUTDOWN, STR_PWR_RESTART,
    STR_SORT_BY, STR_SORT_NAME, STR_SORT_DATE, STR_SORT_CAT,
    STR_CHARM_TITLE, STR_CHARM_SCOPE,
    STR_SCOPE_APPS, STR_SCOPE_SETTINGS, STR_SCOPE_FILES,
    STR_CTX_OPEN, STR_CTX_LOCATION, STR_CTX_RUNAS, STR_CTX_COPY,
    STR_BAR_NAME_GROUP,
    STR_DATE_TODAY, STR_DATE_THISWEEK, STR_DATE_LASTWEEK,
    STR_DATE_THISMONTH, STR_DATE_LASTMONTH, STR_DATE_EARLIER,
    STR_SYS_CMD, STR_SYS_TASKMGR, STR_SYS_PROGRAMS,
    STR_SIZE_RESIZE, STR_SIZE_SMALL, STR_SIZE_MEDIUM, STR_SIZE_WIDE, STR_SIZE_LARGE,
    STR_CTX_UNINSTALL, STR_CTX_UNPIN_START, STR_CTX_NEWWINDOW,
    STR_CTX_NAME_GROUPS, STR_GROUP_PLACEHOLDER,
    STR_BAR_RUN,
    STR_SORTBY_NAME, STR_SORTBY_DATE, STR_SORTBY_CAT,
    STR_COUNT
};
struct LocRow { const wchar_t* text[5]; };
static const LocRow kStrings[STR_COUNT] = {
    { { L"Start", L"Start", L"Inicio", L"D\u00e9marrer", L"Iniciar" } },
    { { L"Desktop", L"Desktop", L"Escritorio", L"Bureau", L"\u00c1rea de trabalho" } },
    { { L"File Explorer", L"Esplora file", L"Explorador de archivos", L"Explorateur de fichiers", L"Explorador de Arquivos" } },
    { { L"Computer", L"Computer", L"Equipo", L"Ordinateur", L"Computador" } },
    { { L"Control Panel", L"Pannello di controllo", L"Panel de control", L"Panneau de configuration", L"Painel de Controle" } },
    { { L"Store", L"Store", L"Tienda", L"Store", L"Loja" } },
    { { L"Apps", L"App", L"Aplicaciones", L"Applications", L"Aplicativos" } },
    { { L"Results for \"%s\" (%u)", L"Risultati per \"%s\" (%u)", L"Resultados para \"%s\" (%u)", L"R\u00e9sultats pour \u00ab %s \u00bb (%u)", L"Resultados para \"%s\" (%u)" } },
    { { L"No applications matched your search.", L"Nessuna applicazione corrisponde alla ricerca.", L"Ninguna aplicaci\u00f3n coincidi\u00f3 con la b\u00fasqueda.", L"Aucune application ne correspond \u00e0 votre recherche.", L"Nenhum aplicativo correspondeu \u00e0 sua pesquisa." } },
    { { L"Search Apps", L"Cerca app", L"Buscar aplicaciones", L"Rechercher des applications", L"Pesquisar aplicativos" } },
    { { L"Search for", L"Cerca", L"Buscar", L"Rechercher", L"Pesquisar" } },
    { { L"Search in", L"Cerca in", L"Buscar en", L"Rechercher dans", L"Pesquisar em" } },
    { { L"Apps", L"App", L"Aplicaciones", L"Applications", L"Aplicativos" } },
    { { L"Settings", L"Impostazioni", L"Configuraci\u00f3n", L"Param\u00e8tres", L"Configura\u00e7\u00f5es" } },
    { { L"Files", L"File", L"Archivos", L"Fichiers", L"Arquivos" } },
    { { L"Store", L"Store", L"Tienda", L"Store", L"Loja" } },
    { { L"Advanced", L"Avanzate", L"Avanzado", L"Avanc\u00e9", L"Avan\u00e7ado" } },
    { { L"Unpin", L"Rimuovi", L"Desanclar", L"D\u00e9tacher", L"Desafixar" } },
    { { L"Pin to Start", L"Aggiungi a Start", L"Anclar a Inicio", L"\u00c9pingler \u00e0 l'accueil", L"Fixar na Tela Inicial" } },
    { { L"Smaller", L"Pi\u00f9 piccola", L"M\u00e1s peque\u00f1o", L"Plus petit", L"Menor" } },
    { { L"Larger", L"Pi\u00f9 grande", L"M\u00e1s grande", L"Plus grand", L"Maior" } },
    { { L"Manage", L"Gestisci", L"Administrar", L"G\u00e9rer", L"Gerenciar" } },
    { { L"Map network drive...", L"Connetti unit\u00e0 di rete...", L"Conectar a unidad de red...", L"Connecter un lecteur r\u00e9seau...", L"Mapear unidade de rede..." } },
    { { L"Disconnect network drive...", L"Disconnetti unit\u00e0 di rete...", L"Desconectar unidad de red...", L"D\u00e9connecter un lecteur r\u00e9seau...", L"Desconectar unidade de rede..." } },
    { { L"Properties", L"Propriet\u00e0", L"Propiedades", L"Propri\u00e9t\u00e9s", L"Propriedades" } },
    { { L"Run as administrator", L"Esegui come amministratore", L"Ejecutar como administrador", L"Ex\u00e9cuter en tant qu'administrateur", L"Executar como administrador" } },
    { { L"Open file location", L"Apri percorso file", L"Abrir ubicaci\u00f3n del archivo", L"Ouvrir l'emplacement du fichier", L"Abrir local do arquivo" } },
    { { L"Computer Management could not be opened.", L"Impossibile aprire Gestione computer.", L"No se pudo abrir Administraci\u00f3n de equipos.", L"Impossible d'ouvrir la Gestion de l'ordinateur.", L"N\u00e3o foi poss\u00edvel abrir o Gerenciamento do Computador." } },
    { { L"The Map network drive dialog is unavailable on this Windows version.", L"La finestra Connetti unit\u00e0 di rete non \u00e8 disponibile in questa versione di Windows.", L"El cuadro Conectar a unidad de red no est\u00e1 disponible en esta versi\u00f3n de Windows.", L"La bo\u00eete de dialogue Connecter un lecteur r\u00e9seau n'est pas disponible dans cette version de Windows.", L"A caixa Mapear unidade de rede n\u00e3o est\u00e1 dispon\u00edvel nesta vers\u00e3o do Windows." } },
    { { L"The Disconnect network drive dialog is unavailable on this Windows version.", L"La finestra Disconnetti unit\u00e0 di rete non \u00e8 disponibile in questa versione di Windows.", L"El cuadro Desconectar unidad de red no est\u00e1 disponible en esta versi\u00f3n de Windows.", L"La bo\u00eete de dialogue D\u00e9connecter un lecteur r\u00e9seau n'est pas disponible dans cette version de Windows.", L"A caixa Desconectar unidade de rede n\u00e3o est\u00e1 dispon\u00edvel nesta vers\u00e3o do Windows." } },
    { { L"Windows could not run this program as administrator. Open its file location and try from Explorer.", L"Windows non pu\u00f2 eseguire questo programma come amministratore. Apri il percorso file e riprova da Explorer.", L"Windows no pudo ejecutar este programa como administrador. Abra su ubicaci\u00f3n de archivo e int\u00e9ntelo desde el Explorador.", L"Windows n'a pas pu ex\u00e9cuter ce programme en tant qu'administrateur. Ouvrez son emplacement et r\u00e9essayez depuis l'Explorateur.", L"O Windows n\u00e3o p\u00f4de executar este programa como administrador. Abra o local do arquivo e tente pelo Explorer." } },
    { { L"Windows did not provide a file location for this app.", L"Windows non ha fornito un percorso file per questa app.", L"Windows no proporcion\u00f3 una ubicaci\u00f3n de archivo para esta aplicaci\u00f3n.", L"Windows n'a pas fourni d'emplacement de fichier pour cette application.", L"O Windows n\u00e3o forneceu um local de arquivo para este aplicativo." } },
    { { L"Change account picture", L"Cambia immagine account", L"Cambiar imagen de cuenta", L"Changer l'image du compte", L"Alterar imagem da conta" } },
    { { L"Lock", L"Blocca", L"Bloquear", L"Verrouiller", L"Bloquear" } },
    { { L"Sign out", L"Disconnetti", L"Cerrar sesi\u00f3n", L"Se d\u00e9connecter", L"Sair" } },
    { { L"User", L"Utente", L"Usuario", L"Utilisateur", L"Usu\u00e1rio" } },
    { { L"Sleep", L"Sospensione", L"Suspensi\u00f3n", L"Veille", L"Suspender" } },
    { { L"Hibernate", L"Ibernazione", L"Hibernar", L"Veille prolong\u00e9e", L"Hibernar" } },
    { { L"Shut down", L"Arresta il sistema", L"Apagar", L"Arr\u00eater", L"Desligar" } },
    { { L"Restart", L"Riavvia il sistema", L"Reiniciar", L"Red\u00e9marrer", L"Reiniciar" } },
    { { L"Sort by", L"Ordina per", L"Ordenar por", L"Trier par", L"Classificar por" } },
    { { L"Name", L"Nome", L"Nombre", L"Nom", L"Nome" } },
    { { L"Date installed", L"Data di installazione", L"Fecha de instalaci\u00f3n", L"Date d'installation", L"Data de instala\u00e7\u00e3o" } },
    { { L"Category", L"Categoria", L"Categor\u00eda", L"Cat\u00e9gorie", L"Categoria" } },
    { { L"Search", L"Cerca", L"Buscar", L"Recherche", L"Pesquisar" } },
    { { L"Everywhere", L"Ovunque", L"En cualquier parte", L"Partout", L"Em qualquer lugar" } },
    { { L"Apps", L"App", L"Aplicaciones", L"Applications", L"Aplicativos" } },
    { { L"Settings", L"Impostazioni", L"Configuración", L"Paramètres", L"Configurações" } },
    { { L"Files", L"File", L"Archivos", L"Fichiers", L"Arquivos" } },
    { { L"Open", L"Apri", L"Abrir", L"Ouvrir", L"Abrir" } },
    { { L"Open file location", L"Apri percorso file", L"Abrir ubicación del archivo", L"Ouvrir l'emplacement du fichier", L"Abrir local do arquivo" } },
    { { L"Run as administrator", L"Esegui come amministratore", L"Ejecutar como administrador", L"Exécuter en tant qu'administrateur", L"Executar como administrador" } },
    { { L"Copy path", L"Copia percorso", L"Copiar ruta", L"Copier le chemin", L"Copiar caminho" } },
    { { L"Name group", L"Assegna nome al gruppo", L"Nombrar grupo", L"Nommer le groupe", L"Nomear grupo" } },
    { { L"Today", L"Oggi", L"Hoy", L"Aujourd'hui", L"Hoje" } },
    { { L"This week", L"Questa settimana", L"Esta semana", L"Cette semaine", L"Esta semana" } },
    { { L"Last week", L"Settimana scorsa", L"Semana pasada", L"Semaine derniere", L"Semana passada" } },
    { { L"Earlier this month", L"All'inizio del mese", L"A principios de mes", L"Plus tot ce mois-ci", L"No inicio deste mes" } },
    { { L"Last month", L"Mese scorso", L"Mes pasado", L"Mois dernier", L"Mes passado" } },
    { { L"Earlier", L"Precedenti", L"Anteriores", L"Plus ancien", L"Anteriores" } },
    { { L"Command Prompt", L"Prompt dei comandi", L"S\u00edmbolo del sistema", L"Invite de commandes", L"Prompt de Comando" } },
    { { L"Task Manager", L"Gestione attivit\u00e0", L"Administrador de tareas", L"Gestionnaire des t\u00e2ches", L"Gerenciador de Tarefas" } },
    { { L"Programs and Features", L"Programmi e funzionalit\u00e0", L"Programas y caracter\u00edsticas", L"Programmes et fonctionnalit\u00e9s", L"Programas e Recursos" } },
    { { L"Resize", L"Ridimensiona", L"Cambiar tama\u00f1o", L"Redimensionner", L"Redimensionar" } },
    { { L"Small", L"Piccolo", L"Peque\u00f1o", L"Petit", L"Pequeno" } },
    { { L"Medium", L"Medio", L"Mediano", L"Moyen", L"M\u00e9dio" } },
    { { L"Wide", L"Largo", L"Ancho", L"Large", L"Largo" } },
    { { L"Large", L"Grande", L"Grande", L"Grand", L"Grande" } },
    { { L"Uninstall", L"Disinstalla", L"Desinstalar", L"D\u00e9sinstaller", L"Desinstalar" } },
    { { L"Unpin from Start", L"Rimuovi da Start", L"Desanclar de Inicio", L"D\u00e9tacher de l'accueil", L"Desafixar da Tela Inicial" } },
    { { L"Open new window", L"Apri nuova finestra", L"Abrir ventana nueva", L"Ouvrir une nouvelle fen\u00eatre", L"Abrir nova janela" } },
    { { L"Name groups", L"Assegna nome ai gruppi", L"Nombrar grupos", L"Nommer les groupes", L"Nomear grupos" } },
    { { L"Name group", L"Nome gruppo", L"Nombre del grupo", L"Nom du groupe", L"Nome do grupo" } },
    { { L"Run", L"Esegui", L"Ejecutar", L"Ex\u00e9cuter", L"Executar" } },
    { { L"by name", L"per nome", L"por nombre", L"par nom", L"por nome" } },
    { { L"by date installed", L"per data di installazione", L"por fecha de instalaci\u00f3n", L"par date d'installation", L"por data de instala\u00e7\u00e3o" } },
    { { L"by category", L"per categoria", L"por categor\u00eda", L"par cat\u00e9gorie", L"por categoria" } },
};
static int g_lang = 0;

static void DetectUiLanguage() {
    ULONG count = 0, sizes = 0;
    if (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, NULL, &sizes) || !sizes) return;
    std::vector<wchar_t> buffer(sizes);
    if (!GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &count, buffer.data(), &sizes) || !count) return;
    for (const wchar_t* p = buffer.data(); *p; p += wcslen(p) + 1) {
        if (!_wcsnicmp(p, L"en", 2)) return;
        if (!_wcsnicmp(p, L"it", 2)) { g_lang = 1; return; }
        if (!_wcsnicmp(p, L"es", 2)) { g_lang = 2; return; }
        if (!_wcsnicmp(p, L"fr", 2)) { g_lang = 3; return; }
        if (!_wcsnicmp(p, L"pt", 2)) { g_lang = 4; return; }
    }
}

static const wchar_t* T(StrId id) {
    if (id < 0 || id >= STR_COUNT) return L"";
    return kStrings[id].text[g_lang];
}


static int TILE = 150, GAP = 10, LEFT = 120, TOP = 168;
static const int MAX_TILES = 48;
static int g_dpi = 96;
static float g_dpiScale = 1.0f;
static int g_userTextW = -1;
static bool g_swallowAccountUp = false;
static bool g_swallowCharmUp = false;
static bool g_swallowNamingUp = false;
static DWORD g_charmSwallowTick = 0;
static int ScI(int v) { return MulDiv(v, g_dpi, 96); }
static float ScF(float v) { return v * g_dpiScale; }
static int GROUP_SEPARATION = 40;  
static volatile LONG g_requestedGroupCols = 2, g_requestedGroupRows = 3;
static const float PRESS_SCALE   = 0.94f;      

static const float OPEN_HOLD     = 0.00f;  
static const float OPEN_SLIDE    = 0.36f;   
static const float OPEN_SCALE_T  = 0.12f;
static const float DESK_SCALE    = 0.948f;
static const float TILES_DELAY   = 0.30f;   
static const float TILE_FADE     = 0.065f;
static const float TILE_T        = 0.50f;
static const float TILE_SCALE_T  = 0.27f;
static const float TILE_S0       = 0.83f;
static float TILE_PIVOT_Y = 180.0f + 2.875f * 152.0f;    
static float TILE_SLIDE = 110.0f;
static const float BLOCK_STAGGER = 0.033f;
static const float LOGON_TILE_DELAY = 0.24f;
static const float LOGON_BLOCK_STAGGER = 0.24f;
static const float LOGON_TILE_FADE = 0.22f;
static const float LOGON_TILE_T = 0.95f;
static const float LOGON_SCALE_T = 0.76f;
static const float LOGON_TILE_S0 = 0.72f;
static float LOGON_TILE_SLIDE = 260.0f;
static const float LOGON_ACCOUNT_DELAY = 0.24f;
static const float LOGON_ACCOUNT_FADE = 0.24f;
static const float LOGON_TITLE_DELAY = 1.43f;
static const float LOGON_TITLE_FADE = 0.24f;
static const float LOGON_ACCOUNT_SLIDE = 48.0f;
static const float LOGON_ACCOUNT_MOVE = 1.12f;
static const float LOGON_TITLE_SLIDE = 30.0f;
static const float LOGON_TITLE_MOVE = 0.42f;
static const float LOGON_GLASS_DELAY = 0.67f;
static const float LOGON_GLASS_FADE = 0.70f;
static const float HEADER_DELAY  = 0.47f;
static const float HEADER_FADE   = 0.19f;
static const float CLOSE_SLIDE   = 0.60f;

static int SB_W = 346, SB_PAD = 40;
static const float SB_IN = 0.11f;
static const float RESULT_FADE = 0.12f;
static const float BG_BACK = 0.15f;
// All Apps / search grid metrics, authored at 96 DPI and scaled with the current DPI.
#define kItemX      ScI(112)
#define kItemY      ScI(178)
#define kItemW      ScI(216)
#define kItemH      ScI(59)
#define kItemPitchX ScI(235)
#define kItemPitchY ScI(60)

enum TileStyle { TOP_APP, BOTTOM_CAPTION, DESKTOP_REFERENCE, STORE_CAPTION, PINNED_APP };

struct Tile {
    const wchar_t* label;
    const wchar_t* target;
    COLORREF c1, c2;
    int col, row, cw;
    const wchar_t* iconFile;
    int iconIdx;
    const wchar_t* glyph;
    HICON icon;
    TileStyle style;
    HBITMAP pinBmp = NULL;
    int pinW = 0, pinH = 0;
    bool owned = false;
    bool alive = false;
    int kind = 0;
    float px = 0.0f, py = 0.0f, tx = 0.0f, ty = 0.0f;
    COLORREF appC = 0;
    bool hasAppC = false;
    bool veDarkText = false;
    bool veHideName = false;
    std::vector<BYTE> veArt{};
    std::vector<BYTE> veArtSmall{};
    int veW = 0, veH = 0, veSmallW = 0, veSmallH = 0;
};

static const Tile kDefaultTiles[] = {
    { L"Windows Explorer", L"explorer.exe",           RGB(78, 35, 120),  RGB(102, 45, 152), 0, 0, 1, L"explorer.exe", 0,   NULL, NULL, TOP_APP },
    { L"Computer",         L"shell:MyComputerFolder", RGB(78, 35, 120),  RGB(102, 45, 152), 1, 0, 1, L"imageres.dll",  109, NULL, NULL, TOP_APP },
    { L"Control Panel",    L"control.exe",            RGB(25, 145, 181), RGB(30, 170, 197), 0, 1, 1, NULL,             0,   NULL, NULL, BOTTOM_CAPTION },
    { L"Desktop",          L"win8start:desktop",     RGB(25, 9, 98),    RGB(70, 120, 215), 1, 1, 1, NULL,             0,   NULL, NULL, DESKTOP_REFERENCE },
    { L"Store",            L"ms-windows-store:",      RGB(155, 116, 0),  RGB(181, 146, 0),  0, 2, 2, NULL,             0,   NULL, NULL, STORE_CAPTION },
};
static const int kDefaultTileCount = sizeof(kDefaultTiles) / sizeof(kDefaultTiles[0]);
static Tile g_tiles[MAX_TILES];


static const UINT WM_APP_TOGGLE = WM_APP + 1;
static const UINT WM_APP_CLOSE  = WM_APP + 2;
static const UINT WM_APP_FRAME  = WM_APP + 3;
static const UINT WM_APP_WALLPAPER_READY = WM_APP + 4;
static const UINT WM_APP_CHAR = WM_APP + 5;
static const UINT WM_APP_KEY  = WM_APP + 6;
static const UINT WM_APP_APPS_READY = WM_APP + 7;
static const UINT WM_APP_ICON_READY = WM_APP + 8;
static const UINT WM_APP_ALL_APPS = WM_APP + 9;
static const UINT WM_APP_PACKAGE_ART = WM_APP + 10;
static const UINT WM_APP_GROUP_SETTINGS = WM_APP + 11;
static const UINT WM_APP_LOGON_OPEN = WM_APP + 12;
static const UINT WM_APP_REBUILD_BG = WM_APP + 13;
static const UINT WM_APP_CHARM_POINTER = WM_APP + 14;
static const UINT WM_APP_SEED_LAYOUT = WM_APP + 15;
static const UINT WM_APP_FILES_READY = WM_APP + 16;
static const UINT WM_APP_APPS_REFRESH = WM_APP + 17;
static const UINT WM_HOOKCTL_MOUSE_ON  = WM_APP + 30;
static const UINT WM_HOOKCTL_MOUSE_OFF = WM_APP + 31;
static const ULONG_PTR kMagic = 0x38303435;
static const wchar_t* kClassName = L"Win8StartScreenMod8045";
static const wchar_t* kDesktopTarget = L"win8start:desktop";

extern "C" IMAGE_DOS_HEADER __ImageBase;
static HINSTANCE g_inst = NULL;
static bool g_classRegistered = false;
static bool g_uiCoInit = false;
static HANDLE g_uiReady = NULL, g_uiStop = NULL;
static HANDLE g_thread = NULL;
static DWORD  g_tid = 0;
static HWND   g_wnd = NULL;
static HHOOK  g_hook = NULL;
static HHOOK  g_mouseHook = NULL;
static HANDLE g_hookThread = NULL, g_hookReady = NULL;
static DWORD  g_hookTid = 0;
static volatile bool g_useWinKey = true;
static volatile bool g_keyboardOnly = false;
static volatile bool g_startHooked = false;
static DWORD g_lastToggleTick = 0;
static volatile LONG g_openSlideMs = 360, g_closeSlideMs = 600;
static volatile LONG g_openAtLogon = 1;
static bool g_logonOpening = false;
static bool g_firstDesktopAfterLogon = false;
static DWORD g_logonVisualMs = 0, g_logonLastFrameTick = 0, g_logonRevealGateTick = 0;
static bool g_userToggledStart = false;
static HANDLE g_logonThread = NULL, g_logonStop = NULL;
static float g_openSlideSeconds = OPEN_SLIDE, g_closeSlideSeconds = CLOSE_SLIDE;

static int  g_sw = 1920, g_sh = 1080;
static int  g_monX = 0, g_monY = 0;   // top-left of the monitor Start is shown on
static int GroupColumns() { return (int)InterlockedCompareExchange(&g_requestedGroupCols, 0, 0); }
static int GroupRows() {
    int requested = (int)InterlockedCompareExchange(&g_requestedGroupRows, 0, 0);
    int fit = (g_sh - ScI(215) + GAP) / (TILE + GAP);
    if (fit < 1) fit = 1;
    return requested < fit ? requested : fit;
}
enum { TS_SMALL = 0, TS_MEDIUM = 1, TS_WIDE = 2, TS_LARGE = 3 };
static int TileCellsW(int size) {
    switch (size) {
    case TS_SMALL: return 1;
    case TS_WIDE:  case TS_LARGE: return 4;
    default:       return 2;
    }
}
static int TileCellsH(int size) {
    switch (size) {
    case TS_SMALL: return 1;
    case TS_LARGE: return 4;
    default:       return 2;
    }
}
static int CellStep() { return (TILE + GAP) / 2; }
static int TilePxW(int size) { return TileCellsW(size) * CellStep() - GAP; }
static int TilePxH(int size) { return TileCellsH(size) * CellStep() - GAP; }
static int GroupWidth() { return GroupColumns() * 2 * CellStep() - GAP; }
static int GroupPitch() { return GroupWidth() + GROUP_SEPARATION; }
static int GroupHeight() { return GroupRows() * 2 * CellStep() - GAP; }
static int SupportedSizes(const Tile& t, int out[4]) {
    const int cols = GroupColumns() * 2, rows = GroupRows() * 2;
    int n = 0;
    for (int size = TS_SMALL; size <= TS_LARGE; ++size)
        if (size == t.cw || (TileCellsW(size) <= cols && TileCellsH(size) <= rows)) out[n++] = size;
    return n;
}
static bool SupportsResize(const Tile& t) {
    int dummy[4];
    return SupportedSizes(t, dummy) > 1;
}
static const wchar_t* SizeName(int size) {
    switch (size) {
    case TS_SMALL: return T(STR_SIZE_SMALL);
    case TS_WIDE:  return T(STR_SIZE_WIDE);
    case TS_LARGE: return T(STR_SIZE_LARGE);
    default:       return T(STR_SIZE_MEDIUM);
    }
}

static volatile bool g_open = false;
static bool g_dirty = true;
static volatile bool g_closing = false;
static DWORD g_openTick = 0, g_closeTick = 0, g_lastTick = 0;
static HANDLE g_frameWake = NULL, g_frameStop = NULL, g_frameThread = NULL;
static volatile LONG g_framePending = 0;
static HANDLE g_wallWake = NULL, g_wallStop = NULL, g_wallThread = NULL;
static SRWLOCK g_wallLock = SRWLOCK_INIT;
static std::vector<BYTE> g_wallStaging;
static int g_wallStagingW = 0, g_wallStagingH = 0;
static std::vector<BYTE> g_wallImage;
static int g_wallImageW = 0, g_wallImageH = 0;
static bool g_wallStagedReady = false, g_wallImageReady = false;
static std::vector<BYTE> g_wallVirt;
static int g_wallVirtW = 0, g_wallVirtH = 0;
static bool g_wallVirtReady = false;
static const int kWallMaxPx = 760;
static BYTE g_wallPixels[kWallMaxPx * kWallMaxPx * 4] = {};
static bool g_wallReady = false;
struct PackageArt {
    std::wstring target;
    std::vector<BYTE> square, wide;
    int squareW = 0, squareH = 0, wideW = 0, wideH = 0;
    COLORREF background = RGB(97, 43, 146);
    bool hasBackground = false;
};
static SRWLOCK g_artLock = SRWLOCK_INIT;
static std::vector<PackageArt> g_packageArts;
static std::vector<std::wstring> g_artQueue, g_artChanged;
static std::wstring g_artLoading;
static HANDLE g_artStop = NULL, g_artWake = NULL, g_artThread = NULL;
static bool g_artRebuildPending = false;
static void QueuePackageArt(const wchar_t* target) {
    const wchar_t* prefix = L"shell:AppsFolder\\";
    if (!target || _wcsnicmp(target, prefix, wcslen(prefix)) ||
        !wcschr(target + wcslen(prefix), L'!')) return;
    AcquireSRWLockExclusive(&g_artLock);
    bool found = !g_artLoading.empty() && !_wcsicmp(g_artLoading.c_str(), target);
    for (const auto& a : g_packageArts) if (!_wcsicmp(a.target.c_str(), target)) { found = true; break; }
    if (!found) for (const auto& q : g_artQueue) if (!_wcsicmp(q.c_str(), target)) { found = true; break; }
    if (!found) g_artQueue.emplace_back(target);
    ReleaseSRWLockExclusive(&g_artLock);
    if (!found && g_artWake) SetEvent(g_artWake);
}
static bool g_snapReady = false;
static HDC g_snapDC = NULL;
static HBITMAP g_snapBmp = NULL;
static HGDIOBJ g_snapOld = NULL;
static float g_blockPivotX[MAX_TILES] = {};
static int g_tileBlock[MAX_TILES] = {}, g_blockCount = 0;
static float g_scrollCur = 0.0f, g_scrollTarget = 0.0f;
static int  g_hover = -1, g_pressed = -1;
static float g_tileScale[MAX_TILES];
static bool g_tracking = false;
static bool g_winDown = false, g_winCombo = false;
static volatile LONG g_privacyMode = 0;
static volatile LONG g_appTileColors = 0;
static volatile LONG g_tileColorsDirty = 0;
static volatile LONG g_appSort = 0;
static int g_charmScope = 0;
static int g_ctxRow = -1;
static int g_ctxTile = -1;
enum { CM_OPEN = 40, CM_LOCATION, CM_RUNAS, CM_COPY, CM_PINSTART, MC_SEP_ID = 60 };
enum { TC_OPEN = 70, TC_RESIZE, TC_UNPIN, TC_UNINSTALL, TC_SIZE0 = 80 };
static wchar_t g_userName[128] = L"User";

static const wchar_t* DisplayName() {
    return InterlockedCompareExchange(&g_privacyMode, 0, 0) ? T(STR_GENERIC_USER) : g_userName;
}
static bool g_hoverAccount = false;
static bool g_hoverPwr = false, g_hoverSrch = false;
static bool g_hoverArrow = false;
static bool g_charmOpen = false;
static DWORD g_charmTick = 0;
static int g_charmSel = 0, g_charmScroll = 0, g_charmHover = -1, g_charmPressed = -1;
static bool g_charmDdOpen = false;
static int g_charmDdHover = -1;
static DWORD g_leaveTick = 0;
static bool g_zoomOut = false;
static DWORD g_zoomTick = 0;
static bool g_hoverZoomBtn = false;
static int g_hoverGroup = -1;
static int g_dragGroup = -1;
static bool g_dragGroupMoved = false;
static float g_dragGroupAnchorWX = 0.0f, g_dragGroupDx = 0.0f;
static int g_namingGroup = -1;
static bool g_customize = false;
static std::wstring g_groupNameEdit;
static bool g_alphaGrid = false;
static bool g_appsSearchFocus = false;
static DWORD g_alphaTick = 0;
static int g_focus = -1;
static std::vector<std::wstring> g_groupNames;
static int g_caretPhase = -1;
static ULONG_PTR g_gdipToken = 0;
static DWORD g_startBand = 0;
static volatile LONG g_bgMode = 0;
static volatile LONG g_seedLayout = 0;
static volatile LONG g_seedPending = 0;
static COLORREF g_bgColor = RGB(0x46, 0x17, 0xB4);
static SRWLOCK g_bgPathLock = SRWLOCK_INIT;
static std::wstring g_bgImagePath;
static std::wstring BgImagePath() {
    AcquireSRWLockShared(&g_bgPathLock);
    std::wstring copy = g_bgImagePath;
    ReleaseSRWLockShared(&g_bgPathLock);
    return copy;
}

enum View { VIEW_START, VIEW_SEARCH };
enum CharmCat { CC_APP = 0, CC_SETTING = 1, CC_FILE = 2 };
struct AppEntry { std::wstring name, lower, parse; bool exe, desktop;
                  unsigned long long date; int cat;
                  std::wstring catName;
                  int charmCat = CC_APP;
                  bool dir = false;
                  bool searchOnly = false;
                  bool direct = false; };
// Copy of the entry the context menu was opened on: g_results can be re-sorted
// (e.g. when the file list finishes) while the menu is open.
static AppEntry g_ctxApp;
static bool g_ctxAppValid = false;

struct IconSlot { BYTE* px; int w, h; LONG ready; ID2D1Bitmap* gpu; LONG tried; };
static std::wstring AppTarget(const AppEntry& a) {
    return (a.exe || a.direct || a.desktop) ? a.parse
                                             : std::wstring(L"shell:AppsFolder\\") + a.parse;
}
static volatile bool g_searchEnabled = true;
static View g_view = VIEW_START;
static wchar_t g_query[128] = L"";
static int g_queryLen = 0;
static std::vector<AppEntry> g_apps;
static std::vector<IconSlot> g_icons;
static std::vector<int> g_results;
struct AppHeading { int col; wchar_t first, last; int bucket = -1; std::wstring text; };
static std::vector<AppHeading> g_appHeadings;
static std::vector<int> g_appCol, g_appRow;
static int g_selected = 0;
static LONG g_appsReady = 0;
static HANDLE g_appsThread = NULL, g_appsStop = NULL;
static SRWLOCK g_appsLock = SRWLOCK_INIT;
static HANDLE g_filesNeed = NULL;
static std::vector<AppEntry>* g_pendingFiles = NULL;
// Catalog refresh: the worker rebuilds the list, the UI thread applies it (so g_apps is
// never replaced under a reader), then signals g_iconsGo to let the worker load icons.
static HANDLE g_appsRefresh = NULL, g_iconsGo = NULL;
static std::vector<AppEntry>* g_pendingApps = NULL;
static ULONGLONG g_catalogBuiltTick = 0;
static const ULONGLONG kCatalogMaxAgeMs = 10ULL * 60ULL * 1000ULL;
static float g_sbDur = SB_IN;
static DWORD g_sbTick = 0, g_searchTick = 0, g_returnTick = 0;
static bool g_skipSnapshot = false;
static bool g_swallowed[256] = {};

static int  g_tileCount = 0;
static int  g_order[MAX_TILES] = {};
static int  g_orderCount = 0;
static int  g_tileGroup[MAX_TILES] = {};
static int  g_committedGroup[MAX_TILES] = {};
static int  g_groupCount = 0;
static int  g_dragStartOrder[MAX_TILES] = {}, g_dragStartGroups[MAX_TILES] = {};
static int  g_dragIdx = -1;
static bool g_dragging = false;
static int  g_pressX = 0, g_pressY = 0;
static float g_grabX = 0.0f, g_grabY = 0.0f;
static bool g_barVisible = false;
static int  g_barMode = 0;
static int  g_barIdx = -1;
static DWORD g_barTick = 0;
static bool g_advancedOpen = false;
static bool g_resizeOpen = false;
static int g_barHoverButton = -1, g_barPressedButton = -1;
static int g_advHoverRow = -1, g_advPressedRow = -1;
static int g_resizeHoverRow = -1, g_resizePressedRow = -1;

static HDC g_tileDC[MAX_TILES][2] = {};
static HBITMAP g_tileBmp[MAX_TILES][2] = {};
static HGDIOBJ g_tileOld[MAX_TILES][2] = {};
static void* g_tileBits[MAX_TILES][2] = {};
static void BuildTileBuffers(HDC screenDC);

static HDC g_bgDC = NULL, g_bbDC = NULL;
static HBITMAP g_bgBmp = NULL, g_bbBmp = NULL;
static HGDIOBJ g_bgOld = NULL, g_bbOld = NULL;
static void* g_bgBits = NULL;
static void* g_snapBits = NULL;

static ID2D1Factory* g_d2dFactory = NULL;
static ID2D1HwndRenderTarget* g_rt = NULL;
static ID2D1Bitmap* g_bgGpu = NULL;
static ID2D1Bitmap* g_snapGpu = NULL;
static ID2D1Bitmap* g_tileGpu[MAX_TILES][2] = {};
static ID2D1SolidColorBrush* g_whiteGpu = NULL;
static ID2D1SolidColorBrush* g_avatarBg = NULL;
static ID2D1SolidColorBrush* g_avatarFg = NULL;
static IDWriteFactory* g_writeFactory = NULL;
static IDWriteTextFormat* g_titleFormat = NULL;
static IDWriteTextFormat* g_userFormat = NULL;
static ID2D1Bitmap* g_desktopAppGpu = NULL;
static ID2D1Bitmap* g_bgPlainGpu = NULL;
static ID2D1SolidColorBrush* g_scratch = NULL;
static IDWriteTextFormat *g_subFormat = NULL, *g_itemFormat = NULL, *g_sbTitleFormat = NULL, *g_boldFormat = NULL,
                         *g_rowFormat = NULL, *g_countFormat = NULL, *g_queryFormat = NULL,
                         *g_glyphFormat = NULL, *g_glyphSmall = NULL, *g_barLabel = NULL, *g_flyFormat = NULL;
static IDWriteTextFormat* g_hdrGlyphFmt = NULL;
static IDWriteTextFormat* g_zoomFormat = NULL;
static HBITMAP g_bgPlainBmp = NULL;
static void* g_bgPlainBits = NULL;

static void ReleaseGpuTarget();
static bool InitGpu();
static bool RenderGpu();
static void DrawSearchGpu(DWORD now, bool closing);
static bool BarClick(int x, int y);
static void HideBar();
static void ShowBar(int mode, int idx);
static void OnRightClick(int x, int y);
static void ShowUserMenu(HWND owner);
static float HeaderOpacity(float to);
static void ShowMenuFor(HWND owner, RECT anchor, int kind);
static int HitTile(int mx, int my);
static void DrawBarGpu(DWORD now);
static void DrawAppsChromeGpu();
static void DrawAppsChromeGdi(HDC sdc);
static void DrawScrollBarTo(HDC dc);
static void DrawCharmBarGpu(DWORD now);
static void DrawCharmBarGdi(HDC sdc, DWORD now);
static void CharmToggle();
static void OpenCharm(wchar_t firstChar);
static void CloseCharm(bool now);
static bool CharmPaneRect(RECT* r);
static void CharmRender(int contOff);
static void CommitAllApps();
#define LEAVE_MS 240.0f
static float LeaveEase(int i, DWORD now);
static int g_charmCont = 0;
static void CharmMoveSel(int delta);
static COLORREF SystemAccentColor();
static bool GetWallpaperPath(wchar_t* out, size_t capacity);
static void AccountRect(RECT* r);
static int HeaderAvatarRight();
static void HeaderButtonRects(RECT* pwr, RECT* srch);
static void AppsArrowRect(RECT* r);
static void SortButtonRect(RECT* r);
static void AppsSearchBoxRect(RECT* r);
static void ZoomButtonRect(RECT* r);
static bool EnsureAvatarPicture();
static void ReleaseAvatarPicture();
struct UiIcon;
static bool EnsureUiIcons();
static void ReleaseUiIcons();
struct UiIcon { HBITMAP bmp = NULL; BYTE* px = NULL; int w = 0, h = 0; ID2D1Bitmap* gpu = NULL; };
static UiIcon g_iconPower, g_iconSearch, g_iconArrowDown, g_iconArrowUp, g_iconZoom;
static UiIcon g_iconHover;
static void BlitIconGdi(HDC dc, const UiIcon& ic, const RECT& r, bool hover, BYTE alpha);
static void IconDestRect(const RECT& hit, RECT* out);
static int UserTextWidth();
static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size);
static void UpdateTileAppColors();
static void SetBr(COLORREF c, float a);
static void FillRc(float l, float t, float r, float b, COLORREF c, float a = 1.0f);
static void TextAt(const wchar_t* s, IDWriteTextFormat* f, float l, float t, float r, float b, COLORREF c, float a = 1.0f);
static void RepaintNow();
static void BuildOneTile(int i, HDC screenDC);
static void CloseStart(bool immediate = false);
static HFONT g_fTitle = NULL, g_fUser = NULL, g_fLabel = NULL, g_fTopLabel = NULL;
static HFONT g_fHdrGlyph = NULL, g_fBarMeas = NULL;
static HBITMAP g_avatarPicBmp = NULL;
static ID2D1Bitmap* g_avatarPicGpu = NULL;
static int g_avatarPicSize = 0;
static void* g_avatarPicBits = NULL;
static bool g_avatarPicFailed = false;

static DWORD NowMs() {
    static const LONGLONG freq = [] {
        LARGE_INTEGER f = {}; QueryPerformanceFrequency(&f); return f.QuadPart;
    }();
    LARGE_INTEGER c = {}; QueryPerformanceCounter(&c);
    return (DWORD)((c.QuadPart / freq) * 1000 + ((c.QuadPart % freq) * 1000) / freq);
}
static float Clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
static float EaseOut(float t) { t = Clamp01(t); float i = 1.0f - t; return 1.0f - i * i * i; }
static float EaseMetroOut(float t) {
    t = Clamp01(t);
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    const float x1 = 0.1f, y1 = 0.9f, x2 = 0.2f, y2 = 1.0f;
    float lo = 0.0f, hi = 1.0f, u = t;
    for (int i = 0; i < 24; ++i) {
        const float v = 1.0f - u;
        const float x = 3.0f * v * v * u * x1 + 3.0f * v * u * u * x2 + u * u * u;
        if (x < t) lo = u; else hi = u;
        u = (lo + hi) * 0.5f;
    }
    const float v = 1.0f - u;
    return 3.0f * v * v * u * y1 + 3.0f * v * u * u * y2 + u * u * u;
}

static float SlideTangent(const float* time, const float* pos, int count, int i) {
    if (i <= 0 || i >= count - 1) return 0.0f;
    float h0 = time[i] - time[i - 1], h1 = time[i + 1] - time[i];
    float d0 = (pos[i] - pos[i - 1]) / h0, d1 = (pos[i + 1] - pos[i]) / h1;
    if (d0 <= 0.0f || d1 <= 0.0f) return 0.0f;
    float w0 = 2.0f * h1 + h0, w1 = h1 + 2.0f * h0;
    return (w0 + w1) / (w0 / d0 + w1 / d1);
}

static float ReferenceSlide(const float* time, const float* pos, int count, float t) {
    t = Clamp01(t);
    int i = 0;
    while (i + 1 < count - 1 && t > time[i + 1]) ++i;
    float h = time[i + 1] - time[i], u = (t - time[i]) / h;
    float u2 = u * u, u3 = u2 * u;
    float a = (2.0f * u3 - 3.0f * u2 + 1.0f) * pos[i] + (u3 - 2.0f * u2 + u) * h * SlideTangent(time, pos, count, i);
    float b = (-2.0f * u3 + 3.0f * u2) * pos[i + 1] + (u3 - u2) * h * SlideTangent(time, pos, count, i + 1);
    return Clamp01(a + b);
}

static float OpenSlideProgress(float t) {
    t = Clamp01(t);
    float smooth = t * t * (3.0f - 2.0f * t);
    float brisk = t * (2.0f - t);
    return 0.65f * smooth + 0.35f * brisk;
}

static float CloseSlideProgress(float t) {
    static const float time[] = { 0.0f, .0333f, .0667f, .1333f, .20f, .3333f, .50f, .6667f, .8333f, 1.0f };
    static const float pos[]  = { 0.0f, .12f,   .32f,   .61f,   .77f, .89f,    .948f,.982f,  .994f,  1.0f };
    return ReferenceSlide(time, pos, ARRAYSIZE(time), t);
}

static float SmoothStep(float t) { t = Clamp01(t); return t * t * (3.0f - 2.0f * t); }
static float EaseSettle(float t) { t = Clamp01(t); float r = 1.0f - t; return 1.0f - r * r * r * r; }

static DWORD OpeningElapsedMs(DWORD now) {
    return g_logonOpening ? g_logonVisualMs : now - g_openTick;
}
static void AdvanceLogonClock() {
    if (!g_logonOpening || !g_open || g_closing) return;
    DWORD now = NowMs();
    if ((LONG)(now - g_logonRevealGateTick) < 0) {
        g_logonLastFrameTick = now;
        return;
    }
    DWORD delta = now - g_logonLastFrameTick;
    if (delta > 50) delta = 50;
    if (g_logonVisualMs < 30000) g_logonVisualMs += delta;
    g_logonLastFrameTick = now;
}

static float TileSlideDistance() {
    int extra = g_blockCount > 1 ? g_blockCount - 1 : 0;
    if (extra > 4) extra = 4;
    return TILE_SLIDE * (1.0f - 0.01f * (float)extra);
}

static void TileRect(const Tile& t, RECT* r) {
    r->left = (int)(t.px + 0.5f);
    r->top = (int)(t.py + 0.5f);
    r->right = r->left + TilePxW(t.cw);
    r->bottom = r->top + TilePxH(t.cw);
}

static float MaxScroll() {
    int right = 0;
    for (int i = 0; i < g_tileCount; i++) {
        if (!g_tiles[i].alive) continue;
        int r = (int)g_tiles[i].tx + TilePxW(g_tiles[i].cw);
        if (r > right) right = r;
    }
    float m = (float)(right + LEFT - g_sw);
    return m > 0.0f ? m : 0.0f;
}

static void ClampScroll();
static void NormalizeGroups();
static void Launch(int i);
static void Relayout(bool animate);
static void SaveLayout();
static std::vector<std::wstring> SplitTabs(const std::wstring& line);
#define ZOOM_MS 260.0f
static float ZoomPresence(DWORD now) {
    if (!g_zoomTick) return g_zoomOut ? 1.0f : 0.0f;
    float t = (float)(now - g_zoomTick) / ZOOM_MS;
    if (g_zoomOut) return EaseOut(Clamp01(t));
    return 1.0f - EaseOut(Clamp01(t));
}
static bool GroupBounds(int g, RECT* r) {
    bool any = false;
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive || g_tileGroup[i] != g) continue;
        RECT t; TileRect(g_tiles[i], &t);
        if (!any) { *r = t; any = true; }
        else {
            if (t.left < r->left) r->left = t.left;
            if (t.top < r->top) r->top = t.top;
            if (t.right > r->right) r->right = t.right;
            if (t.bottom > r->bottom) r->bottom = t.bottom;
        }
    }
    return any;
}
static bool GroupBoundsShifted(int g, RECT* r) {
    if (!GroupBounds(g, r)) return false;
    if (g == g_dragGroup && g_dragGroupMoved) {
        int d = (int)g_dragGroupDx;
        r->left += d; r->right += d;
    }
    return true;
}
static bool GridBounds(RECT* r) {
    bool any = false;
    for (int g = 0; g < g_groupCount; ++g) {
        RECT b; if (!GroupBounds(g, &b)) continue;
        if (!any) { *r = b; any = true; }
        else {
            if (b.left < r->left) r->left = b.left;
            if (b.top < r->top) r->top = b.top;
            if (b.right > r->right) r->right = b.right;
            if (b.bottom > r->bottom) r->bottom = b.bottom;
        }
    }
    if (!any) return false;
    r->top = TOP - ScI(40);
    r->bottom += ScI(40);
    return true;
}
static void ZoomXform(DWORD now, float* s, float* ox, float* oy) {
    float k = ZoomPresence(now);
    RECT gb;
    if (!GridBounds(&gb) || k <= 0.0f) { *s = 1.0f; *ox = 0.0f; *oy = 0.0f; return; }
    float m = ScF(60);
    float gw = (float)std::max(1, (int)gb.right);
    float gh = (float)std::max(1, (int)(gb.bottom - gb.top));
    float zs = std::min(((float)g_sw - 2 * m) / gw, ((float)g_sh - 2 * m) / gh);
    if (zs > 0.62f) zs = 0.62f;
    if (zs < 0.05f) zs = 0.05f;
    float zox = ((float)g_sw - gw * zs) * 0.5f;
    float zoy = ((float)g_sh - gh * zs) * 0.5f - (float)gb.top * zs;
    *s = 1.0f + (zs - 1.0f) * k;
    *ox = zox * k; *oy = zoy * k;
}
static void ScreenToWorld(int mx, int my, float* wx, float* wy) {
    DWORD now = NowMs();
    float k = ZoomPresence(now);
    if (k <= 0.0f) { *wx = (float)mx + g_scrollCur; *wy = (float)my; return; }
    float s, ox, oy; ZoomXform(now, &s, &ox, &oy);
    *wx = ((float)mx - ox) / s + g_scrollCur;
    *wy = ((float)my - oy) / s;
}
static int GroupAtWorld(float wx, float wy) {
    float pad = ScF(8);
    for (int g = 0; g < g_groupCount; ++g) {
        RECT b; if (!GroupBoundsShifted(g, &b)) continue;
        if (wx >= b.left - pad && wx <= b.right + pad && wy >= b.top - pad && wy <= b.bottom + pad) return g;
    }
    return -1;
}
static void GroupScreenRect(DWORD now, int g, float* l, float* t, float* r, float* b) {
    RECT bb; *l = *t = *r = *b = 0.0f;
    if (!GroupBoundsShifted(g, &bb)) return;
    float k = ZoomPresence(now);
    float s = 1.0f, ox = 0.0f, oy = 0.0f; ZoomXform(now, &s, &ox, &oy);
    float nl = bb.left - g_scrollCur, nt = (float)bb.top, nr = bb.right - g_scrollCur, nb = (float)bb.bottom;
    float zl = ox + s * bb.left, zt = oy + s * bb.top, zr = ox + s * bb.right, zb = oy + s * bb.bottom;
    *l = nl + (zl - nl) * k; *t = nt + (zt - nt) * k;
    *r = nr + (zr - nr) * k; *b = nb + (zb - nb) * k;
}
static void SwapGroupOrder(int a, int b) {
    if (a == b || a < 0 || b < 0) return;
    int probe = -1;
    std::vector<int> kept, moved;
    for (int k = 0; k < g_orderCount; ++k) {
        int i = g_order[k];
        if (g_tileGroup[i] == a) { if (probe < 0) probe = i; moved.push_back(i); }
        else kept.push_back(i);
    }
    if (moved.empty()) return;
    int firstB = -1, lastB = -1, fa = -1, fb = -1;
    for (int k = 0; k < (int)kept.size(); ++k)
        if (g_tileGroup[kept[k]] == b) { if (firstB < 0) firstB = k; lastB = k; }
    for (int k = 0; k < g_orderCount; ++k) {
        int i = g_order[k];
        if (fa < 0 && g_tileGroup[i] == a) fa = k;
        if (fb < 0 && g_tileGroup[i] == b) fb = k;
    }
    if (firstB < 0) return;
    int insertAt = (fa < fb) ? lastB + 1 : firstB;
    std::vector<int> out;
    for (int k = 0; k <= (int)kept.size(); ++k) {
        if (k == insertAt) for (int m : moved) out.push_back(m);
        if (k < (int)kept.size()) out.push_back(kept[k]);
    }
    g_orderCount = (int)out.size();
    for (int k = 0; k < g_orderCount; ++k) g_order[k] = out[k];
    NormalizeGroups();
    if (probe >= 0 && a == g_dragGroup) g_dragGroup = g_tileGroup[probe];
    Relayout(true);
}

static void MaybeSwapDraggedGroup() {
    if (g_dragGroup < 0) return;
    RECT before; if (!GroupBounds(g_dragGroup, &before)) return;
    float own = (before.left + before.right) * 0.5f;
    float dcx = own + g_dragGroupDx;
    int desired = 0, current = 0;
    for (int g = 0; g < g_groupCount; ++g) {
        if (g == g_dragGroup) continue;
        RECT b; if (!GroupBounds(g, &b)) continue;
        float c = (b.left + b.right) * 0.5f;
        if (c < own) ++current;
        if (c < dcx) ++desired;
    }
    if (desired == current) return;
    int other = -1;
    if (desired > current) { for (int g = g_dragGroup + 1; g < g_groupCount; ++g) { RECT b; if (GroupBounds(g, &b)) { other = g; break; } } }
    else { for (int g = g_dragGroup - 1; g >= 0; --g) { RECT b; if (GroupBounds(g, &b)) { other = g; break; } } }
    if (other < 0) return;
    SwapGroupOrder(g_dragGroup, other);
    RECT after;
    if (GroupBounds(g_dragGroup, &after)) {
        float shift = (float)(after.left - before.left);
        g_dragGroupAnchorWX += shift;
        g_dragGroupDx -= shift;
    }
    SaveLayout();
}
static std::wstring CurrentUserSid() {
    static const std::wstring sid = [] {
        std::wstring result;
        HANDLE token = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            BYTE buf[512];
            DWORD need = 0;
            if (GetTokenInformation(token, TokenUser, buf, sizeof(buf), &need)) {
                LPWSTR str = NULL;
                if (ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &str) && str) {
                    result = str;
                    LocalFree(str);
                }
            }
            CloseHandle(token);
        }
        if (result.empty()) result = L"unknown";
        return result;
    }();
    return sid;
}
static bool StorageFilePath(const wchar_t* stem, std::wstring& out) {
    wchar_t base[4096] = L"";
    size_t n = Wh_GetModStoragePath(base, ARRAYSIZE(base));
    if (!n) return false;
    std::wstring dir(base, n);
    CreateDirectoryW(dir.c_str(), NULL);
    out = dir + L"\\" + stem + L"-" + CurrentUserSid() + L".txt";
    return true;
}
static bool GroupsPathW(std::wstring& out) {
    return StorageFilePath(L"groups", out);
}
static void SaveGroupNames() {
    std::wstring path;
    if (!GroupsPathW(path)) return;
    std::wstring text = L"#Win8StartScreen groups v1\n";
    for (int g = 0; g < (int)g_groupNames.size(); ++g) {
        if (g_groupNames[g].empty()) continue;
        wchar_t line[192];
        swprintf_s(line, L"%d\t%s\n", g, g_groupNames[g].c_str());
        text += line;
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
    if (n <= 0) return;
    std::string utf8((size_t)n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &utf8[0], n, NULL, NULL);
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(f, utf8.data(), (DWORD)utf8.size(), &written, NULL);
    CloseHandle(f);
}
static void LoadGroupNames() {
    g_groupNames.assign(std::max(0, g_groupCount), std::wstring());
    std::wstring path;
    if (!GroupsPathW(path)) return;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD size = GetFileSize(f, NULL);
    if (size == INVALID_FILE_SIZE || size == 0 || size > (1u << 20)) { CloseHandle(f); return; }
    std::string utf8(size, '\0');
    DWORD got = 0;
    BOOL ok = ReadFile(f, &utf8[0], size, &got, NULL);
    CloseHandle(f);
    if (!ok || !got) return;
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)got, NULL, 0);
    if (n <= 0) return;
    std::wstring text((size_t)n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)got, &text[0], n);
    size_t pos = 0;
    while (pos < text.size()) {
        size_t eol = text.find(L'\n', pos);
        std::wstring line = text.substr(pos, eol == std::wstring::npos ? std::wstring::npos : eol - pos);
        pos = (eol == std::wstring::npos) ? text.size() : eol + 1;
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (line.empty() || line[0] == L'#') continue;
        std::vector<std::wstring> p = SplitTabs(line);
        if (p.size() < 2) continue;
        int g = _wtoi(p[0].c_str());
        if (g < 0 || g >= 64) continue;
        if ((int)g_groupNames.size() <= g) g_groupNames.resize(g + 1);
        g_groupNames[g] = p[1];
    }
}
static void StartNaming(int g) {
    if (g < 0) return;
    g_namingGroup = g;
    g_groupNameEdit = (g < (int)g_groupNames.size()) ? g_groupNames[g] : std::wstring();
    Wh_Log(L"[zoom] naming group %d", g);
    g_dirty = true; RepaintNow();
}
static void CommitNaming() {
    if (g_namingGroup < 0) return;
    while (!g_groupNameEdit.empty() && g_groupNameEdit.back() == L' ') g_groupNameEdit.pop_back();
    if ((int)g_groupNames.size() <= g_namingGroup) g_groupNames.resize(g_namingGroup + 1);
    g_groupNames[g_namingGroup] = g_groupNameEdit;
    Wh_Log(L"[zoom] group %d named", g_namingGroup);
    g_namingGroup = -1;
    g_groupNameEdit.clear();
    SaveGroupNames();
    g_dirty = true; RepaintNow();
}
static void CancelNaming() {
    if (g_namingGroup < 0) return;
    g_namingGroup = -1;
    g_groupNameEdit.clear();
    g_dirty = true; RepaintNow();
}
static void NamingChar(wchar_t c) {
    if (g_namingGroup < 0) return;
    if (c >= L' ' && g_groupNameEdit.size() < 48) g_groupNameEdit += c;
    g_dirty = true; RepaintNow();
}
static void NamingKey(UINT vk) {
    if (g_namingGroup < 0) return;
    if (vk == VK_BACK) { if (!g_groupNameEdit.empty()) g_groupNameEdit.pop_back(); g_dirty = true; RepaintNow(); }
    else if (vk == VK_RETURN) CommitNaming();
    else if (vk == VK_ESCAPE) CancelNaming();
}
static void ExitZoom(int jumpGroup) {
    if (g_namingGroup >= 0) CancelNaming();
    if (jumpGroup >= 0) {
        RECT b;
        if (GroupBounds(jumpGroup, &b)) g_scrollTarget = (float)std::max(0, (int)b.left - LEFT);
    }
    g_zoomOut = false;
    g_zoomTick = NowMs();
    g_hoverGroup = -1;
    g_dragGroup = -1;
    g_dragGroupMoved = false;
    g_dragGroupDx = 0.0f;
    ClampScroll();
    Wh_Log(L"[zoom] overview close jump=%d", jumpGroup);
    g_dirty = true; RepaintNow();
}
static void DrawZoomHighlightGpu(float zk) {
    for (int g = 0; g < g_groupCount; ++g) {
        bool hl = (g == g_hoverGroup) || (g == g_dragGroup && g_dragGroupMoved);
        if (!hl) continue;
        RECT b; if (!GroupBoundsShifted(g, &b)) continue;
        FillRc((float)b.left - g_scrollCur - ScF(8), (float)b.top - ScF(8),
               (float)b.right - g_scrollCur + ScF(8), (float)b.bottom + ScF(8),
               RGB(255, 255, 255), 0.16f * zk);
    }
}
static bool GroupNameFieldRect(int g, RECT* out) {
    float l, t, r, b; GroupScreenRect(NowMs(), g, &l, &t, &r, &b);
    if (!out || r < 0.0f || l > (float)g_sw) return false;
    float bw = std::max(r - l, ScF(180)), bh = ScF(32);
    out->left = (int)l; out->top = (int)(t - bh - ScF(10));
    out->right = (int)(l + bw); out->bottom = (int)(t - ScF(10));
    return true;
}
static void EnterCustomize() {
    if (g_view != VIEW_START || g_charmOpen || g_zoomOut || !g_open || g_closing) return;
    HideBar();
    g_customize = true;
    g_dirty = true; RepaintNow();
}
static void ExitCustomize() {
    if (g_namingGroup >= 0) CommitNaming();
    if (!g_customize) return;
    g_customize = false;
    g_dirty = true; RepaintNow();
}
static void DrawGroupNamesGpu(DWORD now) {
    float k = ZoomPresence(now);
    float ta = HeaderOpacity((float)OpeningElapsedMs(now) / 1000.0f);
    for (int g = 0; g < g_groupCount; ++g) {
        bool naming = (g == g_namingGroup);
        const std::wstring* nm = (g < (int)g_groupNames.size()) ? &g_groupNames[g] : NULL;
        if (!naming && !g_customize && (!nm || nm->empty())) continue;
        float l, t, r, b; GroupScreenRect(now, g, &l, &t, &r, &b);
        if (r < 0.0f || l > (float)g_sw) continue;
        if (naming) {
            float bw = std::max(r - l, ScF(180)), bh = ScF(32);
            float bx = l, by = t - bh - ScF(10);
            FillRc(bx, by, bx + bw, by + bh, RGB(255, 255, 255), 1.0f);
            TextAt(g_groupNameEdit.c_str(), g_queryFormat, bx + ScF(8), by, bx + bw - ScF(14), by + bh, RGB(0, 0, 0));
            if ((NowMs() / 500) & 1) {
                HDC dc = GetDC(NULL); SIZE tsz = {}; int tw = 0;
                if (dc && g_fLabel) {
                    HGDIOBJ o = SelectObject(dc, g_fLabel);
                    if (GetTextExtentPoint32W(dc, g_groupNameEdit.c_str(), (int)g_groupNameEdit.size(), &tsz)) tw = tsz.cx;
                    SelectObject(dc, o);
                }
                if (dc) ReleaseDC(NULL, dc);
                FillRc(bx + ScF(8) + tw, by + ScF(6), bx + ScF(10) + tw, by + bh - ScF(6), RGB(0, 0, 0), 1.0f);
            }
        } else if (g_customize) {
            float bw = std::max(r - l, ScF(180)), bh = ScF(32);
            float bx = l, by = t - bh - ScF(10);
            FillRc(bx, by, bx + bw, by + bh, RGB(0, 0, 0), 0.35f);
            bool has = nm && !nm->empty();
            TextAt(has ? nm->c_str() : T(STR_GROUP_PLACEHOLDER), g_queryFormat,
                   bx + ScF(8), by, bx + bw - ScF(8), by + bh, RGB(255, 255, 255), has ? 1.0f : 0.72f);
        } else {
            float nx = l + ScF(2);
            float ny = (k > 0.5f) ? b + ScF(6) : t - ScF(30);
            TextAt(nm->c_str(), g_subFormat, nx, ny, nx + (r - l), ny + ScF(26), RGB(255, 255, 255), 0.92f * ta);
        }
    }
}
struct AutoCompatDC {
    HDC dc = NULL; HGDIOBJ old = NULL;
    explicit AutoCompatDC(HDC ref) { dc = CreateCompatibleDC(ref); }
    ~AutoCompatDC() { if (old) SelectObject(dc, old); if (dc) DeleteDC(dc); }
    operator HDC() const { return dc; }
};
struct AutoFont {
    HFONT f = NULL;
    AutoFont() = default;
    explicit AutoFont(HFONT h) : f(h) {}
    ~AutoFont() { if (f) DeleteObject(f); }
    AutoFont(const AutoFont&) = delete; AutoFont& operator=(const AutoFont&) = delete;
    HFONT get() const { return f; }
};
struct AutoObj {
    HGDIOBJ o = NULL;
    explicit AutoObj(HGDIOBJ h) : o(h) {}
    ~AutoObj() { if (o) DeleteObject(o); }
    AutoObj(const AutoObj&) = delete; AutoObj& operator=(const AutoObj&) = delete;
};

static void BlendFillGdi(HDC dc, int l, int t, int r, int b, COLORREF c, BYTE a) {
    if (r <= l || b <= t || a == 0) return;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = 1; bi.bmiHeader.biHeight = -1;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = NULL;
    HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!dib || !bits) { if (dib) DeleteObject(dib); return; }
    BYTE* px = (BYTE*)bits;
    px[0] = (BYTE)(GetRValue(c) * a / 255);
    px[1] = (BYTE)(GetGValue(c) * a / 255);
    px[2] = (BYTE)(GetBValue(c) * a / 255);
    px[3] = a;
    {
        AutoCompatDC mdc(dc);
        AutoObj adib(dib);
        if (mdc.dc) {
            mdc.old = SelectObject(mdc.dc, dib);
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            AlphaBlend(dc, l, t, r - l, b - t, mdc.dc, 0, 0, 1, 1, bf);
        }
    }
}

static bool g_sbDrag = false, g_sbHover = false;
static float MaxScrollSearch();
static float ScrollMax() { return g_view == VIEW_SEARCH ? MaxScrollSearch() : MaxScroll(); }
static int g_sbArrowDir = 0;
static DWORD g_sbArrowNext = 0;
static DWORD g_sbArrowPrev = 0;
static int g_sbDragOff = 0;
static RECT ScrollBarRect() {
    RECT r; r.left = ScI(4); r.right = g_sw - ScI(4);
    r.top = g_sh - ScI(16); r.bottom = g_sh - ScI(4); return r;
}
static RECT ScrollArrowRect(bool right) {
    RECT b = ScrollBarRect(), r; r.top = b.top; r.bottom = b.bottom;
    if (!right) { r.left = b.left; r.right = b.left + ScI(12); }
    else        { r.left = b.right - ScI(12); r.right = b.right; }
    return r;
}
static RECT ScrollTrackRect() {
    RECT b = ScrollBarRect(), l = ScrollArrowRect(false), r = ScrollArrowRect(true), t;
    t.left = l.right + ScI(2); t.right = r.left - ScI(2);
    t.top = b.top + ScI(2); t.bottom = b.bottom - ScI(2); return t;
}
static RECT ScrollThumbRect() {
    RECT tr = ScrollTrackRect(), r; r.top = tr.top; r.bottom = tr.bottom;
    float maxs = ScrollMax(), span = (float)(tr.right - tr.left), tw = span;
    if (maxs > 0.5f) {
        tw = span * (float)g_sw / (maxs + (float)g_sw);
        if (tw < ScI(24)) tw = ScI(24);
    }
    float pos = (maxs > 0.5f) ? (span - tw) * (g_scrollCur / maxs) : 0.0f;
    r.left = tr.left + (int)pos; r.right = r.left + (int)tw; return r;
}
static DWORD g_lastMouseMs = 0;
static float ChromeIdleFade(DWORD now) {
    if (g_sbDrag || g_sbHover || g_hoverArrow) return 1.0f;
    DWORD idle = now - g_lastMouseMs;
    if (idle < 3500) return 1.0f;
    return Clamp01(1.0f - (float)(idle - 3500) / 500.0f);
}
static bool ScrollBarVisible() {
    return g_open && !g_closing && ZoomPresence(NowMs()) <= 0.0f && ScrollMax() > 0.5f;
}
static void ScrollStep(int delta) {
    g_scrollTarget += (float)delta; ClampScroll(); RepaintNow();
}
static void ScrollArrowTick() {
    if (!g_sbArrowDir) return;
    DWORD now = NowMs();
    if ((int)(now - g_sbArrowNext) < 0) return;
    DWORD dt = now - g_sbArrowPrev;
    if (dt > 500) dt = 500;
    int step = (int)(0.9f * (float)dt * (float)ScI(1));
    if (step < ScI(8)) step = ScI(8);
    ScrollStep(g_sbArrowDir * step);
    g_sbArrowPrev = now;
    g_sbArrowNext = now + 50;
}
// Scrolling
static void ScrollSetFromX(int mx) {
    RECT tr = ScrollTrackRect(), th = ScrollThumbRect();
    float span = (float)(tr.right - tr.left), tw = (float)(th.right - th.left);
    float den = span - tw; if (den < 1.0f) den = 1.0f;
    float f = ((float)mx - (float)g_sbDragOff - (float)tr.left) / den;
    if (f < 0.0f) f = 0.0f; if (f > 1.0f) f = 1.0f;
    g_scrollTarget = f * ScrollMax(); ClampScroll();
    g_scrollCur = g_scrollTarget;
}
static void DrawScrollBarTo(HDC dc) {
    if (!ScrollBarVisible()) return;
    float fade = ChromeIdleFade(NowMs());
    if (fade <= 0.01f) return;
    RECT tr = ScrollTrackRect(), th = ScrollThumbRect();
    RECT al = ScrollArrowRect(false), ar = ScrollArrowRect(true);
    BlendFillGdi(dc, tr.left, tr.top, tr.right, tr.bottom, RGB(255, 255, 255), (BYTE)(45.0f * fade));
    BYTE la = (BYTE)(((g_sbDrag || g_sbHover) ? 170 : 120) * fade);
    BlendFillGdi(dc, th.left, th.top, th.right, th.bottom, RGB(255, 255, 255), la);
    for (int k = 0; k < 2; ++k) {
        RECT b = k ? ar : al;
        bool hov = g_sbArrowDir == (k ? 1 : -1) ||
                   (g_sbHover && !g_sbDrag && PtInRect(&b, { (b.left + b.right) / 2, (b.top + b.bottom) / 2 }));
        int cx = (b.left + b.right) / 2, cy = (b.top + b.bottom) / 2, e = ScI(3);
        POINT pts[3];
        if (!k) { pts[0] = { cx - e, cy }; pts[1] = { cx + e, cy - e - 1 }; pts[2] = { cx + e, cy + e + 1 }; }
        else    { pts[0] = { cx + e, cy }; pts[1] = { cx - e, cy - e - 1 }; pts[2] = { cx - e, cy + e + 1 }; }
        BlendFillGdi(dc, b.left, b.top, b.right, b.bottom, RGB(255, 255, 255), (BYTE)((hov ? 60 : 0) * fade));
        HBRUSH br = CreateSolidBrush(RGB(225, 228, 238));
        AutoObj abr(br);
        HGDIOBJ ob = SelectObject(dc, br);
        Polygon(dc, pts, 3);
        SelectObject(dc, ob);
    }
}
static void DrawScrollBarGpu() {
    if (!ScrollBarVisible() || !g_rt) return;
    float fade = ChromeIdleFade(NowMs());
    if (fade <= 0.01f) return;
    RECT tr = ScrollTrackRect(), th = ScrollThumbRect();
    RECT al = ScrollArrowRect(false), ar = ScrollArrowRect(true);
    FillRc((float)tr.left, (float)tr.top, (float)tr.right, (float)tr.bottom, RGB(255, 255, 255), 0.18f * fade);
    float la = ((g_sbDrag || g_sbHover) ? 0.67f : 0.47f) * fade;
    FillRc((float)th.left, (float)th.top, (float)th.right, (float)th.bottom, RGB(255, 255, 255), la);
    if (!g_scratch) return;
    for (int k = 0; k < 2; ++k) {
        RECT b = k ? ar : al;
        bool hov = g_sbArrowDir == (k ? 1 : -1);
        float cx = (float)(b.left + b.right) / 2.0f, cy = (float)(b.top + b.bottom) / 2.0f, e = (float)ScI(3);
        if (hov) FillRc((float)b.left, (float)b.top, (float)b.right, (float)b.bottom, RGB(255, 255, 255), 0.25f * fade);
        g_scratch->SetColor(D2D1::ColorF(0.88f, 0.90f, 0.94f, fade));
        float w = (float)ScI(2);
        if (!k) {
            g_rt->DrawLine(D2D1::Point2F(cx + e, cy - e), D2D1::Point2F(cx - e, cy), g_scratch, w);
            g_rt->DrawLine(D2D1::Point2F(cx - e, cy), D2D1::Point2F(cx + e, cy + e), g_scratch, w);
        } else {
            g_rt->DrawLine(D2D1::Point2F(cx - e, cy - e), D2D1::Point2F(cx + e, cy), g_scratch, w);
            g_rt->DrawLine(D2D1::Point2F(cx + e, cy), D2D1::Point2F(cx - e, cy + e), g_scratch, w);
        }
    }
}
static void DrawZoomOverlayGdi(DWORD now) {
    if (g_view != VIEW_START) return;
    float k = ZoomPresence(now);
    float s = 1.0f, ox = 0.0f, oy = 0.0f; ZoomXform(now, &s, &ox, &oy);
    for (int g = 0; g < g_groupCount; ++g) {
        if (k > 0.0f && (g == g_hoverGroup || (g == g_dragGroup && g_dragGroupMoved))) {
            RECT bb;
            if (GroupBoundsShifted(g, &bb)) {
                int l = (int)(ox + s * (bb.left - g_scrollCur)) - ScI(8);
                int t2 = (int)(oy + s * bb.top) - ScI(8);
                int rr = (int)(ox + s * (bb.right - g_scrollCur)) + ScI(8);
                int bo = (int)(oy + s * bb.bottom) + ScI(8);
                BlendFillGdi(g_bbDC, l, t2, rr, bo, RGB(255, 255, 255), (BYTE)(40.0f * k));
            }
        }
        bool naming = (g == g_namingGroup);
        const std::wstring* nm = (g < (int)g_groupNames.size()) ? &g_groupNames[g] : NULL;
        if (!naming && !g_customize && (!nm || nm->empty())) continue;
        float l, t, r, b; GroupScreenRect(now, g, &l, &t, &r, &b);
        if (r < 0.0f || l > (float)g_sw) continue;
        SetBkMode(g_bbDC, TRANSPARENT);
        if (naming) {
            int bw = std::max((int)(r - l), ScI(180)), bh = ScI(32);
            int bx = (int)l, by = (int)t - bh - ScI(10);
            HBRUSH wb = (HBRUSH)GetStockObject(WHITE_BRUSH);
            RECT wr = { bx, by, bx + bw, by + bh };
            FillRect(g_bbDC, &wr, wb);
            SetTextColor(g_bbDC, RGB(0, 0, 0));
            SelectObject(g_bbDC, g_fLabel);
            RECT tr = { bx + ScI(8), by, bx + bw - ScI(8), by + bh };
            DrawTextW(g_bbDC, g_groupNameEdit.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            if ((NowMs() / 500) & 1) {
                SIZE tsz = {}; int tw = 0;
                if (GetTextExtentPoint32W(g_bbDC, g_groupNameEdit.c_str(), (int)g_groupNameEdit.size(), &tsz)) tw = tsz.cx;
                RECT cr = { bx + ScI(8) + tw, by + ScI(6), bx + ScI(10) + tw, by + bh - ScI(6) };
                FillRect(g_bbDC, &cr, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
        } else if (g_customize) {
            int bw = std::max((int)(r - l), ScI(180)), bh = ScI(32);
            int bx = (int)l, by = (int)t - bh - ScI(10);
            BlendFillGdi(g_bbDC, bx, by, bx + bw, by + bh, RGB(0, 0, 0), 90);
            bool has = nm && !nm->empty();
            SetTextColor(g_bbDC, has ? RGB(255, 255, 255) : RGB(200, 200, 200));
            SelectObject(g_bbDC, g_fLabel);
            RECT tr = { bx + ScI(8), by, bx + bw - ScI(8), by + bh };
            DrawTextW(g_bbDC, has ? nm->c_str() : T(STR_GROUP_PLACEHOLDER), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        } else {
            BYTE sh = (BYTE)(255.0f * HeaderOpacity((float)OpeningElapsedMs(now) / 1000.0f));
            SetTextColor(g_bbDC, RGB(sh, sh, sh));
            SelectObject(g_bbDC, g_fLabel);
            int ny = (k > 0.5f) ? (int)b + ScI(6) : (int)t - ScI(30);
            RECT tr = { (int)l + ScI(2), ny, (int)r, ny + ScI(26) };
            DrawTextW(g_bbDC, nm->c_str(), -1, &tr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
        }
    }
}

static void FocusEnsureVisible(int i) {
    RECT r; TileRect(g_tiles[i], &r);
    float l = r.left - g_scrollTarget, rt = r.right - g_scrollTarget;
    if (l < LEFT) g_scrollTarget -= (LEFT - l) + ScI(16);
    else if (rt > g_sw - ScI(16)) g_scrollTarget += (rt - g_sw) + ScI(32);
    ClampScroll();
}
static void FocusFirstVisible() {
    int best = -1; float bestD = 1e9f;
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive) continue;
        RECT r; TileRect(g_tiles[i], &r);
        float d = fabsf((r.left - g_scrollCur) - (float)LEFT);
        if (r.right - g_scrollCur > 0 && d < bestD) { bestD = d; best = i; }
    }
    g_focus = best;
}
static void FocusMove(UINT vk) {
    if (g_view != VIEW_START || g_charmOpen || g_zoomOut) return;
    if (g_focus < 0 || g_focus >= g_tileCount || !g_tiles[g_focus].alive) { FocusFirstVisible(); if (g_focus >= 0) RepaintNow(); return; }
    RECT c; TileRect(g_tiles[g_focus], &c);
    float ccx = (c.left + c.right) * 0.5f, ccy = (c.top + c.bottom) * 0.5f;
    int best = -1; float bestScore = 1e9f;
    for (int i = 0; i < g_tileCount; ++i) {
        if (i == g_focus || !g_tiles[i].alive) continue;
        RECT r; TileRect(g_tiles[i], &r);
        float cx = (r.left + r.right) * 0.5f, cy = (r.top + r.bottom) * 0.5f;
        float dx = cx - ccx, dy = cy - ccy, score;
        if (vk == VK_RIGHT)      { if (dx <= ScI(8)) continue;  score = dx + fabsf(dy) * 3.0f; }
        else if (vk == VK_LEFT)  { if (dx >= -ScI(8)) continue; score = -dx + fabsf(dy) * 3.0f; }
        else if (vk == VK_DOWN)  { if (dy <= ScI(8)) continue;  score = dy + fabsf(dx) * 3.0f; }
        else                     { if (dy >= -ScI(8)) continue; score = -dy + fabsf(dx) * 3.0f; }
        if (score < bestScore) { bestScore = score; best = i; }
    }
    if (best >= 0) { g_focus = best; FocusEnsureVisible(best); }
    else {
        if (vk == VK_RIGHT) g_scrollTarget += ScI(320);
        else if (vk == VK_LEFT) g_scrollTarget -= ScI(320);
        ClampScroll();
    }
    RepaintNow();
}

static std::wstring HeadingLabel(const AppHeading& h) {
    if (!h.text.empty()) return h.text;
    if (h.bucket >= 0) return T((StrId)(STR_DATE_TODAY + h.bucket));
    std::wstring s(1, h.first);
    if (h.last != h.first) { s += L'\x2013'; s += h.last; }
    return s;
}
#define kZoomTop   ScI(196)
#define kZoomTileH ScI(56)
#define kZoomGapY  ScI(10)
static int ZoomTileRows() {
    int rowsAvail = (g_sh - kZoomTop - ScI(64)) / (kZoomTileH + kZoomGapY);
    if (rowsAvail < 1) rowsAvail = 1;
    int colsAvail = (g_sw - kItemX - ScI(48)) / kItemPitchX;
    if (colsAvail < 1) colsAvail = 1;
    int n = (int)g_appHeadings.size();
    int rowsNeeded = std::max(1, (n + colsAvail - 1) / colsAvail);
    return std::max(1, std::min(rowsAvail, rowsNeeded));
}
static void ZoomTileRect(int pos, int rows, RECT* r) {
    int col = pos / rows, row = pos % rows;
    r->left = kItemX + col * kItemPitchX;
    r->top = kZoomTop + row * (kZoomTileH + kZoomGapY);
    r->right = r->left + kItemW;
    r->bottom = r->top + kZoomTileH;
}
static int HitHeading(int mx, int my) {
    if (g_view != VIEW_SEARCH || g_queryLen != 0) return -1;
    for (int i = 0; i < (int)g_appHeadings.size(); ++i) {
        float x = (float)(kItemX + g_appHeadings[i].col * kItemPitchX) - g_scrollCur;
        if (mx >= (int)x && mx <= (int)x + kItemW && my >= ScI(130) && my <= ScI(172)) return i;
    }
    return -1;
}
static void AlphaGridClick(int x, int y) {
    g_alphaGrid = false;
    g_searchTick = NowMs();
    int rows = ZoomTileRows();
    for (int i = 0; i < (int)g_appHeadings.size(); ++i) {
        RECT c; ZoomTileRect(i, rows, &c);
        if (x < c.left || x >= c.right || y < c.top || y >= c.bottom) continue;
        float tx = (float)(kItemX + g_appHeadings[i].col * kItemPitchX) - (float)LEFT;
        g_scrollTarget = std::max(0.0f, tx);
        ClampScroll();
        break;
    }
    RepaintNow();
}
static void DrawAlphaGridGpu(DWORD now) {
    float inK = EaseMetroOut(Clamp01((float)(now - g_alphaTick) / 180.0f));
    int rows = ZoomTileRows();
    IDWriteTextFormat* fmt = g_zoomFormat ? g_zoomFormat : g_subFormat;
    for (int i = 0; i < (int)g_appHeadings.size(); ++i) {
        RECT c; ZoomTileRect(i, rows, &c);
        if (c.left >= g_sw || c.top >= g_sh) continue;
        FillRc((float)c.left, (float)c.top, (float)c.right, (float)c.bottom, RGB(255, 255, 255), 0.10f * inK);
        std::wstring label = HeadingLabel(g_appHeadings[i]);
        TextAt(label.c_str(), fmt, (float)c.left + 12.0f, (float)c.top + 4.0f,
               (float)c.right - 8.0f, (float)c.bottom - 8.0f, RGB(235, 235, 245), inK);
    }
}
static void DrawAlphaGridGdi(DWORD) {
    if (!g_alphaGrid || g_view != VIEW_SEARCH || !g_bbDC) return;
    SetBkMode(g_bbDC, TRANSPARENT);
    SetTextColor(g_bbDC, RGB(235, 235, 245));
    HGDIOBJ of = SelectObject(g_bbDC, g_fTopLabel);
    int rows = ZoomTileRows();
    for (int i = 0; i < (int)g_appHeadings.size(); ++i) {
        RECT c; ZoomTileRect(i, rows, &c);
        if (c.left >= g_sw || c.top >= g_sh) continue;
        BlendFillGdi(g_bbDC, c.left, c.top, c.right, c.bottom, RGB(255, 255, 255), (BYTE)26);
        std::wstring label = HeadingLabel(g_appHeadings[i]);
        RECT tr = { c.left + 12, c.top + 4, c.right - 8, c.bottom - 8 };
        DrawTextW(g_bbDC, label.c_str(), -1, &tr, DT_LEFT | DT_BOTTOM | DT_WORDBREAK | DT_NOPREFIX);
    }
    SelectObject(g_bbDC, of);
}

static HFONT MakeFont(int px, const wchar_t* face) {
    return CreateFontW(-px, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, face);
}

typedef HWND (WINAPI *CreateWindowInBand_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                            HWND, HMENU, HINSTANCE, LPVOID, DWORD);
static CreateWindowInBand_t g_createInBand = NULL;

static HWND CreateTopWindow(int w, int h) {
    const DWORD ex = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    HWND hw = NULL;
    if (g_createInBand) {
        const DWORD bands[2] = { 2  , 16   };
        for (DWORD b : bands) {
            hw = g_createInBand(ex, kClassName, L"", WS_POPUP, 0, 0, w, h, NULL, NULL, g_inst, NULL, b);
            if (hw) { g_startBand = b; Wh_Log(L"Start screen created in z-band %u", (unsigned)b); break; }
            Wh_Log(L"CreateWindowInBand(%u) failed, error %u", (unsigned)b, (unsigned)GetLastError());
        }
    }
    if (!hw) {
        Wh_Log(L"Using an ordinary topmost window for the Start screen");
        hw = CreateWindowExW(ex, kClassName, L"", WS_POPUP, 0, 0, w, h, NULL, NULL, g_inst, NULL);
    }
    return hw;
}

static void FreeBuffers() {
    g_snapReady = false;
    ReleaseGpuTarget();
    g_bgBits = g_snapBits = NULL;
    for (int i = 0; i < g_tileCount; ++i) for (int v = 0; v < 2; ++v) {
        if (g_tileDC[i][v]) {
            if (g_tileOld[i][v]) SelectObject(g_tileDC[i][v], g_tileOld[i][v]);
            DeleteDC(g_tileDC[i][v]); g_tileDC[i][v] = NULL;
        }
        if (g_tileBmp[i][v]) { DeleteObject(g_tileBmp[i][v]); g_tileBmp[i][v] = NULL; }
        g_tileOld[i][v] = NULL;
        g_tileBits[i][v] = NULL;
    }
    if (g_bgDC) { SelectObject(g_bgDC, g_bgOld); DeleteDC(g_bgDC); g_bgDC = NULL; }
    if (g_bgBmp) { DeleteObject(g_bgBmp); g_bgBmp = NULL; }
    if (g_bbDC) { SelectObject(g_bbDC, g_bbOld); DeleteDC(g_bbDC); g_bbDC = NULL; }
    if (g_bbBmp) { DeleteObject(g_bbBmp); g_bbBmp = NULL; }
    if (g_snapDC) { SelectObject(g_snapDC, g_snapOld); DeleteDC(g_snapDC); g_snapDC = NULL; }
    if (g_snapBmp) { DeleteObject(g_snapBmp); g_snapBmp = NULL; }
    if (g_bgPlainBmp) { DeleteObject(g_bgPlainBmp); g_bgPlainBmp = NULL; }
    g_bgPlainBits = NULL;
}

static HBITMAP MakeDib(HDC ref, int w, int h, void** bits) {
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    return CreateDIBSection(ref, &bi, DIB_RGB_COLORS, bits, NULL, 0);
}


static const Gdiplus::PointF kBgFarLeft[] = {
    {-2.0f,617.2f},{7.0f,617.9f},{16.0f,621.0f},{25.0f,625.9f},
    {35.0f,632.9f},{39.0f,636.4f},{40.2f,639.0f},{40.6f,644.0f},
    {39.0f,647.7f},{34.0f,651.3f},{25.0f,653.3f},{10.0f,653.4f},
    {-2.0f,651.1f},
};

static const Gdiplus::PointF kBgLens[] = {
    {792.2f,654.0f},{792.3f,651.0f},{793.7f,648.0f},{798.0f,643.0f},
    {803.0f,639.0f},{814.0f,632.7f},{830.0f,626.8f},{862.0f,618.7f},
    {895.0f,612.6f},{924.0f,608.4f},{926.0f,608.6f},{927.3f,611.0f},
    {928.9f,619.0f},{928.4f,628.0f},{926.5f,636.0f},{923.2f,644.0f},
    {918.6f,652.0f},{912.0f,660.3f},{901.9f,669.0f},{895.1f,672.0f},
    {890.0f,672.8f},{866.0f,672.8f},{864.0f,672.1f},{855.0f,671.9f},
    {840.0f,669.3f},{798.0f,659.3f},{795.0f,657.8f},
};

static const Gdiplus::PointF kBgPetal[] = {
    {568.0f,683.3f},{566.7f,682.0f},{565.9f,679.0f},{568.3f,673.0f},
    {570.5f,663.0f},{570.5f,636.0f},{569.5f,633.0f},{569.5f,620.0f},
    {570.7f,616.0f},{575.0f,608.6f},{583.0f,600.5f},{589.0f,595.7f},
    {605.0f,584.7f},{613.0f,580.7f},{619.0f,578.7f},{626.0f,577.5f},
    {633.0f,577.5f},{645.0f,574.5f},{650.4f,576.0f},{652.5f,581.0f},
    {651.5f,584.0f},{651.5f,592.0f},{650.3f,597.0f},{644.3f,610.0f},
    {635.7f,621.0f},{634.0f,627.4f},{628.6f,631.0f},{625.0f,634.6f},
    {622.7f,638.0f},{621.3f,642.0f},{616.7f,646.0f},{606.3f,663.0f},
    {602.5f,668.0f},{592.4f,678.0f},{585.0f,683.3f},{582.0f,684.3f},
    {574.0f,683.9f},{572.0f,684.5f},
};

static const Gdiplus::PointF kBgLeftDomeTop[] = {
    {145.0f,717.6f},{157.0f,705.2f},{169.0f,696.2f},{181.0f,689.6f},
    {189.0f,686.3f},{201.0f,682.6f},{205.0f,679.9f},{209.0f,680.8f},
    {221.0f,680.2f},{233.0f,680.6f},{245.0f,682.6f},{257.0f,686.4f},
    {269.0f,691.9f},{277.0f,696.5f},{289.0f,704.6f},{309.0f,719.5f},
};

static const Gdiplus::PointF kBgDomeTop[] = {
    {619.0f,719.0f},{643.0f,705.9f},{663.0f,696.2f},{679.0f,689.3f},
    {683.0f,685.4f},{687.0f,685.5f},{691.0f,684.7f},{707.0f,679.1f},
    {727.0f,673.7f},{739.0f,671.5f},{755.0f,670.2f},{767.0f,670.6f},
    {779.0f,672.7f},{791.0f,676.6f},{803.0f,682.5f},{815.0f,691.1f},
    {823.0f,699.1f},{831.0f,710.4f},{835.0f,718.9f},
};

static const Gdiplus::PointF kBgLinePts[] = {
    {873.0f,511.2f},{887.7f,515.0f},{894.3f,518.3f},{906.7f,527.0f},
    {918.1f,538.1f},{928.0f,550.0f},{940.7f,568.7f},{957.2f,606.0f},
    {854.0f,525.0f},{861.1f,527.1f},{879.6f,540.6f},{897.4f,555.4f},
    {914.4f,572.4f},{929.0f,590.3f},{941.0f,609.3f},{950.4f,629.4f},
    {957.0f,648.5f},{1128.0f,572.4f},{1108.0f,604.0f},{1088.9f,628.1f},
    {1077.6f,639.4f},{1065.7f,649.3f},{1059.5f,653.5f},{1052.9f,657.0f},
    {1038.6f,661.0f},{1030.6f,661.0f},{1023.0f,660.0f},{1015.8f,658.0f},
    {1002.3f,652.0f},{990.0f,643.0f},{978.7f,631.7f},{969.0f,619.7f},
    {960.3f,607.3f},{957.2f,606.0f},{1135.0f,589.0f},{1133.6f,596.4f},
    {1127.9f,610.1f},{1113.8f,636.2f},{1101.0f,654.9f},{1091.2f,666.8f},
    {1079.6f,677.4f},{1066.8f,685.2f},{1059.9f,688.1f},{1052.8f,690.0f},
    {1045.2f,691.0f},{1037.6f,690.0f},{1032.4f,691.6f},{957.2f,606.0f},
    {958.0f,613.6f},{964.0f,633.8f},{964.0f,633.8f},{957.0f,648.5f},
    {964.0f,633.8f},{968.6f,639.6f},{976.4f,652.4f},{981.3f,658.3f},
    {992.6f,669.6f},{1004.8f,678.8f},{1017.8f,686.0f},{1032.4f,691.6f},
    {957.0f,648.5f},{956.0f,656.1f},{953.8f,663.2f},{939.7f,689.3f},
    {924.8f,707.2f},{912.9f,717.1f},{908.0f,720.0f},{1032.4f,691.6f},
    {1038.2f,696.2f},{1051.8f,702.0f},{1072.9f,709.0f},{1108.8f,719.0f},
    {1115.0f,720.0f},{1060.0f,527.0f},{1052.6f,528.4f},{1042.3f,533.3f},
    {1042.3f,533.3f},{1036.6f,538.4f},{1032.0f,544.6f},{1030.0f,551.7f},
    {1031.2f,559.2f},{1036.9f,564.9f},{1043.4f,568.4f},{1057.1f,574.0f},
    {1064.3f,576.0f},{1071.9f,577.0f},{1079.0f,579.0f},{1086.7f,579.7f},
    {1102.6f,580.0f},{1110.2f,579.0f},{1115.7f,574.3f},{1118.5f,567.5f},
    {1120.0f,560.1f},{1120.0f,554.0f},{675.0f,582.0f},{682.4f,583.4f},
    {684.0f,590.8f},{680.6f,597.4f},{675.0f,603.0f},{662.5f,611.5f},
    {637.0f,625.0f},{624.0f,641.0f},{650.3f,653.3f},{656.3f,658.3f},
    {659.0f,662.8f},{659.0f,662.8f},{627.5f,683.5f},{603.1f,702.0f},
    {585.8f,718.2f},{583.0f,720.0f},{665.2f,671.2f},{615.9f,706.1f},
    {610.2f,711.8f},{600.0f,720.0f},{665.2f,671.2f},{680.0f,683.0f},
    {210.0f,644.0f},{205.8f,650.2f},{203.0f,665.1f},{204.0f,672.7f},
    {206.0f,676.0f},{406.8f,656.8f},{408.0f,664.3f},{410.3f,671.3f},
    {416.0f,685.0f},{419.5f,691.5f},{430.1f,703.1f},{442.0f,713.0f},
    {448.3f,717.3f},{455.0f,720.0f},{510.0f,706.0f},{505.0f,710.2f},
    {505.0f,718.0f},{535.3f,708.3f},{532.0f,719.0f},{631.3f,625.8f},
    {618.3f,633.7f},{610.7f,641.0f},{590.0f,636.0f},{598.0f,636.0f},
    {605.1f,638.1f},{609.3f,642.3f},{609.1f,636.9f},{604.0f,643.0f},
    {598.0f,648.0f},{585.2f,660.8f},{207.0f,679.0f},{207.7f,686.7f},
    {214.8f,699.8f},{224.7f,711.7f},{230.6f,716.6f},{235.3f,719.2f},
    {682.7f,685.8f},{695.4f,694.4f},{720.9f,709.9f},{734.0f,717.0f},
    {742.0f,717.0f},{570.0f,635.0f},{580.0f,635.8f},{590.5f,636.5f},
    {609.5f,642.0f},{616.0f,642.8f},{623.0f,643.0f},{585.2f,660.8f},
    {580.0f,665.5f},{576.0f,669.5f},{659.0f,662.8f},{662.0f,667.0f},
    {665.2f,671.2f},{740.0f,716.8f},{746.0f,719.0f},{752.0f,721.0f},
};

static const BYTE kBgLineLens[] = { 8,9,17,13,3,2,8,7,6,3,18,7,4,5,4,2,5,9,3,2,3,4,4,6,5,3,3,3,3,3 };

static const Gdiplus::PointF kBgRibbon[] = {
    {91.0f,716.6f},{100.0f,716.1f},{122.1f,711.9f},{139.3f,706.4f},
    {147.6f,702.6f},{155.5f,698.1f},{166.7f,690.3f},{179.7f,677.6f},
    {193.3f,659.7f},{199.5f,653.3f},{212.9f,641.6f},{220.1f,636.4f},
    {231.5f,629.5f},{239.4f,625.6f},{247.5f,622.2f},{260.2f,618.3f},
    {268.7f,616.5f},{273.1f,616.3f},{277.4f,615.4f},{299.5f,615.7f},
    {303.8f,616.7f},{308.1f,616.7f},{321.2f,619.0f},{333.8f,622.5f},
    {341.9f,625.7f},{345.8f,627.7f},{364.9f,639.4f},{373.1f,643.7f},
    {381.6f,647.2f},{394.7f,651.4f},{403.6f,653.4f},{407.9f,654.8f},
    {416.5f,656.4f},{428.8f,660.7f},{436.8f,664.2f},{452.0f,673.2f},
    {477.6f,691.4f},{489.4f,698.6f},{497.9f,702.4f},{506.8f,705.5f},
    {516.1f,707.3f},{530.1f,707.3f},{539.5f,706.1f},{548.6f,703.2f},
    {557.2f,699.0f},{561.2f,696.2f},{568.1f,689.3f},{570.9f,685.4f},
    {577.4f,672.9f},{580.4f,664.0f},{586.8f,637.8f},{589.4f,629.4f},
    {592.4f,621.3f},{596.2f,613.7f},{603.7f,603.5f},{607.1f,600.9f},
    {610.1f,598.0f},{624.6f,588.6f},{636.6f,584.3f},{653.7f,581.1f},
    {671.0f,581.0f},{684.1f,582.1f},{697.6f,583.9f},{707.3f,584.0f},
    {712.4f,583.3f},{717.1f,581.1f},{721.3f,577.9f},{724.9f,574.3f},
    {727.8f,570.4f},{738.7f,550.4f},{745.8f,539.5f},{751.5f,533.0f},
    {757.6f,527.1f},{764.5f,522.9f},{768.4f,521.7f},{776.4f,520.4f},
    {780.6f,520.4f},{788.9f,521.7f},{801.6f,525.2f},{810.1f,528.1f},
    {823.6f,531.3f},{828.3f,532.1f},{833.2f,532.3f},{838.0f,531.6f},
    {847.3f,528.7f},{855.6f,523.7f},{859.3f,521.0f},{869.6f,512.0f},
    {879.4f,502.8f},{893.7f,493.0f},{897.5f,491.1f},{905.3f,488.1f},
    {909.5f,487.2f},{913.7f,487.0f},{917.9f,486.2f},{931.1f,486.3f},
    {939.8f,487.2f},{944.2f,487.2f},{961.2f,490.9f},{973.3f,495.5f},
    {984.4f,502.2f},{987.7f,505.1f},{997.0f,514.4f},{1005.0f,525.5f},
    {1008.5f,528.8f},{1012.6f,531.5f},{1017.2f,533.5f},{1022.0f,534.6f},
    {1031.7f,534.1f},{1057.5f,526.6f},{1061.8f,526.1f},{1070.4f,526.1f},
    {1083.2f,528.4f},{1091.4f,531.4f},{1099.2f,535.2f},{1103.0f,537.4f},
    {1110.0f,542.7f},{1116.2f,548.8f},{1123.8f,559.5f},{1127.8f,567.4f},
    {1133.8f,584.2f},{1137.0f,597.1f},{1137.0f,601.5f},{1138.4f,605.7f},
    {1139.1f,614.7f},{1139.2f,628.1f},{1138.4f,632.4f},{1139.2f,633.0f},
    {1139.5f,632.5f},{1140.5f,628.1f},{1140.3f,605.5f},{1139.0f,601.2f},
    {1139.2f,596.7f},{1136.4f,583.5f},{1130.9f,566.1f},{1126.8f,557.8f},
    {1119.1f,546.4f},{1112.5f,539.8f},{1105.2f,534.1f},{1101.3f,531.7f},
    {1093.0f,527.6f},{1084.3f,524.4f},{1070.8f,521.8f},{1056.9f,522.0f},
    {1030.9f,529.4f},{1022.6f,529.8f},{1018.7f,528.9f},{1015.0f,527.2f},
    {1011.6f,524.9f},{1008.6f,521.9f},{1000.9f,511.0f},{994.6f,504.3f},
    {987.8f,498.0f},{975.8f,490.6f},{971.6f,488.7f},{962.9f,485.5f},
    {945.0f,481.4f},{931.4f,480.3f},{917.6f,480.1f},{913.0f,480.9f},
    {908.3f,481.1f},{903.8f,482.1f},{894.8f,485.4f},{886.6f,489.9f},
    {875.3f,498.1f},{865.4f,507.2f},{851.9f,518.4f},{844.8f,522.8f},
    {836.9f,525.2f},{832.9f,525.8f},{824.7f,524.8f},{812.0f,521.7f},
    {803.5f,518.8f},{790.2f,515.2f},{780.9f,513.7f},{776.2f,513.7f},
    {766.5f,515.3f},{761.9f,516.6f},{753.4f,521.7f},{743.3f,531.6f},
    {737.6f,539.0f},{732.7f,546.9f},{722.0f,566.3f},{716.7f,572.5f},
    {713.8f,574.8f},{710.3f,576.5f},{706.4f,576.9f},{698.0f,576.8f},
    {685.0f,575.0f},{671.4f,573.8f},{652.8f,574.0f},{630.1f,578.1f},
    {625.5f,579.6f},{621.2f,581.7f},{605.8f,592.4f},{598.6f,598.6f},
    {592.8f,606.1f},{590.3f,610.1f},{586.1f,618.5f},{582.9f,627.2f},
    {573.7f,662.0f},{571.0f,670.2f},{565.2f,681.8f},{562.8f,685.2f},
    {557.1f,691.0f},{553.7f,693.3f},{546.2f,697.1f},{538.0f,699.7f},
    {529.7f,700.9f},{516.7f,700.9f},{508.5f,699.3f},{500.3f,696.4f},
    {492.4f,692.9f},{481.2f,686.1f},{455.4f,667.9f},{439.7f,658.6f},
    {431.2f,654.9f},{418.1f,650.4f},{409.2f,648.8f},{396.2f,645.6f},
    {383.6f,641.7f},{375.4f,638.5f},{367.6f,634.5f},{348.3f,623.0f},
    {344.2f,620.9f},{335.5f,617.6f},{322.2f,614.0f},{308.9f,611.8f},
    {304.3f,611.9f},{299.8f,610.9f},{277.0f,610.9f},{272.5f,611.9f},
    {268.0f,612.2f},{259.1f,614.2f},{246.1f,618.3f},{237.7f,621.9f},
    {229.5f,626.1f},{218.0f,633.4f},{210.6f,638.7f},{203.7f,644.7f},
    {197.4f,651.1f},{191.1f,657.7f},{177.7f,675.8f},{165.2f,688.6f},
    {158.1f,693.9f},{150.6f,698.6f},{138.6f,704.6f},{126.0f,709.2f},
    {113.0f,712.5f},{99.8f,714.7f},{95.3f,714.9f},{91.0f,716.3f},
};


static void FillBgClosedCurve(Gdiplus::Graphics& g, const Gdiplus::PointF* pts, int n,
                              Gdiplus::Brush* brush) {
    Gdiplus::GraphicsPath path;
    path.AddClosedCurve(pts, n);
    g.FillPath(brush, &path);
}

static void FillBgDome(Gdiplus::Graphics& g, const Gdiplus::PointF* top, int n,
                       float bx0, float bx1, Gdiplus::Brush* brush) {
    Gdiplus::GraphicsPath path;
    path.AddLine(Gdiplus::PointF(bx0, 722.0f), top[0]);
    path.AddCurve(top, n);
    path.AddLine(top[n - 1], Gdiplus::PointF(bx1, 722.0f));
    path.CloseFigure();
    g.FillPath(brush, &path);
}

static bool DrawVectorBackground(void* bits, int w, int h) {
    if (!g_gdipToken || !bits || w < 2 || h < 2) return false;
    Gdiplus::Bitmap target(w, h, w * 4, PixelFormat32bppRGB, (BYTE*)bits);
    if (target.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::Graphics g(&target);
    if (g.GetLastStatus() != Gdiplus::Ok) return false;
    g.Clear(Gdiplus::Color(255, 36, 0, 64));
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    const float scale = std::max(w / 1280.0f, h / 721.0f);
    const float ox = (w - 1280.0f * scale) * 0.5f, oy = (h - 721.0f * scale) * 0.5f;
    Gdiplus::Matrix m(scale, 0.0f, 0.0f, scale, ox, oy);
    g.SetTransform(&m);

    {
        Gdiplus::SolidBrush br(Gdiplus::Color(255, 74, 24, 109));
        Gdiplus::GraphicsPath path;
        path.AddPolygon(kBgFarLeft, (INT)(sizeof(kBgFarLeft) / sizeof(kBgFarLeft[0])));
        g.FillPath(&br, &path);
    }
    {
        Gdiplus::LinearGradientBrush br(Gdiplus::PointF(0.0f, 679.0f), Gdiplus::PointF(0.0f, 721.0f),
            Gdiplus::Color(255, 88, 37, 127), Gdiplus::Color(255, 53, 11, 83));
        FillBgDome(g, kBgLeftDomeTop, (INT)(sizeof(kBgLeftDomeTop) / sizeof(kBgLeftDomeTop[0])),
                   143.0f, 311.0f, &br);
    }
    {
        Gdiplus::LinearGradientBrush br(Gdiplus::PointF(0.0f, 669.0f), Gdiplus::PointF(0.0f, 721.0f),
            Gdiplus::Color(255, 88, 37, 127), Gdiplus::Color(255, 64, 18, 97));
        FillBgDome(g, kBgDomeTop, (INT)(sizeof(kBgDomeTop) / sizeof(kBgDomeTop[0])),
                   617.0f, 837.0f, &br);
    }
    {
        Gdiplus::LinearGradientBrush br(Gdiplus::PointF(0.0f, 584.0f), Gdiplus::PointF(0.0f, 675.0f),
            Gdiplus::Color(255, 86, 36, 124), Gdiplus::Color(255, 42, 4, 72));
        FillBgClosedCurve(g, kBgPetal, (INT)(sizeof(kBgPetal) / sizeof(kBgPetal[0])), &br);
    }
    {
        Gdiplus::LinearGradientBrush br(Gdiplus::PointF(0.0f, 609.0f), Gdiplus::PointF(0.0f, 674.0f),
            Gdiplus::Color(255, 87, 36, 125), Gdiplus::Color(255, 37, 1, 65));
        FillBgClosedCurve(g, kBgLens, (INT)(sizeof(kBgLens) / sizeof(kBgLens[0])), &br);
    }

    {
        Gdiplus::GraphicsPath path;
        int off = 0;
        for (BYTE n : kBgLineLens) {
            path.StartFigure();
            if (n == 2) path.AddLine(kBgLinePts[off], kBgLinePts[off + 1]);
            else path.AddCurve(&kBgLinePts[off], (INT)n);
            off += n;
        }
        Gdiplus::Pen pen(Gdiplus::Color(97, 111, 58, 149), 3.8f);
        pen.SetLineJoin(Gdiplus::LineJoinRound);
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        g.DrawPath(&pen, &path);
    }

    {
        Gdiplus::SolidBrush br(Gdiplus::Color(225, 121, 66, 160));
        Gdiplus::GraphicsPath path;
        path.AddPolygon(kBgRibbon, (INT)(sizeof(kBgRibbon) / sizeof(kBgRibbon[0])));
        g.FillPath(&br, &path);
    }
    return g.GetLastStatus() == Gdiplus::Ok;
}

static bool DrawWallpaperBackground(void* bits, int w, int h, COLORREF overlay) {
    if (!g_gdipToken || !bits || w < 2 || h < 2) return false;
    wchar_t path[32768] = {};
    if (!GetWallpaperPath(path, sizeof(path) / sizeof(path[0]))) return false;
    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) return false;
    Gdiplus::Bitmap image(path, FALSE);
    if (image.GetLastStatus() != Gdiplus::Ok || !image.GetWidth() || !image.GetHeight()) return false;
    Gdiplus::Bitmap target(w, h, w * 4, PixelFormat32bppRGB, (BYTE*)bits);
    if (target.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::Graphics g(&target);
    if (g.GetLastStatus() != Gdiplus::Ok) return false;
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    int sw = (int)image.GetWidth(), sh = (int)image.GetHeight();
    Gdiplus::Rect dest(0, 0, w, h);
    if ((double)sw * h >= (double)sh * w) {
        int cw = (int)((double)sh * w / h + 0.5); if (cw < 1) cw = 1; if (cw > sw) cw = sw;
        if (g.DrawImage(&image, dest, (sw - cw) / 2, 0, cw, sh, Gdiplus::UnitPixel) != Gdiplus::Ok) return false;
    } else {
        int ch = (int)((double)sw * h / w + 0.5); if (ch < 1) ch = 1; if (ch > sh) ch = sh;
        if (g.DrawImage(&image, dest, 0, (sh - ch) / 2, sw, ch, Gdiplus::UnitPixel) != Gdiplus::Ok) return false;
    }
    Gdiplus::SolidBrush tint(Gdiplus::Color(96, GetRValue(overlay), GetGValue(overlay), GetBValue(overlay)));
    if (g.FillRectangle(&tint, 0, 0, w, h) != Gdiplus::Ok) return false;
    return true;
}

static bool DrawCustomImageBackground(void* bits, int w, int h) {
    if (!g_gdipToken || !bits || w < 2 || h < 2) return false;
    std::wstring path = BgImagePath();
    if (path.empty()) return false;
    if (path.find(L'%') != std::wstring::npos) {
        DWORD need = ExpandEnvironmentStringsW(path.c_str(), NULL, 0);
        if (need > 1 && need < 32768) {
            std::wstring exp(need, 0);
            if (ExpandEnvironmentStringsW(path.c_str(), &exp[0], need)) path = exp.c_str();
        }
    }
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return false;
    Gdiplus::Bitmap image(path.c_str(), FALSE);
    if (image.GetLastStatus() != Gdiplus::Ok || !image.GetWidth() || !image.GetHeight()) return false;
    Gdiplus::Bitmap target(w, h, w * 4, PixelFormat32bppRGB, (BYTE*)bits);
    if (target.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::Graphics g(&target);
    if (g.GetLastStatus() != Gdiplus::Ok) return false;
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    const int sw = (int)image.GetWidth(), sh = (int)image.GetHeight();
    Gdiplus::Rect dest(0, 0, w, h);
    if ((double)sw * h >= (double)sh * w) {
        int cw = (int)((double)sh * w / h + 0.5); if (cw < 1) cw = 1; if (cw > sw) cw = sw;
        return g.DrawImage(&image, dest, (sw - cw) / 2, 0, cw, sh, Gdiplus::UnitPixel) == Gdiplus::Ok;
    }
    int ch = (int)((double)sw * h / w + 0.5); if (ch < 1) ch = 1; if (ch > sh) ch = sh;
    return g.DrawImage(&image, dest, 0, (sh - ch) / 2, sw, ch, Gdiplus::UnitPixel) == Gdiplus::Ok;
}

static void BuildBuffers() {
    FreeBuffers();
    HDC sdc = GetDC(NULL);
    void* bgBits = NULL; void* bbBits = NULL;
    g_bgBmp = MakeDib(sdc, g_sw, g_sh, &bgBits);
    g_bbBmp = MakeDib(sdc, g_sw, g_sh, &bbBits);
    g_snapBmp = MakeDib(sdc, g_sw, g_sh, &g_snapBits);
    g_bgPlainBmp = MakeDib(sdc, g_sw, g_sh, &g_bgPlainBits);
    g_bgDC = CreateCompatibleDC(sdc);
    g_bbDC = CreateCompatibleDC(sdc);
    g_snapDC = CreateCompatibleDC(sdc);
    ReleaseDC(NULL, sdc);
    if (!g_bgBmp || !g_bbBmp || !g_snapBmp || !bgBits || !g_snapBits) return;
    g_bgBits = bgBits;
    g_bgOld = SelectObject(g_bgDC, g_bgBmp);
    g_bbOld = SelectObject(g_bbDC, g_bbBmp);
    g_snapOld = SelectObject(g_snapDC, g_snapBmp);

    const int bgMode = (int)InterlockedCompareExchange(&g_bgMode, 0, 0);
    bool baseDone = false;
    if (bgMode == 1 || bgMode == 2) {
        COLORREF solid = (bgMode == 2) ? SystemAccentColor() : g_bgColor;
        DWORD* px = (DWORD*)bgBits;
        DWORD fill = ((DWORD)GetRValue(solid) << 16) | ((DWORD)GetGValue(solid) << 8) | (DWORD)GetBValue(solid);
        const size_t count = (size_t)g_sw * (size_t)g_sh;
        for (size_t i = 0; i < count; ++i) px[i] = fill;
        baseDone = true;
    } else if (bgMode == 3) {
        baseDone = DrawWallpaperBackground(bgBits, g_sw, g_sh, g_bgColor);
        if (!baseDone)
            Wh_Log(L"Start background: desktop wallpaper unavailable; using the built-in 8.1 artwork");
    } else if (bgMode == 4) {
        baseDone = DrawCustomImageBackground(bgBits, g_sw, g_sh);
        if (!baseDone)
            Wh_Log(L"Start background: custom image unavailable; using the built-in 8.1 artwork");
    }
    if (!baseDone && !DrawVectorBackground(bgBits, g_sw, g_sh)) {
        DWORD* px = (DWORD*)bgBits;
        for (int y = 0; y < g_sh; y++) {
            float fy = ((float)y / (float)(g_sh - 1) - 0.5f) * 2.0f;
            for (int x = 0; x < g_sw; x++) {
                float fx = ((float)x / (float)(g_sw - 1) - 0.5f) * 2.0f;
                float d = fx * fx * 0.55f + fy * fy * 0.45f;
                float k = 1.0f - Clamp01(d);
                k = k * k * k;
                int r = (int)(31.0f + 10.4f * k);
                int g = (int)(13.0f + 4.0f * k);
                int b = (int)(46.0f + 15.2f * k);
                px[y * g_sw + x] = ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
            }
        }
    }

    if (g_bgPlainBits) memcpy(g_bgPlainBits, bgBits, (size_t)g_sw * (size_t)g_sh * 4);
    HDC tileScreen = GetDC(NULL);
    if (tileScreen) { BuildTileBuffers(tileScreen); ReleaseDC(NULL, tileScreen); }
    GdiFlush();
    g_dirty = true;
}

static COLORREF Mix(COLORREF a, COLORREF b, float k) {
    k = Clamp01(k);
    return RGB((int)(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * k),
               (int)(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * k),
               (int)(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * k));
}

static void ComputePivot() {
    int left[MAX_TILES], right[MAX_TILES];
    for (int b = 0; b < MAX_TILES; ++b) {
        left[b] = 1000000;
        right[b] = -1000000;
    }
    g_blockCount = 0;
    const float blockWidth = (float)GroupPitch();
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive) continue;
        const Tile& t = g_tiles[i];
        int b = (int)((t.tx - (float)LEFT + 0.5f) / blockWidth);
        if (b < 0) b = 0;
        if (b >= MAX_TILES) b = MAX_TILES - 1;
        g_tileBlock[i] = b;
        if (b + 1 > g_blockCount) g_blockCount = b + 1;
        int x = (int)(t.tx + 0.5f);
        int w = TilePxW(t.cw);
        if (x < left[b]) left[b] = x;
        if (x + w > right[b]) right[b] = x + w;
    }
    for (int b = 0; b < g_blockCount; ++b) {
        g_blockPivotX[b] = right[b] >= left[b] ? (float)(left[b] + right[b]) * 0.5f : 0.0f;
    }
}

static bool GetWallpaperPath(wchar_t* out, size_t capacity) {
    if (!out || capacity < 2) return false;
    out[0] = 0;
    IDesktopWallpaper* desktop = NULL;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, NULL, CLSCTX_ALL,
                                   IID_PPV_ARGS(&desktop))) && desktop) {
        LPWSTR primaryId = NULL;
        UINT count = 0;
        if (SUCCEEDED(desktop->GetMonitorDevicePathCount(&count))) {
            for (UINT i = 0; i < count; ++i) {
                LPWSTR id = NULL;
                if (SUCCEEDED(desktop->GetMonitorDevicePathAt(i, &id)) && id) {
                    RECT rect = {};
                    if (SUCCEEDED(desktop->GetMonitorRECT(id, &rect)) &&
                        rect.left <= 0 && rect.right > 0 && rect.top <= 0 && rect.bottom > 0) {
                        primaryId = id;
                        break;
                    }
                    CoTaskMemFree(id);
                }
            }
        }
        LPWSTR path = NULL;
        HRESULT hr = desktop->GetWallpaper(primaryId, &path);
        if (SUCCEEDED(hr) && path && *path) {
            wcsncpy(out, path, capacity - 1);
            out[capacity - 1] = 0;
        }
        if (path) CoTaskMemFree(path);
        if (primaryId) CoTaskMemFree(primaryId);
        desktop->Release();
    }
    if (!out[0])
        SystemParametersInfoW(SPI_GETDESKWALLPAPER, (UINT)capacity, out, 0);
    return out[0] != 0;
}

static void DrawDesktopWallpaperTile(HDC dc, HDC screen, int w, int h) {
    RECT entire = { 0, 0, w, h };
    HBRUSH fallback = CreateSolidBrush(RGB(25, 9, 98));
    FillRect(dc, &entire, fallback);
    DeleteObject(fallback);
    if (g_wallVirtReady && g_wallVirtW > 1 && g_wallVirtH > 1) {
        int srcH = g_wallVirtH;
        int srcW = (int)((float)srcH * (float)w / (float)h + 0.5f);
        if (srcW > g_wallVirtW) srcW = g_wallVirtW;
        if (srcW < 1) srcW = 1;
        int srcX = (g_wallVirtW - srcW) / 2;
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = g_wallVirtW;
        info.bmiHeader.biHeight = -g_wallVirtH;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        SetStretchBltMode(dc, HALFTONE);
        SetBrushOrgEx(dc, 0, 0, NULL);
        StretchDIBits(dc, 0, 0, w, h, srcX, 0, srcW, srcH,
                      g_wallVirt.data(), &info, DIB_RGB_COLORS, SRCCOPY);
    } else if (g_snapReady && g_snapDC && screen) {
        SetStretchBltMode(dc, COLORONCOLOR);
        StretchBlt(dc, 0, 0, w, h, g_snapDC, 0, 0, g_sw, g_sh, SRCCOPY);
    }
    if (h < ScI(100)) return;
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, g_fLabel);
    RECT label = { 12, h - 30, w - 8, h - 8 };
    RECT shadow = label; OffsetRect(&shadow, 1, 1);
    SetTextColor(dc, RGB(17, 12, 35));
    DrawTextW(dc, T(STR_DESKTOP), -1, &shadow, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, RGB(255, 255, 255));
    DrawTextW(dc, T(STR_DESKTOP), -1, &label, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
}

static void DrawModernTileCaption(HDC dc, const wchar_t* text, int w, int h, bool dark = false) {
    if (!text || !*text || h < ScI(100)) return;
    SetBkMode(dc, TRANSPARENT);
    SelectObject(dc, g_fLabel);
    RECT label = { 12, h - 30, w - 8, h - 8 };
    if (dark) {
        SetTextColor(dc, RGB(0, 0, 0));
        DrawTextW(dc, text, -1, &label, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
        return;
    }
    RECT shadow = label; OffsetRect(&shadow, 1, 1);
    SetTextColor(dc, RGB(17, 12, 35));
    DrawTextW(dc, text, -1, &shadow, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
    SetTextColor(dc, RGB(255, 255, 255));
    DrawTextW(dc, text, -1, &label, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
}

static void DrawTileHover(HDC dc, int w, int h) {
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(235, 235, 235));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, 1, 1, w, h);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

static void GlyphStoreBagV(Gdiplus::Graphics& gp, float x, float y, float s) {
    Gdiplus::SolidBrush bw(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::Pen pw(Gdiplus::Color(255, 255, 255, 255), s * 0.08f);
    gp.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    gp.FillRectangle(&bw, x + s * 0.12f, y + s * 0.34f, s * 0.76f, s * 0.56f);
    gp.DrawArc(&pw, x + s * 0.32f, y + s * 0.10f, s * 0.36f, s * 0.44f, 180, 180);
}
static bool IsPackagedAppTarget(const wchar_t* target);

static Gdiplus::Bitmap* IconToGpBitmap(HICON ico) {
    if (!ico) return NULL;
    ICONINFO ii = {};
    if (!GetIconInfo(ico, &ii)) return NULL;
    BITMAP bm = {};
    int w = 0, hh = 0;
    const bool haveColor = ii.hbmColor != NULL;
    if (haveColor) { GetObject(ii.hbmColor, sizeof(bm), &bm); w = bm.bmWidth; hh = bm.bmHeight; }
    else { GetObject(ii.hbmMask, sizeof(bm), &bm); w = bm.bmWidth; hh = bm.bmHeight / 2; }
    Gdiplus::Bitmap* out = NULL;
    if (w > 0 && hh > 0 && w <= 512 && hh <= 512) {
        std::vector<BYTE> px((size_t)w * (size_t)hh * 4, 0);
        bool alphaOk = false;
        HDC dc = GetDC(NULL);
        if (dc) {
            if (haveColor) {
                BITMAPINFO bi = {};
                bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = w;
                bi.bmiHeader.biHeight = -hh; bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
                if (GetDIBits(dc, ii.hbmColor, 0, (UINT)hh, px.data(), &bi, DIB_RGB_COLORS)) {
                    if (bm.bmBitsPixel == 32)
                        for (size_t n = 3; n < px.size(); n += 4) if (px[n]) { alphaOk = true; break; }
                }
            }
            if (!alphaOk && ii.hbmMask && haveColor) {
                const int stride = ((w + 15) / 16) * 2;
                std::vector<BYTE> mk((size_t)stride * (size_t)hh, 0);
                BITMAPINFO mi = {};
                mi.bmiHeader.biSize = sizeof(mi.bmiHeader); mi.bmiHeader.biWidth = w;
                mi.bmiHeader.biHeight = -hh; mi.bmiHeader.biPlanes = 1;
                mi.bmiHeader.biBitCount = 1; mi.bmiHeader.biCompression = BI_RGB;
                if (GetDIBits(dc, ii.hbmMask, 0, (UINT)hh, mk.data(), &mi, DIB_RGB_COLORS))
                    for (int y = 0; y < hh; ++y)
                        for (int x = 0; x < w; ++x) {
                            const bool transparent = (mk[(size_t)y * stride + (x >> 3)] >> (7 - (x & 7))) & 1;
                            px[((size_t)y * w + x) * 4 + 3] = transparent ? 0 : 255;
                        }
            }
            ReleaseDC(NULL, dc);
        }
        if (haveColor) {
            out = new Gdiplus::Bitmap(w, hh, PixelFormat32bppARGB);
            Gdiplus::Rect rc(0, 0, w, hh);
            Gdiplus::BitmapData bd;
            if (out->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bd) == Gdiplus::Ok) {
                for (int y = 0; y < hh; ++y)
                    memcpy((BYTE*)bd.Scan0 + (size_t)y * bd.Stride, px.data() + (size_t)y * w * 4, (size_t)w * 4);
                out->UnlockBits(&bd);
            } else { delete out; out = NULL; }
        }
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return out;
}

static void BuildOneTile(int i, HDC screenDC) {
    for (int variant = 0; variant < 2; ++variant) {
        const Tile& t = g_tiles[i];
        RECT r; TileRect(t, &r);
        int w = r.right - r.left, h = r.bottom - r.top;
        HDC tdc = CreateCompatibleDC(screenDC);
        if (!tdc) continue;
        void* bits = NULL;
        HBITMAP tb = MakeDib(screenDC, w, h, &bits);
        if (!tb) { DeleteDC(tdc); continue; }
        HGDIOBJ old = SelectObject(tdc, tb);
        if (!old) { DeleteObject(tb); DeleteDC(tdc); continue; }
        g_tileDC[i][variant] = tdc;
        g_tileBmp[i][variant] = tb;
        g_tileBits[i][variant] = bits;
        g_tileOld[i][variant] = old;
        bool hover = variant != 0;
        std::vector<BYTE> art;
        int aw = 0, ah = 0;
        COLORREF c1 = t.c1, c2 = t.c2;
        if (t.hasAppC && (t.style == PINNED_APP || InterlockedCompareExchange(&g_appTileColors, 0, 0))) c1 = c2 = t.appC;
        if (t.style == PINNED_APP && t.target) {
            AcquireSRWLockShared(&g_artLock);
            for (const auto& entry : g_packageArts) if (!_wcsicmp(entry.target.c_str(), t.target)) {
                if (t.cw == TS_WIDE) { art = entry.wide; aw = entry.wideW; ah = entry.wideH; }
                else { art = entry.square; aw = entry.squareW; ah = entry.squareH; }
                if (entry.hasBackground) c1 = c2 = entry.background;
                break;
            }
            ReleaseSRWLockShared(&g_artLock);
        }
        if (c1 == c2) {
            HBRUSH br = CreateSolidBrush(c1);
            RECT fr = { 0, 0, w, h };
            FillRect(tdc, &fr, br);
            DeleteObject(br);
        } else {
            TRIVERTEX v[2] = {
                { 0, 0, (COLOR16)(GetRValue(c1) << 8), (COLOR16)(GetGValue(c1) << 8), (COLOR16)(GetBValue(c1) << 8), 0 },
                { w, h, (COLOR16)(GetRValue(c2) << 8), (COLOR16)(GetGValue(c2) << 8), (COLOR16)(GetBValue(c2) << 8), 0 },
            };
            GRADIENT_RECT gr = { 0, 1 };
            GradientFill(tdc, v, 2, &gr, 1,
                         t.style == DESKTOP_REFERENCE ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
        }

        SetBkMode(tdc, TRANSPARENT);
        if (t.style == PINNED_APP && aw > 0 && ah > 0 && art.size() == (size_t)aw * (size_t)ah * 4) {
                HDC src = CreateCompatibleDC(tdc);
                void* artBits = NULL;
                HBITMAP bmp = src ? MakeDib(screenDC, aw, ah, &artBits) : NULL;
                if (bmp && artBits) {
                    memcpy(artBits, art.data(), art.size());
                    HGDIOBJ previous = SelectObject(src, bmp);
                    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    AlphaBlend(tdc, 0, 0, w, h, src, 0, 0, aw, ah, blend);
                    SelectObject(src, previous);
                    DrawModernTileCaption(tdc, t.label, w, h);
                    if (hover) DrawTileHover(tdc, w, h);
                    DeleteObject(bmp);
                    DeleteDC(src);
                    continue;
                }
                if (bmp) DeleteObject(bmp);
                if (src) DeleteDC(src);
        }
        if (t.style == TOP_APP && !t.veArt.empty() && t.veW > 0) {
            const bool smallVe = (t.cw == TS_SMALL && !t.veArtSmall.empty());
            const std::vector<BYTE>& img = smallVe ? t.veArtSmall : t.veArt;
            const int iw = smallVe ? t.veSmallW : t.veW, ih = smallVe ? t.veSmallH : t.veH;
            HDC src = CreateCompatibleDC(tdc);
            void* bits = NULL;
            HBITMAP bmp = src ? MakeDib(screenDC, iw, ih, &bits) : NULL;
            if (bmp && bits && img.size() == (size_t)iw * (size_t)ih * 4) {
                memcpy(bits, img.data(), img.size());
                HGDIOBJ prev = SelectObject(src, bmp);
                BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                SetStretchBltMode(tdc, HALFTONE); SetBrushOrgEx(tdc, 0, 0, NULL);
                if (t.cw == TS_WIDE) AlphaBlend(tdc, (w - h) / 2, 0, h, h, src, 0, 0, iw, ih, blend);
                else AlphaBlend(tdc, 0, 0, w, h, src, 0, 0, iw, ih, blend);
                SelectObject(src, prev);
                if (!t.veHideName) DrawModernTileCaption(tdc, t.label, w, h, t.veDarkText);
                if (hover) DrawTileHover(tdc, w, h);
                DeleteObject(bmp);
                DeleteDC(src);
                continue;
            }
            if (bmp) DeleteObject(bmp);
            if (src) DeleteDC(src);
        }
        if (t.cw == TS_SMALL && t.style != DESKTOP_REFERENCE) {
            const int bs = ScI(38);
            if (t.pinBmp) {
                HDC src = CreateCompatibleDC(tdc);
                if (src) {
                    HGDIOBJ so = SelectObject(src, t.pinBmp);
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    AlphaBlend(tdc, (w - bs) / 2, (h - bs) / 2, bs, bs, src, 0, 0, t.pinW, t.pinH, bf);
                    SelectObject(src, so);
                    DeleteDC(src);
                }
            } else if (t.icon) {
                Gdiplus::Bitmap* gpb = IconToGpBitmap(t.icon);
                if (gpb) {
                    {   Gdiplus::Graphics gp(tdc);
                        gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                        gp.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                        gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                        gp.DrawImage(gpb, Gdiplus::Rect((w - bs) / 2, (h - bs) / 2, bs, bs),
                                     0, 0, gpb->GetWidth(), gpb->GetHeight(), Gdiplus::UnitPixel);
                    }
                    delete gpb;
                } else DrawAppIconOrFallback(tdc, t.icon, (w - bs) / 2, (h - bs) / 2, bs);
            } else if (t.target && wcsstr(t.target, L"ms-windows-store")) {
                Gdiplus::Graphics gp(tdc);
                GlyphStoreBagV(gp, (w - bs) * 0.5f, (h - bs) * 0.5f, (float)bs);
            } else DrawAppIconOrFallback(tdc, NULL, (w - bs) / 2, (h - bs) / 2, bs);
            if (hover) DrawTileHover(tdc, w, h);
            continue;
        }
        if (t.kind == 0 && t.style != DESKTOP_REFERENCE) {
            const int gs = t.cw == TS_LARGE ? ScI(96) : ScI(56);
            if (t.icon) {
                Gdiplus::Bitmap* gpb = IconToGpBitmap(t.icon);
                if (gpb) {
                    {   Gdiplus::Graphics gp(tdc);
                        gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                        gp.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                        gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                        gp.DrawImage(gpb, Gdiplus::Rect((w - gs) / 2, (h - gs) / 2 - ScI(10), gs, gs),
                                     0, 0, gpb->GetWidth(), gpb->GetHeight(), Gdiplus::UnitPixel);
                    }
                    delete gpb;
                } else DrawAppIconOrFallback(tdc, t.icon, (w - gs) / 2, (h - gs) / 2 - ScI(10), gs);
            }
            else if (t.target && wcsstr(t.target, L"ms-windows-store")) {
                Gdiplus::Graphics gp(tdc);
                GlyphStoreBagV(gp, (w - gs) * 0.5f, (h - gs) * 0.5f - ScF(10.0f), (float)gs);
            }
            else DrawAppIconOrFallback(tdc, NULL, (w - gs) / 2, (h - gs) / 2 - ScI(10), gs);
            DrawModernTileCaption(tdc, t.label, w, h);
            if (hover) DrawTileHover(tdc, w, h);
            continue;
        }
        if (t.style == DESKTOP_REFERENCE) {
            DrawDesktopWallpaperTile(tdc, screenDC, w, h);
        } else if (t.style == TOP_APP) {
            const int bs = t.cw == TS_LARGE ? ScI(96) : ScI(64);
            if (t.pinBmp) {
                HDC src = CreateCompatibleDC(tdc);
                if (src) {
                    HGDIOBJ old = SelectObject(src, t.pinBmp);
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    AlphaBlend(tdc, (w - bs) / 2, (h - bs) / 2 - ScI(10), bs, bs, src, 0, 0, t.pinW, t.pinH, bf);
                    SelectObject(src, old);
                    DeleteDC(src);
                }
            } else if (t.icon) {
                Gdiplus::Bitmap* gpb = IconToGpBitmap(t.icon);
                if (gpb) {
                    {   Gdiplus::Graphics gp(tdc);
                        gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                        gp.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                        gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                        gp.DrawImage(gpb, Gdiplus::Rect((w - bs) / 2, (h - bs) / 2 - ScI(10), bs, bs),
                                     0, 0, gpb->GetWidth(), gpb->GetHeight(), Gdiplus::UnitPixel);
                    }
                    delete gpb;
                } else DrawAppIconOrFallback(tdc, t.icon, (w - bs) / 2, (h - bs) / 2 - ScI(10), bs);
            } else DrawAppIconOrFallback(tdc, NULL, (w - bs) / 2, (h - bs) / 2 - ScI(10), bs);
            if (!t.veHideName) DrawModernTileCaption(tdc, t.label, w, h, t.veDarkText);
        } else {
            if (t.style == PINNED_APP && t.pinBmp) {
                HDC src = CreateCompatibleDC(tdc);
                if (src) {
                    HGDIOBJ so = SelectObject(src, t.pinBmp);
                    BLENDFUNCTION pf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                    const int bs = t.cw == TS_LARGE ? ScI(96) : ScI(64);
                    AlphaBlend(tdc, (w - bs) / 2, (h - bs) / 2 - ScI(10), bs, bs, src, 0, 0, t.pinW, t.pinH, pf);
                    SelectObject(src, so);
                    DeleteDC(src);
                }
            }
            if (t.style == PINNED_APP) {
                DrawModernTileCaption(tdc, t.label, w, h);
            } else {
                SetTextColor(tdc, t.style == STORE_CAPTION ? RGB(195, 65, 21) : RGB(255, 255, 255));
                SelectObject(tdc, g_fLabel);
                RECT lr = { ScI(12), h - ScI(30), w - ScI(8), h - ScI(8) };
                DrawTextW(tdc, t.label, -1, &lr,
                          DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
            }
        }

        if (hover) DrawTileHover(tdc, w, h);

    }
}

static void BuildTileBuffers(HDC screenDC) {
    for (int i = 0; i < g_tileCount; ++i) if (g_tiles[i].alive) BuildOneTile(i, screenDC);
}

static void RefreshDesktopTile() {
    HDC screen = GetDC(NULL);
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive || g_tiles[i].style != DESKTOP_REFERENCE) continue;
        RECT r; TileRect(g_tiles[i], &r);
        int w = r.right - r.left, h = r.bottom - r.top;
        for (int v = 0; v < 2; ++v) {
            if (!g_tileDC[i][v] || !g_tileBits[i][v]) continue;
            DrawDesktopWallpaperTile(g_tileDC[i][v], screen, w, h);
            if (v) DrawTileHover(g_tileDC[i][v], w, h);
            GdiFlush();
            if (g_rt && g_tileGpu[i][v]) {
                HRESULT hr = g_tileGpu[i][v]->CopyFromMemory(NULL, g_tileBits[i][v], (UINT32)w * 4);
                if (FAILED(hr)) ReleaseGpuTarget();
            }
        }
    }
    if (screen) ReleaseDC(NULL, screen);
}

static bool TileOpeningProgress(int i, float to, float* opacity, float* slide, float* scale) {
    if (!g_tiles[i].alive) return false;
    float delay = g_logonOpening ? LOGON_TILE_DELAY : TILES_DELAY;
    float stagger = g_logonOpening ? LOGON_BLOCK_STAGGER : BLOCK_STAGGER;
    float tt = to - delay - (float)g_tileBlock[i] * stagger;
    if (tt <= 0.0f) return false;
    float fade = g_logonOpening ? LOGON_TILE_FADE : TILE_FADE;
    float duration = g_logonOpening ? LOGON_TILE_T : TILE_T;
    float scaleDuration = g_logonOpening ? LOGON_SCALE_T : TILE_SCALE_T;
    float initialScale = g_logonOpening ? LOGON_TILE_S0 : TILE_S0;
    float distance = g_logonOpening ? LOGON_TILE_SLIDE : TileSlideDistance();
    *opacity = Clamp01(tt / fade);
    *slide = distance * ((float)g_sw / 1920.0f) * (1.0f - EaseSettle(tt / duration));
    *scale = initialScale + (1.0f - initialScale) * EaseSettle(tt / scaleDuration);
    return true;
}

static float HeaderOpacity(float to) {
    return g_logonOpening ? SmoothStep((to - LOGON_TITLE_DELAY) / LOGON_TITLE_FADE) :
                            Clamp01((to - HEADER_DELAY) / HEADER_FADE);
}
static float UserOpacity(float to) {
    return g_logonOpening ? SmoothStep((to - LOGON_ACCOUNT_DELAY) / LOGON_ACCOUNT_FADE) :
                            HeaderOpacity(to);
}
static float LogonHeaderShift(bool account, float to) {
    if (!g_logonOpening) return 0.0f;
    float delay = account ? LOGON_ACCOUNT_DELAY : LOGON_TITLE_DELAY;
    float duration = account ? LOGON_ACCOUNT_MOVE : LOGON_TITLE_MOVE;
    float distance = account ? LOGON_ACCOUNT_SLIDE : LOGON_TITLE_SLIDE;
    float remaining = 1.0f - Clamp01((to - delay) / duration);
    float shift = distance * remaining * remaining * remaining;
    return (account ? -shift : shift) * ((float)g_sw / 1920.0f);
}

static void DrawTile(int i, float a, float slide, float gs) {
    if (a <= 0.0f) return;
    const Tile& t = g_tiles[i];
    RECT r; TileRect(t, &r);
    int w = r.right - r.left, h = r.bottom - r.top;
    HDC tdc = g_tileDC[i][i == g_hover ? 1 : 0];
    if (!tdc) return;

    float sc = g_tileScale[i] * gs;
    if (i == g_dragIdx) sc *= 1.05f;
    int dw = (int)(w * sc), dh = (int)(h * sc);
    float cx = (float)r.left + (float)w * 0.5f - g_scrollCur;
    float cy = (float)r.top + (float)h * 0.5f;
    float px = g_blockPivotX[g_tileBlock[i]] - g_scrollCur, py = TILE_PIVOT_Y;
    float ncx = px + (cx - px) * gs + slide;
    float ncy = py + (cy - py) * gs;
    int dx = (int)(ncx - (float)dw * 0.5f), dy = (int)(ncy - (float)dh * 0.5f);
    {
        DWORD znow = NowMs();
        float zk = ZoomPresence(znow);
        if (zk > 0.0f) {
            float zs, zox, zoy; ZoomXform(znow, &zs, &zox, &zoy);
            dx = (int)(zox + zs * (float)dx); dy = (int)(zoy + zs * (float)dy);
            dw = std::max(1, (int)(zs * (float)dw)); dh = std::max(1, (int)(zs * (float)dh));
        }
    }
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(255.0f * Clamp01(a)), 0 };
    AlphaBlend(g_bbDC, dx, dy, dw, dh, tdc, 0, 0, w, h, bf);
}

static void DrawHeader(float a, HDC sdc, float userA = -1.0f) {
    if (userA < 0.0f) userA = a;
    float to = (float)OpeningElapsedMs(NowMs()) / 1000.0f;
    int titleShift = (int)roundf(LogonHeaderShift(false, to));
    int accountShift = (int)roundf(LogonHeaderShift(true, to));
    if (a > 0.0f) {
        SetTextColor(g_bbDC, Mix(RGB(34, 14, 52), RGB(255, 255, 255), a));
        SelectObject(g_bbDC, g_fTitle);
        RECT tr = { LEFT + titleShift, ScI(40), LEFT + ScI(400) + titleShift, ScI(140) };
        DrawTextW(g_bbDC, T(STR_START), -1, &tr, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);
    }
    if (userA <= 0.0f || g_view != VIEW_START) return;
    SetTextColor(g_bbDC, Mix(RGB(34, 14, 52), RGB(255, 255, 255), userA));
    SelectObject(g_bbDC, g_fUser);
    RECT ur = { g_sw - ScI(700) + accountShift, ScI(30),
                HeaderAvatarRight() - ScI(54) + accountShift, ScI(78) };
    DrawTextW(g_bbDC, DisplayName(), -1, &ur, DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

    const int S = ScI(44);
    const int avL = HeaderAvatarRight() - S + accountShift, avT = ScI(34);
    if (EnsureAvatarPicture() && g_avatarPicBmp) {
        HDC pdc = CreateCompatibleDC(sdc);
        HGDIOBJ op = SelectObject(pdc, g_avatarPicBmp);
        BLENDFUNCTION bfp = { AC_SRC_OVER, 0, (BYTE)(255.0f * Clamp01(userA)), 0 };
        AlphaBlend(g_bbDC, avL, avT, S, S, pdc, 0, 0, S, S, bfp);
        SelectObject(pdc, op);
        DeleteDC(pdc);
    } else {
    HDC adc = CreateCompatibleDC(sdc);
    HBITMAP ab = CreateCompatibleBitmap(sdc, S, S);
    HGDIOBJ oa = SelectObject(adc, ab);
    HBRUSH bg = CreateSolidBrush(RGB(70, 70, 70));
    RECT ar = { 0, 0, S, S };
    FillRect(adc, &ar, bg);
    DeleteObject(bg);
    HBRUSH pb = CreateSolidBrush(RGB(255, 255, 255));
    HGDIOBJ obr = SelectObject(adc, pb);
    HGDIOBJ opn = SelectObject(adc, GetStockObject(NULL_PEN));
    Ellipse(adc, S * 14 / 44, S * 6 / 44, S * 30 / 44, S * 22 / 44);
    Ellipse(adc, S * 8 / 44, S * 24 / 44, S * 36 / 44, S * 56 / 44);
    SelectObject(adc, opn); SelectObject(adc, obr);
    DeleteObject(pb);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(255.0f * Clamp01(userA)), 0 };
    AlphaBlend(g_bbDC, avL, avT, S, S, adc, 0, 0, S, S, bf);
    SelectObject(adc, oa);
    DeleteObject(ab);
    DeleteDC(adc);
    }
    RECT pw, sc; HeaderButtonRects(&pw, &sc);
    if (EnsureUiIcons()) {
        const BYTE ia = (BYTE)(255.0f * Clamp01(userA));
        RECT pd, sd; IconDestRect(pw, &pd); IconDestRect(sc, &sd);
        if (g_hoverPwr) { RECT q; IconDestRect(pw, &q); InflateRect(&q, ScI(6), ScI(6)); BlitIconGdi(g_bbDC, g_iconHover, q, false, 255); }
        if (g_hoverSrch) { RECT q; IconDestRect(sc, &q); InflateRect(&q, ScI(6), ScI(6)); BlitIconGdi(g_bbDC, g_iconHover, q, false, 255); }
        BlitIconGdi(g_bbDC, g_iconPower, pd, false, ia);
        BlitIconGdi(g_bbDC, g_iconSearch, sd, false, ia);
    }
}

static void ReleaseGpuTarget() {
    for (int i = 0; i < g_tileCount; ++i) for (int v = 0; v < 2; ++v) {
        if (g_tileGpu[i][v]) { g_tileGpu[i][v]->Release(); g_tileGpu[i][v] = NULL; }
    }
    if (g_desktopAppGpu) { g_desktopAppGpu->Release(); g_desktopAppGpu = NULL; }
    if (g_bgGpu) { g_bgGpu->Release(); g_bgGpu = NULL; }
    if (g_snapGpu) { g_snapGpu->Release(); g_snapGpu = NULL; }
    if (g_whiteGpu) { g_whiteGpu->Release(); g_whiteGpu = NULL; }
    if (g_avatarBg) { g_avatarBg->Release(); g_avatarBg = NULL; }
    if (g_avatarFg) { g_avatarFg->Release(); g_avatarFg = NULL; }
    if (g_avatarPicGpu) { g_avatarPicGpu->Release(); g_avatarPicGpu = NULL; }
    if (g_bgPlainGpu) { g_bgPlainGpu->Release(); g_bgPlainGpu = NULL; }
    if (g_scratch) { g_scratch->Release(); g_scratch = NULL; }
    for (size_t i = 0; i < g_icons.size(); ++i)
        if (g_icons[i].gpu) { g_icons[i].gpu->Release(); g_icons[i].gpu = NULL; }
    if (g_rt) { g_rt->Release(); g_rt = NULL; }
}

static D2D1_BITMAP_PROPERTIES GpuBitmapProperties() {
    D2D1_BITMAP_PROPERTIES p = {};
    p.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    p.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    p.dpiX = p.dpiY = 96.0f;
    return p;
}

static bool UploadSnapshotGpu() {
    if (!g_rt || !g_snapBits) return false;
    if (g_snapGpu) { g_snapGpu->Release(); g_snapGpu = NULL; }
    D2D1_BITMAP_PROPERTIES p = GpuBitmapProperties();
    HRESULT hr = g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh),
                                    g_snapBits, (UINT32)g_sw * 4, &p, &g_snapGpu);
    return SUCCEEDED(hr) && g_snapGpu;
}

static IDWriteTextFormat* NewFmt(const wchar_t* face, float size, DWRITE_FONT_WEIGHT weight,
                                DWRITE_PARAGRAPH_ALIGNMENT para, DWRITE_TEXT_ALIGNMENT align, bool wrap) {
    IDWriteTextFormat* f = NULL;
    if (FAILED(g_writeFactory->CreateTextFormat(face, NULL, weight, DWRITE_FONT_STYLE_NORMAL,
                                                DWRITE_FONT_STRETCH_NORMAL, ScF(size), L"en-US", &f)) || !f) return NULL;
    f->SetParagraphAlignment(para);
    f->SetTextAlignment(align);
    f->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
    return f;
}

static HRESULT MakeSearchFormats() {
    const DWRITE_PARAGRAPH_ALIGNMENT N = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, C = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    const DWRITE_TEXT_ALIGNMENT L = DWRITE_TEXT_ALIGNMENT_LEADING, T = DWRITE_TEXT_ALIGNMENT_TRAILING,
                                M = DWRITE_TEXT_ALIGNMENT_CENTER;
    g_subFormat     = NewFmt(L"Segoe UI", 22.0f, DWRITE_FONT_WEIGHT_LIGHT, N, L, false);
    g_itemFormat    = NewFmt(L"Segoe UI", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, N, L, true);
    g_flyFormat     = NewFmt(L"Segoe UI", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, N, L, false);
    if (g_flyFormat) {
        DWRITE_TRIMMING trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
        IDWriteInlineObject* ell = NULL;
        if (SUCCEEDED(g_writeFactory->CreateEllipsisTrimmingSign(g_flyFormat, &ell)) && ell) {
            g_flyFormat->SetTrimming(&trim, ell);
            ell->Release();
        }
    }
    g_sbTitleFormat = NewFmt(L"Segoe UI", 28.0f, DWRITE_FONT_WEIGHT_LIGHT, N, L, false);
    g_boldFormat    = NewFmt(L"Segoe UI", 13.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, N, L, false);
    g_rowFormat     = NewFmt(L"Segoe UI", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, C, L, false);
    g_countFormat   = NewFmt(L"Segoe UI", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, C, T, false);
    g_queryFormat   = NewFmt(L"Segoe UI", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, C, L, false);
    g_glyphFormat   = NewFmt(L"Segoe UI Symbol", 24.0f, DWRITE_FONT_WEIGHT_NORMAL, C, M, false);
    g_glyphSmall    = NewFmt(L"Segoe UI Symbol", 15.0f, DWRITE_FONT_WEIGHT_NORMAL, C, M, false);
    g_barLabel      = NewFmt(L"Segoe UI", 13.0f, DWRITE_FONT_WEIGHT_NORMAL, C, M, false);
    g_hdrGlyphFmt   = NewFmt(L"Segoe UI Symbol", 20.0f, DWRITE_FONT_WEIGHT_NORMAL, C, M, false);
    g_zoomFormat    = NewFmt(L"Segoe UI", 20.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_PARAGRAPH_ALIGNMENT_FAR, L, true);
    return (g_subFormat && g_itemFormat && g_sbTitleFormat && g_boldFormat && g_rowFormat && g_countFormat &&
            g_queryFormat && g_glyphFormat && g_glyphSmall && g_barLabel) ? S_OK : E_FAIL;
}

static void CreateUiFonts() {
    g_fTitle = MakeFont(ScI(56), L"Segoe UI Light");
    g_fUser  = MakeFont(ScI(28), L"Segoe UI Light");
    g_fLabel = MakeFont(ScI(15), L"Segoe UI");
    g_fTopLabel = MakeFont(ScI(24), L"Segoe UI Light");
    g_fHdrGlyph = MakeFont(ScI(20), L"Segoe UI Symbol");
    g_fBarMeas = MakeFont(ScI(13), L"Segoe UI");
}

static void RecreateFonts() {
    if (g_fTitle) { DeleteObject(g_fTitle); g_fTitle = NULL; }
    if (g_fUser) { DeleteObject(g_fUser); g_fUser = NULL; }
    if (g_fLabel) { DeleteObject(g_fLabel); g_fLabel = NULL; }
    if (g_fTopLabel) { DeleteObject(g_fTopLabel); g_fTopLabel = NULL; }
    if (g_fHdrGlyph) { DeleteObject(g_fHdrGlyph); g_fHdrGlyph = NULL; }
    if (g_fBarMeas) { DeleteObject(g_fBarMeas); g_fBarMeas = NULL; }
    CreateUiFonts();
    g_userTextW = -1;
    if (g_writeFactory) {
        // Release through pointers so the globals are nulled, never left dangling.
        IDWriteTextFormat** all[] = { &g_titleFormat, &g_userFormat, &g_subFormat, &g_itemFormat,
                                      &g_sbTitleFormat, &g_boldFormat, &g_rowFormat, &g_countFormat,
                                      &g_queryFormat, &g_glyphFormat, &g_glyphSmall, &g_barLabel,
                                      &g_flyFormat, &g_hdrGlyphFmt, &g_zoomFormat };
        for (IDWriteTextFormat** f : all) if (*f) { (*f)->Release(); *f = NULL; }
        if (g_rt) {
            g_writeFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_LIGHT,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, ScF(56.0f), L"en-US", &g_titleFormat);
            g_writeFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_LIGHT,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, ScF(28.0f), L"en-US", &g_userFormat);
            if (g_userFormat) g_userFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            MakeSearchFormats();
        }
    }
}

static void UploadTileTexturesGpu() {
    if (!g_rt) return;
    D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive) continue;
        RECT r; TileRect(g_tiles[i], &r);
        const UINT32 w = (UINT32)std::max(1, (int)(r.right - r.left));
        const UINT32 h = (UINT32)std::max(1, (int)(r.bottom - r.top));
        for (int v = 0; v < 2; ++v)
            if (!g_tileGpu[i][v] && g_tileBits[i][v])
                g_rt->CreateBitmap(D2D1::SizeU(w, h), g_tileBits[i][v], w * 4, &bp, &g_tileGpu[i][v]);
    }
}
static bool InitGpu() {
    if (g_rt) {
        if (!g_bgGpu && g_bgBits) {
            D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
            g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh),
                               g_bgBits, (UINT32)g_sw * 4, &bp, &g_bgGpu);
        }
        if (!g_bgPlainGpu && g_bgPlainBits) {
            D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
            g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh),
                               g_bgPlainBits, (UINT32)g_sw * 4, &bp, &g_bgPlainGpu);
        }
        if (UploadSnapshotGpu()) { UploadTileTexturesGpu(); return true; }
        ReleaseGpuTarget();
        return false;
    }
    BOOL dwmEnabled = FALSE;
    if (FAILED(DwmIsCompositionEnabled(&dwmEnabled)) || !dwmEnabled) return false;

    if (!g_d2dFactory && FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                                  &g_d2dFactory))) return false;
    if (!g_writeFactory && FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                       __uuidof(IDWriteFactory), (IUnknown**)&g_writeFactory))) return false;
    if (!g_bgBits || !g_snapBits || !g_wnd) return false;

    D2D1_RENDER_TARGET_PROPERTIES p = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_HARDWARE,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN),
        96.0f, 96.0f);
    D2D1_HWND_RENDER_TARGET_PROPERTIES hp = D2D1::HwndRenderTargetProperties(
        g_wnd, D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh), D2D1_PRESENT_OPTIONS_NONE);
    HRESULT hr = g_d2dFactory->CreateHwndRenderTarget(p, hp, &g_rt);
    if (FAILED(hr)) { ReleaseGpuTarget(); return false; }
    g_rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

    D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
    hr = g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh),
                            g_bgBits, (UINT32)g_sw * 4, &bp, &g_bgGpu);
    if (FAILED(hr) || !UploadSnapshotGpu()) { ReleaseGpuTarget(); return false; }
    hr = g_rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &g_whiteGpu);
    if (SUCCEEDED(hr)) hr = g_rt->CreateSolidColorBrush(
        D2D1::ColorF(70/255.0f, 70/255.0f, 70/255.0f), &g_avatarBg);
    if (SUCCEEDED(hr)) hr = g_rt->CreateSolidColorBrush(
        D2D1::ColorF(1.0f, 1.0f, 1.0f), &g_avatarFg);
    if (SUCCEEDED(hr) && !g_titleFormat) hr = g_writeFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, ScF(56.0f), L"en-US", &g_titleFormat);
    if (SUCCEEDED(hr) && !g_userFormat) hr = g_writeFactory->CreateTextFormat(
        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, ScF(28.0f), L"en-US", &g_userFormat);
    if (SUCCEEDED(hr) && g_bgPlainBits) {
        if (FAILED(g_rt->CreateBitmap(D2D1::SizeU((UINT32)g_sw, (UINT32)g_sh), g_bgPlainBits,
                                      (UINT32)g_sw * 4, &bp, &g_bgPlainGpu))) g_bgPlainGpu = NULL;
    }
    if (SUCCEEDED(hr)) hr = g_rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &g_scratch);
    if (SUCCEEDED(hr) && !g_subFormat) hr = MakeSearchFormats();
    if (FAILED(hr)) { ReleaseGpuTarget(); return false; }
    g_userFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    UploadTileTexturesGpu();
    Wh_Log(L"Start screen: Direct2D hardware rendering / DWM composition enabled");
    return true;
}

static void WarmLogonTileTextures() {
    if (!g_rt) return;
    D2D1_BITMAP_PROPERTIES props = GpuBitmapProperties();
    for (int i = 0; i < g_tileCount; ++i) {
        if (!g_tiles[i].alive || !g_tileBits[i][0] || g_tileGpu[i][0]) continue;
        int w = TilePxW(g_tiles[i].cw), h = TilePxH(g_tiles[i].cw);
        g_rt->CreateBitmap(D2D1::SizeU((UINT32)w, (UINT32)h), g_tileBits[i][0],
                           (UINT32)w * 4, &props, &g_tileGpu[i][0]);
    }
}

// Rendering
static void DrawHeaderGpu(float a, float userA = -1.0f) {
    if (userA < 0.0f) userA = a;
    float to = (float)OpeningElapsedMs(NowMs()) / 1000.0f;
    float titleShift = LogonHeaderShift(false, to);
    float accountShift = LogonHeaderShift(true, to);
    if (a > 0.0f) {
        g_whiteGpu->SetOpacity(Clamp01(a));
        g_rt->DrawText(T(STR_START), (UINT32)wcslen(T(STR_START)), g_titleFormat,
                       D2D1::RectF((float)LEFT + titleShift, ScF(40.0f),
                                   (float)LEFT + ScI(400) + titleShift, ScF(140.0f)), g_whiteGpu);
    }
    if (userA <= 0.0f || g_view != VIEW_START) return;
    if (g_hoverAccount) {
        RECT ah; AccountRect(&ah);
        FillRc((float)ah.left + accountShift, (float)ah.top,
               (float)ah.right + accountShift, (float)ah.bottom,
               RGB(255, 255, 255), 0.10f * Clamp01(userA));
    }
    g_whiteGpu->SetOpacity(Clamp01(userA));
    g_rt->DrawText(DisplayName(), (UINT32)wcslen(DisplayName()), g_userFormat,
                   D2D1::RectF((float)g_sw - ScI(700) + accountShift, ScF(30.0f),
                               (float)HeaderAvatarRight() - ScI(54) + accountShift, ScF(78.0f)), g_whiteGpu);
    float x = (float)HeaderAvatarRight() - ScI(44) + accountShift, y = ScF(34.0f);
    const float AV = ScF(44.0f);
    if (!g_avatarPicGpu) EnsureAvatarPicture();
    if (g_avatarPicGpu) {
        g_rt->DrawBitmap(g_avatarPicGpu, D2D1::RectF(x, y, x + AV, y + AV), Clamp01(userA),
                         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    } else {
    g_avatarBg->SetOpacity(Clamp01(userA));
    g_avatarFg->SetOpacity(Clamp01(userA));
    g_rt->FillRectangle(D2D1::RectF(x, y, x + AV, y + AV), g_avatarBg);
    g_rt->PushAxisAlignedClip(D2D1::RectF(x, y, x + AV, y + AV), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    g_rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + AV * 0.5f, y + ScF(14.0f)), ScF(8.0f), ScF(8.0f)), g_avatarFg);
    g_rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + AV * 0.5f, y + ScF(40.0f)), ScF(14.0f), ScF(16.0f)), g_avatarFg);
    g_rt->PopAxisAlignedClip();
    }
    if (EnsureUiIcons()) {
        RECT pw, sc; HeaderButtonRects(&pw, &sc);
        auto put = [&](const UiIcon& ic, const RECT& r, bool hov) {
            if (hov && g_iconHover.gpu) {
                RECT q; IconDestRect(r, &q); InflateRect(&q, ScI(6), ScI(6));
                g_rt->DrawBitmap(g_iconHover.gpu, D2D1::RectF((float)q.left + accountShift, (float)q.top,
                                                              (float)q.right + accountShift, (float)q.bottom),
                                 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
            if (ic.gpu) {
                RECT d; IconDestRect(r, &d);
                g_whiteGpu->SetOpacity(Clamp01(userA));
                g_rt->DrawBitmap(ic.gpu, D2D1::RectF((float)d.left + accountShift, (float)d.top,
                                                     (float)d.right + accountShift, (float)d.bottom),
                                 Clamp01(userA), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        };
        put(g_iconPower, pw, g_hoverPwr);
        put(g_iconSearch, sc, g_hoverSrch);
    }
}

static void DrawTileGpu(int i, float a, float slide, float gs) {
    if (!g_tiles[i].alive) return;
    const Tile& t = g_tiles[i];
    RECT r; TileRect(t, &r);
    float w = (float)(r.right - r.left), h = (float)(r.bottom - r.top);
    float sc = g_tileScale[i] * gs;
    if (i == g_dragIdx) sc *= 1.05f;
    float dw = w * sc, dh = h * sc;
    float cx = t.px + w * 0.5f - g_scrollCur;
    float cy = t.py + h * 0.5f;
    float px = g_blockPivotX[g_tileBlock[i]] - g_scrollCur, py = TILE_PIVOT_Y;
    float ncx = px + (cx - px) * gs + slide;
    float ncy = py + (cy - py) * gs;
    int variant = (i == g_hover) ? 1 : 0;
    ID2D1Bitmap*& bitmap = g_tileGpu[i][variant];
    if (!bitmap && g_tileBits[i][variant]) {
        D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
        g_rt->CreateBitmap(D2D1::SizeU((UINT32)w, (UINT32)h), g_tileBits[i][variant], (UINT32)w * 4, &bp, &bitmap);
    }
    D2D1_RECT_F dest = D2D1::RectF(ncx - dw * 0.5f, ncy - dh * 0.5f, ncx + dw * 0.5f, ncy + dh * 0.5f);
    const bool pixelExact = dw == w && dh == h &&
                            dest.left == (float)(int)dest.left && dest.top == (float)(int)dest.top;
    if (bitmap) g_rt->DrawBitmap(bitmap, dest, (i == g_dragIdx) ? 0.92f * Clamp01(a) : Clamp01(a),
                                 pixelExact ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                                            : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    if (g_scratch && g_barVisible && g_barMode == 1 && g_barIdx == i) {
        SetBr(RGB(255, 140, 32), 1.0f);
        g_rt->DrawRectangle(D2D1::RectF(dest.left + 1.5f, dest.top + 1.5f, dest.right - 1.5f, dest.bottom - 1.5f),
                            g_scratch, 3.0f);
        ID2D1PathGeometry* corner = NULL;
        ID2D1GeometrySink* sink = NULL;
        if (g_d2dFactory && SUCCEEDED(g_d2dFactory->CreatePathGeometry(&corner)) && corner &&
            SUCCEEDED(corner->Open(&sink)) && sink) {
            sink->BeginFigure(D2D1::Point2F(dest.right - 31, dest.top + 2), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(D2D1::Point2F(dest.right - 2, dest.top + 2));
            sink->AddLine(D2D1::Point2F(dest.right - 2, dest.top + 31));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close(); sink->Release();
            g_rt->FillGeometry(corner, g_scratch);
        } else if (sink) sink->Release();
        if (corner) corner->Release();
        TextAt(L"\xE10B", g_glyphSmall, dest.right - 24, dest.top + 1, dest.right - 1, dest.top + 24,
               RGB(255, 255, 255));
    }
}

static void DrawSnapshotGpu(float s, float rightEdge) {
    if (!g_snapGpu || rightEdge <= 0.0f) return;
    float dw = s * (float)g_sw, dh = s * (float)g_sh;
    float y = ((float)g_sh - dh) * 0.5f;
    g_rt->DrawBitmap(g_snapGpu,
        D2D1::RectF(rightEdge - dw, y, rightEdge, y + dh), 1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

static bool FocusTileRect(float to, DWORD now, D2D1_RECT_F* out, float* opacity) {
    const int i = g_focus;
    if (i < 0 || i >= g_tileCount || !g_tiles[i].alive) return false;
    float a, slide, gs;
    if (!TileOpeningProgress(i, to, &a, &slide, &gs)) return false;
    if (g_leaveTick) { float e = LeaveEase(i, now); a *= 1.0f - e; gs *= 1.0f - 0.12f * e; }
    if (a <= 0.0f) return false;
    const Tile& t = g_tiles[i];
    float w = (float)TilePxW(t.cw), h = (float)TilePxH(t.cw);
    float sc = g_tileScale[i] * gs;
    if (i == g_dragIdx) sc *= 1.05f;
    float dw = w * sc, dh = h * sc;
    float cx = t.px + w * 0.5f - g_scrollCur;
    float cy = t.py + h * 0.5f;
    float px = g_blockPivotX[g_tileBlock[i]] - g_scrollCur, py = TILE_PIVOT_Y;
    float ncx = px + (cx - px) * gs + slide;
    float ncy = py + (cy - py) * gs;
    *out = D2D1::RectF(ncx - dw * 0.5f - 3.0f, ncy - dh * 0.5f - 3.0f,
                       ncx + dw * 0.5f + 3.0f, ncy + dh * 0.5f + 3.0f);
    *opacity = a;
    return true;
}

static bool RenderGpu() {
    if (!g_rt || !g_bgGpu) return false;
    DWORD now = NowMs();
    g_rt->BeginDraw();
    const bool searchView = (g_view == VIEW_SEARCH) && g_bgPlainGpu;
    g_rt->DrawBitmap(searchView ? g_bgPlainGpu : g_bgGpu, D2D1::RectF(0, 0, (float)g_sw, (float)g_sh));
    if (!g_closing && searchView) {
        DrawSearchGpu(now, false);
        if (g_alphaGrid) DrawAlphaGridGpu(now);
    } else if (!g_closing) {
        float to = (float)OpeningElapsedMs(now) / 1000.0f;
        if (g_bgPlainGpu) {
            float back = g_logonOpening ?
                1.0f - Clamp01((to - LOGON_GLASS_DELAY) / LOGON_GLASS_FADE) :
                (g_returnTick ? 1.0f - Clamp01((float)(now - g_returnTick) / 1000.0f / BG_BACK) : 0.0f);
            if (back > 0.0f) g_rt->DrawBitmap(g_bgPlainGpu, D2D1::RectF(0, 0, (float)g_sw, (float)g_sh), back);
        }
        const float lv = g_leaveTick ? Clamp01((float)(now - g_leaveTick) / LEAVE_MS) : 0.0f;
        const float zk = ZoomPresence(now);
        if (zk > 0.0f) {
            float zs, zox, zoy; ZoomXform(now, &zs, &zox, &zoy);
            g_rt->SetTransform(D2D1::Matrix3x2F::Scale(zs, zs) * D2D1::Matrix3x2F::Translation(zox, zoy));
        }
        DrawHeaderGpu(HeaderOpacity(to) * (1.0f - lv) * (1.0f - zk), UserOpacity(to) * (1.0f - lv) * (1.0f - zk));
        for (int pass = 0; pass < 2; ++pass)
            for (int i = 0; i < g_tileCount; ++i) {
                if ((i == g_dragIdx) != (pass == 1)) continue;
                float a, slide, gs;
                if (TileOpeningProgress(i, to, &a, &slide, &gs)) {
                    if (g_leaveTick) { float e = LeaveEase(i, now); a *= 1.0f - e; gs *= 1.0f - 0.12f * e; }
                    DrawTileGpu(i, a, slide, gs);
                }
            }
        if (zk > 0.0f) {
            DrawZoomHighlightGpu(zk);
            g_rt->SetTransform(D2D1::Matrix3x2F::Identity());
        }
        if (zk <= 0.0f && !g_charmOpen) {
            D2D1_RECT_F fr; float fa;
            if (FocusTileRect(to, now, &fr, &fa)) {
                SetBr(RGB(255, 255, 255), Clamp01(fa));
                g_rt->DrawRectangle(fr, g_scratch, 3.0f);
            }
        }
        if (!g_skipSnapshot && to < OPEN_HOLD + g_openSlideSeconds) {
            float s = 1.0f - (1.0f - DESK_SCALE) * EaseOut(to / OPEN_SCALE_T);
            float p = OpenSlideProgress((to - OPEN_HOLD) / g_openSlideSeconds);
            DrawSnapshotGpu(s, (1.0f + s) * 0.5f * (float)g_sw * (1.0f - p));
        }
    } else {
        if (g_view == VIEW_SEARCH && g_bgPlainGpu) DrawSearchGpu(now, true); else DrawHeaderGpu(1.0f);
        float tc = (float)(now - g_closeTick) / 1000.0f;
        float q = CloseSlideProgress(tc / g_closeSlideSeconds);
        float s = DESK_SCALE + (1.0f - DESK_SCALE) * q;
        DrawSnapshotGpu(s, q * (1.0f + s) * 0.5f * (float)g_sw);
    }
    if (!g_closing && g_open && g_view == VIEW_START) DrawGroupNamesGpu(now);
    if (!g_closing && g_open) DrawScrollBarGpu();
    if (!g_closing) DrawBarGpu(now);
    if (!g_closing && g_open) DrawAppsChromeGpu();
    if (!g_closing && g_open) DrawCharmBarGpu(now);
    HRESULT hr = g_rt->EndDraw();
    if (FAILED(hr)) {
        Wh_Log(L"Direct2D EndDraw failed (%08X); falling back to GDI", (unsigned)hr);
        ReleaseGpuTarget();
        g_dirty = true;
        return false;
    }
    g_dirty = false;
    return true;
}

static void DrawSnapshot(float s, float rightEdge) {
    if (!g_snapDC) return;
    int dw = (int)(s * (float)g_sw + 0.5f), dh = (int)(s * (float)g_sh + 0.5f);
    int dx = (int)(rightEdge + 0.5f) - dw, dy = (g_sh - dh) / 2;
    if (dx >= g_sw || dx + dw <= 0) return;
    if (dw == g_sw && dh == g_sh) {
        BitBlt(g_bbDC, dx, dy, dw, dh, g_snapDC, 0, 0, SRCCOPY);
    } else {
        SetStretchBltMode(g_bbDC, HALFTONE);
        SetBrushOrgEx(g_bbDC, 0, 0, NULL);
        StretchBlt(g_bbDC, dx, dy, dw, dh, g_snapDC, 0, 0, g_sw, g_sh, SRCCOPY);
    }
}

static void Render() {
    if (!g_bbDC || !g_bgDC) return;
    BitBlt(g_bbDC, 0, 0, g_sw, g_sh, g_bgDC, 0, 0, SRCCOPY);
    SetBkMode(g_bbDC, TRANSPARENT);
    DWORD now = NowMs();
    HDC sdc = GetDC(NULL);

    if (!g_closing) {
        float to = (float)OpeningElapsedMs(now) / 1000.0f;
        const float lv = g_leaveTick ? Clamp01((float)(now - g_leaveTick) / LEAVE_MS) : 0.0f;
        const float zk = ZoomPresence(now);
        DrawHeader(HeaderOpacity(to) * (1.0f - lv) * (1.0f - zk), sdc, UserOpacity(to) * (1.0f - lv) * (1.0f - zk));

        for (int pass = 0; pass < 2; ++pass) for (int i = 0; i < g_tileCount; ++i) {
            if ((i == g_dragIdx) != (pass == 1)) continue;
            float a, slide, gs;
            if (TileOpeningProgress(i, to, &a, &slide, &gs)) {
                if (g_leaveTick) { float e = LeaveEase(i, now); a *= 1.0f - e; gs *= 1.0f - 0.12f * e; }
                DrawTile(i, a, slide, gs);
            }
        }

        if (zk <= 0.0f && !g_charmOpen) {
            D2D1_RECT_F ff; float fa;
            if (FocusTileRect(to, now, &ff, &fa)) {
                RECT fr = { (int)roundf(ff.left), (int)roundf(ff.top),
                            (int)roundf(ff.right), (int)roundf(ff.bottom) };
                HBRUSH wb = (HBRUSH)GetStockObject(WHITE_BRUSH);
                for (int bnd = 0; bnd < 3; ++bnd) { FrameRect(g_bbDC, &fr, wb); InflateRect(&fr, -1, -1); }
            }
        }
        if (!g_skipSnapshot && to < OPEN_HOLD + g_openSlideSeconds) {
            float s = 1.0f - (1.0f - DESK_SCALE) * EaseOut(to / OPEN_SCALE_T);
            float p = OpenSlideProgress((to - OPEN_HOLD) / g_openSlideSeconds);
            DrawSnapshot(s, (1.0f + s) * 0.5f * (float)g_sw * (1.0f - p));
        }
    } else {
        DrawHeader(1.0f, sdc);
        float tc = (float)(now - g_closeTick) / 1000.0f;
        float q = CloseSlideProgress(tc / g_closeSlideSeconds);
        float s = DESK_SCALE + (1.0f - DESK_SCALE) * q;
        DrawSnapshot(s, q * (1.0f + s) * 0.5f * (float)g_sw);
    }
    DrawAlphaGridGdi(now);
    DrawZoomOverlayGdi(now);
    DrawScrollBarTo(g_bbDC);
    DrawAppsChromeGdi(sdc);
    DrawCharmBarGdi(sdc, now);
    ReleaseDC(NULL, sdc);
    g_dirty = false;
}


static std::wstring LowerW(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) CharLowerBuffW(&r[0], (DWORD)r.size());
    return r;
}

static int ClassifyCharmCat(const std::wstring& lower, const std::wstring& parse) {
    std::wstring pl = LowerW(parse);
    if (pl.find(L"immersivecontrolpanel") != std::wstring::npos ||
        pl.find(L"ms-settings") != std::wstring::npos ||
        pl.find(L"26ee0668") != std::wstring::npos ||
        lower == LowerW(T(STR_ROW_SETTINGS)) || lower == L"settings" ||
        lower == LowerW(T(STR_TILE_CONTROLPANEL)) || lower == L"control panel")
        return CC_SETTING;
    return CC_APP;
}

static void AddApp(std::vector<AppEntry>& list, const wchar_t* name, const std::wstring& parse,
                   bool exe, bool desktop = false, int charmCat = -1,
                   bool searchOnly = false, bool direct = false) {
    AppEntry a;
    a.name = name; a.lower = LowerW(a.name); a.parse = parse;
    a.exe = exe; a.desktop = desktop;
    a.searchOnly = searchOnly; a.direct = direct;
    a.charmCat = charmCat >= 0 ? charmCat : ClassifyCharmCat(a.lower, parse);
    a.date = 0; a.cat = 2;
    if (exe) a.cat = 1;
    else if (parse.find(L"AppsFolder") != std::wstring::npos ||
             parse.find(L"appsfolder") != std::wstring::npos) a.cat = 0;
    if (!parse.empty()) {
        WIN32_FILE_ATTRIBUTE_DATA fad = {};
        if (GetFileAttributesExW(parse.c_str(), GetFileExInfoStandard, &fad))
            a.date = ((unsigned long long)fad.ftLastWriteTime.dwHighDateTime << 32) |
                     (unsigned long long)fad.ftLastWriteTime.dwLowDateTime;
    }
    list.push_back(std::move(a));
}

static void AddCharmSystemEntries(std::vector<AppEntry>& list) {
    struct Ent { const wchar_t* label; const wchar_t* target; int cat; };
    const Ent ents[] = {
        { T(STR_TILE_CONTROLPANEL), L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}", CC_SETTING },
        { T(STR_ROW_SETTINGS),      L"ms-settings:",                                    CC_SETTING },
        { T(STR_TILE_COMPUTER),     L"shell:MyComputerFolder",                          CC_APP     },
        { T(STR_SYS_CMD),           L"cmd.exe",                                         CC_APP     },
        { T(STR_SYS_TASKMGR),       L"taskmgr.exe",                                     CC_APP     },
        { T(STR_SYS_PROGRAMS),      L"ms-settings:appsfeatures",                        CC_SETTING },
    };
    for (const Ent& e : ents) {
        const std::wstring lw = LowerW(e.label);
        bool dup = false;
        for (const AppEntry& a : list) if (a.lower == lw) { dup = true; break; }
        if (!dup) AddApp(list, e.label, e.target, false, false, e.cat, true, true);
    }
}

static void ScanCharmDir(std::vector<AppEntry>& out, const std::wstring& dir, int depth, size_t cap) {
    if (depth > 8 || out.size() >= cap) return;
    if (g_appsStop && WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) return;
    WIN32_FIND_DATAW fd = {};
    HANDLE hf = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) return;
    do {
        if (out.size() >= cap) break;
        if (g_appsStop && WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) break;
        if (fd.cFileName[0] == L'.') continue;
        if (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM |
                                   FILE_ATTRIBUTE_REPARSE_POINT)) continue;
        const bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDir && !_wcsicmp(fd.cFileName, L"node_modules")) continue;
        std::wstring full = dir + L"\\" + fd.cFileName;
        AppEntry a;
        a.name = fd.cFileName; a.lower = LowerW(a.name); a.parse = full;
        a.exe = false; a.desktop = false;
        a.cat = 2; a.charmCat = CC_FILE; a.dir = isDir;
        a.searchOnly = true; a.direct = true;
        a.date = ((unsigned long long)fd.ftLastWriteTime.dwHighDateTime << 32) |
                 (unsigned long long)fd.ftLastWriteTime.dwLowDateTime;
        out.push_back(std::move(a));
        if (isDir) ScanCharmDir(out, full, depth + 1, cap);
    } while (FindNextFileW(hf, &fd));
    FindClose(hf);
}

static void ScanCharmFiles(std::vector<AppEntry>& list) {
    const KNOWNFOLDERID* folders[] = { &FOLDERID_Desktop, &FOLDERID_Documents, &FOLDERID_Downloads,
                                       &FOLDERID_Pictures, &FOLDERID_Music, &FOLDERID_Videos };
    for (auto id : folders) {
        PWSTR p = NULL;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, 0, NULL, &p)) && p)
            ScanCharmDir(list, p, 0, 80000);
        CoTaskMemFree(p);
        if (g_appsStop && WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) break;
    }
}

static bool BitmapToPixels(HBITMAP hbm, BYTE** outPx, int* outW, int* outH) {
    BITMAP bm = {};
    if (!GetObjectW(hbm, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) return false;
    int w = bm.bmWidth, h = bm.bmHeight;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    size_t count = (size_t)w * (size_t)h;
    BYTE* px = new (std::nothrow) BYTE[count * 4];
    if (!px) return false;
    HDC dc = GetDC(NULL);
    int got = GetDIBits(dc, hbm, 0, h, px, &bi, DIB_RGB_COLORS);
    ReleaseDC(NULL, dc);
    if (!got) { delete[] px; return false; }
    bool anyAlpha = false, straight = false;
    for (size_t i = 0; i < count; ++i) {
        const BYTE* p = px + i * 4;
        if (p[3]) anyAlpha = true;
        if (p[0] > p[3] || p[1] > p[3] || p[2] > p[3]) straight = true;
    }
    for (size_t i = 0; i < count; ++i) {
        BYTE* p = px + i * 4;
        if (!anyAlpha) p[3] = 255;
        else if (straight) { p[0] = (BYTE)(p[0] * p[3] / 255); p[1] = (BYTE)(p[1] * p[3] / 255); p[2] = (BYTE)(p[2] * p[3] / 255); }
    }
    *outPx = px; *outW = w; *outH = h;
    return true;
}

static bool IconPixelsFromHICON(HICON ico, BYTE** outPx, int* outW, int* outH) {
    *outPx = NULL; *outW = *outH = 0;
    ICONINFO ii = {};
    if (!ico || !GetIconInfo(ico, &ii)) return false;
    bool ok = false;
    BITMAP bm = {};
    if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm) &&
        bm.bmWidth > 0 && bm.bmHeight > 0 && bm.bmWidth <= 512 && bm.bmHeight <= 512) {
        const int w = bm.bmWidth, h = bm.bmHeight;
        BYTE* px = new (std::nothrow) BYTE[(size_t)w * (size_t)h * 4];
        HDC dc = GetDC(NULL);
        if (px && dc) {
            BITMAPINFO bi = {};
            bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
            if (GetDIBits(dc, ii.hbmColor, 0, (UINT)h, px, &bi, DIB_RGB_COLORS)) {
                bool anyAlpha = false;
                for (size_t n = 3; n < (size_t)w * (size_t)h * 4; n += 4)
                    if (px[n]) { anyAlpha = true; break; }
                if (!anyAlpha && ii.hbmMask) {
                    const int stride = ((w + 15) / 16) * 2;
                    std::vector<BYTE> mk((size_t)stride * (size_t)h, 0);
                    BITMAPINFO mi = {};
                    mi.bmiHeader.biSize = sizeof(mi.bmiHeader); mi.bmiHeader.biWidth = w;
                    mi.bmiHeader.biHeight = -h; mi.bmiHeader.biPlanes = 1;
                    mi.bmiHeader.biBitCount = 1; mi.bmiHeader.biCompression = BI_RGB;
                    if (GetDIBits(dc, ii.hbmMask, 0, (UINT)h, mk.data(), &mi, DIB_RGB_COLORS))
                        for (int y = 0; y < h; ++y)
                            for (int x = 0; x < w; ++x) {
                                const bool transparent = (mk[(size_t)y * stride + (x >> 3)] >> (7 - (x & 7))) & 1;
                                px[((size_t)y * w + x) * 4 + 3] = transparent ? 0 : 255;
                            }
                } else if (!anyAlpha) {
                    for (size_t n = 3; n < (size_t)w * (size_t)h * 4; n += 4) px[n] = 255;
                }
                for (size_t n = 0; n < (size_t)w * (size_t)h; ++n) {
                    BYTE* p = px + n * 4;
                    if (p[3] != 255) { p[0] = (BYTE)(p[0] * p[3] / 255); p[1] = (BYTE)(p[1] * p[3] / 255); p[2] = (BYTE)(p[2] * p[3] / 255); }
                }
                *outPx = px; *outW = w; *outH = h; ok = true;
                px = NULL;
            }
        }
        if (dc) ReleaseDC(NULL, dc);
        delete[] px;
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return ok;
}

static HICON JumboIconFromSysIndex(int iIcon);
static void LoadIconFor(size_t i) {
    if (InterlockedExchange(&g_icons[i].tried, 1)) return;
    const AppEntry& a = g_apps[i];
    if (a.desktop) return;
    std::wstring path = AppTarget(a);
    if (a.charmCat == CC_FILE) {
        SHFILEINFOW info = {};
        const DWORD attrs = a.dir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
        HICON jumbo = NULL;
        if (SHGetFileInfoW(a.parse.c_str(), attrs, &info, sizeof(info),
                           SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES))
            jumbo = JumboIconFromSysIndex(info.iIcon);
        if (jumbo) {
            BYTE* px = NULL; int w = 0, h = 0;
            if (IconPixelsFromHICON(jumbo, &px, &w, &h)) {
                g_icons[i].px = px; g_icons[i].w = w; g_icons[i].h = h;
                InterlockedExchange(&g_icons[i].ready, 1);
            }
            DestroyIcon(jumbo);
            if (InterlockedCompareExchange(&g_icons[i].ready, 1, 1)) return;
        }
        info = {};
        if (SHGetFileInfoW(a.parse.c_str(), attrs, &info, sizeof(info),
                           SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES) && info.hIcon) {
            BYTE* px = NULL; int w = 0, h = 0;
            if (IconPixelsFromHICON(info.hIcon, &px, &w, &h)) {
                g_icons[i].px = px; g_icons[i].w = w; g_icons[i].h = h;
                InterlockedExchange(&g_icons[i].ready, 1);
            }
            DestroyIcon(info.hIcon);
        }
        return;
    }
    if (path.rfind(L"ms-settings:", 0) == 0) {
        wchar_t win[MAX_PATH] = {};
        if (GetWindowsDirectoryW(win, MAX_PATH))
            path = std::wstring(win) + L"\\ImmersiveControlPanel\\SystemSettings.exe";
    } else if (a.direct && path.find(L'\\') == std::wstring::npos && path.size() > 4 &&
               _wcsicmp(path.c_str() + path.size() - 4, L".exe") == 0) {
        wchar_t dir[MAX_PATH] = {};
        if (GetSystemDirectoryW(dir, MAX_PATH)) {
            std::wstring cand = std::wstring(dir) + L"\\" + path;
            if (GetFileAttributesW(cand.c_str()) != INVALID_FILE_ATTRIBUTES) path = cand;
        }
    }
    IShellItem* item = NULL;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), NULL, IID_PPV_ARGS(&item))) || !item) return;
    IShellItemImageFactory* factory = NULL;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
        const int want = std::min(256, std::max(48, 2 * ScI(48)));
        SIZE sz = { want, want };
        HBITMAP hbm = NULL;
        if (SUCCEEDED(factory->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbm)) && hbm) {
            BYTE* px = NULL; int w = 0, h = 0;
            if (BitmapToPixels(hbm, &px, &w, &h)) {
                g_icons[i].px = px; g_icons[i].w = w; g_icons[i].h = h;
                InterlockedExchange(&g_icons[i].ready, 1);
            }
            DeleteObject(hbm);
        }
        factory->Release();
    }
    item->Release();
}

static std::wstring ReadPackageManifest(const std::wstring& file) {
    HANDLE f = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return L"";
    DWORD size = GetFileSize(f, NULL);
    if (size == INVALID_FILE_SIZE || size < 4 || size > 2 * 1024 * 1024) { CloseHandle(f); return L""; }
    std::string bytes(size, '\0'); DWORD read = 0;
    BOOL ok = ReadFile(f, &bytes[0], size, &read, NULL);
    CloseHandle(f);
    if (!ok || !read) return L"";
    if (read >= 2 && (BYTE)bytes[0] == 0xff && (BYTE)bytes[1] == 0xfe) {
        std::wstring xml((read - 2) / sizeof(wchar_t), L'\0');
        memcpy(&xml[0], bytes.data() + 2, xml.size() * sizeof(wchar_t));
        return xml;
    }
    int start = read >= 3 && (BYTE)bytes[0] == 0xef && (BYTE)bytes[1] == 0xbb && (BYTE)bytes[2] == 0xbf ? 3 : 0;
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data() + start, (int)read - start, NULL, 0);
    if (n <= 0) return L"";
    std::wstring xml((size_t)n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data() + start, (int)read - start, &xml[0], n);
    return xml;
}

static std::wstring ManifestAsset(const std::wstring& xml, const std::wstring& appId,
                                  const wchar_t* name) {
    size_t app = xml.find(L"<Application ");
    while (app != std::wstring::npos) {
        size_t end = xml.find(L"</Application>", app);
        if (end == std::wstring::npos) break;
        size_t header = xml.find(L'>', app);
        if (header != std::wstring::npos && header < end &&
            xml.substr(app, header - app).find(L"Id=\"" + appId + L"\"") != std::wstring::npos) {
            std::wstring key = std::wstring(name) + L"=\"";
            size_t pos = xml.find(key, header);
            if (pos == std::wstring::npos || pos >= end) return L"";
            pos += key.size();
            size_t close = xml.find(L'"', pos);
            return close == std::wstring::npos || close > end ? L"" : xml.substr(pos, close - pos);
        }
        app = xml.find(L"<Application ", end);
    }
    return L"";
}

static bool IsImageFile(const std::wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static std::wstring PackageAssetFile(const std::wstring& root, std::wstring relative) {
    if (relative.empty() || relative.find(L"..") != std::wstring::npos ||
        relative.find(L':') != std::wstring::npos || relative[0] == L'\\' || relative[0] == L'/') return L"";
    for (wchar_t& ch : relative) if (ch == L'/') ch = L'\\';
    std::wstring full = root + L"\\" + relative;
    if (IsImageFile(full)) return full;
    size_t slash = full.find_last_of(L'\\'), dot = full.find_last_of(L'.');
    if (dot == std::wstring::npos || (slash != std::wstring::npos && dot < slash)) return L"";
    std::wstring dir = full.substr(0, slash + 1), file = full.substr(slash + 1);
    const wchar_t* scales[] = { L"scale-200", L"scale-100", L"scale-150", L"scale-400",
                               L"scale-125", L"scale-300" };
    for (const wchar_t* q : scales) {
        std::wstring candidate = full.substr(0, dot) + L"." + q + full.substr(dot);
        if (IsImageFile(candidate)) return candidate;
        candidate = dir + q + L"\\" + file;
        if (IsImageFile(candidate)) return candidate;
    }
    const wchar_t* folders[] = { L"contrast-standard\\theme-light\\", L"contrast-standard\\theme-dark\\",
                                 L"theme-light\\", L"theme-dark\\", L"contrast-standard\\" };
    for (const wchar_t* f : folders) for (const wchar_t* q : scales) {
        std::wstring candidate = dir + f + q + L"\\" + file;
        if (IsImageFile(candidate)) return candidate;
    }
    return L"";
}

static bool ParseManifestColor(const std::wstring& value, COLORREF* out) {
    if (value.empty()) return false;
    if (value[0] != L'#') {
        static const struct { const wchar_t* name; COLORREF color; } kNames[] = {
        { L"aliceblue", RGB(240, 248, 255) },
        { L"antiquewhite", RGB(250, 235, 215) },
        { L"aqua", RGB(0, 255, 255) },
        { L"aquamarine", RGB(127, 255, 212) },
        { L"azure", RGB(240, 255, 255) },
        { L"beige", RGB(245, 245, 220) },
        { L"bisque", RGB(255, 228, 196) },
        { L"black", RGB(0, 0, 0) },
        { L"blanchedalmond", RGB(255, 235, 205) },
        { L"blue", RGB(0, 0, 255) },
        { L"blueviolet", RGB(138, 43, 226) },
        { L"brown", RGB(165, 42, 42) },
        { L"burlywood", RGB(222, 184, 135) },
        { L"cadetblue", RGB(95, 158, 160) },
        { L"chartreuse", RGB(127, 255, 0) },
        { L"chocolate", RGB(210, 105, 30) },
        { L"coral", RGB(255, 127, 80) },
        { L"cornflowerblue", RGB(100, 149, 237) },
        { L"cornsilk", RGB(255, 248, 220) },
        { L"crimson", RGB(220, 20, 60) },
        { L"cyan", RGB(0, 255, 255) },
        { L"darkblue", RGB(0, 0, 139) },
        { L"darkcyan", RGB(0, 139, 139) },
        { L"darkgoldenrod", RGB(184, 134, 11) },
        { L"darkgray", RGB(169, 169, 169) },
        { L"darkgreen", RGB(0, 100, 0) },
        { L"darkgrey", RGB(169, 169, 169) },
        { L"darkkhaki", RGB(189, 183, 107) },
        { L"darkmagenta", RGB(139, 0, 139) },
        { L"darkolivegreen", RGB(85, 107, 47) },
        { L"darkorange", RGB(255, 140, 0) },
        { L"darkorchid", RGB(153, 50, 204) },
        { L"darkred", RGB(139, 0, 0) },
        { L"darksalmon", RGB(233, 150, 122) },
        { L"darkseagreen", RGB(143, 188, 143) },
        { L"darkslateblue", RGB(72, 61, 139) },
        { L"darkslategray", RGB(47, 79, 79) },
        { L"darkslategrey", RGB(47, 79, 79) },
        { L"darkturquoise", RGB(0, 206, 209) },
        { L"darkviolet", RGB(148, 0, 211) },
        { L"deeppink", RGB(255, 20, 147) },
        { L"deepskyblue", RGB(0, 191, 255) },
        { L"dimgray", RGB(105, 105, 105) },
        { L"dimgrey", RGB(105, 105, 105) },
        { L"dodgerblue", RGB(30, 144, 255) },
        { L"firebrick", RGB(178, 34, 34) },
        { L"floralwhite", RGB(255, 250, 240) },
        { L"forestgreen", RGB(34, 139, 34) },
        { L"fuchsia", RGB(255, 0, 255) },
        { L"gainsboro", RGB(220, 220, 220) },
        { L"ghostwhite", RGB(248, 248, 255) },
        { L"gold", RGB(255, 215, 0) },
        { L"goldenrod", RGB(218, 165, 32) },
        { L"gray", RGB(128, 128, 128) },
        { L"green", RGB(0, 128, 0) },
        { L"greenyellow", RGB(173, 255, 47) },
        { L"grey", RGB(128, 128, 128) },
        { L"honeydew", RGB(240, 255, 240) },
        { L"hotpink", RGB(255, 105, 180) },
        { L"indianred", RGB(205, 92, 92) },
        { L"indigo", RGB(75, 0, 130) },
        { L"ivory", RGB(255, 255, 240) },
        { L"khaki", RGB(240, 230, 140) },
        { L"lavender", RGB(230, 230, 250) },
        { L"lavenderblush", RGB(255, 240, 245) },
        { L"lawngreen", RGB(124, 252, 0) },
        { L"lemonchiffon", RGB(255, 250, 205) },
        { L"lightblue", RGB(173, 216, 230) },
        { L"lightcoral", RGB(240, 128, 128) },
        { L"lightcyan", RGB(224, 255, 255) },
        { L"lightgoldenrodyellow", RGB(250, 250, 210) },
        { L"lightgray", RGB(211, 211, 211) },
        { L"lightgreen", RGB(144, 238, 144) },
        { L"lightgrey", RGB(211, 211, 211) },
        { L"lightpink", RGB(255, 182, 193) },
        { L"lightsalmon", RGB(255, 160, 122) },
        { L"lightseagreen", RGB(32, 178, 170) },
        { L"lightskyblue", RGB(135, 206, 250) },
        { L"lightslategray", RGB(119, 136, 153) },
        { L"lightslategrey", RGB(119, 136, 153) },
        { L"lightsteelblue", RGB(176, 196, 222) },
        { L"lightyellow", RGB(255, 255, 224) },
        { L"lime", RGB(0, 255, 0) },
        { L"limegreen", RGB(50, 205, 50) },
        { L"linen", RGB(250, 240, 230) },
        { L"magenta", RGB(255, 0, 255) },
        { L"maroon", RGB(128, 0, 0) },
        { L"mediumaquamarine", RGB(102, 205, 170) },
        { L"mediumblue", RGB(0, 0, 205) },
        { L"mediumorchid", RGB(186, 85, 211) },
        { L"mediumpurple", RGB(147, 112, 219) },
        { L"mediumseagreen", RGB(60, 179, 113) },
        { L"mediumslateblue", RGB(123, 104, 238) },
        { L"mediumspringgreen", RGB(0, 250, 154) },
        { L"mediumturquoise", RGB(72, 209, 204) },
        { L"mediumvioletred", RGB(199, 21, 133) },
        { L"midnightblue", RGB(25, 25, 112) },
        { L"mintcream", RGB(245, 255, 250) },
        { L"mistyrose", RGB(255, 228, 225) },
        { L"moccasin", RGB(255, 228, 181) },
        { L"navajowhite", RGB(255, 222, 173) },
        { L"navy", RGB(0, 0, 128) },
        { L"oldlace", RGB(253, 245, 230) },
        { L"olive", RGB(128, 128, 0) },
        { L"olivedrab", RGB(107, 142, 35) },
        { L"orange", RGB(255, 165, 0) },
        { L"orangered", RGB(255, 69, 0) },
        { L"orchid", RGB(218, 112, 214) },
        { L"palegoldenrod", RGB(238, 232, 170) },
        { L"palegreen", RGB(152, 251, 152) },
        { L"paleturquoise", RGB(175, 238, 238) },
        { L"palevioletred", RGB(219, 112, 147) },
        { L"papayawhip", RGB(255, 239, 213) },
        { L"peachpuff", RGB(255, 218, 185) },
        { L"peru", RGB(205, 133, 63) },
        { L"pink", RGB(255, 192, 203) },
        { L"plum", RGB(221, 160, 221) },
        { L"powderblue", RGB(176, 224, 230) },
        { L"purple", RGB(128, 0, 128) },
        { L"rebeccapurple", RGB(102, 51, 153) },
        { L"red", RGB(255, 0, 0) },
        { L"rosybrown", RGB(188, 143, 143) },
        { L"royalblue", RGB(65, 105, 225) },
        { L"saddlebrown", RGB(139, 69, 19) },
        { L"salmon", RGB(250, 128, 114) },
        { L"sandybrown", RGB(244, 164, 96) },
        { L"seagreen", RGB(46, 139, 87) },
        { L"seashell", RGB(255, 245, 238) },
        { L"sienna", RGB(160, 82, 45) },
        { L"silver", RGB(192, 192, 192) },
        { L"skyblue", RGB(135, 206, 235) },
        { L"slateblue", RGB(106, 90, 205) },
        { L"slategray", RGB(112, 128, 144) },
        { L"slategrey", RGB(112, 128, 144) },
        { L"snow", RGB(255, 250, 250) },
        { L"springgreen", RGB(0, 255, 127) },
        { L"steelblue", RGB(70, 130, 180) },
        { L"tan", RGB(210, 180, 140) },
        { L"teal", RGB(0, 128, 128) },
        { L"thistle", RGB(216, 191, 216) },
        { L"tomato", RGB(255, 99, 71) },
        { L"turquoise", RGB(64, 224, 208) },
        { L"violet", RGB(238, 130, 238) },
        { L"wheat", RGB(245, 222, 179) },
        { L"white", RGB(255, 255, 255) },
        { L"whitesmoke", RGB(245, 245, 245) },
        { L"yellow", RGB(255, 255, 0) },
        { L"yellowgreen", RGB(154, 205, 50) },
        };
        for (const auto& c : kNames) if (!_wcsicmp(value.c_str(), c.name)) {
            *out = c.color;
            return true;
        }
        return false;
    }
    if (value.size() != 7 && value.size() != 9) return false;
    unsigned n = 0;
    for (size_t i = 1; i < value.size(); ++i) {
        wchar_t ch = towlower(value[i]);
        unsigned nibble = ch >= L'0' && ch <= L'9' ? ch - L'0' :
                          ch >= L'a' && ch <= L'f' ? ch - L'a' + 10 : 99;
        if (nibble > 15) return false;
        n = n * 16 + nibble;
    }
    if (value.size() == 9) {
        if ((n >> 24) < 128) return false;
        n &= 0xffffff;
    }
    *out = RGB((n >> 16) & 255, (n >> 8) & 255, n & 255);
    return true;
}

static bool IconCornerColor(const std::wstring& path, COLORREF* out) {
    if (path.empty()) return false;
    Gdiplus::Bitmap image(path.c_str(), FALSE);
    if (image.GetLastStatus() != Gdiplus::Ok || image.GetWidth() < 8 || image.GetHeight() < 8) return false;
    UINT x[] = { 2, image.GetWidth() - 3, 2, image.GetWidth() - 3 };
    UINT y[] = { 2, 2, image.GetHeight() - 3, image.GetHeight() - 3 };
    Gdiplus::Color first;
    if (image.GetPixel(x[0], y[0], &first) != Gdiplus::Ok || first.GetA() < 248) return false;
    for (int i = 1; i < 4; ++i) {
        Gdiplus::Color c;
        if (image.GetPixel(x[i], y[i], &c) != Gdiplus::Ok || c.GetA() < 248 ||
            abs((int)c.GetR() - first.GetR()) > 5 ||
            abs((int)c.GetG() - first.GetG()) > 5 ||
            abs((int)c.GetB() - first.GetB()) > 5) return false;
    }
    *out = RGB(first.GetR(), first.GetG(), first.GetB());
    return true;
}

static bool DecodePackageArt(const std::wstring& path, int outW, int outH,
                             std::vector<BYTE>* output, COLORREF background) {
    if (path.empty()) return false;
    Gdiplus::Bitmap image(path.c_str(), FALSE);
    if (image.GetLastStatus() != Gdiplus::Ok || !image.GetWidth() || !image.GetHeight()) return false;
    Gdiplus::Bitmap frame(outW, outH, PixelFormat32bppPARGB);
    if (frame.GetLastStatus() != Gdiplus::Ok) return false;
    Gdiplus::Graphics g(&frame);
    g.Clear(Gdiplus::Color(255, GetRValue(background), GetGValue(background), GetBValue(background)));
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    double k = std::max((double)outW / image.GetWidth(), (double)outH / image.GetHeight());
    int w = (int)(image.GetWidth() * k + .5), h = (int)(image.GetHeight() * k + .5);
    if (g.DrawImage(&image, Gdiplus::Rect((outW - w) / 2, (outH - h) / 2, w, h)) != Gdiplus::Ok) return false;
    Gdiplus::BitmapData data = {};
    Gdiplus::Rect rect(0, 0, outW, outH);
    if (frame.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &data) != Gdiplus::Ok)
        return false;
    output->resize((size_t)outW * outH * 4);
    for (int y = 0; y < outH; ++y)
        memcpy(output->data() + (size_t)y * outW * 4, (BYTE*)data.Scan0 + (ptrdiff_t)y * data.Stride,
               (size_t)outW * 4);
    frame.UnlockBits(&data);
    return true;
}

static PackageArt LoadPackageArt(const std::wstring& target) {
    PackageArt result; result.target = target;
    const wchar_t* aumid = target.c_str() + wcslen(L"shell:AppsFolder\\");
    const wchar_t* bang = wcschr(aumid, L'!');
    if (!bang || !bang[1]) return result;
    std::wstring family(aumid, bang - aumid), appId(bang + 1);
    HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
    using FindPkgs = LONG (WINAPI*)(PCWSTR, UINT32*, PWSTR*, UINT32*, PWSTR);
    using GetPath = LONG (WINAPI*)(PCWSTR, UINT32*, PWSTR);
    FindPkgs find = kernel ? (FindPkgs)(void*)GetProcAddress(kernel, "GetPackagesByPackageFamily") : NULL;
    GetPath getPath = kernel ? (GetPath)(void*)GetProcAddress(kernel, "GetPackagePathByFullName") : NULL;
    if (!find || !getPath) return result;
    UINT32 count = 0, bufferLen = 0;
    if (find(family.c_str(), &count, NULL, &bufferLen, NULL) != ERROR_INSUFFICIENT_BUFFER ||
        !count || !bufferLen || count > 32 || bufferLen > 32768) return result;
    std::vector<PWSTR> names(count);
    std::vector<wchar_t> buffer(bufferLen);
    if (find(family.c_str(), &count, names.data(), &bufferLen, buffer.data()) != ERROR_SUCCESS) return result;
    for (UINT32 i = 0; i < count; ++i) {
        if (!names[i] || WaitForSingleObject(g_artStop, 0) == WAIT_OBJECT_0) break;
        UINT32 pathLen = 0;
        if (getPath(names[i], &pathLen, NULL) != ERROR_INSUFFICIENT_BUFFER || !pathLen || pathLen > 32768) continue;
        std::vector<wchar_t> path(pathLen);
        if (getPath(names[i], &pathLen, path.data()) != ERROR_SUCCESS) continue;
        std::wstring root(path.data());
        std::wstring xml = ReadPackageManifest(root + L"\\AppxManifest.xml");
        result.hasBackground = ParseManifestColor(ManifestAsset(xml, appId, L"BackgroundColor"),
                                                  &result.background);
        if (!result.hasBackground) {
            std::wstring icon = PackageAssetFile(root, ManifestAsset(xml, appId, L"Square44x44Logo"));
            result.hasBackground = IconCornerColor(icon, &result.background);
        }
        std::wstring square = PackageAssetFile(root, ManifestAsset(xml, appId, L"Square150x150Logo"));
        std::wstring wide = PackageAssetFile(root, ManifestAsset(xml, appId, L"Wide310x150Logo"));
        if (!DecodePackageArt(square, TilePxW(TS_LARGE), TilePxH(TS_LARGE), &result.square, result.background)) continue;
        result.squareW = TilePxW(TS_LARGE); result.squareH = TilePxH(TS_LARGE);
        if (DecodePackageArt(wide.empty() ? square : wide, TilePxW(TS_WIDE), TilePxH(TS_WIDE), &result.wide, result.background)) {
            result.wideW = TilePxW(TS_WIDE); result.wideH = TilePxH(TS_WIDE);
        }
        Wh_Log(L"Default package tile image loaded for %s (wide asset: %d)", aumid, !wide.empty());
        break;
    }
    return result;
}

static DWORD WINAPI PackageArtThread(LPVOID) {
  RunGuarded(L"PackageArtThread", [] {
    ULONG_PTR gdip = 0;
    Gdiplus::GdiplusStartupInput input;
    if (Gdiplus::GdiplusStartup(&gdip, &input, NULL) != Gdiplus::Ok) return;
    HANDLE events[] = { g_artStop, g_artWake };
    for (;;) {
        if (WaitForSingleObject(g_artStop, 0) == WAIT_OBJECT_0) break;
        std::wstring target;
        AcquireSRWLockExclusive(&g_artLock);
        if (!g_artQueue.empty()) {
            target = g_artQueue.back(); g_artQueue.pop_back();
            g_artLoading = target;
        }
        ReleaseSRWLockExclusive(&g_artLock);
        if (target.empty()) {
            if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0) break;
            continue;
        }
        PackageArt art = LoadPackageArt(target);
        bool ok = !art.square.empty() || art.hasBackground;
        AcquireSRWLockExclusive(&g_artLock);
        g_packageArts.push_back(std::move(art));
        if (ok) g_artChanged.push_back(target);
        g_artLoading.clear();
        ReleaseSRWLockExclusive(&g_artLock);
        if (ok && g_wnd) PostMessageW(g_wnd, WM_APP_PACKAGE_ART, 0, 0);
    }
    Gdiplus::GdiplusShutdown(gdip);
  });
    return 0;
}

static void WalkStartLnks(const std::wstring& dir, int depth, size_t rootLen,
                          std::vector<std::pair<std::wstring, std::wstring>>* out) {
    if (depth > 6) return;
    WIN32_FIND_DATAW fd = {};
    HANDLE hf = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.cFileName[0] == L'.') continue;
        std::wstring path = dir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                WalkStartLnks(path, depth + 1, rootLen, out);
        } else {
            std::wstring fn = fd.cFileName;
            if (fn.size() > 4 && _wcsicmp(fn.c_str() + fn.size() - 4, L".lnk") == 0)
                out->push_back({ LowerW(fn.substr(0, fn.size() - 4)), path.substr(rootLen + 1) });
        }
    } while (FindNextFileW(hf, &fd));
    FindClose(hf);
}

static DWORD WINAPI AppsThread(LPVOID) {
  RunGuarded(L"AppsThread", [] {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    for (bool first = true;; first = false) {
    std::vector<AppEntry> list;

    PIDLIST_ABSOLUTE pidl = NULL;
    if (SUCCEEDED(SHParseDisplayName(L"shell:AppsFolder", NULL, &pidl, 0, NULL)) && pidl) {
        IShellFolder* folder = NULL;
        if (SUCCEEDED(SHBindToObject(NULL, pidl, NULL, IID_PPV_ARGS(&folder))) && folder) {
            IEnumIDList* en = NULL;
            if (SUCCEEDED(folder->EnumObjects(NULL, SHCONTF_NONFOLDERS | SHCONTF_FOLDERS, &en)) && en) {
                PITEMID_CHILD child = NULL;
                ULONG got = 0;
                while (en->Next(1, &child, &got) == S_OK && child) {
                    STRRET s1 = {}, s2 = {};
                    wchar_t name[512] = L"", parse[1024] = L"";
                    if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_NORMAL, &s1))) StrRetToBufW(&s1, child, name, 512);
                    if (SUCCEEDED(folder->GetDisplayNameOf(child, SHGDN_FORPARSING, &s2))) StrRetToBufW(&s2, child, parse, 1024);
                    if (name[0] && parse[0]) AddApp(list, name, parse, false);
                    CoTaskMemFree(child);
                    child = NULL;
                    if (WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) break;
                }
                en->Release();
            }
            folder->Release();
        }
        CoTaskMemFree(pidl);
    }
    AddApp(list, T(STR_DESKTOP), kDesktopTarget, false, true);

    {
        std::vector<std::pair<std::wstring, std::wstring>> lnks;
        for (int csidl : { CSIDL_COMMON_PROGRAMS, CSIDL_PROGRAMS }) {
            wchar_t root[MAX_PATH] = L"";
            if (!SUCCEEDED(SHGetFolderPathW(NULL, csidl, NULL, 0, root))) continue;
            WalkStartLnks(std::wstring(root), 0, wcslen(root), &lnks);
        }
        for (auto& a : list) {
            const std::wstring full = (a.exe || a.parse.find(L":\\") != std::wstring::npos ||
                                       a.parse.find(L":\\") != std::wstring::npos) ? a.parse : std::wstring(L"shell:AppsFolder\\") + a.parse;
            if (a.cat == 0 && IsPackagedAppTarget(full.c_str())) continue;
            std::wstring rel;
            const std::wstring low = a.lower;
            for (const auto& kv : lnks) if (kv.first == low) { rel = kv.second; break; }
            if (rel.empty()) { a.catName = a.name; continue; }
            const size_t slash = rel.find_last_of(L"\\/");
            const std::wstring dir = slash == std::wstring::npos ? std::wstring() : rel.substr(0, slash);
            const size_t comp = dir.find_first_of(L"\\/");
            a.catName = dir.empty() ? a.name : (comp == std::wstring::npos ? dir : dir.substr(0, comp));
        }
    }
    Wh_Log(L"Search: %u apps including Desktop", (unsigned)list.size());

    wchar_t sys[MAX_PATH] = L"";
    if (GetSystemDirectoryW(sys, MAX_PATH)) {
        std::wstring dir = sys;
        WIN32_FIND_DATAW fd = {};
        HANDLE find = FindFirstFileW((dir + L"\\*.exe").c_str(), &fd);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                std::wstring file = fd.cFileName;
                if (file.size() > 4) AddApp(list, file.substr(0, file.size() - 4).c_str(), dir + L"\\" + file, true);
            } while (FindNextFileW(find, &fd));
            FindClose(find);
        }
    }

    AddCharmSystemEntries(list);
    Wh_Log(L"Search: %u catalog entries including charm settings", (unsigned)list.size());

    g_catalogBuiltTick = GetTickCount64();
    if (first) {
        AcquireSRWLockExclusive(&g_appsLock);
        g_apps.swap(list);
        g_icons.clear();
        g_icons.resize(g_apps.size());
        ReleaseSRWLockExclusive(&g_appsLock);
        InterlockedExchange(&g_appsReady, 1);
        if (g_wnd) PostMessageW(g_wnd, WM_APP_APPS_READY, 0, 0);
    } else {
        std::vector<AppEntry>* np = new std::vector<AppEntry>(std::move(list));
        AcquireSRWLockExclusive(&g_appsLock);
        delete g_pendingApps;
        g_pendingApps = np;
        ReleaseSRWLockExclusive(&g_appsLock);
        if (g_wnd) PostMessageW(g_wnd, WM_APP_APPS_REFRESH, 0, 0);
        HANDLE go[2] = { g_appsStop, g_iconsGo };
        if (!g_iconsGo || WaitForMultipleObjects(2, go, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) break;
        ResetEvent(g_iconsGo);
    }

    for (size_t i = 0; i < g_apps.size(); ++i) {
        if (WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) break;
        if (g_apps[i].charmCat == CC_FILE || g_apps[i].exe) continue;
        LoadIconFor(i);
        if ((i & 7) == 7 && g_wnd) PostMessageW(g_wnd, WM_APP_ICON_READY, 0, 0);
    }
    if (g_wnd) PostMessageW(g_wnd, WM_APP_ICON_READY, 0, 0);

    // Wait for either a file-scan request or a catalog refresh request.
    HANDLE wait3[3] = { g_appsStop, g_filesNeed, g_appsRefresh };
    DWORD w = WaitForMultipleObjects(g_filesNeed ? 3 : 1, wait3, FALSE, INFINITE);
    if (w == WAIT_OBJECT_0 + 1) {
        std::vector<AppEntry>* files = new std::vector<AppEntry>();
        ScanCharmFiles(*files);
        const bool stopped = WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0;
        if (stopped || files->empty()) {
            delete files;
        } else {
            AcquireSRWLockExclusive(&g_appsLock);
            delete g_pendingFiles;
            g_pendingFiles = files;
            ReleaseSRWLockExclusive(&g_appsLock);
            if (g_wnd) PostMessageW(g_wnd, WM_APP_FILES_READY, 0, 0);
        }
        if (stopped) break;
        HANDLE wait2[2] = { g_appsStop, g_appsRefresh };
        if (WaitForMultipleObjects(g_appsRefresh ? 2 : 1, wait2, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) break;
    } else if (w != WAIT_OBJECT_0 + 2) {
        break;
    }
    ResetEvent(g_appsRefresh);
    }
    CoUninitialize();
  });
    return 0;
}

static int RowsPerCol() {
    int rows = (g_sh - kItemY - ScI(24)) / kItemPitchY;
    return rows < 1 ? 1 : rows;
}

static float MaxScrollSearch() {
    int rows = RowsPerCol();
    int cols = g_queryLen == 0 ? (int)g_appHeadings.size() :
               ((int)g_results.size() + rows - 1) / rows;
    float right = (float)(kItemX + cols * kItemPitchX + ScI(40));
    float m = right - (float)g_sw;
    return m > 0.0f ? m : 0.0f;
}

static int ResultCol(int i) {
    return g_queryLen == 0 && i < (int)g_appCol.size() ? g_appCol[i] : i / RowsPerCol();
}
static int ResultRow(int i) {
    return g_queryLen == 0 && i < (int)g_appRow.size() ? g_appRow[i] : i % RowsPerCol();
}

static int HitResult(int mx, int my) {
    float sx = (float)mx + g_scrollCur;
    for (int i = 0; i < (int)g_results.size(); ++i) {
        float x = (float)(kItemX + ResultCol(i) * kItemPitchX);
        float y = (float)(kItemY + ResultRow(i) * kItemPitchY);
        if (sx >= x && sx < x + kItemW && (float)my >= y && (float)my < y + kItemH) return i;
    }
    return -1;
}

static void RepaintNow() {
    g_dirty = true;
    if (g_wnd) InvalidateRect(g_wnd, NULL, FALSE);
}


static wchar_t AppInitial(const std::wstring& name) {
    for (wchar_t c : name) {
        if (iswspace(c)) continue;
        return iswdigit(c) ? L'#' : (wchar_t)towupper(c);
    }
    return L'#';
}

static int DateBucket(unsigned long long ft) {
    if (!ft) return 5;
    FILETIME nf; GetSystemTimeAsFileTime(&nf);
    unsigned long long now = ((unsigned long long)nf.dwHighDateTime << 32) | nf.dwLowDateTime;
    const unsigned long long DAY = 864000000000ULL;
    unsigned long long dNow = now / DAY, d = ft / DAY;
    if (d == dNow) return 0;
    unsigned long long wNow = dNow / 7, w = d / 7;
    if (w == wNow) return 1;
    if (w + 1 == wNow) return 2;
    SYSTEMTIME sn, st2; FileTimeToSystemTime(&nf, &sn);
    FILETIME ff; ff.dwLowDateTime = (DWORD)ft; ff.dwHighDateTime = (DWORD)(ft >> 32);
    FileTimeToSystemTime(&ff, &st2);
    if (st2.wYear == sn.wYear && st2.wMonth == sn.wMonth) return 3;
    int pm = sn.wMonth == 1 ? 12 : sn.wMonth - 1;
    int py = sn.wMonth == 1 ? sn.wYear - 1 : sn.wYear;
    if (st2.wYear == (unsigned)py && st2.wMonth == (unsigned)pm) return 4;
    return 5;
}

static void LayoutAllApps() {
    g_appCol.clear(); g_appRow.clear(); g_appHeadings.clear();
    const int sortNow = (int)InterlockedCompareExchange(&g_appSort, 0, 0);
    const bool byDate = sortNow == 1, byCat = sortNow == 2;
    int rows = RowsPerCol(), col = 0, row = 0;
    const int n = (int)g_results.size();
    if (byCat) {
        int i = 0;
        auto packaged = [&](int idx) { return IsPackagedAppTarget((std::wstring(L"shell:AppsFolder\\") + g_apps[g_results[idx]].parse).c_str()); };
        while (i < n && packaged(i)) {
            wchar_t letter = AppInitial(g_apps[g_results[i]].name);
            int end = i + 1;
            while (end < n && packaged(end) && AppInitial(g_apps[g_results[end]].name) == letter) ++end;
            if (row && end - i <= rows && row + end - i > rows) { ++col; row = 0; }
            for (; i < end; ++i) {
                if (row == rows) { ++col; row = 0; }
                if (row == 0) g_appHeadings.push_back(AppHeading{ col, letter, letter, -1, L"" });
                else g_appHeadings.back().last = letter;
                g_appCol.push_back(col); g_appRow.push_back(row++);
            }
        }
        while (i < n) {
            const std::wstring grp = g_apps[g_results[i]].catName;
            int end = i + 1;
            while (end < n && !packaged(end) && g_apps[g_results[end]].catName == grp) ++end;
            if (row) { ++col; row = 0; }
            for (; i < end; ++i) {
                if (row == rows) { ++col; row = 0; }
                if (row == 0) g_appHeadings.push_back(AppHeading{ col, L' ', L' ', -1, grp });
                g_appCol.push_back(col); g_appRow.push_back(row++);
            }
        }
        return;
    }
    for (int i = 0; i < (int)g_results.size();) {
        wchar_t letter = AppInitial(g_apps[g_results[i]].name);
        int key = byDate ? DateBucket(g_apps[g_results[i]].date) : -1;
        int end = i + 1;
        while (end < (int)g_results.size() &&
               (byDate ? DateBucket(g_apps[g_results[end]].date) == key

                       : AppInitial(g_apps[g_results[end]].name) == letter)) ++end;
        if (row && end - i <= rows && row + end - i > rows) { ++col; row = 0; }
        for (; i < end; ++i) {
            if (row == rows) { ++col; row = 0; }
            if (row == 0) g_appHeadings.push_back(byDate ? AppHeading{ col, L' ', L' ', key }
                                                         : AppHeading{ col, letter, letter, -1 });
            else if (!byDate) g_appHeadings.back().last = letter;
            g_appCol.push_back(col);
            g_appRow.push_back(row++);
        }
    }
}

static void RunSearch() {
    g_alphaGrid = false;
    g_results.clear();
    g_appCol.clear(); g_appRow.clear(); g_appHeadings.clear();
    g_selected = g_queryLen ? 0 : -1;
    if (!InterlockedCompareExchange(&g_appsReady, 0, 0)) return;
    std::wstring q = LowerW(std::wstring(g_query, (size_t)g_queryLen));
    struct Hit { int rank; int idx; };
    std::vector<Hit> hits;
    if (q.empty()) {
        for (size_t i = 0; i < g_apps.size(); ++i) {
            const AppEntry& a = g_apps[i];
            if (a.exe || a.searchOnly) continue;
            hits.push_back({ 0, (int)i });
        }
        std::sort(hits.begin(), hits.end(), [](const Hit& a, const Hit& b) {
            return g_apps[a.idx].lower < g_apps[b.idx].lower;
        });
        for (const Hit& h : hits) g_results.push_back(h.idx);
    } else {
        std::vector<std::wstring> words;
        size_t st = 0;
        for (size_t i = 0; i <= q.size(); ++i) {
            if (i == q.size() || q[i] == L' ') {
                if (i > st) words.push_back(q.substr(st, i - st));
                st = i + 1;
            }
        }
        if (words.empty()) return;
        if ((g_charmScope == 0 || g_charmScope == 3) && g_filesNeed) SetEvent(g_filesNeed);
        for (size_t i = 0; i < g_apps.size(); ++i) {
            const AppEntry& a = g_apps[i];
            if (a.exe) {
                if (a.lower == q || a.lower + L".exe" == q) hits.push_back({ 0, (int)i });
                continue;
            }
            if (g_charmScope == 1 && a.charmCat != CC_APP)     continue;
            if (g_charmScope == 2 && a.charmCat != CC_SETTING) continue;
            if (g_charmScope == 3 && a.charmCat != CC_FILE)    continue;
            bool all = true;
            for (const std::wstring& w : words)
                if (a.lower.find(w) == std::wstring::npos) { all = false; break; }
            if (!all) continue;
            const int rank = (a.charmCat == CC_APP ? 0 : a.charmCat == CC_SETTING ? 2 : 4)
                           + (a.lower.rfind(q, 0) == 0 ? 0 : 2)
                           + (a.dir ? 1 : 0);
            hits.push_back({ rank, (int)i });
        }
        auto cmp = [](const Hit& a, const Hit& b) {
            if (a.rank != b.rank) return a.rank < b.rank;
            const int c = g_apps[a.idx].lower.compare(g_apps[b.idx].lower);
            if (c != 0) return c < 0;
            return a.idx < b.idx;
        };
        const size_t keep = std::min(hits.size(), (size_t)300);
        std::partial_sort(hits.begin(), hits.begin() + keep, hits.end(), cmp);
        hits.resize(keep);
        for (const Hit& h : hits) g_results.push_back(h.idx);
    }
    if (!g_queryLen) {
        const int sort = (int)InterlockedCompareExchange(&g_appSort, 0, 0);
        if (sort == 1)
            std::stable_sort(g_results.begin(), g_results.end(), [](int a, int b) {
                return g_apps[a].date > g_apps[b].date; });
        else if (sort == 2)
            std::stable_sort(g_results.begin(), g_results.end(), [](int a, int b) {
                const bool pa = IsPackagedAppTarget((std::wstring(L"shell:AppsFolder\\") + g_apps[a].parse).c_str());
                const bool pb = IsPackagedAppTarget((std::wstring(L"shell:AppsFolder\\") + g_apps[b].parse).c_str());
                if (pa != pb) return pa;
                if (!pa) {
                    const int c = _wcsicmp(g_apps[a].catName.c_str(), g_apps[b].catName.c_str());
                    if (c != 0) return c < 0;
                }
                return g_apps[a].lower < g_apps[b].lower; });
        LayoutAllApps();
    }
}

static void QueryAppend(wchar_t c) {
    if (g_queryLen < 120) { g_query[g_queryLen++] = c; g_query[g_queryLen] = 0; }
}

static void QueryBackspace() {
    if (g_queryLen > 0) g_query[--g_queryLen] = 0;
}

static float LeaveEase(int i, DWORD now) {
    if (!g_leaveTick) return 0.0f;
    float t = (float)(now - g_leaveTick) - (float)(i % 6) * 12.0f;
    return Clamp01(t / 160.0f);
}
static void CommitAllApps() {
    g_leaveTick = 0;
    if (!g_rt || !g_bgPlainGpu) return;
    g_view = VIEW_SEARCH;
    g_queryLen = 0; g_query[0] = 0;
    g_scrollCur = g_scrollTarget = 0.0f;
    g_hover = g_pressed = -1;
    g_returnTick = 0;
    RunSearch();
    g_searchTick = NowMs();
    RepaintNow();
}
static void EnterAllApps() {
    g_alphaGrid = false;
    g_appsSearchFocus = false;
    if (g_zoomOut || g_zoomTick) {
        g_zoomOut = false; g_zoomTick = 0; g_hoverGroup = -1; g_dragGroup = -1;
        if (g_namingGroup >= 0) CancelNaming();
    }
    if (g_leaveTick) return;
    CloseCharm(true);
    g_leaveTick = NowMs();
    RepaintNow();
}

static void EnterSearchEmpty() {
    CharmToggle();
}

static void LeaveSearch() {
    DWORD now = NowMs();
    g_logonOpening = false;
    g_view = VIEW_START;
    g_hover = g_pressed = -1;
    g_appsSearchFocus = false;
    g_scrollCur = g_scrollTarget = 0.0f;
    for (int i = 0; i < MAX_TILES; i++) g_tileScale[i] = 1.0f;
    g_returnTick = now;
    g_leaveTick = 0;
    g_skipSnapshot = true;
    g_openTick = now - (DWORD)((TILES_DELAY - 0.08f) * 1000.0f);
    RepaintNow();
}

static void LaunchEntry(const AppEntry& a) {
    if (a.desktop) { CloseStart(g_firstDesktopAfterLogon); return; }
    std::wstring target = AppTarget(a);
    CloseStart();
    ShellExecuteW(NULL, L"open", target.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
static void LaunchResult(int i) {
    if (i < 0 || i >= (int)g_results.size()) return;
    LaunchEntry(g_apps[g_results[i]]);
}

static void OnSearchChar(wchar_t c) {
    if (g_charmOpen) {
        g_charmDdOpen = false; g_charmDdHover = -1;
        QueryAppend(c); RunSearch();
        if (g_charmSel >= (int)g_results.size()) g_charmSel = 0;
        CharmRender(g_charmCont); RepaintNow(); return;
    }
    if (g_view == VIEW_START) { OpenCharm(c); return; }
    g_appsSearchFocus = true;
    g_alphaGrid = false;
    QueryAppend(c);
    RunSearch();
    g_scrollCur = g_scrollTarget = 0.0f;
    RepaintNow();
}

static void MoveSelection(int delta) {
    int n = (int)g_results.size();
    if (!n) return;
    if (g_selected < 0) { g_selected = 0; RepaintNow(); return; }
    if (!g_queryLen && (delta == RowsPerCol() || delta == -RowsPerCol())) {
        int targetCol = ResultCol(g_selected) + (delta > 0 ? 1 : -1);
        int targetRow = ResultRow(g_selected), nearest = -1, distance = 9999;
        for (int i = 0; i < n; ++i) if (ResultCol(i) == targetCol) {
            int d = abs(ResultRow(i) - targetRow);
            if (d < distance) { distance = d; nearest = i; }
        }
        if (nearest >= 0) g_selected = nearest;
    } else g_selected += delta;
    if (g_selected < 0) g_selected = 0;
    if (g_selected >= n) g_selected = n - 1;
    float x = (float)(kItemX + ResultCol(g_selected) * kItemPitchX);
    if (x - g_scrollTarget < 0.0f) g_scrollTarget = x - (float)kItemX;
    else if (x + kItemW - g_scrollTarget > (float)g_sw - ScF(40.0f)) g_scrollTarget = x + kItemW - (float)g_sw + ScF(40.0f);
    float m = MaxScrollSearch();
    if (g_scrollTarget < 0.0f) g_scrollTarget = 0.0f;
    if (g_scrollTarget > m) g_scrollTarget = m;
    RepaintNow();
}

static void OnSearchKey(UINT vk) {
    if (g_charmOpen) {
        switch (vk) {
        case VK_ESCAPE:
            if (g_charmDdOpen) { g_charmDdOpen = false; g_charmDdHover = -1; CharmRender(g_charmCont); RepaintNow(); }
            else CloseCharm(false);
            break;
        case VK_BACK:
            g_charmDdOpen = false; g_charmDdHover = -1;
            QueryBackspace(); RunSearch();
            if (g_charmSel >= (int)g_results.size()) g_charmSel = 0;
            CharmRender(g_charmCont); RepaintNow(); break;
        case VK_RETURN:
            if (!g_results.empty()) LaunchResult(g_charmSel < (int)g_results.size() ? g_charmSel : 0);
            break;
        case VK_DOWN: CharmMoveSel(1); break;
        case VK_UP:   CharmMoveSel(-1); break;
        }
        return;
    }
    if (g_view != VIEW_SEARCH) {
        if (vk == VK_ESCAPE) {
            if (g_namingGroup >= 0) CancelNaming();
            else if (g_barVisible) HideBar();
            else if (ZoomPresence(NowMs()) > 0.0f) ExitZoom(-1);
            else CloseStart();
        } else if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN) {
            if (!g_barVisible && ZoomPresence(NowMs()) <= 0.0f) FocusMove(vk);
        } else if (vk == VK_RETURN) {
            if (!g_barVisible && g_focus >= 0 && g_focus < g_tileCount && g_tiles[g_focus].alive)
                Launch(g_focus);
        } else if (vk == VK_APPS) {
            if (g_barVisible) HideBar();
            else if (g_focus >= 0 && g_focus < g_tileCount && g_tiles[g_focus].alive)
                ShowBar(1, g_focus);
        }
        return;
    }
    switch (vk) {
    case VK_ESCAPE:
        if (g_alphaGrid) { g_alphaGrid = false; g_searchTick = NowMs(); RepaintNow(); break; }
        if (g_barVisible) { HideBar(); break; }
        LeaveSearch();
        break;
    case VK_BACK:
        QueryBackspace();
        RunSearch();
        RepaintNow();
        break;
    case VK_RETURN: LaunchResult(g_selected); break;
    case VK_DOWN:   MoveSelection(1); break;
    case VK_UP:     MoveSelection(-1); break;
    case VK_RIGHT:  MoveSelection(RowsPerCol()); break;
    case VK_LEFT:   MoveSelection(-RowsPerCol()); break;
    }
}

// Search
static void OnSearchClick(int x, int y) {
    if (g_alphaGrid) { AlphaGridClick(x, y); return; }
    if (BarClick(x, y)) return;
    {
        RECT sb; AppsSearchBoxRect(&sb);
        if (x >= sb.left && x < sb.right && y >= sb.top && y < sb.bottom) {
            if (g_queryLen > 0 && x >= sb.right - ScI(36)) { g_queryLen = 0; g_query[0] = 0; RunSearch(); }
            g_appsSearchFocus = true;
            RepaintNow(); return;
        }
    }
    if (g_queryLen == 0) {
        int hh = HitHeading(x, y);
        if (hh >= 0) { g_alphaGrid = true; g_alphaTick = NowMs(); RepaintNow(); return; }
    }
    int r = HitResult(x, y);
    if (r >= 0) { LaunchResult(r); return; }
}

static void SetBr(COLORREF c, float a) {
    g_scratch->SetColor(D2D1::ColorF(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f, GetBValue(c) / 255.0f));
    g_scratch->SetOpacity(a);
}

static void FillRc(float l, float t, float r, float b, COLORREF c, float a) {
    SetBr(c, a);
    g_rt->FillRectangle(D2D1::RectF(l, t, r, b), g_scratch);
}

static void TextAt(const wchar_t* s, IDWriteTextFormat* f, float l, float t, float r, float b, COLORREF c, float a) {
    if (!s || !*s || !f) return;
    SetBr(c, a);
    g_rt->DrawText(s, (UINT32)wcslen(s), f, D2D1::RectF(l, t, r, b), g_scratch, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

static ID2D1Bitmap* IconGpu(int appIdx) {
    if (appIdx < 0 || appIdx >= (int)g_icons.size()) return NULL;
    if (g_apps[appIdx].desktop) {
        if (!g_wallReady) return NULL;
        if (!g_desktopAppGpu) {
            D2D1_BITMAP_PROPERTIES props = GpuBitmapProperties();
            g_rt->CreateBitmap(D2D1::SizeU(TILE, TILE), g_wallPixels,
                               TILE * 4, &props, &g_desktopAppGpu);
        }
        return g_desktopAppGpu;
    }
    IconSlot& s = g_icons[appIdx];
    if (s.gpu) return s.gpu;
    if (!InterlockedCompareExchange(&s.ready, 0, 0) && g_apps[appIdx].exe) LoadIconFor((size_t)appIdx);
    if (!InterlockedCompareExchange(&s.ready, 0, 0) || !s.px) return NULL;
    D2D1_BITMAP_PROPERTIES p = {};
    p.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    p.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    p.dpiX = p.dpiY = 96.0f;
    g_rt->CreateBitmap(D2D1::SizeU((UINT32)s.w, (UINT32)s.h), s.px, (UINT32)s.w * 4, &p, &s.gpu);
    return s.gpu;
}

static void DrawSearchGpu(DWORD now, bool closing) {
    if (!g_scratch || !g_subFormat) return;
    g_whiteGpu->SetOpacity(1.0f);
    g_rt->DrawText(T(STR_APPS), 4, g_titleFormat,
                   D2D1::RectF((float)LEFT, 40.0f, (float)LEFT + 400, 140.0f), g_whiteGpu);
    if (g_queryLen > 0) {
        wchar_t sub[384];
        swprintf_s(sub, T(STR_RESULTS_FOR), std::wstring(g_query, (size_t)g_queryLen).c_str(),
                   (unsigned)g_results.size());
        float sx = (float)LEFT + ScF(150.0f);
        HDC dc = GetDC(NULL);
        if (dc && g_fTitle) {
            HGDIOBJ old = SelectObject(dc, g_fTitle);
            SIZE sz = {};
            if (GetTextExtentPoint32W(dc, T(STR_APPS), (int)wcslen(T(STR_APPS)), &sz))
                sx = (float)LEFT + (float)sz.cx + ScF(18.0f);
            SelectObject(dc, old);
        }
        if (dc) ReleaseDC(NULL, dc);
        TextAt(sub, g_subFormat, sx, 77.0f, 1500.0f, 110.0f, RGB(181, 189, 217));
    }

    if (g_alphaGrid && !closing) return;
    float fade = Clamp01(((float)(now - g_searchTick) / 1000.0f - 0.04f) / RESULT_FADE);
    if (g_results.empty()) {
        if (g_queryLen > 0) TextAt(T(STR_NO_RESULTS), g_itemFormat,
                                   (float)LEFT, ScF(186.0f), ScF(900.0f), ScF(230.0f), RGB(225, 225, 235), fade);
    } else {
        if (g_queryLen == 0) {
            for (const AppHeading& h : g_appHeadings) {
                float x = (float)(kItemX + h.col * kItemPitchX) - g_scrollCur;
                if (x + kItemW < 0.0f || x > (float)g_sw) continue;
                if (!h.text.empty()) {
                    TextAt(h.text.c_str(), g_subFormat, x + ScF(8.0f), ScF(135.0f), x + kItemW, ScF(167.0f),
                           RGB(235, 235, 245), fade);
                } else if (h.bucket >= 0) {
                    TextAt(T((StrId)(STR_DATE_TODAY + h.bucket)), g_subFormat, x + ScF(8.0f), ScF(135.0f), x + kItemW, ScF(167.0f),
                           RGB(235, 235, 245), fade);
                } else {
                    wchar_t label[4] = { h.first, 0, 0, 0 };
                    if (h.last != h.first) { label[1] = L'\x2013'; label[2] = h.last; }
                    TextAt(label, g_subFormat, x + ScF(8.0f), ScF(135.0f), x + kItemW, ScF(167.0f),
                           RGB(235, 235, 245), fade);
                }
            }
        }
        for (int i = 0; i < (int)g_results.size(); ++i) {
            float x = (float)(kItemX + ResultCol(i) * kItemPitchX) - g_scrollCur;
            float y = (float)(kItemY + ResultRow(i) * kItemPitchY);
            if (x + kItemW < 0.0f || x > (float)g_sw) continue;
            int ai = g_results[i];
            COLORREF plate = g_apps[ai].lower == L"store" ? RGB(172, 137, 0) : RGB(92, 40, 138);
            FillRc(x + ScF(8.0f), y + ScF(7.0f), x + ScF(53.0f), y + ScF(52.0f), plate, fade);
            if (ID2D1Bitmap* ic = IconGpu(ai))
                g_rt->DrawBitmap(ic, D2D1::RectF(x + ScF(10.5f), y + ScF(9.5f), x + ScF(50.5f), y + ScF(49.5f)), fade,
                                 D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            TextAt(g_apps[ai].name.c_str(), g_itemFormat, x + ScF(60.0f), y + ScF(8.0f), x + (float)kItemW - ScF(4.0f), y + ScF(54.0f),
                   RGB(235, 235, 245), fade);
            if (i == g_selected) {
                SetBr(RGB(255, 255, 255), fade);
                g_rt->DrawRectangle(D2D1::RectF(x + 1.0f, y + 1.0f, x + (float)kItemW - 1.0f, y + (float)kItemH - 1.0f),
                                    g_scratch, 2.0f);
            }
        }
    }

    (void)now;
}

static const wchar_t* DupW(const wchar_t* s) { return (s && *s) ? _wcsdup(s) : NULL; }

static void FreeTile(Tile& t) {
    if (t.icon) { DestroyIcon(t.icon); t.icon = NULL; }
    if (t.pinBmp) { DeleteObject(t.pinBmp); t.pinBmp = NULL; }
    if (t.owned) { free((void*)t.label); free((void*)t.target); free((void*)t.iconFile); }
    t.label = t.target = t.iconFile = NULL;
    t.owned = false;
}

static void ReleaseTileSurface(int i) {
    for (int v = 0; v < 2; ++v) {
        if (g_tileGpu[i][v]) { g_tileGpu[i][v]->Release(); g_tileGpu[i][v] = NULL; }
        if (g_tileDC[i][v]) {
            if (g_tileOld[i][v]) SelectObject(g_tileDC[i][v], g_tileOld[i][v]);
            DeleteDC(g_tileDC[i][v]); g_tileDC[i][v] = NULL;
        }
        if (g_tileBmp[i][v]) { DeleteObject(g_tileBmp[i][v]); g_tileBmp[i][v] = NULL; }
        g_tileOld[i][v] = NULL;
        g_tileBits[i][v] = NULL;
    }
}

static void RebuildTile(int i) {
    ReleaseTileSurface(i);
    HDC screen = GetDC(NULL);
    if (screen) { BuildOneTile(i, screen); ReleaseDC(NULL, screen); }
    GdiFlush();
}

static void ApplyPackageArtChanges() {
    std::vector<std::wstring> changed;
    AcquireSRWLockExclusive(&g_artLock);
    changed.swap(g_artChanged);
    ReleaseSRWLockExclusive(&g_artLock);
    for (int i = 0; i < g_tileCount; ++i) {
        Tile& t = g_tiles[i];
        if (!t.alive || t.style != PINNED_APP || !t.target) continue;
        for (const auto& target : changed) if (!_wcsicmp(t.target, target.c_str())) {
            RebuildTile(i);
            break;
        }
    }
    g_artRebuildPending = false;
    if (g_open && !changed.empty()) RepaintNow();
}

static bool FindGroupSlot(const std::vector<char>& occ, int cols, int rows,
                          int aw, int ah, int cursor, int* ox, int* oy) {
    if (aw < 1 || ah < 1 || aw > cols || ah > rows) return false;
    for (int lin = cursor; lin < cols * rows; ++lin) {
        int x = lin % cols, y = lin / cols;
        if (x + aw > cols || y + ah > rows) continue;
        bool freeAll = true;
        for (int j = 0; j < ah && freeAll; ++j)
            for (int i = 0; i < aw && freeAll; ++i)
                if (occ[(size_t)(y + j) * (size_t)cols + (size_t)(x + i)]) freeAll = false;
        if (freeAll) { *ox = x; *oy = y; return true; }
    }
    return false;
}
static void MarkGroupSlot(std::vector<char>& occ, int cols, int rows,
                          int x, int y, int aw, int ah) {
    for (int j = 0; j < ah; ++j)
        for (int i = 0; i < aw; ++i)
            if (x + i >= 0 && x + i < cols && y + j >= 0 && y + j < rows)
                occ[(size_t)(y + j) * (size_t)cols + (size_t)(x + i)] = 1;
}

static void NormalizeGroups() {
    const int cols = GroupColumns() * 2, rows = GroupRows() * 2;
    int last = -1, group = -1, cursor = 0;
    std::vector<char> occ((size_t)std::max(1, cols * rows), 0);
    std::vector<std::wstring> oldNames = g_groupNames, newNames;
    for (int k = 0; k < g_orderCount; ++k) {
        int i = g_order[k], source = g_tileGroup[i];
        int aw = TileCellsW(g_tiles[i].cw), ah = TileCellsH(g_tiles[i].cw);
        if (k == 0 || source != last) {
            ++group; cursor = 0;
            std::fill(occ.begin(), occ.end(), (char)0);
            newNames.push_back(source >= 0 && source < (int)oldNames.size() ? oldNames[source] : std::wstring());
        }
        last = source;
        int x = 0, y = 0;
        if (!FindGroupSlot(occ, cols, rows, aw, ah, cursor, &x, &y)) {
            ++group; cursor = 0;
            std::fill(occ.begin(), occ.end(), (char)0);
            newNames.push_back(std::wstring());
            if (!FindGroupSlot(occ, cols, rows, aw, ah, 0, &x, &y)) { x = 0; y = 0; }
        }
        g_tileGroup[i] = group;
        MarkGroupSlot(occ, cols, rows, x, y, aw, ah);
        cursor = y * cols + (x + aw);
    }
    g_groupCount = group + 1;
    g_groupNames.swap(newNames);
    if ((int)g_groupNames.size() < g_groupCount) g_groupNames.resize(g_groupCount);
}

static void LayoutSlots(const int* cws, const int* groups, int n, float* xs, float* ys) {
    const int cols = GroupColumns() * 2, rows = GroupRows() * 2, pitch = GroupPitch();
    const int step = CellStep();
    int block = 0, cursor = 0, last = n ? groups[0] : 0;
    std::vector<char> occ((size_t)std::max(1, cols * rows), 0);
    for (int k = 0; k < n; ++k) {
        if (groups[k] != last) { ++block; cursor = 0; std::fill(occ.begin(), occ.end(), (char)0); last = groups[k]; }
        int aw = TileCellsW(cws[k]), ah = TileCellsH(cws[k]);
        int x = 0, y = 0;
        if (!FindGroupSlot(occ, cols, rows, aw, ah, cursor, &x, &y)) {
            ++block; cursor = 0;
            std::fill(occ.begin(), occ.end(), (char)0);
            if (!FindGroupSlot(occ, cols, rows, aw, ah, 0, &x, &y)) { x = 0; y = 0; }
        }
        xs[k] = (float)(LEFT + block * pitch + x * step);
        ys[k] = (float)(TOP + y * step);
        MarkGroupSlot(occ, cols, rows, x, y, aw, ah);
        cursor = y * cols + (x + aw);
    }
}

static void Relayout(bool animate) {
    NormalizeGroups();
    int cws[MAX_TILES], groups[MAX_TILES]; float xs[MAX_TILES], ys[MAX_TILES];
    for (int k = 0; k < g_orderCount; ++k) {
        int i = g_order[k]; cws[k] = g_tiles[i].cw; groups[k] = g_tileGroup[i];
    }
    LayoutSlots(cws, groups, g_orderCount, xs, ys);
    for (int k = 0; k < g_orderCount; ++k) {
        Tile& t = g_tiles[g_order[k]];
        t.tx = xs[k]; t.ty = ys[k];
        if (!animate || (t.px == 0.0f && t.py == 0.0f)) { t.px = t.tx; t.py = t.ty; }
    }
    ComputePivot();
}

static void ClampScroll() {
    float m = (g_view == VIEW_SEARCH) ? MaxScrollSearch() : MaxScroll();
    if (g_scrollTarget > m) g_scrollTarget = m;
    if (g_scrollTarget < 0.0f) g_scrollTarget = 0.0f;
}

static int OrderPos(int tile) {
    for (int k = 0; k < g_orderCount; ++k) if (g_order[k] == tile) return k;
    return -1;
}

static bool IsPackagedAppTarget(const wchar_t* target) {
    const wchar_t* prefix = L"shell:AppsFolder\\";
    if (!target || _wcsnicmp(target, prefix, wcslen(prefix)) != 0) return false;
    const wchar_t* id = target + wcslen(prefix);
    const wchar_t* bang = wcschr(id, L'!');
    return bang && bang != id && bang[1] != 0;
}

static bool LayoutPath(std::wstring& out) {
    return StorageFilePath(L"tiles", out);
}

static void SaveLayout() {
    for (int i = 0; i < g_tileCount; ++i) g_committedGroup[i] = g_tileGroup[i];
    std::wstring path;
    if (!LayoutPath(path)) return;
    std::wstring text = L"#Win8StartScreen tiles v2 (group column)\n";
    for (int k = 0; k < g_orderCount; ++k) {
        const Tile& t = g_tiles[g_order[k]];
        text += std::to_wstring(t.kind) + L"\t" + (t.label ? t.label : L"") + L"\t" + (t.target ? t.target : L"") + L"\t" +
                std::to_wstring((unsigned)t.c1) + L"\t" + std::to_wstring((unsigned)t.c2) + L"\t" +
                std::to_wstring(t.cw) + L"\t" + std::to_wstring((int)t.style) + L"\t" + std::to_wstring(t.iconIdx) + L"\t" +
                (t.iconFile ? t.iconFile : L"") + L"\t" + std::to_wstring(g_tileGroup[g_order[k]]) + L"\n";
    }
    int n = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
    if (n <= 0) return;
    std::string utf8((size_t)n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &utf8[0], n, NULL, NULL);
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD written = 0;
    WriteFile(f, utf8.data(), (DWORD)utf8.size(), &written, NULL);
    CloseHandle(f);
}

static std::vector<std::wstring> SplitTabs(const std::wstring& line) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        size_t tab = line.find(L'\t', start);
        if (tab == std::wstring::npos) { parts.push_back(line.substr(start)); break; }
        parts.push_back(line.substr(start, tab - start));
        start = tab + 1;
    }
    return parts;
}

static bool BuiltinTargetMatch(const wchar_t* target, const wchar_t* builtin) {
    if (!target || !builtin) return false;
    if (_wcsicmp(target, builtin) == 0) return true;
    size_t n = wcslen(builtin), m = wcslen(target);
    if (m > n) {
        const wchar_t* tail = target + (m - n);
        if (_wcsicmp(tail, builtin) == 0 && (tail[-1] == L'\\' || tail[-1] == L'/')) return true;
    }
    return wcsstr(target, builtin) != NULL;
}
static bool LoadLayout() {
    std::wstring path;
    if (!LayoutPath(path)) return false;
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(f, NULL);
    if (size == INVALID_FILE_SIZE || size == 0 || size > (4u << 20)) { CloseHandle(f); return false; }
    std::string utf8(size, '\0');
    DWORD got = 0;
    BOOL ok = ReadFile(f, &utf8[0], size, &got, NULL);
    CloseHandle(f);
    if (!ok || !got) return false;
    int n = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)got, NULL, 0);
    if (n <= 0) return false;
    std::wstring text((size_t)n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)got, &text[0], n);

    int count = 0;
    size_t pos = 0;
    while (pos < text.size() && count < MAX_TILES) {
        size_t eol = text.find(L'\n', pos);
        std::wstring line = text.substr(pos, eol == std::wstring::npos ? std::wstring::npos : eol - pos);
        pos = (eol == std::wstring::npos) ? text.size() : eol + 1;
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (line.empty() || line[0] == L'#') continue;
        std::vector<std::wstring> p = SplitTabs(line);
        if (p.size() < 9) continue;
        int cw = _wtoi(p[5].c_str()), style = _wtoi(p[6].c_str());
        if (cw < 0 || cw > 3 || style < 0 || style > (int)PINNED_APP) continue;
        Tile t = {};
        t.kind = _wtoi(p[0].c_str());
        t.label = DupW(p[1].c_str());
        t.target = DupW(p[2].c_str());
        t.c1 = (COLORREF)wcstoul(p[3].c_str(), NULL, 10);
        t.c2 = (COLORREF)wcstoul(p[4].c_str(), NULL, 10);
        t.cw = cw;
        t.style = (TileStyle)style;
        t.iconIdx = _wtoi(p[7].c_str());
        t.iconFile = DupW(p[8].c_str());
        g_tileGroup[count] = p.size() >= 10 ? std::max(0, std::min(MAX_TILES - 1, _wtoi(p[9].c_str()))) : -1;
        if (t.kind == 0 && t.style == DESKTOP_REFERENCE && !t.target)
            t.target = DupW(kDesktopTarget);
        if (t.kind == 1 && t.target) {
            if (_wcsicmp(t.target, kDesktopTarget) == 0) {
                t.style = DESKTOP_REFERENCE;
            } else if (IsPackagedAppTarget(t.target)) {
                t.style = PINNED_APP;
            } else {
                t.style = TOP_APP;
                t.c1 = RGB(78, 35, 120);
                t.c2 = RGB(102, 45, 152);
            }
        }
        if (t.kind == 0 && t.target) {
            struct Builtin { const wchar_t* target; int strId; TileStyle style;
                             COLORREF c1, c2; const wchar_t* iconFile; int iconIdx; };
            const Builtin kB[] = {
                { L"explorer.exe",         STR_TILE_EXPLORER,   TOP_APP,        RGB(78, 35, 120), RGB(102, 45, 152), L"explorer.exe", 0 },
                { L"shell:MyComputerFolder", STR_TILE_COMPUTER, TOP_APP,        RGB(78, 35, 120), RGB(102, 45, 152), L"imageres.dll", 109 },
                { L"control.exe",          STR_TILE_CONTROLPANEL, BOTTOM_CAPTION, RGB(25, 145, 181), RGB(30, 170, 197), NULL, 0 },
                { kDesktopTarget,          STR_DESKTOP,         DESKTOP_REFERENCE, RGB(25, 9, 98), RGB(70, 120, 215), NULL, 0 },
                { L"ms-windows-store:",    STR_TILE_STORE,      STORE_CAPTION,  RGB(155, 116, 0), RGB(181, 146, 0), NULL, 0 },
            };
            for (const Builtin& b : kB) {
                if (!BuiltinTargetMatch(t.target, b.target)) continue;
                free((void*)t.label);
                t.label = DupW(T((StrId)b.strId));
                t.style = b.style; t.c1 = b.c1; t.c2 = b.c2;
                free((void*)t.iconFile);
                t.iconFile = b.iconFile ? DupW(b.iconFile) : NULL;
                t.iconIdx = b.iconIdx;
                break;
            }
        }
        t.owned = true;
        t.alive = true;
        g_tiles[count] = t;
        g_order[count] = count;
        ++count;
    }
    g_tileCount = g_orderCount = count;
    if (count && g_tileGroup[0] < 0) {
        for (int k = 0; k < count; ++k) g_tileGroup[k] = 0;
    }
    return count > 0;
}

static bool VisualElementsAttr(const char* xml, const char* name, char* value, size_t valueCount) {
    if (!xml || !name || !value || !valueCount) return false;
    value[0] = 0;
    const size_t n = strlen(name);
    for (const char* pch = xml; *pch; ++pch) {
        if (_strnicmp(pch, name, n) != 0) continue;
        const char* q = pch + n;
        while (*q == ' ' || *q == '\t' || *q == '\r' || *q == '\n') ++q;
        if (*q != '=') continue;
        ++q;
        while (*q == ' ' || *q == '\t') ++q;
        if (*q != '"' && *q != '\'') continue;
        const char quote = *q++;
        size_t i = 0;
        while (*q && *q != quote && i + 1 < valueCount) value[i++] = *q++;
        value[i] = 0;
        return i > 0;
    }
    return false;
}

static bool ParseVisualElementsColor(const char* v, COLORREF& out) {
    static const struct { const char* name; COLORREF c; } kNamed[] = {
        { "black", RGB(0, 0, 0) }, { "silver", RGB(192, 192, 192) }, { "gray", RGB(128, 128, 128) }, { "white", RGB(255, 255, 255) },
        { "maroon", RGB(128, 0, 0) }, { "red", RGB(255, 0, 0) }, { "purple", RGB(128, 0, 128) }, { "fuchsia", RGB(255, 0, 255) },
        { "green", RGB(0, 128, 0) }, { "lime", RGB(0, 255, 0) }, { "olive", RGB(128, 128, 0) }, { "yellow", RGB(255, 255, 0) },
        { "navy", RGB(0, 0, 128) }, { "blue", RGB(0, 0, 255) }, { "teal", RGB(0, 128, 128) }, { "aqua", RGB(0, 255, 255) },
    };
    for (const auto& e : kNamed) if (_stricmp(v, e.name) == 0) { out = e.c; return true; }
    if (*v == '#') ++v;
    if (strlen(v) != 6) return false;
    unsigned rgb = 0;
    for (int i = 0; i < 6; ++i) {
        const char ch = v[i]; unsigned d;
        if (ch >= '0' && ch <= '9') d = (unsigned)(ch - '0');
        else if (ch >= 'a' && ch <= 'f') d = (unsigned)(ch - 'a') + 10;
        else if (ch >= 'A' && ch <= 'F') d = (unsigned)(ch - 'A') + 10;
        else return false;
        rgb = (rgb << 4) | d;
    }
    out = RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255);
    return true;
}

static void ApplyVisualElementsManifest(Tile& t) {
    if (!t.target || t.style != TOP_APP) return;
    std::wstring exe = t.target;
    const auto endsWithCI = [](const std::wstring& str, const wchar_t* suf) {
        const size_t n = wcslen(suf);
        return str.size() >= n && _wcsicmp(str.c_str() + str.size() - n, suf) == 0;
    };
    if (endsWithCI(exe, L".lnk")) {
        IShellLinkW* link = NULL;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link))) && link) {
            IPersistFile* pf = NULL;
            if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&pf))) && pf) {
                if (SUCCEEDED(pf->Load(t.target, STGM_READ))) {
                    wchar_t path[MAX_PATH] = {};
                    if (SUCCEEDED(link->GetPath(path, MAX_PATH, NULL, SLGP_RAWPATH)) && path[0]) exe = path;
                }
                pf->Release();
            }
            link->Release();
        }
    }
    if (!endsWithCI(exe, L".exe")) return;
    const std::wstring dir = exe.substr(0, exe.find_last_of(L"\\/") + 1);
    const std::wstring stem = exe.substr(0, exe.size() - 4);
    const std::wstring candidates[2] = { stem + L".VisualElementsManifest.xml",
                                         dir + L"VisualElementsManifest.xml" };
    HANDLE hf = INVALID_HANDLE_VALUE;
    for (const std::wstring& cand : candidates) {
        hf = CreateFileW(cand.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hf != INVALID_HANDLE_VALUE) break;
    }
    if (hf == INVALID_HANDLE_VALUE) return;
    const DWORD size = GetFileSize(hf, NULL);
    if (size == INVALID_FILE_SIZE || size > 65536) { CloseHandle(hf); return; }
    std::vector<char> buf((size_t)size + 1, 0);
    DWORD read = 0;
    const BOOL ok = ReadFile(hf, buf.data(), size, &read, NULL);
    CloseHandle(hf);
    if (!ok) return;
    char val[512] = {};
    COLORREF bg = 0;
    const bool hasBg = VisualElementsAttr(buf.data(), "BackgroundColor", val, 512) && ParseVisualElementsColor(val, bg);
    const bool dark = VisualElementsAttr(buf.data(), "ForegroundText", val, 512) && _stricmp(val, "dark") == 0;
    const bool hide = VisualElementsAttr(buf.data(), "ShowNameOnSquare150x150Logo", val, 512) && _stricmp(val, "off") == 0;
    char p150[512] = {}, p70[512] = {};
    const bool has150 = VisualElementsAttr(buf.data(), "Square150x150Logo", p150, 512);
    const bool has70  = VisualElementsAttr(buf.data(), "Square70x70Logo", p70, 512);
    if (has150 != has70) return;
    std::vector<BYTE> art150, art70;
    int w150 = 0, w70 = 0;
    if (has150 && t.veArt.empty()) {
        const auto resolve = [&dir](const char* rel) -> std::wstring {
            wchar_t wr[MAX_PATH] = {};
            MultiByteToWideChar(CP_UTF8, 0, rel, -1, wr, MAX_PATH);
            std::wstring relW = wr;
            for (wchar_t& ch : relW) if (ch == L'/') ch = L'\\';
            std::wstring full = (relW.size() > 1 && relW[1] == L':') ? relW : dir + relW;
            if (GetFileAttributesW(full.c_str()) != INVALID_FILE_ATTRIBUTES) return full;
            const size_t dot = full.find_last_of(L'.');
            if (dot != std::wstring::npos) {
                static const wchar_t* kScales[] = { L".scale-180", L".scale-140", L".scale-100", L".scale-80" };
                for (const wchar_t* q : kScales) {
                    const std::wstring cand = full.substr(0, dot) + q + full.substr(dot);
                    if (GetFileAttributesW(cand.c_str()) != INVALID_FILE_ATTRIBUTES) return cand;
                }
            }
            return full;
        };
        w150 = TilePxW(TS_LARGE);
        w70 = std::max(TilePxW(TS_SMALL), 70);
        const COLORREF clear = hasBg ? bg : t.c1;
        if (!DecodePackageArt(resolve(p150), w150, w150, &art150, clear)) return;
        if (!DecodePackageArt(resolve(p70), w70, w70, &art70, clear)) return;
    }
    if (hasBg) t.c1 = t.c2 = bg;
    if (dark) t.veDarkText = true;
    if (hide) t.veHideName = true;
    if (has150 && t.veArt.empty() && !art150.empty()) {
        t.veArt = std::move(art150); t.veW = t.veH = w150;
        t.veArtSmall = std::move(art70); t.veSmallW = t.veSmallH = w70;
    }
}

static void LoadPinIcon(Tile& t) {
    ApplyVisualElementsManifest(t);
    if (!t.target || t.pinBmp || _wcsicmp(t.target, kDesktopTarget) == 0) return;
    IShellItem* item = NULL;
    if (SUCCEEDED(SHCreateItemFromParsingName(t.target, NULL, IID_PPV_ARGS(&item))) && item) {
        IShellItemImageFactory* factory = NULL;
        if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
            const int want = std::min(256, std::max(64, 2 * ScI(64)));
            SIZE sz = { want, want };
            HBITMAP hbm = NULL;
            if (SUCCEEDED(factory->GetImage(sz, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbm)) && hbm) {
                BYTE* px = NULL; int w = 0, h = 0;
                if (BitmapToPixels(hbm, &px, &w, &h)) {
                    void* bits = NULL;
                    HDC sdc = GetDC(NULL);
                    HBITMAP dib = MakeDib(sdc, w, h, &bits);
                    ReleaseDC(NULL, sdc);
                    if (dib && bits) { memcpy(bits, px, (size_t)w * (size_t)h * 4); t.pinBmp = dib; t.pinW = w; t.pinH = h; }
                    else if (dib) DeleteObject(dib);
                    delete[] px;
                }
                DeleteObject(hbm);
            }
            factory->Release();
        }
        item->Release();
    }
    if (t.kind == 0 && !t.pinBmp && !t.icon && t.target && t.style != DESKTOP_REFERENCE) {
        std::wstring full = t.target;
        if (full.find(L'\\') == std::wstring::npos && full.size() > 4 &&
            _wcsicmp(full.c_str() + full.size() - 4, L".exe") == 0) {
            wchar_t dir[MAX_PATH];
            const bool winFirst = _wcsicmp(full.c_str(), L"explorer.exe") == 0;
            UINT (*first)(wchar_t*, UINT) = winFirst ? GetWindowsDirectoryW : GetSystemDirectoryW;
            UINT (*second)(wchar_t*, UINT) = winFirst ? GetSystemDirectoryW : GetWindowsDirectoryW;
            if (first(dir, MAX_PATH)) {
                std::wstring cand = std::wstring(dir) + L"\\" + full;
                if (GetFileAttributesW(cand.c_str()) != INVALID_FILE_ATTRIBUTES) full = cand;
                else if (second(dir, MAX_PATH)) {
                    cand = std::wstring(dir) + L"\\" + full;
                    if (GetFileAttributesW(cand.c_str()) != INVALID_FILE_ATTRIBUTES) full = cand;
                }
            }
        }
        {
            HICON pi = NULL; UINT pid = 0;
            if (PrivateExtractIconsW(full.c_str(), 0, 256, 256, &pi, &pid, 1, 0) == 1 && pi)
                t.icon = pi;
        }
        SHFILEINFOW info = {};
        if (!t.icon && SHGetFileInfoW(full.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_LARGEICON))
            t.icon = info.hIcon;
        if (!t.icon) {
            HICON sm = NULL, lg = NULL;
            if (ExtractIconExW(full.c_str(), 0, &lg, &sm, 1) > 0) {
                if (sm) DestroyIcon(sm);
                t.icon = lg;
            }
        }
    }
}

static bool IsPinned(const wchar_t* target) {
    if (!target) return false;
    for (int k = 0; k < g_orderCount; ++k) {
        const Tile& t = g_tiles[g_order[k]];
        if (t.target && _wcsicmp(t.target, target) == 0) return true;
    }
    return false;
}

static int FindPinned(const wchar_t* target) {
    for (int k = 0; k < g_orderCount; ++k) {
        const Tile& t = g_tiles[g_order[k]];
        if (t.target && target && _wcsicmp(t.target, target) == 0) return g_order[k];
    }
    return -1;
}

static void PinAppSized(const wchar_t* name, const std::wstring& target, int cw, bool newGroup) {
    if (IsPinned(target.c_str())) return;
    int idx = -1;
    for (int i = 0; i < g_tileCount; ++i) if (!g_tiles[i].alive) { idx = i; break; }
    if (idx < 0) { if (g_tileCount >= MAX_TILES) return; idx = g_tileCount++; }
    Tile t = {};
    t.label = DupW(name);
    t.target = DupW(target.c_str());
    if (_wcsicmp(target.c_str(), kDesktopTarget) == 0) {
        t.style = DESKTOP_REFERENCE;
        t.c1 = RGB(25, 9, 98); t.c2 = RGB(70, 120, 215);
    } else if (IsPackagedAppTarget(target.c_str())) {
        t.style = PINNED_APP;
        t.c1 = t.c2 = RGB(97, 43, 146);
    } else {
        t.style = TOP_APP;
        t.c1 = RGB(78, 35, 120); t.c2 = RGB(102, 45, 152);
    }
    t.cw = cw;
    t.owned = true;
    t.alive = true;
    t.kind = 1;
    g_tiles[idx] = t;
    if (g_tiles[idx].style != DESKTOP_REFERENCE) LoadPinIcon(g_tiles[idx]);
    UpdateTileAppColors();
    g_tileScale[idx] = 1.0f;
    g_tileGroup[idx] = g_orderCount ? g_tileGroup[g_order[g_orderCount - 1]] + (newGroup ? 1 : 0) : 0;
    g_order[g_orderCount++] = idx;
    if (t.style == PINNED_APP) QueuePackageArt(t.target);
    Relayout(true);
    RebuildTile(idx);
    SaveLayout();
    RepaintNow();
}
static void PinApp(const wchar_t* name, const std::wstring& target) {
    PinAppSized(name, target, TS_MEDIUM, false);
}

static void SeedStartLayout() {
    InterlockedExchange(&g_seedPending, 0);
    Wh_SetIntValue(L"layoutSeeded", 1);
    struct Seed { const wchar_t* match[3]; int cw; bool newGroup; };
    static const Seed kSeeds[] = {
        { { L"microsoft.windowslive.mail",     L"microsoft.outlookforwindows", NULL }, TS_WIDE,   true  },
        { { L"microsoft.windowslive.calendar", NULL, NULL },                          TS_WIDE,   false },
        { { L"microsoft.windows.photos",       NULL, NULL },                          TS_MEDIUM, false },
        { { L"microsoft.microsoftedge",        NULL, NULL },                          TS_MEDIUM, false },
        { { L"microsoft.bingweather",          NULL, NULL },                          TS_LARGE,  true  },
        { { L"microsoft.bingnews",             L"microsoft.start", NULL },            TS_WIDE,   false },
        { { L"microsoft.windowscamera",        NULL, NULL },                          TS_SMALL,  true  },
        { { L"microsoft.gamingapp",            L"microsoft.xboxapp", NULL },          TS_SMALL,  false },
        { { L"microsoft.zunemusic",            NULL, NULL },                          TS_SMALL,  false },
        { { L"microsoft.zunevideo",            NULL, NULL },                          TS_SMALL,  false },
        { { L"microsoft.skydrive",             L"onedrive.exe", NULL },               TS_MEDIUM, false },
        { { L"microsoft.windowsmaps",          NULL, NULL },                          TS_MEDIUM, false },
        { { L"windows.immersivecontrolpanel",  NULL, NULL },                          TS_MEDIUM, false },
        { { L"microsoft.windowscalculator",    NULL, NULL },                          TS_SMALL,  true  },
        { { L"microsoft.windowsalarms",        NULL, NULL },                          TS_SMALL,  false },
        { { L"microsoft.windowsnotepad",       NULL, NULL },                          TS_SMALL,  false },
        { { L"microsoft.paint",                NULL, NULL },                          TS_SMALL,  false },
    };
    int added = 0;
    std::vector<int> used;
    for (const Seed& seed : kSeeds) {
        std::wstring name, target;
        {
            SrwGuard guard(&g_appsLock, false);
            for (size_t i = 0; i < g_apps.size() && target.empty(); ++i) {
                const AppEntry& a = g_apps[i];
                if (a.exe || a.searchOnly || a.charmCat != CC_APP) continue;
                bool taken = false;
                for (int u : used) if (u == (int)i) { taken = true; break; }
                if (taken) continue;
                const std::wstring parseLow = LowerW(a.parse);
                for (const wchar_t* pat : seed.match) {
                    if (pat && parseLow.find(pat) != std::wstring::npos) {
                        name = a.name; target = AppTarget(a); used.push_back((int)i);
                        break;
                    }
                }
            }
        }
        if (target.empty() || IsPinned(target.c_str())) continue;
        int cw = seed.cw;
        while (cw > TS_MEDIUM && TileCellsH(cw) > GroupRows() * 2) --cw;
        PinAppSized(name.c_str(), target, cw, seed.newGroup);
        ++added;
    }
    if (added) Wh_Log(L"Seeded the Start layout with %d installed app(s) in Windows 8.1 sizes", added);
}

static void UnpinTile(int idx) {
    if (idx < 0 || idx >= g_tileCount || !g_tiles[idx].alive) return;
    int pos = OrderPos(idx);
    if (pos >= 0) { for (int k = pos; k + 1 < g_orderCount; ++k) g_order[k] = g_order[k + 1]; --g_orderCount; }
    ReleaseTileSurface(idx);
    FreeTile(g_tiles[idx]);
    g_tiles[idx].alive = false;
    if (g_hover == idx) g_hover = -1;
    Relayout(true);
    ClampScroll();
    SaveLayout();
    RepaintNow();
}

static void ResizeTileTo(int idx, int size) {
    if (idx < 0 || idx >= g_tileCount || !g_tiles[idx].alive) return;
    int supported[4];
    int count = SupportedSizes(g_tiles[idx], supported);
    bool allowed = false;
    for (int i = 0; i < count; ++i) if (supported[i] == size) allowed = true;
    if (!allowed || g_tiles[idx].cw == size) return;
    g_tiles[idx].cw = size;
    RebuildTile(idx);
    Relayout(true);
    ClampScroll();
    SaveLayout();
    RepaintNow();
}

static void BeginDrag(int idx, int mx, int my) {
    for (int k = 0; k < g_orderCount; ++k) g_dragStartOrder[k] = g_order[k];
    for (int i = 0; i < g_tileCount; ++i) g_dragStartGroups[i] = g_tileGroup[i];
    Tile& t = g_tiles[idx];
    g_dragging = true;
    g_dragIdx = idx;
    g_grabX = (float)mx + g_scrollCur - t.px;
    g_grabY = (float)my - t.py;
    g_pressed = -1;
    HideBar();
}

static int NearbyGroup(float cx, float cy) {
    if (cy < TOP - 55.0f || cy > TOP + GroupHeight() + 55.0f) return -1;
    int nearest = -1;
    float distance = 1e30f;
    for (int b = 0; b < g_groupCount; ++b) {
        float left = (float)(LEFT + b * GroupPitch());
        float right = left + GroupWidth();
        float dx = cx < left ? left - cx : (cx > right ? cx - right : 0.0f);
        if (dx < 43.0f && dx < distance) { distance = dx; nearest = b; }
    }
    return nearest;
}

static void UpdateDrag(int mx, int my) {
    if (g_dragIdx < 0) return;
    Tile& d = g_tiles[g_dragIdx];
    d.px = (float)mx + g_scrollCur - g_grabX;
    d.py = (float)my - g_grabY;
    float w = (float)TilePxW(d.cw), th = (float)TilePxH(d.cw);
    float cx = d.px + w * 0.5f, cy = d.py + th * 0.5f;
    int destination = NearbyGroup(cx, cy);
    int cur = OrderPos(g_dragIdx), n = g_orderCount;
    if (destination >= 0 && cur >= 0 && n > 1) {
        int others[MAX_TILES], on = 0;
        for (int k = 0; k < n; ++k) if (k != cur) others[on++] = g_order[k];
        float bestD = 1e30f;
        int bestJ = -1;
        for (int j = 0; j < n; ++j) {
            if ((j > 0 && g_tileGroup[others[j - 1]] > destination) ||
                (j < on && g_tileGroup[others[j]] < destination)) continue;
            int cws[MAX_TILES], groups[MAX_TILES], m = 0;
            for (int k = 0; k < n; ++k) {
                int i = k == j ? g_dragIdx : others[k < j ? k : k - 1];
                cws[m] = g_tiles[i].cw;
                groups[m++] = k == j ? destination : g_tileGroup[i];
            }
            float xs[MAX_TILES], ys[MAX_TILES];
            LayoutSlots(cws, groups, n, xs, ys);
            float dx = xs[j] + w * 0.5f - cx, dy = ys[j] + th * 0.5f - cy;
            float dist = dx * dx + dy * dy;
            if (dist < bestD) { bestD = dist; bestJ = j; }
        }
        float oldDx = d.tx + w * 0.5f - cx;
        float oldDy = d.ty + th * 0.5f - cy;
        float oldD = oldDx * oldDx + oldDy * oldDy;
        if (bestJ >= 0 && (bestJ != cur || g_tileGroup[g_dragIdx] != destination) &&
            (bestD + 1600.0f < oldD || g_tileGroup[g_dragIdx] != destination)) {
            for (int k = cur; k + 1 < n; ++k) g_order[k] = g_order[k + 1];
            for (int k = n - 1; k > bestJ; --k) g_order[k] = g_order[k - 1];
            g_order[bestJ] = g_dragIdx;
            g_tileGroup[g_dragIdx] = destination;
            Relayout(true);
        }
    }
    if (mx < 80) g_scrollTarget -= 18.0f;
    else if (mx > g_sw - 80) g_scrollTarget += 18.0f;
    ClampScroll();
    RepaintNow();
}

static void SyncDragToCursor(HWND window) {
    if (!g_dragging || g_dragIdx < 0 || !g_open || g_closing || g_view != VIEW_START) return;
    POINT pt;
    if (!GetCursorPos(&pt) || !ScreenToClient(window, &pt)) return;
    const Tile& tile = g_tiles[g_dragIdx];
    float wantX = (float)pt.x + g_scrollCur - g_grabX;
    float wantY = (float)pt.y - g_grabY;
    bool atEdge = pt.x < 80 || pt.x > g_sw - 80;
    if (atEdge || fabsf(wantX - tile.px) >= 0.25f || fabsf(wantY - tile.py) >= 0.25f)
        UpdateDrag(pt.x, pt.y);
}

static void EndDrag() {
    if (g_dragIdx < 0) return;
    const Tile& tile = g_tiles[g_dragIdx];
    float cx = tile.px + (float)TilePxW(tile.cw) * 0.5f;
    float cy = tile.py + (float)TilePxH(tile.cw) * 0.5f;
    float lastRight = (float)(LEFT + (g_groupCount - 1) * GroupPitch() + GroupWidth());
    if (NearbyGroup(cx, cy) < 0 && cx > lastRight + 43.0f &&
        cy >= TOP - 55.0f && cy <= TOP + GroupHeight() + 55.0f) {
        for (int k = 0; k < g_orderCount; ++k) g_order[k] = g_dragStartOrder[k];
        for (int i = 0; i < g_tileCount; ++i) g_tileGroup[i] = g_dragStartGroups[i];
        int cur = OrderPos(g_dragIdx);
        if (cur >= 0) {
            for (int k = cur; k + 1 < g_orderCount; ++k) g_order[k] = g_order[k + 1];
            g_order[g_orderCount - 1] = g_dragIdx;
            g_tileGroup[g_dragIdx] = g_groupCount;
            Relayout(true);
        }
    }
    g_dragging = false;
    g_dragIdx = -1;
    ClampScroll();
    SaveLayout();
    RepaintNow();
}

struct BarButton { int id; const wchar_t* glyph; const wchar_t* label; };
struct AdvancedAction { int id; const wchar_t* label; };
static int kBarH = 100, kBarBtnW = 100;

static void CreateUiFonts();
static void RecreateFonts();
static void UpdateDpiMetrics() {
    TILE = ScI(150); GAP = ScI(10); LEFT = ScI(120); TOP = ScI(168);
    GROUP_SEPARATION = ScI(40);
    TILE_PIVOT_Y = (float)TOP + 2.875f * (float)TILE;
    TILE_SLIDE = ScF(110.0f);
    LOGON_TILE_SLIDE = ScF(260.0f);
    SB_W = ScI(346); SB_PAD = ScI(40);
    kBarH = ScI(100); kBarBtnW = ScI(100);
}
static bool UpdateDpi(int forcedDpi = 0) {
    int dpi = forcedDpi;
    if (!dpi && g_wnd) dpi = (int)GetDpiForWindow(g_wnd);
    if (!dpi) dpi = (int)GetDpiForSystem();
    if (!dpi) { HDC dc = GetDC(NULL); if (dc) { dpi = GetDeviceCaps(dc, LOGPIXELSY); ReleaseDC(NULL, dc); } }
    if (dpi < 96) dpi = 96; if (dpi > 480) dpi = 480;
    if (dpi == g_dpi) return false;
    g_dpi = dpi; g_dpiScale = (float)dpi / 96.0f;
    UpdateDpiMetrics();
    RecreateFonts();
    ReleaseAvatarPicture();
    ReleaseUiIcons();
    return true;
}
static const int kAdvW = 256, kAdvRowH = 42, kAdvPad = 12;

static std::wstring ResultTarget(int resultIdx) {
    if (resultIdx < 0 || resultIdx >= (int)g_results.size()) return L"";
    return AppTarget(g_apps[g_results[resultIdx]]);
}

struct SelectedItem {
    std::wstring label, target;
    bool computer, classic;
};

static SelectedItem CurrentSelection() {
    SelectedItem item = {};
    if (g_barMode == 1 && g_barIdx >= 0 && g_barIdx < g_tileCount && g_tiles[g_barIdx].alive) {
        const Tile& t = g_tiles[g_barIdx];
        item.label = t.label ? t.label : L"";
        item.target = t.target ? t.target : L"";
        item.computer = _wcsicmp(item.target.c_str(), L"shell:MyComputerFolder") == 0;
        item.classic = t.style == TOP_APP && !item.computer;
    } else if (g_barMode == 2 && g_barIdx >= 0 && g_barIdx < (int)g_results.size()) {
        const AppEntry& a = g_apps[g_results[g_barIdx]];
        item.label = a.name;
        item.target = ResultTarget(g_barIdx);
        item.computer = _wcsicmp(item.target.c_str(), L"shell:MyComputerFolder") == 0 ||
                        _wcsicmp(a.name.c_str(), L"Computer") == 0;
        item.classic = !a.desktop && !item.computer && !IsPackagedAppTarget(item.target.c_str());
    }
    if (item.target == kDesktopTarget) item.classic = false;
    return item;
}

static int AdvancedActions(AdvancedAction* out) {
    SelectedItem item = CurrentSelection();
    if (item.computer) {
        out[0] = { 20, T(STR_ADV_MANAGE) };
        out[1] = { 21, T(STR_ADV_MAP) };
        out[2] = { 22, T(STR_ADV_DISCONNECT) };
        out[3] = { 23, T(STR_ADV_PROPERTIES) };
        return 4;
    }
    if (item.classic) {
        out[0] = { 24, T(STR_ADV_RUNAS) };
        out[1] = { 25, T(STR_ADV_OPENFILE) };
        return 2;
    }
    return 0;
}

static int BarButtons(BarButton* out) {
    if (g_barMode == 5) {
        out[0] = { 26, L"\xE13E", T(STR_BAR_NAME_GROUP) };
        return 1;
    }
    if (g_barMode != 1 && g_barMode != 2) return 0;
    SelectedItem item = CurrentSelection();
    if (item.target.empty()) return 0;
    int n = 0;
    AdvancedAction adv[4];
    if (AdvancedActions(adv)) out[n++] = { 10, L"\xE10C", T(STR_BAR_ADVANCED) };
    out[n++] = { 12, L"\xE102", T(STR_BAR_RUN) };
    if (g_barMode == 1) {
        const Tile& tile = g_tiles[g_barIdx];
        if (SupportsResize(tile))
            out[n++] = { 2, L"\xE1D9", T(STR_SIZE_RESIZE) };
        out[n++] = { 1, L"\xE196", T(STR_BAR_UNPIN) };
    }
    else if (IsPinned(item.target.c_str())) out[n++] = { 3, L"\xE196", T(STR_BAR_UNPIN) };
    else out[n++] = { 4, L"\xE141", T(STR_BAR_PIN_START) };
    return n;
}

static void ShowBar(int mode, int idx) {
    g_barVisible = true; g_barMode = mode; g_barIdx = idx; g_barTick = NowMs();
    g_advancedOpen = false; g_resizeOpen = false;
    g_barHoverButton = g_barPressedButton = g_advHoverRow = g_advPressedRow = -1;
    g_resizeHoverRow = g_resizePressedRow = -1;
    RepaintNow();
}

static void HideBar() {
    if (!g_barVisible) return;
    g_barVisible = false; g_barMode = 0; g_barIdx = -1; g_advancedOpen = false; g_resizeOpen = false;
    g_barHoverButton = g_barPressedButton = g_advHoverRow = g_advPressedRow = -1;
    g_resizeHoverRow = g_resizePressedRow = -1;
    RepaintNow();
}

static std::wstring ExistingFile(const std::wstring& path) {
    if (path.empty()) return L"";
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY) ? path : L"";
}

static std::wstring FindStartShortcut(const std::wstring& dir, const std::wstring& name, int depth) {
    if (depth > 6) return L"";
    WIN32_FIND_DATAW fd = {};
    HANDLE find = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return L"";
    std::wstring found;
    do {
        if (fd.cFileName[0] == L'.' && (fd.cFileName[1] == 0 ||
            (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0))) continue;
        std::wstring path = dir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                found = FindStartShortcut(path, name, depth + 1);
        } else {
            std::wstring filename = fd.cFileName;
            if (filename.size() == name.size() + 4 &&
                _wcsicmp(filename.c_str() + filename.size() - 4, L".lnk") == 0 &&
                _wcsnicmp(filename.c_str(), name.c_str(), name.size()) == 0) found = path;
        }
        if (!found.empty()) break;
    } while (FindNextFileW(find, &fd));
    FindClose(find);
    return found;
}

static std::wstring ItemFile(const SelectedItem& item) {
    if (item.target.empty()) return L"";
    std::wstring path = ExistingFile(item.target);
    if (!path.empty()) return path;
    const wchar_t* prefix = L"shell:AppsFolder\\";
    if (_wcsnicmp(item.target.c_str(), prefix, wcslen(prefix)) == 0) {
        path = ExistingFile(item.target.substr(wcslen(prefix)));
        if (!path.empty()) return path;
    }
    IShellItem* sh = NULL;
    if (SUCCEEDED(SHCreateItemFromParsingName(item.target.c_str(), NULL, IID_PPV_ARGS(&sh))) && sh) {
        PWSTR file = NULL;
        if (SUCCEEDED(sh->GetDisplayName(SIGDN_FILESYSPATH, &file)) && file) {
            path = ExistingFile(file);
            CoTaskMemFree(file);
        }
        sh->Release();
        if (!path.empty()) return path;
    }
    for (int folder : { CSIDL_PROGRAMS, CSIDL_COMMON_PROGRAMS }) {
        wchar_t root[MAX_PATH] = L"";
        if (SUCCEEDED(SHGetFolderPathW(NULL, folder, NULL, 0, root))) {
            path = FindStartShortcut(root, item.label, 0);
            if (!path.empty()) return path;
        }
    }
    const wchar_t* exe = NULL;
    if (_wcsicmp(item.label.c_str(), L"Disk Cleanup") == 0) exe = L"cleanmgr.exe";
    else if (_wcsicmp(item.label.c_str(), L"Math Input Panel") == 0) exe = L"mip.exe";
    else if (_wcsicmp(item.label.c_str(), L"Private Character Editor") == 0) exe = L"eudcedit.exe";
    else if (_wcsicmp(item.label.c_str(), L"Windows Explorer") == 0) exe = L"explorer.exe";
    else if (_wcsicmp(item.label.c_str(), L"Control Panel") == 0) exe = L"control.exe";
    if (exe) {
        wchar_t buf[MAX_PATH] = L"";
        if (SearchPathW(NULL, exe, NULL, MAX_PATH, buf, NULL)) return ExistingFile(buf);
    }
    return L"";
}

static void ShellActionError(const wchar_t* message) {
    Wh_Log(L"Shell action failed: %s", message);
    // Modal loop on the UI thread keeps dispatching Start's messages and ends on WM_QUIT at uninit.
    MessageBoxW(NULL, message, T(STR_START), MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);
}

static bool ShortcutCommand(const std::wstring& file, std::wstring* executable, std::wstring* args) {
    if (file.size() < 4 || _wcsicmp(file.c_str() + file.size() - 4, L".lnk") != 0) return false;
    IShellLinkW* link = NULL;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link))) || !link)
        return false;
    IPersistFile* persist = NULL;
    bool ok = false;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&persist))) && persist) {
        if (SUCCEEDED(persist->Load(file.c_str(), STGM_READ))) {
            wchar_t exe[MAX_PATH] = L"", param[2048] = L"";
            WIN32_FIND_DATAW data = {};
            if (SUCCEEDED(link->GetPath(exe, MAX_PATH, &data, 0)) && exe[0]) {
                link->GetArguments(param, 2048);
                *executable = exe; *args = param; ok = true;
            }
        }
        persist->Release();
    }
    link->Release();
    return ok;
}

static void ExecuteAdvanced(int id) {
    SelectedItem item = CurrentSelection();
    std::wstring file = ItemFile(item);
    HideBar();
    CloseStart();
    switch (id) {
    case 20:
        if ((INT_PTR)ShellExecuteW(NULL, L"open", L"compmgmt.msc", NULL, NULL, SW_SHOWNORMAL) <= 32)
            ShellActionError(T(ERR_COMPMGMT));
        break;
    case 21:
        if ((INT_PTR)ShellExecuteW(NULL, L"open", L"rundll32.exe", L"shell32.dll,SHHelpShortcuts_RunDLL Connect", NULL, SW_SHOWNORMAL) <= 32)
            ShellActionError(T(ERR_MAPDRIVE));
        break;
    case 22:
        if ((INT_PTR)ShellExecuteW(NULL, L"open", L"rundll32.exe", L"shell32.dll,SHHelpShortcuts_RunDLL Disconnect", NULL, SW_SHOWNORMAL) <= 32)
            ShellActionError(T(ERR_DISCONNECTDRIVE));
        break;
    case 23:
        if ((INT_PTR)ShellExecuteW(NULL, L"open", L"ms-settings:about", NULL, NULL, SW_SHOWNORMAL) <= 32)
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.System", NULL, SW_SHOWNORMAL);
        break;
    case 24: {
        std::wstring exe, args;
        const wchar_t* launch = file.empty() ? item.target.c_str() : file.c_str();
        if (ShortcutCommand(file, &exe, &args)) launch = exe.c_str();
        if ((INT_PTR)ShellExecuteW(NULL, L"runas", launch, args.empty() ? NULL : args.c_str(), NULL, SW_SHOWNORMAL) <= 32)
            ShellActionError(T(ERR_RUNAS));
        break;
    }
    case 25: {
        if (file.empty()) {
            ShellActionError(T(ERR_NOFILELOC));
            break;
        }
        std::wstring args = L"/select,\"" + file + L"\"";
        ShellExecuteW(NULL, L"open", L"explorer.exe", args.c_str(), NULL, SW_SHOWNORMAL);
        break;
    }
    }
}

static void ExecuteBar(int id) {
    if (id == 10) { g_advancedOpen = !g_advancedOpen; g_resizeOpen = false; RepaintNow(); return; }
    if (id == 2) { g_resizeOpen = !g_resizeOpen; g_advancedOpen = false; RepaintNow(); return; }
    int idx = g_barIdx, mode = g_barMode;
    SelectedItem item = CurrentSelection();
    HideBar();
    switch (id) {
    case 1: UnpinTile(idx); break;
    case 3: UnpinTile(FindPinned(item.target.c_str())); break;
    case 4:
        if (idx >= 0 && idx < (int)g_results.size()) PinApp(g_apps[g_results[idx]].name.c_str(), item.target);
        break;
    case 26: StartNaming(idx); break;
    case 12:
        if (mode == 1) Launch(idx);
        else if (idx >= 0 && idx < (int)g_results.size()) LaunchResult(idx);
        break;
    }
}

static int BarResizeSizes(int out[4]) {
    if (g_barMode != 1 || g_barIdx < 0 || g_barIdx >= g_tileCount || !g_tiles[g_barIdx].alive) return 0;
    return SupportedSizes(g_tiles[g_barIdx], out);
}

static void AdvancedRect(int count, const wchar_t* const* labels, int* left, int* top, int* right, int* bottom) {
    int w = kAdvW;
    if (labels && count > 0 && g_writeFactory && g_flyFormat) {
        static const wchar_t* cFirst = NULL, *cLast = NULL;
        static int cCount = -1, cSW = -1, cW = kAdvW;
        static float cScale = -1.0f;
        if (labels[0] == cFirst && labels[count - 1] == cLast && count == cCount &&
            g_sw == cSW && ScF(1.0f) == cScale) {
            w = cW;
        } else {
            float need = 0.0f;
            for (int i = 0; i < count; ++i) {
                if (!labels[i]) continue;
                IDWriteTextLayout* lay = NULL;
                if (SUCCEEDED(g_writeFactory->CreateTextLayout(labels[i], (UINT32)wcslen(labels[i]),
                              g_flyFormat, 10000.0f, 100.0f, &lay)) && lay) {
                    DWRITE_TEXT_METRICS m = {};
                    if (SUCCEEDED(lay->GetMetrics(&m))) need = std::max(need, m.widthIncludingTrailingWhitespace);
                    lay->Release();
                }
            }
            if (need > 0.0f) w = std::max(w, (int)(need + 34 + 16 + 0.5f));
            w = std::min(w, std::max(kAdvW, g_sw - 40));
            cFirst = labels[0]; cLast = labels[count - 1]; cCount = count; cSW = g_sw; cScale = ScF(1.0f); cW = w;
        }
    }
    *right = g_sw - 20;
    *left = *right - w;
    *bottom = g_sh - kBarH - ScI(8);
    *top = *bottom - (count * kAdvRowH + kAdvPad * 2);
}

static bool BarPoint(int x, int y) {
    if (!g_barVisible) return false;
    AdvancedAction actions[4];
    int ac = AdvancedActions(actions);
    int l, t, r, b;
    if (g_advancedOpen && ac) {
        const wchar_t* labs[4]; for (int i = 0; i < ac; ++i) labs[i] = actions[i].label;
        AdvancedRect(ac, labs, &l, &t, &r, &b);
        if (x >= l && x < r && y >= t && y < g_sh - kBarH) return true;
    }
    int sizes[4];
    int sc = BarResizeSizes(sizes);
    if (g_resizeOpen && sc) {
        const wchar_t* labs[4]; for (int i = 0; i < sc; ++i) labs[i] = SizeName(sizes[i]);
        AdvancedRect(sc, labs, &l, &t, &r, &b);
        if (x >= l && x < r && y >= t && y < g_sh - kBarH) return true;
    }
    return y >= g_sh - kBarH && y < g_sh;
}

static void UpdateBarHover(int x, int y) {
    if (!g_barVisible) return;
    BarButton buttons[4];
    int n = BarButtons(buttons);
    int hovered = -1, advanced = -1, row = -1, resizeBtn = -1, rrow = -1;
    for (int i = 0; i < n; ++i) {
        if (buttons[i].id == 10) advanced = i;
        if (buttons[i].id == 2) resizeBtn = i;
    }
    if (y >= g_sh - kBarH && y < g_sh && x >= g_sw - n * kBarBtnW && x < g_sw)
        hovered = (x - (g_sw - n * kBarBtnW)) / kBarBtnW;
    if (g_advancedOpen && advanced >= 0) {
        AdvancedAction actions[4];
        int ac = AdvancedActions(actions), l, t, r, b;
        if (ac) {
            const wchar_t* labs[4]; for (int i = 0; i < ac; ++i) labs[i] = actions[i].label;
            AdvancedRect(ac, labs, &l, &t, &r, &b);
            if (x >= l && x < r && y >= t + kAdvPad && y < b - kAdvPad)
                row = (y - t - kAdvPad) / kAdvRowH;
        }
    }
    if (g_resizeOpen && resizeBtn >= 0) {
        int sizes[4];
        int sc = BarResizeSizes(sizes), l, t, r, b;
        if (sc) {
            const wchar_t* labs[4]; for (int i = 0; i < sc; ++i) labs[i] = SizeName(sizes[i]);
            AdvancedRect(sc, labs, &l, &t, &r, &b);
            if (x >= l && x < r && y >= t + kAdvPad && y < b - kAdvPad)
                rrow = (y - t - kAdvPad) / kAdvRowH;
        }
    }
    if (hovered != g_barHoverButton || row != g_advHoverRow || rrow != g_resizeHoverRow) {
        g_barHoverButton = hovered;
        g_advHoverRow = row;
        g_resizeHoverRow = rrow;
        RepaintNow();
    }
}

static bool BarMouseDown(int x, int y) {
    if (!BarPoint(x, y)) return false;
    UpdateBarHover(x, y);
    g_barPressedButton = g_barHoverButton;
    g_advPressedRow = g_advHoverRow;
    g_resizePressedRow = g_resizeHoverRow;
    RepaintNow();
    return true;
}

static bool BarClick(int x, int y) {
    if (!g_barVisible) return false;
    UpdateBarHover(x, y);
    AdvancedAction actions[4];
    int ac = AdvancedActions(actions);
    if (g_advancedOpen && ac && g_advHoverRow >= 0 && g_advHoverRow < ac) {
        ExecuteAdvanced(actions[g_advHoverRow].id);
        return true;
    }
    int sizes[4];
    int sc = BarResizeSizes(sizes);
    if (g_resizeOpen && sc && g_resizeHoverRow >= 0 && g_resizeHoverRow < sc) {
        int idx = g_barIdx, size = sizes[g_resizeHoverRow];
        HideBar();
        ResizeTileTo(idx, size);
        return true;
    }
    if (y >= g_sh - kBarH) {
        BarButton buttons[4];
        int n = BarButtons(buttons);
        int start = g_sw - n * kBarBtnW;
        if (n && x >= start && x < g_sw) {
            int which = (x - start) / kBarBtnW;
            ExecuteBar(buttons[which].id);
        } else HideBar();
        return true;
    }
    HideBar();
    return true;
}


static void OnRightClick(int x, int y) {
    if (g_namingGroup >= 0 && !g_customize) CommitNaming();
    if (g_view == VIEW_START && !g_charmOpen && !g_customize) {
        RECT pw, sc, ar; HeaderButtonRects(&pw, &sc); AccountRect(&ar);
        POINT pt = { x, y };
        if (PtInRect(&sc, pt)) { EnterSearchEmpty(); return; }
        if (PtInRect(&pw, pt)) { ShowMenuFor(g_wnd, pw, 1); return; }
        if (PtInRect(&ar, pt)) { ShowUserMenu(g_wnd); return; }
    }
    if (g_view == VIEW_SEARCH) {
        int r = HitResult(x, y);
        if (r >= 0) { g_selected = r; if (g_barVisible && g_barMode == 2 && g_barIdx == r) HideBar(); else ShowBar(2, r); }
        else HideBar();
        return;
    }
    int tile = HitTile(x, y);
    if (tile >= 0) {
        HideBar();
        g_ctxTile = tile;
        g_barMode = 1; g_barIdx = tile;
        RECT an = { x, y, x, y };
        ShowMenuFor(g_wnd, an, 5);
        if (!g_barVisible) { g_barMode = 0; g_barIdx = -1; }
        g_ctxTile = -1;
    } else if (g_view == VIEW_START && !g_charmOpen && !g_zoomOut) {
        HideBar();
        if (g_customize) { ExitCustomize(); return; }
        RECT an = { x, y, x, y };
        ShowMenuFor(g_wnd, an, 7);
    } else HideBar();
}

static void AppsSearchBoxRect(RECT* r) {
    r->left = g_sw - ScI(360); r->top = ScI(52);
    r->right = g_sw - ScI(40); r->bottom = ScI(88);
}
static void DrawAppsSearchBoxGpu() {
    RECT b; AppsSearchBoxRect(&b);
    bool lit = g_appsSearchFocus || g_queryLen > 0;
    FillRc((float)b.left, (float)b.top, (float)b.right, (float)b.bottom,
           RGB(255, 255, 255), lit ? 1.0f : 0.25f);
    std::wstring q(g_query, (size_t)g_queryLen);
    if (!q.empty())
        TextAt(q.c_str(), g_queryFormat, (float)b.left + ScI(10), (float)b.top,
               (float)b.right - ScI(40), (float)b.bottom, RGB(0, 0, 0));
    TextAt(q.empty() ? L"" : L"", g_glyphSmall,
           (float)b.right - ScI(36), (float)b.top, (float)b.right, (float)b.bottom,
           lit ? RGB(90, 90, 90) : RGB(220, 220, 230));
    if (g_appsSearchFocus && ((NowMs() / 500) & 1)) {
        int tw = 0;
        HDC dc = GetDC(NULL);
        if (dc && g_fLabel) {
            HGDIOBJ old = SelectObject(dc, g_fLabel);
            SIZE sz = {};
            if (GetTextExtentPoint32W(dc, q.c_str(), (int)q.size(), &sz)) tw = sz.cx;
            SelectObject(dc, old);
        }
        if (dc) ReleaseDC(NULL, dc);
        float cx = (float)b.left + ScI(10) + (float)tw;
        FillRc(cx, (float)b.top + ScI(6), cx + 1.0f, (float)b.bottom - ScI(6), RGB(0, 0, 0), 1.0f);
    }
}
static void DrawAppsSearchBoxGdi() {
    RECT b; AppsSearchBoxRect(&b);
    bool lit = g_appsSearchFocus || g_queryLen > 0;
    if (lit) FillRect(g_bbDC, &b, (HBRUSH)GetStockObject(WHITE_BRUSH));
    else BlendFillGdi(g_bbDC, b.left, b.top, b.right, b.bottom, RGB(255, 255, 255), 64);
    SetBkMode(g_bbDC, TRANSPARENT);
    HGDIOBJ of = SelectObject(g_bbDC, g_fLabel);
    std::wstring q(g_query, (size_t)g_queryLen);
    if (!q.empty()) {
        SetTextColor(g_bbDC, RGB(0, 0, 0));
        RECT tr = { b.left + ScI(10), b.top, b.right - ScI(40), b.bottom };
        DrawTextW(g_bbDC, q.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
    SetTextColor(g_bbDC, lit ? RGB(90, 90, 90) : RGB(220, 220, 230));
    SelectObject(g_bbDC, g_fHdrGlyph);
    RECT gr = { b.right - ScI(36), b.top, b.right, b.bottom };
    DrawTextW(g_bbDC, q.empty() ? L"" : L"", -1, &gr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    if (g_appsSearchFocus && ((NowMs() / 500) & 1)) {
        SelectObject(g_bbDC, g_fLabel);
        SIZE sz = {}; int tw = 0;
        if (GetTextExtentPoint32W(g_bbDC, q.c_str(), (int)q.size(), &sz)) tw = sz.cx;
        RECT cr = { b.left + ScI(10) + tw, b.top + ScI(6), b.left + ScI(11) + tw, b.bottom - ScI(6) };
        FillRect(g_bbDC, &cr, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }
    SelectObject(g_bbDC, of);
}
static void DrawAppsChromeGdi(HDC) {
    if (!g_open || g_closing || !EnsureUiIcons()) return;
    RECT ar; AppsArrowRect(&ar);
    const BYTE aa = (BYTE)((g_hoverArrow ? 255 : 42) * ChromeIdleFade(NowMs()));
    if (g_view == VIEW_START && ZoomPresence(NowMs()) <= 0.0f) {
        BlitIconGdi(g_bbDC, g_iconArrowDown, ar, g_hoverArrow, aa);
    } else if (g_queryLen == 0) {
        BlitIconGdi(g_bbDC, g_iconArrowUp, ar, g_hoverArrow, aa);
        RECT sr; SortButtonRect(&sr);
        wchar_t txt[128];
        const int sort = (int)InterlockedCompareExchange(&g_appSort, 0, 0);
        swprintf_s(txt, L"%s \x25BE",
                   T(sort == 1 ? STR_SORTBY_DATE : sort == 2 ? STR_SORTBY_CAT : STR_SORTBY_NAME));
        SetTextColor(g_bbDC, RGB(235, 235, 245));
        SetBkMode(g_bbDC, TRANSPARENT);
        SelectObject(g_bbDC, g_fLabel);
        DrawTextW(g_bbDC, txt, -1, &sr, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);
    }
    if (g_view == VIEW_SEARCH && !g_charmOpen) DrawAppsSearchBoxGdi();
}
static void DrawAppsChromeGpu() {
    if (!g_scratch) return;
    RECT ar; AppsArrowRect(&ar);
    auto arrow = [&](bool down) {
        const UiIcon& ic = down ? g_iconArrowDown : g_iconArrowUp;
        if (ic.gpu) {
            float op = (g_hoverArrow ? 1.0f : 0.16f) * ChromeIdleFade(NowMs());
            g_whiteGpu->SetOpacity(op);
            g_rt->DrawBitmap(ic.gpu, D2D1::RectF((float)ar.left, (float)ar.top,
                                                 (float)ar.right, (float)ar.bottom),
                             op,
                             D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    };
    if (!EnsureUiIcons()) return;
    if (g_view == VIEW_START) { if (ZoomPresence(NowMs()) <= 0.0f) arrow(true); }
    else if (g_queryLen == 0) {
        arrow(false);
        RECT sr; SortButtonRect(&sr);
        wchar_t txt[128];
        const int sort = (int)InterlockedCompareExchange(&g_appSort, 0, 0);
        swprintf_s(txt, L"%s \x25BE",
                   T(sort == 1 ? STR_SORTBY_DATE : sort == 2 ? STR_SORTBY_CAT : STR_SORTBY_NAME));
        TextAt(txt, g_queryFormat, (float)sr.left, (float)sr.top, (float)sr.right, (float)sr.bottom,
               RGB(235, 235, 245));
    }
    if (g_view == VIEW_SEARCH && !g_charmOpen) DrawAppsSearchBoxGpu();
}
static void DrawBarLabelFit(const wchar_t* label, float x, float top) {
    try {
    const float maxW = (float)kBarBtnW - ScF(6.0f);
    HDC mdc = GetDC(NULL);
    HGDIOBJ oldM = NULL;
    int oneW = 0;
    if (mdc) {
        if (g_fBarMeas) oldM = SelectObject(mdc, g_fBarMeas);
        SIZE sz = {};
        if (GetTextExtentPoint32W(mdc, label, (int)wcslen(label), &sz)) oneW = sz.cx;
    }
    if ((float)oneW <= maxW || !mdc) {
        if (mdc) { if (oldM) SelectObject(mdc, oldM); ReleaseDC(NULL, mdc); }
        TextAt(label, g_barLabel, x, top + 57, x + kBarBtnW, top + 88, RGB(255, 255, 255));
        return;
    }
    std::wstring l1, l2;
    auto widthOf = [&](const std::wstring& s) {
        SIZE sz = {};
        if (GetTextExtentPoint32W(mdc, s.c_str(), (int)s.size(), &sz)) return (int)sz.cx;
        return 0;
    };
    const wchar_t* p = label;
    while (*p) {
        const wchar_t* sp = wcschr(p, L' ');
        std::wstring word = sp ? std::wstring(p, (size_t)(sp - p)) : std::wstring(p);
        std::wstring tryL1 = l1.empty() ? word : l1 + L" " + word;
        if (widthOf(tryL1) <= (int)maxW || l1.empty()) l1 = tryL1;
        else l2 = l2.empty() ? word : l2 + L" " + word;
        if (!sp) break;
        p = sp + 1;
    }
    if (!l2.empty()) {
        while (widthOf(l2 + L"\x2026") > (int)maxW && l2.size() > 1) l2.resize(l2.size() - 1);
        l2 += L"\x2026";
    }
    if (oldM) SelectObject(mdc, oldM);
    ReleaseDC(NULL, mdc);
    TextAt(l1.c_str(), g_barLabel, x, top + 53, x + kBarBtnW, top + 71, RGB(255, 255, 255));
    if (!l2.empty())
        TextAt(l2.c_str(), g_barLabel, x, top + 71, x + kBarBtnW, top + 88, RGB(255, 255, 255));
    } catch (...) { Wh_Log(L"[bar] label fitting failed; label skipped"); }
}

static void DrawBarGpu(DWORD now) {
    if (!g_barVisible || !g_scratch || !g_barLabel) return;
    BarButton buttons[4];
    int n = BarButtons(buttons);
    if (!n) return;
    float t = Clamp01((float)(now - g_barTick) / 1000.0f / 0.16f);
    float off = (1.0f - EaseOut(t)) * (float)kBarH;
    float top = (float)(g_sh - kBarH) + off;
    FillRc(0, top, (float)g_sw, (float)g_sh + off, RGB(78, 39, 117), 1.0f);
    FillRc(0, top, (float)g_sw, top + 1.0f, RGB(130, 87, 164), 0.85f);
    float start = (float)(g_sw - n * kBarBtnW);
    for (int i = 0; i < n; ++i) {
        float x = start + i * kBarBtnW;
        bool down = g_barPressedButton == i && g_barHoverButton == i;
        bool active = down || (buttons[i].id == 10 && g_advancedOpen) || (buttons[i].id == 2 && g_resizeOpen);
        COLORREF mark = active ? RGB(76, 35, 110) : RGB(255, 255, 255);
        if (buttons[i].id == 10 || buttons[i].id == 1 || buttons[i].id == 3 || buttons[i].id == 2) {
            SetBr(RGB(255, 255, 255), 1.0f);
            D2D1_ELLIPSE circle = D2D1::Ellipse(D2D1::Point2F(x + 50, top + 34), 19, 19);
            if (active) g_rt->FillEllipse(circle, g_scratch);
            else g_rt->DrawEllipse(circle, g_scratch, 2.5f);
            if (buttons[i].id == 10) {
                SetBr(mark, 1.0f);
                for (int d = -1; d <= 1; ++d)
                    g_rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x + 50 + d * 10, top + 34), 2.1f, 2.1f), g_scratch);
            } else if (buttons[i].id == 2) {
                SetBr(mark, 1.0f);
                g_rt->DrawRectangle(D2D1::RectF(x + 39, top + 23, x + 61, top + 45), g_scratch, 2.2f);
                g_rt->FillRectangle(D2D1::RectF(x + 44, top + 28, x + 52, top + 36), g_scratch);
            } else {
                TextAt(buttons[i].glyph, g_glyphSmall, x + 32, top + 16, x + 68, top + 52, mark);
            }
        } else {
            TextAt(buttons[i].glyph, g_glyphFormat, x, top + 11, x + kBarBtnW, top + 56,
                   RGB(255, 255, 255));
        }
        DrawBarLabelFit(buttons[i].label, x, top);
    }
    AdvancedAction actions[4];
    int count = AdvancedActions(actions);
    if (g_advancedOpen && count && off < 1.0f) {
        int l, topMenu, r, bottomMenu;
        const wchar_t* labs[4]; for (int i = 0; i < count; ++i) labs[i] = actions[i].label;
        AdvancedRect(count, labs, &l, &topMenu, &r, &bottomMenu);
        FillRc((float)l + 4, (float)topMenu + 4, (float)r + 4, (float)bottomMenu + 4, RGB(9, 2, 13), 0.45f);
        FillRc((float)l, (float)topMenu, (float)r, (float)bottomMenu, RGB(250, 250, 250));
        for (int i = 0; i < count; ++i) {
            float y = (float)(topMenu + kAdvPad + i * kAdvRowH);
            if (i == g_advHoverRow) {
                COLORREF shade = (i == g_advPressedRow) ? RGB(255, 255, 255) : RGB(224, 224, 224);
                FillRc((float)l + 19, y + 1, (float)r - 19, y + kAdvRowH - 1, shade);
            }
            TextAt(actions[i].label, g_flyFormat ? g_flyFormat : g_itemFormat, (float)l + 34, y + 3, (float)r - 7, y + kAdvRowH - 2,
                   RGB(28, 28, 28));
        }
    }
    int sizes[4];
    int scount = BarResizeSizes(sizes);
    if (g_resizeOpen && scount && off < 1.0f) {
        int l, topMenu, r, bottomMenu;
        const wchar_t* labs[4]; for (int i = 0; i < scount; ++i) labs[i] = SizeName(sizes[i]);
        AdvancedRect(scount, labs, &l, &topMenu, &r, &bottomMenu);
        FillRc((float)l + 4, (float)topMenu + 4, (float)r + 4, (float)bottomMenu + 4, RGB(9, 2, 13), 0.45f);
        FillRc((float)l, (float)topMenu, (float)r, (float)bottomMenu, RGB(250, 250, 250));
        for (int i = 0; i < scount; ++i) {
            float y = (float)(topMenu + kAdvPad + i * kAdvRowH);
            if (i == g_resizeHoverRow) {
                COLORREF shade = (i == g_resizePressedRow) ? RGB(255, 255, 255) : RGB(224, 224, 224);
                FillRc((float)l + 19, y + 1, (float)r - 19, y + kAdvRowH - 1, shade);
            }
            if (sizes[i] == g_tiles[g_barIdx].cw)
                TextAt(L"\xE10B", g_glyphSmall, (float)l + 10, y + 8, (float)l + 34, y + kAdvRowH - 6,
                       RGB(28, 28, 28));
            TextAt(SizeName(sizes[i]), g_flyFormat ? g_flyFormat : g_itemFormat, (float)l + 34, y + 3, (float)r - 7, y + kAdvRowH - 2,
                   RGB(28, 28, 28));
        }
    }
}

enum UserMenuCmd { UM_NONE = 0, UM_PICTURE, UM_LOCK, UM_SIGNOUT,
                   UM_SLEEP = 10, UM_HIBERNATE, UM_SHUTDOWN, UM_RESTART,
                   UM_SORT_NAME = 20, UM_SORT_DATE, UM_SORT_CAT,
                   UM_NAMEGROUPS = 30 };
enum MenuEndReason {
    MENU_END_NONE = 0,
    MENU_END_INSIDE,
    MENU_END_OUTSIDE,
    MENU_END_CANCEL
};
struct UserMenuEntry { std::wstring text; int cmd; bool checked = false; bool arrow = false;
                       bool sep = false; int sub = 0; };
static const wchar_t* kUserMenuClass = L"Win8StartScreenMod8045UserMenu";
static HWND g_userMenuWnd = NULL;
static bool g_userMenuClassRegistered = false;
static std::vector<UserMenuEntry> g_userMenu;
static int g_userMenuHover = -1;
static HWND g_subMenuWnd = NULL;
static std::vector<UserMenuEntry> g_subMenu;
static int g_subMenuHover = -1, g_subParentRow = -1;
static const UINT_PTR kSubOpenTimer = 0xA1, kSubCloseTimer = 0xA2;
static volatile DWORD g_menuClosedTick = 0;
static bool g_swallowMenuUp = false;
static int g_userMenuResult = UM_NONE;
static int g_userMenuEndReason = MENU_END_NONE;
static bool g_inUserMenu = false;

static COLORREF SystemAccentColor() {
    DWORD c = 0; BOOL o = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
        return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    return RGB(0, 116, 203);
}

static HICON g_stockAppIcon = NULL;
[[clang::no_destroy]] static std::optional<
    std::unordered_map<HICON, std::unique_ptr<Gdiplus::Bitmap>>>
    g_iconBitmapCache{std::in_place};

static std::unique_ptr<Gdiplus::Bitmap> CreateGdiplusBitmapFromIcon(HICON icon) {
    ICONINFO iconInfo{};
    if (!icon || !GetIconInfo(icon, &iconInfo)) return {};
    GdiObj<HBITMAP> color(iconInfo.hbmColor);
    GdiObj<HBITMAP> mask(iconInfo.hbmMask);
    if (!color.get()) return {};
    BITMAP sourceInfo{};
    if (GetObjectW(color.get(), sizeof(sourceInfo), &sourceInfo) != sizeof(sourceInfo) ||
        sourceInfo.bmWidth <= 0 || sourceInfo.bmHeight <= 0 ||
        sourceInfo.bmWidth > 512 || sourceInfo.bmHeight > 512) return {};
    const int width = sourceInfo.bmWidth;
    const int height = sourceInfo.bmHeight;
    const size_t pixelBytes = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    std::vector<BYTE> pixels(pixelBytes);
    BITMAPINFO colorInfo{};
    colorInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    colorInfo.bmiHeader.biWidth = width;
    colorInfo.bmiHeader.biHeight = -height;
    colorInfo.bmiHeader.biPlanes = 1;
    colorInfo.bmiHeader.biBitCount = 32;
    colorInfo.bmiHeader.biCompression = BI_RGB;
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return {};
    const int colorLines = GetDIBits(screenDc, color.get(), 0, static_cast<UINT>(height),
                                     pixels.data(), &colorInfo, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screenDc);
    if (colorLines != height) return {};
    bool hasAlpha = false, hasPartialAlpha = false, alphaAllOpaque = true;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        const BYTE a = pixels[i];
        hasAlpha = hasAlpha || a != 0;
        hasPartialAlpha = hasPartialAlpha || (a != 0 && a != 255);
        alphaAllOpaque = alphaAllOpaque && a == 255;
    }
    std::vector<BYTE> maskBits;
    size_t maskStride = ((static_cast<size_t>(width) + 31u) / 32u) * 4u;
    bool hasTransparentMaskPixel = false;
    if (mask.get()) {
        maskBits.resize(maskStride * static_cast<size_t>(height));
        struct MaskBitmapInfo { BITMAPINFOHEADER header; RGBQUAD colors[2]; } maskInfo{};
        maskInfo.header.biSize = sizeof(BITMAPINFOHEADER);
        maskInfo.header.biWidth = width;
        maskInfo.header.biHeight = -height;
        maskInfo.header.biPlanes = 1;
        maskInfo.header.biBitCount = 1;
        maskInfo.header.biCompression = BI_RGB;
        maskInfo.colors[0] = RGBQUAD{0, 0, 0, 0};
        maskInfo.colors[1] = RGBQUAD{255, 255, 255, 0};
        screenDc = GetDC(nullptr);
        if (screenDc) {
            const int maskLines = GetDIBits(screenDc, mask.get(), 0, static_cast<UINT>(height),
                                            maskBits.data(), reinterpret_cast<BITMAPINFO*>(&maskInfo), DIB_RGB_COLORS);
            ReleaseDC(nullptr, screenDc);
            if (maskLines == height) {
                for (int y = 0; y < height && !hasTransparentMaskPixel; ++y) {
                    const BYTE* row = maskBits.data() + static_cast<size_t>(y) * maskStride;
                    for (int x = 0; x < width; ++x) {
                        if ((row[x / 8] & (0x80u >> (x % 8))) != 0) { hasTransparentMaskPixel = true; break; }
                    }
                }
            } else maskBits.clear();
        } else maskBits.clear();
    }
    if (!hasAlpha && maskBits.empty()) return {};
    const bool useMask = !hasPartialAlpha &&
                         ((!hasAlpha && !maskBits.empty()) ||
                          (alphaAllOpaque && hasTransparentMaskPixel));
    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppARGB);
    if (bitmap->GetLastStatus() != Gdiplus::Ok) return {};
    Gdiplus::Rect bounds(0, 0, width, height);
    Gdiplus::BitmapData dst{};
    if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &dst) != Gdiplus::Ok) return {};
    for (int y = 0; y < height; ++y) {
        const BYTE* srow = pixels.data() + static_cast<size_t>(y) * static_cast<size_t>(width) * 4;
        BYTE* out = static_cast<BYTE*>(dst.Scan0) + static_cast<ptrdiff_t>(y) * dst.Stride;
        const BYTE* maskRow = maskBits.empty() ? nullptr : maskBits.data() + static_cast<size_t>(y) * maskStride;
        for (int x = 0; x < width; ++x) {
            BYTE alpha = srow[x * 4 + 3];
            if (useMask && maskRow) alpha = (maskRow[x / 8] & (0x80u >> (x % 8))) ? 0 : 255;
            else if (!hasAlpha && !maskRow) alpha = 255;
            out[x * 4 + 0] = alpha ? srow[x * 4 + 0] : 0;
            out[x * 4 + 1] = alpha ? srow[x * 4 + 1] : 0;
            out[x * 4 + 2] = alpha ? srow[x * 4 + 2] : 0;
            out[x * 4 + 3] = alpha;
        }
    }
    bitmap->UnlockBits(&dst);
    return bitmap;
}

static COLORREF GetIconAccentColor(HICON icon, COLORREF fallback = RGB(0, 120, 215), bool* found = NULL) {
    if (found) *found = false;
    try {
        auto bitmap = CreateGdiplusBitmapFromIcon(icon);
        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) return fallback;
        const int width = static_cast<int>(bitmap->GetWidth());
        const int height = static_cast<int>(bitmap->GetHeight());
        if (width <= 0 || height <= 0) return fallback;
        Gdiplus::Rect bounds(0, 0, width, height);
        Gdiplus::BitmapData data{};
        if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok) return fallback;
        double weight[24]{};
        double sumR[24]{}, sumG[24]{}, sumB[24]{};
        const int stepX = std::max(1, width / 32);
        const int stepY = std::max(1, height / 32);
        for (int y = 0; y < height; y += stepY) {
            const BYTE* row = static_cast<const BYTE*>(data.Scan0) + static_cast<ptrdiff_t>(y) * data.Stride;
            for (int x = 0; x < width; x += stepX) {
                const BYTE* px = row + static_cast<ptrdiff_t>(x) * 4;
                const double b = px[0] / 255.0, g = px[1] / 255.0, r = px[2] / 255.0;
                const double alpha = px[3] / 255.0;
                if (alpha < 0.38) continue;
                const double hi = std::max(r, std::max(g, b));
                const double lo = std::min(r, std::min(g, b));
                const double delta = hi - lo;
                if (hi < 0.18 || delta < 0.12) continue;
                double hue = 0.0;
                if (hi == r) hue = 60.0 * std::fmod((g - b) / delta, 6.0);
                else if (hi == g) hue = 60.0 * ((b - r) / delta + 2.0);
                else hue = 60.0 * ((r - g) / delta + 4.0);
                if (hue < 0.0) hue += 360.0;
                const int bin = std::min(23, static_cast<int>(hue / 15.0));
                const double w = alpha * (delta / std::max(0.01, hi));
                weight[bin] += w; sumR[bin] += r * w; sumG[bin] += g * w; sumB[bin] += b * w;
            }
        }
        bitmap->UnlockBits(&data);
        int best = 0;
        for (int i = 1; i < 24; ++i) if (weight[i] > weight[best]) best = i;
        if (weight[best] < 0.5) return fallback;
        if (found) *found = true;
        const auto toByte = [](double value) {
            return static_cast<BYTE>((std::max)(0.0, (std::min)(255.0, value * 255.0)));
        };
        return RGB(toByte(sumR[best] / weight[best]),
                   toByte(sumG[best] / weight[best]),
                   toByte(sumB[best] / weight[best]));
    } catch (...) {
        return fallback;
    }
}

static COLORREF AccentFromPremulBgra(const BYTE* px, int w, int h, COLORREF fallback, bool* found) {
    if (found) *found = false;
    if (!px || w <= 0 || h <= 0) return fallback;
    double weight[24]{};
    double sumR[24]{}, sumG[24]{}, sumB[24]{};
    const int stepX = std::max(1, w / 32), stepY = std::max(1, h / 32);
    for (int y = 0; y < h; y += stepY) for (int x = 0; x < w; x += stepX) {
        const BYTE* q = px + ((size_t)y * (size_t)w + (size_t)x) * 4;
        const double alpha = q[3] / 255.0;
        if (alpha < 0.38) continue;
        const double r = std::min(1.0, (q[2] / 255.0) / alpha);
        const double g = std::min(1.0, (q[1] / 255.0) / alpha);
        const double b = std::min(1.0, (q[0] / 255.0) / alpha);
        const double hi = std::max(r, std::max(g, b));
        const double lo = std::min(r, std::min(g, b));
        const double delta = hi - lo;
        if (hi < 0.18 || delta < 0.12) continue;
        double hue = 0.0;
        if (hi == r) hue = 60.0 * std::fmod((g - b) / delta, 6.0);
        else if (hi == g) hue = 60.0 * ((b - r) / delta + 2.0);
        else hue = 60.0 * ((r - g) / delta + 4.0);
        if (hue < 0.0) hue += 360.0;
        const int bin = std::min(23, static_cast<int>(hue / 15.0));
        const double wt = alpha * (delta / std::max(0.01, hi));
        weight[bin] += wt; sumR[bin] += r * wt; sumG[bin] += g * wt; sumB[bin] += b * wt;
    }
    int best = 0;
    for (int i = 1; i < 24; ++i) if (weight[i] > weight[best]) best = i;
    if (weight[best] < 0.5) return fallback;
    if (found) *found = true;
    const auto toByte = [](double value) {
        return static_cast<BYTE>((std::max)(0.0, (std::min)(255.0, value * 255.0)));
    };
    return RGB(toByte(sumR[best] / weight[best]),
               toByte(sumG[best] / weight[best]),
               toByte(sumB[best] / weight[best]));
}

static HICON JumboIconFromSysIndex(int iIcon) {
    if (iIcon < 0) return NULL;
    IImageList* iml = NULL;
    if (FAILED(SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(&iml))) || !iml) return NULL;
    HICON ico = NULL;
    iml->GetIcon(iIcon, ILD_TRANSPARENT, &ico);
    iml->Release();
    return ico;
}

static HICON ShellFolderIconHiRes(const wchar_t* parsingName) {
    PIDLIST_ABSOLUTE pidl = NULL;
    if (FAILED(SHParseDisplayName(parsingName, NULL, &pidl, 0, NULL)) || !pidl) return NULL;
    HICON ico = NULL;
    SHFILEINFOW info = {};
    if (SHGetFileInfoW((LPCWSTR)pidl, 0, &info, sizeof(info), SHGFI_PIDL | SHGFI_SYSICONINDEX))
        ico = JumboIconFromSysIndex(info.iIcon);
    if (!ico) {
        info = {};
        if (SHGetFileInfoW((LPCWSTR)pidl, 0, &info, sizeof(info),
                           SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON))
            ico = info.hIcon;
    }
    CoTaskMemFree(pidl);
    return ico;
}

static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size) {
    if (!dc || size <= 0) return;
    RECT clip{}, iconRect{x, y, x + size, y + size}, visible{};
    if (GetClipBox(dc, &clip) == ERROR || !IntersectRect(&visible, &clip, &iconRect)) return;
    if (!icon && !g_stockAppIcon) {
        SHSTOCKICONINFO stock{}; stock.cbSize = sizeof(stock);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_SYSICONINDEX, &stock)))
            g_stockAppIcon = JumboIconFromSysIndex(stock.iSysImageIndex);
        if (!g_stockAppIcon && SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON | SHGSI_LARGEICON, &stock)))
            g_stockAppIcon = stock.hIcon;
    }
    if (!icon) icon = g_stockAppIcon;
    if (!icon) return;
    if (g_gdipToken && g_iconBitmapCache) {
        auto it = g_iconBitmapCache->find(icon);
        if (it == g_iconBitmapCache->end()) {
            auto bitmap = CreateGdiplusBitmapFromIcon(icon);
            if (bitmap) it = g_iconBitmapCache->emplace(icon, std::move(bitmap)).first;
        }
        if (it != g_iconBitmapCache->end()) {
            Gdiplus::Graphics graphics(dc);
            if (graphics.GetLastStatus() == Gdiplus::Ok) {
                graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
                graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                if (graphics.DrawImage(it->second.get(), x, y, size, size) == Gdiplus::Ok)
                    return;
            }
        }
    }
    DrawIconEx(dc, x, y, icon, size, size, 0, nullptr, DI_NORMAL);
}

static int MenuItemHeight() { return ScI(34); }
static int MenuPadY()       { return ScI(6); }
static int MenuPadX()       { return ScI(14); }

static int MenuRowH(const UserMenuEntry& e) { return e.sep ? ScI(9) : MenuItemHeight(); }
static int MenuContentH(const std::vector<UserMenuEntry>& v) {
    int s = MenuPadY() * 2;
    for (const UserMenuEntry& e : v) s += MenuRowH(e);
    return s;
}
static int MenuRowTop(const std::vector<UserMenuEntry>& v, int idx) {
    int y = MenuPadY();
    for (int i = 0; i < idx && i < (int)v.size(); ++i) y += MenuRowH(v[i]);
    return y;
}
static const std::vector<UserMenuEntry>& MenuEntries(HWND h) {
    return (h && h == g_subMenuWnd) ? g_subMenu : g_userMenu;
}
static int& MenuHoverRef(HWND h) {
    return (h && h == g_subMenuWnd) ? g_subMenuHover : g_userMenuHover;
}

static int MenuWidthOf(const std::vector<UserMenuEntry>& v) {
    int maxW = ScI(220);
    ScopedWindowDc dc(NULL);
    if (dc.get()) {
        GdiObj<HFONT> font(CreateFontW(-ScI(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                       DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI"));
        ScopedSelectedObject selFont(dc.get(), font.get());
        for (const UserMenuEntry& e : v) {
            if (e.text.empty()) continue;
            SIZE sz = {};
            if (GetTextExtentPoint32W(dc.get(), e.text.c_str(), (int)e.text.size(), &sz))
                maxW = std::max(maxW, (int)sz.cx + MenuPadX() * 2 + ScI(18));
        }
    }
    return std::min(ScI(520), maxW);
}
static int MenuWidth() { return MenuWidthOf(g_userMenu); }

static bool MenuEntrySelectable(const std::vector<UserMenuEntry>& v, int i) {
    return i >= 0 && i < (int)v.size() && !v[i].sep && !v[i].text.empty();
}

static int MenuStepIn(const std::vector<UserMenuEntry>& v, int from, int dir) {
    const int n = (int)v.size();
    if (n == 0) return -1;
    int i = (from < 0 || from >= n) ? (dir > 0 ? -1 : n) : from;
    for (int k = 0; k < n; k++) {
        i += dir;
        if (i < 0) i = n - 1;
        if (i >= n) i = 0;
        if (MenuEntrySelectable(v, i)) return i;
    }
    return -1;
}

static int MenuHitTest(HWND h, int x, int y) {
    RECT rc; GetClientRect(h, &rc);
    if (x < 0 || x >= rc.right) return -1;
    const std::vector<UserMenuEntry>& v = MenuEntries(h);
    int top = MenuPadY();
    for (int i = 0; i < (int)v.size(); ++i) {
        int hh = MenuRowH(v[i]);
        if (y >= top && y < top + hh) return MenuEntrySelectable(v, i) ? i : -1;
        top += hh;
    }
    return -1;
}

static void BuildMenuEntries(int kind, std::vector<UserMenuEntry>& out);
static HWND CreateUserMenuWindow(HWND owner, int x, int y, int w, int hgt);

// Menus
static void CloseSubMenu() {
    if (g_userMenuWnd) { KillTimer(g_userMenuWnd, kSubOpenTimer); KillTimer(g_userMenuWnd, kSubCloseTimer); }
    if (g_subMenuWnd) { HWND s2 = g_subMenuWnd; g_subMenuWnd = NULL; DestroyWindow(s2); }
    g_subParentRow = -1;
    g_subMenuHover = -1;
}

static void OpenSubMenu(int parentRow) {
    if (!g_userMenuWnd || parentRow < 0 || parentRow >= (int)g_userMenu.size()) return;
    const int kind = g_userMenu[parentRow].sub;
    if (!kind) return;
    if (g_subMenuWnd && g_subParentRow == parentRow) return;
    CloseSubMenu();
    BuildMenuEntries(kind, g_subMenu);
    if (g_subMenu.empty()) return;
    const int w = MenuWidthOf(g_subMenu), hgt = MenuContentH(g_subMenu);
    RECT pr; GetWindowRect(g_userMenuWnd, &pr);
    int x = pr.right - ScI(3);
    int y = pr.top + MenuRowTop(g_userMenu, parentRow) - MenuPadY();
    RECT work = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    POINT anchorPt = { pr.left, pr.top };
    HMONITOR mon = MonitorFromPoint(anchorPt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (mon && GetMonitorInfoW(mon, &mi)) work = mi.rcWork;
    if (x + w > work.right) x = pr.left - w + ScI(3);
    x = std::max((int)work.left, std::min(x, (int)work.right - w));
    y = std::max((int)work.top, std::min(y, (int)work.bottom - hgt));
    HWND sub = CreateUserMenuWindow(g_userMenuWnd, x, y, w, hgt);
    if (!sub) return;
    g_subMenuWnd = sub;
    g_subParentRow = parentRow;
    g_subMenuHover = -1;
    SetWindowPos(sub, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
    InvalidateRect(g_userMenuWnd, NULL, FALSE);
}

static void MenuSetHover(HWND h, int idx) {
    int& hov = MenuHoverRef(h);
    if (idx == hov) return;
    hov = idx;
    InvalidateRect(h, NULL, FALSE);
    if (h == g_userMenuWnd) {
        const bool wantsSub = idx >= 0 && idx < (int)g_userMenu.size() && g_userMenu[idx].sub != 0;
        KillTimer(h, kSubCloseTimer);
        if (wantsSub) {
            if (!(g_subMenuWnd && g_subParentRow == idx)) SetTimer(h, kSubOpenTimer, 220, NULL);
        } else {
            KillTimer(h, kSubOpenTimer);
            if (g_subMenuWnd) SetTimer(h, kSubCloseTimer, 350, NULL);
        }
    } else if (h == g_subMenuWnd && g_userMenuWnd) {
        KillTimer(g_userMenuWnd, kSubCloseTimer);
    }
}

static void MenuFinish(HWND h, int reason, int entry) {
    if (g_userMenuEndReason == MENU_END_NONE) {
        g_userMenuEndReason = reason;
        g_userMenuResult = UM_NONE;
        const std::vector<UserMenuEntry>& v = MenuEntries(h);
        if (entry >= 0 && entry < (int)v.size())
            g_userMenuResult = v[entry].cmd;
    }
    if (g_subMenuWnd) PostMessageW(g_subMenuWnd, WM_CLOSE, 0, 0);
    if (g_userMenuWnd) PostMessageW(g_userMenuWnd, WM_CLOSE, 0, 0);
    else if (h) PostMessageW(h, WM_CLOSE, 0, 0);
}

static void MenuDeactivated(HWND h, HWND to) {
    if (to && to == h) return;
    if (!to) return;
    if (to == g_subMenuWnd || to == g_userMenuWnd) return;
    MenuFinish(h, to == g_wnd ? MENU_END_INSIDE : MENU_END_OUTSIDE, -1);
}

static void MenuPaint(HWND h) {
    ScopedPaint dc(h);
    if (!dc.get()) return;
    RECT rc; GetClientRect(h, &rc);

    GdiObj<HBRUSH> bg(CreateSolidBrush(RGB(255, 255, 255)));
    FillRect(dc.get(), &rc, bg.get());

    GdiObj<HPEN> border(CreatePen(PS_SOLID, 1, RGB(200, 200, 200)));
    {
        ScopedSelectedObject selPen(dc.get(), border.get());
        ScopedSelectedObject selBrush(dc.get(), GetStockObject(NULL_BRUSH));
        Rectangle(dc.get(), rc.left, rc.top, rc.right, rc.bottom);
    }

    GdiObj<HFONT> font(CreateFontW(-ScI(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                   DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI"));
    ScopedSelectedObject selFont(dc.get(), font.get());
    ScopedBkMode bkMode(dc.get(), TRANSPARENT);

    int padY = MenuPadY();
    int padX = MenuPadX();
    COLORREF accent = SystemAccentColor();

    const std::vector<UserMenuEntry>& entries = MenuEntries(h);
    const int hoverIdx = MenuHoverRef(h);
    bool anyChecked = false;
    for (const UserMenuEntry& e : entries) if (e.checked) anyChecked = true;
    int rowY = padY;
    for (size_t i = 0; i < entries.size(); ++i) {
        const int rowH = MenuRowH(entries[i]);
        RECT r = { 1, rowY, rc.right - 1, rowY + rowH };
        rowY += rowH;
        if (entries[i].sep) {
            RECT lr = { ScI(2), r.top + rowH / 2, rc.right - ScI(2), r.top + rowH / 2 + 1 };
            GdiObj<HBRUSH> lb(CreateSolidBrush(RGB(214, 214, 214)));
            FillRect(dc.get(), &lr, lb.get());
            continue;
        }
        const bool hover = ((int)i == hoverIdx) ||
            (entries[i].sub != 0 && h == g_userMenuWnd && g_subParentRow == (int)i && g_subMenuWnd);
        if (hover) {
            GdiObj<HBRUSH> hb(CreateSolidBrush(accent));
            FillRect(dc.get(), &r, hb.get());
        }
        ScopedTextColor tc(dc.get(), hover ? RGB(255, 255, 255) : RGB(32, 32, 32));
        if (entries[i].checked) {
            RECT cr = r; cr.left += ScI(3); cr.right = cr.left + ScI(20);
            DrawTextW(dc.get(), L"\x2713", -1, &cr, DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_NOPREFIX);
        }
        if (entries[i].arrow) {
            RECT ar = r; ar.right -= ScI(8);
            DrawTextW(dc.get(), L"\x203A", -1, &ar, DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX);
        }
        RECT tr = r;
        tr.left += padX + (anyChecked ? ScI(17) : 0);
        tr.right -= padX;
        SIZE tsz = {};
        GetTextExtentPoint32W(dc.get(), entries[i].text.c_str(),
                              (int)entries[i].text.size(), &tsz);
        const UINT fit = (tsz.cx <= tr.right - tr.left)
            ? (DT_SINGLELINE | DT_VCENTER)
            : (DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS);
        DrawTextW(dc.get(), entries[i].text.c_str(), -1, &tr, fit | DT_NOPREFIX);
    }
}

static LRESULT CALLBACK UserMenuWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    return RunGuardedValue<LRESULT>(L"UserMenuWndProc", 0, [&]() -> LRESULT {
        switch (msg) {
        case WM_PAINT:
            MenuPaint(h);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE:
            MenuSetHover(h, MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp)));
            return 0;
        case WM_LBUTTONDOWN: {
            int idx = MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp));
            if (idx >= 0) {
                if (h == g_userMenuWnd && g_userMenu[idx].sub != 0) OpenSubMenu(idx);
                else MenuFinish(h, MENU_END_INSIDE, idx);
            }
            return 0;
        }
        case WM_RBUTTONDOWN:
            MenuFinish(h, MENU_END_INSIDE, -1);
            return 0;
        case WM_KEYDOWN: {
            const std::vector<UserMenuEntry>& v = MenuEntries(h);
            int& hov = MenuHoverRef(h);
            auto openSubKey = [&]() {
                OpenSubMenu(hov);
                if (g_subMenuWnd) {
                    int first = -1;
                    for (int i2 = 0; i2 < (int)g_subMenu.size(); ++i2)
                        if (g_subMenu[i2].checked) { first = i2; break; }
                    if (first < 0) first = MenuStepIn(g_subMenu, -1, +1);
                    SetFocus(g_subMenuWnd);
                    MenuSetHover(g_subMenuWnd, first);
                }
            };
            switch (wp) {
            case VK_ESCAPE:
                if (h == g_subMenuWnd) { CloseSubMenu(); if (g_userMenuWnd) SetFocus(g_userMenuWnd); }
                else if (g_subMenuWnd) CloseSubMenu();
                else MenuFinish(h, MENU_END_INSIDE, -1);
                break;
            case VK_DOWN:
                MenuSetHover(h, MenuStepIn(v, hov, +1));
                break;
            case VK_UP:
                MenuSetHover(h, MenuStepIn(v, hov, -1));
                break;
            case VK_HOME:
                MenuSetHover(h, MenuStepIn(v, -1, +1));
                break;
            case VK_END:
                MenuSetHover(h, MenuStepIn(v, -1, -1));
                break;
            case VK_RIGHT:
                if (h == g_userMenuWnd && hov >= 0 && hov < (int)v.size() && v[hov].sub) openSubKey();
                break;
            case VK_LEFT:
                if (h == g_subMenuWnd) { CloseSubMenu(); if (g_userMenuWnd) SetFocus(g_userMenuWnd); }
                else if (g_subMenuWnd) CloseSubMenu();
                break;
            case VK_RETURN:
            case VK_SPACE:
                if (MenuEntrySelectable(v, hov)) {
                    if (h == g_userMenuWnd && v[hov].sub) openSubKey();
                    else MenuFinish(h, MENU_END_INSIDE, hov);
                }
                break;
            case VK_APPS:
                MenuFinish(h, MENU_END_INSIDE, -1);
                break;
            }
            return 0;
        }
        case WM_TIMER:
            if (wp == kSubOpenTimer) {
                KillTimer(h, kSubOpenTimer);
                int hov2 = g_userMenuHover;
                if (hov2 >= 0 && hov2 < (int)g_userMenu.size() && g_userMenu[hov2].sub) OpenSubMenu(hov2);
            } else if (wp == kSubCloseTimer) {
                KillTimer(h, kSubCloseTimer);
                bool over = false;
                POINT cpt;
                if (g_subMenuWnd && GetCursorPos(&cpt)) {
                    RECT sr; if (GetWindowRect(g_subMenuWnd, &sr)) over = PtInRect(&sr, cpt) != FALSE;
                }
                const bool onParent = g_userMenuHover >= 0 && g_userMenuHover < (int)g_userMenu.size() &&
                                      g_userMenu[g_userMenuHover].sub != 0;
                if (!over && !onParent) CloseSubMenu();
            }
            return 0;
        case WM_SYSKEYDOWN:
            MenuFinish(h, MENU_END_INSIDE, -1);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) MenuDeactivated(h, (HWND)lp);
            return 0;
        case WM_KILLFOCUS:
            MenuDeactivated(h, (HWND)wp);
            return 0;
        case WM_CLOSE:
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            if (g_userMenuWnd == h) g_userMenuWnd = NULL;
            if (g_subMenuWnd == h) { g_subMenuWnd = NULL; g_subParentRow = -1; g_subMenuHover = -1; }
            return 0;
        }
        return DefWindowProcW(h, msg, wp, lp);
    });
}

static bool EnsureUserMenuClass() {
    if (g_userMenuClassRegistered) return true;
    WNDCLASSW wc = {};
    wc.style = CS_DROPSHADOW;
    wc.lpfnWndProc = UserMenuWndProc;
    wc.hInstance = g_inst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = kUserMenuClass;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"[UserMenu] RegisterClassW failed, err=%u", (unsigned)GetLastError());
        return false;
    }
    g_userMenuClassRegistered = true;
    return true;
}

static void UnregisterUserMenuClass() {
    if (g_userMenuClassRegistered) {
        UnregisterClassW(kUserMenuClass, g_inst);
        g_userMenuClassRegistered = false;
    }
}

static void PlaceUserMenu(int& x, int& y, int w, int hgt) {
    POINT pt = { x, y };
    RECT work = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (mon && GetMonitorInfoW(mon, &mi)) work = mi.rcWork;
    if (x + w > work.right) x -= w;
    if (y + hgt > work.bottom) y -= hgt;
    x = std::max((int)work.left, std::min(x, (int)work.right - w));
    y = std::max((int)work.top, std::min(y, (int)work.bottom - hgt));
}

static HWND CreateUserMenuWindow(HWND owner, int x, int y, int w, int hgt) {
    const DWORD ex = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    HWND menu = NULL;
    if (g_startBand != 0 && g_createInBand) {
        menu = g_createInBand(ex, kUserMenuClass, L"", WS_POPUP, x, y, w, hgt,
                              owner, NULL, g_inst, NULL, g_startBand);
        if (!menu)
            Wh_Log(L"[UserMenu] CreateWindowInBand(%u) failed, err=%u",
                   (unsigned)g_startBand, (unsigned)GetLastError());
    }
    if (!menu)
        menu = CreateWindowExW(ex, kUserMenuClass, L"", WS_POPUP, x, y, w, hgt,
                               owner, NULL, g_inst, NULL);
    return menu;
}


static const Gdiplus::PointF kAvatarBodyLeft[] = {
    {21.50f,199.00f},{22.28f,178.83f},{24.68f,158.79f},
    {28.80f,139.00f},{30.82f,131.85f},{33.60f,124.07f},
    {40.40f,120.00f},{49.84f,115.61f},{56.40f,110.36f},
    {66.00f,111.80f},{71.80f,115.41f},{76.75f,120.46f},
    {83.00f,123.33f},{88.05f,125.55f},{93.44f,127.23f},
    {99.50f,127.33f},
};
static bool BuildVectorAvatarPicture(int S) {
    void* bits = NULL;
    HDC sdc = GetDC(NULL);
    HBITMAP dib = MakeDib(sdc, S, S, &bits);
    ReleaseDC(NULL, sdc);
    if (!dib || !bits) { if (dib) DeleteObject(dib); return false; }
    bool ok = false;
    {
        Gdiplus::Bitmap dst(S, S, S * 4, PixelFormat32bppRGB, (BYTE*)bits);
        Gdiplus::Graphics g(&dst);
        g.Clear(Gdiplus::Color(255, 70, 70, 70));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        const float k = S / 200.0f;
        Gdiplus::Matrix m(k, 0.0f, 0.0f, k, 0.0f, 0.0f);
        g.SetTransform(&m);
        Gdiplus::SolidBrush white(Gdiplus::Color(255, 255, 255, 255));
        const int NL = (int)(sizeof(kAvatarBodyLeft) / sizeof(kAvatarBodyLeft[0]));
        Gdiplus::PointF pts[40];
        int n = 0;
        for (int i = 0; i < NL; ++i) pts[n++] = kAvatarBodyLeft[i];
        for (int s = (NL - 4) / 3; s >= 0; --s) {
            pts[n++] = Gdiplus::PointF(199.0f - kAvatarBodyLeft[3 * s + 2].X, kAvatarBodyLeft[3 * s + 2].Y);
            pts[n++] = Gdiplus::PointF(199.0f - kAvatarBodyLeft[3 * s + 1].X, kAvatarBodyLeft[3 * s + 1].Y);
            pts[n++] = Gdiplus::PointF(199.0f - kAvatarBodyLeft[3 * s].X,     kAvatarBodyLeft[3 * s].Y);
        }
        Gdiplus::GraphicsPath body;
        body.AddLine(Gdiplus::PointF(21.5f, 202.0f), pts[0]);
        body.AddBeziers(pts, n);
        body.AddLine(pts[n - 1], Gdiplus::PointF(177.5f, 202.0f));
        body.CloseFigure();
        Gdiplus::GraphicsPath head;
        head.AddEllipse(97.5f - 38.0f, 68.5f - 43.34f, 76.0f, 86.68f);
        Gdiplus::Matrix rot;
        rot.RotateAt(-9.71f, Gdiplus::PointF(97.5f, 68.5f));
        head.Transform(&rot);
        ok = g.FillPath(&white, &body) == Gdiplus::Ok &&
             g.FillPath(&white, &head) == Gdiplus::Ok;
    }
    if (!ok) { DeleteObject(dib); return false; }
    g_avatarPicBmp = dib; g_avatarPicSize = S; g_avatarPicBits = bits;
    if (g_rt) {
        D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
        g_rt->CreateBitmap(D2D1::SizeU((UINT32)S, (UINT32)S), bits,
                           (UINT32)S * 4, &bp, &g_avatarPicGpu);
    }
    return true;
}
static bool ReadFileBytes(const wchar_t* path, std::vector<BYTE>& out) {
    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(f, NULL);
    bool ok = false;
    if (size && size < 8u * 1024u * 1024u) {
        out.resize(size);
        DWORD got = 0;
        ok = ReadFile(f, out.data(), size, &got, NULL) && got == size;
    }
    CloseHandle(f);
    return ok;
}
static bool LoadAccountPictureBytes(std::vector<BYTE>& out) {
    HANDLE token = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return false;
    BYTE buf[512]; DWORD need = 0;
    bool ok = GetTokenInformation(token, TokenUser, buf, sizeof(buf), &need) != FALSE;
    CloseHandle(token);
    if (!ok) return false;
    LPWSTR sid = NULL;
    if (!ConvertSidToStringSidW(((TOKEN_USER*)buf)->User.Sid, &sid) || !sid) return false;
    std::wstring sub = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\";
    sub += sid; LocalFree(sid);
    HKEY key = NULL;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, sub.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS)
        return false;
    static const wchar_t* vals[] = { L"Image240", L"Image448", L"Image1080", L"Image96" };
    bool done = false;
    for (const wchar_t* v : vals) {
        wchar_t path[1024]; DWORD bytes = sizeof(path), type = 0;
        if (RegQueryValueExW(key, v, NULL, &type, (BYTE*)path, &bytes) != ERROR_SUCCESS) continue;
        if (type != REG_SZ && type != REG_EXPAND_SZ) continue;
        size_t chars = bytes / sizeof(wchar_t);
        path[chars < 1023 ? chars : 1023] = 0;
        std::wstring p = path;
        if (type == REG_EXPAND_SZ) {
            DWORD n2 = ExpandEnvironmentStringsW(p.c_str(), NULL, 0);
            if (n2 > 1 && n2 < 32768) { std::wstring e(n2, 0); if (ExpandEnvironmentStringsW(p.c_str(), &e[0], n2)) p = e.c_str(); }
        }
        const wchar_t* dot = wcsrchr(p.c_str(), L'.');
        if (dot && !_wcsicmp(dot, L".accountpicture-ms")) continue;
        if (ReadFileBytes(p.c_str(), out)) { done = true; break; }
    }
    RegCloseKey(key);
    return done;
}
static void ReleaseAvatarPicture() {
    if (g_avatarPicGpu) { g_avatarPicGpu->Release(); g_avatarPicGpu = NULL; }
    if (g_avatarPicBmp) { DeleteObject(g_avatarPicBmp); g_avatarPicBmp = NULL; }
    g_avatarPicSize = 0;
    g_avatarPicBits = NULL;
}
static bool EnsureAvatarPicture() {
    const int S = ScI(44);
    try {
    if (!g_gdipToken || S < 4) return false;
    if (g_avatarPicBmp && g_avatarPicSize == S) {
        if (!g_avatarPicGpu && g_rt && g_avatarPicBits) {
            D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
            g_rt->CreateBitmap(D2D1::SizeU((UINT32)S, (UINT32)S), g_avatarPicBits,
                               (UINT32)S * 4, &bp, &g_avatarPicGpu);
        }
        return true;
    }
    if (g_avatarPicFailed) return false;
    ReleaseAvatarPicture();
    std::vector<BYTE> bytes;
    if (!LoadAccountPictureBytes(bytes)) {
        bool ok = BuildVectorAvatarPicture(S);
        if (!ok) { g_avatarPicFailed = true; Wh_Log(L"Account picture unavailable; header keeps the drawn silhouette"); }
        return ok;
    }
    if (bytes.empty()) return false;
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!hg) return false;
    void* mem = GlobalLock(hg);
    if (!mem) { GlobalFree(hg); return false; }
    memcpy(mem, bytes.data(), bytes.size());
    GlobalUnlock(hg);
    IStream* st = NULL;
    bool ok = false;
    if (SUCCEEDED(CreateStreamOnHGlobal(hg, TRUE, &st)) && st) {
        Gdiplus::Bitmap srcImg(st, FALSE);
        if (srcImg.GetLastStatus() == Gdiplus::Ok && srcImg.GetWidth() && srcImg.GetHeight()) {
            void* bits = NULL;
            HDC sdc = GetDC(NULL);
            HBITMAP dib = MakeDib(sdc, S, S, &bits);
            ReleaseDC(NULL, sdc);
            if (dib && bits) {
                Gdiplus::Bitmap dst(S, S, S * 4, PixelFormat32bppRGB, (BYTE*)bits);
                Gdiplus::Graphics g(&dst);
                g.Clear(Gdiplus::Color(255, 70, 70, 70));
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                if (g.GetLastStatus() == Gdiplus::Ok &&
                    g.DrawImage(&srcImg, 0, 0, S, S) == Gdiplus::Ok) {
                    g_avatarPicBmp = dib; g_avatarPicSize = S; g_avatarPicBits = bits; ok = true;
                    if (g_rt) {
                        D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
                        g_rt->CreateBitmap(D2D1::SizeU((UINT32)S, (UINT32)S), bits,
                                           (UINT32)S * 4, &bp, &g_avatarPicGpu);
                    }
                }
                if (!ok) DeleteObject(dib);
            }
        }
        st->Release();
    } else GlobalFree(hg);
    if (!ok) { g_avatarPicFailed = true; Wh_Log(L"Account picture unavailable; header keeps the drawn silhouette"); }
    return ok;
    } catch (const std::exception& e) {
        Wh_Log(L"[avatar] picture decode failed: %S", e.what());
        ReleaseAvatarPicture();
        return false;
    } catch (...) {
        Wh_Log(L"[avatar] picture decode failed (unknown exception)");
        ReleaseAvatarPicture();
        return false;
    }
}
static void ReleaseUiIcons() {
    UiIcon* all[] = { &g_iconPower, &g_iconSearch, &g_iconArrowDown, &g_iconArrowUp, &g_iconHover, &g_iconZoom };
    for (UiIcon* ic : all) {
        if (ic->gpu) { ic->gpu->Release(); ic->gpu = NULL; }
        if (ic->bmp) { DeleteObject(ic->bmp); ic->bmp = NULL; }
        if (ic->px) { delete[] ic->px; ic->px = NULL; }
        ic->w = ic->h = 0;
    }
}
static void GlyphPower(Gdiplus::Graphics& g, float sz, const Gdiplus::Color& c) {
    Gdiplus::Pen pen(c, sz * 0.105f);
    pen.SetStartCap(Gdiplus::LineCapFlat);
    pen.SetEndCap(Gdiplus::LineCapFlat);
    const float r = sz * 0.38f;
    const float cx = sz * 0.50f, cy = sz * 0.55f;
    g.DrawArc(&pen, cx - r, cy - r, r * 2.0f, r * 2.0f, 305.0f, 290.0f);
    Gdiplus::SolidBrush br(c);
    g.FillRectangle(&br, cx - sz * 0.055f, sz * 0.07f, sz * 0.11f, sz * 0.46f);
}
static void GlyphSearch(Gdiplus::Graphics& g, float sz, const Gdiplus::Color& c) {
    Gdiplus::Pen pen(c, sz * 0.105f);
    pen.SetStartCap(Gdiplus::LineCapFlat);
    pen.SetEndCap(Gdiplus::LineCapRound);
    const float r = sz * 0.30f;
    const float cx = sz * 0.60f, cy = sz * 0.34f;
    g.DrawEllipse(&pen, cx - r, cy - r, r * 2.0f, r * 2.0f);
    Gdiplus::Pen hp(c, sz * 0.12f);
    hp.SetStartCap(Gdiplus::LineCapFlat);
    hp.SetEndCap(Gdiplus::LineCapRound);
    const float dx = -0.77f, dy = 0.64f;
    const float sx = cx + r * dx, sy = cy + r * dy;
    g.DrawLine(&hp, sx, sy, sx + sz * 0.32f * dx, sy + sz * 0.32f * dy);
}
static void GlyphArrow(Gdiplus::Graphics& g, float sz, bool down, const Gdiplus::Color& c) {
    Gdiplus::Pen ring(c, sz * 0.067f);
    g.DrawEllipse(&ring, sz * 0.0335f, sz * 0.0335f, sz * 0.933f, sz * 0.933f);
    const float t = sz * 0.10f, W = sz * 0.333f;
    const float cx = sz * 0.5f, d = down ? 1.0f : -1.0f;
    const float cy = sz * 0.48f;
    const float yT = cy - d * sz * 0.22f, yB = cy - d * sz * 0.04f;
    const float yS = cy + d * sz * 0.07f, yP = cy + d * sz * 0.22f;
    Gdiplus::PointF pts[9] = {
        { cx - t * 0.5f, yT }, { cx + t * 0.5f, yT },
        { cx + t * 0.5f, yB }, { cx + W * 0.5f, yB },
        { cx + W * 0.5f, yS }, { cx, yP },
        { cx - W * 0.5f, yS }, { cx - W * 0.5f, yB },
        { cx - t * 0.5f, yB } };
    Gdiplus::SolidBrush br(c);
    g.FillPolygon(&br, pts, 9);
}
static void GlyphZoom(Gdiplus::Graphics& g, float sz, const Gdiplus::Color& c) {
    Gdiplus::Pen ring(c, sz * 0.067f);
    g.DrawEllipse(&ring, sz * 0.0335f, sz * 0.0335f, sz * 0.933f, sz * 0.933f);
    Gdiplus::Pen bar(c, sz * 0.09f);
    bar.SetStartCap(Gdiplus::LineCapFlat);
    bar.SetEndCap(Gdiplus::LineCapFlat);
    g.DrawLine(&bar, sz * 0.30f, sz * 0.5f, sz * 0.70f, sz * 0.5f);
}
static bool BuildHoverRect(UiIcon& ic, int sz) {
    if (sz <= 0) return false;
    try {
        if (ic.px) { delete[] ic.px; ic.px = NULL; }
        if (ic.bmp) { DeleteObject(ic.bmp); ic.bmp = NULL; }
        if (ic.gpu) { ic.gpu->Release(); ic.gpu = NULL; }
        HDC sdc = GetDC(NULL);
        void* bits = NULL;
        HBITMAP dib = MakeDib(sdc, sz, sz, &bits);
        ReleaseDC(NULL, sdc);
        if (!dib || !bits) return false;
        BYTE* px = new BYTE[(size_t)sz * sz * 4];
        for (int i = 0; i < sz * sz; ++i) {
            px[i * 4 + 0] = 52; px[i * 4 + 1] = 52; px[i * 4 + 2] = 52; px[i * 4 + 3] = 52;
        }
        HDC ref = GetDC(NULL);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = sz; bi.bmiHeader.biHeight = -sz;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        SetDIBits(ref, dib, 0, sz, px, &bi, DIB_RGB_COLORS);
        ReleaseDC(NULL, ref);
        ic.bmp = dib; ic.px = px; ic.w = sz; ic.h = sz;
        if (g_rt) {
            D2D1_BITMAP_PROPERTIES bp = {};
            bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
            bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            bp.dpiX = bp.dpiY = 96.0f;
            g_rt->CreateBitmap(D2D1::SizeU((UINT32)sz, (UINT32)sz), px, (UINT32)sz * 4, &bp, &ic.gpu);
        }
        (void)bits;
        return ic.bmp != NULL;
    } catch (...) {
        Wh_Log(L"[icons] BuildHoverRect failed");
        return false;
    }
}
static bool BuildOneIcon(UiIcon& ic, int sz, int kind) {
    void* bits = NULL;
    HDC sdc = GetDC(NULL);
    HBITMAP dib = MakeDib(sdc, sz, sz, &bits);
    ReleaseDC(NULL, sdc);
    if (!dib || !bits) return false;
    {
        Gdiplus::Bitmap bmp(sz, sz, sz * 4, PixelFormat32bppARGB, (BYTE*)bits);
        Gdiplus::Graphics g(&bmp);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        const Gdiplus::Color white(255, 255, 255);
        if (kind == 0) GlyphPower(g, (float)sz, white);
        else if (kind == 1) GlyphSearch(g, (float)sz, white);
        else if (kind == 4) GlyphZoom(g, (float)sz, white);
        else GlyphArrow(g, (float)sz, kind == 2, white);
    }
    ic.bmp = dib; ic.w = sz; ic.h = sz;
    if (g_rt && bits) {
        BYTE* px = new (std::nothrow) BYTE[(size_t)sz * sz * 4];
        if (px) {
            for (int y = 0; y < sz; ++y) {
                BYTE* dst = px + (size_t)y * sz * 4;
                const BYTE* src = (const BYTE*)bits + (size_t)y * sz * 4;
                memcpy(dst, src, (size_t)sz * 4);
                for (int x = 0; x < sz; ++x) {
                    const BYTE a = dst[x * 4 + 3];
                    if (a == 255) continue;
                    if (a == 0) { dst[x * 4] = dst[x * 4 + 1] = dst[x * 4 + 2] = 0; continue; }
                    dst[x * 4]     = (BYTE)((dst[x * 4]     * a) / 255);
                    dst[x * 4 + 1] = (BYTE)((dst[x * 4 + 1] * a) / 255);
                    dst[x * 4 + 2] = (BYTE)((dst[x * 4 + 2] * a) / 255);
                }
            }
            D2D1_BITMAP_PROPERTIES bp = GpuBitmapProperties();
            bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            if (SUCCEEDED(g_rt->CreateBitmap(D2D1::SizeU((UINT32)sz, (UINT32)sz), px,
                                             (UINT32)sz * 4, &bp, &ic.gpu))) {
                ic.px = px;
            } else delete[] px;
        }
    }
    return true;
}
static bool EnsureUiIcons() {
    if (!g_gdipToken) return false;
    const int S = ScI(48), A = ScI(34);
    if (g_iconPower.bmp && g_iconPower.w == S && g_iconArrowDown.w == A && g_iconHover.bmp && g_iconHover.w == ScI(34) && g_iconZoom.bmp) return true;
    ReleaseUiIcons();
    try {
        BuildOneIcon(g_iconPower, S, 0);
        BuildOneIcon(g_iconSearch, S, 1);
        BuildOneIcon(g_iconArrowDown, A, 2);
        BuildOneIcon(g_iconArrowUp, A, 3);
        BuildOneIcon(g_iconZoom, A, 4);
        BuildHoverRect(g_iconHover, ScI(34));
    } catch (const std::exception& e) {
        Wh_Log(L"[icons] GDI+ icon build failed: %S", e.what());
        ReleaseUiIcons();
        return false;
    } catch (...) {
        Wh_Log(L"[icons] GDI+ icon build failed (unknown exception)");
        ReleaseUiIcons();
        return false;
    }
    return g_iconPower.bmp != NULL;
}
static void BlitIconGdi(HDC dc, const UiIcon& ic, const RECT& r, bool hover, BYTE alpha) {
    if (!ic.bmp) return;
    const int w = r.right - r.left, h = r.bottom - r.top;
    HDC idc = CreateCompatibleDC(dc);
    HGDIOBJ o = SelectObject(idc, ic.bmp);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
    AlphaBlend(dc, r.left, r.top, w, h, idc, 0, 0, ic.w, ic.h, bf);
    SelectObject(idc, o);
    DeleteDC(idc);
}
static HBITMAP g_charmBmp = NULL;
static BYTE* g_charmBits = NULL;
static int g_charmW = 0, g_charmH = 0;
static ID2D1Bitmap* g_charmGpu = NULL;
#define CHARM_SLIDE_MS 260

static int CharmW() { return ScI(346); }
static COLORREF CharmBlend(COLORREF bg, int pct) {
    int r = GetRValue(bg), g = GetGValue(bg), b = GetBValue(bg);
    r += (255 - r) * pct / 100; g += (255 - g) * pct / 100; b += (255 - b) * pct / 100;
    return RGB(r, g, b);
}
static COLORREF CharmPane() {
    DWORD c = 0; BOOL o = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
        return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    return RGB(8, 121, 0);
}
struct CharmLay { int mx, titleY, lblY, editY, editH, editR, btnW, listY, rowH, vis; };
static CharmLay CharmLayout() {
    CharmLay l;
    l.mx = ScI(41); l.titleY = ScI(35); l.lblY = ScI(80);
    l.editY = ScI(110); l.editH = ScI(32);
    l.editR = CharmW() - ScI(40); l.btnW = ScI(32);
    l.listY = ScI(168); l.rowH = ScI(52);
    l.vis = std::max(1, (g_sh - l.listY - ScI(20)) / l.rowH);
    return l;
}
static int CharmClearW() { return ScI(24); }
static RECT CharmScopeRect() {
    CharmLay L = CharmLayout();
    RECT r = { L.mx, L.lblY - ScI(4), L.editR, L.editY - ScI(2) };
    return r;
}
static const wchar_t* CharmScopeName(int scope) {
    switch (scope) {
        case 1:  return T(STR_SCOPE_APPS);
        case 2:  return T(STR_SCOPE_SETTINGS);
        case 3:  return T(STR_SCOPE_FILES);
        default: return T(STR_CHARM_SCOPE);
    }
}
static RECT CharmScopeItemRect(int i) {
    CharmLay L = CharmLayout();
    const int ddY = L.editY + L.editH + ScI(2), itemH = ScI(28);
    RECT r = { L.mx, ddY + i * itemH, L.editR, ddY + (i + 1) * itemH };
    return r;
}
static bool CharmPaneRect(RECT* r) {
    if (!g_charmW) return false;
    r->left = g_sw - g_charmW; r->top = 0; r->right = g_sw; r->bottom = g_charmH;
    return true;
}
static void CharmRelease() {
    if (g_charmGpu) { g_charmGpu->Release(); g_charmGpu = NULL; }
    if (g_charmBmp) { DeleteObject(g_charmBmp); g_charmBmp = NULL; }
    g_charmBits = NULL; g_charmW = g_charmH = 0;
}
static RECT CharmEditBox() {
    CharmLay L = CharmLayout();
    RECT b = { L.mx, L.editY, L.editR - L.btnW, L.editY + L.editH };
    return b;
}
static RECT CharmSubmitBtn() {
    CharmLay L = CharmLayout();
    RECT b = { L.editR - L.btnW, L.editY, L.editR, L.editY + L.editH };
    return b;
}
static RECT CharmClearBtn() {
    RECT b = CharmEditBox();
    b.left = b.right - CharmClearW();
    return b;
}
static RECT CharmScrollTrack() {
    CharmLay L = CharmLayout();
    RECT r = { g_charmW - ScI(8), L.listY, g_charmW - ScI(3), g_charmH - ScI(20) };
    if (r.bottom <= r.top) r.bottom = r.top + 1;
    return r;
}

static void CharmRender(int contOff) {
    if (g_sw <= 0 || g_sh <= 0) return;
    try {
    const int W = CharmW(), H = g_sh;
    if (W <= 0 || H <= 0) return;
    if (g_charmW != W || g_charmH != H) CharmRelease();
    HDC sdc = GetDC(NULL);
    if (!g_charmBmp) {
        g_charmBmp = MakeDib(sdc, W, H, (void**)&g_charmBits);
        if (!g_charmBmp || !g_charmBits) { ReleaseDC(NULL, sdc); CharmRelease(); return; }
        g_charmW = W; g_charmH = H;
    }
    g_charmCont = contOff;
    AutoCompatDC adc(sdc);
    HDC dc = adc;
    HGDIOBJ ob = SelectObject(dc, g_charmBmp);
    RECT all = { 0, 0, W, H };
    COLORREF pane = CharmPane();
    { HBRUSH pb = CreateSolidBrush(pane); FillRect(dc, &all, pb); DeleteObject(pb); }
    SetBkMode(dc, TRANSPARENT);
    const int ox = contOff;
    CharmLay L = CharmLayout();
    AutoFont fTitle(CreateFontW(-ScI(29), 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                                CLEARTYPE_QUALITY, 0, L"Segoe UI Light"));
    AutoFont fLabel(CreateFontW(-ScI(15), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                                CLEARTYPE_QUALITY, 0, L"Segoe UI"));
    AutoFont fText(CreateFontW(-ScI(16), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                               CLEARTYPE_QUALITY, 0, L"Segoe UI"));
    AutoFont fSub(CreateFontW(-ScI(12), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
                              CLEARTYPE_QUALITY, 0, L"Segoe UI"));
    SetTextColor(dc, RGB(255, 255, 255));
    HFONT of = (HFONT)SelectObject(dc, fTitle.get());
    const wchar_t* ttl = T(STR_CHARM_TITLE);
    TextOutW(dc, L.mx + ox, L.titleY, ttl, lstrlenW(ttl));
    SelectObject(dc, fLabel.get());
    const wchar_t* scp = CharmScopeName(g_charmScope);
    TextOutW(dc, L.mx + ox, L.lblY, scp, lstrlenW(scp));
    SIZE ls = {};
    GetTextExtentPoint32W(dc, scp, lstrlenW(scp), &ls);
    {
        Gdiplus::Graphics gg(dc);
        gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen cp(Gdiplus::Color(255, 255, 255), ScF(1.6f));
        cp.SetStartCap(Gdiplus::LineCapRound); cp.SetEndCap(Gdiplus::LineCapRound);
        float x0 = (float)(L.mx + ox + ls.cx + ScI(8)), y0 = (float)(L.lblY + ScI(6));
        float d = (float)ScI(4);
        gg.DrawLine(&cp, x0, y0, x0 + d, y0 + d);
        gg.DrawLine(&cp, x0 + d, y0 + d, x0 + 2 * d, y0);
    }
    RECT box = CharmEditBox(); box.left += ox; box.right += ox;
    RECT btn = CharmSubmitBtn(); btn.left += ox; btn.right += ox;
    { HBRUSH wb = CreateSolidBrush(RGB(255, 255, 255)); FillRect(dc, &box, wb);
      HBRUSH hb2 = CreateSolidBrush(pane); FillRect(dc, &btn, hb2); DeleteObject(hb2);
      FrameRect(dc, &btn, wb); DeleteObject(wb); }
    {
        Gdiplus::Graphics gg(dc);
        gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        float sz = (float)ScI(18);
        float cx = (btn.left + btn.right) * 0.5f - sz * 0.5f;
        float cy = (btn.top + btn.bottom) * 0.5f - sz * 0.5f;
        Gdiplus::Color wc(255, 255, 255);
        Gdiplus::Pen lp(wc, sz * 0.105f);
        float r = sz * 0.325f, lx = cx + sz * 0.62f, ly = cy + sz * 0.36f;
        gg.DrawEllipse(&lp, lx - r, ly - r, r * 2.0f, r * 2.0f);
        Gdiplus::Pen hp(wc, sz * 0.12f);
        hp.SetStartCap(Gdiplus::LineCapFlat); hp.SetEndCap(Gdiplus::LineCapRound);
        float dx = -0.77f, dy = 0.64f;
        float sx = lx + r * dx, sy = ly + r * dy;
        gg.DrawLine(&hp, sx, sy, sx + sz * 0.34f * dx, sy + sz * 0.34f * dy);
    }
    const bool showClear = g_queryLen > 0;
    if (showClear) {
        RECT cb = CharmClearBtn(); cb.left += ox; cb.right += ox;
        Gdiplus::Graphics gg(dc);
        gg.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen xp(Gdiplus::Color(255, 96, 96, 96), ScF(1.6f));
        xp.SetStartCap(Gdiplus::LineCapRound); xp.SetEndCap(Gdiplus::LineCapRound);
        float m = (float)ScI(7);
        float ccx = (cb.left + cb.right) * 0.5f, ccy = (cb.top + cb.bottom) * 0.5f;
        gg.DrawLine(&xp, ccx - m, ccy - m, ccx + m, ccy + m);
        gg.DrawLine(&xp, ccx - m, ccy + m, ccx + m, ccy - m);
    }
    {
        RECT tr = { box.left + ScI(6), box.top, (showClear ? box.right - CharmClearW() : box.right) - ScI(4), box.bottom };
        if (tr.right < tr.left) tr.right = tr.left;
        HFONT tf = (HFONT)SelectObject(dc, fText.get());
        SIZE sz = {};
        GetTextExtentPoint32W(dc, g_query, g_queryLen, &sz);
        bool tail = sz.cx > (tr.right - tr.left - ScI(2));
        SetTextColor(dc, RGB(0, 0, 0));
        SaveDC(dc);
        IntersectClipRect(dc, tr.left, tr.top, tr.right + 1, tr.bottom);
        DrawTextW(dc, g_query, g_queryLen, &tr,
                  DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | (tail ? DT_RIGHT : DT_LEFT));
        RestoreDC(dc, -1);
        if (g_charmOpen && (NowMs() / 500) % 2 == 0) {
            int cx = tail ? tr.right : tr.left + sz.cx;
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            HGDIOBJ op = SelectObject(dc, pen);
            MoveToEx(dc, cx, box.top + ScI(6), NULL);
            LineTo(dc, cx, box.bottom - ScI(6));
            SelectObject(dc, op);
            DeleteObject(pen);
        }
        SelectObject(dc, tf);
    }
    {
        SrwGuard g(&g_appsLock, false);
        if (g_queryLen > 0 && g_results.empty()) {
            HFONT tf = (HFONT)SelectObject(dc, fText.get());
            SetTextColor(dc, RGB(255, 255, 255));
            const wchar_t* msg = T(STR_NO_RESULTS);
            TextOutW(dc, L.mx + ox, L.listY, msg, lstrlenW(msg));
            SelectObject(dc, tf);
        }
        int y = L.listY;
        for (int r = g_charmScroll; g_queryLen > 0 && r < (int)g_results.size() && r < g_charmScroll + L.vis; r++, y += L.rowH) {
            int idx = g_results[r];
            if (idx < 0 || idx >= (int)g_apps.size()) continue;
            if (r == g_charmSel || r == g_charmHover) {
                RECT band = { 0, y, W, y + L.rowH };
                HBRUSH bb = CreateSolidBrush(CharmBlend(pane, r == g_charmSel ? 24 : 14));
                FillRect(dc, &band, bb); DeleteObject(bb);
            }
            if ((size_t)idx < g_icons.size()) {
            IconSlot& sl = g_icons[(size_t)idx];
            if (!InterlockedCompareExchange(&sl.ready, 1, 1) && (g_apps[idx].charmCat == CC_FILE || g_apps[idx].exe))
                LoadIconFor((size_t)idx);
            if (InterlockedCompareExchange(&sl.ready, 1, 1) && sl.px && sl.w > 0) {
                bool drewIcon = false;
                if (g_gdipToken) {
                    Gdiplus::Bitmap wrap(sl.w, sl.h, sl.w * 4, PixelFormat32bppPARGB, sl.px);
                    if (wrap.GetLastStatus() == Gdiplus::Ok) {
                        Gdiplus::Graphics gp(dc);
                        gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                        gp.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                        drewIcon = gp.DrawImage(&wrap,
                            Gdiplus::Rect(L.mx + ox, y + (L.rowH - ScI(32)) / 2, ScI(32), ScI(32)),
                            0, 0, sl.w, sl.h, Gdiplus::UnitPixel) == Gdiplus::Ok;
                    }
                }
                if (!drewIcon) {
                AutoCompatDC apdc(sdc);
                HDC pdc = apdc;
                HBITMAP pb2 = CreateCompatibleBitmap(sdc, sl.w, sl.h);
                HGDIOBJ opb = SelectObject(pdc, pb2);
                BITMAPINFO bi = {};
                bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
                bi.bmiHeader.biWidth = sl.w; bi.bmiHeader.biHeight = -sl.h;
                bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;
                BYTE* tmp = new BYTE[(size_t)sl.w * sl.h * 4];
                for (int q = 0; q < sl.w * sl.h; ++q) {
                    BYTE a = sl.px[q * 4 + 3];
                    tmp[q * 4 + 3] = a;
                    if (a == 0) { tmp[q*4]=0; tmp[q*4+1]=0; tmp[q*4+2]=0; continue; }
                    tmp[q * 4 + 0] = (BYTE)std::min(255, sl.px[q * 4 + 0] * 255 / a);
                    tmp[q * 4 + 1] = (BYTE)std::min(255, sl.px[q * 4 + 1] * 255 / a);
                    tmp[q * 4 + 2] = (BYTE)std::min(255, sl.px[q * 4 + 2] * 255 / a);
                }
                SetDIBitsToDevice(pdc, 0, 0, sl.w, sl.h, 0, 0, 0, sl.h, tmp, &bi, DIB_RGB_COLORS);
                delete[] tmp;
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                AlphaBlend(dc, L.mx + ox, y + (L.rowH - ScI(32)) / 2, ScI(32), ScI(32),
                           pdc, 0, 0, sl.w, sl.h, bf);
                SelectObject(pdc, opb);
                DeleteObject(pb2);
                }
            }
            }
            int tx = L.mx + ox + ScI(44);
            RECT nr = { tx, y + ScI(6), W - ScI(20) + ox, y + ScI(28) };
            RECT sr = { tx, y + ScI(28), W - ScI(20) + ox, y + ScI(48) };
            HFONT nf = (HFONT)SelectObject(dc, fText.get());
            SetTextColor(dc, RGB(255, 255, 255));
            DrawTextW(dc, g_apps[idx].name.c_str(), -1, &nr,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(dc, fSub.get());
            SetTextColor(dc, CharmBlend(pane, 70));
            if (g_apps[idx].charmCat == CC_FILE) {
                std::wstring parent = g_apps[idx].parse;
                size_t slash = parent.find_last_of(L"\\/");
                if (slash != std::wstring::npos) parent.resize(slash);
                DrawTextW(dc, parent.c_str(), (int)parent.size(), &sr,
                          DT_SINGLELINE | DT_VCENTER | DT_PATH_ELLIPSIS | DT_NOPREFIX);
            } else {
                const wchar_t* sub = g_apps[idx].charmCat == CC_SETTING ? T(STR_ROW_SETTINGS)
                                                                     : T(STR_ROW_APPS);
                DrawTextW(dc, sub, -1, &sr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
            }
            SelectObject(dc, nf);
        }
        int total = g_queryLen > 0 ? (int)g_results.size() : 0;
        if (total > L.vis) {
            RECT track = CharmScrollTrack();
            HBRUSH tb = CreateSolidBrush(CharmBlend(pane, 12));
            FillRect(dc, &track, tb); DeleteObject(tb);
            int trackH = track.bottom - track.top;
            int thumbH = std::max(ScI(24), trackH * L.vis / total);
            if (thumbH > trackH) thumbH = trackH;
            int maxScroll = total - L.vis;
            int scroll = std::min(std::max(0, g_charmScroll), maxScroll);
            int thumbY = track.top + (trackH - thumbH) * scroll / std::max(1, maxScroll);
            RECT th = { track.left, thumbY, track.right, thumbY + thumbH };
            HBRUSH hb3 = CreateSolidBrush(CharmBlend(pane, g_charmHover == -2 ? 50 : 38));
            FillRect(dc, &th, hb3); DeleteObject(hb3);
        }
    }
    if (g_charmDdOpen) {
        RECT first = CharmScopeItemRect(0), last = CharmScopeItemRect(3);
        RECT dd = { first.left + ox, first.top, first.right + ox, last.bottom };
        { HBRUSH wb = CreateSolidBrush(RGB(255, 255, 255)); FillRect(dc, &dd, wb); DeleteObject(wb); }
        { HBRUSH fb = CreateSolidBrush(RGB(200, 200, 200)); FrameRect(dc, &dd, fb); DeleteObject(fb); }
        HFONT df = (HFONT)SelectObject(dc, fText.get());
        for (int i = 0; i < 4; ++i) {
            RECT ir = CharmScopeItemRect(i); ir.left += ox; ir.right += ox;
            COLORREF tc = (i == g_charmScope) ? pane : RGB(32, 32, 32);
            if (i == g_charmDdHover) {
                RECT hr = { ir.left + 1, ir.top + (i == 0 ? 1 : 0), ir.right - 1,
                            ir.bottom - (i == 3 ? 1 : 0) };
                HBRUSH hb = CreateSolidBrush(pane); FillRect(dc, &hr, hb); DeleteObject(hb);
                tc = RGB(255, 255, 255);
            }
            SetTextColor(dc, tc);
            ir.left += ScI(10);
            DrawTextW(dc, CharmScopeName(i), -1, &ir,
                      DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
        }
        SelectObject(dc, df);
    }
    SelectObject(dc, of);
    SelectObject(dc, ob);
    ReleaseDC(NULL, sdc);
    for (int i = 0; i < W * H; ++i) g_charmBits[i * 4 + 3] = 255;
    if (g_rt) {
        D2D1_SIZE_U sz = g_charmGpu ? g_charmGpu->GetPixelSize() : D2D1::SizeU(0, 0);
        if (g_charmGpu && sz.width == (UINT32)W && sz.height == (UINT32)H) {
            g_charmGpu->CopyFromMemory(NULL, g_charmBits, (UINT32)W * 4);
        } else {
            if (g_charmGpu) { g_charmGpu->Release(); g_charmGpu = NULL; }
            D2D1_BITMAP_PROPERTIES bp = {};
            bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
            bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            bp.dpiX = bp.dpiY = 96.0f;
            g_rt->CreateBitmap(D2D1::SizeU((UINT32)W, (UINT32)H), g_charmBits, (UINT32)W * 4, &bp, &g_charmGpu);
        }
    }
    } catch (...) { Wh_Log(L"[charm] render failed"); CharmRelease(); }
}

static float CharmPresence(DWORD now) {
    float t = (float)(now - g_charmTick) / (float)CHARM_SLIDE_MS;
    if (g_charmOpen) return Clamp01(t);
    return 1.0f - Clamp01(t);
}
static bool CharmVisible(DWORD now) {
    return g_charmOpen || (g_charmTick && (now - g_charmTick) < CHARM_SLIDE_MS);
}
static void DrawCharmBarGpu(DWORD now) {
    if (!CharmVisible(now) || !g_charmGpu) return;
    try {
    float e = EaseMetroOut(CharmPresence(now));
    float off = (1.0f - e) * (float)g_charmW;
    g_rt->DrawBitmap(g_charmGpu, D2D1::RectF((float)g_sw - (float)g_charmW + off, 0.0f,
                                             (float)g_sw + off, (float)g_charmH), 1.0f,
                     off == 0.0f ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                                 : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    } catch (...) { Wh_Log(L"[charm] gpu draw failed"); }
}
static void DrawCharmBarGdi(HDC sdc, DWORD now) {
    if (!CharmVisible(now) || !g_charmBmp) return;
    try {
    float e = EaseMetroOut(CharmPresence(now));
    float off = (1.0f - e) * (float)g_charmW;
    HDC idc = CreateCompatibleDC(sdc);
    HGDIOBJ o = SelectObject(idc, g_charmBmp);
    BitBlt(sdc, g_sw - g_charmW + (int)off, 0, g_charmW, g_charmH, idc, 0, 0, SRCCOPY);
    SelectObject(idc, o);
    DeleteDC(idc);
    } catch (...) { Wh_Log(L"[charm] gdi draw failed"); }
}
static bool DismissCompanionSearchCharm(POINT screenPt, bool havePt) {
    HWND other = FindWindowW(L"Win81SearchCharmMod", NULL);
    if (!other || !IsWindowVisible(other)) return false;
    if (havePt) {
        RECT r;
        if (GetWindowRect(other, &r) && PtInRect(&r, screenPt)) return false;
    } else {
        screenPt.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        screenPt.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    }
    // Search Charm mod currently only handles WM_APP + 3 (WM_APP_POINTER_DOWN); keep using it
    // until a Search Charm update that accepts a named message is published.
    PostMessageW(other, WM_APP + 3, (WPARAM)(LONG_PTR)screenPt.x, (LPARAM)(LONG_PTR)screenPt.y);
    return true;
}
static void SuppressOtherSearchCharm() { POINT z = { 0, 0 }; DismissCompanionSearchCharm(z, false); }

static void SetCharmMouseHook(bool on) {
    if (g_hookTid) PostThreadMessageW(g_hookTid, on ? WM_HOOKCTL_MOUSE_ON : WM_HOOKCTL_MOUSE_OFF, 0, 0);
}
static void OpenCharm(wchar_t firstChar) {
    if (!g_open || g_closing) return;
    try {
    g_queryLen = 0; g_query[0] = 0;
    if (firstChar) QueryAppend(firstChar);
    Wh_Log(L"[charm] open char=%d view=%d", (int)firstChar, (int)g_view);
    g_charmSel = 0; g_charmScroll = 0; g_charmHover = -1;
    g_charmScope = 0;
    g_charmDdOpen = false;
    g_charmDdHover = -1;
    RunSearch();
    if (!g_charmOpen) { g_charmOpen = true; g_charmTick = NowMs(); SuppressOtherSearchCharm(); SetCharmMouseHook(true); }
    CharmRender(0);
    RepaintNow();
    } catch (...) { Wh_Log(L"[charm] open failed"); g_charmOpen = false; }
}
static void CloseCharm(bool now) {
    if (!g_charmOpen) return;
    Wh_Log(L"[charm] close now=%d", (int)now);
    g_charmOpen = false;
    SetCharmMouseHook(false);
    g_charmTick = now ? 0 : NowMs();
    g_charmHover = -1;
    g_charmDdOpen = false; g_charmDdHover = -1;
    RepaintNow();
}
static void CharmToggle() {
    if (g_charmOpen) CloseCharm(false);
    else OpenCharm(0);
}
static void CharmMoveSel(int delta) {
    int n = (int)g_results.size();
    if (!n) return;
    g_charmSel += delta;
    if (g_charmSel < 0) g_charmSel = 0;
    if (g_charmSel >= n) g_charmSel = n - 1;
    CharmLay L = CharmLayout();
    if (g_charmSel < g_charmScroll) g_charmScroll = g_charmSel;
    if (g_charmSel >= g_charmScroll + L.vis) g_charmScroll = g_charmSel - L.vis + 1;
    CharmRender(g_charmCont);
    RepaintNow();
}
enum { CH_NONE = -1, CH_ROW = 0, CH_CLEAR = 1, CH_SUBMIT = 2, CH_SCROLL = 3, CH_SCOPE = 4,
       CH_SCOPEITEM = 5, CH_DDCLOSE = 6 };
static int CharmHit(int lx, int ly, int* rowOut) {
    *rowOut = -1;
    RECT cb = CharmClearBtn(), sb = CharmSubmitBtn(), tr = CharmScrollTrack();
    POINT p = { lx, ly };
    RECT sco = CharmScopeRect();
    if (g_charmDdOpen) {
        for (int i = 0; i < 4; ++i) {
            RECT ir = CharmScopeItemRect(i);
            if (PtInRect(&ir, p)) { *rowOut = i; return CH_SCOPEITEM; }
        }
        if (PtInRect(&sco, p)) return CH_SCOPE;
        return CH_DDCLOSE;
    }
    if (PtInRect(&sco, p)) return CH_SCOPE;
    if (g_queryLen > 0) { RECT cbHit = cb; cbHit.left -= ScI(10);
                          if (PtInRect(&cbHit, p)) return CH_CLEAR; }
    if (PtInRect(&sb, p)) return CH_SUBMIT;
    if ((int)g_results.size() > CharmLayout().vis &&
        lx >= tr.left - ScI(7) && lx <= g_charmW && ly >= tr.top - ScI(4) && ly < tr.bottom + ScI(4))
        return CH_SCROLL;
    CharmLay L = CharmLayout();
    if (ly >= L.listY && ly < g_charmH - ScI(20)) {
        int r = (ly - L.listY) / L.rowH + g_charmScroll;
        if (r >= 0 && r < (int)g_results.size()) { *rowOut = r; return CH_ROW; }
    }
    return CH_NONE;
}

static int UserTextWidth() {
    if (g_userTextW >= 0) return g_userTextW;
    int w = ScI(120);
    HDC dc = GetDC(NULL);
    if (dc) {
        HFONT old = g_fUser ? (HFONT)SelectObject(dc, g_fUser) : NULL;
        SIZE sz = {};
        if (GetTextExtentPoint32W(dc, DisplayName(), (int)wcslen(DisplayName()), &sz)) w = sz.cx;
        if (old) SelectObject(dc, old);
        ReleaseDC(NULL, dc);
    }
    g_userTextW = w;
    return w;
}

static int HeaderAvatarRight() { return g_sw - ScI(150); }
static void HeaderButtonRects(RECT* pwr, RECT* srch) {
    const int s = ScI(40), top = ScI(34);
    if (srch) { srch->right = g_sw - ScI(40); srch->left = srch->right - s; srch->top = top; srch->bottom = top + s; }
    if (pwr) { pwr->right = g_sw - ScI(104); pwr->left = pwr->right - s; pwr->top = top; pwr->bottom = top + s; }
}
static void IconDestRect(const RECT& hit, RECT* out) {
    const int s = ScI(22);
    const int cx = (hit.left + hit.right) / 2, cy = (hit.top + hit.bottom) / 2;
    out->left = cx - s / 2; out->right = cx + s / 2;
    out->top = cy - s / 2; out->bottom = cy + s / 2;
}
static void AppsArrowRect(RECT* r) {
    const int cx = LEFT + ScI(11), cy = g_sh - ScI(44), h = ScI(18);
    r->left = cx - h; r->top = cy - h; r->right = cx + h; r->bottom = cy + h;
}
static void ZoomButtonRect(RECT* r) {
    const int cx = g_sw - LEFT - ScI(11), cy = g_sh - ScI(44), h = ScI(18);
    r->left = cx - h; r->top = cy - h; r->right = cx + h; r->bottom = cy + h;
}
static int AppsTitleWidth() {
    static int cached = -1, cachedLang = -1;
    if (cached >= 0 && cachedLang == g_lang) return cached;
    int w = ScI(150);
    HDC dc = GetDC(NULL);
    if (dc && g_fTitle) {
        HGDIOBJ old = SelectObject(dc, g_fTitle);
        SIZE sz = {};
        if (GetTextExtentPoint32W(dc, T(STR_APPS), (int)wcslen(T(STR_APPS)), &sz)) w = sz.cx;
        SelectObject(dc, old);
    }
    if (dc) ReleaseDC(NULL, dc);
    cached = w; cachedLang = g_lang;
    return w;
}
static void SortButtonRect(RECT* r) {
    static int cachedW = -1, cachedSort = -1, cachedLang = -1;
    const int sort = (int)InterlockedCompareExchange(&g_appSort, 0, 0);
    if (cachedW < 0 || cachedSort != sort || cachedLang != g_lang) {
        wchar_t txt[128];
        swprintf_s(txt, L"%s \x25BE",
                   T(sort == 1 ? STR_SORTBY_DATE : sort == 2 ? STR_SORTBY_CAT : STR_SORTBY_NAME));
        int w = ScI(160);
        HDC dc = GetDC(NULL);
        if (dc && g_fLabel) {
            HGDIOBJ old = SelectObject(dc, g_fLabel);
            SIZE sz = {};
            if (GetTextExtentPoint32W(dc, txt, (int)wcslen(txt), &sz)) w = sz.cx;
            SelectObject(dc, old);
        }
        if (dc) ReleaseDC(NULL, dc);
        cachedW = w; cachedSort = sort; cachedLang = g_lang;
    }
    r->left = LEFT + AppsTitleWidth() + ScI(16);
    r->top = ScI(60); r->right = r->left + cachedW + ScI(8); r->bottom = ScI(98);
}
static void AccountRect(RECT* r) {
    r->top = ScI(26);
    r->bottom = ScI(86);
    r->right = HeaderAvatarRight() + ScI(6);
    r->left = HeaderAvatarRight() - ScI(44) - ScI(10) - UserTextWidth() - ScI(10);
}

static void DismissUserMenu() {
    if (g_userMenuWnd) MenuFinish(g_userMenuWnd, MENU_END_CANCEL, -1);
}

static bool HibernateAvailable() {
    bool yes = false;
    SYSTEM_POWER_CAPABILITIES caps = {};
    if (GetPwrCapabilities(&caps)) yes = caps.SystemS4 != FALSE && caps.HiberFilePresent != FALSE;
    return yes;
}
static void ShowMenuFor(HWND owner, RECT anchor, int kind);
static void ShowUserMenu(HWND owner) {
    RECT anchor; AccountRect(&anchor);
    ShowMenuFor(owner, anchor, 0);
}
static void CharmClickActions(int hit, int row, int ly) {
    if (hit == CH_SCOPE) {
        g_charmDdOpen = !g_charmDdOpen;
        g_charmDdHover = -1;
        CharmRender(g_charmCont); RepaintNow();
    } else if (hit == CH_SCOPEITEM) {
        g_charmScope = row;
        g_charmDdOpen = false; g_charmDdHover = -1;
        RunSearch(); g_charmSel = 0; g_charmScroll = 0;
        CharmRender(g_charmCont); RepaintNow();
    } else if (hit == CH_DDCLOSE) {
        g_charmDdOpen = false; g_charmDdHover = -1;
        CharmRender(g_charmCont); RepaintNow();
    } else if (hit == CH_ROW) {
        LaunchResult(row);
    } else if (hit == CH_CLEAR) {
        g_queryLen = 0; g_query[0] = 0; RunSearch(); g_charmSel = 0;
        CharmRender(g_charmCont); RepaintNow();
    } else if (hit == CH_SUBMIT) {
        if (!g_results.empty()) LaunchResult(g_charmSel < (int)g_results.size() ? g_charmSel : 0);
    } else if (hit == CH_SCROLL) {
        RECT tr = CharmScrollTrack();
        int total = (int)g_results.size(), vis = CharmLayout().vis;
        int maxScroll = std::max(1, total - vis);
        g_charmScroll = (int)(Clamp01((float)(ly - tr.top) / (float)std::max(1, (int)(tr.bottom - tr.top))) * maxScroll);
        CharmRender(g_charmCont); RepaintNow();
    }
}

static void BuildMenuEntries(int kind, std::vector<UserMenuEntry>& out) {
    out.clear();
    if (kind == 1) {
        out.push_back({ T(STR_PWR_SLEEP), UM_SLEEP });
        if (HibernateAvailable())
            out.push_back({ T(STR_PWR_HIBERNATE), UM_HIBERNATE });
        out.push_back({ T(STR_PWR_SHUTDOWN), UM_SHUTDOWN });
        out.push_back({ T(STR_PWR_RESTART), UM_RESTART });
    } else if (kind == 4) {
        if (!g_ctxAppValid) return;
        const AppEntry& a = g_ctxApp;
        bool fileish = !a.parse.empty() && wcsstr(a.parse.c_str(), L"shell:") == NULL &&
                       wcsstr(a.parse.c_str(), L"ms-") == NULL;
        out.push_back({ T(STR_CTX_OPEN), CM_OPEN });
        if (a.charmCat == CC_APP && !AppTarget(a).empty())
            out.push_back({ IsPinned(AppTarget(a).c_str()) ? T(STR_CTX_UNPIN_START) : T(STR_BAR_PIN_START), CM_PINSTART });
        if (fileish) out.push_back({ T(STR_CTX_LOCATION), CM_LOCATION });
        if (fileish) out.push_back({ T(STR_CTX_RUNAS), CM_RUNAS });
        if (!a.parse.empty()) out.push_back({ T(STR_CTX_COPY), CM_COPY });
        out.push_back({ L"", MC_SEP_ID, false, false, true });
        out.push_back({ T(STR_SORT_NAME), UM_SORT_NAME });
        out.push_back({ T(STR_SORT_DATE), UM_SORT_DATE });
        out.push_back({ T(STR_SORT_CAT), UM_SORT_CAT });

    } else if (kind == 5) {
        const Tile& tile = g_tiles[g_ctxTile];
        SelectedItem item = CurrentSelection();
        out.push_back({ T(STR_CTX_UNPIN_START), TC_UNPIN });
        if (tile.kind == 1) out.push_back({ T(STR_CTX_UNINSTALL), TC_UNINSTALL });
        out.push_back({ (item.classic || item.computer) ? T(STR_CTX_NEWWINDOW) : T(STR_CTX_OPEN), TC_OPEN });
        AdvancedAction adv[4];
        int ac = AdvancedActions(adv);
        for (int i = 0; i < ac; ++i) out.push_back({ adv[i].label, adv[i].id });
        if (SupportsResize(tile)) {
            UserMenuEntry re;
            re.text = T(STR_SIZE_RESIZE); re.cmd = TC_RESIZE; re.arrow = true; re.sub = 6;
            out.push_back(re);
        }
    } else if (kind == 6) {
        const Tile& tile = g_tiles[g_ctxTile];
        int sizes[4];
        int count = SupportedSizes(tile, sizes);
        for (int i = 0; i < count; ++i)
            out.push_back({ SizeName(sizes[i]), TC_SIZE0 + sizes[i], sizes[i] == tile.cw, false });
    } else if (kind == 7) {
        out.push_back({ T(STR_CTX_NAME_GROUPS), UM_NAMEGROUPS });
    } else if (kind == 2) {
        out.push_back({ T(STR_SORTBY_NAME), UM_SORT_NAME });
        out.push_back({ T(STR_SORTBY_DATE), UM_SORT_DATE });
        out.push_back({ T(STR_SORTBY_CAT), UM_SORT_CAT });

    } else {
        out.push_back({ T(STR_MENU_PICTURE), UM_PICTURE });
        out.push_back({ T(STR_MENU_LOCK), UM_LOCK });
        out.push_back({ T(STR_MENU_SIGNOUT), UM_SIGNOUT });
    }
}

static void ShowMenuFor(HWND owner, RECT anchor, int kind) {
    if (g_inUserMenu) return;
    if (!EnsureUserMenuClass()) return;
    if (g_userMenuWnd) { HWND old = g_userMenuWnd; g_userMenuWnd = NULL; DestroyWindow(old); }
    CloseSubMenu();

    BuildMenuEntries(kind, g_userMenu);

    int w = MenuWidth();
    int hgt = MenuContentH(g_userMenu);
    int x = anchor.right - w;
    int y = anchor.bottom + 2;
    if (kind == 2 || kind == 5 || kind == 6 || kind == 7) x = anchor.left;
    PlaceUserMenu(x, y, w, hgt);

    g_inUserMenu = true;
    g_userMenuHover = -1;
    g_userMenuResult = UM_NONE;
    g_userMenuEndReason = MENU_END_NONE;

    HWND menu = CreateUserMenuWindow(owner, x, y, w, hgt);
    if (!menu) {
        Wh_Log(L"[UserMenu] could not create the context menu, err=%u", (unsigned)GetLastError());
        g_inUserMenu = false;
        return;
    }
    g_userMenuWnd = menu;
    SetWindowPos(menu, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetForegroundWindow(menu);
    SetFocus(menu);

    bool quitReceived = false;
    MSG msg;
    while (g_userMenuWnd) {
        BOOL ret = GetMessageW(&msg, NULL, 0, 0);
        if (ret == 0) {
            quitReceived = true;
            PostQuitMessage((int)msg.wParam);
            break;
        }
        if (ret == -1) {
            Wh_Log(L"[UserMenu] GetMessageW failed, err=%u", (unsigned)GetLastError());
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_userMenuWnd) {
        HWND mm = g_userMenuWnd;
        g_userMenuWnd = NULL;
        DestroyWindow(mm);
    }
    CloseSubMenu();
    g_inUserMenu = false;
    g_menuClosedTick = NowMs();
    const int cmd = g_userMenuResult;
    g_userMenuResult = UM_NONE;
    g_userMenuEndReason = MENU_END_NONE;
    g_userMenuHover = -1;
    if (quitReceived || !g_wnd) return;

    switch (cmd) {
    case UM_PICTURE:
        CloseStart();
        if ((INT_PTR)ShellExecuteW(NULL, L"open", L"ms-settings:accounts", NULL, NULL, SW_SHOWNORMAL) <= 32)
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.UserAccounts", NULL, SW_SHOWNORMAL);
        break;
    case UM_LOCK:
        CloseStart();
        LockWorkStation();
        break;
    case UM_SIGNOUT:
        CloseStart();
        ExitWindowsEx(EWX_LOGOFF, SHTDN_REASON_FLAG_PLANNED);
        break;
    case UM_NAMEGROUPS:
        EnterCustomize();
        break;
    case UM_SLEEP:
    case UM_HIBERNATE:
        CloseStart();
        if (!SetSuspendState(cmd == UM_HIBERNATE, FALSE, FALSE))
            Wh_Log(L"[Power] SetSuspendState failed, error %u", (unsigned)GetLastError());
        break;
    case UM_SHUTDOWN:
    case UM_RESTART:
        CloseStart();
        ExitWindowsEx(cmd == UM_RESTART ? EWX_REBOOT : (EWX_SHUTDOWN | EWX_POWEROFF),
                      SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
        break;
    case MC_SEP_ID: break;
    case CM_OPEN: if (g_ctxAppValid) LaunchEntry(g_ctxApp); break;
    case CM_PINSTART:
        if (g_ctxAppValid) {
            const AppEntry& a = g_ctxApp;
            const std::wstring target = AppTarget(a);
            if (!target.empty()) {
                const int existing = FindPinned(target.c_str());
                if (existing >= 0) UnpinTile(existing);
                else PinApp(a.name.c_str(), target);
            }
        }
        break;
    case CM_LOCATION:
    case CM_RUNAS:
    case CM_COPY:
        if (g_ctxAppValid) {
            const AppEntry& a = g_ctxApp;
            if (cmd == CM_COPY) {
                if (OpenClipboard(g_wnd)) {
                    EmptyClipboard();
                    size_t n = (a.parse.size() + 1) * sizeof(wchar_t);
                    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, n);
                    if (hg) {
                        void* m = GlobalLock(hg);
                        if (m) {
                            memcpy(m, a.parse.c_str(), n);
                            GlobalUnlock(hg);
                            SetClipboardData(CF_UNICODETEXT, hg);
                        } else {
                            GlobalFree(hg);
                        }
                    }
                    CloseClipboard();
                }
            } else if (cmd == CM_LOCATION) {
                std::wstring sel = L"/select,\"" + std::wstring(a.parse) + L"\"";
                ShellExecuteW(g_wnd, L"open", L"explorer.exe", sel.c_str(), NULL, SW_SHOWNORMAL);
            } else {
                ShellExecuteW(g_wnd, L"runas", a.parse.c_str(), NULL, NULL, SW_SHOWNORMAL);
            }
        }
        break;
    case UM_SORT_NAME: InterlockedExchange(&g_appSort, 0); RunSearch(); RepaintNow(); break;
    case UM_SORT_DATE: InterlockedExchange(&g_appSort, 1); RunSearch(); RepaintNow(); break;
    case UM_SORT_CAT: InterlockedExchange(&g_appSort, 2); RunSearch(); RepaintNow(); break;
    case TC_OPEN:
        if (g_ctxTile >= 0 && g_ctxTile < g_tileCount && g_tiles[g_ctxTile].alive) Launch(g_ctxTile);
        break;
    case TC_UNPIN:
        if (g_ctxTile >= 0 && g_ctxTile < g_tileCount) UnpinTile(g_ctxTile);
        break;
    case TC_UNINSTALL: {
        const bool packaged = g_ctxTile >= 0 && g_ctxTile < g_tileCount &&
                              g_tiles[g_ctxTile].style == PINNED_APP;
        CloseStart();
        if (packaged) {
            if ((INT_PTR)ShellExecuteW(NULL, L"open", L"ms-settings:appsfeatures", NULL, NULL, SW_SHOWNORMAL) <= 32)
                ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ProgramsAndFeatures", NULL, SW_SHOWNORMAL);
        } else {
            ShellExecuteW(NULL, L"open", L"control.exe", L"/name Microsoft.ProgramsAndFeatures", NULL, SW_SHOWNORMAL);
        }
        break;
    }
        default:
        if (cmd >= TC_SIZE0 && cmd < TC_SIZE0 + 4) ResizeTileTo(g_ctxTile, cmd - TC_SIZE0);
        else if (cmd >= 20 && cmd <= 25) ExecuteAdvanced(cmd);
        break;
    }
}

static bool CreateWallpaperPreview(const wchar_t* path, std::vector<BYTE>& output, int& outW, int& outH) {
    output.clear();
    outW = outH = 0;
    Gdiplus::Bitmap image(path, FALSE);
    if (image.GetLastStatus() != Gdiplus::Ok || !image.GetWidth() || !image.GetHeight())
        return false;
    int w = (int)image.GetWidth(), h = (int)image.GetHeight();
    if (w > 640) { h = (int)((double)h * 640.0 / (double)w + 0.5); w = 640; }
    if (w < 1 || h < 1) return false;
    Gdiplus::Bitmap small(w, h, PixelFormat32bppARGB);
    if (small.GetLastStatus() != Gdiplus::Ok) return false;
    {
        Gdiplus::Graphics graphics(&small);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        if (graphics.DrawImage(&image, 0, 0, w, h) != Gdiplus::Ok) return false;
    }
    Gdiplus::BitmapData data = {};
    Gdiplus::Rect rect(0, 0, w, h);
    if (small.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB,
                       &data) != Gdiplus::Ok) return false;
    output.resize((size_t)w * (size_t)h * 4);
    for (int y = 0; y < h; ++y)
        memcpy(output.data() + (size_t)y * w * 4,
               (const BYTE*)data.Scan0 + (ptrdiff_t)y * data.Stride, (size_t)w * 4);
    small.UnlockBits(&data);
    outW = w; outH = h;
    return true;
}

static DWORD WINAPI WallpaperThread(LPVOID) {
  RunGuarded(L"WallpaperThread", [] {
    HRESULT com = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput input;
    if (Gdiplus::GdiplusStartup(&token, &input, NULL) != Gdiplus::Ok) token = 0;
    wchar_t previous[32768] = { 1 };
    FILETIME previousTime = {};
    HANDLE events[2] = { g_wallStop, g_wallWake };
    for (;;) {
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (result != WAIT_OBJECT_0 + 1) break;
        ResetEvent(g_wallWake);
        if (!token) continue;
        wchar_t path[32768] = {};
        bool hasPath = GetWallpaperPath(path, sizeof(path) / sizeof(path[0])) != false;
        WIN32_FILE_ATTRIBUTE_DATA attributes = {};
        bool hasFile = hasPath && GetFileAttributesExW(path, GetFileExInfoStandard, &attributes) != 0;
        FILETIME stamp = {};
        if (hasFile) {
            if (wcscmp(path, previous) == 0 &&
                CompareFileTime(&attributes.ftLastWriteTime, &previousTime) == 0)
                continue;
            stamp = attributes.ftLastWriteTime;
        } else {
            path[0] = 0;
            if (!previous[0] && previous[1] == 0) continue;
        }
        std::vector<BYTE> pixels;
        int pw = 0, ph = 0;
        if (hasFile) {
            if (!CreateWallpaperPreview(path, pixels, pw, ph)) continue;
        } else {
            pixels.assign(4, 0);
            pw = ph = 1;
        }
        {
            SrwGuard lock(&g_wallLock, true);
            g_wallStaging.swap(pixels);
            g_wallStagingW = pw;
            g_wallStagingH = ph;
            g_wallStagedReady = true;
        }
        wcsncpy(previous, path, 32767);
        previous[32767] = 0;
        previousTime = stamp;
        PostMessageW(g_wnd, WM_APP_WALLPAPER_READY, 0, 0);
    }
    if (token) Gdiplus::GdiplusShutdown(token);
    if (SUCCEEDED(com)) CoUninitialize();
  });
    return 0;
}

static int ReadWallpaperStyle(bool* tileMode) {
    *tileMode = false;
    int style = 10;
    HKEY key = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        wchar_t value[16] = {};
        DWORD type = 0, size = sizeof(value);
        if (RegQueryValueExW(key, L"TileWallpaper", NULL, &type, (BYTE*)value, &size) == ERROR_SUCCESS && type == REG_SZ)
            *tileMode = value[0] == L'1';
        value[0] = 0; size = sizeof(value);
        if (RegQueryValueExW(key, L"WallpaperStyle", NULL, &type, (BYTE*)value, &size) == ERROR_SUCCESS && type == REG_SZ)
            style = _wtoi(value);
        RegCloseKey(key);
    }
    return style;
}

static COLORREF ReadClassicBackgroundColor() {
    COLORREF color = RGB(0, 0, 0);
    HKEY key = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        wchar_t value[32] = {};
        DWORD type = 0, size = sizeof(value);
        if (RegQueryValueExW(key, L"Background", NULL, &type, (BYTE*)value, &size) == ERROR_SUCCESS && type == REG_SZ) {
            int r = 0, g = 0, b = 0;
            if (swscanf_s(value, L"%d %d %d", &r, &g, &b) == 3)
                color = RGB(r & 255, g & 255, b & 255);
        }
        RegCloseKey(key);
    }
    return color;
}

static void RebuildDesktopPreview() {
    if (!g_gdipToken) return;
    bool tileMode = false;
    int style = ReadWallpaperStyle(&tileMode);
    COLORREF bg = ReadClassicBackgroundColor();
    float k = (float)g_sh / (float)TILE;
    if (k < 1.0f) k = 1.0f;
    int vw = (int)((float)g_sw / k + 0.5f);
    int vh = TILE;
    if (vw < 4) vw = 4;
    if (vh < 4) vh = 4;
    bool ok = false;
    std::vector<BYTE> virt;
    {
        Gdiplus::Bitmap frame(vw, vh, PixelFormat32bppPARGB);
        if (frame.GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Graphics g(&frame);
            g.Clear(Gdiplus::Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            SrwGuard lock(&g_wallLock, false);
            if (g_wallImageReady && g_wallImageW > 0 && g_wallImageH > 0 &&
                (size_t)g_wallImage.size() >= (size_t)g_wallImageW * g_wallImageH * 4) {
                Gdiplus::Bitmap src(g_wallImageW, g_wallImageH, g_wallImageW * 4,
                                    PixelFormat32bppARGB, g_wallImage.data());
                if (src.GetLastStatus() == Gdiplus::Ok) {
                    float iw = (float)g_wallImageW / k, ih = (float)g_wallImageH / k;
                    Gdiplus::RectF dest(0, 0, (float)vw, (float)vh);
                    if (tileMode) {
                        Gdiplus::ImageAttributes wrap;
                        wrap.SetWrapMode(Gdiplus::WrapModeTile);
                        g.DrawImage(&src, Gdiplus::RectF(0, 0, (float)vw, (float)vh),
                                    0, 0, iw, ih, Gdiplus::UnitPixel, &wrap);
                    } else if (style == 2) {
                        g.DrawImage(&src, dest);
                    } else if (style == 6) {
                        float s = std::min((float)vw / iw, (float)vh / ih);
                        float dw = iw * s, dh = ih * s;
                        g.DrawImage(&src, Gdiplus::RectF(((float)vw - dw) * 0.5f, ((float)vh - dh) * 0.5f, dw, dh));
                    } else if (style == 0) {
                        g.DrawImage(&src, Gdiplus::RectF(((float)vw - iw) * 0.5f, ((float)vh - ih) * 0.5f, iw, ih));
                    } else {
                        float s = std::max((float)vw / iw, (float)vh / ih);
                        float dw = iw * s, dh = ih * s;
                        g.DrawImage(&src, Gdiplus::RectF(((float)vw - dw) * 0.5f, ((float)vh - dh) * 0.5f, dw, dh));
                    }
                    ok = g.GetLastStatus() == Gdiplus::Ok;
                }
            } else {
                ok = true;
            }
            if (ok) {
                Gdiplus::BitmapData data = {};
                Gdiplus::Rect rect(0, 0, vw, vh);
                if (frame.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &data) == Gdiplus::Ok) {
                    virt.resize((size_t)vw * vh * 4);
                    for (int y = 0; y < vh; ++y)
                        memcpy(virt.data() + (size_t)y * vw * 4,
                               (const BYTE*)data.Scan0 + (ptrdiff_t)y * data.Stride, (size_t)vw * 4);
                    frame.UnlockBits(&data);
                } else ok = false;
            }
        }
    }
    if (!ok) return;
    g_wallVirt.swap(virt);
    g_wallVirtW = vw;
    g_wallVirtH = vh;
    g_wallVirtReady = true;
    int sq = vh < vw ? vh : vw;
    int sx = (vw - sq) / 2;
    for (int y = 0; y < TILE && y < sq; ++y)
        memcpy(g_wallPixels + (size_t)y * TILE * 4,
               g_wallVirt.data() + ((size_t)y * vw + sx) * 4, (size_t)sq * 4);
    g_wallReady = true;
}

static void ApplyWallpaperResult() {
    bool changed = false;
    {
        SrwGuard lock(&g_wallLock, true);
        if (g_wallStagedReady) {
            g_wallImage.swap(g_wallStaging);
            g_wallImageW = g_wallStagingW;
            g_wallImageH = g_wallStagingH;
            g_wallStagingW = g_wallStagingH = 0;
            g_wallStagedReady = false;
            g_wallImageReady = true;
            changed = true;
        }
    }
    if (!changed) return;
    RebuildDesktopPreview();
    if ((int)InterlockedCompareExchange(&g_bgMode, 0, 0) == 3)
        PostMessageW(g_wnd, WM_APP_REBUILD_BG, 0, 0);
    if (g_desktopAppGpu) { g_desktopAppGpu->Release(); g_desktopAppGpu = NULL; }
    RefreshDesktopTile();
    g_dirty = true;
    if (g_wnd && IsWindowVisible(g_wnd)) InvalidateRect(g_wnd, NULL, FALSE);
}

static LONGLONG RefreshPeriod(LONGLONG frequency) {
    DWM_TIMING_INFO info = {};
    info.cbSize = sizeof(info);
    if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &info)) && info.qpcRefreshPeriod) {
        LONGLONG period = (LONGLONG)info.qpcRefreshPeriod;
        if (period >= frequency / 360 && period <= frequency / 30) return period;
    }
    HDC dc = GetDC(NULL);
    int hz = dc ? GetDeviceCaps(dc, VREFRESH) : 0;
    if (dc) ReleaseDC(NULL, dc);
    if (hz < 30 || hz > 360) hz = 60;
    return frequency / hz;
}

static DWORD WINAPI FrameThread(LPVOID) {
  RunGuarded(L"FrameThread", [] {
    HANDLE timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                          TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!timer) timer = CreateWaitableTimerW(NULL, FALSE, NULL);
    if (!timer) return;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    HANDLE wakeHandles[2] = { g_frameStop, g_frameWake };
    HANDLE frameHandles[2] = { g_frameStop, timer };
    auto nextVBlank = [&](LONGLONG after, LONGLONG period) -> LONGLONG {
        DWM_TIMING_INFO info = {};
        info.cbSize = sizeof(info);
        if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &info)) && info.qpcVBlank && period > 0) {
            LONGLONG t = (LONGLONG)info.qpcVBlank;
            if (t <= after) t += ((after - t) / period + 1) * period;
            return t;
        }
        return after + period;
    };
    for (;;) {
        DWORD w = WaitForMultipleObjects(2, wakeHandles, FALSE, INFINITE);
        if (w != WAIT_OBJECT_0 + 1) break;
        LONGLONG period = RefreshPeriod(freq.QuadPart);
        LARGE_INTEGER target;
        QueryPerformanceCounter(&target);
        target.QuadPart = nextVBlank(target.QuadPart, period);
        while (WaitForSingleObject(g_frameWake, 0) == WAIT_OBJECT_0) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            if (target.QuadPart <= now.QuadPart) target.QuadPart = nextVBlank(now.QuadPart, period);
            LONGLONG delta = target.QuadPart - now.QuadPart;
            LARGE_INTEGER due;
            due.QuadPart = -(delta * 10000000LL / freq.QuadPart);
            if (due.QuadPart == 0) due.QuadPart = -1;
            if (!SetWaitableTimer(timer, &due, 0, NULL, NULL, FALSE)) break;
            if (WaitForMultipleObjects(2, frameHandles, FALSE, INFINITE) != WAIT_OBJECT_0 + 1)
                break;
            if (WaitForSingleObject(g_frameWake, 0) != WAIT_OBJECT_0) break;
            if (InterlockedCompareExchange(&g_framePending, 1, 0) == 0 &&
                !PostMessageW(g_wnd, WM_APP_FRAME, 0, 0))
                InterlockedExchange(&g_framePending, 0);
            target.QuadPart += period;
        }
        CancelWaitableTimer(timer);
        if (WaitForSingleObject(g_frameStop, 0) == WAIT_OBJECT_0) break;
    }
    CloseHandle(timer);
  });
    return 0;
}

static bool g_framesIdle = false;
static int g_idleTicks = 0;
static void StartFrames() {
    g_framesIdle = false; g_idleTicks = 0;
    if (g_frameThread && WaitForSingleObject(g_frameThread, 0) == WAIT_TIMEOUT) {
        SetEvent(g_frameWake);
    } else {
        SetTimer(g_wnd, 1, 8, NULL);
    }
}

static void StopFrames() {
    g_framesIdle = false;
    if (g_frameWake) ResetEvent(g_frameWake);
    KillTimer(g_wnd, 1);
}

// When nothing has animated for a while, drop the per-vblank pump and poll at 10 Hz;
// any input (or Tick noticing a change) restores the full-rate pump.
static void EnterIdleFrames(HWND h) {
    if (g_framesIdle || !g_frameWake) return;
    g_framesIdle = true;
    ResetEvent(g_frameWake);
    SetTimer(h, 1, 100, NULL);
}
static void ResumeFrames() {
    if (!g_framesIdle) return;
    KillTimer(g_wnd, 1);
    StartFrames();
}

static bool InteractiveDesktopReady() {
    HDESK input = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!input) return false;
    HDESK own = GetThreadDesktop(GetCurrentThreadId());
    wchar_t inputName[128] = {}, ownName[128] = {};
    DWORD size = 0;
    bool ready = own &&
        GetUserObjectInformationW(input, UOI_NAME, inputName, sizeof(inputName), &size) &&
        GetUserObjectInformationW(own, UOI_NAME, ownName, sizeof(ownName), &size) &&
        _wcsicmp(inputName, ownName) == 0;
    CloseDesktop(input);
    return ready;
}

static DWORD WINAPI LogonOpenThread(LPVOID) {
  RunGuarded(L"LogonOpenThread", [] {
    DWORD session = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &session)) return;
    LPWSTR buffer = NULL;
    DWORD bytes = 0;
    if (!WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, session, WTSSessionInfoEx, &buffer, &bytes))
        return;
    ULONGLONG loggedOn = 0;
    if (bytes >= sizeof(WTSINFOEXW)) {
        WTSINFOEXW* info = reinterpret_cast<WTSINFOEXW*>(buffer);
        if (info->Level == 1 && info->Data.WTSInfoExLevel1.SessionState == WTSActive)
            loggedOn = (ULONGLONG)info->Data.WTSInfoExLevel1.LogonTime.QuadPart;
    }
    WTSFreeMemory(buffer);
    if (!loggedOn) return;

    FILETIME nowFile = {}, created = {}, exited = {}, kernel = {}, user = {};
    GetSystemTimeAsFileTime(&nowFile);
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) return;
    ULARGE_INTEGER now = {}, started = {};
    now.LowPart = nowFile.dwLowDateTime; now.HighPart = nowFile.dwHighDateTime;
    started.LowPart = created.dwLowDateTime; started.HighPart = created.dwHighDateTime;
    const ULONGLONG second = 10000000ULL;
    if (now.QuadPart < loggedOn || now.QuadPart - loggedOn > 120ULL * second ||
        now.QuadPart < started.QuadPart || now.QuadPart - started.QuadPart > 45ULL * second ||
        started.QuadPart + 10ULL * second < loggedOn ||
        started.QuadPart > loggedOn + 120ULL * second) return;

    int stable = 0;
    for (int i = 0; i < 1900; ++i) {
        if (InteractiveDesktopReady()) {
            if (++stable >= 2) break;
        } else stable = 0;
        if (WaitForSingleObject(g_logonStop, 16) != WAIT_TIMEOUT) return;
    }
    if (stable < 2 || WaitForSingleObject(g_logonStop, 0) != WAIT_TIMEOUT) {
        Wh_Log(L"Sign-in Start: interactive desktop was not ready; skipped this session");
        return;
    }
    const std::wstring valueName = L"LastStartLogonTime-" + CurrentUserSid();
    ULONGLONG previous = 0;
    const size_t got = Wh_GetBinaryValue(valueName.c_str(), &previous, sizeof(previous));
    if (got == sizeof(previous) && previous == loggedOn) return;
    if (!Wh_SetBinaryValue(valueName.c_str(), &loggedOn, sizeof(loggedOn))) return;

    Wh_Log(L"Sign-in Start: interactive desktop ready; requesting opening reveal");
    if (g_wnd) PostMessageW(g_wnd, WM_APP_LOGON_OPEN, 0, 0);
  });
    return 0;
}

static void CaptureDesktop() {
    if (!g_snapDC) return;
    HDC sdc = GetDC(NULL);
    g_snapReady = BitBlt(g_snapDC, 0, 0, g_sw, g_sh, sdc, g_monX, g_monY, SRCCOPY | CAPTUREBLT) != 0;
    ReleaseDC(NULL, sdc);
    GdiFlush();
}

static void OpenStart(bool atLogon = false) {
    if (!g_wnd) return;
    if (g_appsRefresh && InterlockedCompareExchange(&g_appsReady, 0, 0) &&
        GetTickCount64() - g_catalogBuiltTick > kCatalogMaxAgeMs)
        SetEvent(g_appsRefresh);   // catalog is stale: rebuild apps (and files, if they were requested)
    // Open on the monitor that holds the cursor (the primary one at logon).
    int monX = 0, monY = 0;
    int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);
    int targetDpi = 0;
    {
        POINT cp = { 0, 0 };
        HMONITOR mon = NULL;
        if (atLogon || !GetCursorPos(&cp)) { cp.x = 0; cp.y = 0; }
        mon = MonitorFromPoint(cp, MONITOR_DEFAULTTOPRIMARY);
        if (mon) {
            UINT dx = 0, dy = 0;
            if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) targetDpi = (int)dx;
        }
        if (!atLogon && GetCursorPos(&cp)) {
            MONITORINFO mi = { sizeof(mi) };
            if (mon && GetMonitorInfoW(mon, &mi)) {
                monX = mi.rcMonitor.left; monY = mi.rcMonitor.top;
                sw = mi.rcMonitor.right - mi.rcMonitor.left;
                sh = mi.rcMonitor.bottom - mi.rcMonitor.top;
            }
        }
    }
    bool wasVisible = IsWindowVisible(g_wnd) != 0;
    bool dpiChanged = UpdateDpi(targetDpi);
    if (dpiChanged || sw != g_sw || sh != g_sh || monX != g_monX || monY != g_monY || !g_bgBmp) {
        if (wasVisible) ShowWindow(g_wnd, SW_HIDE);
        g_sw = sw; g_sh = sh; g_monX = monX; g_monY = monY;
        for (int i = 0; i < g_tileCount; ++i) g_tileGroup[i] = g_committedGroup[i];
        Relayout(false);
        ClampScroll();
        BuildBuffers();
        RebuildDesktopPreview();
        SetWindowPos(g_wnd, NULL, g_monX, g_monY, g_sw, g_sh,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
        wasVisible = false;
    }
    if (atLogon) g_snapReady = false;
    else if (!wasVisible) CaptureDesktop();
    RefreshDesktopTile();
    g_openSlideSeconds = (float)InterlockedCompareExchange(&g_openSlideMs, 0, 0) / 1000.0f;
    ComputePivot();
    for (int i = 0; i < MAX_TILES; i++) g_tileScale[i] = 1.0f;
    g_hover = g_pressed = -1;
    g_scrollCur = g_scrollTarget = 0.0f;
    g_view = VIEW_START;
    g_zoomOut = false; g_zoomTick = 0; g_hoverZoomBtn = false; g_hoverGroup = -1;
    g_dragGroup = -1; g_dragGroupMoved = false; g_dragGroupDx = 0.0f;
    g_namingGroup = -1; g_groupNameEdit.clear(); g_focus = -1; g_alphaGrid = false;
    g_charmOpen = false; g_charmTick = 0; g_leaveTick = 0; g_charmHover = -1; g_charmPressed = -1;
    SetCharmMouseHook(false);
    g_queryLen = 0; g_query[0] = 0;
    g_results.clear(); g_selected = 0;
    g_logonOpening = atLogon;
    if (atLogon) g_firstDesktopAfterLogon = true;
    g_logonVisualMs = 0;
    g_skipSnapshot = atLogon || !g_snapReady;
    g_returnTick = 0;
    g_dragIdx = -1; g_dragging = false;
    g_customize = false;
    g_lastMouseMs = NowMs();
    g_appsSearchFocus = false;
    g_barVisible = false; g_barMode = 0; g_barIdx = -1; g_advancedOpen = false; g_resizeOpen = false;
    g_barHoverButton = g_barPressedButton = g_advHoverRow = g_advPressedRow = -1;
    g_resizeHoverRow = g_resizePressedRow = -1;

    bool gpuReady = atLogon && g_rt && g_bgGpu ? true : InitGpu();
    if (!gpuReady) Wh_Log(L"Direct2D unavailable; using GDI fallback");
    if (gpuReady && atLogon) WarmLogonTileTextures();
    if (atLogon) Wh_Log(L"Sign-in reveal: desktop capture %ls, %d tile groups, GPU %ls",
                        L"skipped (direct return)", g_blockCount,
                        gpuReady ? L"ready" : L"fallback");
    g_openTick = g_lastTick = NowMs();
    g_open = true;
    g_closing = false;
    g_dirty = true;
    if (!gpuReady) Render();
    SetWindowPos(g_wnd, HWND_TOPMOST, g_monX, g_monY, g_sw, g_sh,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_wnd, NULL, FALSE);
    UpdateWindow(g_wnd);
    g_logonLastFrameTick = NowMs();
    g_logonRevealGateTick = atLogon ? g_logonLastFrameTick + 400 : 0;
    StartFrames();
    if (g_wallWake) SetEvent(g_wallWake);
}

static void CloseStart(bool immediate) {
    if (!g_open) return;
    bool instantDesktop = immediate && g_firstDesktopAfterLogon;
    if (instantDesktop) g_firstDesktopAfterLogon = false;
    g_open = false;
    if (g_namingGroup >= 0) CommitNaming();
    g_customize = false;
    HideBar();
    DismissUserMenu();
    if (g_dragging) EndDrag();
    g_logonOpening = false;
    if (instantDesktop || !g_snapReady) {
        g_closing = false;
        g_pressed = g_hover = -1;
        g_dirty = true;
        ShowWindow(g_wnd, SW_HIDE);
        StopFrames();
        return;
    }
    g_closing = true;
    g_closeSlideSeconds = (float)InterlockedCompareExchange(&g_closeSlideMs, 0, 0) / 1000.0f;
    g_closeTick = NowMs();
    g_pressed = -1;
    g_hover = -1;
    g_dirty = true;
}

static void Launch(int i) {
    const Tile& tile = g_tiles[i];
    const wchar_t* target = tile.target;
    bool desktop = tile.style == DESKTOP_REFERENCE ||
                   (target && _wcsicmp(target, kDesktopTarget) == 0);
    if (desktop) Wh_Log(L"Desktop tile: first-after-logon=%d, direct return=%d",
                        (int)g_firstDesktopAfterLogon, (int)(desktop && g_firstDesktopAfterLogon));
    CloseStart(desktop && g_firstDesktopAfterLogon);
    if (target && !desktop)
        ShellExecuteW(NULL, L"open", target, NULL, NULL, SW_SHOWNORMAL);
}

static int HitTile(int mx, int my) {
    POINT pt = { mx + (int)g_scrollCur, my };
    for (int i = g_tileCount - 1; i >= 0; i--) {
        if (!g_tiles[i].alive) continue;
        RECT r; TileRect(g_tiles[i], &r);
        if (PtInRect(&r, pt)) return i;
    }
    return -1;
}

static DWORD ArtRefreshAfterMs() {
    if (!g_logonOpening) return 1700 + (DWORD)g_blockCount * 33;
    float end = LOGON_TILE_DELAY + (float)(g_blockCount > 0 ? g_blockCount - 1 : 0) *
                LOGON_BLOCK_STAGGER + LOGON_TILE_T + 0.10f;
    return (DWORD)(end * 1000.0f);
}

static void Tick() {
    DWORD now = NowMs();
    float dt = (float)(now - g_lastTick) / 1000.0f;
    if (dt > 0.1f) dt = 0.1f;
    g_lastTick = now;
    bool changed = false;
    ScrollArrowTick();

    if (g_leaveTick) {
        if ((float)(now - g_leaveTick) >= LEAVE_MS) CommitAllApps();
        changed = true;
    }
    if (g_charmTick && (DWORD)(now - g_charmTick) <= CHARM_SLIDE_MS + 80) changed = true;
    if (g_charmOpen) {
        int phase = (int)(now / 500);
        if (phase != g_caretPhase) {
            g_caretPhase = phase;
            CharmRender(g_charmCont); SuppressOtherSearchCharm();
            changed = true;
        }
    }
    if (g_zoomTick && (float)(now - g_zoomTick) / 1000.0f < (ZOOM_MS + 80.0f) / 1000.0f) changed = true;
    if (g_namingGroup >= 0) changed = true;
    if (g_alphaGrid && (DWORD)(now - g_alphaTick) < 260) changed = true;
    {
        float idf = ChromeIdleFade(now);
        if (idf > 0.0f && idf < 1.0f) changed = true;
    }
    if (g_view == VIEW_SEARCH && g_appsSearchFocus && !g_charmOpen) {
        int ph = (int)(now / 500);
        if (ph != g_caretPhase) { g_caretPhase = ph; changed = true; }
    }
    if (g_view == VIEW_SEARCH) {
        if ((float)(now - g_sbTick) / 1000.0f < g_sbDur + 0.03f ||
            (float)(now - g_searchTick) / 1000.0f < RESULT_FADE + 0.1f) changed = true;
    } else if (g_returnTick && (float)(now - g_returnTick) / 1000.0f < BG_BACK + 0.03f) {
        changed = true;
    }

    if (g_closing) {
        float tc = (float)(now - g_closeTick) / 1000.0f;
        if (tc >= g_closeSlideSeconds + 0.02f) {
            g_closing = false;
            ShowWindow(g_wnd, SW_HIDE);
            StopFrames();
            return;
        }
        changed = true;
    } else {
        float end = g_logonOpening ? 0.0f : OPEN_HOLD + g_openSlideSeconds;
        float tileEnd = g_logonOpening ?
            LOGON_TILE_DELAY + (float)(g_blockCount > 0 ? g_blockCount - 1 : 0) * LOGON_BLOCK_STAGGER + LOGON_TILE_T :
            TILES_DELAY + (float)(g_blockCount > 0 ? g_blockCount - 1 : 0) * BLOCK_STAGGER + TILE_T;
        if (end < tileEnd) end = tileEnd;
        float titleEnd = g_logonOpening ? LOGON_TITLE_DELAY + LOGON_TITLE_FADE : HEADER_DELAY + HEADER_FADE;
        if (end < titleEnd) end = titleEnd;
        if (g_logonOpening) {
            if (end < LOGON_ACCOUNT_DELAY + LOGON_ACCOUNT_MOVE)
                end = LOGON_ACCOUNT_DELAY + LOGON_ACCOUNT_MOVE;
            if (end < LOGON_TITLE_DELAY + LOGON_TITLE_MOVE)
                end = LOGON_TITLE_DELAY + LOGON_TITLE_MOVE;
        }
        if (g_logonOpening && end < LOGON_GLASS_DELAY + LOGON_GLASS_FADE)
            end = LOGON_GLASS_DELAY + LOGON_GLASS_FADE;
        if ((float)OpeningElapsedMs(now) / 1000.0f < end + 0.1f) changed = true;
    }

    if (fabsf(g_scrollTarget - g_scrollCur) > 0.3f) {
        float k = dt * 12.0f; if (k > 1.0f) k = 1.0f;
        g_scrollCur += (g_scrollTarget - g_scrollCur) * k;
        changed = true;
    } else if (g_scrollCur != g_scrollTarget) { g_scrollCur = g_scrollTarget; changed = true; }

    for (int i = 0; i < g_tileCount; i++) {
        float want = g_dragging ? (i == g_dragIdx ? 1.0f : 0.85f) :
                     ((g_pressed == i && g_hover == i) ? PRESS_SCALE : 1.0f);
        if (fabsf(g_tileScale[i] - want) > 0.002f) {
            float k = dt * 22.0f; if (k > 1.0f) k = 1.0f;
            g_tileScale[i] += (want - g_tileScale[i]) * k;
            changed = true;
        } else g_tileScale[i] = want;
    }

    {
        float k = 1.0f - expf(-dt * 16.0f);
        for (int i = 0; i < g_tileCount; ++i) {
            Tile& tl = g_tiles[i];
            if (!tl.alive || i == g_dragIdx) continue;
            float dx = tl.tx - tl.px, dy = tl.ty - tl.py;
            if (fabsf(dx) > 0.3f || fabsf(dy) > 0.3f) { tl.px += dx * k; tl.py += dy * k; changed = true; }
            else if (dx != 0.0f || dy != 0.0f) { tl.px = tl.tx; tl.py = tl.ty; changed = true; }
        }
        if (g_barVisible && (float)(now - g_barTick) / 1000.0f < 0.2f) changed = true;
    }

    if (changed) { g_idleTicks = 0; g_dirty = true; InvalidateRect(g_wnd, NULL, FALSE); }
    else ++g_idleTicks;
}

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    return RunGuardedValue<LRESULT>(L"WndProc", 0, [&]() -> LRESULT {
    if (msg != WM_TIMER && msg != WM_PAINT && msg != WM_APP_FRAME &&
        msg != WM_NCHITTEST && msg != WM_SETCURSOR && msg != WM_ERASEBKGND) {
        g_idleTicks = 0;
        ResumeFrames();
    }
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        if (!g_rt || !RenderGpu()) {
            if (g_dirty) Render();
            if (g_bbDC) BitBlt(dc, 0, 0, g_sw, g_sh, g_bbDC, 0, 0, SRCCOPY);
        }
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_SETTINGCHANGE:
        if (wp == SPI_SETDESKWALLPAPER && g_wallWake) SetEvent(g_wallWake);
        break;
    case WM_APP_WALLPAPER_READY:
        ApplyWallpaperResult();
        return 0;
    case WM_DPICHANGED:
        UpdateDpi();
        Relayout(g_open && !g_closing);
        ClampScroll();
        BuildBuffers();
        InitGpu();
        RepaintNow();
        return 0;
    case WM_APP_REBUILD_BG:
        if (InterlockedExchange(&g_tileColorsDirty, 0)) UpdateTileAppColors();
        BuildBuffers();
        InitGpu();
        RepaintNow();
        return 0;
    case WM_APP_GROUP_SETTINGS:
        if (g_orderCount) {
            for (int i = 0; i < g_tileCount; ++i) g_tileGroup[i] = g_committedGroup[i];
            Relayout(g_open && !g_closing);
            ClampScroll();
            RepaintNow();
        }
        return 0;
    case WM_APP_FRAME:
        InterlockedExchange(&g_framePending, 0);
        AdvanceLogonClock();
        SyncDragToCursor(h);
        if (g_open || g_closing) Tick();
        if (g_open && !g_closing && !g_dragging && !g_logonOpening && g_idleTicks > 45) EnterIdleFrames(h);
        if (g_artRebuildPending && !g_closing &&
            (!g_open || OpeningElapsedMs(NowMs()) > ArtRefreshAfterMs())) {
            ApplyPackageArtChanges();
        }
        if (g_dirty && IsWindowVisible(h)) UpdateWindow(h);
        return 0;
    case WM_TIMER:
        AdvanceLogonClock();
        SyncDragToCursor(h);
        Tick();
        if (g_framesIdle && g_idleTicks == 0) ResumeFrames();
        if (g_artRebuildPending && !g_closing &&
            (!g_open || OpeningElapsedMs(NowMs()) > ArtRefreshAfterMs())) {
            ApplyPackageArtChanges();
        }
        if (g_dirty && IsWindowVisible(h)) UpdateWindow(h);
        return 0;
    case WM_APP_LOGON_OPEN:
        if (InterlockedCompareExchange(&g_openAtLogon, 0, 0) != 0) {
            g_firstDesktopAfterLogon = true;
            if (!g_userToggledStart && !g_open && !g_closing) {
                Wh_Log(L"Opening Start once after sign-in with the 8102-style tile reveal");
                OpenStart(true);
            }
        }
        return 0;
    case WM_APP_TOGGLE: {
        DWORD now = NowMs();
        if (now - g_lastToggleTick < 250) return 0;
        g_lastToggleTick = now;
        g_userToggledStart = true;
        if (g_open) { if (g_view == VIEW_SEARCH) LeaveSearch(); else CloseStart(); } else OpenStart();
        return 0;
    }
    case WM_APP_CHARM_POINTER: {
        if (g_open && !g_closing && g_charmOpen && !g_userMenuWnd) {
            POINT sp = { (LONG)(LONG_PTR)wp, (LONG)(LONG_PTR)lp };
            POINT cp = sp;
            ScreenToClient(h, &cp);
            RECT pr; CharmPaneRect(&pr);
            const int visLeft = pr.left + (int)((1.0f - EaseMetroOut(CharmPresence(NowMs()))) * (float)g_charmW);
            if (!(cp.x >= visLeft && PtInRect(&pr, cp))) {
                CloseCharm(false);
                if (WindowFromPoint(sp) == h) {
                    g_swallowCharmUp = true;
                    g_charmSwallowTick = NowMs();
                }
            }
        }
        return 0;
    }
    case WM_APP_CLOSE: CloseStart(); return 0;
    case WM_APP_ALL_APPS:
        if (g_open && !g_closing) {
            if (g_view == VIEW_SEARCH && g_queryLen == 0) LeaveSearch();
            else EnterAllApps();
        }
        return 0;
    case WM_KEYDOWN:
        if (g_open && !g_closing && !g_inUserMenu) {
            if (g_namingGroup >= 0 && (wp == VK_BACK || wp == VK_RETURN || wp == VK_ESCAPE))
                NamingKey((UINT)wp);
            else if (g_customize) { if (wp == VK_ESCAPE) ExitCustomize(); }
            else if (wp == VK_BACK || wp == VK_RETURN || wp == VK_UP || wp == VK_DOWN ||
                     wp == VK_LEFT || wp == VK_RIGHT || wp == VK_ESCAPE)
                OnSearchKey((UINT)wp);
            else if ((wp >= L'A' && wp <= L'Z') || (wp >= L'0' && wp <= L'9'))
                OnSearchChar((wchar_t)(wp + 32));
            else if (wp == VK_SPACE) OnSearchChar(L' ');
        }
        return 0;
    case WM_APP_CHAR:
        if (g_open && !g_closing) {
            if (g_namingGroup >= 0) {
                NamingChar((wchar_t)wp);
            } else if (!g_customize) {
                OnSearchChar((wchar_t)wp);
            }
        }
        return 0;
    case WM_APP_KEY:
        if (g_open && !g_closing) {
            if (g_namingGroup >= 0 && (wp == VK_BACK || wp == VK_RETURN || wp == VK_ESCAPE)) {
                NamingKey((UINT)wp);
            } else if (g_customize) {
                if (wp == VK_ESCAPE) ExitCustomize();
            } else {
                OnSearchKey((UINT)wp);
            }
        }
        return 0;
    case WM_APP_SEED_LAYOUT:
        if (InterlockedCompareExchange(&g_appsReady, 0, 0)) SeedStartLayout();
        return 0;
    case WM_APP_APPS_READY:
        RunSearch();
        if (InterlockedCompareExchange(&g_seedPending, 0, 0)) SeedStartLayout();
        if (g_charmOpen) CharmRender(g_charmCont);
        if (g_open) RepaintNow();
        return 0;
    case WM_APP_APPS_REFRESH: {
        std::vector<AppEntry>* fresh = NULL;
        AcquireSRWLockExclusive(&g_appsLock);
        fresh = g_pendingApps;
        g_pendingApps = NULL;
        if (fresh) {
            g_apps.swap(*fresh);
            for (IconSlot& sl : g_icons) {
                if (sl.gpu) { sl.gpu->Release(); sl.gpu = NULL; }
                delete[] sl.px; sl.px = NULL;
            }
            g_icons.clear();
            g_icons.resize(g_apps.size());
        }
        ReleaseSRWLockExclusive(&g_appsLock);
        const bool haveFresh = fresh != NULL;
        delete fresh;
        if (g_iconBitmapCache) g_iconBitmapCache->clear();
        if (g_iconsGo) SetEvent(g_iconsGo);
        if (haveFresh) {
            RunSearch();
            if (g_selected >= (int)g_results.size()) g_selected = g_results.empty() ? -1 : 0;
            if (g_charmOpen) CharmRender(g_charmCont);
            if (g_open) RepaintNow();
        }
        return 0;
    }
    case WM_APP_FILES_READY: {
        std::vector<AppEntry>* files = NULL;
        AcquireSRWLockExclusive(&g_appsLock);
        files = g_pendingFiles;
        g_pendingFiles = NULL;
        if (files) {
            for (AppEntry& a : *files) g_apps.push_back(std::move(a));
            g_icons.resize(g_apps.size());
        }
        ReleaseSRWLockExclusive(&g_appsLock);
        if (files) {
            delete files;
            Wh_Log(L"Search: %u catalog entries including files", (unsigned)g_apps.size());
            RunSearch();
            if (g_charmOpen) CharmRender(g_charmCont);
            if (g_open) RepaintNow();
        }
        return 0;
    }
    case WM_APP_ICON_READY:
        if (g_iconBitmapCache) g_iconBitmapCache->clear();
        // Tile accent colours are the only thing that depends on icons; skip the
        // rebuild entirely when appTileColors is off.
        if (g_appTileColors) {
            UpdateTileAppColors();
            if (g_open && !g_closing)
                PostMessageW(g_wnd, WM_APP_REBUILD_BG, 0, 0);
        }
        if (g_view == VIEW_SEARCH && g_open) RepaintNow();
        return 0;
    case WM_APP_PACKAGE_ART:
        g_artRebuildPending = true;
        if (!g_open && !g_closing) {
            ApplyPackageArtChanges();
        } else if (g_open && !g_closing && OpeningElapsedMs(NowMs()) > ArtRefreshAfterMs()) {
            ApplyPackageArtChanges();
        }
        return 0;
    case WM_MOUSEMOVE: {
        if (!g_tracking) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, h, 0 };
            TrackMouseEvent(&tme);
            g_tracking = true;
        }
        int mx = (short)LOWORD(lp), my = (short)HIWORD(lp);
        {
            DWORD mn = NowMs();
            bool wasFaded = ChromeIdleFade(mn) < 1.0f;
            g_lastMouseMs = mn;
            if (wasFaded) RepaintNow();
        }
        {
            RECT sth = ScrollThumbRect(), str = ScrollTrackRect();
            POINT spt = { mx, my };
            bool shov = ScrollBarVisible() && (PtInRect(&sth, spt) || PtInRect(&str, spt));
            if (g_sbDrag) { ScrollSetFromX(mx); RepaintNow(); }
            else if (shov != g_sbHover) { g_sbHover = shov; RepaintNow(); }
        }
        {
            RECT ar; AccountRect(&ar);
            POINT pt = { mx, my };
            if (g_charmOpen) {
                RECT pr; CharmPaneRect(&pr);
                if (PtInRect(&pr, pt)) {
                    int lx = pt.x - pr.left - g_charmCont, ly = pt.y - pr.top, row = -1;
                    int hit = CharmHit(lx, ly, &row);
                    if (g_charmPressed == CH_SCROLL) {
                        RECT tr = CharmScrollTrack();
                        int total = (int)g_results.size(), vis = CharmLayout().vis;
                        int maxScroll = std::max(1, total - vis);
                        int ns = (int)(Clamp01((float)(ly - tr.top) / (float)std::max(1, (int)(tr.bottom - tr.top))) * maxScroll);
                        if (ns != g_charmScroll) { g_charmScroll = ns; CharmRender(g_charmCont); RepaintNow(); }
                        return 0;
                    }
                    int nh = (hit == CH_ROW) ? row : (hit == CH_SCROLL ? -2 : (hit == CH_SCOPE ? -3 : -1));
                    int ddHov = (hit == CH_SCOPEITEM) ? row : -1;
                    bool dirty = false;
                    if (ddHov != g_charmDdHover) { g_charmDdHover = ddHov; dirty = true; }
                    if (nh != g_charmHover) { g_charmHover = nh; dirty = true; }
                    if (dirty) CharmRender(g_charmCont);
                    SetCursor(LoadCursorW(NULL, (hit == CH_SCOPE || hit == CH_SCOPEITEM || hit == CH_ROW)
                                                ? IDC_HAND : IDC_ARROW));
                    return 0;
                }
                if (g_charmHover != -1 || g_charmDdHover != -1) { g_charmHover = -1; g_charmDdHover = -1; CharmRender(g_charmCont); }
            }
            bool over = g_open && !g_closing && g_view == VIEW_START && PtInRect(&ar, pt) != FALSE;
            if (over != g_hoverAccount) { g_hoverAccount = over; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
            RECT pw, sc, arw, srt; HeaderButtonRects(&pw, &sc);
            AppsArrowRect(&arw); SortButtonRect(&srt);
            const bool inStart = g_view == VIEW_START;
            const bool inApps = g_view == VIEW_SEARCH && g_queryLen == 0;
            const bool zActive = g_open && !g_closing && ZoomPresence(NowMs()) > 0.0f;
            bool overPwr = g_open && !g_closing && inStart && !zActive && PtInRect(&pw, pt) != FALSE;
            bool overSrch = g_open && !g_closing && inStart && !zActive && PtInRect(&sc, pt) != FALSE;
            bool overArw = g_open && !g_closing && (inStart || inApps) && !zActive && PtInRect(&arw, pt) != FALSE;
            RECT zr; ZoomButtonRect(&zr);
            bool overZoom = g_open && !g_closing && inStart && !zActive && PtInRect(&zr, pt) != FALSE;
            if (overZoom != g_hoverZoomBtn) { g_hoverZoomBtn = overZoom; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
            if (overPwr != g_hoverPwr || overSrch != g_hoverSrch || overArw != g_hoverArrow) {
                g_hoverPwr = overPwr; g_hoverSrch = overSrch; g_hoverArrow = overArw;
                g_dirty = true; InvalidateRect(h, NULL, FALSE);
            }
            bool overBtn = overPwr || overSrch || overArw || overZoom ||
                           (inApps && PtInRect(&srt, pt) != FALSE);
            SetCursor(LoadCursorW(NULL, (over || overBtn) ? IDC_HAND : IDC_ARROW));
        }
        if (g_barVisible) {
            UpdateBarHover(mx, my);
            if (BarPoint(mx, my)) {
                if (g_hover != -1) { g_hover = -1; RepaintNow(); }
                return 0;
            }
        }
        if (g_view == VIEW_SEARCH) {
            int r = g_open ? HitResult(mx, my) : -1;
            if (r >= 0 && r != g_selected) { g_selected = r; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
            int hh = (g_open && g_alphaGrid == false) ? HitHeading(mx, my) : -1;
            SetCursor(LoadCursorW(NULL, (r >= 0 || hh >= 0) ? IDC_HAND : IDC_ARROW));
            return 0;
        }
        {
            int mxp = (short)LOWORD(lp), myp = (short)HIWORD(lp);
            if (g_dragGroup >= 0) {
                float wx, wy; ScreenToWorld(mxp, myp, &wx, &wy);
                float ddx = wx - g_dragGroupAnchorWX;
                if (!g_dragGroupMoved && fabsf(ddx) > ScF(10)) g_dragGroupMoved = true;
                if (g_dragGroupMoved) {
                    g_dragGroupDx = ddx;
                    MaybeSwapDraggedGroup();
                    g_dirty = true; InvalidateRect(h, NULL, FALSE);
                }
                return 0;
            }
            if (g_dragging) { UpdateDrag(mxp, myp); return 0; }
            if (g_open && g_pressed >= 0 && (wp & MK_LBUTTON)) {
                int ddx = mxp - g_pressX, ddy = myp - g_pressY;
                if (ddx * ddx + ddy * ddy > 64) { BeginDrag(g_pressed, g_pressX, g_pressY); UpdateDrag(mxp, myp); return 0; }
            }
        }
        if (ZoomPresence(NowMs()) > 0.5f) {
            if (g_hover != -1) { g_hover = -1; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
            float wx, wy; ScreenToWorld((short)LOWORD(lp), (short)HIWORD(lp), &wx, &wy);
            int gh = (g_open && !g_closing && g_view == VIEW_START && !g_charmOpen) ? GroupAtWorld(wx, wy) : -1;
            if (gh != g_hoverGroup) { g_hoverGroup = gh; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
            SetCursor(LoadCursorW(NULL, gh >= 0 ? IDC_HAND : IDC_ARROW));
            return 0;
        }
        int hv = g_open ? HitTile((short)LOWORD(lp), (short)HIWORD(lp)) : -1;
        if (hv != g_hover) { g_hover = hv; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
        return 0;
    }
    case WM_MOUSELEAVE:
        g_tracking = false;
        if (g_hoverAccount) { g_hoverAccount = false; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
        if (g_hoverPwr || g_hoverSrch || g_hoverArrow) {
            g_hoverPwr = g_hoverSrch = g_hoverArrow = false;
            g_dirty = true; InvalidateRect(h, NULL, FALSE);
        }
        if (g_hoverZoomBtn || g_hoverGroup >= 0) {
            g_hoverZoomBtn = false; g_hoverGroup = -1;
            g_dirty = true; InvalidateRect(h, NULL, FALSE);
        }
        if (g_barVisible) {
            g_barHoverButton = g_advHoverRow = g_resizeHoverRow = -1;
            RepaintNow();
        }
        if (g_hover != -1) { g_hover = -1; g_dirty = true; InvalidateRect(h, NULL, FALSE); }
        return 0;
    case WM_RBUTTONDOWN:
        if (g_swallowCharmUp && NowMs() - g_charmSwallowTick < 500) return 0;
        if (g_userMenuWnd) { g_swallowMenuUp = true; DismissUserMenu(); return 0; }
                if (g_open && !g_closing) {
                    POINT sp = { (short)LOWORD(lp), (short)HIWORD(lp) };
                    ClientToScreen(h, &sp);
                    if (DismissCompanionSearchCharm(sp, true)) {
                        g_swallowCharmUp = true;
                        return 0;
                    }
                }
                if (g_open && !g_closing && g_charmOpen) {
                    POINT rpt = { (short)LOWORD(lp), (short)HIWORD(lp) };
                    RECT pr; if (CharmPaneRect(&pr) && PtInRect(&pr, rpt)) {
                        int lx = rpt.x - pr.left - g_charmCont, ly = rpt.y - pr.top, row = -1;
                        if (CharmHit(lx, ly, &row) == CH_ROW && row >= 0) {
                            g_charmSel = row; g_ctxRow = row;
                            g_ctxAppValid = row < (int)g_results.size();
                            if (g_ctxAppValid) g_ctxApp = g_apps[g_results[row]];
                            RECT an = { rpt.x, rpt.y, rpt.x, rpt.y };
                            ShowMenuFor(h, an, 4);
                            CharmRender(g_charmCont); RepaintNow();
                        }
                        return 0;
                    }
                    CloseCharm(false);
                    g_swallowCharmUp = true;
                    return 0;
                }
        return 0;
    case WM_LBUTTONDOWN:
        if (g_swallowCharmUp && NowMs() - g_charmSwallowTick < 500) { g_pressed = -1; return 0; }
        if (g_userMenuWnd) {
            RECT ar; AccountRect(&ar);
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            g_swallowAccountUp = PtInRect(&ar, pt) != 0;
            g_swallowMenuUp = true;
            DismissUserMenu();
            return 0;
        }
        g_swallowAccountUp = false;
        if (g_open && !g_closing) {
            POINT sp = { (short)LOWORD(lp), (short)HIWORD(lp) };
            ClientToScreen(h, &sp);
            if (DismissCompanionSearchCharm(sp, true)) {
                g_swallowCharmUp = true;
                return 0;
            }
        }
        if (g_open && !g_closing && g_charmOpen) {
            POINT cpt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT cpr; CharmPaneRect(&cpr);
            const int visLeft = cpr.left + (int)((1.0f - EaseMetroOut(CharmPresence(NowMs()))) * (float)g_charmW);
            if (cpt.x >= visLeft && PtInRect(&cpr, cpt)) {
                int lx = cpt.x - cpr.left - g_charmCont, ly = cpt.y - cpr.top, row = -1;
                int hit = CharmHit(lx, ly, &row);
                if (hit == CH_CLEAR) {
                    CharmClickActions(hit, row, ly);
                    g_charmPressed = -1;
                    return 0;
                }
                g_charmPressed = (hit == CH_ROW) ? row : hit;
                return 0;
            }
            CloseCharm(false);
            g_swallowCharmUp = true;
            return 0;
        }
        if (!g_open) return 0;
        if (g_namingGroup >= 0 && !g_customize && !g_closing && g_view == VIEW_START) {
            POINT xp = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT fr;
            if (GroupNameFieldRect(g_namingGroup, &fr) && PtInRect(&fr, xp)) {
                g_swallowNamingUp = true;
                return 0;
            }
            CommitNaming();
        }
        if (g_customize && !g_closing && g_view == VIEW_START) {
            POINT xn = { (short)LOWORD(lp), (short)HIWORD(lp) };
            for (int g = 0; g < g_groupCount; ++g) {
                RECT fr;
                if (GroupNameFieldRect(g, &fr) && PtInRect(&fr, xn)) {
                    if (g_namingGroup >= 0 && g_namingGroup != g) CommitNaming();
                    if (g_namingGroup != g) StartNaming(g);
                    g_swallowNamingUp = true;
                    return 0;
                }
            }
            if (g_namingGroup >= 0) CommitNaming();
            else ExitCustomize();
            return 0;
        }
        if (ScrollBarVisible()) {
            POINT spt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT sal = ScrollArrowRect(false), sar = ScrollArrowRect(true);
            RECT sth = ScrollThumbRect(), str = ScrollTrackRect();
            if (PtInRect(&sal, spt) || PtInRect(&sar, spt)) {
                g_sbArrowDir = PtInRect(&sal, spt) ? -1 : 1;
                g_sbArrowNext = NowMs() + 400;
                g_sbArrowPrev = NowMs();
                ScrollStep(g_sbArrowDir * ScI(50));
                SetCapture(h);
                return 0;
            }
            if (PtInRect(&sth, spt)) {
                g_sbDrag = true; g_sbDragOff = spt.x - sth.left;
                SetCapture(h); RepaintNow(); return 0;
            }
            if (PtInRect(&str, spt)) {
                float page = (float)(str.right - str.left);
                ScrollStep(spt.x < sth.left ? -page : page);
                return 0;
            }
        }
        if (g_barVisible && BarMouseDown((short)LOWORD(lp), (short)HIWORD(lp))) {
            g_pressed = -1;
            SetCapture(h);
            return 0;
        }
        if (g_view == VIEW_START && !g_charmOpen && !g_barVisible && ZoomPresence(NowMs()) > 0.5f) {
            float wx, wy; ScreenToWorld((short)LOWORD(lp), (short)HIWORD(lp), &wx, &wy);
            int zg = GroupAtWorld(wx, wy);
            if (zg >= 0) {
                g_dragGroup = zg; g_dragGroupMoved = false;
                g_dragGroupAnchorWX = wx; g_dragGroupDx = 0.0f;
                SetCapture(h);
            }
            return 0;
        }
        if (g_view == VIEW_SEARCH) return 0;
        g_pressed = g_hover;
        if (g_hover >= 0) g_focus = g_hover;
        g_pressX = (short)LOWORD(lp); g_pressY = (short)HIWORD(lp);
        SetCapture(h);
        return 0;
    case WM_LBUTTONUP: {
        ReleaseCapture();
        if (g_swallowMenuUp) { g_swallowMenuUp = false; return 0; }
        if (g_swallowCharmUp) { g_swallowCharmUp = false; g_pressed = -1; g_charmPressed = -1; return 0; }
        if (g_swallowNamingUp) { g_swallowNamingUp = false; return 0; }
        if (g_sbDrag) { g_sbDrag = false; RepaintNow(); return 0; }
        if (g_sbArrowDir) { g_sbArrowDir = 0; RepaintNow(); return 0; }
        {
            POINT zpt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            if (g_open && !g_closing && !g_barVisible && g_view == VIEW_START && !g_charmOpen) {
                float zk = ZoomPresence(NowMs());
                RECT zr; ZoomButtonRect(&zr);
                if (zk > 0.5f) {
                    if (g_dragGroup >= 0) {
                        int zg = g_dragGroup; bool moved = g_dragGroupMoved;
                        g_dragGroup = -1; g_dragGroupMoved = false; g_dragGroupDx = 0.0f;
                        if (moved) { SaveLayout(); SaveGroupNames(); RepaintNow(); }
                        else ExitZoom(zg);
                        return 0;
                    }
                    float wx, wy; ScreenToWorld(zpt.x, zpt.y, &wx, &wy);
                    ExitZoom(GroupAtWorld(wx, wy));
                    return 0;
                }
            }
        }
        {
            RECT ar; AccountRect(&ar);
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            {
                RECT pw, sc, arw, srt; HeaderButtonRects(&pw, &sc);
                AppsArrowRect(&arw); SortButtonRect(&srt);
                if (g_open && !g_closing && g_charmOpen) {
                    RECT pr; CharmPaneRect(&pr);
                    if (PtInRect(&pr, pt)) {
                        int lx = pt.x - pr.left - g_charmCont, ly = pt.y - pr.top, row = -1;
                        int hit = CharmHit(lx, ly, &row);
                        g_charmPressed = (hit == CH_ROW) ? row : hit;
                        CharmClickActions(hit, row, ly);
                        g_charmPressed = -1;
                        return 0;
                    }
                    CloseCharm(false);
                    return 0;
                }
                if (g_open && !g_closing && g_view == VIEW_START && PtInRect(&sc, pt)) {
                    g_pressed = -1; EnterSearchEmpty(); return 0;
                }
                if (g_open && !g_closing && g_view == VIEW_START && PtInRect(&pw, pt)) {
                    g_pressed = -1; ShowMenuFor(h, pw, 1); return 0;
                }
                if (g_open && !g_closing && PtInRect(&arw, pt)) {
                    g_pressed = -1;
                    if (g_view == VIEW_START) EnterAllApps(); else LeaveSearch();
                    return 0;
                }
                (void)srt;
                if (g_open && !g_closing && g_view == VIEW_SEARCH && g_queryLen == 0 && PtInRect(&srt, pt)) {
                    g_pressed = -1; ShowMenuFor(h, srt, 2); return 0;
                }
            }
            if (g_open && !g_closing && g_view == VIEW_START && !g_dragging && PtInRect(&ar, pt)) {
                g_pressed = -1;
                if (g_swallowAccountUp) { g_swallowAccountUp = false; return 0; }
                ShowUserMenu(h);
                return 0;
            }
            g_swallowAccountUp = false;
        }
        if (g_charmPressed != -1) {
            int pressed = g_charmPressed; g_charmPressed = -1;
            POINT pt2 = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT pr; if (CharmPaneRect(&pr) && PtInRect(&pr, pt2)) {
                int lx = pt2.x - pr.left - g_charmCont, ly = pt2.y - pr.top, row = -1;
                int hit = CharmHit(lx, ly, &row);
                if (hit == pressed || (hit == CH_ROW && row == pressed))
                    CharmClickActions(hit, row, ly);
            }
            return 0;
        }
        if (g_barVisible && (g_barPressedButton >= 0 || g_advPressedRow >= 0 || g_resizePressedRow >= 0)) {
            int pressedButton = g_barPressedButton, pressedRow = g_advPressedRow, pressedSize = g_resizePressedRow;
            int mx = (short)LOWORD(lp), my = (short)HIWORD(lp);
            UpdateBarHover(mx, my);
            g_barPressedButton = g_advPressedRow = g_resizePressedRow = -1;
            if ((pressedButton >= 0 && g_barHoverButton == pressedButton) ||
                (pressedRow >= 0 && g_advHoverRow == pressedRow) ||
                (pressedSize >= 0 && g_resizeHoverRow == pressedSize)) BarClick(mx, my);
            else RepaintNow();
            return 0;
        }
        if (g_view == VIEW_SEARCH) { if (g_open) OnSearchClick((short)LOWORD(lp), (short)HIWORD(lp)); return 0; }
        if (g_dragging) { UpdateDrag((short)LOWORD(lp), (short)HIWORD(lp)); EndDrag(); return 0; }
        if (BarClick((short)LOWORD(lp), (short)HIWORD(lp))) { g_pressed = -1; return 0; }
        int p = g_pressed;
        g_pressed = -1;
        if (p >= 0 && p == HitTile((short)LOWORD(lp), (short)HIWORD(lp))) Launch(p);
        return 0;
    }
    case WM_RBUTTONUP:
        if (g_swallowCharmUp) { g_swallowCharmUp = false; return 0; }
        if (g_open && !g_closing && g_charmOpen) {
            POINT rpu = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT rpr; if (CharmPaneRect(&rpr) && PtInRect(&rpr, rpu)) return 0;
        }
        if (g_open && !g_closing) {
            if (g_view == VIEW_START && !g_charmOpen && ZoomPresence(NowMs()) > 0.5f) {
                float wx, wy; ScreenToWorld((short)LOWORD(lp), (short)HIWORD(lp), &wx, &wy);
                int zg = GroupAtWorld(wx, wy);
                if (zg >= 0) { if (g_barVisible && g_barMode == 5 && g_barIdx == zg) HideBar(); else ShowBar(5, zg); }
                else HideBar();
            } else OnRightClick((short)LOWORD(lp), (short)HIWORD(lp));
        }
        return 0;
    case WM_MOUSEWHEEL: {
        if (g_open && !g_closing && ZoomPresence(NowMs()) > 0.0f) return 0;
        int delta = GET_WHEEL_DELTA_WPARAM(wp);
        if (g_open && !g_closing && !g_charmOpen && g_view == VIEW_SEARCH && g_queryLen == 0 &&
            (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            if (delta < 0 && !g_alphaGrid) { g_alphaGrid = true; g_alphaTick = NowMs(); RepaintNow(); }
            else if (delta > 0 && g_alphaGrid) { g_alphaGrid = false; g_searchTick = NowMs(); RepaintNow(); }
            return 0;
        }
        g_scrollTarget -= (float)delta * 2.5f;
        if (g_charmOpen) {
            RECT pr; POINT pw2 = { (short)LOWORD(lp), (short)HIWORD(lp) };
            if (CharmPaneRect(&pr) && PtInRect(&pr, pw2)) {
                int delta = GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -3 : 3;
                int total = (int)g_results.size(), vis = CharmLayout().vis;
                g_charmScroll = std::min(std::max(0, g_charmScroll + delta), std::max(0, total - vis));
                CharmRender(g_charmCont); RepaintNow();
                return 0;
            }
        }
        float m = (g_view == VIEW_SEARCH) ? MaxScrollSearch() : MaxScroll();
        if (g_scrollTarget < 0.0f) g_scrollTarget = 0.0f;
        if (g_scrollTarget > m) g_scrollTarget = m;
        return 0;
    }
    }
    return DefWindowProcW(h, msg, wp, lp);
    });
}

using XamlLauncher_ShowStartView_t = HRESULT (WINAPI*)(void* pThis, int immersiveLauncherShowMethod,
                                                       int immersiveLauncherShowFlags);
static XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original = NULL;

static HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int method, int flags) {
    Wh_Log(L"ShowStartView requested (method=%d flags=%d)", method, flags);
    if (g_useWinKey && g_wnd) {
        PostMessageW(g_wnd, WM_APP_TOGGLE, 0, 0);
        return S_OK;
    }
    return XamlLauncher_ShowStartView_Original(pThis, method, flags);
}

static bool InstallStartHook() {
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    HMODULE twinui = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!twinui) twinui = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinui) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return false;
    }
    if (!WindhawkUtils::HookSymbols(twinui, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed for ShowStartView; using the keyboard hook only");
        return false;
    }
    Wh_Log(L"ShowStartView hooked: the Windows key and the taskbar Start button open this Start screen");
    return true;
}

// An unassigned key pressed between the Windows key down and up stops Explorer from treating it as a lone press.
static void MaskWinKey() {
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD; in[0].ki.wVk = 0xE8; in[0].ki.dwExtraInfo = kMagic;
    in[1].type = INPUT_KEYBOARD; in[1].ki.wVk = 0xE8; in[1].ki.dwExtraInfo = kMagic; in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static wchar_t TranslateKey(const KBDLLHOOKSTRUCT* k, bool altGr, HKL hkl) {
    BYTE ks[256] = {};
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) ks[VK_SHIFT] = 0x80;
    if (GetKeyState(VK_CAPITAL) & 1) ks[VK_CAPITAL] = 1;
    if (altGr) {
        ks[VK_CONTROL] = ks[VK_LCONTROL] = 0x80;
        ks[VK_MENU] = ks[VK_RMENU] = 0x80;
    }
    WCHAR buf[8] = {};
    int n = ToUnicodeEx(k->vkCode, k->scanCode, ks, buf, 8, 0x4  , hkl);
    return (n == 1 && buf[0] >= 0x20 && buf[0] != 0x7f) ? buf[0] : 0;
}

static wchar_t KeyToChar(const KBDLLHOOKSTRUCT* k, bool altGr) {
    HWND fg = GetForegroundWindow();
    DWORD tid = fg ? GetWindowThreadProcessId(fg, NULL) : 0;
    HKL hkl = GetKeyboardLayout(tid);
    wchar_t c = TranslateKey(k, altGr, hkl);
    if (altGr && c && c == TranslateKey(k, false, hkl)) return 0;
    return c;
}

static bool HandleSearchKey(const KBDLLHOOKSTRUCT* k, bool down, bool up) {
    UINT vk = k->vkCode & 255;
    if (up) {
        if (g_swallowed[vk]) { g_swallowed[vk] = false; return true; }
        return false;
    }
    if (!down) return false;
    if (vk == VK_TAB && (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        !(GetAsyncKeyState(VK_MENU) & 0x8000)) {
        g_swallowed[vk] = true;
        PostMessageW(g_wnd, WM_APP_ALL_APPS, 0, 0);
        return true;
    }
    const bool altGr = (GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_LCONTROL) & 0x8000) &&
                       !(GetAsyncKeyState(VK_RCONTROL) & 0x8000) && !(GetAsyncKeyState(VK_LMENU) & 0x8000);
    if (!altGr && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) || (GetAsyncKeyState(VK_MENU) & 0x8000))) return false;
    bool searching = (g_view == VIEW_SEARCH) || g_charmOpen || g_namingGroup >= 0;
    if (searching && (vk == VK_BACK || vk == VK_RETURN || vk == VK_UP || vk == VK_DOWN ||
                      vk == VK_LEFT || vk == VK_RIGHT)) {
        g_swallowed[vk] = true;
        PostMessageW(g_wnd, WM_APP_KEY, vk, 0);
        return true;
    }
    bool navigating = (g_view == VIEW_START) && !g_charmOpen && g_namingGroup < 0 &&
                      ZoomPresence(NowMs()) <= 0.0f && !g_barVisible;
    if (navigating && (vk == VK_RETURN || vk == VK_UP || vk == VK_DOWN ||
                       vk == VK_LEFT || vk == VK_RIGHT)) {
        g_swallowed[vk] = true;
        PostMessageW(g_wnd, WM_APP_KEY, vk, 0);
        return true;
    }
    wchar_t c = KeyToChar(k, altGr);
    if (!c) return false;
    g_swallowed[vk] = true;
    if (!searching && c == L' ') return true;
    PostMessageW(g_wnd, WM_APP_CHAR, (WPARAM)c, 0);
    return true;
}

static LRESULT CALLBACK KbProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lp;
        if (k->dwExtraInfo != kMagic) {
            bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
            bool up = (wp == WM_KEYUP || wp == WM_SYSKEYUP);
            if (g_inUserMenu || NowMs() - g_menuClosedTick < 400) {
                if (k->vkCode == VK_ESCAPE && down) {
                    if (g_userMenuWnd) PostMessageW(g_userMenuWnd, WM_KEYDOWN, VK_ESCAPE, 0);
                    return 1;
                }
                return CallNextHookEx(g_hook, code, wp, lp);
            }
            if (k->vkCode == VK_ESCAPE && down && g_open && !g_closing) {
                PostMessageW(g_wnd, WM_APP_KEY, VK_ESCAPE, 0);
                return 1;
            }
            if (g_open && !g_closing && g_searchEnabled && !g_winDown) {
                if (HandleSearchKey(k, down, up)) return 1;
            }
            if (g_useWinKey) {
                if (k->vkCode == VK_LWIN || k->vkCode == VK_RWIN) {
                    if (down) {
                        if (!g_winDown) { g_winDown = true; g_winCombo = false; }
                    } else if (up) {
                        bool alone = g_winDown && !g_winCombo;
                        g_winDown = false;
                        if (alone) {
                            Wh_Log(L"Windows key pressed alone (keyboard hook)");
                            MaskWinKey();
                            PostMessageW(g_wnd, WM_APP_TOGGLE, 0, 0);
                        }
                    }
                } else if (down && g_winDown) {
                    g_winCombo = true;
                }
            }
        }
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}

static LRESULT CALLBACK CharmMouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        switch (wp) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN: {
            const MSLLHOOKSTRUCT* m = (const MSLLHOOKSTRUCT*)lp;
            if (m && g_wnd)
                PostMessageW(g_wnd, WM_APP_CHARM_POINTER,
                             (WPARAM)(LONG_PTR)m->pt.x, (LPARAM)(LONG_PTR)m->pt.y);
            break;
        }
        }
    }
    return CallNextHookEx(g_mouseHook, code, wp, lp);
}

static DWORD WINAPI HookThread(LPVOID) {
  RunGuarded(L"HookThread", [] {
    MSG m;
    PeekMessageW(&m, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_hookReady) SetEvent(g_hookReady);
    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc, GetModuleHandleW(NULL), 0);
    if (g_hook) Wh_Log(L"Keyboard hook installed");
    else Wh_Log(L"SetWindowsHookEx failed, error %u", (unsigned)GetLastError());
    while (GetMessageW(&m, NULL, 0, 0) > 0) {
        if (m.message == WM_HOOKCTL_MOUSE_ON) {
            if (!g_mouseHook) g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, CharmMouseProc, GetModuleHandleW(NULL), 0);
            continue;
        }
        if (m.message == WM_HOOKCTL_MOUSE_OFF) {
            if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = NULL; }
            continue;
        }
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }
    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = NULL; }
    if (g_hook) UnhookWindowsHookEx(g_hook);
    g_hook = NULL;
  });
    if (g_hookReady) SetEvent(g_hookReady);
    return 0;
}

static void LoadTileIcons() {
    wchar_t win[MAX_PATH] = L"";
    GetWindowsDirectoryW(win, MAX_PATH);
    for (int i = 0; i < g_tileCount; i++) {
        Tile& t = g_tiles[i];
        if (!t.icon && t.target &&
            (!_wcsicmp(t.target, L"shell:MyComputerFolder") || !_wcsicmp(t.target, L"control.exe"))) {
            const wchar_t* folder = !_wcsicmp(t.target, L"control.exe")
                                    ? L"shell:ControlPanelFolder" : L"shell:MyComputerFolder";
            t.icon = ShellFolderIconHiRes(folder);
            if (t.icon) continue;
        }
        if (!t.iconFile) continue;
        wchar_t path[MAX_PATH];
        if (wcscmp(t.iconFile, L"explorer.exe") == 0) wsprintfW(path, L"%s\\explorer.exe", win);
        else wsprintfW(path, L"%s", t.iconFile);
        HICON ico = NULL; UINT id = 0;
        if (PrivateExtractIconsW(path, t.iconIdx, 256, 256, &ico, &id, 1, 0) == 1 && ico) t.icon = ico;
    }
}

static void UpdateTileAppColors() {
    for (int i = 0; i < g_tileCount; ++i) {
        Tile& t = g_tiles[i];
        t.hasAppC = false;
        t.appC = t.c1;
        if (!t.alive) continue;
        bool found = false;
        COLORREF c = t.c1;
        if (t.icon) {
            c = GetIconAccentColor(t.icon, t.c1, &found);
        } else if (t.pinBmp) {
            BYTE* px = NULL; int w = 0, h = 0;
            if (BitmapToPixels(t.pinBmp, &px, &w, &h) && px) {
                c = AccentFromPremulBgra(px, w, h, t.c1, &found);
                delete[] px;
            }
        }
        if (found) { t.appC = c; t.hasAppC = true; }
    }
}

// True only if the Microsoft Store package is actually registered for this user.
static bool StoreAppInstalled() {
    PIDLIST_ABSOLUTE pidl = NULL;
    const bool ok = SUCCEEDED(SHParseDisplayName(L"shell:AppsFolder\\Microsoft.WindowsStore_8wekyb3d8bbwe!App",
                                                 NULL, &pidl, 0, NULL)) && pidl;
    if (pidl) CoTaskMemFree(pidl);
    return ok;
}

static void InitTiles() {
    g_tileCount = g_orderCount = 0;
    if (!LoadLayout()) {
        // The Store tile is the last default (index kDefaultTileCount - 1); skip it when absent.
        const int defaultCount = StoreAppInstalled() ? kDefaultTileCount : kDefaultTileCount - 1;
        for (int i = 0; i < defaultCount; ++i) {
            g_tiles[i] = kDefaultTiles[i];
            g_tiles[i].alive = true;
            g_tiles[i].owned = false;
            g_tiles[i].kind = 0;
            g_order[i] = i;
        }
        g_tileCount = g_orderCount = defaultCount;
        {
            const wchar_t* localized[kDefaultTileCount];
            localized[0] = T(STR_TILE_EXPLORER);
            localized[1] = T(STR_TILE_COMPUTER);
            localized[2] = T(STR_TILE_CONTROLPANEL);
            localized[3] = T(STR_DESKTOP);
            localized[4] = T(STR_TILE_STORE);
            for (int i = 0; i < defaultCount; ++i) g_tiles[i].label = localized[i];
        }
        for (int i = 0; i < defaultCount; ++i) g_tileGroup[i] = 0;
    }
    for (int i = 0; i < MAX_TILES; ++i) g_tileScale[i] = 1.0f;
    LoadTileIcons();
        UpdateTileAppColors();
    for (int i = 0; i < g_tileCount; ++i)
        if (g_tiles[i].alive && g_tiles[i].kind == 1 &&
            g_tiles[i].style != DESKTOP_REFERENCE) LoadPinIcon(g_tiles[i]);
    UpdateTileAppColors();
    Relayout(false);
    for (int i = 0; i < g_tileCount; ++i) g_committedGroup[i] = g_tileGroup[i];
}

static void UiRun() {
    g_uiCoInit = SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED));
    DetectUiLanguage();
    Gdiplus::GdiplusStartupInput gdipInput;
    if (Gdiplus::GdiplusStartup(&g_gdipToken, &gdipInput, NULL) != Gdiplus::Ok) g_gdipToken = 0;
    g_inst = (HINSTANCE)&__ImageBase;
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_inst;
    wc.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    wc.lpszClassName = kClassName;
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed for the Start window, err=%u", (unsigned)GetLastError());
        return;
    }
    g_classRegistered = true;

    CreateUiFonts();
    DWORD n = 127;
    GetUserNameW(g_userName, &n);
    g_userTextW = -1;
    UpdateDpi();
    InitTiles();
    LoadGroupNames();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    g_createInBand = (CreateWindowInBand_t)(void*)GetProcAddress(user32, "CreateWindowInBand");
    g_sw = GetSystemMetrics(SM_CXSCREEN);
    g_sh = GetSystemMetrics(SM_CYSCREEN);
    Relayout(false);
    g_wnd = CreateTopWindow(g_sw, g_sh);
    if (g_wnd) BuildBuffers();
    if (g_wnd && InterlockedCompareExchange(&g_openAtLogon, 0, 0) != 0) {
        g_logonStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (g_logonStop) g_logonThread = CreateThread(NULL, 0, LogonOpenThread, NULL, 0, NULL);
    }
    if (g_wnd && InterlockedCompareExchange(&g_openAtLogon, 0, 0) != 0 && InitGpu())
        WarmLogonTileTextures();
    g_artStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_artWake = CreateEventW(NULL, FALSE, FALSE, NULL);
    for (int i = 0; i < g_tileCount; ++i)
        if (g_tiles[i].alive && g_tiles[i].style == PINNED_APP) QueuePackageArt(g_tiles[i].target);
    if (g_wnd && g_artStop && g_artWake)
        g_artThread = CreateThread(NULL, 0, PackageArtThread, NULL, 0, NULL);
    g_appsStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_filesNeed = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_appsRefresh = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_iconsGo = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (g_wnd && g_appsStop) g_appsThread = CreateThread(NULL, 0, AppsThread, NULL, 0, NULL);
    g_wallStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_wallWake = CreateEventW(NULL, TRUE, TRUE, NULL);
    if (g_wnd && g_wallStop && g_wallWake)
        g_wallThread = CreateThread(NULL, 0, WallpaperThread, NULL, 0, NULL);
    g_frameStop = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_frameWake = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (g_wnd && g_frameStop && g_frameWake)
        g_frameThread = CreateThread(NULL, 0, FrameThread, NULL, 0, NULL);
    g_hookReady = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_hookThread = CreateThread(NULL, 0, HookThread, NULL, 0, &g_hookTid);

    MSG m;
    bool quit = false;
    while (!quit) {
        if (MsgWaitForMultipleObjectsEx(1, &g_uiStop, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0) break;
        while (PeekMessageW(&m, NULL, 0, 0, PM_REMOVE)) {
            if (m.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }
}

static void UiTeardown() {

    if (g_logonStop) SetEvent(g_logonStop);
    if (g_logonThread) { WaitForSingleObject(g_logonThread, INFINITE); CloseHandle(g_logonThread); g_logonThread = NULL; }
    if (g_logonStop) { CloseHandle(g_logonStop); g_logonStop = NULL; }
    if (g_artStop) SetEvent(g_artStop);
    if (g_artWake) SetEvent(g_artWake);
    if (g_artThread) { WaitForSingleObject(g_artThread, INFINITE); CloseHandle(g_artThread); g_artThread = NULL; }
    if (g_artWake) { CloseHandle(g_artWake); g_artWake = NULL; }
    if (g_artStop) { CloseHandle(g_artStop); g_artStop = NULL; }
    if (g_appsStop) SetEvent(g_appsStop);
    if (g_appsThread) { WaitForSingleObject(g_appsThread, INFINITE); CloseHandle(g_appsThread); g_appsThread = NULL; }
    if (g_appsStop) { CloseHandle(g_appsStop); g_appsStop = NULL; }
    if (g_filesNeed) { CloseHandle(g_filesNeed); g_filesNeed = NULL; }
    if (g_appsRefresh) { CloseHandle(g_appsRefresh); g_appsRefresh = NULL; }
    if (g_iconsGo) { CloseHandle(g_iconsGo); g_iconsGo = NULL; }
    delete g_pendingApps;
    g_pendingApps = NULL;
    delete g_pendingFiles;
    g_pendingFiles = NULL;
    if (g_wallStop) SetEvent(g_wallStop);
    if (g_wallWake) SetEvent(g_wallWake);
    if (g_wallThread) {
        WaitForSingleObject(g_wallThread, INFINITE);
        CloseHandle(g_wallThread); g_wallThread = NULL;
    }
    if (g_wallWake) { CloseHandle(g_wallWake); g_wallWake = NULL; }
    if (g_wallStop) { CloseHandle(g_wallStop); g_wallStop = NULL; }
    if (g_frameStop) SetEvent(g_frameStop);
    if (g_frameWake) SetEvent(g_frameWake);
    if (g_frameThread) {
        WaitForSingleObject(g_frameThread, INFINITE);
        CloseHandle(g_frameThread); g_frameThread = NULL;
    }
    if (g_frameWake) { CloseHandle(g_frameWake); g_frameWake = NULL; }
    if (g_frameStop) { CloseHandle(g_frameStop); g_frameStop = NULL; }
    if (g_hookThread) {
        HANDLE hookWait[2] = { g_hookReady, g_hookThread };
        if (g_hookReady) WaitForMultipleObjects(2, hookWait, FALSE, INFINITE);
        PostThreadMessageW(g_hookTid, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread); g_hookThread = NULL;
    }
    if (g_hookReady) { CloseHandle(g_hookReady); g_hookReady = NULL; }
    if (g_wnd) DestroyWindow(g_wnd);
    g_wnd = NULL;
    ReleaseUiIcons();
    ReleaseAvatarPicture();
    CharmRelease();
    FreeBuffers();
    for (size_t i = 0; i < g_icons.size(); ++i) { delete[] g_icons[i].px; g_icons[i].px = NULL; }
    g_packageArts.clear(); g_artQueue.clear();
    if (g_titleFormat) { g_titleFormat->Release(); g_titleFormat = NULL; }
    if (g_userFormat) { g_userFormat->Release(); g_userFormat = NULL; }
    IDWriteTextFormat** searchFormats[] = { &g_subFormat, &g_itemFormat, &g_sbTitleFormat, &g_boldFormat, &g_rowFormat,
                                            &g_countFormat, &g_queryFormat, &g_glyphFormat, &g_glyphSmall, &g_barLabel, &g_flyFormat, &g_zoomFormat, &g_hdrGlyphFmt };
    for (IDWriteTextFormat** f : searchFormats) if (*f) { (*f)->Release(); *f = NULL; }
    if (g_writeFactory) { g_writeFactory->Release(); g_writeFactory = NULL; }
    if (g_d2dFactory) { g_d2dFactory->Release(); g_d2dFactory = NULL; }
    for (int i = 0; i < g_tileCount; i++) FreeTile(g_tiles[i]);
    HFONT* fonts[] = { &g_fTitle, &g_fUser, &g_fLabel, &g_fTopLabel, &g_fHdrGlyph, &g_fBarMeas };
    for (HFONT* f : fonts) if (*f) { DeleteObject(*f); *f = NULL; }
    if (g_stockAppIcon) { DestroyIcon(g_stockAppIcon); g_stockAppIcon = NULL; }
    UnregisterUserMenuClass();
    if (g_classRegistered) {
        UnregisterClassW(kClassName, g_inst);
        g_classRegistered = false;
    }
    g_iconBitmapCache.reset();
    if (g_gdipToken) { Gdiplus::GdiplusShutdown(g_gdipToken); g_gdipToken = 0; }
    if (g_uiCoInit) {
        CoUninitialize();
        g_uiCoInit = false;
    }
}

static DWORD WINAPI UiThread(LPVOID) {
    MSG queueMsg;
    PeekMessageW(&queueMsg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_uiReady) SetEvent(g_uiReady);
    HANDLE mtx = CreateMutexW(NULL, TRUE, L"Local\\Win8StartScreenMod8045");
    if (mtx && GetLastError() == ERROR_ALREADY_EXISTS) {
        HANDLE waits[2] = { mtx, g_uiStop };
        DWORD w = WaitForMultipleObjects(g_uiStop ? 2 : 1, waits, FALSE, 8000);
        if (w != WAIT_OBJECT_0 && w != WAIT_ABANDONED_0) {
            Wh_Log(L"Another live Start screen instance holds the mutex, or the mod is unloading; standing down");
            CloseHandle(mtx);
            return 0;
        }
        Wh_Log(L"Took over the Start screen instance from a previous shell");
    }
    if (g_uiStop && WaitForSingleObject(g_uiStop, 0) == WAIT_OBJECT_0) {
        if (mtx) CloseHandle(mtx);
        return 0;
    }
    RunGuarded(L"UiThread", [] { UiRun(); });
    RunGuarded(L"UiTeardown", [] { UiTeardown(); });
    if (mtx) CloseHandle(mtx);
    return 0;
}

static void LoadSettings() {
    g_useWinKey = Wh_GetIntSetting(L"winKey") != 0;
    g_searchEnabled = Wh_GetIntSetting(L"searchOnType") != 0;
    InterlockedExchange(&g_openAtLogon, Wh_GetIntSetting(L"openAtLogon") != 0);
    {
        WindhawkUtils::StringSetting mode(WindhawkUtils::StringSetting::make(L"interceptMode"));
        g_keyboardOnly = wcscmp(mode.get(), L"keyboard") == 0;
    }
    LONG openMs = Wh_GetIntSetting(L"openSlideMs");
    LONG closeMs = Wh_GetIntSetting(L"closeSlideMs");
    if (openMs < 80) openMs = 80;
    if (openMs > 3000) openMs = 3000;
    if (closeMs < 80) closeMs = 80;
    if (closeMs > 3000) closeMs = 3000;
    InterlockedExchange(&g_openSlideMs, openMs);
    InterlockedExchange(&g_closeSlideMs, closeMs);
    WindhawkUtils::StringSetting bgModeStr(WindhawkUtils::StringSetting::make(L"backgroundMode"));
    LONG bgMode = 0;
    if (wcscmp(bgModeStr.get(), L"solid") == 0) bgMode = 1;
    else if (wcscmp(bgModeStr.get(), L"accent") == 0) bgMode = 2;
    else if (wcscmp(bgModeStr.get(), L"wallpaper") == 0) bgMode = 3;
    else if (wcscmp(bgModeStr.get(), L"image") == 0) bgMode = 4;
    bool bgPathChanged = false;
    {
        WindhawkUtils::StringSetting pathStr(WindhawkUtils::StringSetting::make(L"backgroundImagePath"));
        const std::wstring want = pathStr.get();
        AcquireSRWLockExclusive(&g_bgPathLock);
        bgPathChanged = (g_bgImagePath != want);
        if (bgPathChanged) g_bgImagePath = want;
        ReleaseSRWLockExclusive(&g_bgPathLock);
    }
    COLORREF bgColor = RGB(0x46, 0x17, 0xB4);
    {
        WindhawkUtils::StringSetting colorStr(WindhawkUtils::StringSetting::make(L"backgroundColor"));
        const wchar_t* v = colorStr.get();
        if (v[0] == L'#') ++v;
        wchar_t* end = NULL;
        unsigned long parsed = wcstoul(v, &end, 16);
        if (end != v && end == v + 6 && *end == L'\0')
            bgColor = RGB((parsed >> 16) & 255, (parsed >> 8) & 255, parsed & 255);
    }
    LONG oldBgMode = InterlockedExchange(&g_bgMode, bgMode);
    bool bgChanged = (oldBgMode != bgMode) || (bgColor != g_bgColor) || bgPathChanged;
    g_bgColor = bgColor;
    LONG cols = Wh_GetIntSetting(L"groupColumns");
    LONG rows = Wh_GetIntSetting(L"groupRows");
    if (cols < 2) cols = 2;
    if (cols > 8) cols = 8;
    if (rows < 1) rows = 1;
    if (rows > 8) rows = 8;
    LONG oldCols = InterlockedExchange(&g_requestedGroupCols, cols);
    LONG oldRows = InterlockedExchange(&g_requestedGroupRows, rows);
    if ((oldCols != cols || oldRows != rows) && g_wnd)
        PostMessageW(g_wnd, WM_APP_GROUP_SETTINGS, 0, 0);
    {
        const LONG seed = Wh_GetIntSetting(L"seedStartLayout") != 0 ? 1 : 0;
        // Seed at most once per user: re-seeding would re-pin apps the user removed.
        // Turning the option off clears the flag, so off -> on seeds again on request.
        if (!seed) Wh_SetIntValue(L"layoutSeeded", 0);
        const bool alreadySeeded = Wh_GetIntValue(L"layoutSeeded", 0) != 0;
        if (!seed) InterlockedExchange(&g_seedLayout, 0);
        if (seed && !alreadySeeded && InterlockedExchange(&g_seedLayout, seed) == 0) {
            InterlockedExchange(&g_seedPending, 1);
            if (g_wnd) PostMessageW(g_wnd, WM_APP_SEED_LAYOUT, 0, 0);
        }
    }
    LONG priv = Wh_GetIntSetting(L"privacyMode") != 0 ? 1 : 0;
    LONG appc = Wh_GetIntSetting(L"appTileColors") != 0 ? 1 : 0;
    bool chromeChanged = false;
    if (InterlockedExchange(&g_privacyMode, priv) != priv) { g_userTextW = -1; chromeChanged = true; }
    if (InterlockedExchange(&g_appTileColors, appc) != appc) { InterlockedExchange(&g_tileColorsDirty, 1); chromeChanged = true; }
    if ((bgChanged || chromeChanged) && g_wnd)
        PostMessageW(g_wnd, WM_APP_REBUILD_BG, 0, 0);
}

// Mod entry points
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    const bool oldKeyboardOnly = g_keyboardOnly;
    RunGuarded(L"ModSettingsChanged", [] { LoadSettings(); });
    // The hook set depends on interceptMode, so a change needs a reload.
    if (bReload && g_keyboardOnly != oldKeyboardOnly) *bReload = TRUE;
    return TRUE;
}

// explorer.exe also runs as COM/file-window helper processes (/factory, -Embedding); only the
// shell instance should host the Start screen, otherwise it could claim the mutex first.
static bool IsShellExplorerProcess() {
    const wchar_t* cmd = GetCommandLineW();
    if (!cmd) return true;
    std::wstring lower = cmd;
    for (wchar_t& c : lower) c = (wchar_t)towlower(c);
    if (lower.find(L"/factory") != std::wstring::npos || lower.find(L"-embedding") != std::wstring::npos ||
        lower.find(L"/embedding") != std::wstring::npos || lower.find(L"/select") != std::wstring::npos)
        return false;
    // If a shell window already exists, it must belong to this process.
    HWND shell = GetShellWindow();
    if (shell) {
        DWORD pid = 0;
        GetWindowThreadProcessId(shell, &pid);
        if (pid && pid != GetCurrentProcessId()) return false;
    }
    return true;
}

BOOL Wh_ModInit() {
    return RunGuardedValue<BOOL>(L"ModInit", FALSE, []() -> BOOL {
        if (!IsShellExplorerProcess()) {
            Wh_Log(L"Not the shell explorer.exe; not hosting the Start screen here");
            return FALSE;
        }
        LoadSettings();
        g_uiReady = CreateEventW(NULL, TRUE, FALSE, NULL);
        g_uiStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (g_uiReady && g_uiStop) g_thread = CreateThread(NULL, 0, UiThread, NULL, 0, &g_tid);
        if (!g_thread) {
            if (g_uiReady) { CloseHandle(g_uiReady); g_uiReady = NULL; }
            if (g_uiStop) { CloseHandle(g_uiStop); g_uiStop = NULL; }
            return FALSE;
        }
        if (!g_keyboardOnly) g_startHooked = InstallStartHook();
        return TRUE;
    });
}

void Wh_ModUninit() {
    RunGuarded(L"ModUninit", [] {
        if (g_thread) {
            if (g_uiStop) SetEvent(g_uiStop);
            HANDLE readyWait[2] = { g_uiReady, g_thread };
            if (g_uiReady) WaitForMultipleObjects(2, readyWait, FALSE, INFINITE);
            PostThreadMessageW(g_tid, WM_QUIT, 0, 0);
            WaitForSingleObject(g_thread, INFINITE);
            CloseHandle(g_thread);
            g_thread = NULL;
        }
        if (g_uiReady) { CloseHandle(g_uiReady); g_uiReady = NULL; }
        if (g_uiStop) { CloseHandle(g_uiStop); g_uiStop = NULL; }
    });
}
