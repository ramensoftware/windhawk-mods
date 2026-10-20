// ==WindhawkMod==
// @id              cursor-override
// @name            Cursor Override
// @description     Recognizes supported cursors by image and hotspot and loads replacements on demand
// @version         1.1.1
// @author          Basti
// @github          https://github.com/sebastianheder01
// @include         *
// @exclude         dwm.exe
// @exclude         LogonUI.exe
// @exclude         consent.exe
// @exclude         winlogon.exe
// @exclude         csrss.exe
// @exclude         lsass.exe
// @exclude         services.exe
// @exclude         smss.exe
// @exclude         wininit.exe
// @exclude         fontdrvhost.exe
// @exclude         windhawk.exe
// @exclude         windhawk-x64-helper.exe
// @compilerOptions -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cursor Override

Replaces common additional cursors with custom `.cur` or `.ani` files.

![Grab cursor before and after](https://raw.githubusercontent.com/sebastianheder01/Galaxy-Cursor/main/preview/comparison-grab.png)

![Cell cursor before and after](https://raw.githubusercontent.com/sebastianheder01/Galaxy-Cursor/main/preview/comparison-cell.png)

## How it works

Identifies supported cursors by their image and hotspot in applications targeted by Windhawk.
Recognition is independent of the application or website the cursor originates from.
Resource cursors are compared at a fixed size when their source resource is available; known bitmap signatures provide a fallback.
Recognition can require an update when an application changes its cursor artwork.
Supports `Grab`, `Grabbing`, `Cell`, `Copy`, `Alias`, `ZoomIn`, `ZoomOut`, `ColResize`, `ColSelect`, `RowResize`, `RowSelect`, `VerticalText` and `SelectionBar`.

Network paths and mapped network drives are not supported.
If a file cannot be loaded, the original cursor stays active. Check the path and save the settings again to retry.
Windhawk debug logging reports the affected setting and error code without logging the file path.
With all fields empty, the mod installs no hooks.

## Usage

Under Settings, enter the full local path to each `.cur` or `.ani` file you want to change. Leave a field empty to keep the original cursor.
Keep the cursor files in a permanent local folder.

## Compatibility

This mod overlaps with [Chromium Cursor Remap](https://windhawk.net/mods/chromium-cursor-remap) for Chromium-based applications and additionally recognizes Mozilla and RichEdit cursors.
Do not enable both mods in the same applications.

## Optional cursor pack

For an optional cursor pack, check out [Galaxy Cursor](https://github.com/sebastianheder01/Galaxy-Cursor).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- grabPath: ""
  $name: Grab
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- grabbingPath: ""
  $name: Grabbing
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- selectionBarPath: ""
  $name: Selection bar
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- copyPath: ""
  $name: Copy
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- aliasPath: ""
  $name: Alias
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- rowResizePath: ""
  $name: Row resize
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- colResizePath: ""
  $name: Column resize
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- rowSelectPath: ""
  $name: Row select
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- colSelectPath: ""
  $name: Column select
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- verticalTextPath: ""
  $name: Vertical text
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- zoomOutPath: ""
  $name: Zoom out
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- zoomInPath: ""
  $name: Zoom in
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

- cellPath: ""
  $name: Cell
  $description: Full local path to a .cur or .ani file. Leave empty to keep the original cursor.

*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cwchar>
#include <string>
#include <utility>

enum class Role : int {
    Unknown = -1, Grab, Grabbing, Cell, Copy, Alias, ZoomIn, ZoomOut,
    ColResize, RowResize, VerticalText, SelectionBar, RowSelect, ColSelect, Count
};

static constexpr PCWSTR kSettingNames[] = {
    L"grabPath", L"grabbingPath", L"cellPath", L"copyPath", L"aliasPath",
    L"zoomInPath", L"zoomOutPath", L"colResizePath", L"rowResizePath",
    L"verticalTextPath", L"selectionBarPath", L"rowSelectPath", L"colSelectPath"
};
static constexpr size_t kRoleCount = static_cast<size_t>(Role::Count);
static constexpr size_t kPoolSize = kRoleCount * 2;
static constexpr DWORD kProcessUserLimit = 6000;
static constexpr DWORD kOwnedUserLimit = 4096;
static_assert(ARRAYSIZE(kSettingNames) == kRoleCount);

struct Fingerprint {
    uint64_t first = 14695981039346656037ULL;
    uint64_t second = 7809847782465536322ULL;
    bool operator==(const Fingerprint&) const = default;
};

struct CursorSignature {
    uint64_t first;
    uint64_t second;
    Role role;
};

static constexpr CursorSignature kSignatures[] = {
    {0x005f96ad8efd3aa6ULL, 0x190bbd0cdf1090a7ULL, Role::SelectionBar},
    {0x006257a685b51bdaULL, 0x99dacdabaf9bc964ULL, Role::RowSelect},
    {0x00b675d8ea3db725ULL, 0xbfbca9f198492825ULL, Role::ColSelect},
    {0x0162212d2b8a79c4ULL, 0xbf9adeedc4fbd617ULL, Role::RowResize},
    {0x02e00035b859040cULL, 0xfc88c67c9474d0c9ULL, Role::Grabbing},
    {0x038443a20c67e6e9ULL, 0x213a89a26b2d8753ULL, Role::ZoomOut},
    {0x03b92489111a2889ULL, 0x3c74b2fcd76618b4ULL, Role::Alias},
    {0x03becd8b8d8bb0c5ULL, 0xb7f62ab10c125b49ULL, Role::RowSelect},
    {0x04e38414481e0f9bULL, 0xf8a6baf1796f3cdcULL, Role::RowResize},
    {0x067a3205131f87a2ULL, 0x4d97ffab0d6aae98ULL, Role::RowResize},
    {0x0695cebe5f024f60ULL, 0x765a7226a6677fd8ULL, Role::Grab},
    {0x082e2273472bffbcULL, 0xd37baba74803e0f2ULL, Role::Grabbing},
    {0x0922dbf5a099af12ULL, 0x521a43750bb92ea0ULL, Role::Alias},
    {0x0b0ab2de231bf479ULL, 0x5ae57895734bd92cULL, Role::ColSelect},
    {0x0b3836785e6a4543ULL, 0x132ab65d083574faULL, Role::RowSelect},
    {0x0cc4100afdd95dd6ULL, 0x932fb74deba84156ULL, Role::ColResize},
    {0x0df855fea1fc4939ULL, 0x49f8b751f1031b7fULL, Role::SelectionBar},
    {0x0e726a12ffdf7235ULL, 0xae5aadbbb5062b01ULL, Role::RowSelect},
    {0x1060d5b0e05b266bULL, 0x1845eda8cc345b38ULL, Role::RowResize},
    {0x11a1afe98a91616eULL, 0x7a8fdc2f64fe8674ULL, Role::RowSelect},
    {0x11a94af3faba74c4ULL, 0x28ce428a9c0df4bdULL, Role::Cell},
    {0x11ddfd140134da08ULL, 0x2fb26aee14f02a66ULL, Role::Grab},
    {0x138cb78702bdd9e5ULL, 0xcca30a5cfd3d93f1ULL, Role::ZoomIn},
    {0x13e6b8f9ac5c4e77ULL, 0xdb07caa0238acc20ULL, Role::Alias},
    {0x13ec6b090033fecbULL, 0x4643a7eba6165beaULL, Role::SelectionBar},
    {0x14463982bb04ff05ULL, 0x73b3e970ada3f095ULL, Role::ColSelect},
    {0x1635743a5b6d5598ULL, 0x884359a0af1dc858ULL, Role::ColSelect},
    {0x16404e7f6424e327ULL, 0x23fc51344c0ae0e0ULL, Role::RowResize},
    {0x168111e9f3d6eaa5ULL, 0xeb3b0ddf72f43d0cULL, Role::RowSelect},
    {0x1844fe4f1c5bc913ULL, 0xfebe18e9c18aa536ULL, Role::ColResize},
    {0x1877fa0870ebb835ULL, 0xf3b0bd99bb4b09b9ULL, Role::ColSelect},
    {0x18af55ec1d4eb096ULL, 0x6e2ae6c7cf9ae947ULL, Role::SelectionBar},
    {0x1a54414fb8a60e55ULL, 0x94f90f140ffbd7e4ULL, Role::Alias},
    {0x1a92d483bab4b35bULL, 0x93d5b3a42298b8a2ULL, Role::ColResize},
    {0x1a971b2ddda7f208ULL, 0x4314375ba9f90addULL, Role::Grabbing},
    {0x1ae033019d79dce9ULL, 0x7871478a1c9ad2bcULL, Role::RowSelect},
    {0x1af208ed30c5bdc1ULL, 0x10347cd23753ffa7ULL, Role::ColSelect},
    {0x1bd39abc41b4c3e5ULL, 0xb9c994926ce32fddULL, Role::Grab},
    {0x1c4402974d282c85ULL, 0x1ed3b434ed235937ULL, Role::Alias},
    {0x1c84813c5526df17ULL, 0x4e54e85d2b054632ULL, Role::RowResize},
    {0x1d2ff11f5416be34ULL, 0x99b1d514dbc8d17eULL, Role::ZoomOut},
    {0x1d98977bd7bd1946ULL, 0x216ec3ae481be1d3ULL, Role::ColResize},
    {0x1ff69de75cf73c92ULL, 0x77da95e56e3a9a8dULL, Role::Copy},
    {0x2039bc04b4b38997ULL, 0xe1af02e9f78f98e6ULL, Role::RowSelect},
    {0x205ce6a815819d6cULL, 0x3a78096ad883cacfULL, Role::Grab},
    {0x208c3cc733e29202ULL, 0xbf7cdacd413d5322ULL, Role::Copy},
    {0x21260e5ede2c44d2ULL, 0xb2ed76ca2a9ce543ULL, Role::RowSelect},
    {0x216e9ab50b7cded3ULL, 0x4774379828899ce8ULL, Role::SelectionBar},
    {0x21fce665d30679f9ULL, 0xde64fa29312c34ecULL, Role::ColSelect},
    {0x231b7519e9f8fb9cULL, 0xeb5cc26262ffbbe7ULL, Role::Grabbing},
    {0x2488668a375688f0ULL, 0x6415dfc0540e2a81ULL, Role::ColSelect},
    {0x28934f01bc7cf2f0ULL, 0x7f7baf3138a0b364ULL, Role::VerticalText},
    {0x29938c4d30e9dfaeULL, 0x010b64caf1053fc1ULL, Role::RowSelect},
    {0x2bf0fa94abc1b851ULL, 0xf41171dad2fedef7ULL, Role::Copy},
    {0x2dab096b381f222dULL, 0xfba0c230360019a4ULL, Role::Grabbing},
    {0x2f19ba0b480515bdULL, 0xe1f52943e2df5e85ULL, Role::RowResize},
    {0x2f5a38d632f8a5a6ULL, 0x2d6e86ad45272ec8ULL, Role::ColResize},
    {0x2f947ca0fef588b8ULL, 0xcc7e9f2e3338dc52ULL, Role::RowSelect},
    {0x30a535f756a5175dULL, 0xf2ec118ea7c9c107ULL, Role::RowSelect},
    {0x30cce90bf679bb01ULL, 0x3e3bfee59dd4af5aULL, Role::Grabbing},
    {0x3150477b6bc173b7ULL, 0x0d88c864e1d376eaULL, Role::RowSelect},
    {0x328543aa68253ac8ULL, 0xba89e4dc433aefa2ULL, Role::RowSelect},
    {0x33d31ee7ebaab0dfULL, 0xffe4dcd065418556ULL, Role::ColSelect},
    {0x344243efd8b1ee60ULL, 0x7e7c24f92e469074ULL, Role::VerticalText},
    {0x34b594656fb4e89bULL, 0xecbbc0b227c38cebULL, Role::RowSelect},
    {0x35c83e2fbaf35a03ULL, 0xb5c29d1bcd2f7181ULL, Role::RowSelect},
    {0x3854277678f99fe8ULL, 0xb2d23c0b87cd2d36ULL, Role::ColSelect},
    {0x38a11c0478f72b84ULL, 0x2f15b6d0e1892898ULL, Role::ZoomIn},
    {0x3984d6d3319e5371ULL, 0x101c4be158bf562fULL, Role::RowSelect},
    {0x3a1371f94eb84104ULL, 0xc00315b650dafe97ULL, Role::ZoomOut},
    {0x3a3be6b8a964da61ULL, 0x5b62f6d0bc77e6a3ULL, Role::RowResize},
    {0x3a44fb6fae4338ffULL, 0xd4a4b9ba04e9bb2aULL, Role::VerticalText},
    {0x3b3ed8099a306e75ULL, 0x4639abc46e8aef99ULL, Role::Grabbing},
    {0x3bb904859f3161bfULL, 0x1a45f22c55cea519ULL, Role::ColSelect},
    {0x3c0c5aa86310b680ULL, 0x3e27f152139de56eULL, Role::VerticalText},
    {0x3d83dbaede2bcb6eULL, 0xf5bb0ff2848c2284ULL, Role::ColResize},
    {0x3e2f409500135c61ULL, 0x4aeebfc5d0c96e54ULL, Role::ColSelect},
    {0x3e38aea4175831a3ULL, 0x38ceff24c19bbd7eULL, Role::RowSelect},
    {0x3f23440573c42869ULL, 0x6b715e9666d14bcdULL, Role::RowResize},
    {0x3f83ba1df4d3c77cULL, 0x8984ebda01572273ULL, Role::Cell},
    {0x415aa65e0545ff20ULL, 0x7e006cec5bdab143ULL, Role::ColSelect},
    {0x41d1554461dafca3ULL, 0xfce9404ae4fe4475ULL, Role::ColSelect},
    {0x425dd3c0fef2472bULL, 0xe665e0dd4427438fULL, Role::SelectionBar},
    {0x42db5e67b982c719ULL, 0xa313b2e082942188ULL, Role::ZoomOut},
    {0x42f649702236ecf3ULL, 0xd952ff750c764767ULL, Role::ColSelect},
    {0x43038d7fda7dc5e6ULL, 0xbcffff2375fbc48aULL, Role::ColSelect},
    {0x446aeef584806401ULL, 0x64d6d14836fcf722ULL, Role::Alias},
    {0x449d44159a8c82bdULL, 0x280c12b4065e62d1ULL, Role::Grabbing},
    {0x457bdff819bb6869ULL, 0x8c3961fb7d6af9e6ULL, Role::Grab},
    {0x459a3183f631d70fULL, 0xd1e6593cb7bcbd55ULL, Role::VerticalText},
    {0x461fda914e573de7ULL, 0xa8ef6378729c4fabULL, Role::RowSelect},
    {0x474ded38b79659a7ULL, 0xbc842eace25031abULL, Role::ColResize},
    {0x49d310915dd2e10bULL, 0xb32b1e2f0aad4b80ULL, Role::ColSelect},
    {0x4a117948993f0142ULL, 0x4161ad30ce1bcd40ULL, Role::Alias},
    {0x4ac6c6c9f2b134e2ULL, 0xaf45dc089c968292ULL, Role::Grabbing},
    {0x4b94fdaad5a67759ULL, 0x9ee08f34d25cb4d7ULL, Role::ColSelect},
    {0x4bea2df10f67c69cULL, 0x603b455a04b1f271ULL, Role::Grabbing},
    {0x4ce42a8939f5b329ULL, 0x608d9e71d2df0d19ULL, Role::SelectionBar},
    {0x4cec3c64fe25b71dULL, 0xe3be9890366ae7bcULL, Role::ZoomIn},
    {0x51a68311456b00f8ULL, 0x5b5c6027bf080686ULL, Role::Grabbing},
    {0x51ad24cc2f77be8cULL, 0xd9b0a1e1dfe619f2ULL, Role::ColSelect},
    {0x53101c8ef5b87084ULL, 0x10ef8b051b817291ULL, Role::Grab},
    {0x536beb541b74eb1fULL, 0x883ce4ce6e6ae4fcULL, Role::RowSelect},
    {0x54997cfe90b7e820ULL, 0x53b235c4187fab0fULL, Role::ColResize},
    {0x55f5e62ffdc89867ULL, 0x7ddd27f01884d3bcULL, Role::Grabbing},
    {0x57e3d5909030759dULL, 0x1c6f3a51253fdf29ULL, Role::Grabbing},
    {0x58e31d80e533f9c4ULL, 0x5bcb07034e6b02aaULL, Role::ZoomOut},
    {0x59b64a3191972407ULL, 0xc02fd412c8df172aULL, Role::RowSelect},
    {0x5b2318f448c4cfa8ULL, 0x4a58732b661426a3ULL, Role::Grabbing},
    {0x5bbe0ce8f4b794e4ULL, 0x397dde3eec37560fULL, Role::Grabbing},
    {0x5d0f6787b0ef75eaULL, 0x0ab9928f2f8c151cULL, Role::ColSelect},
    {0x5f549d95f2d4f50bULL, 0x1a90a05f992868a1ULL, Role::Grab},
    {0x5f8347d80a28ab49ULL, 0x01e42cc1b7062104ULL, Role::VerticalText},
    {0x620952f4032bc0eeULL, 0x87b1e2b6e8fd167eULL, Role::Grabbing},
    {0x62d2777ad398ea0dULL, 0xa7ba9234f5582cb7ULL, Role::ZoomIn},
    {0x6452158023d505e8ULL, 0x0c383d56eff1e05dULL, Role::ColSelect},
    {0x64902c8a952ee638ULL, 0x64707a79661f5f72ULL, Role::RowSelect},
    {0x64e3f263ee29a8adULL, 0x244e460ebbc735e4ULL, Role::ZoomIn},
    {0x65d70308c9329f18ULL, 0xb9a6caa1fd9a2abbULL, Role::ColResize},
    {0x662531121855dba4ULL, 0x31a8b75880521661ULL, Role::ZoomOut},
    {0x668b81dafc56b13bULL, 0x0b9a250a7208c36aULL, Role::RowSelect},
    {0x66a85a8305cd48f4ULL, 0xe7d7e1fccae0625bULL, Role::Cell},
    {0x68220a0c5fbb46adULL, 0x243fbdc8cf8e4a62ULL, Role::ZoomOut},
    {0x682d3d0d9b3d43ccULL, 0x7fb4f77845a34081ULL, Role::RowSelect},
    {0x693ab328f3981b23ULL, 0x125f1574a139477fULL, Role::RowResize},
    {0x696cedaa3991b904ULL, 0xe5f014e0e5324798ULL, Role::Cell},
    {0x6a51d7f1be10d7f3ULL, 0x5ac03f93e45e2affULL, Role::ColSelect},
    {0x6a79440fdd5f5843ULL, 0x08a8d2fe95f520b7ULL, Role::Alias},
    {0x6ab1c13d7ce411a1ULL, 0xb9c74b60c18902c5ULL, Role::ColSelect},
    {0x6acfa9c3051b6695ULL, 0x7502859f4d46736aULL, Role::RowSelect},
    {0x6b1a8ca4434decb7ULL, 0x2aeac2a3cae831f4ULL, Role::Grabbing},
    {0x6b659b2d945db842ULL, 0x7d4987f71f10214cULL, Role::RowResize},
    {0x6c43c7b065c82d7eULL, 0x39f5f9555a0bda4bULL, Role::ColSelect},
    {0x6dadf39d31fd575dULL, 0x2e08171788e5bc00ULL, Role::Copy},
    {0x6dc2b0a599d1cd3dULL, 0x55896c0899538126ULL, Role::Cell},
    {0x6e16802ded4454fdULL, 0x80d5b490cb1be9a4ULL, Role::ColSelect},
    {0x6f87b3ad5b8e4b47ULL, 0x3a0d3df13ed5ab77ULL, Role::VerticalText},
    {0x6feb1dadce5ea616ULL, 0x2e30a453d8ede189ULL, Role::RowSelect},
    {0x7089c56cd3e76588ULL, 0xfbbe82dbf04598bcULL, Role::Cell},
    {0x716a981bc6eccb0dULL, 0xcf25042d5a2b0bd5ULL, Role::Cell},
    {0x7212dbb5db9547fbULL, 0xbaacf9387bfd2abcULL, Role::ColSelect},
    {0x72fc3c44aacf02dbULL, 0x1e06dd45d5788e5eULL, Role::SelectionBar},
    {0x7328318433facc83ULL, 0x24c8a31dba1b5f60ULL, Role::Copy},
    {0x7376950708aeda9eULL, 0x2ce0149c46bdef00ULL, Role::ColSelect},
    {0x73cd03e1bf0e5173ULL, 0x2f7016514bab52daULL, Role::RowResize},
    {0x7464040dc8fddf91ULL, 0xb756bb9cc57ed134ULL, Role::Grab},
    {0x74758b3ea54db083ULL, 0x29fb675ce4916668ULL, Role::Copy},
    {0x757c21f2094e9909ULL, 0xe764c5ebc6a95d9cULL, Role::ZoomIn},
    {0x7600b4d7275330daULL, 0xea035ce5e64e3ce9ULL, Role::Grab},
    {0x76102af560390d72ULL, 0x91293101c9a9a355ULL, Role::RowSelect},
    {0x78c9b1a10dc29de8ULL, 0x7839eb3df791b523ULL, Role::RowSelect},
    {0x79cd5160f678b7c3ULL, 0x4dd2d8062a202a24ULL, Role::Alias},
    {0x79fdc190c79086b0ULL, 0xda6340282e2f1827ULL, Role::ZoomOut},
    {0x7a5e8a34bab51959ULL, 0xb4e96d5a4ad20df0ULL, Role::Grabbing},
    {0x7a71d3af928dcd96ULL, 0xd50fcd39b8ceb9d1ULL, Role::RowSelect},
    {0x7af4c93371c83301ULL, 0x6ef926a48d05d53bULL, Role::RowSelect},
    {0x7b191d1575e01b98ULL, 0x827d457f49420896ULL, Role::Grabbing},
    {0x7c254b7baa0b18dcULL, 0xdc75350264b0baedULL, Role::ZoomOut},
    {0x7db66124c39d915fULL, 0xa5a31169eecb0264ULL, Role::ColSelect},
    {0x7e30bac769e7f8a4ULL, 0x97f5a558e0586b80ULL, Role::ZoomIn},
    {0x7e9f0b459a3a276eULL, 0x512fc579132e091eULL, Role::VerticalText},
    {0x80d19f10ca59db50ULL, 0x186f30954c47ee1fULL, Role::RowSelect},
    {0x816454fe3ed0853fULL, 0x345a215f7ea43483ULL, Role::Grab},
    {0x81b98cfd07ea9e9aULL, 0x371a2b0d5e156840ULL, Role::ColResize},
    {0x827bc79933ea7284ULL, 0xc14dfa379ee26314ULL, Role::RowSelect},
    {0x83aa68bbe81cb6abULL, 0x6f35b9207e4015fdULL, Role::SelectionBar},
    {0x84de897be093676bULL, 0x48a067bb957708eaULL, Role::VerticalText},
    {0x858868590ac2a279ULL, 0x2c94e257624f9536ULL, Role::Grabbing},
    {0x85a9106a709d8d64ULL, 0xce7a7420f551ce7eULL, Role::Grabbing},
    {0x86220a5396881fa9ULL, 0xa6a1be28180d1d11ULL, Role::Grab},
    {0x865eb07a75b32a25ULL, 0xb3385d6b3333a9d2ULL, Role::Grab},
    {0x87677d9844b85130ULL, 0xb56dab22d03e2892ULL, Role::ColSelect},
    {0x877877ee1535cb65ULL, 0x531d3a3694f9d05cULL, Role::ZoomOut},
    {0x89eb924c5760e1abULL, 0x47c2104fee289b3cULL, Role::ColSelect},
    {0x8a623bcb90cebbcaULL, 0xc7efbd264720ed04ULL, Role::ColSelect},
    {0x8b7591d6cc325723ULL, 0x450d6ddc8f6e9688ULL, Role::Grab},
    {0x8ec0764b246ebdd9ULL, 0x90fff51891bf34c0ULL, Role::RowSelect},
    {0x8fd4c1c4ef988b21ULL, 0x7eb07392c9909ff8ULL, Role::RowSelect},
    {0x903a6e8ed1d6edecULL, 0xe14dbf13907ea56dULL, Role::ZoomIn},
    {0x9128f6c9315f2386ULL, 0x3126c60f9b445a3aULL, Role::VerticalText},
    {0x93e67361d90b2a3eULL, 0x052144f25d8d1383ULL, Role::Grabbing},
    {0x955715ac80994be5ULL, 0x68c5fce75a6eb0f6ULL, Role::Grabbing},
    {0x9581444def9a0badULL, 0x4e08b227eafdf67fULL, Role::Grab},
    {0x95ec55d1e622d568ULL, 0x02acafd954d015f9ULL, Role::ColSelect},
    {0x964f8e13a552b6a5ULL, 0x32f8d384bf33d4f8ULL, Role::Copy},
    {0x9659de02498d5bacULL, 0xc654d621651f9371ULL, Role::RowSelect},
    {0x96627894bceed370ULL, 0x9c824dcc34a1486aULL, Role::Cell},
    {0x968c3b415c1ce22dULL, 0x7b63807c92ef3fd4ULL, Role::ColSelect},
    {0x96efe404411c8d3aULL, 0x6fb7121b999619b1ULL, Role::SelectionBar},
    {0x983e0181e4cb247cULL, 0x830752133c9eb59bULL, Role::ColSelect},
    {0x9844b77439bcfe47ULL, 0x79354765b36bf256ULL, Role::SelectionBar},
    {0x986a76ec5af21b5cULL, 0xad016ac5b69d0d7aULL, Role::Cell},
    {0x986c68061ab95cacULL, 0xa0421d20c2da9fc2ULL, Role::Cell},
    {0x99fe33326167faa9ULL, 0xbe539fef7c7fead6ULL, Role::VerticalText},
    {0x9a96852a32065b51ULL, 0x97c76289d902b345ULL, Role::Cell},
    {0x9c131e8ad3cee489ULL, 0x2691e2ce6f229f55ULL, Role::ColSelect},
    {0x9e360f4e6fa0fd30ULL, 0x85c33651c207ce4eULL, Role::Grabbing},
    {0xa1da9d5dd4758b55ULL, 0x33e5cc1901e916aaULL, Role::RowResize},
    {0xa23b2d5efda180d8ULL, 0x4c332b55c8190b59ULL, Role::ColSelect},
    {0xa259f1e233e5bac4ULL, 0xf5c7f931cdc47311ULL, Role::Grabbing},
    {0xa2df9c7701a699d4ULL, 0x442285c52c94f453ULL, Role::VerticalText},
    {0xa33329dfc147cdf0ULL, 0x1d58e5820256cf95ULL, Role::ColSelect},
    {0xa3c02612dea7efaaULL, 0x0de1c3de53d7f657ULL, Role::Copy},
    {0xa4a775028c23eff5ULL, 0x706a797d706a442eULL, Role::ColSelect},
    {0xa6bf5eb50b650c89ULL, 0x96497e6d7af11c66ULL, Role::Copy},
    {0xa739921e4d2cbfb0ULL, 0x982030aa1fd1c8b8ULL, Role::Grab},
    {0xa881a71edaef5bb6ULL, 0xe72f10f090d77db8ULL, Role::RowSelect},
    {0xa9a1a5af5cf89a21ULL, 0x272aa717e74e6b37ULL, Role::RowResize},
    {0xaa90ffbf244cc0d8ULL, 0x9e975e3cfe1d78b2ULL, Role::RowResize},
    {0xaaed841227371027ULL, 0xa93d60bd472f08c6ULL, Role::Copy},
    {0xab49584a59b9dc27ULL, 0x66dc8fca4a6de03bULL, Role::ColResize},
    {0xac199a8660e17e65ULL, 0x4930f3d93eff6baaULL, Role::ColSelect},
    {0xac2fa60b3a24bdedULL, 0x09e9e4405a664997ULL, Role::Grabbing},
    {0xad2f1b04685322d7ULL, 0x56e8abf084503733ULL, Role::Grab},
    {0xae8adbcd8245f8f5ULL, 0x198cf7cabd8e639cULL, Role::VerticalText},
    {0xaf23a9048ee892ddULL, 0x23a9e991083d3138ULL, Role::Grab},
    {0xaffa6e1a6bccf691ULL, 0x81f5194473f1de9bULL, Role::VerticalText},
    {0xb0627d0a4b3aa2dcULL, 0x611f5f8d4ed6c6ccULL, Role::ColSelect},
    {0xb16b59938e9097f5ULL, 0xde1eb2c07a091fd0ULL, Role::RowSelect},
    {0xb1a1f131fc87df98ULL, 0x71513ea5f896caf3ULL, Role::VerticalText},
    {0xb23027d74809d5fcULL, 0x00d2ba80ecedb9ceULL, Role::RowSelect},
    {0xb262d3235fa124b5ULL, 0xa14c0daaccc749c7ULL, Role::ColResize},
    {0xb2696d4fa1838924ULL, 0x037010e0daf1b5e7ULL, Role::ZoomIn},
    {0xb27e6b7c64b741fdULL, 0xc96060d874bfdea1ULL, Role::ColSelect},
    {0xb2acdbf7070472bdULL, 0xb0f1b8f1037779a7ULL, Role::Grab},
    {0xb4929e86c7d6158bULL, 0xea2570d50aa379acULL, Role::SelectionBar},
    {0xb49f3c16ad9377a7ULL, 0xd78ed10298dd81f9ULL, Role::Alias},
    {0xb50a857cf50f323fULL, 0x362f1459f5848218ULL, Role::ZoomOut},
    {0xb52ab70b988fecbcULL, 0xe95a353d0ed721aaULL, Role::Alias},
    {0xb610de4be533227bULL, 0x5682ec9b17c605c9ULL, Role::ColSelect},
    {0xb63b0f2708e11abfULL, 0xc502b18743155cb0ULL, Role::Grabbing},
    {0xb6f5979ea7691a1cULL, 0xcb291ee705c9de5bULL, Role::Alias},
    {0xb774a6493f61fb04ULL, 0x6ed084e0ebadfe72ULL, Role::Grab},
    {0xb862852aaa2f5b8aULL, 0x03b25e08b4c17306ULL, Role::Copy},
    {0xb8b78f463aac22cdULL, 0x370c7655f8ef2baaULL, Role::ColSelect},
    {0xba8161473f77bcc8ULL, 0xcca59e2f9a4eac3cULL, Role::Grab},
    {0xbab2c5521c73bd7dULL, 0xe3737f9a2b86d9afULL, Role::Grab},
    {0xbb6e7e01ff67f094ULL, 0x6841a1fd2c477e72ULL, Role::ColResize},
    {0xbb9763535ad071e3ULL, 0x056f51df40984f8eULL, Role::RowSelect},
    {0xbc390ac0cd9144b0ULL, 0xcb2706ca040d52c8ULL, Role::RowSelect},
    {0xbd23b8ff35856953ULL, 0x19ee38dcf50bd617ULL, Role::ColSelect},
    {0xbf7a5eb534bb62c9ULL, 0x5428815da5c6dedbULL, Role::RowSelect},
    {0xc0fff5b0224b4967ULL, 0xaa0105617f2cf42aULL, Role::RowResize},
    {0xc1906ff9540f4b99ULL, 0xf52c91e44ccc92f5ULL, Role::SelectionBar},
    {0xc1f0dffac78e50d5ULL, 0x3d2b077cd3197f76ULL, Role::RowSelect},
    {0xc2afe378fad0c4ecULL, 0x2a025d739ebbe8c1ULL, Role::Grab},
    {0xc39428f5bdbe6290ULL, 0xbc6a95f68a8b148eULL, Role::ZoomIn},
    {0xc5104bba701f8542ULL, 0x11f8814c789607dbULL, Role::ColResize},
    {0xc565b2258456a63fULL, 0xc35a29d85d74bd4fULL, Role::ZoomIn},
    {0xc5eb57037259061dULL, 0x39f99a49d11ed703ULL, Role::Grab},
    {0xc6052517455573fcULL, 0x05300d56135969cbULL, Role::ZoomIn},
    {0xc69dc07124fc7f6fULL, 0x53df44bea8557d8bULL, Role::ColSelect},
    {0xc6cce4073320be75ULL, 0x87009666b92fe182ULL, Role::Grab},
    {0xc7d3a4c7707d372bULL, 0xf2dbc74df7b34fd7ULL, Role::SelectionBar},
    {0xc8ec49ed21c5f692ULL, 0x438472a817be6a30ULL, Role::ColSelect},
    {0xc9431947ac6fd21dULL, 0x23ed31fd5224a8d6ULL, Role::Grabbing},
    {0xcb22eb0b64125b45ULL, 0xe8d6fa2276d91e67ULL, Role::Copy},
    {0xcc0c5c1ff6e7337dULL, 0xd1148eac5273aa86ULL, Role::Grab},
    {0xcea885fa8364ea67ULL, 0x169fdda990a0273bULL, Role::ColSelect},
    {0xcebaa6418e97003cULL, 0x8516b91abad118b5ULL, Role::Cell},
    {0xcf6543d7dd53c160ULL, 0x75d43aee6f20d46bULL, Role::ColSelect},
    {0xd02aac86a54060b4ULL, 0xb0fd619b47acbfd1ULL, Role::Grab},
    {0xd0ca026c0427f118ULL, 0x1a8bf2e21bb1108aULL, Role::ColSelect},
    {0xd0ebc9c05fbaf8bdULL, 0x86464908ec3d3501ULL, Role::Alias},
    {0xd0fe817d6cae9ef5ULL, 0x9667f60ba5ec14e1ULL, Role::ColResize},
    {0xd3330e69ed04514eULL, 0x556ab000b5177924ULL, Role::RowSelect},
    {0xd3815643707e4290ULL, 0x5cd98c48ac228879ULL, Role::ZoomOut},
    {0xd4169a7265f90532ULL, 0xf9d6d3af5757ef6dULL, Role::Grabbing},
    {0xd46ce094f8ddb2f6ULL, 0x7e5024b537fc8642ULL, Role::SelectionBar},
    {0xd49f93375488a40fULL, 0xc44cc7e6ae6d3beaULL, Role::RowSelect},
    {0xd622db49a5675709ULL, 0x916877dffb709d0cULL, Role::Cell},
    {0xd65f4bb8433f719eULL, 0x652b9dabe4bc2a3fULL, Role::RowSelect},
    {0xd71cc50fda38da64ULL, 0x1be043754e674251ULL, Role::Grab},
    {0xd7383372024bf69cULL, 0x4f418ecd73c0f600ULL, Role::RowSelect},
    {0xd79c44fed32bb3acULL, 0xd11e047aef9116e6ULL, Role::Grab},
    {0xd7bfe0824233860cULL, 0xfd9bb7e3feaf3d1bULL, Role::Grabbing},
    {0xda99f9d2064e06b1ULL, 0x403a5e01684681c3ULL, Role::RowSelect},
    {0xda9dde5075a41596ULL, 0x02f7d6ad39bc9435ULL, Role::RowSelect},
    {0xdb87828d3753ee70ULL, 0xf1518786cea94ddfULL, Role::ZoomIn},
    {0xdd1138e7950ce6e6ULL, 0xcf8acbefc8204ea2ULL, Role::Grab},
    {0xddaa53905fa6edebULL, 0xb27fff32fd0fdf65ULL, Role::ColSelect},
    {0xdecdfd04aad45b1dULL, 0xbe974cf65fa0b3a2ULL, Role::ZoomOut},
    {0xe10457650d75da84ULL, 0xfeb69f21ffa5038dULL, Role::Cell},
    {0xe3ad6cf691d2d73eULL, 0x1c798b3b7cb92e55ULL, Role::RowSelect},
    {0xe5754cdeb5fa89a7ULL, 0x4898145ed8e58a5fULL, Role::Alias},
    {0xe677537c5ebd2d26ULL, 0xfd6d76060cc45c95ULL, Role::ColSelect},
    {0xe67a6a057a953716ULL, 0xd056eeb5658d5b21ULL, Role::Grab},
    {0xe7cdab08272e1c64ULL, 0xf664758a592640f0ULL, Role::ZoomIn},
    {0xe8c97e0b914cff1fULL, 0xa2cc013b459c41a8ULL, Role::ColSelect},
    {0xea15e37b26a5e4f0ULL, 0x368db9dbbbc69afdULL, Role::ColSelect},
    {0xea2ba5f1fc714f44ULL, 0x1a6b5e0b5d9d1cc9ULL, Role::Cell},
    {0xeb31a4a365425835ULL, 0xe95380f7878c5176ULL, Role::Alias},
    {0xed9140e8a8a2d99eULL, 0xde0a4f67c12b5deaULL, Role::VerticalText},
    {0xede39db984cfbdf5ULL, 0x76bffef2beccd146ULL, Role::Copy},
    {0xedef7aafbd3252c3ULL, 0x7c270e4b790f974aULL, Role::Copy},
    {0xefa2d9334500c5c0ULL, 0xe87750b864208f2fULL, Role::Grabbing},
    {0xefcfba58f578f8e5ULL, 0x7cac1c1b7b0824d8ULL, Role::ZoomIn},
    {0xefe3f3a85c909409ULL, 0x80b6c9a4ceafcbfaULL, Role::RowSelect},
    {0xf118a27e8cc61d3cULL, 0x7a13a467e66e8d82ULL, Role::ZoomOut},
    {0xf20418c0a2dc3bbfULL, 0x5a63a9ff578089c6ULL, Role::RowSelect},
    {0xf2fd75052b3b9394ULL, 0xbb28e6d70a560c16ULL, Role::ColSelect},
    {0xf4545199bb6f34acULL, 0x0be32845fd1042c3ULL, Role::Grabbing},
    {0xf468acea653c64fdULL, 0x793c581bc69418f7ULL, Role::ZoomOut},
    {0xf5829183c2e4623cULL, 0xb6348bbd8cb74a64ULL, Role::ZoomOut},
    {0xf5d8d415d0c445ffULL, 0xf398bd5815a7dc61ULL, Role::Cell},
    {0xf5fd5fbff285c8e5ULL, 0xa358e163af6192d2ULL, Role::Copy},
    {0xf627df91d72b89f9ULL, 0x31a7cfca66c083adULL, Role::ZoomIn},
    {0xf750dc99a47e9723ULL, 0x253e6570b01cb761ULL, Role::Alias},
    {0xf7ea6c502a01e097ULL, 0x71bfa8e4b68c800dULL, Role::SelectionBar},
    {0xf829bee6e4777e76ULL, 0x8619ced2a6b9abcdULL, Role::RowSelect},
    {0xfa5d1ca2b8463590ULL, 0x4e8c26dec01f4b4aULL, Role::Grab},
    {0xfc4160058600aa31ULL, 0x7cb67801bbd2bc19ULL, Role::Grab},
    {0xfd70396d8eb16488ULL, 0xbea1ca65da99d4e6ULL, Role::ColResize},
    {0xfde0c9cbdf7f0a12ULL, 0xbf5896e0411b4e3eULL, Role::RowSelect},
    {0xfe9450e61e1911f5ULL, 0xfe972f6b1204ee0cULL, Role::Copy},
    {0xff17cd89acdb0ae5ULL, 0x2333a5e29903f9d0ULL, Role::RowSelect}
};

void HashBytes(Fingerprint& hash, const void* bytes, size_t count) {
    const auto* data = static_cast<const unsigned char*>(bytes);
    for (size_t i = 0; i < count; ++i) {
        hash.first = (hash.first ^ data[i]) * 1099511628211ULL;
        hash.second ^= data[i];
        hash.second = (hash.second << 7) | (hash.second >> 57);
        hash.second *= 14029467366897019727ULL;
    }
}

void HashNumber(Fingerprint& hash, uint32_t number) {
    unsigned char bytes[4] = {
        static_cast<unsigned char>(number), static_cast<unsigned char>(number >> 8),
        static_cast<unsigned char>(number >> 16), static_cast<unsigned char>(number >> 24)
    };
    HashBytes(hash, bytes, sizeof(bytes));
}

bool HashBitmap(Fingerprint& hash, HBITMAP bitmap, HDC dc) {
    HashNumber(hash, bitmap ? 1 : 0);
    if (!bitmap) {
        return true;
    }
    BITMAP object{};
    if (!GetObjectW(bitmap, sizeof(object), &object) || object.bmWidth <= 0 ||
        object.bmWidth > 256 || object.bmHeight <= 0 || object.bmHeight > 512) {
        return false;
    }
    size_t bytes = static_cast<size_t>(object.bmWidth) * object.bmHeight * 4;
    void* pixels = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes);
    if (!pixels) {
        return false;
    }
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = object.bmWidth;
    info.bmiHeader.biHeight = -object.bmHeight;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    bool ok = GetDIBits(dc, bitmap, 0, object.bmHeight, pixels, &info, DIB_RGB_COLORS)
              == object.bmHeight;
    if (ok) {
        HashNumber(hash, object.bmWidth);
        HashNumber(hash, object.bmHeight);
        HashBytes(hash, pixels, bytes);
    }
    HeapFree(GetProcessHeap(), 0, pixels);
    return ok;
}

bool GetFingerprint(HCURSOR cursor, Fingerprint& hash) {
    ICONINFO info{};
    if (!cursor || !GetIconInfo(cursor, &info)) {
        return false;
    }
    HDC dc = info.fIcon ? nullptr : CreateCompatibleDC(nullptr);
    HashNumber(hash, info.xHotspot);
    HashNumber(hash, info.yHotspot);
    bool ok = dc && HashBitmap(hash, info.hbmMask, dc) && HashBitmap(hash, info.hbmColor, dc);
    if (dc) DeleteDC(dc);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    if (info.hbmColor) DeleteObject(info.hbmColor);
    return ok;
}

Role RecognizeFingerprint(const Fingerprint& hash) {
    for (const auto& signature : kSignatures) {
        if (signature.first == hash.first && signature.second == hash.second) {
            return signature.role;
        }
    }
    return Role::Unknown;
}

struct Settings {
    std::wstring paths[kRoleCount];
};

struct OwnedCursor {
    HCURSOR cursor = nullptr;
    std::wstring path;
    DWORD cost = 0;
};

struct CachedRole {
    HCURSOR cursor = nullptr;
    Role role = Role::Unknown;
    uint64_t generation = 0;
    bool ready = false;
};

struct ThreadCursor {
    DWORD id = 0;
    HANDLE thread = nullptr;
    HCURSOR logical = nullptr;
    HCURSOR displayed = nullptr;
};

static SRWLOCK g_lock = SRWLOCK_INIT;
static Settings g_settings;
static OwnedCursor g_pool[kPoolSize];
static std::atomic<HCURSOR> g_owned[kPoolSize]{};
static CachedRole g_cache[256];
static ThreadCursor g_threads[128];
static bool g_loadAttempted[kRoleCount]{};
static size_t g_cacheNext = 0;
static bool g_needsPrune = false;
struct CursorDestruction {
    HCURSOR cursor;
    CursorDestruction* next;
};

static SRWLOCK g_cacheLock = SRWLOCK_INIT;
static uint64_t g_cacheGeneration = 0;
static CursorDestruction* g_destroying = nullptr;
static HCURSOR g_systemCursors[18]{};
static std::atomic<bool> g_stopping{false};
static std::atomic<bool> g_hasPaths{false};
static thread_local bool g_inHook = false;
static thread_local HCURSOR g_threadLogical = nullptr;
static thread_local HCURSOR g_threadDisplayed = nullptr;

using SetCursor_t = decltype(&SetCursor);
using GetCursor_t = decltype(&GetCursor);
using DestroyCursor_t = decltype(&DestroyCursor);
using DestroyIcon_t = decltype(&DestroyIcon);
static SetCursor_t SetCursor_Original = nullptr;
static GetCursor_t GetCursor_Original = nullptr;
static DestroyCursor_t DestroyCursor_Original = nullptr;
static DestroyIcon_t DestroyIcon_Original = nullptr;

class HookGuard {
public:
    HookGuard() { g_inHook = true; }
    ~HookGuard() { g_inHook = false; }
};

bool IsOwned(HCURSOR cursor) {
    if (!cursor) return false;
    for (const auto& owned : g_owned) {
        if (owned.load(std::memory_order_acquire) == cursor) return true;
    }
    return false;
}

bool IsConfigured(PCWSTR path) {
    for (const auto& configured : g_settings.paths) {
        if (!configured.empty() && _wcsicmp(configured.c_str(), path) == 0) return true;
    }
    return false;
}

void ReapThreads() {
    for (auto& state : g_threads) {
        if (state.thread && WaitForSingleObject(state.thread, 0) == WAIT_OBJECT_0) {
            CloseHandle(state.thread);
            state = {};
        }
    }
}

bool IsInUse(HCURSOR cursor) {
    for (const auto& state : g_threads) {
        if (state.thread && state.displayed == cursor) return true;
    }
    CURSORINFO info{};
    info.cbSize = sizeof(info);
    return !GetCursorInfo(&info) || info.hCursor == cursor;
}

void PrunePool(bool all = false) {
    ReapThreads();
    g_needsPrune = false;
    for (size_t i = 0; i < kPoolSize; ++i) {
        auto& owned = g_pool[i];
        if (owned.cursor && (all || !IsConfigured(owned.path.c_str())) && !IsInUse(owned.cursor)) {
            g_owned[i].store(nullptr, std::memory_order_release);
            BOOL destroyed = DestroyCursor_Original ? DestroyCursor_Original(owned.cursor) : DestroyCursor(owned.cursor);
            if (destroyed) {
                owned = {};
            } else {
                g_owned[i].store(owned.cursor, std::memory_order_release);
            }
        }
        if (owned.cursor && !IsConfigured(owned.path.c_str())) g_needsPrune = true;
    }
}

ThreadCursor* FindThread(bool create) {
    DWORD id = GetCurrentThreadId();
    for (auto& state : g_threads) {
        if (state.id == id && state.thread && WaitForSingleObject(state.thread, 0) == WAIT_TIMEOUT) return &state;
    }
    if (!create) return nullptr;
    ReapThreads();
    for (auto& state : g_threads) {
        if (!state.thread) {
            HANDLE thread = OpenThread(SYNCHRONIZE, FALSE, id);
            if (!thread) return nullptr;
            state = {id, thread, nullptr, nullptr};
            return &state;
        }
    }
    return nullptr;
}

Role IdentifyCursor(HCURSOR cursor) {
    if (!cursor || IsOwned(cursor)) return Role::Unknown;
    for (HCURSOR standard : g_systemCursors) {
        if (cursor == standard) return Role::Unknown;
    }
    AcquireSRWLockExclusive(&g_cacheLock);
    for (auto* pending = g_destroying; pending; pending = pending->next) {
        if (pending->cursor == cursor) {
            ReleaseSRWLockExclusive(&g_cacheLock);
            return Role::Unknown;
        }
    }
    for (const auto& cached : g_cache) {
        if (cached.cursor == cursor && cached.ready) {
            Role result = cached.role;
            ReleaseSRWLockExclusive(&g_cacheLock);
            return result;
        }
    }
    size_t slot = g_cacheNext++ % ARRAYSIZE(g_cache);
    uint64_t generation = ++g_cacheGeneration;
    g_cache[slot] = {cursor, Role::Unknown, generation, false};
    ReleaseSRWLockExclusive(&g_cacheLock);
    Role role = Role::Unknown;
    ICONINFOEXW info{};
    info.cbSize = sizeof(info);
    if (GetIconInfoExW(cursor, &info)) {
        if (info.hbmMask) DeleteObject(info.hbmMask);
        if (info.hbmColor) DeleteObject(info.hbmColor);
        if (!info.fIcon && info.szModName[0] && (info.wResID || info.szResName[0])) {
            HMODULE module = nullptr;
            if (GetModuleHandleExW(0, info.szModName, &module)) {
                PCWSTR resource = info.wResID ? MAKEINTRESOURCEW(info.wResID) : info.szResName;
                HCURSOR normalized = static_cast<HCURSOR>(LoadImageW(module, resource, IMAGE_CURSOR, 32, 32, 0));
                if (normalized) {
                    Fingerprint hash;
                    if (GetFingerprint(normalized, hash)) {
                        role = RecognizeFingerprint(hash);
                        if (role == Role::Unknown) {
                            Wh_Log(L"Unrecognized resource cursor: %016llx %016llx",
                                   static_cast<unsigned long long>(hash.first),
                                   static_cast<unsigned long long>(hash.second));
                        }
                    }
                    if (DestroyCursor_Original) DestroyCursor_Original(normalized);
                    else DestroyCursor(normalized);
                }
                FreeLibrary(module);
            }
        }
    }
    if (role == Role::Unknown) {
        Fingerprint hash;
        if (GetFingerprint(cursor, hash)) role = RecognizeFingerprint(hash);
    }
    AcquireSRWLockExclusive(&g_cacheLock);
    auto& cached = g_cache[slot];
    if (cached.cursor == cursor && cached.generation == generation) {
        cached.role = role;
        cached.ready = true;
    } else {
        role = Role::Unknown;
    }
    ReleaseSRWLockExclusive(&g_cacheLock);
    return role;
}

bool ReadGuiCount(HANDLE process, DWORD& count) {
    SetLastError(ERROR_SUCCESS);
    count = GetGuiResources(process, GR_USEROBJECTS);
    return count != 0 || GetLastError() == ERROR_SUCCESS;
}

bool InspectCursorFile(PCWSTR path, DWORD& cost) {
    if (!path || wcslen(path) < 3 || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/')) {
        SetLastError(ERROR_BAD_PATHNAME);
        return false;
    }
    WCHAR root[] = {path[0], L':', L'\\', 0};
    UINT driveType = GetDriveTypeW(root);
    if (driveType != DRIVE_FIXED && driveType != DRIVE_REMOVABLE && driveType != DRIVE_RAMDISK) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return false;
    }
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!GetFileAttributesExW(path, GetFileExInfoStandard, &attributes)) return false;
    if (attributes.nFileSizeHigh || attributes.nFileSizeLow > 16 * 1024 * 1024 ||
        (attributes.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE))) {
        SetLastError(ERROR_INVALID_DATA);
        return false;
    }
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD header[3]{};
    DWORD read = 0;
    bool ok = ReadFile(file, header, sizeof(header), &read, nullptr) && read == sizeof(header);
    cost = 2;
    if (ok && (header[0] & 0xffffFFFFu) == 0x00020000u) {
        WORD count = static_cast<WORD>(header[1]);
        ok = count > 0 && count <= 32;
    } else if (ok && header[0] == 0x46464952u && header[2] == 0x4e4f4341u) {
        bool found = false;
        uint64_t offset = 12;
        while (ok && offset + 8 <= attributes.nFileSizeLow) {
            LARGE_INTEGER position{};
            position.QuadPart = offset;
            DWORD chunk[2]{};
            ok = SetFilePointerEx(file, position, nullptr, FILE_BEGIN) &&
                 ReadFile(file, chunk, sizeof(chunk), &read, nullptr) && read == sizeof(chunk);
            if (!ok || offset + 8 + chunk[1] > attributes.nFileSizeLow) break;
            if (chunk[0] == 0x68696e61u && chunk[1] >= 36) {
                DWORD ani[9]{};
                ok = ReadFile(file, ani, sizeof(ani), &read, nullptr) && read == sizeof(ani) &&
                     ani[0] >= 36 && ani[1] >= 1 && ani[1] <= 180 && ani[2] <= 4096 &&
                     ani[3] <= 256 && ani[4] <= 256;
                if (ok) cost = 2 * (ani[1] + 1);
                found = ok;
                break;
            }
            offset += 8ULL + chunk[1] + (chunk[1] & 1);
        }
        ok = ok && found;
    } else {
        ok = false;
    }
    CloseHandle(file);
    if (!ok) SetLastError(ERROR_INVALID_DATA);
    return ok;
}

HCURSOR GetReplacement(Role role) {
    size_t index = static_cast<size_t>(role);
    if (index >= kRoleCount || g_settings.paths[index].empty()) return nullptr;
    const auto& path = g_settings.paths[index];
    for (const auto& owned : g_pool) {
        if (owned.cursor && _wcsicmp(owned.path.c_str(), path.c_str()) == 0) return owned.cursor;
    }
    if (g_loadAttempted[index]) return nullptr;
    g_loadAttempted[index] = true;
    std::wstring storedPath;
    try {
        storedPath = path;
    } catch (...) {
        Wh_Log(L"Replacement unavailable: %s; insufficient memory. Save settings to retry.", kSettingNames[index]);
        return nullptr;
    }
    PrunePool();
    size_t freeIndex = kPoolSize;
    DWORD ownedCost = 0;
    for (size_t i = 0; i < kPoolSize; ++i) {
        ownedCost += g_pool[i].cost;
        if (!g_pool[i].cursor && freeIndex == kPoolSize) freeIndex = i;
    }
    DWORD expected = 0, processBefore = 0;
    SetLastError(ERROR_SUCCESS);
    HCURSOR cursor = nullptr;
    if (freeIndex < kPoolSize && InspectCursorFile(path.c_str(), expected) &&
        ReadGuiCount(GetCurrentProcess(), processBefore) &&
        processBefore + expected < kProcessUserLimit &&
        ownedCost + expected <= kOwnedUserLimit) {
        cursor = static_cast<HCURSOR>(LoadImageW(nullptr, path.c_str(), IMAGE_CURSOR, 0, 0,
                                                LR_LOADFROMFILE | LR_DEFAULTSIZE));
        if (cursor) {
            DWORD processAfter = 0;
            bool measured = ReadGuiCount(GetCurrentProcess(), processAfter);
            DWORD actual = processAfter > processBefore ? processAfter - processBefore : expected;
            actual = (std::max)(actual, expected);
            if (!measured || processAfter >= kProcessUserLimit ||
                ownedCost + actual > kOwnedUserLimit) {
                DWORD error = measured ? ERROR_NOT_ENOUGH_MEMORY : GetLastError();
                DestroyCursor_Original(cursor);
                cursor = nullptr;
                SetLastError(error);
            } else {
                g_pool[freeIndex] = {cursor, std::move(storedPath), actual};
                g_owned[freeIndex].store(cursor, std::memory_order_release);
            }
        }
    }
    if (!cursor) {
        DWORD error = GetLastError();
        Wh_Log(L"Replacement unavailable: %s; Windows error %lu. Save settings to retry.",
               kSettingNames[index], error ? error : ERROR_NOT_ENOUGH_MEMORY);
    }
    return cursor;
}

HCURSOR InvokeSetCursor(HCURSOR logical, HCURSOR displayed, DWORD error) {
    SetLastError(error);
    HCURSOR previous = SetCursor_Original(displayed);
    if (previous && previous == g_threadDisplayed) previous = g_threadLogical;
    g_threadLogical = logical;
    g_threadDisplayed = displayed;
    return previous;
}

HCURSOR WINAPI SetCursor_Hook(HCURSOR cursor) {
    if (g_inHook) return SetCursor_Original(cursor);
    DWORD error = GetLastError();
    HookGuard guard;
    if (!TryAcquireSRWLockExclusive(&g_lock)) {
        return InvokeSetCursor(cursor, cursor, error);
    }
    ThreadCursor* state = FindThread(false);
    HCURSOR logical = cursor;
    if (state && cursor && cursor == state->displayed && IsOwned(cursor)) logical = state->logical;
    HCURSOR displayed = logical;
    Role role = Role::Unknown;
    if (!g_stopping.load() && g_hasPaths.load()) {
        role = IdentifyCursor(logical);
        if (role != Role::Unknown) {
            if (!state) state = FindThread(true);
            if (state) {
                if (HCURSOR replacement = GetReplacement(role)) displayed = replacement;
            }
        }
    }
    HCURSOR previous = InvokeSetCursor(logical, displayed, error);
    DWORD resultError = GetLastError();
    if (state) {
        state->logical = logical;
        state->displayed = displayed;
    }
    if (g_needsPrune) PrunePool();
    ReleaseSRWLockExclusive(&g_lock);
    static thread_local Role lastLogged = Role::Unknown;
    if (role != Role::Unknown && role != lastLogged) {
        lastLogged = role;
        Wh_Log(L"Recognized: %s | Replacement applied: %d", kSettingNames[static_cast<size_t>(role)],
               displayed != logical);
    }
    SetLastError(resultError);
    return previous;
}

HCURSOR WINAPI GetCursor_Hook() {
    HCURSOR cursor = GetCursor_Original();
    if (g_inHook) return cursor;
    if (cursor && cursor == g_threadDisplayed) cursor = g_threadLogical;
    return cursor;
}

void ForgetCursor(HCURSOR cursor) {
    for (auto& cached : g_cache) {
        if (cached.cursor == cursor) cached = {};
    }
}

void BeginCursorDestruction(CursorDestruction& pending, HCURSOR cursor) {
    AcquireSRWLockExclusive(&g_cacheLock);
    pending = {cursor, g_destroying};
    g_destroying = &pending;
    ForgetCursor(cursor);
    ReleaseSRWLockExclusive(&g_cacheLock);
}

void EndCursorDestruction(CursorDestruction& pending) {
    AcquireSRWLockExclusive(&g_cacheLock);
    auto** link = &g_destroying;
    while (*link && *link != &pending) link = &(*link)->next;
    if (*link) *link = pending.next;
    ForgetCursor(pending.cursor);
    ReleaseSRWLockExclusive(&g_cacheLock);
}

BOOL WINAPI DestroyCursor_Hook(HCURSOR cursor) {
    if (g_inHook) return DestroyCursor_Original(cursor);
    DWORD error = GetLastError();
    HookGuard guard;
    if (IsOwned(cursor)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    CursorDestruction pending{};
    BeginCursorDestruction(pending, cursor);
    SetLastError(error);
    BOOL result = DestroyCursor_Original(cursor);
    DWORD resultError = GetLastError();
    EndCursorDestruction(pending);
    SetLastError(resultError);
    return result;
}

BOOL WINAPI DestroyIcon_Hook(HICON icon) {
    if (g_inHook) return DestroyIcon_Original(icon);
    DWORD error = GetLastError();
    HookGuard guard;
    HCURSOR cursor = reinterpret_cast<HCURSOR>(icon);
    if (IsOwned(cursor)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }
    CursorDestruction pending{};
    BeginCursorDestruction(pending, cursor);
    SetLastError(error);
    BOOL result = DestroyIcon_Original(icon);
    DWORD resultError = GetLastError();
    EndCursorDestruction(pending);
    SetLastError(resultError);
    return result;
}

void LoadSettings() {
    Settings settings;
    for (size_t i = 0; i < kRoleCount; ++i) {
        settings.paths[i] = WindhawkUtils::StringSetting::make(kSettingNames[i]).get();
    }
    AcquireSRWLockExclusive(&g_lock);
    bool any = false;
    bool changed = false;
    for (size_t i = 0; i < kRoleCount; ++i) {
        g_loadAttempted[i] = false;
        if (settings.paths[i] != g_settings.paths[i]) {
            changed = true;
        }
        any = any || !settings.paths[i].empty();
    }
    g_settings = std::move(settings);
    g_needsPrune = g_needsPrune || changed;
    g_hasPaths.store(any);
    ReleaseSRWLockExclusive(&g_lock);
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();
    } catch (...) {
        return FALSE;
    }
    if (!g_hasPaths.load()) return FALSE;
    const PCWSTR systemIds[] = {
        IDC_ARROW, IDC_IBEAM, IDC_WAIT, IDC_CROSS, IDC_UPARROW, IDC_SIZE,
        IDC_ICON, IDC_SIZENWSE, IDC_SIZENESW, IDC_SIZEWE, IDC_SIZENS,
        IDC_SIZEALL, IDC_NO, IDC_HAND, IDC_APPSTARTING, IDC_HELP, MAKEINTRESOURCEW(32671), MAKEINTRESOURCEW(32672)
    };
    static_assert(ARRAYSIZE(systemIds) == ARRAYSIZE(g_systemCursors));
    for (size_t i = 0; i < ARRAYSIZE(systemIds); ++i) {
        g_systemCursors[i] = LoadCursorW(nullptr, systemIds[i]);
    }
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto destroyCursor = reinterpret_cast<DestroyCursor_t>(GetProcAddress(user32, "DestroyCursor"));
    auto destroyIcon = reinterpret_cast<DestroyIcon_t>(GetProcAddress(user32, "DestroyIcon"));
    if (!destroyCursor || !destroyIcon ||
        !WindhawkUtils::SetFunctionHook(SetCursor, SetCursor_Hook, &SetCursor_Original) ||
        !WindhawkUtils::SetFunctionHook(GetCursor, GetCursor_Hook, &GetCursor_Original) ||
        !WindhawkUtils::SetFunctionHook(destroyCursor, DestroyCursor_Hook, &DestroyCursor_Original)) {
        Wh_Log(L"Initialization failed while registering cursor hooks");
        return FALSE;
    }
    if (reinterpret_cast<void*>(destroyCursor) != reinterpret_cast<void*>(destroyIcon) &&
        !WindhawkUtils::SetFunctionHook(destroyIcon, DestroyIcon_Hook, &DestroyIcon_Original)) {
        Wh_Log(L"Initialization failed while registering DestroyIcon hook");
        return FALSE;
    }
    Wh_Log(L"Cursor Override initialized");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
    } catch (...) {
        Wh_Log(L"Settings update failed; previous settings retained");
    }
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true);
}

void Wh_ModUninit() {
    AcquireSRWLockExclusive(&g_lock);
    DestroyCursor_Original = nullptr;
    PrunePool(true);
    for (auto& state : g_threads) {
        if (state.thread) CloseHandle(state.thread);
        state = {};
    }
    ReleaseSRWLockExclusive(&g_lock);
}
