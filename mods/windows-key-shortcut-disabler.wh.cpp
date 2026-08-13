// ==WindhawkMod==
// @id                 windows-key-shortcut-disabler
// @name               Windows Key Shortcut Disabler (Win+V, Win+W, Win+Q, and more!)
// @name:ja-JP         Windowsキーショートカット無効化（Win+V、Win+W、Win+Qなど）
// @description        Disables Win+V, Win+W, Win+Q, and custom Win+* keyboard shortcuts. ※ explorer.exe must be restarted for changes to take effect.
// @description:ja-JP  Win+V、Win+W、Win+Q、およびカスタムのWin+〇〇キーボードショートカットを無効化します。※変更を反映するにはexplorer.exeの再起動が必要です。
// @version            1.0
// @author             Karen/あけみ (akemin_dayo)
// @github             https://github.com/akemin-dayo
// @twitter            https://twitter.com/akemin_dayo
// @homepage           https://akemi.ai/
// @license            MIT
// @include            explorer.exe
// @architecture       x86-64
// @architecture       x86
// @architecture       arm64
// ==/WindhawkMod==

// ******************************************************************************************************************************** //

// ==WindhawkModReadme==
/*
**【※日本語は英語の下に掲載しています。 Japanese version can be found below.】**
# Windows Key Shortcut Disabler
###### Win+V, Win+W, Win+Q, and more!
If you're like me and frequently switch between macOS and Windows, you've probably accidentally pressed Win+V (Cmd+V) sometimes when you're not thinking, and then have to manually close the annoying little popup that comes up when you have Clipboard/Pasteboard History disabled.

To make matters worse, said popup can't be focused using Alt+Tab then Alt+F4, so you have to manually close it with the cursor.

This tweak/mod fixes that. It also allows you to define additional keys you want to disable, just in case you don't like Win+P, or if you just _really_ hate the notification centre (Win+A) or clock (Win+C) or something like that.

Supports 64-bit and 32-bit Intel/AMD, as well as ARM. (※ `x86_64`, `i386`, `arm64`)

**※ ⚠️ IMPORTANT:** Due to how this tweak/mod works, `explorer.exe` **must** be restarted in order for any changes to actually take effect. (※ It prevents Explorer from registering the configured keyboard shortcuts, which only happens once during Explorer initialisation.)

## Presets (※ all enabled by default)
* Win+V (Clipboard/Pasteboard History)
* Win+W (Windows Ink/Widgets)
* Win+Q (Search/Copilot)
	* ※ If you want an alternative keyboard shortcut to specifically open Search for some reason (instead of just typing in the start menu), you can use Win+S.

## Defining custom shortcuts to disable
Don't like some other Windows key combination for some reason?

Simply add any other keys you don't like (※ A-Z, 0-9) in the Windhawk preference pane, separated by commas or spaces. (※ Case-insensitive.)

Valid examples:
* `C`
* `C, A`
* `C A`
* `c a`
* `0,1,2,3,4,5,6,7,8,9`
* `0 1 2 3 4 5 6 7 8 9`

## Help! It's not working!
Due to how this tweak/mod works, `explorer.exe` must be restarted in order for any changes to actually take effect. (※ It prevents Explorer from registering the configured keyboard shortcuts, which only happens once during Explorer initialisation.)

If you don't know how to do that: Press Win+R to open the Run dialog, or open Terminal/Command Prompt, then paste this in, then press Enter/Return:
```cmd
cmd /c "taskkill -f -im explorer.exe && start explorer"
```

If you're not comfortable running that, you can achieve the same thing by logging out and logging back in, or by rebooting.

## Special thanks
This tweak/mod uses the same method selector hook/injection point and similar methodology as Fenig's "[Block Win+V](https://windhawk.net/mods/block-win-v)" (※ [MIT License](https://github.com/ramensoftware/windhawk-mods/blob/453bb94231c97af750e51e2584e73991074cb4fc/README.md?plain=1#L21)).

## License
Licensed under the [MIT License](https://opensource.org/licenses/MIT).

---

**【※ English version can be found above. 英語は日本語の上に掲載しています。】**
# Windowsキーショートカット無効化
###### Win+V、Win+W、Win+Qなど
macOSとWindowsをよく切り替えて使っていると、うっかりWin+V（Cmd+V）を押してしまうことはありますか？クリップボード・ペーストボード履歴機能を無効にしていても、「クリップボード・ペーストボード履歴」のウィンドウが表示されます。

しかも、あのウィンドウはAlt+Tabでフォーカスできず、Alt+F4でも閉じられないので、毎回マウスで閉じる必要があります。

このtweak/modはその問題を解決します。また、無効化したいWindowsキーのショートカットを自由に追加できます。

対応CPUアーキテクチャ: 64ビットおよび32ビットIntel/AMDとARM（※`x86_64`、`i386`、`arm64`）

**※ ⚠️ 重要:** このtweak/modの仕組み上、設定を反映するには`explorer.exe`の**再起動が必須です。**（※このtweak/modはExplorerが指定されたキーボードショートカットを登録しないようにします。Explorerは起動時にしかキーボードショートカットを登録しないため、設定を反映するには再起動が必要です。）

## プリセット（※ すべて既定で有効）
* Win+V（クリップボード・ペーストボード履歴）
* Win+W（Windows Ink、ウィジェット）
* Win+Q（検索、Copilot）
	* ※検索を開くショートカットが必要な場合は、代わりにWin+Sを使用できます。

## 無効化するWin+◯◯ショートカットを追加で指定する
Windhawkの環境設定で、無効化したいキー（※ A～Z、0～9）をカンマ（`,`）または**半角**スペース（` `）区切りで入力するだけです。（※大文字と小文字は区別されません。）

例：
* `C`
* `C, A`
* `C A`
* `c a`
* `0,1,2,3,4,5,6,7,8,9`
* `0 1 2 3 4 5 6 7 8 9`

## 動作しません！
このtweak/modの仕組み上、設定を反映するには`explorer.exe`の**再起動が必須です。**（※このtweak/modはExplorerが指定されたキーボードショートカットを登録しないようにします。Explorerは起動時にしかキーボードショートカットを登録しないため、設定を反映するには再起動が必要です。）

方法がわからない場合は、Win+Rで「ファイル名を指定して実行」を開くか、ターミナルまたはコマンドプロンプトを開き、以下を貼り付けてEnter/Returnキーを押してください。

```cmd
cmd /c "taskkill -f -im explorer.exe && start explorer"
```

コマンドを実行したくない場合は、サインアウトして再度サインインするか、Windowsを再起動しても同じ効果が得られます。

## Special thanks
このtweak/modはFenigさんの「Block Win+V」と同じメソッドセレクタhookおよびインジェクションポイントを使用し、類似した手法を採用しています。（※ [MIT License](https://github.com/ramensoftware/windhawk-mods/blob/453bb94231c97af750e51e2584e73991074cb4fc/README.md?plain=1#L21)）

## ライセンス
[MIT License](https://opensource.org/licenses/MIT)の下で公開されています。
*/
// ==/WindhawkModReadme==

// ******************************************************************************************************************************** //

// ==WindhawkModSettings==
/*
- shouldDisableWinV: true
  $name: "Disable Win+V"
  $name:ja-JP: "Win+Vを無効化"
  $description: "Disables the Clipboard/Pasteboard History (Win+V) keyboard shortcut."
  $description:ja-JP: "クリップボード・ペーストボード履歴（Win+V）のキーボードショートカットを無効にします。"

- shouldDisableWinW: true
  $name: "Disable Win+W"
  $name:ja-JP: "Win+Wを無効化"
  $description: "Disables the Windows Ink/Widgets (Win+W) keyboard shortcut."
  $description:ja-JP: "Windows Ink、ウィジェット（Win+W）のキーボードショートカットを無効にします。"

- shouldDisableWinQ: true
  $name: "Disable Win+Q"
  $name:ja-JP: "Win+Qを無効化"
  $description: "Disables the Search/Copilot (Win+Q) keyboard shortcut. (※ If you want an alternative keyboard shortcut to specifically open Search for some reason, you can use Win+S.)"
  $description:ja-JP: "検索、Copilot（Win+Q）のキーボードショートカットを無効にします。（※検索を開くショートカットが必要な場合は、代わりにWin+Sを使用できます。）"

- userDefinedWinKeyExclusions: ""
  $name: "Custom entries"
  $name:ja-JP: "カスタム項目"
  $description: "Enter any other key(s) (※ A-Z, 0-9) you want to disable with Windows Key, separated by commas or spaces, case insensitive."
  $description:ja-JP: "Windowsキーとの組み合わせで無効にしたいキー（※A～Z、0～9）を、カンマ（,）または半角スペース（ ）で区切って入力してください。大文字と小文字は区別されません。"
*/
// ==/WindhawkModSettings==

// ******************************************************************************************************************************** //

// AkemiWindowsKeyShortcutDisabler
// Intended to be built with Windhawk's MinGW LLVM/Clang/LLD-based toolchain. (※ https://github.com/mstorsjo/llvm-mingw)

// Requires C++20.
// clang: -std=c++20

// As of version 1.7, Windhawk uses Clang 20 (mingw-w64 toolchain) and compiles the mods in C++23 mode.
// ※ The full command line parameters can be seen in editing mode by clicking Ctrl+P and selecting `compile_flags.txt`.

// ******************************************************************************************************************************** //

#include <cwctype>
#include <unordered_set>

// ******************************************************************************************************************************** //

#define AkemiLog(format, ...) Wh_Log(L"[AkemiWindowsKeyShortcutDisabler] [%S] [L%d] " format, __func__, __LINE__, ##__VA_ARGS__)
#define AkemiWarning(format, ...) Wh_Log(L"[AkemiWindowsKeyShortcutDisabler] [%S] [L%d] [WARNING] " format, __func__, __LINE__, ##__VA_ARGS__)
#define AkemiError(format, ...) Wh_Log(L"[AkemiWindowsKeyShortcutDisabler] [%S] [L%d] [ERROR] " format, __func__, __LINE__, ##__VA_ARGS__)

// ******************************************************************************************************************************** //

struct AkemiPreferences {
	bool shouldDisableWinV = true;
	bool shouldDisableWinW = true;
	bool shouldDisableWinQ = true;
	std::unordered_set<UINT> userDefinedWinKeyExclusions;
};

AkemiPreferences akemiPreferences;

void initialiseAndLoadPreferences() {
	AkemiLog("Initialising preferences…");

	akemiPreferences.shouldDisableWinV = Wh_GetIntSetting(L"shouldDisableWinV");
	akemiPreferences.shouldDisableWinW = Wh_GetIntSetting(L"shouldDisableWinW");
	akemiPreferences.shouldDisableWinQ = Wh_GetIntSetting(L"shouldDisableWinQ");
	AkemiLog("shouldDisableWinV → %d", akemiPreferences.shouldDisableWinV);
	AkemiLog("shouldDisableWinW → %d", akemiPreferences.shouldDisableWinW);
	AkemiLog("shouldDisableWinQ → %d", akemiPreferences.shouldDisableWinQ);

	akemiPreferences.userDefinedWinKeyExclusions.clear();

	PCWSTR userDefinedWinKeyExclusionsString = Wh_GetStringSetting(L"userDefinedWinKeyExclusions");
	if (!userDefinedWinKeyExclusionsString) {
		// This should never happen. Windhawk preferences initialises to an empty string, not nil.
		AkemiError("userDefinedWinKeyExclusionsString → nil");
		return;
	}
	AkemiLog("userDefinedWinKeyExclusionsString → %s", userDefinedWinKeyExclusionsString);

	// Sanitise keys to allow only A-Z, 0-9, and normalise to uppercase.
	for (const wchar_t* iteratedUserDefinedKey = userDefinedWinKeyExclusionsString; *iteratedUserDefinedKey; iteratedUserDefinedKey++) {
		wchar_t sanitisedUserDefinedKey = std::towupper(*iteratedUserDefinedKey);
		if ((sanitisedUserDefinedKey >= L'A' && sanitisedUserDefinedKey <= L'Z') || (sanitisedUserDefinedKey >= L'0' && sanitisedUserDefinedKey <= L'9')) {
			akemiPreferences.userDefinedWinKeyExclusions.insert(sanitisedUserDefinedKey);
		}
	}

	Wh_FreeStringSetting(userDefinedWinKeyExclusionsString);

	AkemiLog("Preferences initialised!");
	return;
}

// ******************************************************************************************************************************** //

BOOL(WINAPI* orig_registerHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL WINAPI hook_registerHotKey(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
	// All Win+whatever shortcuts are MOD_NOREPEAT as far as I can tell.
	if (fsModifiers == (MOD_WIN | MOD_NOREPEAT)) {
		AkemiLog("Explorer is attempting to register key combination Win+%lc (0x%02x)…", static_cast<wchar_t>(vk), vk);
		if (
			(vk == 'V' && akemiPreferences.shouldDisableWinV) ||
			(vk == 'W' && akemiPreferences.shouldDisableWinW) ||
			(vk == 'Q' && akemiPreferences.shouldDisableWinQ) ||
			(akemiPreferences.userDefinedWinKeyExclusions.contains(vk))
		) {
			AkemiLog("Preventing Explorer from registering key combination Win+%lc (0x%02x)…", static_cast<wchar_t>(vk), vk);
			SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
			return false;
		}
		AkemiLog("Looks like everything's fine, calling original method selector implementation…");
	} else {
		AkemiLog("Calling original method selector implementation for Explorer requested key combination involving key %lc (%u)…", static_cast<wchar_t>(vk), vk);
	}
	return orig_registerHotKey(hWnd, id, fsModifiers, vk);
}

// ******************************************************************************************************************************** //

void initialiseHooksAndSwizzleMethodSelectors() {
	AkemiLog("Initialising hooks and preparing to swizzle method selectors…");

	HMODULE user32Image = GetModuleHandleW(L"user32.dll");
	if (!user32Image) {
		AkemiError("Failed to get user32.dll image!");
		return;
	}

	void* user32RegisterHotKeySymbol = (void*)GetProcAddress(user32Image, "RegisterHotKey");
	if (!user32RegisterHotKeySymbol) {
		AkemiError("Failed to get method selector symbol for RegisterHotKey() in image user32.dll!");
		return;
	}

	AkemiLog("Hooking and swizzling method selector RegisterHotKey()…");
	if (!Wh_SetFunctionHook(user32RegisterHotKeySymbol, (void*)hook_registerHotKey, (void**)&orig_registerHotKey)) {
		AkemiError("Failed to hook and swizzle method selector RegisterHotKey()!");
		return;
	}

	AkemiLog("Hooks initialised, method selector RegisterHotKey() successfully swizzled!");
	return;
}

// ******************************************************************************************************************************** //

void Wh_ModSettingsChanged() {
	AkemiLog("Preferences were updated! Reinitialising preferences…");
	initialiseAndLoadPreferences();
	AkemiLog("Preferences reinitialised! An Explorer restart is required, though…");
	return;
}

BOOL Wh_ModInit() {
	AkemiLog("Tweak successfully injected!");
	initialiseAndLoadPreferences();
	initialiseHooksAndSwizzleMethodSelectors();
	AkemiLog("Tweak initialisation complete!");
	return true;
}
