// ==WindhawkMod==
// @id              super-mouse-shortcuts
// @name            Super Shortcuts for Mouse
// @name:tr-TR      Mouse için süper kısayollar
// @name:de-DE      Super-Shortcuts für die Maus
// @name:fr-FR      Super raccourcis pour la souris
// @name:ru-RU      Супер-ярлыки для мыши
// @description     Hold the right (or a side) mouse button and click or scroll to close, minimize, maximize the window under the cursor, skip tracks and more
// @description:tr-TR Sağ (veya yan) fare tuşunu basılı tutup tıklayarak ya da tekerleği çevirerek imlecin altındaki pencereyi kapatın, küçültün, büyütün, şarkı geçin ve daha fazlası
// @description:de-DE Rechte (oder seitliche) Maustaste gedrückt halten und klicken oder scrollen, um das Fenster unter dem Mauszeiger zu schließen, zu minimieren, zu maximieren, Titel zu überspringen und mehr
// @description:fr-FR Maintenez le bouton droit (ou latéral) de la souris et cliquez ou faites défiler pour fermer, réduire, agrandir la fenêtre sous le curseur, changer de piste et plus encore
// @description:ru-RU Удерживайте правую (или боковую) кнопку мыши и щёлкайте или прокручивайте колесо, чтобы закрыть, свернуть, развернуть окно под курсором, переключать треки и многое другое
// @version         1.0
// @author          DahaSakinOlamazdim
// @github          https://github.com/DahaSakinOlamazdim
// @include         windhawk.exe
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Super Shortcuts for Mouse

**English** · [Türkçe](#türkçe) · [Deutsch](#deutsch) · [Français](#français) · [Русский](#русский)

Hold a mouse button, then click or scroll with another one to run an action on
the **window under the cursor**: close, minimize, maximize or restore it, snap
it, toggle always-on-top, skip to the next / previous track, play / pause,
change the volume, show the desktop, switch virtual desktops, take a screenshot
and more.

## How it works

* **Hold button** (choose in the settings):
  * Right mouse button (default)
  * Back (side) mouse button
  * Forward (side) mouse button
* While it is held, these inputs are available as triggers, and each can be
  assigned any action in the settings: left click, middle click, scroll wheel up /
  down, the other side button, and the right button (side-button mode only).
* Window actions always apply to the window under the mouse cursor, not to the
  active window.
* Default mapping: left click = close, middle click = play / pause, wheel up =
  maximize, wheel down = minimize, Back = previous track, Forward = next track,
  right click (side-button mode) = always on top.
* If you press and release the hold button without using any trigger, it works as a
  normal click (context menu, browser back / forward...). Right-button drags are
  detected and passed through to the application unchanged.
* The language (English, Turkish, German, French, Russian) follows your Windows
  display language automatically. Other languages fall back to English.

## Notes

* The mod runs in `windhawk.exe` and uses a global low-level mouse hook.
* Windows that run as administrator ignore requests from a non-elevated process.
  To control them, run Windhawk as administrator.
* Synthetic mouse events from other programs are not affected, and the mod is
  disabled while a fullscreen app / game is active (can be turned off).
* "None" assigned to a trigger blocks that input while the hold button is down.

---

## Türkçe

Bir fare tuşunu basılı tutup başka bir tuşa tıklayarak ya da tekerleği çevirerek
**imlecin altındaki pencerede** işlev çalıştırın: kapat, simge durumuna küçült,
büyüt, eski boyuta getir, yasla, her zaman üstte, sonraki / önceki şarkı,
oynat / duraklat, ses, masaüstünü göster, sanal masaüstü değiştir, ekran
görüntüsü ve daha fazlası.

* Basılı tutulacak tuş: sağ fare tuşu (varsayılan), Geri veya İleri yan tuşu.
* Tuş basılıyken sol tık, orta tık, tekerlek yukarı / aşağı, diğer yan tuş ve
  (yalnızca yan tuş modunda) sağ tık tetikleyici olarak kullanılabilir; her birine
  ayarlardan istediğiniz işlevi atayabilirsiniz.
* Pencere işlevleri her zaman etkin pencereye değil, fare imlecinin altındaki
  pencereye uygulanır.
* Tuşa hiçbir tetikleyici kullanmadan basıp bırakırsanız normal tıklama gibi çalışır.
* Dil, Windows görüntü dilinize göre otomatik seçilir; desteklenmeyen diller için
  İngilizce kullanılır.
* Yönetici olarak çalışan pencereleri de kontrol etmek için Windhawk'u yönetici
  olarak çalıştırın. Tam ekran uygulama / oyun açıkken mod devre dışı kalır.

---

## Deutsch

Halten Sie eine Maustaste gedrückt und klicken oder scrollen Sie mit einer anderen,
um eine Aktion für das **Fenster unter dem Mauszeiger** auszuführen: schließen,
minimieren, maximieren, andocken, immer im Vordergrund, Titel überspringen,
Wiedergabe/Pause, Lautstärke, Desktop anzeigen, virtuelle Desktops wechseln,
Screenshot und mehr. Die Sprache folgt automatisch der Windows-Anzeigesprache
(sonst Englisch).

---

## Français

Maintenez un bouton de la souris, puis cliquez ou faites défiler avec un autre
pour exécuter une action sur la **fenêtre située sous le curseur** : fermer,
réduire, agrandir, ancrer, toujours au premier plan, changer de piste,
lecture/pause, volume, afficher le bureau, changer de bureau virtuel, capture
d'écran, etc. La langue suit automatiquement celle de Windows (sinon l'anglais).

---

## Русский

Удерживайте одну кнопку мыши и щёлкайте или прокручивайте колесо другой, чтобы
выполнить действие над **окном под курсором**: закрыть, свернуть, развернуть,
прикрепить, поверх всех окон, переключить трек, воспроизведение/пауза, громкость,
показать рабочий стол, сменить виртуальный рабочий стол, снимок экрана и другое.
Язык выбирается автоматически по языку интерфейса Windows (иначе английский).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- holdButton: right
  $name: Hold button
  $name:tr-TR: Basılı tutulacak tuş
  $name:de-DE: Gedrückt zu haltende Taste
  $name:fr-FR: Bouton à maintenir
  $name:ru-RU: Удерживаемая кнопка
  $description: Hold this mouse button, then click / scroll with another button to run an action.
  $description:tr-TR: Bu fare tuşunu basılı tutup başka bir tuşa tıklayarak veya tekerleği çevirerek işlev çalıştırın.
  $description:de-DE: Halten Sie diese Maustaste gedrückt und klicken oder scrollen Sie mit einer anderen Taste, um eine Aktion auszuführen.
  $description:fr-FR: Maintenez ce bouton de la souris, puis cliquez ou faites défiler avec un autre bouton pour exécuter une action.
  $description:ru-RU: Удерживайте эту кнопку мыши и щёлкните или прокрутите колесо другой кнопкой, чтобы выполнить действие.
  $options:
  - right: Right mouse button
  - back: Back (side) mouse button
  - forward: Forward (side) mouse button
  $options:tr-TR:
  - right: Sağ fare tuşu
  - back: Geri (yan) fare tuşu
  - forward: İleri (yan) fare tuşu
  $options:de-DE:
  - right: Rechte Maustaste
  - back: Zurück-Maustaste (seitlich)
  - forward: Vor-Maustaste (seitlich)
  $options:fr-FR:
  - right: Bouton droit de la souris
  - back: Bouton Précédent (latéral)
  - forward: Bouton Suivant (latéral)
  $options:ru-RU:
  - right: Правая кнопка мыши
  - back: Кнопка «Назад» (боковая)
  - forward: Кнопка «Вперёд» (боковая)
- showToast: true
  $name: Show action label
  $name:tr-TR: İşlev etiketini göster
  $name:de-DE: Aktionsbezeichnung anzeigen
  $name:fr-FR: Afficher l'étiquette de l'action
  $name:ru-RU: Показывать название действия
  $description: Show a small label next to the cursor when an action runs.
  $description:tr-TR: Bir işlev çalıştığında imlecin yanında küçük bir etiket gösterir.
  $description:de-DE: Zeigt neben dem Mauszeiger eine kleine Beschriftung an, wenn eine Aktion ausgeführt wird.
  $description:fr-FR: Affiche une petite étiquette près du curseur lorsqu'une action est exécutée.
  $description:ru-RU: Показывает небольшую подпись рядом с курсором при выполнении действия.
- ignoreFullscreen: true
  $name: Disable in fullscreen apps
  $name:tr-TR: Tam ekran uygulamalarda devre dışı bırak
  $name:de-DE: In Vollbild-Apps deaktivieren
  $name:fr-FR: Désactiver dans les applications plein écran
  $name:ru-RU: Отключать в полноэкранных приложениях
  $description: Do not intercept the mouse while a fullscreen app or game is in the foreground.
  $description:tr-TR: Tam ekran bir uygulama veya oyun öndeyken fareye müdahale edilmez.
  $description:de-DE: Die Maus wird nicht abgefangen, solange eine Vollbild-App oder ein Spiel im Vordergrund ist.
  $description:fr-FR: N'intercepte pas la souris lorsqu'une application ou un jeu en plein écran est au premier plan.
  $description:ru-RU: Не перехватывать мышь, пока на переднем плане полноэкранное приложение или игра.
- actions:
  - left: close
    $name: Left click
    $name:tr-TR: Sol tık
    $name:de-DE: Linksklick
    $name:fr-FR: Clic gauche
    $name:ru-RU: Левый щелчок
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - middle: playPause
    $name: Middle click
    $name:tr-TR: Orta tık
    $name:de-DE: Mittelklick
    $name:fr-FR: Clic central
    $name:ru-RU: Средний щелчок
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - right: alwaysOnTop
    $name: Right click (only when holding a side button)
    $name:tr-TR: Sağ tık (yalnızca yan tuş basılıyken)
    $name:de-DE: Rechtsklick (nur bei gedrückter Seitentaste)
    $name:fr-FR: Clic droit (uniquement avec un bouton latéral maintenu)
    $name:ru-RU: Правый щелчок (только при удержании боковой кнопки)
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - x1: prevTrack
    $name: Back button click (not available when it is the hold button)
    $name:tr-TR: Geri tuşu tıklaması (basılı tutulan tuş ise çalışmaz)
    $name:de-DE: Klick auf Zurück-Taste (nicht verfügbar, wenn sie die gedrückte Taste ist)
    $name:fr-FR: Clic sur le bouton Précédent (indisponible s'il est le bouton maintenu)
    $name:ru-RU: Щелчок кнопки «Назад» (недоступно, если это удерживаемая кнопка)
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - x2: nextTrack
    $name: Forward button click (not available when it is the hold button)
    $name:tr-TR: İleri tuşu tıklaması (basılı tutulan tuş ise çalışmaz)
    $name:de-DE: Klick auf Vor-Taste (nicht verfügbar, wenn sie die gedrückte Taste ist)
    $name:fr-FR: Clic sur le bouton Suivant (indisponible s'il est le bouton maintenu)
    $name:ru-RU: Щелчок кнопки «Вперёд» (недоступно, если это удерживаемая кнопка)
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - wheelUp: maximize
    $name: Scroll wheel up
    $name:tr-TR: Tekerlek yukarı
    $name:de-DE: Mausrad nach oben
    $name:fr-FR: Molette vers le haut
    $name:ru-RU: Колесо вверх
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  - wheelDown: minimize
    $name: Scroll wheel down
    $name:tr-TR: Tekerlek aşağı
    $name:de-DE: Mausrad nach unten
    $name:fr-FR: Molette vers le bas
    $name:ru-RU: Колесо вниз
    $options:
    - none: None (do nothing)
    - close: Close window
    - minimize: Minimize window
    - maximize: Maximize window
    - toggleMaximize: Maximize / restore (toggle)
    - restore: Restore window
    - alwaysOnTop: Toggle always on top
    - snapLeft: Snap window to the left
    - snapRight: Snap window to the right
    - moveMonitor: Move window to next monitor
    - nextTrack: Next track
    - prevTrack: Previous track
    - playPause: Play / pause
    - volumeUp: Volume up
    - volumeDown: Volume down
    - mute: Mute / unmute
    - showDesktop: Show desktop
    - taskView: Task View
    - desktopPrev: Previous virtual desktop
    - desktopNext: Next virtual desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:tr-TR:
    - none: Yok (hiçbir şey yapma)
    - close: Pencereyi kapat
    - minimize: Pencereyi simge durumuna küçült
    - maximize: Pencereyi büyüt
    - toggleMaximize: Büyüt / eski boyuta getir (değiştir)
    - restore: Pencereyi eski boyutuna getir
    - alwaysOnTop: Her zaman üstte (aç/kapat)
    - snapLeft: Pencereyi sola yasla
    - snapRight: Pencereyi sağa yasla
    - moveMonitor: Pencereyi sonraki monitöre taşı
    - nextTrack: Sonraki şarkı
    - prevTrack: Önceki şarkı
    - playPause: Oynat / duraklat
    - volumeUp: Sesi artır
    - volumeDown: Sesi azalt
    - mute: Sesi kapat / aç
    - showDesktop: Masaüstünü göster
    - taskView: Görev Görünümü
    - desktopPrev: Önceki sanal masaüstü
    - desktopNext: Sonraki sanal masaüstü
    - screenshot: Ekran görüntüsü (Ekran Alıntısı Aracı)
    $options:de-DE:
    - none: Keine (nichts tun)
    - close: Fenster schließen
    - minimize: Fenster minimieren
    - maximize: Fenster maximieren
    - toggleMaximize: Maximieren / Wiederherstellen (umschalten)
    - restore: Fenster wiederherstellen
    - alwaysOnTop: Immer im Vordergrund (umschalten)
    - snapLeft: Fenster links andocken
    - snapRight: Fenster rechts andocken
    - moveMonitor: Fenster auf nächsten Monitor verschieben
    - nextTrack: Nächster Titel
    - prevTrack: Vorheriger Titel
    - playPause: Wiedergabe / Pause
    - volumeUp: Lauter
    - volumeDown: Leiser
    - mute: Stumm / Ton an
    - showDesktop: Desktop anzeigen
    - taskView: Aufgabenansicht
    - desktopPrev: Vorheriger virtueller Desktop
    - desktopNext: Nächster virtueller Desktop
    - screenshot: Screenshot (Snipping Tool)
    $options:fr-FR:
    - none: Aucune (ne rien faire)
    - close: Fermer la fenêtre
    - minimize: Réduire la fenêtre
    - maximize: Agrandir la fenêtre
    - toggleMaximize: Agrandir / restaurer (basculer)
    - restore: Restaurer la fenêtre
    - alwaysOnTop: Toujours au premier plan (basculer)
    - snapLeft: Ancrer la fenêtre à gauche
    - snapRight: Ancrer la fenêtre à droite
    - moveMonitor: Déplacer la fenêtre vers l'écran suivant
    - nextTrack: Piste suivante
    - prevTrack: Piste précédente
    - playPause: Lecture / pause
    - volumeUp: Augmenter le volume
    - volumeDown: Baisser le volume
    - mute: Couper / rétablir le son
    - showDesktop: Afficher le bureau
    - taskView: Vue des tâches
    - desktopPrev: Bureau virtuel précédent
    - desktopNext: Bureau virtuel suivant
    - screenshot: Capture d'écran (Outil Capture d'écran)
    $options:ru-RU:
    - none: Нет (ничего не делать)
    - close: Закрыть окно
    - minimize: Свернуть окно
    - maximize: Развернуть окно
    - toggleMaximize: Развернуть / восстановить (переключить)
    - restore: Восстановить окно
    - alwaysOnTop: Поверх всех окон (переключить)
    - snapLeft: Прикрепить окно слева
    - snapRight: Прикрепить окно справа
    - moveMonitor: Переместить окно на следующий монитор
    - nextTrack: Следующий трек
    - prevTrack: Предыдущий трек
    - playPause: Воспроизведение / пауза
    - volumeUp: Громкость выше
    - volumeDown: Громкость ниже
    - mute: Выключить / включить звук
    - showDesktop: Показать рабочий стол
    - taskView: Представление задач
    - desktopPrev: Предыдущий виртуальный рабочий стол
    - desktopNext: Следующий виртуальный рабочий стол
    - screenshot: Снимок экрана (Ножницы)
  $name: Actions
  $name:tr-TR: İşlevler
  $name:de-DE: Aktionen
  $name:fr-FR: Actions
  $name:ru-RU: Действия
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <vector>

namespace {

constexpr ULONG_PTR kMarker = 0x4D425348;  // marks events injected by this mod
constexpr UINT WM_RUN_ACTION = WM_APP + 1;
constexpr UINT_PTR kToastTimer = 1;

enum Button {
    B_LEFT,
    B_MIDDLE,
    B_RIGHT,
    B_X1,
    B_X2,
    B_WHEEL_UP,
    B_WHEEL_DOWN,
    B_COUNT
};
constexpr int kRealButtons = B_WHEEL_UP;

const wchar_t* const kTriggerKeys[B_COUNT] = {
    L"left", L"middle", L"right", L"x1", L"x2", L"wheelUp", L"wheelDown"};

enum Action {
    ACT_NONE,
    ACT_CLOSE,
    ACT_MINIMIZE,
    ACT_MAXIMIZE,
    ACT_TOGGLE_MAX,
    ACT_RESTORE,
    ACT_TOPMOST,
    ACT_SNAP_LEFT,
    ACT_SNAP_RIGHT,
    ACT_MOVE_MONITOR,
    ACT_NEXT_TRACK,
    ACT_PREV_TRACK,
    ACT_PLAY_PAUSE,
    ACT_VOL_UP,
    ACT_VOL_DOWN,
    ACT_MUTE,
    ACT_SHOW_DESKTOP,
    ACT_TASK_VIEW,
    ACT_DESKTOP_PREV,
    ACT_DESKTOP_NEXT,
    ACT_SCREENSHOT,
    ACT_COUNT
};

// UI languages. Order of the name arrays below: en, tr, de, fr, ru.
enum Lang { L_EN, L_TR, L_DE, L_FR, L_RU, L_COUNT };

struct ActionInfo {
    const wchar_t* key;
    const wchar_t* names[L_COUNT];
};

// Order must match enum Action. Keep in sync with the settings block.
const ActionInfo kActionInfo[ACT_COUNT] = {
    {L"none",
     {L"None (do nothing)", L"Yok (hiçbir şey yapma)", L"Keine (nichts tun)",
      L"Aucune (ne rien faire)", L"Нет (ничего не делать)"}},
    {L"close",
     {L"Close window", L"Pencereyi kapat", L"Fenster schließen",
      L"Fermer la fenêtre", L"Закрыть окно"}},
    {L"minimize",
     {L"Minimize window", L"Pencereyi simge durumuna küçült", L"Fenster minimieren",
      L"Réduire la fenêtre", L"Свернуть окно"}},
    {L"maximize",
     {L"Maximize window", L"Pencereyi büyüt", L"Fenster maximieren",
      L"Agrandir la fenêtre", L"Развернуть окно"}},
    {L"toggleMaximize",
     {L"Maximize / restore (toggle)", L"Büyüt / eski boyuta getir (değiştir)",
      L"Maximieren / Wiederherstellen (umschalten)",
      L"Agrandir / restaurer (basculer)",
      L"Развернуть / восстановить (переключить)"}},
    {L"restore",
     {L"Restore window", L"Pencereyi eski boyutuna getir", L"Fenster wiederherstellen",
      L"Restaurer la fenêtre", L"Восстановить окно"}},
    {L"alwaysOnTop",
     {L"Toggle always on top", L"Her zaman üstte (aç/kapat)",
      L"Immer im Vordergrund (umschalten)", L"Toujours au premier plan (basculer)",
      L"Поверх всех окон (переключить)"}},
    {L"snapLeft",
     {L"Snap window to the left", L"Pencereyi sola yasla", L"Fenster links andocken",
      L"Ancrer la fenêtre à gauche", L"Прикрепить окно слева"}},
    {L"snapRight",
     {L"Snap window to the right", L"Pencereyi sağa yasla", L"Fenster rechts andocken",
      L"Ancrer la fenêtre à droite", L"Прикрепить окно справа"}},
    {L"moveMonitor",
     {L"Move window to next monitor", L"Pencereyi sonraki monitöre taşı",
      L"Fenster auf nächsten Monitor verschieben",
      L"Déplacer la fenêtre vers l'écran suivant",
      L"Переместить окно на следующий монитор"}},
    {L"nextTrack",
     {L"Next track", L"Sonraki şarkı", L"Nächster Titel", L"Piste suivante",
      L"Следующий трек"}},
    {L"prevTrack",
     {L"Previous track", L"Önceki şarkı", L"Vorheriger Titel", L"Piste précédente",
      L"Предыдущий трек"}},
    {L"playPause",
     {L"Play / pause", L"Oynat / duraklat", L"Wiedergabe / Pause", L"Lecture / pause",
      L"Воспроизведение / пауза"}},
    {L"volumeUp",
     {L"Volume up", L"Sesi artır", L"Lauter", L"Augmenter le volume",
      L"Громкость выше"}},
    {L"volumeDown",
     {L"Volume down", L"Sesi azalt", L"Leiser", L"Baisser le volume",
      L"Громкость ниже"}},
    {L"mute",
     {L"Mute / unmute", L"Sesi kapat / aç", L"Stumm / Ton an",
      L"Couper / rétablir le son", L"Выключить / включить звук"}},
    {L"showDesktop",
     {L"Show desktop", L"Masaüstünü göster", L"Desktop anzeigen",
      L"Afficher le bureau", L"Показать рабочий стол"}},
    {L"taskView",
     {L"Task View", L"Görev Görünümü", L"Aufgabenansicht", L"Vue des tâches",
      L"Представление задач"}},
    {L"desktopPrev",
     {L"Previous virtual desktop", L"Önceki sanal masaüstü",
      L"Vorheriger virtueller Desktop", L"Bureau virtuel précédent",
      L"Предыдущий виртуальный рабочий стол"}},
    {L"desktopNext",
     {L"Next virtual desktop", L"Sonraki sanal masaüstü",
      L"Nächster virtueller Desktop", L"Bureau virtuel suivant",
      L"Следующий виртуальный рабочий стол"}},
    {L"screenshot",
     {L"Screenshot (Snipping Tool)", L"Ekran görüntüsü (Ekran Alıntısı Aracı)",
      L"Screenshot (Snipping Tool)", L"Capture d'écran (Outil Capture d'écran)",
      L"Снимок экрана (Ножницы)"}},
};

const wchar_t* const kOn[L_COUNT] = {L": On", L": Açık", L": An", L": Activé",
                                     L": Вкл."};
const wchar_t* const kOff[L_COUNT] = {L": Off", L": Kapalı", L": Aus", L": Désactivé",
                                      L": Выкл."};

// ---------------------------------------------------------------------------
// Settings / language
// ---------------------------------------------------------------------------

std::atomic<int> g_hold{B_RIGHT};
std::atomic<bool> g_showToast{true};
std::atomic<bool> g_ignoreFullscreen{true};
std::atomic<int> g_actions[B_COUNT];

int g_lang = L_EN;  // chosen automatically from the Windows UI language

// Uses the Windows display language; falls back to English when unsupported.
void DetectLanguage() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_TURKISH: g_lang = L_TR; break;
        case LANG_GERMAN: g_lang = L_DE; break;
        case LANG_FRENCH: g_lang = L_FR; break;
        case LANG_RUSSIAN: g_lang = L_RU; break;
        default: g_lang = L_EN; break;
    }
}

const wchar_t* ActionName(int a) {
    if (a < 0 || a >= ACT_COUNT) {
        return L"";
    }
    return kActionInfo[a].names[g_lang];
}

std::wstring GetStringSetting(const std::wstring& name) {
    PCWSTR value = Wh_GetStringSetting(name.c_str());
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

int ParseAction(const std::wstring& key) {
    for (int i = 0; i < ACT_COUNT; i++) {
        if (key == kActionInfo[i].key) {
            return i;
        }
    }
    return ACT_NONE;
}

void LoadSettings() {
    std::wstring hold = GetStringSetting(L"holdButton");
    if (hold == L"back") {
        g_hold = B_X1;
    } else if (hold == L"forward") {
        g_hold = B_X2;
    } else {
        g_hold = B_RIGHT;
    }

    g_showToast = Wh_GetIntSetting(L"showToast") != 0;
    g_ignoreFullscreen = Wh_GetIntSetting(L"ignoreFullscreen") != 0;

    for (int i = 0; i < B_COUNT; i++) {
        g_actions[i] =
            ParseAction(GetStringSetting(std::wstring(L"actions.") + kTriggerKeys[i]));
    }
}

// ---------------------------------------------------------------------------
// Action execution
// ---------------------------------------------------------------------------

bool IsExtendedKey(WORD vk) {
    return vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN ||
           vk == VK_LWIN || vk == VK_RWIN;
}

void SendKeys(const std::vector<WORD>& keys) {
    std::vector<INPUT> inputs;
    for (size_t i = 0; i < keys.size(); i++) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = keys[i];
        in.ki.wScan = (WORD)MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC);
        in.ki.dwFlags = IsExtendedKey(keys[i]) ? KEYEVENTF_EXTENDEDKEY : 0;
        inputs.push_back(in);
    }
    for (size_t i = keys.size(); i > 0; i--) {
        INPUT in = {};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = keys[i - 1];
        in.ki.wScan = (WORD)MapVirtualKeyW(keys[i - 1], MAPVK_VK_TO_VSC);
        in.ki.dwFlags =
            KEYEVENTF_KEYUP | (IsExtendedKey(keys[i - 1]) ? KEYEVENTF_EXTENDEDKEY : 0);
        inputs.push_back(in);
    }
    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

bool IsShellWindow(HWND hwnd) {
    wchar_t cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
           wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

// Window actions always target the window under the mouse cursor.
HWND GetTargetWindow(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return nullptr;
    }
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd || IsShellWindow(hwnd)) {
        return nullptr;
    }
    return hwnd;
}

// Runs the action; returns the text for the toast (empty = nothing to show).
std::wstring RunAction(int action, POINT pt) {
    if (action <= ACT_NONE || action >= ACT_COUNT) {
        return L"";
    }

    std::wstring label = ActionName(action);

    switch (action) {
        case ACT_CLOSE:
        case ACT_MINIMIZE:
        case ACT_MAXIMIZE:
        case ACT_TOGGLE_MAX:
        case ACT_RESTORE:
        case ACT_TOPMOST: {
            HWND hwnd = GetTargetWindow(pt);
            if (!hwnd) {
                return L"";
            }
            if (action == ACT_TOPMOST) {
                bool top = (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
                SetWindowPos(hwnd, top ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                 SWP_ASYNCWINDOWPOS);
                label += top ? kOff[g_lang] : kOn[g_lang];
            } else {
                WPARAM cmd = SC_RESTORE;
                if (action == ACT_CLOSE) {
                    cmd = SC_CLOSE;
                } else if (action == ACT_MINIMIZE) {
                    cmd = SC_MINIMIZE;
                } else if (action == ACT_MAXIMIZE) {
                    cmd = SC_MAXIMIZE;
                } else if (action == ACT_TOGGLE_MAX) {
                    cmd = IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE;
                }
                PostMessageW(hwnd, WM_SYSCOMMAND, cmd, 0);
            }
            break;
        }
        case ACT_SNAP_LEFT:
        case ACT_SNAP_RIGHT:
        case ACT_MOVE_MONITOR: {
            // Win+Arrow shortcuts act on the active window, so activate the
            // window under the cursor first.
            HWND hwnd = GetTargetWindow(pt);
            if (!hwnd) {
                return L"";
            }
            if (GetForegroundWindow() != hwnd) {
                // Alt tap lets SetForegroundWindow succeed from this process.
                keybd_event(VK_MENU, 0, 0, 0);
                keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
                SetForegroundWindow(hwnd);
            }
            if (action == ACT_SNAP_LEFT) {
                SendKeys({VK_LWIN, VK_LEFT});
            } else if (action == ACT_SNAP_RIGHT) {
                SendKeys({VK_LWIN, VK_RIGHT});
            } else {
                SendKeys({VK_LWIN, VK_SHIFT, VK_RIGHT});
            }
            break;
        }
        case ACT_NEXT_TRACK:
            SendKeys({VK_MEDIA_NEXT_TRACK});
            break;
        case ACT_PREV_TRACK:
            SendKeys({VK_MEDIA_PREV_TRACK});
            break;
        case ACT_PLAY_PAUSE:
            SendKeys({VK_MEDIA_PLAY_PAUSE});
            break;
        case ACT_VOL_UP:
            SendKeys({VK_VOLUME_UP});
            break;
        case ACT_VOL_DOWN:
            SendKeys({VK_VOLUME_DOWN});
            break;
        case ACT_MUTE:
            SendKeys({VK_VOLUME_MUTE});
            break;
        case ACT_SHOW_DESKTOP:
            SendKeys({VK_LWIN, 'D'});
            break;
        case ACT_TASK_VIEW:
            SendKeys({VK_LWIN, VK_TAB});
            break;
        case ACT_DESKTOP_PREV:
            SendKeys({VK_CONTROL, VK_LWIN, VK_LEFT});
            break;
        case ACT_DESKTOP_NEXT:
            SendKeys({VK_CONTROL, VK_LWIN, VK_RIGHT});
            break;
        case ACT_SCREENSHOT:
            SendKeys({VK_LWIN, VK_SHIFT, 'S'});
            break;
        default:
            return L"";
    }

    return label;
}

// ---------------------------------------------------------------------------
// Toast (small on-screen label next to the cursor)
// ---------------------------------------------------------------------------

HWND g_msgWnd = nullptr;
HWND g_toastWnd = nullptr;
HFONT g_toastFont = nullptr;
std::wstring g_toastText;

LRESULT CALLBACK ToastWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_TIMER:
            if (wParam == kToastTimer) {
                KillTimer(hwnd, kToastTimer);
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH brush = CreateSolidBrush(RGB(32, 32, 32));
            FillRect(dc, &rc, brush);
            DeleteObject(brush);
            HGDIOBJ oldFont = SelectObject(dc, g_toastFont);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(255, 255, 255));
            DrawTextW(dc, g_toastText.c_str(), -1, &rc,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(dc, oldFont);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowToast(const std::wstring& text, POINT pt) {
    if (!g_toastWnd) {
        return;
    }

    g_toastText = text;

    HDC dc = GetDC(g_toastWnd);
    int dpi = GetDeviceCaps(dc, LOGPIXELSY);

    HFONT newFont = CreateFontW(-MulDiv(11, dpi, 72), 0, 0, 0, FW_SEMIBOLD, FALSE,
                                FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    if (g_toastFont) {
        DeleteObject(g_toastFont);
    }
    g_toastFont = newFont;

    HGDIOBJ oldFont = SelectObject(dc, g_toastFont);
    RECT textRect = {};
    DrawTextW(dc, text.c_str(), -1, &textRect,
              DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, oldFont);
    ReleaseDC(g_toastWnd, dc);

    int w = (textRect.right - textRect.left) + MulDiv(28, dpi, 96);
    int h = (textRect.bottom - textRect.top) + MulDiv(16, dpi, 96);

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);

    int x = pt.x + MulDiv(18, dpi, 96);
    int y = pt.y + MulDiv(22, dpi, 96);
    if (x + w > mi.rcWork.right) {
        x = mi.rcWork.right - w;
    }
    if (y + h > mi.rcWork.bottom) {
        y = pt.y - h - MulDiv(10, dpi, 96);
    }
    x = std::max<int>(x, mi.rcWork.left);
    y = std::max<int>(y, mi.rcWork.top);

    int radius = MulDiv(10, dpi, 96);
    SetWindowRgn(g_toastWnd, CreateRoundRectRgn(0, 0, w + 1, h + 1, radius, radius),
                 TRUE);
    SetWindowPos(g_toastWnd, HWND_TOPMOST, x, y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_toastWnd, nullptr, TRUE);
    SetTimer(g_toastWnd, kToastTimer, 900, nullptr);
}

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_RUN_ACTION) {
        int trigger = (int)wParam;
        if (trigger < 0 || trigger >= B_COUNT) {
            return 0;
        }
        POINT pt = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
        std::wstring label = RunAction(g_actions[trigger].load(), pt);
        if (g_showToast && !label.empty()) {
            ShowToast(label, pt);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Mouse hook
// ---------------------------------------------------------------------------

HHOOK g_hook = nullptr;

// Hook-thread state.
bool g_holdActive = false;     // hold button is down and was swallowed
bool g_chordUsed = false;      // at least one chord action fired
bool g_passedThrough = false;  // hold press was replayed (drag), chords disabled
POINT g_holdOrigin = {};
DWORD g_lastActivity = 0;
int g_wheelAccum = 0;
bool g_swallowedDown[kRealButtons] = {};

void FillMouseInput(INPUT& in, DWORD flags, DWORD data = 0) {
    in = {};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = flags;
    in.mi.mouseData = data;
    in.mi.dwExtraInfo = kMarker;
}

void FillMoveInput(INPUT& in, POINT p) {
    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = std::max(2, (int)GetSystemMetrics(SM_CXVIRTUALSCREEN));
    int vh = std::max(2, (int)GetSystemMetrics(SM_CYVIRTUALSCREEN));
    FillMouseInput(in, MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK);
    in.mi.dx = MulDiv(p.x - vx, 65535, vw - 1);
    in.mi.dy = MulDiv(p.y - vy, 65535, vh - 1);
}

void GetHoldFlags(int hold, DWORD& downFlag, DWORD& upFlag, DWORD& data) {
    if (hold == B_RIGHT) {
        downFlag = MOUSEEVENTF_RIGHTDOWN;
        upFlag = MOUSEEVENTF_RIGHTUP;
        data = 0;
    } else {
        downFlag = MOUSEEVENTF_XDOWN;
        upFlag = MOUSEEVENTF_XUP;
        data = hold == B_X1 ? XBUTTON1 : XBUTTON2;
    }
}

// No chord happened: give the app the click it would normally have received.
void ReplayHoldClick(int hold) {
    DWORD downFlag, upFlag, data;
    GetHoldFlags(hold, downFlag, upFlag, data);
    INPUT in[2];
    FillMouseInput(in[0], downFlag, data);
    FillMouseInput(in[1], upFlag, data);
    SendInput(2, in, sizeof(INPUT));
}

// The user started dragging with the held button: replay the press at the
// original position so right-button drags keep working.
void ReplayHoldDrag(int hold, POINT origin, POINT current) {
    DWORD downFlag, upFlag, data;
    GetHoldFlags(hold, downFlag, upFlag, data);
    INPUT in[3];
    FillMoveInput(in[0], origin);
    FillMouseInput(in[1], downFlag, data);
    FillMoveInput(in[2], current);
    SendInput(3, in, sizeof(INPUT));
}

bool IsFullscreenForeground() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == GetDesktopWindow() || hwnd == GetShellWindow() ||
        IsShellWindow(hwnd)) {
        return false;
    }
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
        return false;
    }
    bool coversMonitor = rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
                         rc.right >= mi.rcMonitor.right &&
                         rc.bottom >= mi.rcMonitor.bottom;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    return coversMonitor && (style & WS_CAPTION) != WS_CAPTION;
}

void ResetHoldState() {
    g_holdActive = false;
    g_chordUsed = false;
    g_passedThrough = false;
    g_wheelAccum = 0;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;
    if (ms->dwExtraInfo == kMarker) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    int btn = -1;
    bool isDown = false;
    bool isUp = false;
    int wheelDelta = 0;

    switch (wParam) {
        case WM_LBUTTONDOWN: btn = B_LEFT; isDown = true; break;
        case WM_LBUTTONUP: btn = B_LEFT; isUp = true; break;
        case WM_MBUTTONDOWN: btn = B_MIDDLE; isDown = true; break;
        case WM_MBUTTONUP: btn = B_MIDDLE; isUp = true; break;
        case WM_RBUTTONDOWN: btn = B_RIGHT; isDown = true; break;
        case WM_RBUTTONUP: btn = B_RIGHT; isUp = true; break;
        case WM_XBUTTONDOWN:
            btn = HIWORD(ms->mouseData) == XBUTTON1 ? B_X1 : B_X2;
            isDown = true;
            break;
        case WM_XBUTTONUP:
            btn = HIWORD(ms->mouseData) == XBUTTON1 ? B_X1 : B_X2;
            isUp = true;
            break;
        case WM_MOUSEWHEEL:
            wheelDelta = (short)HIWORD(ms->mouseData);
            break;
        case WM_MOUSEMOVE:
            break;
        default:
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    DWORD now = GetTickCount();
    const int hold = g_hold.load();

    // Drop a stale hold (e.g. the release happened on the secure desktop).
    if (g_holdActive && now - g_lastActivity > 30000) {
        ResetHoldState();
    }

    // Swallow the release of a button whose press we consumed.
    if (isUp && btn < kRealButtons && g_swallowedDown[btn]) {
        g_swallowedDown[btn] = false;
        return 1;
    }

    // The hold button itself.
    if (btn == hold) {
        if (isDown) {
            if (g_holdActive) {
                return 1;
            }
            if (g_ignoreFullscreen && IsFullscreenForeground()) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
            ResetHoldState();
            g_holdActive = true;
            g_holdOrigin = ms->pt;
            g_lastActivity = now;
            return 1;
        }
        if (isUp) {
            if (!g_holdActive) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
            bool passed = g_passedThrough;
            bool used = g_chordUsed;
            ResetHoldState();
            if (passed) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
            if (!used) {
                ReplayHoldClick(hold);
            }
            return 1;
        }
    }

    if (g_holdActive && !g_passedThrough) {
        g_lastActivity = now;

        if (wParam == WM_MOUSEMOVE) {
            if (hold == B_RIGHT && !g_chordUsed) {
                int threshold = std::max(8, GetSystemMetrics(SM_CXDRAG) * 2);
                int dx = ms->pt.x - g_holdOrigin.x;
                int dy = ms->pt.y - g_holdOrigin.y;
                if (dx * dx + dy * dy > threshold * threshold) {
                    g_passedThrough = true;
                    ReplayHoldDrag(hold, g_holdOrigin, ms->pt);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        if (wParam == WM_MOUSEWHEEL) {
            g_chordUsed = true;
            g_wheelAccum += wheelDelta;
            int trigger = -1;
            if (g_wheelAccum >= WHEEL_DELTA) {
                trigger = B_WHEEL_UP;
                g_wheelAccum = 0;
            } else if (g_wheelAccum <= -WHEEL_DELTA) {
                trigger = B_WHEEL_DOWN;
                g_wheelAccum = 0;
            }
            if (trigger >= 0 && g_msgWnd) {
                PostMessageW(g_msgWnd, WM_RUN_ACTION, trigger,
                             MAKELPARAM((WORD)ms->pt.x, (WORD)ms->pt.y));
            }
            return 1;
        }

        if (isDown && btn >= 0 && btn != hold) {
            g_swallowedDown[btn] = true;
            g_chordUsed = true;
            if (g_msgWnd) {
                PostMessageW(g_msgWnd, WM_RUN_ACTION, btn,
                             MAKELPARAM((WORD)ms->pt.x, (WORD)ms->pt.y));
            }
            return 1;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------

constexpr wchar_t kMsgClass[] = L"WH_SuperMouseShortcuts_Msg";
constexpr wchar_t kToastClass[] = L"WH_SuperMouseShortcuts_Toast";

HANDLE g_thread = nullptr;
DWORD g_threadId = 0;
HANDLE g_readyEvent = nullptr;

DWORD WINAPI WorkerThread(LPVOID) {
    // Use physical pixel coordinates for the hook and for SendInput.
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContext_t)(HANDLE);
    auto setDpi = (SetThreadDpiAwarenessContext_t)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext");
    if (setDpi) {
        setDpi((HANDLE)-4);  // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    }

    HINSTANCE inst = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = kMsgClass;
    RegisterClassW(&wc);

    WNDCLASSW tc = {};
    tc.lpfnWndProc = ToastWndProc;
    tc.hInstance = inst;
    tc.lpszClassName = kToastClass;
    tc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&tc);

    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, inst, nullptr);
    g_toastWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED |
            WS_EX_TRANSPARENT,
        kToastClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (g_toastWnd) {
        SetLayeredWindowAttributes(g_toastWnd, 0, 235, LWA_ALPHA);
    }

    g_threadId = GetCurrentThreadId();
    g_hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, inst, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
    }
    SetEvent(g_readyEvent);

    if (g_hook) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }

    if (g_toastWnd) {
        DestroyWindow(g_toastWnd);
        g_toastWnd = nullptr;
    }
    if (g_msgWnd) {
        DestroyWindow(g_msgWnd);
        g_msgWnd = nullptr;
    }
    if (g_toastFont) {
        DeleteObject(g_toastFont);
        g_toastFont = nullptr;
    }
    UnregisterClassW(kToastClass, inst);
    UnregisterClassW(kMsgClass, inst);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    DetectLanguage();
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, 5000);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;

    if (!g_hook) {
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    ResetHoldState();
}

void WhTool_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
