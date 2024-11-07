// ==WindhawkMod==
// @id              explorer-details-better-file-sizes
// @name            Better file sizes in Explorer details
// @description     Optional improvements: show folder sizes, use MB/GB for large files (by default, all sizes are shown in KBs), use IEC terms (such as KiB instead of KB)
// @version         1.2.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lole32 -loleaut32 -lpropsys
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Better file sizes in Explorer details

![Screenshot](https://i.imgur.com/aEaxCWe.png)

This mod offers the following optional improvements to file sizes in File
Explorer's details view:

## Show folder sizes

Explorer doesn't show folder sizes. The mod adds this ability, which can be
enabled in the mod settings using one of the following methods:

### Via "Everything" integration

[Everything](https://www.voidtools.com/) is a search engine that locates files
and folders by filename instantly for Windows. It's a great tool which is useful
on its own, but it can also be used with this mod to instantly get folder sizes
without having to calculate them manually.

To show folder sizes via "Everything" integration:

* [Download](https://www.voidtools.com/downloads/) and install "Everything".
  * Note that the integration won't work with the Lite version.
* Enable folder size indexing in "Everything":
  * Open **"Everything"**.
  * From the Tools menu, click **Options**.
  * Click the **Indexes** tab.
  * Check **Index file size**.
  * Check **Index folder size**.
  * Click **OK**.
* Set **Show folder sizes** in the mod's settings to **Enabled via "Everything"
  integration**.

Note: "Everything" must be running for the integration to work.

### Calculated manually

If you prefer to avoid installing "Everything", you can enable folder sizes and
have them calculated manually. Since calculating folder sizes can be slow, it's
not enabled by default, and there's an option to enable it only while holding
the Shift key.

## Use MB/GB for large files

Explorer always shows file sizes in KBs in details, make it use MB/GB when
appropriate.

## Use IEC terms

Use the International Electronic Commission terms: KB → KiB, MB → MiB, GB → GiB.

For a curious read on the topic, check out the following old (2009) blog post by
a Microsoft employee: [Why does Explorer use the term KB instead of
KiB?](https://devblogs.microsoft.com/oldnewthing/20090611-00/?p=17933).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- calculateFolderSizes: disabled
  $name: Show folder sizes
  $description: >-
    The recommended option to enable folder sizes is via "Everything"
    integration. Refer to the mod description for the steps of installing and
    configuring "Everything".

    An alternative option is to calculate folder sizes manually. This can be
    slow, and there's an option to enable it only while holding the Shift key.
    In this case, folder sizes will only be shown if the Shift key is held when
    the list is loaded or refreshed. For example, select a folder, hold Shift
    and press Enter to navigate to it. Another example: hold Shift and click
    refresh, back or forward.
  $options:
  - disabled: Disabled
  - everything: Enabled via "Everything" integration
  - always: Enabled, calculated manually (can be slow)
  - withShiftKey: Enabled, calculated manually while holding the Shift key
- disableKbOnlySizes: true
  $name: Use MB/GB for large files
  $description: >-
    By default, sizes are shown in KBs
- useIecTerms: false
  $name: Use IEC terms
  $description: >-
    Use the International Electronic Commission terms, such as KiB instead of KB
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <map>
#include <memory>
#include <optional>

#include <initguid.h>

#include <propsys.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shtypes.h>
#include <winrt/base.h>

enum class CalculateFolderSizes {
    disabled,
    everything,
    withShiftKey,
    always,
};

struct {
    CalculateFolderSizes calculateFolderSizes;
    bool disableKbOnlySizes;
    bool useIecTerms;
} g_settings;

HMODULE g_propsysModule;
std::atomic<int> g_hookRefCount;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// Partial version of the Everything SDK to be able to query folder sizes.
// https://www.voidtools.com/support/everything/sdk/
#pragma region everything_sdk
// clang-format off

//
// Copyright (C) 2016 David Carpenter
// 
// Permission is hereby granted, free of charge, 
// to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), 
// to deal in the Software without restriction, 
// including without limitation the rights to use, 
// copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit 
// persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be 
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#define EVERYTHING_ERROR_MEMORY								1 // out of memory.
#define EVERYTHING_ERROR_IPC								2 // Everything search client is not running
#define EVERYTHING_ERROR_REGISTERCLASSEX					3 // unable to register window class.
#define EVERYTHING_ERROR_CREATEWINDOW						4 // unable to create listening window
#define EVERYTHING_ERROR_INVALIDINDEX						6 // invalid index
#define EVERYTHING_ERROR_INVALIDCALL						7 // invalid call
#define EVERYTHING_ERROR_INVALIDREQUEST						8 // invalid request data, request data first.

#define EVERYTHING_SORT_NAME_ASCENDING						1

#define EVERYTHING_REQUEST_FILE_NAME						0x00000001
#define EVERYTHING_REQUEST_PATH								0x00000002
#define EVERYTHING_REQUEST_SIZE								0x00000010

#ifndef EVERYTHINGAPI
#define EVERYTHINGAPI //__stdcall
#endif

#ifndef EVERYTHINGUSERAPI
#define EVERYTHINGUSERAPI //__declspec(dllimport)
#endif

// find the everything IPC window
#define EVERYTHING_IPC_WNDCLASSW										L"EVERYTHING_TASKBAR_NOTIFICATION"

// search flags for querys
#define EVERYTHING_IPC_MATCHCASE										0x00000001	// match case
#define EVERYTHING_IPC_MATCHWHOLEWORD									0x00000002	// match whole word
#define EVERYTHING_IPC_MATCHPATH										0x00000004	// include paths in search
#define EVERYTHING_IPC_REGEX											0x00000008	// enable regex

typedef struct EVERYTHING_IPC_COMMAND_LINE
{
	DWORD show_command; // MUST be one of the SW_* ShowWindow() commands

	// null terminated variable sized command line text in UTF-8.
	BYTE command_line_text[1];

}EVERYTHING_IPC_COMMAND_LINE;

// the WM_COPYDATA message for a query.
#define EVERYTHING_IPC_COPYDATAQUERYA									1
#define EVERYTHING_IPC_COPYDATAQUERYW									2

// all results
#define EVERYTHING_IPC_ALLRESULTS										0xFFFFFFFF // all results

#pragma pack (push,1)

typedef struct EVERYTHING_IPC_QUERYW
{
	// the window that will receive the new results.
	// only 32bits are required to store a window handle. (even on x64)
	DWORD reply_hwnd;

	// the value to set the dwData member in the COPYDATASTRUCT struct 
	// sent by Everything when the query is complete.
	DWORD reply_copydata_message;

	// search flags (see EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHWHOLEWORD | EVERYTHING_IPC_MATCHPATH)
	DWORD search_flags;

	// only return results after 'offset' results (0 to return from the first result)
	// useful for scrollable lists
	DWORD offset;

	// the number of results to return 
	// zero to return no results
	// EVERYTHING_IPC_ALLRESULTS to return ALL results
	DWORD max_results;

	// null terminated string. variable lengthed search string buffer.
	WCHAR search_string[1];

}EVERYTHING_IPC_QUERYW;

// ASCII version
typedef struct EVERYTHING_IPC_QUERYA
{
	// the window that will receive the new results.
	// only 32bits are required to store a window handle. (even on x64)
	DWORD reply_hwnd;

	// the value to set the dwData member in the COPYDATASTRUCT struct 
	// sent by Everything when the query is complete.
	DWORD reply_copydata_message;

	// search flags (see EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHWHOLEWORD | EVERYTHING_IPC_MATCHPATH)
	DWORD search_flags;

	// only return results after 'offset' results (0 to return from the first result)
	// useful for scrollable lists
	DWORD offset;

	// the number of results to return 
	// zero to return no results
	// EVERYTHING_IPC_ALLRESULTS to return ALL results
	DWORD max_results;

	// null terminated string. variable lengthed search string buffer.
	CHAR search_string[1];

}EVERYTHING_IPC_QUERYA;

typedef struct EVERYTHING_IPC_ITEMW
{
	// item flags
	DWORD flags;

	// The offset of the filename from the beginning of the list structure.
	// (wchar_t *)((char *)everything_list + everythinglist->name_offset)
	DWORD filename_offset;

	// The offset of the filename from the beginning of the list structure.
	// (wchar_t *)((char *)everything_list + everythinglist->path_offset)
	DWORD path_offset;

}EVERYTHING_IPC_ITEMW;

typedef struct EVERYTHING_IPC_ITEMA
{
	// item flags
	DWORD flags;

	// The offset of the filename from the beginning of the list structure.
	// (char *)((char *)everything_list + everythinglist->name_offset)
	DWORD filename_offset;

	// The offset of the filename from the beginning of the list structure.
	// (char *)((char *)everything_list + everythinglist->path_offset)
	DWORD path_offset;

}EVERYTHING_IPC_ITEMA;

typedef struct EVERYTHING_IPC_LISTW
{
	// the total number of folders found.
	DWORD totfolders;

	// the total number of files found.
	DWORD totfiles;

	// totfolders + totfiles
	DWORD totitems;

	// the number of folders available.
	DWORD numfolders;

	// the number of files available.
	DWORD numfiles;

	// the number of items available.
	DWORD numitems;

	// index offset of the first result in the item list.
	DWORD offset;

	// variable lengthed item list. 
	// use numitems to determine the actual number of items available.
	EVERYTHING_IPC_ITEMW items[1];

}EVERYTHING_IPC_LISTW;

typedef struct EVERYTHING_IPC_LISTA
{
	// the total number of folders found.
	DWORD totfolders;

	// the total number of files found.
	DWORD totfiles;

	// totfolders + totfiles
	DWORD totitems;

	// the number of folders available.
	DWORD numfolders;

	// the number of files available.
	DWORD numfiles;

	// the number of items available.
	DWORD numitems;

	// index offset of the first result in the item list.
	DWORD offset;

	// variable lengthed item list. 
	// use numitems to determine the actual number of items available.
	EVERYTHING_IPC_ITEMA items[1];

}EVERYTHING_IPC_LISTA;

#pragma pack (pop)

#define EVERYTHING_IPC_WNDCLASS			EVERYTHING_IPC_WNDCLASSW

// the WM_COPYDATA message for a query.
// requires Everything 1.4.1
#define EVERYTHING_IPC_COPYDATA_QUERY2A									17
#define EVERYTHING_IPC_COPYDATA_QUERY2W									18

#pragma pack (push,1)

// ASCII version
typedef struct EVERYTHING_IPC_QUERY2
{
	// the window that will receive the new results.
	// only 32bits are required to store a window handle. (even on x64)
	DWORD reply_hwnd;

	// the value to set the dwData member in the COPYDATASTRUCT struct 
	// sent by Everything when the query is complete.
	DWORD reply_copydata_message;

	// search flags (see EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHWHOLEWORD | EVERYTHING_IPC_MATCHPATH)
	DWORD search_flags;

	// only return results after 'offset' results (0 to return from the first result)
	// useful for scrollable lists
	DWORD offset;

	// the number of results to return 
	// zero to return no results
	// EVERYTHING_IPC_ALLRESULTS to return ALL results
	DWORD max_results;

	// request types.
	// one or more of EVERYTHING_IPC_QUERY2_REQUEST_* types.
	DWORD request_flags;

	// sort type, set to one of EVERYTHING_IPC_SORT_* types.
	// set to EVERYTHING_IPC_SORT_NAME_ASCENDING for the best performance (there will never be a performance hit when sorting by name ascending).
	// Other sorts will also be instant if the corresponding fast sort is enabled from Tools -> Options -> Indexes.
	DWORD sort_type;

	// followed by null terminated search.
	// TCHAR search_string[1];

}EVERYTHING_IPC_QUERY2;

typedef struct EVERYTHING_IPC_ITEM2
{
	// item flags one of (EVERYTHING_IPC_FOLDER|EVERYTHING_IPC_DRIVE|EVERYTHING_IPC_ROOT)
	DWORD flags;

	// offset from the start of the EVERYTHING_IPC_LIST2 struct to the data content
	DWORD data_offset;

}EVERYTHING_IPC_ITEM2;

typedef struct EVERYTHING_IPC_LIST2
{
	// number of items found.
	DWORD totitems;

	// the number of items available.
	DWORD numitems;

	// index offset of the first result in the item list.
	DWORD offset;

	// valid request types.
	DWORD request_flags;

	// this sort type.
	// one of EVERYTHING_IPC_SORT_* types.
	// maybe different to requested sort type.
	DWORD sort_type;

	// items follow.
	// EVERYTHING_IPC_ITEM2 items[numitems]

	// item data follows.

}EVERYTHING_IPC_LIST2;

#pragma pack (pop)

#pragma pack (push,1)

typedef struct EVERYTHING_IPC_RUN_HISTORY
{
	DWORD run_count;

	// null terminated ansi/wchar filename follows.
	// TCHAR filename[];

}EVERYTHING_IPC_RUN_HISTORY;

#pragma pack (pop)

// return copydata code
#define _EVERYTHING_COPYDATA_QUERYREPLY		0

#define _EVERYTHING_MSGFLT_ALLOW		1

typedef struct _EVERYTHING_tagCHANGEFILTERSTRUCT
{
	DWORD cbSize;
	DWORD ExtStatus;
}_EVERYTHING_CHANGEFILTERSTRUCT, * _EVERYTHING_PCHANGEFILTERSTRUCT;

static void* _Everything_Alloc(DWORD size);
static void _Everything_Free(void* ptr);
static void _Everything_Lock(void);
static void _Everything_Unlock(void);
static DWORD _Everything_StringLengthA(LPCSTR start);
static DWORD _Everything_StringLengthW(LPCWSTR start);
static BOOL _Everything_ShouldUseVersion2(void);
static BOOL _Everything_SendIPCQuery(void);
static BOOL _Everything_SendIPCQuery2(HWND everything_hwnd);
static void _Everything_FreeLists(void);
static BOOL _Everything_IsValidResultIndex(DWORD dwIndex);
static void* _Everything_GetRequestData(DWORD dwIndex, DWORD dwRequestType);
static void _Everything_ChangeWindowMessageFilter(HWND hwnd);
static BOOL _Everything_GetResultRequestData(DWORD dwIndex, DWORD dwRequestType, void* data, int size);
static DWORD _Everything_GetNumResults(void);

// internal state
static thread_local BOOL _Everything_MatchPath = FALSE;
static thread_local BOOL _Everything_MatchCase = FALSE;
static thread_local BOOL _Everything_MatchWholeWord = FALSE;
static thread_local BOOL _Everything_Regex = FALSE;
static thread_local DWORD _Everything_LastError = FALSE;
static thread_local DWORD _Everything_Max = EVERYTHING_IPC_ALLRESULTS;
static thread_local DWORD _Everything_Offset = 0;
static thread_local DWORD _Everything_Sort = EVERYTHING_SORT_NAME_ASCENDING;
static thread_local DWORD _Everything_RequestFlags = EVERYTHING_REQUEST_PATH | EVERYTHING_REQUEST_FILE_NAME;
static thread_local BOOL _Everything_IsUnicodeQuery = FALSE;
static thread_local DWORD _Everything_QueryVersion = 0;
static thread_local BOOL _Everything_IsUnicodeSearch = FALSE;
static thread_local void* _Everything_Search = NULL; // wchar or char
static thread_local EVERYTHING_IPC_LIST2* _Everything_List2 = NULL;
static thread_local void* _Everything_List = NULL; // EVERYTHING_IPC_LISTW or EVERYTHING_IPC_LISTA
static thread_local HWND _Everything_ReplyWindow = 0;
static thread_local DWORD _Everything_ReplyID = 0;
static thread_local BOOL(WINAPI* _Everything_pChangeWindowMessageFilterEx)(HWND hWnd, UINT message, DWORD action, _EVERYTHING_PCHANGEFILTERSTRUCT pChangeFilterStruct) = 0;
static thread_local HANDLE _Everything_user32_hdll = NULL;
static thread_local BOOL _Everything_GotChangeWindowMessageFilterEx = FALSE;

static void _Everything_Lock(void)
{
	// Unnecessary since everything is thread-local.
}

static void _Everything_Unlock(void)
{
	// Unnecessary since everything is thread-local.
}

// avoid other libs
static DWORD _Everything_StringLengthA(LPCSTR start)
{
	LPCSTR s;

	s = start;

	while (*s)
	{
		s++;
	}

	return (DWORD)(s - start);
}

static DWORD _Everything_StringLengthW(LPCWSTR start)
{
	LPCWSTR s;

	s = start;

	while (*s)
	{
		s++;
	}

	return (DWORD)(s - start);
}

void EVERYTHINGAPI Everything_SetMax(DWORD dwMax)
{
	_Everything_Lock();

	_Everything_Max = dwMax;

	_Everything_Unlock();
}

EVERYTHINGUSERAPI void EVERYTHINGAPI Everything_SetRequestFlags(DWORD dwRequestFlags)
{
	_Everything_Lock();

	_Everything_RequestFlags = dwRequestFlags;

	_Everything_Unlock();
}

// get the search length
static DWORD _Everything_GetSearchLengthW(void)
{
	if (_Everything_Search)
	{
		if (_Everything_IsUnicodeSearch)
		{
			return _Everything_StringLengthW((LPCWSTR)_Everything_Search);
		}
		else
		{
			return MultiByteToWideChar(CP_ACP, 0, (LPCSTR)_Everything_Search, -1, 0, 0);
		}
	}

	return 0;
}

// get the search length
static DWORD _Everything_GetSearchLengthA(void)
{
	if (_Everything_Search)
	{
		if (_Everything_IsUnicodeSearch)
		{
			return WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)_Everything_Search, -1, 0, 0, 0, 0);
		}
		else
		{
			return _Everything_StringLengthA((LPCSTR)_Everything_Search);
		}
	}

	return 0;
}

// get the search
static void _Everything_GetSearchTextW(LPWSTR wbuf)
{
	DWORD wlen;

	if (_Everything_Search)
	{
		wlen = _Everything_GetSearchLengthW();

		if (_Everything_IsUnicodeSearch)
		{
			CopyMemory(wbuf, _Everything_Search, (wlen + 1) * sizeof(WCHAR));

			return;
		}
		else
		{
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)_Everything_Search, -1, wbuf, wlen + 1);

			return;
		}
	}

	*wbuf = 0;
}

// get the search
static void _Everything_GetSearchTextA(LPSTR buf)
{
	DWORD len;

	if (_Everything_Search)
	{
		len = _Everything_GetSearchLengthA();

		if (_Everything_IsUnicodeSearch)
		{
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)_Everything_Search, -1, buf, len + 1, 0, 0);

			return;
		}
		else
		{
			CopyMemory(buf, _Everything_Search, len + 1);

			return;
		}
	}

	*buf = 0;
}

static BOOL _Everything_SendIPCQuery2(HWND everything_hwnd)
{
	BOOL ret;
	DWORD size;
	EVERYTHING_IPC_QUERY2* query;

	// try version 2.

	if (_Everything_IsUnicodeQuery)
	{
		// unicode
		size = sizeof(EVERYTHING_IPC_QUERY2) + ((_Everything_GetSearchLengthW() + 1) * sizeof(WCHAR));
	}
	else
	{
		// ansi
		size = sizeof(EVERYTHING_IPC_QUERY2) + ((_Everything_GetSearchLengthA() + 1) * sizeof(char));
	}

	// alloc
	query = (EVERYTHING_IPC_QUERY2*)_Everything_Alloc(size);

	if (query)
	{
		COPYDATASTRUCT cds;

		query->max_results = _Everything_Max;
		query->offset = _Everything_Offset;
		query->reply_copydata_message = _Everything_ReplyID;
		query->search_flags = (_Everything_Regex ? EVERYTHING_IPC_REGEX : 0) | (_Everything_MatchCase ? EVERYTHING_IPC_MATCHCASE : 0) | (_Everything_MatchWholeWord ? EVERYTHING_IPC_MATCHWHOLEWORD : 0) | (_Everything_MatchPath ? EVERYTHING_IPC_MATCHPATH : 0);
		query->reply_hwnd = (DWORD)(DWORD_PTR)_Everything_ReplyWindow;
		query->sort_type = (DWORD)_Everything_Sort;
		query->request_flags = (DWORD)_Everything_RequestFlags;

		if (_Everything_IsUnicodeQuery)
		{
			_Everything_GetSearchTextW((LPWSTR)(query + 1));
		}
		else
		{
			_Everything_GetSearchTextA((LPSTR)(query + 1));
		}

		cds.cbData = size;
		cds.dwData = _Everything_IsUnicodeQuery ? EVERYTHING_IPC_COPYDATA_QUERY2W : EVERYTHING_IPC_COPYDATA_QUERY2A;
		cds.lpData = query;

		if (SendMessage(everything_hwnd, WM_COPYDATA, (WPARAM)_Everything_ReplyWindow, (LPARAM)&cds))
		{
			// successful.
			ret = TRUE;
		}
		else
		{
			// no ipc
			_Everything_LastError = EVERYTHING_ERROR_IPC;

			ret = FALSE;
		}

		// get result from window.
		_Everything_Free(query);
	}
	else
	{
		_Everything_LastError = EVERYTHING_ERROR_MEMORY;

		ret = FALSE;
	}

	return ret;
}

static BOOL _Everything_ShouldUseVersion2(void)
{
	if (_Everything_RequestFlags != (EVERYTHING_REQUEST_PATH | EVERYTHING_REQUEST_FILE_NAME))
	{
		return TRUE;
	}

	if (_Everything_Sort != EVERYTHING_SORT_NAME_ASCENDING)
	{
		return TRUE;
	}

	// just use version 1
	return FALSE;
}

static BOOL _Everything_SendIPCQuery(void)
{
	HWND everything_hwnd;
	BOOL ret;

	// find the everything ipc window.
	everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS, 0);

	if (!everything_hwnd) {
		everything_hwnd = FindWindow(L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)", 0);
	}

	if (everything_hwnd)
	{
		_Everything_QueryVersion = 2;

		// try version 2 first (if we specified some non-version 1 request flags or sort)
		if ((_Everything_ShouldUseVersion2()) && (_Everything_SendIPCQuery2(everything_hwnd)))
		{
			// successful.
			ret = TRUE;
		}
		else
		{
			DWORD len;
			DWORD size;
			void* query;

			// try version 1.		

			if (_Everything_IsUnicodeQuery)
			{
				// unicode
				len = _Everything_GetSearchLengthW();

				size = sizeof(EVERYTHING_IPC_QUERYW) - sizeof(WCHAR) + len * sizeof(WCHAR) + sizeof(WCHAR);
			}
			else
			{
				// ansi
				len = _Everything_GetSearchLengthA();

				size = sizeof(EVERYTHING_IPC_QUERYA) - sizeof(char) + (len * sizeof(char)) + sizeof(char);
			}

			// alloc
			query = _Everything_Alloc(size);

			if (query)
			{
				COPYDATASTRUCT cds;

				if (_Everything_IsUnicodeQuery)
				{
					((EVERYTHING_IPC_QUERYW*)query)->max_results = _Everything_Max;
					((EVERYTHING_IPC_QUERYW*)query)->offset = _Everything_Offset;
					((EVERYTHING_IPC_QUERYW*)query)->reply_copydata_message = _Everything_ReplyID;
					((EVERYTHING_IPC_QUERYW*)query)->search_flags = (_Everything_Regex ? EVERYTHING_IPC_REGEX : 0) | (_Everything_MatchCase ? EVERYTHING_IPC_MATCHCASE : 0) | (_Everything_MatchWholeWord ? EVERYTHING_IPC_MATCHWHOLEWORD : 0) | (_Everything_MatchPath ? EVERYTHING_IPC_MATCHPATH : 0);
					((EVERYTHING_IPC_QUERYW*)query)->reply_hwnd = (DWORD)(DWORD_PTR)_Everything_ReplyWindow;

					_Everything_GetSearchTextW(((EVERYTHING_IPC_QUERYW*)query)->search_string);
				}
				else
				{
					((EVERYTHING_IPC_QUERYA*)query)->max_results = _Everything_Max;
					((EVERYTHING_IPC_QUERYA*)query)->offset = _Everything_Offset;
					((EVERYTHING_IPC_QUERYA*)query)->reply_copydata_message = _Everything_ReplyID;
					((EVERYTHING_IPC_QUERYA*)query)->search_flags = (_Everything_Regex ? EVERYTHING_IPC_REGEX : 0) | (_Everything_MatchCase ? EVERYTHING_IPC_MATCHCASE : 0) | (_Everything_MatchWholeWord ? EVERYTHING_IPC_MATCHWHOLEWORD : 0) | (_Everything_MatchPath ? EVERYTHING_IPC_MATCHPATH : 0);
					((EVERYTHING_IPC_QUERYA*)query)->reply_hwnd = (DWORD)(DWORD_PTR)_Everything_ReplyWindow;

					_Everything_GetSearchTextA(((EVERYTHING_IPC_QUERYA*)query)->search_string);
				}

				cds.cbData = size;
				cds.dwData = _Everything_IsUnicodeQuery ? EVERYTHING_IPC_COPYDATAQUERYW : EVERYTHING_IPC_COPYDATAQUERYA;
				cds.lpData = query;

				_Everything_QueryVersion = 1;

				if (SendMessage(everything_hwnd, WM_COPYDATA, (WPARAM)_Everything_ReplyWindow, (LPARAM)&cds))
				{
					// successful.
					ret = TRUE;
				}
				else
				{
					// no ipc
					_Everything_LastError = EVERYTHING_ERROR_IPC;

					ret = FALSE;
				}

				// get result from window.
				_Everything_Free(query);
			}
			else
			{
				_Everything_LastError = EVERYTHING_ERROR_MEMORY;

				ret = FALSE;
			}
		}
	}
	else
	{
		_Everything_LastError = EVERYTHING_ERROR_IPC;

		ret = FALSE;
	}

	return ret;
}

static DWORD _Everything_GetNumResults(void)
{
	DWORD ret;

	if (_Everything_List)
	{
		if (_Everything_IsUnicodeQuery)
		{
			ret = ((EVERYTHING_IPC_LISTW*)_Everything_List)->numitems;
		}
		else
		{
			ret = ((EVERYTHING_IPC_LISTA*)_Everything_List)->numitems;
		}
	}
	else
		if (_Everything_List2)
		{
			ret = _Everything_List2->numitems;
		}
		else
		{
			_Everything_LastError = EVERYTHING_ERROR_INVALIDCALL;

			ret = 0;
		}

	return ret;
}

static void* _Everything_Alloc(DWORD size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

static void _Everything_Free(void* ptr)
{
	HeapFree(GetProcessHeap(), 0, ptr);
}

static void _Everything_FreeLists(void)
{
	if (_Everything_List)
	{
		_Everything_Free(_Everything_List);

		_Everything_List = 0;
	}

	if (_Everything_List2)
	{
		_Everything_Free(_Everything_List2);

		_Everything_List2 = 0;
	}
}

static BOOL _Everything_IsValidResultIndex(DWORD dwIndex)
{
	if (dwIndex < 0)
	{
		return FALSE;
	}

	if (dwIndex >= _Everything_GetNumResults())
	{
		return FALSE;
	}

	return TRUE;
}

// assumes _Everything_List2 and dwIndex are valid.
static void* _Everything_GetRequestData(DWORD dwIndex, DWORD dwRequestType)
{
	char* p;
	EVERYTHING_IPC_ITEM2* items;

	items = (EVERYTHING_IPC_ITEM2*)(_Everything_List2 + 1);

	p = ((char*)_Everything_List2) + items[dwIndex].data_offset;

	if (_Everything_List2->request_flags & EVERYTHING_REQUEST_SIZE)
	{
		if (dwRequestType == EVERYTHING_REQUEST_SIZE)
		{
			return p;
		}

		p += sizeof(LARGE_INTEGER);
	}

	return NULL;
}

static void _Everything_ChangeWindowMessageFilter(HWND hwnd)
{
	if (!_Everything_GotChangeWindowMessageFilterEx)
	{
		// allow the everything window to send a reply.
		_Everything_user32_hdll = LoadLibraryW(L"user32.dll");

		if (_Everything_user32_hdll)
		{
			_Everything_pChangeWindowMessageFilterEx = (BOOL(WINAPI*)(HWND hWnd, UINT message, DWORD action, _EVERYTHING_PCHANGEFILTERSTRUCT pChangeFilterStruct))GetProcAddress((HMODULE)_Everything_user32_hdll, "ChangeWindowMessageFilterEx");
		}

		_Everything_GotChangeWindowMessageFilterEx = 1;
	}

	if (_Everything_GotChangeWindowMessageFilterEx)
	{
		if (_Everything_pChangeWindowMessageFilterEx)
		{
			_Everything_pChangeWindowMessageFilterEx(hwnd, WM_COPYDATA, _EVERYTHING_MSGFLT_ALLOW, 0);
		}
	}
}

static BOOL _Everything_GetResultRequestData(DWORD dwIndex, DWORD dwRequestType, void* data, int size)
{
	BOOL ret;

	_Everything_Lock();

	if (_Everything_List2)
	{
		if (_Everything_IsValidResultIndex(dwIndex))
		{
			void* request_data;

			request_data = _Everything_GetRequestData(dwIndex, dwRequestType);
			if (request_data)
			{
				CopyMemory(data, request_data, size);

				ret = TRUE;
			}
			else
			{
				_Everything_LastError = EVERYTHING_ERROR_INVALIDREQUEST;

				ret = FALSE;
			}
		}
		else
		{
			_Everything_LastError = EVERYTHING_ERROR_INVALIDINDEX;

			ret = FALSE;
		}
	}
	else
	{
		_Everything_LastError = EVERYTHING_ERROR_INVALIDCALL;

		ret = FALSE;
	}

	_Everything_Unlock();

	return ret;
}

BOOL EVERYTHINGAPI Everything_GetResultSize(DWORD dwIndex, LARGE_INTEGER* lpSize)
{
	return _Everything_GetResultRequestData(dwIndex, EVERYTHING_REQUEST_SIZE, lpSize, sizeof(LARGE_INTEGER));
}

// clang-format on
#pragma endregion  // everything_sdk

constexpr WCHAR kEverything4Wh_className[] = "EVERYTHING_WINDHAWK_" WH_MOD_ID;

LPCITEMIDLIST PIDLNext(LPCITEMIDLIST pidl) {
    return reinterpret_cast<LPCITEMIDLIST>(reinterpret_cast<const BYTE*>(pidl) +
                                           pidl->mkid.cb);
}

size_t PIDLSize(LPCITEMIDLIST pidl) {
    size_t s = 0;
    while (pidl->mkid.cb > 0) {
        s += pidl->mkid.cb;
        pidl = PIDLNext(pidl);
    }
    // We add 2 because an LPITEMIDLIST is terminated by two NULL bytes.
    return 2 + s;
}

std::vector<BYTE> PIDLToVector(const ITEMIDLIST* pidl) {
    if (!pidl) {
        return {};
    }

    const BYTE* ptr = reinterpret_cast<const BYTE*>(pidl);
    size_t size = PIDLSize(pidl);
    return std::vector<BYTE>(ptr, ptr + size);
}

thread_local winrt::com_ptr<IShellFolder2> g_everything4Wh_cacheShellFolder;
thread_local std::map<std::vector<BYTE>, std::optional<ULONGLONG>>
    g_everything4Wh_cache;
thread_local DWORD g_everything4Wh_cacheUsedTickCount;

// custom window proc
LRESULT WINAPI Everything4Wh_window_proc(HWND hwnd,
                                         UINT msg,
                                         WPARAM wParam,
                                         LPARAM lParam) {
    switch (msg) {
        case WM_COPYDATA: {
            COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lParam;

            switch (cds->dwData) {
                case _EVERYTHING_COPYDATA_QUERYREPLY:

                    if (_Everything_QueryVersion == 2) {
                        _Everything_FreeLists();

                        _Everything_List2 =
                            (EVERYTHING_IPC_LIST2*)_Everything_Alloc(
                                cds->cbData);

                        if (_Everything_List2) {
                            CopyMemory(_Everything_List2, cds->lpData,
                                       cds->cbData);
                        } else {
                            _Everything_LastError = EVERYTHING_ERROR_MEMORY;
                        }

                        PostMessage(hwnd, WM_APP, 0, 0);

                        return TRUE;
                    } else if (_Everything_QueryVersion == 1) {
                        _Everything_FreeLists();

                        _Everything_List = _Everything_Alloc(cds->cbData);

                        if (_Everything_List) {
                            CopyMemory(_Everything_List, cds->lpData,
                                       cds->cbData);
                        } else {
                            _Everything_LastError = EVERYTHING_ERROR_MEMORY;
                        }

                        PostMessage(hwnd, WM_APP, 0, 0);

                        return TRUE;
                    }

                    break;
            }

            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Everything4Wh_InitForQuery() {
    if (_Everything_ReplyWindow) {
        return true;
    }

    WNDCLASSEX wcex;

    ZeroMemory(&wcex, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);

    if (!GetClassInfoEx(GetModuleHandle(0), kEverything4Wh_className, &wcex)) {
        ZeroMemory(&wcex, sizeof(WNDCLASSEX));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.hInstance = GetModuleHandle(0);
        wcex.lpfnWndProc = Everything4Wh_window_proc;
        wcex.lpszClassName = kEverything4Wh_className;

        if (!RegisterClassEx(&wcex)) {
            _Everything_LastError = EVERYTHING_ERROR_REGISTERCLASSEX;
            return false;
        }
    }

    HWND hwnd = CreateWindow(kEverything4Wh_className, L"", 0, 0, 0, 0, 0, 0, 0,
                             GetModuleHandle(0), 0);
    if (!hwnd) {
        _Everything_LastError = EVERYTHING_ERROR_CREATEWINDOW;
        return false;
    }

    _Everything_ChangeWindowMessageFilter(hwnd);

    _Everything_ReplyWindow = hwnd;
    _Everything_ReplyID = _EVERYTHING_COPYDATA_QUERYREPLY;
    return true;
}

bool Everything4Wh_QueryAndWait() {
    HWND hwnd = _Everything_ReplyWindow;
    MSG msg;
    int ret;
    bool succeeded = false;

    _Everything_IsUnicodeQuery = TRUE;

    if (_Everything_SendIPCQuery()) {
        // message pump
    loop:

        WaitMessage();

        // update windows
        while (PeekMessage(&msg, hwnd, 0, 0, 0)) {
            ret = (DWORD)GetMessage(&msg, hwnd, 0, 0);
            if (ret == -1 || !ret) {
                goto exit;
            }
            if (msg.message == WM_APP) {
                succeeded = true;
                goto exit;
            }

            // let windows handle it.
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        goto loop;
    }

exit:

    // get result from window.
    DestroyWindow(hwnd);
    _Everything_ReplyWindow = nullptr;

    return succeeded;
}

void Everything4Wh_Cleanup() {
    UnregisterClass(kEverything4Wh_className, GetModuleHandle(0));
}

thread_local WCHAR
    g_Everything4Wh_SearchBuffer[sizeof("path:wholefilename:\"\"") - 1 +
                                 MAX_PATH];

bool Everything4Wh_GetFileSize(WCHAR path[MAX_PATH], LARGE_INTEGER* size) {
    if (!Everything4Wh_InitForQuery()) {
        return false;
    }

    swprintf_s(g_Everything4Wh_SearchBuffer, L"path:wholefilename:\"%s\"",
               path);

    _Everything_Search = g_Everything4Wh_SearchBuffer;
    _Everything_IsUnicodeSearch = 1;
    Everything_SetRequestFlags(EVERYTHING_REQUEST_SIZE);
    Everything_SetMax(1);

    if (!Everything4Wh_QueryAndWait()) {
        return false;
    }

    bool succeeded = Everything_GetResultSize(0, size);
    _Everything_FreeLists();
    return succeeded;
}

constexpr GUID KStorage = {0xB725F130,
                           0x47EF,
                           0x101A,
                           {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}};
constexpr PROPERTYKEY kPKEY_Size = {KStorage, 12};

class SizeCalculator : public INamespaceWalkCB2 {
   public:
    SizeCalculator() : m_totalSize(0) {}
    virtual ~SizeCalculator() {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppvObject) override {
        winrt::guid riidguid{riid};
        if (riidguid == winrt::guid_of<IUnknown>() ||
            riidguid == winrt::guid_of<INamespaceWalkCB>() ||
            riidguid == winrt::guid_of<INamespaceWalkCB2>()) {
            *ppvObject = static_cast<INamespaceWalkCB2*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // INamespaceWalkCB methods
    HRESULT STDMETHODCALLTYPE FoundItem(IShellFolder* psf,
                                        LPCITEMIDLIST pidl) override {
        winrt::com_ptr<IShellFolder2> psf2;
        HRESULT hr = psf->QueryInterface(IID_PPV_ARGS(psf2.put()));
        if (FAILED(hr)) {
            Wh_Log(L"Failed: %08X", hr);
            return hr;
        }

        VARIANT varSize;
        hr = psf2->GetDetailsEx(pidl, &kPKEY_Size, &varSize);
        if (SUCCEEDED(hr) && varSize.vt == VT_UI8) {
            m_totalSize += varSize.ullVal;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnterFolder(IShellFolder* /*psf*/,
                                          LPCITEMIDLIST /*pidl*/) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE LeaveFolder(IShellFolder* /*psf*/,
                                          LPCITEMIDLIST /*pidl*/) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE WalkComplete(HRESULT hr) override { return S_OK; }

    HRESULT STDMETHODCALLTYPE
    InitializeProgressDialog(LPWSTR* /*ppszTitle*/,
                             LPWSTR* /*ppszCancel*/) override {
        return E_NOTIMPL;
    }

    // Get the total size.
    ULONGLONG GetTotalSize() const { return m_totalSize; }

   private:
    ULONG m_refCount = 1;
    ULONGLONG m_totalSize;
};

std::optional<ULONGLONG> CalculateFolderSize(IShellFolder2* shellFolder) {
    // Create the namespace walker.
    winrt::com_ptr<INamespaceWalk> namespaceWalk;
    HRESULT hr = CoCreateInstance(CLSID_NamespaceWalker, nullptr, CLSCTX_INPROC,
                                  IID_PPV_ARGS(namespaceWalk.put()));
    if (FAILED(hr)) {
        Wh_Log(L"Failed: %08X", hr);
        return std::nullopt;
    }

    // Create the callback object.
    SizeCalculator* callback = new SizeCalculator();

    // Enumerate child items and sum sizes in the callback.
    hr = namespaceWalk->Walk(
        shellFolder, NSWF_DONT_ACCUMULATE_RESULT | NSWF_DONT_TRAVERSE_LINKS,
        255, callback);
    if (FAILED(hr)) {
        Wh_Log(L"Failed: %08X", hr);
        callback->Release();
        return std::nullopt;
    }

    ULONGLONG totalSize = callback->GetTotalSize();
    callback->Release();

    return totalSize;
}

bool GetFolderPathFromIShellFolder(IShellFolder2* shellFolder,
                                   WCHAR path[MAX_PATH]) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (SUCCEEDED(hr)) {
        bool succeeded = SHGetPathFromIDList(pidl, path);
        CoTaskMemFree(pidl);
        return succeeded;
    }
    return false;
}

using CFSFolder__GetSize_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                              const ITEMID_CHILD* itemidChild,
                                              const void* idFolder,
                                              PROPVARIANT* propVariant);
CFSFolder__GetSize_t CFSFolder__GetSize_Original;
HRESULT WINAPI CFSFolder__GetSize_Hook(void* pCFSFolder,
                                       const ITEMID_CHILD* itemidChild,
                                       const void* idFolder,
                                       PROPVARIANT* propVariant) {
    auto hookScope = hookRefCountScope();

    HRESULT ret = CFSFolder__GetSize_Original(pCFSFolder, itemidChild, idFolder,
                                              propVariant);
    if (ret != S_OK || propVariant->vt != VT_EMPTY) {
        return ret;
    }

    switch (g_settings.calculateFolderSizes) {
        case CalculateFolderSizes::disabled:
            return ret;

        case CalculateFolderSizes::withShiftKey:
            if (GetAsyncKeyState(VK_SHIFT) >= 0) {
                return ret;
            }
            break;

        case CalculateFolderSizes::everything:
        case CalculateFolderSizes::always:
            break;
    }

    Wh_Log(L">");

    winrt::com_ptr<IShellFolder2> shellFolder2;
    HRESULT hr =
        ((IUnknown*)pCFSFolder)
            ->QueryInterface(IID_IShellFolder2, shellFolder2.put_void());
    if (FAILED(hr) || !shellFolder2) {
        Wh_Log(L"Failed: %08X", hr);
        return S_OK;
    }

    if (shellFolder2 != g_everything4Wh_cacheShellFolder ||
        GetTickCount() - g_everything4Wh_cacheUsedTickCount > 1000) {
        g_everything4Wh_cache.clear();
    }

    g_everything4Wh_cacheShellFolder = shellFolder2;

    auto [cacheIt, cacheMissing] = g_everything4Wh_cache.try_emplace(
        PIDLToVector(itemidChild), std::nullopt);

    if (cacheMissing) {
        winrt::com_ptr<IShellFolder2> childFolder;
        hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                        IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr) || !childFolder) {
            Wh_Log(L"Failed: %08X", hr);
        } else if (g_settings.calculateFolderSizes ==
                   CalculateFolderSizes::everything) {
            WCHAR path[MAX_PATH];
            if (GetFolderPathFromIShellFolder(childFolder.get(), path)) {
                LARGE_INTEGER size;
                if (Everything4Wh_GetFileSize(path, &size)) {
                    cacheIt->second = size.QuadPart;
                } else {
                    Wh_Log(L"Failed to get size of %s", path);
                }
            } else {
                Wh_Log(L"Failed to get path");
            }
        } else {
            cacheIt->second = CalculateFolderSize(childFolder.get());
        }
    } else {
        Wh_Log(L"Using cached size");
    }

    g_everything4Wh_cacheUsedTickCount = GetTickCount();

    std::optional<ULONGLONG> folderSize = cacheIt->second;
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

using PSFormatForDisplayAlloc_t = decltype(&PSFormatForDisplayAlloc);
PSFormatForDisplayAlloc_t PSFormatForDisplayAlloc_Original;
HRESULT WINAPI PSFormatForDisplayAlloc_Hook(const PROPERTYKEY& key,
                                            const PROPVARIANT& propvar,
                                            PROPDESC_FORMAT_FLAGS pdff,
                                            PWSTR* ppszDisplay) {
    auto original = [=]() {
        return PSFormatForDisplayAlloc_Original(key, propvar, pdff,
                                                ppszDisplay);
    };

    PROPDESC_FORMAT_FLAGS pdffNew = pdff & ~PDFF_ALWAYSKB;
    if (pdffNew == pdff) {
        return original();
    }

    void* retAddress = __builtin_return_address(0);

    HMODULE explorerFrame = GetModuleHandle(L"explorerframe.dll");
    if (!explorerFrame) {
        return original();
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != explorerFrame) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplayAlloc_Original(key, propvar, pdffNew, ppszDisplay);
}

using LoadStringW_t = decltype(&LoadStringW);
LoadStringW_t LoadStringW_Original;
int WINAPI LoadStringW_Hook(HINSTANCE hInstance,
                            UINT uID,
                            LPWSTR lpBuffer,
                            int cchBufferMax) {
    int ret = LoadStringW_Original(hInstance, uID, lpBuffer, cchBufferMax);
    if (!ret || hInstance != g_propsysModule || cchBufferMax == 0) {
        return ret;
    }

    PCWSTR newStr = nullptr;
    if (wcscmp(lpBuffer, L"%s KB") == 0) {
        newStr = L"%s KiB";
    } else if (wcscmp(lpBuffer, L"%s MB") == 0) {
        newStr = L"%s MiB";
    } else if (wcscmp(lpBuffer, L"%s GB") == 0) {
        newStr = L"%s GiB";
    } else if (wcscmp(lpBuffer, L"%s TB") == 0) {
        newStr = L"%s TiB";
    } else if (wcscmp(lpBuffer, L"%s PB") == 0) {
        newStr = L"%s PiB";
    } else if (wcscmp(lpBuffer, L"%s EB") == 0) {
        newStr = L"%s EiB";
    }

    if (!newStr) {
        return ret;
    }

    Wh_Log(L"> Overriding string %u: %s -> %s", uID, lpBuffer, newStr);
    wcsncpy_s(lpBuffer, cchBufferMax, newStr, cchBufferMax - 1);
    return wcslen(lpBuffer);
}

bool HookWindowsStorageSymbols() {
    HMODULE windowsStorageModule = LoadLibrary(L"windows.storage.dll");
    if (!windowsStorageModule) {
        Wh_Log(L"Failed to load windows.storage.dll");
        return false;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {
#ifdef _WIN64
                LR"(protected: static long __cdecl CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct tagPROPVARIANT *))",
#else
                LR"(protected: static long __stdcall CFSFolder::_GetSize(class CFSFolder *,struct _ITEMID_CHILD const *,struct IDFOLDER const *,struct tagPROPVARIANT *))",
#endif
            },
            &CFSFolder__GetSize_Original,
            CFSFolder__GetSize_Hook,
        },
    };

    return HookSymbols(windowsStorageModule, windowsStorageHooks,
                       ARRAYSIZE(windowsStorageHooks));
}

void LoadSettings() {
    PCWSTR calculateFolderSizes = Wh_GetStringSetting(L"calculateFolderSizes");
    g_settings.calculateFolderSizes = CalculateFolderSizes::disabled;
    if (wcscmp(calculateFolderSizes, L"everything") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::everything;
    } else if (wcscmp(calculateFolderSizes, L"withShiftKey") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::withShiftKey;
    } else if (wcscmp(calculateFolderSizes, L"always") == 0) {
        g_settings.calculateFolderSizes = CalculateFolderSizes::always;
    }
    Wh_FreeStringSetting(calculateFolderSizes);

    g_settings.disableKbOnlySizes = Wh_GetIntSetting(L"disableKbOnlySizes");
    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    if (g_settings.disableKbOnlySizes) {
        WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplayAlloc,
                                           PSFormatForDisplayAlloc_Hook,
                                           &PSFormatForDisplayAlloc_Original);
    }

    if (g_settings.useIecTerms) {
        g_propsysModule = GetModuleHandle(L"propsys.dll");
        if (!g_propsysModule) {
            Wh_Log(L"Failed getting propsys.dll");
            return FALSE;
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

        auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                         PCSTR targetName, void* hookFunction,
                                         void** originalFunction) {
            void* targetFunction =
                (void*)GetProcAddress(kernelBaseModule, targetName);
            if (!targetFunction) {
                targetFunction =
                    (void*)GetProcAddress(kernel32Module, targetName);
                if (!targetFunction) {
                    return FALSE;
                }
            }

            return Wh_SetFunctionHook(targetFunction, hookFunction,
                                      originalFunction);
        };

        setKernelFunctionHook("LoadStringW", (void*)LoadStringW_Hook,
                              (void**)&LoadStringW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::everything) {
        Everything4Wh_Cleanup();
    }

    while (g_hookRefCount > 0) {
        Sleep(200);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
