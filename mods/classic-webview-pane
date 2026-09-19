// ==WindhawkMod==
// @id              classic-webview-pane
// @name            Classic WebView pane in Explorer
// @description     Recreates the Windows 2000 folder WebView - the pane left of the file list with the icon and name of the folder or of the selected item, a divider and See also links
// @name:ru         Панель WebView как в Windows 2000
// @description:ru  Воссоздаёт панель WebView из Windows 2000 - панель слева от списка файлов со значком и именем папки или выбранного объекта, разделителем и ссылками «Перейти к»
// @version         2.7
// @author          appEW
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -lmsimg32 -lole32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- paneWidth: 200
  $name: Pane width
  $name:ru: Ширина панели
  $description: Width of the pane in pixels, at 100% scaling. Windows 2000 used about 200.
  $description:ru: >-
    Ширина панели в пикселях при масштабе 100%. В Windows 2000 она была около 200.
- position: left
  $name: Position
  $name:ru: Расположение
  $description: Which side of the file list the pane is put on.
  $description:ru: С какой стороны от списка файлов размещается панель.
  $options:
  - left: Left of the file list
  - right: Right of the file list
- showHeader: true
  $name: Icon and name at the top
  $name:ru: Значок и имя сверху
  $description: >-
    Show the icon and the name of the selected item at the top of the pane, or
    of the folder itself when nothing is selected.
  $description:ru: >-
    Показывать сверху значок и имя выбранного объекта, а когда ничего не
    выделено - самой папки.
- iconSize: 32
  $name: Icon size
  $name:ru: Размер значка
  $description: Size of the icon in the header, in pixels at 100% scaling.
  $description:ru: Размер значка в шапке, в пикселях при масштабе 100%.
- showItemType: true
  $name: Type and size under the name
  $name:ru: Тип и размер под именем
  $description: >-
    Add the kind of the item, and its size when it has one, under its name -
    the line Windows 2000 put there for a selected file.
  $description:ru: >-
    Добавлять под именем тип объекта и, если он есть, размер - строку, которую
    Windows 2000 показывала для выбранного файла.
- showDriveSpace: true
  $name: Drive space chart
  $name:ru: Диаграмма занятого места
  $description: >-
    For a drive, show the used/free legend, capacity and a three-dimensional
    pie chart under the name, the way the Windows 2000 WebView did.
  $description:ru: >-
    Показывать для диска ёмкость, легенду «Занято/Свободно» и объёмную круговую
    диаграмму под именем, как это делала панель WebView в Windows 2000.
- usedLabel: 'Занято:'
  $name: Caption of the bar
  $name:ru: Подпись полосы
- freeLabel: 'Свободно:'
  $name: Caption of the free size
  $name:ru: Подпись свободного места
- totalLabel: 'Емкость:'
  $name: Caption of the capacity
  $name:ru: Подпись ёмкости
- barColorSource: theme
  $name: Bar colour source
  $name:ru: Источник цвета индикатора
  $description: >-
    Use the selection colour supplied by the current Windows theme, or the
    custom colour below.
  $description:ru: >-
    Использовать цвет выделения, установленный текущей темой Windows, либо
    заданный ниже собственный цвет.
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorBar: '#000080'
  $name: Custom colour of the chart
  $name:ru: Собственный цвет диаграммы
  $description: Used only when "Custom colour" is selected above.
  $description:ru: Используется только при выборе «Собственный цвет» выше.
- descriptionText: Выберите объект для просмотра его описания.
  $name: Text under the divider
  $name:ru: Текст под разделителем
  $description: >-
    The line Windows 2000 showed while nothing was selected. The default is the
    wording of the Russian Windows 2000; in English it read "Select an item to
    view its description." Leave empty to omit it.
  $description:ru: >-
    Строка, которую Windows 2000 показывала, пока ничего не выделено. Оставьте
    пустым, чтобы убрать её.
- showSeeAlso: true
  $name: See also links
  $name:ru: Ссылки «Перейти к»
  $description: Show the list of links at the bottom of the pane.
  $description:ru: Показывать список ссылок внизу панели.
- seeAlsoTitle: 'Перейти к:'
  $name: See also caption
  $name:ru: Заголовок списка ссылок
  $description: The caption over the links. In English Windows 2000 it read "See also:".
  $description:ru: Заголовок над ссылками. В английской Windows 2000 - «See also:».
- seeAlso:
  - - label: Мои документы
      $name: Text
      $description: The text of the link.
      $description:ru: Текст ссылки.
    - target: shell:Personal
      $name: Target
      $description: >-
        Where the link goes. Anything Explorer can navigate to - a path, a
        shell: folder name or a shell:::{CLSID}.
      $description:ru: >-
        Куда ведёт ссылка. Всё, куда умеет переходить проводник - путь, имя
        папки вида shell: или shell:::{CLSID}.
  - - label: Сетевое окружение
    - target: shell:NetworkPlacesFolder
  - - label: Мой компьютер
    - target: shell:MyComputerFolder
  $name: Links
  $name:ru: Ссылки
  $description: The links under the See also caption.
  $description:ru: Ссылки под заголовком «Перейти к».
- underlineLinks: true
  $name: Underline the links
  $name:ru: Подчёркивать ссылки
  $description: >-
    Keep the links underlined the way the web view of Windows 2000 did, rather
    than only under the pointer.
  $description:ru: >-
    Держать ссылки подчёркнутыми, как в веб-виде Windows 2000, а не только под
    курсором.
- imagePath: ''
  $name: Picture
  $name:ru: Картинка
  $description: >-
    Path to the picture in the corner of the pane - the clouds Windows 2000 drew
    there, which live in %SystemRoot%\Web of a Windows 2000 install. BMP, GIF,
    PNG, JPEG and ICO all work, and the path may contain environment variables.
    Leave empty for no picture.
  $description:ru: >-
    Путь к картинке в углу панели - облакам, которые рисовала там Windows 2000;
    они лежат в %SystemRoot%\Web установленной Windows 2000. Подходят BMP, GIF,
    PNG, JPEG и ICO, в пути можно использовать переменные среды. Пусто - без
    картинки.
- imagePosition: background
  $name: Where the picture goes
  $name:ru: Где картинка
  $description: >-
    Windows 2000 had it in the top left corner of the pane, with the icon and
    the name drawn over it.
  $description:ru: >-
    В Windows 2000 она была в левом верхнем углу панели, а значок и имя
    рисовались поверх неё.
  $options:
  - background: In the top left corner, behind the icon and the name
  - top: Above the icon and the name
  - bottom: At the bottom of the pane
- imageWidth: 0
  $name: Picture width
  $name:ru: Ширина картинки
  $description: >-
    Width in pixels at 100% scaling. 0 stretches the picture across the pane
    instead, the way the one of Windows 2000 spans the top of its web view.
    Either way the height follows the proportions of the picture.
  $description:ru: >-
    Ширина в пикселях при масштабе 100%. 0 - растянуть картинку на всю ширину
    панели, как картинка Windows 2000 растянута по верху её веб-вида. И так и
    так высота берётся по пропорциям картинки.
- imageSmooth: false
  $name: Smooth the picture when it is scaled
  $name:ru: Сглаживать картинку при масштабировании
  $description: >-
    Off keeps the pixels as they are, which is what a picture out of Windows
    2000 wants. On smooths them, which suits a photograph.
  $description:ru: >-
    Выключено - пиксели остаются как есть, что и нужно картинке из Windows 2000.
    Включено - сглаживать, что подходит фотографии.
- imageBlendWhite: true
  $name: Let the picture's white background through
  $name:ru: Растворять белый фон картинки
  $description: >-
    Pictures like the Windows 2000 one are drawn on white, which shows as a
    white block on a pane that is not white. With this on, a picture without an
    alpha channel of its own is multiplied into the background instead: white
    leaves it untouched, everything else tints it. Pictures that do carry alpha
    are drawn by it either way.
  $description:ru: >-
    Картинки вроде той, что была в Windows 2000, нарисованы на белом, и на
    небелой панели белое лезет прямоугольником. Если включено, картинка без
    своего альфа-канала умножается на фон: белое фон не трогает, остальное его
    подкрашивает. Картинки с альфа-каналом рисуются по нему в любом случае.
- titleFontSize: 12
  $name: Size of the name
  $name:ru: Размер имени
  $description: >-
    Size of the folder or item name in points, at 100% scaling. Windows 2000
    set it well above the rest of the pane. 0 keeps the size of the other text.
  $description:ru: >-
    Размер имени папки или объекта в пунктах при масштабе 100%. В Windows 2000
    оно заметно крупнее остального текста в панели. 0 - как остальной текст.
- paneBorder: true
  $name: Take the pane inside the border of the file list
  $name:ru: Панель внутри рамки списка файлов
  $description: >-
    Windows 2000 had one sunken frame around the pane and the file list
    together, starting at the splitter of the folder tree. The pane draws the
    left, top and bottom of that frame and covers the left edge of the frame the
    list draws, so that the two read as one and no line is left between them.
  $description:ru: >-
    В Windows 2000 панель и список файлов были в одной утопленной рамке,
    начинавшейся от разделителя дерева папок. Панель дорисовывает эту рамку
    слева, сверху и снизу и закрывает левую грань рамки списка, так что они
    читаются как одна и линии между ними не остаётся.
- removeViewBorder: false
  $name: No sunken border on the file list
  $name:ru: Убрать рамку у списка файлов
  $description: >-
    Off, the file list keeps the sunken border Explorer gives it. On, the border
    is taken off - but only in the windows the pane is in, not in Explorer as a
    whole - and it is put back the moment the mod is turned off.
  $description:ru: >-
    Выключено - список файлов сохраняет утопленную рамку, которую ему рисует
    проводник. Включено - рамка убирается, но только в окнах с панелью, а не во
    всём проводнике, и возвращается, как только мод выключен.
- hideDetailsPane: false
  $name: Hide the details pane
  $name:ru: Скрыть панель сведений
  $description: >-
    Take the Windows details pane on the right out of the layout, so the new
    pane is the only one. Windows 2000 had no details pane.
  $description:ru: >-
    Убрать из раскладки панель сведений Windows справа, чтобы новая панель
    осталась единственной. В Windows 2000 панели сведений не было.
- backgroundColorSource: theme
  $name: Background colour source
  $name:ru: Источник цвета фона
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorBackground: window
  $name: Custom background colour
  $name:ru: Собственный цвет фона
  $description: >-
    A system colour name - window, buttonface, infobackground, appworkspace and
    the rest - or #RRGGBB.
  $description:ru: >-
    Имя системного цвета - window, buttonface, infobackground, appworkspace и
    так далее - либо #RRGGBB.
- titleColorSource: theme
  $name: Name colour source
  $name:ru: Источник цвета имени
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorTitle: windowtext
  $name: Custom name colour
  $name:ru: Собственный цвет имени
- textColorSource: theme
  $name: Text colour source
  $name:ru: Источник цвета текста
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorText: windowtext
  $name: Custom text colour
  $name:ru: Собственный цвет текста
- linkColorSource: custom
  $name: Link colour source
  $name:ru: Источник цвета ссылок
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorLink: '#0000FF'
  $name: Custom link colour
  $name:ru: Собственный цвет ссылок
  $description: >-
    The blue of a link, as the web view had it. It is a colour of its own rather
    than the system hotlight colour, which classic colour schemes are free to
    set to anything and often do.
  $description:ru: >-
    Синий цвет ссылки, как в веб-виде. Задан отдельным цветом, а не системным
    hotlight: классические схемы вольны ставить туда что угодно и часто ставят.
- dividerColorSource: custom
  $name: Divider colour source
  $name:ru: Источник цвета разделителя
  $options:
  - theme: Из темы Windows
  - custom: Собственный цвет
- colorDivider: '#0000FF'
  $name: Custom divider colour
  $name:ru: Собственный цвет разделителя
  $description: Only used when no divider picture is set.
  $description:ru: Используется, только если не задана картинка разделителя.
- dividerImagePath: ''
  $name: Divider picture
  $name:ru: Картинка разделителя
  $description: >-
    Path to the picture the line under the name is drawn from - the coloured bar
    of Windows 2000, one pixel tall. It is stretched across the pane and keeps
    the height it has. Leave empty to have the line drawn instead.
  $description:ru: >-
    Путь к картинке, из которой рисуется линия под именем - цветной полоске
    Windows 2000 высотой в один пиксель. Она растягивается на всю ширину панели
    и сохраняет свою высоту. Пусто - линия рисуется сама.
- dividerGradient: true
  $name: Fade the divider out
  $name:ru: Разделитель с переходом в фон
  $description: >-
    The line under the name of Windows 2000 was a gradient that started in
    colour on the left and faded into the background on the right, not a line
    of one colour all the way across.
  $description:ru: >-
    Линия под именем в Windows 2000 была градиентом: слева цветная, справа
    уходила в фон, а не сплошной одноцветной через всю панель.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Classic WebView pane in Explorer

Windows 2000 showed a pane on the left of the file list - the *WebView* - with
the icon and the name of whatever was selected, a divider line under it, a line
of text and a short list of *See also* links. Windows Vista dropped it, and
Windows 11 has no trace of it left.

This mod puts it back, and the interesting half of it works: the icon and the
name follow the selection, they fall back to the folder itself when nothing is
selected, and the links navigate the same window the way they used to.

## How it works

The content area of an Explorer window is not a set of child windows, it is a
DirectUI tree. `shell32` keeps its layout in `UIFILE` resources - one per folder
type - and hands the whole thing to `DirectUI::DUIXmlParser::SetXML` in
`dui70.dll` as one XML document, which is the single place where the layout of
every Explorer window can be seen and changed.

The mod hooks that function and inserts one element next to the one that hosts
the file list:

```
<Element layoutpos="Client" layout="shellborderlayout()">
  <BannerContainer .../>
  <Element id="atom(ClassicWebViewPane)" layoutpos="left" .../>   <-- inserted
  <Element id="atom(ViewHostContainer)" ...>
  <DetailsContainer layoutpos="right"/>
```

That element is only a spacer. Everything inside the pane - the icon, the name,
the divider, the text and the links - is drawn by the mod into a child window
of its own, which it keeps over the gap the spacer opens up between the folder
tree and the file list.

It is done that way because the parts of the shell the pane would otherwise be
built out of are gone. The Windows 2000 recreations for Windows 10 fill their
header with `PreviewThumbnail` and `PreviewMetadata` elements bound to
`PreviewThumbnailModule` and `PreviewMetadataModule`. On Windows 11 24H2 those
module names do not appear in `shell32` at all any more: the element classes
still exist and still take up space, but nothing ever fills them, so a header
built that way comes out empty. Drawing it directly also means the pane needs no
patched `shellstyle.dll` and looks right on the Classic theme, where the theme
colour functions the shell's own style sheets use return nothing.

The contents come from the shell view itself: the mod asks the
`SHELLDLL_DefView` window of the folder for its `IShellBrowser`, and through it
for the current folder and the selection, refreshing whenever the list reports
that the selection changed.

## Notes

- The text and the links come out of the settings, and they start out as the
  wording of the Russian Windows 2000. In English it read *Select an item to
  view its description.* over *See also:* and *My Documents / My Network Places
  / My Computer*.
- Windows 2000 drew a picture across the top of the pane, out of the images in
  its `%SystemRoot%\Web`. Copy that file over and give the *Picture* setting its
  path; nothing is drawn while the setting is empty. By default it is stretched
  across the pane at its own height with the pixels left alone, and it can also
  be given a width of its own, put above the name or at the bottom instead, and
  smoothed when scaled. BMP, GIF, PNG, JPEG and ICO are all read - `LoadImage`
  takes the BMPs and icons, the shell takes the rest.
- Two things in DirectUI markup take the whole window down with them - the file
  list, the folder tree and all - while `SetXML` still reports success: an
  attribute the element class does not know, and a value the parser cannot
  read. Colours from the settings are checked against the names DirectUI
  actually accepts for that reason.
- The pane sits between the folder tree and the file list. In Windows 2000 the
  tree and the WebView replaced each other; here they can both be open.
- If you also inject a WebView pane through patched `shell32` resources - the
  WinClassic recreation with Resource Redirect, say - turn that off, or you get
  two panes.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>

#include <commctrl.h>
#include <exdisp.h>
#include <objbase.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windowsx.h>

#include <algorithm>
#include <cmath>

#include <mutex>
#include <string>
#include <vector>

// The message SHELLDLL_DefView answers with the IShellBrowser of its folder.
#define WM_GETISHELLBROWSER (WM_USER + 7)


// -----------------------------------------------------------------------------
// Colours
// -----------------------------------------------------------------------------

// DirectUI hands an element that carries an attribute it cannot make sense of -
// an unknown name, or a value it fails to parse - back to the shell as a broken
// subtree, and the shell then gives up on the whole layout: the file list, the
// folder tree and everything else in the window stay blank, while SetXML still
// reports success. A colour out of the settings is the one part of the markup
// the mod does not write itself, so it is checked against what the parser
// actually accepts instead of being passed through.
struct ColorName {
    PCWSTR name;
    int sysColor;
};

constexpr ColorName kColorNames[] = {
    {L"activecaption", COLOR_ACTIVECAPTION},
    {L"appworkspace", COLOR_APPWORKSPACE},
    {L"background", COLOR_BACKGROUND},
    {L"buttonface", COLOR_BTNFACE},
    {L"buttonhighlight", COLOR_BTNHIGHLIGHT},
    {L"buttonshadow", COLOR_BTNSHADOW},
    {L"buttontext", COLOR_BTNTEXT},
    {L"captiontext", COLOR_CAPTIONTEXT},
    {L"graytext", COLOR_GRAYTEXT},
    {L"highlight", COLOR_HIGHLIGHT},
    {L"highlighttext", COLOR_HIGHLIGHTTEXT},
    {L"hotlight", COLOR_HOTLIGHT},
    {L"inactivecaption", COLOR_INACTIVECAPTION},
    {L"infobackground", COLOR_INFOBK},
    {L"infotext", COLOR_INFOTEXT},
    {L"menu", COLOR_MENU},
    {L"menutext", COLOR_MENUTEXT},
    {L"scrollbar", COLOR_SCROLLBAR},
    {L"threeddarkshadow", COLOR_3DDKSHADOW},
    {L"threedhighlight", COLOR_3DHIGHLIGHT},
    {L"threedlightshadow", COLOR_3DLIGHT},
    {L"threedshadow", COLOR_3DSHADOW},
    {L"window", COLOR_WINDOW},
    {L"windowframe", COLOR_WINDOWFRAME},
    {L"windowtext", COLOR_WINDOWTEXT},
};

// A colour the mod can both hand to DirectUI and paint with.
struct PaneColor {
    std::wstring dui = L"window";
    int sysColor = COLOR_WINDOW;  // -1 when the colour is a literal
    COLORREF rgb = 0;

    COLORREF Get() const {
        return sysColor >= 0 ? GetSysColor(sysColor) : rgb;
    }
};

static bool ParseHexColor(PCWSTR text, COLORREF* rgb) {
    if (*text != L'#' || wcslen(text) != 7) {
        return false;
    }

    unsigned value = 0;
    for (int i = 1; i <= 6; i++) {
        WCHAR c = towlower(text[i]);
        int digit;
        if (c >= L'0' && c <= L'9') {
            digit = c - L'0';
        } else if (c >= L'a' && c <= L'f') {
            digit = 10 + (c - L'a');
        } else {
            return false;
        }
        value = value * 16 + digit;
    }

    *rgb = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
    return true;
}

static PaneColor ParseColor(PCWSTR text, PCWSTR fallbackName) {
    std::wstring trimmed = text ? text : L"";
    size_t first = trimmed.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        trimmed = fallbackName;
    } else {
        trimmed = trimmed.substr(first, trimmed.find_last_not_of(L" \t") - first + 1);
    }

    for (int attempt = 0; attempt < 2; attempt++) {
        for (const ColorName& known : kColorNames) {
            if (_wcsicmp(trimmed.c_str(), known.name) == 0) {
                PaneColor color;
                color.dui = known.name;
                color.sysColor = known.sysColor;
                return color;
            }
        }

        COLORREF rgb;
        if (ParseHexColor(trimmed.c_str(), &rgb)) {
            PaneColor color;
            WCHAR dui[64];
            swprintf(dui, ARRAYSIZE(dui), L"argb(255,%u,%u,%u)", GetRValue(rgb),
                     GetGValue(rgb), GetBValue(rgb));
            color.dui = dui;
            color.sysColor = -1;
            color.rgb = rgb;
            return color;
        }

        if (attempt == 0) {
            Wh_Log(L"'%s' is not a colour DirectUI knows, using '%s'",
                   trimmed.c_str(), fallbackName);
            trimmed = fallbackName;
        }
    }

    return PaneColor();
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct SeeAlsoLink {
    std::wstring label;
    std::wstring target;
};

struct {
    int paneWidth;
    bool onRight;
    bool showHeader;
    int iconSize;
    bool showItemType;
    bool showDriveSpace;
    std::wstring usedLabel;
    std::wstring freeLabel;
    std::wstring totalLabel;
    PaneColor bar;
    std::wstring descriptionText;
    bool showSeeAlso;
    std::wstring seeAlsoTitle;
    std::vector<SeeAlsoLink> links;
    bool underlineLinks;
    std::wstring imagePath;
    enum class ImagePlace { Background, Top, Bottom } imagePlace;
    int imageWidth;
    bool imageSmooth;
    bool imageBlendWhite;
    int titleFontSize;
    bool paneBorder;
    bool removeViewBorder;
    bool hideDetailsPane;
    PaneColor background;
    PaneColor title;
    PaneColor text;
    PaneColor link;
    PaneColor divider;
    bool dividerGradient;
    std::wstring dividerImagePath;
    std::wstring spacerXml;  // the element handed to DirectUI, built once
} g_settings;

static std::wstring Px(int value) {
    return std::to_wstring(value) + L"rp";
}

// A path out of the settings, with any environment variables in it filled in.
static std::wstring PathSetting(PCWSTR name) {
    WindhawkUtils::StringSetting setting =
        WindhawkUtils::StringSetting::make(name);
    if (!*setting.get()) {
        return L"";
    }

    WCHAR expanded[MAX_PATH * 2];
    DWORD length =
        ExpandEnvironmentStringsW(setting.get(), expanded, ARRAYSIZE(expanded));
    if (length > 0 && length <= ARRAYSIZE(expanded)) {
        return expanded;
    }

    return setting.get();
}

// A colour can either follow a stable system colour supplied by the current
// theme, or use the companion free-form setting. PaneColor keeps the system
// colour index rather than taking a snapshot, so GetSysColor is evaluated at
// paint time and a theme change is reflected without rebuilding the pane.
static PaneColor ThemedOrCustomColor(PCWSTR sourceSetting,
                                     PCWSTR colorSetting,
                                     PCWSTR themeColor,
                                     PCWSTR customFallback,
                                     bool themeByDefault) {
    WindhawkUtils::StringSetting source =
        WindhawkUtils::StringSetting::make(sourceSetting);
    bool fromTheme = *source.get()
                         ? _wcsicmp(source.get(), L"custom") != 0
                         : themeByDefault;
    if (fromTheme) {
        return ParseColor(themeColor, themeColor);
    }

    WindhawkUtils::StringSetting custom =
        WindhawkUtils::StringSetting::make(colorSetting);
    return ParseColor(custom.get(), customFallback);
}

static void LoadSettings() {
    g_settings.paneWidth = Wh_GetIntSetting(L"paneWidth");
    if (g_settings.paneWidth < 40) {
        g_settings.paneWidth = 40;
    } else if (g_settings.paneWidth > 2000) {
        g_settings.paneWidth = 2000;
    }

    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"position");
    g_settings.onRight = wcscmp(position.get(), L"right") == 0;

    g_settings.showHeader = Wh_GetIntSetting(L"showHeader");

    g_settings.iconSize = Wh_GetIntSetting(L"iconSize");
    if (g_settings.iconSize < 8) {
        g_settings.iconSize = 8;
    } else if (g_settings.iconSize > 256) {
        g_settings.iconSize = 256;
    }

    g_settings.showItemType = Wh_GetIntSetting(L"showItemType");
    g_settings.showDriveSpace = Wh_GetIntSetting(L"showDriveSpace");

    WindhawkUtils::StringSetting usedLabel =
        WindhawkUtils::StringSetting::make(L"usedLabel");
    WindhawkUtils::StringSetting freeLabel =
        WindhawkUtils::StringSetting::make(L"freeLabel");
    WindhawkUtils::StringSetting totalLabel =
        WindhawkUtils::StringSetting::make(L"totalLabel");
    g_settings.usedLabel = usedLabel.get();
    g_settings.freeLabel = freeLabel.get();
    g_settings.totalLabel = totalLabel.get();

    g_settings.bar = ThemedOrCustomColor(
        L"barColorSource", L"colorBar", L"highlight", L"#000080", true);

    WindhawkUtils::StringSetting descriptionText =
        WindhawkUtils::StringSetting::make(L"descriptionText");
    g_settings.descriptionText = descriptionText.get();

    g_settings.showSeeAlso = Wh_GetIntSetting(L"showSeeAlso");

    WindhawkUtils::StringSetting seeAlsoTitle =
        WindhawkUtils::StringSetting::make(L"seeAlsoTitle");
    g_settings.seeAlsoTitle = seeAlsoTitle.get();

    g_settings.links.clear();
    for (int i = 0;; i++) {
        WindhawkUtils::StringSetting label =
            WindhawkUtils::StringSetting::make(L"seeAlso[%d].label", i);
        WindhawkUtils::StringSetting target =
            WindhawkUtils::StringSetting::make(L"seeAlso[%d].target", i);
        if (!*label.get() && !*target.get()) {
            break;
        }
        if (*label.get() && *target.get()) {
            g_settings.links.push_back({label.get(), target.get()});
        }
    }

    g_settings.underlineLinks = Wh_GetIntSetting(L"underlineLinks");

    g_settings.imagePath = PathSetting(L"imagePath");
    g_settings.dividerImagePath = PathSetting(L"dividerImagePath");

    WindhawkUtils::StringSetting imagePosition =
        WindhawkUtils::StringSetting::make(L"imagePosition");
    if (wcscmp(imagePosition.get(), L"top") == 0) {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Top;
    } else if (wcscmp(imagePosition.get(), L"bottom") == 0) {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Bottom;
    } else {
        g_settings.imagePlace = decltype(g_settings.imagePlace)::Background;
    }

    g_settings.imageWidth = Wh_GetIntSetting(L"imageWidth");
    if (g_settings.imageWidth < 0) {
        g_settings.imageWidth = 0;
    } else if (g_settings.imageWidth > 2000) {
        g_settings.imageWidth = 2000;
    }

    g_settings.imageSmooth = Wh_GetIntSetting(L"imageSmooth");
    g_settings.imageBlendWhite = Wh_GetIntSetting(L"imageBlendWhite");

    g_settings.titleFontSize = Wh_GetIntSetting(L"titleFontSize");
    if (g_settings.titleFontSize < 0) {
        g_settings.titleFontSize = 0;
    } else if (g_settings.titleFontSize > 72) {
        g_settings.titleFontSize = 72;
    }

    g_settings.paneBorder = Wh_GetIntSetting(L"paneBorder");
    g_settings.removeViewBorder = Wh_GetIntSetting(L"removeViewBorder");
    g_settings.hideDetailsPane = Wh_GetIntSetting(L"hideDetailsPane");

    g_settings.background = ThemedOrCustomColor(
        L"backgroundColorSource", L"colorBackground", L"window", L"window",
        true);
    g_settings.title = ThemedOrCustomColor(
        L"titleColorSource", L"colorTitle", L"windowtext", L"windowtext",
        true);
    g_settings.text = ThemedOrCustomColor(
        L"textColorSource", L"colorText", L"windowtext", L"windowtext",
        true);
    g_settings.link = ThemedOrCustomColor(
        L"linkColorSource", L"colorLink", L"hotlight", L"#0000FF", false);
    g_settings.divider = ThemedOrCustomColor(
        L"dividerColorSource", L"colorDivider", L"highlight", L"#0000FF",
        false);
    g_settings.dividerGradient = Wh_GetIntSetting(L"dividerGradient");

    g_settings.spacerXml =
        L"<Element id=\"atom(ClassicWebViewPane)\" layoutpos=\"" +
        std::wstring(g_settings.onRight ? L"right" : L"left") + L"\" width=\"" +
        Px(g_settings.paneWidth) + L"\" background=\"" + g_settings.background.dui +
        L"\"/>";
}

// -----------------------------------------------------------------------------
// The layout hook
// -----------------------------------------------------------------------------

// The id of the spacer, also used to recognise a document the mod has already
// been through and one that carries somebody else's WebView.
constexpr WCHAR kPaneAtom[] = L"atom(ClassicWebViewPane)";

// The element that hosts the file list. The spacer goes right before it.
constexpr WCHAR kViewHostContainer[] = L"<Element id=\"atom(ViewHostContainer)\"";

constexpr WCHAR kDetailsContainer[] = L"<DetailsContainer layoutpos=\"right\"/>";
constexpr WCHAR kDetailsContainerHidden[] =
    L"<DetailsContainer layoutpos=\"none\" width=\"0\" height=\"0\"/>";

using SetXML_t = HRESULT(WINAPI*)(void* pThis,
                                  const WCHAR* pszXML,
                                  HINSTANCE hInst,
                                  HINSTANCE hResInst);
SetXML_t SetXML_Original;

HRESULT WINAPI SetXML_Hook(void* pThis,
                           const WCHAR* pszXML,
                           HINSTANCE hInst,
                           HINSTANCE hResInst) {
    if (!pszXML) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    // Only folder layouts, and only ones the mod has not been through.
    const WCHAR* viewHost = wcsstr(pszXML, kViewHostContainer);
    if (!viewHost || wcsstr(pszXML, kPaneAtom)) {
        return SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    std::wstring modified(pszXML, viewHost);
    modified += g_settings.spacerXml;
    modified += viewHost;

    if (g_settings.hideDetailsPane) {
        size_t details = modified.find(kDetailsContainer);
        if (details != std::wstring::npos) {
            modified.replace(details, wcslen(kDetailsContainer),
                             kDetailsContainerHidden);
        }
    }

    HRESULT hr = SetXML_Original(pThis, modified.c_str(), hInst, hResInst);
    if (FAILED(hr)) {
        Wh_Log(L"The parser rejected the pane (%08X), using the original layout",
               hr);
        hr = SetXML_Original(pThis, pszXML, hInst, hResInst);
    }

    return hr;
}

// -----------------------------------------------------------------------------
// The pane window
// -----------------------------------------------------------------------------

constexpr PCWSTR kPaneClassName = L"ClassicWebViewPane";

constexpr UINT_PTR kRefreshTimer = 1;
constexpr UINT WM_PANE_CLOSE = WM_APP + 1;

// Sent to a folder view to make it build its pane on its own thread. Registered
// rather than WM_APP based - it goes to a window of the shell, not of the mod.
UINT g_adoptMessage;

struct PaneLink {
    RECT rect;
    size_t index;
};

struct Pane {
    HWND hwnd = nullptr;
    HWND host = nullptr;     // the DirectUIHWND the pane lives in
    HWND defView = nullptr;  // the folder view the contents come from

    std::wstring title;
    std::wstring subtitle;
    HICON icon = nullptr;
    bool showingSelection = false;

    bool hasSpace = false;
    ULONGLONG capacity = 0;
    ULONGLONG freeSpace = 0;

    std::vector<PaneLink> linkRects;
    int hotLink = -1;
    bool tracking = false;

    HFONT font = nullptr;
    HFONT fontBold = nullptr;
    HFONT fontTitle = nullptr;
    HFONT fontUnderline = nullptr;
    int dpi = 96;
};

std::mutex g_panesMutex;
std::vector<HWND> g_panes;

struct Subclass {
    HWND hWnd;
    WindhawkUtils::WH_SUBCLASSPROC proc;
};

std::mutex g_subclassesMutex;
std::vector<Subclass> g_subclasses;

static bool AddSubclass(HWND hWnd,
                        WindhawkUtils::WH_SUBCLASSPROC proc,
                        DWORD_PTR data) {
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, proc, data)) {
        return false;
    }

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    g_subclasses.push_back({hWnd, proc});
    return true;
}

static void DropSubclass(HWND hWnd, WindhawkUtils::WH_SUBCLASSPROC proc) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, proc);

    std::lock_guard<std::mutex> guard(g_subclassesMutex);
    std::erase_if(g_subclasses, [hWnd, proc](const Subclass& subclass) {
        return subclass.hWnd == hWnd && subclass.proc == proc;
    });
}

static void DropAllSubclasses() {
    std::vector<Subclass> subclasses;
    {
        std::lock_guard<std::mutex> guard(g_subclassesMutex);
        subclasses.swap(g_subclasses);
    }

    for (const Subclass& subclass : subclasses) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(subclass.hWnd,
                                                         subclass.proc);
    }
}

static bool IsClassName(HWND hWnd, PCWSTR name) {
    WCHAR className[64];
    if (!hWnd || !GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return _wcsicmp(className, name) == 0;
}

// The file list of Explorer carries a sunken border, and on a classic theme
// that border draws a line between the list and the pane, where Windows 2000
// had none. It is taken off the lists of the windows the pane is in and nowhere
// else, and every list it was taken off is remembered so that it can be given
// back when the mod stops.
std::mutex g_bordersMutex;
std::vector<HWND> g_borderless;

static void RestoreViewBorders() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> guard(g_bordersMutex);
        windows.swap(g_borderless);
    }

    for (HWND window : windows) {
        if (!IsWindow(window)) {
            continue;
        }

        LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
        SetWindowLongPtrW(window, GWL_EXSTYLE, exStyle | WS_EX_CLIENTEDGE);
        SetWindowPos(window, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
    }
}

static void ApplyViewBorder(HWND defView) {
    if (!g_settings.removeViewBorder || !defView) {
        return;
    }

    for (HWND child = GetWindow(defView, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        LONG_PTR exStyle = GetWindowLongPtrW(child, GWL_EXSTYLE);
        if (!(exStyle & WS_EX_CLIENTEDGE)) {
            continue;
        }

        SetWindowLongPtrW(child, GWL_EXSTYLE, exStyle & ~WS_EX_CLIENTEDGE);
        SetWindowPos(child, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);

        std::lock_guard<std::mutex> guard(g_bordersMutex);
        g_borderless.push_back(child);
    }
}

static int Scale(const Pane* pane, int value) {
    return MulDiv(value, pane->dpi, 96);
}

// -----------------------------------------------------------------------------
// What the pane shows
// -----------------------------------------------------------------------------

// The browser behind a folder view is the way to both the folder and the
// selection. A view of the shell answers WM_GETISHELLBROWSER with it directly;
// where it does not, the open browsers are walked instead and the one whose
// active view is this window wins - which is also what keeps tabs apart, since
// every tab of a window has a view of its own.
static IShellBrowser* GetShellBrowser(HWND defView) {
    if (!defView || !IsWindow(defView)) {
        return nullptr;
    }

    DWORD_PTR result = 0;
    if (SendMessageTimeoutW(defView, WM_GETISHELLBROWSER, 0, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, &result) &&
        result) {
        return reinterpret_cast<IShellBrowser*>(result);
    }

    IShellWindows* windows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&windows)) ||
        !windows) {
        return nullptr;
    }

    IShellBrowser* found = nullptr;

    long count = 0;
    windows->get_Count(&count);
    for (long i = 0; i < count && !found; i++) {
        VARIANT index = {};
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* dispatch = nullptr;
        if (windows->Item(index, &dispatch) != S_OK || !dispatch) {
            continue;
        }

        IServiceProvider* provider = nullptr;
        if (SUCCEEDED(dispatch->QueryInterface(IID_IServiceProvider,
                                               (void**)&provider)) &&
            provider) {
            IShellBrowser* browser = nullptr;
            if (SUCCEEDED(provider->QueryService(SID_STopLevelBrowser,
                                                 IID_IShellBrowser,
                                                 (void**)&browser)) &&
                browser) {
                IShellView* view = nullptr;
                if (SUCCEEDED(browser->QueryActiveShellView(&view)) && view) {
                    HWND viewWindow = nullptr;
                    if (SUCCEEDED(view->GetWindow(&viewWindow)) &&
                        viewWindow == defView) {
                        found = browser;
                        found->AddRef();
                    }
                    view->Release();
                }
                browser->Release();
            }
            provider->Release();
        }

        dispatch->Release();
    }

    windows->Release();

    // The reference is dropped right away: the browser outlives the view it
    // belongs to, and the pane only ever uses it while handling a message of
    // that view.
    if (found) {
        found->Release();
    }

    return found;
}

static IFolderView* GetFolderView(HWND defView) {
    IShellBrowser* browser = GetShellBrowser(defView);
    if (!browser) {
        return nullptr;
    }

    IShellView* view = nullptr;
    HRESULT hr = browser->QueryActiveShellView(&view);
    if (FAILED(hr) || !view) {
        return nullptr;
    }

    IFolderView* folderView = nullptr;
    hr = view->QueryInterface(IID_IFolderView, (void**)&folderView);
    view->Release();
    return folderView;
}

static std::wstring GetItemText(IShellItem* item, SIGDN form) {
    PWSTR name = nullptr;
    if (FAILED(item->GetDisplayName(form, &name)) || !name) {
        return L"";
    }
    std::wstring result = name;
    CoTaskMemFree(name);
    return result;
}

static std::wstring FormatSize(ULONGLONG bytes) {
    WCHAR text[64];
    if (!StrFormatByteSizeW(bytes, text, ARRAYSIZE(text))) {
        return L"";
    }
    return text;
}

// System.ItemTypeText and System.Size, both out of the storage property set.
constexpr PROPERTYKEY kPropertyItemTypeText = {
    {0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
    4};
constexpr PROPERTYKEY kPropertySize = {
    {0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}},
    12};

// The kind of the item and, for a file, its size - the second line Windows 2000
// put under the name.
static std::wstring DescribeItem(IShellItem* item) {
    std::wstring description;

    IShellItem2* item2 = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_IShellItem2, (void**)&item2)) && item2) {
        PWSTR kind = nullptr;
        if (SUCCEEDED(item2->GetString(kPropertyItemTypeText, &kind)) && kind) {
            description = kind;
            CoTaskMemFree(kind);
        }

        SFGAOF attributes = 0;
        bool isFolder = SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &attributes)) &&
                        (attributes & SFGAO_FOLDER);
        if (!isFolder) {
            ULONGLONG size = 0;
            if (SUCCEEDED(item2->GetUInt64(kPropertySize, &size)) && size) {
                std::wstring sizeText = FormatSize(size);
                if (!sizeText.empty()) {
                    description += description.empty() ? L"" : L", ";
                    description += sizeText;
                }
            }
        }

        item2->Release();
    }

    return description;
}

// System.Capacity and System.FreeSpace, out of the volume property set - what a
// drive answers with, and what Windows 2000 drew its bar of used space from.
constexpr PROPERTYKEY kPropertyFreeSpace = {
    {0x9B174B35, 0x40FF, 0x11D2, {0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71}},
    2};
constexpr PROPERTYKEY kPropertyCapacity = {
    {0x9B174B35, 0x40FF, 0x11D2, {0xA2, 0x7E, 0x00, 0xC0, 0x4F, 0xC3, 0x08, 0x71}},
    3};

static bool GetItemSpace(IShellItem* item,
                         ULONGLONG* capacity,
                         ULONGLONG* freeSpace) {
    *capacity = 0;
    *freeSpace = 0;

    // Only a drive gets the bar. The shell answers the volume properties for
    // any folder on the volume, so the item has to be the root of one.
    PWSTR path = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) {
        return false;
    }

    std::wstring root = path;
    CoTaskMemFree(path);

    if (!PathIsRootW(root.c_str())) {
        return false;
    }

    IShellItem2* item2 = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_IShellItem2, (void**)&item2)) &&
        item2) {
        ULONGLONG total = 0;
        ULONGLONG free = 0;
        if (SUCCEEDED(item2->GetUInt64(kPropertyCapacity, &total)) &&
            SUCCEEDED(item2->GetUInt64(kPropertyFreeSpace, &free)) && total > 0) {
            *capacity = total;
            *freeSpace = free;
        }
        item2->Release();
    }

    if (*capacity > 0) {
        return true;
    }

    // A drive that keeps no such properties - a network one, say - still
    // answers the plain question about its path.
    ULARGE_INTEGER available = {};
    ULARGE_INTEGER total = {};
    ULARGE_INTEGER free = {};
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, &free) &&
        total.QuadPart > 0) {
        *capacity = total.QuadPart;
        *freeSpace = free.QuadPart;
    }

    return *capacity > 0;
}

static HICON GetItemIcon(IShellItem* item, int size) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetIDListFromObject(item, &pidl)) || !pidl) {
        return nullptr;
    }

    SHFILEINFOW info = {};
    UINT flags = SHGFI_PIDL | SHGFI_ICON |
                 (size <= 16 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
    HICON icon = nullptr;
    if (SHGetFileInfoW((PCWSTR)pidl, 0, &info, sizeof(info), flags)) {
        icon = info.hIcon;
    }

    CoTaskMemFree(pidl);
    return icon;
}

static IShellItem* GetPaneItem(HWND defView, bool* isSelection) {
    *isSelection = false;

    IFolderView* folderView = GetFolderView(defView);
    if (!folderView) {
        return nullptr;
    }

    IShellItem* item = nullptr;

    int selected = 0;
    if (SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &selected)) &&
        selected == 1) {
        IShellItemArray* array = nullptr;
        if (SUCCEEDED(folderView->Items(SVGIO_SELECTION, IID_IShellItemArray,
                                        (void**)&array)) &&
            array) {
            if (SUCCEEDED(array->GetItemAt(0, &item)) && item) {
                *isSelection = true;
            }
            array->Release();
        }
    }

    if (!item) {
        IPersistFolder2* folder = nullptr;
        if (SUCCEEDED(folderView->GetFolder(IID_IPersistFolder2, (void**)&folder)) &&
            folder) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(folder->GetCurFolder(&pidl)) && pidl) {
                SHCreateItemFromIDList(pidl, IID_IShellItem, (void**)&item);
                CoTaskMemFree(pidl);
            }
            folder->Release();
        }
    }

    folderView->Release();
    return item;
}

static void RefreshPane(Pane* pane) {
    std::wstring title;
    std::wstring subtitle;
    HICON icon = nullptr;
    bool hasSpace = false;
    ULONGLONG capacity = 0;
    ULONGLONG freeSpace = 0;

    bool isSelection = false;
    IShellItem* item = GetPaneItem(pane->defView, &isSelection);
    if (item) {
        title = GetItemText(item, SIGDN_NORMALDISPLAY);
        if (g_settings.showItemType) {
            subtitle = DescribeItem(item);
        }
        if (g_settings.showHeader) {
            icon = GetItemIcon(item, Scale(pane, g_settings.iconSize));
        }
        if (g_settings.showDriveSpace) {
            hasSpace = GetItemSpace(item, &capacity, &freeSpace);
        }
        item->Release();
    }

    if (pane->title == title && pane->subtitle == subtitle &&
        pane->showingSelection == isSelection && pane->hasSpace == hasSpace &&
        pane->capacity == capacity && pane->freeSpace == freeSpace &&
        pane->icon && icon) {
        // Same item, nothing to repaint - drop the icon that was just fetched.
        DestroyIcon(icon);
        return;
    }

    pane->hasSpace = hasSpace;
    pane->capacity = capacity;
    pane->freeSpace = freeSpace;

    if (pane->icon) {
        DestroyIcon(pane->icon);
    }
    pane->icon = icon;
    pane->title = title;
    pane->subtitle = subtitle;
    pane->showingSelection = isSelection;

    InvalidateRect(pane->hwnd, nullptr, TRUE);
}

// -----------------------------------------------------------------------------
// Painting
// -----------------------------------------------------------------------------

// Windows 2000 kept a watermark at the bottom of its web view, wvlogo.gif out of
// %SystemRoot%\Web. The mod draws whatever file the settings point at, and lets
// the shell load it - which is what makes GIF, PNG and JPEG work next to BMP
// without dragging in an imaging library.
struct PaneImage {
    HBITMAP bitmap = nullptr;
    HICON icon = nullptr;
    int width = 0;
    int height = 0;
    bool alpha = false;
};

// The pane draws two pictures: the one in its corner and the coloured line
// under the name, both of which the settings can point anywhere.
struct CachedImage {
    PaneImage image;
    std::wstring path;
    bool loaded = false;
};

std::mutex g_imageMutex;
CachedImage g_picture;
CachedImage g_dividerPicture;

static void FreeCachedImage(CachedImage& cache) {
    if (cache.image.bitmap) {
        DeleteObject(cache.image.bitmap);
    }
    if (cache.image.icon) {
        DestroyIcon(cache.image.icon);
    }
    cache = {};
}

static void FreeImage() {
    FreeCachedImage(g_picture);
    FreeCachedImage(g_dividerPicture);
}

// AlphaBlend wants the colour channels premultiplied by the alpha one, and what
// comes back from a file depends on where it came from: the shell hands over
// premultiplied bits, a 32 bit BMP usually carries an alpha channel that is
// either straight or plain unused. Both are told apart by looking at the bits -
// a channel brighter than its own alpha cannot be premultiplied, and an alpha
// channel that is zero everywhere is not one.
static bool NormalizeAlpha(const BITMAP& info) {
    if (info.bmBitsPixel != 32 || !info.bmBits) {
        return false;
    }

    BYTE* bits = (BYTE*)info.bmBits;
    bool anyAlpha = false;
    bool straight = false;

    for (int row = 0; row < info.bmHeight && !straight; row++) {
        BYTE* pixel = bits + (size_t)row * info.bmWidthBytes;
        for (int column = 0; column < info.bmWidth; column++, pixel += 4) {
            if (pixel[3]) {
                anyAlpha = true;
            }
            if (pixel[0] > pixel[3] || pixel[1] > pixel[3] ||
                pixel[2] > pixel[3]) {
                straight = true;
                break;
            }
        }
    }

    if (!anyAlpha) {
        return false;
    }
    if (!straight) {
        return true;
    }

    for (int row = 0; row < info.bmHeight; row++) {
        BYTE* pixel = bits + (size_t)row * info.bmWidthBytes;
        for (int column = 0; column < info.bmWidth; column++, pixel += 4) {
            BYTE alpha = pixel[3];
            pixel[0] = (BYTE)(pixel[0] * alpha / 255);
            pixel[1] = (BYTE)(pixel[1] * alpha / 255);
            pixel[2] = (BYTE)(pixel[2] * alpha / 255);
        }
    }

    return true;
}

static bool HasExtension(const std::wstring& path, PCWSTR extension) {
    size_t dot = path.rfind(L'.');
    return dot != std::wstring::npos &&
           _wcsicmp(path.c_str() + dot, extension) == 0;
}

static PaneImage LoadPaneImage(const std::wstring& path) {
    PaneImage image;

    if (HasExtension(path, L".ico") || HasExtension(path, L".cur")) {
        image.icon = (HICON)LoadImageW(nullptr, path.c_str(), IMAGE_ICON, 0, 0,
                                       LR_LOADFROMFILE | LR_DEFAULTSIZE);
        if (image.icon) {
            ICONINFO info = {};
            if (GetIconInfo(image.icon, &info)) {
                BITMAP bitmap = {};
                HBITMAP source = info.hbmColor ? info.hbmColor : info.hbmMask;
                if (GetObjectW(source, sizeof(bitmap), &bitmap)) {
                    image.width = bitmap.bmWidth;
                    image.height =
                        info.hbmColor ? bitmap.bmHeight : bitmap.bmHeight / 2;
                }
                if (info.hbmColor) {
                    DeleteObject(info.hbmColor);
                }
                if (info.hbmMask) {
                    DeleteObject(info.hbmMask);
                }
            }
            return image;
        }
    }

    HBITMAP bitmap =
        (HBITMAP)LoadImageW(nullptr, path.c_str(), IMAGE_BITMAP, 0, 0,
                            LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!bitmap) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                                  IID_IShellItem,
                                                  (void**)&item)) &&
            item) {
            IShellItemImageFactory* factory = nullptr;
            if (SUCCEEDED(item->QueryInterface(IID_IShellItemImageFactory,
                                               (void**)&factory)) &&
                factory) {
                SIZE size = {512, 512};
                factory->GetImage(size, SIIGBF_BIGGERSIZEOK, &bitmap);
                factory->Release();
            }
            item->Release();
        }
    }

    if (bitmap) {
        BITMAP info = {};
        if (GetObjectW(bitmap, sizeof(info), &info) && info.bmWidth > 0 &&
            info.bmHeight > 0) {
            image.bitmap = bitmap;
            image.width = info.bmWidth;
            image.height = info.bmHeight;
            image.alpha = NormalizeAlpha(info);
        } else {
            DeleteObject(bitmap);
        }
    }

    return image;
}

// Loads what the path points at, once, and hands it back for as long as the
// setting keeps pointing there. The caller holds the lock.
static const PaneImage* EnsureImage(CachedImage& cache,
                                    const std::wstring& path) {
    if (path.empty()) {
        if (cache.loaded) {
            FreeCachedImage(cache);
        }
        return nullptr;
    }

    if (!cache.loaded || cache.path != path) {
        FreeCachedImage(cache);
        cache.image = LoadPaneImage(path);
        cache.path = path;
        cache.loaded = true;
        if (!cache.image.bitmap && !cache.image.icon) {
            Wh_Log(L"'%s' could not be loaded", path.c_str());
        }
    }

    if (!cache.image.width || !cache.image.height) {
        return nullptr;
    }

    return &cache.image;
}

// A picture with no alpha channel of its own - the Windows 2000 one is an 8 bit
// BMP - is drawn on white, and a white block is exactly what it turns into on a
// pane that is not white. Multiplying it into what is already there is the way
// out: white leaves the background alone, everything else tints it, and a
// gradient fading to white fades out instead of ending at an edge.
static void MultiplyIntoBackground(HDC dc,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   HBITMAP source,
                                   int sourceWidth,
                                   int sourceHeight) {
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // top down
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    BYTE* backgroundBits = nullptr;
    BYTE* pictureBits = nullptr;
    HDC backgroundDc = CreateCompatibleDC(dc);
    HDC pictureDc = CreateCompatibleDC(dc);
    HBITMAP background = CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                                          (void**)&backgroundBits, nullptr, 0);
    HBITMAP picture = CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                                       (void**)&pictureBits, nullptr, 0);

    if (backgroundDc && pictureDc && background && picture) {
        HBITMAP oldBackground = (HBITMAP)SelectObject(backgroundDc, background);
        HBITMAP oldPicture = (HBITMAP)SelectObject(pictureDc, picture);

        BitBlt(backgroundDc, 0, 0, width, height, dc, x, y, SRCCOPY);

        HDC sourceDc = CreateCompatibleDC(dc);
        HBITMAP oldSource = (HBITMAP)SelectObject(sourceDc, source);
        SetStretchBltMode(pictureDc,
                          g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(pictureDc, 0, 0, width, height, sourceDc, 0, 0, sourceWidth,
                   sourceHeight, SRCCOPY);
        SelectObject(sourceDc, oldSource);
        DeleteDC(sourceDc);

        size_t count = (size_t)width * height * 4;
        for (size_t i = 0; i < count; i++) {
            if (i % 4 == 3) {
                continue;  // the unused byte of an RGB DIB
            }
            backgroundBits[i] =
                (BYTE)(backgroundBits[i] * pictureBits[i] / 255);
        }

        BitBlt(dc, x, y, width, height, backgroundDc, 0, 0, SRCCOPY);

        SelectObject(backgroundDc, oldBackground);
        SelectObject(pictureDc, oldPicture);
    }

    if (background) {
        DeleteObject(background);
    }
    if (picture) {
        DeleteObject(picture);
    }
    if (backgroundDc) {
        DeleteDC(backgroundDc);
    }
    if (pictureDc) {
        DeleteDC(pictureDc);
    }
}

// The line under the name was a picture as well - a thin bar of colour. It is
// stretched across the pane and keeps the height it was drawn at. Returns that
// height, or nothing when there is no picture to draw.
static int PaintDividerImage(HDC dc, int x, int y, int width, int dpi) {
    std::lock_guard<std::mutex> guard(g_imageMutex);

    const PaneImage* line =
        EnsureImage(g_dividerPicture, g_settings.dividerImagePath);
    if (!line || !line->bitmap || width <= 0) {
        return 0;
    }

    int height = MulDiv(line->height, dpi, 96);
    if (height < 1) {
        height = 1;
    }

    if (!dc) {
        return height;
    }

    HDC source = CreateCompatibleDC(dc);
    HBITMAP old = (HBITMAP)SelectObject(source, line->bitmap);

    if (line->alpha) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, width, height, source, 0, 0, line->width,
                   line->height, blend);
    } else {
        int mode = SetStretchBltMode(
            dc, g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(dc, x, y, width, height, source, 0, 0, line->width,
                   line->height, SRCCOPY);
        SetStretchBltMode(dc, mode);
    }

    SelectObject(source, old);
    DeleteDC(source);

    return height;
}

// Measures the picture, and draws it as well when a device context is given.
// The lock is held across the drawing so that a settings change cannot free the
// bitmap out from under a paint on another thread.
static SIZE PaintPaneImage(HDC dc, int x, int y, int dpi, int maxWidth) {
    SIZE size = {0, 0};

    std::lock_guard<std::mutex> guard(g_imageMutex);

    const PaneImage* picture = EnsureImage(g_picture, g_settings.imagePath);
    if (!picture) {
        return size;
    }
    const PaneImage& g_image = *picture;

    // Without a width of its own the picture is stretched across the pane, the
    // way the one of Windows 2000 spans the top of its web view however wide it
    // is. Either way the proportions of the picture are kept.
    size.cx = g_settings.imageWidth > 0 ? MulDiv(g_settings.imageWidth, dpi, 96)
                                        : maxWidth;
    if (size.cx > maxWidth) {
        size.cx = maxWidth;
    }
    size.cy = MulDiv(size.cx, g_image.height, g_image.width);

    if (size.cx <= 0 || size.cy <= 0) {
        size.cx = 0;
        size.cy = 0;
        return size;
    }

    if (!dc) {
        return size;
    }

    if (g_image.icon) {
        DrawIconEx(dc, x, y, g_image.icon, size.cx, size.cy, 0, nullptr,
                   DI_NORMAL);
        return size;
    }

    if (!g_image.alpha && g_settings.imageBlendWhite) {
        MultiplyIntoBackground(dc, x, y, size.cx, size.cy, g_image.bitmap,
                               g_image.width, g_image.height);
        return size;
    }

    HDC source = CreateCompatibleDC(dc);
    HBITMAP old = (HBITMAP)SelectObject(source, g_image.bitmap);

    if (g_image.alpha) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, x, y, size.cx, size.cy, source, 0, 0, g_image.width,
                   g_image.height, blend);
    } else {
        int mode = SetStretchBltMode(
            dc, g_settings.imageSmooth ? HALFTONE : COLORONCOLOR);
        StretchBlt(dc, x, y, size.cx, size.cy, source, 0, 0, g_image.width,
                   g_image.height, SRCCOPY);
        SetStretchBltMode(dc, mode);
    }

    SelectObject(source, old);
    DeleteDC(source);

    return size;
}

static void EnsureFonts(Pane* pane) {
    if (pane->font) {
        return;
    }

    NONCLIENTMETRICSW metrics = {sizeof(metrics)};
    if (!SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics),
                                    &metrics, 0, pane->dpi)) {
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
    }

    LOGFONTW logFont = metrics.lfMessageFont;
    pane->font = CreateFontIndirectW(&logFont);

    LOGFONTW bold = logFont;
    bold.lfWeight = FW_BOLD;
    pane->fontBold = CreateFontIndirectW(&bold);

    // Windows 2000 set the name of the folder well above the rest of the pane.
    LOGFONTW title = bold;
    if (g_settings.titleFontSize > 0) {
        title.lfHeight = -MulDiv(g_settings.titleFontSize, pane->dpi, 72);
        title.lfWidth = 0;
    }
    pane->fontTitle = CreateFontIndirectW(&title);

    LOGFONTW underline = logFont;
    underline.lfUnderline = TRUE;
    pane->fontUnderline = CreateFontIndirectW(&underline);
}

static void FreeFonts(Pane* pane) {
    if (pane->font) {
        DeleteObject(pane->font);
        pane->font = nullptr;
    }
    if (pane->fontBold) {
        DeleteObject(pane->fontBold);
        pane->fontBold = nullptr;
    }
    if (pane->fontTitle) {
        DeleteObject(pane->fontTitle);
        pane->fontTitle = nullptr;
    }
    if (pane->fontUnderline) {
        DeleteObject(pane->fontUnderline);
        pane->fontUnderline = nullptr;
    }
}

// Draws wrapped text and returns the height it took.
static int DrawWrapped(HDC dc,
                       PCWSTR text,
                       HFONT font,
                       COLORREF color,
                       int x,
                       int y,
                       int width) {
    if (!text || !*text) {
        return 0;
    }

    HFONT old = (HFONT)SelectObject(dc, font);
    SetTextColor(dc, color);

    RECT rect = {x, y, x + width, y};
    DrawTextW(dc, text, -1, &rect, DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
    DrawTextW(dc, text, -1, &rect, DT_WORDBREAK | DT_NOPREFIX);

    SelectObject(dc, old);
    return rect.bottom - rect.top;
}

static COLORREF MixColor(COLORREF first, COLORREF second, int firstWeight) {
    int secondWeight = 255 - firstWeight;
    return RGB((GetRValue(first) * firstWeight + GetRValue(second) * secondWeight) /
                   255,
               (GetGValue(first) * firstWeight + GetGValue(second) * secondWeight) /
                   255,
               (GetBValue(first) * firstWeight + GetBValue(second) * secondWeight) /
                   255);
}

// The classic disk page and the Windows 2000 WebView built the chart from a
// top ellipse plus a darker copy shifted down to form the visible side wall.
static void DrawDrivePie(HDC dc,
                         RECT bounds,
                         unsigned usedPer1000,
                         COLORREF usedColor,
                         COLORREF freeColor) {
    int availableWidth = bounds.right - bounds.left;
    int availableHeight = bounds.bottom - bounds.top;
    int width = std::min(availableWidth, availableHeight * 2);
    int height = width / 2;
    if (width < 20 || height < 10) {
        return;
    }

    bounds.left += (availableWidth - width) / 2;
    bounds.right = bounds.left + width;
    bounds.top += (availableHeight - height) / 2;
    bounds.bottom = bounds.top + height;

    int depth = std::max(2, height / 6);
    RECT top = bounds;
    top.bottom -= depth;
    int cx = (top.left + top.right) / 2;
    int cy = (top.top + top.bottom) / 2;
    int rx = (top.right - top.left) / 2;
    int ry = (top.bottom - top.top) / 2;

    usedPer1000 = std::min(usedPer1000, 1000u);
    double freeFraction = (1000.0 - usedPer1000) / 1000.0;
    constexpr double pi = 3.14159265358979323846;
    double angle = pi + freeFraction * 2.0 * pi;
    POINT start = {top.left, cy};
    POINT end = {cx + (int)(rx * cos(angle)),
                 cy - (int)(ry * sin(angle))};

    COLORREF frameColor = GetSysColor(COLOR_WINDOWFRAME);
    COLORREF usedShadow = MixColor(usedColor, RGB(0, 0, 0), 128);
    COLORREF freeShadow = MixColor(freeColor, RGB(0, 0, 0), 128);
    HPEN pen = CreatePen(PS_SOLID, 1, frameColor);
    HPEN oldPen = (HPEN)SelectObject(dc, pen);

    RECT lower = top;
    OffsetRect(&lower, 0, depth);
    HBRUSH brush = CreateSolidBrush(usedShadow);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);
    Ellipse(dc, lower.left, lower.top, lower.right, lower.bottom);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    if (usedPer1000 > 0 && usedPer1000 < 1000) {
        brush = CreateSolidBrush(freeShadow);
        oldBrush = (HBRUSH)SelectObject(dc, brush);
        Pie(dc, lower.left, lower.top, lower.right, lower.bottom,
            start.x, start.y + depth, end.x, end.y + depth);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);
    }

    brush = CreateSolidBrush(usedColor);
    oldBrush = (HBRUSH)SelectObject(dc, brush);
    Ellipse(dc, top.left, top.top, top.right, top.bottom);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);

    if (usedPer1000 > 0 && usedPer1000 < 1000) {
        brush = CreateSolidBrush(freeColor);
        oldBrush = (HBRUSH)SelectObject(dc, brush);
        Pie(dc, top.left, top.top, top.right, top.bottom, start.x, start.y,
            end.x, end.y);
        SelectObject(dc, oldBrush);
        DeleteObject(brush);

        if (end.y >= cy) {
            MoveToEx(dc, end.x, end.y, nullptr);
            LineTo(dc, end.x, end.y + depth);
        }
    }

    Arc(dc, lower.left, lower.top, lower.right, lower.bottom,
        lower.left, cy + depth, lower.right, cy + depth);
    MoveToEx(dc, top.left, cy, nullptr);
    LineTo(dc, top.left, cy + depth);
    MoveToEx(dc, top.right - 1, cy, nullptr);
    LineTo(dc, top.right - 1, cy + depth);

    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

static void PaintPane(Pane* pane, HDC target, const RECT& client) {
    EnsureFonts(pane);

    int width = client.right - client.left;
    int height = client.bottom - client.top;

    HDC dc = CreateCompatibleDC(target);
    HBITMAP bitmap = CreateCompatibleBitmap(target, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, bitmap);

    HBRUSH background = CreateSolidBrush(g_settings.background.Get());
    FillRect(dc, &client, background);
    DeleteObject(background);

    SetBkMode(dc, TRANSPARENT);

    // The left, top and bottom of the frame that goes round the pane and the
    // file list together; the right of it belongs to the list, whose own left
    // edge the pane covers.
    int edge = 0;
    if (g_settings.paneBorder && !g_settings.removeViewBorder) {
        edge = GetSystemMetricsForDpi(SM_CXEDGE, pane->dpi);
        RECT frame = client;
        if (g_settings.onRight) {
            DrawEdge(dc, &frame, EDGE_SUNKEN, BF_RIGHT | BF_TOP | BF_BOTTOM);
        } else {
            DrawEdge(dc, &frame, EDGE_SUNKEN, BF_LEFT | BF_TOP | BF_BOTTOM);
        }
    }

    int padding = Scale(pane, 12);
    int x = padding + (g_settings.onRight ? 0 : edge);
    int y = padding + edge;
    int contentWidth = width - padding * 2 - edge;

    pane->linkRects.clear();

    // Narrower than its own padding, the pane is only a strip of background.
    if (contentWidth > 0) {
    // In Windows 2000 the picture sits right in the corner of the pane, with
    // the icon and the name over it, so it gets neither the padding nor a share
    // of the height.
    if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Background) {
        PaintPaneImage(dc, g_settings.onRight ? 0 : edge, edge, pane->dpi,
                       width - edge);
    } else if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Top) {
        SIZE image = PaintPaneImage(nullptr, 0, 0, pane->dpi, contentWidth);
        if (image.cy > 0) {
            PaintPaneImage(dc, x, y, pane->dpi, contentWidth);
            y += image.cy + Scale(pane, 8);
        }
    }

    // The icon sits on a line of its own with the name in bold under it, the
    // way the web view of Windows 2000 had it - not next to it, the way the
    // details pane of Windows 11 does.
    if (g_settings.showHeader) {
        if (pane->icon) {
            int iconSize = Scale(pane, g_settings.iconSize);
            DrawIconEx(dc, x, y, pane->icon, iconSize, iconSize, 0, nullptr,
                       DI_NORMAL);
            y += iconSize + Scale(pane, 4);
        }

        y += DrawWrapped(dc, pane->title.c_str(), pane->fontTitle,
                         g_settings.title.Get(), x, y, contentWidth);

        if (!pane->subtitle.empty()) {
            y += Scale(pane, 1);
            y += DrawWrapped(dc, pane->subtitle.c_str(), pane->font,
                             g_settings.text.Get(), x, y, contentWidth);
        }

        y += Scale(pane, 6);
    }

    {
        // The line runs the whole width of the pane, edge to edge, and is the
        // one thing in it that ignores the padding - it stops at the frame,
        // and at the strip that lies over the border of the file list.
        int lineLeft = edge;
        int lineRight = width - edge;
        int drawn =
            PaintDividerImage(dc, lineLeft, y, lineRight - lineLeft, pane->dpi);
        RECT line = {lineLeft, y, lineRight,
                     y + (drawn > 0 ? drawn : std::max(1, Scale(pane, 1)))};

        if (drawn > 0) {
            // The picture is the line.
        } else if (g_settings.dividerGradient) {
            // The line of Windows 2000 was a picture that started in colour and
            // faded out to the right; a gradient into the background of the
            // pane is the same thing without the file.
            COLORREF from = g_settings.divider.Get();
            COLORREF to = g_settings.background.Get();

            TRIVERTEX vertices[2] = {};
            vertices[0].x = line.left;
            vertices[0].y = line.top;
            vertices[0].Red = (USHORT)(GetRValue(from) * 257);
            vertices[0].Green = (USHORT)(GetGValue(from) * 257);
            vertices[0].Blue = (USHORT)(GetBValue(from) * 257);
            vertices[1].x = line.right;
            vertices[1].y = line.bottom;
            vertices[1].Red = (USHORT)(GetRValue(to) * 257);
            vertices[1].Green = (USHORT)(GetGValue(to) * 257);
            vertices[1].Blue = (USHORT)(GetBValue(to) * 257);

            GRADIENT_RECT band = {0, 1};
            GradientFill(dc, vertices, 2, &band, 1, GRADIENT_FILL_RECT_H);
        } else {
            HBRUSH divider = CreateSolidBrush(g_settings.divider.Get());
            FillRect(dc, &line, divider);
            DeleteObject(divider);
        }

        y = line.bottom + Scale(pane, 10);
    }

    // Windows 2000 put this instruction before the capacity and chart when a
    // drive itself was being viewed.
    if (pane->hasSpace && !g_settings.descriptionText.empty() &&
        !pane->showingSelection) {
        y += DrawWrapped(dc, g_settings.descriptionText.c_str(), pane->font,
                         g_settings.text.Get(), x, y, contentWidth);
        y += Scale(pane, 14);
    }

    // What a drive gets: capacity, the used/free legend and the classic 3-D
    // pie chart. The construction follows the Disk Pie Chart Windhawk mod and
    // the old shell implementation it was reconstructed from.
    if (pane->hasSpace && pane->capacity > 0) {
        HFONT oldFont = (HFONT)SelectObject(dc, pane->font);
        SetTextColor(dc, g_settings.text.Get());

        TEXTMETRICW metrics = {};
        GetTextMetricsW(dc, &metrics);
        int lineHeight = metrics.tmHeight;
        int rowGap = Scale(pane, 4);
        ULONGLONG used = pane->capacity > pane->freeSpace
                             ? pane->capacity - pane->freeSpace
                             : 0;
        COLORREF usedColor = g_settings.bar.Get();
        COLORREF freeColor =
            MixColor(usedColor, g_settings.background.Get(), 64);

        std::wstring capacity = g_settings.totalLabel;
        if (!capacity.empty()) {
            capacity += L" ";
        }
        capacity += FormatSize(pane->capacity);
        RECT capacityRect = {x, y, x + contentWidth, y + lineHeight};
        DrawTextW(dc, capacity.c_str(), -1, &capacityRect,
                  DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX |
                      DT_END_ELLIPSIS);
        y = capacityRect.bottom + Scale(pane, 12);

        const struct {
            const std::wstring* label;
            ULONGLONG value;
            COLORREF color;
        } legend[] = {
            {&g_settings.usedLabel, used, usedColor},
            {&g_settings.freeLabel, pane->freeSpace, freeColor},
        };

        int square = std::max(8, Scale(pane, 11));
        int legendX = x + Scale(pane, 16);
        for (const auto& item : legend) {
            RECT swatch = {legendX, y + (lineHeight - square) / 2,
                           legendX + square, y + (lineHeight - square) / 2 + square};
            HBRUSH swatchBrush = CreateSolidBrush(item.color);
            FillRect(dc, &swatch, swatchBrush);
            DeleteObject(swatchBrush);
            DrawEdge(dc, &swatch, BDR_SUNKENOUTER, BF_RECT);

            std::wstring text = *item.label;
            if (!text.empty()) {
                text += L" ";
            }
            text += FormatSize(item.value);
            RECT textRect = {swatch.right + Scale(pane, 5), y,
                             x + contentWidth, y + lineHeight};
            DrawTextW(dc, text.c_str(), -1, &textRect,
                      DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX |
                          DT_END_ELLIPSIS);
            y += lineHeight + rowGap;
        }

        y += Scale(pane, 10);
        int pieWidth = std::min(contentWidth, Scale(pane, 120));
        int pieHeight = std::max(Scale(pane, 60), pieWidth / 2);
        RECT pie = {x, y, x + pieWidth, y + pieHeight};
        unsigned usedPer1000 =
            (unsigned)((double)used * 1000.0 / (double)pane->capacity);
        DrawDrivePie(dc, pie, usedPer1000, usedColor, freeColor);
        y = pie.bottom;

        SelectObject(dc, oldFont);
        y += Scale(pane, 8);
    }

    // For an ordinary folder, this is the instruction under the divider. A
    // drive handled it above, before its capacity block.
    if (!pane->hasSpace && !g_settings.descriptionText.empty() &&
        !pane->showingSelection) {
        y += DrawWrapped(dc, g_settings.descriptionText.c_str(), pane->font,
                         g_settings.text.Get(), x, y, contentWidth);
    }

    if (g_settings.showSeeAlso && !g_settings.links.empty()) {
        y += Scale(pane, 16);

        if (!g_settings.seeAlsoTitle.empty()) {
            y += DrawWrapped(dc, g_settings.seeAlsoTitle.c_str(), pane->fontBold,
                             g_settings.text.Get(), x, y, contentWidth);
            y += Scale(pane, 4);
        }

        for (size_t i = 0; i < g_settings.links.size(); i++) {
            const std::wstring& label = g_settings.links[i].label;
            HFONT font = g_settings.underlineLinks || (int)i == pane->hotLink
                             ? pane->fontUnderline
                             : pane->font;

            HFONT old = (HFONT)SelectObject(dc, font);
            SIZE size = {};
            GetTextExtentPoint32W(dc, label.c_str(), (int)label.length(), &size);
            SetTextColor(dc, g_settings.link.Get());
            RECT rect = {x, y, x + std::min<LONG>(size.cx, contentWidth), y + size.cy};
            DrawTextW(dc, label.c_str(), -1, &rect,
                      DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS);
            SelectObject(dc, old);

            pane->linkRects.push_back({rect, i});
            y = rect.bottom + Scale(pane, 3);
        }
    }

    if (g_settings.imagePlace == decltype(g_settings.imagePlace)::Bottom) {
        SIZE image = PaintPaneImage(nullptr, 0, 0, pane->dpi, contentWidth);
        int imageY = height - padding - image.cy;
        if (image.cy > 0 && imageY > y) {
            PaintPaneImage(dc, x, imageY, pane->dpi, contentWidth);
        }
    }
    }

    BitBlt(target, 0, 0, width, height, dc, 0, 0, SRCCOPY);

    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

// -----------------------------------------------------------------------------
// Clicking a link
// -----------------------------------------------------------------------------

static void FollowLink(Pane* pane, const SeeAlsoLink& link) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(link.target.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        Wh_Log(L"'%s' is not something the shell can navigate to",
               link.target.c_str());
        return;
    }

    IShellBrowser* browser = GetShellBrowser(pane->defView);
    if (browser) {
        browser->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
    } else {
        SHELLEXECUTEINFOW info = {sizeof(info)};
        info.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
        info.lpIDList = pidl;
        info.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&info);
    }

    CoTaskMemFree(pidl);
}

static int LinkFromPoint(Pane* pane, POINT point) {
    for (const PaneLink& link : pane->linkRects) {
        if (PtInRect(&link.rect, point)) {
            return (int)link.index;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// The window itself
// -----------------------------------------------------------------------------

static LRESULT CALLBACK PaneWndProc(HWND hWnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    Pane* pane = (Pane*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    switch (message) {
        case WM_NCCREATE: {
            pane = new Pane();
            pane->hwnd = hWnd;
            pane->dpi = GetDpiForWindow(hWnd);
            if (!pane->dpi) {
                pane->dpi = 96;
            }
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pane);
            break;
        }

        case WM_PAINT: {
            if (pane) {
                PAINTSTRUCT paint;
                HDC dc = BeginPaint(hWnd, &paint);
                RECT client;
                GetClientRect(hWnd, &client);
                PaintPane(pane, dc, client);
                EndPaint(hWnd, &paint);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            if (!pane) {
                break;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hot = LinkFromPoint(pane, point);
            if (hot != pane->hotLink) {
                pane->hotLink = hot;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            if (!pane->tracking) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, hWnd, 0};
                TrackMouseEvent(&track);
                pane->tracking = true;
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            if (pane) {
                pane->tracking = false;
                if (pane->hotLink != -1) {
                    pane->hotLink = -1;
                    InvalidateRect(hWnd, nullptr, TRUE);
                }
            }
            return 0;
        }

        case WM_SETCURSOR: {
            if (pane && pane->hotLink != -1) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONUP: {
            if (!pane) {
                break;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = LinkFromPoint(pane, point);
            if (index >= 0 && (size_t)index < g_settings.links.size()) {
                FollowLink(pane, g_settings.links[index]);
            }
            return 0;
        }

        case WM_TIMER: {
            if (pane && wParam == kRefreshTimer) {
                KillTimer(hWnd, kRefreshTimer);
                RefreshPane(pane);
                return 0;
            }
            break;
        }

        case WM_DPICHANGED_AFTERPARENT: {
            if (pane) {
                pane->dpi = GetDpiForWindow(hWnd);
                if (!pane->dpi) {
                    pane->dpi = 96;
                }
                FreeFonts(pane);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE: {
            if (pane) {
                FreeFonts(pane);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            break;
        }

        case WM_PANE_CLOSE: {
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_NCDESTROY: {
            {
                std::lock_guard<std::mutex> guard(g_panesMutex);
                std::erase(g_panes, hWnd);
            }
            if (pane) {
                if (pane->icon) {
                    DestroyIcon(pane->icon);
                }
                FreeFonts(pane);
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
                delete pane;
            }
            break;
        }
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

static ATOM g_paneClass;

static bool RegisterPaneClass() {
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = PaneWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kPaneClassName;
    g_paneClass = RegisterClassExW(&wc);
    return g_paneClass != 0;
}

// The gap the spacer opens up: everything between the file list and whatever
// sits on the other side of it, which is the folder tree when it is open.
static bool GetPaneRect(HWND host, HWND defView, HWND self, RECT* result) {
    RECT hostClient;
    RECT viewRect;
    if (!GetClientRect(host, &hostClient) || !GetWindowRect(defView, &viewRect)) {
        return false;
    }

    MapWindowPoints(nullptr, host, (POINT*)&viewRect, 2);

    UINT dpi = GetDpiForWindow(host);
    if (!dpi) {
        dpi = 96;
    }
    int paneWidth = MulDiv(g_settings.paneWidth, dpi, 96);

    // The spacer is what makes the room, so the pane is anchored to the file
    // list and given the width the spacer was asked for, rather than taking
    // everything up to the next window - the folder tree of a freshly opened
    // view is not always there yet when the pane is first placed.
    // With the border on, the pane runs a couple of pixels into the file list,
    // over the edge of the frame the list draws for itself: that is what makes
    // the two frames read as one around the pane and the list together.
    int overlap = 0;
    if (g_settings.paneBorder && !g_settings.removeViewBorder) {
        overlap = GetSystemMetricsForDpi(SM_CXEDGE, dpi);
    }

    RECT rect = viewRect;
    if (g_settings.onRight) {
        rect.left = viewRect.right;
        rect.right = std::min(hostClient.right, viewRect.right + paneWidth);
    } else {
        rect.right = viewRect.left;
        rect.left = std::max(hostClient.left, viewRect.left - paneWidth);
    }

    // Anything else next to the file list - the folder tree, the details pane -
    // takes its own share of that space first.
    for (HWND child = GetWindow(host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (child == self || !IsWindowVisible(child)) {
            continue;
        }

        RECT childRect;
        if (!GetWindowRect(child, &childRect)) {
            continue;
        }
        MapWindowPoints(nullptr, host, (POINT*)&childRect, 2);

        if (childRect.right <= rect.left || childRect.left >= rect.right) {
            continue;
        }

        if (g_settings.onRight) {
            if (childRect.left >= rect.left) {
                rect.right = std::min(rect.right, childRect.left);
            }
        } else {
            if (childRect.right <= rect.right) {
                rect.left = std::max(rect.left, childRect.right);
            }
        }
    }

    if (rect.right <= rect.left || rect.bottom <= rect.top) {
        return false;
    }

    if (g_settings.onRight) {
        rect.left -= overlap;
    } else {
        rect.right += overlap;
    }

    *result = rect;
    return true;
}

static void LayOutPane(HWND paneWindow) {
    Pane* pane = (Pane*)GetWindowLongPtrW(paneWindow, GWLP_USERDATA);
    if (!pane || !pane->host || !pane->defView || !IsWindow(pane->defView)) {
        return;
    }

    RECT rect;
    if (!GetPaneRect(pane->host, pane->defView, paneWindow, &rect)) {
        ShowWindow(paneWindow, SW_HIDE);
        return;
    }

    // Above the file list, so that the strip of the pane that lies over the
    // left edge of its frame actually covers it.
    SetWindowPos(paneWindow, HWND_TOP, rect.left, rect.top,
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// -----------------------------------------------------------------------------
// Finding the folder view
// -----------------------------------------------------------------------------

static HWND FindPaneOfHost(HWND host) {
    for (HWND child = GetWindow(host, GW_CHILD); child;
         child = GetWindow(child, GW_HWNDNEXT)) {
        if (IsClassName(child, kPaneClassName)) {
            return child;
        }
    }
    return nullptr;
}

// The pane of a folder view, kept on the view itself so that the subclass can
// find it whichever thread put it there.
constexpr PCWSTR kPaneProperty = L"ClassicWebViewPane_Pane";

static void AttachToDefViewOnItsThread(HWND defView);

LRESULT CALLBACK DefViewSubclassProc(HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     DWORD_PTR data) {
    HWND paneWindow = (HWND)GetPropW(hWnd, kPaneProperty);

    if (message == g_adoptMessage) {
        AttachToDefViewOnItsThread(hWnd);
        return 0;
    }

    switch (message) {

        case WM_NOTIFY: {
            NMHDR* header = (NMHDR*)lParam;
            if (header &&
                (header->code == (UINT)LVN_ITEMCHANGED ||
                 header->code == (UINT)LVN_ODSTATECHANGED ||
                 header->code == (UINT)LVN_DELETEALLITEMS ||
                 header->code == (UINT)LVN_INSERTITEM)) {
                if (IsWindow(paneWindow)) {
                    // Coalesce - a rubber band selection reports every item.
                    SetTimer(paneWindow, kRefreshTimer, 80, nullptr);
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            // A folder view is created under the tab window and only put into
            // the DirectUI host afterwards, so the pane cannot be built when
            // the view appears - this is the first moment it can be.
            if (!IsWindow(paneWindow)) {
                AttachToDefViewOnItsThread(hWnd);
                paneWindow = (HWND)GetPropW(hWnd, kPaneProperty);
            }
            if (IsWindow(paneWindow)) {
                ApplyViewBorder(hWnd);
                LayOutPane(paneWindow);
            }
            break;
        }

        case WM_NCDESTROY: {
            RemovePropW(hWnd, kPaneProperty);
            DropSubclass(hWnd, DefViewSubclassProc);
            break;
        }
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

static void AttachToDefViewOnItsThread(HWND defView) {
    HWND host = GetParent(defView);
    while (host && !IsClassName(host, L"DirectUIHWND")) {
        host = GetParent(host);
    }
    if (!host) {
        return;
    }

    HWND paneWindow = FindPaneOfHost(host);
    if (!paneWindow) {
        paneWindow = CreateWindowExW(
            0, kPaneClassName, nullptr,
            WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, host, nullptr,
            GetModuleHandleW(nullptr), nullptr);
        if (!paneWindow) {
            Wh_Log(L"The pane window could not be created");
            return;
        }

        std::lock_guard<std::mutex> guard(g_panesMutex);
        g_panes.push_back(paneWindow);
    }

    Pane* pane = (Pane*)GetWindowLongPtrW(paneWindow, GWLP_USERDATA);
    if (!pane) {
        return;
    }

    pane->host = host;
    pane->defView = defView;
    SetPropW(defView, kPaneProperty, paneWindow);

    ApplyViewBorder(defView);
    LayOutPane(paneWindow);

    // The view has only just been created; it needs a moment before it can
    // answer for its folder.
    SetTimer(paneWindow, kRefreshTimer, 120, nullptr);
}

// The window may belong to another thread - the mod is loaded into Explorer
// windows that are already open, and every Explorer window runs on one of its
// own. A window has to be created by the thread that pumps its parent, so the
// work is handed over to the view itself.
static void AttachToDefView(HWND defView) {
    if (!AddSubclass(defView, DefViewSubclassProc, 0)) {
        return;
    }

    if (GetWindowThreadProcessId(defView, nullptr) == GetCurrentThreadId()) {
        AttachToDefViewOnItsThread(defView);
    } else {
        PostMessageW(defView, g_adoptMessage, 0, 0);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 PCWSTR className,
                                 PCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 PVOID param) {
    HWND result = CreateWindowExW_Original(exStyle, className, windowName, style,
                                           x, y, width, height, parent, menu,
                                           instance, param);
    if (result && IsClassName(result, L"SHELLDLL_DefView")) {
        AttachToDefView(result);
    }

    return result;
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void AdoptOpenWindows() {
    // Windows that were already open when the mod was loaded. Their layout has
    // no spacer in it, so the pane stays hidden until they are reopened, but
    // picking them up costs nothing and keeps the state consistent.
    struct Enumerator {
        static BOOL CALLBACK Child(HWND hWnd, LPARAM param) {
            if (IsClassName(hWnd, L"SHELLDLL_DefView")) {
                AttachToDefView(hWnd);
            } else {
                EnumChildWindows(hWnd, Child, param);
            }
            return TRUE;
        }

        static BOOL CALLBACK Top(HWND hWnd, LPARAM param) {
            DWORD processId = 0;
            GetWindowThreadProcessId(hWnd, &processId);
            if (processId == GetCurrentProcessId()) {
                EnumChildWindows(hWnd, Child, param);
            }
            return TRUE;
        }
    };

    EnumWindows(Enumerator::Top, 0);
}

static void ClosePanes() {
    std::vector<HWND> panes;
    {
        std::lock_guard<std::mutex> guard(g_panesMutex);
        panes = g_panes;
    }

    for (HWND pane : panes) {
        if (IsWindow(pane)) {
            SendMessageTimeoutW(pane, WM_PANE_CLOSE, 0, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 1000, nullptr);
        }
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_adoptMessage = RegisterWindowMessageW(L"ClassicWebViewPane_Adopt");

    if (!RegisterPaneClass()) {
        Wh_Log(L"The pane window class could not be registered");
        return FALSE;
    }

    HMODULE dui70 = LoadLibraryW(L"dui70.dll");
    if (!dui70) {
        Wh_Log(L"dui70.dll could not be loaded");
        return FALSE;
    }

    // public: long __cdecl DirectUI::DUIXmlParser::SetXML(unsigned short const *,
    //     struct HINSTANCE__ *, struct HINSTANCE__ *)
    void* setXml = (void*)GetProcAddress(
        dui70, "?SetXML@DUIXmlParser@DirectUI@@QEAAJPEBGPEAUHINSTANCE__@@1@Z");
    if (!setXml) {
        // The 32 bit name, for a 32 bit host.
        setXml = (void*)GetProcAddress(
            dui70, "?SetXML@DUIXmlParser@DirectUI@@QAAJPBGPAUHINSTANCE__@@1@Z");
    }
    if (!setXml) {
        Wh_Log(L"DUIXmlParser::SetXML was not found in dui70.dll");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(setXml, (void*)SetXML_Hook,
                            (void**)&SetXML_Original)) {
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW,
                                            CreateWindowExW_Hook,
                                            &CreateWindowExW_Original)) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    AdoptOpenWindows();
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::vector<HWND> panes;
    {
        std::lock_guard<std::mutex> guard(g_panesMutex);
        panes = g_panes;
    }

    if (!g_settings.removeViewBorder) {
        RestoreViewBorders();
    }

    for (HWND paneWindow : panes) {
        if (!IsWindow(paneWindow)) {
            continue;
        }

        Pane* pane = (Pane*)GetWindowLongPtrW(paneWindow, GWLP_USERDATA);
        if (pane) {
            ApplyViewBorder(pane->defView);
        }

        LayOutPane(paneWindow);
        InvalidateRect(paneWindow, nullptr, TRUE);
    }
}

void Wh_ModUninit() {
    // A window still carrying a subclass of the mod, or a window whose window
    // procedure is in it, calls into a DLL that is about to be gone.
    ClosePanes();
    DropAllSubclasses();
    RestoreViewBorders();

    {
        std::lock_guard<std::mutex> guard(g_imageMutex);
        FreeImage();
    }

    if (g_paneClass) {
        UnregisterClassW(kPaneClassName, GetModuleHandleW(nullptr));
        g_paneClass = 0;
    }
}
