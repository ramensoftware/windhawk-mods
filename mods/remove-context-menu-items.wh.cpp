// ==WindhawkMod==
// @id              remove-context-menu-items
// @name            Remove Context Menu Items
// @description     Removes unwanted items from file context menus with configurable options and context-aware filtering
// @version         1.14.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Context Menu Items

⚠️ **Windows 11 Users:** This mod currently works only with the classic context menu. Please install the [Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic) mod to use this mod.

---

This mod removes unwanted items from file context menus with configurable options. It hooks into the context menu creation process and removes unwanted menu items by checking their text labels before they are displayed. It also automatically cleans up duplicate separator lines.

## Screenshots

**Before and after right-clicking on an empty space:**

![Before and after on empty space](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/Don't%20Touch/Blank%20Space.png)

**Before and after right-clicking a video file:**

![Before and after on video file](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/Don't%20Touch/Video%20File.png)

## Features

Clean up your Windows context menus by removing bloatware and unwanted items:

| Bloatware Items                | Basic Items                 | App-specific Items                   |
|--------------------------------|-----------------------------|--------------------------------------|
| `Move to OneDrive`             | `Open`                      | `Add to VLC media player's Playlist` |
| `Ask Copilot`                  | `Cast to Device`            | `Edit in Notepad`                    |
| `Scan with Microsoft Defender` | `Include in library`        | `Edit with Paint`                    |
| `Create with Designer`         | `Restore previous versions` | `NVIDIA App`                         |
| ...and more!                   | ...and more!                | ...and more!                         |

### Context-Aware Filtering
Some items like `Edit in Notepad` and `WinRAR` can be filtered based on file extensions. For example, you can configure the mod to only show `Edit in Notepad` for text files or `WinRAR` items for archives, hiding them for all other file types.

### Modifier Key Override
Hold `Ctrl` or `Alt` while right-clicking to temporarily bypass the mod and see all original context menu items. Useful when you need access to a hidden item without changing settings.

### Custom Items
You can also add your own custom menu items to remove by entering their text in the settings.

**Basic Usage (Exact Match):**
- `Copy` - Removes the "Copy" option (exact match)
- `Pin to Quick access` - Removes the "Pin to Quick access" option (exact match)
- `Open` - Removes only "Open" (exact match, won't remove "Open in new tab")

**Wildcard Usage (Prefix Match):**
Add an asterisk (*) at the end to match items that start with the given text:
- `Open*` - Removes all menu items that start with "Open":
  - Open
  - Open with...
  - Open in new tab
  - Open in Terminal (and more)
- `Pin to*` - Removes all menu items that start with "Pin to":
  - Pin to Start
  - Pin to Quick access (and more)
- `C*` - Removes all menu items that start with "C":
  - Cut
  - Copy
  - Create shortcut (and more, so be careful!)

> **Tip:** Right-click a file/folder, note the exact text of the menu item you want to remove, then add it to Custom Items. Use the asterisk (*) only if you want to remove multiple items with the same prefix.

## Supported Languages

- `cs-CZ`
- `de-DE` (added by [Schleifenkratzer](https://github.com/Schleifenkratzer))
- `en-AU`, `en-GB`
- `es-MX` (added by [frankh93](https://github.com/frankh93))
- `fr-FR` (added by [Catif](https://github.com/Catif))
- `pl-PL` (added by [FadeMind](https://github.com/FadeMind))
- `pt-BR`, `pt-PT`
- `ru-RU` (added by [VitalityV1nT](https://github.com/VitalityV1nT))
- `tr-TR` (added by [bcrtvkcs](https://github.com/bcrtvkcs))
- `ja-JP` (added by [haru612](https://github.com/haru612))

If you find a mistake and for additional details, please click [here](https://github.com/armaninyow/Remove-Context-Menu-Items).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- bloatwareItems:
  - removeOneDrive: true
    $name: Move to OneDrive
    $name:cs-CZ: "Přesunout na OneDrive"
    $name:de-DE: "Auf OneDrive verschieben"
    $name:es-MX: "Mover a OneDrive"
    $name:fr-FR: "Déplacer vers OneDrive"
    $name:ja-JP: "OneDrive に移動"
    $name:pl-PL: "Przenieś do usługi OneDrive"
    $name:pt-BR: "Mover para o OneDrive"
    $name:pt-PT: "Mover para o OneDrive"
    $name:tr-TR: "OneDrive'a taşı"
  - removeCopilot: true
    $name: Ask Copilot
    $name:cs-CZ: "Zeptat se Copilota"
    $name:de-DE: "Copilot fragen"
    $name:es-MX: "Preguntar a Copilot"
    $name:fr-FR: "Demander à Copilot"
    $name:ja-JP: "Copilot とチャットする"
    $name:pl-PL: "Zapytaj aplikację Copilot"
    $name:pt-BR: "Perguntar ao Copilot"
    $name:pt-PT: "Perguntar ao Copilot"
    $name:tr-TR: "Copilot'a sor"
  - removeDefender: true
    $name: Scan with Microsoft Defender...
    $name:cs-CZ: "Prohledat pomocí programu Microsoft Defender..."
    $name:de-DE: "Mit Microsoft Defender überprüfen..."
    $name:es-MX: "Analizar con Microsoft Defender..."
    $name:fr-FR: "Analyser avec Microsoft Defender..."
    $name:ja-JP: "Microsoft Defenderでスキャンする..."
    $name:pl-PL: "Skanuj za pomocą programu Microsoft Defender..."
    $name:pt-BR: "Verificar com o Microsoft Defender..."
    $name:pt-PT: "Analisar com o Microsoft Defender..."
    $name:ru-RU: "Проверка с использованием Microsoft Defender..."
    $name:tr-TR: "Microsoft Defender ile tara..."
  - removeDesigner: true
    $name: Create with Designer
    $name:cs-CZ: "Vytvořit pomocí Designeru"
    $name:de-DE: "Mit Designer erstellen"
    $name:es-MX: "Crear con Designer"
    $name:fr-FR: "Créer avec Designer"
    $name:ja-JP: "Designerで作成"
    $name:pl-PL: "Utwórz za pomocą aplikacji Designer"
    $name:pt-BR: "Criar com o Designer"
    $name:pt-PT: "Criar com o Designer"
    $name:tr-TR: "Designer ile oluştur"
  - removeClipchamp: true
    $name: Edit with Clipchamp
    $name:cs-CZ: "Upravit pomocí Clipchampu"
    $name:de-DE: "Mit Clipchamp bearbeiten"
    $name:es-MX: "Editar con Clipchamp"
    $name:fr-FR: "Modifier avec Clipchamp"
    $name:ja-JP: "Clipchampで編集"
    $name:pl-PL: "Edytuj za pomocą aplikacji Clipchamp"
    $name:pt-BR: "Editar com o Clipchamp"
    $name:pt-PT: "Editar com o Clipchamp"
    $name:ru-RU: "Редактировать в Clipchamp"
    $name:tr-TR: "Clipchamp ile düzenle"
  - removeMicrosoft365Copilot: true
    $name: Ask Microsoft 365 Copilot
    $name:cs-CZ: "Zeptat se Microsoft 365 Copilota"
    $name:de-DE: "Microsoft 365 Copilot fragen"
    $name:es-MX: "Preguntar a Microsoft 365 Copilot"
    $name:fr-FR: "Demander à Microsoft 365 Copilot"
    $name:pl-PL: "Zapytaj aplikację Microsoft 365 Copilot"
    $name:pt-BR: "Perguntar ao Microsoft 365 Copilot"
    $name:pt-PT: "Perguntar ao Microsoft 365 Copilot"
    $name:tr-TR: "Microsoft 365 Copilot'a sor"
  $name: Bloatware Items
  $description: Enabled by default

- basicItems:
  - removeOpen: false
    $name: Open
    $name:cs-CZ: "Otevřít"
    $name:de-DE: "Öffnen"
    $name:es-MX: "Abrir"
    $name:fr-FR: "Ouvrir"
    $name:ja-JP: "開く"
    $name:pl-PL: "Otwórz"
    $name:pt-BR: "Abrir"
    $name:pt-PT: "Abrir"
    $name:ru-RU: "Открыть"
    $name:tr-TR: "Aç"
  - removeOpenWith: false
    $name: Open with
    $name:cs-CZ: "Otevřít v aplikaci"
    $name:de-DE: "Öffnen mit"
    $name:es-MX: "Abrir con"
    $name:fr-FR: "Ouvrir avec"
    $name:ja-JP: "プログラムから開く"
    $name:pl-PL: "Otwórz za pomocą"
    $name:pt-BR: "Abrir com"
    $name:pt-PT: "Abrir com"
    $name:ru-RU: "Открыть с помощью"
    $name:tr-TR: "Birlikte aç"
  - removeCut: false
    $name: Cut
    $name:cs-CZ: "Vyjmout"
    $name:de-DE: "Ausschneiden"
    $name:es-MX: "Cortar"
    $name:fr-FR: "Couper"
    $name:ja-JP: "切り取り"
    $name:pl-PL: "Wytnij"
    $name:pt-BR: "Recortar"
    $name:pt-PT: "Cortar"
    $name:ru-RU: "Вырезать"
    $name:tr-TR: "Kes"
  - removeCopy: false
    $name: Copy
    $name:cs-CZ: "Kopírovat"
    $name:de-DE: "Kopieren"
    $name:es-MX: "Copiar"
    $name:fr-FR: "Copier"
    $name:ja-JP: "コピー"
    $name:pl-PL: "Kopiuj"
    $name:pt-BR: "Copiar"
    $name:pt-PT: "Copiar"
    $name:ru-RU: "Копировать"
    $name:tr-TR: "Kopyala"
  - removeCreateShortcut: false
    $name: Create shortcut
    $name:cs-CZ: "Vytvořit zástupce"
    $name:de-DE: "Verknüpfung erstellen"
    $name:es-MX: "Crear acceso directo"
    $name:fr-FR: "Créer un raccourci"
    $name:ja-JP: "ショートカットの作成"
    $name:pl-PL: "Utwórz skrót"
    $name:pt-BR: "Criar atalho"
    $name:pt-PT: "Criar atalho"
    $name:ru-RU: "Создать ярлык"
    $name:tr-TR: "Kısayol oluştur"
  - removeDelete: false
    $name: Delete
    $name:cs-CZ: "Odstranit"
    $name:de-DE: "Löschen"
    $name:es-MX: "Eliminar"
    $name:fr-FR: "Supprimer"
    $name:ja-JP: "削除"
    $name:pl-PL: "Usuń"
    $name:pt-BR: "Excluir"
    $name:pt-PT: "Eliminar"
    $name:ru-RU: "Удалить"
    $name:tr-TR: "Sil"
  - removeRename: false
    $name: Rename
    $name:cs-CZ: "Přejmenovat"
    $name:de-DE: "Umbenennen"
    $name:es-MX: "Cambiar nombre"
    $name:fr-FR: "Renommer"
    $name:ja-JP: "名前の変更"
    $name:pl-PL: "Zmień nazwę"
    $name:pt-BR: "Renomear"
    $name:pt-PT: "Mudar o nome"
    $name:ru-RU: "Переименовать"
    $name:tr-TR: "Yeniden adlandır"
  - removeSendTo: false
    $name: Send to
    $name:cs-CZ: "Odeslat do"
    $name:de-DE: "Senden an"
    $name:es-MX: "Enviar a"
    $name:fr-FR: "Envoyer vers"
    $name:ja-JP: "送る"
    $name:pl-PL: "Wyślij do"
    $name:pt-BR: "Enviar para"
    $name:pt-PT: "Enviar para"
    $name:ru-RU: "Отправить"
    $name:tr-TR: "Gönder"
  - removeOpenInNewTab: false
    $name: Open in new tab
    $name:cs-CZ: "Otevřít na nové kartě"
    $name:de-DE: "In neuer Registerkarte öffnen"
    $name:es-MX: "Abrir en nueva pestaña"
    $name:fr-FR: "Ouvrir dans un nouvel onglet"
    $name:ja-JP: "新しいタブで開く"
    $name:pl-PL: "Otwórz w nowej karcie"
    $name:pt-BR: "Abrir em uma nova guia"
    $name:pt-PT: "Abrir num novo separador"
    $name:ru-RU: "Открыть в новой вкладке"
    $name:tr-TR: "Yeni sekmede aç"
  - removeOpenInNewWindow: false
    $name: Open in new window
    $name:cs-CZ: "Otevřít v novém okně"
    $name:de-DE: "In neuem Fenster öffnen"
    $name:es-MX: "Abrir en nueva ventana"
    $name:fr-FR: "Ouvrir dans une nouvelle fenêtre"
    $name:ja-JP: "新しいウィンドウで開く"
    $name:pl-PL: "Otwórz w nowym oknie"
    $name:pt-BR: "Abrir em uma nova janela"
    $name:pt-PT: "Abrir numa nova janela"
    $name:ru-RU: "Открыть в новом окне"
    $name:tr-TR: "Yeni pencerede aç"
  - removeEdit: false
    $name: Edit
    $name:cs-CZ: "Upravit"
    $name:de-DE: "Bearbeiten"
    $name:es-MX: "Editar"
    $name:fr-FR: "Modifier"
    $name:ja-JP: "編集"
    $name:pl-PL: "Edytuj"
    $name:pt-BR: "Editar"
    $name:pt-PT: "Editar"
    $name:ru-RU: "Изменить"
    $name:tr-TR: "Düzenle"
  - removePlay: false
    $name: Play
    $name:cs-CZ: "Přehrát"
    $name:de-DE: "Wiedergabe"
    $name:es-MX: "Reproducir"
    $name:fr-FR: "Lire"
    $name:ja-JP: "再生"
    $name:pl-PL: "Odtwarzaj"
    $name:pt-BR: "Reproduzir"
    $name:pt-PT: "Reproduzir"
    $name:ru-RU: "Воспроизвести"
    $name:tr-TR: "Oynat"
  - removePreview: false
    $name: Preview
    $name:cs-CZ: "Náhled"
    $name:de-DE: "Vorschau"
    $name:es-MX: "Vista previa"
    $name:fr-FR: "Aperçu"
    $name:ja-JP: "プレビュー"
    $name:pl-PL: "Podgląd"
    $name:pt-BR: "Visualizar"
    $name:pt-PT: "Pré-visualizar"
    $name:tr-TR: "Önizleme"
  - removePrint: false
    $name: Print
    $name:cs-CZ: "Tisk"
    $name:de-DE: "Drucken"
    $name:es-MX: "Imprimir"
    $name:fr-FR: "Imprimer"
    $name:ja-JP: "印刷"
    $name:pl-PL: "Drukuj"
    $name:pt-BR: "Imprimir"
    $name:pt-PT: "Imprimir"
    $name:ru-RU: "Печать"
    $name:tr-TR: "Yazdır"
  - removeShare: false
    $name: Share
    $name:cs-CZ: "Sdílet"
    $name:de-DE: "Freigabe"
    $name:es-MX: "Compartir"
    $name:fr-FR: "Partager"
    $name:ja-JP: "共有"
    $name:pl-PL: "Udostępnij"
    $name:pt-BR: "Compartilhar"
    $name:pt-PT: "Partilhar"
    $name:ru-RU: "Поделиться"
    $name:tr-TR: "Paylaş"
  - removeRefresh: false
    $name: Refresh
    $name:cs-CZ: "Aktualizovat"
    $name:de-DE: "Aktualisieren"
    $name:es-MX: "Actualizar"
    $name:fr-FR: "Actualiser"
    $name:ja-JP: "最新の情報に更新"
    $name:pl-PL: "Odśwież"
    $name:pt-BR: "Atualizar"
    $name:pt-PT: "Atualizar"
    $name:ru-RU: "Обновить"
    $name:tr-TR: "Yenile"
  - removeCopyAsPath: false
    $name: Copy as path
    $name:cs-CZ: "Kopírovat jako cestu"
    $name:de-DE: "Als Pfad kopieren"
    $name:es-MX: "Copiar como ruta de acceso"
    $name:fr-FR: "Copier comme chemin"
    $name:ja-JP: "パスのコピー"
    $name:pl-PL: "Kopiuj jako ścieżkę"
    $name:pt-BR: "Copiar como caminho"
    $name:pt-PT: "Copiar como caminho"
    $name:ru-RU: "Копировать как путь"
    $name:tr-TR: "Yolu kopyala"
  - removeCustomizeFolder: false
    $name: Customize this folder...
    $name:cs-CZ: "Přizpůsobit tuto složku..."
    $name:de-DE: "Ordner anpassen..."
    $name:es-MX: "Personalizar esta carpeta..."
    $name:fr-FR: "Personnaliser ce dossier..."
    $name:ja-JP: "このフォルダーのカスタマイズ..."
    $name:pl-PL: "Dostosuj ten folder..."
    $name:pt-BR: "Personalizar esta pasta..."
    $name:pt-PT: "Personalizar esta pasta..."
    $name:ru-RU: "Настроить папку..."
    $name:tr-TR: "Bu klasörü özelleştir..."
  - removeFavorites: false
    $name: Add to Favorites
    $name:cs-CZ: "Přidat k oblíbeným položkám"
    $name:de-DE: "Zu Favoriten hinzufügen"
    $name:es-MX: "Agregar a favoritos"
    $name:fr-FR: "Ajouter aux favoris"
    $name:ja-JP: "お気に入りに追加"
    $name:pl-PL: "Dodaj do ulubionych"
    $name:pt-BR: "Adicionar aos Favoritos"
    $name:pt-PT: "Adicionar aos Favoritos"
    $name:ru-RU: "Добавить в избранное"
    $name:tr-TR: "Sık kullanılanlara ekle"
  - removePinToQuickAccess: false
    $name: Pin to Quick access
    $name:cs-CZ: "Připnout na Rychlý přístup"
    $name:de-DE: "An Schnellzugriff anheften"
    $name:es-MX: "Anclar a Acceso rápido"
    $name:fr-FR: "Épingler à Accès rapide"
    $name:ja-JP: "クイック アクセスにピン留めする"
    $name:pl-PL: "Przypnij do obszaru Szybki dostęp"
    $name:pt-BR: "Fixar no Acesso Rápido"
    $name:pt-PT: "Afixar no Acesso rápido"
    $name:ru-RU: "Закрепить на панели быстрого доступа"
    $name:tr-TR: "Hızlı erişime sabitle"
  - removePinToStart: false
    $name: Pin to Start
    $name:cs-CZ: "Připnout na Start"
    $name:de-DE: "An \"Start\" anheften"
    $name:es-MX: "Anclar a Inicio"
    $name:fr-FR: "Épingler au menu Démarrer"
    $name:ja-JP: "スタートにピン留めする"
    $name:pl-PL: "Przypnij do menu Start"
    $name:pt-BR: "Fixar em Iniciar"
    $name:pt-PT: "Afixar no Iniciar"
    $name:ru-RU: "Закрепить в меню \"Пуск\""
    $name:tr-TR: "Başlat'a sabitle"
  - removeCast: false
    $name: Cast to Device
    $name:cs-CZ: "Přetypovat do zařízení"
    $name:de-DE: "Wiedergabe auf Gerät"
    $name:es-MX: "Transmitir en dispositivo"
    $name:fr-FR: "Diffuser vers un appareil"
    $name:ja-JP: "デバイス キャスト"
    $name:pl-PL: "Przesyłanie na urządzenie"
    $name:pt-BR: "Transmitir para Dispositivo"
    $name:pt-PT: "Transmitir para o Dispositivo"
    $name:ru-RU: "Передать на устройство"
    $name:tr-TR: "Cihaza yayınla"
  - removeGiveAccess: false
    $name: Give access to
    $name:cs-CZ: "Poskytnout přístup k"
    $name:de-DE: "Freigeben für"
    $name:es-MX: "Dar acceso a"
    $name:fr-FR: "Donner l'accès à"
    $name:ja-JP: "アクセスを許可する"
    $name:pl-PL: "Udziel dostępu do"
    $name:pt-BR: "Conceder acesso a"
    $name:pt-PT: "Conceder acesso a"
    $name:ru-RU: "Предоставить доступ к"
    $name:tr-TR: "Erişim ver"
  - removeRestoreVersions: false
    $name: Restore previous versions
    $name:cs-CZ: "Obnovit předchozí verze"
    $name:de-DE: "Vorgängerversionen wiederhestellen"
    $name:es-MX: "Restaurar versiones anteriores"
    $name:fr-FR: "Restaurer les versions précédentes"
    $name:ja-JP: "以前のバージョンの復元"
    $name:pl-PL: "Przywróć poprzednie wersje"
    $name:pt-BR: "Restaurar versões anteriores"
    $name:pt-PT: "Restaurar versões anteriores"
    $name:ru-RU: "Восстановить прежнюю версию"
    $name:tr-TR: "Önceki sürümleri geri yükle"
  - removeIncludeInLibrary: false
    $name: Include in library
    $name:cs-CZ: "Zahrnout do knihovny"
    $name:de-DE: "In Bibliothek aufnehmen"
    $name:es-MX: "Incluir en biblioteca"
    $name:fr-FR: "Inclure dans la bibliothèque"
    $name:ja-JP: "ライブラリに追加"
    $name:pl-PL: "Dołącz do biblioteki"
    $name:pt-BR: "Incluir na biblioteca"
    $name:pt-PT: "Incluir na biblioteca"
    $name:ru-RU: "Добавить в библиотеку"
    $name:tr-TR: "Kitaplığa ekle"
  - removeRotate: false
    $name: Rotate options
    $name:cs-CZ: "Možnosti otáčení"
    $name:de-DE: "Drehoptionen"
    $name:es-MX: "Opciones de giro"
    $name:fr-FR: "Options de rotation"
    $name:ja-JP: "回転オプション"
    $name:pl-PL: "Opcje obracania"
    $name:pt-BR: "Opções de rotação"
    $name:pt-PT: "Opções de rotação"
    $name:ru-RU: "Параметры поворота"
    $name:tr-TR: "Döndürme seçenekleri"
    $description: The "Rotate right" and "Rotate left" items
  - removeDisplaySettings: false
    $name: Display settings
    $name:cs-CZ: "Nastavení zobrazení"
    $name:de-DE: "Anzeigeeinstellungen"
    $name:es-MX: "Configuración de pantalla"
    $name:fr-FR: "Paramètres d'affichage"
    $name:ja-JP: "ディスプレイ設定"
    $name:pl-PL: "Ustawienia ekranu"
    $name:pt-BR: "Configurações de vídeo"
    $name:pt-PT: "Definições do ecrã"
    $name:ru-RU: "Параметры экрана"
    $name:tr-TR: "Görüntü ayarları"
    $description: Desktop menu item
  - removePersonalize: false
    $name: Personalize
    $name:cs-CZ: "Přizpůsobit"
    $name:de-DE: "Anpassen"
    $name:es-MX: "Personalizar"
    $name:fr-FR: "Personnaliser"
    $name:ja-JP: "個人用設定"
    $name:pl-PL: "Personalizuj"
    $name:pt-BR: "Personalizar"
    $name:pt-PT: "Personalizar"
    $name:ru-RU: "Персонализация"
    $name:tr-TR: "Kişiselleştir"
    $description: Desktop menu item
  - removeSetAsDesktopBackground: false
    $name: Set as desktop background
    $name:cs-CZ: "Nastavit jako pozadí plochy"
    $name:de-DE: "Als Desktophintergrund festlegen"
    $name:es-MX: "Establecer como fondo de escritorio"
    $name:fr-FR: "Définir comme arrière-plan du Bureau"
    $name:ja-JP: "デスクトップの背景として設定"
    $name:pl-PL: "Ustaw jako tło pulpitu"
    $name:pt-BR: "Definir como plano de fundo da área de trabalho"
    $name:pt-PT: "Definir como fundo do ambiente de trabalho"
    $name:ru-RU: "Сделать фоном рабочего стола"
    $name:tr-TR: "Masaüstü arka planı olarak ayarla"
  - removeView: false
    $name: View
    $name:cs-CZ: "Zobrazení"
    $name:de-DE: "Ansicht"
    $name:es-MX: "Ver"
    $name:fr-FR: "Affichage"
    $name:ja-JP: "表示"
    $name:pl-PL: "Widok"
    $name:pt-BR: "Exibir"
    $name:pt-PT: "Ver"
    $name:ru-RU: "Вид"
    $name:tr-TR: "Görünüm"
  - removeSortBy: false
    $name: Sort by
    $name:cs-CZ: "Seřadit podle"
    $name:de-DE: "Sortieren nach"
    $name:es-MX: "Ordenar por"
    $name:fr-FR: "Trier par"
    $name:ja-JP: "並べ替え"
    $name:pl-PL: "Sortuj według"
    $name:pt-BR: "Classificar por"
    $name:pt-PT: "Ordenar por"
    $name:ru-RU: "Сортировка"
    $name:tr-TR: "Sıralama ölçütü"
  - removeGroupBy: false
    $name: Group by
    $name:cs-CZ: "Seskupit podle"
    $name:de-DE: "Gruppieren nach"
    $name:es-MX: "Agrupar por"
    $name:fr-FR: "Regrouper par"
    $name:ja-JP: "グループで表示"
    $name:pl-PL: "Grupuj według"
    $name:pt-BR: "Agrupar por"
    $name:pt-PT: "Agrupar por"
    $name:ru-RU: "Группировка"
    $name:tr-TR: "Gruplama ölçütü"
  - removeNew: false
    $name: New
    $name:cs-CZ: "Nový"
    $name:de-DE: "Neu"
    $name:es-MX: "Nuevo"
    $name:fr-FR: "Nouveau"
    $name:ja-JP: "新規作成"
    $name:pl-PL: "Nowy"
    $name:pt-BR: "Novo"
    $name:pt-PT: "Novo"
    $name:ru-RU: "Создать"
    $name:tr-TR: "Yeni"
  - removeProperties: false
    $name: Properties
    $name:cs-CZ: "Vlastnosti"
    $name:de-DE: "Eigenschaften"
    $name:es-MX: "Propiedades"
    $name:fr-FR: "Propriétés"
    $name:ja-JP: "プロパティ"
    $name:pl-PL: "Właściwości"
    $name:pt-BR: "Propriedades"
    $name:pt-PT: "Propriedades"
    $name:ru-RU: "Свойства"
    $name:tr-TR: "Özellikler"
  - removePaste: false
    $name: Paste
    $name:cs-CZ: "Vložit"
    $name:de-DE: "Einfügen"
    $name:es-MX: "Pegar"
    $name:fr-FR: "Coller"
    $name:ja-JP: "貼り付け"
    $name:pl-PL: "Wklej"
    $name:pt-BR: "Colar"
    $name:pt-PT: "Colar"
    $name:ru-RU: "Вставить"
    $name:tr-TR: "Yapıştır"
    $description: Only when greyed-out
  - removeExtractAll: false
    $name: Extract All...
    $name:cs-CZ: "Extrahovat vše..."
    $name:de-DE: "Alle extrahieren..."
    $name:es-MX: "Extraer todo..."
    $name:fr-FR: "Extraire tout..."
    $name:ja-JP: "すべて展開..."
    $name:pl-PL: "Wyodrębnij wszystkie..."
    $name:pt-BR: "Extrair Tudo..."
    $name:pt-PT: "Extrair Todos..."
    $name:ru-RU: "Извлечь все..."
    $name:tr-TR: "Tümünü ayıkla..."
  $name: Basic Items
  $description: Disabled by default

- appSpecificItems:
  - removeSendWithQuickShare: false
    $name: Send with Quick Share
    $name:cs-CZ: "Odeslat pomocí Rychlého sdílení"
    $name:de-DE: "Mit Quick Share senden"
    $name:es-MX: "Enviar con Quick Share"
    $name:fr-FR: "Envoyer avec Quick Share"
    $name:pl-PL: "Wyślij za pomocą funkcji Quick Share"
    $name:pt-BR: "Enviar com o Compartilhamento Rápido"
    $name:pt-PT: "Enviar com Partilha Rápida"
    $name:tr-TR: "Quick Share ile gönder"
  - removeVLCPlaylist: false
    $name: Add to VLC media player's Playlist
    $name:cs-CZ: "Přidat do seznamu stop přehrávače médií VLC"
    $name:de-DE: "Zur VLC media player Wiedergabeliste hinzufügen"
    $name:es-MX: "Agregar a la lista de reproducción de VLC media player"
    $name:fr-FR: "Ajouter à la liste de lecture de VLC"
    $name:ja-JP: "VLCメディアプレイヤーのプレイリストに追加"
    $name:pl-PL: "Dodaj do listy odtwarzania programu VLC media player"
    $name:pt-BR: "Adicionar à lista de reprodução do VLC media player"
    $name:pt-PT: "Adicionar à lista de reprodução do VLC media player"
    $name:ru-RU: "Добавить в плейлист VLC"
    $name:tr-TR: "VLC media player oynatma listesine ekle"
  - removeVLCPlay: false
    $name: Play with VLC media player
    $name:cs-CZ: "Přehrát přehrávačem médií VLC"
    $name:de-DE: "Mit VLC media player wiedergeben"
    $name:es-MX: "Reproducir con VLC media player"
    $name:fr-FR: "Lire avec VLC"
    $name:ja-JP: "VLCメディアプレイヤーで再生"
    $name:pl-PL: "Odtwórz w programie VLC media player"
    $name:pt-BR: "Reproduzir com o VLC media player"
    $name:pt-PT: "Reproduzir com o VLC media player"
    $name:ru-RU: "Воспроизвести в VLC"
    $name:tr-TR: "VLC media player ile oynat"
  - removeAddToMediaPlayerQueue: false
    $name: Add to Media Player play queue
    $name:cs-CZ: "Přidat do fronty přehrávání přehrávače médií"
    $name:de-DE: "Zur Windows Media Player-Wiedergabeliste hinzufügen"
    $name:es-MX: "Agregar a la cola de reproducción del Reproductor multimedia"
    $name:fr-FR: "Ajouter à la file d'attente de Media Player"
    $name:ja-JP: "メディアプレイヤーの再生キューに追加"
    $name:pl-PL: "Dodaj do kolejki odtwarzania programu Media Player"
    $name:pt-BR: "Adicionar à fila de reprodução do Media Player"
    $name:pt-PT: "Adicionar à fila de reprodução do Media Player"
    $name:tr-TR: "Media Player oynatma kuyruğuna ekle"
  - removePlayWithMediaPlayer: false
    $name: Play with Media Player
    $name:cs-CZ: "Přehrát přehrávačem médií"
    $name:de-DE: "Mit Windows Media Player wiedergeben"
    $name:es-MX: "Reproducir con el Reproductor de Windows Media"
    $name:fr-FR: "Lire avec Media Player"
    $name:ja-JP: "メディアプレイヤーで再生"
    $name:pl-PL: "Odtwórz w programie Media Player"
    $name:pt-BR: "Reproduzir com o Media Player"
    $name:pt-PT: "Reproduzir com o Media Player"
    $name:tr-TR: "Media Player ile oynat"
  - removeEditInNotepad: false
    $name: Edit in Notepad
    $name:cs-CZ: "Upravit v Poznámkovém bloku"
    $name:de-DE: "Im Editor bearbeiten"
    $name:es-MX: "Editar en el Bloc de notas"
    $name:fr-FR: "Modifier dans le Bloc-notes"
    $name:ja-JP: "メモ帳で編集"
    $name:pl-PL: "Edytuj w Notatniku"
    $name:pt-BR: "Editar no Bloco de Notas"
    $name:pt-PT: "Editar no Bloco de Notas"
    $name:ru-RU: "Изменить в Блокноте"
    $name:tr-TR: "Not Defteri'nde düzenle"
    $description: Enable extension filtering below to show this only for relevant file types
  - removeEditInNotepadPlusPlus: false
    $name: Edit with Notepad++
    $name:cs-CZ: "Upravit v aplikaci Notepad++"
    $name:de-DE: "Mit Notepad++ bearbeiten"
    $name:es-MX: "Editar con Notepad++"
    $name:fr-FR: "Modifier avec Notepad++"
    $name:pl-PL: "Edytuj w Notepad++"
    $name:pt-BR: "Editar no Notepad++"
    $name:pt-PT: "Editar no Notepad++"
    $name:ru-RU: "Редактировать в Notepad++"
    $name:tr-TR: "Notepad++ ile düzenle"
    $description: Enable extension filtering below to show this only for relevant file types
  - removeEditWithPhotos: false
    $name: Edit with Photos
    $name:cs-CZ: "Upravit pomocí Fotky"
    $name:de-DE: "Mit Fotos bearbeiten"
    $name:es-MX: "Editar con Fotos"
    $name:fr-FR: "Modifier avec Photos"
    $name:ja-JP: "フォトで編集"
    $name:pl-PL: "Edytuj w Zdjęcia"
    $name:pt-BR: "Editar com Fotos"
    $name:pt-PT: "Editar com Fotografias"
    $name:ru-RU: "Изменить с помощью приложения \"Фотографии\""
    $name:tr-TR: "Fotoğraflar ile düzenle"
  - removeEditWithPaint: false
    $name: Edit with Paint
    $name:cs-CZ: "Upravit pomocí Malování"
    $name:de-DE: "Mit Paint bearbeiten"
    $name:es-MX: "Editar con Paint"
    $name:fr-FR: "Modifier avec Paint"
    $name:ja-JP: "ペイントで編集する"
    $name:pl-PL: "Edytuj w Paint"
    $name:pt-BR: "Editar com o Paint"
    $name:pt-PT: "Editar com o Paint"
    $name:ru-RU: "Редактирование с помощью приложения Paint"
    $name:tr-TR: "Paint ile düzenle"
  - removeNvidiaApp: false
    $name: NVIDIA App
    $name:cs-CZ: "Ovládací panely NVIDIA"
    $name:de-DE: "NVIDIA Systemsteuerung"
    $name:es-MX: "Panel de control de NVIDIA"
    $name:fr-FR: "Panneau de configuration NVIDIA"
    $name:ja-JP: "NVIDIA コントロールパネル"
    $name:pl-PL: "Panel sterowania NVIDIA"
    $name:pt-BR: "Painel de Controle NVIDIA"
    $name:pt-PT: "Painel de Controlo da NVIDIA"
    $name:ru-RU: "Панель управления NVIDIA"
    $name:tr-TR: "NVIDIA Denetim Masası"
    $description: Desktop menu item
  - removeOpenInTerminal: false
    $name: Open in Terminal
    $name:cs-CZ: "Otevřít v terminálu"
    $name:de-DE: "In Terminal öffnen"
    $name:es-MX: "Abrir en Terminal"
    $name:fr-FR: "Ouvrir dans le Terminal"
    $name:pl-PL: "Otwórz w Terminalu"
    $name:pt-BR: "Abrir no Terminal"
    $name:pt-PT: "Abrir no Terminal"
    $name:ru-RU: "Открыть в Терминале"
    $name:tr-TR: "Terminal'de aç"
  - removeAlwaysKeepOnThisDevice: false
    $name: Always keep on this device
    $name:cs-CZ: "Vždy ponechat na tomto zařízení"
    $name:de-DE: "Immer auf diesem Gerät behalten"
    $name:es-MX: "Mantener siempre en este dispositivo"
    $name:fr-FR: "Toujours conserver sur cet appareil"
    $name:ja-JP: "このデバイス上に常に保持する"
    $name:pl-PL: "Zawsze przechowuj na tym urządzeniu"
    $name:pt-BR: "Sempre manter neste dispositivo"
    $name:pt-PT: "Manter sempre neste dispositivo"
    $name:tr-TR: "Her zaman bu cihazda tut"
    $description: OneDrive menu item
  - removeFreeUpSpace: false
    $name: Free up space
    $name:cs-CZ: "Uvolnit místo"
    $name:de-DE: "Bereinigen"
    $name:es-MX: "Liberar espacio"
    $name:fr-FR: "Libérer de l'espace"
    $name:ja-JP: "空き領域を増やす"
    $name:pl-PL: "Zwolnij miejsce"
    $name:pt-BR: "Liberar espaço"
    $name:pt-PT: "Libertar espaço"
    $name:tr-TR: "Alan boşalt"
    $description: OneDrive menu item
  - removeWinRAR: false
    $name: WinRAR
    $description: Enable extension filtering below to show this only for relevant file types
  $name: App-specific Items
  $description: Disabled by default

- customItems:
  - ""
  $name: Custom items to remove
  $description: >-
    Basic Usage (Exact Match): Enter exact menu item names.

    Wildcard Usage (Prefix Match): Add * at the end for prefix matching (e.g., "Open*" removes all items starting with "Open")

- modifierKeyOverride:
  - enableModifierOverride: false
    $name: Enable modifier key override
    $description: >-
      When enabled, holding Ctrl or Alt while right-clicking temporarily bypasses the mod and shows all original context menu items.
  - overrideKey: ctrl
    $name: Override key
    $description: Which modifier key to hold while right-clicking to bypass filtering
    $options:
    - ctrl: Ctrl
    - alt: Alt
    - both: Both
  $name: Modifier Key Override
  $description: Hold a modifier key while right-clicking to see all original menu items

- extensionFiltering:
  - enableExtensionFiltering: false
    $name: Enable Notepad extension filtering
    $description: >-
      When enabled, "Edit in Notepad" and "Edit with Notepad++" will ONLY appear for files whose extensions are in the Notepad whitelist below. They are hidden for all other file types. (Note: Requires "Edit in Notepad" or "Edit with Notepad++" menu items to be enabled.)
  - notepadExtensions:
    - ".txt"
    - ".log"
    - ".json"
    - ".xml"
    - ".html"
    - ".css"
    - ".js"
    - ".ts"
    - ".md"
    - ".csv"
    - ".sql"
    - ".yaml"
    - ".yml"
    - ".py"
    - ".java"
    - ".c"
    - ".cpp"
    - ".cs"
    $name: Extensions for Notepad/Notepad++
    $description: Use lowercase with dot (e.g., .txt)
  - enableWinRARFiltering: false
    $name: Enable WinRAR extension filtering
    $description: >-
      When enabled, the WinRAR menu item will ONLY appear for files whose extensions are in the WinRAR whitelist below. It is hidden for all other file types. (Note: Requires "WinRAR" menu item to be enabled.)
  - winrarExtensions:
    - ".zip"
    - ".rar"
    - ".7z"
    - ".iso"
    - ".tar"
    - ".gz"
    - ".tgz"
    - ".bz2"
    - ".xz"
    - ".zst"
    - ".cab"
    $name: Extensions for WinRAR
    $description: Use lowercase with dot (e.g., .zip)
  $name: Extension Filtering
  $description: >-
    On Windows 11, Notepad and WinRAR items can appear even when right-clicking unrelated files. Use the Extension Filtering to show each only for relevant file types.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <shlguid.h>
#include <servprov.h>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <cwchar>
#include <windhawk_utils.h>

// Thread-local storage for file paths
thread_local std::vector<std::wstring> tl_filePaths;

// RemoveMenu() detaches a filtered submenu without destroying it, passing
// ownership to us. We stash the handle here and destroy it in
// ExitMenuTracking(), after the session's tracking call has returned.
thread_local std::vector<HMENU> tl_detachedSubmenus;

// Result of a single ancestor-chain classification of a popup menu's owner
// window: whether to filter the menu at all, and (for ShellDefView) enough
// to skip a redundant re-walk later when looking up the selected files.
enum class ShellViewKind {
    None,           // not a menu the mod filters (Start menu, Explorer-frame toolbars, etc.)
    Desktop,        // right-click on the desktop
    ShellDefView,   // right-click in the main file list view -- has a selection to read
    NavPane,        // right-click in the navigation pane -- no IFolderView selection to read
    Taskbar,        // right-click on the taskbar (incl. the Windows 10-style one) -- filtered, but no file selection
};

// True only for the desktop: the root window is the shell desktop window
// itself, or a top-level Progman/WorkerW that actually hosts the desktop's
// SHELLDLL_DefView -- checking that child excludes other WorkerW/Progman roots reused elsewhere in Explorer.
bool IsDesktopWindow(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }
    if (root == GetShellWindow()) {
        return true;
    }
    wchar_t className[256] = {0};
    GetClassNameW(root, className, ARRAYSIZE(className));
    // Progman at creation time; a slideshow or wallpaper tool can reparent
    // the desktop shell view onto a top-level WorkerW afterwards. Either
    // way, the desktop's root always hosts a SHELLDLL_DefView child.
    if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
        return false;
    }
    return FindWindowExW(root, nullptr, L"SHELLDLL_DefView", nullptr) != nullptr;
}

// True for the taskbar's own top-level window classes: the primary taskbar
// and the per-monitor secondary ones.
bool IsTaskbarWindowClass(const wchar_t* className) {
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// Classifies hwnd with a single ancestor walk. Desktop check MUST run
// first: desktop's icon view is itself hosted in a SHELLDLL_DefView, so
// checking the loop first misclassifies desktop, breaking extension filtering.
ShellViewKind ClassifyShellView(HWND hwnd) {
    if (IsDesktopWindow(hwnd)) {
        return ShellViewKind::Desktop;
    }
    for (HWND h = hwnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t className[256] = {0};
        GetClassNameW(h, className, ARRAYSIZE(className));
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            return ShellViewKind::ShellDefView;
        }
        if (wcscmp(className, L"NamespaceTreeControl") == 0) {
            return ShellViewKind::NavPane;
        }
        if (IsTaskbarWindowClass(className)) {
            return ShellViewKind::Taskbar;
        }
    }
    // A popup can also be owned by a taskbar window without being its child
    // (e.g. a taskbar button's menu), so check the owner chain as well.
    for (HWND h = GetWindow(GetAncestor(hwnd, GA_ROOT), GW_OWNER); h; h = GetWindow(h, GW_OWNER)) {
        wchar_t className[256] = {0};
        GetClassNameW(h, className, ARRAYSIZE(className));
        if (IsTaskbarWindowClass(className)) {
            return ShellViewKind::Taskbar;
        }
    }
    return ShellViewKind::None;
}

// Sends CWM_GETISHELLBROWSER (WM_USER+7) only to known frame/tab window
// classes. WM_USER-range values are only meaningful within a specific
// class's own message set, so an unknown class risks misinterpreting it.
IShellBrowser* GetShellBrowser(HWND hwnd) {
    // SHELLDLL_DefView isn't included: it never answers this message and
    // has its own private WM_USER-range messages, risking a misread reply.
    // Progman/WorkerW route to GetDesktopShellBrowser() before this is called.
    struct ShellWindowClass {
        const wchar_t* name;
        bool isFrame;  // true if this class also bounds the ancestor walk
    };
    static const ShellWindowClass kShellWindowClasses[] = {
        {L"ShellTabWindowClass", false},
        {L"CabinetWClass", true},
        {L"ExploreWClass", true},
    };

    IShellBrowser* pShellBrowser = nullptr;
    for (HWND h = hwnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t className[256] = {0};
        GetClassNameW(h, className, ARRAYSIZE(className));

        for (const ShellWindowClass& cls : kShellWindowClasses) {
            if (wcscmp(className, cls.name) != 0) {
                continue;
            }

            LRESULT result = SendMessageW(h, WM_USER + 7 /* CWM_GETISHELLBROWSER */, 0, 0);
            pShellBrowser = (IShellBrowser*)result;
            if (pShellBrowser) {
                return pShellBrowser;
            }

            if (cls.isFrame) {
                return nullptr;
            }
            break;
        }
    }
    return nullptr;
}

// Set by GetSelectedFilesFromExplorer() to distinguish a genuine "nothing
// is selected" from a lookup failure, so ShouldRemoveByExtension() only
// fails closed (hides) on the former -- an error shouldn't hide a match.
thread_local bool tl_selectionLookupFailed = false;

// The desktop doesn't expose an IShellBrowser via CWM_GETISHELLBROWSER --
// use IShellWindows::FindWindowSW(SWC_DESKTOP) then IServiceProvider ->
// SID_STopLevelBrowser. Returns a new reference; the caller must Release() it.
IShellBrowser* GetDesktopShellBrowser() {
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                 IID_IShellWindows, (void**)&pShellWindows)) || !pShellWindows) {
        return nullptr;
    }
    
    IShellBrowser* pShellBrowser = nullptr;
    VARIANT vEmpty = {};
    long lhwnd = 0;
    IDispatch* pDisp = nullptr;
    if (SUCCEEDED(pShellWindows->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &lhwnd,
                                               SWFO_NEEDDISPATCH, &pDisp)) && pDisp) {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider)) && pServiceProvider) {
            pServiceProvider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&pShellBrowser);
            pServiceProvider->Release();
        }
        pDisp->Release();
    }
    
    pShellWindows->Release();
    return pShellBrowser;
}

// Extracts the currently-selected file paths from a live IShellBrowser.
// Shared by both the desktop and regular-folder-window lookup paths.
// Reference-counting of pShellBrowser is the caller's responsibility.
std::vector<std::wstring> GetSelectedFilesFromShellBrowser(IShellBrowser* pShellBrowser) {
    std::vector<std::wstring> files;
    if (!pShellBrowser) {
        return files;
    }
    
    IShellView* pShellView = nullptr;
    if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
        IFolderView* pFolderView = nullptr;
        if (SUCCEEDED(pShellView->QueryInterface(IID_IFolderView, (void**)&pFolderView)) && pFolderView) {
            // Get the folder interface
            IShellFolder* pFolder = nullptr;
            if (SUCCEEDED(pFolderView->GetFolder(IID_IShellFolder, (void**)&pFolder)) && pFolder) {
                // Get selected items
                IEnumIDList* pEnum = nullptr;
                if (SUCCEEDED(pFolderView->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
                    LPITEMIDLIST pidl = nullptr;
                    while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                        // Use StrRetToStrW (heap-allocated, no length cap)
                        // instead of a fixed MAX_PATH buffer, since a
                        // truncated deep path would lose its extension.
                        STRRET strret;
                        if (SUCCEEDED(pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret))) {
                            LPWSTR pszPath = nullptr;
                            if (SUCCEEDED(StrRetToStrW(&strret, pidl, &pszPath)) && pszPath) {
                                if (pszPath[0]) {
                                    files.push_back(std::wstring(pszPath));
                                }
                                CoTaskMemFree(pszPath);
                            }
                        }
                        CoTaskMemFree(pidl);
                    }
                    pEnum->Release();
                }
                pFolder->Release();
            }
            
            pFolderView->Release();
        }
        pShellView->Release();
    }
    
    return files;
}

// Function to get selected files from Explorer, using the exact hWnd that
// TrackPopupMenu(Ex) was called with, so no cross-process/cross-tab
// guessing is needed. kind is the caller's ClassifyShellView() result.
std::vector<std::wstring> GetSelectedFilesFromExplorer(HWND hwnd, ShellViewKind kind) {
    std::vector<std::wstring> files;
    tl_selectionLookupFailed = false;
    
    if (!hwnd) {
        Wh_Log(L"GetSelectedFilesFromExplorer: No hWnd provided");
        tl_selectionLookupFailed = true;
        return files;
    }
    
    if (kind == ShellViewKind::Desktop) {
        IShellBrowser* pShellBrowser = GetDesktopShellBrowser();
        if (!pShellBrowser) {
            Wh_Log(L"Could not get desktop IShellBrowser");
            tl_selectionLookupFailed = true;
            return files;
        }
        files = GetSelectedFilesFromShellBrowser(pShellBrowser);
        // Released here, on the creating thread and still inside the COM
        // apartment -- no session-scoped caching needed since this is the
        // only call site per session.
        pShellBrowser->Release();
        return files;

    }
    
    if (kind != ShellViewKind::ShellDefView) {
        // Either not a shell view menu at all, or a right-click in the
        // navigation pane, which has no IFolderView selection to read.
        // This is "no file context", not a failure -- filtering fails closed for it.
        Wh_Log(L"GetSelectedFilesFromExplorer: not a ShellDefView window, skipping");
        return files;
    }
    
    IShellBrowser* pShellBrowser = GetShellBrowser(hwnd);
    if (!pShellBrowser) {
        Wh_Log(L"Could not get IShellBrowser from window");
        tl_selectionLookupFailed = true;
        return files;
    }
    
    // CWM_GETISHELLBROWSER returns this pointer without an added reference
    // (it's owned by the frame window), so take a temporary one for the
    // duration of use -- cheap insurance against it being released elsewhere mid-call.
    pShellBrowser->AddRef();
    files = GetSelectedFilesFromShellBrowser(pShellBrowser);
    pShellBrowser->Release();
    
    return files;
}

// Settings structures
struct BloatwareSettings {
    bool removeOneDrive;
    bool removeCopilot;
    bool removeDefender;
    bool removeDesigner;
    bool removeClipchamp;
    bool removeMicrosoft365Copilot;
};

struct BasicSettings {
    bool removeOpen;
    bool removeOpenWith;
    bool removeCut;
    bool removeCopy;
    bool removeCreateShortcut;
    bool removeDelete;
    bool removeRename;
    bool removeSendTo;
    bool removeOpenInNewTab;
    bool removeOpenInNewWindow;
    bool removeEdit;
    bool removePlay;
    bool removePreview;
    bool removePrint;
    bool removeShare;
    bool removeRefresh;
    bool removeCopyAsPath;
    bool removeCustomizeFolder;
    bool removeFavorites;
    bool removePinToQuickAccess;
    bool removePinToStart;
    bool removeCast;
    bool removeGiveAccess;
    bool removeRestoreVersions;
    bool removeIncludeInLibrary;
    bool removeRotate;
    bool removeDisplaySettings;
    bool removePersonalize;
    bool removeSetAsDesktopBackground;
    bool removeView;
    bool removeSortBy;
    bool removeGroupBy;
    bool removeNew;
    bool removeProperties;
    bool removePaste;
    bool removeExtractAll;
};

struct AppSpecificSettings {
    bool removeSendWithQuickShare;
    bool removeVLCPlaylist;
    bool removeVLCPlay;
    bool removeAddToMediaPlayerQueue;
    bool removePlayWithMediaPlayer;
    bool removeEditInNotepad;
    bool removeEditInNotepadPlusPlus;
    bool removeEditWithPhotos;
    bool removeEditWithPaint;
    bool removeNvidiaApp;
    bool removeOpenInTerminal;
    bool removeAlwaysKeepOnThisDevice;
    bool removeFreeUpSpace;
    bool removeWinRAR;
};

struct ExtensionFilteringSettings {
    bool enableExtensionFiltering;
    std::vector<std::wstring> notepadExtensions;
    bool enableWinRARFiltering;
    std::vector<std::wstring> winrarExtensions;
};

struct ModifierKeySettings {
    bool enableModifierOverride;
    std::wstring overrideKey; // "ctrl", "alt", "both"
};

struct {
    BloatwareSettings bloatwareItems;
    BasicSettings basicItems;
    AppSpecificSettings appSpecificItems;
    ExtensionFilteringSettings extensionFiltering;
    ModifierKeySettings modifierKeyOverride;
    std::vector<std::wstring> customItems;
} g_settings;

// Structure to hold menu item info
// Forward declarations (definitions further below) -- needed because
// InitializeMenuItems() below precomputes each entry's normalized text.
std::wstring RemoveAmpersands(const std::wstring& str);
std::wstring NormalizeString(const std::wstring& str);

// Forward declaration of the thread-local "should ProcessMenu() recurse
// into submenus itself" flag (fully defined further below, alongside
// tl_hMenuHook) -- see its own comment for why this isn't just !tl_hMenuHook.
extern thread_local bool tl_recurseSubmenus;

// Forward declarations of the per-session filter-toggle snapshots (fully
// defined further below, alongside tl_sessionNeedFiles) -- see their own
// comment for why ShouldRemoveMenuItem() uses these instead of a live read.
extern thread_local bool tl_sessionExtensionFilterOn;
extern thread_local bool tl_sessionWinRARFilterOn;

struct MenuItem {
    std::wstring text;
    bool* enabled;
    std::vector<std::wstring>* allowedExtensions;  // nullptr = no extension check
    const bool* filterEnabled;                     // toggle gating the check above
    bool greyedOnly; // If true, only remove when the item is greyed out (disabled)
};

// List of predefined menu items to check
std::vector<MenuItem> g_menuItems;

// Indexes g_menuItems by normalizedText for O(1) lookup. Stores an index
// rather than a pointer so it can't dangle if g_menuItems is ever resized.
std::unordered_map<std::wstring, size_t> g_menuItemsByText;

// Guards g_settings and g_menuItems against concurrent access from
// LoadSettings() (settings-change thread) and the popup menu hooks
// (Explorer UI threads).
std::mutex g_settingsMutex;

// Forward declaration; full definition (and the reason it exists instead
// of raw ::towlower) is further below, near the other string-matching helpers.
std::wstring ToLower(const std::wstring& str);

// Function to get file extension from path
std::wstring GetFileExtension(const std::wstring& path) {
    // Only look at the last path component, so a folder named "v1.0"
    // doesn't get mistaken for an extension. No MAX_PATH limit, unlike
    // PathFindExtensionW, and no Windows 8+ dependency, unlike PathCchFindExtension.
    size_t nameStart = path.find_last_of(L"\\/");
    nameStart = (nameStart == std::wstring::npos) ? 0 : nameStart + 1;
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos || dot < nameStart || dot + 1 == path.size()) {
        return L"";
    }
    std::wstring ext = ToLower(path.substr(dot));
    return ext;
}

// Function to check if extension is in list
// extList entries are already lowercased once in LoadSettings(); compare directly.
bool IsExtensionInList(const std::wstring& ext, const std::vector<std::wstring>& extList) {
    if (ext.empty() || extList.empty()) {
        return false;
    }
    return std::ranges::find(extList, ext) != extList.end();
}

// Check if any of the current files has an extension in the list.
// Deliberately any-match, not all-match: a mixed selection keeps the item
// visible since it still works on the matching file (see extensionFiltering description).
bool AnyFileHasExtensionInList(const std::vector<std::wstring>& extList) {
    for (const auto& filePath : tl_filePaths) {
        std::wstring ext = GetFileExtension(filePath);
        if (IsExtensionInList(ext, extList)) {
            return true;
        }
    }
    return false;
}

// Function to initialize menu items based on settings
void InitializeMenuItems() {
    // One entry per menu item: the setting it belongs to, its flags, and then
    // the item's text in every supported language. Each text is expanded into
    // its own g_menuItems entry below, so lookups work exactly as before.
    struct MenuItemGroup {
        bool* enabled;
        std::vector<std::wstring>* allowedExtensions;
        const bool* filterEnabled;
        bool greyedOnly;
        std::vector<std::wstring> texts;
    };
    const std::vector<MenuItemGroup> menuItemGroups = {
        // Bloatware Items
        {&g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false, {L"Move to OneDrive", L"OneDrive に移動", L"Mover para o OneDrive", L"Mover a OneDrive", L"Přesunout na OneDrive", L"Auf OneDrive verschieben", L"OneDrive'a taşı", L"Przenieś do usługi OneDrive", L"Déplacer vers OneDrive"}},
        {&g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false, {L"Always keep on this device", L"このデバイス上に常に保持する", L"Sempre manter neste dispositivo", L"Manter sempre neste dispositivo", L"Mantener siempre en este dispositivo", L"Vždy ponechat na tomto zařízení", L"Immer auf diesem Gerät behalten", L"Her zaman bu cihazda tut", L"Zawsze przechowuj na tym urządzeniu", L"Toujours conserver sur cet appareil"}},
        {&g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false, {L"Free up space", L"空き領域を増やす", L"Liberar espaço", L"Libertar espaço", L"Liberar espacio", L"Uvolnit místo", L"Bereinigen", L"Alan boşalt", L"Zwolnij miejsce", L"Libérer de l'espace"}},
        {&g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false, {L"Ask Copilot", L"Copilot とチャットする", L"Perguntar ao Copilot", L"Preguntar a Copilot", L"Zeptat se Copilota", L"Copilot fragen", L"Copilot'a sor", L"Zapytaj aplikację Copilot", L"Demander à Copilot"}},
        {&g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false, {L"Scan with Microsoft Defender...", L"Microsoft Defenderでスキャンする...", L"Verificar com o Microsoft Defender...", L"Analisar com o Microsoft Defender...", L"Analizar con Microsoft Defender...", L"Prohledat pomocí programu Microsoft Defender...", L"Mit Microsoft Defender überprüfen...", L"Microsoft Defender ile tara...", L"Skanuj za pomocą programu Microsoft Defender...", L"Analyser avec Microsoft Defender...", L"Проверка с использованием Microsoft Defender..."}},
        {&g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false, {L"Create with Designer", L"Designerで作成", L"Criar com o Designer", L"Crear con Designer", L"Vytvořit pomocí Designeru", L"Mit Designer erstellen", L"Designer ile oluştur", L"Utwórz za pomocą aplikacji Designer", L"Créer avec Designer"}},
        {&g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false, {L"Edit with Clipchamp", L"Clipchampで編集", L"Editar com o Clipchamp", L"Editar con Clipchamp", L"Upravit pomocí Clipchampu", L"Mit Clipchamp bearbeiten", L"Clipchamp ile düzenle", L"Edytuj za pomocą aplikacji Clipchamp", L"Modifier avec Clipchamp", L"Редактировать в Clipchamp"}},
        {&g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false, {L"Ask Microsoft 365 Copilot", L"Perguntar ao Microsoft 365 Copilot", L"Preguntar a Microsoft 365 Copilot", L"Zeptat se Microsoft 365 Copilota", L"Microsoft 365 Copilot fragen", L"Microsoft 365 Copilot'a sor", L"Zapytaj aplikację Microsoft 365 Copilot", L"Demander à Microsoft 365 Copilot"}},
        
        // Basic Items
        {&g_settings.basicItems.removeOpen, nullptr, nullptr, false, {L"Open", L"開く", L"Abrir", L"Otevřít", L"Öffnen", L"Aç", L"Otwórz", L"Ouvrir", L"Открыть"}},
        {&g_settings.basicItems.removeOpenWith, nullptr, nullptr, false, {L"Open with", L"プログラムから開く", L"Abrir com", L"Abrir con", L"Otevřít v aplikaci", L"Öffnen mit", L"Birlikte aç", L"Otwórz za pomocą", L"Ouvrir avec", L"Открыть с помощью"}},
        {&g_settings.basicItems.removeCut, nullptr, nullptr, false, {L"Cut", L"切り取り", L"Recortar", L"Cortar", L"Vyjmout", L"Ausschneiden", L"Kes", L"Wytnij", L"Couper", L"Вырезать"}},
        {&g_settings.basicItems.removeCopy, nullptr, nullptr, false, {L"Copy", L"コピー", L"Copiar", L"Kopírovat", L"Kopieren", L"Kopyala", L"Kopiuj", L"Copier", L"Копировать"}},
        {&g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false, {L"Create shortcut", L"ショートカットの作成", L"Criar atalho", L"Crear acceso directo", L"Vytvořit zástupce", L"Verknüpfung erstellen", L"Kısayol oluştur", L"Utwórz skrót", L"Créer un raccourci", L"Создать ярлык"}},
        {&g_settings.basicItems.removeDelete, nullptr, nullptr, false, {L"Delete", L"削除", L"Excluir", L"Eliminar", L"Odstranit", L"Löschen", L"Sil", L"Usuń", L"Supprimer", L"Удалить"}},
        {&g_settings.basicItems.removeRename, nullptr, nullptr, false, {L"Rename", L"名前の変更", L"Renomear", L"Mudar o nome", L"Cambiar nombre", L"Přejmenovat", L"Umbenennen", L"Yeniden adlandır", L"Zmień nazwę", L"Renommer", L"Переименовать"}},
        {&g_settings.basicItems.removeSendTo, nullptr, nullptr, false, {L"Send to", L"送る", L"Enviar para", L"Enviar a", L"Odeslat do", L"Senden an", L"Gönder", L"Wyślij do", L"Envoyer vers", L"Отправить"}},
        {&g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false, {L"Open in new tab", L"新しいタブで開く", L"Abrir em uma nova guia", L"Abrir num novo separador", L"Abrir en nueva pestaña", L"Otevřít na nové kartě", L"In neuer Registerkarte öffnen", L"Yeni sekmede aç", L"Otwórz w nowej karcie", L"Ouvrir dans un nouvel onglet", L"Открыть в новой вкладке"}},
        {&g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false, {L"Open in new window", L"新しいウィンドウで開く", L"Abrir em uma nova janela", L"Abrir numa nova janela", L"Abrir en nueva ventana", L"Otevřít v novém okně", L"In neuem Fenster öffnen", L"Yeni pencerede aç", L"Otwórz w nowym oknie", L"Ouvrir dans une nouvelle fenêtre", L"Открыть в новом окне"}},
        {&g_settings.basicItems.removeEdit, nullptr, nullptr, false, {L"Edit", L"編集", L"Editar", L"Upravit", L"Bearbeiten", L"Düzenle", L"Edytuj", L"Modifier", L"Изменить"}},
        {&g_settings.basicItems.removePlay, nullptr, nullptr, false, {L"Play", L"再生", L"Reproduzir", L"Reproducir", L"Přehrát", L"Wiedergabe", L"Oynat", L"Odtwarzaj", L"Lire", L"Воспроизвести"}},
        {&g_settings.basicItems.removePreview, nullptr, nullptr, false, {L"Preview", L"プレビュー", L"Visualizar", L"Pré-visualizar", L"Vista previa", L"Náhled", L"Vorschau", L"Önizleme", L"Podgląd", L"Aperçu"}},
        {&g_settings.basicItems.removePrint, nullptr, nullptr, false, {L"Print", L"印刷", L"Imprimir", L"Tisk", L"Drucken", L"Yazdır", L"Drukuj", L"Imprimer", L"Печать"}},
        {&g_settings.basicItems.removeShare, nullptr, nullptr, false, {L"Share", L"共有", L"Compartilhar", L"Partilhar", L"Compartir", L"Sdílet", L"Freigabe", L"Paylaş", L"Udostępnij", L"Partager", L"Поделиться"}},
        {&g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false, {L"Send with Quick Share", L"Enviar com o Compartilhamento Rápido", L"Enviar com Partilha Rápida", L"Enviar con Quick Share", L"Odeslat pomocí Rychlého sdílení", L"Mit Quick Share senden", L"Quick Share ile gönder", L"Wyślij za pomocą funkcji Quick Share", L"Envoyer avec Quick Share"}},
        {&g_settings.basicItems.removeRefresh, nullptr, nullptr, false, {L"Refresh", L"最新の情報に更新", L"Atualizar", L"Actualizar", L"Aktualizovat", L"Aktualisieren", L"Yenile", L"Odśwież", L"Actualiser", L"Обновить"}},
        {&g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false, {L"Copy as path", L"パスのコピー", L"Copiar como caminho", L"Copiar como ruta de acceso", L"Kopírovat jako cestu", L"Als Pfad kopieren", L"Yolu kopyala", L"Kopiuj jako ścieżkę", L"Copier comme chemin", L"Копировать как путь"}},
        {&g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false, {L"Customize this folder...", L"このフォルダーのカスタマイズ...", L"Customise this folder...", L"Personalizar esta pasta...", L"Personalizar esta carpeta...", L"Přizpůsobit tuto složku...", L"Ordner anpassen...", L"Bu klasörü özelleştir...", L"Dostosuj ten folder...", L"Personnaliser ce dossier...", L"Настроить папку..."}},
        {&g_settings.basicItems.removeFavorites, nullptr, nullptr, false, {L"Add to Favorites", L"お気に入りに追加", L"Add to Favourites", L"Adicionar aos Favoritos", L"Agregar a favoritos", L"Přidat k oblíbeným položkám", L"Zu Favoriten hinzufügen", L"Sık kullanılanlara ekle", L"Dodaj do ulubionych", L"Ajouter aux favoris", L"Добавить в избранное"}},
        {&g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false, {L"Pin to Quick access", L"クイック アクセスにピン留めする", L"Fixar no Acesso Rápido", L"Afixar no Acesso rápido", L"Anclar a Acceso rápido", L"Připnout na Rychlý přístup", L"An Schnellzugriff anheften", L"Hızlı erişime sabitle", L"Przypnij do obszaru Szybki dostęp", L"Épingler à Accès rapide", L"Закрепить на панели быстрого доступа"}},
        {&g_settings.basicItems.removePinToStart, nullptr, nullptr, false, {L"Pin to Start", L"スタートにピン留めする", L"Fixar em Iniciar", L"Afixar no Iniciar", L"Anclar a Inicio", L"Připnout na Start", L"An \"Start\" anheften", L"Başlat'a sabitle", L"Przypnij do menu Start", L"Épingler au menu Démarrer", L"Закрепить в меню \"Пуск\""}},
        {&g_settings.basicItems.removeCast, nullptr, nullptr, false, {L"Cast to Device", L"デバイス キャスト", L"Transmitir para Dispositivo", L"Transmitir para o Dispositivo", L"Transmitir en dispositivo", L"Přetypovat do zařízení", L"Wiedergabe auf Gerät", L"Cihaza yayınla", L"Przesyłanie na urządzenie", L"Diffuser vers un appareil", L"Передать на устройство"}},
        {&g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false, {L"Give access to", L"アクセスを許可する", L"Conceder acesso a", L"Dar acceso a", L"Poskytnout přístup k", L"Freigeben für", L"Erişim ver", L"Udziel dostępu do", L"Donner l'accès à", L"Предоставить доступ к"}},
        {&g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false, {L"Restore previous versions", L"以前のバージョンの復元", L"Restaurar versões anteriores", L"Restaurar versiones anteriores", L"Obnovit předchozí verze", L"Vorgängerversionen wiederhestellen", L"Önceki sürümleri geri yükle", L"Przywróć poprzednie wersje", L"Restaurer les versions précédentes", L"Восстановить прежнюю версию"}},
        {&g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false, {L"Include in library", L"ライブラリに追加", L"Incluir na biblioteca", L"Incluir en biblioteca", L"Zahrnout do knihovny", L"In Bibliothek aufnehmen", L"Kitaplığa ekle", L"Dołącz do biblioteki", L"Inclure dans la bibliothèque", L"Добавить в библиотеку"}},
        {&g_settings.basicItems.removeRotate, nullptr, nullptr, false, {L"Rotate right", L"右に回転", L"Girar para a direita", L"Rodar para a direita", L"Girar a la derecha", L"Otočit doprava", L"Nach rechts drehen", L"Sağa döndür", L"Obróć w prawo", L"Faire pivoter vers la droite", L"Повернуть вправо"}},
        {&g_settings.basicItems.removeRotate, nullptr, nullptr, false, {L"Rotate left", L"左に回転", L"Girar para a esquerda", L"Rodar para a esquerda", L"Girar a la izquierda", L"Otočit doleva", L"Nach links drehen", L"Sola döndür", L"Obróć w lewo", L"Faire pivoter vers la gauche", L"Повернуть влево"}},
        {&g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false, {L"Display settings", L"ディスプレイ設定", L"Configurações de vídeo", L"Definições do ecrã", L"Configuración de pantalla", L"Nastavení zobrazení", L"Anzeigeeinstellungen", L"Görüntü ayarları", L"Ustawienia ekranu", L"Paramètres d'affichage", L"Параметры экрана"}},
        {&g_settings.basicItems.removePersonalize, nullptr, nullptr, false, {L"Personalize", L"個人用設定", L"Personalise", L"Personalizar", L"Přizpůsobit", L"Anpassen", L"Kişiselleştir", L"Personalizuj", L"Personnaliser", L"Персонализация"}},
        {&g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false, {L"Set as desktop background", L"デスクトップの背景として設定", L"Definir como plano de fundo da área de trabalho", L"Definir como fundo do ambiente de trabalho", L"Establecer como fondo de escritorio", L"Nastavit jako pozadí plochy", L"Als Desktophintergrund festlegen", L"Masaüstü arka planı olarak ayarla", L"Ustaw jako tło pulpitu", L"Définir comme arrière-plan du Bureau", L"Сделать фоном рабочего стола"}},
        {&g_settings.basicItems.removeView, nullptr, nullptr, false, {L"View", L"表示", L"Exibir", L"Ver", L"Zobrazení", L"Ansicht", L"Görünüm", L"Widok", L"Affichage", L"Вид"}},
        {&g_settings.basicItems.removeSortBy, nullptr, nullptr, false, {L"Sort by", L"並べ替え", L"Classificar por", L"Ordenar por", L"Seřadit podle", L"Sortieren nach", L"Sıralama ölçütü", L"Sortuj według", L"Trier par", L"Сортировка"}},
        {&g_settings.basicItems.removeGroupBy, nullptr, nullptr, false, {L"Group by", L"グループで表示", L"Agrupar por", L"Seskupit podle", L"Gruppieren nach", L"Gruplama ölçütü", L"Grupuj według", L"Regrouper par", L"Группировка"}},
        {&g_settings.basicItems.removeNew, nullptr, nullptr, false, {L"New", L"新規作成", L"Novo", L"Nuevo", L"Nový", L"Neu", L"Yeni", L"Nowy", L"Nouveau", L"Создать"}},
        {&g_settings.basicItems.removeProperties, nullptr, nullptr, false, {L"Properties", L"プロパティ", L"Propriedades", L"Propiedades", L"Vlastnosti", L"Eigenschaften", L"Özellikler", L"Właściwości", L"Propriétés", L"Свойства"}},
        {&g_settings.basicItems.removePaste, nullptr, nullptr, true, {L"Paste", L"貼り付け", L"Colar", L"Pegar", L"Vložit", L"Einfügen", L"Yapıştır", L"Wklej", L"Coller", L"Вставить"}},
        {&g_settings.basicItems.removeExtractAll, nullptr, nullptr, false, {L"Extract All...", L"すべて展開...", L"Extrair Tudo...", L"Extrair Todos...", L"Extraer todo...", L"Extrahovat vše...", L"Alle extrahieren...", L"Tümünü ayıkla...", L"Wyodrębnij wszystkie...", L"Extraire tout...", L"Извлечь все..."}},
        
        // App-specific Items
        {&g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false, {L"Add to VLC media player's Playlist", L"VLCメディアプレイヤーのプレイリストに追加", L"Adicionar à lista de reprodução do VLC media player", L"Agregar a la lista de reproducción de VLC media player", L"Přidat do seznamu stop přehrávače médií VLC", L"Zur VLC media player Wiedergabeliste hinzufügen", L"VLC media player oynatma listesine ekle", L"Dodaj do listy odtwarzania programu VLC media player", L"Ajouter à la liste de lecture de VLC", L"Добавить в плейлист VLC"}},
        {&g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false, {L"Play with VLC media player", L"VLCメディアプレイヤーで再生", L"Reproduzir com o VLC media player", L"Reproducir con VLC media player", L"Přehrát přehrávačem médií VLC", L"Mit VLC media player wiedergeben", L"VLC media player ile oynat", L"Odtwórz w programie VLC media player", L"Lire avec VLC", L"Воспроизвести в VLC"}},
        {&g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false, {L"Add to Media Player play queue", L"メディアプレイヤーの再生キューに追加", L"Adicionar à fila de reprodução do Media Player", L"Agregar a la cola de reproducción del Reproductor multimedia", L"Přidat do fronty přehrávání přehrávače médií", L"Zur Windows Media Player-Wiedergabeliste hinzufügen", L"Media Player oynatma kuyruğuna ekle", L"Dodaj do kolejki odtwarzania programu Media Player", L"Ajouter à la file d'attente de Media Player"}},
        {&g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false, {L"Play with Media Player", L"メディアプレイヤーで再生", L"Reproduzir com o Media Player", L"Reproducir con el Reproductor de Windows Media", L"Přehrát přehrávačem médií", L"Mit Windows Media Player wiedergeben", L"Media Player ile oynat", L"Odtwórz w programie Media Player", L"Lire avec Media Player"}},
        {&g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false, {L"Edit in Notepad", L"メモ帳で編集", L"Editar no Bloco de Notas", L"Editar en el Bloc de notas", L"Upravit v Poznámkovém bloku", L"Im Editor bearbeiten", L"Not Defteri'nde düzenle", L"Edytuj w Notatniku", L"Modifier dans le Bloc-notes", L"Изменить в Блокноте"}},
        {&g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false, {L"Edit with Notepad++", L"Editar no Notepad++", L"Editar con Notepad++", L"Upravit v aplikaci Notepad++", L"Mit Notepad++ bearbeiten", L"Notepad++ ile düzenle", L"Edytuj w Notepad++", L"Modifier avec Notepad++", L"Редактировать в Notepad++"}},
        {&g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false, {L"Edit with Photos", L"フォトで編集", L"Editar com Fotos", L"Editar com Fotografias", L"Editar con Fotos", L"Upravit pomocí Fotky", L"Mit Fotos bearbeiten", L"Fotoğraflar ile düzenle", L"Edytuj w Zdjęcia", L"Modifier avec Photos", L"Изменить с помощью приложения \"Фотографии\""}},
        {&g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false, {L"Edit with Paint", L"ペイントで編集する", L"Editar com o Paint", L"Editar con Paint", L"Upravit pomocí Malování", L"Mit Paint bearbeiten", L"Paint ile düzenle", L"Edytuj w Paint", L"Modifier avec Paint", L"Редактирование с помощью приложения Paint"}},
        {&g_settings.appSpecificItems.removeNvidiaApp, nullptr, nullptr, false, {L"NVIDIA App", L"NVIDIA コントロールパネル", L"Painel de Controle NVIDIA", L"Painel de Controlo da NVIDIA", L"Panel de control de NVIDIA", L"Ovládací panely NVIDIA", L"NVIDIA Systemsteuerung", L"NVIDIA Denetim Masası", L"Panel sterowania NVIDIA", L"Panneau de configuration NVIDIA", L"Панель управления NVIDIA"}},
        {&g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false, {L"Open in Terminal", L"Abrir no Terminal", L"Abrir en Terminal", L"Otevřít v terminálu", L"In Terminal öffnen", L"Terminal'de aç", L"Otwórz w Terminalu", L"Ouvrir dans le Terminal", L"Открыть в Терминале"}},
        {&g_settings.appSpecificItems.removeWinRAR, &g_settings.extensionFiltering.winrarExtensions, &g_settings.extensionFiltering.enableWinRARFiltering, false, {L"WinRAR"}}
    };

    g_menuItems.clear();
    for (const auto& group : menuItemGroups) {
        for (const auto& text : group.texts) {
            g_menuItems.push_back({text, group.enabled, group.allowedExtensions,
                                   group.filterEnabled, group.greyedOnly});
        }
    }
    
    // Index by normalized text so lookup is a single hash-map probe
    // instead of a linear scan over all ~564 entries.
    g_menuItemsByText.clear();
    for (size_t i = 0; i < g_menuItems.size(); i++) {
        std::wstring normalizedText = NormalizeString(RemoveAmpersands(g_menuItems[i].text));
        auto insertResult = g_menuItemsByText.insert({normalizedText, i});
        if (!insertResult.second) {
            // insert() keeps the first entry and silently drops this one on
            // a collision, leaving entry i's toggle unreachable. No
            // collisions exist today, but this warns whoever adds the next translation, instead of a silently dead toggle.
            Wh_Log(L"WARNING: Menu item text '%s' (entry %d) collides with an "
                   L"already-registered entry (%d) after normalization -- the "
                   L"newer entry's toggle will never be reachable by lookup",
                   g_menuItems[i].text.c_str(), (int)i, (int)insertResult.first->second);
        }
    }
}


// Utility function to remove ampersands for hotkey underlines. A single '&'
// is Windows' hotkey-underline escape (stripped); a doubled '&&' is a
// literal '&' in visible text, which must collapse to one, not vanish.
std::wstring RemoveAmpersands(const std::wstring& str) {
    std::wstring result;
    result.reserve(str.length());
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == L'&') {
            if (i + 1 < str.length() && str[i + 1] == L'&') {
                // Literal '&&' -> keep a single '&' and skip both source chars.
                result += L'&';
                i++;
            }
            // else: lone '&' hotkey-underline escape -- drop it.
        } else {
            result += str[i];
        }
    }
    return result;
}

// Utility function to convert string to lowercase
std::wstring ToLower(const std::wstring& str) {
    // CharLowerBuffW covers the full Unicode range, unlike ::towlower in
    // the CRT's default "C" locale, which only maps ASCII A-Z -- needed
    // so non-Latin Custom Items (Cyrillic, Greek, Turkish, etc.) can match.
    std::wstring result = str;
    if (!result.empty()) {
        CharLowerBuffW(result.data(), (DWORD)result.size());
    }
    return result;
}

// Utility function to normalize string for comparison
std::wstring NormalizeString(const std::wstring& str) {
    // Menu items can render with a tab-separated accelerator suffix (e.g.
    // "Copy\tCtrl+C"). Cut that off before comparing, so entries still match on just the visible label.
    std::wstring text = str;
    size_t tabPos = text.find(L'\t');
    if (tabPos != std::wstring::npos) {
        text = text.substr(0, tabPos);
    }
    
    // Some shell entries use the single-character ellipsis (U+2026)
    // instead of three ASCII dots -- normalize to the latter so both
    // spellings compare equal against the predefined table.
    size_t ellipsisPos;
    while ((ellipsisPos = text.find(L'\u2026')) != std::wstring::npos) {
        text.replace(ellipsisPos, 1, L"...");
    }
    
    std::wstring result = ToLower(text);
    // Remove leading/trailing whitespace
    size_t start = result.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = result.find_last_not_of(L" \t\r\n");
    return result.substr(start, end - start + 1);
}

// Check if text matches a custom item pattern
bool MatchesCustomItem(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty()) return false;
    
    // Check if pattern ends with wildcard
    if (pattern.back() == L'*') {
        // Prefix matching - remove the asterisk and compare
        std::wstring prefix = pattern.substr(0, pattern.length() - 1);
        return text.compare(0, prefix.length(), prefix) == 0;
    } else {
        // Exact matching
        return text == pattern;
    }
}

// Checks if a menu item should be removed based on extension filtering.
// Called only when the caller has already confirmed the relevant filter
// toggle is on and item.allowedExtensions is set.
bool ShouldRemoveByExtension(const MenuItem& item) {
    if (tl_filePaths.empty()) {
        if (tl_selectionLookupFailed) {
            // The lookup itself failed (no shell browser found, etc.), not
            // a genuine empty selection. Don't fail closed here, since
            // the file might match the whitelist and we couldn't check.
            return false;
        }
        // No file context at all (right-clicking empty space, in a regular
        // folder or a virtual one like This PC/Recycle Bin/Libraries).
        // Explicitly hide, so behavior is the same everywhere regardless of whether a path happened to be available.
        return true;
    }
    
    // Check if ANY of the selected files has an extension in the whitelist
    bool hasMatchingExtension = AnyFileHasExtensionInList(*item.allowedExtensions);
    
    // Whitelist mode: Remove if NO file has a matching extension
    return !hasMatchingExtension;
}

// Function to check if a menu item should be removed
bool ShouldRemoveMenuItem(const std::wstring& text, bool isGreyed) {
    std::wstring cleanText = NormalizeString(RemoveAmpersands(text));
    
    // Custom items are user-authored and additive: they must apply even
    // when the same label also exists in the predefined table with its
    // toggle off. Checking them first is why "Copy"/"Open*" README examples work.
    for (const auto& cleanCustomItem : g_settings.customItems) {
        if (MatchesCustomItem(cleanText, cleanCustomItem)) {
            return true;
        }
    }
    
    // Check against predefined items (single hash lookup instead of a
    // linear scan over all ~564 entries)
    auto it = g_menuItemsByText.find(cleanText);
    if (it != g_menuItemsByText.end()) {
        const MenuItem& item = g_menuItems[it->second];
        bool isEnabled = *(item.enabled);
        
        // If this item should only be removed when greyed out, check the state first
        if (item.greyedOnly && !isGreyed) {
            return false;
        }
        
        // Special handling for extension-filtered items (Notepad and WinRAR)
        if (item.allowedExtensions) {
            if (isEnabled) {
                // filterEnabled is set explicitly per entry (not inferred
                // from allowedExtensions), and the two session snapshots
                // avoid a live re-read that could disagree mid-session.
                bool filterOn = (item.filterEnabled == &g_settings.extensionFiltering.enableWinRARFiltering)
                    ? tl_sessionWinRARFilterOn
                    : tl_sessionExtensionFilterOn;
                
                if (filterOn) {
                    // Filter is on: only remove if extension is not in whitelist
                    return ShouldRemoveByExtension(item);
                } else {
                    // Filter is off: removal toggle wins, hide globally
                    return true;
                }
            }
            // If the removal setting is OFF, never remove this item
            return false;
        }
        
        // Normal behavior for non-extension-filtered items
        return isEnabled;
    }
    
    return false;
}

// Function to check if the modifier key bypass is active
bool IsModifierKeyBypassActive() {
    if (!g_settings.modifierKeyOverride.enableModifierOverride) {
        return false;
    }
    
    const std::wstring& key = g_settings.modifierKeyOverride.overrideKey;
    bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    
    if (key == L"ctrl") {
        return ctrlPressed;
    } else if (key == L"alt") {
        return altPressed;
    } else { // "both" - either key works
        return ctrlPressed || altPressed;
    }
}

// Function to process a menu and remove unwanted items
void ProcessMenu(HMENU hMenu) {
    if (!hMenu) return;
    
    int itemCount = GetMenuItemCount(hMenu);
    bool anyRemoved = false;
    
    // Iterate through menu items in reverse to safely remove items
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE | MIIM_STATE;
        
        bool deleted = false;
        
        // Get the length of the menu item text (this call also fills
        // fState, since MIIM_STATE is in fMask -- no separate round trip
        // needed to check greyed state).
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            std::wstring text;
            bool haveText = false;
            
            if (mii.cch > 0) {
                // Allocate buffer and get the actual text
                text.assign(mii.cch + 1, L'\0');
                mii.dwTypeData = &text[0];
                mii.cch++;
                
                if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
                    text.resize(wcslen(text.c_str()));
                    haveText = true;
                }
            } else {
                // mii.cch == 0 means a separator or an owner-drawn
                // (MFT_OWNERDRAW) item; GetMenuStringW returns nothing
                // useful for either, so there's no text to fall back to.
                haveText = false;
            }
            
            if (haveText) {
                bool isGreyed = (mii.fState & MFS_GRAYED) != 0;
                
                // Check if this item should be removed
                if (ShouldRemoveMenuItem(text, isGreyed)) {
                    // RemoveMenu detaches (doesn't destroy) the submenu so a
                    // third-party owner can still manage it; we now own the
                    // handle otherwise, so stash it for ExitMenuTracking().
                    HMENU hDetached = mii.hSubMenu;
                    RemoveMenu(hMenu, i, MF_BYPOSITION);
                    if (hDetached) {
                        tl_detachedSubmenus.push_back(hDetached);
                    }
                    deleted = true;
                    anyRemoved = true;
                }
            }
            
            // Recursively process submenus, but only if we didn't just
            // delete this item, and only when nothing else will re-filter
            // this submenu itself (see tl_recurseSubmenus).
            if (!deleted && mii.hSubMenu && tl_recurseSubmenus) {
                ProcessMenu(mii.hSubMenu);
            }
        }
    }
    
    // Separator cleanup below only ever matters if something was actually
    // removed from this menu -- an untouched menu's separators are already
    // however Explorer built it, so skip mutating it at all in that case.
    if (!anyRemoved) {
        return;
    }
    
    // Remove consecutive separators
    itemCount = GetMenuItemCount(hMenu);
    bool lastWasSeparator = false;
    
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            bool isSeparator = (mii.fType & MFT_SEPARATOR) != 0;
            
            if (isSeparator && lastWasSeparator) {
                // Remove consecutive separator
                DeleteMenu(hMenu, i, MF_BYPOSITION);
            }
            
            lastWasSeparator = isSeparator;
        }
    }
    
    // Remove leading separator if no items exist before it
    itemCount = GetMenuItemCount(hMenu);
    if (itemCount > 0) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, 0, TRUE, &mii)) {
            if (mii.fType & MFT_SEPARATOR) {
                DeleteMenu(hMenu, 0, MF_BYPOSITION);
            }
        }
    }
    
    // Remove trailing separator if exists
    itemCount = GetMenuItemCount(hMenu);
    if (itemCount > 0) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, itemCount - 1, TRUE, &mii)) {
            if (mii.fType & MFT_SEPARATOR) {
                DeleteMenu(hMenu, itemCount - 1, MF_BYPOSITION);
            }
        }
    }
}

// Original function pointers
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

// Tracks active thread-local menu hooks so Wh_ModUninit can force-unhook
// any still active -- a HHOOK outliving the mod's loaded image is a crash.
// g_uninitInProgress ensures no hook installs after Wh_ModUninit's sweep.
std::mutex g_activeMenuHooksMutex;
std::vector<HHOOK> g_activeMenuHooks;
bool g_uninitInProgress = false;

// WM_INITMENUPOPUP is a *sent* message delivered straight to the owner
// window's WndProc during the menu's modal loop, so a thread-local
// WH_CALLWNDPROCRET hook is how to intercept it (same as other merged mods).
thread_local HHOOK tl_hMenuHook = nullptr;
thread_local int tl_menuDepth = 0;
// Whether ProcessMenu() should recurse into submenus itself, rather than
// leaving them to MenuCallWndProcRetHook via WM_INITMENUPOPUP. True when
// no hook is installed, or when TPM_NONOTIFY means that message won't come.
thread_local bool tl_recurseSubmenus = true;
// Decided once per tracking session in EnterMenuTracking(), so the
// top-level menu and every submenu agree on whether the bypass is active,
// even if the Alt key state changes while the menu is still open.
thread_local bool tl_menuBypassed = false;
// True if this session successfully called CoInitializeEx (see
// EnterMenuTracking) and owns a matching CoUninitialize call in ExitMenuTracking.
thread_local bool tl_sessionComInitialized = false;
// Whether extension filtering needs the selected-file lookup, computed
// once per session in EnterMenuTracking() and reused by ProcessPopupMenu()
// instead of a second read that could disagree if settings change mid-session.
thread_local bool tl_sessionNeedFiles = false;
// Snapshots of the two filter toggles, taken alongside tl_sessionNeedFiles
// so a settings change mid-session can't disagree with the file lookup
// that already ran (or didn't) based on the toggles' state at menu-open time.
thread_local bool tl_sessionExtensionFilterOn = false;
thread_local bool tl_sessionWinRARFilterOn = false;

LRESULT CALLBACK MenuCallWndProcRetHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        CWPRETSTRUCT* cwp = (CWPRETSTRUCT*)lParam;
        if (cwp->message == WM_INITMENUPOPUP) {
            HMENU hSubMenu = (HMENU)cwp->wParam;
            if (hSubMenu) {
                // tl_filePaths was populated before tracking started and
                // stays valid for the whole session. Don't remove the
                // parent item when filtering empties a submenu -- it's already displayed, so mutating it risks a handle/layout bug.
                std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
                ProcessMenu(hSubMenu);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Installs the thread-local hook on the outermost TrackPopupMenu(Ex) call
// (nested calls bump the depth counter). SetWindowsHookEx runs outside
// g_activeMenuHooksMutex; COM inits once per session outside that lock too.
void EnterMenuTracking() {
    if (tl_menuDepth == 0) {
        tl_selectionLookupFailed = false;
        bool needFiles = false;
        {
            std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
            tl_menuBypassed = IsModifierKeyBypassActive();
            tl_sessionExtensionFilterOn = g_settings.extensionFiltering.enableExtensionFiltering;
            tl_sessionWinRARFilterOn = g_settings.extensionFiltering.enableWinRARFiltering;
            needFiles = tl_sessionExtensionFilterOn || tl_sessionWinRARFilterOn;
        }
        tl_sessionNeedFiles = needFiles;
        tl_recurseSubmenus = true;
        if (!tl_menuBypassed) {
            HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROCRET, MenuCallWndProcRetHook,
                                            nullptr, GetCurrentThreadId());
            if (hHook) {
                bool uninitRacing = false;
                {
                    std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
                    uninitRacing = g_uninitInProgress;
                    if (!uninitRacing) {
                        tl_hMenuHook = hHook;
                        g_activeMenuHooks.push_back(hHook);
                        // The hook will re-filter submenus via
                        // WM_INITMENUPOPUP; ProcessPopupMenu() flips this
                        // back if TPM_NONOTIFY means that won't happen.
                        tl_recurseSubmenus = false;
                    }
                }
                // Wh_ModUninit's sweep can't have seen this handle -- it
                // wasn't in the registry yet -- so unhook it ourselves.
                if (uninitRacing) {
                    UnhookWindowsHookEx(hHook);
                }
            } else {
                Wh_Log(L"SetWindowsHookEx failed (err %lu); submenus won't be filtered",
                       GetLastError());
            }
            if (needFiles) {
                HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                tl_sessionComInitialized = SUCCEEDED(hrCom);
            }
        }
    }
    tl_menuDepth++;
}

// Tears down the hook and clears the selected-file cache once the
// outermost TrackPopupMenu(Ex) call has returned.
void ExitMenuTracking() {
    tl_menuDepth--;
    if (tl_menuDepth <= 0) {
        tl_menuDepth = 0;
        if (tl_hMenuHook) {
            HHOOK hook = tl_hMenuHook;
            tl_hMenuHook = nullptr;
            // Unhook while still holding the lock, so the handle is always
            // either registered-and-hooked or removed-and-unhooked, never
            // caught in between -- see EnterMenuTracking() for the other half.
            std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
            if (std::erase(g_activeMenuHooks, hook) > 0) {
                UnhookWindowsHookEx(hook);
            }
        }
        if (tl_sessionComInitialized) {
            CoUninitialize();
            tl_sessionComInitialized = false;
        }
        // Runs after TrackPopupMenu(Ex)_Original has returned, so a handle
        // here is already unreachable from the shell's own destroyed tree.
        // RemoveMenu's contract makes us the sole owner of each handle.
        for (HMENU h : tl_detachedSubmenus) {
            DestroyMenu(h);
        }
        tl_detachedSubmenus.clear();
        tl_filePaths.clear();
    }
}

// RAII pairing for Enter/ExitMenuTracking, so a thrown exception between
// them (e.g. std::bad_alloc) can't leave the hook installed forever.
struct MenuTrackingGuard {
    MenuTrackingGuard() { EnterMenuTracking(); }
    ~MenuTrackingGuard() { ExitMenuTracking(); }
};

// Shared logic for both TrackPopupMenu(Ex) hooks: file lookup and, as a
// fallback, menu filtering for the top-level menu. kind is the caller's
// already-computed ClassifyShellView() result, reused to avoid a re-walk.
void ProcessPopupMenu(HMENU hMenu, HWND hWnd, UINT uFlags, ShellViewKind kind, const wchar_t* logPrefix) {
    if (tl_menuBypassed) {
        Wh_Log(L"%s: Modifier key bypass active, skipping menu processing", logPrefix);
        return;
    }
    
    // TPM_NONOTIFY suppresses WM_INITMENUPOPUP, so MenuCallWndProcRetHook
    // will never see this menu's submenus -- fall back to recursing here.
    if (uFlags & TPM_NONOTIFY) {
        tl_recurseSubmenus = true;
    }
    
    // Only do the (COM-based) selected-file lookup on the outermost call
    // of this session: TrackPopupMenu is implemented on top of
    // TrackPopupMenuEx, so a nested call would repeat the same COM round trip for an identical result.
    if (tl_menuDepth == 1) {
        if (tl_sessionNeedFiles) {
            tl_filePaths = GetSelectedFilesFromExplorer(hWnd, kind);
            
            if (!tl_filePaths.empty()) {
                Wh_Log(L"%s called with %d files:", logPrefix, (int)tl_filePaths.size());
                for (const auto& path : tl_filePaths) {
                    Wh_Log(L"  - %s (ext: %s)", path.c_str(), GetFileExtension(path).c_str());
                }
            } else {
                Wh_Log(L"%s called with no file context", logPrefix);
            }
        }
    }
    
    // MenuCallWndProcRetHook re-processes this menu via WM_INITMENUPOPUP,
    // but TPM_NONOTIFY can suppress that message, so run this pass
    // unconditionally too -- cheap, since an already-filtered menu removes nothing.
    {
        std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
        ProcessMenu(hMenu);
    }
    
    // NOTE: tl_filePaths is deliberately NOT cleared here -- it stays
    // populated for submenus processed later via MenuCallWndProcRetHook,
    // cleared in ExitMenuTracking() once the outermost call returns.
}

// Hook function for TrackPopupMenuEx
BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm
) {
    // Only filter file/folder context menus and the taskbar's menus.
    // TrackPopupMenuEx is hooked process-wide, so hWnd can also belong to
    // other windows, such as the Start menu.
    ShellViewKind kind = ClassifyShellView(hWnd);
    std::optional<MenuTrackingGuard> guard;
    if (kind != ShellViewKind::None) {
        guard.emplace();
        if (tl_menuDepth == 1) {
            // Outermost call for this session -- reset tl_filePaths here,
            // not before guard.emplace(), since clearing there would wipe
            // what an outer call already collected before a nested one skips repopulating it.
            tl_filePaths.clear();
        }
        ProcessPopupMenu(hMenu, hWnd, uFlags, kind, L"TrackPopupMenuEx");
    }
    
    BOOL bRes = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    
    return bRes;
}

// Hook function for TrackPopupMenu
BOOL WINAPI TrackPopupMenu_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    int nReserved,
    HWND hWnd,
    const RECT* prcRect
) {
    ShellViewKind kind = ClassifyShellView(hWnd);
    std::optional<MenuTrackingGuard> guard;
    if (kind != ShellViewKind::None) {
        guard.emplace();
        if (tl_menuDepth == 1) {
            tl_filePaths.clear();
        }
        ProcessPopupMenu(hMenu, hWnd, uFlags, kind, L"TrackPopupMenu");
    }
    
    BOOL bRes = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    
    return bRes;
}

// Shared cap for all settings arrays (customItems, notepadExtensions, winrarExtensions).
constexpr int kMaxSettingArrayItems = 100;

// Logs a warning if a settings array has more entries configured than the
// cap, so an overflowed 101st+ entry doesn't just silently vanish.
void WarnIfSettingArrayOverflows(const wchar_t* keyFormat, const wchar_t* displayName) {
    auto overflowCheck = WindhawkUtils::StringSetting::make(keyFormat, kMaxSettingArrayItems);
    if (*overflowCheck) {
        Wh_Log(L"WARNING: More than %d entries are configured for '%s'; entries beyond the first %d are ignored",
               kMaxSettingArrayItems, displayName, kMaxSettingArrayItems);
    }
}

// Load settings
void LoadSettings() {
    // Guard against a hook thread reading g_settings/g_menuItems while this
    // rebuilds them (see g_settingsMutex comment).
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    
    // Bloatware items
    g_settings.bloatwareItems.removeOneDrive = Wh_GetIntSetting(L"bloatwareItems.removeOneDrive");
    g_settings.bloatwareItems.removeCopilot = Wh_GetIntSetting(L"bloatwareItems.removeCopilot");
    g_settings.bloatwareItems.removeDefender = Wh_GetIntSetting(L"bloatwareItems.removeDefender");
    g_settings.bloatwareItems.removeDesigner = Wh_GetIntSetting(L"bloatwareItems.removeDesigner");
    g_settings.bloatwareItems.removeClipchamp = Wh_GetIntSetting(L"bloatwareItems.removeClipchamp");
    g_settings.bloatwareItems.removeMicrosoft365Copilot = Wh_GetIntSetting(L"bloatwareItems.removeMicrosoft365Copilot");
    
    // Basic items
    g_settings.basicItems.removeOpen = Wh_GetIntSetting(L"basicItems.removeOpen");
    g_settings.basicItems.removeOpenWith = Wh_GetIntSetting(L"basicItems.removeOpenWith");
    g_settings.basicItems.removeCut = Wh_GetIntSetting(L"basicItems.removeCut");
    g_settings.basicItems.removeCopy = Wh_GetIntSetting(L"basicItems.removeCopy");
    g_settings.basicItems.removeCreateShortcut = Wh_GetIntSetting(L"basicItems.removeCreateShortcut");
    g_settings.basicItems.removeDelete = Wh_GetIntSetting(L"basicItems.removeDelete");
    g_settings.basicItems.removeRename = Wh_GetIntSetting(L"basicItems.removeRename");
    g_settings.basicItems.removeSendTo = Wh_GetIntSetting(L"basicItems.removeSendTo");
    g_settings.basicItems.removeOpenInNewTab = Wh_GetIntSetting(L"basicItems.removeOpenInNewTab");
    g_settings.basicItems.removeOpenInNewWindow = Wh_GetIntSetting(L"basicItems.removeOpenInNewWindow");
    g_settings.basicItems.removeEdit = Wh_GetIntSetting(L"basicItems.removeEdit");
    g_settings.basicItems.removePlay = Wh_GetIntSetting(L"basicItems.removePlay");
    g_settings.basicItems.removePreview = Wh_GetIntSetting(L"basicItems.removePreview");
    g_settings.basicItems.removePrint = Wh_GetIntSetting(L"basicItems.removePrint");
    g_settings.basicItems.removeShare = Wh_GetIntSetting(L"basicItems.removeShare");
    g_settings.basicItems.removeRefresh = Wh_GetIntSetting(L"basicItems.removeRefresh");
    g_settings.basicItems.removeCopyAsPath = Wh_GetIntSetting(L"basicItems.removeCopyAsPath");
    g_settings.basicItems.removeCustomizeFolder = Wh_GetIntSetting(L"basicItems.removeCustomizeFolder");
    g_settings.basicItems.removeFavorites = Wh_GetIntSetting(L"basicItems.removeFavorites");
    g_settings.basicItems.removePinToQuickAccess = Wh_GetIntSetting(L"basicItems.removePinToQuickAccess");
    g_settings.basicItems.removePinToStart = Wh_GetIntSetting(L"basicItems.removePinToStart");
    g_settings.basicItems.removeCast = Wh_GetIntSetting(L"basicItems.removeCast");
    g_settings.basicItems.removeGiveAccess = Wh_GetIntSetting(L"basicItems.removeGiveAccess");
    g_settings.basicItems.removeRestoreVersions = Wh_GetIntSetting(L"basicItems.removeRestoreVersions");
    g_settings.basicItems.removeIncludeInLibrary = Wh_GetIntSetting(L"basicItems.removeIncludeInLibrary");
    g_settings.basicItems.removeRotate = Wh_GetIntSetting(L"basicItems.removeRotate");
    g_settings.basicItems.removeDisplaySettings = Wh_GetIntSetting(L"basicItems.removeDisplaySettings");
    g_settings.basicItems.removePersonalize = Wh_GetIntSetting(L"basicItems.removePersonalize");
    g_settings.basicItems.removeSetAsDesktopBackground = Wh_GetIntSetting(L"basicItems.removeSetAsDesktopBackground");
    g_settings.basicItems.removeView = Wh_GetIntSetting(L"basicItems.removeView");
    g_settings.basicItems.removeSortBy = Wh_GetIntSetting(L"basicItems.removeSortBy");
    g_settings.basicItems.removeGroupBy = Wh_GetIntSetting(L"basicItems.removeGroupBy");
    g_settings.basicItems.removeNew = Wh_GetIntSetting(L"basicItems.removeNew");
    g_settings.basicItems.removeProperties = Wh_GetIntSetting(L"basicItems.removeProperties");
    g_settings.basicItems.removePaste = Wh_GetIntSetting(L"basicItems.removePaste");
    g_settings.basicItems.removeExtractAll = Wh_GetIntSetting(L"basicItems.removeExtractAll");
    
    // App-specific items
    g_settings.appSpecificItems.removeSendWithQuickShare = Wh_GetIntSetting(L"appSpecificItems.removeSendWithQuickShare");
    g_settings.appSpecificItems.removeVLCPlaylist = Wh_GetIntSetting(L"appSpecificItems.removeVLCPlaylist");
    g_settings.appSpecificItems.removeVLCPlay = Wh_GetIntSetting(L"appSpecificItems.removeVLCPlay");
    g_settings.appSpecificItems.removeAddToMediaPlayerQueue = Wh_GetIntSetting(L"appSpecificItems.removeAddToMediaPlayerQueue");
    g_settings.appSpecificItems.removePlayWithMediaPlayer = Wh_GetIntSetting(L"appSpecificItems.removePlayWithMediaPlayer");
    g_settings.appSpecificItems.removeEditInNotepad = Wh_GetIntSetting(L"appSpecificItems.removeEditInNotepad");
    g_settings.appSpecificItems.removeEditInNotepadPlusPlus = Wh_GetIntSetting(L"appSpecificItems.removeEditInNotepadPlusPlus");
    g_settings.appSpecificItems.removeEditWithPhotos = Wh_GetIntSetting(L"appSpecificItems.removeEditWithPhotos");
    g_settings.appSpecificItems.removeEditWithPaint = Wh_GetIntSetting(L"appSpecificItems.removeEditWithPaint");
    g_settings.appSpecificItems.removeNvidiaApp = Wh_GetIntSetting(L"appSpecificItems.removeNvidiaApp");
    g_settings.appSpecificItems.removeOpenInTerminal = Wh_GetIntSetting(L"appSpecificItems.removeOpenInTerminal");
    g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice = Wh_GetIntSetting(L"appSpecificItems.removeAlwaysKeepOnThisDevice");
    g_settings.appSpecificItems.removeFreeUpSpace = Wh_GetIntSetting(L"appSpecificItems.removeFreeUpSpace");
    g_settings.appSpecificItems.removeWinRAR = Wh_GetIntSetting(L"appSpecificItems.removeWinRAR");
    
    // Extension filtering settings
    g_settings.extensionFiltering.enableExtensionFiltering = Wh_GetIntSetting(L"extensionFiltering.enableExtensionFiltering");
    
    // Load extension lists (lowercased once here instead of on every
    // IsExtensionInList() comparison)
    g_settings.extensionFiltering.notepadExtensions.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto ext = WindhawkUtils::StringSetting::make(L"extensionFiltering.notepadExtensions[%d]", i);
        if (!*ext) continue;
        std::wstring lowerExt = ToLower(ext.get());
        g_settings.extensionFiltering.notepadExtensions.push_back(lowerExt);
    }
    WarnIfSettingArrayOverflows(L"extensionFiltering.notepadExtensions[%d]", L"Notepad extensions");
    
    // Load custom items (normalized once here instead of on every
    // ShouldRemoveMenuItem() comparison)
    g_settings.customItems.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto customItem = WindhawkUtils::StringSetting::make(L"customItems[%d]", i);
        if (!*customItem) continue;
        
        // RemoveAmpersands is deliberately NOT applied here: it models
        // Windows' menu-text escaping, which applies to menu text, not to
        // what a user types (they type the visible text as-is).
        std::wstring cleanItem = NormalizeString(customItem.get());
        
        // A bare "*" matches every menu item and would empty the whole
        // context menu -- almost certainly a mistake, so refuse to load
        // it rather than let a stray keystroke leave no right-click menu.
        if (cleanItem == L"*") {
            Wh_Log(L"Ignoring custom item %d: a bare '*' would remove every menu item", i);
            continue;
        }
        
        // Only a trailing '*' is treated as a wildcard (see
        // MatchesCustomItem); a '*' anywhere else is matched literally
        // and will never match real menu text, so flag it for the user.
        size_t starPos = cleanItem.find(L'*');
        if (starPos != std::wstring::npos && starPos != cleanItem.length() - 1) {
            Wh_Log(L"WARNING: Custom item %d ('%s') has '*' before the end; only a trailing '*' is a wildcard, so this will be matched literally",
                   i, customItem.get());
        }
        
        g_settings.customItems.push_back(cleanItem);
        Wh_Log(L"Loaded custom item %d: %s", i, customItem.get());
    }
    WarnIfSettingArrayOverflows(L"customItems[%d]", L"Custom items");
    
    // WinRAR filtering settings (nested under extensionFiltering)
    g_settings.extensionFiltering.enableWinRARFiltering = Wh_GetIntSetting(L"extensionFiltering.enableWinRARFiltering");
    
    g_settings.extensionFiltering.winrarExtensions.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto ext = WindhawkUtils::StringSetting::make(L"extensionFiltering.winrarExtensions[%d]", i);
        if (!*ext) continue;
        std::wstring lowerExt = ToLower(ext.get());
        g_settings.extensionFiltering.winrarExtensions.push_back(lowerExt);
    }
    WarnIfSettingArrayOverflows(L"extensionFiltering.winrarExtensions[%d]", L"WinRAR extensions");
    
    // Modifier key override settings
    g_settings.modifierKeyOverride.enableModifierOverride = Wh_GetIntSetting(L"modifierKeyOverride.enableModifierOverride");
    {
        PCWSTR key = Wh_GetStringSetting(L"modifierKeyOverride.overrideKey");
        if (key) {
            g_settings.modifierKeyOverride.overrideKey = key;
            Wh_FreeStringSetting(key);
        } else {
            g_settings.modifierKeyOverride.overrideKey = L"ctrl";
        }
    }
    
    Wh_Log(L"Total custom items loaded: %d", (int)g_settings.customItems.size());
    Wh_Log(L"Notepad extension filtering enabled: %d", g_settings.extensionFiltering.enableExtensionFiltering);
    Wh_Log(L"Notepad extensions count: %d", (int)g_settings.extensionFiltering.notepadExtensions.size());
    Wh_Log(L"WinRAR extension filtering enabled: %d", g_settings.extensionFiltering.enableWinRARFiltering);
    Wh_Log(L"WinRAR extensions count: %d", (int)g_settings.extensionFiltering.winrarExtensions.size());
    Wh_Log(L"Modifier key override enabled: %d, key: %s", g_settings.modifierKeyOverride.enableModifierOverride, g_settings.modifierKeyOverride.overrideKey.c_str());
    
    InitializeMenuItems();
}

// True if the mod's current settings would actually remove anything. If
// nothing is toggled and there are no custom items, every popup gets
// hooked and walked for zero effect. Must be called after LoadSettings().
bool AnyRemovalConfigured() {
    for (const auto& item : g_menuItems) {
        if (*item.enabled) {
            return true;
        }
    }
    return !g_settings.customItems.empty();
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing context menu cleaner mod");
    
    LoadSettings();
    
    if (!AnyRemovalConfigured()) {
        // Nothing configured to remove -- unload rather than hook
        // TrackPopupMenu(Ex) process-wide for no effect. Windhawk reloads
        // an unloaded mod automatically once settings change again.
        Wh_Log(L"No items are configured to remove (all toggles off, no custom "
               L"items) -- unloading until settings change");
        return FALSE;
    }
    
    if (!WindhawkUtils::SetFunctionHook(
        TrackPopupMenuEx,
        TrackPopupMenuEx_Hook,
        &TrackPopupMenuEx_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }
    
    if (!WindhawkUtils::SetFunctionHook(
        TrackPopupMenu,
        TrackPopupMenu_Hook,
        &TrackPopupMenu_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }
    
    Wh_Log(L"Context menu cleaner mod initialized successfully");
    return TRUE;
}

// Settings changed callback
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
    *bReload = FALSE;
    // If nothing is left to remove, unload until settings change again
    // instead of an already-loaded mod keeping both hooks forever.
    return AnyRemovalConfigured();
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing context menu cleaner mod");
    
    // Force-unhook any thread-local menu hooks still installed -- a HHOOK
    // callback pointing into an unloaded mod image is a crash.
    // g_uninitInProgress ensures no new hook can slip past this sweep.
    std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
    g_uninitInProgress = true;
    for (HHOOK hook : g_activeMenuHooks) {
        UnhookWindowsHookEx(hook);
    }
    g_activeMenuHooks.clear();
}
