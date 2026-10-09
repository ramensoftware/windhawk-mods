// ==WindhawkMod==
// @id              shake-to-find-cursor
// @name            Shake to Find Cursor
// @description     Temporarily enlarges the mouse cursor when you shake the mouse, like macOS "Shake to locate"
// @version         1.4
// @author          Darius Varnelis
// @github          https://github.com/Darius-Varnelis
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shake to Find Cursor

![Shake to Find Cursor demo](https://raw.githubusercontent.com/Darius-Varnelis/shake-to-find-cursor/main/shake-to-find.gif)

Shake the mouse (or swipe back and forth on the touchpad) and the cursor grows
so you can spot it instantly. Stop shaking and it shrinks back.

## How it works

* Runs as a *tool mod* in its own background process, so it never touches
  Explorer.
* Listens to raw mouse input: no polling, no CPU use while the mouse is idle,
  and it can't add input lag the way a low-level mouse hook can.
* A shake means the cursor travelled a long way (**Shake distance**) while
  staying inside a small area (**Back-and-forth ratio**) within the
  **Time window**.
* Enlarging replaces the real system cursors with bigger versions loaded from
  your current cursor scheme's files at the target size. The big cursor *is*
  the pointer: no overlay window, no trailing, and it keeps your cursor
  style and colour. The normal cursors are restored the same way Control Panel
  applies a scheme.
* Keep shaking and the cursor keeps growing, slower and slower: each further
  step of the same size takes twice as long as the previous one. By default
  it stops at the height of your screen, which takes hours of shaking.
* The grow and shrink animations are time-based and run at your display's
  refresh rate. While animating, only the cursor that's on screen is resized,
  which keeps every frame cheap.
* Animated cursors (.ani) grow and shrink smoothly too, and keep animating
  while enlarged: the mod plays the animation itself, one frame at a time at
  the current size, and hands back to the real cursor when it's done.

## vs. macOS magnifying cursor

The catalog already has [macOS magnifying cursor](https://windhawk.net/mods/mac-magnifying-cursor),
which does something similar but works differently: it hides the system
cursors and draws a scaled copy in its own overlay window, polling the cursor
position every 16-40 ms. This mod instead replaces the real system cursors
with bigger renders of your own cursor scheme, driven by raw input rather
than polling. In practice that means no overlay lag or trailing, the enlarged
cursor keeps your actual cursor style and colour, and there's no CPU cost
while the mouse is idle. Don't enable both at once - they'll fight over the
system cursors.

## Notes

* Apps that draw their own custom cursor keep their small one.
* Cursor files contain images of up to 256 px, so past that the cursor is
  upscaled and gets softer.
* If the mod's process is killed while the cursor is big, the normal cursors
  are restored the next time the mod starts (re-applying a pointer scheme in
  Mouse Properties also fixes it).
* Raw input may not reach the mod while an elevated window (Task Manager, a
  UAC prompt, an installer) has focus, depending on Windows version. If so, a
  shake over such a window won't be detected until focus moves elsewhere.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scalePercent: 400
  $name: Enlarged size (%)
  $description: Size of the enlarged cursor relative to its normal size (400 = 4x bigger)
- keepGrowing: 50
  $name: Keep growing while shaking (%)
  $description: >-
    If you keep shaking after the first enlargement, the cursor keeps growing.
    It grows by this much in the first second, then slower and slower (the next
    step of the same size takes 2 s, then 4 s, 8 s and so on).
    0 = stop at the first enlargement
- sizeLimit: 0
  $name: Size limit (px)
  $description: The cursor never grows past this size (0 = the height of your screen)
- shakeDistance: 800
  $name: Shake distance (px)
  $description: How far the cursor has to travel within the time window. Lower = easier to trigger
- shakeRatio: 350
  $name: Back-and-forth ratio (%)
  $description: >-
    Distance travelled divided by the diagonal of the area the cursor stayed in.
    A straight swipe is about 100, four strokes left and right about 400.
    Lower = easier to trigger, but fast normal movements may start to count as shakes
- shakeWindow: 1000
  $name: Time window (ms)
  $description: How much recent movement is looked at when detecting a shake
- holdTime: 500
  $name: Stay enlarged for (ms)
  $description: How long the cursor stays big after you stop shaking
- growDuration: 150
  $name: Grow animation (ms)
  $description: How long the cursor takes to grow (0 = instant)
- shrinkDuration: 250
  $name: Shrink animation (ms)
  $description: How long the cursor takes to shrink back (0 = instant)
- disableInFullscreen: true
  $name: Disable in fullscreen apps and games
  $description: Don't enlarge the cursor while a fullscreen app, game or presentation is in the foreground
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <cmath>
#include <deque>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int scalePercent = 400;
    int keepGrowingPercent = 50;
    int sizeLimit = 0;  // 0 = height of the screen.
    int shakeDistance = 800;
    int shakeRatioPercent = 350;
    int shakeWindowMs = 1000;
    int holdMs = 500;
    int growMs = 150;
    int shrinkMs = 250;
    bool disableInFullscreen = true;
};

// Only accessed on the worker thread once it's running.
Settings g_settings;

int PositiveIntSetting(PCWSTR name, int fallback, int minValue, int maxValue) {
    int value = Wh_GetIntSetting(name);
    if (value <= 0) {
        value = fallback;
    }
    return std::clamp(value, minValue, maxValue);
}

Settings LoadSettings() {
    Settings s;
    s.scalePercent = PositiveIntSetting(L"scalePercent", 400, 110, 1000);
    s.keepGrowingPercent = std::clamp(Wh_GetIntSetting(L"keepGrowing"), 0, 500);
    const int sizeLimit = Wh_GetIntSetting(L"sizeLimit");
    s.sizeLimit = sizeLimit <= 0 ? 0 : std::clamp(sizeLimit, 32, 4096);
    s.shakeDistance = PositiveIntSetting(L"shakeDistance", 800, 100, 20000);
    s.shakeRatioPercent = PositiveIntSetting(L"shakeRatio", 350, 150, 2000);
    s.shakeWindowMs = PositiveIntSetting(L"shakeWindow", 1000, 200, 5000);
    s.holdMs = std::clamp(Wh_GetIntSetting(L"holdTime"), 0, 10000);
    s.growMs = std::clamp(Wh_GetIntSetting(L"growDuration"), 0, 5000);
    s.shrinkMs = std::clamp(Wh_GetIntSetting(L"shrinkDuration"), 0, 5000);
    s.disableInFullscreen = Wh_GetIntSetting(L"disableInFullscreen") != 0;
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// System cursors

struct CursorType {
    DWORD id;               // OCR_* value used by SetSystemCursor.
    PCWSTR registryName;    // Value under HKCU\Control Panel\Cursors.
    bool animatedByDefault; // Built-in version is animated.
};

// OCR_* constants need OEMRESOURCE defined before <windows.h>, which Windhawk
// has already included by now, so the raw values are used.
constexpr CursorType kCursorTypes[] = {
    {32512, L"Arrow", false},        // OCR_NORMAL
    {32513, L"IBeam", false},        // OCR_IBEAM
    {32514, L"Wait", true},          // OCR_WAIT
    {32515, L"Crosshair", false},    // OCR_CROSS
    {32516, L"UpArrow", false},      // OCR_UP
    {32642, L"SizeNWSE", false},     // OCR_SIZENWSE
    {32643, L"SizeNESW", false},     // OCR_SIZENESW
    {32644, L"SizeWE", false},       // OCR_SIZEWE
    {32645, L"SizeNS", false},       // OCR_SIZENS
    {32646, L"SizeAll", false},      // OCR_SIZEALL
    {32648, L"No", false},           // OCR_NO
    {32649, L"Hand", false},         // OCR_HAND
    {32650, L"AppStarting", true},   // OCR_APPSTARTING
    {32651, L"Help", false},         // OCR_HELP
    {32631, L"NWPen", false},        // Handwriting
    {32671, L"Pin", false},          // Location select
    {32672, L"Person", false},       // Person select
};

// Reads the cursor file configured for this cursor type in the current scheme.
// An empty value means the scheme uses the built-in cursor.
bool GetSchemeCursorPath(PCWSTR registryName, WCHAR (&path)[MAX_PATH]) {
    WCHAR raw[MAX_PATH];
    DWORD bytes = sizeof(raw);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                     registryName,
                     RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
                     nullptr, raw, &bytes) != ERROR_SUCCESS ||
        !raw[0]) {
        return false;
    }

    DWORD length = ExpandEnvironmentStringsW(raw, path, MAX_PATH);
    return length > 0 && length <= MAX_PATH;
}

bool IsAnimatedCursorFile(PCWSTR path) {
    int length = lstrlenW(path);
    return length >= 4 && lstrcmpiW(path + length - 4, L".ani") == 0;
}

// One frame of an animated cursor, kept in memory so the grow and shrink
// animations can show it at any size without decoding every frame of the .ani
// file (60+ frames in some themes) on every animation step.
struct StaticFrame {
    std::vector<BYTE> file;      // The frame's .cur/.ico data.
    std::vector<BYTE> resource;  // The image in use, prepared for
                                 // CreateIconFromResourceEx.
    int preparedEntry = -1;      // Which image `resource` holds.
    int width = 0;
    int height = 0;
    int hotspotX = 0;
    int hotspotY = 0;
};

constexpr DWORD FourCC(char a, char b, char c, char d) {
    return static_cast<DWORD>(static_cast<BYTE>(a)) |
           static_cast<DWORD>(static_cast<BYTE>(b)) << 8 |
           static_cast<DWORD>(static_cast<BYTE>(c)) << 16 |
           static_cast<DWORD>(static_cast<BYTE>(d)) << 24;
}

bool ReadAt(HANDLE file, ULONGLONG offset, void* buffer, DWORD size) {
    LARGE_INTEGER position;
    position.QuadPart = static_cast<LONGLONG>(offset);
    DWORD read = 0;
    return SetFilePointerEx(file, position, nullptr, FILE_BEGIN) &&
           ReadFile(file, buffer, size, &read, nullptr) && read == size;
}

// Where each frame of an .ani file is and when each one is shown, so the grow
// and shrink animations can show the frame that's due at each step without
// decoding the whole file. Only chunk headers and the timing tables are read
// up front; frames are read one at a time, when they're needed.
struct AniIndex {
    std::vector<ULONGLONG> frameOffsets;  // Start of each frame's .cur data.
    std::vector<DWORD> frameSizes;
    std::vector<DWORD> stepFrames;    // Frame shown at each step.
    std::vector<DWORD> stepJiffies;   // How long each step lasts (1/60 s).
    ULONGLONG totalJiffies = 0;
};

constexpr DWORD kMaxAniFrames = 4096;

bool ReadAniIndex(PCWSTR path, AniIndex& index) {
    index = {};
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    // anih: cbSize, nFrames, nSteps, cx, cy, bitCount, planes, jifRate, flags.
    DWORD header[9]{};
    bool haveHeader = false;
    std::vector<DWORD> rates;
    std::vector<DWORD> sequence;

    LARGE_INTEGER fileSize{};
    DWORD riff[3];
    if (GetFileSizeEx(file, &fileSize) && ReadAt(file, 0, riff, sizeof(riff)) &&
        riff[0] == FourCC('R', 'I', 'F', 'F') &&
        riff[2] == FourCC('A', 'C', 'O', 'N')) {
        const ULONGLONG end = static_cast<ULONGLONG>(fileSize.QuadPart);
        ULONGLONG offset = 12;
        DWORD chunk[2];
        while (offset + 8 <= end && ReadAt(file, offset, chunk, sizeof(chunk))) {
            const ULONGLONG data = offset + 8;
            const ULONGLONG chunkEnd = data + chunk[1];
            if (chunkEnd > end) {
                break;
            }

            if (chunk[0] == FourCC('a', 'n', 'i', 'h') &&
                chunk[1] >= sizeof(header)) {
                haveHeader = ReadAt(file, data, header, sizeof(header));
            } else if ((chunk[0] == FourCC('r', 'a', 't', 'e') ||
                        chunk[0] == FourCC('s', 'e', 'q', ' ')) &&
                       chunk[1] / 4 <= kMaxAniFrames) {
                std::vector<DWORD>& table =
                    chunk[0] == FourCC('r', 'a', 't', 'e') ? rates : sequence;
                table.resize(chunk[1] / 4);
                if (!table.empty() &&
                    !ReadAt(file, data, table.data(),
                            static_cast<DWORD>(table.size() * 4))) {
                    table.clear();
                }
            } else if (chunk[0] == FourCC('L', 'I', 'S', 'T') && chunk[1] >= 4) {
                DWORD listType;
                if (ReadAt(file, data, &listType, sizeof(listType)) &&
                    listType == FourCC('f', 'r', 'a', 'm')) {
                    ULONGLONG inner = data + 4;
                    DWORD icon[2];
                    while (inner + 8 <= chunkEnd &&
                           index.frameOffsets.size() < kMaxAniFrames &&
                           ReadAt(file, inner, icon, sizeof(icon))) {
                        if (inner + 8 + icon[1] > chunkEnd) {
                            break;
                        }
                        if (icon[0] == FourCC('i', 'c', 'o', 'n') &&
                            icon[1] >= 6 && icon[1] <= 16 * 1024 * 1024) {
                            index.frameOffsets.push_back(inner + 8);
                            index.frameSizes.push_back(icon[1]);
                        }
                        inner += 8ULL + icon[1] + (icon[1] & 1);
                    }
                }
            }

            offset = chunkEnd + (chunk[1] & 1);
        }
    }
    CloseHandle(file);

    const size_t frames = index.frameOffsets.size();
    if (!haveHeader || frames == 0) {
        index = {};
        return false;
    }

    // Without a "seq " chunk, step i shows frame i. Without a "rate" chunk,
    // every step lasts the header's default rate.
    size_t steps = header[2] ? header[2] : frames;
    steps = std::min<size_t>(steps, kMaxAniFrames);
    index.stepFrames.resize(steps);
    index.stepJiffies.resize(steps);
    for (size_t i = 0; i < steps; i++) {
        const DWORD frame = i < sequence.size() ? sequence[i] : static_cast<DWORD>(i);
        index.stepFrames[i] = frame < frames ? frame : static_cast<DWORD>(i % frames);
        const DWORD jiffies = i < rates.size() ? rates[i] : header[7];
        index.stepJiffies[i] = std::clamp<DWORD>(jiffies, 1, 600);
        index.totalJiffies += index.stepJiffies[i];
    }
    return true;
}

// The step that's due `elapsedMs` after the animation started, looping.
size_t AniStepAt(const AniIndex& index, ULONGLONG elapsedMs) {
    ULONGLONG jiffy = (elapsedMs * 60 / 1000) % index.totalJiffies;
    for (size_t i = 0; i < index.stepJiffies.size(); i++) {
        if (jiffy < index.stepJiffies[i]) {
            return i;
        }
        jiffy -= index.stepJiffies[i];
    }
    return index.stepJiffies.size() - 1;
}

// The frame that's due `elapsedMs` after the animation started, looping.
size_t AniFrameAt(const AniIndex& index, ULONGLONG elapsedMs) {
    return index.stepFrames[AniStepAt(index, elapsedMs)];
}

// Reads one frame's .cur data.
bool ReadAniFrame(PCWSTR path, const AniIndex& index, size_t frameIndex,
                  std::vector<BYTE>& frame) {
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    frame.resize(index.frameSizes[frameIndex]);
    const bool ok = ReadAt(file, index.frameOffsets[frameIndex], frame.data(),
                           index.frameSizes[frameIndex]);
    CloseHandle(file);
    if (!ok) {
        frame.clear();
    }
    return ok;
}

// Validates the .cur/.ico data of a frame (the frame format inside .ani files)
// before any of its images are used.
bool IsValidIconFile(const std::vector<BYTE>& file) {
    if (file.size() < 6) {
        return false;
    }
    const WORD type = static_cast<WORD>(file[2] | file[3] << 8);
    const WORD count = static_cast<WORD>(file[4] | file[5] << 8);
    return (type == 1 || type == 2) && count > 0 &&
           file.size() >= 6 + 16 * static_cast<size_t>(count);
}

// Picks the image to scale from, like LoadImage does for the full cursor: the
// smallest one at least `size` wide, or the largest one if none is that big.
// Returns -1 if the file has no usable image.
int PickIconEntry(const std::vector<BYTE>& file, int size) {
    auto readDword = [&](size_t at) {
        return static_cast<DWORD>(file[at]) |
               static_cast<DWORD>(file[at + 1]) << 8 |
               static_cast<DWORD>(file[at + 2]) << 16 |
               static_cast<DWORD>(file[at + 3]) << 24;
    };

    const int count = file[4] | file[5] << 8;
    int fitting = -1;
    int fittingWidth = 0;
    int largest = -1;
    int largestWidth = 0;
    for (int i = 0; i < count; i++) {
        const size_t entry = 6 + 16 * static_cast<size_t>(i);
        const int width = file[entry] ? file[entry] : 256;
        const DWORD bytes = readDword(entry + 8);
        const DWORD offset = readDword(entry + 12);
        if (bytes == 0 || offset > file.size() || bytes > file.size() - offset) {
            continue;
        }
        if (width > largestWidth) {
            largest = i;
            largestWidth = width;
        }
        if (width >= size && (fitting < 0 || width < fittingWidth)) {
            fitting = i;
            fittingWidth = width;
        }
    }
    return fitting >= 0 ? fitting : largest;
}

// Prepares one image of the frame for CreateIconFromResourceEx: its hotspot as
// two WORDs, followed by the image data.
void PrepareIconEntry(StaticFrame& frame, int index) {
    const std::vector<BYTE>& file = frame.file;
    auto readWord = [&](size_t at) {
        return static_cast<WORD>(file[at] | file[at + 1] << 8);
    };
    auto readDword = [&](size_t at) {
        return static_cast<DWORD>(readWord(at)) |
               static_cast<DWORD>(readWord(at + 2)) << 16;
    };

    const bool isCursor = readWord(2) == 2;
    const size_t entry = 6 + 16 * static_cast<size_t>(index);
    const DWORD bytes = readDword(entry + 8);
    const DWORD offset = readDword(entry + 12);

    frame.width = file[entry] ? file[entry] : 256;
    frame.height = file[entry + 1] ? file[entry + 1] : 256;
    // In .cur files these two fields are the hotspot; .ico frames have none.
    frame.hotspotX = isCursor ? readWord(entry + 4) : 0;
    frame.hotspotY = isCursor ? readWord(entry + 6) : 0;

    frame.resource.resize(4 + bytes);
    frame.resource[0] = static_cast<BYTE>(frame.hotspotX);
    frame.resource[1] = static_cast<BYTE>(frame.hotspotX >> 8);
    frame.resource[2] = static_cast<BYTE>(frame.hotspotY);
    frame.resource[3] = static_cast<BYTE>(frame.hotspotY >> 8);
    std::copy(file.begin() + offset, file.begin() + offset + bytes,
              frame.resource.begin() + 4);
    frame.preparedEntry = index;
}

// Creates a static cursor from the frame at the given size, with the hotspot
// scaled to match.
HCURSOR CreateStaticFrameCursor(StaticFrame& frame, int size) {
    const int index = PickIconEntry(frame.file, size);
    if (index < 0) {
        return nullptr;
    }
    if (index != frame.preparedEntry) {
        PrepareIconEntry(frame, index);
    }

    HCURSOR scaled = reinterpret_cast<HCURSOR>(CreateIconFromResourceEx(
        frame.resource.data(), static_cast<DWORD>(frame.resource.size()),
        FALSE, 0x00030000, size, size, LR_DEFAULTCOLOR));
    if (!scaled) {
        return nullptr;
    }

    // Set the hotspot explicitly instead of relying on how
    // CreateIconFromResourceEx scales it, so the tip stays exactly in place.
    ICONINFO info{};
    HCURSOR cursor = nullptr;
    if (GetIconInfo(scaled, &info)) {
        info.fIcon = FALSE;
        info.xHotspot = static_cast<DWORD>(
            std::lround(static_cast<double>(frame.hotspotX) * size / frame.width));
        info.yHotspot = static_cast<DWORD>(
            std::lround(static_cast<double>(frame.hotspotY) * size / frame.height));
        cursor = reinterpret_cast<HCURSOR>(CreateIconIndirect(&info));
        if (info.hbmColor) {
            DeleteObject(info.hbmColor);
        }
        if (info.hbmMask) {
            DeleteObject(info.hbmMask);
        }
    }

    if (cursor) {
        DestroyCursor(scaled);
        return cursor;
    }
    return scaled;
}

// pristineFallback, if given, is an owned copy of this cursor type's original
// image, taken before any enlargement. It's the only safe fallback source:
// the shared system cursor handle returned by LoadCursorW may already be
// showing a previous frame's enlarged image by the time this runs.
HCURSOR LoadCursorAtSize(const CursorType& type,
                         PCWSTR schemePath,
                         int size,
                         HCURSOR pristineFallback) {
    HCURSOR cursor = nullptr;

    // 1. The scheme's own file. LoadImage picks the frame closest to the
    //    requested size, so multi-size .cur files stay sharp.
    if (schemePath) {
        cursor = static_cast<HCURSOR>(LoadImageW(
            nullptr, schemePath, IMAGE_CURSOR, size, size, LR_LOADFROMFILE));
    }

    // 2. The built-in cursor, for entries the scheme leaves empty.
    if (!cursor) {
        if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
            cursor = static_cast<HCURSOR>(LoadImageW(user32,
                                                     MAKEINTRESOURCEW(type.id),
                                                     IMAGE_CURSOR, size, size,
                                                     LR_DEFAULTCOLOR));
        }
    }

    // 3. Last resort: stretch the pristine copy taken before enlargement.
    if (!cursor && pristineFallback) {
        cursor = static_cast<HCURSOR>(
            CopyImage(pristineFallback, IMAGE_CURSOR, size, size, 0));
    }

    return cursor;
}

void RestoreSystemCursors() {
    // Reloads every system cursor from the registry, exactly like applying a
    // scheme in Mouse Properties.
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_SetIntValue(L"cursorsModified", 0);
}

// Size of the normal arrow as currently loaded (includes DPI scaling and the
// Accessibility pointer size setting).
int GetNormalCursorSize() {
    int size = 0;

    ICONINFO info{};
    if (HCURSOR arrow = LoadCursorW(nullptr, IDC_ARROW);
        arrow && GetIconInfo(arrow, &info)) {
        BITMAP bitmap{};
        HBITMAP measured = info.hbmColor ? info.hbmColor : info.hbmMask;
        if (measured && GetObjectW(measured, sizeof(bitmap), &bitmap)) {
            size = bitmap.bmWidth;
        }

        if (info.hbmColor) {
            DeleteObject(info.hbmColor);
        }
        if (info.hbmMask) {
            DeleteObject(info.hbmMask);
        }
    }

    if (size <= 0) {
        size = GetSystemMetrics(SM_CXCURSOR);
    }

    return size > 0 ? size : 32;
}

////////////////////////////////////////////////////////////////////////////////
// Shake detection

struct Sample {
    ULONGLONG time;
    LONG x;
    LONG y;
    double segment;  // Distance from the previous sample.
};

std::deque<Sample> g_samples;
double g_pathLength;

// Samples closer together than this are merged, which keeps the history small
// even with 1000+ Hz mice.
constexpr ULONGLONG kSampleSpacingMs = 4;

ULONGLONG NowMs() {
    static const LONGLONG frequency = [] {
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return value.QuadPart;
    }();

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<ULONGLONG>(counter.QuadPart / (frequency / 1000));
}

double Distance(LONG x1, LONG y1, LONG x2, LONG y2) {
    return std::hypot(static_cast<double>(x2 - x1),
                      static_cast<double>(y2 - y1));
}

void ClearSamples() {
    g_samples.clear();
    g_pathLength = 0;
}

void AddSample(ULONGLONG now, POINT pt) {
    if (g_samples.size() >= 2 &&
        now - g_samples.back().time < kSampleSpacingMs) {
        Sample& last = g_samples.back();
        const Sample& previous = g_samples[g_samples.size() - 2];
        g_pathLength -= last.segment;
        last.x = pt.x;
        last.y = pt.y;
        last.segment = Distance(previous.x, previous.y, pt.x, pt.y);
        g_pathLength += last.segment;
    } else {
        const double segment =
            g_samples.empty() ? 0.0
                              : Distance(g_samples.back().x,
                                         g_samples.back().y, pt.x, pt.y);
        g_samples.push_back({now, pt.x, pt.y, segment});
        g_pathLength += segment;
    }

    // Forget movement older than the time window.
    const ULONGLONG window = g_settings.shakeWindowMs;
    while (g_samples.size() > 1 && now - g_samples.front().time > window) {
        g_samples.pop_front();
        // The segment leading into the new oldest sample is no longer inside
        // the window.
        g_pathLength -= g_samples.front().segment;
        g_samples.front().segment = 0;
    }

    if (g_samples.size() <= 1) {
        g_pathLength = 0;  // Also resets any floating-point drift.
    }
}

// A shake is a long path that stays inside a small area: the path length has
// to be several times the diagonal of the area's bounding box.
bool IsShaking() {
    if (g_samples.size() < 3 || g_pathLength < g_settings.shakeDistance) {
        return false;
    }

    LONG minX = g_samples.front().x;
    LONG maxX = minX;
    LONG minY = g_samples.front().y;
    LONG maxY = minY;
    for (const Sample& sample : g_samples) {
        minX = std::min(minX, sample.x);
        maxX = std::max(maxX, sample.x);
        minY = std::min(minY, sample.y);
        maxY = std::max(maxY, sample.y);
    }

    const double diagonal = Distance(minX, minY, maxX, maxY);
    return diagonal >= 1.0 &&
           g_pathLength * 100.0 >= diagonal * g_settings.shakeRatioPercent;
}

////////////////////////////////////////////////////////////////////////////////
// Grow / hold / shrink animation

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

enum class Phase { Idle, Growing, Enlarged, Shrinking };

constexpr size_t kCursorTypeCount = sizeof(kCursorTypes) / sizeof(kCursorTypes[0]);
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;

// Pauses longer than this don't count towards the time spent shaking.
constexpr ULONGLONG kShakeGapMs = 200;

// Per-type state, captured when an animation starts so that frames don't have
// to touch the registry.
struct CursorSource {
    HCURSOR systemHandle;  // Unchanged when SetSystemCursor swaps the image.
    HCURSOR pristineCopy;  // Owned copy of the original image; see LoadCursorAtSize.
    WCHAR path[MAX_PATH];
    bool hasPath;
    bool animated;
    int appliedSize;
    bool appliedStatic;     // The applied cursor is a static stand-in frame.
    int appliedFrame;       // Which .ani frame the stand-in shows.
    bool standInTried;      // The .ani index is read lazily, when first needed.
    bool standInReady;
    AniIndex aniIndex;
    ULONGLONG aniStartMs;   // When the stand-in's animation clock started.
    int loadedFrame;        // Which frame `staticFrame` holds (-1 = none).
    StaticFrame staticFrame;
    size_t lastStep;        // Stand-in step at the last loop-start check.
};

CursorSource g_sources[kCursorTypeCount];

HWND g_hwnd;
HANDLE g_timer;  // Drives animation frames and the hold check.
LONGLONG g_frameInterval100ns = 10'000'000 / 60;
bool g_animating;  // Timer runs frames rather than waiting to shrink.

Phase g_phase = Phase::Idle;
int g_normalSize;
int g_enlargedSize;  // Size of the first enlargement.
int g_sizeCeiling;
double g_currentSize;
double g_tweenFrom;
double g_tweenTo;
ULONGLONG g_tweenStart;
ULONGLONG g_tweenDuration;
ULONGLONG g_lastShakeTime;
ULONGLONG g_shakeTimeMs;  // Time spent shaking since the cursor started growing.
ULONGLONG g_shrinkDoneMs; // When the shrink reached the normal size (0 = not yet).

// Longest the restore waits for an animated stand-in's loop to come around.
constexpr ULONGLONG kMaxLoopWaitMs = 5000;

void CaptureCursorSources(int normalSize) {
    for (size_t i = 0; i < kCursorTypeCount; i++) {
        const CursorType& type = kCursorTypes[i];
        CursorSource& source = g_sources[i];
        source.systemHandle = LoadCursorW(nullptr, MAKEINTRESOURCEW(type.id));
        // Taken now, before SetCursorTypeSize ever runs for this type, so it's
        // guaranteed to still be the original image.
        source.pristineCopy =
            source.systemHandle
                ? static_cast<HCURSOR>(
                      CopyImage(source.systemHandle, IMAGE_CURSOR, 0, 0, 0))
                : nullptr;
        source.hasPath = GetSchemeCursorPath(type.registryName, source.path);
        source.animated = source.hasPath ? IsAnimatedCursorFile(source.path)
                                         : type.animatedByDefault;
        source.appliedSize = normalSize;
        source.appliedStatic = false;
        source.appliedFrame = -1;
        source.standInTried = false;
        source.standInReady = false;
        source.aniIndex = {};
        source.loadedFrame = -1;
        source.staticFrame = {};
        source.lastStep = 0;
    }
}

// The frame the stand-in for an animated cursor should show right now. The
// stand-in keeps the animation running while the size changes: each step shows
// the frame that's due at that moment, so only one frame is decoded per step.
// Returns 0 if the .ani file can't be indexed (e.g. it fails to parse); the
// stand-in is then a still image.
int StandInFrameNow(CursorSource& source) {
    if (!source.standInTried) {
        source.standInTried = true;
        source.standInReady = ReadAniIndex(source.path, source.aniIndex);
        source.aniStartMs = NowMs();
        source.loadedFrame = -1;
    }
    if (!source.standInReady) {
        return 0;
    }
    return static_cast<int>(
        AniFrameAt(source.aniIndex, NowMs() - source.aniStartMs));
}

// Whether the stand-in's animation is back at the start of its loop: at its
// first step, or wrapped around since the last check (a step can be shorter
// than the time between checks).
bool StandInAtLoopStart(CursorSource& source) {
    const size_t step =
        AniStepAt(source.aniIndex, NowMs() - source.aniStartMs);
    const bool atStart = step == 0 || step < source.lastStep;
    source.lastStep = step;
    return atStart;
}

void ResetLoopStartCheck(CursorSource& source) {
    if (source.standInReady) {
        source.lastStep =
            AniStepAt(source.aniIndex, NowMs() - source.aniStartMs);
    }
}

// A static stand-in for an animated cursor at the given size: the given frame
// of its .ani file, or the pristine copy stretched if that can't be read.
HCURSOR LoadStandInAtSize(CursorSource& source, int size, int frameIndex) {
    HCURSOR cursor = nullptr;
    if (source.standInReady) {
        if (source.loadedFrame != frameIndex) {
            source.loadedFrame = -1;
            source.staticFrame = {};
            if (ReadAniFrame(source.path, source.aniIndex,
                             static_cast<size_t>(frameIndex),
                             source.staticFrame.file) &&
                IsValidIconFile(source.staticFrame.file)) {
                source.loadedFrame = frameIndex;
            }
        }
        if (source.loadedFrame == frameIndex) {
            cursor = CreateStaticFrameCursor(source.staticFrame, size);
        }
    }
    if (!cursor && source.pristineCopy) {
        cursor = static_cast<HCURSOR>(
            CopyImage(source.pristineCopy, IMAGE_CURSOR, size, size, 0));
    }
    return cursor;
}

// Animated cursors are never loaded in full while the cursor is enlarged:
// that decodes every frame of the .ani file (60+ in some themes), which takes
// long enough to stall the animation and holds up the system's cursor
// handling. Instead they get a single-frame stand-in that this mod animates
// itself, advancing it to the frame that's due on every tick, so only one
// frame is decoded per frame change. The real animated cursors come back when
// the cursors are restored at the end (see FinishShrinkOnLoopStart).
// The built-in animated cursors (no file in the scheme) are small resources
// that load quickly, so they're loaded in full like static ones.
bool UsesStandIn(const CursorSource& source) {
    return source.animated && source.hasPath;
}

void SetCursorTypeSize(size_t index, int size) {
    CursorSource& source = g_sources[index];

    if (!UsesStandIn(source)) {
        if (source.appliedSize == size) {
            return;
        }
        HCURSOR cursor =
            LoadCursorAtSize(kCursorTypes[index],
                             source.hasPath ? source.path : nullptr, size,
                             source.pristineCopy);
        if (!cursor) {
            return;
        }
        // On success, the system takes ownership of the handle and destroys
        // it.
        if (SetSystemCursor(cursor, kCursorTypes[index].id)) {
            source.appliedSize = size;
        } else {
            DestroyCursor(cursor);
        }
        return;
    }

    // The original full cursor still animates on its own at its normal size.
    if (source.appliedSize == size && !source.appliedStatic) {
        return;
    }

    const int frame = StandInFrameNow(source);
    if (source.appliedStatic && source.appliedSize == size &&
        source.appliedFrame == frame) {
        return;  // Already showing this frame at this size.
    }

    HCURSOR cursor = LoadStandInAtSize(source, size, frame);
    if (!cursor) {
        return;
    }
    if (SetSystemCursor(cursor, kCursorTypes[index].id)) {
        source.appliedSize = size;
        source.appliedStatic = true;
        source.appliedFrame = frame;
    } else {
        DestroyCursor(cursor);
    }
}

// Resizing only the cursor that's on screen means one file load per frame
// instead of seventeen, which is what makes a smooth animation possible. For
// an animated cursor this also advances it to the frame that's due.
void ResizeVisibleCursor(int size) {
    CURSORINFO info{.cbSize = sizeof(info)};
    if (!GetCursorInfo(&info) || !info.hCursor) {
        return;
    }

    for (size_t i = 0; i < kCursorTypeCount; i++) {
        if (g_sources[i].systemHandle == info.hCursor) {
            SetCursorTypeSize(i, size);
            return;
        }
    }
}

// Static cursors are cheap, so they're all brought to size up front. Animated
// ones are left alone until they're on screen (ResizeVisibleCursor catches
// them up within a tick), so that nothing slow runs while the visible
// cursor is animating.
void ResizeAllStaticCursors(int size) {
    for (size_t i = 0; i < kCursorTypeCount; i++) {
        if (!UsesStandIn(g_sources[i])) {
            SetCursorTypeSize(i, size);
        }
    }
}

bool HasAnimatedCursors() {
    for (const CursorSource& source : g_sources) {
        if (UsesStandIn(source)) {
            return true;
        }
    }
    return false;
}

void ScheduleTimer(LONGLONG delay100ns) {
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -std::max<LONGLONG>(delay100ns, 1);  // Relative time.
    SetWaitableTimer(g_timer, &dueTime, 0, nullptr, nullptr, FALSE);
}

void ScheduleHoldCheck(ULONGLONG now) {
    const ULONGLONG holdMs = g_settings.holdMs;
    const ULONGLONG elapsed = now - g_lastShakeTime;
    const ULONGLONG remaining = elapsed < holdMs ? holdMs - elapsed : 0;
    ScheduleTimer(static_cast<LONGLONG>(remaining) * 10'000);
}

struct MonitorMetrics {
    LONGLONG frameInterval100ns;  // One frame per refresh.
    int height;
};

// Refresh rate and height of the monitor the cursor is on. The height is in
// cursor-bitmap pixels, not display pixels: Windows re-scales a loaded system
// cursor again by the monitor's DPI when it draws it, so a bitmap loaded at
// the monitor's raw pixel height would end up displaying larger than the
// screen on a scaled-up monitor. Dividing by the DPI factor here keeps the
// size limit accurate to what's actually shown on screen.
MonitorMetrics GetCursorMonitorMetrics() {
    int refreshRate = 60;
    int height = 0;
    double dpiScale = 1.0;

    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

        MONITORINFOEXW monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor,
                            reinterpret_cast<LPMONITORINFO>(&monitorInfo))) {
            height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

            DEVMODEW mode{};
            mode.dmSize = sizeof(mode);
            if (EnumDisplaySettingsW(monitorInfo.szDevice,
                                     ENUM_CURRENT_SETTINGS, &mode) &&
                mode.dmDisplayFrequency > 1) {
                refreshRate = static_cast<int>(mode.dmDisplayFrequency);
            }
        }

        UINT dpiX, dpiY;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                       &dpiY)) &&
            dpiX > 0) {
            dpiScale = dpiX / 96.0;
        }
    }

    refreshRate = std::clamp(refreshRate, 30, 360);
    const int rawHeight = height > 0 ? height : 1080;
    return {10'000'000LL / refreshRate,
           static_cast<int>(std::lround(rawHeight / dpiScale))};
}

double EaseOutCubic(double t) {
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

// The first enlargement plus extra growth while you keep shaking. The extra
// part is logarithmic: every further step of the same size takes twice as long
// as the previous one, so it keeps slowing down but never quite stops.
double TargetSize() {
    const double extra = g_settings.keepGrowingPercent / 100.0 *
                         std::log2(1.0 + g_shakeTimeMs / 1000.0);
    return std::min(g_enlargedSize * (1.0 + extra),
                    static_cast<double>(g_sizeCeiling));
}

// Animates from the current size to `to`. The duration is for the full normal
// <-> enlarged distance, so reversing halfway through takes half as long.
void StartTween(double to, int fullDurationMs, ULONGLONG now) {
    const double span = std::max(1, g_enlargedSize - g_normalSize);
    const double fraction = std::min(1.0, std::abs(to - g_currentSize) / span);
    g_shrinkDoneMs = 0;
    g_tweenFrom = g_currentSize;
    g_tweenTo = to;
    g_tweenStart = now;
    g_tweenDuration =
        static_cast<ULONGLONG>(std::lround(fullDurationMs * fraction));
}

void GoIdle() {
    CancelWaitableTimer(g_timer);
    RestoreSystemCursors();  // Exact original cursors, all types.
    for (CursorSource& source : g_sources) {
        if (source.pristineCopy) {
            DestroyCursor(source.pristineCopy);
            source.pristineCopy = nullptr;
        }
        source.staticFrame = {};  // Frees the frame's memory.
        source.aniIndex = {};
        source.standInTried = false;
        source.standInReady = false;
        source.loadedFrame = -1;
    }
    g_phase = Phase::Idle;
    g_animating = false;
    g_shakeTimeMs = 0;
}

void ResetToIdle() {
    if (g_phase != Phase::Idle) {
        GoIdle();
    }
}

// At the end of the shrink, the normal cursors are restored, and an animated
// one restarts its animation from the beginning. If the cursor on screen is an
// animated stand-in, keep it animating at the normal size until its loop comes
// back to the start, so the restore doesn't make the animation jump. Returns
// true once the cursors are restored.
bool FinishShrinkOnLoopStart(ULONGLONG now) {
    if (g_shrinkDoneMs == 0) {
        g_shrinkDoneMs = now;
        for (CursorSource& source : g_sources) {
            ResetLoopStartCheck(source);
        }
    }

    CURSORINFO info{.cbSize = sizeof(info)};
    if (GetCursorInfo(&info) && info.hCursor) {
        for (size_t i = 0; i < kCursorTypeCount; i++) {
            CursorSource& source = g_sources[i];
            if (source.systemHandle != info.hCursor) {
                continue;
            }
            if (source.appliedStatic && source.standInReady &&
                now - g_shrinkDoneMs < kMaxLoopWaitMs &&
                !StandInAtLoopStart(source)) {
                // The shrink only resized the visible cursor, so the other
                // types are still enlarged. Bring them back now, so one that
                // appears while waiting (e.g. the arrow when a busy app
                // finishes) isn't shown big. Each type is resized once; after
                // that it's skipped.
                for (size_t j = 0; j < kCursorTypeCount; j++) {
                    if (j != i && g_sources[j].appliedSize != g_normalSize) {
                        SetCursorTypeSize(j, g_normalSize);
                    }
                }
                SetCursorTypeSize(i, g_normalSize);  // Next frame, normal size.
                return false;
            }
            break;
        }
    }

    GoIdle();
    return true;
}

void RenderTweenFrame(ULONGLONG now) {
    if (g_phase == Phase::Growing) {
        g_tweenTo = TargetSize();  // Moves on while you keep shaking.
    }

    const double t =
        g_tweenDuration == 0
            ? 1.0
            : std::min(1.0, static_cast<double>(now - g_tweenStart) /
                                g_tweenDuration);
    g_currentSize = g_tweenFrom + (g_tweenTo - g_tweenFrom) * EaseOutCubic(t);

    if (t >= 1.0) {
        if (g_phase == Phase::Growing) {
            // Once the visible cursor is there, the static cursor types catch
            // up too.
            const int size = static_cast<int>(std::lround(g_currentSize));
            ResizeVisibleCursor(size);
            ResizeAllStaticCursors(size);
            g_phase = Phase::Enlarged;
            ScheduleTimer(g_frameInterval100ns);
        } else if (!FinishShrinkOnLoopStart(now)) {
            ScheduleTimer(g_frameInterval100ns);  // Still waiting.
        }
        return;
    }

    ResizeVisibleCursor(static_cast<int>(std::lround(g_currentSize)));
    ScheduleTimer(g_frameInterval100ns);
}

void StartShrinking(ULONGLONG now) {
    g_phase = Phase::Shrinking;
    g_animating = true;
    StartTween(g_normalSize, g_settings.shrinkMs, now);
    RenderTweenFrame(now);
}

// While waiting to shrink: with animated cursors in the scheme, tick every
// frame so they keep animating; otherwise just wake up for the hold check.
void ScheduleEnlargedWait(ULONGLONG now) {
    if (HasAnimatedCursors()) {
        ScheduleTimer(g_frameInterval100ns);  // The hold is checked each tick.
    } else {
        ScheduleHoldCheck(now);
    }
}

void OnTimerFired() {
    const ULONGLONG now = NowMs();
    switch (g_phase) {
        case Phase::Growing:
        case Phase::Shrinking:
            RenderTweenFrame(now);
            break;

        case Phase::Enlarged:
            if (!g_animating) {
                // Hold check: shrink once you've stopped shaking long enough.
                if (now - g_lastShakeTime >=
                    static_cast<ULONGLONG>(g_settings.holdMs)) {
                    StartShrinking(now);
                } else {
                    // Keeps an animated cursor on screen animating, and
                    // catches up a cursor that just appeared (e.g. the busy
                    // cursor when an app starts loading while the mouse is
                    // still).
                    ResizeVisibleCursor(
                        static_cast<int>(std::lround(g_currentSize)));
                    ScheduleEnlargedWait(now);
                }
            } else if (g_settings.keepGrowingPercent > 0 &&
                       now - g_lastShakeTime <= kShakeGapMs) {
                // Still shaking: keep growing.
                g_currentSize = TargetSize();
                ResizeVisibleCursor(
                    static_cast<int>(std::lround(g_currentSize)));
                ScheduleTimer(g_frameInterval100ns);
            } else {
                // Stopped shaking: wait for the hold time, then shrink.
                g_animating = false;
                ResizeVisibleCursor(static_cast<int>(std::lround(g_currentSize)));
                ScheduleEnlargedWait(now);
            }
            break;

        case Phase::Idle:
            break;
    }
}

bool CanEnlargeNow() {
    // Skip when the cursor is hidden (games, video playback) or suppressed
    // (touch and pen input).
    CURSORINFO info{.cbSize = sizeof(info)};
    if (!GetCursorInfo(&info) || !(info.flags & CURSOR_SHOWING) ||
        (info.flags & CURSOR_SUPPRESSED)) {
        return false;
    }

    if (g_settings.disableInFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
            (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
             state == QUNS_PRESENTATION_MODE)) {
            return false;
        }
    }

    return true;
}

void OnShake(ULONGLONG now) {
    switch (g_phase) {
        case Phase::Idle: {
            if (!CanEnlargeNow()) {
                // Require a fresh shake instead of re-checking on every event.
                ClearSamples();
                return;
            }

            const MonitorMetrics monitor = GetCursorMonitorMetrics();
            g_frameInterval100ns = monitor.frameInterval100ns;
            g_sizeCeiling =
                g_settings.sizeLimit > 0 ? g_settings.sizeLimit : monitor.height;
            g_normalSize = GetNormalCursorSize();
            g_enlargedSize =
                std::min(g_normalSize * g_settings.scalePercent / 100,
                         g_sizeCeiling);
            if (g_enlargedSize <= g_normalSize) {
                ClearSamples();
                return;
            }

            Wh_Log(L"Shake detected, enlarging cursor %d -> %d px",
                   g_normalSize, g_enlargedSize);

            // Lets the next start restore the cursors if this process dies
            // while they're enlarged.
            Wh_SetIntValue(L"cursorsModified", 1);

            CaptureCursorSources(g_normalSize);
            g_shakeTimeMs = 0;
            g_lastShakeTime = now;
            g_currentSize = g_normalSize;
            g_phase = Phase::Growing;
            g_animating = true;
            StartTween(TargetSize(), g_settings.growMs, now);
            RenderTweenFrame(now);
            break;
        }

        case Phase::Shrinking:
            // Shaking again while shrinking: grow back to where it was.
            g_lastShakeTime = now;
            g_phase = Phase::Growing;
            StartTween(TargetSize(), g_settings.growMs, now);
            break;

        case Phase::Growing:
        case Phase::Enlarged:
            if (now - g_lastShakeTime <= kShakeGapMs) {
                g_shakeTimeMs += now - g_lastShakeTime;
            }
            g_lastShakeTime = now;

            if (!g_animating) {
                // Was waiting to shrink: start growing again right away.
                g_animating = true;
                ScheduleTimer(1);
            }
            break;
    }
}

void OnRawMouseInput() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    // Clicks, wheel and sub-pixel movement don't move the cursor.
    if (!g_samples.empty() && g_samples.back().x == pt.x &&
        g_samples.back().y == pt.y) {
        return;
    }

    const ULONGLONG now = NowMs();
    AddSample(now, pt);
    if (IsShaking()) {
        OnShake(now);
    }

    // While it's waiting to shrink, a cursor that just appeared (e.g. the text
    // cursor when moving over text) catches up with the current size.
    if (g_phase == Phase::Enlarged && !g_animating) {
        ResizeVisibleCursor(static_cast<int>(std::lround(g_currentSize)));
    }
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread with a message-only window

constexpr WCHAR kWindowClassName[] = L"WindhawkShakeToFindCursor_" WH_MOD_ID;

HANDLE g_workerThread;
HANDLE g_workerReadyEvent;
LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter;

LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT msg,
                            WPARAM wParam,
                            LPARAM lParam) {
    switch (msg) {
        case WM_INPUT:
            OnRawMouseInput();
            break;  // DefWindowProc still has to run to clean up the input.

        case WM_APP_SETTINGS_CHANGED: {
            std::unique_ptr<Settings> settings(
                reinterpret_cast<Settings*>(lParam));
            ResetToIdle();
            g_settings = *settings;
            ClearSamples();
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY: {
            ResetToIdle();
            RAWINPUTDEVICE device{
                .usUsagePage = 0x01,  // HID_USAGE_PAGE_GENERIC
                .usUsage = 0x02,      // HID_USAGE_GENERIC_MOUSE
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = nullptr,
            };
            RegisterRawInputDevices(&device, 1, sizeof(device));
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Work in physical pixels regardless of the host process's DPI awareness.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{
        .cbSize = sizeof(windowClass),
        .lpfnWndProc = WindowProc,
        .hInstance = instance,
        .lpszClassName = kWindowClassName,
    };
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, kWindowClassName, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    // A high-resolution waitable timer paces the animation far more precisely
    // than WM_TIMER, which is limited to ~15.6 ms steps.
    g_timer = CreateWaitableTimerExW(nullptr, nullptr,
                                     CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                     TIMER_ALL_ACCESS);
    if (!g_timer) {
        g_timer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }
    if (!g_timer) {
        Wh_Log(L"CreateWaitableTimerExW failed: %u", GetLastError());
        DestroyWindow(hwnd);
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    // Raw input is event-driven (nothing runs while the mouse is idle) and,
    // unlike a low-level mouse hook, can never delay the mouse itself.
    RAWINPUTDEVICE device{
        .usUsagePage = 0x01,  // HID_USAGE_PAGE_GENERIC
        .usUsage = 0x02,      // HID_USAGE_GENERIC_MOUSE
        .dwFlags = RIDEV_INPUTSINK,
        .hwndTarget = hwnd,
    };
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
        DestroyWindow(hwnd);
        UnregisterClassW(kWindowClassName, instance);
        SetEvent(g_workerReadyEvent);
        return 1;
    }

    g_hwnd = hwnd;
    SetEvent(g_workerReadyEvent);

    // Message loop that also wakes up for the animation timer.
    bool running = true;
    while (running) {
        const DWORD result = MsgWaitForMultipleObjectsEx(
            1, &g_timer, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0) {
            OnTimerFired();
            continue;
        }

        if (result != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %u", GetLastError());
            break;
        }

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            DispatchMessageW(&msg);
        }
    }

    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);  // Only if the loop failed; restores the cursors.
    }

    CloseHandle(g_timer);
    g_timer = nullptr;
    UnregisterClassW(kWindowClassName, instance);
    return 0;
}

LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS* exceptionInfo) {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    return g_previousExceptionFilter
               ? g_previousExceptionFilter(exceptionInfo)
               : EXCEPTION_CONTINUE_SEARCH;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    g_settings = LoadSettings();

    // The previous run ended while the cursor was enlarged (crash or killed
    // process), so put the normal cursors back.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreSystemCursors();
    }

    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(RestoreCursorsOnCrash);

    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        return FALSE;
    }

    HANDLE handles[] = {g_workerReadyEvent, g_workerThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    return g_hwnd != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    // Settings are read here and handed over to the worker thread, which owns
    // all the state.
    auto settings = std::make_unique<Settings>(LoadSettings());
    if (g_hwnd && PostMessageW(g_hwnd, WM_APP_SETTINGS_CHANGED, 0,
                               reinterpret_cast<LPARAM>(settings.get()))) {
        settings.release();
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }

    // Safety net in case the worker thread couldn't clean up in time.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreSystemCursors();
    }

    SetUnhandledExceptionFilter(g_previousExceptionFilter);
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
