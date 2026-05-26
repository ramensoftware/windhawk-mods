// ==WindhawkMod==
// @id              neko-cat
// @name            Neko Cat
// @description     Adds a desktop pet cat that runs around and follows your mouse
// @version         1.2.1
// @author          ciizerr
// @github          https://github.com/ciizerr
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lwinmm -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🐱 Neko Cat
A cute desktop pet that follows your mouse and runs around your screen.
![Neko Cat Preview](https://raw.githubusercontent.com/ciizerr/wh-mods/2c1ecbf9ba9d0964e1a764a090cb2b7df729dc5c/previews/Neko-cat.gif)

## ✨ What's New in v1.2.0?
*   **Play With Window:** Neko can now climb the sides of your active window and pace along the title bar! He even reacts physically if you drag the window into him while he's sleeping.
*   **Multiple Cats:** Feeling lonely? You can now spawn an entire litter of cats! They have built-in separation physics so they won't just stack on top of each other.
*   **Neko Key:** Set a custom keyboard shortcut to instantly hide all your cats and mute their sounds when you need to focus.

## 🎮 How to Interact
*   **Left-Click:** Cycles through 6 different behaviors.
*   **Right-Click:** Wakes Neko up instantly if he's napping.
*   **Drag & Drop:** Pick him up and drop him! He'll yawn, take a quick nap, and then automatically wake up and go back to what he was doing.
*   **Window Nudging:** If Neko is exhausted and deeply asleep on the floor, try dragging a window edge into him. He'll slide out of the way and wake up to see what bumped him!

## 🏃 Cat Behaviors
1.  **Chase Mouse:** (Default) He follows your cursor everywhere!
2.  **Run Away:** Try to catch him if you can!
3.  **Random:** Total zoomies!
4.  **Pace:** He patrols the bottom of your screen.
5.  **Run Around:** He chases an invisible ball.
6.  **Play With Window:** He hunts down your currently focused window to climb the walls and pace the roof.

## 🖥️ Multi-Monitor Support
Neko can roam freely across **all your monitors**! He can seamlessly jump from one screen to another to follow your mouse.

## ⚙️ Customization
You can tweak almost everything in the settings: **Size**, **Speed**, **FPS**, **How many cats to spawn**, and your **Neko Key shortcut**. You can also turn his meows and snores on or off.

## 🔒 Assets & Transparency
Neko's graphics and sounds are downloaded automatically on the first run.
*   **Source:** All files come from the GitHub repository: [assets/neko-cat](https://github.com/ciizerr/wh-mods/tree/2c1ecbf9ba9d0964e1a764a090cb2b7df729dc5c/assets/neko-cat)
*   **Standard API:** The mod uses the trusted Windhawk `Wh_GetUrlContent` API.
*   **Local Storage:** Files are safely cached in your Windhawk `modstorage` folder.

## 📜 License & Credits
This mod uses implementations, logic, and state definitions referenced from the **Neko 98** source code.
*   **Original Author:** Masayuki Koba (X-Windows Neko)
*   **Windows Port (Neko 98):** David Harvey - [Neko98 Source on SourceForge](https://sourceforge.net/projects/neko98/)
*   **JavaScript Port (nekojs):** Louis Abraham - [louisabraham/nekojs on GitHub](https://github.com/louisabraham/nekojs)
*   **License:** Used in accordance with the Neko 98 source code license (freely usable as long as credit is given).

## 💬 Feedback & Support
Got ideas or found a bug? I’d love to hear from you!
*   **Discord:** `ciizerr`
*   **GitHub:** [wh-mods](https://github.com/ciizerr/wh-mods)

Enjoy your new friends!
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- scale: 2
  $name: Cat Size
  $description: Changes how big Neko is on your screen.
- speed: 24
  $name: Movement Speed
  $description: How fast Neko runs to follow your mouse.
- sound: true
  $name: Enable Sound
  $description: Turn all meows and snoring sounds on or off.
- sleep_sound_interval: 30
  $name: Sleep Snore Interval
  $description: How many seconds to wait between each snore.
- sleep_sound_repeat: true
  $name: Repeat Sleep Sound
  $description: If checked, Neko will keep snoring while he sleeps.
- fps: 60
  $name: Fluidity (FPS)
  $description: Smoothness of movement. Use 30 to save battery.
- cat_count: 1
  $name: Number of Cats
  $description: "How many cats do you want? (Max recommended: 10)"
- neko_key: "Ctrl+Alt+N"
  $name: Neko Key Shortcut
  $description: Keyboard shortcut to quickly hide/unhide Neko. (e.g. Ctrl+Alt+N, Shift+Esc, empty to disable)
*/
// ==/WindhawkModSettings==


#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#include <string>
#include <cmath>
#include <vector>

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

std::wstring g_assetPath = L"";
int g_scale = 2;
int g_speed = 24;
bool g_soundEnabled = true;
int g_sleepSoundInterval = 30;
bool g_sleepSoundRepeat = true;
int g_fps = 60;
int g_catCount = 1;
static bool g_modExit = false;

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
static HANDLE g_hWindowReady = nullptr;

struct SpriteConfig {
    const wchar_t* files[2];
};

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

void DownloadMissingAssets() {
    Wh_Log(L"Checking for missing assets in: %s", g_assetPath.c_str());
    CreatePath(g_assetPath);
    CreatePath(g_assetPath + L"\\sounds");

    std::wstring baseUrl = L"https://raw.githubusercontent.com/ciizerr/wh-mods/2c1ecbf9ba9d0964e1a764a090cb2b7df729dc5c/assets/neko-cat/";

    for (int i = 0; i < MAX_STATE; i++) {
        for (int f = 0; f < 2; f++) {
            std::wstring file = g_spriteConfigs[i].files[f];
            EnsureFileExists(g_assetPath + L"\\" + file, baseUrl + file);
        }
    }
    
    const wchar_t* audios[] = { L"awake.wav", L"sleep.wav", L"idle1.wav", L"idle2.wav", L"idle3.wav" };
    for (const wchar_t* au : audios) {
        std::wstring file(au);
        EnsureFileExists(g_assetPath + L"\\sounds\\" + file, baseUrl + L"sounds/" + file);
    }
}

class Neko {
public:
    HWND hwnd = NULL;
    Bitmap* sprites[MAX_STATE][2] = {};

    int behaviorMode = CHASE_MOUSE;
    int prevBehaviorMode = CHASE_MOUSE;  // restored after FORCED_SLEEP
    int idleThreshold = 6;
    NekoState state = STOP;
    int tickCount = 0;
    int stateCount = 0;
    
    double x = 0, y = 0;
    double logicX = 0, logicY = 0;
    double prevLogicX = 0, prevLogicY = 0;
    double targetX = 0, targetY = 0;
    double oldTargetX = 0, oldTargetY = 0;
    
    int moveDX = 0, moveDY = 0;
    int lastMoveDX = 0, lastMoveDY = 0;

    int virtualX = 0, virtualY = 0;
    int boundsWidth = 1920, boundsHeight = 1080;
    
    int mouseX = 0, mouseY = 0;
    bool hasMouseMoved = false;

    double tickAccumulator = 0;
    
    int cornerIndex = 0;
    double ballX = -9999, ballY = -9999;
    double ballVX = 0, ballVY = 0;
    int actionCount = 0;

    bool isDragging = false;
    ULONGLONG lastSleepSoundTime = 0;
    bool hasPlayedSleepSound = false;

    int offsetX = 0;
    int offsetY = 0;

    void LoadSprites() {
        for (int i = 0; i < MAX_STATE; i++) {
            for (int f = 0; f < 2; f++) {
                std::wstring path = g_assetPath + L"\\" + g_spriteConfigs[i].files[f];
                sprites[i][f] = Bitmap::FromFile(path.c_str());
                if (!sprites[i][f] || sprites[i][f]->GetLastStatus() != Ok) {
                    Wh_Log(L"Error loading sprite: %s", path.c_str());
                }
            }
        }
    }

    void PlayAudio(const wchar_t* file, bool loop) {
        if (!g_soundEnabled) return;
        std::wstring path = g_assetPath + L"\\sounds\\" + file;
        Wh_Log(L"Playing audio: %s", file);
        DWORD flags = SND_ASYNC | SND_FILENAME | SND_NODEFAULT;
        if (loop) flags |= SND_LOOP;
        PlaySoundW(path.c_str(), NULL, flags);
    }
    void StopAudio() {
        PlaySoundW(NULL, NULL, 0);
    }

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

    void Init() {
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

    static LRESULT CALLBACK NekoWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        Neko* pThis = (Neko*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (!pThis) return DefWindowProc(hwnd, msg, wp, lp);

        if (msg == WM_NCLBUTTONDOWN || msg == WM_LBUTTONDOWN) {
            if (pThis->behaviorMode != FORCED_SLEEP) {
                if (pThis->state == SLEEP) {
                    pThis->PlayAudio(L"awake.wav", false);
                } else {
                    const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                    pThis->PlayAudio(idles[rand() % 3], false);
                }
                pThis->SetState(AWAKE);
                pThis->CycleBehavior();
            }
            SetForegroundWindow(hwnd);
        } else if (msg == WM_NCRBUTTONDOWN || msg == WM_RBUTTONDOWN) {
            if (pThis->behaviorMode == FORCED_SLEEP || pThis->behaviorMode == EXHAUSTED_SLEEP) {
                pThis->behaviorMode = pThis->prevBehaviorMode;
                if (pThis->state == SLEEP || pThis->state == YAWN) {
                    pThis->PlayAudio(L"awake.wav", false);
                } else {
                    const wchar_t* idles[] = { L"idle1.wav", L"idle2.wav", L"idle3.wav" };
                    pThis->PlayAudio(idles[rand() % 3], false);
                }
                pThis->SetState(AWAKE);
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
            
            pThis->prevBehaviorMode = pThis->behaviorMode;
            pThis->behaviorMode = FORCED_SLEEP;
            Wh_Log(L"Neko dropped at %d, %d. Behavior: %d (%s)", 
                   (int)pThis->x, (int)pThis->y, pThis->behaviorMode, GetBehaviorName(pThis->behaviorMode));
            pThis->SetState(YAWN);
            pThis->oldTargetX = pThis->targetX = pThis->logicX + SPRITE_SIZE * g_scale / 2.0;
            pThis->oldTargetY = pThis->targetY = pThis->logicY + SPRITE_SIZE * g_scale;
        } else if (msg == WM_NCHITTEST) {
            LRESULT hit = DefWindowProc(hwnd, msg, wp, lp);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }
        return DefWindowProc(hwnd, msg, wp, lp);
    }

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
        Wh_Log(L"Behavior changed to: %d (%s)", behaviorMode, GetBehaviorName(behaviorMode));
        if (state == SLEEP) SetState(AWAKE);
    }

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

        // Smooth window collision resolution
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

        UpdateWindowPosition();
    }

    void ProcessOriginalTick() {
        tickCount++;
        if (tickCount >= 9999) tickCount = 0;
        if (tickCount % 2 == 0) stateCount++;

        if (logicY < virtualY + boundsHeight && (state == SLEEP || state == STOP || state == WASH || state == SCRATCH || state == YAWN) && behaviorMode == FORCED_SLEEP) {
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

    void ForcedSleep() {
        if ((int)logicY < virtualY + boundsHeight) {
            double fallSpeed = g_speed * 4.0 * g_scale;
            if (state != FALL) SetState(FALL);
            logicY = std::min(logicY + fallSpeed, (double)(virtualY + boundsHeight));
            moveDX = moveDY = 0;
        } else {
            // Landed — do the yawn->sleep sequence
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

    void PlayWithWindow() {
        int sz = SPRITE_SIZE * g_scale;
        double fallSpeed = g_speed * 4.0 * g_scale;
        double climbSpeed = g_speed * 1.0 * g_scale;

        RECT r;
        playWindowValid = GetPlayWindow(r);

        if (!playWindowValid) {
            // No window — fall to screen bottom
            if ((int)logicY < virtualY + boundsHeight) {
                if (state != FALL) SetState(FALL);
                logicY = std::min(logicY + fallSpeed, (double)(virtualY + boundsHeight));
            } else {
                if (state == FALL) SetState(STOP);
            }
            moveDX = moveDY = 0;
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
            if (state == FALL) SetState(STOP);
            if (moveDX == 0) moveDX = g_speed * g_scale;
            double newX = logicX + moveDX;
            if (newX < r.left)         { newX = r.left;        moveDX =  g_speed * g_scale; }
            if (newX + sz > r.right)   { newX = r.right - sz;  moveDX = -g_speed * g_scale; }
            logicX = newX;
            moveDY = 0;
            lastMoveDX = (int)moveDX; lastMoveDY = 0;
            CalcDirection(moveDX, 0);
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
            bool onFloor  = std::abs(logicY - (virtualY + boundsHeight)) < 2.0;
            bool atWall   = std::abs(logicX - wallX) < runSpeed + 2.0;

            if (climbSide != -1) { climbSide = -1; climbRetries = 0; }

            if (!atWall) {
                if (!onFloor) {
                    // Still falling to floor
                    if (state != FALL) SetState(FALL);
                    logicY = std::min(logicY + fallSpeed, (double)(virtualY + boundsHeight));
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
            bool onFloor  = std::abs(logicY - (virtualY + boundsHeight)) < 2.0;
            bool atWall   = std::abs(logicX - wallX) < runSpeed + 2.0;

            if (climbSide != 1) { climbSide = 1; climbRetries = 0; }

            if (!atWall) {
                if (!onFloor) {
                    if (state != FALL) SetState(FALL);
                    logicY = std::min(logicY + fallSpeed, (double)(virtualY + boundsHeight));
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
            bool onFloor = std::abs(logicY - (virtualY + boundsHeight)) < 2.0;

            if (!onFloor) {
                // Fall to floor first
                if (state != FALL) SetState(FALL);
                logicY = std::min(logicY + fallSpeed, (double)(virtualY + boundsHeight));
            } else {
                // On floor — move toward the nearer window edge
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

    void ChaseMouse() {
        if (!hasMouseMoved) {
            RunTowards(logicX + SPRITE_SIZE * g_scale / 2.0, logicY + SPRITE_SIZE * g_scale);
            return;
        }
        RunTowards(mouseX + offsetX, mouseY + offsetY);
    }

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

    void RunRandomly() {
        if (state != SLEEP) actionCount++;
        if (state != SLEEP && actionCount > idleThreshold * 20) {
            actionCount = 0;
            targetX = virtualX + rand() % (boundsWidth > 0 ? boundsWidth : 1);
            targetY = virtualY + rand() % (boundsHeight > 0 ? boundsHeight : 1);
            RunTowards(targetX, targetY);
        } else {
            RunTowards(targetX, targetY);
        }
    }

    void PaceAroundScreen() {
        if (lastMoveDX == 0 && lastMoveDY == 0) {
            cornerIndex = (cornerIndex + 1) % 4;
        }
        int sz = SPRITE_SIZE * g_scale;
        double corners[4][2] = {
            { (double)virtualX + sz/2.0, (double)virtualY + sz },
            { (double)virtualX + sz/2.0, (double)virtualY + boundsHeight + sz },
            { (double)virtualX + boundsWidth + sz/2.0, (double)virtualY + boundsHeight + sz },
            { (double)virtualX + boundsWidth + sz/2.0, (double)virtualY + sz }
        };
        RunTowards(corners[cornerIndex][0], corners[cornerIndex][1]);
    }

    void RunAround() {
        double bbox = g_speed * 8 * g_scale;
        if (ballX == -9999 && ballY == -9999) {
            ballX = virtualX + rand() % (boundsWidth - (int)bbox);
            ballY = virtualY + rand() % (boundsHeight - (int)bbox);
            ballVX = ((rand() % 2) ? 1 : -1) * (g_speed / 2.0) + 1;
            ballVY = ((rand() % 2) ? 1 : -1) * (g_speed / 2.0) + 1;
        }
        ballX += ballVX;
        ballY += ballVY;
        if (ballX < virtualX + bbox) {
            if (ballX > virtualX) ballVX++; else ballVX = -ballVX;
        } else if (ballX > virtualX + boundsWidth - bbox) {
            if (ballX < virtualX + boundsWidth) ballVX--; else ballVX = -ballVX;
        }
        if (ballY < virtualY + bbox) {
            if (ballY > virtualY) ballVY++; else ballVY = -ballVY;
        } else if (ballY > virtualY + boundsHeight - bbox) {
            if (ballY < virtualY + boundsHeight) ballVY--; else ballVY = -ballVY;
        }
        RunTowards(ballX, ballY);
    }

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
                    if (moveDX < 0 && logicX <= 0) SetState(L_CLAW);
                    else if (moveDX > 0 && logicX >= boundsWidth) SetState(R_CLAW);
                    else if (moveDY < 0 && logicY <= 0) SetState(U_CLAW);
                    else if (moveDY > 0 && logicY >= boundsHeight) SetState(D_CLAW);
                    else SetState(WASH);
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
                bool wasOutside = nx <= virtualX || nx >= virtualX + boundsWidth || ny <= virtualY || ny >= virtualY + boundsHeight;
                CalcDirection(moveDX, moveDY);
                
                nx = fmax((double)virtualX, fmin((double)virtualX + boundsWidth, nx));
                ny = fmax((double)virtualY, fmin((double)virtualY + boundsHeight, ny));
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
        int frameObj = 0;
        if (state == SLEEP) frameObj = (tickCount >> 2) & 1;
        else frameObj = tickCount & 1;

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

    for (int i = 0; i < g_catCount; ++i) {
        Neko* pNeko = new Neko();
        pNeko->Init();
        if (g_catCount > 1) pNeko->behaviorMode = rand() % 5;
        g_Nekos.push_back(pNeko);
    }
    for (size_t i = 0; i < g_Nekos.size(); ++i) {
        g_Nekos[i]->UpdateOffsets(i, g_Nekos.size());
    }
    
    // Track the overlay HWND for the tool mod
    if (!g_Nekos.empty()) g_hwndOverlay = g_Nekos[0]->hwnd;
    
    // Signal that the window is ready
    if (g_hWindowReady) {
        SetEvent(g_hWindowReady);
    }

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
            intervalMs = 1000 / (g_fps > 0 ? g_fps : 60);
            KillTimer(NULL, timerId);
            timerId = SetTimer(NULL, 1, intervalMs, UpdateAllCatsTimer);
            RegisterNekoKey();
            
            while ((int)g_Nekos.size() < g_catCount) {
                Neko* pNeko = new Neko();
                pNeko->Init();
                if (g_catCount > 1) pNeko->behaviorMode = rand() % 5;
                if (g_isHidden) ShowWindow(pNeko->hwnd, SW_HIDE);
                g_Nekos.push_back(pNeko);
            }
            while ((int)g_Nekos.size() > g_catCount) {
                Neko* pNeko = g_Nekos.back();
                g_Nekos.pop_back();
                pNeko->StopAudio();
                delete pNeko;
            }
            for (size_t i = 0; i < g_Nekos.size(); ++i) {
                g_Nekos[i]->UpdateOffsets(i, g_Nekos.size());
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
            Wh_Log(L"Neko Key toggled. IsHidden: %d", g_isHidden);
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
        g_assetPath = storagePath;
    }

    g_scale = Wh_GetIntSetting(L"scale");
    g_speed = Wh_GetIntSetting(L"speed");
    g_soundEnabled = Wh_GetIntSetting(L"sound") != 0;
    g_sleepSoundInterval = Wh_GetIntSetting(L"sleep_sound_interval");
    g_sleepSoundRepeat = Wh_GetIntSetting(L"sleep_sound_repeat") != 0;
    g_fps = Wh_GetIntSetting(L"fps");
    g_catCount = Wh_GetIntSetting(L"cat_count");
    if (g_catCount < 1) g_catCount = 1;

    PCWSTR nekoKeyStr = Wh_GetStringSetting(L"neko_key");
    if (nekoKeyStr) {
        g_nekoKeyStr = nekoKeyStr;
        Wh_FreeStringSetting(nekoKeyStr);
    } else {
        g_nekoKeyStr = L"";
    }
}

BOOL WhTool_ModInit()
{
    Wh_Log(L"WhTool_ModInit called");

    LoadSettings();

    g_hWindowReady = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_hWindowReady) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, NekoProcessThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_hWindowReady);
        return FALSE;
    }

    // Wait for the window to be created
    WaitForSingleObject(g_hWindowReady, 5000);
    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    LoadSettings();
    if (g_hThread) {
        PostThreadMessage(GetThreadId(g_hThread), WM_UPDATE_SETTINGS, 0, 0);
    }
    for (auto pNeko : g_Nekos) {
        // Force bounds recalculation
        pNeko->virtualX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        pNeko->virtualY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        pNeko->boundsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
        pNeko->boundsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN) - SPRITE_SIZE * g_scale;
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

    if (g_hWindowReady) {
        CloseHandle(g_hWindowReady);
        g_hWindowReady = nullptr;
    }
}

// ============================================================================
//  Tool mod launcher code (using Windhawk injection & hooking)
// ============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"Neko Cat: entry point hook triggered, exiting main thread.");
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