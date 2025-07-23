// ==WindhawkMod==
// @id              explorer-details-better-file-sizes
// @name            Better file sizes in Explorer details
// @description     Optional improvements: show folder sizes, use MB/GB for large files (by default, all sizes are shown in KBs), use IEC terms (such as KiB instead of KB)
// @version         1.4.11
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @exclude         conhost.exe
// @exclude         Plex*.exe
// @exclude         backgroundTaskHost.exe
// @exclude         LockApp.exe
// @exclude         SearchHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
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

#### Notes

* "Everything" must be running for the integration to work.
* Both "Everything" 1.4 and [1.5
  Alpha](https://www.voidtools.com/forum/viewtopic.php?t=9787) are supported.
  With version 1.5.0.1384a or newer, the mod uses the new [Everything
  SDK3](https://www.voidtools.com/forum/viewtopic.php?t=15853), which results in
  a much faster folder size query (can be around 20x faster).

### Calculated manually

If you prefer to avoid installing "Everything", you can enable folder sizes and
have them calculated manually. Since calculating folder sizes can be slow, it's
not enabled by default, and there's an option to enable it only while holding
the Shift key.

## Mix files and folders when sorting by size

When sorting by size, files end up in one separate chunk, and folders in
another. That's the default Explorer behavior, which also applies when sorting
by other columns. This option changes sorting by size to disable this
separation.

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
- sortSizesMixFolders: true
  $name: Mix files and folders when sorting by size
  $description: >-
    By default, folders are kept separately from files when sorting
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
#include <mutex>
#include <optional>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>

#include <comutil.h>
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
    bool sortSizesMixFolders;
    bool disableKbOnlySizes;
    bool useIecTerms;
} g_settings;

bool g_isEverything;
HMODULE g_propsysModule;
std::atomic<int> g_hookRefCount;

thread_local bool g_inCRecursiveFolderOperation_Prepare;
thread_local bool g_inCRecursiveFolderOperation_Do;

auto hookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// The partial version of the Everything SDK below and some of the code for
// querying Everything are based on code from SizeES: A Plugin for Fast,
// Persistent FolderSizes in x2 via Everything Search.
// https://forum.zabkat.com/viewtopic.php?t=12326

#pragma region everything_sdk
// clang-format off

//
// Copyright (C) 2024 David Carpenter / voidtools
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

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

#ifndef EVERYTHING3_BYTE
#define EVERYTHING3_BYTE		BYTE
#endif

#ifndef EVERYTHING3_CHAR
#define EVERYTHING3_CHAR		CHAR
#endif

#ifndef EVERYTHING3_WORD
#define EVERYTHING3_WORD		WORD
#endif

#ifndef EVERYTHING3_DWORD
#define EVERYTHING3_DWORD		DWORD
#endif

#ifndef EVERYTHING3_INT32
#define EVERYTHING3_INT32		__int32
#endif

#ifndef EVERYTHING3_UINT64
#define EVERYTHING3_UINT64		unsigned __int64
#endif

#ifndef EVERYTHING3_SIZE_T
#define EVERYTHING3_SIZE_T		SIZE_T
#endif

#ifndef EVERYTHING3_BOOL
#define EVERYTHING3_BOOL		BOOL
#endif

#ifndef EVERYTHING3_WCHAR
#define EVERYTHING3_WCHAR		WCHAR
#endif

#define EVERYTHING3_DWORD_MAX							0xffffffff
#define EVERYTHING3_UINT64_MAX							0xffffffffffffffffULL

#define EVERYTHING3_OK									0
#define EVERYTHING3_ERROR_OUT_OF_MEMORY					0xE0000001
#define EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND			0xE0000002
#define EVERYTHING3_ERROR_DISCONNECTED					0xE0000003
#define EVERYTHING3_ERROR_INVALID_PARAMETER				0xE0000004
#define EVERYTHING3_ERROR_BAD_REQUEST					0xE0000005
#define EVERYTHING3_ERROR_CANCELLED						0xE0000006
#define EVERYTHING3_ERROR_PROPERTY_NOT_FOUND			0xE0000007
#define EVERYTHING3_ERROR_SERVER						0xE0000008
#define EVERYTHING3_ERROR_INVALID_COMMAND				0xE0000009
#define EVERYTHING3_ERROR_BAD_RESPONSE					0xE000000A
#define EVERYTHING3_ERROR_INSUFFICIENT_BUFFER			0xE000000B
#define EVERYTHING3_ERROR_SHUTDOWN						0xE000000C

typedef EVERYTHING3_BYTE EVERYTHING3_UTF8;
typedef struct _everything3_client_s EVERYTHING3_CLIENT;

#ifndef EVERYTHING3_API
#define EVERYTHING3_API //__stdcall
#endif

EVERYTHING3_CLIENT *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_ShutdownClient(EVERYTHING3_CLIENT *client);
EVERYTHING3_BOOL EVERYTHING3_API Everything3_DestroyClient(EVERYTHING3_CLIENT *client);

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, const EVERYTHING3_WCHAR *lpFilename);

//
// Copyright (C) 2024 David Carpenter / voidtools
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

// This is an edited version (required code only) of the file in the original Everything 1.5 SDK
// https://www.voidtools.com/forum/viewtopic.php?t=15853

// #define WIN32_LEAN_AND_MEAN

// #include <windows.h>

// #include "Everything3.h"

#define _EVERYTHING3_WCHAR_BUF_STACK_SIZE					MAX_PATH
#define _EVERYTHING3_UTF8_BUF_STACK_SIZE					MAX_PATH

// IPC pipe commands.
#define _EVERYTHING3_COMMAND_GET_FOLDER_SIZE				18

// IPC pipe responses
#define _EVERYTHING3_RESPONSE_OK_MORE_DATA					100
#define _EVERYTHING3_RESPONSE_OK							200
#define _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST				400
#define _EVERYTHING3_RESPONSE_ERROR_CANCELLED				401
#define _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND				404
#define _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY			500
#define _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND			501

typedef struct _everything3_wchar_buf_s {
	EVERYTHING3_WCHAR *buf;
	SIZE_T length_in_wchars;
	SIZE_T size_in_wchars;
	EVERYTHING3_WCHAR stack_buf[_EVERYTHING3_WCHAR_BUF_STACK_SIZE];
} _everything3_wchar_buf_t;

typedef struct _everything3_utf8_buf_s {
	EVERYTHING3_UTF8 *buf;
	SIZE_T length_in_bytes;
	SIZE_T size_in_bytes;
	EVERYTHING3_UTF8 stack_buf[_EVERYTHING3_UTF8_BUF_STACK_SIZE];
} _everything3_utf8_buf_t;

typedef struct _everything3_message_s {
	DWORD code;
	DWORD size;
} _everything3_message_t;

typedef struct _everything3_client_s {
	CRITICAL_SECTION cs;
	HANDLE pipe_handle;
	HANDLE send_event;
	HANDLE recv_event;
	HANDLE shutdown_event;
} _everything3_client_t;

static void _everything3_Lock(_everything3_client_t *client);
static void _everything3_Unlock(_everything3_client_t *client);
static void *_everything3_mem_alloc(SIZE_T size);
static void *_everything3_mem_calloc(SIZE_T size);
static void _everything3_mem_free(void *ptr);
static SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b);
static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s);
static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf);
static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf);
static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars);
static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name);
static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size);
static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header);
static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size);
static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size);
static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread);
static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *wcbuf);
static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *wcbuf);
static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *wcbuf, SIZE_T size_in_bytes);
static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *wcbuf, SIZE_T length_in_bytes);
static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws);
static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws);

static EVERYTHING3_WCHAR *_everything3_wchar_string_cat_wchar_string_no_null_terminate(EVERYTHING3_WCHAR *buf, EVERYTHING3_WCHAR *current_d, const EVERYTHING3_WCHAR *s) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_WCHAR *d;

	p = s;
	d = current_d;

	while (*p) {
		if (buf) {
			*d++ = *p;
		} else {
			d = (WCHAR *)_everything3_safe_size_add((SIZE_T)d, 1);
		}

		p++;
	}

	return d;
}

static EVERYTHING3_WCHAR *_Everything3_get_pipe_name(EVERYTHING3_WCHAR *buf, const EVERYTHING3_WCHAR *instance_name) {
	EVERYTHING3_WCHAR *d;

	d = buf;

	d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L"\\\\.\\PIPE\\Everything IPC");

	if ((instance_name) && (*instance_name)) {
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L" (");
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, instance_name);
		d = _everything3_wchar_string_cat_wchar_string_no_null_terminate(buf, d, L")");
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_wchar_buf_get_pipe_name(_everything3_wchar_buf_t *wcbuf, const EVERYTHING3_WCHAR *instance_name) {
	if (_everything3_wchar_buf_grow_length(wcbuf, (SIZE_T)_Everything3_get_pipe_name(NULL, instance_name))) {
		_Everything3_get_pipe_name(wcbuf->buf, instance_name);

		return TRUE;
	}

	return FALSE;
}

// connects to the named pipe "\\\\.\\PIPE\\Everything IPC"
// instance_name can BE NULL.
// a NULL instance_name or an empty instance_name will connect to the unnamed instance.
// The Everything 1.5a release will use an "1.5a" instance.
_everything3_client_t *EVERYTHING3_API Everything3_ConnectW(const EVERYTHING3_WCHAR *instance_name) {
	_everything3_client_t *ret;
	_everything3_wchar_buf_t pipe_name_wcbuf;

	ret = NULL;
	_everything3_wchar_buf_init(&pipe_name_wcbuf);

	if (_everything3_wchar_buf_get_pipe_name(&pipe_name_wcbuf, instance_name)) {
		HANDLE pipe_handle;

		pipe_handle = CreateFileW(pipe_name_wcbuf.buf, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (pipe_handle != INVALID_HANDLE_VALUE) {
			_everything3_client_t *client;

			client = (_everything3_client_t *)_everything3_mem_calloc(sizeof(_everything3_client_t));

			if (client) {
				InitializeCriticalSection(&client->cs);

				client->pipe_handle = pipe_handle;

				pipe_handle = INVALID_HANDLE_VALUE;

				client->shutdown_event = CreateEvent(0, TRUE, FALSE, 0);
				if (client->shutdown_event) {
					client->send_event = CreateEvent(0, TRUE, FALSE, 0);
					if (client->send_event) {
						client->recv_event = CreateEvent(0, TRUE, FALSE, 0);
						if (client->recv_event) {
							ret = client;
							client = NULL;
						} else {
							SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
				}

				if (client) {
					Everything3_DestroyClient(client);
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
			}

			if (pipe_handle != INVALID_HANDLE_VALUE) {
				CloseHandle(pipe_handle);
			}

		} else {
			SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
		}
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	_everything3_wchar_buf_kill(&pipe_name_wcbuf);

	return ret;
}

BOOL EVERYTHING3_API Everything3_ShutdownClient(_everything3_client_t *client) {
	if (client) {
		SetEvent(client->shutdown_event);

		return TRUE;
	}

	SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

	return FALSE;
}

BOOL EVERYTHING3_API Everything3_DestroyClient(_everything3_client_t *client) {
	if (client) {
		Everything3_ShutdownClient(client);

		if (client->recv_event) {
			CloseHandle(client->recv_event);
		}

		if (client->send_event) {
			CloseHandle(client->send_event);
		}

		if (client->shutdown_event) {
			CloseHandle(client->shutdown_event);
		}

		if (client->pipe_handle != INVALID_HANDLE_VALUE) {
			CloseHandle(client->pipe_handle);

			client->pipe_handle = INVALID_HANDLE_VALUE;
		}

		DeleteCriticalSection(&client->cs);

		_everything3_mem_free(client);

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);

		return FALSE;
	}
}

static void *_everything3_mem_alloc(SIZE_T size) {
	void *p;

	if (size == SIZE_MAX) {
		return NULL;
	}

	p = HeapAlloc(GetProcessHeap(), 0, size);

	if (!p) {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);
	}

	return p;
}

static void *_everything3_mem_calloc(SIZE_T size) {
	void *p;

	p = _everything3_mem_alloc(size);
	if (p) {
		ZeroMemory(p, size);
	}

	return p;
}

static void _everything3_mem_free(void *ptr) {
	HeapFree(GetProcessHeap(), 0, ptr);
}

SIZE_T _everything3_safe_size_add(SIZE_T a, SIZE_T b) {
	SIZE_T c;

	c = a + b;

	if (c < a) {
		return SIZE_MAX;
	}

	return c;
}

static void _everything3_Lock(_everything3_client_t *client) {
	EnterCriticalSection(&client->cs);
}

static void _everything3_Unlock(_everything3_client_t *client) {
	LeaveCriticalSection(&client->cs);
}

static void _everything3_wchar_buf_init(_everything3_wchar_buf_t *wcbuf) {
	wcbuf->buf = wcbuf->stack_buf;
	wcbuf->length_in_wchars = 0;
	wcbuf->size_in_wchars = _EVERYTHING3_WCHAR_BUF_STACK_SIZE;
	wcbuf->buf[0] = 0;
}

static void _everything3_wchar_buf_kill(_everything3_wchar_buf_t *wcbuf) {
	if (wcbuf->buf != wcbuf->stack_buf) {
		_everything3_mem_free(wcbuf->buf);
	}
}

static void _everything3_wchar_buf_empty(_everything3_wchar_buf_t *wcbuf) {
	_everything3_wchar_buf_kill(wcbuf);
	_everything3_wchar_buf_init(wcbuf);
}

static BOOL _everything3_wchar_buf_grow_size(_everything3_wchar_buf_t *wcbuf, SIZE_T size_in_wchars) {
	EVERYTHING3_WCHAR *new_buf;

	if (size_in_wchars <= wcbuf->size_in_wchars) {
		return TRUE;
	}

	_everything3_wchar_buf_empty(wcbuf);

	new_buf = (WCHAR *)_everything3_mem_alloc(_everything3_safe_size_add(size_in_wchars, size_in_wchars));

	if (new_buf) {
		wcbuf->buf = new_buf;
		wcbuf->size_in_wchars = size_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_wchar_buf_grow_length(_everything3_wchar_buf_t *wcbuf, SIZE_T length_in_wchars) {
	if (_everything3_wchar_buf_grow_size(wcbuf, _everything3_safe_size_add(length_in_wchars, 1))) {
		wcbuf->length_in_wchars = length_in_wchars;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_send(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size) {
	if (in_size <= EVERYTHING3_DWORD_MAX) {
		_everything3_message_t send_message;
		DWORD numwritten;
		OVERLAPPED send_overlapped;
		BYTE *send_p;
		DWORD send_run;

		send_message.code = code;
		send_message.size = (DWORD)in_size;

		send_overlapped.hEvent = client->send_event;
		send_overlapped.Offset = 0;
		send_overlapped.OffsetHigh = 0;

		send_p = (BYTE *)&send_message;
		send_run = sizeof(send_message);

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				if (numwritten) {
					send_p += numwritten;
					send_run -= numwritten;

					continue;
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						if (numwritten) {
							send_p += numwritten;
							send_run -= numwritten;

							continue;
						} else {
							SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

							return FALSE;
						}
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		send_p = (BYTE *)in_data;
		send_run = (DWORD)in_size;

		while (send_run) {
			if (WriteFile(client->pipe_handle, send_p, send_run, &numwritten, &send_overlapped)) {
				send_p += numwritten;
				send_run -= numwritten;

				continue;
			} else {
				DWORD last_error;

				last_error = GetLastError();

				if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
					HANDLE wait_handles[2];

					wait_handles[0] = client->shutdown_event;
					wait_handles[1] = client->send_event;

					if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
						SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

						return FALSE;
					}

					if (GetOverlappedResult(client->pipe_handle, &send_overlapped, &numwritten, FALSE)) {
						send_p += numwritten;
						send_run -= numwritten;

						continue;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						return FALSE;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					return FALSE;
				}
			}
		}

		return TRUE;
	} else {
		SetLastError(EVERYTHING3_ERROR_OUT_OF_MEMORY);

		return FALSE;
	}
}

static BOOL _everything3_recv_header(_everything3_client_t *client, _everything3_message_t *recv_header) {
	BOOL ret;

	ret = FALSE;

	if (_everything3_recv_data(client, recv_header, sizeof(_everything3_message_t))) {
		if ((recv_header->code == _EVERYTHING3_RESPONSE_OK) || (recv_header->code == _EVERYTHING3_RESPONSE_OK_MORE_DATA)) {
			ret = TRUE;
		} else {
			if (_everything3_recv_skip(client, recv_header->size)) {
				if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_BAD_REQUEST) {
					SetLastError(EVERYTHING3_ERROR_BAD_REQUEST);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_CANCELLED) {
					SetLastError(EVERYTHING3_ERROR_CANCELLED);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_NOT_FOUND) {
					SetLastError(EVERYTHING3_ERROR_IPC_PIPE_NOT_FOUND);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_OUT_OF_MEMORY) {
					SetLastError(EVERYTHING3_ERROR_SERVER);
				} else if (recv_header->code == _EVERYTHING3_RESPONSE_ERROR_INVALID_COMMAND) {
					SetLastError(EVERYTHING3_ERROR_INVALID_COMMAND);
				} else {
					SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
				}
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_data(_everything3_client_t *client, void *buf, SIZE_T buf_size) {
	BOOL ret;
	DWORD numread;
	OVERLAPPED recv_overlapped;
	BYTE *recv_p;
	SIZE_T recv_run;

	ret = FALSE;
	recv_overlapped.hEvent = client->recv_event;
	recv_overlapped.Offset = 0;
	recv_overlapped.OffsetHigh = 0;

	recv_p = (BYTE *)buf;
	recv_run = buf_size;

	for (;;) {
		DWORD chunk_size;

		if (!recv_run) {
			ret = TRUE;

			break;
		}

		if (recv_run <= 65536) {
			chunk_size = (DWORD)recv_run;
		} else {
			chunk_size = 65536;
		}

		if (ReadFile(client->pipe_handle, recv_p, chunk_size, &numread, &recv_overlapped)) {
			if (numread) {
				recv_p += numread;
				recv_run -= numread;
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		} else {
			DWORD last_error;

			last_error = GetLastError();

			if ((last_error == ERROR_IO_INCOMPLETE) || (last_error == ERROR_IO_PENDING)) {
				HANDLE wait_handles[2];

				wait_handles[0] = client->shutdown_event;
				wait_handles[1] = client->recv_event;

				if (WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE) == WAIT_OBJECT_0) {
					SetLastError(EVERYTHING3_ERROR_SHUTDOWN);

					break;
				}

				if (GetOverlappedResult(client->pipe_handle, &recv_overlapped, &numread, FALSE)) {
					if (numread) {
						recv_p += numread;
						recv_run -= numread;
					} else {
						SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

						break;
					}
				} else {
					SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

					break;
				}
			} else {
				SetLastError(EVERYTHING3_ERROR_DISCONNECTED);

				break;
			}
		}
	}

	return ret;
}

static BOOL _everything3_recv_skip(_everything3_client_t *client, SIZE_T size) {
	BOOL ret;
	BYTE buf[256];
	SIZE_T run;

	ret = FALSE;
	run = size;

	for (;;) {
		SIZE_T recv_size;

		if (!run) {
			ret = TRUE;
			break;
		}

		recv_size = run;
		if (recv_size > 256) {
			recv_size = 256;
		}

		if (!_everything3_recv_data(client, buf, recv_size)) {
			break;
		}

		run -= recv_size;
	}

	return ret;
}


static BOOL _everything3_ioctrl(_everything3_client_t *client, DWORD code, const void *in_data, SIZE_T in_size, void *out_data, SIZE_T out_size, SIZE_T *out_numread) {
	BOOL ret;

	ret = FALSE;

	if (client) {
		_everything3_Lock(client);

		if (_everything3_send(client, code, in_data, in_size)) {
			_everything3_message_t recv_header;

			if (_everything3_recv_header(client, &recv_header)) {
				if (recv_header.size <= out_size) {
					if (_everything3_recv_data(client, out_data, recv_header.size)) {
						if (out_numread) {
							*out_numread = recv_header.size;
						}

						ret = TRUE;
					}
				} else {
					if (_everything3_recv_skip(client, recv_header.size)) {
						SetLastError(EVERYTHING3_ERROR_INSUFFICIENT_BUFFER);
					}
				}
			}
		}

		_everything3_Unlock(client);
	} else {
		SetLastError(EVERYTHING3_ERROR_INVALID_PARAMETER);
	}

	return ret;
}

static void _everything3_utf8_buf_init(_everything3_utf8_buf_t *cbuf) {
	cbuf->buf = cbuf->stack_buf;
	cbuf->length_in_bytes = 0;
	cbuf->size_in_bytes = _EVERYTHING3_UTF8_BUF_STACK_SIZE;
	cbuf->buf[0] = 0;
}

static void _everything3_utf8_buf_kill(_everything3_utf8_buf_t *cbuf) {
	if (cbuf->buf != cbuf->stack_buf) {
		_everything3_mem_free(cbuf->buf);
	}
}

static void _everything3_utf8_buf_empty(_everything3_utf8_buf_t *cbuf) {
	_everything3_utf8_buf_kill(cbuf);
	_everything3_utf8_buf_init(cbuf);
}

static BOOL _everything3_utf8_buf_grow_size(_everything3_utf8_buf_t *cbuf, SIZE_T size_in_bytes) {
	BYTE *new_buf;

	if (size_in_bytes <= cbuf->size_in_bytes) {
		return TRUE;
	}

	_everything3_utf8_buf_empty(cbuf);

	new_buf = (BYTE *)_everything3_mem_alloc(size_in_bytes);

	if (new_buf) {
		cbuf->buf = new_buf;
		cbuf->size_in_bytes = size_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static BOOL _everything3_utf8_buf_grow_length(_everything3_utf8_buf_t *cbuf, SIZE_T length_in_bytes) {
	if (_everything3_utf8_buf_grow_size(cbuf, _everything3_safe_size_add(length_in_bytes, 1))) {
		cbuf->length_in_bytes = length_in_bytes;

		return TRUE;
	}

	return FALSE;
}

static EVERYTHING3_UTF8 *_everything3_utf8_string_copy_wchar_string(EVERYTHING3_UTF8 *buf, const EVERYTHING3_WCHAR *ws) {
	const EVERYTHING3_WCHAR *p;
	EVERYTHING3_UTF8 *d;
	int c;

	p = ws;
	d = buf;

	while (*p) {
		c = *p++;

		// surrogates
		if ((c >= 0xD800) && (c < 0xDC00)) {
			if ((*p >= 0xDC00) && (*p < 0xE000)) {
				c = 0x10000 + ((c - 0xD800) << 10) + (*p - 0xDC00);

				p++;
			}
		}

		if (c > 0xffff) {
			// 4 bytes
			if (buf) {
				*d++ = ((c >> 18) & 0x07) | 0xF0; // 11110xxx
				*d++ = ((c >> 12) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 4);
			}
		} else if (c > 0x7ff) {
			// 3 bytes
			if (buf) {
				*d++ = ((c >> 12) & 0x0f) | 0xE0; // 1110xxxx
				*d++ = ((c >> 6) & 0x3f) | 0x80; // 10xxxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 3);
			}
		} else if (c > 0x7f) {
			// 2 bytes
			if (buf) {
				*d++ = ((c >> 6) & 0x1f) | 0xC0; // 110xxxxx
				*d++ = (c & 0x3f) | 0x80; // 10xxxxxx
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 2);
			}
		} else {
			// ascii
			if (buf) {
				*d++ = c;
			} else {
				d = (EVERYTHING3_UTF8 *)_everything3_safe_size_add((SIZE_T)d, 1);
			}
		}
	}

	if (buf) {
		*d = 0;
	}

	return d;
}

static BOOL _everything3_utf8_buf_copy_wchar_string(_everything3_utf8_buf_t *cbuf, const EVERYTHING3_WCHAR *ws) {
	if (_everything3_utf8_buf_grow_length(cbuf, (SIZE_T)_everything3_utf8_string_copy_wchar_string(NULL, ws))) {
		_everything3_utf8_string_copy_wchar_string(cbuf->buf, ws);

		return TRUE;
	}

	return FALSE;
}


static EVERYTHING3_UINT64 _everything3_get_folder_size(EVERYTHING3_CLIENT *client, const EVERYTHING3_UTF8 *filename, SIZE_T filename_length_in_bytes) {
	EVERYTHING3_UINT64 ret;
	EVERYTHING3_UINT64 value;
	SIZE_T numread;

	ret = EVERYTHING3_UINT64_MAX;

	if (_everything3_ioctrl(client, _EVERYTHING3_COMMAND_GET_FOLDER_SIZE, filename, filename_length_in_bytes, &value, sizeof(EVERYTHING3_UINT64), &numread)) {
		if (numread == sizeof(EVERYTHING3_UINT64)) {
			ret = value;

			if (ret == EVERYTHING3_UINT64_MAX) {
				SetLastError(EVERYTHING3_OK);
			}
		} else {
			SetLastError(EVERYTHING3_ERROR_BAD_RESPONSE);
		}
	}

	return ret;
}

EVERYTHING3_UINT64 EVERYTHING3_API Everything3_GetFolderSizeFromFilenameW(EVERYTHING3_CLIENT *client, LPCWSTR lpFilename) {
	EVERYTHING3_UINT64 ret;
	_everything3_utf8_buf_t filename_cbuf;

	ret = EVERYTHING3_UINT64_MAX;

	_everything3_utf8_buf_init(&filename_cbuf);

	if (_everything3_utf8_buf_copy_wchar_string(&filename_cbuf, lpFilename)) {
		ret = _everything3_get_folder_size(client, filename_cbuf.buf, filename_cbuf.length_in_bytes);
	}

	_everything3_utf8_buf_kill(&filename_cbuf);

	return ret;
}

// Severely reduced Everything_IPC.h, Copyright (C) 2022 David Carpenter
// https://www.voidtools.com/Everything-SDK.zip

#define EVERYTHING_IPC_WNDCLASSW				L"EVERYTHING_TASKBAR_NOTIFICATION"
#define EVERYTHING_IPC_WNDCLASSW_15A			L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)"

#define EVERYTHING_IPC_MATCHCASE				0x00000001
#define EVERYTHING_IPC_MATCHDIACRITICS			0x00000010
#define EVERYTHING_IPC_COPYDATA_QUERY2W			18
#define EVERYTHING_IPC_SORT_NAME_ASCENDING		1

#define EVERYTHING_IPC_QUERY2_REQUEST_SIZE		0x00000010

typedef struct {
	DWORD reply_hwnd;
	DWORD reply_copydata_message;
	DWORD search_flags;
	DWORD offset;
	DWORD max_results;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_QUERY2;

typedef struct {
	DWORD totitems;
	DWORD numitems;
	DWORD offset;
	DWORD request_flags;
	DWORD sort_type;
} EVERYTHING_IPC_LIST2;

typedef struct {
	DWORD flags;
	DWORD data_offset;
} EVERYTHING_IPC_ITEM2;

// clang-format on
#pragma endregion  // everything_sdk

constexpr DWORD kGsTimeoutIPC = 1000;

#define GS_SEARCH_PREFIX L"folder:wfn:\""
#define GS_SEARCH_SUFFIX L"\""

std::atomic<HWND> g_gsReceiverWnd;

struct {
    // Auto-reset event signalled when a reply to a search query is received.
    std::atomic<HANDLE> hEvent;
    DWORD dwID;
    bool bResult;
    int64_t liSize;
} g_gsReply;

std::mutex g_gsReplyMutex;
std::mutex g_gsReplyCopyDataMutex;
std::atomic<DWORD> g_gsReplyCounter;

enum : unsigned {
    ES_QUERY_OK,
    ES_QUERY_NO_INDEX,
    ES_QUERY_NO_ES_IPC,
    ES_QUERY_NO_PLUGIN_IPC,
    ES_QUERY_TIMEOUT,
    ES_QUERY_REPLY_TIMEOUT,
    ES_QUERY_ZERO_SIZE_REPARSE_POINT,
};

PCWSTR g_gsQueryStatus[] = {
    L"Ok",
    L"No Index",
    L"No ES IPC",
    L"No Plugin IPC",
    L"Query Timeout",
    L"Reply Timeout",
    L"Zero-size reparse point",
};

std::mutex g_everything4Wh_ThreadMutex;
std::atomic<HANDLE> g_everything4Wh_Thread;
HANDLE g_everything4Wh_ThreadReadyEvent;

bool IsUncPath(PCWSTR folderPath) {
    return folderPath[0] == L'\\' && folderPath[1] == L'\\';
}

bool IsReparse(PCWSTR folderPath) {
    WIN32_FILE_ATTRIBUTE_DATA fad = {};
    return GetFileAttributesEx(folderPath, GetFileExInfoStandard, &fad) &&
           (fad.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring NativePathToPath(std::wstring_view wide_path) {
    constexpr auto NativeUncPathPrefix = L"\\\\?\\UNC\\"sv;
    if (wide_path.starts_with(NativeUncPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeUncPathPrefix));
        std::wstring result{L"\\\\"};
        result += wide_path;
        return result;
    }

    constexpr auto NativeLocalPathPrefix = L"\\\\?\\"sv;
    if (wide_path.starts_with(NativeLocalPathPrefix)) {
        wide_path.remove_prefix(std::size(NativeLocalPathPrefix));
        return std::wstring{wide_path};
    }

    return std::wstring{wide_path};
}

// Code based on:
// https://github.com/transmission/transmission/blob/f2aeb11b0733957d8d77d7038daa3ae88442dd5b/libtransmission/file-win32.cc
std::wstring ResolvePath(PCWSTR path) {
    std::wstring result;

    if (auto const handle = CreateFile(
            path, FILE_READ_EA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
        handle != INVALID_HANDLE_VALUE) {
        if (auto const wide_ret_size =
                GetFinalPathNameByHandle(handle, nullptr, 0, 0);
            wide_ret_size != 0) {
            std::wstring wide_ret;
            wide_ret.resize(wide_ret_size);
            if (GetFinalPathNameByHandle(handle, std::data(wide_ret),
                                         wide_ret_size,
                                         0) == wide_ret_size - 1) {
                // `wide_ret_size` includes the terminating '\0'; remove it from
                // `wide_ret`.
                wide_ret.resize(std::size(wide_ret) - 1);
                result = NativePathToPath(wide_ret);
            }
        }

        CloseHandle(handle);
    }

    return result;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter);

unsigned Everything4Wh_GetFileSize(PCWSTR folderPath, int64_t* size) {
    *size = 0;

    // Prevent querying from within the Everything process to avoid deadlocks.
    if (g_isEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    EVERYTHING3_CLIENT* pClient = Everything3_ConnectW(nullptr);
    if (pClient) {
        Wh_Log(L"Connected to Everything IPC (unnamed instance)");
    } else {
        pClient = Everything3_ConnectW(L"1.5a");
        if (pClient) {
            Wh_Log(L"Connected to Everything IPC (v1.5a)");
        }
    }

    if (pClient) {
        *size = Everything3_GetFolderSizeFromFilenameW(pClient, folderPath);
        Everything3_DestroyClient(pClient);

        if (*size == -1) {
            return ES_QUERY_NO_INDEX;
        }

        if (!*size && IsReparse(folderPath)) {
            return ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        }

        return ES_QUERY_OK;
    }

    HWND hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW_15A, nullptr);
    if (hEverything) {
        Wh_Log(L"Found Everything IPC window (v1.5a) 0x%08X",
               (DWORD)(DWORD_PTR)hEverything);
    } else {
        hEverything = FindWindow(EVERYTHING_IPC_WNDCLASSW, nullptr);
        if (hEverything) {
            Wh_Log(L"Found Everything IPC window 0x%08X",
                   (DWORD)(DWORD_PTR)hEverything);
        }
    }

    if (!hEverything) {
        return ES_QUERY_NO_ES_IPC;
    }

    if (!g_everything4Wh_Thread) {
        std::lock_guard<std::mutex> guard(g_everything4Wh_ThreadMutex);

        if (!g_everything4Wh_Thread) {
            g_everything4Wh_ThreadReadyEvent =
                CreateEvent(nullptr, TRUE, FALSE, nullptr);

            g_everything4Wh_Thread = CreateThread(
                nullptr, 0, Everything4Wh_Thread, nullptr, 0, nullptr);
            if (g_everything4Wh_Thread) {
                // Wait for a message queue to be created.
                WaitForSingleObject(g_everything4Wh_ThreadReadyEvent, INFINITE);
                CloseHandle(g_everything4Wh_ThreadReadyEvent);
            } else {
                Wh_Log(L"CreateThread failed: %d", GetLastError());
            }

            g_everything4Wh_ThreadReadyEvent = nullptr;
        }
    }

    HWND hReceiverWnd = g_gsReceiverWnd;

    if (!hReceiverWnd) {
        return ES_QUERY_NO_PLUGIN_IPC;
    }

    DWORD dwSize = sizeof(EVERYTHING_IPC_QUERY2) +
                   sizeof(GS_SEARCH_PREFIX GS_SEARCH_SUFFIX) +
                   (wcslen(folderPath) * sizeof(WCHAR));
    std::vector<BYTE> queryBuffer(dwSize, 0);
    EVERYTHING_IPC_QUERY2* pQuery = (EVERYTHING_IPC_QUERY2*)queryBuffer.data();

    pQuery->reply_hwnd = (DWORD)(DWORD_PTR)hReceiverWnd;
    pQuery->reply_copydata_message = ++g_gsReplyCounter;
    // Pokémon != Pokemon | folder != FOLDER (Windows Subsystem for Linux).
    pQuery->search_flags =
        EVERYTHING_IPC_MATCHCASE | EVERYTHING_IPC_MATCHDIACRITICS;
    pQuery->offset = 0;
    pQuery->max_results = 1;
    pQuery->request_flags = EVERYTHING_IPC_QUERY2_REQUEST_SIZE;
    // Unused (defined for clarity).
    pQuery->sort_type = EVERYTHING_IPC_SORT_NAME_ASCENDING;

    swprintf_s((LPWSTR)(pQuery + 1),
               (dwSize - sizeof(*pQuery)) / sizeof(wchar_t), L"%s%s%s",
               GS_SEARCH_PREFIX, folderPath, GS_SEARCH_SUFFIX);

    COPYDATASTRUCT cds = {
        .dwData = EVERYTHING_IPC_COPYDATA_QUERY2W,
        .cbData = dwSize,
        .lpData = pQuery,
    };

    std::lock_guard<std::mutex> guard(g_gsReplyMutex);

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = pQuery->reply_copydata_message;
    }

    unsigned result;

    ResetEvent(g_gsReply.hEvent);

    LRESULT lrPending = SendMessageTimeout(
        hEverything, WM_COPYDATA, (WPARAM)hReceiverWnd, (LPARAM)&cds,
        SMTO_BLOCK | SMTO_ABORTIFHUNG, kGsTimeoutIPC, nullptr);
    if (lrPending) {
        // Wait on ReceiverWndProc (below) to signal that the reply was
        // processed.
        DWORD waitResult = WaitForSingleObject(g_gsReply.hEvent, kGsTimeoutIPC);
        if (waitResult != WAIT_OBJECT_0) {
            result = ES_QUERY_REPLY_TIMEOUT;
        } else if (!g_gsReply.bResult) {
            result = ES_QUERY_NO_INDEX;
        } else if (!g_gsReply.liSize && IsReparse(folderPath)) {
            result = ES_QUERY_ZERO_SIZE_REPARSE_POINT;
        } else {
            *size = g_gsReply.liSize;
            result = ES_QUERY_OK;
        }
    } else {
        result = ES_QUERY_TIMEOUT;
    }

    {
        std::lock_guard<std::mutex> copyDataGuard(g_gsReplyCopyDataMutex);
        g_gsReply.dwID = 0;
    }

    return result;
}

LRESULT CALLBACK Everything4Wh_ReceiverWndProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    switch (uMsg) {
        case WM_COPYDATA: {
            std::lock_guard<std::mutex> guard(g_gsReplyCopyDataMutex);

            COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

            if (pcds->dwData == g_gsReply.dwID) {
                EVERYTHING_IPC_LIST2* list =
                    (EVERYTHING_IPC_LIST2*)pcds->lpData;

                if (list->numitems) {
                    g_gsReply.liSize =
                        *(int64_t*)(((BYTE*)(list + 1)) +
                                    sizeof(EVERYTHING_IPC_ITEM2));
                    g_gsReply.bResult = true;
                } else {
                    g_gsReply.liSize = 0;
                    g_gsReply.bResult = false;
                }

                SetEvent(g_gsReply.hEvent);
            }

            return TRUE;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

DWORD WINAPI Everything4Wh_Thread(void* parameter) {
    constexpr WCHAR kClassName[] = "WindhawkEverythingReceiver_" WH_MOD_ID;

    HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        Wh_Log(L"CreateEvent failed");
        return 1;
    }

    WNDCLASSEXW wc = {
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = Everything4Wh_ReceiverWndProc,
        .hInstance = GetCurrentModuleHandle(),
        .lpszClassName = kClassName,
    };

    if (!RegisterClassEx(&wc)) {
        Wh_Log(L"RegisterClassEx failed");
        CloseHandle(hEvent);
        return 1;
    }

    HWND hReceiverWnd =
        CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                       HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!hReceiverWnd) {
        Wh_Log(L"CreateWindowEx failed");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        CloseHandle(hEvent);
        return 1;
    }

    ChangeWindowMessageFilterEx(hReceiverWnd, WM_COPYDATA, MSGFLT_ALLOW,
                                nullptr);

    g_gsReply.hEvent = hEvent;
    g_gsReceiverWnd = hReceiverWnd;

    Wh_Log(L"hReceiverWnd=0x%08X", (DWORD)(DWORD_PTR)hReceiverWnd);

    SetEvent(g_everything4Wh_ThreadReadyEvent);

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            g_gsReceiverWnd = nullptr;
            DestroyWindow(hReceiverWnd);
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    g_gsReply.hEvent = nullptr;
    CloseHandle(hEvent);
    return 0;
}

std::vector<BYTE> PIDLToVector(const ITEMIDLIST* pidl) {
    if (!pidl) {
        return {};
    }

    const BYTE* ptr = reinterpret_cast<const BYTE*>(pidl);
    size_t size = ILGetSize(pidl);
    return std::vector<BYTE>(ptr, ptr + size);
}

thread_local winrt::com_ptr<IShellFolder2> g_cacheShellFolder;
thread_local std::map<std::vector<BYTE>, std::optional<ULONGLONG>>
    g_cacheShellFolderSizes;
thread_local DWORD g_cacheShellFolderLastUsedTickCount;

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

std::wstring GetFolderPathFromIShellFolder(IShellFolder2* shellFolder) {
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetIDListFromObject(shellFolder, &pidl);
    if (FAILED(hr)) {
        return {};
    }

    std::wstring path;
    path.resize(MAX_PATH);
    while (
        !SHGetPathFromIDListEx(pidl, &path[0], path.size(), GPFIDL_DEFAULT)) {
        // The maximum path length is documented to be "approximately" 32,767.
        if (path.size() >= 32767 + 256) {
            CoTaskMemFree(pidl);
            return {};
        }

        path.resize(path.size() * 2);
    }

    path.resize(wcslen(path.c_str()));
    CoTaskMemFree(pidl);

    // SHGetPathFromIDListEx() for long path returns path with super path prefix
    // "\\\\?\\".
    constexpr auto kSuperPathPrefix = L"\\\\?\\"sv;
    if (path.starts_with(kSuperPathPrefix)) {
        path = path.substr(kSuperPathPrefix.size());
    }

    return path;
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
    if (g_inCRecursiveFolderOperation_Prepare ||
        g_inCRecursiveFolderOperation_Do || ret != S_OK ||
        propVariant->vt != VT_EMPTY) {
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

    if (shellFolder2 != g_cacheShellFolder ||
        GetTickCount() - g_cacheShellFolderLastUsedTickCount > 1000) {
        g_cacheShellFolderSizes.clear();
    }

    g_cacheShellFolder = shellFolder2;

    auto [cacheIt, cacheMissing] = g_cacheShellFolderSizes.try_emplace(
        PIDLToVector(itemidChild), std::nullopt);

    if (cacheMissing) {
        winrt::com_ptr<IShellFolder2> childFolder;
        hr = shellFolder2->BindToObject(itemidChild, nullptr,
                                        IID_PPV_ARGS(childFolder.put()));
        if (FAILED(hr) || !childFolder) {
            Wh_Log(L"Failed: %08X", hr);
        } else if (g_settings.calculateFolderSizes ==
                   CalculateFolderSizes::everything) {
            const auto path = GetFolderPathFromIShellFolder(childFolder.get());
            if (!path.empty()) {
                Wh_Log(L"Getting size for %s", path.c_str());

                int64_t size;
                unsigned result =
                    Everything4Wh_GetFileSize(path.c_str(), &size);

                // Regular reparse points are indexed with size 0, and
                // ES_QUERY_ZERO_SIZE_REPARSE_POINT is returned when querying
                // the link itself. Subfolders of reparse points aren't indexed,
                // so ES_QUERY_NO_INDEX is returned in this case.
                //
                // Avoid resolving UNC folders which are not indexed as it can
                // be slow, and will be done for all folders if the UNC host
                // isn't indexed.
                if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT ||
                    (result == ES_QUERY_NO_INDEX && !IsUncPath(path.c_str()))) {
                    Wh_Log(L"Resolving path due to status: %s",
                           g_gsQueryStatus[result]);

                    auto resolved = ResolvePath(path.c_str());
                    if (resolved.empty()) {
                        Wh_Log(L"Failed to resolve path");
                    } else if (resolved == path) {
                        Wh_Log(L"Path is already resolved");

                        // In some cases, reparse points are indexed with the
                        // real size. In these cases, it was observed that the
                        // path resolves to itself. An example is OneDrive, see:
                        // https://github.com/ramensoftware/windhawk-mods/issues/1527
                        if (result == ES_QUERY_ZERO_SIZE_REPARSE_POINT) {
                            size = 0;
                            result = ES_QUERY_OK;
                        }
                    } else {
                        Wh_Log(L"Trying resolved path %s", resolved.c_str());
                        result =
                            Everything4Wh_GetFileSize(resolved.c_str(), &size);
                    }
                }

                if (result == ES_QUERY_OK) {
                    cacheIt->second = size;
                } else {
                    Wh_Log(L"Failed to get size: %s", g_gsQueryStatus[result]);
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

    g_cacheShellFolderLastUsedTickCount = GetTickCount();

    std::optional<ULONGLONG> folderSize = cacheIt->second;
    if (folderSize) {
        propVariant->uhVal.QuadPart = *folderSize;
        propVariant->vt = VT_UI8;
        Wh_Log(L"Done: %I64u", propVariant->uhVal.QuadPart);
    }

    return S_OK;
}

using CRecursiveFolderOperation_Prepare_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Prepare_t CRecursiveFolderOperation_Prepare_Original;
HRESULT __thiscall CRecursiveFolderOperation_Prepare_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Prepare = true;

    HRESULT ret = CRecursiveFolderOperation_Prepare_Original(pThis);

    g_inCRecursiveFolderOperation_Prepare = false;

    return ret;
}

using CRecursiveFolderOperation_Do_t = HRESULT(__thiscall*)(void* pThis);
CRecursiveFolderOperation_Do_t CRecursiveFolderOperation_Do_Original;
HRESULT __thiscall CRecursiveFolderOperation_Do_Hook(void* pThis) {
    auto hookScope = hookRefCountScope();

    g_inCRecursiveFolderOperation_Do = true;

    HRESULT ret = CRecursiveFolderOperation_Do_Original(pThis);

    g_inCRecursiveFolderOperation_Do = false;

    return ret;
}

using CFSFolder_MapColumnToSCID_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                     int column,
                                                     PROPERTYKEY* scid);
CFSFolder_MapColumnToSCID_t CFSFolder_MapColumnToSCID_Original;

using CFSFolder_GetDetailsEx_t = HRESULT(WINAPI*)(void* pCFSFolder,
                                                  const ITEMID_CHILD* itemid,
                                                  const PROPERTYKEY* scid,
                                                  VARIANT* value);
CFSFolder_GetDetailsEx_t CFSFolder_GetDetailsEx_Original;

using CFSFolder_CompareIDs_t =
    HRESULT(WINAPI*)(void* pCFSFolder,
                     int column,
                     const ITEMIDLIST_RELATIVE* itemid1,
                     const ITEMIDLIST_RELATIVE* itemid2);
CFSFolder_CompareIDs_t CFSFolder_CompareIDs_Original;
HRESULT WINAPI CFSFolder_CompareIDs_Hook(void* pCFSFolder,
                                         int column,
                                         const ITEMIDLIST_RELATIVE* itemid1,
                                         const ITEMIDLIST_RELATIVE* itemid2) {
    auto hookScope = hookRefCountScope();

    auto original = [=]() {
        return CFSFolder_CompareIDs_Original(pCFSFolder, column, itemid1,
                                             itemid2);
    };

    if (!itemid1 || !itemid2 || !g_settings.sortSizesMixFolders) {
        return original();
    }

    PROPERTYKEY columnSCID;
    if (FAILED(CFSFolder_MapColumnToSCID_Original(pCFSFolder, column,
                                                  &columnSCID)) ||
        !IsEqualPropertyKey(columnSCID, kPKEY_Size)) {
        return original();
    }

    _variant_t value1;
    _variant_t value2;
    bool succeeded =
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid1, &columnSCID, value1.GetAddress())) &&
        value1.vt == VT_UI8 &&
        SUCCEEDED(CFSFolder_GetDetailsEx_Original(
            pCFSFolder, itemid2, &columnSCID, value2.GetAddress())) &&
        value2.vt == VT_UI8;

    if (!succeeded) {
        return original();
    }

    ULONGLONG size1 = value1.ullVal;
    ULONGLONG size2 = value2.ullVal;

    if (size1 > size2) {
        return 1;
    } else if (size1 < size2) {
        return 0xFFFF;
    } else {
        return 0;
    }
}

using SHOpenFolderAndSelectItems_t = decltype(&SHOpenFolderAndSelectItems);
SHOpenFolderAndSelectItems_t SHOpenFolderAndSelectItems_Original;
HRESULT WINAPI SHOpenFolderAndSelectItems_Hook(LPCITEMIDLIST pidlFolder,
                                               UINT cidl,
                                               LPCITEMIDLIST* apidl,
                                               DWORD dwFlags) {
    Wh_Log(L">");

    // If the current thread is the UI thread, run the function in a new thread
    // to prevent a deadlock. See:
    // https://voidtools.com/forum/viewtopic.php?p=71433#p71433

    bool isUiThread = false;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            constexpr WCHAR kClassNamePrefix[] =
                L"EVERYTHING_TASKBAR_NOTIFICATION";
            constexpr size_t kClassNameLen = ARRAYSIZE(kClassNamePrefix) - 1;

            bool& isUiThread = *(bool*)lParam;

            WCHAR szClassName[64];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsnicmp(kClassNamePrefix, szClassName, kClassNameLen) == 0) {
                isUiThread = true;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&isUiThread);

    if (!isUiThread) {
        return SHOpenFolderAndSelectItems_Original(pidlFolder, cidl, apidl,
                                                   dwFlags);
    }

    struct ThreadParam {
        LPITEMIDLIST pidlFolder;
        std::vector<LPITEMIDLIST> apidl;
        DWORD dwFlags;
    };

    auto threadParam = std::make_unique<ThreadParam>();

    threadParam->pidlFolder = ILClone(pidlFolder);

    if (cidl) {
        threadParam->apidl.reserve(cidl);
        for (UINT i = 0; i < cidl; i++) {
            threadParam->apidl.push_back(ILClone(apidl[i]));
        }
    }

    threadParam->dwFlags = dwFlags;

    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) WINAPI -> DWORD {
            Wh_Log(L">");

            std::unique_ptr<ThreadParam> threadParam{(ThreadParam*)lpParam};

            CoInitializeEx(nullptr, COINIT_MULTITHREADED);

            SHOpenFolderAndSelectItems_Original(
                threadParam->pidlFolder, threadParam->apidl.size(),
                (LPCITEMIDLIST*)threadParam->apidl.data(),
                threadParam->dwFlags);

            ILFree(threadParam->pidlFolder);
            for (auto pidl : threadParam->apidl) {
                ILFree(pidl);
            }

            CoUninitialize();
            return 0;
        },
        threadParam.release(), 0, nullptr);
    if (thread) {
        CloseHandle(thread);
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

using PSFormatForDisplay_t = decltype(&PSFormatForDisplay);
PSFormatForDisplay_t PSFormatForDisplay_Original;
HRESULT WINAPI PSFormatForDisplay_Hook(const PROPERTYKEY& propkey,
                                       const PROPVARIANT& propvar,
                                       PROPDESC_FORMAT_FLAGS pdfFlags,
                                       LPWSTR pwszText,
                                       DWORD cchText) {
    auto original = [=]() {
        return PSFormatForDisplay_Original(propkey, propvar, pdfFlags, pwszText,
                                           cchText);
    };

    PROPDESC_FORMAT_FLAGS pdfFlagsNew = pdfFlags & ~PDFF_ALWAYSKB;
    if (pdfFlagsNew == pdfFlags) {
        return original();
    }

    void* retAddress = __builtin_return_address(0);

    HMODULE shell32 = GetModuleHandle(L"shell32.dll");
    if (!shell32) {
        return original();
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != shell32) {
        return original();
    }

    Wh_Log(L">");

    return PSFormatForDisplay_Original(propkey, propvar, pdfFlagsNew, pwszText,
                                       cchText);
}

using PSStrFormatKBSizeW_t = void*(WINAPI*)(ULONGLONG size,
                                            LPWSTR pwszText,
                                            DWORD cchText);
PSStrFormatKBSizeW_t PSStrFormatKBSizeW_Original;
void* WINAPI PSStrFormatKBSizeW_Hook(ULONGLONG size,
                                     LPWSTR pwszText,
                                     DWORD cchText) {
    Wh_Log(L">");

    void* ret = PSStrFormatKBSizeW_Original(size, pwszText, cchText);

    if (!pwszText || cchText == 0) {
        return ret;
    }

    int len = wcslen(pwszText);
    if (len < 2 || (size_t)len + 1 > cchText - 1 || pwszText[len - 2] != 'K' ||
        pwszText[len - 1] != 'B') {
        return ret;
    }

    pwszText[len - 1] = 'i';
    pwszText[len] = 'B';
    pwszText[len + 1] = '\0';

    return ret;
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
    HMODULE windowsStorageModule = LoadLibraryEx(
        L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
        {
            {
#ifdef _WIN64
                LR"(public: long __cdecl CRecursiveFolderOperation::Prepare(void))",
#else
                LR"(public: long __thiscall CRecursiveFolderOperation::Prepare(void))",
#endif
            },
            &CRecursiveFolderOperation_Prepare_Original,
            CRecursiveFolderOperation_Prepare_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CRecursiveFolderOperation::Do(void))",
#else
                LR"(public: virtual long __thiscall CRecursiveFolderOperation::Do(void))",
#endif
            },
            &CRecursiveFolderOperation_Do_Original,
            CRecursiveFolderOperation_Do_Hook,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::MapColumnToSCID(unsigned int,struct _tagpropertykey *))",
#endif
            },
            &CFSFolder_MapColumnToSCID_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const __unaligned *,struct _tagpropertykey const *,struct tagVARIANT *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::GetDetailsEx(struct _ITEMID_CHILD const *,struct _tagpropertykey const *,struct tagVARIANT *))",
#endif
            },
            &CFSFolder_GetDetailsEx_Original,
        },
        {
            {
#ifdef _WIN64
                LR"(public: virtual long __cdecl CFSFolder::CompareIDs(__int64,struct _ITEMIDLIST_RELATIVE const __unaligned *,struct _ITEMIDLIST_RELATIVE const __unaligned *))",
#else
                LR"(public: virtual long __stdcall CFSFolder::CompareIDs(long,struct _ITEMIDLIST_RELATIVE const *,struct _ITEMIDLIST_RELATIVE const *))",
#endif
            },
            &CFSFolder_CompareIDs_Original,
            CFSFolder_CompareIDs_Hook,
        },
    };

    return HookSymbols(windowsStorageModule, windowsStorageHooks,
                       ARRAYSIZE(windowsStorageHooks));
}

// A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
// Separate mod implementation:
// https://gist.github.com/m417z/f0cdf071868a6f31210e84dd0d444055.
// This workaround will be included in Windhawk in future versions.
#include <cxxabi.h>
#include <locale.h>
namespace ProcessShutdownMessageBoxFix {

using _errno_t = decltype(&_errno);

WCHAR errorMsg[1025];
void** g_ppSetlocale;
HMODULE g_msvcrtModule;
_errno_t g_msvcrtErrno;
bool g_msvcrtSetLocaleIsPatched;

void** FindImportPtr(HMODULE hFindInModule,
                     PCSTR pModuleName,
                     PCSTR pImportName) {
    IMAGE_DOS_HEADER* pDosHeader;
    IMAGE_NT_HEADERS* pNtHeader;
    ULONG_PTR ImageBase;
    IMAGE_IMPORT_DESCRIPTOR* pImportDescriptor;
    ULONG_PTR* pOriginalFirstThunk;
    ULONG_PTR* pFirstThunk;
    ULONG_PTR ImageImportByName;

    // Init
    pDosHeader = (IMAGE_DOS_HEADER*)hFindInModule;
    pNtHeader = (IMAGE_NT_HEADERS*)((char*)pDosHeader + pDosHeader->e_lfanew);

    if (!pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress)
        return nullptr;

    ImageBase = (ULONG_PTR)hFindInModule;
    pImportDescriptor =
        (IMAGE_IMPORT_DESCRIPTOR*)(ImageBase +
                                   pNtHeader->OptionalHeader.DataDirectory[1]
                                       .VirtualAddress);

    // Search!
    while (pImportDescriptor->OriginalFirstThunk) {
        if (lstrcmpiA((char*)(ImageBase + pImportDescriptor->Name),
                      pModuleName) == 0) {
            pOriginalFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->OriginalFirstThunk);
            ImageImportByName = *pOriginalFirstThunk;

            pFirstThunk =
                (ULONG_PTR*)(ImageBase + pImportDescriptor->FirstThunk);

            while (ImageImportByName) {
                if (!(ImageImportByName & IMAGE_ORDINAL_FLAG)) {
                    if ((ULONG_PTR)pImportName & ~0xFFFF) {
                        ImageImportByName += sizeof(WORD);

                        if (lstrcmpA((char*)(ImageBase + ImageImportByName),
                                     pImportName) == 0)
                            return (void**)pFirstThunk;
                    }
                } else {
                    if (((ULONG_PTR)pImportName & ~0xFFFF) == 0)
                        if ((ImageImportByName & 0xFFFF) ==
                            (ULONG_PTR)pImportName)
                            return (void**)pFirstThunk;
                }

                pOriginalFirstThunk++;
                ImageImportByName = *pOriginalFirstThunk;

                pFirstThunk++;
            }
        }

        pImportDescriptor++;
    }

    return nullptr;
}

using SetLocale_t = char*(__cdecl*)(int category, const char* locale);
SetLocale_t SetLocale_Original;
char* __cdecl SetLocale_Wrapper(int category, const char* locale) {
    // A workaround for https://github.com/mstorsjo/llvm-mingw/issues/459.
    errno_t* err = g_msvcrtErrno();
    HMODULE module;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (PCWSTR)err, &module) &&
        module == g_msvcrtModule) {
        // Getting a process-wide errno from the module section instead of the
        // thread errno from the heap means we have no PTD (Per-thread data) and
        // setlocale will fail, likely with a message box and abort. Return NULL
        // instead to let the caller handle it gracefully.
        // Wh_Log(L"Returning NULL for setlocale");
        return nullptr;
    }

    return SetLocale_Original(category, locale);
}

bool Init(HMODULE module) {
    // Make sure the functions are in the import table.
    void* p;
    InterlockedExchangePointer(&p, (void*)__cxxabiv1::__cxa_throw);
    InterlockedExchangePointer(&p, (void*)setlocale);

    void** ppCxaThrow = FindImportPtr(module, "libc++.dll", "__cxa_throw");
    if (!ppCxaThrow) {
        wsprintf(errorMsg, L"No __cxa_throw");
        return false;
    }

    void** ppSetlocale = FindImportPtr(module, "msvcrt.dll", "setlocale");
    if (!ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale");
        return false;
    }

    HMODULE libcppModule;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppCxaThrow, &libcppModule)) {
        wsprintf(errorMsg, L"No libcpp module");
        return false;
    }

    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*ppSetlocale, &g_msvcrtModule)) {
        wsprintf(errorMsg, L"No msvcrt module");
        return false;
    }

    g_ppSetlocale = FindImportPtr(libcppModule, "msvcrt.dll", "setlocale");
    if (!g_ppSetlocale) {
        wsprintf(errorMsg, L"No setlocale for libc++.dll");
        return false;
    }

    HMODULE msvcrtModuleForLibcpp;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)*g_ppSetlocale, &msvcrtModuleForLibcpp)) {
        wsprintf(errorMsg, L"No msvcrt module for libc++.dll");
        return false;
    }

    if (msvcrtModuleForLibcpp != g_msvcrtModule) {
        wsprintf(errorMsg, L"Bad msvcrt module, already patched? %p!=%p",
                 msvcrtModuleForLibcpp, g_msvcrtModule);
        return false;
    }

    g_msvcrtErrno = (_errno_t)GetProcAddress(msvcrtModuleForLibcpp, "_errno");
    if (!g_msvcrtErrno) {
        wsprintf(errorMsg, L"No _errno");
        return false;
    }

    DWORD dwOldProtect;
    if (!VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                        &dwOldProtect)) {
        wsprintf(errorMsg, L"VirtualProtect failed");
        return false;
    }

    SetLocale_Original = (SetLocale_t)*g_ppSetlocale;
    *g_ppSetlocale = (void*)SetLocale_Wrapper;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);

    g_msvcrtSetLocaleIsPatched = true;
    return true;
}

void LogErrorIfAny() {
    if (*errorMsg) {
        Wh_Log(L"%s", errorMsg);
    }
}

void Uninit() {
    if (!g_msvcrtSetLocaleIsPatched) {
        return;
    }

    DWORD dwOldProtect;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), PAGE_READWRITE,
                   &dwOldProtect);
    *g_ppSetlocale = (void*)SetLocale_Original;
    VirtualProtect(g_ppSetlocale, sizeof(*g_ppSetlocale), dwOldProtect,
                   &dwOldProtect);
}

}  // namespace ProcessShutdownMessageBoxFix

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ProcessShutdownMessageBoxFix::Init(hinstDLL);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            // Do not do cleanup if process termination scenario.
            if (lpReserved) {
                break;
            }

            ProcessShutdownMessageBoxFix::Uninit();
            break;
    }

    return TRUE;
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

    g_settings.sortSizesMixFolders = Wh_GetIntSetting(L"sortSizesMixFolders");
    g_settings.disableKbOnlySizes = Wh_GetIntSetting(L"disableKbOnlySizes");
    g_settings.useIecTerms = Wh_GetIntSetting(L"useIecTerms");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    ProcessShutdownMessageBoxFix::LogErrorIfAny();

    LoadSettings();

    if (g_settings.calculateFolderSizes != CalculateFolderSizes::disabled) {
        if (!HookWindowsStorageSymbols()) {
            Wh_Log(L"Failed hooking Windows Storage symbols");
            return false;
        }
    }

    if (g_settings.calculateFolderSizes == CalculateFolderSizes::everything) {
        bool isEverything = false;
        WCHAR moduleFilePath[MAX_PATH];
        switch (GetModuleFileName(nullptr, moduleFilePath,
                                  ARRAYSIZE(moduleFilePath))) {
            case 0:
            case ARRAYSIZE(moduleFilePath):
                Wh_Log(L"GetModuleFileName failed");
                break;

            default:
                if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                    moduleFileName++;
                    if (_wcsicmp(moduleFileName, L"Everything.exe") == 0 ||
                        _wcsicmp(moduleFileName, L"Everything64.exe") == 0) {
                        isEverything = true;
                    }
                } else {
                    Wh_Log(L"GetModuleFileName returned an unsupported path");
                }
                break;
        }

        if (isEverything) {
            g_isEverything = true;

            WindhawkUtils::Wh_SetFunctionHookT(
                SHOpenFolderAndSelectItems, SHOpenFolderAndSelectItems_Hook,
                &SHOpenFolderAndSelectItems_Original);
        }
    }

    if (g_settings.disableKbOnlySizes) {
        WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplayAlloc,
                                           PSFormatForDisplayAlloc_Hook,
                                           &PSFormatForDisplayAlloc_Original);

        // Used by older file dialogs, for example Regedit's export dialog.
        WindhawkUtils::Wh_SetFunctionHookT(PSFormatForDisplay,
                                           PSFormatForDisplay_Hook,
                                           &PSFormatForDisplay_Original);
    }

    if (g_settings.useIecTerms) {
        HMODULE propsysModule = GetModuleHandle(L"propsys.dll");
        if (!propsysModule) {
            Wh_Log(L"Failed getting propsys.dll");
            return FALSE;
        }

        g_propsysModule = propsysModule;

        if (!g_settings.disableKbOnlySizes) {
            auto pPSStrFormatKBSizeW =
                (PSStrFormatKBSizeW_t)GetProcAddress(propsysModule, (PCSTR)422);
            WindhawkUtils::Wh_SetFunctionHookT(pPSStrFormatKBSizeW,
                                               PSStrFormatKBSizeW_Hook,
                                               &PSStrFormatKBSizeW_Original);
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

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    if (HANDLE thread = g_everything4Wh_Thread.exchange(nullptr)) {
        PostThreadMessage(GetThreadId(thread), WM_APP, 0, 0);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    *bReload = TRUE;
    return TRUE;
}
