// ==WindhawkMod==
// @id              neko-cat
// @name            Desktop Companions
// @description     Spawn multiple animated characters (neko cat, sakura, and tomoyo) on your screen that interact with your windows and follow your cursor.
// @version         1.4.0
// @author          ciizerr
// @github          https://github.com/ciizerr
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lwinmm -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🐾 Desktop Companions
Cute animated characters that follow your mouse, run around your screen, and climb your windows!

![Mod Showcase](https://raw.githubusercontent.com/ciizerr/wh-mods/2c1ecbf9ba9d0964e1a764a090cb2b7df729dc5c/previews/Neko-cat.gif)

## 🏃 Available Characters

| Neko Cat (`neko-cat`) | Neko Dog (`neko-dog`) | Sakura (`sakura-icon`) | Tomoyo (`tomoyo-icon`) |
| :---: | :---: | :---: | :---: |
| ![Neko Cat](https://raw.githubusercontent.com/ciizerr/wh-mods/07dcad2878e35d697af51b42582e47c8e68a69ec/previews/Neko-mod/Neko-cat.gif) | ![Neko Dog](https://raw.githubusercontent.com/ciizerr/wh-mods/db8ca8c4e3247b01f1fbfb0aa8d1383d2211d13a/previews/Neko-mod/neko-dog.gif) | ![Sakura](https://raw.githubusercontent.com/ciizerr/wh-mods/07dcad2878e35d697af51b42582e47c8e68a69ec/previews/Neko-mod/sakura-icon.gif) | ![Tomoyo](https://raw.githubusercontent.com/ciizerr/wh-mods/07dcad2878e35d697af51b42582e47c8e68a69ec/previews/Neko-mod/tomoyo-icon.gif) |

---

## 🎮 How to Interact

*   **Left-Click**: Wakes the character up or cycles through all available movement behaviors.
*   **Right-Click**: Opens a context menu to instantly choose a specific movement behavior (and wakes them up).
*   **Drag & Drop**: Pick up your companion and throw/drop them anywhere! They'll recover, yawn, take a brief sleep cycle, and wake up.
*   **Window Nudging**: If sleeping on the floor, dragging a window boundary into them will gently slide them away and wake them up!

---

## 🏃 Movement Behaviors

| Behavior Mode | Visual Style | In-Game Mechanics |
| :--- | :--- | :--- |
| **Chase Mouse** (Default) | Targeted chase | Character follows your mouse cursor everywhere. |
| **Run Away** | Avoidance | Try to catch your companion! They escape if the mouse gets too close. |
| **Random** | Zoomies | Total random wandering and playing. |
| **Pace** | Border patrol | They walk along the bottom edge of your screens. |
| **Run Around** | Toy chase | The character plays with and chases an invisible bouncing ball. |
| **Play With Window** | Active scaling | The companion hunts down your focused window to climb the walls and walk the roof. |

---

## 🖥️ Multi-Monitor Support
Companions roam freely across **all your active monitors**! They seamlessly jump screen borders to track your mouse.

---

## 🔒 Optimization & Assets

*   **Low Footprint**: Written in pure native C++ with direct Win32/GDI+ calls. CPU usage is **virtually 0.0%** at 60 FPS.
*   **Secure External Downloads**: All graphic spritesheets and audio files are fetched dynamically on first initialization using Windhawk's secure HTTPS `Wh_GetUrlContent` API.
*   **Source Location**: Files are served directly from the official GitHub repository:
    *   **Repository Root**: [ciizerr/wh-mods on GitHub](https://github.com/ciizerr/wh-mods)
    *   **Secure Assets Path**:[Assets/](https://raw.githubusercontent.com/ciizerr/wh-mods/07dcad2878e35d697af51b42582e47c8e68a69ec/assets/)
*   **Local Storage**: Downloaded assets are safely cached locally in your Windhawk `modstorage` folder for offline use.

---

## 📜 Credits & License
*   **Original X-Windows Neko**: Masayuki Koba
*   **Windows Port (Neko 98)**: David Harvey - [SourceForge](https://sourceforge.net/projects/neko98/)
*   **JavaScript Port (nekojs)**: Louis Abraham - [GitHub](https://github.com/louisabraham/nekojs)
*   **Sakura & Tomoyo**: Icons by @zkyhzxf (quantum) on Discord.
*   **License**: MIT / freely usable as long as original credits are maintained.

---

## 💬 Feedback & Support
Got ideas or found a bug? I’d love to hear from you!
*   **Discord:** `ciizerr`
*   **GitHub:** [wh-mods](https://github.com/ciizerr/wh-mods)

Enjoy your new friends!
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- AppearanceGroup:
  - characterList:
    - - theme: "neko-cat"
        $name: Desktop Character
        $description: Choose your desktop character.
        $options:
          - neko-cat: Neko Cat
          - neko-dog: Neko Dog
          - sakura-icon: Sakura
          - tomoyo-icon: Tomoyo
    $name: Custom Character List
    $description: "Choose the characters you want to appear. The total number on screen is controlled by 'Number of Characters' below. It will cycle through this list to fill the count."
  - scale: 2
    $name: Character Size
    $description: Changes how big the character is on your screen.
  - random_themes: false
    $name: Randomize Multiple Characters
    $description: "If checked, the custom list will be ignored and random characters will be chosen up to the total number of characters."
  - character_count: 1
    $name: Number of Characters
    $description: "How many characters do you want? (Max recommended: 10)"
  $name: Appearance & Character

- BehaviorGroup:
  - speed: 24
    $name: Movement Speed
    $description: How fast the character runs to follow your mouse.
  - fps: 60
    $name: Fluidity (FPS)
    $description: Smoothness of movement. Use 30 to save battery.
  - save_behavior: true
    $name: Save Last Behavior
    $description: Remembers the last behavior when the mod or PC restarts.
  $name: Movement & Behavior

- AudioGroup:
  - sound: true
    $name: Enable Sound
    $description: Turn all character sounds (like meows, yawns, and snores) on or off.
  - sleep_sound_interval: 30
    $name: Sleep Sound Interval
    $description: How many seconds to wait between each sleep sound.
  - sleep_sound_repeat: true
    $name: Repeat Sleep Sound
    $description: If checked, the character will keep making sleep sounds while sleeping.
  $name: Sound & Audio

- AdvancedGroup:
  - neko_key: "Ctrl+Alt+N"
    $name: Pet Toggle Shortcut
    $description: Keyboard shortcut to quickly hide/unhide your pets. (e.g. Ctrl+Alt+N, Shift+Esc, empty to disable)
  $name: Advanced & Shortcuts
*/
// ==/WindhawkModSettings==


#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include <string>
#include <cmath>
#include <vector>
#include <cstdio>
#include <algorithm>

using namespace Gdiplus;

enum NekoState {
    STOP = 0, WASH, SCRATCH, YAWN, SLEEP, AWAKE,
    U_MOVE, D_MOVE, L_MOVE, R_MOVE, UL_MOVE, UR_MOVE, DL_MOVE, DR_MOVE,
    U_CLAW, D_CLAW, L_CLAW, R_CLAW, FALL, MAX_STATE
};

enum BehaviorMode {
    CHASE_MOUSE = 0, RUN_AWAY, RANDOM, PACE, RUN_AROUND, PLAY_WITH_WINDOW, FORCED_SLEEP, EXHAUSTED_SLEEP, MAX_BEHAVIOR
};

inline const wchar_t* GetBehaviorName(int mode) {
    switch (mode) {
        case CHASE_MOUSE:     return L"Chase Mouse";
        case RUN_AWAY:        return L"Run Away";
        case RANDOM:          return L"Random";
        case PACE:            return L"Pace";
        case RUN_AROUND:      return L"Run Around";
        case PLAY_WITH_WINDOW:return L"Play With Window";
        case FORCED_SLEEP:    return L"Forced Sleep";
        case EXHAUSTED_SLEEP: return L"Exhausted Sleep";
        default:              return L"Unknown";
    }
}

const int STOP_TIME = 4;
const int WASH_TIME = 10;
const int SCRATCH_TIME = 4;
const int YAWN_TIME = 3;
const int AWAKE_TIME = 3;
const int CLAW_TIME = 10;

const int SPRITE_SIZE = 32;

struct PetConfig {
    std::wstring theme;
};

std::vector<PetConfig> g_customPets;
const std::vector<std::wstring> g_officialThemes = { L"neko-cat", L"sakura-icon", L"tomoyo-icon", L"neko-dog" };

std::wstring g_storagePath = L"";
bool g_randomThemes = true;
int g_scale = 2;
int g_speed = 24;
bool g_soundEnabled = true;
int g_sleepSoundInterval = 30;
bool g_sleepSoundRepeat = true;
int g_fps = 60;
int g_catCount = 1;
bool g_saveBehavior = true;
static bool g_modExit = false;

// Forward declaration
void LoadSettings();

std::wstring g_nekoKeyStr = L"Ctrl+Alt+N";
bool g_isHidden = false;
#define WM_UPDATE_SETTINGS (WM_APP + 1)

UINT ParseHotkeyModifiers(const std::wstring& hotkeyStr) {
    UINT modifiers = MOD_NOREPEAT;
    std::wstring upperStr = hotkeyStr;
    for (auto& c : upperStr) c = towupper(c);
    
    if (upperStr.find(L"CTRL") != std::wstring::npos) modifiers |= MOD_CONTROL;
    if (upperStr.find(L"ALT") != std::wstring::npos) modifiers |= MOD_ALT;
    if (upperStr.find(L"SHIFT") != std::wstring::npos) modifiers |= MOD_SHIFT;
    if (upperStr.find(L"WIN") != std::wstring::npos) modifiers |= MOD_WIN;
    return modifiers;
}

UINT ParseHotkeyVK(const std::wstring& hotkeyStr) {
    std::wstring upperStr = hotkeyStr;
    for (auto& c : upperStr) c = towupper(c);
    
    size_t lastPlus = upperStr.find_last_of(L"+");
    std::wstring keyPart = (lastPlus != std::wstring::npos) ? upperStr.substr(lastPlus + 1) : upperStr;
    
    keyPart.erase(0, keyPart.find_first_not_of(L" "));
    keyPart.erase(keyPart.find_last_not_of(L" ") + 1);
    
    if (keyPart.length() == 1) return keyPart[0];
    if (keyPart == L"ESC" || keyPart == L"ESCAPE") return VK_ESCAPE;
    if (keyPart == L"SPACE") return VK_SPACE;
    if (keyPart == L"ENTER") return VK_RETURN;
    if (keyPart == L"TAB") return VK_TAB;
    if (keyPart == L"UP") return VK_UP;
    if (keyPart == L"DOWN") return VK_DOWN;
    if (keyPart == L"LEFT") return VK_LEFT;
    if (keyPart == L"RIGHT") return VK_RIGHT;
    if (keyPart.find(L"F") == 0 && keyPart.length() > 1) {
        int fNum = _wtoi(keyPart.substr(1).c_str());
        if (fNum >= 1 && fNum <= 24) return VK_F1 + fNum - 1;
    }
    return 0;
}

// Tool mod handles
static HWND   g_hwndOverlay = nullptr;
static HANDLE g_hThread = nullptr;

struct SpriteConfig {
    const wchar_t* files[2];
};

// Layout mapping specification for the single spritesheet.png.
// Each state maps to a row (0 to 18) in a 19x2 grid of 32x32 sprites.
SpriteConfig g_spriteConfigs[MAX_STATE] = {
    { L"awake.png", L"awake.png" }, // STOP
    { L"wash1.png", L"wash2.png" }, // WASH
    { L"scratch1.png", L"scratch2.png" }, // SCRATCH
    { L"yawn1.png", L"yawn2.png" }, // YAWN
    { L"sleep1.png", L"sleep2.png" }, // SLEEP
    { L"awake.png", L"awake.png" }, // AWAKE
    { L"up1.png", L"up2.png" }, // U_MOVE
    { L"down1.png", L"down2.png" }, // D_MOVE
    { L"left1.png", L"left2.png" }, // L_MOVE
    { L"right1.png", L"right2.png" }, // R_MOVE
    { L"upleft1.png", L"upleft2.png" }, // UL_MOVE
    { L"upright1.png", L"upright2.png" }, // UR_MOVE
    { L"downleft1.png", L"downleft2.png" }, // DL_MOVE
    { L"downright1.png", L"downright2.png" }, // DR_MOVE
    { L"upclaw1.png", L"upclaw2.png" }, // U_CLAW
    { L"downclaw1.png", L"downclaw2.png" }, // D_CLAW
    { L"leftclaw1.png", L"leftclaw2.png" }, // L_CLAW
    { L"rightclaw1.png", L"rightclaw2.png" }, // R_CLAW
    { L"awake.png", L"awake.png" } // FALL
};

void CreatePath(std::wstring path) {
    size_t pos = 0;
    while ((pos = path.find_first_of(L"\\/", pos + 1)) != std::wstring::npos) {
        std::wstring dir = path.substr(0, pos);
        CreateDirectoryW(dir.c_str(), NULL);
    }
    CreateDirectoryW(path.c_str(), NULL);
}

bool EnsureFileExists(const std::wstring& localPath, const std::wstring& remoteUrl) {
    // Security: Only allow established asset formats
    bool isPng = localPath.size() >= 4 && (_wcsicmp(localPath.c_str() + localPath.size() - 4, L".png") == 0);
    bool isWav = localPath.size() >= 4 && (_wcsicmp(localPath.c_str() + localPath.size() - 4, L".wav") == 0);
    
    if (!isPng && !isWav) {
        return false;
    }

    if (GetFileAttributesW(localPath.c_str()) != INVALID_FILE_ATTRIBUTES) return true;
    
    Wh_Log(L"Downloading missing asset: %s", remoteUrl.c_str());
    
    WH_GET_URL_CONTENT_OPTIONS opt = { sizeof(WH_GET_URL_CONTENT_OPTIONS), localPath.c_str() };
    const WH_URL_CONTENT* content = Wh_GetUrlContent(remoteUrl.c_str(), &opt);
    if (!content) {
        Wh_Log(L"Failed to fetch asset: %s", remoteUrl.c_str());
        return false;
    }
    
    bool ok = (content->statusCode == 200 || content->statusCode == 0);
    if (!ok) {
        Wh_Log(L"Download failed with status %d: %s", content->statusCode, remoteUrl.c_str());
        DeleteFileW(localPath.c_str());
    } else {
        Wh_Log(L"Successfully downloaded: %s", localPath.c_str());
    }
    
    Wh_FreeUrlContent(content);
    return ok;
}

bool EnsureThemeDownloaded(const std::wstring& themeName) {
    if (themeName.empty()) return false;
    std::wstring themePath = g_storagePath + L"\\" + themeName;
    
    // Quick check to avoid repeated 404 HTTP requests for themes that lack optional sounds
    std::wstring spritePath = themePath + L"\\spritesheet.png";
    if (GetFileAttributesW(spritePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return true; 
    }

    CreatePath(themePath);
    CreatePath(themePath + L"\\sounds");

    std::wstring baseUrl = L"https://raw.githubusercontent.com/ciizerr/wh-mods/9a7de898a9a149f096417ea74204925c4f930dcf/assets/" + themeName + L"/";

    bool ok = EnsureFileExists(spritePath, baseUrl + L"spritesheet.png");
    
    const wchar_t* audios[] = { L"awake.wav", L"sleep.wav", L"idle1.wav", L"idle2.wav", L"idle3.wav" };
    for (const wchar_t* au : audios) {
        std::wstring file(au);
        if (!EnsureFileExists(themePath + L"\\sounds\\" + file, baseUrl + L"sounds/" + file)) {
            ok = false;
        }
    }
    return ok;
}

void DownloadMissingAssets() {
    Wh_Log(L"Checking for missing assets in: %s", g_storagePath.c_str());
    
    // Always ensure the default theme is downloaded for fallback
    EnsureThemeDownloaded(L"neko-cat");
}

std::wstring GetRandomAvailableTheme() {
    if (g_storagePath.empty()) return L"";

    std::vector<std::wstring> themes;
    WIN32_FIND_DATAW fd;
    std::wstring searchPath = g_storagePath + L"\\*";
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (wcscmp(fd.cFileName, L".") != 0 && wcscmp(fd.cFileName, L"..") != 0) {
                    std::wstring testPath = g_storagePath + L"\\" + fd.cFileName + L"\\spritesheet.png";
                    if (GetFileAttributesW(testPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                        themes.push_back(g_storagePath + L"\\" + fd.cFileName);
                    }
                }
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }

    if (!themes.empty()) {
        int idx = rand() % themes.size();
        return themes[idx];
    }
    return g_storagePath + L"\\neko-cat"; // Fallback folder
}

std::wstring GetThemePathForPet(int index) {
    if (g_randomThemes) {
        return GetRandomAvailableTheme();
    }

    if (g_customPets.empty()) {
        return g_storagePath + L"\\neko-cat";
    }
    
    std::wstring chosenTheme = g_customPets[index % g_customPets.size()].theme;
    
    // Find official theme match case-insensitively
    std::wstring matchedTheme = chosenTheme;
    std::wstring lowerChosen = chosenTheme;
    for (auto& c : lowerChosen) c = towlower(c);
    
    for (const auto& official : g_officialThemes) {
        std::wstring lowerOfficial = official;
        for (auto& c : lowerOfficial) c = towlower(c);
        if (lowerChosen == lowerOfficial) {
            matchedTheme = official;
            break;
        }
    }
    return g_storagePath + L"\\" + matchedTheme;
}

class Neko {
public:
    HWND hwnd = NULL;
    Bitmap* sprites[MAX_STATE][2] = {};
    std::wstring assetPath;

    // ==========================================
    // --- 1. State & Behavior Variables ---
    // ==========================================
    int behaviorMode = CHASE_MOUSE;      // The current active behavior (e.g., CHASE_MOUSE, RUN_AWAY)
    int prevBehaviorMode = CHASE_MOUSE;  // Stores previous behavior to restore after a forced event (like being dropped)
    int idleThreshold = 6;               // How many pixels the mouse must move to wake the cat from an idle state
    NekoState state = STOP;              // The precise current animation/logic state (e.g., WASH, L_MOVE, SCRATCH)
    int tickCount = 0;                   // Raw logic tick counter, increments by 1 every tick
    int stateCount = 0;                  // How many ticks the character has spent in the current state (increments every 2 ticks)

    // ==========================================
    // --- 2. Physics & Position Variables ---
    // ==========================================
    double x = 0, y = 0;                 // Actual rendering coordinates on screen (interpolated for smoothness)
    double logicX = 0, logicY = 0;       // Underlying logic coordinates tied to the 5 Hz physics tick
    double prevLogicX = 0, prevLogicY = 0; // Previous logic coordinates (used for interpolation)
    double targetX = 0, targetY = 0;     // The target destination coordinates (e.g., the mouse position)
    double oldTargetX = 0, oldTargetY = 0; // The previous target destination (used to check if target moved)
    int moveDX = 0, moveDY = 0;          // Current physical velocity in X and Y
    int lastMoveDX = 0, lastMoveDY = 0;  // Previous physical velocity

    // ==========================================
    // --- 3. Screen & Bounds Variables ---
    // ==========================================
    int virtualX = 0, virtualY = 0;      // Top-left coordinates of the entire virtual screen (all monitors combined)
    int boundsWidth = 1920, boundsHeight = 1080; // Total width and height of the virtual screen minus sprite size

    // ==========================================
    // --- 4. Input & Render State ---
    // ==========================================
    int mouseX = 0, mouseY = 0;          // Cached mouse coordinates
    bool hasMouseMoved = false;          // Flag indicating if the mouse has moved recently
    double tickAccumulator = 0;          // Time accumulator used to decouple physics (5 Hz) from rendering (60+ FPS)
    double animationFrameFloat = 0;      // Distance accumulator used to smoothly alternate leg animations
    double renderLastX = -9999, renderLastY = -9999; // Previous render frame coordinates for calculating distance
    bool isDragging = false;             // True if the user is currently dragging the character via the window title

    // ==========================================
    // --- 5. Behavior-Specific Variables ---
    // ==========================================
    int cornerIndex = 0;                 // Used in PaceAroundScreen to track which corner it is heading to
    double ballX = -9999, ballY = -9999; // Physics coordinates of the invisible bouncing ball (RunAround mode)
    double ballVX = 0, ballVY = 0;       // Velocity of the invisible bouncing ball
    int actionCount = 0;                 // General purpose counter for long-running behaviors (e.g. PlayWithWindow)
    int offsetX = 0, offsetY = 0;        // Circular offset used when multiple cats swarm the mouse

    // ==========================================
    // --- 6. Audio Variables ---
    // ==========================================
    ULONGLONG lastSleepSoundTime = 0;    // Timestamp of the last sleep (snoring) sound played
    bool hasPlayedSleepSound = false;    // Whether the sleep sound has already played (for non-repeating config)
    int id = 0;                          // The unique index of this character in the litter

    // ==========================================
    // --- UTILITY & SETUP FUNCTIONS ---
    // ==========================================

    // Saves behaviorMode using Windhawk persistence
    void SaveBehavior() {
        extern bool g_saveBehavior;
        if (!g_saveBehavior) return;
        std::wstring key = L"behavior_" + std::to_wstring(id);
        Wh_SetIntValue(key.c_str(), behaviorMode);
    }

    // Loads last saved behaviorMode from Windhawk storage
    void LoadBehavior() {
        extern bool g_saveBehavior;
        if (!g_saveBehavior) return;
        std::wstring key = L"behavior_" + std::to_wstring(id);
        int mode = Wh_GetIntValue(key.c_str(), CHASE_MOUSE);
        if (mode >= CHASE_MOUSE && mode <= PLAY_WITH_WINDOW) {
            behaviorMode = mode;
            prevBehaviorMode = mode;
        }
    }

    // Slices spritesheet PNG into animation frames
    void LoadSprites() {
        // Free existing sprites if any (prevents memory leak on theme switch)
        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                if (sprites[i][f]) {
                    delete sprites[i][f];
                    sprites[i][f] = nullptr;
                }
            }
        }

        std::wstring path = assetPath + L"\\spritesheet.png";
        Bitmap* sheet = Bitmap::FromFile(path.c_str());
        if (!sheet || sheet->GetLastStatus() != Ok) {
            Wh_Log(L"Error loading spritesheet: %s. Falling back to default neko-cat.", path.c_str());
            if (sheet) delete sheet;
            
            // Fallback to default neko-cat
            std::wstring fallbackPath = g_storagePath + L"\\neko-cat\\spritesheet.png";
            sheet = Bitmap::FromFile(fallbackPath.c_str());
            if (!sheet || sheet->GetLastStatus() != Ok) {
                Wh_Log(L"Critical: Fallback spritesheet failed to load: %s", fallbackPath.c_str());
                if (sheet) delete sheet;
                return;
            }
            assetPath = g_storagePath + L"\\neko-cat";
        }

        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                int srcX = f * SPRITE_SIZE;
                int srcY = i * SPRITE_SIZE;
                sprites[i][f] = sheet->Clone(srcX, srcY, SPRITE_SIZE, SPRITE_SIZE, PixelFormat32bppARGB);
                if (!sprites[i][f] || sprites[i][f]->GetLastStatus() != Ok) {
                    Wh_Log(L"Error cloning sprite frame %d for state %d", f, i);
                }
            }
        }
        delete sheet;
    }

    // Plays an audio file from the theme's sounds folder
    void PlayAudio(const wchar_t* file, bool loop) {
        if (!g_soundEnabled) return;
        std::wstring path = assetPath + L"\\sounds\\" + file;
        Wh_Log(L"Playing audio: %s", file);
        DWORD flags = SND_ASYNC | SND_FILENAME | SND_NODEFAULT;
        if (loop) flags |= SND_LOOP;
        PlaySoundW(path.c_str(), NULL, flags);
    }
    // Stops currently playing audio
    void StopAudio() {
        PlaySoundW(NULL, NULL, 0);
    }

    // Calculates circular offset for swarm spawning
    void UpdateOffsets(int index, int total) {
        if (total <= 1) {
            offsetX = 0;
            offsetY = 0;
            return;
        }
        double angle = (2.0 * 3.1415926535 / total) * index;
        double radius = (16.0 + 8.0 * total) * g_scale;
        offsetX = (int)(cos(angle) * radius);
        offsetY = (int)(sin(angle) * radius);
    }

    // Gets absolute Y-coordinate of the floor below the character
    double GetCurrentFloorY() {
        int sz = SPRITE_SIZE * g_scale;
        POINT pt = { (LONG)(logicX + sz / 2.0), (LONG)(logicY + sz / 2.0) };
        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hMon, &mi)) {
            return mi.rcMonitor.bottom - sz;
        }
        return virtualY + boundsHeight;
    }

    // Checks if the character box is fully inside any active monitor
    bool IsRectInMonitors(double nx, double ny, int sz) {
        POINT corners[4] = {
            { (LONG)nx, (LONG)ny },
            { (LONG)(nx + sz - 1), (LONG)ny },
            { (LONG)nx, (LONG)(ny + sz - 1) },
            { (LONG)(nx + sz - 1), (LONG)(ny + sz - 1) }
        };
        for (int i = 0; i < 4; i++) {
            if (MonitorFromPoint(corners[i], MONITOR_DEFAULTTONULL) == NULL) {
                return false;
            }
        }
        return true;
    }

    // Prevents character from stepping outside monitor boundaries
    void ClampToMonitor(double& nx, double& ny, bool& wasOutside) {
        int sz = SPRITE_SIZE * g_scale;
        
        if (IsRectInMonitors(nx, ny, sz)) {
            wasOutside = false;
            return;
        }
        
        bool okX = IsRectInMonitors(nx, logicY, sz);
        bool okY = IsRectInMonitors(logicX, ny, sz);
        
        if (okX && !okY) {
            ny = logicY;
            wasOutside = true;
        } else if (okY && !okX) {
            nx = logicX;
            wasOutside = true;
        } else {
            HMONITOR hMonCur = MonitorFromPoint({ (LONG)(logicX + sz/2.0), (LONG)(logicY + sz/2.0) }, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfo(hMonCur, &mi)) {
                nx = fmax((double)mi.rcMonitor.left, fmin((double)mi.rcMonitor.right - sz, nx));
                ny = fmax((double)mi.rcMonitor.top, fmin((double)mi.rcMonitor.bottom - sz, ny));
            } else {
                nx = logicX;
                ny = logicY;
            }
            wasOutside = true;
        }
    }

    // ==========================================
    // --- INITIALIZATION & WINDOW PROC ---
    // ==========================================

    // Initializes character, loads sprites, and creates window
    void Init(int _id) {
        id = _id;
        virtualX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtualY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        boundsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
        boundsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
        
        x = virtualX + (rand() % (boundsWidth > 0 ? boundsWidth : 1));
        y = virtualY + (rand() % (boundsHeight > 0 ? boundsHeight : 1));
        logicX = x; logicY = y;
        prevLogicX = x; prevLogicY = y;
        targetX = x; targetY = y;
        oldTargetX = x; oldTargetY = y;
        
        LoadSprites();

        WNDCLASSW wc = {};
        wc.lpfnWndProc = NekoWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"NekoCatLayeredWnd";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        
        hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            L"NekoCatLayeredWnd", L"Neko Cat",
            WS_POPUP,
            0, 0, SPRITE_SIZE * g_scale, SPRITE_SIZE * g_scale,
            NULL, NULL, wc.hInstance, NULL
        );
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        ShowWindow(hwnd, SW_SHOWNA);
    }

    // Window procedure handling clicks, drags, and context menus
    static LRESULT CALLBACK NekoWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        Neko* pThis = (Neko*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (!pThis) return DefWindowProc(hwnd, msg, wp, lp);

        if (msg == WM_NCLBUTTONDOWN || msg == WM_LBUTTONDOWN || msg == WM_NCLBUTTONDBLCLK || msg == WM_LBUTTONDBLCLK) {
            if (pThis->behaviorMode == FORCED_SLEEP || pThis->behaviorMode == EXHAUSTED_SLEEP) {
                pThis->behaviorMode = pThis->prevBehaviorMode;
                if (pThis->state == SLEEP || pThis->state == YAWN) pThis->PlayAudio(L"awake.wav", false);
                else { const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" }; pThis->PlayAudio(idles[rand() % 3], false); }
                pThis->SetState(AWAKE);
            } else {
                if (pThis->state == SLEEP) pThis->PlayAudio(L"awake.wav", false);
                else { const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" }; pThis->PlayAudio(idles[rand() % 3], false); }
                pThis->SetState(AWAKE);
                if (msg == WM_NCLBUTTONDOWN || msg == WM_LBUTTONDOWN) {
                    pThis->CycleBehavior();
                }
            }
            SetForegroundWindow(hwnd);
            
            // Critical: Only return 0 for double clicks to prevent maximizing.
            // For single clicks, we MUST let DefWindowProc handle it so dragging works!
            if (msg == WM_NCLBUTTONDBLCLK || msg == WM_LBUTTONDBLCLK) return 0;
        } else if (msg == WM_NCRBUTTONUP || msg == WM_RBUTTONUP) {
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == CHASE_MOUSE ? MF_CHECKED : 0), 1000 + CHASE_MOUSE, L"Chase Mouse");
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == RUN_AWAY ? MF_CHECKED : 0), 1000 + RUN_AWAY, L"Run Away");
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == RANDOM ? MF_CHECKED : 0), 1000 + RANDOM, L"Random");
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == PACE ? MF_CHECKED : 0), 1000 + PACE, L"Pace");
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == RUN_AROUND ? MF_CHECKED : 0), 1000 + RUN_AROUND, L"Run Around");
            AppendMenuW(hMenu, MF_STRING | (pThis->behaviorMode == PLAY_WITH_WINDOW ? MF_CHECKED : 0), 1000 + PLAY_WITH_WINDOW, L"Play With Window");
            
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
            return 0;
        } else if (msg == WM_COMMAND) {
            int cmd = LOWORD(wp);
            if (cmd >= 1000 && cmd < 1000 + MAX_BEHAVIOR) {
                pThis->behaviorMode = cmd - 1000;
                pThis->prevBehaviorMode = pThis->behaviorMode;
                pThis->actionCount = 0;
                pThis->ballX = -9999;
                pThis->ballY = -9999;
                Wh_Log(L"Behavior changed via menu to: %d (%s)", pThis->behaviorMode, GetBehaviorName(pThis->behaviorMode));
                pThis->SaveBehavior();
                if (pThis->state == SLEEP || pThis->state == YAWN) {
                    pThis->PlayAudio(L"awake.wav", false);
                    pThis->SetState(AWAKE);
                }
            }
            return 0;
        } else if (msg == WM_ENTERSIZEMOVE) {
            pThis->isDragging = true;
        } else if (msg == WM_EXITSIZEMOVE) {
            pThis->isDragging = false;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            pThis->x = rect.left;
            pThis->y = rect.top;
            pThis->logicX = pThis->x;
            pThis->logicY = pThis->y;
            pThis->prevLogicX = pThis->x;
            pThis->prevLogicY = pThis->y;
            
            if (pThis->behaviorMode == PLAY_WITH_WINDOW) {
                pThis->prevBehaviorMode = pThis->behaviorMode;
                pThis->behaviorMode = FORCED_SLEEP;
                Wh_Log(L"Character dropped at %d, %d. Behavior: %d (%s)", 
                       (int)pThis->x, (int)pThis->y, pThis->behaviorMode, GetBehaviorName(pThis->behaviorMode));
                pThis->SetState(YAWN);
                pThis->oldTargetX = pThis->targetX = pThis->logicX + SPRITE_SIZE * g_scale / 2.0;
                pThis->oldTargetY = pThis->targetY = pThis->logicY + SPRITE_SIZE * g_scale;
            } else if (pThis->behaviorMode == FORCED_SLEEP || pThis->behaviorMode == EXHAUSTED_SLEEP) {
                Wh_Log(L"Character dropped at %d, %d while sleeping. Continuing sleep.", (int)pThis->x, (int)pThis->y);
                pThis->oldTargetX = pThis->targetX = pThis->logicX + SPRITE_SIZE * g_scale / 2.0;
                pThis->oldTargetY = pThis->targetY = pThis->logicY + SPRITE_SIZE * g_scale;
            } else {
                Wh_Log(L"Character dropped at %d, %d. Resuming behavior.", (int)pThis->x, (int)pThis->y);
                pThis->SetState(AWAKE);
                pThis->oldTargetX = pThis->targetX = pThis->logicX;
                pThis->oldTargetY = pThis->targetY = pThis->logicY;
            }
        } else if (msg == WM_NCHITTEST) {
            LRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    // Cycles behavior mode on left-click
    void CycleBehavior() {
        if (behaviorMode == FORCED_SLEEP) return;
        int behaviors[] = {CHASE_MOUSE, RUN_AWAY, RANDOM, PACE, RUN_AROUND, PLAY_WITH_WINDOW};
        int count = 6;
        int nextMode = CHASE_MOUSE;
        for (int i = 0; i < count; i++) {
            if (behaviorMode == behaviors[i]) {
                nextMode = behaviors[(i + 1) % count];
                break;
            }
        }
        prevBehaviorMode = behaviorMode;
        behaviorMode = nextMode;
        actionCount = 0;
        ballX = -9999;
        ballY = -9999;
        Wh_Log(L"Behavior changed to: %d (%s)", behaviorMode, GetBehaviorName(behaviorMode));
        SaveBehavior();
        if (state == SLEEP) SetState(AWAKE);
    }

    // ==========================================
    // --- MAIN LOOP & RENDERING ---
    // ==========================================

    // High FPS render loop: interpolates position and resolves collisions
    void Update() {
        POINT pt;
        GetCursorPos(&pt);
        if (pt.x != mouseX || pt.y != mouseY) {
            mouseX = pt.x; mouseY = pt.y;
            hasMouseMoved = true;
        }
        
        virtualX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtualY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        boundsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
        boundsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - SPRITE_SIZE * g_scale;

        if (isDragging) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            x = rect.left;
            y = rect.top;
            logicX = x; logicY = y;
            prevLogicX = x; prevLogicY = y;
            return;
        }

        double originalFPS = 5.0;
        tickAccumulator += originalFPS / (double)g_fps;

        while (tickAccumulator >= 1.0) {
            tickAccumulator -= 1.0;
            prevLogicX = logicX;
            prevLogicY = logicY;
            ProcessOriginalTick();
        }

        double t = tickAccumulator;
        x = prevLogicX + (logicX - prevLogicX) * t;
        y = prevLogicY + (logicY - prevLogicY) * t;

        ResolveWindowCollisions();

        UpdateWindowPosition();
    }

    // Resolves physics collisions when the character bumps into the active window
    // while in PLAY_WITH_WINDOW or EXHAUSTED_SLEEP modes.
    void ResolveWindowCollisions() {
        if (!isDragging && (behaviorMode == PLAY_WITH_WINDOW || behaviorMode == EXHAUSTED_SLEEP)) {
            RECT r;
            if (GetPlayWindow(r)) {
                int sz = SPRITE_SIZE * g_scale;
                if (x + sz > r.left && x < r.right && y + sz > r.top && y < r.bottom) {
                    double distLeft = (x + sz) - r.left;
                    double distRight = r.right - x;
                    double distTop = (y + sz) - r.top;
                    double distBottom = r.bottom - y;

                    double minDist = std::min({distLeft, distRight, distTop, distBottom});

                    if (minDist == distTop) {
                        y = r.top - sz;
                    } else if (minDist == distBottom) {
                        y = (double)r.bottom;
                    } else if (minDist == distLeft) {
                        x = r.left - sz;
                    } else {
                        x = (double)r.right;
                    }
                    
                    // Sync logic coordinates so the tick engine doesn't fight the push
                    logicX = prevLogicX = x;
                    logicY = prevLogicY = y;

                    // Wake up if pushed while exhausted
                    if (behaviorMode == EXHAUSTED_SLEEP) {
                        behaviorMode = prevBehaviorMode; // Resumes PLAY_WITH_WINDOW
                        SetState(AWAKE);
                    }
                }
            }
        }
    }

    // ==========================================
    // --- PHYSICS TICK & STATE MACHINE ---
    // ==========================================

    // 5Hz logic tick: processes behaviors, timers, and sounds
    void ProcessOriginalTick() {
        tickCount++;
        if (tickCount >= 9999) tickCount = 0;
        if (tickCount % 2 == 0) stateCount++;

        double floorY = GetCurrentFloorY();
        if (logicY < floorY && (state == SLEEP || state == STOP || state == WASH || state == SCRATCH || state == YAWN) && behaviorMode == FORCED_SLEEP) {
            // falling while in forced_sleep, gravity handled by ForcedSleep()
        }

        switch (behaviorMode) {
            case CHASE_MOUSE: ChaseMouse(); break;
            case RUN_AWAY: RunAwayFromMouse(); break;
            case RANDOM: RunRandomly(); break;
            case PACE: PaceAroundScreen(); break;
            case RUN_AROUND: RunAround(); break;
            case PLAY_WITH_WINDOW: PlayWithWindow(); break;
            case FORCED_SLEEP: 
            case EXHAUSTED_SLEEP: ForcedSleep(); break;
        }

        // Auto-restore behavior after post-drop sleep cycle finishes
        if (behaviorMode == FORCED_SLEEP && state == SLEEP && stateCount >= WASH_TIME * 3) {
            behaviorMode = prevBehaviorMode;
            SetState(AWAKE);
        }
        if (state == SLEEP) {
            ULONGLONG now = GetTickCount64();
            bool shouldPlay = false;
            if (!g_sleepSoundRepeat) {
                if (!hasPlayedSleepSound) {
                    shouldPlay = true;
                    hasPlayedSleepSound = true;
                }
            } else if (now - lastSleepSoundTime > (ULONGLONG)g_sleepSoundInterval * 1000) {
                shouldPlay = true;
            }

            if (shouldPlay) {
                PlayAudio(L"sleep.wav", false);
                lastSleepSoundTime = now;
            }
        } else {
            // random chance to purr/idle noise
            // skip if forced sleep is active or if the user wants a quiet sleeping cat
            if ((state == STOP || state == WASH) && g_soundEnabled && g_sleepSoundRepeat && behaviorMode != FORCED_SLEEP && rand() % 100 == 0) {
                const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                PlayAudio(idles[rand() % 3], false);
            }
        }
    }

    // ==========================================
    // --- BEHAVIOR IMPLEMENTATIONS ---
    // ==========================================

    // Behavior: Fall and sleep when dropped
    void ForcedSleep() {
        double floorY = GetCurrentFloorY();
        if (logicY < floorY) {
            double fallSpeed = g_speed * 4.0 * g_scale;
            if (state != FALL) SetState(FALL);
            logicY = std::min(logicY + fallSpeed, floorY);
            moveDX = moveDY = 0;
        } else {
            // Landed — do the yawn->sleep sequence
            logicY = floorY;
            if (state == FALL) SetState(YAWN);
            else if (state == YAWN && stateCount >= YAWN_TIME) SetState(SLEEP);
            moveDX = moveDY = 0;
        }
    }

    // -------------------------------------------------------
    // PLAY_WITH_WINDOW logic
    // Neko interacts with the currently active (non-maximized)
    // window: falls onto it, walks its top edge, climbs sides.
    // -------------------------------------------------------
    bool playWindowValid = false;
    RECT playWindowRect = {};
    int  climbRetries  = 0;
    int  climbSide     = 0;
    bool climbOnFloor  = false;

    // Finds an active, non-maximized window to interact with
    bool GetPlayWindow(RECT& out) {
        HWND hw = GetForegroundWindow();
        if (!hw || hw == GetDesktopWindow() || hw == GetShellWindow() || hw == hwnd) return false;
        if (IsZoomed(hw)) return false;  // skip maximised
        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(wp);
        GetWindowPlacement(hw, &wp);
        if (wp.showCmd == SW_SHOWMINIMIZED) return false;
        RECT r;
        if (!GetWindowRect(hw, &r)) return false;
        out = r;
        return true;
    }

    // Behavior: Interact with the currently active window
    void PlayWithWindow() {
        int sz = SPRITE_SIZE * g_scale;
        double fallSpeed = g_speed * 4.0 * g_scale;
        double climbSpeed = g_speed * 1.0 * g_scale;

        RECT r;
        playWindowValid = GetPlayWindow(r);
        double floorY = GetCurrentFloorY();

        if (!playWindowValid) {
            // No window — fall to screen bottom
            if (logicY < floorY) {
                if (state != FALL) SetState(FALL);
                logicY = std::min(logicY + fallSpeed, floorY);
                moveDX = moveDY = 0;
            } else {
                logicY = floorY;
                if (state == FALL) {
                    SetState(STOP);
                    actionCount = 0;
                    targetX = logicX;
                }
                
                actionCount++;
                double nextTargetX = targetX;
                if (actionCount > idleThreshold * (state == SLEEP ? 10 : 5) || targetX < virtualX || targetX > virtualX + boundsWidth) {
                    actionCount = 0;
                    if (boundsWidth > 0) {
                        nextTargetX = virtualX + rand() % boundsWidth;
                    } else {
                        nextTargetX = virtualX;
                    }
                }
                RunTowards(nextTargetX, floorY + sz);
            }
            return;
        }

        int catCenterX = (int)logicX + sz / 2;
        int catBottom  = (int)logicY + sz;

        // Snap tolerance: within 2px of the window top counts as "landed"
        int snapTol = 2;
        bool landedOnTop = catBottom >= r.top - snapTol && catBottom <= r.top + snapTol;
        bool overWindowX = catCenterX >= r.left && catCenterX <= r.right;
        bool strictlyAbove = catBottom < r.top - snapTol;   // clearly not touching
        bool leftOfWindow  = catCenterX < r.left;
        bool rightOfWindow = catCenterX > r.right;

        // --- 1. Landed on top: snap and walk left/right ---
        if (landedOnTop && overWindowX) {
            logicY = r.top - sz; // hard snap
            if (state == FALL) {
                SetState(STOP);
                actionCount = 0;
                targetX = logicX;
            }

            actionCount++;
            double nextTargetX = targetX;
            // Wait before moving again, if sleeping wait much longer.
            if (actionCount > idleThreshold * (state == SLEEP ? 10 : 5) || targetX < r.left || targetX > r.right - sz) {
                actionCount = 0;
                long windowWidth = r.right - r.left - sz;
                if (windowWidth > 0) {
                    nextTargetX = r.left + rand() % windowWidth;
                } else {
                    nextTargetX = r.left;
                }
            }

            RunTowards(nextTargetX, logicY + sz);
            return;
        }

        // --- 2. Strictly above the window and over it: fall straight down ---
        if (strictlyAbove && overWindowX) {
            if (state != FALL) SetState(FALL);
            double newY = logicY + fallSpeed;
            if (newY + sz >= r.top) {
                // Land precisely
                logicY = r.top - sz;
                moveDX = g_speed * g_scale; // start walking on landing
                SetState(STOP);
            } else {
                logicY = newY;
            }
            moveDX = 0; moveDY = 0;
            return;
        }

        // --- 3. Left of window: approach then climb the left wall ---
        if (leftOfWindow) {
            double runSpeed = g_speed * (double)g_scale;
            // Target: cat's left edge flush with window left wall
            double wallX = r.left - sz;  // logicX target when at wall
            bool onFloor  = std::abs(logicY - floorY) < 2.0;
            bool atWall   = std::abs(logicX - wallX) < runSpeed + 2.0;

            if (climbSide != -1) { climbSide = -1; climbRetries = 0; }

            if (!atWall) {
                if (!onFloor) {
                    // Still falling to floor
                    if (state != FALL) SetState(FALL);
                    logicY = std::min(logicY + fallSpeed, floorY);
                } else {
                    // On floor — walk directly toward wall (no RunTowards)
                    if (climbOnFloor == false) {
                        if (climbRetries >= 3) {
                            climbRetries = 0;
                            climbSide = 0;
                            prevBehaviorMode = PLAY_WITH_WINDOW;
                            behaviorMode = EXHAUSTED_SLEEP;
                            return;
                        }
                        // Just landed after a fall — record retry
                        if (climbRetries > 0) {
                            // Shift position: alternate left/right small offsets
                            int offset = (climbRetries % 2 == 1 ? 1 : -1) * (climbRetries / 2 + 1) * sz / 4;
                            logicX = std::max((double)virtualX, wallX + offset);
                        }
                        climbRetries++;
                    }
                    climbOnFloor = true;
                    double dx = wallX - logicX;
                    if (state == FALL) SetState(STOP);
                    if (std::abs(dx) > runSpeed) {
                        logicX += (dx > 0 ? runSpeed : -runSpeed);
                        CalcDirection(dx, 0);
                    } else {
                        logicX = wallX;
                    }
                }
            } else {
                // Snap to wall and climb
                climbOnFloor = false;
                logicX = wallX;
                if ((int)logicY > r.top - sz + 2) {
                    if (state != U_MOVE) SetState(U_MOVE);
                    logicY -= climbSpeed;
                } else {
                    // Reached the top!
                    logicY = r.top - sz;
                    logicX = r.left;
                    moveDX = (int)runSpeed;
                    climbRetries = 0; climbSide = 0;
                    SetState(STOP);
                }
            }
            moveDX = 0; moveDY = 0;
            return;
        }

        // --- 4. Right of window: approach then climb the right wall ---
        if (rightOfWindow) {
            double runSpeed = g_speed * (double)g_scale;
            // Target: cat's right edge flush with window right wall
            double wallX = (double)r.right;  // logicX target when at wall
            bool onFloor  = std::abs(logicY - floorY) < 2.0;
            bool atWall   = std::abs(logicX - wallX) < runSpeed + 2.0;

            if (climbSide != 1) { climbSide = 1; climbRetries = 0; }

            if (!atWall) {
                if (!onFloor) {
                    if (state != FALL) SetState(FALL);
                    logicY = std::min(logicY + fallSpeed, floorY);
                } else {
                    if (climbOnFloor == false) {
                        if (climbRetries >= 3) {
                            climbRetries = 0;
                            climbSide = 0;
                            prevBehaviorMode = PLAY_WITH_WINDOW;
                            behaviorMode = EXHAUSTED_SLEEP;
                            return;
                        }
                        if (climbRetries > 0) {
                            int offset = (climbRetries % 2 == 1 ? -1 : 1) * (climbRetries / 2 + 1) * sz / 4;
                            logicX = std::min((double)(virtualX + boundsWidth), wallX + offset);
                        }
                        climbRetries++;
                    }
                    climbOnFloor = true;
                    double dx = wallX - logicX;
                    if (state == FALL) SetState(STOP);
                    if (std::abs(dx) > runSpeed) {
                        logicX += (dx > 0 ? runSpeed : -runSpeed);
                        CalcDirection(dx, 0);
                    } else {
                        logicX = wallX;
                    }
                }
            } else {
                climbOnFloor = false;
                logicX = wallX;
                if ((int)logicY > r.top - sz + 2) {
                    if (state != U_MOVE) SetState(U_MOVE);
                    logicY -= climbSpeed;
                } else {
                    logicY = r.top - sz;
                    logicX = r.right - sz;
                    moveDX = -(int)runSpeed;
                    climbRetries = 0; climbSide = 0;
                    SetState(STOP);
                }
            }
            moveDX = 0; moveDY = 0;
            return;
        }

        // --- 5. Cat is under the window (on floor, overWindowX but not at top) ---
        // Shuffle sideways to the nearest wall edge so we can start climbing.
        // Never use RunTowards here — it triggers the sleep cycle.
        {
            double runSpeed = g_speed * (double)g_scale;
            bool onFloor = std::abs(logicY - floorY) < 2.0;

            if (!onFloor) {
                // Fall to floor first
                if (state != FALL) SetState(FALL);
                logicY = std::min(logicY + fallSpeed, floorY);
            } else {
                // On floor — move toward the nearer window edge
                logicY = floorY;
                if (state == FALL) SetState(STOP);
                int catCX = (int)logicX + sz / 2;
                double distToLeft  = catCX - r.left;
                double distToRight = r.right - catCX;
                double targetX;
                if (distToLeft <= distToRight) {
                    // Nearer to left wall — exit left
                    targetX = r.left - sz;
                    double dx = targetX - logicX;
                    logicX += (std::abs(dx) > runSpeed ? -runSpeed : dx);
                    CalcDirection(-runSpeed, 0);
                } else {
                    // Nearer to right wall — exit right
                    targetX = (double)r.right;
                    double dx = targetX - logicX;
                    logicX += (std::abs(dx) > runSpeed ? runSpeed : dx);
                    CalcDirection(runSpeed, 0);
                }
            }
            moveDX = 0; moveDY = 0;
        }
    }

    // Behavior: Chase the mouse cursor
    void ChaseMouse() {
        if (!hasMouseMoved) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2.0, logicY + SPRITE_SIZE * g_scale);
            return;
        }
        RunTowards(mouseX + offsetX, mouseY + offsetY);
    }

    // Behavior: Run away from the mouse cursor
    void RunAwayFromMouse() {
        if (!hasMouseMoved) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2.0, logicY + SPRITE_SIZE * g_scale);
            return;
        }
        int dwLimit = idleThreshold * 16 * g_scale;
        double xdiff = logicX + SPRITE_SIZE * g_scale / 2.0 - mouseX;
        double ydiff = logicY + SPRITE_SIZE * g_scale / 2.0 - mouseY;
        
        if (abs(xdiff) < dwLimit && abs(ydiff) < dwLimit) {
            double dLength = sqrt(xdiff * xdiff + ydiff * ydiff);
            double tx, ty;
            if (dLength != 0) {
                tx = logicX + (xdiff / dLength) * dwLimit;
                ty = logicY + (ydiff / dLength) * dwLimit;
            } else {
                tx = ty = 32 * g_scale;
            }
            RunTowards(tx, ty);
            if (state == AWAKE) CalcDirection(tx - logicX, ty - logicY);
        } else {
            RunTowards(targetX, targetY);
        }
    }

    // Behavior: Run to random screen locations
    void RunRandomly() {
        actionCount++;
        double nx = targetX, ny = targetY;
        if (actionCount > idleThreshold * (state == SLEEP ? 15 : 20)) {
            actionCount = 0;
            nx = virtualX + rand() % (boundsWidth > 0 ? boundsWidth : 1) + SPRITE_SIZE * g_scale / 2.0;
            ny = virtualY + rand() % (boundsHeight > 0 ? boundsHeight : 1) + SPRITE_SIZE * g_scale;
        }
        RunTowards(nx, ny);
    }

    // Behavior: Patrol the screen borders
    void PaceAroundScreen() {
        if (lastMoveDX == 0 && lastMoveDY == 0) {
            cornerIndex = (cornerIndex + 1) % 4;
        }
        int sz = SPRITE_SIZE * g_scale;
        HMONITOR hMon = MonitorFromPoint({ (LONG)(logicX + sz/2.0), (LONG)(logicY + sz/2.0) }, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hMon, &mi)) {
            double corners[4][2] = {
                { (double)mi.rcMonitor.left + sz/2.0, (double)mi.rcMonitor.top + sz },
                { (double)mi.rcMonitor.left + sz/2.0, (double)mi.rcMonitor.bottom },
                { (double)mi.rcMonitor.right - sz/2.0, (double)mi.rcMonitor.bottom },
                { (double)mi.rcMonitor.right - sz/2.0, (double)mi.rcMonitor.top + sz }
            };
            RunTowards(corners[cornerIndex][0], corners[cornerIndex][1]);
        } else {
            double corners[4][2] = {
                { (double)virtualX + sz/2.0, (double)virtualY + sz },
                { (double)virtualX + sz/2.0, (double)virtualY + boundsHeight + sz },
                { (double)virtualX + boundsWidth + sz/2.0, (double)virtualY + boundsHeight + sz },
                { (double)virtualX + boundsWidth + sz/2.0, (double)virtualY + sz }
            };
            RunTowards(corners[cornerIndex][0], corners[cornerIndex][1]);
        }
    }

    // Behavior: Chase a bouncing invisible ball (with natural rest breaks)
    void RunAround() {
        actionCount++;
        if (actionCount >= 300) {
            actionCount = 0;
            if (state == SLEEP || state == YAWN) {
                PlayAudio(L"awake.wav", false);
                SetState(AWAKE);
            }
        }

        // After chasing for ~45 seconds (225 ticks), take a ~15 second nap break!
        if (actionCount > 225) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2.0, logicY + SPRITE_SIZE * g_scale);
            return;
        }

        double bbox = SPRITE_SIZE * g_scale;
        double ballSpeed = g_speed * g_scale * 1.3;
        HMONITOR hMon = MonitorFromPoint({ (LONG)(logicX + SPRITE_SIZE * g_scale / 2.0), (LONG)(logicY + SPRITE_SIZE * g_scale / 2.0) }, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hMon, &mi)) {
            if (ballX == -9999 && ballY == -9999) {
                ballX = mi.rcMonitor.left + rand() % (std::max(1, (int)(mi.rcMonitor.right - mi.rcMonitor.left - bbox * 2))) + bbox;
                ballY = mi.rcMonitor.top + rand() % (std::max(1, (int)(mi.rcMonitor.bottom - mi.rcMonitor.top - bbox * 2))) + bbox;
                double angle = ((rand() % 360) * 3.1415926535) / 180.0;
                ballVX = cos(angle) * ballSpeed;
                ballVY = sin(angle) * ballSpeed;
            }
            ballX += ballVX;
            ballY += ballVY;
            if (ballX < mi.rcMonitor.left + bbox) {
                ballX = mi.rcMonitor.left + bbox;
                ballVX = std::abs(ballVX);
                ballVY += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            } else if (ballX > mi.rcMonitor.right - bbox) {
                ballX = mi.rcMonitor.right - bbox;
                ballVX = -std::abs(ballVX);
                ballVY += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            }
            if (ballY < mi.rcMonitor.top + bbox) {
                ballY = mi.rcMonitor.top + bbox;
                ballVY = std::abs(ballVY);
                ballVX += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            } else if (ballY > mi.rcMonitor.bottom - bbox) {
                ballY = mi.rcMonitor.bottom - bbox;
                ballVY = -std::abs(ballVY);
                ballVX += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            }
            double currentSpeed = sqrt(ballVX * ballVX + ballVY * ballVY);
            if (currentSpeed > 0.001) {
                ballVX = (ballVX / currentSpeed) * ballSpeed;
                ballVY = (ballVY / currentSpeed) * ballSpeed;
            }
        } else {
            if (ballX == -9999 && ballY == -9999) {
                ballX = virtualX + rand() % (std::max(1, (int)(boundsWidth - bbox * 2))) + bbox;
                ballY = virtualY + rand() % (std::max(1, (int)(boundsHeight - bbox * 2))) + bbox;
                double angle = ((rand() % 360) * 3.1415926535) / 180.0;
                ballVX = cos(angle) * ballSpeed;
                ballVY = sin(angle) * ballSpeed;
            }
            ballX += ballVX;
            ballY += ballVY;
            if (ballX < virtualX + bbox) {
                ballX = virtualX + bbox;
                ballVX = std::abs(ballVX);
                ballVY += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            } else if (ballX > virtualX + boundsWidth - bbox) {
                ballX = virtualX + boundsWidth - bbox;
                ballVX = -std::abs(ballVX);
                ballVY += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            }
            if (ballY < virtualY + bbox) {
                ballY = virtualY + bbox;
                ballVY = std::abs(ballVY);
                ballVX += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            } else if (ballY > virtualY + boundsHeight - bbox) {
                ballY = virtualY + boundsHeight - bbox;
                ballVY = -std::abs(ballVY);
                ballVX += ((rand() % 201) - 100) * 0.01 * (ballSpeed * 0.75);
            }
            double currentSpeed = sqrt(ballVX * ballVX + ballVY * ballVY);
            if (currentSpeed > 0.001) {
                ballVX = (ballVX / currentSpeed) * ballSpeed;
                ballVY = (ballVY / currentSpeed) * ballSpeed;
            }
        }
        RunTowards(ballX, ballY);
    }

    // Updates logic state and handles transitions
    void SetState(NekoState newState) {
        if (state == SLEEP && newState != SLEEP) {
            StopAudio();
        }
        if (newState == SLEEP && state != SLEEP) {
            hasPlayedSleepSound = false;
        }

        // 50% chance to meow/purr when starting to yawn before sleep
        if (newState == YAWN && g_soundEnabled && (rand() % 2 == 0)) {
            const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
            PlayAudio(idles[rand() % 3], false);
        }

        tickCount = 0;
        stateCount = 0;
        state = newState;
    }

    // Calculates the 16-way movement state based on dx/dy
    void CalcDirection(double dx, double dy) {
        NekoState newState;
        if (dx == 0 && dy == 0) {
            newState = STOP;
        } else {
            double length = sqrt(dx*dx + dy*dy);
            double sinTheta = -dy / length; // inverted Y
            const double sinPiPer8 = 0.3826834323651;
            const double sinPiPer8Times3 = 0.9238795325113;

            if (dx > 0) {
                if (sinTheta > sinPiPer8Times3) newState = U_MOVE;
                else if (sinTheta > sinPiPer8) newState = UR_MOVE;
                else if (sinTheta > -sinPiPer8) newState = R_MOVE;
                else if (sinTheta > -sinPiPer8Times3) newState = DR_MOVE;
                else newState = D_MOVE;
            } else {
                if (sinTheta > sinPiPer8Times3) newState = U_MOVE;
                else if (sinTheta > sinPiPer8) newState = UL_MOVE;
                else if (sinTheta > -sinPiPer8) newState = L_MOVE;
                else if (sinTheta > -sinPiPer8Times3) newState = DL_MOVE;
                else newState = D_MOVE;
            }
        }
        if (state != newState) SetState(newState);
    }

    // Moves the character towards a target (tx, ty) and handles idle states
    void RunTowards(double tx, double ty) {
        oldTargetX = targetX; oldTargetY = targetY;
        targetX = tx; targetY = ty;
        
        int sz = SPRITE_SIZE * g_scale;
        double dx = tx - logicX - sz/2.0;
        double dy = ty - logicY - sz; 
        double dist = sqrt(dx*dx + dy*dy);

        double actualSpeed = g_speed * g_scale;

        if (dist != 0) {
            if (dist <= actualSpeed) {
                moveDX = (int)dx; moveDY = (int)dy;
            } else {
                moveDX = (int)(actualSpeed * dx / dist);
                moveDY = (int)(actualSpeed * dy / dist);
            }
        } else {
            moveDX = moveDY = 0;
        }

        lastMoveDX = moveDX; lastMoveDY = moveDY;

        bool moveStart = !(
            oldTargetX >= targetX - idleThreshold && oldTargetX <= targetX + idleThreshold &&
            oldTargetY >= targetY - idleThreshold && oldTargetY <= targetY + idleThreshold
        );

        switch (state) {
            case STOP:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= STOP_TIME) {
                    int sz = SPRITE_SIZE * g_scale;
                    HMONITOR hMon = MonitorFromPoint({ (LONG)(logicX + sz/2.0), (LONG)(logicY + sz/2.0) }, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO mi = { sizeof(mi) };
                    if (GetMonitorInfo(hMon, &mi)) {
                        if ((moveDX < 0 || targetX <= mi.rcMonitor.left + 5.0) && logicX <= mi.rcMonitor.left + 1.0) SetState(L_CLAW);
                        else if ((moveDX > 0 || targetX + sz >= mi.rcMonitor.right - 5.0) && logicX + sz >= mi.rcMonitor.right - 1.0) SetState(R_CLAW);
                        else if ((moveDY < 0 || targetY <= mi.rcMonitor.top + 5.0) && logicY <= mi.rcMonitor.top + 1.0) SetState(U_CLAW);
                        else if ((moveDY > 0 || targetY + sz >= mi.rcMonitor.bottom - 5.0) && logicY + sz >= mi.rcMonitor.bottom - 1.0) SetState(D_CLAW);
                        else SetState(WASH);
                    } else {
                        if ((moveDX < 0 || targetX <= virtualX + 5.0) && logicX <= virtualX + 1.0) SetState(L_CLAW);
                        else if ((moveDX > 0 || targetX + sz >= virtualX + boundsWidth - 5.0) && logicX + sz >= virtualX + boundsWidth - 1.0) SetState(R_CLAW);
                        else if ((moveDY < 0 || targetY <= virtualY + 5.0) && logicY <= virtualY + 1.0) SetState(U_CLAW);
                        else if ((moveDY > 0 || targetY + sz >= virtualY + boundsHeight - 5.0) && logicY + sz >= virtualY + boundsHeight - 1.0) SetState(D_CLAW);
                        else SetState(WASH);
                    }
                }
                break;
            case WASH:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= WASH_TIME) SetState(SCRATCH);
                break;
            case SCRATCH:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= SCRATCH_TIME) SetState(YAWN);
                break;
            case YAWN:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= YAWN_TIME) SetState(SLEEP);
                break;
            case SLEEP:
                if (moveStart) SetState(AWAKE);
                break;
            case AWAKE:
                if (stateCount >= AWAKE_TIME + rand() % 20) CalcDirection(moveDX, moveDY);
                break;
            case FALL:
            case U_MOVE: case D_MOVE: case L_MOVE: case R_MOVE:
            case UL_MOVE: case UR_MOVE: case DL_MOVE: case DR_MOVE: {
                double nx = logicX + moveDX;
                double ny = logicY + moveDY;
                bool wasOutside = false;
                ClampToMonitor(nx, ny, wasOutside);
                CalcDirection(moveDX, moveDY);
                
                bool notMoved = nx == logicX && ny == logicY;
                if (wasOutside && notMoved) SetState(STOP);
                else { logicX = nx; logicY = ny; }
                break;
            }
            case U_CLAW: case D_CLAW: case L_CLAW: case R_CLAW:
                if (moveStart) SetState(AWAKE);
                else if (stateCount >= CLAW_TIME) SetState(SCRATCH);
                break;
            default:
                SetState(STOP);
                break;
        }
    }

    double lastUpdateX = -999, lastUpdateY = -999;
    int lastUpdateFrame = -1;
    NekoState lastUpdateState = MAX_STATE;
    int lastUpdateScale = -1;

    void UpdateWindowPosition() {
        if (renderLastX == -9999) {
            renderLastX = x;
            renderLastY = y;
        }
        
        double deltaX = x - renderLastX;
        double deltaY = y - renderLastY;
        double distanceMovedThisFrame = sqrt(deltaX * deltaX + deltaY * deltaY);
        renderLastX = x;
        renderLastY = y;

        int frameObj = 0;
        bool isMoving = (state >= U_MOVE && state <= DR_MOVE) || state == FALL;
        
        if (isMoving) {
            // Legs toggle every 16 physical pixels (scaled)
            animationFrameFloat += distanceMovedThisFrame / (16.0 * g_scale);
            frameObj = (int)animationFrameFloat & 1;
        } else if (state == SLEEP) {
            frameObj = (tickCount >> 2) & 1;
            animationFrameFloat = 0;
        } else {
            frameObj = tickCount & 1;
            animationFrameFloat = 0;
        }

        if (lastUpdateX == x && lastUpdateY == y && lastUpdateFrame == frameObj && lastUpdateState == state && lastUpdateScale == g_scale) {
            return;
        }
        lastUpdateX = x;
        lastUpdateY = y;
        lastUpdateFrame = frameObj;
        lastUpdateState = state;
        lastUpdateScale = g_scale;

        Bitmap* bmp = sprites[state][frameObj];
        if (!bmp || bmp->GetLastStatus() != Ok) return;

        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        int outSize = SPRITE_SIZE * g_scale;

        HBITMAP hbmMem;
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = outSize;
        bi.bmiHeader.biHeight = -outSize;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits;
        hbmMem = CreateDIBSection(hdcMem, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
        
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        Graphics g(hdcMem);
        g.SetInterpolationMode(InterpolationModeNearestNeighbor);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);
        g.Clear(Color(0, 0, 0, 0));
        g.DrawImage(bmp, Rect(0, 0, outSize, outSize), 0, 0, SPRITE_SIZE, SPRITE_SIZE, UnitPixel);

        POINT ptSrc = {0, 0};
        POINT ptDest = { (LONG)round(x), (LONG)round(y) };
        SIZE winSize = { outSize, outSize };
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        UpdateLayeredWindow(hwnd, hdcScreen, &ptDest, &winSize, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
        
        // Re-enforce topmost status to prevent disappearing behind other "Topmost" windows
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
    }

    ~Neko() {
        if (hwnd) DestroyWindow(hwnd);
        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                if (sprites[i][f]) delete sprites[i][f];
            }
        }
    }
};

std::vector<Neko*> g_Nekos;

void CALLBACK UpdateAllCatsTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (g_isHidden || g_modExit) return;
    for (size_t i = 0; i < g_Nekos.size(); i++) {
        for (size_t j = i + 1; j < g_Nekos.size(); j++) {
            Neko* a = g_Nekos[i];
            Neko* b = g_Nekos[j];
            double dx = a->logicX - b->logicX;
            double dy = a->logicY - b->logicY;
            double distSq = dx*dx + dy*dy;
            double minDist = 32.0 * g_scale;
            if (distSq < minDist * minDist && distSq > 0.001) {
                double dist = sqrt(distSq);
                double overlap = minDist - dist;
                double pushX = (dx / dist) * overlap * 0.1;
                double pushY = (dy / dist) * overlap * 0.1;
                a->logicX += pushX; a->logicY += pushY;
                b->logicX -= pushX; b->logicY -= pushY;
            }
        }
    }
    for (auto pNeko : g_Nekos) {
        pNeko->Update();
    }
}

DWORD WINAPI NekoProcessThread(LPVOID param) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    DownloadMissingAssets();

    if (g_randomThemes) {
        // Ensure all official themes are downloaded so random selection has a pool
        for (const auto& off : g_officialThemes) {
            EnsureThemeDownloaded(off);
        }
    } else {
        // Ensure custom pet themes are downloaded
        for (const auto& pet : g_customPets) {
            EnsureThemeDownloaded(pet.theme);
        }
    }

    int targetCount = g_catCount;

    for (int i = 0; i < targetCount; ++i) {
        Neko* pNeko = new Neko();
        pNeko->assetPath = GetThemePathForPet(i);
        pNeko->Init(i);
        if (targetCount > 1) pNeko->behaviorMode = rand() % 5;
        pNeko->LoadBehavior();
        g_Nekos.push_back(pNeko);
    }
    for (size_t i = 0; i < g_Nekos.size(); ++i) {
        g_Nekos[i]->UpdateOffsets(i, g_Nekos.size());
    }
    
    // Track the overlay HWND for the tool mod
    if (!g_Nekos.empty()) g_hwndOverlay = g_Nekos[0]->hwnd;

    int intervalMs = 1000 / (g_fps > 0 ? g_fps : 60);

    UINT modifiers = 0;
    UINT vk = 0;
    bool hotkeyRegistered = false;
    
    auto RegisterNekoKey = [&]() {
        if (hotkeyRegistered) {
            UnregisterHotKey(NULL, 1);
            hotkeyRegistered = false;
        }
        if (!g_nekoKeyStr.empty()) {
            modifiers = ParseHotkeyModifiers(g_nekoKeyStr);
            vk = ParseHotkeyVK(g_nekoKeyStr);
            if (vk != 0) {
                hotkeyRegistered = RegisterHotKey(NULL, 1, modifiers, vk);
                if (!hotkeyRegistered) {
                    Wh_Log(L"Failed to register neko key. It might be in use.");
                } else {
                    Wh_Log(L"Registered Neko Key");
                }
            }
        }
    };
    
    RegisterNekoKey();

    UINT_PTR timerId = SetTimer(NULL, 1, intervalMs, UpdateAllCatsTimer);

    MSG msg;
    while (!g_modExit && GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_UPDATE_SETTINGS) {
            LoadSettings();
            
            intervalMs = 1000 / (g_fps > 0 ? g_fps : 60);
            KillTimer(NULL, timerId);
            timerId = SetTimer(NULL, 1, intervalMs, UpdateAllCatsTimer);
            RegisterNekoKey();
            
            // Ensure main theme is downloaded
            DownloadMissingAssets();
            
            if (g_randomThemes) {
                // Ensure all official themes are downloaded
                for (const auto& off : g_officialThemes) {
                    EnsureThemeDownloaded(off);
                }
            } else {
                // Ensure any custom selected themes in the pet list are downloaded
                for (const auto& pet : g_customPets) {
                    EnsureThemeDownloaded(pet.theme);
                }
            }

            int currentTargetCount = g_catCount;

            // Sync themes for existing Nekos if needed
            for (size_t i = 0; i < g_Nekos.size() && i < (size_t)currentTargetCount; ++i) {
                std::wstring newPath = GetThemePathForPet(i);
                if (g_Nekos[i]->assetPath != newPath) {
                    g_Nekos[i]->assetPath = newPath;
                    g_Nekos[i]->LoadSprites();
                }
            }

            // Spawn any new Nekos up to targetCount
            while ((int)g_Nekos.size() < currentTargetCount) {
                Neko* pNeko = new Neko();
                pNeko->assetPath = GetThemePathForPet(g_Nekos.size());
                pNeko->Init(g_Nekos.size());
                if (currentTargetCount > 1) pNeko->behaviorMode = rand() % 5;
                pNeko->LoadBehavior();
                if (g_isHidden) ShowWindow(pNeko->hwnd, SW_HIDE);
                g_Nekos.push_back(pNeko);
            }
            
            // Trim excess Nekos if targetCount decreased
            while ((int)g_Nekos.size() > currentTargetCount) {
                Neko* pNeko = g_Nekos.back();
                g_Nekos.pop_back();
                pNeko->StopAudio();
                delete pNeko;
            }
            
            for (size_t i = 0; i < g_Nekos.size(); ++i) {
                g_Nekos[i]->UpdateOffsets(i, g_Nekos.size());
                
                // Recalculate bounds and update size/scale variables for this pet
                g_Nekos[i]->virtualX = GetSystemMetrics(SM_XVIRTUALSCREEN);
                g_Nekos[i]->virtualY = GetSystemMetrics(SM_YVIRTUALSCREEN);
                g_Nekos[i]->boundsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
                g_Nekos[i]->boundsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
            }
            
            if (!g_Nekos.empty()) g_hwndOverlay = g_Nekos[0]->hwnd;
        } else if (msg.message == WM_HOTKEY && msg.wParam == 1) {
            g_isHidden = !g_isHidden;
            for (auto pNeko : g_Nekos) {
                ShowWindow(pNeko->hwnd, g_isHidden ? SW_HIDE : SW_SHOWNA);
                if (g_isHidden) {
                    pNeko->StopAudio();
                } else {
                    pNeko->hasMouseMoved = false;
                }
            }
            Wh_Log(L"Pet Key toggled. IsHidden: %d", g_isHidden);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (timerId) KillTimer(NULL, timerId);
    
    if (hotkeyRegistered) {
        UnregisterHotKey(NULL, 1);
    }

    for (auto pNeko : g_Nekos) {
        pNeko->StopAudio();
        delete pNeko;
    }
    g_Nekos.clear();

    GdiplusShutdown(gdiplusToken);
    return 0;
}

// ─────────────────────────────────────────────
//  Tool mod implementation
// ─────────────────────────────────────────────
void LoadSettings() {
    WCHAR storagePath[MAX_PATH];
    if (Wh_GetModStoragePath(storagePath, ARRAYSIZE(storagePath))) {
        g_storagePath = storagePath;
    }

    g_randomThemes = Wh_GetIntSetting(L"AppearanceGroup.random_themes") != 0;
    g_scale = Wh_GetIntSetting(L"AppearanceGroup.scale");
    g_speed = Wh_GetIntSetting(L"BehaviorGroup.speed");
    g_saveBehavior = Wh_GetIntSetting(L"BehaviorGroup.save_behavior") != 0;
    g_soundEnabled = Wh_GetIntSetting(L"AudioGroup.sound") != 0;
    g_sleepSoundInterval = Wh_GetIntSetting(L"AudioGroup.sleep_sound_interval");
    g_sleepSoundRepeat = Wh_GetIntSetting(L"AudioGroup.sleep_sound_repeat") != 0;
    g_fps = Wh_GetIntSetting(L"BehaviorGroup.fps");
    g_catCount = Wh_GetIntSetting(L"AppearanceGroup.character_count");
    if (g_catCount < 1) g_catCount = 1;

    g_customPets.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR key[64];
        swprintf_s(key, L"AppearanceGroup.characterList[%d].theme", i);
        PCWSTR themeVal = Wh_GetStringSetting(key);
        if (!themeVal || themeVal[0] == L'\0') {
            if (themeVal) Wh_FreeStringSetting(themeVal);
            break;
        }
        g_customPets.push_back({ themeVal });
        Wh_FreeStringSetting(themeVal);
    }

    PCWSTR nekoKeyStr = Wh_GetStringSetting(L"AdvancedGroup.neko_key");
    if (nekoKeyStr && nekoKeyStr[0] != L'\0') {
        g_nekoKeyStr = nekoKeyStr;
    } else {
        g_nekoKeyStr = L"";
    }
    if (nekoKeyStr) Wh_FreeStringSetting(nekoKeyStr);
}

BOOL WhTool_ModInit()
{
    Wh_Log(L"WhTool_ModInit called");

    srand(GetTickCount());
    LoadSettings();

    g_hThread = CreateThread(nullptr, 0, NekoProcessThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    if (g_hThread) {
        PostThreadMessage(GetThreadId(g_hThread), WM_UPDATE_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit()
{
    Wh_Log(L"WhTool_ModUninit called");

    g_modExit = true;
    if (g_hwndOverlay) {
        PostMessage(g_hwndOverlay, WM_QUIT, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, 5000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

// ============================================================================
//  Tool mod launcher code (using Windhawk injection & hooking)
// ============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"Desktop Pet: entry point hook triggered, exiting main thread.");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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