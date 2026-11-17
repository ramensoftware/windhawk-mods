// ==WindhawkMod==
// @id              win81-rt-update3-start-menu
// @name            Windows 8.1 Update 3 Start Menu Recreation
// @name:it         Ricreazione del menu Start di Windows 8.1 Update 3
// @name:es         Recreación del menú Inicio de Windows 8.1 Update 3
// @name:fr         Recréation du menu Démarrer de Windows 8.1 Update 3
// @name:pt         Recriação do menu Iniciar do Windows 8.1 Atualização 3
// @description     This mod recreates the scrapped Windows 8.1 Update 3 Start menu (the one that shipped with Windows RT 8.1 Update 3: Win7 layout, flat Metro styling, tiles) on Windows 10 and Windows 11
// @description:it  Questa mod ricrea su Windows 10 e Windows 11 il menu Start di Windows 8.1 Update 3 (quello spedito con Windows RT 8.1 Update 3: layout di Windows 7, stile Metro piatto, con tile)
// @description:es  Este mod recrea en Windows 10 y Windows 11 el menú Inicio de Windows 8.1 Actualización 3 (el que llegó con Windows RT 8.1 Update 3: diseño de Windows 7, estilo Metro plano, con mosaicos)
// @description:fr  Ce mod recrée sous Windows 10 et Windows 11 le menu Démarrer de Windows 8.1 Update 3 (celui livré avec Windows RT 8.1 Update 3 : disposition Windows 7, style Metro plat, avec vignettes)
// @description:pt  Este mod recria no Windows 10 e no Windows 11 o menu Iniciar do Windows 8.1 Atualização 3 (aquele que veio com o Windows RT 8.1 Update 3: layout do Windows 7, estilo Metro plano, com blocos)
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         GPL-3.0-or-later
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luser32 -lgdi32 -ldwmapi -lshell32 -loleaut32 -luuid -lshcore -lcomctl32 -lshlwapi -ladvapi32 -lgdiplus -lpowrprof -lsecur32 -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8.1 Update 3 Start Menu Recreation

This mod recreates the Start menu introduced with Windows RT 8.1 Update 3 on
Windows 10 and Windows 11. It combines an application list with Windows 8-style
tiles. The menu is a separate window; the mod does not replace system files.

## Dedicated process

This mod has its own settings and tile storage.
The menu runs in a dedicated explorer.exe tool process; a small bridge remains
in the shell Explorer for Start keyboard messages. Disable the regular Start
menu mod and other Start replacements before enabling this mod.
The hybrid startup, elevation, Explorer restart and shutdown require testing
on real Windows; the bridge is not crash-isolated from Explorer.

## Compatibility

Do not enable this mod at the same time as another Start menu or Start screen
replacement (including Open-Shell or the Windows 8.1 Start Screen Recreation).
They compete for the Windows key and can open multiple Start surfaces.

## Features

- **Start menu**: open or close it with the Windows key. The Start button can
  also open it when the mod can reliably identify the button on the taskbar.
- **Apps**: most-used and recently added applications, plus an alphabetical
  All apps list with expandable folders. Tiles remain visible in All apps.
- **Tiles**: pin, unpin, drag and resize tiles; choose the colors and shading.
  Keyboard focus and mouse hover have distinct tile highlights.
- **Search**: typing searches apps, Control Panel and Settings entries, and
  files in the user's Desktop, Documents, Downloads, Pictures, Music and Videos.
  The file scan is limited to 80,000 entries and eight folder levels, excludes
  hidden/system entries, reparse points and node_modules, and is not a web or
  full-disk search. Results appear as the background scan completes, up to 300
  matches. Settings and file results do not offer app pinning/context commands.
  Modern settings use Windows' system search metadata and a best-effort activation
  route; availability varies by Windows version.
- **Menus**: right-click apps and tiles for their available commands; account
  and power menus use the same Windows 8-style popup presentation.
- **Appearance**: choose the background and accent, use a profile picture, and
  resize the menu. Privacy mode substitutes a generic account name.
- **Layout**: tile positions and sizes are kept in Windhawk's mod storage; a
  serialized layout can also be imported through the settings.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| Windows key | Opens or closes the menu |
| Typing | Searches apps, settings and files |
| Arrow keys and Enter | Navigates and opens items |
| Tab / Shift+Tab | Moves between menu areas |
| Menu key / Shift+F10 | Opens a context menu |
| Esc | Closes a popup or the menu, or clears the active search |

## Additional information

- The menu is available in English, Italian, Spanish, French, Persian, Arabic,
  Portuguese, Chinese, Japanese, Polish and Russian; it follows Windows language.
- The settings page includes controls for the menu and tile appearance.
- Only the Calendar tile can show locally generated live content; the mod cannot
  read the live-tile content of other applications.
- On taskbars whose Start button cannot be identified reliably, the Windows key
  still opens the menu, but clicking the button may not be intercepted.
- The Windows 8-style popup and RT-inspired tile highlights are recreations,
  not original Microsoft DirectUI components.

## Credits

- **cips** - concept, source material and feedback.
- **AdmnistratoX** - the original Windows 8 (8045) Start Screen mod.
- **m417z** - Windhawk and technical references.
- **win81-taskbar-recreation** and **taskbar-auto-hide-keyboard-only** - the
  optional Explorer keyboard Start hook and compatibility approach.
- All testers who reported problems and provided feedback.

## Notes

This modification is not affiliated with, endorsed by, or sponsored by Microsoft.
Windows is a trademark of Microsoft Corporation. Report problems on GitHub.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- accentMode: auto
  $name: Accent color
  $description: "This setting controls which accent color is used for Start menu highlights, the search selection and the accent-colored tiles. \"System accent\" follows the color configured in Windows personalisation settings, while \"Windows 8.1 Update 3 default\" restores the fixed blue of the Windows 8 tile palette (#0A5BC4, from the reference palette) for a more authentic retro appearance."
  $options:
  - auto: System accent
  - classic: Windows 8.1 Update 3 default
  $name:it-IT: Colore di accento
  $description:it-IT: "Questa impostazione determina il colore di accento usato per evidenziare il menu Start, la selezione della ricerca e i riquadri colorati. «Accento di sistema» segue il colore impostato nelle personalizzazioni di Windows, mentre «Predefinito di Windows 8.1 Update 3» ripristina il blu fisso della tavolozza dei riquadri di Windows 8 (#0A5BC4, della tavolozza di riferimento) per un aspetto d'epoca più autentico."
  $options:it-IT:
  - auto: Accento di sistema
  - classic: Predefinito di Windows 8.1 Update 3
  $name:es-ES: Color de acento
  $description:es-ES: "Esta configuración controla el color de acento usado para resaltar el menú Inicio, la selección de la búsqueda y los mosaicos coloreados. «Acento del sistema» sigue el color configurado en la personalización de Windows, mientras que «Predeterminado de Windows 8.1 Update 3» restaura el azul fijo de la paleta de mosaicos de Windows 8 (#0A5BC4, de la paleta de referencia) para un aspecto retro más auténtico."
  $options:es-ES:
  - auto: Acento del sistema
  - classic: Predeterminado de Windows 8.1 Update 3
  $name:fr-FR: Couleur d'accentuation
  $description:fr-FR: "Ce paramètre contrôle la couleur d'accentuation utilisée pour la mise en surbrillance du menu Démarrer, la sélection de la recherche et les vignettes colorées. « Accentuation du système » suit la couleur configurée dans les paramètres de personnalisation de Windows, tandis que « Par défaut de Windows 8.1 Update 3 » restaure le bleu fixe de la palette de vignettes de Windows 8 (#0A5BC4, de la palette de référence) pour un aspect rétro plus authentique."
  $options:fr-FR:
  - auto: Accentuation du système
  - classic: Par défaut de Windows 8.1 Update 3
  $name:pt-PT: Cor de destaque
  $description:pt-PT: "Esta definição controla a cor de destaque usada para realçar o menu Iniciar, a seleção da pesquisa e os blocos coloridos. « Destaque do sistema » segue a cor configurada nas personalizações do Windows, enquanto « Predefinição do Windows 8.1 Update 3 » restaura o azul fixo da paleta de blocos do Windows 8 (#0A5BC4, da paleta de referência) para um aspeto retró mais autêntico."
  $options:pt-PT:
  - auto: Destaque do sistema
  - classic: Predefinição do Windows 8.1 Update 3
- backgroundColorMode: classic
  $name: Start menu background
  $description: "This setting chooses the Start menu background: the default purple, the Windows accent, or a custom color. Very light accents are darkened for readability."
  $options:
  - classic: Windows 8.1 default (purple)
  - accent: Accent color
  - custom: Custom color
  $name:it-IT: Sfondo del menu Start
  $description:it-IT: "Questa impostazione sceglie lo sfondo del menu Start: il viola predefinito, il colore di accento di Windows oppure un colore personalizzato. Gli accenti molto chiari vengono scuriti per mantenere il testo leggibile."
  $options:it-IT:
  - classic: Predefinito di Windows 8.1 (viola)
  - accent: Colore di accento
  - custom: Colore personalizzato
  $name:es-ES: Fondo del menú Inicio
  $description:es-ES: "Esta configuración elige el fondo del menú Inicio: el púrpura predeterminado, el color de acento de Windows o un color personalizado. Los acentos muy claros se oscurecen para mantener la legibilidad."
  $options:es-ES:
  - classic: Predeterminado de Windows 8.1 (púrpura)
  - accent: Color de acento
  - custom: Color personalizado
  $name:fr-FR: Arrière-plan du menu Démarrer
  $description:fr-FR: "Ce paramètre choisit le fond du menu Démarrer : le violet par défaut, la couleur d’accentuation Windows ou une couleur personnalisée. Les accents très clairs sont assombris pour préserver la lisibilité."
  $options:fr-FR:
  - classic: Par défaut de Windows 8.1 (violet)
  - accent: Couleur d'accentuation
  - custom: Couleur personnalisée
  $name:pt-PT: Fundo do menu Iniciar
  $description:pt-PT: "Esta definição escolhe o fundo do menu Iniciar: o roxo predefinido, a cor de destaque do Windows ou uma cor personalizada. Os destaques muito claros são escurecidos para manter a legibilidade."
  $options:pt-PT:
  - classic: Predefinição do Windows 8.1 (roxo)
  - accent: Cor de destaque
  - custom: Cor personalizada
- backgroundColorCustom: "4617B4"
  $name: Custom background color (RRGGBB)
  $description: "This setting specifies the custom background color as hexadecimal RRGGBB, for example 4617B4. It is used when the background mode is Custom; invalid values use the default purple."
  $name:it-IT: Colore di sfondo personalizzato (RRGGBB)
  $description:it-IT: "Questa impostazione specifica il colore di sfondo personalizzato in formato esadecimale RRGGBB, ad esempio 4617B4. Viene usato con la modalità Personalizzato; i valori non validi usano il viola predefinito."
  $name:es-ES: Color de fondo personalizado (RRGGBB)
  $description:es-ES: "Esta configuración especifica el color de fondo personalizado en formato hexadecimal RRGGBB, por ejemplo 4617B4. Se usa en el modo Personalizado; los valores no válidos usan el púrpura predeterminado."
  $name:fr-FR: Couleur d'arrière-plan personnalisée (RRVB)
  $description:fr-FR: "Ce paramètre définit la couleur de fond personnalisée au format hexadécimal RRGGBB, par exemple 4617B4. Elle est utilisée en mode personnalisé ; une valeur incorrecte utilise le violet par défaut."
  $name:pt-PT: Cor de fundo personalizada (RRGGBB)
  $description:pt-PT: "Esta definição especifica a cor de fundo personalizada em hexadecimal RRGGBB, por exemplo 4617B4. É usada no modo personalizado; valores inválidos usam o roxo predefinido."
- tileColorMode: metro
  $name: Tile colors
  $description: "This setting selects tile colors from the Windows 8-inspired palette, the Windows accent, or colors sampled from each application icon."
  $options:
  - metro: Windows 8 default palette (per app)
  - accent: Accent color
  - icon: Sampled from the app icon
  $name:it-IT: Colori dei riquadri
  $description:it-IT: "Questa impostazione sceglie i colori dei riquadri dalla tavolozza ispirata a Windows 8, dal colore di accento di Windows oppure dalle icone delle singole app."
  $options:it-IT:
  - metro: Tavolozza predefinita di Windows 8 (per app)
  - accent: Colore di accento
  - icon: Campionato dall'icona dell'app
  $name:es-ES: Colores de los mosaicos
  $description:es-ES: "Esta configuración selecciona los colores de los mosaicos de la paleta inspirada en Windows 8, del acento de Windows o de los iconos de cada aplicación."
  $options:es-ES:
  - metro: Paleta predeterminada de Windows 8 (por app)
  - accent: Color de acento
  - icon: Muestreado del icono de la app
  $name:fr-FR: Couleurs des vignettes
  $description:fr-FR: "Ce paramètre choisit les couleurs des vignettes dans la palette inspirée de Windows 8, la couleur d’accentuation Windows ou les icônes de chaque application."
  $options:fr-FR:
  - metro: Palette par défaut de Windows 8 (par app)
  - accent: Couleur d'accentuation
  - icon: Échantillonné depuis l'icône de l'app
  $name:pt-PT: Cores dos blocos
  $description:pt-PT: "Esta definição escolhe as cores dos blocos da paleta inspirada no Windows 8, da cor de destaque do Windows ou dos ícones de cada aplicação."
  $options:pt-PT:
  - metro: Paleta predefinida do Windows 8 (por app)
  - accent: Cor de destaque
  - icon: Amostrada do ícone da app
- tileGradients: true
  $name: Windows 8.1 tile shading
  $description: "This setting enables shading across tile backgrounds; turn it off to use flat colors."
  $name:it-IT: Sfumatura dei riquadri di Windows 8.1
  $description:it-IT: "Questa impostazione attiva la sfumatura sugli sfondi dei riquadri; disattivala per usare colori uniformi."
  $name:es-ES: Sombreado de mosaicos de Windows 8.1
  $description:es-ES: "Esta configuración activa el sombreado de los fondos de mosaico; desactívala para usar colores lisos."
  $name:fr-FR: Dégradé des vignettes de Windows 8.1
  $description:fr-FR: "Ce paramètre active le dégradé des fonds de vignettes ; désactivez-le pour utiliser des couleurs unies."
  $name:pt-PT: Sombreamento dos blocos do Windows 8.1
  $description:pt-PT: "Esta definição ativa o sombreado nos fundos dos blocos; desative-a para usar cores uniformes."
- liveTileAnimation: false
  $name: Calendar tile flip animation
  $description: "This setting enables the periodic flip animation on the Calendar tile only, which then alternates between its icon and today's weekday and date, computed locally. It is disabled by default. Microsoft documents tile notifications as a write-only API for an app's own tile (TileUpdateManager.CreateTileUpdaterForApplication), so there is no supported way for a third-party Start menu to read the live content of Mail, Photos, Weather or other apps; those tiles therefore stay static instead of showing made-up content."
  $name:it-IT: Animazione del riquadro Calendario
  $description:it-IT: "Questa impostazione attiva l'animazione periodica solo sul riquadro Calendario, che alterna la propria icona al giorno della settimana e alla data di oggi, calcolati in locale. È disattivato per impostazione predefinita. Microsoft documenta le notifiche dei riquadri come un'API scrivibile solo per il riquadro della propria app (TileUpdateManager.CreateTileUpdaterForApplication), quindi non esiste un modo supportato per un menu Start di terze parti di leggere il contenuto live di Posta, Foto, Meteo o altre app; quei riquadri restano quindi statici invece di mostrare contenuti inventati."
  $name:es-ES: Animación del mosaico Calendario
  $description:es-ES: "Esta configuración activa la animación periódica solo en el mosaico Calendario, que alterna entre su icono y el día de la semana y la fecha de hoy, calculados localmente. Está desactivado por defecto. Microsoft documenta las notificaciones de mosaicos como una API de solo escritura para el mosaico de la propia aplicación (TileUpdateManager.CreateTileUpdaterForApplication), por lo que no hay una forma admitida de que un menú Inicio de terceros lea el contenido en vivo de Correo, Fotos, El tiempo u otras apps; esos mosaicos permanecen estáticos en lugar de mostrar contenido inventado."
  $name:fr-FR: Animation de la vignette Calendrier
  $description:fr-FR: "Ce paramètre active l'animation périodique uniquement sur la vignette Calendrier, qui alterne alors entre son icône et le jour de la semaine et la date du jour, calculés localement. Il est désactivé par défaut. Microsoft documente les notifications de vignettes comme une API en écriture seule pour la vignette de sa propre application (TileUpdateManager.CreateTileUpdaterForApplication), il n'existe donc pas de moyen pris en charge pour qu'un menu Démarrer tiers lise le contenu en direct de Courrier, Photos, Météo ou d'autres apps ; ces vignettes restent donc statiques au lieu d'afficher du contenu inventé."
  $name:pt-PT: Animação do bloco Calendário
  $description:pt-PT: "Esta definição ativa a animação periódica apenas no bloco Calendário, que então alterna entre o seu ícone e o dia da semana e a data de hoje, calculados localmente. Está desativada por predefinição. A Microsoft documenta as notificações de blocos como uma API só de escrita para o bloco da própria aplicação (TileUpdateManager.CreateTileUpdaterForApplication), por isso não existe uma forma suportada de um menu Iniciar de terceiros ler o conteúdo em tempo real do Correio, Fotos, Meteorologia ou outras apps; esses blocos ficam portanto estáticos em vez de mostrarem conteúdo inventado."
- liveTileIntervalMs: 4000
  $name: Live tile flip interval (ms)
  $description: "This setting defines how many milliseconds elapse between each Calendar tile flip when the flip animation is enabled. Lower values make the tiles cycle more rapidly and feel busier, while higher values produce a calmer, more subdued effect. The default of 4000 ms matches the pacing used by the original live tiles."
  $name:it-IT: Intervallo dell'animazione dei riquadri live (ms)
  $description:it-IT: "Questa impostazione definisce quanti millisecondi passano tra un flip del riquadro Calendario e l'altro quando l'animazione è attiva. Valori più bassi fanno ciclare i riquadri più rapidamente e danno un effetto più movimentato, valori più alti un effetto più calmo. Il valore predefinito di 4000 ms corrisponde al ritmo dei riquadri live originali."
  $name:es-ES: Intervalo de animación de mosaicos en vivo (ms)
  $description:es-ES: "Esta configuración define cuántos milisegundos transcurren entre cada giro del mosaico Calendario cuando la animación está activa. Los valores más bajos hacen que los mosaicos ciclen más rápido y dan un efecto más animado, mientras que los valores más altos producen un efecto más calmado. El valor predeterminado de 4000 ms coincide con el ritmo de los mosaicos en vivo originales."
  $name:fr-FR: Intervalle d'animation des vignettes dynamiques (ms)
  $description:fr-FR: "Ce paramètre définit le nombre de millisecondes entre chaque retournement de la vignette Calendrier lorsque l'animation est activée. Les valeurs plus faibles font défiler les vignettes plus rapidement et donnent un effet plus animé, tandis que les valeurs plus élevées produisent un effet plus calme. La valeur par défaut de 4000 ms correspond au rythme des vignettes dynamiques d'origine."
  $name:pt-PT: Intervalo da animação dos blocos dinâmicos (ms)
  $description:pt-PT: "Esta definição define quantos milissegundos decorrem entre cada inversão do bloco Calendário quando a animação está ativa. Valores mais baixos fazem os blocos ciclar mais rapidamente e dão um efeito mais movimentado, enquanto valores mais altos produzem um efeito mais calmo. A predefinição de 4000 ms corresponde ao ritmo dos blocos dinâmicos originais."
- menuWidthOverride: 0
  $name: Menu width override in px (0 = default)
  $description: "This setting overrides the width of the Start menu in pixels (measured at 100% scaling). A value of 0 keeps the default width, or the width the menu was last resized to. Any positive value forces the menu to that width, even if the menu was resized by dragging its edge. The default 533 px holds the 267 px left column plus the 266 px tile pane (two medium tile columns of small 56 px cells), as on the RT Update 3 menu."
  $name:it-IT: Forza la larghezza del menu in px (0 = predefinita)
  $description:it-IT: "Questa impostazione forza la larghezza del menu Start in pixel (misurata al 100% di scala). Il valore 0 mantiene la larghezza predefinita oppure quella a cui il menu è stato ridimensionato l'ultima volta. Qualsiasi valore positivo forza il menu a quella larghezza, anche se il menu è stato ridimensionato trascinando il bordo. Il valore predefinito di 533 px contiene la colonna sinistra di 267 px più il pannello dei riquadri di 266 px (due colonne di riquadri medi di celle piccole da 56 px), come nel menu di RT Update 3."
  $name:es-ES: Ancho del menú forzado en px (0 = predeterminado)
  $description:es-ES: "Esta configuración fuerza el ancho del menú Inicio en píxeles (medido al 100% de escala). Un valor de 0 mantiene el ancho predeterminado o el al que el menú se redimensionó por última vez. Cualquier valor positivo fuerza el menú a ese ancho, aunque el menú se haya redimensionado arrastrando su borde. Los 533 px predeterminados contienen la columna izquierda de 267 px más el panel de mosaicos de 266 px (dos columnas de mosaicos medianos de celdas pequeñas de 56 px), como en el menú de RT Update 3."
  $name:fr-FR: Forcer la largeur du menu en px (0 = par défaut)
  $description:fr-FR: "Ce paramètre force la largeur du menu Démarrer en pixels (mesurée à 100 % de mise à l'échelle). Une valeur de 0 conserve la largeur par défaut, ou celle à laquelle le menu a été redimensionné la dernière fois. Toute valeur positive force le menu à cette largeur, même si le menu a été redimensionné en faisant glisser son bord. Les 533 px par défaut contiennent la colonne de gauche de 267 px plus le panneau de vignettes de 266 px (deux colonnes de vignettes moyennes de petites cellules de 56 px), comme dans le menu de RT Update 3."
  $name:pt-PT: Impor a largura do menu em px (0 = predefinição)
  $description:pt-PT: "Esta definição impõe a largura do menu Iniciar em píxeis (medida a 100% de escala). Um valor de 0 mantém a largura predefinida ou aquela em que o menu foi redimensionado pela última vez. Qualquer valor positivo impõe o menu a essa largura, mesmo que o menu tenha sido redimensionado arrastando o seu contorno. Os 533 px predefinidos contêm a coluna esquerda de 267 px mais o painel de blocos de 266 px (duas colunas de blocos médios de células pequenas de 56 px), como no menu do RT Update 3."
- profilePicturePath: ""
  $name: Profile picture file path (empty = use Windows account picture, then bundled avatar fallback)
  $description: "This setting specifies a custom file path to an image that will be used as the profile picture shown in the Start menu. When left empty, the mod falls back to the Windows account picture, and if that is unavailable it uses the bundled fallback avatar shipped with the mod, ensuring a picture is always displayed."
  $name:it-IT: Percorso dell'immagine del profilo (vuoto = immagine dell'account Windows, quindi avatar incluso)
  $description:it-IT: "Questa impostazione specifica un percorso di file a un'immagine da usare come immagine del profilo mostrata nel menu Start. Se lasciato vuoto, il mod usa l'immagine dell'account Windows e, se non è disponibile, l'avatar incluso distribuito con il mod, così che venga sempre mostrata un'immagine."
  $name:es-ES: Ruta del archivo de imagen de perfil (vacío = imagen de la cuenta de Windows, luego avatar incluido)
  $description:es-ES: "Esta configuración especifica una ruta de archivo a una imagen que se usará como imagen de perfil mostrada en el menú Inicio. Si se deja vacío, el mod usa la imagen de la cuenta de Windows y, si no está disponible, el avatar incluido que se distribuye con el mod, asegurando que siempre se muestre una imagen."
  $name:fr-FR: Chemin du fichier de photo de profil (vide = photo du compte Windows, puis avatar intégré)
  $description:fr-FR: "Ce paramètre spécifie un chemin de fichier vers une image à utiliser comme photo de profil affichée dans le menu Démarrer. Lorsqu'il est laissé vide, le mod utilise la photo du compte Windows, et si elle n'est pas disponible, l'avatar intégré fourni avec le mod, ce qui garantit qu'une image est toujours affichée."
  $name:pt-PT: Caminho da imagem de perfil (vazio = imagem da conta do Windows, depois avatar incluído)
  $description:pt-PT: "Esta definição especifica um caminho de ficheiro para uma imagem a usar como imagem de perfil mostrada no menu Iniciar. Quando deixado vazio, o mod usa a imagem da conta do Windows e, se não estiver disponível, o avatar incluído distribuído com o mod, garantindo que é sempre mostrada uma imagem."
- privacyMode: false
  $name: Privacy mode (hide account name)
  $description: "This setting replaces the Windows account name with a generic localized label in the Start menu."
  $name:it-IT: Modalità privacy (nascondi il nome dell'account)
  $description:it-IT: "Questa impostazione sostituisce il nome dell’account Windows con una dicitura generica tradotta nel menu Start."
  $name:es-ES: Modo privacidad (ocultar el nombre de la cuenta)
  $description:es-ES: "Esta configuración sustituye el nombre de la cuenta de Windows por una etiqueta genérica traducida en el menú Inicio."
  $name:fr-FR: Mode confidentialité (masquer le nom du compte)
  $description:fr-FR: "Ce paramètre remplace le nom du compte Windows par une étiquette générique traduite dans le menu Démarrer."
  $name:pt-PT: Modo privacidade (ocultar o nome da conta)
  $description:pt-PT: "Esta definição substitui o nome da conta Windows por uma designação genérica traduzida no menu Iniciar."
- tileLayout: ""
  $name: Import tile layout (one-time, empty = none)
  $description: "This setting allows to paste a serialized tile layout here (a \"WHSM2#...\" string) to import it. The layout is applied once, the first time the value changes; after that, your own tile drags, resizes and pins take precedence again. The current layout is otherwise saved automatically in Windhawk's mod storage, so most users can leave this empty."
  $name:it-IT: Importa disposizione dei riquadri (una tantum, vuoto = nessuna)
  $description:it-IT: "Questa impostazione permette di incollare qui una disposizione dei riquadri serializzata (una stringa WHSM2#...) per importarla. La disposizione viene applicata una sola volta, la prima volta che il valore cambia; dopodiché i tuoi trascinamenti, ridimensionamenti e fissaggi dei riquadri tornano a prevalere. La disposizione corrente viene inoltre salvata automaticamente nell'archivio del mod di Windhawk, quindi la maggior parte degli utenti può lasciare questo campo vuoto."
  $name:es-ES: Importar disposición de mosaicos (una vez, vacío = ninguna)
  $description:es-ES: "Esta configuración permite pegar aquí una disposición de mosaicos serializada (una cadena WHSM2#...) para importarla. La disposición se aplica una sola vez, la primera vez que cambia el valor; después, tus propios arrastres, redimensionados y anclajes de mosaicos vuelven a prevalecer. La disposición actual se guarda automáticamente en el almacenamiento del mod de Windhawk, así que la mayoría de los usuarios pueden dejar esto vacío."
  $name:fr-FR: Importer une disposition de vignettes (une fois, vide = aucune)
  $description:fr-FR: "Ce paramètre permet de coller ici une disposition de vignettes sérialisée (une chaîne WHSM2#...) pour l'importer. La disposition est appliquée une seule fois, la première fois que la valeur change ; ensuite, vos propres glisser-déposer, redimensionnements et épinglages de vignettes reprennent le dessus. La disposition actuelle est par ailleurs enregistrée automatiquement dans le stockage du mod de Windhawk, donc la plupart des utilisateurs peuvent laisser ceci vide."
  $name:pt-PT: Importar disposição de blocos (uma vez, vazio = nenhuma)
  $description:pt-PT: "Esta definição permite colar aqui uma disposição de blocos serializada (uma cadeia WHSM2#...) para a importar. A disposição é aplicada uma única vez, na primeira vez que o valor muda; depois, os seus próprios arrastes, redimensionamentos e afixações de blocos voltam a prevalecer. A disposição atual é guardada automaticamente no armazenamento do mod do Windhawk, por isso a maioria dos utilizadores pode deixar isto vazio."
*/
// ==/WindhawkModSettings==
/*
 Overview of how the mod works
 ----------------------------
 * Anchoring: the Start button rectangle is read from the Win32 "Start" child
   window (Windows 10), from UI Automation (AutomationId "StartButton", Windows 11
   XAML taskbar) or, as a last resort, derived from the taskbar rectangle. Primary
   and secondary taskbars are both handled. The menu is placed against the
   taskbar edge, clamped to the monitor work area, and laid out at that monitor's
   DPI (GetDpiForMonitor / WM_DPICHANGED).
 * Opening the menu: a WH_KEYBOARD_LL hook turns a bare Win tap into a toggle of
   the custom menu (an inert key, VK 0xE8, is injected before the Win release so
   the native menu is not opened; the release itself is passed through). A
   WH_MOUSE_LL hook swallows a left click on the Start button only when the
   window under the cursor belongs to a taskbar. Both hooks live on a dedicated
   thread that does nothing but pump messages, so slow work on the UI thread can
   never trip LowLevelHooksTimeout. WH_GETMESSAGE hooks on the Progman and
   taskbar threads drop WM_SYSCOMMAND/SC_TASKLIST. The cached Start rectangles are
   recomputed on the UI thread when the mouse enters a taskbar, when the menu
   opens, and on display/work-area changes; there is no periodic UIA polling.
 * Keyboard: the menu window takes the focus when it opens, so arrow keys,
   Tab, Home/End, Page Up/Down, Enter and the context-menu key are handled by
   its own window procedure. The keyboard highlight reuses the hover state, so
   only one element is ever highlighted; moving the mouse hands the highlight
   back to the pointer.
 * Tiles: front face = icon + name. Only the Calendar tile has a back face
   (weekday + date from GetDateFormatEx), because that is the only live content
   that can be computed locally: the documented tile APIs (TileUpdateManager /
   TileUpdater) are write-only and scoped to the calling app, so the real
   content of other apps' live tiles cannot be read. The flip is off by default
   and is a cosine deformation drawn into a GDI double buffer.
 * Lifetime: the menu window, catalog, GDI+ objects and hooks are created and
   destroyed on the mod's own threads. Globals with non-trivial teardown are
   marked [[clang::no_destroy]] and released explicitly when the UI thread exits,
   so nothing runs under the loader lock when Explorer exits.
*/

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define SECURITY_WIN32

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <sddl.h>
#include <uiautomation.h>
#include <uxtheme.h>
#include <shellscalingapi.h>
#include <powrprof.h>
#include <security.h>
#include <windhawk_utils.h>
#include <wrl/client.h>

#include <algorithm>
#include <cwctype>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <climits>
#include <exception>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using Microsoft::WRL::ComPtr;
using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;

// GDI+ text for every painted string. Face, size and weight come from the font
// selected in the DC and the colour from SetTextColor, so call sites keep their
// GDI setup; measuring (DT_CALCRECT) and any failure fall back to DrawTextW.
static int DrawTextG(HDC dc, LPCWSTR text, int len, LPRECT rc, UINT fmt) {
    if (!g_gdiplusToken || !text || !rc || (fmt & DT_CALCRECT)) return DrawTextW(dc, text, len, rc, fmt);
    LOGFONTW lf{};
    HGDIOBJ hf = GetCurrentObject(dc, OBJ_FONT);
    if (!hf || !GetObjectW(hf, sizeof(lf), &lf)) return DrawTextW(dc, text, len, rc, fmt);
    Gdiplus::Graphics g(dc);
    Gdiplus::Font font(dc, &lf);
    if (g.GetLastStatus() != Gdiplus::Ok || font.GetLastStatus() != Gdiplus::Ok) return DrawTextW(dc, text, len, rc, fmt);
    Gdiplus::StringFormat sf(Gdiplus::StringFormat::GenericTypographic());
    INT flags = (fmt & DT_NOCLIP) ? Gdiplus::StringFormatFlagsNoClip : 0;
    if (!(fmt & DT_WORDBREAK)) flags |= Gdiplus::StringFormatFlagsNoWrap;
    sf.SetFormatFlags(flags);
    sf.SetAlignment((fmt & DT_RIGHT) ? Gdiplus::StringAlignmentFar : (fmt & DT_CENTER) ? Gdiplus::StringAlignmentCenter : Gdiplus::StringAlignmentNear);
    sf.SetLineAlignment((fmt & DT_VCENTER) ? Gdiplus::StringAlignmentCenter : (fmt & DT_BOTTOM) ? Gdiplus::StringAlignmentFar : Gdiplus::StringAlignmentNear);
    sf.SetTrimming((fmt & DT_END_ELLIPSIS) ? Gdiplus::StringTrimmingEllipsisCharacter : Gdiplus::StringTrimmingNone);
    sf.SetHotkeyPrefix((fmt & DT_HIDEPREFIX) ? Gdiplus::HotkeyPrefixHide : Gdiplus::HotkeyPrefixNone);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    COLORREF c = GetTextColor(dc);
    Gdiplus::SolidBrush br(Gdiplus::Color(255, GetRValue(c), GetGValue(c), GetBValue(c)));
    Gdiplus::RectF lay((Gdiplus::REAL)rc->left, (Gdiplus::REAL)rc->top, (Gdiplus::REAL)(rc->right - rc->left), (Gdiplus::REAL)(rc->bottom - rc->top));
    return g.DrawString(text, len, &font, lay, &sf, &br) == Gdiplus::Ok ? 1 : DrawTextW(dc, text, len, rc, fmt);
}
static HICON g_stockFolderIcon = nullptr;
static HICON g_stockFolderIconSmall = nullptr;
static bool g_stockFolderIconTriedSmall = false;
static bool g_stockFolderIconTriedLarge = false;
static HICON g_stockAppIcon = nullptr;
// Populated and used on the menu UI thread only. Cleared before catalog icon
// handles are replaced, so a recycled HICON value cannot return a stale bitmap.
// [[clang::no_destroy]]: the GDI+ bitmaps are released explicitly by the UI
// thread before GdiplusShutdown; the automatic destructor must not run at
// process exit (see the Windhawk wiki, "Global objects and process shutdown").
[[clang::no_destroy]] static std::optional<
    std::unordered_map<HICON, std::unique_ptr<Gdiplus::Bitmap>>>
    g_iconBitmapCache{std::in_place};
static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size);

constexpr UINT WM_APP_TOGGLE = WM_APP + 101;
constexpr UINT WM_APP_HIDE = WM_APP + 102;
constexpr UINT WM_APP_REFRESH = WM_APP + 103;
constexpr UINT WM_APP_PROFILE_PHOTO_READY = WM_APP + 104;
constexpr UINT WM_APP_REFRESH_ANCHOR = WM_APP + 105;
constexpr UINT WM_APP_SEARCH_CATALOG_READY = WM_APP + 106;
constexpr UINT WM_APP_SEARCH_ICON_READY = WM_APP + 107;
constexpr UINT WM_APP_BRIDGE_TOGGLE = WM_APP + 108;
constexpr UINT WM_APP_INITIAL_CATALOG_READY = WM_APP + 109;
constexpr UINT_PTR TIMER_LIVE_FLIP = 2001;
constexpr UINT_PTR TIMER_LIVE_TICK = 2002;
constexpr UINT_PTR TIMER_ALL_APPS_SCROLL_REPEAT = 2004;
constexpr UINT_PTR TIMER_TILE_ANIM = 2005;
constexpr UINT_PTR TIMER_SEARCH_CARET = 2006;
constexpr UINT_PTR TIMER_CATALOG_PUBLISH = 2007;
constexpr UINT_PTR TIMER_CATALOG_ENRICH = 2008;
constexpr int TILE_PRESSED_SCALE_PERCENT = 94; // subtle press feedback; inspired by the 8045 reference mod.

// Tile movement animations, ported from the Windows 8 (8045) Start screen mod:
// a tile keeps an animated offset from its slot and eases into it exponentially
// instead of jumping, the pressed tile shrinks a little and the dragged one is
// lifted above the grid, slightly enlarged and semi-transparent. Only painting
// is animated: hit testing and the saved layout keep using the slot rectangles.
constexpr UINT TILE_ANIM_INTERVAL_MS = 16;      // ~60 frames per second
constexpr float TILE_ANIM_GLIDE_RATE = 16.0f;   // k = 1 - exp(-dt * rate)
constexpr float TILE_ANIM_SCALE_RATE = 22.0f;   // press/lift scale easing
constexpr float TILE_ANIM_SETTLE_PX = 0.3f;     // below this the glide is over
constexpr float TILE_ANIM_SETTLE_SCALE = 0.001f;
constexpr float TILE_PRESSED_SCALE = TILE_PRESSED_SCALE_PERCENT / 100.0f;
constexpr float TILE_DRAG_SCALE = 1.05f;        // the lifted tile grows by 5%
constexpr BYTE TILE_DRAG_ALPHA = 235;           // ~92% opacity, as in the reference
constexpr BYTE kMenuOpacity = 255; // the Update 3 menu was fully opaque
// The original 1507 Showing/Hiding paths pass 0x50 to private DWM calls.
// The RT Update 3 menu, however, appears and disappears with no slide at all:
// the leaked hardware photos and the reference recreations show an instant
// popup, so no animation is played here either.

// Context menu command identifiers
constexpr UINT IDM_OPEN_APP         = 3001;
constexpr UINT IDM_RUNAS_ADMIN      = 3002;
constexpr UINT IDM_OPEN_LOCATION    = 3003;
constexpr UINT IDM_PIN_TO_START     = 3004;
constexpr UINT IDM_UNPIN_FROM_START = 3005;
constexpr UINT IDM_RESIZE_SMALL     = 3006;
constexpr UINT IDM_RESIZE_MEDIUM    = 3007;
constexpr UINT IDM_RESIZE_WIDE      = 3008;
constexpr UINT IDM_RESIZE_LARGE     = 3009;
constexpr UINT IDM_TOGGLE_LIVE      = 3010;

constexpr UINT IDM_USER_SETTINGS    = 3101;
constexpr UINT IDM_USER_LOCK        = 3102;
constexpr UINT IDM_USER_SIGNOUT     = 3103;

constexpr UINT IDM_POWER_SLEEP      = 3201;
constexpr UINT IDM_POWER_SHUTDOWN   = 3202;
constexpr UINT IDM_POWER_RESTART    = 3203;
constexpr UINT IDM_POWER_HIBERNATE  = 3204;

// ===================== ERROR HANDLING =====================
#define GUARDED(label, ...)                                                     \
    try {                                                                        \
        __VA_ARGS__                                                              \
    } catch (const std::exception& _e) {                                         \
        Wh_Log(L"[%s] C++ Exception: %S", label, _e.what());                     \
    } catch (...) {                                                              \
        Wh_Log(L"[%s] Unknown exception caught", label);                         \
    }

// ===================== RAII WRAPPERS =====================
template <typename T, typename Deleter>
class UniqueRes {
    T h_{};
public:
    UniqueRes() = default;
    explicit UniqueRes(T h) : h_(h) {}
    UniqueRes(const UniqueRes&) = delete;
    UniqueRes& operator=(const UniqueRes&) = delete;
    UniqueRes(UniqueRes&& o) noexcept : h_(o.h_) { o.h_ = T{}; }
    UniqueRes& operator=(UniqueRes&& o) noexcept {
        if (this != &o) reset(o.release());
        return *this;
    }
    ~UniqueRes() { reset(); }

    T get() const { return h_; }
    T release() { T t = h_; h_ = T{}; return t; }
    void reset(T h = T{}) {
        if (h_) Deleter()(h_);
        h_ = h;
    }
    explicit operator bool() const { return h_ != T{}; }
};

struct DelObj  { void operator()(HGDIOBJ h) const { DeleteObject(h); } };
struct DelIcon { void operator()(HICON h)   const { DestroyIcon(h); } };
struct DelWnd  { void operator()(HWND h)    const { DestroyWindow(h); } };
struct DelHnd  { void operator()(HANDLE h)  const { CloseHandle(h); } };
struct DelDC   { void operator()(HDC h)     const { DeleteDC(h); } };

using UniqueBitmap = UniqueRes<HBITMAP, DelObj>;
using UniqueFont   = UniqueRes<HFONT, DelObj>;
using UniqueBrush  = UniqueRes<HBRUSH, DelObj>;
using UniqueRegion = UniqueRes<HRGN, DelObj>;
using UniquePen    = UniqueRes<HPEN, DelObj>;
using UniqueIcon   = UniqueRes<HICON, DelIcon>;
using UniqueHwnd   = UniqueRes<HWND, DelWnd>;
using UniqueHandle = UniqueRes<HANDLE, DelHnd>;
using UniqueMemDC  = UniqueRes<HDC, DelDC>;

// FromHICON can lose semitransparent edge pixels on some legacy Shell icons.
// Copy the documented ICONINFO color bitmap and, when its alpha plane is empty,
// reconstruct transparency from the documented AND mask before handing pixels
// to GDI+ as straight 32-bit ARGB.
static std::unique_ptr<Gdiplus::Bitmap> CreateGdiplusBitmapFromIcon(HICON icon) {
    ICONINFO iconInfo{};
    if (!icon || !GetIconInfo(icon, &iconInfo)) return {};
    UniqueBitmap color(iconInfo.hbmColor);
    UniqueBitmap mask(iconInfo.hbmMask);
    if (!color) return {}; // monochrome icon; caller uses the native fallback

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
    colorInfo.bmiHeader.biHeight = -height; // top-down rows
    colorInfo.bmiHeader.biPlanes = 1;
    colorInfo.bmiHeader.biBitCount = 32;
    colorInfo.bmiHeader.biCompression = BI_RGB;
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return {};
    const int colorLines = GetDIBits(screenDc, color.get(), 0, static_cast<UINT>(height),
                                    pixels.data(), &colorInfo, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screenDc);
    if (colorLines != height) return {};

    bool hasAlpha = false;
    bool hasPartialAlpha = false;
    bool alphaAllOpaque = true;
    for (size_t i = 3; i < pixels.size(); i += 4) {
        const BYTE a = pixels[i];
        hasAlpha = hasAlpha || a != 0;
        hasPartialAlpha = hasPartialAlpha || (a != 0 && a != 255);
        alphaAllOpaque = alphaAllOpaque && a == 255;
    }

    std::vector<BYTE> maskBits;
    size_t maskStride = ((static_cast<size_t>(width) + 31u) / 32u) * 4u;
    bool hasTransparentMaskPixel = false;
    if (mask) {
        maskBits.resize(maskStride * static_cast<size_t>(height));
        struct MaskBitmapInfo {
            BITMAPINFOHEADER header;
            RGBQUAD colors[2];
        } maskInfo{};
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
                        if ((row[x / 8] & (0x80u >> (x % 8))) != 0) {
                            hasTransparentMaskPixel = true;
                            break;
                        }
                    }
                }
            } else {
                maskBits.clear();
            }
        } else {
            maskBits.clear();
        }
    }

    // 32-bit icons with genuine partial alpha use that channel. Older icons
    // commonly have an all-zero alpha plane (or an opaque plane plus AND mask).
    // If neither a useful alpha plane nor a readable AND mask exists, let
    // DrawIconEx render the HICON rather than caching a potentially black box.
    if (!hasAlpha && maskBits.empty()) return {};
    const bool useMask = !hasPartialAlpha &&
                         ((!hasAlpha && !maskBits.empty()) ||
                          (alphaAllOpaque && hasTransparentMaskPixel));
    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppARGB);
    if (bitmap->GetLastStatus() != Gdiplus::Ok) return {};
    Gdiplus::Rect bounds(0, 0, width, height);
    Gdiplus::BitmapData dst{};
    if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeWrite,
                         PixelFormat32bppARGB, &dst) != Gdiplus::Ok) return {};
    for (int y = 0; y < height; ++y) {
        const BYTE* src = pixels.data() + static_cast<size_t>(y) * static_cast<size_t>(width) * 4;
        BYTE* out = static_cast<BYTE*>(dst.Scan0) + static_cast<ptrdiff_t>(y) * dst.Stride;
        const BYTE* maskRow = maskBits.empty() ? nullptr : maskBits.data() + static_cast<size_t>(y) * maskStride;
        for (int x = 0; x < width; ++x) {
            BYTE alpha = src[x * 4 + 3];
            if (useMask && maskRow) {
                alpha = (maskRow[x / 8] & (0x80u >> (x % 8))) ? 0 : 255;
            } else if (!hasAlpha && !maskRow) {
                alpha = 255;
            }
            out[x * 4 + 0] = alpha ? src[x * 4 + 0] : 0;
            out[x * 4 + 1] = alpha ? src[x * 4 + 1] : 0;
            out[x * 4 + 2] = alpha ? src[x * 4 + 2] : 0;
            out[x * 4 + 3] = alpha;
        }
    }
    bitmap->UnlockBits(&dst);
    return bitmap;
}

// Derive a stable brand-like tile fill from the icon's dominant saturated hue.
// Called when a tile is attached/refreshed, never from the paint loop.
static COLORREF GetIconAccentColor(HICON icon, COLORREF fallback = RGB(10, 91, 196)) {
    try {
        auto bitmap = CreateGdiplusBitmapFromIcon(icon);
        if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) return fallback;
        const int width = static_cast<int>(bitmap->GetWidth());
        const int height = static_cast<int>(bitmap->GetHeight());
        if (width <= 0 || height <= 0) return fallback;

        Gdiplus::Rect bounds(0, 0, width, height);
        Gdiplus::BitmapData data{};
        if (bitmap->LockBits(&bounds, Gdiplus::ImageLockModeRead,
                             PixelFormat32bppARGB, &data) != Gdiplus::Ok) return fallback;
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
                if (hi < 0.18 || delta < 0.12) continue; // ignore shadows and neutral pixels
                double hue = 0.0;
                if (hi == r) hue = 60.0 * std::fmod((g - b) / delta, 6.0);
                else if (hi == g) hue = 60.0 * ((b - r) / delta + 2.0);
                else hue = 60.0 * ((r - g) / delta + 4.0);
                if (hue < 0.0) hue += 360.0;
                const int bin = std::min(23, static_cast<int>(hue / 15.0));
                const double w = alpha * (delta / std::max(0.01, hi));
                weight[bin] += w;
                sumR[bin] += r * w;
                sumG[bin] += g * w;
                sumB[bin] += b * w;
            }
        }
        bitmap->UnlockBits(&data);
        int best = 0;
        for (int i = 1; i < 24; ++i) if (weight[i] > weight[best]) best = i;
        if (weight[best] < 0.5) return fallback;
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

// Only use a sampled fill when an Xbox icon has an unmistakable, opaque,
// nearly uniform green square at its edges. Transparent/complex icons retain
// their existing palette. This runs when the tile is attached, not while painting.
static bool XboxIconBackground(const std::wstring& name, const std::wstring& aumid,
                               HICON icon, COLORREF& color) {
    std::wstring id=name+L" "+aumid;
    std::transform(id.begin(),id.end(),id.begin(),::towlower);
    if (id.find(L"xbox")==std::wstring::npos || !icon) return false;
    auto bitmap=CreateGdiplusBitmapFromIcon(icon);
    if (!bitmap) return false;
    const int w=static_cast<int>(bitmap->GetWidth()),h=static_cast<int>(bitmap->GetHeight());
    if (w<8 || h<8) return false;
    Gdiplus::Rect rect(0,0,w,h); Gdiplus::BitmapData data{};
    if (bitmap->LockBits(&rect,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&data)!=Gdiplus::Ok) return false;
    int sums[3]{}, minC[3]{255,255,255}, maxC[3]{}; bool valid=true;
    const int coords[8][2]={{1,1},{4,1},{7,1},{1,4},{7,4},{1,7},{4,7},{7,7}};
    for (const auto& point:coords) {
        int x=std::min(w-1,point[0]*w/8),y=std::min(h-1,point[1]*h/8);
        const BYTE* px=static_cast<const BYTE*>(data.Scan0)+static_cast<ptrdiff_t>(y)*data.Stride+x*4;
        if (px[3]<250) { valid=false; break; }
        for(int i=0;i<3;++i) { sums[i]+=px[i]; minC[i]=std::min(minC[i],int(px[i])); maxC[i]=std::max(maxC[i],int(px[i])); }
    }
    bitmap->UnlockBits(&data);
    if (!valid) return false;
    for(int i=0;i<3;++i) if(maxC[i]-minC[i]>12) return false;
    const int b=(sums[0]+4)/8,g=(sums[1]+4)/8,r=(sums[2]+4)/8;
    if (g<45 || g<r+25 || g<b+25) return false;
    color=RGB(r,g,b); return true;
}

static void DrawAppIconOrFallback(HDC dc, HICON icon, int x, int y, int size) {
    if (!dc || size <= 0) return;
    RECT clip{}, iconRect{x, y, x + size, y + size}, visible{};
    if (GetClipBox(dc, &clip) == ERROR || !IntersectRect(&visible, &clip, &iconRect)) return;
    if (!icon && !g_stockAppIcon) {
        SHSTOCKICONINFO stock{}; stock.cbSize=sizeof(stock);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION,SHGSI_ICON|SHGSI_LARGEICON,&stock)))
            g_stockAppIcon=stock.hIcon;
    }
    if (!icon) icon=g_stockAppIcon;
    if (!icon) return;

    if (g_gdiplusToken && g_iconBitmapCache) {
        auto it = g_iconBitmapCache->find(icon);
        if (it == g_iconBitmapCache->end()) {
            auto bitmap = CreateGdiplusBitmapFromIcon(icon);
            if (bitmap) it = g_iconBitmapCache->emplace(icon, std::move(bitmap)).first;
        }
        if (it != g_iconBitmapCache->end()) {
            Gdiplus::Graphics graphics(dc);
            if (graphics.GetLastStatus() == Gdiplus::Ok) {
                graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                if (graphics.DrawImage(it->second.get(), x, y, size, size) == Gdiplus::Ok)
                    return;
            }
        }
    }
    // Native fallback for monochrome/unsupported icons or GDI+ errors.
    DrawIconEx(dc, x, y, icon, size, size, 0, nullptr, DI_NORMAL);
}

// Flat manila folder drawn only when the shell exposes no stock folder icon
// at all (minimal Wine prefixes); on Windows the real shell image is used.
static void DrawFlatFolderGlyph(HDC dc, int x, int y, int size) {
    if (!dc || size <= 0) return;
    RECT clip{}, folderRect{x, y, x + size, y + size}, visible{};
    if (GetClipBox(dc, &clip) == ERROR || !IntersectRect(&visible, &clip, &folderRect)) return;
    const int backTop = y + (size * 3) / 16;
    const int backRight = x + size - (size * 2) / 16;
    const int backBottom = y + (size * 13) / 16;
    const int frontTop = y + (size * 6) / 16;
    const int frontBottom = y + (size * 15) / 16;
    RECT back = {x + (size * 1) / 16, backTop, backRight, backBottom};
    RECT front = {x, frontTop, x + size, frontBottom};
    UniqueBrush backBrush(CreateSolidBrush(RGB(0xCE, 0x9A, 0x2A)));
    UniqueBrush frontBrush(CreateSolidBrush(RGB(0xF5, 0xC2, 0x44)));
    if (backBrush) FillRect(dc, &back, backBrush.get());
    if (frontBrush) FillRect(dc, &front, frontBrush.get());
}

// The byte buffer handed over by the worker is protected by a mutex; the bitmap
// and its stream stay confined to the UI thread. The destruction order keeps the
// stream alive for as long as GDI+ uses the image.
static std::mutex g_profilePhotoMutex;
static std::mutex g_profilePhotoPathMutex;
static std::wstring g_profilePhotoPath;
static std::vector<BYTE> g_profilePhotoPending;
// Account display name resolved by the worker (GetUserNameExW may contact a
// domain controller, so it never runs on the UI thread). Guarded by
// g_profilePhotoMutex; g_accountNamePendingValid marks a fresh value.
static std::wstring g_accountNamePending;
static bool g_accountNamePendingValid = false;
static ComPtr<IStream> g_profilePhotoStream;
// Released explicitly by the UI thread before GdiplusShutdown; never at process exit.
[[clang::no_destroy]] static std::unique_ptr<Gdiplus::Bitmap> g_profilePhoto;
// Bundled copy of the supplied neutral profile-avatar PNG. Keeping its stream alive is
// required for the lifetime of the GDI+ Bitmap created from it.
static const BYTE g_defaultProfileAvatarPng[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x94, 0x08, 0x02, 0x00, 0x00, 0x00, 0xFA, 0x5E, 0x97,
    0x83, 0x00, 0x00, 0x10, 0x00, 0x49, 0x44, 0x41, 0x54, 0x78, 0x01, 0xEC, 0xDD, 0x09, 0x88, 0x96,
    0x55, 0xDB, 0x07, 0xF0, 0x98, 0x37, 0xDA, 0x43, 0x5B, 0xB4, 0xA8, 0x28, 0xDA, 0x24, 0x49, 0x24,
    0x4C, 0x22, 0x68, 0xCF, 0xB0, 0x98, 0xA2, 0x85, 0xA4, 0xC8, 0x8A, 0x0A, 0x4A, 0x5A, 0x6C, 0x37,
    0xA9, 0x14, 0x23, 0xD3, 0x16, 0x93, 0x4C, 0x9A, 0x0A, 0x6C, 0x33, 0x0A, 0x25, 0x29, 0x5A, 0xA8,
    0xA1, 0x24, 0xDB, 0xF7, 0xD2, 0x42, 0x02, 0xA5, 0xA0, 0x22, 0xEA, 0x6B, 0x99, 0x16, 0xA5, 0x68,
    0xB1, 0x82, 0xEF, 0x37, 0xDE, 0x76, 0x3C, 0xDE, 0xF3, 0xCC, 0xCC, 0x33, 0xCF, 0xDC, 0xDB, 0xE4,
    0x33, 0x9C, 0xEF, 0xFE, 0xAE, 0x73, 0xDD, 0xE7, 0x5C, 0xD7, 0xFF, 0xFA, 0xFF, 0xCF, 0xB9, 0xB7,
    0xE7, 0x7D, 0x7D, 0x5B, 0x36, 0x19, 0x98, 0x7F, 0x23, 0x47, 0x8E, 0x1C, 0x37, 0x6E, 0xDC, 0x05,
    0x17, 0x5C, 0x70, 0xFD, 0xF5, 0xD7, 0xCF, 0x99, 0x33, 0x67, 0xC1, 0x82, 0x05, 0xCF, 0x3F, 0xFF,
    0xFC, 0x1B, 0x6F, 0xBC, 0xF1, 0xF1, 0xC7, 0x1F, 0xAF, 0x58, 0xB1, 0xE2, 0x8B, 0x2F, 0xBE, 0xF8,
    0xBF, 0xE8, 0x4F, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x13, 0x4D, 0x17, 0x44, 0xA8,
    0x81, 0xC9, 0xC1, 0x26, 0x03, 0x49, 0xBC, 0xE1, 0xC3, 0x87, 0x63, 0xFC, 0x89, 0x27, 0x9E, 0xA0,
    0x01, 0x01, 0x66, 0xCF, 0x9E, 0x3D, 0x63, 0xC6, 0x8C, 0xC9, 0x93, 0x27, 0x9F, 0x77, 0xDE, 0x79,
    0x63, 0xC7, 0x8E, 0x1D, 0x3D, 0x7A, 0xF4, 0xBE, 0xFB, 0xEE, 0x3B, 0x64, 0xC8, 0x90, 0x41, 0x83,
    0x06, 0x6D, 0xB6, 0xD9, 0x66, 0xB1, 0x1E, 0xBA, 0x9C, 0x4E, 0x19, 0x60, 0x98, 0xC1, 0xA6, 0x98,
    0x68, 0xBA, 0x20, 0x42, 0x09, 0x28, 0xAC, 0xE0, 0x52, 0xC4, 0x13, 0x2B, 0x6E, 0x57, 0x5D, 0x3C,
    0x3B, 0x03, 0xC5, 0xB6, 0x8B, 0x8D, 0xB4, 0x74, 0xE9, 0x52, 0x8C, 0x1F, 0x71, 0xC4, 0x11, 0x34,
    0xA0, 0x04, 0x49, 0xFE, 0xF9, 0xE7, 0x9F, 0xDF, 0xD6, 0xFE, 0xAD, 0x59, 0xFB, 0xA7, 0xAB, 0x75,
    0xC7, 0xB8, 0x53, 0xDA, 0xDA, 0x81, 0x6B, 0xD6, 0x4E, 0xFA, 0x4D, 0x57, 0x10, 0xA1, 0x04, 0x14,
    0x56, 0x70, 0x29, 0x24, 0x92, 0x4E, 0x52, 0xA9, 0xBB, 0x0B, 0x55, 0x11, 0x7F, 0x45, 0xC5, 0x3B,
    0xF2, 0xC8, 0x23, 0xEF, 0xBF, 0xFF, 0xFE, 0x65, 0xCB, 0x96, 0xD9, 0x19, 0x13, 0x27, 0x4E, 0xB4,
    0x5D, 0xF0, 0xB5, 0x6A, 0xD5, 0x2A, 0xA4, 0x63, 0x1F, 0xE9, 0x1A, 0x4F, 0x26, 0x4D, 0x28, 0x4D,
    0x58, 0xC1, 0xA5, 0x10, 0x53, 0x3A, 0x49, 0xA5, 0x06, 0x00, 0x0C, 0x60, 0x38, 0x2B, 0xD8, 0xAA,
    0x25, 0xDE, 0x09, 0x27, 0x9C, 0x70, 0xCF, 0x3D, 0xF7, 0xB8, 0x39, 0xB5, 0xB7, 0xB7, 0x5B, 0xF8,
    0xBB, 0xEE, 0xBA, 0xAB, 0x9D, 0x81, 0x53, 0xCC, 0x16, 0xC9, 0x9D, 0x74, 0x92, 0x4A, 0x0D, 0x00,
    0x18, 0xC0, 0x80, 0x04, 0x18, 0x78, 0x45, 0xC2, 0xE8, 0x35, 0x57, 0x55, 0xC4, 0x3B, 0xE7, 0x9C,
    0x73, 0xDC, 0x78, 0xDA, 0xDA, 0xDA, 0xC6, 0x8F, 0x1F, 0xEF, 0x3A, 0x66, 0x07, 0xA0, 0xCF, 0x86,
    0xE8, 0xB5, 0x80, 0x5C, 0x07, 0x00, 0x00, 0x06, 0x30, 0x20, 0x01, 0x06, 0x1E, 0x90, 0xA0, 0xE6,
    0x9A, 0xB4, 0xFE, 0xE0, 0x25, 0x8B, 0x77, 0xD0, 0x41, 0x07, 0x79, 0xF0, 0xFB, 0xE4, 0x93, 0x4F,
    0xEE, 0xBD, 0xF7, 0x5E, 0x37, 0x1E, 0x8B, 0x1D, 0x59, 0x28, 0xAB, 0xBF, 0x80, 0x62, 0x46, 0x82,
    0x04, 0x18, 0x78, 0x40, 0x82, 0x0A, 0x30, 0xD8, 0xC0, 0x17, 0x93, 0xBD, 0xBB, 0x2C, 0xA5, 0x89,
    0xE7, 0xB9, 0xCE, 0x85, 0xE8, 0xD1, 0x47, 0x1F, 0x3D, 0xFB, 0xEC, 0xB3, 0x3D, 0x0A, 0x5A, 0xDD,
    0x08, 0xEA, 0x0E, 0x65, 0x75, 0xFC, 0x40, 0x82, 0x0A, 0x30, 0xD8, 0xC0, 0x2B, 0x41, 0x21, 0x65,
    0xC1, 0x2B, 0x41, 0x3C, 0xD5, 0xAA, 0x79, 0xC9, 0x92, 0x25, 0x2E, 0x44, 0xDB, 0x6C, 0xB3, 0x8D,
    0x1B, 0x0C, 0x46, 0xCA, 0xAA, 0xBF, 0xB1, 0xBC, 0x00, 0x83, 0x0D, 0xBC, 0x12, 0x14, 0xA2, 0x1C,
    0x45, 0x35, 0x16, 0xAA, 0x3F, 0xB3, 0x72, 0x14, 0xAF, 0x2B, 0xAC, 0xC1, 0x83, 0x07, 0x7B, 0x97,
    0x7A, 0xFA, 0xE9, 0xA7, 0x4F, 0x3B, 0xED, 0xB4, 0xFF, 0xFD, 0xEF, 0x7F, 0x2E, 0x44, 0x5D, 0xC7,
    0x0C, 0x2C, 0x8F, 0x12, 0x14, 0xA2, 0x1C, 0x45, 0x29, 0x4D, 0x81, 0x45, 0xE2, 0x2F, 0x4E, 0x3C,
    0xF7, 0xF9, 0x97, 0x5E, 0x7A, 0x69, 0xEA, 0xD4, 0xA9, 0xAE, 0x39, 0x96, 0xAD, 0xC5, 0x5B, 0x64,
    0x9D, 0xF9, 0xE5, 0x52, 0x88, 0x72, 0x14, 0xA5, 0x34, 0x05, 0x2A, 0x33, 0xBF, 0x5C, 0xA9, 0xC8,
    0x45, 0x88, 0xB7, 0xE7, 0x9E, 0x7B, 0xFA, 0x8A, 0x31, 0x6B, 0xD6, 0x2C, 0x4F, 0xDE, 0x6E, 0x18,
    0xAA, 0x4D, 0x81, 0xF8, 0x0F, 0x74, 0x15, 0xA5, 0x34, 0x05, 0x2A, 0x53, 0xB1, 0x4A, 0x2E, 0xA0,
    0xA8, 0xDC, 0xC5, 0xBB, 0xE4, 0x92, 0x4B, 0x5E, 0x78, 0xE1, 0x85, 0xD6, 0xD6, 0x56, 0xE5, 0x69,
    0x05, 0x94, 0x54, 0x62, 0x0A, 0x05, 0x6A, 0x8A, 0x55, 0xB2, 0xC2, 0xF3, 0x46, 0x92, 0xA3, 0x78,
    0x56, 0x9F, 0x0F, 0x86, 0xD3, 0xA6, 0x4D, 0x73, 0x49, 0x71, 0x6F, 0xC8, 0xBB, 0x92, 0xEA, 0xC4,
    0x57, 0xAC, 0x92, 0x15, 0xAE, 0x7C, 0x24, 0xE4, 0x07, 0x2C, 0x2F, 0xF1, 0x7C, 0x8C, 0x00, 0x7D,
    0xCC, 0x98, 0x31, 0x56, 0xA2, 0x96, 0x5F, 0x01, 0xD5, 0x8C, 0xAC, 0x64, 0x4D, 0xF9, 0x48, 0x40,
    0x45, 0x4E, 0x20, 0x73, 0x11, 0xCF, 0x73, 0xD7, 0xFC, 0xF9, 0xF3, 0xDD, 0x00, 0xAC, 0xC1, 0x9C,
    0x70, 0x0F, 0x88, 0xB0, 0xCA, 0x47, 0x02, 0x2A, 0x10, 0x92, 0x07, 0xE0, 0xEC, 0xC5, 0xB3, 0xD6,
    0xAE, 0xBA, 0xEA, 0xAA, 0x35, 0x6B, 0xD6, 0x58, 0x7A, 0x79, 0x20, 0x1E, 0x58, 0x31, 0x91, 0x80,
    0x0A, 0x84, 0xA0, 0x25, 0x73, 0xE4, 0x59, 0x8A, 0xE7, 0x57, 0x4D, 0x9F, 0xFE, 0xFC, 0xB6, 0x02,
    0x6E, 0xE6, 0x40, 0x07, 0x74, 0x40, 0x84, 0xA0, 0x05, 0x39, 0x28, 0xCA, 0xB0, 0x90, 0xCC, 0xC4,
    0x73, 0x67, 0x7E, 0xEC, 0xB1, 0xC7, 0xF6, 0xDB, 0x6F, 0x3F, 0x40, 0x33, 0xC4, 0x57, 0x3B, 0xD4,
    0x00, 0xF4, 0xA2, 0x05, 0x39, 0x28, 0x42, 0x54, 0x56, 0xF0, 0xB3, 0x11, 0xCF, 0x9B, 0xA9, 0x87,
    0xE3, 0xED, 0xB7, 0xDF, 0x1E, 0xC4, 0xAC, 0x90, 0xFD, 0xF7, 0xE2, 0x20, 0x07, 0x45, 0x88, 0x42,
    0x57, 0x26, 0xD5, 0x65, 0x20, 0x9E, 0xA7, 0xA9, 0x5B, 0x6E, 0xB9, 0xC5, 0xC3, 0xB1, 0xEB, 0x7B,
    0x26, 0x98, 0xFE, 0xC3, 0x41, 0x50, 0x84, 0x28, 0x74, 0x21, 0xAD, 0xFF, 0x65, 0xF6, 0x57, 0x3C,
    0xAF, 0xA2, 0xF3, 0xE6, 0xCD, 0xF3, 0x7D, 0x0F, 0xAC, 0xFE, 0xA3, 0xD9, 0x18, 0x22, 0x20, 0x0A,
    0x5D, 0x48, 0x43, 0x5D, 0x3F, 0xEB, 0xED, 0x97, 0x78, 0x96, 0xCF, 0x94, 0x29, 0x53, 0x40, 0x01,
    0xA8, 0x9F, 0x38, 0x36, 0xAA, 0xE9, 0xE8, 0x42, 0x1A, 0xEA, 0x10, 0xD8, 0x9F, 0xC2, 0x1B, 0x17,
    0xCF, 0x85, 0xDB, 0xF2, 0x01, 0x02, 0x94, 0xFE, 0x20, 0xD8, 0x38, 0xE7, 0x22, 0x0D, 0x75, 0x08,
    0x44, 0x63, 0xC3, 0x0C, 0x34, 0x28, 0x9E, 0x47, 0x26, 0x0B, 0x47, 0x7A, 0x20, 0x1A, 0xCE, 0xBD,
    0x91, 0x4F, 0x44, 0x1D, 0x02, 0xD1, 0x88, 0xCC, 0xC6, 0xA8, 0x68, 0x44, 0x3C, 0x2F, 0x2B, 0xED,
    0xED, 0xED, 0x6E, 0xBC, 0xD2, 0x37, 0x96, 0xB5, 0x39, 0x2B, 0x61, 0x00, 0x81, 0x68, 0x44, 0x26,
    0x4A, 0x13, 0x4F, 0x9F, 0x8E, 0x8D, 0x88, 0xE7, 0x87, 0xE3, 0x21, 0x43, 0x86, 0x48, 0xDC, 0xA7,
    0x4C, 0xCD, 0xC1, 0x35, 0x19, 0x40, 0x23, 0x32, 0x51, 0x5A, 0xF3, 0x6C, 0xCF, 0xCE, 0x3E, 0x8B,
    0xE7, 0x33, 0x8F, 0x97, 0x4D, 0xAF, 0x2C, 0x3D, 0xC7, 0x1D, 0xB0, 0x67, 0x4B, 0x00, 0x8E, 0x4C,
    0x94, 0x22, 0xB6, 0xAF, 0xB9, 0xFB, 0x26, 0x9E, 0x0F, 0xAC, 0x3E, 0xF3, 0x48, 0xD6, 0xD7, 0x34,
    0xCD, 0xF1, 0x3D, 0x33, 0x80, 0x52, 0xC4, 0xA2, 0xB7, 0xE7, 0x61, 0xA9, 0xB3, 0x7D, 0x10, 0xCF,
    0x73, 0xAD, 0x0F, 0xAC, 0xD2, 0xA4, 0x42, 0x34, 0xBB, 0x99, 0x30, 0x80, 0x58, 0xF4, 0x22, 0xB9,
    0xFE, 0x68, 0xF5, 0x8A, 0xE7, 0x89, 0x68, 0xFA, 0xF4, 0xE9, 0x2E, 0xD0, 0xF5, 0x87, 0x6E, 0x8E,
    0xEC, 0x2B, 0x03, 0xE8, 0x45, 0x32, 0xAA, 0xEB, 0x9C, 0x58, 0xAF, 0x78, 0xB3, 0x67, 0xCF, 0xDE,
    0x6B, 0xAF, 0xBD, 0x44, 0xAF, 0x33, 0x6E, 0x73, 0x58, 0x03, 0x0C, 0xA0, 0x17, 0xC9, 0xA8, 0xAE,
    0x73, 0x6E, 0x5D, 0xE2, 0xF9, 0x90, 0x73, 0xE8, 0xA1, 0x87, 0xFA, 0x69, 0xB1, 0xCE, 0xA0, 0xCD,
    0x61, 0x0D, 0x33, 0x80, 0x64, 0x54, 0x23, 0xBC, 0x9E, 0x08, 0xBD, 0x8B, 0x67, 0x17, 0x5F, 0x7E,
    0xF9, 0xE5, 0xF5, 0xC4, 0x6A, 0x8E, 0xC9, 0x8A, 0x01, 0x84, 0xA3, 0xBD, 0xD7, 0x68, 0xBD, 0x8B,
    0x37, 0x73, 0xE6, 0xCC, 0xA1, 0x43, 0x87, 0xDA, 0xD1, 0xBD, 0xC6, 0x6A, 0x0E, 0xC8, 0x84, 0x01,
    0x54, 0x23, 0x1C, 0xED, 0xBD, 0x46, 0xEB, 0x45, 0x3C, 0x5F, 0xDE, 0xC6, 0x8C, 0x19, 0x63, 0x2F,
    0xF7, 0x1A, 0xA8, 0x39, 0x20, 0x43, 0x06, 0x10, 0x8E, 0x76, 0xE4, 0xF7, 0x1C, 0xB3, 0x27, 0xF1,
    0x06, 0x0F, 0x1E, 0x6C, 0xFF, 0xF6, 0x3C, 0xBF, 0x3A, 0x67, 0x7D, 0x27, 0xDC, 0x6C, 0xB3, 0xCD,
    0x1C, 0x13, 0x48, 0x9E, 0xBC, 0x43, 0x4B, 0x3C, 0x4E, 0xC5, 0x03, 0x12, 0x67, 0x91, 0xC7, 0xBE,
    0xE6, 0x42, 0x3E, 0x09, 0x7A, 0x98, 0xD5, 0x93, 0x78, 0x17, 0x5F, 0x7C, 0xB1, 0x37, 0x7F, 0xBB,
    0xB8, 0x87, 0xF9, 0x55, 0x38, 0x45, 0x92, 0xAD, 0xB6, 0xDA, 0xAA, 0xA3, 0xA3, 0xE3, 0x83, 0x0F,
    0x3E, 0xF0, 0x9D, 0xA2, 0xAD, 0xAD, 0xED, 0xBA, 0xEB, 0xAE, 0x9B, 0x18, 0xFD, 0xE9, 0x72, 0x3A,
    0x65, 0x80, 0x61, 0x06, 0x9B, 0x52, 0x05, 0xE4, 0x3D, 0x60, 0x40, 0x3B, 0xF2, 0x49, 0xD0, 0xC3,
    0x98, 0x6E, 0xC5, 0x1B, 0x3E, 0x7C, 0xF8, 0xB9, 0xE7, 0x9E, 0xFB, 0xCB, 0x2F, 0xBF, 0xF4, 0x30,
    0xB9, 0xDC, 0x53, 0x76, 0x92, 0x66, 0x7B, 0xBD, 0xFA, 0xEA, 0xAB, 0x16, 0x29, 0xB1, 0x6E, 0xBC,
    0xF1, 0xC6, 0xF9, 0xF3, 0xE7, 0xBF, 0xFC, 0xF2, 0xCB, 0x9F, 0x7D, 0xF6, 0xD9, 0xEA, 0xE8, 0x4F,
    0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x13, 0x4D, 0xD7, 0xCA, 0xAD, 0xA2, 0x87, 0xEC,
    0xC8, 0x27, 0x01, 0x21, 0xBA, 0x1B, 0xD3, 0xAD, 0x78, 0x97, 0x5E, 0x7A, 0xE9, 0xF6, 0xDB, 0x6F,
    0x4F, 0xFF, 0xEE, 0x66, 0x96, 0xEB, 0xB7, 0x75, 0x7E, 0xFF, 0xFD, 0x77, 0x9B, 0x89, 0x18, 0x53,
    0xA7, 0x4E, 0x25, 0x0F, 0x3C, 0x9B, 0x6F, 0xBE, 0xB9, 0x5D, 0xE5, 0x48, 0x92, 0x54, 0xE3, 0x4C,
    0x4E, 0x19, 0x66, 0xB0, 0x29, 0x26, 0x9A, 0x2E, 0x88, 0x50, 0x9C, 0x15, 0x6C, 0xC8, 0x27, 0x01,
    0x21, 0xBA, 0xC3, 0x56, 0x5B, 0x3C, 0x6A, 0x9F, 0x7C, 0xF2, 0xC9, 0x26, 0x77, 0x37, 0xAD, 0x44,
    0x3F, 0x55, 0x64, 0x5F, 0xBC, 0x78, 0xB1, 0x55, 0x69, 0x33, 0xD9, 0x60, 0x3B, 0xED, 0xB4, 0x53,
    0xE2, 0xE4, 0xAF, 0xA7, 0x19, 0x6C, 0x8A, 0x89, 0xA6, 0x0B, 0x22, 0x94, 0x59, 0x9C, 0x8E, 0x55,
    0x6B, 0x24, 0x20, 0x04, 0x39, 0x6A, 0x02, 0xAB, 0x2D, 0x1E, 0xB5, 0xAD, 0x53, 0x33, 0x6B, 0xCE,
    0x29, 0xD1, 0x69, 0x97, 0x20, 0xDD, 0x63, 0xF4, 0xAD, 0xB7, 0xDE, 0x0A, 0x1E, 0x90, 0x35, 0x49,
    0x77, 0x2A, 0xD5, 0xBA, 0x62, 0x36, 0xD1, 0x74, 0xC3, 0x84, 0x12, 0x50, 0x58, 0xC1, 0xBB, 0x0E,
    0x2B, 0xD7, 0x03, 0x1E, 0x90, 0xE4, 0xA8, 0x09, 0xA3, 0x86, 0x78, 0x07, 0x1D, 0x74, 0xD0, 0x31,
    0xC7, 0x1C, 0xE3, 0x69, 0xB5, 0xE6, 0x84, 0x12, 0x9D, 0xC8, 0x5D, 0xB9, 0x72, 0xE5, 0x84, 0x09,
    0x13, 0x96, 0x2D, 0x5B, 0xE6, 0x7A, 0x82, 0xFD, 0x14, 0x98, 0x3F, 0xFF, 0xFC, 0xF3, 0xA7, 0x9F,
    0x7E, 0x72, 0xF4, 0x0B, 0xA7, 0xBD, 0x15, 0x9A, 0x2E, 0x67, 0x72, 0x2A, 0x35, 0x45, 0x10, 0xA1,
    0x04, 0x14, 0x56, 0x70, 0x29, 0x52, 0x03, 0x4A, 0xEF, 0x12, 0x82, 0x1C, 0x44, 0xE9, 0x8A, 0xA4,
    0x86, 0x78, 0x67, 0x9C, 0x71, 0x86, 0x7A, 0xBA, 0x0E, 0x2D, 0xD7, 0x63, 0x01, 0x7A, 0x56, 0xF4,
    0xDD, 0xDD, 0x62, 0x74, 0x03, 0x8B, 0xC1, 0xF0, 0xA8, 0xF0, 0xBB, 0xEF, 0xBE, 0x1B, 0x36, 0x6C,
    0xD8, 0xB5, 0xD7, 0x5E, 0x7B, 0xFB, 0xED, 0xB7, 0xDB, 0x4C, 0x37, 0xDF, 0x7C, 0xB3, 0x8F, 0x84,
    0x1A, 0x43, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x94, 0x38, 0x82, 0x80, 0x3C, 0x82, 0x4B,
    0x21, 0x51, 0x7C, 0xAA, 0x0A, 0x36, 0x39, 0x88, 0xD2, 0x15, 0x49, 0x0D, 0xF1, 0x8E, 0x3F, 0xFE,
    0x78, 0x95, 0x74, 0x1D, 0x5A, 0xA2, 0xC7, 0x86, 0x78, 0xE7, 0x9D, 0x77, 0xA6, 0x4D, 0x9B, 0x86,
    0x65, 0x7B, 0x25, 0x46, 0x42, 0x09, 0x1B, 0xEB, 0xC4, 0x13, 0x4F, 0x7C, 0xFA, 0xE9, 0xA7, 0xE7,
    0xCE, 0x9D, 0x3B, 0x76, 0xEC, 0xD8, 0x7D, 0xF7, 0xDD, 0xD7, 0x6F, 0xD3, 0x9C, 0x66, 0x69, 0x0C,
    0x5D, 0x4E, 0xA7, 0x0C, 0x30, 0xCC, 0x60, 0x4E, 0x13, 0xE3, 0x38, 0xC2, 0x0A, 0x2E, 0x85, 0x44,
    0x66, 0xC5, 0xA7, 0x4A, 0xB7, 0xC9, 0x41, 0x94, 0xAE, 0x30, 0xD2, 0xE2, 0x79, 0xAB, 0xDF, 0x6D,
    0xB7, 0xDD, 0x8C, 0xEE, 0x3A, 0xB4, 0x2C, 0x0F, 0x2A, 0xBF, 0xFD, 0xF6, 0x5B, 0xB4, 0x22, 0x17,
    0xC5, 0x31, 0x0C, 0xDB, 0xC8, 0x25, 0xC5, 0xC6, 0xBA, 0xF0, 0xC2, 0x0B, 0x13, 0x3D, 0xBC, 0x00,
    0x00, 0x9F, 0xB4, 0x64, 0x64, 0x62, 0x3B, 0x3A, 0x45, 0x30, 0xC3, 0x0C, 0x36, 0xC5, 0x44, 0xD3,
    0x93, 0x31, 0xC9, 0x51, 0x70, 0x29, 0x24, 0x92, 0x4E, 0xD2, 0xC4, 0x59, 0x85, 0x23, 0xF0, 0x44,
    0x21, 0x4D, 0x0A, 0xCC, 0x7A, 0xF1, 0x92, 0x13, 0x17, 0x5C, 0x70, 0x81, 0xD7, 0x8B, 0xC4, 0xAE,
    0xC2, 0x11, 0xA1, 0xEE, 0x55, 0xAE, 0x78, 0x68, 0x65, 0x07, 0x48, 0xEA, 0x61, 0xCF, 0x99, 0x33,
    0x67, 0xF2, 0xE4, 0xC9, 0x36, 0x16, 0x61, 0x12, 0x0F, 0x67, 0xCF, 0xCD, 0x30, 0x83, 0x4D, 0x31,
    0xD1, 0x74, 0x83, 0x79, 0x1C, 0x93, 0x26, 0x85, 0x44, 0xD2, 0x49, 0xCA, 0x4E, 0x9C, 0x55, 0x38,
    0x12, 0x85, 0x34, 0x29, 0x24, 0x1B, 0x88, 0xE7, 0x67, 0x5C, 0x0A, 0xC7, 0xC5, 0xA4, 0x46, 0x17,
    0xDF, 0x45, 0xE5, 0xA2, 0x45, 0x8B, 0xBE, 0xFF, 0xFE, 0xFB, 0x98, 0xCA, 0x04, 0xE1, 0x8C, 0x19,
    0x33, 0x46, 0x8F, 0x1E, 0x6D, 0x33, 0x25, 0xDD, 0x3E, 0x61, 0x33, 0xC5, 0x44, 0xD3, 0x05, 0x31,
    0x51, 0xD7, 0x31, 0x69, 0x12, 0x49, 0x27, 0xA9, 0xD4, 0x89, 0xA7, 0x0A, 0x47, 0x08, 0x49, 0x43,
    0xA0, 0x18, 0xCC, 0x06, 0xE2, 0xB5, 0xB6, 0xB6, 0xF6, 0xFC, 0x31, 0x2D, 0x9E, 0x59, 0x80, 0xED,
    0xDA, 0xE5, 0x0E, 0xF4, 0xC0, 0x03, 0x0F, 0xC4, 0x0F, 0x11, 0xCA, 0x70, 0xE9, 0x9B, 0x37, 0x6F,
    0x9E, 0xAF, 0x47, 0xF6, 0x50, 0x7F, 0x60, 0x98, 0x2E, 0x88, 0x50, 0x02, 0x0A, 0x1B, 0x42, 0x49,
    0x27, 0xA9, 0xD4, 0x00, 0x04, 0x67, 0xE9, 0x06, 0x69, 0x08, 0x14, 0xC3, 0xD8, 0x40, 0xBC, 0xC3,
    0x0F, 0x3F, 0xDC, 0x23, 0x75, 0x7C, 0xBA, 0x5C, 0x1B, 0xB9, 0x3E, 0x82, 0x58, 0x71, 0x31, 0x0C,
    0x08, 0x5D, 0xD6, 0x5C, 0xF7, 0x9C, 0x8D, 0xFD, 0x8D, 0xD9, 0x82, 0x08, 0x25, 0xA0, 0xB0, 0x71,
    0x04, 0x49, 0xA5, 0x76, 0x36, 0x76, 0x96, 0x6B, 0x43, 0x48, 0xA0, 0x18, 0xC3, 0x7A, 0xF1, 0x8E,
    0x3C, 0xF2, 0x48, 0xBF, 0xC1, 0xC7, 0x0B, 0x30, 0x1E, 0x57, 0xBC, 0xED, 0xF2, 0xF5, 0xF6, 0xDB,
    0x6F, 0xBF, 0xFB, 0xEE, 0xBB, 0x8C, 0x90, 0xDD, 0xAD, 0xE8, 0x8A, 0x2B, 0xAE, 0x18, 0x31, 0x62,
    0x44, 0x86, 0xB4, 0x0A, 0x25, 0xA0, 0xB0, 0x82, 0x87, 0x44, 0x92, 0x4A, 0x0D, 0x00, 0x23, 0x38,
    0xCB, 0x35, 0x48, 0x43, 0x20, 0x32, 0x05, 0x18, 0xEB, 0xC5, 0x3B, 0xEB, 0xAC, 0xB3, 0x94, 0x11,
    0x4E, 0x94, 0x6E, 0xB8, 0xE5, 0x2C, 0x5C, 0xB8, 0x70, 0xEB, 0xAD, 0xB7, 0x0E, 0x48, 0x2C, 0xBD,
    0xC3, 0x0E, 0x3B, 0x0C, 0x7A, 0xB7, 0xAB, 0xE0, 0xCC, 0xC4, 0x10, 0x50, 0x58, 0xC1, 0xA5, 0x08,
    0x01, 0xA5, 0x06, 0x00, 0x8C, 0xE0, 0x29, 0xDD, 0x20, 0x10, 0x99, 0x02, 0x8C, 0xF5, 0xE2, 0x8D,
    0x1A, 0x35, 0x2A, 0x86, 0x1E, 0x46, 0x94, 0x62, 0x58, 0xEF, 0x3E, 0x1F, 0x2F, 0x5D, 0xBA, 0x34,
    0xE6, 0xCE, 0xD2, 0xF3, 0xAE, 0x9A, 0xD3, 0x7D, 0x48, 0x58, 0xC1, 0xA5, 0x08, 0xF5, 0x4A, 0x0D,
    0x00, 0x18, 0xC0, 0x04, 0x67, 0xB9, 0x06, 0x81, 0xC8, 0x14, 0x30, 0xAC, 0x13, 0x6F, 0xDC, 0xB8,
    0x71, 0x5E, 0xE3, 0x63, 0xE8, 0x61, 0x44, 0x29, 0x06, 0xE2, 0xFC, 0x6A, 0xE3, 0x6E, 0x14, 0xB2,
    0xC3, 0xED, 0xFB, 0xAC, 0xEB, 0x9B, 0xD5, 0x17, 0x9C, 0x19, 0x1A, 0xC2, 0x0A, 0x2E, 0x85, 0x44,
    0x21, 0x2C, 0x00, 0x60, 0x00, 0x13, 0x3C, 0xE5, 0x1A, 0x04, 0x22, 0x13, 0xB1, 0x12, 0x18, 0xEB,
    0xC4, 0x3B, 0xE0, 0x80, 0x03, 0x3C, 0xCC, 0x24, 0xAE, 0x2A, 0x1C, 0x57, 0xAD, 0x5A, 0xF5, 0xD1,
    0x47, 0x1F, 0xC5, 0xAC, 0xF1, 0xD8, 0x19, 0x7F, 0xFF, 0xFD, 0x77, 0x7E, 0xF0, 0x04, 0x97, 0x42,
    0xA2, 0x90, 0x02, 0x00, 0x30, 0x62, 0x4F, 0x38, 0x55, 0x96, 0x41, 0x26, 0x62, 0x25, 0xD9, 0xD7,
    0x89, 0xE7, 0x8D, 0x07, 0xF4, 0xC4, 0x95, 0xFD, 0xB1, 0x8F, 0x11, 0x5D, 0xA6, 0x3C, 0x3B, 0xFC,
    0xF8, 0xE3, 0x8F, 0x61, 0x9E, 0x15, 0xB7, 0xF7, 0xDE, 0x7B, 0xBB, 0x5D, 0xDB, 0x1F, 0xC1, 0x99,
    0xB9, 0x21, 0xB8, 0x14, 0x7B, 0xEF, 0xBD, 0xB7, 0x74, 0x21, 0x38, 0x18, 0xC0, 0x80, 0x14, 0x3C,
    0xE5, 0x1A, 0x64, 0x22, 0x56, 0x82, 0x61, 0x9D, 0x78, 0x23, 0x47, 0x8E, 0x04, 0x3D, 0x71, 0x55,
    0xE1, 0xE8, 0x35, 0xF9, 0xCB, 0x2F, 0xBF, 0x0C, 0x94, 0x41, 0x0C, 0xE1, 0x36, 0xDB, 0x6C, 0x93,
    0x37, 0x36, 0x29, 0x24, 0x92, 0x2E, 0x49, 0x04, 0x00, 0x18, 0xC0, 0x24, 0xDD, 0x2A, 0x1C, 0xC9,
    0x04, 0x61, 0x82, 0xA4, 0x53, 0x3C, 0x17, 0x7A, 0x9B, 0x31, 0xE9, 0x57, 0xE1, 0x88, 0x32, 0x8B,
    0x3D, 0x46, 0x62, 0x2B, 0x78, 0xF1, 0xF2, 0x4C, 0x11, 0x3B, 0xF3, 0xB0, 0xA5, 0x90, 0x48, 0xBA,
    0x38, 0x38, 0x30, 0x20, 0xC5, 0x9E, 0x72, 0x6D, 0x62, 0x91, 0x0C, 0x86, 0x4E, 0xF1, 0x4E, 0x39,
    0xE5, 0x14, 0x7A, 0xEA, 0x54, 0xA7, 0xF9, 0x64, 0xBC, 0xC5, 0x16, 0x5B, 0xC4, 0x78, 0xFC, 0x38,
    0x17, 0x77, 0xF3, 0xB3, 0x53, 0x89, 0xC0, 0x00, 0x26, 0xBF, 0x74, 0x0D, 0x44, 0x26, 0x16, 0xC9,
    0x4C, 0xEC, 0x14, 0xEF, 0xC0, 0x03, 0x0F, 0x0C, 0x17, 0x0A, 0xAE, 0xD2, 0xDB, 0xA6, 0x9B, 0x6E,
    0xDA, 0xD1, 0xD1, 0x91, 0x82, 0xE1, 0xAD, 0x2B, 0xE5, 0xC9, 0xA9, 0xDB, 0x35, 0x11, 0x30, 0x20,
    0xE5, 0x94, 0xAE, 0x81, 0xB0, 0xC4, 0x22, 0x99, 0x89, 0x9D, 0xE2, 0x59, 0x6B, 0xA9, 0x0B, 0x85,
    0x13, 0x55, 0x6B, 0xEE, 0x46, 0x05, 0x80, 0x94, 0x42, 0xA2, 0xAA, 0xD5, 0x9E, 0xC2, 0x03, 0x24,
    0xC9, 0x38, 0x5B, 0xDC, 0xFD, 0x5C, 0x43, 0x59, 0xCD, 0x36, 0x80, 0x18, 0x20, 0x19, 0xE1, 0x5A,
    0x86, 0x0D, 0x1B, 0x66, 0xAD, 0x11, 0xB3, 0x52, 0xD0, 0x7D, 0xD7, 0x4F, 0xE1, 0xF9, 0xF5, 0xD7,
    0x5F, 0x0B, 0x78, 0x6A, 0x90, 0x42, 0xA2, 0x54, 0xEA, 0xAE, 0x60, 0x52, 0x03, 0x0A, 0xEE, 0x12,
    0x8B, 0x64, 0x84, 0x6B, 0xD9, 0x6E, 0xBB, 0xED, 0xB6, 0xDC, 0x72, 0xCB, 0x82, 0xD3, 0xF7, 0x9C,
    0xCE, 0x35, 0x7D, 0xE7, 0x9D, 0x77, 0xFE, 0xE3, 0x8F, 0x3F, 0xE2, 0x61, 0x5F, 0x7D, 0xF5, 0x55,
    0xDC, 0xCD, 0xCF, 0x4E, 0x25, 0x02, 0x03, 0x18, 0x90, 0xF2, 0xCB, 0xD8, 0x40, 0x64, 0x92, 0x11,
    0xAE, 0xC5, 0x17, 0x20, 0xDF, 0x11, 0x1A, 0x98, 0x9F, 0xDF, 0x14, 0x2B, 0xCB, 0xF3, 0x7A, 0x2A,
    0xFE, 0xB7, 0xDF, 0x7E, 0xCB, 0x9F, 0x72, 0x6E, 0xB2, 0x49, 0xC6, 0x0E, 0x29, 0x24, 0x4A, 0x05,
    0x05, 0x86, 0x3F, 0xE5, 0x2C, 0xB7, 0x4B, 0x32, 0xC2, 0x75, 0x8A, 0x57, 0x2E, 0x8E, 0x9A, 0xD9,
    0x87, 0x0E, 0x1D, 0xEA, 0x07, 0xD2, 0x70, 0x0A, 0xD6, 0x4F, 0x3F, 0xFD, 0xF4, 0xF7, 0xDF, 0x7F,
    0x0F, 0x9E, 0x9C, 0x0C, 0x29, 0x24, 0x92, 0x2E, 0xC4, 0x07, 0x03, 0x98, 0xD0, 0xAD, 0x8E, 0xD1,
    0x29, 0x9E, 0xE7, 0x96, 0xAA, 0x5D, 0x13, 0x2C, 0x73, 0xD7, 0x74, 0xC0, 0x18, 0x09, 0x59, 0x6E,
    0x45, 0x1F, 0x7E, 0xF8, 0x61, 0xDE, 0x2F, 0xCB, 0xB2, 0x48, 0x21, 0x11, 0x23, 0xC9, 0x0B, 0x00,
    0x18, 0xC0, 0x30, 0x12, 0x4F, 0x45, 0x8E, 0x24, 0x03, 0xAC, 0xC5, 0x57, 0xEA, 0x8A, 0x00, 0x8A,
    0x61, 0x40, 0xE5, 0x86, 0x0C, 0x62, 0x70, 0x7A, 0x59, 0x6E, 0x6F, 0x6F, 0x8F, 0xF7, 0x44, 0x38,
    0x95, 0x95, 0x21, 0xB8, 0x14, 0x12, 0x85, 0x80, 0x00, 0x80, 0x01, 0x4C, 0xF0, 0x54, 0xC7, 0x80,
    0xAA, 0xA5, 0x82, 0xCB, 0x2A, 0x21, 0xC8, 0x4F, 0xFE, 0xF1, 0xE7, 0x7C, 0x8F, 0x7C, 0x2F, 0xBC,
    0xF0, 0x82, 0xCF, 0x8C, 0x61, 0x5B, 0x24, 0xC3, 0xB2, 0x3A, 0x0A, 0x2B, 0xB8, 0x14, 0x12, 0x85,
    0x98, 0x00, 0x80, 0x11, 0xBA, 0xD5, 0x31, 0x5C, 0x09, 0x08, 0xD7, 0xE2, 0x8D, 0xA1, 0x3A, 0x98,
    0x02, 0x12, 0x5F, 0x80, 0x0E, 0x3E, 0xF8, 0xE0, 0x6D, 0xB7, 0xDD, 0x36, 0x78, 0x18, 0x7E, 0x69,
    0x5B, 0xB4, 0x68, 0x11, 0x96, 0xD9, 0x99, 0x37, 0x61, 0x05, 0x97, 0x22, 0x8E, 0x0C, 0x00, 0x18,
    0xC0, 0xC4, 0xCE, 0x8A, 0xD8, 0x84, 0x6B, 0x01, 0x9A, 0x8C, 0x15, 0x01, 0x14, 0xC3, 0x70, 0xC9,
    0xF2, 0xEB, 0x9A, 0x9B, 0x50, 0x70, 0xDA, 0x13, 0xCF, 0x3C, 0xF3, 0x8C, 0xCF, 0xFC, 0x3E, 0x1F,
    0x07, 0x67, 0x26, 0x86, 0x80, 0xC2, 0x0A, 0x2E, 0x45, 0x08, 0x28, 0x35, 0x00, 0x60, 0x04, 0x4F,
    0x75, 0x0C, 0x92, 0x11, 0xAE, 0xC5, 0x1B, 0x43, 0x75, 0x30, 0xC5, 0x48, 0xE0, 0x3B, 0xEE, 0xB8,
    0xE3, 0x76, 0xDF, 0x7D, 0x77, 0x46, 0xF0, 0xBB, 0x2D, 0x4D, 0x9B, 0x36, 0xCD, 0x7B, 0x34, 0xE8,
    0xC1, 0xD9, 0x4F, 0x43, 0x28, 0x01, 0x85, 0x15, 0x3C, 0x84, 0x92, 0x54, 0x6A, 0x00, 0x18, 0xC1,
    0x59, 0x29, 0x83, 0x70, 0x2D, 0x16, 0x5D, 0xA5, 0x30, 0x05, 0x30, 0x58, 0xF3, 0x34, 0xDC, 0xDA,
    0xDA, 0xBA, 0x7A, 0xF5, 0xEA, 0xE0, 0x44, 0xB4, 0x3B, 0xD3, 0xBC, 0xB5, 0xFF, 0x30, 0x72, 0x70,
    0xF6, 0xD3, 0x10, 0x53, 0x40, 0x61, 0x19, 0x21, 0x94, 0xA4, 0x52, 0x03, 0x00, 0x46, 0x70, 0x56,
    0xCA, 0x20, 0x5C, 0xE7, 0x87, 0xE9, 0x4A, 0x61, 0x8A, 0xC1, 0xB8, 0xD9, 0x9C, 0x76, 0xDA, 0x69,
    0x9E, 0xF7, 0xE2, 0x5B, 0x91, 0x2B, 0xDB, 0xB3, 0xCF, 0x3E, 0x7B, 0xC3, 0x0D, 0x37, 0x18, 0x19,
    0xD3, 0xAD, 0xDB, 0xD7, 0x96, 0x4C, 0x17, 0x4A, 0x40, 0x61, 0xC3, 0x74, 0xE9, 0x86, 0x0D, 0x1B,
    0x26, 0x35, 0x00, 0xC1, 0x59, 0x41, 0xA3, 0xD2, 0xE2, 0xE1, 0xCB, 0xC2, 0xBF, 0xE2, 0x8A, 0x2B,
    0x18, 0x71, 0xF3, 0x94, 0xFC, 0xFE, 0xFB, 0xEF, 0xCF, 0x9C, 0x39, 0xD3, 0x59, 0x0B, 0x30, 0x3E,
    0x55, 0xBF, 0x6D, 0xA2, 0xE9, 0x82, 0x08, 0x25, 0x60, 0x6A, 0xA2, 0xA4, 0xCE, 0xA6, 0x9C, 0x55,
    0xEB, 0x0E, 0x00, 0xF1, 0x46, 0x8C, 0x18, 0x31, 0x69, 0xD2, 0xA4, 0xD4, 0x2F, 0xA2, 0x36, 0x0A,
    0xD2, 0x4F, 0x3D, 0xF5, 0xD4, 0x0F, 0x3E, 0xF8, 0xC0, 0x06, 0xA2, 0x44, 0xFD, 0xCC, 0x1A, 0x6C,
    0x8A, 0x89, 0xA6, 0x0B, 0x22, 0x54, 0x3C, 0x57, 0x22, 0xE9, 0x24, 0x1D, 0x00, 0xE2, 0x55, 0xFC,
    0xCA, 0x80, 0xD6, 0xDF, 0x7E, 0xFB, 0xED, 0x88, 0x23, 0x8E, 0x38, 0xF3, 0xCC, 0x33, 0x3D, 0xFE,
    0xE9, 0x86, 0x86, 0x74, 0xBF, 0x91, 0x7A, 0xD0, 0x98, 0x3B, 0x77, 0xAE, 0x67, 0x45, 0xDD, 0x44,
    0x95, 0x30, 0x20, 0x65, 0x10, 0xCC, 0x00, 0xC3, 0x0C, 0x36, 0xC5, 0x44, 0xD3, 0x75, 0xE3, 0x61,
    0x52, 0x48, 0x24, 0x9D, 0xA4, 0xB1, 0xBF, 0x82, 0x36, 0xE1, 0x5A, 0x7C, 0xCD, 0xAB, 0x20, 0xB2,
    0x14, 0x24, 0x40, 0x2F, 0xBC, 0xF0, 0xC2, 0xF3, 0xCE, 0x3B, 0xCF, 0xB6, 0x88, 0x4F, 0xD1, 0x03,
    0xFB, 0x2F, 0xBF, 0xFC, 0xB2, 0x3D, 0x74, 0xF9, 0xE5, 0x97, 0x2F, 0x5E, 0xBC, 0xF8, 0xEB, 0xAF,
    0xBF, 0x36, 0x38, 0x11, 0xC9, 0xA9, 0xA4, 0xE9, 0x72, 0x3A, 0x65, 0x80, 0x61, 0x06, 0x9B, 0xE2,
    0x94, 0xE9, 0x71, 0x34, 0xC1, 0xA5, 0x90, 0xC8, 0xE0, 0xD8, 0x5F, 0x4D, 0x9B, 0x70, 0x2D, 0x2E,
    0x0E, 0xA9, 0x1A, 0xAA, 0x89, 0x15, 0xA1, 0xE3, 0xC7, 0x8F, 0x9F, 0x30, 0x61, 0x82, 0xCD, 0x01,
    0x73, 0x0C, 0xD2, 0x23, 0xFE, 0x1E, 0x7B, 0xEC, 0xF1, 0xC9, 0x27, 0x9F, 0xDC, 0x7A, 0xEB, 0xAD,
    0xF6, 0x93, 0x07, 0x10, 0xAD, 0xAD, 0xAD, 0xED, 0xA1, 0xB5, 0x7F, 0xB3, 0x66, 0xCD, 0xD2, 0xD5,
    0x9C, 0x32, 0xC0, 0x30, 0x83, 0x4D, 0x89, 0x23, 0x08, 0x28, 0xAC, 0xE0, 0x52, 0x48, 0x14, 0x9F,
    0xAA, 0xA6, 0x4D, 0x32, 0x98, 0x5B, 0x7C, 0x01, 0xAA, 0x26, 0xBE, 0xAE, 0xA8, 0xC0, 0xB5, 0x33,
    0x88, 0x01, 0xBA, 0x07, 0xC2, 0xD4, 0x00, 0x7A, 0x78, 0xEE, 0xF0, 0xC6, 0xF6, 0xD9, 0x67, 0x9F,
    0x2D, 0x5F, 0xBE, 0xDC, 0x1B, 0xF7, 0xC2, 0xB5, 0x7F, 0x2F, 0xBE, 0xF8, 0xA2, 0x2E, 0xA7, 0x53,
    0x06, 0x18, 0x96, 0x9A, 0x28, 0x94, 0x80, 0xC2, 0x0A, 0x2E, 0x45, 0xEA, 0x6C, 0x65, 0xBB, 0x84,
    0x6B, 0x51, 0x12, 0xE8, 0x95, 0x85, 0x98, 0x02, 0xE6, 0x56, 0xE4, 0x7B, 0x95, 0xD7, 0xB2, 0x1E,
    0x1E, 0x28, 0x94, 0xA3, 0xB9, 0x2A, 0x86, 0xA6, 0xAB, 0xA5, 0x42, 0x25, 0x5D, 0x6A, 0x09, 0x25,
    0xA0, 0xB0, 0x82, 0x27, 0xCE, 0xEA, 0x1F, 0x95, 0x43, 0xB8, 0x16, 0x97, 0x8B, 0xEA, 0x63, 0x4D,
    0x10, 0x42, 0x4C, 0x0F, 0x7B, 0xE8, 0xB9, 0xE7, 0x9E, 0xFB, 0xE6, 0x9B, 0x6F, 0x74, 0x13, 0x7F,
    0x7F, 0x8E, 0x82, 0x08, 0x25, 0xA0, 0xB0, 0x82, 0xEB, 0xF6, 0x27, 0x5A, 0x91, 0x73, 0x09, 0xD7,
    0xE2, 0x2E, 0xED, 0xA1, 0xAB, 0xC8, 0xAC, 0x8D, 0xE5, 0xF2, 0xD0, 0xE1, 0x16, 0xED, 0x4E, 0x76,
    0xD5, 0x55, 0x57, 0xB9, 0x1C, 0xFA, 0x02, 0xD2, 0x58, 0x9C, 0xAE, 0xB3, 0x84, 0x12, 0x50, 0x58,
    0xC1, 0xA5, 0x90, 0xA8, 0xEB, 0x98, 0x8C, 0x3D, 0xFD, 0x0E, 0x47, 0x32, 0xC2, 0xB5, 0x74, 0x74,
    0xF9, 0x4F, 0x48, 0xF6, 0x3B, 0x72, 0xC6, 0x01, 0xEC, 0x06, 0x84, 0x7A, 0x50, 0x3C, 0xFB, 0xEC,
    0xB3, 0xDD, 0xC9, 0x44, 0xEF, 0x6E, 0x8B, 0xB8, 0x06, 0x6A, 0xAE, 0x7E, 0x56, 0xA5, 0xDA, 0x42,
    0xD3, 0xE5, 0x74, 0x4A, 0x33, 0x3D, 0xD5, 0xC4, 0x17, 0x90, 0x53, 0x70, 0x29, 0x24, 0x92, 0x8E,
    0x93, 0xA7, 0xCA, 0x8D, 0x70, 0x9D, 0xE2, 0xB9, 0x63, 0x57, 0x16, 0x25, 0x1E, 0x51, 0xEF, 0x59,
    0xF1, 0xCE, 0x3B, 0xEF, 0x04, 0x32, 0x61, 0x99, 0x11, 0x37, 0xF8, 0x69, 0xC3, 0x33, 0x68, 0xD0,
    0xA0, 0xFD, 0xF6, 0xDB, 0xEF, 0xC4, 0x13, 0x4F, 0xBC, 0xEC, 0xB2, 0xCB, 0x66, 0xCC, 0x98, 0x31,
    0x67, 0xED, 0x1F, 0x43, 0x97, 0xD3, 0x29, 0x03, 0x0C, 0x33, 0xD8, 0x14, 0x46, 0xAA, 0x25, 0xC1,
    0x25, 0x92, 0x4E, 0x52, 0xA9, 0x53, 0x03, 0xAA, 0xD3, 0x85, 0xBF, 0x53, 0xBC, 0x9F, 0x7F, 0xFE,
    0xD9, 0xB5, 0xA2, 0x3A, 0xB0, 0x62, 0x24, 0xD8, 0x5C, 0xB9, 0xF6, 0xDF, 0xAB, 0x4A, 0xBE, 0x83,
    0xA4, 0x76, 0x83, 0x6D, 0x84, 0x62, 0xED, 0xA8, 0xA3, 0x8E, 0xF2, 0x35, 0xEB, 0x8E, 0x3B, 0xEE,
    0xF0, 0xDC, 0x71, 0xFB, 0xED, 0xB7, 0x4F, 0x9C, 0x38, 0x71, 0xDC, 0xB8, 0x71, 0x5E, 0xB4, 0x47,
    0xAF, 0xFD, 0x63, 0xE8, 0x72, 0x3A, 0x65, 0x80, 0x61, 0x06, 0x9B, 0x62, 0xA2, 0x26, 0x48, 0x9C,
    0x51, 0x0A, 0x49, 0xA5, 0xF3, 0xDA, 0x20, 0x35, 0x3B, 0x3E, 0x5B, 0x1D, 0x9B, 0x64, 0x84, 0x6B,
    0xF1, 0xDE, 0xE3, 0xB9, 0x05, 0xE8, 0xEA, 0x20, 0x4B, 0x90, 0x58, 0xF8, 0xED, 0xED, 0xED, 0xD7,
    0x5C, 0x73, 0x8D, 0x6E, 0x8A, 0x44, 0x8C, 0xDB, 0x3D, 0xB6, 0x91, 0x17, 0xEA, 0xC7, 0x1F, 0x7F,
    0x9C, 0x30, 0x63, 0xC7, 0x8E, 0xDD, 0x75, 0xD7, 0x5D, 0x8D, 0xE4, 0x4F, 0x9A, 0xD7, 0xB5, 0xD0,
    0x12, 0x8F, 0xA3, 0x01, 0x86, 0x19, 0x6C, 0x8A, 0x89, 0xA6, 0x0B, 0xC2, 0x2F, 0xA0, 0x53, 0xA1,
    0x25, 0xE9, 0xA4, 0x06, 0x00, 0x8C, 0xE0, 0xAF, 0x88, 0x41, 0x2C, 0x92, 0x11, 0xAE, 0xC5, 0x3B,
    0x90, 0x37, 0x86, 0x8A, 0xC0, 0x0A, 0x30, 0xD0, 0xE7, 0xDE, 0x33, 0x65, 0xCA, 0x14, 0xAF, 0x65,
    0xB0, 0x06, 0x3F, 0x03, 0xD7, 0x8E, 0x76, 0x8F, 0xE7, 0x0B, 0xEF, 0xD4, 0xD8, 0xD7, 0xA5, 0x53,
    0x4A, 0x00, 0xCE, 0x9A, 0xCD, 0x30, 0x83, 0x9D, 0x32, 0xD1, 0x74, 0x41, 0x84, 0xD2, 0x4D, 0xC2,
    0x32, 0x92, 0x26, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x4C, 0xE2, 0xAC, 0xCE, 0x91, 0x64, 0x84, 0xEB,
    0xFC, 0x30, 0xED, 0xC6, 0x0E, 0x6B, 0x75, 0x90, 0x21, 0x6B, 0xC1, 0x82, 0x05, 0xEE, 0x3D, 0x3E,
    0x85, 0xC4, 0xA8, 0xF0, 0x8E, 0xE2, 0x63, 0x8E, 0x39, 0xE6, 0x81, 0x07, 0x1E, 0xB0, 0x7B, 0xFC,
    0x1A, 0xA9, 0xCB, 0x19, 0x8F, 0xA9, 0xDF, 0x36, 0xD1, 0x74, 0x41, 0x84, 0x12, 0x50, 0x58, 0x5D,
    0xCE, 0x38, 0x02, 0x00, 0x60, 0x00, 0x03, 0x52, 0xEC, 0x2F, 0xD7, 0x26, 0x16, 0xC9, 0x60, 0xE8,
    0x14, 0x6F, 0xE9, 0xD2, 0xA5, 0x1E, 0x3D, 0x75, 0xAA, 0xD0, 0xD0, 0xB4, 0x64, 0xC9, 0x12, 0xF7,
    0x27, 0x46, 0x8C, 0xC7, 0x2D, 0x5A, 0x9B, 0x3E, 0x7D, 0xBA, 0x67, 0x7A, 0x9B, 0xC6, 0xEE, 0x49,
    0x11, 0x1D, 0x0F, 0xAE, 0xDF, 0x16, 0x44, 0x28, 0x01, 0x85, 0x15, 0x5C, 0x0A, 0x2D, 0x9E, 0x0E,
    0x06, 0x30, 0x20, 0x31, 0x62, 0x7F, 0x89, 0x36, 0xB1, 0x05, 0x5A, 0x7F, 0xB9, 0x00, 0x00, 0x08,
    0xDD, 0x49, 0x44, 0x41, 0x54, 0x48, 0x06, 0x40, 0xA7, 0x78, 0x4F, 0x3E, 0xF9, 0x64, 0x45, 0xAE,
    0xEC, 0x60, 0xBC, 0xF3, 0xCE, 0x3B, 0xB7, 0xDD, 0x76, 0x9B, 0x25, 0x0F, 0x5C, 0x68, 0xF6, 0x84,
    0x7B, 0xD5, 0x83, 0x0F, 0x3E, 0xE8, 0x11, 0x04, 0xD7, 0x18, 0x0F, 0xA7, 0x32, 0x31, 0x04, 0x14,
    0x56, 0x70, 0x29, 0x24, 0x92, 0x2E, 0x0E, 0x0B, 0x0C, 0x48, 0x80, 0x81, 0x17, 0xFB, 0xCB, 0xB2,
    0xC1, 0x20, 0x99, 0xEC, 0x9D, 0xE2, 0xAD, 0x58, 0xB1, 0xC2, 0x35, 0x54, 0xA7, 0xDC, 0xE6, 0x6A,
    0xE0, 0x3E, 0xEC, 0xF3, 0xB1, 0x9B, 0x4D, 0x8C, 0xC4, 0x33, 0xE1, 0xF0, 0xE1, 0xC3, 0x67, 0xCF,
    0x9E, 0x3D, 0x64, 0xC8, 0x10, 0x14, 0xC7, 0xA7, 0xB2, 0xB5, 0x05, 0x97, 0x42, 0x22, 0xE9, 0x24,
    0x8D, 0x83, 0x83, 0x74, 0xE3, 0x8D, 0x37, 0x82, 0x07, 0x64, 0xEC, 0x2F, 0xC5, 0x26, 0x16, 0xC9,
    0xA4, 0xEE, 0x14, 0xCF, 0xFF, 0x5B, 0xBE, 0x7C, 0x39, 0x3D, 0x19, 0x9D, 0xAD, 0xA4, 0xFF, 0xB3,
    0xFC, 0x5D, 0xA0, 0xB0, 0xA3, 0x05, 0x08, 0x36, 0xC1, 0x61, 0x87, 0x1D, 0x86, 0x38, 0x37, 0x27,
    0x03, 0x82, 0x3F, 0x27, 0x43, 0x0A, 0x89, 0xA4, 0x93, 0x54, 0xEA, 0x90, 0x25, 0x81, 0x04, 0x9E,
    0x01, 0xC1, 0x59, 0x8A, 0x41, 0x26, 0x62, 0x25, 0xA9, 0xD7, 0x89, 0xE7, 0x67, 0x65, 0x57, 0xD2,
    0xC4, 0x55, 0xCA, 0xD1, 0x1D, 0xE5, 0x95, 0x57, 0x5E, 0x79, 0xFD, 0xF5, 0xD7, 0xAD, 0xF1, 0x00,
    0xC0, 0xED, 0x67, 0xE8, 0xD0, 0xA1, 0x1E, 0xD9, 0x11, 0x5A, 0x18, 0x6B, 0x12, 0x49, 0x27, 0xA9,
    0xD4, 0x00, 0x04, 0x30, 0x80, 0x81, 0x07, 0x24, 0xA8, 0xC1, 0x59, 0xBC, 0x41, 0x26, 0x62, 0x25,
    0x79, 0xD7, 0x89, 0x57, 0xEE, 0xBF, 0x36, 0x62, 0x5D, 0xBB, 0x14, 0xDC, 0x74, 0xD3, 0x4D, 0x7E,
    0xB2, 0x49, 0x60, 0x39, 0x22, 0x71, 0x87, 0x1D, 0x76, 0xF0, 0x0B, 0x1C, 0x2A, 0xD9, 0x3C, 0x85,
    0x35, 0xE9, 0x24, 0x95, 0x1A, 0x00, 0x76, 0xC8, 0x0B, 0x1E, 0x90, 0xA0, 0x02, 0x1C, 0x9C, 0x05,
    0x1B, 0xB2, 0x13, 0x2B, 0x49, 0xBA, 0x4E, 0x3C, 0x6F, 0xAC, 0xAE, 0xF2, 0x65, 0x61, 0x92, 0xF7,
    0xD1, 0x47, 0x1F, 0xDD, 0x76, 0xC3, 0xFF, 0x7C, 0xF4, 0x0F, 0x3F, 0xFC, 0xE0, 0x55, 0x7A, 0xE7,
    0x9D, 0x77, 0x8E, 0xE9, 0x4B, 0x40, 0x17, 0x70, 0x94, 0x54, 0x6A, 0x00, 0xC0, 0x88, 0xD3, 0x01,
    0x09, 0x2A, 0xC0, 0xB1, 0xB3, 0x30, 0x5B, 0x5E, 0x32, 0x11, 0x2B, 0xC9, 0xB8, 0x4E, 0x3C, 0x9D,
    0x65, 0xCB, 0x96, 0xB9, 0x32, 0x30, 0x0A, 0x6E, 0x00, 0x7D, 0xF9, 0xE5, 0x97, 0x7E, 0x94, 0x89,
    0x2F, 0x47, 0xEE, 0x37, 0xA7, 0x9C, 0x72, 0xCA, 0xE1, 0x87, 0x1F, 0xEE, 0x21, 0xA2, 0x60, 0x3C,
    0x21, 0x9D, 0xD4, 0x00, 0x80, 0x01, 0x4C, 0x70, 0x02, 0x09, 0x2A, 0xC0, 0x60, 0x07, 0x67, 0x61,
    0x06, 0x81, 0xC8, 0x14, 0xD2, 0xAD, 0x17, 0xCF, 0x82, 0x72, 0x33, 0x0C, 0x27, 0x0A, 0x33, 0x00,
    0xF2, 0x15, 0xCA, 0x4A, 0x0F, 0x19, 0xD9, 0x38, 0xF2, 0xF9, 0xE3, 0x97, 0xB2, 0xFF, 0xC7, 0x70,
    0x00, 0x00, 0x03, 0x18, 0x90, 0x62, 0x78, 0x00, 0x83, 0x1D, 0x3C, 0x85, 0x19, 0x04, 0x22, 0x53,
    0x48, 0xB7, 0x5E, 0x3C, 0xB7, 0x62, 0x3F, 0x48, 0x16, 0xBC, 0xA0, 0xA4, 0x73, 0x11, 0x5F, 0xB4,
    0x68, 0x11, 0x82, 0x02, 0x26, 0x3F, 0xB0, 0xF9, 0x2E, 0x5C, 0x85, 0xFF, 0x3E, 0x2A, 0xCD, 0xC0,
    0x00, 0x06, 0xA4, 0x00, 0x0F, 0x54, 0x80, 0xC1, 0x06, 0x3E, 0x38, 0x0B, 0x30, 0xA4, 0x23, 0x10,
    0x99, 0x42, 0xAE, 0xF5, 0xE2, 0x71, 0xBD, 0xF6, 0xDA, 0x6B, 0x05, 0x2F, 0x28, 0x80, 0xDE, 0x7A,
    0xEB, 0xAD, 0xF8, 0xBF, 0x12, 0x07, 0xC6, 0x5F, 0x7F, 0xFD, 0xE5, 0x93, 0x55, 0x7C, 0xB1, 0xE2,
    0x2C, 0xAB, 0x81, 0x01, 0x0C, 0x48, 0x31, 0x00, 0x80, 0xC1, 0x06, 0x3E, 0x76, 0xE6, 0x6D, 0x93,
    0x86, 0x40, 0x71, 0x96, 0x0D, 0xC4, 0x73, 0x35, 0xB0, 0xA0, 0xE2, 0xD3, 0x79, 0xDB, 0xEE, 0x2B,
    0x6F, 0xBE, 0xF9, 0x66, 0xCC, 0x82, 0x1B, 0xF2, 0xD5, 0x57, 0x5F, 0xED, 0x81, 0x38, 0xEF, 0xD4,
    0xF5, 0xC7, 0x07, 0x06, 0x24, 0xC0, 0xC2, 0x14, 0x80, 0xC1, 0x06, 0x3E, 0x78, 0x0A, 0x30, 0x48,
    0x43, 0xA0, 0x38, 0xD1, 0x06, 0xE2, 0x3D, 0xFB, 0xEC, 0xB3, 0x5F, 0x7D, 0xF5, 0x15, 0x64, 0xF1,
    0x88, 0x4C, 0xED, 0x0D, 0x82, 0x49, 0x84, 0x11, 0x3F, 0x9E, 0x59, 0x53, 0xE1, 0x84, 0xC7, 0xF1,
    0x51, 0x55, 0xFA, 0x67, 0x5B, 0x01, 0xF3, 0xB6, 0x07, 0x12, 0x60, 0xEC, 0xA4, 0x01, 0x0C, 0x36,
    0xF0, 0x4A, 0x48, 0x3C, 0x79, 0x1F, 0x25, 0x22, 0x0D, 0x81, 0xE2, 0x44, 0x1B, 0x88, 0xE7, 0xC4,
    0x7D, 0xF7, 0xDD, 0xE7, 0x69, 0x98, 0x51, 0x40, 0x03, 0xC8, 0xC7, 0x02, 0x0F, 0x05, 0x21, 0x97,
    0x7B, 0xCC, 0xFE, 0xFB, 0xEF, 0xEF, 0xEB, 0x22, 0x23, 0x38, 0x4B, 0x37, 0x80, 0x01, 0x09, 0x30,
    0x46, 0x00, 0x03, 0x36, 0xF0, 0x4A, 0x08, 0x9E, 0x5C, 0x0D, 0xA2, 0x90, 0x26, 0x95, 0x22, 0x2D,
    0xDE, 0xC3, 0x0F, 0x3F, 0x4C, 0xE1, 0x62, 0x30, 0xB9, 0x1C, 0x7D, 0xFC, 0xF1, 0xC7, 0x60, 0x05,
    0x4C, 0x9E, 0x0B, 0x0E, 0x39, 0xE4, 0x90, 0x62, 0xB2, 0x87, 0xA4, 0xF5, 0x18, 0x20, 0x01, 0x06,
    0x5E, 0x18, 0x0C, 0x36, 0xF0, 0x4A, 0x08, 0x9E, 0xFC, 0x0C, 0xD9, 0x89, 0x42, 0x9A, 0x54, 0x8A,
    0xB4, 0x78, 0x4E, 0x7B, 0x8F, 0x31, 0x9A, 0x51, 0x40, 0x73, 0xDB, 0xF7, 0xF0, 0x16, 0x12, 0x59,
    0xCE, 0x3E, 0xED, 0xC7, 0x0B, 0x3C, 0x9C, 0x2A, 0xD7, 0x00, 0x09, 0x30, 0xF0, 0x02, 0x0C, 0xB0,
    0x81, 0x0F, 0xDD, 0x5C, 0x0D, 0x72, 0x10, 0xA5, 0x6B, 0x8A, 0x1A, 0xE2, 0x2D, 0x5C, 0xB8, 0xD0,
    0xD5, 0xBC, 0xEB, 0xD0, 0x6C, 0x3D, 0x00, 0x7D, 0xFF, 0xFD, 0xF7, 0x6E, 0x27, 0x71, 0x58, 0xCB,
    0xD9, 0x17, 0x45, 0x4C, 0xC5, 0xCE, 0x2A, 0xD8, 0x20, 0x01, 0x06, 0x5E, 0x0C, 0x06, 0x78, 0x25,
    0x28, 0x24, 0x76, 0xE6, 0x61, 0x93, 0x83, 0x28, 0x5D, 0x23, 0xD7, 0x10, 0xEF, 0xBD, 0xF7, 0xDE,
    0x7B, 0xF1, 0xC5, 0x17, 0xAD, 0xAC, 0xAE, 0xA3, 0xB3, 0xF5, 0xA8, 0x3C, 0x0E, 0x88, 0x0B, 0xDF,
    0xF2, 0x63, 0x4F, 0xD5, 0x6C, 0xF0, 0x80, 0x8C, 0x51, 0xA5, 0x4A, 0x88, 0x4F, 0x65, 0x65, 0x13,
    0x82, 0x1C, 0x44, 0xE9, 0x1A, 0xB0, 0x86, 0x78, 0x06, 0xDD, 0x75, 0xD7, 0x5D, 0xDE, 0x6F, 0x72,
    0x5D, 0x53, 0x82, 0xFB, 0x2D, 0xDF, 0x8A, 0x96, 0x2E, 0x69, 0xEC, 0x7D, 0xF6, 0xD9, 0xA7, 0x9A,
    0xFF, 0x05, 0x7E, 0x08, 0x01, 0x03, 0x0F, 0x48, 0x76, 0xD2, 0xD8, 0x4A, 0x50, 0x48, 0xD2, 0xCD,
    0xE3, 0x28, 0x38, 0x21, 0xC8, 0x51, 0x33, 0x78, 0x6D, 0xF1, 0xFC, 0xD6, 0xF7, 0xD4, 0x53, 0x4F,
    0x99, 0x59, 0x73, 0x4E, 0x56, 0xCE, 0x9F, 0x7F, 0xFE, 0x39, 0x0E, 0xF5, 0xC7, 0x1F, 0x7F, 0xEC,
    0xB4, 0xD3, 0x4E, 0xB1, 0xA7, 0x6A, 0x36, 0x78, 0x40, 0xC6, 0xA8, 0x52, 0x25, 0xC4, 0xA7, 0x32,
    0xB1, 0x49, 0x40, 0x08, 0x72, 0xD4, 0x8C, 0x56, 0x5B, 0x3C, 0x43, 0xA9, 0xED, 0x52, 0x6B, 0x32,
    0x3B, 0xA7, 0x96, 0xBC, 0xE4, 0x5A, 0xBF, 0x49, 0xF3, 0x15, 0xC3, 0xFB, 0x13, 0x3B, 0xA7, 0x74,
    0xFD, 0x0C, 0x0B, 0x18, 0x78, 0x40, 0x32, 0x92, 0x26, 0x60, 0x52, 0x02, 0x23, 0x8F, 0x86, 0x7C,
    0x12, 0x10, 0xA2, 0xBB, 0xE0, 0xDD, 0x8A, 0x47, 0xED, 0xF9, 0xF3, 0xE7, 0xA7, 0x6E, 0xD1, 0xDD,
    0x45, 0x69, 0xD8, 0xEF, 0x82, 0x3E, 0xE8, 0xDF, 0xBF, 0x1D, 0x77, 0xDC, 0xB1, 0xEB, 0x3F, 0x11,
    0x5C, 0x7F, 0xE4, 0x02, 0x46, 0x82, 0x07, 0xE4, 0xBF, 0x78, 0x07, 0x01, 0x9F, 0x6B, 0x52, 0xE4,
    0x93, 0x80, 0x10, 0xDD, 0x65, 0xE9, 0x56, 0x3C, 0x13, 0xEE, 0xB9, 0xE7, 0x9E, 0x95, 0x2B, 0x57,
    0xD2, 0x9F, 0x9D, 0x79, 0xB3, 0x66, 0x5B, 0x5B, 0x5B, 0x81, 0x6B, 0xFB, 0xF7, 0xEF, 0x91, 0x47,
    0x1E, 0x19, 0x39, 0x72, 0xA4, 0x45, 0x9D, 0x79, 0xAE, 0x4C, 0x02, 0x02, 0x06, 0x1E, 0x90, 0xFF,
    0xE2, 0x6D, 0x03, 0x5E, 0x09, 0x0A, 0xC9, 0x24, 0x7E, 0x2A, 0x08, 0xDA, 0x91, 0x4F, 0x82, 0x94,
    0x3F, 0xEE, 0xF6, 0x24, 0x9E, 0x8F, 0x69, 0x73, 0xE7, 0xCE, 0x8D, 0x47, 0x67, 0x6B, 0xFB, 0x81,
    0x23, 0xD5, 0xB2, 0x8D, 0x9F, 0x47, 0xB4, 0x14, 0x60, 0xDD, 0x3C, 0xB2, 0x24, 0x31, 0x91, 0x4F,
    0x82, 0xC4, 0xAE, 0x79, 0xEC, 0x49, 0x3C, 0x13, 0xBC, 0xD5, 0x57, 0xEA, 0x3F, 0xB2, 0x08, 0xD2,
    0xC6, 0xD0, 0x5C, 0x90, 0xD1, 0x8E, 0xFC, 0x9E, 0x8B, 0xED, 0x45, 0x3C, 0x93, 0xA7, 0x4C, 0x99,
    0xE2, 0x6D, 0xC6, 0x2E, 0x66, 0x37, 0x5B, 0x01, 0x0C, 0xA0, 0x1A, 0xE1, 0x68, 0xEF, 0x35, 0x57,
    0xEF, 0xE2, 0x7D, 0xFE, 0xF9, 0xE7, 0xF6, 0x6F, 0xAF, 0x81, 0x9A, 0x03, 0x32, 0x64, 0x00, 0xE1,
    0x68, 0xEF, 0x35, 0x60, 0xEF, 0xE2, 0x09, 0x71, 0xF7, 0xDD, 0x77, 0xBF, 0xF1, 0xC6, 0x1B, 0xF6,
    0x32, 0xBB, 0xD9, 0x72, 0x65, 0x00, 0xC9, 0xA8, 0x46, 0x78, 0x3D, 0x59, 0xEA, 0x12, 0x4F, 0xA0,
    0x49, 0x93, 0x26, 0xF9, 0x0D, 0xDE, 0x8E, 0x66, 0x37, 0x5B, 0x4E, 0x0C, 0xA0, 0x17, 0xC9, 0xA8,
    0xAE, 0x33, 0x7E, 0xBD, 0xE2, 0xD9, 0xC5, 0xD3, 0xA6, 0x4D, 0x13, 0xBD, 0xCE, 0xB8, 0xCD, 0x61,
    0x0D, 0x30, 0x80, 0x5E, 0x24, 0xA3, 0xBA, 0xCE, 0xB9, 0xF5, 0x8A, 0x27, 0x9C, 0x9F, 0x71, 0xEF,
    0xB8, 0xE3, 0x8E, 0x5C, 0x1F, 0x8E, 0x65, 0xA9, 0x62, 0x2B, 0x04, 0x13, 0x62, 0xD1, 0x8B, 0xE4,
    0xFA, 0xB3, 0xF5, 0x41, 0x3C, 0x41, 0x6F, 0xBE, 0xF9, 0xE6, 0x57, 0x5F, 0x7D, 0x55, 0x1A, 0x76,
    0xB3, 0x65, 0xC8, 0x00, 0x4A, 0x11, 0x8B, 0xDE, 0x3E, 0xC5, 0xEC, 0x9B, 0x78, 0x42, 0x9F, 0x7A,
    0xEA, 0xA9, 0xDE, 0xFC, 0x25, 0x63, 0x37, 0x5B, 0x26, 0x0C, 0x20, 0x13, 0xA5, 0x88, 0xED, 0x6B,
    0xB4, 0x3E, 0x8B, 0x27, 0xC1, 0xC5, 0x17, 0x5F, 0xDC, 0xD1, 0xD1, 0xE1, 0x02, 0xCD, 0x6E, 0xB6,
    0x7E, 0x32, 0x80, 0x46, 0x64, 0xA2, 0xB4, 0x81, 0x38, 0x8D, 0x88, 0xB7, 0x7C, 0xF9, 0x72, 0xDF,
    0xF4, 0x56, 0xAF, 0x5E, 0x2D, 0x71, 0x03, 0x29, 0x9B, 0x53, 0x02, 0x03, 0x08, 0x44, 0x23, 0x32,
    0x51, 0x1A, 0x9C, 0xF5, 0x1B, 0x8D, 0x88, 0x27, 0xBA, 0x27, 0xA2, 0x99, 0x6B, 0xFF, 0xA1, 0x59,
    0xE9, 0x75, 0x9B, 0xAD, 0x01, 0x06, 0x50, 0xE7, 0x63, 0x37, 0x1A, 0x91, 0xD9, 0xC0, 0x74, 0x53,
    0x1A, 0x14, 0xCF, 0x4C, 0x5F, 0xDE, 0x26, 0x4C, 0x98, 0x20, 0x3D, 0x10, 0xBA, 0xCD, 0xD6, 0x27,
    0x06, 0x90, 0x86, 0x3A, 0x04, 0xA2, 0xB1, 0x4F, 0x13, 0xE3, 0xC1, 0x8D, 0x8B, 0x27, 0x8A, 0xE7,
    0x5A, 0x0B, 0x07, 0x08, 0x50, 0x74, 0x9B, 0xAD, 0x4E, 0x06, 0xD0, 0x85, 0x34, 0xD4, 0x21, 0xB0,
    0xCE, 0x29, 0x35, 0x87, 0xF5, 0x4B, 0x3C, 0x11, 0x7D, 0xC8, 0xB1, 0x7C, 0x40, 0x01, 0x48, 0xB7,
    0xD9, 0x7A, 0x65, 0x00, 0x51, 0xE8, 0x42, 0x1A, 0xEA, 0x7A, 0x1D, 0xDC, 0xF3, 0x80, 0xFE, 0x8A,
    0x27, 0xBA, 0xE5, 0x73, 0xDD, 0x75, 0xD7, 0xB9, 0xF1, 0x82, 0xA5, 0xDB, 0x6C, 0x3D, 0x30, 0x80,
    0x22, 0x44, 0xA1, 0x0B, 0x69, 0x6B, 0x87, 0xF5, 0xEB, 0x90, 0x81, 0x78, 0xF2, 0xBB, 0x70, 0x1F,
    0x7B, 0xEC, 0xB1, 0x3F, 0xFD, 0xF4, 0x93, 0x57, 0x16, 0xDD, 0x66, 0xAB, 0xC9, 0x00, 0x72, 0x50,
    0x84, 0x28, 0x74, 0xD5, 0x1C, 0xD0, 0x57, 0x67, 0x36, 0xE2, 0xC9, 0xEA, 0x91, 0xE9, 0xF4, 0xD3,
    0x4F, 0xF7, 0xB2, 0x09, 0xA2, 0x6E, 0xB3, 0xA5, 0x18, 0x40, 0x0B, 0x72, 0x50, 0x84, 0xA8, 0xD4,
    0xA9, 0x86, 0xBB, 0x99, 0x89, 0x07, 0x81, 0x97, 0x95, 0x43, 0x0F, 0x3D, 0xD4, 0x67, 0x1E, 0x40,
    0x75, 0x9B, 0x2D, 0x30, 0x80, 0x10, 0xB4, 0x20, 0x07, 0x45, 0xC1, 0xD9, 0x7F, 0x23, 0x4B, 0xF1,
    0x12, 0x34, 0x3E, 0xF3, 0xF8, 0xC0, 0x0A, 0xAE, 0xEB, 0x7B, 0xE2, 0xD9, 0x98, 0x8F, 0x48, 0x40,
    0x05, 0x42, 0xD0, 0x92, 0x39, 0x0F, 0xD9, 0x8B, 0x07, 0xA2, 0x0F, 0xAC, 0xE7, 0x9E, 0x7B, 0xEE,
    0xD7, 0x5F, 0x7F, 0xED, 0xA7, 0x45, 0xDD, 0x8D, 0xB6, 0x29, 0x1F, 0x09, 0xA8, 0x40, 0x48, 0x1E,
    0x24, 0xE4, 0x22, 0x1E, 0xA0, 0x9E, 0xA6, 0xAC, 0xB5, 0x25, 0x4B, 0x96, 0x58, 0x7A, 0x1A, 0xCF,
    0x46, 0xD5, 0x94, 0xAC, 0x29, 0x1F, 0x09, 0xA8, 0xC8, 0xA9, 0xF6, 0xBC, 0xC4, 0x03, 0xD7, 0x9D,
    0x19, 0xF4, 0xE9, 0xD3, 0xA7, 0x7B, 0x38, 0xB6, 0x06, 0x79, 0x36, 0x92, 0xA6, 0x58, 0x25, 0x2B,
    0x5C, 0xF9, 0x48, 0xC8, 0xAF, 0xEA, 0x1C, 0xC5, 0x4B, 0x40, 0x7B, 0x15, 0xF5, 0x70, 0xDC, 0xDE,
    0xDE, 0x6E, 0x25, 0x6A, 0x89, 0xF3, 0xBF, 0x7A, 0x54, 0xA0, 0xA6, 0x58, 0x25, 0x2B, 0x3C, 0xEF,
    0x32, 0x73, 0x17, 0x4F, 0x01, 0x56, 0xDF, 0xF8, 0xF1, 0xE3, 0x27, 0x4F, 0x9E, 0xEC, 0x06, 0x30,
    0x78, 0xF0, 0x60, 0xE5, 0x71, 0xFE, 0xC7, 0x9A, 0xA2, 0x94, 0xA6, 0x40, 0x65, 0x2A, 0x56, 0xC9,
    0x05, 0x14, 0xD8, 0xB3, 0x78, 0x59, 0x02, 0xF0, 0x66, 0x7A, 0xF4, 0xD1, 0x47, 0xCF, 0x98, 0x31,
    0xC3, 0x25, 0xC5, 0x03, 0x98, 0x6A, 0xB3, 0x8C, 0x5E, 0x5E, 0x2C, 0x85, 0x28, 0x47, 0x51, 0x4A,
    0x53, 0xA0, 0x32, 0x0B, 0xC3, 0x52, 0x9C, 0x78, 0x4A, 0x5A, 0xB5, 0x6A, 0x95, 0xE7, 0xAE, 0x93,
    0x4E, 0x3A, 0x69, 0xD1, 0xA2, 0x45, 0xBE, 0xEF, 0xB9, 0x37, 0x70, 0x0E, 0xE8, 0xA6, 0x04, 0x85,
    0x28, 0x47, 0x51, 0x4A, 0x53, 0x60, 0x91, 0xE5, 0x14, 0x2A, 0x5E, 0x52, 0xD8, 0x8A, 0x15, 0x2B,
    0xFC, 0x70, 0x3C, 0x66, 0xCC, 0x98, 0x05, 0x0B, 0x16, 0xFC, 0xFA, 0xEB, 0xAF, 0x96, 0xAD, 0xC5,
    0x9B, 0x9C, 0x1A, 0x28, 0x47, 0x80, 0xC1, 0x06, 0x5E, 0x09, 0x0A, 0x51, 0x8E, 0xA2, 0x8A, 0x07,
    0x5F, 0x82, 0x78, 0x49, 0x91, 0xAA, 0x55, 0xF3, 0x59, 0x67, 0x9D, 0xF5, 0xC8, 0x23, 0x8F, 0xB8,
    0xE6, 0xB8, 0x61, 0x60, 0x24, 0x39, 0x55, 0xE5, 0x23, 0x90, 0xA0, 0x02, 0x0C, 0x36, 0xF0, 0x4A,
    0x50, 0x48, 0x59, 0x80, 0x4B, 0x13, 0x2F, 0x29, 0xF8, 0xBD, 0xF7, 0xDE, 0xBB, 0xF2, 0xCA, 0x2B,
    0x87, 0x0D, 0x1B, 0x76, 0xD1, 0x45, 0x17, 0x7D, 0xFA, 0xE9, 0xA7, 0x6B, 0xD6, 0xAC, 0x71, 0x21,
    0x42, 0x50, 0x72, 0xB6, 0x3A, 0x47, 0x90, 0x00, 0x03, 0x0F, 0x48, 0x50, 0x01, 0x06, 0x1B, 0xF8,
    0x72, 0x11, 0x96, 0x2C, 0x5E, 0x28, 0xDE, 0x7D, 0xDE, 0xA7, 0xBF, 0x89, 0x13, 0x27, 0xBA, 0x10,
    0x75, 0x74, 0x74, 0x58, 0xDD, 0xC8, 0x42, 0x59, 0x18, 0x50, 0x8A, 0x01, 0x00, 0x18, 0xC0, 0x80,
    0x04, 0x18, 0x78, 0x40, 0x82, 0x5A, 0x0A, 0x98, 0xAE, 0x49, 0xAB, 0x22, 0x5E, 0x82, 0xCC, 0xC7,
    0x08, 0x17, 0xA2, 0x11, 0x23, 0x46, 0xB4, 0xB6, 0xB6, 0x3E, 0xFE, 0xF8, 0xE3, 0x9E, 0xBC, 0x2D,
    0x76, 0xF4, 0xB9, 0xC1, 0x24, 0x03, 0x8A, 0x39, 0x4A, 0x27, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x0C,
    0x48, 0x80, 0x81, 0x57, 0x0C, 0x80, 0x3A, 0xB3, 0x54, 0x4B, 0xBC, 0x00, 0xFA, 0x95, 0x57, 0x5E,
    0x39, 0xFF, 0xFC, 0xF3, 0x47, 0x8D, 0x1A, 0x35, 0x69, 0xD2, 0xA4, 0xB6, 0xB6, 0xB6, 0xE4, 0x5F,
    0x55, 0xB6, 0x03, 0x70, 0x8A, 0x59, 0x1B, 0x42, 0x0B, 0x83, 0xFB, 0x69, 0x08, 0xA5, 0x09, 0x2B,
    0xB8, 0x14, 0xA2, 0x49, 0x27, 0xA9, 0xD4, 0x00, 0x80, 0x01, 0x0C, 0x67, 0x05, 0x5B, 0x45, 0xC5,
    0x0B, 0x4C, 0x59, 0xF8, 0x53, 0xA7, 0x4E, 0x3D, 0xEE, 0xB8, 0xE3, 0x76, 0xD9, 0x65, 0x97, 0x03,
    0x0F, 0x3C, 0x70, 0xD6, 0xAC, 0x59, 0x7E, 0x5B, 0x71, 0xE3, 0x71, 0x1D, 0xB3, 0x33, 0x90, 0x8E,
    0x71, 0x0D, 0xF5, 0x9A, 0xAE, 0x16, 0xE6, 0xA6, 0x0C, 0xA7, 0x34, 0xC3, 0x34, 0x53, 0x34, 0x5D,
    0x41, 0x84, 0x12, 0x50, 0x58, 0xC1, 0xA5, 0x90, 0x48, 0x3A, 0x49, 0xA5, 0x4E, 0x45, 0xA8, 0x5A,
    0xB7, 0x34, 0xF1, 0x1A, 0x20, 0xC2, 0x73, 0x9D, 0x77, 0x29, 0x1F, 0x0C, 0xDD, 0x78, 0x7C, 0xC5,
    0xB0, 0x33, 0x50, 0x8C, 0xF1, 0x87, 0x1E, 0x7A, 0x68, 0xF1, 0xE2, 0xC5, 0xB6, 0x0B, 0x0D, 0x28,
    0xE1, 0x51, 0x90, 0x24, 0x71, 0x7C, 0x5D, 0x4E, 0xA7, 0x0C, 0x30, 0xCC, 0x60, 0x53, 0x4C, 0x34,
    0x5D, 0x10, 0xA1, 0x04, 0x14, 0x56, 0x70, 0x29, 0xE2, 0x89, 0x15, 0xB7, 0xFF, 0x1F, 0x00, 0x00,
    0xFF, 0xFF, 0x46, 0xEA, 0x19, 0x9B, 0x00, 0x00, 0x00, 0x06, 0x49, 0x44, 0x41, 0x54, 0x03, 0x00,
    0x83, 0xBA, 0xC0, 0x62, 0xE9, 0x01, 0xDE, 0x47, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
    0xAE, 0x42, 0x60, 0x82,
};
static ComPtr<IStream> g_defaultProfileAvatarStream;
[[clang::no_destroy]] static std::unique_ptr<Gdiplus::Bitmap> g_defaultProfileAvatar;
static UniqueHandle g_profilePhotoThread;
static std::atomic<bool> g_profilePhotoStop{ false };

class ScopedCom {
    bool ok_;
public:
    ScopedCom() : ok_(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {}
    ~ScopedCom() { if (ok_) CoUninitialize(); }
    bool IsOk() const { return ok_; }
};

class ScopedSelectObject {
    HDC hdc_;
    HGDIOBJ old_;
public:
    ScopedSelectObject(HDC hdc, HGDIOBJ obj) : hdc_(hdc), old_(SelectObject(hdc, obj)) {}
    ~ScopedSelectObject() { if (hdc_ && old_ && old_ != HGDI_ERROR) SelectObject(hdc_, old_); }
    bool IsValid() const { return old_ && old_ != HGDI_ERROR; }
    ScopedSelectObject(const ScopedSelectObject&) = delete;
    ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;
};

class ScopedSaveDC {
    HDC dc_ = nullptr;
    int saved_ = 0;
public:
    explicit ScopedSaveDC(HDC dc) : dc_(dc), saved_(dc ? SaveDC(dc) : 0) {}
    ScopedSaveDC(const ScopedSaveDC&) = delete;
    ScopedSaveDC& operator=(const ScopedSaveDC&) = delete;
    ~ScopedSaveDC() { if (dc_ && saved_) RestoreDC(dc_, saved_); }
    explicit operator bool() const { return saved_ != 0; }
};

class ScopedDC {
    HWND hwnd_;
    HDC hdc_;
public:
    explicit ScopedDC(HWND hwnd) : hwnd_(hwnd), hdc_(GetDC(hwnd)) {}
    ~ScopedDC() { if (hwnd_ && hdc_) ReleaseDC(hwnd_, hdc_); }
    HDC get() const { return hdc_; }
    explicit operator bool() const { return hdc_ != nullptr; }
    ScopedDC(const ScopedDC&) = delete;
    ScopedDC& operator=(const ScopedDC&) = delete;
};

class ScopedPaint {
    HWND hwnd_;
    PAINTSTRUCT ps_{};
    HDC hdc_;
public:
    explicit ScopedPaint(HWND hwnd) : hwnd_(hwnd), hdc_(BeginPaint(hwnd, &ps_)) {}
    ~ScopedPaint() { if (hdc_) EndPaint(hwnd_, &ps_); }
    HDC get() const { return hdc_; }
    const RECT& PaintRect() const { return ps_.rcPaint; }
    ScopedPaint(const ScopedPaint&) = delete;
    ScopedPaint& operator=(const ScopedPaint&) = delete;
};

class ScopedTextColor {
    HDC hdc_;
    COLORREF old_;
public:
    ScopedTextColor(HDC hdc, COLORREF col) : hdc_(hdc), old_(SetTextColor(hdc, col)) {}
    ~ScopedTextColor() { if (hdc_) SetTextColor(hdc_, old_); }
    ScopedTextColor(const ScopedTextColor&) = delete;
    ScopedTextColor& operator=(const ScopedTextColor&) = delete;
};

class ScopedBkMode {
    HDC hdc_;
    int old_;
public:
    ScopedBkMode(HDC hdc, int mode) : hdc_(hdc), old_(SetBkMode(hdc, mode)) {}
    ~ScopedBkMode() { if (hdc_) SetBkMode(hdc_, old_); }
    ScopedBkMode(const ScopedBkMode&) = delete;
    ScopedBkMode& operator=(const ScopedBkMode&) = delete;
};


// ===================== SETTINGS =====================
// Windhawk string values are limited only by the backing store; the layout of
// a very large tile grid stays well below this buffer.
constexpr size_t kStorageStringMaxChars = 256 * 1024;

static std::wstring ReadStorageString(const wchar_t* name) {
    std::vector<wchar_t> buffer(kStorageStringMaxChars, L'\0');
    const size_t chars = Wh_GetStringValue(name, buffer.data(), buffer.size());
    if (chars == 0 || chars >= buffer.size()) return {};
    return std::wstring(buffer.data(), chars);
}

struct Settings {
    bool classicAccent = false;   // accentMode == classic (8.1 tile blue)
    bool liveTiles = false;
    bool tileGradients = true;    // Windows 8.1 tile shading
    int  bgMode = 0;              // 0 = 8.1 default purple, 1 = accent, 2 = custom
    COLORREF bgCustom = RGB(0x46, 0x17, 0xB4);
    int  tileColors = 0;          // 0 = Win8 palette, 1 = accent, 2 = icon-sampled
    int  liveTileIntervalMs = 4000;
    int  widthOverride = 0;
    bool privacyMode = false;
    std::wstring profilePicturePath;
    // Layout to apply to the tile grid. Either the stored layout from mod
    // storage, or a layout pasted in the tileLayout setting that has not been
    // imported yet (see tileLayoutImportPending).
    std::wstring tileLayout;
    bool tileLayoutImportPending = false;

    void Load() {
        using WindhawkUtils::StringSetting;
        classicAccent = _wcsicmp(StringSetting::make(L"accentMode").get(), L"classic") == 0;

        liveTiles = Wh_GetIntSetting(L"liveTileAnimation") != 0;
        tileGradients = Wh_GetIntSetting(L"tileGradients") != 0;

        {
            const std::wstring backgroundMode = StringSetting::make(L"backgroundColorMode").get();
            bgMode = (backgroundMode == L"accent") ? 1 : (backgroundMode == L"custom") ? 2 : 0;
        }
        {
            const std::wstring backgroundHex = StringSetting::make(L"backgroundColorCustom").get();
            unsigned parsed = 0;
            if (backgroundHex.size() == 6 &&
                swscanf_s(backgroundHex.c_str(), L"%06x", &parsed) == 1) {
                bgCustom = RGB((parsed >> 16) & 255, (parsed >> 8) & 255, parsed & 255);
            } else {
                bgCustom = RGB(0x46, 0x17, 0xB4);
            }
        }
        {
            const std::wstring tileColorMode = StringSetting::make(L"tileColorMode").get();
            tileColors = (tileColorMode == L"accent") ? 1 : (tileColorMode == L"icon") ? 2 : 0;
        }

        int interval = Wh_GetIntSetting(L"liveTileIntervalMs");
        liveTileIntervalMs = (interval >= 1000 && interval <= 60000) ? interval : 4000;

        widthOverride = Wh_GetIntSetting(L"menuWidthOverride");
        if (widthOverride < 0 || widthOverride > 3000) widthOverride = 0;
        privacyMode = Wh_GetIntSetting(L"privacyMode") != 0;

        profilePicturePath = StringSetting::make(L"profilePicturePath").get();
        { std::lock_guard<std::mutex> lock(g_profilePhotoPathMutex); g_profilePhotoPath = profilePicturePath; }

        // The tileLayout setting is a one-time import: it is applied only when its
        // value differs from the last value that was imported, so changing an
        // unrelated option never discards the user's own tile edits.
        const std::wstring importLayout = StringSetting::make(L"tileLayout").get();
        tileLayoutImportPending = false;
        if (!importLayout.empty() && importLayout != ReadStorageString(L"tileLayoutImported")) {
            tileLayout = importLayout;
            tileLayoutImportPending = true;
        } else {
            tileLayout = ReadStorageString(L"startMenuLayout");
        }

        Wh_Log(L"[Settings] Loaded: classicAccent=%d, liveTiles=%d, tileGradients=%d, bgMode=%d, tileColors=%d, privacyMode=%d, interval=%d ms, widthOverride=%d, layoutImport=%d",
               classicAccent ? 1 : 0, liveTiles ? 1 : 0,
               tileGradients ? 1 : 0, bgMode, tileColors, privacyMode ? 1 : 0,
               liveTileIntervalMs, widthOverride, tileLayoutImportPending ? 1 : 0);
    }
};

// ===================== DPI AND LAYOUT METRICS =====================
struct Metrics {
    UINT dpi = 96;

    int Scale(int v) const {
        return MulDiv(v, (int)dpi, 96);
    }

    // Base geometry constants (at 96 DPI), based on reference material:
    // the left column is 267 px wide, with a 44 px
    // account/power row, 40 px program rows with 32 px icons, a 34 px All apps
    // toggle and a 34 px white search box at the bottom.
    static constexpr int BASE_LEFT_COL_W        = 267;
    static constexpr int BASE_USER_HEADER_H     = 44;
    static constexpr int BASE_PLACE_ITEM_H      = 34;
    static constexpr int BASE_APP_ITEM_H        = 40;
    // Normal list: compact spacing around the separators, not title rows.
    static constexpr int BASE_FIRST_LIST_INSET  = 6;
    static constexpr int BASE_LIST_SECTION_GAP  = 8;
    static constexpr int BASE_SECTION_HEADER_H  = 26;
    static constexpr int BASE_ICON_SIZE         = 32;
    static constexpr int BASE_PLACE_ICON_SIZE   = 20;

    // All-apps view ("Back" section), Win7-style like the leaked RT Update 3
    // menu: the menu narrows to the left column alone (the reference implementation hides the
    // tiles host and drops the window to 273 px when there is nothing to show
    // on the right), and the programs list becomes a flat alphabetical list of
    // compact 30 px rows with 16 px icons — no letter section headers, folders
    // interleaved with the apps as in the Windows 7 All Programs list.
    static constexpr int BASE_NARROW_MENU_W     = 273;
    static constexpr int BASE_ALLAPPS_ITEM_H    = 30;
    static constexpr int BASE_ALLAPPS_ICON      = 16;
    static constexpr int BASE_ALLAPPS_TEXT_X    = 32;
    static constexpr int BASE_ALLAPPS_SB_W      = 17;

    // Tile grid metrics (96 DPI), from the reference implementation and the leaked RT Update 3
    // menu: the grid is built from 56 px small cells on a 64 px pitch (8 px
    // gutter), the way the Start screen flows tiles. Medium tiles span 2x2
    // cells, wide 4x2 and large 4x4, so the pane's two medium tile columns
    // share the group's four small columns.
    static constexpr int BASE_TILE_SMALL        = 56;
    static constexpr int BASE_TILE_GAP          = 8;
    static constexpr int BASE_TILE_MEDIUM       = 120;
    static constexpr int BASE_TILE_WIDE         = 248;
    static constexpr int BASE_TILE_LARGE        = 248;
    static constexpr int BASE_GROUP_HEADER_H    = 28;
    static constexpr int BASE_GROUP_GAP         = 24;
    static constexpr int BASE_GROUP_PAD_X       = 2;
    static constexpr int BASE_GROUP_PAD_Y       = 4;
    static constexpr int BASE_COLUMNS_PER_GROUP = 4;

    // Overall menu size (96 DPI): 267 (left column) + 2 + 4 * 56 + 3 * 8 (one
    // tile group = two medium columns) + 16 = 533 px wide; 520 px tall, exactly
    // the reference implementation's window and the leaked RT Update 3 menu.
    static constexpr int BASE_DEFAULT_MENU_W    = 533;
    static constexpr int BASE_DEFAULT_MENU_H    = 520;
    static constexpr int BASE_SCROLLBAR_W       = 8;
};

// The original 1507 layout computes its actual pane size against available
// rectangles before positioning. Keep the custom window inside the monitor's
// work area while preserving its minimum size when the work area permits it.
static RECT FitRectToWorkArea(const RECT& candidate, const RECT& workArea,
                              LONG desiredMinWidth, LONG desiredMinHeight) {
    const LONG workW = (std::max)(1L, workArea.right - workArea.left);
    const LONG workH = (std::max)(1L, workArea.bottom - workArea.top);
    const LONG minW = (std::min)((std::max)(1L, desiredMinWidth), workW);
    const LONG minH = (std::min)((std::max)(1L, desiredMinHeight), workH);
    const int64_t requestedW = static_cast<int64_t>(candidate.right) - candidate.left;
    const int64_t requestedH = static_cast<int64_t>(candidate.bottom) - candidate.top;
    const LONG width = static_cast<LONG>((std::max<int64_t>)(minW,
        (std::min<int64_t>)(requestedW, workW)));
    const LONG height = static_cast<LONG>((std::max<int64_t>)(minH,
        (std::min<int64_t>)(requestedH, workH)));
    const LONG left = (std::max)(workArea.left,
        (std::min)(candidate.left, workArea.right - width));
    const LONG top = (std::max)(workArea.top,
        (std::min)(candidate.top, workArea.bottom - height));
    return RECT{ left, top, left + width, top + height };
}

// ===================== COLOR AND DWM HELPERS =====================
static COLORREF GetSystemAccentColor(bool forceClassic) {
    if (forceClassic) {
        return RGB(10, 91, 196); // The Windows 8 tile palette blue the reference implementation uses
    }
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    return RGB(10, 91, 196);
}

// Windows 8 and 8.1 never drew a flat surface: colors are shaded towards white
// or black. Ported from the Windows 8.1 Start Screen Recreation mod.
static COLORREF ShadeColor(COLORREF c, float k) {
    // Positive k shades toward white, negative k toward black (by |k|). The
    // Start Screen recreation's original lambda captured a negative k and
    // ended up multiplying away from the target; this port blends correctly.
    const auto toward = [](int value, int target, float a) {
        float mixed = (float)value + ((float)target - (float)value) * a;
        if (mixed < 0.0f) mixed = 0.0f;
        if (mixed > 255.0f) mixed = 255.0f;
        return (int)(mixed + 0.5f);
    };
    if (k >= 0.0f)
        return RGB(toward(GetRValue(c), 255, k), toward(GetGValue(c), 255, k),
                   toward(GetBValue(c), 255, k));
    const float a = -k;
    return RGB(toward(GetRValue(c), 0, a), toward(GetGValue(c), 0, a),
               toward(GetBValue(c), 0, a));
}

static COLORREF BlendColor(COLORREF from, COLORREF to, float k) {
    if (k < 0.0f) k = 0.0f;
    if (k > 1.0f) k = 1.0f;
    const auto channel = [k](int a, int b) {
        const float mixed = (float)a + ((float)b - (float)a) * k;
        if (mixed < 0.0f) return 0;
        if (mixed > 255.0f) return 255;
        return (int)(mixed + 0.5f);
    };
    return RGB(channel(GetRValue(from), GetRValue(to)),
               channel(GetGValue(from), GetGValue(to)),
               channel(GetBValue(from), GetBValue(to)));
}

// The ten colors in the Windows 8-style reference palette.
struct MetroTilePaletteEntry {
    COLORREF left;   // darker end of the tile gradient
    COLORREF right;  // lighter end of the tile gradient
    COLORREF border; // the 1 px rim
};
static const MetroTilePaletteEntry kMetroTilePalette[] = {
    { RGB(0x09, 0x4A, 0xB2), RGB(0x0A, 0x5B, 0xC4), RGB(0x23, 0x6C, 0xCA) }, // Blue
    { RGB(0x00, 0x82, 0x99), RGB(0x00, 0xA0, 0xB1), RGB(0x1A, 0xAA, 0xB9) }, // Teal
    { RGB(0xD2, 0x47, 0x26), RGB(0xDC, 0x57, 0x2E), RGB(0xE0, 0x68, 0x43) }, // Pumpkin
    { RGB(0x00, 0x8A, 0x00), RGB(0x00, 0xA6, 0x00), RGB(0x1A, 0xAF, 0x1A) }, // Light green
    { RGB(0x12, 0x80, 0x23), RGB(0x15, 0x9E, 0x2A), RGB(0x2A, 0x8D, 0x39) }, // Pale green
    { RGB(0x59, 0x59, 0x59), RGB(0x6E, 0x6E, 0x6E), RGB(0x7D, 0x7D, 0x7D) }, // Gray
    { RGB(0x51, 0x33, 0xAB), RGB(0x64, 0x3E, 0xBF), RGB(0x74, 0x52, 0xC6) }, // Purple
    { RGB(0x8C, 0x00, 0x95), RGB(0xA7, 0x00, 0xAE), RGB(0xB0, 0x1A, 0xB6) }, // Pink
    { RGB(0x7B, 0x00, 0x00), RGB(0x98, 0x00, 0x00), RGB(0xA3, 0x1A, 0x1A) }, // Red
    { RGB(0x32, 0x32, 0x32), RGB(0x37, 0x37, 0x37), RGB(0x44, 0x44, 0x44) }, // Black
};

// the reference implementation assigns the Windows 8 default palette by sampling the app icon
// and keeping the closest palette color in HSL space, with fixed overrides for
// a few well-known tiles (TileColorCalculator.cs). Ported with two robustness
// tweaks: the dominant icon color replaces the plain bitmap average (alpha
// padding would drag it to black) and the hue difference wraps around the
// wheel. The same app always keeps its color.
static bool MetroNameMatches(const std::wstring& name, const wchar_t* const* names, size_t count) {
    for (size_t i = 0; i < count; ++i)
        if (_wcsicmp(name.c_str(), names[i]) == 0) return true;
    return false;
}

static size_t MetroPaletteIndexForIcon(const std::wstring& name, COLORREF appColor) {
    // Fixed per-app overrides from the reference implementation's FindClosestColor,
    // plus the localized names the Shell can produce for those folders.
    static const wchar_t* kPinkNames[] = { L"GitHub Desktop" };
    static const wchar_t* kRedNames[] = { L"Snipping Tool", L"Strumento Cattura" };
    static const wchar_t* kPurpleNames[] = { L"winspy", L"Core Temp" };
    static const wchar_t* kTealNames[] = { L"Downloads", L"Download", L"Videos", L"Video",
                                           L"File Explorer", L"Windows Explorer", L"Esplora file",
                                           L"Calculator", L"Calcolatrice", L"Task Manager",
                                           L"Gestione attività", L"Paint", L"Notepad", L"Blocco note" };
    static const wchar_t* kBlueNames[] = { L"Pictures", L"Immagini", L"Music", L"Musica",
                                           L"Desktop", L"Scrivania" };
    static const wchar_t* kGrayNames[] = { L"Documents", L"Documenti", L"Command Prompt", L"cmd" };
    if (MetroNameMatches(name, kPinkNames, ARRAYSIZE(kPinkNames))) return 7;   // Pink
    if (MetroNameMatches(name, kRedNames, ARRAYSIZE(kRedNames))) return 8;     // Red
    if (MetroNameMatches(name, kPurpleNames, ARRAYSIZE(kPurpleNames))) return 6; // Purple
    if (MetroNameMatches(name, kTealNames, ARRAYSIZE(kTealNames))) return 1;   // Teal
    if (MetroNameMatches(name, kBlueNames, ARRAYSIZE(kBlueNames))) return 0;   // Blue
    if (MetroNameMatches(name, kGrayNames, ARRAYSIZE(kGrayNames))) return 5;   // Gray

    const double r = GetRValue(appColor) / 255.0, g = GetGValue(appColor) / 255.0,
                 b = GetBValue(appColor) / 255.0;
    const double hi = (std::max)(r, (std::max)(g, b)), lo = (std::min)(r, (std::min)(g, b));
    const double delta = hi - lo;
    double h = 0.0, s = 0.0;
    const double l = (hi + lo) / 2.0;
    if (delta > 0.0) {
        if (hi == r) h = 60.0 * std::fmod((g - b) / delta, 6.0);
        else if (hi == g) h = 60.0 * ((b - r) / delta + 2.0);
        else h = 60.0 * ((r - g) / delta + 4.0);
        if (h < 0.0) h += 360.0;
        s = delta / (1.0 - std::fabs(2.0 * l - 1.0));
    }
    size_t best = 0;
    double bestDistance = 1.0e30;
    constexpr size_t kCount = sizeof(kMetroTilePalette) / sizeof(kMetroTilePalette[0]);
    for (size_t i = 0; i < kCount; ++i) {
        const COLORREF right = kMetroTilePalette[i].right;
        const double pr = GetRValue(right) / 255.0, pg = GetGValue(right) / 255.0,
                     pb = GetBValue(right) / 255.0;
        const double phi = (std::max)(pr, (std::max)(pg, pb)), plo = (std::min)(pr, (std::min)(pg, pb));
        const double pdelta = phi - plo;
        double ph = 0.0, ps = 0.0;
        const double pl = (phi + plo) / 2.0;
        if (pdelta > 0.0) {
            if (phi == pr) ph = 60.0 * std::fmod((pg - pb) / pdelta, 6.0);
            else if (phi == pg) ph = 60.0 * ((pb - pr) / pdelta + 2.0);
            else ph = 60.0 * ((pr - pg) / pdelta + 4.0);
            if (ph < 0.0) ph += 360.0;
            ps = pdelta / (1.0 - std::fabs(2.0 * pl - 1.0));
        }
        double hd = std::fabs(h - ph);
        if (hd > 180.0) hd = 360.0 - hd;
        const double distance = std::sqrt(hd * hd + (s - ps) * (s - ps) + (l - pl) * (l - pl));
        if (distance < bestDistance) {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

// The Start-screen-derived background of the Update 3 menu (the reference implementation paints
// it with the 8.1 Start screen DUI color): the purple of a fresh Windows 8.1
// Start screen by default, the accent color like the 8.1 "Show accent color on
// Start" option, or a custom color.
static COLORREF ResolveMenuBackgroundColor(const Settings& settings) {
    switch (settings.bgMode) {
        case 1: {
            COLORREF accent = GetSystemAccentColor(false);
            // A very light accent would leave the white Metro text unreadable.
            const int luminance = (GetRValue(accent) * 299 +
                                   GetGValue(accent) * 587 +
                                   GetBValue(accent) * 114) / 1000;
            if (luminance > 176) accent = ShadeColor(accent, -0.35f);
            return accent;
        }
        case 2:
            return settings.bgCustom;
        default:
            return RGB(0x46, 0x17, 0xB4); // Windows 8.1 default Start screen purple
    }
}

// The gradient pair and rim a tile is painted with.
struct TileColors {
    COLORREF c1;     // left end of the Windows 8.1 sweep
    COLORREF c2;     // right end of the Windows 8.1 sweep
    COLORREF border; // 1 px rim, from the reference palette
};

// ===================== APP AND TILE DATA MODEL =====================
enum class TileSize { Small, Medium, Wide, Large };

struct AppEntry {
    std::wstring name;
    std::wstring path;
    std::wstring aumid;
    std::wstring folder;
    std::wstring target; // raw shortcut target (shortcut entries only), used for identity matching
    FILETIME lastModified{};
    int launchCount = 0;
    HICON hIcon = nullptr; // Reference to the icon in the catalog cache
    int searchExtra = 0; // 0 app, 1 setting, 2 file
    size_t searchExtraIndex = 0;
};

struct FolderGroup {
    std::wstring name;
    bool expanded = false;
    std::vector<AppEntry> apps;
};

struct LetterSection {
    wchar_t letter = 0;
    std::vector<AppEntry> apps;
    std::vector<FolderGroup> folders;
};

struct Tile {
    std::wstring name;
    std::wstring aumid;
    std::wstring path;
    TileSize size = TileSize::Medium;
    int col = 0; // column in tile units (0, 1, 2)
    int row = 0; // row in tile units (0, 1, 2, ...)
    HICON hIcon = nullptr;
    COLORREF appColor = RGB(0, 120, 215); // sampled from the icon once, outside the paint loop
    COLORREF flatIconBackground = 0;
    bool hasFlatIconBackground = false;
    bool available = true; // runtime state, not serialized: persisted tiles are kept even when stale

    // Live tile flag. Only tiles for which HasLocalLiveContent() is true get a
    // back face (the Calendar tile: weekday + date computed locally); the live*
    // strings are kept for layout (de)serialization compatibility and are not
    // displayed.
    bool isLive = false;
    std::wstring liveTitle;
    std::wstring liveSubtitle;
    std::wstring liveDetail;
    float flipProgress = 0.0f; // 0.0f (fronte) a 1.0f (retro)
    bool flipping = false;
    bool showBack = false;

    // Animation state, never serialized: offset between the position the tile
    // is painted at and its slot, and the eased press/lift scale. Both are
    // driven by TIMER_TILE_ANIM and reset whenever the menu closes.
    float animOffsetX = 0.0f;
    float animOffsetY = 0.0f;
    float animScale = 1.0f;
};

// The only live content a third-party Start menu can legitimately produce is
// the Calendar tile's weekday and date. Microsoft's tile APIs only let an app
// update its own tile, so other apps' live content is not readable and those
// tiles are never flipped.
static bool CanHaveLocalLiveContent(const Tile& tile) {
    std::wstring aumid = tile.aumid;
    for (auto& ch : aumid) ch = static_cast<wchar_t>(towlower(ch));
    return aumid.find(L"windowslive.calendar") != std::wstring::npos ||
           _wcsicmp(tile.name.c_str(), L"Calendario") == 0 ||
           _wcsicmp(tile.name.c_str(), L"Calendar") == 0;
}

static bool HasLocalLiveContent(const Tile& tile) {
    return tile.isLive && CanHaveLocalLiveContent(tile);
}

struct TileGroup {
    std::wstring title;
    std::vector<Tile> tiles;
};

// Resolve a shortcut's own icon resource only when its Shell image-list entry
// is the generic .lnk glyph. This follows the same target/icon-location idea used
// by OpenStartScreen, but uses native documented Shell COM and HICON ownership.
// Loads the shortcut and returns its icon (never the composite .lnk image with
// the arrow overlay). The raw, unresolved target path is returned in
// `targetOut` so callers can identify the app without a slow IShellLink::Resolve.
// Resolves the colors of one tile: the Windows 8 default palette assigned per
// app (the reference implementation), the accent color, or the color sampled from the app icon.
static TileColors ResolveTileColors(const Tile& tile, const Settings& settings) {
    if (settings.tileColors == 0) {
        if (tile.hasFlatIconBackground) {
            COLORREF base=tile.flatIconBackground;
            return TileColors{base,base,ShadeColor(base,-0.28f)};
        }
        const MetroTilePaletteEntry& entry =
            kMetroTilePalette[MetroPaletteIndexForIcon(tile.name, tile.appColor)];
        if (!settings.tileGradients) {
            const COLORREF mid = BlendColor(entry.left, entry.right, 0.5f);
            return TileColors{ mid, mid, entry.border };
        }
        return TileColors{ entry.left, entry.right, entry.border };
    }
    COLORREF base = GetSystemAccentColor(settings.classicAccent);
    if (settings.tileColors == 2 && tile.appColor) base = tile.appColor;
    if (!settings.tileGradients) return TileColors{ base, base, ShadeColor(base, -0.28f) };
    return TileColors{ ShadeColor(base, -0.08f), ShadeColor(base, 0.08f), ShadeColor(base, -0.28f) };
}

static HICON GetShortcutIconNoOverlay(LPCWSTR shortcut, std::wstring* targetOut = nullptr) {
    if (!shortcut || !*shortcut) return nullptr;
    ComPtr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&link)))) return nullptr;
    ComPtr<IPersistFile> file;
    if (FAILED(link.As(&file)) || FAILED(file->Load(shortcut,STGM_READ))) return nullptr;
    if (targetOut) {
        wchar_t rawTarget[MAX_PATH]={};
        if (SUCCEEDED(link->GetPath(rawTarget,ARRAYSIZE(rawTarget),nullptr,SLGP_RAWPATH))) *targetOut=rawTarget;
    }
    wchar_t iconPath[MAX_PATH]={}; int iconIndex=0;
    if (SUCCEEDED(link->GetIconLocation(iconPath,ARRAYSIZE(iconPath),&iconIndex))&&iconPath[0]) {
        wchar_t expanded[MAX_PATH]={}; DWORD n=ExpandEnvironmentStringsW(iconPath,expanded,ARRAYSIZE(expanded));
        LPCWSTR path=(n&&n<=ARRAYSIZE(expanded))?expanded:iconPath;
        HICON large=nullptr,small=nullptr;
        if (ExtractIconExW(path,iconIndex,&large,&small,1)>0) {
            if (small) DestroyIcon(small);
            if (large) return large;
        } else { if(large)DestroyIcon(large);if(small)DestroyIcon(small); }
    }
    wchar_t target[MAX_PATH]={}; WIN32_FIND_DATAW fd{};
    if (SUCCEEDED(link->GetPath(target,ARRAYSIZE(target),&fd,SLGP_RAWPATH))&&target[0]) {
        wchar_t expanded[MAX_PATH]={}; DWORD n=ExpandEnvironmentStringsW(target,expanded,ARRAYSIZE(expanded));
        LPCWSTR path=(n&&n<=ARRAYSIZE(expanded))?expanded:target;
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(path,0,&sfi,sizeof(sfi),SHGFI_ICON|SHGFI_SHELLICONSIZE)&&sfi.hIcon)
            return sfi.hIcon;
    }
    return nullptr;
}
// ===================== APP CATALOG =====================
// Extracts a Shell icon through the item's own icon provider instead of relying
// solely on the SHGetFileInfo cache for virtual AppsFolder items.
static HICON ExtractShellIconFromPidl(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl) return nullptr;
    ComPtr<IShellFolder> parent;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(pidl, IID_PPV_ARGS(&parent), &child)) || !parent || !child)
        return nullptr;

    ComPtr<IExtractIconW> extractor;
    if (FAILED(parent->GetUIObjectOf(nullptr, 1, &child, IID_IExtractIconW, nullptr,
                                    reinterpret_cast<void**>(extractor.GetAddressOf()))) || !extractor)
        return nullptr;

    wchar_t location[MAX_PATH]{};
    int iconIndex = 0;
    UINT flags = 0;
    if (FAILED(extractor->GetIconLocation(GIL_FORSHELL, location, ARRAYSIZE(location),
                                          &iconIndex, &flags))) return nullptr;

    HICON largeRaw = nullptr, smallRaw = nullptr;
    const HRESULT hr = extractor->Extract(location, iconIndex, &largeRaw, &smallRaw,
                                           MAKELONG(48, 48));
    UniqueIcon large(largeRaw), small(smallRaw);
    if (SUCCEEDED(hr) && large) return large.release();
    if (small) return small.release();

    SHFILEINFOW info{};
    if (SHGetFileInfoW(reinterpret_cast<LPCWSTR>(pidl), 0, &info, sizeof(info),
                       SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_SHELLICONSIZE))
        return info.hIcon;
    return nullptr;
}

static bool GetKnownFolderPathString(REFKNOWNFOLDERID folderId, std::wstring& result) {
    PWSTR rawPath = nullptr;
    if (FAILED(SHGetKnownFolderPath(folderId, KF_FLAG_DEFAULT, nullptr, &rawPath)) || !rawPath)
        return false;
    struct CoTaskMemPathDeleter { void operator()(wchar_t* p) const { CoTaskMemFree(p); } };
    std::unique_ptr<wchar_t, CoTaskMemPathDeleter> path(rawPath);
    result.assign(path.get());
    return !result.empty();
}

// Per-user snapshot in Windhawk mod storage, not separate image files.
// Two banks prevent an interrupted write from replacing a good snapshot.
namespace AppSnapshot {
constexpr uint32_t kMagic=0x31534d41, kMaxBytes=16*1024*1024, kChunk=65536;
struct Manifest { uint32_t magic, bank, bytes, chunks, hash; };
static std::wstring Key() {
    HANDLE raw=nullptr;
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&raw)) return {};
    UniqueHandle token(raw); DWORD bytes=0;
    GetTokenInformation(token.get(),TokenUser,nullptr,0,&bytes);
    if (!bytes || bytes>65536) return {};
    std::vector<BYTE> data(bytes);
    if (!GetTokenInformation(token.get(),TokenUser,data.data(),bytes,&bytes)) return {};
    LPWSTR sid=nullptr;
    if (!ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(data.data())->User.Sid,&sid)) return {};
    std::wstring name=L"AppSnapshot.v1."; name+=sid; LocalFree(sid); return name;
}
static uint32_t Hash(const std::vector<BYTE>& data) {
    uint32_t h=2166136261u;
    for (BYTE b:data) h=(h^b)*16777619u;
    return h;
}
static bool Read(std::vector<BYTE>& out) {
    std::wstring key=Key(); if (key.empty()) return false;
    Manifest m{};
    if (Wh_GetBinaryValue((key+L".manifest").c_str(),&m,sizeof(m))!=sizeof(m) ||
        m.magic!=kMagic || m.bank>1 || !m.bytes || m.bytes>kMaxBytes ||
        m.chunks!=(m.bytes+kChunk-1)/kChunk) return false;
    out.resize(m.bytes);
    for (uint32_t i=0;i<m.chunks;++i) {
        uint32_t offset=i*kChunk, n=std::min(kChunk,m.bytes-offset);
        std::wstring part=key+L"."+std::to_wstring(m.bank)+L"."+std::to_wstring(i);
        if (Wh_GetBinaryValue(part.c_str(),out.data()+offset,n)!=n) { out.clear(); return false; }
    }
    if (Hash(out)!=m.hash) { out.clear(); return false; }
    return true;
}
static bool Write(const std::vector<BYTE>& bytes) {
    if (bytes.empty() || bytes.size()>kMaxBytes) return false;
    std::wstring key=Key(); if (key.empty()) return false;
    Manifest old{};
    bool valid=Wh_GetBinaryValue((key+L".manifest").c_str(),&old,sizeof(old))==sizeof(old) && old.magic==kMagic && old.bank<=1;
    Manifest m{kMagic,valid ? 1u-old.bank : 0u,static_cast<uint32_t>(bytes.size()),
               static_cast<uint32_t>((bytes.size()+kChunk-1)/kChunk),Hash(bytes)};
    for (uint32_t i=0;i<m.chunks;++i) {
        uint32_t offset=i*kChunk, n=std::min(kChunk,m.bytes-offset);
        std::wstring part=key+L"."+std::to_wstring(m.bank)+L"."+std::to_wstring(i);
        if (!Wh_SetBinaryValue(part.c_str(),bytes.data()+offset,n)) return false;
    }
    return Wh_SetBinaryValue((key+L".manifest").c_str(),&m,sizeof(m))!=FALSE;
}
static void Put32(std::vector<BYTE>& out,uint32_t v) {
    for (int i=0;i<4;++i) out.push_back(static_cast<BYTE>(v>>(i*8)));
}
static bool Get32(const std::vector<BYTE>& in,size_t& pos,uint32_t& v) {
    if (pos>in.size() || in.size()-pos<4) return false;
    v=0; for (int i=0;i<4;++i) v|=uint32_t(in[pos++])<<(i*8); return true;
}
static bool PutText(std::vector<BYTE>& out,const std::wstring& s) {
    if (s.size()>8192 || out.size()+4+s.size()*sizeof(wchar_t)>kMaxBytes) return false;
    Put32(out,static_cast<uint32_t>(s.size()));
    const BYTE* p=reinterpret_cast<const BYTE*>(s.data()); out.insert(out.end(),p,p+s.size()*sizeof(wchar_t));
    return true;
}
static bool GetText(const std::vector<BYTE>& in,size_t& pos,std::wstring& s) {
    uint32_t n=0; if (!Get32(in,pos,n) || n>8192 || n>(in.size()-pos)/sizeof(wchar_t)) return false;
    s.resize(n); if (n) memcpy(s.data(),in.data()+pos,n*sizeof(wchar_t)); pos+=n*sizeof(wchar_t);
    return s.find(L'\0')==std::wstring::npos;
}
// A fixed-size image gives a bounded cache and avoids handles across processes.
constexpr int kIconSize=48;
constexpr size_t kIconBytes=kIconSize*kIconSize*4;
static bool IconPixels(HICON icon,std::vector<BYTE>& out) {
    auto source=CreateGdiplusBitmapFromIcon(icon); if (!source) return false;
    Gdiplus::Bitmap target(kIconSize,kIconSize,PixelFormat32bppARGB);
    if (target.GetLastStatus()!=Gdiplus::Ok) return false;
    {
        Gdiplus::Graphics graphics(&target);
        if (graphics.GetLastStatus()!=Gdiplus::Ok) return false;
        graphics.Clear(Gdiplus::Color(0,0,0,0));
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        if (graphics.DrawImage(source.get(),0,0,kIconSize,kIconSize)!=Gdiplus::Ok) return false;
    }
    Gdiplus::Rect r(0,0,kIconSize,kIconSize); Gdiplus::BitmapData locked{};
    if (target.LockBits(&r,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&locked)!=Gdiplus::Ok) return false;
    out.resize(kIconBytes);
    for (int y=0;y<kIconSize;++y)
        memcpy(out.data()+y*kIconSize*4,static_cast<BYTE*>(locked.Scan0)+y*locked.Stride,kIconSize*4);
    target.UnlockBits(&locked); return true;
}
static HICON MakeIcon(const BYTE* pixels) {
    BITMAPINFO bi{}; bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth=kIconSize;
    bi.bmiHeader.biHeight=-kIconSize; bi.bmiHeader.biPlanes=1; bi.bmiHeader.biBitCount=32; bi.bmiHeader.biCompression=BI_RGB;
    void* bits=nullptr; UniqueBitmap color(CreateDIBSection(nullptr,&bi,DIB_RGB_COLORS,&bits,nullptr,0));
    if (!color || !bits) return nullptr;
    memcpy(bits,pixels,kIconBytes);
    BYTE maskBits[((kIconSize+15)/16)*2*kIconSize]{};
    UniqueBitmap mask(CreateBitmap(kIconSize,kIconSize,1,1,maskBits));
    if (!mask) return nullptr;
    ICONINFO info{}; info.fIcon=TRUE; info.hbmColor=color.get(); info.hbmMask=mask.get();
    return CreateIconIndirect(&info);
}
}

class AppCatalog {
public:
    void Refresh(bool quick = false) {
        quickScan_ = quick;
        apps_.clear();
        iconCache_.clear();
        std::set<std::wstring> seenPaths;

        // 1. User shortcuts (documented Known Folder API).
        std::wstring userPrograms;
        if (GetKnownFolderPathString(FOLDERID_Programs, userPrograms)) {
            ScanDirectory(userPrograms, L"", seenPaths);
        }

        // 2. Common (all users) shortcuts.
        std::wstring commonPrograms;
        if (GetKnownFolderPathString(FOLDERID_CommonPrograms, commonPrograms)) {
            ScanDirectory(commonPrograms, L"", seenPaths);
        }

        // 3. UWP / Store apps from FOLDERID_AppsFolder
        ScanAppsFolder(seenPaths);

        // Sort alphabetically
        std::sort(apps_.begin(), apps_.end(), [](const AppEntry& a, const AppEntry& b) {
            return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
        });

        ApplyLaunchCounts();
        BuildSections();
        Wh_Log(L"[Catalog] Refreshed: total %u apps catalogued", (unsigned)apps_.size());
    }

    bool LoadSnapshot() {
        std::vector<BYTE> bytes;
        if (!AppSnapshot::Read(bytes)) return false;
        size_t pos=0; uint32_t version=0,count=0;
        if (!AppSnapshot::Get32(bytes,pos,version) || version!=1 ||
            !AppSnapshot::Get32(bytes,pos,count) || !count || count>10000) return false;
        std::vector<AppEntry> entries; entries.reserve(count);
        std::unordered_map<std::wstring,UniqueIcon> icons;
        for (uint32_t i=0;i<count;++i) {
            AppEntry a; uint32_t lo=0,hi=0,iconLen=0;
            if (!AppSnapshot::GetText(bytes,pos,a.name) || !AppSnapshot::GetText(bytes,pos,a.path) ||
                !AppSnapshot::GetText(bytes,pos,a.aumid) || !AppSnapshot::GetText(bytes,pos,a.folder) ||
                !AppSnapshot::GetText(bytes,pos,a.target) || !AppSnapshot::Get32(bytes,pos,lo) ||
                !AppSnapshot::Get32(bytes,pos,hi) || !AppSnapshot::Get32(bytes,pos,iconLen) ||
                (iconLen && iconLen!=AppSnapshot::kIconBytes) || iconLen>bytes.size()-pos || a.name.empty()) return false;
            a.lastModified.dwLowDateTime=lo; a.lastModified.dwHighDateTime=hi;
            if (iconLen) {
                UniqueIcon icon(AppSnapshot::MakeIcon(bytes.data()+pos));
                if (icon) { std::wstring key=std::to_wstring(i); a.hIcon=icon.get(); icons.emplace(std::move(key),std::move(icon)); }
                pos+=iconLen;
            }
            entries.push_back(std::move(a));
        }
        if (pos!=bytes.size()) return false;
        apps_=std::move(entries); iconCache_=std::move(icons); ApplyLaunchCounts(); BuildSections();
        Wh_Log(L"[CatalogCache] restored %u app entries",count);
        return true;
    }
    bool SaveSnapshot() const {
        if (apps_.empty() || apps_.size()>10000) return false;
        std::vector<BYTE> out; out.reserve(1024*1024);
        AppSnapshot::Put32(out,1); AppSnapshot::Put32(out,static_cast<uint32_t>(apps_.size()));
        for (const auto& a:apps_) {
            if (!AppSnapshot::PutText(out,a.name) || !AppSnapshot::PutText(out,a.path) ||
                !AppSnapshot::PutText(out,a.aumid) || !AppSnapshot::PutText(out,a.folder) ||
                !AppSnapshot::PutText(out,a.target)) return false;
            AppSnapshot::Put32(out,a.lastModified.dwLowDateTime); AppSnapshot::Put32(out,a.lastModified.dwHighDateTime);
            std::vector<BYTE> pixels;
            bool hasIcon=a.hIcon && AppSnapshot::IconPixels(a.hIcon,pixels) &&
                         out.size()+4+pixels.size()<=AppSnapshot::kMaxBytes;
            if (out.size()+4>AppSnapshot::kMaxBytes) return false;
            AppSnapshot::Put32(out,hasIcon ? static_cast<uint32_t>(pixels.size()) : 0);
            if (hasIcon) out.insert(out.end(),pixels.begin(),pixels.end());
        }
        bool ok=AppSnapshot::Write(out);
        Wh_Log(L"[CatalogCache] saved apps=%u bytes=%u ok=%d",(unsigned)apps_.size(),(unsigned)out.size(),ok);
        return ok;
    }

    // Preserve live usage and folder expansion changes made while a refresh ran.
    void InheritUserState(const AppCatalog& previous) {
        launchCounts_ = previous.launchCounts_;
        launchCountsLoaded_ = previous.launchCountsLoaded_;
        expandedFolders_ = previous.expandedFolders_;
        ApplyLaunchCounts();
        BuildSections();
    }

    const std::vector<AppEntry>& Apps() const { return apps_; }
    const std::vector<LetterSection>& Sections() const { return sections_; }
    const std::vector<AppEntry>& MostUsed() const { return mostUsed_; }
    const std::vector<AppEntry>& RecentlyAdded() const { return recentlyAdded_; }

    const AppEntry* FindApp(const std::wstring& term) const {
        for (const auto& a : apps_) {
            if (_wcsicmp(a.name.c_str(), term.c_str()) == 0 ||
                (!a.aumid.empty() && _wcsicmp(a.aumid.c_str(), term.c_str()) == 0)) {
                return &a;
            }
        }
        return nullptr;
    }

    // Launch counts are keyed by path (shortcut entries) or AUMID (AppsFolder
    // entries) and persisted in mod storage, so Most used survives Explorer
    // restarts. BuildSections() preserves the expanded state of folders.
    void RecordLaunch(const std::wstring& pathOrAumid) {
        if (pathOrAumid.empty()) return;
        int& count = launchCounts_[pathOrAumid];
        ++count;
        for (auto& a : apps_) {
            if (a.path == pathOrAumid || (!a.aumid.empty() && a.aumid == pathOrAumid)) {
                a.launchCount = count;
                break;
            }
        }
        SaveLaunchCounts();
        BuildSections();
    }

    // Folder headers in All apps are toggled through the catalog so the
    // expanded state can be preserved when the sections are rebuilt.
    void ToggleFolder(size_t sectionIndex, size_t folderIndex) {
        if (sectionIndex >= sections_.size()) return;
        auto& folders = sections_[sectionIndex].folders;
        if (folderIndex >= folders.size()) return;
        FolderGroup& folder = folders[folderIndex];
        folder.expanded = !folder.expanded;
        const std::wstring key = FolderKey(sections_[sectionIndex].letter, folder.name);
        if (folder.expanded) expandedFolders_.insert(key);
        else expandedFolders_.erase(key);
    }

private:
    bool quickScan_ = false;
    std::vector<AppEntry> apps_;
    std::vector<LetterSection> sections_;
    std::vector<AppEntry> mostUsed_;
    std::vector<AppEntry> recentlyAdded_;
    std::unordered_map<std::wstring, UniqueIcon> iconCache_;
    std::unordered_map<std::wstring, int> launchCounts_;
    std::set<std::wstring> expandedFolders_;
    bool launchCountsLoaded_ = false;

    // Case-insensitive match of a token against the AUMID (prefix, e.g. the
    // package family name) or against the shortcut's target/file name.
    static bool MatchesIdentityToken(const AppEntry& a, const wchar_t* token) {
        const size_t tokenLen = wcslen(token);
        if (!a.aumid.empty() && _wcsnicmp(a.aumid.c_str(), token, tokenLen) == 0) return true;
        if (a.aumid.empty() && !a.path.empty()) {
            if (StrStrIW(a.path.c_str(), token)) return true;
            if (!a.target.empty() && StrStrIW(a.target.c_str(), token)) return true;
        }
        return false;
    }

    static std::wstring FolderKey(wchar_t letter, const std::wstring& name) {
        std::wstring key(1, letter);
        key += L'|';
        key += name;
        return key;
    }

    // Storage format: "<key>=<count>\n" per entry; keys never contain '\n'
    // and the '=' separator is searched from the end because paths may contain it.
    void LoadLaunchCounts() {
        if (launchCountsLoaded_) return;
        launchCountsLoaded_ = true;
        launchCounts_.clear();
        const std::wstring stored = ReadStorageString(L"startMenuLaunchCounts");
        size_t pos = 0;
        while (pos < stored.size()) {
            size_t lineEnd = stored.find(L'\n', pos);
            if (lineEnd == std::wstring::npos) lineEnd = stored.size();
            const std::wstring line = stored.substr(pos, lineEnd - pos);
            pos = lineEnd + 1;
            const size_t sep = line.rfind(L'=');
            if (sep == std::wstring::npos || sep == 0) continue;
            const int count = _wtoi(line.c_str() + sep + 1);
            if (count > 0) launchCounts_[line.substr(0, sep)] = count;
        }
    }

    void SaveLaunchCounts() {
        // Keep the store bounded: only the 64 most launched entries matter.
        std::vector<std::pair<std::wstring, int>> entries(launchCounts_.begin(), launchCounts_.end());
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        if (entries.size() > 64) entries.resize(64);
        std::wstring out;
        for (const auto& [key, count] : entries) {
            out += key;
            out += L'=';
            out += std::to_wstring(count);
            out += L'\n';
        }
        if (!Wh_SetStringValue(L"startMenuLaunchCounts", out.c_str()))
            Wh_Log(L"[Catalog] failed to persist launch counts");
    }

    void ApplyLaunchCounts() {
        LoadLaunchCounts();
        for (auto& a : apps_) {
            auto it = launchCounts_.find(a.path);
            if (it == launchCounts_.end() && !a.aumid.empty()) it = launchCounts_.find(a.aumid);
            a.launchCount = it != launchCounts_.end() ? it->second : 0;
        }
    }

    HICON CacheIcon(const std::wstring& key, HICON hIcon) {
        if (!hIcon) return nullptr;
        auto it = iconCache_.find(key);
        if (it != iconCache_.end()) {
            DestroyIcon(hIcon);
            return it->second.get();
        }
        iconCache_[key] = UniqueIcon(hIcon);
        return iconCache_[key].get();
    }

    void ScanDirectory(const std::wstring& rootDir, const std::wstring& subFolder, std::set<std::wstring>& seen) {
        std::wstring searchPattern = rootDir + (subFolder.empty() ? L"" : (L"\\" + subFolder)) + L"\\*";
        WIN32_FIND_DATAW fd{};
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
        if (hFind == INVALID_HANDLE_VALUE) return;
        struct FindCloseGuard { HANDLE h; ~FindCloseGuard() { if (h != INVALID_HANDLE_VALUE) FindClose(h); } } findGuard{hFind};

        do {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;

            std::wstring curRel = subFolder.empty() ? fd.cFileName : (subFolder + L"\\" + fd.cFileName);
            std::wstring fullPath = rootDir + L"\\" + curRel;

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ScanDirectory(rootDir, curRel, seen);
            } else {
                const wchar_t* ext = PathFindExtensionW(fd.cFileName);
                if (ext && _wcsicmp(ext, L".lnk") == 0) {
                    wchar_t baseName[MAX_PATH] = {};
                    wcscpy_s(baseName, fd.cFileName);
                    PathRemoveExtensionW(baseName);

                    std::wstring lowerName = baseName;
                    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);
                    if (seen.find(lowerName) != seen.end()) continue;
                    seen.insert(lowerName);

                    // Resolve the shortcut's explicit IconLocation or target icon.
                    // Do not ask the Shell for the .lnk's composite image, which
                    // may include the shortcut-arrow overlay.
                    std::wstring target;
                    HICON hIcon = quickScan_ ? nullptr : GetShortcutIconNoOverlay(fullPath.c_str(), &target);
                    if (hIcon) hIcon=CacheIcon(fullPath,hIcon);

                    AppEntry entry;
                    entry.name = baseName;
                    entry.path = fullPath;
                    entry.target = std::move(target);
                    entry.folder = subFolder;
                    entry.lastModified = fd.ftLastWriteTime;
                    entry.hIcon = hIcon;
                    apps_.push_back(std::move(entry));
                }
            }
        } while (FindNextFileW(hFind, &fd));
    }

    void ScanAppsFolder(std::set<std::wstring>& seen) {
        PIDLIST_ABSOLUTE rawPidlApps = nullptr;
        if (FAILED(SHGetKnownFolderIDList(FOLDERID_AppsFolder, 0, nullptr, &rawPidlApps)) || !rawPidlApps) return;
        struct PidlScope { PIDLIST_ABSOLUTE p; ~PidlScope(){ if (p) ILFree(p); } } pidlAppsScope{rawPidlApps};
        PIDLIST_ABSOLUTE pidlApps = rawPidlApps;

        ComPtr<IShellFolder> desktop;
        if (SUCCEEDED(SHGetDesktopFolder(&desktop)) && desktop) {
            ComPtr<IShellFolder> appsFolder;
            if (SUCCEEDED(desktop->BindToObject(pidlApps, nullptr, IID_PPV_ARGS(&appsFolder))) && appsFolder) {
                ComPtr<IEnumIDList> enumList;
                if (SUCCEEDED(appsFolder->EnumObjects(nullptr, SHCONTF_NONFOLDERS | SHCONTF_FOLDERS, &enumList)) && enumList) {
                    PITEMID_CHILD child = nullptr;
                    ULONG fetched = 0;
                    while (enumList->Next(1, &child, &fetched) == S_OK && child) {
                        struct ChildPidlScope { PITEMID_CHILD p; ~ChildPidlScope(){ if (p) CoTaskMemFree(p); } } childScope{child};
                        wchar_t nameBuf[MAX_PATH] = {};
                        STRRET strret{};
                        if (SUCCEEDED(appsFolder->GetDisplayNameOf(child, SHGDN_NORMAL, &strret))) {
                            StrRetToBufW(&strret, child, nameBuf, ARRAYSIZE(nameBuf));
                        }

                        wchar_t aumidBuf[512] = {};
                        strret = {};
                        if (SUCCEEDED(appsFolder->GetDisplayNameOf(child, SHGDN_FORPARSING, &strret))) {
                            StrRetToBufW(&strret, child, aumidBuf, ARRAYSIZE(aumidBuf));
                        }

                        if (nameBuf[0] && aumidBuf[0]) {
                            std::wstring lower = nameBuf;
                            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
                            if (seen.find(lower) == seen.end()) {
                                seen.insert(lower);
                                std::wstring shellPath = std::wstring(L"shell:AppsFolder\\") + aumidBuf;

                                HICON hIcon = nullptr;
                                PIDLIST_ABSOLUTE parsedPidl = nullptr;
                                if (!quickScan_ && SUCCEEDED(SHParseDisplayName(shellPath.c_str(), nullptr, &parsedPidl, 0, nullptr)) && parsedPidl) {
                                    // SHParseDisplayName returns COM memory; it is released on
                                    // every path, including intermediate errors and early exits.
                                    struct PidlScope { PIDLIST_ABSOLUTE p; ~PidlScope(){ if (p) CoTaskMemFree(p); } } pidlScope{parsedPidl};
                                    hIcon = CacheIcon(shellPath, ExtractShellIconFromPidl(parsedPidl));
                                }

                                AppEntry entry;
                                entry.name = nameBuf;
                                entry.aumid = aumidBuf;
                                entry.path = shellPath;
                                entry.hIcon = hIcon;
                                apps_.push_back(std::move(entry));
                            }
                        }
                    }
                }
            }
        }
    }

    void BuildSections() {
        sections_.clear();
        std::map<wchar_t, std::vector<AppEntry>> groups;
        std::map<wchar_t, std::map<std::wstring, std::vector<AppEntry>>> folderGroups;

        for (const auto& a : apps_) {
            wchar_t initial = L'#';
            if (!a.name.empty()) {
                wchar_t ch = (wchar_t)::towupper(a.name[0]);
                if (ch >= L'A' && ch <= L'Z') initial = ch;
                else if (a.name[0] >= L'0' && a.name[0] <= L'9') initial = L'0';
                else if (a.name[0] > 0x7f) {
                    WORD charType = 0;
                    if (GetStringTypeW(CT_CTYPE1, a.name.data(), 1, &charType) && (charType & C1_ALPHA)) initial = L'~';
                    else initial = L'#';
                }
            }
            if (a.folder.empty()) {
                groups[initial].push_back(a);
            } else {
                folderGroups[initial][a.folder].push_back(a);
            }
        }

        // Add 0-9 only when a real app/folder starts with a digit, then A-Z;
        // punctuation stays in a separate # bucket and never creates a fake numeric section.
        std::vector<wchar_t> sectionOrder{ L'0' };
        for (wchar_t ch = L'A'; ch <= L'Z'; ++ch) sectionOrder.push_back(ch);
        sectionOrder.push_back(L'#');
        sectionOrder.push_back(L'~');
        for (wchar_t ch : sectionOrder) {
            auto appsIt = groups.find(ch);
            auto foldersIt = folderGroups.find(ch);
            bool hasApps = appsIt != groups.end() && !appsIt->second.empty();
            bool hasFolders = foldersIt != folderGroups.end() && !foldersIt->second.empty();
            if (!hasApps && !hasFolders) continue;
            LetterSection sec;
            sec.letter = ch;
            if (hasApps) sec.apps = appsIt->second;
            if (hasFolders) {
                for (auto& fg : foldersIt->second) {
                    FolderGroup group;
                    group.name = fg.first;
                    group.apps = fg.second;
                    group.expanded = expandedFolders_.count(FolderKey(ch, group.name)) != 0;
                    sec.folders.push_back(std::move(group));
                }
            }
            sections_.push_back(std::move(sec));
        }

        // Most used: top 6 by launch count.
        std::vector<AppEntry> sorted = apps_;
        std::sort(sorted.begin(), sorted.end(), [](const AppEntry& a, const AppEntry& b) {
            return a.launchCount > b.launchCount;
        });
        mostUsed_.clear();
        for (size_t i = 0; i < sorted.size() && i < 6; ++i) {
            if (sorted[i].launchCount <= 0) break;
            mostUsed_.push_back(sorted[i]);
        }

        // Until enough launches were recorded, fill Most used with well-known
        // system apps, matched by AUMID / executable name so the list does not
        // depend on the display language.
        if (mostUsed_.size() < 6) {
            struct DefaultApp { const wchar_t* tokens[4]; };
            static const DefaultApp defaults[] = {
                {{L"MSEdge", L"Microsoft.MicrosoftEdge", L"msedge.exe", nullptr}},
                {{L"Microsoft.Windows.Explorer", L"explorer.exe", nullptr, nullptr}},
                {{L"windows.immersivecontrolpanel", nullptr, nullptr, nullptr}},
                {{L"Microsoft.WindowsCalculator", L"calc.exe", nullptr, nullptr}},
                {{L"Microsoft.Windows.Photos", nullptr, nullptr, nullptr}},
                {{L"Microsoft.WindowsNotepad", L"Microsoft.Notepad", L"notepad.exe", nullptr}},
            };
            auto alreadyListed = [this](const AppEntry& a) {
                for (const auto& m : mostUsed_)
                    if (m.path == a.path && m.aumid == a.aumid) return true;
                return false;
            };
            for (const auto& def : defaults) {
                if (mostUsed_.size() >= 6) break;
                const AppEntry* match = nullptr;
                for (const wchar_t* token : def.tokens) {
                    if (!token) break;
                    for (const auto& a : apps_) {
                        if (MatchesIdentityToken(a, token)) { match = &a; break; }
                    }
                    if (match) break;
                }
                if (match && !alreadyListed(*match)) mostUsed_.push_back(*match);
            }
        }

        // Recently added: most recently modified entries.
        std::sort(sorted.begin(), sorted.end(), [](const AppEntry& a, const AppEntry& b) {
            return CompareFileTime(&a.lastModified, &b.lastModified) > 0;
        });
        recentlyAdded_.clear();
        for (size_t i = 0; i < sorted.size() && i < 3; ++i) {
            recentlyAdded_.push_back(sorted[i]);
        }
    }
};

// Validates classic tile targets. The presence of the .lnk is not enough: the
// shortcut is resolved silently and its target checked; namespace shortcuts are
// accepted when they carry a PIDL.
static bool IsUsableTileFileTarget(const std::wstring& path) {
    if (path.empty()) return false;
    const DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) return false;
    const wchar_t* extension = PathFindExtensionW(path.c_str());
    if (!extension || _wcsicmp(extension, L".lnk") != 0) return true;

    ScopedCom com;
    if (!com.IsOk()) return false;
    ComPtr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&link))) || !link) return false;
    ComPtr<IPersistFile> persist;
    if (FAILED(link.As(&persist)) || !persist || FAILED(persist->Load(path.c_str(), STGM_READ))) return false;
    const DWORD resolveFlags = SLR_NO_UI | SLR_NOUPDATE | SLR_NOSEARCH | SLR_NOTRACK | (400u << 16);
    if (FAILED(link->Resolve(nullptr, resolveFlags))) return false;

    wchar_t target[32768]{};
    WIN32_FIND_DATAW findData{};
    if (SUCCEEDED(link->GetPath(target, ARRAYSIZE(target), &findData, SLGP_RAWPATH)) && target[0]) {
        wchar_t expanded[32768]{};
        const DWORD n = ExpandEnvironmentStringsW(target, expanded, ARRAYSIZE(expanded));
        const wchar_t* resolved = (n && n <= ARRAYSIZE(expanded)) ? expanded : target;
        return GetFileAttributesW(resolved) != INVALID_FILE_ATTRIBUTES;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    const HRESULT pidlHr = link->GetIDList(&pidl);
    struct PidlCleanup { PIDLIST_ABSOLUTE p; ~PidlCleanup(){ if (p) CoTaskMemFree(p); } } cleanup{pidl};
    return SUCCEEDED(pidlHr) && pidl != nullptr;
}

static bool IsUsableCatalogApp(const AppEntry& app) {
    // AppsFolder entries are enumerated at refresh time and carry an AUMID;
    // shortcut entries must also pass the target check above.
    if (!app.aumid.empty()) return true;
    return IsUsableTileFileTarget(app.path);
}

// ===================== TILE LAYOUT =====================
class TileLayout {
public:
    std::vector<TileGroup> groups;

    void LoadDefault(const AppCatalog& catalog) {
        groups.clear();

        // One unnamed group, as on the RT Update 3 menu: no group header, the
        // tiles start right below the top padding.
        TileGroup g1;
        g1.title = L"";

        // The RT Update 3 arrangement (leaked menu and the reference implementation's gallery):
        // coordinates are small cells, so columns 0-3 span the pane's two
        // medium tile columns and rows step by 64 px. The left medium column
        // stacks two small tiles over three medium ones.
        Tile tPhotos;
        tPhotos.name = L"Foto";
        tPhotos.size = TileSize::Small;
        tPhotos.col = 0; tPhotos.row = 0;

        Tile tXbox;
        tXbox.name = L"Xbox";
        tXbox.size = TileSize::Small;
        tXbox.col = 0; tXbox.row = 1;

        Tile tMail;
        tMail.name = L"Posta";
        tMail.size = TileSize::Medium;
        tMail.col = 0; tMail.row = 2;

        Tile tCal;
        tCal.name = L"Calendario";
        tCal.size = TileSize::Medium;
        tCal.col = 0; tCal.row = 4;
        tCal.isLive = true;

        Tile tEdge;
        tEdge.name = L"Microsoft Edge";
        tEdge.size = TileSize::Medium;
        tEdge.col = 0; tEdge.row = 6;

        Tile tWeather;
        tWeather.name = L"Meteo";
        tWeather.size = TileSize::Medium;
        tWeather.col = 2; tWeather.row = 0;

        // A default tile is added only when the app exists in the installed
        // catalog (AppsFolder or Start shortcuts): no fake or unlaunchable tiles.
        // The right medium column: a medium tile, a pair of small tiles and
        // two more medium tiles, as on the leaked RT Update 3 menu.
        Tile tSol; tSol.name = L"Solitaire"; tSol.size = TileSize::Small; tSol.col = 2; tSol.row = 2;
        Tile tCalc; tCalc.name = L"Calcolatrice"; tCalc.size = TileSize::Small; tCalc.col = 3; tCalc.row = 2;
        Tile tMusic; tMusic.name = L"Groove Musica"; tMusic.size = TileSize::Medium; tMusic.col = 2; tMusic.row = 4;
        Tile tVideo; tVideo.name = L"Film e TV"; tVideo.size = TileSize::Medium; tVideo.col = 2; tVideo.row = 6;

        if (AttachAppDetails(tMail, catalog)) g1.tiles.push_back(tMail);
        if (AttachAppDetails(tCal, catalog)) g1.tiles.push_back(tCal);
        if (AttachAppDetails(tEdge, catalog)) g1.tiles.push_back(tEdge);
        if (AttachAppDetails(tPhotos, catalog)) g1.tiles.push_back(tPhotos);
        if (AttachAppDetails(tXbox, catalog)) g1.tiles.push_back(tXbox);
        if (AttachAppDetails(tWeather, catalog)) g1.tiles.push_back(tWeather);
        if (AttachAppDetails(tMusic, catalog)) g1.tiles.push_back(tMusic);
        if (AttachAppDetails(tVideo, catalog)) g1.tiles.push_back(tVideo);
        if (AttachAppDetails(tSol, catalog)) g1.tiles.push_back(tSol);
        if (AttachAppDetails(tCalc, catalog)) g1.tiles.push_back(tCalc);

        if (!g1.tiles.empty()) groups.push_back(std::move(g1));
    }

    bool AttachAppDetails(Tile& t, const AppCatalog& cat) {
        // Prefer exact name/AUMID matches, then known localized/legacy names and
        // package identity tokens. Tile captions are localized independently of AppsFolder.
        const AppEntry* app = cat.FindApp(t.name);
        if (!app && !t.aumid.empty()) app = cat.FindApp(t.aumid);
        if (!app) {
            struct AliasSet { const wchar_t* tile; const wchar_t* aliases[8]; };
            static const AliasSet sets[] = {
                {L"Posta", {L"Mail", L"Outlook", L"Microsoft.OutlookForWindows", nullptr}},
                {L"Calendario", {L"Calendar", L"Outlook Calendar", L"WindowsCommunicationsApps", nullptr}},
                {L"Microsoft Edge", {L"Edge", L"MSEdge", L"MicrosoftEdge", nullptr}},
                {L"Foto", {L"Photos", L"Microsoft.Windows.Photos", L"Microsoft Photos", nullptr}},
                {L"Meteo", {L"Weather", L"MSN Weather", L"BingWeather", nullptr}},
                {L"Xbox", {L"Xbox", L"XboxApp", L"Gaming", nullptr}},
                {L"Groove Musica", {L"Groove Music", L"ZuneMusic", L"Media Player", nullptr}},
                {L"Film e TV", {L"Movies & TV", L"Movies & Television", L"ZuneVideo", nullptr}},
                {L"Store", {L"Microsoft Store", L"WindowsStore", L"WinStore", nullptr}},
                {L"Solitaire", {L"Microsoft Solitaire Collection", L"SolitaireCollection", nullptr}},
                {L"Calcolatrice", {L"Calculator", L"WindowsCalculator", nullptr}},
            };
            const AliasSet* aliasSet = nullptr;
            for (const auto& set : sets) if (_wcsicmp(set.tile, t.name.c_str()) == 0) { aliasSet = &set; break; }
            if (aliasSet) {
                // First perform case-insensitive exact aliases; then package-token matching.
                for (const wchar_t* alias : aliasSet->aliases) {
                    if (!alias) break;
                    if ((app = cat.FindApp(alias))) break;
                    std::wstring needle(alias);
                    std::transform(needle.begin(), needle.end(), needle.begin(), ::towlower);
                    for (const auto& candidate : cat.Apps()) {
                        std::wstring identity = candidate.name + L" " + candidate.aumid + L" " + candidate.path;
                        std::transform(identity.begin(), identity.end(), identity.begin(), ::towlower);
                        if (identity.find(needle) != std::wstring::npos) { app = &candidate; break; }
                    }
                    if (app) break;
                }
            }
        }
        if (app && !IsUsableCatalogApp(*app)) app = nullptr;
        if (app) {
            t.path = app->path;
            t.aumid = app->aumid;
            t.hIcon = app->hIcon;
            t.appColor = GetIconAccentColor(t.hIcon);
            t.hasFlatIconBackground = XboxIconBackground(t.name,t.aumid,t.hIcon,t.flatIconBackground);
            t.available = true;
            return true;
        }
        // Keep saved custom tiles rather than deleting them after an app refresh.
        // They are rendered with a fallback icon and are not launched until a
        // later refresh resolves the AUMID/path again.
        t.hIcon = nullptr; // the old HICON may belong to the catalog being replaced
        t.hasFlatIconBackground = false;
        t.available = IsUsableTileFileTarget(t.path);
        return t.available;
    }

    // Refresh launchability/icons without deleting persisted groups or tiles.
    // Missing targets remain visible and editable, but cannot be launched until
    // a later catalog refresh resolves them again.
    void RefreshInstalledApps(const AppCatalog& cat) {
        for (auto& group : groups) {
            for (auto& tile : group.tiles)
                AttachAppDetails(tile, cat);
        }
    }

    static void AppendNumber(std::wstring& out, size_t value, wchar_t terminator) {
        out += std::to_wstring(value);
        out.push_back(terminator);
    }

    static void AppendField(std::wstring& out, const std::wstring& value) {
        AppendNumber(out, value.size(), L':');
        out += value;
    }

    static bool ReadNumber(const std::wstring& input, size_t& pos, wchar_t terminator,
                           size_t maximum, size_t& value) {
        if (pos >= input.size()) return false;
        size_t parsed = 0;
        bool haveDigit = false;
        while (pos < input.size() && input[pos] >= L'0' && input[pos] <= L'9') {
            const size_t digit = static_cast<size_t>(input[pos] - L'0');
            if (digit > maximum || parsed > (maximum - digit) / 10) return false;
            parsed = parsed * 10 + digit;
            ++pos;
            haveDigit = true;
        }
        if (!haveDigit || pos >= input.size() || input[pos] != terminator) return false;
        ++pos;
        value = parsed;
        return true;
    }

    static bool ReadField(const std::wstring& input, size_t& pos, std::wstring& value) {
        size_t length = 0;
        if (!ReadNumber(input, pos, L':', 32768, length) || length > input.size() - pos)
            return false;
        value.assign(input, pos, length);
        pos += length;
        return true;
    }

    static bool ParseBoundedInt(const std::wstring& text, int maximum, int& value) {
        if (text.empty()) return false;
        size_t parsed = 0;
        for (wchar_t ch : text) {
            if (ch < L'0' || ch > L'9') return false;
            const size_t digit = static_cast<size_t>(ch - L'0');
            if (digit > static_cast<size_t>(maximum) ||
                parsed > (static_cast<size_t>(maximum) - digit) / 10) return false;
            parsed = parsed * 10 + digit;
        }
        value = static_cast<int>(parsed);
        return true;
    }

    bool ParseV2(const std::wstring& str, const AppCatalog& cat) {
        static constexpr wchar_t kHeader[] = L"WHSM2#";
        constexpr size_t kHeaderLength = ARRAYSIZE(kHeader) - 1;
        if (str.compare(0, kHeaderLength, kHeader) != 0 || str.size() > 16 * 1024 * 1024)
            return false;

        size_t pos = kHeaderLength;
        size_t groupCount = 0;
        if (!ReadNumber(str, pos, L'#', 128, groupCount)) return false;
        std::vector<TileGroup> newGroups;
        newGroups.reserve(groupCount);
        size_t totalTiles = 0;

        for (size_t gi = 0; gi < groupCount; ++gi) {
            TileGroup group;
            if (!ReadField(str, pos, group.title)) return false;
            size_t tileCount = 0;
            if (!ReadNumber(str, pos, L'#', 4096, tileCount) ||
                tileCount > 8192 - totalTiles) return false;
            totalTiles += tileCount;
            group.tiles.reserve(tileCount);

            for (size_t ti = 0; ti < tileCount; ++ti) {
                std::wstring fields[10];
                for (auto& field : fields) {
                    if (!ReadField(str, pos, field)) return false;
                }

                int size = 0, col = 0, row = 0, live = 0;
                if (!ParseBoundedInt(fields[3], 3, size) ||
                    !ParseBoundedInt(fields[4], Metrics::BASE_COLUMNS_PER_GROUP - 1, col) ||
                    !ParseBoundedInt(fields[5], 100000, row) ||
                    !ParseBoundedInt(fields[6], 1, live)) return false;

                Tile tile;
                tile.name = std::move(fields[0]);
                tile.aumid = std::move(fields[1]);
                tile.path = std::move(fields[2]);
                tile.size = static_cast<TileSize>(size);
                tile.col = col;
                tile.row = row;
                tile.isLive = live != 0;
                tile.liveTitle = std::move(fields[7]);
                tile.liveSubtitle = std::move(fields[8]);
                tile.liveDetail = std::move(fields[9]);
                tile.available = AttachAppDetails(tile, cat);

                // Retain saved custom tiles even if their app is temporarily
                // absent. They remain removable/editable and are not launched
                // until a later catalog refresh resolves their target.
                if (!tile.name.empty() || !tile.path.empty() || !tile.aumid.empty())
                    group.tiles.push_back(std::move(tile));
            }
            newGroups.push_back(std::move(group));
        }
        if (pos != str.size()) return false;
        groups = std::move(newGroups); // an explicitly empty v2 layout is valid
        return true;
    }

    bool ParseLegacy(const std::wstring& str, const AppCatalog& cat) {
        try {
            std::vector<TileGroup> newGroups;
            size_t start = 0;
            while (start < str.size()) {
                size_t endGroup = str.find(L"||", start);
                if (endGroup == std::wstring::npos) endGroup = str.size();
                const std::wstring groupChunk = str.substr(start, endGroup - start);
                start = endGroup == str.size() ? str.size() : endGroup + 2;

                const size_t sep = groupChunk.find(L"@@");
                if (sep == std::wstring::npos) continue;
                TileGroup group;
                group.title = groupChunk.substr(0, sep);
                const std::wstring tilesChunk = groupChunk.substr(sep + 2);
                size_t tileStart = 0;
                while (tileStart < tilesChunk.size()) {
                    size_t tileEnd = tilesChunk.find(L";", tileStart);
                    if (tileEnd == std::wstring::npos) tileEnd = tilesChunk.size();
                    const std::wstring tileStr = tilesChunk.substr(tileStart, tileEnd - tileStart);
                    tileStart = tileEnd == tilesChunk.size() ? tilesChunk.size() : tileEnd + 1;

                    // The historical format has ten comma-separated fields.
                    // Split only the first nine separators so an empty final
                    // field remains present (the old parser silently lost it).
                    std::vector<std::wstring> fields;
                    size_t fieldStart = 0;
                    for (int field = 0; field < 9; ++field) {
                        const size_t comma = tileStr.find(L",", fieldStart);
                        if (comma == std::wstring::npos) break;
                        fields.push_back(tileStr.substr(fieldStart, comma - fieldStart));
                        fieldStart = comma + 1;
                    }
                    if (fields.size() == 9) fields.push_back(tileStr.substr(fieldStart));
                    if (fields.size() != 10) continue;

                    int size = 0, col = 0, row = 0, live = 0;
                    if (!ParseBoundedInt(fields[3], 3, size) ||
                        !ParseBoundedInt(fields[4], Metrics::BASE_COLUMNS_PER_GROUP - 1, col) ||
                        !ParseBoundedInt(fields[5], 100000, row) ||
                        !ParseBoundedInt(fields[6], 1, live)) continue;

                    Tile tile;
                    tile.name = std::move(fields[0]);
                    tile.aumid = std::move(fields[1]);
                    tile.path = std::move(fields[2]);
                    tile.size = static_cast<TileSize>(size);
                    tile.col = col;
                    tile.row = row;
                    tile.isLive = live != 0;
                    tile.liveTitle = std::move(fields[7]);
                    tile.liveSubtitle = std::move(fields[8]);
                    tile.liveDetail = std::move(fields[9]);
                    tile.available = AttachAppDetails(tile, cat);
                    if (!tile.name.empty() || !tile.path.empty() || !tile.aumid.empty())
                        group.tiles.push_back(std::move(tile));
                }
                if (!group.tiles.empty()) newGroups.push_back(std::move(group));
            }
            if (newGroups.empty()) return false;
            groups = std::move(newGroups);
            return true;
        } catch (...) {
            return false;
        }
    }

    std::wstring Serialize() const {
        // Length-prefixed fields avoid delimiter collisions in paths, names,
        // localized text, or custom tile content. The header also represents
        // an intentionally empty layout without falling back to defaults.
        std::wstring out = L"WHSM2#";
        AppendNumber(out, groups.size(), L'#');
        for (const auto& group : groups) {
            AppendField(out, group.title);
            AppendNumber(out, group.tiles.size(), L'#');
            for (const auto& tile : group.tiles) {
                AppendField(out, tile.name);
                AppendField(out, tile.aumid);
                AppendField(out, tile.path);
                AppendField(out, std::to_wstring(static_cast<int>(tile.size)));
                AppendField(out, std::to_wstring(tile.col));
                AppendField(out, std::to_wstring(tile.row));
                AppendField(out, tile.isLive ? L"1" : L"0");
                AppendField(out, tile.liveTitle);
                AppendField(out, tile.liveSubtitle);
                AppendField(out, tile.liveDetail);
            }
        }
        return out;
    }

    bool Deserialize(const std::wstring& str, const AppCatalog& cat) {
        if (str.empty()) return false;
        if (str.rfind(L"WHSM2#", 0) == 0) return ParseV2(str, cat);
        return ParseLegacy(str, cat);
    }

};

// ===================== ANCHORING AND POSITIONING =====================
struct StartAnchor {
    RECT startButton{};
    UINT edge = ABE_BOTTOM;
    HMONITOR monitor = nullptr;
    int method = 0; // 1=Win32 "Start" child, 2=UI Automation, 3=geometric fallback
    RECT taskbarRect{};
    HWND taskbarHwnd = nullptr;
    RECT workArea{};
};

static bool IsTaskbarClassName(const wchar_t* cls) {
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

static bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) return false;
    wchar_t cls[64]{};
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0 && IsTaskbarClassName(cls);
}

// Secondary taskbars are not reported by ABM_GETTASKBARPOS: derive the edge from
// the taskbar rectangle relative to its monitor.
static UINT DeriveTaskbarEdge(const RECT& taskbar, const RECT& monitor) {
    const LONG w = taskbar.right - taskbar.left;
    const LONG h = taskbar.bottom - taskbar.top;
    if (w >= h) return (taskbar.top - monitor.top) <= (monitor.bottom - taskbar.bottom) ? ABE_TOP : ABE_BOTTOM;
    return (taskbar.left - monitor.left) <= (monitor.right - taskbar.right) ? ABE_LEFT : ABE_RIGHT;
}

// Locates the Start button of `taskbar` (primary taskbar when null). Called on
// the UI thread only; may use UI Automation on Windows 11.
static bool GetStartAnchor(StartAnchor& out, HWND taskbar = nullptr) {
    HWND hTray = taskbar ? taskbar : FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray && !IsWindow(hTray)) hTray = nullptr;
    out.taskbarHwnd = hTray;
    if (!hTray) {
        POINT pt{ 0, 0 };
        out.monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(out.monitor, &mi);
        out.workArea = mi.rcWork;
        out.edge = ABE_BOTTOM;
        out.taskbarRect = { mi.rcMonitor.left, mi.rcWork.bottom, mi.rcMonitor.right, mi.rcMonitor.bottom };
        out.startButton = { mi.rcMonitor.left, mi.rcWork.bottom, mi.rcMonitor.left + 48, mi.rcMonitor.bottom };
        out.method = 3;
        return false;
    }

    // 1. Taskbar rectangle and edge
    GetWindowRect(hTray, &out.taskbarRect);
    wchar_t cls[64]{};
    GetClassNameW(hTray, cls, ARRAYSIZE(cls));
    APPBARDATA abd{ sizeof(abd) };
    abd.hWnd = hTray;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 && SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        out.edge = abd.uEdge;
        out.taskbarRect = abd.rc;
    } else {
        HMONITOR mon = MonitorFromWindow(hTray, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{ sizeof(mi) };
        GetMonitorInfoW(mon, &mi);
        out.edge = DeriveTaskbarEdge(out.taskbarRect, mi.rcMonitor);
    }

    // 2. Start button rectangle
    bool foundButton = false;

    // Method A: Win32 child window (Windows 10 / classic taskbar)
    HWND hStart = FindWindowExW(hTray, nullptr, L"Start", nullptr);
    if (!hStart) {
        HWND hChild = nullptr;
        while ((hChild = FindWindowExW(hTray, hChild, nullptr, nullptr)) != nullptr) {
            wchar_t childCls[64] = {};
            GetClassNameW(hChild, childCls, ARRAYSIZE(childCls));
            if (_wcsicmp(childCls, L"Start") == 0 || _wcsicmp(childCls, L"Button") == 0) {
                hStart = hChild;
                break;
            }
        }
    }
    if (hStart && IsWindowVisible(hStart)) {
        RECT rc{};
        if (GetWindowRect(hStart, &rc) && (rc.right > rc.left) && (rc.bottom > rc.top)) {
            out.startButton = rc; out.method = 1;
            foundButton = true;
        }
    }

    // Method B: UI Automation (Windows 11 XAML taskbar)
    if (!foundButton) {
        ComPtr<IUIAutomation> uia;
        if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia))) && uia) {
            ComPtr<IUIAutomationElement> trayElem;
            if (SUCCEEDED(uia->ElementFromHandle(hTray, &trayElem)) && trayElem) {
                VARIANT varId;
                varId.vt = VT_BSTR;
                varId.bstrVal = SysAllocString(L"StartButton");
                ComPtr<IUIAutomationCondition> cond;
                if (SUCCEEDED(uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, varId, &cond)) && cond) {
                    ComPtr<IUIAutomationElement> btnElem;
                    if (SUCCEEDED(trayElem->FindFirst(TreeScope_Descendants, cond.Get(), &btnElem)) && btnElem) {
                        RECT uiaRc{};
                        if (SUCCEEDED(btnElem->get_CurrentBoundingRectangle(&uiaRc)) && (uiaRc.right > uiaRc.left)) {
                            out.startButton = uiaRc; out.method = 2;
                            foundButton = true;
                        }
                    }
                }
                SysFreeString(varId.bstrVal);
            }
        }
    }

    // Method C: geometric fallback (square at the taskbar's leading end)
    if (!foundButton) {
        out.method = 3;
        switch (out.edge) {
            case ABE_BOTTOM:
            case ABE_TOP:
                out.startButton.left = out.taskbarRect.left;
                out.startButton.top = out.taskbarRect.top;
                out.startButton.right = out.taskbarRect.left + (out.taskbarRect.bottom - out.taskbarRect.top);
                out.startButton.bottom = out.taskbarRect.bottom;
                break;
            case ABE_LEFT:
            case ABE_RIGHT:
                out.startButton.left = out.taskbarRect.left;
                out.startButton.top = out.taskbarRect.top;
                out.startButton.right = out.taskbarRect.right;
                out.startButton.bottom = out.taskbarRect.top + (out.taskbarRect.right - out.taskbarRect.left);
                break;
        }
    }

    // Monitor and work area
    out.monitor = MonitorFromRect(&out.startButton, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{ sizeof(mi) };
    GetMonitorInfoW(out.monitor, &mi);
    out.workArea = mi.rcWork;
    return true;
}

// Effective DPI of the monitor the menu will be shown on. Explorer keeps the
// system DPI it started with, so GetDeviceCaps(LOGPIXELSX) must not be used.
static UINT GetAnchorDpi(const StartAnchor& anchor) {
    UINT dpiX = 0, dpiY = 0;
    if (anchor.monitor && SUCCEEDED(GetDpiForMonitor(anchor.monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX > 0)
        return dpiX;
    HDC screen = GetDC(nullptr);
    UINT dpi = 96;
    if (screen) {
        const int logical = GetDeviceCaps(screen, LOGPIXELSX);
        if (logical > 0) dpi = static_cast<UINT>(logical);
        ReleaseDC(nullptr, screen);
    }
    return dpi;
}

static POINT ComputeMenuOrigin(const StartAnchor& anchor, SIZE menuSize) {
    POINT pt{};
    switch (anchor.edge) {
        case ABE_BOTTOM:
            pt.x = anchor.startButton.left;
            pt.y = anchor.taskbarRect.top - menuSize.cy;
            break;
        case ABE_TOP:
            pt.x = anchor.startButton.left;
            pt.y = anchor.taskbarRect.bottom;
            break;
        case ABE_LEFT:
            pt.x = anchor.taskbarRect.right;
            pt.y = anchor.startButton.top;
            break;
        case ABE_RIGHT:
            pt.x = anchor.taskbarRect.left - menuSize.cx;
            pt.y = anchor.startButton.top;
            break;
    }

    // Clamp the origin to the monitor work area
    if (pt.x + menuSize.cx > anchor.workArea.right) {
        pt.x = anchor.workArea.right - menuSize.cx;
    }
    if (pt.x < anchor.workArea.left) {
        pt.x = anchor.workArea.left;
    }
    if (pt.y + menuSize.cy > anchor.workArea.bottom) {
        pt.y = anchor.workArea.bottom - menuSize.cy;
    }
    if (pt.y < anchor.workArea.top) {
        pt.y = anchor.workArea.top;
    }

    return pt;
}

// Items of the context menus (owner-drawn HMENU, see ImmersiveStartMenu).
struct MetroPopupItem { std::wstring text; UINT command; bool separator=false; bool heading=false; };

// ===================== START BUTTON CACHE (shared with the hook thread) =====================
// The low-level mouse hook must decide synchronously, without COM/UIA, whether
// a click lands on a Start button. The UI thread computes the rectangles of the
// Start buttons of every taskbar and publishes them here. They are refreshed
// when the mouse enters a taskbar, when the menu is shown, and on display/work
// area changes (no periodic polling).
struct CachedStartButton {
    HWND taskbar = nullptr;
    RECT taskbarRect{};
    RECT startRect{};
    bool reliable = false; // false when the geometric fallback was used
};
static SRWLOCK g_startCacheLock = SRWLOCK_INIT;
static std::vector<CachedStartButton> g_startCache; // guarded by g_startCacheLock
static std::atomic<ULONGLONG> g_startCacheTick{ 0 };

static void RefreshStartRects(bool logIt);

// ===================== APP LAUNCH AND COMMANDS =====================
static bool LaunchApplication(const std::wstring& path, const std::wstring& aumid) {
    if (!aumid.empty()) {
        ComPtr<IApplicationActivationManager> aam;
        HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&aam));
        if (SUCCEEDED(hr) && aam) {
            DWORD pid = 0;
            hr = aam->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid);
            if (SUCCEEDED(hr)) return true;
        }
    }
    if (!path.empty()) {
        HINSTANCE hInst = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static bool LaunchAdmin(const std::wstring& path) {
    if (!path.empty()) {
        HINSTANCE hInst = ShellExecuteW(nullptr, L"runas", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static bool OpenExplorerLocation(const std::wstring& path) {
    if (!path.empty()) {
        std::wstring param = L"/select,\"" + path + L"\"";
        HINSTANCE hInst = ShellExecuteW(nullptr, L"open", L"explorer.exe", param.c_str(), nullptr, SW_SHOWNORMAL);
        return ((INT_PTR)hInst > 32);
    }
    return false;
}

static void ExecutePowerAction(int cmd) {
    HANDLE token=nullptr; TOKEN_PRIVILEGES previous{}; DWORD previousSize=sizeof(previous);
    bool restore=false;
    if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&token)){
        TOKEN_PRIVILEGES enabled{}; enabled.PrivilegeCount=1;
        if(LookupPrivilegeValueW(nullptr,L"SeShutdownPrivilege",&enabled.Privileges[0].Luid)){
            enabled.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED; SetLastError(ERROR_SUCCESS);
            if(AdjustTokenPrivileges(token,FALSE,&enabled,sizeof(previous),&previous,&previousSize)&&GetLastError()!=ERROR_NOT_ALL_ASSIGNED)restore=true;
        }
    }
    if(cmd==IDM_POWER_SLEEP||cmd==IDM_POWER_HIBERNATE){
        if(!SetSuspendState(cmd==IDM_POWER_HIBERNATE,FALSE,FALSE))Wh_Log(L"[Power] SetSuspendState failed (error=%u)",GetLastError());
    }else if(cmd==IDM_POWER_SHUTDOWN||cmd==IDM_POWER_RESTART){
        UINT flags=cmd==IDM_POWER_RESTART?EWX_REBOOT:EWX_SHUTDOWN;
        if(!ExitWindowsEx(flags,SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED))Wh_Log(L"[Power] ExitWindowsEx failed (error=%u)",GetLastError());
    }
    if(token){if(restore&&previousSize)AdjustTokenPrivileges(token,FALSE,&previous,0,nullptr,nullptr);CloseHandle(token);}
}

// Which power entries make sense on this machine (like the native menu).
// IsPwrSuspendAllowed/IsPwrHibernateAllowed (powrprof) already account for the
// supported S-states, modern standby and the presence of the hibernation file.
struct PowerMenuAvailability { bool sleep = true; bool hibernate = true; };
static PowerMenuAvailability QueryPowerMenuAvailability() {
    PowerMenuAvailability result;
    result.sleep = IsPwrSuspendAllowed() != FALSE;
    result.hibernate = IsPwrHibernateAllowed() != FALSE;
    return result;
}

// ===================== START MENU WINDOW =====================
// Owner-drawn opaque context-menu port, structurally following the supplied
// ImmersiveMenu (LoadApi/OpenMenuTheme/Begin/End/Prepare/Measure/Draw/OwnerProc/Track).
// Unlike the Start window, these popup menus are not layered and are always
// light, like the immersive Windows 8.1 menus the reference implementation draws: a #F2F2F2
// background, black Segoe UI text and an #E6E6E6 highlight.
namespace ImmersiveStartMenu {
struct Style {
    bool use=false, accentHot=false, rt3Context=false;
    COLORREF accent=RGB(0,119,234);
};
} // namespace ImmersiveStartMenu

struct DelMenu { void operator()(HMENU h) const { if(h) DestroyMenu(h); } };
using UniqueMenu = UniqueRes<HMENU,DelMenu>;


// Popup window engine copied from win81-start-screen-recreation 1.3.0
// (GPL-3.0-or-later). The adapter below translates this mod's HMENU entries and
// commands into the reference engine. UI state stays on the owner UI thread.
namespace StartScreenPopup {
enum MenuEndReason { MENU_END_NONE=0, MENU_END_INSIDE, MENU_END_OUTSIDE, MENU_END_CANCEL };
struct UserMenuEntry { std::wstring text; int cmd; bool checked=false; bool arrow=false; bool sep=false; int sub=0; };
static const wchar_t* kUserMenuClass = L"Win81Update3StartScreenPopup";
static HWND g_userMenuWnd=nullptr, g_subMenuWnd=nullptr, g_wnd=nullptr;
// Read by the low-level mouse-hook thread. Ordinary popup state stays on UI thread.
static std::atomic<HWND> g_hookRootPopup{nullptr}, g_hookSubPopup{nullptr};
static bool PointInPopupForMouseHook(POINT pt) {
    for (HWND h : {g_hookRootPopup.load(),g_hookSubPopup.load()}) {
        DWORD pid=0; RECT rc{};
        if (h && IsWindowVisible(h) && GetWindowThreadProcessId(h,&pid) &&
            pid==GetCurrentProcessId() && GetWindowRect(h,&rc) && PtInRect(&rc,pt)) return true;
    }
    return false;
}
static HINSTANCE g_inst=nullptr;
static bool g_userMenuClassRegistered=false, g_inUserMenu=false;
static std::vector<UserMenuEntry> g_userMenu, g_subMenu;
static std::vector<std::vector<UserMenuEntry>> g_submenus;
static int g_userMenuHover=-1, g_subMenuHover=-1, g_subParentRow=-1;
static const UINT_PTR kSubOpenTimer=0xA1, kSubCloseTimer=0xA2;
static bool g_swallowPickUp=false, g_swallowPickRightUp=false;
static DWORD g_swallowPickTick=0;
static int g_menuPressIdx=-1, g_menuPressBtn=0, g_userMenuResult=0, g_userMenuEndReason=MENU_END_NONE;
static const int UM_NONE=0;
static constexpr COLORREF kMenuFrame=RGB(0,0,0);
static UINT g_dpi=96;
static int ScI(int v) { return MulDiv(v, static_cast<int>(g_dpi), 96); }
static DWORD NowMs() { return GetTickCount(); }
static COLORREF SystemAccentColor() {
    DWORD c=0; BOOL o=FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c,&o)))
        return RGB((c>>16)&255,(c>>8)&255,c&255);
    return RGB(0,116,203);
}
class ScopedWindowDc {
    HWND w_; HDC dc_;
public:
    explicit ScopedWindowDc(HWND w):w_(w),dc_(GetDC(w)) {}
    ~ScopedWindowDc() { if(dc_) ReleaseDC(w_,dc_); }
    HDC get() const { return dc_; }
};
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
        UniqueFont font(CreateFontW(-ScI(15), 0, 0, 0, FW_NORMAL, 0, 0, 0,
                                       DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI"));
        ScopedSelectObject selFont(dc.get(), font.get());
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
    if (g_subMenuWnd) { HWND s2 = g_subMenuWnd; g_hookSubPopup.store(nullptr); g_subMenuWnd = NULL; DestroyWindow(s2); }
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
    g_hookSubPopup.store(sub);
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

static void DismissForOwner(HWND owner) {
    if (!g_inUserMenu || !g_userMenuWnd || g_wnd != owner) return;
    HWND root = g_userMenuWnd;
    g_userMenuEndReason = MENU_END_CANCEL;
    g_userMenuResult = UM_NONE;
    if (g_subMenuWnd && IsWindow(g_subMenuWnd)) ShowWindow(g_subMenuWnd, SW_HIDE);
    if (IsWindow(root)) ShowWindow(root, SW_HIDE);
    MenuFinish(root, MENU_END_CANCEL, -1);
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

    UniqueBrush bg(CreateSolidBrush(RGB(255, 255, 255)));
    FillRect(dc.get(), &rc, bg.get());

    // The context menus of Windows 8.1 are outlined with a 1 px black frame (kMenuFrame), not with
    // the light grey one that was here.
    UniquePen border(CreatePen(PS_SOLID, 1, kMenuFrame));
    {
        ScopedSelectObject selPen(dc.get(), border.get());
        ScopedSelectObject selBrush(dc.get(), GetStockObject(NULL_BRUSH));
        Rectangle(dc.get(), rc.left, rc.top, rc.right, rc.bottom);
    }

    // The context menus of Windows 8.1 are written in Segoe UI Semibold, the weight of the menu
    // of the reference capture, not in the regular face.
    UniqueFont font(CreateFontW(-ScI(15), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0,
                                   DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Semibold"));
    ScopedSelectObject selFont(dc.get(), font.get());
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
            UniqueBrush lb(CreateSolidBrush(RGB(214, 214, 214)));
            FillRect(dc.get(), &lr, lb.get());
            continue;
        }
        const bool hover = ((int)i == hoverIdx) ||
            (entries[i].sub != 0 && h == g_userMenuWnd && g_subParentRow == (int)i && g_subMenuWnd);
        if (hover) {
            UniqueBrush hb(CreateSolidBrush(accent));
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
    try { return [&]() -> LRESULT {
        switch (msg) {
        case WM_PAINT:
            MenuPaint(h);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE:
            MenuSetHover(h, MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp)));
            return 0;
        // As for the native menus (TrackPopupMenu), a command is chosen on the button release, not on
        // the press, and the menu captures the mouse (SetCapture) until then: the release is
        // delivered to the menu even if the pointer is over something else, so it can never reach
        // the Start window and click what is under the cursor (an app of the search results...).
        // The swallow flags stay set only as a safety net, for the case in which the capture is lost
        // before the release arrives; they expire after 800 ms.
        case WM_LBUTTONDOWN: {
            int idx = MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp));
            if (idx >= 0) {
                if (h == g_userMenuWnd && g_userMenu[idx].sub != 0) OpenSubMenu(idx);
                else {
                    g_menuPressIdx = idx; g_menuPressBtn = 1;
                    g_swallowPickUp = true; g_swallowPickTick = NowMs();
                    SetCapture(h);
                }
            }
            return 0;
        }
        case WM_LBUTTONUP:
            if (g_menuPressBtn == 1) {
                const int idx = g_menuPressIdx;
                g_menuPressIdx = -1; g_menuPressBtn = 0;
                g_swallowPickUp = false;
                if (GetCapture() == h) ReleaseCapture();
                const int now = MenuHitTest(h, (short)LOWORD(lp), (short)HIWORD(lp));
                MenuFinish(h, MENU_END_INSIDE, now == idx ? idx : -1);
            }
            return 0;
        case WM_RBUTTONDOWN:
            g_menuPressIdx = -1; g_menuPressBtn = 2;
            g_swallowPickRightUp = true; g_swallowPickTick = NowMs();
            SetCapture(h);
            return 0;
        case WM_RBUTTONUP:
            if (g_menuPressBtn == 2) {
                g_menuPressBtn = 0;
                g_swallowPickRightUp = false;
                if (GetCapture() == h) ReleaseCapture();
                MenuFinish(h, MENU_END_INSIDE, -1);
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_menuPressIdx = -1; g_menuPressBtn = 0;
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
            if (g_userMenuWnd == h) { g_hookRootPopup.store(nullptr); g_userMenuWnd = NULL; }
            if (g_subMenuWnd == h) { g_hookSubPopup.store(nullptr); g_subMenuWnd = NULL; g_subParentRow = -1; g_subMenuHover = -1; }
            return 0;
        }
        return DefWindowProcW(h, msg, wp, lp);
    }(); } catch (...) { Wh_Log(L"[StartScreenPopup] exception in window procedure"); return 0; }
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
    return CreateWindowExW(ex, kUserMenuClass, L"", WS_POPUP, x, y, w, hgt,
                           owner, NULL, g_inst, NULL);
}

// Translate the host's model, not the reference app's global tile structures.
static void BuildMenuEntries(int kind, std::vector<UserMenuEntry>& out) {
    out.clear();
    if (kind>0 && static_cast<size_t>(kind)<=g_submenus.size()) out=g_submenus[kind-1];
}
static void ReadItems(HMENU menu, std::vector<UserMenuEntry>& out, int depth=0) {
    if (!menu || depth>1) return;
    for (int i=0, count=GetMenuItemCount(menu); i<count; ++i) {
        wchar_t text[256]{}; MENUITEMINFOW info{}; info.cbSize=sizeof(info);
        info.fMask=MIIM_FTYPE|MIIM_STATE|MIIM_ID|MIIM_SUBMENU|MIIM_STRING;
        info.dwTypeData=text; info.cch=static_cast<UINT>(_countof(text));
        if (!GetMenuItemInfoW(menu,i,TRUE,&info)) continue;
        UserMenuEntry entry; entry.sep=(info.fType&MFT_SEPARATOR)!=0;
        if (!entry.sep && (info.fState&(MFS_DISABLED|MFS_GRAYED))) continue;
        entry.text=text; entry.cmd=static_cast<int>(info.wID); entry.checked=(info.fState&MFS_CHECKED)!=0;
        if (info.hSubMenu && depth==0) {
            std::vector<UserMenuEntry> child; ReadItems(info.hSubMenu,child,depth+1);
            if (!child.empty()) { g_submenus.push_back(std::move(child)); entry.sub=static_cast<int>(g_submenus.size()); entry.arrow=true; }
            else continue;
        }
        out.push_back(std::move(entry));
    }
}
struct Result { UINT command=0; int reason=MENU_END_CANCEL; };
static Result Track(HMENU menu, HWND owner, POINT pt, UINT dpi, HINSTANCE instance) {
    Result result;
    if (!menu || !owner || g_inUserMenu || GetWindowThreadProcessId(owner,nullptr)!=GetCurrentThreadId()) return result;
    g_inst=instance; g_wnd=owner; g_dpi=(dpi>=96 && dpi<=480)?dpi:96;
    g_submenus.clear(); g_userMenu.clear(); ReadItems(menu,g_userMenu);
    if (g_userMenu.empty() || !EnsureUserMenuClass()) return result;
    int width=MenuWidth(), height=MenuContentH(g_userMenu), x=pt.x, y=pt.y;
    PlaceUserMenu(x,y,width,height);
    g_inUserMenu=true; g_userMenuHover=-1; g_userMenuResult=UM_NONE; g_userMenuEndReason=MENU_END_NONE;
    g_menuPressIdx=-1; g_menuPressBtn=0; g_swallowPickUp=false; g_swallowPickRightUp=false;
    HWND popup=CreateUserMenuWindow(owner,x,y,width,height);
    if (!popup) { g_inUserMenu=false; UnregisterUserMenuClass(); return result; }
    g_userMenuWnd=popup;
    g_hookRootPopup.store(popup);
    SetWindowPos(popup,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
    SetForegroundWindow(popup); SetFocus(popup);
    MSG msg{};
    while (g_userMenuWnd) {
        BOOL r=GetMessageW(&msg,nullptr,0,0);
        if (r==0) { PostQuitMessage(static_cast<int>(msg.wParam)); break; }
        if (r<0) break;
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
    if (g_userMenuWnd) { HWND h=g_userMenuWnd; g_hookRootPopup.store(nullptr); g_userMenuWnd=nullptr; DestroyWindow(h); }
    CloseSubMenu(); g_inUserMenu=false;
    result.command=static_cast<UINT>(g_userMenuResult);
    result.reason=g_userMenuEndReason==MENU_END_NONE ? MENU_END_CANCEL : g_userMenuEndReason;
    g_userMenu.clear(); g_subMenu.clear(); g_submenus.clear();
    UnregisterUserMenuClass();
    if (result.reason==MENU_END_INSIDE && IsWindow(owner) && IsWindowVisible(owner)) {
        SetForegroundWindow(owner); SetFocus(owner);
    }
    return result;
}
} // namespace StartScreenPopup


enum class ViewMode { Normal, AllApps, Search };

enum class UiText : size_t {
    Explorer, Settings, Power, AllApps, Back, MostUsed, RecentlyAdded, SearchPrefix, SearchPlaceholder,
    SearchEmpty, CatalogLoading,
    DefaultUser, GroupApps, GroupLeisure, Pinned, Unpin, Resize, Small, Medium, Wide,
    Large, EnableLive, DisableLive, RunAsAdmin, OpenLocation, Open, PinToStart,
    EditAccount, Lock, SignOut, Sleep, Shutdown, Restart, Hibernate,
    TileMail, TileCalendar, TilePhotos, TileWeather, TileGrooveMusic, TileMoviesTV,
    TileCalculator, Count
};

// Translation columns follow the requested order: Italian, English, Spanish, French,
// Persian, Arabic, Portuguese, Simplified Chinese, Japanese, Polish, Russian.
static int GetUiLanguageIndex() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case 0x10: return 0; // Italian
        case 0x09: return 1; // English
        case 0x0A: return 2; // Spanish
        case 0x0C: return 3; // French
        case 0x29: return 4; // Persian
        case 0x01: return 5; // Arabic
        case 0x16: return 6; // Portuguese
        case 0x04: return 7; // Chinese
        case 0x11: return 8; // Japanese
        case 0x15: return 9; // Polish
        case 0x19: return 10; // Russian
        default: return 1;
    }
}

static const wchar_t* Ui(UiText text) {
    static const wchar_t* const strings[][11] = {
        {L"Esplora file",L"File Explorer",L"Explorador de archivos",L"Explorateur de fichiers",L"\u0641\u0627\u06CC\u0644 \u0627\u06A9\u0633\u067E\u0644\u0648\u0631",L"\u0645\u0633\u062A\u0643\u0634\u0641 \u0627\u0644\u0645\u0644\u0641\u0627\u062A",L"Explorador de Arquivos",L"\u6587\u4EF6\u8D44\u6E90\u7BA1\u7406\u5668",L"\u30D5\u30A1\u30A4\u30EB \u30A8\u30AF\u30B9\u30D7\u30ED\u30FC\u30E9\u30FC",L"Eksplorator plik\u00F3w",L"\u041F\u0440\u043E\u0432\u043E\u0434\u043D\u0438\u043A"},
        {L"Impostazioni",L"Settings",L"Configuraci\u00F3n",L"Param\u00E8tres",L"\u062A\u0646\u0638\u06CC\u0645\u0627\u062A",L"\u0627\u0644\u0625\u0639\u062F\u0627\u062F\u0627\u062A",L"Configura\u00E7\u00F5es",L"\u8BBE\u7F6E",L"\u8A2D\u5B9A",L"Ustawienia",L"\u041F\u0430\u0440\u0430\u043C\u0435\u0442\u0440\u044B"},
        {L"Alimentazione",L"Power",L"Inicio/apagado",L"Marche/arr\u00EAt",L"\u062A\u0648\u0627\u0646",L"\u0627\u0644\u0637\u0627\u0642\u0629",L"Ligar/desligar",L"\u7535\u6E90",L"\u96FB\u6E90",L"Zasilanie",L"\u041F\u0438\u0442\u0430\u043D\u0438\u0435"},
        {L"Tutte le app",L"All apps",L"Todas las aplicaciones",L"Toutes les applications",L"\u0647\u0645\u0647 \u0628\u0631\u0646\u0627\u0645\u0647\u200C\u0647\u0627",L"\u0643\u0644 \u0627\u0644\u062A\u0637\u0628\u064A\u0642\u0627\u062A",L"Todas as aplica\u00E7\u00F5es",L"\u6240\u6709\u5E94\u7528",L"\u3059\u3079\u3066\u306E\u30A2\u30D7\u30EA",L"Wszystkie aplikacje",L"Все приложения"},
        {L"Indietro",L"Back",L"Atr\u00E1s",L"Retour",L"\u0628\u0627\u0632\u06AF\u0634\u062A",L"\u0631\u062C\u0648\u0639",L"Voltar",L"\u8FD4\u56DE",L"\u623B\u308B",L"Wstecz",L"\u041D\u0430\u0437\u0430\u0434"},
        {L"Pi\u00F9 usate",L"Most used",L"M\u00E1s usadas",L"Les plus utilis\u00E9es",L"\u067E\u0631\u06A9\u0627\u0631\u0628\u0631\u062F\u062A\u0631\u06CC\u0646",L"\u0627\u0644\u0623\u0643\u062B\u0631 \u0627\u0633\u062A\u062E\u062F\u0627\u0645\u064B\u0627",L"Mais usadas",L"\u6700\u5E38\u7528",L"\u3088\u304F\u4F7F\u3046\u30A2\u30D7\u30EA",L"Najcz\u0119\u015Bciej u\u017Cywane",L"\u0427\u0430\u0441\u0442\u043E \u0438\u0441\u043F\u043E\u043B\u044C\u0437\u0443\u0435\u043C\u044B\u0435"},
        {L"Aggiunte di recente",L"Recently added",L"Agregadas recientemente",L"R\u00E9cemment ajout\u00E9es",L"\u0627\u0641\u0632\u0648\u062F\u0647\u200C\u0634\u062F\u0647\u200C\u0647\u0627\u06CC \u0627\u062E\u06CC\u0631",L"\u0627\u0644\u0645\u0636\u0627\u0641\u0629 \u0645\u0624\u062E\u0631\u064B\u0627",L"Adicionadas recentemente",L"\u6700\u8FD1\u6DFB\u52A0",L"\u6700\u8FD1\u8FFD\u52A0\u3057\u305F\u30A2\u30D7\u30EA",L"Ostatnio dodane",L"\u041D\u0435\u0434\u0430\u0432\u043D\u043E \u0434\u043E\u0431\u0430\u0432\u043B\u0435\u043D\u043D\u044B\u0435"},
        {L"Ricerca: ",L"Search: ",L"Buscar: ",L"Rechercher : ",L"\u062C\u0633\u062A\u062C\u0648: ",L"\u0628\u062D\u062B: ",L"Pesquisar: ",L"\u641C\u7D22: ",L"\u691C\u7D22: ",L"Szukaj: ",L"\u041F\u043E\u0438\u0441\u043A: "},
        {L"Cerca ovunque",L"Search everywhere",L"Buscar en todas partes",L"Rechercher partout",L"جستجو در همه جا",L"البحث في كل مكان",L"Pesquisar em todo o lado",L"搜索所有位置",L"すべての場所を検索",L"Wyszukaj wszędzie",L"Поиск везде"},
        {L"Nessuna app corrisponde alla ricerca.",L"No apps match your search.",L"Ninguna aplicaci\u00F3n coincide con la b\u00FAsqueda.",L"Aucune application ne correspond \u00E0 votre recherche.",L"\u0647\u06CC\u062C \u0628\u0631\u0646\u0627\u0645\u0647\u200C\u0627\u06CC \u0645\u0637\u0627\u0628\u0642 \u062C\u0633\u062A\u062C\u0648 \u0646\u062F\u0627\u0631\u062F.",L"\u0644\u0627 \u064A\u0648\u062C\u062F \u062A\u0637\u0628\u064A\u0642\u0627\u062A \u062A\u0637\u0627\u0628\u0642 \u0627\u0644\u0628\u062D\u062B.",L"Nenhuma aplica\u00E7\u00E3o corresponde \u00E0 pesquisa.",L"\u6CA1\u6709\u5E94\u7528\u5339\u914D\u60A8\u7684\u641C\u7D22\u3002",L"\u4E00\u81F4\u3059\u308B\u30A2\u30D7\u30EA\u306F\u3042\u308A\u307E\u305B\u3093\u3002",L"\u017Badna aplikacja nie pasuje do wyszukiwania.",L"\u041D\u0438 \u043E\u0434\u043D\u043E \u043F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u0435 \u043D\u0435 \u0441\u043E\u043E\u0442\u0432\u0435\u0442\u0441\u0442\u0432\u0443\u0435\u0442 \u043F\u043E\u0438\u0441\u043A\u0443."},
        {L"Caricamento app…",L"Loading apps…",L"Cargando aplicaciones…",L"Chargement des applications…",L"در حال بارگیری برنامه\u200Cها…",L"جارٍ تحميل التطبيقات…",L"A carregar aplicações…",L"正在加载应用…",L"アプリを読み込んでいます…",L"Ładowanie aplikacji…",L"Загрузка приложений…"},
        {L"Utente",L"User",L"Usuario",L"Utilisateur",L"\u06A9\u0627\u0631\u0628\u0631",L"\u0645\u0633\u062A\u062E\u062F\u0645",L"Usu\u00E1rio",L"\u7528\u6237",L"\u30E6\u30FC\u30B6\u30FC",L"U\u017Cytkownik",L"\u041F\u043E\u043B\u044C\u0437\u043E\u0432\u0430\u0442\u0435\u043B\u044C"},
        {L"Applicazioni",L"Apps",L"Aplicaciones",L"Applications",L"\u0628\u0631\u0646\u0627\u0645\u0647\u200C\u0647\u0627",L"\u0627\u0644\u062A\u0637\u0628\u064A\u0642\u0627\u062A",L"Aplica\u00E7\u00F5es",L"\u5E94\u7528",L"\u30A2\u30D7\u30EA",L"Aplikacje",L"\u041F\u0440\u0438\u043B\u043E\u0436\u0435\u043D\u0438\u044F"},
        {L"Tempo libero",L"Play and explore",L"Jugar y explorar",L"Divertissement",L"\u0633\u0631\u06AF\u0631\u0645\u06CC \u0648 \u06A9\u0634\u0641",L"\u0627\u0644\u0644\u0639\u0628 \u0648\u0627\u0644\u0627\u0633\u062A\u0643\u0634\u0627\u0641",L"Divers\u00E3o e descoberta",L"\u5A31\u4E50\u4E0E\u63A2\u7D22",L"\u904A\u3073\u3068\u63A2\u7D22",L"Rozrywka i odkrywanie",L"\u0418\u0433\u0440\u044B \u0438 \u043E\u0442\u043A\u0440\u044B\u0442\u0438\u044F"},
        {L"Fissati",L"Pinned",L"Ancladas",L"\u00C9pingl\u00E9es",L"\u0633\u0646\u062C\u0627\u0642\u200C\u0634\u062F\u0647",L"\u0645\u062B\u0628\u062A",L"Fixados",L"\u5DF2\u56FA\u5B9A",L"\u30D4\u30F3\u7559\u3081\u6E08\u307F",L"Przypi\u0119te",L"Закрепленные"},
        {L"Rimuovi da Start",L"Unpin from Start",L"Desanclar de Inicio",L"D\u00E9tacher de l\u2019\u00E9cran de d\u00E9marrage",L"\u0628\u0631\u062F\u0627\u0634\u062A\u0646 \u0627\u0632 \u0645\u0646\u0648\u06CC \u0634\u0631\u0648\u0639",L"\u0625\u0644\u063A\u0627\u0621 \u0627\u0644\u062A\u062B\u0628\u064A\u062A \u0645\u0646 \u0627\u0628\u062F\u0623",L"Desafixar do Iniciar",L"\u4ECE\u201C\u5F00\u59CB\u201D\u53D6\u6D88\u56FA\u5B9A",L"\u30B9\u30BF\u30FC\u30C8\u304B\u3089\u30D4\u30F3\u7559\u3081\u3092\u5916\u3059",L"Odepnij od menu Start",L"Открепить от начального экрана"},
        {L"Ridimensiona",L"Resize",L"Cambiar tama\u00F1o",L"Redimensionner",L"\u062A\u063A\u06CC\u06CC\u0631 \u0627\u0646\u062F\u0627\u0632\u0647",L"\u062A\u063A\u064A\u064A\u0631 \u0627\u0644\u062D\u062C\u0645",L"Redimensionar",L"\u8C03\u6574\u5927\u5C0F",L"\u30B5\u30A4\u30BA\u5909\u66F4",L"Zmie\u0144 rozmiar",L"Изменить размер"},
        {L"Piccola",L"Small",L"Peque\u00F1a",L"Petite",L"\u06A9\u0648\u0686\u06A9",L"\u0635\u063A\u064A\u0631",L"Pequena",L"\u5C0F",L"\u5C0F\u3055\u3044",L"Ma\u0142y",L"Маленькая"},
        {L"Media",L"Medium",L"Mediana",L"Moyenne",L"\u0645\u062A\u0648\u0633\u0637",L"\u0645\u062A\u0648\u0633\u0637",L"M\u00E9dia",L"\u4E2D",L"\u4E2D",L"\u015Aredni",L"Средняя"},
        {L"Larga",L"Wide",L"Ancha",L"Large",L"\u0639\u0631\u06CC\u0636",L"\u0639\u0631\u064A\u0636",L"Larga",L"\u5BBD",L"\u5E45\u5E83",L"Szeroki",L"Широкая"},
        {L"Grande",L"Large",L"Grande",L"Grande",L"\u0628\u0632\u0631\u06AF",L"\u0643\u0628\u064A\u0631",L"Grande",L"\u5927",L"\u5927\u304D\u3044",L"Du\u017Cy",L"Большая"},
        {L"Attiva riquadro animato",L"Turn live tile on",L"Activar icono din\u00E1mico",L"Activer la vignette dynamique",L"\u0641\u0639\u0627\u0644\u200C\u0633\u0627\u0632\u06CC \u067E\u0627\u06CC\u0647 \u0632\u0646\u062F\u0647",L"\u062A\u0641\u0639\u064A\u0644 \u0627\u0644\u0628\u0644\u0627\u0637\u0629 \u0627\u0644\u062D\u064A\u0629",L"Ativar bloco din\u00E2mico",L"\u542F\u7528\u52A8\u6001\u78C1\u8D34",L"\u30E9\u30A4\u30D6\u30BF\u30A4\u30EB\u3092\u6709\u52B9\u306B\u3059\u308B",L"W\u0142\u0105cz kafelek dynamiczny",L"Включить живую плитку"},
        {L"Disattiva riquadro animato",L"Turn live tile off",L"Desactivar icono din\u00E1mico",L"D\u00E9sactiver la vignette dynamique",L"\u063A\u06CC\u0631\u0641\u0639\u0627\u0644\u200C\u0633\u0627\u0632\u06CC \u067E\u0627\u06CC\u0647 \u0632\u0646\u062F\u0647",L"\u062A\u0639\u0637\u064A\u0644 \u0627\u0644\u0628\u0644\u0627\u0637\u0629 \u0627\u0644\u062D\u064A\u0629",L"Desativar bloco din\u00E2mico",L"\u5173\u95ED\u52A8\u6001\u78C1\u8D34",L"\u30E9\u30A4\u30D6\u30BF\u30A4\u30EB\u3092\u7121\u52B9\u306B\u3059\u308B",L"Wy\u0142\u0105cz kafelek dynamiczny",L"Выключить живую плитку"},
        {L"Esegui come amministratore",L"Run as administrator",L"Ejecutar como administrador",L"Ex\u00E9cuter en tant qu\u2019administrateur",L"\u0627\u062C\u0631\u0627 \u0628\u0647\u200C\u0639\u0646\u0648\u0627\u0646 \u0645\u062F\u06CC\u0631",L"\u062A\u0634\u063A\u064A\u0644 \u0643\u0645\u0633\u0624\u0648\u0644",L"Executar como administrador",L"\u4EE5\u7BA1\u7406\u5458\u8EAB\u4EFD\u8FD0\u884C",L"\u7BA1\u7406\u8005\u3068\u3057\u3066\u5B9F\u884C",L"Uruchom jako administrator",L"Запуск от имени администратора"},
        {L"Apri percorso file",L"Open file location",L"Abrir ubicaci\u00F3n del archivo",L"Ouvrir l\u2019emplacement du fichier",L"\u06AF\u0634\u0648\u062F\u0646 \u0645\u062D\u0644 \u0641\u0627\u06CC\u0644",L"\u0641\u062A\u062D \u0645\u0648\u0642\u0639 \u0627\u0644\u0645\u0644\u0641",L"Abrir local do ficheiro",L"\u6253\u5F00\u6587\u4EF6\u4F4D\u7F6E",L"\u30D5\u30A1\u30A4\u30EB\u306E\u5834\u6240\u3092\u958B\u304F",L"Otw\u00F3rz lokalizacj\u0119 pliku",L"Открыть расположение файла"},
        {L"Apri",L"Open",L"Abrir",L"Ouvrir",L"\u06AF\u0634\u0648\u062F\u0646",L"\u0641\u062A\u062D",L"Abrir",L"\u6253\u5F00",L"\u958B\u304F",L"Otw\u00F3rz",L"Открыть"},
        {L"Aggiungi a Start",L"Pin to Start",L"Anclar a Inicio",L"\u00C9pingler \u00E0 l\u2019\u00E9cran de d\u00E9marrage",L"\u0633\u0646\u062C\u0627\u0642\u200C\u06A9\u0631\u062F\u0646 \u0628\u0647 \u0645\u0646\u0648\u06CC \u0634\u0631\u0648\u0639",L"\u062A\u062B\u0628\u064A\u062A \u0641\u064A \u0627\u0628\u062F\u0623",L"Afixar ao Iniciar",L"\u56FA\u5B9A\u5230\u201C\u5F00\u59CB\u201D",L"\u30B9\u30BF\u30FC\u30C8\u306B\u30D4\u30F3\u7559\u3081",L"Przypnij do menu Start",L"Закрепить на начальном экране"},
        {L"Modifica impostazioni account",L"Change account settings",L"Cambiar configuración de la cuenta",L"Modifier les paramètres du compte",L"\u062A\u063A\u06CC\u06CC\u0631 \u062A\u0646\u0638\u06CC\u0645\u0627\u062A \u062D\u0633\u0627\u0628",L"\u062A\u063A\u064A\u064A\u0631 \u0625\u0639\u062F\u0627\u062F\u0627\u062A \u0627\u0644\u062D\u0633\u0627\u0628",L"Alterar definições da conta",L"\u66F4\u6539\u8D26\u6237\u8BBE\u7F6E",L"\u30A2\u30AB\u30A6\u30F3\u30C8\u8A2D\u5B9A\u306E\u5909\u66F4",L"Zmie\u0144 ustawienia konta",L"Изменить параметры учетной записи"},
        {L"Blocca",L"Lock",L"Bloquear",L"Verrouiller",L"\u0642\u0641\u0644",L"\u0642\u0641\u0644",L"Bloquear",L"\u9501\u5B9A",L"\u30ED\u30C3\u30AF",L"Zablokuj",L"Заблокировать"},
        {L"Disconnetti",L"Sign out",L"Cerrar sesión",L"Se déconnecter",L"\u062E\u0631\u0648\u062C",L"\u062A\u0633\u062C\u064A\u0644 \u0627\u0644\u062E\u0631\u0648\u062C",L"Terminar sessão",L"\u6CE8\u9500",L"\u30B5\u30A4\u30F3\u30A2\u30A6\u30C8",L"Wyloguj",L"Выйти"},
        {L"Sospendi",L"Sleep",L"Suspender",L"Mettre en veille",L"\u0627\u0633\u062A\u0646\u0628\u0627\u062A",L"\u0627\u0633\u062A\u0639\u062F\u0627\u062F",L"Suspender",L"\u7761\u7720",L"\u30B9\u30EA\u30FC\u30D7",L"U\u015Bpij",L"Спящий режим"},
        {L"Arresta il sistema",L"Shut down",L"Apagar",L"Arrêter",L"\u062E\u0627\u0645\u0648\u0634 \u06A9\u0631\u062F\u0646",L"\u0625\u064A\u0642\u0627\u0641 \u0627\u0644\u062A\u0634\u063A\u064A\u0644",L"Encerrar",L"\u5173\u673A",L"\u30B7\u30E3\u30C3\u30C8\u30C0\u30A6\u30F3",L"Zamknij system",L"Завершение работы"},
        {L"Riavvia il sistema",L"Restart",L"Reiniciar",L"Redémarrer",L"\u0628\u0627\u0632\u0646\u0634\u0627\u0646\u06CC",L"\u0625\u0639\u0627\u062F\u0629 \u0627\u0644\u062A\u0634\u063A\u064A\u0644",L"Reiniciar",L"\u91CD\u542F",L"\u518D\u8D77\u52D5",L"Uruchom ponownie",L"Перезагрузка"},
        {L"Iberna",L"Hibernate",L"Hibernar",L"Mettre en veille prolongée",L"\u0632\u0645\u0633\u062A\u0627\u0646\u06CC",L"\u0625\u0633\u0628\u0627\u062A",L"Hibernar",L"\u4F11\u7720",L"\u4F11\u6B62\u72B6\u614B",L"Hibernuj",L"Гибернация"},
        {L"Posta",L"Mail",L"Correo",L"Courrier",L"\u067E\u0633\u062A",L"\u0627\u0644\u0628\u0631\u064A\u062F",L"Correio",L"\u90AE\u4EF6",L"\u30E1\u30FC\u30EB",L"Poczta",L"Почта"},
        {L"Calendario",L"Calendar",L"Calendario",L"Calendrier",L"تقویم",L"\u0627\u0644\u062A\u0642\u0648\u064A\u0645",L"Calendário",L"\u65E5\u5386",L"\u30AB\u30EC\u30F3\u30C0\u30FC",L"Kalendarz",L"Календарь"},
        {L"Foto",L"Photos",L"Fotos",L"Photos",L"\u0639\u06A9\u0633",L"\u0627\u0644\u0635\u0648\u0631",L"Fotografias",L"\u7167\u7247",L"\u5199\u771F",L"Zdjęcia",L"Фотографии"},
        {L"Meteo",L"Weather",L"El tiempo",L"Météo",L"\u0622\u0628\u200C\u0648\u0647\u0648\u0627",L"\u0627\u0644\u0637\u0642\u0633",L"Meteorologia",L"\u5929\u6C14",L"\u5929\u6C17",L"Pogoda",L"Погода"},
        {L"Groove Musica",L"Groove Music",L"Groove Música",L"Groove Musique",L"Groove Music",L"Groove Music",L"Groove Música",L"Groove \u97F3\u4E50",L"Groove \u30DF\u30E5\u30FC\u30B8\u30C3\u30AF",L"Groove Muzyka",L"Groove Музыка"},
        {L"Film e TV",L"Movies & TV",L"Películas y TV",L"Films et TV",L"\u0641\u06CC\u0644\u0645 \u0648 \u062A\u0644\u0648\u06CC\u0632\u06CC\u0648\u0646",L"\u0627\u0644\u0623\u0641\u0644\u0627\u0645 \u0648\u0627\u0644\u062A\u0644\u0641\u0632\u064A\u0648\u0646",L"Filmes e TV",L"\u7535\u5F71\u548C\u7535\u89C6",L"\u6620\u753B\u3068\u30C6\u30EC\u30D3",L"Filmy i TV",L"Фильмы и ТВ"},
        {L"Calcolatrice",L"Calculator",L"Calculadora",L"Calculatrice",L"\u062D\u0627\u0633\u0628",L"\u0627\u0644\u062D\u0627\u0633\u0628\u0629",L"Calculadora",L"\u8BA1\u7B97\u5668",L"\u96FB\u5353",L"Kalkulator",L"Калькулятор"},
    };
    const size_t index = static_cast<size_t>(text);
    return index < _countof(strings) ? strings[index][GetUiLanguageIndex()] : L"";
}

static const wchar_t* TranslateTileCaption(const std::wstring& caption) {
    if (_wcsicmp(caption.c_str(), L"Posta") == 0) return Ui(UiText::TileMail);
    if (_wcsicmp(caption.c_str(), L"Calendario") == 0) return Ui(UiText::TileCalendar);
    if (_wcsicmp(caption.c_str(), L"Foto") == 0) return Ui(UiText::TilePhotos);
    if (_wcsicmp(caption.c_str(), L"Meteo") == 0) return Ui(UiText::TileWeather);
    if (_wcsicmp(caption.c_str(), L"Groove Musica") == 0) return Ui(UiText::TileGrooveMusic);
    if (_wcsicmp(caption.c_str(), L"Film e TV") == 0) return Ui(UiText::TileMoviesTV);
    if (_wcsicmp(caption.c_str(), L"Calcolatrice") == 0) return Ui(UiText::TileCalculator);
    return caption.c_str();
}


// File and Settings catalog ported from Windows 8.1 Start Screen Recreation
// (babamohammed2022, GPL-3.0-or-later). Original enumeration, XML parser,
// matching data and launch routines remain in this namespace; publication and
// menu integration are adapted for this Start menu.
namespace StartScreenSearch {
enum CharmCat { CC_APP = 0, CC_SETTING = 1, CC_FILE = 2 };
struct AppEntry { std::wstring name, lower, parse; bool exe, desktop;
                  unsigned long long date; int cat;
                  std::wstring catName;
                  int charmCat = CC_APP;
                  bool dir = false;
                  bool searchOnly = false;
                  bool direct = false;
                  std::wstring keywords;   // lower-case search keywords (settings entries)
                  std::string pidl;        // absolute PIDL bytes (Control Panel items)
                  int launchKind = 0;      // 0 = ShellExecute(AppTarget); see enum SettingLaunch
                  std::wstring launch; };

static HANDLE g_appsStop = nullptr;
[[clang::no_destroy]] static std::mutex g_lock;
[[clang::no_destroy]] static std::vector<AppEntry> g_pending;
static HANDLE g_worker = nullptr;
struct SearchIconPixels { int w=0, h=0; std::vector<BYTE> pixels; };
struct IconRequest { size_t index; AppEntry entry; };
struct IconReply { size_t index; SearchIconPixels image; };
[[clang::no_destroy]] static std::vector<IconRequest> g_iconRequests;
[[clang::no_destroy]] static std::vector<IconReply> g_iconReplies;
static HANDLE g_iconWake=nullptr, g_iconWorker=nullptr;

static std::wstring LowerW(const std::wstring& s) {
    std::wstring r=s; if(!r.empty()) CharLowerBuffW(&r[0], static_cast<DWORD>(r.size())); return r;
}
static void ScanCharmDir(std::vector<AppEntry>& out, const std::wstring& dir, int depth, size_t cap) {
    if (depth > 8 || out.size() >= cap) return;
    if (g_appsStop && WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0) return;
    WIN32_FIND_DATAW fd = {};
    HANDLE hf = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) return;
    size_t visited=0;
    do {
        // Yield disk and CPU periodically, with an interruptible wait on shutdown.
        if ((++visited % 256)==0 && g_appsStop &&
            WaitForSingleObject(g_appsStop, 10)==WAIT_OBJECT_0) break;
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

enum SettingLaunch { SL_NONE = 0, SL_PIDL = 1, SL_SHELLEXEC = 2, SL_PROCESS = 3, SL_ACTIVATE = 4 };

static const PROPERTYKEY kPkeyKeywords =
    { { 0xF29F85E0, 0x4FF9, 0x1068, { 0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9 } }, 5 };
static const GUID kBhidEnumItems =
    { 0x94F60519, 0x2850, 0x4924, { 0xAA, 0x5A, 0xD1, 0x5E, 0x84, 0x86, 0x80, 0x39 } };
static const CLSID kClsidActivationManager =
    { 0x45BA127D, 0x10A8, 0x46EA, { 0x8A, 0xB7, 0x56, 0xEA, 0x90, 0x78, 0x94, 0x3C } };
static const wchar_t kSettingsAumid[] =
    L"windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel";

static bool AppsStopRequested() {
    return g_appsStop && WaitForSingleObject(g_appsStop, 0) == WAIT_OBJECT_0;
}

static std::wstring ExpandEnvW(const std::wstring& s) {
    wchar_t buf[2048];
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), buf, (DWORD)(sizeof(buf) / sizeof(buf[0])));
    if (n == 0 || n > sizeof(buf) / sizeof(buf[0])) return s;
    return buf;
}

// ---- tiny tolerant XML reader (the settings file is machine generated and well formed) ----
static std::wstring ReadTextFileW(const std::wstring& path) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return L"";
    LARGE_INTEGER sz = {};
    if (!GetFileSizeEx(f, &sz) || sz.QuadPart < 4 || sz.QuadPart > 64LL * 1024 * 1024) { CloseHandle(f); return L""; }
    std::string bytes((size_t)sz.QuadPart, '\0');
    DWORD got = 0;
    const BOOL ok = ReadFile(f, &bytes[0], (DWORD)bytes.size(), &got, NULL);
    CloseHandle(f);
    if (!ok || got < 4) return L"";
    bytes.resize(got);
    const BYTE* b = (const BYTE*)bytes.data();
    if (b[0] == 0xFF && b[1] == 0xFE)
        return std::wstring((const wchar_t*)(bytes.data() + 2), (bytes.size() - 2) / 2);
    const size_t off = (bytes.size() >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) ? 3 : 0;
    const int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data() + off, (int)(bytes.size() - off), NULL, 0);
    if (n <= 0) return L"";
    std::wstring w((size_t)n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data() + off, (int)(bytes.size() - off), &w[0], n);
    return w;
}

static std::wstring XmlDecode(const std::wstring& s) {
    if (s.find(L'&') == std::wstring::npos) return s;
    static const struct { const wchar_t* ent; wchar_t ch; } named[] = {
        { L"&amp;", L'&' }, { L"&lt;", L'<' }, { L"&gt;", L'>' }, { L"&quot;", L'"' }, { L"&apos;", L'\'' } };
    std::wstring r;
    r.reserve(s.size());
    for (size_t i = 0; i < s.size();) {
        if (s[i] == L'&') {
            bool hit = false;
            for (const auto& e : named) {
                const size_t n = wcslen(e.ent);
                if (s.compare(i, n, e.ent) == 0) { r += e.ch; i += n; hit = true; break; }
            }
            if (hit) continue;
            if (i + 3 < s.size() && s[i + 1] == L'#') {
                const bool hex = (s[i + 2] == L'x' || s[i + 2] == L'X');
                const size_t semi = s.find(L';', i);
                if (semi != std::wstring::npos && semi - i <= 10) {
                    const wchar_t* start = s.c_str() + i + (hex ? 3 : 2);
                    wchar_t* end = NULL;
                    const unsigned long v = wcstoul(start, &end, hex ? 16 : 10);
                    if (end == s.c_str() + semi && v > 0 && v < 0x10000) { r += (wchar_t)v; i = semi + 1; continue; }
                }
            }
        }
        r += s[i++];
    }
    return r;
}

// Finds the first <name ...>content</name> inside x[from, to) and returns the content range.
// The search is confined to the record: a missing tag never scans on into the next records.
static bool XmlRange(const std::wstring& x, size_t from, size_t to, const wchar_t* name,
                     size_t* cs, size_t* ce) {
    if (to > x.size()) to = x.size();
    if (from >= to) return false;
    const std::wstring_view v(x.data() + from, to - from);
    const std::wstring open = std::wstring(L"<") + name;
    const std::wstring close = std::wstring(L"</") + name + L">";
    size_t p = 0;
    while ((p = v.find(std::wstring_view(open), p)) != std::wstring_view::npos) {
        const size_t q = p + open.size();
        if (q < v.size() && (v[q] == L'>' || v[q] == L' ' || v[q] == L'/' || v[q] == L'\t' || v[q] == L'\r' || v[q] == L'\n')) {
            const size_t gt = v.find(L'>', q);
            if (gt == std::wstring_view::npos) return false;
            if (v[gt - 1] == L'/') { *cs = *ce = from + gt; return true; }
            const size_t e = v.find(std::wstring_view(close), gt + 1);
            if (e == std::wstring_view::npos) return false;
            *cs = from + gt + 1; *ce = from + e;
            return true;
        }
        p = q;
    }
    return false;
}

static std::wstring XmlText(const std::wstring& x, size_t from, size_t to, const wchar_t* name) {
    size_t cs = 0, ce = 0;
    if (!XmlRange(x, from, to, name, &cs, &ce)) return L"";
    std::wstring t = x.substr(cs, ce - cs);
    if (t.compare(0, 9, L"<![CDATA[") == 0) {
        const size_t end = t.rfind(L"]]>");
        if (end != std::wstring::npos && end >= 9) return t.substr(9, end - 9);
    }
    return XmlDecode(t);
}

// ---- indirect strings: "@%SystemRoot%\system32\x.dll,-123", "@{PackageFamily?ms-resource://...}" ----
static std::wstring PackageFullNameFromFamily(const std::wstring& family) {
    typedef LONG (WINAPI *GetPackagesByPackageFamily_t)(PCWSTR, UINT32*, PWSTR*, UINT32*, PWSTR);
    static GetPackagesByPackageFamily_t pGet = (GetPackagesByPackageFamily_t)
        GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetPackagesByPackageFamily");
    if (!pGet) return L"";
    UINT32 count = 0, len = 0;
    if (pGet(family.c_str(), &count, NULL, &len, NULL) != ERROR_INSUFFICIENT_BUFFER || count == 0) return L"";
    std::vector<PWSTR> names(count);
    std::vector<wchar_t> buf(len);
    if (pGet(family.c_str(), &count, names.data(), &len, buf.data()) != ERROR_SUCCESS || count == 0) return L"";
    return names[0];
}

static std::wstring IndirectOne(const std::wstring& s) {
    wchar_t buf[1024];
    buf[0] = 0;
    HRESULT hr = SHLoadIndirectString(s.c_str(), buf, 1024, NULL);
    if (hr == E_INVALIDARG && s.size() > 3 && s[0] == L'@' && s[1] == L'{') {
        std::wstring inner = s.substr(2);
        if (!inner.empty() && inner.back() == L'}') inner.pop_back();
        const size_t q = inner.find(L'?');
        if (q != std::wstring::npos) {
            const std::wstring full = PackageFullNameFromFamily(inner.substr(0, q));
            if (!full.empty()) {
                const std::wstring fixed = L"@{" + full + L"?" + inner.substr(q + 1) + L"}";
                buf[0] = 0;
                hr = SHLoadIndirectString(fixed.c_str(), buf, 1024, NULL);
            }
        }
    }
    return SUCCEEDED(hr) ? std::wstring(buf) : std::wstring();
}

static std::wstring TranslateSettingString(const std::wstring& s) {
    if (s.empty() || s[0] != L'@') return s;
    if (s.size() > 1 && s[1] == L'@') {                    // "@@res1@res2": several resources joined
        std::wstring r;
        size_t pos = 1;
        while (pos < s.size()) {
            size_t next = s.find(L'@', pos + 1);
            if (next == std::wstring::npos) next = s.size();
            r += IndirectOne(s.substr(pos, next - pos));
            pos = next;
        }
        return r;
    }
    return IndirectOne(s);
}

static bool SettingsKeywordsOnly(const std::wstring& s) { return s.find_first_not_of(L"; \t\r\n") == std::wstring::npos; }

static std::wstring FindAllSystemSettingsXml() {
    wchar_t win[MAX_PATH] = {};
    if (!GetWindowsDirectoryW(win, MAX_PATH)) return L"";
    const std::wstring dir = std::wstring(win) + L"\\ImmersiveControlPanel\\Settings";
    // Known names first (as Open-Shell does), then any AllSystemSettings_*.xml for newer builds.
    const wchar_t* known[] = { L"AllSystemSettings_{FDB289F3-FCFC-4702-8015-18926E996EC1}.xml",
                               L"AllSystemSettings_{253E530E-387D-4BC2-959D-E6F86122E5F2}.xml" };
    for (const wchar_t* k : known) {
        const std::wstring p = dir + L"\\" + k;
        if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) return p;
    }
    WIN32_FIND_DATAW fd = {};
    HANDLE h = FindFirstFileW((dir + L"\\AllSystemSettings_*.xml").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return L"";
    std::wstring best;
    FILETIME bestTime = {};
    do {
        if (best.empty() || CompareFileTime(&fd.ftLastWriteTime, &bestTime) > 0) {
            best = dir + L"\\" + fd.cFileName;
            bestTime = fd.ftLastWriteTime;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return best;
}

struct ModernSettingRec { std::wstring fileName, description, keywords, parse, launch; int kind; };

static void ParseModernSettings(std::vector<AppEntry>& out) {
    const std::wstring path = FindAllSystemSettingsXml();
    if (path.empty()) { Wh_Log(L"Settings: AllSystemSettings xml not found"); return; }
    const std::wstring x = ReadTextFileW(path);
    if (x.empty()) { Wh_Log(L"Settings: cannot read %s", path.c_str()); return; }

    std::vector<ModernSettingRec> recs;
    size_t p = 0;
    while ((p = x.find(L"<SearchableContent", p)) != std::wstring::npos) {
        if (AppsStopRequested()) return;
        size_t e = x.find(L"</SearchableContent>", p);
        if (e == std::wstring::npos) break;
        const size_t from = p, to = e;
        p = e;

        size_t ics = 0, ice = 0, ccs = 0, cce = 0;
        std::wstring deepLink;
        if (XmlRange(x, from, to, L"ApplicationInformation", &ics, &ice))
            deepLink = TranslateSettingString(XmlText(x, ics, ice, L"DeepLink"));
        const std::wstring pageId    = TranslateSettingString(XmlText(x, from, to, L"PageID"));
        const std::wstring hostId    = TranslateSettingString(XmlText(x, from, to, L"HostID"));
        const std::wstring groupId   = TranslateSettingString(XmlText(x, from, to, L"GroupID"));
        const std::wstring settingId = TranslateSettingString(XmlText(x, from, to, L"SettingID"));

        std::wstring description, keywords;
        if (XmlRange(x, from, to, L"SettingInformation", &ccs, &cce)) {
            description = TranslateSettingString(XmlText(x, ccs, cce, L"Description"));
            if (description.empty()) description = TranslateSettingString(XmlText(x, ccs, cce, L"Name"));
            keywords += TranslateSettingString(XmlText(x, ccs, cce, L"HighKeywords"));
            keywords += L";";
            keywords += TranslateSettingString(XmlText(x, ccs, cce, L"LowKeywords"));
            keywords += L";";
            keywords += TranslateSettingString(XmlText(x, ccs, cce, L"Keywords"));
        }
        if (description.empty()) continue;

        ModernSettingRec r;
        r.fileName = XmlText(x, from, to, L"Filename");
        r.description = description;
        r.keywords = SettingsKeywordsOnly(keywords) ? std::wstring() : keywords;
        r.parse = L"ms-settings:" + pageId;          // only used to pick the Settings icon
        r.kind = SL_NONE;

        // Same dispatch Windows uses for the three kinds of hosts (as in Open-Shell's context menu).
        if (_wcsicmp(hostId.c_str(), L"{6E6DDBCB-9C89-434B-A994-D5F22239523B}") == 0) {          // Windows Security
            if (deepLink.empty()) continue;
            r.launch = L"windowsdefender://" + deepLink; r.kind = SL_SHELLEXEC;
        } else if (_wcsicmp(hostId.c_str(), L"{7E0522FC-1AC4-41CA-AFD0-3610417A9C41}") == 0) {   // shell folder page
            if (pageId.empty()) continue;
            r.launch = L"shell:::" + pageId; r.kind = SL_SHELLEXEC;
        } else if (_wcsicmp(hostId.c_str(), L"{12B1697E-D3A0-4DBC-B568-CCF64A3F934D}") == 0) {   // legacy applet command
            if (deepLink.empty()) continue;
            r.launch = deepLink;
            r.kind = deepLink.compare(0, 8, L"shell:::") == 0 ? SL_SHELLEXEC : SL_PROCESS;
        } else {                                                                                  // SystemSettings page
            if (pageId.empty()) continue;
            r.launch = L"page=" + pageId;
            if (!settingId.empty()) r.launch += L"&target=" + settingId;
            else if (!groupId.empty()) r.launch += L"&group=" + groupId;
            r.launch += L"&ActivationType=Search";
            r.kind = SL_ACTIVATE;
        }
        recs.push_back(std::move(r));
    }

    // Newer pages (e.g. "SomeSetting-2") come first, then one entry per visible name.
    std::stable_sort(recs.begin(), recs.end(), [](const ModernSettingRec& a, const ModernSettingRec& b) {
        return a.fileName > b.fileName; });
    std::unordered_map<std::wstring, bool> seen;
    for (ModernSettingRec& r : recs) {
        const std::wstring key = LowerW(r.description);
        if (!seen.emplace(key, true).second) continue;
        AppEntry a{};
        a.name = r.description; a.lower = key; a.parse = r.parse;
        a.exe = false; a.desktop = false; a.date = 0; a.cat = 2;
        a.charmCat = CC_SETTING; a.searchOnly = true; a.direct = true;
        a.keywords = LowerW(r.keywords);
        a.launchKind = r.kind; a.launch = r.launch;
        out.push_back(std::move(a));
    }
    Wh_Log(L"Settings: %u modern settings from %s", (unsigned)out.size(), path.c_str());
}

// ---- classic Control Panel / Administrative Tools / All Tasks ----
static std::wstring ShellItemKeywords(IShellItem* item) {
    IShellItem2* i2 = NULL;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&i2))) || !i2) return L"";
    PROPVARIANT v;
    PropVariantInit(&v);
    std::wstring r;
    if (SUCCEEDED(i2->GetProperty(kPkeyKeywords, &v))) {
        if ((v.vt == VT_LPWSTR || v.vt == VT_BSTR) && v.pwszVal) {
            r = v.pwszVal;
        } else if (v.vt == (VT_VECTOR | VT_LPWSTR) || v.vt == (VT_VECTOR | VT_BSTR)) {
            for (ULONG k = 0; k < v.calpwstr.cElems; ++k) {
                if (!v.calpwstr.pElems[k]) continue;
                r += L";";
                r += v.calpwstr.pElems[k];
            }
        }
    }
    PropVariantClear(&v);
    i2->Release();
    return LowerW(r);
}

static void AddShellSetting(std::vector<AppEntry>& out, IShellItem* item, bool wantKeywords) {
    PWSTR nm = NULL;
    if (FAILED(item->GetDisplayName(SIGDN_PARENTRELATIVEEDITING, &nm)) || !nm) return;
    std::wstring name = nm;
    CoTaskMemFree(nm);
    if (name.empty()) return;
    PIDLIST_ABSOLUTE pidl = NULL;
    if (FAILED(SHGetIDListFromObject(item, &pidl)) || !pidl) return;
    AppEntry a{};
    a.name = name; a.lower = LowerW(name);
    PWSTR pn = NULL;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pn)) && pn) { a.parse = pn; CoTaskMemFree(pn); }
    else a.parse = name;
    a.exe = false; a.desktop = false; a.date = 0; a.cat = 2;
    a.charmCat = CC_SETTING; a.searchOnly = true; a.direct = true;
    a.launchKind = SL_PIDL;
    a.pidl.assign((const char*)pidl, ILGetSize(pidl));
    CoTaskMemFree(pidl);
    if (wantKeywords) a.keywords = ShellItemKeywords(item);
    out.push_back(std::move(a));
}

static void CollectShellSettings(std::vector<AppEntry>& out, IShellItem* folder, bool withFolders,
                                 bool recursive, bool wantKeywords, int depth) {
    if (depth > 4 || out.size() > 6000) return;
    IEnumShellItems* en = NULL;
    if (FAILED(folder->BindToHandler(NULL, kBhidEnumItems, IID_PPV_ARGS(&en))) || !en) return;
    IShellItem* child = NULL;
    while (!AppsStopRequested() && en->Next(1, &child, NULL) == S_OK && child) {
        SFGAOF fl = 0;
        if (SUCCEEDED(child->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM | SFGAO_LINK | SFGAO_HIDDEN, &fl)) &&
            !(fl & SFGAO_HIDDEN)) {
            if (recursive && (fl & (SFGAO_FOLDER | SFGAO_STREAM | SFGAO_LINK)) == SFGAO_FOLDER)
                CollectShellSettings(out, child, withFolders, recursive, wantKeywords, depth + 1);
            if (withFolders || !(fl & SFGAO_FOLDER))
                AddShellSetting(out, child, wantKeywords);
        }
        child->Release();
        child = NULL;
    }
    en->Release();
}

static IShellItem* ShellItemFromKnownFolder(REFKNOWNFOLDERID id) {
    PIDLIST_ABSOLUTE pidl = NULL;
    if (FAILED(SHGetKnownFolderIDList(id, 0, NULL, &pidl)) || !pidl) return NULL;
    IShellItem* item = NULL;
    if (FAILED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item)))) item = NULL;
    CoTaskMemFree(pidl);
    return item;
}

// The settings do not change within a session: they are built once (BuildSettingsCache, on the
// catalog thread, after the apps are already published) and then only appended to the catalog.
[[clang::no_destroy]] static std::vector<AppEntry> g_settingsCache;   // catalog thread only
static bool g_settingsBuilt = false;

static void BuildSettingsCache() {
    std::vector<AppEntry> found;

    if (IShellItem* cp = ShellItemFromKnownFolder(FOLDERID_ControlPanelFolder)) {
        CollectShellSettings(found, cp, true, false, true, 0);
        cp->Release();
    }
    if (IShellItem* at = ShellItemFromKnownFolder(FOLDERID_AdminTools)) {
        CollectShellSettings(found, at, false, true, false, 0);
        at->Release();
    }
    if (IShellItem* cat = ShellItemFromKnownFolder(FOLDERID_CommonAdminTools)) {
        CollectShellSettings(found, cat, false, true, false, 0);
        cat->Release();
    }
    {
        IShellItem* tasks = NULL;   // "All Tasks" (God Mode): hundreds of tasks with good keywords
        if (SUCCEEDED(SHCreateItemFromParsingName(L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}", NULL,
                                                  IID_PPV_ARGS(&tasks))) && tasks) {
            CollectShellSettings(found, tasks, false, false, true, 0);
            tasks->Release();
        }
    }
    Wh_Log(L"Settings: %u classic Control Panel items", (unsigned)found.size());
    if (!AppsStopRequested()) ParseModernSettings(found);

    // One row per visible name; the classic item wins over the modern page with the same name.
    std::unordered_map<std::wstring, bool> seen;
    g_settingsCache.clear();
    for (AppEntry& a : found) {
        if (!seen.emplace(a.lower, true).second) continue;
        g_settingsCache.push_back(std::move(a));
    }
    g_settingsCache.shrink_to_fit();
}

static void LaunchSettingEntry(const AppEntry& a) {
    switch (a.launchKind) {
    case SL_PIDL: {
        if (a.pidl.empty()) break;
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
        sei.lpIDList = (LPVOID)a.pidl.data();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        break;
    }
    case SL_SHELLEXEC:
        ShellExecuteW(NULL, L"open", a.launch.c_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
    case SL_PROCESS: {
        const std::wstring cmd = ExpandEnvW(a.launch);
        std::vector<wchar_t> buf(cmd.begin(), cmd.end());
        buf.push_back(0);
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        if (CreateProcessW(NULL, buf.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        } else {
            ShellExecuteW(NULL, L"open", cmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    }
    case SL_ACTIVATE: {
        bool ok = false;
        IApplicationActivationManager* mgr = NULL;
        if (SUCCEEDED(CoCreateInstance(kClsidActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&mgr))) && mgr) {
            DWORD pid = 0;
            ok = SUCCEEDED(mgr->ActivateApplication(kSettingsAumid, a.launch.c_str(), AO_NONE, &pid));
            mgr->Release();
        }
        if (!ok) ShellExecuteW(NULL, L"open", a.parse.c_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
    }
    default:
        break;
    }
}


// Pixel extraction follows the Start Screen implementation.
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

static SearchIconPixels LoadSearchIcon(const AppEntry& a) {
    SearchIconPixels image;
    BYTE* px=nullptr; int w=0, h=0;
    if (a.charmCat == CC_FILE) {
        SHFILEINFOW info{};
        const DWORD attr = a.dir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
        if (SHGetFileInfoW(a.parse.c_str(), attr, &info, sizeof(info),
                           SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES) && info.hIcon) {
            IconPixelsFromHICON(info.hIcon, &px, &w, &h);
            DestroyIcon(info.hIcon);
        }
    } else {
        // Classic settings use their own Shell PIDL. Modern Settings pages use
        // the SystemSettings application icon, as in the Start Screen mod.
        std::wstring path=a.parse;
        if (path.rfind(L"ms-settings:",0)==0) {
            wchar_t windowsDir[MAX_PATH]{};
            if (GetWindowsDirectoryW(windowsDir,MAX_PATH))
                path=std::wstring(windowsDir)+L"\\ImmersiveControlPanel\\SystemSettings.exe";
        }
        IShellItem* item=nullptr;
        HRESULT hr = !a.pidl.empty()
            ? SHCreateItemFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(a.pidl.data()),IID_PPV_ARGS(&item))
            : SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(&item));
        if (SUCCEEDED(hr) && item) {
            IShellItemImageFactory* factory=nullptr;
            if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
                SIZE size{96,96}; HBITMAP bm=nullptr;
                if (SUCCEEDED(factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bm)) && bm) {
                    BitmapToPixels(bm,&px,&w,&h); DeleteObject(bm);
                }
                factory->Release();
            }
            item->Release();
        }
    }
    if (px && w>0 && h>0 && w<=512 && h<=512) {
        image.w=w; image.h=h;
        image.pixels.assign(px, px+static_cast<size_t>(w)*h*4);
    }
    delete[] px;
    return image;
}
static DWORD WINAPI IconWorker(void* param) {
    HWND owner=static_cast<HWND>(param); ScopedCom com;
    HANDLE events[2]={g_appsStop,g_iconWake};
    SearchIconPixels modernIcon;
    while (WaitForMultipleObjects(2,events,FALSE,INFINITE)==WAIT_OBJECT_0+1) {
        for (;;) {
            if (AppsStopRequested()) return 0;
            IconRequest request;
            { std::lock_guard<std::mutex> guard(g_lock);
              if (g_iconRequests.empty()) break;
              request=std::move(g_iconRequests.back()); g_iconRequests.pop_back(); }
            SearchIconPixels image;
            try {
                const bool modern=request.entry.charmCat==CC_SETTING &&
                    request.entry.parse.rfind(L"ms-settings:",0)==0 && request.entry.pidl.empty();
                if (modern && !modernIcon.pixels.empty()) image=modernIcon;
                else { image=LoadSearchIcon(request.entry); if (modern && !image.pixels.empty()) modernIcon=image; }
            } catch (...) { Wh_Log(L"[Search] icon extraction failed"); }
            { std::lock_guard<std::mutex> guard(g_lock); g_iconReplies.push_back({request.index,std::move(image)}); }
            PostMessageW(owner,WM_APP_SEARCH_ICON_READY,0,0);
        }
    }
    return 0;
}
static bool QueueIcon(size_t index,const AppEntry& entry) {
    if (!g_iconWorker || !g_iconWake) return false;
    { std::lock_guard<std::mutex> guard(g_lock); g_iconRequests.push_back({index,entry}); }
    SetEvent(g_iconWake);
    return true;
}

static void Publish(HWND owner, std::vector<AppEntry>& list) {
    if (AppsStopRequested() || list.empty()) return;
    { std::lock_guard<std::mutex> guard(g_lock);
      for (auto& entry : list) g_pending.push_back(std::move(entry)); }
    if (!PostMessageW(owner, WM_APP_SEARCH_CATALOG_READY, 0, 0))
        Wh_Log(L"[Search] catalog notification failed: %u", GetLastError());
}
static DWORD WINAPI Worker(void* param) {
    HWND owner=static_cast<HWND>(param);
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
    const ULONGLONG started=GetTickCount64();
    ScopedCom com;
    try {
        BuildSettingsCache();
        Publish(owner,g_settingsCache);
        if (!AppsStopRequested()) { std::vector<AppEntry> files; ScanCharmFiles(files); Publish(owner,files); }
    } catch (...) { Wh_Log(L"[Search] background catalog failed"); }
    Wh_Log(L"[SearchPerf] on-demand catalog elapsed=%llu ms", GetTickCount64()-started);
    return 0;
}
static void Start(HWND owner) {
    if (g_worker || g_appsStop || !owner) return;
    Wh_Log(L"[SearchPerf] starting settings/files on first query");
    g_appsStop=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if (!g_appsStop) return;
    g_iconWake=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if (g_iconWake) g_iconWorker=CreateThread(nullptr,0,IconWorker,owner,0,nullptr);
    g_worker=CreateThread(nullptr,0,Worker,owner,0,nullptr);
    if (!g_worker) {
        SetEvent(g_appsStop);
        if (g_iconWake) SetEvent(g_iconWake);
        if (g_iconWorker) { WaitForSingleObject(g_iconWorker,INFINITE); CloseHandle(g_iconWorker); g_iconWorker=nullptr; }
        if (g_iconWake) { CloseHandle(g_iconWake); g_iconWake=nullptr; }
        CloseHandle(g_appsStop); g_appsStop=nullptr;
        Wh_Log(L"[SearchPerf] worker creation failed; retry on next query");
    }
}
static void Stop() {
    if (g_appsStop) SetEvent(g_appsStop);
    if (g_worker) { WaitForSingleObject(g_worker,INFINITE); CloseHandle(g_worker); g_worker=nullptr; }
    if (g_iconWorker) { WaitForSingleObject(g_iconWorker,INFINITE); CloseHandle(g_iconWorker); g_iconWorker=nullptr; }
    if (g_iconWake) { CloseHandle(g_iconWake); g_iconWake=nullptr; }
    if (g_appsStop) { CloseHandle(g_appsStop); g_appsStop=nullptr; }
    { std::lock_guard<std::mutex> guard(g_lock); g_pending.clear(); g_iconRequests.clear(); g_iconReplies.clear(); }
    g_settingsCache.clear(); g_settingsBuilt=false;
}
} // namespace StartScreenSearch

// The tool process owns the menu. The shell process only runs the bridge.
static bool g_isHybridTool = false;
static constexpr wchar_t kHybridReadyProperty[] = L"Win81Update3StartMenuTool.Ready.v1";
static bool RequestToggle(const wchar_t* source, HWND taskbar = nullptr);

class StartMenuWindow {
public:
    StartMenuWindow() = default;
    ~StartMenuWindow() {
        Destroy();
        if (classRegistered_ && hInst_) {
            UnregisterClassW(kWindowClassName, hInst_);
            classRegistered_ = false;
        }
    }

    static constexpr const wchar_t* kWindowClassName = L"Win81_Update3_StartMenu_WndClass";

    // The class is registered under the mod's own module handle, so a class left
    // behind by a previous instance (same name, different module) is never
    // reused with a dangling WndProc. Registration failure fails Create().
    bool Create(HINSTANCE hInstance) {
        hInst_ = hInstance;
        WNDCLASSW wc{};
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInst_;
        wc.lpszClassName = kWindowClassName;
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        if (!RegisterClassW(&wc)) {
            DWORD error = GetLastError();
            if (error == ERROR_CLASS_ALREADY_EXISTS) {
                // Stale registration from a previous instance of this module
                // (e.g. after a crash): drop it and register the current WndProc.
                UnregisterClassW(kWindowClassName, hInst_);
                if (RegisterClassW(&wc)) error = ERROR_SUCCESS;
                else error = GetLastError();
            }
            if (error != ERROR_SUCCESS) {
                Wh_Log(L"[StartMenu] RegisterClass failed: %u", error);
                return false;
            }
        }
        classRegistered_ = true;

        // Keep Start above ordinary applications. A per-frame window region clips
        // the moving edge so the taskbar remains visually in front during the slide.
        HWND h = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
            wc.lpszClassName,
            L"Windows 8.1 Start Menu",
            WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
            0, 0, 100, 100,
            nullptr, nullptr, hInst_, this
        );

        if (!h) {
            Wh_Log(L"[StartMenu] CreateWindowEx failed: %u", GetLastError());
            return false;
        }

        hwnd_.reset(h);
        if (!ApplyWindowOpacity()) {
            // Do not expose a Start window if its fixed alpha could not be set.
            hwnd_.reset();
            return false;
        }
        InitFonts();
        lastCatalogScanTick_ = GetTickCount64();
        // Shell icon extraction and AppsFolder enumeration can take seconds.
        // Build on a private STA worker and publish only after it is complete.
        catalogThread_.reset(CreateThread(nullptr, 0, InitialCatalogWorker, this, 0, nullptr));
        if (!catalogThread_) {
            Wh_Log(L"[Catalog] worker creation failed; scanning on UI thread");
            catalog_.Refresh();
            catalogReady_ = true;
            lastCatalogScanTick_ = GetTickCount64();
            layout_.LoadDefault(catalog_);
            NormalizeTileLayout();
        }

        const size_t savedSizeChars = Wh_GetStringValue(L"startMenuWindowSize", savedWindowSize_, ARRAYSIZE(savedWindowSize_));
        int savedWidth = 0, savedHeight = 0;
        if (savedSizeChars > 0 && savedSizeChars < ARRAYSIZE(savedWindowSize_) &&
            swscanf_s(savedWindowSize_, L"%d,%d", &savedWidth, &savedHeight) == 2 &&
            savedWidth >= 540 && savedHeight >= 420 && savedWidth <= 5000 && savedHeight <= 5000) {
            savedWidth96_ = savedWidth;
            savedHeight96_ = savedHeight;
            hasUserSized_ = true;
        }

        Wh_Log(L"[StartMenu] Window created successfully (HWND=%p)", hwnd_.get());
        return true;
    }

    void Destroy() {
        // Worker owns no UI resources. Join before releasing its receiver.
        if (catalogThread_) {
            WaitForSingleObject(catalogThread_.get(), INFINITE);
            catalogThread_.reset();
        }
        if (hwnd_) {
            StartScreenSearch::Stop();
            StartScreenPopup::DismissForOwner(hwnd_.get());
            StopAllAppsArrowRepeat(true);
            KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
            KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            KillTimer(hwnd_.get(), TIMER_TILE_ANIM);
            KillTimer(hwnd_.get(), TIMER_SEARCH_CARET);
            KillTimer(hwnd_.get(), TIMER_CATALOG_PUBLISH);
            KillTimer(hwnd_.get(), TIMER_CATALOG_ENRICH);
            tileAnimActive_ = false;
            HWND h = hwnd_.release();
            if (h && IsWindow(h)) {
                DestroyWindow(h);
            }
        }
    }

    HWND GetHwnd() const { return hwnd_.get(); }
    bool IsVisible() const { return visible_; }

    // `taskbar` is the taskbar whose Start button was activated (nullptr = primary).
    void Toggle(HWND taskbar = nullptr) {
        if (visible_) Hide();
        else Show(taskbar);
    }

    void Show(HWND taskbar = nullptr) {
        if (!hwnd_) return;
        GUARDED(L"Show", {
            StartAnchor anchor;
            GetStartAnchor(anchor, taskbar);
            requestedTaskbar_ = anchor.taskbarHwnd;

            // Lay out at the DPI of the monitor the menu is shown on.
            const UINT targetDpi = GetAnchorDpi(anchor);
            if (targetDpi > 0) UpdateDpiMetrics(targetDpi);
            ApplyWindowOpacity();
            InitFonts();

            // The explicit width override wins over a saved drag-resize width;
            // the saved height is kept.
            int menuW = (settings_.widthOverride > 0) ? metrics_.Scale(settings_.widthOverride)
                      : hasUserSized_ ? metrics_.Scale(savedWidth96_)
                      : metrics_.Scale(Metrics::BASE_DEFAULT_MENU_W);
            int menuH = hasUserSized_ ? metrics_.Scale(savedHeight96_)
                                      : metrics_.Scale(Metrics::BASE_DEFAULT_MENU_H);

            // Clamp to the work area without altering tile sizes (which are
            // controlled exclusively by TileSize and the context menu).
            const int maxW = (std::max)(1L, anchor.workArea.right - anchor.workArea.left);
            const int maxH = (std::max)(1L, anchor.workArea.bottom - anchor.workArea.top);
            const int minW = (std::min)(metrics_.Scale(420), maxW);
            const int minH = (std::min)(metrics_.Scale(420), maxH);
            menuW = (std::max)(minW, (std::min)(menuW, maxW));
            menuH = (std::max)(minH, (std::min)(menuH, maxH));

            SIZE sz{ menuW, menuH };
            POINT pt = ComputeMenuOrigin(anchor, sz);

            // Publish fresh Start rectangles for the hook thread, unless the
            // mouse hook just had them refreshed.
            if (GetTickCount64() - g_startCacheTick.load() > 1000) RefreshStartRects(false);
            showTick_ = GetTickCount64();
            Wh_Log(L"[StartMenu] Show: pos=(%ld,%ld) size=%dx%d anchorMethod=%d edge=%u", pt.x, pt.y, menuW, menuH, anchor.method, (unsigned)anchor.edge);
            visible_ = true;
            visibleAnchorEdge_ = anchor.edge;
            visibleTaskbarHwnd_ = anchor.taskbarHwnd;
            visibleTaskbarRect_ = anchor.taskbarRect;
            // The Update 3 menu pops in place instantly: no slide, no fade.
            wideWidthPx_ = menuW;
            programmaticResize_ = true;
            SetWindowPos(hwnd_.get(), HWND_TOPMOST, pt.x, pt.y,
                         menuW, menuH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
            UpdateTaskbarWindowRegion();
            programmaticResize_ = false;
            viewMode_ = ViewMode::Normal;
            searchQuery_.clear();
            searchAllSelected_ = false;
            searchFocused_ = false;
            KillSearchCaretTimer();
            allAppsScrollY_ = 0;
            hoverArea_ = -1;
            hoverIndex_ = -1;
            hoverGroup_ = -1;
            ResetKeyboardNavigation();

            if (settings_.liveTiles) {
                SetTimer(hwnd_.get(), TIMER_LIVE_FLIP, settings_.liveTileIntervalMs, nullptr);
            }

            BOOL fgOk = SetForegroundWindow(hwnd_.get());
            SetFocus(hwnd_.get());
            Wh_Log(L"[StartMenu] SetForegroundWindow=%d, foreground is menu=%d", fgOk ? 1 : 0, GetForegroundWindow() == hwnd_.get() ? 1 : 0);
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        })
    }

    // Blinking caret of the white search box, Win7-style: a slow timer that
    // only runs while the search view is active.
    void EnsureSearchCaretTimer() {
        if (hwnd_) SetTimer(hwnd_.get(), TIMER_SEARCH_CARET, 530, nullptr);
    }

    void KillSearchCaretTimer() {
        if (hwnd_) KillTimer(hwnd_.get(), TIMER_SEARCH_CARET);
    }

    void Hide() {
        if (!visible_ || !hwnd_) return;
        GUARDED(L"Hide", {
            StopAllAppsArrowRepeat(true);
            KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
            KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            KillSearchCaretTimer();
            // A closed menu reopens with the tiles already in place.
            SettleTileAnimations();
            visible_ = false;
            StartScreenPopup::DismissForOwner(hwnd_.get());
            ResetKeyboardNavigation();
            // The Update 3 menu disappears instantly: no slide, no fade.
            ShowWindow(hwnd_.get(), SW_HIDE);
            ClearTaskbarWindowRegion();
            Wh_Log(L"[StartMenu] Hide");
            // Refresh the app list once the menu is out of the way: either a
            // change notification arrived while it was open, or the catalog is
            // old enough that Store apps may have been installed or removed.
            constexpr ULONGLONG kStaleCatalogMs = 15ull * 60 * 1000;
            if (catalogSwapPending_) PostMessageW(hwnd_.get(), WM_APP_INITIAL_CATALOG_READY, 0, 0);
            if (catalogRefreshPending_ || GetTickCount64() - lastCatalogScanTick_ > kStaleCatalogMs)
                PostMessageW(hwnd_.get(), WM_APP_REFRESH, 0, 0);
        })
    }

    void ApplySettings(const Settings& s) {
        settings_ = s;
        if (!catalogReady_) {
            if (hwnd_) ApplyWindowOpacity();
            return; // Do not save an empty layout while the initial scan runs.
        }
        // Apply a layout only on first creation (restore from storage) or when a
        // new layout was pasted into the tileLayout setting. Other settings
        // changes leave the user's current tile arrangement untouched.
        if (!settings_.tileLayout.empty() && (!layoutApplied_ || settings_.tileLayoutImportPending)) {
            if (layout_.Deserialize(settings_.tileLayout, catalog_)) {
                if (settings_.tileLayoutImportPending) {
                    Wh_Log(L"[TileLayout] imported layout from the tileLayout setting");
                    Wh_SetStringValue(L"tileLayoutImported", settings_.tileLayout.c_str());
                }
            } else {
                Wh_Log(L"[TileLayout] stored/imported layout could not be parsed; keeping current layout");
            }
        }
        layoutApplied_ = true;
        NormalizeTileLayout();
        PersistTileLayout();
        if (!settings_.liveTiles) {
            for (auto& group : layout_.groups) {
                for (auto& tile : group.tiles) {
                    tile.flipping = false;
                    tile.flipProgress = 0.0f;
                    tile.showBack = false;
                }
            }
        }
        if (hwnd_) ApplyWindowOpacity();
        if (hwnd_) {
            if (visible_ && settings_.liveTiles) {
                SetTimer(hwnd_.get(), TIMER_LIVE_FLIP, settings_.liveTileIntervalMs, nullptr);
            } else {
                KillTimer(hwnd_.get(), TIMER_LIVE_FLIP);
                KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
            }
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    void InvalidateTileRect(int groupIndex, int tileIndex) {
        if (!hwnd_ || groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
        const auto& group = layout_.groups[groupIndex];
        if (tileIndex < 0 || tileIndex >= static_cast<int>(group.tiles.size())) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int groupX = startX + groupIndex * (groupW + groupGap);
        const int tileY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            GroupHeaderHeight(groupIndex) + metrics_.Scale(4);
        RECT rect = ComputeTileRect(groupX, tileY, group.tiles[tileIndex]);
        InvalidateRect(hwnd_.get(), &rect, FALSE);
    }

    void OnLiveTimer() {
        if (!settings_.liveTiles) return;
        size_t liveCount = 0;
        for (const auto& group : layout_.groups)
            for (const auto& tile : group.tiles)
                if (HasLocalLiveContent(tile) && !tile.flipping) ++liveCount;
        if (!liveCount) return;

        static size_t nextLiveIndex = 0;
        size_t selected = nextLiveIndex++ % liveCount;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& group = layout_.groups[gi];
            for (size_t ti = 0; ti < group.tiles.size(); ++ti) {
                auto& tile = group.tiles[ti];
                if (!HasLocalLiveContent(tile) || tile.flipping) continue;
                if (selected-- == 0) {
                    tile.flipping = true;
                    tile.flipProgress = 0.0f;
                    InvalidateTileRect(static_cast<int>(gi), static_cast<int>(ti));
                    SetTimer(hwnd_.get(), TIMER_LIVE_TICK, 25, nullptr);
                    return;
                }
            }
        }
    }

    void OnTick() {
        bool stillFlipping = false;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& group = layout_.groups[gi];
            for (size_t ti = 0; ti < group.tiles.size(); ++ti) {
                auto& tile = group.tiles[ti];
                if (!tile.flipping) continue;
                tile.flipProgress += 0.06f;
                if (tile.flipProgress >= 1.0f) {
                    tile.flipProgress = 0.0f;
                    tile.flipping = false;
                    tile.showBack = !tile.showBack;
                } else {
                    stillFlipping = true;
                }
                InvalidateTileRect(static_cast<int>(gi), static_cast<int>(ti));
            }
        }
        if (!stillFlipping) KillTimer(hwnd_.get(), TIMER_LIVE_TICK);
    }

    void PersistWindowSize() {
        if (!hasUserSized_) return;
        wchar_t value[64]{};
        swprintf_s(value, L"%d,%d", savedWidth96_, savedHeight96_);
        if (!Wh_SetStringValue(L"startMenuWindowSize", value))
            Wh_Log(L"[StartMenu] failed to persist custom window size");
    }

    // Full rescan of the app catalog (Programs folders + AppsFolder). Launch
    // counts come from storage and expanded folders are preserved by the catalog.
    void RefreshCatalog() {
        if (!catalogReady_ || catalogThread_ || catalogEnriching_) { catalogRefreshPending_ = true; return; }
        catalogRefreshPending_ = false;
        catalogThread_.reset(CreateThread(nullptr, 0, InitialCatalogWorker, this, 0, nullptr));
        if (!catalogThread_) {
            catalogRefreshPending_ = true;
            Wh_Log(L"[Catalog] refresh worker creation failed");
        }
    }

    void PersistTileLayout() {
        if (!catalogReady_) return;
        const std::wstring serialized = layout_.Serialize();
        if (!Wh_SetStringValue(L"startMenuLayout", serialized.c_str()))
            Wh_Log(L"[TileLayout] failed to save layout to Windhawk mod storage");
    }

    const AppEntry* GetListAppByIndex(int index) const {
        if (index < 0) return nullptr;
        if (viewMode_ == ViewMode::Normal) {
            const int frequent = static_cast<int>(catalog_.MostUsed().size());
            if (index < frequent) return &catalog_.MostUsed()[index];
            const int recent = index - frequent;
            return recent >= 0 && recent < static_cast<int>(catalog_.RecentlyAdded().size())
                ? &catalog_.RecentlyAdded()[recent] : nullptr;
        }
        if (viewMode_ == ViewMode::Search)
            return index < static_cast<int>(searchResults_.size()) ? &searchResults_[index] : nullptr;
        if (viewMode_ == ViewMode::AllApps) return FindAllAppsAppByIndex(index);
        return nullptr;
    }

    static void GetTileFootprint(TileSize size, int& columns, int& rows) {
        // The Update 3 grid is built from 56 px small cells with an 8 px
        // gutter, the way the Start screen flows tiles: medium tiles span
        // 2x2 cells, wide 4x2 and large 4x4, so a wide or large tile fills
        // the whole group exactly like on the leaked RT Update 3 menu.
        switch (size) {
            case TileSize::Small:  columns = 1; rows = 1; break;
            case TileSize::Medium: columns = 2; rows = 2; break;
            case TileSize::Wide:   columns = 4; rows = 2; break;
            default:               columns = 4; rows = 4; break; // Large
        }
    }

    bool IsTilePlacementFree(int groupIndex, int exceptTile, int col, int row,
                             TileSize size, bool onlyPriorTiles = false) const {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size()) || row < 0)
            return false;
        int candidateW = 1, candidateH = 1;
        GetTileFootprint(size, candidateW, candidateH);
        if (col < 0 || col + candidateW > Metrics::BASE_COLUMNS_PER_GROUP) return false;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (static_cast<int>(i) == exceptTile || (onlyPriorTiles && static_cast<int>(i) >= exceptTile))
                continue;
            const Tile& other = tiles[i];
            int otherW = 1, otherH = 1;
            GetTileFootprint(other.size, otherW, otherH);
            if (col < other.col + otherW && other.col < col + candidateW &&
                row < other.row + otherH && other.row < row + candidateH)
                return false;
        }
        return true;
    }

    bool FindTilePlacement(int groupIndex, int exceptTile, int preferredCol, int preferredRow,
                           TileSize size, bool onlyPriorTiles, int& resultCol, int& resultRow) const {
        int tileW = 1, tileH = 1;
        GetTileFootprint(size, tileW, tileH);
        const int lastCol = Metrics::BASE_COLUMNS_PER_GROUP - tileW;
        if (lastCol < 0) return false;
        preferredCol = std::max(0, std::min(lastCol, preferredCol));
        preferredRow = std::max(0, std::min(100000 - tileH + 1, preferredRow));
        for (int row = preferredRow; row <= 100000 - tileH + 1; ++row) {
            for (int delta = 0; delta <= lastCol; ++delta) {
                const int left = preferredCol - delta;
                if (left >= 0 && IsTilePlacementFree(groupIndex, exceptTile, left, row, size, onlyPriorTiles)) {
                    resultCol = left; resultRow = row; return true;
                }
                const int right = preferredCol + delta;
                if (delta && right <= lastCol &&
                    IsTilePlacementFree(groupIndex, exceptTile, right, row, size, onlyPriorTiles)) {
                    resultCol = right; resultRow = row; return true;
                }
            }
        }
        return false;
    }

    bool ApplyDraggedTilePlacement(int groupIndex, int movingIndex, int wantedCol, int wantedRow) {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return false;
        auto& tiles = layout_.groups[groupIndex].tiles;
        if (movingIndex < 0 || movingIndex >= static_cast<int>(tiles.size()) ||
            dragOriginalTilePositions_.size() != tiles.size()) return false;

        auto restoreOriginalPositions = [&]() {
            for (size_t i = 0; i < tiles.size(); ++i) {
                tiles[i].col = dragOriginalTilePositions_[i].x;
                tiles[i].row = dragOriginalTilePositions_[i].y;
            }
        };
        restoreOriginalPositions();

        int movingW = 1, movingH = 1;
        GetTileFootprint(tiles[movingIndex].size, movingW, movingH);
        wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - movingW, wantedCol));
        wantedRow = std::max(0, std::min(100000 - movingH + 1, wantedRow));
        tiles[movingIndex].col = wantedCol;
        tiles[movingIndex].row = wantedRow;

        std::vector<int> displaced;
        for (int i = 0; i < static_cast<int>(tiles.size()); ++i) {
            if (i == movingIndex) continue;
            int otherW = 1, otherH = 1;
            GetTileFootprint(tiles[i].size, otherW, otherH);
            const bool overlaps = wantedCol < tiles[i].col + otherW &&
                tiles[i].col < wantedCol + movingW &&
                wantedRow < tiles[i].row + otherH &&
                tiles[i].row < wantedRow + movingH;
            if (overlaps) displaced.push_back(i);
        }

        // Park only the colliding neighbors beyond the legal grid while finding
        // new cells; the search range ends at row 100000, so these sentinels can
        // never be selected or collide with a valid tile.
        for (int i : displaced) tiles[i].row = 200000 + i;
        for (int i : displaced) {
            int newCol = 0, newRow = 0;
            const POINT original = dragOriginalTilePositions_[i];
            if (!FindTilePlacement(groupIndex, i, original.x, original.y,
                                   tiles[i].size, false, newCol, newRow)) {
                restoreOriginalPositions();
                return false;
            }
            tiles[i].col = newCol;
            tiles[i].row = newRow;
        }
        return true;
    }

    // ---- Tile movement animations -----------------------------------------

    static int TileAnimOffset(float value) {
        return static_cast<int>(value >= 0.0f ? value + 0.5f : value - 0.5f);
    }

    bool IsTileLifted(int groupIndex, int tileIndex) const {
        return tileDragging_ && groupIndex >= 0 && tileIndex >= 0 &&
               groupIndex == dragGroup_ && tileIndex == dragTile_;
    }

    // Rectangle the tile is painted at: its slot shifted by the running glide,
    // or the position under the pointer while the tile is being dragged.
    bool GetTileDrawRect(int groupIndex, int tileIndex, RECT& out) const {
        if (!GetTileClientRect(groupIndex, tileIndex, out)) return false;
        if (IsTileLifted(groupIndex, tileIndex)) {
            const LONG width = out.right - out.left;
            const LONG height = out.bottom - out.top;
            out.left = dragPointer_.x - dragTileGrabOffset_.x;
            out.top = dragPointer_.y - dragTileGrabOffset_.y;
            out.right = out.left + width;
            out.bottom = out.top + height;
            ClampTileToGrid(out);
            return true;
        }
        const Tile& tile = layout_.groups[groupIndex].tiles[tileIndex];
        OffsetRect(&out, TileAnimOffset(tile.animOffsetX), TileAnimOffset(tile.animOffsetY));
        return true;
    }

    // Keeps the dragged tile inside the tile area, whatever the pointer does:
    // it can never cover the app list or the group titles, and it cannot leave
    // the menu when the pointer is dragged outside the window.
    void ClampTileToGrid(RECT& rect) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return;
        const int width = static_cast<int>(rect.right - rect.left);
        const int height = static_cast<int>(rect.bottom - rect.top);
        if (width <= 0 || height <= 0) return;
        // The lifted tile is painted slightly larger than its slot, so that
        // growth is kept inside the grid as well.
        const int grow = static_cast<int>((TILE_DRAG_SCALE - 1.0f) * std::max(width, height) / 2.0f) + 1;
        const int minLeft = metrics_.Scale(Metrics::BASE_LEFT_COL_W) +
                            metrics_.Scale(Metrics::BASE_GROUP_PAD_X) + grow;
        const int minTop = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                           metrics_.Scale(Metrics::BASE_GROUP_HEADER_H) + metrics_.Scale(4) + grow;
        const int maxLeft = std::max(minLeft, static_cast<int>(client.right) -
                                     metrics_.Scale(Metrics::BASE_GROUP_PAD_X) - grow - width);
        const int maxTop = std::max(minTop, static_cast<int>(client.bottom) -
                                    metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) - grow - height);
        const int left = std::max(minLeft, std::min(maxLeft, static_cast<int>(rect.left)));
        const int top = std::max(minTop, std::min(maxTop, static_cast<int>(rect.top)));
        rect.left = left;
        rect.top = top;
        rect.right = left + width;
        rect.bottom = top + height;
    }

    // Where a group's tiles are painted right now. The result is handed to
    // StartTileGlide once the layout has changed.
    std::vector<POINT> CaptureTileRenderPositions(int groupIndex) const {
        std::vector<POINT> positions;
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return positions;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        positions.reserve(tiles.size());
        for (int i = 0; i < static_cast<int>(tiles.size()); ++i) {
            RECT rect{};
            if (!GetTileDrawRect(groupIndex, i, rect)) rect = RECT{};
            positions.push_back(POINT{rect.left, rect.top});
        }
        return positions;
    }

    // Turns a layout change into a movement: every tile keeps the position it
    // was painted at and eases from there into its new slot.
    void StartTileGlide(int groupIndex, const std::vector<POINT>& before) {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
        auto& tiles = layout_.groups[groupIndex].tiles;
        if (before.size() != tiles.size()) return;
        bool moved = false;
        for (size_t i = 0; i < tiles.size(); ++i) {
            // A lifted tile follows the pointer, so it is never glided.
            if (IsTileLifted(groupIndex, static_cast<int>(i))) {
                tiles[i].animOffsetX = 0.0f;
                tiles[i].animOffsetY = 0.0f;
                continue;
            }
            RECT slot{};
            if (!GetTileClientRect(groupIndex, static_cast<int>(i), slot)) continue;
            const float dx = static_cast<float>(before[i].x - slot.left);
            const float dy = static_cast<float>(before[i].y - slot.top);
            if (fabsf(dx) < TILE_ANIM_SETTLE_PX && fabsf(dy) < TILE_ANIM_SETTLE_PX) continue;
            tiles[i].animOffsetX = dx;
            tiles[i].animOffsetY = dy;
            moved = true;
        }
        if (moved) EnsureTileAnimationTimer();
    }

    void EnsureTileAnimationTimer() {
        if (!hwnd_ || tileAnimActive_) return;
        tileAnimLastTick_ = GetTickCount64();
        if (!SetTimer(hwnd_.get(), TIMER_TILE_ANIM, TILE_ANIM_INTERVAL_MS, nullptr)) {
            Wh_Log(L"[TileAnim] timer unavailable (%u): tiles move without animation", GetLastError());
            SettleTileAnimations();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }
        tileAnimActive_ = true;
    }

    void StopTileAnimationTimer() {
        if (hwnd_ && tileAnimActive_) KillTimer(hwnd_.get(), TIMER_TILE_ANIM);
        tileAnimActive_ = false;
    }

    // Drops every animation to its final state: used when the menu closes and
    // whenever the timer cannot be created.
    void SettleTileAnimations() {
        for (auto& group : layout_.groups) {
            for (auto& tile : group.tiles) {
                tile.animOffsetX = 0.0f;
                tile.animOffsetY = 0.0f;
                tile.animScale = 1.0f;
                tile.flipping = false;
                tile.flipProgress = 0.0f;
            }
        }
        StopTileAnimationTimer();
    }

    // Scale the tile is easing towards: reduced while pressed, enlarged while
    // lifted by a drag, otherwise its natural size.
    float WantedTileScale(int groupIndex, int tileIndex) const {
        if (IsTileLifted(groupIndex, tileIndex)) return TILE_DRAG_SCALE;
        if (tileDragCandidate_ && !tileDragging_ &&
            groupIndex == dragGroup_ && tileIndex == dragTile_) return TILE_PRESSED_SCALE;
        return 1.0f;
    }

    // One animation frame: advances offsets and scales, then repaints only the
    // rectangles that actually changed.
    void OnTileAnimationTick() {
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG elapsed = now > tileAnimLastTick_ ? now - tileAnimLastTick_ : 0;
        tileAnimLastTick_ = now;
        // The step is clamped so that a stalled UI thread cannot make the tiles
        // jump across the grid in a single frame.
        const float dt = std::min(0.05f, std::max(0.001f, static_cast<float>(elapsed) / 1000.0f));
        const float glide = 1.0f - expf(-dt * TILE_ANIM_GLIDE_RATE);
        const float scaleStep = std::min(1.0f, dt * TILE_ANIM_SCALE_RATE);

        bool animating = false;
        bool dirty = false;
        RECT dirtyRect{};
        const auto addDirty = [&dirty, &dirtyRect](const RECT& rect) {
            if (!dirty) {
                dirtyRect = rect;
                dirty = true;
                return;
            }
            RECT merged{};
            if (UnionRect(&merged, &dirtyRect, &rect)) dirtyRect = merged;
        };

        for (int gi = 0; gi < static_cast<int>(layout_.groups.size()); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (int ti = 0; ti < static_cast<int>(tiles.size()); ++ti) {
                Tile& tile = tiles[ti];
                const bool lifted = IsTileLifted(gi, ti);
                const float wantedScale = WantedTileScale(gi, ti);
                const bool glidePending = !lifted &&
                    (tile.animOffsetX != 0.0f || tile.animOffsetY != 0.0f);
                const bool scalePending = fabsf(tile.animScale - wantedScale) > TILE_ANIM_SETTLE_SCALE;
                if (!glidePending && !scalePending) continue;

                RECT before{};
                const bool hadBefore = GetTileDrawRect(gi, ti, before);
                if (glidePending) {
                    if (fabsf(tile.animOffsetX) < TILE_ANIM_SETTLE_PX &&
                        fabsf(tile.animOffsetY) < TILE_ANIM_SETTLE_PX) {
                        tile.animOffsetX = 0.0f;
                        tile.animOffsetY = 0.0f;
                    } else {
                        tile.animOffsetX -= tile.animOffsetX * glide;
                        tile.animOffsetY -= tile.animOffsetY * glide;
                        animating = true;
                    }
                }
                if (scalePending) {
                    tile.animScale += (wantedScale - tile.animScale) * scaleStep;
                    if (fabsf(tile.animScale - wantedScale) <= TILE_ANIM_SETTLE_SCALE)
                        tile.animScale = wantedScale;
                    else
                        animating = true;
                }
                if (hadBefore) addDirty(before);
                RECT after{};
                if (GetTileDrawRect(gi, ti, after)) addDirty(after);
            }
        }

        if (dirty && hwnd_) {
            // The scale makes a tile slightly larger than its slot.
            InflateRect(&dirtyRect, metrics_.Scale(8), metrics_.Scale(8));
            InvalidateRect(hwnd_.get(), &dirtyRect, FALSE);
        }
        // While a tile is lifted the timer stays alive: the pointer can start a
        // new glide at any moment. Otherwise it stops as soon as everything has
        // settled and is restarted on demand.
        if (!animating && !tileDragging_) StopTileAnimationTimer();
    }

    void NormalizeTileLayout() {
        // Indices must stay stable while a tile is being dragged, so the
        // duplicate cleanup is skipped until the drag is over.
        bool changed = !tileDragCandidate_ && RemoveDuplicateTiles();
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                auto& tile = tiles[ti];
                int tileW = 1, tileH = 1;
                GetTileFootprint(tile.size, tileW, tileH);
                const int wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - tileW, tile.col));
                const int wantedRow = std::max(0, std::min(100000 - tileH + 1, tile.row));
                int placedCol = wantedCol, placedRow = wantedRow;
                if (IsTilePlacementFree(static_cast<int>(gi), static_cast<int>(ti),
                                        wantedCol, wantedRow, tile.size, true)) {
                    if (tile.col != wantedCol || tile.row != wantedRow) changed = true;
                    tile.col = wantedCol; tile.row = wantedRow;
                    continue;
                }
                if (FindTilePlacement(static_cast<int>(gi), static_cast<int>(ti), wantedCol,
                                      wantedRow, tile.size, true, placedCol, placedRow)) {
                    tile.col = placedCol; tile.row = placedRow; changed = true;
                    Wh_Log(L"[TileLayout] moved overlapping/out-of-bounds tile to free grid cell (group=%u, tile=%u)",
                           static_cast<unsigned>(gi), static_cast<unsigned>(ti));
                }
            }
        }
        if (changed) PersistTileLayout();
    }

    // Two tiles are the same application when they start the same target: the
    // AUMID is compared first, the shortcut path when either side has none.
    static bool SameTileTarget(const std::wstring& pathA, const std::wstring& aumidA,
                               const std::wstring& pathB, const std::wstring& aumidB) {
        if (!aumidA.empty() && !aumidB.empty())
            return _wcsicmp(aumidA.c_str(), aumidB.c_str()) == 0;
        if (!pathA.empty() && !pathB.empty())
            return _wcsicmp(pathA.c_str(), pathB.c_str()) == 0;
        return false;
    }

    bool FindPinnedTile(const std::wstring& path, const std::wstring& aumid,
                        int& groupOut, int& tileOut) const {
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (SameTileTarget(path, aumid, tiles[ti].path, tiles[ti].aumid)) {
                    groupOut = static_cast<int>(gi);
                    tileOut = static_cast<int>(ti);
                    return true;
                }
            }
        }
        return false;
    }

    // A layout saved by an earlier build can hold the same application twice;
    // only the first tile of each application is kept.
    bool RemoveDuplicateTiles() {
        bool removed = false;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size();) {
                int firstGroup = -1, firstTile = -1;
                if (FindPinnedTile(tiles[ti].path, tiles[ti].aumid, firstGroup, firstTile) &&
                    (firstGroup != static_cast<int>(gi) || firstTile != static_cast<int>(ti))) {
                    Wh_Log(L"[TileLayout] duplicate tile '%s' removed (already pinned in group %d)",
                           tiles[ti].name.c_str(), firstGroup);
                    tiles.erase(tiles.begin() + ti);
                    removed = true;
                    continue;
                }
                ++ti;
            }
        }
        return removed;
    }

    bool PinAppInGroup(const AppEntry& app, int groupIndex, int wantedCol, int wantedRow) {
        if (!IsUsableCatalogApp(app) || groupIndex < 0 ||
            groupIndex >= static_cast<int>(layout_.groups.size())) return false;

        // An application is pinned only once: a second pin would create two
        // tiles starting the same target, so the existing one is kept.
        int pinnedGroup = -1, pinnedTile = -1;
        if (FindPinnedTile(app.path, app.aumid, pinnedGroup, pinnedTile)) {
            Wh_Log(L"[TileLayout] '%s' is already pinned (group %d, tile %d); no duplicate added",
                   app.name.c_str(), pinnedGroup, pinnedTile);
            InvalidateTileRect(pinnedGroup, pinnedTile);
            return true;
        }
        auto& tiles = layout_.groups[groupIndex].tiles;
        wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - 1, wantedCol));
        wantedRow = std::max(0, std::min(100000, wantedRow));

        auto cellFree = [&tiles](int col, int row) {
            for (const auto& existing : tiles) {
                const int existingW = (existing.size == TileSize::Wide || existing.size == TileSize::Large) ? 2 : 1;
                const int existingH = existing.size == TileSize::Large ? 2 : 1;
                if (col < existing.col + existingW && col + 1 > existing.col &&
                    row < existing.row + existingH && row + 1 > existing.row) return false;
            }
            return true;
        };
        int chosenCol = -1, chosenRow = -1;
        for (int distance = 0; distance < 64 && chosenCol < 0; ++distance) {
            const int firstRow = std::max(0, wantedRow - distance);
            const int lastRow = std::min(100000, wantedRow + distance);
            for (int row = firstRow; row <= lastRow && chosenCol < 0; ++row) {
                for (int col = 0; col < Metrics::BASE_COLUMNS_PER_GROUP; ++col) {
                    if (std::abs(col - wantedCol) + std::abs(row - wantedRow) == distance && cellFree(col, row)) {
                        chosenCol = col; chosenRow = row; break;
                    }
                }
            }
        }
        if (chosenCol < 0) return false;

        Tile tile;
        tile.name = app.name;
        tile.path = app.path;
        tile.aumid = app.aumid;
        tile.hIcon = app.hIcon;
        tile.appColor = GetIconAccentColor(app.hIcon);
        tile.hasFlatIconBackground = XboxIconBackground(tile.name,tile.aumid,tile.hIcon,tile.flatIconBackground);
        tile.size = TileSize::Medium;
        tile.col = chosenCol;
        tile.row = chosenRow;
        tile.available = true;
        tiles.push_back(std::move(tile));
        PersistTileLayout();
        InvalidateTileRect(groupIndex, static_cast<int>(tiles.size()) - 1);
        return true;
    }

    bool PinAppAtPoint(const AppEntry& app, POINT point) {
        if (!IsUsableCatalogApp(app) || point.x < metrics_.Scale(Metrics::BASE_LEFT_COL_W)) return false;
        if (layout_.groups.empty()) {
            TileGroup group;
            group.title = L"Applicazioni";
            layout_.groups.push_back(std::move(group));
        }
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int groupStartX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int cellW = metrics_.Scale(Metrics::BASE_TILE_SMALL) + metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupStep = groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int offsetX = point.x - groupStartX;
        const int groupIndex = offsetX >= 0 ? offsetX / std::max(1, groupStep) : -1;
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size()) ||
            offsetX % std::max(1, groupStep) >= groupW) return false;

        const int tileStartY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            GroupHeaderHeight(groupIndex) + metrics_.Scale(4);
        if (point.y < tileStartY) return false;
        const int pointX = static_cast<int>(point.x);
        const int pointY = static_cast<int>(point.y);
        const int wantedCol = std::max(0, std::min(Metrics::BASE_COLUMNS_PER_GROUP - 1,
            (pointX - (groupStartX + groupIndex * groupStep)) / std::max(1, cellW)));
        const int wantedRow = std::max(0, (pointY - tileStartY) / std::max(1, cellW));
        return PinAppInGroup(app, groupIndex, wantedCol, wantedRow);
    }

private:
    HINSTANCE hInst_ = nullptr;
    UniqueHwnd hwnd_;
    UniqueMemDC renderDc_;
    UniqueBitmap renderBitmap_;
    int renderBufferWidth_ = 0;
    int renderBufferHeight_ = 0;
    Metrics metrics_;
    Settings settings_;
    AppCatalog catalog_;
    TileLayout layout_;
    UniqueHandle catalogThread_;
    std::mutex catalogMutex_;
    std::unique_ptr<AppCatalog> pendingInitialCatalog_;
    bool catalogReady_ = false;
    bool catalogSwapPending_ = false;
    bool catalogQuickPhase_ = true; // modified only after joining the previous worker
    bool catalogEnriching_ = false;
    static DWORD WINAPI InitialCatalogWorker(void* arg) {
        auto* self = static_cast<StartMenuWindow*>(arg);
        SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
        const ULONGLONG started=GetTickCount64();
        ScopedCom com;
        try {
            auto result = std::make_unique<AppCatalog>();
            const bool cached=self->catalogQuickPhase_ && result->LoadSnapshot();
            Wh_Log(L"[CatalogPerf] cache lookup elapsed=%llu ms hit=%d", GetTickCount64()-started,cached);
            if (!cached) result->Refresh(self->catalogQuickPhase_);
            // Save after a successful scan rather than only on unload, which is
            // not guaranteed after a crash or system shutdown. A first quick
            // scan may be saved without icons until the full scan replaces it.
            if (!cached && !result->SaveSnapshot())
                Wh_Log(L"[CatalogCache] snapshot unavailable; next start uses live scan");
            Wh_Log(L"[CatalogPerf] worker elapsed=%llu ms quick=%d", GetTickCount64()-started,self->catalogQuickPhase_);
            { std::lock_guard<std::mutex> guard(self->catalogMutex_);
              self->pendingInitialCatalog_ = std::move(result); }
            PostMessageW(self->hwnd_.get(), WM_APP_INITIAL_CATALOG_READY, 0, 0);
        } catch (...) {
            Wh_Log(L"[Catalog] initial scan failed");
            PostMessageW(self->hwnd_.get(), WM_APP_INITIAL_CATALOG_READY, 0, 0);
        }
        return 0;
    }
    void FinishInitialCatalog() {
        // Keep the active catalog and its icon handles stable while a user is
        // interacting with it. The first publication has no old app entries.
        if (catalogReady_ && visible_ &&
            (!catalogEnriching_ || GetCapture()==hwnd_.get() || pendingContext_!=PendingContext::None)) {
            catalogSwapPending_ = true;
            if (catalogEnriching_ && hwnd_) SetTimer(hwnd_.get(),TIMER_CATALOG_PUBLISH,500,nullptr);
            return;
        }
        catalogSwapPending_ = false;
        if (hwnd_) KillTimer(hwnd_.get(),TIMER_CATALOG_PUBLISH);
        std::unique_ptr<AppCatalog> result;
        { std::lock_guard<std::mutex> guard(catalogMutex_); result = std::move(pendingInitialCatalog_); }
        if (catalogThread_) { WaitForSingleObject(catalogThread_.get(), INFINITE); catalogThread_.reset(); }
        if (!result) { Wh_Log(L"[Catalog] scan failed; preserving current layout"); return; }
        bool first = !catalogReady_;
        if (!first) result->InheritUserState(catalog_);
        if (g_iconBitmapCache) g_iconBitmapCache->clear();
        searchResults_.clear();
        catalog_ = std::move(*result);
        catalogReady_ = true;
        lastCatalogScanTick_ = GetTickCount64();
        if (first) {
            layout_.LoadDefault(catalog_);
            ApplySettings(settings_);
        } else {
            layout_.RefreshInstalledApps(catalog_);
        }
        allAppsRowsDirty_ = true;
        if (!searchQuery_.empty()) FilterSearch();
        ClearKeyboardFocus();
        keyboardTileGroup_ = keyboardTileIndex_ = -1;
        if (hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE);
        Wh_Log(L"[Catalog] background scan published (initial=%d quick=%d)", first, catalogQuickPhase_);
        if (first && catalogQuickPhase_) {
            // Enrich the quick inventory with real icons and shortcut targets on
            // a fresh worker; never block the window while Shell providers run.
            catalogQuickPhase_ = false;
            // Avoid launching another expensive Shell enumeration at the same
            // instant as the first inventory and other Explorer startup work.
            catalogEnriching_ = true;
            if (!SetTimer(hwnd_.get(),TIMER_CATALOG_ENRICH,10000,nullptr)) {
                catalogEnriching_=false; catalogRefreshPending_=true;
                Wh_Log(L"[CatalogPerf] enrichment timer failed; refresh deferred");
            } else Wh_Log(L"[CatalogPerf] enrichment scheduled in 10 seconds");
        } else catalogEnriching_ = false;
        if (catalogRefreshPending_ && !visible_ && hwnd_)
            PostMessageW(hwnd_.get(), WM_APP_REFRESH, 0, 0);
    }
    enum class PendingContext { None, Tile, App, SearchExtra, User, Power };
    PendingContext pendingContext_ = PendingContext::None;
    int pendingGroup_ = -1;
    int pendingTile_ = -1;
    std::wstring pendingAppName_;
    std::wstring pendingAppPath_;
    std::wstring pendingAppAumid_;
    std::unique_ptr<StartScreenSearch::AppEntry> pendingSearchExtra_;
    int pendingSearchKind_ = 0;
    std::wstring accountDisplayName_; // cached; resolved off the UI thread

    bool visible_ = false;
    bool programmaticResize_ = false;
    bool customResizeActive_ = false;
    bool customResizeTop_ = false;
    bool customResizeRight_ = false;
    POINT customResizeStartPoint_{};
    RECT customResizeStartRect_{};
    bool hasUserSized_ = false;
    bool classRegistered_ = false;
    bool layoutApplied_ = false;      // stored/imported layout applied once per window
    bool catalogRefreshPending_ = false; // refresh requested while the menu was open
    ULONGLONG lastCatalogScanTick_ = 0;
    HWND requestedTaskbar_ = nullptr; // taskbar whose Start button was clicked (0 = primary)
    int savedWidth96_ = Metrics::BASE_DEFAULT_MENU_W;
    int savedHeight96_ = Metrics::BASE_DEFAULT_MENU_H;
    wchar_t savedWindowSize_[64]{};
    ULONGLONG showTick_ = 0;
    UINT visibleAnchorEdge_ = ABE_BOTTOM;
    HWND visibleTaskbarHwnd_ = nullptr;
    RECT visibleTaskbarRect_{};
    bool taskbarClipActive_ = false;
    // Width of the wide (tiles) state, restored when leaving the All-apps view.
    int wideWidthPx_ = 0;

    ViewMode viewMode_ = ViewMode::Normal;
    std::wstring searchQuery_;
    bool searchFocused_ = false; // box clicked with no query: stays in its own state
    bool searchAllSelected_ = false;
    RECT dragPreviewRect_{};
    POINT dragPreviewPoint_{};
    std::vector<AppEntry> searchResults_;
    std::vector<StartScreenSearch::AppEntry> searchExtraCatalog_;
    struct SearchIconSlot { int state=0; StartScreenSearch::SearchIconPixels image; };
    std::vector<SearchIconSlot> searchIcons_;

    int mainListScrollY_ = 0;
    int totalMainListH_ = 0;
    bool mainListScrollDragging_ = false;
    int allAppsScrollY_ = 0;
    int totalAllAppsH_ = 0;
    bool allAppsScrollDragging_ = false;
    bool allAppsScrollArrowPressed_ = false;
    int allAppsScrollRepeatDirection_ = 0;
    int scrollDragTrackTop_ = 0;
    int scrollDragTrackHeight_ = 0;
    int scrollDragThumbHeight_ = 0;
    int scrollDragThumbOffset_ = 0;
    int scrollDragMaxScroll_ = 0;

    int hoverArea_ = -1;  // 0=User, 1=Places, 2=AppList, 3=Tile, 4=GroupHeader
    int hoverIndex_ = -1;
    int hoverGroup_ = -1;

    // Keyboard navigation. The focused element is stored in the hover fields
    // above, so a pointer hover and a keyboard focus can never be displayed at
    // the same time; keyboardNav_ only records which of the two owns the
    // highlight and whether the focus ring has to be drawn.
    bool keyboardNav_ = false;
    int keyboardTileGroup_ = -1; // tile the keyboard left the grid from
    int keyboardTileIndex_ = -1;
    bool tileDragCandidate_ = false;
    bool tileDragging_ = false;
    bool appDragCandidate_ = false;
    bool appDragging_ = false;
    AppEntry draggedApp_;
    int dragGroup_ = -1, dragTile_ = -1;
    std::vector<POINT> dragOriginalTilePositions_;
    POINT dragStart_{};
    POINT dragTileGrabOffset_{}; // pointer position within tile, retained while snapping to grid.
    POINT dragPointer_{};        // last pointer position while a tile is lifted.
    bool tileAnimActive_ = false;      // TIMER_TILE_ANIM currently running
    ULONGLONG tileAnimLastTick_ = 0;   // tick of the previous animation frame

    UniqueFont fontNormal_;
    UniqueFont fontLight_;
    UniqueFont fontBold_;
    UniqueFont fontTitle_;
    UniqueFont fontSmall_;
    UniqueFont fontHeader_;

    void ClearTaskbarWindowRegion() {
        if (!hwnd_ || !taskbarClipActive_) return;
        if (SetWindowRgn(hwnd_.get(), nullptr, TRUE)) taskbarClipActive_ = false;
        else Wh_Log(L"[StartMenu] clearing taskbar clip failed: %u", GetLastError());
    }

    void UpdateTaskbarWindowRegion() {
        if (!hwnd_) return;
        if (!visibleTaskbarHwnd_ || !IsWindow(visibleTaskbarHwnd_) ||
            !IsWindowVisible(visibleTaskbarHwnd_)) {
            ClearTaskbarWindowRegion();
            return;
        }
        RECT windowRect{}, taskbarRect{};
        if (!GetWindowRect(hwnd_.get(), &windowRect)) return;
        if (!GetWindowRect(visibleTaskbarHwnd_, &taskbarRect)) taskbarRect = visibleTaskbarRect_;
        RECT overlap{};
        if (!IntersectRect(&overlap, &windowRect, &taskbarRect)) {
            ClearTaskbarWindowRegion();
            return;
        }
        const int width = windowRect.right - windowRect.left;
        const int height = windowRect.bottom - windowRect.top;
        UniqueRegion windowRegion(CreateRectRgn(0, 0, width, height));
        UniqueRegion taskbarCut(CreateRectRgn(overlap.left - windowRect.left,
            overlap.top - windowRect.top, overlap.right - windowRect.left,
            overlap.bottom - windowRect.top));
        if (!windowRegion || !taskbarCut ||
            CombineRgn(windowRegion.get(), windowRegion.get(), taskbarCut.get(), RGN_DIFF) == ERROR) {
            Wh_Log(L"[StartMenu] failed to create taskbar clip region");
            return;
        }
        if (SetWindowRgn(hwnd_.get(), windowRegion.get(), TRUE)) {
            windowRegion.release(); // ownership transfers to the system on success
            taskbarClipActive_ = true;
        } else {
            Wh_Log(L"[StartMenu] SetWindowRgn failed: %u", GetLastError());
        }
    }

    // All apps changes the left list without resizing or hiding the tile pane.
    void UpdateDpiMetrics(UINT dpi) {
        if (!dpi || dpi>960 || dpi==metrics_.dpi) return;
        const UINT old=metrics_.dpi ? metrics_.dpi : 96;
        const bool scrollCaptured=allAppsScrollDragging_ || mainListScrollDragging_ || allAppsScrollArrowPressed_;
        StopAllAppsArrowRepeat(true);
        allAppsScrollDragging_=mainListScrollDragging_=false;
        if (scrollCaptured && hwnd_ && GetCapture()==hwnd_.get()) ReleaseCapture();
        mainListScrollY_=MulDiv(mainListScrollY_,dpi,old);
        allAppsScrollY_=MulDiv(allAppsScrollY_,dpi,old);
        totalMainListH_=MulDiv(totalMainListH_,dpi,old);
        totalAllAppsH_=MulDiv(totalAllAppsH_,dpi,old);
        if (wideWidthPx_>0) wideWidthPx_=MulDiv(wideWidthPx_,dpi,old);
        metrics_.dpi=dpi;
    }

    void ApplyViewWindowSize() {
        // Views share the same width: All Apps replaces the left list only.
        // No programmatic resize or saved-width changes are necessary.
    }

    int WideMenuWidthPx() const {
        if (wideWidthPx_ > 0) return wideWidthPx_;
        if (settings_.widthOverride > 0) return metrics_.Scale(settings_.widthOverride);
        if (hasUserSized_) return metrics_.Scale(savedWidth96_);
        return metrics_.Scale(Metrics::BASE_DEFAULT_MENU_W);
    }

    bool ApplyWindowOpacity() {
        if (!hwnd_) return false;
        // SetLayeredWindowAttributes uses a documented alpha byte: 255 opaque,
        // 0 transparent. 242/255 is approximately 95% opacity.
        if (!SetLayeredWindowAttributes(hwnd_.get(), 0, kMenuOpacity, LWA_ALPHA)) {
            Wh_Log(L"[StartMenu] SetLayeredWindowAttributes failed: %u", GetLastError());
            return false;
        }
        return true;
    }

    RECT ClampRectToWorkArea(const RECT& candidate) const {
        HMONITOR monitor = MonitorFromRect(&candidate, MONITOR_DEFAULTTONEAREST);
        MONITORINFO info{ sizeof(info) };
        if (!monitor || !GetMonitorInfoW(monitor, &info)) return candidate;
        return FitRectToWorkArea(candidate, info.rcWork,
                                 static_cast<LONG>(metrics_.Scale(420)),
                                 static_cast<LONG>(metrics_.Scale(420)));
    }

    bool BeginCustomResize(int x, int y) {
        if (!hwnd_ || !visible_) return false;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return false;
        const int border = std::max(5, static_cast<int>(metrics_.dpi ? metrics_.dpi : 96) / 12);
        customResizeTop_ = y >= 0 && y < border;
        customResizeRight_ = x >= client.right - border && x < client.right;
        if (!customResizeTop_ && !customResizeRight_) return false;
        if (!GetWindowRect(hwnd_.get(), &customResizeStartRect_)) return false;
        if (!GetCursorPos(&customResizeStartPoint_)) return false;
        customResizeActive_ = true;
        SetCapture(hwnd_.get());
        return true;
    }

    void ApplyCustomResize() {
        if (!customResizeActive_ || !hwnd_) return;
        POINT point{};
        if (!GetCursorPos(&point)) return;
        const LONG dx = point.x - customResizeStartPoint_.x;
        const LONG dy = point.y - customResizeStartPoint_.y;
        RECT r = customResizeStartRect_;
        MONITORINFO mi{sizeof(mi)};
        HMONITOR monitor = MonitorFromWindow(hwnd_.get(), MONITOR_DEFAULTTONEAREST);
        const bool haveWorkArea = monitor && GetMonitorInfoW(monitor, &mi);
        const LONG workW = haveWorkArea ? (std::max)(1L, mi.rcWork.right - mi.rcWork.left) : LONG_MAX;
        const LONG workH = haveWorkArea ? (std::max)(1L, mi.rcWork.bottom - mi.rcWork.top) : LONG_MAX;
        const LONG minHeight = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workH);
        const LONG minWidth = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workW);
        if (customResizeTop_) {
            const LONG topLimit = haveWorkArea ? mi.rcWork.top : LONG_MIN;
            r.top = (std::max)(topLimit, (std::min)(r.bottom - minHeight, r.top + dy));
        }
        if (customResizeRight_) {
            const LONG rightLimit = haveWorkArea ? mi.rcWork.right : LONG_MAX;
            r.right = (std::max)(r.left + minWidth, (std::min)(rightLimit, r.right + dx));
        }
        r = ClampRectToWorkArea(r);
        SetWindowPos(hwnd_.get(), nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void InitFonts() {
        fontNormal_.reset(CreateFontW(-metrics_.Scale(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        // Regular Segoe UI for the app list, as in the Technical Preview reference.
        // GDI substitutes a system font if Segoe UI is unavailable.
        fontLight_.reset(CreateFontW(-metrics_.Scale(12), 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontBold_.reset(CreateFontW(-metrics_.Scale(13), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontTitle_.reset(CreateFontW(-metrics_.Scale(16), 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Light"));
        fontSmall_.reset(CreateFontW(-metrics_.Scale(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
        fontHeader_.reset(CreateFontW(-metrics_.Scale(14), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"));
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        StartMenuWindow* pThis = nullptr;
        if (uMsg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            pThis = reinterpret_cast<StartMenuWindow*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        } else {
            pThis = reinterpret_cast<StartMenuWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }
        if (pThis) {
            try {
                return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
            } catch (const std::exception& e) {
                Wh_Log(L"[WndProc] C++ exception in message %u: %S", uMsg, e.what());
            } catch (...) {
                Wh_Log(L"[WndProc] unknown C++ exception in message %u", uMsg);
            }
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_SETCURSOR: {
                if (LOWORD(lParam) == HTCLIENT && hwnd_ && visible_) {
                    POINT p{}; GetCursorPos(&p); ScreenToClient(hwnd_.get(), &p);
                    RECT c{}; GetClientRect(hwnd_.get(), &c);
                    const int edge=std::max(5, static_cast<int>(metrics_.dpi ? metrics_.dpi : 96)/12);
                    if (p.y >= 0 && p.y < edge) { SetCursor(LoadCursorW(nullptr, IDC_SIZENS)); return TRUE; }
                    if (p.x >= c.right-edge && p.x < c.right) { SetCursor(LoadCursorW(nullptr, IDC_SIZEWE)); return TRUE; }
                    const int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
                    const int leftW = std::min(static_cast<int>(c.right), metrics_.Scale(Metrics::BASE_LEFT_COL_W));
                    const int margin = metrics_.Scale(8);
                    RECT searchBox{margin, c.bottom - placeH + metrics_.Scale(1),
                                   leftW - margin, c.bottom - metrics_.Scale(1)};
                    if (PtInRect(&searchBox, p)) {
                        SetCursor(LoadCursorW(nullptr, IDC_IBEAM));
                        return TRUE;
                    }
                }
                break;
            }
            case WM_APP_INITIAL_CATALOG_READY:
                FinishInitialCatalog();
                return 0;
            case WM_APP_SEARCH_CATALOG_READY: {
                std::vector<StartScreenSearch::AppEntry> batch;
                { std::lock_guard<std::mutex> guard(StartScreenSearch::g_lock);
                  batch.swap(StartScreenSearch::g_pending); }
                for (auto& entry : batch) searchExtraCatalog_.push_back(std::move(entry));
                searchIcons_.resize(searchExtraCatalog_.size());
                Wh_Log(L"[Search] settings and files available: %u", (unsigned)searchExtraCatalog_.size());
                if (!searchQuery_.empty()) {
                    FilterSearch();
                    if (hoverIndex_ >= (int)searchResults_.size()) ClearKeyboardFocus();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            case WM_APP_SEARCH_ICON_READY: {
                std::vector<StartScreenSearch::IconReply> replies;
                { std::lock_guard<std::mutex> guard(StartScreenSearch::g_lock); replies.swap(StartScreenSearch::g_iconReplies); }
                for (auto& reply : replies) {
                    if (reply.index >= searchIcons_.size()) continue;
                    auto& slot=searchIcons_[reply.index];
                    slot.image=std::move(reply.image); slot.state=2;
                }
                if (visible_ && viewMode_==ViewMode::Search) InvalidateRect(hwnd,nullptr,FALSE);
                return 0;
            }
            case WM_APP_PROFILE_PHOTO_READY: {
                std::vector<BYTE> bytes;
                {
                    std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
                    bytes.swap(g_profilePhotoPending);
                    if (g_accountNamePendingValid) {
                        accountDisplayName_ = g_accountNamePending;
                        g_accountNamePendingValid = false;
                        InvalidateRect(hwnd, nullptr, FALSE);
                    }
                }
                if (!bytes.empty() && bytes.size() <= 32u * 1024u * 1024u) {
                    ComPtr<IStream> stream;
                    stream.Attach(SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size())));
                    if (stream) {
                        auto bitmap = std::make_unique<Gdiplus::Bitmap>(stream.Get(), FALSE);
                        if (bitmap->GetLastStatus() == Gdiplus::Ok && bitmap->GetWidth() && bitmap->GetHeight()) {
                            g_profilePhoto.reset();
                            g_profilePhotoStream = std::move(stream);
                            g_profilePhoto = std::move(bitmap);
                            InvalidateRect(hwnd, nullptr, FALSE);
                        } else {
                            Wh_Log(L"[Profile] GDI+ could not decode account image (status=%d)",
                                   bitmap ? static_cast<int>(bitmap->GetLastStatus()) : -1);
                        }
                    } else {
                        Wh_Log(L"[Profile] SHCreateMemStream failed for account image");
                    }
                }
                return 0;
            }
            case WM_PAINT: {
                ScopedPaint paint(hwnd);
                if (paint.get()) Render(paint.get(), paint.PaintRect());
                return 0;
            }
            case WM_PRINT:
            case WM_PRINTCLIENT: {
                HDC printDc = reinterpret_cast<HDC>(wParam);
                RECT client{};
                if (printDc && GetClientRect(hwnd, &client)) Render(printDc, client);
                return 0;
            }
            case WM_ERASEBKGND:
                return 1;

            case WM_ACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE) {
                    // a context popup can steal the focus for an instant
                    if (StartScreenPopup::g_inUserMenu) return 0;
                    if (GetTickCount64() - showTick_ > 300) Hide();
                    else Wh_Log(L"[StartMenu] WM_ACTIVATE(inactive) ignored during grace period");
                }
                return 0;

            case WM_KILLFOCUS:
                if (!StartScreenPopup::g_inUserMenu && GetTickCount64() - showTick_ > 300) Hide();
                return 0;

            case WM_KEYDOWN:
                OnKeyDown((UINT)wParam);
                return 0;

            case WM_SYSKEYDOWN:
                // F10 and Shift+F10 arrive as system keys; every other system
                // key combination keeps its default handling.
                if (wParam == VK_F10) {
                    OnKeyDown((UINT)wParam);
                    return 0;
                }
                break;

            case WM_CLOSE:
                // Alt+F4 / SC_CLOSE only dismisses the menu; the window lives until unload.
                Hide();
                return 0;

            case WM_CHAR:
                OnChar((wchar_t)wParam);
                return 0;

            case WM_MOUSEMOVE:
                if (customResizeActive_) {
                    ApplyCustomResize();
                    return 0;
                }
                OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;

            case WM_MOUSELEAVE:
                if (!tileDragging_ && !allAppsScrollDragging_ && !mainListScrollDragging_) {
                    const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
                    hoverArea_ = hoverIndex_ = hoverGroup_ = -1;
                    InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
                }
                return 0;

            case WM_LBUTTONDBLCLK: {
                POINT pt{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                RECT client{}; GetClientRect(hwnd,&client);
                const int leftW=std::min(static_cast<int>(client.right),metrics_.Scale(Metrics::BASE_LEFT_COL_W));
                const int placeH=metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
                const int bottomY=client.bottom-BottomPlaceItemCount()*placeH;
                RECT field{metrics_.Scale(8),bottomY+placeH+metrics_.Scale(1),
                           leftW-metrics_.Scale(8),bottomY+2*placeH-metrics_.Scale(1)};
                if (PtInRect(&field,pt)) {
                    keyboardNav_=false; searchFocused_=true;
                    if (!searchQuery_.empty()) { viewMode_=ViewMode::Search; searchAllSelected_=true; }
                    EnsureSearchCaretTimer(); InvalidateRect(hwnd,nullptr,FALSE);
                    return 0;
                }
                break;
            }
            case WM_LBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
                if (BeginCustomResize(x, y)) return 0;
                if ((viewMode_ == ViewMode::AllApps || viewMode_ == ViewMode::Search) && BeginAllAppsScroll(x, y)) return 0;
                if (viewMode_ == ViewMode::Normal && BeginMainListScroll(x, y)) return 0;
                int area=-1, idx=-1, grp=-1; HitTest(POINT{x,y}, area, idx, grp);
                if (area == 3 && grp >= 0 && grp < static_cast<int>(layout_.groups.size()) &&
                    idx >= 0 && idx < static_cast<int>(layout_.groups[grp].tiles.size())) {
                    dragOriginalTilePositions_.clear();
                    dragOriginalTilePositions_.reserve(layout_.groups[grp].tiles.size());
                    for (const auto& tile : layout_.groups[grp].tiles)
                        dragOriginalTilePositions_.push_back(POINT{tile.col, tile.row});
                    tileDragCandidate_ = true;
                    tileDragging_ = false;
                    dragGroup_ = grp;
                    dragTile_ = idx;
                    dragStart_ = {x, y};
                    const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
                    const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                        (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                    const int groupStep = groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP);
                    const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) + grp * groupStep;
                    const int tileY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                        GroupHeaderHeight(grp) + metrics_.Scale(4);
                    const RECT grabbedTile = ComputeTileRect(groupX, tileY, layout_.groups[grp].tiles[idx]);
                    dragTileGrabOffset_ = {x - grabbedTile.left, y - grabbedTile.top};
                    dragPointer_ = POINT{x, y};
                    // Eases the press-in effect of the tile under the pointer.
                    EnsureTileAnimationTimer();
                    SetCapture(hwnd);
                } else if (area == 2) {
                    const AppEntry* app = GetListAppByIndex(idx);
                    if (app && app->searchExtra) {
                        OnLButtonDown(x,y); // settings/files open; they are not pinnable app tiles
                    } else if (app) {
                        draggedApp_ = *app;
                        appDragCandidate_ = true;
                        appDragging_ = false;
                        dragStart_ = {x, y};
                        SetCapture(hwnd);
                    } else {
                        OnLButtonDown(x,y); // e.g. expand/collapse an All Apps folder
                    }
                } else {
                    if (area==1 && idx==1) keyboardNav_=false;
                    OnLButtonDown(x,y);
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                if (allAppsScrollArrowPressed_) {
                    StopAllAppsArrowRepeat(true);
                    return 0;
                }
                if (customResizeActive_) {
                    customResizeActive_ = false;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    PersistWindowSize();
                    return 0;
                }
                if (appDragCandidate_) {
                    const bool wasDragging = appDragging_;
                    AppEntry app = std::move(draggedApp_);
                    const POINT clickStart = dragStart_;
                    appDragCandidate_ = appDragging_ = false;
                    ClearDragPreview();
                    draggedApp_ = AppEntry{};
                    if (GetCapture() == hwnd) ReleaseCapture();
                    if (wasDragging) {
                        POINT dropPoint{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                        if (!PinAppAtPoint(app, dropPoint))
                            Wh_Log(L"[TileLayout] app drag ended outside an available tile group");
                    } else {
                        OnLButtonDown(clickStart.x, clickStart.y);
                    }
                    return 0;
                }
                if (allAppsScrollDragging_ || mainListScrollDragging_) {
                    allAppsScrollDragging_ = false;
                    mainListScrollDragging_ = false;
                    if (GetCapture() == hwnd) ReleaseCapture();
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
                if (tileDragCandidate_) {
                    bool dragged=tileDragging_; int grp=dragGroup_, idx=dragTile_;
                    // Captured while the tile is still lifted: once the drag
                    // state is cleared it glides from the pointer into its slot
                    // instead of snapping there.
                    const std::vector<POINT> droppedFrom =
                        dragged ? CaptureTileRenderPositions(grp) : std::vector<POINT>();
                    tileDragCandidate_=tileDragging_=false; dragGroup_=dragTile_=-1;
                    if (dragged) StartTileGlide(grp, droppedFrom);
                    // Also lets the press-in effect ease back out.
                    EnsureTileAnimationTimer();
                    ClearDragPreview();
                    dragOriginalTilePositions_.clear();
                    if (GetCapture()==hwnd) ReleaseCapture();
                    if (!dragged && grp>=0 && grp<(int)layout_.groups.size() && idx>=0 && idx<(int)layout_.groups[grp].tiles.size()) {
                        auto& tile=layout_.groups[grp].tiles[idx];
                        if (tile.available) {
                            Hide(); catalog_.RecordLaunch(tile.path.empty()?tile.aumid:tile.path);
                            LaunchApplication(tile.path,tile.aumid);
                        } else {
                            Wh_Log(L"[TileLayout] retained tile target is currently unavailable; click ignored");
                        }
                    } else if (dragged) {
                        PersistTileLayout();
                    }
                    InvalidateRect(hwnd,nullptr,FALSE); return 0;
                }
                return 0;
            }

            case WM_RBUTTONUP:
                OnRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;

            case WM_MOUSEWHEEL: {
                POINT wheelPt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(hwnd, &wheelPt);
                OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), wheelPt);
                return 0;
            }
            case WM_CAPTURECHANGED:
            case WM_CANCELMODE:
                StopAllAppsArrowRepeat(true);
                customResizeActive_ = false;
                ClearDragPreview();
                if (appDragCandidate_) {
                    appDragCandidate_ = appDragging_ = false;
                    ClearDragPreview();
                    draggedApp_ = AppEntry{};
                }
                if (tileDragCandidate_) {
                    const int cancelledGroup = dragGroup_;
                    const std::vector<POINT> cancelledFrom = CaptureTileRenderPositions(cancelledGroup);
                    if (dragGroup_ >= 0 && dragGroup_ < static_cast<int>(layout_.groups.size()) &&
                        dragOriginalTilePositions_.size() == layout_.groups[dragGroup_].tiles.size()) {
                        auto& tiles = layout_.groups[dragGroup_].tiles;
                        for (size_t i = 0; i < tiles.size(); ++i) {
                            tiles[i].col = dragOriginalTilePositions_[i].x;
                            tiles[i].row = dragOriginalTilePositions_[i].y;
                        }
                    }
                    dragOriginalTilePositions_.clear();
                    tileDragCandidate_ = tileDragging_ = false;
                    dragGroup_ = dragTile_ = -1;
                    // A cancelled drag puts the tiles back by gliding them.
                    StartTileGlide(cancelledGroup, cancelledFrom);
                    EnsureTileAnimationTimer();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                if (allAppsScrollDragging_ || mainListScrollDragging_) {
                    allAppsScrollDragging_ = false;
                    mainListScrollDragging_ = false;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;

            case WM_TIMER:
                if (wParam == TIMER_ALL_APPS_SCROLL_REPEAT) OnAllAppsArrowRepeatTimer();
                else if (wParam == TIMER_LIVE_FLIP) OnLiveTimer();
                else if (wParam == TIMER_LIVE_TICK) OnTick();
                else if (wParam == TIMER_TILE_ANIM) OnTileAnimationTick();
                else if (wParam == TIMER_CATALOG_ENRICH) {
                    KillTimer(hwnd,TIMER_CATALOG_ENRICH);
                    if (!catalogThread_) {
                        catalogThread_.reset(CreateThread(nullptr,0,InitialCatalogWorker,this,0,nullptr));
                        if (!catalogThread_) { catalogEnriching_=false; catalogRefreshPending_=true; }
                    }
                }
                else if (wParam == TIMER_CATALOG_PUBLISH) {
                    if (catalogSwapPending_) FinishInitialCatalog();
                    else KillTimer(hwnd,TIMER_CATALOG_PUBLISH);
                }
                else if (wParam == TIMER_SEARCH_CARET) {
                    // Repaint for the blinking caret of the search box.
                    if (viewMode_ == ViewMode::Search) InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;

            case WM_DPICHANGED: {
                const UINT dpi = HIWORD(wParam);
                UpdateDpiMetrics(dpi);
                InitFonts();
                RECT* prc = reinterpret_cast<RECT*>(lParam);
                if (!prc) return 0;
                const RECT suggested = ClampRectToWorkArea(*prc);
                programmaticResize_ = true;
                SetWindowPos(hwnd, nullptr, suggested.left, suggested.top,
                             suggested.right - suggested.left, suggested.bottom - suggested.top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
                programmaticResize_ = false;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_NCCALCSIZE:
                // WS_POPUP has no native frame; keep all pixels in our client area.
                return 0;
            case WM_NCPAINT:
                return 0;
            case WM_NCACTIVATE:
                return TRUE;
            case WM_NCHITTEST:
                // Resizing is handled by our client-area drag code, never by Windows chrome.
                return HTCLIENT;
            case WM_GETMINMAXINFO: {
                auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                MONITORINFO mi{ sizeof(mi) };
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (!GetMonitorInfoW(monitor, &mi)) SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
                const LONG workW = (std::max)(1L, mi.rcWork.right - mi.rcWork.left);
                const LONG workH = (std::max)(1L, mi.rcWork.bottom - mi.rcWork.top);
                // Match the original layout's monitor-bounded sizing behavior:
                // the minimum must never exceed the available work area.
                mmi->ptMinTrackSize.x = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workW);
                mmi->ptMinTrackSize.y = (std::min)(static_cast<LONG>(metrics_.Scale(420)), workH);
                mmi->ptMaxTrackSize.x = workW;
                mmi->ptMaxTrackSize.y = workH;
                return 0;
            }
            case WM_SIZE:
                if (visible_ && !programmaticResize_ && wParam != SIZE_MINIMIZED) {
                    RECT client{};
                    if (GetClientRect(hwnd, &client)) {
                        const int width = client.right - client.left;
                        const int height = client.bottom - client.top;
                        if (width > 0 && height > 0) {
                            savedWidth96_ = MulDiv(width, 96, static_cast<int>(metrics_.dpi));
                            savedHeight96_ = MulDiv(height, 96, static_cast<int>(metrics_.dpi));
                            hasUserSized_ = true;
                        }
                    }
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;

            case WM_NCDESTROY:
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);

            case WM_APP_BRIDGE_TOGGLE:
                RequestToggle(L"Explorer bridge");
                return 0;

            case WM_APP_TOGGLE:
                Wh_Log(L"[StartMenu] WM_APP_TOGGLE received (visible=%d)", visible_ ? 1 : 0);
                Toggle(reinterpret_cast<HWND>(lParam));
                return 0;

            case WM_APP_HIDE:
                Hide();
                return 0;

            case WM_APP_REFRESH:
                // Posted by the Programs-folder watcher and by the stale-catalog
                // check. Defer while the menu is open so the lists don't change
                // under the user's cursor.
                if (visible_) { catalogRefreshPending_ = true; return 0; }
                RefreshCatalog();
                return 0;

            case WM_APP_REFRESH_ANCHOR:
                RefreshStartRects(false);
                return 0;

            case WM_DISPLAYCHANGE:
                PostMessageW(hwnd, WM_APP_REFRESH_ANCHOR, 0, 0);
                return 0;

            case WM_SETTINGCHANGE:
                if (wParam == SPI_SETWORKAREA) PostMessageW(hwnd, WM_APP_REFRESH_ANCHOR, 0, 0);
                return 0;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    int BottomPlaceItemCount() const {
        // The All apps / Back toggle and the search box are always visible,
        // as on the Update 3 menu (the reference implementation AppsToggle + SearchBorder).
        return 2;
    }

    // Named tile groups keep the Windows 8.1 group header strip; the default,
    // unnamed group of the Update 3 menu starts the tiles right below the pad.
    int GroupHeaderHeight(int groupIndex) const {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size()))
            return metrics_.Scale(Metrics::BASE_GROUP_HEADER_H);
        return layout_.groups[groupIndex].title.empty()
                   ? 0
                   : metrics_.Scale(Metrics::BASE_GROUP_HEADER_H);
    }

    // Flat Metro hover: a white veil over the darkened left column, where every
    // hoverable row lives (the reference implementation uses a 30% white overlay on buttons and
    // a lighter one on list rows).
    COLORREF HoverFill(float k) const {
        return BlendColor(ShadeColor(ResolveMenuBackgroundColor(settings_), -0.32f),
                          RGB(255, 255, 255), k);
    }


    // Hovered row veil; a row selected with the keyboard (and the preselected
    // best match in the search view) gets a slightly stronger flat veil, as on
    // Windows 8.1 list rows. No outline is drawn around list rows.
    COLORREF RowHoverFill() const { return HoverFill(keyboardNav_ ? 0.26f : 0.16f); }

    void InvalidateHoverTarget(int area, int index, int groupIndex) {
        if (!hwnd_) return;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int userH = ListAreaTopPx();
        const int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int bottomY = static_cast<int>(client.bottom) - BottomPlaceItemCount() * placeH;
        RECT dirty{};
        switch (area) {
            case 0: dirty = RECT{0, 0, leftW, userH}; break;
            case 7:
                if (!PowerButtonRect(dirty)) return;
                break;
            case 1:
                if (index < 0 || index > 1) return;
                dirty = RECT{0, bottomY + index * placeH, leftW, bottomY + (index + 1) * placeH};
                break;
            case 2: dirty = RECT{0, userH, leftW, bottomY}; break;
            case 3:
                if (!GetTileClientRect(groupIndex, index, dirty)) return;
                break;
            case 4: {
                if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
                const int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
                const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                    (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                const int groupX = startX + groupIndex * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
                dirty = RECT{groupX, metrics_.Scale(Metrics::BASE_GROUP_PAD_Y),
                    groupX + groupW, metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) + GroupHeaderHeight(groupIndex)};
                break;
            }
            default: return;
        }
        InvalidateRect(hwnd_.get(), &dirty, FALSE);
    }

    void OnMouseMove(int x, int y) {
        if (allAppsScrollDragging_ || mainListScrollDragging_) {
            const int travel = std::max(1, scrollDragTrackHeight_ - scrollDragThumbHeight_);
            const int thumbTop = std::max(scrollDragTrackTop_,
                std::min(y - scrollDragThumbOffset_, scrollDragTrackTop_ + travel));
            const int offset = thumbTop - scrollDragTrackTop_;
            const int newScroll = scrollDragMaxScroll_ > 0
                ? (offset * scrollDragMaxScroll_ + travel / 2) / travel : 0;
            if (mainListScrollDragging_ || viewMode_ == ViewMode::Search) mainListScrollY_ = newScroll;
            else allAppsScrollY_ = newScroll;
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }
        if (appDragCandidate_ && !appDragging_ &&
            (std::abs(x - dragStart_.x) > metrics_.Scale(6) || std::abs(y - dragStart_.y) > metrics_.Scale(6))) {
            appDragging_ = true;
        }
        if (appDragging_) {
            SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCEW(32649)));
            UpdateDragPreview(x, y);
        }

        if (tileDragCandidate_ && dragGroup_ >= 0 && dragGroup_ < static_cast<int>(layout_.groups.size()) &&
            dragTile_ >= 0 && dragTile_ < static_cast<int>(layout_.groups[dragGroup_].tiles.size())) {
            const int dragThreshold = metrics_.Scale(6);
            if (!tileDragging_ &&
                (std::abs(x - dragStart_.x) > dragThreshold || std::abs(y - dragStart_.y) > dragThreshold))
                tileDragging_ = true;
            if (tileDragging_) {
                // The lifted tile follows the pointer: repaint the area it is
                // leaving and the one it now covers, not the whole grid.
                RECT liftedBefore{};
                const bool hadLifted = GetTileDrawRect(dragGroup_, dragTile_, liftedBefore);
                dragPointer_ = POINT{x, y};
                EnsureTileAnimationTimer();
                RECT liftedNow{};
                const bool haveLifted = GetTileDrawRect(dragGroup_, dragTile_, liftedNow);
                if (hadLifted && haveLifted && hwnd_) {
                    RECT dirty{};
                    if (UnionRect(&dirty, &liftedBefore, &liftedNow)) {
                        InflateRect(&dirty, metrics_.Scale(10), metrics_.Scale(10));
                        InvalidateRect(hwnd_.get(), &dirty, FALSE);
                    }
                }
                const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
                const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                    (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
                const int cell = std::max(1, metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                                             metrics_.Scale(Metrics::BASE_TILE_GAP));
                const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) +
                    dragGroup_ * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
                const int tilesTop = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
                    GroupHeaderHeight(dragGroup_) + metrics_.Scale(4);
                // The target cell follows the tile as it is drawn, which the
                // clamp keeps inside the grid, rather than the bare pointer.
                const int wantedLeft = haveLifted ? static_cast<int>(liftedNow.left)
                                                  : x - dragTileGrabOffset_.x;
                const int wantedTop = haveLifted ? static_cast<int>(liftedNow.top)
                                                 : y - dragTileGrabOffset_.y;
                int movingW = 1, movingH = 1;
                GetTileFootprint(layout_.groups[dragGroup_].tiles[dragTile_].size, movingW, movingH);
                const int lastCol = Metrics::BASE_COLUMNS_PER_GROUP - movingW;
                // Ignore positions outside this group's snap area instead of forcing
                // the tile to an edge while the pointer crosses the menu or group gap.
                const int minSnapX = groupX - cell / 2;
                const int maxSnapX = groupX + lastCol * cell + cell / 2;
                if (lastCol >= 0 && wantedLeft >= minSnapX && wantedLeft <= maxSnapX &&
                    wantedTop >= tilesTop - cell / 2) {
                    const int wantedCol = std::max(0, std::min(lastCol,
                        (wantedLeft - groupX + cell / 2) / cell));
                    const int wantedRow = std::max(0, (wantedTop - tilesTop + cell / 2) / cell);
                    const Tile& moving = layout_.groups[dragGroup_].tiles[dragTile_];
                    if (moving.col != wantedCol || moving.row != wantedRow) {
                        // Remember where the tiles are painted so that the
                        // neighbours pushed aside by the drop glide to their new
                        // slot instead of appearing there.
                        const std::vector<POINT> beforeReflow = CaptureTileRenderPositions(dragGroup_);
                        if (ApplyDraggedTilePlacement(dragGroup_, dragTile_, wantedCol, wantedRow))
                            StartTileGlide(dragGroup_, beforeReflow);
                        RECT client{};
                        if (GetClientRect(hwnd_.get(), &client)) {
                            RECT tileViewport{leftW, 0, client.right, client.bottom};
                            InvalidateRect(hwnd_.get(), &tileViewport, FALSE);
                        }
                    }
                }
                return;
            }
        }
        if (keyboardNav_) {
            // The pointer wins over the keyboard: drop the focus ring even when
            // the pointer lands on the element that was focused.
            keyboardNav_ = false;
            InvalidateHoverTarget(hoverArea_, hoverIndex_, hoverGroup_);
        }

        int oldArea = hoverArea_;
        int oldIdx = hoverIndex_;
        int oldGrp = hoverGroup_;

        POINT pt{ x, y };
        HitTest(pt, hoverArea_, hoverIndex_, hoverGroup_);

        if (oldArea != hoverArea_ || oldIdx != hoverIndex_ || oldGrp != hoverGroup_) {
            InvalidateHoverTarget(oldArea, oldIdx, oldGrp);
            InvalidateHoverTarget(hoverArea_, hoverIndex_, hoverGroup_);
        }

        TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd_.get(), 0 };
        TrackMouseEvent(&tme);
    }

    void OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        int area = -1, idx = -1, grp = -1;
        HitTest(pt, area, idx, grp);
        ActivateItem(area, idx, grp, pt, false);
    }

    // Runs an element of the menu. Shared by the pointer (click) and by the
    // keyboard (Enter): `pt` only positions the flyouts.
    void ActivateItem(int area, int idx, int grp, POINT pt, bool /*explorerChevron*/) {
        if (!(area == 1 && idx == 1) && searchFocused_) {
            searchFocused_ = false;
            if (viewMode_ != ViewMode::Search) KillSearchCaretTimer();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
        if (area == 0) { // user button -> account menu
            ShowUserMenu(pt);
        } else if (area == 7) { // power button -> power menu
            ShowPowerMenu(pt);
        } else if (area == 1) { // All apps / Back toggle, and the search box
            if (idx == 0) { // All apps / Back
                SetViewMode(viewMode_ == ViewMode::Normal ? ViewMode::AllApps : ViewMode::Normal);
                if (viewMode_ != ViewMode::Search) KillSearchCaretTimer();
                // The list below changed: keep the focus on the row just used,
                // whose index is valid in both views.
                if (keyboardNav_) SetKeyboardFocus(1, 0, -1);
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (idx == 1) { // search box: focus it, Win7-style
                if (viewMode_ != ViewMode::Search) {
                    // Focus only: the view changes once something is typed.
                    searchAllSelected_ = false;
                    searchFocused_ = true;
                    EnsureSearchCaretTimer();
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                } else if (keyboardNav_ && !searchResults_.empty()) {
                    // Enter with the keyboard focus on the box opens the best
                    // match; a plain click just keeps the box focused.
                    LaunchCatalogEntry(searchResults_[0]);
                    return;
                }
                if (keyboardNav_) SetKeyboardFocus(1, 1, -1);
            }
        } else if (area == 2) { // App list / folders
            if (viewMode_ == ViewMode::Normal) {
                if (idx >= 0 && idx < (int)catalog_.MostUsed().size()) {
                    LaunchCatalogEntry(catalog_.MostUsed()[idx]);
                } else if (idx >= (int)catalog_.MostUsed().size() &&
                           idx < (int)(catalog_.MostUsed().size() + catalog_.RecentlyAdded().size())) {
                    int recentIdx = idx - (int)catalog_.MostUsed().size();
                    LaunchCatalogEntry(catalog_.RecentlyAdded()[recentIdx]);
                }
            } else if (viewMode_ == ViewMode::Search) {
                if (idx >= 0 && idx < (int)searchResults_.size()) {
                    LaunchCatalogEntry(searchResults_[idx]);
                }
            } else if (viewMode_ == ViewMode::AllApps) {
                HandleAllAppsClick(idx);
            }
        } else if (area == 3) { // Click on a tile
            if (grp >= 0 && grp < (int)layout_.groups.size()) {
                auto& group = layout_.groups[grp];
                if (idx >= 0 && idx < (int)group.tiles.size()) {
                    const auto& tile = group.tiles[idx];
                    Hide();
                    catalog_.RecordLaunch(tile.path.empty() ? tile.aumid : tile.path);
                    LaunchApplication(tile.path, tile.aumid);
                }
            }
        }
    }

    void OnRButtonUp(int x, int y) {
        POINT pt{ x, y };
        // The scrollbar is not a result row and must not open its context menu.
        if (viewMode_ == ViewMode::Search) {
            RECT track{}, thumb{}, up{}, down{}; int maxScroll=0;
            if (GetAllAppsScrollGeometry(track,thumb,maxScroll,&up,&down) &&
                (PtInRect(&track,pt) || PtInRect(&up,pt) || PtInRect(&down,pt))) return;
        }
        int area = -1, idx = -1, grp = -1;
        HitTest(pt, area, idx, grp);

        if (area == 0) { // Account button context menu
            ShowUserMenu(pt);
        } else if (area == 7) { // Power button context menu
            ShowPowerMenu(pt);
        } else if (area == 3 && grp >= 0 && idx >= 0) { // Tile context menu
            ShowTileContextMenu(pt, grp, idx);
        } else if (area == 2 && idx >= 0) { // App context menu
            ShowAppContextMenu(pt, idx);
        }
    }

    // Geometry is derived from the live client area, not the default menu size;
    // this keeps scrolling correct with DPI changes, short work areas and resizing.
    bool GetMainListScrollGeometry(RECT& track, RECT& thumb, int& maxScroll) const {
        track = {}; thumb = {}; maxScroll = 0;
        if ((viewMode_ != ViewMode::Normal && viewMode_ != ViewMode::Search) || !hwnd_) return false;
        RECT client{}; if (!GetClientRect(hwnd_.get(), &client)) return false;
        const int top = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int bottom = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int visible = bottom - top;
        if (visible <= 0 || totalMainListH_ <= visible) return false;
        const int edge = std::min(static_cast<int>(client.right), metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int width = std::max(4, metrics_.Scale(6));
        track = {std::max(0,edge-width),top,edge,bottom};
        const int trackH = track.bottom-track.top;
        const int thumbH = std::min(trackH,std::max(metrics_.Scale(20),trackH*visible/totalMainListH_));
        maxScroll=totalMainListH_-visible;
        const int travel=trackH-thumbH;
        const int thumbY=track.top+(maxScroll ? travel*std::max(0,std::min(mainListScrollY_,maxScroll))/maxScroll : 0);
        thumb={track.left,thumbY,track.right,thumbY+thumbH};
        return true;
    }

    // Scrollbar palette: a white veil over the menu background, so the bars
    // stay readable on the accent-colored Start background as well.
    COLORREF ScrollRailColor() const { return BlendColor(ResolveMenuBackgroundColor(settings_), RGB(255, 255, 255), 0.12f); }
    COLORREF ScrollThumbColor() const { return BlendColor(ResolveMenuBackgroundColor(settings_), RGB(255, 255, 255), 0.38f); }
    COLORREF ScrollThumbHotColor() const { return BlendColor(ResolveMenuBackgroundColor(settings_), RGB(255, 255, 255), 0.60f); }
    COLORREF ScrollArrowColor() const { return BlendColor(ResolveMenuBackgroundColor(settings_), RGB(255, 255, 255), 0.60f); }
    COLORREF ScrollArrowDimColor() const { return BlendColor(ResolveMenuBackgroundColor(settings_), RGB(255, 255, 255), 0.30f); }
    COLORREF ScrollArrowHotColor() const { return RGB(255, 255, 255); }

    // Chevron sampled from the supplied 9841 screenshot: a 7 by 6 pixel
    // outline, scaled with DPI; the source screenshot is compressed.
    void DrawAllAppsScrollChevron(HDC hdc, const RECT& button, bool up, COLORREF color) {
        const float u=static_cast<float>(metrics_.dpi ? metrics_.dpi : 96)/96.0f;
        const float cx=(static_cast<float>(button.left)+button.right)*0.5f;
        const float cy=(static_cast<float>(button.top)+button.bottom)*0.5f;
        if (g_gdiplusToken) {
            Gdiplus::Graphics g(hdc);
            if (g.GetLastStatus()==Gdiplus::Ok) {
                g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                Gdiplus::Pen pen(Gdiplus::Color(255,GetRValue(color),GetGValue(color),GetBValue(color)),std::max(1.0f,u));
                Gdiplus::PointF pts[3]={{cx-3*u,cy+(up?1.5f:-1.5f)*u},
                    {cx,cy+(up?-1.5f:1.5f)*u},{cx+3*u,cy+(up?1.5f:-1.5f)*u}};
                g.DrawLines(&pen,pts,3); return;
            }
        }
        UniquePen pen(CreatePen(PS_SOLID,std::max(1,metrics_.Scale(1)),color));
        if (!pen) return;
        ScopedSelectObject selected(hdc,pen.get());
        int x=static_cast<int>(cx),y=static_cast<int>(cy),d=std::max(2,metrics_.Scale(3));
        MoveToEx(hdc,x-d,y+(up?d/2:-d/2),nullptr); LineTo(hdc,x,y+(up?-d/2:d/2));
        LineTo(hdc,x+d,y+(up?d/2:-d/2));
    }

    void DrawScrollThumb(HDC hdc, RECT thumb, bool hot, bool lightStyle = false) {
        // Leave a 1 px gutter on both sides so the thumb never touches the
        // column border or the list text.
        const int gutter = lightStyle ? 0 : std::max(1, metrics_.Scale(1));
        InflateRect(&thumb, -gutter, 0);
        if (thumb.right <= thumb.left || thumb.bottom <= thumb.top) return;
        // lightStyle: the Win7 All Programs scrollbar (light rail, gray thumb).
        UniqueBrush brush(CreateSolidBrush(lightStyle
            ? (hot ? RGB(120, 120, 120) : RGB(205, 205, 205))
            : (hot ? ScrollThumbHotColor() : ScrollThumbColor())));
        FillRect(hdc, &thumb, brush.get());
    }

    // Colors of the All-apps scrollbar, Win7 All Programs style: a light rail
    // on the darkened left column with a gray thumb and dark arrows, exactly
    // as in the leaked Update 3 photos.
    COLORREF AllAppsSbRailColor() const { return RGB(241, 240, 240); }
    COLORREF AllAppsSbArrowColor() const { return RGB(85, 85, 85); }
    COLORREF AllAppsSbArrowDimColor() const { return RGB(160, 160, 160); }
    COLORREF AllAppsSbArrowHotColor() const { return RGB(0, 0, 0); }

    void DrawMainListScrollbar(HDC hdc, const RECT&) {
        RECT track{},thumb{}; int maxScroll=0;
        if (!GetMainListScrollGeometry(track,thumb,maxScroll)) return;
        UniqueBrush rail(CreateSolidBrush(ScrollRailColor()));
        FillRect(hdc,&track,rail.get());
        DrawScrollThumb(hdc, thumb, mainListScrollDragging_);
    }

    bool BeginMainListScroll(int x, int y) {
        RECT track{},thumb{}; int maxScroll=0;
        if (!GetMainListScrollGeometry(track,thumb,maxScroll)) return false;
        if (x < track.left-metrics_.Scale(10) || x >= track.right+metrics_.Scale(2) || y < track.top || y >= track.bottom) return false;
        if (y >= thumb.top && y < thumb.bottom) {
            mainListScrollDragging_=true; scrollDragTrackTop_=track.top;
            scrollDragTrackHeight_=track.bottom-track.top; scrollDragThumbHeight_=thumb.bottom-thumb.top;
            scrollDragThumbOffset_=y-thumb.top; scrollDragMaxScroll_=maxScroll; SetCapture(hwnd_.get());
        } else {
            const int page=std::max(metrics_.Scale(32),static_cast<int>(track.bottom-track.top));
            mainListScrollY_=std::max(0,std::min(mainListScrollY_+(y<thumb.top?-page:page),maxScroll));
            InvalidateRect(hwnd_.get(),nullptr,FALSE);
        }
        return true;
    }

    bool GetAllAppsScrollGeometry(RECT& track, RECT& thumb, int& maxScroll,
                                  RECT* upButton = nullptr, RECT* downButton = nullptr) const {
        track = {}; thumb = {}; maxScroll = 0;
        if (upButton) *upButton = {};
        if (downButton) *downButton = {};
        if ((viewMode_ != ViewMode::AllApps && viewMode_ != ViewMode::Search) || !hwnd_) return false;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return false;
        // In the narrow All-apps view the list reaches the top of the menu;
        // there is no account header row.
        const int listTop = ListAreaTopPx();
        const int bottomY = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int visibleH = bottomY - listTop;
        const int contentH = std::max(0, viewMode_ == ViewMode::Search ? totalMainListH_ : totalAllAppsH_);
        if (visibleH <= 0 || contentH <= visibleH) return false;
        // Win7 All Programs scrollbar: 17 px rail with square 17 px arrow
        // buttons at both ends.
        const int sbW = std::max(14, metrics_.Scale(Metrics::BASE_ALLAPPS_SB_W));
        const int buttonH = std::min(sbW, std::max(1, visibleH / 4));
        const int leftEdge = std::min(static_cast<int>(client.right), metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int trackTop = listTop + buttonH;
        const int trackBottom = bottomY - buttonH;
        track = { std::max(0, leftEdge - sbW), trackTop, leftEdge, trackBottom };
        const int trackH = track.bottom - track.top;
        if (trackH <= 0) return false;
        if (upButton) *upButton = { std::max(0, leftEdge - sbW), listTop, leftEdge, trackTop };
        if (downButton) *downButton = { std::max(0, leftEdge - sbW), trackBottom, leftEdge, bottomY };
        const int thumbH = std::min(trackH, std::max(metrics_.Scale(20), trackH * visibleH / contentH));
        maxScroll = contentH - visibleH;
        const int boundedScroll = std::max(0, std::min(viewMode_ == ViewMode::Search ? mainListScrollY_ : allAppsScrollY_, maxScroll));
        const int travel = trackH - thumbH;
        const int thumbY = track.top + (maxScroll > 0 ? (travel * boundedScroll / maxScroll) : 0);
        thumb = { track.left, thumbY, track.right, thumbY + thumbH };
        return true;
    }

    void StopAllAppsArrowRepeat(bool releaseCapture) {
        if (hwnd_) KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
        const bool wasPressed = allAppsScrollArrowPressed_;
        allAppsScrollRepeatDirection_ = 0;
        allAppsScrollArrowPressed_ = false;
        if (releaseCapture && hwnd_ && GetCapture() == hwnd_.get()) ReleaseCapture();
        if (wasPressed && hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE); // clear the pressed arrow highlight
    }

    void ScrollAllAppsArrowStep() {
        if (!allAppsScrollRepeatDirection_) return;
        RECT track{}, thumb{}; int maxScroll = 0;
        if (!GetAllAppsScrollGeometry(track, thumb, maxScroll)) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            allAppsScrollRepeatDirection_ = 0;
            return;
        }
        int& scroll = viewMode_ == ViewMode::Search ? mainListScrollY_ : allAppsScrollY_;
        const int oldScroll = scroll;
        const int step = std::max(1, metrics_.Scale(32));
        if (allAppsScrollRepeatDirection_ < 0)
            scroll = std::max(0, scroll - step);
        else
            scroll = std::min(maxScroll, scroll + step);
        if (scroll == oldScroll) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            allAppsScrollRepeatDirection_ = 0;
            return;
        }
        InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    void BeginAllAppsArrowRepeat(int direction) {
        allAppsScrollArrowPressed_ = true;
        allAppsScrollRepeatDirection_ = direction;
        SetCapture(hwnd_.get());
        ScrollAllAppsArrowStep(); // immediate first line, followed by standard held-button repeat
        if (allAppsScrollRepeatDirection_ &&
            !SetTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT, 400, nullptr)) {
            Wh_Log(L"[AllApps] scrollbar arrow repeat timer unavailable; held repeat disabled");
            allAppsScrollRepeatDirection_ = 0;
        }
    }

    void OnAllAppsArrowRepeatTimer() {
        if (!allAppsScrollArrowPressed_ || !allAppsScrollRepeatDirection_) {
            KillTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT);
            return;
        }
        ScrollAllAppsArrowStep();
        if (allAppsScrollRepeatDirection_ &&
            !SetTimer(hwnd_.get(), TIMER_ALL_APPS_SCROLL_REPEAT, 50, nullptr)) {
            Wh_Log(L"[AllApps] scrollbar arrow repeat timer stopped");
            allAppsScrollRepeatDirection_ = 0;
        }
    }

    bool BeginAllAppsScroll(int x, int y) {
        RECT track{}, thumb{}, upButton{}, downButton{}; int maxScroll = 0;
        if (!GetAllAppsScrollGeometry(track, thumb, maxScroll, &upButton, &downButton)) return false;
        const int hitLeft = std::min(static_cast<int>(upButton.left), static_cast<int>(track.left)) - metrics_.Scale(10);
        if (x < hitLeft || x >= track.right + metrics_.Scale(2)) return false;
        const POINT point{ x, y };
        if (PtInRect(&upButton, point)) {
            BeginAllAppsArrowRepeat(-1);
            return true;
        }
        if (PtInRect(&downButton, point)) {
            BeginAllAppsArrowRepeat(1);
            return true;
        }
        if (y < track.top || y >= track.bottom) return false;
        if (y >= thumb.top && y < thumb.bottom) {
            allAppsScrollDragging_ = true;
            scrollDragTrackTop_ = track.top;
            scrollDragTrackHeight_ = track.bottom - track.top;
            scrollDragThumbHeight_ = thumb.bottom - thumb.top;
            scrollDragThumbOffset_ = y - thumb.top;
            scrollDragMaxScroll_ = maxScroll;
            SetCapture(hwnd_.get());
        } else {
            const int page = std::max(metrics_.Scale(32), static_cast<int>(track.bottom - track.top));
            int& scroll = viewMode_ == ViewMode::Search ? mainListScrollY_ : allAppsScrollY_;
            scroll = std::max(0, std::min(scroll + (y < thumb.top ? -page : page), maxScroll));
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
        return true;
    }

    void OnMouseWheel(int delta, POINT clientPoint) {
        if (viewMode_ != ViewMode::AllApps && viewMode_ != ViewMode::Normal && viewMode_ != ViewMode::Search) return;
        RECT client{};
        if (!GetClientRect(hwnd_.get(), &client)) return;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int userH = ListAreaTopPx();
        const int bottomY = client.bottom - BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        if (clientPoint.x < 0 || clientPoint.x >= leftW || clientPoint.y < userH || clientPoint.y >= bottomY) return;
        const int visibleH = bottomY - userH;
        const bool allApps = viewMode_ == ViewMode::AllApps;
        const int maxScroll = std::max(0, (allApps ? totalAllAppsH_ : totalMainListH_) - visibleH);
        if (maxScroll == 0) return;
        const int notches = delta / WHEEL_DELTA;
        if (notches == 0) return;
        if (allApps) allAppsScrollY_ = std::max(0, std::min(allAppsScrollY_ - notches * metrics_.Scale(40), maxScroll));
        else mainListScrollY_ = std::max(0, std::min(mainListScrollY_ - notches * metrics_.Scale(40), maxScroll));
        InvalidateRect(hwnd_.get(), nullptr, FALSE);
    }

    void OnChar(wchar_t ch) {
        if (ch >= 32 && ch != 127) {
            if (searchAllSelected_) searchQuery_.clear();
            searchAllSelected_ = false;
            searchFocused_ = true;
            searchQuery_.push_back(ch);
            mainListScrollY_ = 0;
            SetViewMode(ViewMode::Search);
            EnsureSearchCaretTimer();
            FilterSearch();
            // As in the original menu, the best match is preselected so that
            // Enter launches it and the arrow keys walk the other results.
            FocusFirstSearchResult();
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    void OnKeyDown(UINT vk) {
        const bool control = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (vk == 'A' && control && !searchQuery_.empty()) {
            viewMode_ = ViewMode::Search;
            searchAllSelected_ = true;
            InvalidateRect(hwnd_.get(), nullptr, FALSE);
            return;
        }

        switch (vk) {
            case VK_ESCAPE:
                searchAllSelected_ = false;
                ClearKeyboardFocus();
                if (viewMode_ == ViewMode::Search) {
                    searchQuery_.clear();
                    searchResults_.clear();
                    SetViewMode(ViewMode::Normal);
                    KillSearchCaretTimer();
                    mainListScrollY_ = 0;
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                } else {
                    Hide();
                }
                return;

            case VK_BACK:
                if (viewMode_ == ViewMode::Search && !searchQuery_.empty()) {
                    if (searchAllSelected_) {
                        searchQuery_.clear();
                        searchResults_.clear();
                        searchAllSelected_ = false;
                        SetViewMode(ViewMode::Normal);
                        KillSearchCaretTimer();
                        ClearKeyboardFocus();
                        InvalidateRect(hwnd_.get(), nullptr, FALSE);
                        return;
                    }
                    searchQuery_.pop_back();
                    searchAllSelected_ = false;
                    if (searchQuery_.empty()) {
                        SetViewMode(ViewMode::Normal);
                        KillSearchCaretTimer();
                        ClearKeyboardFocus();
                    } else {
                        FilterSearch();
                        FocusFirstSearchResult();
                    }
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                }
                return;

            case VK_UP:    MoveFocusVertical(-1); return;
            case VK_DOWN:  MoveFocusVertical(1); return;
            case VK_LEFT:  MoveFocusHorizontal(-1); return;
            case VK_RIGHT: MoveFocusHorizontal(1); return;
            case VK_PRIOR: MoveFocusPage(-1); return;
            case VK_NEXT:  MoveFocusPage(1); return;
            case VK_HOME:  MoveFocusToEdge(false); return;
            case VK_END:   MoveFocusToEdge(true); return;
            case VK_TAB:   MoveFocusToNextArea(shift ? -1 : 1); return;

            case VK_RETURN:
                ActivateKeyboardFocus();
                return;

            case VK_APPS:
                ShowKeyboardContextMenu();
                return;

            case VK_F10:
                if (shift) ShowKeyboardContextMenu();
                return;

            default:
                return;
        }
    }

    // ===================== KEYBOARD NAVIGATION =====================
    // Arrow keys, Tab, Home/End and Page Up/Down move a single highlight over
    // the account header (area 0), the bottom bar (area 1), the app list
    // (area 2), the tile grid (area 3) and the A-Z grid of All apps (area 6).
    // The highlight is stored in the hover fields, so the existing painting
    // code highlights a focused element exactly like a hovered one; only the
    // focus ring is added on top of it.

    int ListItemHeight() const {
        // Compact Win7-style rows in the All-apps list, regular 40 px rows in
        // the Most used and Search lists.
        return viewMode_ == ViewMode::AllApps ? metrics_.Scale(Metrics::BASE_ALLAPPS_ITEM_H)
                                              : metrics_.Scale(Metrics::BASE_APP_ITEM_H);
    }

    // Top of the scrollable list in client coordinates: the account header
    // row, or 2 px below the menu's top edge in the narrow All-apps view
    // where there is no header at all (as in the leaked Update 3 photos).
    int ListAreaTopPx() const {
        return viewMode_ == ViewMode::AllApps ? metrics_.Scale(2)
                                              : metrics_.Scale(Metrics::BASE_USER_HEADER_H);
    }

    bool AllAppsNarrow() const { return viewMode_ == ViewMode::AllApps; }

    // Every view transition goes through here so the window always matches
    // the view: narrow (273 px) while the All-apps list is open, wide (tiles)
    // otherwise.
    void SetViewMode(ViewMode mode) {
        if (viewMode_ == mode) return;
        StopAllAppsArrowRepeat(true);
        allAppsScrollDragging_ = false;
        mainListScrollDragging_ = false;
        viewMode_ = mode;
        ApplyViewWindowSize();
    }

    // ---- flat All-apps row cache (Win7 All Programs order) ----
    // One entry per visible top-level row: a loose app or a folder, merged
    // alphabetically inside each letter section. Expanded folder children are
    // walked on the fly, so expanding a folder never rebuilds this cache; it
    // is only rebuilt when the catalog itself is replaced (RefreshCatalog).
    struct AllAppsRow {
        bool isFolder = false;
        int section = 0; // index into catalog_.Sections()
        int index = 0;   // app index in section.apps, or folder index in section.folders
    };

    void EnsureAllAppsRows() const {
        if (!allAppsRowsDirty_) return;
        allAppsRows_.clear();
        const auto& sections = catalog_.Sections();
        for (size_t si = 0; si < sections.size(); ++si) {
            const auto& sec = sections[si];
            std::vector<std::pair<std::wstring, AllAppsRow>> merged;
            merged.reserve(sec.apps.size() + sec.folders.size());
            for (size_t ai = 0; ai < sec.apps.size(); ++ai)
                merged.push_back({sec.apps[ai].name, AllAppsRow{false, (int)si, (int)ai}});
            for (size_t fi = 0; fi < sec.folders.size(); ++fi)
                merged.push_back({sec.folders[fi].name, AllAppsRow{true, (int)si, (int)fi}});
            std::stable_sort(merged.begin(), merged.end(),
                [](const auto& a, const auto& b) { return _wcsicmp(a.first.c_str(), b.first.c_str()) < 0; });
            for (auto& entry : merged) allAppsRows_.push_back(entry.second);
        }
        allAppsRowsDirty_ = false;
    }

    mutable std::vector<AllAppsRow> allAppsRows_;
    mutable bool allAppsRowsDirty_ = true;

    // Walks the rows of the left column that the keyboard can reach. The
    // callback receives (index, top), where `top` is measured from the top of
    // the scrollable area before the scroll offset is applied, and returns
    // false to stop the walk. One extra call is made past the last row, so the
    // same walk also yields the row count and the total content height.
    template <typename Callback>
    void ForEachListRow(Callback callback) const {
        const int itemH = ListItemHeight();
        int index = 0;
        int top = 0;
        if (viewMode_ == ViewMode::Normal) {
            top = metrics_.Scale(Metrics::BASE_FIRST_LIST_INSET);
            for (size_t i = 0; i < catalog_.MostUsed().size(); ++i) {
                if (!callback(index++, top)) return;
                top += itemH;
            }
            if (!catalog_.RecentlyAdded().empty()) {
                top += metrics_.Scale(Metrics::BASE_LIST_SECTION_GAP);
                for (size_t i = 0; i < catalog_.RecentlyAdded().size(); ++i) {
                    if (!callback(index++, top)) return;
                    top += itemH;
                }
            }
        } else if (viewMode_ == ViewMode::Search) {
            top = metrics_.Scale(8); // DrawSearchResults starts the rows at +8, with no heading
            for (size_t i = 0; i < searchResults_.size(); ++i) {
                if (!callback(index++, top)) return;
                top += itemH;
            }
        } else if (viewMode_ == ViewMode::AllApps) {
            // Flat alphabetical list (the Win7 All Programs arrangement):
            // folders interleaved with the loose apps, no letter headers. The
            // expanded folder children follow their folder row.
            EnsureAllAppsRows();
            const auto& sections = catalog_.Sections();
            for (const auto& row : allAppsRows_) {
                if (!callback(index++, top)) return;
                top += itemH;
                if (!row.isFolder) continue;
                const auto& folder = sections[row.section].folders[row.index];
                if (!folder.expanded) continue;
                for (size_t sub = 0; sub < folder.apps.size(); ++sub) {
                    if (!callback(index++, top)) return;
                    top += itemH;
                }
            }
        }
        callback(index, top); // one past the last row
    }

    int ListItemCount() const {
        int count = 0;
        ForEachListRow([&](int index, int) { count = index; return true; });
        return count;
    }

    int ListContentHeight() const {
        int height = 0;
        ForEachListRow([&](int, int top) { height = top; return true; });
        return height;
    }

    int ListItemOffset(int index) const {
        int offset = 0;
        ForEachListRow([&](int current, int top) {
            if (current != index) return true;
            offset = top;
            return false;
        });
        return offset;
    }

    // Row closest to a position expressed in content coordinates.
    int ListIndexAtOffset(int contentY) const {
        const int itemH = ListItemHeight();
        const int count = ListItemCount();
        int best = -1;
        int bestDistance = INT_MAX;
        ForEachListRow([&](int index, int top) {
            const int distance = std::abs(top + itemH / 2 - contentY);
            if (distance < bestDistance) { bestDistance = distance; best = index; }
            return true;
        });
        if (best >= count) best = count - 1; // the past-the-end call
        return best;
    }

    bool ListViewportRect(RECT& out) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        const int leftW = std::min(static_cast<int>(client.right),
                                   metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int userH = ListAreaTopPx();
        const int bottomY = static_cast<int>(client.bottom) -
                            BottomPlaceItemCount() * metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        if (leftW <= 0 || bottomY <= userH) return false;
        out = RECT{0, userH, leftW, bottomY};
        return true;
    }

    int CurrentListScroll() const {
        return viewMode_ == ViewMode::AllApps ? allAppsScrollY_ : mainListScrollY_;
    }

    void SetCurrentListScroll(int value) {
        if (viewMode_ == ViewMode::AllApps) allAppsScrollY_ = value;
        else mainListScrollY_ = value;
    }

    bool ListItemRect(int index, RECT& out) const {
        RECT viewport{};
        if (index < 0 || index >= ListItemCount() || !ListViewportRect(viewport)) return false;
        const int top = static_cast<int>(viewport.top) + ListItemOffset(index) - CurrentListScroll();
        out = RECT{viewport.left, top, viewport.right, top + ListItemHeight()};
        return true;
    }

    // Scrolls by the smallest amount that brings a row fully into view.
    void EnsureListItemVisible(int index) {
        RECT viewport{};
        if (index < 0 || !ListViewportRect(viewport)) return;
        const int visibleH = static_cast<int>(viewport.bottom - viewport.top);
        const int maxScroll = std::max(0, ListContentHeight() - visibleH);
        const int top = ListItemOffset(index);
        const int bottom = top + ListItemHeight();
        const int current = CurrentListScroll();
        int scroll = current;
        if (top < scroll) {
            // Scrolling upwards also reveals the heading that sits above the
            // row ("Most used", "Recently added", or the A-Z letter).
            scroll = index > 0 ? ListItemOffset(index - 1) + ListItemHeight() : 0;
        } else if (bottom > scroll + visibleH) {
            scroll = bottom - visibleH;
        }
        scroll = std::max(0, std::min(scroll, maxScroll));
        if (scroll != current) {
            SetCurrentListScroll(scroll);
            if (hwnd_) InvalidateRect(hwnd_.get(), nullptr, FALSE);
        }
    }

    bool PlaceItemRect(int index, RECT& out) const {
        RECT client{};
        if (index < 0 || index > 1 || !hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        const int leftW = std::min(static_cast<int>(client.right),
                                   metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        const int bottomY = static_cast<int>(client.bottom) - BottomPlaceItemCount() * placeH;
        out = RECT{0, bottomY + index * placeH, leftW, bottomY + (index + 1) * placeH};
        return true;
    }

    // Client rectangle of a tile, shared by hit testing, repainting and
    // keyboard navigation so that all three always agree.
    bool GetTileClientRect(int groupIndex, int tileIndex, RECT& out) const {
        if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return false;
        const auto& tiles = layout_.groups[groupIndex].tiles;
        if (tileIndex < 0 || tileIndex >= static_cast<int>(tiles.size())) return false;
        const int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
            (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X) +
            groupIndex * (groupW + metrics_.Scale(Metrics::BASE_GROUP_GAP));
        const int tilesY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y) +
            GroupHeaderHeight(groupIndex) + metrics_.Scale(4);
        out = ComputeTileRect(groupX, tilesY, tiles[tileIndex]);
        return true;
    }

    // A tile scrolled out of the grid viewport must not receive the focus.
    bool IsTileRectReachable(const RECT& tile) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        RECT viewport{metrics_.Scale(Metrics::BASE_LEFT_COL_W), 0, client.right, client.bottom};
        RECT intersection{};
        return IntersectRect(&intersection, &tile, &viewport) != FALSE;
    }

    bool FocusTargetRect(int area, int index, int group, RECT& out) const {
        RECT client{};
        switch (area) {
            case 0: {
                if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
                if (AllAppsNarrow()) return false; // no account row in the All-apps view
                const int right = std::min(static_cast<int>(client.right),
                                           metrics_.Scale(Metrics::BASE_LEFT_COL_W)) -
                                  metrics_.Scale(44);
                out = RECT{0, 0, std::max(1, right),
                           metrics_.Scale(Metrics::BASE_USER_HEADER_H)};
                return true;
            }
            case 1: return PlaceItemRect(index, out);
            case 7: return PowerButtonRect(out);
            case 2: return ListItemRect(index, out);
            case 3: return GetTileClientRect(group, index, out);
            default: return false;
        }
    }

    void SetKeyboardFocus(int area, int index, int group) {
        const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
        hoverArea_ = area;
        hoverIndex_ = index;
        hoverGroup_ = group;
        keyboardNav_ = area >= 0;
        if (area == 3) {
            keyboardTileGroup_ = group;
            keyboardTileIndex_ = index;
        }
        if (area == 2) EnsureListItemVisible(index);
        InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
        InvalidateHoverTarget(area, index, group);
    }

    void ClearKeyboardFocus() {
        if (!keyboardNav_) return;
        const int oldArea = hoverArea_, oldIndex = hoverIndex_, oldGroup = hoverGroup_;
        keyboardNav_ = false;
        hoverArea_ = hoverIndex_ = hoverGroup_ = -1;
        InvalidateHoverTarget(oldArea, oldIndex, oldGroup);
    }

    void ResetKeyboardNavigation() {
        keyboardNav_ = false;
        keyboardTileGroup_ = -1;
        keyboardTileIndex_ = -1;
    }

    void FocusFirstSearchResult() {
        if (!searchResults_.empty()) SetKeyboardFocus(2, 0, -1);
        else ClearKeyboardFocus();
    }

    int FirstPlaceIndex() const { return 0; }

    bool FocusPlace(int index) {
        if (index < 0 || index > 1) return false;
        SetKeyboardFocus(1, index, -1);
        return true;
    }

    bool FocusListRow(int index) {
        const int count = ListItemCount();
        if (count <= 0) return false;
        SetKeyboardFocus(2, std::max(0, std::min(index, count - 1)), -1);
        return true;
    }


    // ---- tile grid ----
    // Picks the reachable tile that best continues the requested direction:
    // the closest one along the axis of travel, with lateral drift penalised.
    bool MoveTileFocus(int dx, int dy) {
        RECT currentRect{};
        if (hoverArea_ != 3 || !GetTileClientRect(hoverGroup_, hoverIndex_, currentRect))
            return FocusTileFromColumn();
        const int fromX = static_cast<int>(currentRect.left + currentRect.right) / 2;
        const int fromY = static_cast<int>(currentRect.top + currentRect.bottom) / 2;
        int bestGroup = -1, bestTile = -1;
        long long bestScore = 0;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (static_cast<int>(gi) == hoverGroup_ && static_cast<int>(ti) == hoverIndex_) continue;
                RECT rect{};
                if (!GetTileClientRect(static_cast<int>(gi), static_cast<int>(ti), rect)) continue;
                if (!IsTileRectReachable(rect)) continue;
                const int centerX = static_cast<int>(rect.left + rect.right) / 2;
                const int centerY = static_cast<int>(rect.top + rect.bottom) / 2;
                const long long along = dx != 0 ? static_cast<long long>(centerX - fromX) * dx
                                                : static_cast<long long>(centerY - fromY) * dy;
                if (along <= 0) continue; // not in the requested direction
                const long long across = dx != 0 ? std::abs(centerY - fromY) : std::abs(centerX - fromX);
                const long long score = along + across * 3;
                if (bestTile < 0 || score < bestScore) {
                    bestScore = score;
                    bestGroup = static_cast<int>(gi);
                    bestTile = static_cast<int>(ti);
                }
            }
        }
        if (bestTile >= 0) {
            SetKeyboardFocus(3, bestTile, bestGroup);
            return true;
        }
        if (dx < 0) return FocusLeftColumnFromTile(); // leaving the grid on its left
        return false;
    }

    // Enters the tile grid from the left column, keeping the vertical position
    // or, when available, the tile the grid was last left from.
    bool FocusTileFromColumn() {
        RECT remembered{};
        if (GetTileClientRect(keyboardTileGroup_, keyboardTileIndex_, remembered) &&
            IsTileRectReachable(remembered)) {
            SetKeyboardFocus(3, keyboardTileIndex_, keyboardTileGroup_);
            return true;
        }
        RECT origin{};
        const int wantedY = FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, origin)
            ? static_cast<int>(origin.top + origin.bottom) / 2 : 0;
        int bestGroup = -1, bestTile = -1;
        long long bestScore = 0;
        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            const auto& tiles = layout_.groups[gi].tiles;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                RECT rect{};
                if (!GetTileClientRect(static_cast<int>(gi), static_cast<int>(ti), rect)) continue;
                if (!IsTileRectReachable(rect)) continue;
                const int centerY = static_cast<int>(rect.top + rect.bottom) / 2;
                const long long score =
                    static_cast<long long>(std::abs(centerY - wantedY)) * 4 + rect.left;
                if (bestTile < 0 || score < bestScore) {
                    bestScore = score;
                    bestGroup = static_cast<int>(gi);
                    bestTile = static_cast<int>(ti);
                }
            }
        }
        if (bestTile < 0) return false;
        SetKeyboardFocus(3, bestTile, bestGroup);
        return true;
    }

    // Returns from the tile grid to the row of the left column at the same height.
    bool FocusLeftColumnFromTile() {
        RECT tileRect{};
        int wantedY = -1;
        if (GetTileClientRect(hoverGroup_, hoverIndex_, tileRect))
            wantedY = static_cast<int>(tileRect.top + tileRect.bottom) / 2;
        RECT viewport{};
        if (ListItemCount() > 0 && ListViewportRect(viewport)) {
            int index = 0;
            if (wantedY >= 0) {
                const int clampedY = std::max(static_cast<int>(viewport.top),
                    std::min(wantedY, static_cast<int>(viewport.bottom) - 1));
                index = std::max(0, ListIndexAtOffset(clampedY - static_cast<int>(viewport.top) +
                                                      CurrentListScroll()));
            }
            return FocusListRow(index);
        }
        return FocusPlace(FirstPlaceIndex());
    }

    // ---- movement ----
    // First target when navigation starts without a highlight.
    void FocusFirstTarget(int direction) {
        const int count = ListItemCount();
        if (count > 0) {
            SetKeyboardFocus(2, direction < 0 ? count - 1 : 0, -1);
            return;
        }
        if (!FocusPlace(direction < 0 ? 1 : FirstPlaceIndex())) FocusTileFromColumn();
    }

    void MoveFocusVertical(int delta) {
        if (!keyboardNav_ || hoverArea_ < 0) {
            FocusFirstTarget(delta);
            return;
        }
        switch (hoverArea_) {
            case 0: // account button: down to the list, up to the power button
                if (delta > 0) {
                    if (!FocusListRow(0)) FocusPlace(FirstPlaceIndex());
                } else {
                    SetKeyboardFocus(7, -1, -1);
                }
                return;
            case 7: // power button: only back down to the account button
                if (delta > 0) SetKeyboardFocus(0, -1, -1);
                return;
            case 1: {
                const int next = hoverIndex_ + delta;
                if (next >= 0 && next <= 1) {
                    SetKeyboardFocus(1, next, -1);
                    return;
                }
                if (delta < 0) {
                    const int count = ListItemCount();
                    if (count > 0) FocusListRow(count - 1);
                    else SetKeyboardFocus(0, -1, -1);
                }
                return;
            }
            case 2: {
                const int count = ListItemCount();
                const int next = hoverIndex_ + delta;
                if (next >= 0 && next < count) {
                    SetKeyboardFocus(2, next, -1);
                    return;
                }
                if (delta < 0 && !AllAppsNarrow()) SetKeyboardFocus(0, -1, -1); // above the list: account header
                else if (delta < 0) FocusPlace(1);                            // narrow view: search box
                else FocusPlace(FirstPlaceIndex());                           // below the list: bottom bar
                return;
            }
            case 3: MoveTileFocus(0, delta); return;
            default: FocusFirstTarget(delta); return;
        }
    }

    void MoveFocusHorizontal(int delta) {
        if (!keyboardNav_ || hoverArea_ < 0) {
            if (delta > 0 && FocusTileFromColumn()) return;
            FocusFirstTarget(1);
            return;
        }
        switch (hoverArea_) {
            case 0:
            case 7:
            case 1:
            case 2:
                if (delta > 0) FocusTileFromColumn(); // step into the tile grid
                return;
            case 3: MoveTileFocus(delta, 0); return;
            default: return;
        }
    }

    void MoveFocusPage(int delta) {
        if (keyboardNav_ && hoverArea_ == 3) { // in the grid a page is one row
            MoveTileFocus(0, delta);
            return;
        }
        RECT viewport{};
        const int count = ListItemCount();
        if (count <= 0 || !ListViewportRect(viewport)) {
            MoveFocusVertical(delta);
            return;
        }
        const int rows = std::max(1, static_cast<int>(viewport.bottom - viewport.top) /
                                         std::max(1, ListItemHeight()) - 1);
        const int current = (keyboardNav_ && hoverArea_ == 2) ? hoverIndex_
                                                              : (delta > 0 ? -1 : count);
        const int next = std::max(0, std::min(current + delta * rows, count - 1));
        SetKeyboardFocus(2, next, -1);
    }

    void MoveFocusToEdge(bool last) {
        if (keyboardNav_ && hoverArea_ == 3 && hoverGroup_ >= 0 &&
            hoverGroup_ < static_cast<int>(layout_.groups.size())) {
            const auto& tiles = layout_.groups[hoverGroup_].tiles;
            int best = -1;
            for (size_t ti = 0; ti < tiles.size(); ++ti) {
                if (best < 0) { best = static_cast<int>(ti); continue; }
                const Tile& candidate = tiles[ti];
                const Tile& current = tiles[best];
                const bool better = last
                    ? (candidate.row > current.row ||
                       (candidate.row == current.row && candidate.col > current.col))
                    : (candidate.row < current.row ||
                       (candidate.row == current.row && candidate.col < current.col));
                if (better) best = static_cast<int>(ti);
            }
            if (best >= 0) {
                SetKeyboardFocus(3, best, hoverGroup_);
                return;
            }
        }
        const int count = ListItemCount();
        if (count > 0) {
            SetKeyboardFocus(2, last ? count - 1 : 0, -1);
            return;
        }
        FocusPlace(last ? 1 : FirstPlaceIndex());
    }

    // Tab and Shift+Tab walk the areas of the menu in a fixed order.
    void MoveFocusToNextArea(int delta) {
        constexpr int kStops = 6; // account button, power button, list, bottom bar, search box, tiles
        int current;
        switch (keyboardNav_ ? hoverArea_ : -1) {
            case 0: current = 0; break;
            case 7: current = 1; break;
            case 2:
            case 6: current = 2; break;
            case 1: current = hoverIndex_ == 1 ? 4 : 3; break;
            case 3: current = 5; break;
            default: current = delta > 0 ? 0 : kStops; break;
        }
        for (int step = 1; step <= kStops; ++step) {
            const int stop = ((current + delta * step) % kStops + kStops) % kStops;
            switch (stop) {
                case 0:
                    if (AllAppsNarrow()) break; // no account row in the All-apps view
                    SetKeyboardFocus(0, -1, -1);
                    return;
                case 1:
                    if (AllAppsNarrow()) break; // no power button in the All-apps view
                    SetKeyboardFocus(7, -1, -1);
                    return;
                case 2:
                    if (FocusListRow(0)) {
                        return;
                    }
                    break;
                case 3:
                    if (FocusPlace(0)) return;
                    break;
                case 4:
                    if (FocusPlace(1)) return;
                    break;
                default:
                    if (FocusTileFromColumn()) return;
                    break;
            }
        }
    }

    // ---- activation ----
    // Flyouts opened from the keyboard are anchored to the focused element.
    POINT FocusAnchorPoint() const {
        RECT rect{};
        if (!FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, rect)) return POINT{0, 0};
        return POINT{rect.left + (rect.right - rect.left) / 2,
                     rect.top + (rect.bottom - rect.top) / 2};
    }

    void ActivateKeyboardFocus() {
        if (!keyboardNav_ || hoverArea_ < 0) {
            // No highlight: Enter keeps launching the best search match.
            if (viewMode_ == ViewMode::Search && !searchResults_.empty())
                LaunchCatalogEntry(searchResults_[0]);
            return;
        }
        ActivateItem(hoverArea_, hoverIndex_, hoverGroup_, FocusAnchorPoint(), false);
    }

    void ShowKeyboardContextMenu() {
        if (!keyboardNav_ || hoverArea_ < 0) return;
        const POINT anchor = FocusAnchorPoint();
        switch (hoverArea_) {
            case 0: ShowUserMenu(anchor); return;
            case 7: ShowPowerMenu(anchor); return;
            case 2:
                if (hoverIndex_ >= 0) ShowAppContextMenu(anchor, hoverIndex_);
                return;
            case 3:
                if (hoverGroup_ >= 0 && hoverIndex_ >= 0)
                    ShowTileContextMenu(anchor, hoverGroup_, hoverIndex_);
                return;
            default: return;
        }
    }

    // ---- focus ring ----
    // Tiles already get the white border of the hover state, which is also the
    // focus look of the original menu; the other areas receive a thin outline
    // on top of their highlight.
    void DrawKeyboardFocusRing(HDC hdc) {
        if (!keyboardNav_ || hoverArea_ < 0 || hoverArea_ == 3) return;
        // The search box draws its own focus rim; a white ring on the white box
        // showed up as an empty white rectangle.
        if (hoverArea_ == 1 && hoverIndex_ == 1) return;
        // List rows (Most used, All apps, search results) are shown by their
        // selection veil only; the white outline read as an empty selector.
        if (hoverArea_ == 2 || hoverArea_ == 6) return;
        RECT focus{};
        if (!FocusTargetRect(hoverArea_, hoverIndex_, hoverGroup_, focus)) return;
        if (hoverArea_ == 2 || hoverArea_ == 6) {
            // Rows and A-Z cells scroll: clip the outline to the visible area.
            RECT viewport{}, visible{};
            if (!ListViewportRect(viewport)) return;
            if (!IntersectRect(&visible, &focus, &viewport)) return;
            focus = visible;
        }
        const int thickness = std::max(1, metrics_.Scale(1));
        if (focus.right - focus.left <= 2 * thickness ||
            focus.bottom - focus.top <= 2 * thickness) return;
        UniquePen pen(CreatePen(PS_SOLID, thickness, RGB(255, 255, 255)));
        if (!pen) return;
        HBRUSH hollow = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        ScopedSelectObject selectPen(hdc, pen.get());
        ScopedSelectObject selectBrush(hdc, hollow);
        const int inset = thickness; // keep the outline inside the element
        Rectangle(hdc, focus.left + inset, focus.top + inset,
                  focus.right - inset, focus.bottom - inset);
    }

    void FilterSearch() {
        searchResults_.clear();
        if (searchQuery_.empty()) return;
        StartScreenSearch::Start(hwnd_.get());
        // Adapted from Start Screen RunSearch: all words must match the name
        // or keywords; rank by category, prefix, folder and name match.
        const std::wstring q = StartScreenSearch::LowerW(searchQuery_);
        std::vector<std::wstring> words;
        size_t start=0;
        for (size_t i=0; i<=q.size(); ++i) {
            if (i==q.size() || q[i]==L' ') {
                if (i>start) words.push_back(q.substr(start,i-start));
                start=i+1;
            }
        }
        if (words.empty()) return;
        struct Hit { int rank; size_t index; bool extra; std::wstring lower; };
        std::vector<Hit> hits;
        auto add = [&](const std::wstring& lower, const std::wstring& keywords,
                       int category, bool dir, size_t index, bool extra) {
            bool all=true, inName=true;
            for (const auto& word : words) {
                if (lower.find(word)!=std::wstring::npos) continue;
                inName=false;
                if (keywords.find(word)==std::wstring::npos) { all=false; break; }
            }
            if (!all) return;
            const int rank=(category==StartScreenSearch::CC_APP ? 0 : category==StartScreenSearch::CC_SETTING ? 2 : 4)
                           +(lower.rfind(q,0)==0 ? 0 : 2)+(dir ? 1 : 0)+(inName ? 0 : 1);
            hits.push_back({rank,index,extra,lower});
        };
        const auto& apps=catalog_.Apps();
        for (size_t i=0;i<apps.size();++i)
            add(StartScreenSearch::LowerW(apps[i].name),L"",StartScreenSearch::CC_APP,false,i,false);
        for (size_t i=0;i<searchExtraCatalog_.size();++i) {
            const auto& a=searchExtraCatalog_[i]; add(a.lower,a.keywords,a.charmCat,a.dir,i,true);
        }
        const size_t keep=std::min(hits.size(),static_cast<size_t>(300));
        std::partial_sort(hits.begin(),hits.begin()+keep,hits.end(),[](const Hit& a,const Hit& b) {
            if (a.rank!=b.rank) return a.rank<b.rank;
            if (a.lower!=b.lower) return a.lower<b.lower;
            if (a.extra!=b.extra) return !a.extra;
            return a.index<b.index;
        });
        for (size_t i=0;i<keep;++i) {
            const Hit& h=hits[i];
            if (!h.extra) searchResults_.push_back(apps[h.index]);
            else {
                const auto& a=searchExtraCatalog_[h.index];
                AppEntry result{}; result.name=a.name;
                result.searchExtra=a.charmCat==StartScreenSearch::CC_FILE ? 2 : 1;
                result.searchExtraIndex=h.index; searchResults_.push_back(std::move(result));
            }
        }
    }

    // ===================== DOUBLE-BUFFERED RENDERING =====================
    void Render(HDC hdc, const RECT& rcPaint) {
        RECT rcClient{};
        GetClientRect(hwnd_.get(), &rcClient);

        const int width = static_cast<int>(rcClient.right);
        const int height = static_cast<int>(rcClient.bottom);
        if (width <= 0 || height <= 0) return;
        if (!renderDc_) renderDc_.reset(CreateCompatibleDC(hdc));
        if (!renderDc_) return;
        if (!renderBitmap_ || renderBufferWidth_ != width || renderBufferHeight_ != height) {
            renderBitmap_.reset(CreateCompatibleBitmap(hdc, width, height));
            if (!renderBitmap_) { renderBufferWidth_ = renderBufferHeight_ = 0; return; }
            renderBufferWidth_ = width;
            renderBufferHeight_ = height;
        }
        ScopedSelectObject sBmp(renderDc_.get(), renderBitmap_.get());
        ScopedSaveDC savedDc(renderDc_.get());
        IntersectClipRect(renderDc_.get(), rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);

        // GDI solid fills are cheaper than creating GDI+ Graphics objects for
        // each background on every hover/live-tile frame. One flat Metro
        // background covers the tile pane (the reference implementation paints the Update 3
        // menu with the Start screen DUI color), while the left column sits
        // on the same color darkened about one third, as on the leaked RT
        // Update 3 menu, so the white list text keeps its contrast. A subtle
        // lighter rim replaces the 1507 dark frame.
        const COLORREF menuBackground = ResolveMenuBackgroundColor(settings_);
        UniqueBrush background(CreateSolidBrush(menuBackground));
        UniqueBrush leftBackground(CreateSolidBrush(
            ShadeColor(menuBackground, -0.32f)));
        UniqueBrush borderBrush(CreateSolidBrush(BlendColor(menuBackground, RGB(255, 255, 255), 0.22f)));
        FillRect(renderDc_.get(), &rcClient, background.get());

        int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        RECT rcLeft = { 0, 0, leftW, rcClient.bottom };
        FillRect(renderDc_.get(), &rcLeft, leftBackground.get());
        FrameRect(renderDc_.get(), &rcClient, borderBrush.get());

        ScopedBkMode sBk(renderDc_.get(), TRANSPARENT);
        DrawLeftColumn(renderDc_.get(), rcLeft);

        RECT rcRight = { leftW, 0, rcClient.right, rcClient.bottom };
        DrawTiles(renderDc_.get(), rcRight);
        DrawDragPreview(renderDc_.get());
        if (!savedDc) SelectClipRgn(renderDc_.get(), nullptr);

        BitBlt(hdc, rcPaint.left, rcPaint.top,
               rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top,
               renderDc_.get(), rcPaint.left, rcPaint.top, SRCCOPY);
    }

    bool DrawCircularProfilePhoto(HDC hdc, Gdiplus::Bitmap* source,
                                  int x, int y, int size) const {
        if (!hdc || !source || source->GetLastStatus() != Gdiplus::Ok || size <= 0) return false;
        const UINT imageW = source->GetWidth(), imageH = source->GetHeight();
        if (!imageW || !imageH || imageW > static_cast<UINT>(INT_MAX) ||
            imageH > static_cast<UINT>(INT_MAX)) return false;
        constexpr int kSupersample = 4;
        const int highSize = size * kSupersample;
        Gdiplus::Bitmap avatar(highSize, highSize, PixelFormat32bppARGB);
        if (avatar.GetLastStatus() != Gdiplus::Ok) return false;
        Gdiplus::Graphics graphics(&avatar);
        Gdiplus::GraphicsPath clip;
        if (graphics.GetLastStatus() != Gdiplus::Ok ||
            graphics.Clear(Gdiplus::Color(0, 0, 0, 0)) != Gdiplus::Ok) return false;
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        if (clip.AddEllipse(0.5f, 0.5f, static_cast<REAL>(highSize - 1),
                            static_cast<REAL>(highSize - 1)) != Gdiplus::Ok ||
            graphics.SetClip(&clip, Gdiplus::CombineModeReplace) != Gdiplus::Ok) return false;
        const UINT cropSide = (std::min)(imageW, imageH);
        const INT cropX = static_cast<INT>((imageW - cropSide) / 2);
        const INT cropY = static_cast<INT>((imageH - cropSide) / 2);
        const bool copied = graphics.DrawImage(source, Gdiplus::Rect(0, 0, highSize, highSize),
            cropX, cropY, static_cast<INT>(cropSide), static_cast<INT>(cropSide),
            Gdiplus::UnitPixel) == Gdiplus::Ok;
        graphics.ResetClip();
        if (!copied) return false;
        Gdiplus::Pen border(Gdiplus::Color(220, 255, 255, 255),
            (std::max<REAL>)(1.0f, static_cast<REAL>(metrics_.dpi) / 96.0f) * kSupersample);
        if (graphics.DrawEllipse(&border, 2.0f, 2.0f,
                                 static_cast<REAL>(highSize - 4),
                                 static_cast<REAL>(highSize - 4)) != Gdiplus::Ok) return false;
        Gdiplus::Graphics target(hdc);
        if (target.GetLastStatus() != Gdiplus::Ok) return false;
        target.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        target.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        return target.DrawImage(&avatar, Gdiplus::Rect(x, y, size, size),
            0, 0, highSize, highSize, Gdiplus::UnitPixel) == Gdiplus::Ok;
    }

    void DrawLeftColumn(HDC hdc, const RECT& rcLeft) {
        int leftW = rcLeft.right - rcLeft.left;
        int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);

        // 1. Header row, in the Windows RT 8.1 Update 3 arrangement (the reference implementation):
        //    the account button on the left, the power button on the right.
        //    In the narrow All-apps view the header is gone and the list
        //    starts right below the menu's top edge, as in the leaked photos.
        if (AllAppsNarrow()) {
            userH = ListAreaTopPx();
        } else
        {
        RECT rcUser = { 0, 0, leftW - metrics_.Scale(44), userH };
        RECT paintClip{}, headerIntersection{};
        const bool headerNeedsPaint = GetClipBox(hdc, &paintClip) != ERROR &&
                                      IntersectRect(&headerIntersection, &paintClip, &rcUser);
        if (hoverArea_ == 0) {
            UniqueBrush brHov(CreateSolidBrush(HoverFill(0.13f)));
            FillRect(hdc, &rcUser, brHov.get());
        }

        // Real account photo (loaded off the UI thread). If the Shell exposes no
        // valid photo, the bundled fallback is kept without blocking the menu.
        const int avSize = metrics_.Scale(26);
        const int avX = metrics_.Scale(10);
        const int avY = (userH - avSize) / 2;
        if (headerNeedsPaint) {
            bool drewProfilePhoto = false;
            if (g_profilePhoto && g_profilePhoto->GetLastStatus() == Gdiplus::Ok)
                drewProfilePhoto = DrawCircularProfilePhoto(hdc, g_profilePhoto.get(), avX, avY, avSize);
            if (!drewProfilePhoto && g_defaultProfileAvatar)
                drewProfilePhoto = DrawCircularProfilePhoto(hdc, g_defaultProfileAvatar.get(), avX, avY, avSize);
            // If GDI+ cannot render either source, leave the avatar area blank rather
            // than substitute the old hand-drawn approximation.
        }

        // User name: resolved once by ProfilePhotoWorker (a domain lookup must
        // never run inside WM_PAINT); only the cached string is drawn here.
        // Privacy mode avoids even reading the real name, and hides it
        // completely: no placeholder is drawn in its place.
        const bool hideUserName = settings_.privacyMode;
        const wchar_t* userNameToDisplay = Ui(UiText::DefaultUser);
        if (!hideUserName && !accountDisplayName_.empty())
            userNameToDisplay = accountDisplayName_.c_str();

        RECT rcName = { avX + avSize + metrics_.Scale(10), 0, leftW - metrics_.Scale(48), userH };
        bool drewUserName = false;
        if (headerNeedsPaint) { // privacy mode shows the generic name (userNameToDisplay)
        {
            Gdiplus::Graphics graphics(hdc);
            // Segoe UI Light, the Update 3 menu's typeface for the account
            // name; fall back to plain Segoe UI where the Light family is
            // not installed.
            Gdiplus::FontFamily lightFamily(L"Segoe UI Light");
            Gdiplus::FontFamily regularFamily(L"Segoe UI");
            const Gdiplus::FontFamily* drawFamily =
                lightFamily.GetLastStatus() == Gdiplus::Ok ? &lightFamily :
                regularFamily.GetLastStatus() == Gdiplus::Ok ? &regularFamily : nullptr;
            Gdiplus::Font font(drawFamily, static_cast<REAL>(metrics_.Scale(20)),
                               Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::StringFormat format;
            Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
            format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
            if (graphics.GetLastStatus() == Gdiplus::Ok && drawFamily &&
                font.GetLastStatus() == Gdiplus::Ok) {
                Gdiplus::RectF layout(static_cast<REAL>(rcName.left), 0.0f,
                    static_cast<REAL>(rcName.right - rcName.left), static_cast<REAL>(userH));
                drewUserName = graphics.DrawString(userNameToDisplay, -1, &font, layout, &format, &textBrush) == Gdiplus::Ok;
            }
        }
        if (!drewUserName) {
            ScopedSelectObject sFont(hdc, fontLight_.get());
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            DrawTextG(hdc, userNameToDisplay, -1, &rcName, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        }

        // Power button: a 30x30 glyph button at the right edge of the header
        // row (the reference implementation PowerButton), opening the power menu.
        RECT rcPower{};
        if (PowerButtonRect(rcPower)) {
            if (hoverArea_ == 7) {
                UniqueBrush brHov(CreateSolidBrush(HoverFill(0.13f)));
                FillRect(hdc, &rcPower, brHov.get());
            }
            if (GetClipBox(hdc, &paintClip) != ERROR &&
                IntersectRect(&headerIntersection, &paintClip, &rcPower)) {
                const int powerGlyphX = rcPower.left +
                    ((rcPower.right - rcPower.left) - metrics_.Scale(16)) / 2;
                DrawPlaceGlyph(hdc, 2, rcPower, powerGlyphX);
            }
        }

        }

        // 2. Bottom bar, in the Update 3 order (the reference implementation): the All apps
        //    toggle above the white search box.
        int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        int bottomH = BottomPlaceItemCount() * placeH;
        int bottomY = rcLeft.bottom - bottomH;

        const bool allAppsView = viewMode_ == ViewMode::AllApps;
        {
            RECT rcItem = { 0, bottomY, leftW, bottomY + placeH };
            if (hoverArea_ == 1 && hoverIndex_ == 0) {
                UniqueBrush brHov(CreateSolidBrush(HoverFill(0.30f)));
                FillRect(hdc, &rcItem, brHov.get());
            }

            // Draw the toggle glyph: the right-pointing E2B7 arrow (or the
            // left-pointing E2B6 back arrow), 5 px from the left edge like
            // the reference implementation's ToggleButtonGlyph, with the label at 40 px.
            DrawPlaceGlyph(hdc, 3, rcItem, metrics_.Scale(6));

            RECT rcText = { metrics_.Scale(40), rcItem.top, leftW - metrics_.Scale(8), rcItem.bottom };
            ScopedSelectObject sF(hdc, fontLight_.get());
            ScopedTextColor sC(hdc, RGB(255, 255, 255));
            DrawTextG(hdc, allAppsView ? Ui(UiText::Back) : Ui(UiText::AllApps), -1, &rcText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        {
            RECT rcSearch = { 0, bottomY + placeH, leftW, bottomY + 2 * placeH };
            DrawSearchBox(hdc, rcSearch);
        }

        // 3. Scrollable middle section (Most used / All apps / Search)
        RECT rcScroll = { 0, ListAreaTopPx(), leftW, bottomY };
        UniqueRegion rgnClip(CreateRectRgn(rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.bottom));
        SelectClipRgn(hdc, rgnClip.get());

        if (viewMode_ == ViewMode::Normal) {
            DrawNormalAppList(hdc, rcScroll);
        } else if (viewMode_ == ViewMode::AllApps) {
            DrawAllApps(hdc, rcScroll);
        } else if (viewMode_ == ViewMode::Search) {
            DrawSearchResults(hdc, rcScroll);
        }

        SelectClipRgn(hdc, nullptr);
    }

    // Client rectangle of the power button in the header row.
    bool PowerButtonRect(RECT& out) const {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return false;
        if (AllAppsNarrow()) return false; // no power button in the All-apps view
        const int leftW = std::min(static_cast<int>(client.right),
                                   metrics_.Scale(Metrics::BASE_LEFT_COL_W));
        const int userH = metrics_.Scale(Metrics::BASE_USER_HEADER_H);
        const int size = metrics_.Scale(30);
        out = RECT{ leftW - metrics_.Scale(38), (userH - size) / 2,
                    leftW - metrics_.Scale(8), (userH + size) / 2 };
        return true;
    }

    // The white search box of the Update 3 menu (the reference implementation SearchBorder):
    // white fill, soft dark rim, grey placeholder, black query with a
    // blinking caret, and the magnifier glyph at the right.
    void DrawSearchBox(HDC hdc, const RECT& rcRow) {
        // Inset field like Win7Taskbar's 222x22 box (Margin 12,10,0,11) and
        // the reference implementation's rendered box (251x32, 8 px from either side of the
        // 267 px column): never a full-width slab.
        const int margin = metrics_.Scale(8);
        RECT rcBox = { rcRow.left + margin, rcRow.top + metrics_.Scale(1),
                       rcRow.right - margin, rcRow.bottom - metrics_.Scale(1) };
        UniqueBrush boxBrush(CreateSolidBrush(RGB(255, 255, 255)));
        if (boxBrush) FillRect(hdc, &rcBox, boxBrush.get());
        // the reference implementation brushes the box with #33000000: a translucent black that
        // darkens whatever menu color is behind it, so the border follows the
        // menu color instead of being a flat gray line.
        const bool searching = viewMode_ == ViewMode::Search;
        const bool boxFocused = searching || searchFocused_;
        const bool boxHot = hoverArea_ == 1 && hoverIndex_ == 1;
        // Win8.1 states: dark rim at rest, accent rim on hover, accent rim
        // (two pixels) while the box has the focus.
        COLORREF rimColor = BlendColor(ShadeColor(ResolveMenuBackgroundColor(settings_), -0.32f),
                                       RGB(0, 0, 0), 0.20f);
        if (boxFocused || boxHot) rimColor = GetSystemAccentColor(settings_.classicAccent);
        UniqueBrush borderBrush(CreateSolidBrush(rimColor));
        if (borderBrush) {
            FrameRect(hdc, &rcBox, borderBrush.get());
            if (boxFocused) {
                RECT in = rcBox; InflateRect(&in, -1, -1);
                FrameRect(hdc, &in, borderBrush.get());
            }
        }

        RECT rcText = { rcBox.left + metrics_.Scale(15), rcBox.top,
                        rcBox.right - metrics_.Scale(30), rcBox.bottom };
        ScopedSelectObject sFont(hdc, fontNormal_.get());
        if (searching && !searchQuery_.empty()) {
            if (searchAllSelected_) {
                // Ctrl+A selected the whole query: highlight it with the accent.
                SIZE querySize{};
                GetTextExtentPoint32W(hdc, searchQuery_.c_str(),
                                      static_cast<int>(searchQuery_.size()), &querySize);
                RECT rcSel = { rcText.left, rcBox.top + metrics_.Scale(4),
                               std::min(rcText.right, rcText.left + querySize.cx + metrics_.Scale(4)),
                               rcBox.bottom - metrics_.Scale(4) };
                UniqueBrush selectionBrush(CreateSolidBrush(GetSystemAccentColor(settings_.classicAccent)));
                if (selectionBrush) FillRect(hdc, &rcSel, selectionBrush.get());
                ScopedTextColor sCol(hdc, RGB(255, 255, 255));
                RECT rcSelText = rcSel;
                rcSelText.left += metrics_.Scale(2);
                DrawTextG(hdc, searchQuery_.c_str(), -1, &rcSelText,
                          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            } else {
                ScopedTextColor sCol(hdc, RGB(0, 0, 0));
                DrawTextG(hdc, searchQuery_.c_str(), -1, &rcText,
                          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                // Blinking caret right after the query, Win7-style.
                SIZE querySize{};
                GetTextExtentPoint32W(hdc, searchQuery_.c_str(),
                                      static_cast<int>(searchQuery_.size()), &querySize);
                if ((GetTickCount() / 530) % 2 == 0) {
                    const int caretX = std::min(rcText.left + querySize.cx + 1, rcText.right - 1);
                    RECT rcCaret = { caretX, rcBox.top + metrics_.Scale(9),
                                     caretX + std::max(1, metrics_.Scale(1)),
                                     rcBox.bottom - metrics_.Scale(9) };
                    UniqueBrush caretBrush(CreateSolidBrush(RGB(0, 0, 0)));
                    if (caretBrush) FillRect(hdc, &rcCaret, caretBrush.get());
                }
            }
        } else {
            // Focused but empty: the placeholder stays, with the caret at its start.
            if (searchFocused_ && (GetTickCount() / 530) % 2 == 0) {
                RECT rcCaret = { rcText.left - 1, rcBox.top + metrics_.Scale(9),
                                 rcText.left - 1 + std::max(1, metrics_.Scale(1)),
                                 rcBox.bottom - metrics_.Scale(9) };
                UniqueBrush caretBrush(CreateSolidBrush(RGB(0, 0, 0)));
                if (caretBrush) FillRect(hdc, &rcCaret, caretBrush.get());
            }
            ScopedTextColor sCol(hdc, RGB(112, 112, 112));
            DrawTextG(hdc, Ui(UiText::SearchPlaceholder), -1, &rcText,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        DrawSearchGlyph(hdc, rcBox);
    }

    // Vector reconstruction of the screenshot's magnifier: upper-right lens
    // and lower-left handle. DIP geometry remains smooth at fractional DPI.
    void DrawSearchGlyph(HDC hdc, const RECT& rcBox) {
        const float u = static_cast<float>(metrics_.dpi ? metrics_.dpi : 96) / 96.0f;
        // Match the supplied mod's 12-DIP search glyph; keep fractional DPI.
        const float glyph=12.0f*u;
        // Coordinates are calculated in floating-point physical pixels; avoid
        // rounding the scale to whole pixels at 125%, 150%, etc.
        const float x=static_cast<float>(rcBox.right)-10.0f*u-glyph;
        const float y=(static_cast<float>(rcBox.top+rcBox.bottom)-glyph)*0.5f;
        if (g_gdiplusToken) {
            Gdiplus::Graphics g(hdc);
            if (g.GetLastStatus()==Gdiplus::Ok) {
                g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                Gdiplus::Pen lens(Gdiplus::Color(255,120,120,120),1.35f*u);
                Gdiplus::Pen handle(Gdiplus::Color(255,120,120,120),1.65f*u);
                handle.SetStartCap(Gdiplus::LineCapRound); handle.SetEndCap(Gdiplus::LineCapRound);
                g.DrawLine(&handle,x+4.4f*u,y+7.6f*u,x+1.2f*u,y+10.8f*u);
                g.DrawEllipse(&lens,x+3.2f*u,y+0.8f*u,7.6f*u,7.6f*u);
                return;
            }
        }
        // Monochrome GDI fallback when GDI+ cannot initialize.
        const int sx=static_cast<int>(x+0.5f),sy=static_cast<int>(y+0.5f);
        UniquePen pen(CreatePen(PS_SOLID,std::max(1,metrics_.Scale(1)),RGB(120,120,120)));
        if (!pen) return;
        ScopedSelectObject p(hdc,pen.get()); ScopedSelectObject b(hdc,GetStockObject(NULL_BRUSH));
        Ellipse(hdc,sx+metrics_.Scale(3),sy+metrics_.Scale(1),sx+metrics_.Scale(11),sy+metrics_.Scale(9));
        MoveToEx(hdc,sx+metrics_.Scale(4),sy+metrics_.Scale(8),nullptr);
        LineTo(hdc,sx+metrics_.Scale(1),sy+metrics_.Scale(11));
    }

    // Folder disclosure indicator; this is independent of scrollbar arrows.
    void DrawFolderDisclosure(HDC hdc, const RECT& rect, bool expanded) {
        if (!g_gdiplusToken) {
            RECT fallback = rect;
            DrawTextW(hdc, expanded ? L"\u25BE" : L"\u25B8", -1, &fallback,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return;
        }
        Gdiplus::Graphics g(hdc);
        if (g.GetLastStatus() != Gdiplus::Ok) return;
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        const float u = static_cast<float>(metrics_.dpi) / 96.0f;
        const float x = (static_cast<float>(rect.left) + static_cast<float>(rect.right)) / 2.0f;
        const float y = (static_cast<float>(rect.top) + static_cast<float>(rect.bottom)) / 2.0f;
        Gdiplus::PointF pts[3];
        if (expanded) {
            pts[0] = {x - 3.0f*u, y - 2.0f*u};
            pts[1] = {x + 3.0f*u, y - 2.0f*u};
            pts[2] = {x, y + 2.0f*u};
        } else {
            pts[0] = {x - 2.0f*u, y - 3.0f*u};
            pts[1] = {x + 2.0f*u, y};
            pts[2] = {x - 2.0f*u, y + 3.0f*u};
        }
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
        g.FillPolygon(&brush, pts, 3);
    }

    void DrawPlaceGlyph(HDC hdc, int index, const RECT& rcItem, int glyphX = -1) {
        const REAL x = static_cast<REAL>(glyphX >= 0 ? glyphX : metrics_.Scale(18));
        const REAL sz = static_cast<REAL>(metrics_.Scale(16));
        const REAL u = sz / 16.0f;
        const REAL rowHeight = static_cast<REAL>(rcItem.bottom - rcItem.top);
        const REAL y = static_cast<REAL>(rcItem.top) + (rowHeight - sz) / 2.0f;
        Gdiplus::Graphics graphics(hdc);
        if (graphics.GetLastStatus() != Gdiplus::Ok) return;
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        Gdiplus::Pen pen(Gdiplus::Color(255, 230, 230, 230),
            std::max<REAL>(1.25f, static_cast<REAL>(metrics_.dpi) / 96.0f));
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        pen.SetLineJoin(Gdiplus::LineJoinRound);

        if (index == 3) { // All apps / Back: the RT Update 3 toggle glyphs.
            // the reference implementation labels the toggle with the Segoe UI Symbol
            // characters U+E2B7 ("All apps", a right-pointing arrow) and
            // U+E2B6 ("Back", a left-pointing one); both are plain arrows, so
            // they are drawn as vectors on the same 16 px grid as the glyph
            // instead of depending on the symbol font being installed.
            const REAL midY = y + 8.0f * u;
            if (viewMode_ == ViewMode::AllApps) {
                // Back (U+E2B6): a left-pointing arrow.
                graphics.DrawLine(&pen, x + 2.0f * u, midY, x + 13.5f * u, midY);
                graphics.DrawLine(&pen, x + 2.0f * u, midY, x + 7.5f * u, y + 2.5f * u);
                graphics.DrawLine(&pen, x + 2.0f * u, midY, x + 7.5f * u, y + 13.5f * u);
            } else {
                // All apps (U+E2B7): a right-pointing arrow.
                graphics.DrawLine(&pen, x + 2.5f * u, midY, x + 14.0f * u, midY);
                graphics.DrawLine(&pen, x + 14.0f * u, midY, x + 8.5f * u, y + 2.5f * u);
                graphics.DrawLine(&pen, x + 14.0f * u, midY, x + 8.5f * u, y + 13.5f * u);
            }
        } else if (index == 2) { // Power: heavier stroke without changing the button hit box.
            pen.SetWidth(std::max<REAL>(2.0f, 2.0f * static_cast<REAL>(metrics_.dpi) / 96.0f));
            graphics.DrawArc(&pen, x + 2.0f * u, y + 2.0f * u,
                             sz - 4.0f * u, sz - 4.0f * u, 315.0f, 270.0f);
            graphics.DrawLine(&pen, x + sz / 2.0f, y + 1.0f * u,
                              x + sz / 2.0f, y + sz / 2.0f + 0.5f * u);
        } else if (index == 1) { // Settings: geometric gear
            Gdiplus::PointF teeth[16];
            const REAL cx = x + sz / 2.0f, cy = y + sz / 2.0f;
            for (int i = 0; i < 16; ++i) {
                const REAL angle = static_cast<REAL>(i) * 3.14159265f / 8.0f - 1.5707963f;
                const REAL radius = (i % 2 == 0) ? sz * 0.46f : sz * 0.34f;
                teeth[i] = {cx + std::cos(angle) * radius, cy + std::sin(angle) * radius};
            }
            Gdiplus::GraphicsPath gear;
            gear.AddPolygon(teeth, ARRAYSIZE(teeth));
            graphics.DrawPath(&pen, &gear);
            graphics.DrawEllipse(&pen, cx - sz * 0.13f, cy - sz * 0.13f, sz * 0.26f, sz * 0.26f);
        } else if (index == 0) { // File Explorer: folder with front flap
            Gdiplus::PointF folderPts[] = {
                {x + 0.5f * u,  y + 1.5f * u},
                {x + 6.5f * u,  y + 1.5f * u},
                {x + 8.0f * u,  y + 3.0f * u},
                {x + 15.5f * u, y + 3.0f * u},
                {x + 15.5f * u, y + 14.5f * u},
                {x + 0.5f * u,  y + 14.5f * u}
            };
            Gdiplus::GraphicsPath folder;
            folder.AddPolygon(folderPts, ARRAYSIZE(folderPts));
            graphics.DrawPath(&pen, &folder);

            Gdiplus::PointF archPts[] = {
                {x + 2.7f * u,  y + 8.0f * u},
                {x + 13.3f * u, y + 8.0f * u},
                {x + 13.3f * u, y + 14.5f * u},
                {x + 11.5f * u, y + 14.5f * u},
                {x + 11.5f * u, y + 9.8f * u},
                {x + 4.5f * u,  y + 9.8f * u},
                {x + 5.3f * u,  y + 14.5f * u},
                {x + 2.7f * u,  y + 14.5f * u}
            };
            Gdiplus::GraphicsPath arch;
            arch.AddPolygon(archPts, ARRAYSIZE(archPts));
            Gdiplus::SolidBrush archBrush(Gdiplus::Color(255, 230, 230, 230));
            graphics.FillPath(&archBrush, &arch);
        }
    }

    void DrawListSeparator(HDC hdc, const RECT& area, int y) {
        if (y < area.top || y >= area.bottom) return;
        COLORREF c=BlendColor(ShadeColor(ResolveMenuBackgroundColor(settings_),-0.32f),RGB(255,255,255),0.18f);
        UniqueBrush brush(CreateSolidBrush(c));
        RECT line{area.left+std::max(1,metrics_.Scale(1)),y,area.right-std::max(1,metrics_.Scale(1)),y+std::max(1,metrics_.Scale(1))};
        if (brush && line.right>line.left) FillRect(hdc,&line,brush.get());
    }

    void DrawNormalAppList(HDC hdc, const RECT& rcArea) {
        const int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        int curY = rcArea.top - mainListScrollY_;
        int idx = 0;
        // One thin, low-contrast line spanning the list, matching the
        // separators in the supplied Windows RT screenshot. It consumes no
        // heading-height row; the gaps below are shared with hit testing.
        auto drawApp = [&](const AppEntry& app) {
            if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
                RECT item{0, curY, rcArea.right, curY + itemH};
                if (hoverArea_ == 2 && hoverIndex_ == idx) {
                    UniqueBrush hover(CreateSolidBrush(RowHoverFill())); FillRect(hdc,&item,hover.get());
                }
                const int iconSize=metrics_.Scale(Metrics::BASE_ICON_SIZE);
                DrawAppIconOrFallback(hdc,app.hIcon,metrics_.Scale(12),curY+(itemH-iconSize)/2,iconSize);
                RECT text{metrics_.Scale(52),curY,rcArea.right-metrics_.Scale(8),curY+itemH};
                ScopedSelectObject sf(hdc,fontLight_.get()); ScopedTextColor sc(hdc,RGB(255,255,255));
                DrawTextG(hdc,app.name.c_str(),-1,&text,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
            }
            curY += itemH; ++idx;
        };
        DrawListSeparator(hdc,rcArea,curY);
        curY += metrics_.Scale(Metrics::BASE_FIRST_LIST_INSET);
        for (const auto& app : catalog_.MostUsed()) drawApp(app);
        if (!catalogReady_) {
            RECT loading{metrics_.Scale(12),curY,rcArea.right-metrics_.Scale(8),curY+itemH};
            ScopedSelectObject sf(hdc,fontLight_.get()); ScopedTextColor sc(hdc,RGB(215,215,215));
            DrawTextG(hdc,Ui(UiText::CatalogLoading),-1,&loading,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
        }
        if (!catalog_.RecentlyAdded().empty()) {
            const int gap = metrics_.Scale(Metrics::BASE_LIST_SECTION_GAP);
            DrawListSeparator(hdc,rcArea,curY + gap / 2);
            curY += gap;
            for (const auto& app : catalog_.RecentlyAdded()) drawApp(app);
        }
        const int areaTop = static_cast<int>(rcArea.top);
        const int visibleHeight = static_cast<int>(rcArea.bottom - rcArea.top);
        totalMainListH_ = std::max(0, curY + mainListScrollY_ - areaTop);
        const int maxScroll = std::max(0, totalMainListH_ - visibleHeight);
        mainListScrollY_ = std::max(0,std::min(mainListScrollY_,maxScroll));
        DrawMainListScrollbar(hdc, rcArea);
    }

    void DrawAllAppsStyleScrollbar(HDC hdc) {
        const int scroll = viewMode_ == ViewMode::Search ? mainListScrollY_ : allAppsScrollY_;
        RECT rcTrack{}, rcThumb{}, rcUpButton{}, rcDownButton{}; int maxScroll = 0;
        if (GetAllAppsScrollGeometry(rcTrack, rcThumb, maxScroll, &rcUpButton, &rcDownButton)) {
            UniqueBrush rail(CreateSolidBrush(AllAppsSbRailColor()));
            RECT rcBar{ rcTrack.left, rcUpButton.top, rcTrack.right, rcDownButton.bottom };
            FillRect(hdc, &rcBar, rail.get());
            const bool upDisabled = scroll <= 0;
            const bool downDisabled = scroll >= maxScroll;
            const bool upPressed = allAppsScrollArrowPressed_ && allAppsScrollRepeatDirection_ < 0;
            const bool downPressed = allAppsScrollArrowPressed_ && allAppsScrollRepeatDirection_ > 0;
            DrawAllAppsScrollChevron(hdc, rcUpButton, true,
                upDisabled ? AllAppsSbArrowDimColor() : upPressed ? AllAppsSbArrowHotColor() : AllAppsSbArrowColor());
            DrawAllAppsScrollChevron(hdc, rcDownButton, false,
                downDisabled ? AllAppsSbArrowDimColor() : downPressed ? AllAppsSbArrowHotColor() : AllAppsSbArrowColor());
            DrawScrollThumb(hdc, rcThumb, allAppsScrollDragging_, true);
        }
    }

    void DrawAllApps(HDC hdc, const RECT& rcArea) {
        // Win7-style All Programs list: flat alphabetical rows with no letter
        // headers, compact 30 px rows, 16 px icons, folders interleaved with
        // the loose apps and expanding in place below their row.
        EnsureAllAppsRows();
        const auto& sections = catalog_.Sections();
        int curY = rcArea.top - allAppsScrollY_;
        const int itemH = metrics_.Scale(Metrics::BASE_ALLAPPS_ITEM_H);
        const int iconSz = metrics_.Scale(Metrics::BASE_ALLAPPS_ICON);
        const int textX = metrics_.Scale(Metrics::BASE_ALLAPPS_TEXT_X);
        const int textRight = rcArea.right - metrics_.Scale(26); // scrollbar column stays clear
        int totalH = 0;
        int renderIdx = 0;

        auto rowVisible = [&](int y) {
            return y + itemH >= rcArea.top && y <= rcArea.bottom;
        };

        for (const auto& row : allAppsRows_) {
            if (rowVisible(curY)) {
                RECT rcItem = { 0, curY, rcArea.right, curY + itemH };
                if (hoverArea_ == 2 && hoverIndex_ == renderIdx) {
                    UniqueBrush brHov(CreateSolidBrush(RowHoverFill()));
                    FillRect(hdc, &rcItem, brHov.get());
                }
                const LetterSection& sec = sections[row.section];
                const int icY = curY + (itemH - iconSz) / 2;
                if (row.isFolder) {
                    const FolderGroup& folder = sec.folders[row.index];
                    // The classic shell folder icon, like the Win7 All Programs
                    // list: prefer the true 16 px shell image, then the 32 px
                    // one scaled down, and fall back to a flat vector folder
                    // where the shell exposes neither (e.g. minimal Wine).
                    if (!g_stockFolderIconSmall && !g_stockFolderIconTriedSmall) {
                        g_stockFolderIconTriedSmall = true;
                        SHSTOCKICONINFO si{}; si.cbSize=sizeof(si);
                        if (SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER,SHGSI_ICON|SHGSI_SMALLICON,&si)))
                            g_stockFolderIconSmall=si.hIcon;
                    }
                    if (!g_stockFolderIconSmall && !g_stockFolderIcon && !g_stockFolderIconTriedLarge) {
                        g_stockFolderIconTriedLarge = true;
                        SHSTOCKICONINFO si{}; si.cbSize=sizeof(si);
                        if (SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER,SHGSI_ICON|SHGSI_LARGEICON,&si)))
                            g_stockFolderIcon=si.hIcon;
                    }
                    const int icX = metrics_.Scale(6);
                    if (g_stockFolderIconSmall)
                        DrawIconEx(hdc, icX, icY, g_stockFolderIconSmall, iconSz, iconSz, 0, nullptr, DI_NORMAL);
                    else if (g_stockFolderIcon)
                        DrawIconEx(hdc, icX, icY, g_stockFolderIcon, iconSz, iconSz, 0, nullptr, DI_NORMAL);
                    else
                        DrawFlatFolderGlyph(hdc, icX, icY, iconSz);
                    // Reserve the entire chevron column, plus a small gap.
                    RECT rcText = { textX, curY, std::max(textX, textRight - metrics_.Scale(18)), curY + itemH };
                    ScopedSelectObject sF(hdc, fontLight_.get());
                    ScopedTextColor sC(hdc, RGB(255, 255, 255));
                    DrawTextG(hdc, folder.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                    // Chevron (v expanded, > collapsed) at the right edge.
                    RECT rcChev = { textRight - metrics_.Scale(14), curY, textRight, curY + itemH };
                    DrawFolderDisclosure(hdc, rcChev, folder.expanded);
                } else {
                    const AppEntry& app = sec.apps[row.index];
                    DrawAppIconOrFallback(hdc, app.hIcon, metrics_.Scale(6), icY, iconSz);
                    RECT rcText = { textX, curY, textRight, curY + itemH };
                    ScopedSelectObject sF(hdc, fontLight_.get());
                    ScopedTextColor sC(hdc, RGB(255, 255, 255));
                    DrawTextG(hdc, app.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                }
            }
            curY += itemH;
            totalH += itemH;
            ++renderIdx;

            if (row.isFolder) {
                const FolderGroup& folder = sections[row.section].folders[row.index];
                if (folder.expanded) {
                    for (const auto& sub : folder.apps) {
                        if (rowVisible(curY)) {
                            RECT rcSub = { 0, curY, rcArea.right, curY + itemH };
                            if (hoverArea_ == 2 && hoverIndex_ == renderIdx) {
                                UniqueBrush brHov(CreateSolidBrush(RowHoverFill()));
                                FillRect(hdc, &rcSub, brHov.get());
                            }
                            const int icY = curY + (itemH - iconSz) / 2;
                            DrawAppIconOrFallback(hdc, sub.hIcon, metrics_.Scale(22), icY, iconSz);
                            RECT rcText = { metrics_.Scale(48), curY, textRight, curY + itemH };
                            ScopedSelectObject sF(hdc, fontLight_.get());
                            ScopedTextColor sC(hdc, RGB(255, 255, 255));
                            DrawTextG(hdc, sub.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                        }
                        curY += itemH;
                        totalH += itemH;
                        ++renderIdx;
                    }
                }
            }
        }
        totalAllAppsH_ = totalH;

        // Scrollbar, Win7 All Programs style: light rail, gray thumb, dark
        // arrow buttons at both ends, with paging and dragging.
        const int visibleH = rcArea.bottom - rcArea.top;
        const int currentMaxScroll = std::max(0, totalAllAppsH_ - visibleH);
        allAppsScrollY_ = std::max(0, std::min(allAppsScrollY_, currentMaxScroll));
        DrawAllAppsStyleScrollbar(hdc);

    }


    void UpdateDragPreview(int x, int y) {
        RECT client{};
        if (!hwnd_ || !GetClientRect(hwnd_.get(), &client)) return;
        const int width = metrics_.Scale(40), height = width, gap = metrics_.Scale(12);
        RECT next{x + gap, y + gap, x + gap + width, y + gap + height};
        if (next.right > client.right) OffsetRect(&next, -(next.right - client.right), 0);
        if (next.bottom > client.bottom) OffsetRect(&next, 0, -(next.bottom - client.bottom));
        if (next.left < 0) OffsetRect(&next, -next.left, 0);
        if (next.top < 0) OffsetRect(&next, 0, -next.top);
        if (EqualRect(&next, &dragPreviewRect_)) return;
        RECT old = dragPreviewRect_;
        dragPreviewRect_ = next;
        dragPreviewPoint_ = {x, y};
        if (old.right > old.left && old.bottom > old.top) InvalidateRect(hwnd_.get(), &old, FALSE);
        InvalidateRect(hwnd_.get(), &dragPreviewRect_, FALSE);
    }

    void ClearDragPreview() {
        if (dragPreviewRect_.right > dragPreviewRect_.left && dragPreviewRect_.bottom > dragPreviewRect_.top && hwnd_)
            InvalidateRect(hwnd_.get(), &dragPreviewRect_, FALSE);
        dragPreviewRect_ = {};
    }

    void DrawDragPreview(HDC hdc) {
        if (dragPreviewRect_.right <= dragPreviewRect_.left ||
            dragPreviewRect_.bottom <= dragPreviewRect_.top || !appDragging_) return;
        DrawAppIconOrFallback(hdc, draggedApp_.hIcon, dragPreviewRect_.left,
                              dragPreviewRect_.top, dragPreviewRect_.right-dragPreviewRect_.left);
    }

    void DrawSearchResults(HDC hdc, const RECT& rcArea) {
        // The query itself is shown in the white search box at the bottom of
        // the left column (DrawSearchBox); the results start right below the
        // top padding, as in the reference implementation's filtered programs list.
        DrawListSeparator(hdc,rcArea,rcArea.top);
        int curY = rcArea.top + metrics_.Scale(8) - mainListScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        const int resultHeight=metrics_.Scale(8)+static_cast<int>(searchResults_.size())*itemH;
        const bool hasScroll=resultHeight>rcArea.bottom-rcArea.top;
        const int textRight=rcArea.right-(hasScroll ? std::max(14,metrics_.Scale(Metrics::BASE_ALLAPPS_SB_W)) : 0);

        int idx = 0;
        for (const auto& a : searchResults_) {
            if (curY + itemH >= rcArea.top && curY <= rcArea.bottom) {
            RECT rcItem = { 0, curY, rcArea.right, curY + itemH };
            if (hoverArea_ == 2 && hoverIndex_ == idx) {
                UniqueBrush brHov(CreateSolidBrush(RowHoverFill()));
                FillRect(hdc, &rcItem, brHov.get());
            }
            int icSz = metrics_.Scale(Metrics::BASE_ICON_SIZE);
            int icY = curY + (itemH - icSz) / 2;
            if (a.searchExtra && a.searchExtraIndex < searchIcons_.size()) {
                auto& slot=searchIcons_[a.searchExtraIndex];
                if (slot.state==0) {
                    slot.state=StartScreenSearch::QueueIcon(a.searchExtraIndex,searchExtraCatalog_[a.searchExtraIndex]) ? 1 : 2;
                }
                bool drawn=false;
                auto& image=slot.image;
                if (!image.pixels.empty() && image.w>0 && image.h>0 && g_gdiplusToken) {
                    Gdiplus::Bitmap bitmap(image.w,image.h,image.w*4,PixelFormat32bppPARGB,image.pixels.data());
                    Gdiplus::Graphics graphics(hdc);
                    if (bitmap.GetLastStatus()==Gdiplus::Ok && graphics.GetLastStatus()==Gdiplus::Ok) {
                        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
                        drawn=graphics.DrawImage(&bitmap,Gdiplus::Rect(metrics_.Scale(12),icY,icSz,icSz),
                                                 0,0,image.w,image.h,Gdiplus::UnitPixel)==Gdiplus::Ok;
                    }
                }
                if (!drawn) DrawAppIconOrFallback(hdc,nullptr,metrics_.Scale(12),icY,icSz);
            } else DrawAppIconOrFallback(hdc,a.hIcon,metrics_.Scale(12),icY,icSz);
            RECT rcText = { metrics_.Scale(52), curY, std::max(metrics_.Scale(52), textRight - metrics_.Scale(8)), curY + itemH };
            ScopedSelectObject sFont(hdc, fontLight_.get());
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            DrawTextG(hdc, a.name.c_str(), -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            curY += itemH;
            idx++;
        }
        if (searchResults_.empty() && !searchQuery_.empty()) {
            // The original empty state, word for word from the build's MUI.
            RECT rcEmpty = { metrics_.Scale(16), rcArea.top + metrics_.Scale(8),
                             rcArea.right - metrics_.Scale(8), rcArea.top + metrics_.Scale(40) };
            ScopedSelectObject sFont(hdc, fontLight_.get());
            ScopedTextColor sCol(hdc, RGB(200, 200, 200));
            DrawTextG(hdc, Ui(UiText::SearchEmpty), -1, &rcEmpty,
                      DT_LEFT | DT_TOP | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        totalMainListH_ = metrics_.Scale(8) + static_cast<int>(searchResults_.size()) * itemH;
        const int visibleHeight = static_cast<int>(rcArea.bottom - rcArea.top);
        const int maxScroll = std::max(0, totalMainListH_ - visibleHeight);
        mainListScrollY_ = std::max(0, std::min(mainListScrollY_, maxScroll));
        DrawAllAppsStyleScrollbar(hdc);
    }

    void DrawTiles(HDC hdc, const RECT& rcRight) {
        // CStartMenuGridPane::OnSize (explorer.exe 10240.16384, RVA 0x91f50)
        // resizes both the child window and its DirectUI grid element to the
        // available client rectangle. Mirror that viewport boundary here without
        // resizing user tiles: off-viewport groups/tiles are clipped and skipped.
        ScopedSaveDC savedGridDc(hdc);
        if (!savedGridDc) return;
        const int clipResult = IntersectClipRect(hdc, rcRight.left, rcRight.top,
                                                  rcRight.right, rcRight.bottom);
        if (clipResult == ERROR || clipResult == NULLREGION) return;

        const int startX = rcRight.left + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        const int startY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y);
        const int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                           (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        const int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);
        const int groupStep = groupW + groupGap;
        const int tileGap = metrics_.Scale(4);

        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            // Group origins increase monotonically; stop once the grid viewport is passed.
            const int64_t gx64 = static_cast<int64_t>(startX) +
                                 static_cast<int64_t>(gi) * groupStep;
            if (gx64 >= rcRight.right) break;
            if (gx64 + groupW <= rcRight.left || gx64 > INT_MAX) continue;
            const int gx = static_cast<int>(gx64);
            auto& g = layout_.groups[gi];
            const int gy = startY;
            const int headerH = GroupHeaderHeight(static_cast<int>(gi));

            RECT rcHead{ gx, gy, gx + groupW, gy + headerH };
            RECT visibleHeader{};
            if (headerH > 0 && IntersectRect(&visibleHeader, &rcHead, &rcRight)) {
                ScopedSelectObject sFont(hdc, fontTitle_.get());
                ScopedTextColor sCol(hdc, (hoverArea_ == 4 && hoverGroup_ == static_cast<int>(gi))
                    ? RGB(255, 255, 255) : RGB(215, 215, 215));
                DrawTextG(hdc, (_wcsicmp(g.title.c_str(), L"Applicazioni") == 0 ? Ui(UiText::GroupApps) :
                    (_wcsicmp(g.title.c_str(), L"Tempo libero") == 0 ? Ui(UiText::GroupLeisure) : g.title.c_str())),
                    -1, &rcHead, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }

            const int tilesStartY = gy + headerH + tileGap;
            for (size_t ti = 0; ti < g.tiles.size(); ++ti) {
                // The dragged tile is painted after every group, so that it
                // stays above the whole grid while it follows the pointer.
                if (IsTileLifted(static_cast<int>(gi), static_cast<int>(ti))) continue;
                auto& tile = g.tiles[ti];
                RECT rcTile = ComputeTileRect(gx, tilesStartY, tile);
                OffsetRect(&rcTile, TileAnimOffset(tile.animOffsetX), TileAnimOffset(tile.animOffsetY));
                RECT visibleTile{};
                if (!IntersectRect(&visibleTile, &rcTile, &rcRight)) continue;
                const bool isHovered = hoverArea_ == 3 && hoverGroup_ == static_cast<int>(gi) &&
                                       hoverIndex_ == static_cast<int>(ti);
                const TileColors colors = tile.available
                    ? ResolveTileColors(tile, settings_)
                    : TileColors{ RGB(72, 72, 72), RGB(72, 72, 72), RGB(56, 56, 56) };
                DrawSingleTile(hdc, tile, rcTile, isHovered, colors, tile.animScale);
            }
        }

        DrawLiftedTile(hdc, rcRight);
    }

    // The dragged tile floats above the grid: enlarged, slightly transparent
    // and painted last, the way the 8045 reference mod lifts its tiles.
    void DrawLiftedTile(HDC hdc, const RECT& rcRight) {
        if (!tileDragging_ || dragGroup_ < 0 ||
            dragGroup_ >= static_cast<int>(layout_.groups.size())) return;
        auto& tiles = layout_.groups[dragGroup_].tiles;
        if (dragTile_ < 0 || dragTile_ >= static_cast<int>(tiles.size())) return;
        RECT rcTile{};
        if (!GetTileDrawRect(dragGroup_, dragTile_, rcTile)) return;
        RECT visibleTile{};
        if (!IntersectRect(&visibleTile, &rcTile, &rcRight)) return;

        Tile& tile = tiles[dragTile_];
        const TileColors colors = tile.available
            ? ResolveTileColors(tile, settings_)
            : TileColors{ RGB(72, 72, 72), RGB(72, 72, 72), RGB(56, 56, 56) };
        const int width = rcTile.right - rcTile.left;
        const int height = rcTile.bottom - rcTile.top;
        if (width <= 0 || height <= 0) return;

        // GDI has no per-primitive alpha: the tile is painted once on a scratch
        // surface seeded with the background and blended back over it. The
        // margin leaves room for the 5% the lifted tile grows by.
        const int margin = metrics_.Scale(8);
        const int surfaceW = width + margin * 2;
        const int surfaceH = height + margin * 2;
        UniqueMemDC scratchDc(CreateCompatibleDC(hdc));
        UniqueBitmap scratchBitmap(scratchDc ? CreateCompatibleBitmap(hdc, surfaceW, surfaceH) : nullptr);
        if (scratchDc && scratchBitmap) {
            ScopedSelectObject selectScratch(scratchDc.get(), scratchBitmap.get());
            ScopedBkMode scratchBkMode(scratchDc.get(), TRANSPARENT);
            if (BitBlt(scratchDc.get(), 0, 0, surfaceW, surfaceH, hdc,
                       rcTile.left - margin, rcTile.top - margin, SRCCOPY)) {
                RECT rcScratchTile{margin, margin, margin + width, margin + height};
                DrawSingleTile(scratchDc.get(), tile, rcScratchTile, true, colors, tile.animScale);
                BLENDFUNCTION blend{AC_SRC_OVER, 0, TILE_DRAG_ALPHA, 0};
                if (AlphaBlend(hdc, rcTile.left - margin, rcTile.top - margin, surfaceW, surfaceH,
                               scratchDc.get(), 0, 0, surfaceW, surfaceH, blend))
                    return;
            }
        }
        // Without the scratch surface the lifted tile is simply drawn opaque.
        DrawSingleTile(hdc, tile, rcTile, true, colors, tile.animScale);
    }

    RECT ComputeTileRect(int groupOriginX, int groupOriginY, const Tile& t) const {
        int gap = metrics_.Scale(Metrics::BASE_TILE_GAP);
        int med = metrics_.Scale(Metrics::BASE_TILE_MEDIUM);
        int sm = metrics_.Scale(Metrics::BASE_TILE_SMALL);
        int wd = metrics_.Scale(Metrics::BASE_TILE_WIDE);
        int lg = metrics_.Scale(Metrics::BASE_TILE_LARGE);

        int x = groupOriginX + t.col * (sm + gap);
        int y = groupOriginY + t.row * (sm + gap);

        int w = med, h = med;
        switch (t.size) {
            case TileSize::Small:  w = sm; h = sm; break;
            case TileSize::Medium: w = med; h = med; break;
            case TileSize::Wide:   w = wd;  h = med; break;
            case TileSize::Large:  w = lg;  h = lg;  break;
        }
        return RECT{ x, y, x + w, y + h };
    }

    // `scale` is the animated press/lift factor: the tile is painted around
    // the centre of its rectangle, which itself never changes, so hit testing
    // and the layout are unaffected.
    void DrawSingleTile(HDC hdc, Tile& tile, const RECT& rcTile, bool hovered,
                        const TileColors& colors, float scale) {

        RECT drawRc = rcTile;
        bool drawBack = tile.showBack;

        // While the tile flips, apply the vertical cosine deformation
        if (tile.flipping) {
            float p = tile.flipProgress;
            float scaleY = 1.0f;
            if (p < 0.5f) {
                scaleY = cosf(p * 3.14159265f);
            } else {
                scaleY = cosf((1.0f - p) * 3.14159265f);
                drawBack = !tile.showBack;
            }
            if (scaleY < 0.05f) scaleY = 0.05f;

            int fullH = rcTile.bottom - rcTile.top;
            int squeezedH = (int)(fullH * scaleY);
            int midY = (rcTile.top + rcTile.bottom) / 2;
            drawRc.top = midY - squeezedH / 2;
            drawRc.bottom = midY + squeezedH / 2;
        }

        // Centered press-in and lift effects, inspired by AdmnistratoX's 8045
        // mod: only the painted rectangle changes, never the layout one.
        if (scale > 0.0f && fabsf(scale - 1.0f) > TILE_ANIM_SETTLE_SCALE) {
            const int width = drawRc.right - drawRc.left;
            const int height = drawRc.bottom - drawRc.top;
            const int scaledW = (std::max)(1, static_cast<int>(width * scale + 0.5f));
            const int scaledH = (std::max)(1, static_cast<int>(height * scale + 0.5f));
            drawRc.left -= (scaledW - width) / 2;
            drawRc.right = drawRc.left + scaledW;
            drawRc.top -= (scaledH - height) / 2;
            drawRc.bottom = drawRc.top + scaledH;
        }

        // GDI+ draws both flat and shaded tile backgrounds. The icon bitmap
        // cache is shared with the rest of the UI; only visible tiles are painted.
        const int tileW = drawRc.right - drawRc.left;
        const int tileH = drawRc.bottom - drawRc.top;
        bool tilePainted = false;
        if (g_gdiplusToken && tileW > 0 && tileH > 0) {
            Gdiplus::Graphics graphics(hdc);
            if (graphics.GetLastStatus() == Gdiplus::Ok) {
                auto color = [](COLORREF c) { return Gdiplus::Color(255, GetRValue(c), GetGValue(c), GetBValue(c)); };
                Gdiplus::Rect rect(drawRc.left, drawRc.top, tileW, tileH);
                Gdiplus::Status status;
                if (colors.c1 == colors.c2) {
                    Gdiplus::SolidBrush fill(color(colors.c1));
                    status = graphics.FillRectangle(&fill, rect);
                } else {
                    Gdiplus::LinearGradientBrush fill(rect, color(colors.c1), color(colors.c2), Gdiplus::LinearGradientModeHorizontal);
                    status = graphics.FillRectangle(&fill, rect);
                }
                tilePainted = status == Gdiplus::Ok;
                if (tilePainted && tileW > 2 && tileH > 2) {
                    COLORREF rim = BlendColor(BlendColor(colors.c1, colors.c2, 0.5f), colors.border, 0.40f);
                    Gdiplus::Pen pen(color(rim), 1.0f);
                    graphics.DrawRectangle(&pen, drawRc.left, drawRc.top, tileW-1, tileH-1);
                }
            }
        }
        if (!tilePainted) {
        // The Windows 8.1 tile fill: a sweep from the darker left edge to the
        // lighter right one (the reference implementation), or a flat fill when shading is off.
        if (colors.c1 == colors.c2) {
            UniqueBrush tileBrush(CreateSolidBrush(colors.c1));
            if (tileBrush) FillRect(hdc, &drawRc, tileBrush.get());
        } else {
            TRIVERTEX vertices[2] = {
                { drawRc.left, drawRc.top,
                  (COLOR16)(GetRValue(colors.c1) << 8), (COLOR16)(GetGValue(colors.c1) << 8),
                  (COLOR16)(GetBValue(colors.c1) << 8), 0 },
                { drawRc.right, drawRc.bottom,
                  (COLOR16)(GetRValue(colors.c2) << 8), (COLOR16)(GetGValue(colors.c2) << 8),
                  (COLOR16)(GetBValue(colors.c2) << 8), 0 },
            };
            GRADIENT_RECT gradient = { 0, 1 };
            GradientFill(hdc, vertices, 2, &gradient, 1, GRADIENT_FILL_RECT_H);
        }

        // 1 px rim over the sweep at 40% opacity, like the reference implementation's Stroke
        // (BorderBrush with Opacity 0.4): the palette border color is blended
        // over the middle of the tile's own gradient.
        if (drawRc.right - drawRc.left > 2 && drawRc.bottom - drawRc.top > 2) {
            const COLORREF mid = BlendColor(colors.c1, colors.c2, 0.5f);
            UniqueBrush rimBrush(CreateSolidBrush(BlendColor(mid, colors.border, 0.40f)));
            if (rimBrush) FrameRect(hdc, &drawRc, rimBrush.get());
        }

        }

        if (drawBack && HasLocalLiveContent(tile)) {
            // Back face of the Calendar tile: weekday and date computed locally
            // with GetDateFormatEx (day number large, as in Windows 10).
            ScopedTextColor sCol(hdc, RGB(255, 255, 255));
            wchar_t dayText[16]{}, weekdayText[64]{}, monthText[64]{};
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"d", dayText, ARRAYSIZE(dayText), nullptr);
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"dddd", weekdayText, ARRAYSIZE(weekdayText), nullptr);
            GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, L"MMMM yyyy", monthText, ARRAYSIZE(monthText), nullptr);

            RECT rcDay = { drawRc.left + metrics_.Scale(8), drawRc.top + metrics_.Scale(6), drawRc.right - metrics_.Scale(8), drawRc.top + metrics_.Scale(40) };
            ScopedSelectObject sFT(hdc, fontTitle_.get());
            DrawTextG(hdc, dayText, -1, &rcDay, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

            RECT rcWeekday = { drawRc.left + metrics_.Scale(8), drawRc.top + metrics_.Scale(40), drawRc.right - metrics_.Scale(8), drawRc.top + metrics_.Scale(58) };
            ScopedSelectObject sFB(hdc, fontBold_.get());
            DrawTextG(hdc, weekdayText, -1, &rcWeekday, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);

            RECT rcMonth = { drawRc.left + metrics_.Scale(8), drawRc.bottom - metrics_.Scale(22), drawRc.right - metrics_.Scale(8), drawRc.bottom - metrics_.Scale(4) };
            ScopedSelectObject sFS(hdc, fontSmall_.get());
            DrawTextG(hdc, monthText, -1, &rcMonth, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        } else {
            // Standard front face (the reference implementation): a 48 px icon lifted off the
            // caption, 32 px and centered on small tiles, and a 12 px caption
            // that wraps on two lines at the bottom left.
            int icSz = metrics_.Scale(tile.size == TileSize::Small ? 32 : 48);
            int icX = drawRc.left + ((drawRc.right - drawRc.left) - icSz) / 2;
            int icY = drawRc.top + ((drawRc.bottom - drawRc.top) - icSz) / 2;
            if (tile.size != TileSize::Small) icY -= metrics_.Scale(13);

            DrawAppIconOrFallback(hdc,tile.hIcon,icX,icY,icSz);

            if (tile.size != TileSize::Small) {
                RECT rcLabel = { drawRc.left + metrics_.Scale(16), drawRc.bottom - metrics_.Scale(32),
                                 drawRc.right - metrics_.Scale(6), drawRc.bottom - metrics_.Scale(6) };
                ScopedSelectObject sF(hdc, fontSmall_.get());
                ScopedTextColor sCol(hdc, RGB(255, 255, 255));
                DrawTextG(hdc, TranslateTileCaption(tile.name), -1, &rcLabel, DT_LEFT | DT_BOTTOM | DT_WORDBREAK);
            }
        }
        if (hovered && !tileDragging_ && drawRc.right > drawRc.left && drawRc.bottom > drawRc.top) {
            Gdiplus::Graphics graphics(hdc);
            const int width = drawRc.right - drawRc.left, height = drawRc.bottom - drawRc.top;
            if (g_gdiplusToken && graphics.GetLastStatus() == Gdiplus::Ok) {
                auto frame = [&](int inset, int thickness, Gdiplus::Color color) {
                    int w=width-2*inset, h=height-2*inset;
                    int t=std::min(thickness,std::min(w,h)/2);
                    if (t<=0) return;
                    Gdiplus::SolidBrush brush(color);
                    const int x=drawRc.left+inset, y=drawRc.top+inset;
                    graphics.FillRectangle(&brush,x,y,w,t);
                    graphics.FillRectangle(&brush,x,y+h-t,w,t);
                    graphics.FillRectangle(&brush,x,y+t,t,h-2*t);
                    graphics.FillRectangle(&brush,x+w-t,y+t,t,h-2*t);
                };
                if (keyboardNav_) {
                    const int outer=std::max(1,metrics_.Scale(2));
                    frame(0,outer,Gdiplus::Color(255,255,255,255));
                    frame(outer,std::max(1,metrics_.Scale(1)),Gdiplus::Color(255,0,0,0));
                } else frame(0,std::max(1,metrics_.Scale(3)),Gdiplus::Color(62,255,255,255));
            } else {
                // Fallback only if GDI+ cannot paint the target DC.
        if (hovered && !tileDragging_ && drawRc.right > drawRc.left && drawRc.bottom > drawRc.top) {
            // the reference implementation's RT-inspired Metro skin uses a 3-DIP white border at
            // opacity 0.242 for pointer hover. Its tile keyboard FocusVisual
            // instead has a 2-DIP white rectangle and a 1-DIP inner black one.
            // This is a third-party reconstruction, not a verified RT asset.
            if (keyboardNav_) {
                RECT focus = drawRc;
                HBRUSH white = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
                HBRUSH black = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
                const int whiteWidth = std::max(1, metrics_.Scale(2));
                for (int i = 0; i < whiteWidth && focus.right > focus.left && focus.bottom > focus.top; ++i) {
                    FrameRect(hdc, &focus, white);
                    InflateRect(&focus, -1, -1);
                }
                const int blackWidth = std::max(1, metrics_.Scale(1));
                for (int i = 0; i < blackWidth && focus.right > focus.left && focus.bottom > focus.top; ++i) {
                    FrameRect(hdc, &focus, black);
                    InflateRect(&focus, -1, -1);
                }
            } else {
                // Composite against each actual background pixel, including
                // the gradient; a solid approximation changes color at edges.
                const int thickness = std::max(1, std::min(metrics_.Scale(3),
                    std::min(static_cast<int>(drawRc.right - drawRc.left), static_cast<int>(drawRc.bottom - drawRc.top)) / 2));
                HDC source = CreateCompatibleDC(hdc);
                HBITMAP pixel = source ? CreateCompatibleBitmap(hdc, 1, 1) : nullptr;
                if (pixel) {
                    HGDIOBJ old = SelectObject(source, pixel);
                    RECT one{0,0,1,1}; FillRect(source, &one, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
                    BLENDFUNCTION blend{AC_SRC_OVER, 0, 62, 0};
                    const int width=drawRc.right-drawRc.left, height=drawRc.bottom-drawRc.top;
                    GdiAlphaBlend(hdc, drawRc.left, drawRc.top, width, thickness, source, 0, 0, 1, 1, blend);
                    GdiAlphaBlend(hdc, drawRc.left, drawRc.bottom-thickness, width, thickness, source, 0, 0, 1, 1, blend);
                    if (height > 2 * thickness) {
                        GdiAlphaBlend(hdc, drawRc.left, drawRc.top+thickness, thickness, height-2*thickness, source, 0, 0, 1, 1, blend);
                        GdiAlphaBlend(hdc, drawRc.right-thickness, drawRc.top+thickness, thickness, height-2*thickness, source, 0, 0, 1, 1, blend);
                    }
                    if (old) SelectObject(source, old);
                    DeleteObject(pixel);
                }
                if (source) DeleteDC(source);
            }
        }
            }
        }
    }

    int HitTestNormal(int relY) const {
        int curY = metrics_.Scale(Metrics::BASE_FIRST_LIST_INSET) - mainListScrollY_;
        int itemH = metrics_.Scale(Metrics::BASE_APP_ITEM_H);
        for (int i = 0; i < (int)catalog_.MostUsed().size(); ++i) {
            if (relY >= curY && relY < curY + itemH) return i;
            curY += itemH;
        }
        if (!catalog_.RecentlyAdded().empty()) {
            curY += metrics_.Scale(Metrics::BASE_LIST_SECTION_GAP);
            for (int j = 0; j < (int)catalog_.RecentlyAdded().size(); ++j) {
                if (relY >= curY && relY < curY + itemH) return (int)catalog_.MostUsed().size() + j;
                curY += itemH;
            }
        }
        return -1;
    }

    int HitTestAllApps(int relY) const {
        EnsureAllAppsRows();
        int curY = -allAppsScrollY_;
        const int itemH = ListItemHeight();
        int idx = 0;
        for (const auto& row : allAppsRows_) {
            if (relY >= curY && relY < curY + itemH) return idx;
            curY += itemH;
            idx++;
            if (row.isFolder) {
                const auto& folder = catalog_.Sections()[row.section].folders[row.index];
                if (folder.expanded) {
                    for (size_t sub = 0; sub < folder.apps.size(); ++sub) {
                        if (relY >= curY && relY < curY + itemH) return idx;
                        curY += itemH;
                        idx++;
                    }
                }
            }
        }
        return -1;
    }

    // ===================== HIT TESTING =====================
    void HitTest(POINT pt, int& outArea, int& outIndex, int& outGroup) const {
        outArea = -1; outIndex = -1; outGroup = -1;
        RECT rcClient{};
        GetClientRect(hwnd_.get(), &rcClient);

        int leftW = metrics_.Scale(Metrics::BASE_LEFT_COL_W);
        int userH = ListAreaTopPx();
        int placeH = metrics_.Scale(Metrics::BASE_PLACE_ITEM_H);
        int bottomH = BottomPlaceItemCount() * placeH;
        int bottomY = rcClient.bottom - bottomH;

        if (pt.x < leftW) {
            if (pt.y < userH) {
                if (pt.x >= leftW - metrics_.Scale(44)) outArea = 7; // Power button
                else outArea = 0; // User
                return;
            }
            if (pt.y >= bottomY) {
                outArea = 1; // All apps / Back (0) and the search box (1)
                outIndex = std::max(0, std::min(1, static_cast<int>((pt.y - bottomY) / placeH)));
                return;
            }
            outArea = 2; // App list
            int relY = pt.y - userH;
            if (viewMode_ == ViewMode::Normal) {
                outIndex = HitTestNormal(relY);
            } else if (viewMode_ == ViewMode::AllApps) {
                outIndex = HitTestAllApps(relY);
            } else if (viewMode_ == ViewMode::Search) {
                { const int searchY = relY + mainListScrollY_ - metrics_.Scale(8); outIndex = searchY < 0 ? -1 : searchY / metrics_.Scale(Metrics::BASE_APP_ITEM_H); }
            }
            return;
        }

        // Tile column
        int startX = leftW + metrics_.Scale(Metrics::BASE_GROUP_PAD_X);
        int startY = metrics_.Scale(Metrics::BASE_GROUP_PAD_Y);
        int groupW = Metrics::BASE_COLUMNS_PER_GROUP * metrics_.Scale(Metrics::BASE_TILE_SMALL) +
                     (Metrics::BASE_COLUMNS_PER_GROUP - 1) * metrics_.Scale(Metrics::BASE_TILE_GAP);
        int groupGap = metrics_.Scale(Metrics::BASE_GROUP_GAP);

        for (size_t gi = 0; gi < layout_.groups.size(); ++gi) {
            int gx = startX + (int)gi * (groupW + groupGap);
            int gy = startY;

            const int headerH = GroupHeaderHeight((int)gi);
            RECT rcHead = { gx, gy, gx + groupW, gy + headerH };
            if (headerH > 0 && PtInRect(&rcHead, pt)) {
                outArea = 4; outGroup = (int)gi;
                return;
            }

            int tilesStartY = gy + headerH + metrics_.Scale(4);
            const auto& g = layout_.groups[gi];
            for (size_t ti = 0; ti < g.tiles.size(); ++ti) {
                RECT rcTile = ComputeTileRect(gx, tilesStartY, g.tiles[ti]);
                if (PtInRect(&rcTile, pt)) {
                    outArea = 3; outGroup = (int)gi; outIndex = (int)ti;
                    return;
                }
            }
        }
    }

    // Copies the identity of the entry first: RecordLaunch() rebuilds the
    // catalog sections, which would invalidate a reference into them.
    void LaunchCatalogEntry(const AppEntry& entry) {
        if (entry.searchExtra) {
            if (entry.searchExtraIndex >= searchExtraCatalog_.size()) return;
            const StartScreenSearch::AppEntry extra = searchExtraCatalog_[entry.searchExtraIndex];
            Hide();
            if (entry.searchExtra==1) StartScreenSearch::LaunchSettingEntry(extra);
            else ShellExecuteW(nullptr,L"open",extra.parse.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
            return;
        }
        const std::wstring path = entry.path, aumid = entry.aumid;
        Hide();
        catalog_.RecordLaunch(path.empty() ? aumid : path);
        LaunchApplication(path, aumid);
    }

    void HandleAllAppsClick(int clickIdx) {
        EnsureAllAppsRows();
        const auto& sections = catalog_.Sections();
        int idx = 0;
        for (const auto& row : allAppsRows_) {
            if (idx == clickIdx) {
                if (row.isFolder) {
                    // Win7-like folders expand in place; the flat row order is
                    // unaffected, only the visible children change.
                    catalog_.ToggleFolder((size_t)row.section, (size_t)row.index);
                    InvalidateRect(hwnd_.get(), nullptr, FALSE);
                } else {
                    LaunchCatalogEntry(sections[row.section].apps[row.index]);
                }
                return;
            }
            idx++;
            if (row.isFolder) {
                const auto& f = sections[row.section].folders[row.index];
                if (f.expanded) {
                    for (const auto& sub : f.apps) {
                        if (idx == clickIdx) { LaunchCatalogEntry(sub); return; }
                        idx++;
                    }
                }
            }
        }
    }

    // ===================== CONTEXT MENUS: owner-drawn HMENU (ImmersiveStartMenu) =====================
    // Tile and app context menus of Win8.1 RT3: white, Segoe UI Semibold,
    // accent-filled hot row (values of the reference mod).
    ImmersiveStartMenu::Style Rt3ContextStyle() const {
        ImmersiveStartMenu::Style st;
        st.use = true; st.rt3Context = true;
        st.accent = GetSystemAccentColor(settings_.classicAccent);
        return st;
    }

    bool ShowMetroMenuAt(POINT pt,const std::vector<MetroPopupItem>& items,
                         const ImmersiveStartMenu::Style* style=nullptr) {
        UniqueMenu menu(CreatePopupMenu());
        if(!menu) return false;
        for(const auto& item:items) {
            if(item.separator) { if(!AppendMenuW(menu.get(),MF_SEPARATOR,0,nullptr)) return false; }
            else {
                UINT flags=MF_STRING;
                if(item.heading || item.command==0) flags|=MF_GRAYED;
                if(!AppendMenuW(menu.get(),flags,item.command,item.text.c_str())) return false;
            }
        }
        TrackImmersiveMenu(menu.get(),pt,false,style);
        return true;
    }

    void TrackImmersiveMenu(HMENU menu,POINT pt,bool compact=false,
                            const ImmersiveStartMenu::Style* style=nullptr) {
        if(!hwnd_||!menu) { HandleMetroPopupResult(0,1); return; }
        ClientToScreen(hwnd_.get(),&pt);
        (void)compact; (void)style;
        HMODULE module = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&StartScreenPopup::Track), &module);
        const StartScreenPopup::Result result = StartScreenPopup::Track(menu, hwnd_.get(), pt, metrics_.dpi, module);
        Wh_Log(L"[ContextMenu] command=%u reason=%d",result.command,result.reason);
        HandleMetroPopupResult(result.command, result.reason);
    }

    void ClearPendingContext() {
        pendingContext_ = PendingContext::None;
        pendingGroup_ = pendingTile_ = -1;
        pendingAppName_.clear();
        pendingAppPath_.clear();
        pendingAppAumid_.clear();
        pendingSearchExtra_.reset();
        pendingSearchKind_ = 0;
    }

    void ShowTileContextMenu(POINT pt,int grp,int idx) {
        if(grp<0||grp>=static_cast<int>(layout_.groups.size())) return;
        auto& group=layout_.groups[grp];
        if(idx<0||idx>=static_cast<int>(group.tiles.size())) return;
        const auto& tile=group.tiles[idx];
        UniqueMenu menu(CreatePopupMenu());
        if(!menu) return;
        AppendMenuW(menu.get(),MF_STRING,IDM_UNPIN_FROM_START,Ui(UiText::Unpin));
        HMENU resizeRaw=CreatePopupMenu();
        if(resizeRaw) {
            UniqueMenu resize(resizeRaw);
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_SMALL,Ui(UiText::Small));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_MEDIUM,Ui(UiText::Medium));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_WIDE,Ui(UiText::Wide));
            AppendMenuW(resize.get(),MF_STRING,IDM_RESIZE_LARGE,Ui(UiText::Large));
            if(AppendMenuW(menu.get(),MF_POPUP|MF_STRING,
                reinterpret_cast<UINT_PTR>(resize.get()),Ui(UiText::Resize))) resize.release();
        }
        // Offered only for tiles that have real local content (Calendar);
        // no other app's live tile can be read through documented APIs.
        if (CanHaveLocalLiveContent(tile))
            AppendMenuW(menu.get(),MF_STRING,IDM_TOGGLE_LIVE,
                tile.isLive?Ui(UiText::DisableLive):Ui(UiText::EnableLive));
        if (IsFileBackedEntry(tile.path)) {
            AppendMenuW(menu.get(),MF_SEPARATOR,0,nullptr);
            AppendMenuW(menu.get(),MF_STRING,IDM_RUNAS_ADMIN,Ui(UiText::RunAsAdmin));
            AppendMenuW(menu.get(),MF_STRING,IDM_OPEN_LOCATION,Ui(UiText::OpenLocation));
        }
        pendingContext_=PendingContext::Tile; pendingGroup_=grp; pendingTile_=idx;
        { ImmersiveStartMenu::Style cs = Rt3ContextStyle(); TrackImmersiveMenu(menu.get(),pt,false,&cs); }
    }

    const AppEntry* FindAllAppsAppByIndex(int targetIndex) const {
        if (targetIndex < 0) return nullptr;
        EnsureAllAppsRows();
        const auto& sections = catalog_.Sections();
        int index = 0;
        for (const auto& row : allAppsRows_) {
            if (index++ == targetIndex && !row.isFolder)
                return &sections[row.section].apps[row.index];
            if (row.isFolder) {
                const auto& folder = sections[row.section].folders[row.index];
                if (folder.expanded) {
                    for (const auto& app : folder.apps) {
                        if (index++ == targetIndex) return &app;
                    }
                }
            }
        }
        return nullptr;
    }

    void ShowAppContextMenu(POINT pt,int idx) {
        const AppEntry* app = nullptr;
        if (viewMode_ == ViewMode::Normal) {
            const int frequentCount = static_cast<int>(catalog_.MostUsed().size());
            if (idx >= 0 && idx < frequentCount) app = &catalog_.MostUsed()[idx];
            else {
                const int recentIndex = idx - frequentCount;
                if (recentIndex >= 0 && recentIndex < static_cast<int>(catalog_.RecentlyAdded().size()))
                    app = &catalog_.RecentlyAdded()[recentIndex];
            }
        } else if (viewMode_ == ViewMode::Search && idx >= 0 && idx < static_cast<int>(searchResults_.size())) {
            app = &searchResults_[idx];
        } else if (viewMode_ == ViewMode::AllApps) {
            app = FindAllAppsAppByIndex(idx);
        }
        if (!app) return;
        if (app->searchExtra) {
            if ((app->searchExtra != 1 && app->searchExtra != 2) ||
                app->searchExtraIndex >= searchExtraCatalog_.size()) return;
            // The popup pumps messages: keep a copy rather than a pointer or an
            // index into a search vector that may grow or be replaced meanwhile.
            pendingSearchExtra_ = std::make_unique<StartScreenSearch::AppEntry>(searchExtraCatalog_[app->searchExtraIndex]);
            pendingSearchKind_ = app->searchExtra;
            pendingContext_ = PendingContext::SearchExtra;
            std::vector<MetroPopupItem> menu{{Ui(UiText::Open), IDM_OPEN_APP}};
            ImmersiveStartMenu::Style cs = Rt3ContextStyle();
            if (!ShowMetroMenuAt(pt, menu, &cs)) ClearPendingContext();
            return;
        }
        std::vector<MetroPopupItem> menu = {
            {Ui(UiText::Open), IDM_OPEN_APP},
            {Ui(UiText::PinToStart), IDM_PIN_TO_START}};
        // "Run as administrator" / "Open file location" only make sense for
        // shortcut entries; AppsFolder (shell:AppsFolder\<AUMID>) items have no
        // file to elevate or to select in Explorer.
        if (IsFileBackedEntry(app->path)) {
            menu.push_back({Ui(UiText::RunAsAdmin), IDM_RUNAS_ADMIN});
            menu.push_back({Ui(UiText::OpenLocation), IDM_OPEN_LOCATION});
        }
        pendingContext_ = PendingContext::App;
        pendingAppName_ = app->name;
        pendingAppPath_ = app->path;
        pendingAppAumid_ = app->aumid;
        { ImmersiveStartMenu::Style cs = Rt3ContextStyle(); if (!ShowMetroMenuAt(pt, menu, &cs)) ClearPendingContext(); }
    }

    static bool IsFileBackedEntry(const std::wstring& path) {
        return !path.empty() && _wcsnicmp(path.c_str(), L"shell:", 6) != 0;
    }

    // Renders the circular account picture into a 32bpp ARGB bitmap for the
    // account flyout header. The caller owns the returned bitmap. The zeroed
    // DIB keeps fully transparent corners, so the steel panel shows through
    // when the header is drawn with AlphaBlend.
    HBITMAP RenderFlyoutAvatar(int px) {
        if(px<=0) return nullptr;
        BITMAPINFO bi{}; bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth=px; bi.bmiHeader.biHeight=-px; bi.bmiHeader.biPlanes=1;
        bi.bmiHeader.biBitCount=32; bi.bmiHeader.biCompression=BI_RGB;
        void* bits=nullptr;
        HBITMAP hb=CreateDIBSection(nullptr,&bi,DIB_RGB_COLORS,&bits,nullptr,0);
        if(!hb) return nullptr;
        bool drew=false;
        HDC screen=GetDC(nullptr);
        HDC dc=screen?CreateCompatibleDC(screen):nullptr;
        if(dc){
            HGDIOBJ old=SelectObject(dc,hb);
            {
                std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
                if(g_profilePhoto&&g_profilePhoto->GetLastStatus()==Gdiplus::Ok)
                    drew=DrawCircularProfilePhoto(dc,g_profilePhoto.get(),0,0,px);
                if(!drew&&g_defaultProfileAvatar)
                    drew=DrawCircularProfilePhoto(dc,g_defaultProfileAvatar.get(),0,0,px);
            }
            if(old)SelectObject(dc,old);
            DeleteDC(dc);
        }
        if(screen)ReleaseDC(nullptr,screen);
        if(!drew){DeleteObject(hb);return nullptr;}
        return hb;
    }

    void ShowUserMenu(POINT pt) {
        std::vector<MetroPopupItem> menu={{Ui(UiText::EditAccount),IDM_USER_SETTINGS},{Ui(UiText::Lock),IDM_USER_LOCK},{Ui(UiText::SignOut),IDM_USER_SIGNOUT}};
        pendingContext_=PendingContext::User;
        ImmersiveStartMenu::Style style = Rt3ContextStyle();
        if (!ShowMetroMenuAt(pt, menu, &style)) ClearPendingContext();
    }

    void ShowPowerMenu(POINT pt) {
        const PowerMenuAvailability power = QueryPowerMenuAvailability();
        std::vector<MetroPopupItem> menu;
        if (power.sleep) menu.push_back({Ui(UiText::Sleep),IDM_POWER_SLEEP});
        if (power.hibernate) menu.push_back({Ui(UiText::Hibernate),IDM_POWER_HIBERNATE});
        menu.push_back({Ui(UiText::Shutdown),IDM_POWER_SHUTDOWN});
        menu.push_back({Ui(UiText::Restart),IDM_POWER_RESTART});
        pendingContext_=PendingContext::Power;
        // The power flyout shares the account flyout's steel styling.
        ImmersiveStartMenu::Style style;
        style.use=true; style.rt3Context=true; style.accentHot=true;
        style.accent=GetSystemAccentColor(settings_.classicAccent);
        if(!ShowMetroMenuAt(pt,menu,&style))ClearPendingContext();
    }

    void HandleMetroPopupResult(UINT command,int reason) {
        if (reason == 2) { // outside the owner and flyout: dismiss Start as well
            ClearPendingContext();
            Hide();
            return;
        }
        const PendingContext context = pendingContext_;
        const int groupIndex = pendingGroup_;
        const int tileIndex = pendingTile_;
        const std::wstring appName = pendingAppName_;
        const std::wstring appPath = pendingAppPath_;
        const std::wstring appAumid = pendingAppAumid_;
        const int searchKind = pendingSearchKind_;
        auto searchExtra = std::move(pendingSearchExtra_);
        ClearPendingContext();
        if (!command || reason == 3) return;

        if (context == PendingContext::Tile) {
            if (groupIndex < 0 || groupIndex >= static_cast<int>(layout_.groups.size())) return;
            auto& tiles = layout_.groups[groupIndex].tiles;
            if (tileIndex < 0 || tileIndex >= static_cast<int>(tiles.size())) return;
            auto& tile = tiles[tileIndex];
            if (command == IDM_UNPIN_FROM_START) {
                tiles.erase(tiles.begin() + tileIndex);
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command >= IDM_RESIZE_SMALL && command <= IDM_RESIZE_LARGE) {
                const TileSize requestedSize = command == IDM_RESIZE_SMALL ? TileSize::Small :
                                               command == IDM_RESIZE_MEDIUM ? TileSize::Medium :
                                               command == IDM_RESIZE_WIDE ? TileSize::Wide : TileSize::Large;
                int placedCol = tile.col, placedRow = tile.row;
                if (!FindTilePlacement(groupIndex, tileIndex, tile.col, tile.row, requestedSize,
                                       false, placedCol, placedRow)) {
                    Wh_Log(L"[TileLayout] resize rejected because no collision-free cell is available");
                    return;
                }
                const std::vector<POINT> beforeResize = CaptureTileRenderPositions(groupIndex);
                tile.size = requestedSize;
                tile.col = placedCol;
                tile.row = placedRow;
                // The resized tile reaches its new cell with a short glide.
                StartTileGlide(groupIndex, beforeResize);
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command == IDM_TOGGLE_LIVE && CanHaveLocalLiveContent(tile)) {
                tile.isLive = !tile.isLive; tile.flipping = false; tile.showBack = false;
                PersistTileLayout();
                InvalidateRect(hwnd_.get(), nullptr, FALSE);
            } else if (command == IDM_RUNAS_ADMIN) {
                if (tile.available && !tile.path.empty()) {
                    const std::wstring path = tile.path; Hide(); LaunchAdmin(path);
                }
            } else if (command == IDM_OPEN_LOCATION) {
                if (!tile.path.empty()) {
                    const std::wstring path = tile.path; Hide(); OpenExplorerLocation(path);
                }
            }
            return;
        }

        if (context == PendingContext::SearchExtra) {
            if (command == IDM_OPEN_APP && searchExtra) {
                Hide();
                if (searchKind == 1) StartScreenSearch::LaunchSettingEntry(*searchExtra);
                else if (searchKind == 2) ShellExecuteW(nullptr,L"open",searchExtra->parse.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
            }
            return;
        }

        if (context == PendingContext::App) {
            if (command == IDM_OPEN_APP) {
                Hide();
                catalog_.RecordLaunch(appPath.empty() ? appAumid : appPath);
                LaunchApplication(appPath, appAumid);
            } else if (command == IDM_PIN_TO_START) {
                const AppEntry* app = nullptr;
                for (const auto& candidate : catalog_.Apps()) {
                    if ((!appPath.empty() && candidate.path == appPath) ||
                        (!appAumid.empty() && candidate.aumid == appAumid)) { app = &candidate; break; }
                }
                if (app && IsUsableCatalogApp(*app)) {
                    if (layout_.groups.empty()) {
                        TileGroup group;
                        group.title = L"Applicazioni";
                        layout_.groups.push_back(std::move(group));
                    }
                    // Reuse the collision-aware placement path used by drag-to-pin.
                    PinAppInGroup(*app, 0, 0, 0);
                }
            } else if (command == IDM_RUNAS_ADMIN) {
                Hide(); LaunchAdmin(appPath);
            } else if (command == IDM_OPEN_LOCATION) {
                Hide(); OpenExplorerLocation(appPath);
            }
            return;
        }

        if (context == PendingContext::User) {
            if (command == IDM_USER_SETTINGS) { Hide(); ShellExecuteW(nullptr,L"open",L"ms-settings:yourinfo",nullptr,nullptr,SW_SHOWNORMAL); }
            else if (command == IDM_USER_LOCK) { Hide(); LockWorkStation(); }
            else if (command == IDM_USER_SIGNOUT) { Hide(); ExitWindowsEx(EWX_LOGOFF,0); }
            return;
        }

        if (context == PendingContext::Power && command >= IDM_POWER_SLEEP && command <= IDM_POWER_HIBERNATE) {
            Hide(); ExecutePowerAction(static_cast<int>(command));
        }
    }

};

// ===================== UI THREAD, HOOKS AND NATIVE-MENU SUPPRESSION =====================
// Threads owned by the mod:
//  - UI thread: owns the menu window, published catalog, icon cache and GDI+.
//  - Catalog worker: builds a private catalog and publishes it to the UI.
//  - Hook thread: installs WH_KEYBOARD_LL / WH_MOUSE_LL and only pumps messages,
//    so the hook procedures always return quickly (LowLevelHooksTimeout).
//  - Watcher thread: waits on change notifications for the Programs folders and
//    asks the UI thread to rescan the catalog.
//  - Profile-photo worker: loads the account picture in the background.
//  - WH_GETMESSAGE hooks on the Progman / taskbar threads drop
//    WM_SYSCOMMAND/SC_TASKLIST (classic Start activation) and toggle our menu.
//  - A 250 ms debounce avoids double toggles across sources.
// g_menu has a non-trivial destructor (DestroyWindow, UnregisterClass, GDI
// objects) and is released explicitly by the UI thread; see the Windhawk wiki
// page "Global objects and process shutdown".
[[clang::no_destroy]] static std::unique_ptr<StartMenuWindow> g_menu; // UI thread only
static Settings g_settings;

constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 110;
constexpr ULONG_PTR KEY_MARKER = 0x57484B31;
constexpr WORD VK_DUMMY = 0xE8; // unassigned virtual key

static bool g_winPressed = false;   // keyboard hook state (hook thread only)
static bool g_winChorded = false;
static bool g_swallowMouseUp = false; // mouse hook state (hook thread only)
static ULONGLONG g_lastAnchorRequestTick = 0; // hook thread only

static std::atomic<ULONGLONG> g_lastToggleTick{ 0 };
static std::atomic<ULONGLONG> g_lastKeyboardTapTick{ 0 };
static std::atomic<DWORD> g_uiThreadId{ 0 };
static std::atomic<DWORD> g_hookThreadId{ 0 };
static std::atomic<HWND> g_menuHwnd{ nullptr };
static std::atomic<bool> g_stopRequested{ false };
static std::atomic<bool> g_settingsChangedPending{ false };
static std::atomic<DWORD> g_hybridChildPid{0};
static SRWLOCK g_hybridChildLock=SRWLOCK_INIT;
static HANDLE g_hybridStopEvent=nullptr;
static HANDLE g_hybridLauncherThread=nullptr;
static std::atomic<HANDLE> g_hybridChildProcess{nullptr};
static HANDLE g_hybridParentProcess=nullptr;
static HANDLE g_hybridSupervisor=nullptr;
static HANDLE g_hybridMutex=nullptr;
static std::atomic<bool> g_hybridWindhawkUnloading{false};
static constexpr wchar_t kHybridStopEventName[] = L"Local\\Win81Update3StartMenuTool.Stop.v1";

extern "C" IMAGE_DOS_HEADER __ImageBase;
static HINSTANCE GetModInstance() { return reinterpret_cast<HINSTANCE>(&__ImageBase); }

struct FindHandleScope {
    HANDLE h = INVALID_HANDLE_VALUE;
    ~FindHandleScope() { if (h != INVALID_HANDLE_VALUE) FindClose(h); }
};
struct LocalFreeScope {
    PWSTR p = nullptr;
    ~LocalFreeScope() { if (p) CoTaskMemFree(p); }
};

static bool ReadPictureFile(const std::wstring& path, std::vector<BYTE>& bytes) {
    UniqueHandle file(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr));
    if (!file) return false;
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 || size.QuadPart > 32ll * 1024 * 1024) return false;
    bytes.resize(static_cast<size_t>(size.QuadPart));
    size_t offset = 0;
    while (offset < bytes.size()) {
        DWORD got = 0;
        DWORD chunk = static_cast<DWORD>(std::min<size_t>(bytes.size() - offset, 1u << 20));
        if (!ReadFile(file.get(), bytes.data() + offset, chunk, &got, nullptr) || !got) { bytes.clear(); return false; }
        offset += got;
    }
    return true;
}

// Compatibility fallback based on the Win7Taskbar approach: Windows currently
// stores per-SID account-picture file paths here, but this registry contract is
// undocumented and may change. Keep the fully user-configurable file path first.
static bool LoadRegistryAccountPhoto(std::vector<BYTE>& result) {
    HANDLE rawToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &rawToken)) {
        Wh_Log(L"[Profile] OpenProcessToken failed: %u", GetLastError());
        return false;
    }
    UniqueHandle token(rawToken);
    DWORD tokenBytes = 0;
    GetTokenInformation(token.get(), TokenUser, nullptr, 0, &tokenBytes);
    if (!tokenBytes) return false;
    std::vector<BYTE> tokenData(tokenBytes);
    if (!GetTokenInformation(token.get(), TokenUser, tokenData.data(), tokenBytes, &tokenBytes)) {
        Wh_Log(L"[Profile] GetTokenInformation(TokenUser) failed: %u", GetLastError());
        return false;
    }
    auto* tokenUser = reinterpret_cast<TOKEN_USER*>(tokenData.data());
    LPWSTR rawSid = nullptr;
    if (!ConvertSidToStringSidW(tokenUser->User.Sid, &rawSid) || !rawSid) {
        Wh_Log(L"[Profile] ConvertSidToStringSid failed: %u", GetLastError());
        return false;
    }
    struct LocalStringDeleter { void operator()(wchar_t* p) const { if (p) LocalFree(p); } };
    std::unique_ptr<wchar_t, LocalStringDeleter> sid(rawSid);

    std::wstring subkey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AccountPicture\\Users\\";
    subkey += sid.get();
    HKEY rawKey = nullptr;
    LONG status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, subkey.c_str(), 0,
                                KEY_QUERY_VALUE | KEY_WOW64_64KEY, &rawKey);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"[Profile] AccountPicture registry key unavailable (status=%ld)", status);
        return false;
    }
    struct RegistryKeyScope { HKEY h; ~RegistryKeyScope() { if (h) RegCloseKey(h); } };
    RegistryKeyScope key{rawKey};

    static const wchar_t* values[] = {L"Image1080", L"Image448", L"Image240", L"Image96", L"Image64", L"Image48"};
    for (const wchar_t* valueName : values) {
        DWORD type = 0, bytes = 0;
        status = RegQueryValueExW(key.h, valueName, nullptr, &type, nullptr, &bytes);
        if (status != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t))
            continue;
        std::vector<wchar_t> value((bytes / sizeof(wchar_t)) + 2, wchar_t{});
        status = RegQueryValueExW(key.h, valueName, nullptr, &type,
                                  reinterpret_cast<BYTE*>(value.data()), &bytes);
        if (status != ERROR_SUCCESS || !value[0]) continue;
        std::wstring path(value.data());
        if (type == REG_EXPAND_SZ) {
            DWORD needed = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
            if (needed > 1 && needed < 32768) {
                std::vector<wchar_t> expanded(needed);
                if (ExpandEnvironmentStringsW(path.c_str(), expanded.data(), needed)) path.assign(expanded.data());
            }
        }
        if (ReadPictureFile(path, result)) {
            Wh_Log(L"[Profile] Loaded account picture from registry value %s (%u bytes)",
                   valueName, static_cast<unsigned>(result.size()));
            return true;
        }
    }
    Wh_Log(L"[Profile] AccountPicture registry key has no readable image values");
    return false;
}

static bool LoadImageFilesFromDirectory(const std::wstring& directory, std::vector<BYTE>& result) {
    const std::wstring pattern = directory + L"\\*";
    WIN32_FIND_DATAW data{};
    FindHandleScope find{FindFirstFileW(pattern.c_str(), &data)};
    if (find.h == INVALID_HANDLE_VALUE) return false;
    struct Candidate { std::wstring path; FILETIME time; };
    std::vector<Candidate> files;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const wchar_t* dot = wcsrchr(data.cFileName, L'.');
        if (!dot || (_wcsicmp(dot, L".jpg") && _wcsicmp(dot, L".jpeg") &&
                     _wcsicmp(dot, L".png") && _wcsicmp(dot, L".bmp"))) continue;
        files.push_back({directory + L"\\" + data.cFileName, data.ftLastWriteTime});
    } while (FindNextFileW(find.h, &data));
    std::sort(files.begin(), files.end(), [](const Candidate& a, const Candidate& b) {
        return CompareFileTime(&a.time, &b.time) > 0;
    });
    if (files.size() > 32) files.resize(32);
    for (const auto& candidate : files) {
        if (ReadPictureFile(candidate.path, result)) {
            Wh_Log(L"[Profile] Loaded account picture from roaming folder (%u bytes)",
                   static_cast<unsigned>(result.size()));
            return true;
        }
    }
    return false;
}

static std::vector<BYTE> ExtractAccountPicturePayload(const std::vector<BYTE>& file) {
    // .accountpicture-ms files usually contain two concatenated images. Look for
    // JPEG/PNG markers and validate the final decode with GDI+.
    std::vector<BYTE> best;
    const BYTE pngSig[] = {0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
    for (size_t i = 0; i + 8 < file.size(); ++i) {
        size_t end = 0;
        if (std::equal(std::begin(pngSig), std::end(pngSig), file.begin() + i)) {
            for (size_t j = i + 8; j + 8 <= file.size(); ++j) {
                if (file[j] == 'I' && file[j+1] == 'E' && file[j+2] == 'N' && file[j+3] == 'D') { end = j + 8; break; }
            }
        } else if (file[i] == 0xff && file[i+1] == 0xd8 && file[i+2] == 0xff) {
            for (size_t j = i + 3; j + 1 < file.size(); ++j) if (file[j] == 0xff && file[j+1] == 0xd9) { end = j + 2; break; }
        }
        if (end > i && end - i > best.size()) best.assign(file.begin() + i, file.begin() + end);
    }
    return best;
}

static bool LoadShellAccountPhoto(std::vector<BYTE>& result) {
    // CoInitializeEx is documented as a prerequisite for the known-folder APIs.
    ScopedCom com;
    if (!com.IsOk()) return false;
    PWSTR rawFolder = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_AccountPictures, KF_FLAG_DEFAULT, nullptr, &rawFolder)) || !rawFolder) return false;
    LocalFreeScope folder{rawFolder};
    std::wstring pattern = std::wstring(rawFolder) + L"\\*";
    WIN32_FIND_DATAW data{};
    FindHandleScope find{FindFirstFileW(pattern.c_str(), &data)};
    if (find.h == INVALID_HANDLE_VALUE) return false;
    struct Candidate { std::wstring path; FILETIME time; };
    std::vector<Candidate> files;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        const wchar_t* dot = wcsrchr(data.cFileName, L'.');
        if (!dot || (_wcsicmp(dot, L".accountpicture-ms") && _wcsicmp(dot, L".png") &&
                     _wcsicmp(dot, L".jpg") && _wcsicmp(dot, L".jpeg"))) continue;
        files.push_back({std::wstring(rawFolder) + L"\\" + data.cFileName, data.ftLastWriteTime});
    } while (FindNextFileW(find.h, &data));
    std::sort(files.begin(), files.end(), [](const Candidate& a, const Candidate& b) {
        return CompareFileTime(&a.time, &b.time) > 0;
    });
    if (files.size() > 16) files.resize(16);
    for (const auto& candidate : files) {
        std::vector<BYTE> raw;
        if (!ReadPictureFile(candidate.path, raw)) continue;
        std::vector<BYTE> image;
        if (PathMatchSpecW(candidate.path.c_str(), L"*.accountpicture-ms")) image = ExtractAccountPicturePayload(raw);
        else image = std::move(raw);
        if (!image.empty()) { result = std::move(image); return true; }
    }
    return false;
}


static bool InitializeDefaultProfileAvatar() {
    if (!g_gdiplusToken) return false;
    if (g_defaultProfileAvatar && g_defaultProfileAvatarStream) return true;
    g_defaultProfileAvatar.reset();
    g_defaultProfileAvatarStream.Reset();
    g_defaultProfileAvatarStream.Attach(SHCreateMemStream(
        g_defaultProfileAvatarPng, static_cast<UINT>(sizeof(g_defaultProfileAvatarPng))));
    if (!g_defaultProfileAvatarStream) {
        Wh_Log(L"[Profile] Could not create a stream for the bundled fallback avatar");
        return false;
    }
    // GDI+ objects must be allocated with GdiplusBase::operator new (GdipAlloc):
    // a global `::new` would pair a CRT allocation with GdipFree on delete and
    // crash when the bitmap is released at unload.
    g_defaultProfileAvatar.reset(new Gdiplus::Bitmap(g_defaultProfileAvatarStream.Get(), FALSE));
    if (!g_defaultProfileAvatar || g_defaultProfileAvatar->GetLastStatus() != Gdiplus::Ok ||
        !g_defaultProfileAvatar->GetWidth() || !g_defaultProfileAvatar->GetHeight()) {
        Wh_Log(L"[Profile] GDI+ could not decode the bundled fallback avatar");
        g_defaultProfileAvatar.reset();
        g_defaultProfileAvatarStream.Reset();
        return false;
    }
    return true;
}

// Like the native menu, prefer the display name over the logon name.
static std::wstring ResolveAccountDisplayName() {
    wchar_t name[256] = {};
    ULONG displayLen = ARRAYSIZE(name);
    if (GetUserNameExW(NameDisplay, name, &displayLen) && name[0]) return name;
    DWORD nameLen = ARRAYSIZE(name);
    name[0] = 0;
    if (GetUserNameW(name, &nameLen) && name[0]) return name;
    return {};
}

static DWORD WINAPI ProfilePhotoWorker(void*) {
    try {
    // Account name first: it is cheap locally but may wait for a domain
    // controller, which is exactly why it runs here and not on the UI thread.
    if (!g_settings.privacyMode) {
        std::wstring name = ResolveAccountDisplayName();
        if (!g_profilePhotoStop.load()) {
            {
                std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
                g_accountNamePending = std::move(name);
                g_accountNamePendingValid = true;
            }
            if (HWND hwnd = g_menuHwnd.load()) PostMessageW(hwnd, WM_APP_PROFILE_PHOTO_READY, 0, 0);
        }
    }

    std::vector<BYTE> bytes;
    std::wstring explicitPath;
    { std::lock_guard<std::mutex> lock(g_profilePhotoPathMutex); explicitPath = g_profilePhotoPath; }
    if (!explicitPath.empty() && ReadPictureFile(explicitPath, bytes)) {
        Wh_Log(L"[Profile] Loaded configured picture file (%u bytes)", static_cast<unsigned>(bytes.size()));
    } else {
        if (!explicitPath.empty())
            Wh_Log(L"[Profile] Configured profilePicturePath could not be read; trying Windows account sources");
        // First try the per-user registry image paths, then the roaming
        // AccountPictures directory and the known-folder fallback.
        if (!LoadRegistryAccountPhoto(bytes)) {
            std::wstring roaming;
            bool loadedRoaming = GetKnownFolderPathString(FOLDERID_RoamingAppData, roaming) &&
                LoadImageFilesFromDirectory(roaming + L"\\Microsoft\\Windows\\AccountPictures", bytes);
            if (!loadedRoaming && !LoadShellAccountPhoto(bytes))
                Wh_Log(L"[Profile] No account photo source yielded an image; avatar remains fallback");
        }
    }
    if (!bytes.empty() && !g_profilePhotoStop.load()) {
        { std::lock_guard<std::mutex> lock(g_profilePhotoMutex); g_profilePhotoPending = std::move(bytes); }
        HWND hwnd = g_menuHwnd.load();
        if (hwnd && !g_profilePhotoStop.load()) PostMessageW(hwnd, WM_APP_PROFILE_PHOTO_READY, 0, 0);
    }
    return 0;
    } catch (const std::exception& e) {
        Wh_Log(L"[Profile] C++ exception in worker: %S", e.what());
    } catch (...) {
        Wh_Log(L"[Profile] unknown C++ exception in worker");
    }
    return 0;
}

static void RestartProfilePhotoWorker() {
    g_profilePhotoStop = true;
    if (g_profilePhotoThread) {
        WaitForSingleObject(g_profilePhotoThread.get(), INFINITE);
        g_profilePhotoThread.reset();
    }
    g_profilePhoto.reset();
    g_profilePhotoStream.Reset();
    {
        std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
        g_profilePhotoPending.clear();
        g_accountNamePending.clear();
        g_accountNamePendingValid = false;
    }
    g_profilePhotoStop = false;
    g_profilePhotoThread.reset(CreateThread(nullptr, 0, ProfilePhotoWorker, nullptr, 0, nullptr));
    HWND hwnd = g_menuHwnd.load();
    if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);
}

static UniqueHandle g_hUiThread;
static UniqueHandle g_hHookThread;
static UniqueHandle g_hWatcherThread;
static UniqueHandle g_hookReadyEvent;
static UniqueHandle g_watcherStopEvent;

static HHOOK g_hKbdHook = nullptr;   // hook thread
static HHOOK g_hMouseHook = nullptr; // hook thread
static HHOOK g_hProgHook = nullptr;  // UI thread
static HHOOK g_hTrayHook = nullptr;  // UI thread

static bool RequestToggle(const wchar_t* source, HWND taskbar) {
    HWND h = g_menuHwnd.load();
    if (!h || !IsWindow(h)) return false;
    const ULONGLONG now = GetTickCount64();
    ULONGLONG last = g_lastToggleTick.load();
    do {
        if (last && now - last < 250) {
            Wh_Log(L"[Toggle] %s ignored (debounce, %u ms)", source, (unsigned)(now - last));
            return true; // another route already queued the request
        }
    } while (!g_lastToggleTick.compare_exchange_weak(last, now));
    const BOOL posted = PostMessageW(h, WM_APP_TOGGLE, 0, reinterpret_cast<LPARAM>(taskbar));
    Wh_Log(L"[Toggle] request from %s posted=%d", source, posted ? 1 : 0);
    return posted != FALSE;
}

// Optional private Explorer keyboard route, adapted from the GPL-3.0
// win81-taskbar-recreation / taskbar-auto-hide-keyboard-only compatibility
// hook. The original is always retained as fallback. This is not a documented
// Windows API and may be absent or intercepted by another mod.
using ShowStartViewFn = HRESULT(WINAPI*)(void*, int, int);
static ShowStartViewFn g_originalShowStartView = nullptr;
static int ForegroundElevationState() {
    DWORD pid = 0;
    HWND fg = GetForegroundWindow();
    if (!fg || !GetWindowThreadProcessId(fg, &pid) || !pid) return -1;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return -1;
    HANDLE token = nullptr; TOKEN_ELEVATION elevation{}; DWORD size = 0;
    const bool ok = OpenProcessToken(process, TOKEN_QUERY, &token) &&
        GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size);
    if (token) CloseHandle(token);
    CloseHandle(process);
    return ok ? (elevation.TokenIsElevated ? 1 : 0) : -1;
}
// Only the shell process may relay. Never intercept if the exact child process
// and its live window are not available; Windows' original route remains usable.
static bool BridgeSendToggle() {
    AcquireSRWLockShared(&g_hybridChildLock);
    const DWORD expected=g_hybridChildPid.load();
    HANDLE child=g_hybridChildProcess.load();
    bool posted=false;
    if (expected && child && WaitForSingleObject(child,0)==WAIT_TIMEOUT) {
        HWND menu=FindWindowW(StartMenuWindow::kWindowClassName,nullptr);
        DWORD pid=0;
        if (menu && GetWindowThreadProcessId(menu,&pid) && pid==expected &&
            GetPropW(menu,kHybridReadyProperty)==reinterpret_cast<HANDLE>(1))
            posted=PostMessageW(menu,WM_APP_BRIDGE_TOGGLE,0,0)!=FALSE;
    }
    ReleaseSRWLockShared(&g_hybridChildLock);
    return posted;
}
static HRESULT WINAPI ShowStartViewHook(void* self, int method, int flags) {
    if (method==1 && !g_stopRequested.load()) {
        DWORD shellPid=0; HWND shell=GetShellWindow();
        if (shell) GetWindowThreadProcessId(shell,&shellPid);
        DWORD fgPid=0; HWND fg=GetForegroundWindow();
        if (fg) GetWindowThreadProcessId(fg,&fgPid);
        const int elevation=ForegroundElevationState();
        if (shellPid==GetCurrentProcessId() &&
            (elevation==1 || (fgPid && fgPid==g_hybridChildPid.load())) && BridgeSendToggle()) {
            Wh_Log(L"[HybridBridge] native keyboard Start relayed, elevation=%d",elevation);
            return S_OK;
        }
    }
    return g_originalShowStartView(self,method,flags);
}
static void InstallNativeKeyboardStartHook() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) { Wh_Log(L"[NativeWin] twinui.pcshell.dll unavailable"); return; }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellHooks[] = {
        {{LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
         &g_originalShowStartView, ShowStartViewHook},
    };
    const bool ok = WindhawkUtils::HookSymbols(module, twinuiPcshellHooks,
                                               std::size(twinuiPcshellHooks));
    Wh_Log(L"[NativeWin] optional Explorer symbol hook installed=%d", ok ? 1 : 0);
    if (!ok) FreeLibrary(module); // retained until mod unload when installed
}

// UI thread: recompute the Start button rectangle of every taskbar and publish
// the result for the mouse hook.
static void RefreshStartRects(bool logIt) {
    std::vector<CachedStartButton> fresh;
    std::vector<HWND> taskbars;
    if (HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr)) taskbars.push_back(primary);
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr)
        taskbars.push_back(secondary);

    for (HWND taskbar : taskbars) {
        StartAnchor a;
        if (!GetStartAnchor(a, taskbar)) continue;
        CachedStartButton entry;
        entry.taskbar = taskbar;
        entry.taskbarRect = a.taskbarRect;
        entry.startRect = a.startButton;
        entry.reliable = a.method != 3; // the geometric fallback is not trusted for click capture
        fresh.push_back(entry);
        if (logIt)
            Wh_Log(L"[Anchor] taskbar=%p method=%d rect=(%ld,%ld,%ld,%ld) edge=%u valid=%d",
                   taskbar, a.method, a.startButton.left, a.startButton.top, a.startButton.right,
                   a.startButton.bottom, (unsigned)a.edge, entry.reliable ? 1 : 0);
        if (!entry.reliable && logIt)
            Wh_Log(L"[Anchor] Start button NOT found reliably on taskbar %p; its clicks will not be intercepted, only the Win key", taskbar);
    }

    AcquireSRWLockExclusive(&g_startCacheLock);
    g_startCache.swap(fresh);
    ReleaseSRWLockExclusive(&g_startCacheLock);
    g_startCacheTick = GetTickCount64();
}

// Hook thread helpers: read-only lookups in the published cache.
static bool PointInAnyTaskbar(POINT pt) {
    AcquireSRWLockShared(&g_startCacheLock);
    bool inside = false;
    for (const auto& e : g_startCache) {
        if (PtInRect(&e.taskbarRect, pt)) { inside = true; break; }
    }
    ReleaseSRWLockShared(&g_startCacheLock);
    return inside;
}

static bool FindStartButtonAt(POINT pt, HWND& taskbarOut) {
    AcquireSRWLockShared(&g_startCacheLock);
    bool hit = false;
    for (const auto& e : g_startCache) {
        if (e.reliable && PtInRect(&e.startRect, pt)) { taskbarOut = e.taskbar; hit = true; break; }
    }
    ReleaseSRWLockShared(&g_startCacheLock);
    return hit;
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (kbd->dwExtraInfo == KEY_MARKER) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam); // event injected by us
        }
        bool isWin = (kbd->vkCode == VK_LWIN || kbd->vkCode == VK_RWIN);
        bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (down) {
            if (isWin) {
                if (!g_winPressed) { // ignore auto-repeat
                    g_winPressed = true;
                    g_winChorded = false;
                }
            } else if (g_winPressed) {
                g_winChorded = true; // Win+R, Win+E, ...
            }
        } else if (up && isWin) {
            bool tap = g_winPressed && !g_winChorded;
            g_winPressed = false;
            g_winChorded = false;
            if (tap) {
                Wh_Log(L"[Key] isolated Win tap detected");
                // Inert key BEFORE the Win release: cancels the native menu.
                INPUT inp[2]{};
                inp[0].type = INPUT_KEYBOARD;
                inp[0].ki.wVk = VK_DUMMY;
                inp[0].ki.dwExtraInfo = KEY_MARKER;
                inp[1].type = INPUT_KEYBOARD;
                inp[1].ki.wVk = VK_DUMMY;
                inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
                inp[1].ki.dwExtraInfo = KEY_MARKER;
                const UINT sent = SendInput(2, inp, sizeof(INPUT));
                if (sent != 2) Wh_Log(L"[Key] SendInput incomplete (%u/2); native keyboard route is best effort", sent);
                g_lastKeyboardTapTick = GetTickCount64();
                RequestToggle(L"Win key");
                // the Win release passes through normally (no stuck key)
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        const POINT pt = ms->pt;
        if (wParam == WM_MOUSEMOVE) {
            // Ask the UI thread for fresh Start rectangles while the pointer is over
            // a taskbar (throttled), so a centered Windows 11 taskbar whose icons
            // shifted is measured again before the click arrives.
            if (PointInAnyTaskbar(pt)) {
                const ULONGLONG now = GetTickCount64();
                if (now - g_lastAnchorRequestTick > 600 && now - g_startCacheTick.load() > 600) {
                    g_lastAnchorRequestTick = now;
                    if (HWND h = g_menuHwnd.load()) PostMessageW(h, WM_APP_REFRESH_ANCHOR, 0, 0);
                }
            }
        } else if (wParam == WM_LBUTTONDOWN) {
            HWND taskbar = nullptr;
            if (FindStartButtonAt(pt, taskbar)) {
                // Only when a taskbar is really under the cursor: a fullscreen app
                // covering that corner must keep receiving its clicks.
                HWND root = GetAncestor(WindowFromPoint(pt), GA_ROOT);
                if (root && (root == taskbar || IsTaskbarWindow(root))) {
                    g_swallowMouseUp = true;
                    Wh_Log(L"[Mouse] click on Start button at (%ld,%ld)", pt.x, pt.y);
                    RequestToggle(L"Start click", taskbar);
                    return 1; // keeps the native menu from opening
                }
            }
            HWND menu = g_menuHwnd.load();
            if (menu && IsWindowVisible(menu)) {
                RECT rcMenu{};
                if (GetWindowRect(menu, &rcMenu) && !PtInRect(&rcMenu, pt) &&
                    !StartScreenPopup::PointInPopupForMouseHook(pt)) {
                    PostMessageW(menu, WM_APP_HIDE, 0, 0); // light dismiss outside Start and its popups
                }
            }
        } else if (wParam == WM_LBUTTONUP && g_swallowMouseUp) {
            g_swallowMouseUp = false;
            return 1;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Runs on the Progman / Shell_TrayWnd thread: PostMessage only in here.
static LRESULT CALLBACK ShellThreadGetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == PM_REMOVE) {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == WM_SYSCOMMAND && (msg->wParam & 0xFFF0) == SC_TASKLIST) {
            if (BridgeSendToggle()) msg->message = WM_NULL;
            // No child/window: leave native activation untouched.
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Dedicated thread for the low-level hooks: it only pumps messages, so the
// hook procedures are never delayed by catalog scans or UI Automation.
static DWORD WINAPI HookThreadProc(LPVOID) {
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hookThreadId = GetCurrentThreadId();

    const HINSTANCE hMod = GetModInstance();
    g_hKbdHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
    Wh_Log(L"[Hooks] WH_KEYBOARD_LL: %s (err=%u)", g_hKbdHook ? L"ok" : L"FAILED", g_hKbdHook ? 0 : GetLastError());
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
    Wh_Log(L"[Hooks] WH_MOUSE_LL: %s (err=%u)", g_hMouseHook ? L"ok" : L"FAILED", g_hMouseHook ? 0 : GetLastError());
    if (g_hookReadyEvent) SetEvent(g_hookReadyEvent.get());

    // Keep the UI and bridge alive in degraded mode and retry missing input
    // hooks on this same message-pumping thread at a bounded rate.
    UINT_PTR retryTimer=SetTimer(nullptr, 0, 5000, nullptr);
    if (!retryTimer) Wh_Log(L"[Hooks] retry timer failed: %u",GetLastError());
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message==WM_TIMER && msg.wParam==retryTimer) {
            if (!g_hKbdHook) {
                g_hKbdHook=SetWindowsHookExW(WH_KEYBOARD_LL,LowLevelKeyboardProc,hMod,0);
                Wh_Log(L"[Hooks] keyboard retry: %s",g_hKbdHook ? L"ok" : L"failed");
            }
            if (!g_hMouseHook) {
                g_hMouseHook=SetWindowsHookExW(WH_MOUSE_LL,LowLevelMouseProc,hMod,0);
                Wh_Log(L"[Hooks] mouse retry: %s",g_hMouseHook ? L"ok" : L"failed");
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (retryTimer) KillTimer(nullptr,retryTimer);

    if (g_hKbdHook) { UnhookWindowsHookEx(g_hKbdHook); g_hKbdHook = nullptr; }
    if (g_hMouseHook) { UnhookWindowsHookEx(g_hMouseHook); g_hMouseHook = nullptr; }
    g_hookThreadId = 0;
    Wh_Log(L"[Hooks] hook thread finished");
    return 0;
}

static void InstallShellMessageHooks() {
    // A replacement taskbar may also use Shell_TrayWnd in another process.
    // A null module handle is valid only for threads in this process.
    HWND shell = GetShellWindow();
    if (!shell) shell = FindWindowW(L"Progman", nullptr);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD shellPid=0, trayPid=0;
    DWORD tp = shell ? GetWindowThreadProcessId(shell, &shellPid) : 0;
    DWORD tt = tray ? GetWindowThreadProcessId(tray, &trayPid) : 0;
    if (tp && shellPid == GetCurrentProcessId()) {
        g_hProgHook = SetWindowsHookExW(WH_GETMESSAGE, ShellThreadGetMsgProc, nullptr, tp);
        Wh_Log(L"[Hooks] WH_GETMESSAGE shell thread %u: %s", tp, g_hProgHook ? L"ok" : L"FAILED");
    }
    if (tt && tt != tp && trayPid == GetCurrentProcessId()) {
        g_hTrayHook = SetWindowsHookExW(WH_GETMESSAGE, ShellThreadGetMsgProc, nullptr, tt);
        Wh_Log(L"[Hooks] WH_GETMESSAGE taskbar thread %u: %s", tt, g_hTrayHook ? L"ok" : L"FAILED");
    } else if (tt && trayPid != GetCurrentProcessId()) {
        Wh_Log(L"[Hooks] foreign taskbar PID %u; not hooking that thread", trayPid);
    }
}

static void RemoveShellMessageHooks() {
    if (g_hProgHook) { UnhookWindowsHookEx(g_hProgHook); g_hProgHook = nullptr; }
    if (g_hTrayHook) { UnhookWindowsHookEx(g_hTrayHook); g_hTrayHook = nullptr; }
}

static bool StartHookThread() {
    g_hookReadyEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!g_hookReadyEvent) { Wh_Log(L"[Hooks] ready event failed: %u",GetLastError()); return false; }
    g_hHookThread.reset(CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr));
    if (!g_hHookThread) {
        Wh_Log(L"[Hooks] CreateThread failed: %u", GetLastError());
        return false;
    }
    if (WaitForSingleObject(g_hookReadyEvent.get(), 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"[Hooks] initialization not ready");
        return false;
    }
    return true;
}

static void StopHookThread() {
    const DWORD tid = g_hookThreadId.load();
    if (tid) PostThreadMessageW(tid, WM_QUIT, 0, 0);
    if (g_hHookThread) {
        WaitForSingleObject(g_hHookThread.get(), INFINITE);
        g_hHookThread.reset();
    }
    g_hookReadyEvent.reset();
}

// Watches the user and common Programs folders; a change (install/uninstall of
// a Win32 app) asks the UI thread to rescan after a short quiet period.
static DWORD WINAPI ProgramsWatcherProc(LPVOID) {
    std::vector<HANDLE> waitHandles;
    waitHandles.push_back(g_watcherStopEvent.get());
    const KNOWNFOLDERID folders[] = { FOLDERID_Programs, FOLDERID_CommonPrograms };
    std::vector<HANDLE> notifications;
    for (const auto& id : folders) {
        std::wstring path;
        if (!GetKnownFolderPathString(id, path)) continue;
        HANDLE h = FindFirstChangeNotificationW(path.c_str(), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
        if (h != INVALID_HANDLE_VALUE) { notifications.push_back(h); waitHandles.push_back(h); }
    }
    struct Cleanup { std::vector<HANDLE>& h; ~Cleanup() { for (HANDLE x : h) FindCloseChangeNotification(x); } } cleanup{notifications};
    if (notifications.empty()) {
        Wh_Log(L"[Watcher] no Programs folder could be watched");
        return 0;
    }

    bool pending = false;
    while (true) {
        const DWORD wait = WaitForMultipleObjects(static_cast<DWORD>(waitHandles.size()), waitHandles.data(),
                                                  FALSE, pending ? 2000 : INFINITE);
        if (wait == WAIT_OBJECT_0) break; // stop
        if (wait == WAIT_TIMEOUT) {
            pending = false;
            if (HWND h = g_menuHwnd.load()) PostMessageW(h, WM_APP_REFRESH, 0, 0);
            continue;
        }
        const DWORD index = wait - WAIT_OBJECT_0;
        if (index >= 1 && index < waitHandles.size()) {
            pending = true; // debounce: wait for 2 s of quiet before refreshing
            if (!FindNextChangeNotification(waitHandles[index])) break;
        } else {
            break; // WAIT_FAILED / abandoned
        }
    }
    return 0;
}

static void StartProgramsWatcher() {
    g_watcherStopEvent.reset(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!g_watcherStopEvent) return;
    g_hWatcherThread.reset(CreateThread(nullptr, 0, ProgramsWatcherProc, nullptr, 0, nullptr));
}

static void StopProgramsWatcher() {
    if (g_watcherStopEvent) SetEvent(g_watcherStopEvent.get());
    if (g_hWatcherThread) {
        WaitForSingleObject(g_hWatcherThread.get(), INFINITE);
        g_hWatcherThread.reset();
    }
    g_watcherStopEvent.reset();
}

static bool WaitForMainExplorerReady();

static DWORD WINAPI UiThreadProc(LPVOID) {
    // Create the thread message queue before publishing its ID, so settings
    // changes or unload requests can be posted while Explorer is initializing.
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_uiThreadId = GetCurrentThreadId();
    ScopedCom com;
    Wh_Log(L"[UI] bootstrap thread started (id=%u, COM=%d)", (unsigned)g_uiThreadId.load(), com.IsOk() ? 1 : 0);
    if (!WaitForMainExplorerReady()) {
        Wh_Log(L"[UI] shell did not become ready in this Explorer process; no UI hooks installed");
        g_uiThreadId = 0;
        return 0;
    }
    if (g_settingsChangedPending.exchange(false)) g_settings.Load();

    GdiplusStartupInput gdipInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdipInput, nullptr) != Ok) {
        g_gdiplusToken = 0;
        Wh_Log(L"[GDI+] startup failed");
    }
    if (g_gdiplusToken && !InitializeDefaultProfileAvatar())
        Wh_Log(L"[Profile] bundled avatar unavailable; no drawn substitute will be used");
    Wh_Log(L"[UI] primary Explorer shell confirmed; initializing menu");

    bool ok = false;
    GUARDED(L"UiInit", {
        g_menu = std::make_unique<StartMenuWindow>();
        if (g_menu->Create(GetModInstance())) {
            g_menu->ApplySettings(g_settings);
            g_menuHwnd = g_menu->GetHwnd();
            g_profilePhotoStop = false;
            g_profilePhotoThread.reset(CreateThread(nullptr, 0, ProfilePhotoWorker, nullptr, 0, nullptr));
            RefreshStartRects(true);
            // Shell message hooks live in the Explorer bridge, not this process.
            ok = StartHookThread();
            if (ok && !SetPropW(g_menu->GetHwnd(),kHybridReadyProperty,reinterpret_cast<HANDLE>(1))) {
                Wh_Log(L"[HybridTool] ready property failed: %u",GetLastError());
                ok=false;
            }
            if (ok) StartProgramsWatcher();
        }
    })

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (!msg.hwnd && msg.message == WM_APP_SETTINGS_CHANGED) {
                GUARDED(L"Settings", {
                    g_settingsChangedPending = false;
                    g_settings.Load();
                    if (g_menu) g_menu->ApplySettings(g_settings);
                    RestartProfilePhotoWorker();
                })
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    } else {
        Wh_Log(L"[UI] initialization failed; the native Start menu stays active");
    }

    // Teardown, in dependency order, on the thread that owns the objects.
    // Each step is logged so a failure during unload can be located.
    if (g_menuHwnd.load()) RemovePropW(g_menuHwnd.load(),kHybridReadyProperty);
    Wh_Log(L"[UI] teardown: stopping hook thread");
    StopHookThread();
    Wh_Log(L"[UI] teardown: stopping Programs watcher");
    StopProgramsWatcher();
    // The bridge uninstalls its own shell-thread hooks.
    g_profilePhotoStop = true;
    if (g_profilePhotoThread) {
        WaitForSingleObject(g_profilePhotoThread.get(), INFINITE);
        g_profilePhotoThread.reset();
    }
    g_menuHwnd = nullptr;
    Wh_Log(L"[UI] teardown: destroying the Start window");
    GUARDED(L"Teardown", {
        g_menu.reset(); // destroys the window on the thread that created it
    })
    Wh_Log(L"[UI] teardown: releasing icon bitmap cache (%u entries)",
           g_iconBitmapCache ? (unsigned)g_iconBitmapCache->size() : 0u);
    g_iconBitmapCache.reset(); // GDI+ bitmaps destroyed before GdiplusShutdown
    Wh_Log(L"[UI] teardown: releasing stock icons");
    if (g_stockFolderIcon) { DestroyIcon(g_stockFolderIcon); g_stockFolderIcon=nullptr; }
    if (g_stockAppIcon) { DestroyIcon(g_stockAppIcon); g_stockAppIcon=nullptr; }
    Wh_Log(L"[UI] teardown: releasing profile photo");
    g_profilePhoto.reset();
    g_profilePhotoStream.Reset();
    Wh_Log(L"[UI] teardown: releasing default avatar");
    g_defaultProfileAvatar.reset();
    g_defaultProfileAvatarStream.Reset();
    {
        std::lock_guard<std::mutex> lock(g_profilePhotoMutex);
        g_profilePhotoPending.clear();
        g_accountNamePending.clear();
        g_accountNamePendingValid = false;
    }
    Wh_Log(L"[UI] teardown: GdiplusShutdown");
    if (g_gdiplusToken) { GdiplusShutdown(g_gdiplusToken); g_gdiplusToken=0; }
    Wh_Log(L"[UI] teardown: GDI+ shut down");
    g_uiThreadId = 0;
    Wh_Log(L"[UI] thread finished");
    return 0;
}

// GetShellWindow identifies the desktop shell, unlike Shell_TrayWnd which
// may belong to a replacement taskbar. Progman is a fallback only if the
// documented shell window is temporarily unavailable.
static DWORD SelectDesktopOwner(DWORD shellPid, DWORD progmanPid) {
    return shellPid ? shellPid : progmanPid;
}
static DWORD DesktopShellOwnerPid() {
    HWND shell = GetShellWindow();
    HWND progman = shell ? nullptr : FindWindowW(L"Progman", nullptr);
    DWORD shellPid = 0, progmanPid = 0;
    if (shell) GetWindowThreadProcessId(shell, &shellPid);
    if (progman) GetWindowThreadProcessId(progman, &progmanPid);
    return SelectDesktopOwner(shellPid,progmanPid);
}
static bool WaitForMainExplorerReady() {
    DWORD lastOwner = 0;
    while (!g_stopRequested.load()) {
        DWORD shellPid = DesktopShellOwnerPid();
        if (g_isHybridTool) {
            if (shellPid || FindWindowW(L"Shell_TrayWnd", nullptr)) return true;
        } else if (shellPid == GetCurrentProcessId()) {
            DWORD trayPid=0;
            HWND tray=FindWindowW(L"Shell_TrayWnd",nullptr);
            if (tray) GetWindowThreadProcessId(tray,&trayPid);
            Wh_Log(L"[Init] desktop shell owner PID %u; discovered taskbar PID %u", shellPid, trayPid);
            return true;
        }
        if (shellPid != lastOwner) {
            Wh_Log(L"[Init] waiting for desktop shell owner (owner=%u this=%u)",shellPid,GetCurrentProcessId());
            lastOwner = shellPid;
        }
        for (int i=0; i<10 && !g_stopRequested.load(); ++i) Sleep(100);
    }
    return false;
}

// Hybrid launcher: adapted from the Windhawk tool-mod pattern and the taskbar
// mod. Explorer owns only the native hook and message bridge; the tool owns the
// window, input hooks, catalog, and graphics. No protected system files changed.
static bool HybridStartToolRuntime() {
    g_stopRequested=false; g_settingsChangedPending=false; g_settings.Load();
    HANDLE thread=CreateThread(nullptr,0,UiThreadProc,nullptr,0,nullptr);
    if (!thread) return false;
    g_hUiThread.reset(thread); return true;
}
static void HybridStopToolRuntime() {
    g_stopRequested=true;
    DWORD tid=g_uiThreadId.load();
    if (tid) PostThreadMessageW(tid,WM_QUIT,0,0);
    if (g_hUiThread) { WaitForSingleObject(g_hUiThread.get(),INFINITE); g_hUiThread.reset(); }
    g_uiThreadId=0;
}
static DWORD WINAPI HybridSupervisor(void*) {
    // If UI initialization fails or its message loop exits, notify the parent
    // by exiting; otherwise a live but unusable child would prevent recovery.
    HANDLE signals[3]={g_hybridStopEvent,g_hybridParentProcess,g_hUiThread.get()};
    DWORD wait=WaitForMultipleObjects(3,signals,FALSE,INFINITE);
    Wh_Log(L"[HybridTool] supervisor wake=%u",wait);
    HybridStopToolRuntime();
    if (!g_hybridWindhawkUnloading.load()) ExitProcess(0);
    return 0;
}
static void WINAPI HybridEntryPointHook() {
    if (!HybridStartToolRuntime()) ExitProcess(1);
    g_hybridSupervisor=CreateThread(nullptr,0,HybridSupervisor,nullptr,0,nullptr);
    if (!g_hybridSupervisor) { HybridStopToolRuntime(); ExitProcess(1); }
    ExitThread(0);
}
static bool RetryDelay(DWORD ms) {
    while (ms && !g_stopRequested.load()) {
        DWORD step=(std::min<DWORD>)(ms,250);
        Sleep(step); ms-=step;
    }
    return !g_stopRequested.load();
}
static DWORD WINAPI HybridLauncher(void*) {
    if (!WaitForMainExplorerReady() || g_stopRequested.load()) return 0;
    InstallShellMessageHooks();
    DWORD delay=1000;
    while (!g_stopRequested.load()) {
        // A previous instance may still be cleaning up. Never reset somebody
        // else's event or start a competing instance; retry after it disappears.
        HANDLE stop=CreateEventW(nullptr,TRUE,FALSE,kHybridStopEventName);
        if (!stop) {
            Wh_Log(L"[HybridBridge] stop event failed %u; retrying",GetLastError());
            if (!RetryDelay(delay)) break;
            delay=(std::min<DWORD>)(delay*2,30000); continue;
        }
        if (GetLastError()==ERROR_ALREADY_EXISTS) {
            CloseHandle(stop);
            Wh_Log(L"[HybridBridge] previous instance still owns event; retrying");
            if (!RetryDelay(delay)) break;
            delay=(std::min<DWORD>)(delay*2,30000); continue;
        }
        g_hybridStopEvent=stop;
        if (g_stopRequested.load()) break;
        WCHAR path[MAX_PATH]{};
        DWORD len=GetModuleFileNameW(nullptr,path,ARRAYSIZE(path));
        PROCESS_INFORMATION pi{};
        BOOL launched=FALSE;
        if (len && len<ARRAYSIZE(path)) {
            std::wstring command=L"\""; command+=path; command+=L"\" -tool-mod \""; command+=WH_MOD_ID; command+=L"\"";
            command+=L" -hybrid-parent "; command+=std::to_wstring(GetCurrentProcessId());
            std::vector<wchar_t> cmd(command.begin(),command.end()); cmd.push_back(0);
            HMODULE kernel=GetModuleHandleW(L"kernelbase.dll");
            if (!kernel) kernel=GetModuleHandleW(L"kernel32.dll");
            using CreateProcessInternalW_t=BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
            auto create=kernel ? reinterpret_cast<CreateProcessInternalW_t>(GetProcAddress(kernel,"CreateProcessInternalW")) : nullptr;
            STARTUPINFOW si{}; si.cb=sizeof(si); si.dwFlags=STARTF_FORCEOFFFEEDBACK;
            if (create) launched=create(nullptr,path,cmd.data(),nullptr,nullptr,FALSE,NORMAL_PRIORITY_CLASS,nullptr,nullptr,&si,&pi,nullptr);
            if (!launched && !g_stopRequested.load()) {
                Wh_Log(L"[HybridBridge] internal launcher unavailable/failed (%u); trying CreateProcessW",GetLastError());
                cmd.assign(command.begin(),command.end()); cmd.push_back(0);
                launched=CreateProcessW(path,cmd.data(),nullptr,nullptr,FALSE,NORMAL_PRIORITY_CLASS,nullptr,nullptr,&si,&pi);
            }
        }
        if (!launched) {
            Wh_Log(L"[HybridBridge] launch failed %u; retrying",GetLastError());
            CloseHandle(g_hybridStopEvent); g_hybridStopEvent=nullptr;
            if (!RetryDelay(delay)) break;
            delay=(std::min<DWORD>)(delay*2,30000); continue;
        }
        CloseHandle(pi.hThread);
        AcquireSRWLockExclusive(&g_hybridChildLock);
        g_hybridChildProcess=pi.hProcess; g_hybridChildPid=pi.dwProcessId;
        ReleaseSRWLockExclusive(&g_hybridChildLock);
        Wh_Log(L"[HybridBridge] tool process launched pid=%u",pi.dwProcessId);
        ULONGLONG launchedAt=GetTickCount64(); bool warned=false, ready=false;
        while (!g_stopRequested.load()) {
            HANDLE waits[2]={pi.hProcess,stop};
            DWORD state=WaitForMultipleObjects(2,waits,FALSE,500);
            if (state==WAIT_OBJECT_0 || state==WAIT_OBJECT_0+1) break;
            if (state==WAIT_FAILED) {
                Wh_Log(L"[HybridBridge] child wait failed %u; avoiding duplicate launch",GetLastError());
                if (!RetryDelay(5000)) break;
                continue;
            }
            HWND menu=FindWindowW(StartMenuWindow::kWindowClassName,nullptr); DWORD owner=0;
            if (menu) GetWindowThreadProcessId(menu,&owner);
            if (owner==pi.dwProcessId && GetPropW(menu,kHybridReadyProperty)==reinterpret_cast<HANDLE>(1)) ready=true;
            if (!ready && !warned && GetTickCount64()-launchedAt>30000) {
                Wh_Log(L"[HybridBridge] tool still not ready; keeping native route, no duplicate launch"); warned=true;
            }
        }
        if (g_stopRequested.load() || WaitForSingleObject(stop,0)==WAIT_OBJECT_0) break;
        DWORD exitCode=0; GetExitCodeProcess(pi.hProcess,&exitCode);
        Wh_Log(L"[HybridBridge] tool ended code=%u; retrying",exitCode);
        AcquireSRWLockExclusive(&g_hybridChildLock);
        g_hybridChildPid=0; g_hybridChildProcess=nullptr;
        ReleaseSRWLockExclusive(&g_hybridChildLock);
        CloseHandle(pi.hProcess);
        CloseHandle(g_hybridStopEvent); g_hybridStopEvent=nullptr;
        if (ready) delay=1000;
        if (!RetryDelay(delay)) break;
        delay=(std::min<DWORD>)(delay*2,30000);
    }
    return 0;
}
BOOL Wh_ModInit() {
    DWORD session=0; if (ProcessIdToSessionId(GetCurrentProcessId(),&session) && session==0) return FALSE;
    int argc=0; LPWSTR* args=CommandLineToArgvW(GetCommandLineW(),&argc);
    if (!args) return FALSE;
    bool excluded=false, tool=false, ours=false; DWORD parentPid=0;
    for (int i=1;i<argc;++i) {
        if (!wcscmp(args[i],L"-service") || !wcscmp(args[i],L"-service-start") || !wcscmp(args[i],L"-service-stop")) excluded=true;
        if (!wcscmp(args[i],L"-tool-mod") && i+1<argc) { tool=true; ours=!wcscmp(args[i+1],WH_MOD_ID); }
        if (!wcscmp(args[i],L"-hybrid-parent") && i+1<argc) parentPid=wcstoul(args[i+1],nullptr,10);
    }
    LocalFree(args);
    if (excluded || (tool && !ours)) return FALSE;
    if (ours) {
        g_isHybridTool=true;
        g_hybridMutex=CreateMutexW(nullptr,TRUE,L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_hybridMutex || GetLastError()==ERROR_ALREADY_EXISTS) ExitProcess(1);
        g_hybridStopEvent=OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,kHybridStopEventName);
        if (!g_hybridStopEvent || !parentPid) ExitProcess(1);
        g_hybridParentProcess=OpenProcess(SYNCHRONIZE,FALSE,parentPid);
        if (!g_hybridParentProcess) ExitProcess(1);
        IMAGE_DOS_HEADER* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<BYTE*>(dos)+dos->e_lfanew);
        void* entry=reinterpret_cast<BYTE*>(dos)+nt->OptionalHeader.AddressOfEntryPoint;
        if (!Wh_SetFunctionHook(entry,reinterpret_cast<void*>(HybridEntryPointHook),nullptr)) ExitProcess(1);
        return TRUE;
    }
    g_stopRequested=false;
    InstallNativeKeyboardStartHook();
    return TRUE;
}
void Wh_ModAfterInit() {
    if (g_isHybridTool) return;
    g_hybridLauncherThread=CreateThread(nullptr,0,HybridLauncher,nullptr,0,nullptr);
    if (!g_hybridLauncherThread) Wh_Log(L"[HybridBridge] launcher thread creation failed");
}
void Wh_ModSettingsChanged() {
    if (!g_isHybridTool) return;
    g_settingsChangedPending=true;
    DWORD tid=g_uiThreadId.load();
    if (tid) PostThreadMessageW(tid,WM_APP_SETTINGS_CHANGED,0,0);
}
void Wh_ModBeforeUninit() {
    if (g_isHybridTool) {
        g_hybridWindhawkUnloading=true;
        if (g_hybridStopEvent) SetEvent(g_hybridStopEvent);
        if (g_hybridSupervisor) { WaitForSingleObject(g_hybridSupervisor,INFINITE); CloseHandle(g_hybridSupervisor); g_hybridSupervisor=nullptr; }
        else HybridStopToolRuntime();
        return;
    }
    g_stopRequested=true;
    if (g_hybridLauncherThread) {
        WaitForSingleObject(g_hybridLauncherThread, INFINITE);
        CloseHandle(g_hybridLauncherThread);
        g_hybridLauncherThread = nullptr;
    }
    if (g_hybridStopEvent) SetEvent(g_hybridStopEvent);
    RemoveShellMessageHooks();
    AcquireSRWLockExclusive(&g_hybridChildLock);
    g_hybridChildPid=0;
    HANDLE child=g_hybridChildProcess.exchange(nullptr);
    ReleaseSRWLockExclusive(&g_hybridChildLock);
    if (child) {
        if (WaitForSingleObject(child,20000)==WAIT_TIMEOUT)
            Wh_Log(L"[HybridBridge] tool still stopping; no forced termination");
        CloseHandle(child);
    }
    if (g_hybridStopEvent) { CloseHandle(g_hybridStopEvent); g_hybridStopEvent=nullptr; }
}
void Wh_ModUninit() {
    if (g_isHybridTool) {
        if (g_hybridStopEvent) CloseHandle(g_hybridStopEvent);
        if (g_hybridParentProcess) CloseHandle(g_hybridParentProcess);
        if (g_hybridMutex) CloseHandle(g_hybridMutex);
        ExitProcess(0);
    }
}
