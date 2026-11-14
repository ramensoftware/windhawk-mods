// ==WindhawkMod==
// @id              context-converter
// @name            Context Converter
// @description     Adds a "Convert to" submenu for images, video and audio to the Explorer right-click menu
// @version         0.2.5f
// @author          Wdb1ue
// @github          https://github.com/Wdb1ue
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Context Converter

Adds **Convert to** to the right-click menu of image, video and audio files.
The converted file is saved next to the original (the original is kept).

## Requirements
Install ffmpeg and ImageMagick and make sure they are in PATH:

    winget install Gyan.FFmpeg
    winget install ImageMagick.ImageMagick

## Windows 11
The entries are in the classic menu ("Show more options"). To see them right
away, enable a Windhawk mod that restores the classic context menu.

## Notes
- Images are converted with ImageMagick (first frame / first page only),
  video and audio with ffmpeg.
- GIF files can be converted to images and to video.
- RAW photo formats and HEIC/AVIF/JXL depend on what your ImageMagick build supports.
- Converting video to an audio format extracts the audio track.

## How it works
The mod writes verbs to HKCU\Software\Classes (no admin rights needed) and a
helper script to %LOCALAPPDATA%\ContextConverter\convert.ps1.
Disabling the mod removes the menu entries.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <string>
#include <vector>

struct Group {
    const wchar_t* id;
    std::vector<const wchar_t*> inExts;
    std::vector<const wchar_t*> outFmts;
};

static const Group kGroups[] = {
    // Images (gif is in its own group because it can also become a video)
    {L"Image",
     {L".png", L".jpg", L".jpeg", L".jpe", L".jfif", L".webp", L".bmp", L".dib",
      L".tif", L".tiff", L".heic", L".heif", L".avif", L".ico", L".cur", L".tga",
      L".psd", L".svg", L".jp2", L".j2k", L".jxl", L".exr", L".hdr", L".pbm",
      L".pgm", L".ppm", L".pnm", L".pcx", L".dds", L".xpm",
      L".cr2", L".cr3", L".nef", L".arw", L".dng", L".orf", L".rw2", L".raf"},
     {L"jpg", L"jpeg", L"jpe", L"png", L"webp", L"avif", L"heic", L"bmp", L"tiff",
      L"gif", L"ico", L"tga", L"jp2", L"pdf"}},

    {L"Gif",
     {L".gif"},
     {L"jpg", L"jpeg", L"png", L"webp", L"avif", L"bmp", L"tiff", L"ico",
      L"mp4", L"webm", L"mkv", L"avi", L"mov"}},

    // Video (converting to an audio format extracts the audio track)
    {L"Video",
     {L".mp4", L".m4v", L".mov", L".mkv", L".avi", L".webm", L".wmv", L".asf",
      L".flv", L".f4v", L".mpg", L".mpeg", L".mpe", L".3gp", L".3g2", L".ts",
      L".mts", L".m2ts", L".vob", L".ogv", L".rm", L".rmvb", L".divx", L".mxf"},
     {L"mp4", L"mkv", L"webm", L"avi", L"mov", L"wmv", L"flv", L"mpg", L"ogv",
      L"m4v", L"3gp", L"ts", L"gif",
      L"mp3", L"wav", L"flac", L"ogg", L"opus", L"m4a", L"aac", L"wma", L"aiff",
      L"ac3", L"mka"}},

    {L"Audio",
     {L".mp3", L".wav", L".flac", L".ogg", L".oga", L".opus", L".m4a", L".m4b",
      L".aac", L".wma", L".aiff", L".aif", L".aifc", L".ape", L".wv", L".ac3",
      L".dts", L".mp2", L".mka", L".amr", L".au", L".caf", L".ra"},
     {L"mp3", L"wav", L"flac", L"ogg", L"opus", L"m4a", L"aac", L"wma", L"aiff",
      L"ac3", L"mka"}},
};

static const char kScript[] = R"PS(param([string]$Path, [string]$Fmt)
$ext = [IO.Path]::GetExtension($Path).TrimStart('.').ToLower()
$dir = [IO.Path]::GetDirectoryName($Path)
$name = [IO.Path]::GetFileNameWithoutExtension($Path)
$out = Join-Path $dir "$name.$Fmt"
$i = 1
while (Test-Path -LiteralPath $out) { $out = Join-Path $dir "$name ($i).$Fmt"; $i++ }

$img = 'png','jpg','jpeg','jpe','jfif','webp','bmp','dib','tif','tiff','heic','heif','avif','gif','ico','cur','tga','psd','svg','jp2','j2k','jxl','exr','hdr','pbm','pgm','ppm','pnm','pcx','dds','xpm','cr2','cr3','nef','arw','dng','orf','rw2','raf','pdf'
$audio = 'mp3','wav','flac','ogg','opus','m4a','aac','wma','aiff','ac3','mka'

if (($img -contains $ext) -and ($img -contains $Fmt) -and (Get-Command magick -ErrorAction SilentlyContinue)) {
    $a = @("$($Path)[0]")
    if ('jpg','jpeg','jpe','bmp' -contains $Fmt) { $a += '-background','white','-alpha','remove','-alpha','off' }
    if ($Fmt -eq 'ico') { $a += '-resize','256x256>' }
    $a += $out
    & magick @a
} else {
    $a = @('-hide_banner','-loglevel','error','-y','-i',$Path)
    if ($audio -contains $Fmt) {
        $a += '-vn'
        if ($Fmt -eq 'mp3') { $a += '-q:a','2' }
    }
    if ($Fmt -eq 'gif') {
        $a += '-vf','fps=15,scale=480:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse'
    }
    $a += $out
    & ffmpeg @a
}
)PS";

static bool SetStr(const std::wstring& subkey, const wchar_t* name, const std::wstring& val) {
    HKEY k;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, subkey.c_str(), 0, nullptr, 0,
                        KEY_WRITE, nullptr, &k, nullptr) != ERROR_SUCCESS) {
        return false;
    }
    LONG r = RegSetValueExW(k, name, 0, REG_SZ,
                            reinterpret_cast<const BYTE*>(val.c_str()),
                            static_cast<DWORD>((val.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(k);
    return r == ERROR_SUCCESS;
}

static void WriteScript(const std::wstring& path) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Failed to write helper script");
        return;
    }
    DWORD written;
    WriteFile(h, kScript, static_cast<DWORD>(sizeof(kScript) - 1), &written, nullptr);
    CloseHandle(h);
}

static std::wstring ParentKey(const wchar_t* ext) {
    return std::wstring(L"Software\\Classes\\SystemFileAssociations\\") + ext +
           L"\\shell\\ContextConverter";
}

static std::wstring GroupKey(const wchar_t* id) {
    return std::wstring(L"Software\\Classes\\ContextConverter.") + id;
}

BOOL Wh_ModInit() {
    wchar_t dir[MAX_PATH];
    ExpandEnvironmentStringsW(L"%LOCALAPPDATA%\\ContextConverter", dir, MAX_PATH);
    CreateDirectoryW(dir, nullptr);
    std::wstring script = std::wstring(dir) + L"\\convert.ps1";
    WriteScript(script);

    for (const Group& g : kGroups) {
        std::wstring grp = GroupKey(g.id);

        int n = 0;
        for (const wchar_t* fmt : g.outFmts) {
            std::wstring sub = grp + L"\\shell\\" + std::to_wstring(100 + n++) + L"_" + fmt;

            std::wstring label = fmt;
            CharUpperBuffW(&label[0], static_cast<DWORD>(label.size()));
            SetStr(sub, L"MUIVerb", label);

            std::wstring cmd =
                L"powershell.exe -NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass "
                L"-File \"" + script + L"\" \"%1\" " + fmt;
            SetStr(sub + L"\\command", nullptr, cmd);
        }

        for (const wchar_t* ext : g.inExts) {
            std::wstring parent = ParentKey(ext);
            SetStr(parent, L"MUIVerb", L"Convert to");
            SetStr(parent, L"ExtendedSubCommandsKey",
                   std::wstring(L"ContextConverter.") + g.id);
        }
    }
    return TRUE;
}

void Wh_ModUninit() {
    for (const Group& g : kGroups) {
        for (const wchar_t* ext : g.inExts) {
            RegDeleteTreeW(HKEY_CURRENT_USER, ParentKey(ext).c_str());
        }
        RegDeleteTreeW(HKEY_CURRENT_USER, GroupKey(g.id).c_str());
    }
}
