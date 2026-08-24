// ==WindhawkMod==
// @id              explorer-status-metadata
// @name            Explorer Status Bar Metadata
// @description     Appends rich metadata (dimensions, date, type, duration, bitrate, fps) to Explorer's status bar. Zero polling — purely reactive, path-keyed cache.
// @version         1.4.0
// @author          VitalS
// @github          https://github.com/VitalSkib
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lpropsys -luuid
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- allowNetworkDrives: false
  $name: Enable metadata on network drives (UNC paths)
  $description: >
    WARNING: May cause Explorer to freeze for several seconds if the network
    is slow or unavailable. The metadata lookup is synchronous on the UI thread.
    Only enable this if you are on a fast, reliable local network.

- allowRemovableDrives: true
  $name: Enable metadata on removable drives (USB, SD cards)
  $description: >
    Metadata lookup on slow removable media (e.g. old USB drives) may cause
    a brief UI stutter. Fast USB 3.x drives are generally fine.

- cacheEvictCount: 10
  $name: Cache eviction count
  $description: >
    When the cache reaches 1000 entries, this many of the oldest entries
    are removed. Range: 1-50. Lower values preserve more history;
    higher values free more memory at once.
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Explorer Status Bar Metadata — v1.4.0

Appends file metadata to the Windows Explorer status bar when a single file
is selected: dimensions, modification date, file type, duration, bitrate, fps.

## How it works
- No timers, no polling. Metadata is computed only when the selected file changes.
- Path-keyed LRU-style cache (1000 entries). Same file = instant return, zero COM calls.
- Hooks `PSFormatForDisplayAlloc` in propsys.dll — the exact function Explorer calls
  to format the file size for the status bar. Language-independent, no text matching.
- Filter: `key == System.Size` AND `SHELL32.dll` appears in the first 5 call stack
  frames (status bar path), but NOT `explorerframe.dll` (Details column — skipped).
- Compatible with "Better file sizes in Explorer details" — multi-frame stack scan
  handles chained hooks transparently.
- Custom binary parsers for formats the Windows Property System does not index:
  SVG, HDR, EXR, TIFF/TX (LE+BE), TGA, PSD/PSB, WebM/MKV (EBML).

## Settings
- **Network drives** — disabled by default. Can freeze Explorer on slow networks.
- **Removable drives** — enabled by default. Disable if you use slow USB media.
- **Cache evict count** — how many old entries to drop when the 1000-entry limit is hit.

## Changelog
### v1.4.0
- Replaced DrawTextW/GetTextExtentPoint32W hooks with PSFormatForDisplayAlloc hook
  (language-independent, no text matching, no "selected"-string false positives)
- SHELL32.dll stack filter (5 frames) precisely identifies the status bar call
- Selection path discovery uses IFolderView2::GetSelection — same-thread, no
  CWM_GETISHELLBROWSER broadcast to arbitrary windows
- CWM_GETISHELLBROWSER is now sent only to windows of known safe classes:
  CabinetWClass and ShellTabWindowClass
- SafeGetShellBrowser: thread-ID guard replaced with process-ID guard.
  PSFormatForDisplayAlloc fires on a shell worker thread (not the GUI thread),
  so the thread-ID check always failed and GetSelectedFilePath always returned
  empty. Process-ID check is sufficient and works correctly from any thread.
- Added Wh_Log to Wh_ModInit/Wh_ModUninit for basic injection diagnostics
- Added Wh_ModSettingsChanged so settings take effect without restarting Explorer
- Cache upgraded from FIFO to true LRU: cache hits now move the entry to the
  back of the eviction queue, keeping frequently-accessed files safe from eviction
- SVG parser rewritten with ExtractAttribute helper: tolerant of whitespace
  around '=' and both quote styles (viewBox = "...", width='100px', etc.)
- Added PSD/PSB binary parser (width/height from fixed 26-byte header)
- File type moved to last position (right edge) for all file types
- CleanTypeString strips spurious ".<digits>" suffix from type names (Windows
  shell quirk: "Adobe Photoshop Image.27" → "Adobe Photoshop Image")
- PSD/PSB type label overridden to "Adobe Photoshop File" for consistency
- Removed cross-window fallback (Strategy 3): GetSelectedFilePath is strictly
  scoped to the root window identified by GetGUIThreadInfo / GetForegroundWindow
  — no risk of reading a selection from a different Explorer window

### v1.3.0
- Initial release
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <propsys.h>
#include <shlobj.h>
#include <shlguid.h>
#include <propkey.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>

// ─── Undocumented message to obtain IShellBrowser from a window ──────────────
#define CWM_GETISHELLBROWSER (WM_USER + 7)

// ─── PSFormatForDisplayAlloc hook ────────────────────────────────────────────
typedef HRESULT (WINAPI *PSFormatForDisplayAlloc_t)(
    REFPROPERTYKEY, REFPROPVARIANT, PROPDESC_FORMAT_FLAGS, PWSTR*);
static PSFormatForDisplayAlloc_t g_origPSF = nullptr;

// ─── System.Size PKEY ────────────────────────────────────────────────────────
static const PROPERTYKEY kPKEY_Size = {
    { 0xB725F130,0x47EF,0x101A,{0xA5,0xF1,0x02,0x60,0x8C,0x9E,0xEB,0xAC} }, 12 };

static bool IsSystemSize(REFPROPERTYKEY k) {
    return k.pid == kPKEY_Size.pid && IsEqualGUID(k.fmtid, kPKEY_Size.fmtid);
}

// ─── Settings ────────────────────────────────────────────────────────────────
static bool   g_allowNetworkDrives   = false;
static bool   g_allowRemovableDrives = true;
static size_t g_cacheEvictCount      = 10;

// ─── Metadata cache ──────────────────────────────────────────────────────────
// unordered_map for O(1) lookup + vector to track insertion order for eviction.
static CRITICAL_SECTION g_cs;
static std::unordered_map<std::wstring, std::wstring> g_metaCache;
static std::vector<std::wstring>                      g_cacheOrder;
static const size_t MAX_CACHE_SIZE = 1000;

// ─── Binary structures for TIFF/TX parser ────────────────────────────────────
#pragma pack(push, 1)
struct TiffHeader { WORD magic; WORD version; DWORD ifdOffset; };
struct TiffTag    { WORD tagId; WORD tagType; DWORD count; DWORD valueOffset; };
#pragma pack(pop)

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 1: Custom format binary parsers
//   Covers formats the Windows Property System does not index natively:
//   WebM/MKV (EBML), TIFF/TX (LE+BE), SVG, HDR, EXR, TGA, PSD/PSB, AI.
// ═══════════════════════════════════════════════════════════════════════════════

struct CustomMeta {
    int    width  = 0;
    int    height = 0;
    double fps    = 0.0;
};

static CustomMeta ParseCustomFormatMetadata(const std::wstring& filePath, const std::wstring& ext)
{
    CustomMeta meta;

    HANDLE hFile = CreateFileW(
        filePath.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
        return meta;

    DWORD bytesRead = 0;

    // ── WebM / MKV (EBML/Matroska) ───────────────────────────────────────────
    // Flat streaming parse: containers (Segment, Info, Tracks, TrackEntry, Video)
    // are entered by letting the loop continue into their bodies without skipping.
    // Leaf elements (PixelWidth, PixelHeight, DefaultDuration) are read directly.
    if (ext == L"webm" || ext == L"mkv") {
        unsigned char buf[16384];
        if (ReadFile(hFile, buf, sizeof(buf), &bytesRead, nullptr) && bytesRead > 16) {
            const unsigned char* p   = buf;
            const unsigned char* end = buf + bytesRead;

            // Reads a variable-length EBML ID (1–4 bytes, leading 1-bit signals length)
            auto readID = [&]() -> uint32_t {
                if (p >= end) return 0;
                uint8_t first = *p;
                if (first == 0) return 0;
                int     len  = 1;
                uint8_t mask = 0x80;
                while (!(first & mask) && len <= 4) { mask >>= 1; len++; }
                if (p + len > end) return 0;
                uint32_t val = 0;
                for (int i = 0; i < len; ++i) val = (val << 8) | *p++;
                return val;
            };

            // Reads a variable-length EBML data size (1–8 bytes).
            // Returns (uint64_t)-1 for "unknown size" (all data bits set).
            auto readSize = [&]() -> uint64_t {
                if (p >= end) return 0;
                uint8_t first = *p;
                if (first == 0) return 0;
                int     len  = 1;
                uint8_t mask = 0x80;
                while (!(first & mask) && len <= 8) { mask >>= 1; len++; }
                if (p + len > end) return 0;
                uint64_t val         = (*p++) & ~mask;
                for (int i = 1; i < len; ++i) val = (val << 8) | *p++;
                uint64_t unknownSize = (1ULL << (7 * len)) - 1;
                if (val == unknownSize) return (uint64_t)-1;
                return val;
            };

            // Reads an unsigned integer from the next `size` bytes (big-endian)
            auto readUint = [&](uint64_t size) -> uint64_t {
                if (size > 8 || p + size > end) return 0;
                uint64_t val = 0;
                for (uint64_t i = 0; i < size; ++i) val = (val << 8) | *p++;
                return val;
            };

            while (p < end) {
                uint32_t id   = readID();
                if (id == 0) break;
                uint64_t size = readSize();

                // Handle unknown-size and truncated-in-buffer containers
                if (size == (uint64_t)-1) {
                    if (id == 0x18538067 /*Segment*/) size = (uint64_t)(end - p);
                    else break;
                } else if (p + size > end) {
                    // Container body extends past our read buffer — clamp and enter anyway
                    if (id == 0x18538067 /*Segment*/  || id == 0x1549A966 /*Info*/    ||
                        id == 0x1654AE6B /*Tracks*/   || id == 0xAE /*TrackEntry*/    ||
                        id == 0xE0       /*Video*/)
                        size = (uint64_t)(end - p);
                    else
                        break;
                }

                if (id == 0x18538067 || id == 0x1549A966 || id == 0x1654AE6B ||
                    id == 0xAE        || id == 0xE0) {
                    // Container: do not skip, let the loop parse the body directly
                    continue;
                } else if (id == 0xB0) {           // PixelWidth
                    meta.width  = (int)readUint(size);
                } else if (id == 0xBA) {           // PixelHeight
                    meta.height = (int)readUint(size);
                } else if (id == 0x23E383) {       // DefaultDuration (nanoseconds)
                    uint64_t defDur = readUint(size);
                    if (defDur > 0) meta.fps = 1000000000.0 / (double)defDur;
                } else {
                    // Guard: size == 0 means empty element body.
                    // Without this break, p would not advance → infinite loop.
                    if (size == 0) break;
                    p += size;
                }

                if (meta.width > 0 && meta.height > 0 && meta.fps > 0.0) break;
            }
        }
    }
    // ── TIFF / TX ─────────────────────────────────────────────────────────────
    // Supports both Little-Endian (II, magic=42) and Big-Endian (MM, magic=42 BE).
    else if (ext == L"tx" || ext == L"tif" || ext == L"tiff") {
        TiffHeader head;
        if (ReadFile(hFile, &head, sizeof(head), &bytesRead, nullptr)
            && bytesRead == sizeof(head))
        {
            bool isLE = (head.magic == 0x4949 && head.version == 42);
            bool isBE = (head.magic == 0x4D4D && head.version == 0x2A00);

            if (isLE || isBE) {
                DWORD ifdOffset = isLE ? head.ifdOffset : _byteswap_ulong(head.ifdOffset);
                if (SetFilePointer(hFile, ifdOffset, nullptr, FILE_BEGIN)
                    != INVALID_SET_FILE_POINTER)
                {
                    WORD numTagsRaw = 0;
                    ReadFile(hFile, &numTagsRaw, sizeof(numTagsRaw), &bytesRead, nullptr);
                    WORD numTags = isLE ? numTagsRaw : _byteswap_ushort(numTagsRaw);

                    int width = 0, height = 0;
                    for (WORD i = 0; i < numTags && !(width && height); ++i) {
                        TiffTag tag;
                        if (!ReadFile(hFile, &tag, sizeof(tag), &bytesRead, nullptr)) break;

                        WORD  tagId   = isLE ? tag.tagId       : _byteswap_ushort(tag.tagId);
                        WORD  tagType = isLE ? tag.tagType     : _byteswap_ushort(tag.tagType);
                        DWORD valOff  = isLE ? tag.valueOffset : _byteswap_ulong(tag.valueOffset);

                        if      (tagId == 0x0100) width  = (tagType == 3) ? (int)(valOff & 0xFFFF) : (int)valOff;
                        else if (tagId == 0x0101) height = (tagType == 3) ? (int)(valOff & 0xFFFF) : (int)valOff;
                    }
                    if (width > 0 && height > 0) { meta.width = width; meta.height = height; }
                }
            }
        }
    }
    // ── SVG ───────────────────────────────────────────────────────────────────
    // Priority: viewBox (logical canvas size). Fallback: explicit width/height
    // on the root <svg> tag only.
    //
    // ExtractAttribute handles whitespace around '=' and both quote styles,
    // so `viewBox = "..."`, `width='100px'`, etc. all parse correctly.
    // Search is anchored inside the opening <svg ... > tag to avoid false
    // matches on nested elements such as <symbol width=...>.
    else if (ext == L"svg") {
        char buf[2048] = {0};
        if (ReadFile(hFile, buf, sizeof(buf) - 1, &bytesRead, nullptr) && bytesRead > 0) {
            std::string content(buf, bytesRead);

            // Locate the root <svg tag and its closing '>'
            size_t svgTagPos = content.find("<svg");
            if (svgTagPos == std::string::npos) svgTagPos = 0;
            size_t svgTagEnd = content.find('>', svgTagPos);
            if (svgTagEnd == std::string::npos) svgTagEnd = content.size();

            // Helper: find attribute NAME inside [svgTagPos, svgTagEnd),
            // return its value string regardless of whitespace or quote style.
            // Returns empty string if not found.
            auto ExtractAttribute = [&](const std::string& attrName) -> std::string {
                size_t pos = svgTagPos;
                while (pos < svgTagEnd) {
                    size_t found = content.find(attrName, pos);
                    if (found == std::string::npos || found >= svgTagEnd) break;
                    // Skip whitespace after attribute name
                    size_t eq = found + attrName.size();
                    while (eq < svgTagEnd && content[eq] == ' ') ++eq;
                    if (eq >= svgTagEnd || content[eq] != '=') { pos = found + 1; continue; }
                    ++eq; // skip '='
                    while (eq < svgTagEnd && content[eq] == ' ') ++eq;
                    if (eq >= svgTagEnd) break;
                    char q = content[eq];
                    if (q != '"' && q != '\'') { pos = found + 1; continue; }
                    ++eq; // skip opening quote
                    size_t qEnd = content.find(q, eq);
                    if (qEnd == std::string::npos) break;
                    return content.substr(eq, qEnd - eq);
                }
                return "";
            };

            // 1. viewBox="minX minY width height"
            std::string vb = ExtractAttribute("viewBox");
            if (!vb.empty()) {
                float x, y, w, h;
                if (sscanf_s(vb.c_str(), "%f %f %f %f", &x, &y, &w, &h) == 4
                    && w > 0 && h > 0) {
                    meta.width  = (int)w;
                    meta.height = (int)h;
                }
            }
            // 2. Fallback: explicit width / height (ignore unit suffixes: px, pt, %)
            if (meta.width == 0) {
                std::string ws = ExtractAttribute("width");
                std::string hs = ExtractAttribute("height");
                float w = 0, h = 0;
                if (!ws.empty()) sscanf_s(ws.c_str(), "%f", &w);
                if (!hs.empty()) sscanf_s(hs.c_str(), "%f", &h);
                if (w > 0 && h > 0) {
                    meta.width  = (int)w;
                    meta.height = (int)h;
                }
            }
        }
    }
    // ── HDR (Radiance RGBE) ───────────────────────────────────────────────────
    // Resolution line format: "-Y <height> +X <width>"
    else if (ext == L"hdr") {
        char buf[1024] = {0};
        if (ReadFile(hFile, buf, sizeof(buf) - 1, &bytesRead, nullptr) && bytesRead > 0) {
            std::string content(buf, bytesRead);
            size_t marker = content.find("-Y");
            if (marker != std::string::npos) {
                size_t lineEnd = content.find('\n', marker);
                if (lineEnd != std::string::npos) {
                    int w = 0, h = 0;
                    if (sscanf_s(content.substr(marker, lineEnd - marker).c_str(),
                                 "-Y %d +X %d", &h, &w) == 2) {
                        meta.width = w; meta.height = h;
                    }
                }
            }
        }
    }
    // ── EXR (OpenEXR) ─────────────────────────────────────────────────────────
    // Scans attribute list for "dataWindow" (type box2i: xMin,yMin,xMax,yMax).
    // Width  = xMax - xMin + 1
    // Height = yMax - yMin + 1
    // Uses memcpy to avoid undefined behaviour from unaligned int* cast.
    else if (ext == L"exr") {
        unsigned char head[512] = {0};
        if (ReadFile(hFile, head, sizeof(head), &bytesRead, nullptr) && bytesRead >= 4
            && head[0] == 0x76 && head[1] == 0x2f && head[2] == 0x31 && head[3] == 0x01)
        {
            for (DWORD i = 4; i + 32 < bytesRead; ++i) {
                if (memcmp(head + i, "dataWindow", 10) == 0) {
                    size_t off = i + 11; // skip "dataWindow\0"
                    // Layout: type "box2i\0" (6) + size DWORD (4) + xMin,yMin,xMax,yMax (16)
                    if (off + 26 <= (size_t)bytesRead
                        && memcmp(head + off, "box2i", 5) == 0)
                    {
                        int box[4];
                        memcpy(box, head + off + 10, sizeof(box)); // safe unaligned read
                        int w = box[2] - box[0] + 1;
                        int h = box[3] - box[1] + 1;
                        if (w > 0 && h > 0 && w < 65535 && h < 65535) {
                            meta.width = w; meta.height = h;
                        }
                    }
                    break;
                }
            }
        }
    }
    // ── TGA (Truevision TARGA) ────────────────────────────────────────────────
    // Fixed 18-byte header. Width/height at bytes 12–15 (little-endian WORD each).
    // Supported image types: 1=CM, 2=RGB, 3=BW (raw) and 9,10,11 (RLE variants).
    else if (ext == L"tga") {
        unsigned char hdr[18] = {0};
        if (ReadFile(hFile, hdr, sizeof(hdr), &bytesRead, nullptr)
            && bytesRead == sizeof(hdr))
        {
            BYTE imgType = hdr[2];
            if (imgType == 1 || imgType == 2  || imgType == 3  ||
                imgType == 9 || imgType == 10 || imgType == 11)
            {
                int w = (int)(hdr[12] | (hdr[13] << 8));
                int h = (int)(hdr[14] | (hdr[15] << 8));
                if (w > 0 && h > 0 && w < 65535 && h < 65535) {
                    meta.width = w; meta.height = h;
                }
            }
        }
    }
    // ── PSD (Adobe Photoshop) ─────────────────────────────────────────────────
    // Fixed 26-byte header:
    //   Bytes 0–3:  signature "8BPS"
    //   Bytes 4–5:  version (1 = PSD, 2 = PSB)
    //   Bytes 6–11: reserved (must be zero)
    //   Bytes 12–13: channels (1–56)
    //   Bytes 14–17: height (rows), big-endian DWORD
    //   Bytes 18–21: width (columns), big-endian DWORD
    else if (ext == L"psd" || ext == L"psb") {
        unsigned char hdr[26] = {0};
        if (ReadFile(hFile, hdr, sizeof(hdr), &bytesRead, nullptr)
            && bytesRead == sizeof(hdr)
            && hdr[0] == '8' && hdr[1] == 'B' && hdr[2] == 'P' && hdr[3] == 'S'
            && (hdr[4] == 0 && (hdr[5] == 1 || hdr[5] == 2))) // version 1=PSD, 2=PSB
        {
            DWORD h = ((DWORD)hdr[14] << 24) | ((DWORD)hdr[15] << 16)
                    | ((DWORD)hdr[16] <<  8) |  (DWORD)hdr[17];
            DWORD w = ((DWORD)hdr[18] << 24) | ((DWORD)hdr[19] << 16)
                    | ((DWORD)hdr[20] <<  8) |  (DWORD)hdr[21];
            if (w > 0 && h > 0 && w <= 300000 && h <= 300000) {
                meta.width  = (int)w;
                meta.height = (int)h;
            }
        }
    }

    CloseHandle(hFile);
    return meta;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 2: Utility helpers
// ═══════════════════════════════════════════════════════════════════════════════

// Returns true if the file at `path` is safe to read synchronously on the
// Explorer UI thread (< ~5 ms). Controlled by settings.
// Offline files and reparse points (symlinks/junctions) are always skipped.
static bool IsSafeToParse(const std::wstring& path)
{
    if (path.empty() || path.length() < 3) return false;

    // UNC network paths (\\server\share\...)
    if (path[0] == L'\\' && path[1] == L'\\') {
        if (!g_allowNetworkDrives)
            return false;
        goto check_attributes;
    }

    {
        wchar_t root[4] = { path[0], L':', L'\\', L'\0' };
        UINT driveType  = GetDriveTypeW(root);

        if (driveType == DRIVE_REMOVABLE && !g_allowRemovableDrives)
            return false;
        if (driveType != DRIVE_FIXED     &&
            driveType != DRIVE_REMOVABLE &&
            driveType != DRIVE_REMOTE)
            return false;
    }

check_attributes:
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES)          return false;
    if (attr & FILE_ATTRIBUTE_OFFLINE)            return false;
    if (attr & FILE_ATTRIBUTE_REPARSE_POINT)      return false; // symlinks/junctions
    return true;
}

// Strips Unicode LTR/RTL directional marks (U+200E, U+200F) that Windows
// appends to formatted date strings.
static std::wstring CleanPropString(LPCWSTR src)
{
    if (!src) return L"";
    std::wstring s(src);
    s.erase(std::remove(s.begin(), s.end(), L'\x200e'), s.end());
    s.erase(std::remove(s.begin(), s.end(), L'\x200f'), s.end());
    return s;
}

// Cleans a file-type description string.
// Applies CleanPropString, then strips any trailing " .<digits>" suffix that
// Windows appends to some type names (e.g. "Adobe Photoshop Image.27" → 
// "Adobe Photoshop Image"). This is a known Windows shell quirk for PSD files.
static std::wstring CleanTypeString(LPCWSTR src)
{
    std::wstring s = CleanPropString(src);
    // Strip trailing ".<one or more digits>"
    size_t dot = s.rfind(L'.');
    if (dot != std::wstring::npos && dot + 1 < s.size()) {
        bool allDigits = true;
        for (size_t i = dot + 1; i < s.size(); ++i) {
            if (!iswdigit(s[i])) { allDigits = false; break; }
        }
        if (allDigits) s.resize(dot);
    }
    return s;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 3: Selection path discovery
// ═══════════════════════════════════════════════════════════════════════════════

// Extracts the filesystem path of the single selected item from an IShellBrowser.
// Uses IFolderView2::GetSelection — avoids IDataObject round-trip.
// GetSelection(FALSE) prevents the folder itself from being returned when
// nothing is selected.
// Returns empty string if zero or more than one item is selected.
static std::wstring ExtractPathFromBrowser(IShellBrowser* pSB)
{
    if (!pSB) return L"";
    IShellView* pSV = nullptr;
    if (FAILED(pSB->QueryActiveShellView(&pSV)) || !pSV) return L"";

    std::wstring path;
    IFolderView2* pFV2 = nullptr;
    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView2, (void**)&pFV2)) && pFV2) {
        IShellItemArray* pIA = nullptr;
        if (SUCCEEDED(pFV2->GetSelection(FALSE, &pIA)) && pIA) {
            DWORD cnt = 0;
            pIA->GetCount(&cnt);
            if (cnt == 1) {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pIA->GetItemAt(0, &pItem)) && pItem) {
                    LPWSTR ps = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &ps)) && ps) {
                        // Skip directories — status bar uses a different path for them
                        DWORD attr = GetFileAttributesW(ps);
                        if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY))
                            path = ps;
                        CoTaskMemFree(ps);
                    }
                    pItem->Release();
                }
            }
            pIA->Release();
        }
        pFV2->Release();
    }
    pSV->Release();
    return path;
}

// Attempts to send CWM_GETISHELLBROWSER to a window of a known-safe class.
// Guards:
//   1. Thread ID — only windows belonging to the current thread; prevents
//      cross-process or cross-window-station SendMessage to a foreign Explorer.
//   2. Class name — only CabinetWClass / ShellTabWindowClass; these are the
//      only classes known to handle WM_USER+7 correctly.
static IShellBrowser* SafeGetShellBrowser(HWND h)
{
    if (!h) return nullptr;
    // Guard 1: must belong to the current process.
    // PSFormatForDisplayAlloc fires on a shell worker thread, not the GUI thread,
    // so GetCurrentThreadId() never matches the window's thread. Process-ID check
    // is sufficient to prevent sending CWM_GETISHELLBROWSER to other processes.
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId())
        return nullptr;
    // Guard 2: must be a known-safe class
    wchar_t cls[64] = {};
    GetClassNameW(h, cls, 64);
    if (_wcsicmp(cls, L"CabinetWClass")      != 0 &&
        _wcsicmp(cls, L"ShellTabWindowClass") != 0)
        return nullptr;
    return (IShellBrowser*)SendMessageW(h, CWM_GETISHELLBROWSER, 0, 0);
}

// Finds the path of the currently selected file in the Explorer window that
// triggered the current PSFormatForDisplayAlloc call.
//
// PSFormatForDisplayAlloc may fire on a shell worker thread (not the GUI thread
// that owns the Explorer window), so GetGUIThreadInfo may return no window.
// GetForegroundWindow() is the reliable cross-thread fallback: it returns the
// single window the user is actively interacting with, which is the Explorer
// window whose status bar is being updated.
//
// We deliberately do NOT enumerate all CabinetWClass windows: that would risk
// reading a selection from a different Explorer window, which is worse than
// showing nothing.
//
// Strategy 1: Walk up the focus-window hierarchy (safe classes only).
//             GetGUIThreadInfo is tried first; GetForegroundWindow as fallback.
// Strategy 2: ShellTabWindowClass children of the same root window.
//             Covers address bar / search box focus with a tab selection.
// No cross-window fallback. If neither strategy finds a selection, we return
// empty and the status bar shows only the file size (normal Explorer behaviour).
static std::wstring GetSelectedFilePath()
{
    // Prefer the focused window of the current thread (works when called on the
    // UI thread). Fall back to the system foreground window (works from any thread).
    HWND hFocus = NULL;
    GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
    if (GetGUIThreadInfo(GetCurrentThreadId(), &gti))
        hFocus = gti.hwndFocus ? gti.hwndFocus : gti.hwndActive;
    if (!hFocus) {
        hFocus = GetForegroundWindow();
    }
    if (!hFocus) return L"";

    // Strategy 1: walk up the focus chain, safe-class only
    for (HWND h = hFocus; h; h = GetParent(h)) {
        IShellBrowser* pSB = SafeGetShellBrowser(h);
        if (pSB) {
            std::wstring p = ExtractPathFromBrowser(pSB);
            if (!p.empty()) return p;
        }
    }

    // Strategy 2: ShellTabWindowClass children of the same root window
    HWND hRoot = GetAncestor(hFocus, GA_ROOT);
    if (hRoot) {
        HWND hTab = NULL;
        while ((hTab = FindWindowExW(hRoot, hTab, L"ShellTabWindowClass", NULL)) != NULL) {
            IShellBrowser* pSB = SafeGetShellBrowser(hTab);
            if (pSB) {
                std::wstring p = ExtractPathFromBrowser(pSB);
                if (!p.empty()) return p;
            }
        }
    }

    return L"";
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 4: Cache helpers
// ═══════════════════════════════════════════════════════════════════════════════

// Removes the N oldest cache entries (by insertion order).
// Must be called with g_cs held.
static void EvictOldestEntries(size_t n)
{
    n = std::min(n, g_cacheOrder.size());
    for (size_t i = 0; i < n; ++i)
        g_metaCache.erase(g_cacheOrder[i]);
    g_cacheOrder.erase(g_cacheOrder.begin(), g_cacheOrder.begin() + (ptrdiff_t)n);
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 5: Metadata suffix builder
// ═══════════════════════════════════════════════════════════════════════════════

// Builds the status-bar suffix for `filePath`. Called at most once per unique
// path (results are cached by the caller).
//
// Metadata order: Dimensions | Date Modified | Duration | Bitrate | FPS | Type
//
// Dimensions priority:
//   1. PKEY_Image_Dimensions  (Windows indexer — JPEG, PNG, BMP, …)
//   2. PKEY_Video_FrameWidth/Height (Windows indexer — MP4, AVI, …)
//   3. Custom binary parser   (SVG, HDR, EXR, TIFF/TX, TGA, PSD, WebM/MKV) as fallback
//
// Note: g_origPSF (the real PSFormatForDisplayAlloc) is called here for keys
// other than System.Size — this does NOT re-enter our hook because we only
// intercept System.Size calls from SHELL32.dll.
static std::wstring BuildMetadataSuffix(const std::wstring& filePath)
{
    IShellItem2* pItem2 = nullptr;
    if (FAILED(SHCreateItemFromParsingName(
            filePath.c_str(), nullptr, IID_PPV_ARGS(&pItem2))) || !pItem2)
        return L"";

    // Extension for custom parser dispatch
    std::wstring ext;
    size_t dot = filePath.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        ext = filePath.substr(dot + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    }

    // Run custom parser for formats not covered by the Windows Property System
    CustomMeta customMeta;
    if (ext == L"webm" || ext == L"mkv"  ||
        ext == L"tx"   || ext == L"tif"  || ext == L"tiff" ||
        ext == L"svg"  || ext == L"hdr"  || ext == L"exr"  ||
        ext == L"tga"  || ext == L"psd"  || ext == L"psb")
    {
        customMeta = ParseCustomFormatMetadata(filePath, ext);
    }

    std::wstring sfx;

    // ── 1. Dimensions ────────────────────────────────────────────────────────
    {
        LPWSTR pszDim = nullptr;
        std::wstring dimStr;

        if (SUCCEEDED(pItem2->GetString(PKEY_Image_Dimensions, &pszDim)) && pszDim) {
            dimStr = CleanPropString(pszDim);
            CoTaskMemFree(pszDim);
        }
        if (dimStr.empty()) {
            ULONG fw = 0, fh = 0;
            if (SUCCEEDED(pItem2->GetUInt32(PKEY_Video_FrameWidth,  &fw)) && fw > 0 &&
                SUCCEEDED(pItem2->GetUInt32(PKEY_Video_FrameHeight, &fh)) && fh > 0) {
                wchar_t buf[64];
                swprintf_s(buf, L"%u \u00D7 %u", fw, fh);
                dimStr = buf;
            }
        }
        if (dimStr.empty() && customMeta.width > 0 && customMeta.height > 0) {
            wchar_t buf[64];
            swprintf_s(buf, L"%d \u00D7 %d", customMeta.width, customMeta.height);
            dimStr = buf;
        }
        if (!dimStr.empty()) sfx += L" \u2502 " + dimStr;
    }

    // ── 2. Date Modified ─────────────────────────────────────────────────────
    // PSFormatForDisplayAlloc handles timezone and DST conversion correctly,
    // avoiding the ±1 h error from manual FileTimeToLocalFileTime chains.
    {
        PROPVARIANT pv; PropVariantInit(&pv);
        if (SUCCEEDED(pItem2->GetProperty(PKEY_DateModified, &pv))) {
            LPWSTR pszDate = nullptr;
            if (SUCCEEDED(g_origPSF(PKEY_DateModified, pv, PDFF_DEFAULT, &pszDate))
                && pszDate) {
                std::wstring s = CleanPropString(pszDate);
                if (!s.empty()) sfx += L" \u2502 " + s;
                CoTaskMemFree(pszDate);
            }
            PropVariantClear(&pv);
        }
    }

    // ── 3. Duration ──────────────────────────────────────────────────────────
    ULONGLONG duration100ns = 0;
    bool hasDuration = SUCCEEDED(pItem2->GetUInt64(PKEY_Media_Duration, &duration100ns))
                       && duration100ns > 0;

    if (hasDuration) {
        ULONG totalSec = (ULONG)(duration100ns / 10000000ULL);
        ULONG hh = totalSec / 3600;
        ULONG mm = (totalSec % 3600) / 60;
        ULONG ss = totalSec % 60;
        wchar_t durBuf[64];
        if (hh > 0) swprintf_s(durBuf, L"%02u:%02u:%02u", hh, mm, ss);
        else        swprintf_s(durBuf, L"%02u:%02u", mm, ss);
        sfx += L" \u2502 " + std::wstring(durBuf);
    }

    // ── 4. Bitrate ───────────────────────────────────────────────────────────
    // Priority: video total bitrate → audio encoding bitrate → file-size estimate
    if (hasDuration) {
        ULONG bitrateBps = 0;
        if (FAILED(pItem2->GetUInt32(PKEY_Video_TotalBitrate, &bitrateBps)) || !bitrateBps)
            pItem2->GetUInt32(PKEY_Audio_EncodingBitrate, &bitrateBps);
        if (bitrateBps == 0) {
            ULONGLONG fileSize = 0;
            if (SUCCEEDED(pItem2->GetUInt64(PKEY_Size, &fileSize)) && fileSize > 0) {
                double durationSec = (double)duration100ns / 10000000.0;
                if (durationSec > 0)
                    bitrateBps = (ULONG)((fileSize * 8.0) / durationSec);
            }
        }
        if (bitrateBps > 0) {
            wchar_t bitBuf[64];
            if (bitrateBps >= 1000000)
                swprintf_s(bitBuf, L"%.1f Mbps", (double)bitrateBps / 1000000.0);
            else
                swprintf_s(bitBuf, L"%u kbps", bitrateBps / 1000);
            sfx += L" \u2502 " + std::wstring(bitBuf);
        }
    }

    // ── 5. FPS ───────────────────────────────────────────────────────────────
    // Property System stores fps * 1000 as VT_UI4 (e.g. 29970 → 29.97 fps).
    // Fallback: EBML DefaultDuration from the custom WebM/MKV parser.
    {
        ULONG fps1000 = 0;
        if (!SUCCEEDED(pItem2->GetUInt32(PKEY_Video_FrameRate, &fps1000)) || !fps1000) {
            if (customMeta.fps > 0.0)
                fps1000 = (ULONG)(customMeta.fps * 1000.0 + 0.5);
        }
        if (fps1000 > 0) {
            wchar_t fpsBuf[64];
            if (fps1000 % 1000 == 0) swprintf_s(fpsBuf, L"%u fps", fps1000 / 1000);
            else                     swprintf_s(fpsBuf, L"%.2f fps", (double)fps1000 / 1000.0);
            sfx += L" \u2502 " + std::wstring(fpsBuf);
        }
    }

    // ── 6. File type description ──────────────────────────────────────────────
    // Placed last so it appears at the right edge for all file types.
    // IShellItem2::GetString(PKEY_ItemTypeText) works correctly on the UI thread.
    // SHGetFileInfoW(SHGFI_USEFILEATTRIBUTES) is used as fallback — never hits
    // disk and is safe on any thread.
    // CleanTypeString strips the spurious ".<digits>" suffix Windows appends to
    // some registered types (e.g. "Adobe Photoshop Image.27" → "Adobe Photoshop Image").
    // For PSD/PSB we override the system string entirely: Windows reports
    // "Adobe Photoshop Image" while the consistent convention is "...File".
    {
        std::wstring typeStr;

        // Hard override for known problematic types
        if (ext == L"psd" || ext == L"psb")
            typeStr = L"Adobe Photoshop File";

        if (typeStr.empty()) {
            LPWSTR pszType = nullptr;
            if (SUCCEEDED(pItem2->GetString(PKEY_ItemTypeText, &pszType)) && pszType) {
                typeStr = CleanTypeString(pszType);
                CoTaskMemFree(pszType);
            }
        }
        if (typeStr.empty()) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW(filePath.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
                               SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME) && sfi.szTypeName[0])
                typeStr = CleanTypeString(sfi.szTypeName);
        }
        if (!typeStr.empty()) sfx += L" \u2502 " + typeStr;
    }

    pItem2->Release();
    return sfx;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 6: PSFormatForDisplayAlloc hook
// ═══════════════════════════════════════════════════════════════════════════════

HRESULT WINAPI Hook_PSF(REFPROPERTYKEY key, REFPROPVARIANT propvar,
                        PROPDESC_FORMAT_FLAGS pdff, PWSTR* ppszDisplay)
{
    HRESULT hr = g_origPSF(key, propvar, pdff, ppszDisplay);

    // Only intercept System.Size
    if (!IsSystemSize(key)) return hr;
    if (FAILED(hr) || !ppszDisplay || !*ppszDisplay) return hr;

    // ── Stack filter: identify the status bar call ────────────────────────────
    // The status bar path goes through SHELL32.dll.
    // The Details column path goes through explorerframe.dll — skip it.
    // When other mods chain-hook PSFormatForDisplayAlloc (e.g. "Better file sizes"),
    // Windhawk inserts their thunk between SHELL32 and us, so we scan 5 frames.
    {
        void* frames[5] = {};
        CaptureStackBackTrace(1, 5, frames, nullptr);
        bool isStatusBar = false;
        for (int i = 0; i < 5 && frames[i]; ++i) {
            HMODULE hMod = nullptr;
            GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCWSTR)frames[i], &hMod);
            wchar_t modPath[MAX_PATH] = {};
            GetModuleFileNameW(hMod, modPath, MAX_PATH);
            const wchar_t* name = wcsrchr(modPath, L'\\');
            name = name ? name + 1 : modPath;
            if (_wcsicmp(name, L"explorerframe.dll") == 0) return hr; // Details column
            if (_wcsicmp(name, L"SHELL32.dll")       == 0) { isStatusBar = true; break; }
        }
        if (!isStatusBar) return hr;
    }

    // ── Get selected file path ────────────────────────────────────────────────
    std::wstring path = GetSelectedFilePath();
    if (path.empty()) return hr;

    // ── Drive / offline / reparse safety check ───────────────────────────────
    if (!IsSafeToParse(path)) return hr;

    // ── Cache lookup ─────────────────────────────────────────────────────────
    EnterCriticalSection(&g_cs);
    auto it = g_metaCache.find(path);
    if (it != g_metaCache.end()) {
        std::wstring cached = it->second;
        // LRU: move this entry to the back so it is evicted last
        auto oit = std::find(g_cacheOrder.begin(), g_cacheOrder.end(), path);
        if (oit != g_cacheOrder.end()) {
            g_cacheOrder.erase(oit);
            g_cacheOrder.push_back(path);
        }
        LeaveCriticalSection(&g_cs);
        if (cached.empty()) return hr;
        std::wstring full = std::wstring(*ppszDisplay) + cached;
        if (auto* s = (PWSTR)CoTaskMemAlloc((full.size() + 1) * sizeof(WCHAR))) {
            wcscpy_s(s, full.size() + 1, full.c_str());
            CoTaskMemFree(*ppszDisplay);
            *ppszDisplay = s;
        }
        return hr;
    }

    // Cache miss — evict if full, then compute
    if (g_metaCache.size() >= MAX_CACHE_SIZE)
        EvictOldestEntries(g_cacheEvictCount);
    LeaveCriticalSection(&g_cs);

    std::wstring suffix = BuildMetadataSuffix(path);

    EnterCriticalSection(&g_cs);
    // Double-check: another call may have inserted the entry while we computed
    if (g_metaCache.find(path) == g_metaCache.end()) {
        g_metaCache[path] = suffix;
        g_cacheOrder.push_back(path);
    }
    LeaveCriticalSection(&g_cs);

    if (suffix.empty()) return hr;

    std::wstring full = std::wstring(*ppszDisplay) + suffix;
    if (auto* s = (PWSTR)CoTaskMemAlloc((full.size() + 1) * sizeof(WCHAR))) {
        wcscpy_s(s, full.size() + 1, full.c_str());
        CoTaskMemFree(*ppszDisplay);
        *ppszDisplay = s;
    }
    return hr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLOCK 7: Mod lifecycle
// ═══════════════════════════════════════════════════════════════════════════════

static void LoadSettings()
{
    g_allowNetworkDrives   = Wh_GetIntSetting(L"allowNetworkDrives")   != 0;
    g_allowRemovableDrives = Wh_GetIntSetting(L"allowRemovableDrives") != 0;
    int evictRaw           = Wh_GetIntSetting(L"cacheEvictCount");
    g_cacheEvictCount      = (evictRaw >= 1 && evictRaw <= 50) ? (size_t)evictRaw : 10;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Wh_ModInit: starting");
    LoadSettings();

    InitializeCriticalSection(&g_cs);

    HMODULE hPS = GetModuleHandleW(L"propsys.dll");
    if (!hPS) hPS = LoadLibraryW(L"propsys.dll");
    if (!hPS) { Wh_Log(L"Wh_ModInit: FAILED — propsys.dll not found"); return FALSE; }

    void* pfn = (void*)GetProcAddress(hPS, "PSFormatForDisplayAlloc");
    if (!pfn) { Wh_Log(L"Wh_ModInit: FAILED — PSFormatForDisplayAlloc not found"); return FALSE; }

    Wh_SetFunctionHook(pfn, (void*)Hook_PSF, (void**)&g_origPSF);
    Wh_Log(L"Wh_ModInit: hook set — OK");
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
    // Clear the cache so settings changes (network/removable drives) take
    // effect immediately without requiring an Explorer restart.
    EnterCriticalSection(&g_cs);
    g_metaCache.clear();
    g_cacheOrder.clear();
    LeaveCriticalSection(&g_cs);
}

void Wh_ModUninit()
{
    Wh_Log(L"Wh_ModUninit: unloading");
    EnterCriticalSection(&g_cs);
    g_metaCache.clear();
    g_cacheOrder.clear();
    LeaveCriticalSection(&g_cs);
    DeleteCriticalSection(&g_cs);
}
