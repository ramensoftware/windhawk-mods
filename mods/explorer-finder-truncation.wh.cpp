// ==WindhawkMod==
// @id           explorer-finder-truncation
// @name         Explorer Finder-style Truncation
// @description  Makes Explorer use middle ellipsis for long item names.
// @version      1.0.1
// @author       Kayhan
// @include      explorer.exe
// @architecture x86-64
// @compilerOptions -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Finder-style Truncation

Makes long Explorer item names use middle ellipsis.

Example:

    VeryLongFileNameThatContainsImportantInformation.txt

becomes:

    VeryLongFile…Information.txt

It Has A Little Flicker. That Is A Little Bug.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <string>
#include <cwctype>
#include <climits>

using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);

static DrawTextW_t DrawTextW_Original = nullptr;
static DrawTextExW_t DrawTextExW_Original = nullptr;

static bool g_enabled = true;


// ------------------------------------------------------------
// Settings
// ------------------------------------------------------------

static void LoadSettings()
{
    g_enabled =
        Wh_GetIntSetting(L"ellipsis") != 0;
}


// ------------------------------------------------------------
// Text measurement
// ------------------------------------------------------------

static int GetTextWidth(
    HDC hdc,
    LPCWSTR text,
    int length
)
{
    if (!hdc || !text || length <= 0)
        return 0;

    SIZE size{};

    if (!GetTextExtentPoint32W(
            hdc,
            text,
            length,
            &size))
    {
        return INT_MAX;
    }

    return size.cx;
}


static int GetTextWidth(
    HDC hdc,
    const std::wstring& text
)
{
    return GetTextWidth(
        hdc,
        text.c_str(),
        static_cast<int>(text.length())
    );
}


// ------------------------------------------------------------
// Filename detection
// ------------------------------------------------------------

static bool LooksLikeItemName(
    LPCWSTR text,
    int length
)
{
    if (!text || length <= 0)
        return false;

    if (length > 1024)
        return false;

    bool useful = false;

    for (int i = 0; i < length; ++i)
    {
        wchar_t c = text[i];

        if (c < L' ')
            return false;

        if (c == L'\\' || c == L'/')
            return false;

        if (
            c == L'.' ||
            c == L'_' ||
            c == L'-' ||
            std::iswalnum(c)
        )
        {
            useful = true;
        }
    }

    return useful;
}


// ------------------------------------------------------------
// Extension
// ------------------------------------------------------------

static size_t FindExtension(
    const std::wstring& text
)
{
    size_t dot =
        text.find_last_of(L'.');

    if (dot == std::wstring::npos)
        return std::wstring::npos;

    if (dot == 0)
        return std::wstring::npos;

    if (dot == text.length() - 1)
        return std::wstring::npos;

    return dot;
}


// ------------------------------------------------------------
// Candidate construction
// ------------------------------------------------------------

static std::wstring MakeCandidate(
    const std::wstring& stem,
    const std::wstring& extension,
    size_t left,
    size_t right
)
{
    std::wstring result;

    result.reserve(
        left +
        1 +
        right +
        extension.length()
    );

    result.append(
        stem,
        0,
        left
    );

    result += L'\x2026';

    if (right > 0)
    {
        result.append(
            stem,
            stem.length() - right,
            right
        );
    }

    result += extension;

    return result;
}


// ------------------------------------------------------------
// Finder-style truncation
// ------------------------------------------------------------

static std::wstring FinderTruncate(
    HDC hdc,
    const std::wstring& original,
    int availableWidth
)
{
    if (original.empty())
        return original;

    if (availableWidth <= 0)
        return L"";


    // Already fits: do absolutely nothing.

    if (
        GetTextWidth(
            hdc,
            original
        ) <= availableWidth
    )
    {
        return original;
    }


    const size_t extensionPos =
        FindExtension(original);


    std::wstring stem;
    std::wstring extension;


    if (extensionPos != std::wstring::npos)
    {
        stem =
            original.substr(
                0,
                extensionPos
            );

        extension =
            original.substr(
                extensionPos
            );
    }
    else
    {
        stem = original;
    }


    if (stem.empty())
        return L"\x2026";


    const std::wstring ellipsis =
        L"\x2026";


    const int ellipsisWidth =
        GetTextWidth(
            hdc,
            ellipsis
        );


    if (
        ellipsisWidth <= 0 ||
        ellipsisWidth >= availableWidth
    )
    {
        return ellipsis;
    }


    /*
     * Find the largest filename that fits the EXACT
     * width supplied by Explorer.
     *
     * There is no max_chars.
     * There is no fixed filename length.
     */

    size_t bestLeft = 0;
    size_t bestRight = 0;

    int bestWidth = 0;


    const int extensionWidth =
        extension.empty()
            ? 0
            : GetTextWidth(
                hdc,
                extension
            );


    if (extensionWidth == INT_MAX)
        return ellipsis;


    for (
        size_t left = 1;
        left < stem.length();
        ++left
    )
    {
        const int leftWidth =
            GetTextWidth(
                hdc,
                stem.c_str(),
                static_cast<int>(left)
            );


        if (leftWidth == INT_MAX)
            break;


        const int remaining =
            availableWidth -
            leftWidth -
            ellipsisWidth -
            extensionWidth;


        if (remaining <= 0)
            continue;


        size_t bestRightForLeft = 0;


        for (
            size_t right = 1;
            right <= stem.length() - left;
            ++right
        )
        {
            const size_t start =
                stem.length() - right;


            const int rightWidth =
                GetTextWidth(
                    hdc,
                    stem.c_str() + start,
                    static_cast<int>(right)
                );


            if (rightWidth == INT_MAX)
                break;


            if (rightWidth <= remaining)
            {
                bestRightForLeft = right;
            }
            else
            {
                break;
            }
        }


        if (bestRightForLeft == 0)
            continue;


        std::wstring candidate =
            MakeCandidate(
                stem,
                extension,
                left,
                bestRightForLeft
            );


        const int candidateWidth =
            GetTextWidth(
                hdc,
                candidate
            );


        if (
            candidateWidth <= availableWidth &&
            candidateWidth > bestWidth
        )
        {
            bestWidth = candidateWidth;
            bestLeft = left;
            bestRight = bestRightForLeft;
        }
    }


    if (bestLeft && bestRight)
    {
        return MakeCandidate(
            stem,
            extension,
            bestLeft,
            bestRight
        );
    }


    /*
     * Extremely narrow area:
     *
     * …extension
     */

    if (!extension.empty())
    {
        std::wstring candidate =
            ellipsis + extension;


        if (
            GetTextWidth(
                hdc,
                candidate
            ) <= availableWidth
        )
        {
            return candidate;
        }
    }


    return ellipsis;
}


// ------------------------------------------------------------
// DrawText filtering
// ------------------------------------------------------------

static bool ShouldModify(
    UINT format
)
{
    if (!(format & DT_END_ELLIPSIS))
        return false;

    if (format & DT_PATH_ELLIPSIS)
        return false;

    if (format & DT_CALCRECT)
        return false;

    return true;
}


// ------------------------------------------------------------
// DrawTextW hook
// ------------------------------------------------------------

static int WINAPI DrawTextW_Hook(
    HDC hdc,
    LPCWSTR text,
    int count,
    LPRECT rect,
    UINT format
)
{
    if (
        !g_enabled ||
        !hdc ||
        !text ||
        !rect ||
        count == 0 ||
        !ShouldModify(format)
    )
    {
        return DrawTextW_Original(
            hdc,
            text,
            count,
            rect,
            format
        );
    }


    int length = count;

    if (length < 0)
    {
        length =
            static_cast<int>(
                wcslen(text)
            );
    }


    if (!LooksLikeItemName(
            text,
            length
        ))
    {
        return DrawTextW_Original(
            hdc,
            text,
            count,
            rect,
            format
        );
    }


    const int availableWidth =
        rect->right -
        rect->left;


    if (availableWidth <= 0)
    {
        return DrawTextW_Original(
            hdc,
            text,
            count,
            rect,
            format
        );
    }


    std::wstring original(
        text,
        text + length
    );


    std::wstring shortened =
        FinderTruncate(
            hdc,
            original,
            availableWidth
        );


    if (
        shortened.empty() ||
        shortened == original
    )
    {
        return DrawTextW_Original(
            hdc,
            text,
            count,
            rect,
            format
        );
    }


    Wh_Log(
        L"[Finder] \"%s\" -> \"%s\" width=%d",
        original.c_str(),
        shortened.c_str(),
        availableWidth
    );


    return DrawTextW_Original(
        hdc,
        shortened.c_str(),
        static_cast<int>(
            shortened.length()
        ),
        rect,
        format
    );
}


// ------------------------------------------------------------
// DrawTextExW hook
// ------------------------------------------------------------

static int WINAPI DrawTextExW_Hook(
    HDC hdc,
    LPWSTR text,
    int count,
    LPRECT rect,
    UINT format,
    LPDRAWTEXTPARAMS params
)
{
    if (
        !g_enabled ||
        !hdc ||
        !text ||
        !rect ||
        count == 0 ||
        !ShouldModify(format)
    )
    {
        return DrawTextExW_Original(
            hdc,
            text,
            count,
            rect,
            format,
            params
        );
    }


    int length = count;

    if (length < 0)
    {
        length =
            static_cast<int>(
                wcslen(text)
            );
    }


    if (!LooksLikeItemName(
            text,
            length
        ))
    {
        return DrawTextExW_Original(
            hdc,
            text,
            count,
            rect,
            format,
            params
        );
    }


    const int availableWidth =
        rect->right -
        rect->left;


    if (availableWidth <= 0)
    {
        return DrawTextExW_Original(
            hdc,
            text,
            count,
            rect,
            format,
            params
        );
    }


    std::wstring original(
        text,
        text + length
    );


    std::wstring shortened =
        FinderTruncate(
            hdc,
            original,
            availableWidth
        );


    if (
        shortened.empty() ||
        shortened == original
    )
    {
        return DrawTextExW_Original(
            hdc,
            text,
            count,
            rect,
            format,
            params
        );
    }


    Wh_Log(
        L"[Finder] \"%s\" -> \"%s\" width=%d",
        original.c_str(),
        shortened.c_str(),
        availableWidth
    );


    return DrawTextExW_Original(
        hdc,
        const_cast<LPWSTR>(
            shortened.c_str()
        ),
        static_cast<int>(
            shortened.length()
        ),
        rect,
        format,
        params
    );
}


// ------------------------------------------------------------
// Initialization
// ------------------------------------------------------------

BOOL Wh_ModInit()
{
    LoadSettings();


    HMODULE user32 =
        GetModuleHandleW(
            L"user32.dll"
        );


    if (!user32)
        return FALSE;


    auto drawText =
        reinterpret_cast<DrawTextW_t>(
            GetProcAddress(
                user32,
                "DrawTextW"
            )
        );


    auto drawTextEx =
        reinterpret_cast<DrawTextExW_t>(
            GetProcAddress(
                user32,
                "DrawTextExW"
            )
        );


    if (!drawText || !drawTextEx)
        return FALSE;


    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(
                drawText
            ),
            reinterpret_cast<void*>(
                DrawTextW_Hook
            ),
            reinterpret_cast<void**>(
                &DrawTextW_Original
            )
        ))
    {
        Wh_Log(
            L"Failed to hook DrawTextW"
        );

        return FALSE;
    }


    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(
                drawTextEx
            ),
            reinterpret_cast<void*>(
                DrawTextExW_Hook
            ),
            reinterpret_cast<void**>(
                &DrawTextExW_Original
            )
        ))
    {
        Wh_Log(
            L"Failed to hook DrawTextExW"
        );

        return FALSE;
    }


    Wh_Log(
        L"Explorer Finder-style truncation loaded"
    );


    return TRUE;
}


void Wh_ModSettingsChanged()
{
    LoadSettings();
}
