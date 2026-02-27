// ==WindhawkMod==
// @id              clipboard-history-upgrade
// @name            Smart Copy & Paste
// @name:tr-TR      Akıllı Kopyala & Yapıştır
// @name:de-DE      Intelligentes Kopieren & Einfügen
// @name:fr-FR      Copier-Coller Intelligent
// @name:es-ES      Copiar y Pegar Inteligente
// @name:pt-BR      Copiar e Colar Inteligente
// @name:it-IT      Copia e Incolla Intelligente
// @name:ru-RU      Умное Копирование и Вставка
// @name:uk-UA      Розумне Копіювання та Вставка
// @name:ja-JP      スマートコピー＆ペースト
// @name:ko-KR      스마트 복사 & 붙여넣기
// @name:zh-CN      智能复制粘贴
// @name:zh-TW      智慧複製貼上
// @name:pl-PL      Inteligentne Kopiowanie i Wklejanie
// @name:nl-NL      Slim Kopiëren & Plakken
// @description     Automatically format, clean, and enrich text instantly as you copy it to the clipboard.
// @description:tr-TR Metni panoya kopyaladığınız anda otomatik olarak biçimlendirin, temizleyin ve zenginleştirin.
// @description:de-DE Text beim Kopieren in die Zwischenablage automatisch formatieren, bereinigen und anreichern.
// @description:fr-FR Formatez, nettoyez et enrichissez automatiquement le texte dès que vous le copiez dans le presse-papiers.
// @description:es-ES Formatee, limpie y enriquezca texto automáticamente al copiarlo al portapapeles.
// @description:pt-BR Formate, limpe e enriqueça texto automaticamente ao copiá-lo para a área de transferência.
// @description:it-IT Formatta, pulisci e arricchisci automaticamente il testo quando lo copi negli appunti.
// @description:ru-RU Автоматическое форматирование, очистка и обогащение текста при копировании в буфер обмена.
// @description:uk-UA Автоматичне форматування, очищення та збагачення тексту при копіюванні в буфер обміну.
// @description:ja-JP クリップボードにコピーしたテキストを自動的にフォーマット、クリーンアップ、リッチ化します。
// @description:ko-KR 클립보드에 복사한 텍스트를 자동으로 포맷, 정리 및 보강합니다.
// @description:zh-CN 复制到剪贴板时自动格式化、清理和丰富文本内容。
// @description:zh-TW 複製到剪貼簿時自動格式化、清理和豐富文字內容。
// @description:pl-PL Automatyczne formatowanie, czyszczenie i wzbogacanie tekstu podczas kopiowania do schowka.
// @description:nl-NL Tekst automatisch opmaken, opschonen en verrijken bij het kopiëren naar het klembord.
// @version         1.3.2
// @author          SwiftExplorer567
// @github          https://github.com/SwiftExplorer567
// @homepage        https://v0.hasanjws.com/user/hasanjws
// @include         *
// @compilerOptions -luser32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 📋 Smart Copy & Paste

**Enhance your everyday copy and paste experience by automatically cleaning, formatting, and upgrading text the moment you copy it.**

Whether you're pasting normally (`Ctrl+V`) or using the Windows Clipboard History (`Win+V`), your text will already be formatted exactly how you need it!

---

## ✨ Core Features

*   **⚡ Regex-based formatting:** Automatically replace text based on your custom rules. Perfect for fixing common typos or replacing specific words on the fly.
*   **🛡️ Remove Tracking Variables:** Automatically strip invasive `utm_`, `fbclid`, `gclid`, and other marketing parameters from copied URLs before sharing them.
*   **✂️ Auto-Trim Whitespace:** Instantly strip invisible leading/trailing spaces, tabs, and newlines that are accidentally included during text selection.
*   **📄 PDF Text Unwrapper:** Merge broken text strings back into fluid paragraphs. Extremely useful when copying tabular data or text from narrow PDF columns.
*   **🔠 Smart Casing:** Auto-convert copied text into `lowercase`, `UPPERCASE`, or `Title Case`.
*   **💻 Code Path Auto-Escaper:** Detects Windows file paths (e.g., `C:\Users\file.txt`) and automatically escapes the backslashes (`C:\\` or `C:/`) so they are instantly ready to paste into code.
*   **📥 Data Extractor:** Instead of copying bulk text, cleanly extract *only* the URLs or Email Addresses found within a massive block of text.
*   **📝 Markdown to Rich Text:** Type simple Markdown (like `**bold**` or `[links](url)`) and have it automatically converted into actual Rich Text (`CF_HTML`) on the clipboard.
*   **🚫 Force Plain Text:** Strip all annoying rich formatting (HTML, RTF, fonts, colors) from the source application, ensuring text always pastes matching the destination format.
*   **⌨️ Trigger Modifier Key:** Optionally restrict all these features so they *only* apply when holding a specific key (Shift/Alt) while copying.

---

## 🌍 Localization

The mod's UI (setting names, descriptions, and options) is fully translated into **14 languages**:

English, Türkçe, Deutsch, Français, Español, Português, Italiano, Русский, Українська, 日本語, 한국어, 简体中文, 繁體中文, Polski, Nederlands

🇺🇸 🇹🇷 🇩🇪 🇫🇷 🇪🇸 🇧🇷 🇮🇹 🇷🇺 🇺🇦 🇯🇵 🇰🇷 🇨🇳 🇹🇼 🇵🇱 🇳🇱

Windhawk will automatically display the mod in your system language if supported.

---

### 💡 Why use this?
Instead of manually cleaning up tracking URLs, manually escaping backslashes, or continuously using *Paste as Plain Text* (Ctrl+Shift+V), this mod intercepts the Windows clipboard at the system level and sanitizes the data instantly.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Core:
  - TriggerModifierKey: none
    $name: ⌨️ Trigger modifier key
    $name:tr-TR: ⌨️ Tetikleyici değiştirici tuş
    $name:de-DE: ⌨️ Auslöser-Modifikatortaste
    $name:fr-FR: ⌨️ Touche modificatrice
    $name:es-ES: ⌨️ Tecla modificadora
    $name:pt-BR: ⌨️ Tecla modificadora
    $name:it-IT: ⌨️ Tasto modificatore
    $name:ru-RU: ⌨️ Клавиша-модификатор
    $name:uk-UA: ⌨️ Клавіша-модифікатор
    $name:ja-JP: ⌨️ トリガー修飾キー
    $name:ko-KR: ⌨️ 트리거 수정 키
    $name:zh-CN: ⌨️ 触发修饰键
    $name:zh-TW: ⌨️ 觸發修飾鍵
    $name:pl-PL: ⌨️ Klawisz modyfikujący
    $name:nl-NL: ⌨️ Triggermodi-toets
    $description: >-
      Only apply formatting if this key is held while copying. If none is selected, formatting always applies.
    $description:tr-TR: >-
      Biçimlendirmeyi yalnızca kopyalama sırasında bu tuş basılıyken uygula. Hiçbiri seçilmezse biçimlendirme her zaman uygulanır.
    $description:de-DE: >-
      Formatierung nur anwenden, wenn diese Taste beim Kopieren gedrückt wird. Wenn keine ausgewählt ist, wird immer formatiert.
    $description:fr-FR: >-
      Appliquer le formatage uniquement si cette touche est maintenue lors de la copie. Si aucune n'est sélectionnée, le formatage s'applique toujours.
    $description:es-ES: >-
      Aplicar formato solo si esta tecla se mantiene presionada al copiar. Si no se selecciona ninguna, el formato siempre se aplica.
    $description:pt-BR: >-
      Aplicar formatação somente se esta tecla estiver pressionada ao copiar. Se nenhuma for selecionada, a formatação sempre se aplica.
    $description:it-IT: >-
      Applica la formattazione solo se questo tasto è premuto durante la copia. Se non selezionato, la formattazione si applica sempre.
    $description:ru-RU: >-
      Применять форматирование только при удержании этой клавиши во время копирования. Если не выбрано, форматирование применяется всегда.
    $description:uk-UA: >-
      Застосовувати форматування лише при утриманні цієї клавіші під час копіювання. Якщо не вибрано, форматування застосовується завжди.
    $description:ja-JP: >-
      コピー時にこのキーを押している場合のみフォーマットを適用します。何も選択されていない場合、常にフォーマットが適用されます。
    $description:ko-KR: >-
      복사할 때 이 키를 누르고 있을 때만 서식을 적용합니다. 선택하지 않으면 항상 서식이 적용됩니다.
    $description:zh-CN: >-
      仅在复制时按住此键才应用格式化。如果未选择，则始终应用格式化。
    $description:zh-TW: >-
      僅在複製時按住此鍵才套用格式化。如果未選擇，則始終套用格式化。
      تطبيق التنسيق فقط عند الضغط على هذا المفتاح أثناء النسخ. إذا لم يتم التحديد، يُطبق التنسيق دائمًا.
    $description:pl-PL: >-
      Zastosuj formatowanie tylko, gdy ten klawisz jest wciśnięty podczas kopiowania. Jeśli nie wybrano, formatowanie jest zawsze stosowane.
    $description:nl-NL: >-
      Pas opmaak alleen toe als deze toets wordt ingedrukt tijdens het kopiëren. Als er geen is geselecteerd, wordt opmaak altijd toegepast.
    $options:
    - none: None (always process)
    - shift: Shift
    - alt: Alt
    $options:tr-TR:
    - none: Yok (her zaman işle)
    - shift: Shift
    - alt: Alt
    $options:de-DE:
    - none: Keine (immer verarbeiten)
    - shift: Shift
    - alt: Alt
    $options:fr-FR:
    - none: Aucune (toujours traiter)
    - shift: Shift
    - alt: Alt
    $options:es-ES:
    - none: Ninguna (siempre procesar)
    - shift: Shift
    - alt: Alt
    $options:pt-BR:
    - none: Nenhuma (sempre processar)
    - shift: Shift
    - alt: Alt
    $options:it-IT:
    - none: Nessuno (elabora sempre)
    - shift: Shift
    - alt: Alt
    $options:ru-RU:
    - none: Нет (всегда обрабатывать)
    - shift: Shift
    - alt: Alt
    $options:uk-UA:
    - none: Немає (завжди обробляти)
    - shift: Shift
    - alt: Alt
    $options:ja-JP:
    - none: なし（常に処理）
    - shift: Shift
    - alt: Alt
    $options:ko-KR:
    - none: 없음 (항상 처리)
    - shift: Shift
    - alt: Alt
    $options:zh-CN:
    - none: 无（始终处理）
    - shift: Shift
    - alt: Alt
    $options:zh-TW:
    - none: 無（始終處理）
    - shift: Shift
    - alt: Alt
    - none: لا شيء (معالجة دائمًا)
    - shift: Shift
    - alt: Alt
    $options:pl-PL:
    - none: Brak (zawsze przetwarzaj)
    - shift: Shift
    - alt: Alt
    $options:nl-NL:
    - none: Geen (altijd verwerken)
    - shift: Shift
    - alt: Alt
  $name: ⚙️ Core & Triggers
  $name:tr-TR: ⚙️ Çekirdek & Tetikleyiciler
  $name:de-DE: ⚙️ Kern & Auslöser
  $name:fr-FR: ⚙️ Base & Déclencheurs
  $name:es-ES: ⚙️ Núcleo & Activadores
  $name:pt-BR: ⚙️ Núcleo & Gatilhos
  $name:it-IT: ⚙️ Base & Attivatori
  $name:ru-RU: ⚙️ Основные & Триггеры
  $name:uk-UA: ⚙️ Основні & Тригери
  $name:ja-JP: ⚙️ コア & トリガー
  $name:ko-KR: ⚙️ 코어 & 트리거
  $name:zh-CN: ⚙️ 核心与触发器
  $name:zh-TW: ⚙️ 核心與觸發器
  $name:pl-PL: ⚙️ Rdzeń & Wyzwalacze
  $name:nl-NL: ⚙️ Kern & Triggers

- CleanupAndFormatting:
  - RemoveTrackingParams: false
    $name: 🛡️ Remove tracking parameters
    $name:tr-TR: 🛡️ İzleme parametrelerini kaldır
    $name:de-DE: 🛡️ Tracking-Parameter entfernen
    $name:fr-FR: 🛡️ Supprimer les paramètres de suivi
    $name:es-ES: 🛡️ Eliminar parámetros de seguimiento
    $name:pt-BR: 🛡️ Remover parâmetros de rastreamento
    $name:it-IT: 🛡️ Rimuovi parametri di tracciamento
    $name:ru-RU: 🛡️ Удалить параметры отслеживания
    $name:uk-UA: 🛡️ Видалити параметри відстеження
    $name:ja-JP: 🛡️ トラッキングパラメータを削除
    $name:ko-KR: 🛡️ 추적 매개변수 제거
    $name:zh-CN: 🛡️ 移除跟踪参数
    $name:zh-TW: 🛡️ 移除追蹤參數
    $name:pl-PL: 🛡️ Usuń parametry śledzenia
    $name:nl-NL: 🛡️ Trackingparameters verwijderen
    $description: >-
      Automatically strip utm_source, fbclid, gclid, and other
      common tracking parameters from copied URLs.
    $description:tr-TR: >-
      Kopyalanan URL'lerden utm_source, fbclid, gclid ve diğer yaygın izleme parametrelerini otomatik olarak kaldırır.
    $description:de-DE: >-
      utm_source, fbclid, gclid und andere Tracking-Parameter aus kopierten URLs automatisch entfernen.
    $description:fr-FR: >-
      Supprimer automatiquement utm_source, fbclid, gclid et autres paramètres de suivi des URLs copiées.
    $description:es-ES: >-
      Eliminar automáticamente utm_source, fbclid, gclid y otros parámetros de seguimiento de las URLs copiadas.
    $description:pt-BR: >-
      Remover automaticamente utm_source, fbclid, gclid e outros parâmetros de rastreamento das URLs copiadas.
    $description:it-IT: >-
      Rimuovi automaticamente utm_source, fbclid, gclid e altri parametri di tracciamento dagli URL copiati.
    $description:ru-RU: >-
      Автоматически удалять utm_source, fbclid, gclid и другие параметры отслеживания из скопированных URL.
    $description:uk-UA: >-
      Автоматично видаляти utm_source, fbclid, gclid та інші параметри відстеження зі скопійованих URL.
    $description:ja-JP: >-
      コピーしたURLからutm_source、fbclid、gclidなどのトラッキングパラメータを自動的に除去します。
    $description:ko-KR: >-
      복사한 URL에서 utm_source, fbclid, gclid 등 추적 매개변수를 자동으로 제거합니다.
    $description:zh-CN: >-
      自动从复制的URL中移除utm_source、fbclid、gclid等跟踪参数。
    $description:zh-TW: >-
      自動從複製的URL中移除utm_source、fbclid、gclid等追蹤參數。
      إزالة utm_source و fbclid و gclid ومعلمات التتبع الأخرى تلقائيًا من عناوين URL المنسوخة.
    $description:pl-PL: >-
      Automatycznie usuwa utm_source, fbclid, gclid i inne parametry śledzenia ze skopiowanych adresów URL.
    $description:nl-NL: >-
      Verwijder automatisch utm_source, fbclid, gclid en andere trackingparameters uit gekopieerde URL's.
  - ForcePlainText: false
    $name: 🚫 Force plain text
    $name:tr-TR: 🚫 Düz metin zorla
    $name:de-DE: 🚫 Nur-Text erzwingen
    $name:fr-FR: 🚫 Forcer le texte brut
    $name:es-ES: 🚫 Forzar texto sin formato
    $name:pt-BR: 🚫 Forçar texto simples
    $name:it-IT: 🚫 Forza testo semplice
    $name:ru-RU: 🚫 Принудительный простой текст
    $name:uk-UA: 🚫 Примусовий простий текст
    $name:ja-JP: 🚫 プレーンテキストを強制
    $name:ko-KR: 🚫 일반 텍스트 강제
    $name:zh-CN: 🚫 强制纯文本
    $name:zh-TW: 🚫 強制純文字
    $name:pl-PL: 🚫 Wymuś zwykły tekst
    $name:nl-NL: 🚫 Platte tekst forceren
    $description: >-
      Strip all rich formatting (HTML, RTF, images) from the source
      application so that text always pastes as plain, unformatted text.
    $description:tr-TR: >-
      Kaynak uygulamadan tüm zengin biçimlendirmeyi (HTML, RTF, resimler) kaldırır, böylece metin her zaman düz, biçimsiz olarak yapıştırılır.
    $description:de-DE: >-
      Alle Formatierungen (HTML, RTF, Bilder) entfernen, damit Text immer als unformatierter Text eingefügt wird.
    $description:fr-FR: >-
      Supprimer tout le formatage riche (HTML, RTF, images) pour que le texte soit toujours collé en texte brut.
    $description:es-ES: >-
      Eliminar todo el formato enriquecido (HTML, RTF, imágenes) para que el texto siempre se pegue como texto sin formato.
    $description:pt-BR: >-
      Remover toda a formatação rica (HTML, RTF, imagens) para que o texto sempre seja colado como texto simples.
    $description:it-IT: >-
      Rimuovi tutta la formattazione ricca (HTML, RTF, immagini) in modo che il testo venga sempre incollato come testo semplice.
    $description:ru-RU: >-
      Удалить всё форматирование (HTML, RTF, изображения), чтобы текст всегда вставлялся как простой текст.
    $description:uk-UA: >-
      Видалити все форматування (HTML, RTF, зображення), щоб текст завжди вставлявся як простий текст.
    $description:ja-JP: >-
      ソースアプリケーションからすべてのリッチフォーマット（HTML、RTF、画像）を除去し、常にプレーンテキストとして貼り付けます。
    $description:ko-KR: >-
      소스 애플리케이션에서 모든 서식(HTML, RTF, 이미지)을 제거하여 항상 서식 없는 텍스트로 붙여넣습니다.
    $description:zh-CN: >-
      从源应用程序中移除所有富文本格式（HTML、RTF、图片），使文本始终以纯文本粘贴。
    $description:zh-TW: >-
      從來源應用程式中移除所有富文本格式（HTML、RTF、圖片），使文字始終以純文字貼上。
      إزالة جميع التنسيقات الغنية (HTML، RTF، الصور) بحيث يُلصق النص دائمًا كنص عادي.
    $description:pl-PL: >-
      Usuń całe formatowanie (HTML, RTF, obrazy), aby tekst zawsze był wklejany jako zwykły tekst.
    $description:nl-NL: >-
      Verwijder alle opmaak (HTML, RTF, afbeeldingen) zodat tekst altijd als platte tekst wordt geplakt.
  - AutoTrimWhitespace: false
    $name: ✂️ Auto-trim whitespace
    $name:tr-TR: ✂️ Boşlukları otomatik kırp
    $name:de-DE: ✂️ Leerzeichen automatisch entfernen
    $name:fr-FR: ✂️ Supprimer les espaces automatiquement
    $name:es-ES: ✂️ Recortar espacios automáticamente
    $name:pt-BR: ✂️ Aparar espaços automaticamente
    $name:it-IT: ✂️ Rimuovi spazi automaticamente
    $name:ru-RU: ✂️ Автоудаление пробелов
    $name:uk-UA: ✂️ Автовидалення пробілів
    $name:ja-JP: ✂️ 空白の自動トリミング
    $name:ko-KR: ✂️ 공백 자동 제거
    $name:zh-CN: ✂️ 自动修剪空白
    $name:zh-TW: ✂️ 自動修剪空白
    $name:pl-PL: ✂️ Automatyczne przycinanie spacji
    $name:nl-NL: ✂️ Witruimte automatisch bijsnijden
    $description: >-
      Remove leading and trailing spaces, tabs, and newlines
      that are often accidentally included when selecting text.
    $description:tr-TR: >-
      Metin seçerken genellikle yanlışlıkla dahil edilen baştaki ve sondaki boşlukları, sekmeleri ve satır sonlarını kaldırır.
    $description:de-DE: >-
      Führende und nachfolgende Leerzeichen, Tabs und Zeilenumbrüche entfernen, die oft versehentlich bei der Textauswahl enthalten sind.
    $description:fr-FR: >-
      Supprimer les espaces, tabulations et sauts de ligne en début et fin de texte, souvent inclus accidentellement lors de la sélection.
    $description:es-ES: >-
      Eliminar espacios, tabulaciones y saltos de línea al inicio y final que a menudo se incluyen accidentalmente al seleccionar texto.
    $description:pt-BR: >-
      Remover espaços, tabulações e quebras de linha no início e no final que são frequentemente incluídos acidentalmente ao selecionar texto.
    $description:it-IT: >-
      Rimuovi spazi, tabulazioni e interruzioni di riga iniziali e finali spesso inclusi accidentalmente durante la selezione del testo.
    $description:ru-RU: >-
      Удалить начальные и конечные пробелы, табуляции и переносы строк, часто случайно включаемые при выделении текста.
    $description:uk-UA: >-
      Видалити початкові та кінцеві пробіли, табуляції та розриви рядків, які часто випадково включаються при виділенні тексту.
    $description:ja-JP: >-
      テキスト選択時に誤って含まれがちな先頭と末尾の空白、タブ、改行を削除します。
    $description:ko-KR: >-
      텍스트 선택 시 실수로 포함되는 앞뒤 공백, 탭, 줄바꿈을 제거합니다.
    $description:zh-CN: >-
      移除选择文本时经常意外包含的前导和尾随空格、制表符和换行符。
    $description:zh-TW: >-
      移除選取文字時經常意外包含的前導和尾隨空格、定位字元和換行符。
      إزالة المسافات والجدولة وأسطر جديدة في البداية والنهاية التي غالبًا ما يتم تضمينها عن طريق الخطأ عند تحديد النص.
    $description:pl-PL: >-
      Usuń wiodące i końcowe spacje, tabulatory i znaki nowej linii, często przypadkowo dołączane podczas zaznaczania tekstu.
    $description:nl-NL: >-
      Verwijder begin- en eindspaties, tabs en nieuwe regels die vaak per ongeluk worden meegenomen bij het selecteren van tekst.
  - UnwrapText: false
    $name: 📄 Unwrap text (PDF fixer)
    $name:tr-TR: 📄 Metni düzleştir (PDF düzeltici)
    $name:de-DE: 📄 Text zusammenführen (PDF-Fixer)
    $name:fr-FR: 📄 Fusionner le texte (correcteur PDF)
    $name:es-ES: 📄 Unir texto (corrector PDF)
    $name:pt-BR: 📄 Unir texto (corretor de PDF)
    $name:it-IT: 📄 Unisci testo (correttore PDF)
    $name:ru-RU: 📄 Объединение текста (исправление PDF)
    $name:uk-UA: 📄 Об'єднання тексту (виправлення PDF)
    $name:ja-JP: 📄 テキスト結合（PDF修正）
    $name:ko-KR: 📄 텍스트 병합 (PDF 수정)
    $name:zh-CN: 📄 合并文本（PDF修复）
    $name:zh-TW: 📄 合併文字（PDF修復）
    $name:pl-PL: 📄 Łączenie tekstu (naprawa PDF)
    $name:nl-NL: 📄 Tekst samenvoegen (PDF-fixer)
    $description: >-
      Merge broken lines back into flowing paragraphs.
      Useful when copying text from PDFs or narrow columns
      that insert hard line breaks mid-sentence.
      Paragraph breaks (double newlines) are preserved.
    $description:tr-TR: >-
      Kırık satırları akıcı paragraflara geri birleştirir. PDF'lerden veya dar sütunlardan metin kopyalarken kullanışlıdır. Paragraf araları (çift satır sonu) korunur.
    $description:de-DE: >-
      Umbrochene Zeilen wieder zu fließenden Absätzen zusammenführen. Nützlich beim Kopieren aus PDFs oder schmalen Spalten. Absatzumbrüche (doppelte Zeilenumbrüche) bleiben erhalten.
    $description:fr-FR: >-
      Fusionner les lignes coupées en paragraphes fluides. Utile pour copier depuis des PDF ou des colonnes étroites. Les sauts de paragraphe (doubles sauts de ligne) sont préservés.
    $description:es-ES: >-
      Unir líneas rotas en párrafos fluidos. Útil al copiar desde PDF o columnas estrechas. Los saltos de párrafo (doble salto de línea) se conservan.
    $description:pt-BR: >-
      Unir linhas quebradas em parágrafos fluidos. Útil ao copiar de PDFs ou colunas estreitas. Quebras de parágrafo (linhas duplas) são preservadas.
    $description:it-IT: >-
      Unisci le righe spezzate in paragrafi fluidi. Utile quando si copia da PDF o colonne strette. Le interruzioni di paragrafo (doppio a capo) vengono preservate.
    $description:ru-RU: >-
      Объединить разбитые строки в плавные абзацы. Полезно при копировании из PDF или узких столбцов. Разрывы абзацев (двойные переносы) сохраняются.
    $description:uk-UA: >-
      Об'єднати розбиті рядки у плавні абзаци. Корисно при копіюванні з PDF або вузьких стовпців. Розриви абзаців (подвійні переноси) зберігаються.
    $description:ja-JP: >-
      改行で分割されたテキストを流れるような段落に結合します。PDFや狭いカラムからのコピー時に便利です。段落区切り（二重改行）は保持されます。
    $description:ko-KR: >-
      끊어진 줄을 흐르는 단락으로 병합합니다. PDF나 좁은 열에서 텍스트를 복사할 때 유용합니다. 단락 구분(이중 줄바꿈)은 유지됩니다.
    $description:zh-CN: >-
      将断行合并回流畅的段落。从PDF或窄列复制文本时非常有用。段落分隔（双换行）会被保留。
    $description:zh-TW: >-
      將斷行合併回流暢的段落。從PDF或窄欄複製文字時非常有用。段落分隔（雙換行）會被保留。
      دمج الأسطر المقطوعة في فقرات متدفقة. مفيد عند النسخ من ملفات PDF أو الأعمدة الضيقة. يتم الحفاظ على فواصل الفقرات.
    $description:pl-PL: >-
      Łączy przerwane linie w płynne akapity. Przydatne podczas kopiowania z PDF lub wąskich kolumn. Podziały akapitów (podwójne znaki nowej linii) są zachowane.
    $description:nl-NL: >-
      Voeg afgebroken regels samen tot vloeiende alinea's. Handig bij kopiëren uit PDF's of smalle kolommen. Alinea-einden (dubbele regelovergangen) worden behouden.
  - CasingMode: none
    $name: 🔠 Smart casing
    $name:tr-TR: 🔠 Akıllı büyük/küçük harf
    $name:de-DE: 🔠 Intelligente Groß-/Kleinschreibung
    $name:fr-FR: 🔠 Casse intelligente
    $name:es-ES: 🔠 Mayúsculas inteligentes
    $name:pt-BR: 🔠 Capitalização inteligente
    $name:it-IT: 🔠 Maiuscole intelligenti
    $name:ru-RU: 🔠 Умный регистр
    $name:uk-UA: 🔠 Розумний регістр
    $name:ja-JP: 🔠 スマートケース変換
    $name:ko-KR: 🔠 스마트 대소문자
    $name:zh-CN: 🔠 智能大小写
    $name:zh-TW: 🔠 智慧大小寫
    $name:pl-PL: 🔠 Inteligentna wielkość liter
    $name:nl-NL: 🔠 Slim hoofdlettergebruik
    $description: >-
      Automatically convert copied text to the selected casing style.
    $description:tr-TR: >-
      Kopyalanan metni otomatik olarak seçilen büyük/küçük harf stiline dönüştürür.
    $description:de-DE: >-
      Kopierten Text automatisch in den gewählten Schreibstil konvertieren.
    $description:fr-FR: >-
      Convertir automatiquement le texte copié dans le style de casse sélectionné.
    $description:es-ES: >-
      Convertir automáticamente el texto copiado al estilo de mayúsculas seleccionado.
    $description:pt-BR: >-
      Converter automaticamente o texto copiado para o estilo de capitalização selecionado.
    $description:it-IT: >-
      Converti automaticamente il testo copiato nello stile di maiuscole selezionato.
    $description:ru-RU: >-
      Автоматически преобразовывать скопированный текст в выбранный стиль регистра.
    $description:uk-UA: >-
      Автоматично перетворювати скопійований текст у вибраний стиль регістру.
    $description:ja-JP: >-
      コピーしたテキストを選択したケーススタイルに自動変換します。
    $description:ko-KR: >-
      복사한 텍스트를 선택한 대소문자 스타일로 자동 변환합니다.
    $description:zh-CN: >-
      自动将复制的文本转换为选定的大小写样式。
    $description:zh-TW: >-
      自動將複製的文字轉換為選定的大小寫樣式。
      تحويل النص المنسوخ تلقائيًا إلى نمط حالة الأحرف المحدد.
    $description:pl-PL: >-
      Automatycznie konwertuj skopiowany tekst na wybrany styl wielkości liter.
    $description:nl-NL: >-
      Kopieer tekst automatisch naar de geselecteerde hoofdletterstijl.
    $options:
    - none: None (no change)
    - lowercase: lowercase
    - uppercase: UPPERCASE
    - titlecase: Title Case
    $options:tr-TR:
    - none: Yok (değişiklik yok)
    - lowercase: küçük harf
    - uppercase: BÜYÜK HARF
    - titlecase: Başlık Stili
    $options:de-DE:
    - none: Keine (keine Änderung)
    - lowercase: kleinbuchstaben
    - uppercase: GROSSBUCHSTABEN
    - titlecase: Titelschreibung
    $options:fr-FR:
    - none: Aucun (pas de changement)
    - lowercase: minuscules
    - uppercase: MAJUSCULES
    - titlecase: Casse De Titre
    $options:es-ES:
    - none: Ninguno (sin cambio)
    - lowercase: minúsculas
    - uppercase: MAYÚSCULAS
    - titlecase: Tipo Título
    $options:pt-BR:
    - none: Nenhum (sem alteração)
    - lowercase: minúsculas
    - uppercase: MAIÚSCULAS
    - titlecase: Tipo Título
    $options:it-IT:
    - none: Nessuno (nessuna modifica)
    - lowercase: minuscolo
    - uppercase: MAIUSCOLO
    - titlecase: Stile Titolo
    $options:ru-RU:
    - none: Нет (без изменений)
    - lowercase: нижний регистр
    - uppercase: ВЕРХНИЙ РЕГИСТР
    - titlecase: Каждое Слово С Заглавной
    $options:uk-UA:
    - none: Немає (без змін)
    - lowercase: нижній регістр
    - uppercase: ВЕРХНІЙ РЕГІСТР
    - titlecase: Кожне Слово З Великої
    $options:ja-JP:
    - none: なし（変更なし）
    - lowercase: 小文字
    - uppercase: 大文字
    - titlecase: タイトルケース
    $options:ko-KR:
    - none: 없음 (변경 없음)
    - lowercase: 소문자
    - uppercase: 대문자
    - titlecase: 제목 스타일
    $options:zh-CN:
    - none: 无（不更改）
    - lowercase: 小写
    - uppercase: 大写
    - titlecase: 标题格式
    $options:zh-TW:
    - none: 無（不更改）
    - lowercase: 小寫
    - uppercase: 大寫
    - titlecase: 標題格式
    - none: لا شيء (بدون تغيير)
    - lowercase: أحرف صغيرة
    - uppercase: أحرف كبيرة
    - titlecase: نمط العنوان
    $options:pl-PL:
    - none: Brak (bez zmian)
    - lowercase: małe litery
    - uppercase: WIELKIE LITERY
    - titlecase: Każde Słowo Wielką Literą
    $options:nl-NL:
    - none: Geen (geen wijziging)
    - lowercase: kleine letters
    - uppercase: HOOFDLETTERS
    - titlecase: Titelhoofdletters
  - SmartCasingExcludeUrls: true
    $name: 🔗 Exclude URLs from smart casing
    $name:tr-TR: 🔗 URL'leri akıllı büyük/küçük harften hariç tut
    $name:de-DE: 🔗 URLs von Groß-/Kleinschreibung ausschließen
    $name:fr-FR: 🔗 Exclure les URLs de la casse intelligente
    $name:es-ES: 🔗 Excluir URLs de mayúsculas inteligentes
    $name:pt-BR: 🔗 Excluir URLs da capitalização inteligente
    $name:it-IT: 🔗 Escludi URL dalle maiuscole intelligenti
    $name:ru-RU: 🔗 Исключить URL из умного регистра
    $name:uk-UA: 🔗 Виключити URL з розумного регістру
    $name:ja-JP: 🔗 URLをスマートケースから除外
    $name:ko-KR: 🔗 스마트 대소문자에서 URL 제외
    $name:zh-CN: 🔗 从智能大小写中排除URL
    $name:zh-TW: 🔗 從智慧大小寫中排除URL
    $name:pl-PL: 🔗 Wyklucz adresy URL z inteligentnej wielkości liter
    $name:nl-NL: 🔗 URL's uitsluiten van slim hoofdlettergebruik
    $description: >-
      Do not change the casing of URLs (http://... or https://...) when smart casing is enabled.
    $description:tr-TR: >-
      Akıllı büyük/küçük harf etkinken URL'lerin (http://... veya https://...) büyük/küçük harfini değiştirme.
    $description:de-DE: >-
      Die Groß-/Kleinschreibung von URLs (http://... oder https://...) nicht ändern, wenn intelligente Schreibweise aktiv ist.
    $description:fr-FR: >-
      Ne pas modifier la casse des URLs (http://... ou https://...) lorsque la casse intelligente est activée.
    $description:es-ES: >-
      No cambiar las mayúsculas de las URLs (http://... o https://...) cuando las mayúsculas inteligentes están habilitadas.
    $description:pt-BR: >-
      Não alterar a capitalização de URLs (http://... ou https://...) quando a capitalização inteligente está ativada.
    $description:it-IT: >-
      Non modificare le maiuscole degli URL (http://... o https://...) quando le maiuscole intelligenti sono attive.
    $description:ru-RU: >-
      Не изменять регистр URL (http://... или https://...) при включённом умном регистре.
    $description:uk-UA: >-
      Не змінювати регістр URL (http://... або https://...) коли розумний регістр увімкнено.
    $description:ja-JP: >-
      スマートケースが有効な場合、URL（http://...またはhttps://...）のケースを変更しません。
    $description:ko-KR: >-
      스마트 대소문자가 활성화된 경우 URL(http://... 또는 https://...)의 대소문자를 변경하지 않습니다.
    $description:zh-CN: >-
      启用智能大小写时，不更改URL（http://...或https://...）的大小写。
    $description:zh-TW: >-
      啟用智慧大小寫時，不更改URL（http://...或https://...）的大小寫。
      عدم تغيير حالة أحرف عناوين URL (http://... أو https://...) عند تفعيل تغيير حالة الأحرف الذكي.
    $description:pl-PL: >-
      Nie zmieniaj wielkości liter w adresach URL (http://... lub https://...) gdy inteligentna wielkość liter jest włączona.
    $description:nl-NL: >-
      Wijzig het hoofdlettergebruik van URL's (http://... of https://...) niet wanneer slim hoofdlettergebruik is ingeschakeld.
  - PathEscaperMode: none
    $name: 💻 Path auto-escaper
    $name:tr-TR: 💻 Yol otomatik kaçış
    $name:de-DE: 💻 Pfad-Auto-Escaper
    $name:fr-FR: 💻 Échappement automatique des chemins
    $name:es-ES: 💻 Escapador automático de rutas
    $name:pt-BR: 💻 Escapador automático de caminhos
    $name:it-IT: 💻 Escape automatico percorsi
    $name:ru-RU: 💻 Автоэкранирование путей
    $name:uk-UA: 💻 Автоекранування шляхів
    $name:ja-JP: 💻 パス自動エスケープ
    $name:ko-KR: 💻 경로 자동 이스케이프
    $name:zh-CN: 💻 路径自动转义
    $name:zh-TW: 💻 路徑自動轉義
    $name:pl-PL: 💻 Automatyczne escapowanie ścieżek
    $name:nl-NL: 💻 Pad automatisch escapen
    $description: >-
      When a Windows file path is detected (e.g. C:\Users\file.txt),
      automatically escape the backslashes for use in code.
    $description:tr-TR: >-
      Bir Windows dosya yolu algılandığında (ör. C:\Users\file.txt), ters eğik çizgileri kod için otomatik olarak kaçırır.
    $description:de-DE: >-
      Wenn ein Windows-Dateipfad erkannt wird (z.B. C:\Users\file.txt), werden die Backslashes automatisch für Code escaped.
    $description:fr-FR: >-
      Lorsqu'un chemin Windows est détecté (ex. C:\Users\file.txt), les antislashs sont automatiquement échappés pour le code.
    $description:es-ES: >-
      Cuando se detecta una ruta de Windows (ej. C:\Users\file.txt), se escapan automáticamente las barras invertidas para código.
    $description:pt-BR: >-
      Quando um caminho do Windows é detectado (ex. C:\Users\file.txt), as barras invertidas são automaticamente escapadas para código.
    $description:it-IT: >-
      Quando viene rilevato un percorso Windows (es. C:\Users\file.txt), i backslash vengono automaticamente escapati per il codice.
    $description:ru-RU: >-
      При обнаружении пути Windows (например, C:\Users\file.txt) автоматически экранирует обратные косые черты для кода.
    $description:uk-UA: >-
      При виявленні шляху Windows (наприклад, C:\Users\file.txt) автоматично екранує зворотні косі риски для коду.
    $description:ja-JP: >-
      Windowsファイルパスが検出された場合（例：C:\Users\file.txt）、バックスラッシュをコード用に自動的にエスケープします。
    $description:ko-KR: >-
      Windows 파일 경로가 감지되면(예: C:\Users\file.txt) 코드에서 사용할 수 있도록 백슬래시를 자동으로 이스케이프합니다.
    $description:zh-CN: >-
      检测到Windows文件路径时（如C:\Users\file.txt），自动转义反斜杠以用于代码。
    $description:zh-TW: >-
      偵測到Windows檔案路徑時（如C:\Users\file.txt），自動轉義反斜線以用於程式碼。
      عند اكتشاف مسار ملف Windows (مثل C:\Users\file.txt)، يتم تحويل الخطوط المائلة العكسية تلقائيًا للاستخدام في الكود.
    $description:pl-PL: >-
      Po wykryciu ścieżki Windows (np. C:\Users\file.txt), automatycznie escapuje ukośniki odwrotne do użycia w kodzie.
    $description:nl-NL: >-
      Wanneer een Windows-bestandspad wordt gedetecteerd (bijv. C:\Users\file.txt), worden backslashes automatisch ge-escaped voor gebruik in code.
    $options:
    - none: None (no change)
    - doubleBackslash: Double backslash (C:\\Users\\file.txt)
    - forwardSlash: Forward slash (C:/Users/file.txt)
    $options:tr-TR:
    - none: Yok (değişiklik yok)
    - doubleBackslash: Çift ters eğik çizgi (C:\\Users\\file.txt)
    - forwardSlash: İleri eğik çizgi (C:/Users/file.txt)
    $options:de-DE:
    - none: Keine (keine Änderung)
    - doubleBackslash: Doppelter Backslash (C:\\Users\\file.txt)
    - forwardSlash: Schrägstrich (C:/Users/file.txt)
    $options:fr-FR:
    - none: Aucun (pas de changement)
    - doubleBackslash: Double antislash (C:\\Users\\file.txt)
    - forwardSlash: Barre oblique (C:/Users/file.txt)
    $options:es-ES:
    - none: Ninguno (sin cambio)
    - doubleBackslash: Doble barra invertida (C:\\Users\\file.txt)
    - forwardSlash: Barra diagonal (C:/Users/file.txt)
    $options:pt-BR:
    - none: Nenhum (sem alteração)
    - doubleBackslash: Barra invertida dupla (C:\\Users\\file.txt)
    - forwardSlash: Barra (C:/Users/file.txt)
    $options:it-IT:
    - none: Nessuno (nessuna modifica)
    - doubleBackslash: Doppio backslash (C:\\Users\\file.txt)
    - forwardSlash: Barra (C:/Users/file.txt)
    $options:ru-RU:
    - none: Нет (без изменений)
    - doubleBackslash: Двойной обратный слеш (C:\\Users\\file.txt)
    - forwardSlash: Прямой слеш (C:/Users/file.txt)
    $options:uk-UA:
    - none: Немає (без змін)
    - doubleBackslash: Подвійний зворотний слеш (C:\\Users\\file.txt)
    - forwardSlash: Прямий слеш (C:/Users/file.txt)
    $options:ja-JP:
    - none: なし（変更なし）
    - doubleBackslash: ダブルバックスラッシュ (C:\\Users\\file.txt)
    - forwardSlash: フォワードスラッシュ (C:/Users/file.txt)
    $options:ko-KR:
    - none: 없음 (변경 없음)
    - doubleBackslash: 이중 백슬래시 (C:\\Users\\file.txt)
    - forwardSlash: 슬래시 (C:/Users/file.txt)
    $options:zh-CN:
    - none: 无（不更改）
    - doubleBackslash: 双反斜杠 (C:\\Users\\file.txt)
    - forwardSlash: 正斜杠 (C:/Users/file.txt)
    $options:zh-TW:
    - none: 無（不更改）
    - doubleBackslash: 雙反斜線 (C:\\Users\\file.txt)
    - forwardSlash: 正斜線 (C:/Users/file.txt)
    - none: لا شيء (بدون تغيير)
    - doubleBackslash: خط مائل عكسي مزدوج (C:\\Users\\file.txt)
    - forwardSlash: خط مائل أمامي (C:/Users/file.txt)
    $options:pl-PL:
    - none: Brak (bez zmian)
    - doubleBackslash: Podwójny ukośnik odwrotny (C:\\Users\\file.txt)
    - forwardSlash: Ukośnik (C:/Users/file.txt)
    $options:nl-NL:
    - none: Geen (geen wijziging)
    - doubleBackslash: Dubbele backslash (C:\\Users\\file.txt)
    - forwardSlash: Schuine streep (C:/Users/file.txt)
  - MarkdownToHtml: false
    $name: 📝 Markdown to rich text
    $name:tr-TR: 📝 Markdown'dan zengin metne
    $name:de-DE: 📝 Markdown zu Rich Text
    $name:fr-FR: 📝 Markdown vers texte riche
    $name:es-ES: 📝 Markdown a texto enriquecido
    $name:pt-BR: 📝 Markdown para texto formatado
    $name:it-IT: 📝 Markdown a testo formattato
    $name:ru-RU: 📝 Markdown в форматированный текст
    $name:uk-UA: 📝 Markdown у форматований текст
    $name:ja-JP: 📝 Markdownからリッチテキスト
    $name:ko-KR: 📝 Markdown을 서식 텍스트로
    $name:zh-CN: 📝 Markdown转富文本
    $name:zh-TW: 📝 Markdown轉富文字
    $name:pl-PL: 📝 Markdown na tekst sformatowany
    $name:nl-NL: 📝 Markdown naar opgemaakte tekst
    $description: >-
      Automatically convert simple Markdown (like **bold** or [links](url))
      into actual Rich Text on the clipboard.
    $description:tr-TR: >-
      Basit Markdown'ı (**kalın** veya [bağlantılar](url) gibi) otomatik olarak panodaki gerçek Zengin Metne dönüştürür.
    $description:de-DE: >-
      Einfaches Markdown (wie **fett** oder [Links](url)) automatisch in Rich Text in der Zwischenablage konvertieren.
    $description:fr-FR: >-
      Convertir automatiquement le Markdown simple (comme **gras** ou [liens](url)) en texte riche dans le presse-papiers.
    $description:es-ES: >-
      Convertir automáticamente Markdown simple (como **negrita** o [enlaces](url)) en texto enriquecido en el portapapeles.
    $description:pt-BR: >-
      Converter automaticamente Markdown simples (como **negrito** ou [links](url)) em texto formatado na área de transferência.
    $description:it-IT: >-
      Converti automaticamente il Markdown semplice (come **grassetto** o [link](url)) in testo formattato negli appunti.
    $description:ru-RU: >-
      Автоматически преобразовывать простой Markdown (например **жирный** или [ссылки](url)) в форматированный текст в буфере обмена.
    $description:uk-UA: >-
      Автоматично перетворювати простий Markdown (наприклад **жирний** або [посилання](url)) у форматований текст у буфері обміну.
    $description:ja-JP: >-
      シンプルなMarkdown（**太字**や[リンク](url)など）をクリップボード上のリッチテキストに自動変換します。
    $description:ko-KR: >-
      간단한 Markdown(**굵게** 또는 [링크](url) 등)을 클립보드의 서식 텍스트로 자동 변환합니다.
    $description:zh-CN: >-
      自动将简单的Markdown（如**粗体**或[链接](url)）转换为剪贴板上的富文本。
    $description:zh-TW: >-
      自動將簡單的Markdown（如**粗體**或[連結](url)）轉換為剪貼簿上的富文字。
      تحويل Markdown البسيط (مثل **عريض** أو [روابط](url)) تلقائيًا إلى نص منسق في الحافظة.
    $description:pl-PL: >-
      Automatycznie konwertuj prosty Markdown (jak **pogrubienie** lub [linki](url)) na tekst sformatowany w schowku.
    $description:nl-NL: >-
      Converteer automatisch eenvoudige Markdown (zoals **vet** of [links](url)) naar opgemaakte tekst op het klembord.
  $name: 🧹 Text Cleanup & Formatting
  $name:tr-TR: 🧹 Metin Temizleme & Biçimlendirme
  $name:de-DE: 🧹 Textbereinigung & Formatierung
  $name:fr-FR: 🧹 Nettoyage & Formatage de Texte
  $name:es-ES: 🧹 Limpieza & Formato de Texto
  $name:pt-BR: 🧹 Limpeza & Formatação de Texto
  $name:it-IT: 🧹 Pulizia & Formattazione Testo
  $name:ru-RU: 🧹 Очистка & Форматирование Текста
  $name:uk-UA: 🧹 Очищення & Форматування Тексту
  $name:ja-JP: 🧹 テキストクリーンアップ & フォーマット
  $name:ko-KR: 🧹 텍스트 정리 & 서식
  $name:zh-CN: 🧹 文本清理与格式化
  $name:zh-TW: 🧹 文字清理與格式化
  $name:pl-PL: 🧹 Czyszczenie & Formatowanie Tekstu
  $name:nl-NL: 🧹 Tekst Opschonen & Opmaken

- DataExtraction:
  - DataExtractorMode: none
    $name: 📥 Data extractor
    $name:tr-TR: 📥 Veri çıkarıcı
    $name:de-DE: 📥 Daten-Extraktor
    $name:fr-FR: 📥 Extracteur de données
    $name:es-ES: 📥 Extractor de datos
    $name:pt-BR: 📥 Extrator de dados
    $name:it-IT: 📥 Estrattore dati
    $name:ru-RU: 📥 Извлечение данных
    $name:uk-UA: 📥 Витяг даних
    $name:ja-JP: 📥 データ抽出
    $name:ko-KR: 📥 데이터 추출기
    $name:zh-CN: 📥 数据提取器
    $name:zh-TW: 📥 資料擷取器
    $name:pl-PL: 📥 Ekstraktor danych
    $name:nl-NL: 📥 Data-extractor
    $description: >-
      Instead of copying the full text, extract only the URLs
      or email addresses found within it.
    $description:tr-TR: >-
      Tam metni kopyalamak yerine, yalnızca içindeki URL'leri veya e-posta adreslerini çıkarır.
    $description:de-DE: >-
      Statt den gesamten Text zu kopieren, nur die darin enthaltenen URLs oder E-Mail-Adressen extrahieren.
    $description:fr-FR: >-
      Au lieu de copier le texte complet, extraire uniquement les URLs ou adresses e-mail qu'il contient.
    $description:es-ES: >-
      En lugar de copiar el texto completo, extraer solo las URLs o direcciones de correo electrónico encontradas.
    $description:pt-BR: >-
      Em vez de copiar o texto completo, extrair apenas as URLs ou endereços de e-mail encontrados.
    $description:it-IT: >-
      Invece di copiare l'intero testo, estrai solo gli URL o gli indirizzi e-mail trovati al suo interno.
    $description:ru-RU: >-
      Вместо копирования всего текста извлекать только URL или адреса электронной почты.
    $description:uk-UA: >-
      Замість копіювання всього тексту витягувати лише URL або адреси електронної пошти.
    $description:ja-JP: >-
      テキスト全体をコピーする代わりに、含まれるURLまたはメールアドレスのみを抽出します。
    $description:ko-KR: >-
      전체 텍스트를 복사하는 대신 포함된 URL 또는 이메일 주소만 추출합니다.
    $description:zh-CN: >-
      不复制完整文本，仅提取其中的URL或电子邮件地址。
    $description:zh-TW: >-
      不複製完整文字，僅擷取其中的URL或電子郵件地址。
      بدلاً من نسخ النص الكامل، استخراج عناوين URL أو عناوين البريد الإلكتروني الموجودة فيه فقط.
    $description:pl-PL: >-
      Zamiast kopiować cały tekst, wyodrębnij tylko adresy URL lub adresy e-mail znalezione w tekście.
    $description:nl-NL: >-
      In plaats van de volledige tekst te kopiëren, alleen de URL's of e-mailadressen erin extraheren.
    $options:
    - none: None (copy full text)
    - urls: Extract URLs only
    - emails: Extract email addresses only
    $options:tr-TR:
    - none: Yok (tam metni kopyala)
    - urls: Yalnızca URL'leri çıkar
    - emails: Yalnızca e-posta adreslerini çıkar
    $options:de-DE:
    - none: Keine (vollständigen Text kopieren)
    - urls: Nur URLs extrahieren
    - emails: Nur E-Mail-Adressen extrahieren
    $options:fr-FR:
    - none: Aucun (copier le texte complet)
    - urls: Extraire les URLs uniquement
    - emails: Extraire les adresses e-mail uniquement
    $options:es-ES:
    - none: Ninguno (copiar texto completo)
    - urls: Extraer solo URLs
    - emails: Extraer solo correos electrónicos
    $options:pt-BR:
    - none: Nenhum (copiar texto completo)
    - urls: Extrair apenas URLs
    - emails: Extrair apenas endereços de e-mail
    $options:it-IT:
    - none: Nessuno (copia testo completo)
    - urls: Estrai solo URL
    - emails: Estrai solo indirizzi e-mail
    $options:ru-RU:
    - none: Нет (копировать весь текст)
    - urls: Извлечь только URL
    - emails: Извлечь только адреса электронной почты
    $options:uk-UA:
    - none: Немає (копіювати весь текст)
    - urls: Витягти лише URL
    - emails: Витягти лише адреси електронної пошти
    $options:ja-JP:
    - none: なし（全文をコピー）
    - urls: URLのみ抽出
    - emails: メールアドレスのみ抽出
    $options:ko-KR:
    - none: 없음 (전체 텍스트 복사)
    - urls: URL만 추출
    - emails: 이메일 주소만 추출
    $options:zh-CN:
    - none: 无（复制完整文本）
    - urls: 仅提取URL
    - emails: 仅提取电子邮件地址
    $options:zh-TW:
    - none: 無（複製完整文字）
    - urls: 僅擷取URL
    - emails: 僅擷取電子郵件地址
    - none: لا شيء (نسخ النص الكامل)
    - urls: استخراج عناوين URL فقط
    - emails: استخراج عناوين البريد الإلكتروني فقط
    $options:pl-PL:
    - none: Brak (kopiuj cały tekst)
    - urls: Wyodrębnij tylko adresy URL
    - emails: Wyodrębnij tylko adresy e-mail
    $options:nl-NL:
    - none: Geen (volledige tekst kopiëren)
    - urls: Alleen URL's extraheren
    - emails: Alleen e-mailadressen extraheren
  $name: 🛡️ Data Extraction
  $name:tr-TR: 🛡️ Veri Çıkarma
  $name:de-DE: 🛡️ Datenextraktion
  $name:fr-FR: 🛡️ Extraction de Données
  $name:es-ES: 🛡️ Extracción de Datos
  $name:pt-BR: 🛡️ Extração de Dados
  $name:it-IT: 🛡️ Estrazione Dati
  $name:ru-RU: 🛡️ Извлечение Данных
  $name:uk-UA: 🛡️ Витяг Даних
  $name:ja-JP: 🛡️ データ抽出
  $name:ko-KR: 🛡️ 데이터 추출
  $name:zh-CN: 🛡️ 数据提取
  $name:zh-TW: 🛡️ 資料擷取
  $name:pl-PL: 🛡️ Ekstrakcja Danych
  $name:nl-NL: 🛡️ Data-extractie

- AdvancedConversions:
  - RegexReplacements:
    - - Search: ""
        $name: Search Regex/String
        $name:tr-TR: Arama Regex/Metin
        $name:de-DE: Such-Regex/Zeichenkette
        $name:fr-FR: Recherche Regex/Texte
        $name:es-ES: Buscar Regex/Texto
        $name:pt-BR: Buscar Regex/Texto
        $name:it-IT: Cerca Regex/Testo
        $name:ru-RU: Поиск Regex/Строка
        $name:uk-UA: Пошук Regex/Рядок
        $name:ja-JP: 検索 正規表現/文字列
        $name:ko-KR: 검색 정규식/문자열
        $name:zh-CN: 搜索 正则/字符串
        $name:zh-TW: 搜尋 正則/字串
        $name:pl-PL: Szukaj Regex/Tekst
        $name:nl-NL: Zoek Regex/Tekst
      - Replace: ""
        $name: Replace String
        $name:tr-TR: Değiştirme Metni
        $name:de-DE: Ersetzungstext
        $name:fr-FR: Texte de remplacement
        $name:es-ES: Texto de reemplazo
        $name:pt-BR: Texto de substituição
        $name:it-IT: Testo sostitutivo
        $name:ru-RU: Строка замены
        $name:uk-UA: Рядок заміни
        $name:ja-JP: 置換文字列
        $name:ko-KR: 대체 문자열
        $name:zh-CN: 替换字符串
        $name:zh-TW: 替換字串
        $name:pl-PL: Tekst zamienny
        $name:nl-NL: Vervangtekst
    $name: ⚡ Regex text replacements
    $name:tr-TR: ⚡ Regex metin değiştirmeleri
    $name:de-DE: ⚡ Regex-Textersetzungen
    $name:fr-FR: ⚡ Remplacements de texte par Regex
    $name:es-ES: ⚡ Reemplazos de texto con Regex
    $name:pt-BR: ⚡ Substituições de texto com Regex
    $name:it-IT: ⚡ Sostituzioni testo con Regex
    $name:ru-RU: ⚡ Замены текста по Regex
    $name:uk-UA: ⚡ Заміни тексту за Regex
    $name:ja-JP: ⚡ 正規表現テキスト置換
    $name:ko-KR: ⚡ 정규식 텍스트 치환
    $name:zh-CN: ⚡ 正则文本替换
    $name:zh-TW: ⚡ 正則文字替換
    $name:pl-PL: ⚡ Zamiana tekstu wyrażeniami regularnymi
    $name:nl-NL: ⚡ Regex-tekstvervangingen
    $description: >-
      Define custom find-and-replace rules using regular expressions.
      These are applied to all copied text.
    $description:tr-TR: >-
      Düzenli ifadeler kullanarak özel bul-ve-değiştir kuralları tanımlayın. Bunlar tüm kopyalanan metne uygulanır.
    $description:de-DE: >-
      Benutzerdefinierte Such- und Ersetzungsregeln mit regulären Ausdrücken definieren. Diese werden auf allen kopierten Text angewendet.
    $description:fr-FR: >-
      Définir des règles personnalisées de recherche et remplacement avec des expressions régulières. Elles sont appliquées à tout texte copié.
    $description:es-ES: >-
      Definir reglas personalizadas de buscar y reemplazar usando expresiones regulares. Se aplican a todo el texto copiado.
    $description:pt-BR: >-
      Definir regras personalizadas de buscar e substituir usando expressões regulares. São aplicadas a todo o texto copiado.
    $description:it-IT: >-
      Definisci regole personalizzate di trova e sostituisci usando espressioni regolari. Vengono applicate a tutto il testo copiato.
    $description:ru-RU: >-
      Определите пользовательские правила поиска и замены с помощью регулярных выражений. Они применяются ко всему скопированному тексту.
    $description:uk-UA: >-
      Визначте власні правила пошуку та заміни за допомогою регулярних виразів. Вони застосовуються до всього скопійованого тексту.
    $description:ja-JP: >-
      正規表現を使用してカスタムの検索と置換ルールを定義します。すべてのコピーされたテキストに適用されます。
    $description:ko-KR: >-
      정규식을 사용하여 사용자 정의 찾기 및 바꾸기 규칙을 정의합니다. 모든 복사된 텍스트에 적용됩니다.
    $description:zh-CN: >-
      使用正则表达式定义自定义查找和替换规则。这些规则应用于所有复制的文本。
    $description:zh-TW: >-
      使用正則表達式定義自訂尋找和取代規則。這些規則套用於所有複製的文字。
      تحديد قواعد بحث واستبدال مخصصة باستخدام التعبيرات النمطية. يتم تطبيقها على جميع النصوص المنسوخة.
    $description:pl-PL: >-
      Zdefiniuj własne reguły wyszukiwania i zamiany za pomocą wyrażeń regularnych. Są stosowane do całego skopiowanego tekstu.
    $description:nl-NL: >-
      Definieer aangepaste zoek-en-vervangregels met reguliere expressies. Deze worden toegepast op alle gekopieerde tekst.
  $name: ⚡ Advanced Conversions
  $name:tr-TR: ⚡ Gelişmiş Dönüşümler
  $name:de-DE: ⚡ Erweiterte Konvertierungen
  $name:fr-FR: ⚡ Conversions Avancées
  $name:es-ES: ⚡ Conversiones Avanzadas
  $name:pt-BR: ⚡ Conversões Avançadas
  $name:it-IT: ⚡ Conversioni Avanzate
  $name:ru-RU: ⚡ Расширенные Преобразования
  $name:uk-UA: ⚡ Розширені Перетворення
  $name:ja-JP: ⚡ 高度な変換
  $name:ko-KR: ⚡ 고급 변환
  $name:zh-CN: ⚡ 高级转换
  $name:zh-TW: ⚡ 進階轉換
  $name:pl-PL: ⚡ Zaawansowane Konwersje
  $name:nl-NL: ⚡ Geavanceerde Conversies
*/
// ==/WindhawkModSettings==

#include <cwchar>
#include <cwctype>
#include <regex>
#include <string>
#include <vector>
#include <windows.h>

struct RegexReplacementItem {
  std::wregex searchRegex;
  std::wstring replaceW;
};

std::vector<RegexReplacementItem> g_regexReplacements;
bool g_removeTrackingParams = false;
bool g_autoTrimWhitespace = false;
bool g_unwrapText = false;
int g_casingMode = 0;
bool g_smartCasingExcludeUrls = true;
int g_pathEscaperMode = 0;
int g_dataExtractorMode = 0;
bool g_markdownToHtml = true;
bool g_forcePlainText = false;
int g_triggerModifierKey = 0;

// -------------------------------------------------------------------------
// Settings Loader
// -------------------------------------------------------------------------

void LoadSettings() {
  g_regexReplacements.clear();

  g_removeTrackingParams = Wh_GetIntSetting(L"DataExtraction.RemoveTrackingParams");
  g_autoTrimWhitespace = Wh_GetIntSetting(L"CleanupAndFormatting.AutoTrimWhitespace");
  g_unwrapText = Wh_GetIntSetting(L"CleanupAndFormatting.UnwrapText");
  g_markdownToHtml = Wh_GetIntSetting(L"CleanupAndFormatting.MarkdownToHtml");
  g_forcePlainText = Wh_GetIntSetting(L"CleanupAndFormatting.ForcePlainText");

  PCWSTR triggerKey = Wh_GetStringSetting(L"Core.TriggerModifierKey");
  g_triggerModifierKey = 0;
  if (triggerKey) {
    if (wcscmp(triggerKey, L"shift") == 0)
      g_triggerModifierKey = 1;
    else if (wcscmp(triggerKey, L"alt") == 0)
      g_triggerModifierKey = 2;
    Wh_FreeStringSetting(triggerKey);
  }

  PCWSTR casingMode = Wh_GetStringSetting(L"CleanupAndFormatting.CasingMode");
  g_casingMode = 0;
  if (wcscmp(casingMode, L"lowercase") == 0)
    g_casingMode = 1;
  else if (wcscmp(casingMode, L"uppercase") == 0)
    g_casingMode = 2;
  else if (wcscmp(casingMode, L"titlecase") == 0)
    g_casingMode = 3;
  Wh_FreeStringSetting(casingMode);

  g_smartCasingExcludeUrls = Wh_GetIntSetting(L"CleanupAndFormatting.SmartCasingExcludeUrls");

  PCWSTR pathMode = Wh_GetStringSetting(L"CleanupAndFormatting.PathEscaperMode");
  g_pathEscaperMode = 0;
  if (wcscmp(pathMode, L"doubleBackslash") == 0)
    g_pathEscaperMode = 1;
  else if (wcscmp(pathMode, L"forwardSlash") == 0)
    g_pathEscaperMode = 2;
  Wh_FreeStringSetting(pathMode);

  PCWSTR extractorMode = Wh_GetStringSetting(L"DataExtraction.DataExtractorMode");
  g_dataExtractorMode = 0;
  if (wcscmp(extractorMode, L"urls") == 0)
    g_dataExtractorMode = 1;
  else if (wcscmp(extractorMode, L"emails") == 0)
    g_dataExtractorMode = 2;
  Wh_FreeStringSetting(extractorMode);

  for (int i = 0;; i++) {
    PCWSTR search = Wh_GetStringSetting(L"AdvancedConversions.RegexReplacements[%d].Search", i);
    bool hasSearch = *search;

    if (hasSearch) {
      PCWSTR replace = Wh_GetStringSetting(L"AdvancedConversions.RegexReplacements[%d].Replace", i);

      try {
        g_regexReplacements.push_back(
            {std::wregex(search), std::wstring(replace)});
      } catch (const std::regex_error &) {
        Wh_Log(L"Invalid regex provided in settings: %s", search);
      }
      Wh_FreeStringSetting(replace);
    }

    Wh_FreeStringSetting(search);

    if (!hasSearch) {
      break;
    }
  }
}

// -------------------------------------------------------------------------
// Text Transformations
// -------------------------------------------------------------------------

std::wstring ApplyRegexReplacements(std::wstring text) {
  for (const auto &item : g_regexReplacements) {
    text = std::regex_replace(text, item.searchRegex, item.replaceW);
  }
  return text;
}

std::wstring RemoveUrlTrackingParams(std::wstring text) {
  if (!g_removeTrackingParams)
    return text;

  if (text.find(L"http") == std::wstring::npos ||
      text.find(L"?") == std::wstring::npos) {
    return text;
  }

  static const std::wregex trackingRegex(L"([?&])(utm_[^&=]+|fbclid|gclid|igshid|mc_cid|mc_"
                            L"eid|msclkid)=[^&#\\r\\n]*(&?)",
                            std::regex_constants::icase);
  static const std::wregex ampersandMerge(L"&&+");
  static const std::wregex questionAmpersand(L"\\?&");
  static const std::wregex dangling(L"[?&](?=\\s|$)");

  std::wstring prevText;
  do {
    prevText = text;
    text = std::regex_replace(text, trackingRegex, L"$1$3");
  } while (text != prevText);

  text = std::regex_replace(text, ampersandMerge, L"&");
  text = std::regex_replace(text, questionAmpersand, L"?");
  text = std::regex_replace(text, dangling, L"");

  return text;
}

std::wstring ExtractData(const std::wstring &text) {
  if (g_dataExtractorMode == 0)
    return text;

  static const std::wregex urlPattern(L"https?://[^\\s]+", std::regex_constants::icase);
  static const std::wregex emailPattern(L"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}", std::regex_constants::icase);

  const std::wregex* pattern = nullptr;
  if (g_dataExtractorMode == 1) {
    pattern = &urlPattern;
  } else if (g_dataExtractorMode == 2) {
    pattern = &emailPattern;
  } else {
    return text;
  }

  std::wstring result;
  auto words_begin = std::wsregex_iterator(text.begin(), text.end(), *pattern);
  auto words_end = std::wsregex_iterator();

  for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
    std::wsmatch match = *i;
    result += match.str() + L"\r\n";
  }

  if (!result.empty()) {
    result.pop_back();
    result.pop_back();
  }

  return result.empty() ? text : result;
}

std::wstring TrimWhitespace(std::wstring text) {
  if (!g_autoTrimWhitespace)
    return text;

  auto start = text.find_first_not_of(L" \t\r\n");
  if (start == std::wstring::npos)
    return L"";

  auto end = text.find_last_not_of(L" \t\r\n");
  return text.substr(start, end - start + 1);
}

std::wstring UnwrapText(std::wstring text) {
  if (!g_unwrapText)
    return text;

  static const std::wregex winNewline(L"\r\n");
  static const std::wregex doubleNewline(L"\n\n");
  static const std::wregex singleNewline(L"\n");
  static const std::wregex placeholder(L"\x01\x01");

  text = std::regex_replace(text, winNewline, L"\n");

  // Preserve paragraph breaks (double newlines) using a placeholder
  text = std::regex_replace(text, doubleNewline, L"\x01\x01");
  text = std::regex_replace(text, singleNewline, L" ");
  text = std::regex_replace(text, placeholder, L"\r\n\r\n");

  return text;
}

std::wstring ApplyCasing(std::wstring text) {
  if (g_casingMode == 0)
    return text;

  std::vector<std::pair<size_t, size_t>> urlRanges;
  if (g_smartCasingExcludeUrls) {
    static const std::wregex urlPattern(L"https?://[^\\s]+", std::regex_constants::icase);
    auto words_begin = std::wsregex_iterator(text.begin(), text.end(), urlPattern);
    auto words_end = std::wsregex_iterator();
    for (std::wsregex_iterator i = words_begin; i != words_end; ++i) {
      std::wsmatch match = *i;
      urlRanges.push_back({match.position(), match.length()});
    }
  }

  size_t urlIdx = 0;
  auto is_in_url = [&](size_t pos) {
    if (!g_smartCasingExcludeUrls) return false;
    while (urlIdx < urlRanges.size() && pos >= urlRanges[urlIdx].first + urlRanges[urlIdx].second) {
      urlIdx++;
    }
    return (urlIdx < urlRanges.size() && pos >= urlRanges[urlIdx].first);
  };

  if (g_casingMode == 1) { // Lowercase
    for (size_t i = 0; i < text.length(); ++i) {
      if (!is_in_url(i))
        text[i] = std::towlower(text[i]);
    }
  } else if (g_casingMode == 2) { // UPPERCASE
    for (size_t i = 0; i < text.length(); ++i) {
      if (!is_in_url(i))
        text[i] = std::towupper(text[i]);
    }
  } else if (g_casingMode == 3) { // Title Case
    bool newWord = true;
    for (size_t i = 0; i < text.length(); ++i) {
      if (is_in_url(i)) {
        newWord = false;
        continue;
      }
      wchar_t &c = text[i];
      if (std::iswspace(c)) {
        newWord = true;
      } else if (newWord) {
        c = std::towupper(c);
        newWord = false;
      } else {
        c = std::towlower(c);
      }
    }
  }
  return text;
}

std::wstring ApplyPathEscaper(std::wstring text) {
  if (g_pathEscaperMode == 0)
    return text;

  if (text.find(L":\\") != std::wstring::npos || text.find(L"\\\\") == 0) {
    static const std::wregex doubleSlash(L"\\\\+");
    static const std::wregex singleSlash(L"\\\\");

    if (g_pathEscaperMode == 1) {
      text = std::regex_replace(text, doubleSlash, L"\\");
      text = std::regex_replace(text, singleSlash, L"\\\\");
    } else if (g_pathEscaperMode == 2) {
      text = std::regex_replace(text, doubleSlash, L"/");
    }
  }
  return text;
}

std::wstring CleanCopiedText(const std::wstring &originalText) {
  std::wstring text = originalText;

  text = ExtractData(text);
  text = RemoveUrlTrackingParams(text);
  text = ApplyRegexReplacements(text);
  text = UnwrapText(text);
  text = ApplyCasing(text);
  text = ApplyPathEscaper(text);
  text = TrimWhitespace(text);

  return text;
}

// -------------------------------------------------------------------------
// Markdown to HTML Format Generation
// -------------------------------------------------------------------------

std::string ConvertMarkdownToHtml(const std::wstring &text) {
  std::wstring htmlW = text;

  static const std::wregex amp(L"&");
  static const std::wregex lt(L"<");
  static const std::wregex gt(L">");
  static const std::wregex newline(L"\\r\\n|\\r|\\n");
  static const std::wregex bold1(L"\\*\\*(.*?)\\*\\*");
  static const std::wregex bold2(L"__(.*?)__");
  static const std::wregex italic1(L"\\*([^\\*]+)\\*");
  static const std::wregex italic2(L"_([^_]+)_");
  static const std::wregex link(L"\\[(.*?)\\]\\((.*?)\\)");

  htmlW = std::regex_replace(htmlW, amp, L"&amp;");
  htmlW = std::regex_replace(htmlW, lt, L"&lt;");
  htmlW = std::regex_replace(htmlW, gt, L"&gt;");

  htmlW = std::regex_replace(htmlW, newline, L"<br>\n");
  htmlW = std::regex_replace(htmlW, bold1, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, bold2, L"<strong>$1</strong>");
  htmlW = std::regex_replace(htmlW, italic1, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, italic2, L"<em>$1</em>");
  htmlW = std::regex_replace(htmlW, link, L"<a href=\"$2\">$1</a>");

  int u8Len =
      WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, NULL, 0, NULL, NULL);
  std::string htmlU8(u8Len, 0);
  WideCharToMultiByte(CP_UTF8, 0, htmlW.c_str(), -1, &htmlU8[0], u8Len, NULL,
                      NULL);

  if (!htmlU8.empty() && htmlU8.back() == '\0') {
    htmlU8.pop_back();
  }

  return htmlU8;
}

std::string GenerateClipboardHtmlPayload(const std::string &htmlBodyFragment) {
  const char *headerFormat = "Version:0.9\r\n"
                             "StartHTML:%010u\r\n"
                             "EndHTML:%010u\r\n"
                             "StartFragment:%010u\r\n"
                             "EndFragment:%010u\r\n";

  const char *htmlPrefix = "<html>\r\n"
                           "<body>\r\n"
                           "<!--StartFragment-->";

  const char *htmlSuffix = "<!--EndFragment-->\r\n"
                           "</body>\r\n"
                           "</html>";

  std::string htmlPrefixStr = htmlPrefix;
  std::string htmlSuffixStr = htmlSuffix;

  size_t headerLength = 105;

  size_t startHtml = headerLength;
  size_t startFragment = startHtml + htmlPrefixStr.length();
  size_t endFragment = startFragment + htmlBodyFragment.length();
  size_t endHtml = endFragment + htmlSuffixStr.length();

  char headerBuffer[128];
  snprintf(headerBuffer, sizeof(headerBuffer), headerFormat,
           (unsigned int)startHtml, (unsigned int)endHtml,
           (unsigned int)startFragment, (unsigned int)endFragment);

  return std::string(headerBuffer) + htmlPrefixStr + htmlBodyFragment +
         htmlSuffixStr;
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

thread_local DWORD t_lastClipboardSeq = 0;
thread_local bool t_modifiedCurrentSeq = false;
thread_local bool t_isCopyingOurFormats = false;
thread_local bool t_shouldFormatThisSeq = true;

using SetClipboardData_t = decltype(&SetClipboardData);
SetClipboardData_t pOriginalSetClipboardData;
HANDLE WINAPI SetClipboardDataHook(UINT uFormat, HANDLE hMem) {

  if (t_isCopyingOurFormats) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  DWORD seq = GetClipboardSequenceNumber();
  if (seq != t_lastClipboardSeq) {
    t_lastClipboardSeq = seq;
    t_modifiedCurrentSeq = false;
    
    t_shouldFormatThisSeq = true;
    if (g_triggerModifierKey != 0) {
      t_shouldFormatThisSeq = false; // Require key if set
      if (g_triggerModifierKey == 1 && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) t_shouldFormatThisSeq = true;
      else if (g_triggerModifierKey == 2 && (GetAsyncKeyState(VK_MENU) & 0x8000)) t_shouldFormatThisSeq = true;
    }
  }

  if (!t_shouldFormatThisSeq) {
    return pOriginalSetClipboardData(uFormat, hMem);
  }

  static UINT cfHtml = RegisterClipboardFormatW(L"HTML Format");
  static UINT cfRtf = RegisterClipboardFormatW(L"Rich Text Format");

  if (g_forcePlainText || t_modifiedCurrentSeq) {
    if (uFormat != CF_UNICODETEXT && uFormat != CF_TEXT && uFormat != CF_OEMTEXT && uFormat != CF_LOCALE) {
      if (hMem) {
        static UINT cfDropped = RegisterClipboardFormatW(L"Windhawk_Dropped");
        return pOriginalSetClipboardData(cfDropped, hMem);
      }
      return NULL;
    }
  }

  if (uFormat == CF_UNICODETEXT && hMem != NULL) {
    SIZE_T size = GlobalSize(hMem);
    SIZE_T maxChars = size / sizeof(WCHAR);
    if (maxChars > 0 && maxChars < 5 * 1024 * 1024) { // 10MB safety limit
      LPCWSTR pData = (LPCWSTR)GlobalLock(hMem);
      if (pData) {
        SIZE_T actualLen = 0;
        while (actualLen < maxChars && pData[actualLen] != L'\0') {
          actualLen++;
        }
        std::wstring originalText(pData, actualLen);
        GlobalUnlock(hMem);

        std::wstring cleanedText = CleanCopiedText(originalText);

        if (cleanedText != originalText || g_forcePlainText) {
          t_modifiedCurrentSeq = true;
          SIZE_T allocSize = (cleanedText.length() + 1) * sizeof(WCHAR);
          HANDLE hNewMem = GlobalAlloc(GMEM_MOVEABLE, allocSize);
          if (hNewMem) {
            LPWSTR pNewData = (LPWSTR)GlobalLock(hNewMem);
            if (pNewData) {
              memcpy(pNewData, cleanedText.c_str(), allocSize);
              GlobalUnlock(hNewMem);

              HANDLE hRet = pOriginalSetClipboardData(uFormat, hNewMem);
              if (hRet) {
                GlobalFree(hMem);
                
                if (!g_forcePlainText && g_markdownToHtml && cfHtml) {
                    t_isCopyingOurFormats = true;

                    std::string htmlPayload = GenerateClipboardHtmlPayload(ConvertMarkdownToHtml(cleanedText));
                    HANDLE hHtmlMem = GlobalAlloc(GMEM_MOVEABLE, htmlPayload.length() + 1);
                    if (hHtmlMem) {
                        LPSTR pHtmlData = (LPSTR)GlobalLock(hHtmlMem);
                        if (pHtmlData) {
                            memcpy(pHtmlData, htmlPayload.c_str(), htmlPayload.length() + 1);
                            GlobalUnlock(hHtmlMem);
                            pOriginalSetClipboardData(cfHtml, hHtmlMem);
                        } else {
                            GlobalFree(hHtmlMem);
                        }
                    }

                    if (cfRtf) {
                        HANDLE hRtfMem = GlobalAlloc(GMEM_MOVEABLE, 1);
                        if (hRtfMem) {
                            LPSTR pRtfData = (LPSTR)GlobalLock(hRtfMem);
                            if (pRtfData) {
                                pRtfData[0] = '\0';
                                GlobalUnlock(hRtfMem);
                                pOriginalSetClipboardData(cfRtf, hRtfMem);
                            } else {
                                GlobalFree(hRtfMem);
                            }
                        }
                    }

                    t_isCopyingOurFormats = false;
                }

                return hRet;
              } else {
                GlobalFree(hNewMem);
                return NULL;
              }
            } else {
              GlobalFree(hNewMem);
            }
          }
        }
      }
    }
  }

  return pOriginalSetClipboardData(uFormat, hMem);
}

// -------------------------------------------------------------------------
// Init
// -------------------------------------------------------------------------

BOOL Wh_ModInit(void) {
  Wh_Log(L"Init");
  LoadSettings();

  Wh_SetFunctionHook((void *)SetClipboardData, (void *)SetClipboardDataHook,
                     (void **)&pOriginalSetClipboardData);

  return TRUE;
}

void Wh_ModUninit(void) { Wh_Log(L"Uninit"); }

void Wh_ModSettingsChanged(void) {
  Wh_Log(L"SettingsChanged");
  LoadSettings();
}
