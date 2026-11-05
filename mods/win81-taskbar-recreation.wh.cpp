// ==WindhawkMod==
// @id              win81-taskbar-recreation
// @name            Windows 7/8/8.1 Taskbar Recreation
// @name:it         Ricreazione della barra delle applicazioni di Windows 7/8/8.1
// @name:es         Recreación de la barra de tareas de Windows 7/8/8.1
// @name:fr         Recréation de la barre des tâches de Windows 7/8/8.1
// @name:pt         Recriação da barra de tarefas do Windows 7/8/8.1
// @description     This mod recreates Windows 7/8/8.1-inspired taskbars on Windows 10 and Windows 11
// @description:it  Questa mod ricrea barre delle applicazioni ispirate a Windows 7/8/8.1 su Windows 10 e Windows 11
// @description:es  Este mod recrea barras de tareas inspiradas en Windows 7/8/8.1 en Windows 10 y Windows 11
// @description:fr  Ce mod recrée des barres des tâches inspirées de Windows 7/8/8.1 sur Windows 10 et Windows 11
// @description:pt  Este mod recria barras de tarefas inspiradas no Windows 7/8/8.1 no Windows 10 e no Windows 11
// @version         1.0.0
// @license         GPL-3.0-or-later
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lgdiplus -lgdi32 -luser32 -ldwmapi -lshell32 -lole32 -luuid -lpropsys -loleaut32 -lshlwapi -lcomctl32 -lpsapi -ladvapi32 -liphlpapi -lws2_32 -lwinhttp -lbcrypt -lwlanapi -lsetupapi -lruntimeobject
// ==/WindhawkMod==
//
// ==WindhawkModReadme==
/*
# Windows 7/8/8.1 Taskbar Recreation

This mod recreates taskbars inspired by Windows 7, Windows 8 and Windows 8.1
on Windows 10 and 11. This mod has been tested on Windows 10 22H2, Windows 11 24H2 and Windows 11 25H2.
The mod is provided on a best-effort basis; appearance and behavior can vary with
the Windows build, DPI, taskbar replacement and other shell mods.

## Features

- Four skin presets: Windows 7, Windows 8 Beta 8148, Windows 8.0 and Windows 8.1, plus Custom for individual Start options.
- Task buttons, window previews, Jump Lists, notification icons and toolbars.
- An integrated notification area and taskbar, with a fallback to the native bar if the integrated tray cannot start.
- Horizontal and vertical layouts, Aero Peek and classic flyout options.
- Change notification area icon visibility using the Control Panel page that the mod attempts to restore, including on Windows 11; availability and behavior can vary.
- Static taskbar textures and system colors; the textures do not blur live windows behind the bar.
- Local taskbar pins and ordering: the mod does not change Windows pins. Drag to an empty task area to pin; hover a running app to activate its window.
- An advanced original battery flyout option; compatibility may vary between Windows builds.

## Themes

### Windows 7

![Windows 7 taskbar](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win7taskskin.png)

### Windows 8 Beta 8148

![Windows 8 Beta taskbar](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8betatask.png)

### Windows 8.0

![Windows 8.0 taskbar](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win8.0task.png)

### Windows 8.1

![Windows 8.1 taskbar](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/win81task.png)

### Custom

Choose individual Start options instead of a preset.

## Installation

1. Install and enable the mod in Windhawk.
2. Choose a skin preset in the mod settings.
3. Restart the mod after changing settings that require it.

## Settings

The mod settings control the skin, Start button, taskbar layout, notification
area, previews, flyouts and other behavior. Individual descriptions in
Windhawk explain their scope and any restart requirements.

## Behavior and limitations

- Skins are approximations; Windows 7 popup frames use the current system's DWM frame when available.
- The Windows 8 Start corner is illustrated, not a live Start-screen preview.
- Vertical layout, Aero Peek and classic flyouts may vary between Windows builds.
- Some resources are downloaded on first use. The advanced battery flyout loads a Windows 10 1511 DLL into Explorer; the DLL stays loaded until Explorer restarts.
- Other shell mods can conflict. Keep a way to disable the mod or restart Explorer.
- Windows 11 Taskbar Styler and its skins are technical references; their authors are not involved in this mod.

## Credits

- WinBoeing777 - Testing on Windows 10 22H2 and Suggestions
- Cips - Testing on Windows 11 25H2
- Kiyaku - Suggestions and Testing

## Disclaimer

This is an independent, unofficial project. It is not affiliated with, endorsed, sponsored or approved by Microsoft Corporation. Windows is a trademark of Microsoft Corporation, used here only to describe the style and compatibility of this mod.

The mod is provided "as is", without warranty of any kind, under the GPL-3.0-or-later license. To the maximum extent permitted by law, the author is not responsible for data loss, system instability, blocked updates, or any other damage or consequence arising from the use or misuse of this mod.

Use at your own risk. You are responsible for how you use the mod and for complying with the licenses and terms of any software or resources it interacts with, including Microsoft's terms for any resources downloaded on first use. Keep a way to disable the mod or restart Explorer.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- taskbarSkin: "custom"
  $name: "Taskbar skin preset"
  $name:it: "Skin predefinita della barra"
  $name:es: "Estilo predefinido de la barra"
  $name:fr: "Thème prédéfini de la barre"
  $name:pt: "Tema predefinido da barra"
  $description: "This setting selects a taskbar skin. Custom keeps the separate Start and beta settings; the other presets override those two settings without deleting their values. The Windows 7 skin is an approximation and uses the system DWM popup frame when available."
  $description:it: "Questa impostazione seleziona la skin della barra. Personalizzata usa le opzioni separate per Start e beta; gli altri preset le sostituiscono senza cancellarne i valori. La skin Windows 7 è approssimativa e usa la cornice DWM di sistema per i popup quando disponibile."
  $description:es: "Esta configuración selecciona el estilo de la barra. Personalizado usa las opciones separadas de Inicio y beta; los otros estilos las anulan sin borrar sus valores. El estilo Windows 7 es aproximado y usa el marco DWM del sistema para ventanas emergentes cuando está disponible."
  $description:fr: "Ce paramètre sélectionne le thème de la barre. Personnalisé utilise les options distinctes Démarrer et bêta ; les autres thèmes les remplacent sans effacer leurs valeurs. Le thème Windows 7 est approximatif et utilise le cadre DWM système pour les fenêtres contextuelles lorsqu'il est disponible."
  $description:pt: "Esta configuração seleciona o tema da barra. Personalizado usa as opções separadas de Iniciar e beta; os outros temas as substituem sem apagar seus valores. O tema Windows 7 é aproximado e usa a moldura DWM do sistema nos pop-ups quando disponível."
  $options:
    - custom: "Custom (individual options)"
    - win7: "Windows 7"
    - win8beta: "Windows 8 Beta 8148"
    - win8: "Windows 8.0"
    - win81: "Windows 8.1"
  $options:it:
    - custom: "Personalizzata (opzioni singole)"
    - win7: "Windows 7"
    - win8beta: "Windows 8 Beta 8148"
    - win8: "Windows 8.0"
    - win81: "Windows 8.1"
  $options:es:
    - custom: "Personalizado (opciones individuales)"
    - win7: "Windows 7"
    - win8beta: "Windows 8 Beta 8148"
    - win8: "Windows 8.0"
    - win81: "Windows 8.1"
  $options:fr:
    - custom: "Personnalisé (options individuelles)"
    - win7: "Windows 7"
    - win8beta: "Windows 8 Bêta 8148"
    - win8: "Windows 8.0"
    - win81: "Windows 8.1"
  $options:pt:
    - custom: "Personalizado (opções individuais)"
    - win7: "Windows 7"
    - win8beta: "Windows 8 Beta 8148"
    - win8: "Windows 8.0"
    - win81: "Windows 8.1"
- showJumpShowWindow: true
  $name: "Show the Show window action in Jump Lists"
  $name:it: "Mostra l’azione Mostra finestra nelle Jump List"
  $name:es: "Mostrar la acción Mostrar ventana en las Jump Lists"
  $name:fr: "Afficher l’action Afficher la fenêtre dans les Jump Lists"
  $name:pt: "Mostrar a ação Mostrar janela nas Jump Lists"
  $description: "This setting controls whether the Show window action appears in Jump Lists."
  $description:it: "Questa impostazione controlla se l’azione Mostra finestra appare nelle Jump List."
  $description:es: "Esta configuración controla si la acción Mostrar ventana aparece en las Jump Lists."
  $description:fr: "Ce paramètre contrôle l’affichage de l’action Afficher la fenêtre dans les Jump Lists."
  $description:pt: "Esta configuração controla se a ação Mostrar janela aparece nas Jump Lists."
- enablePrivateTray: true
  $name: "Use the integrated taskbar and hide the original taskbar"
  $name:it: "Usa la barra integrata e nascondi la barra originale"
  $name:es: "Usar la barra de tareas integrada y ocultar la original"
  $name:fr: "Utiliser la barre des tâches intégrée et masquer la barre d’origine"
  $name:pt: "Usar a barra de tarefas integrada e ocultar a original"
  $description: "This setting has the following behavior: Uses the mod’s own notification area and hides the original taskbar. Keeps the original bar if the integrated tray cannot start. Restart the mod after changing this option."
  $description:it: "Questa impostazione ha il seguente comportamento: Usa l’area di notifica della mod e nasconde la barra originale. Mantiene la barra originale se l’area integrata non si avvia. Riavvia la mod dopo aver modificato questa opzione."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Usa el área de notificación del mod y oculta la barra original. Mantiene la barra original si el área integrada no puede iniciarse. Reinicia el mod después de cambiar esta opción."
  $description:fr: "Ce paramètre a le comportement suivant : Utilise la zone de notification du mod et masque la barre d’origine. Conserve la barre d’origine si la zone intégrée ne peut pas démarrer. Redémarrez le mod après modification."
  $description:pt: "Esta configuração tem o seguinte comportamento: Usa a área de notificação do mod e oculta a barra original. Mantém a barra original se a área integrada não puder iniciar. Reinicie o mod depois de alterar esta opção."
- reserveWorkArea: true
  $name: "Reserve desktop space for a moved taskbar"
  $name:it: "Riserva spazio sul desktop per la barra spostata"
  $name:es: "Reservar espacio del escritorio para la barra desplazada"
  $name:fr: "Réserver de l’espace sur le bureau pour la barre déplacée"
  $name:pt: "Reservar espaço na área de trabalho para a barra movida"
  $description: "This setting has the following behavior: Enabled by default. Requests a Windows AppBar when the integrated taskbar is on a different edge from the native taskbar. Windows or other mods may prevent reservation or retain space at the native edge; windows can then overlap the bar. The reservation is removed on normal unload; restarting the shell may be necessary after an unexpected interruption."
  $description:it: "Questa impostazione ha il seguente comportamento: Attiva per impostazione predefinita. Richiede una AppBar di Windows quando la barra integrata si trova su un bordo diverso da quella nativa. Windows o altre mod possono impedire la riserva o mantenere spazio sul bordo nativo; le finestre possono quindi sovrapporsi alla barra. La riserva viene rimossa alla chiusura normale; dopo un’interruzione imprevista può essere necessario riavviare la shell."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Activada de forma predeterminada. Solicita una AppBar de Windows cuando la barra integrada está en un borde distinto de la barra nativa. Windows u otros mods pueden impedir la reserva o conservar espacio en el borde nativo; las ventanas pueden superponerse a la barra. La reserva se elimina al descargarse normalmente; después de una interrupción inesperada puede ser necesario reiniciar el shell."
  $description:fr: "Ce paramètre a le comportement suivant : Activé par défaut. Demande une AppBar Windows lorsque la barre intégrée se trouve sur un bord différent de la barre native. Windows ou d’autres mods peuvent empêcher la réservation ou conserver de l’espace au bord natif ; les fenêtres peuvent alors recouvrir la barre. La réservation est supprimée lors d’un déchargement normal ; après une interruption inattendue, un redémarrage du shell peut être nécessaire."
  $description:pt: "Esta configuração tem o seguinte comportamento: Ativada por padrão. Solicita uma AppBar do Windows quando a barra integrada está em uma borda diferente da barra nativa. O Windows ou outros mods podem impedir a reserva ou manter espaço na borda nativa; as janelas podem então sobrepor a barra. A reserva é removida no descarregamento normal; após uma interrupção inesperada, pode ser necessário reiniciar o shell."
- taskbarPlacement: "allEdges"
  $name: "Taskbar position"
  $name:it: "Posizione della barra delle applicazioni"
  $name:es: "Posición de la barra de tareas"
  $name:fr: "Position de la barre des tâches"
  $name:pt: "Posição da barra de tarefas"
  $description: "This setting has the following behavior: Applies to the integrated taskbar. All edges starts at the native horizontal position and allows dragging the unlocked bar by an empty area to any edge. Left and Right select a vertical edge directly. Work-area reservation is controlled separately; Windows and other taskbar mods can affect placement."
  $description:it: "Questa impostazione ha il seguente comportamento: Si applica alla barra integrata. Tutti i bordi segue inizialmente la posizione orizzontale di Windows e consente di trascinare la barra sbloccata da uno spazio vuoto verso qualsiasi bordo. Sinistra e Destra scelgono direttamente un bordo verticale. La riserva dell’area di lavoro si gestisce separatamente; Windows e altre mod possono influire sul posizionamento."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Se aplica a la barra integrada. Todos los bordes sigue inicialmente la posición horizontal de Windows y permite arrastrar la barra desbloquejada desde un área vacía a cualquier borde. Izquierda y Derecha seleccionan directamente un borde vertical. La reserva del área de trabajo se controla por separado; Windows y otros mods pueden afectar a la posición."
  $description:fr: "Ce paramètre a le comportement suivant : S’applique à la barre intégrée. Tous les bords suit d’abord la position horizontale de Windows et permet de faire glisser la barre déverrouillée depuis un espace vide vers n’importe quel bord. Gauche et Droite choisissent directement un bord vertical. La réservation de la zone de travail se règle séparément ; Windows et d’autres mods peuvent affecter le positionnement."
  $description:pt: "Esta configuração tem o seguinte comportamento: Aplica-se à barra integrada. Todas as bordas segue inicialmente a posição horizontal do Windows e permite arrastar a barra desbloqueada por uma área vazia para qualquer borda. Esquerda e Direita selecionam diretamente uma borda vertical. A reserva da área de trabalho é controlada separadamente; o Windows e outros mods podem afetar o posicionamento."
  $options:
    - native: "Follow Windows (horizontal)"
    - left: "Left"
    - right: "Right"
    - allEdges: "All edges (drag to move)"
  $options:it:
    - native: "Segui Windows (orizzontale)"
    - left: "Sinistra"
    - right: "Destra"
    - allEdges: "Tutti i bordi (trascina per spostare)"
  $options:es:
    - native: "Seguir Windows (horizontal)"
    - left: "Izquierda"
    - right: "Derecha"
    - allEdges: "Todos los bordes (arrastrar para mover)"
  $options:fr:
    - native: "Suivre Windows (horizontal)"
    - left: "Gauche"
    - right: "Droite"
    - allEdges: "Tous les bords (glisser pour déplacer)"
  $options:pt:
    - native: "Seguir o Windows (horizontal)"
    - left: "Esquerda"
    - right: "Direita"
    - allEdges: "Todas as bordas (arrastar para mover)"
- reserveVerticalWorkArea: true
  $name: "Reserve desktop space beside a vertical taskbar"
  $name:it: "Riserva spazio sul desktop accanto alla barra verticale"
  $name:es: "Reservar espacio del escritorio junto a la barra vertical"
  $name:fr: "Réserver de l’espace sur le bureau à côté de la barre verticale"
  $name:pt: "Reservar espaço na área de trabalho ao lado da barra vertical"
  $description: "This setting has the following behavior: Enabled by default. Requests a Windows AppBar for a vertical taskbar and removes it if the work area cannot be observed. If the request fails, maximized windows may extend beneath the bar and native top or bottom space may remain. The mod does not force the work area through build-specific Explorer hooks."
  $description:it: "Questa impostazione ha il seguente comportamento: Attiva per impostazione predefinita. Richiede una AppBar di Windows per la barra verticale e la rimuove se non è possibile rilevare l’area di lavoro. Se la richiesta non riesce, le finestre ingrandite possono estendersi sotto la barra e può restare spazio nativo in alto o in basso. La mod non forza l’area di lavoro tramite hook interni di Explorer dipendenti dalla build."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Activada de forma predeterminada. Solicita una AppBar de Windows para la barra vertical y la elimina si no puede observar el área de trabajo. Si la solicitud falla, las ventanas maximizadas pueden extenderse debajo de la barra y puede quedar espacio nativo arriba o abajo. El mod no fuerza el área de trabajo mediante hooks internos de Explorer dependientes de la compilación."
  $description:fr: "Ce paramètre a le comportement suivant : Activé par défaut. Demande une AppBar Windows pour la barre verticale et la supprime si la zone de travail ne peut pas être observée. Si la demande échoue, les fenêtres agrandies peuvent s’étendre sous la barre et un espace natif peut subsister en haut ou en bas. Le mod ne force pas la zone de travail via des hooks internes d’Explorer propres à la build."
  $description:pt: "Esta configuração tem o seguinte comportamento: Ativada por padrão. Solicita uma AppBar do Windows para a barra vertical e a remove se a área de trabalho não puder ser observada. Se a solicitação falhar, as janelas maximizadas podem se estender sob a barra e o espaço nativo superior ou inferior pode permanecer. O mod não força a área de trabalho por meio de hooks internos do Explorer dependentes da build."
- taskbarOpacity: 220
  $name: "Global opacity in classic mode (140-255)"
  $name:it: "Opacità globale in modalità classica (140-255)"
  $name:es: "Opacidad global en modo clásico (140-255)"
  $name:fr: "Opacité globale en mode classique (140-255)"
  $name:pt: "Opacidade global no modo clássico (140-255)"
  $description: "This setting is used only when per-pixel taskbar transparency is disabled."
  $description:it: "Questa impostazione si usa solo quando la trasparenza per pixel della barra è disattivata."
  $description:es: "Esta configuración se usa solo cuando la transparencia por píxel de la barra de tareas está desactivada."
  $description:fr: "Ce paramètre est utilisé uniquement lorsque la transparence pixel par pixel de la barre des tâches est désactivée."
  $description:pt: "Esta configuração é usada somente quando a transparência por pixel da barra de tarefas está desativada."
- taskbarBackgroundOpacity: 155
  $name: "Per-pixel taskbar background opacity (100-255)"
  $name:it: "Opacità dello sfondo della barra per pixel (100-255)"
  $name:es: "Opacidad del fondo de la barra de tareas por píxel (100-255)"
  $name:fr: "Opacité de l'arrière-plan de la barre des tâches pixel par pixel (100-255)"
  $name:pt: "Opacidade do fundo da barra de tarefas por pixel (100-255)"
  $description: "This setting has an initial value estimated from a reference image, not the original Windows 8.1 algorithm. It is used only in per-pixel mode."
  $description:it: "Questa impostazione ha un valore iniziale stimato da un'immagine di riferimento, non dall'algoritmo originale di Windows 8.1. Si usa solo nella modalità per pixel."
  $description:es: "Esta configuración tiene un valor inicial estimado a partir de una imagen de referencia, no del algoritmo original de Windows 8.1. Se usa solo en el modo por píxel."
  $description:fr: "Ce paramètre a une valeur initiale estimée à partir d'une image de référence, et non de l'algorithme original de Windows 8.1. Il est utilisé uniquement dans le mode pixel par pixel."
  $description:pt: "Esta configuração tem um valor inicial estimado a partir de uma imagem de referência, não do algoritmo original do Windows 8.1. É usada somente no modo por pixel."
- perPixelTaskbar: true
  $name: "Per-pixel taskbar transparency"
  $name:it: "Trasparenza per pixel della barra"
  $name:es: "Transparencia por píxel de la barra de tareas"
  $name:fr: "Transparence pixel par pixel de la barre des tâches"
  $name:pt: "Transparência por pixel da barra de tarefas"
  $description: "This setting has the following behavior: Enabled by default. Separates background and content using UpdateLayeredWindow, rather than the original Windows 8.1 compositor. Compatibility can vary with the Windows build and other mods. Disable this option if the bar is invisible."
  $description:it: "Questa impostazione ha il seguente comportamento: Attiva per impostazione predefinita. Separa sfondo e contenuto usando UpdateLayeredWindow, anziché il compositor originale di Windows 8.1. La compatibilità può variare con la build di Windows e le altre mod. Disattiva questa opzione se la barra è invisibile."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Activada de forma predeterminada. Separa el fondo y el contenido mediante UpdateLayeredWindow, en lugar del compositor original de Windows 8.1. La compatibilidad puede variar con la compilación de Windows y otros mods. Desactiva esta opción si la barra es invisible."
  $description:fr: "Ce paramètre a le comportement suivant : Activé par défaut. Sépare l’arrière-plan et le contenu avec UpdateLayeredWindow, plutôt qu’avec le compositeur d’origine de Windows 8.1. La compatibilité peut varier selon la version de Windows et les autres mods. Désactivez cette option si la barre est invisible."
  $description:pt: "Esta configuração tem o seguinte comportamento: Ativada por padrão. Separa o fundo e o conteúdo usando UpdateLayeredWindow, em vez do compositor original do Windows 8.1. A compatibilidade pode variar com a build do Windows e outros mods. Desative esta opção se a barra estiver invisível."
- perPixelPreview: true
  $name: "Per-pixel preview transparency"
  $name:it: "Trasparenza per pixel delle anteprime"
  $name:es: "Transparencia por píxel de las vistas previas"
  $name:fr: "Transparence pixel par pixel des aperçus"
  $name:pt: "Transparência por pixel das visualizações"
  $description: "This setting has the following behavior: Enabled by default. Requires DWM; live thumbnails may fail on some Windows builds or with other mods. Disable this option if previews are invisible."
  $description:it: "Questa impostazione ha il seguente comportamento: Attiva per impostazione predefinita. Richiede DWM; le miniature live possono non funzionare su alcune build di Windows o con altre mod. Disattiva questa opzione se le anteprime sono invisibili."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Activada de forma predeterminada. Requiere DWM; las miniaturas en directo pueden fallar en algunas compilaciones de Windows o con otros mods. Desactiva esta opción si las vistas previas son invisibles."
  $description:fr: "Ce paramètre a le comportement suivant : Activé par défaut. Nécessite DWM ; les miniatures dynamiques peuvent échouer sur certaines versions de Windows ou avec d’autres mods. Désactivez cette option si les aperçus sont invisibles."
  $description:pt: "Esta configuração tem o seguinte comportamento: Ativada por padrão. Requer DWM; as miniaturas ao vivo podem falhar em algumas builds do Windows ou com outros mods. Desative esta opção se as visualizações estiverem invisíveis."
- previewOpacity: 200
  $name: "Preview background opacity (140-255)"
  $name:it: "Opacità dello sfondo delle anteprime (140-255)"
  $name:es: "Opacidad del fondo de las vistas previas (140-255)"
  $name:fr: "Opacité de l'arrière-plan des aperçus (140-255)"
  $name:pt: "Opacidade do fundo das visualizações (140-255)"
  $description: "This setting is used only for per-pixel previews; thumbnails and text remain independent."
  $description:it: "Questa impostazione si usa solo per le anteprime per pixel; miniature e testo restano indipendenti."
  $description:es: "Esta configuración se usa solo para las vistas previas por píxel; las miniaturas y el texto permanecen independientes."
  $description:fr: "Ce paramètre est utilisé uniquement pour les aperçus pixel par pixel ; les miniatures et le texte restent indépendants."
  $description:pt: "Esta configuração é usada somente para visualizações por pixel; miniaturas e texto permanecem independentes."
- startHotCorner: false
  $name: "Windows 8-style Start hot corner"
  $name:it: "Angolo Start in stile Windows 8"
  $name:es: "Esquina Inicio al estilo Windows 8"
  $name:fr: "Coin Démarrer de style Windows 8"
  $name:pt: "Canto Iniciar no estilo Windows 8"
  $description: "This setting has the following behavior: Hides the Start button and shows an illustrated, clickable Start preview on hover at the start of the reconstructed bar. It is not a live screenshot or a system-wide hot corner."
  $description:it: "Questa impostazione ha il seguente comportamento: Nasconde il pulsante Start e mostra al passaggio del mouse un'anteprima illustrata e cliccabile all'inizio della barra ricreata. Non è una cattura live né un angolo attivo in tutto il sistema."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Oculta el botón Inicio y muestra una vista ilustrada y pulsable al pasar el ratón al inicio de la barra recreada. No es una captura en vivo ni una esquina activa en todo el sistema."
  $description:fr: "Ce paramètre a le comportement suivant : Masque le bouton Démarrer et affiche un aperçu illustré cliquable au survol du début de la barre recréée. Ce n'est ni une capture en direct ni un coin actif dans tout le système."
  $description:pt: "Esta configuração tem o seguinte comportamento: Oculta o botão Iniciar e mostra uma prévia ilustrada clicável ao passar o mouse no início da barra recriada. Não é uma captura ao vivo nem um canto ativo em todo o sistema."
- customStartColor: false
  $name: "Custom Windows 8.1 Start hover color"
  $name:it: "Colore personalizzato Start Windows 8.1 al passaggio del mouse"
  $name:es: "Color personalizado de Inicio Windows 8.1 al pasar el ratón"
  $name:fr: "Couleur personnalisée de Démarrer Windows 8.1 au survol"
  $name:pt: "Cor personalizada de Iniciar Windows 8.1 ao passar o mouse"
  $description: "This setting uses the RGB values below for the Windows 8.1 preset Start logo on hover and press only. The resting logo remains white; other presets ignore this setting."
  $description:it: "Questa impostazione usa i valori RGB seguenti per il logo Start del preset Windows 8.1 solo al passaggio del mouse e alla pressione. A riposo il logo resta bianco; gli altri preset ignorano questa impostazione."
  $description:es: "Esta configuración usa los valores RGB siguientes para el logotipo Inicio del estilo Windows 8.1 solo al pasar el ratón y al pulsar. En reposo el logotipo permanece blanco; los demás estilos ignoran esta configuración."
  $description:fr: "Ce paramètre utilise les valeurs RVB ci-dessous pour le logo Démarrer du thème Windows 8.1 uniquement au survol et à l’appui. Au repos le logo reste blanc ; les autres thèmes ignorent ce paramètre."
  $description:pt: "Esta configuração usa os valores RGB abaixo para o logotipo Iniciar do tema Windows 8.1 somente ao passar o mouse e pressionar. Em repouso o logotipo permanece branco; os outros temas ignoram esta configuração."
- startLogoRed: 0
  $name: "Start logo red (0-255)"
  $name:it: "Rosso logo Start (0-255)"
  $name:es: "Rojo del logotipo Inicio (0-255)"
  $name:fr: "Rouge du logo Démarrer (0-255)"
  $name:pt: "Vermelho do logotipo Iniciar (0-255)"
  $description: "This setting selects the red RGB component of the custom Start logo color, from 0 to 255. It is used when the custom color is enabled."
  $description:it: "Questa impostazione seleziona la componente rossa RGB del colore personalizzato del logo Start, da 0 a 255. Viene usata quando il colore personalizzato è attivo."
  $description:es: "Esta configuración selecciona el componente rojo RGB del color personalizado del logotipo Inicio, de 0 a 255. Se usa cuando el color personalizado está activado."
  $description:fr: "Ce paramètre définit la composante rouge RVB de la couleur personnalisée du logo Démarrer, de 0 à 255. Il est utilisé lorsque la couleur personnalisée est activée."
  $description:pt: "Esta configuração seleciona o componente vermelho RGB da cor personalizada do logotipo Iniciar, de 0 a 255. Ela é usada quando a cor personalizada está ativada."
- startLogoGreen: 120
  $name: "Start logo green (0-255)"
  $name:it: "Verde logo Start (0-255)"
  $name:es: "Verde del logotipo Inicio (0-255)"
  $name:fr: "Vert du logo Démarrer (0-255)"
  $name:pt: "Verde do logotipo Iniciar (0-255)"
  $description: "This setting selects the green RGB component of the custom Start logo color, from 0 to 255. It is used when the custom color is enabled."
  $description:it: "Questa impostazione seleziona la componente verde RGB del colore personalizzato del logo Start, da 0 a 255. Viene usata quando il colore personalizzato è attivo."
  $description:es: "Esta configuración selecciona el componente verde RGB del color personalizado del logotipo Inicio, de 0 a 255. Se usa cuando el color personalizado está activado."
  $description:fr: "Ce paramètre définit la composante verte RVB de la couleur personnalisée du logo Démarrer, de 0 à 255. Il est utilisé lorsque la couleur personnalisée est activée."
  $description:pt: "Esta configuração seleciona o componente verde RGB da cor personalizada do logotipo Iniciar, de 0 a 255. Ela é usada quando a cor personalizada está ativada."
- startLogoBlue: 215
  $name: "Start logo blue (0-255)"
  $name:it: "Blu logo Start (0-255)"
  $name:es: "Azul del logotipo Inicio (0-255)"
  $name:fr: "Bleu du logo Démarrer (0-255)"
  $name:pt: "Azul do logotipo Iniciar (0-255)"
  $description: "This setting selects the blue RGB component of the custom Start logo color, from 0 to 255. It is used when the custom color is enabled."
  $description:it: "Questa impostazione seleziona la componente blu RGB del colore personalizzato del logo Start, da 0 a 255. Viene usata quando il colore personalizzato è attivo."
  $description:es: "Esta configuración selecciona el componente azul RGB del color personalizado del logotipo Inicio, de 0 a 255. Se usa cuando el color personalizado está activado."
  $description:fr: "Ce paramètre définit la composante bleue RVB de la couleur personnalisée du logo Démarrer, de 0 à 255. Il est utilisé lorsque la couleur personnalisée est activée."
  $description:pt: "Esta configuração seleciona o componente azul RGB da cor personalizada do logotipo Iniciar, de 0 a 255. Ela é usada quando a cor personalizada está ativada."
- customTaskbarColor: false
  $name: "Use a custom taskbar color"
  $name:it: "Usa un colore personalizzato per la barra"
  $name:es: "Usar un color personalizado para la barra"
  $name:fr: "Utiliser une couleur personnalisée pour la barre"
  $name:pt: "Usar uma cor personalizada para a barra"
  $description: "This setting uses the RGB values below for the reconstructed taskbar instead of the system accent or sampled native taskbar. It does not change the Windows accent or the Start hover color."
  $description:it: "Questa impostazione usa i valori RGB seguenti per la barra ricreata invece del colore di sistema o della barra nativa campionata. Non cambia il colore di Windows né il colore Start al passaggio del mouse."
  $description:es: "Esta configuración usa los valores RGB siguientes para la barra recreada en lugar del color del sistema o de la barra nativa muestreada. No cambia el color de Windows ni el color de Inicio al pasar el ratón."
  $description:fr: "Ce paramètre utilise les valeurs RVB ci-dessous pour la barre recréée au lieu de la couleur système ou de la barre native échantillonnée. Il ne modifie ni la couleur Windows ni celle du bouton Démarrer au survol."
  $description:pt: "Esta configuração usa os valores RGB abaixo para a barra recriada em vez da cor do sistema ou da barra nativa amostrada. Não altera a cor do Windows nem a cor de Iniciar ao passar o mouse."
- taskbarColorRed: 0
  $name: "Taskbar red (0-255)"
  $name:it: "Rosso barra (0-255)"
  $name:es: "Rojo de la barra (0-255)"
  $name:fr: "Rouge de la barre (0-255)"
  $name:pt: "Vermelho da barra (0-255)"
  $description: "This setting selects the red component of the custom taskbar color from 0 to 255. It applies when the custom taskbar color is enabled."
  $description:it: "Questa impostazione seleziona la componente rossa del colore personalizzato della barra da 0 a 255. Si applica quando il colore personalizzato della barra è attivo."
  $description:es: "Esta configuración selecciona el componente rojo del color personalizado de la barra de 0 a 255. Se aplica cuando el color personalizado de la barra está activado."
  $description:fr: "Ce paramètre définit la composante rouge de la couleur personnalisée de la barre de 0 à 255. Elle s’applique lorsque la couleur personnalisée de la barre est activée."
  $description:pt: "Esta configuração seleciona o componente vermelho da cor personalizada da barra de 0 a 255. Ela se aplica quando a cor personalizada da barra está ativada."
- taskbarColorGreen: 120
  $name: "Taskbar green (0-255)"
  $name:it: "Verde barra (0-255)"
  $name:es: "Verde de la barra (0-255)"
  $name:fr: "Vert de la barre (0-255)"
  $name:pt: "Verde da barra (0-255)"
  $description: "This setting selects the green component of the custom taskbar color from 0 to 255. It applies when the custom taskbar color is enabled."
  $description:it: "Questa impostazione seleziona la componente verde del colore personalizzato della barra da 0 a 255. Si applica quando il colore personalizzato della barra è attivo."
  $description:es: "Esta configuración selecciona el componente verde del color personalizado de la barra de 0 a 255. Se aplica cuando el color personalizado de la barra está activado."
  $description:fr: "Ce paramètre définit la composante verte de la couleur personnalisée de la barre de 0 à 255. Elle s’applique lorsque la couleur personnalisée de la barre est activée."
  $description:pt: "Esta configuração seleciona o componente verde da cor personalizada da barra de 0 a 255. Ela se aplica quando a cor personalizada da barra está ativada."
- taskbarColorBlue: 215
  $name: "Taskbar blue (0-255)"
  $name:it: "Blu barra (0-255)"
  $name:es: "Azul de la barra (0-255)"
  $name:fr: "Bleu de la barre (0-255)"
  $name:pt: "Azul da barra (0-255)"
  $description: "This setting selects the blue component of the custom taskbar color from 0 to 255. It applies when the custom taskbar color is enabled."
  $description:it: "Questa impostazione seleziona la componente blu del colore personalizzato della barra da 0 a 255. Si applica quando il colore personalizzato della barra è attivo."
  $description:es: "Esta configuración selecciona el componente azul del color personalizado de la barra de 0 a 255. Se aplica cuando el color personalizado de la barra está activado."
  $description:fr: "Ce paramètre définit la composante bleue de la couleur personnalisée de la barre de 0 à 255. Elle s’applique lorsque la couleur personnalisée de la barre est activée."
  $description:pt: "Esta configuração seleciona o componente azul da cor personalizada da barra de 0 a 255. Ela se aplica quando a cor personalizada da barra está ativada."
- showRecoverableErrors: true
  $name: "Notify recoverable UI errors"
  $name:it: "Notifica errori recuperabili dell'interfaccia"
  $name:es: "Notificar errores recuperables de la interfaz"
  $name:fr: "Signaler les erreurs récupérables de l'interface"
  $name:pt: "Notificar erros recuperáveis da interface"
  $description: "This setting has the following behavior: Shows a tooltip for caught C++ errors in the preview and language flyout. Does not intercept fatal faults or guarantee Explorer will not restart."
  $description:it: "Questa impostazione ha il seguente comportamento: Mostra un tooltip per errori C++ intercettati nell'anteprima e nel flyout lingua. Non intercetta errori fatali e non garantisce che Explorer non si riavvii."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Muestra un aviso para errores C++ capturados en las vistas previas y el panel de idiomas. No intercepta fallos fatales ni garantiza que Explorer no se reinicie."
  $description:fr: "Ce paramètre a le comportement suivant : Affiche une infobulle pour les erreurs C++ capturées dans les aperçus et le volet de langue. N'intercepte pas les erreurs fatales et ne garantit pas qu'Explorer ne redémarre pas."
  $description:pt: "Esta configuração tem o seguinte comportamento: Mostra uma dica para erros C++ capturados nas prévias e no painel de idioma. Não intercepta falhas fatais nem garante que o Explorer não reinicie."
- useBetaStartLogo: false
  $name: "Windows 8 Beta 8148 skin"
  $name:it: "Skin Windows 8 Beta 8148"
  $name:es: "Estilo Windows 8 Beta 8148"
  $name:fr: "Thème Windows 8 Beta 8148"
  $name:pt: "Tema Windows 8 Beta 8148"
  $description: "This setting has the following behavior: Applies a best-effort skin inspired by Windows 8 Beta 8148, including Start, taskbar textures, task buttons, Show desktop and the Jump List close icon. Turn off to restore the standard Windows 8.1 appearance."
  $description:it: "Questa impostazione ha il seguente comportamento: Applica una skin best effort ispirata a Windows 8 Beta 8148, con Start, texture della barra, pulsanti delle app, Mostra desktop e icona di chiusura della Jump List. Disattivala per ripristinare l'aspetto standard di Windows 8.1."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Aplica un estilo aproximado inspirado en Windows 8 Beta 8148, con Inicio, texturas de la barra, botones de aplicaciones, Mostrar escritorio y el icono de cierre de la lista de accesos directos. Desactívalo para recuperar el aspecto estándar de Windows 8.1."
  $description:fr: "Ce paramètre a le comportement suivant : Applique un thème approximatif inspiré de Windows 8 Beta 8148, avec Démarrer, les textures de la barre, les boutons d'applications, Afficher le bureau et l'icône de fermeture des listes de raccourcis. Désactivez-le pour retrouver l'apparence standard de Windows 8.1."
  $description:pt: "Esta configuração tem o seguinte comportamento: Aplica um tema aproximado inspirado no Windows 8 Beta 8148, com Iniciar, texturas da barra, botões de aplicativos, Mostrar área de trabalho e ícone de fechar da lista de atalhos. Desative para restaurar a aparência padrão do Windows 8.1."
- enableAeroPeek: true
  $name: "Aero Peek on previews"
  $name:it: "Aero Peek sulle anteprime"
  $name:es: "Aero Peek en las vistas previas"
  $name:fr: "Aero Peek sur les aperçus"
  $name:pt: "Aero Peek nas visualizações"
  $description: "This setting uses an undocumented DWM export that depends on the Windows build."
  $description:it: "Questa impostazione usa un export DWM non documentato che dipende dalla build di Windows."
  $description:es: "Esta configuración usa una exportación de DWM no documentada que depende de la compilación de Windows."
  $description:fr: "Ce paramètre utilise un export DWM non documenté qui dépend de la version de Windows."
  $description:pt: "Esta configuração usa uma exportação não documentada do DWM que depende da build do Windows."
- matchNativeTray: true
  $name: "Match the real tray color (original-tray mode only)"
  $name:it: "Adatta il colore alla tray reale (solo modalità con tray originale)"
  $name:es: "Igualar el color de la bandeja real (solo en modo de bandeja original)"
  $name:fr: "Adapter à la couleur de la zone réelle (mode zone d'origine uniquement)"
  $name:pt: "Combinar com a cor da bandeja real (somente no modo de bandeja original)"
  $description: "This setting is used only when the private tray is inactive. It samples in memory without saving images."
  $description:it: "Questa impostazione si usa solo quando la tray privata non è attiva. Campiona in memoria senza salvare immagini."
  $description:es: "Esta configuración se usa solo cuando la bandeja privada está inactiva. Toma muestras en memoria sin guardar imágenes."
  $description:fr: "Ce paramètre est utilisé uniquement lorsque la zone privée est inactive. Il échantillonne en mémoire sans enregistrer d'images."
  $description:pt: "Esta configuração é usada somente quando a bandeja privada está inativa. Faz amostragem na memória sem salvar imagens."
- trayFallbackWidth: 240
  $name: "Fallback tray width (pixels at 96 DPI)"
  $name:it: "Larghezza della tray di riserva (pixel a 96 DPI)"
  $name:es: "Ancho de bandeja de reserva (píxeles a 96 PPP)"
  $name:fr: "Largeur de secours de la zone de notification (pixels à 96 PPP)"
  $name:pt: "Largura da bandeja de fallback (pixels a 96 DPI)"
  $description: "This setting is used only if the classic TrayNotifyWnd is not found; Windows 11 might need manual adjustment."
  $description:it: "Questa impostazione si usa solo se non viene trovata la finestra classica TrayNotifyWnd; su Windows 11 potrebbe essere necessaria una regolazione manuale."
  $description:es: "Esta configuración se usa solo si no se encuentra la ventana clásica TrayNotifyWnd; Windows 11 podría requerir un ajuste manual."
  $description:fr: "Ce paramètre est utilisé uniquement si la fenêtre classique TrayNotifyWnd est introuvable ; un réglage manuel peut être nécessaire sous Windows 11."
  $description:pt: "Esta configuração é usada somente se a janela clássica TrayNotifyWnd não for encontrada; o Windows 11 pode exigir ajuste manual."
- preferClassicClock: true
  $name: "Use the classic calendar flyout"
  $name:it: "Usa il flyout del calendario classico"
  $name:es: "Usar el panel de calendario clásico"
  $name:fr: "Utiliser le panneau de calendrier classique"
  $name:pt: "Usar o painel de calendário clássico"
  $description: "This setting uses the undocumented COM interface also used by RetroBar. In private-tray mode, opens date and time settings if unavailable."
  $description:it: "Questa impostazione usa l'interfaccia COM non documentata utilizzata anche da RetroBar. Nella modalità con tray privata, apre le impostazioni di data e ora se non è disponibile."
  $description:es: "Esta configuración usa la interfaz COM no documentada que también utiliza RetroBar. En el modo de bandeja privada, abre la configuración de fecha y hora si no está disponible."
  $description:fr: "Ce paramètre utilise l'interface COM non documentée également employée par RetroBar. En mode zone privée, ouvre les paramètres de date et d'heure si elle est indisponible."
  $description:pt: "Esta configuração usa a interface COM não documentada também usada pelo RetroBar. No modo de bandeja privada, abre as configurações de data e hora se não estiver disponível."
- preferClassicVolume: true
  $name: "Use the classic volume flyout"
  $name:it: "Usa il flyout classico del volume"
  $name:es: "Usar el panel de volumen clásico"
  $name:fr: "Utiliser le panneau de volume classique"
  $name:pt: "Usar o painel de volume clássico"
  $description: "This setting uses SndVol.exe -f with the private tray; otherwise intercepts the recognized native control."
  $description:it: "Questa impostazione usa SndVol.exe -f con la tray privata; altrimenti intercetta il controllo nativo riconosciuto."
  $description:es: "Esta configuración usa SndVol.exe -f con la bandeja privada; en caso contrario, intercepta el control nativo reconocido."
  $description:fr: "Ce paramètre utilise SndVol.exe -f avec la zone privée ; sinon, intercepte le contrôle natif reconnu."
  $description:pt: "Esta configuração usa SndVol.exe -f com a bandeja privada; caso contrário, intercepta o controle nativo reconhecido."
- useOriginalBatteryFlyout: true
  $name: "Use the original Win32 battery flyout (restart mod)"
  $name:it: "Usa il flyout batteria Win32 originale (riavvia la mod)"
  $name:es: "Usar el panel de batería Win32 original (reiniciar el mod)"
  $name:fr: "Utiliser le panneau de batterie Win32 d’origine (redémarrer le mod)"
  $name:pt: "Usar o painel de bateria Win32 original (reiniciar o mod)"
  $description: "This setting has the following behavior: Advanced feature. Executes the verified Windows 10 1511 stobject DLL inside Explorer; compatibility may vary between Windows builds. The DLL stays loaded until Explorer restarts. When off or unavailable, battery opens Windows Settings."
  $description:it: "Questa impostazione ha il seguente comportamento: Funzionalità avanzata. Esegue la DLL stobject verificata di Windows 10 1511 dentro Explorer; la compatibilità può variare tra le versioni di Windows. La DLL resta caricata fino al riavvio di Explorer. Se disattivata o non disponibile, la batteria apre Impostazioni di Windows."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Función avanzada. Ejecuta la DLL stobject verificada de Windows 10 1511 dentro de Explorer; la compatibilidad puede variar entre versiones de Windows. La DLL permanece cargada hasta que Explorer se reinicie. Si está desactivada o no está disponible, la batería abre la Configuración de Windows."
  $description:fr: "Ce paramètre a le comportement suivant : Fonctionnalité avancée. Exécute la DLL stobject vérifiée de Windows 10 1511 dans Explorer ; la compatibilité peut varier selon la version de Windows. La DLL reste chargée jusqu’au redémarrage d’Explorer. Si cette option est désactivée ou indisponible, la batterie ouvre les Paramètres Windows."
  $description:pt: "Esta configuração tem o seguinte comportamento: Recurso avançado. Executa a DLL stobject verificada do Windows 10 1511 dentro do Explorer; a compatibilidade pode variar entre versões do Windows. A DLL permanece carregada até o Explorer reiniciar. Se estiver desativada ou indisponível, a bateria abre as Configurações do Windows."
- networkUse100PercentArtwork: false
  $name: "Use 100% DPI artwork for network, volume and battery"
  $name:it: "Usa le risorse grafiche al 100% DPI per rete, volume e batteria"
  $name:es: "Usar gráficos al 100 % de PPP para red, volumen y batería"
  $name:fr: "Utiliser les graphismes à 100 % PPP pour le réseau, le volume et la batterie"
  $name:pt: "Usar arte em 100% de DPI para rede, volume e bateria"
  $description: "This setting uses the 16px source artwork for all three icons, scaled to their current display size."
  $description:it: "Questa impostazione usa le risorse originali da 16 pixel per tutte e tre le icone, adattandole alla dimensione di visualizzazione corrente."
  $description:es: "Esta configuración usa los gráficos originales de 16 píxeles para los tres iconos, escalados a su tamaño de visualización actual."
  $description:fr: "Ce paramètre utilise les graphismes sources de 16 pixels pour les trois icônes, mis à l'échelle de leur taille d'affichage actuelle."
  $description:pt: "Esta configuração usa a arte original de 16 pixels para os três ícones, dimensionada para o tamanho de exibição atual."
- allowDragReorder: true
  $name: "Allow dragging icons to reorder"
  $name:it: "Consenti di riordinare le icone trascinandole"
  $name:es: "Permitir arrastrar los iconos para reordenarlos"
  $name:fr: "Autoriser le réordonnancement des icônes par glisser-déposer"
  $name:pt: "Permitir arrastar os ícones para reordenar"
  $description: "This setting stores the order in this Windhawk mod; does not modify the original pins or taskbar. The taskbar menu can reset it."
  $description:it: "Questa impostazione salva l'ordine in questa mod di Windhawk; non modifica i pin o la barra originali. Il menu della barra può ripristinarlo."
  $description:es: "Esta configuración guarda el orden en este mod de Windhawk; no modifica los anclajes ni la barra de tareas originales. El menú de la barra puede restablecerlo."
  $description:fr: "Ce paramètre enregistre l'ordre dans ce mod Windhawk ; ne modifie ni les épinglages ni la barre des tâches d'origine. Le menu de la barre permet de le réinitialiser."
  $description:pt: "Esta configuração armazena a ordem neste mod do Windhawk; não modifica os fixados nem a barra de tarefas originais. O menu da barra pode redefini-la."
- keepOverlayAbove: true
  $name: "Keep the bar above the native taskbar"
  $name:it: "Mantieni la barra sopra la barra delle applicazioni nativa"
  $name:es: "Mantener la barra sobre la barra de tareas nativa"
  $name:fr: "Conserver la barre au-dessus de la barre des tâches native"
  $name:pt: "Manter a barra acima da barra de tarefas nativa"
  $description: "This setting applies only with the original tray; it raises the bar if covered and suspends repeated repairs on conflict."
  $description:it: "Questa impostazione vale solo con la tray originale; solleva la barra se viene coperta e sospende i ripristini ripetuti in caso di conflitto."
  $description:es: "Esta configuración solo se aplica con la bandeja original; eleva la barra si queda cubierta y suspende las reparaciones repetidas en caso de conflicto."
  $description:fr: "Ce paramètre s'applique uniquement avec la zone d'origine ; il remonte la barre si elle est recouverte et suspend les corrections répétées en cas de conflit."
  $description:pt: "Esta configuração se aplica somente com a bandeja original; eleva a barra se ela for coberta e suspende reparos repetidos em caso de conflito."
- animateReorder: true
  $name: "Animate icon rearrangement"
  $name:it: "Anima il riordinamento delle icone"
  $name:es: "Animar la reorganización de los iconos"
  $name:fr: "Animer le réordonnancement des icônes"
  $name:pt: "Animar a reorganização dos ícones"
  $description: "This setting animates other buttons while dragging and when dropping in the new position."
  $description:it: "Questa impostazione anima gli altri pulsanti durante il trascinamento e quando rilasci nella nuova posizione."
  $description:es: "Esta configuración anima los demás botones al arrastrar y al soltar en la nueva posición."
  $description:fr: "Ce paramètre anime les autres boutons pendant le glissement et lors du dépôt à la nouvelle position."
  $description:pt: "Esta configuração anima os outros botões ao arrastar e ao soltar na nova posição."
- volumeWheel: false
  $name: "Scroll on volume icon to adjust volume"
  $name:it: "Scorri sull'icona del volume per regolarlo"
  $name:es: "Desplazarse sobre el icono de volumen para ajustarlo"
  $name:fr: "Faire défiler sur l'icône de volume pour l'ajuster"
  $name:pt: "Rolar sobre o ícone de volume para ajustá-lo"
  $description: "This setting is optional. It changes the default multimedia output in device steps; other volume mods may also handle the wheel."
  $description:it: "Questa impostazione è opzionale. Regola il volume dell'uscita multimediale predefinita con incrementi del dispositivo; altre mod del volume possono gestire la stessa rotellina."
  $description:es: "Esta configuración es opcional. Cambia el volumen de la salida multimedia predeterminada en pasos del dispositivo; otros mods de volumen también pueden gestionar la rueda."
  $description:fr: "Ce paramètre est facultatif. Il modifie le volume de la sortie multimédia par défaut par paliers du périphérique ; d'autres mods de volume peuvent également gérer la molette."
  $description:pt: "Esta configuração é opcional. Altera o volume da saída multimídia padrão em etapas do dispositivo; outros mods de volume também podem controlar a roda."
- useAccent: false
  $name: "Use the Windows accent color in related UI"
  $name:it: "Usa il colore di Windows negli altri elementi dell'interfaccia"
  $name:es: "Usar el color de Windows en otros elementos de la interfaz"
  $name:fr: "Utiliser la couleur de Windows dans les autres éléments de l'interface"
  $name:pt: "Usar a cor do Windows em outros elementos da interface"
  $description: "This setting has the following behavior: Controls the accent in related UI elements. Both recreated taskbar skins follow the Windows colorization color independently of this setting."
  $description:it: "Questa impostazione ha il seguente comportamento: Controlla il colore principale negli elementi dell'interfaccia collegati. Entrambe le skin della barra ricreata seguono il colore di Windows indipendentemente da questa impostazione."
  $description:es: "Esta configuración tiene el siguiente comportamiento: Controla el color de énfasis en otros elementos de la interfaz. Ambos estilos de la barra recreada siguen el color de Windows independientemente de esta opción."
  $description:fr: "Ce paramètre a le comportement suivant : Contrôle la couleur d'accentuation dans les autres éléments de l'interface. Les deux thèmes de la barre recréée suivent la couleur de Windows indépendamment de ce paramètre."
  $description:pt: "Esta configuração tem o seguinte comportamento: Controla a cor de destaque nos outros elementos da interface. Ambos os temas da barra recriada seguem a cor do Windows independentemente desta opção."
- showLanguageIndicator: true
  $name: "Show the input language and its flyout"
  $name:it: "Mostra la lingua di input e il relativo flyout"
  $name:es: "Mostrar el idioma de entrada y su panel"
  $name:fr: "Afficher la langue d’entrée et son panneau"
  $name:pt: "Mostrar o idioma de entrada e seu painel"
  $description: "This setting shows the active input language and region on separate lines, such as ITA over IT, next to the clock and opens the Windows 8.1-style language flyout."
  $description:it: "Questa impostazione mostra la lingua e l'area di input attive su due righe, ad esempio ITA sopra IT, accanto all'orologio e apre il flyout della lingua in stile Windows 8.1."
  $description:es: "Esta configuración muestra el idioma y la región de entrada activos en dos líneas, como ITA sobre IT, junto al reloj y abre el panel de idioma al estilo de Windows 8.1."
  $description:fr: "Ce paramètre affiche la langue et la région d’entrée actives sur deux lignes, par exemple ITA au-dessus de IT, à côté de l’horloge et ouvre le panneau de langue de style Windows 8.1."
  $description:pt: "Esta configuração mostra o idioma e a região de entrada ativos em duas linhas, como ITA sobre IT, ao lado do relógio e abre o painel de idioma no estilo do Windows 8.1."
*/
// ==/WindhawkModSettings==
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <roapi.h>
#include <winstring.h>
#include <windows.globalization.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <cstring>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <psapi.h>
#include <gdiplus.h>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <cmath>
#include <new>
#include <cstdint>
#include <iterator>
#include <memory>
#include <shlobj.h>
#include <shobjidl.h>
#include <propsys.h>
#include <propvarutil.h>
#include <shlwapi.h>
#include <shldisp.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <setupapi.h>
#include <wlanapi.h>
#include <cwctype>

/*
 * Task-window discovery, activation and group commands use documented Win32
 * behavior. Public references:
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows
 * https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
 * https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmgetwindowattribute
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindowasync
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setforegroundwindow
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagetimeoutw
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-cascadewindows
 * https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-tilewindows
 * The implementation is organized for this mod; no upstream taskbar
 * implementation is included here.
 */

// Runtime UI uses the Windows display language: English, Italian, Spanish,
// French or Portuguese. Strings supplied by apps, Shell extensions and Windows
// remain under their providers' own localization.
#ifdef TASKBAR81_TEST
static int testLanguageOverride=-1; // 0 English, 1 Italian, 2 Spanish, 3 French, 4 Portuguese
static int testJumpAnimation=-1; // 0 disabled, 1 enabled
static int testPreviewAnimation=-1;
static int testDpiOverride=0;
static bool testThrowEnum=false;
#endif
enum class UiLanguage { English, Italian, Spanish, French, Portuguese };
struct UiTranslation { const wchar_t* english; const wchar_t* spanish; const wchar_t* french; const wchar_t* portuguese; };
static const UiTranslation kUiTranslations[]={
    {L"Restore",L"Restaurar",L"Restaurer",L"Restaurar"},
    {L"Move",L"Mover",L"Déplacer",L"Mover"},
    {L"Size",L"Tamaño",L"Taille",L"Tamanho"},
    {L"Minimize",L"Minimizar",L"Réduire",L"Minimizar"},
    {L"Maximize",L"Maximizar",L"Agrandir",L"Maximizar"},
    {L"Close",L"Cerrar",L"Fermer",L"Fechar"},
    {L"Minimize group",L"Minimizar grupo",L"Réduire le groupe",L"Minimizar grupo"},
    {L"Close all windows",L"Cerrar todas las ventanas",L"Fermer toutes les fenêtres",L"Fechar todas as janelas"},
    {L"Network",L"Red",L"Réseau",L"Rede"},
    {L"Battery",L"Batería",L"Batterie",L"Bateria"},
    {L"Volume",L"Volumen",L"Volume",L"Volume"},
    {L"Volume: ",L"Volumen: ",L"Volume : ",L"Volume: "},
    {L"Show the desktop\n-\n",L"Mostrar el escritorio\n-\n",L"Afficher le bureau\n-\n",L"Mostrar a área de trabalho\n-\n"},
    {L"Peek at desktop",L"Vista previa del escritorio",L"Aperçu du bureau",L"Visualizar a área de trabalho"},
    {L"Windows blocked the command. Use the original window or check permissions.",L"Windows bloqueó el comando. Usa la ventana original o comprueba los permisos.",L"Windows a bloqué la commande. Utilisez la fenêtre d'origine ou vérifiez les autorisations.",L"O Windows bloqueou o comando. Use a janela original ou verifique as permissões."},
    {L"Battery: unknown status",L"Batería: estado desconocido",L"Batterie : état inconnu",L"Bateria: estado desconhecido"},
    {L"Battery: ",L"Batería: ",L"Batterie : ",L"Bateria: "},
    {L" (plugged in)",L" (conectado a la corriente)",L" (branché)",L" (conectado à alimentação)"},
    {L"Network status unavailable",L"Estado de red no disponible",L"État du réseau non disponible",L"Estado da rede indisponível"},
    {L"Internet access",L"Acceso a Internet",L"Accès à Internet",L"Acesso à Internet"},
    {L"No Internet access",L"Sin acceso a Internet",L"Aucun accès à Internet",L"Sem acesso à Internet"},
    {L"Not connected",L"No conectado",L"Non connecté",L"Não conectado"},
    {L"Volume: unavailable",L"Volumen: no disponible",L"Volume : non disponible",L"Volume: indisponível"},
    {L" (muted)",L" (silenciado)",L" (muet)",L" (sem som)"},
    {L"Customize...",L"Personalizar...",L"Personnaliser...",L"Personalizar..."},
    {L"Icon ",L"Icono ",L"Icône ",L"Ícone "},
    {L"No icons available",L"No hay iconos disponibles",L"Aucune icône disponible",L"Nenhum ícone disponível"},
    {L"Open volume mixer\nPlayback devices\nRecording devices\nSounds",L"Abrir mezclador de volumen\nDispositivos de reproducción\nDispositivos de grabación\nSonidos",L"Ouvrir le mélangeur de volume\nPériphériques de lecture\nPériphériques d'enregistrement\nSons",L"Abrir o mixer de volume\nDispositivos de reprodução\nDispositivos de gravação\nSons"},
    {L"Troubleshoot problems\nOpen Network and Sharing Center",L"Solucionar problemas\nAbrir el Centro de redes y recursos compartidos",L"Résoudre les problèmes\nOuvrir le Centre réseau et partage",L"Resolver problemas\nAbrir a Central de Rede e Compartilhamento"},
    {L"Power Options\nWindows Mobility Center",L"Opciones de energía\nCentro de movilidad de Windows",L"Options d'alimentation\nCentre de mobilité Windows",L"Opções de energia\nCentro de Mobilidade do Windows"},
    {L"Open folder",L"Abrir carpeta",L"Ouvrir le dossier",L"Abrir pasta"},
    {L"No items",L"No hay elementos",L"Aucun élément",L"Nenhum item"},
    {L">Toolbars\n",L">Barras de herramientas\n",L">Barres d'outils\n",L">Barras de ferramentas\n"},
    {L"*Address\n",L"*Dirección\n",L"*Adresse\n",L"*Endereço\n"},
    {L"Address\n",L"Dirección\n",L"Adresse\n",L"Endereço\n"},
    {L"*Links\n",L"*Vínculos\n",L"*Liens\n",L"*Links\n"},
    {L"Links\n",L"Vínculos\n",L"Liens\n",L"Links\n"},
    {L"Adjust date/time\nCustomize notification icons\n-\n",L"Cambiar fecha y hora\nPersonalizar iconos de notificación\n-\n",L"Modifier la date et l'heure\nPersonnaliser les icônes de notification\n-\n",L"Ajustar data/hora\nPersonalizar ícones de notificação\n-\n"},
    {L"Cascade windows\nShow windows stacked\nShow windows side by side\nShow the desktop\n-\nTask Manager\n-\n",L"Ventanas en cascada\nMostrar ventanas apiladas\nMostrar ventanas en mosaico\nMostrar el escritorio\n-\nAdministrador de tareas\n-\n",L"Fenêtres en cascade\nAfficher les fenêtres empilées\nAfficher les fenêtres côte à côte\nAfficher le bureau\n-\nGestionnaire des tâches\n-\n",L"Janelas em cascata\nMostrar janelas empilhadas\nMostrar janelas lado a lado\nMostrar a área de trabalho\n-\nGerenciador de Tarefas\n-\n"},
    {L"Lock the taskbar\nProperties",L"Bloquear la barra de tareas\nPropiedades",L"Verrouiller la barre des tâches\nPropriétés",L"Bloquear a barra de tarefas\nPropriedades"},
    {L"Unable to start Task Manager. Windows error: %lu. Check the Windhawk log.",L"No se puede iniciar el Administrador de tareas. Error de Windows: %lu. Consulta el registro de Windhawk.",L"Impossible de démarrer le Gestionnaire des tâches. Erreur Windows : %lu. Consultez le journal Windhawk.",L"Não foi possível iniciar o Gerenciador de Tarefas. Erro do Windows: %lu. Verifique o log do Windhawk."},
    {L"Task Manager",L"Administrador de tareas",L"Gestionnaire des tâches",L"Gerenciador de Tarefas"},
    {L"Unable to open network connections. Check the Windhawk log.",L"No se pueden abrir las conexiones de red. Consulta el registro de Windhawk.",L"Impossible d'ouvrir les connexions réseau. Consultez le journal Windhawk.",L"Não foi possível abrir as conexões de rede. Verifique o log do Windhawk."},
    {L"Unable to open volume. Check the Windhawk log.",L"No se puede abrir el volumen. Consulta el registro de Windhawk.",L"Impossible d'ouvrir le volume. Consultez le journal Windhawk.",L"Não foi possível abrir o volume. Verifique o log do Windhawk."},
    {L"Battery flyout unavailable: opening Settings. Check the Windhawk log.",L"El panel de batería no está disponible: abriendo Configuración. Consulta el registro de Windhawk.",L"Le panneau de batterie n'est pas disponible : ouverture des Paramètres. Consultez le journal Windhawk.",L"O painel da bateria não está disponível: abrindo as Configurações. Verifique o log do Windhawk."},
    {L"Unable to open battery Settings. Check the Windhawk log.",L"No se puede abrir Configuración de batería. Consulta el registro de Windhawk.",L"Impossible d'ouvrir les Paramètres de la batterie. Consultez le journal Windhawk.",L"Não foi possível abrir as Configurações da bateria. Verifique o log do Windhawk."},
    {L"Address",L"Dirección",L"Adresse",L"Endereço"},
    {L"Links",L"Vínculos",L"Liens",L"Links"},
    {L"Window",L"Ventana",L"Fenêtre",L"Janela"},
    {L"Application",L"Aplicación",L"Application",L"Aplicativo"},
    {L"Show desktop",L"Mostrar escritorio",L"Afficher le bureau",L"Mostrar área de trabalho"},
    {L"Start",L"Inicio",L"Démarrer",L"Iniciar"},
    {L"Frequent",L"Frecuentes",L"Fréquents",L"Frequentes"},
    {L"Recent",L"Recientes",L"Récents",L"Recentes"},
    {L"Open ",L"Abrir ",L"Ouvrir ",L"Abrir "},
    {L"Show window",L"Mostrar ventana",L"Afficher la fenêtre",L"Mostrar janela"},
    {L"Close window",L"Cerrar ventana",L"Fermer la fenêtre",L"Fechar janela"},
    {L"Links »",L"Vínculos »",L"Liens »",L"Links »"},
    {L"Open",L"Abrir",L"Ouvrir",L"Abrir"},
    {L"Open shortcuts folder",L"Abrir carpeta de accesos directos",L"Ouvrir le dossier des raccourcis",L"Abrir a pasta de atalhos"},
    {L"Cascade windows\nShow windows stacked\nShow windows side by side\nMinimize all\n-\nClose all windows",L"Ventanas en cascada\nMostrar ventanas apiladas\nMostrar ventanas en mosaico\nMinimizar todas\n-\nCerrar todas las ventanas",L"Fenêtres en cascade\nAfficher les fenêtres empilées\nAfficher les fenêtres côte à côte\nRéduire toutes les fenêtres\n-\nFermer toutes les fenêtres",L"Janelas em cascata\nMostrar janelas empilhadas\nMostrar janelas lado a lado\nMinimizar todas\n-\nFechar todas as janelas"},
    {L"Language preferences",L"Preferencias de idioma",L"Préférences de langue",L"Preferências de idioma"},
    {L"Show the Language bar",L"Mostrar la barra de idioma",L"Afficher la barre de langue",L"Mostrar a barra de idiomas"},
    {L"Pin to taskbar",L"Anclar a la barra de tareas",L"Épingler à la barre des tâches",L"Fixar na barra de tarefas"},
    {L"Unpin from taskbar",L"Desanclar de la barra de tareas",L"Détacher de la barre des tâches",L"Desafixar da barra de tarefas"},
    {L"Unable to update the mod pin. Check the Windhawk log.",L"No se pudo actualizar el elemento anclado del mod. Comprueba el registro de Windhawk.",L"Impossible de modifier l’élément épinglé du mod. Consultez le journal Windhawk.",L"Não foi possível atualizar o item fixado do mod. Verifique o registro do Windhawk."},
    {L"Unable to show the system language bar. Check Windows input settings and the Windhawk log.",L"No se pudo mostrar la barra de idioma del sistema. Comprueba la configuración de entrada de Windows y el registro de Windhawk.",L"Impossible d’afficher la barre de langue du système. Vérifiez les paramètres de saisie Windows et le journal Windhawk.",L"Não foi possível mostrar a barra de idiomas do sistema. Verifique as configurações de entrada do Windows e o registro do Windhawk."},
    {L"Unable to open language preferences. Check the Windhawk log.",L"No se pudieron abrir las preferencias de idioma. Comprueba el registro de Windhawk.",L"Impossible d’ouvrir les préférences de langue. Consultez le journal Windhawk.",L"Não foi possível abrir as preferências de idioma. Verifique o registro do Windhawk."},
    {L"Unable to change the input language. Check the Windhawk log.",L"No se pudo cambiar el idioma de entrada. Comprueba el registro de Windhawk.",L"Impossible de changer la langue d’entrée. Consultez le journal Windhawk.",L"Não foi possível alterar o idioma de entrada. Verifique o registro do Windhawk."},
    {L"Select an input language",L"Selecciona un idioma de entrada",L"Sélectionnez une langue d’entrée",L"Selecione um idioma de entrada"},
};
static UiLanguage CurrentUiLanguage() {
#ifdef TASKBAR81_TEST
    if(testLanguageOverride>=0 && testLanguageOverride<=4) return static_cast<UiLanguage>(testLanguageOverride);
#endif
    switch(PRIMARYLANGID(GetUserDefaultUILanguage())) {
    case LANG_ITALIAN:return UiLanguage::Italian;
    case LANG_SPANISH:return UiLanguage::Spanish;
    case LANG_FRENCH:return UiLanguage::French;
    case LANG_PORTUGUESE:return UiLanguage::Portuguese;
    default:return UiLanguage::English;
    }
}
static bool ItalianUI() { return CurrentUiLanguage()==UiLanguage::Italian; }
static const wchar_t* Tr(const wchar_t* it,const wchar_t* en) {
    UiLanguage language=CurrentUiLanguage();
    if(language==UiLanguage::Italian)return it;
    if(language==UiLanguage::English)return en;
    for(const auto& entry:kUiTranslations) {
        if(wcscmp(entry.english,en))continue;
        if(language==UiLanguage::Spanish)return entry.spanish;
        if(language==UiLanguage::French)return entry.french;
        if(language==UiLanguage::Portuguese)return entry.portuguese;
        break;
    }
    return en;
}


namespace { void ActivateTask(HWND h); static bool RequestTaskClose(HWND h); static bool RequestTaskMinimize(HWND h); }

// Small non-copyable RAII owners; never own shared system image lists.
struct ScopedHandle {HANDLE h;
    explicit ScopedHandle(HANDLE value):h(value){}~ScopedHandle(){if(h && h!=INVALID_HANDLE_VALUE)CloseHandle(h);
    }ScopedHandle(const ScopedHandle&)=delete;
    ScopedHandle& operator=(const ScopedHandle&)=delete;
    };
struct ScopedFind {
    HANDLE h=INVALID_HANDLE_VALUE; explicit ScopedFind(HANDLE v):h(v) {}
    ~ScopedFind(){if(h!=INVALID_HANDLE_VALUE) FindClose(h);}
    ScopedFind(const ScopedFind&)=delete; ScopedFind& operator=(const ScopedFind&)=delete;
};
template<typename T> struct ReleaseCom {void operator()(T* p) const noexcept {if(p) p->Release();}};
struct FreePidl {void operator()(ITEMIDLIST* p) const noexcept {if(p) CoTaskMemFree(p);}};
struct FreeCoTask {void operator()(wchar_t* p) const noexcept {if(p) CoTaskMemFree(p);}};
struct ScopedPropVariant {PROPVARIANT value{};
    ScopedPropVariant(){PropVariantInit(&value);
    }~ScopedPropVariant(){PropVariantClear(&value);
    }ScopedPropVariant(const ScopedPropVariant&)=delete;
    };
struct FreeBstr {void operator()(wchar_t* p) const noexcept {if(p) SysFreeString(p);}};
struct DestroyIconHandle {void operator()(std::remove_pointer<HICON>::type* p) const noexcept {if(p) DestroyIcon(p);}};
struct DeleteBitmapHandle {void operator()(std::remove_pointer<HBITMAP>::type* p) const noexcept {if(p) DeleteObject(p);}};
struct DeleteMemDC {void operator()(std::remove_pointer<HDC>::type* p) const noexcept {if(p) DeleteDC(p);}};
struct DeleteFontHandle {void operator()(std::remove_pointer<HFONT>::type* p) const noexcept {if(p) DeleteObject(p);}};
struct ScopedPaintDC {
    HWND window; PAINTSTRUCT paint{}; HDC dc;
    explicit ScopedPaintDC(HWND h):window(h),dc(BeginPaint(h,&paint)) {}
    void end() noexcept {if(window) {EndPaint(window,&paint);window=nullptr;}}
    ~ScopedPaintDC(){end();}
    ScopedPaintDC(const ScopedPaintDC&)=delete;ScopedPaintDC& operator=(const ScopedPaintDC&)=delete;
};
struct ScopedSavedDC {
    HDC dc; int state; explicit ScopedSavedDC(HDC h):dc(h),state(h?SaveDC(h):0) {}
    void restore() noexcept {if(dc && state) {RestoreDC(dc,state);state=0;}}
    ~ScopedSavedDC(){restore();}
    ScopedSavedDC(const ScopedSavedDC&)=delete;ScopedSavedDC& operator=(const ScopedSavedDC&)=delete;
};
struct ScopedSelection {
    HDC dc=nullptr; HGDIOBJ old=nullptr;
    ScopedSelection(HDC d,HGDIOBJ object):dc(d),old(d && object?SelectObject(d,object):nullptr) {}
    ~ScopedSelection(){if(dc && old && old!=HGDI_ERROR) SelectObject(dc,old);}
    ScopedSelection(const ScopedSelection&)=delete;ScopedSelection& operator=(const ScopedSelection&)=delete;
};

/* Native menu support, GPL-3.0-or-later; adapted to this mod:
 * inline support types, Italian fallback strings, unique owner class,
 * explicit owner cleanup and menu anchoring at the given point; CBT menu
 * hook and repeated z-order timer removed to reduce taskbar conflicts.
 */
namespace w7t {
std::atomic<HWND> menuOwnerForShutdown{nullptr};
namespace raii {
class MenuHandle {
    HMENU h_;
public:
    explicit MenuHandle(HMENU h=nullptr):h_(h) {}
    ~MenuHandle() { if(h_) DestroyMenu(h_); }
    MenuHandle(const MenuHandle&)=delete;
    MenuHandle& operator=(const MenuHandle&)=delete;
    HMENU get() const { return h_; }
    explicit operator bool() const { return h_!=nullptr; }
    void reset(HMENU h=nullptr) { if(h_) DestroyMenu(h_); h_=h; }
    HMENU release() { HMENU h=h_;h_=nullptr;return h; }
};
}
enum class StrId { SysRestore,SysMove,SysSize,SysMinimize,SysMaximize,SysClose,GroupMinimize,GroupClose };
const wchar_t* S(StrId id) {
    switch(id) {
    case StrId::SysRestore:return Tr(L"Ripristina",L"Restore");
    case StrId::SysMove:return Tr(L"Sposta",L"Move");
    case StrId::SysSize:return Tr(L"Ridimensiona",L"Size");
    case StrId::SysMinimize:return Tr(L"Riduci a icona",L"Minimize");
    case StrId::SysMaximize:return Tr(L"Ingrandisci",L"Maximize");
    case StrId::SysClose:return Tr(L"Chiudi",L"Close");
    case StrId::GroupMinimize:return Tr(L"Riduci a icona il gruppo",L"Minimize group");
    case StrId::GroupClose:return Tr(L"Chiudi tutte le finestre",L"Close all windows");
    }
    return L"";
}
constexpr int W7T_OK=0, W7T_ERR_NOT_FOUND=-1, W7T_ERR_CREATE_WINDOW=-2, W7T_ERR_APPBAR=-3;
class ShellMenu {
public:
    static int32_t ShowWindowSystemMenu(HWND,int32_t,int32_t,bool);
    static int32_t ShowGroupMenu(HWND,int32_t,int32_t,bool,const wchar_t*,const wchar_t*);
    static int32_t ShowPinMenu(int32_t,int32_t,bool,const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
    static int32_t ShowContextMenu(int32_t,int32_t,bool,const wchar_t*);
    static int32_t ShowContextMenuEx(int32_t,int32_t,bool,const wchar_t*,bool=false);
};
}
/*
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * ---------------------------------------------------------------------------
 * v2.6.1 (2026-09): HARDENING + FIX OF THE "FLASH" ON THE TOOLBARS SUBMENU
 *
 * 1. RAII. Every HMENU created here is owned by a w7t::raii::MenuHandle
 *    (RaiiWrappers.h): no more manual CreatePopupMenu()/DestroyMenu()
 *    pairs, no leak possible on early returns or C++ exceptions. All the
 *    exported entry points are wrapped in standard C++ try/catch (the
 *    W7T_SEH_* macros are no-ops on MinGW), so a C++ exception can never
 *    unwind through the P/Invoke boundary into the CLR.
 *
 * 2. THE FLASH. The "Toolbars" submenu of the taskbar context menu used
 *    to flicker on open/close. Root cause: the topmost promotion below
 *    was NOT idempotent. On every HCBT_CREATEWND/HCBT_ACTIVATE of ANY
 *    "#32768" window (the parent menu AND its submenu are both #32768)
 *    the hook re-applied SetWindowLongPtrW(GWL_EXSTYLE) + SetWindowPos,
 *    and each window's 15 ms timer re-issued SetWindowPos(HWND_TOPMOST)
 *    UNCONDITIONALLY. While the submenu was open, the PARENT's timer
 *    yanked the parent back to the head of the topmost band - above its
 *    own submenu - every 15 ms; the menu manager then restored the
 *    submenu above the parent, and the Z-order ping-pong was visible as
 *    a flash (the same restyle churn ran on every submenu activation).
 *    Fix:
 *    - the CBT hook promotes a menu window ONLY while it is not topmost
 *      yet: the style change now happens once, at creation time, while
 *      the window is still invisible (nothing to redraw);
 *    - the timer re-asserts the Z-order ONLY if the window has been
 *      covered by a NON-menu window (the taskbar / its AppBar guard, or
 *      another application). If the window directly above it is another
 *      "#32768" menu - i.e. its own submenu - the order is already
 *      correct and the timer does nothing.
 * ---------------------------------------------------------------------------
 */


namespace w7t {

namespace {

constexpr UINT kGroupMinimizeId = 0xF100;
constexpr UINT kGroupCloseId    = 0xF101;
// The owner is already in the topmost band; the overlay no longer reasserts
// its z-order every second. Do not install CBT hooks or 15ms z-order timers:
// they can compete with taskbar replacements while a menu is open.
class ForegroundMenuScope {
public:
    explicit ForegroundMenuScope(HWND owner) : m_owner(owner) {
        SetForegroundWindow(m_owner);
        SetWindowPos(m_owner,HWND_TOPMOST,0,0,0,0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER);
    }
    ~ForegroundMenuScope() { PostMessageW(m_owner,WM_NULL,0,0); }
private:
    HWND m_owner;
};

thread_local HWND s_modMenuOwner = nullptr;
HWND GetMenuOwnerWindow() {
    /* One owner per calling thread. TrackPopupMenuEx requires an owner
     * created on THIS thread: a process-wide HWND made on the Superbar
     * STA cannot host a menu from the Start Menu STA, so those menus
     * never appeared. */
    HWND& s_owner = s_modMenuOwner;
    if (s_owner != nullptr && IsWindow(s_owner)) {
        return s_owner;
    }

    static ATOM s_atom = 0;
    if (s_atom == 0) {
        WNDCLASSEXW wc = {};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = DefWindowProcW;
        wc.hInstance     = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"WH_Taskbar81SuperbarMenuOwner";
        s_atom = RegisterClassExW(&wc);
        if (s_atom == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return nullptr;
        }
    }

    /* v1.7.6: WS_EX_TOPMOST carries the menu into the topmost band, so a
     * context menu always opens ABOVE the taskbar window itself (see
     * ForegroundMenuScope for the full story). */
    s_owner = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                              L"WH_Taskbar81SuperbarMenuOwner", L"",
                              WS_POPUP, 0, 0, 0, 0,
                              nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    /* TrackPopupMenuEx is unreliable against a never-shown owner: the
     * taskbar context menu then appeared only sometimes. SW_SHOWNOACTIVATE
     * keeps the 0x0 popup in the thread's window list without stealing
     * focus. */
    if (s_owner != nullptr) {
        ShowWindow(s_owner, SW_SHOWNOACTIVATE);
        menuOwnerForShutdown.store(s_owner);
    }
    return s_owner;
}

void BuildFallbackWindowMenu(HMENU popup, HWND target) {
    const LONG_PTR style = GetWindowLongPtrW(target, GWL_STYLE);

    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);

    bool minimized = IsIconic(target) != FALSE;
    bool maximized = IsZoomed(target) != FALSE;
    if (GetWindowPlacement(target, &placement)) {
        minimized = placement.showCmd == SW_SHOWMINIMIZED;
        maximized = placement.showCmd == SW_SHOWMAXIMIZED;
    }

    const bool normal = !minimized && !maximized;

    auto append = [popup](UINT id, const wchar_t* text, bool enabled, bool isDefault) {
        UINT flags = MF_STRING | (enabled ? MF_ENABLED : MF_GRAYED);
        if (isDefault) {
            flags |= MF_DEFAULT;
        }
        AppendMenuW(popup, flags, id, text);
    };

    append(SC_RESTORE,  S(StrId::SysRestore),  minimized || maximized, minimized || maximized);
    append(SC_MOVE,     S(StrId::SysMove),     !minimized,             false);
    append(SC_SIZE,     S(StrId::SysSize),     normal && (style & WS_THICKFRAME) != 0, false);
    append(SC_MINIMIZE, S(StrId::SysMinimize),
           (style & WS_MINIMIZEBOX) != 0 && !minimized, false);
    append(SC_MAXIMIZE, S(StrId::SysMaximize),
           (style & WS_MAXIMIZEBOX) != 0 && !maximized, false);

    AppendMenuW(popup, MF_SEPARATOR, 0, nullptr);
    append(SC_CLOSE, S(StrId::SysClose), true, !(minimized || maximized));
}

UINT CommonFlags(bool bottomEdge) {
    UINT flags = TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN;
    flags |= bottomEdge ? TPM_BOTTOMALIGN : TPM_TOPALIGN;
    return flags;
}

int32_t AnchorYToTaskbarTop(int32_t x, int32_t y, bool bottomEdge) {
    (void)x; (void)bottomEdge; return y;
}

/* ------------------------------------------------------------------ */
/*  Native menu icons, for the program-icon (Superbar) menus ONLY.       */
/*                                                                     */
/*  Rules:                                                             */
/*   - ONLY icons drawn or supplied by Windows itself: the documented    */
/*     HBMMENU_POPUP_* glyph handles for Restore/Minimize/Maximize/     */
/*     Close (the menu code draws them themed, like the caption         */
/*     buttons - no bitmap is invented here). The pin-menu rows stay    */
/*     text-only: no app icon next to the launch entry.                 */
/*   - Everything icon-related is best effort and exception-guarded:     */
/*     any failure degrades to the plain menu, which keeps working       */
/*     exactly as before.                                                */
/*   - The tray/bar/clock/generic menus never pass through here.         */
/* ------------------------------------------------------------------ */

/* The four caption glyphs Windows itself draws in menus. Shared system
 * handles: never destroyed, never modified. Move/Size have no native
 * menu glyph, so they intentionally stay icon-less. */
HBITMAP SystemGlyphForCommand(UINT commandId) noexcept {
    switch (commandId) {
    case SC_RESTORE:  return HBMMENU_POPUP_RESTORE;
    case SC_MINIMIZE: return HBMMENU_POPUP_MINIMIZE;
    case SC_MAXIMIZE: return HBMMENU_POPUP_MAXIMIZE;
    case SC_CLOSE:    return HBMMENU_POPUP_CLOSE;
    default:          return nullptr;
    }
}

/* Attaches a bitmap to one menu item, looked up by command id. Best
 * effort: failures are swallowed on purpose, the menu works with or
 * without the icon. */
void SetItemBitmapByCommand(HMENU menu, UINT commandId,
                            HBITMAP bitmap) noexcept {
    if (menu == nullptr || bitmap == nullptr) {
        return;
    }
    try {
        MENUITEMINFOW info = {};
        info.cbSize   = sizeof(info);
        info.fMask    = MIIM_BITMAP;
        info.hbmpItem = bitmap;
        SetMenuItemInfoW(menu, commandId, FALSE, &info);
    } catch (...) {
        /* icon unavailable: the row stays text-only */
    }
}

/* Walks a window system menu (cloned or rebuilt - both use SC_* ids) and
 * attaches the native glyph where Windows has one. Custom application
 * rows keep whatever id they came with and are never touched. */
void ApplySystemMenuGlyphs(HMENU menu) noexcept {
    if (menu == nullptr) {
        return;
    }
    try {
        const int count = GetMenuItemCount(menu);
        for (int i = 0; i < count; ++i) {
            MENUITEMINFOW info = {};
            info.cbSize = sizeof(info);
            info.fMask  = MIIM_ID | MIIM_FTYPE;
            if (!GetMenuItemInfoW(menu, static_cast<UINT>(i), TRUE, &info)) {
                continue;
            }
            if ((info.fType & MFT_SEPARATOR) != 0) {
                continue;
            }
            HBITMAP glyph = SystemGlyphForCommand(info.wID);
            if (glyph != nullptr) {
                MENUITEMINFOW set = {};
                set.cbSize   = sizeof(set);
                set.fMask    = MIIM_BITMAP;
                set.hbmpItem = glyph;
                SetMenuItemInfoW(menu, static_cast<UINT>(i), TRUE, &set);
            }
        }
    } catch (...) {
        /* icon pass failed: the menu below is still the correct one */
    }
}

} /* namespace */

int32_t ShellMenu::ShowWindowSystemMenu(HWND ownerHwnd, int32_t x, int32_t y,
                                        bool bottomEdge) {
    /* v2.6.1: standard C++ try/catch - the SEH macros are no-ops on
     * MinGW, and nothing may unwind through the P/Invoke boundary. */
    try {
    y = AnchorYToTaskbarTop(x, y, bottomEdge);   /* v2.42 */
    if (ownerHwnd == nullptr || !IsWindow(ownerHwnd)) {
        return W7T_ERR_NOT_FOUND;
    }

    HWND menuOwner = GetMenuOwnerWindow();
    if (menuOwner == nullptr) {
        return W7T_ERR_CREATE_WINDOW;
    }

    DWORD originalPid=0;
    GetWindowThreadProcessId(ownerHwnd,&originalPid);
    /* NOT ours: GetSystemMenu(FALSE) returns a handle owned by the
     * system/target window and must never be destroyed here. */
    HMENU systemMenu = GetSystemMenu(ownerHwnd, FALSE);

    /* v2.6.1: RAII - the popup (and everything appended to it) is
     * destroyed automatically on every return path. */
    raii::MenuHandle popup(CreatePopupMenu());
    if (!popup) {
        return W7T_ERR_APPBAR;
    }

    if (systemMenu == nullptr) {
        BuildFallbackWindowMenu(popup.get(), ownerHwnd);
    } else {
        const int count = GetMenuItemCount(systemMenu);
        for (int i = 0; i < count; ++i) {
            wchar_t text[256] = {};
            MENUITEMINFOW info = {};
            info.cbSize     = sizeof(info);
            info.fMask      = MIIM_ID | MIIM_STATE | MIIM_FTYPE | MIIM_STRING;
            info.dwTypeData = text;
            info.cch        = static_cast<UINT>(std::size(text));

            if (!GetMenuItemInfoW(systemMenu, static_cast<UINT>(i), TRUE, &info)) {
                continue;
            }

            if ((info.fType & MFT_SEPARATOR) != 0) {
                AppendMenuW(popup.get(), MF_SEPARATOR, 0, nullptr);
                continue;
            }

            UINT flags = MF_STRING;
            if ((info.fState & MFS_DISABLED) != 0 ||
                (info.fState & MFS_GRAYED) != 0) {
                flags |= MF_GRAYED;
            }
            if ((info.fState & MFS_CHECKED) != 0) {
                flags |= MF_CHECKED;
            }
            if ((info.fState & MFS_DEFAULT) != 0) {
                flags |= MF_DEFAULT;
            }

            info.dwTypeData = text;
            AppendMenuW(popup.get(), flags, info.wID, text);
        }
    }

    /* v1.7.1: a copied system menu can come back EMPTY or with every item
     * disabled (packaged/UWP frame windows such as Snipping Tool on
     * Windows 11 24H2 expose a stub menu). Windows 7 always shows
     * something there: rebuild the standard menu instead of showing
     * nothing, so Close/Minimize are always available. */
    bool usable = false;
    const int copiedCount = GetMenuItemCount(popup.get());
    for (int i = 0; i < copiedCount && !usable; ++i) {
        MENUITEMINFOW state = {};
        state.cbSize = sizeof(state);
        state.fMask  = MIIM_STATE | MIIM_FTYPE;
        if (GetMenuItemInfoW(popup.get(), static_cast<UINT>(i), TRUE, &state) &&
            (state.fType & MFT_SEPARATOR) == 0 &&
            (state.fState & (MFS_DISABLED | MFS_GRAYED)) == 0) {
            usable = true;
        }
    }
    if (!usable) {
        /* v2.6.1 RAII: reset() destroys the unusable clone and takes
         * ownership of the fresh one; no manual pairing. */
        popup.reset(CreatePopupMenu());
        if (!popup) {
            return W7T_ERR_APPBAR;
        }
        BuildFallbackWindowMenu(popup.get(), ownerHwnd);
    }

    /* Native caption glyphs (Restore/Minimize/Maximize/Close) drawn by
     * Windows itself. Best effort: without them this is still the exact
     * menu built above. */
    ApplySystemMenuGlyphs(popup.get());

    int32_t chosen = 0;
    {
        ForegroundMenuScope scope(menuOwner);
        chosen = static_cast<int32_t>(TrackPopupMenuEx(
            popup.get(), CommonFlags(bottomEdge), x, y, menuOwner, nullptr));
    }

    /* popup destroyed here by MenuHandle */

    if (chosen == 0) {
        return W7T_OK;
    }

    DWORD currentPid=0;
    GetWindowThreadProcessId(ownerHwnd,&currentPid);
    if (originalPid && originalPid==currentPid && IsWindow(ownerHwnd)) {
        if ((chosen & 0xFFF0)==SC_RESTORE) ActivateTask(ownerHwnd);
        else if((chosen & 0xFFF0)==SC_CLOSE) RequestTaskClose(ownerHwnd);
        else if((chosen & 0xFFF0)==SC_MINIMIZE) RequestTaskMinimize(ownerHwnd);
        else PostMessageW(ownerHwnd, WM_SYSCOMMAND, static_cast<WPARAM>(chosen),
                          MAKELPARAM(x, y));
    }
    return W7T_OK;
    } catch (...) {
        /* Never let an exception cross the P/Invoke boundary. */
        return W7T_ERR_APPBAR;
    }
}

int32_t ShellMenu::ShowGroupMenu(HWND ownerHwnd, int32_t x, int32_t y,
                                 bool bottomEdge,
                                 const wchar_t* minimizeText,
                                 const wchar_t* closeText) {
    /* v2.6.1: standard C++ try/catch around the whole body. */
    try {
    y = AnchorYToTaskbarTop(x, y, bottomEdge);   /* v2.42 */
    HWND menuOwner = GetMenuOwnerWindow();
    if (menuOwner == nullptr) {
        return W7T_ERR_CREATE_WINDOW;
    }

    raii::MenuHandle popup(CreatePopupMenu());   /* v2.6.1: RAII */
    if (!popup) {
        return W7T_ERR_APPBAR;
    }

    AppendMenuW(popup.get(), MF_STRING, kGroupMinimizeId,
                minimizeText != nullptr ? minimizeText : S(StrId::GroupMinimize));
    AppendMenuW(popup.get(), MF_SEPARATOR, 0, nullptr);
    AppendMenuW(popup.get(), MF_STRING, kGroupCloseId,
                closeText != nullptr ? closeText : S(StrId::GroupClose));

    /* Native minimize/close glyphs drawn by Windows itself. Best effort. */
    SetItemBitmapByCommand(popup.get(), kGroupMinimizeId, HBMMENU_POPUP_MINIMIZE);
    SetItemBitmapByCommand(popup.get(), kGroupCloseId, HBMMENU_POPUP_CLOSE);

    int32_t chosen = 0;
    {
        ForegroundMenuScope scope(menuOwner);
        chosen = static_cast<int32_t>(TrackPopupMenuEx(
            popup.get(), CommonFlags(bottomEdge), x, y, menuOwner, nullptr));
    }

    /* popup destroyed here by MenuHandle */
    (void)ownerHwnd;

    if (chosen == static_cast<int32_t>(kGroupMinimizeId)) {
        return 1;
    }
    if (chosen == static_cast<int32_t>(kGroupCloseId)) {
        return 2;
    }
    return 0;
    } catch (...) {
        return 0;
    }
}

int32_t ShellMenu::ShowPinMenu(int32_t x, int32_t y, bool bottomEdge,
                               const wchar_t* launchText,
                               const wchar_t* pinText,
                               const wchar_t* lnkPath,
                               const wchar_t* targetPath) {
    /* v2.6.1: standard C++ try/catch around the whole body. */
    try {
    y = AnchorYToTaskbarTop(x, y, bottomEdge);
    HWND menuOwner = GetMenuOwnerWindow();
    if (menuOwner == nullptr) {
        return 0;
    }

    raii::MenuHandle popup(CreatePopupMenu());   /* v2.6.1: RAII */
    if (!popup) {
        return 0;
    }

    /* Same two rows, same ids (1/2) the generic menu produced for the pin:
     * the managed switch on the result does not change. */
    AppendMenuW(popup.get(), MF_STRING, 1,
                launchText != nullptr ? launchText : L"");
    AppendMenuW(popup.get(), MF_STRING, 2,
                pinText != nullptr ? pinText : L"");

    /* Text-only rows: Windows 7 draws no icon next to the launch entry,
     * and exposing one was a regression. lnkPath/targetPath stay in the
     * signature (the managed caller keeps passing them) but are unused. */
    (void)lnkPath;
    (void)targetPath;

    int32_t chosen = 0;
    {
        ForegroundMenuScope scope(menuOwner);
        chosen = static_cast<int32_t>(TrackPopupMenuEx(
            popup.get(), CommonFlags(bottomEdge), x, y, menuOwner, nullptr));
    }

    /* popup destroyed here by MenuHandle */
    return chosen;
    } catch (...) {
        return 0;
    }
}

int32_t ShellMenu::ShowContextMenu(int32_t x, int32_t y, bool bottomEdge,
                                   const wchar_t* itemsSeparatedByNewline) {
    /* v2.6.1: standard C++ try/catch around the whole body. */
    try {
    y = AnchorYToTaskbarTop(x, y, bottomEdge);   /* v2.42 */
    if (itemsSeparatedByNewline == nullptr) {
        return 0;
    }

    HWND menuOwner = GetMenuOwnerWindow();
    if (menuOwner == nullptr) {
        return 0;
    }

    raii::MenuHandle popup(CreatePopupMenu());   /* v2.6.1: RAII */
    if (!popup) {
        return 0;
    }

    UINT nextId = 1;

    std::wstring all(itemsSeparatedByNewline);
    size_t start = 0;

    while (start <= all.size()) {
        size_t end = all.find(L'\n', start);
        if (end == std::wstring::npos) {
            end = all.size();
        }

        std::wstring item = all.substr(start, end - start);
        start = end + 1;

        if (item == L"-") {
            AppendMenuW(popup.get(), MF_SEPARATOR, 0, nullptr);
            continue;
        }

        if (item.empty()) {
            if (end >= all.size()) {
                break;
            }
            continue;
        }

        UINT flags = MF_STRING;
        if (item[0] == L'!') {
            flags |= MF_GRAYED;
            item.erase(0, 1);
        }

        AppendMenuW(popup.get(), flags, nextId++, item.c_str());
    }

    int32_t chosen = 0;
    {
        ForegroundMenuScope scope(menuOwner);
        chosen = static_cast<int32_t>(TrackPopupMenuEx(
            popup.get(), CommonFlags(bottomEdge), x, y, menuOwner, nullptr));
    }

    /* popup destroyed here by MenuHandle */
    return chosen;
    } catch (...) {
        return 0;
    }
}

#ifdef TASKBAR81_TEST
static decltype(&TrackPopupMenuEx) toolbarMenuTrack=TrackPopupMenuEx;
#else
#define toolbarMenuTrack TrackPopupMenuEx
#endif
int32_t ShellMenu::ShowContextMenuEx(int32_t x, int32_t y, bool bottomEdge,
                                     const wchar_t* itemsSeparatedByNewline,
                                     bool anchorAtCursor) {
    /* v2.6.1: standard C++ try/catch around the whole body. */
    try {
    if (!anchorAtCursor) {
        y = AnchorYToTaskbarTop(x, y, bottomEdge);
    }
    if (itemsSeparatedByNewline == nullptr) {
        return 0;
    }

    HWND menuOwner = GetMenuOwnerWindow();
    if (menuOwner == nullptr) {
        return 0;
    }

    /* v2.6.1: RAII. DestroyMenu on the root recursively destroys every
     * submenu appended with MF_POPUP, so the root wrapper owns the whole
     * tree and each child handle is release()d the moment it is attached
     * to its parent (see the ">" branch below). */
    raii::MenuHandle root(CreatePopupMenu());
    if (!root) {
        return 0;
    }

    std::vector<HMENU> stack;
    stack.push_back(root.get());

    UINT nextId = 1;
    std::wstring all(itemsSeparatedByNewline);
    size_t start = 0;

    while (start <= all.size()) {
        size_t end = all.find(L'\n', start);
        if (end == std::wstring::npos) {
            end = all.size();
        }

        std::wstring item = all.substr(start, end - start);
        start = end + 1;

        if (item.empty()) {
            if (end >= all.size()) {
                break;
            }
            continue;
        }

        HMENU current = stack.back();

        if (item == L"<") {
            if (stack.size() > 1) {
                stack.pop_back();
            }
            continue;
        }

        if (item == L"-") {
            AppendMenuW(current, MF_SEPARATOR, 0, nullptr);
            continue;
        }

        if (item[0] == L'>') {
            std::wstring label = item.substr(1);
            /* The child is RAII-guarded until the instant it is attached
             * to its parent; release() hands ownership over (the parent's
             * DestroyMenu will free it). If AppendMenuW ever failed, the
             * wrapper would still destroy the orphan - no leak either way. */
            raii::MenuHandle sub(CreatePopupMenu());
            if (sub) {
                AppendMenuW(current, MF_STRING | MF_POPUP,
                            reinterpret_cast<UINT_PTR>(sub.get()), label.c_str());
                stack.push_back(sub.release());
            }
            continue;
        }

        UINT flags = MF_STRING;
        while (!item.empty() && (item[0] == L'!' || item[0] == L'*')) {
            if (item[0] == L'!') {
                flags |= MF_GRAYED;
            } else {
                flags |= MF_CHECKED;
            }
            item.erase(0, 1);
        }

        AppendMenuW(current, flags, nextId++, item.c_str());
    }

    int32_t chosen = 0;
    {
        ForegroundMenuScope scope(menuOwner);
        chosen = static_cast<int32_t>(toolbarMenuTrack(
            root.get(), CommonFlags(bottomEdge), x, y, menuOwner, nullptr));
    }

    return chosen;
    } catch (...) {
        return 0;
    }
}

} /* namespace w7t */

namespace w7t {
void CleanupShellMenuOwner() {
    menuOwnerForShutdown.store(nullptr);
    if (s_modMenuOwner && IsWindow(s_modMenuOwner)) DestroyWindow(s_modMenuOwner);
    s_modMenuOwner=nullptr;
    UnregisterClassW(L"WH_Taskbar81SuperbarMenuOwner",GetModuleHandleW(nullptr));
}
}


/* Shortcut discovery/validation support, GPL-3.0-or-later.
 * Modified for in-thread polling, no file watcher, no CoreState, no SEH macros,
 * and no path logging. */
namespace w7t {
struct PinnedApp {
    std::wstring identity, lnkPath, target, displayName;
    bool hasAppId;
};
}
// GPL v3 or later


namespace w7t {

namespace {

constexpr int kMaxPathW = 520;

class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE handle = nullptr) : m_handle(handle) {}
    ~ScopedHandle() {
        if (m_handle && m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);
        }
    }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    HANDLE get() const { return m_handle; }
    HANDLE release() {
        HANDLE handle = m_handle;
        m_handle = nullptr;
        return handle;
    }

private:
    HANDLE m_handle;
};

template <typename T>
class UniqueCom {
public:
    UniqueCom() noexcept = default;
    explicit UniqueCom(T* p) noexcept : p_(p) {}
    ~UniqueCom() { reset(); }
    UniqueCom(const UniqueCom&) = delete;
    UniqueCom& operator=(const UniqueCom&) = delete;
    void reset(T* p = nullptr) noexcept {
        if (p_) {
            p_->Release();
        }
        p_ = p;
    }
    T** put() noexcept {
        reset();
        return &p_;
    }
    T* get() const noexcept { return p_; }
    T* operator->() const { return p_; }
    explicit operator bool() const noexcept { return p_ != nullptr; }

private:
    T* p_ = nullptr;
};

class UniqueFind {
public:
    UniqueFind() noexcept = default;
    explicit UniqueFind(HANDLE h) noexcept : h_(h) {}
    ~UniqueFind() {
        if (h_ != nullptr && h_ != INVALID_HANDLE_VALUE) {
            FindClose(h_);
        }
    }
    UniqueFind(const UniqueFind&) = delete;
    UniqueFind& operator=(const UniqueFind&) = delete;
    HANDLE get() const noexcept { return h_; }
    explicit operator bool() const noexcept {
        return h_ != nullptr && h_ != INVALID_HANDLE_VALUE;
    }

private:
    HANDLE h_ = INVALID_HANDLE_VALUE;
};

struct UniquePidl {
    PIDLIST_ABSOLUTE p = nullptr;
    UniquePidl() = default;
    ~UniquePidl() {
        if (p) {
            CoTaskMemFree(p);
        }
    }
    UniquePidl(const UniquePidl&) = delete;
    UniquePidl& operator=(const UniquePidl&) = delete;
};

std::wstring UserPinnedRoot() {
    wchar_t base[MAX_PATH]{};
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, base))) {
        return {};
    }
    return std::wstring(base) +
        L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned";
}

std::wstring PinnedFolder() {
    const std::wstring root = UserPinnedRoot();
    if (root.empty()) {
        return {};
    }
    return root + L"\\TaskBar";
}

std::wstring ImplicitPinnedFolder() {
    const std::wstring root = UserPinnedRoot();
    if (root.empty()) {
        return {};
    }
    return root + L"\\ImplicitAppShortcuts";
}

/* PKEY_AppUserModel_ID: {9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3}, 5 (Windows SDK propkey.h). */
PROPERTYKEY PKEY_Aumid() {
    PROPERTYKEY k{};
    k.fmtid.Data1 = 0x9F4C2855; k.fmtid.Data2 = 0x9F79; k.fmtid.Data3 = 0x4B39;
    const BYTE d[8] = { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 };
    memcpy(k.fmtid.Data4, d, 8);
    k.pid = 5;
    return k;
}

std::wstring ReadAumid(const std::wstring& lnk) {
    std::wstring out;
    try {
        
            UniqueCom<IPropertyStore> ps;
            if (SUCCEEDED(SHGetPropertyStoreFromParsingName(lnk.c_str(), nullptr,
                    GPS_DEFAULT, IID_PPV_ARGS(ps.put())))) {
                PROPERTYKEY key = PKEY_Aumid();
                PROPVARIANT pv{};
                PropVariantInit(&pv);
                if (SUCCEEDED(ps->GetValue(key, &pv)) &&
                    pv.vt == VT_LPWSTR && pv.pwszVal != nullptr) {
                    out = pv.pwszVal;
                }
                PropVariantClear(&pv);
            }
        
        
    } catch (...) {
        out.clear();
    }
    return out;
}

std::wstring NormalizeTarget(IShellLinkW* link, const std::wstring& raw) {
    if (raw.empty()) return {};
    wchar_t exp[kMaxPathW]{};
    ExpandEnvironmentStringsW(raw.c_str(), exp, kMaxPathW);
    PathUnquoteSpacesW(exp);

    std::wstring full = exp;
    if (!full.empty() && !PathIsRootW(full.c_str())) {
        wchar_t wd[kMaxPathW]{};
        if (link != nullptr &&
            SUCCEEDED(link->GetWorkingDirectory(wd, kMaxPathW)) && wd[0]) {
            wchar_t combined[kMaxPathW]{};
            if (PathCombineW(combined, wd, full.c_str())) {
                full = combined;
            }
        }
        wchar_t abs[kMaxPathW]{};
        if (GetFullPathNameW(full.c_str(), kMaxPathW, abs, nullptr)) {
            full = abs;
        }
    }
    std::transform(full.begin(), full.end(), full.begin(), ::towlower);
    return full;
}

bool IsSpecialTarget(const std::wstring& t) {
    return t.rfind(L"::", 0) == 0 || t.rfind(L"shell:", 0) == 0 ||
           t.rfind(L"com:", 0) == 0 || t.rfind(L"ms-", 0) == 0;
}

bool IsPackagedAumid(const std::wstring& id) {
    return id.find(L'!') != std::wstring::npos;
}

bool TargetAlive(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    if (IsSpecialTarget(path)) {
        return true;
    }
    if (GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }
    /* WindowsApps binaries often return ACCESS_DENIED to GetFileAttributes
     * even though the packaged app is installed. That is not a missing pin. */
    return GetLastError() == ERROR_ACCESS_DENIED;
}

// Adaptation: do not log personal paths into the Windhawk log.
void Log(const wchar_t* fmt, ...) { (void)fmt; }

std::wstring ModPinsFolder() {
    wchar_t base[32768]{};
    if(!Wh_GetModStoragePath(base,std::size(base)) || PathIsRelativeW(base))return {};
    std::wstring dir=std::wstring(base)+L"\\pins";
    if(!CreateDirectoryW(dir.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS)return {};
    DWORD attrs=GetFileAttributesW(dir.c_str());
    if(attrs==INVALID_FILE_ATTRIBUTES || !(attrs&FILE_ATTRIBUTE_DIRECTORY) || (attrs&FILE_ATTRIBUTE_REPARSE_POINT))return {};
    return dir;
}
void CollectFolder(const std::wstring& dir,
                   std::vector<PinnedApp>& apps,
                   std::vector<std::wstring>& seenIdentities,bool modOwned=false) {
    if (dir.empty()) {
        return;
    }

    WIN32_FIND_DATAW fd{};
    UniqueFind find(FindFirstFileW((dir + L"\\*.lnk").c_str(), &fd));
    if (!find) {
        return;
    }
    do {
        try {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            const std::wstring name = fd.cFileName;
            const std::wstring lnk = dir + L"\\" + name;

            std::wstring targetRaw, target, aumid, identity, status;
            bool valid = false;
            bool hasIdList = false;

            
            UniqueCom<IShellLinkW> link;
            if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr,
                    CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put())))) {
                UniqueCom<IPersistFile> pf;
                if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(pf.put())))) {
                    if (SUCCEEDED(pf->Load(lnk.c_str(), STGM_READ))) {
                        wchar_t t[kMaxPathW]{};
                        WIN32_FIND_DATAW dummy{};
                        link->GetPath(t, kMaxPathW, &dummy, 0);
                        targetRaw = t;

                        if (targetRaw.empty()) {
                            UniquePidl pidl;
                            if (SUCCEEDED(link->GetIDList(&pidl.p)) &&
                                pidl.p != nullptr) {
                                hasIdList = true;
                                wchar_t p2[kMaxPathW]{};
                                if (SHGetPathFromIDListW(pidl.p, p2)) {
                                    targetRaw = p2;
                                }
                            }
                        }
                        if (IsSpecialTarget(targetRaw)) {
                            target = targetRaw;
                        } else {
                            target = NormalizeTarget(link.get(), targetRaw);
                        }
                    }
                }
            }
            
            

            aumid = ReadAumid(lnk);

            if (!aumid.empty()) {
                identity = aumid;
            } else if (!target.empty()) {
                identity = target;
            } else {
                identity = name;
                std::transform(identity.begin(), identity.end(),
                               identity.begin(), ::towlower);
            }

            /* Packaged (UWP) pins often have an AUMID and either no
             * filesystem path (AppsFolder IDList) or a stub/WindowsApps
             * path that GetFileAttributes cannot see. Those are live
             * pins, not missing targets. */
            if (TargetAlive(target) || TargetAlive(targetRaw)) {
                valid = true;
                status = L"VALID";
            } else if (IsPackagedAumid(aumid)) {
                valid = true;
                status = L"VALID (packaged)";
                if (!TargetAlive(target)) {
                    target = L"shell:AppsFolder\\";
                    target += aumid;
                }
            } else if (target.empty() &&
                       (IsSpecialTarget(targetRaw) || hasIdList)) {
                valid = true;
                status = hasIdList ? L"VALID (shell item)"
                                   : L"VALID (special)";
            } else {
                valid = false;
                status = L"INVALID TARGET";
            }

            bool dup = false;
            if (valid) {
                for (const auto& s : seenIdentities) {
                    if (_wcsicmp(s.c_str(), identity.c_str()) == 0) {
                        dup = true;
                        break;
                    }
                }
            }

            Log(L"[Pinned] %s", name.c_str());
            Log(L"  Target: %s",
                targetRaw.empty() ? L"<none>" : targetRaw.c_str());
            Log(L"  Identity: %s", identity.c_str());
            if (!valid) {
                Log(L"  Status: %s", status.c_str());
                Log(L"  Action: IGNORED");
            } else if (dup) {
                Log(L"  Status: DUPLICATE");
                Log(L"  Action: IGNORED");
            } else {
                Log(L"  Status: %s", status.c_str());
                Log(L"  Action: ADDED");
                seenIdentities.push_back(identity);
                std::wstring disp = name;
                if (disp.size() > 4) disp.erase(disp.size() - 4);
                // Private filenames end in a 16-digit identity hash; do not
                // expose the implementation suffix as the button's title.
                if(modOwned && disp.size()>17 && disp[disp.size()-17]==L'-') {
                    bool hex=true;for(size_t i=disp.size()-16;i<disp.size();++i)if(!iswxdigit(disp[i]))hex=false;
                    if(hex)disp.resize(disp.size()-17);
                }
                apps.push_back(PinnedApp{ identity, lnk, target, disp, !aumid.empty() });
            }
        } catch (...) {
            Log(L"[Pinned] skipped unreadable shortcut");
        }
    } while (FindNextFileW(find.get(), &fd));
}

} /* namespace */

std::vector<PinnedApp> LoadPinnedApps() {
    std::vector<PinnedApp> apps;
    std::vector<std::wstring> seen;
    CollectFolder(ModPinsFolder(),apps,seen,true);
    CollectFolder(PinnedFolder(),apps,seen);
    CollectFolder(ImplicitPinnedFolder(),apps,seen);
    return apps;
}
} /* namespace w7t */

// Clock framing uses the same-thread subclass fallback. The private symbol
// hooks from v5 were removed because resolving them could block mod startup.
// RetroBar's Windows 8+ Aero clock COM interface is not a public Windows
// contract. Keep the interface type outside an anonymous namespace: GCC may
// otherwise devirtualize calls to a pure virtual method at optimization time.
struct W8AeroClock : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ShowFlyout(HWND taskbar, RECT* taskbarRect) = 0;
};

namespace {
using namespace Gdiplus;
const wchar_t kClass[] = L"WH_Taskbar81_GDILab";
const UINT WM_UPDATE_SETTINGS = WM_APP + 313;
const UINT WM_RESOURCES_READY=WM_APP+314, WM_WIFI_CHANGED=WM_APP+315;
const UINT_PTR TIMER_REFRESH = 7;
const UINT_PTR TIMER_HOVER = 8;
const UINT_PTR TIMER_PREVIEW = 9;
const UINT_PTR TIMER_CLOCK_FIX = 10;
const UINT_PTR TIMER_ANIMATION = 11;
const UINT_PTR TIMER_Z_GUARD = 12;
const UINT_PTR TIMER_PEEK = 13;
const UINT_PTR TIMER_FADE = 14;
const UINT_PTR TIMER_DESKTOP_PEEK = 15;
const UINT_PTR TIMER_ACTIVATE_RETRY = 16, TIMER_DRAG_ACTIVATE = 17, TIMER_PREVIEW_ANIM=18;
const UINT_PTR TIMER_BATTERY_SLIDE = 19, TIMER_RESOURCE_RELOAD = 20, TIMER_CLASSIC_POSITION_NOTIFY = 21, TIMER_APPBAR_NOTIFY=22, TIMER_APPBAR_VERIFY=23, TIMER_NETWORK_POSITION=24, TIMER_VOLUME_POSITION=25, TIMER_ACTION_POSITION=26;
const UINT_PTR TIMER_ATTENTION=27, TIMER_BUTTON_TRANSITION=28, TIMER_LANGUAGE_REFRESH=29, TIMER_WIN7_PREVIEW_CLOSE=30;
const UINT WM_T81_APPBAR=WM_APP+452;
const wchar_t kNativeControlOverlayClass[] = L"WH_Taskbar81NativeControlOverlay";
const wchar_t kPreviewClass[] = L"WH_Taskbar81SuperbarPreview";
const wchar_t kTooltipClass[] = L"WH_Taskbar81PinTooltip";
const int HIT_START = -1, HIT_DESKTOP = -2, HIT_NONE = -3, HIT_OVERFLOW = -4;
const int HIT_CLOCK=-5,HIT_NETWORK=-6,HIT_VOLUME=-7,HIT_BATTERY=-8,HIT_TRAY_MORE=-9,HIT_BAND_LINKS=-10,HIT_BAND_DESKTOP=-11,HIT_BAND_ADDRESS=-12,HIT_LANGUAGE=-13,HIT_TRAY_ICON_BASE=-1000;
constexpr size_t MAX_TASKS = 80;

struct Task {
    HWND hwnd;
    DWORD pid;
    HICON icon;
    bool ownsIcon;
    std::wstring title;
    std::wstring key;
    std::wstring processPath;
    std::wstring relaunchPath;
    ULONGLONG seq=0;
};
struct Pin {
    std::wstring key, path, title, target;
    HICON icon = nullptr; // owned copy from SHGetFileInfoW, released on refresh
};
struct Group { std::wstring key; std::vector<size_t> tasks; int pin=-1; std::wstring baseKey; };
static const std::wstring& GroupBase(const Group& g) {return g.baseKey.empty()?g.key:g.baseKey;}
struct AttentionItem {HWND hwnd=nullptr;DWORD pid=0;ULONGLONG until=0;};
struct PreviewItem {
    HWND hwnd;
    DWORD processId;
    RECT cell;
    RECT win7ThumbnailRect{};
    HTHUMBNAIL thumbnail;
    std::wstring title;
    HICON icon;
    bool more=false;
};
struct PreviewExtra { HWND hwnd; DWORD processId; std::wstring title; };
struct VisualPosition { std::wstring key; float x; };
struct HoverFade { std::wstring key; float value=0; };
// A closing visual owns an independent icon; it is never a task or hit target.
struct ButtonTransition {
    std::wstring key, title;
    std::shared_ptr<std::remove_pointer<HICON>::type> icon;
    RECT rect{};
    int layers=0, barWidth=0, barHeight=0;
    ULONGLONG start=0;
    bool ghost=false, vertical=false, reveal=false;
};
struct PendingButtonChanges {
    std::vector<ButtonTransition> closed;
    std::vector<std::wstring> previousKeys;
    bool enabled=false;
};

// Native adaptation of the message protocol used by ManagedShell TrayService,
// NotificationArea and NotifyIcon (Apache-2.0, cairoshell/ManagedShell,
// commit 855222035e927910ca53fd3215caef056337a048). This is NOT the original
// C#/WPF implementation. In particular, ExplorerTrayService's legacy toolbar
// full ExplorerTrayService enumeration, balloon UI, and the original overflow
// class are not ported. The private model has persisted pinning and drag/drop;
// classic Explorer toolbar startup import is partial and undocumented.
#pragma pack(push, 4)
struct TrayWireIcon {
    DWORD cbSize, hwnd32, id, flags, callback, icon32;
    WCHAR tip[128]; DWORD state, stateMask; WCHAR info[256];
    DWORD version; WCHAR title[64]; DWORD infoFlags; GUID guid; DWORD balloonIcon;
};
struct TrayWireData { DWORD magic, message; TrayWireIcon icon; };
struct TrayWireIdentifier { DWORD magic, message, cbSize, padding, hwnd32, id; GUID guid; };
#pragma pack(pop)
static_assert(offsetof(TrayWireData,icon)==8, "ManagedShell wire header");

struct LanguageFlyoutItem {
    HKL hkl=nullptr; LANGID langId=0;
    std::wstring abbreviation,region,languageName,layoutName;
};
struct PrivateIcon {
    HWND owner=nullptr; DWORD ownerPid=0, id=0, callback=0, version=0;
    GUID guid{}; HICON icon=nullptr; std::wstring tip, preferenceKey; bool hidden=false, importedFromNative=false;
};

static bool GuidPresent(const GUID& guid) {
    static const GUID zero{}; return memcmp(&guid,&zero,sizeof(guid))!=0;
}
static bool MatchesIcon(const PrivateIcon& item, HWND owner, DWORD id, const GUID& guid) {
    if(GuidPresent(guid) && GuidPresent(item.guid)) return IsEqualGUID(guid,item.guid);
    return owner && item.owner==owner && item.id==id;
}
static bool IsSystemTrayGuid(const GUID& g) {
    // ManagedShell's fixed system icon GUIDs: power, network and volume.
    if(g.Data2!=0x23e3 || g.Data3!=0x4229) return false;
    const BYTE tail[8]={0x82,0xc1,0xe4,0x1c,0xb6,0x7d,0x5b,0x9c};
    return !memcmp(g.Data4,tail,8) && g.Data1>=0x7820ae73 && g.Data1<=0x7820ae75;
}

// Mod-owned notification identities, following Action Center Recreation's
// real Shell_NotifyIconW registration model. These are deliberately distinct
// from Windows' reserved SCA GUIDs: adding/deleting our icons must never alter
// the operating system's own network, volume or power registrations.
static GUID CniSystemGuid(int hit) {
    DWORD suffix = hit==HIT_NETWORK ? 1u : hit==HIT_VOLUME ? 2u : 3u;
    return {0x6C7336F0, 0x8A60, 0x4F2A,
            {0x9E,0x51,0x2E,0x7A,0x3C,0x81,0xA0,(BYTE)suffix}};
}
static int CniSystemSlot(int hit) {
    return hit==HIT_NETWORK ? 0 : hit==HIT_VOLUME ? 1 : hit==HIT_BATTERY ? 2 : -1;
}
static int CniSystemHitFromGuid(const GUID& guid) {
    for(int hit : {HIT_NETWORK,HIT_VOLUME,HIT_BATTERY}) {
        GUID own=CniSystemGuid(hit);
        if(IsEqualGUID(guid,own)) return hit;
    }
    return 0;
}
static const wchar_t* CniSystemDisplayName(int hit) {
    return hit==HIT_NETWORK ? Tr(L"Rete",L"Network") :
           hit==HIT_VOLUME ? Tr(L"Volume",L"Volume") : Tr(L"Batteria",L"Battery");
}
constexpr UINT WM_T81_CNI_ICON = WM_APP + 949;

struct HiddenStartWindow { HWND hwnd=nullptr; DWORD pid=0, tid=0; ATOM cls=0; ULONGLONG lastLogged=0; };
struct TrayDragState {
    HWND capture=nullptr, ghost=nullptr; HICON image=nullptr; std::wstring key;
    POINT origin{}; bool active=false, fromOverflow=false; int systemHit=0; ULONGLONG arrowSince=0;
    HHOOK hook=nullptr; bool moveQueued=false,endQueued=false,nativeUpSeen=false; ULONGLONG releasedSince=0;
    int dropArea=0; RECT dropCell{}; bool dropAfter=false; std::wstring dropAnchor;
};
struct State {
    HWND bar = nullptr;
    HWND batteryFrameWindow=nullptr; LONG_PTR batteryFrameOriginalStyle=0,batteryFrameOriginalExStyle=0;
    HWND batterySlideWindow=nullptr; POINT batterySlideTarget{}; int batterySlideOffset=0; ULONGLONG batterySlideStart=0;
    bool perPixelBar=false, perPixelFrame=false, legacyAlphaReady=false, perPixelPreview=false, previewPixelFrame=false;
    HWND native = nullptr;
    HWND privateHost = nullptr, privateNotify = nullptr; HMODULE trayModule = nullptr; ATOM trayAtom = 0, notifyAtom = 0;
    HWND overflowPanel=nullptr, overflowTooltip=nullptr; ATOM overflowClass=0; RECT overflowAnchor{};
    int overflowScroll=0, overflowHot=-1, overflowMouseMask=0; bool overflowFooterHot=false;
    std::wstring overflowTooltipText;
    Gdiplus::Image* overflowHoverImage=nullptr; IStream* overflowHoverStream=nullptr; bool overflowImageTried=false;
    bool privateActive=false, nativeWasVisible=false, nativeHiddenByUs=false;
    bool verticalActive=false; int verticalSide=0, verticalOffset=0, verticalPressed=HIT_NONE;
    std::vector<PrivateIcon> trayIcons;
    // CNI-only registrations: owned HICON copies remain alive across callbacks,
    // but these entries never join trayIcons and therefore never duplicate the
    // synthetic network/volume/battery glyphs in the visible private tray.
    PrivateIcon cniSystemIcons[3];
    NOTIFYICONDATAW cniSystemNid[3]{};
    HICON cniSystemPublishedIcons[3]{};
    bool cniSystemRegistered[3]{};
    std::vector<std::wstring> trayPinned, trayHiddenSystem, trayOverflowOrder, trayBarOrder;
    // [T81-CNI] EnableAutoTray(FALSE) means "Always show all icons".
    bool trayShowAll = false;
    bool trayPreferencesInitialized=false, nativeSnapshotActive=false, nativeSnapshotTimedOut=false;
    std::vector<HiddenStartWindow> hiddenStarts;
    TrayDragState trayDrag;
    bool suppressTrayRelease=false;ULONGLONG suppressTrayReleaseUntil=0;
    bool nativeWasTopmost=false; ULONGLONG nativeHideLastLogged=0; HWINEVENTHOOK visibilityHook=nullptr;
    IAudioEndpointVolume* endpoint=nullptr; int volumeWheelRemainder=0;
    float volume=0; bool muted=false,hasBattery=false,charging=false,batteryCharging=false;
    int batteryPct=-1,netKind=0;
    bool peekOn=false,peekDesktop=false,desktopPeekEnabled=true; HWND peekTarget=nullptr,peekCandidate=nullptr;
    DWORD peekCandidatePid=0;
    RECT nativeRect{}; UINT nativeEdge=ABE_BOTTOM;
    bool rectValid = false;
    bool tracking = false;
    bool startPressed = false;
    bool dragCandidate = false;
    bool dragActive = false;
    POINT dragStart{};
    int dragCursorX = 0;
    int dragGrabOffset = 0;
    std::vector<VisualPosition> visualPositions;
    std::vector<ButtonTransition> buttonTransitions; ULONGLONG buttonPaintTick=0;
    std::vector<HoverFade> hoverFades;
    float startFade=0; ULONGLONG fadeTick=0;
    ULONGLONG animationTick = 0;
    std::vector<ULONGLONG> zRepairs;
    ULONGLONG zGuardSuspendedUntil = 0;
    int dragInsertion = -1;
    std::wstring dragKey;
    std::vector<std::wstring> customOrder;
    bool preservingTray = true;
    bool desktopBand=false, linksBand=false, addressBand=false,desktopArrowHot=false,linksArrowHot=false;
    int nativeGlomLevel=-1;
    UINT shellHookMessage=0;bool shellHookRegistered=false,attentionOn=false;
    std::vector<AttentionItem> attention;
    bool labelMode=false;
    ULONGLONG nextTaskSequence=0;
    HWND desktopPopup=nullptr; bool desktopPopupClass=false;
    HWND addressHost=nullptr, addressEdit=nullptr; HFONT addressFont=nullptr; int addressFontDpi=0; bool addressClass=false;
    int clockWidthPx=0, clockMetricsDpi=0;
    std::wstring clockMetricTime, clockMetricDate;
    HKL languageHkl=nullptr; HWND languageTarget=nullptr;
    size_t languageLayoutCount=0; int languageWidthPx=0,languageMetricsDpi=0;
    std::wstring languageText,languageRegion,languageName,languageTag;
    HWND languageFlyout=nullptr,languageFlyoutTarget=nullptr;
    DWORD languageFlyoutTargetPid=0,languageFlyoutTargetThread=0;
    bool languageFlyoutClass=false,languageFlyoutFooterHover=false;
    int languageFlyoutHover=-1; size_t languageFlyoutSelected=0;
    std::vector<LanguageFlyoutItem> languageFlyoutItems;
    std::vector<COLORREF> trayColors;
    Gdiplus::Image* startImage = nullptr;
    IStream* startStream = nullptr;
    Gdiplus::Image* win7StartImage = nullptr; IStream* win7StartStream = nullptr;
    Gdiplus::Image* betaStartImage = nullptr;
    IStream* betaStartStream = nullptr;
    Gdiplus::Image* betaTexture[33]{}; IStream* betaTextureStream[33]{};
    bool betaDesktopPressed=false;
    Gdiplus::Bitmap* netImages[8]{}; Gdiplus::Bitmap* volumeImages[6]{}; Gdiplus::Bitmap* batteryStrip=nullptr; Gdiplus::Bitmap* batteryFrames[43]{};
    int batteryFrameSize=0; float batteryOffset[43]{}; std::wstring resourceDir; HANDLE resourceThread=nullptr,resourceStop=nullptr;
    std::wstring batteryInstanceDir; HANDLE batteryInstanceLock=INVALID_HANDLE_VALUE; bool batteryCleanupDone=false;
    bool bandsLocked=true; int bandOrder[3]{0,1,2}; int bandDragHit=0; POINT bandDragStart{};
    // 0 follows native, 1 top, 2 bottom; 3/4 left/right only when All edges placement mode is selected.
    int barEdgeOverride=0,barDragInitial=0,barDragPending=0;bool barDragCandidate=false,barDragMoved=false;
    POINT barDragStart{};RECT barDragMonitor{};HHOOK barDragHook=nullptr;bool barHookEndQueued=false,barMoveQueued=false;
    // Last known external system locations; -1 means unavailable/unknown.
    bool edgeSourcesInitialized=false;int sourceNativeEdge=-1,sourceModernEdge=-1,sourceClassicEdge=-1;
    bool appbarRegistered=false,appbarDirty=false,appbarArranging=false,appbarFullScreen=false;
    UINT appbarEdge=ABE_BOTTOM;RECT appbarDesired{},appbarApproved{};ULONGLONG appbarRetryAt=0;
    int previewAnimX=0,previewAnimY=0,previewAnimOffset=0,previewAnimOffsetX=0; ULONGLONG previewAnimStart=0; bool previewAnimClosing=false;
    bool dropRegistered=false;
    HWND activateTarget=nullptr, dragTarget=nullptr;
    DWORD activatePid=0, activateInput=0, dragPid=0;
    int activateRetries=0;
    std::wstring dragHoverKey;
    bool dragActivated=false;
    HWND nativeClock = nullptr;
    HWND clockOverlay = nullptr;
    HWND volumeToolbar = nullptr;
    HWND volumeOverlay = nullptr;
    HWND clockFlyout = nullptr;
    W8AeroClock* aeroClock = nullptr;
    int clockFixAttempts = 0;
    bool toolbarTimedOut = false;
    DWORD lastSndvolTick = 0, volumeLaunchPid=0;
    int networkPositionAttempts=0,volumePositionAttempts=0,actionPositionAttempts=0;
    RECT networkAnchor{},volumeAnchor{},actionAnchor{};
    DWORD favoritesVersion = 0;
    bool favoritesVersionKnown = false;
    int hot = HIT_NONE;
    int ticks = 0;
    HWND preview = nullptr;
    HWND jumpPopup = nullptr, jumpClosing=nullptr;
         std::wstring jumpGroupKey,jumpDismissKey,jumpPressedKey,jumpLeftDismissKey,jumpLeftPressedKey;
         ULONGLONG jumpDismissAt=0,jumpLeftDismissAt=0;
    HWND tooltip = nullptr;
    std::wstring tooltipText;
    bool hotCornerPreview=false; ULONGLONG recoveryTooltipUntil=0;
    Gdiplus::Bitmap* tintBeta[2]{}; Gdiplus::Image* tintBetaSource[2]{}; int tintBetaKey[2]{-1,-1};
    int hover = HIT_NONE;
    std::vector<Task> tasks;
    std::vector<Group> groups;
    std::vector<Pin> pins;
    std::vector<PreviewItem> previews;
    std::vector<PreviewExtra> previewExtras;
    std::wstring previewGroupKey; int previewOffset=0,previewPageSize=0,previewTotal=0;
    int previewHot=-1;
    bool previewCloseHot=false;
    float win7PreviewCloseFade=0.f; ULONGLONG win7PreviewCloseTick=0;
    bool previewClosePress=false,previewCloseCancelled=false;
    int previewClosePressedIndex=-1; HWND previewClosePressedWindow=nullptr; DWORD previewClosePressedPid=0;
    bool previewTracking=false;
    State()=default;
    State(const State&)=delete;
    State& operator=(const State&)=delete;
    ~State(){if(batteryInstanceLock!=INVALID_HANDLE_VALUE && batteryInstanceLock)CloseHandle(batteryInstanceLock);}
};
// State is owned and freed by the UI thread; no destructible globals.
State* s = nullptr;
std::atomic<HWND> barWindow{nullptr};
std::atomic<HWND> nativeWindow{nullptr};
std::atomic<bool> quitting{false};
std::atomic<bool> enablePrivateTray{true},reserveWorkArea{true};
std::atomic<int> taskbarPlacementMode{3};
std::atomic<bool> reserveVerticalWorkArea{true};
std::atomic<int> taskbarOpacity{220}, taskbarBackgroundOpacity{155};
std::atomic<bool> perPixelTaskbar{true},perPixelPreview{true};
std::atomic<int> previewOpacity{200};
std::atomic<bool> aeroPeek{true}, useBetaStartLogo{false};
// 0=custom, 1=7, 2=8 beta, 3=8.0, 4=8.1. Presets are effective values only.
std::atomic<int> taskbarSkin{0};
std::atomic<bool> startHotCorner{false}, customStartColor{false}, showRecoverableErrors{true};
std::atomic<int> startLogoRgb{0x0078D7};
std::atomic<bool> customTaskbarColor{false};
std::atomic<int> taskbarColorRgb{0x0078D7};
std::atomic<bool> matchNativeTray{true};
std::atomic<bool> allowDragReorder{true};
std::atomic<bool> keepOverlayAbove{true};
std::atomic<bool> animateReorder{true};
std::atomic<int> trayFallbackWidth{240};
std::atomic<bool> preferClassicClock{true};
std::atomic<bool> preferClassicVolume{true};
std::atomic<bool> useOriginalBatteryFlyout{false}, networkUse100PercentArtwork{false};
std::atomic<bool> useAccent{false}, volumeWheel{false};
std::atomic<bool> showLanguageIndicator{true}, showJumpShowWindow{true};
std::atomic<int> jumpCloseStyle{0};
HANDLE threadHandle = nullptr;
DWORD threadId = 0;
// One-time migration for renamed settings. Wh_GetIntValue's sentinel preserves
// an explicit false/0, which Wh_GetIntSetting cannot distinguish from missing.
// Keep legacy values for rollback; after the marker is written, only the new
// settings are read, so Reset defaults and subsequent user changes are honored.
static int TaskbarPlacementFromSetting(const std::wstring& value) {
    if(value==L"native")return 0;
    if(value==L"left")return 1;
    if(value==L"right")return 2;
    return 3; // allEdges, including an unrecognized imported value
}
static void MigrateTaskbarSettings() {
    constexpr int kSettingsSchemaVersion=1;
    const wchar_t* marker=L"Taskbar81.SettingsSchemaVersion";
    if(Wh_GetIntValue(marker,0)>=kSettingsSchemaVersion)return;
    const struct {const wchar_t* previous;const wchar_t* current;} renamed[]={
        {L"experimentalReserveWorkArea",L"reserveWorkArea"},
        {L"experimentalVerticalReserveWorkArea",L"reserveVerticalWorkArea"},
        {L"experimentalPerPixelBar",L"perPixelTaskbar"},
        {L"experimentalPerPixelPreview",L"perPixelPreview"},
        {L"legacyBatteryFlyoutExperimental",L"useOriginalBatteryFlyout"},
    };
    bool complete=true;
    for(const auto& item:renamed) {
        int previous=Wh_GetIntValue(item.previous,-1);
        if(previous==0 || previous==1)
            complete=Wh_SetIntValue(item.current,previous)!=FALSE && complete;
    }
    int previousMode=Wh_GetIntValue(L"experimentalVerticalSide",-1);
    if(previousMode>=0 && previousMode<=3) {
        const wchar_t* names[]={L"native",L"left",L"right",L"allEdges"};
        complete=Wh_SetStringValue(L"taskbarPlacement",names[previousMode])!=FALSE && complete;
    }
    if(complete)Wh_SetIntValue(marker,kSettingsSchemaVersion);
    else Wh_Log(L"[T81-DIAG] settings migration incomplete; will retry on next initialization");
}
void LoadSettings() {
    MigrateTaskbarSettings();
    enablePrivateTray.store(Wh_GetIntSetting(L"enablePrivateTray") != 0);
    reserveWorkArea.store(Wh_GetIntSetting(L"reserveWorkArea") != 0);
    {
        PCWSTR value=Wh_GetStringSetting(L"taskbarPlacement");
        std::wstring placement(value?value:L"allEdges");
        Wh_FreeStringSetting(value);
        taskbarPlacementMode.store(TaskbarPlacementFromSetting(placement));
    }
    reserveVerticalWorkArea.store(Wh_GetIntSetting(L"reserveVerticalWorkArea")!=0);
    { int value=Wh_GetIntSetting(L"taskbarOpacity"); taskbarOpacity.store(value>=140 && value<=255?value:220); }
    { int value=Wh_GetIntSetting(L"taskbarBackgroundOpacity"); taskbarBackgroundOpacity.store(value>=100 && value<=255?value:155); }
    perPixelTaskbar.store(Wh_GetIntSetting(L"perPixelTaskbar") != 0);
    perPixelPreview.store(Wh_GetIntSetting(L"perPixelPreview") != 0);
    { int value=Wh_GetIntSetting(L"previewOpacity"); previewOpacity.store(value>=140 && value<=255?value:200); }
    aeroPeek.store(Wh_GetIntSetting(L"enableAeroPeek") != 0);
    useBetaStartLogo.store(Wh_GetIntSetting(L"useBetaStartLogo") != 0);
    startHotCorner.store(Wh_GetIntSetting(L"startHotCorner") != 0);
    {
        PCWSTR value=Wh_GetStringSetting(L"taskbarSkin");
        std::wstring skin(value?value:L"custom");Wh_FreeStringSetting(value);
        int mode=skin==L"win7"?1:skin==L"win8beta"?2:skin==L"win8"?3:skin==L"win81"?4:0;
        taskbarSkin.store(mode);
        if(mode) {useBetaStartLogo.store(mode==1 || mode==2);startHotCorner.store(mode==3);}
    }
    customStartColor.store(Wh_GetIntSetting(L"customStartColor") != 0);
    showRecoverableErrors.store(Wh_GetIntSetting(L"showRecoverableErrors") != 0);
    int r=std::clamp(Wh_GetIntSetting(L"startLogoRed"),0,255);
    int g=std::clamp(Wh_GetIntSetting(L"startLogoGreen"),0,255);
    int b=std::clamp(Wh_GetIntSetting(L"startLogoBlue"),0,255);
    startLogoRgb.store((r<<16)|(g<<8)|b);
    customTaskbarColor.store(Wh_GetIntSetting(L"customTaskbarColor") != 0);
    int tr=std::clamp(Wh_GetIntSetting(L"taskbarColorRed"),0,255);
    int tg=std::clamp(Wh_GetIntSetting(L"taskbarColorGreen"),0,255);
    int tb=std::clamp(Wh_GetIntSetting(L"taskbarColorBlue"),0,255);
    taskbarColorRgb.store((tr<<16)|(tg<<8)|tb);
    matchNativeTray.store(Wh_GetIntSetting(L"matchNativeTray") != 0);
    allowDragReorder.store(Wh_GetIntSetting(L"allowDragReorder") != 0);
    keepOverlayAbove.store(Wh_GetIntSetting(L"keepOverlayAbove") != 0);
    animateReorder.store(Wh_GetIntSetting(L"animateReorder") != 0);
    preferClassicClock.store(Wh_GetIntSetting(L"preferClassicClock") != 0);
    preferClassicVolume.store(Wh_GetIntSetting(L"preferClassicVolume") != 0);
    useOriginalBatteryFlyout.store(Wh_GetIntSetting(L"useOriginalBatteryFlyout") != 0);
    networkUse100PercentArtwork.store(Wh_GetIntSetting(L"networkUse100PercentArtwork") != 0);
    int fallback=Wh_GetIntSetting(L"trayFallbackWidth");
    trayFallbackWidth.store(std::max(96,std::min(fallback ? fallback : 240,600)));
    useAccent.store(Wh_GetIntSetting(L"useAccent") != 0);
    volumeWheel.store(Wh_GetIntSetting(L"volumeWheel") != 0);
    showLanguageIndicator.store(Wh_GetIntSetting(L"showLanguageIndicator") != 0);
    showJumpShowWindow.store(Wh_GetIntSetting(L"showJumpShowWindow") != 0);
}
// Three 50 x 41 state frames; preserve alpha and stream lifetime.
static const char kStartPngBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAADIAAAB7CAYAAAA2aNMVAAADr0lEQVR4AexaMU7rQBCNUToaQJHQvwGUv6WChluE"
    "gnsghLgHBbkFDVS0lHCDr0hRkoYukv+bAonY3mXeZnZjkUH74mQ9OzvvvZWM7d0b/JI/J9I3I90RdySTAr1ZWnVd"
    "V8AQ2AeOWb5FiaDAtWLx+wQ4B8Yo/AZ4AJ6AN4Bq5kRQFFPsO6p9Bh6BO0AIneH4B6BaEhFtsVVVTVGNWbHIFWw0"
    "EZAQ1ZrLIFRscGLrEzQRFLDxMkAO85ZCxLwIi4ROxEJFyxzuiKWaFrncEQsVLXPQjuBqrW5SqDq4EShjGdBEcGUP"
    "tVa/FNLqVHbIWAY0ESZ5yVgnUlJtzVzuiEalkjHuSEm1NXOlOHKFxLfABHgF/gFbbzQRXICFwD0qvwYugb/AKXAB"
    "NEmiq0yjiUhZIFMDK+ATmAIfwAuwRhIXcXk+FSJp6mQSESETAsioSGJ8zEmapDkRFBhsJMlgnq4TRYl0FfDV1yT5"
    "1a899oaItuBQnBMJKbOtfndkW8qH5nVHQsoY9dNp9g4PD+vfAF9atPeZB7gjmQWm07sjtGQ/D6gRsgI+cUMmb4Px"
    "Vd9KO9Is9gOlvgAT/Bt/DwLXOF4Oh0O56UK3vuUgoi52sVicAhfA1Xw+v10ulxMcX2ezWbE7RFWxUPcYRZoVG/Mn"
    "xZHWMggVG5vY+hxNBEVvvAysSUg+mogM6iOcSN9ccUfckUwK7O7SYm6LRXwm/nusjGUQcaQ7DS6IlRaSQRvbjJOx"
    "DGgiTPKSsU6kpNqaudwRjUolY9yRkmpr5qIdwUXr8ejo6O7g4GCM49loNKI35GsKY2NoIphgjKcdN7gff8DxabVa"
    "vYHcO/AMrJFEbLGWQkSKq/AxBPZBSDYFnOD7ObBGEkSnINdJ0trJVCKoOdhUJGNOppDMQSTIECfUJBFLtdJEYsU1"
    "ScZiW+f6RKRVHNPhRBi1SsS6IyVUZuZwRxi1BoP80e5Ifo25GdwRTq/80e5Ifo25GfrkSPNNMcWkNJFmsb5hoGlX"
    "qiMqZeV+Hk/ZfcNAU/XYb9oRKOwbBmKKbnqOdmTTCXONdyK5lE3N646kKpdr3O46gqfr6r30oj4T/z1Wxnaju5d2"
    "BBdE3zDQraVNL+2IzbT2WZyIvaabZXRHNtPPfvTuOoKL1tq79JQ3sPZ+DAYpjqy9S4+9Zs5RcChnChHJVeHDNwzU"
    "dR3d+pGyXFMdgSFJTeWkLFc2e2kisfqaJGOxrXN9ItIqjulwIoxaJWLdkRIqM3PsuCOMVIVi/wMAAP//NyYHNQAA"
    "AAZJREFUAwBBxpDo33helAAAAABJRU5ErkJggg==";

static std::vector<BYTE> DecodeBase64(const char* text) {
    std::vector<BYTE> bytes;
    unsigned accumulator=0;
    int bits=0;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(text); *p; ++p) {
        unsigned v;
        if(*p>='A' && *p<='Z') v=*p-'A';
        else if(*p>='a' && *p<='z') v=*p-'a'+26;
        else if(*p>='0' && *p<='9') v=*p-'0'+52;
        else if(*p=='+') v=62;
        else if(*p=='/') v=63;
        else if(*p=='=') break;
        else continue;
        accumulator=(accumulator<<6)|v;
        bits+=6;
        if(bits>=8) { bits-=8; bytes.push_back((BYTE)((accumulator>>bits)&255)); }
    }
    return bytes;
}
// Microsoft symbol-server resources. Adapted conceptually from Anixx's
// win10-taskbar-on-win11-24h2: pinned SHA-256, mod-local cache and fallback.
// The 8.1 downloads are data only. A separately gated 1511 binary is executable.
// Fixed SHA-256 and sizes pin each download; this does not independently verify catalogs.
int Dpi();
int Px(int n);
struct LegacyFile {const wchar_t* file;const wchar_t* urlPath;const char* hash; DWORD size;};
static const LegacyFile kLegacyFiles[]={
 {L"pnidui.dll",L"/download/symbols/pnidui.dll/5450390D9D000/pnidui.dll","67cfd88515dff73e5772e9cf805e74d62e851107cd7660eb83df405377282279",628224},
 {L"SndVolSSO.dll",L"/download/symbols/SndVolSSO.dll/5450366040000/SndVolSSO.dll","db09e74013eac1b8aca84ac95653ede33c6aa93287fa71da9769ebaaf6e95d58",234496},
 {L"batmeter.dll",L"/download/symbols/batmeter.dll/545054931F3000/batmeter.dll","f32f18d44f9a6511c73ca1a9a4a6edad38aff23a15fd4c75d9aaaaf31526a506",2022912}
};
// This is Windows 10 1511, NOT the 8.1 batmeter resource DLL. Never overwrite
// system files. ShowBatteryUI below is internal and version-specific.
static const LegacyFile kBattery1511={L"stobject.dll",L"/download/symbols/stobject.dll/5632D3AA64000/stobject.dll","2daafd7672036a18d3b89d9d85086b0ba4e692c510b4398ebe599d62e1a3c7c9",387072};
// Only these pinned Microsoft language packs are allowed to exceed 4 MiB.
static const LegacyFile kBatteryCabIt={L"battery-lang-it.cab",L"/c/msdownload/update/software/updt/2015/11/lp_b7e30dbc9fd06b34116a61a90a21a472ae90a08f.cab","063a4e6dc9372ef47ebf5af55614a3a9ae3eeb99e3f872111af7883001d5f95c",38671131};
static const LegacyFile kBatteryCabEn={L"battery-lang-en.cab",L"/c/msdownload/update/software/updt/2015/11/lp_cde3e6fcf3e36d655948c41fc1dd847c09f7d23f.cab","cde0ae927a92c2984f914cf2bb7dfdd57926face762ca0922d4c8453541bc2d8",26410764};
static const LegacyFile kBatteryMuiIt={L"stobject.dll.mui",L"","301228cba92a98eba5805539b426bae808d955f036c1d79891dbae270c3aa6ec",9728};
static const LegacyFile kBatteryMuiEn={L"stobject.dll.mui",L"","a7efe4ef8abb1fe557ac22bb4733772882da440bbb5b6894d0c99463012c3196",7680};
static bool IsBatteryCab(const LegacyFile& s) {return &s==&kBatteryCabIt || &s==&kBatteryCabEn;}
static const LegacyFile& SelectedBatteryMui() {return ItalianUI()?kBatteryMuiIt:kBatteryMuiEn;}
static std::wstring BatteryModuleDirectory(const std::wstring& dir) {return dir+L"\\battery1511";}
static std::wstring BatteryMuiDirectory(const std::wstring& dir) {return BatteryModuleDirectory(dir)+(ItalianUI()?L"\\it-IT":L"\\en-US");}
struct HttpHandle { HINTERNET h=nullptr;
     explicit HttpHandle(HINTERNET x=nullptr):h(x){} ~HttpHandle(){if(h)WinHttpCloseHandle(h);
    } HttpHandle(const HttpHandle&)=delete;
     };
struct BCryptAlg { BCRYPT_ALG_HANDLE h=nullptr; ~BCryptAlg(){if(h)BCryptCloseAlgorithmProvider(h,0);} };
struct BCryptHash { BCRYPT_HASH_HANDLE h=nullptr; ~BCryptHash(){if(h)BCryptDestroyHash(h);} };
struct ResourceModule {HMODULE h=nullptr; ~ResourceModule(){if(h)FreeLibrary(h);} };
static bool DigestMatches(const std::vector<BYTE>& bytes,const LegacyFile& spec) {
    if(bytes.size()!=spec.size || !spec.hash) return false;
    BCryptAlg alg; if(BCryptOpenAlgorithmProvider(&alg.h,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0) return false;
    DWORD size=0,got=0; if(BCryptGetProperty(alg.h,BCRYPT_OBJECT_LENGTH,(PUCHAR)&size,sizeof(size),&got,0)<0 || !size || size>1024*1024) return false;
    std::vector<BYTE> object(size); BCryptHash hash;
    if(BCryptCreateHash(alg.h,&hash.h,object.data(),size,nullptr,0,0)<0 ||
       BCryptHashData(hash.h,(PUCHAR)bytes.data(),(ULONG)bytes.size(),0)<0) return false;
    BYTE result[32]{};if(BCryptFinishHash(hash.h,result,sizeof(result),0)<0) return false;
    static const char hex[]="0123456789abcdef";
    for(size_t i=0;i<32;i++) if(spec.hash[i*2]!=hex[result[i]>>4] || spec.hash[i*2+1]!=hex[result[i]&15]) return false;
    return spec.hash[64]==0;
}
static bool ReadVerifiedHandle(HANDLE file,const LegacyFile& spec) {
    LARGE_INTEGER size{};if(!GetFileSizeEx(file,&size) || size.QuadPart!=spec.size || size.QuadPart>(IsBatteryCab(spec)?40*1024*1024:4*1024*1024)) return false;
    LARGE_INTEGER zero{};if(!SetFilePointerEx(file,zero,nullptr,FILE_BEGIN)) return false;
    std::vector<BYTE> bytes((size_t)size.QuadPart);DWORD off=0;
    while(off<bytes.size()) {DWORD got=0;if(!ReadFile(file,bytes.data()+off,(DWORD)bytes.size()-off,&got,nullptr)||!got)return false;off+=got;}
    return DigestMatches(bytes,spec);
}
static std::wstring ResourcePath(const std::wstring& dir,const LegacyFile& spec) {return dir+(dir.empty() || dir.back()!=L'\\'?L"\\":L"")+spec.file;}
static bool VerifiedOnDisk(const std::wstring& path,const LegacyFile& spec) {
    ScopedHandle file(CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    return file.h!=INVALID_HANDLE_VALUE && ReadVerifiedHandle(file.h,spec);
}
static bool FetchResource(const std::wstring& dir,const LegacyFile& spec,HANDLE stop) {
    if(WaitForSingleObject(stop,0)==WAIT_OBJECT_0) return false;
    std::wstring path=ResourcePath(dir,spec); if(VerifiedOnDisk(path,spec)) return true;
    HttpHandle session(WinHttpOpen(L"Taskbar81/0.24",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0));
    if(!session.h) return false;
    if(!WinHttpSetTimeouts(session.h,4000,5000,5000,5000)) return false;
    std::vector<BYTE> bytes;bytes.reserve(spec.size);ULONGLONG start=GetTickCount64();
    std::wstring host=IsBatteryCab(spec)?L"catalog.s.download.windowsupdate.com":L"msdl.microsoft.com", object=spec.urlPath; bool complete=false;
    // msdl redirects signed downloads to Microsoft Azure blob storage. Follow
    // only HTTPS:443 and only the symbol-server or blob.core.windows.net host.
    // The pinned digest is still checked independently before any file is used.
    for(int redirect=0;redirect<4 && !complete && WaitForSingleObject(stop,0)!=WAIT_OBJECT_0;redirect++) {
        HttpHandle connection(WinHttpConnect(session.h,host.c_str(),INTERNET_DEFAULT_HTTPS_PORT,0)); if(!connection.h)return false;
        HttpHandle request(WinHttpOpenRequest(connection.h,L"GET",object.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE)); if(!request.h)return false;
        DWORD disable=WINHTTP_DISABLE_REDIRECTS;if(!WinHttpSetOption(request.h,WINHTTP_OPTION_DISABLE_FEATURE,&disable,sizeof(disable)))return false;
        if(!WinHttpSendRequest(request.h,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0) || !WinHttpReceiveResponse(request.h,nullptr))return false;
        DWORD status=0,len=sizeof(status);if(!WinHttpQueryHeaders(request.h,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&status,&len,WINHTTP_NO_HEADER_INDEX))return false;
        if(status==301 || status==302 || status==303 || status==307 || status==308) {
            DWORD needed=0;WinHttpQueryHeaders(request.h,WINHTTP_QUERY_LOCATION,WINHTTP_HEADER_NAME_BY_INDEX,nullptr,&needed,WINHTTP_NO_HEADER_INDEX);
            if(!needed || needed>32768 || needed%sizeof(wchar_t))return false;
            std::vector<wchar_t> location(needed/sizeof(wchar_t)+1,0);
            if(!WinHttpQueryHeaders(request.h,WINHTTP_QUERY_LOCATION,WINHTTP_HEADER_NAME_BY_INDEX,location.data(),&needed,WINHTTP_NO_HEADER_INDEX))return false;
            URL_COMPONENTS parts{};parts.dwStructSize=sizeof(parts);parts.dwHostNameLength=(DWORD)-1;parts.dwUrlPathLength=(DWORD)-1;
            parts.dwExtraInfoLength=(DWORD)-1;parts.dwUserNameLength=(DWORD)-1;parts.dwPasswordLength=(DWORD)-1;
            if(!WinHttpCrackUrl(location.data(),0,0,&parts) || parts.nScheme!=INTERNET_SCHEME_HTTPS || parts.nPort!=INTERNET_DEFAULT_HTTPS_PORT || parts.dwUserNameLength || parts.dwPasswordLength || !parts.dwHostNameLength || !parts.dwUrlPathLength)return false;
            std::wstring next(parts.lpszHostName,parts.dwHostNameLength);
            std::transform(next.begin(),next.end(),next.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
            const std::wstring suffix=L".blob.core.windows.net";
            if(next!=L"msdl.microsoft.com" && !(IsBatteryCab(spec) && next==L"catalog.s.download.windowsupdate.com") && !(next.size()>suffix.size() && next.compare(next.size()-suffix.size(),suffix.size(),suffix)==0))return false;
            host=std::move(next);object.assign(parts.lpszUrlPath,parts.dwUrlPathLength);
            if(parts.dwExtraInfoLength) object.append(parts.lpszExtraInfo,parts.dwExtraInfoLength);
            if(object.empty() || object[0]!=L'/' || object.find(L'#')!=std::wstring::npos)return false;
            continue;
        }
        if(status!=200) {Wh_Log(L"[T81-DIAG] resource server status=%lu for %ls",status,spec.file);return false;}
        while(bytes.size()<=spec.size && WaitForSingleObject(stop,0)!=WAIT_OBJECT_0 && GetTickCount64()-start<(IsBatteryCab(spec)?120000:30000)) {
            BYTE buf[16384];DWORD got=0;if(!WinHttpReadData(request.h,buf,sizeof(buf),&got))return false;
            if(!got) {complete=true;break;}if(bytes.size()+got>spec.size)return false;bytes.insert(bytes.end(),buf,buf+got);
        }
    }
    if(!complete)return false;
    if(WaitForSingleObject(stop,0)==WAIT_OBJECT_0 || !DigestMatches(bytes,spec)) {Wh_Log(L"[T81-DIAG] resource download hash/size failed: %ls",spec.file);return false;}
    // Write a private temporary file, deny modification and rename by handle.
    std::wstring temp=path+L".tmp-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetCurrentThreadId());
    ScopedHandle out(CreateFileW(temp.c_str(),GENERIC_READ|GENERIC_WRITE|DELETE,FILE_SHARE_READ,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
    if(out.h==INVALID_HANDLE_VALUE) return false;
    bool committed=false; DWORD written=0;
    bool wrote=WriteFile(out.h,bytes.data(),(DWORD)bytes.size(),&written,nullptr)!=FALSE && written==bytes.size();
    bool flushed=wrote && FlushFileBuffers(out.h)!=FALSE; bool checked=flushed && ReadVerifiedHandle(out.h,spec);
    if(checked) {
        size_t nameBytes=path.size()*sizeof(wchar_t);std::vector<BYTE> renameBuffer(offsetof(FILE_RENAME_INFO,FileName)+nameBytes+sizeof(wchar_t),0);
        auto* rename=(FILE_RENAME_INFO*)renameBuffer.data();
            rename->ReplaceIfExists=TRUE;
            rename->RootDirectory=nullptr;
            rename->FileNameLength=(DWORD)nameBytes;
        memcpy(rename->FileName,path.data(),nameBytes);
        committed=SetFileInformationByHandle(out.h,FileRenameInfo,rename,(DWORD)renameBuffer.size())!=FALSE;
    }
    if(!committed) Wh_Log(L"[T81-DIAG] cache commit failure file=%ls wrote=%d flushed=%d rechecked=%d error=%lu",spec.file,wrote?1:0,flushed?1:0,checked?1:0,GetLastError());
    if(!committed) {FILE_DISPOSITION_INFO remove{};remove.DeleteFile=TRUE;SetFileInformationByHandle(out.h,FileDispositionInfo,&remove,sizeof(remove));}
    Wh_Log(L"[T81-DIAG] Microsoft resource %ls downloaded verified=%d",spec.file,committed?1:0);
    return committed;
}
static bool FetchResourceWithRetry(const std::wstring& directory,const LegacyFile& file,HANDLE stop) {
    for(int attempt=0;attempt<3;attempt++) {
        if(WaitForSingleObject(stop,0)==WAIT_OBJECT_0)return false;
        if(FetchResource(directory,file,stop))return true;
        if(attempt<2 && WaitForSingleObject(stop,1000)==WAIT_OBJECT_0)return false;
    }
    return false;
}
// Extract exactly one hard-coded CAB entry. Cabinet paths never become target
// paths, so a malicious name cannot traverse directories. Verify the CAB before
// SetupAPI, and verify the MUI again while denying writes before atomic rename.
struct MuiCabContext {const wchar_t* entry;std::wstring target;DWORD size;HANDLE stop;bool selected=false;};
static UINT CALLBACK BatteryCabCallback(PVOID context,UINT notification,UINT_PTR param1,UINT_PTR) {
    auto* c=static_cast<MuiCabContext*>(context);
    if(WaitForSingleObject(c->stop,0)==WAIT_OBJECT_0)return FILEOP_ABORT;
    if(notification==SPFILENOTIFY_FILEINCABINET) {
        auto* info=reinterpret_cast<FILE_IN_CABINET_INFO_W*>(param1);
        if(!info || !info->NameInCabinet)return FILEOP_SKIP;
        std::wstring name(info->NameInCabinet);std::replace(name.begin(),name.end(),L'/',L'\\');
        if(_wcsicmp(name.c_str(),c->entry)!=0 || info->FileSize!=c->size || c->selected)return FILEOP_SKIP;
        if(c->target.size()>=MAX_PATH)return FILEOP_ABORT;
        wcscpy(info->FullTargetName,c->target.c_str());c->selected=true;return FILEOP_DOIT;
    }
    if(notification==SPFILENOTIFY_NEEDNEWCABINET)return FILEOP_ABORT;
    if(notification==SPFILENOTIFY_FILEEXTRACTED) {
        auto* info=reinterpret_cast<FILEPATHS_W*>(param1);
        if(info && info->Win32Error!=NO_ERROR)return FILEOP_ABORT;
    }
    return NO_ERROR;
}
static bool PrepareBatteryMui(const std::wstring& dir,HANDLE stop) {
    const bool it=ItalianUI();const LegacyFile& cab=it?kBatteryCabIt:kBatteryCabEn;
    const LegacyFile& mui=SelectedBatteryMui();std::wstring muiDir=BatteryMuiDirectory(dir);
    std::wstring moduleDir=BatteryModuleDirectory(dir);
    if(!CreateDirectoryW(moduleDir.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS)return false;
    if(!CreateDirectoryW(muiDir.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS)return false;
    std::wstring finalPath=ResourcePath(muiDir,mui);
    if(VerifiedOnDisk(finalPath,mui))return true;
    if(!FetchResourceWithRetry(dir,cab,stop))return false;
    std::wstring cabPath=ResourcePath(dir,cab);
    ScopedHandle lock(CreateFileW(cabPath.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    if(lock.h==INVALID_HANDLE_VALUE || !ReadVerifiedHandle(lock.h,cab))return false;
    std::wstring temp=muiDir+L"\\battery-mui.tmp-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetCurrentThreadId());
    {
        ScopedHandle reserve(CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
        if(reserve.h==INVALID_HANDLE_VALUE)return false;
    }
    MuiCabContext c{it?L"amd64_microsoft-windows-stobject.resources_31bf3856ad364e35_10.0.10586.0_it-it_ae7cfb07bf1cbc6f\\stobject.dll.mui":
        L"amd64_microsoft-windows-stobject.resources_31bf3856ad364e35_10.0.10586.0_en-us_21d231ddf4f1ceea\\stobject.dll.mui",temp,mui.size,stop};
    bool extracted=SetupIterateCabinetW(cabPath.c_str(),0,BatteryCabCallback,&c)!=FALSE && c.selected;
    bool committed=false;
    if(extracted && WaitForSingleObject(stop,0)!=WAIT_OBJECT_0) {
        ScopedHandle out(CreateFileW(temp.c_str(),GENERIC_READ|DELETE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
        if(out.h!=INVALID_HANDLE_VALUE && ReadVerifiedHandle(out.h,mui)) {
            size_t nameBytes=finalPath.size()*sizeof(wchar_t);
            std::vector<BYTE> buf(offsetof(FILE_RENAME_INFO,FileName)+nameBytes+sizeof(wchar_t),0);
            auto* rename=(FILE_RENAME_INFO*)buf.data();rename->ReplaceIfExists=TRUE;rename->FileNameLength=(DWORD)nameBytes;
            memcpy(rename->FileName,finalPath.data(),nameBytes);
            committed=SetFileInformationByHandle(out.h,FileRenameInfo,rename,(DWORD)buf.size())!=FALSE;
        }
    }
    if(!committed)DeleteFileW(temp.c_str());
    // Close the read lock before deleting the now-unneeded large CAB.
    if(lock.h!=INVALID_HANDLE_VALUE){CloseHandle(lock.h);lock.h=INVALID_HANDLE_VALUE;}
    if(committed)DeleteFileW(cabPath.c_str());
    Wh_Log(L"[T81-DIAG] battery 1511 MUI language=%ls extracted=%d verified=%d",it?L"it-IT":L"en-US",extracted?1:0,committed?1:0);
    return committed;
}
static std::atomic<int> wifiQuality{-1};
static int ReadWifiQuality() {
    HANDLE raw=nullptr;DWORD version=0;if(WlanOpenHandle(2,nullptr,&version,&raw)!=ERROR_SUCCESS || !raw)return -1;
    struct WlanOwner {HANDLE h;~WlanOwner(){WlanCloseHandle(h,nullptr);}} owner{raw};
    PWLAN_INTERFACE_INFO_LIST list=nullptr;if(WlanEnumInterfaces(raw,nullptr,&list)!=ERROR_SUCCESS || !list)return -1;
    struct WlanMemory {void* p;~WlanMemory(){if(p)WlanFreeMemory(p);}} listOwner{list};
    int quality=-1;
    for(DWORD i=0;i<list->dwNumberOfItems;i++) if(list->InterfaceInfo[i].isState==wlan_interface_state_connected) {
        DWORD size=0; PVOID data=nullptr;WLAN_OPCODE_VALUE_TYPE type{};
        if(WlanQueryInterface(raw,&list->InterfaceInfo[i].InterfaceGuid,wlan_intf_opcode_current_connection,nullptr,&size,&data,&type)==ERROR_SUCCESS && data) {
            WlanMemory dataOwner{data}; if(size>=sizeof(WLAN_CONNECTION_ATTRIBUTES)) quality=std::min(100,(int)((WLAN_CONNECTION_ATTRIBUTES*)data)->wlanAssociationAttributes.wlanSignalQuality);
        }
        if(quality>=0)break;
    }
    return quality;
}
struct ResourceWork {std::wstring directory;HWND bar;HANDLE stop;};
static DWORD WINAPI ResourceWorker(void* parameter) {
    std::unique_ptr<ResourceWork> work((ResourceWork*)parameter);
    try {
        if(!CreateDirectoryW(work->directory.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS) {
            Wh_Log(L"[T81-DIAG] cannot create mod resource directory error=%lu",GetLastError());return 1;
        }
        DWORD attributes=GetFileAttributesW(work->directory.c_str());
        if(attributes==INVALID_FILE_ATTRIBUTES || !(attributes&FILE_ATTRIBUTE_DIRECTORY))return 1;
        for(const auto& file:kLegacyFiles) {
            if(WaitForSingleObject(work->stop,0)==WAIT_OBJECT_0)break;
            if(!FetchResourceWithRetry(work->directory,file,work->stop)) Wh_Log(L"[T81-DIAG] resource unavailable: %ls; using fallback",file.file);
            else if(WaitForSingleObject(work->stop,0)!=WAIT_OBJECT_0) PostMessageW(work->bar,WM_RESOURCES_READY,0,0);
        }
        if(WaitForSingleObject(work->stop,0)!=WAIT_OBJECT_0) PostMessageW(work->bar,WM_RESOURCES_READY,0,0);
        if(useOriginalBatteryFlyout.load() && WaitForSingleObject(work->stop,0)!=WAIT_OBJECT_0) {
            std::wstring batteryDir=BatteryModuleDirectory(work->directory);
            bool made=CreateDirectoryW(batteryDir.c_str(),nullptr) || GetLastError()==ERROR_ALREADY_EXISTS;
            bool ok=made && FetchResourceWithRetry(batteryDir,kBattery1511,work->stop);
            Wh_Log(L"[T81-DIAG] original battery DLL ready=%d; no Windows compatibility guarantee",ok?1:0);
            if(ok && WaitForSingleObject(work->stop,0)!=WAIT_OBJECT_0) {
                bool mui=PrepareBatteryMui(work->directory,work->stop);
                Wh_Log(L"[T81-DIAG] original battery MUI ready=%d; resource resolution is not guaranteed",mui?1:0);
            }
        }
        while(WaitForSingleObject(work->stop,0)!=WAIT_OBJECT_0) {
            int quality=ReadWifiQuality();int before=wifiQuality.exchange(quality);
            if(before!=quality) PostMessageW(work->bar,WM_WIFI_CHANGED,0,0);
            if(WaitForSingleObject(work->stop,5000)==WAIT_OBJECT_0) break;
        }
    } catch(...) {Wh_Log(L"[T81-DIAG] background resources/WLAN C++ exception; vector fallback");}
    return 0;
}
static void StopResourceWorker() {
    if(!s)return;
    if(s->resourceStop) SetEvent(s->resourceStop);
    if(s->resourceThread) {WaitForSingleObject(s->resourceThread,INFINITE);CloseHandle(s->resourceThread);s->resourceThread=nullptr;}
    if(s->resourceStop) {CloseHandle(s->resourceStop);s->resourceStop=nullptr;}
    wifiQuality.store(-1);
}
static void StartResourceWorker() {
    if(!s || !s->bar || s->resourceThread)return;
#ifdef TASKBAR81_TEST
    return; // explicit integration tests call the worker with a controlled directory
#else
    wchar_t storage[32768]={}; if(!Wh_GetModStoragePath(storage,std::size(storage)) || PathIsRelativeW(storage)) {Wh_Log(L"[T81-DIAG] no mod storage; vector fallback");return;}
    s->resourceDir=storage;
    s->resourceStop=CreateEventW(nullptr,TRUE,FALSE,nullptr);if(!s->resourceStop)return;
    auto work=std::make_unique<ResourceWork>();work->directory=s->resourceDir;work->bar=s->bar;work->stop=s->resourceStop;
    s->resourceThread=CreateThread(nullptr,0,ResourceWorker,work.get(),0,nullptr);
    if(s->resourceThread) { [[maybe_unused]] auto* transferred=work.release(); }
    else {CloseHandle(s->resourceStop);s->resourceStop=nullptr;}
#endif
}
static void ClearLegacyImages() {
    if(!s)return;for(auto& image:s->netImages){delete image;image=nullptr;}for(auto& image:s->volumeImages){delete image;image=nullptr;}
    for(auto& frame:s->batteryFrames) { delete frame; frame=nullptr; }
    delete s->batteryStrip;s->batteryStrip=nullptr;s->batteryFrameSize=0;
}
static bool OpenVerifiedResource(const std::wstring& path,const LegacyFile& spec,ResourceModule& mod) {
    if(path.empty() || PathIsRelativeW(path.c_str()) || GetModuleHandleW(path.c_str())) return false;
    ScopedHandle locked(CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    if(locked.h==INVALID_HANDLE_VALUE || !ReadVerifiedHandle(locked.h,spec)) return false;
    mod.h=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE|LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    return mod.h!=nullptr;
}
// Select the exact 32-bpp frame from a verified Microsoft group. Using
// Bitmap(HICON) here destroys intermediate alpha on some systems, turning
// the intended 50%-black outlines solid black. Do not turn an icon handle
// into a GDI+ bitmap; copy the 32-bit resource XOR pixels instead.
static Bitmap* ResourceIcon(HMODULE module,int id,int size) {
    if(!module || size<16 || size>32) return nullptr;
    HRSRC group=FindResourceW(module,MAKEINTRESOURCEW(id),RT_GROUP_ICON);
    DWORD groupBytes=group?SizeofResource(module,group):0;
    HGLOBAL groupHandle=group?LoadResource(module,group):nullptr;
    const BYTE* dir=groupHandle?(const BYTE*)LockResource(groupHandle):nullptr;
    auto word=[](const BYTE* p)->WORD { WORD v;memcpy(&v,p,sizeof(v));return v; };
    if(!dir || groupBytes<6 || word(dir)!=0 || word(dir+2)!=1) return nullptr;
    WORD count=word(dir+4); if(!count || (size_t)count>(groupBytes-6)/14) return nullptr;
    WORD imageId=0;
    for(WORD i=0;i<count;i++) {
        const BYTE* entry=dir+6+(size_t)i*14;
        int width=entry[0]?entry[0]:256, height=entry[1]?entry[1]:256;
        if(width==size && height==size && word(entry+6)==32) {imageId=word(entry+12);break;}
    }
    if(!imageId) return nullptr;
    HRSRC image=FindResourceW(module,MAKEINTRESOURCEW(imageId),RT_ICON);
    DWORD imageBytes=image?SizeofResource(module,image):0;
    HGLOBAL imageHandle=image?LoadResource(module,image):nullptr;
    const BYTE* raw=imageHandle?(const BYTE*)LockResource(imageHandle):nullptr;
    if(!raw || imageBytes<sizeof(BITMAPINFOHEADER)) return nullptr;
    BITMAPINFOHEADER header{};memcpy(&header,raw,sizeof(header));
    const size_t rowBytes=(size_t)size*4, required=sizeof(header)+rowBytes*(size_t)size;
    // These pinned resources use bottom-up BI_RGB 32-bit icon DIBs. The
    // doubled height includes the AND mask after the XOR image. Reject any
    // unexpected format rather than guessing, and use the vector fallback.
    if(header.biSize!=sizeof(header) || header.biWidth!=size || header.biHeight!=2*size ||
       header.biPlanes!=1 || header.biBitCount!=32 || header.biCompression!=BI_RGB ||
       header.biClrUsed!=0 || imageBytes<required) return nullptr;
    bool anyAlpha=false;for(size_t i=sizeof(header)+3;i<required;i+=4) if(raw[i]) {anyAlpha=true;break;}
    if(!anyAlpha) return nullptr;
    auto bitmap=std::make_unique<Bitmap>(size,size,PixelFormat32bppARGB);
    if(bitmap->GetLastStatus()!=Ok) return nullptr;
    BitmapData data{};Rect bounds(0,0,size,size);
    if(bitmap->LockBits(&bounds,ImageLockModeWrite,PixelFormat32bppARGB,&data)!=Ok) return nullptr;
    for(int y=0;y<size;y++) memcpy((BYTE*)data.Scan0+(ptrdiff_t)y*data.Stride,
        raw+sizeof(header)+(size_t)(size-1-y)*rowBytes,rowBytes);
    if(bitmap->UnlockBits(&data)!=Ok) return nullptr;
    return bitmap.release();
}
static Bitmap* ResourceBattery(HMODULE module,int resource,int frame) {
    HRSRC resourceHandle=FindResourceW(module,MAKEINTRESOURCEW(resource),RT_BITMAP); if(!resourceHandle)return nullptr;
    DWORD bytes=SizeofResource(module,resourceHandle);
        HGLOBAL loaded=LoadResource(module,resourceHandle);
        const BYTE* raw=loaded?(const BYTE*)LockResource(loaded):nullptr;
    if(!raw || bytes<sizeof(BITMAPINFOHEADER))return nullptr;
    BITMAPINFOHEADER header{};memcpy(&header,raw,sizeof(header));int width=frame*43,height=frame;
    if(header.biSize!=40 || header.biWidth!=width || std::abs(header.biHeight)!=height || header.biPlanes!=1 || header.biBitCount!=32 || header.biCompression!=BI_RGB ||
       (size_t)bytes<40+(size_t)width*height*4) return nullptr;
    // Centre the opaque artwork within each frame, not the transparent 16/20/24/32px cell.
    // Discharging frames have several transparent pixels on the left; charging
    // frames contain a plug and naturally occupy the full width.
    if(s) for(int i=0;i<43;i++) {
        int left=frame,right=-1;
        for(int y=0;y<height;y++) for(int x=0;x<frame;x++) {
            size_t offset=40+((size_t)y*width+(size_t)i*frame+x)*4;
            if(raw[offset+3]>8) {left=std::min(left,x);right=std::max(right,x);}
        }
        s->batteryOffset[i]=right>=left?(frame-left-right-1)/2.0f:0.0f;
    }
    auto bmp=std::make_unique<Bitmap>(width,height,PixelFormat32bppARGB);if(bmp->GetLastStatus()!=Ok)return nullptr;
    BitmapData data{};Rect rect(0,0,width,height);if(bmp->LockBits(&rect,ImageLockModeWrite,PixelFormat32bppARGB,&data)!=Ok)return nullptr;
    for(int y=0;y<height;y++) {int srcY=header.biHeight>0?height-1-y:y;memcpy((BYTE*)data.Scan0+y*data.Stride,raw+40+(size_t)srcY*width*4,(size_t)width*4);}
    bmp->UnlockBits(&data);return bmp.release();
}
static void PublishCniSystemIcon(int hit);
static void LoadLegacyImages() {
    if(!s || s->resourceDir.empty())return;
    ClearLegacyImages();int wanted=std::max(16,Px(16));int dimension=wanted<=16?16:wanted<=20?20:wanted<=24?24:32;
    const int artwork=networkUse100PercentArtwork.load()?16:dimension; // old setting key preserves upgrades
    ResourceModule network,volume,battery;
    if(OpenVerifiedResource(ResourcePath(s->resourceDir,kLegacyFiles[0]),kLegacyFiles[0],network)) {
        int ids[]={3020,3048,3021,3022,3023,3024,3025,3026};for(int i=0;i<8;i++)s->netImages[i]=ResourceIcon(network.h,ids[i],artwork);
    }
    if(OpenVerifiedResource(ResourcePath(s->resourceDir,kLegacyFiles[1]),kLegacyFiles[1],volume))
        for(int i=0;i<6;i++)s->volumeImages[i]=ResourceIcon(volume.h,120+i,artwork);
    if(OpenVerifiedResource(ResourcePath(s->resourceDir,kLegacyFiles[2]),kLegacyFiles[2],battery)) {
        int resource=artwork==16?353:artwork==20?354:artwork==24?355:356;
        s->batteryStrip=ResourceBattery(battery.h,resource,artwork);
        if(s->batteryStrip) {
            s->batteryFrameSize=artwork;
            // Isolate frames before interpolation: the strip's neighbouring
            // charging/discharging images must never bleed into this frame.
            for(int i=0;i<43;i++) {
                Bitmap* frame=s->batteryStrip->Clone(i*artwork,0,artwork,artwork,PixelFormat32bppARGB);
                if(frame && frame->GetLastStatus()==Ok) s->batteryFrames[i]=frame;
                else delete frame;
            }
        }
    }
    Wh_Log(L"[T81-DIAG] 8.1 data resources network=%d volume=%d battery=%d dpiSize=%d",s->netImages[0]?1:0,s->volumeImages[0]?1:0,s->batteryStrip?1:0,dimension);
    auto partialAlpha=[](Bitmap* image) {
        int count=0;
        if(image) {
            for(UINT y=0;y<image->GetHeight();y++) {
                for(UINT x=0;x<image->GetWidth();x++) {
                    Color pixel;
                    if(image->GetPixel(x,y,&pixel)==Ok && pixel.GetAlpha()>0 && pixel.GetAlpha()<255) ++count;
                }
            }
        }
        return count;
    };
    Wh_Log(L"[T81-DIAG] legacy-alpha-partial network=%d volume=%d",partialAlpha(s->netImages[4]),partialAlpha(s->volumeImages[3]));
    int readyFrames=0;for(auto frame:s->batteryFrames) if(frame) ++readyFrames;
    Wh_Log(L"[T81-DIAG] legacy-image-ARGB dpi=%d network=%ux%u volume=%ux%u battery-frame=%d isolated=%d/43 draw-cell=%d",
        Dpi(),s->netImages[0]?s->netImages[0]->GetWidth():0,s->netImages[0]?s->netImages[0]->GetHeight():0,
        s->volumeImages[0]?s->volumeImages[0]->GetWidth():0,s->volumeImages[0]?s->volumeImages[0]->GetHeight():0,
        s->batteryFrameSize,readyFrames,Px(16));
    InvalidateRect(s->bar,nullptr,FALSE);if(s->overflowPanel)InvalidateRect(s->overflowPanel,nullptr,FALSE);
    if(s->privateActive) {
        PublishCniSystemIcon(HIT_NETWORK);
        PublishCniSystemIcon(HIT_VOLUME);
        if(s->hasBattery) PublishCniSystemIcon(HIT_BATTERY);
        Wh_Log(L"[T81-CNI] taskbar DLL artwork republished");
    }
}
// Destination dimensions are derived from the source, not assumed square.
// The selected Microsoft 125% images are 20x20; UnitPixel makes source
// bounds independent of the DPI metadata attached to a GDI+ bitmap.
static bool DrawLegacyImage(Graphics& g,Image* image,float cx,float cy,float u) {
    if(!image || u<=0 || !image->GetWidth() || !image->GetHeight()) return false;
    float sw=(float)image->GetWidth(), sh=(float)image->GetHeight(), limit=16*u;
    float scale=std::min(limit/sw,limit/sh), dw=sw*scale, dh=sh*scale;
    InterpolationMode old=g.GetInterpolationMode();
    g.SetInterpolationMode(std::abs(dw-sw)<0.01f && std::abs(dh-sh)<0.01f
        ? InterpolationModeNearestNeighbor : InterpolationModeHighQualityBicubic);
    Status result=g.DrawImage(image,RectF(std::round(cx-dw/2),std::round(cy-dh/2),dw,dh),
                              0.f,0.f,sw,sh,UnitPixel);
    g.SetInterpolationMode(old);return result==Ok;
}
static bool DrawLegacyBattery(Graphics& g,int frame,float cx,float cy,float u) {
    if(!s || frame<0 || frame>=43 || u<=0 || !s->batteryFrames[frame] || s->batteryFrameSize<=0) return false;
    float shift=s->batteryOffset[frame]*(16*u)/s->batteryFrameSize;
    return DrawLegacyImage(g,s->batteryFrames[frame],cx+shift,cy,u);
}

// Hover tile PNG for the notification overflow.
static const char kW7OverflowTile[] = R"W7B(
iVBORw0KGgoAAAANSUhEUgAAAB8AAAAbCAYAAACEP1QvAAAACXBIWXMAAA7EAAAOxAGVKw4bAAACm0lEQVRIicXWv3LbRhDH8e/eHQCGpGRzwkSKZ5wu
qVLmXfKOeRaXqqwZF/ZMosiWqX8miTvc/lIIyozHYuIK2QrFAh8sbm9xAJ2kGRPG6HUJiECYEgcS4GG8mLTy0UsJaCR9M6U8ekpAl3OeT4mPXk1AZ2aT
4qOXA9DVWifFR697xCdtuNF7wN19Unz0ugC07t5NjHdAG4DWzNKU+Oi1AYg550kn3OjFwMN4/T8iBkC11jqlOnoKQHX3MiU+ejUAxd3zxHgGSgAKsJ8S
H72SgP3ES06tdQ/sE7BPKfnX3ijJgHgu/bNLfjKrQDUzfc0zUkpbICdgl3P+z9IlxXfv1L7Z0JZE43saM0xC5zMNcbD89q3nly8t28PLHIyc8+4R7yX9
a/KZ1J5fadZ0Nh8y8+LqEG2AIJlyVm6r+n7G9s2G7Zm0+8XsYBNL2gHDIz4cSnzt3nU3zO+CHZWiowJLCgsXnRsxSK5C74mtxL3Q7fEN8bX7p59D6A/g
e8bDRF9KsaeSXklN3DC7lZYqPB/EanCeuXgmmOEkC7gZe1VuY+BaxeJdFUtMryT/1eyLGVJK2TEeo/pa6xe4pHBxofY6sQgDx71YDZX14Kyr20po4SJZ
xaOxHSKbJJqKrA14P6jOLq1Iqmb2WUPXWntAT1YMD+vMjZbqWVVnXZzTMnCaKycu1o4d42oIDGbcR+MqGpdd5M8mcNE1vA8z+xiWujv0+Q/+Sld/kDbJ
2sE188oyDxwPslV1fVfF6SA9F3ShkiPcEYkYQwlsA9xXcW+92qNPJEn5qW34WeW/n6mdOws6Vilp3Yg1cCLxwuGFix9cnBh8v2j5tgnMerd832tjcBMC
VwHem/GX4AJx6caHwfkYnOsgPtwuuf7tx7AD+BuM36qzzy+RlgAAAABJRU5ErkJggg==
)W7B";
// Embedded 54 x 162 sprite with three 54 x 54 state frames.
// Asset reference: GraphicalResourceBundle, startwin8beta8148orb.
static const char kWin7StartPngBase64[] =
    "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"
    ;
static const char kBetaStartPngBase64[] =
    "iVBORw0KGgoAAAANSUhEUgAAADYAAACiCAMAAAA9bM66AAAAIGNIUk0AAHomAACAhAAA+gAAAIDoAAB1MAAA6mAAADqYAAAXcJy6UTwAAAFZUExURQAAADMz"
    "M3kqLaYkKb4hJ20rLtUeJe0cJEowMZolKo8nK2WFOVpzOD5FNLIjKFUuL4e8Po3GP4GzPVVqNzg8M09hNmqOOnyqPeEdJENONF98OGEtL8ogJ0Q7Mz4xMiNZ"
    "bRlwkBCHtA2PwClJVYUpLACu7yBheRN/qGVSNnahPBZ3nNaYPcmQPExDNVhKNQae1/ywQJdxObCAOuKfPS1CSn5iOB1phbyIOz87NO+nPjE5PCVUZQOm4yZR"
    "YaN4OUZGRlY9Pnc1N48wM1A/QKwqL84kKkZERGw3OWQ5O1BaQktRQkVGRYIzNW+QQmR9QUlOQ0ZGRUhLQ1NfQmmFQl90Qr0nLE1VQkxBQUZHRJ0tMUZFRTxM"
    "USNwjAWn4y1gc0lHRBKNvHpiQENGRlZOQjVUYNWaQR55m6J6QEVFRmZXQT5KTguazylnfj9JTEFHSbGDQMKNQG9cQJNxQGedP7EAAAABdFJOUwBA5thmAAAA"
    "CXBIWXMAAAsSAAALEgHS3X78AAAAB3RJTUUH6gkUBzQnsg9g5AAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAyNi0wOS0yMFQwNzo1MjozOSswMDowMGfUCX8AAAAl"
    "dEVYdGRhdGU6bW9kaWZ5ADIwMjYtMDktMjBUMDc6NTI6MzkrMDA6MDAWibHDAAAAKHRFWHRkYXRlOnRpbWVzdGFtcAAyMDI2LTA5LTIwVDA3OjUyOjM5KzAw"
    "OjAwQZyQHAAAAk5JREFUaN7t1u1TElEUBnCvsJAUhkIQUCKLqAEqoGtbmaviZpqbhGa1tJoRFNmr//+HVmrvUbxxuE5jOtznAzvszm/OzoG58/T1ifRSyLki"
    "WJes3+V2uyROJnm8rXgkHnbNSzPAwfqB+a7fcO76MTYI7GZgaDgYIqFb4UAEY7ep8kSHaGIIi8MwVwxYFGEDwKQIVQFsJXeA3YVhYYSNDPqkuOQ7xu4gsCA2"
    "LTGalOXRVNztHQsDCyEsmf4dOTVmr99JhCAsTTPuD/xl/WdZAtjEZPSes5AQwjLAEtncVDRmJ+ifJgibATaVz+ezBTvF2SLC5kAllTyNgrB5YPP3gakIk4HN"
    "PaAqSzqzh61frKVmJmFYAWGPFiYe25dUZiGdWQQ2jTBYjZzIUjVLumWEqDCsyMGI4qykoPIwoiqLuVxBUVnPLsNh3ttMRETk4nIlDoWry0QzPx3RzNsjmnnb"
    "d9HMRTPnYp0i2D9jIiIiFxftXBGsS7a0vLKyXOJkpVX9uBbqqyUe9kR3+qS+xsGWoIc+Xd9w7j7D2ObJZm483ypr5ReV7R2MvWxr5oZh2J+7CHuls5q58Rph"
    "a8xmvo2t5A2zmVcQZm4ym/kWNq3KauZGGWFvLVYz39EQZjGb+S7CqharmVfKCNtjNvN9DWHvmM38AGHvLWYzryHsA7OZ102ENZjN/KPWmX2y0pbVsKy2Zt5E"
    "2OfDL1/ty7e9w5PNvL6PMFhNowrN/LvWLbP/0nXKDjiYVvvx5xWbJg/TzNrPo6NmzWQ9uwyHeW8zkf+bX0AfR/+tNH6NAAAAAElFTkSuQmCC";
// Embedded textures; retain streams for the lifetime of their GDI+ images.
// Asset reference: GraphicalResourceBundle.
static const char* const kBetaTextureBase64[] = {
    // Desktop overlay with alpha for the current accent.
    "iVBORw0KGgoAAAANSUhEUgAAAA8AAAAoCAYAAAAhf6DEAAACIUlEQVR4nI2UXY7bMAyEP/onSXuLAgX62gvsGdq79CT7sofse3Y3sS32wRyHFpSkAgQ5ioYz"
    "HEo0d//JbXSAVVN7pBXg62Bmv9z9L/vRAw4MwAJMwCX2LsBkZt8N+Obub8Brxd5V31oBfpvZnyGizxHdKuA98AXoBSYC6MAS0hWgVEE/gcMQhzJzBtQK9P8F"
    "GAZgDOB8h6Ul/SqwIolZJRkeGPcBjANwiEi1YXI+y9f3O2AyTMyZYYm1r4LJsA38mdzW7GkzW5B1yvkczMq3Va6c0jvBOoYBcyNPbxhWQmkR80fDsHtgVYcM"
    "FnProvSVYVMNvjZAzt6ozFx0r89h0JIO9cm0vgLPNbjOuXB717lclsEFuJqZu/sSwVSibFieKGdY6zaaWYlgi7s7t27iaZZIZxbzmfWOE7+LmU3uPif2nPOO"
    "+RrgHBkzM2B29+y+iySDx+TuroOa2RJ+KAjAou44B1juCqjRmVkXKpZQvDFPwJFbbTNYpVEms7ubwCrPIcmeUm75iU6xjnJ7k5bcVnlqo/r8v3o2isbtdjn7"
    "oTQ6qdTBsXEoG1aPE1AEPjw42Ar0ReCF1WnJzFeRtJfHEZjEfHzCVO9vspeQ8UxqzVxUhtODg60gO8Nq2c8CjaRSnZ7IrMeYmSW7axysHwlEN82l+h+5G9jd"
    "XXf44O4vrHe2Z700eqJqz+prW5M04EdE0+sqrH15jrXE98z6nlcpZsM/YmzfiAUa7ZgAAAAASUVORK5CYII=",
    // Windows 7
    "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",
    // win7_activenormal
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAoCAYAAACiu5n/AAAACXBIWXMAAA7EAAAOxAGVKw4bAAAELElEQVRogeWav27sRBSHvzM2iYRACCTokRAFFSUFHR09"
    "b5CaDqWOFOURkgegokDQ8w7AA1Be4MK95Cr/s7u2D8XMcY4n442zJLrZ5SdZMx6Pd/zNOXPm2FoB3gS+Ab5I9YeQugOgy9o0a+sKZV5f1nbX9SvgF+D7Gvj6"
    "4ODgo93d3U9DCG+vTKiq/tTBlurL2lRENP3eaJ+Ra8Xztm3P9/f3/9nb2/tSgJ+apvkshPC8cCNZfcq1vK23XAJpfRvQFq717dy2/qAuIp2bHLuWg7/XNM2H"
    "W1tb39bAu1VVvaWqzQTA+05C50pV1dzt+odMsLlLUgBXEWkNMpWDcWx8EVFipxd1XX8CbNXuIdsV4AZ9bIA0SGnNdjZOsujYusstPIB3kH4ZdG5cm2DJxl+U"
    "gHGdisBu5gbt+bkb6JYVRmCLgSu5bel6v97dBOcu7etNDjwGNIAYay+UY+7WZmXJwn09m5zBBJSsfRewNc6B4DjvWqNT17CHbl1ZBF5mTevjIL37+/Hy57Br"
    "Te0eZg5Ud0D1dbd1LINeBlvcM701XQRugSYdVrffnSoF2jrdDDADKhEJqioZ0C2oEdi8zbvzmNXy4GSg16o6BxYissieZRV1QOeB50BQ1QoQF+HGYKZE7pJL"
    "l4KURe9L4FJVZ+4e/jtr/1wROM3eHBCgTmV1B9QU8Dwg+XMFFsAFcEZM/x5TChGuSSfXIlKlBCQQgWUJTF7e2rqypMAyqlZVz4ETIuyDmG+CeuBFOpkndw4M"
    "gQMFwLG92MpC4jETkZeq+or7B5wHU0105Q64JoGmwOXhvaWnBC2gTwrOgJfAyQOtxVU1sHCHW8MpYL0BSArXOTQstzLAK1V9zuOvzftIvIVnRDfuUtkAVYI1"
    "a5uUcoKiInKsqn8RPeapKQyAU9AywIoYaEJ2mPJAdQo8U9WnCGoKvUu7zV1cemfbk5UG7N37CnhG3FqesgRn4TZtR22ysiUCIZXe2jYBLfAH8OI1PPwqEqDq"
    "gUWkUdVguWyK1ObGXQpetsZPgd+J3rEu6oEXxKS9AUICJYFX5t4JvgX+JAKvmwbA9nkFF3ntVbEiRt9LVf2bm9x73TS0MMM8N7hPMEpMAU9SXcq/9+QVyCw8"
    "eDtyUfoMuErJyLrCQgSu7eXBfynw4KfcZGDrrt6lDdhbUIhubFvSJqh3aQ8MQ9hQvnctJSSXtpdzexWcpQ6bYllTb2H77GJJhRLfojZN/Rq27agiRuxNs6wp"
    "kHJpuInKmwprqgbAIrKxwCmPCH6t2rvwpkpIXzxMmwxrkpob0I115ySBuP3874DbrusuQgjvi8jxa36oR5GqftC27RVwUQO/HR0dfbezs/PV9vb21D+1yD3K"
    "sfqyfqUyr0/WYrE4Pzw8/BH41T7OfQ58DLzjBh87wgP1yWGnTs4qOgZ+Bn74F+mYmzmGJkX4AAAAAElFTkSuQmCC",
    // win7_inactivenormal
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAoCAYAAACiu5n/AAAACXBIWXMAAA7EAAAOxAGVKw4bAAAEzElEQVRogeWZy24dRRBAT83cYMjCYeMVLE0kL/iDbKys"
    "8x/+ASQ+yCtLCHnNLyD4A2QLCSkhfiW62L7zKBZT1bemPDbB2MG+KWl0+zGPPl2Pru4rwFPgO+Clle9CNFwAfWrT1NZP/ObyTW3/1H8O/Ar8IMD3r169+npv"
    "b+/LtbW1LeCzWwL6bw+oiPSqmiG60JcH1aWyTrRP1f398Zk4jnY+nx/v7Ow0u7u7BwL8dHp6+sf6+vpz4L3dLAGmmngR17TFAQA0aZCdtXV2tdbehHps60O9"
    "9feISJcmLMI6qKpqLyKVqn51dHT0+cbGxhMBfu77vgFO7CNVAo7l66D71F+0Y9rMgA7mE6BeF5E23F+eMcg4cQXY+qZcQwFEpFbVb6uqkpkN9KmI/G4P1Qky"
    "Qhcf9BlME9DZB4rJqmqEnNLYqJ7u7wymtbF5n4P2qtqbJuN4IjSq6opcODCqemlF17BDRvgShMKMFlD/SDC3OMguArDUoJtvG+4vE6Gq0Uqi+XYG6tBZszFo"
    "ermdhYYRsAxTFqH94QKWgKeicIQqABkqTUrrgO6rwcSvBKoJUy7uJiKuYQ9q7SwMMAKjqlUAdOArpp1mdaRpxr5XBi0iTTLREpAA7xv5bIj6OZp7v8M5MMPt"
    "Iy23szC4C+uore7AOYA5eA5WcTkqmrdy9L02tXUTV+k332zND9302/DtCHeTdEA3s4efYBq2MD7lvw5bhUkSxuY8gmWp9Xai3ITJiaBevgAuVLUxixgB3kIU"
    "6B1YgXML32r+68ErT11epggRMpu3/46WEzfzpOkG+AuYG2w2zf8qI+AeuIyaFRFU1c3bo3b+ckwd85IQM6CR6QattsCZiLxnSP/uTcz/ceCWYVbFOmPyEWGn"
    "lqgYKHIUHS0jLCHfAWcM2tQ70uAHyYxlWnfOeA2eyrbcf2Hpzx3LCRnltCnLWojIW1U9IQWcjykzYMEQ1pul+2qM0DFie1kZQ4qlhB7EYNBcx6DNt8D7j6nJ"
    "CRFYarhR1XMAG3Q1dTNXTdqh+7Dm+dJ0JiJv/L0PQBQQ13DLYHKaAhUi4olINuEobqICHKvqnwxB8N4p/oVUQOXADcOal/22skFH8/W6i2t1rqqvReSShykF"
    "2DOYReqMv1OgvdUvgTcM6/i9jfaOpGh4wbAsxWAlDJqrwrpR6pY8nDIsL3eVHNynCFBHH24ZBysFfMdUfNrA5gbb8XikxoDdnBsxm0w7pdEuSUSOw975MUlF"
    "AG4ZdjHAMjLDsExZ9F4A7+4gif+/ZKxhEVm4ZkNkdjlnCEoP3lGvE3PNJXDYbMM4u5rbXvQxBKabpAZmvnnwI1FYJhyNql7wuALTtSIiNWF72PhRZwhcl6wI"
    "LJRAXDuwm7SnkFHjqyQzP9MqgOFUf9WkhmG3VM6Zwv51FUWAyve27YrDQgCGYT+8imYcRYDagVdZs1EGDYuIn0OtupTTjE9Fw1IBlWn4k5CK4RhnYanXSoqI"
    "fIEd9FdAN5/PX6vqN9axavJMVV+cnJz8BswFeLm5uVnt7+/3W1tb68B10PGAXsKfbvH8yy9J5Tq22Qa79v7wrngf6V2S2j5Uzg4PDw+3t7ebg4MD8Y+8AJ4D"
    "zzLYxJX/nbjtPfmvnKk2Jn5vI8fAL8CPfwN2yjbGZZiIBgAAAABJRU5ErkJggg==",
    // win7_inactivepointerover
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAoCAYAAACiu5n/AAAACXBIWXMAAA7EAAAOxAGVKw4bAAAE1klEQVRogeWav28dRRCAv7l7lmJAQrIQQnSWIhoKi9ZK"
    "h2iokP+KVOn4i1y4QvSmpkMgREuRhgY7iRPHvud3tzcUO3Nvbn32C8TE9stIp723t3e3387PW1uAD4Dvga/t/CZEwwHQF31a9PUTbXl+Xd+q6w3wG/DDDHiy"
    "t7f36cHBQdrY2PhSRB78azrVEmR0iEivqglIoT+Fowe6eB7u6ewZKTyjvNevT723bZrm+ePHjz/b39//VoCfzs7OdHNz83PgL3uI2KFAFUAozpO1U7+1gFGg"
    "td9tMWmHbcPku4kxJVwK74mtiojamBp4eHJy8mBra6sV4BdVfaiqPwOLACs2eQeXAKThBRHQz3t7aVz1CNAVrS9Ma1pxuEtjrtBkMisrXcPntAE8qqrqZGYd"
    "HwOvABGRCkBVS+D8Y7lyUdMReFhlM0GHvWS2hQZbIJp+eY8CXQGb7D19Ma8IjT27BloHBtOugUZYwnlv15PB92FlI3gMIFFbA0DhkyV8LyKtPbszGHeLGAfc"
    "T/tCw2XQ9PMuAl8EsCqcR+iRaYfVJrTRzGNw8mCUVDVNmO0w1gNWGBsX7DrQOB8VEbV5+rVuFlZhHgAduArXHXwwb4L5TgEH3xqAVDVG4JGZi0hngKX5pmBN"
    "iWxpMRimAOfzIA8fabmbhYk6cMVlk65snC/AZH71VfdrAWrwQeu/KgXF8aMUpqot2Re7sICDGNx10gNpZjdD9mGHHNLSRBCjMJUhWgdY13IqUsgIyCOya65Y"
    "oIbsZq3NbSXRClGgj8BzayuDqlRV4soFUIfvJUeUGKRiunKIUU62IBRzrQepBjizuYw0eEOSgW3CbtKwjNYYkBgoLDUfV7xMBz7ZLrTJ7hs0StbeSxF5DTQi"
    "MT7erNg7cQ0nxkFrOGxgVdwvwXxdXJNT8H2IuImc81+Stam62v9uQirIwF7mNUAVNFqmJOx3z7j6ui5qD5FUVS9E5JmqvuD/MddVIpCBFzaplqy5kRkzTkdR"
    "00O6EhEPWlNfQ6+AY+D0HWnyKhmAW3IN2wCoahV8tdRy1LCLm6SGa4nsm0cWiO6CCCCu4VZVPS1FLXpOjsm7NPPSxJ/ZcXHLGi2lBioHXjAuLUtzLn21LEAA"
    "XgN/i8gFd1BUdQD2CmYRrsfykqJ/eIa1c+DYXeIOS0XQ8AUw92KjGBiDVtwY6IAT4BTeqLS7bamAOpp0CrlVw6Apvz0nwybuj8ww4JZlzeri5WU3UUO/UNU7"
    "6acrpCYAL1gCCzm3SlECLVT1lNspGm5Cxhq24n3QZuHLDdnH77yjXiUWpQfguX25lEUGZH9t7aZ3OskblhqY+ceDf2/G+rgn+/Z9CkzXyWDSHdmkk6qqLL/R"
    "5hNfRPdZBpPuyGVgB4PZ+ob5OkkFzHyvasFy8/zSftGaSA3UFdlHFwBhr3gdpcKAleWfONYVFuz7wD8QFmsWoKZEMA3DcrNt3WXQcMvb7/veB5H3TcNSkaPX"
    "+wKMA8/JfzReV/mIvAVFBaSmaZ4CX9mFdZNPgO/Oz89/B85mwJ87Ozv14eHh5vb29jeq+uHETXHLtgqtH2LbQ3XRXxftqE9EalWdGj/5juL9byrHR0dHf+zu"
    "7j51kBp4BHxB/teHS39uuQL4bcfEHdGr+pho/4s8B34FfvwHatwA2yP92n4AAAAASUVORK5CYII=",
    // win7_notrunningpointerover
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAlCAYAAAAeJYohAAAACXBIWXMAAA7EAAAOxAGVKw4bAAADRElEQVRogeWZwW7cRhBEXzeHpKWVFQmyLNgHI5fk5/MT"
    "+Q75YixgGUYSryWtpCWH3ZXDWgZyX+0CYZ14m1esZk8BNKCThJmN/L/VScKBRlJ7aJo9qJHUFqAxs3Jomj2oMbPynHB3aJo9qJHUOVCAORguwE/D/YFh9qEC"
    "9A604zjOIeF2HMdtwmY2i4TNbJvwNE1zMNxO07Q1bGazGGkz654Nz6F4tGbWOtBGxCwSjojt0oqIOSRcIqJ93tJzqJbluVqWzJyF4czcdumI8EPT7EFNRLgD"
    "DWCHptmDGsAcsIg4NMw+ZBHBs2EdmmYPsoiQAwLy0DR7kIB0QJLmMNOSFA5EZk6HptmDIjMnByZJczA8SZocCHevh6bZg8LdawFqRDSHptmDakQ0BajuPoem"
    "Vd3dCzBGxBya1hgRVoDRbA5+tz6fR3ovJ0oyoPko/dwZv5kFEGb20m2vuvvWcK31RQ+T1Nzc0H/6TlcbtdOGYoZJ6OMrTW1Y/fxZ4/v3DLZ9AS+hWmvVc8Iv"
    "Vi2vM/vlLcebXsd15MiSXm6tIZNM9UF1dA1jz9Py1h6vMx9/dx9eAKW6exagDsPwIoaXyzzarDkZBp1W7LXQYgw7QurScJeStLF1PdWwBwvds6Ysl7n+8MGf"
    "doxTh2HYGgZ23rSuM/vNmpO64UzJeU2dTclpJicSvRI3Jy0ZQqyL6w6nsw3OGVxn5o6TrsBUgGmxWOx0TUtqlrccD4NOlZyP4s2UXNTgPNFpVo4EbkG668mD"
    "O8GRjNIF8ET2aZOkaYff9LRYLKwA02q12qnhmxv6Ta/jir3+kexFDa5qcpHiLMTCRCMjGvHgxoIffzDNmXAbNWlzc2MD8LgjrGm1Wu3+ApZkn77zy8OoN4Kr"
    "zci7MXhfk3eTuIrgIuEkoXWoDuum4Z9ifG2dL13DzauOLwZfF539/esZt7u8sv5j+Mc9yR/gl39if13ilwt8vFXz1OOlWvFXKjZRalpbOrUpOm3T6SXriuu4"
    "OBcGlw5vE64yuRTbdB3OjntOW6Md0sb1RnfmPLpxb3DrxgrxTca3FKtJrCQ2ggEYGhjQ9tlgBIYKY3Gr1VXbwWoWpuiZbKPwB4tyqnh7Z3l/j/4FC4rojF9j"
    "ZqcAAAAASUVORK5CYII=",
    // win7_notrunningpressed
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAlCAYAAAAeJYohAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAEQklEQVRogeWZ3YocRRTHf6e6+nt2ZpfdTUyWmL0KiOQq"
    "EBD0IiAqPkAewVfIM+QVfIT4EBG8EALuTe5yJQlsDJpMJk7PTH/UOV7MrkYx2d04s4OTHzQ0VHf1/9+n69SpLgESM0NEGtabxMxwQGRm8arVnAORmcUeSA8O"
    "DjxQHzXYCkW9K3KKa9KDgwPvgbSqqgQYn4OIVb7MtKqqxAP5aDTKgNEKxZwH+Wg0yjxQTCaTEkhXrWjJFJPJpPRAbzwel0B21PC2z+40Y+WYf/ZzlnvP2vfb"
    "OH5ubzwelx7IzazH+kc4N7OeB3ohhA3+ivC60gshbHig6Lru9U96XSm6ris9UAI562+4BHIP5Kpa8B6MYVUtjg3nvNnwu2btk7L0WdtPy5v6ycws80AiIgmw"
    "7vV0CqQeSM3svTBsZokHvJlFQLRqRUvGm1nkAQ+4o2Od8YBzAKq6Yi3ng6rimWcxPTrWGQPUA6qqAQgrFrRsVFWDB1oR8UC7akVLphUR74FWVR2nM3yWJd8i"
    "C4+TlpanKVIaVXUemLp57qpPuOH/zsw5Jx6ozQzW33BtZnhgIiIdMFu1oiUzEZHOA5VzrmX9DVfOudYDlYi8F4ZFZG44iqKGf/8Rf9bMehb+S5Y+65LVgHEc"
    "x7EHxlmWxZxDhMuylCtXrkSbly//Wbe/PDzUJ0+ehKqqlv2TvkqSJPZAlWXZ61stC+f69evRzZu3kg/2P0x621tx7AsfOSdB1dpu0o2fD9tffn7cPHhwv3n4"
    "8OGyKr4qyzLvgclgMIiApewefnPnTvrJjc/Sja0LRZyVeRIlaZyn3omImlk7rbsmNHU7q6affvmF/PjTD9Nv795dxsufDAaDyAPTfr8f8W6l5VvH1r179/Pd"
    "q/t5sVH2XVH0RFzZBcnFSaKCc4bawBof2dRMK53s+/2rH/P5ja/C7du3pqd9zgmajq+f9vv9yAOzvb09YcGLh0eqKWPydsaGBTZbZbNT+oL0zFmK4ixCRaQ2"
    "Z2PveBVvFb63s2N7H10Nj1Tba84tMtKzvb098UBXluUit0Ews+jxiKKurW/KVmPsdMp2G9hSrK8tuYGTgDpnUxd4ZZCb4JMATNFUpTOzTkQWFYiuLMu54eFw"
    "uFDDh4eks9SKFtlo1TaPzF5slW01NoNRihGZECKjckIJJADi6HDSWGezw0OpgcmCZHXD4VAWahTAzOTnlwyqxnYMLs4aLjWBy61yqTMuhsC2Qk8hdtA6GEcR"
    "z73wLHY8TSIOs4SnAs/KRH7b32QkIgubsv5m2MwE4Dtwu98jv+7idktcM7JomuJ8K95l5qXDtyqxTyxWI7F5dFIzSbyzwju2BXYdXFC4qMquMY+ug80ipR8L"
    "ca3SjGf2ShwTJ/wuMHLCEOOFCS/UGHbG0IyZzafNOoIam5/LfGapW2i8k7Z11sa1tOrpQkonMwuukuD7Fr6+Ns8HfwBJTPTqYg9ZmgAAAABJRU5ErkJggg==",
    // win7_requestingattention
    "iVBORw0KGgoAAAANSUhEUgAAADwAAAAoCAYAAACiu5n/AAAACXBIWXMAAA7EAAAOxAGVKw4bAAACvUlEQVRogeWau2/UQBDGf+NzHhwUBClFJCQ6REhJJCrE"
    "o8i/QGgoaOj4Q6CjpoICiSYFTaCBCio6Ch4iBRQHChQkQB72zkex67M5EkVIRJE2n2R5b26859HMfLOztwaUwBwwk8b/A9a5j453+jx6FUmn2EOvqzs67l41"
    "8B54XgKzT5dO3rhyYfFqYWeOwVjz0D9C6TLFu3dkAquTUvc7B1zgYJ7mSTIL/KFrSS/NZyjNFaKORbnwKLfm2Uqh/vzr4aOXz67f/BoMWKi+3LrXK7cmKJbX"
    "YLP55UwwZtjs0e2qqCann9wugfFeMTcFdwb4RmbGAmwL3v4cn7h0AjiScrZfENbqA32vfcWmU5YGDUnF0M/Qux0oJn/LyoG8DXYXRA8bLlFn7mEEycMFAmrL"
    "22BpGNKx7nre9iIBMaSTh3Wg77P/cAAvgR5C1ORtscxIId1DDlXe9qaQVgmUyLHMPRw5qwlpXGS8zuqgDWnVytrDqCWt0hSk3HMYdUkrQO15Wxyt8xIo5Mo/"
    "h4UxJC2vRe45HDeWFJsHhUNQh4EmpFNZyttihWFIG34I6nAMaY8bAIchhyPSjocHqHJvD1uWFl7n3x5a2zwIuagyJ62IlMMhKHcPm9qlpVCdf1mibQ+FnOzL"
    "Et0c9vxD+m8Ph8y7pYQScMIhWGlFdzakFfKvwx2DHQURcmdpoCUtxT/Sc0ZyZ/RwteX0yoKqznNB3Z/oaasOEA3eDp8+rvXOzk/x7vV3fqznRV/H+2OcPz0d"
    "Vla/AVUJfLj/4MXytcX5i5OXF2aYnCjjoRCzeFjEoFDqJ520+9ecj4njAig6cmHD8zeNvAAko7C4kC/M0oIeTM1MDGc1GL6HqT3PAxjeau8BrW9W2yurg8d3"
    "Xy0Bg+aozzngFNBv9Dr3LplpF9luujvJR2U7fbfX+F+xAQyAN78BXTJ1646qBi0AAAAASUVORK5CYII=",
    // Windows 7: JumpList close normal
    "iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAABhklEQVR42pXSu04CQRSA4X/ZZVmuWRSNEgslJBJiIWijYGGstCLBxE5K3kATHoAE3sASbDTR"
    "wsonABvxEsB4CdoQQowFBYUQ1rUwLJcKpzuZ882ZOXMEXdc53dpq5J4b80ywEgFvCVgX8tHo7dxhIrS5ETJNAos39zTzuZKw4/Hpl9d52pW7SRyOlTDx3UMk"
    "AF3T0LpdLl8+KdRbAEQWVICROL48i65pAH/wp9dD63xTqLe4OD8BYP8gCTASxxZd/PR6A/j19MjH1RlBh4/9gyQX5ycG6KNgp8nrVZElxQbASEMi7XeCnaZR"
    "bRhF2u8jb5UABFHEJMsACCYRgFqtZiQJpqF9URxUFGULZruTotNP1TxDJn1koEz6iKp5hqLTj9nuRJQtQ1BRkF0qZVQDHaeyHKeyBi6jIrtUREUZXFWy2lHc"
    "HsLoRnLYoRsH9GPF7UGy2oehDWVqmtgUxMY+fDyWrH9dlRIBb6lSa6ytbu9NNDkPlTcSAW9JyEejALe558baf4b8F1/egSzJiuNOAAAAAElFTkSuQmCC",
    // Windows 7: JumpList close hover
    "iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAABnUlEQVR42pWSMUtbYRSGn+9eEwOXqJGrpEFFEm5JwC3BQROCTXFxcLDYwUKQQkr/gAF/gBL/"
    "gLTgoE6Ci5OQQehwl3DTwcVSuVCCmEGkgqSQCJ4O0ZtGaUnP9sJ5vvc9L58SEfYzGWf322WSHiYfj1SBlNpLp52xwsdk9tVsLxxfTmwuPm9XVc6MStk+Qn58"
    "7wlUky+Zn12kD4C7FvxqsOO4lM/rAMxbLwC69PtUrL0LbVBaTe4bt5TP6xwefALgzdsPAF16NTGK1mq2rXNmVOqlopzGNNmcy8jSckGeztJyQTbnMnIa06Re"
    "KkrOjIr2Z/6Fmk3q6sxze3RKXZ2xULO7bu37Wwmu6/6zJA1ABfzoQwbHVhZnJEFpY81bKG2s4YwkOLay6EMGKuDvgHrIwDdhUhmwPKi4vkVxfcuDKwMWvgkT"
    "PWR0ourDQfyxMDM/r73lmeC198Cj9ofC6MPBDugLDxKYGmcFWOHmIaQOz/Q4vvBgG8zHI9Wv7k1y+vU7UE2UaoB2293EfRARA6Sfil0jH49U1V46DfDfn/w3"
    "M2Gc5P4I7F8AAAAASUVORK5CYII=",
    // Windows 7: JumpList close pressed
    "iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAABxElEQVR42o3Sv2sTYRzH8fflrrkLd+31Ei6BK7UKkmhiBAdBJKO7S7DgD0QcMri30D+g0O4d"
    "upuh0MVdcImj1WIbSyCDChdaSQIS27vLc3cOsUkjKH22L3xez/N8n+crvXj8MhbfT3C/HHKZ5dwsoVzJIT3M342fvHpK+U6e5MwMiqKQkKSpcBTHCCEIhkM+"
    "f2xR33qNMuh1yRcWOHj7Bl3TSCYVZDkxBcMwIggEvzyP/P0HDHpdFIBIhER+wPuTmKNeDEDJlgE4/BECcCMtcdscEolR/QcKQt/nqKewu7MNQHW5BjBVlzSf"
    "SIgJdPf32avXsQoVqss1dne2x+AcWW6TvXcNDGsBgKlmcq0Gltscn3YR5VqNqb6Vfz17u93+77eMTtQkMCWOyxX6TpGN9ZVxYGN9hb5T5LhcAVMaZc+hqqsY"
    "tkk/M0Gra5usrm1OcKaIYZuoujq5ampex1qyWUy44/A1tTPeAGBRcbGWbFLz+gTq6Tmy1x2ups7Q1K/IiYuT840wivF8wemZg56eG8FZO4vbOeVetYaqhiQ1"
    "D1ke/DU5BoGn4fsyBx86zNpZpGePnsc/PzUZ9LqXGnIjnUG/VeA3lCml9GTco10AAAAASUVORK5CYII=",
    // Vertical beta Start at rest: pixels cropped from supplied reference.
    "iVBORw0KGgoAAAANSUhEUgAAADsAAAA2CAIAAAD22XY6AAADaklEQVR4nO2YTUzTYBjHn7brNlgXikQ2FZjhY4sHVjOiDkiAi3DioPGI8WT04oGYeEY5eTIx"
    "mhjxZLyaGIkhfhwgZkBMJGwxwQ4xDKYMFRh2bF27rh4wY9lGa780xP6O3ZP3/fXJ/33aDuno6IADBfqvBRRjGhuPaWw8prHxmMbGYxobj0WvhfozXDuXcwt5"
    "AEhg6IwNn7bjei1ejA7GXSx/lcm482LhCsULAyyfYJAHzirdvbWmwiXkR7bTxboF3HlxZDt95WdG4xYlHLwcazWmuJx0wQ6KBM5bfX37ZsNxCFG0o9Ycd2Z5"
    "6YJpmyVwBrdWI74+/MMEtxrJ8RnAq8DVhh1rtzT6LTub4sTt9J/vqNVYuscJFNk8jlurEQAg6tDgkD1YVvNxklO0o6ZU+LkcUeHI7RG2Whr8Mk2JR2RyVYIm"
    "405WJhIzNrzRLzXduLSY/CJ502VoMj7JC9IFCyRe2yC1xarCBoOWHDvyYgJDQjbbZwuWQhEiLzbnhO5sriX3+zbCOFZLyaz//ZPMPZej3ngHRYZPn027WjKu"
    "VsFOAAARmyfpUDAevZhiKV6YseOHWzHpRf5qj5cu3Ez6uouvMB5qredSPBaefXV/aHlhHscCksduK57nlT8Q1ee4RLcA46EWLj8cCw6uu2TeKFYjMge3IiqN"
    "k94u6QKmifI50Td30+uLlZO6NMvTk2qMVaaCaaKkC8hoaLjfxrDi6OPse4dYPJXTm+L6onAUR1REAlQb7xeJAie+zXrdDgB4ds0STQhT9N4JO1KPBk5Z52LC"
    "6HhWxdaKjbkaFwBwpFuihqRDAc/elPC6Ma+7dGg8eqsmEqDCWLa7AOBcCfd4ZeZacdcVodzYK29M0qGOHinjaEJIscoezgWUzQrB5mA8FOPZO3bOWLikxppM"
    "BMkfziqplVU3GJT2mPFQGJsCAJIO1UVeOlciu9fT9c0b/oENakCwEyQdMi4SAIDo+B89V+NaHrxR/+7peO98+VEr8HVLOHdPwSt8Cbp9/QOAdXvd++Q6YUe8"
    "bkKibC6m+O2nGP2/TFOseOs5y2TyFX8dm8reea3so6MEPVNRDGFHen2WtnpkNx5zMWFtW5yic6pHRAE9U1FMihVfhFU+I6T5//6v+PuYxsZjGhuPaWw8prHx"
    "mMbGYxobz8Ez/gW7wSwmnH22XwAAAABJRU5ErkJggg==",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAEPUlEQVR4nM2avW7cRhDHf0NSusSBESBF8gJ+gDSuk0dwFyB1UgjQE6hXozIC9AJ+AVfu8gqB"
    "XRpIFcTOhwMLsmSfpDvepODOcrhcHmyf7sgBFrv8uNvf/Wd2drlHAUrge+BBaK9CSU3C9QKoXNlLyn4oufZe8tkqfJ8kfZ0DvwK/CPDd6enpw4ODgx+Kovhi"
    "AMyXIillpi4z9xmIiEgK1LG6rq9OTk6eHB0d/S7Az/P5/KfZbPYceA3UoaxCHT8Xiroiri5EpFLVKtR7wCyUfeCzpN531/bdDyuBSlW/KcvyWQF8OZvN7ovI"
    "m+SHqIisgAVwHYALp6pZVEhVC0ACpHVmqpfhXDx2Smvad1EU94F7Of/nTIGbUOz+6LKgXkHjvjJAGkRlIOFcFWAL990k7XjuQwHNFsB797kYW6paAqWqlk69"
    "MkD34lBVY78iYiGTAn80IDRx+I424GPH1jbYcP+eO/bQPpbJtD8ZEBolL2mVKGjU8yqZi4VuOCigqhqBQttsYwXNbg2SJqa8y2OaSc7nLB0gHdsEEGBOcLcb"
    "FJ2RaiObbnrKxVvu+saAiMiFiNzQquVnnKHvH4q9nm0MqI39SzuS/YhNgdbBDCo4FBsfY0vgL9r4y3U8BEPmejQ/uja1t6H47xuKsR7sUC7c2MWJvaSdvzsu"
    "CwBkzuWAo92lgtDkx1cOAgfW69znP58Xc4B3aa9p0o8HUgezbgbpHd+1gmZ/WifrZokMWM+2oSA0M8xb+ioNJeveffbDtqUgtCoO5b6ea5N4xQNuw66TRfC6"
    "ZL02D24NUlX/WXd5TR3LNhWE5lHh3DrL5L0OXGZA6TZj0OxvA3O5zh5t1dWaHLMrwLmqXmZgSCA7YNbetovN/kug/OaA1Vl1d6EgwAXNE6E9bxtY7doebKcu"
    "BkBETEWl3QAgaa+AlX+g2pWLUdVzEfE7E15Bg66BlarG+3amYOj4ir4r/ZYK7vxuFQx2QXcgeFBzbWeQVOGDu4J8R7NmtP0YU9B2G1buGoygoAZIi7+hJD1K"
    "DJpdQnYWyZ4bA3BOq16u1KOkmcTeZ8DiAHFpZpXbkNwVoI3cHOjoCt4YWBi5vYFiKWeMGCSAXSdQfmTXYWkWXTyG3Q64F5yiFeMoCLBwC9jc/AxjKigiC1y6"
    "cWp2gMcaJPb8sTSQMFggs2CFkSBpANOVTU/BMc3Px70cSMiDY9oS4j9a9mjagR4b0Cdra0/KxTDgWrs2BcB018G7fBKA9qAU3erbUwCErnLxmIkADm2HwEQA"
    "s65lInkwtXRFMwkFoZ9eJhWDHk6STc5JAHrLKvhmsVhcqerX43G1KoaB8lVd11fAvAJ+Ozs7mx0eHj4qy/Jzd3P691S66sjtjuamKzNxtX9JKH1loFgul/Pj"
    "4+PHwB9C85rIj8C3gH/zKF2GW13TrEJybV9yzxre/OsD6atWt8AL4On/PdHnIiya0vcAAAAASUVORK5CYII=",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAFKElEQVR4nM2avW7cRhDHf8O70+miJIaqFAFSqbkuVdrErxHAZYqUfhW/ioF0eQRHlYokSACV"
    "sWPZknCWT3fkpODMcnZJxx+6Dw2wOO6SS/75n5md2TkKMAJ+AE7suLFWitj5ChiHNinagbWh40kxd2z3k+JZr4BfgScCfH9ycnJwenraHB0dfSUiB6o6EpGR"
    "qo4MVLxxfFgEMS5+J2HexO4zGgAloQG8ubq6+uPRo0fXT58+fSHAT2dnZ3/P5/NbEflTVeuCNUSkUlW/aRXA+7EDc0CHBvIQOBCRmapOgJmNTe14GvpTEZmq"
    "6ufA8XK5/Gs2m31TAQ/m8/mXInJegANQQFW1FpGkdlVVfwEDjoiIMVP5ORFRu97PiY2PGBC77wq4nk6nc+AznzRT1ZuhSWFyLSI1oAamtJuhOaMAVMK4ikhF"
    "Z9PevK+0GmD8IQ+KIG3y2IcgMwHsflU4X5qH0DIvPp/cDrOXH/Kg90mjqktTeTk/MlWynPrhZaoIeuhhnwIQoFbVtwPjzkq0RR/HwI1K8MbmoHwqQIAaWIQH"
    "KK29JWDhOKpSi375EtFE7gQQYCUiN8EJ1DyxVHXy4IHnSXFtfKFs4ieJqt6o6m3xAALY7PJ3gCmvS0vanQEae1fhIYnNcP/yOF5XAm0oGLyrmgEaEbkwwNE5"
    "fOGvyOO7q32IYWWTDKY7qy6BRYwyDNhUkIaOUU9QMnAJYPHWd5HXFm3iQ6LKtRhvwrh6aKR0kv9bhz5SalV9TX/5cBuMwISctUZVeyymELQhgACXwJLcGZTO"
    "Hl28X7Ldc5JNsRflObltuT02ItKYKiOY2lpDbo9bYRBbvBehv/YHqmqpyqF1MMlWABqQf+iiS/TW0mvrMFazCwYBRGQJXNBXYVJvSIIji/EFNrZQv0teFCAy"
    "dkzViVW7VmPbGoMmS1W9pK9eZzQDHgDjYLcNEBF5DviepmTI1V7a4cqzoo2FuneJ7XWubLtQtsYiTw2sgXV4kWSDviHfpvxLn6U1sDbgCVR4EQWarTNocg3c"
    "0vfmCDb+JnPYug26iIizmKl44Hcdzu8OoKq+Imdoba2hdaI69FfEZca2fduWhjbzdsfwtW9Fa4sObk3n9erp1radxOWSNiWraVla23KSebJd0wCNVwi2rmKT"
    "hQGYiIgvzGs752WPmi5XrLz0sQsVQ6vWS2BiWitTrDWdT3Qq3mDK/14RkWvycOdqXdEtP37cjNlsyv8hcmMAnMGalrE1ecmkBsa7iiSlvKG/BpbOkjnJLhmE"
    "1lm+oGVrZWNeCK3Ms3cbSQp5Swh5tubVdLE49T+qgLlBaQxkBYzMB7x5xSElC7B7BjGAMcxFz+4lC/tgcWXbT1+wYzxOMXvXkSSJiKws/qa12PpegW3YYT7Y"
    "E2Nt5Q7iBfoiq2n2YXtRVmFJaegy7GSHe1Oxidsa4bcJ/ey/jX1IUiVhCxC8OjG4L3FwnmZB69Xp3L4BQpf/iYiIOQ/cI4Ap3FlEyQDu24uhi8EN+UY+C3V7"
    "k1DM1FbDmpVF7oOKwaqwhYrvB4NFCS6WhlOycF+krFvfDwZNMnCe5XBfAEZHoa1pp8jiAC+BB3vC16vwi0hl/ww0FXBxfn5+DnwnIrOdwwPC0qL2mcvXi8Xi"
    "ArgZA789fPjw8NmzZ0fHx8c/037P0rtHaDW0NWRVTXkc/S0k5H/keOEc8j+3R/FXVZuXL1++efz48Qo4FdovOH4EvgWOBkCV+9dYaCyPy9pfVtUvJH6KUn5q"
    "dQv8DvzyH8tdURcwCu97AAAAAElFTkSuQmCC",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAFE0lEQVR4nM2au24sRRCGv9rZ9XptBAkSIbbkByBETiDkGZAcEvtFiHgQh2S8AAFohYRASM4Q"
    "ki/YPj54j70zUwRT1VPTHouDvTvrklrTc+2//7p1164ABfAlcGD92louYvdHwDi0Sda2rPX1J9m7Y/ueZGNdAT8A3wnwxcHBwXQ+n+/OZrM9YDsbfNwz4NTa"
    "lj0/Ddf9GEH5dwqgEJFRDwEuN3d3dz8eHR39cXJycibAN6enp2d7e3sl8DOwzFhDREaq6h8dichYVYvmloxVNTJYGOgtYCoiW6o6s3szu7cdnvG+T+5D4NPl"
    "cnk+nU6LEfDR/v7+J8CvGTgABVRVKxGpHbCqqk9AVaN65Al2hFaVkl2XrP8A/DmZTD4HdkY2yC7wtufDLdIGZOUoegby5+KAifnsHQcbQfvkBKhEZDs++F6i"
    "qhUtyxpu+WAaztN3RcSZ7mNslOHovPu/AJrUwDs7+szzb0Q15+AiUx3pmcSzAKKqNXAfzlVENAfGE8wYEOWxmtMnXwTQpALuAsg4aAQZB1dnOzhTfCeqvQPw"
    "uSCXwCIDFPvafZxR5vW5x2toKwEIjT0+ZIPEY95vLjRAU+jKjimTvRig2d6tHX2ApFIe2yLh2ahmB1XF749Ce4nUNPnTARL6dXgmv+/qdO9+xP4qVNx8UfWe"
    "xmlydnIHMKeXaGsVUFl0iGx2Ivoq5BooyQw9nNeBpT7J31s5wMpA+mBRopZqy+e1vVNbrvfzZIerBghwS+PZ2IBuYwmwxcIYUuqwAOnIymwwkwvHYoASI6pa"
    "m61VdFVax+aARzy9RHq2iMiCZnWU25SDcifwfjzveLIvt4pVAjQ5w8KMsREZc4eJW4vY92fXYoMAiMg9cElrf5EthbTocKfwSXTi5qoC9VNySQ9zZJ4aAMVJ"
    "1ICujUGTe+CGjLkMVJ3djxNJANdhgwCIyDldZkpaBn0b4eD8PF1bN4Oo6gJ4Y8DSgsCBBQcq7TwyytoZNLmgZbFscCcHcUb9Xg2UnlnWzqDJLXAf1Bc9N6a4"
    "OIGKIVTsIiKXPmgAVIZjtM+lT2AoFaOqVzS2VwZArs4ItmLAMBOlBt6EHWBl4GpjNqp4mQNcO4MmNzQL02SH1u9jscKcBBEZCuA/NOyUYf3nqxcHFUHqmGax"
    "MISKoXGQG2CiqiWNaRVYPYZu6h3Wi11E5C1mX8aisxaZS040OEBgISJLWvuLcXBJNwQ1KmY4JwFS+tuyhXJpm3hXbQRZp6rpkABpnKW25X8nu2TOM7wNmrwL"
    "oDTu6FQ1hpzaVbzqTdN/SU1TdEoqFhFsn1QYyOQkMDyD0Cxma5q9SZXtk92BkpNsAqA7g9thrF8XBBX7xaHlgdYOrYydHAayODi0DUKTVZYerC2zxEVEJ8xs"
    "AiDAMtieEjKIB/FNqhja9IYfzWH8XB3gphh0xlI5WFVrCzkdBjclHmbENlFeKgFLdZsGCG0u9i1pQSiDbMr2oihtuosO08kkmxTfwKes4td4DQBDMV0tWHv/"
    "1dggtB7stuiVh80zGGoxmrW0Hnwt0gHGa7FBkw44s8vXAzA6CmZ/+YL1Avh4Q/j6fh8pVHWJAfz7/Pz8F+Ar4INBoZl4aLE2AQ4Wi8VfwGIM/HR4eDibz+d7"
    "Ozs73z4BMi+Ap71rT8miDJ4Z64BJfXR/YMz/AVJdX19fHR8fXwO/Cc0/gr4GPgN23wNUXj/JCz55Ca2veA7tPrjg8V+tHoDfge//BRWXQPDLpfnPAAAAAElF"
    "TkSuQmCC",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACUAAAA8CAYAAADhR3NQAAAC+0lEQVR4nM2ay27cRhBFT1V3k6MHFCmKLFiLIJvk5/MT+QitnE0gwDKMRB5pNC+++mbhkaMM"
    "HGc33QcoNAmQwEGR/SrS+IcExF2kr0Szi5fzsLs2ALYL7WLaxfgqhr0YX7Xjq3um+GJ0e3trFxcXtlqtDAht24acc0wppZxzI6kNITQ552bXJjOL7h7NLAAe"
    "QrAQAoBCCNndR0ljCGEIIfSSBkl9jLGT1DVN0wMd0JtZL0lmNlElkqy0wz5eWuBr+K8VivnVb9T3+P68qjFTJxVK9U8KpSX28U1bYabiYPH/Lzss7jPVJ2Uj"
    "9UkN2VJpiX08NqpPKoumtMQ+LiqUAtrSEvu4ZPVlKrqOa1voeXQu/3jku/fvdSzVMQ+6wdWq1w/LVhd3T5y9y7n4O+YObwTXw2RXXafvWXJ2d5ePSkrFDNfb"
    "nrfRtcBpbItzDu9yzr+4d2WkMlf9xI3gSEZsJmBDbrONksYS+7AouBwyb9kNouaMuPUatb2/tw5YH1wqi/NRXJMB6A02Ea2GiWV/zErSxsx0SCl3OJ8mLrM4"
    "z3CWs50Ogx1Zph2CXmoGB8WPW84ynE7iJGeO5GpxNXJL45b4e4Gxy5ORMiQTQeDKeDbcTGZWZk/oXbbeYZAxGWRzsossmSQO+i59kVputXBYBmPlzsaydWTr"
    "LWuIM8afSwwJ5qxD4MGNR4eFu5YpsRF0abKBz4Wsg+JuPEfjY3IeUmAenYVhqxTYNGv6Qw8HANHgKTkfUmAeAw/JeGzQcxttfXNJmWnGjXkTuI/OIhmPFpgz"
    "Y9Gdam3mRUp9EfFp1vDBsFWDnpmxmD2y/PGszGQMEGV8MviYgjZttHV3qnVJIfg8981PGvurWdPfXNKVemT/khrF/Kdznuzi8L3sv3CJbYlu/y1clOn238Kp"
    "USrUKIVqlKoxUwZ9aYl96szUUGOmottQWmIfH1z1SaWuwkzlyFhaYh+f2gqlbKvi66d93Ff1fXL3eFZfpuKbheXdcT1/cDw/f64X1PQHx9/j3qr2JG3mVAAA"
    "AABJRU5ErkJggg==",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACUAAAA8CAYAAADhR3NQAAAEJElEQVR4nM1azW7bRhD+ZrhLSqQiS1FkNwbc+gV8cYEAPfSQl8gj5BXyDHmFPkL6ECnQQ4EA"
    "PdmnntwYduqkjixZFH93pwdRqaJYsRRJFD9gsFhgufw4s5yZ3R3C/9AAVCE1AHUAPoAAQKNoJ+ID8IoxuhAGQMVceSEJgBRAVEgIYARgWPSHRT8qxiYAjJow"
    "Oj09pXa7TWEY0sXFhTMYDJx+v6/iONZxHGtjjCsirrXWFRGPiOrWWo+ZPRHRzOwAYGYGAEtEhohSEUmZ2WVmDUArpbTjOExE3Gg0xPd9u7OzY4IgMMfHxxkR"
    "mcmXzcIpvt4tNFKbamtTfW9KU06hLQCwAAyArJCJFuIpSabadEq7YBGZR2xr4PuHAABkA++enfNTn39dnFhp4O5vqJ75PnQX0lSpxLkbbNx8S69HTvvibILJ"
    "KuDIq+BCVxmp+4fNNcEqrmLuOmWuySKkSgVTjuqRyizpbZOYBStX7iI1a+959l/Efy3t49gK3GUf2jRYUEFSGOdElQKL0F2a2kSqsjBYsfhVS/RYMTpnN9i5"
    "uBBfpBpxkAnohqk8GnrSfttH8y9rl1lji5h5kRD1maWYgV0B9jJD3SSRhxii+erV6/oSxNYOtsBenOJxlstuatHJYrS6Pxw2nr94sbW/kq1FNzXYzyz2UsEj"
    "MWj7D4LmTz/+XD86OtrKGmMBOpnF48xgLzfoZBYt9v3Gg/au/+TJ0/sc6yphZu6zygpauWAPFgCQEhARcaRrQfLd4fdZEARJGIYLvHslfPYzMAMtY9CxgpYF"
    "mtZSIzdUdx3Xa3Ta+uDgoHQTsu+haYGGEQTWoi4sHjG5uu4prXzV2t8vPV1WmqAtoEngCMBiwUJgJiKHeWL3VdLh5XcziaWUgUwIhgBLDMsCa0XEWLuVGMjD"
    "WAYMDB1CyIyILCViJc2iJM/yUX5zeWnLJqWIMXIcXDPhhoEBswwJEqUmTYbXvez8/NxgfMxTGpgJt4pwpRnX2kFPMQYiNsziMPrn7G0ahqGg7HSYgL5mvNMO"
    "rpSDa824saPR8Lb3fvTmzet02QnXAcWEnuvgUjEGmnBDDnqj2/D2jz9/j05OTgQofwumIPhYc/GOQKELuUUNgw9/n0W/vHyZY0v5uxLCRwKutCORp2iUNGT0"
    "7NnTHONzzAlK9VPKCnqBS/+6I6T7HSREXPrf9gWpXNA7bKFPbdrqZmEaLIKY6AtC69whLw2W8Tl2pcCoIimniqQgd5Kae/C+Rsyds5rmo/FlTaVQTU1lq2lq"
    "nWvtk89jxZStceK1gDOW6pHSyZ2aWjTMLIJvOIhV4yvSKoGNV0FSFIvZNolZMIe0aVLz3Mb8CyPVvFNTZcS+uZhm+7UKjtkqjkkFRw3jzcXXKjgm9QfrreBI"
    "01RnWfZNFRwAEiLylqng+A/2C95AY+yHMAAAAABJRU5ErkJggg==",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAACtUlEQVR4nNWavW5TQRCFvzO7xlGAQBEKhEKDxI9ElwKREhCiJBINBQjxANQ8BC+ABDwARVLQ"
    "0FFEgoYGiT+BRIUUEEVEEiUhse9S7NpemySUnhzpanZ9fe3jMzP75xFgwBxwBjhU+trF1hcjbYAENMV2S7sLdIodbTfVlRjGNrAMfIrA7NNrrfm7d+ZuhJnT"
    "U7TahgkUhAIEEzKwkK2iCEHIhEWwABaFmcDyMxYlk1AgqTxH+RwZCA39TJNKW7DRdJsPK69eLyxen//+TMCtjSeXH06YJvn4doVupyEAAREFkUyy147l9VbP"
    "GkSpf6+lqm+iJVT6qX+/PGsGMRlRRsAwGWEqwtVjTTqa4vSjewZMTsycmubL+9/sdBrGjtUGe7lqOn8EiAbARDuyvtYZL7EaW012TU6A9G+M+kFWUNJ/3jc2"
    "FAUdhN4eyAT9ClhcbO5jcMws9sEBiUGSWw0LQb84EDFItbZwhwPiYhszi32QFTSPWZwd69jFA4JOs1gJegSTR4oJ8g6G5DNJ8vTr2MVDCrrNYh2QLPYYg+pA"
    "URCSRxeT6BM0r/x6MSiPYTgYZlL/8MYVBgriWEHPw0xTJYk8ZvFAQZ9TnWqCftGPQYcaendxThLzS1BdwPUJa17xOz5hHYpBtwo63pNoaCYZL5fd4X6grlYz"
    "onGoYb3ccgiRqhh0CecK5r+UewO1S9RHHx4h9y6u52KHSHm55djF7rPYfwxWc3Fa29rh+GRrvIRqtAQ7/Zlkc/vbr2UunT3BZDuMmxpMGLpwuNv5sQF0IrD8"
    "4vG7xZsPZm+H+1fOqR3D7rVFg8KjpLLCTQKV/UICFPJxfN4nlqN5pdSLJKlcKYH1+gKBWS73USdt//m5/nzhzRLwtVf6dBE4CdRuHi2F2qtkqra2S3+vMisb"
    "+R4quwl8Bpb+AlVEvGOFkcgvAAAAAElFTkSuQmCC",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAERElEQVR4nM2av28cRRTHP2/3LmcndlAsCElAtPwVVHSpkKj5F5DcuHHh2qJ16RoJiX8hEn8C"
    "dYSEaEFCQlGIzHr3UezM7Jt3s3Yc3+3eSKOZm5u7/dz3/ZiZ3RNgBbwEvgQeAA1wbWoLdKH6IkBlal2oi5F+ZVoJNZa3wK/Ajwvg27Ozsy9OT0+/WywW+wFE"
    "Q+1cGyumtbC+RnAZae3nUr9t23cXFxccHx+/E+CHpmle1nX9O/CXUS0q2NzSxrmdqWouGIGsalHJhYjUqmqVrYHnTdN8tVqtfq6A/bquD4C/RcQrMEsRkT+X"
    "y+UBcGRNYGXemRIBAURVhd48s6sYy5iCOwEHOSDk0bUTxapXARICpZSbZimZD7Kem2ZXc80HVXVn1AMDaE1r+vWccGAArXIh3eyEit4HS2vprCXzQWPaSkTs"
    "LmW2ku02VLVmMHMcX8zIl0WwN+1OBIpPM3FbZEHnUlA9YPI3439Cv2ebK5q1pGAV/C9t41V1ORNgFqWlM0KM7DkAlaCgT8rZQSio+WAGQGDIef7M4PPjHtPn"
    "w6RgZZY2b16r4mpiQCiYuGTeOOfh1HAYGBERb9rK1akBwSoYzGwhfa2B/SnhKORBf/r3Y4dzAPolbi1xMyj7iGn3iAkQXOQy3IawsMsAOQkcFIIi+KI3sZ33"
    "0USAMJJmYDC539lUInLANEk790GTZiwYuPVZVWsReTIBIBTSjD+j1K4f5348BRylICH3ORgUhEHhFdv3xbU8WLqRZMG8upOo6I+cse/B7Zy4NB6y3ZWlaOJS"
    "ks6A413Y4LPPpgJMq4M5f/gWAxbrE2Bvm5A35cExyKyIyKfbggO6mxKuhyr9EFT1iO2pqGuAxoQezBcL+/k24BLgyOOHUn9s3mO2sxVbV9BctNTPXruA2rSK"
    "g4LmImOmzVQzilvgfeCTTUOWFBSjjFUpDaVP9/4axwR4weYO+WsKZhePr13eEwNW8sca+GxDgHjApMYNt4BL47Efn4Q+DvXecB7Ql6L/uff9WHy2/JzN3Lbr"
    "SkEylvdumpMexYpIC6iIPN3AbbtbFSzlxOwLDFwLqKoq0KnqSlXvu2csppkxH/Pvq4hEsPgQuwXaMK70J8D7bMn0Nj8ZM1FHr5JX0I63AfLQvH9nwNE0Y1cJ"
    "txT6/zNYBTurqojEuYd8WH4spxnfNw+6WwYztvT/W8C87lQ1wZp+NPddnxiMKihmlcAokUxqLt4CrapGyNha88f+Q+6m5KiC2ViAaYHrYML0GhMUqhoj2YJF"
    "OEJ/Fer7pCCtgH+7rntT+IAYkORnASKL3KDeNUNk25ouFtqO/nyzByzNE61hourR1dXVG+CfBfDq/Pz8j5OTk2/C31PGf04fnZmpyU3ehh9go7r1c8132D8K"
    "pdJ13dvLy8ufgNdC7xffA1/TH4IypvCF1642wH+mLfVttZ9NSb2gQTw9tsBvwC//A8Vtw/i+0LgiAAAAAElFTkSuQmCC",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAE/UlEQVR4nM2avW4bRxCAvzmStiTClCPFcSmoSFILatO5C6AXCPQMeYU8QAo9hIA8R+pU6dQI"
    "gVSkCFIINqTolzcpOLM3NzzCsEXyPMBieccj77v52925FeAl8CPwPfACeASeQpsCtbUsAlShDTracMHnKvRizeUG+BP4TYCfjo+P356cnFS7u7tb9gM1sNh7"
    "i3AKVCJSAaiqpJsLMBCRgapW9js/n6Fcbq+urv46PDy8u7i4uBPg1+vr63+3trZuRORvVb0zbbnmXItTEXlSVT9XehGZArWqth5GRGpV9YfKD9gpIrKpqj9c"
    "Xl5u7O/vf1sBm+PxeMfgpv5kItL1dCsXVb0F/tjb29sDdoZApapDFvtYH/Ie2IbGucX8qPiG+VPvUiLQgDwqFznw2qVokC8QDhqgDCqE9NGnDGlDIiKVpYaP"
    "poR1SBwBREQk+OIXEShRgwMDqmecqszAexUH9BZNXPu5PmUO0MZM117vfljgLGJbPhmOe5OYqOM0KMINe2JT7OZxikTofWzuC7CGNNkUEZ9UutkFGPUECLQB"
    "h2ZmMVCfZI56GlEU0AjoWhsa2ICZVoeq2ocWa2jM6aYVmzH7bNhnOX0AFg3GiC2RnKJ6owfAlgZdS092TsNwNzTAXvwwaq6y4KhoAkTC9+vWYg2o50GPYqHJ"
    "f7GvgTHwYY2ACinN0PhgXt9WwNYa4cDW42WqRRPJ3nKlYARsrotOROby4IjG3KMAGiP71boAPd3lNONgeQh0F9hmfQuquTwYtRXNXVlE+zXjNQE2kwXTUITy"
    "YU7seOTjM7biX7VYvUd9eHNAgCcbf73wM7VrPJon1neVSpYmvrKMa+ER7VmNA+WoHorIV6uEM2kFSfS7mPtiX5qqfr1uwDjkDVOfh7wBs0rsqlNOKw+WSmiA"
    "rcIMO8MOgFVrsYwkOJAFSwEKgdEysYFPRGSVI0sTJGHtkbUUl6ClqORRr6rfrBCw8cFQ4M7rYx81upakIiLbzN4SrBYwtjBq5BJcLtN5FeLNigAVqFs+SKOx"
    "UsRMwDmaBdhR1VVosWjQgVqzaxo/zJpz86odq4i8XTVgl7TKwiLSMre9GQBAVcequuyIXqjBbEIPpOifXoGNml1FRLc0GIvoLv59LmTG1OO/fclyZzpzJi6m"
    "ZN7nlDZ8TfOuzkVF5DVLqsrGKX85B+WFYCsx09aqn9d03hf7r5cBGKf8UfLkobzR7PgPhytvBGxEGovIMtLOnAZjrssgNW1zxglrMak9Naq6w/MrEa1E7SAZ"
    "0rUTg8EL7F2f43WTZ77G6NRglAzqfeumZtZ4zs394pmzne4gSTC5yh/fvNfpWgmwanlzk+eV7lqA+YakzzF6/bj4XfjssH7dmM9PPYuHunADh1RbCmZwaApM"
    "OTf6f218JuRCQK8Nlt0eIuKayqaPx9lHldlehspSz6dGdgtQO5qxzYqZAVoN2LUGwUWsfOwPFIG9APDJgA80+S7uk6mBqarWaZOFWgrJZl94I3s4UdXBxyAt"
    "+m8d8L/7+/szZsvI0aLdHmnLSe0+6qB2Wc3MpEWTNFqV8F++YpwbGGyPzd7Z2dkH4L0AvxwdHb05PT19NZlMvqMpVGZTu3nLPhqaXUqP1h7sOPYP6fu4s+mR"
    "2X6cqap6/3Bzc/PPwcFBdX5+/iAG9DPwDsgljQj1lP74IfRdn2OLv3U36nqT6nOAKXAO/P4/ib1VWVDfN0gAAAAASUVORK5CYII=",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAAE7ElEQVR4nM2azW4cRRDHfzU7G2cdJ7ZJHIM4IyQk5AfIEckHTlyN5FfgRTj5GSzxDFyQOHKE"
    "K8JyrshcEFl/7xaHruqt6RknsrM745Ja0zsf2/+pj3/XVLcAa8C3wJfAE+AGuA1tBsytlSJAFdqoo9V39KtwFGsuU+B34CcBvj88PPz06Ojo2dbW1rY9pAYo"
    "HrFj/EMpBqwCiC5g7wPl8u78/PyPvb29tycnJxcC/DidTi8mk8l/wAlwbqBcc67FmYjcqqqfy0cRmQFzVZ3ZSyigIjJX1fhyyodlA/ju7OxsbXd3d7MCJpPJ"
    "5DPgL5J5BRAR6Xq7PuQd8PPOzs7XwCcVUInImKSlUoYC+Q/wCsxvVLUSkYZvqepQ4BqSI9AAeVTe5cC9S6SIRwcOuoFlTZrZBxUPksxP1vc2uLj/jTBqCb74"
    "KAKlJkxTBmiecKqStDqodAaJqj6aYCkn+dIX6wGxAQFgERyVaXHwYIn+1wLJIjsZQhQb3FOmyhIPn5M9/xsK4BwWPFjmc07UAowHAqgZoPGgJ5UiIjXJ7JUB"
    "HCKaFWgEQk7TjQ/juScDAJzDgqiz9vxCIGvX4hAAEyGbD2YutCkv+uXaQACTBi1hrVX1FqhVFRHBNDgDJvTPhw0frI0H89dXmO5cs097BrgwMc3ZI5tbRCJ5"
    "PxsAYA6SESkQKpJJR6TPSDfxXEQmPQOcETQYP6zHoe8kPlLVMckX+5Js4rIi4KQdyVpILrAxBMAYCF11lToEzyY9zSoi0gmwxoKDdupVk8zfS7BYGaWzKlX7"
    "3BwJPPQ3+wBIESQeHDmqRWQUNDdSVa/XvKAf0m7zIB1aDNdqVXUX2O4BYEODzoMRVFnny/O2qr7sAWArWXA+jEXGVpSbZteA530BjFEajzGqI6F7/9WKAWYT"
    "Q7MEks1rQeEZ9yjcWwMvVjz9tYJkFICUwCJPZtOr6s4KAbZoplHZKlqUeH2T1SWznTQTf0eJlYeywr+qiJ4D8+iDfnQT+wzi1+KLOEAFXqrqKrSYNeiDx0pW"
    "1mAogTQ05wFkIF8vG50tbTQA+tH7ERihH1/E799Q1aVGtK3HJIBBG2VgRI01SsIW4bBYnFk2L+aPppYEwLHaGleJ3P9iQD1lubNLwwchRGrwu/ydTDOyyyUt"
    "d4MtlleVbQH0ge7MmENJpLzPf9cGciUAPXGIaRdYMV1E3KRxUVDDnznQdRFZBu20fFAga8lBlJpyQJE3teP6Nm2yv680iNolJgQSbnTgEYivI0s4p+He5x+5"
    "jNHSYNRGYwHbzFtK6YPluSd8XLmk08RdQZK3AhjQcmuAu0T8Y5cJDy/dtQC6yaL23C/dzF0mLcFJofF1Hk497SApBs9+FgYtXyLuaYBU0ylX7NceuDCpJfnq"
    "Hf6mPjcW9zWAlQ+E+yuSP94XZBtg6PtWlPK8hvJw17NCW6uuzbF9oN0PoKpeYttRSk2FwVxDGZzVTzKYQvuNvj9jhdIPgdwQkakDPL+5ufkN+JxEDS2NFYN2"
    "+Z2jaJyPbhB8Uq3k7IlxyRpj4KvT09O/gX9r4JeDg4PXx8fHX6yvr7/hPbUXAzCzdquqN6StLNd2vLL+FXCtqlfAZej79Wt79pq09ybudrq8uLh4u7+/PwX+"
    "FBIN/AB8Q5qiGphYbOCJLYK6vqMfW3zWX7IrGD2LmpH28fz6P5SkQEWmS5PlAAAAAElFTkSuQmCC",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACUAAAA8CAYAAADhR3NQAAAC5ElEQVR4nO3ay2ojVxDG8X+V+mJJtsYeJxi8mF1m5YfIm89jBJxNEIwgYwbPOLKlPpf6sogD"
    "Iptk5T6LFBS96As/vnOaPk23SRok9WbWSxqAARhTSoOZjaWU0cwGM+trrUOttTezLiK6WqsDVmul1iogJNWIKJKKu+daa3L3VGtNZja5e8o5J3dP0zRlYFqv"
    "1+Xx8bHc3d2lT5/UdWYGEEABBFQgAz1weN2ednfSi9e219bJNcrJtTKQXjuf7Ksn5wjg4gLrXg9spr5s5D434p9VnmzRHCrWag+lswaTWkx0zaG8NIjKo/rm"
    "UH1Ye6gSLSYFQ3MoYGwOpf+T+q9lDaJqk0m1iLLWUJLMjLNubgiApMVux/jbN4bOuJoddR8xbr+zOo5a5cTS50Ztt7E87jmfJm0ydiG0NvF+NtR9xHjcc56P"
    "XCq4yqHLEmww3s8y0SUtxidbcWCjylUSP+TgJlVuQzMN327HeBy1ytjFa0LXuXKTg2vBuzdPSpKlFUOuLIXWJdjkylUOrou4CXExR1KLvFBvwZiqLSM4D7QJ"
    "cVkr1wpWb57Ur9KiHOnk1uMa5BojWFaxDjg/P7PNLHPKDDNkYbgCF7iJRUA/uuZZT0lIMrkIc8IgZFSHnEV+c9RPZrU7o1goE5YsbHLnsDCeHfYvE09zDF/t"
    "q+XkmnrXoYq9V57cWC8WjKnw7c1RZqbPn5XSyCFXe+5cT4IlMBBQbAYUwO0t0/a7vVjVHziDjL8daYKvM919Vu8jXtjT2REfKphTDA7uPMz2QP7oPm23secS"
    "OBC4pQ49+8Tvsy5dPnzww31EjGFFRcdc2Qd8mX2R99F9klR2O5vSimeh+YbvtMysAi+SDr888LWpFwczUwl7aQoFYKbUHAqYmkMZtJeUW4Ookuztly7/Vr2r"
    "PZQ6SnOoOFp7qNKrPdRyIppDDe+sNod6eG4wqR8fGkQ9/PzXB+6mSpI1l1SzZa39wQHQ3B8cAH8CqucBi36Mv6EAAAAASUVORK5CYII=",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACUAAAA8CAYAAADhR3NQAAAD7klEQVR4nNWazY4bRRDHf9Xj+fLY3l2WEJQokBOn5IQUiQMSeQjyCLxCniGvwCPkAbjmwAEp"
    "ErnkgJRTSCSHxKz2y2N7ZnqqOOAFZxevg816hr9UsqbV0/2b6lJ1udViZpGZhc+ePUvzPE+Pj4+zyWTSH4/HPTPr1XXd995nQKqqXTNLgHj+XgA4VQVQVa1F"
    "pFLVwjlXmNlURHLnXC4ieRiGp1EU5UmSjHd2dvLBYDC9efPmJMsyf3h46O/cuVMCdEQEQAEPlEAAOEDm7QpUC1YA8dw6cwOwhb4lMJv3LRfsbAwP1HOzBYP5"
    "xGc6myCaT5gs/CYLz9HcwoUPOPuwegGqnEPNzlmxAFvN3/lLP7zQ2LFdyYpn/IkEl0HZkvYLA/2X0swuhWpEllzuqUYUFHRaB+X8elDLYm2ddy+MVcUWts5T"
    "oUr7oLy+76lNluW81k4bIUSt8xSw9Yy+UvZ/9NRVbCcr9z6khctXtzGmaCOUXCHUspx36TZjZiJC0qEFMrNgOCR+eUTUEfYah3qhGr86pjuLrVuVpG5NqA9J"
    "Fcv6vNf++PGTlDFpUdigQvqGZWJ81Jinvnv4ML72+e20mtE3Za9S2/XKAGkI6u7du8FXX34dd/vZwGp2S+Njr+xXNXtqDcXUvXv3o/7eJ13X7fYqZXcOdL1S"
    "9g12rqryXJoSsiyTT29/FoVJloq4zCuDqmavUva9cV2N/tY9devWraC3vxdGQRT7WlJBeooN1Nita/ZN6W49o+/euOHCTrcTpnFHnETmLFYlrY1ModdLZNBI"
    "TAXOiRMRFRyKM3BiBAph7GytemrjPFWrmpqZM1QcKqAm1A6qyqi2vnxHw6FWfuKraeFNrRSVwjmmgZA7GE8KTra+fK9fv67HB4dVWZdFJ7CpORu7mhMnZEFA"
    "XHqOzkNd6eEFQJ7n9tvLV2U1y6dmmnccJwYpEKHghaN1Tl02yVMAPH36pDw9fDfRyWQcOo46AQdhwNvQ8UbgoJEi7/nz5/VPP/84nZzmJxJwGAm/h463UcDQ"
    "OUaNVZ7fP3pUjH59OQ4TjiLHQdiRd0nEGwdvG62nHjy4P32hWsUq3rzNqpqxwrvzUIux8EE10QZ9APjiz1NkPxxKUXbJDRs1XnkCiEgNTMxs+suooUBfJhEx"
    "rzJpGurCMotY2TTUP6loHZRA+zzlZD2ojcrhVf18KdsvXVYpdNY+KOvgWwelM7kUapPT4rW3KB9a+zyVFmjroKIdqa/q38zaGuXb99TKOL02auHyjb7BWgf1"
    "Le97avXB++b6oDFb5ykRMfm3NzhUNRWRaJ0bHEEQjJMkGV/FDY6Svy9ULLvBUQFT1rzB8QfqSyYLGDCi/QAAAABJRU5ErkJggg==",
    // Pre-rotated beta task state drawn directly in vertical bar coordinates.
    "iVBORw0KGgoAAAANSUhEUgAAACgAAAA8CAYAAAAUufjgAAACuElEQVR4nNWavWsVQRTFf7Nv80kSGyWmtZAIdnaBgHY2Cga09A+ws7Dxz7CztTDa2dgKIgQs"
    "tBL8AAUNkiKKIia8ZGfusZjZ577khXS+64FhZnZnZg/3zpk7O7sBmABWgWVghgx18rZsnbqNyA+WR7U5mHef0T4HoAG2gDc1cPHB/ZM3b6xdWJ2cWpxDdUAI"
    "M4FAEirjKQQkch2QAQpl+EBoH5VymfZ6bh+kcIhPl5pAezGlT9vfn9x7tX79aYoBuNX8uHK3F/qB9H6H1I+YjIQRZcRgmEEUREQjQkREUJRoBNFEBAZ1OvXS"
    "t0GDMfI9SNYpIxIwPdVjZXlxX2Fn+urr2xVQ9+rTs+jtDvRbs48Pu3uJlx+2J8+cWgJmKkAwEaDRcX3/GX7uNmF+egKgEKzGzOhoZIIKxzYcF4oFfRMkULll"
    "WAHm3YLu5yDeVWzQGzePI/FfqFgiuGXo1oKiEjgm2MJvLC5bl7IOOowkeeMX/Lq4BA/HLtZAJE5jcYDiYpDDSNIRieFxuzWkYo9zMFNsVewX7To4bh6H4d7F"
    "Ct1Y7FAkGcGzBQHPuxnTQMU4tqDjzYJlGbfroD+Goati1WNmMwLlILBMPn8GLPA8B0M3Fjsk2Al1uFxm3L+T5G8HrekcHm8Nu9gfv4Lgl2BHxT4xvFC7hWML"
    "akjFDjGsYrdw7GK8q7h8Y/JL0P0ctL9vdT6Rjwcdu9j/HJTzhXrohJXGYNoP2bn5mv5ehGzBaHr3G7u8AAvjJzlRV5w9f6K/+fUbsFsDW89ePHp8aWXtWq+6"
    "s0SYragQFdCTmCwd8w84QoZkgAkZQUkoIZOwKJTyPbNctwQWQSZSEopCBpZyniz3URImaPYsbX75tf5w4znwOQA1cA5YggGdFoFs5Tb1SqpHlNv7bZ/uLvjA"
    "/0VDadQvU/vAR2DjD7+AcsGujQYeAAAAAElFTkSuQmCC",
    // Neutral soft-glass overlay for both taskbar skins.
    "iVBORw0KGgoAAAANSUhEUgAAAMAAAAAoCAYAAACoy/XoAAADTklEQVR4nO2bW5bjIAxEnTnZw+x/pZ6PPu720Dz0KBXg6P4kwRg5dgESwq/zPP8eCYPTWad2"
    "7Kx8H31e32vn9upLrqFX5gHd3jfvqIaTb7zCj+J1s3v/jmr7jrftsr0aJhvZAeKIFrW2/ZrItcK/hHgOylrnWZBen8lGdgA8jOm/LIvobOUMcXR+38vQ14Oe"
    "nf7jT1TDH0jpV6PaZPNqfB/V3ZKcAfzMEKnGvtf1sYzAM2IAk73sADZmi/4iytVoid7q+jBiAJO9dIH0IEWHdpuiRtqy/NWo2yr3EOpm5QygAyFWz7IoQuBS"
    "d6gs1557J12gzfE+QOn52kRYBNr8gLSOlVAXKDvAGI/wEMJH26rRE3FvOfQoyhHX0rMPJztAG4bwPW1ZbVyCkor++n23OQp80wXaHMsDizgnKha46Lk70pE3"
    "XaCHwRA/WvgRHaL1u2Yv2gWSkHuBnFgfGtLPXyG/IF39OSrl5fEWUVs31GQH+AI9gmvrrr40Khn5LZvqNIQMDu+j/kc/iSjxzwyEo3aK9kb+nnYQ9yIkjihn"
    "gFqU/2QixD/bJfKuDo3KrnKNrdkjftN+ywXqGX9K55jhxkjreo+P0G6Ga9lcIfh12bbEADlL6I5r6rEyzi1GYrcudzKCe3oQfCVTdoMtWLbbJMWzzdl6noXQ"
    "zpOrQDbYwpfUtSaMLD5+1NtfI1tSaJngXdyh1fzzmfGHBtT7vuz8Bj0TvEtHGOERN3KEnuES9bC6PuX5Gij/LfMAP0SJO0L4mrqSwBWxIoSGEje08gCoC1kB"
    "rwvBEH/UiB8lbGYMYGHaZriVZpOZwkcc99ZnMXPp042kA/T2f3wiKwbBKJAuzrJ+/53IZdCdA+OIjWdM0TMzxUjo+QJLELyzsO/UbliEwNmjPUq4zIAXASQT"
    "rBH3rpngFtYHzQyCWe3cWT3gdZGZ4DHWkT16Q9sMdpsVhkh2gz5plEfADoKZbUl5zKzAWAVirSIx1txZQfDMNrVQti1H8UmZ4KVuvJEn/AeExmD3ITfDyY6v"
    "wA7XyMKqt1/3EPVK5C4doQQtqqe6OU/hlz5zFaiORXTI5FmKnsTTOwBjvw1qJ6jVfuIA/U6wJKBuPWT0ixUo8UXlAbT1kgD+AXjzVQh5exlhAAAAAElFTkSu"
    "QmCC",
    // Windows 7: win7_overflow_arrow
    "iVBORw0KGgoAAAANSUhEUgAAAAkAAAAHCAYAAADam2dgAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAiUlEQVQYlW2MIQvCYBCGn/tEWRomEcanLIhlWAwm67ff"
    "vGRYWl8xWRTEIIJFJu4McsicL1y49557BnSSLCBeQTyE+9Va1wVcqnrcgUs/e8/gQ/N8qQ34YKCY4XSpC35fJ1kO7UHAh3pf9gBLttzmArM1tBuQKejoe5YG"
    "9AyuEphHwBiI/ogewO0NbTonFU8xG5AAAAAASUVORK5CYII=",
    // Windows 7: win7_overflow_tilehover
    "iVBORw0KGgoAAAANSUhEUgAAABUAAAAUCAYAAABiS3YzAAAACXBIWXMAAA7EAAAOxAGVKw4bAAACyUlEQVQ4jaWTTWhUVxiGn3P/fzIDN1enE1PQaujErKeL"
    "2ELbgGAbke66aqHQhUQwGzddNIW06N7g4CKrCu6ELhoRAi0FMZssBTOQHwVjberc0fm5M3fm3nu60FvSabro5N188PGch5fDOYLXOQ6cBDyGTx3YBp4I4PTC"
    "wsJnc3Nz5wuFQnFY497e3vNKpfLz4uLiTwL4tNFoXHYcpwMkh2iqhmFo5/P56xpgOY7TS9M0kFL2hjUKIQzHcY4ClgaQpmmYJEkLiA7R1ARcAA0gjuN2kiRN"
    "KeXQUiFET0o5kkkdoJskSSil7B5Cmqiq2gUcDRjp9XrtKIqacRx3Djqwunq/sLn52JuYOFE/e/aDvYMYTdP6QBsY0YBcv98PoyhqRVEUDsL37v1WfPhw0715"
    "89qVixe/udpsNt1z5z58PsiZppkoihICOQH8sLu726jVao+jKGrvB1dWfj22sbH91o+3bnyf7b784tK3k5Mn/5id/fjZgNT1ff/E+Ph4XgC319fXV9bW1qrV"
    "arWVQQ8ebBx/8aL+9tr6L8uDrabLM18fOeI9PXNm8km2K5VKI9PT06VyuTyrAduu60rP82Lf9+MMevmyOXZ39c5yvV7/1/3dXb2zfOGTz7/yfX8r23meF7uu"
    "K4FtDXhl27YwTTO1LCvNINd1go/ev/Bdv98/mqZSz/aKIvq6rv85NuYH+3nTNFPbtgXwSgOaqqoqlmX9QzozU65ubT37vdttm4NNLcuNTp061tjPW5aVqqqq"
    "AE0NaBmGodi2nTqO8zc0NTXRmZqaOPCJHRTbtlPDMATQ0oBQVdX8oPT/xrbtrGlLA9B1Xei6LnO53NBSXdelrusCXn/TThAEhWKxmDcMQw4rHR0dzQdBYAId"
    "DdipVCrvzs/Pv1cqlRJAAcSb+V8RgATSbNZqNXVpaUkHdsQb6DTwDmAN2xToAjvAo78Apz8MBJIk/tgAAAAASUVORK5CYII=",
    // Windows 7: win7_overflow_tilepressed
    "iVBORw0KGgoAAAANSUhEUgAAABUAAAAUCAYAAABiS3YzAAAACXBIWXMAAA7EAAAOxAGVKw4bAAACjElEQVQ4jaWTQWsTQRiGnx1n1p3uJrZWjAgqra1Yiid7"
    "8SB4Fn+A4NWbKIK6UpSIIAgi+Qke/RW9FAV70B7aQg/VNFYUCdboJna33dkZD5pSalRIX3gvH8/3ft8HMx6/dAIYBYboXy2gDrwHmKhWq9PNZnPRWvuxXzeb"
    "zcVqtToNTHjAxXa7/UBrvQUUe9h0X5qmfqlUeiiBQGttrLUJYPYQKrXWg0AgAay1mXMudc7l/SZ6nqestQGABCiKIrPWbgB9hwJKCKG7oQPAlrU222NoIYTY"
    "AgYkEOV5nuV5njrnNnvRr169GarX16LR0eOdc+fOtnoxnuftd85lQCSAkjEmK4oizfP8D8/OzumFhWV948bVywsLy3p2dk734oqiSI0xGVASwAFjzGaWZelu"
    "z8y8DOfnlwbuxNev5MZyJ75+ZX5+aWBm5mXYizfGbAIHPOD5ysrKu/X19Q9pmqbdc168eH3k7du1Q4+fPop3nzp9+/6TsbHjX86fn/rcrWmt9fDw8LHx8fGT"
    "AqgHQUAYhkUURdtuND4djO/dilutFrsd37sVNxqfDu7kwzAsgiAAqEvgu+/7ZSmlVUrZ7uTDh4eTm9fuPmu1kkFjCtmtS7nPDA2Vv01NnUl28lJK6/s+wHcJ"
    "tIUQg0opZ611XejSpQsfJidPfe10Omr3+VEU5SMjR38opbZ5pZQTQgigLYGOlNJTSlnn3PbkSqWyWalUej6xXlJKWSmlB3QksCGE8JRSzjnn/tf8j1AnhPCA"
    "DQkgpfSEEM73/b5DhRDu96ZIIE2SRGmtwzzv/5cqpcIkSRSQSmC1VqutxXF8ulwuW8Db4b/JA9xOJ0kiarVaA1jtNk4AI0DQ96qQAavA8k+kHqPlx2eyZwAA"
    "AABJRU5ErkJggg==",
    // Taskbar background texture.
    "iVBORw0KGgoAAAANSUhEUgAAAAEAAAAnCAYAAADO4CKiAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAH0lEQVQImWNgYGCYyfT///+zTIyMjFeYGBgYGIYYAQDu"
    "0AWHyp0LQgAAAABJRU5ErkJggg==",
    // Reflection texture.
    "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",
    // Start button resting state.
    "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",
    // Start button hover state.
    "iVBORw0KGgoAAAANSUhEUgAAADYAAAA2CAYAAACMRWrdAAAACXBIWXMAAAsTAAALEwEAmpwYAAAZdUlEQVRogbWaWYxlx3nff18tZ7lb316np2frmeGmGZEi"
    "PVJkSw8cGbbiOLETxB4bSRwkcYAAgYA85NEPofiYhwDxi7MAQfwQJAEHiWDFSGLLtoZxaEtwqI2aEanhMvvS271917NVfXnoHoqUKFm25e+iUOfec05V/e7/"
    "qzp1virhR2wK8qddI6A/6no/oI6/uL0H5iC/9AHlXn506fvyvzTIPzfY+2AuIWwhTBDOHP4+RugeNvptlAvAUZQrwBrKOZQXD4qSwwIPS/uRgLo/6w2HQMLz"
    "GCYIqxjA0EU4gTBE6SMsHj1o4OC+8uFTyq2bygDlSZQ34LPnkPMvoVxGOIe+i6aHf8xfEPDPpJgeqGMASwvDYNkP0mgMbYs3arS2sbERb1S9qM6shvLtuNIj"
    "0jqqlEm8wk2OPURHJ4kXjqKfvQjntw8grl56P8yL73XTPyPoDwX2PqBF3GCw6G2WO6PBigYrRIs9qFiDiQBUotG5Rp2oBhcbScJ2OtCp2w0XSuKVTViFeO08"
    "DM6g9/4HuvFzyL0L3wG4BsplOHcJfRfyhwT8U11RD9zOMicZ2oXEjlsuTcoMKdSEkOJ6QlmAfXSDHFRsJJqGqN4ErSXMOpNwNG03g6Sxry+40N3YDQWY6ZSY"
    "vAZLS2jRe1TlgZ0D7p85cNNLoJdB0R8Ozv6gk+9CPUmC6+bash3jGm+brGOMzUxmc2msEzG54BIJJhWcE2M9Rp1YY6UUW/vStStr5okxRRRTL6YysGK2j5dY"
    "i4mrCBcRl2C6Y4x0kRTMV8Gc2UAAY69jnl1GOiA3Aa7A4eDzgfZ9XfFd92vhqRfyeTAdk9uM0lvxISFYJ6hFrEFjgkGxTgioLKwmnP6xBVnbyEJvqWWTVogL"
    "i5m22nUZq31SV+PjcD8fDq/tfu7+fvO5kkW4unCgxGz5O4q0DhWcgI5BUwjlq8TBBeJliN9PvQ8Ee1+f6pKi61lRa8dAW5zkBEnBWiFkiFMiKa22l83za6yd"
    "XHgYW6u///b09Fd369N3p2xU1pu9mpUSwaduu4ha97vpjQ89vvTaL3/6qa9vnmrdKeTWg69M/uWdq821ptw4aOwq6AjIQAvQHHQXYgsC0FyD8P3gvgfs3eH8"
    "Eo5yOWU191oUaZUnXWmaVMQuiNJGbR5VEpw3dvP8MVk9cfz3b1VP/p+71XPSXlx/8swGpzc3WF1dxOcdJE2IzlGJYxYj7+wM+dKb93jl6luMq9lbP33hyP/6"
    "zD946sv347W3vlC/cJsWkR3w+VynbWJnAihx3CWmQwJ9mhs3aGab1JeFyHc96D8Y7Hksa3j8yYxe6Rm7pETbktIyURYRlgTXJe+25bGPnn1tZE7/7s3q+bOb"
    "x/sfe+Ys/U4O1hDE0BhPsJbgPdGnRJ+gaQqZhcRRG8u9wYT/+vJXeeXat+/+ys8d/08/+Xz/ta/Yf/e1Xb4xn4D2KWOhxGIALiPQoilHhGlNPRpTfRCcfA/U"
    "JQxjHCs40uOpWpOWscwkSG6ca0tjFsTIGv0jp+KRs0/+t7fCJ6Wz8NjzH3uSXuqoQ0MTIlEsjfWo8TRJSkhSGp/RpJbGp5Spg8SiHpLMk6ae7dGEf/3f/5BZ"
    "s/fKr/2zs//5Tfv51+74L+8lTOOcJDRUISuIpDTjObpTYnfG1AHmD7aZvPpRmkdw7xsVPwuGHMs6FrvkSI0TEd+I80ZNItGlYmjL2uMnJ6tnP/K5b0//xrnH"
    "Txz78SePYqop5XREnE/QcobWBVrP0VAT6ooYaxqtqENN4SJTaopQUmrJrJkyqaf4NPCTHz2FGHfyNy6/9uFPf+TpndTYare6W+u09M3cu1hb92AS2w/26Q4D"
    "i7tX6c8fULkj1Lefp+Hyd4G9q9aHsNRYdNXTpE6z2kfwjclaamxbVs6sT449ceE/vDb95Z/52GMLx7KGZjxAqxlSTZFyiqlnSDXDNCVSl0gs0VgTNRC0Zu4s"
    "U1NTSkX56KNzijBjVk85u9Hhw2eP9H/9v3z1wifOnbnv/GQybrbDFPLtWhfu78mpvZGcqBNp5adN2VrVWWtK8ake9av//gDsvQ/og4ksGMplwzHYkswk3ltb"
    "BG+JJi6tdfTJj57/jVe2/uYvf+q5dt6MKHZG+GaEUTAxIEREAxhBrUNtSp21cKGN1TmqObUmFA0EqwRVQohw2EVEYRqEbjfnM3/74/3/ePmP/tGv/P1n3U51"
    "+8btZq87F9rNgsW5MEgSZh4kevIwxN1axxw+wA8Ue1etNvbBEFc/c87W9chG8b7BJSJJStrK3bN/7cKvf3nnly7+lafOLHilGI0wYUoyG5AWe/hmTFLtY4sx"
    "YW9EdXeP5v42Mhzg6ilOAuKF7YUOI1tT+YraFNSmoJGCSkoa5tRaUoaCLFOWjvnub732zePt42bp/mxvqZrSci2ZyUimbRt2vOh0Fbalj3kWZmtfQK9dfo9i"
    "V7aQzc1TpsVMxqPKDFqn7IrWtkRsGrD50z9x6pW79dO9I+sfXunljHYf4kPENkpLlbQaEoNl71qNjgqStiHtWmwqyLgm7g+p7z9AnjpNcXSNKlUaiVS2ILqA"
    "So0YCDQokcLMKamYHxlj/XDzTjWyWdK9Mbf7bzpi47pmkrUofWCeG0wcEm6v4BfPEIDoAC5fwpxZROwgyP76gmliZkpJzEMyk1hrQ+Yyf+Tx07/96ls/+08+"
    "/QRbg11sA1kI+KgEUaJJmI0iWtT0N1LcgkVyA4kFb8BA1jSUwwnjyT7lomOWVxTpGJUGkUigpnQFu26XaTJlO9tmaqZ0j3YY3df108vLd4fldGyLZpBpmPRq"
    "t5/lTTGbY5dOEKUNrYcILyDu0ZC/desx4863TLO6Jlv7ziTStR0pTOXw6Yc+ceylb+4/99y5zeVBUVJWFT5GiJDJwbw0Wk85LUhXDG7VI10PLQO5HIIpEi3z"
    "YcZ23GNe36cZLlMuB5p+wdzOGMgWRV1S7sDsdsSf84T1QN3UnLAnfHWnbj22fqYacH1oEimcYE2FJpFYFjhTY7famBc+e+iKV889L+evlebGwBi3smZGnWjy"
    "qGbU9L3Jattf2zz+pVe++al/+FOP8XDrAQQhj4KIozIJCDRYYowkSw5ZSmDBQPsRGGCAOuH1Ycp8saLXC4ymt5jeEaY3LKNQMS4SdseGrSkc9S2KZ65TyIyz"
    "08c5k57hla/ubH7y+LofZtc1EWofqK1HuynV3FP3O9Dvw7XLiHvhBQTWzO3j0DmyaKr23CT7HbO32DUijT3ZO75wbRCOrW+srezMKkZRcGpQ40moiNGhWKrG"
    "YNOI66fQN7BgocshmCAuoncSrruEjfUuJol4MyZJp5hZRTEFaRwuN2RVIFtu2FuZsDxdwU1SXJNxvLXWm+2WSxudYzqubzeaEKvDEf1IhzgCrUDOXULMtWuX"
    "5BrnuLeRmmvpURm1jpubyyfNdtrxu/myq46eWvmd67vnjp1aZ6sKDHHs24SJyylsSmMM86RNVUV8y2AXDNI10JODtACyAPShfBi5u2JZ7i3Q8Yu0fZ9O0qaf"
    "OfqpoZcYWhY6VrAbFR3t0Z+uks471FNhc3mZ11/XEyeTM6lE6iRK7T2NjYTqHpqBcvuA1G2dOyfPLO3JA87a7GjP7Ba5tLqVNaVanxkX+qv9d0Z3n3qq1WZn"
    "WNHYHGc9NQf9bCmOGNPDz94m7Rpix0AHpKvIooEMsArRcSMchbNdummXWQWqSogNja+YJ3OmFaQidIwjHhmzOjtKZ9rHFhllBSt5m4f33Mm222jlIqV6gpvT"
    "pELDBpFd6J04CBi5ycaG/MlkZE6c2JD9rGO8jbZ0qa37wdja2Um60J5avzxxKQPbopAaI8q+DVQxxzjQ3Vv06wmxidx9K/DweosgyvpSwbHnAv7DLWSrzxdW"
    "Ps7xx6akvk2IkaANSZiT2Izcl2QukAlULaHqz7GzPlolSNUm1h6XpDRla7nHgmt1pKpqqeqEhpKDJ/wyEQ7eQR3AbPU5s7UaJNkLZpy3rUlzY613NepmrXZ7"
    "KL69E4RJmjPRnJkRooWbznBL+mxKzhc//hh/eH2bPWlBq01Czd5ol9bn9/i1l7/Fc08t8mbf8CtHV3Ah4G2CDyne5nhJ8NaSmIATIfYjUcAWHUzVwYQEGzOm"
    "U2E6sXmPvlD6ppeGpmoI9TqxGKBNiVbr6MWL4PaLwuw/8YTsbZemt5qaonFmsW58nSVOxSVjsmRbvft/D8YkrYyhMYzTjCZtIUmKTT1l9lHCviFbafH0csbp"
    "juX4gqGdCzcH27z4yh9wq6r5VOcBbdPDEKhMgRWHFYsxFicGb5XcOnS5xMwX2R4rZhDJpjX9puHN0R47E+My34/WNOVUJZARMgitRaKD+CXQB6DmTR4n3Jmb"
    "kFVmXHvTJN4O0rbbk5bf93myF2zSJDlX54GvzyO3JGeUdphlPcYLS+zny1TXhUnVo5dl9DC0ORgQjxn4mY01/s3f+zu0zz/NjfIEn/nNK/zhzTepasWJw4nF"
    "qKGohQc7wht3J8iCwKwF0y7zecatoeVP7kz5+sM5IWaaal57L/VSYpqlSMhuoFsQuQ2Lrz6aBD+Ty17TlXbcNUma2KZ0Lsu9dcEnjcNPrTNNmjQmb/uYpZBl"
    "hLyNZimapHSGgSiCBep4kJoIVXNwrApfeViwWNzj935K+eN7z/Jvf+c1xpNvcXR5TJLUxHpOygzTRNbXUywrVINFZuMcpgZXGuo6qgkpoqZOTbtsJXlDrJq1"
    "SBhtEp64hxYniG+8hV4EdbAJkxsyXT8qYdea9mKwVXRunhorOL8bTDR5WjQ+9eI86jPUJZBmxDShtbfPIBgIyiwo06DMGmFSK+NKyDz80bW3+GTzNY70nuFv"
    "9bt8ejPjxv077G2/QVEOWc4SVtOc7nTA6/5prgxPcH+kbM2hKRvKMip1jcYWmTaTBB2vaaehPQx3byxEX+5re4P4e6AXOQi0OtL7gl8WqsrocluKBhuzxMZo"
    "nRH8IEpIsnxcJmlXrD14FfEWvEVECDs1GlNME5k2ym4Ni5XSKiCxkYDl+v1v86+euwdyAeKYVmo5d7IDGyehcdDchPmE4s4Iu7zG2iQyN57SRK1xlEQKVcRm"
    "LCXze2ki41ZdhnTUi4M416NrR+PulftcugiXLx64ouEOsAC0VyiDtZUGq2VpmyimUbEP61AurPTuBpsQrCV6S3SW6B2tIjAeNmg0mCbSVJH7VeRhpewUyvZM"
    "+db9knn9gI8fTyBsfyfFIcg+mCGYBg3KXFawqdB1hgVnaBtDYhyJtXi8Gpvp0+utq2Y+HJ3Ujma95XikZ+L9e/dpXyQOXkXP8QgMYDoRqtLQ1IZgTBMTA9EF"
    "FXtn3hSrJ47eI0SidQQEFYuKYXF3zrCEJihSBUwVGFeRdwrlTqHcmynfunmfzWSbbq8F4Q7ErfekBxAHEAJalLCwibOR3KCpiZpaS26FBFHrEuy8jr/4iVPf"
    "kOaNnWxsY840bq64+MTGqXiGC3rvAvoohupYLZUyBSOCEamNgxCsjWIQZFDFitXu0BLGdeK7AGoMYgzZ7SlRhVprpKiwjRArx04now4Jg+DR3df5x5u3wLYh"
    "DIAKdAo6hDiGpoIyEqYVsr6ERUksJAKZCFYMRoPa1qIuDof3P/pEfms4+4Nxu2MU0BFGj12/qY8/vsLbHK7aCOqYz5XolPkcEENUQRyhAWw0phG5Pykm62v9"
    "67cK/TFEMFFoh8h4WuNOeeojnmqljSYWGZWY2yPm96FYXOCIvcdfPX7rMNxZH6YSYgUhQB3ReU1dZWgCpkQNESsOI6gDtdZrKE38hY+0/ieyfS/HqOsuxYSB"
    "tu/BxuPn9Aqv6mE0AAEck5PKYhGZz0EWFK+CCUJdGbJUY1C9PWH/Q2uLD931e7Nmod1SicytY/zpTeJaDvVBadIobiUnOd3Fj0sWbtzl2PEFTvd2YObACIhC"
    "VGgUmoiWDXFSEvzqwTyoPGiaEVFiVAKq7b4uD7Yf/tOf/7FXdfKb9yX2Q7NfsrJg9faG1avAxUdqveuKV7cjz/lImgWoDlxyAuREJqXFWhPAbI/ns+V++/bD"
    "efkkqSPEGtIcO2kQD7aKuCaQSSDTQJYHNp8RTlVt3rp7glPlTaw/iDOrHkJVES2VpojI8VUUJUY5CGCrqCGoSTLVQam/+hO93zLh7VvOzau0tDpcSOPWFpxY"
    "swr594S4Hee3FXMmUNWRuo4EE5AsMo1gRIm1pbbu4aycHV/tD7PRZFiGpi+zAtMEXOJxLh6ECiSQaSS3gcTUdM2QPPd840O/ymT3y6yNvkG73sJSoY0hqKdJ"
    "c+x6m/YRRxEaVVDkYE4rYohJN56TO1/5uY8/9tpk9vk7jam0r4n2B0ZZyyNUH7go4bh6VVnqBI72a1LTMKkUrcFiwQmKQaOh060flLN7R/qZ29nee0qWFtt2"
    "VpJ4h7cG44XMKi0XyajpmDmZVCxIhciEYn2dakNI4wCJcywNPhEERVFCjEqjQE2MiRIhtBbi0t2bd//F3z35ecZffL0jZVDSQLuMVO9VqfPuor3Io+H+xReV"
    "vTwwijXlpGIyC0ymkbpRigKiBlI3Q6tJo/X2ftRrZ0+s/bG8eXOeVRXZfI6fzcjLkjxUpKEi0ZpU64NYI5Y5lnk0TGPG2PSYuS4z12USW0xCzqRq66RJtVaJ"
    "EYkB38SsVentG8N//vNLn/P1114n2ZmiMUiMgZgerJre9gqJwsvwndXrQ8VA4UrEXqwhqWBaE0WZzwWfKWmYQzS08hKfNpOqqieJv37x2c2br/7f137WnDp5"
    "1Pc7uLImMYI3Aa8BLw0YS0SYq2EcExLJaELTlAiOBtGAxEo1CiGkIUSxjXFBafAP3tn5zE/3f3fRv/0t6tt7qDSkGmhU0DqyPAgsLz7qXwrv31pxEOJ++WX4"
    "2U8IRc+wv58gVYpPYSFXYlPjbUGoh/hsG6139q3fynrZ3b/+9Ml37n7pa93p/mSjc2yJXCIJjTobaNmGzEU6psG5WoWgKtoEjAZRraPTMnotm5QiJKGMqc4l"
    "CdOdUeyWgzuffrb1xba993XqB3dwocDIDCkrPDPiJLC/GukkEUyEmwroiy++X7EDGe/dC/QWKyimNJoRsGirwntLUEU8pEnljAuYaN6qFJX5vUu/8MnBzW++"
    "8ZU/fvmVn7dnjq3kZ9ZIPRgrGDWhVKP7MUFNTgUUWFrBaxZrtRpUBUWbphrsG0b702c24utH+3aL5p13lGpPTCg0SCGJltS9ChuExTrCOHJ1pJy/pgj63Rth"
    "5H3Hl14y/FSWQr3AYrtNZy159+xir4YUEtUkN6Zja6vNzKzG2PrF9WzjQ4k7/QdfePXjV9958Mm41F5ZOr3CkWMtel0kb9WxJSVtKZt+nGrezEmqmYb9mca9"
    "gbTq6e5TR+Kdc8fcbRjtEne2iTpE4w6Rh7hmiKQTynKC93PqOrB2t+bVhciFV8N7B40PAjv4/sILls7ZlMee9nQqS7po8NbQ7QhlcfByRE7f1Ta1I7Nsg13X"
    "aXIinec/nqYnz3i/OtjePvW1r994ajYZbZR1sZZ3rQmxXmi5EFc7YZCHIq7nYbTal+G5k8n95Xa9g44nxOFApZ4RakTsFGGqsXkoagfYeo4NU6qqZDmvYDty"
    "+VrgEvG7oT4I7OC3l14ydJ512JkjtCy5N5TWYCfCohE6XdpVYU+4gUk7lTnr5q4XBsnx+CA5lpXuRKbt47F7dMk3vUT8QhomiVPpOFcg9TSI1GJlXFmdFcTt"
    "MVpVIIqGEkQxpkJDg9GS2u2RxBHzOCKXhpkpGNuGx+cBXo4fpNZ7+9h7TfmlX4q89NLh6uDxiD1tSHcFb4ViGbhpXJIECGbNj601ddhIYtOeVqEnjTUyrKNe"
    "n86aMtFYplaCl7J24lSc1M5I4ywasaIYoCEeboBpiFKhMRJNjZEGaeYEU9KqIuVSRb0VuZsGHn85flDf+kGKvdctBS4azm8bVj928IqTWuHUlqwOxuZ0Mjen"
    "ViZy3Ozb3nTfbC5E1x8+8EfblevVhfdh4rM0urQpTCJYL1jvK6EG39SCd6AxqsYoXlTVRtG6wfuAhohqIDYVgYaSApPMD9T63/X3U+qHAfvO+ZdeMqyuCt2L"
    "wttXhWdvySmfy8ru2Hzo2Buy6Cf2jMUsmdSum1t2eR5tz0ytt41rSWmzqrQiwSQAqZO0qkBrJcmUsjncHBYDAImLGqURbQ63LlUVHavMKbg9KXn71fj9+tV7"
    "7QfuzHnXLl+GzU349kVl+xpsvsP+fFM/0ptzrNqPSw/nHGt14ip3tTMa6oKGaFpltOUkJplrshAbI9TakiovtYKmRExFoESkUqpSoEJNRWMqyaoaSWpMURNt"
    "STGp2N2v+cg3Aud/sFKP7Ifd1qe8+KLCiwIa4Irw2UWZc1UfXMXQ2wub6zekuZdqk9XR7e8Ze6wrZiAmSafS+CA9gIaDlRd1Cl7Z34doVRZSJY4OlXPK3Cvl"
    "KBBSZbod2V6NXHk18uHv36e+2/6cGzH1oNvyglzimpy7siUbF5+UE9dvmdwfkdS+IadOrIihFLPVlzW2vnNrTA4aFvcO8uAP81Spc4U3oUqU7dXI9svKVZQX"
    "v3fK9JcE9ojv4P4XQM5zSVbZki5PSsZAkutjeRzg8TeBx+D6Yf6o4Y+syJULh7PzK/AXgfnLMAHkhRcwL72EVcXqS1j94vNO9Xn3bv4oPbpGMfoCRhWjijxK"
    "P4oG/X/PYryFIbW3rQAAAABJRU5ErkJggg==",
    // Start button pressed state.
    "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"
};
static void DestroyStartImage(State& state) {
    delete state.win7StartImage;state.win7StartImage=nullptr;
    if(state.win7StartStream)state.win7StartStream->Release();state.win7StartStream=nullptr;
    for(auto& image:state.tintBeta){delete image;image=nullptr;}
    for(size_t i=0;i<std::size(state.betaTexture);++i) {
        delete state.betaTexture[i];state.betaTexture[i]=nullptr;
        if(state.betaTextureStream[i])state.betaTextureStream[i]->Release();
        state.betaTextureStream[i]=nullptr;
    }
    delete state.betaStartImage; state.betaStartImage=nullptr;
    if(state.betaStartStream)state.betaStartStream->Release();
    state.betaStartStream=nullptr;
    delete state.startImage;
    state.startImage=nullptr;
    if(state.startStream) state.startStream->Release();
    state.startStream=nullptr;
}
static bool LoadStartImage(State& state) {
    const auto bytes=DecodeBase64(kStartPngBase64);
    if(bytes.empty() || bytes.size()>0xFFFFFFFFu) return false;
    state.startStream=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
    if(!state.startStream) return false;
    state.startImage=Gdiplus::Image::FromStream(state.startStream,FALSE);
    if(!state.startImage || state.startImage->GetLastStatus()!=Ok ||
       state.startImage->GetWidth()!=50 || state.startImage->GetHeight()!=123) {
        DestroyStartImage(state);
        return false;
    }
    return true;
}
static bool LoadWin7StartImage(State& state) {
    const auto bytes=DecodeBase64(kWin7StartPngBase64);
    if(bytes.empty() || bytes.size()>0xFFFFFFFFu)return false;
    state.win7StartStream=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
    if(!state.win7StartStream)return false;
    state.win7StartImage=Image::FromStream(state.win7StartStream,FALSE);
    if(!state.win7StartImage || state.win7StartImage->GetLastStatus()!=Ok || state.win7StartImage->GetWidth()!=54 || state.win7StartImage->GetHeight()!=162) {
        delete state.win7StartImage;state.win7StartImage=nullptr;state.win7StartStream->Release();state.win7StartStream=nullptr;return false;
    }
    return true;
}
static bool LoadBetaStartImage(State& state) {
    const auto bytes=DecodeBase64(kBetaStartPngBase64);
    if(bytes.empty() || bytes.size()>0xFFFFFFFFu)return false;
    state.betaStartStream=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
    if(!state.betaStartStream)return false;
    state.betaStartImage=Gdiplus::Image::FromStream(state.betaStartStream,FALSE);
    if(!state.betaStartImage || state.betaStartImage->GetLastStatus()!=Ok ||
       state.betaStartImage->GetWidth()!=54 || state.betaStartImage->GetHeight()!=162) {
        delete state.betaStartImage;state.betaStartImage=nullptr;
        state.betaStartStream->Release();state.betaStartStream=nullptr;return false;
    }
    return true;
}
static void LoadBetaTextures(State& state) {
    for(size_t i=0;i<std::size(state.betaTexture);++i) {
        const auto bytes=DecodeBase64(kBetaTextureBase64[i]);
        if(bytes.empty() || bytes.size()>0xFFFFFFFFu)continue;
        IStream* stream=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
        if(!stream)continue;
        Gdiplus::Image* image=Gdiplus::Image::FromStream(stream,FALSE);
        if(!image || image->GetLastStatus()!=Ok) {delete image;stream->Release();continue;}
        state.betaTextureStream[i]=stream;state.betaTexture[i]=image;
    }
}
static bool BetaTextureAvailable(int index) {
    return s && useBetaStartLogo.load() && s->betaStartImage && index>=0 &&
        index<(int)std::size(s->betaTexture) && s->betaTexture[index];
}
static void DrawBetaTexture(Graphics& g,int index,const RECT& rect) {
    if(!BetaTextureAvailable(index) || IsRectEmpty(&rect))return;
    auto* image=s->betaTexture[index];
    g.DrawImage(image,Rect((int)rect.left,(int)rect.top,(int)(rect.right-rect.left),
                (int)(rect.bottom-rect.top)),0,0,(int)image->GetWidth(),
                (int)image->GetHeight(),UnitPixel);
}
// Recolor chromatic pixels of the beta art while preserving its neutral tile.
// Cache the bitmap per source and RGB value.
static Image* CustomBetaStart(Image* src,int slot) {
    if(!src || !s || !customStartColor.load() || taskbarSkin.load()!=4)return src;
    int key=startLogoRgb.load();
    if(s->tintBeta[slot] && s->tintBetaKey[slot]==key && s->tintBetaSource[slot]==src)return s->tintBeta[slot];
    delete s->tintBeta[slot];s->tintBeta[slot]=nullptr;s->tintBetaKey[slot]=-1;
    int w=(int)src->GetWidth(),h=(int)src->GetHeight();
    if(w<=0 || h<=0 || w>256 || h>512)return src;
    auto bitmap=std::make_unique<Bitmap>(w,h,PixelFormat32bppARGB);
    if(bitmap->GetLastStatus()!=Ok)return src;
    {Graphics g(bitmap.get());g.SetCompositingMode(CompositingModeSourceCopy);g.DrawImage(src,Rect(0,0,w,h),0,0,w,h,UnitPixel);}
    for(int y=0;y<h;y++)for(int x=0;x<w;x++) {
        Color c;if(bitmap->GetPixel(x,y,&c)!=Ok || !c.GetA())continue;
        int hi=std::max({(int)c.GetR(),(int)c.GetG(),(int)c.GetB()});
        int lo=std::min({(int)c.GetR(),(int)c.GetG(),(int)c.GetB()});
        if(hi-lo<14 || hi<45)continue;
        float coverage=std::min(1.f,(hi-lo)/80.f);
        int red=(int)std::lround(c.GetR()*(1-coverage)+((key>>16)&255)*coverage);
        int green=(int)std::lround(c.GetG()*(1-coverage)+((key>>8)&255)*coverage);
        int blue=(int)std::lround(c.GetB()*(1-coverage)+(key&255)*coverage);
        bitmap->SetPixel(x,y,Color(c.GetA(),red,green,blue));
    }
    s->tintBetaKey[slot]=key;s->tintBetaSource[slot]=src;s->tintBeta[slot]=bitmap.release();return s->tintBeta[slot];
}
static Color StartCustomColor() {int v=startLogoRgb.load();return Color(255,(v>>16)&255,(v>>8)&255,v&255);}
static void DrawWin7Start(Graphics& g,int w,int h,int frame,int spriteSize=0) {
    if(!s || (!s->win7StartImage && !s->betaTexture[30]) || w<=0 || h<=0)return;
    // Use the standard size unless the caller supplies an alternate size.
    REAL side=(REAL)(spriteSize>0?spriteSize:Px(54));RectF dst(((REAL)w-side)/2.f,((REAL)h-side)/2.f,side,side);
    auto save=g.Save();g.SetClip(Rect(0,0,w,h),CombineModeIntersect);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    // Styler ships three separate 54x54 files. The older three-frame image
    // is retained solely as a fallback if a resource fails to load.
    auto drawFrame=[&](int index,float alpha) {
        Image* src=s->betaTexture[30+index];REAL sourceY=0.f;
        if(!src) {src=s->win7StartImage;sourceY=(REAL)(54*index);}
        if(!src)return;
        src=CustomBetaStart(src,0);
        if(alpha>=1.f)g.DrawImage(src,dst,0.f,sourceY,54.f,54.f,UnitPixel);
        else if(alpha>0.f) {
            ImageAttributes attr;ColorMatrix matrix={{{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,alpha,0},{0,0,0,0,1}}};
            attr.SetColorMatrix(&matrix);g.DrawImage(src,dst,0.f,sourceY,54.f,54.f,UnitPixel,&attr);
        }
    };
    if(frame==2)drawFrame(2,1.f);
    else {drawFrame(0,1.f);drawFrame(1,std::max(0.f,std::min(1.f,s->startFade)));}
    g.Restore(save);
}
static void DrawBetaStartImageVertical(Graphics& g,int w,int h,int frame) {
    if(!s || !s->betaStartImage || w<=0 || h<=0)return;
    // Fit the same 54x38 artwork used horizontally to the vertical bar width.
    // Keep it upright, proportionate and centred in the Start cell.
    const float scale=std::min((float)w/54.f,(float)h/38.f);
    const float width=54.f*scale,height=38.f*scale;
    RectF dst(((float)w-width)/2.f,((float)h-height)/2.f,width,height);
    auto saved=g.Save();g.SetClip(Rect(0,0,w,h),CombineModeIntersect);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.DrawImage(CustomBetaStart(s->betaStartImage,0),dst,0.f,(float)(54*frame+9),54.f,38.f,UnitPixel);
    g.Restore(saved);
}
static void DrawBetaStartImage(Graphics& g,int w,int h,int frame) {
    if(!s || !s->betaStartImage || w<=0 || h<=0)return;
    // All three 54x54 frames have identical transparent margins at the top
    // and bottom (rows 0..8 and 47..53). Crop those margins and scale the
    // visible 54x38 artwork to the bar height, preserving its aspect ratio.
    // Clip to the Start cell so it never paints over adjacent task buttons.
    const float width=(float)h*54.f/38.f;
    RectF dst(((float)w-width)/2.f,0.f,width,(float)h);
    auto saved=g.Save();
    g.SetClip(Rect(0,0,w,h),CombineModeIntersect);
    if(taskbarSkin.load()==2 && h<Px(38) && dst.X>0.f) {
        // The source sprite's left edge is RGB 51 in normal/hover and
        // RGB 70 in pressed state; preserve the actual artwork color.
        BYTE shade=frame==2?70:51;SolidBrush edge(Color(255,shade,shade,shade));
        g.FillRectangle(&edge,0,0,(int)std::ceil(dst.X),h);
    }
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.DrawImage(CustomBetaStart(s->betaStartImage,0),dst,0.f,(float)(54*frame+9),54.f,38.f,UnitPixel);
    g.Restore(saved);
}
Color AccentBase();
// Match the bar being painted, not a hard-coded Windows-blue logo. If the
// colour is too dark on the black tile, brighten while retaining its hue.
static Color StartHoverColor() {
    Color c=AccentBase();
    if(s && matchNativeTray.load() && !s->trayColors.empty()) {
        COLORREF native=s->trayColors[s->trayColors.size()/2];
        c=Color(255,GetRValue(native),GetGValue(native),GetBValue(native));
    }
    int r=c.GetR(), green=c.GetG(), b=c.GetB(), peak=std::max({r,green,b});
    if(peak<145) {
        if(peak==0) return Color(255,55,145,230);
        r=std::min(255,(r*175+peak/2)/peak);green=std::min(255,(green*175+peak/2)/peak);b=std::min(255,(b*175+peak/2)/peak);
    }
    return Color(255,r,green,b);
}
static void DrawStartImage(Graphics& g, int w, int h, int frame, float opacity=1.0f, const Color* tint=nullptr) {
    if(!s || !s->startImage || w<=0 || h<=0 || opacity<=0) return;
    const float scale=std::min((float)w/50.0f,(float)h/41.0f);
    const float drawW=50.0f*scale, drawH=41.0f*scale;
    // The normal-height logo is unchanged. In compact bars, reduce the
    // old leftward bias to centre the logo inside its dark hover tile.
    const float bias=(h<=Px(32))?2.0f:4.0f;
    const RectF dst(((float)w-drawW)/2.0f-bias*scale,((float)h-drawH)/2.0f,drawW,drawH);
    // Extend only the dark hover tile to the left edge, including in compact
    // bars where the aspect-preserving sprite would otherwise leave a gap.
    if(frame>0 && dst.X>0) {SolidBrush extension(Color((BYTE)std::lround(255*std::min(1.f,opacity)),17,17,17));g.FillRectangle(&extension,0,0,(int)std::ceil(dst.X)+1,h);}
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    ImageAttributes attr; ColorMatrix matrix={{{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,opacity,0},{0,0,0,0,1}}};
    if(tint && frame==0) {matrix.m[0][0]=tint->GetR()/255.f;matrix.m[1][1]=tint->GetG()/255.f;matrix.m[2][2]=tint->GetB()/255.f;}
    attr.SetColorMatrix(&matrix);
    g.DrawImage(s->startImage,dst,0.0f,(float)(41*frame),50.0f,41.0f,UnitPixel,&attr);
}

static bool IsModOrShellSurface(HWND hwnd) {
    return hwnd==barWindow.load() || hwnd==GetShellWindow() || hwnd==GetDesktopWindow();
}
static bool HasTaskbarExtendedStyle(HWND hwnd) {
    const DWORD style=(DWORD)GetWindowLongPtrW(hwnd,GWL_EXSTYLE);
    const bool forcedAppWindow=(style&WS_EX_APPWINDOW)!=0;
    if(!forcedAppWindow && GetWindow(hwnd,GW_OWNER)!=nullptr)return false;
    return (style&(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE))==0;
}
static bool IsCloakedTaskWindow(HWND hwnd) {
    DWORD cloaked=0;
    const HRESULT hr=DwmGetWindowAttribute(hwnd,DWMWA_CLOAKED,&cloaked,sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked!=0;
}
std::wstring ProcessImage(DWORD pid);
// Inspired by RetroBar's ManagedShell immersive-shell filter. Keep the
// class/process conjunction deliberately narrow: CoreWindow and XAML are also
// used by real apps, so never exclude those classes globally. Shell window
// implementation details can change across Windows builds.
static bool IsNotificationShellSurface(HWND hwnd,const wchar_t* cls) {
    bool core=!_wcsicmp(cls,L"Windows.UI.Core.CoreWindow");
    bool control=!_wcsicmp(cls,L"ControlCenterWindow");
    if(!core && !control)return false;
    DWORD pid=0;GetWindowThreadProcessId(hwnd,&pid);if(!pid)return false;
    std::wstring path=ProcessImage(pid);if(path.empty())return false;
    size_t slash=path.find_last_of(L"\\/");
    const wchar_t* exe=path.c_str()+(slash==std::wstring::npos?0:slash+1);
    bool match=core && (!_wcsicmp(exe,L"ShellExperienceHost.exe") || !_wcsicmp(exe,L"ShellHost.exe"));
    match=match || (control && !_wcsicmp(exe,L"ShellHost.exe"));
    if(match) {
        // This filter changes task enumeration only; it never hides or closes
        // the notification, the notification center, or the underlying app.
        static HWND last=nullptr;static DWORD lastPid=0;
        if(last!=hwnd || lastPid!=pid) {
            Wh_Log(L"[T81-NOTIFY] excluded shell surface hwnd=%p pid=%lu class=%s process=%s",hwnd,pid,cls,exe);
            last=hwnd;lastPid=pid;
        }
    }
    return match;
}
bool IsTaskWindow(HWND hwnd) {
    if(!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsModOrShellSurface(hwnd))return false;
    wchar_t className[64]{};
    if(!GetClassNameW(hwnd,className,(int)std::size(className)))return false;
    static const wchar_t* const excludedClasses[]={
        L"Progman",L"WorkerW",L"Shell_TrayWnd",L"Shell_SecondaryTrayWnd",kClass
    };
    for(const wchar_t* excluded:excludedClasses)
        if(!wcscmp(className,excluded))return false;
    // Legacy shell charms can also be hosted by rundll32 in the native path.
    // They are shell surfaces rather than application taskbar buttons.
    if(!wcscmp(className,L"Shell_CharmWindow"))return false;
    if(!HasTaskbarExtendedStyle(hwnd))return false;
    if(IsCloakedTaskWindow(hwnd))return false;
    if(IsNotificationShellSurface(hwnd,className))return false;
    return true;
}
bool GetTitle(HWND h, std::wstring& out) {
    wchar_t title[256] = {};
    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(h, WM_GETTEXT, 256, (LPARAM)title,
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 30, &ignored)) return false;
    title[255] = 0;
    out.assign(title);
    return true;
}
HICON GetIcon(HWND h) {
    DWORD_PTR p = 0;
    // Prefer a large application icon so 24 logical pixels are not enlarged
    // from a 16px SMALLICON. Handles here belong to the application/class.
    SendMessageTimeoutW(h, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 30, &p);
    if (!p) p = (DWORD_PTR)GetClassLongPtrW(h, GCLP_HICON);
    if (!p) SendMessageTimeoutW(h, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 30, &p);
    if (!p) SendMessageTimeoutW(h, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 30, &p);
    if (!p) p = (DWORD_PTR)GetClassLongPtrW(h, GCLP_HICONSM);
    return p ? (HICON)p : LoadIconW(nullptr, MAKEINTRESOURCEW(32512));
}
/* Application identity handling: process image paths and the
 * AUMID/path/fallback identity branches. GPL-3.0-or-later. Control Panel
 * recognition is selectively adapted below; Toolhelp process-name fallback is
 * not ported. */
std::wstring ProcessImage(DWORD pid) {
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if (!h) return {}; ScopedHandle guard(h);
    wchar_t buffer[1024] = {};
    DWORD n = 1024;
    std::wstring result;
    if (QueryFullProcessImageNameW(h,0,buffer,&n)) result.assign(buffer,n);
    return result;
}
// Only documented, read-only identity queries. GetApplicationUserModelId
// normally supplies package identity; Win32 processes may return no identity.
// See https://learn.microsoft.com/windows/win32/shell/appids
static std::wstring ProcessApplicationId(DWORD pid) {
    using GetId=LONG (WINAPI*)(HANDLE,UINT32*,PWSTR);
    HMODULE kernel=GetModuleHandleW(L"kernel32.dll");
    auto fn=kernel?reinterpret_cast<GetId>(GetProcAddress(kernel,"GetApplicationUserModelId")):nullptr;
    if(!fn || !pid) return {};
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    if(!process) return {}; ScopedHandle guard(process);
    UINT32 length=0;
    LONG status=fn(process,&length,nullptr);
    std::wstring value;
    if(status==ERROR_INSUFFICIENT_BUFFER && length>1 && length<=1024) {
        std::vector<wchar_t> buf(length,L'\0');
        if(fn(process,&length,buf.data())==ERROR_SUCCESS && length<=buf.size() && buf[0])
            value.assign(buf.data());
    }
    return value;
}

static std::wstring NormalizedMatchPath(const std::wstring& path) {
    if(path.empty() || path.size()>=32768) return {};
    wchar_t expanded[32768]={};
    DWORD needed=ExpandEnvironmentStringsW(path.c_str(),expanded,(DWORD)std::size(expanded));
    if(!needed || needed>std::size(expanded)) return {};
    PathUnquoteSpacesW(expanded);
    // Never probe UNC/network paths or infer a relative path from Explorer's
    // current directory. Exact comparisons still work when already absolute.
    if(PathIsRelativeW(expanded)) return {};
    std::wstring result(expanded);
    if(result.rfind(L"\\\\",0)!=0) {
        wchar_t full[32768]={};
        DWORD n=GetFullPathNameW(result.c_str(),(DWORD)std::size(full),full,nullptr);
        if(n && n<std::size(full)) result=full;
        wchar_t longName[32768]={};
        n=GetLongPathNameW(result.c_str(),longName,(DWORD)std::size(longName));
        if(n && n<std::size(longName)) result=longName;
    }
    std::transform(result.begin(),result.end(),result.begin(),
        [](wchar_t c){return static_cast<wchar_t>(towlower(c));});
    return result;
}

static std::wstring RelaunchExecutable(const std::wstring& command) {
    if(command.empty() || command.size()>32767) return {};
    int argc=0; LPWSTR* argv=CommandLineToArgvW(command.c_str(),&argc);
    if(!argv) return {};
    std::wstring path=(argc>0 && argv[0])?NormalizedMatchPath(argv[0]):L"";
    LocalFree(argv);
    return path;
}

struct HostedIdentity { DWORD hostPid; unsigned scanned=0; std::wstring id; bool ambiguous=false; };
static BOOL CALLBACK FindHostedIdentity(HWND child, LPARAM arg) {
    auto* context=reinterpret_cast<HostedIdentity*>(arg);
    try { if(++context->scanned>64) return FALSE;
    DWORD pid=0; GetWindowThreadProcessId(child,&pid);
    if(!pid || pid==context->hostPid) return TRUE;
    std::wstring id=ProcessApplicationId(pid);
    if(id.empty()) return TRUE;
    std::transform(id.begin(),id.end(),id.begin(),
        [](wchar_t c){return static_cast<wchar_t>(towlower(c));});
    if(context->id.empty()) context->id=std::move(id);
    else if(context->id!=id) { context->ambiguous=true; return FALSE; }
    return TRUE;
    } catch(...) { context->ambiguous=true; Wh_Log(L"[T81-DIAG] hosted identity C++ exception"); return FALSE; }
}

static std::wstring WindowIdentity(HWND h, std::wstring* relaunchPath=nullptr) {
    static const PROPERTYKEY appKey={{0x9f4c2855,0x9f79,0x4b39,{0xa8,0xd0,0xe1,0xd4,0x2d,0xe1,0xd5,0xf3}},5};
    static const PROPERTYKEY relaunchKey={{0x9f4c2855,0x9f79,0x4b39,{0xa8,0xd0,0xe1,0xd4,0x2d,0xe1,0xd5,0xf3}},2};
    if(relaunchPath) relaunchPath->clear();
    std::wstring result;
    IPropertyStore* rawStore=nullptr; HRESULT storeHr=SHGetPropertyStoreForWindow(h,IID_PPV_ARGS(&rawStore));
    std::unique_ptr<IPropertyStore,ReleaseCom<IPropertyStore>> store(rawStore);
    if(SUCCEEDED(storeHr) && store) {
        ScopedPropVariant app;
        if(SUCCEEDED(store->GetValue(appKey,&app.value)) && app.value.vt==VT_LPWSTR && app.value.pwszVal && app.value.pwszVal[0])
            result=std::wstring(L"app:")+app.value.pwszVal;
        if(relaunchPath) {
            ScopedPropVariant relaunch;
            if(SUCCEEDED(store->GetValue(relaunchKey,&relaunch.value)) && relaunch.value.vt==VT_LPWSTR && relaunch.value.pwszVal)
                *relaunchPath=RelaunchExecutable(relaunch.value.pwszVal);
        }
    }
    DWORD pid=0; GetWindowThreadProcessId(h,&pid);
    // Control Panel identity recognition (GPL-3.0-or-later):
    // only control.exe or Explorer Cabinet windows with positive shell evidence.
    std::wstring image=NormalizedMatchPath(ProcessImage(pid));
    auto slash=image.find_last_of(L"\\/");
    std::wstring base=slash==std::wstring::npos?image:image.substr(slash+1);
    bool control=(base==L"control.exe");
    if(!control && base==L"explorer.exe") {
        wchar_t cls[64]={}; GetClassNameW(h,cls,64);
        if(!_wcsicmp(cls,L"CabinetWClass")) {
            std::wstring command;
            if(store) { ScopedPropVariant value; if(SUCCEEDED(store->GetValue(relaunchKey,&value.value)) && value.value.vt==VT_LPWSTR && value.value.pwszVal) command=value.value.pwszVal; }
            std::transform(command.begin(),command.end(),command.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
            std::wstring id=result;
            std::transform(id.begin(),id.end(),id.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
            std::wstring caption; GetTitle(h,caption); // bounded cross-thread WM_GETTEXT
            control=command.find(L"{26ee0668-a00a-44d7-9371-beb064c98683}")!=std::wstring::npos ||
                command.find(L"{21ec2020-3aea-1069-a2dd-08002b30309d}")!=std::wstring::npos ||
                id.find(L"controlpanel")!=std::wstring::npos ||
                !_wcsicmp(caption.c_str(),L"Control Panel") || !_wcsicmp(caption.c_str(),L"Pannello di controllo");
        }
    }
    if(control) { if(relaunchPath) relaunchPath->clear(); return L"app:taskbar81.controlpanel"; }
    if(result.empty()) {
        std::wstring app=ProcessApplicationId(pid);
        if(!app.empty()) result=L"app:"+app;
    }
    if(result.empty()) {
        wchar_t className[80]={}; GetClassNameW(h,className,(int)std::size(className));
        if(!wcscmp(className,L"ApplicationFrameWindow")) {
            HostedIdentity host{}; host.hostPid=pid;
            EnumChildWindows(h,FindHostedIdentity,reinterpret_cast<LPARAM>(&host));
            if(!host.ambiguous && !host.id.empty()) result=L"app:"+host.id;
        }
    }
    if(result.empty()) {
        std::wstring exe=NormalizedMatchPath(ProcessImage(pid));
        result=exe.empty()?L"pid:"+std::to_wstring(pid):L"exe:"+exe;
    }
    std::transform(result.begin(),result.end(),result.begin(),
        [](wchar_t c){return static_cast<wchar_t>(towlower(c));});
    return result;
}

// Taskband Favorites is an undocumented on-disk registry representation,
// not a stable Microsoft API. The observed format can provide pinned-item
// order without changing the registry. Unknown/truncated data is rejected;
// unresolved entries fall back to folder enumeration. Running unpinned apps
// are NOT covered by Favorites and their exact native order remains unknown.
// Format observation: https://github.com/AppifySheets/Deskbar/pull/193
// This implementation is independent; no source code from that repository
// is incorporated (that repository has its own non-GPL license).
struct FavoriteEntry {
    std::vector<BYTE> pidl;
    std::wstring parsingName;
    std::wstring displayName;
    std::wstring appId;
    std::wstring targetPath;
};

static bool ValidateAbsolutePidlBytes(const BYTE* data, size_t length) {
    if(!data || length<2 || length>65536) return false;
    size_t offset=0;
    while(offset+2<=length) {
        unsigned segment=(unsigned)data[offset] | ((unsigned)data[offset+1]<<8);
        if(segment==0) return offset+2==length;
        if(segment<2 || segment>length-offset) return false;
        offset+=segment;
    }
    return false;
}

static bool SplitFavoriteBytes(const std::vector<BYTE>& blob,
                               std::vector<FavoriteEntry>& entries) {
    entries.clear();
    size_t at=0;
    while(at<blob.size()) {
        // A single FF byte is the observed end marker, including for an
        // empty list. Do not treat arbitrary extra trailing bytes as valid.
        if(blob[at]==0xFF) return at+1==blob.size();
        if(blob.size()-at<5 || entries.size()>=MAX_TASKS) return false;
        uint32_t size=(uint32_t)blob[at+1] | ((uint32_t)blob[at+2]<<8) |
                      ((uint32_t)blob[at+3]<<16) | ((uint32_t)blob[at+4]<<24);
        size_t payload=at+5;
        if(size<2 || size>65536 || (size_t)size>blob.size()-payload ||
           !ValidateAbsolutePidlBytes(blob.data()+payload,size)) return false;
        FavoriteEntry entry;
        entry.pidl.assign(blob.begin()+payload,blob.begin()+payload+size);
        entries.push_back(std::move(entry));
        at=payload+size;
    }
    // Some observed lists end immediately after the last item.
    return true;
}

static bool ReadTaskbandFavorites(std::vector<BYTE>& bytes) {
    bytes.clear();
    HKEY key=nullptr;
    if(RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband",
        0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS) return false;
    DWORD type=0,size=0;
    LSTATUS result=RegQueryValueExW(key,L"Favorites",nullptr,&type,nullptr,&size);
    if(result!=ERROR_SUCCESS || type!=REG_BINARY || size<1 || size>1024*1024) {
        RegCloseKey(key); return false;
    }
    bytes.resize(size);
    result=RegQueryValueExW(key,L"Favorites",nullptr,&type,bytes.data(),&size);
    RegCloseKey(key);
    if(result!=ERROR_SUCCESS || type!=REG_BINARY || size>bytes.size()) { bytes.clear(); return false; }
    bytes.resize(size);
    return true;
}

static bool ReadFavoritesVersion(DWORD& version) {
    DWORD size=sizeof(version),type=0;
    return RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband",
        L"FavoritesChanges",RRF_RT_REG_DWORD,&type,&version,&size)==ERROR_SUCCESS;
}

static bool HasFavoritesVersionChanged() {
    DWORD version=0;
    if(!ReadFavoritesVersion(version)) return false;
    if(!s->favoritesVersionKnown || s->favoritesVersion!=version) {
        s->favoritesVersionKnown=true;
        s->favoritesVersion=version;
        return true;
    }
    return false;
}

static std::wstring ItemDisplayName(IShellItem2* item, SIGDN kind) {
    PWSTR value=nullptr;
    std::wstring result;
    if(item && SUCCEEDED(item->GetDisplayName(kind,&value)) && value) result=value;
    if(value) CoTaskMemFree(value);
    return result;
}

static std::wstring ItemStringProperty(IShellItem2* item, REFPROPERTYKEY key) {
    PWSTR value=nullptr;
    std::wstring result;
    if(item && SUCCEEDED(item->GetString(key,&value)) && value) result=value;
    if(value) CoTaskMemFree(value);
    return result;
}

static bool DescribeFavorite(FavoriteEntry& entry) {
    static const PROPERTYKEY appIdKey={{0x9f4c2855,0x9f79,0x4b39,
        {0xa8,0xd0,0xe1,0xd4,0x2d,0xe1,0xd5,0xf3}},5};
    static const PROPERTYKEY targetKey={{0xb9b4b3fc,0x2b51,0x4a42,
        {0xb5,0xd8,0x32,0x41,0x46,0xaf,0xcf,0x25}},2};
    if(entry.pidl.empty()) return false;
    IShellItem2* item=nullptr;
    HRESULT hr=SHCreateItemFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(entry.pidl.data()),
                                      IID_PPV_ARGS(&item));
    if(FAILED(hr) || !item) return false;
    entry.parsingName=ItemDisplayName(item,SIGDN_DESKTOPABSOLUTEPARSING);
    std::wstring filePath=ItemDisplayName(item,SIGDN_FILESYSPATH);
    if(!filePath.empty()) entry.parsingName=filePath;
    entry.displayName=ItemDisplayName(item,SIGDN_NORMALDISPLAY);
    entry.appId=ItemStringProperty(item,appIdKey);
    entry.targetPath=ItemStringProperty(item,targetKey);
    item->Release();
    return !entry.parsingName.empty() || !entry.appId.empty();
}

static bool EqualNoCase(const std::wstring& a, const std::wstring& b) {
    return !a.empty() && !b.empty() && _wcsicmp(a.c_str(),b.c_str())==0;
}

static size_t MatchDiscoveredPin(const FavoriteEntry& favorite,
                                 const std::vector<w7t::PinnedApp>& pins,
                                 const std::vector<bool>& used) {
    // Exact shell shortcut path first, then unambiguous app-id and target.
    for(size_t i=0;i<pins.size();i++)
        if(!used[i] && EqualNoCase(favorite.parsingName,pins[i].lnkPath)) return i;
    size_t match=pins.size();
    for(size_t i=0;i<pins.size();i++) {
        if(used[i]) continue;
        bool same=(pins[i].hasAppId && EqualNoCase(favorite.appId,pins[i].identity)) ||
                  EqualNoCase(favorite.targetPath,pins[i].target);
        if(!same) continue;
        if(match!=pins.size()) return pins.size(); // ambiguous; do not guess
        match=i;
    }
    return match;
}

static bool OrderPinsFromFavorites(std::vector<w7t::PinnedApp>& pins,
                                   const std::vector<BYTE>& bytes) {
    std::vector<FavoriteEntry> favorites;
    if(!SplitFavoriteBytes(bytes,favorites)) return false;
    std::vector<bool> used(pins.size(),false);
    std::vector<w7t::PinnedApp> ordered;
    bool unresolved=false;
    for(auto& favorite:favorites) {
        if(!DescribeFavorite(favorite)) { unresolved=true; continue; }
        size_t index=MatchDiscoveredPin(favorite,pins,used);
        if(index<pins.size()) {
            ordered.push_back(pins[index]);
            used[index]=true;
            continue;
        }
        if(!favorite.appId.empty()) {
            bool already=false;
            for(const auto& current:ordered)
                if(current.hasAppId && EqualNoCase(current.identity,favorite.appId)) { already=true; break; }
            if(!already) {
                w7t::PinnedApp installed;
                installed.identity=favorite.appId;
                installed.lnkPath=L"shell:AppsFolder\\"+favorite.appId;
                installed.target=favorite.targetPath;
                installed.displayName=favorite.displayName.empty()?favorite.appId:favorite.displayName;
                installed.hasAppId=true;
                ordered.push_back(std::move(installed));
            }
        } else {
            unresolved=true;
        }
    }
    // Preserve discovered pins only if the snapshot could not be completely
    // interpreted. In that case their relative order is only a fallback.
    const std::wstring modDir=w7t::ModPinsFolder();
    const std::wstring modPrefix=modDir.empty()?L"":modDir+L"\\";
    for(size_t i=0;i<pins.size();i++) if(!used[i]) {
        bool owned=!modPrefix.empty() && pins[i].lnkPath.size()>modPrefix.size() &&
            !_wcsnicmp(pins[i].lnkPath.c_str(),modPrefix.c_str(),modPrefix.size());
        if(unresolved || owned)ordered.push_back(pins[i]);
    }
    pins.swap(ordered);
    return true;
}

void FreePins() {
    if (!s) return;
    for(auto& pin:s->pins) if(pin.icon) DestroyIcon(pin.icon);
    s->pins.clear();
}

// Obtain an icon from a shell namespace item (including AppsFolder entries)
// through the documented image factory. Convert its alpha bitmap to an owned
// HICON so the UI can draw a properly sized image without a shortcut badge.
static HICON ShellItemIcon(const std::wstring& parsingName, int pixels) {
    if(parsingName.empty() || pixels<=0 || pixels>256) return nullptr;
    IShellItemImageFactory* factory=nullptr;
    HRESULT hr=SHCreateItemFromParsingName(parsingName.c_str(),nullptr,IID_PPV_ARGS(&factory));
    if(FAILED(hr) || !factory) return nullptr;
    HBITMAP bitmap=nullptr;
    SIZE requested{pixels,pixels};
    hr=factory->GetImage(requested,(SIIGBF)(SIIGBF_ICONONLY|SIIGBF_BIGGERSIZEOK),&bitmap);
    factory->Release();
    if(FAILED(hr) || !bitmap) return nullptr;
    BITMAP dimensions{};
    HICON result=nullptr;
    if(GetObjectW(bitmap,sizeof(dimensions),&dimensions) && dimensions.bmWidth>0 && dimensions.bmHeight>0 &&
       dimensions.bmWidth<=1024 && dimensions.bmHeight<=1024) {
        const size_t stride=((size_t)dimensions.bmWidth+15)/16*2;
        std::vector<BYTE> maskBits(stride*(size_t)dimensions.bmHeight,0);
        HBITMAP mask=CreateBitmap(dimensions.bmWidth,dimensions.bmHeight,1,1,maskBits.data());
        if(mask) {
            ICONINFO info{};
            info.fIcon=TRUE;
            info.hbmColor=bitmap;
            info.hbmMask=mask;
            result=CreateIconIndirect(&info);
            DeleteObject(mask);
        }
    }
    DeleteObject(bitmap);
    return result;
}

static std::wstring EncodeOrder(const std::vector<std::wstring>& keys);
static bool DecodeOrder(const std::wstring& encoded,std::vector<std::wstring>& result);
static std::vector<std::wstring> ReadHiddenModPins() {
    std::vector<wchar_t> buf(32768);std::vector<std::wstring> keys;
    size_t n=Wh_GetStringValue(L"Taskbar81.HiddenPins",buf.data(),buf.size());
    if(n && n<buf.size() && !DecodeOrder(std::wstring(buf.data(),n),keys)) {
        keys.clear();Wh_Log(L"[T81-PIN] invalid hidden pin list");
    }
    return keys;
}
static bool SetModPinHidden(const std::wstring& key,bool hidden) {
    if(key.rfind(L"app:",0)!=0 && key.rfind(L"exe:",0)!=0)return false;
    auto keys=ReadHiddenModPins();
    keys.erase(std::remove(keys.begin(),keys.end(),key),keys.end());
    if(hidden) {if(keys.size()>=MAX_TASKS)return false;keys.push_back(key);}
    std::wstring encoded=EncodeOrder(keys);
    std::vector<std::wstring> check;
    if(!DecodeOrder(encoded,check) || check!=keys)return false;
    return Wh_SetStringValue(L"Taskbar81.HiddenPins",encoded.c_str())!=FALSE;
}
void RefreshPins() {
    std::vector<w7t::PinnedApp> discovered=w7t::LoadPinnedApps();
    std::vector<BYTE> favoritesBytes;
    if(ReadTaskbandFavorites(favoritesBytes)) {
        if(!OrderPinsFromFavorites(discovered,favoritesBytes))
            Wh_Log(L"Unrecognized Taskband Favorites data; using folder enumeration order");
    }
    const auto hidden=ReadHiddenModPins();
    std::vector<Pin> next;
    for(const auto& item:discovered) {
        if(next.size()>=MAX_TASKS) break;
        Pin pin;
        pin.key=(item.hasAppId?L"app:":L"exe:")+item.identity;
        std::transform(pin.key.begin(),pin.key.end(),pin.key.begin(),
            [](wchar_t c){return static_cast<wchar_t>(towlower(c));});
        if(std::find(hidden.begin(),hidden.end(),pin.key)!=hidden.end())continue;
        pin.path=item.lnkPath;
        pin.title=item.displayName;
        pin.target=NormalizedMatchPath(item.target);
        // Prefer an explicitly assigned shortcut icon; do not add a link badge.
        SHFILEINFOW fi{};
        if(!pin.icon) {
            IShellLinkW* link=nullptr;
            if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,
                                          IID_PPV_ARGS(&link))) && link) {
                IPersistFile* file=nullptr;
                if(SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file))) && file) {
                    if(SUCCEEDED(file->Load(item.lnkPath.c_str(),STGM_READ))) {
                        wchar_t raw[MAX_PATH]={}, expanded[MAX_PATH]={}; int index=0;
                        if(SUCCEEDED(link->GetIconLocation(raw,MAX_PATH,&index)) && raw[0]) {
                            DWORD n=ExpandEnvironmentStringsW(raw,expanded,MAX_PATH);
                            if(n>0 && n<=MAX_PATH) ExtractIconExW(expanded,index,&pin.icon,nullptr,1);
                        }
                    }
                    file->Release();
                }
                link->Release();
            }
        }
        if(!pin.icon && item.hasAppId && w7t::IsPackagedAumid(item.identity))
            pin.icon=ShellItemIcon(std::wstring(L"shell:AppsFolder\\")+item.identity,48);
        if(!pin.icon && !item.target.empty() &&
           SHGetFileInfoW(item.target.c_str(),0,&fi,sizeof(fi),SHGFI_ICON|SHGFI_LARGEICON))
            pin.icon=fi.hIcon;
        if(!pin.icon && item.hasAppId)
            pin.icon=ShellItemIcon(std::wstring(L"shell:AppsFolder\\")+item.identity,32);
        if(!pin.icon && SHGetFileInfoW(item.lnkPath.c_str(),0,&fi,sizeof(fi),SHGFI_ICON|SHGFI_LARGEICON))
            pin.icon=fi.hIcon;
        next.push_back(std::move(pin));
    }
    FreePins();
    s->pins.swap(next);
}

// Mod-local order override. The binary taskbar order is used until the user
// actually drags an item. Saving does not write Taskband or change Windows'
// pins. Length-prefix encoding avoids using a path character as a delimiter.
static bool IsStableOrderKey(const std::wstring& key) {
    return key.rfind(L"app:",0)==0 || key.rfind(L"exe:",0)==0;
}

static std::wstring EncodeOrder(const std::vector<std::wstring>& keys) {
    std::wstring encoded;
    for(const auto& key:keys) {
        if(!IsStableOrderKey(key) || key.size()>4096) continue;
        std::wstring part=std::to_wstring(key.size())+L":"+key;
        if(encoded.size()+part.size()>32760) break;
        encoded+=part;
    }
    return encoded;
}

static bool DecodeOrder(const std::wstring& encoded, std::vector<std::wstring>& result) {
    result.clear();
    if(encoded.size()>32760) return false;
    size_t cursor=0;
    while(cursor<encoded.size()) {
        size_t length=0, digits=0;
        while(cursor<encoded.size() && encoded[cursor]>=L'0' && encoded[cursor]<=L'9') {
            length=length*10+(encoded[cursor]-L'0');
            ++cursor; ++digits;
            if(digits>5 || length>4096) { result.clear(); return false; }
        }
        if(!digits || cursor>=encoded.size() || encoded[cursor++]!=L':' ||
           !length || length>encoded.size()-cursor || result.size()>=MAX_TASKS) {
            result.clear(); return false;
        }
        std::wstring key=encoded.substr(cursor,length);
        cursor+=length;
        if(!IsStableOrderKey(key) || std::find(result.begin(),result.end(),key)!=result.end()) {
            result.clear(); return false;
        }
        result.push_back(std::move(key));
    }
    return true;
}

static void LoadOrderOverride() {
    if(!s) return;
    std::vector<wchar_t> buffer(32768,L'\0');
    size_t len=Wh_GetStringValue(L"Taskbar81.UserOrder",buffer.data(),buffer.size());
    if(!len) { s->customOrder.clear(); return; }
    if(len>=buffer.size() || !DecodeOrder(std::wstring(buffer.data(),len),s->customOrder)) {
        s->customOrder.clear();
        Wh_Log(L"Invalid saved taskbar order ignored");
    }
}

static void SaveOrderOverride() {
    if(!s) return;
    std::wstring encoded=EncodeOrder(s->customOrder);
    if(!Wh_SetStringValue(L"Taskbar81.UserOrder",encoded.c_str()))
        Wh_Log(L"Could not save custom taskbar order");
}

[[maybe_unused]] static void ClearOrderOverride() {
    if(!s) return;
    s->customOrder.clear();
    if(!Wh_SetStringValue(L"Taskbar81.UserOrder",L""))
        Wh_Log(L"Could not clear custom taskbar order");
}

static size_t OrderRank(const std::wstring& key, const std::vector<std::wstring>& order) {
    auto found=std::find(order.begin(),order.end(),key);
    return found==order.end()?order.size():(size_t)(found-order.begin());
}

static void ApplyOrderOverride(std::vector<Group>& groups) {
    if(!s || s->customOrder.empty()) return;
    std::stable_sort(groups.begin(),groups.end(),[](const Group& left,const Group& right) {
        return OrderRank(left.key,s->customOrder)<OrderRank(right.key,s->customOrder);
    });
}

static void MoveGroupToInsertion(const std::wstring& key, int insertion) {
    if(!s || key.empty() || s->groups.empty()) return;
    std::vector<std::wstring> keys;
    const Group* selected=nullptr;
    for(const Group& group:s->groups) {
        if(group.key==key)selected=&group;
        const auto& base=GroupBase(group);
        if(std::find(keys.begin(),keys.end(),base)==keys.end())keys.push_back(base);
    }
    if(!selected)return;
    std::wstring base=GroupBase(*selected);
    auto at=std::find(keys.begin(),keys.end(),base);
    if(at==keys.end()) return;
    int visual=std::max(0,std::min(insertion,(int)s->groups.size()));
    // Snap a drop inside another application's block to the nearest edge.
    if(visual>0 && visual<(int)s->groups.size() && GroupBase(s->groups[visual-1])==GroupBase(s->groups[visual])) {
        const auto& targetBase=GroupBase(s->groups[visual]);int first=visual,last=visual;
        while(first>0 && GroupBase(s->groups[first-1])==targetBase)--first;
        while(last<(int)s->groups.size() && GroupBase(s->groups[last])==targetBase)++last;
        visual=(visual-first<last-visual)?first:last;
    }
    std::vector<std::wstring> preceding;
    for(int i=0;i<visual;i++) {
        const auto& candidate=GroupBase(s->groups[i]);
        if(candidate!=base && std::find(preceding.begin(),preceding.end(),candidate)==preceding.end())preceding.push_back(candidate);
    }
    std::wstring moving=*at;keys.erase(at);
    keys.insert(keys.begin()+std::min(preceding.size(),keys.size()),std::move(moving));
    s->customOrder.swap(keys);
    SaveOrderOverride();
}


static bool IsExplorerImplicitWindow(const Task& task) {
    if(task.key.rfind(L"exe:",0)!=0 || !task.hwnd || task.processPath.empty()) return false;
    wchar_t win[MAX_PATH]={}; UINT len=GetWindowsDirectoryW(win,MAX_PATH);
    if(!len || len>=MAX_PATH) return false;
    std::wstring systemExplorer=NormalizedMatchPath(std::wstring(win)+L"\\explorer.exe");
    if(systemExplorer.empty() || task.processPath!=systemExplorer) return false;
    wchar_t cls[64]={};
    return GetClassNameW(task.hwnd,cls,64) && !_wcsicmp(cls,L"CabinetWClass");
}
static bool IsCanonicalExplorerPin(const Pin& pin) {
    return !_wcsicmp(pin.key.c_str(),L"app:microsoft.windows.explorer");
}
static int CurrentTaskAreaWidth();
void HidePreview();
static void CloseJumpPopup(bool animate);
static void SplitGroups(std::vector<Group>& groups,int level,int available,bool vertical) {
    if(!s)return;
    s->labelMode=false;
    for(Group& group:groups) {
        group.baseKey=group.key;
        std::stable_sort(group.tasks.begin(),group.tasks.end(),[](size_t a,size_t b){
            return s->tasks[a].seq<s->tasks[b].seq;
        });
    }
    if(vertical || (level!=1 && level!=2))return;
    size_t count=0;for(const Group& g:groups)count+=std::max<size_t>(1,g.tasks.size());
    std::vector<bool> combined(groups.size(),false);
    if(level==1) {
        std::vector<size_t> order(groups.size());for(size_t i=0;i<order.size();++i)order[i]=i;
        std::stable_sort(order.begin(),order.end(),[&](size_t a,size_t b){return groups[a].tasks.size()>groups[b].tasks.size();});
        for(size_t i:order) {
            if(count*(size_t)Px(150)<=(size_t)std::max(0,available))break;
            size_t n=groups[i].tasks.size();if(n>1){combined[i]=true;count-=n-1;}
        }
    }
    s->labelMode=level==2 || count*(size_t)Px(150)<=(size_t)std::max(0,available);
    std::vector<Group> split;split.reserve(count);
    for(size_t i=0;i<groups.size();++i) {
        const Group& group=groups[i];
        if(group.tasks.empty() || combined[i]) {split.push_back(group);continue;}
        for(size_t t:group.tasks) {
            wchar_t suffix[2+sizeof(uintptr_t)*2+1]{};
            swprintf(suffix,std::size(suffix),L"#%llX",(unsigned long long)(uintptr_t)s->tasks[t].hwnd);
            split.push_back({group.baseKey+suffix,{t},group.pin,group.baseKey});
        }
    }
    groups.swap(split);
}
void BuildGroups() {
    std::vector<Group> next;
    for(size_t i=0;i<s->pins.size();i++) {
        bool seen=false;
        for(const auto& g:next) if(g.key==s->pins[i].key) {seen=true;break;}
        if(!seen) next.push_back({s->pins[i].key,{},(int)i});
    }
    for (const Group& g : s->groups) {
        bool seen=false;
        const auto& base=GroupBase(g);
        for(const auto& n:next) if(n.key==base) {seen=true;break;}
        if (!seen && !base.empty()) next.push_back({base,{},-1});
    }
    for(size_t i=0;i<s->tasks.size();++i) {
        const Task& task=s->tasks[i];
        size_t chosen=next.size();
        // First prefer an exact pinned AppID or executable identity. An old
        // transient group must not preempt the pin after it becomes available.
        for(size_t j=0;j<next.size();j++) {
            if(next[j].pin>=0 && next[j].key==task.key) { chosen=j; break; }
        }
        if(chosen==next.size() && IsExplorerImplicitWindow(task)) {
            size_t candidate=next.size(); bool ambiguous=false;
            for(size_t j=0;j<next.size();j++) {
                int pin=next[j].pin;
                if(pin<0 || (size_t)pin>=s->pins.size() || !IsCanonicalExplorerPin(s->pins[pin])) continue;
                if(candidate!=next.size()) { ambiguous=true; break; }
                candidate=j;
            }
            if(!ambiguous && candidate!=next.size()) chosen=candidate;
        }
        if(chosen==next.size()) {
            size_t candidate=next.size(); bool ambiguous=false;
            for(size_t j=0;j<next.size();j++) {
                int pin=next[j].pin;
                if(pin<0 || (size_t)pin>=s->pins.size()) continue;
                const Pin& p=s->pins[pin];
                if(p.target.empty()) continue;
                // Distinct explicit IDs (e.g. browser profiles) must stay
                // distinct even when they happen to share an executable.
                if(task.key.rfind(L"app:",0)==0 && p.key.rfind(L"app:",0)==0 && p.key!=task.key) continue;
                bool same=(!task.relaunchPath.empty() && task.relaunchPath==p.target) ||
                          (!task.processPath.empty() && task.processPath==p.target) ||
                          (task.key.rfind(L"exe:",0)==0 && task.key.substr(4)==p.target);
                if(!same) continue;
                if(candidate!=next.size()) { ambiguous=true; break; }
                candidate=j;
            }
            if(!ambiguous && candidate!=next.size()) chosen=candidate;
        }
        // Only after trying pins, retain the previous unpinned group for
        // stable order. Unmatched transient groups are removed below.
        if(chosen==next.size()) {
            for(size_t j=0;j<next.size();j++)
                if(next[j].key==task.key) { chosen=j; break; }
        }
        if(chosen==next.size()) { next.push_back({task.key,{},-1}); chosen=next.size()-1; }
        next[chosen].tasks.push_back(i);
    }
    next.erase(std::remove_if(next.begin(),next.end(),
        [](const Group& g){return g.tasks.empty() && g.pin<0;}),next.end());
    ApplyOrderOverride(next);
    SplitGroups(next,s->nativeGlomLevel,CurrentTaskAreaWidth(),s->verticalActive);
    bool identityChanged=next.size()!=s->groups.size();
    if(!identityChanged)for(size_t i=0;i<next.size();++i)if(next[i].key!=s->groups[i].key){identityChanged=true;break;}
    if(identityChanged && s->bar) {
        if(s->preview || s->tooltip)HidePreview();
        if(s->jumpPopup)CloseJumpPopup(false);
        s->hot=HIT_NONE;s->hover=HIT_NONE;KillTimer(s->bar,TIMER_HOVER);
        if(s->dragCandidate || s->dragActive) {
            s->dragCandidate=false;s->dragActive=false;s->dragInsertion=-1;s->dragKey.clear();
            if(GetCapture()==s->bar)ReleaseCapture();
        }
    }
    s->groups.swap(next);
}
const Task& GroupTask(size_t i) {
    const Group& g=s->groups[i];
    HWND fg=GetForegroundWindow();
    for(size_t t:g.tasks) if(s->tasks[t].hwnd==fg) return s->tasks[t];
    return s->tasks[g.tasks.front()];
}
HICON GroupIcon(size_t i) {
    const Group& g=s->groups[i];
    if(GroupBase(g)==L"app:taskbar81.controlpanel" && !g.tasks.empty() && GroupTask(i).icon) return GroupTask(i).icon;
    if(g.pin>=0 && (size_t)g.pin<s->pins.size() && s->pins[g.pin].icon) return s->pins[g.pin].icon;
    if(GroupBase(g).rfind(L"app:",0)==0 && w7t::IsPackagedAumid(GroupBase(g).substr(4)) &&
       !g.tasks.empty() && GroupTask(i).icon) return GroupTask(i).icon;
    if(!g.tasks.empty() && GroupTask(i).icon) return GroupTask(i).icon;
    return LoadIconW(nullptr,MAKEINTRESOURCEW(32512));
}
static std::wstring LocalizeExplorerTitle(const std::wstring& title) {
    if(_wcsicmp(title.c_str(),L"File Explorer") && _wcsicmp(title.c_str(),L"Windows Explorer"))return title;
    if(ItalianUI())return L"Esplora file";
    LANGID lang=PRIMARYLANGID(GetUserDefaultUILanguage());
    if(lang==LANG_SPANISH)return L"Explorador de archivos";
    if(lang==LANG_FRENCH)return L"Explorateur de fichiers";
    if(lang==LANG_PORTUGUESE)return L"Explorador de Arquivos";
    return title;
}
std::wstring GroupTitle(size_t i) {
    const Group& g=s->groups[i];
    if(!g.tasks.empty()) return LocalizeExplorerTitle(GroupTask(i).title);
    if(g.pin>=0 && (size_t)g.pin<s->pins.size()) return LocalizeExplorerTitle(s->pins[g.pin].title);
    return L"";
}
// Aero Peek copied from user's GPL-3.0-or-later mod.txt v0.5.0; uses
// undocumented dwmapi ordinal 113. No guarantee on Windows 11 24H2.
typedef HRESULT (WINAPI* ActivateLivePreviewFn)(BOOL, HWND, HWND, UINT, UINT_PTR);
const UINT kPeekWindow = 3, kPeekDesktop = 1;

#ifdef TASKBAR81_TEST
static ActivateLivePreviewFn testPeekFn=nullptr;
#endif
ActivateLivePreviewFn GetActivateLivePreview() {
#ifdef TASKBAR81_TEST
    if(testPeekFn) return testPeekFn;
#endif
    static ActivateLivePreviewFn fn = nullptr;
    static bool tried = false;
    if (!tried) {
        tried = true;
        HMODULE nt=GetModuleHandleW(L"ntdll.dll");
        if(nt && GetProcAddress(nt,"wine_get_version")) return nullptr;
        HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
        if (!dwm) dwm = LoadLibraryW(L"dwmapi.dll");
        if (dwm) fn = (ActivateLivePreviewFn)GetProcAddress(dwm, MAKEINTRESOURCEA(113));
    }
    return fn;
}
bool PeekAllowedByDwm() {
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"EnableAeroPeek",
                     RRF_RT_REG_DWORD, nullptr, &value, &size) != ERROR_SUCCESS) return true;
    return value != 0;
}
/* Documented: windows excluded from peek stay visible while it is active. */
void ExcludeFromPeek(HWND h) {
    if (!h) return;
    BOOL exclude = TRUE;
    DwmSetWindowAttribute(h, (DWMWINDOWATTRIBUTE)12 /* DWMWA_EXCLUDED_FROM_PEEK */, &exclude, sizeof(exclude));
}
void PeekOff(bool onlyDesktop) {
    if (!s || !s->peekOn) return;
    if (onlyDesktop && !s->peekDesktop) return;
    ActivateLivePreviewFn fn = GetActivateLivePreview();
    if (fn) {
        if(s->peekDesktop) fn(FALSE,s->bar,nullptr,kPeekDesktop,0);
        else fn(FALSE,s->peekTarget,s->bar,kPeekWindow,0);
    }
    s->peekOn = false;
    s->peekDesktop = false;
    s->peekTarget = nullptr;
}
// The companion Start screen is a full-screen shell surface. Aero Peek
// while it is visible can create a graphical defect (desktop/window leaking
// through Start), so temporarily suppress Peek without changing preferences.
// This class name is the companion mod's private, version-dependent API.
static bool CompanionStartScreenVisible() {
    HWND h=nullptr;
    for(int i=0;i<32 && (h=FindWindowExW(nullptr,h,L"Win8StartScreenMod8045",nullptr));++i) {
        if(!IsWindowVisible(h))continue;
        DWORD pid=0;GetWindowThreadProcessId(h,&pid);
        if(pid==GetCurrentProcessId())return true;
        // The companion can be hosted by a different Explorer process.
        std::wstring image=ProcessImage(pid);
        if(!image.empty() && !_wcsicmp(PathFindFileNameW(image.c_str()),L"explorer.exe"))return true;
    }
    return false;
}
void PeekWindow(HWND target) {
    if (!s || !s->bar || !aeroPeek.load() || !target || !IsWindow(target) || !PeekAllowedByDwm() || CompanionStartScreenVisible()) return;
    ActivateLivePreviewFn fn = GetActivateLivePreview();
    if (!fn) return;
    PeekOff(false);
    if (SUCCEEDED(fn(TRUE, target, s->bar, kPeekWindow, 0))) {
        s->peekOn = true; s->peekDesktop = false; s->peekTarget = target;
    }
}
static bool DesktopPeekAvailable() {
    if(CompanionStartScreenVisible() || !PeekAllowedByDwm() || !GetActivateLivePreview()) return false;
#ifdef TASKBAR81_TEST
    if(testPeekFn) return true;
#endif
    BOOL composed=FALSE; return SUCCEEDED(DwmIsCompositionEnabled(&composed)) && composed;
}
void PeekDesktop() {
    if (!s || !s->bar || !s->desktopPeekEnabled || !DesktopPeekAvailable()) return;
    ActivateLivePreviewFn fn = GetActivateLivePreview();
    if (!fn) return;
    PeekOff(false);
    if (SUCCEEDED(fn(TRUE, s->bar, nullptr, kPeekDesktop, 0))) {
        s->peekOn = true; s->peekDesktop = true; s->peekTarget = nullptr;
    }
}

// Desktop Peek policy and Show Desktop button behavior (GPL-3.0-or-later).
// Never writes Windows' policy.
static void LoadDesktopPeekPreference() {
    if(!s) return; wchar_t saved[4]={}; size_t n=Wh_GetStringValue(L"Taskbar81.DesktopPeek",saved,4);
    if(n==1 && (saved[0]==L'0'||saved[0]==L'1')) { s->desktopPeekEnabled=(saved[0]==L'1'); return; }
    DWORD value=0,size=sizeof(value);
    if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",L"DisablePreviewDesktop",RRF_RT_REG_DWORD,nullptr,&value,&size)==ERROR_SUCCESS)
        s->desktopPeekEnabled=(value==0);
}
static DWORD DesktopPeekDelay() {
    DWORD value=450,size=sizeof(value);
    if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",L"DesktopLivePreviewHoverTime",RRF_RT_REG_DWORD,nullptr,&value,&size)!=ERROR_SUCCESS) value=450;
    return std::min<DWORD>(value,5000);
}
static void ToggleDesktopPeekPreference() {
    if(!s) return; s->desktopPeekEnabled=!s->desktopPeekEnabled;
    if(!Wh_SetStringValue(L"Taskbar81.DesktopPeek",s->desktopPeekEnabled?L"1":L"0")) Wh_Log(L"[T81-DIAG] cannot save desktop Peek preference");
    if(!s->desktopPeekEnabled) PeekOff(true);
    Wh_Log(L"[T81-DIAG] desktop Peek preference=%d",s->desktopPeekEnabled?1:0);
}
void SendWin(WORD key);
static void ToggleDesktop() {
    IShellDispatch4* raw=nullptr;
    HRESULT hr=CoCreateInstance(CLSID_Shell,nullptr,CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER,IID_IShellDispatch4,(void**)&raw);
    std::unique_ptr<IShellDispatch4,ReleaseCom<IShellDispatch4>> shell(raw);
    if(SUCCEEDED(hr)) hr=shell?shell->ToggleDesktop():E_NOINTERFACE;
    if(FAILED(hr)) { Wh_Log(L"[T81-DIAG] ToggleDesktop failed hr=0x%08lx; Win+D fallback",(unsigned long)hr); SendWin('D'); }
}
static void ShowDesktopContextMenu(POINT p) {
    if(!s || !s->bar) return;
    KillTimer(s->bar,TIMER_DESKTOP_PEEK); PeekOff(true);
    std::wstring menu=Tr(L"Mostra desktop\n-\n",L"Show the desktop\n-\n");
    if(!DesktopPeekAvailable()) menu+=L"!";
    if(s->desktopPeekEnabled) menu+=L"*";
    menu+=Tr(L"Anteprima del desktop",L"Peek at desktop");
    int cmd=w7t::ShellMenu::ShowContextMenuEx(p.x,p.y,true,menu.c_str(),true);
    if(cmd==1) ToggleDesktop();
    else if(cmd==2 && DesktopPeekAvailable()) ToggleDesktopPeekPreference();
    if(s) s->hot=HIT_NONE; // next mouse movement re-evaluates the hover delay
}


void HidePreview();
void ShowPreview(int group);
void ShowPreviewPage(int group,int offset);
struct RefreshContext { std::vector<Task>* tasks; bool refreshIcons; bool failed=false; };
struct TaskSnapshot {
    Task task{};
    std::unique_ptr<std::remove_pointer<HICON>::type,DestroyIconHandle> newIcon;
};
int Px(int n);
static const Task* FindStoredTask(HWND hwnd,DWORD pid) {
    for(const Task& stored:s->tasks)
        if(stored.hwnd==hwnd && stored.pid==pid)return &stored;
    return nullptr;
}
static bool BuildTaskSnapshot(HWND hwnd,const RefreshContext& context,TaskSnapshot& snapshot) {
    DWORD pid=0;
    GetWindowThreadProcessId(hwnd,&pid);
    const Task* stored=FindStoredTask(hwnd,pid);
    std::wstring title;
    if(!GetTitle(hwnd,title)) {
        if(!stored)return false;
        title=stored->title;
    }
    if(title.empty())return false;

    std::wstring relaunchPath=stored?stored->relaunchPath:std::wstring{};
    std::wstring identity;
    if(stored && !context.refreshIcons)identity=stored->key;
    else identity=WindowIdentity(hwnd,&relaunchPath);

    if(!stored || (context.refreshIcons && s->ticks%30==0)) {
        wchar_t className[80]{};
        GetClassNameW(hwnd,className,(int)std::size(className));
        if(!_wcsicmp(className,L"ApplicationFrameWindow"))
            Wh_Log(L"[T81-DIAG] uwp-frame hwnd=%p pid=%lu app-identity=%d packaged-identity=%d",
                hwnd,pid,identity.rfind(L"app:",0)==0?1:0,
                identity.rfind(L"app:",0)==0 && w7t::IsPackagedAumid(identity.substr(4))?1:0);
    }

    HICON icon=stored?stored->icon:nullptr;
    bool ownsIcon=stored?stored->ownsIcon:false;
    if(!stored || context.refreshIcons) {
        const bool packaged=identity.rfind(L"app:",0)==0 && w7t::IsPackagedAumid(identity.substr(4));
        HICON candidate=nullptr;
        if(identity==L"app:taskbar81.controlpanel")
            candidate=ShellItemIcon(L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}",Px(48));
        else if(packaged)
            candidate=ShellItemIcon(L"shell:AppsFolder\\"+identity.substr(4),Px(48));
        if(packaged && (!stored || s->ticks%30==0))
            Wh_Log(L"[T81-DIAG] packaged-window hwnd=%p pid=%lu appsfolder-icon=%d",hwnd,pid,candidate?1:0);
        if(!candidate) {
            HICON source=GetIcon(hwnd);
            candidate=source?CopyIcon(source):nullptr;
        }
        if(candidate) {
            snapshot.newIcon.reset(candidate);
            icon=candidate;
            ownsIcon=true;
        } else if(!stored) {
            icon=LoadIconW(nullptr,MAKEINTRESOURCEW(32512));
            ownsIcon=false;
        }
    }
    std::wstring processPath=stored?stored->processPath:NormalizedMatchPath(ProcessImage(pid));
    const ULONGLONG sequence=stored?stored->seq:++s->nextTaskSequence;
    snapshot.task=Task{hwnd,pid,icon,ownsIcon,std::move(title),std::move(identity),
                       std::move(processPath),std::move(relaunchPath),sequence};
    return true;
}
BOOL CALLBACK EnumTask(HWND hwnd,LPARAM parameter) {
    auto* context=reinterpret_cast<RefreshContext*>(parameter);
    try {
        if(quitting.load())return FALSE;
#ifdef TASKBAR81_TEST
        if(testThrowEnum)throw std::bad_alloc();
#endif
        if(context->tasks->size()>=MAX_TASKS)return FALSE;
        if(!IsTaskWindow(hwnd))return TRUE;
        TaskSnapshot snapshot;
        if(!BuildTaskSnapshot(hwnd,*context,snapshot))return TRUE;
        context->tasks->push_back(std::move(snapshot.task));
        [[maybe_unused]] auto transferredNewIcon=snapshot.newIcon.release();
        return TRUE;
    } catch(...) {
        if(context)context->failed=true;
        Wh_Log(L"[T81-DIAG] task enumeration C++ exception; previous snapshot retained");
        return FALSE;
    }
}
static PendingButtonChanges PrepareButtonChanges(const std::vector<Task>& next);
static void CommitButtonChanges(PendingButtonChanges&& pending,const std::vector<Task>& previous);
void Refresh() {
    std::vector<Task> tasks;
    RefreshContext ctx{&tasks, (++s->ticks % 5)==0};
    EnumWindows(EnumTask, reinterpret_cast<LPARAM>(&ctx));
    if(ctx.failed) {
        for(const Task& candidate:tasks) if(candidate.ownsIcon && candidate.icon) {
            bool borrowed=false;for(const Task& old:s->tasks) if(old.icon==candidate.icon) {borrowed=true;break;}
            if(!borrowed) DestroyIcon(candidate.icon);
        }
        return;
    }
    auto changes=PrepareButtonChanges(tasks);
    s->tasks.swap(tasks);
    // Old vectors can reuse owned handles; release only handles not retained
    // by any current item. Preview windows take independent icon copies.
    for(const Task& old:tasks) {
        if(!old.ownsIcon || !old.icon) continue;
        bool retained=false;
        for(const Task& now:s->tasks) if(now.icon==old.icon) { retained=true; break; }
        if(!retained) DestroyIcon(old.icon);
    }
    if(s->ticks==1 || s->ticks%30==0 || HasFavoritesVersionChanged()) RefreshPins();
    BuildGroups();
    CommitButtonChanges(std::move(changes),tasks);
    for(const auto& item:s->previews) if(!item.more && !IsWindow(item.hwnd)) { HidePreview(); break; }
    InvalidateRect(s->bar, nullptr, FALSE);
}
void FreeTaskIcons() {
    if(!s) return;
    for(Task& task:s->tasks) {
        if(task.ownsIcon && task.icon) DestroyIcon(task.icon);
        task.icon=nullptr; task.ownsIcon=false;
    }
    s->tasks.clear();
}
static HWND ActivationPopup(HWND hwnd) {
    if(!IsWindow(hwnd))return nullptr;
    HWND root=GetAncestor(hwnd,GA_ROOTOWNER);
    if(!root)root=hwnd;
    HWND candidate=GetLastActivePopup(root);
    if(candidate && IsWindow(candidate) && IsWindowVisible(candidate))return candidate;
    return hwnd;
}
static bool SameInput(DWORD tick) { LASTINPUTINFO info{sizeof(info),0}; return GetLastInputInfo(&info) && info.dwTime==tick; }
static void CancelActivationRetry() {
    if(!s) return; if(s->bar) KillTimer(s->bar,TIMER_ACTIVATE_RETRY);
    s->activateTarget=nullptr; s->activatePid=0; s->activateRetries=0;
}
// The RetroBar ManagedShell ApplicationWindow Restore/DoClose methods use
// bounded WM_SYSCOMMAND. This is best effort and does not bypass UIPI/UAC.
#ifdef TASKBAR81_TEST
static decltype(&ShowWindowAsync) taskShowAsync=ShowWindowAsync;
static decltype(&PostMessageW) taskPostMessage=PostMessageW;
static decltype(&SendMessageTimeoutW) taskSendTimeout=SendMessageTimeoutW;
#else
#define taskShowAsync ShowWindowAsync
#define taskPostMessage PostMessageW
#define taskSendTimeout SendMessageTimeoutW
#endif
static void ShowInfoTooltip(const std::wstring& message,int center);
static void NotifyTaskAccessDenied() {
    if(!s || !s->bar)return;RECT bar{};if(!GetWindowRect(s->bar,&bar))return;
    ShowInfoTooltip(Tr(L"Windows ha bloccato il comando. Usa la finestra originale o verifica i privilegi.",
                       L"Windows blocked the command. Use the original window or check permissions."),(bar.left+bar.right)/2);
}
static bool TaskSystemCommand(HWND h,WPARAM command,bool asyncFirst) {
    if(!IsWindow(h))return false;DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(!pid)return false;
    if(asyncFirst) {
        SetLastError(0);if(taskPostMessage(h,WM_SYSCOMMAND,command,0)) {
            Wh_Log(L"[T81-DIAG] task syscommand queued hwnd=%p pid=%lu command=0x%lx (completion not confirmed)",h,pid,(unsigned long)command);return true;
        }
        Wh_Log(L"[T81-DIAG] task syscommand post failed hwnd=%p pid=%lu command=0x%lx error=%lu",h,pid,(unsigned long)command,GetLastError());
    }
    // Same-thread SendMessageTimeout does not time out; never use it there.
    if(GetWindowThreadProcessId(h,nullptr)==GetCurrentThreadId())return false;
    DWORD_PTR result=0;SetLastError(0);
    if(taskSendTimeout(h,WM_SYSCOMMAND,command,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,200,&result)) {
        Wh_Log(L"[T81-DIAG] task syscommand delivered hwnd=%p pid=%lu command=0x%lx (result is not completion)",h,pid,(unsigned long)command);return true;
    }
    DWORD error=GetLastError();Wh_Log(L"[T81-DIAG] task syscommand rejected/timed out hwnd=%p pid=%lu command=0x%lx error=%lu",h,pid,(unsigned long)command,error);
    if(error==ERROR_ACCESS_DENIED)NotifyTaskAccessDenied();
    return false;
}
static bool RequestTaskShowState(HWND hwnd,int showCommand,WPARAM systemCommand,
                                 bool systemFirst,const wchar_t* action) {
    if(systemFirst)return TaskSystemCommand(hwnd,systemCommand,false);
    SetLastError(0);
    const BOOL accepted=taskShowAsync(hwnd,showCommand);
    const DWORD error=accepted?0:GetLastError();
    if(accepted) {
        Wh_Log(L"[T81-DIAG] %ls request accepted hwnd=%p (state not confirmed)",action,hwnd);
        return true;
    }
    Wh_Log(L"[T81-DIAG] %ls ShowWindowAsync rejected hwnd=%p error=%lu; trying system command",action,hwnd,error);
    return TaskSystemCommand(hwnd,systemCommand,false);
}
static bool RequestTaskClose(HWND hwnd) {
    if(s && s->activateTarget==hwnd)CancelActivationRetry();
    return TaskSystemCommand(hwnd,SC_CLOSE,true);
}
static bool RequestTaskMinimize(HWND hwnd) {
    if(s && s->activateTarget==hwnd)CancelActivationRetry();
    if(!IsWindow(hwnd) || IsHungAppWindow(hwnd))return false;
    return RequestTaskShowState(hwnd,SW_MINIMIZE,SC_MINIMIZE,false,L"minimize");
}
static bool RequestTaskRestore(HWND hwnd,bool systemFirst=false) {
    if(!IsWindow(hwnd))return false;
    return RequestTaskShowState(hwnd,SW_RESTORE,SC_RESTORE,systemFirst,L"restore");
}
void ActivateTask(HWND hwnd) {
    if(!s || !IsWindow(hwnd))return;
    CancelActivationRetry();
    DWORD pid=0;
    GetWindowThreadProcessId(hwnd,&pid);
    if(!pid)return;
    const HWND target=ActivationPopup(hwnd);
    if(IsIconic(hwnd))RequestTaskRestore(hwnd);
    if(target && target!=hwnd && IsIconic(target))RequestTaskRestore(target);
    const BOOL foreground=SetForegroundWindow(target?target:hwnd);
    LASTINPUTINFO input{sizeof(input),0};
    if(s->bar && GetLastInputInfo(&input)) {
        s->activateTarget=hwnd;
        s->activatePid=pid;
        s->activateInput=input.dwTime;
        s->activateRetries=2;
        SetTimer(s->bar,TIMER_ACTIVATE_RETRY,120,nullptr);
    }
    Wh_Log(L"[T81-DIAG] activate hwnd=%p pid=%lu initialForeground=%d minimized=%d",
           hwnd,pid,foreground?1:0,IsIconic(hwnd)?1:0);
}
static void TickActivateRetry() {
    if(!s || !s->activateTarget)return;
    const HWND hwnd=s->activateTarget;
    DWORD pid=0;
    if(!IsWindow(hwnd) || !GetWindowThreadProcessId(hwnd,&pid) ||
       pid!=s->activatePid || !SameInput(s->activateInput)) {
        CancelActivationRetry();
        return;
    }
    const HWND target=ActivationPopup(hwnd);
    if(!IsIconic(hwnd) &&
       (GetForegroundWindow()==hwnd || GetForegroundWindow()==target)) {
        CancelActivationRetry();
        return;
    }
    if(IsIconic(hwnd) && s->activateRetries==2)RequestTaskRestore(hwnd,true);
    const BOOL accepted=SetForegroundWindow(target?target:hwnd);
    --s->activateRetries;
    if(s->activateRetries>0)return;
    if(!accepted || IsIconic(hwnd)) {
        FLASHWINFO flash{sizeof(flash),hwnd,FLASHW_TRAY|FLASHW_TIMERNOFG,3,0};
        FlashWindowEx(&flash);
    }
    Wh_Log(L"[T81-DIAG] activation retry hwnd=%p foregroundAccepted=%d iconic=%d",
           hwnd,accepted?1:0,IsIconic(hwnd)?1:0);
    CancelActivationRetry();
}
void ClickTask(HWND hwnd) {
    if(!IsWindow(hwnd))return;
    const HWND popup=ActivationPopup(hwnd);
    const bool active=GetForegroundWindow()==hwnd ||
                      (popup && GetForegroundWindow()==popup);
    if(!IsIconic(hwnd) && active) {
        CancelActivationRetry();
        RequestTaskMinimize(hwnd);
    } else {
        ActivateTask(hwnd);
    }
}
static void LaunchGroupInstance(size_t index) {
    if(!s || index>=s->groups.size()) return;
    const Group& group=s->groups[index];
    std::wstring path;
    if(group.pin>=0 && (size_t)group.pin<s->pins.size()) path=s->pins[group.pin].path;
    else if(!group.tasks.empty()) path=ProcessImage(s->tasks[group.tasks.front()].pid);
    if(!path.empty()) ShellExecuteW(s->bar,L"open",path.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
}
struct GroupWindowRef { HWND hwnd; DWORD pid; };
static std::vector<HWND> LiveGroupWindows(const std::vector<GroupWindowRef>& references) {
    std::vector<HWND> result;
    result.reserve(references.size());
    for(const GroupWindowRef& reference:references) {
        DWORD currentPid=0;
        GetWindowThreadProcessId(reference.hwnd,&currentPid);
        if(!reference.pid || currentPid!=reference.pid || !IsWindow(reference.hwnd))continue;
        result.push_back(reference.hwnd);
    }
    return result;
}
static void RunGroupWindowCommand(int action,const std::vector<HWND>& windows) {
    if(action>=1 && action<=3 && !windows.empty()) {
        for(HWND hwnd:windows)if(IsIconic(hwnd))RequestTaskRestore(hwnd);
        if(action==1)CascadeWindows(nullptr,0,nullptr,(UINT)windows.size(),windows.data());
        else TileWindows(nullptr,action==2?MDITILE_HORIZONTAL:MDITILE_VERTICAL,
                         nullptr,(UINT)windows.size(),windows.data());
        return;
    }
    if(action==4)for(HWND hwnd:windows)RequestTaskMinimize(hwnd);
    if(action==5)for(HWND hwnd:windows)RequestTaskClose(hwnd);
}
static void ShowGroupWindowMenu(POINT screen,const Group& group) {
    std::vector<GroupWindowRef> references;
    references.reserve(group.tasks.size());
    for(size_t taskIndex:group.tasks) {
        if(taskIndex>=s->tasks.size())continue;
        HWND hwnd=s->tasks[taskIndex].hwnd;
        DWORD pid=0;
        GetWindowThreadProcessId(hwnd,&pid);
        references.push_back({hwnd,pid});
    }
    const wchar_t* menuText=Tr(L"Finestre a cascata\nMostra finestre sovrapposte\nMostra finestre affiancate\nRiduci a icona tutte\n-\nChiudi tutte le finestre",L"Cascade windows\nShow windows stacked\nShow windows side by side\nMinimize all\n-\nClose all windows");
    const int action=w7t::ShellMenu::ShowContextMenuEx(screen.x,screen.y,true,menuText,true);
    const std::vector<HWND> windows=LiveGroupWindows(references);
    RunGroupWindowCommand(action,windows);
}
// Integration with win81-start-screen-recreation 1.1.0 (GPL-3.0-or-later):
// its Win8StartScreenMod8045 UI window handles WM_APP+1 as a toggle. This is
// an internal, version-dependent protocol, not a public Windows API. Only
// accept a window in the current Explorer process; otherwise fall back to Win.
static bool ToggleCompanionStart() {
    HWND h=nullptr;
    for(int i=0;i<32 && (h=FindWindowExW(nullptr,h,L"Win8StartScreenMod8045",nullptr));++i) {
        DWORD pid=0; GetWindowThreadProcessId(h,&pid);
        if(pid!=GetCurrentProcessId()) continue;
        PeekOff(false); // Stop an existing Peek before opening the companion Start screen.
        if(PostMessageW(h,WM_APP+1,0,0)) {
            Wh_Log(L"[T81-DIAG] Start: companion toggle posted (private protocol)");
            return true;
        }
        Wh_Log(L"[T81-DIAG] Start: companion post failed, error %lu",GetLastError());
    }
    return false;
}
void SendWin(WORD extra) {
    if(!extra && ToggleCompanionStart()) return;
    INPUT a[4] = {}; int n = 0;
    a[n].type=INPUT_KEYBOARD; a[n++].ki.wVk=VK_LWIN;
    if (extra) {
        a[n].type=INPUT_KEYBOARD; a[n++].ki.wVk=extra;
        a[n].type=INPUT_KEYBOARD; a[n].ki.wVk=extra; a[n++].ki.dwFlags=KEYEVENTF_KEYUP;
    }
    a[n].type=INPUT_KEYBOARD; a[n].ki.wVk=VK_LWIN; a[n++].ki.dwFlags=KEYEVENTF_KEYUP;
    UINT sent=SendInput(n,a,sizeof(INPUT));
    if(sent!=(UINT)n) Wh_Log(L"[T81-DIAG] SendInput inserted %u/%d events; cause unknown",sent,n);
}
int Dpi() {
#ifdef TASKBAR81_TEST
    if(testDpiOverride>0) return testDpiOverride;
#endif
    UINT d=s && s->bar?GetDpiForWindow(s->bar):96; return d?(int)d:96;
}
int Px(int n) { return MulDiv(n,Dpi(),96); }
// Request the system-installed Segoe UI, regular 9 pt. GDI may substitute a
// different face if it is missing. The per-pixel black/white reconstruction
// uses grayscale antialiasing; normal GDI mode respects system smoothing.
static HFONT CreateTaskbarFont(int dpi,bool perPixel) {
    return CreateFontW(-MulDiv(9,dpi,72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                       DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                       perPixel?ANTIALIASED_QUALITY:DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
}
struct ClockTextRows { RECT time{}, date{}; bool showDate=false; };
static ClockTextRows GetClockTextRows(RECT clock,int height,int dpi) {
    ClockTextRows rows{};
    if(height<=0 || clock.right<=clock.left) return rows;
    int minimum=MulDiv(32,dpi,96), stride=std::max(1,MulDiv(17,dpi,96));
    if(height<minimum || height<2*stride) {
        rows.time=RECT{clock.left,0,clock.right,height}; return rows;
    }
    int top=(height-2*stride)/2;
    rows.time=RECT{clock.left,top,clock.right,top+stride};
    rows.date=RECT{clock.left,top+stride,clock.right,top+2*stride};
    rows.showDate=true; return rows;
}
static int MeasureClockWidth(HDC dc,int dpi,const wchar_t* time,const wchar_t* date,bool perPixel) {
    int minimum=MulDiv(80,dpi,96), maximum=MulDiv(240,dpi,96), padding=MulDiv(16,dpi,96);
    if(!dc) return minimum;
    HFONT font=CreateTaskbarFont(dpi,perPixel); if(!font) return minimum;
    HGDIOBJ old=SelectObject(dc,font);
    SIZE a{},b{};
    BOOL okA=time && GetTextExtentPoint32W(dc,time,(int)wcslen(time),&a);
    BOOL okB=date && GetTextExtentPoint32W(dc,date,(int)wcslen(date),&b);
    if(old && old!=HGDI_ERROR) SelectObject(dc,old);
    DeleteObject(font);
    if(!okA || !okB) return minimum;
    return std::max(minimum,std::min(maximum,(int)std::max(a.cx,b.cx)+padding));
}
static bool UpdateClockMetrics(HDC dc,const wchar_t* time,const wchar_t* date) {
    if(!s) return false;
    int dpi=Dpi();
    if(s->clockMetricsDpi==dpi && s->clockMetricTime==time && s->clockMetricDate==date) return false;
    int width=MeasureClockWidth(dc,dpi,time,date,s->perPixelBar);
    bool changed=width!=s->clockWidthPx;
    s->clockMetricsDpi=dpi; s->clockMetricTime=time; s->clockMetricDate=date; s->clockWidthPx=width;
    return changed;
}
const GUID kClsidMMDeviceEnumerator = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const GUID kIidIMMDeviceEnumerator = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const GUID kIidIAudioEndpointVolume = {0x5CDF2C82, 0x841E, 0x4546, {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

static void PublishCniSystemIcon(int hit);
static void RemoveCniSystemIcon(int hit);
bool EnsureEndpoint() {
    if (s->endpoint) return true;
    IMMDeviceEnumerator* enumerator = nullptr;
    if (FAILED(CoCreateInstance(kClsidMMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                kIidIMMDeviceEnumerator, (void**)&enumerator)) || !enumerator) return false;
    IMMDevice* device = nullptr;
    HRESULT hr = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    enumerator->Release();
    if (FAILED(hr) || !device) return false;
    hr = device->Activate(kIidIAudioEndpointVolume, CLSCTX_ALL, nullptr, (void**)&s->endpoint);
    device->Release();
    if (FAILED(hr)) { s->endpoint = nullptr; return false; }
    return s->endpoint != nullptr;
}
void RefreshVolume() {
    if (!EnsureEndpoint()) return;
    float oldLevel = s->volume;
    bool oldMuted = s->muted;
    float level = 0.0f;
    BOOL mute = FALSE;
    if (FAILED(s->endpoint->GetMasterVolumeLevelScalar(&level)) || FAILED(s->endpoint->GetMute(&mute))) {
        // default device changed or disappeared: drop the stale pointer, retry next tick
        s->endpoint->Release();
        s->endpoint = nullptr;
        return;
    }
    s->volume = level;
    s->muted = mute != FALSE;
    if (oldLevel != level || oldMuted != s->muted)
        PublishCniSystemIcon(HIT_VOLUME);
}
// Wheel deltas may be smaller than WHEEL_DELTA on high-resolution wheels.
static int ConsumeVolumeWheelDelta(int& remainder,int delta) {
    int total=std::max(-1200,std::min(1200,remainder+delta));
    int steps=total/WHEEL_DELTA; remainder=total%WHEEL_DELTA; return steps;
}
static void AdjustVolumeWheel(int delta) {
    if(!s || !volumeWheel.load())return;
    int steps=ConsumeVolumeWheelDelta(s->volumeWheelRemainder,delta);
    if(!steps)return;
    // Refresh the default endpoint on each gesture so a newly selected device
    // is used even when the old endpoint still responds successfully.
    if(s->endpoint) {s->endpoint->Release();s->endpoint=nullptr;}
    if(!EnsureEndpoint())return;
    HRESULT hr=S_OK;
    for(int i=0;i<std::abs(steps) && SUCCEEDED(hr);++i)
        hr=steps>0?s->endpoint->VolumeStepUp(nullptr):s->endpoint->VolumeStepDown(nullptr);
    if(FAILED(hr)) {Wh_Log(L"[T81-DIAG] volume wheel failed hr=0x%08lx",(unsigned long)hr);s->endpoint->Release();s->endpoint=nullptr;return;}
    RefreshVolume();if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);
    if(s->overflowPanel)InvalidateRect(s->overflowPanel,nullptr,FALSE);
}
// Determine physical connection from the IPv4 route first. This is a route
// lookup only: it sends no packet. A VPN/tunnel or IPv6-only setup is ambiguous,
// so fall back to operational physical adapters; this is not reachability nor
// actual Wi-Fi signal strength.
static int NetworkTypeKind(DWORD type) {
    return type==IF_TYPE_IEEE80211?2:(type==IF_TYPE_ETHERNET_CSMACD?1:0);
}
static int ChooseNetworkKind(int routeKind,bool wifi,bool ethernet) {
    return routeKind?routeKind:(wifi?2:(ethernet?1:0));
}
int QueryNetworkKind() {
    DWORD index=0;
    if(GetBestInterface(0x08080808u,&index)==NO_ERROR) {
        MIB_IF_ROW2 route{}; route.InterfaceIndex=index;
        if(GetIfEntry2(&route)==NO_ERROR && route.OperStatus==IfOperStatusUp &&
           route.InterfaceAndOperStatusFlags.HardwareInterface) {
            int kind=NetworkTypeKind(route.Type);
            if(kind) return kind;
        }
    }
    // A VPN's virtual Ethernet adapter is not the physical Wi-Fi connection.
    // On systems supporting MIB_IF_ROW2 consider only operational hardware.
    MIB_IF_TABLE2* table2=nullptr;
    if(GetIfTable2(&table2)==NO_ERROR && table2) {
        bool wifi=false, ethernet=false;
        for(ULONG i=0;i<table2->NumEntries;i++) {
            const MIB_IF_ROW2& row=table2->Table[i];
            if(row.OperStatus!=IfOperStatusUp || !row.InterfaceAndOperStatusFlags.HardwareInterface) continue;
            wifi|=NetworkTypeKind(row.Type)==2; ethernet|=NetworkTypeKind(row.Type)==1;
        }
        FreeMibTable(table2);
        return ChooseNetworkKind(0,wifi,ethernet);
    }
    ULONG size=0;
    if(GetIfTable(nullptr,&size,FALSE)!=ERROR_INSUFFICIENT_BUFFER || !size || size>4*1024*1024) return 0;
    std::vector<BYTE> bytes(size);
    auto* table=reinterpret_cast<MIB_IFTABLE*>(bytes.data());
    if(GetIfTable(table,&size,FALSE)!=NO_ERROR) return 0;
    bool wifi=false, ethernet=false;
    for(DWORD i=0;i<table->dwNumEntries;i++) {
        const MIB_IFROW& row=table->table[i];
        if(row.dwOperStatus!=MIB_IF_OPER_STATUS_OPERATIONAL) continue;
        wifi|=row.dwType==IF_TYPE_IEEE80211; ethernet|=row.dwType==IF_TYPE_ETHERNET_CSMACD;
    }
    return ChooseNetworkKind(0,wifi,ethernet);
}
static void ResizePrivateOverflow();
static void ApplyBatteryStatus(const SYSTEM_POWER_STATUS& p) {
    if(!s) return;
    bool oldPresent=s->hasBattery; int oldPct=s->batteryPct; bool oldCharge=s->charging,oldCharging=s->batteryCharging;
    if(p.BatteryFlag!=255 && (p.BatteryFlag&128)) {s->hasBattery=false; s->batteryPct=-1; s->charging=false;s->batteryCharging=false;}
    else if(p.BatteryFlag!=255 || p.BatteryLifePercent<=100) {
        s->hasBattery=true; s->batteryPct=p.BatteryLifePercent<=100?(int)p.BatteryLifePercent:-1;
        s->charging=p.ACLineStatus==1;
        s->batteryCharging=p.BatteryFlag!=255 && (p.BatteryFlag&8)!=0;
    }
    bool changed=oldPresent!=s->hasBattery || oldPct!=s->batteryPct || oldCharge!=s->charging || oldCharging!=s->batteryCharging;
    if(!changed) return;
    if(s->bar) InvalidateRect(s->bar,nullptr,FALSE);
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel)) {
        if(oldPresent!=s->hasBattery) ResizePrivateOverflow();
        InvalidateRect(s->overflowPanel,nullptr,FALSE);
    }
    // Update the text of an already-visible tooltip without moving the pointer.
    if(s->tooltip && IsWindowVisible(s->tooltip) && s->hot==HIT_BATTERY) {
        if(s->batteryPct<0) s->tooltipText=Tr(L"Batteria: stato sconosciuto",L"Battery: unknown status");
        else s->tooltipText=Tr(L"Batteria: ",L"Battery: ")+std::to_wstring(s->batteryPct)+L"%"+(s->charging?Tr(L" (alimentazione collegata)",L" (plugged in)"):L"");
        InvalidateRect(s->tooltip,nullptr,FALSE);
    }
    if(s->hasBattery) PublishCniSystemIcon(HIT_BATTERY);
    else RemoveCniSystemIcon(HIT_BATTERY);
}
void RefreshBattery() { SYSTEM_POWER_STATUS p{};
     if(GetSystemPowerStatus(&p)) {bool old=s->hasBattery;
    ApplyBatteryStatus(p);
    if(old!=s->hasBattery && s->bar)BuildGroups();
    } }


// Based on the supplied Windows 8.1 screenshot (1440x900, 40 px bar),
// assumed to be 96 DPI: first button begins at x=54, its cadence is 61 px,
// and the notification clock is approximately 80 px wide. Other themes,
// locales and taskbar sizes can differ. Keep these logical, not physical px.
static constexpr int kStartLogical=54, kTaskButtonLogical=61, kClockLogical=80;
// Reference: 70 x 69 x 51 physical px at assumed 125% DPI.
static constexpr int kVerticalWidthLogical=56, kVerticalStartLogical=55, kVerticalTaskLogical=41;
// Orientation groundwork only: no vertical layout or shell AppBar hooks yet.
// Edge/axis classification inspired by m417z, taskbar-vertical.wh.cpp
// (GPL-3.0, https://github.com/m417z/my-windhawk-mods), and RetroBar
// RetroBar/Converters/EdgeOrientationConverter.cs (Apache-2.0, commit
// 69f36278150cb39bc37d3237e3178926734c6804).
// Additional orientation behavior is GPL-3.0-or-later. No XAML/symbol hooks copied.
enum class BarEdge { Bottom, Top, Left, Right };
static bool HorizontalEdge(BarEdge edge){return edge==BarEdge::Top || edge==BarEdge::Bottom;}
static BarEdge EdgeForRect(const RECT& bar,const RECT& monitor) {
    int w=bar.right-bar.left,h=bar.bottom-bar.top;
    if(w<=0 || h<=0) return BarEdge::Bottom;
    if(w>h) return std::abs(bar.top-monitor.top)<=std::abs(monitor.bottom-bar.bottom)?BarEdge::Top:BarEdge::Bottom;
    return std::abs(bar.left-monitor.left)<=std::abs(monitor.right-bar.right)?BarEdge::Left:BarEdge::Right;
}
// Vertical coordinate groundwork only; the UI is still not rotated/registered.
// https://learn.microsoft.com/en-us/windows/win32/shell/application-desktop-toolbars
// ABM_QUERYPOS/SETPOS apply to a registered appbar, not to moving Explorer's
// native taskbar. Do not repurpose them on the native taskbar HWND.
[[maybe_unused]] static POINT EdgeLogicalPoint(BarEdge edge,POINT p,int width,int height) {
    switch(edge) {
    case BarEdge::Left:return POINT{p.y,width-1-p.x};
    case BarEdge::Right:return POINT{height-1-p.y,p.x};
    case BarEdge::Top:return POINT{width-1-p.x,height-1-p.y};
    default:return p;
    }
}
struct EdgeDelta {int away,along;};
// Direct C++ translation of RetroBar Taskbar.xaml.cs DragCoordsToScreenEdge,
// commit 69f36278150cb39bc37d3237e3178926734c6804, Apache-2.0. Top/bottom are supported normally; left/right are acted on only when the
// separate All edges placement mode is selected.
static BarEdge RetroBarDragCoordsToScreenEdge(int x,int y,const RECT& screen) {
    double relativeX=((double)x-screen.left)/(screen.right-screen.left);
    double relativeY=((double)y-screen.top)/(screen.bottom-screen.top);
    if(relativeX<0.5 && relativeY<0.5) {
        if(relativeX>=relativeY)return BarEdge::Top;
        else return BarEdge::Left;
    } else if(relativeX>=0.5 && relativeY<0.5) {
        relativeX-=0.5;
        if(relativeX+relativeY<0.5)return BarEdge::Top;
        else return BarEdge::Right;
    } else if(relativeX<0.5 && relativeY>=0.5) {
        relativeY-=0.5;
        if(relativeX+relativeY<0.5)return BarEdge::Left;
        else return BarEdge::Bottom;
    } else {
        if(relativeX>=relativeY)return BarEdge::Right;
        else return BarEdge::Bottom;
    }
}
static EdgeDelta DragRelativeToEdge(BarEdge edge,int dx,int dy) {
    switch(edge) {case BarEdge::Top:return {dy,dx};case BarEdge::Left:return {dx,dy};case BarEdge::Right:return {-dx,dy};default:return {-dy,dx};}
}
static const wchar_t* EdgeName(BarEdge edge) {
    switch(edge){case BarEdge::Top:return L"top";case BarEdge::Left:return L"left";case BarEdge::Right:return L"right";default:return L"bottom";}
}
struct Layout {
    RECT start, clock, language, desktop, overflow, tray, desktopBand, linksBand, addressBand;
    int first, width, count; bool labels=false;
};
static void RefreshNativeLabelPreference() {
    if(!s) return; DWORD value=0,size=sizeof(value); int previous=s->nativeGlomLevel;
    if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",L"TaskbarGlomLevel",RRF_RT_REG_DWORD,nullptr,&value,&size)==ERROR_SUCCESS && value<=2) s->nativeGlomLevel=(int)value;
    else s->nativeGlomLevel=-1;
    if(previous!=s->nativeGlomLevel) {BuildGroups();if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);}
}
// Public Win32, WinRT and NLS input-profile contracts. The three-letter NLS
// language value and its region are kept on separate presentation lines.
struct InputLanguageInfo {
    std::wstring tag,abbreviation,region,displayName,nativeName,scripts;
};
static std::wstring ConsumeHString(HSTRING value) {
    if(!value)return {};
    UINT32 length=0;const wchar_t* data=WindowsGetStringRawBuffer(value,&length);
    std::wstring result=data && length?std::wstring(data,length):std::wstring{};
    WindowsDeleteString(value);return result;
}
static std::wstring CurrentInputMethodLanguageTag() {
    using ABI::Windows::Globalization::ILanguageStatics;
    HSTRING classId=nullptr;
    if(FAILED(WindowsCreateString(RuntimeClass_Windows_Globalization_Language,
            (UINT32)wcslen(RuntimeClass_Windows_Globalization_Language),&classId)))return {};
    ILanguageStatics* raw=nullptr;
    HRESULT hr=RoGetActivationFactory(classId,IID_PPV_ARGS(&raw));
    WindowsDeleteString(classId);
    std::unique_ptr<ILanguageStatics,ReleaseCom<ILanguageStatics>> statics(raw);
    if(FAILED(hr) || !raw)return {};
    HSTRING value=nullptr;hr=raw->get_CurrentInputMethodLanguageTag(&value);
    return SUCCEEDED(hr)?ConsumeHString(value):std::wstring{};
}
static std::wstring WinRtLanguageDisplayName(const std::wstring& tag) {
    using ABI::Windows::Globalization::ILanguage;
    using ABI::Windows::Globalization::ILanguageFactory;
    if(tag.empty() || tag.size()>256)return {};
    HSTRING classId=nullptr;
    if(FAILED(WindowsCreateString(RuntimeClass_Windows_Globalization_Language,
            (UINT32)wcslen(RuntimeClass_Windows_Globalization_Language),&classId)))return {};
    ILanguageFactory* rawFactory=nullptr;
    HRESULT hr=RoGetActivationFactory(classId,IID_PPV_ARGS(&rawFactory));
    WindowsDeleteString(classId);
    std::unique_ptr<ILanguageFactory,ReleaseCom<ILanguageFactory>> factory(rawFactory);
    if(FAILED(hr) || !rawFactory)return {};
    HSTRING tagHandle=nullptr;
    if(FAILED(WindowsCreateString(tag.c_str(),(UINT32)tag.size(),&tagHandle)))return {};
    ILanguage* rawLanguage=nullptr;hr=rawFactory->CreateLanguage(tagHandle,&rawLanguage);
    WindowsDeleteString(tagHandle);
    std::unique_ptr<ILanguage,ReleaseCom<ILanguage>> language(rawLanguage);
    if(FAILED(hr) || !rawLanguage)return {};
    HSTRING value=nullptr;hr=rawLanguage->get_DisplayName(&value);
    return SUCCEEDED(hr)?ConsumeHString(value):std::wstring{};
}
static bool LanguageTagMatchesHkl(HKL hkl,const std::wstring& resolvedTag) {
    if(!hkl)return true;
    LANGID lang=LOWORD((ULONG_PTR)hkl);
    // Transient Windows 8+ keyboard LANGIDs need the BCP-47 tag. For ordinary
    // LANGIDs, reject stale foreground tags from another language or region.
    if(!PRIMARYLANGID(lang))return true;
    LCID lcid=LocaleNameToLCID(resolvedTag.c_str(),0);
    if(!lcid || PRIMARYLANGID((LANGID)lcid)!=PRIMARYLANGID(lang))return false;
    wchar_t hklLocale[LOCALE_NAME_MAX_LENGTH]{};
    if(LCIDToLocaleName(MAKELCID(lang,SORT_DEFAULT),hklLocale,(int)std::size(hklLocale),0)>1) {
        wchar_t tagRegion[16]{},hklRegion[16]{};
        int tagLength=GetLocaleInfoEx(resolvedTag.c_str(),LOCALE_SISO3166CTRYNAME,tagRegion,(int)std::size(tagRegion));
        int hklLength=GetLocaleInfoEx(hklLocale,LOCALE_SISO3166CTRYNAME,hklRegion,(int)std::size(hklRegion));
        if(tagLength>1 && hklLength>1 && wcscmp(tagRegion,L"ZZZ") && wcscmp(hklRegion,L"ZZZ") &&
           _wcsicmp(tagRegion,hklRegion))return false;
    }
    return true;
}
static std::wstring InputLocaleName(HKL hkl,const std::wstring& tag,bool* tagUsed=nullptr) {
    if(tagUsed)*tagUsed=false;
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if(!tag.empty() && ResolveLocaleName(tag.c_str(),locale,(int)std::size(locale))>1 &&
       LanguageTagMatchesHkl(hkl,locale)) {
        if(tagUsed)*tagUsed=true;
        return locale;
    }
    if(hkl) {
        LANGID lang=LOWORD((ULONG_PTR)hkl);
        if(LCIDToLocaleName(MAKELCID(lang,SORT_DEFAULT),locale,(int)std::size(locale),0)>1)
            return locale;
    }
    return {};
}
static std::wstring LocaleInfoString(const std::wstring& locale,LCTYPE type) {
    if(locale.empty())return {};
    wchar_t value[256]{};int length=GetLocaleInfoEx(locale.c_str(),type,value,(int)std::size(value));
    return length>1?std::wstring(value,length-1):std::wstring{};
}
static InputLanguageInfo QueryInputLanguageInfo(HKL hkl,const std::wstring& tag={}) {
    InputLanguageInfo info;bool tagUsed=false;
    std::wstring locale=InputLocaleName(hkl,tag,&tagUsed);info.tag=locale;
    info.abbreviation=LocaleInfoString(locale,LOCALE_SISO639LANGNAME2);
    if(!info.abbreviation.empty())
        CharUpperBuffW(info.abbreviation.data(),(DWORD)info.abbreviation.size());
    info.region=LocaleInfoString(locale,LOCALE_SISO3166CTRYNAME);
    if(info.region==L"ZZZ")info.region.clear();
    if(!info.region.empty())CharUpperBuffW(info.region.data(),(DWORD)info.region.size());
    info.nativeName=LocaleInfoString(locale,LOCALE_SNATIVELANGNAME);
    info.scripts=LocaleInfoString(locale,LOCALE_SSCRIPTS);
    if(tagUsed)info.displayName=WinRtLanguageDisplayName(locale);
    if(info.displayName.empty())info.displayName=LocaleInfoString(locale,LOCALE_SLOCALIZEDDISPLAYNAME);
    return info;
}
static std::vector<HKL> QueryKeyboardLayouts() {
    int count=GetKeyboardLayoutList(0,nullptr);
    if(count<=0 || count>64)return {};
    std::vector<HKL> layouts((size_t)count);
    int fetched=GetKeyboardLayoutList(count,layouts.data());
    if(fetched<=0)return {};
    layouts.resize((size_t)std::min(fetched,count));
    layouts.erase(std::remove(layouts.begin(),layouts.end(),nullptr),layouts.end());
    return layouts;
}
static bool OwnInputSurface(HWND hwnd) {
    if(!s || !hwnd)return false;
    return hwnd==s->bar || hwnd==s->native || hwnd==s->privateHost || hwnd==s->privateNotify ||
           hwnd==s->overflowPanel || hwnd==s->preview || hwnd==s->tooltip || hwnd==s->jumpPopup ||
           hwnd==s->desktopPopup || hwnd==s->addressHost || hwnd==s->languageFlyout;
}
static HWND InputLanguageTarget(bool remember=true) {
    HWND foreground=GetForegroundWindow();
    if(foreground && !OwnInputSurface(foreground)) {
        if(remember && s)s->languageTarget=foreground;
        return foreground;
    }
    if(s && s->languageTarget && IsWindow(s->languageTarget))return s->languageTarget;
    return nullptr;
}
static HKL ActiveInputLanguage() {
    HWND target=InputLanguageTarget();
    DWORD thread=target?GetWindowThreadProcessId(target,nullptr):0;
    HKL hkl=GetKeyboardLayout(thread);
    if(!hkl)hkl=GetKeyboardLayout(0);
    return hkl;
}
static int MeasureLanguageWidth(const std::wstring& text) {
    if(text.empty())return 0;
    int minimum=Px(34),maximum=Px(96),padding=Px(14),width=minimum;
    HDC dc=s && s->bar?GetDC(s->bar):nullptr;
    HFONT font=dc?CreateTaskbarFont(Dpi(),s->perPixelBar):nullptr;
    if(dc && font) {
        HGDIOBJ old=SelectObject(dc,font);SIZE size{};
        if(GetTextExtentPoint32W(dc,text.c_str(),(int)text.size(),&size))
            width=std::max(minimum,std::min(maximum,(int)size.cx+padding));
        if(old && old!=HGDI_ERROR)SelectObject(dc,old);
    }
    if(font)DeleteObject(font);
    if(dc)ReleaseDC(s->bar,dc);
    return width;
}
static void HideLanguageFlyout();
static void DestroyLanguageFlyout();
static void PopulateLanguageFlyoutItems();
static void RedrawLanguageFlyout(HWND hwnd);
static void SyncLanguageFlyoutHover(HWND hwnd);
static void ConfirmLanguageFlyoutRequest();
static HKL g_pendingLanguageFlyoutHkl=nullptr;
static DWORD g_pendingLanguageFlyoutThread=0;
static int g_pendingLanguageFlyoutAttempts=0;
static bool LanguageIndicatorVisible() {
    return s && s->privateActive && showLanguageIndicator.load() &&
           s->languageLayoutCount>1 && !s->languageText.empty();
}
static int LanguageIndicatorWidth() {return LanguageIndicatorVisible()?std::max(Px(34),s->languageWidthPx):0;}
static int LanguageIndicatorHeight() {return LanguageIndicatorVisible()?Px(24):0;}
static void RefreshLanguageIndicator() {
    if(!s)return;
    if(!showLanguageIndicator.load())HideLanguageFlyout();
    bool oldVisible=LanguageIndicatorVisible();
    int oldWidth=s->languageWidthPx;HKL oldHkl=s->languageHkl;
    size_t oldCount=s->languageLayoutCount;
    std::wstring oldText=s->languageText,oldRegion=s->languageRegion,oldName=s->languageName,oldTag=s->languageTag;
    auto layouts=QueryKeyboardLayouts();HKL active=ActiveInputLanguage();
    std::wstring tag=CurrentInputMethodLanguageTag();
    InputLanguageInfo info=QueryInputLanguageInfo(active,tag);
    s->languageLayoutCount=layouts.size();s->languageHkl=active;s->languageTag=info.tag;
    s->languageText=info.abbreviation;s->languageRegion=info.region;
    s->languageName=info.displayName.empty()?info.abbreviation:info.displayName;
    s->languageWidthPx=std::max(MeasureLanguageWidth(s->languageText),MeasureLanguageWidth(s->languageRegion));s->languageMetricsDpi=Dpi();
    bool visible=LanguageIndicatorVisible();
    bool geometry=visible!=oldVisible || (visible && s->languageWidthPx!=oldWidth);
    if(geometry && s->bar)BuildGroups();
    if(s->bar && (geometry || oldText!=s->languageText || oldRegion!=s->languageRegion ||
       oldName!=s->languageName || oldTag!=s->languageTag || oldHkl!=active || oldCount!=layouts.size()))
        InvalidateRect(s->bar,nullptr,FALSE);
    if(s->languageFlyout && IsWindowVisible(s->languageFlyout)) {
        PopulateLanguageFlyoutItems();SyncLanguageFlyoutHover(s->languageFlyout);RedrawLanguageFlyout(s->languageFlyout);
    }
}
static int PrivateTrayWidth();
static Layout TaskAreaLayout(int w,int h,int& avail) {
    Layout L{};
    const bool beta8148=useBetaStartLogo.load() && s->betaStartImage;
    // Windows 8-style layout: retain a small empty leading margin when Start is hidden.
    int start=std::min(w,Px(startHotCorner.load()?8:kStartLogical));
    int desk=s->preservingTray ? 0 : std::min(std::max(0,w-start),Px(beta8148?15:8));
    int desiredClock=std::max(Px(kClockLogical),s->clockWidthPx);
    int clock=s->preservingTray ? 0 : std::min(std::max(0,w-start-desk),desiredClock);
    int language=s->preservingTray?0:std::min(std::max(0,w-start-desk-clock),LanguageIndicatorWidth());
    L.start={0,0,startHotCorner.load()?std::min(w,std::max(1,Px(2))):start,h}; L.desktop={w-desk,0,w,h};
    L.clock={w-desk-clock,0,w-desk,h};
    // 8148 only: leave a small visual gap after Start. L.first is the
    // shared origin for painting, hit testing, thumbnails and drag/drop.
    L.first=std::min((int)L.clock.left,start+(beta8148 && !startHotCorner.load()?Px(4):0));
    int tw=std::min(PrivateTrayWidth(),std::max(0,(int)L.clock.left-language-start));
    int bandRight;
    if(taskbarSkin.load()==1) {
        // Win7: independent language cell on the task side of the tray.
        L.tray={L.clock.left-tw,0,L.clock.left,h};
        L.language={L.tray.left-language,0,L.tray.left,h};
        bandRight=L.language.left;
    } else {
        L.language={L.clock.left-language,0,L.clock.left,h};
        L.tray={L.language.left-tw,0,L.language.left,h};
        bandRight=L.tray.left;
    }
    // Bands are our own compact implementations, not the Explorer deskband.
    // Leave room for at least one task button on narrow displays.
    int budget=std::max(0,bandRight-L.first-Px(kTaskButtonLogical));
    for(int index:s->bandOrder) {
        bool visible=index==0?s->desktopBand:index==1?s->linksBand:index==2?s->addressBand:false;
        int width=Px(index==0?65:index==1?95:210);if(!visible || budget<width)continue;
        RECT rect{bandRight-width,0,bandRight,h};
        if(index==0)L.desktopBand=rect;else if(index==1)L.linksBand=rect;else L.addressBand=rect;
        bandRight-=width;budget-=width;
    }
    avail=std::max(0,bandRight-L.first);
    return L;
}
static int CurrentTaskAreaWidth() {
    if(!s || !s->bar)return 0;
    RECT r{};if(!GetClientRect(s->bar,&r))return 0;
    int available=0;TaskAreaLayout(r.right,r.bottom,available);return available;
}
Layout GetLayout(int w,int h) {
    int avail=0;Layout L=TaskAreaLayout(w,h,avail);
    int bandRight=L.first+avail;
    L.labels=s->labelMode;
    L.width=std::max(1,Px(L.labels?150:kTaskButtonLogical));
    int capacity=avail/L.width;
    if((int)s->groups.size()>capacity && avail>=Px(34)) {
        int overflowW=Px(34);
        L.overflow={bandRight-overflowW,0,bandRight,h};
        capacity=std::max(0,(avail-overflowW)/L.width);
    }
    L.count=std::min((int)s->groups.size(),capacity);
    return L;
}
void DrawNetworkGlyph(Graphics& g, float cx, float cy, float u, int kind, bool dark=false) {
    int quality=wifiQuality.load();
    int index=kind==1?1:kind==2?(quality>=0?2+std::min(5,quality*6/101):-1):0;
    if(index>=0 && s && DrawLegacyImage(g,s->netImages[index],cx,cy,u)) return;
    SolidBrush white(dark?Color(kind?255:160,30,68,104):Color(kind ? 255 : 130,245,253,255));
    if (kind == 1) {
        g.FillRectangle(&white, cx - 6.5f * u, cy - 6.5f * u, 13 * u, 9 * u);
        SolidBrush screen(dark?Color(255,235,245,252):Color(255,22,70,120));
        g.FillRectangle(&screen, cx - 5.2f * u, cy - 5.2f * u, 10.4f * u, 6.4f * u);
        g.FillRectangle(&white, cx - 1 * u, cy + 2.5f * u, 2 * u, 2.5f * u);
        g.FillRectangle(&white, cx - 4.5f * u, cy + 5 * u, 9 * u, 1.6f * u);
        return;
    }
    for (int i = 0; i < 4; ++i) {
        const float bh = (3 + i * 3) * u;
        g.FillRectangle(&white, cx - 7.5f * u + i * 4.2f * u, cy + 6.5f * u - bh, 3.2f * u, bh);
    }
    if (!kind) {
        SolidBrush red(Color(255, 214, 60, 52));
        g.FillEllipse(&red, cx + 2 * u, cy - 1 * u, 6.5f * u, 6.5f * u);
    }
}
void DrawVolumeGlyph(Graphics& g, float cx, float cy, float u, float vol, bool muted, bool dark=false) {
    int frame=muted?0:(vol<=0.01f?1:(vol<=0.33f?2:(vol<=0.66f?3:4)));
    if(s && DrawLegacyImage(g,s->volumeImages[frame],cx,cy,u)) return;
    SolidBrush white(dark?Color(255,30,68,104):Color(255,245,253,255));
    PointF body[] = {{cx - 8 * u, cy - 2.5f * u}, {cx - 4.5f * u, cy - 2.5f * u}, {cx - 0.5f * u, cy - 6.5f * u},
                     {cx - 0.5f * u, cy + 6.5f * u}, {cx - 4.5f * u, cy + 2.5f * u}, {cx - 8 * u, cy + 2.5f * u}};
    g.FillPolygon(&white, body, 6);
    if (muted) {
        Pen red(Color(255, 226, 70, 60), std::max(1.2f, 1.6f * u));
        g.DrawLine(&red, cx + 2 * u, cy - 3.5f * u, cx + 8 * u, cy + 3.5f * u);
        g.DrawLine(&red, cx + 8 * u, cy - 3.5f * u, cx + 2 * u, cy + 3.5f * u);
        return;
    }
    Pen wave(dark?Color(255,30,68,104):Color(255,245,253,255),std::max(1.0f,1.3f*u));
    const int waves = vol > 0.66f ? 3 : (vol > 0.33f ? 2 : (vol > 0.01f ? 1 : 0));
    for (int i = 0; i < waves; ++i) {
        const float r = (3.2f + i * 2.9f) * u;
        g.DrawArc(&wave, cx - 0.5f * u - r, cy - r, 2 * r, 2 * r, -50.0f, 100.0f);
    }
}
void DrawBatteryGlyph(Graphics& g, float cx, float cy, float u, int pct, bool charging, bool dark=false) {
    if(pct>=0 && pct<=100) {
        int frame=charging?33+(pct*9+50)/100:(pct*8+50)/100;
        if(DrawLegacyBattery(g,frame,cx,cy,u)) return;
    }
    Color ink=dark?Color(255,30,68,104):Color(255,245,253,255);
    SolidBrush white(ink);
    Pen outline(ink,std::max(1.0f,1.2f*u));
    const float bw = 14 * u, bh = 8 * u;
    const float x = cx - bw / 2 - 0.5f * u, y = cy - bh / 2;
    g.DrawRectangle(&outline, x, y, bw, bh);
    g.FillRectangle(&white, x + bw + 0.5f * u, cy - 2 * u, 1.8f * u, 4 * u);
    const float fill = std::max(0.0f, std::min(1.0f, pct / 100.0f)) * (bw - 3 * u);
    SolidBrush level(pct <= 15 && !charging ? Color(255,226,70,60) : ink);
    g.FillRectangle(&level, x + 1.5f * u, y + 1.5f * u, fill, bh - 3 * u);
    if (charging) {
        // Keep the charge indicator outside the fill, so the level remains
        // visible even while the machine is charging.
        PointF b[] = {{cx + 9.0f*u, cy-4.0f*u}, {cx + 7.5f*u, cy+0.3f*u}, {cx + 9.0f*u, cy+0.3f*u},
                      {cx + 8.0f*u, cy+4.0f*u}, {cx + 11.0f*u, cy-0.7f*u}, {cx + 9.5f*u, cy-0.7f*u}};
        g.FillPolygon(&white, b, 6);
    }
}

// Fixed-size fields are read only if both cbData and the sender's cbSize cover
// them. WM_COPYDATA itself is synchronously marshalled by USER32.
static bool OverflowIconScreenRect(int index, RECT* result);
static void ResizePrivateOverflow();
static void HidePrivateOverflow();
static bool TrayFieldAvailable(size_t payload, DWORD cbSize, size_t end) {
    return cbSize>=end && payload>=offsetof(TrayWireData,icon)+end;
}
static int FindPrivateIcon(HWND owner,DWORD id,const GUID& guid) {
    for(size_t i=0;i<s->trayIcons.size();++i)
        if(MatchesIcon(s->trayIcons[i],owner,id,guid)) return (int)i;
    return -1;
}
static std::wstring TrayPreferenceKey(const PrivateIcon& icon) {
    if(GuidPresent(icon.guid)) {
        wchar_t text[40]={}; StringFromGUID2(icon.guid,text,(int)std::size(text));
        std::wstring key=L"app:tray-guid:"; key+=text;
        std::transform(key.begin(),key.end(),key.begin(),[](wchar_t c){return (wchar_t)towlower(c);});
        return key;
    }
    std::wstring path=NormalizedMatchPath(ProcessImage(icon.ownerPid));
    if(path.empty()) {
        // No stable executable identity: usable for this session only.
        return L"exe:tray-session:"+std::to_wstring(icon.ownerPid)+L":"+std::to_wstring(icon.id);
    }
    return L"exe:tray:"+path+L":"+std::to_wstring(icon.id);
}
// Negative entries in the flyout are reserved for the mod's synthetic
// system icons. -1 remains the ordinary "no hit" value.
static int SystemEntry(int hit) { return -2000+hit; }
static int SystemHitForEntry(int entry) {
    for(int hit:{HIT_NETWORK,HIT_VOLUME,HIT_BATTERY}) if(entry==SystemEntry(hit)) return hit;
    return 0;
}
static const wchar_t* SystemKey(int hit) {
    return hit==HIT_NETWORK?L"app:tray-system:network":hit==HIT_VOLUME?L"app:tray-system:volume":
           hit==HIT_BATTERY?L"app:tray-system:battery":L"";
}
static int SystemHitForKey(const std::wstring& key) {
    for(int hit:{HIT_NETWORK,HIT_VOLUME,HIT_BATTERY}) if(key==SystemKey(hit)) return hit;
    return 0;
}
static bool IsSystemVisible(int hit) {
    if(!s || (hit==HIT_BATTERY && !s->hasBattery)) return false;
    if(s->trayShowAll) return true;
    return std::find(s->trayHiddenSystem.begin(),s->trayHiddenSystem.end(),SystemKey(hit))==s->trayHiddenSystem.end();
}
static void SaveSystemPreferences() {
    if(!s) return;
    std::wstring value=EncodeOrder(s->trayHiddenSystem);
    if(!Wh_SetStringValue(L"Taskbar81.TrayHiddenSystem",value.c_str())) Wh_Log(L"Could not save system icon placement");
}
static void SaveTrayPreferences() {
    if(!s) return;
    auto copy=s->trayPinned;
    copy.erase(std::remove_if(copy.begin(),copy.end(),[](const std::wstring& v){return v.rfind(L"exe:tray-session:",0)==0;}),copy.end());
    std::wstring encoded=EncodeOrder(copy);
    if(!Wh_SetStringValue(L"Taskbar81.TrayPinned",encoded.c_str())) { Wh_Log(L"Could not save tray preferences"); return; }
    if(!Wh_SetStringValue(L"Taskbar81.TrayPinnedInit",L"1")) Wh_Log(L"Could not save tray preference marker");
    else s->trayPreferencesInitialized=true;
}
static void SaveOverflowOrder() {
    if(!s)return;
    auto copy=s->trayOverflowOrder;
    copy.erase(std::remove_if(copy.begin(),copy.end(),[](const std::wstring& v){return v.rfind(L"exe:tray-session:",0)==0;}),copy.end());
    std::wstring encoded=EncodeOrder(copy);
    if(!Wh_SetStringValue(L"Taskbar81.TrayOverflowOrder",encoded.c_str())) Wh_Log(L"Could not save overflow icon order");
}
static void SaveTrayBarOrder() {
    if(!s)return;
    auto copy=s->trayBarOrder;
    copy.erase(std::remove_if(copy.begin(),copy.end(),[](const std::wstring& v){return v.rfind(L"exe:tray-session:",0)==0;}),copy.end());
    auto encoded=EncodeOrder(copy);
    if(!Wh_SetStringValue(L"Taskbar81.TrayBarOrder",encoded.c_str()))Wh_Log(L"Could not save tray bar order");
}
static void LoadTrayPreferences() {
    if(!s) return;
    std::vector<wchar_t> b(32768,L'\0');
    size_t n=Wh_GetStringValue(L"Taskbar81.TrayPinned",b.data(),b.size());
    if(!n || n>=b.size() || !DecodeOrder(std::wstring(b.data(),n),s->trayPinned)) s->trayPinned.clear();
    wchar_t marker[4]={};
    s->trayPreferencesInitialized=!s->trayPinned.empty() ||
        (Wh_GetStringValue(L"Taskbar81.TrayPinnedInit",marker,std::size(marker))==1 && marker[0]==L'1');
    std::fill(b.begin(),b.end(),L'\0');
    n=Wh_GetStringValue(L"Taskbar81.TrayHiddenSystem",b.data(),b.size());
    std::vector<std::wstring> hidden;
    s->trayHiddenSystem.clear();
    if(n && n<b.size() && DecodeOrder(std::wstring(b.data(),n),hidden))
        for(const auto& key:hidden) if(SystemHitForKey(key)) s->trayHiddenSystem.push_back(key);
    std::fill(b.begin(),b.end(),L'\0');n=Wh_GetStringValue(L"Taskbar81.TrayOverflowOrder",b.data(),b.size());
    if(!n || n>=b.size() || !DecodeOrder(std::wstring(b.data(),n),s->trayOverflowOrder))s->trayOverflowOrder.clear();
    std::fill(b.begin(),b.end(),L'\0');n=Wh_GetStringValue(L"Taskbar81.TrayBarOrder",b.data(),b.size());
    if(!n || n>=b.size() || !DecodeOrder(std::wstring(b.data(),n),s->trayBarOrder))s->trayBarOrder.clear();
}
// [T81-CNI] Private Notification Area Icons provider. Defined below,
// after SetTrayPinned; all calls execute on the owning UI thread.
static bool StartTrayNotifyServer();
static void StopTrayNotifyServer();
static void NotifyTrayCni(ULONG event, const PrivateIcon& icon);
static void InitializeCniSystemIcons();
static void RemoveAllCniSystemIcons();

static LRESULT ProcessRegisteredSystemTrayCopy(const COPYDATASTRUCT* data,
                                               const TrayWireData& wire,int hit) {
    int slot=CniSystemSlot(hit);
    if(!s || slot<0) return 0;
    const auto& n=wire.icon;
    HWND owner=(HWND)(ULONG_PTR)n.hwnd32;
    GUID guid=n.guid;
    PrivateIcon& icon=s->cniSystemIcons[slot];
    bool registered=s->cniSystemRegistered[slot];
    if(wire.message==NIM_DELETE) {
        if(!registered) return 1;
        // The CPL may retain both pointers until its synchronous Notify returns.
        NotifyTrayCni(2,icon);
        if(icon.icon) DestroyIcon(icon.icon);
        icon=PrivateIcon{};
        s->cniSystemRegistered[slot]=false;
        return 1;
    }
    if(wire.message==NIM_SETVERSION) {
        if(!registered || !TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,version)+sizeof(DWORD)) || n.version>4) return 0;
        icon.version=n.version;
        NotifyTrayCni(1,icon);
        return 1;
    }
    if(wire.message!=NIM_ADD && wire.message!=NIM_MODIFY) return 0;
    bool added=!registered;
    if(added) {
        if(!owner || !IsWindow(owner)) return 0;
        if(icon.icon) DestroyIcon(icon.icon);
        icon=PrivateIcon{};
    }
    if(owner && IsWindow(owner)) {
        icon.owner=owner; icon.id=n.id;
        GetWindowThreadProcessId(owner,&icon.ownerPid);
    }
    if(GuidPresent(guid)) icon.guid=guid;
    icon.preferenceKey=SystemKey(hit);
    icon.importedFromNative=false;
    if((n.flags&NIF_MESSAGE) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,callback)+sizeof(DWORD))) icon.callback=n.callback;
    if((n.flags&NIF_STATE) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,stateMask)+sizeof(DWORD)) && (n.stateMask&NIS_HIDDEN)) icon.hidden=(n.state&NIS_HIDDEN)!=0;
    if(n.flags&NIF_TIP) {
        size_t limit=std::min<size_t>(n.cbSize,data->cbData-sizeof(DWORD)*2);
        if(limit>offsetof(TrayWireIcon,tip)) {
            size_t count=std::min<size_t>(128,(limit-offsetof(TrayWireIcon,tip))/sizeof(WCHAR));
            size_t len=0; while(len<count && n.tip[len]) ++len;
            icon.tip.assign(n.tip,n.tip+len);
        }
    }
    if((n.flags&NIF_ICON) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,icon32)+sizeof(DWORD))) {
        HICON duplicate=n.icon32?CopyIcon((HICON)(ULONG_PTR)n.icon32):nullptr;
        if(duplicate || !n.icon32) { if(icon.icon) DestroyIcon(icon.icon); icon.icon=duplicate; }
    }
    s->cniSystemRegistered[slot]=true;
    NotifyTrayCni(added?0:1,icon);
    // No BuildGroups/overflow refresh: this registration is CNI-only.
    return 1;
}

static LRESULT ProcessTrayCopy(const COPYDATASTRUCT* data) {
    if(!data || !data->lpData || data->dwData!=1 ||
       data->cbData<offsetof(TrayWireData,icon)+offsetof(TrayWireIcon,callback)) return 0;
    TrayWireData wire{};
    memcpy(&wire,data->lpData,std::min<size_t>(data->cbData,sizeof(wire)));
    auto& n=wire.icon;
    if(n.cbSize<offsetof(TrayWireIcon,callback)) return 0;
    HWND owner=(HWND)(ULONG_PTR)n.hwnd32;
    GUID guid{};
    if((n.flags&NIF_GUID) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,guid)+sizeof(GUID))) guid=n.guid;
    int registeredSystem=CniSystemHitFromGuid(guid);
    if(registeredSystem) return ProcessRegisteredSystemTrayCopy(data,wire,registeredSystem);
    if(IsSystemTrayGuid(guid)) return 1;
    int index=FindPrivateIcon(owner,n.id,guid);
    if(wire.message==NIM_DELETE) {
        if(index<0) return 0;
        // Keep strings and HICON alive during the synchronous callbacks.
        NotifyTrayCni(2, s->trayIcons[index]);
        if(s->trayIcons[index].icon) DestroyIcon(s->trayIcons[index].icon);
        s->trayIcons.erase(s->trayIcons.begin()+index);
    } else if(wire.message==NIM_SETVERSION) {
        if(index<0 || !TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,version)+sizeof(DWORD)) || n.version>4) return 0;
        s->trayIcons[index].version=n.version;
        NotifyTrayCni(1, s->trayIcons[index]);
    } else if(wire.message==NIM_ADD || wire.message==NIM_MODIFY) {
        const bool cniAdded=(index<0);
        if(index<0) {
            if(!owner || !IsWindow(owner) || s->trayIcons.size()>=128) return 0;
            PrivateIcon icon; icon.owner=owner; icon.id=n.id;
            GetWindowThreadProcessId(owner,&icon.ownerPid);
            s->trayIcons.push_back(std::move(icon)); index=(int)s->trayIcons.size()-1;
        }
        PrivateIcon& icon=s->trayIcons[index];
        icon.importedFromNative=false; // actual NIM message takes precedence over snapshots
        if(owner && IsWindow(owner)) { icon.owner=owner; icon.id=n.id; GetWindowThreadProcessId(owner,&icon.ownerPid); }
        if(GuidPresent(guid)) icon.guid=guid;
        std::wstring oldKey=icon.preferenceKey;
        icon.preferenceKey=TrayPreferenceKey(icon);
        if(!oldKey.empty() && oldKey!=icon.preferenceKey) {
            auto old=std::find(s->trayPinned.begin(),s->trayPinned.end(),oldKey);
            if(old!=s->trayPinned.end()) { *old=icon.preferenceKey; SaveTrayPreferences(); }
        }
        if((n.flags&NIF_MESSAGE) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,callback)+sizeof(DWORD))) icon.callback=n.callback;
        if((n.flags&NIF_STATE) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,stateMask)+sizeof(DWORD)) && (n.stateMask&NIS_HIDDEN)) icon.hidden=(n.state&NIS_HIDDEN)!=0;
        if(n.flags&NIF_TIP) {
            size_t limit=std::min<size_t>(n.cbSize,data->cbData-sizeof(DWORD)*2);
            if(limit>offsetof(TrayWireIcon,tip)) {
                size_t count=std::min<size_t>(128,(limit-offsetof(TrayWireIcon,tip))/sizeof(WCHAR));
                size_t len=0; while(len<count && n.tip[len]) ++len;
                icon.tip.assign(n.tip,n.tip+len);
            }
        }
        if((n.flags&NIF_ICON) && TrayFieldAvailable(data->cbData,n.cbSize,offsetof(TrayWireIcon,icon32)+sizeof(DWORD))) {
            HICON duplicate=n.icon32?CopyIcon((HICON)(ULONG_PTR)n.icon32):nullptr;
            if(duplicate || !n.icon32) { if(icon.icon) DestroyIcon(icon.icon); icon.icon=duplicate; }
        }
        NotifyTrayCni(cniAdded ? 0 : 1, icon);
    } else return 0;
    if(s->bar) {BuildGroups();InvalidateRect(s->bar,nullptr,FALSE);}
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel)) ResizePrivateOverflow();
    return 1;
}
static RECT PrivateIconRect(int index);
static bool OverflowIconScreenRect(int index, RECT* result);
static void ResizePrivateOverflow();
static void HidePrivateOverflow();
static LRESULT ProcessTrayIdentifier(const COPYDATASTRUCT* data) {
    if(!data || !data->lpData || data->dwData!=3 || data->cbData<sizeof(TrayWireIdentifier)) return 0;
    TrayWireIdentifier wire{}; memcpy(&wire,data->lpData,sizeof(wire));
    int index=FindPrivateIcon((HWND)(ULONG_PTR)wire.hwnd32,wire.id,wire.guid);
    if(index<0 || (wire.message!=1 && wire.message!=2)) return 0;
    RECT r{};
    bool screen=OverflowIconScreenRect(index,&r);
    if(!screen) r=PrivateIconRect(index);
    POINT p{wire.message==1?r.left:r.right,wire.message==1?r.top:r.bottom};
    if(!screen && s->bar) ClientToScreen(s->bar,&p);
    return (LRESULT)(DWORD)MAKELONG((WORD)p.x,(WORD)p.y);
}
static LRESULT ForwardShellMessage(UINT msg,WPARAM wp,LPARAM lp) {
    if(!s || !s->native || !IsWindow(s->native) || s->native==s->privateHost) return 0;
    DWORD_PTR result=0;
    if(msg==WM_USER+372) { PostMessageW(s->native,msg,wp,lp); return 0; }
    return SendMessageTimeoutW(s->native,msg,wp,lp,SMTO_ABORTIFHUNG|SMTO_BLOCK,150,&result) ? (LRESULT)result : 0;
}
static LRESULT CALLBACK PrivateTrayProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    if(!s) return DefWindowProcW(hwnd,msg,wp,lp);
    try {
        if(msg==WM_COPYDATA && lp) {
            auto* data=(const COPYDATASTRUCT*)lp;
            if(data->dwData==1) {
                LRESULT own=ProcessTrayCopy(data);
                // Keep Explorer informed: system shell services may depend on
                // its copy, and NIF_INFO balloons are not implemented here.
                LRESULT native=ForwardShellMessage(msg,wp,lp);
                return own?own:native;
            }
            if(data->dwData==3) {
                LRESULT own=ProcessTrayIdentifier(data); if(own) return own;
            }
            return ForwardShellMessage(msg,wp,lp);
        }
        // Classic Properties sends this pointer-free message to the first
        // Shell_TrayWnd. Defer delivery: its caller may be Explorer's own
        // UI thread. WM_SETTINGCHANGE is a system pointer-message and cannot
        // be PostMessage'd even with null lParam, so a one-shot timer performs
        // a bounded SendMessageTimeout after the current call has returned.
        if(hwnd==s->privateHost && msg==WM_SETTINGCHANGE && wp==SPI_SETLOGICALDPIOVERRIDE && lp==0) {
            if(s->bar && !SetTimer(s->bar,TIMER_CLASSIC_POSITION_NOTIFY,200,nullptr))
                Wh_Log(L"[T81-DIAG] cannot defer classic position notification");
            return 0;
        }
        if(msg==WM_ACTIVATEAPP || msg==WM_COMMAND || msg>=WM_USER) return ForwardShellMessage(msg,wp,lp);
    } catch(...) { Wh_Log(L"Private tray C++ exception; forwarding message to native shell"); return ForwardShellMessage(msg,wp,lp); }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
static void SendTaskbarCreated() {
    UINT message=RegisterWindowMessageW(L"TaskbarCreated");
    if(message) SendNotifyMessageW(HWND_BROADCAST,message,0,0);
}
// Initial snapshot of Explorer's legacy notification toolbars. Uses GPL-3.0-or-later
// tray-reader behavior and the Apache-2.0 ManagedShell ExplorerTrayService.
// The data behind TBBUTTON::dwData is a
// *private, version-dependent* Explorer structure, not a StartAllBack API.
// This is strictly a read-only best-effort import: no EnableAutoTray toggle,
// no registry changes, no remote injection, no destructive reconciliation.
#pragma pack(push,1)
struct NativeTrayDataSnapshot {
    HWND owner; UINT id, callback; DWORD state; UINT version;
    HICON icon; HANDLE demoteTimer; DWORD userPref, lastSoundTime;
    WCHAR exeName[260], tip[260]; UINT seconds; GUID guid;
};
#pragma pack(pop)
static_assert(offsetof(NativeTrayDataSnapshot,icon)==24, "x64 legacy tray data layout");
static_assert(offsetof(NativeTrayDataSnapshot,guid)==1092, "x64 legacy tray GUID layout");
struct NativeTrayScan {
    bool learnPositions=false, timedOut=false;
    unsigned imported=0, updated=0, inspected=0;
};
static bool ReadCurrentProcess(const void* address,void* target,size_t bytes) {
    SIZE_T read=0;
    return address && ReadProcessMemory(GetCurrentProcess(),address,target,bytes,&read) && read==bytes;
}
static void ReadNativeTrayToolbar(HWND toolbar,bool overflow,NativeTrayScan& scan) {
    if(!s || scan.timedOut || !IsWindow(toolbar)) return;
    DWORD pid=0; GetWindowThreadProcessId(toolbar,&pid);
    if(pid!=GetCurrentProcessId()) return;
    DWORD_PTR count=0;
    if(!SendMessageTimeoutW(toolbar,TB_BUTTONCOUNT,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,120,&count)) return;
    if(count>128) return;
    // A timed-out cross-thread TB_GETBUTTON can still write later. Allocate a
    // dedicated page; if the send times out, retain the page until Explorer
    // exits instead of risking a write to freed memory. Stop after one timeout.
    auto* scratch=static_cast<TBBUTTON*>(VirtualAlloc(nullptr,sizeof(TBBUTTON),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
    if(!scratch) return;
    bool safeToFree=true;
    for(DWORD_PTR n=0;n<count && s->trayIcons.size()<128;++n) {
        DWORD_PTR result=0;
        if(!SendMessageTimeoutW(toolbar,TB_GETBUTTON,n,(LPARAM)scratch,SMTO_ABORTIFHUNG|SMTO_BLOCK,120,&result)) {
            safeToFree=false; scan.timedOut=true;
            Wh_Log(L"Legacy tray import timed out; scratch page retained to avoid late-write use-after-free");
            break;
        }
        ++scan.inspected;
        if(!result || !scratch->dwData) continue;
        NativeTrayDataSnapshot raw{};
        if(!ReadCurrentProcess((const void*)scratch->dwData,&raw,sizeof(raw))) continue;
        if(!raw.owner || !IsWindow(raw.owner) || raw.callback<WM_USER || raw.version>4 || !raw.icon ||
           IsSystemTrayGuid(raw.guid) || CniSystemHitFromGuid(raw.guid)) continue;
        DWORD ownerPid=0; GetWindowThreadProcessId(raw.owner,&ownerPid); if(!ownerPid) continue;
        int existing=FindPrivateIcon(raw.owner,raw.id,raw.guid);
        if(existing>=0 && !s->trayIcons[existing].importedFromNative) continue;
        HICON duplicate=CopyIcon(raw.icon); if(!duplicate) continue;
        if(existing>=0) {
            PrivateIcon& old=s->trayIcons[existing];
            if(old.icon) DestroyIcon(old.icon);
            old.icon=duplicate; old.callback=raw.callback; old.version=raw.version;
            old.owner=raw.owner; old.ownerPid=ownerPid; old.id=raw.id;
            old.hidden=(raw.state&NIS_HIDDEN)!=0;
            raw.tip[std::size(raw.tip)-1]=L'\0'; old.tip=raw.tip;
            ++scan.updated; continue;
        }
        PrivateIcon icon; icon.owner=raw.owner; icon.ownerPid=ownerPid; icon.id=raw.id;
        icon.callback=raw.callback; icon.version=raw.version; icon.guid=raw.guid;
        icon.icon=duplicate; icon.hidden=(raw.state&NIS_HIDDEN)!=0; icon.importedFromNative=true;
        raw.tip[std::size(raw.tip)-1]=L'\0'; icon.tip=raw.tip;
        icon.preferenceKey=TrayPreferenceKey(icon);
        if(scan.learnPositions && !overflow && !(scratch->fsState&TBSTATE_HIDDEN) && !icon.hidden &&
           !icon.preferenceKey.empty() && s->trayPinned.size()<MAX_TASKS &&
           std::find(s->trayPinned.begin(),s->trayPinned.end(),icon.preferenceKey)==s->trayPinned.end())
            s->trayPinned.push_back(icon.preferenceKey);
        s->trayIcons.push_back(std::move(icon));
        ++scan.imported;
    }
    if(safeToFree) VirtualFree(scratch,0,MEM_RELEASE);
}
struct ToolbarEnumContext { NativeTrayScan* scan; bool overflow; };
static BOOL CALLBACK EnumNativeTrayToolbar(HWND h,LPARAM param) {
    auto* ctx=reinterpret_cast<ToolbarEnumContext*>(param);
    wchar_t name[80]={}; GetClassNameW(h,name,(int)std::size(name));
    if(!_wcsicmp(name,TOOLBARCLASSNAMEW)) ReadNativeTrayToolbar(h,ctx->overflow,*ctx->scan);
    return ctx->scan->timedOut?FALSE:TRUE;
}
static unsigned ImportNativeTraySnapshot() {
    if(!s || !s->native || !IsWindow(s->native) || s->nativeSnapshotTimedOut) return 0;
    NativeTrayScan scan{}; scan.learnPositions=!s->trayPreferencesInitialized;
    HWND notify=FindWindowExW(s->native,nullptr,L"TrayNotifyWnd",nullptr);
    if(notify) { ToolbarEnumContext ctx{&scan,false}; EnumChildWindows(notify,EnumNativeTrayToolbar,(LPARAM)&ctx); }
    const wchar_t* overflowClasses[]={L"NotifyIconOverflowWindow",L"TopLevelWindowForOverflowXamlIsland"};
    for(const wchar_t* cls:overflowClasses) {
        HWND root=nullptr;
        while(!scan.timedOut && (root=FindWindowExW(nullptr,root,cls,nullptr))!=nullptr) {
            DWORD pid=0; GetWindowThreadProcessId(root,&pid);
            if(pid!=GetCurrentProcessId()) continue;
            ToolbarEnumContext ctx{&scan,true}; EnumChildWindows(root,EnumNativeTrayToolbar,(LPARAM)&ctx);
        }
    }
    if(scan.learnPositions && scan.imported) SaveTrayPreferences();
    if(scan.timedOut) s->nativeSnapshotTimedOut=true;
    if(scan.imported || scan.timedOut)
        Wh_Log(L"Legacy tray snapshot imported %u and updated %u entries from %u buttons%ls",scan.imported,scan.updated,scan.inspected,scan.timedOut?L" (incomplete)":L"");
    if((scan.imported || scan.updated) && s->bar) InvalidateRect(s->bar,nullptr,FALSE);
    if((scan.imported || scan.updated) && s->overflowPanel && IsWindowVisible(s->overflowPanel)) ResizePrivateOverflow();
    return scan.imported+scan.updated;
}
static bool StartPrivateTray() {
    if(!s || !s->bar || !s->native || !IsWindow(s->native)) return false;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)(ULONG_PTR)&PrivateTrayProc,&module)) return false;
    s->trayModule=module;
    WNDCLASSW wc{}; wc.lpfnWndProc=PrivateTrayProc; wc.hInstance=module; wc.lpszClassName=L"Shell_TrayWnd";
    s->trayAtom=RegisterClassW(&wc);
    if(!s->trayAtom) { Wh_Log(L"Private Shell_TrayWnd class registration failed: %lu",GetLastError()); return false; }
    s->privateHost=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,(LPCWSTR)(ULONG_PTR)(WORD)(s->trayAtom),L"",
        WS_POPUP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,0,1,1,nullptr,nullptr,module,nullptr);
    if(!s->privateHost) { UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)(s->trayAtom),module); s->trayAtom=0; return false; }
    WNDCLASSW notifyClass{}; notifyClass.lpfnWndProc=PrivateTrayProc; notifyClass.hInstance=module;
    notifyClass.lpszClassName=L"TrayNotifyWnd";
    s->notifyAtom=RegisterClassW(&notifyClass);
    if(s->notifyAtom) s->privateNotify=CreateWindowExW(0,(LPCWSTR)(ULONG_PTR)(WORD)s->notifyAtom,L"",WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,0,1,1,s->privateHost,nullptr,module,nullptr);
    if(!s->privateNotify) {
        DestroyWindow(s->privateHost); s->privateHost=nullptr;
        if(s->notifyAtom) UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->notifyAtom,module);
        s->notifyAtom=0; UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->trayAtom,module); s->trayAtom=0;
        return false;
    }
    if(!(GetWindowLongPtrW(s->privateHost,GWL_EXSTYLE)&WS_EX_TOPMOST))
        SetWindowPos(s->privateHost,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    if(FindWindowW(L"Shell_TrayWnd",nullptr)!=s->privateHost) {
        Wh_Log(L"Private tray is not discoverable; leaving native taskbar visible");
        DestroyWindow(s->privateHost); s->privateHost=nullptr; s->privateNotify=nullptr;
        if(s->notifyAtom) UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->notifyAtom,module);
        s->notifyAtom=0; UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)(s->trayAtom),module); s->trayAtom=0; return false;
    }
    s->nativeWasVisible=IsWindowVisible(s->native)!=FALSE;
    s->nativeWasTopmost=(GetWindowLongPtrW(s->native,GWL_EXSTYLE)&WS_EX_TOPMOST)!=0;
    s->privateActive=true;
    RefreshLanguageIndicator();
    // Broadcasting at the same time as importing can make some apps register
    // with a new ID and duplicate. Broadcast only if this backend yielded no
    // usable entries; updates subsequently arrive through the private host.
    s->nativeSnapshotActive=ImportNativeTraySnapshot()!=0;
    if(!s->nativeSnapshotActive) SendTaskbarCreated();
    // Publish before accepting CPL callbacks. The initial enumeration then sees
    // the same persistent registrations as later update notifications.
    InitializeCniSystemIcons();
    // Failure is logged and leaves the native CNI service as fallback.
    StartTrayNotifyServer();
    return true;
}
// RetroBar/ManagedShell also moves the original bar to HWND_BOTTOM and
// hides a separate Start button on a 100 ms guard. The mod does not change
// the global Windows taskbar auto-hide setting.
// Only hide a top-level Explorer Start candidate overlapping the primary bar.
static BOOL CALLBACK HideOwnedStart(HWND h, LPARAM) {
    try {
    if(!s || h==s->bar || h==s->native || h==s->privateHost || !IsWindowVisible(h)) return TRUE;
    DWORD pid=0; DWORD tid=GetWindowThreadProcessId(h,&pid);
    if(pid!=GetCurrentProcessId()) return TRUE;
    wchar_t cls[128]={}; GetClassNameW(h,cls,128);
    ATOM atom=(ATOM)GetClassLongPtrW(h,GCW_ATOM);
    // A top-level BUTTON at the Start position may have a localized caption;
    // global class atoms are not stable across sessions. Further geometry,
    // process and window-style checks below restrict this heuristic.
    bool isButton=!_wcsicmp(cls,L"Button");
    bool known=!_wcsicmp(cls,L"Start") || !_wcsicmp(cls,L"StartButton") || isButton;
    if(!known || (GetWindowLongPtrW(h,GWL_STYLE)&(WS_CAPTION|WS_SYSMENU))) return TRUE;
    RECT r{},bar{}; if(!GetWindowRect(h,&r) || !GetWindowRect(s->bar,&bar)) return TRUE;
    int w=r.right-r.left,ht=r.bottom-r.top;
    if(w<=0 || ht<=0 || w>Px(96) || ht>Px(96) || r.right<=bar.left || r.left>=bar.left+Px(96) ||
       r.bottom<=bar.top || r.top>=bar.bottom) return TRUE;
    auto it=std::find_if(s->hiddenStarts.begin(),s->hiddenStarts.end(),[&](const HiddenStartWindow& e){return e.hwnd==h && e.pid==pid && e.tid==tid && e.cls==atom;});
    if(it==s->hiddenStarts.end()) { s->hiddenStarts.push_back({h,pid,tid,atom}); it=s->hiddenStarts.end()-1; }
    ULONGLONG now=GetTickCount64();
    if(!it->lastLogged || now-it->lastLogged>=10000) {
        Wh_Log(L"[T81-DIAG] hiding-start hwnd=%p class=%ls pid=%lu rect=%ld,%ld,%ld,%ld",h,cls,pid,r.left,r.top,r.right,r.bottom);
        it->lastLogged=now;
    }
    SetWindowPos(h,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_ASYNCWINDOWPOS);
    return TRUE;
    } catch(...) {Wh_Log(L"[T81-DIAG] Start guard C++ exception");return FALSE;}
}
static void RestoreHiddenStarts() {
    if(!s) return;
    for(auto& item:s->hiddenStarts) {
        DWORD pid=0; DWORD tid=GetWindowThreadProcessId(item.hwnd,&pid);
        if(IsWindow(item.hwnd) && pid==item.pid && tid==item.tid && (ATOM)GetClassLongPtrW(item.hwnd,GCW_ATOM)==item.cls)
            SetWindowPos(item.hwnd,nullptr,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_ASYNCWINDOWPOS);
    }
    s->hiddenStarts.clear();
}
static void CancelTrayDrag();
static void RemoveWorkAreaAppBar();
static void StopPrivateTray() {
    if(!s) return;
    DestroyLanguageFlyout();
    // Revoke on the UI thread before icons are destroyed/OleUninitialize.
    StopTrayNotifyServer();
    RemoveAllCniSystemIcons();
    bool active=s->privateActive;
    RemoveWorkAreaAppBar();
    s->privateActive=false;
    if(s->bar) KillTimer(s->bar,TIMER_DESKTOP_PEEK); PeekOff(true);
    CancelTrayDrag();
    if(s->privateHost && IsWindow(s->privateHost)) DestroyWindow(s->privateHost);
    s->privateHost=nullptr; s->privateNotify=nullptr;
    if(s->notifyAtom && s->trayModule) UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->notifyAtom,s->trayModule);
    if(s->trayAtom && s->trayModule) UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)(s->trayAtom),s->trayModule);
    s->trayAtom=0; s->notifyAtom=0;
    HidePrivateOverflow();
    for(auto& icon:s->trayIcons) if(icon.icon) DestroyIcon(icon.icon);
    s->trayIcons.clear();
    for(int slot=0;slot<3;++slot) {
        if(s->cniSystemIcons[slot].icon) DestroyIcon(s->cniSystemIcons[slot].icon);
        s->cniSystemIcons[slot]=PrivateIcon{};
        if(s->cniSystemPublishedIcons[slot]) DestroyIcon(s->cniSystemPublishedIcons[slot]);
        s->cniSystemPublishedIcons[slot]=nullptr;
        s->cniSystemNid[slot]=NOTIFYICONDATAW{};
        s->cniSystemRegistered[slot]=false;
    }
    if(s->nativeHiddenByUs && s->nativeWasVisible && s->native && IsWindow(s->native)) {
        ShowWindowAsync(s->native,SW_SHOWNA);
        SetWindowPos(s->native,s->nativeWasTopmost?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_ASYNCWINDOWPOS);
    }
    s->nativeHiddenByUs=false;
    RestoreHiddenStarts();
    if(active) SendTaskbarCreated();
}
static void GuardPrivateTray() {
    if(!s || !s->privateActive) return;
    if(!s->privateHost || !IsWindow(s->privateHost) || !s->bar || !IsWindow(s->bar)) { StopPrivateTray(); return; }
    SetWindowPos(s->privateHost,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    if(s->native && IsWindow(s->native) && IsWindowVisible(s->native)) {
        ULONGLONG now=GetTickCount64();
        if(!s->nativeHideLastLogged || now-s->nativeHideLastLogged>=10000) {
            Wh_Log(L"[T81-DIAG] hiding-native hwnd=%p",s->native); s->nativeHideLastLogged=now;
        }
        s->nativeHiddenByUs=true;
        ShowWindowAsync(s->native,SW_HIDE);
        SetWindowPos(s->native,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW|SWP_ASYNCWINDOWPOS|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    }
    EnumWindows(HideOwnedStart,0);
}
// Event-based repair reduces the interval in which Explorer's own bar can
// reappear after a balloon/notification. The 100ms guard remains as backup.
static void CALLBACK ShellVisibilityEvent(HWINEVENTHOOK,DWORD event,HWND hwnd,LONG object,LONG child,DWORD,DWORD) {
    if(!s || !hwnd || object!=OBJID_WINDOW || child!=CHILDID_SELF) return;
    DWORD pid=0;GetWindowThreadProcessId(hwnd,&pid);if(pid!=GetCurrentProcessId())return;
    if(event==EVENT_OBJECT_SHOW && s->peekOn) {
        wchar_t startClass[64]{};
        if(GetClassNameW(hwnd,startClass,(int)std::size(startClass)) &&
           !wcscmp(startClass,L"Win8StartScreenMod8045") && IsWindowVisible(hwnd))
            PeekOff(false);
    }
    if(event==EVENT_OBJECT_SHOW && s->privateActive && hwnd==s->native) {
        Wh_Log(L"[T81-DIAG] native bar shown: event-driven rehide");GuardPrivateTray();return;
    }
    if(event==EVENT_OBJECT_CREATE || event==EVENT_OBJECT_SHOW) {
        wchar_t cls[64]{};if(GetClassNameW(hwnd,cls,64) && !wcscmp(cls,L"ClockFlyoutWindow")) {
            BOOL disabled=TRUE;HRESULT hr=DwmSetWindowAttribute(hwnd,DWMWA_TRANSITIONS_FORCEDISABLED,&disabled,sizeof(disabled));
            Wh_Log(L"[T81-DIAG] clock transition disable event=%lu hr=0x%08lx",event,(unsigned long)hr);
        }
    }
}
static void QueueActionFlyoutPosition(int index,POINT screen);
static void SendPrivateIconEvent(int index,UINT message,POINT screen) {
    if(index<0 || (size_t)index>=s->trayIcons.size()) return;
    const auto& icon=s->trayIcons[index]; DWORD pid=0; GetWindowThreadProcessId(icon.owner,&pid);
    if(!icon.owner || !IsWindow(icon.owner) || !pid || pid!=icon.ownerPid || icon.callback<WM_USER) return;
    if(message==WM_LBUTTONUP) QueueActionFlyoutPosition(index,screen);
    WPARAM w=icon.version>3?MAKELONG((WORD)screen.x,(WORD)screen.y):icon.id;
    LPARAM l=message | ((icon.version>3 ? icon.id : 0)<<16);
    PostMessageW(icon.owner,icon.callback,w,l);
}

static bool OpenClassicClock();
// Isolated vertical geometry; uses real groups and the private notification model.
// Task buttons remain upright, not a rotated horizontal surface.
static std::vector<int> VisiblePrivateIcons();
static std::vector<int> VisibleTrayEntries();
static std::wstring TrayEntryKey(int entry);
struct VerticalLayout {RECT start{},clock{},language{},desktop{},arrow{},icons{},network{},volume{},battery{},more{},desktopBand{},linksBand{},addressBand{};
     int first=0, step=0, count=0, offset=0;
    };
static int VerticalClockHeight() {
    wchar_t time[64]{};GetTimeFormatW(LOCALE_USER_DEFAULT,TIME_NOSECONDS,nullptr,nullptr,time,64);
    // Reserve a fourth line for localized time markers such as AM/PM.
    return Px(wcslen(time)>5?72:55);
}
static VerticalLayout GetVerticalLayout(int width,int height) {
    VerticalLayout v{}; if(!s || width<=0 || height<Px(280))return v;
    const bool beta8148=useBetaStartLogo.load() && s->betaStartImage;
    int start=Px(startHotCorner.load()?0:(beta8148?54:kVerticalStartLogical)), desk=Px(beta8148?15:8), clock=VerticalClockHeight(), cell=Px(24), arrow=Px(25);
    const int taskStep=beta8148?50:kVerticalTaskLogical;
    int y=height; v.desktop=RECT{0,y-desk,width,y};y-=desk;
    v.clock=RECT{0,y-clock,width,y};y-=clock;
    int language=LanguageIndicatorHeight();
    if(taskbarSkin.load()!=1) {v.language=RECT{0,y-language,width,y};y-=language;}
    auto entries=VisibleTrayEntries();
    int iconCount=(int)entries.size();v.icons=RECT{0,y-cell*iconCount,width,y};y-=cell*iconCount;
    for(size_t i=0;i<entries.size();++i) {
        RECT r{0,v.icons.top+(LONG)i*cell,width,v.icons.top+(LONG)(i+1)*cell};
        int hit=SystemHitForEntry(entries[i]);
        if(hit==HIT_VOLUME)v.volume=r;else if(hit==HIT_NETWORK)v.network=r;else if(hit==HIT_BATTERY)v.battery=r;
    }
    v.arrow=RECT{0,y-arrow,width,y};y-=arrow;
    if(taskbarSkin.load()==1) {v.language=RECT{0,y-language,width,y};y-=language;}
    // Compact independent toolbar rows. Retain at least two task cells; when
    // space is insufficient the later bands are omitted rather than overlapping.
    for(int index:s->bandOrder) {
        bool visible=index==0?s->desktopBand:index==1?s->linksBand:index==2?s->addressBand:false;
        if(!visible || y-cell<start+(beta8148 && !startHotCorner.load()?Px(4):0)+2*Px(taskStep))continue;
        RECT row{0,y-cell,width,y};y-=cell;
        if(index==0)v.desktopBand=row;else if(index==1)v.linksBand=row;else v.addressBand=row;
    }
    v.start=RECT{0,0,width,startHotCorner.load()?std::max(1,Px(2)):start};
        v.first=start+(beta8148 && !startHotCorner.load()?Px(4):0);
        v.step=std::max(1,Px(taskStep));
    int capacity=std::max(0,(y-v.first)/v.step), total=(int)s->groups.size();
    if(total>capacity && capacity>0) {y-=cell;v.more=RECT{0,y,width,y+cell};capacity=std::max(0,(y-v.first)/v.step);}
    v.count=std::min(total,capacity);v.offset=std::max(0,std::min(s->verticalOffset,std::max(0,total-v.count)));
    return v;
}
static std::vector<int> VisiblePrivateIcons() {
    std::vector<int> list;
    if(!s || !s->privateActive) return list;
    RECT bar{}; if(s->bar) GetClientRect(s->bar,&bar);
    int reserve=Px(kStartLogical+8+25+24*3+kTaskButtonLogical+4)+std::max(Px(kClockLogical),s->clockWidthPx)+LanguageIndicatorWidth();
    int capacity=std::max(0,((int)(bar.right-bar.left)-reserve)/std::max(1,Px(24)));
    if(s->verticalActive) {
        int systems=(IsSystemVisible(HIT_NETWORK)?1:0)+(IsSystemVisible(HIT_VOLUME)?1:0)+(IsSystemVisible(HIT_BATTERY)?1:0);
        int remaining=(int)bar.bottom-Px((useBetaStartLogo.load() && s->betaStartImage?54:kVerticalStartLogical)+8+25+2*(useBetaStartLogo.load() && s->betaStartImage?50:kVerticalTaskLogical))-VerticalClockHeight()-systems*Px(24)-LanguageIndicatorHeight();
        capacity=std::min(8,std::max(0,remaining/std::max(1,Px(24))));
    }
    capacity=std::min(capacity,80);
    // [T81-CNI] Additive fast path: the original pinned-only algorithm
    // below remains unchanged and is used whenever show-all is disabled.
    if(s->trayShowAll) {
        for(size_t i=0;i<s->trayIcons.size();++i) {
            const auto& icon=s->trayIcons[i];
            if(!icon.hidden && icon.icon) list.push_back((int)i);
            if((int)list.size()>=capacity) break;
        }
        if(capacity==0) list.clear();
        return list;
    }
    for(const auto& key:s->trayPinned) {
        for(size_t i=0;i<s->trayIcons.size();++i) {
            const auto& icon=s->trayIcons[i];
            if(!icon.hidden && icon.icon && icon.preferenceKey==key) { list.push_back((int)i); break; }
        }
        if((int)list.size()>=capacity) break;
    }
    if(capacity==0) list.clear();
    return list;
}
// RetroBar Controls/NotifyIconList.xaml.cs UpdateIconOrder and
// Utilities/NotifyIconDropHandler.cs, commit 69f36278150cb39bc37d3237e3178926734c6804,
// Apache-2.0: https://github.com/dremin/RetroBar/blob/master/LICENSE
// The Win32 implementation adapts insertion-index ordering and retention of
// non-visible identifiers to application and synthetic system entries. It does
// not embed RetroBar's WPF/GongSolutions drag-and-drop implementation.
static std::vector<int> VisibleTrayEntries() {
    auto entries=VisiblePrivateIcons();if(!s || !s->privateActive)return entries;
    for(int hit:{HIT_BATTERY,HIT_NETWORK,HIT_VOLUME})if(IsSystemVisible(hit))entries.push_back(SystemEntry(hit));
    auto rank=[&](int entry){auto key=TrayEntryKey(entry);
        auto it=std::find(s->trayBarOrder.begin(),s->trayBarOrder.end(),key);
        return it==s->trayBarOrder.end()?s->trayBarOrder.size():(size_t)(it-s->trayBarOrder.begin());
        };
    std::stable_sort(entries.begin(),entries.end(),[&](int a,int b){return rank(a)<rank(b);});
    return entries;
}
static int OverflowButtonWidth() { return Px(25); }
static int PrivateTrayWidth() {
    if(!s || !s->privateActive) return 0;
    int systems=(IsSystemVisible(HIT_NETWORK)?1:0)+(IsSystemVisible(HIT_VOLUME)?1:0)+(IsSystemVisible(HIT_BATTERY)?1:0);
    // Four logical pixels separate the last notification cell from the input
    // indicator or clock. This is layout space, not a hit cell.
    return ((int)VisiblePrivateIcons().size()+systems)*Px(24)+OverflowButtonWidth()+Px(4);
}
static RECT TrayBarEntryRect(int entry) {
    RECT r{};if(!s || !s->bar || !GetClientRect(s->bar,&r))return RECT{};
    auto entries=VisibleTrayEntries();auto it=std::find(entries.begin(),entries.end(),entry);
    if(it==entries.end())return RECT{};
    LONG slot=(LONG)(it-entries.begin()),cell=Px(24);
    if(s->verticalActive) {auto v=GetVerticalLayout(r.right,r.bottom);return RECT{0,v.icons.top+slot*cell,r.right,v.icons.top+(slot+1)*cell};}
    Layout l=GetLayout(r.right,r.bottom);LONG x=l.tray.left+OverflowButtonWidth()+slot*cell;
    return RECT{x,0,x+cell,r.bottom};
}
static RECT PrivateIconRect(int index) {
    RECT cell=TrayBarEntryRect(index);if(!IsRectEmpty(&cell))return cell;
    // A hidden application's Shell_NotifyIconGetRect fallback remains the arrow.
    RECT r{};if(!s || !s->bar || !GetClientRect(s->bar,&r))return RECT{};
    if(s->verticalActive)return GetVerticalLayout(r.right,r.bottom).arrow;
    Layout l=GetLayout(r.right,r.bottom);
    return RECT{l.tray.left,0,l.tray.left+OverflowButtonWidth(),r.bottom};
}
static RECT SystemCell(int which) {return TrayBarEntryRect(SystemEntry(which));}
// Shared overflow artwork for both tray and overflow cells.
static void EnsureTrayHoverImage() {
    if(!s || s->overflowImageTried) return;
    s->overflowImageTried=true; auto bytes=DecodeBase64(kW7OverflowTile);
    if(!bytes.empty()) {
        s->overflowHoverStream=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
        if(s->overflowHoverStream) {
            s->overflowHoverImage=Image::FromStream(s->overflowHoverStream,FALSE);
            if(s->overflowHoverImage && s->overflowHoverImage->GetLastStatus()!=Ok) { delete s->overflowHoverImage; s->overflowHoverImage=nullptr; }
        }
    }
}
static void DrawTrayHoverTile(Graphics& g,const RECT& cell,bool onBar=false,float opacity=1.0f) {
    opacity=std::max(0.f,std::min(1.f,opacity)); if(opacity<=0) return;
    int rw=std::max(1,(int)(cell.right-cell.left)-2*Px(2));
    int rh=std::max(1,(int)(cell.bottom-cell.top)-2*Px(2));
    int tw=onBar?std::max(1,MulDiv((int)(cell.right-cell.left),106,100)):std::max(1,MulDiv(rw,976,1000));
    int th=onBar?std::max(1,(int)(cell.bottom-cell.top)):std::max(1,MulDiv(rh,976,1000));
    int x=onBar?cell.left+((cell.right-cell.left)-tw)/2:cell.left+Px(2)+(rw-tw)/2;
    int y=onBar?cell.top:cell.top+Px(2)+(rh-th)/2;
    if(s->overflowHoverImage) {
        ImageAttributes attr; ColorMatrix matrix={{{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,opacity,0},{0,0,0,0,1}}};
        attr.SetColorMatrix(&matrix);
        // Preserve narrow side borders when the 31px tray tile is used on a
        // wide clock/address band; stretching the whole PNG makes huge glows.
        if(onBar && tw>Px(36)) {
            int cap=std::min(Px(3),tw/2);
            g.DrawImage(s->overflowHoverImage,RectF((float)x,(float)y,(float)cap,(float)th),0.f,0.f,3.f,27.f,UnitPixel,&attr);
            g.DrawImage(s->overflowHoverImage,RectF((float)(x+cap),(float)y,(float)(tw-2*cap),(float)th),3.f,0.f,25.f,27.f,UnitPixel,&attr);
            g.DrawImage(s->overflowHoverImage,RectF((float)(x+tw-cap),(float)y,(float)cap,(float)th),28.f,0.f,3.f,27.f,UnitPixel,&attr);
        } else {
            RectF dest((float)x,(float)y,(float)tw,(float)th); g.DrawImage(s->overflowHoverImage,dest,0.f,0.f,31.f,27.f,UnitPixel,&attr);
        }
    }
    else { SolidBrush fill(Color((int)std::lround(100*opacity),220,233,245));
         Pen edge(Color((int)std::lround(170*opacity),110,165,210));
         g.FillRectangle(&fill,x,y,tw,th);
         g.DrawRectangle(&edge,x,y,tw-1,th-1);
         }
}
// Geometry shared by tray, Desktop and Links arrows. Keep the hit rectangle
// and the drawn glyph centred on the same anchor at every DPI.
static RECT BandArrowRect(const RECT& band) {
    int width=std::min(std::max(0,(int)(band.right-band.left)),Px(18));
    return RECT{band.right-width,band.top,band.right,band.bottom};
}
static RECT ArrowHoverRect(RECT cell) {
    int tw=std::min(std::max(1,(int)(cell.right-cell.left)),Px(16));
    int th=std::min(std::max(1,(int)(cell.bottom-cell.top)),Px(18));
    int left=cell.left+((cell.right-cell.left)-tw)/2,top=cell.top+((cell.bottom-cell.top)-th)/2;
    return RECT{left,top,left+tw,top+th};
}
static void DrawArrowHover(Graphics& g,RECT cell) {
    RECT r=ArrowHoverRect(cell); SmoothingMode previous=g.GetSmoothingMode();g.SetSmoothingMode(SmoothingModeNone);
    SolidBrush fill(Color(65,224,243,255)); Pen edge(Color(125,212,237,255),1.0f);
    g.FillRectangle(&fill,(int)r.left,(int)r.top,(int)(r.right-r.left),(int)(r.bottom-r.top));
    g.DrawRectangle(&edge,(int)r.left,(int)r.top,std::max(1,(int)(r.right-r.left)-1),std::max(1,(int)(r.bottom-r.top)-1));
    g.SetSmoothingMode(previous);
}
static RECT TrayArrowHoverRect(RECT cell) {
    int tw=std::min(std::max(1,(int)(cell.right-cell.left)),Px(17));
    int th=std::min(std::max(1,(int)(cell.bottom-cell.top)),Px(19));
    int x=cell.left+((cell.right-cell.left)-tw)/2,y=cell.top+((cell.bottom-cell.top)-th)/2;
    return RECT{x,y,x+tw,y+th};
}
static void DrawTrayArrowHover(Graphics& g,RECT cell) {
    RECT r=TrayArrowHoverRect(cell);int w=r.right-r.left,h=r.bottom-r.top;
    SmoothingMode old=g.GetSmoothingMode();g.SetSmoothingMode(SmoothingModeNone);
    SolidBrush fill(Color(65,224,243,255)),edge(Color(125,212,237,255));
    g.FillRectangle(&fill,(int)r.left,(int)r.top,w,h);
    g.FillRectangle(&edge,(int)r.left,(int)r.top,w,1);
    if(h>1)g.FillRectangle(&edge,(int)r.left,(int)r.bottom-1,w,1);
    if(h>2){g.FillRectangle(&edge,(int)r.left,(int)r.top+1,1,h-2);if(w>1)g.FillRectangle(&edge,(int)r.right-1,(int)r.top+1,1,h-2);}
    g.SetSmoothingMode(old);
}
static bool DrawWin7OverflowButton(Graphics& g,const RECT& cell,bool vertical) {
    if(!s || taskbarSkin.load()!=1)return false;
    bool pressed=s->hot==HIT_TRAY_MORE && (GetKeyState(VK_LBUTTON)&0x8000);
    bool hot=s->hot==HIT_TRAY_MORE || (s->overflowPanel && IsWindowVisible(s->overflowPanel));
    int index=pressed?27:(hot?26:25);
    Image* image=s->betaTexture[index];if(!image)return false;
    REAL width=(REAL)Px(index==25?9:21),height=(REAL)Px(index==25?7:20);
    REAL cx=(cell.left+cell.right)/2.f,cy=(cell.top+cell.bottom)/2.f;
    REAL rotation=vertical?(s->verticalSide==1?90.f:-90.f):(s->nativeEdge==ABE_TOP?180.f:0.f);
    auto state=g.Save();g.SetClip(Rect((int)cell.left,(int)cell.top,(int)(cell.right-cell.left),(int)(cell.bottom-cell.top)),CombineModeIntersect);
    g.TranslateTransform(cx,cy);g.RotateTransform(rotation);
    // Draw the three state textures with GDI+.
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    g.DrawImage(image,RectF(-width/2.f,-height/2.f,width,height),0.f,0.f,(REAL)image->GetWidth(),(REAL)image->GetHeight(),UnitPixel);
    g.Restore(state);return true;
}
static float TrayArrowY(float y,float center,bool top) {return top?2*center-y:y;}
static void PaintPrivateArea(Graphics& g,HDC dc,const Layout& L,int h) {
    if(!s->privateActive) return;
    auto visible=VisiblePrivateIcons(); int n=(int)visible.size();
    EnsureTrayHoverImage();
    auto drawTrayHover=[&](RECT r) { DrawTrayHoverTile(g,r,true); };
    for(int i=0;i<n;i++) if(s->hot==HIT_TRAY_ICON_BASE-visible[i]) drawTrayHover(PrivateIconRect(visible[i]));
    for(int hit:{HIT_NETWORK,HIT_VOLUME,HIT_BATTERY}) if(s->hot==hit && IsSystemVisible(hit)) drawTrayHover(SystemCell(hit));
    for(int i=0;i<n;i++) {
        const auto& icon=s->trayIcons[visible[i]]; RECT r=PrivateIconRect(visible[i]);
        int size=std::min(Px(16),h-Px(4));
        if(icon.icon && size>0) DrawIconEx(dc,(r.left+r.right-size)/2,(h-size)/2,icon.icon,size,size,0,nullptr,DI_NORMAL);
    }
    if(s->trayDrag.active && s->trayDrag.dropArea==1 && !s->verticalActive && !IsRectEmpty(&s->trayDrag.dropCell)) {
        RECT c=s->trayDrag.dropCell;int x=s->trayDrag.dropAfter?c.right-1:c.left;SolidBrush cue(Color(235,240,250,255));
        g.FillRectangle(&cue,x,Px(5),std::max(1,Px(2)),std::max(1,h-Px(10)));
    }
    RECT overflowCell{L.tray.left,0,L.tray.left+OverflowButtonWidth(),h};
    bool win7Arrow=DrawWin7OverflowButton(g,overflowCell,false);
    if(!win7Arrow && (s->hot==HIT_TRAY_MORE || (s->overflowPanel && IsWindowVisible(s->overflowPanel)))) {
        DrawTrayArrowHover(g,overflowCell);
    }
    // Use a small fixed triangle rather than a font glyph, whose advance and
    // baseline vary across fonts; it occupies the centre of the arrow cell.
    float scale=Dpi()/96.0f, ax=L.tray.left+OverflowButtonWidth()/2.0f, ay=h/2.0f;
    PointF outer[]={{ax-4.6f*scale,ay+2.5f*scale},{ax,ay-3.3f*scale},{ax+4.6f*scale,ay+2.5f*scale}};
    PointF inner[]={{ax-3.3f*scale,ay+1.3f*scale},{ax,ay-1.9f*scale},{ax+3.3f*scale,ay+1.3f*scale}};
    for(auto& p:outer)p.Y=TrayArrowY(p.Y,ay,s->nativeEdge==ABE_TOP);
    for(auto& p:inner)p.Y=TrayArrowY(p.Y,ay,s->nativeEdge==ABE_TOP);
    SolidBrush shade(Color(145,0,0,0)), arrowWhite(Color(255,245,253,255));
    if(!win7Arrow) {g.FillPolygon(&shade,outer,3);g.FillPolygon(&arrowWhite,inner,3);}
    RECT net=SystemCell(HIT_NETWORK),vol=SystemCell(HIT_VOLUME);
    if(IsSystemVisible(HIT_NETWORK)) DrawNetworkGlyph(g,(net.left+net.right)/2.0f,h/2.0f,scale,s->netKind);
    if(IsSystemVisible(HIT_VOLUME)) DrawVolumeGlyph(g,(vol.left+vol.right)/2.0f,h/2.0f,scale,s->volume,s->muted);
    if(IsSystemVisible(HIT_BATTERY)) { RECT b=SystemCell(HIT_BATTERY); DrawBatteryGlyph(g,(b.left+b.right)/2.0f,h/2.0f,scale,s->batteryPct,s->batteryCharging); }
}
static int HitPrivateArea(POINT p,const Layout& L) {
    if(!s->privateActive || !PtInRect(&L.tray,p)) return HIT_NONE;
    if(p.x<L.tray.left+OverflowButtonWidth()) return HIT_TRAY_MORE;
    auto visible=VisibleTrayEntries();
    int slot=(p.x-L.tray.left-OverflowButtonWidth())/std::max(1,Px(24));
    if(slot>=0 && (size_t)slot<visible.size()) {int entry=visible[slot];int hit=SystemHitForEntry(entry);return hit?hit:HIT_TRAY_ICON_BASE-entry;}
    return HIT_NONE;
}
// Overflow positioning helper (GPL-3.0-or-later).
void ComputeOverflowNear(const RECT& btn, int w, int h, int& x, int& y) {
    const int gapY = (6 + (h * 3) / 100) + (h * 45) / 1000;
    const int gapX = 6;

    RECT wa{ 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    try {
        HMONITOR mon = MonitorFromRect(&btn, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (mon != nullptr && GetMonitorInfoW(mon, &mi)) {
            wa = mi.rcWork;
        }
    } catch (...) {
    }

    const int cx = (btn.left + btn.right) / 2;
    const int cy = (btn.top + btn.bottom) / 2;
    if(s && s->verticalActive) {
        x=s->verticalSide==1?btn.right+gapX:btn.left-w-gapX;y=cy-h/2;
    } else if (btn.top <= wa.top + 4) {
        x = cx - w / 2;
        y = btn.bottom + gapY;
    } else if (btn.bottom >= wa.bottom - 4) {
        x = cx - w / 2;
        y = btn.top - h - gapY;
    } else if (btn.left <= wa.left + 4) {
        x = btn.right + gapX;
        y = cy - h / 2;
    } else if (btn.right >= wa.right - 4) {
        x = btn.left - w - gapX;
        y = cy - h / 2;
    } else {
        x = cx - w / 2;
        y = btn.top - h - gapY;
    }

    if (x < wa.left + 2) x = wa.left + 2;
    if (y < wa.top + 2) y = wa.top + 2;
    if (x + w > wa.right - 2) x = wa.right - w - 2;
    if (y + h > wa.bottom - 2) y = wa.bottom - h - 2;
}
// Overflow window behavior, GPL-3.0-or-later.
// The positioning helper ComputeOverflowNear and the hover PNG below are
// included verbatim; the class itself is adapted to this mod's private tray
// without linking a WPF frontend, TrayService singleton or SEH handler.
// Network List Manager reports a connection profile name (not necessarily Wi-Fi
// SSID) and connectivity as judged by Windows. This independently follows the
// documented NLM API, also used by win7-network-flyout-recreation; no Wi-Fi
// scan or location-sensitive WLAN current_connection request is made.
// Wording follows Win7 Network Flyout Recreation's Italian/English status
// vocabulary, but availability of Wi-Fi networks is not inferred from NLM.
static std::wstring NetworkStatusText(bool known,NLM_CONNECTIVITY state) {
    if(!known) return Tr(L"Stato di rete non disponibile",L"Network status unavailable");
    if(state & (NLM_CONNECTIVITY_IPV4_INTERNET|NLM_CONNECTIVITY_IPV6_INTERNET))
        return Tr(L"Accesso a Internet",L"Internet access");
    return Tr(L"Nessun accesso a Internet",L"No Internet access");
}
static std::wstring NetworkTooltipText() {
    Wh_Log(L"[T81-DIAG] network tooltip query begin");
    INetworkListManager* manager=nullptr;
    HRESULT hr=CoCreateInstance(CLSID_NetworkListManager,nullptr,CLSCTX_ALL,
                                IID_INetworkListManager,(void**)&manager);
    std::unique_ptr<INetworkListManager,ReleaseCom<INetworkListManager>> managerGuard(manager);
    if(FAILED(hr) || !manager) return NetworkStatusText(false,NLM_CONNECTIVITY_DISCONNECTED);
    IEnumNetworks* en=nullptr;
    hr=manager->GetNetworks(NLM_ENUM_NETWORK_CONNECTED,&en);
    std::unique_ptr<IEnumNetworks,ReleaseCom<IEnumNetworks>> enumGuard(en);
    std::wstring text; unsigned count=0; HRESULT nextStatus=S_OK;
    if(SUCCEEDED(hr) && en) {
        INetwork* net=nullptr; ULONG got=0;
        while(count<4 && (nextStatus=en->Next(1,&net,&got))==S_OK && got && net) {
            std::unique_ptr<INetwork,ReleaseCom<INetwork>> networkGuard(net);
            BSTR name=nullptr; NLM_CONNECTIVITY state=NLM_CONNECTIVITY_DISCONNECTED;
            HRESULT hn=net->GetName(&name); std::unique_ptr<wchar_t,FreeBstr> nameGuard(name);
            HRESULT hc=net->GetConnectivity(&state);
            std::wstring label=SUCCEEDED(hn) && name && *name ? std::wstring(name,SysStringLen(name)) : Tr(L"Rete",L"Network");
            if(label.size()>80) label.resize(80);
            for(auto& c:label) if(c<L' ') c=L' ';
            if(!text.empty()) text+=L"\n";
            text+=label+L"\n"+NetworkStatusText(SUCCEEDED(hc),state);
            net=nullptr; ++count;
        }
    }
    if(FAILED(hr) || !en || FAILED(nextStatus)) {
        if(!text.empty()) text+=L"\n";
        text+=NetworkStatusText(false,NLM_CONNECTIVITY_DISCONNECTED);
    } else if(text.empty()) text=Tr(L"Non connesso",L"Not connected");
    return text;
}
static std::wstring TrayEntryKey(int entry) {
    int system=SystemHitForEntry(entry);if(system)return SystemKey(system);
    return s && entry>=0 && (size_t)entry<s->trayIcons.size()?s->trayIcons[entry].preferenceKey:L"";
}
static std::vector<int> OverflowPrivateItems() {
    std::vector<int> out; if(!s) return out;
    auto shown=VisiblePrivateIcons();
    for(size_t i=0;i<s->trayIcons.size();++i) {
        auto it=std::find(shown.begin(),shown.end(),(int)i);
        if(it!=shown.end()) continue;
        if(!s->trayIcons[i].hidden && s->trayIcons[i].icon) out.push_back((int)i);
    }
    for(int hit:{HIT_BATTERY,HIT_NETWORK,HIT_VOLUME}) {
        if(hit==HIT_BATTERY && !s->hasBattery) continue;
        if(!IsSystemVisible(hit)) out.push_back(SystemEntry(hit));
    }
    // Stable explicit order; previously unseen icons retain discovery order.
    auto rank=[&](int entry){auto key=TrayEntryKey(entry);
        auto it=std::find(s->trayOverflowOrder.begin(),s->trayOverflowOrder.end(),key);
        return it==s->trayOverflowOrder.end()?s->trayOverflowOrder.size():(size_t)(it-s->trayOverflowOrder.begin());
        };
    std::stable_sort(out.begin(),out.end(),[&](int a,int b){return rank(a)<rank(b);});
    return out;
}
static COLORREF PopupChromeColor() {
    DWORD preference=0,size=sizeof(preference);
    bool windowsBorders=RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\DWM",L"ColorPrevalence",RRF_RT_REG_DWORD,nullptr,&preference,&size)==ERROR_SUCCESS && preference!=0;
    if(useAccent.load() || windowsBorders) { DWORD c=0;
         BOOL opaque=FALSE;
         if(SUCCEEDED(DwmGetColorizationColor(&c,&opaque))) return RGB((BYTE)(c>>16),(BYTE)(c>>8),(BYTE)c);
         }
    return RGB(15,74,121);
}
static int OverflowRowsShown() {
    auto count=OverflowPrivateItems().size();
    int rows=std::max(1,(int)((count+2)/3));
    return std::min(rows,10);
}
static int OverflowContentPad() {return Px(taskbarSkin.load()==1?6:12);}
static RECT OverflowCell(int slot) {
    int cell=Px(30),pad=OverflowContentPad();
    int x=pad+(slot%3)*cell;
    int y=pad+(slot/3-s->overflowScroll)*cell;
    return RECT{x,y,x+cell,y+cell};
}
static RECT OverflowFooter() {
    int pad=OverflowContentPad(),top=pad+OverflowRowsShown()*Px(30)+Px(1);
    if(taskbarSkin.load()==1)return RECT{0,top,pad*2+3*Px(30),top+Px(26)};
    return RECT{pad,top,pad+3*Px(30),top+Px(26)};
}
static bool OverflowIconScreenRect(int index, RECT* result) {
    if(!s || !s->overflowPanel || !IsWindowVisible(s->overflowPanel) || !result) return false;
    auto entries=OverflowPrivateItems();
    auto it=std::find(entries.begin(),entries.end(),index);
    if(it==entries.end()) return false;
    int slot=(int)(it-entries.begin()), row=slot/3-s->overflowScroll;
    if(row<0 || row>=OverflowRowsShown()) return false;
    *result=OverflowCell(slot);
    MapWindowPoints(s->overflowPanel,nullptr,reinterpret_cast<POINT*>(result),2);
    return true;
}
static int OverflowHit(POINT point) {
    auto entries=OverflowPrivateItems();
    for(size_t i=0;i<entries.size();++i) {
        int row=(int)i/3-s->overflowScroll;
        if(row<0 || row>=OverflowRowsShown()) continue;
        RECT cell=OverflowCell((int)i);
        if(PtInRect(&cell,point)) return entries[i];
    }
    return -1;
}
static void HidePrivateOverflow() {
    if(!s) return;
    if(s->overflowTooltip) { TOOLINFOW tool{}; tool.cbSize=sizeof(tool); tool.hwnd=s->overflowPanel; tool.uId=1;
        SendMessageW(s->overflowTooltip,TTM_TRACKACTIVATE,FALSE,(LPARAM)&tool); }
    if(s->overflowPanel) ShowWindow(s->overflowPanel,SW_HIDE);
    s->overflowHot=-1; s->overflowFooterHot=false;
}
static std::wstring SystemTooltipText(int hit) {
    if(hit==HIT_NETWORK) return NetworkTooltipText();
    if(hit==HIT_VOLUME) {
        if(!s || !s->endpoint) return Tr(L"Volume: non disponibile",L"Volume: unavailable");
        int percent=(int)std::lround(std::max(0.f,std::min(1.f,s->volume))*100.f);
        return Tr(L"Volume: ",L"Volume: ")+std::to_wstring(percent)+L"%"+(s->muted?Tr(L" (disattivato)",L" (muted)"):L"");
    }
    if(hit==HIT_BATTERY) {
        if(!s || s->batteryPct<0) return Tr(L"Batteria: stato sconosciuto",L"Battery: unknown status");
        return Tr(L"Batteria: ",L"Battery: ")+std::to_wstring(s->batteryPct)+L"%"+(s->charging?Tr(L" (alimentazione collegata)",L" (plugged in)"):L"");
    }
    return {};
}
static void UpdateOverflowTooltip(int index,POINT client) {
    if(!s || !s->overflowTooltip) return;
    TOOLINFOW tool{}; tool.cbSize=sizeof(tool); tool.hwnd=s->overflowPanel; tool.uId=1;
    int system=SystemHitForEntry(index);
    if(system) s->overflowTooltipText=SystemTooltipText(system);
    else if(index>=0 && (size_t)index<s->trayIcons.size()) s->overflowTooltipText=s->trayIcons[index].tip;
    else s->overflowTooltipText.clear();
    if(s->overflowTooltipText.empty()) {
        SendMessageW(s->overflowTooltip,TTM_TRACKACTIVATE,FALSE,(LPARAM)&tool); return;
    }
    tool.lpszText=const_cast<wchar_t*>(s->overflowTooltipText.c_str());
    SendMessageW(s->overflowTooltip,TTM_UPDATETIPTEXTW,0,(LPARAM)&tool);
    ClientToScreen(s->overflowPanel,&client);
    SendMessageW(s->overflowTooltip,TTM_TRACKPOSITION,0,MAKELPARAM(client.x+Px(12),client.y+Px(18)));
    SendMessageW(s->overflowTooltip,TTM_TRACKACTIVATE,TRUE,(LPARAM)&tool);
}
static void DismissOverflowOnOutsideClick() {
    if(!s || !s->overflowPanel || !IsWindowVisible(s->overflowPanel)) return;
    int now=((GetAsyncKeyState(VK_LBUTTON)&0x8000)?1:0)|((GetAsyncKeyState(VK_RBUTTON)&0x8000)?2:0);
    if(s->trayDrag.capture) {s->overflowMouseMask=now;return;}
    int newlyPressed=now & ~s->overflowMouseMask; s->overflowMouseMask=now;
    if(newlyPressed) {
        POINT p{}; RECT panel{},bar{}; GetCursorPos(&p); GetWindowRect(s->overflowPanel,&panel); GetWindowRect(s->bar,&bar);
        if(!PtInRect(&panel,p) && !PtInRect(&bar,p)) HidePrivateOverflow();
    }
}
// Apply a system non-client frame only for the Windows 7 preset. The frame
// is drawn by the current OS; no Windows 7 glass/blur is assumed.
// Restore the real system non-client frame for Win7. Its width and material
// are decided by the current Windows/DWM theme; this is not Win7 Aero glass.
static DWORD TaskbarPopupStyle() {return WS_POPUP | (taskbarSkin.load()==1?WS_THICKFRAME:WS_BORDER);}
static void ConfigureSystemPopupFrame(HWND hwnd) {
    if(!hwnd || !IsWindow(hwnd))return;
    LONG_PTR old=GetWindowLongPtrW(hwnd,GWL_STYLE);
    LONG_PTR wanted=(old & ~((LONG_PTR)(WS_THICKFRAME|WS_BORDER))) | (TaskbarPopupStyle() & (WS_THICKFRAME|WS_BORDER));
    if(old!=wanted) {SetWindowLongPtrW(hwnd,GWL_STYLE,wanted);SetWindowPos(hwnd,nullptr,0,0,0,0,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);}
    DWMNCRENDERINGPOLICY policy=taskbarSkin.load()==1?DWMNCRP_ENABLED:DWMNCRP_USEWINDOWSTYLE;
    HRESULT hr=DwmSetWindowAttribute(hwnd,DWMWA_NCRENDERING_POLICY,&policy,sizeof(policy));
    if(FAILED(hr))Wh_Log(L"[T81-DWM-FRAME] policy failed hr=0x%08lx",(unsigned long)hr);
}
static void ResizePrivateOverflow() {
    if(!s || !s->overflowPanel) return;
    auto entries=OverflowPrivateItems();
    int totalRows=std::max(1,(int)((entries.size()+2)/3));
    s->overflowScroll=std::max(0,std::min(s->overflowScroll,totalRows-OverflowRowsShown()));
    int pad=OverflowContentPad();
    int w=2*pad+3*Px(30), h=2*pad+OverflowRowsShown()*Px(30)+Px(26)+Px(1);
    RECT rect{0,0,w,h};
    using AdjustFn=BOOL (WINAPI*)(LPRECT,DWORD,BOOL,DWORD,UINT);
    auto adjust=(AdjustFn)GetProcAddress(GetModuleHandleW(L"user32.dll"),"AdjustWindowRectExForDpi");
    DWORD style=(DWORD)GetWindowLongPtrW(s->overflowPanel,GWL_STYLE);
    DWORD ex=(DWORD)GetWindowLongPtrW(s->overflowPanel,GWL_EXSTYLE);
    if(adjust) adjust(&rect,style,FALSE,ex,(UINT)Dpi()); else AdjustWindowRectEx(&rect,style,FALSE,ex);
    SetWindowPos(s->overflowPanel,nullptr,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
    if(IsWindowVisible(s->overflowPanel)) {
        // The arrow moves when pinned icons change, the bar moves or DPI changes.
        RECT barClient{}; GetClientRect(s->bar,&barClient);
        Layout layout=GetLayout(barClient.right,barClient.bottom);
        RECT anchor=s->verticalActive?GetVerticalLayout(barClient.right,barClient.bottom).arrow:RECT{layout.tray.left,0,layout.tray.left+OverflowButtonWidth(),barClient.bottom};
        MapWindowPoints(s->bar,nullptr,reinterpret_cast<POINT*>(&anchor),2);
        s->overflowAnchor=anchor;
        RECT now{}; GetWindowRect(s->overflowPanel,&now); int x=0,y=0;
        ComputeOverflowNear(s->overflowAnchor,now.right-now.left,now.bottom-now.top,x,y);
        SetWindowPos(s->overflowPanel,HWND_TOPMOST,x,y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
        InvalidateRect(s->overflowPanel,nullptr,FALSE);
    }
}
static void PaintPrivateOverflow(HWND h) {
    ScopedPaintDC paint(h); HDC dc=paint.dc; if(!dc) return; RECT r{}; GetClientRect(h,&r);
    std::unique_ptr<std::remove_pointer<HDC>::type,DeleteMemDC> memGuard(CreateCompatibleDC(dc)); HDC mem=memGuard.get();
    std::unique_ptr<std::remove_pointer<HBITMAP>::type,DeleteBitmapHandle> bitmapGuard(CreateCompatibleBitmap(dc,std::max(1L,r.right),std::max(1L,r.bottom))); HBITMAP bitmap=bitmapGuard.get();
    ScopedSelection selected(mem,bitmap);
    if(mem && bitmap && selected.old && selected.old!=HGDI_ERROR) {
        auto entries=OverflowPrivateItems();
        int rowsShown=std::min(std::max(1,(int)((entries.size()+2)/3)),10);
        {
            Graphics g(mem);
            int inset=taskbarSkin.load()==1?0:Px(6);
            if(inset) {COLORREF c=PopupChromeColor();
                SolidBrush chrome(Color(255,GetRValue(c),GetGValue(c),GetBValue(c)));
                g.FillRectangle(&chrome,0,0,r.right,r.bottom);
                }
            Rect paper(inset,inset,std::max(1,(int)r.right-2*inset),std::max(1,(int)r.bottom-2*inset));
            LinearGradientBrush bg(Point(0,0),Point(0,std::max(1L,r.bottom)),Color(255,255,255,255),Color(255,244,244,244));
            g.FillRectangle(&bg,paper);
            if(inset) {Pen border(Color(255,161,161,161),1);g.DrawRectangle(&border,paper.X,paper.Y,paper.Width-1,paper.Height-1);}
            for(size_t i=0;i<entries.size();++i) {
                int row=(int)i/3-s->overflowScroll;
                if(row<0 || row>=rowsShown) continue;
                RECT cell=OverflowCell((int)i);
                if(entries[i]==s->overflowHot) {
                    DrawTrayHoverTile(g,cell);
                }
            }
            if(s->trayDrag.active && s->trayDrag.dropArea==2 && !IsRectEmpty(&s->trayDrag.dropCell)) {
                RECT cell=s->trayDrag.dropCell;int x=s->trayDrag.dropAfter?cell.right-1:cell.left;
                SolidBrush cue(Color(230,20,108,172));g.FillRectangle(&cue,x,(int)cell.top+Px(3),std::max(1,Px(2)),std::max(1,(int)(cell.bottom-cell.top)-Px(6)));
            }
            RECT footer=OverflowFooter();
            int top=std::max(0,(int)footer.top-Px(3));
            LinearGradientBrush foot(Point(0,top),Point(0,std::max(top+1,(int)r.bottom)),
                                     Color(255,233,240,248),Color(255,240,245,252));
            int footPad=taskbarSkin.load()==1?0:Px(7);
            g.FillRectangle(&foot,footPad,top,std::max(0,(int)r.right-2*footPad),std::max(0,(int)r.bottom-footPad-top));
            Pen sep(Color(255,204,217,234)); g.DrawLine(&sep,footPad,top,(int)r.right-footPad,top);
        }
        int icon=Px(16);
        for(size_t i=0;i<entries.size();++i) {
            int row=(int)i/3-s->overflowScroll;
            if(row<0 || row>=rowsShown) continue;
            RECT cell=OverflowCell((int)i);
            int system=SystemHitForEntry(entries[i]);
            if(system) {
                Graphics g(mem); float x=(cell.left+cell.right)/2.0f, y=(cell.top+cell.bottom)/2.0f, u=Dpi()/96.0f;
                if(system==HIT_NETWORK) DrawNetworkGlyph(g,x,y,u,s->netKind,true);
                if(system==HIT_VOLUME) DrawVolumeGlyph(g,x,y,u,s->volume,s->muted,true);
                if(system==HIT_BATTERY) DrawBatteryGlyph(g,x,y,u,s->batteryPct,s->batteryCharging,true);
            } else if(entries[i]>=0 && (size_t)entries[i]<s->trayIcons.size()) {
                HICON img=s->trayIcons[entries[i]].icon;
                if(img) DrawIconEx(mem,(cell.left+cell.right-icon)/2,(cell.top+cell.bottom-icon)/2,img,icon,icon,0,nullptr,DI_NORMAL);
            }
        }
        HFONT font=CreateFontW(-Px(12),0,0,0,FW_NORMAL,FALSE,s->overflowFooterHot,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        HGDIOBJ previous=font?SelectObject(mem,font):nullptr;
        SetBkMode(mem,TRANSPARENT); SetTextColor(mem,s->overflowFooterHot?RGB(0,78,158):RGB(0,102,204));
        RECT footer=OverflowFooter(); DrawTextW(mem,Tr(L"Personalizza...",L"Customize..."),-1,&footer,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
        if(previous) SelectObject(mem,previous); if(font) DeleteObject(font);
        BitBlt(dc,0,0,r.right,r.bottom,mem,0,0,SRCCOPY);
    }
}
static void BeginTrayDrag(HWND capture,int index,bool fromOverflow,POINT client);
static void MoveTrayDrag(HWND capture,POINT client);
static void EndTrayDrag(HWND capture,POINT client);
static void TickTrayDrag();
static void ShowTrayCustomize(HWND owner);
static void ShowSystemTrayContextMenu(int hit,POINT p);
static void OpenNotificationIconsPage(HWND owner) {
    // Notification Icons namespace launch (GPL-3.0-or-later).
    const wchar_t* ns=L"shell:::{05D7B0F4-2121-4EFF-BF6B-ED3F69B894D9}";
    SHELLEXECUTEINFOW se{}; se.cbSize=sizeof(se); se.hwnd=owner; se.lpVerb=L"open";
    se.lpFile=ns; se.nShow=SW_SHOWNORMAL; se.fMask=SEE_MASK_NOASYNC|SEE_MASK_FLAG_NO_UI;
    if(ShellExecuteExW(&se)) { Wh_Log(L"[T81-DIAG] notification namespace launch accepted"); return; }
    Wh_Log(L"[T81-DIAG] notification namespace failed error=%lu; trying Explorer",GetLastError());
    se.lpFile=L"explorer.exe"; se.lpParameters=ns;
    if(!ShellExecuteExW(&se)) Wh_Log(L"[T81-DIAG] notification namespace Explorer fallback failed error=%lu",GetLastError());
}
static void LaunchSystemTrayHit(int hit,const POINT* click=nullptr);
static LRESULT CALLBACK PrivateOverflowProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    if(!s) return DefWindowProcW(h,msg,wp,lp);
    try {
        if(msg==WM_LBUTTONDOWN)s->suppressTrayRelease=false;
        if(msg==WM_LBUTTONUP && s->suppressTrayRelease && !s->trayDrag.capture) {
            bool consume=GetTickCount64()<s->suppressTrayReleaseUntil;s->suppressTrayRelease=false;if(consume)return 0;
        }
        switch(msg) {
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintPrivateOverflow(h); return 0;
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_MOUSEMOVE: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(s->trayDrag.capture==h) { MoveTrayDrag(h,p); return 0; }
            int hot=OverflowHit(p); RECT footer=OverflowFooter();
            bool foot=PtInRect(&footer,p)!=FALSE;
            if(hot!=s->overflowHot || foot!=s->overflowFooterHot) {
                s->overflowHot=hot; s->overflowFooterHot=foot;
                UpdateOverflowTooltip(hot,p); InvalidateRect(h,nullptr,FALSE);
            }
            TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,h,0}; TrackMouseEvent(&t); return 0;
        }
        case WM_MOUSELEAVE: s->overflowHot=-1; s->overflowFooterHot=false; UpdateOverflowTooltip(-1,POINT{}); InvalidateRect(h,nullptr,FALSE); return 0;
        case WM_MOUSEWHEEL: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};ScreenToClient(h,&p);
            if(volumeWheel.load() && SystemHitForEntry(OverflowHit(p))==HIT_VOLUME) {AdjustVolumeWheel(GET_WHEEL_DELTA_WPARAM(wp));return 0;}
            auto entries=OverflowPrivateItems(); int rows=std::max(1,(int)((entries.size()+2)/3));
            s->overflowScroll=std::max(0,std::min(rows-OverflowRowsShown(),s->overflowScroll-(GET_WHEEL_DELTA_WPARAM(wp)>0?1:-1)));
            InvalidateRect(h,nullptr,FALSE); return 0;
        }
        case WM_LBUTTONDOWN: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)}; int index=OverflowHit(p);
            if(index!=-1) { BeginTrayDrag(h,index,true,p); return 0; }
            return 0;
        }
        case WM_CAPTURECHANGED: case WM_CANCELMODE:
            if(s->trayDrag.capture==h) CancelTrayDrag(); return 0;
        case WM_LBUTTONUP: case WM_RBUTTONUP: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(msg==WM_LBUTTONUP && s->trayDrag.capture==h) { if(s->trayDrag.endQueued)s->trayDrag.nativeUpSeen=true;else EndTrayDrag(h,p); return 0; }
            RECT footer=OverflowFooter();
            if(msg==WM_LBUTTONUP && PtInRect(&footer,p)) {
                HidePrivateOverflow();
                if(GetKeyState(VK_SHIFT)&0x8000) ShowTrayCustomize(s->bar); else OpenNotificationIconsPage(s->bar);
                return 0;
            }
            if(msg==WM_LBUTTONUP) return 0; // no orphan callback after capture loss
            int index=OverflowHit(p); if(index==-1) return 0;
            int system=SystemHitForEntry(index);
            if(system) {
                POINT screen=p; ClientToScreen(h,&screen); ShowSystemTrayContextMenu(system,screen); return 0;
            }
            RECT iconRect{}; if(!OverflowIconScreenRect(index,&iconRect)) return 0;
            POINT center{(iconRect.left+iconRect.right)/2,(iconRect.top+iconRect.bottom)/2};
            if(msg==WM_RBUTTONUP && s->trayIcons[index].owner) SetForegroundWindow(s->trayIcons[index].owner);
            SendPrivateIconEvent(index,msg==WM_LBUTTONUP?WM_LBUTTONDOWN:WM_RBUTTONDOWN,center);
            SendPrivateIconEvent(index,msg,center);
            if(s->trayIcons[index].version>=3) SendPrivateIconEvent(index,msg==WM_LBUTTONUP?NIN_SELECT:WM_CONTEXTMENU,center);
            if(msg==WM_LBUTTONUP) HidePrivateOverflow();
            return 0;
        }
        case WM_NCHITTEST: {
            LRESULT hit=DefWindowProcW(h,msg,wp,lp);
            switch(hit) {case HTTOP: case HTTOPRIGHT: case HTRIGHT: case HTBOTTOMRIGHT: case HTBOTTOM: case HTBOTTOMLEFT: case HTLEFT: case HTTOPLEFT:return HTBORDER; default:return hit;}
        }
        case WM_SETCURSOR: {
            POINT p{}; GetCursorPos(&p); ScreenToClient(h,&p); RECT footer=OverflowFooter();
            if(PtInRect(&footer,p)) {SetCursor(LoadCursorW(nullptr,MAKEINTRESOURCEW(32649)));return TRUE;} break;
        }
        case WM_CLOSE: HidePrivateOverflow(); return 0;
        }
    } catch(...) { HidePrivateOverflow(); return 0; }
    return DefWindowProcW(h,msg,wp,lp);
}
static void ShowPrivateOverflow() {
    if(!s || !s->bar) return;
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel)) { HidePrivateOverflow(); return; }
    if(!s->overflowPanel) {
        HINSTANCE instance=GetModuleHandleW(nullptr);
        WNDCLASSW wc{}; wc.hInstance=instance; wc.lpfnWndProc=PrivateOverflowProc;
        wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)); wc.lpszClassName=L"WH_Taskbar81Overflow";
        s->overflowClass=RegisterClassW(&wc);
        if(!s->overflowClass) { Wh_Log(L"Overflow class registration failed"); return; }
        s->overflowPanel=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,
            (LPCWSTR)(ULONG_PTR)(WORD)s->overflowClass,L"",TaskbarPopupStyle(),0,0,200,100,s->bar,nullptr,instance,nullptr);
        if(!s->overflowPanel) {UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->overflowClass,instance);s->overflowClass=0;return;}
        ConfigureSystemPopupFrame(s->overflowPanel);
        INITCOMMONCONTROLSEX controls{sizeof(controls),ICC_WIN95_CLASSES}; InitCommonControlsEx(&controls);
        s->overflowTooltip=CreateWindowExW(WS_EX_TOPMOST,TOOLTIPS_CLASSW,nullptr,WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,
            CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,s->overflowPanel,nullptr,instance,nullptr);
        if(s->overflowTooltip) { TOOLINFOW tool{}; tool.cbSize=sizeof(tool); tool.hwnd=s->overflowPanel; tool.uId=1;
            tool.uFlags=TTF_TRACK|TTF_ABSOLUTE; tool.lpszText=const_cast<wchar_t*>(L"");
            SendMessageW(s->overflowTooltip,TTM_ADDTOOLW,0,(LPARAM)&tool);
            SendMessageW(s->overflowTooltip,TTM_SETMAXTIPWIDTH,0,Px(300)); }
        EnsureTrayHoverImage();
    }
    s->overflowScroll=0;
    s->overflowMouseMask=((GetAsyncKeyState(VK_LBUTTON)&0x8000)?1:0)|((GetAsyncKeyState(VK_RBUTTON)&0x8000)?2:0);
    ResizePrivateOverflow();
    RECT client{}; GetClientRect(s->bar,&client); Layout layout=GetLayout(client.right,client.bottom);
    RECT anchor=s->verticalActive?GetVerticalLayout(client.right,client.bottom).arrow:RECT{layout.tray.left,0,layout.tray.left+OverflowButtonWidth(),client.bottom};
    MapWindowPoints(s->bar,nullptr,reinterpret_cast<POINT*>(&anchor),2); s->overflowAnchor=anchor;
    RECT window{}; GetWindowRect(s->overflowPanel,&window); int x=0,y=0;
    ComputeOverflowNear(anchor,window.right-window.left,window.bottom-window.top,x,y);
    SetWindowPos(s->overflowPanel,HWND_TOPMOST,x,y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
    InvalidateRect(s->overflowPanel,nullptr,FALSE);
}
// Drag capture and the identity are independent of vector indices: NIM_DELETE
// or NIM_MODIFY may arrive while the mouse is held. Never notify the owner of
// a click if the operation became a drag or if capture is lost.
// Render synthetic icons on a transparent premultiplied surface, not onto a
// magenta color key. Antialiased pixels otherwise blend with that key and the
// magenta halo remains visible. Application-icon ghost path is unchanged.
static void DrawSystemTrayGhost(Graphics& g,int hit,int width,int height) {
    float x=width/2.0f,y=height/2.0f,u=Dpi()/96.0f;
    if(hit==HIT_NETWORK)DrawNetworkGlyph(g,x,y,u,s->netKind,true);
    if(hit==HIT_VOLUME)DrawVolumeGlyph(g,x,y,u,s->volume,s->muted,true);
    if(hit==HIT_BATTERY)DrawBatteryGlyph(g,x,y,u,s->batteryPct,s->batteryCharging,true);
}
static HICON CreateCniSystemImage(int hit) {
    if(!s) return nullptr;
    // taskbarcpl's list uses a 32px representation. Render the glyph at that
    // size instead of handing it a temporary 16px HICON or a destroyed handle.
    constexpr int size=32;
    Bitmap bitmap(size,size,PixelFormat32bppARGB);
    if(bitmap.GetLastStatus()!=Ok) return nullptr;
    {
        Graphics g(&bitmap);
        g.Clear(Color(0,0,0,0));
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        float x=size/2.0f,y=size/2.0f,u=size/16.0f;
        if(hit==HIT_NETWORK) DrawNetworkGlyph(g,x,y,u,s->netKind,true);
        if(hit==HIT_VOLUME) DrawVolumeGlyph(g,x,y,u,s->volume,s->muted,true);
        if(hit==HIT_BATTERY) DrawBatteryGlyph(g,x,y,u,s->batteryPct,s->batteryCharging,true);
        g.Flush(FlushIntentionSync);
    }
    HICON result=nullptr;
    return bitmap.GetHICON(&result)==Ok ? result : nullptr;
}

static void RemoveCniSystemIcon(int hit) {
    if(!s) return;
    int slot=CniSystemSlot(hit);
    if(slot<0) return;
    if(!s->cniSystemRegistered[slot] && !s->cniSystemPublishedIcons[slot] && !s->cniSystemNid[slot].cbSize) return;
    if(!s->privateActive || !s->bar || !IsWindow(s->bar)) return;
    NOTIFYICONDATAW nid=s->cniSystemNid[slot];
    if(!nid.cbSize) {
        nid.cbSize=sizeof(nid); nid.hWnd=s->bar;
        nid.uID=(UINT)(0xC000-hit); nid.uFlags=NIF_GUID;
        nid.guidItem=CniSystemGuid(hit);
    }
    if(Shell_NotifyIconW(NIM_DELETE,&nid)) {
        if(s->cniSystemPublishedIcons[slot]) DestroyIcon(s->cniSystemPublishedIcons[slot]);
        s->cniSystemPublishedIcons[slot]=nullptr;
        s->cniSystemNid[slot]=NOTIFYICONDATAW{};
    } else {
        // Keep the last published HICON alive if a native shell still has it.
        Wh_Log(L"[T81-CNI] registered system delete failed hit=%d error=%lu",hit,GetLastError());
    }
}

static void PublishCniSystemIcon(int hit) {
    if(!s || !s->privateActive || !s->bar || !IsWindow(s->bar)) return;
    int slot=CniSystemSlot(hit);
    if(slot<0) return;
    if(hit==HIT_BATTERY && !s->hasBattery) { RemoveCniSystemIcon(hit); return; }
    HICON fresh=CreateCniSystemImage(hit);
    if(!fresh) { Wh_Log(L"[T81-CNI] registered system image failed hit=%d",hit); return; }

    NOTIFYICONDATAW nid{};
    nid.cbSize=sizeof(nid); nid.hWnd=s->bar; nid.uID=(UINT)(0xC000-hit);
    nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_GUID|NIF_SHOWTIP;
    nid.uCallbackMessage=WM_T81_CNI_ICON; nid.guidItem=CniSystemGuid(hit); nid.hIcon=fresh;
    std::wstring tip=SystemTooltipText(hit);
    size_t length=std::min(tip.size(),std::size(nid.szTip)-1);
    if(length) memcpy(nid.szTip,tip.data(),length*sizeof(wchar_t));
    nid.szTip[length]=L'\0';

    NOTIFYICONDATAW del{};
    del.cbSize=sizeof(del); del.hWnd=nid.hWnd; del.uID=nid.uID;
    del.uFlags=NIF_GUID; del.guidItem=nid.guidItem;
    BOOL ok=FALSE;
    bool wasRegistered=s->cniSystemRegistered[slot];
    if(wasRegistered) {
        ok=Shell_NotifyIconW(NIM_MODIFY,&nid);
        if(!ok) { Shell_NotifyIconW(NIM_DELETE,&del); ok=Shell_NotifyIconW(NIM_ADD,&nid); }
    } else {
        // A private GUID can safely be cleaned after an Explorer restart.
        Shell_NotifyIconW(NIM_DELETE,&del);
        ok=Shell_NotifyIconW(NIM_ADD,&nid);
    }
    if(!ok) {
        DestroyIcon(fresh);
        Wh_Log(L"[T81-CNI] registered system publish failed hit=%d error=%lu",hit,GetLastError());
        return;
    }
    NOTIFYICONDATAW version{};
    version.cbSize=sizeof(version); version.hWnd=nid.hWnd; version.uID=nid.uID;
    version.uFlags=NIF_GUID; version.guidItem=nid.guidItem;
    version.uVersion=NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION,&version);

    HICON old=s->cniSystemPublishedIcons[slot];
    s->cniSystemPublishedIcons[slot]=fresh;
    s->cniSystemNid[slot]=nid;
    if(old && old!=fresh) DestroyIcon(old);
    if(!wasRegistered)
        Wh_Log(L"[T81-CNI] Shell_NotifyIcon registered hit=%d slot=%d",hit,slot);
}

static void InitializeCniSystemIcons() {
    PublishCniSystemIcon(HIT_NETWORK);
    PublishCniSystemIcon(HIT_VOLUME);
    if(s && s->hasBattery) PublishCniSystemIcon(HIT_BATTERY);
}
static void RemoveAllCniSystemIcons() {
    for(int hit : {HIT_NETWORK,HIT_VOLUME,HIT_BATTERY}) RemoveCniSystemIcon(hit);
}

static bool ShowSystemTrayGhost(HWND ghost,POINT origin,int hit) {
    if(!s || !ghost || !SystemHitForKey(SystemKey(hit)))return false;
    int size=std::max(1,Px(28));HDC screen=GetDC(nullptr);if(!screen)return false;
    HDC mem=CreateCompatibleDC(screen);if(!mem){ReleaseDC(nullptr,screen);return false;}
    BITMAPINFO bi{};bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bi.bmiHeader.biWidth=size;bi.bmiHeader.biHeight=-size;
    bi.bmiHeader.biPlanes=1;bi.bmiHeader.biBitCount=32;bi.bmiHeader.biCompression=BI_RGB;void* bits=nullptr;
    HBITMAP dib=CreateDIBSection(screen,&bi,DIB_RGB_COLORS,&bits,nullptr,0);
    HGDIOBJ old=dib?SelectObject(mem,dib):nullptr;bool rendered=false;
    if(dib && bits && old && old!=HGDI_ERROR) {
        Bitmap bitmap(size,size,size*4,PixelFormat32bppPARGB,static_cast<BYTE*>(bits));
        if(bitmap.GetLastStatus()==Ok) {
            Graphics g(&bitmap);g.Clear(Color(0,0,0,0));DrawSystemTrayGhost(g,hit,size,size);g.Flush(FlushIntentionSync);
            POINT src{0,0};SIZE dim{size,size};BLENDFUNCTION blend{AC_SRC_OVER,0,200,AC_SRC_ALPHA};
            rendered=UpdateLayeredWindow(ghost,screen,&origin,&dim,mem,&src,0,&blend,ULW_ALPHA)!=FALSE;
        }
    }
    if(old && old!=HGDI_ERROR)SelectObject(mem,old);if(dib)DeleteObject(dib);DeleteDC(mem);ReleaseDC(nullptr,screen);
    return rendered;
}
static LRESULT CALLBACK TrayGhostProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_NCHITTEST) return HTTRANSPARENT;
    if(msg==WM_ERASEBKGND) return 1;
    if(msg==WM_PAINT) {
        PAINTSTRUCT ps{}; HDC dc=BeginPaint(h,&ps); RECT r{}; GetClientRect(h,&r);
        if(s && s->trayDrag.systemHit) {EndPaint(h,&ps);return 0;} // per-pixel surface already supplied by UpdateLayeredWindow
        HBRUSH b=CreateSolidBrush(RGB(255,0,255)); FillRect(dc,&r,b); DeleteObject(b);
        if(s && s->trayDrag.image) { int n=Px(16); DrawIconEx(dc,(r.right-n)/2,(r.bottom-n)/2,s->trayDrag.image,n,n,0,nullptr,DI_NORMAL); }
        EndPaint(h,&ps); return 0;
    }
    return DefWindowProcW(h,msg,wp,lp);
}
static constexpr UINT WM_TRAY_DRAG_MOVE=WM_APP+453,WM_TRAY_DRAG_END=WM_APP+454;
// Temporary hook is on the bar's UI thread. It never swallows OS mouse
// events: suppressing button-up can leave global button state inconsistent.
// Both clicks and drags are finished on the UI thread, even if the
// non-activating popup could not acquire mouse capture.
static LRESULT CALLBACK TrayDragHookProc(int code,WPARAM message,LPARAM data) {
    if(code>=0 && s && s->trayDrag.capture && data && !s->trayDrag.endQueued) {
        auto* e=reinterpret_cast<MSLLHOOKSTRUCT*>(data);
        if(message==WM_MOUSEMOVE && !s->trayDrag.moveQueued) {
            if(PostMessageW(s->bar,WM_TRAY_DRAG_MOVE,(WPARAM)(INT_PTR)e->pt.x,(LPARAM)e->pt.y))s->trayDrag.moveQueued=true;
        } else if(message==WM_LBUTTONUP) {
            if(PostMessageW(s->bar,WM_TRAY_DRAG_END,(WPARAM)(INT_PTR)e->pt.x,(LPARAM)e->pt.y)) s->trayDrag.endQueued=true;
        }
    }
    return CallNextHookEx(nullptr,code,message,data);
}
static void CancelTrayDrag() {
    if(!s) return;
    HWND capture=s->trayDrag.capture, ghost=s->trayDrag.ghost;HHOOK hook=s->trayDrag.hook;
    HICON image=s->trayDrag.image;
    s->trayDrag=TrayDragState{};
    if(hook)UnhookWindowsHookEx(hook);
    if(capture && GetCapture()==capture) ReleaseCapture();
    if(ghost && IsWindow(ghost)) DestroyWindow(ghost);
    if(ghost) UnregisterClassW(L"WH_Taskbar81TrayGhost",GetModuleHandleW(nullptr));
    if(image) DestroyIcon(image);
    if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel))InvalidateRect(s->overflowPanel,nullptr,FALSE);
}
#ifdef TASKBAR81_TEST
static bool testDisableTrayDragHook=false;
#endif
static void BeginTrayDrag(HWND capture,int index,bool fromOverflow,POINT client) {
    if(!s) return;
    int system=SystemHitForEntry(index);
    if(!system && (index<0 || (size_t)index>=s->trayIcons.size() || s->trayIcons[index].preferenceKey.empty())) return;
    CancelTrayDrag();
    // Never let a stale hover timer or a topmost tooltip intercept this gesture.
    if(s->bar)KillTimer(s->bar,TIMER_HOVER);
    HidePreview();
    s->hover=HIT_NONE;s->hot=HIT_NONE;
    s->trayDrag.capture=capture; s->trayDrag.fromOverflow=fromOverflow; s->trayDrag.systemHit=system;
    s->trayDrag.key=system?SystemKey(system):s->trayIcons[index].preferenceKey;
    s->trayDrag.origin=client; ClientToScreen(capture,&s->trayDrag.origin);
    if(!system) s->trayDrag.image=s->trayIcons[index].icon?CopyIcon(s->trayIcons[index].icon):nullptr;
    HMODULE module=nullptr;
#ifdef TASKBAR81_TEST
    if(!testDisableTrayDragHook)
#endif
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)TrayDragHookProc,&module))
        s->trayDrag.hook=SetWindowsHookExW(WH_MOUSE_LL,TrayDragHookProc,module,0);
    SetCapture(capture);
    if(GetCapture()!=capture && !s->trayDrag.hook) {CancelTrayDrag();return;}
    if(s->overflowTooltip)UpdateOverflowTooltip(-1,POINT{});
}
static void UpdateTrayDropFeedback(POINT screen) {
    if(!s || !s->trayDrag.active)return;
    s->trayDrag.dropArea=0;s->trayDrag.dropCell=RECT{};s->trayDrag.dropAfter=false;s->trayDrag.dropAnchor.clear();
    POINT q=screen;RECT r{};
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel) && ScreenToClient(s->overflowPanel,&q) && GetClientRect(s->overflowPanel,&r) && PtInRect(&r,q)) {
        s->trayDrag.dropArea=2;int entry=OverflowHit(q);auto items=OverflowPrivateItems();
        auto it=std::find(items.begin(),items.end(),entry);
        if(entry!=-1 && it!=items.end()) {s->trayDrag.dropAnchor=TrayEntryKey(entry);
            s->trayDrag.dropCell=OverflowCell((int)(it-items.begin()));
            s->trayDrag.dropAfter=q.x>=(s->trayDrag.dropCell.left+s->trayDrag.dropCell.right)/2;
            }
    } else {
        q=screen;if(ScreenToClient(s->bar,&q) && GetClientRect(s->bar,&r)) {
            Layout l=GetLayout(r.right,r.bottom);auto v=s->verticalActive?GetVerticalLayout(r.right,r.bottom):VerticalLayout{};
            RECT tray=s->verticalActive?RECT{0,v.arrow.top,r.right,v.clock.top}:l.tray;
            if(PtInRect(&tray,q)) {
                s->trayDrag.dropArea=1;auto shown=VisibleTrayEntries();
                for(int index:shown) {RECT cell=TrayBarEntryRect(index);if(PtInRect(&cell,q)) {
                    s->trayDrag.dropAnchor=TrayEntryKey(index);s->trayDrag.dropCell=cell;
                    s->trayDrag.dropAfter=s->verticalActive?q.y>=(cell.top+cell.bottom)/2:q.x>=(cell.left+cell.right)/2;break;
                }}
            }
        }
    }
    if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel))InvalidateRect(s->overflowPanel,nullptr,FALSE);
}
static void MoveTrayDrag(HWND capture,POINT client) {
    if(!s || s->trayDrag.capture!=capture) return;
    POINT p=client; ClientToScreen(capture,&p);
    if(!s->trayDrag.active && (std::abs(p.x-s->trayDrag.origin.x)>=std::max(1,GetSystemMetrics(SM_CXDRAG)/2) ||
       std::abs(p.y-s->trayDrag.origin.y)>=std::max(1,GetSystemMetrics(SM_CYDRAG)/2))) {
        s->trayDrag.active=true;
        WNDCLASSW wc{}; wc.lpfnWndProc=TrayGhostProc; wc.hInstance=GetModuleHandleW(nullptr);
        wc.lpszClassName=L"WH_Taskbar81TrayGhost";
        if(RegisterClassW(&wc)) {
            s->trayDrag.ghost=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT|WS_EX_LAYERED,
                wc.lpszClassName,L"",WS_POPUP,p.x,p.y,Px(28),Px(28),s->bar,nullptr,wc.hInstance,nullptr);
            if(s->trayDrag.ghost) {
                if(s->trayDrag.systemHit) {
                    POINT origin{p.x-Px(14),p.y-Px(14)};
                    if(!ShowSystemTrayGhost(s->trayDrag.ghost,origin,s->trayDrag.systemHit)) {
                        Wh_Log(L"[T81-DIAG] system drag ghost alpha render failed; drag continues without ghost");
                        DestroyWindow(s->trayDrag.ghost);s->trayDrag.ghost=nullptr;UnregisterClassW(wc.lpszClassName,wc.hInstance);
                    }
                } else SetLayeredWindowAttributes(s->trayDrag.ghost,RGB(255,0,255),200,LWA_COLORKEY|LWA_ALPHA);
            } else UnregisterClassW(wc.lpszClassName,wc.hInstance);
        }
    }
    if(s->trayDrag.active && s->trayDrag.ghost)
        SetWindowPos(s->trayDrag.ghost,HWND_TOPMOST,p.x-Px(14),p.y-Px(14),0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
    if(s->trayDrag.active)UpdateTrayDropFeedback(p);
}
static int FindTrayKey(const std::wstring& key) {
    int system=SystemHitForKey(key); if(system) return SystemEntry(system);
    for(size_t i=0;i<s->trayIcons.size();++i) if(s->trayIcons[i].preferenceKey==key) return (int)i;
    return -1;
}
static void SetTrayPinned(const std::wstring& key,bool pinned) {
    if(!s || key.empty()) return;
    if(SystemHitForKey(key)) {
        auto it=std::find(s->trayHiddenSystem.begin(),s->trayHiddenSystem.end(),key);
        if(!pinned && it==s->trayHiddenSystem.end()) s->trayHiddenSystem.push_back(key);
        else if(pinned && it!=s->trayHiddenSystem.end()) s->trayHiddenSystem.erase(it);
        SaveSystemPreferences();
    } else {
        auto it=std::find(s->trayPinned.begin(),s->trayPinned.end(),key);
        if(pinned && it==s->trayPinned.end() && s->trayPinned.size()<MAX_TASKS) s->trayPinned.push_back(key);
        if(!pinned && it!=s->trayPinned.end()) s->trayPinned.erase(it);
        SaveTrayPreferences();
    }
    if(s->bar) {BuildGroups();InvalidateRect(s->bar,nullptr,FALSE);}
    if(s->overflowPanel && IsWindowVisible(s->overflowPanel)) ResizePrivateOverflow();
}
// ----- [T81-CNI] Undocumented CNI provider, x86-64. -----
// ABI checked against taskbarcpl.dll 10.0.10240.16384, 10.0.22621.1
// and 10.0.26100.4484. Close the page before mod unload.
struct NOTIFYITEM {
    PWSTR pszExeName;
    PWSTR pszTip;
    HICON hIcon;
    HWND hWnd;
    DWORD dwPreference;
    UINT uID;
    GUID guidItem;
};
static_assert(sizeof(NOTIFYITEM) == 0x38);
static_assert(offsetof(NOTIFYITEM, dwPreference) == 0x20);
static_assert(offsetof(NOTIFYITEM, guidItem) == 0x28);

struct INotificationCB : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Notify(ULONG event, NOTIFYITEM* item) = 0;
};
struct ITrayNotifyWin8 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE RegisterCallback(
        INotificationCB* callback, ULONG* cookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterCallback(ULONG cookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPreference(const NOTIFYITEM* item) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableAutoTray(BOOL enabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoAction(BOOL action) = 0;
};

constexpr GUID T81_CLSID_TrayNotify = {
    0x25DEAD04, 0x1EAC, 0x4911,
    {0x9E, 0x3A, 0xAD, 0x0A, 0x4A, 0xB5, 0x60, 0xFD}
};
// A private activation identity avoids relying on factory precedence for the
// system's CLSID_TrayNotify. Only the filtered CoCreateInstance hook below
// redirects clients to this identity; normal COM marshalling is preserved.
constexpr GUID T81_CLSID_PrivateTrayNotify = {
    0x30E8C40C, 0xE870, 0x4659,
    {0xAC, 0xC1, 0xF5, 0x7E, 0x8C, 0xAA, 0x5A, 0x50}
};
[[maybe_unused]] constexpr GUID T81_IID_INotificationCB = {
    0xD782CCBA, 0xAFB0, 0x43F1,
    {0x94, 0xDB, 0xFD, 0xA3, 0x77, 0x9E, 0xAC, 0xCB}
};
constexpr GUID T81_IID_ITrayNotifyWin8 = {
    0xD133CE13, 0x3537, 0x48BA,
    {0x93, 0xA7, 0xAF, 0xCD, 0x5D, 0x20, 0x53, 0xB4}
};

enum class CniUiRequestKind { Register, Unregister, SetPreference, EnableAutoTray };
constexpr UINT WM_T81_CNI_REQUEST = WM_APP + 948;
struct CniUiRequest {
    CniUiRequestKind kind = CniUiRequestKind::SetPreference;
    ULONG cookie = 0;
    INotificationCB* callback = nullptr; // owned until the request is deleted
    HWND owner = nullptr;
    UINT id = 0;
    GUID guid{};
    DWORD preference = 0;
    BOOL enabled = FALSE;
    CniUiRequest() = default;
    CniUiRequest(const CniUiRequest&) = delete;
    CniUiRequest& operator=(const CniUiRequest&) = delete;
    ~CniUiRequest() { if (callback) callback->Release(); }
};
struct CniUiSyncResult {
    HRESULT hr = E_FAIL;
    std::atomic<bool> completed{false};
};
static HRESULT QueueCniUiRequest(CniUiRequest* request, bool waitForUi = false);
static std::atomic<ULONG> g_trayCniNextCookie{1};
static std::atomic<unsigned> g_trayCniCallbackDepth{0};
struct CniCallbackScope {
    CniCallbackScope() { g_trayCniCallbackDepth.fetch_add(1, std::memory_order_acq_rel); }
    ~CniCallbackScope() { g_trayCniCallbackDepth.fetch_sub(1, std::memory_order_acq_rel); }
};

class CPrivateTrayNotify final : public ITrayNotifyWin8 {
    struct CallbackSlot {
        ULONG cookie = 0;
        INotificationCB* callback = nullptr; // strong reference
    };

    std::atomic<ULONG> refs_{1}; // one reference owned by g_trayCni
    std::vector<CallbackSlot> callbacks_;

    DWORD PreferenceOf(const PrivateIcon& icon) const {
        if (!s) return 0;
        if (s->trayShowAll) return 2;
        int system = SystemHitForKey(icon.preferenceKey);
        if (system) return IsSystemVisible(system) ? 2 : 0;
        return std::find(s->trayPinned.begin(), s->trayPinned.end(),
                         icon.preferenceKey) != s->trayPinned.end() ? 2 : 0;
    }

    bool CniSystemAvailable(int hit) const {
        int slot=CniSystemSlot(hit);
        return s && slot>=0 && s->cniSystemRegistered[slot];
    }

    bool MakeItem(const PrivateIcon& icon, std::wstring& exe,
                  NOTIFYITEM& item) const {
        if (!s || icon.preferenceKey.empty()) return false;
        int system=CniSystemHitFromGuid(icon.guid);
        if(system && (!icon.icon || !icon.owner)) return false;
        // For our private GUIDs taskbarcpl's SetFriendlyName falls back to the
        // final path component. A non-empty display token therefore gives the
        // three rows stable localized names without reserved SCA identities.
        exe = system ? CniSystemDisplayName(system) : ProcessImage(icon.ownerPid);
        item = {};
        item.pszExeName = exe.empty() ? const_cast<PWSTR>(L"") : exe.data();
        item.pszTip = icon.tip.empty() ? const_cast<PWSTR>(L"") :
                                         const_cast<PWSTR>(icon.tip.c_str());
        item.hIcon = icon.icon;
        item.hWnd = icon.owner;
        item.dwPreference = PreferenceOf(icon);
        item.uID = icon.id;
        item.guidItem = icon.guid;
        return true;
    }

    void RefreshPrivateTray() {
        if (!s) return;
        if (s->bar) {
            BuildGroups();
            InvalidateRect(s->bar, nullptr, FALSE);
        }
        if (s->overflowPanel && IsWindowVisible(s->overflowPanel))
            ResizePrivateOverflow();
    }

public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, T81_IID_ITrayNotifyWin8)) {
            *ppv = static_cast<ITrayNotifyWin8*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return refs_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG left = refs_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!left) delete this;
        return left;
    }

    ~CPrivateTrayNotify() { ResetCallbacks(); }

    void ResetCallbacks() noexcept {
        for (auto& entry : callbacks_)
            if (entry.callback) entry.callback->Release();
        callbacks_.clear();
    }

    HRESULT RegisterCallbackOnUi(INotificationCB* callback, ULONG id) {
        if (!callback || !id) return E_INVALIDARG;
        if (!s || !s->privateActive) return E_UNEXPECTED;
        try {
            if (callbacks_.size() >= 64) return E_OUTOFMEMORY;

            callback->AddRef();
            std::unique_ptr<INotificationCB, ReleaseCom<INotificationCB>>
                enumerateGuard(callback);

            callbacks_.push_back({id, callback});
            callback->AddRef();

            for (const auto& icon : s->trayIcons) {
                std::wstring exe;
                NOTIFYITEM item{};
                if (MakeItem(icon, exe, item)) {
                    CniCallbackScope callbackScope;
                    callback->Notify(0, &item);
                }
            }
            unsigned systemCount = 0;
            for (int hit : {HIT_NETWORK, HIT_VOLUME, HIT_BATTERY}) {
                int slot=CniSystemSlot(hit);
                if(slot<0 || !s->cniSystemRegistered[slot]) continue;
                // This HICON belongs to State and remains valid after Notify;
                // unlike v6, no temporary image is destroyed under the CPL.
                const PrivateIcon& system=s->cniSystemIcons[slot];
                std::wstring exe;
                NOTIFYITEM item{};
                if (MakeItem(system, exe, item)) {
                    CniCallbackScope callbackScope;
                    callback->Notify(0, &item);
                    ++systemCount;
                }
            }
            Wh_Log(L"[T81-CNI] private RegisterCallback cookie=%lu icons=%u system=%u tid=%lu",
                   (unsigned long)id, (unsigned)s->trayIcons.size(), systemCount,
                   GetCurrentThreadId());
            return S_OK;
        } catch (const std::bad_alloc&) {
            if (id) UnregisterCallbackOnUi(id);
            return E_OUTOFMEMORY;
        } catch (...) {
            if (id) UnregisterCallbackOnUi(id);
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE RegisterCallback(
        INotificationCB* callback, ULONG* cookie) override {
        if (!callback || !cookie) return E_POINTER;
        *cookie = 0;
        ULONG id = g_trayCniNextCookie.fetch_add(1);
        if (!id) id = g_trayCniNextCookie.fetch_add(1);
        auto* request = new (std::nothrow) CniUiRequest();
        if (!request) return E_OUTOFMEMORY;
        request->kind = CniUiRequestKind::Register;
        request->cookie = id;
        request->callback = callback;
        callback->AddRef(); // owned by the queued request
        HRESULT hr = QueueCniUiRequest(request);
        if (SUCCEEDED(hr)) *cookie = id;
        Wh_Log(L"[T81-CNI] RegisterCallback queued cookie=%lu tid=%lu hr=0x%08lx",
               (unsigned long)id, GetCurrentThreadId(), (unsigned long)hr);
        return hr;
    }

    HRESULT UnregisterCallbackOnUi(ULONG cookie) {
        auto it = std::find_if(callbacks_.begin(), callbacks_.end(),
            [cookie](const CallbackSlot& entry) {
                return entry.cookie == cookie;
            });
        if (it != callbacks_.end()) {
            INotificationCB* callback = it->callback;
            callbacks_.erase(it);
            if (callback) callback->Release();
            Wh_Log(L"[T81-CNI] private UnregisterCallback cookie=%lu",
                   (unsigned long)cookie);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE UnregisterCallback(ULONG cookie) override {
        auto* request = new (std::nothrow) CniUiRequest();
        if (!request) return E_OUTOFMEMORY;
        request->kind = CniUiRequestKind::Unregister;
        request->cookie = cookie;
        return QueueCniUiRequest(request);
    }

    HRESULT SetPreferenceOnUi(const NOTIFYITEM* item) {
        if (!item) return E_POINTER;
        if (!s || !s->privateActive) return E_UNEXPECTED;
        try {
            DWORD preference = item->dwPreference;
            if (preference == 0x8000) preference = 0;
            if (preference > 2) return E_INVALIDARG;

            int system = CniSystemHitFromGuid(item->guidItem);
            if (system) {
                if (!CniSystemAvailable(system)) return S_OK;
                SetTrayPinned(SystemKey(system), preference == 2);
                Wh_Log(L"[T81-CNI] private SetPreference applied system=%d preference=%lu tid=%lu",
                       system, (unsigned long)preference, GetCurrentThreadId());
                return S_OK;
            }
            int index = FindPrivateIcon(item->hWnd, item->uID, item->guidItem);
            if (index < 0) return S_OK;
            const auto& key = s->trayIcons[index].preferenceKey;
            if (key.empty()) return S_OK;

            SetTrayPinned(key, preference == 2);
            Wh_Log(L"[T81-CNI] private SetPreference applied id=%lu preference=%lu tid=%lu",
                   (unsigned long)item->uID, (unsigned long)preference,
                   GetCurrentThreadId());
            return S_OK;
        } catch (const std::bad_alloc&) {
            return E_OUTOFMEMORY;
        } catch (...) {
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE SetPreference(const NOTIFYITEM* item) override {
        if (!item) return E_POINTER;
        // Copy the marshalled input before returning. No NOTIFYITEM/string
        // pointer is retained, and no State/window function runs on this thread.
        Wh_Log(L"[T81-CNI] SetPreference enter id=%lu preference=%lu tid=%lu",
               (unsigned long)item->uID, (unsigned long)item->dwPreference,
               GetCurrentThreadId());
        auto* request = new (std::nothrow) CniUiRequest();
        if (!request) return E_OUTOFMEMORY;
        request->kind = CniUiRequestKind::SetPreference;
        request->owner = item->hWnd;
        request->id = item->uID;
        request->guid = item->guidItem;
        request->preference = item->dwPreference;
        // taskbarcpl expects the write to be complete when this method returns.
        return QueueCniUiRequest(request, true);
    }

    HRESULT EnableAutoTrayOnUi(BOOL enabled) {
        if (!s || !s->privateActive) return E_UNEXPECTED;
        try {
            DWORD value = enabled ? 1u : 0u;
            LSTATUS error = RegSetKeyValueW(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",
                L"EnableAutoTray", REG_DWORD, &value, sizeof(value));
            if (error != ERROR_SUCCESS) return HRESULT_FROM_WIN32(error);

            s->trayShowAll = !enabled;
            RefreshPrivateTray();
            Wh_Log(L"[T81-CNI] private EnableAutoTray applied enabled=%d tid=%lu",
                   (int)enabled, GetCurrentThreadId());
            return S_OK;
        } catch (...) {
            return E_FAIL;
        }
    }

    HRESULT STDMETHODCALLTYPE EnableAutoTray(BOOL enabled) override {
        // Write the registry synchronously because taskbarcpl UpdateUI reads it
        // immediately after this call. Only private State/layout is deferred.
        DWORD value = enabled ? 1u : 0u;
        LSTATUS error = RegSetKeyValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",
            L"EnableAutoTray", REG_DWORD, &value, sizeof(value));
        if (error != ERROR_SUCCESS) return HRESULT_FROM_WIN32(error);
        Wh_Log(L"[T81-CNI] EnableAutoTray queued enabled=%d tid=%lu",
               (int)enabled, GetCurrentThreadId());
        auto* request = new (std::nothrow) CniUiRequest();
        if (!request) return E_OUTOFMEMORY;
        request->kind = CniUiRequestKind::EnableAutoTray;
        request->enabled = enabled;
        return QueueCniUiRequest(request, true);
    }

    HRESULT STDMETHODCALLTYPE DoAction(BOOL action) override {
        (void)action;
        return S_OK;
    }

    void NotifyOne(ULONG event, const PrivateIcon& icon) noexcept {
        if (callbacks_.empty()) return;
        try {
            std::wstring exe;
            NOTIFYITEM item{};
            if (!MakeItem(icon, exe, item)) return;

            std::vector<CallbackSlot> copied = callbacks_;
            for (auto& entry : copied)
                if (entry.callback) entry.callback->AddRef();
            struct ReleaseCopied {
                std::vector<CallbackSlot>& entries;
                ~ReleaseCopied() {
                    for (auto& entry : entries)
                        if (entry.callback) entry.callback->Release();
                }
            } releaseCopied{copied};

            for (auto& entry : copied) {
                if (!entry.callback) continue;
                CniCallbackScope callbackScope;
                entry.callback->Notify(event, &item);
            }
        } catch (...) {
            Wh_Log(L"[T81-CNI] exception while broadcasting a tray change");
        }
    }
};

class CPrivateTrayFactory final : public IClassFactory {
    std::atomic<ULONG> refs_{1}; // one reference owned by g_trayCniFactory
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_IClassFactory)) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return refs_.fetch_add(1, std::memory_order_relaxed) + 1;
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG left = refs_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!left) delete this;
        return left;
    }
    HRESULT STDMETHODCALLTYPE CreateInstance(
        IUnknown* outer, REFIID riid, void** ppv) override;
    HRESULT STDMETHODCALLTYPE LockServer(BOOL lock) override {
        (void)lock;
        return S_OK;
    }
};

static CPrivateTrayNotify* g_trayCni = nullptr;
static CPrivateTrayFactory* g_trayCniFactory = nullptr;
static DWORD g_trayCniCookie = 0;
static bool g_trayCniRegistered = false;
static bool g_trayCniCoInitialized = false;
static DWORD g_trayCniPrivateCookie = 0;
// Lifecycle flag for the private registration. Activation attempts may also
// come from another Explorer process and therefore do not test this local flag.
static std::atomic<bool> g_trayCniPrivateRegistered{false};
static std::atomic<DWORD> g_trayCniUiThreadId{0};

HRESULT STDMETHODCALLTYPE CPrivateTrayFactory::CreateInstance(
    IUnknown* outer, REFIID riid, void** ppv) {
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (outer) return CLASS_E_NOAGGREGATION;
    if (!g_trayCni) return E_UNEXPECTED;
    return g_trayCni->QueryInterface(riid, ppv);
}

static HRESULT ProcessCniUiRequest(CniUiRequest* request) {
    if (!request) return E_POINTER;
    if (!g_trayCni || !s || !s->privateActive) return E_UNEXPECTED;
    switch (request->kind) {
    case CniUiRequestKind::Register:
        return g_trayCni->RegisterCallbackOnUi(request->callback, request->cookie);
    case CniUiRequestKind::Unregister:
        return g_trayCni->UnregisterCallbackOnUi(request->cookie);
    case CniUiRequestKind::SetPreference: {
        NOTIFYITEM item{};
        item.hWnd = request->owner;
        item.uID = request->id;
        item.guidItem = request->guid;
        item.dwPreference = request->preference;
        return g_trayCni->SetPreferenceOnUi(&item);
    }
    case CniUiRequestKind::EnableAutoTray:
        return g_trayCni->EnableAutoTrayOnUi(request->enabled);
    }
    return E_INVALIDARG;
}

static HRESULT QueueCniUiRequest(CniUiRequest* request, bool waitForUi) {
    if (!request) return E_POINTER;
    if (GetCurrentThreadId() == g_trayCniUiThreadId.load()) {
        HRESULT hr = ProcessCniUiRequest(request);
        delete request;
        return hr;
    }
    HWND bar = barWindow.load();
    if (!bar) {
        delete request;
        return E_UNEXPECTED;
    }
    bool callbackActive = g_trayCniCallbackDepth.load(std::memory_order_acquire) != 0;
    if (waitForUi && callbackActive) {
        // The UI is waiting for the CPL callback to return. A synchronous
        // SendMessage would deadlock; apply immediately after that callback.
        Wh_Log(L"[T81-CNI] reentrant write deferred until the active callback returns");
    }
    if (waitForUi && !callbackActive) {
        // SendMessage is synchronous: the stack result remains alive until the
        // UI has either completed the request or rejected the window message.
        // SetPreference/EnableAutoTray do not broadcast COM callbacks, so this
        // does not wait for a callback into the blocked Control Panel thread.
        CniUiSyncResult sync;
        SendMessageW(bar, WM_T81_CNI_REQUEST, (WPARAM)&sync, (LPARAM)request);
        if (!sync.completed.load(std::memory_order_acquire)) {
            delete request;
            Wh_Log(L"[T81-CNI] synchronous UI request was not accepted");
            return E_FAIL;
        }
        return sync.hr;
    }
    if (!PostMessageW(bar, WM_T81_CNI_REQUEST, 0, (LPARAM)request)) {
        DWORD error = GetLastError();
        delete request;
        return HRESULT_FROM_WIN32(error);
    }
    return S_OK;
}

static bool StartTrayNotifyServer() {
    if (g_trayCni) return true;
    if (!s || !s->privateActive) return false;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"[T81-CNI] CoInitializeEx failed hr=0x%08lx", (unsigned long)hr);
        return false;
    }
    g_trayCniCoInitialized = true; // bilanciare anche S_FALSE

    DWORD autoTray = 1, size = sizeof(autoTray);
    if (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",
            L"EnableAutoTray", RRF_RT_REG_DWORD, nullptr,
            &autoTray, &size) != ERROR_SUCCESS)
        autoTray = 1;
    s->trayShowAll = (autoTray == 0);

    g_trayCni = new (std::nothrow) CPrivateTrayNotify();
    g_trayCniFactory = new (std::nothrow) CPrivateTrayFactory();
    if (!g_trayCni || !g_trayCniFactory) {
        Wh_Log(L"[T81-CNI] out of memory creating the private provider");
        StopTrayNotifyServer();
        return false;
    }

    hr = CoRegisterClassObject(T81_CLSID_TrayNotify, g_trayCniFactory,
        CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_trayCniCookie);
    if (FAILED(hr)) {
        Wh_Log(L"[T81-CNI] CoRegisterClassObject failed hr=0x%08lx; "
               L"native CNI service left in use", (unsigned long)hr);
        StopTrayNotifyServer();
        return false;
    }
    g_trayCniRegistered = true;
    Wh_Log(L"[T81-CNI] private class factory registered cookie=%lu",
           (unsigned long)g_trayCniCookie);
    hr = CoRegisterClassObject(T81_CLSID_PrivateTrayNotify, g_trayCniFactory,
        CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &g_trayCniPrivateCookie);
    if (FAILED(hr)) {
        Wh_Log(L"[T81-CNI] private-CLSID registration failed hr=0x%08lx",
               (unsigned long)hr);
        StopTrayNotifyServer();
        return false;
    }
    g_trayCniPrivateRegistered.store(true);
    g_trayCniUiThreadId.store(GetCurrentThreadId());
    Wh_Log(L"[T81-CNI] private activation CLSID registered cookie=%lu",
           (unsigned long)g_trayCniPrivateCookie);
    return true;
}

static void StopTrayNotifyServer() {
    g_trayCniUiThreadId.store(0);
    if (s) s->trayShowAll = false;
    if (g_trayCniPrivateRegistered.exchange(false)) {
        HRESULT hr = CoRevokeClassObject(g_trayCniPrivateCookie);
        if (FAILED(hr))
            Wh_Log(L"[T81-CNI] private CoRevokeClassObject hr=0x%08lx",
                   (unsigned long)hr);
        g_trayCniPrivateCookie = 0;
    }
    if (g_trayCniRegistered) {
        HRESULT hr = CoRevokeClassObject(g_trayCniCookie);
        if (FAILED(hr))
            Wh_Log(L"[T81-CNI] CoRevokeClassObject hr=0x%08lx",
                   (unsigned long)hr);
        g_trayCniRegistered = false;
        g_trayCniCookie = 0;
    }
    if (g_trayCniFactory) {
        CoDisconnectObject(g_trayCniFactory, 0);
        ULONG left = g_trayCniFactory->Release();
        if (left)
            Wh_Log(L"[T81-CNI] factory still has %lu references", (unsigned long)left);
        g_trayCniFactory = nullptr;
    }
    if (g_trayCni) {
        g_trayCni->ResetCallbacks();
        CoDisconnectObject(g_trayCni, 0);
        ULONG left = g_trayCni->Release();
        if (left)
            Wh_Log(L"[T81-CNI] SERVER STILL HAS %lu REFERENCES; close the CNI "
                   L"page before unloading the mod", (unsigned long)left);
        g_trayCni = nullptr;
    }
    if (g_trayCniCoInitialized) {
        CoUninitialize();
        g_trayCniCoInitialized = false;
    }
}

static void NotifyTrayCni(ULONG event, const PrivateIcon& icon) {
    if (g_trayCni) g_trayCni->NotifyOne(event, icon);
}

// Client-side bridge returned only by our filtered activation hook. Register
// and callback teardown keep using the COM backend that already works. The two
// write methods stay in this mod, avoiding the first-write COM proxy path seen
// in the runtime log and applying the icon change before returning to the CPL.
class CTrayCniClientBridge final : public ITrayNotifyWin8 {
    std::atomic<ULONG> refs_{1};
    ITrayNotifyWin8* backend_;
public:
    explicit CTrayCniClientBridge(ITrayNotifyWin8* backend) : backend_(backend) {}
    ~CTrayCniClientBridge() { if (backend_) backend_->Release(); }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, T81_IID_ITrayNotifyWin8)) {
            *ppv = static_cast<ITrayNotifyWin8*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return refs_.fetch_add(1, std::memory_order_relaxed) + 1;
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG left = refs_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!left) delete this;
        return left;
    }
    HRESULT STDMETHODCALLTYPE RegisterCallback(
        INotificationCB* callback, ULONG* cookie) override {
        return backend_->RegisterCallback(callback, cookie);
    }
    HRESULT STDMETHODCALLTYPE UnregisterCallback(ULONG cookie) override {
        return backend_->UnregisterCallback(cookie);
    }
    HRESULT STDMETHODCALLTYPE SetPreference(const NOTIFYITEM* item) override {
        if (!g_trayCni || !g_trayCniPrivateRegistered.load()) return E_UNEXPECTED;
        Wh_Log(L"[T81-CNI] local SetPreference bridge tid=%lu", GetCurrentThreadId());
        return g_trayCni->SetPreference(item);
    }
    HRESULT STDMETHODCALLTYPE EnableAutoTray(BOOL enabled) override {
        if (!g_trayCni || !g_trayCniPrivateRegistered.load()) return E_UNEXPECTED;
        Wh_Log(L"[T81-CNI] local EnableAutoTray bridge tid=%lu", GetCurrentThreadId());
        return g_trayCni->EnableAutoTray(enabled);
    }
    HRESULT STDMETHODCALLTYPE DoAction(BOOL action) override {
        return backend_->DoAction(action);
    }
};

using TrayCniCoCreateInstance_t = decltype(&CoCreateInstance);
static TrayCniCoCreateInstance_t realTrayCniCoCreateInstance = nullptr;

static HRESULT WINAPI TrayCniCoCreateInstanceHook(
    REFCLSID rclsid, LPUNKNOWN outer, DWORD context, REFIID riid, LPVOID* ppv) {
    // taskbarcpl.dll uses exactly CLSCTX_LOCAL_SERVER + ITrayNotifyWin8.
    // Every other CoCreateInstance call, including our private CLSID, follows
    // the original path. No direct cross-thread State access occurs here.
    if (ppv && !outer && (context & CLSCTX_LOCAL_SERVER) &&
        IsEqualCLSID(rclsid, T81_CLSID_TrayNotify) &&
        IsEqualIID(riid, T81_IID_ITrayNotifyWin8)) {
        *ppv = nullptr;
        HRESULT hr = realTrayCniCoCreateInstance(T81_CLSID_PrivateTrayNotify,
            nullptr, context, riid, ppv);
        if (SUCCEEDED(hr)) {
            // Only a same-process provider can use the local write bridge.
            // Otherwise preserve the ordinary COM object and its marshalling.
            if (g_trayCni && g_trayCniPrivateRegistered.load()) {
                auto* backend = static_cast<ITrayNotifyWin8*>(*ppv);
                auto* bridge = new (std::nothrow) CTrayCniClientBridge(backend);
                if (bridge) {
                    *ppv = bridge;
                    Wh_Log(L"[T81-CNI] activation redirected with local synchronous "
                           L"write bridge pid=%lu tid=%lu hr=0x%08lx",
                           GetCurrentProcessId(), GetCurrentThreadId(),
                           (unsigned long)hr);
                    return S_OK;
                }
            }
            Wh_Log(L"[T81-CNI] activation redirected pid=%lu tid=%lu hr=0x%08lx",
                   GetCurrentProcessId(), GetCurrentThreadId(), (unsigned long)hr);
            return hr;
        }
        Wh_Log(L"[T81-CNI] private activation failed hr=0x%08lx; native fallback",
               (unsigned long)hr);
        // Preserve the page if the private server is stopping or marshalling
        // is unavailable: retry the exact original request.
    }
    return realTrayCniCoCreateInstance(rclsid, outer, context, riid, ppv);
}

static void InstallTrayCniHook() {
    HMODULE module = GetModuleHandleW(L"combase.dll");
    void* target = module ? (void*)GetProcAddress(module, "CoCreateInstance") : nullptr;
    if (!target) {
        module = GetModuleHandleW(L"ole32.dll");
        if (!module) module = LoadLibraryW(L"ole32.dll");
        if (module) target = (void*)GetProcAddress(module, "CoCreateInstance");
    }
    if (!target || !Wh_SetFunctionHook(target, (void*)TrayCniCoCreateInstanceHook,
            (void**)&realTrayCniCoCreateInstance)) {
        Wh_Log(L"[T81-CNI] CoCreateInstance hook NOT installed target=%p error=%lu",
               target, GetLastError());
        return;
    }
    Wh_Log(L"[T81-CNI] CoCreateInstance hook installed target=%p", target);
}

#ifdef TASKBAR81_TEST
static void ReorderPrivateTrayKey(const std::wstring& key,const std::wstring& anchor,bool after) {
    if(!s || key.empty() || anchor.empty() || key==anchor || SystemHitForKey(key) || SystemHitForKey(anchor)) return;
    auto& keys=s->trayPinned; auto from=std::find(keys.begin(),keys.end(),key);
    if(from==keys.end() || std::find(keys.begin(),keys.end(),anchor)==keys.end()) return;
    keys.erase(from); auto to=std::find(keys.begin(),keys.end(),anchor);
    if(to==keys.end()) return; if(after) ++to; keys.insert(to,key);
    SaveTrayPreferences(); if(s->bar) InvalidateRect(s->bar,nullptr,FALSE);
}
#endif
static void ReorderTrayBarKey(const std::wstring& key,const std::wstring& anchor,bool after) {
    if(!s || key.empty() || anchor.empty() || key==anchor)return;
    auto entries=VisibleTrayEntries();std::vector<std::wstring> visible;
    for(int entry:entries) {auto k=TrayEntryKey(entry);if(!k.empty() && std::find(visible.begin(),visible.end(),k)==visible.end())visible.push_back(k);}
    auto from=std::find(visible.begin(),visible.end(),key),target=std::find(visible.begin(),visible.end(),anchor);
    if(from==visible.end() || target==visible.end())return;
    auto before=visible;
    visible.erase(from);target=std::find(visible.begin(),visible.end(),anchor);if(after)++target;visible.insert(target,key);
    Wh_Log(L"[T81-DIAG] tray-bar-reorder changed=%d source-system=%d target-system=%d after=%d",before!=visible?1:0,SystemHitForKey(key)?1:0,SystemHitForKey(anchor)?1:0,after?1:0);
    // Preserve keys for temporarily absent icons, as RetroBar does.
    std::vector<std::wstring> merged;size_t next=0;
    for(const auto& old:s->trayBarOrder) {
        if(std::find(visible.begin(),visible.end(),old)!=visible.end()) {if(next<visible.size())merged.push_back(visible[next++]);}
        else if(std::find(merged.begin(),merged.end(),old)==merged.end())merged.push_back(old);
    }
    while(next<visible.size())merged.push_back(visible[next++]);
    s->trayBarOrder=std::move(merged);SaveTrayBarOrder();
    if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);
}
static void ReorderOverflowKey(const std::wstring& key,const std::wstring& anchor,bool after) {
    if(!s || key.empty() || key==anchor)return;
    auto entries=OverflowPrivateItems();std::vector<std::wstring> ordered;
    for(int entry:entries){auto k=TrayEntryKey(entry);if(!k.empty() && std::find(ordered.begin(),ordered.end(),k)==ordered.end())ordered.push_back(std::move(k));}
    auto from=std::find(ordered.begin(),ordered.end(),key);if(from==ordered.end())return;
    ordered.erase(from);auto to=std::find(ordered.begin(),ordered.end(),anchor);
    if(to!=ordered.end() && after)++to;ordered.insert(to,key);
    s->trayOverflowOrder=std::move(ordered);SaveOverflowOrder();
    if(s->overflowPanel)InvalidateRect(s->overflowPanel,nullptr,FALSE);
}
// Unlike the Windows settings page, this menu modifies the private tray's
// own preferences. Checked means visible on the bar; unchecked means overflow.
static void ShowTrayCustomize(HWND owner) {
    if(!s) return;
    w7t::raii::MenuHandle menuGuard(CreatePopupMenu()); HMENU menu=menuGuard.get(); if(!menu) return;
    std::vector<std::wstring> keys;
    for(int hit:{HIT_BATTERY,HIT_NETWORK,HIT_VOLUME}) {
        if(hit==HIT_BATTERY && !s->hasBattery) continue;
        const wchar_t* label=hit==HIT_NETWORK?Tr(L"Rete",L"Network"):hit==HIT_VOLUME?L"Volume":Tr(L"Batteria",L"Battery");
        keys.push_back(SystemKey(hit));
        AppendMenuW(menu,MF_STRING|(IsSystemVisible(hit)?MF_CHECKED:MF_UNCHECKED),(UINT_PTR)keys.size(),label);
    }
    for(const auto& icon:s->trayIcons) {
        if(icon.hidden || !icon.icon || icon.preferenceKey.empty()) continue;
        std::wstring label=icon.tip.empty()?Tr(L"Icona ",L"Icon ")+std::to_wstring(icon.id):icon.tip;
        if(label.size()>90) label.resize(90);
        std::replace(label.begin(),label.end(),L'\n',L' ');
        std::replace(label.begin(),label.end(),L'\r',L' ');
        size_t pos=0; while((pos=label.find(L'&',pos))!=std::wstring::npos) { label.insert(pos,L"&"); pos+=2; }
        bool checked=std::find(s->trayPinned.begin(),s->trayPinned.end(),icon.preferenceKey)!=s->trayPinned.end();
        keys.push_back(icon.preferenceKey);
        AppendMenuW(menu,MF_STRING|(checked?MF_CHECKED:MF_UNCHECKED),(UINT_PTR)keys.size(),label.c_str());
    }
    if(keys.empty()) AppendMenuW(menu,MF_STRING|MF_GRAYED,0,Tr(L"Nessuna icona disponibile",L"No icons available"));
    POINT pt{}; GetCursorPos(&pt);
    SetForegroundWindow(owner);
    UINT selected=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON,pt.x,pt.y,0,owner,nullptr);
    if(selected && selected<=keys.size()) {
        const std::wstring& key=keys[selected-1];
        int system=SystemHitForKey(key);
        bool pinned=system?IsSystemVisible(system):std::find(s->trayPinned.begin(),s->trayPinned.end(),key)!=s->trayPinned.end();
        SetTrayPinned(key,!pinned);
    }
}
static void TickTrayDrag() {
    if(!s || !s->trayDrag.capture)return;
    if(s->trayDrag.hook && !s->trayDrag.endQueued && !(GetAsyncKeyState(VK_LBUTTON)&0x8000)) {
        ULONGLONG now=GetTickCount64();
        if(!s->trayDrag.releasedSince) {
            s->trayDrag.releasedSince=now;
        } else if(now-s->trayDrag.releasedSince>500) {
            CancelTrayDrag();return;
        }
    } else s->trayDrag.releasedSince=0;
    if(!s->trayDrag.active || s->trayDrag.fromOverflow)return;
    POINT p{}; GetCursorPos(&p); RECT r{}; GetClientRect(s->bar,&r); Layout L=GetLayout(r.right,r.bottom);
    RECT arrow=s->verticalActive?GetVerticalLayout(r.right,r.bottom).arrow:RECT{L.tray.left,0,L.tray.left+OverflowButtonWidth(),r.bottom};
    MapWindowPoints(s->bar,nullptr,(POINT*)&arrow,2);
    if(PtInRect(&arrow,p)) {
        if(!s->trayDrag.arrowSince) s->trayDrag.arrowSince=GetTickCount64();
        else if(GetTickCount64()-s->trayDrag.arrowSince>=500 && (!s->overflowPanel || !IsWindowVisible(s->overflowPanel))) ShowPrivateOverflow();
    } else s->trayDrag.arrowSince=0;
}
static void EndTrayDrag(HWND capture,POINT client) {
    if(!s || s->trayDrag.capture!=capture) return;
    POINT p=client; ClientToScreen(capture,&p);
    bool active=s->trayDrag.active, fromOverflow=s->trayDrag.fromOverflow;
    bool awaitingNativeUp=s->trayDrag.endQueued && !s->trayDrag.nativeUpSeen;
    std::wstring key=s->trayDrag.key; int index=FindTrayKey(key);
    bool clickSame=false;
    int system=SystemHitForEntry(index);
    if(!active && index!=-1) {
        POINT q=p; ScreenToClient(capture,&q);
        if(fromOverflow) clickSame=OverflowHit(q)==index;
        else { RECT r=system?SystemCell(system):PrivateIconRect(index); clickSame=PtInRect(&r,q)!=FALSE; }
    }
    RECT panel{}, bar{}; bool inPanel=s->overflowPanel && IsWindowVisible(s->overflowPanel) && GetWindowRect(s->overflowPanel,&panel) && PtInRect(&panel,p);
    GetClientRect(s->bar,&bar); Layout layout=GetLayout(bar.right,bar.bottom);
    VerticalLayout vl=s->verticalActive?GetVerticalLayout(bar.right,bar.bottom):VerticalLayout{};
    RECT tray=s->verticalActive?RECT{0,vl.arrow.top,bar.right,vl.clock.top}:layout.tray;MapWindowPoints(s->bar,nullptr,(POINT*)&tray,2);
    RECT arrow=s->verticalActive?vl.arrow:RECT{layout.tray.left,0,layout.tray.left+OverflowButtonWidth(),bar.bottom};MapWindowPoints(s->bar,nullptr,(POINT*)&arrow,2);
    bool inTray=PtInRect(&tray,p)!=FALSE, inArrow=PtInRect(&arrow,p)!=FALSE;
    std::wstring anchor; bool after=false;
    if(inTray && !inArrow) {
        POINT q=p; ScreenToClient(s->bar,&q); auto shown=VisibleTrayEntries();
        for(int visible:shown) { RECT cell=TrayBarEntryRect(visible); if(PtInRect(&cell,q)) {
            anchor=TrayEntryKey(visible);
            after=s->verticalActive?q.y>=(cell.top+cell.bottom)/2:q.x>=(cell.left+cell.right)/2; break;
        } }
    }
    std::wstring panelAnchor;bool panelAfter=false;
    if(inPanel) {POINT q=p;ScreenToClient(s->overflowPanel,&q);int item=OverflowHit(q);panelAnchor=TrayEntryKey(item);
        auto entries=OverflowPrivateItems();auto it=std::find(entries.begin(),entries.end(),item);
        if(it!=entries.end()){RECT c=OverflowCell((int)(it-entries.begin()));panelAfter=q.x>=(c.left+c.right)/2;}}
    // The panel can move when promotion changes the tray width. Its outside
    // click guard must not mistake this gesture's still-in-flight button-up
    // for a fresh click outside the newly positioned panel.
    if(active && s->overflowPanel && IsWindowVisible(s->overflowPanel))s->overflowMouseMask|=1;
    if(awaitingNativeUp){s->suppressTrayRelease=true;s->suppressTrayReleaseUntil=GetTickCount64()+500;}
    CancelTrayDrag();
    index=FindTrayKey(key);
    if(index==-1) return;
    if(active) {
        Wh_Log(L"[T81-DIAG] tray-drop from-overflow=%d in-tray=%d in-arrow=%d in-panel=%d",fromOverflow?1:0,inTray?1:0,inArrow?1:0,inPanel?1:0);
        if(fromOverflow && inTray) { SetTrayPinned(key,true); if(!inArrow)ReorderTrayBarKey(key,anchor,after); }
        else if(!fromOverflow && inPanel) {SetTrayPinned(key,false);ReorderOverflowKey(key,panelAnchor,panelAfter);}
        else if(!fromOverflow && inArrow) SetTrayPinned(key,false);
        else if(fromOverflow && inPanel) ReorderOverflowKey(key,panelAnchor,panelAfter);
        else if(!fromOverflow && inTray) ReorderTrayBarKey(key,anchor,after);
    } else if(clickSame) {
        if(system) {
            // Carry the actual button-up screen point while overflow is visible.
            LaunchSystemTrayHit(system,&p);
        } else {
            SendPrivateIconEvent(index,WM_LBUTTONDOWN,p);
            SendPrivateIconEvent(index,WM_LBUTTONUP,p);
            if(s->trayIcons[index].version>=3) SendPrivateIconEvent(index,NIN_SELECT,p);
        }
        // Keep the overflow and its anchor visible while a flyout opens.
        // Outside-click dismissal, explicit toggle and drag behavior are unchanged.
    }
}
static void DestroyPrivateOverflow() {
    if(!s) return;
    if(s->overflowTooltip && IsWindow(s->overflowTooltip)) DestroyWindow(s->overflowTooltip);
    s->overflowTooltip=nullptr;
    if(s->overflowPanel && IsWindow(s->overflowPanel)) DestroyWindow(s->overflowPanel);
    s->overflowPanel=nullptr;
    if(s->overflowClass) UnregisterClassW((LPCWSTR)(ULONG_PTR)(WORD)s->overflowClass,GetModuleHandleW(nullptr));
    s->overflowClass=0;
    delete s->overflowHoverImage; s->overflowHoverImage=nullptr; s->overflowImageTried=false;
    if(s->overflowHoverStream) {s->overflowHoverStream->Release();s->overflowHoverStream=nullptr;}
}

static void HandlePrivateIconEvent(int hit,UINT message) {
    if(hit>HIT_TRAY_ICON_BASE) return;
    int index=HIT_TRAY_ICON_BASE-hit; POINT p{}; GetCursorPos(&p);
    if(message==WM_RBUTTONUP && index>=0 && (size_t)index<s->trayIcons.size()) SetForegroundWindow(s->trayIcons[index].owner);
    SendPrivateIconEvent(index,message,p);
    if(index>=0 && (size_t)index<s->trayIcons.size() && s->trayIcons[index].version>=3) {
        if(message==WM_LBUTTONUP) SendPrivateIconEvent(index,NIN_SELECT,p);
        if(message==WM_RBUTTONUP) SendPrivateIconEvent(index,WM_CONTEXTMENU,p);
    }
}
static void ShowSystemTrayContextMenu(int hit,POINT p) {
    const wchar_t* lines=hit==HIT_VOLUME?
        Tr(L"Apri mixer volume\nDispositivi di riproduzione\nDispositivi di registrazione\nSuoni",L"Open volume mixer\nPlayback devices\nRecording devices\nSounds"):
        hit==HIT_NETWORK?Tr(L"Risoluzione dei problemi\nApri Centro connessioni di rete e condivisione",L"Troubleshoot problems\nOpen Network and Sharing Center"):
        Tr(L"Opzioni risparmio energia\nCentro PC portatile Windows",L"Power Options\nWindows Mobility Center");
    int cmd=w7t::ShellMenu::ShowContextMenuEx(p.x,p.y,true,lines,true);
    if(hit==HIT_VOLUME) {
        if(cmd==1) { wchar_t dir[MAX_PATH]={};
             if(GetSystemDirectoryW(dir,MAX_PATH)) { std::wstring exe=std::wstring(dir)+L"\\SndVol.exe";
             ShellExecuteW(s->bar,L"open",exe.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
             } }
        else if(cmd>=2 && cmd<=4) { const wchar_t* tabs[]={L"mmsys.cpl,,0",L"mmsys.cpl,,1",L"mmsys.cpl,,2"}; ShellExecuteW(s->bar,L"open",L"control.exe",tabs[cmd-2],nullptr,SW_SHOWNORMAL); }
    } else if(hit==HIT_NETWORK) {
        if(cmd==1) ShellExecuteW(s->bar,L"open",L"ms-settings:troubleshoot",nullptr,nullptr,SW_SHOWNORMAL);
        if(cmd==2) ShellExecuteW(s->bar,L"open",L"control.exe",L"/name Microsoft.NetworkAndSharingCenter",nullptr,SW_SHOWNORMAL);
    } else if(hit==HIT_BATTERY) {
        if(cmd==1) ShellExecuteW(s->bar,L"open",L"control.exe",L"powercfg.cpl",nullptr,SW_SHOWNORMAL);
        if(cmd==2) ShellExecuteW(s->bar,L"open",L"mblctr.exe",nullptr,nullptr,SW_SHOWNORMAL);
    }
}
static void LogTaskbarDiagnostics(const wchar_t* reason);
void HidePreview();
// Folder-backed Desktop/Links bands (GPL-3.0-or-later), not a COM deskband:
// the
// Links exposes local filesystem items; Desktop has its own namespace popup.
struct BandEntry { std::wstring name,path; bool directory=false; };
static void CollectBandFolder(int csidl,std::vector<BandEntry>& out) {
    wchar_t dir[MAX_PATH]={}; if(FAILED(SHGetFolderPathW(nullptr,csidl,nullptr,SHGFP_TYPE_CURRENT,dir)) || !dir[0]) return;
    std::wstring prefix=std::wstring(dir)+L"\\"; WIN32_FIND_DATAW fd{};
    HANDLE find=FindFirstFileW((prefix+L"*").c_str(),&fd); if(find==INVALID_HANDLE_VALUE) return; ScopedFind findGuard(find);
    do {
        if(out.size()>=60) break;
        if(!wcscmp(fd.cFileName,L".") || !wcscmp(fd.cFileName,L"..") || ((fd.dwFileAttributes&(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))) continue;
        std::wstring path=prefix+fd.cFileName; SHFILEINFOW info{};
        SHGetFileInfoW(path.c_str(),0,&info,sizeof(info),SHGFI_DISPLAYNAME);
        std::wstring label=info.szDisplayName[0]?info.szDisplayName:fd.cFileName;
        if(label.size()>100) label.resize(100);
        for(auto& ch:label) if(ch<L' ') ch=L' ';
        if(std::none_of(out.begin(),out.end(),[&](const BandEntry& e){return !_wcsicmp(e.path.c_str(),path.c_str());})) out.push_back({label,path,(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0});
    } while(FindNextFileW(find,&fd));
}
// A shell namespace view rather than the old filesystem-only Desktop menu.
// PIDLs returned relative to the desktop root are absolute from that root.
// Links keeps its previous filesystem implementation. This is not a native deskband.
static const wchar_t kDesktopPopupClass[]=L"WH_Taskbar81DesktopPopup";
struct DesktopItem { std::wstring name; PIDLIST_RELATIVE pidl=nullptr; int image=0; };
struct DesktopPopupData {
    std::vector<DesktopItem> items; HWND list=nullptr; bool windowOwns=false;
    ~DesktopPopupData(){for(auto& item:items) CoTaskMemFree(item.pidl);}
};
static void CloseDesktopPopup(){if(!s) return; HWND h=s->desktopPopup;s->desktopPopup=nullptr;if(h && IsWindow(h)) DestroyWindow(h);}
static void DesktopOpenItem(HWND h,DesktopPopupData* d) {
    if(!d || !d->list) return; int row=ListView_GetNextItem(d->list,-1,LVNI_SELECTED); if(row<0 || (size_t)row>=d->items.size()) return;
    // Copy before destruction; ShellExecuteEx may pump messages.
    PIDLIST_ABSOLUTE absolute=ILClone(d->items[row].pidl); if(!absolute) return;
    DestroyWindow(h);
    SHELLEXECUTEINFOW e{};e.cbSize=sizeof(e);e.fMask=SEE_MASK_IDLIST;e.hwnd=s?s->bar:nullptr;e.lpVerb=L"open";e.lpIDList=absolute;e.nShow=SW_SHOWNORMAL;
    if(!ShellExecuteExW(&e)) Wh_Log(L"[T81-DIAG] Desktop item open failed=%lu",GetLastError());
    CoTaskMemFree(absolute);
}
static LRESULT CALLBACK DesktopPopupProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    auto* d=(DesktopPopupData*)GetWindowLongPtrW(h,GWLP_USERDATA);
    if(msg==WM_NCCREATE) {d=(DesktopPopupData*)((CREATESTRUCTW*)lp)->lpCreateParams;SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)d);}
    if(!d) return DefWindowProcW(h,msg,wp,lp);
    try { switch(msg) {
    case WM_CREATE: {
        d->list=CreateWindowExW(0,WC_LISTVIEWW,L"",WS_CHILD|WS_VISIBLE|WS_TABSTOP|LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_SINGLESEL|LVS_SHOWSELALWAYS|WS_VSCROLL,0,0,10,10,h,(HMENU)1,GetModuleHandleW(nullptr),nullptr);
        if(!d->list) return -1;
        ListView_SetExtendedListViewStyle(d->list,LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER);
        SHFILEINFOW icon{}; HIMAGELIST sys=(HIMAGELIST)SHGetFileInfoW(L"C:\\",0,&icon,sizeof(icon),SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
        if(sys) ListView_SetImageList(d->list,sys,LVSIL_SMALL);
        LVCOLUMNW col{};col.mask=LVCF_WIDTH;col.cx=Px(260);ListView_InsertColumn(d->list,0,&col);
        SendMessageW(d->list,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);
        for(size_t i=0;i<d->items.size();++i) {LVITEMW it{};
            it.mask=LVIF_TEXT|LVIF_IMAGE;
            it.iItem=(int)i;
            it.pszText=(wchar_t*)d->items[i].name.c_str();
            it.iImage=d->items[i].image;
            ListView_InsertItem(d->list,&it);
            }
        SetTimer(h,1,100,nullptr); return 0;
    }
    case WM_SIZE: {int w=LOWORD(lp),hh=HIWORD(lp),pad=Px(6);
         if(d->list) {MoveWindow(d->list,pad,pad,std::max(1,w-2*pad),std::max(1,hh-2*pad),TRUE);
         ListView_SetColumnWidth(d->list,0,std::max(1,w-2*pad-Px(20)));
        } return 0;
        }
    case WM_PAINT: {PAINTSTRUCT ps{};
        HDC dc=BeginPaint(h,&ps);
        RECT r{};
        GetClientRect(h,&r);
        HBRUSH br=CreateSolidBrush(PopupChromeColor());
        FillRect(dc,&r,br);
        DeleteObject(br);
        EndPaint(h,&ps);
        return 0;
        }
    case WM_NOTIFY: {auto* nm=(NMHDR*)lp;
         if(nm && nm->hwndFrom==d->list) {if(nm->code==NM_DBLCLK) {DesktopOpenItem(h,d);
        return 0;
        } if(nm->code==LVN_KEYDOWN && ((NMLVKEYDOWN*)lp)->wVKey==VK_RETURN) {DesktopOpenItem(h,d);
        return 0;
        } if(nm->code==LVN_KEYDOWN && ((NMLVKEYDOWN*)lp)->wVKey==VK_ESCAPE) {DestroyWindow(h);
        return 0;
        }} break;
        }
    case WM_KEYDOWN: if(wp==VK_ESCAPE) {DestroyWindow(h);return 0;} break;
    case WM_ACTIVATE: if(LOWORD(wp)==WA_INACTIVE && IsWindowVisible(h)) {DestroyWindow(h);return 0;} break;
    case WM_TIMER: if(wp==1) {if(GetAsyncKeyState(VK_LBUTTON)&0x8000 || GetAsyncKeyState(VK_RBUTTON)&0x8000) {POINT p{};
        RECT r{};
        GetCursorPos(&p);
        GetWindowRect(h,&r);
        if(!PtInRect(&r,p)) {DestroyWindow(h);
        return 0;
        }} return 0;
        }break;
    case WM_NCDESTROY: KillTimer(h,1);
        if(s && s->desktopPopup==h) s->desktopPopup=nullptr;
        SetWindowLongPtrW(h,GWLP_USERDATA,0);
        if(d->windowOwns) delete d;
        return DefWindowProcW(h,msg,wp,lp);
    }
    return DefWindowProcW(h,msg,wp,lp);
    } catch(const std::bad_alloc&) {Wh_Log(L"[T81-DIAG] Desktop callback allocation failed");if(msg==WM_CREATE) return -1;if(msg!=WM_NCDESTROY) DestroyWindow(h);return 0;}
      catch(...) {Wh_Log(L"[T81-DIAG] Desktop callback C++ exception");if(msg==WM_CREATE) return -1;if(msg!=WM_NCDESTROY) DestroyWindow(h);return 0;}
}
static void DestroyDesktopPopup() {CloseDesktopPopup();
    if(s && s->desktopPopupClass) {UnregisterClassW(kDesktopPopupClass,GetModuleHandleW(nullptr));
    s->desktopPopupClass=false;
    }}
static void ShowDesktopBandPopup() {
    if(!s || !s->bar) return;
    if(s->desktopPopup && IsWindow(s->desktopPopup)) {CloseDesktopPopup();return;}
    Wh_Log(L"[T81-DIAG] Desktop namespace enumeration begin");
    auto d=std::make_unique<DesktopPopupData>(); IShellFolder* folderRaw=nullptr;
    HRESULT hr=SHGetDesktopFolder(&folderRaw);
         std::unique_ptr<IShellFolder,ReleaseCom<IShellFolder>> folder(folderRaw);
         if(FAILED(hr) || !folder) {Wh_Log(L"[T81-DIAG] Desktop namespace failed hr=%08lx",(unsigned long)hr);
        return;
        }
    IEnumIDList* enumRaw=nullptr; hr=folder->EnumObjects(s->bar,(SHCONTF)(SHCONTF_FOLDERS|SHCONTF_NONFOLDERS),&enumRaw);
    std::unique_ptr<IEnumIDList,ReleaseCom<IEnumIDList>> e(enumRaw);
    if(SUCCEEDED(hr) && e) {
        PIDLIST_RELATIVE pidl=nullptr; ULONG fetched=0;
        while(d->items.size()<4096 && e->Next(1,&pidl,&fetched)==S_OK && fetched==1 && pidl) {
            std::unique_ptr<ITEMIDLIST,FreePidl> itemGuard(pidl);
            STRRET str{};wchar_t name[MAX_PATH]={};
            if(SUCCEEDED(folder->GetDisplayNameOf(pidl,SHGDN_NORMAL,&str)) && SUCCEEDED(StrRetToBufW(&str,pidl,name,MAX_PATH)) && name[0]) {
                SHFILEINFOW info{}; SHGetFileInfoW((LPCWSTR)pidl,0,&info,sizeof(info),SHGFI_PIDL|SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
                d->items.push_back({name,pidl,info.iIcon});[[maybe_unused]] auto transferred_itemGuard=itemGuard.release();
            }
            pidl=nullptr;
        }
    }
    e.reset(); folder.reset();
    std::sort(d->items.begin(),d->items.end(),[](const DesktopItem& a,const DesktopItem& b){return _wcsicmp(a.name.c_str(),b.name.c_str())<0;});
    Wh_Log(L"[T81-DIAG] Desktop namespace enumeration hr=%08lx items=%u",(unsigned long)hr,(unsigned)d->items.size());
    if(FAILED(hr)) return;
    INITCOMMONCONTROLSEX controls{sizeof(controls),ICC_LISTVIEW_CLASSES}; InitCommonControlsEx(&controls);
    WNDCLASSW wc{};
        wc.hInstance=GetModuleHandleW(nullptr);
        wc.lpfnWndProc=DesktopPopupProc;
        wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        wc.lpszClassName=kDesktopPopupClass;
    if(!s->desktopPopupClass) {if(!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) return;
        WNDCLASSW existing{};
        if(!GetClassInfoW(wc.hInstance,kDesktopPopupClass,&existing) || existing.lpfnWndProc!=DesktopPopupProc)return;
        s->desktopPopupClass=true;
        }
    RECT client{},bar{};GetClientRect(s->bar,&client);GetWindowRect(s->bar,&bar);Layout layout=GetLayout(client.right,client.bottom);
    MONITORINFO mi{};mi.cbSize=sizeof(mi);if(!GetMonitorInfoW(MonitorFromWindow(s->bar,MONITOR_DEFAULTTONEAREST),&mi)) return;
    int width=std::min(Px(290),(int)(mi.rcWork.right-mi.rcWork.left));
    int height=std::min(Px(400),std::max(Px(100),(int)d->items.size()*Px(22)+Px(12)));
    height=std::min(height,(int)(mi.rcWork.bottom-mi.rcWork.top));
    int x=std::max((int)mi.rcWork.left,std::min((int)(bar.left+layout.desktopBand.left),(int)mi.rcWork.right-width));
    int y=bar.top-height;if(y<mi.rcWork.top)y=std::min((int)bar.bottom,(int)mi.rcWork.bottom-height);
    if(s->verticalActive) {
        VerticalLayout v=GetVerticalLayout(client.right,client.bottom);
        x=s->verticalSide==1?bar.right:bar.left-width;
        x=std::max((int)mi.rcWork.left,std::min(x,(int)mi.rcWork.right-width));
        y=std::max((int)mi.rcWork.top,std::min((int)(bar.top+v.desktopBand.top),(int)mi.rcWork.bottom-height));
    }
    DesktopPopupData* raw=d.get(); HWND popup=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,kDesktopPopupClass,L"Desktop",WS_POPUP|WS_BORDER,x,y,width,height,s->bar,nullptr,wc.hInstance,raw);
    if(!popup) {Wh_Log(L"[T81-DIAG] Desktop popup create failed=%lu",GetLastError());return;}
    raw->windowOwns=true;[[maybe_unused]] auto transferred_d=d.release();s->desktopPopup=popup;ExcludeFromPeek(popup);
    SetWindowPos(popup,HWND_TOPMOST,x,y,width,height,SWP_SHOWWINDOW);SetForegroundWindow(popup);if(raw->list) SetFocus(raw->list);
}
static void ShowBandPopup(int which) {
    if(which==HIT_BAND_DESKTOP) {ShowDesktopBandPopup();return;}
    if(!s || !s->bar) return;
    std::vector<BandEntry> items;
    if(which==HIT_BAND_DESKTOP) { CollectBandFolder(CSIDL_DESKTOPDIRECTORY,items); CollectBandFolder(CSIDL_COMMON_DESKTOPDIRECTORY,items); }
    else {
        wchar_t dir[MAX_PATH]={};
        // Links is a known folder; use Favorites\Links fallback on older systems.
        PWSTR path=nullptr;
        if(SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Links,0,nullptr,&path)) && path) {
            std::unique_ptr<wchar_t,FreeCoTask> pathGuard(path);
            std::wstring prefix=std::wstring(path)+L"\\"; WIN32_FIND_DATAW fd{};
            HANDLE f=FindFirstFileW((prefix+L"*").c_str(),&fd);
            if(f!=INVALID_HANDLE_VALUE) { ScopedFind findGuard(f);
                 do { if(items.size()>=60) break;
                 if(fd.cFileName[0]==L'.' || ((fd.dwFileAttributes&(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))) continue;
                SHFILEINFOW info{}; std::wstring full=prefix+fd.cFileName; SHGetFileInfoW(full.c_str(),0,&info,sizeof(info),SHGFI_DISPLAYNAME);
                items.push_back({info.szDisplayName[0]?info.szDisplayName:fd.cFileName,full,(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0}); } while(FindNextFileW(f,&fd)); }
        } else { if(path) CoTaskMemFree(path); (void)dir; }
    }
    auto bandLess=[](const BandEntry& a,const BandEntry& b) { return a.directory!=b.directory?a.directory:_wcsicmp(a.name.c_str(),b.name.c_str())<0; };
    std::sort(items.begin(),items.end(),bandLess);
    struct BitmapList {std::vector<HBITMAP> values; ~BitmapList(){for(auto b:values) if(b) DeleteObject(b);}} bitmaps; std::vector<std::wstring> targets;
    w7t::raii::MenuHandle menuGuard(CreatePopupMenu()); HMENU menu=menuGuard.get(); if(!menu) return;
    auto addBitmap=[&](HMENU m,UINT pos,const std::wstring& path) {
        SHFILEINFOW info{}; if(!SHGetFileInfoW(path.c_str(),0,&info,sizeof(info),SHGFI_ICON|SHGFI_SMALLICON) || !info.hIcon) return;
        std::unique_ptr<std::remove_pointer<HICON>::type,DestroyIconHandle> iconGuard(info.hIcon);
        int size=Px(16);
             BITMAPINFO bi{};
             bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
             bi.bmiHeader.biWidth=size;
             bi.bmiHeader.biHeight=-size;
             bi.bmiHeader.biPlanes=1;
             bi.bmiHeader.biBitCount=32;
             bi.bmiHeader.biCompression=BI_RGB;
        std::unique_ptr<std::remove_pointer<HDC>::type,DeleteMemDC> dc(CreateCompatibleDC(nullptr)); void* pixels=nullptr;
        std::unique_ptr<std::remove_pointer<HBITMAP>::type,DeleteBitmapHandle> bmp(dc?CreateDIBSection(dc.get(),&bi,DIB_RGB_COLORS,&pixels,nullptr,0):nullptr);
        if(!bmp || !pixels) return;
        memset(pixels,0,(size_t)size*size*4);
             HGDIOBJ old=SelectObject(dc.get(),bmp.get());
             DrawIconEx(dc.get(),0,0,info.hIcon,size,size,0,nullptr,DI_NORMAL);
             if(old) SelectObject(dc.get(),old);
        bitmaps.values.push_back(bmp.get()); [[maybe_unused]] auto transferred_bmp=bmp.release();
        MENUITEMINFOW mi{}; mi.cbSize=sizeof(mi); mi.fMask=MIIM_BITMAP; mi.hbmpItem=bitmaps.values.back();
        SetMenuItemInfoW(m,pos,TRUE,&mi);
    };
    std::function<void(HMENU,const std::vector<BandEntry>&,int)> appendEntries;
    appendEntries=[&](HMENU m,const std::vector<BandEntry>& entries,int depth) {
        for(const auto& item:entries) {
            if(targets.size()>=200) break;
            std::wstring text=item.name; size_t at=0; while((at=text.find(L'&',at))!=std::wstring::npos) {text.insert(at,L"&");at+=2;}
            DWORD attrs=GetFileAttributesW(item.path.c_str());
            bool folder=attrs!=INVALID_FILE_ATTRIBUTES && (attrs&FILE_ATTRIBUTE_DIRECTORY) && !(attrs&FILE_ATTRIBUTE_REPARSE_POINT) && depth<3;
            w7t::raii::MenuHandle subGuard(folder?CreatePopupMenu():nullptr); HMENU sub=subGuard.get();
            if(sub) {
                targets.push_back(item.path); UINT open=(UINT)targets.size(); AppendMenuW(sub,MF_STRING,open,Tr(L"Apri cartella",L"Open folder")); AppendMenuW(sub,MF_SEPARATOR,0,nullptr);
                std::vector<BandEntry> children; WIN32_FIND_DATAW fd{}; HANDLE f=FindFirstFileW((item.path+L"\\*").c_str(),&fd);
                if(f!=INVALID_HANDLE_VALUE) { ScopedFind findGuard(f);
                     do { if(children.size()>=60) break;
                     if(!wcscmp(fd.cFileName,L".")||!wcscmp(fd.cFileName,L"..")||((fd.dwFileAttributes&(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))==(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM))) continue;
                    std::wstring path=item.path+L"\\"+fd.cFileName; SHFILEINFOW info{}; SHGetFileInfoW(path.c_str(),0,&info,sizeof(info),SHGFI_DISPLAYNAME);
                    std::wstring label=info.szDisplayName[0]?info.szDisplayName:fd.cFileName;
                         if(label.size()>100) label.resize(100);
                         for(auto& c:label) if(c<L' ') c=L' ';
                         children.push_back({label,path,(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0});
                } while(FindNextFileW(f,&fd)); }
                std::sort(children.begin(),children.end(),bandLess);
                appendEntries(sub,children,depth+1);
                if(children.empty()) AppendMenuW(sub,MF_STRING|MF_GRAYED,0,Tr(L"Nessun elemento",L"No items"));
                if(AppendMenuW(m,MF_STRING|MF_POPUP,(UINT_PTR)sub,text.c_str())) { [[maybe_unused]] auto transferred_subGuard=subGuard.release(); }
            } else { targets.push_back(item.path); AppendMenuW(m,MF_STRING,(UINT_PTR)targets.size(),text.c_str()); }
            int count=GetMenuItemCount(m); if(count>0) addBitmap(m,(UINT)(count-1),item.path);
        }
    };
    appendEntries(menu,items,0);
    if(items.empty()) AppendMenuW(menu,MF_STRING|MF_GRAYED,0,Tr(L"Nessun elemento",L"No items"));
    POINT pt{}; GetCursorPos(&pt); HWND owner=w7t::GetMenuOwnerWindow();
    UINT cmd=0;
         if(owner) { w7t::ForegroundMenuScope scope(owner);
         cmd=TrackPopupMenuEx(menu,TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON,pt.x,pt.y,owner,nullptr);
         }
    menuGuard.reset();
    if(cmd && cmd<=targets.size()) ShellExecuteW(s->bar,L"open",targets[cmd-1].c_str(),nullptr,nullptr,SW_SHOWNORMAL);
}
static void SaveBands() { BuildGroups(); Wh_SetStringValue(L"Taskbar81.Bands",(std::wstring(s->linksBand?L"1":L"0")+(s->desktopBand?L"1":L"0")+(s->addressBand?L"1":L"0")).c_str()); }
static void LoadBands() { wchar_t value[8]={};
     size_t n=Wh_GetStringValue(L"Taskbar81.Bands",value,8);
     if(n>=2) { s->linksBand=value[0]==L'1';
     s->desktopBand=value[1]==L'1';
     s->addressBand=n>=3 && value[2]==L'1';
     }
    wchar_t lock[4]={};if(Wh_GetStringValue(L"Taskbar81.BandsLocked",lock,4)==1)s->bandsLocked=lock[0]!=L'0';
    wchar_t edge[4]={};if(Wh_GetStringValue(L"Taskbar81.PlacementEdge",edge,4)==1 && (edge[0]==L'1' || edge[0]==L'2' || (taskbarPlacementMode.load()==3 && (edge[0]==L'3' || edge[0]==L'4'))))s->barEdgeOverride=edge[0]-L'0';
    wchar_t order[8]={};if(Wh_GetStringValue(L"Taskbar81.BandOrder",order,8)==3 && order[0]>=L'0' && order[0]<=L'2' && order[1]>=L'0' && order[1]<=L'2' && order[2]>=L'0' && order[2]<=L'2' && order[0]!=order[1] && order[0]!=order[2] && order[1]!=order[2]) for(int i=0;i<3;i++)s->bandOrder[i]=order[i]-L'0';
}
// Address toolbar: simplified native EDIT (GPL-3.0-or-later),
// not an Explorer deskband.
static const wchar_t kAddressClass[]=L"WH_Taskbar81Address";
static std::wstring NormalizeAddressTarget(std::wstring text) {
    const wchar_t* ws=L" \t\r\n"; size_t first=text.find_first_not_of(ws); if(first==std::wstring::npos) return {};
    text=text.substr(first,text.find_last_not_of(ws)-first+1);
    DWORD n=ExpandEnvironmentStringsW(text.c_str(),nullptr,0);
    if(n && n<=32768) { std::vector<wchar_t> b(n); if(ExpandEnvironmentStringsW(text.c_str(),b.data(),n)) text=b.data(); }
    // Bare hostname -> HTTPS. Do not log the input.
    if(text.find(L"://")==std::wstring::npos && text.find(L'\\')==std::wstring::npos &&
       text.find(L' ')==std::wstring::npos && text.find(L'.')!=std::wstring::npos &&
       GetFileAttributesW(text.c_str())==INVALID_FILE_ATTRIBUTES && text.find(L':')==std::wstring::npos)
        text=L"https://"+text;
    return text;
}
static LRESULT CALLBACK AddressEditSubclass(HWND edit,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR,DWORD_PTR) {
    if(msg==WM_KEYDOWN && wp==VK_RETURN) {
        try {
        int n=GetWindowTextLengthW(edit); if(n>0 && n<=32767) {
            std::vector<wchar_t> b((size_t)n+1); GetWindowTextW(edit,b.data(),n+1);
            std::wstring target=NormalizeAddressTarget(b.data());
            if(!target.empty()) {
                HINSTANCE result=ShellExecuteW(edit,L"open",target.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
                if((INT_PTR)result<=32) { MessageBeep(MB_ICONEXCLAMATION); Wh_Log(L"[T81-DIAG] address launch failed code=%lld",(long long)(INT_PTR)result); }
            }
        }
        } catch(...) { Wh_Log(L"[T81-DIAG] address processing failed"); MessageBeep(MB_ICONEXCLAMATION); }
        return 0;
    }
    if(msg==WM_KEYDOWN && wp==VK_ESCAPE) {SetFocus(nullptr); return 0;}
    if(msg==WM_NCDESTROY) RemoveWindowSubclass(edit,AddressEditSubclass,1);
    return DefSubclassProc(edit,msg,wp,lp);
}
static LRESULT AddressHostProcImpl(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    if(!s) return DefWindowProcW(h,msg,wp,lp);
    switch(msg) {
    case WM_CREATE: {
        s->addressEdit=CreateWindowExW(WS_EX_CLIENTEDGE,L"EDIT",L"",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,
            0,0,10,10,h,(HMENU)1,GetModuleHandleW(nullptr),nullptr);
        if(!s->addressEdit) return -1;
        SendMessageW(s->addressEdit,EM_SETLIMITTEXT,32767,0);
        s->addressFontDpi=Dpi();
        s->addressFont=CreateFontW(-MulDiv(9,s->addressFontDpi,72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        if(s->addressFont) SendMessageW(s->addressEdit,WM_SETFONT,(WPARAM)s->addressFont,TRUE);
        if(!SetWindowSubclass(s->addressEdit,AddressEditSubclass,1,0)) { DestroyWindow(s->addressEdit);
             s->addressEdit=nullptr;
             if(s->addressFont) {DeleteObject(s->addressFont);
            s->addressFont=nullptr;
            } return -1;
             }
        return 0;
    }
    case WM_SIZE: if(s->addressEdit) MoveWindow(s->addressEdit,0,0,LOWORD(lp),HIWORD(lp),TRUE); return 0;
    case WM_NCDESTROY: s->addressEdit=nullptr;
         if(s->addressHost==h) s->addressHost=nullptr;
         if(s->addressFont) {DeleteObject(s->addressFont);
        s->addressFont=nullptr;
        } break;
    }
    return DefWindowProcW(h,msg,wp,lp);
}
static LRESULT CALLBACK AddressHostProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    try {return AddressHostProcImpl(h,msg,wp,lp);}
    catch(...) {Wh_Log(L"[T81-RECOVERY] AddressHostProc C++ exception");return msg==WM_CREATE?-1:0;}
}

static void DestroyAddressBand() {
    if(!s) return;
    if(s->addressHost && IsWindow(s->addressHost)) DestroyWindow(s->addressHost);
    s->addressHost=nullptr; s->addressEdit=nullptr;
    if(s->addressFont) {DeleteObject(s->addressFont);s->addressFont=nullptr;}
    if(s->addressClass) {UnregisterClassW(kAddressClass,GetModuleHandleW(nullptr));s->addressClass=false;}
}
static void UpdateAddressBand() {
    if(!s || !s->bar) return;
    RECT cr{}; GetClientRect(s->bar,&cr); Layout l=GetLayout(cr.right,cr.bottom);
    VerticalLayout v=s->verticalActive?GetVerticalLayout(cr.right,cr.bottom):VerticalLayout{};
    RECT area=s->verticalActive?v.addressBand:l.addressBand;
    if(!s->addressBand || area.right<=area.left || area.bottom<=area.top || !IsWindowVisible(s->bar)) {
        if(s->addressHost && IsWindowVisible(s->addressHost)) ShowWindow(s->addressHost,SW_HIDE);
        return;
    }
    if(!s->addressClass) {
        WNDCLASSW wc{};
             wc.lpfnWndProc=AddressHostProc;
             wc.hInstance=GetModuleHandleW(nullptr);
             wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
             wc.lpszClassName=kAddressClass;
        if(!RegisterClassW(&wc)) { Wh_Log(L"[T81-DIAG] address window registration failed=%lu",GetLastError()); return; }
        s->addressClass=true;
    }
    if(!s->addressHost) {
        HWND host=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,kAddressClass,L"",WS_POPUP,
            0,0,10,10,s->bar,nullptr,GetModuleHandleW(nullptr),nullptr);
        if(!host) {Wh_Log(L"[T81-DIAG] address window create failed=%lu",GetLastError());return;}
        s->addressHost=host; ExcludeFromPeek(host);
    }
    if(s->addressEdit && s->addressFontDpi!=Dpi()) {
        int dpi=Dpi(); HFONT font=CreateFontW(-MulDiv(9,dpi,72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        if(font) {SendMessageW(s->addressEdit,WM_SETFONT,(WPARAM)font,TRUE);
             if(s->addressFont) DeleteObject(s->addressFont);
             s->addressFont=font;
             s->addressFontDpi=dpi;
            }
    }
    RECT r=s->verticalActive ? RECT{area.left+Px(2),area.top+Px(1),area.right-Px(2),area.bottom-Px(1)} :
        RECT{l.addressBand.left+Px(66),std::max(0,(int)(cr.bottom-Px(24))/2),l.addressBand.right-Px(3),std::min((int)cr.bottom,(int)(cr.bottom+Px(24))/2)};
    MapWindowPoints(s->bar,nullptr,(POINT*)&r,2); RECT old{}; GetWindowRect(s->addressHost,&old);
    if(!EqualRect(&old,&r) || !IsWindowVisible(s->addressHost))
        SetWindowPos(s->addressHost,HWND_TOPMOST,r.left,r.top,r.right-r.left,r.bottom-r.top,SWP_NOACTIVATE|SWP_SHOWWINDOW);
}

static std::wstring BuildBarMenuText(bool clock) {
    std::wstring lines;
    lines+=Tr(L">Barre degli strumenti\n",L">Toolbars\n");
    lines+=s->addressBand?Tr(L"*Indirizzo\n",L"*Address\n"):Tr(L"Indirizzo\n",L"Address\n");
    lines+=s->linksBand?Tr(L"*Collegamenti\n",L"*Links\n"):Tr(L"Collegamenti\n",L"Links\n");
    lines+=s->desktopBand?L"*Desktop\n":L"Desktop\n";
    lines+=L"<\n-\n";
    if(clock) lines+=Tr(L"Modifica data/ora\nPersonalizza icone di notifica\n-\n",L"Adjust date/time\nCustomize notification icons\n-\n");
    lines+=Tr(L"Sovrapponi le finestre\nMostra le finestre in pila\nMostra le finestre affiancate\nMostra desktop\n-\nGestione attività\n-\n",L"Cascade windows\nShow windows stacked\nShow windows side by side\nShow the desktop\n-\nTask Manager\n-\n");
    if(s->bandsLocked) lines+=L"*";
    lines+=Tr(L"Blocca la barra delle applicazioni\nProprietà",L"Lock the taskbar\nProperties");
    return lines;
}
// Leaf IDs 1..3 are the toolbar submenu; the clock adds 4 and 5.
static int NormalizeBarMenuCommand(bool clock,int cmd) { return clock && cmd>=6?cmd-2:cmd; }
// CreateProcess uses the explicit system path and no undocumented switches.
// Only elevation-required is retried through the shell, which can show UAC.
// On other errors a second blind launch could duplicate a running process.
#ifdef TASKBAR81_TEST
static decltype(&CreateProcessW) taskmgrCreateProcess=CreateProcessW;
static decltype(&ShellExecuteExW) taskmgrShellExecute=ShellExecuteExW;
#else
#define taskmgrCreateProcess CreateProcessW
#define taskmgrShellExecute ShellExecuteExW
#endif
static DWORD LaunchTaskManager(HWND owner) {
    wchar_t dir[MAX_PATH]{}; UINT n=GetSystemDirectoryW(dir,(UINT)std::size(dir));
    if(!n) {DWORD e=GetLastError();Wh_Log(L"[T81-DIAG] task manager system directory error=%lu",e);return e?e:ERROR_PATH_NOT_FOUND;}
    if(n>=std::size(dir)) {Wh_Log(L"[T81-DIAG] task manager system directory buffer too small");return ERROR_INSUFFICIENT_BUFFER;}
    std::wstring exe=std::wstring(dir)+L"\\Taskmgr.exe";
    // The command line buffer is mutable as required by CreateProcessW.
    std::wstring command=L"\""+exe+L"\"";
    STARTUPINFOW si{};si.cb=sizeof(si);PROCESS_INFORMATION pi{};
    if(taskmgrCreateProcess(exe.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&si,&pi)) {
        if(pi.hThread)CloseHandle(pi.hThread);if(pi.hProcess)CloseHandle(pi.hProcess);
        Wh_Log(L"[T81-DIAG] task manager CreateProcess accepted");return ERROR_SUCCESS;
    }
    DWORD error=GetLastError();
    Wh_Log(L"[T81-DIAG] task manager CreateProcess error=%lu",error);
    if(error!=ERROR_ELEVATION_REQUIRED)return error?error:ERROR_GEN_FAILURE;
    SHELLEXECUTEINFOW se{};se.cbSize=sizeof(se);se.hwnd=owner;se.lpVerb=L"open";
    se.lpFile=exe.c_str();se.nShow=SW_SHOWNORMAL;se.fMask=SEE_MASK_NOASYNC|SEE_MASK_FLAG_NO_UI;
    if(taskmgrShellExecute(&se)) {Wh_Log(L"[T81-DIAG] task manager shell/UAC request accepted");return ERROR_SUCCESS;}
    error=GetLastError();Wh_Log(L"[T81-DIAG] task manager shell/UAC error=%lu",error);
    return error?error:ERROR_GEN_FAILURE;
}
static void OpenTaskManager(HWND owner) {
    DWORD error=LaunchTaskManager(owner);
    if(error && error!=ERROR_CANCELLED) {
        wchar_t message[256]{};
        swprintf(message,std::size(message),Tr(L"Impossibile avviare Gestione attività. Errore Windows: %lu. Consulta il log di Windhawk.",
                                              L"Unable to start Task Manager. Windows error: %lu. Check the Windhawk log."),error);
        MessageBoxW(owner,message,Tr(L"Gestione attività",L"Task Manager"),MB_OK|MB_ICONERROR);
    }
}
static void ShowBarContextMenu(bool clock) {
    POINT p{};GetCursorPos(&p);std::wstring lines=BuildBarMenuText(clock);
    int cmd=w7t::ShellMenu::ShowContextMenuEx(p.x,p.y,true,lines.c_str(),true);
    if(clock) {
        Wh_Log(L"[T81-DIAG] clock/shared context command=%d",cmd);
        if(cmd==4) { ShellExecuteW(s->bar,L"open",L"control.exe",L"timedate.cpl",nullptr,SW_SHOWNORMAL);return; }
        if(cmd==5) { OpenNotificationIconsPage(s->bar);return; }
        cmd=NormalizeBarMenuCommand(true,cmd);
    }
    if(cmd==1) { s->addressBand=!s->addressBand; SaveBands(); UpdateAddressBand(); InvalidateRect(s->bar,nullptr,FALSE); }
    else if(cmd==2) { s->linksBand=!s->linksBand; SaveBands(); InvalidateRect(s->bar,nullptr,FALSE); }
    else if(cmd==3) { s->desktopBand=!s->desktopBand; SaveBands(); InvalidateRect(s->bar,nullptr,FALSE); }
    else if(cmd==4) CascadeWindows(nullptr,0,nullptr,0,nullptr);
    else if(cmd==5) TileWindows(nullptr,MDITILE_HORIZONTAL,nullptr,0,nullptr);
    else if(cmd==6) TileWindows(nullptr,MDITILE_VERTICAL,nullptr,0,nullptr);
    else if(cmd==7) ToggleDesktop();
    else if(cmd==8) OpenTaskManager(s->bar);
    else if(cmd==9) {
        s->bandsLocked=!s->bandsLocked;Wh_SetStringValue(L"Taskbar81.BandsLocked",s->bandsLocked?L"1":L"0");
        Wh_Log(L"[T81-DIAG] own taskbar drag and toolbar grips locked=%d; native taskbar lock unchanged",s->bandsLocked?1:0);InvalidateRect(s->bar,nullptr,FALSE);
    } else if(cmd==10) {
        if((INT_PTR)ShellExecuteW(s->bar,L"open",L"ms-settings:taskbar",nullptr,nullptr,SW_SHOWNORMAL)<=32) {
            IShellDispatch* raw=nullptr;HRESULT hr=CoCreateInstance(CLSID_Shell,nullptr,CLSCTX_INPROC_SERVER,IID_IShellDispatch,(void**)&raw);
            std::unique_ptr<IShellDispatch,ReleaseCom<IShellDispatch>> shell(raw);if(SUCCEEDED(hr) && shell) shell->TrayProperties();
        }
    }
}
static void ShowClockContextMenu() {
    Wh_Log(L"[T81-DIAG] private clock context menu requested");
    ShowBarContextMenu(true);
}
// Anchor the native window to the real synthetic battery cell, rather than
// the invisible 1x1 private Shell_TrayWnd that old shell code might discover.
static POINT BatteryFlyoutPosition(const RECT& anchor,const RECT& work,int width,int height,bool top,int gap) {
    // Integer centre; clamp to monitor work area even on secondary monitors.
    int x=anchor.left+(anchor.right-anchor.left-width)/2;
    int y=top?anchor.bottom+gap:anchor.top-height-gap;
    int xmax=work.right-width, ymax=work.bottom-height;
    x=std::max((int)work.left,std::min(x,xmax));
    y=std::max((int)work.top,std::min(y,ymax));
    return POINT{x,y};
}
// Uses the visual bar rather than the hidden native taskbar. Positioning is
// best effort; never subclass or change another process's window procedure.
static bool VisualFlyoutTarget(HWND flyout,const RECT& anchor,POINT* target) {
    if(!s || !target || !s->bar || !IsWindow(flyout) || !IsWindowVisible(flyout))return false;
    RECT bar{},current{};
    if(!GetWindowRect(s->bar,&bar) || !GetWindowRect(flyout,&current))return false;
    RECT icon=IsRectEmpty(&anchor)?bar:anchor;
    MONITORINFO mi{};mi.cbSize=sizeof(mi);
    if(!GetMonitorInfoW(MonitorFromRect(&icon,MONITOR_DEFAULTTONEAREST),&mi))return false;
    int width=current.right-current.left,height=current.bottom-current.top;
    if(width<=0 || height<=0 || width>mi.rcMonitor.right-mi.rcMonitor.left || height>mi.rcMonitor.bottom-mi.rcMonitor.top)return false;
    bool top=s->nativeEdge==ABE_TOP;
    // Action Center Recreation and our battery flyout use the cached icon rect,
    // including when it belongs to the overflow. Do not overwrite its Y with
    // the bar's Y. Shell_NotifyIconGetRect is not appropriate for the CNI-only
    // registration: S_FALSE identifies the native overflow arrow, not this cell.
    POINT pos=BatteryFlyoutPosition(icon,mi.rcWork,width,height,top,Px(8));
    if(s->verticalActive) {
        RECT panel{};POINT center{(icon.left+icon.right)/2,(icon.top+icon.bottom)/2};
        bool fromOverflow=s->overflowPanel && IsWindowVisible(s->overflowPanel) && GetWindowRect(s->overflowPanel,&panel) && PtInRect(&panel,center);
        RECT edge=fromOverflow?panel:bar;
        pos.x=s->verticalSide==1?edge.right+Px(8):edge.left-width-Px(8);
        pos.y=(icon.top+icon.bottom-height)/2;
        pos.x=std::max((int)mi.rcWork.left,std::min((int)pos.x,(int)mi.rcWork.right-width));
        pos.y=std::max((int)mi.rcWork.top,std::min((int)pos.y,(int)mi.rcWork.bottom-height));
    }
    *target=pos;return true;
}
static bool FlyoutAtVisualTarget(HWND flyout,const RECT& anchor) {
    POINT target{};RECT actual{};
    return VisualFlyoutTarget(flyout,anchor,&target) && GetWindowRect(flyout,&actual) &&
        std::abs(actual.left-target.x)<=2 && std::abs(actual.top-target.y)<=2;
}
static bool PlaceFlyoutAtVisualBar(HWND flyout,const RECT& anchor) {
    POINT pos{};if(!VisualFlyoutTarget(flyout,anchor,&pos))return false;
    UINT flags=SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE;
    DWORD tid=GetWindowThreadProcessId(flyout,nullptr);if(tid!=GetCurrentThreadId())flags|=SWP_ASYNCWINDOWPOS;
    BOOL ok=SetWindowPos(flyout,nullptr,pos.x,pos.y,0,0,flags);
    Wh_Log(L"[T81-DIAG] flyout visual placement hwnd=%p top=%d anchor=%ld,%ld,%ld,%ld x=%ld y=%ld accepted=%d",
           flyout,s->nativeEdge==ABE_TOP?1:0,anchor.left,anchor.top,anchor.right,anchor.bottom,pos.x,pos.y,ok?1:0);
    return ok!=FALSE;
}
// Compatibility with win7-action-center-recreation 2.2.0, a separate mod.
// Its tray-handler and flyout class names are private implementation details.
// Only arm on a validated click on that handler in this Explorer process.
static HWND FindOwnActionFlyout() {
    HWND h=nullptr;while((h=FindWindowExW(nullptr,h,L"Win7ActionCenterFlyoutClass",nullptr))) {
        DWORD pid=0;GetWindowThreadProcessId(h,&pid);
        if(pid==GetCurrentProcessId() && IsWindowVisible(h))return h;
    }return nullptr;
}
static void QueueActionFlyoutPosition(int index,POINT screen) {
    if(!s || !s->verticalActive || !s->bar || index<0 || (size_t)index>=s->trayIcons.size())return;
    const auto& icon=s->trayIcons[index];wchar_t cls[80]{};
    DWORD pid=0;GetWindowThreadProcessId(icon.owner,&pid);
    if(pid!=GetCurrentProcessId() || !GetClassNameW(icon.owner,cls,80) || wcscmp(cls,L"Win7ActionCenterTrayMsgClass"))return;
    KillTimer(s->bar,TIMER_ACTION_POSITION);s->actionPositionAttempts=0;
    if(FindOwnActionFlyout())return; // closing an existing flyout, not opening one
    RECT r{};if(!OverflowIconScreenRect(index,&r)) {r=PrivateIconRect(index);if(!IsRectEmpty(&r))MapWindowPoints(s->bar,nullptr,(POINT*)&r,2);}
    if(IsRectEmpty(&r) || !PtInRect(&r,screen))r=RECT{screen.x,screen.y,screen.x+1,screen.y+1};
    s->actionAnchor=r;s->actionPositionAttempts=20;SetTimer(s->bar,TIMER_ACTION_POSITION,100,nullptr);
    Wh_Log(L"[T81-DIAG] action center companion click recognized; checking flyout placement");
}
static RECT PrivateSystemAnchor(int hit) {
    RECT r{};if(!s || !s->bar)return r;
    if(hit!=HIT_CLOCK && OverflowIconScreenRect(SystemEntry(hit),&r))return r;
    if(hit==HIT_CLOCK) {RECT client{};
        GetClientRect(s->bar,&client);
        r=s->verticalActive?GetVerticalLayout(client.right,client.bottom).clock:GetLayout(client.right,client.bottom).clock;
        }
    else r=SystemCell(hit);
    if(!IsRectEmpty(&r))MapWindowPoints(s->bar,nullptr,(POINT*)&r,2);
    return r;
}
// BatMeterFlyout-only adaptation of aubymori's Aero Flyout Fix: (MIT; full license notice at the end of this file)
// https://windhawk.net/mods/aero-flyout-fix -- add the thick frame and
// tool-window style after the 1511 flyout has updated. Keep resize hit tests
// noninteractive and remove the subclass on the owning thread at teardown.
static constexpr UINT_PTR kBatteryFrameSubclass=0x8130;
static LRESULT CALLBACK BatteryFrameSubclass(HWND h,UINT msg,WPARAM wp,LPARAM lp,UINT_PTR id,DWORD_PTR) {
    if(msg==WM_NCHITTEST) {
        LRESULT hit=DefSubclassProc(h,msg,wp,lp);
        if(hit==HTTOP || hit==HTTOPRIGHT || hit==HTRIGHT || hit==HTBOTTOMRIGHT ||
           hit==HTBOTTOM || hit==HTBOTTOMLEFT || hit==HTLEFT || hit==HTTOPLEFT)return HTBORDER;
        return hit;
    }
    if(msg==WM_NCDESTROY) {
        RemoveWindowSubclass(h,BatteryFrameSubclass,id);
        if(s && s->batteryFrameWindow==h)s->batteryFrameWindow=nullptr;
    }
    return DefSubclassProc(h,msg,wp,lp);
}
static void CleanupBatteryFrame() {
    if(!s || !s->batteryFrameWindow)return;
    HWND h=s->batteryFrameWindow;s->batteryFrameWindow=nullptr;
    if(IsWindow(h) && GetWindowThreadProcessId(h,nullptr)==GetCurrentThreadId()) {
        RemoveWindowSubclass(h,BatteryFrameSubclass,kBatteryFrameSubclass);
        LONG_PTR style=GetWindowLongPtrW(h,GWL_STYLE),ex=GetWindowLongPtrW(h,GWL_EXSTYLE);
        SetWindowLongPtrW(h,GWL_STYLE,(style&~(LONG_PTR)WS_THICKFRAME)|(s->batteryFrameOriginalStyle&WS_THICKFRAME));
        SetWindowLongPtrW(h,GWL_EXSTYLE,(ex&~(LONG_PTR)WS_EX_TOOLWINDOW)|(s->batteryFrameOriginalExStyle&WS_EX_TOOLWINDOW));
        SetWindowPos(h,nullptr,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
    }
}
static void StyleOriginalBatteryFlyout(HWND flyout) {
    if(!s || !flyout || GetWindowThreadProcessId(flyout,nullptr)!=GetCurrentThreadId())return;
    wchar_t cls[64]{};if(!GetClassNameW(flyout,cls,64) || wcscmp(cls,L"BatMeterFlyout"))return;
    if(s->batteryFrameWindow && s->batteryFrameWindow!=flyout)CleanupBatteryFrame();
    LONG_PTR oldStyle=GetWindowLongPtrW(flyout,GWL_STYLE),oldEx=GetWindowLongPtrW(flyout,GWL_EXSTYLE);
    if(!s->batteryFrameWindow) {
        if(!SetWindowSubclass(flyout,BatteryFrameSubclass,kBatteryFrameSubclass,0)) {
            Wh_Log(L"[T81-DIAG] battery frame subclass failed error=%lu; leave original frame",GetLastError());return;
        }
        s->batteryFrameWindow=flyout;s->batteryFrameOriginalStyle=oldStyle;s->batteryFrameOriginalExStyle=oldEx;
    }
    // The 1511 UpdateFlyoutUI resets both styles each time it runs.
    if(!(oldStyle&WS_THICKFRAME) || !(oldEx&WS_EX_TOOLWINDOW)) {
        RECT before{};GetClientRect(flyout,&before);
        SetLastError(0);LONG_PTR result=SetWindowLongPtrW(flyout,GWL_STYLE,oldStyle|WS_THICKFRAME);
        DWORD err=result?0:GetLastError();
        SetLastError(0);LONG_PTR resultEx=SetWindowLongPtrW(flyout,GWL_EXSTYLE,oldEx|WS_EX_TOOLWINDOW);
        DWORD errEx=resultEx?0:GetLastError();
        if(err || errEx) {Wh_Log(L"[T81-DIAG] battery frame style failed style=%lu ex=%lu",err,errEx);CleanupBatteryFrame();return;}
        SetWindowPos(flyout,nullptr,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
        RECT after{},outer{};
        if(GetClientRect(flyout,&after) && GetWindowRect(flyout,&outer)) {
            int dw=(before.right-before.left)-(after.right-after.left),dh=(before.bottom-before.top)-(after.bottom-after.top);
            if(dw>0 || dh>0)SetWindowPos(flyout,nullptr,0,0,outer.right-outer.left+std::max(0,dw),outer.bottom-outer.top+std::max(0,dh),SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
        }
        Wh_Log(L"[T81-DIAG] battery thick frame applied style=0x%Ix ex=0x%Ix",(size_t)GetWindowLongPtrW(flyout,GWL_STYLE),(size_t)GetWindowLongPtrW(flyout,GWL_EXSTYLE));
    }
    HIGHCONTRASTW hc{};hc.cbSize=sizeof(hc);
    bool high=SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(hc),&hc,0) && (hc.dwFlags&HCF_HIGHCONTRASTON);
    if(!high) {
        const int noRound=1; const COLORREF automatic=0xffffffff;
        HRESULT a=DwmSetWindowAttribute(flyout,(DWMWINDOWATTRIBUTE)33,&noRound,sizeof(noRound));
        HRESULT b=DwmSetWindowAttribute(flyout,(DWMWINDOWATTRIBUTE)34,&automatic,sizeof(automatic));
        Wh_Log(L"[T81-DIAG] battery DWM square=0x%08lx system-border=0x%08lx",(unsigned long)a,(unsigned long)b);
    }
}
static void CancelBatterySlide() {
    if(!s)return;
    if(s->bar) KillTimer(s->bar,TIMER_BATTERY_SLIDE);
    s->batterySlideWindow=nullptr;s->batterySlideOffset=0;
}
// Integer ease-out; never fade and never block the window/message thread.
static int BatterySlideDelta(int offset,ULONGLONG elapsed) {
    if(elapsed>=70)return 0;
    ULONGLONG remaining=70-elapsed;
    return (int)((long long)offset*(long long)(remaining*remaining)/4900);
}
static void TickBatterySlide() {
    if(!s || !s->batterySlideWindow)return;
    HWND h=s->batterySlideWindow;
    if(!IsWindow(h) || !IsWindowVisible(h) || GetWindowThreadProcessId(h,nullptr)!=GetCurrentThreadId()) {CancelBatterySlide();return;}
    ULONGLONG elapsed=GetTickCount64()-s->batterySlideStart;
    int dy=BatterySlideDelta(s->batterySlideOffset,elapsed);
    SetWindowPos(h,nullptr,s->batterySlideTarget.x,s->batterySlideTarget.y+dy,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
    if(elapsed>=70)CancelBatterySlide();
}
static void StartBatterySlide(HWND h,POINT target,int offset) {
    CancelBatterySlide();
    if(!s || !s->bar || !h || !offset)return;
    BOOL animations=FALSE;
    if(!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0) || !animations)return;
    s->batterySlideWindow=h;s->batterySlideTarget=target;s->batterySlideOffset=offset;s->batterySlideStart=GetTickCount64();
    if(!SetTimer(s->bar,TIMER_BATTERY_SLIDE,15,nullptr)) {CancelBatterySlide();return;}
    SetWindowPos(h,nullptr,target.x,target.y+offset,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}
static void PositionOriginalBatteryFlyout(HWND flyout) {
    if(!s || !flyout || !s->bar)return;
    RECT anchor{};bool popup=OverflowIconScreenRect(SystemEntry(HIT_BATTERY),&anchor);
    if(!popup && IsSystemVisible(HIT_BATTERY)) {anchor=SystemCell(HIT_BATTERY);MapWindowPoints(s->bar,nullptr,(POINT*)&anchor,2);}
    else if(!popup) { POINT cursor{};if(!GetCursorPos(&cursor))return;anchor={cursor.x,cursor.y,cursor.x+1,cursor.y+1}; }
    RECT r{};if(!GetWindowRect(flyout,&r))return;int w=r.right-r.left,h=r.bottom-r.top;
    if(w<=0 || h<=0)return;
    MONITORINFO mi{};mi.cbSize=sizeof(mi);if(!GetMonitorInfoW(MonitorFromRect(&anchor,MONITOR_DEFAULTTONEAREST),&mi))return;
    POINT position=BatteryFlyoutPosition(anchor,mi.rcWork,w,h,s->nativeEdge==ABE_TOP,Px(4));
    if(s->verticalActive) {
        RECT bar{};if(!GetWindowRect(s->bar,&bar))return;
        RECT panel{};if(popup && s->overflowPanel && GetWindowRect(s->overflowPanel,&panel))bar=panel;
        position.x=s->verticalSide==1?bar.right+Px(4):bar.left-w-Px(4);
        position.y=(anchor.top+anchor.bottom-h)/2;
        position.x=std::max((int)mi.rcWork.left,std::min((int)position.x,(int)mi.rcWork.right-w));
        position.y=std::max((int)mi.rcWork.top,std::min((int)position.y,(int)mi.rcWork.bottom-h));
        CancelBatterySlide();SetWindowPos(flyout,HWND_TOPMOST,position.x,position.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);return;
    }
    int x=position.x,y=position.y;
    SetWindowPos(flyout,HWND_TOPMOST,x,y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
    // Move only into available work area. With a bottom bar the flyout rises;
    // with a top bar it moves down. Do not cross the monitor work bounds.
    int desired=Px(8),offset=s->nativeEdge==ABE_TOP?-std::max(0,std::min(desired,y-(int)mi.rcWork.top)):std::max(0,std::min(desired,(int)mi.rcWork.bottom-(y+h)));
    StartBatterySlide(flyout,position,offset);
}

static BOOL CALLBACK FindOwnBatteryFlyout(HWND hwnd,LPARAM param) {
    wchar_t cls[64]{};
    if(GetClassNameW(hwnd,cls,64) && !wcscmp(cls,L"BatMeterFlyout") && IsWindowVisible(hwnd)) {
        *reinterpret_cast<HWND*>(param)=hwnd;return FALSE;
    }
    return TRUE;
}
// A narrowly filtered resource bridge for this one verified DLL. The normal
// MUI lookup is tried first; if it fails, return strings directly from the
// separately verified resource-only MUI. All other modules pass through.
using BatteryLoadStringFn=int (WINAPI*)(HINSTANCE,UINT,LPWSTR,int);
static BatteryLoadStringFn realLoadStringW=nullptr;
static std::atomic<HMODULE> batteryStringModule{nullptr},batteryStringMui{nullptr};
static DWORD batteryOwnerThread=0;
static int WINAPI BatteryLoadStringHook(HINSTANCE instance,UINT id,LPWSTR buffer,int capacity) {
    if(!realLoadStringW)return 0;
    int n=realLoadStringW(instance,id,buffer,capacity);
    HMODULE expected=batteryStringModule.load(),satellite=batteryStringMui.load();
    if(!n && expected && satellite && instance==expected)
        return realLoadStringW(satellite,id,buffer,capacity);
    return n;
}
// The exact 1511 code itself calls QueryBatteryData(nullptr, &data) with a
// 0x80-byte versioned structure at RVA 0x3d320. Try the same refresh against
// the OS BatMeter implementation. This advanced feature is ABI-sensitive.
static void RefreshOriginalBatterySnapshot(HMODULE module) {
    BYTE* data=(BYTE*)module+0x3d320;MEMORY_BASIC_INFORMATION mem{};
    if(!VirtualQuery(data,&mem,sizeof(mem)) || mem.State!=MEM_COMMIT ||
       (mem.Protect & (PAGE_GUARD|PAGE_NOACCESS)) ||
       ((BYTE*)mem.BaseAddress+mem.RegionSize<data+128) ||
       !(mem.Protect & (PAGE_READWRITE|PAGE_WRITECOPY|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY))) {
        Wh_Log(L"[T81-DIAG] battery private snapshot memory unavailable");return;
    }
    ResourceModule os;os.h=LoadLibraryExW(L"batmeter.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    wchar_t systemDir[32768]{},actual[32768]{};
    UINT sysLen=GetSystemDirectoryW(systemDir,(UINT)std::size(systemDir));
    DWORD actualLen=os.h?GetModuleFileNameW(os.h,actual,(DWORD)std::size(actual)):0;
    std::wstring expected=sysLen && sysLen<std::size(systemDir)?std::wstring(systemDir)+L"\\batmeter.dll":L"";
    if(!actualLen || actualLen>=std::size(actual) || expected.empty() || _wcsicmp(actual,expected.c_str())!=0) {
        Wh_Log(L"[T81-DIAG] OS battery module path mismatch; skip private snapshot refresh");return;
    }
    using QueryFn=INT_PTR (WINAPI*)(void*,void*);
    auto query=reinterpret_cast<QueryFn>(GetProcAddress(os.h,"QueryBatteryData"));
    if(!query) {Wh_Log(L"[T81-DIAG] OS battery QueryBatteryData unavailable; snapshot not refreshed");return;}
    *reinterpret_cast<DWORD*>(data)=128;
    INT_PTR result=query(nullptr,data);
    Wh_Log(L"[T81-DIAG] battery QueryBatteryData private snapshot result=0x%Ix",result);
}
// The verified 1511 binary's matching public PDB identifies RVA 0x162d8 as
// Flyout_UpdateCompositeIcon(HICON, DirectUI::Element*). The routine first
// duplicates the supplied HICON; it is safe to destroy our source after show
// returns. Interception is thread-local and only during our ShowBatteryUI.
using BatteryCompositeFn=void (WINAPI*)(HICON,void*);
static BatteryCompositeFn realBatteryComposite=nullptr;
static thread_local HICON batteryCompositeOverride=nullptr;
static thread_local unsigned batteryCompositeHits=0;
static bool batteryCompositeHookAttempted=false,batteryCompositeHookReady=false;
static void WINAPI BatteryCompositeHook(HICON original,void* element) {
    if(batteryCompositeOverride)++batteryCompositeHits;
    if(realBatteryComposite)realBatteryComposite(batteryCompositeOverride?batteryCompositeOverride:original,element);
}
static bool InstallBatteryCompositeHook(HMODULE module) {
    if(batteryCompositeHookAttempted)return batteryCompositeHookReady;
    batteryCompositeHookAttempted=true;
    BYTE* target=(BYTE*)module+0x162d8;
    const BYTE prefix[]={0x48,0x85,0xd2,0x0f,0x84};
    if(memcmp(target,prefix,sizeof(prefix))!=0) {Wh_Log(L"[T81-DIAG] battery composite signature mismatch");return false;}
    if(!Wh_SetFunctionHook(target,(void*)BatteryCompositeHook,(void**)&realBatteryComposite)) {
        Wh_Log(L"[T81-DIAG] battery composite hook registration failed");return false;
    }
    batteryCompositeHookReady=Wh_ApplyHookOperations()!=FALSE && realBatteryComposite;
    Wh_Log(L"[T81-DIAG] battery composite hook installed=%d",batteryCompositeHookReady?1:0);
    return batteryCompositeHookReady;
}
static int LegacyLargeBatteryFrame(const SYSTEM_POWER_STATUS& st) {
    if(st.BatteryFlag!=255 && (st.BatteryFlag&128))return 27;
    if(st.BatteryLifePercent>100)return 26;
    int p=st.BatteryLifePercent;
    if(p<=10)return 19+(p*6+50)/100;
    if(p<=20)return 10+(p*8+50)/100;
    return (p*9+50)/100;
}
// Read the 8.1 PE exclusively as resources: 351/352 are 31-frame
// 32/40px large composite artwork, unlike the 43-frame tray strips.
static HICON CreateLegacyLargeBatteryIcon(const SYSTEM_POWER_STATUS& st) {
    if(!s || s->resourceDir.empty())return nullptr;
    ResourceModule source;
    if(!OpenVerifiedResource(ResourcePath(s->resourceDir,kLegacyFiles[2]),kLegacyFiles[2],source))return nullptr;
    int size=Px(32)>32?40:32,id=size==40?352:351,width=size*31;
    HRSRC r=FindResourceW(source.h,MAKEINTRESOURCEW(id),RT_BITMAP);
    DWORD bytes=r?SizeofResource(source.h,r):0;HGLOBAL loaded=r?LoadResource(source.h,r):nullptr;
    const BYTE* raw=loaded?(const BYTE*)LockResource(loaded):nullptr;
    BITMAPINFOHEADER bh{};if(!raw || bytes<40)return nullptr;memcpy(&bh,raw,40);
    if(bh.biSize!=40 || bh.biWidth!=width || abs(bh.biHeight)!=size || bh.biPlanes!=1 || bh.biBitCount!=32 || bh.biCompression!=BI_RGB || bytes<40+(size_t)width*size*4)return nullptr;
    // Keep both original 8.1 cells registered at the same origin. A small
    // transparent border protects edge pixels during the flyout scaling.
    int margin=std::max(1,size/16), canvas=size+2*margin;
    Bitmap strip(width,size,PixelFormat32bppARGB), output(canvas,canvas,PixelFormat32bppARGB);
    if(strip.GetLastStatus()!=Ok || output.GetLastStatus()!=Ok)return nullptr;
    BitmapData d{};Rect rect(0,0,width,size);
    if(strip.LockBits(&rect,ImageLockModeWrite,PixelFormat32bppARGB,&d)!=Ok)return nullptr;
    for(int y=0;y<size;y++) {
        int sy=bh.biHeight>0?size-1-y:y;
        memcpy((BYTE*)d.Scan0+(ptrdiff_t)y*d.Stride,raw+40+(size_t)sy*width*4,(size_t)width*4);
    }
    strip.UnlockBits(&d);
    {Graphics g(&output);g.Clear(Color(0,0,0,0));g.SetInterpolationMode(InterpolationModeNearestNeighbor);
     int frame=LegacyLargeBatteryFrame(st);bool plug=st.ACLineStatus==1 && frame!=27;
     g.DrawImage(&strip,Rect(margin,margin,size,size),frame*size,0,size,size,UnitPixel);
     if(plug)g.DrawImage(&strip,Rect(margin,margin,size,size),30*size,0,size,size,UnitPixel);
    }
    HICON icon=nullptr;if(output.GetHICON(&icon)!=Ok)return nullptr;return icon;
}
// Directory names are random and must be created new. Keep an exclusive marker
// open while this UI thread owns its instance. Never recursively delete unknown
// files or follow reparse points while cleaning up earlier instances.
static bool BatteryPlainDirectory(const std::wstring& path) {
    DWORD attr=GetFileAttributesW(path.c_str());
    return attr!=INVALID_FILE_ATTRIBUTES && (attr&FILE_ATTRIBUTE_DIRECTORY) && !(attr&FILE_ATTRIBUTE_REPARSE_POINT);
}
static bool BatteryCopyVerified(const std::wstring& from,const std::wstring& to,const LegacyFile& spec) {
    if(!CopyFileW(from.c_str(),to.c_str(),TRUE))return false;
    if(VerifiedOnDisk(to,spec))return true;
    DeleteFileW(to.c_str());return false;
}
static bool PrepareBatteryInstance(std::wstring& dllPath,std::wstring& muiPath) {
    if(!s || s->resourceDir.empty())return false;
    std::wstring base=BatteryModuleDirectory(s->resourceDir);
    if(PathIsRelativeW(base.c_str()) || !BatteryPlainDirectory(base))return false;
    if(s->batteryInstanceDir.empty()) {
        for(int attempt=0;attempt<16 && s->batteryInstanceDir.empty();++attempt) {
            BYTE random[16]{};
            if(BCryptGenRandom(nullptr,random,sizeof(random),BCRYPT_USE_SYSTEM_PREFERRED_RNG)<0)return false;
            static const wchar_t hex[]=L"0123456789abcdef";std::wstring name=L"inst-";
            for(BYTE c:random){name+=hex[c>>4];name+=hex[c&15];}
            std::wstring candidate=base+L"\\"+name;
            if(!CreateDirectoryW(candidate.c_str(),nullptr)) {if(GetLastError()==ERROR_ALREADY_EXISTS)continue;return false;}
            if(!BatteryPlainDirectory(candidate)){RemoveDirectoryW(candidate.c_str());return false;}
            HANDLE lock=CreateFileW((candidate+L"\\session.lock").c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(lock==INVALID_HANDLE_VALUE){RemoveDirectoryW(candidate.c_str());return false;}
            FILETIME created{},exited{},kernel{},user{};
            struct {DWORD pid;FILETIME created;} marker{GetCurrentProcessId(),{}};
            DWORD written=0;
            if(!GetProcessTimes(GetCurrentProcess(),&created,&exited,&kernel,&user)) {
                CloseHandle(lock);DeleteFileW((candidate+L"\\session.lock").c_str());RemoveDirectoryW(candidate.c_str());return false;
            }
            marker.created=created;
            if(!WriteFile(lock,&marker,sizeof(marker),&written,nullptr) || written!=sizeof(marker) || !FlushFileBuffers(lock)) {
                CloseHandle(lock);DeleteFileW((candidate+L"\\session.lock").c_str());RemoveDirectoryW(candidate.c_str());return false;
            }
            s->batteryInstanceDir=candidate;s->batteryInstanceLock=lock;
        }
        if(s->batteryInstanceDir.empty())return false;
    }
    const auto& inst=s->batteryInstanceDir;
    if(!BatteryPlainDirectory(inst) || s->batteryInstanceLock==INVALID_HANDLE_VALUE)return false;
    std::wstring muiDir=inst+(ItalianUI()?L"\\it-IT":L"\\en-US");
    if(!CreateDirectoryW(muiDir.c_str(),nullptr) && GetLastError()!=ERROR_ALREADY_EXISTS)return false;
    if(!BatteryPlainDirectory(muiDir))return false;
    dllPath=ResourcePath(inst,kBattery1511);muiPath=ResourcePath(muiDir,SelectedBatteryMui());
    if(!VerifiedOnDisk(dllPath,kBattery1511)) {
        DeleteFileW(dllPath.c_str());
        if(!BatteryCopyVerified(ResourcePath(base,kBattery1511),dllPath,kBattery1511))return false;
    }
    if(!VerifiedOnDisk(muiPath,SelectedBatteryMui())) {
        DeleteFileW(muiPath.c_str());
        if(!BatteryCopyVerified(ResourcePath(BatteryMuiDirectory(s->resourceDir),SelectedBatteryMui()),muiPath,SelectedBatteryMui()))return false;
    }
    return true;
}
static bool BatteryInstanceName(const wchar_t* name) {
    if(!name || wcslen(name)!=37 || wcsncmp(name,L"inst-",5))return false;
    for(int i=5;i<37;++i)if(!((name[i]>=L'0' && name[i]<=L'9') || (name[i]>=L'a' && name[i]<=L'f')))return false;
    return true;
}
static void DeleteBatteryKnownFile(const std::wstring& path) {
    DWORD attr=GetFileAttributesW(path.c_str());
    if(attr!=INVALID_FILE_ATTRIBUTES && !(attr&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)))DeleteFileW(path.c_str());
}
static void CleanOldBatteryInstances() {
    if(!s || s->batteryCleanupDone || s->batteryInstanceDir.empty())return;
    s->batteryCleanupDone=true;std::wstring base=BatteryModuleDirectory(s->resourceDir);
    if(!BatteryPlainDirectory(base))return;
    WIN32_FIND_DATAW fd{};HANDLE f=FindFirstFileW((base+L"\\inst-*").c_str(),&fd);
    if(f==INVALID_HANDLE_VALUE)return;
    ScopedFind find(f);
    do {
        if(!BatteryInstanceName(fd.cFileName) || !(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || (fd.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT))continue;
        std::wstring dir=base+L"\\"+fd.cFileName;if(_wcsicmp(dir.c_str(),s->batteryInstanceDir.c_str())==0 || !BatteryPlainDirectory(dir))continue;
        std::wstring markerPath=dir+L"\\session.lock";
        ScopedHandle lock(CreateFileW(markerPath.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT,nullptr));
        if(lock.h==INVALID_HANDLE_VALUE)continue; // another live session or no valid marker
        BY_HANDLE_FILE_INFORMATION markerInfo{};
        struct {DWORD pid;FILETIME created;} marker{};DWORD read=0;
        if(!GetFileInformationByHandle(lock.h,&markerInfo) || (markerInfo.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT) ||
           !ReadFile(lock.h,&marker,sizeof(marker),&read,nullptr) || read!=sizeof(marker) || !marker.pid)continue;
        ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,marker.pid));
        if(process.h && process.h!=INVALID_HANDLE_VALUE) {
            FILETIME created{},exited{},kernel{},user{};
            if(!GetProcessTimes(process.h,&created,&exited,&kernel,&user))continue;
            if(CompareFileTime(&created,&marker.created)==0)continue; // its pinned copy may still be mapped
        } else if(GetLastError()!=ERROR_INVALID_PARAMETER)continue; // inaccessible is not proof of exit
        DeleteBatteryKnownFile(dir+L"\\stobject.dll");
        for(const wchar_t* language:{L"it-IT",L"en-US"}) {
            std::wstring child=dir+L"\\"+language;if(!BatteryPlainDirectory(child))continue;
            DeleteBatteryKnownFile(child+L"\\stobject.dll.mui");RemoveDirectoryW(child.c_str());
        }
        CloseHandle(lock.h);lock.h=INVALID_HANDLE_VALUE;
        DeleteBatteryKnownFile(markerPath);RemoveDirectoryW(dir.c_str());
    } while(FindNextFileW(f,&fd));
}
// Advanced focus workaround: leave focus on the popup, rather than
// its DirectUI child, so that opening the popup does not preselect a link.
// Never inject keystrokes or redirect focus outside the battery popup.
static void ClearBatteryInitialChildFocus(HWND flyout) {
    if(!flyout || !IsWindowVisible(flyout) ||
       GetWindowThreadProcessId(flyout,nullptr)!=GetCurrentThreadId())return;
    POINT cursor{};RECT bounds{};
    if(!GetCursorPos(&cursor) || !GetWindowRect(flyout,&bounds) || PtInRect(&bounds,cursor))return;
    HWND child=GetFocus();
    if(!child || !IsChild(flyout,child) ||
       GetWindowThreadProcessId(child,nullptr)!=GetCurrentThreadId())return;
    wchar_t cls[64]{};
    if(!GetClassNameW(child,cls,(int)std::size(cls)) || wcscmp(cls,L"DirectUIHWND"))return;
    SetLastError(0);
    HWND previous=SetFocus(flyout);
    HWND after=GetFocus();
    Wh_Log(L"[T81-BAT-FOCUS] initial child=%p previous=%p current=%p visible=%d error=%lu",
           child,previous,after,IsWindowVisible(flyout)?1:0,GetLastError());
}
// Opt-in only: direct call into the exact 1511 x64 binary, RVA from its
// matching Microsoft public PDB. It initializes DirectUI and creates the
// BatMeterFlyout class. These private APIs are version-sensitive.
static bool TryOriginalBatteryFlyout1511() {
    if(!s || !useOriginalBatteryFlyout.load() || s->resourceDir.empty())return false;
    std::wstring source=ResourcePath(BatteryModuleDirectory(s->resourceDir),kBattery1511);
    std::wstring sourceMui=ResourcePath(BatteryMuiDirectory(s->resourceDir),SelectedBatteryMui());
    if(PathIsRelativeW(source.c_str()) || !VerifiedOnDisk(source,kBattery1511) || !VerifiedOnDisk(sourceMui,SelectedBatteryMui())) {
        Wh_Log(L"[T81-DIAG] battery 1511 source or MUI unavailable/invalid; download may still be in progress");return false;
    }
    std::wstring path,muiPath;
    if(!PrepareBatteryInstance(path,muiPath)) {
        Wh_Log(L"[T81-DIAG] battery session copy could not be prepared");return false;
    }
    CleanOldBatteryInstances();
    ScopedHandle locked(CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    if(locked.h==INVALID_HANDLE_VALUE || !ReadVerifiedHandle(locked.h,kBattery1511)) {
        Wh_Log(L"[T81-DIAG] battery session copy failed verification");return false;
    }
    HMODULE module=GetModuleHandleW(path.c_str());
    // The matching public PDB names RVA 0x181b4 DestroyFlyout. Its complete
    // lifetime/thread and reinitialization contract is private and unverified;
    // do not call it or waive this guard based on the symbol name alone.
    if(module && batteryOwnerThread!=GetCurrentThreadId()) {
        Wh_Log(L"[T81-DIAG] battery session module bound to a different thread; refusing reuse");return false;
    }
    Wh_Log(L"[T81-DIAG] original battery 1511 verified; loading code into Explorer");
    bool loadedHere=module==nullptr;
    if(!module) module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module) { Wh_Log(L"[T81-DIAG] battery 1511 LoadLibrary failed error=%lu",GetLastError());return false; }
    wchar_t loadedPath[32768]{};DWORD loadedLength=GetModuleFileNameW(module,loadedPath,(DWORD)std::size(loadedPath));
    if(!loadedLength || loadedLength>=std::size(loadedPath) || _wcsicmp(loadedPath,path.c_str())!=0) {
        Wh_Log(L"[T81-DIAG] battery DLL path mismatch; refusing private RVA");if(loadedHere)FreeLibrary(module);return false;
    }
    HMODULE pinned=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,(LPCWSTR)module,&pinned)) {
        Wh_Log(L"[T81-DIAG] battery 1511 pin failed error=%lu",GetLastError());if(loadedHere)FreeLibrary(module);return false;
    }
    // Resource-only module remains mapped to process exit, including for
    // LoadStringW's capacity=0 pointer form. No code from the MUI is executed.
    if(realLoadStringW && !batteryStringMui.load()) {
        ScopedHandle muiLock(CreateFileW(muiPath.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
        if(muiLock.h!=INVALID_HANDLE_VALUE && ReadVerifiedHandle(muiLock.h,SelectedBatteryMui())) {
            HMODULE data=LoadLibraryExW(muiPath.c_str(),nullptr,LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE|LOAD_LIBRARY_AS_IMAGE_RESOURCE);
            if(data) batteryStringMui.store(data);
        }
    }
    batteryOwnerThread=GetCurrentThreadId();
    batteryStringModule.store(module);
    Wh_Log(L"[T81-DIAG] battery resource bridge hook=%d satellite=%d",realLoadStringW?1:0,batteryStringMui.load()?1:0);
    // Log only lengths, never resource text; MUI binding and completeness are
    // platform-dependent. A loaded satellite alone cannot initialize battery data.
    wchar_t text[512]{};int n192=LoadStringW(module,192,text,512);
    int n195=LoadStringW(module,195,text,512),n198=LoadStringW(module,198,text,512);
    Wh_Log(L"[T81-DIAG] battery 1511 MUI LoadString lengths 192=%d 195=%d 198=%d",n192,n195,n198);
    SYSTEM_POWER_STATUS status{}; if(GetSystemPowerStatus(&status))
        Wh_Log(L"[T81-DIAG] system battery percent=%u AC=%u; flyout may use different private state",(unsigned)status.BatteryLifePercent,(unsigned)status.ACLineStatus);
    using ShowFn=HRESULT (WINAPI*)();
    auto show=reinterpret_cast<ShowFn>((BYTE*)module+0x187e0);
    Wh_Log(L"[T81-DIAG] original battery 1511 ShowBatteryUI enter");
    RefreshOriginalBatterySnapshot(module);
    HICON overrideIcon=nullptr;
    if(InstallBatteryCompositeHook(module) && GetSystemPowerStatus(&status)) {
        overrideIcon=CreateLegacyLargeBatteryIcon(status);
        batteryCompositeOverride=overrideIcon;batteryCompositeHits=0;
        Wh_Log(L"[T81-DIAG] battery large artwork ready=%d frame=%d AC=%u",overrideIcon?1:0,LegacyLargeBatteryFrame(status),(unsigned)status.ACLineStatus);
    }
    HRESULT hr=show();
    batteryCompositeOverride=nullptr;
    if(overrideIcon) {Wh_Log(L"[T81-DIAG] battery large artwork applied calls=%u",batteryCompositeHits);DestroyIcon(overrideIcon);}
    HWND flyout=nullptr; EnumThreadWindows(GetCurrentThreadId(),FindOwnBatteryFlyout,(LPARAM)&flyout);
    bool visible=flyout && IsWindowVisible(flyout);
    Wh_Log(L"[T81-DIAG] battery 1511 ShowBatteryUI hr=0x%08lx own-visible=%d hwnd=%p",(unsigned long)hr,visible?1:0,flyout);
    if(visible) {StyleOriginalBatteryFlyout(flyout);PositionOriginalBatteryFlyout(flyout);ClearBatteryInitialChildFocus(flyout);}
    else CancelBatterySlide();
    return SUCCEEDED(hr) && visible;
}
static void ShowInfoTooltip(const std::wstring& message,int center);
// IPC implemented by babamohammed's Windows 8 Network Flyout Restorer 1.0.0.
// This is its internal host protocol, not a Windows or stable Windhawk API.
// A successful PostMessage only means queued, not that VAN actually opened.
static bool RequestWin8NetworkFlyout() {
    const UINT message=RegisterWindowMessageW(L"Windhawk.Win8NetworkFlyout.Show");
    if(!message)return false;
    HWND host=nullptr;
    while((host=FindWindowExW(HWND_MESSAGE,host,L"WindhawkWin8VanHostWnd",nullptr))) {
        DWORD pid=0;GetWindowThreadProcessId(host,&pid);
        if(pid!=GetCurrentProcessId())continue;
        if(PostMessageW(host,message,1,0)) {
            Wh_Log(L"[T81-DIAG] win8 network host IPC queued (not confirmation of display)");return true;
        }
        Wh_Log(L"[T81-DIAG] win8 network host IPC failed error=%lu",GetLastError());
    }
    return false;
}
// Advanced native VAN route, used only when no companion flyout accepted
// the request. Run the legacy entry point in a separate system process rather
// than calling undocumented VAN exports on the taskbar's UI thread.
static bool RequestNativeWin8NetworkFlyout() {
    using RtlGetVersionFn=LONG (WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll=GetModuleHandleW(L"ntdll.dll");
    auto rtl=ntdll?(RtlGetVersionFn)GetProcAddress(ntdll,"RtlGetVersion"):nullptr;
    OSVERSIONINFOW os{};os.dwOSVersionInfoSize=sizeof(os);
    if(!rtl || rtl(&os)!=0 || os.dwMajorVersion!=10 ||
       os.dwBuildNumber<17763 || os.dwBuildNumber>=25346) {
        Wh_Log(L"[T81-VAN-NATIVE] build not handled or unknown; skipping");return false;
    }
    wchar_t systemDir[MAX_PATH]{};
    UINT n=GetSystemDirectoryW(systemDir,(UINT)std::size(systemDir));
    if(!n || n>=std::size(systemDir))return false;
    const wchar_t* const components[]={L"pnidui.dll",L"van.dll",L"wlanmm.dll",L"wlanconn.dll",
                                       L"dot3mm.dll",L"rasmm.dll",L"datusage.dll",L"SetProxyCredential.dll"};
    for(const wchar_t* name:components) {
        std::wstring path=std::wstring(systemDir)+L"\\"+name;
        DWORD attr=GetFileAttributesW(path.c_str());
        if(attr==INVALID_FILE_ATTRIBUTES || (attr&FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"[T81-VAN-NATIVE] missing System32 component %s",name);return false;
        }
    }
    std::wstring van=std::wstring(systemDir)+L"\\van.dll";
    HMODULE dll=LoadLibraryExW(van.c_str(),nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!dll) {Wh_Log(L"[T81-VAN-NATIVE] load failed error=%lu",GetLastError());return false;}
    // Ensure the loader did not resolve an already-loaded private VAN image.
    wchar_t actual[MAX_PATH]{};
    DWORD len=GetModuleFileNameW(dll,actual,(DWORD)std::size(actual));
    bool matches=len && len<std::size(actual) && _wcsicmp(actual,van.c_str())==0;
    bool exports=GetProcAddress(dll,"ShowVAN") && GetProcAddress(dll,"HideVAN") &&
                 GetProcAddress(dll,"RunVANW");
    FreeLibrary(dll);
    if(!matches || !exports) {
        Wh_Log(L"[T81-VAN-NATIVE] System32 image or required exports unavailable");return false;
    }
    std::wstring runner=std::wstring(systemDir)+L"\\rundll32.exe";
    std::wstring args=L"\""+van+L"\",RunVAN";
    SHELLEXECUTEINFOW se{};se.cbSize=sizeof(se);se.hwnd=s?s->bar:nullptr;
    se.lpVerb=L"open";se.lpFile=runner.c_str();se.lpParameters=args.c_str();
    se.nShow=SW_SHOWNORMAL;se.fMask=SEE_MASK_NOASYNC|SEE_MASK_FLAG_NO_UI;
    if(!ShellExecuteExW(&se)) {
        Wh_Log(L"[T81-VAN-NATIVE] launch failed error=%lu",GetLastError());return false;
    }
    Wh_Log(L"[T81-VAN-NATIVE] launch accepted build=%lu; flyout visibility not confirmed",(unsigned long)os.dwBuildNumber);
    return true;
}
// Internal companion protocol verified against win7-network-flyout-recreation
// 5.2.0: its hidden TrayInfo window lives on the hotkey/flyout thread, which
// accepts WM_USER+111 with wParam=0 (toggle). Not a documented public API.
static bool RequestWin7NetworkFlyout() {
    HWND host=nullptr;
    while((host=FindWindowExW(nullptr,host,L"Win7NetFlyout_TrayInfoWnd",nullptr))) {
        DWORD pid=0;DWORD tid=GetWindowThreadProcessId(host,&pid);
        if(pid!=GetCurrentProcessId() || !tid)continue;
        RECT anchor=PrivateSystemAnchor(HIT_NETWORK);
        if(!PostThreadMessageW(tid,WM_USER+111,0,0)) {Wh_Log(L"[T81-DIAG] win7 network thread IPC failed error=%lu",GetLastError());continue;}
        s->networkAnchor=anchor;s->networkPositionAttempts=30;SetTimer(s->bar,TIMER_NETWORK_POSITION,100,nullptr);
        Wh_Log(L"[T81-DIAG] win7 network 5.2 internal toggle queued tid=%lu (display not confirmed)",tid);return true;
    }
    Wh_Log(L"[T81-DIAG] win7 network companion host absent in this Explorer process");return false;
}
static HWND FindOwnNetworkFlyout() {
    HWND h=nullptr;while((h=FindWindowExW(nullptr,h,L"Win7NetworkFlyoutSafe",nullptr))) {
        DWORD pid=0;GetWindowThreadProcessId(h,&pid);if(pid==GetCurrentProcessId() && IsWindowVisible(h))return h;
    }return nullptr;
}
struct FindVolumePopupContext {DWORD pid;HWND found;};
static BOOL CALLBACK FindVolumePopupProc(HWND h,LPARAM l) {
    auto* c=(FindVolumePopupContext*)l;DWORD pid=0;GetWindowThreadProcessId(h,&pid);
    if(pid!=c->pid || !IsWindowVisible(h) || GetWindow(h,GW_OWNER))return TRUE;
    wchar_t cls[32]{};if(!GetClassNameW(h,cls,32) || wcscmp(cls,L"#32770"))return TRUE;
    RECT r{};if(!GetWindowRect(h,&r))return TRUE;
    int width=r.right-r.left,height=r.bottom-r.top;
    // Avoid moving an existing full mixer or an unrelated process window.
    if(width<Px(35) || width>Px(250) || height<Px(100) || height>Px(700))return TRUE;
    c->found=h;return FALSE;
}
// Based on the launch technique in "Legacy (Win32) sound volume flyout"
// 2.0.0 by Anixx: SndVol -f, packed screen point, CreateProcessW and
// STARTF_FORCEOFFFEEDBACK. The -f coordinate contract is undocumented.
// We use the existing owner thread rather than an unjoined detached mod thread.
#ifdef TASKBAR81_TEST
static decltype(&CreateProcessW) sndvolCreateProcess=CreateProcessW;
#else
#define sndvolCreateProcess CreateProcessW
#endif
static bool PackVolumeAnchor(POINT pt,DWORD* packed) {
    if(!packed || pt.x < -32768 || pt.x > 32767 || pt.y < -32768 || pt.y > 32767)return false;
    *packed=(DWORD)MAKELONG((WORD)pt.x,(WORD)pt.y);return true;
}
static bool ChooseVolumeAnchor(const RECT& r,const POINT* click,POINT* result) {
    if(!result || r.right<=r.left || r.bottom<=r.top)return false;
    if(click && PtInRect(&r,*click))*result=*click;
    else *result=POINT{r.left+(r.right-r.left)/2,r.top+(r.bottom-r.top)/2};
    return true;
}
static bool LaunchSndvolAt(POINT pt,const wchar_t* source,const RECT* iconAnchor=nullptr) {
    if(!s)return false;
    DWORD tick=GetTickCount();
    if(s->lastSndvolTick && tick-s->lastSndvolTick<400) {
        Wh_Log(L"[T81-DIAG] volume launch suppressed duplicate");return true;
    }
    DWORD packed=0;if(!PackVolumeAnchor(pt,&packed)) {
        Wh_Log(L"[T81-DIAG] volume anchor out of signed 16-bit range x=%ld y=%ld",pt.x,pt.y);return false;
    }
    wchar_t dir[MAX_PATH]{};UINT n=GetSystemDirectoryW(dir,(UINT)std::size(dir));
    if(!n || n>=std::size(dir)) {Wh_Log(L"[T81-DIAG] volume system directory unavailable error=%lu",GetLastError());return false;}
    std::wstring exe=std::wstring(dir)+L"\\SndVol.exe";
    std::wstring command=L"\""+exe+L"\" -f "+std::to_wstring((unsigned long)packed);
    STARTUPINFOW startup{};startup.cb=sizeof(startup);startup.dwFlags=STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi{};
    Wh_Log(L"[T81-DIAG] volume launch source=%ls x=%ld y=%ld packed=%lu",source,pt.x,pt.y,(unsigned long)packed);
    if(!sndvolCreateProcess(exe.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&startup,&pi)) {
        Wh_Log(L"[T81-DIAG] volume CreateProcess failed error=%lu",GetLastError());return false;
    }
    if(pi.hThread)CloseHandle(pi.hThread);if(pi.hProcess)CloseHandle(pi.hProcess);
    s->lastSndvolTick=tick;s->volumeLaunchPid=pi.dwProcessId;
    s->volumeAnchor=iconAnchor && !IsRectEmpty(iconAnchor) ? *iconAnchor : RECT{pt.x,pt.y,pt.x+1,pt.y+1};
    s->volumePositionAttempts=30;
    Wh_Log(L"[T81-DIAG] volume visual anchor=%ld,%ld,%ld,%ld",
           s->volumeAnchor.left,s->volumeAnchor.top,s->volumeAnchor.right,s->volumeAnchor.bottom);
    if(s->bar && pi.dwProcessId)SetTimer(s->bar,TIMER_VOLUME_POSITION,100,nullptr);
    Wh_Log(L"[T81-DIAG] volume process accepted pid=%lu (not confirmation of position)",pi.dwProcessId);return true;
}
static bool LaunchPrivateVolume(const POINT* click) {
    if(!s || !s->bar)return false;
    RECT r{};bool popup=OverflowIconScreenRect(SystemEntry(HIT_VOLUME),&r);
    if(!popup) {
        if(!IsSystemVisible(HIT_VOLUME)) {Wh_Log(L"[T81-DIAG] volume icon has no visible anchor");return false;}
        r=SystemCell(HIT_VOLUME);
        if(IsRectEmpty(&r))return false;
        SetLastError(ERROR_SUCCESS);int mapped=MapWindowPoints(s->bar,nullptr,(POINT*)&r,2);
        if(!mapped && GetLastError()!=ERROR_SUCCESS) {Wh_Log(L"[T81-DIAG] volume screen mapping failed error=%lu",GetLastError());return false;}
    }
    POINT point{};if(!ChooseVolumeAnchor(r,click,&point))return false;
    return LaunchSndvolAt(point,popup?L"overflow":L"tray",&r);
}
// Settings URI is public Microsoft documentation; availability can vary by
// Windows SKU/policy and Wi-Fi hardware. Fall back to Network Connections.
#ifdef TASKBAR81_TEST
static decltype(&ShellExecuteExW) networkShellExecute=ShellExecuteExW;
#else
#define networkShellExecute ShellExecuteExW
#endif
static bool OpenNetworkConnectionInterface(HWND owner) {
    SHELLEXECUTEINFOW se{};se.cbSize=sizeof(se);se.hwnd=owner;se.lpVerb=L"open";
    se.nShow=SW_SHOWNORMAL;se.fMask=SEE_MASK_NOASYNC|SEE_MASK_FLAG_NO_UI;
    se.lpFile=(s && s->netKind==2)?L"ms-settings:network-wifi":L"ms-settings:network-status";
    if(networkShellExecute(&se)) {Wh_Log(L"[T81-DIAG] network settings request accepted (visibility not confirmed)");return true;}
    Wh_Log(L"[T81-DIAG] network settings request failed error=%lu; trying classic network connections",GetLastError());
    se.lpFile=L"control.exe";se.lpParameters=L"ncpa.cpl";se.hInstApp=nullptr;
    if(networkShellExecute(&se)) {Wh_Log(L"[T81-DIAG] classic network connections request accepted");return true;}
    Wh_Log(L"[T81-DIAG] network connections fallback failed error=%lu",GetLastError());
    MessageBoxW(owner,Tr(L"Impossibile aprire le connessioni di rete. Controlla il log Windhawk.",L"Unable to open network connections. Check the Windhawk log."),L"Windows 8.1 Taskbar Recreation",MB_OK|MB_ICONERROR);
    return false;
}
#ifdef TASKBAR81_TEST
static decltype(&ShellExecuteW) batteryShellExecute=ShellExecuteW;
#else
#define batteryShellExecute ShellExecuteW
#endif
static std::wstring SubstituteKeyboardKlid(const std::wstring& klid) {
    if(klid.empty())return klid;
    HKEY key=nullptr;
    if(RegOpenKeyExW(HKEY_CURRENT_USER,L"Keyboard Layout\\Substitutes",0,KEY_READ,&key)!=ERROR_SUCCESS)return klid;
    wchar_t value[64]{};DWORD size=sizeof(value)-sizeof(wchar_t);
    LSTATUS status=RegQueryValueExW(key,klid.c_str(),nullptr,nullptr,(LPBYTE)value,&size);
    RegCloseKey(key);return status==ERROR_SUCCESS && value[0]?std::wstring(value):klid;
}
static std::wstring KeyboardKlidForLayoutId(WORD layoutId) {
    if(!layoutId)return {};
    HKEY root=nullptr;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",0,KEY_READ,&root)!=ERROR_SUCCESS)return {};
    std::wstring result;
    for(DWORD index=0;;++index) {
        wchar_t subkey[256]{};DWORD length=(DWORD)std::size(subkey);
        if(RegEnumKeyExW(root,index,subkey,&length,nullptr,nullptr,nullptr,nullptr)!=ERROR_SUCCESS)break;
        HKEY item=nullptr;
        if(RegOpenKeyExW(root,subkey,0,KEY_READ,&item)!=ERROR_SUCCESS)continue;
        wchar_t id[16]{};DWORD size=sizeof(id)-sizeof(wchar_t);
        LSTATUS status=RegQueryValueExW(item,L"Layout Id",nullptr,nullptr,(LPBYTE)id,&size);
        RegCloseKey(item);
        if(status!=ERROR_SUCCESS || !id[0])continue;
        wchar_t* end=nullptr;unsigned long parsed=wcstoul(id,&end,16);
        if(end!=id && (parsed&0xffff)==layoutId) {result=subkey;break;}
    }
    RegCloseKey(root);return result;
}
static std::wstring KeyboardLayoutKlid(HKL hkl) {
    DWORD value=(DWORD)(ULONG_PTR)hkl;WORD device=HIWORD(value),lang=LOWORD(value);
    wchar_t klid[16]{};
    if((device&0xf000)==0xf000) {
        std::wstring resolved=KeyboardKlidForLayoutId(device&0x0fff);
        if(!resolved.empty())return resolved;
    }
    if(device && device!=lang && (device&0xf000)!=0xf000)
        swprintf(klid,std::size(klid),L"%04X%04X",device,lang);
    else swprintf(klid,std::size(klid),L"%08X",(UINT)lang);
    return klid;
}
static std::wstring ReadKeyboardLayoutName(const std::wstring& klid) {
    if(klid.empty())return {};
    std::wstring path=L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\"+klid;
    HKEY key=nullptr;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,path.c_str(),0,KEY_READ,&key)!=ERROR_SUCCESS)return {};
    wchar_t value[512]{};DWORD size=sizeof(value)-sizeof(wchar_t);
    LSTATUS status=RegQueryValueExW(key,L"Layout Display Name",nullptr,nullptr,(LPBYTE)value,&size);
    if(status==ERROR_SUCCESS && value[0]) {
        wchar_t resolved[512]{};
        if(SUCCEEDED(SHLoadIndirectString(value,resolved,(UINT)std::size(resolved),nullptr)) && resolved[0]) {
            RegCloseKey(key);return resolved;
        }
    }
    std::fill(std::begin(value),std::end(value),L'\0');size=sizeof(value)-sizeof(wchar_t);
    status=RegQueryValueExW(key,L"Layout Text",nullptr,nullptr,(LPBYTE)value,&size);
    RegCloseKey(key);return status==ERROR_SUCCESS && value[0]?std::wstring(value):std::wstring{};
}
static RECT LanguageIndicatorScreenRect() {
    RECT client{},cell{};
    if(!s || !s->bar)return cell;
    GetClientRect(s->bar,&client);
    cell=s->verticalActive?GetVerticalLayout(client.right,client.bottom).language:GetLayout(client.right,client.bottom).language;
    MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);return cell;
}
static UINT LanguageFlyoutDpi(HWND hwnd) {
    UINT dpi=hwnd?GetDpiForWindow(hwnd):0;return dpi?dpi:(UINT)std::max(96,Dpi());
}
static int LanguageFlyoutPx(int value,UINT dpi) {return MulDiv(value,(int)(dpi?dpi:96),96);}
static void HideLanguageFlyout() {
    if(!s)return;
    if(s->languageFlyout && IsWindow(s->languageFlyout))ShowWindow(s->languageFlyout,SW_HIDE);
    s->languageFlyoutHover=-1;s->languageFlyoutFooterHover=false;
}
static void DestroyLanguageFlyout() {
    if(!s)return;
    HWND hwnd=s->languageFlyout;s->languageFlyout=nullptr;
    if(hwnd && IsWindow(hwnd) && GetWindowThreadProcessId(hwnd,nullptr)==GetCurrentThreadId())DestroyWindow(hwnd);
    if(s->languageFlyoutClass) {
        UnregisterClassW(L"WH_Taskbar81LanguageFlyout",GetModuleHandleW(nullptr));s->languageFlyoutClass=false;
    }
    s->languageFlyoutItems.clear();s->languageFlyoutTarget=nullptr;
    s->languageFlyoutTargetPid=0;s->languageFlyoutTargetThread=0;
    g_pendingLanguageFlyoutHkl=nullptr;g_pendingLanguageFlyoutThread=0;g_pendingLanguageFlyoutAttempts=0;
    s->languageFlyoutHover=-1;s->languageFlyoutFooterHover=false;s->languageFlyoutSelected=0;
}
static std::wstring LanguageFlyoutAbbreviation(const InputLanguageInfo& info) {
    // The Windows 8.1 presentation uses a native glyph for non-Latin scripts
    // (for example, the first Tamil glyph) and a three-letter Latin language code otherwise.
    if(!info.scripts.empty() && info.scripts.find(L"Latn;")==std::wstring::npos && !info.nativeName.empty()) {
        size_t length=1;
        if(info.nativeName.size()>1 && info.nativeName[0]>=0xD800 && info.nativeName[0]<=0xDBFF &&
           info.nativeName[1]>=0xDC00 && info.nativeName[1]<=0xDFFF)length=2;
        return info.nativeName.substr(0,length);
    }
    return info.abbreviation;
}
static void PopulateLanguageFlyoutItems() {
    if(!s)return;
    s->languageFlyoutItems.clear();s->languageFlyoutSelected=0;
    auto layouts=QueryKeyboardLayouts();HKL active=ActiveInputLanguage();std::wstring activeTag=s->languageTag;
    for(HKL hkl:layouts) {
        LanguageFlyoutItem item;item.hkl=hkl;item.langId=LOWORD((ULONG_PTR)hkl);
        InputLanguageInfo info=QueryInputLanguageInfo(hkl,hkl==active?activeTag:std::wstring{});
        item.abbreviation=LanguageFlyoutAbbreviation(info);item.region=info.region;item.languageName=info.displayName;
        std::wstring klid=KeyboardLayoutKlid(hkl);
        item.layoutName=ReadKeyboardLayoutName(SubstituteKeyboardKlid(klid));
        if(item.layoutName.empty())item.layoutName=ReadKeyboardLayoutName(klid);
        if(item.languageName.empty())item.languageName=item.layoutName;
        if(item.layoutName.empty())item.layoutName=item.languageName;
        if(item.languageName.empty()) {
            wchar_t fallback[32]{};swprintf(fallback,std::size(fallback),L"HKL %08X",(DWORD)(ULONG_PTR)hkl);
            item.languageName=fallback;
        }
        s->languageFlyoutItems.push_back(std::move(item));
    }
    for(size_t i=0;i<s->languageFlyoutItems.size();++i)
        if(s->languageFlyoutItems[i].hkl==active)s->languageFlyoutSelected=i;
    s->languageFlyoutHover=-1;s->languageFlyoutFooterHover=false;
}
// The supplied Windows 8.1 photograph is the visual specification. Interaction
// follows win7-language-switcher-restorer v1.1.0, integrated on this UI thread;
// no upstream hooks or process-local ActivateKeyboardLayout fallback is included.
static COLORREF LanguageFlyoutSystemAccent() {
    DWORD color=0;BOOL opaque=FALSE;
    if(SUCCEEDED(DwmGetColorizationColor(&color,&opaque)))
        return RGB((BYTE)(color>>16),(BYTE)(color>>8),(BYTE)color);
    return GetSysColor(COLOR_HIGHLIGHT);
}
static void PaintLanguageFlyout(HWND hwnd,HDC hdc) {
    static thread_local bool painting=false;
    if(painting || !s)return;
    painting=true;struct PaintingReset {~PaintingReset(){painting=false;}} reset;
    const auto items=s->languageFlyoutItems;
    const size_t selectedIndex=s->languageFlyoutSelected;
    const int hoverIndex=s->languageFlyoutHover;
    const bool footerHover=s->languageFlyoutFooterHover;
    RECT client{};
    if(!GetClientRect(hwnd,&client))return;
    int width=client.right-client.left,height=client.bottom-client.top;
    if(width<=0 || height<=0)return;
    UINT dpi=LanguageFlyoutDpi(hwnd);
    const int itemHeight=LanguageFlyoutPx(60,dpi),badgeWidth=LanguageFlyoutPx(40,dpi);
    const int paddingX=LanguageFlyoutPx(16,dpi),borderWidth=std::max(1,LanguageFlyoutPx(2,dpi));
    const int separatorHeight=std::max(1,LanguageFlyoutPx(1,dpi));
    HDC mem=CreateCompatibleDC(hdc);if(!mem)return;
    HBITMAP bitmap=CreateCompatibleBitmap(hdc,width,height);
    if(!bitmap) {DeleteDC(mem);return;}
    HGDIOBJ oldBitmap=SelectObject(mem,bitmap);
    if(!oldBitmap || oldBitmap==HGDI_ERROR) {DeleteObject(bitmap);DeleteDC(mem);return;}
    SetBkMode(mem,TRANSPARENT);
    const COLORREF background=GetSysColor(COLOR_WINDOW),normalText=GetSysColor(COLOR_WINDOWTEXT),subText=normalText;
    const COLORREF hoverBackground=RGB(220,220,220),border=RGB(42,42,42),separator=RGB(42,42,42);
    const COLORREF selectedBackground=LanguageFlyoutSystemAccent(),selectedText=GetSysColor(COLOR_HIGHLIGHTTEXT);
    const COLORREF linkColor=GetSysColor(COLOR_HOTLIGHT);
    HBRUSH brush=CreateSolidBrush(background);FillRect(mem,&client,brush);DeleteObject(brush);
    HFONT fontAbbr=CreateFontW(LanguageFlyoutPx(18,dpi),0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    HFONT fontSubAbbr=CreateFontW(-LanguageFlyoutPx(14,dpi),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    HFONT fontTitle=CreateFontW(LanguageFlyoutPx(14,dpi),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    HFONT fontSub=CreateFontW(LanguageFlyoutPx(12,dpi),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    HFONT fontLink=CreateFontW(-LanguageFlyoutPx(14,dpi),0,0,0,FW_NORMAL,FALSE,footerHover?TRUE:FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    HFONT fontHint=CreateFontW(LanguageFlyoutPx(11,dpi),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
    int currentY=LanguageFlyoutPx(7,dpi);
    for(size_t i=0;i<items.size();++i) {
        const auto& item=items[i];RECT row{borderWidth,currentY,width-borderWidth,currentY+itemHeight};
        bool selected=i==selectedIndex,hovered=(int)i==hoverIndex;
        if(selected || hovered) {
            HBRUSH rowBrush=CreateSolidBrush(selected?selectedBackground:hoverBackground);FillRect(mem,&row,rowBrush);DeleteObject(rowBrush);
        }
        COLORREF text=selected?selectedText:normalText;
        COLORREF secondary=selected?selectedText:subText;
        if(item.region.empty()) {
            HGDIOBJ old=SelectObject(mem,fontAbbr);SetTextColor(mem,text);
            RECT badge{paddingX,currentY,badgeWidth+paddingX,currentY+itemHeight};
            DrawTextW(mem,item.abbreviation.c_str(),-1,&badge,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);SelectObject(mem,old);
        } else {
            HGDIOBJ old=SelectObject(mem,fontAbbr);SetTextColor(mem,text);
            RECT first{paddingX,currentY+LanguageFlyoutPx(6,dpi),badgeWidth+paddingX,currentY+LanguageFlyoutPx(30,dpi)};
            DrawTextW(mem,item.abbreviation.c_str(),-1,&first,DT_CENTER|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
            SelectObject(mem,fontSubAbbr);
            RECT second{paddingX,currentY+LanguageFlyoutPx(28,dpi),badgeWidth+paddingX,currentY+LanguageFlyoutPx(48,dpi)};
            DrawTextW(mem,item.region.c_str(),-1,&second,DT_CENTER|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);SelectObject(mem,old);
        }
        int rightX=LanguageFlyoutPx(75,dpi),textRight=width-paddingX;
        HGDIOBJ old=SelectObject(mem,fontTitle);SetTextColor(mem,text);
        RECT title{rightX,currentY+LanguageFlyoutPx(8,dpi),textRight,currentY+LanguageFlyoutPx(30,dpi)};
        DrawTextW(mem,item.languageName.c_str(),-1,&title,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
        SelectObject(mem,fontSub);SetTextColor(mem,secondary);
        RECT layout{rightX,currentY+LanguageFlyoutPx(30,dpi),textRight,currentY+LanguageFlyoutPx(50,dpi)};
        DrawTextW(mem,item.layoutName.c_str(),-1,&layout,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
        SelectObject(mem,old);currentY+=itemHeight;
    }
    RECT line{paddingX,currentY+LanguageFlyoutPx(10,dpi),width-paddingX,currentY+LanguageFlyoutPx(10,dpi)+separatorHeight};
    brush=CreateSolidBrush(separator);FillRect(mem,&line,brush);DeleteObject(brush);currentY+=LanguageFlyoutPx(32,dpi);
    HGDIOBJ old=SelectObject(mem,fontLink);SetTextColor(mem,linkColor);
    RECT link{paddingX,currentY,width-paddingX,currentY+LanguageFlyoutPx(22,dpi)};
    DrawTextW(mem,Tr(L"Preferenze lingua",L"Language preferences"),-1,&link,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
    SelectObject(mem,fontHint);SetTextColor(mem,subText);
    RECT hint{paddingX,currentY+LanguageFlyoutPx(24,dpi),width-paddingX,currentY+LanguageFlyoutPx(46,dpi)};
    DrawTextW(mem,Tr(L"Seleziona una lingua di input",L"Select an input language"),-1,&hint,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
    SelectObject(mem,old);
    brush=CreateSolidBrush(border);
    RECT edges[]={{0,0,width,borderWidth},{0,height-borderWidth,width,height},
                  {0,0,borderWidth,height},{width-borderWidth,0,width,height}};
    for(const RECT& edge:edges)FillRect(mem,&edge,brush);
    DeleteObject(brush);
    BitBlt(hdc,0,0,width,height,mem,0,0,SRCCOPY);
    for(HFONT font:{fontAbbr,fontSubAbbr,fontTitle,fontSub,fontLink,fontHint})if(font)DeleteObject(font);
    SelectObject(mem,oldBitmap);DeleteObject(bitmap);DeleteDC(mem);
}
static int LanguageFlyoutHit(HWND hwnd,POINT point) {
    if(!s)return -1;
    RECT client{};
    if(!GetClientRect(hwnd,&client))return -1;
    UINT dpi=LanguageFlyoutDpi(hwnd);int itemHeight=LanguageFlyoutPx(60,dpi);
    int count=(int)s->languageFlyoutItems.size(),padding=LanguageFlyoutPx(16,dpi);
    int border=std::max(1,LanguageFlyoutPx(2,dpi)),top=LanguageFlyoutPx(7,dpi);
    int width=client.right-client.left;
    if(point.x>=border && point.x<width-border && point.y>=top && point.y<top+count*itemHeight)
        return (point.y-top)/itemHeight;
    int linkY=top+count*itemHeight+LanguageFlyoutPx(32,dpi);
    if(point.y>=linkY && point.y<=linkY+LanguageFlyoutPx(22,dpi) &&
       point.x>=padding && point.x<=width-padding)return count;
    return -1;
}
static void SyncLanguageFlyoutHover(HWND hwnd) {
    if(!s || !hwnd)return;
    POINT point{};int hit=-1;
    if(GetCursorPos(&point) && ScreenToClient(hwnd,&point))hit=LanguageFlyoutHit(hwnd,point);
    int count=(int)s->languageFlyoutItems.size();
    s->languageFlyoutHover=hit>=0 && hit<count?hit:-1;
    s->languageFlyoutFooterHover=hit==count;
}
static bool RequestLanguageFlyoutLayout(size_t index) {
    if(!s || index>=s->languageFlyoutItems.size())return false;
    g_pendingLanguageFlyoutHkl=nullptr;g_pendingLanguageFlyoutThread=0;g_pendingLanguageFlyoutAttempts=0;
    HKL hkl=s->languageFlyoutItems[index].hkl;HWND target=s->languageFlyoutTarget;
    DWORD targetPid=s->languageFlyoutTargetPid,targetThread=s->languageFlyoutTargetThread;
    auto stillLoaded=QueryKeyboardLayouts();
    bool targetValid=target && IsWindow(target) && targetPid && targetThread && hkl;
    DWORD currentPid=0,currentThread=targetValid?GetWindowThreadProcessId(target,&currentPid):0;
    targetValid=targetValid && currentPid==targetPid && currentThread==targetThread;
    bool layoutLoaded=std::find(stillLoaded.begin(),stillLoaded.end(),hkl)!=stillLoaded.end();
    if(!targetValid || !layoutLoaded) {
        Wh_Log(L"[T81-DIAG] Metro input language validation failed target=%p pid=%lu tid=%lu hkl=%p targetValid=%d layoutLoaded=%d currentPid=%lu currentTid=%lu",
            target,targetPid,targetThread,hkl,targetValid?1:0,layoutLoaded?1:0,currentPid,currentThread);
        return false;
    }
    // Windows 8+ documents system-wide activation here, provided this process
    // owns the focused window. The flyout is still foreground at this point.
    bool ownsFocus=GetForegroundWindow()==s->languageFlyout && IsWindowVisible(s->languageFlyout);
    std::wstring klid=KeyboardLayoutKlid(hkl);
    if(ownsFocus && !klid.empty()) {
        SetLastError(ERROR_SUCCESS);
        HKL loaded=LoadKeyboardLayoutW(klid.c_str(),KLF_ACTIVATE);
        DWORD loadError=loaded?ERROR_SUCCESS:GetLastError();
        HKL targetLayout=GetKeyboardLayout(targetThread),ownLayout=GetKeyboardLayout(0);
        Wh_Log(L"[T81-DIAG] Metro input language LoadKeyboardLayout target=%p tid=%lu hkl=%p klid=%ls loaded=%p error=%lu targetLayout=%p ownLayout=%p",
            target,targetThread,hkl,klid.c_str(),loaded,loadError,targetLayout,ownLayout);
        if(loaded && (targetLayout==hkl ||
           (targetThread==GetCurrentThreadId() && ownLayout==hkl))) {
            RefreshLanguageIndicator();
            Wh_Log(L"[T81-DIAG] Metro input language activation confirmed hkl=%p",hkl);
            return true;
        }
    } else {
        Wh_Log(L"[T81-DIAG] Metro input language direct activation skipped ownsFocus=%d klid=%ls",ownsFocus?1:0,klid.c_str());
    }
    HWND recipient=target;
    GUITHREADINFO info{};info.cbSize=sizeof(info);
    if(GetGUIThreadInfo(targetThread,&info)) {
        HWND focus=info.hwndFocus?info.hwndFocus:info.hwndActive;
        DWORD focusPid=0,focusThread=focus?GetWindowThreadProcessId(focus,&focusPid):0;
        if(focus && focusThread==targetThread && focusPid==targetPid)recipient=focus;
    }
    SetLastError(ERROR_SUCCESS);
    BOOL posted=PostMessageW(recipient,WM_INPUTLANGCHANGEREQUEST,0,(LPARAM)hkl);
    DWORD focusError=posted?ERROR_SUCCESS:GetLastError();HWND postedTo=recipient;
    if(!posted && recipient!=target) {
        Wh_Log(L"[T81-DIAG] Metro input language focus post failed recipient=%p target=%p pid=%lu tid=%lu error=%lu; retrying top-level",
            recipient,target,targetPid,targetThread,focusError);
        SetLastError(ERROR_SUCCESS);
        posted=PostMessageW(target,WM_INPUTLANGCHANGEREQUEST,0,(LPARAM)hkl);postedTo=target;
    }
    if(!posted) {
        DWORD error=GetLastError();
        Wh_Log(L"[T81-DIAG] Metro input language top-level post failed target=%p pid=%lu tid=%lu hkl=%p error=%lu%ls",
            target,targetPid,targetThread,hkl,error,
            error==ERROR_ACCESS_DENIED?L" (possible UIPI block or elevated target)":L"");
        return false;
    }
    g_pendingLanguageFlyoutHkl=hkl;g_pendingLanguageFlyoutThread=targetThread;g_pendingLanguageFlyoutAttempts=0;
    Wh_Log(L"[T81-DIAG] Metro input language change posted; awaiting confirmation recipient=%p hkl=%p tid=%lu",postedTo,hkl,targetThread);
    SetTimer(s->bar,TIMER_LANGUAGE_REFRESH,150,nullptr);return true;
}
static void ConfirmLanguageFlyoutRequest() {
    if(!s || !g_pendingLanguageFlyoutHkl)return;
    HKL expected=g_pendingLanguageFlyoutHkl;DWORD thread=g_pendingLanguageFlyoutThread;
    HKL observed=thread?GetKeyboardLayout(thread):nullptr;
    if(observed==expected) {
        Wh_Log(L"[T81-DIAG] Metro input language posted change confirmed hkl=%p tid=%lu",expected,thread);
        g_pendingLanguageFlyoutHkl=nullptr;g_pendingLanguageFlyoutThread=0;g_pendingLanguageFlyoutAttempts=0;
        return;
    }
    ++g_pendingLanguageFlyoutAttempts;
    if(g_pendingLanguageFlyoutAttempts<10) {
        if(s->bar)SetTimer(s->bar,TIMER_LANGUAGE_REFRESH,100,nullptr);
        return;
    }
    Wh_Log(L"[T81-DIAG] Metro input language posted change timeout expected=%p observed=%p tid=%lu",expected,observed,thread);
    g_pendingLanguageFlyoutHkl=nullptr;g_pendingLanguageFlyoutThread=0;g_pendingLanguageFlyoutAttempts=0;
    RECT cell=LanguageIndicatorScreenRect();
    ShowInfoTooltip(Tr(L"Impossibile cambiare la lingua di input. Controlla il log Windhawk.",L"Unable to change the input language. Check the Windhawk log."),(cell.left+cell.right)/2);
}
static ULONGLONG g_languageDismissTick=0;
static void MarkLanguageDismissByClick() {
    POINT p{};RECT cell=LanguageIndicatorScreenRect();
    if((GetAsyncKeyState(VK_LBUTTON)&0x8000) && GetCursorPos(&p) && PtInRect(&cell,p))
        g_languageDismissTick=GetTickCount64();
}
static bool ConsumeLanguageDismissClick() {
    bool result=g_languageDismissTick && GetTickCount64()-g_languageDismissTick<1000;
    g_languageDismissTick=0;return result;
}
static void RedrawLanguageFlyout(HWND hwnd) {
    if(hwnd)RedrawWindow(hwnd,nullptr,nullptr,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_NOCHILDREN);
}
static int LanguageFlyoutHitTestResult(HWND hwnd,LPARAM lp) {
    return LanguageFlyoutHit(hwnd,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)});
}
static LRESULT CALLBACK LanguageFlyoutProc(HWND hwnd,UINT message,WPARAM wp,LPARAM lp) {
    try {
        switch(message) {
        case WM_PAINT: {
            PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
            if(dc) {PaintLanguageFlyout(hwnd,dc);EndPaint(hwnd,&ps);}return 0;
        }
        case WM_ERASEBKGND:return 1;
        case WM_MOUSEMOVE: {
            int hit=LanguageFlyoutHitTestResult(hwnd,lp),count=s?(int)s->languageFlyoutItems.size():0;
            int hover=hit>=0 && hit<count?hit:-1;bool footer=hit==count;
            if(s && (hover!=s->languageFlyoutHover || footer!=s->languageFlyoutFooterHover)) {
                s->languageFlyoutHover=hover;s->languageFlyoutFooterHover=footer;RedrawLanguageFlyout(hwnd);
            }
            TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,hwnd,0};TrackMouseEvent(&track);return 0;
        }
        case WM_MOUSELEAVE:
            if(s) {s->languageFlyoutHover=-1;s->languageFlyoutFooterHover=false;RedrawLanguageFlyout(hwnd);}return 0;
        case WM_DWMCOLORIZATIONCOLORCHANGED:RedrawLanguageFlyout(hwnd);return 0;
        case WM_SETCURSOR:
            if(s && s->languageFlyoutFooterHover) {SetCursor(LoadCursorW(nullptr,IDC_HAND));return TRUE;}
            break;
        case WM_LBUTTONUP: {
            if(!s)return 0;
            int hit=LanguageFlyoutHitTestResult(hwnd,lp),count=(int)s->languageFlyoutItems.size();
            if(hit>=0 && hit<count) {
                bool requested=RequestLanguageFlyoutLayout((size_t)hit);
                HideLanguageFlyout();
                if(!requested) {
                    RECT cell=LanguageIndicatorScreenRect();
                    ShowInfoTooltip(Tr(L"Impossibile cambiare la lingua di input. Controlla il log Windhawk.",L"Unable to change the input language. Check the Windhawk log."),(cell.left+cell.right)/2);
                }
            } else if(hit==count) {
                HideLanguageFlyout();
                HINSTANCE opened=ShellExecuteW(nullptr,L"open",L"ms-settings:regionlanguage",nullptr,nullptr,SW_SHOWNORMAL);
                if((INT_PTR)opened<=32) {
                    RECT cell=LanguageIndicatorScreenRect();
                    ShowInfoTooltip(Tr(L"Impossibile aprire le preferenze lingua. Controlla il log Windhawk.",L"Unable to open language preferences. Check the Windhawk log."),(cell.left+cell.right)/2);
                }
            } else {
                HideLanguageFlyout();
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if(!s || s->languageFlyoutItems.empty())return 0;
            size_t count=s->languageFlyoutItems.size();
            if(wp==VK_ESCAPE) {HideLanguageFlyout();return 0;}
            if(wp==VK_DOWN || wp==VK_TAB) {s->languageFlyoutSelected=(s->languageFlyoutSelected+1)%count;RedrawLanguageFlyout(hwnd);return 0;}
            if(wp==VK_UP) {s->languageFlyoutSelected=(s->languageFlyoutSelected+count-1)%count;RedrawLanguageFlyout(hwnd);return 0;}
            if(wp==VK_RETURN || wp==VK_SPACE) {
                size_t selected=s->languageFlyoutSelected;
                bool requested=RequestLanguageFlyoutLayout(selected);HideLanguageFlyout();
                if(!requested) {
                    RECT cell=LanguageIndicatorScreenRect();
                    ShowInfoTooltip(Tr(L"Impossibile cambiare la lingua di input. Controlla il log Windhawk.",L"Unable to change the input language. Check the Windhawk log."),(cell.left+cell.right)/2);
                }
                return 0;
            }
            break;
        }
        case WM_ACTIVATE:if(LOWORD(wp)==WA_INACTIVE){MarkLanguageDismissByClick();HideLanguageFlyout();}return 0;
        case WM_CANCELMODE:case WM_CLOSE:HideLanguageFlyout();return 0;
        case WM_DESTROY:if(s && s->languageFlyout==hwnd)s->languageFlyout=nullptr;return 0;
        }
    } catch(...) {
        Wh_Log(L"[T81-RECOVERY] language flyout C++ exception");
        if(showRecoverableErrors.load()) {try {HideLanguageFlyout();
            RECT bar{};
            if(s && s->bar && GetWindowRect(s->bar,&bar))ShowInfoTooltip(Tr(L"Errore recuperabile nel flyout lingua. Controlla il log Windhawk.",L"Recoverable language flyout error. Check the Windhawk log."),(bar.left+bar.right)/2);
            if(s && s->tooltip && IsWindowVisible(s->tooltip))s->recoveryTooltipUntil=GetTickCount64()+5000;
            } catch(...) {}}
    }
    return DefWindowProcW(hwnd,message,wp,lp);
}
static bool ShowSystemFloatingLanguageBar() {
    HINSTANCE result=ShellExecuteW(s?s->bar:nullptr,L"open",
        L"shell:::{62D8ED13-C9D0-4CE8-A914-47DD628FB1B0}",nullptr,nullptr,SW_SHOWNORMAL);
    INT_PTR code=reinterpret_cast<INT_PTR>(result);
    Wh_Log(L"[T81-LANGUAGE] language shell CLSID result=%Id",code);
    return code>32;
}
static void ShowWin7LanguageMenu(HWND target,DWORD targetPid,DWORD targetThread) {
    if(!s || !s->bar)return;
    const auto items=s->languageFlyoutItems;
    if(items.empty())return;
    HMENU menu=CreatePopupMenu();if(!menu)return;
    struct MenuGuard {HMENU h;~MenuGuard(){if(h)DestroyMenu(h);}} guard{menu};
    HKL active=s->languageHkl;
    for(size_t i=0;i<items.size();++i) {
        wchar_t code[16]{};
        GetLocaleInfoW(MAKELCID(items[i].langId,SORT_DEFAULT),LOCALE_SISO639LANGNAME,code,(int)std::size(code));
        std::wstring abbr=code[0]?std::wstring(code):items[i].abbreviation;
        for(auto& ch:abbr)ch=(wchar_t)towupper(ch);
        std::wstring raw=abbr+L"  "+items[i].languageName,label;
        for(wchar_t ch:raw) {label+=ch;if(ch==L'&')label+=L'&';}
        AppendMenuW(menu,MF_STRING | (items[i].hkl==active?MF_CHECKED:0),1000+(UINT)i,label.c_str());
    }
    AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
    constexpr UINT showBar=60000;
    AppendMenuW(menu,MF_STRING,showBar,Tr(L"Mostra la barra della lingua",L"Show the Language bar"));
    RECT cell=LanguageIndicatorScreenRect();
    HidePreview();HidePrivateOverflow();PeekOff(false);
    HWND owner=w7t::GetMenuOwnerWindow();if(!owner)return;
    UINT flags=TPM_RETURNCMD|TPM_NONOTIFY|TPM_RIGHTBUTTON;
    int x=cell.left,y=cell.top;
    if(s->verticalActive) {x=s->verticalSide==1?cell.right:cell.left;y=cell.top;flags|=s->verticalSide==1?TPM_LEFTALIGN:TPM_RIGHTALIGN;}
    else if(s->nativeEdge==ABE_TOP) {y=cell.bottom;flags|=TPM_LEFTALIGN|TPM_TOPALIGN;}
    else flags|=TPM_LEFTALIGN|TPM_BOTTOMALIGN;
    UINT chosen=0;
    {w7t::ForegroundMenuScope scope(owner);chosen=TrackPopupMenuEx(menu,flags,x,y,owner,nullptr);}
    if(!chosen)MarkLanguageDismissByClick();
    if(target && IsWindow(target) && GetForegroundWindow()==owner)SetForegroundWindow(target);
    if(chosen==showBar) {
        if(!ShowSystemFloatingLanguageBar())
            ShowInfoTooltip(Tr(L"Impossibile mostrare la barra della lingua di sistema. Controlla le impostazioni di input di Windows e il log Windhawk.",L"Unable to show the system language bar. Check Windows input settings and the Windhawk log."),(cell.left+cell.right)/2);
    } else if(chosen>=1000 && chosen<1000+items.size()) {
        HKL selected=items[chosen-1000].hkl;
        PopulateLanguageFlyoutItems();
        s->languageFlyoutTarget=target;s->languageFlyoutTargetPid=targetPid;s->languageFlyoutTargetThread=targetThread;
        auto it=std::find_if(s->languageFlyoutItems.begin(),s->languageFlyoutItems.end(),[&](const LanguageFlyoutItem& item){return item.hkl==selected;});
        bool requested=it!=s->languageFlyoutItems.end() && RequestLanguageFlyoutLayout((size_t)(it-s->languageFlyoutItems.begin()));
        if(!requested)ShowInfoTooltip(Tr(L"Impossibile cambiare la lingua di input. Controlla il log Windhawk.",L"Unable to change the input language. Check the Windhawk log."),(cell.left+cell.right)/2);
    }
}
static void ShowLanguageIndicatorMenu() {
    if(!s || !s->bar || !LanguageIndicatorVisible())return;
    if(s->languageFlyout && IsWindowVisible(s->languageFlyout)) {HideLanguageFlyout();return;}
    HWND target=InputLanguageTarget();DWORD targetPid=0;
    DWORD targetThread=target?GetWindowThreadProcessId(target,&targetPid):0;
    RefreshLanguageIndicator();if(!LanguageIndicatorVisible())return;
    PopulateLanguageFlyoutItems();if(s->languageFlyoutItems.empty())return;
    if(taskbarSkin.load()==1) {ShowWin7LanguageMenu(target,targetPid,targetThread);return;}
    if(!s->languageFlyout) {
        if(!s->languageFlyoutClass) {
            WNDCLASSW wc{};wc.lpfnWndProc=LanguageFlyoutProc;wc.hInstance=GetModuleHandleW(nullptr);
            wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.lpszClassName=L"WH_Taskbar81LanguageFlyout";
            if(!RegisterClassW(&wc)) {Wh_Log(L"[T81-DIAG] Metro language flyout class registration failed error=%lu",GetLastError());return;}
            s->languageFlyoutClass=true;
        }
        s->languageFlyout=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,L"WH_Taskbar81LanguageFlyout",L"Windows 8.1 Language Switcher",
            WS_POPUP|WS_CLIPCHILDREN,0,0,100,100,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        if(!s->languageFlyout)return;
        // DWMWCP_DONOTROUND: the supplied Windows 8.1 window has square corners.
        DWORD corners=1;DwmSetWindowAttribute(s->languageFlyout,33,&corners,sizeof(corners));
    }
    s->languageFlyoutTarget=target;s->languageFlyoutTargetPid=targetPid;s->languageFlyoutTargetThread=targetThread;
    RECT cell=LanguageIndicatorScreenRect();MONITORINFO monitor{};monitor.cbSize=sizeof(monitor);
    if(!GetMonitorInfoW(MonitorFromRect(&cell,MONITOR_DEFAULTTONEAREST),&monitor))return;
    UINT dpi=LanguageFlyoutDpi(s->languageFlyout);
    int width=LanguageFlyoutPx(304,dpi),height=(int)s->languageFlyoutItems.size()*LanguageFlyoutPx(60,dpi)+LanguageFlyoutPx(94,dpi),gap=0;
    int x=monitor.rcWork.right-width-gap,y=monitor.rcWork.bottom-height-gap;
    if(s->verticalActive)x=s->verticalSide==1?cell.right+gap:cell.left-width-gap;
    else if(s->nativeEdge==ABE_TOP)y=cell.bottom+gap;else y=cell.top-height-gap;
    x=std::max((int)monitor.rcMonitor.left,std::min(x,(int)monitor.rcMonitor.right-width));
    y=std::max((int)monitor.rcMonitor.top,std::min(y,(int)monitor.rcMonitor.bottom-height));
    HidePreview();HidePrivateOverflow();PeekOff(false);
    SetWindowPos(s->languageFlyout,HWND_TOPMOST,x,y,width,height,SWP_SHOWWINDOW);
    ShowWindow(s->languageFlyout,SW_SHOW);
    if(!SetForegroundWindow(s->languageFlyout))
        Wh_Log(L"[T81-DIAG] Metro language flyout foreground activation was rejected");
    SetActiveWindow(s->languageFlyout);SetFocus(s->languageFlyout);
    RedrawLanguageFlyout(s->languageFlyout);
}
static void LaunchSystemTrayHit(int hit,const POINT* click) {
    if(hit==HIT_TRAY_MORE) { ShowPrivateOverflow(); return; }
    if(hit==HIT_LANGUAGE) { ShowLanguageIndicatorMenu(); return; }
    if(hit==HIT_CLOCK) {
        if(!preferClassicClock.load() || !OpenClassicClock()) ShellExecuteW(s->bar,L"open",L"ms-settings:dateandtime",nullptr,nullptr,SW_SHOWNORMAL);
    } else if(hit==HIT_VOLUME) {
        if(!LaunchPrivateVolume(click)) {
            RECT cell=SystemCell(HIT_VOLUME);MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
            ShowInfoTooltip(Tr(L"Impossibile aprire il volume. Controlla il log Windhawk.",L"Unable to open volume. Check the Windhawk log."),(cell.left+cell.right)/2);
        }
    } else if(hit==HIT_NETWORK) {
        if(!RequestWin7NetworkFlyout() && !RequestWin8NetworkFlyout() && !RequestNativeWin8NetworkFlyout()) OpenNetworkConnectionInterface(s->bar);
    } else if(hit==HIT_BATTERY) {
        if(!TryOriginalBatteryFlyout1511()) {
            RECT cell{};
                if(!OverflowIconScreenRect(SystemEntry(HIT_BATTERY),&cell)) {cell=SystemCell(HIT_BATTERY);
                MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
                }
            if(useOriginalBatteryFlyout.load()) {
                Wh_Log(L"[T81-DIAG] original battery flyout unavailable; requesting Settings fallback");
                ShowInfoTooltip(Tr(L"Flyout batteria non disponibile: apro Impostazioni. Controlla il log Windhawk.",L"Battery flyout unavailable: opening Settings. Check the Windhawk log."),(cell.left+cell.right)/2);
            }
            HINSTANCE opened=batteryShellExecute(s->bar,L"open",L"ms-settings:batterysaver",nullptr,nullptr,SW_SHOWNORMAL);
            if((INT_PTR)opened<=32) {
                Wh_Log(L"[T81-DIAG] battery Settings fallback failed code=%Id",(INT_PTR)opened);
                ShowInfoTooltip(Tr(L"Impossibile aprire Impostazioni batteria. Controlla il log Windhawk.",L"Unable to open battery Settings. Check the Windhawk log."),(cell.left+cell.right)/2);
            }
        }
    }
}

static HWND gWin7OrbWindow=nullptr;
static bool gWin7OrbReady=false;
static int VerticalHit(int x,int y);
int Hit(int x,int y) {
    if(s && s->verticalActive)return VerticalHit(x,y);
    if(s && gWin7OrbReady && gWin7OrbWindow && IsWindowVisible(gWin7OrbWindow)) {
        POINT screen{x,y};RECT orb{};
        if(ClientToScreen(s->bar,&screen) && GetWindowRect(gWin7OrbWindow,&orb) && PtInRect(&orb,screen))return HIT_START;
    }
    RECT r{}; GetClientRect(s->bar,&r); auto L=GetLayout(r.right,r.bottom); POINT p{x,y};
    if (PtInRect(&L.start,p) && (!startHotCorner.load() || y>=r.bottom-std::max(1,Px(2)))) return HIT_START;
    if (PtInRect(&L.desktop,p)) return HIT_DESKTOP;
    if (PtInRect(&L.overflow,p)) return HIT_OVERFLOW;
    if (PtInRect(&L.desktopBand,p)) return HIT_BAND_DESKTOP;
    if (PtInRect(&L.linksBand,p)) return HIT_BAND_LINKS;
    if (PtInRect(&L.addressBand,p)) return HIT_BAND_ADDRESS;
    if (s->privateActive && PtInRect(&L.language,p)) return HIT_LANGUAGE;
    if (s->privateActive && PtInRect(&L.clock,p)) return HIT_CLOCK;
    if (s->privateActive && PtInRect(&L.tray,p)) return HitPrivateArea(p,L);
    if (x>=L.first && x<L.first+L.count*L.width && y>=0 && y<r.bottom) return (x-L.first)/L.width;
    return HIT_NONE;
}
Color Mix(Color a, Color b, int pct) {
    auto f=[pct](int x,int y){return (BYTE)((x*(100-pct)+y*pct)/100);};
    return Color(255,f(a.GetR(),b.GetR()),f(a.GetG(),b.GetG()),f(a.GetB(),b.GetB()));
}
Color AccentBase() {
    if (!useAccent.load()) return Color(255, 15, 74, 121);
    DWORD c=0; BOOL opaque=FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c,&opaque)))
        return Color(255,(BYTE)(c>>16),(BYTE)(c>>8),(BYTE)c);
    return Color(255,15,74,121);
}

// Retain positions by stable group key, so both the moving button and its
// neighbours animate continuously even when the backing vector is reordered.
static std::vector<int> AnimatedGroupPositions(const Layout& layout) {
    std::vector<int> positions;
    if(!s) return positions;
    positions.reserve(layout.count);
    ULONGLONG now=GetTickCount64();
    ULONGLONG elapsed=s->animationTick && now>=s->animationTick?now-s->animationTick:16;
    s->animationTick=now;
    float fraction=1.0f-std::exp(-(float)std::min<ULONGLONG>(elapsed,80)/85.0f);
    int source=-1;
    if(s->dragActive) for(int i=0;i<layout.count;i++)
        if(s->groups[i].key==s->dragKey) { source=i; break; }
    int blockFirst=source,blockEnd=source<0?source:source+1;
    std::vector<int> slots((size_t)layout.count);for(int i=0;i<layout.count;++i)slots[i]=i;
    if(source>=0 && s->dragInsertion>=0) {
        const auto& base=GroupBase(s->groups[source]);
        while(blockFirst>0 && GroupBase(s->groups[blockFirst-1])==base)--blockFirst;
        while(blockEnd<layout.count && GroupBase(s->groups[blockEnd])==base)++blockEnd;
        int pos=std::max(0,std::min(s->dragInsertion,layout.count));
        if(pos>0 && pos<layout.count && GroupBase(s->groups[pos-1])==GroupBase(s->groups[pos])) {
            const auto& targetBase=GroupBase(s->groups[pos]);int lo=pos,hi=pos;
            while(lo>0 && GroupBase(s->groups[lo-1])==targetBase)--lo;
            while(hi<layout.count && GroupBase(s->groups[hi])==targetBase)++hi;
            pos=(pos-lo<hi-pos)?lo:hi;
        }
        std::vector<int> sequence;for(int i=0;i<layout.count;i++)if(i<blockFirst || i>=blockEnd)sequence.push_back(i);
        int insert=0;for(int i=0;i<pos;i++)if(i<blockFirst || i>=blockEnd)++insert;
        for(int i=blockFirst;i<blockEnd;i++)sequence.insert(sequence.begin()+insert++,i);
        for(int i=0;i<layout.count;i++)slots[sequence[i]]=i;
    }
    bool pending=false;
    for(int i=0;i<layout.count;i++) {
        bool inDraggedBlock=source>=0 && i>=blockFirst && i<blockEnd;
        float target=(float)(layout.first+slots[i]*layout.width);
        if(inDraggedBlock) {
            int blockWidth=(blockEnd-blockFirst)*layout.width;
            int left=s->dragCursorX-s->dragGrabOffset-(source-blockFirst)*layout.width;
            left=std::max(layout.first,std::min(left,(int)layout.clock.left-blockWidth));
            target=(float)(left+(i-blockFirst)*layout.width);
        }
        auto found=std::find_if(s->visualPositions.begin(),s->visualPositions.end(),
            [&](const VisualPosition& v){return v.key==s->groups[i].key;});
        if(found==s->visualPositions.end()) {
            s->visualPositions.push_back({s->groups[i].key,(float)(layout.first+i*layout.width)});
            found=s->visualPositions.end()-1;
        }
        if(!animateReorder.load() || inDraggedBlock) found->x=target;
        else {
            found->x+=(target-found->x)*fraction;
            if(std::abs(found->x-target)<0.5f) found->x=target;
            else pending=true;
        }
        positions.push_back((int)std::lround(found->x));
    }
    s->visualPositions.erase(std::remove_if(s->visualPositions.begin(),s->visualPositions.end(),
        [](const VisualPosition& v) {
            return std::none_of(s->groups.begin(),s->groups.end(),
                                [&](const Group& g){return g.key==v.key;});
        }),s->visualPositions.end());
    if(s->bar) {
        if(pending) SetTimer(s->bar,TIMER_ANIMATION,16,nullptr);
        else KillTimer(s->bar,TIMER_ANIMATION);
    }
    return positions;
}

static void AdvanceHoverFade() {
    if(!s) return;
    ULONGLONG now=GetTickCount64();
    float step=s->fadeTick?std::min(80.0f,(float)(now-s->fadeTick))/140.0f:0;
    s->fadeTick=now; bool pending=false;
    auto advance=[&](float& value,float target) {
        if(value<target) value=std::min(target,value+step);
        else if(value>target) value=std::max(target,value-step);
        if(std::fabs(value-target)>0.001f) pending=true;
    };
    if(taskbarSkin.load()==1) {
        // Windows7.xaml uses 180 ms hover-in and 350 ms hover-out.
        float elapsed=step*140.f;float target=s->hot==HIT_START?1.f:0.f;
        if(target>0.f)s->startFade=std::min(1.f,s->startFade+elapsed/180.f);
        else s->startFade=std::max(0.f,s->startFade-elapsed/350.f);
        if(std::fabs(s->startFade-target)>0.001f)pending=true;
    } else advance(s->startFade,s->hot==HIT_START?1.0f:0.0f);
    for(size_t i=0;i<s->groups.size();++i) {
        auto it=std::find_if(s->hoverFades.begin(),s->hoverFades.end(),[&](const HoverFade& h){return h.key==s->groups[i].key;});
        if(it==s->hoverFades.end()) { s->hoverFades.push_back({s->groups[i].key,0}); it=s->hoverFades.end()-1; }
        bool hot=(s->hot==(int)i)||(s->dragActive && s->dragKey==s->groups[i].key);
        advance(it->value,hot?1.0f:0.0f);
    }
    s->hoverFades.erase(std::remove_if(s->hoverFades.begin(),s->hoverFades.end(),[&](const HoverFade& h) {
        return std::none_of(s->groups.begin(),s->groups.end(),[&](const Group& g){return g.key==h.key;});
    }),s->hoverFades.end());
    if(s->bar) { if(pending) SetTimer(s->bar,TIMER_FADE,16,nullptr); else KillTimer(s->bar,TIMER_FADE); }
}
static float GroupHoverFade(int index) {
    if(!s || index<0 || (size_t)index>=s->groups.size()) return 0;
    for(const auto& h:s->hoverFades) if(h.key==s->groups[index].key) return h.value;
    return 0;
}
static BYTE EffectiveBarBackgroundOpacity();
static void PaintTaskbarBackground(Graphics& g, int width, int height);
// 32-bit top-down surfaces. The black/white passes reconstruct coverage of
// GDI icons and text as well as GDI+ elements; this avoids treating the whole
// taskbar as one translucent layer. ClearType is disabled for these passes.
struct AlphaSurface {
    HDC dc=nullptr; HBITMAP bmp=nullptr; HGDIOBJ old=nullptr; BYTE* bits=nullptr;
    AlphaSurface(HDC ref,int w,int h) {
        BITMAPINFO bi{}; bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth=w; bi.bmiHeader.biHeight=-h; bi.bmiHeader.biPlanes=1;
        bi.bmiHeader.biBitCount=32; bi.bmiHeader.biCompression=BI_RGB;
        dc=CreateCompatibleDC(ref);
        if(dc) bmp=CreateDIBSection(ref,&bi,DIB_RGB_COLORS,(void**)&bits,nullptr,0);
        if(bmp) old=SelectObject(dc,bmp);
    }
    bool ok() const { return dc && bmp && old && old!=HGDI_ERROR && bits; }
    ~AlphaSurface() { if(dc && old && old!=HGDI_ERROR) SelectObject(dc,old); if(bmp) DeleteObject(bmp); if(dc) DeleteDC(dc); }
    AlphaSurface(const AlphaSurface&)=delete;
    AlphaSurface& operator=(const AlphaSurface&)=delete;
};
static const wchar_t kWin7OrbClass[]=L"WH_Taskbar81Win7OrbSurface";
static LRESULT CALLBACK Win7OrbProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    if(msg==WM_MOUSEACTIVATE)return MA_NOACTIVATE;
    if(msg==WM_NCDESTROY) {if(gWin7OrbWindow==h){gWin7OrbWindow=nullptr;gWin7OrbReady=false;}return DefWindowProcW(h,msg,wp,lp);}
    if(!s || !s->bar || !IsWindow(s->bar))return DefWindowProcW(h,msg,wp,lp);
    if(msg==WM_MOUSEMOVE || msg==WM_LBUTTONDOWN || msg==WM_LBUTTONUP || msg==WM_LBUTTONDBLCLK || msg==WM_RBUTTONUP) {
        POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};ClientToScreen(h,&p);ScreenToClient(s->bar,&p);
        if(msg==WM_MOUSEMOVE) {TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,h,0};TrackMouseEvent(&track);}
        return SendMessageW(s->bar,msg,wp,MAKELPARAM(p.x,p.y));
    }
    if(msg==WM_MOUSELEAVE) {
        POINT p{};RECT bar{},orb{};
        if(GetCursorPos(&p) && GetWindowRect(s->bar,&bar) && GetWindowRect(h,&orb) &&
           (PtInRect(&bar,p) || PtInRect(&orb,p)))return 0;
        return SendMessageW(s->bar,WM_MOUSELEAVE,0,0);
    }
    return DefWindowProcW(h,msg,wp,lp);
}
#ifdef TASKBAR81_TEST
static bool testForceLayeredFailure=false, testForcePreviewLayeredFailure=false;
static int testBarFrames=0, testPreviewFrames=0;
#endif
static BYTE BlendByte(int fg,int bg,int coverage) {
    return (BYTE)std::min(255,fg+(bg*(255-coverage)+127)/255);
}
static void ComposeForeground(BYTE* bg,const BYTE* black,const BYTE* white,size_t pixels,BYTE backgroundAlpha) {
    for(size_t i=0;i<pixels;i++) {
        BYTE* d=bg+i*4; const BYTE* k=black+i*4; const BYTE* w=white+i*4;
        // GDI RGB can have per-channel edge coverage. Choose the maximum
        // coverage and clamp premultiplied channels so RGB never exceeds A.
        int a=0;
        for(int c=0;c<3;c++) a=std::max(a,255-std::max(0,(int)w[c]-(int)k[c]));
        for(int c=0;c<3;c++) {
            int fg=std::min(a,(int)k[c]);
            int base=((int)d[c]*backgroundAlpha+127)/255;
            d[c]=BlendByte(fg,base,a);
        }
        d[3]=BlendByte(a,backgroundAlpha,a);
    }
}
static void HideWin7OrbSurface() {
    gWin7OrbReady=false;
    if(gWin7OrbWindow && IsWindow(gWin7OrbWindow) && IsWindowVisible(gWin7OrbWindow))ShowWindow(gWin7OrbWindow,SW_HIDE);
}
static bool UpdateWin7OrbSurface(HWND bar,int cellWidth,int barHeight) {
    if(!s || taskbarSkin.load()!=1 || startHotCorner.load() || s->verticalActive ||
       barHeight>=Px(38) || cellWidth<=0 || (!s->win7StartImage && !s->betaTexture[30]) || !IsWindowVisible(bar)) {
        HideWin7OrbSurface();return false;
    }
    if(!gWin7OrbWindow || !IsWindow(gWin7OrbWindow)) {
        WNDCLASSW wc{};wc.lpfnWndProc=Win7OrbProc;wc.hInstance=GetModuleHandleW(nullptr);
        wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.lpszClassName=kWin7OrbClass;
        if(!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) {HideWin7OrbSurface();return false;}
        WNDCLASSW current{};
        if(!GetClassInfoW(wc.hInstance,kWin7OrbClass,&current) || current.lpfnWndProc!=Win7OrbProc)return false;
        gWin7OrbWindow=CreateWindowExW(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST,
            kWin7OrbClass,L"",WS_POPUP,0,0,1,1,bar,nullptr,wc.hInstance,nullptr);
        if(!gWin7OrbWindow){Wh_Log(L"[T81-ORB] popup create failed error=%lu",GetLastError());return false;}
        ExcludeFromPeek(gWin7OrbWindow);
    }
    // The small-bar sprite is slightly smaller, but still overhangs the bar.
    int side=std::max(1,Px(51));RECT barRect{};
    if(!GetWindowRect(bar,&barRect)){HideWin7OrbSurface();return false;}
    int x=barRect.left+(cellWidth-side)/2;
    // Transparent sprite margins can sit outside the monitor; the orb stays on-screen.
    int margin=std::max(1,MulDiv(side,8,54));
    int y=s->nativeEdge==ABE_TOP?barRect.top-margin:barRect.bottom-side+margin;
    HDC screen=GetDC(nullptr);if(!screen){HideWin7OrbSurface();return false;}
    bool ok=false;
    {
        AlphaSurface background(screen,side,side),black(screen,side,side),white(screen,side,side);
        if(background.ok() && black.ok() && white.ok()) {
            memset(background.bits,0,(size_t)side*side*4);
            PatBlt(black.dc,0,0,side,side,BLACKNESS);PatBlt(white.dc,0,0,side,side,WHITENESS);
            int frame=(s->startPressed && s->hot==HIT_START)?2:(s->hot==HIT_START?1:0);
            {Graphics g(black.dc);DrawWin7Start(g,side,side,frame,side);}
            {Graphics g(white.dc);DrawWin7Start(g,side,side,frame,side);}
            GdiFlush();ComposeForeground(background.bits,black.bits,white.bits,(size_t)side*side,0);
            POINT dst{x,y},src{0,0};SIZE size{side,side};BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
            ok=UpdateLayeredWindow(gWin7OrbWindow,nullptr,&dst,&size,background.dc,&src,0,&blend,ULW_ALPHA)!=FALSE;
        }
    }
    ReleaseDC(nullptr,screen);
    if(!ok){Wh_Log(L"[T81-ORB] layered update failed error=%lu",GetLastError());HideWin7OrbSurface();return false;}
    gWin7OrbReady=true;if(!IsWindowVisible(gWin7OrbWindow))ShowWindow(gWin7OrbWindow,SW_SHOWNOACTIVATE);
    return true;
}
static void DestroyWin7OrbSurface() {
    gWin7OrbReady=false;if(gWin7OrbWindow && IsWindow(gWin7OrbWindow))DestroyWindow(gWin7OrbWindow);
    gWin7OrbWindow=nullptr;UnregisterClassW(kWin7OrbClass,GetModuleHandleW(nullptr));
}
static void ConfigureBarAlpha(HWND h,bool perPixel) {
    if(!s || !h) return;
    // SLWA and ULW cannot be mixed until the layered style is reset.
    LONG_PTR style=GetWindowLongPtrW(h,GWL_EXSTYLE);
    if(s->perPixelBar!=perPixel) {
        SetWindowLongPtrW(h,GWL_EXSTYLE,style & ~((LONG_PTR)WS_EX_LAYERED));
        SetWindowLongPtrW(h,GWL_EXSTYLE,style | WS_EX_LAYERED);
    }
    s->perPixelBar=perPixel; s->perPixelFrame=false; s->legacyAlphaReady=false;
    if(!perPixel) {
        s->legacyAlphaReady=SetLayeredWindowAttributes(h,0,(BYTE)taskbarOpacity.load(),LWA_ALPHA)!=FALSE;
        if(!s->legacyAlphaReady) Wh_Log(L"[T81-DIAG] could not apply legacy bar alpha error=%lu",GetLastError());
    }
}
static void DrawActiveReferenceTile(Graphics& g,int x,int width,int height,float fade) {
    int edge=std::max(1,Px(1));
    int tileWidth=std::max(1,width-edge);
    if(tileWidth<=4*edge || height<=4*edge) return;
    // Separate gradient and borders, rather than a single faint stroke.
    int extra=(int)std::lround(12*std::max(0.f,std::min(1.f,fade)));
    LinearGradientBrush fill(Point(x,0),Point(x,height),
        Color(std::min(255,70+extra),255,255,255),Color(std::min(255,100+extra),255,255,255));
    g.FillRectangle(&fill,x+2*edge,2*edge,tileWidth-4*edge,height-4*edge);
    SolidBrush outer(Color(84,0,0,0));
    g.FillRectangle(&outer,x,0,tileWidth,edge);
    g.FillRectangle(&outer,x,height-edge,tileWidth,edge);
    g.FillRectangle(&outer,x,edge,edge,height-2*edge);
    g.FillRectangle(&outer,x+tileWidth-edge,edge,edge,height-2*edge);
    SolidBrush inner(Color(230,255,255,255));
    g.FillRectangle(&inner,x+edge,edge,tileWidth-2*edge,edge);
    g.FillRectangle(&inner,x+edge,height-2*edge,tileWidth-2*edge,edge);
    g.FillRectangle(&inner,x+edge,2*edge,edge,height-4*edge);
    g.FillRectangle(&inner,x+tileWidth-2*edge,2*edge,edge,height-4*edge);
}
// Scale vertical button artwork as nine pieces. Preserve the original four
// corners and border thickness instead of stretching them with the center.
// All coordinates are in the rotated tile's local space.
static void DrawBetaTaskTextureVertical(Graphics& g,int index,const RECT& cell) {
    if(!BetaTextureAvailable(index) || IsRectEmpty(&cell))return;
    Gdiplus::Image* image=s->betaTexture[index];
    int sw=(int)image->GetWidth(),sh=(int)image->GetHeight();
    int dw=(int)(cell.right-cell.left),dh=(int)(cell.bottom-cell.top);
    if(sw<10 || sh<10 || dw<4 || dh<4) {DrawBetaTexture(g,index,cell);return;}
    const int sourceEdge=4;
    const int dx=std::min(std::max(1,Px(sourceEdge)),dw/2);
    const int dy=std::min(std::max(1,Px(sourceEdge)),dh/2);
    int sx[4]={0,sourceEdge,sw-sourceEdge,sw};
    int sy[4]={0,sourceEdge,sh-sourceEdge,sh};
    int x[4]={(int)cell.left,(int)cell.left+dx,(int)cell.right-dx,(int)cell.right};
    int y[4]={(int)cell.top,(int)cell.top+dy,(int)cell.bottom-dy,(int)cell.bottom};
    auto saved=g.Save();
    g.SetInterpolationMode(InterpolationModeNearestNeighbor);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    for(int row=0;row<3;row++)for(int col=0;col<3;col++) {
        int width=x[col+1]-x[col],height=y[row+1]-y[row];
        if(width<=0 || height<=0)continue;
        g.DrawImage(image,Rect(x[col],y[row],width,height),sx[col],sy[row],
                    sx[col+1]-sx[col],sy[row+1]-sy[row],UnitPixel);
    }
    g.Restore(saved);
}
// Texture order: desktop, background, active, inactive, hover, pin hover,
// pin pressed, attention. Missing assets fall back to the existing renderer.
static bool DrawBetaTaskState(Graphics& g,const RECT& cell,bool running,bool active,bool hot,bool pressed,bool attention,bool vertical=false,bool multi=false) {
    // The legacy artwork maps running InactivePressed and all
    // active states to ActiveNormal; only a closed pin uses NotRunningPressed.
    (void)multi; // Group count affects stack geometry, not the front state.
    int tex=attention?7:running?(pressed||active?2:(hot?4:3)):(pressed?6:(hot?5:-1));
    if(tex<0)return BetaTextureAvailable(2); // unhovered pin has no tile
    if(!BetaTextureAvailable(tex))return false;
    if(vertical)DrawBetaTaskTextureVertical(g,tex,cell);
    else DrawBetaTexture(g,tex,cell);
    // The reference shares the active bitmap for active normal and hover.
    // Do not wash out the full button on hover; the pointer glow is separate.
    // Keep a subtle press cue for feedback, without changing the state asset.
    if(!attention && running && pressed) {
        SolidBrush cue(Color(38,0,0,0));
        int inset=vertical?std::min(std::max(1,Px(2)),std::min((int)(cell.right-cell.left),(int)(cell.bottom-cell.top))/2):0;
        g.FillRectangle(&cue,(int)cell.left+inset,(int)cell.top+inset,
                        std::max(0,(int)(cell.right-cell.left)-2*inset),std::max(0,(int)(cell.bottom-cell.top)-2*inset));
    }
    return true;
}
// Approximation of Windows 7/8 color hot-track: sample the running app's
// icon once when the hovered group/icon changes; ignore neutral pixels.
// The light follows the pointer and is clipped to the task button.
static Color BetaHotTrackColor(size_t index) {
    if(!s || index>=s->groups.size())return Color(255,85,165,235);
    HICON icon=GroupIcon(index);static HICON lastIcon=nullptr;
    static std::wstring lastKey;static Color cached(255,85,165,235);
    if(icon==lastIcon && lastKey==s->groups[index].key)return cached;
    lastIcon=icon;lastKey=s->groups[index].key;cached=Color(255,85,165,235);
    if(!icon)return cached;
    std::unique_ptr<Gdiplus::Bitmap> image(Gdiplus::Bitmap::FromHICON(icon));
    if(!image || image->GetLastStatus()!=Ok)return cached;
    unsigned long long weight[64]{},rs[64]{},gs[64]{},bs[64]{};
    UINT w=image->GetWidth(),h=image->GetHeight();if(!w || !h)return cached;
    for(UINT y=0;y<h;y+=std::max(1u,h/24))for(UINT x=0;x<w;x+=std::max(1u,w/24)) {
        Color c;if(image->GetPixel(x,y,&c)!=Ok || c.GetA()<80)continue;
        int r=c.GetR(),green=c.GetG(),b=c.GetB();
        int high=std::max({r,green,b}),low=std::min({r,green,b});
        if(high-low<30 || high<45 || low>235)continue;
        int bucket=(r/64)*16+(green/64)*4+(b/64);
        unsigned q=(unsigned)c.GetA()*(unsigned)(high-low);
        weight[bucket]+=q;rs[bucket]+=(unsigned long long)q*r;
        gs[bucket]+=(unsigned long long)q*green;bs[bucket]+=(unsigned long long)q*b;
    }
    int best=0;for(int i=1;i<64;i++)if(weight[i]>weight[best])best=i;
    if(weight[best])cached=Color(255,(BYTE)(rs[best]/weight[best]),
        (BYTE)(gs[best]/weight[best]),(BYTE)(bs[best]/weight[best]));
    return cached;
}
static void DrawBetaHotTrack(Graphics& g,const RECT& cell,int index,float fade) {
    if(!BetaTextureAvailable(2) || !s || index<0 || (size_t)index>=s->groups.size() ||
       s->groups[index].tasks.empty() || fade<=0.001f || IsRectEmpty(&cell))return;
    POINT p{};if(!GetCursorPos(&p) || !ScreenToClient(s->bar,&p))return;
    float x=(float)std::max(cell.left,std::min(cell.right-1,p.x));
    float y=(float)std::max(cell.top,std::min(cell.bottom-1,p.y));
    float w=(float)(cell.right-cell.left),h=(float)(cell.bottom-cell.top);
    Color tint=BetaHotTrackColor((size_t)index);
    GraphicsPath path;path.AddEllipse(x-w*.70f,y-h*.80f,w*1.40f,h*1.60f);
    PathGradientBrush glow(&path);glow.SetCenterPoint(PointF(x,y));
    glow.SetCenterColor(Color((BYTE)std::lround(125*std::min(1.f,fade)),tint.GetR(),tint.GetG(),tint.GetB()));
    Color edge(0,tint.GetR(),tint.GetG(),tint.GetB());INT count=1;glow.SetSurroundColors(&edge,&count);
    // Keep the dark and light frame pixels intact as the pointer moves.
    int inset=std::min(std::max(1,Px(2)),std::min((int)w,(int)h)/4);
    auto saved=g.Save();g.SetClip(Rect((int)cell.left+inset,(int)cell.top+inset,
        std::max(0,(int)w-2*inset),std::max(0,(int)h-2*inset)),CombineModeIntersect);
    g.FillPath(&glow,&path);g.Restore(saved);
}
static int TaskTileLayers(size_t windows) {return (int)std::min<size_t>(windows,3);}
static void DrawTaskTileStack(Graphics& g,int x,int width,int height,int layers,bool active,float fade) {
    int step=std::max(1,Px(3)), frontWidth=width-std::max(0,layers-1)*step;
    if(frontWidth<=std::max(4,Px(8)))return;
    // Draw the rear slabs first; the front tile remains the same height.
    auto rearClip=g.Save();
    g.SetClip(Rect(x+frontWidth,0,std::max(0,width-frontWidth),height),CombineModeIntersect);
    for(int level=layers-1;level>=1;--level) {
        bool fullHeight=taskbarSkin.load()==3 || taskbarSkin.load()==4;
        int left=x+level*step, top=fullHeight?0:std::max(1,Px(1));
        int tw=frontWidth-std::max(1,Px(1)), th=fullHeight?height:std::max(1,height-top-Px(2));
        SolidBrush fill(Color(active?58:34,255,255,255));g.FillRectangle(&fill,left,top,tw,th);
        Pen border(Color((int)std::lround((active?150:85)+30*fade),255,255,255),1);
        g.DrawRectangle(&border,left,top,std::max(1,tw-1),std::max(1,th-1));
    }
    g.Restore(rearClip);
    if(active) DrawActiveReferenceTile(g,x,frontWidth,height,fade);
    else {
        // Match the active tile's full-cell frame; only fill and stroke intensity differ.
        int edge=std::max(1,Px(1)),tileWidth=std::max(1,frontWidth-edge);
        SolidBrush fill(Color((int)std::lround(40+37*fade),255,255,255));g.FillRectangle(&fill,x,0,tileWidth,height);
        Pen border(Color((int)std::lround(60+65*fade),255,255,255),1);
        g.DrawRectangle(&border,x,0,std::max(1,tileWidth-1),std::max(1,height-1));
    }
}
// Best effort only. RegisterShellHookWindow is documented but explicitly not
// intended for general use; delivery and stop semantics are not guaranteed.
static void StopAttentionHook(HWND h) noexcept {
    try {
        if(!s)return;KillTimer(h,TIMER_ATTENTION);s->attention.clear();s->attentionOn=false;
        if(s->shellHookRegistered) {
            using DeregisterFn=BOOL(WINAPI*)(HWND);
            auto fn=(DeregisterFn)GetProcAddress(GetModuleHandleW(L"user32.dll"),"DeregisterShellHookWindow");
            if(fn)fn(h);s->shellHookRegistered=false;
        }
        s->shellHookMessage=0;
    } catch(...) {Wh_Log(L"[T81-DIAG] attention cleanup exception");}
}
static void StartAttentionHook(HWND h) noexcept {
    try {
        if(!s || !h)return;
        UINT message=RegisterWindowMessageW(L"SHELLHOOK");
        using RegisterFn=BOOL(WINAPI*)(HWND);
        auto fn=(RegisterFn)GetProcAddress(GetModuleHandleW(L"user32.dll"),"RegisterShellHookWindow");
        if(!message || !fn || !fn(h)) {Wh_Log(L"[T81-DIAG] attention hook unavailable; flash disabled");return;}
        s->shellHookMessage=message;s->shellHookRegistered=true;
    } catch(...) {Wh_Log(L"[T81-DIAG] attention hook setup exception");}
}
static void PruneAttention() {
    if(!s)return;
    ULONGLONG now=GetTickCount64();HWND fg=GetForegroundWindow();
    HWND fgRoot=fg?GetAncestor(fg,GA_ROOTOWNER):nullptr;
    s->attention.erase(std::remove_if(s->attention.begin(),s->attention.end(),[&](const AttentionItem& a) {
        DWORD pid=0;bool valid=IsWindow(a.hwnd) && GetWindowThreadProcessId(a.hwnd,&pid) && pid==a.pid;
        bool active=valid && (fg==a.hwnd || (fgRoot && GetAncestor(a.hwnd,GA_ROOTOWNER)==fgRoot));
        return !valid || active || now>=a.until;
    }),s->attention.end());
    if(s->attention.empty() && s->bar) {KillTimer(s->bar,TIMER_ATTENTION);s->attentionOn=false;}
}
static void HandleAttentionEvent(UINT_PTR code,HWND target) {
    if(!s || !target)return;
    DWORD pid=0;
    if(code==HSHELL_WINDOWDESTROYED || code==HSHELL_WINDOWACTIVATED || code==HSHELL_RUDEAPPACTIVATED) {
        s->attention.erase(std::remove_if(s->attention.begin(),s->attention.end(),[&](const AttentionItem& a){return a.hwnd==target;}),s->attention.end());
        PruneAttention();if(s->bar)InvalidateRect(s->bar,nullptr,FALSE);return;
    }
    if(code!=HSHELL_FLASH || !IsWindow(target) || !GetWindowThreadProcessId(target,&pid) || !pid)return;
    auto known=std::find_if(s->tasks.begin(),s->tasks.end(),[&](const Task& t){return t.hwnd==target && t.pid==pid;});
    if(known==s->tasks.end() || GetForegroundWindow()==target)return;
    auto old=std::find_if(s->attention.begin(),s->attention.end(),[&](const AttentionItem& a){return a.hwnd==target;});
    AttentionItem item{target,pid,GetTickCount64()+15000};
    if(old!=s->attention.end())*old=item;else if(s->attention.size()<MAX_TASKS)s->attention.push_back(item);
    s->attentionOn=true;
    if(s->bar) {
        if(!SetTimer(s->bar,TIMER_ATTENTION,400,nullptr)) {s->attention.clear();s->attentionOn=false;Wh_Log(L"[T81-DIAG] attention timer unavailable; flash disabled");}
        InvalidateRect(s->bar,nullptr,FALSE);
    }
}
static bool GroupNeedsAttention(size_t index) {
    if(!s || !s->attentionOn || index>=s->groups.size())return false;
    for(size_t taskIndex:s->groups[index].tasks)if(taskIndex<s->tasks.size()) {
        const Task& task=s->tasks[taskIndex];
        for(const AttentionItem& a:s->attention)if(a.hwnd==task.hwnd && a.pid==task.pid)return true;
    }
    return false;
}
static const ButtonTransition* LiveButtonTransition(const std::wstring& key);
static float ButtonTransitionFraction(const ButtonTransition& t);
static RECT ButtonRevealRect(RECT r,float portion,bool vertical);
static void ButtonPulse(Graphics& g,const RECT& r,const ButtonTransition* t);
static bool ValidGhost(const ButtonTransition& t,int w,int h,bool vertical);
static void DrawBarForeground(HDC mem,const Layout& L,int w,int h,
                              const std::vector<int>& xs,const std::vector<int>& drawOrder,
                              HWND foreground,const wchar_t* time,const wchar_t* date) {
    {
        Graphics g(mem);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        const bool startDown=s->startPressed && s->hot==HIT_START;
        if(!startHotCorner.load()) {
        if(taskbarSkin.load()==1 && (s->win7StartImage || s->betaTexture[30])) {
            if(!gWin7OrbReady)DrawWin7Start(g,L.start.right,h,startDown?2:(s->hot==HIT_START?1:0));
        } else if(useBetaStartLogo.load() && s->betaStartImage) {
            DrawBetaStartImage(g,L.start.right,h,startDown?2:(s->hot==HIT_START?1:0));
        } else {
        DrawStartImage(g,L.start.right,h,0,1.f,nullptr);
        float startHover=startDown?1.0f:std::max(0.f,std::min(1.f,s->startFade));
        if(startHover>0) {
            Color tint=(taskbarSkin.load()==4 && customStartColor.load())?StartCustomColor():StartHoverColor();
            // Frame 1/2 is the dark tile with a transparent logo cutout.
            // Tint the white logo below it rather than tinting the black tile.
            DrawStartImage(g,L.start.right,h,0,startHover,&tint);
            DrawStartImage(g,L.start.right,h,startDown?2:1,startHover);
        }
        // Distinguish the pressed state with a subtle tint.
        if(startDown) {
            SolidBrush pressedTint(Color(50,0,0,0));
            g.FillRectangle(&pressedTint,0,0,L.start.right,h);
            Pen pressedEdge(Color(100,255,255,255),1);
            g.DrawLine(&pressedEdge,1,h-2,std::max(1,(int)L.start.right-2),h-2);
        }
        }
        }
        HWND fg=foreground;
        EnsureTrayHoverImage();
        g.SetClip(Rect(L.first,0,std::max(0,(int)L.tray.left-L.first),h));
        for(const auto& t:s->buttonTransitions)if(ValidGhost(t,w,h,false)) {
            RECT clip=ButtonRevealRect(t.rect,1.f-ButtonTransitionFraction(t),false);
            auto save=g.Save();g.SetClip(Rect(clip.left,clip.top,clip.right-clip.left,clip.bottom-clip.top),CombineModeIntersect);
            DrawTaskTileStack(g,t.rect.left,t.rect.right-t.rect.left,h,t.layers,false,0);g.Restore(save);
        }
        for(int i:drawOrder) {
            int x=xs[i]; bool active=false;
            for(size_t idx:s->groups[i].tasks) if(s->tasks[idx].hwnd==fg) { active=true; break; }
            const bool running=!s->groups[i].tasks.empty();
            const float fade=GroupHoverFade(i);
            RECT cell{x,0,x+L.width,h};const auto* transition=LiveButtonTransition(s->groups[i].key);
            auto savedClip=g.Save();
                if(transition && transition->reveal) {RECT clip=ButtonRevealRect(cell,ButtonTransitionFraction(*transition),false);
                g.SetClip(Rect(clip.left,clip.top,clip.right-clip.left,clip.bottom-clip.top),CombineModeIntersect);
                }
            // Closed pins have no tile, unless hovered.
            int layers=TaskTileLayers(s->groups[i].tasks.size());
            int step=std::max(1,Px(3));RECT front=cell;
            if(running && layers>1)front.right=std::max(front.left+1,front.right-(layers-1)*step);
            bool pressed=s->hot==i && s->dragCandidate && s->dragKey==s->groups[i].key;
            bool attention=GroupNeedsAttention((size_t)i);
            if(running && layers>1 && BetaTextureAvailable(2)) {
                auto rear=g.Save();g.SetClip(Rect(front.right,0,cell.right-front.right,h),CombineModeIntersect);
                for(int level=layers-1;level>=1;--level) {
                    RECT slab{cell.left+level*step,cell.top,front.right+level*step,cell.bottom};
                    DrawBetaTaskState(g,slab,true,active,s->hot==i,pressed,attention,false,true);
                }
                g.Restore(rear);
            }
            bool betaTask=DrawBetaTaskState(g,front,running,active,s->hot==i,pressed,attention,false,layers>1);
            if(betaTask) {
                // The texture renderer already painted this state.
            } else if(running) {
                DrawTaskTileStack(g,x,L.width,h,TaskTileLayers(s->groups[i].tasks.size()),active,fade);
            } else if(fade>0.001f) {
                RECT tile{x,0,x+L.width,h};
                DrawTrayHoverTile(g,tile,true,fade);
            }
            if(betaTask && running && s->hot==i)DrawBetaHotTrack(g,cell,i,fade);
            ButtonPulse(g,cell,transition);g.Restore(savedClip);
            if(!betaTask && GroupNeedsAttention((size_t)i)) {
                SolidBrush attention(Color(125,255,140,20));
                g.FillRectangle(&attention,x+Px(2),Px(2),std::max(1,L.width-Px(4)),std::max(1,h-Px(4)));
            }
        }
        g.ResetClip();
        if(s->dragActive && s->dragInsertion>=0) {
            int marker=L.first+std::min(s->dragInsertion,L.count)*L.width;
            SolidBrush caret(Color(220,255,255,255));
            g.FillRectangle(&caret,marker-1,Px(4),std::max(1,Px(2)),std::max(1,h-Px(8)));
        }
        if(BetaTextureAvailable(0) && !IsRectEmpty(&L.desktop)) {
            DrawBetaTexture(g,0,L.desktop);
            if(s->hot==HIT_DESKTOP) {
                SolidBrush overlay(Color(s->betaDesktopPressed?42:26,255,255,255));
                g.FillRectangle(&overlay,(int)L.desktop.left,0,(int)(L.desktop.right-L.desktop.left),h);
            }
        } else if(s->hot==HIT_DESKTOP) {
            SolidBrush hover(Color(26,255,255,255));g.FillRectangle(&hover,L.desktop.left,0,L.desktop.right-L.desktop.left,h);
        }
        EnsureTrayHoverImage();
        auto bandHover=[&](const RECT& r,int hit) { if(s->hot==hit && r.right>r.left) DrawTrayHoverTile(g,r,true); };
        bandHover(L.clock,HIT_CLOCK); bandHover(L.language,HIT_LANGUAGE); bandHover(L.addressBand,HIT_BAND_ADDRESS);
        if(!s->bandsLocked) {
            Pen grip(Color(130,245,250,255),1);
            for(RECT band:{L.desktopBand,L.linksBand,L.addressBand}) if(band.right>band.left)
                for(int y=Px(8);y<h-Px(6);y+=std::max(2,Px(3))) g.DrawLine(&grip,band.left+Px(3),y,band.left+Px(4),y);
        }
        if(s->desktopArrowHot && L.desktopBand.right>L.desktopBand.left) DrawArrowHover(g,BandArrowRect(L.desktopBand));
        if(s->linksArrowHot && L.linksBand.right>L.linksBand.left) DrawArrowHover(g,BandArrowRect(L.linksBand));
    }
    // GDI is used only for native HICONs and system-font text on the GDI+ surface.
    int icon=std::max(1,std::min(Px(32),h-Px(6)));
    ScopedSavedDC savedDc(mem);
    IntersectClipRect(mem,L.first,0,L.tray.left,h);
    for(const auto& t:s->buttonTransitions)if(ValidGhost(t,w,h,false) && t.icon) {
        RECT c=ButtonRevealRect(t.rect,1.f-ButtonTransitionFraction(t),false);ScopedSavedDC clip(mem);IntersectClipRect(mem,c.left,c.top,c.right,c.bottom);
        int ix=L.labels?t.rect.left+Px(10):t.rect.left+(t.rect.right-t.rect.left-icon)/2;
        DrawIconEx(mem,ix,(h-icon)/2,t.icon.get(),icon,icon,0,nullptr,DI_NORMAL);
    }
    for(int i:drawOrder) {
        int x=xs[i], ix=L.labels?x+Px(10):x+(L.width-icon)/2;
        ScopedSavedDC clip(mem);const auto* t=LiveButtonTransition(s->groups[i].key);
        if(t && t->reveal){RECT c=ButtonRevealRect(RECT{x,0,x+L.width,h},ButtonTransitionFraction(*t),false);IntersectClipRect(mem,c.left,c.top,c.right,c.bottom);}
        DrawIconEx(mem,ix,(h-icon)/2,GroupIcon(i),icon,icon,0,nullptr,DI_NORMAL);
    }
    HFONT font=CreateTaskbarFont(Dpi(),s->perPixelBar);
    std::unique_ptr<std::remove_pointer<HFONT>::type,DeleteFontHandle> fontGuard(font);
         ScopedSelection fontSelection(mem,font);
         SetTextColor(mem,RGB(255,255,255));
         SetBkMode(mem,TRANSPARENT);
    if(L.labels) for(const auto& ghost:s->buttonTransitions)if(ValidGhost(ghost,w,h,false)) {
        RECT c=ButtonRevealRect(ghost.rect,1.f-ButtonTransitionFraction(ghost),false);
            ScopedSavedDC clip(mem);
            IntersectClipRect(mem,c.left,c.top,c.right,c.bottom);
        RECT label{ghost.rect.left+Px(10)+icon+Px(6),1,ghost.rect.right-Px(4),h-1};
        DrawTextW(mem,ghost.title.c_str(),-1,&label,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
    }
    if(L.labels) for(int i:drawOrder) {
        int x=xs[i]; RECT t{x+Px(10)+icon+Px(6),1,x+L.width-Px(4),h-1};
        ScopedSavedDC clip(mem);const auto* transition=LiveButtonTransition(s->groups[i].key);
        if(transition && transition->reveal){RECT c=ButtonRevealRect(RECT{x,0,x+L.width,h},ButtonTransitionFraction(*transition),false);IntersectClipRect(mem,c.left,c.top,c.right,c.bottom);}
        DrawTextW(mem,GroupTitle(i).c_str(),-1,&t,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
    }
    savedDc.restore();
    // RestoreDC also restores the original font, then select ours again for
    // overflow and clock text; the font itself is deleted after painting.
    if(font) SelectObject(mem,font);
    SetTextColor(mem,RGB(255,255,255)); SetBkMode(mem,TRANSPARENT);
    if(L.overflow.right>L.overflow.left) {
        RECT more=L.overflow;
        DrawTextW(mem,L"»",-1,&more,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
    }
    if(!s->preservingTray) {
        ClockTextRows rows=GetClockTextRows(L.clock,h,Dpi());
        Graphics clockGraphics(mem); clockGraphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        FontFamily family(L"Segoe UI"); Gdiplus::Font clockFont(&family,(REAL)(9.0*Dpi()/72.0),FontStyleRegular,UnitPixel);
        StringFormat format;
             format.SetAlignment(StringAlignmentCenter);
             format.SetLineAlignment(StringAlignmentCenter);
             format.SetFormatFlags(StringFormatFlagsNoWrap);
        SolidBrush white(Color(255,255,255,255));
        auto clockRow=[&](const wchar_t* text,const RECT& row) {
            RectF rect((REAL)row.left,(REAL)row.top,(REAL)(row.right-row.left),(REAL)(row.bottom-row.top));
            clockGraphics.DrawString(text,-1,&clockFont,rect,&format,&white);
        };
        if(family.GetLastStatus()==Ok && clockFont.GetLastStatus()==Ok) {clockRow(time,rows.time); if(rows.showDate) clockRow(date,rows.date);}
        else {DrawTextW(mem,time,-1,&rows.time,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);if(rows.showDate)DrawTextW(mem,date,-1,&rows.date,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);}
        if(!IsRectEmpty(&L.language)) {
            ClockTextRows languageRows=GetClockTextRows(L.language,h,Dpi());
            bool twoLines=taskbarSkin.load()!=1 && languageRows.showDate && !s->languageRegion.empty();
            if(family.GetLastStatus()==Ok && clockFont.GetLastStatus()==Ok) {
                clockRow(s->languageText.c_str(),twoLines?languageRows.time:L.language);
                if(twoLines)clockRow(s->languageRegion.c_str(),languageRows.date);
            } else if(twoLines) {
                DrawTextW(mem,s->languageText.c_str(),-1,&languageRows.time,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);
                DrawTextW(mem,s->languageRegion.c_str(),-1,&languageRows.date,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);
            } else {
                RECT label=L.language;
                DrawTextW(mem,s->languageText.c_str(),-1,&label,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);
            }
        }
    }
    if(s->addressBand && L.addressBand.right>L.addressBand.left) { RECT label=L.addressBand;
         label.right=label.left+Px(66);
         DrawTextW(mem,Tr(L"Indirizzo",L"Address"),-1,&label,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);
         }
    auto drawBand=[&](RECT band,const wchar_t* title) {
        RECT arrow=BandArrowRect(band); band.right=arrow.left;
        DrawTextW(mem,title,-1,&band,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
        DrawTextW(mem,L"»",-1,&arrow,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOPREFIX);
    };
    if(s->desktopBand && L.desktopBand.right>L.desktopBand.left) drawBand(L.desktopBand,L"Desktop");
    if(s->linksBand && L.linksBand.right>L.linksBand.left) drawBand(L.linksBand,Tr(L"Collegamenti",L"Links"));
    if(s->privateActive) { Graphics trayGraphics(mem); trayGraphics.SetSmoothingMode(SmoothingModeAntiAlias); PaintPrivateArea(trayGraphics,mem,L,h); }
}
void Paint(HWND hwnd) {
    s->buttonPaintTick=GetTickCount64();
    ScopedPaintDC paint(hwnd); HDC dc=paint.dc; if(!dc) return;
    RECT r{}; GetClientRect(hwnd,&r); int w=r.right,h=r.bottom;
    if(w<=0 || h<=0 || w>32768 || h>4096) { return; }
    wchar_t time[64]={},date[64]={};
    GetTimeFormatW(LOCALE_USER_DEFAULT,TIME_NOSECONDS,nullptr,nullptr,time,64);
    GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,nullptr,nullptr,date,64);
    bool clockWidthChanged=UpdateClockMetrics(dc,time,date);
    if(clockWidthChanged)BuildGroups();
    auto L=GetLayout(w,h);
    AdvanceHoverFade();
    UpdateWin7OrbSurface(hwnd,L.start.right,h);
    std::vector<int> xs=AnimatedGroupPositions(L), drawOrder;
    std::wstring draggingBase;
    if(s->dragActive)for(const auto& group:s->groups)if(group.key==s->dragKey){draggingBase=GroupBase(group);break;}
    for(int i=0;i<L.count;i++)if(draggingBase.empty() || GroupBase(s->groups[i])!=draggingBase)drawOrder.push_back(i);
    if(!draggingBase.empty())for(int i=0;i<L.count;i++)if(GroupBase(s->groups[i])==draggingBase)drawOrder.push_back(i);
    HWND foreground=GetForegroundWindow();
    bool rendered=false;
    if(s->perPixelBar && (size_t)w*(size_t)h>8u*1024u*1024u) {
        Wh_Log(L"[T81-DIAG] per-pixel taskbar too large; reverting to legacy alpha");
        ConfigureBarAlpha(hwnd,false);
    }
    if(s->perPixelBar) {
        AlphaSurface bg(dc,w,h), black(dc,w,h), white(dc,w,h);
        if(bg.ok() && black.ok() && white.ok()) {
            { Graphics g(bg.dc); PaintTaskbarBackground(g,w,h); }
            PatBlt(black.dc,0,0,w,h,BLACKNESS);
            PatBlt(white.dc,0,0,w,h,WHITENESS);
            DrawBarForeground(black.dc,L,w,h,xs,drawOrder,foreground,time,date);
            DrawBarForeground(white.dc,L,w,h,xs,drawOrder,foreground,time,date);
            GdiFlush();
            ComposeForeground(bg.bits,black.bits,white.bits,(size_t)w*(size_t)h,EffectiveBarBackgroundOpacity());
            SIZE size{w,h}; POINT src{0,0}; RECT rect{}; GetWindowRect(hwnd,&rect);
            POINT dst{rect.left,rect.top}; BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
#ifdef TASKBAR81_TEST
            if(!testForceLayeredFailure)
#endif
                rendered=UpdateLayeredWindow(hwnd,nullptr,&dst,&size,bg.dc,&src,0,&blend,ULW_ALPHA)!=FALSE;
            if(rendered) {
                s->perPixelFrame=true;
#ifdef TASKBAR81_TEST
                ++testBarFrames;
#endif
            }
        }
        if(!rendered) {
            Wh_Log(L"[T81-DIAG] per-pixel taskbar failed, reverting to legacy alpha error=%lu",GetLastError());
            ConfigureBarAlpha(hwnd,false);
        }
    }
    if(!rendered) {
        std::unique_ptr<std::remove_pointer<HDC>::type,DeleteMemDC> mem(CreateCompatibleDC(dc));
        std::unique_ptr<std::remove_pointer<HBITMAP>::type,DeleteBitmapHandle> bmp(CreateCompatibleBitmap(dc,w,h));
        ScopedSelection selected(mem.get(),bmp.get());
        if(mem && bmp && selected.old && selected.old!=HGDI_ERROR) {
            { Graphics g(mem.get()); PaintTaskbarBackground(g,w,h); }
            DrawBarForeground(mem.get(),L,w,h,xs,drawOrder,foreground,time,date);
            BitBlt(dc,0,0,w,h,mem.get(),0,0,SRCCOPY);
        }
    }
    paint.end();
    UpdateAddressBand();
    if(clockWidthChanged && s->overflowPanel && IsWindowVisible(s->overflowPanel)) ResizePrivateOverflow();
}

/* Live thumbnail popup. DWM relationship lifecycle: register for each source,
 * update the destination, and always unregister on hide/source change/unload.
 * Reimplemented in native C++/GDI+; no WPF control or bitmap-capture fallback
 * is embedded. GPL-3.0-or-later. */
void HidePreview() {
    if(!s) return;
    if(s->preview) {KillTimer(s->preview,TIMER_PEEK);KillTimer(s->preview,TIMER_PREVIEW_ANIM);KillTimer(s->preview,TIMER_WIN7_PREVIEW_CLOSE);}
    s->previewAnimClosing=false;
    s->peekCandidate=nullptr; s->peekCandidatePid=0; PeekOff(false);
    for(auto& item:s->previews) {
        if(item.thumbnail) DwmUnregisterThumbnail(item.thumbnail);
        item.thumbnail=nullptr;
        if(item.icon) DestroyIcon(item.icon);
        item.icon=nullptr;
    }
    s->previews.clear();
    s->previewExtras.clear();
    s->previewHot=-1;
    s->previewCloseHot=false;s->win7PreviewCloseFade=0.f;s->win7PreviewCloseTick=0;
    s->previewClosePress=false;s->previewCloseCancelled=false;s->previewClosePressedIndex=-1;
    s->previewClosePressedWindow=nullptr;s->previewClosePressedPid=0;
    if(s->preview && GetCapture()==s->preview)ReleaseCapture();
    s->previewTracking=false;
    if(s->preview && IsWindow(s->preview)) ShowWindow(s->preview,SW_HIDE);
    if(s->tooltip && IsWindow(s->tooltip)) ShowWindow(s->tooltip,SW_HIDE);
    s->tooltipText.clear();s->hotCornerPreview=false;s->recoveryTooltipUntil=0;
    if(s->bar) KillTimer(s->bar,TIMER_PREVIEW);
}
static bool PreviewAnimationsEnabled() {
#ifdef TASKBAR81_TEST
    if(testPreviewAnimation>=0)return testPreviewAnimation!=0;
#endif
    BOOL enabled=FALSE;return SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&enabled,0) && enabled;
}
static void StartPreviewAnimation(bool closing) {
    if(!s || !s->preview || !IsWindowVisible(s->preview)) {if(closing)HidePreview();return;}
    if(!PreviewAnimationsEnabled()) {if(closing)HidePreview();return;}
    RECT r{},bar{};GetWindowRect(s->preview,&r);GetWindowRect(s->bar,&bar);
    s->previewAnimX=r.left;s->previewAnimY=r.top;
    s->previewAnimOffsetX=s->verticalActive?(s->verticalSide==1?-Px(4):Px(4)):0;
    s->previewAnimOffset=s->verticalActive?0:(r.bottom<=bar.top?Px(4):-Px(4));
    s->previewAnimStart=GetTickCount64();s->previewAnimClosing=closing;
    if(!closing) SetWindowPos(s->preview,nullptr,r.left+s->previewAnimOffsetX,r.top+s->previewAnimOffset,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
    if(!SetTimer(s->preview,TIMER_PREVIEW_ANIM,16,nullptr)) {if(closing)HidePreview();else SetWindowPos(s->preview,nullptr,r.left,r.top,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);}
}
static void TickPreviewAnimation() {
    if(!s || !s->preview)return;
    ULONGLONG elapsed=GetTickCount64()-s->previewAnimStart;float t=std::min(1.0f,(float)elapsed/(s->previewAnimClosing?100.0f:65.0f));
    float progress=s->previewAnimClosing?t:(1.0f-t);int y=s->previewAnimY+(int)std::lround(progress*s->previewAnimOffset);
    int x=s->previewAnimX+(int)std::lround(progress*s->previewAnimOffsetX);
    SetWindowPos(s->preview,nullptr,x,y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
    if(t>=1.0f) {KillTimer(s->preview,TIMER_PREVIEW_ANIM);if(s->previewAnimClosing)HidePreview();}
}
static void DrawPreviewBackground(HDC dc,RECT r) {
    Graphics g(dc); Color base=AccentBase();
    if(!s->trayColors.empty()) {
        COLORREF c=s->trayColors[s->trayColors.size()/2];
        base=Color(255,GetRValue(c),GetGValue(c),GetBValue(c));
    }
    SolidBrush bg(Mix(base,Color(255,246,249,251),8));
    g.FillRectangle(&bg,0,0,r.right,r.bottom);
}
// One geometry source for drawing, hover and the enlarged hit target.
static RECT PreviewCloseButtonRect(const PreviewItem& item) {
    if(taskbarSkin.load()==1) {
        LONG left=item.cell.right-Px(25),top=item.cell.top+Px(8);
        return RECT{left,top,left+Px(16),top+Px(14)};
    }
    LONG oldLeft=item.cell.right-Px(21),oldTop=item.cell.top+Px(2);
    int oldW=Px(18),oldH=Px(17);
    int width=std::max(1,(int)std::lround(oldW*0.96)),height=std::max(1,(int)std::lround(oldH*0.96));
    LONG left=oldLeft+(oldW-width)/2,top=oldTop+(oldH-height)/2;
    return RECT{left,top,left+width,top+height};
}
static bool PreviewCloseHit(const PreviewItem& item,POINT p) {
    if(taskbarSkin.load()==1 || taskbarSkin.load()==2) {RECT hit=PreviewCloseButtonRect(item);InflateRect(&hit,Px(3),Px(3));
        return !item.more && PtInRect(&item.cell,p) && PtInRect(&hit,p);}
    return !item.more && PtInRect(&item.cell,p) && p.x>=item.cell.right-Px(23) && p.y<item.cell.top+Px(21);
}
// Scale a font-independent mask and interpolate coverage for display DPI.
static void DrawPreviewCloseCross(HDC dc,const RECT& r,COLORREF color) {
    static constexpr BYTE mask[7][7]={
        {123,123,0,0,0,123,123}, {123,240,123,0,123,240,123},
        {0,123,240,167,240,123,0}, {0,0,167,255,167,0,0},
        {0,123,240,167,240,123,0}, {123,240,123,0,123,240,123},
        {123,123,0,0,0,123,123}};
    int n=std::max(3,MulDiv(7,Dpi()*96,12000));
    int left=(r.left+r.right-n)/2,top=(r.top+r.bottom-n)/2+std::max(1,Px(1));
    Graphics g(dc);g.SetSmoothingMode(SmoothingModeNone);
    // Scale the cross itself by four percent around its center.
    const REAL cx=(r.left+r.right)/2.f,cy=(r.top+r.bottom)/2.f;
    g.TranslateTransform(cx,cy);g.ScaleTransform(0.96f,0.96f);g.TranslateTransform(-cx,-cy);
    SolidBrush brush(Color(255,GetRValue(color),GetGValue(color),GetBValue(color)));
    auto sample=[&](int x,int y) -> int {return x<0 || x>6 || y<0 || y>6?0:mask[y][x];};
    for(int y=0;y<n;y++)for(int x=0;x<n;x++) {
        float sx=((x+0.5f)*7.f/n)-0.5f,sy=((y+0.5f)*7.f/n)-0.5f;
        int ix=(int)std::floor(sx),iy=(int)std::floor(sy);float fx=sx-ix,fy=sy-iy;
        float a=(1-fy)*((1-fx)*sample(ix,iy)+fx*sample(ix+1,iy))+
                       fy*((1-fx)*sample(ix,iy+1)+fx*sample(ix+1,iy+1));
        int alpha=std::max(0,std::min(255,(int)std::lround(a)));if(!alpha)continue;
        brush.SetColor(Color(alpha,GetRValue(color),GetGValue(color),GetBValue(color)));
        g.FillRectangle(&brush,left+x,top+y,1,1);
    }
}
// Win7 group outline and hover geometry; the DWM thumbnail remains live.
static void Win7RoundedPath(GraphicsPath& path,int x,int y,int w,int h,int radius) {
    radius=std::max(1,std::min(radius,std::min(w,h)/2));int d=2*radius;
    path.AddArc(x,y,d,d,180,90);path.AddArc(x+w-d,y,d,d,270,90);
    path.AddArc(x+w-d,y+h-d,d,d,0,90);path.AddArc(x,y+h-d,d,d,90,90);path.CloseFigure();
}
static void DrawWin7PreviewChrome(Graphics& g,RECT r) {
    int w=r.right,h=r.bottom;if(w<Px(20)||h<Px(20))return;
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    // One outline around the group. Individual outlines are drawn only for
    // the thumbnail currently under the pointer, never at rest.
    GraphicsPath outer;Win7RoundedPath(outer,0,0,w-1,h-1,Px(5));
    Pen border(Color(190,220,232,241),std::max(1,Px(1)));g.DrawPath(&border,&outer);
    g.SetSmoothingMode(SmoothingModeNone);
}
// ComposeForeground applies the Win8 opacity to the whole rectangle; clip its
// rounded corners afterward so the desktop remains transparent outside them.
static void ClipWin7PreviewCorners(BYTE* bits,int w,int h) {
    int radius=std::max(1,std::min(Px(5),std::min(w,h)/2));
    for(int yy=0;yy<radius;++yy)for(int xx=0;xx<radius;++xx) {
        int coverage=0;for(int sy=0;sy<4;++sy)for(int sx=0;sx<4;++sx) {
            float x=xx+(sx+0.5f)/4.f-radius,y=yy+(sy+0.5f)/4.f-radius;
            if(x*x+y*y<=radius*radius)++coverage;
        }
        int xs[]={xx,w-1-xx},ys[]={yy,h-1-yy};
        for(int cy=0;cy<2;++cy)for(int cx=0;cx<2;++cx) {
            BYTE* p=bits+((size_t)ys[cy]*w+xs[cx])*4;
            for(int c=0;c<4;++c)p[c]=(BYTE)((p[c]*coverage+8)/16);
        }
    }
}
static void DrawPreviewForeground(HDC mem,RECT r) {
                {
                    Graphics g(mem);
                    if(taskbarSkin.load()==1) DrawWin7PreviewChrome(g,r);
                    else {Pen border(Color(190,245,248,250),1);g.DrawRectangle(&border,0,0,r.right-1,r.bottom-1);}
                    for(size_t i=0;i<s->previews.size();i++) {
                        const auto& item=s->previews[i];
                        if((int)i==s->previewHot) {
                            if(taskbarSkin.load()==1 && !item.more) {
                                int x=item.cell.left,y=item.cell.top;
                                int w=item.cell.right-item.cell.left,h=item.cell.bottom-item.cell.top;
                                if(w>Px(6) && h>Px(6)) {
                                    auto state=g.Save();g.SetSmoothingMode(SmoothingModeAntiAlias);
                                    GraphicsPath selection;Win7RoundedPath(selection,x,y,w-1,h-1,Px(3));
                                    SolidBrush hover(Color(45,255,255,255));g.FillPath(&hover,&selection);
                                    Pen edge(Color(155,230,241,252),std::max(1,Px(1)));g.DrawPath(&edge,&selection);
                                    g.Restore(state);
                                }
                            } else if(taskbarSkin.load()==2 && !item.more) {
                                SolidBrush hover(Color(45,255,255,255));
                                Pen edge(Color(155,230,241,252),std::max(1,Px(1)));
                                int x=item.cell.left,y=item.cell.top,w=item.cell.right-item.cell.left,h=item.cell.bottom-item.cell.top;
                                if(w>1 && h>1){g.FillRectangle(&hover,x,y,w,h);g.DrawRectangle(&edge,x,y,w-1,h-1);}
                            } else if(taskbarSkin.load()!=1) {
                                SolidBrush hover(Color(40,255,255,255));
                                g.FillRectangle(&hover,(int)item.cell.left,(int)item.cell.top,
                                    (int)(item.cell.right-item.cell.left),(int)(item.cell.bottom-item.cell.top));
                            }
                        }
                        if(!item.more && (int)i==s->previewHot) {
                            bool down=s->previewCloseHot && s->previewClosePress && s->previewClosePressedIndex==(int)i;
                            if((taskbarSkin.load()==1 || taskbarSkin.load()==2) && BetaTextureAvailable(8)) {
                                RECT button=PreviewCloseButtonRect(item);Rect dst(button.left,button.top,button.right-button.left,button.bottom-button.top);
                                // Scale the same artwork used by the Jump List with GDI+.
                                auto savedClose=g.Save();
                                g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                                g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
                                g.SetCompositingQuality(CompositingQualityHighQuality);
                                Image* normal=s->betaTexture[8];Image* pressed=BetaTextureAvailable(10)?s->betaTexture[10]:normal;
                                if(down)g.DrawImage(pressed,dst,0,0,(int)pressed->GetWidth(),(int)pressed->GetHeight(),UnitPixel);
                                else {
                                    g.DrawImage(normal,dst,0,0,(int)normal->GetWidth(),(int)normal->GetHeight(),UnitPixel);
                                    if(BetaTextureAvailable(9) && s->win7PreviewCloseFade>0.f) {
                                        float a=std::max(0.f,std::min(1.f,s->win7PreviewCloseFade));
                                        // A small RGB lift makes hover slightly lighter; alpha
                                        // remains independent and follows the Start-style fade.
                                        ColorMatrix cm={{{1,0,0,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,a,0},{0.035f,0.035f,0.035f,0,1}}};
                                        ImageAttributes attr;attr.SetColorMatrix(&cm);attr.SetWrapMode(WrapModeTileFlipXY);Image* over=s->betaTexture[9];
                                        g.DrawImage(over,dst,0,0,(int)over->GetWidth(),(int)over->GetHeight(),UnitPixel,&attr);
                                    }
                                }
                                g.Restore(savedClose);
                            } else {
                            // The X is already on a red button when the thumbnail
                            // is hot; the X hover uses the brighter reference red.
                            SolidBrush close(down?Color(255,130,24,24):
                                (s->previewCloseHot?Color(255,186,73,50):Color(255,155,45,35)));
                            RECT button=PreviewCloseButtonRect(item);
                            g.FillRectangle(&close,(int)button.left,(int)button.top,(int)(button.right-button.left),(int)(button.bottom-button.top));
                            // Dark border from the supplied Windows 8.1 reference.
                            int width=(int)(button.right-button.left),height=(int)(button.bottom-button.top);
                            Pen closeBorder(s->previewCloseHot?Color(255,67,20,34):Color(255,186,73,50),1);
                            if(width>1 && height>1)g.DrawRectangle(&closeBorder,(int)button.left,(int)button.top,width-1,height-1);
                            }
                        }
                        if(taskbarSkin.load()==1 && item.thumbnail && !IsRectEmpty(&item.win7ThumbnailRect)) {
                            RECT q=item.win7ThumbnailRect;Pen line(Color(210,230,238,245),1);
                            g.DrawRectangle(&line,(int)q.left-1,(int)q.top-1,(int)(q.right-q.left)+1,(int)(q.bottom-q.top)+1);
                        }
                    }
                }
                HFONT font=CreateTaskbarFont(Dpi(),s->perPixelPreview);
                HGDIOBJ oldFont=font?SelectObject(mem,font):nullptr;
                COLORREF frame=RGB(15,74,121);
                if(!s->trayColors.empty()) frame=s->trayColors[s->trayColors.size()/2];
                int luminance=(GetRValue(frame)*299+GetGValue(frame)*587+GetBValue(frame)*114)/1000;
                int skin=taskbarSkin.load();
                COLORREF foreground=(skin>=1 && skin<=4)?RGB(255,255,255):(luminance>=145?RGB(24,24,24):RGB(255,255,255));
                SetTextColor(mem,foreground); SetBkMode(mem,TRANSPARENT);
                for(size_t i=0;i<s->previews.size();i++) {
                    const auto& item=s->previews[i];
                    if(item.more) {
                        RECT text=item.cell;
                        DrawTextW(mem,item.title.c_str(),-1,&text,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
                        continue;
                    }
                    bool win7=taskbarSkin.load()==1;
                    int small=Px(16);
                    if(item.icon) DrawIconEx(mem,item.cell.left+Px(win7?9:4),item.cell.top+Px(win7?7:3),item.icon,small,small,0,nullptr,DI_NORMAL);
                    RECT title{item.cell.left+Px(win7?30:25),item.cell.top+Px(win7?5:2),item.cell.right-Px(win7?28:24),item.cell.top+Px(win7?25:22)};
                    bool centerTitle=(skin==3 || skin==4);
                    if(centerTitle) {title.left=item.cell.left+Px(25);title.right=item.cell.right-Px(25);}
                    DrawTextW(mem,item.title.c_str(),-1,&title,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX|(centerTitle?DT_CENTER:0));
                    if((int)i==s->previewHot && !((taskbarSkin.load()==1 || taskbarSkin.load()==2) && BetaTextureAvailable(8))) {
                        RECT x=PreviewCloseButtonRect(item);
                        DrawPreviewCloseCross(mem,x,RGB(255,255,255));
                    }
                    if(!item.thumbnail && item.icon) {
                        int sz=Px(32); DrawIconEx(mem,(item.cell.left+item.cell.right-sz)/2,
                           (item.cell.top+item.cell.bottom-sz)/2,item.icon,sz,sz,0,nullptr,DI_NORMAL);
                    }
                }
                if(oldFont) SelectObject(mem,oldFont);
                if(font) DeleteObject(font);

}
static void ConfigurePreviewAlpha(HWND h,bool perPixel) {
    if(!s || !h) return;
    LONG_PTR style=GetWindowLongPtrW(h,GWL_EXSTYLE);
    if(perPixel) style|=WS_EX_LAYERED;
    else style&=~((LONG_PTR)WS_EX_LAYERED);
    SetWindowLongPtrW(h,GWL_EXSTYLE,style);
    s->perPixelPreview=perPixel; s->previewPixelFrame=false;
}
static void PaintPreview(HWND h) {
    PAINTSTRUCT ps{}; HDC dc=BeginPaint(h,&ps); RECT r{}; GetClientRect(h,&r);
    int w=r.right,hgt=r.bottom;
    if(w<=0 || hgt<=0 || w>32768 || hgt>4096) { EndPaint(h,&ps); return; }
    bool rendered=false;
    if(s->perPixelPreview && (size_t)w*(size_t)hgt>8u*1024u*1024u) {
        Wh_Log(L"[T81-DIAG] per-pixel preview too large; reverting to opaque preview");
        ConfigurePreviewAlpha(h,false);
    }
    if(s->perPixelPreview) {
        AlphaSurface bg(dc,w,hgt),black(dc,w,hgt),white(dc,w,hgt);
        if(bg.ok() && black.ok() && white.ok()) {
            DrawPreviewBackground(bg.dc,r);
            PatBlt(black.dc,0,0,w,hgt,BLACKNESS);
            PatBlt(white.dc,0,0,w,hgt,WHITENESS);
            DrawPreviewForeground(black.dc,r);
            DrawPreviewForeground(white.dc,r);
            GdiFlush();
            ComposeForeground(bg.bits,black.bits,white.bits,(size_t)w*(size_t)hgt,(BYTE)previewOpacity.load());
            if(taskbarSkin.load()==1)ClipWin7PreviewCorners(bg.bits,w,hgt);
            RECT rect{}; GetWindowRect(h,&rect); POINT dst{rect.left,rect.top},src{0,0}; SIZE size{w,hgt};
            BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
#ifdef TASKBAR81_TEST
            if(!testForcePreviewLayeredFailure)
#endif
                rendered=UpdateLayeredWindow(h,nullptr,&dst,&size,bg.dc,&src,0,&blend,ULW_ALPHA)!=FALSE;
            if(rendered) {
                s->previewPixelFrame=true;
#ifdef TASKBAR81_TEST
                ++testPreviewFrames;
#endif
            }
        }
        if(!rendered) {
            Wh_Log(L"[T81-DIAG] per-pixel preview failed, reverting to opaque preview error=%lu",GetLastError());
            ConfigurePreviewAlpha(h,false);
            if(taskbarSkin.load()==1) {
                HRGN region=CreateRoundRectRgn(0,0,w+1,hgt+1,Px(10),Px(10));
                if(region && !SetWindowRgn(h,region,TRUE))DeleteObject(region);
            }
        }
    }
    if(!rendered) {
        HDC mem=CreateCompatibleDC(dc); HBITMAP bmp=CreateCompatibleBitmap(dc,w,hgt);
        if(mem && bmp) {
            HGDIOBJ old=SelectObject(mem,bmp);
            DrawPreviewBackground(mem,r); DrawPreviewForeground(mem,r);
            BitBlt(dc,0,0,w,hgt,mem,0,0,SRCCOPY); SelectObject(mem,old);
        }
        if(bmp) DeleteObject(bmp); if(mem) DeleteDC(mem);
    }
    EndPaint(h,&ps);
}
LRESULT CALLBACK PreviewProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    try {
        if(!s) return DefWindowProcW(h,msg,wp,lp);
        switch(msg) {
#ifdef TASKBAR81_TEST
        case WM_APP+940: {if(s->previews.empty())return 0;
            RECT r=PreviewCloseButtonRect(s->previews[0]);
            return MAKELPARAM((r.left+r.right)/2,(r.top+r.bottom)/2);
            }
        case WM_APP+941: {if(s->previews.empty())return 0;RECT r=s->previews[0].cell;return MAKELPARAM((r.left+r.right)/2,r.top+Px(10));}
        case WM_APP+942: return (s->previewClosePress?1:0)|(s->previewCloseHot?2:0)|(GetCapture()==h?4:0);
        case WM_APP+943: return s->perPixelPreview && s->previewPixelFrame?1:0;
        case WM_APP+944: {for(const auto& item:s->previews)if(item.hwnd==(HWND)wp && !item.more) {RECT r=PreviewCloseButtonRect(item);
            return MAKELPARAM((r.left+r.right)/2,(r.top+r.bottom)/2);
            }return 0;
            }
#endif
        case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintPreview(h); return 0;
        case WM_MOUSEWHEEL: {
            if(s->previewPageSize<=0 || s->previewTotal<=s->previewPageSize)return 0;
            const std::wstring key=s->previewGroupKey; int group=-1;
            for(size_t i=0;i<s->groups.size();++i)if(s->groups[i].key==key){group=(int)i;break;}
            int next=s->previewOffset+(GET_WHEEL_DELTA_WPARAM(wp)<0?s->previewPageSize:-s->previewPageSize);
            next=std::max(0,std::min(next,((s->previewTotal-1)/s->previewPageSize)*s->previewPageSize));
            if(group>=0 && next!=s->previewOffset)ShowPreviewPage(group,next);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if(s->previewAnimClosing) {KillTimer(h,TIMER_PREVIEW_ANIM);
                s->previewAnimClosing=false;
                SetWindowPos(h,nullptr,s->previewAnimX,s->previewAnimY,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
                SetTimer(s->bar,TIMER_PREVIEW,350,nullptr);
                }
            if(!s->previewTracking) {
                TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE,h,0};
                TrackMouseEvent(&track);
                s->previewTracking=true;
            }
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            int hot=-1; bool close=false;
            for(size_t i=0;i<s->previews.size();i++) if(PtInRect(&s->previews[i].cell,p)) {
                hot=(int)i;
                close=PreviewCloseHit(s->previews[i],p);
                break;
            }
            HWND candidate=hot>=0 && !s->previews[hot].more ? s->previews[hot].hwnd : nullptr;
            DWORD candidatePid=hot>=0 && !s->previews[hot].more ? s->previews[hot].processId : 0;
            if(candidate!=s->peekCandidate || candidatePid!=s->peekCandidatePid) {
                PeekOff(false); s->peekCandidate=candidate; s->peekCandidatePid=candidatePid;
                KillTimer(h,TIMER_PEEK); if(candidate) SetTimer(h,TIMER_PEEK,450,nullptr);
            }
            if(hot!=s->previewHot || close!=s->previewCloseHot) {
                if(taskbarSkin.load()==1 || taskbarSkin.load()==2) {
                    if(hot!=s->previewHot)s->win7PreviewCloseFade=0.f;
                    s->win7PreviewCloseTick=GetTickCount64();
                    if(!SetTimer(h,TIMER_WIN7_PREVIEW_CLOSE,16,nullptr))s->win7PreviewCloseFade=close?1.f:0.f;
                }
                s->previewHot=hot; s->previewCloseHot=close; InvalidateRect(h,nullptr,FALSE);
            }
            return 0;
        }
        case WM_TIMER:
            if(wp==TIMER_WIN7_PREVIEW_CLOSE) {
                if(taskbarSkin.load()!=1 && taskbarSkin.load()!=2) {KillTimer(h,TIMER_WIN7_PREVIEW_CLOSE);return 0;}
                ULONGLONG now=GetTickCount64();float dt=(float)std::min<ULONGLONG>(80,now-s->win7PreviewCloseTick);s->win7PreviewCloseTick=now;
                if(s->previewCloseHot)s->win7PreviewCloseFade=std::min(1.f,s->win7PreviewCloseFade+dt/180.f);
                else s->win7PreviewCloseFade=std::max(0.f,s->win7PreviewCloseFade-dt/350.f);
                if((s->previewCloseHot && s->win7PreviewCloseFade>=1.f)||(!s->previewCloseHot && s->win7PreviewCloseFade<=0.f))KillTimer(h,TIMER_WIN7_PREVIEW_CLOSE);
                InvalidateRect(h,nullptr,FALSE);return 0;
            }
            if(wp==TIMER_PREVIEW_ANIM) {TickPreviewAnimation();return 0;}
            if(wp==TIMER_PEEK) { KillTimer(h,TIMER_PEEK); DWORD pid=0;
                if(s->peekCandidate) GetWindowThreadProcessId(s->peekCandidate,&pid);
                if(pid && pid==s->peekCandidatePid && IsWindow(s->peekCandidate)) PeekWindow(s->peekCandidate);
                return 0;
            } break;
        case WM_MOUSELEAVE:
            KillTimer(h,TIMER_PEEK); s->peekCandidate=nullptr; s->peekCandidatePid=0; PeekOff(false);
            s->previewTracking=false; s->previewHot=-1; s->previewCloseHot=false;
            KillTimer(h,TIMER_WIN7_PREVIEW_CLOSE);s->win7PreviewCloseFade=0.f;s->win7PreviewCloseTick=0;
            // A non-activating window may have only partial capture. Leaving
            // the preview cancels the press and releases capture, so a mouse-up
            // delivered to another process cannot leave a stuck close button.
            if(s->previewClosePress) {
                s->previewClosePress=false;s->previewCloseCancelled=true;s->previewClosePressedIndex=-1;
                if(GetCapture()==h)ReleaseCapture();
            }
            InvalidateRect(h,nullptr,FALSE); return 0;
        case WM_CAPTURECHANGED:
            if(s->previewClosePress) {s->previewClosePress=false;s->previewCloseCancelled=true;s->previewClosePressedIndex=-1;InvalidateRect(h,nullptr,FALSE);}
            return 0;
        case WM_CANCELMODE:
            if(s->previewClosePress) {s->previewClosePress=false;s->previewCloseCancelled=true;s->previewClosePressedIndex=-1;}
            if(GetCapture()==h)ReleaseCapture();InvalidateRect(h,nullptr,FALSE);return 0;
        case WM_LBUTTONDOWN: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            s->previewCloseCancelled=false;s->previewClosePress=false;s->previewClosePressedIndex=-1;
            for(size_t i=0;i<s->previews.size();i++)if(PreviewCloseHit(s->previews[i],p)) {
                const auto& item=s->previews[i];s->previewClosePress=true;s->previewClosePressedIndex=(int)i;
                s->previewClosePressedWindow=item.hwnd;s->previewClosePressedPid=item.processId;
                s->previewHot=(int)i;s->previewCloseHot=true;SetCapture(h);InvalidateRect(h,nullptr,FALSE);return 0;
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            for(const auto& item:s->previews) if(!item.more && PtInRect(&item.cell,p)) {
                HWND target=item.hwnd; DWORD expected=item.processId, actual=0;
                POINT screen{}; GetCursorPos(&screen);
                HidePreview();
                GetWindowThreadProcessId(target,&actual);
                if(expected && expected==actual && IsWindow(target))
                    w7t::ShellMenu::ShowWindowSystemMenu(target,screen.x,screen.y,true);
                return 0;
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(s->previewClosePress) {
                int index=s->previewClosePressedIndex;HWND target=s->previewClosePressedWindow;DWORD expected=s->previewClosePressedPid;
                bool commit=index>=0 && (size_t)index<s->previews.size() && PreviewCloseHit(s->previews[index],p) &&
                    s->previews[index].hwnd==target && s->previews[index].processId==expected;
                s->previewClosePress=false;s->previewClosePressedIndex=-1;s->previewCloseCancelled=false;
                if(GetCapture()==h)ReleaseCapture();
                if(commit) {HidePreview();DWORD actual=0;GetWindowThreadProcessId(target,&actual);
                    if(expected && actual==expected && IsWindow(target))RequestTaskClose(target);
                } else InvalidateRect(h,nullptr,FALSE);
                return 0;
            }
            if(s->previewCloseCancelled) {s->previewCloseCancelled=false;return 0;}
            for(const auto& item:s->previews) {
                if(PtInRect(&item.cell,p)) {
                    if(item.more) {
                        std::vector<PreviewExtra> extras=s->previewExtras;
                        std::wstring labels;
                        for(const auto& extra:extras) {
                            std::wstring title=extra.title;
                            std::replace(title.begin(),title.end(),L'\n',L' ');
                            std::replace(title.begin(),title.end(),L'\r',L' ');
                            if(!labels.empty()) labels+=L"\n";
                            labels+=title.empty()?Tr(L"Finestra",L"Window"):title;
                        }
                        POINT screen{}; GetCursorPos(&screen);
                        HidePreview();
                        int selected=w7t::ShellMenu::ShowContextMenuEx(screen.x,screen.y,true,labels.c_str(),true);
                        if(selected>0 && (size_t)selected<=extras.size()) {
                            auto target=extras[(size_t)selected-1]; DWORD pid=0;
                            GetWindowThreadProcessId(target.hwnd,&pid);
                            if(pid && pid==target.processId && IsWindow(target.hwnd)) {
                                ActivateTask(target.hwnd);
                            }
                        }
                        return 0;
                    }
                    HWND target=item.hwnd;
                    DWORD expectedPid=item.processId;
                    // A close request requires a matching press and release.
                    if(PreviewCloseHit(item,p))return 0;
                    HidePreview();
                    DWORD currentPid=0; GetWindowThreadProcessId(target,&currentPid);
                    if(IsWindow(target) && expectedPid && currentPid==expectedPid) {
                        ActivateTask(target);
                    }
                    break;
                }
            }
            return 0;
        }
        }
        return DefWindowProcW(h,msg,wp,lp);
    } catch(...) {
        Wh_Log(L"[T81-RECOVERY] C++ exception in preview window; closing thumbnails");
        HidePreview();
        if(showRecoverableErrors.load()) {try {RECT bar{};
            if(s && s->bar && GetWindowRect(s->bar,&bar))ShowInfoTooltip(Tr(L"Errore recuperabile nell'anteprima. Controlla il log Windhawk.",L"Recoverable preview error. Check the Windhawk log."),(bar.left+bar.right)/2);
            if(s && s->tooltip && IsWindowVisible(s->tooltip))s->recoveryTooltipUntil=GetTickCount64()+5000;
            } catch(...) {}}
        return 0;
    }
}

// A closed pin has no DWM source. Show a small non-activating title tooltip
// rather than an empty preview; title and anchor are recomputed each time.
static LRESULT PinTooltipProcImpl(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if(msg==WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if(msg==WM_LBUTTONUP && s && s->hotCornerPreview) {HidePreview();SendWin(0);return 0;}
    if(msg==WM_ERASEBKGND) return 1;
    if(msg==WM_PAINT) {
        ScopedPaintDC paint(h); HDC dc=paint.dc; if(!dc)return 0;
        RECT r{}; GetClientRect(h,&r);
        if(s && s->hotCornerPreview) {
            // Vector reconstruction based on the Windows 8 RTM thumbnail in
            // AskVG: dark purple frame, Start label and compact tile rows.
            // The tiles are illustrative, not the user's live Start screen.
            {Graphics g(dc);g.SetSmoothingMode(SmoothingModeNone);
            g.ScaleTransform((REAL)r.right/93.f,(REAL)r.bottom/75.f);
            SolidBrush bg(Color(255,29,0,71));g.FillRectangle(&bg,0,0,93,75);
            Pen frame(Color(255,55,44,70),1);g.DrawRectangle(&frame,0,0,92,74);
            const int xs[]={10,30,50,60,78}, widths[]={18,18,8,8,12};
            const int colors[4][5]={{0x0066AE,0x2973D9,0x008B46,0x008B99,0xB04408},{0xE05214,0x00A0AD,0x057825,0x4E790D,0x006D96},{0x094BA3,0x7B28A6,0x0A75A9,0xCE4B10,0x31A629},{0xA12E95,0x0073BA,0x672A99,0x006D99,0xAE4112}};
            for(int y=0;y<4;y++)for(int x=0;x<5;x++){int c=colors[y][x];
                SolidBrush tile(Color(255,(c>>16)&255,(c>>8)&255,c&255));
                g.FillRectangle(&tile,xs[x],21+y*10,widths[x],8);
                }
            g.Flush(FlushIntentionSync);}
            SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(255,255,255));
            std::unique_ptr<std::remove_pointer<HFONT>::type,DeleteFontHandle> font(CreateFontW(-Px(11),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI"));
            {ScopedSelection selected(dc,font.get());
                RECT label{Px(10),Px(5),r.right-Px(4),Px(20)};
                DrawTextW(dc,L"Start",-1,&label,DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
                }
            return 0;
        }
        HBRUSH brush=CreateSolidBrush(RGB(250,250,250));
        if(brush) { FillRect(dc,&r,brush); DeleteObject(brush); }
        HPEN pen=CreatePen(PS_SOLID,1,RGB(140,140,140));
        HGDIOBJ oldPen=pen?SelectObject(dc,pen):nullptr;
        HGDIOBJ oldBrush=SelectObject(dc,GetStockObject(NULL_BRUSH));
        Rectangle(dc,0,0,r.right,r.bottom);
        if(oldBrush) SelectObject(dc,oldBrush);
        if(oldPen) SelectObject(dc,oldPen);
        if(pen) DeleteObject(pen);
        HFONT font=CreateFontW(-MulDiv(9,Dpi(),72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        HGDIOBJ oldFont=font?SelectObject(dc,font):nullptr;
        SetTextColor(dc,RGB(25,25,25)); SetBkMode(dc,TRANSPARENT);
        RECT text{Px(7),Px(4),r.right-Px(7),r.bottom};
        if(s) DrawTextW(dc,s->tooltipText.c_str(),-1,&text,DT_WORDBREAK|DT_NOPREFIX);
        if(oldFont) SelectObject(dc,oldFont);
        if(font) DeleteObject(font);
        return 0;
    }
    return DefWindowProcW(h,msg,wp,lp);
}
static LRESULT CALLBACK PinTooltipProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    try {return PinTooltipProcImpl(h,msg,wp,lp);}
    catch(...) {Wh_Log(L"[T81-RECOVERY] PinTooltipProc C++ exception");return msg==WM_CREATE?-1:0;}
}


static void ShowInfoTooltip(const std::wstring& message,int center) {
    if(!s || !s->bar || s->trayDrag.capture) return;
    s->hotCornerPreview=false;s->recoveryTooltipUntil=0;
    s->tooltipText=message;
    if(s->tooltipText.empty()) return;
    if(!s->tooltip) {
        s->tooltip=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,
            kTooltipClass,L"",WS_POPUP,0,0,1,1,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    }
    if(!s->tooltip) return;
    HDC dc=GetDC(s->tooltip);
    SIZE text{};
    HFONT font=CreateFontW(-MulDiv(9,Dpi(),72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    HGDIOBJ old=(dc && font)?SelectObject(dc,font):nullptr;
    RECT measured{0,0,Px(300),0};
    if(dc) { DrawTextW(dc,s->tooltipText.c_str(),-1,&measured,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);
             text.cx=measured.right-measured.left; text.cy=measured.bottom-measured.top; }
    if(dc && old) SelectObject(dc,old);
    if(font) DeleteObject(font);
    if(dc) ReleaseDC(s->tooltip,dc);
    int width=std::min(Px(320),std::max(Px(60),(int)text.cx+Px(16)));
    int height=std::max(Px(25),(int)text.cy+Px(8));
    RECT bar{}; GetWindowRect(s->bar,&bar);
    MONITORINFO info{}; info.cbSize=sizeof(info);
    if(!GetMonitorInfoW(MonitorFromWindow(s->bar,MONITOR_DEFAULTTONEAREST),&info)) return;
    int x=std::max((int)info.rcMonitor.left,std::min(center-width/2,(int)info.rcMonitor.right-width));
    int y=bar.top-height-Px(3);
    if(y<info.rcMonitor.top) y=bar.bottom+Px(3);
    if(s->verticalActive) {POINT cursor{};GetCursorPos(&cursor);x=s->verticalSide==1?bar.right+Px(3):bar.left-width-Px(3);y=cursor.y-height/2;
        x=std::max((int)info.rcMonitor.left,std::min(x,(int)info.rcMonitor.right-width));y=std::max((int)info.rcMonitor.top,std::min(y,(int)info.rcMonitor.bottom-height));}
    SetWindowPos(s->tooltip,HWND_TOPMOST,x,y,width,height,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    InvalidateRect(s->tooltip,nullptr,FALSE);
    SetTimer(s->bar,TIMER_PREVIEW,350,nullptr);
}

static void ShowHotCornerPreview() {
    if(!s || !s->bar)return;
    RECT bar{};GetWindowRect(s->bar,&bar);
    ShowInfoTooltip(L"Start",(int)bar.left+Px(35));
    if(!s->tooltip)return;
    s->hotCornerPreview=true;
    MONITORINFO mi{};mi.cbSize=sizeof(mi);
    if(!GetMonitorInfoW(MonitorFromWindow(s->bar,MONITOR_DEFAULTTONEAREST),&mi))return;
    int width=Px(93),height=Px(75),x=(int)bar.left,y=(int)bar.bottom-height;
    if(s->verticalActive) {x=s->verticalSide==1?bar.right:bar.left-width;y=bar.top;}
    else if(y<mi.rcMonitor.top)y=bar.bottom;
    x=std::max((int)mi.rcMonitor.left,std::min(x,(int)mi.rcMonitor.right-width));
    y=std::max((int)mi.rcMonitor.top,std::min(y,(int)mi.rcMonitor.bottom-height));
    SetWindowPos(s->tooltip,HWND_TOPMOST,x,y,width,height,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    InvalidateRect(s->tooltip,nullptr,FALSE);SetTimer(s->bar,TIMER_PREVIEW,100,nullptr);
}
static void ShowPinTooltip(int group) {
    if(!s || !s->bar || group<0 || (size_t)group>=s->groups.size()) return;
    RECT bar{},client{}; GetWindowRect(s->bar,&bar); GetClientRect(s->bar,&client);
    Layout layout=GetLayout(client.right,client.bottom);
    ShowInfoTooltip(GroupTitle((size_t)group),bar.left+layout.first+group*layout.width+layout.width/2);
}
static void ShowSystemTooltip(int hit) {
    if(!s || !s->bar) return;
    RECT cell=SystemCell(hit); MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
    ShowInfoTooltip(SystemTooltipText(hit),(cell.left+cell.right)/2);
}

static void ShowBarButtonTooltip(int hit) {
    if(!s || !s->bar || (hit!=HIT_START && hit!=HIT_DESKTOP))return;
    RECT client{},cell{};GetClientRect(s->bar,&client);
    if(s->verticalActive) { auto layout=GetVerticalLayout(client.right,client.bottom);
        cell=hit==HIT_START?layout.start:layout.desktop;
    } else { auto layout=GetLayout(client.right,client.bottom);
        cell=hit==HIT_START?layout.start:layout.desktop;
    }
    if(IsRectEmpty(&cell))return;
    MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
    ShowInfoTooltip(hit==HIT_START?Tr(L"Start",L"Start"):Tr(L"Mostra desktop",L"Show desktop"),
                    (cell.left+cell.right)/2);
}
static void ShowLanguageTooltip() {
    if(!s || !s->bar || !LanguageIndicatorVisible())return;
    RECT client{},cell{};GetClientRect(s->bar,&client);
    cell=s->verticalActive?GetVerticalLayout(client.right,client.bottom).language:GetLayout(client.right,client.bottom).language;
    MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
    ShowInfoTooltip(s->languageName,(cell.left+cell.right)/2);
}
static void ShowPrivateIconTooltip(int hit) {
    if(!s || hit>HIT_TRAY_ICON_BASE) return; int i=HIT_TRAY_ICON_BASE-hit;
    if(i<0 || (size_t)i>=s->trayIcons.size() || s->trayIcons[i].tip.empty()) return;
    RECT cell=PrivateIconRect(i); MapWindowPoints(s->bar,nullptr,(POINT*)&cell,2);
    ShowInfoTooltip(s->trayIcons[i].tip,(cell.left+cell.right)/2);
}

// Crop only when DWM's source size and the window rectangle agree exactly.
// This avoids converting between incompatible DPI coordinate spaces. If the
// bounds look suspicious, retain the full source rather than trim content.
static bool ThumbnailVisibleSource(SIZE source,RECT window,RECT frame,int maxInset,RECT* result) {
    if(!result || source.cx<=0 || source.cy<=0 || maxInset<0 ||
       window.right-window.left!=source.cx || window.bottom-window.top!=source.cy) return false;
    int l=frame.left-window.left,t=frame.top-window.top;
    int r=window.right-frame.right,b=window.bottom-frame.bottom;
    if(l<0 || t<0 || r<0 || b<0 || l>maxInset || t>maxInset || r>maxInset || b>maxInset ||
       (!l && !t && !r && !b) || l+r>=source.cx || t+b>=source.cy) return false;
    *result=RECT{l,t,source.cx-r,source.cy-b};
    return true;
}
void ShowPreview(int group) {ShowPreviewPage(group,0);}
void ShowPreviewPage(int group,int offset) {
    if(!s || !s->bar || group<0 || (size_t)group>=s->groups.size()) return;
    if(s->jumpPopup && IsWindow(s->jumpPopup)) return;
    HidePreview();
    if(s->groups[group].tasks.empty()) { ShowPinTooltip(group); return; }
    if(!s->preview) {
        s->preview=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,
            kPreviewClass,L"Taskbar preview",WS_POPUP,0,0,10,10,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    }
    if(!s->preview) return;
    ConfigurePreviewAlpha(s->preview,perPixelPreview.load());
    ExcludeFromPeek(s->preview);
    Group g=s->groups[group];
    int total=(int)g.tasks.size();
    if(total<=0) return;
    RECT barRect{},barClient{}; GetWindowRect(s->bar,&barRect); GetClientRect(s->bar,&barClient);
    Layout layout=GetLayout(barClient.right,barClient.bottom);
    MONITORINFO mi{}; mi.cbSize=sizeof(mi);
    if(!GetMonitorInfoW(MonitorFromWindow(s->bar,MONITOR_DEFAULTTONEAREST),&mi)) return;
    // Win7: shrink the previous 228 x 186 DIP single-preview outer size
    // by about 15%, preserving its proportions. Title and icons stay legible.
    int gap=Px(taskbarSkin.load()==1?6:7),cellH=Px(taskbarSkin.load()==1?146:132),available=(int)(mi.rcWork.right-mi.rcWork.left);
    if(s->verticalActive)available=std::max(Px(110)+2*gap,available-(int)(barRect.right-barRect.left)-Px(6));
    int maxCols=std::max(1,std::min(8,(available-gap)/(Px(110)+gap)));
    int availableH=std::max(1,(int)(mi.rcWork.bottom-mi.rcWork.top));
    int maxRows=std::max(1,(availableH-gap)/(cellH+gap));
    int capacity=std::max(1,maxCols*maxRows);
    offset=std::max(0,std::min(offset,((total-1)/capacity)*capacity));
    int count=std::min(total-offset,capacity),columns=std::min(count,maxCols);
    int rows=(count+columns-1)/columns;
    int cellW=std::max(1,std::min(Px(taskbarSkin.load()==1?182:188),(available-gap-columns*gap)/columns));
    int width=gap+columns*(cellW+gap),height=gap+rows*(cellH+gap);
    int center=barRect.left+layout.first+group*layout.width+layout.width/2;
    int x=std::max((int)mi.rcWork.left,std::min(center-width/2,(int)mi.rcWork.right-width));
    int y=s->nativeEdge==ABE_TOP?barRect.bottom+Px(2):barRect.top-height-Px(2);
    y=std::max((int)mi.rcWork.top,std::min(y,(int)mi.rcWork.bottom-height));
    if(s->verticalActive) {auto v=GetVerticalLayout(barClient.right,barClient.bottom);if(group<v.offset || group>=v.offset+v.count)return;
        x=s->verticalSide==1?barRect.right+Px(2):barRect.left-width-Px(2);
        y=barRect.top+v.first+(group-v.offset)*v.step+v.step/2-height/2;
        x=std::max((int)mi.rcWork.left,std::min(x,(int)mi.rcWork.right-width));y=std::max((int)mi.rcWork.top,std::min(y,(int)mi.rcWork.bottom-height));}
    s->previewGroupKey=g.key;s->previewOffset=offset;s->previewPageSize=capacity;s->previewTotal=total;
    SetWindowPos(s->preview,HWND_TOPMOST,x,y,width,height,SWP_NOACTIVATE|SWP_SHOWWINDOW);
    if(taskbarSkin.load()==1 && !s->perPixelPreview) {
        HRGN region=CreateRoundRectRgn(0,0,width+1,height+1,Px(10),Px(10));
        if(region && !SetWindowRgn(s->preview,region,TRUE))DeleteObject(region);
    } else SetWindowRgn(s->preview,nullptr,TRUE);
    for(int i=0;i<count;i++) {
        int col=i%columns,row=i/columns;
        const Task& task=s->tasks[g.tasks[offset+i]];
        PreviewItem item{};
        item.hwnd=task.hwnd; GetWindowThreadProcessId(task.hwnd,&item.processId);
        item.icon=task.icon?CopyIcon(task.icon):nullptr; item.title=task.title;
        item.cell={gap+col*(cellW+gap),gap+row*(cellH+gap),gap+col*(cellW+gap)+cellW,gap+row*(cellH+gap)+cellH};
        item.thumbnail=nullptr;
        if(IsWindow(task.hwnd) && SUCCEEDED(DwmRegisterThumbnail(s->preview,task.hwnd,&item.thumbnail)) && item.thumbnail) {
            SIZE src{}; RECT sourceCrop{},windowRect{},frameRect{}; bool crop=false;
            const bool win7=taskbarSkin.load()==1;
            RECT dst{item.cell.left+Px(win7?7:5),item.cell.top+Px(win7?33:24),item.cell.right-Px(win7?7:5),item.cell.bottom-Px(win7?7:5)};
            if(SUCCEEDED(DwmQueryThumbnailSourceSize(item.thumbnail,&src)) && src.cx>0 && src.cy>0) {
                if(GetWindowRect(task.hwnd,&windowRect) &&
                   SUCCEEDED(DwmGetWindowAttribute(task.hwnd,DWMWA_EXTENDED_FRAME_BOUNDS,&frameRect,sizeof(frameRect))))
                    crop=ThumbnailVisibleSource(src,windowRect,frameRect,Px(24),&sourceCrop);
                int sw=crop?sourceCrop.right-sourceCrop.left:src.cx;
                int sh=crop?sourceCrop.bottom-sourceCrop.top:src.cy;
                int maxW=dst.right-dst.left,maxH=dst.bottom-dst.top;
                int tw=maxW, th=MulDiv(sh,tw,sw);
                if(th>maxH) { th=maxH; tw=MulDiv(sw,th,sh); }
                dst.left+=(maxW-tw)/2; dst.top+=(maxH-th)/2;
                dst.right=dst.left+tw; dst.bottom=dst.top+th;
            }
            DWM_THUMBNAIL_PROPERTIES prop{};
            prop.dwFlags=DWM_TNP_RECTDESTINATION|DWM_TNP_VISIBLE|DWM_TNP_OPACITY;
            if(crop) { prop.dwFlags|=DWM_TNP_RECTSOURCE; prop.rcSource=sourceCrop; }
            prop.rcDestination=dst; prop.fVisible=TRUE; prop.opacity=255;
            HRESULT update=DwmUpdateThumbnailProperties(item.thumbnail,&prop);
            if(FAILED(update) && crop) {
                // A source rectangle can be rejected on some builds; retry
                // the complete thumbnail before dropping the live preview.
                prop.dwFlags&=~DWM_TNP_RECTSOURCE;
                RECT full{item.cell.left+Px(win7?7:5),item.cell.top+Px(win7?33:24),item.cell.right-Px(win7?7:5),item.cell.bottom-Px(win7?7:5)};
                int maxW=full.right-full.left,maxH=full.bottom-full.top;
                int tw=maxW,th=MulDiv(src.cy,tw,src.cx);
                if(th>maxH) { th=maxH; tw=MulDiv(src.cx,th,src.cy); }
                full.left+=(maxW-tw)/2; full.top+=(maxH-th)/2;
                full.right=full.left+tw; full.bottom=full.top+th;
                prop.rcDestination=full;
                update=DwmUpdateThumbnailProperties(item.thumbnail,&prop);
            }
            if(FAILED(update)) {
                DwmUnregisterThumbnail(item.thumbnail); item.thumbnail=nullptr;
            } else if(win7) item.win7ThumbnailRect=prop.rcDestination;
        }
        s->previews.push_back(std::move(item));
    }
    if(total>capacity) Wh_Log(L"[T81-DIAG] preview pages offset=%d visible=%d total=%d; mouse wheel changes page",offset,count,total);
    if(taskbarSkin.load()==1 && !s->previews.empty()) {
        const auto& first=s->previews.front();const RECT& thumb=first.win7ThumbnailRect;
        Wh_Log(L"[T81-PREVIEW] dpi=%d group=%dx%d cell=%dx%d thumbnail=%ldx%ld live=%d layered=%d opacity=%d",
            Dpi(),width,height,cellW,cellH,thumb.right-thumb.left,thumb.bottom-thumb.top,
            first.thumbnail?1:0,s->perPixelPreview?1:0,previewOpacity.load());
    }
    InvalidateRect(s->preview,nullptr,FALSE);
    StartPreviewAnimation(false);
    SetTimer(s->bar,TIMER_PREVIEW,350,nullptr);
}

/* Snapshot overflow targets before a modal menu; timers may refresh groups
 * while TrackPopupMenu is running. */
void ShowOverflow() {
    RECT rc{}; GetClientRect(s->bar,&rc);
    Layout layout=GetLayout(rc.right,rc.bottom);
    struct Target { HWND hwnd; DWORD pid; std::wstring pinPath; };
    std::vector<Target> targets;
    std::wstring labels;
    for(size_t i=(size_t)layout.count;i<s->groups.size();++i) {
        const Group& group=s->groups[i];
        Target target{};
        if(!group.tasks.empty()) {
            target.hwnd=GroupTask(i).hwnd;
            GetWindowThreadProcessId(target.hwnd,&target.pid);
        } else if(group.pin>=0 && (size_t)group.pin<s->pins.size()) {
            target.pinPath=s->pins[group.pin].path;
        }
        std::wstring title=GroupTitle(i);
        std::replace(title.begin(),title.end(),L'\n',L' ');
        std::replace(title.begin(),title.end(),L'\r',L' ');
        if(title.empty()) title=Tr(L"Applicazione",L"Application");
        if(!labels.empty()) labels+=L"\n";
        labels+=L"App: "+title;
        targets.push_back(std::move(target));
    }
    POINT p{}; GetCursorPos(&p);
    int selected=w7t::ShellMenu::ShowContextMenuEx(p.x,p.y,true,labels.c_str(),true);
    if(selected<=0 || (size_t)selected>targets.size()) return;
    Target target=targets[(size_t)selected-1];
    if(!target.pinPath.empty()) ShellExecuteW(s->bar,L"open",target.pinPath.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
    else {
        DWORD pid=0; GetWindowThreadProcessId(target.hwnd,&pid);
        if(pid && pid==target.pid && IsWindow(target.hwnd)) ClickTask(target.hwnd);
    }
}
RECT FallbackRect() {
    MONITORINFO mi{}; mi.cbSize=sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(POINT{0,0},MONITOR_DEFAULTTOPRIMARY),&mi);
    RECT r=mi.rcMonitor; r.top=r.bottom-Px(44); return r;
}
struct NativeTraySearch {
    RECT bar;
    RECT found;
    bool valid=false;
};
static BOOL CALLBACK FindNativeTrayChild(HWND child, LPARAM parameter) {
    auto* search=reinterpret_cast<NativeTraySearch*>(parameter);
    wchar_t cls[64]={};
    if(!IsWindowVisible(child) || !GetClassNameW(child,cls,64) || wcscmp(cls,L"TrayNotifyWnd")) return TRUE;
    RECT r{};
    if(!GetWindowRect(child,&r)) return TRUE;
    // Exclude stale, hidden or implausible child windows and use the first
    // right-hand notification area that intersects the taskbar rectangle.
    RECT intersection{};
    if(!IntersectRect(&intersection,&r,&search->bar) || r.right<=r.left ||
       r.right<search->bar.right-Px(48) || r.left<=search->bar.left+Px(kStartLogical)) return TRUE;
    search->found=r;
    search->valid=true;
    return FALSE;
}
static int NativeTrayLeft(HWND native, const RECT& bar) {
    NativeTraySearch search{};
    search.bar=bar;
    if(native) EnumChildWindows(native,FindNativeTrayChild,reinterpret_cast<LPARAM>(&search));
    if(search.valid) return std::max((int)bar.left,std::min((int)search.found.left,(int)bar.right));
    // Win11's XAML area does not necessarily expose a TrayNotifyWnd HWND.
    // This is intentionally a user-adjustable estimate, not tray detection.
    int fallback=Px(trayFallbackWidth.load());
    return std::max((int)bar.left+Px(kStartLogical),std::max((int)bar.left,(int)bar.right-fallback));
}


// Samples only a narrow screen rectangle inside the native tray. No bitmap is
// written to disk, sent over the network, or retained after the call: the
// persistent state is one median color per scanline. Foreground menus and
// unrelated windows cause sampling to be skipped rather than picked up.
static bool IsTrayPixelAvailable(HWND native, POINT pt) {
    HWND hit=WindowFromPoint(pt);
    if(!hit || !IsWindowVisible(hit)) return false;
    if(hit==native || IsChild(native,hit)) return true;
    return s && (hit==s->clockOverlay || hit==s->volumeOverlay);
}

static bool SampleNativeTrayRows(HWND native, const RECT& tray, std::vector<COLORREF>& rows) {
    if(!native || !IsWindowVisible(native) || tray.right<=tray.left || tray.bottom<=tray.top) return false;
    const int height=tray.bottom-tray.top;
    const int width=std::min((int)(tray.right-tray.left)-Px(3),Px(104));
    if(height<8 || height>512 || width<Px(16) || width>1024) return false;
    const int sx=tray.left+Px(2), sy=tray.top;
    // At least two independent locations must currently belong to the real
    // taskbar. Avoid color changes while another window covers the tray.
    int eligible=0;
    for(int i=1;i<=3;i++) {
        POINT point{sx+width*i/4,sy+std::max(1,height/2)};
        if(IsTrayPixelAvailable(native,point)) ++eligible;
    }
    if(eligible<2) return false;
    HDC screen=GetDC(nullptr);
    if(!screen) return false;
    HDC memory=CreateCompatibleDC(screen);
    BITMAPINFO bitmap{};
    bitmap.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bitmap.bmiHeader.biWidth=width;
    bitmap.bmiHeader.biHeight=-height;
    bitmap.bmiHeader.biPlanes=1;
    bitmap.bmiHeader.biBitCount=32;
    bitmap.bmiHeader.biCompression=BI_RGB;
    void* bits=nullptr;
    HBITMAP dib=memory?CreateDIBSection(screen,&bitmap,DIB_RGB_COLORS,&bits,nullptr,0):nullptr;
    HGDIOBJ old=(dib&&memory)?SelectObject(memory,dib):nullptr;
    bool ok=dib && old && bits && BitBlt(memory,0,0,width,height,screen,sx,sy,SRCCOPY|CAPTUREBLT);
    std::vector<COLORREF> result;
    if(ok) {
        result.reserve(height);
        const DWORD* pixels=static_cast<const DWORD*>(bits);
        const int samples=std::min(width,32);
        for(int y=0;y<height;y++) {
            BYTE red[32],green[32],blue[32];
            for(int j=0;j<samples;j++) {
                int x=(samples==1)?0:((width-1)*j/(samples-1));
                DWORD c=pixels[(size_t)y*width+x];
                blue[j]=(BYTE)c;
                green[j]=(BYTE)(c>>8);
                red[j]=(BYTE)(c>>16);
            }
            const int mid=samples/2;
            std::nth_element(red,red+mid,red+samples);
            std::nth_element(green,green+mid,green+samples);
            std::nth_element(blue,blue+mid,blue+samples);
            result.push_back(RGB(red[mid],green[mid],blue[mid]));
        }
    }
    if(old && memory) SelectObject(memory,old);
    if(dib) DeleteObject(dib);
    if(memory) DeleteDC(memory);
    ReleaseDC(nullptr,screen);
    if(!ok || result.size()!=(size_t)height) return false;
    rows.swap(result);
    return true;
}

// Window colorization is 0xAARRGGBB (not COLORREF 0x00BBGGRR).
// Independent of the old useAccent option, for both reconstructed skins.
static Color TaskbarSystemColor() {
    DWORD value=0;BOOL opaque=FALSE;
    if(SUCCEEDED(DwmGetColorizationColor(&value,&opaque)))
        return Color(255,(BYTE)(value>>16),(BYTE)(value>>8),(BYTE)value);
    COLORREF c=GetSysColor(COLOR_HIGHLIGHT);
    return Color(255,GetRValue(c),GetGValue(c),GetBValue(c));
}
static Color EffectiveTaskbarColor() {
    if(customTaskbarColor.load()) {int c=taskbarColorRgb.load();return Color(255,(c>>16)&255,(c>>8)&255,c&255);}
    return TaskbarSystemColor();
}
static Color TintBetaReference(const BYTE* sample,Color accent) {
    // Preserve the sampled luminance profile, not its screenshot-specific blue.
    int luma=(299*(int)sample[0]+587*(int)sample[1]+114*(int)sample[2])/1000;
    if(luma<125)return Mix(accent,Color(255,0,0,0),std::min(75,(125-luma)*55/125));
    return Mix(accent,Color(255,255,255),std::min(65,(luma-125)*65/130));
}
static void DrawSoftGlassTexture(Graphics& g,int width,int height,bool vertical) {
    if(!s || !s->betaTexture[24] || width<=0 || height<=0)return;
    Image* image=s->betaTexture[24];auto saved=g.Save();
    g.SetClip(Rect(0,0,width,height),CombineModeIntersect);
    if(vertical) {
        int tile=std::max(1,Px(192));
        if(s->verticalSide==1){g.TranslateTransform((REAL)width,0.f);g.RotateTransform(90.f);}
        else {g.TranslateTransform(0.f,(REAL)height);g.RotateTransform(-90.f);}
        for(int x=0;x<height;x+=tile)g.DrawImage(image,Rect(x,0,tile,width),0,0,(int)image->GetWidth(),(int)image->GetHeight(),UnitPixel);
    } else {
        int tile=std::max(1,Px(192));
        for(int x=0;x<width;x+=tile)g.DrawImage(image,Rect(x,0,tile,height),0,0,(int)image->GetWidth(),(int)image->GetHeight(),UnitPixel);
    }
    g.Restore(saved);
}
// Sampled RGB rows for the static background profile.
static const BYTE kBetaReferenceRows[41][3] = {
    {37,73,95},
    {157,211,245},
    {92,182,238},
    {92,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {93,183,238},
    {92,183,238},
    {92,182,238},
    {90,182,237},
    {88,180,237},
    {85,179,236},
    {81,177,235},
    {76,174,234},
    {70,171,233},
    {64,168,232},
    {58,165,230},
    {58,165,230},
    {52,162,230},
    {47,159,229},
    {41,157,230},
    {37,156,230},
    {34,155,232},
    {32,155,232},
    {30,155,234},
    {30,155,234},
    {29,155,234},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235},
    {29,155,235}
};
static Color EffectiveLegacySkinTint() {return EffectiveTaskbarColor();}
// Paint the reflection and alpha strip without live desktop blur.
static void DrawWin7AeroTextures(Graphics& g,int width,int height,bool vertical) {
    if(!s || width<=0 || height<=0)return;
    Image* reflection=s->betaTexture[29];Image* strip=s->betaTexture[28];
    auto saved=g.Save();g.SetClip(Rect(0,0,width,height),CombineModeIntersect);
    int length=width,thickness=height;
    if(vertical) {
        length=height;thickness=width;
        if(s->verticalSide==1){g.TranslateTransform((REAL)width,0.f);g.RotateTransform(90.f);}
        else {g.TranslateTransform(0.f,(REAL)height);g.RotateTransform(-90.f);}
    }
    if(reflection) {
        g.SetInterpolationMode(InterpolationModeHighQualityBilinear);
        g.DrawImage(reflection,Rect(0,0,length,thickness),0,0,(int)reflection->GetWidth(),(int)reflection->GetHeight(),UnitPixel);
    }
    if(strip && s->betaTextureStream[28]) {
        // A one-pixel source stretched horizontally can sample beyond the
        // source edge in GDI+. Read each original row and fill its full span.
        Bitmap rows(s->betaTextureStream[28],FALSE);
        int sourceHeight=(int)rows.GetHeight();
        if(rows.GetLastStatus()==Ok && rows.GetWidth()==1 && sourceHeight>0) {
            for(int y=0;y<thickness;y++) {
                int sy=std::min(sourceHeight-1,(int)((long long)y*sourceHeight/thickness));
                Color color;
                if(rows.GetPixel(0,sy,&color)==Ok) {SolidBrush row(color);g.FillRectangle(&row,0,y,length,1);}
            }
        } else {
            ImageAttributes attr;attr.SetWrapMode(WrapModeTileFlipXY);
            g.DrawImage(strip,Rect(0,0,length,thickness),0,0,(int)strip->GetWidth(),(int)strip->GetHeight(),UnitPixel,&attr);
        }
    }
    g.Restore(saved);
}
static BYTE EffectiveBarBackgroundOpacity() {
    // The user's setting remains the actual opacity, including for Win7.
    return (BYTE)taskbarBackgroundOpacity.load();
}
static void PaintTaskbarBackground(Graphics& g, int width, int height) {
    if(s && taskbarSkin.load()==1 && width>0 && height>0) {
        SolidBrush base(EffectiveLegacySkinTint());g.FillRectangle(&base,0,0,width,height);
        DrawWin7AeroTextures(g,width,height,false);return;
    }
    if(s && useBetaStartLogo.load() && s->betaStartImage && !s->verticalActive && width>0 && height>0) {
        Color accent=EffectiveTaskbarColor();
        for(int y=0;y<height;y++) {
            int sample=std::min(40,(int)(((long long)y*41)/height));
            SolidBrush row(TintBetaReference(kBetaReferenceRows[sample],accent));
            g.FillRectangle(&row,0,y,width,1);
        }
        DrawSoftGlassTexture(g,width,height,false);
        return;
    }
    if(s && !customTaskbarColor.load() && matchNativeTray.load() && !s->privateActive && s->trayColors.size()==(size_t)height) {
        for(int y=0;y<height;y++) {
            COLORREF c=s->trayColors[y];
            SolidBrush row(Color(255,GetRValue(c),GetGValue(c),GetBValue(c)));
            g.FillRectangle(&row,0,y,width,1);
        }
        DrawSoftGlassTexture(g,width,height,s->verticalActive);
        return;
    }
    // A restrained fallback: 8.1 has a flatter surface than 7's strong
    // glossy gradient. Native matching wins when sampling is available.
    Color base=EffectiveTaskbarColor();
    LinearGradientBrush background(Point(0,0),Point(0,std::max(1,height)),
        Mix(base,Color(255,225,225,225),15),Mix(base,Color(255,15,15,15),12));
    g.FillRectangle(&background,0,0,width,height);
    // Draw a dark top rule.
    SolidBrush edge(Color(42,0,0,0));
    g.FillRectangle(&edge,0,0,width,std::max(1,Px(1)));
    DrawSoftGlassTexture(g,width,height,s && s->verticalActive);
}

// Legacy fallback path when private tray creation fails or is disabled. The
// two overlays are one-alpha layered windows over native icons/text.
// A recognized classic control can use a requested legacy action; otherwise
// it is left uncovered. No owner, notification registration or tray icons are
// replaced. The Win11 XAML tray does not provide this HWND structure.
enum class NativeControlKind : ULONG_PTR { Clock=1, Volume=2 };

static const CLSID kAeroClockClass =
    {0xA323554A,0x0FE1,0x4E49,{0xAE,0xE1,0x67,0x22,0x46,0x5D,0x79,0x9F}};
static const IID kAeroClockInterface =
    {0x7A5FCA8A,0x76B1,0x44C8,{0xA9,0x7C,0xE7,0x17,0x3C,0xCA,0x5F,0x4F}};

static bool ReadLocalPointer(const void* source, void* target, size_t size) {
    SIZE_T read=0;
    return source && ReadProcessMemory(GetCurrentProcess(),source,target,size,&read) && read==size;
}

// TB_GETBUTTON / TB_GETITEMRECT are synchronous window messages with caller
// buffers. If SendMessageTimeout expires, the other thread might still write
// to the buffer. Reserve it outside our stack and deliberately retain it
// until process exit on a timeout; do not try again in this session. This
// bounded leak is safer than a late write into released module/stack memory.
static bool QueryToolbarBuffer(HWND toolbar, UINT message, WPARAM index,
                               void* output, SIZE_T size) {
    if(!s || s->toolbarTimedOut || !size || size>4096) return false;
    void* buffer=VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!buffer) return false;
    DWORD_PTR result=0;
    BOOL delivered=SendMessageTimeoutW(toolbar,message,index,(LPARAM)buffer,
                         SMTO_ABORTIFHUNG|SMTO_BLOCK,80,&result);
    if(!delivered) {
        s->toolbarTimedOut=true;
        Wh_Log(L"Native tray toolbar did not respond; volume interception disabled for this session");
        return false; // retain at most one page until explorer.exe exits
    }
    if(result) CopyMemory(output,buffer,size);
    VirtualFree(buffer,0,MEM_RELEASE);
    return result!=0;
}

static bool IsClassicVolumeButton(HWND toolbar, int index, uintptr_t begin, uintptr_t end) {
    TBBUTTON button{};
    if(!QueryToolbarBuffer(toolbar,TB_GETBUTTON,(WPARAM)index,&button,sizeof(button))) return false;
    if(button.fsState & TBSTATE_HIDDEN) return false;
    HWND owner=nullptr;
    // Like the referenced legacy-sound-flyout, this relies on a private
    // toolbar dwData layout and SndVolSSO's ATL window class. ReadProcessMemory
    // avoids blindly dereferencing data that may have changed on another build.
    if(!button.dwData || !ReadLocalPointer(reinterpret_cast<void*>(button.dwData),&owner,sizeof(HWND)) ||
       !owner || !IsWindow(owner)) return false;
    wchar_t name[128]={};
    if(!GetClassNameW(owner,name,128) || wcsncmp(name,L"ATL:",4)) return false;
    wchar_t* endptr=nullptr;
    unsigned long long address=wcstoull(name+4,&endptr,16);
    if(endptr==name+4 || *endptr!=0) return false;
    return address>=begin && address<end;
}

struct FindTrayControls {
    HWND native=nullptr;
    RECT bar{};
    RECT area{};
    HWND clock=nullptr;
    RECT clockRect{};
    HWND toolbar=nullptr;
    RECT volumeRect{};
    uintptr_t sndBegin=0;
    uintptr_t sndEnd=0;
};

static bool InsideNativeTray(const RECT& rect, const FindTrayControls& f) {
    RECT intersection{};
    if(rect.right<=rect.left || rect.bottom<=rect.top ||
       !IntersectRect(&intersection,&rect,&f.area)) return false;
    return rect.left>=f.area.left && rect.right<=f.bar.right &&
           rect.top>=f.bar.top && rect.bottom<=f.bar.bottom;
}

static BOOL CALLBACK FindTrayControlsProc(HWND child, LPARAM parameter) {
    auto& f=*reinterpret_cast<FindTrayControls*>(parameter);
    if(!IsWindowVisible(child)) return TRUE;
    wchar_t cls[64]={};
    if(!GetClassNameW(child,cls,64)) return TRUE;
    if(!f.clock && !wcscmp(cls,L"TrayClockWClass")) {
        RECT rect{};
        if(GetWindowRect(child,&rect) && InsideNativeTray(rect,f)) {
            f.clock=child;
            f.clockRect=rect;
        }
    }
    if(!f.toolbar && f.sndBegin && !wcscmp(cls,L"ToolbarWindow32") && !s->toolbarTimedOut) {
        DWORD_PTR count=0;
        if(!SendMessageTimeoutW(child,TB_BUTTONCOUNT,0,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,80,&count)) {
            s->toolbarTimedOut=true;
            return TRUE;
        }
        int n=std::min((int)count,64);
        for(int i=0;i<n && !s->toolbarTimedOut;i++) {
            if(!IsClassicVolumeButton(child,i,f.sndBegin,f.sndEnd)) continue;
            RECT rect{};
            if(!QueryToolbarBuffer(child,TB_GETITEMRECT,(WPARAM)i,&rect,sizeof(rect))) break;
            MapWindowPoints(child,nullptr,reinterpret_cast<POINT*>(&rect),2);
            if(InsideNativeTray(rect,f)) { f.toolbar=child; f.volumeRect=rect; }
            break;
        }
    }
    return TRUE;
}

static void ForwardNativeMouse(HWND overlay, HWND target, UINT message, WPARAM wp, LPARAM lp) {
    if(!overlay || !target || !IsWindow(target)) return;
    POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
    ClientToScreen(overlay,&p);
    ScreenToClient(target,&p);
    PostMessageW(target,message,wp,MAKELPARAM(p.x,p.y));
}

static bool LaunchClassicVolume(HWND overlay) {
    if(!s || !overlay)return false;
    RECT icon{};if(!GetWindowRect(overlay,&icon))return false;
    POINT cursor{},point{};bool haveCursor=GetCursorPos(&cursor)!=FALSE;
    if(!ChooseVolumeAnchor(icon,haveCursor?&cursor:nullptr,&point))return false;
    return LaunchSndvolAt(point,L"native-overlay",&icon);
}

static LRESULT CALLBACK ClassicClockFrameProc(HWND h, UINT msg, WPARAM wp, LPARAM lp,
                                               UINT_PTR id, DWORD_PTR) {
    if(msg==WM_NCHITTEST) {
        LRESULT result=DefSubclassProc(h,msg,wp,lp);
        switch(result) {
        case HTTOP: case HTTOPRIGHT: case HTRIGHT: case HTBOTTOMRIGHT:
        case HTBOTTOM: case HTBOTTOMLEFT: case HTLEFT: case HTTOPLEFT:
            return HTBORDER;
        default: return result;
        }
    }
    if(msg==WM_NCDESTROY) {
        RemoveWindowSubclass(h,ClassicClockFrameProc,id);
        if(s && s->clockFlyout==h) s->clockFlyout=nullptr;
    }
    return DefSubclassProc(h,msg,wp,lp);
}

// Same-thread subclass fallback for the Aero frame. No private symbol hooks
// are installed, to avoid blocking initialisation. RetroBar supplies the COM
// interface; this is not the full retrobar-clock-flyout-fix.
static bool AttachClassicClockFrame() {
    if(!s) return false;
    HWND flyout=nullptr;
    while((flyout=FindWindowExW(nullptr,flyout,L"ClockFlyoutWindow",nullptr))) {
        DWORD pid=0;GetWindowThreadProcessId(flyout,&pid);if(pid==GetCurrentProcessId() && IsWindowVisible(flyout))break;
    }
    if(!flyout)return false;
    RECT anchor=PrivateSystemAnchor(HIT_CLOCK);
    if(!IsRectEmpty(&anchor))PlaceFlyoutAtVisualBar(flyout,anchor);
    if(GetWindowThreadProcessId(flyout,nullptr)!=GetCurrentThreadId()) {
        Wh_Log(L"[T81-DIAG] clock found on another Explorer thread; position requested, no subclass");return true;
    }
    BOOL disabled=TRUE; DwmSetWindowAttribute(flyout,DWMWA_TRANSITIONS_FORCEDISABLED,&disabled,sizeof(disabled));
    if(s->clockFlyout && s->clockFlyout!=flyout && IsWindow(s->clockFlyout))
        RemoveWindowSubclass(s->clockFlyout,ClassicClockFrameProc,1);
    if(!SetWindowSubclass(flyout,ClassicClockFrameProc,1,0)) return false;
    s->clockFlyout=flyout;
    SetWindowLongPtrW(flyout,GWL_STYLE,GetWindowLongPtrW(flyout,GWL_STYLE)|WS_THICKFRAME);
    SetWindowLongPtrW(flyout,GWL_EXSTYLE,GetWindowLongPtrW(flyout,GWL_EXSTYLE)|WS_EX_TOOLWINDOW);
    Wh_Log(L"Classic ClockFlyoutWindow attached to border fix");
    return true;
}

static bool OpenClassicClock() {
    if(!s || !s->bar) return false;
    if(!s->aeroClock) {
        HRESULT hr=CoCreateInstance(kAeroClockClass,nullptr,CLSCTX_INPROC_SERVER,
                                    kAeroClockInterface,reinterpret_cast<void**>(&s->aeroClock));
        if(FAILED(hr) || !s->aeroClock) {
            Wh_Log(L"Classic Aero clock COM unavailable (0x%08X); trying configured fallback",(unsigned)hr);
            return false;
        }
    }
    RECT rect=s->nativeRect;
    if(s->privateActive) {RECT visual{};if(GetWindowRect(s->bar,&visual))rect=visual;}
    HRESULT hr=s->aeroClock->ShowFlyout(s->bar,&rect);
    if(FAILED(hr)) {
        Wh_Log(L"Classic clock ShowFlyout failed (0x%08X); trying configured fallback",(unsigned)hr);
        return false;
    }
    s->clockFixAttempts=20;
    SetTimer(s->bar,TIMER_CLOCK_FIX,100,nullptr);
    // S_OK only means the request was accepted. The timer checks for an
    // actual ClockFlyoutWindow before claiming the border fix was attached.
    return true;
}

static LRESULT NativeControlOverlayProcImpl(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if(msg==WM_NCCREATE) {
        auto* create=reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)create->lpCreateParams);
    }
    if(!s) return DefWindowProcW(h,msg,wp,lp);
    auto kind=(NativeControlKind)(ULONG_PTR)GetWindowLongPtrW(h,GWLP_USERDATA);
    HWND target=kind==NativeControlKind::Clock?s->nativeClock:s->volumeToolbar;
    switch(msg) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc=BeginPaint(h,&ps);
        RECT rect{}; GetClientRect(h,&rect);
        FillRect(dc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
        EndPaint(h,&ps);
        return 0;
    }
    case WM_LBUTTONDOWN: return 0;
    case WM_LBUTTONUP: {
        bool handled=kind==NativeControlKind::Clock?OpenClassicClock():LaunchClassicVolume(h);
        if(!handled) {
            ForwardNativeMouse(h,target,WM_LBUTTONDOWN,MK_LBUTTON,lp);
            ForwardNativeMouse(h,target,WM_LBUTTONUP,0,lp);
        }
        return 0;
    }
    case WM_RBUTTONDOWN: case WM_RBUTTONUP:
    case WM_MBUTTONDOWN: case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
        ForwardNativeMouse(h,target,msg,wp,lp);
        return 0;
    }
    return DefWindowProcW(h,msg,wp,lp);
}
static LRESULT CALLBACK NativeControlOverlayProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    try {return NativeControlOverlayProcImpl(h,msg,wp,lp);}
    catch(...) {Wh_Log(L"[T81-RECOVERY] NativeControlOverlayProc C++ exception");return msg==WM_CREATE?-1:0;}
}


static HWND CreateNativeOverlay(NativeControlKind kind) {
    HWND h=CreateWindowExW(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST,
                           kNativeControlOverlayClass,L"",WS_POPUP,0,0,1,1,
                           nullptr,nullptr,GetModuleHandleW(nullptr),(void*)(ULONG_PTR)kind);
    if(h && !SetLayeredWindowAttributes(h,0,1,LWA_ALPHA)) { DestroyWindow(h); return nullptr; }
    return h;
}

static void PlaceNativeOverlay(HWND& overlay, NativeControlKind kind, const RECT& rect, bool visible) {
    if(!visible) {
        if(overlay && IsWindow(overlay)) ShowWindow(overlay,SW_HIDE);
        return;
    }
    if(!overlay || !IsWindow(overlay)) overlay=CreateNativeOverlay(kind);
    if(overlay) {
        RECT old{}; bool wasVisible=IsWindowVisible(overlay)!=FALSE;
        bool same=GetWindowRect(overlay,&old) && EqualRect(&old,&rect);
        if(!wasVisible || !same) {
            UINT flags=SWP_NOACTIVATE|SWP_SHOWWINDOW;
            if(wasVisible) flags|=SWP_NOZORDER;
            SetWindowPos(overlay,HWND_TOPMOST,rect.left,rect.top,
                         rect.right-rect.left,rect.bottom-rect.top,flags);
        }
    }
}

static void UpdateNativeControlOverlays(HWND native, const RECT& bar, int trayLeft, bool show) {
    FindTrayControls found{};
    found.native=native;
    found.bar=bar;
    found.area=bar;
    found.area.left=trayLeft;
    HMODULE snd=preferClassicVolume.load()?GetModuleHandleW(L"SndVolSSO.dll"):nullptr;
    MODULEINFO info{};
    if(snd && GetModuleInformation(GetCurrentProcess(),snd,&info,sizeof(info))) {
        found.sndBegin=(uintptr_t)info.lpBaseOfDll;
        found.sndEnd=found.sndBegin+info.SizeOfImage;
    }
    if(show && native && (preferClassicClock.load() || found.sndBegin))
        EnumChildWindows(native,FindTrayControlsProc,(LPARAM)&found);
    s->nativeClock=found.clock;
    s->volumeToolbar=found.toolbar;
    PlaceNativeOverlay(s->clockOverlay,NativeControlKind::Clock,found.clockRect,
                       show && preferClassicClock.load() && found.clock);
    PlaceNativeOverlay(s->volumeOverlay,NativeControlKind::Volume,found.volumeRect,
                       show && preferClassicVolume.load() && found.toolbar);
}

static void ReleaseNativeControlOverlays() {
    if(!s) return;
    if(s->bar) KillTimer(s->bar,TIMER_CLOCK_FIX);
    if(s->clockFlyout && IsWindow(s->clockFlyout))
        RemoveWindowSubclass(s->clockFlyout,ClassicClockFrameProc,1);
    s->clockFlyout=nullptr;
    if(s->clockOverlay && IsWindow(s->clockOverlay)) DestroyWindow(s->clockOverlay);
    if(s->volumeOverlay && IsWindow(s->volumeOverlay)) DestroyWindow(s->volumeOverlay);
    s->clockOverlay=nullptr;
    s->volumeOverlay=nullptr;
    s->nativeClock=nullptr;
    s->volumeToolbar=nullptr;
    if(s->aeroClock) { s->aeroClock->Release(); s->aeroClock=nullptr; }
}

struct NativeTrayCandidate { HWND best=nullptr; long long score=-1; };
static BOOL CALLBACK FindOwnedNativeProc(HWND h,LPARAM lp) {
    DWORD pid=0; GetWindowThreadProcessId(h,&pid);
    if(pid!=GetCurrentProcessId() || (s && h==s->privateHost)) return TRUE;
    wchar_t cls[64]={};
    if(!GetClassNameW(h,cls,64) || wcscmp(cls,L"Shell_TrayWnd")) return TRUE;
    RECT r{}; if(!GetWindowRect(h,&r)) return TRUE;
    long long w=(long long)r.right-r.left,hg=(long long)r.bottom-r.top;
    if(w<200 || hg<12 || w<=hg || hg>400) return TRUE;
    auto* result=reinterpret_cast<NativeTrayCandidate*>(lp);
    long long score=std::min(w,10000LL)*std::min(hg,400LL)+(IsWindowVisible(h)?10000000LL:0);
    if(score>result->score) { result->best=h; result->score=score; }
    return TRUE;
}
static HWND FindOwnedNativeTaskbar() {
    NativeTrayCandidate candidate; EnumWindows(FindOwnedNativeProc,(LPARAM)&candidate); return candidate.best;
}
static void CloseJumpPopup(bool animate);
static void CancelBarEdgeDrag(bool revert);
// Undocumented external values are read-only signals, never written by this
// mod. Geometry remains the authoritative signal when it really changes.
static int ReadModernTaskbarEdge() {
    DWORD value=0,size=sizeof(value);
    if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",L"TaskbarLocation",RRF_RT_REG_DWORD,nullptr,&value,&size)!=ERROR_SUCCESS || value>3)return -1;
    return (int)value;
}
static int ParseClassicTaskbarEdge(const BYTE* data,size_t size) {
    if(!data || size<16)return -1;DWORD edge=0;memcpy(&edge,data+12,sizeof(edge));return edge<=3?(int)edge:-1;
}
static int ReadClassicTaskbarEdge() {
    DWORD type=0,size=0;const wchar_t* path=L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRects3";
    HKEY key=nullptr;if(RegOpenKeyExW(HKEY_CURRENT_USER,path,0,KEY_QUERY_VALUE,&key)!=ERROR_SUCCESS)return -1;
    LSTATUS st=RegQueryValueExW(key,L"Settings",nullptr,&type,nullptr,&size);int result=-1;
    if(st==ERROR_SUCCESS && type==REG_BINARY && size>=16 && size<=4096) {
        BYTE bytes[4096]{};DWORD got=size;
        if(RegQueryValueExW(key,L"Settings",nullptr,&type,bytes,&got)==ERROR_SUCCESS && type==REG_BINARY)result=ParseClassicTaskbarEdge(bytes,got);
    }
    RegCloseKey(key);return result;
}
static bool ChangedKnownEdge(int previous,int now) {return now>=0 && now!=previous;}
static int HorizontalOverrideForSource(int edge) {return edge==ABE_TOP?1:edge==ABE_BOTTOM?2:0;}
static void SaveBarEdgePreference() {
    if(!s)return;
    wchar_t saved[40]{};swprintf(saved,std::size(saved),L"%d,%d,%d",s->sourceNativeEdge,s->sourceModernEdge,s->sourceClassicEdge);
    bool a=Wh_SetStringValue(L"Taskbar81.PlacementSources",saved)!=FALSE;
    bool b=Wh_SetStringValue(L"Taskbar81.PlacementEdge",s->barEdgeOverride==1?L"1":s->barEdgeOverride==2?L"2":s->barEdgeOverride==3?L"3":s->barEdgeOverride==4?L"4":L"0")!=FALSE;
    if(!a || !b)Wh_Log(L"[T81-DIAG] visual bar placement preference could not be saved");
}
static void ObserveExternalBarEdge(int nativeEdge) {
    if(!s || nativeEdge<0)return;
    int modern=ReadModernTaskbarEdge(),classic=ReadClassicTaskbarEdge();
    int oldNative=s->sourceNativeEdge,oldModern=s->sourceModernEdge,oldClassic=s->sourceClassicEdge;
    bool first=!s->edgeSourcesInitialized;
    if(first) {
        wchar_t saved[40]{};int n=-1,m=-1,c=-1;
        bool baseline=Wh_GetStringValue(L"Taskbar81.PlacementSources",saved,std::size(saved))>0 &&
            swscanf(saved,L"%d,%d,%d",&n,&m,&c)==3 && n>=0 && n<=3 && m>=-1 && m<=3 && c>=-1 && c<=3;
        if(s->barEdgeOverride && !baseline) {
            // Upgrade from v34: an unqualified override could hide a change
            // made while the mod was disabled. Start by following the OS.
            s->barEdgeOverride=0;
            Wh_Log(L"[T81-DIAG] old visual position had no system baseline; following native bar");
        }
        if(baseline && s->barEdgeOverride) {oldNative=n;oldModern=m;oldClassic=c;}
        else {oldNative=nativeEdge;oldModern=modern;oldClassic=classic;}
        s->edgeSourcesInitialized=true;
    }
    bool nativeChanged=ChangedKnownEdge(oldNative,nativeEdge);
    bool modernChanged=ChangedKnownEdge(oldModern,modern);
    bool classicChanged=ChangedKnownEdge(oldClassic,classic);
    s->sourceNativeEdge=nativeEdge;
    if(modern>=0)s->sourceModernEdge=modern;else if(first)s->sourceModernEdge=oldModern;
    if(classic>=0)s->sourceClassicEdge=classic;else if(first)s->sourceClassicEdge=oldClassic;
    if(!nativeChanged && !modernChanged && !classicChanged)return;
    if(s->barDragCandidate && taskbarPlacementMode.load()==3 && !nativeChanged) {
        Wh_Log(L"[T81-DIAG] registry edge signal observed during four-edge gesture; baseline refreshed, gesture retains priority modern=%d classic=%d",modern,classic);
        return;
    }
    // Actual native move wins over registry signals arriving together. With
    // no physical move, newer Windows signal wins over classic if simultaneous.
    int eventEdge=nativeChanged?nativeEdge:modernChanged?modern:classic;
    const wchar_t* source=nativeChanged?L"native rectangle":modernChanged?L"Windows TaskbarLocation":L"classic StuckRects3";
    if(eventEdge!=ABE_TOP && eventEdge!=ABE_BOTTOM) {
        if(taskbarPlacementMode.load()==3 && !nativeChanged && (s->privateActive || first) && (eventEdge==ABE_LEFT || eventEdge==ABE_RIGHT)) {
            if(s->barDragCandidate)CancelBarEdgeDrag(false);s->barEdgeOverride=eventEdge==ABE_LEFT?3:4;
            HidePrivateOverflow();HidePreview();CloseJumpPopup(false);CloseDesktopPopup();PeekOff(true);
            SaveBarEdgePreference();Wh_Log(L"[T81-DIAG] external side edge=%d requested integrated taskbar placement",eventEdge);return;
        }
        Wh_Log(L"[T81-DIAG] external %ls edge=%d is vertical/not handled; waiting for native geometry",source,eventEdge);
        SaveBarEdgePreference();return;
    }
    if(s->barDragCandidate)CancelBarEdgeDrag(false);
    s->barEdgeOverride=nativeChanged?0:HorizontalOverrideForSource(eventEdge);
    if(!s->privateActive && !first)s->barEdgeOverride=0;
    if(s->privateActive) {HidePrivateOverflow();HidePreview();CloseJumpPopup(false);CloseDesktopPopup();PeekOff(true);}
    SaveBarEdgePreference();
    Wh_Log(L"[T81-DIAG] external taskbar edge=%d from %ls; visual follows; native work area can differ",eventEdge,source);
}
// Optional AppBar adapter following ManagedShell/RetroBar's documented
// ABM_NEW -> QUERYPOS -> SETPOS -> REMOVE lifecycle. Never changes Explorer's
// global auto-hide setting. All coordinates are physical pixels.
#ifdef TASKBAR81_TEST
static decltype(&SHAppBarMessage) t81AppBarMessage=SHAppBarMessage;
#else
#define t81AppBarMessage SHAppBarMessage
#endif
// Microsoft documents QUNS_BUSY for a running fullscreen application and the
// D3D value for exclusive mode. ABN_FULLSCREENAPP adds immediate transitions
// when registered; the normal placement refresh covers the default path.
struct CaptureOverlayCheck { DWORD pid; RECT monitor; bool found; };
static BOOL CALLBACK FindCaptureOverlayWindow(HWND h,LPARAM value) {
    auto* c=reinterpret_cast<CaptureOverlayCheck*>(value);
    DWORD pid=0;GetWindowThreadProcessId(h,&pid);
    if(pid!=c->pid || !IsWindowVisible(h) || IsIconic(h))return TRUE;
    RECT r{};if(!GetWindowRect(h,&r))return TRUE;
    LONG_PTR style=GetWindowLongPtrW(h,GWL_STYLE),ex=GetWindowLongPtrW(h,GWL_EXSTYLE);
    const int tolerance=std::max(2,Px(2));
    if(r.left<=c->monitor.left+tolerance && r.top<=c->monitor.top+tolerance &&
       r.right>=c->monitor.right-tolerance && r.bottom>=c->monitor.bottom-tolerance &&
       ((ex & (WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE)) || !(style & WS_CAPTION))) {
        c->found=true;return FALSE;
    }
    return TRUE;
}
static bool IsSnippingCaptureOverlay() {
    if(!s || !s->bar)return false;
    HWND foreground=GetForegroundWindow();DWORD pid=0;
    if(!foreground || !GetWindowThreadProcessId(foreground,&pid) || !pid)return false;
    std::wstring path=ProcessImage(pid);if(path.empty())return false;
    const wchar_t* name=PathFindFileNameW(path.c_str());
    if(!name || (_wcsicmp(name,L"SnippingTool.exe") && _wcsicmp(name,L"ScreenSketch.exe") &&
                 _wcsicmp(name,L"ScreenClippingHost.exe")))return false;
    MONITORINFO mi{};mi.cbSize=sizeof(mi);
    if(!GetMonitorInfoW(MonitorFromWindow(s->bar,MONITOR_DEFAULTTONEAREST),&mi))return false;
    CaptureOverlayCheck check{pid,mi.rcMonitor,false};
    EnumWindows(FindCaptureOverlayWindow,reinterpret_cast<LPARAM>(&check));
    return check.found;
}
static bool ShouldHideForFullScreen() {
    QUERY_USER_NOTIFICATION_STATE q{};
    bool known=SUCCEEDED(SHQueryUserNotificationState(&q));
    // Preserve real exclusive fullscreen and presentation behavior. Snipping
    // can create a borderless fullscreen selection overlay and be mistaken
    // for a fullscreen app; don't hide an already-visible bar in that case.
    if(known && (q==QUNS_RUNNING_D3D_FULL_SCREEN || q==QUNS_PRESENTATION_MODE))return true;
    bool requested=(s && s->appbarFullScreen) || (known && q==QUNS_BUSY);
    if(!requested)return false;
    bool capture=IsSnippingCaptureOverlay();
    static int last=-1;int current=capture?1:0;
    if(last!=current) {
        Wh_Log(L"[T81-CAPTURE] fullscreen request=%d state=%d capture-overlay=%d",requested?1:0,known?(int)q:-1,current);
        last=current;
    }
    return !capture;
}
static void RemoveWorkAreaAppBar() {
    if(!s)return;
    if(s->bar) {KillTimer(s->bar,TIMER_APPBAR_NOTIFY);KillTimer(s->bar,TIMER_APPBAR_VERIFY);}
    if(!s->appbarRegistered)return;
    APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;
    s->appbarRegistered=false;s->appbarDirty=false;s->appbarArranging=false;s->appbarFullScreen=false;
    s->appbarDesired=RECT{};s->appbarApproved=RECT{};
    if(!t81AppBarMessage(ABM_REMOVE,&abd)) Wh_Log(L"[T81-DIAG] work-area AppBar remove returned failure");
    else Wh_Log(L"[T81-DIAG] work-area AppBar removed");
}
static bool ValidApprovedAppbar(const RECT& r,const RECT& screen,int height) {
    return height>0 && r.bottom-r.top==height && r.right>r.left &&
           r.left>=screen.left && r.right<=screen.right && r.top>=screen.top && r.bottom<=screen.bottom;
}
static void ReserveWorkAreaAppBar(RECT& desired,const MONITORINFO& mi) {
    if(!s || !s->bar)return;
    int native=s->sourceNativeEdge;UINT edge=s->nativeEdge;
    bool needs=reserveWorkArea.load() && s->privateActive && !s->barDragCandidate &&
        (native==ABE_TOP || native==ABE_BOTTOM) && (edge==ABE_TOP || edge==ABE_BOTTOM) && native!=(int)edge;
    if(!needs) {if(!s->barDragCandidate)RemoveWorkAreaAppBar();return;}
    if(!s->appbarRegistered && GetTickCount64()<s->appbarRetryAt)return;
    if(!s->appbarRegistered) {
        APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;abd.uCallbackMessage=WM_T81_APPBAR;
        if(!t81AppBarMessage(ABM_NEW,&abd)) {s->appbarRetryAt=GetTickCount64()+30000;Wh_Log(L"[T81-DIAG] work-area AppBar registration failed; visual placement only");return;}
        s->appbarRegistered=true;s->appbarDirty=true;Wh_Log(L"[T81-DIAG] work-area AppBar registered");
    }
    if(!s->appbarDirty && s->appbarEdge==edge && EqualRect(&desired,&s->appbarDesired)) {desired=s->appbarApproved;return;}
    APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;abd.uEdge=edge;abd.rc=desired;
    int height=desired.bottom-desired.top;
    t81AppBarMessage(ABM_QUERYPOS,&abd);
    if(edge==ABE_TOP)abd.rc.bottom=abd.rc.top+height;else abd.rc.top=abd.rc.bottom-height;
    // A SETPOS notifies appbars, including potentially ourselves. Re-query on
    // notification, but don't send SETPOS again if nothing actually changed.
    if(s->appbarEdge==edge && EqualRect(&desired,&s->appbarDesired) && EqualRect(&abd.rc,&s->appbarApproved)) {
        s->appbarDirty=false;desired=s->appbarApproved;return;
    }
    if(!ValidApprovedAppbar(abd.rc,mi.rcMonitor,height) || !t81AppBarMessage(ABM_SETPOS,&abd) || !ValidApprovedAppbar(abd.rc,mi.rcMonitor,height)) {
        Wh_Log(L"[T81-DIAG] work-area AppBar position rejected; visual placement only");RemoveWorkAreaAppBar();s->appbarRetryAt=GetTickCount64()+30000;return;
    }
    s->appbarEdge=edge;s->appbarDesired=desired;s->appbarApproved=abd.rc;s->appbarDirty=false;desired=abd.rc;
    SetTimer(s->bar,TIMER_APPBAR_VERIFY,650,nullptr);
    Wh_Log(L"[T81-DIAG] work-area AppBar approved edge=%u rect=%ld,%ld,%ld,%ld; native edge can still reserve space",edge,abd.rc.left,abd.rc.top,abd.rc.right,abd.rc.bottom);
}
// Separate opt-in for vertical AppBars; horizontal AppBar behavior is unchanged.
// Like the horizontal path, a shell-approved rectangle is not proof that the
// work area changed: TIMER_APPBAR_VERIFY observes it and removes on failure.
static bool AppbarWorkAreaObserved(UINT edge,const RECT& approved,const RECT& work) {
    switch(edge) {case ABE_LEFT:return work.left>=approved.right;case ABE_RIGHT:return work.right<=approved.left;
        case ABE_TOP:return work.top>=approved.bottom;case ABE_BOTTOM:return work.bottom<=approved.top;default:return false;}
}
static bool ValidVerticalAppbar(const RECT& r,const RECT& screen,int width) {
    return width>0 && r.right-r.left==width && r.bottom-r.top>=Px(320) &&
        r.left>=screen.left && r.right<=screen.right && r.top>=screen.top && r.bottom<=screen.bottom;
}
static void ReserveVerticalAppbar(RECT& desired,const MONITORINFO& mi,int side) {
    if(!s || !s->bar)return;
    if(s->barDragCandidate)return; // do not repeatedly register or remove while dragging
    if(!reserveVerticalWorkArea.load() || !s->privateActive || (side!=1 && side!=2)) {RemoveWorkAreaAppBar();return;}
    if(!s->appbarRegistered && GetTickCount64()<s->appbarRetryAt)return;
    UINT edge=side==1?ABE_LEFT:ABE_RIGHT;
    if(!s->appbarRegistered) {
        APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;abd.uCallbackMessage=WM_T81_APPBAR;
        if(!t81AppBarMessage(ABM_NEW,&abd)) {s->appbarRetryAt=GetTickCount64()+30000;Wh_Log(L"[T81-DIAG] vertical AppBar registration failed; visual only");return;}
        s->appbarRegistered=true;s->appbarDirty=true;Wh_Log(L"[T81-DIAG] vertical AppBar registered");
    }
    if(!s->appbarDirty && s->appbarEdge==edge && EqualRect(&desired,&s->appbarDesired)) {desired=s->appbarApproved;return;}
    APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;abd.uEdge=edge;abd.rc=desired;int width=desired.right-desired.left;
    UINT_PTR queryResult=t81AppBarMessage(ABM_QUERYPOS,&abd);
    RECT queried=abd.rc;
    if(edge==ABE_LEFT)abd.rc.right=abd.rc.left+width;else abd.rc.left=abd.rc.right-width;
    if(s->appbarEdge==edge && EqualRect(&desired,&s->appbarDesired) && EqualRect(&abd.rc,&s->appbarApproved)) {s->appbarDirty=false;
        desired=s->appbarApproved;
        return;
        }
    bool validQuery=ValidVerticalAppbar(abd.rc,mi.rcMonitor,width);
    UINT_PTR setResult=validQuery?t81AppBarMessage(ABM_SETPOS,&abd):0;
    bool validSet=setResult && ValidVerticalAppbar(abd.rc,mi.rcMonitor,width);
    if(!validQuery || !validSet) {
        HWND shellFirst=FindWindowW(L"Shell_TrayWnd",nullptr);DWORD shellPid=0;GetWindowThreadProcessId(shellFirst,&shellPid);
        Wh_Log(L"[T81-DIAG] vertical AppBar query result=%llu first-shell=%p first-pid=%lu proxy=%p native=%p; no auto-hide change",
            (unsigned long long)queryResult,shellFirst,shellPid,s->privateHost,s->native);
        Wh_Log(L"[T81-DIAG] vertical AppBar position rejected stage=%ls edge=%u desired=%ld,%ld,%ld,%ld query=%ld,%ld,%ld,%ld final=%ld,%ld,%ld,%ld monitor=%ld,%ld,%ld,%ld set=%llu; visual only",
            !validQuery?L"query":!setResult?L"set-return":L"set-rect",edge,desired.left,desired.top,desired.right,desired.bottom,
            queried.left,queried.top,queried.right,queried.bottom,abd.rc.left,abd.rc.top,abd.rc.right,abd.rc.bottom,
            mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right,mi.rcMonitor.bottom,(unsigned long long)setResult);
        RemoveWorkAreaAppBar();s->appbarRetryAt=GetTickCount64()+30000;return;
    }
    s->appbarEdge=edge;s->appbarDesired=desired;s->appbarApproved=abd.rc;s->appbarDirty=false;desired=abd.rc;
    SetTimer(s->bar,TIMER_APPBAR_VERIFY,650,nullptr);
    Wh_Log(L"[T81-DIAG] vertical AppBar approved edge=%u rect=%ld,%ld,%ld,%ld; work area not yet verified",edge,abd.rc.left,abd.rc.top,abd.rc.right,abd.rc.bottom);
}

void UpdatePlacement() {
    if(quitting.load() || !s || !s->bar) return;
    if(s->barEdgeOverride>=3 && taskbarPlacementMode.load()!=3) {
        if(s->barDragCandidate)CancelBarEdgeDrag(false);
        s->barEdgeOverride=0;if(s->edgeSourcesInitialized)SaveBarEdgePreference();
    }
    HWND native=s->privateActive?s->native:FindOwnedNativeTaskbar();
    if(native) {
        DWORD pid=0;
        GetWindowThreadProcessId(native,&pid);
        if(pid!=GetCurrentProcessId()) native=nullptr;
    }
    if(native!=s->native) {
        s->native=native;
        s->rectValid=false;
        s->trayColors.clear();
        nativeWindow.store(native);
    }
    if(s->privateActive && (!native || !IsWindow(native))) {
        // Never leave an orphan private Shell_TrayWnd when Explorer replaces
        // the original HWND. Stop redirects first; on the next refresh the
        // actual Explorer taskbar can be rediscovered by FindWindow.
        StopPrivateTray();
        s->native=nullptr; s->rectValid=false; nativeWindow.store(nullptr);
        return;
    }
    RECT full{};
    bool gotNative=native && (s->privateActive || IsWindowVisible(native)) && GetWindowRect(native,&full) && full.right>full.left && full.bottom>full.top;
    if(gotNative) {
        MONITORINFO detected{};detected.cbSize=sizeof(detected);RECT visible{};
        bool onMonitor=GetMonitorInfoW(MonitorFromRect(&full,MONITOR_DEFAULTTONEAREST),&detected) && IntersectRect(&visible,&full,&detected.rcMonitor) &&
            visible.right-visible.left>=(full.right-full.left)/2 && visible.bottom-visible.top>=(full.bottom-full.top)/2;
        if(onMonitor) {
            BarEdge edge=EdgeForRect(full,detected.rcMonitor); UINT value=edge==BarEdge::Top?ABE_TOP:edge==BarEdge::Left?ABE_LEFT:edge==BarEdge::Right?ABE_RIGHT:ABE_BOTTOM;
            ObserveExternalBarEdge((int)value);
            if(!s->privateActive || !s->barEdgeOverride) {
                if(s->nativeEdge!=value) Wh_Log(L"[T81-DIAG] native taskbar edge=%ls (vertical layout not implemented)",EdgeName(edge));
                s->nativeEdge=value;
            }
            // Never cover or retain a hidden native vertical taskbar with the
            // horizontal-only UI. Stop forwarding and restore it instead.
            if(!HorizontalEdge(edge)) {
                KillTimer(s->bar,TIMER_DESKTOP_PEEK);PeekOff(true);CloseDesktopPopup();CloseJumpPopup(false);HidePreview();
                if(s->privateActive) StopPrivateTray();
                s->rectValid=false; if(IsWindowVisible(s->bar)) { Wh_Log(L"[T81-CAPTURE] bar hidden: native taskbar geometry or visibility unavailable"); ShowWindow(s->bar,SW_HIDE); }
                UpdateNativeControlOverlays(nullptr,RECT{},0,false);UpdateAddressBand();return;
            }
        }
    }
    bool validNative=gotNative && full.right-full.left>full.bottom-full.top;
    if(!validNative && s->privateActive && native && IsWindow(native)) {
        full=s->nativeRect;
        if(full.right<=full.left || full.bottom<=full.top) full=FallbackRect();
        validNative=true;
    }
    if(!validNative) {
        KillTimer(s->bar,TIMER_DESKTOP_PEEK); PeekOff(true);
        s->rectValid=false;
        s->trayColors.clear();
        if(IsWindowVisible(s->bar)) { Wh_Log(L"[T81-CAPTURE] bar hidden: native taskbar geometry or visibility unavailable"); ShowWindow(s->bar,SW_HIDE); }
        UpdateNativeControlOverlays(nullptr,RECT{},0,false); UpdateAddressBand();
        return;
    }
    MONITORINFO mi{}; mi.cbSize=sizeof(mi);
    RECT visible{};
    bool onScreen=GetMonitorInfoW(MonitorFromRect(&full,MONITOR_DEFAULTTONEAREST),&mi) &&
        IntersectRect(&visible,&full,&mi.rcMonitor) &&
        visible.bottom-visible.top>=(full.bottom-full.top)/2;
    if(!onScreen && s->privateActive) {
        // Some shells move a hidden taskbar off-screen. A working private bar
        // must not disappear with it; use the primary-screen fallback instead.
        full=FallbackRect();
        mi.cbSize=sizeof(mi);
        onScreen=GetMonitorInfoW(MonitorFromRect(&full,MONITOR_DEFAULTTOPRIMARY),&mi)!=FALSE;
    }
    if(!onScreen) {
        KillTimer(s->bar,TIMER_DESKTOP_PEEK); PeekOff(true);
        s->rectValid=false;
        s->trayColors.clear();
        if(IsWindowVisible(s->bar)) { Wh_Log(L"[T81-CAPTURE] bar hidden: native taskbar geometry or visibility unavailable"); ShowWindow(s->bar,SW_HIDE); }
        UpdateNativeControlOverlays(nullptr,RECT{},0,false); UpdateAddressBand();
        return;
    }
    // Preserve native geometry separately from our visual top/bottom override.
    s->nativeRect=full;
    if(s->privateActive && (s->barEdgeOverride==1 || s->barEdgeOverride==2)) {
        int height=full.bottom-full.top;
        if(height>0 && height<mi.rcMonitor.bottom-mi.rcMonitor.top) {
            if(s->barEdgeOverride==1) {full.top=mi.rcMonitor.top;full.bottom=full.top+height;s->nativeEdge=ABE_TOP;}
            else {full.bottom=mi.rcMonitor.bottom;full.top=full.bottom-height;s->nativeEdge=ABE_BOTTOM;}
        }
    }
    int mode=taskbarPlacementMode.load();
    int requested=mode==3?(s->barEdgeOverride==3?1:s->barEdgeOverride==4?2:0):mode;
    bool vertical=s->privateActive && requested && mi.rcMonitor.bottom-mi.rcMonitor.top>=Px(320);
    if(vertical) {
        // Convert the logical vertical bar width to physical pixels.
        int width=std::max(1,Px(useBetaStartLogo.load() && s->betaStartImage?61:kVerticalWidthLogical));
        full=mi.rcMonitor;
        if(requested==1)full.right=full.left+width;else full.left=full.right-width;
    }
    if(s->verticalActive!=vertical || (vertical && s->verticalSide!=requested)) {
        HidePrivateOverflow();HidePreview();CloseJumpPopup(false);CloseDesktopPopup();PeekOff(true);s->verticalPressed=HIT_NONE;s->verticalOffset=0;
        Wh_Log(L"[T81-DIAG] vertical taskbar active=%d side=%d mode=%d override=%d dragging=%d; reservation separately gated",vertical?1:0,requested,mode,s->barEdgeOverride,s->barDragCandidate?1:0);
    }
    bool groupingEdgeChanged=s->verticalActive!=vertical;
    s->verticalActive=vertical;s->verticalSide=vertical?requested:0;
    if(groupingEdgeChanged)BuildGroups();
    if(s->privateActive && vertical)ReserveVerticalAppbar(full,mi,requested);
    else if(s->privateActive)ReserveWorkAreaAppBar(full,mi);
    else RemoveWorkAreaAppBar();
    s->rectValid=true;
    s->preservingTray=!s->privateActive;
    RECT overlay=full;
    if(!s->privateActive) overlay.right=NativeTrayLeft(native,full);
    RECT tray=full;
    tray.left=overlay.right;
    bool fullScreen=ShouldHideForFullScreen();
    // The capture overlay is allowed to stay above us; we only suppress an
    // erroneous fullscreen hide, not raise the bar over the capture UI.
    if(fullScreen || s->appbarArranging) {
        HideLanguageFlyout();KillTimer(s->bar,TIMER_DESKTOP_PEEK); PeekOff(true);
        if(IsWindowVisible(s->bar)) {
            Wh_Log(L"[T81-CAPTURE] bar hidden fullscreen=%d arranging=%d",fullScreen?1:0,s->appbarArranging?1:0);
            ShowWindow(s->bar,SW_HIDE);
        }
    } else {
        RECT old{};
        bool wasVisible=IsWindowVisible(s->bar)!=FALSE;
        bool same=GetWindowRect(s->bar,&old) && EqualRect(&old,&overlay);
        if((!wasVisible || !same) && (wasVisible || !IsSnippingCaptureOverlay())) {
            if(!same)HideLanguageFlyout();
            UINT flags=SWP_NOACTIVATE|SWP_SHOWWINDOW;
            if(wasVisible) flags|=SWP_NOZORDER;
            SetWindowPos(s->bar,HWND_TOPMOST,overlay.left,overlay.top,
                overlay.right-overlay.left,overlay.bottom-overlay.top,flags);
            if(!same) {
                InvalidateRect(s->bar,nullptr,FALSE);UpdateWindow(s->bar);
                HWND menu=FindWindowW(L"#32768",nullptr);
                if(gWin7OrbReady && gWin7OrbWindow && IsWindowVisible(gWin7OrbWindow) &&
                   !IsSnippingCaptureOverlay() && !(menu && IsWindowVisible(menu))) {
                    if(!SetWindowPos(gWin7OrbWindow,HWND_TOPMOST,0,0,0,0,
                        SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER))
                        Wh_Log(L"[T81-ORB] z-order restore failed error=%lu",GetLastError());
                }
            }
            if(s->appbarRegistered && !same) {APPBARDATA abd{};abd.cbSize=sizeof(abd);abd.hWnd=s->bar;t81AppBarMessage(ABM_WINDOWPOSCHANGED,&abd);}
        }
        if(matchNativeTray.load() && !s->privateActive) {
            std::vector<COLORREF> rows;
            if(SampleNativeTrayRows(native,tray,rows)) s->trayColors.swap(rows);
        } else s->trayColors.clear();
    }
    if(s->privateActive) UpdateNativeControlOverlays(nullptr,RECT{},0,false);
    else UpdateNativeControlOverlays(native,full,overlay.right,!fullScreen);
    UpdateAddressBand();
}

// StartAllBack may raise Shell_TrayWnd after this overlay is placed. Detect
// only that specific z-order inversion and reposition OUR window, never the
// shell window. Avoid menu/fullscreen interference and suspend if another
// component repeatedly fights the change. These are documented USER APIs;
// they do not guarantee priority against a continuously reasserting shell.
static bool IsAbove(HWND possibleAbove, HWND window) {
    if(!possibleAbove || !window) return false;
    HWND current=window;
    for(unsigned i=0;i<1024;i++) {
        current=GetWindow(current,GW_HWNDPREV);
        if(!current) return false;
        if(current==possibleAbove) return true;
    }
    return false;
}

static void GuardOverlayZOrder() {
    if(!s || !keepOverlayAbove.load() || !s->bar || !s->native ||
       !IsWindowVisible(s->bar) || !IsWindowVisible(s->native)) return;
    const bool barBehind=IsAbove(s->native,s->bar);
    const bool clockBehind=s->clockOverlay && IsWindowVisible(s->clockOverlay) && IsAbove(s->native,s->clockOverlay);
    const bool volumeBehind=s->volumeOverlay && IsWindowVisible(s->volumeOverlay) && IsAbove(s->native,s->volumeOverlay);
    if(!barBehind && !clockBehind && !volumeBehind) return;
    HWND menu=FindWindowW(L"#32768",nullptr);
    if(menu && IsWindowVisible(menu)) return;
    if(ShouldHideForFullScreen()) return;
    ULONGLONG now=GetTickCount64();
    if(now<s->zGuardSuspendedUntil) return;
    s->zRepairs.erase(std::remove_if(s->zRepairs.begin(),s->zRepairs.end(),
        [now](ULONGLONG t){return now>=t && now-t>10000;}),s->zRepairs.end());
    if(s->zRepairs.size()>=6) {
        s->zGuardSuspendedUntil=now+15000;
        s->zRepairs.clear();
        Wh_Log(L"Repeated native z-order conflict; taskbar overlay guard paused for 15 seconds");
        return;
    }
    bool repaired=false;
    auto raise=[&](HWND window) {
        if(window && SetWindowPos(window,HWND_TOPMOST,0,0,0,0,
                         SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER)) repaired=true;
    };
    if(barBehind) {
        raise(s->bar);
        if(gWin7OrbReady && gWin7OrbWindow && IsWindowVisible(gWin7OrbWindow) && !IsSnippingCaptureOverlay())raise(gWin7OrbWindow);
    }
    if(clockBehind) raise(s->clockOverlay);
    if(volumeBehind) raise(s->volumeOverlay);
    if(repaired) s->zRepairs.push_back(now);
}

// Bounded, read-only diagnostics for the Windows 11/StartAllBack machine.
// No titles, tooltips, app IDs, module paths, SSIDs, IPs, screenshots or text
// contents are logged by these routines. Handles, class names, positions,
// process/thread IDs and adapter types remain visible in the Windhawk log.
static bool DiagRootClass(const wchar_t* cls) {
    return !_wcsicmp(cls,L"Shell_TrayWnd") || !_wcsicmp(cls,L"Shell_SecondaryTrayWnd") ||
        !_wcsicmp(cls,L"NotifyIconOverflowWindow") || !_wcsicmp(cls,L"TopLevelWindowForOverflowXamlIsland") ||
        !_wcsnicmp(cls,L"WH_Taskbar81",12);
}
static void DiagWindow(HWND h,const wchar_t* scope) {
    wchar_t cls[128]={}; GetClassNameW(h,cls,(int)std::size(cls));
    DWORD pid=0,tid=GetWindowThreadProcessId(h,&pid); RECT r{}; bool ok=GetWindowRect(h,&r)!=FALSE;
    Wh_Log(L"[T81-DIAG] %ls hwnd=%p pid=%lu tid=%lu class=%ls visible=%d rect=%ld,%ld,%ld,%ld rect-valid=%d parent=%p owner=%p style=0x%08lx exstyle=0x%08lx id=%d",
        scope,h,pid,tid,cls,IsWindowVisible(h)?1:0,r.left,r.top,r.right,r.bottom,ok?1:0,
        GetParent(h),GetWindow(h,GW_OWNER),(unsigned long)GetWindowLongW(h,GWL_STYLE),
        (unsigned long)GetWindowLongW(h,GWL_EXSTYLE),GetDlgCtrlID(h));
}
struct DiagWindowContext { unsigned top=0, child=0; RECT nativeRect{}; bool nativeRectValid=false; };
static BOOL CALLBACK DiagChildProc(HWND h,LPARAM p) {
    auto& ctx=*reinterpret_cast<DiagWindowContext*>(p);
    DWORD pid=0; GetWindowThreadProcessId(h,&pid);
    if(pid==GetCurrentProcessId() && ctx.child<120) { DiagWindow(h,L"child"); ++ctx.child; }
    return ctx.child<120;
}
static BOOL CALLBACK DiagTopProc(HWND h,LPARAM p) {
    auto& ctx=*reinterpret_cast<DiagWindowContext*>(p);
    DWORD pid=0; GetWindowThreadProcessId(h,&pid); if(pid!=GetCurrentProcessId()) return TRUE;
    wchar_t cls[128]={}; GetClassNameW(h,cls,(int)std::size(cls));
    bool root=DiagRootClass(cls);
    bool start=!_wcsicmp(cls,L"Start") || !_wcsicmp(cls,L"Button") || !_wcsicmp(cls,L"StartButton");
    bool nearBar=false; RECT r{};
    if(ctx.nativeRectValid && GetWindowRect(h,&r)) {
        int lim=Px(200), w=r.right-r.left, ht=r.bottom-r.top;
        nearBar=w>0 && w<=lim && ht>0 && ht<=lim && r.right>ctx.nativeRect.left &&
             r.left<ctx.nativeRect.left+lim && r.bottom>ctx.nativeRect.top && r.top<ctx.nativeRect.bottom;
    }
    if((root || start || nearBar) && ctx.top<60) { DiagWindow(h,L"top"); ++ctx.top; }
    if(root && ctx.child<120) EnumChildWindows(h,DiagChildProc,(LPARAM)&ctx);
    return ctx.top<60 && ctx.child<120;
}
static void DiagStartAllBackVersion() {
    const wchar_t* names[]={L"StartAllBackX64.dll",L"StartAllBack.dll",L"StartAllBack64.dll"};
    HMODULE mod=nullptr; const wchar_t* loaded=nullptr;
    for(const auto* name:names) { mod=GetModuleHandleW(name); if(mod) {loaded=name; break;} }
    if(!mod) { Wh_Log(L"[T81-DIAG] StartAllBack module not found in this Explorer process"); return; }
    // Dynamically load the Windows version API from System32. The resolved
    // module path is used locally but never appears in the log.
    HMODULE version=LoadLibraryExW(L"version.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    wchar_t path[32768]={}; DWORD length=GetModuleFileNameW(mod,path,(DWORD)std::size(path));
    using SizeFn=DWORD (WINAPI*)(LPCWSTR,LPDWORD);
    using InfoFn=BOOL (WINAPI*)(LPCWSTR,DWORD,DWORD,LPVOID);
    using QueryFn=BOOL (WINAPI*)(LPCVOID,LPCWSTR,LPVOID*,PUINT);
    auto sizeFn=version?(SizeFn)GetProcAddress(version,"GetFileVersionInfoSizeW"):nullptr;
    auto infoFn=version?(InfoFn)GetProcAddress(version,"GetFileVersionInfoW"):nullptr;
    auto queryFn=version?(QueryFn)GetProcAddress(version,"VerQueryValueW"):nullptr;
    bool logged=false;
    if(length && length<std::size(path) && sizeFn && infoFn && queryFn) {
        DWORD dummy=0, bytes=sizeFn(path,&dummy);
        if(bytes && bytes<=1024*1024) {
            std::vector<BYTE> data(bytes); VS_FIXEDFILEINFO* fixed=nullptr; UINT fixedSize=0;
            if(infoFn(path,0,bytes,data.data()) && queryFn(data.data(),L"\\",(void**)&fixed,&fixedSize) &&
               fixed && fixedSize>=sizeof(VS_FIXEDFILEINFO) && fixed->dwSignature==0xFEEF04BD) {
                Wh_Log(L"[T81-DIAG] StartAllBack loaded=%ls file-version=%u.%u.%u.%u",loaded,
                    HIWORD(fixed->dwFileVersionMS),LOWORD(fixed->dwFileVersionMS),
                    HIWORD(fixed->dwFileVersionLS),LOWORD(fixed->dwFileVersionLS));
                logged=true;
            }
        }
    }
    if(!logged) Wh_Log(L"[T81-DIAG] StartAllBack loaded=%ls file-version=unavailable",loaded);
    if(version) FreeLibrary(version);
}
static void LogTaskbarDiagnostics(const wchar_t* reason) {
    if(!s) return;
    Wh_Log(L"[T81-DIAG] BEGIN reason=%ls mod=0.53.0 pid=%lu",reason,GetCurrentProcessId());
    Wh_Log(L"[T81-DIAG] work-area reservation setting=%d appbar-registered=%d",reserveWorkArea.load()?1:0,s && s->appbarRegistered?1:0);
    using RtlGetVersionFn=LONG (WINAPI*)(OSVERSIONINFOW*);
    HMODULE ntdll=GetModuleHandleW(L"ntdll.dll");
    auto rtl=ntdll?(RtlGetVersionFn)GetProcAddress(ntdll,"RtlGetVersion"):nullptr;
    OSVERSIONINFOW os{}; os.dwOSVersionInfoSize=sizeof(os);
    if(rtl && rtl(&os)==0) Wh_Log(L"[T81-DIAG] windows=%lu.%lu build=%lu",os.dwMajorVersion,os.dwMinorVersion,os.dwBuildNumber);
    DiagStartAllBackVersion();
    {
        HDC dc=GetDC(s->bar); HFONT font=dc?CreateTaskbarFont(Dpi(),s->perPixelBar):nullptr;
        HGDIOBJ old=(dc && font)?SelectObject(dc,font):nullptr;
        wchar_t face[LF_FACESIZE]={}; if(dc && font) GetTextFaceW(dc,LF_FACESIZE,face);
        if(dc && old && old!=HGDI_ERROR) SelectObject(dc,old);
        if(font) DeleteObject(font); if(dc) ReleaseDC(s->bar,dc);
        Wh_Log(L"[T81-DIAG] taskbar-GDI-font-requested=Segoe UI 9pt selected=%ls clock-GDIplus-font-requested=Segoe UI 9pt clock-width=%d per-pixel=%d",
               face[0]?face:L"unavailable",s->clockWidthPx,s->perPixelBar?1:0);
    }
    Wh_Log(L"[T81-DIAG] alpha-bar=%d alpha-bar-frame=%d background-opacity=%d alpha-preview-request=%d alpha-preview-active=%d preview-opacity=%d",
        s->perPixelBar?1:0,s->perPixelFrame?1:0,taskbarBackgroundOpacity.load(),perPixelPreview.load()?1:0,
        s->perPixelPreview?1:0,previewOpacity.load());
    Wh_Log(L"[T81-DIAG] bar=%p native=%p private-host=%p private-active=%d native-visible=%d dpi=%d opacity=%d",
        s->bar,s->native,s->privateHost,s->privateActive?1:0,s->native&&IsWindowVisible(s->native)?1:0,Dpi(),taskbarOpacity.load());
    Wh_Log(L"[T81-DIAG] orientation-native-edge=%u native-vertical-layout=not-handled vertical-active=%d",s->nativeEdge,s->verticalActive?1:0);
    Wh_Log(L"[T81-DIAG] native-glom=%d; horizontal grouping follows preference; vertical combines",s->nativeGlomLevel);
    Wh_Log(L"[T81-DIAG] desktop-peek enabled=%d preview-peek=%d dwm-allowed=%d available=%d delay-ms=%lu",s->desktopPeekEnabled?1:0,aeroPeek.load()?1:0,PeekAllowedByDwm()?1:0,DesktopPeekAvailable()?1:0,DesktopPeekDelay());
    Wh_Log(L"[T81-DIAG] tray-icons=%u visible=%u overflow=%u imported-snapshot=%d snapshot-timeout=%d preference-initialized=%d network-kind=%d battery-present=%d",
        (unsigned)s->trayIcons.size(),(unsigned)VisiblePrivateIcons().size(),(unsigned)OverflowPrivateItems().size(),
        s->nativeSnapshotActive?1:0,s->nativeSnapshotTimedOut?1:0,s->trayPreferencesInitialized?1:0,s->netKind,s->hasBattery?1:0);
    DWORD route=0; DWORD routeStatus=GetBestInterface(0x08080808u,&route);
    Wh_Log(L"[T81-DIAG] network-route-status=%lu route-ifindex=%lu",routeStatus,route);
    MIB_IF_TABLE2* table=nullptr; DWORD status=GetIfTable2(&table);
    if(status==NO_ERROR && table) {
        ULONG printed=0;
        for(ULONG i=0;i<table->NumEntries && printed<32;++i) {
            const auto& entry=table->Table[i];
            if(entry.Type!=IF_TYPE_IEEE80211 && entry.Type!=IF_TYPE_ETHERNET_CSMACD && entry.InterfaceIndex!=route) continue;
            Wh_Log(L"[T81-DIAG] adapter index=%lu type=%lu oper=%u hardware=%u",
                entry.InterfaceIndex,(unsigned long)entry.Type,(unsigned)entry.OperStatus,(unsigned)entry.InterfaceAndOperStatusFlags.HardwareInterface);
            ++printed;
        }
        if(printed==32) Wh_Log(L"[T81-DIAG] adapter list truncated");
        FreeMibTable(table);
    } else Wh_Log(L"[T81-DIAG] network-table-status=%lu",status);
    DiagWindowContext ctx{};
    ctx.nativeRectValid=s->native && GetWindowRect(s->native,&ctx.nativeRect)!=FALSE;
    EnumWindows(DiagTopProc,(LPARAM)&ctx);
    if(ctx.top>=60 || ctx.child>=120) Wh_Log(L"[T81-DIAG] window list truncated");
    Wh_Log(L"[T81-DIAG] END reason=%ls",reason);
}

// Documented Shell Jump List subset using IApplicationDocumentLists
// (GPL-3.0-or-later).
// This API cannot enumerate pinned items, custom categories or application
// tasks. Do not infer or modify the user's private destination store. System
// implicit AppIDs are not retrievable, so only explicit app: keys are queried.
struct JumpDocument { std::wstring label; std::shared_ptr<IUnknown> object; };
static std::vector<JumpDocument> ReadJumpDocuments(const std::wstring& appId,bool& frequent) {
    std::vector<JumpDocument> docs; frequent=true;
    if(appId.empty()) return docs;
    Wh_Log(L"[T81-DIAG] Jump List provider query begin");
    IApplicationDocumentLists* lists=nullptr;
    HRESULT hr=CoCreateInstance(CLSID_ApplicationDocumentLists,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&lists));
    std::unique_ptr<IApplicationDocumentLists,ReleaseCom<IApplicationDocumentLists>> listsGuard(lists);
    if(FAILED(hr) || !lists) { Wh_Log(L"[T81-DIAG] Jump List provider unavailable hr=0x%08lx",(unsigned long)hr); return docs; }
    hr=lists->SetAppID(appId.c_str());
    if(SUCCEEDED(hr)) for(int pass=0;pass<2 && docs.empty();++pass) {
        frequent=(pass==0); IObjectArray* arr=nullptr;
        hr=lists->GetList(frequent?ADLT_FREQUENT:ADLT_RECENT,10,IID_PPV_ARGS(&arr));
        std::unique_ptr<IObjectArray,ReleaseCom<IObjectArray>> arrGuard(arr);
        if(FAILED(hr) || !arr) continue;
        UINT count=0; arr->GetCount(&count); count=std::min(count,10u);
        for(UINT i=0;i<count;i++) {
            IUnknown* raw=nullptr; if(FAILED(arr->GetAt(i,IID_IUnknown,(void**)&raw)) || !raw) continue;
            std::shared_ptr<IUnknown> obj(raw,[](IUnknown* o){o->Release();});
            std::wstring label; IShellItem* item=nullptr;
            if(SUCCEEDED(raw->QueryInterface(IID_PPV_ARGS(&item))) && item) {
                std::unique_ptr<IShellItem,ReleaseCom<IShellItem>> itemGuard(item);
                PWSTR name=nullptr; HRESULT hn=item->GetDisplayName(SIGDN_NORMALDISPLAY,&name); std::unique_ptr<wchar_t,FreeCoTask> nameGuard(name);
                if(SUCCEEDED(hn) && name) label=name;
            } else {
                IShellLinkW* link=nullptr;
                if(SUCCEEDED(raw->QueryInterface(IID_PPV_ARGS(&link))) && link) {
                    std::unique_ptr<IShellLinkW,ReleaseCom<IShellLinkW>> linkGuard(link);
                    wchar_t path[32768]={},description[1024]={};
                    link->GetDescription(description,(int)std::size(description));
                    WIN32_FIND_DATAW fd{}; link->GetPath(path,(int)std::size(path),&fd,SLGP_RAWPATH);
                    label=description[0]?description:PathFindFileNameW(path);
                }
            }
            for(auto& c:label) if(c<L' ') c=L' ';
            if(label.size()>140) label.resize(140);
            if(!label.empty()) docs.push_back({label,std::move(obj)});
        }
    }
    Wh_Log(L"[T81-DIAG] Jump List automatic entries=%u",(unsigned)docs.size());
    return docs;
}
static bool OpenJumpDocument(HWND owner,IUnknown* object) {
    if(!object) return false;
    IShellItem* item=nullptr;
    if(SUCCEEDED(object->QueryInterface(IID_PPV_ARGS(&item))) && item) {
        PIDLIST_ABSOLUTE pidl=nullptr; HRESULT hr=SHGetIDListFromObject(item,&pidl); item->Release();
        if(SUCCEEDED(hr) && pidl) {
            SHELLEXECUTEINFOW se{}; se.cbSize=sizeof(se); se.hwnd=owner; se.fMask=SEE_MASK_IDLIST|SEE_MASK_NOASYNC;
            se.lpVerb=L"open"; se.lpIDList=pidl; se.nShow=SW_SHOWNORMAL;
            bool ok=ShellExecuteExW(&se)!=FALSE; CoTaskMemFree(pidl); return ok;
        }
    }
    IShellLinkW* link=nullptr;
    if(SUCCEEDED(object->QueryInterface(IID_PPV_ARGS(&link))) && link) {
        wchar_t path[32768]={},args[32768]={},work[32768]={}; WIN32_FIND_DATAW fd{};
        link->GetPath(path,(int)std::size(path),&fd,SLGP_RAWPATH);
        link->GetArguments(args,(int)std::size(args)); link->GetWorkingDirectory(work,(int)std::size(work));
        PIDLIST_ABSOLUTE pidl=nullptr; if(!path[0]) link->GetIDList(&pidl); link->Release();
        SHELLEXECUTEINFOW se{}; se.cbSize=sizeof(se); se.hwnd=owner; se.lpVerb=L"open"; se.nShow=SW_SHOWNORMAL;
        se.fMask=SEE_MASK_NOASYNC|(pidl?SEE_MASK_IDLIST:0); se.lpIDList=pidl; se.lpFile=path[0]?path:nullptr;
        se.lpParameters=args[0]?args:nullptr; se.lpDirectory=work[0]?work:nullptr;
        bool ok=(pidl || path[0]) && ShellExecuteExW(&se); if(pidl) CoTaskMemFree(pidl); return ok;
    }
    return false;
}
// Native Win32 jump-view adaptation, GPL-3.0-or-later. One owner-window,
// timer and STA lifetime are used instead of WPF integration, a low-level
// mouse hook and a large class. Real recent/frequent objects use the
// documented Shell API.
static const wchar_t kJumpClass[]=L"WH_Taskbar81JumpList";
// Mod-owned pins: only validated applications and shortcuts are accepted.
// Original Windows shortcuts and Taskband are never modified.
struct ModPinCandidate {std::wstring source,exe,appId,name,identity; bool shortcut=false;};
static std::wstring ShellItemName(IShellItem* item,SIGDN kind) {
    PWSTR raw=nullptr;std::wstring value;
    if(item && SUCCEEDED(item->GetDisplayName(kind,&raw)) && raw) {value=raw;CoTaskMemFree(raw);}
    return value;
}
static bool InspectModPin(IShellItem* item,ModPinCandidate& out) {
    if(!item)return false;
    ModPinCandidate c;c.source=ShellItemName(item,SIGDN_FILESYSPATH);c.name=ShellItemName(item,SIGDN_NORMALDISPLAY);
    IShellItem2* raw=nullptr;
    if(SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&raw))) && raw) {
        std::unique_ptr<IShellItem2,ReleaseCom<IShellItem2>> item2(raw);
        c.appId=ItemStringProperty(item2.get(),w7t::PKEY_Aumid());
    }
    const wchar_t* ext=PathFindExtensionW(c.source.c_str());
    if(!c.source.empty() && !_wcsicmp(ext,L".lnk")) {
        c.shortcut=true;c.appId=w7t::ReadAumid(c.source);
        IShellLinkW* rawLink=nullptr;
        if(FAILED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&rawLink))) || !rawLink)return false;
        std::unique_ptr<IShellLinkW,ReleaseCom<IShellLinkW>> link(rawLink);
        IPersistFile* rawFile=nullptr;
        if(FAILED(link->QueryInterface(IID_PPV_ARGS(&rawFile))) || !rawFile)return false;
        std::unique_ptr<IPersistFile,ReleaseCom<IPersistFile>> file(rawFile);
        if(FAILED(file->Load(c.source.c_str(),STGM_READ)))return false;
        wchar_t path[32768]{};WIN32_FIND_DATAW fd{};
        if(SUCCEEDED(link->GetPath(path,(int)std::size(path),&fd,SLGP_RAWPATH)) && path[0])c.exe=w7t::NormalizeTarget(link.get(),path);
        if(!w7t::IsPackagedAumid(c.appId) && c.exe.empty()) {
            PIDLIST_ABSOLUTE target=nullptr;
            if(SUCCEEDED(link->GetIDList(&target)) && target) {
                IShellItem2* shellTarget=nullptr;
                if(SUCCEEDED(SHCreateItemFromIDList(target,IID_PPV_ARGS(&shellTarget))) && shellTarget) {
                    std::unique_ptr<IShellItem2,ReleaseCom<IShellItem2>> holder(shellTarget);
                    std::wstring id=ItemStringProperty(holder.get(),w7t::PKEY_Aumid());
                    if(w7t::IsPackagedAumid(id))c.appId=id;
                }
                CoTaskMemFree(target);
            }
        }
        if(!w7t::IsPackagedAumid(c.appId) && (c.exe.empty() || _wcsicmp(PathFindExtensionW(c.exe.c_str()),L".exe")))return false;
    } else if(!c.source.empty() && !_wcsicmp(ext,L".exe")) {
        DWORD attr=GetFileAttributesW(c.source.c_str());
        if(attr==INVALID_FILE_ATTRIBUTES || (attr&FILE_ATTRIBUTE_DIRECTORY))return false;
        c.exe=NormalizedMatchPath(c.source);
    } else if(!c.source.empty() || !w7t::IsPackagedAumid(c.appId))return false;
    c.identity=!c.appId.empty()?c.appId:c.exe;
    if(c.identity.empty())return false;
    std::transform(c.identity.begin(),c.identity.end(),c.identity.begin(),[](wchar_t x){return (wchar_t)towlower(x);});
    if(c.name.size()>4 && !_wcsicmp(c.name.c_str()+c.name.size()-4,L".lnk"))c.name.resize(c.name.size()-4);
    if(c.name.empty())c.name=L"app";
    out=std::move(c);return true;
}
static bool IsOwnedModPin(const std::wstring& path) {
    std::wstring dir=w7t::ModPinsFolder();if(dir.empty())return false;
    std::wstring prefix=dir+L"\\";
    if(path.size()<=prefix.size() || _wcsnicmp(path.c_str(),prefix.c_str(),prefix.size()))return false;
    std::wstring name=path.substr(prefix.size());
    return name.find_first_of(L"\\/")==std::wstring::npos && name!=L"." && name!=L".." && !_wcsicmp(PathFindExtensionW(name.c_str()),L".lnk");
}
static bool CreateModPin(IShellItem* item,const std::wstring& explicitId={},const std::wstring& displayName={}) {
    ModPinCandidate c;if(!InspectModPin(item,c))return false;
    if(!displayName.empty()) {c.name=displayName;if(c.name.size()>4 && !_wcsicmp(c.name.c_str()+c.name.size()-4,L".lnk"))c.name.resize(c.name.size()-4);}
    if(!explicitId.empty() && !c.shortcut) {c.appId=explicitId;
        c.identity=explicitId;
        std::transform(c.identity.begin(),c.identity.end(),c.identity.begin(),[](wchar_t x){return (wchar_t)towlower(x);});
        }
    std::wstring dir=w7t::ModPinsFolder();if(dir.empty())return false;
    std::vector<w7t::PinnedApp> existing;std::vector<std::wstring> seen;w7t::CollectFolder(dir,existing,seen,true);
    std::wstring key=(c.appId.empty()?L"exe:":L"app:")+c.identity;
    for(const auto& id:seen)if(!_wcsicmp(id.c_str(),c.identity.c_str()))return SetModPinHidden(key,false);
    w7t::CollectFolder(w7t::PinnedFolder(),existing,seen);
    w7t::CollectFolder(w7t::ImplicitPinnedFolder(),existing,seen);
    for(const auto& id:seen)if(!_wcsicmp(id.c_str(),c.identity.c_str()))return SetModPinHidden(key,false);
    if(existing.size()>=MAX_TASKS)return false;
    std::wstring name=c.name;for(auto& ch:name)if(ch<L' ' || wcschr(L"\\/:*?\"<>|",ch))ch=L'_';
    if(name.size()>70)name.resize(70);while(!name.empty() && (name.back()==L'.' || name.back()==L' '))name.pop_back();if(name.empty())name=L"app";
    uint64_t hash=14695981039346656037ull;for(wchar_t ch:c.identity){hash^=(uint16_t)ch;hash*=1099511628211ull;}
    wchar_t suffix[32]{};swprintf(suffix,std::size(suffix),L"-%016llx.lnk",(unsigned long long)hash);
    std::wstring path=dir+L"\\"+name+suffix;
    bool ok=false;
    if(c.shortcut) {
        // Fail rather than overwrite any existing destination.
        ok=CopyFileW(c.source.c_str(),path.c_str(),TRUE)!=FALSE;
    } else {
        wchar_t temp[MAX_PATH]{};
        if(!GetTempFileNameW(dir.c_str(),L"t81",0,temp))return false;
        struct TempGuard {const wchar_t* p;~TempGuard(){if(p)DeleteFileW(p);}} temporary{temp};
        PIDLIST_ABSOLUTE pidl=nullptr;IShellLinkW* raw=nullptr;
        if(SUCCEEDED(SHGetIDListFromObject(item,&pidl)) && pidl && SUCCEEDED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&raw))) && raw) {
            std::unique_ptr<IShellLinkW,ReleaseCom<IShellLinkW>> link(raw);
            HRESULT hr=link->SetIDList(pidl);
            if(SUCCEEDED(hr))hr=link->SetDescription(c.name.c_str());
            if(SUCCEEDED(hr) && !c.appId.empty()) {
                IPropertyStore* p=nullptr;hr=link->QueryInterface(IID_PPV_ARGS(&p));
                std::unique_ptr<IPropertyStore,ReleaseCom<IPropertyStore>> ps(p);
                PROPVARIANT pv{};if(SUCCEEDED(hr))hr=InitPropVariantFromString(c.appId.c_str(),&pv);
                if(SUCCEEDED(hr))hr=ps->SetValue(w7t::PKEY_Aumid(),pv);
                if(SUCCEEDED(hr))hr=ps->Commit();PropVariantClear(&pv);
            }
            IPersistFile* p=nullptr;if(SUCCEEDED(hr))hr=link->QueryInterface(IID_PPV_ARGS(&p));
            std::unique_ptr<IPersistFile,ReleaseCom<IPersistFile>> file(p);
            if(SUCCEEDED(hr) && file)hr=file->Save(temp,TRUE);
            ok=SUCCEEDED(hr) && MoveFileW(temp,path.c_str())!=FALSE;
        }
        if(pidl)CoTaskMemFree(pidl);
    }
    if(ok && !SetModPinHidden(key,false)) {Wh_Log(L"[T81-PIN] unable to clear local hidden marker");return false;}
    Wh_Log(L"[T81-PIN] create success=%d error=%lu",ok?1:0,ok?0:GetLastError());return ok;
}
static bool CreateModPinFromGroup(const std::wstring& key,const std::wstring& exe) {
    std::wstring id=key.rfind(L"app:",0)==0?key.substr(4):L"";
    std::wstring parsing=w7t::IsPackagedAumid(id)?L"shell:AppsFolder\\"+id:exe;
    if(parsing.empty())return false;
    IShellItem* raw=nullptr;HRESULT hr=SHCreateItemFromParsingName(parsing.c_str(),nullptr,IID_PPV_ARGS(&raw));
    std::unique_ptr<IShellItem,ReleaseCom<IShellItem>> item(raw);
    return SUCCEEDED(hr) && item && CreateModPin(item.get(),id);
}
static void RefreshModPinView() {if(s && s->bar){RefreshPins();BuildGroups();InvalidateRect(s->bar,nullptr,FALSE);}}

enum JumpRowKind { JR_DOCUMENT,JR_APP,JR_ACTIVATE,JR_CLOSE,JR_PIN };
struct JumpRow { JumpRowKind kind; int document=-1; RECT rect{}; };
// Legacy 14x14 close artwork (GPL-3.0-or-later).
constexpr char kJumpClosePng[] =
    "iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAABhklEQVR42pXSu04CQRSA4X/ZZVmuWRSNEgslJB"
    "JiIWijYGGstCLBxE5K3kATHoAE3sASbDTRwsonABvxEsB4CdoQQowFBYUQ1rUwLJcKpzuZ882ZOXMEXdc53dpq"
    "5J4b80ywEgFvCVgX8tHo7dxhIrS5ETJNAos39zTzuZKw4/Hpl9d52pW7SRyOlTDx3UMkAF3T0LpdLl8+KdRbAE"
    "QWVICROL48i65pAH/wp9dD63xTqLe4OD8BYP8gCTASxxZd/PR6A/j19MjH1RlBh4/9gyQX5ycG6KNgp8nrVZEl"
    "xQbASEMi7XeCnaZRbRhF2u8jb5UABFHEJMsACCYRgFqtZiQJpqF9URxUFGULZruTotNP1TxDJn1koEz6iKp5hq"
    "LTj9nuRJQtQ1BRkF0qZVQDHaeyHKeyBi6jIrtUREUZXFWy2lHcHsLoRnLYoRsH9GPF7UGy2oehDWVqmtgUxMY+"
    "fDyWrH9dlRIBb6lSa6ytbu9NNDkPlTcSAW9JyEejALe558baf4b8F1/egSzJiuNOAAAAAElFTkSuQmCC";
// Embedded high-resolution close glyph; retain the fallback for draw failures.
constexpr char kJumpCloseReferencePng[] =
    "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"
    "TZib/iCMAAAAAElFTkSuQmCC";
// Pixel-hinted sizes avoid resampling twice at common DPI scales.
static constexpr char kJumpClose16[] =
    "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAACxUlEQVR4nI2TTWhUZxSG33O+7965MxnqeBP8SRqMP0"
    "UURINtCbSUightKW0XXVVcxB+KtogKSgsBUVuidCEUumihXbS4c6k71+KiOytFa03BxJ+QTJw4mZl773fOcZFOiLjQ"
    "d38eeJ/DS2+uGfiQmN/v5JkSmPEKMaiW4ohFcdMb6IPdQxuO9qd9EDMQveTYAMeE2UYD1+79c9UH1VbaU5X1tZXSKg"
    "rnmGGqIGbADAaAiAA1gAmiirL34oI4mC14ArhQdfOtNjIpnOSFRUlCIcvgSzEAguQ5yEciecYujqhgjywEB6NlnYlg"
    "IaB/y2b6+MI5GxzejuxpE6HVxmuDA+1PL46HjSNvkxUBSz0ZWAIQjNqdrD08uq/e98ZG2vXNCevfvs3Ka1e1P/r+NP"
    "euHyrt/HL0SQ6bN1M2sy5j0YyoWRzH/Me3p2/VHz1+GFertGfspH32w3e+J01LrWazfunk2B0KEpmZ6XMAACLBnPel"
    "lXk+8tMX++/O3J+cTmorqKe3N2rOzTV+GT38L6Ye7IiTpFwEMSwHGABTo1yCSQjRusGBHVEcxQATiMFE8WD/mmEVib"
    "NQGKBYqtDVqAAkL6i2aUP2+cXz8YrVq2tPZ2bq9amp6UqtVv5k/IwbGnkLmuek3fcurwAYtzudhXePHn5SSdPSQqMx"
    "++uR4/d+3Hvwdv3R46moUqH3jn89lxvmTY31BYkiGke+fOXMuE7c/Ovu78dO/acPHg6nhHd+PnBkYuLv27cuj52dRp"
    "EnthgAgO86EBXAOQ6zs2svHfpqMikl68rVHmcAklZr5Ld9B+6k1epmnyQoJIiZAiB4AMaAMCBQhTEjrdVeV1UNIQgA"
    "eO95oK9vayFBRAQxs5ABBFLvmJNmp+OyTuYKVRAtDoYARndYhv83AWcAGOQcE4ys4oPan9cn7182MoXZK80ZROqIuC"
    "PhxjNDI3LdECOo+wAAAABJRU5ErkJggg==";
static constexpr char kJumpClose20[] =
    "iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAADuElEQVR4nJ2UW2icRRTH/2dmvt00aZrL5tZms2LFS9"
    "IkJWqwTcGiFW0fRBFiQ/ugLW0xlgpa7wUFaaG0T14oqPgmokIVQai1ocTS5sWHiIIlPikBjduYy+5+++18czk+fNk0"
    "0QeJ52WGw5wf58z/P0MDHR3dxuP+ktYeDIH/EwS/Pp0WaZI/KO1oV19Ly7ut6zeAlASYQWvkMTPmwxA/zf75tjLWxJ"
    "naOvdw9xYHISSYAVobUjK776auy8mZPyoKYHKALEQRtNaSiEBEABhAsjJXd4AQAsweAIGZwWCkZQDLLBkg4ZbKCIAg"
    "Sgq8B3tmb60XSHIg4kApxFrHzLDsHIQQEETL9QBuisBgeGYwe7BzePCV53n7s4eisFCwACNQihbyN8It+578fc/JN1"
    "gGAdg5eEZSB/4H0CfjhIViPHT0cHzL0DbRveeRdTuee6YSFUs8l8+X+57elx/avzfb0dsTPPDqcVOOogpRIko11IoO"
    "GczwQhS/+vSLy11D23bXNTTU9z/2aK2JKqWF+YW5nQef6gJ7ZeI4+vb8l1eYeZCANMNXG1zRIYO0Mdy4oT7D16fuPX"
    "tg9OuoVCoAEPeMDNftGj2cg/fKOlf+6KUT43+NX+2vr69v1sbAe9C/RgZ7AKCwUuFsLntr7czMjtMjBy6WC4XQG0Mu"
    "1jDG6A+PHb9W+H5yYFO2c2NZVxgAJaontBWiILEDA9oYpKXq6B0cuE2lUkIIIiklCSnojt7eu2pSQXtZayQwxjJwtS"
    "gM5z0CJWluZibs2v3Q1PCbr/elamrWeeesNcZIFaR2HT3SuXX/3nxxdjYWRPC+6o4Etso2UggsLhRKPSPDvz1+4uU7"
    "4WwAwHxy6sy19469+I2JtYZ3YueRg5nto4cK5bCcqOx5acYqcEl6ZsA6a2ubGlkAKUjlPjt1dmL6wlhPcfLHre+/8N"
    "oFY6wGoGra2maNNdHNt7TKNgQGEJsYDQ0NjRPvnGMi+nlxfnFu6vPzt2/MZVsFEX69ctV+/NbpS92Dd28eO3kmk2nJ"
    "NMXWckpIqiIVqi+WGcyMKI65ub2taeLcB8xAS3t2U1sYRUwA53Jdm6fHLqvpi5dUW2tLu7Z2hcq82tjLKhEoNoYbmx"
    "qbiQgVrVkkvwWFlQq3NjflAEAby0QgrjZTHZlALJids8ZFsYEUybVqY6tnaHkDIF6RBwAPhghSThGBAa+Ukumirshf"
    "buRl5Nyav2wGkJZSlqyBgKil/vbO+wThCQf2EiQA/k/I6iA4sA9AwgPjfwPHEgVLcUdkIgAAAABJRU5ErkJggg==";
static constexpr char kJumpClose24[] =
    "iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAEr0lEQVR4nK1Vb2iVVRj/Pef9s7urOGfDuak1t7mt8m"
    "5uOjcbQ+iDNaOrCFJQQR/8VF+C/oiWZMOWm5llUgmG/9IIDeEassBViiERfogILIqMZS7n3Z13u/d93/Oec54+3G3d"
    "zSmIPvB+eM/vOc/v+Xeeh5aVlZWMKizL6oAttgj3QDRpjloWFbnuL/ZIGDYvmDn7dJFbDGXMvbAPIQQ8pXBlJP2ind"
    "VkZrkOr66u5ZL7SogJuKswiJAeHtZ9v/9q/XljWNs2AM1MVoELJSVpY+6EgDHFHyKCRRZpZgIAG1AgAMwMGYYYJyAC"
    "AMphY/hkT8EWCTJj5+M4EUEbPcEqxrXBOVfGPzBgQgWltITWICHGNUFEbDHID4KMUQpsDIgm38VkgpwYZjAzmIBMKm"
    "WanlqPjp7t6YyUA6QUQMQGYJuIBodS/Y92vj7Y/sJG9tJpYwyDwTkbeQyTCJhzUJj10L7xedHw7NMoq1tcsnZ3N0al"
    "HCCtybUEXUsO9a/pfiusamqsqH58NVa/+pLQUsLwmA02NxMwAMOGLUHIyiD1R3LoGzBD+R7Pr6uZt+6DHh4NgsGr/1"
    "67sqa7U9a1tlQqL2sA0OV0+uxwZrTfsQQMG86vlz2lJygMFc+aVTTjqx27R0YymcRzW16J6yAw5TWLy57o7rzqjYzK"
    "2pbmKuV7xi6Mit5DR/q+3NyZeaS5aW4QhkxMlN8Pdr51BqDZEBm4q9pWrD3/0f6EEJR4ZtPLcS0DrmioLwMAHfhsRw"
    "rF14eP9Z18Y3umvbU5HioFYxiCcPsaAIDShj3f57aW5fFze/ZZx97Z2WvZDnTgG+V5xiqI0JlDR88f37TVa1uxLB5I"
    "yUprBsbyP22KJgrEIICYyZgwpKrqRQ2xtpU3GExEBLIIbDQqG+vnx+qXVId+AE0EAk+kJr8GN7WpYYYB2ALEwPVkf7"
    "xnu4ytan/YKMXCdUk4DpHWXFkfW7Rhz06kpRwgFRIDPH5/WgLOUYMNsw2iofTIlXW7umRd64pK5Xvacgso8fH+vgNv"
    "diXgOKQD35TX1szb8OG78NhcE5yLIOfgrSIwzI5tUTKZHHxs25bhupUtVcrLajtSaPUeOtrX2/O+/OHgZ87Rrl0Jqy"
    "AilO+ZBXW189a/tyObTN24alsCUwLIJ2AwmEKlODojWnTxixN2Jp1O2YVR6/SBw30nNm/LNC55sGP50vqOvr37cOTt"
    "nQk7UihUGGbPHv78eqTAmam0ZmZDt3wHzAytNdmu6yZ/+rnq+GtbL89pbrpwaluXbm6MxX0pGcxobWqIf7f3k1NOJH"
    "LSJK9X/H3m27o5paUzAxWOzaLbEDCAUClEi2fbqUuXqn77/oJX/1DtkjBUMGwIAAIZoGlp7MkfPz14obR4dm1x6dyo"
    "LyUTETF40rCbpk0NCAKhUrBclx5YOD/myxAMHhvegGYDYwxqKu5fGWoNX0omEE3MoOkiMHltSsgpaq2R0XpiyeW/UA"
    "aQDXwDkABA+Rj4/z8bRCyItPI8TmWzd7rRAEBPPSiJRrVDAiCw7QhhZ5WyLg78A82MabbgHctfmbQ1LH1YRC4tLy9f"
    "6CuzxgvV2Kq5ezEgE3VsUSAKzv0Hm2/Sg47BMrIAAAAASUVORK5CYII=";
static constexpr char kJumpClose32[] =
    "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAGiUlEQVR4nLVXXWxVxxH+Zs+5P3bS6x8wpgYMiUidCI"
    "wJ2MQ/MTSEGjAmTSpAjfraqBL0IUmpgrGUAMJ1GpykagGVPhW1oMiGJDKJavKjPNBGTZTkoQ9VVFWEBmJsjPH1/T17"
    "ztmZPtx7nWv72liRPKvVkXZH8307OzM7hwBQbXV16XiUKQIAESysxIAYYohEIlhz48YE1VZXl1nafByw1SJmEWGhhc"
    "QnRUJE5LPxjMJWezwapaX3RJbWL/l+aciyoCwbWEAKxhgYY/DZrWGMJmO2HUEEArhBy5L1K1bKovLFBLVwBCbGx+Vf"
    "178miHhEIbGz6wQiEstCdPwOycLhZ8CICFk/29M3RQQFCRBQeKMgAiCzK+cjzHC2zJhZdQGIaMpaAT0hIrAxANHkYa"
    "bPfJlJQGTKzKwxPM8T7WqXmCGgGXoiEAWQ9txUoCjMvtYgpSDCBW0WJCAAWCTjvklwgRNLoHX/z/H4i4fiiWQyqTJ7"
    "Ilk9EZGQbdPordGRhgO/+KbjRDeFSyKiUykQKUi+zbkI5DtTcuCJJB498Ax+sHM73ddQv6jtyOF0IpFMEoRAEEYGfH"
    "h4ZGTzoeeiddu3PVCyfBnt7H6JisvLxNM6cx15Y3YC+SQJ8NNpbNn/DNb+5Mcw6bSwdrC6pWlx25HOdCKeTJKAQoEA"
    "DQ/fGml94dlofUd7Dbsa7KRRtmolnnr5GNmBAJjNVAfIbASyp2ZmCSgLd5Kp/1755PM33VSKrVCQQDRJYvuxrlQqlX"
    "KGh27e2tL53Hh9R3sNaw0FiLJtATPe7XvrvTsTE0NBywYLS6EYmJmGWSe5vidlZSUVl0/9KZpKxC8c6O3eEwgGFfu+"
    "sHZodXNjhXfo+ZH4xERs466dD7KroQjCpKBsm/7a03tpsPcP8cdaGktd3xcISLIVRmYjkLt3gMg3LEHLKml/fPPPLp"
    "+/cJ6AC/tPdO8JhEIKvifiufTQ1h9WAqgUzwURAUST4O+9fjq5bXPLT4WN8gxn9idPP0cdyMSBgADSvi/EEmprbXr6"
    "H+f7kqd/3dXve66BZWfMuRriagAAEQksm871vHZp8PXTyceaH9lnfF+5xgjlDlagEEwLQkFeaoEI5BpfiDn8o9bmp/"
    "/5xkV96tkX3jLGGIBASmVzXUSURed7TgwO9v4+tbWlca/xfeWxEQIo3+acdeDbGMiOrCdc45tiyw4/tGrlIxvati2H"
    "Uoox02Dto00r1q99cIsyxvJFOAc+fcxNYFrNYIEUBYLW1a+vf7XjaKfX+tTuRiWcuXGlQJaVeVyMj9rNrWs6eo6a2+"
    "PR2zZBCSgb+bPWoZmVMJ8pi0jYtun6N0PXdhztim/atXOt0Y6QiJCywMbAdzSTbYNExGhHapobl+3+7TEnGovftiA0"
    "SSLfA3erA5laIBIO2HTj5s3/bT/SGWvYtWOd0Y5YRGBSgFI4e/zEwGu/PNjnac2wbSIisHZQ09y4fPfLR53xidhtSz"
    "LJN6+3AN8Gi4QCNt0YGv6q7aXOO5s62tcZJ805cBWw6VzPqwOfnj1n/+eDD0OnfnW4z9MuK9smEIlx0lLT3LR8z+9e"
    "0QnHGbMo+3Dk7N89BgSGGcFwMPTvjz8Z9bR2rGBQMZGogE3nfvPqwIcnz6C+rra9ft3aJ7/ofxunD3b150jkThkfG/"
    "MhYnMGl+YdAwBIe55UlJdXXXvn8gNnnu8cNMyuCgTVX7p7Bz46eQZNG9c/kXYcMZ5PLQ0b9n3W/yZOHTzc5zpa7KJi"
    "+nTg3S8vdR0pvqcoXOKzEcySBZOVMNeH5tgTQMl0mqtXrbjv2pW/y9kXj39QvKjcfHTyj2jcuP4JR2sRSKYx8D3V0r"
    "Bh75U3Lvb9ubS0v27Thtr3j79SXlGxpEIbIwSZ1ubJTAL5VzApRCqRSvGqldX3Xx18Pz02Ebu26eG6Xdp1hUVAIIBA"
    "PrOI56um+of3fdF3cWDob5fDVUsrKx3PY4DUXJ1cwcdoClEiFU+npWpJxZpllUvWuJ6LHHZeSpFhAyJSdavvf1IApF"
    "1PQFB3ayTzCFDBNMltas/PBAdlb6uAmojAcT0hyhmcE3saAZrymcmA5ve7cjc9npKGLuwYYihCBMwsqXhCRpPJTF+4"
    "QLK4uFhUHsesB0iJCI2lEuSzmXf7/10kqh3ymQEiBQB2JBKB8dn9fGTYY2GheTv7u4mICIFIG9/7XjAotBewvqyqWp"
    "122MqouAuHDgAIAnARCgZF33vv1f8D0ZxJf0bjW3YAAAAASUVORK5CYII=";
struct JumpPopupData {
    std::vector<JumpDocument> docs; std::vector<JumpRow> rows;
    std::vector<HICON> icons;
         HICON appIcon=nullptr;
         Image* closeImage=nullptr;
         IStream* closeStream=nullptr;
         Image* referenceClose=nullptr;
         IStream* referenceStream=nullptr;
    Image* referenceVariants[4]={}; IStream* referenceVariantStreams[4]={};
    std::wstring title,launch,activationAppId,pinKey,pinExe,modPinPath; bool pinAction=false,pinnedForMod=false;
    HICON pinIcon=nullptr;
    std::vector<std::pair<HWND,DWORD>> targets;
    HWND owner=nullptr; bool frequent=true,windowOwns=false; int hot=-1,mouseMask=0,headerHeight=0;
    bool betaCloseHot=false,betaCloseDown=false;
    int animX=0,animFinalY=0,animStartOffset=0,animStartOffsetX=0; ULONGLONG animStart=0;
    bool closing=false; int closeX=0,closeY=0,closeOffset=0,closeOffsetX=0; ULONGLONG closeStart=0;
    ~JumpPopupData() { if(pinIcon)DestroyIcon(pinIcon);
        for(HICON i:icons) if(i) DestroyIcon(i);
         if(appIcon) DestroyIcon(appIcon);
         for(int i=0;i<4;i++){delete referenceVariants[i];
        if(referenceVariantStreams[i])referenceVariantStreams[i]->Release();
        } delete referenceClose;
         if(referenceStream) referenceStream->Release();
         delete closeImage;
         if(closeStream) closeStream->Release();
         }
};
static HICON JumpDocumentIcon(IUnknown* object) {
    if(!object) return nullptr; IShellItem* item=nullptr; HICON result=nullptr;
    if(SUCCEEDED(object->QueryInterface(IID_PPV_ARGS(&item))) && item) {
        PIDLIST_ABSOLUTE pidl=nullptr;
        if(SUCCEEDED(SHGetIDListFromObject(item,&pidl)) && pidl) {
            SHFILEINFOW info{};
            if(SHGetFileInfoW((LPCWSTR)pidl,0,&info,sizeof(info),SHGFI_PIDL|SHGFI_ICON|SHGFI_SMALLICON)) result=info.hIcon;
            CoTaskMemFree(pidl);
        }
        item->Release();
    } else {
        IShellLinkW* link=nullptr;
        if(SUCCEEDED(object->QueryInterface(IID_PPV_ARGS(&link))) && link) {
            wchar_t path[32768]={}; WIN32_FIND_DATAW fd{}; link->GetPath(path,(int)std::size(path),&fd,SLGP_RAWPATH);
            if(path[0]) { SHFILEINFOW info{}; if(SHGetFileInfoW(path,0,&info,sizeof(info),SHGFI_ICON|SHGFI_SMALLICON)) result=info.hIcon; }
            link->Release();
        }
    }
    return result;
}
static int JumpHit(const JumpPopupData* data,POINT p) {
    if(!data) return -1;
    for(size_t i=0;i<data->rows.size();++i) if(PtInRect(&data->rows[i].rect,p)) return (int)i;
    return -1;
}
static void MarkJumpDismissFromPointer() {
    if(!s || !s->bar || s->jumpGroupKey.empty()) return;
    bool right=(GetAsyncKeyState(VK_RBUTTON)&0x8000)!=0,left=(GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0; if(!right && !left) return;
    POINT p{}; if(!GetCursorPos(&p) || !ScreenToClient(s->bar,&p)) return; int hit=Hit(p.x,p.y);
    if(hit>=0 && (size_t)hit<s->groups.size() && s->groups[hit].key==s->jumpGroupKey) {
        if(right) {s->jumpDismissKey=s->jumpGroupKey;s->jumpDismissAt=GetTickCount64();}
        else {s->jumpLeftDismissKey=s->jumpGroupKey;s->jumpLeftDismissAt=GetTickCount64();}
    }
}
// Closing is a short, non-blocking movement of the existing painted window;
// no AnimateWindow, layered-window fade, or screenshot is involved.
// Forced shutdown/replacement always destroys immediately.
static void CloseJumpPopup(bool animate=false) {
    if(!s) return;
    if(s->jumpClosing && IsWindow(s->jumpClosing)) DestroyWindow(s->jumpClosing);
    s->jumpClosing=nullptr;
    HWND h=s->jumpPopup; if(!h || !IsWindow(h)) {s->jumpPopup=nullptr;s->jumpGroupKey.clear();return;}
    auto* d=(JumpPopupData*)GetWindowLongPtrW(h,GWLP_USERDATA);
    BOOL enabled=FALSE; bool readAnimation=SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&enabled,0)!=FALSE;
#ifdef TASKBAR81_TEST
    if(testJumpAnimation>=0) {enabled=testJumpAnimation!=0;readAnimation=true;}
#endif
    if(animate && d && IsWindowVisible(h) && readAnimation && enabled) {
        RECT r{};if(GetWindowRect(h,&r)) {
            KillTimer(h,1);KillTimer(h,2);d->closing=true;d->hot=-1;
            d->closeX=r.left;d->closeY=r.top; RECT barRect{}; bool above=!s->bar || !GetWindowRect(s->bar,&barRect) || r.top<barRect.top;
            d->closeOffsetX=s->verticalActive?(s->verticalSide==1?-Px(4):Px(4)):0;
            d->closeOffset=s->verticalActive?0:(above?Px(4):-Px(4));d->closeStart=GetTickCount64();
            s->jumpPopup=nullptr;s->jumpGroupKey.clear();s->jumpClosing=h;
            if(SetTimer(h,3,16,nullptr)) return;
            s->jumpClosing=nullptr;
        }
    }
    DestroyWindow(h);s->jumpPopup=nullptr;s->jumpGroupKey.clear();
}
static void ActivateJumpRow(HWND h,JumpPopupData* data,int row) {
    if(!data || row<0 || (size_t)row>=data->rows.size()) return;
    JumpRow selected=data->rows[row];
    std::shared_ptr<IUnknown> object;
    if(selected.kind==JR_DOCUMENT && selected.document>=0 && (size_t)selected.document<data->docs.size()) object=data->docs[selected.document].object;
    std::wstring launch=data->launch, appId=data->activationAppId;
    std::wstring pinKey=data->pinKey,pinExe=data->pinExe,modPinPath=data->modPinPath;
    bool pinnedForMod=data->pinnedForMod;
    auto targets=data->targets; HWND owner=data->owner;
    DestroyWindow(h); // data is destroyed by WM_NCDESTROY; copies above remain valid
    if(selected.kind==JR_DOCUMENT && object) {
        if(!OpenJumpDocument(owner,object.get())) Wh_Log(L"[T81-DIAG] Jump List item activation failed");
    } else if(selected.kind==JR_APP) {
        if(!launch.empty()) ShellExecuteW(owner,L"open",launch.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
        else if(!appId.empty()) {
            IApplicationActivationManager* raw=nullptr;HRESULT hr=CoCreateInstance(CLSID_ApplicationActivationManager,nullptr,CLSCTX_LOCAL_SERVER,IID_PPV_ARGS(&raw));
            std::unique_ptr<IApplicationActivationManager,ReleaseCom<IApplicationActivationManager>> manager(raw);DWORD pid=0;
            if(SUCCEEDED(hr) && manager)hr=manager->ActivateApplication(appId.c_str(),nullptr,AO_NONE,&pid);
            if(FAILED(hr))Wh_Log(L"[T81-DIAG] packaged app activation failed hr=0x%08lx",(unsigned long)hr);
        }
    }
    else if(selected.kind==JR_PIN) {
        bool ok=false;
        if(pinnedForMod) {
            // Hide the identity in this mod, without changing Windows pins.
            ok=SetModPinHidden(pinKey,true);
            if(ok && !modPinPath.empty() && (!IsOwnedModPin(modPinPath) || !DeleteFileW(modPinPath.c_str()))) {
                SetModPinHidden(pinKey,false);ok=false;
            }
        } else ok=CreateModPinFromGroup(pinKey,pinExe);
        Wh_Log(L"[T81-PIN] menu action success=%d unpin=%d",ok?1:0,pinnedForMod?1:0);
        if(ok)RefreshModPinView();else {RECT rect{};if(owner)GetWindowRect(owner,&rect);ShowInfoTooltip(Tr(L"Impossibile aggiornare il collegamento della mod. Controlla il log Windhawk.",L"Unable to update the mod pin. Check the Windhawk log."),(rect.left+rect.right)/2);}
    }
    else if(selected.kind==JR_ACTIVATE && !targets.empty()) {DWORD pid=0;
        GetWindowThreadProcessId(targets.front().first,&pid);
        if(pid && pid==targets.front().second && IsWindow(targets.front().first))ActivateTask(targets.front().first);
        }
    else if(selected.kind==JR_CLOSE) for(auto target:targets) { DWORD pid=0;
         GetWindowThreadProcessId(target.first,&pid);
         if(pid && pid==target.second && IsWindow(target.first)) RequestTaskClose(target.first);
         }
}
// A decoded image can still fail to render; try the original artwork in
// both the unavailable and the draw-failure cases before the GDI fallback.
static bool DrawJumpCloseImages(Graphics& g,Image* reference,Image* legacy,int x,int y,int size) {
    if(reference && g.DrawImage(reference,Rect(x,y,size,size),0,0,(int)reference->GetWidth(),(int)reference->GetHeight(),UnitPixel)==Ok)return true;
    return legacy && g.DrawImage(legacy,Rect(x,y,size,size),0,0,(int)legacy->GetWidth(),(int)legacy->GetHeight(),UnitPixel)==Ok;
}
static Image* JumpCloseBestReference(JumpPopupData* d,int size) {
    if(!d)return nullptr;
    const int sizes[]={16,20,24,32};
    for(int i=0;i<4;i++)if(size==sizes[i] && d->referenceVariants[i])return d->referenceVariants[i];
    return d->referenceClose;
}
static Image* JumpClosePrimary(JumpPopupData* d,int style,int size) {
    if(!d)return nullptr;
    return style==0?d->referenceClose:(style==1?d->closeImage:JumpCloseBestReference(d,size));
}
static RECT BetaJumpCloseRect(const JumpRow& row) {
    int size=Px(14),top=row.rect.top+((row.rect.bottom-row.rect.top)-size)/2;
    int left=Px(taskbarSkin.load()==1?8:16);
    return RECT{left,top,left+size,top+size};
}
static bool BetaJumpCloseHit(JumpPopupData* d,POINT p) {
    if(!d || !BetaTextureAvailable(8))return false;
    for(const auto& row:d->rows)if(row.kind==JR_CLOSE) {
        RECT rc=BetaJumpCloseRect(row);return PtInRect(&rc,p)!=FALSE;
    }
    return false;
}
static void PaintJumpPopup(HWND h,JumpPopupData* d) {
    ScopedPaintDC paint(h); HDC screen=paint.dc; if(!screen) return;
    RECT client{}; GetClientRect(h,&client);
    std::unique_ptr<std::remove_pointer<HDC>::type,DeleteMemDC> memGuard(CreateCompatibleDC(screen)); HDC mem=memGuard.get();
    std::unique_ptr<std::remove_pointer<HBITMAP>::type,DeleteBitmapHandle> bitmapGuard(mem?CreateCompatibleBitmap(screen,client.right,client.bottom):nullptr); HBITMAP bm=bitmapGuard.get();
    ScopedSelection bitmapSelection(mem,bm); HDC dc=(mem && bm && bitmapSelection.old && bitmapSelection.old!=HGDI_ERROR)?mem:screen;
    const bool win7=taskbarSkin.load()==1;
    const int contentOffset=win7?Px(8):0;
    RECT paper=client;
    if(!win7) {
        COLORREF chrome=PopupChromeColor();
        HBRUSH bg=CreateSolidBrush(chrome); FillRect(dc,&client,bg); DeleteObject(bg);
        InflateRect(&paper,-Px(6),-Px(6));
    }
    HBRUSH white=CreateSolidBrush(RGB(255,255,255)); FillRect(dc,&paper,white); DeleteObject(white);
    if(taskbarSkin.load()!=1) {HBRUSH border=CreateSolidBrush(RGB(161,161,161)); FrameRect(dc,&paper,border); DeleteObject(border);}
    for(const auto& row:d->rows) if(row.kind==JR_APP) {
        RECT footer{paper.left+1,row.rect.top-1,paper.right-1,paper.bottom-1};
        HBRUSH foot=CreateSolidBrush(RGB(245,245,245)); FillRect(dc,&footer,foot); DeleteObject(foot); break;
    }
    HFONT font=CreateFontW(-MulDiv(9,Dpi(),72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    std::unique_ptr<std::remove_pointer<HFONT>::type,DeleteFontHandle> fontGuard(font);
         ScopedSelection fontSelection(dc,font);
         SetBkMode(dc,TRANSPARENT);
    if(!d->docs.empty()) {
        RECT head{Px(15)-contentOffset,win7?Px(5):Px(10),client.right-Px(14)+contentOffset,d->headerHeight}; SetTextColor(dc,RGB(24,83,147));
        const wchar_t* heading=d->frequent?Tr(L"Frequenti",L"Frequent"):Tr(L"Recenti",L"Recent");
        DrawTextW(dc,heading,-1,&head,DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
        HPEN line=CreatePen(PS_SOLID,1,RGB(165,192,215)); HGDIOBJ pen=SelectObject(dc,line);
        if(win7) {
            SIZE label{};GetTextExtentPoint32W(dc,heading,(int)wcslen(heading),&label);
            int lineLeft=head.left+label.cx+Px(4), lineRight=client.right-Px(7);
            int lineY=(head.top+head.bottom)/2;
            if(lineLeft<lineRight) {MoveToEx(dc,lineLeft,lineY,nullptr);LineTo(dc,lineRight,lineY);}
        } else {MoveToEx(dc,Px(15),d->headerHeight,nullptr);LineTo(dc,client.right-Px(15),d->headerHeight);}
        if(pen) SelectObject(dc,pen); DeleteObject(line);
    }
    for(size_t i=0;i<d->rows.size();++i) {
        const auto& row=d->rows[i]; RECT rr=row.rect;
        if(row.kind==JR_APP) { HPEN sep=CreatePen(PS_SOLID,1,RGB(210,210,210));
             HGDIOBJ pen=SelectObject(dc,sep);
             MoveToEx(dc,Px(7),rr.top,nullptr);
             LineTo(dc,client.right-Px(7),rr.top);
             if(pen) SelectObject(dc,pen);
             DeleteObject(sep);
             }
        if((int)i==d->hot) {
            RECT hover=rr; InflateRect(&hover,-Px(3),-Px(1));
            HBRUSH hb=CreateSolidBrush(RGB(222,240,252)); FillRect(dc,&hover,hb); DeleteObject(hb);
            HBRUSH edge=CreateSolidBrush(RGB(137,188,224)); FrameRect(dc,&hover,edge); DeleteObject(edge);
        }
        HICON icon=nullptr; std::wstring text;
        if(row.kind==JR_DOCUMENT) { if((size_t)row.document<d->docs.size()) text=d->docs[row.document].label; if((size_t)row.document<d->icons.size()) icon=d->icons[row.document]; }
        else if(row.kind==JR_APP) { text=(d->docs.empty()?Tr(L"Apri ",L"Open "):L"")+d->title; icon=d->appIcon; }
        else if(row.kind==JR_ACTIVATE) {text=Tr(L"Mostra finestra",L"Show window");}
        else if(row.kind==JR_PIN) {text=!d->pinnedForMod?Tr(L"Aggiungi alla barra delle applicazioni",L"Pin to taskbar"):Tr(L"Rimuovi dalla barra delle applicazioni",L"Unpin from taskbar");icon=d->pinIcon;}
        else text=d->targets.size()>1?Tr(L"Chiudi tutte le finestre",L"Close all windows"):Tr(L"Chiudi finestra",L"Close window");
        int size=Px(16);
        if(icon) { DrawIconEx(dc,Px(16)-contentOffset,rr.top+((rr.bottom-rr.top)-size)/2,icon,size,size,0,nullptr,DI_NORMAL); }
        else if(row.kind==JR_ACTIVATE) {
            RECT iconRect{Px(16)-contentOffset,rr.top+((rr.bottom-rr.top)-size)/2,
                          Px(16)-contentOffset+size,rr.top+((rr.bottom-rr.top)-size)/2+size};
            bool drawn=false;
            HFONT font=CreateFontW(-size,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
                                   OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,L"Segoe MDL2 Assets");
            if(font) {
                HGDIOBJ old=SelectObject(dc,font);wchar_t face[LF_FACESIZE]{};WORD glyph=0xffff;
                if(old && old!=HGDI_ERROR && GetTextFaceW(dc,LF_FACESIZE,face) && !_wcsicmp(face,L"Segoe MDL2 Assets") &&
                   GetGlyphIndicesW(dc,L"\uE923",1,&glyph,GGI_MARK_NONEXISTING_GLYPHS)!=GDI_ERROR && glyph!=0xffff) {
                    int previousMode=SetBkMode(dc,TRANSPARENT);COLORREF previousColor=SetTextColor(dc,RGB(55,70,80));
                    DrawTextW(dc,L"\uE923",1,&iconRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
                    SetTextColor(dc,previousColor);SetBkMode(dc,previousMode);drawn=true;
                }
                if(old && old!=HGDI_ERROR)SelectObject(dc,old);DeleteObject(font);
            }
            if(!drawn)DrawFrameControl(dc,&iconRect,DFC_CAPTION,DFCS_CAPTIONRESTORE|DFCS_FLAT);
        }
        else if(row.kind==JR_PIN) {
            // Fallback when the running Windows build does not contain the
            // legacy icon groups. Never load or patch third-party system DLLs.
            Graphics pinGraphics(dc);pinGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
            float x=(float)(Px(16)-contentOffset),y=(float)rr.top+((float)(rr.bottom-rr.top)-size)/2.0f,u=size/16.0f;
            Pen stem(Color(255,70,89,104),std::max(1.0f,u));
            SolidBrush body(Color(255,72,130,174));
            pinGraphics.DrawLine(&stem,x+8*u,y+8*u,x+4*u,y+15*u);
            PointF shape[]={{x+5*u,y+2*u},{x+12*u,y+2*u},{x+11*u,y+6*u},{x+14*u,y+9*u},{x+4*u,y+9*u},{x+7*u,y+6*u}};
            pinGraphics.FillPolygon(&body,shape,6);
        }
        else if(row.kind==JR_CLOSE) {
            Graphics gfx(dc);gfx.SetInterpolationMode(InterpolationModeHighQualityBicubic);gfx.SetPixelOffsetMode(PixelOffsetModeHighQuality);
            int style=jumpCloseStyle.load();int c=Px(style==2?16:14),y=rr.top+((rr.bottom-rr.top)-c)/2;
            if(BetaTextureAvailable(8)) {
                RECT iconRect=BetaJumpCloseRect(row);
                int state=(d->betaCloseDown && d->betaCloseHot)?10:(d->betaCloseHot?9:8);
                if(!BetaTextureAvailable(state))state=8;
                DrawBetaTexture(gfx,state,iconRect);
            } else {
            Image* primary=JumpClosePrimary(d,style,c);
            Image* fallback=style==1?d->referenceClose:d->closeImage;
            if(style==2 && primary && primary->GetWidth()==(UINT)c && primary->GetHeight()==(UINT)c)gfx.SetInterpolationMode(InterpolationModeNearestNeighbor);
            if(!DrawJumpCloseImages(gfx,primary,fallback,Px(16)-contentOffset,y,c)) { HPEN p=CreatePen(PS_SOLID,std::max(1,Px(2)),RGB(160,35,35));
                 HGDIOBJ old=SelectObject(dc,p);
                 int x=Px(18)-contentOffset,y=rr.top+(rr.bottom-rr.top)/2-Px(5);
                 MoveToEx(dc,x,y,nullptr);
                LineTo(dc,x+Px(10),y+Px(10));
                MoveToEx(dc,x+Px(10),y,nullptr);
                LineTo(dc,x,y+Px(10));
                if(old)SelectObject(dc,old);
                DeleteObject(p);
                 }
            }
        }
        RECT tr{Px(37)-contentOffset,rr.top,client.right-Px(15)+contentOffset,rr.bottom};
        SetTextColor(dc,RGB(25,25,25));
        DrawTextW(dc,text.c_str(),-1,&tr,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
    }
    if(dc==mem) BitBlt(screen,0,0,client.right,client.bottom,mem,0,0,SRCCOPY);
}
static LRESULT CALLBACK JumpPopupProc(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    JumpPopupData* d=(JumpPopupData*)GetWindowLongPtrW(h,GWLP_USERDATA);
    if(msg==WM_NCCREATE) { d=(JumpPopupData*)((CREATESTRUCTW*)lp)->lpCreateParams; SetWindowLongPtrW(h,GWLP_USERDATA,(LONG_PTR)d); }
    if(!d) return DefWindowProcW(h,msg,wp,lp);
    try {
        if(d->closing && (msg==WM_MOUSEMOVE || msg==WM_LBUTTONDOWN || msg==WM_LBUTTONUP || msg==WM_RBUTTONUP || msg==WM_KEYDOWN)) return 0;
        switch(msg) {
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: PaintJumpPopup(h,d); return 0;
        case WM_NCHITTEST: {LRESULT hit=DefWindowProcW(h,msg,wp,lp);
            switch(hit) {case HTTOP:case HTTOPRIGHT:case HTRIGHT:case HTBOTTOMRIGHT:case HTBOTTOM:case HTBOTTOMLEFT:case HTLEFT:case HTTOPLEFT:return HTBORDER;
            default:return hit;
            }}
        case WM_MOUSEMOVE: { POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
             int hot=JumpHit(d,p);
             bool close=BetaJumpCloseHit(d,p);
             if(hot!=d->hot || close!=d->betaCloseHot) {d->hot=hot;
            d->betaCloseHot=close;
            InvalidateRect(h,nullptr,FALSE);
            } TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,h,0};
             TrackMouseEvent(&t);
             return 0;
             }
        case WM_MOUSELEAVE: d->hot=-1;d->betaCloseHot=false;InvalidateRect(h,nullptr,FALSE); return 0;
        case WM_LBUTTONDOWN: {POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            if(BetaJumpCloseHit(d,p)){d->betaCloseDown=true;
            d->betaCloseHot=true;
            SetCapture(h);
            InvalidateRect(h,nullptr,FALSE);
            return 0;
            }break;
            }
        case WM_CAPTURECHANGED: case WM_CANCELMODE: {if(d->betaCloseDown){d->betaCloseDown=false;
            d->betaCloseHot=false;
            InvalidateRect(h,nullptr,FALSE);
            }break;
            }
        case WM_LBUTTONUP: { POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
             if(d->betaCloseDown){bool commit=BetaJumpCloseHit(d,p);
            d->betaCloseDown=false;
            if(GetCapture()==h)ReleaseCapture();
            if(commit)ActivateJumpRow(h,d,JumpHit(d,p));
            else InvalidateRect(h,nullptr,FALSE);
            return 0;
            }ActivateJumpRow(h,d,JumpHit(d,p));
             return 0;
             }
        case WM_KEYDOWN:
            if(wp==VK_ESCAPE) {CloseJumpPopup(true);return 0;}
            if(wp==VK_UP || wp==VK_DOWN) { int n=(int)d->rows.size();
                 if(n) {int delta=wp==VK_UP?-1:1;
                 d->hot=(d->hot<0?(delta>0?0:n-1):(d->hot+delta+n)%n);
                 InvalidateRect(h,nullptr,FALSE);
                } return 0;
                 }
            if(wp==VK_RETURN) {ActivateJumpRow(h,d,d->hot);return 0;} break;
        case WM_ACTIVATE: if(LOWORD(wp)==WA_INACTIVE && IsWindowVisible(h)) {if(d->closing) return 0;
            MarkJumpDismissFromPointer();
            CloseJumpPopup(true);
            return 0;
            } break;
        case WM_TIMER: { if(wp==3 && d->closing) {
                ULONGLONG elapsed=GetTickCount64()-d->closeStart; double t=std::min(1.0,(double)elapsed/100.0);
                int y=d->closeY+(int)std::lround(d->closeOffset*t*t);
                if(t>=1.0) {KillTimer(h,3);DestroyWindow(h);return 0;}
                int x=d->closeX+(int)std::lround(d->closeOffsetX*t*t);
                if(!SetWindowPos(h,nullptr,x,y,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE)) {KillTimer(h,3);DestroyWindow(h);} return 0;
            }
            if(wp==2) {
                ULONGLONG elapsed=GetTickCount64()-d->animStart; double t=std::min(1.0,(double)elapsed/70.0);
                double remain=(1.0-t)*(1.0-t); int y=d->animFinalY+(int)std::lround(d->animStartOffset*remain);
                int x=d->animX+(int)std::lround(d->animStartOffsetX*remain);
                SetWindowPos(h,nullptr,x,y,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
                if(t>=1.0) KillTimer(h,2); return 0;
            }
            if(wp!=1) return 0;
            int now=((GetAsyncKeyState(VK_LBUTTON)&0x8000)?1:0)|((GetAsyncKeyState(VK_RBUTTON)&0x8000)?2:0); int fresh=now & ~d->mouseMask; d->mouseMask=now;
            if(fresh) { POINT p{};
                RECT r{};
                GetCursorPos(&p);
                GetWindowRect(h,&r);
                if(!PtInRect(&r,p)) {if(d->closing) return 0;
                MarkJumpDismissFromPointer();
                CloseJumpPopup(true);
                return 0;
                } } return 0;
                 }
        case WM_CLOSE: DestroyWindow(h); return 0;
        case WM_NCDESTROY: KillTimer(h,1);
             KillTimer(h,2);
             KillTimer(h,3);
             if(s && s->jumpClosing==h) s->jumpClosing=nullptr;
             if(s && s->jumpPopup==h) {s->jumpPopup=nullptr;
            s->jumpGroupKey.clear();
            } SetWindowLongPtrW(h,GWLP_USERDATA,0);
             if(d->windowOwns) delete d;
             return DefWindowProcW(h,msg,wp,lp);
        }
    } catch(...) { if(msg!=WM_NCDESTROY) DestroyWindow(h); return 0; }
    return DefWindowProcW(h,msg,wp,lp);
}
static HICON LoadLegacyPinIcon(bool pinned) {
    wchar_t win[MAX_PATH]{};UINT n=GetWindowsDirectoryW(win,(UINT)std::size(win));
    if(!n || n>=std::size(win))return nullptr;
    const std::wstring base(win);
    const std::wstring paths[]={base+L"\\System32\\imageres.dll",base+L"\\SystemResources\\imageres.dll.mun"};
    (void)pinned;HICON result=nullptr;int resource=0;
    for(int id:{5100,5101}) {
        for(const auto& path:paths) {
            HMODULE module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE);
            if(!module)continue;
            result=(HICON)LoadImageW(module,MAKEINTRESOURCEW(id),IMAGE_ICON,Px(16),Px(16),0);
            FreeLibrary(module);if(result){resource=id;break;}
        }
        if(result)break;
    }
    Wh_Log(L"[T81-PIN] system icon resource=%d loaded=%d",resource,result?1:0);
    return result;
}
static bool ShowJumpList(size_t index,POINT) {
    if(!s || !s->bar || index>=s->groups.size()) return false;
    Group group=s->groups[index]; std::wstring id;
    if(s->jumpPopup && IsWindow(s->jumpPopup) && s->jumpGroupKey==group.key) {CloseJumpPopup(true);return true;}
    if(GroupBase(group).rfind(L"app:",0)==0) id=GroupBase(group).substr(4);
    if(id.empty() && !group.tasks.empty() && group.tasks.front()<s->tasks.size()) {
        const auto& key=s->tasks[group.tasks.front()].key; if(key.rfind(L"app:",0)==0) id=key.substr(4);
    }
    KillTimer(s->bar,TIMER_HOVER); HidePreview(); CloseJumpPopup(); auto d=std::make_unique<JumpPopupData>();
    d->owner=s->bar; d->title=GroupTitle(index); if(d->title.empty()) d->title=Tr(L"Applicazione",L"Application");
    if(!_wcsicmp(d->title.c_str(),L"File Explorer") || !_wcsicmp(d->title.c_str(),L"Windows Explorer")) {
        // Same five translations as win81-start-screen-recreation (GPL-3.0-or-later).
        LANGID lang=PRIMARYLANGID(GetUserDefaultUILanguage());
        if(lang==LANG_ITALIAN) d->title=L"Esplora file";
        else if(lang==LANG_SPANISH) d->title=L"Explorador de archivos";
        else if(lang==LANG_FRENCH) d->title=L"Explorateur de fichiers";
        else if(lang==LANG_PORTUGUESE) d->title=L"Explorador de Arquivos";
    }
    d->pinKey=GroupBase(group);
    if(group.pin>=0 && (size_t)group.pin<s->pins.size()) {
        const Pin& pin=s->pins[group.pin];d->launch=pin.path;d->pinKey=pin.key;
        d->pinnedForMod=true;d->pinAction=true;
        if(IsOwnedModPin(d->launch))d->modPinPath=d->launch;
    }
    for(auto i:group.tasks) if(i<s->tasks.size()) { const Task& t=s->tasks[i];
         d->targets.push_back({t.hwnd,t.pid});
         if(d->launch.empty()) d->launch=t.processPath;
        if(d->pinExe.empty())d->pinExe=t.processPath;
         }
    if(group.pin<0 && !group.tasks.empty()) {
        std::wstring app=d->pinKey.rfind(L"app:",0)==0?d->pinKey.substr(4):L"";
        // The synthetic Control Panel group does not identify a launchable app.
        d->pinAction=app!=L"taskbar81.controlpanel" && (w7t::IsPackagedAumid(app) ||
            (!d->pinExe.empty() && !_wcsicmp(PathFindExtensionW(d->pinExe.c_str()),L".exe")));
    }
    if(!id.empty() && w7t::IsPackagedAumid(id) && group.pin<0) {d->activationAppId=id;d->launch.clear();}
    HICON source=GroupIcon(index); if(source) d->appIcon=CopyIcon(source);
    if(d->pinAction)d->pinIcon=LoadLegacyPinIcon(d->pinnedForMod);
    auto referenceBytes=DecodeBase64(kJumpCloseReferencePng);
    if(!referenceBytes.empty()) {d->referenceStream=SHCreateMemStream(referenceBytes.data(),(UINT)referenceBytes.size());
        if(d->referenceStream){d->referenceClose=Image::FromStream(d->referenceStream,FALSE);
        if(d->referenceClose && d->referenceClose->GetLastStatus()!=Ok){delete d->referenceClose;
        d->referenceClose=nullptr;
        }}}
    const char* variants[]={kJumpClose16,kJumpClose20,kJumpClose24,kJumpClose32};
    for(int i=0;i<4;i++) {
        auto bytes=DecodeBase64(variants[i]);
        if(bytes.empty())continue;
        d->referenceVariantStreams[i]=SHCreateMemStream(bytes.data(),(UINT)bytes.size());
        if(d->referenceVariantStreams[i]) {
            d->referenceVariants[i]=Image::FromStream(d->referenceVariantStreams[i],FALSE);
            if(d->referenceVariants[i] && d->referenceVariants[i]->GetLastStatus()!=Ok) {delete d->referenceVariants[i];d->referenceVariants[i]=nullptr;}
        }
    }
    auto closeBytes=DecodeBase64(kJumpClosePng);
    if(!closeBytes.empty()) { d->closeStream=SHCreateMemStream(closeBytes.data(),(UINT)closeBytes.size());
         if(d->closeStream) { d->closeImage=Image::FromStream(d->closeStream,FALSE);
         if(d->closeImage && d->closeImage->GetLastStatus()!=Ok) {delete d->closeImage;
         d->closeImage=nullptr;
        } } }
    if(!id.empty()) d->docs=ReadJumpDocuments(id,d->frequent);
    RECT client{},bar{}; GetClientRect(s->bar,&client); GetWindowRect(s->bar,&bar); Layout l=GetLayout(client.right,client.bottom);
    RECT button{};
    if(s->verticalActive) {auto v=GetVerticalLayout(client.right,client.bottom);if((int)index<v.offset || (int)index>=v.offset+v.count)return false;
        button=RECT{bar.left,bar.top+v.first+((int)index-v.offset)*v.step,bar.right,bar.top+v.first+((int)index-v.offset+1)*v.step};}
    else {if((int)index>=l.count)return false;button=RECT{bar.left+l.first+(int)index*l.width,bar.top,bar.left+l.first+((int)index+1)*l.width,bar.bottom};}
    MONITORINFO mi{}; mi.cbSize=sizeof(mi); if(!GetMonitorInfoW(MonitorFromRect(&button,MONITOR_DEFAULTTONEAREST),&mi)) return false;
    // Size bounded by the owning monitor; categories stay real, never invented.
    int maxDocs=std::max(0,std::min(10,(int)((mi.rcMonitor.bottom-mi.rcMonitor.top)-Px(150))/std::max(1,Px(22))));
    if(d->docs.size()>(size_t)maxDocs) d->docs.resize(maxDocs);
    const bool win7Jump=taskbarSkin.load()==1;
    d->headerHeight=d->docs.empty()?Px(win7Jump?4:12):Px(win7Jump?25:29); int y=d->headerHeight;
    for(size_t i=0;i<d->docs.size();i++) {
        std::unique_ptr<std::remove_pointer<HICON>::type,DestroyIconHandle> icon(JumpDocumentIcon(d->docs[i].object.get()));
        d->icons.push_back(icon.get()); [[maybe_unused]] auto transferred_icon=icon.release();
        d->rows.push_back({JR_DOCUMENT,(int)i,{Px(10),y,Px(242),y+Px(22)}}); y+=Px(22);
    }
    if(!d->launch.empty() || !d->activationAppId.empty()) {if(!win7Jump || !d->docs.empty())y+=Px(4);
         d->rows.push_back({JR_APP,-1,{Px(10),y,Px(242),y+Px(25)}});
         y+=Px(25);
        }
    if(showJumpShowWindow.load() && d->docs.empty() && !d->targets.empty()) {d->rows.push_back({JR_ACTIVATE,-1,{Px(10),y,Px(242),y+Px(25)}});y+=Px(25);}
    if(d->pinAction) {d->rows.push_back({JR_PIN,-1,{Px(10),y,Px(242),y+Px(22)}});y+=Px(22);}
    if(!d->targets.empty()) {d->rows.push_back({JR_CLOSE,-1,{Px(10),y,Px(242),y+Px(22)}});y+=Px(22);}
    if(d->rows.empty()) return false;
    int width=Px(252),height=y+Px(win7Jump?6:12); RECT frame{0,0,width,height};
    DWORD popupEx=WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
    if(taskbarSkin.load()==1) {
        // Measure and retain the system-provided non-client frame.
        using AdjustFn=BOOL (WINAPI*)(LPRECT,DWORD,BOOL,DWORD,UINT);
        auto adjust=(AdjustFn)GetProcAddress(GetModuleHandleW(L"user32.dll"),"AdjustWindowRectExForDpi");
        auto adjustFrame=[&](RECT* rect) {
            if(!adjust || !adjust(rect,TaskbarPopupStyle(),FALSE,popupEx,(UINT)Dpi()))
                AdjustWindowRectEx(rect,TaskbarPopupStyle(),FALSE,popupEx);
        };
        adjustFrame(&frame);
        int nonClient=(frame.right-frame.left)-width;
        // Remove the two 8-DIP legacy horizontal content margins in Win7;
        // the real non-client DWM frame is retained and measured separately.
        int desired=std::max(Px(80),MulDiv(318,Dpi(),120)-2*Px(8));
        int target=std::min(desired,std::max(1,(int)(mi.rcWork.right-mi.rcWork.left)-4));
        width=std::max(1,target-nonClient);
        for(auto& row:d->rows) {
            row.rect.left=Px(2);
            row.rect.right=std::max(row.rect.left+1L,(LONG)(width-Px(2)));
        }
        frame=RECT{0,0,width,height};adjustFrame(&frame);
    } else AdjustWindowRectEx(&frame,TaskbarPopupStyle(),FALSE,popupEx);
    int w=frame.right-frame.left,h=frame.bottom-frame.top;
    int x=button.left+(button.right-button.left-w)/2, top=button.top-h-Px(4);
    x=std::max((int)mi.rcWork.left+2,std::min(x,(int)mi.rcWork.right-w-2));
    if(top<mi.rcMonitor.top) top=button.bottom+Px(4);
    if(s->verticalActive) {x=s->verticalSide==1?bar.right+Px(4):bar.left-w-Px(4);top=(button.top+button.bottom-h)/2;
        x=std::max((int)mi.rcWork.left,std::min(x,(int)mi.rcWork.right-w));top=std::max((int)mi.rcWork.top,std::min(top,(int)mi.rcWork.bottom-h));}
    WNDCLASSW wc{};
         wc.hInstance=GetModuleHandleW(nullptr);
         wc.lpfnWndProc=JumpPopupProc;
         wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
         wc.lpszClassName=kJumpClass;
    if(!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) {Wh_Log(L"[T81-DIAG] Jump List class registration failed"); return false;}
    WNDCLASSW existing{}; if(!GetClassInfoW(wc.hInstance,kJumpClass,&existing) || existing.lpfnWndProc!=JumpPopupProc) return false;
    // Ownership transfers at WM_NCCREATE; on CreateWindow failure Windows may
    // already have delivered WM_NCDESTROY. Use a creation context to avoid
    // double deletion by keeping ownership in the caller until success.
    BOOL animate=FALSE; bool move=SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animate,0) && animate;
#ifdef TASKBAR81_TEST
    if(testJumpAnimation>=0)move=testJumpAnimation!=0;
#endif
    int offset=move && !s->verticalActive?((top<button.top)?Px(4):-Px(4)):0;
    int offsetX=move && s->verticalActive?(s->verticalSide==1?-Px(4):Px(4)):0;
    JumpPopupData* raw=d.get(); raw->animX=x; raw->animFinalY=top; raw->animStartOffset=offset;raw->animStartOffsetX=offsetX;
    HWND popup=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,kJumpClass,L"",TaskbarPopupStyle(),x+offsetX,top+offset,w,h,s->bar,nullptr,wc.hInstance,raw);
    if(!popup) { Wh_Log(L"[T81-DIAG] Jump List window creation failed error=%lu",GetLastError()); return false; }
    raw->windowOwns=true; [[maybe_unused]] auto transferred_d=d.release(); s->jumpPopup=popup; s->jumpGroupKey=group.key;
    ConfigureSystemPopupFrame(popup);if(!IsWindow(popup))return false;
    if(taskbarSkin.load()==1) {
        RECT actualClient{};if(GetClientRect(popup,&actualClient)) {
            for(auto& row:raw->rows) {
                row.rect.left=Px(2);
                row.rect.right=std::max(row.rect.left+1L,actualClient.right-Px(2));
            }
        }
    }
    raw->mouseMask=((GetAsyncKeyState(VK_LBUTTON)&0x8000)?1:0)|((GetAsyncKeyState(VK_RBUTTON)&0x8000)?2:0);
    Wh_Log(L"[T81-DIAG] Jump List native popup documents=%u",(unsigned)raw->docs.size());
    SetTimer(popup,1,100,nullptr);
    ShowWindow(popup,SW_SHOWNORMAL);
    if(IsWindow(popup)) {
        if(taskbarSkin.load()==1) {
            RECT wr{},cr{};if(GetWindowRect(popup,&wr) && GetClientRect(popup,&cr))
                Wh_Log(L"[T81-JUMP-SIZE] dpi=%d window=%ldx%ld client=%ldx%ld target-width=%d",
                    Dpi(),wr.right-wr.left,wr.bottom-wr.top,cr.right-cr.left,cr.bottom-cr.top,w);
        }
        InvalidateRect(popup,nullptr,FALSE); UpdateWindow(popup); // paint before moving, no black animation surface
        if(IsWindow(popup) && s && s->jumpPopup==popup && move) {
            auto state=(JumpPopupData*)GetWindowLongPtrW(popup,GWLP_USERDATA);
            if(state && !state->closing) {state->animStart=GetTickCount64(); if(!SetTimer(popup,2,16,nullptr)) SetWindowPos(popup,nullptr,x,top,0,0,SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);}
        }
        if(IsWindow(popup) && s && s->jumpPopup==popup) SetForegroundWindow(popup);
    }
    return true;
}

// RetroBar DelayedActivationHandler.cs (Apache-2.0): delayed hover idea,
// reimplemented with OLE IDropTarget. Pin drops copy links, never move data.
static void CancelDragHover() {
    if(!s) return; if(s->bar) KillTimer(s->bar,TIMER_DRAG_ACTIVATE);
    s->dragHoverKey.clear();s->dragTarget=nullptr;s->dragPid=0;s->dragActivated=false;
}
static void UpdateDragHover(POINTL screen) {
    if(!s || !s->bar) return;
    POINT point{screen.x,screen.y}; if(!ScreenToClient(s->bar,&point)) {CancelDragHover();return;}
    int hit=Hit(point.x,point.y);
    if(hit<0 || (size_t)hit>=s->groups.size() || s->groups[hit].tasks.empty()) {CancelDragHover();return;}
    const Group& group=s->groups[hit]; const Task& task=GroupTask(hit);
    if(group.key==s->dragHoverKey && task.hwnd==s->dragTarget && task.pid==s->dragPid) return;
    CancelDragHover();s->dragHoverKey=group.key;s->dragTarget=task.hwnd;s->dragPid=task.pid;
    if(!SetTimer(s->bar,TIMER_DRAG_ACTIVATE,650,nullptr)) CancelDragHover();
}
static void TickDragActivate() {
    if(!s) return; KillTimer(s->bar,TIMER_DRAG_ACTIVATE);
    if(s->dragActivated || !s->dragTarget) return;
    POINT p{}; DWORD pid=0;
    if(!GetCursorPos(&p) || !ScreenToClient(s->bar,&p)) {CancelDragHover();return;}
    int hit=Hit(p.x,p.y);
    if(hit<0 || (size_t)hit>=s->groups.size() || s->groups[hit].key!=s->dragHoverKey ||
       !IsWindow(s->dragTarget) || !GetWindowThreadProcessId(s->dragTarget,&pid) || pid!=s->dragPid) {CancelDragHover();return;}
    s->dragActivated=true; ActivateTask(s->dragTarget);
    Wh_Log(L"[T81-DIAG] OLE drag hover activation attempted hwnd=%p",s->dragTarget);
}
static bool PinDropArea(POINTL screen) {
    if(!s || !s->bar)return false;POINT p{screen.x,screen.y};
    if(!ScreenToClient(s->bar,&p))return false;
    RECT rc{};GetClientRect(s->bar,&rc);if(!PtInRect(&rc,p))return false;
    // An existing app button is reserved for drag-hover activation, never pinning.
    // Hit handles both horizontal and vertical layouts and excludes tray controls.
    int hit=Hit(p.x,p.y);
    if(hit>=0 && (size_t)hit<s->groups.size())return false;
    if(s->verticalActive) {auto v=GetVerticalLayout(rc.right,rc.bottom);return p.y>=v.first && p.y<v.first+v.step*v.count+v.step;}
    int avail=0;Layout l=TaskAreaLayout(rc.right,rc.bottom,avail);
    return p.x>=l.first && p.x<l.first+avail;
}
using DropItemPtr=std::unique_ptr<IShellItem,ReleaseCom<IShellItem>>;
static DropItemPtr ResolvePinDropItem(IShellItem* item) {
    ModPinCandidate direct;if(item && InspectModPin(item,direct)){item->AddRef();return DropItemPtr(item);}
    std::wstring path=ShellItemName(item,SIGDN_FILESYSPATH);
    if(path.empty())return DropItemPtr{};
    if(!_wcsicmp(PathFindExtensionW(path.c_str()),L".lnk")) {
        IShellLinkW* raw=nullptr;
        if(FAILED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&raw))) || !raw)return DropItemPtr{};
        std::unique_ptr<IShellLinkW,ReleaseCom<IShellLinkW>> link(raw);IPersistFile* pfRaw=nullptr;
        if(FAILED(link->QueryInterface(IID_PPV_ARGS(&pfRaw))) || !pfRaw)return DropItemPtr{};
        std::unique_ptr<IPersistFile,ReleaseCom<IPersistFile>> pf(pfRaw);
        if(FAILED(pf->Load(path.c_str(),STGM_READ)))return DropItemPtr{};
        wchar_t target[32768]{};WIN32_FIND_DATAW fd{};
        if(FAILED(link->GetPath(target,(int)std::size(target),&fd,SLGP_RAWPATH)) || !target[0])return DropItemPtr{};
        path=w7t::NormalizeTarget(link.get(),target);
    }
    DWORD attr=GetFileAttributesW(path.c_str());
    if(path.empty() || attr==INVALID_FILE_ATTRIBUTES || (attr&FILE_ATTRIBUTE_DIRECTORY))return DropItemPtr{};
    const wchar_t* ext=PathFindExtensionW(path.c_str());if(!ext || !*ext)return DropItemPtr{};
    wchar_t value[32768]{};DWORD length=(DWORD)std::size(value);
    HRESULT hr=AssocQueryStringW(ASSOCF_NONE,ASSOCSTR_APPID,ext,L"open",value,&length);
    std::wstring parsing;
    if(SUCCEEDED(hr) && w7t::IsPackagedAumid(value))parsing=L"shell:AppsFolder\\"+std::wstring(value);
    if(parsing.empty()) {
        length=(DWORD)std::size(value);value[0]=0;
        hr=AssocQueryStringW(ASSOCF_NONE,ASSOCSTR_EXECUTABLE,ext,L"open",value,&length);
        if(FAILED(hr) || !value[0] || _wcsicmp(PathFindExtensionW(value),L".exe"))return DropItemPtr{};
        parsing=value;
    }
    IShellItem* result=nullptr;hr=SHCreateItemFromParsingName(parsing.c_str(),nullptr,IID_PPV_ARGS(&result));
    DropItemPtr resolved(result);ModPinCandidate check;
    if(FAILED(hr) || !resolved || !InspectModPin(resolved.get(),check))return DropItemPtr{};
    return resolved;
}
static bool ValidateDropItems(std::vector<DropItemPtr>& items) {
    if(items.empty() || items.size()>16)return false;
    for(auto& item:items) {auto resolved=ResolvePinDropItem(item.get());if(!resolved)return false;item=std::move(resolved);}
    return true;
}
static bool GetModPinDropItems(IDataObject* data,std::vector<DropItemPtr>& items,const wchar_t** formatUsed=nullptr) {
    items.clear();if(!data)return false;
    IShellItemArray* raw=nullptr;HRESULT hr=SHCreateShellItemArrayFromDataObject(data,IID_PPV_ARGS(&raw));
    std::unique_ptr<IShellItemArray,ReleaseCom<IShellItemArray>> array(raw);DWORD count=0;
    if(SUCCEEDED(hr) && array && SUCCEEDED(array->GetCount(&count)) && count && count<=16) {
        for(DWORD i=0;i<count;i++) {IShellItem* p=nullptr;hr=array->GetItemAt(i,&p);items.emplace_back(p);if(FAILED(hr) || !p)break;}
        if(items.size()==count && ValidateDropItems(items)) {if(formatUsed)*formatUsed=L"shell";return true;}
    }
    items.clear();
    // CF_HDROP is explicitly supported for filesystem drag sources that do
    // not supply a shell-item array. ReleaseStgMedium owns the data handle.
    FORMATETC format{CF_HDROP,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};STGMEDIUM medium{};
    hr=data->GetData(&format,&medium);if(FAILED(hr))return false;
    struct MediumGuard {STGMEDIUM* p;~MediumGuard(){ReleaseStgMedium(p);}} guard{&medium};
    if(medium.tymed!=TYMED_HGLOBAL || !medium.hGlobal)return false;
    HDROP drop=(HDROP)medium.hGlobal;UINT n=DragQueryFileW(drop,0xFFFFFFFF,nullptr,0);
    if(!n || n>16)return false;
    for(UINT i=0;i<n;i++) {
        UINT len=DragQueryFileW(drop,i,nullptr,0);if(!len || len>=32767){items.clear();return false;}
        std::wstring path((size_t)len+1,L'\0');
        if(!DragQueryFileW(drop,i,path.data(),len+1)){items.clear();return false;}path.resize(len);
        IShellItem* p=nullptr;hr=SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(&p));
        items.emplace_back(p);if(FAILED(hr) || !p){items.clear();return false;}
    }
    if(!ValidateDropItems(items)){items.clear();return false;}
    if(formatUsed)*formatUsed=L"hdrop";return true;
}
static void LogPinDropFormats(IDataObject* data) {
    if(!data)return;IEnumFORMATETC* raw=nullptr;
    HRESULT hr=data->EnumFormatEtc(DATADIR_GET,&raw);
    std::unique_ptr<IEnumFORMATETC,ReleaseCom<IEnumFORMATETC>> list(raw);
    if(FAILED(hr) || !list){Wh_Log(L"[T81-PIN] format enumeration hr=0x%08lx",(unsigned long)hr);return;}
    FORMATETC f{};ULONG fetched=0;int count=0;
    while(count<24 && list->Next(1,&f,&fetched)==S_OK && fetched==1) {
        wchar_t name[128]{};
        if(f.cfFormat==CF_HDROP)wcscpy(name,L"CF_HDROP");
        else if(!GetClipboardFormatNameW(f.cfFormat,name,(int)std::size(name)))wcscpy(name,L"predefined-or-unknown");
        Wh_Log(L"[T81-PIN] offered format=%ls id=%u medium=0x%lx index=%ld",name,(unsigned)f.cfFormat,(unsigned long)f.tymed,(long)f.lindex);
        if(f.ptd)CoTaskMemFree(f.ptd);f=FORMATETC{};++count;
    }
}
// Virtual Shell shortcut transfer: validate only descriptors on drag enter;
// materialize bounded content into a private temporary .lnk on drop.
static bool GetVirtualPinNames(IDataObject* data,std::vector<std::wstring>& names) {
    names.clear();if(!data)return false;
    CLIPFORMAT id=(CLIPFORMAT)RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW);
    if(!id)return false;FORMATETC f{id,nullptr,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};STGMEDIUM m{};
    if(FAILED(data->GetData(&f,&m)))return false;
    struct MediumGuard {STGMEDIUM* p;~MediumGuard(){ReleaseStgMedium(p);}} guard{&m};
    if(m.tymed!=TYMED_HGLOBAL || !m.hGlobal)return false;
    SIZE_T bytes=GlobalSize(m.hGlobal);auto* group=(FILEGROUPDESCRIPTORW*)GlobalLock(m.hGlobal);
    if(!group)return false;
    struct UnlockGuard {HGLOBAL h;~UnlockGuard(){GlobalUnlock(h);}} unlock{m.hGlobal};
    if(bytes<offsetof(FILEGROUPDESCRIPTORW,fgd) || !group->cItems || group->cItems>16 ||
       bytes<offsetof(FILEGROUPDESCRIPTORW,fgd)+(size_t)group->cItems*sizeof(FILEDESCRIPTORW))return false;
    for(UINT i=0;i<group->cItems;i++) {
        const auto& desc=group->fgd[i];
        if((desc.dwFlags&FD_ATTRIBUTES) && (desc.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){names.clear();return false;}
        const wchar_t* end=std::find(std::begin(desc.cFileName),std::end(desc.cFileName),L'\0');
        if(end==std::end(desc.cFileName)){names.clear();return false;}
        std::wstring name(desc.cFileName,end);
        if(name.empty() || name.find_first_of(L"\\/")!=std::wstring::npos || _wcsicmp(PathFindExtensionW(name.c_str()),L".lnk")){names.clear();return false;}
        if((desc.dwFlags&FD_FILESIZE) && (desc.nFileSizeHigh || desc.nFileSizeLow>2*1024*1024)){names.clear();return false;}
        names.push_back(std::move(name));
    }
    return !names.empty();
}
static bool MaterializeVirtualPin(IDataObject* data,LONG index,const std::wstring& name) {
    constexpr size_t limit=2u*1024u*1024u;
    CLIPFORMAT id=(CLIPFORMAT)RegisterClipboardFormatW(CFSTR_FILECONTENTS);
    if(!id)return false;FORMATETC f{id,nullptr,DVASPECT_CONTENT,index,TYMED_ISTREAM};STGMEDIUM m{};
    HRESULT getHr=data->GetData(&f,&m);
    if(FAILED(getHr)) {f.tymed=TYMED_HGLOBAL;m=STGMEDIUM{};getHr=data->GetData(&f,&m);}
    if(FAILED(getHr)){Wh_Log(L"[T81-PIN] virtual contents GetData failed hr=0x%08lx index=%ld",(unsigned long)getHr,(long)index);return false;}
    Wh_Log(L"[T81-PIN] virtual contents medium=%lu index=%ld",(unsigned long)m.tymed,(long)index);
    struct MediumGuard {STGMEDIUM* p;~MediumGuard(){ReleaseStgMedium(p);}} guard{&m};
    std::vector<BYTE> bytes;
    if(m.tymed==TYMED_HGLOBAL && m.hGlobal) {
        SIZE_T size=GlobalSize(m.hGlobal);if(!size || size>limit)return false;
        void* p=GlobalLock(m.hGlobal);if(!p)return false;
        struct Unlock {HGLOBAL h;~Unlock(){GlobalUnlock(h);}} unlock{m.hGlobal};
        bytes.assign((BYTE*)p,(BYTE*)p+size);
    } else if(m.tymed==TYMED_ISTREAM && m.pstm) {
        BYTE buffer[16384];
        for(;;) {ULONG got=0;HRESULT hr=m.pstm->Read(buffer,sizeof(buffer),&got);
            if(FAILED(hr) || got>sizeof(buffer) || bytes.size()+got>limit)return false;
            bytes.insert(bytes.end(),buffer,buffer+got);if(hr==S_FALSE || !got)break;}
        if(bytes.empty())return false;
    } else return false;
    wchar_t tempDir[MAX_PATH]{};DWORD n=GetTempPathW((DWORD)std::size(tempDir),tempDir);
    if(!n || n>=std::size(tempDir)){Wh_Log(L"[T81-PIN] virtual temp folder failed error=%lu",GetLastError());return false;}
    wchar_t temp[MAX_PATH]{};if(!GetTempFileNameW(tempDir,L"t81",0,temp)){Wh_Log(L"[T81-PIN] virtual temp create failed error=%lu",GetLastError());return false;}
    std::wstring tempPath=temp,linkPath=tempPath+L".lnk";
    struct TempGuard {std::wstring a,b;~TempGuard(){if(!a.empty())DeleteFileW(a.c_str());if(!b.empty())DeleteFileW(b.c_str());}} cleanup{tempPath,L""};
    if(!MoveFileW(tempPath.c_str(),linkPath.c_str())){Wh_Log(L"[T81-PIN] virtual temp rename failed error=%lu",GetLastError());return false;}
    cleanup.a.clear();cleanup.b=linkPath;
    {ScopedHandle file(CreateFileW(linkPath.c_str(),GENERIC_WRITE,0,nullptr,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
        if(file.h==INVALID_HANDLE_VALUE)return false;DWORD wrote=0;
        if(!WriteFile(file.h,bytes.data(),(DWORD)bytes.size(),&wrote,nullptr) || wrote!=bytes.size())return false;}
    IShellItem* raw=nullptr;HRESULT hr=SHCreateItemFromParsingName(linkPath.c_str(),nullptr,IID_PPV_ARGS(&raw));
    DropItemPtr item(raw);
    if(FAILED(hr) || !item){Wh_Log(L"[T81-PIN] virtual shell item failed hr=0x%08lx",(unsigned long)hr);return false;}
    auto resolved=ResolvePinDropItem(item.get());
    if(!resolved){
        IShellLinkW* rawLink=nullptr;HRESULT loadHr=E_FAIL,pathHr=E_FAIL,pidlHr=E_FAIL;bool hasPath=false,hasPidl=false;
        if(SUCCEEDED(CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&rawLink))) && rawLink) {
            std::unique_ptr<IShellLinkW,ReleaseCom<IShellLinkW>> link(rawLink);IPersistFile* rawFile=nullptr;
            if(SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&rawFile))) && rawFile) {
                std::unique_ptr<IPersistFile,ReleaseCom<IPersistFile>> file(rawFile);loadHr=file->Load(linkPath.c_str(),STGM_READ);
                if(SUCCEEDED(loadHr)) {wchar_t target[32768]{};WIN32_FIND_DATAW fd{};
                    pathHr=link->GetPath(target,(int)std::size(target),&fd,SLGP_RAWPATH);hasPath=target[0]!=0;
                    PIDLIST_ABSOLUTE pidl=nullptr;pidlHr=link->GetIDList(&pidl);hasPidl=pidl!=nullptr;if(pidl)CoTaskMemFree(pidl);}
            }
        }
        Wh_Log(L"[T81-PIN] virtual shortcut rejected load=0x%08lx pathHr=0x%08lx pathPresent=%d pidlHr=0x%08lx pidlPresent=%d",(unsigned long)loadHr,(unsigned long)pathHr,hasPath?1:0,(unsigned long)pidlHr,hasPidl?1:0);
        return false;
    }
    bool ok=CreateModPin(resolved.get(),L"",resolved.get()==item.get()?name:L"");Wh_Log(L"[T81-PIN] virtual pin result=%d",ok?1:0);return ok;
}
static DWORD SafePinDropEffect(DWORD allowed) {
    if(allowed&DROPEFFECT_LINK)return DROPEFFECT_LINK;
    if(allowed&DROPEFFECT_COPY)return DROPEFFECT_COPY;
    return DROPEFFECT_NONE; // never claim MOVE; source files must remain untouched
}
class TaskDropTarget final : public IDropTarget {
    std::atomic<ULONG> refs{1};bool canPin=false,virtualPin=false;DWORD allowed=0;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** out) override {
        if(!out) return E_POINTER; *out=nullptr;
        if(IsEqualIID(iid,IID_IUnknown) || IsEqualIID(iid,IID_IDropTarget)) {*out=static_cast<IDropTarget*>(this);AddRef();return S_OK;}
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {return ++refs;}
    ULONG STDMETHODCALLTYPE Release() override {ULONG n=--refs;if(!n) delete this;return n;}
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* data,DWORD,POINTL pt,DWORD* effect) override {
        if(!effect)return E_POINTER;allowed=*effect;*effect=DROPEFFECT_NONE;canPin=false;virtualPin=false;
        try {CancelDragHover();std::vector<DropItemPtr> items;std::vector<std::wstring> virtualNames;const wchar_t* format=L"none";
            if(SafePinDropEffect(allowed)) {
                canPin=GetModPinDropItems(data,items,&format);
                if(!canPin && GetVirtualPinNames(data,virtualNames)) {canPin=true;virtualPin=true;format=L"virtual-shortcut";}
            }
            if(!canPin)LogPinDropFormats(data);
            bool inArea=PinDropArea(pt);
            Wh_Log(L"[T81-PIN] DragEnter allowed=0x%lx format=%ls accepted=%d area=%d count=%u",(unsigned long)allowed,format,canPin?1:0,inArea?1:0,(unsigned)items.size());
            if(canPin && inArea)*effect=SafePinDropEffect(allowed);else UpdateDragHover(pt);
        } catch(...) {canPin=false;CancelDragHover();Wh_Log(L"[T81-PIN] drag enter exception");}return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD,POINTL pt,DWORD* effect) override {
        if(!effect)return E_POINTER;DWORD current=*effect;*effect=DROPEFFECT_NONE;
        try {if(canPin && PinDropArea(pt)){CancelDragHover();*effect=SafePinDropEffect(allowed & current);}else UpdateDragHover(pt);}
        catch(...) {CancelDragHover();Wh_Log(L"[T81-PIN] drag hover exception");}return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() override {canPin=false;virtualPin=false;allowed=0;CancelDragHover();return S_OK;}
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* data,DWORD,POINTL pt,DWORD* effect) override {
        if(!effect)return E_POINTER;DWORD current=*effect;*effect=DROPEFFECT_NONE;CancelDragHover();
        DWORD selected=SafePinDropEffect(allowed & current);
        bool accept=canPin && selected && PinDropArea(pt),wasVirtual=virtualPin;
        canPin=false;virtualPin=false;allowed=0;
        Wh_Log(L"[T81-PIN] Drop accepted=%d effect=0x%lx",accept?1:0,(unsigned long)selected);
        if(!accept || !data || !s)return S_OK;
        try {
            std::vector<DropItemPtr> items;std::vector<std::wstring> names;const wchar_t* format=L"none";
            size_t succeeded=0,total=0;
            if(wasVirtual) {
                if(!GetVirtualPinNames(data,names))return S_OK;
                format=L"virtual-shortcut";total=names.size();
                for(size_t i=0;i<names.size();i++)if(MaterializeVirtualPin(data,(LONG)i,names[i]))++succeeded;
            } else {
                if(!GetModPinDropItems(data,items,&format))return S_OK;
                total=items.size();for(const auto& item:items)if(CreateModPin(item.get()))++succeeded;
            }
            Wh_Log(L"[T81-PIN] Drop format=%ls success=%u total=%u",format,(unsigned)succeeded,(unsigned)total);
            if(succeeded){*effect=selected;RefreshModPinView();}
        } catch(...) {Wh_Log(L"[T81-PIN] drop exception");}
        return S_OK;
    }
};
static void RegisterTaskDropTarget(HWND h) {
    if(!s || !h) return;
    TaskDropTarget* target=new(std::nothrow) TaskDropTarget;
    if(!target) {Wh_Log(L"[T81-DIAG] OLE target allocation failed");return;}
    HRESULT hr=RegisterDragDrop(h,target); target->Release();
    s->dropRegistered=SUCCEEDED(hr);
    Wh_Log(L"[T81-DIAG] OLE task drop target registered=%d hr=0x%08lx",s->dropRegistered?1:0,(unsigned long)hr);
}
// RetroBar also uses a low-level mouse hook while unlocked. Install it only
// during a drag: capture alone cannot track a non-foreground taskbar reliably.
static constexpr UINT WM_BAR_DRAG_POINT=WM_APP+450,WM_BAR_DRAG_END=WM_APP+451;
static LRESULT CALLBACK BarDragHookProc(int code,WPARAM msg,LPARAM data) {
    if(code>=0 && s && s->barDragCandidate && data && !s->barHookEndQueued) {
        auto* event=reinterpret_cast<MSLLHOOKSTRUCT*>(data);
        if(msg==WM_MOUSEMOVE && !s->barMoveQueued) {
            if(PostMessageW(s->bar,WM_BAR_DRAG_POINT,(WPARAM)(INT_PTR)event->pt.x,(LPARAM)event->pt.y)) s->barMoveQueued=true;
        }
        else if(msg==WM_LBUTTONUP) {
            if(PostMessageW(s->bar,WM_BAR_DRAG_END,(WPARAM)(INT_PTR)event->pt.x,(LPARAM)event->pt.y)) {
                s->barHookEndQueued=true;return 1; // the empty-space release belongs to this gesture
            }
        }
    }
    return CallNextHookEx(nullptr,code,msg,data);
}
static void CancelBarEdgeDrag(bool revert) {
    if(!s)return;
    bool active=s->barDragCandidate,changed=s->barDragMoved;int initial=s->barDragInitial;
    s->barDragCandidate=false;s->barDragMoved=false;s->barDragPending=0;s->barHookEndQueued=false;s->barMoveQueued=false;
    HHOOK hook=s->barDragHook;s->barDragHook=nullptr;if(hook)UnhookWindowsHookEx(hook);
    if(active && changed) {
        // A deliberate drop or revert retries immediately and bypasses cached
        // geometry so the final edge always receives QUERYPOS -> SETPOS.
        s->appbarRetryAt=0;s->appbarDirty=true;
    }
    if(active && revert && changed) {s->barEdgeOverride=initial;UpdatePlacement();InvalidateRect(s->bar,nullptr,FALSE);}
}
// RetroBar's screen-wide diagonal-quadrant classification is used during
// motion and on release. The current cursor is sampled for queued move
// messages to avoid applying stale pointer coordinates after a busy UI thread.
// The actual hook-provided button-up point is authoritative at commit.
static void UpdateBarEdgeDrag(HWND h,POINT point) {
    if(!s || !s->barDragCandidate || !PtInRect(&s->barDragMonitor,point))return;
    int dx=std::abs(point.x-s->barDragStart.x),dy=std::abs(point.y-s->barDragStart.y);
    // Deliberate difference from RetroBar: allow a strictly horizontal or
    // vertical drag after either Windows drag threshold is exceeded.
    if(dx<GetSystemMetrics(SM_CXDRAG) && dy<GetSystemMetrics(SM_CYDRAG))return;
    BarEdge edge=RetroBarDragCoordsToScreenEdge(point.x,point.y,s->barDragMonitor);
    if(!HorizontalEdge(edge) && taskbarPlacementMode.load()!=3)return;
    int choice=edge==BarEdge::Top?1:edge==BarEdge::Bottom?2:edge==BarEdge::Left?3:4;
    s->barDragPending=choice;
    if(s->barEdgeOverride==choice)return;
    int previous=s->barEdgeOverride;
    // Release the old reservation before moving the visual window away from
    // the approved AppBar rectangle. Drop or revert registers it again.
    RemoveWorkAreaAppBar();
    s->barEdgeOverride=choice;s->barDragMoved=true;
    Wh_Log(L"[T81-DIAG] drag live x=%ld y=%ld edge=%ls choice=%d previous=%d",point.x,point.y,EdgeName(edge),choice,previous);
    HidePrivateOverflow();HidePreview();CloseJumpPopup(false);CloseDesktopPopup();PeekOff(true);
    UpdatePlacement();InvalidateRect(h,nullptr,FALSE);
}
// Upright private bar. Kept separate from horizontal input and paint.
// Limitations: no native vertical bar; vertical AppBar is controlled by its own work-area option.
static int VerticalHit(int x,int y) {
    RECT r{};GetClientRect(s->bar,&r);VerticalLayout v=GetVerticalLayout(r.right,r.bottom);POINT p{x,y};
    if(PtInRect(&v.icons,p)) {auto shown=VisibleTrayEntries();
        int slot=(p.y-v.icons.top)/std::max(1,Px(24));
        if(slot>=0 && (size_t)slot<shown.size()){int entry=shown[slot],hit=SystemHitForEntry(entry);
        return hit?hit:HIT_TRAY_ICON_BASE-entry;
        }}
    if(PtInRect(&v.start,p) && (!startHotCorner.load() || (s->verticalSide==1?x<std::max(1,Px(2)):x>=r.right-std::max(1,Px(2)))))return HIT_START;if(PtInRect(&v.desktop,p))return HIT_DESKTOP;
    if(PtInRect(&v.language,p))return HIT_LANGUAGE;
    if(PtInRect(&v.clock,p))return HIT_CLOCK;if(PtInRect(&v.arrow,p))return HIT_TRAY_MORE;
    if(PtInRect(&v.network,p))return HIT_NETWORK;if(PtInRect(&v.volume,p))return HIT_VOLUME;
    if(PtInRect(&v.battery,p))return HIT_BATTERY;if(PtInRect(&v.more,p))return HIT_OVERFLOW;
    if(PtInRect(&v.desktopBand,p))return HIT_BAND_DESKTOP;
    if(PtInRect(&v.linksBand,p))return HIT_BAND_LINKS;
    if(PtInRect(&v.addressBand,p))return HIT_BAND_ADDRESS;
    if(x>=0 && x<r.right && y>=v.first && y<v.first+v.count*v.step)return v.offset+(y-v.first)/v.step;
    return HIT_NONE;
}
// Same two painted triangles as the horizontal overflow button, rotated
// around their center: the tip faces into the desktop on each vertical side.
static void DrawVerticalTrayArrow(Gdiplus::Graphics& g,const RECT& r,int side) {
    float cx=(r.left+r.right)/2.f,cy=(r.top+r.bottom)/2.f,sc=Dpi()/96.f;
    auto rotate=[&](float x,float y) { return side==1?Gdiplus::PointF(cx-y*sc,cy+x*sc):Gdiplus::PointF(cx+y*sc,cy-x*sc); };
    Gdiplus::PointF outer[]={rotate(-4.6f,2.5f),rotate(0,-3.3f),rotate(4.6f,2.5f)};
    Gdiplus::PointF inner[]={rotate(-3.3f,1.3f),rotate(0,-1.9f),rotate(3.3f,1.3f)};
    Gdiplus::SolidBrush shadow(Gdiplus::Color(145,0,0,0));
    Gdiplus::SolidBrush fill(Gdiplus::Color(255,245,253,255));
    g.FillPolygon(&shadow,outer,3);g.FillPolygon(&fill,inner,3);
}
// Reuse horizontal task/tray tile renderers in rotated geometry.  The
// application icons and text themselves remain upright.
static void BeginVerticalTile(Graphics& g,const RECT& r) {
    g.SetClip(Rect(r.left,r.top,std::max(0,(int)(r.right-r.left)),std::max(0,(int)(r.bottom-r.top))),CombineModeIntersect);
    if(s->verticalSide==1) {g.TranslateTransform((REAL)r.right,(REAL)r.top);g.RotateTransform(90.f);}
    else {g.TranslateTransform((REAL)r.left,(REAL)r.bottom);g.RotateTransform(-90.f);}
}
static void DrawVerticalTrayHover(Graphics& g,const RECT& r,float opacity=1.f) {
    if(IsRectEmpty(&r))return;
    auto saved=g.Save();BeginVerticalTile(g,r);
    RECT rotated{0,0,r.bottom-r.top,r.right-r.left};DrawTrayHoverTile(g,rotated,true,opacity);
    g.Restore(saved);
}
#ifdef TASKBAR81_TEST
// Retained only for regression tests of the disabled 0.50 implementation.
static bool ButtonAnimationPreference(bool spiOk,bool client, LSTATUS reg,DWORD value) {
    return spiOk && client && (reg==ERROR_FILE_NOT_FOUND || (reg==ERROR_SUCCESS && value!=0));
}
static int testButtonAnimationOverride=-1;
#endif
static bool ButtonAnimationsEnabled() {
#ifdef TASKBAR81_TEST
    if(testButtonAnimationOverride>=0)return testButtonAnimationOverride!=0;
#endif
    // 0.50's approximation was rejected; no production transition until a
    // reliable Windows 8.1 reference and an explicit future revision exist.
    return false;
}
static bool TaskIdentityPresent(const std::vector<Task>& tasks,const Task& item) {
    for(const auto& t:tasks) if(t.hwnd==item.hwnd && t.pid==item.pid) return true;
    return false;
}
static RECT ButtonGroupRect(size_t i) {
    RECT r{}; if(!s || !s->bar || !GetClientRect(s->bar,&r))return RECT{};
    if(s->verticalActive) {
        VerticalLayout v=GetVerticalLayout(r.right,r.bottom);int slot=(int)i-v.offset;
        return slot>=0 && slot<v.count?RECT{0,v.first+slot*v.step,r.right,v.first+(slot+1)*v.step}:RECT{};
    }
    Layout l=GetLayout(r.right,r.bottom);if(i>=(size_t)l.count)return RECT{};
    int x=l.first+(int)i*l.width;
    for(const auto& p:s->visualPositions)if(p.key==s->groups[i].key){x=(int)std::lround(p.x);break;}
    return RECT{x,0,x+l.width,r.bottom};
}
static PendingButtonChanges PrepareButtonChanges(const std::vector<Task>& next) {
    PendingButtonChanges p;
    // No entry effects on initial discovery, failed preference lookup or drag.
    if(!s || !s->bar || s->ticks<=1 || s->barDragCandidate || s->dragActive || !ButtonAnimationsEnabled()) return p;
    p.enabled=true;for(const auto& g:s->groups)p.previousKeys.push_back(g.key);
    RECT client{};GetClientRect(s->bar,&client);
    for(size_t i=0;i<s->groups.size() && p.closed.size()<MAX_TASKS;++i) {
        const auto& g=s->groups[i];bool removed=false;
        for(size_t idx:g.tasks)if(idx<s->tasks.size() && !TaskIdentityPresent(next,s->tasks[idx])){removed=true;break;}
        if(!removed)continue;
        RECT r=ButtonGroupRect(i);if(IsRectEmpty(&r))continue;
        ButtonTransition t; t.key=g.key;t.rect=r;t.vertical=s->verticalActive;t.layers=(int)std::min<size_t>(g.tasks.size(),3);
        t.barWidth=client.right;t.barHeight=client.bottom;t.title=GroupTitle(i);
        HICON icon=GroupIcon(i);if(icon){HICON copy=CopyIcon(icon);if(copy)t.icon={copy,DestroyIconHandle{}};}
        p.closed.push_back(std::move(t));
    }
    return p;
}
static void CommitButtonChanges(PendingButtonChanges&& p,const std::vector<Task>& previous) {
    if(!s)return;
    if(!p.enabled) { if(!ButtonAnimationsEnabled()) {s->buttonTransitions.clear();if(s->bar)KillTimer(s->bar,TIMER_BUTTON_TRANSITION);} return; }
    const ULONGLONG now=GetTickCount64();
    auto push=[&](ButtonTransition&& t) {
        s->buttonTransitions.erase(std::remove_if(s->buttonTransitions.begin(),s->buttonTransitions.end(),[&](const ButtonTransition& old){return old.key==t.key;}),s->buttonTransitions.end());
        if(s->buttonTransitions.size()<MAX_TASKS){t.start=now;s->buttonTransitions.push_back(std::move(t));}
    };
    for(auto& t:p.closed) {
        bool remains=false;for(const auto& g:s->groups)if(g.key==t.key){remains=true;break;}
        t.ghost=!remains;push(std::move(t));
    }
    for(const Task& task:s->tasks)if(!TaskIdentityPresent(previous,task)) {
        for(size_t i=0;i<s->groups.size();++i) {
            bool inGroup=false;
                for(size_t idx:s->groups[i].tasks)if(idx<s->tasks.size() && s->tasks[idx].hwnd==task.hwnd && s->tasks[idx].pid==task.pid){inGroup=true;
                break;
                }
            if(!inGroup)continue;
            RECT r=ButtonGroupRect(i);if(IsRectEmpty(&r))break;
            ButtonTransition t;t.key=s->groups[i].key;t.vertical=s->verticalActive;t.rect=r;
            t.reveal=std::find(p.previousKeys.begin(),p.previousKeys.end(),t.key)==p.previousKeys.end();
            push(std::move(t));break;
        }
    }
    if(!s->buttonTransitions.empty() && !SetTimer(s->bar,TIMER_BUTTON_TRANSITION,16,nullptr))s->buttonTransitions.clear();
}
static void TickButtonTransitions() {
    if(!s)return;ULONGLONG now=GetTickCount64();
    s->buttonTransitions.erase(std::remove_if(s->buttonTransitions.begin(),s->buttonTransitions.end(),[&](const ButtonTransition& t){return now<t.start || now-t.start>=180;}),s->buttonTransitions.end());
    if(s->buttonTransitions.empty())KillTimer(s->bar,TIMER_BUTTON_TRANSITION);
    InvalidateRect(s->bar,nullptr,FALSE);
}
static float ButtonTransitionFraction(const ButtonTransition& t) {
    ULONGLONG now=s && s->buttonPaintTick?s->buttonPaintTick:GetTickCount64();return now<t.start?0.f:std::min(1.f,(float)(now-t.start)/180.f);
}
static RECT ButtonRevealRect(RECT r,float portion,bool vertical) {
    portion=std::max(0.f,std::min(1.f,portion));
    if(vertical){int n=(int)std::lround((r.bottom-r.top)*portion);int mid=(r.top+r.bottom)/2;r.top=mid-n/2;r.bottom=r.top+n;}
    else {int n=(int)std::lround((r.right-r.left)*portion);int mid=(r.left+r.right)/2;r.left=mid-n/2;r.right=r.left+n;}
    return r;
}
static const ButtonTransition* LiveButtonTransition(const std::wstring& key) {
    for(const auto& t:s->buttonTransitions)if(!t.ghost && t.key==key && t.vertical==s->verticalActive && ButtonTransitionFraction(t)<1.f)return &t;
    return nullptr;
}
static void ButtonPulse(Graphics& g,const RECT& r,const ButtonTransition* t) {
    if(!t)return;float f=ButtonTransitionFraction(*t);int alpha=(int)std::lround(45.f*(1.f-f));
    if(alpha>0){SolidBrush b(Color(alpha,255,255,255));g.FillRectangle(&b,(int)r.left,(int)r.top,(int)(r.right-r.left),(int)(r.bottom-r.top));}
}
static bool ValidGhost(const ButtonTransition& t,int w,int h,bool vertical) {
    return t.ghost && t.vertical==vertical && t.barWidth==w && t.barHeight==h && ButtonTransitionFraction(t)<1.f;
}
// Keep the exact vertical 8.1 cell geometry: the task frame spans the bar.
// Only the beta artwork inside that frame differs from the 8.1 renderer.
static RECT BetaVerticalTaskBounds(const RECT& cell) { return cell; }
static RECT BetaVerticalTaskInterior(const RECT& cell) {
    RECT inner=cell;
    int edge=std::min(std::max(1,Px(2)),std::min((int)(cell.right-cell.left),(int)(cell.bottom-cell.top))/4);
    InflateRect(&inner,-edge,-edge);
    return inner;
}
static bool DrawBetaVerticalTaskState(Graphics& g,const RECT& bounds,int layers,bool active,bool hot,bool pressed,bool attention,float fade) {
    const bool running=layers>0;
    if(!running && !hot && !pressed && !attention)return BetaTextureAvailable(2);
    const int width=(int)(bounds.bottom-bounds.top),height=(int)(bounds.right-bounds.left);
    if(width<=0 || height<=0)return false;
    // Use exactly the horizontal renderer and its original state textures.
    // Like the 8.1 vertical task renderer, map the complete bar-width cell to
    // local coordinates; never replace its width with an icon-sized rectangle.
    auto textureState=g.Save();BeginVerticalTile(g,bounds);
    RECT local{0,0,width,height};
    bool drawn=DrawBetaTaskState(g,local,running,active,hot,pressed,attention,false,layers>1);
    g.Restore(textureState);
    if(!drawn)return false;
    // Paint only the perimeter with the existing 8.1 frame/stack logic. This
    // leaves the horizontal beta bitmap untouched inside the frame.
    RECT inner=BetaVerticalTaskInterior(bounds);
    auto frameState=g.Save();
    g.SetClip(Rect((int)bounds.left,(int)bounds.top,(int)(bounds.right-bounds.left),(int)(bounds.bottom-bounds.top)),CombineModeIntersect);
    if(!IsRectEmpty(&inner))g.SetClip(Rect((int)inner.left,(int)inner.top,(int)(inner.right-inner.left),(int)(inner.bottom-inner.top)),CombineModeExclude);
    BeginVerticalTile(g,bounds);
    if(running)DrawTaskTileStack(g,0,width,height,layers,active,fade);
    else DrawTrayHoverTile(g,local,true,fade);
    g.Restore(frameState);
    return true;
}
static void DrawVerticalTaskTile(Graphics& g,const RECT& r,int layers,bool active,float fade) {
    if(IsRectEmpty(&r))return;
    if(BetaTextureAvailable(2) && DrawBetaVerticalTaskState(g,BetaVerticalTaskBounds(r),layers,active,fade>0.01f,false,false,fade))return;
    auto saved=g.Save();BeginVerticalTile(g,r);
    if(layers>0)DrawTaskTileStack(g,0,r.bottom-r.top,r.right-r.left,layers,active,fade);
    else if(fade>0) {RECT rotated{0,0,r.bottom-r.top,r.right-r.left};DrawTrayHoverTile(g,rotated,true,fade);}
    g.Restore(saved);
}
static void DrawVerticalForeground(HDC dc,int w,int h) {
    VerticalLayout v=GetVerticalLayout(w,h);HWND fg=GetForegroundWindow();
    {
        Graphics g(dc);g.SetSmoothingMode(SmoothingModeAntiAlias);
        // Same sprite, fade and native horizontal thickness, centred in the
        // upright start cell. The Windows logo itself must not be rotated.
        if(!startHotCorner.load()) {
        int sw=std::min(w,Px(kVerticalStartLogical)),sh=std::min((int)v.start.bottom,w);
        auto startState=g.Save();g.SetClip(Rect(0,0,w,(int)v.start.bottom));
        g.TranslateTransform((w-sw)/2.0f,((int)v.start.bottom-sh)/2.0f);
        bool down=s->verticalPressed==HIT_START && s->hot==HIT_START;
        if(taskbarSkin.load()==1 && (s->win7StartImage || s->betaTexture[30])) {
            g.Restore(startState);DrawWin7Start(g,w,(int)v.start.bottom,down?2:(s->hot==HIT_START?1:0));startState=g.Save();
        } else if(useBetaStartLogo.load() && s->betaStartImage) {
            g.Restore(startState);
            if(BetaTextureAvailable(11)) {
                int edge=std::min(w,std::max(1,Px(2)));
                RECT imageRect{s->verticalSide==1?0:edge,0,s->verticalSide==1?w-edge:w,(int)v.start.bottom};
                if(customStartColor.load() && taskbarSkin.load()==4) {Image* image=CustomBetaStart(s->betaTexture[11],1);g.DrawImage(image,Rect(imageRect.left,imageRect.top,imageRect.right-imageRect.left,imageRect.bottom-imageRect.top),0,0,(int)image->GetWidth(),(int)image->GetHeight(),UnitPixel);}
                else DrawBetaTexture(g,11,imageRect);
                if(s->hot==HIT_START) {
                    SolidBrush stateCue(down?Color(50,0,0,0):Color(28,255,255,255));
                    g.FillRectangle(&stateCue,(int)imageRect.left,0,(int)(imageRect.right-imageRect.left),(int)imageRect.bottom);
                }
            } else DrawBetaStartImageVertical(g,w,(int)v.start.bottom,down?2:(s->hot==HIT_START?1:0));
            startState=g.Save();
        } else {
        DrawStartImage(g,sw,sh,0,1.f,nullptr);
        float fade=down?1.f:std::max(0.f,std::min(1.f,s->startFade));
        if(fade>0) {
            auto fillState=g.Save();g.ResetTransform();
            SolidBrush fullTile(Color((int)std::lround(255*fade),17,17,17));
            g.FillRectangle(&fullTile,0,0,w,(int)v.start.bottom);g.Restore(fillState);
            Color tint=(taskbarSkin.load()==4 && customStartColor.load())?StartCustomColor():StartHoverColor();
                DrawStartImage(g,sw,sh,0,fade,&tint);
                DrawStartImage(g,sw,sh,down?2:1,fade);
        }
        if(down) {SolidBrush pressed(Color(50,0,0,0));g.FillRectangle(&pressed,0,0,sw,sh);}
        }
        g.Restore(startState);
        }
        EnsureTrayHoverImage();
        for(const auto& t:s->buttonTransitions)if(ValidGhost(t,w,h,true)) {
            RECT clip=ButtonRevealRect(t.rect,1.f-ButtonTransitionFraction(t),true);
            auto save=g.Save();g.SetClip(Rect(clip.left,clip.top,clip.right-clip.left,clip.bottom-clip.top),CombineModeIntersect);
            DrawVerticalTaskTile(g,t.rect,t.layers,false,0);g.Restore(save);
        }
        for(int slot=0;slot<v.count;slot++) {
            int i=v.offset+slot;RECT tile{0,v.first+slot*v.step,w,v.first+(slot+1)*v.step};
            bool active=false;for(size_t index:s->groups[i].tasks)if(index<s->tasks.size() && s->tasks[index].hwnd==fg)active=true;
            float hover=GroupHoverFade(i);
            const auto* transition=LiveButtonTransition(s->groups[i].key);auto save=g.Save();
            if(transition && transition->reveal){RECT clip=ButtonRevealRect(tile,ButtonTransitionFraction(*transition),true);g.SetClip(Rect(clip.left,clip.top,clip.right-clip.left,clip.bottom-clip.top),CombineModeIntersect);}
            bool betaTask=false;
            if(BetaTextureAvailable(2)) {
                RECT bounds=BetaVerticalTaskBounds(tile);
                betaTask=DrawBetaVerticalTaskState(g,bounds,TaskTileLayers(s->groups[i].tasks.size()),active,s->hot==i,
                    s->verticalPressed==i && s->hot==i,GroupNeedsAttention((size_t)i),hover);
            }
            if(!betaTask)DrawVerticalTaskTile(g,tile,TaskTileLayers(s->groups[i].tasks.size()),active,hover);
            if(betaTask && !s->groups[i].tasks.empty() && s->hot==i)DrawBetaHotTrack(g,BetaVerticalTaskInterior(tile),i,hover);
            ButtonPulse(g,tile,transition);g.Restore(save);
            if(!betaTask && GroupNeedsAttention((size_t)i)) {
                SolidBrush attention(Color(125,255,140,20));
                g.FillRectangle(&attention,(int)tile.left,(int)tile.top,(int)(tile.right-tile.left),(int)(tile.bottom-tile.top));
            }
        }
        if(s->dragActive && s->dragInsertion>=0){int slot=std::max(0,std::min(v.count,s->dragInsertion-v.offset));
            int yy=v.first+slot*v.step;
            SolidBrush caret(Color(220,255,255,255));
            g.FillRectangle(&caret,Px(4),yy-Px(1),std::max(1,w-Px(8)),std::max(1,Px(2)));
            }
        for(auto pair:{std::pair<int,RECT>{HIT_NETWORK,v.network},{HIT_VOLUME,v.volume},{HIT_BATTERY,v.battery},{HIT_CLOCK,v.clock},{HIT_LANGUAGE,v.language},{HIT_BAND_ADDRESS,v.addressBand},{HIT_OVERFLOW,v.more},{HIT_BAND_DESKTOP,v.desktopBand},{HIT_BAND_LINKS,v.linksBand}})
            if(s->hot==pair.first)DrawVerticalTrayHover(g,pair.second);
        for(int index:VisiblePrivateIcons())if(s->hot==HIT_TRAY_ICON_BASE-index)DrawVerticalTrayHover(g,PrivateIconRect(index));
        if(s->trayDrag.active && s->trayDrag.dropArea==1 && !IsRectEmpty(&s->trayDrag.dropCell)) {
            RECT c=s->trayDrag.dropCell;int y=s->trayDrag.dropAfter?c.bottom-1:c.top;SolidBrush cue(Color(235,240,250,255));
            g.FillRectangle(&cue,Px(5),y,std::max(1,w-Px(10)),std::max(1,Px(2)));
        }
        bool win7Arrow=DrawWin7OverflowButton(g,v.arrow,true);
        if(!win7Arrow && (s->hot==HIT_TRAY_MORE || (s->overflowPanel && IsWindowVisible(s->overflowPanel)))) {
            auto saved=g.Save();
                BeginVerticalTile(g,v.arrow);
                RECT r{0,0,v.arrow.bottom-v.arrow.top,v.arrow.right-v.arrow.left};
                DrawTrayArrowHover(g,r);
                g.Restore(saved);
        }
        if(BetaTextureAvailable(0) && !IsRectEmpty(&v.desktop)) {
            auto ds=g.Save();BeginVerticalTile(g,v.desktop);
            RECT rotated{0,0,v.desktop.bottom-v.desktop.top,v.desktop.right-v.desktop.left};
            DrawBetaTexture(g,0,rotated);
            if(s->hot==HIT_DESKTOP) {
                SolidBrush overlay(Color(s->verticalPressed==HIT_DESKTOP?42:26,255,255,255));
                g.FillRectangle(&overlay,0,0,(int)rotated.right,(int)rotated.bottom);
            }
            g.Restore(ds);
        } else if(s->hot==HIT_DESKTOP) {
            SolidBrush b(Color(26,255,255,255));g.FillRectangle(&b,(int)v.desktop.left,(int)v.desktop.top,(int)(v.desktop.right-v.desktop.left),(int)(v.desktop.bottom-v.desktop.top));
        }
        float scale=Dpi()/96.f;
        if(!IsRectEmpty(&v.network))DrawNetworkGlyph(g,w/2.f,(v.network.top+v.network.bottom)/2.f,scale,s->netKind);
        if(!IsRectEmpty(&v.volume))DrawVolumeGlyph(g,w/2.f,(v.volume.top+v.volume.bottom)/2.f,scale,s->volume,s->muted);
        if(!IsRectEmpty(&v.battery))DrawBatteryGlyph(g,w/2.f,(v.battery.top+v.battery.bottom)/2.f,scale,s->batteryPct,s->batteryCharging);
        if(!win7Arrow && !IsRectEmpty(&v.arrow))DrawVerticalTrayArrow(g,v.arrow,s->verticalSide);
    }
    for(int index:VisiblePrivateIcons()) {RECT cell=PrivateIconRect(index);
        int size=Px(16);
        if(index>=0 && (size_t)index<s->trayIcons.size() && s->trayIcons[index].icon)DrawIconEx(dc,(cell.left+cell.right-size)/2,(cell.top+cell.bottom-size)/2,s->trayIcons[index].icon,size,size,0,nullptr,DI_NORMAL);
        }
    for(const auto& t:s->buttonTransitions)if(ValidGhost(t,w,h,true) && t.icon) {
        int step=t.rect.bottom-t.rect.top,size=std::max(1,std::min(Px(26),std::min(w,step)-Px(6)));
        RECT c=ButtonRevealRect(t.rect,1.f-ButtonTransitionFraction(t),true);ScopedSavedDC clip(dc);IntersectClipRect(dc,c.left,c.top,c.right,c.bottom);
        DrawIconEx(dc,(w-size)/2,t.rect.top+(step-size)/2,t.icon.get(),size,size,0,nullptr,DI_NORMAL);
    }
    for(int slot=0;slot<v.count;slot++) {int i=v.offset+slot;int size=std::max(1,std::min(Px(BetaTextureAvailable(11)?32:26),std::min(w,v.step)-Px(6)));
        ScopedSavedDC clip(dc);const auto* t=LiveButtonTransition(s->groups[i].key);
        if(t && t->reveal){RECT cell{0,v.first+slot*v.step,w,v.first+(slot+1)*v.step};
            RECT c=ButtonRevealRect(cell,ButtonTransitionFraction(*t),true);
            IntersectClipRect(dc,c.left,c.top,c.right,c.bottom);
            }
        DrawIconEx(dc,(w-size)/2,v.first+slot*v.step+(v.step-size)/2,GroupIcon(i),size,size,0,nullptr,DI_NORMAL);}
    HFONT font=CreateTaskbarFont(Dpi(),s->perPixelBar);ScopedSelection sel(dc,font);SetTextColor(dc,RGB(255,255,255));SetBkMode(dc,TRANSPARENT);
    if(!IsRectEmpty(&v.more)){RECT r=v.more;DrawTextW(dc,L"\u00bb",-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
    for(auto entry:{std::pair<RECT,const wchar_t*>{v.desktopBand,L"Desktop \u00bb"},
                    {v.linksBand,Tr(L"Link \u00bb",L"Links \u00bb")}}) {
        RECT r=entry.first;if(!IsRectEmpty(&r))DrawTextW(dc,entry.second,-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
    }
    wchar_t time[64]{},date[64]{};
        GetTimeFormatW(LOCALE_USER_DEFAULT,TIME_NOSECONDS,nullptr,nullptr,time,64);
        GetDateFormatW(LOCALE_USER_DEFAULT,DATE_SHORTDATE,nullptr,nullptr,date,64);
    std::wstring firstTime=time,timeMarker,firstDate=date,year;
    SIZE timeSize{},dateSize{};
    GetTextExtentPoint32W(dc,time,(int)wcslen(time),&timeSize);GetTextExtentPoint32W(dc,date,(int)wcslen(date),&dateSize);
    if(timeSize.cx>v.clock.right-v.clock.left-Px(4)) {
        size_t separator=firstTime.find_last_of(L" ");
        if(separator!=std::wstring::npos && separator>0 && separator+1<firstTime.size()) {
            timeMarker=firstTime.substr(separator+1);firstTime.resize(separator);
        }
    }
    if(dateSize.cx>v.clock.right-v.clock.left-Px(4)) {
        size_t separator=firstDate.find_last_of(L"/.-");
        if(separator!=std::wstring::npos && separator>0 && separator+1<firstDate.size()) {
            year=firstDate.substr(separator+1);firstDate.resize(separator);
        }
    }
    std::vector<std::wstring> clockLines{firstTime};
    if(!timeMarker.empty())clockLines.push_back(timeMarker);
    clockLines.push_back(firstDate);if(!year.empty())clockLines.push_back(year);
    int n=(int)clockLines.size(),stride=std::max(1,std::min(Px(17),(int)(v.clock.bottom-v.clock.top)/n));
    int top=v.clock.top+std::max(0,((int)(v.clock.bottom-v.clock.top)-n*stride)/2);
    {
        Graphics clockGraphics(dc);clockGraphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        FontFamily family(L"Segoe UI");Gdiplus::Font clockFont(&family,(REAL)(9.0*Dpi()/72.0),FontStyleRegular,UnitPixel);
        StringFormat format;format.SetAlignment(StringAlignmentCenter);format.SetLineAlignment(StringAlignmentCenter);
        format.SetFormatFlags(StringFormatFlagsNoWrap);format.SetTrimming(StringTrimmingEllipsisCharacter);
        SolidBrush white(Color(255,255,255,255));
        auto row=[&](const wchar_t* text,RECT rect) {
            if(family.GetLastStatus()==Ok && clockFont.GetLastStatus()==Ok) {
                RectF r((REAL)rect.left,(REAL)rect.top,(REAL)(rect.right-rect.left),(REAL)(rect.bottom-rect.top));
                clockGraphics.DrawString(text,-1,&clockFont,r,&format,&white);
            } else DrawTextW(dc,text,-1,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
        };
        for(const auto& text:clockLines) {RECT r=v.clock;r.top=top;r.bottom=top+stride;row(text.c_str(),r);top+=stride;}
        if(!IsRectEmpty(&v.language)) {
            int languageHeight=v.language.bottom-v.language.top;
            int languageStride=std::max(1,std::min(Px(17),languageHeight/2));
            int languageTop=v.language.top+std::max(0,(languageHeight-2*languageStride)/2);
            if(taskbarSkin.load()!=1 && !s->languageRegion.empty() && languageHeight>=2*Px(8)) {
                RECT first{v.language.left,languageTop,v.language.right,languageTop+languageStride};
                RECT second{v.language.left,languageTop+languageStride,v.language.right,languageTop+2*languageStride};
                row(s->languageText.c_str(),first);
                row(s->languageRegion.c_str(),second);
            } else {
                row(s->languageText.c_str(),v.language);
            }
        }
    }
    if(font){SelectObject(dc,sel.old);sel.old=nullptr;DeleteObject(font);}
}
// The screenshot has a one-pixel highlight and dark outer edge on the
// work-area side. Mirror it when the bar is at the right monitor edge.
static void PaintVerticalBackground(Graphics& g,int w,int h) {
    if(!s || w<2 || h<1)return;
    if(BetaTextureAvailable(11)) {
        // Preserve luminance while following the system hue.
        const BYTE reference[3]={56,165,231};
        SolidBrush background(taskbarSkin.load()==1?EffectiveLegacySkinTint():TintBetaReference(reference,EffectiveTaskbarColor()));g.FillRectangle(&background,0,0,w,h);
        if(taskbarSkin.load()==1)DrawWin7AeroTextures(g,w,h,true);else DrawSoftGlassTexture(g,w,h,true);
        SolidBrush border(Color(255,157,166,185));
        int edge=s->verticalSide==1?w-1:0;
        g.FillRectangle(&border,edge,0,1,h);
        int separator=Px(57);
        if(separator<h)g.FillRectangle(&border,0,separator,w,std::max(1,Px(1)));
        return;
    }
    PaintTaskbarBackground(g,w,h);
    int dark=s->verticalSide==1?w-1:0, light=s->verticalSide==1?w-2:1;
    SolidBrush lineLight(Color(115,255,255,255)),lineDark(Color(155,0,0,0));
    g.FillRectangle(&lineLight,light,0,1,h);g.FillRectangle(&lineDark,dark,0,1,h);
}
static void PaintVertical(HWND h) {
    s->buttonPaintTick=GetTickCount64();
    ScopedPaintDC paint(h);HDC dc=paint.dc;if(!dc)return;RECT r{};GetClientRect(h,&r);int w=r.right,ht=r.bottom;
    if(w<=0 || ht<=0 || ht>4096 || (size_t)w*(size_t)ht>8u*1024u*1024u)return;
    AdvanceHoverFade();
    bool rendered=false;
    if(s->perPixelBar) {
        AlphaSurface bg(dc,w,ht),black(dc,w,ht),white(dc,w,ht);
        if(bg.ok() && black.ok() && white.ok()) {
            {Graphics g(bg.dc);PaintVerticalBackground(g,w,ht);}PatBlt(black.dc,0,0,w,ht,BLACKNESS);PatBlt(white.dc,0,0,w,ht,WHITENESS);
            DrawVerticalForeground(black.dc,w,ht);DrawVerticalForeground(white.dc,w,ht);GdiFlush();
            ComposeForeground(bg.bits,black.bits,white.bits,(size_t)w*(size_t)ht,EffectiveBarBackgroundOpacity());
            RECT screen{};
                GetWindowRect(h,&screen);
                SIZE size{w,ht};
                POINT src{0,0},dst{screen.left,screen.top};
                BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
#ifdef TASKBAR81_TEST
            if(!testForceLayeredFailure)
#endif
            rendered=UpdateLayeredWindow(h,nullptr,&dst,&size,bg.dc,&src,0,&blend,ULW_ALPHA)!=FALSE;
            if(rendered)s->perPixelFrame=true;
        }
        if(!rendered)ConfigureBarAlpha(h,false);
    }
    if(!rendered) {
        std::unique_ptr<std::remove_pointer<HDC>::type,DeleteMemDC> mem(CreateCompatibleDC(dc));
        std::unique_ptr<std::remove_pointer<HBITMAP>::type,DeleteBitmapHandle> bmp(CreateCompatibleBitmap(dc,w,ht));ScopedSelection sel(mem.get(),bmp.get());
        if(mem && bmp && sel.old && sel.old!=HGDI_ERROR){ {Graphics g(mem.get());
            PaintVerticalBackground(g,w,ht);
            }DrawVerticalForeground(mem.get(),w,ht);
            BitBlt(dc,0,0,w,ht,mem.get(),0,0,SRCCOPY);
            }
    }
}
static bool VerticalInput(HWND h,UINT m,WPARAM wp,LPARAM lp,LRESULT& result) {
    auto hitAt=[&](){return VerticalHit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));};result=0;
    if(m==WM_MOUSEMOVE) {
        if(s->trayDrag.capture==h){MoveTrayDrag(h,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)});return true;}
        if(s->dragCandidate && (wp&MK_LBUTTON)) {int dx=GET_X_LPARAM(lp)-s->dragStart.x,dy=GET_Y_LPARAM(lp)-s->dragStart.y;
            if(allowDragReorder.load() && (std::abs(dx)>=GetSystemMetrics(SM_CXDRAG)/2 || std::abs(dy)>=GetSystemMetrics(SM_CYDRAG)/2))s->dragActive=true;
            if(s->dragActive){RECT r{};
                GetClientRect(h,&r);
                auto v=GetVerticalLayout(r.right,r.bottom);
                int pos=(GET_Y_LPARAM(lp)-v.first+v.step/2)/v.step;
                s->dragInsertion=v.offset+std::max(0,std::min(pos,v.count));
                KillTimer(h,TIMER_HOVER);
                HidePreview();
                InvalidateRect(h,nullptr,FALSE);
                return true;
                }}
        if(!s->tracking){TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,h,0};
            TrackMouseEvent(&t);
            s->tracking=true;
            }int hit=hitAt();
            if(BetaTextureAvailable(2) && hit>=0 && hit==s->hot)InvalidateRect(h,nullptr,FALSE);
            if(hit!=s->hot){s->hot=hit;
            s->hover=hit;
            KillTimer(h,TIMER_HOVER);
            HidePreview();
        if(hit>=0 || hit==HIT_START || hit==HIT_DESKTOP || hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY || hit==HIT_LANGUAGE || hit<=HIT_TRAY_ICON_BASE)SetTimer(h,TIMER_HOVER,450,nullptr);
        s->fadeTick=GetTickCount64();SetTimer(h,TIMER_FADE,16,nullptr);InvalidateRect(h,nullptr,FALSE);}return true;}
    if(m==WM_MOUSELEAVE){if((s->hot==HIT_START || s->hot==HIT_DESKTOP) && !s->hotCornerPreview)HidePreview();
        s->tracking=false;
        s->hot=HIT_NONE;
        s->hover=HIT_NONE;
        KillTimer(h,TIMER_HOVER);
        s->fadeTick=GetTickCount64();
        SetTimer(h,TIMER_FADE,16,nullptr);
        InvalidateRect(h,nullptr,FALSE);
        return true;
        }
    if(m==WM_LBUTTONDOWN || m==WM_LBUTTONDBLCLK){
        if(g_languageDismissTick && GetTickCount64()-g_languageDismissTick>200)g_languageDismissTick=0;
        s->verticalPressed=hitAt();int hit=s->verticalPressed;
        if(hit==HIT_START || hit==HIT_DESKTOP){KillTimer(h,TIMER_HOVER);HidePreview();}
        if(hit==HIT_NONE && !s->bandsLocked && s->privateActive && taskbarPlacementMode.load()==3) {
            POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};MONITORINFO mi{};mi.cbSize=sizeof(mi);
            if(ClientToScreen(h,&point) && GetMonitorInfoW(MonitorFromWindow(h,MONITOR_DEFAULTTONEAREST),&mi)) {
                s->barDragInitial=s->barEdgeOverride;s->barDragMoved=false;s->barDragStart=point;s->barDragMonitor=mi.rcMonitor;
                HMODULE module=nullptr;
                if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)BarDragHookProc,&module)) {
                    s->barDragCandidate=true;s->barHookEndQueued=false;s->barDragPending=0;s->barDragHook=SetWindowsHookExW(WH_MOUSE_LL,BarDragHookProc,module,0);
                    if(!s->barDragHook){Wh_Log(L"[T81-DIAG] vertical bar drag hook failed error=%lu",GetLastError());CancelBarEdgeDrag(false);}
                    else Wh_Log(L"[T81-DIAG] drag start x=%ld y=%ld mode=%d override=%d",point.x,point.y,taskbarPlacementMode.load(),s->barEdgeOverride);
                }return true;
            }
        }
        if(hit<=HIT_TRAY_ICON_BASE || hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY) {int index=hit<=HIT_TRAY_ICON_BASE?HIT_TRAY_ICON_BASE-hit:SystemEntry(hit);
            BeginTrayDrag(h,index,false,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)});
            return true;
            }
        if(hit>=0 && (size_t)hit<s->groups.size()){s->dragCandidate=true;
            s->dragActive=false;
            s->dragInsertion=-1;
            s->dragKey=s->groups[hit].key;
            s->dragStart=POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            }
        SetCapture(h);InvalidateRect(h,nullptr,FALSE);return true;}
    if(m==WM_LBUTTONUP) {
        if(s->barDragCandidate){CancelBarEdgeDrag(true);return true;}
        if(s->trayDrag.capture==h) {
            if(s->trayDrag.endQueued) {
                s->trayDrag.nativeUpSeen=true;
            } else {
                EndTrayDrag(h,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)});
            }
            s->verticalPressed=HIT_NONE;
            return true;
        }
        int hit=hitAt(),pressed=s->verticalPressed,insertion=s->dragInsertion;bool moved=s->dragActive;std::wstring key=s->dragKey;
        s->verticalPressed=HIT_NONE;
            s->dragCandidate=false;
            s->dragActive=false;
            s->dragInsertion=-1;
            s->dragKey.clear();
            if(GetCapture()==h)ReleaseCapture();
            InvalidateRect(h,nullptr,FALSE);
        if(moved){MoveGroupToInsertion(key,insertion);BuildGroups();InvalidateRect(h,nullptr,FALSE);return true;}
        if(hit!=pressed || hit==HIT_NONE || (hit>=0 && ((size_t)hit>=s->groups.size() || s->groups[hit].key!=key)))return true;
        if(hit==HIT_START){SendWin(0);return true;}if(hit==HIT_DESKTOP){ToggleDesktop();return true;}
        if(hit==HIT_LANGUAGE){if(!ConsumeLanguageDismissClick())ShowLanguageIndicatorMenu();return true;}
        if(hit==HIT_BAND_DESKTOP || hit==HIT_BAND_LINKS){ShowBandPopup(hit);return true;}
        if(hit==HIT_BAND_ADDRESS){UpdateAddressBand();
            if(s->addressHost && s->addressEdit){SetForegroundWindow(s->addressHost);
            SetFocus(s->addressEdit);
            }return true;
            }
        if(hit==HIT_OVERFLOW){RECT r{};
            GetClientRect(h,&r);
            auto v=GetVerticalLayout(r.right,r.bottom);
            s->verticalOffset=(v.offset+std::max(1,v.count))%(int)s->groups.size();
            InvalidateRect(h,nullptr,FALSE);
            return true;
            }
        if(hit<=HIT_CLOCK && hit>=HIT_TRAY_MORE) {POINT pt{};GetCursorPos(&pt);LaunchSystemTrayHit(hit,&pt);return true;}
        if(hit>=0 && (size_t)hit<s->groups.size()) {
            Group& g=s->groups[hit];
            if(g.tasks.empty() && g.pin>=0 && (size_t)g.pin<s->pins.size()) {
                ShellExecuteW(h,L"open",s->pins[g.pin].path.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
            } else if(g.tasks.size()==1) {
                ClickTask(GroupTask(hit).hwnd);
            } else if(!g.tasks.empty()) {
                ShowPreview(hit);
            }
        }return true;
    }
    if(m==WM_CANCELMODE || m==WM_CAPTURECHANGED){s->verticalPressed=HIT_NONE;InvalidateRect(h,nullptr,FALSE);return false;}
    if(m==WM_MOUSEWHEEL){POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};ScreenToClient(h,&p);int hit=VerticalHit(p.x,p.y);
        if(hit==HIT_VOLUME && volumeWheel.load())AdjustVolumeWheel(GET_WHEEL_DELTA_WPARAM(wp));
        else {RECT r{};
            GetClientRect(h,&r);
            auto v=GetVerticalLayout(r.right,r.bottom);
            int max=std::max(0,(int)s->groups.size()-v.count);
            s->verticalOffset=std::max(0,std::min(max,v.offset+(GET_WHEEL_DELTA_WPARAM(wp)<0?1:-1)));
            InvalidateRect(h,nullptr,FALSE);
            }return true;
            }
    if(m==WM_RBUTTONDOWN){int hit=hitAt();
        if(hit<=HIT_TRAY_ICON_BASE){POINT pt{};
        GetCursorPos(&pt);
        SendPrivateIconEvent(HIT_TRAY_ICON_BASE-hit,m,pt);
        }return true;
        }
    if(m==WM_RBUTTONUP){int hit=hitAt();POINT pt{};GetCursorPos(&pt);
        if(hit<=HIT_TRAY_ICON_BASE){HandlePrivateIconEvent(hit,m);return true;}
        if(hit==HIT_START)SendWin('X');else if(hit==HIT_LANGUAGE)ShowLanguageIndicatorMenu();else if(hit==HIT_CLOCK)ShowClockContextMenu();
        else if(hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY)ShowSystemTrayContextMenu(hit,pt);
        else if(hit>=0 && (size_t)hit<s->groups.size()) {if(!ShowJumpList((size_t)hit,pt) && !s->groups[hit].tasks.empty())w7t::ShellMenu::ShowWindowSystemMenu(GroupTask(hit).hwnd,pt.x,pt.y,true);}
        else ShowBarContextMenu(false);return true;}
    if(m==WM_MBUTTONDOWN || m==WM_MBUTTONUP)return true;
    return false;
}

LRESULT BarProcImpl(HWND h,UINT m,WPARAM wp,LPARAM lp) {
    if(s && m==WM_LBUTTONDOWN)s->suppressTrayRelease=false;
    if(s && m==WM_LBUTTONUP && s->suppressTrayRelease && !s->trayDrag.capture) {
        bool consume=GetTickCount64()<s->suppressTrayReleaseUntil;s->suppressTrayRelease=false;if(consume)return 0;
    }
    if(s && s->shellHookMessage && m==s->shellHookMessage) {
        try {
            UINT code=(UINT)(wp&0x7fff);
            if(code==HSHELL_LANGUAGE) {
                if(s->privateActive)RefreshLanguageIndicator();
                SetTimer(h,TIMER_LANGUAGE_REFRESH,75,nullptr);
            } else {
                HandleAttentionEvent((UINT_PTR)wp,(HWND)lp);
                if(s->privateActive && (code==HSHELL_WINDOWACTIVATED || code==HSHELL_RUDEAPPACTIVATED))
                    RefreshLanguageIndicator();
            }
        } catch(...) {Wh_Log(L"[T81-DIAG] shell input/attention event exception");}
        return 0;
    }
    if(s && s->verticalActive) {LRESULT result=0;if(VerticalInput(h,m,wp,lp,result))return result;}
    switch(m) {
    case WM_SIZE: BuildGroups();InvalidateRect(h,nullptr,FALSE);return 0;
    case WM_CREATE:
        SetTimer(h,TIMER_REFRESH,1000,nullptr);
        SetTimer(h,TIMER_Z_GUARD,100,nullptr);
        return 0;
    case WM_T81_CNI_REQUEST: {
        auto* request = (CniUiRequest*)lp;
        HRESULT hr = ProcessCniUiRequest(request);
        if (wp) {
            auto* sync = (CniUiSyncResult*)wp;
            sync->hr = hr;
            sync->completed.store(true, std::memory_order_release);
        }
        delete request;
        if (FAILED(hr))
            Wh_Log(L"[T81-CNI] UI request failed hr=0x%08lx", (unsigned long)hr);
        return 0;
    }
    case WM_T81_APPBAR:
        if(s->appbarRegistered && wp==ABN_POSCHANGED) {s->appbarDirty=true;SetTimer(h,TIMER_APPBAR_NOTIFY,250,nullptr);}
        else if(s->appbarRegistered && wp==ABN_FULLSCREENAPP) {s->appbarFullScreen=lp!=0;UpdatePlacement();}
        else if(s->appbarRegistered && wp==ABN_WINDOWARRANGE) {s->appbarArranging=lp!=0;UpdatePlacement();}
        return 0;
    case WM_TIMER:
        if(wp==TIMER_LANGUAGE_REFRESH) {KillTimer(h,TIMER_LANGUAGE_REFRESH);
            if(s->privateActive){RefreshLanguageIndicator();
            ConfirmLanguageFlyoutRequest();
            }return 0;
            }
        if(wp==TIMER_ATTENTION) {PruneAttention();if(!s->attention.empty())s->attentionOn=!s->attentionOn;InvalidateRect(h,nullptr,FALSE);return 0;}
        if(wp==TIMER_NETWORK_POSITION) {
            HWND f=FindOwnNetworkFlyout();
            if(f && !IsRectEmpty(&s->networkAnchor) && PlaceFlyoutAtVisualBar(f,s->networkAnchor)) {KillTimer(h,TIMER_NETWORK_POSITION);s->networkPositionAttempts=0;}
            else if(--s->networkPositionAttempts<=0) {KillTimer(h,TIMER_NETWORK_POSITION);Wh_Log(L"[T81-DIAG] win7 network request not observed visible");}return 0;
        }
        if(wp==TIMER_VOLUME_POSITION) {
            FindVolumePopupContext c{s->volumeLaunchPid,nullptr};if(c.pid)EnumWindows(FindVolumePopupProc,(LPARAM)&c);
            if(c.found && FlyoutAtVisualTarget(c.found,s->volumeAnchor)) {
                KillTimer(h,TIMER_VOLUME_POSITION);s->volumePositionAttempts=0;
                Wh_Log(L"[T81-DIAG] volume popup target rectangle observed");
            } else {
                if(c.found) PlaceFlyoutAtVisualBar(c.found,s->volumeAnchor);
                if(--s->volumePositionAttempts<=0) {KillTimer(h,TIMER_VOLUME_POSITION);Wh_Log(L"[T81-DIAG] volume popup target not observed in launched PID");}
            }
            return 0;
        }
        if(wp==TIMER_ACTION_POSITION) {
            HWND f=FindOwnActionFlyout();
            if(f && s->verticalActive && FlyoutAtVisualTarget(f,s->actionAnchor)) {
                KillTimer(h,TIMER_ACTION_POSITION);s->actionPositionAttempts=0;
                Wh_Log(L"[T81-DIAG] action center companion target rectangle observed");
            } else if(--s->actionPositionAttempts<=0 || !s->verticalActive) {
                KillTimer(h,TIMER_ACTION_POSITION);Wh_Log(L"[T81-DIAG] action center companion position not confirmed");
            } else if(f)PlaceFlyoutAtVisualBar(f,s->actionAnchor);
            return 0;
        }
        if(wp==TIMER_APPBAR_NOTIFY) {KillTimer(h,TIMER_APPBAR_NOTIFY);UpdatePlacement();return 0;}
        if(wp==TIMER_APPBAR_VERIFY) {
            KillTimer(h,TIMER_APPBAR_VERIFY);
            if(s->appbarRegistered) {
                MONITORINFO mi{};mi.cbSize=sizeof(mi);RECT r=s->appbarApproved;
                bool known=GetMonitorInfoW(MonitorFromRect(&r,MONITOR_DEFAULTTONEAREST),&mi)!=FALSE;
                bool reserved=known && AppbarWorkAreaObserved(s->appbarEdge,r,mi.rcWork);
                Wh_Log(L"[T81-DIAG] work-area AppBar work-area observed=%d known=%d",reserved?1:0,known?1:0);
                if(known && !reserved) {RemoveWorkAreaAppBar();s->appbarRetryAt=GetTickCount64()+30000;UpdatePlacement();}
            }return 0;
        }
        if(wp==TIMER_CLASSIC_POSITION_NOTIFY) {
            KillTimer(h,TIMER_CLASSIC_POSITION_NOTIFY);
            if(s->privateActive && s->native && IsWindow(s->native)) {DWORD_PTR result=0;
                if(!SendMessageTimeoutW(s->native,WM_SETTINGCHANGE,SPI_SETLOGICALDPIOVERRIDE,0,SMTO_ABORTIFHUNG|SMTO_BLOCK,150,&result))
                    Wh_Log(L"[T81-DIAG] deferred classic native setting message timed out/failed error=%lu",GetLastError());
            }return 0;
        }
        if(wp==TIMER_RESOURCE_RELOAD) {KillTimer(h,TIMER_RESOURCE_RELOAD);LoadLegacyImages();return 0;}
        if(wp==TIMER_BATTERY_SLIDE) {TickBatterySlide();return 0;}
        if(wp==TIMER_ACTIVATE_RETRY) {TickActivateRetry();return 0;}
        if(wp==TIMER_DRAG_ACTIVATE) {TickDragActivate();return 0;}
        if(wp==TIMER_Z_GUARD) {
            if(s->barDragCandidate && (!s->privateActive || s->bandsLocked ||
               (!s->barHookEndQueued && !(GetAsyncKeyState(VK_LBUTTON)&0x8000)))) CancelBarEdgeDrag(true);
            GuardPrivateTray(); GuardOverlayZOrder(); TickTrayDrag(); DismissOverflowOnOutsideClick(); }
        if(wp==TIMER_BUTTON_TRANSITION) {TickButtonTransitions();return 0;}
        if(wp==TIMER_ANIMATION || wp==TIMER_FADE) InvalidateRect(h,nullptr,FALSE);
        if(wp==TIMER_REFRESH) {
            // Covers Start opened externally, including in another Explorer process.
            if(s->peekOn && CompanionStartScreenVisible()) PeekOff(false);
            if(s->ticks%5==0) RefreshNativeLabelPreference();
            if(s->privateActive) {
                RefreshLanguageIndicator();
                RefreshVolume(); RefreshBattery();
                if(s->ticks%5==0) {
                    int oldKind=s->netKind;
                    s->netKind=QueryNetworkKind();
                    if(oldKind!=s->netKind) PublishCniSystemIcon(HIT_NETWORK);
                }
                if(s->nativeSnapshotActive && s->ticks && s->ticks%10==0) ImportNativeTraySnapshot();
                // Remove icons whose owner was destroyed or whose handle was reused.
                for(size_t i=s->trayIcons.size();i>0;--i) {
                    auto& icon=s->trayIcons[i-1]; DWORD pid=0;
                    if(!IsWindow(icon.owner) || !GetWindowThreadProcessId(icon.owner,&pid) || pid!=icon.ownerPid) {
                        if(icon.icon) DestroyIcon(icon.icon);
                        s->trayIcons.erase(s->trayIcons.begin()+i-1);
                    }
                }
            }
            if(s->peekOn && !s->peekDesktop && (!s->peekTarget || !IsWindow(s->peekTarget))) PeekOff(false);
            UpdatePlacement(); Refresh();
        }
        if(wp==TIMER_DESKTOP_PEEK) { KillTimer(h,TIMER_DESKTOP_PEEK);
             POINT p{};
             if(s->hot==HIT_DESKTOP && GetCursorPos(&p) && ScreenToClient(h,&p) && Hit(p.x,p.y)==HIT_DESKTOP) { PeekDesktop();
             Wh_Log(L"[T81-DIAG] desktop Peek active=%d",s->peekDesktop?1:0);
             } }
        if(wp==TIMER_HOVER) { KillTimer(h,TIMER_HOVER);
             if(s->trayDrag.capture)return 0;
             if(s->hover>=0) ShowPreview(s->hover);
             else if(s->hover==HIT_START && startHotCorner.load()) ShowHotCornerPreview();
             else if(s->hover==HIT_START || s->hover==HIT_DESKTOP) ShowBarButtonTooltip(s->hover);
             else if(s->hover==HIT_NETWORK || s->hover==HIT_VOLUME || s->hover==HIT_BATTERY) ShowSystemTooltip(s->hover);
             else if(s->hover==HIT_LANGUAGE) ShowLanguageTooltip();
             else if(s->hover<=HIT_TRAY_ICON_BASE) ShowPrivateIconTooltip(s->hover);
             }
        if(wp==TIMER_CLOCK_FIX) {
            if(AttachClassicClockFrame()) { KillTimer(h,TIMER_CLOCK_FIX); s->clockFixAttempts=0; }
            else if(--s->clockFixAttempts<=0) {
                KillTimer(h,TIMER_CLOCK_FIX);
                Wh_Log(L"Clock request accepted but no visible ClockFlyoutWindow found in this Explorer process");
            }
        }
        if(wp==TIMER_PREVIEW) {
            if(s->recoveryTooltipUntil) {
                if(GetTickCount64()>=s->recoveryTooltipUntil)HidePreview();
                return 0;
            }
            HWND popup=(s->preview && IsWindowVisible(s->preview))?s->preview:
                       ((s->tooltip && IsWindowVisible(s->tooltip))?s->tooltip:nullptr);
            if(popup) {
                POINT p{}; RECT a{},b{}; GetCursorPos(&p); GetWindowRect(h,&a); GetWindowRect(popup,&b);
                bool inBar=PtInRect(&a,p);
                if(s->hotCornerPreview && inBar) {POINT q=p;ScreenToClient(h,&q);inBar=(s->verticalActive?VerticalHit(q.x,q.y):Hit(q.x,q.y))==HIT_START;}
                if(!inBar && !PtInRect(&b,p)) {
                    KillTimer(h,TIMER_PREVIEW);
                    if(popup==s->preview) {PeekOff(false);StartPreviewAnimation(true);} else HidePreview();
                }
            } else KillTimer(h,TIMER_PREVIEW);
        }
        return 0;
    case WM_T81_CNI_ICON:
        // The native hidden shell may send NIN callbacks for our registrations.
        // Visible input is already handled by the synthetic private-tray cells.
        return 0;
    case WM_INPUTLANGCHANGE:
        if(s->privateActive) {RefreshLanguageIndicator();SetTimer(h,TIMER_LANGUAGE_REFRESH,75,nullptr);}
        return DefWindowProcW(h,m,wp,lp);
    case WM_POWERBROADCAST: if(wp==PBT_APMPOWERSTATUSCHANGE) RefreshBattery(); return TRUE;
    case WM_RESOURCES_READY: if(!SetTimer(h,TIMER_RESOURCE_RELOAD,120,nullptr))LoadLegacyImages();return 0;
    case WM_WIFI_CHANGED: InvalidateRect(h,nullptr,FALSE);if(s->overflowPanel)InvalidateRect(s->overflowPanel,nullptr,FALSE);return 0;
    case WM_UPDATE_SETTINGS: CloseJumpPopup();
         if(s->overflowPanel){HidePrivateOverflow();
        ConfigureSystemPopupFrame(s->overflowPanel);
        ResizePrivateOverflow();
        } BuildGroups();
         if(!s->resourceDir.empty()) LoadLegacyImages();
         if(s->jumpPopup) InvalidateRect(s->jumpPopup,nullptr,FALSE);
         if(s->overflowPanel) InvalidateRect(s->overflowPanel,nullptr,FALSE);
         HidePreview();
         s->clockMetricsDpi=0;
        s->languageMetricsDpi=0;
        if(s->privateActive)RefreshLanguageIndicator();
         if(!aeroPeek.load()) PeekOff(false);
         ConfigureBarAlpha(h,perPixelTaskbar.load());
         UpdatePlacement();
         InvalidateRect(h,nullptr,FALSE);
         return 0;
    case WM_DWMCOMPOSITIONCHANGED: InvalidateRect(h,nullptr,FALSE);return 0;
    case WM_DWMCOLORIZATIONCOLORCHANGED: if(s->jumpPopup) InvalidateRect(s->jumpPopup,nullptr,FALSE);
         if(s->overflowPanel) InvalidateRect(s->overflowPanel,nullptr,FALSE);
         InvalidateRect(h,nullptr,FALSE);
         return 0;
    case WM_DISPLAYCHANGE: case WM_SETTINGCHANGE: case WM_DPICHANGED:
        s->buttonTransitions.clear();KillTimer(h,TIMER_BUTTON_TRANSITION);
        if(m!=WM_SETTINGCHANGE || taskbarPlacementMode.load()!=3)CancelBarEdgeDrag(true);
        if(s->appbarRegistered)s->appbarDirty=true;
        if(m==WM_DPICHANGED && !s->resourceDir.empty()) LoadLegacyImages();
        s->clockMetricsDpi=0; s->clockWidthPx=0;s->languageMetricsDpi=0;s->languageWidthPx=0;
        s->rectValid=false; RefreshNativeLabelPreference(); UpdatePlacement();
        if(s->privateActive)RefreshLanguageIndicator();
        if(s->overflowPanel && IsWindowVisible(s->overflowPanel)) ResizePrivateOverflow();
        InvalidateRect(h,nullptr,FALSE); return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: if(s->verticalActive) {HideWin7OrbSurface();PaintVertical(h);} else Paint(h); return 0;
    case WM_MOUSEMOVE: {
        if(s->bandDragHit && GetCapture()==h) return 0;
        if(s->trayDrag.capture==h) { POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)}; MoveTrayDrag(h,p); return 0; }
        if(s->dragCandidate && (wp & MK_LBUTTON)) {
            int dx=GET_X_LPARAM(lp)-s->dragStart.x, dy=GET_Y_LPARAM(lp)-s->dragStart.y;
            // An upward pull opens the Jump List.
            // Decide before horizontal reorder starts; do not install a hook.
            BarEdge edge=s->nativeEdge==ABE_TOP?BarEdge::Top:s->nativeEdge==ABE_LEFT?BarEdge::Left:s->nativeEdge==ABE_RIGHT?BarEdge::Right:BarEdge::Bottom;
            EdgeDelta gesture=DragRelativeToEdge(edge,dx,dy);
            if(!s->dragActive && gesture.away>=Px(16) && std::abs(gesture.along)<Px(30)) {
                size_t index=s->groups.size();
                for(size_t i=0;i<s->groups.size();++i) if(s->groups[i].key==s->dragKey) {index=i;break;}
                s->dragCandidate=false; s->dragActive=false; s->dragInsertion=-1; s->dragKey.clear();
                if(GetCapture()==h) ReleaseCapture(); KillTimer(h,TIMER_HOVER); HidePreview();
                POINT cursor{}; GetCursorPos(&cursor); if(index<s->groups.size()) ShowJumpList(index,cursor);
                return 0;
            }
            // Small vertical movements remain candidates for the upward pull.
            if(!s->dragActive && gesture.away>0 && gesture.away>std::abs(gesture.along)) return 0;
            if(allowDragReorder.load() && !s->dragActive && (std::abs(dx)>=GetSystemMetrics(SM_CXDRAG)/2 ||
                                   std::abs(dy)>=GetSystemMetrics(SM_CYDRAG)/2)) {
                s->dragActive=true;
                KillTimer(h,TIMER_HOVER);
                HidePreview();
            }
            if(s->dragActive) {
                RECT client{}; GetClientRect(h,&client);
                Layout layout=GetLayout(client.right,client.bottom);
                int insertion=(GET_X_LPARAM(lp)-layout.first+layout.width/2)/layout.width;
                s->dragInsertion=std::max(0,std::min(insertion,layout.count));
                s->dragCursorX=GET_X_LPARAM(lp);
                InvalidateRect(h,nullptr,FALSE);
                return 0;
            }
        }
        POINT cursorForTrack{};RECT orbForTrack{};
        bool onOrb=gWin7OrbReady && gWin7OrbWindow && GetCursorPos(&cursorForTrack) &&
                   GetWindowRect(gWin7OrbWindow,&orbForTrack) && PtInRect(&orbForTrack,cursorForTrack);
        if(!s->tracking && !onOrb) { TRACKMOUSEEVENT t{sizeof(t),TME_LEAVE,h,0}; TrackMouseEvent(&t); s->tracking=true; }
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        if(BetaTextureAvailable(2) && hit>=0 && hit==s->hot)InvalidateRect(h,nullptr,FALSE);
        RECT barClient{}; GetClientRect(h,&barClient); Layout hoverLayout=GetLayout(barClient.right,barClient.bottom);
        POINT mouse{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        RECT desktopArrow=BandArrowRect(hoverLayout.desktopBand),linksArrow=BandArrowRect(hoverLayout.linksBand);
        bool arrowHot=hit==HIT_BAND_DESKTOP && PtInRect(&desktopArrow,mouse);
        bool linksHot=hit==HIT_BAND_LINKS && PtInRect(&linksArrow,mouse);
        if(arrowHot!=s->desktopArrowHot || linksHot!=s->linksArrowHot) {s->desktopArrowHot=arrowHot;
            s->linksArrowHot=linksHot;
            InvalidateRect(h,nullptr,FALSE);
            }
        if(hit!=s->hot) {
            s->hot=hit;
            s->hover=hit;
            s->fadeTick=GetTickCount64(); SetTimer(h,TIMER_FADE,16,nullptr);
            KillTimer(h,TIMER_HOVER); KillTimer(h,TIMER_DESKTOP_PEEK); PeekOff(true);
            if(hit==HIT_DESKTOP && s->desktopPeekEnabled) { DWORD delay=DesktopPeekDelay(); SetTimer(h,TIMER_DESKTOP_PEEK,std::max<DWORD>(1,delay),nullptr); }
            if(hit>=0) { HidePreview(); if(!(s->jumpPopup && IsWindow(s->jumpPopup))) SetTimer(h,TIMER_HOVER,450,nullptr); }
            else { HidePreview(); if(hit==HIT_START || hit==HIT_DESKTOP || hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY || hit==HIT_LANGUAGE || hit<=HIT_TRAY_ICON_BASE) SetTimer(h,TIMER_HOVER,450,nullptr); }
            InvalidateRect(h,nullptr,FALSE);
        } return 0;
    }
    case WM_MOUSELEAVE:
        if(gWin7OrbReady && gWin7OrbWindow) {POINT cursor{};RECT orb{};
            if(GetCursorPos(&cursor) && GetWindowRect(gWin7OrbWindow,&orb) && PtInRect(&orb,cursor)) {s->tracking=false;return 0;}}
        if((s->hot==HIT_START || s->hot==HIT_DESKTOP) && !s->hotCornerPreview)HidePreview();
        s->tracking=false; s->hot=HIT_NONE; s->hover=HIT_NONE; s->desktopArrowHot=false; s->linksArrowHot=false;
        s->fadeTick=GetTickCount64(); SetTimer(h,TIMER_FADE,16,nullptr);
        KillTimer(h,TIMER_HOVER); KillTimer(h,TIMER_DESKTOP_PEEK); PeekOff(true); InvalidateRect(h,nullptr,FALSE); return 0;
    case WM_LBUTTONDBLCLK: {
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        if(hit>=0 && (size_t)hit<s->groups.size() && s->jumpLeftDismissKey==s->groups[hit].key && GetTickCount64()-s->jumpLeftDismissAt<=GetDoubleClickTime()) {
            s->jumpLeftPressedKey=s->jumpLeftDismissKey;s->jumpLeftDismissKey.clear();return 0;
        }
        // Preserve normal task-button behaviour on a double click.
        [[fallthrough]];
    }
    case WM_LBUTTONDOWN: {
        if(g_languageDismissTick && GetTickCount64()-g_languageDismissTick>200)g_languageDismissTick=0;
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        s->jumpLeftPressedKey.clear();
        if(hit>=0 && (size_t)hit<s->groups.size()) {
            const auto& key=s->groups[hit].key;
            if((s->jumpPopup && IsWindow(s->jumpPopup) && s->jumpGroupKey==key) || (s->jumpLeftDismissKey==key && GetTickCount64()-s->jumpLeftDismissAt<500)) {
                s->jumpLeftPressedKey=key;s->jumpLeftDismissKey=key;s->jumpLeftDismissAt=GetTickCount64();CloseJumpPopup(true);return 0;
            }
        }
        if(!s->bandsLocked && s->privateActive && hit==HIT_NONE) {
            RECT r{};GetClientRect(h,&r);Layout layout=GetLayout(r.right,r.bottom);
            POINT local{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)},screen=local;
            MONITORINFO mi{};mi.cbSize=sizeof(mi);
            if(!PtInRect(&layout.tray,local) && ClientToScreen(h,&screen) && GetMonitorInfoW(MonitorFromWindow(h,MONITOR_DEFAULTTONEAREST),&mi)) {
                s->barDragInitial=s->barEdgeOverride;s->barDragMoved=false;s->barDragStart=screen;s->barDragMonitor=mi.rcMonitor;
                HMODULE module=nullptr;
                if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)BarDragHookProc,&module)) {
                    s->barDragCandidate=true;s->barHookEndQueued=false;s->barDragPending=0;
                    s->barDragHook=SetWindowsHookExW(WH_MOUSE_LL,BarDragHookProc,module,0);
                    if(!s->barDragHook) {Wh_Log(L"[T81-DIAG] bar drag hook failed error=%lu",GetLastError());CancelBarEdgeDrag(false);}
                    else Wh_Log(L"[T81-DIAG] drag start x=%ld y=%ld mode=%d override=%d",screen.x,screen.y,taskbarPlacementMode.load(),s->barEdgeOverride);
                }
                return 0;
            }
        }
        if(!s->bandsLocked && (hit==HIT_BAND_DESKTOP || hit==HIT_BAND_LINKS || hit==HIT_BAND_ADDRESS)) {
            RECT r{};GetClientRect(h,&r);Layout l=GetLayout(r.right,r.bottom);
            RECT band=hit==HIT_BAND_DESKTOP?l.desktopBand:hit==HIT_BAND_LINKS?l.linksBand:l.addressBand;
            if(GET_X_LPARAM(lp)<band.left+Px(7)) {s->bandDragHit=hit;s->bandDragStart={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};SetCapture(h);return 0;}
        }
        if(hit<=HIT_TRAY_ICON_BASE) { BeginTrayDrag(h,HIT_TRAY_ICON_BASE-hit,false,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)}); return 0; }
        if(hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY) {
            BeginTrayDrag(h,SystemEntry(hit),false,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)}); return 0;
        }
        if(hit==HIT_START || hit==HIT_DESKTOP){KillTimer(h,TIMER_HOVER);HidePreview();}
        if(hit==HIT_TRAY_MORE && taskbarSkin.load()==1)InvalidateRect(h,nullptr,FALSE);
        if(hit==HIT_DESKTOP && BetaTextureAvailable(0)) {
            s->betaDesktopPressed=true;SetCapture(h);InvalidateRect(h,nullptr,FALSE);
        } else if(hit==HIT_START) {
            s->startPressed=true;
            SetCapture(h);
            InvalidateRect(h,nullptr,FALSE);
        } else if(hit>=0 && (size_t)hit<s->groups.size()) {
            s->dragCandidate=true;
            s->dragActive=false;
            s->dragKey=s->groups[hit].key;
            s->dragStart={GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
            RECT r{}; GetClientRect(h,&r); Layout layout=GetLayout(r.right,r.bottom);
            s->dragCursorX=s->dragStart.x;
            s->dragGrabOffset=s->dragStart.x-(layout.first+hit*layout.width);
            s->dragInsertion=-1;
            SetCapture(h);InvalidateRect(h,nullptr,FALSE);
        }
        return 0;
    }
    case WM_CANCELMODE:
        CancelBarEdgeDrag(true);s->bandDragHit=0;
        if(s->trayDrag.capture==h) CancelTrayDrag();
        if(GetCapture()==h) ReleaseCapture();
        s->startPressed=false;s->betaDesktopPressed=false;
        s->dragCandidate=false; s->dragActive=false; s->dragInsertion=-1; s->dragKey.clear();
        InvalidateRect(h,nullptr,FALSE);
        return 0;
    case WM_CAPTURECHANGED:
        CancelBarEdgeDrag(true);s->bandDragHit=0;
        if(s->trayDrag.capture==h) CancelTrayDrag();
        s->startPressed=false;s->betaDesktopPressed=false;
        s->dragCandidate=false; s->dragActive=false; s->dragInsertion=-1; s->dragKey.clear();
        InvalidateRect(h,nullptr,FALSE);
        return 0;
    case WM_TRAY_DRAG_MOVE:
        if(s->trayDrag.capture) {
            s->trayDrag.moveQueued=false;
            if(!s->trayDrag.endQueued) {POINT p{};if(!GetCursorPos(&p))p=POINT{(LONG)(INT_PTR)wp,(LONG)lp};
                if(ScreenToClient(s->trayDrag.capture,&p))MoveTrayDrag(s->trayDrag.capture,p);}
        }return 0;
    case WM_TRAY_DRAG_END:
        if(s->trayDrag.capture) {HWND capture=s->trayDrag.capture;POINT p{(LONG)(INT_PTR)wp,(LONG)lp};
            if(ScreenToClient(capture,&p)){MoveTrayDrag(capture,p);EndTrayDrag(capture,p);}else CancelTrayDrag();}
        return 0;
    case WM_BAR_DRAG_POINT:
        s->barMoveQueued=false;
        if(s->barDragCandidate && !s->barHookEndQueued) {
            POINT current{};
            if(GetCursorPos(&current))UpdateBarEdgeDrag(h,current);
            else UpdateBarEdgeDrag(h,POINT{(LONG)(INT_PTR)wp,(LONG)lp});
        }return 0;
    case WM_BAR_DRAG_END:
        if(s->barDragCandidate) {
            POINT released{(LONG)(INT_PTR)wp,(LONG)lp};UpdateBarEdgeDrag(h,released);
            if(!s->barDragCandidate)return 0; // a genuine native move cancelled the gesture
            int pending=s->barDragPending;RECT monitor=s->barDragMonitor;
            bool moved=s->barDragMoved,inside=PtInRect(&monitor,released)!=FALSE;
            if(moved)SaveBarEdgePreference();
            CancelBarEdgeDrag(false);UpdatePlacement();
            Wh_Log(L"[T81-DIAG] drag release x=%ld y=%ld monitor=%ld,%ld,%ld,%ld inside=%d choice=%d changed=%d",released.x,released.y,monitor.left,monitor.top,monitor.right,monitor.bottom,inside?1:0,pending,moved?1:0);
            RECT bar{};MONITORINFO mi{};mi.cbSize=sizeof(mi);
            if(GetWindowRect(h,&bar) && GetMonitorInfoW(MonitorFromRect(&bar,MONITOR_DEFAULTTONEAREST),&mi))
                Wh_Log(L"[T81-DIAG] placement committed visual=%ls override=%d native-edge=%d workarea-option=%d registered=%d rect=%ld,%ld,%ld,%ld work=%ld,%ld,%ld,%ld",s->verticalActive?(s->verticalSide==1?L"left":L"right"):(s->nativeEdge==ABE_TOP?L"top":L"bottom"),s->barEdgeOverride,s->sourceNativeEdge,reserveWorkArea.load()?1:0,s->appbarRegistered?1:0,bar.left,bar.top,bar.right,bar.bottom,mi.rcWork.left,mi.rcWork.top,mi.rcWork.right,mi.rcWork.bottom);
        }return 0;
    case WM_LBUTTONUP: {
        if(s->barDragCandidate) {CancelBarEdgeDrag(true);return 0;} // hook could not queue release
        if(s->bandDragHit && GetCapture()==h) {
            int from=s->bandDragHit,to=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
            s->bandDragHit=0;ReleaseCapture();
            auto id=[](int hit){return hit==HIT_BAND_DESKTOP?0:hit==HIT_BAND_LINKS?1:hit==HIT_BAND_ADDRESS?2:-1;};
            int a=id(from),b=id(to);if(a>=0 && b>=0 && a!=b) {
                int ia=-1,ib=-1;for(int i=0;i<3;i++){if(s->bandOrder[i]==a)ia=i;if(s->bandOrder[i]==b)ib=i;}
                if(ia>=0 && ib>=0) {std::swap(s->bandOrder[ia],s->bandOrder[ib]);
                    wchar_t value[4]={(wchar_t)(L'0'+s->bandOrder[0]),(wchar_t)(L'0'+s->bandOrder[1]),(wchar_t)(L'0'+s->bandOrder[2]),0};
                    Wh_SetStringValue(L"Taskbar81.BandOrder",value);
                    UpdateAddressBand();
                    InvalidateRect(h,nullptr,FALSE);
                    }
            } return 0;
        }
        if(!s->jumpLeftPressedKey.empty()) {s->jumpLeftPressedKey.clear();return 0;}
        if(s->trayDrag.capture==h) {
            if(s->trayDrag.endQueued) {
                s->trayDrag.nativeUpSeen=true;
            } else {
                EndTrayDrag(h,POINT{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)});
            }
            return 0;
        }
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        bool startClick=s->startPressed && hit==HIT_START;
        bool didDrag=s->dragActive;
        int insertion=s->dragInsertion;
        std::wstring dragged=s->dragKey;
        bool hadCapture=s->startPressed || s->dragCandidate || s->betaDesktopPressed;
        s->betaDesktopPressed=false;
        s->dragCandidate=false; s->dragActive=false; s->dragInsertion=-1; s->dragKey.clear();
        if(hadCapture && GetCapture()==h) ReleaseCapture();
        if(didDrag) {
            MoveGroupToInsertion(dragged,insertion);
            BuildGroups();
            InvalidateRect(h,nullptr,FALSE);
            return 0;
        }
        s->startPressed=false;
        if(hadCapture) InvalidateRect(h,nullptr,FALSE);
        if(hit<=HIT_TRAY_ICON_BASE) return 0;
        if(hit==HIT_BAND_ADDRESS) { UpdateAddressBand();
             if(s->addressHost && s->addressEdit) {SetForegroundWindow(s->addressHost);
            SetFocus(s->addressEdit);
            } return 0;
             }
        if(hit==HIT_BAND_LINKS || hit==HIT_BAND_DESKTOP) { ShowBandPopup(hit); return 0; }
        if(hit==HIT_LANGUAGE) { if(!ConsumeLanguageDismissClick())ShowLanguageIndicatorMenu(); return 0; }
        if(hit<=HIT_CLOCK && hit>=HIT_TRAY_MORE) { LaunchSystemTrayHit(hit); if(hit==HIT_TRAY_MORE)InvalidateRect(h,nullptr,FALSE); return 0; }
        if(startClick) SendWin(0); else if(hit==HIT_DESKTOP) {KillTimer(h,TIMER_DESKTOP_PEEK);PeekOff(true);ToggleDesktop();}
        else if(hit==HIT_OVERFLOW) ShowOverflow();
        else if(hit>=0 && (size_t)hit<s->groups.size()) {
            if(GetKeyState(VK_SHIFT)&0x8000) { LaunchGroupInstance((size_t)hit); return 0; }
            Group& g=s->groups[hit];
            if(g.tasks.empty() && g.pin>=0 && (size_t)g.pin<s->pins.size())
                ShellExecuteW(h,L"open",s->pins[g.pin].path.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
            else if(g.tasks.size()==1) ClickTask(GroupTask(hit).hwnd);
            else if(!g.tasks.empty()) ShowPreview(hit);
        }
        return 0;
    }
#ifdef TASKBAR81_TEST
    case WM_APP+945: testButtonAnimationOverride=(int)wp;return 0;
    case WM_APP+946: {Refresh();int flags=0;for(const auto& t:s->buttonTransitions)flags|=t.ghost?2:t.reveal?1:4;return flags;}
    case WM_APP+947: {for(auto& t:s->buttonTransitions)t.start=GetTickCount64()-(ULONGLONG)wp;
        InvalidateRect(h,nullptr,FALSE);
        UpdateWindow(h);
        return (LRESULT)s->buttonTransitions.size();
        }
    case WM_APP+937: HandleAttentionEvent((UINT_PTR)wp,(HWND)lp);return 0;
    case WM_APP+938: return (LRESULT)s->attention.size();
    case WM_APP+939: return GroupNeedsAttention((size_t)wp)?1:0;
    case WM_APP+935: s->nativeGlomLevel=(int)wp;BuildGroups();InvalidateRect(h,nullptr,FALSE);return s->labelMode?1:0;
    case WM_APP+936: return s->labelMode?1:0;
    case WM_APP+911: taskbarPlacementMode.store((int)wp);UpdatePlacement();InvalidateRect(h,nullptr,FALSE);return s->verticalActive;
    case WM_APP+928: reserveVerticalWorkArea.store(wp!=0);s->appbarRetryAt=0;UpdatePlacement();return s->appbarRegistered;
    case WM_APP+929: return s->barEdgeOverride;
    case WM_APP+930: return s->appbarRegistered?((LRESULT)s->appbarEdge+1):0;
    case WM_APP+931: {RECT r{};GetClientRect(h,&r);for(int y=r.bottom/2;y>=0;--y)if(VerticalHit(r.right/2,y)==HIT_NONE)return y;return -1;}
    case WM_APP+912: {RECT r{};
        GetClientRect(h,&r);
        auto v=GetVerticalLayout(r.right,r.bottom);
        RECT a=wp==1?v.start:wp==2?v.arrow:wp==3?v.network:wp==4?v.volume:wp==5?v.clock:wp==6?v.desktop:wp==7?v.more:wp==8?v.desktopBand:wp==9?v.linksBand:wp==10?v.addressBand:RECT{};
        return MAKELPARAM((a.left+a.right)/2,(a.top+a.bottom)/2);
        }
    case WM_APP+913: {RECT r{};
        GetClientRect(h,&r);
        auto v=GetVerticalLayout(r.right,r.bottom);
        int i=(int)wp;
        if(i<v.offset || i>=v.offset+v.count)return 0;
        return MAKELPARAM(r.right/2,v.first+(i-v.offset)*v.step+v.step/2);
        }
    case WM_APP+914: return VerticalHit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
    case WM_APP+915: for(size_t i=0;i<s->groups.size();++i)for(size_t t:s->groups[i].tasks)if(s->tasks[t].hwnd==(HWND)wp)return (LRESULT)i+1;return 0;
    case WM_APP+916: testForceLayeredFailure=false;
        ConfigureBarAlpha(h,true);
        InvalidateRect(h,nullptr,FALSE);
        UpdateWindow(h);
        return s->perPixelBar && s->perPixelFrame;
    case WM_APP+917: if(s->trayIcons.empty())return 0;
        if(std::find(s->trayPinned.begin(),s->trayPinned.end(),s->trayIcons.front().preferenceKey)==s->trayPinned.end())s->trayPinned.push_back(s->trayIcons.front().preferenceKey);
        InvalidateRect(h,nullptr,FALSE);
        return 1;
    case WM_APP+924: PositionOriginalBatteryFlyout((HWND)wp);return 0;
    case WM_APP+926: return (size_t)wp<s->groups.size()?(LRESULT)s->groups[(size_t)wp].tasks.size():0;
    case WM_APP+927: return (LRESULT)s->previews.size();
    case WM_APP+925: s->hasBattery=wp!=0;return 0;
    case WM_APP+923: return (LRESULT)VisiblePrivateIcons().size();
    case WM_APP+922: allowDragReorder.store(wp!=0);return 0;
    case WM_APP+921: HidePreview();return 0;
    case WM_APP+919: ShowPreview((int)wp);return s->preview && IsWindowVisible(s->preview);
    case WM_APP+920: return ShowJumpList((size_t)wp,POINT{});
    case WM_APP+918: {if(s->trayIcons.empty())return 0;RECT r=PrivateIconRect(0);return MAKELPARAM((r.left+r.right)/2,(r.top+r.bottom)/2);}
    case WM_APP+910: { // Find a genuinely empty task-area coordinate for input tests.
        RECT r{};GetClientRect(h,&r);Layout l=GetLayout(r.right,r.bottom);
        for(int x=r.right/2;x>=l.first;--x) {POINT p{x,r.bottom/2};if(!PtInRect(&l.tray,p) && Hit(p.x,p.y)==HIT_NONE)return x;}
        for(int x=r.right/2;x<l.tray.left;++x) {POINT p{x,r.bottom/2};if(Hit(p.x,p.y)==HIT_NONE)return x;} return -1;
    }
    case WM_APP+909: s->bandsLocked=wp!=0;return 0;
    case WM_APP+908: ShowPrivateOverflow();return s->overflowPanel && IsWindowVisible(s->overflowPanel);
    case WM_APP+907: if(wp>1){s->desktopBand=(wp&2)!=0;
        s->linksBand=(wp&4)!=0;
        } s->addressBand=(wp&1)!=0;
         UpdateAddressBand();
         InvalidateRect(h,nullptr,FALSE);
         return s->addressHost && IsWindowVisible(s->addressHost);
    case WM_APP+906: return testBarFrames;
    case WM_APP+904: return s->perPixelBar && s->perPixelFrame;
    case WM_APP+905: testForceLayeredFailure=true; InvalidateRect(h,nullptr,FALSE); return 0;
    case WM_APP+901: return IsSystemVisible((int)wp)?1:0;
    case WM_APP+902: {
        RECT r=SystemCell((int)wp); POINT p{(r.left+r.right)/2,(r.top+r.bottom)/2};
        ClientToScreen(h,&p); return MAKELPARAM(p.x,p.y);
    }
    case WM_APP+903: {
        RECT r{}; if(!OverflowIconScreenRect(SystemEntry((int)wp),&r)) return 0;
        return MAKELPARAM((r.left+r.right)/2,(r.top+r.bottom)/2);
    }
#endif
    case WM_MOUSEWHEEL: {
        POINT p{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};
        if(volumeWheel.load() && ScreenToClient(h,&p) && Hit(p.x,p.y)==HIT_VOLUME) {AdjustVolumeWheel(GET_WHEEL_DELTA_WPARAM(wp));return 0;}
        break;
    }
    case WM_MBUTTONDOWN: { int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
         if(hit<=HIT_TRAY_ICON_BASE) HandlePrivateIconEvent(hit,WM_MBUTTONDOWN);
         return 0;
         }
    case WM_MBUTTONUP: {
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        if(hit<=HIT_TRAY_ICON_BASE) { HandlePrivateIconEvent(hit,WM_MBUTTONUP); return 0; }
        if(hit>=0 && (size_t)hit<s->groups.size()) LaunchGroupInstance((size_t)hit);
        return 0;
    }
    case WM_RBUTTONDOWN: { int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        s->jumpPressedKey.clear();
        if(hit>=0 && (size_t)hit<s->groups.size()) {
            const auto& key=s->groups[hit].key;
            if(s->jumpPopup && IsWindow(s->jumpPopup) && s->jumpGroupKey==key) {s->jumpPressedKey=key;CloseJumpPopup(true);}
            else if(s->jumpDismissKey==key && GetTickCount64()-s->jumpDismissAt<500) s->jumpPressedKey=key;
        }
        if(hit<=HIT_TRAY_ICON_BASE) HandlePrivateIconEvent(hit,WM_RBUTTONDOWN); return 0; }
    case WM_RBUTTONUP: {
        int hit=Hit(GET_X_LPARAM(lp),GET_Y_LPARAM(lp));
        if(hit<=HIT_TRAY_ICON_BASE) { HandlePrivateIconEvent(hit,WM_RBUTTONUP); return 0; }
        if(hit==HIT_LANGUAGE) { ShowLanguageIndicatorMenu(); return 0; }
        if(hit>=0 && (size_t)hit<s->groups.size() && !s->jumpPressedKey.empty() && s->jumpPressedKey==s->groups[hit].key) {s->jumpPressedKey.clear();
            s->jumpDismissKey.clear();
            return 0;
            }
        s->jumpPressedKey.clear(); s->jumpDismissKey.clear();
        if(hit==HIT_CLOCK) { ShowClockContextMenu(); return 0; }
        if(hit==HIT_DESKTOP) {POINT p{};GetCursorPos(&p);ShowDesktopContextMenu(p);return 0;}
        if(hit==HIT_NETWORK || hit==HIT_VOLUME || hit==HIT_BATTERY) { POINT p{}; GetCursorPos(&p); ShowSystemTrayContextMenu(hit,p); return 0; }
        if(hit==HIT_START) { SendWin('X'); return 0; }
        POINT p{}; GetCursorPos(&p);
        if(hit>=0 && (size_t)hit<s->groups.size()) {
            HidePreview();
            if(!(GetKeyState(VK_SHIFT)&0x8000) && ShowJumpList((size_t)hit,p)) return 0;
            Group g=s->groups[hit];
            if(g.tasks.empty() && g.pin>=0 && (size_t)g.pin<s->pins.size()) {
                std::wstring path=s->pins[g.pin].path;
                int action=w7t::ShellMenu::ShowPinMenu(p.x,p.y,true,Tr(L"Apri",L"Open"),Tr(L"Apri cartella collegamenti",L"Open shortcuts folder"),path.c_str(),nullptr);
                if(action==1) ShellExecuteW(h,L"open",path.c_str(),nullptr,nullptr,SW_SHOWNORMAL);
                if(action==2) {
                    std::wstring args=L"/select,\""+path+L"\"";
                    ShellExecuteW(h,L"open",L"explorer.exe",args.c_str(),nullptr,SW_SHOWNORMAL);
                }
            } else if(g.tasks.size()==1) {
                w7t::ShellMenu::ShowWindowSystemMenu(s->tasks[g.tasks.front()].hwnd,p.x,p.y,true);
            } else {
                ShowGroupWindowMenu(p,g);
            }
        } else {
            ShowBarContextMenu(false);
        }
        return 0;
    }
    case WM_DESTROY: s->buttonTransitions.clear();
        KillTimer(h,TIMER_BUTTON_TRANSITION);
        StopAttentionHook(h);
        KillTimer(h,TIMER_NETWORK_POSITION);
         KillTimer(h,TIMER_VOLUME_POSITION);
         KillTimer(h,TIMER_ACTION_POSITION);
         CancelBarEdgeDrag(false);
         RemoveWorkAreaAppBar();
         KillTimer(h,TIMER_CLASSIC_POSITION_NOTIFY);
         KillTimer(h,TIMER_RESOURCE_RELOAD);
         CancelBatterySlide();
         CleanupBatteryFrame();
         CancelDragHover();
         CancelActivationRetry();
         if(s && s->dropRegistered) {RevokeDragDrop(h);
        s->dropRegistered=false;
        } DestroyDesktopPopup();
         DestroyAddressBand();
         CloseJumpPopup();
         KillTimer(h,TIMER_REFRESH);
        KillTimer(h,TIMER_LANGUAGE_REFRESH);
         KillTimer(h,TIMER_HOVER);
         KillTimer(h,TIMER_PREVIEW);
         KillTimer(h,TIMER_CLOCK_FIX);
         KillTimer(h,TIMER_ANIMATION);
         KillTimer(h,TIMER_FADE);
         KillTimer(h,TIMER_Z_GUARD);
         KillTimer(h,TIMER_DESKTOP_PEEK);
         PeekOff(false);
         HidePreview();
         PostQuitMessage(0);
         return 0;
    }
    return DefWindowProcW(h,m,wp,lp);
}
LRESULT CALLBACK BarProc(HWND h,UINT m,WPARAM wp,LPARAM lp) {
    try { return BarProcImpl(h,m,wp,lp); }
    catch(const std::bad_alloc&) { Wh_Log(L"Allocation failure in bar callback; stopping the taskbar UI"); PostQuitMessage(0); return 0; }
    catch(...) { Wh_Log(L"C++ exception in bar callback; stopping the taskbar UI"); PostQuitMessage(0); return 0; }
}
DWORD WINAPI UIThread(LPVOID) {
    // Create the message queue before checking the stop flag: posting WM_QUIT is then safe.
    MSG msg{}; PeekMessageW(&msg,nullptr,WM_USER,WM_USER,PM_NOREMOVE);
    // Explorer can run as several processes. Only the process that owns the
    // primary Shell_TrayWnd may create this bar or change its visibility.
    HWND ownedTray=nullptr;
    for(int i=0;i<120 && !quitting.load();i++) {
        ownedTray=FindOwnedNativeTaskbar();
        if(ownedTray) break;
        Sleep(500);
    }
    if(quitting.load() || !ownedTray) {
        Wh_Log(L"No taskbar owned by this Explorer process; no UI created"); return 0;
    }
    // Limit DPI-awareness change to our UI thread, never the Explorer process.
    using DpiCtxFn=DPI_AWARENESS_CONTEXT (WINAPI*)(DPI_AWARENESS_CONTEXT);
    auto setDpi=(DpiCtxFn)GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetThreadDpiAwarenessContext");
    DPI_AWARENESS_CONTEXT previousDpi=setDpi?setDpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2):nullptr;
    ULONG_PTR token=0; GdiplusStartupInput input;
    if(GdiplusStartup(&token,&input,nullptr)!=Ok) { Wh_Log(L"GDI+ startup failed"); if(setDpi && previousDpi) setDpi(previousDpi); return 1; }
    bool registered=false;
    HRESULT com=OleInitialize(nullptr);
    if(FAILED(com)) Wh_Log(L"[T81-DIAG] OleInitialize failed hr=0x%08lx; drag hover unavailable",(unsigned long)com);
    HRESULT ro=RoInitialize(RO_INIT_SINGLETHREADED);
    if(FAILED(ro)) Wh_Log(L"[T81-DIAG] RoInitialize failed hr=0x%08lx; using NLS input language fallback",(unsigned long)ro);
    State local; s=&local;
    try {
        if(!LoadStartImage(local)) Wh_Log(L"Start sprite failed to load");
        if(!LoadBetaStartImage(local)) Wh_Log(L"Beta Start sprite failed to load; using default");
        if(!LoadWin7StartImage(local)) Wh_Log(L"[T81-DIAG] Windows 7 Start sprite unavailable; using existing Start");
        LoadBetaTextures(local);

        LoadOrderOverride(); LoadBands(); LoadDesktopPeekPreference(); RefreshNativeLabelPreference();
        LoadTrayPreferences();
        WNDCLASSW wc{}; wc.style=CS_DBLCLKS; wc.lpfnWndProc=BarProc; wc.hInstance=GetModuleHandleW(nullptr);
        wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)); wc.lpszClassName=kClass;
        registered=RegisterClassW(&wc)!=0;
        WNDCLASSW previewWc{};
        previewWc.lpfnWndProc=PreviewProc;
        previewWc.hInstance=wc.hInstance;
        previewWc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
        previewWc.lpszClassName=kPreviewClass;
        bool previewRegistered=RegisterClassW(&previewWc)!=0;
        if(!previewRegistered) Wh_Log(L"Preview window registration failed");
        WNDCLASSW tooltipWc{};
        tooltipWc.lpfnWndProc=PinTooltipProc;
        tooltipWc.hInstance=wc.hInstance;
        tooltipWc.lpszClassName=kTooltipClass;
        bool tooltipRegistered=RegisterClassW(&tooltipWc)!=0;
        if(!tooltipRegistered) Wh_Log(L"Pin tooltip class registration failed");
        WNDCLASSW nativeControlWc{};
        nativeControlWc.lpfnWndProc=NativeControlOverlayProc;
        nativeControlWc.hInstance=wc.hInstance;
        nativeControlWc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));
        nativeControlWc.lpszClassName=kNativeControlOverlayClass;
        bool nativeControlsRegistered=RegisterClassW(&nativeControlWc)!=0;
        if(!nativeControlsRegistered) Wh_Log(L"Native control overlay class registration failed; controls remain native");
        if(registered && !quitting.load()) {
            RECT r=FallbackRect();
            local.bar=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST|WS_EX_LAYERED,kClass,L"Windows 8.1 Taskbar Recreation",
                WS_POPUP,r.left,r.top,r.right-r.left,r.bottom-r.top,nullptr,nullptr,wc.hInstance,nullptr);
            barWindow.store(local.bar);
            if(local.bar) {ConfigureBarAlpha(local.bar,perPixelTaskbar.load());StartAttentionHook(local.bar);}
            if(local.bar && !quitting.load()) {
                ExcludeFromPeek(local.bar);
                if(SUCCEEDED(com)) RegisterTaskDropTarget(local.bar);
                local.native=ownedTray;
                local.visibilityHook=SetWinEventHook(EVENT_OBJECT_CREATE,EVENT_OBJECT_SHOW,nullptr,ShellVisibilityEvent,GetCurrentProcessId(),0,WINEVENT_OUTOFCONTEXT);
                if(!local.visibilityHook) Wh_Log(L"[T81-DIAG] visibility event hook unavailable error=%lu",GetLastError());
                UpdatePlacement(); Refresh();
                if(local.perPixelBar) { InvalidateRect(local.bar,nullptr,FALSE); UpdateWindow(local.bar); }
                if(enablePrivateTray.load() && IsWindowVisible(local.bar) && (local.perPixelBar?local.perPixelFrame:local.legacyAlphaReady) && StartPrivateTray()) {
                    StartResourceWorker();
                    RefreshVolume(); PublishCniSystemIcon(HIT_VOLUME);
                    RefreshBattery(); local.netKind=QueryNetworkKind(); PublishCniSystemIcon(HIT_NETWORK);
                    UpdatePlacement(); LogTaskbarDiagnostics(L"startup"); GuardPrivateTray(); InvalidateRect(local.bar,nullptr,FALSE);
                } else {
                    Wh_Log(L"[T81-DIAG] private tray unavailable or disabled; native taskbar remains available");
                    LogTaskbarDiagnostics(L"startup-fallback");
                }
                while(!quitting.load() && GetMessageW(&msg,nullptr,0,0)>0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
            } else if(!local.bar) Wh_Log(L"Failed to create bar window, native bar was not hidden");
        } else if(!registered) Wh_Log(L"Failed to register bar window class");
        if(local.visibilityHook) {UnhookWinEvent(local.visibilityHook);local.visibilityHook=nullptr;}
        CancelBatterySlide(); CleanupBatteryFrame();
        StopResourceWorker();
        DestroyDesktopPopup(); DestroyAddressBand(); CloseJumpPopup(); UnregisterClassW(kJumpClass,GetModuleHandleW(nullptr));
        DestroyPrivateOverflow();
        DestroyLanguageFlyout();
        StopPrivateTray();
        if(local.endpoint) { local.endpoint->Release(); local.endpoint=nullptr; }
        ReleaseNativeControlOverlays();
        HidePreview();
        FreePins();
        FreeTaskIcons();
        if(local.preview && IsWindow(local.preview)) DestroyWindow(local.preview);
        if(local.tooltip && IsWindow(local.tooltip)) DestroyWindow(local.tooltip);
        UnregisterClassW(kPreviewClass,wc.hInstance);
        if(tooltipRegistered) UnregisterClassW(kTooltipClass,wc.hInstance);
        if(nativeControlsRegistered) UnregisterClassW(kNativeControlOverlayClass,wc.hInstance);
        nativeWindow.store(nullptr); barWindow.store(nullptr);
        StopAttentionHook(local.bar);
        DestroyWin7OrbSurface();
        if(local.bar && IsWindow(local.bar)) DestroyWindow(local.bar);
        w7t::CleanupShellMenuOwner();
        if(registered) UnregisterClassW(kClass,wc.hInstance);
        DestroyStartImage(local); ClearLegacyImages();
        s=nullptr;
    } catch(...) {
        // C++ only; each cleanup is isolated so one failure cannot skip the
        // attempt to restore the native taskbar. Fatal SEH is not intercepted.
        auto cleanup=[](auto fn) noexcept {try {fn();} catch(...) {Wh_Log(L"[T81-DIAG] C++ exception during cleanup");}};
        if(s) {
            cleanup([&]{StopAttentionHook(local.bar);});
            cleanup([&]{if(local.visibilityHook){UnhookWinEvent(local.visibilityHook);local.visibilityHook=nullptr;}});
            cleanup([&]{CancelBatterySlide();CleanupBatteryFrame();});
            cleanup([&]{StopResourceWorker();});cleanup([&]{StopPrivateTray();});
            cleanup([&]{DestroyDesktopPopup();}); cleanup([&]{DestroyAddressBand();});
            cleanup([&]{CloseJumpPopup();}); cleanup([&]{DestroyPrivateOverflow();});
            cleanup([&]{DestroyLanguageFlyout();});
            cleanup([&]{if(local.endpoint){local.endpoint->Release();local.endpoint=nullptr;}});
            cleanup([&]{ReleaseNativeControlOverlays();});cleanup([&]{HidePreview();});
            cleanup([&]{FreePins();});cleanup([&]{FreeTaskIcons();});
            cleanup([&]{if(s->preview && IsWindow(s->preview)) DestroyWindow(s->preview);});
            cleanup([&]{if(s->tooltip && IsWindow(s->tooltip)) DestroyWindow(s->tooltip);});
            cleanup([&]{DestroyWin7OrbSurface();});
            cleanup([&]{if(s->bar && IsWindow(s->bar)) DestroyWindow(s->bar);});
        }
        cleanup([&]{DestroyDesktopPopup();}); UnregisterClassW(kJumpClass,GetModuleHandleW(nullptr));
        UnregisterClassW(kPreviewClass,GetModuleHandleW(nullptr)); UnregisterClassW(kTooltipClass,GetModuleHandleW(nullptr));
        UnregisterClassW(kNativeControlOverlayClass,GetModuleHandleW(nullptr));
        barWindow.store(nullptr); nativeWindow.store(nullptr);
        cleanup([&]{w7t::CleanupShellMenuOwner();}); if(registered) UnregisterClassW(kClass,GetModuleHandleW(nullptr));
        cleanup([&]{DestroyStartImage(local);});cleanup([&]{ClearLegacyImages();});s=nullptr;
        Wh_Log(L"C++ exception on taskbar thread; custom UI stopped; native taskbar restoration attempted");
    }
    if(SUCCEEDED(ro))RoUninitialize();
    if(SUCCEEDED(com)) OleUninitialize();
    GdiplusShutdown(token); if(setDpi && previousDpi) setDpi(previousDpi); return 0;
}
} // namespace

// No user32 hooks. The private host periodically hides the native window;
// normal unload restores its original visibility. Competing mods can conflict.
BOOL Wh_ModInit() {
    quitting.store(false);
    LoadSettings();
    InstallTrayCniHook();
    if(useOriginalBatteryFlyout.load()) {
        HMODULE user=GetModuleHandleW(L"user32.dll");
        void* target=user?(void*)GetProcAddress(user,"LoadStringW"):nullptr;
        if(!target || !Wh_SetFunctionHook(target,(void*)BatteryLoadStringHook,(void**)&realLoadStringW))
            Wh_Log(L"[T81-DIAG] battery resource bridge hook could not be installed");
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    threadHandle=CreateThread(nullptr,0,UIThread,nullptr,0,&threadId);
    if(!threadHandle) Wh_Log(L"Unable to create bar thread; native taskbar remains untouched");
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    const bool oldTray=enablePrivateTray.load();
    const bool oldBattery=useOriginalBatteryFlyout.load();
    LoadSettings();
    // These choices require setup normally performed once at initialization:
    // tray ownership, or LoadStringW hook and battery resource provisioning.
    const bool needsReload=oldTray!=enablePrivateTray.load() || (!oldBattery && useOriginalBatteryFlyout.load());
    if(reload)*reload=needsReload?TRUE:FALSE;
    if(needsReload) Wh_Log(L"[T81-DIAG] settings request Windhawk mod reload; battery DLL guard remains in force");
    else { HWND h=barWindow.load(); if(h && !PostMessageW(h,WM_UPDATE_SETTINGS,0,0)) Wh_Log(L"[T81-DIAG] settings notification failed error=%lu",GetLastError()); }
    return TRUE;
}
void Wh_ModBeforeUninit() {
    quitting.store(true);
    if(threadHandle) {
        // TrackPopupMenuEx has its own modal loop: ask its owner to cancel it
        // before posting WM_QUIT, or unloading can wait forever on that loop.
        HWND menuOwner=w7t::menuOwnerForShutdown.load();
        if(menuOwner) PostMessageW(menuOwner,WM_CANCELMODE,0,0);
        HWND customBar=barWindow.load();
        if(customBar) PostMessageW(customBar,WM_CANCELMODE,0,0);
        PostThreadMessageW(threadId,WM_QUIT,0,0);
        // The DLL cannot safely be unloaded while its thread executes its code.
        WaitForSingleObject(threadHandle,INFINITE);
        CloseHandle(threadHandle); threadHandle=nullptr;
    }
}
void Wh_ModUninit() { batteryStringModule.store(nullptr); batteryStringMui.store(nullptr); }

/*
ManagedShell and RetroBar: Apache License, Version 2.0.
Source adaptations identified above are modified from their C# originals.
                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets "[]"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same "printed page" as the copyright notice for easier
      identification within third-party archives.

   Copyright [yyyy] [name of copyright owner]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

/*
RetroBar drag-coordinate translation: copyright RetroBar contributors;
https://github.com/dremin/RetroBar, commit 69f36278150cb39bc37d3237e3178926734c6804.
Adapted to C++, restricted to horizontal edges; drag threshold changed.
The RetroBar component is distributed under the following license:

                                 Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/

   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

   1. Definitions.

      "License" shall mean the terms and conditions for use, reproduction,
      and distribution as defined by Sections 1 through 9 of this document.

      "Licensor" shall mean the copyright owner or entity authorized by
      the copyright owner that is granting the License.

      "Legal Entity" shall mean the union of the acting entity and all
      other entities that control, are controlled by, or are under common
      control with that entity. For the purposes of this definition,
      "control" means (i) the power, direct or indirect, to cause the
      direction or management of such entity, whether by contract or
      otherwise, or (ii) ownership of fifty percent (50%) or more of the
      outstanding shares, or (iii) beneficial ownership of such entity.

      "You" (or "Your") shall mean an individual or Legal Entity
      exercising permissions granted by this License.

      "Source" form shall mean the preferred form for making modifications,
      including but not limited to software source code, documentation
      source, and configuration files.

      "Object" form shall mean any form resulting from mechanical
      transformation or translation of a Source form, including but
      not limited to compiled object code, generated documentation,
      and conversions to other media types.

      "Work" shall mean the work of authorship, whether in Source or
      Object form, made available under the License, as indicated by a
      copyright notice that is included in or attached to the work
      (an example is provided in the Appendix below).

      "Derivative Works" shall mean any work, whether in Source or Object
      form, that is based on (or derived from) the Work and for which the
      editorial revisions, annotations, elaborations, or other modifications
      represent, as a whole, an original work of authorship. For the purposes
      of this License, Derivative Works shall not include works that remain
      separable from, or merely link (or bind by name) to the interfaces of,
      the Work and Derivative Works thereof.

      "Contribution" shall mean any work of authorship, including
      the original version of the Work and any modifications or additions
      to that Work or Derivative Works thereof, that is intentionally
      submitted to Licensor for inclusion in the Work by the copyright owner
      or by an individual or Legal Entity authorized to submit on behalf of
      the copyright owner. For the purposes of this definition, "submitted"
      means any form of electronic, verbal, or written communication sent
      to the Licensor or its representatives, including but not limited to
      communication on electronic mailing lists, source code control systems,
      and issue tracking systems that are managed by, or on behalf of, the
      Licensor for the purpose of discussing and improving the Work, but
      excluding communication that is conspicuously marked or otherwise
      designated in writing by the copyright owner as "Not a Contribution."

      "Contributor" shall mean Licensor and any individual or Legal Entity
      on behalf of whom a Contribution has been received by Licensor and
      subsequently incorporated within the Work.

   2. Grant of Copyright License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      copyright license to reproduce, prepare Derivative Works of,
      publicly display, publicly perform, sublicense, and distribute the
      Work and such Derivative Works in Source or Object form.

   3. Grant of Patent License. Subject to the terms and conditions of
      this License, each Contributor hereby grants to You a perpetual,
      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
      (except as stated in this section) patent license to make, have made,
      use, offer to sell, sell, import, and otherwise transfer the Work,
      where such license applies only to those patent claims licensable
      by such Contributor that are necessarily infringed by their
      Contribution(s) alone or by combination of their Contribution(s)
      with the Work to which such Contribution(s) was submitted. If You
      institute patent litigation against any entity (including a
      cross-claim or counterclaim in a lawsuit) alleging that the Work
      or a Contribution incorporated within the Work constitutes direct
      or contributory patent infringement, then any patent licenses
      granted to You under this License for that Work shall terminate
      as of the date such litigation is filed.

   4. Redistribution. You may reproduce and distribute copies of the
      Work or Derivative Works thereof in any medium, with or without
      modifications, and in Source or Object form, provided that You
      meet the following conditions:

      (a) You must give any other recipients of the Work or
          Derivative Works a copy of this License; and

      (b) You must cause any modified files to carry prominent notices
          stating that You changed the files; and

      (c) You must retain, in the Source form of any Derivative Works
          that You distribute, all copyright, patent, trademark, and
          attribution notices from the Source form of the Work,
          excluding those notices that do not pertain to any part of
          the Derivative Works; and

      (d) If the Work includes a "NOTICE" text file as part of its
          distribution, then any Derivative Works that You distribute must
          include a readable copy of the attribution notices contained
          within such NOTICE file, excluding those notices that do not
          pertain to any part of the Derivative Works, in at least one
          of the following places: within a NOTICE text file distributed
          as part of the Derivative Works; within the Source form or
          documentation, if provided along with the Derivative Works; or,
          within a display generated by the Derivative Works, if and
          wherever such third-party notices normally appear. The contents
          of the NOTICE file are for informational purposes only and
          do not modify the License. You may add Your own attribution
          notices within Derivative Works that You distribute, alongside
          or as an addendum to the NOTICE text from the Work, provided
          that such additional attribution notices cannot be construed
          as modifying the License.

      You may add Your own copyright statement to Your modifications and
      may provide additional or different license terms and conditions
      for use, reproduction, or distribution of Your modifications, or
      for any such Derivative Works as a whole, provided Your use,
      reproduction, and distribution of the Work otherwise complies with
      the conditions stated in this License.

   5. Submission of Contributions. Unless You explicitly state otherwise,
      any Contribution intentionally submitted for inclusion in the Work
      by You to the Licensor shall be under the terms and conditions of
      this License, without any additional terms or conditions.
      Notwithstanding the above, nothing herein shall supersede or modify
      the terms of any separate license agreement you may have executed
      with Licensor regarding such Contributions.

   6. Trademarks. This License does not grant permission to use the trade
      names, trademarks, service marks, or product names of the Licensor,
      except as required for reasonable and customary use in describing the
      origin of the Work and reproducing the content of the NOTICE file.

   7. Disclaimer of Warranty. Unless required by applicable law or
      agreed to in writing, Licensor provides the Work (and each
      Contributor provides its Contributions) on an "AS IS" BASIS,
      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
      implied, including, without limitation, any warranties or conditions
      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
      PARTICULAR PURPOSE. You are solely responsible for determining the
      appropriateness of using or redistributing the Work and assume any
      risks associated with Your exercise of permissions under this License.

   8. Limitation of Liability. In no event and under no legal theory,
      whether in tort (including negligence), contract, or otherwise,
      unless required by applicable law (such as deliberate and grossly
      negligent acts) or agreed to in writing, shall any Contributor be
      liable to You for damages, including any direct, indirect, special,
      incidental, or consequential damages of any character arising as a
      result of this License or out of the use or inability to use the
      Work (including but not limited to damages for loss of goodwill,
      work stoppage, computer failure or malfunction, or any and all
      other commercial damages or losses), even if such Contributor
      has been advised of the possibility of such damages.

   9. Accepting Warranty or Additional Liability. While redistributing
      the Work or Derivative Works thereof, You may choose to offer,
      and charge a fee for, acceptance of support, warranty, indemnity,
      or other liability obligations and/or rights consistent with this
      License. However, in accepting such obligations, You may act only
      on Your own behalf and on Your sole responsibility, not on behalf
      of any other Contributor, and only if You agree to indemnify,
      defend, and hold each Contributor harmless for any liability
      incurred by, or claims asserted against, such Contributor by reason
      of your accepting any such warranty or additional liability.

   END OF TERMS AND CONDITIONS

   APPENDIX: How to apply the Apache License to your work.

      To apply the Apache License to your work, attach the following
      boilerplate notice, with the fields enclosed by brackets "[]"
      replaced with your own identifying information. (Don't include
      the brackets!)  The text should be enclosed in the appropriate
      comment syntax for the file format. We also recommend that a
      file or class name and description of purpose be included on the
      same "printed page" as the copyright notice for easier
      identification within third-party archives.

   Copyright [yyyy] [name of copyright owner]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

/*
Aero Flyout Fix: https://windhawk.net/mods/aero-flyout-fix
The BatteryFrameSubclass adaptation above is based on this mod. The mod
declares no license; per the official Windhawk mods repository rules
(https://github.com/ramensoftware/windhawk-mods), mods which don't specify
a license are submitted under the MIT license, reproduced below.

MIT License

Copyright (c) Aero Flyout Fix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
