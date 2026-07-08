# Audit: net-toggle.wh.cpp

> **Date:** 2026-07-08 · **Mod version:** 2.1.1 · **Lines:** 1673 · **Skill:** windhawk-master v1.0

## CRITICAL (0 findings)

No critical issues found.

---

## WARNING (3 findings)

### W-01 — Non-canonical CoUninitialize guard (C7)

**Location:** `net-toggle.wh.cpp:792, 844, 1259, 1275, 1323`
**Category:** 4 — COM Initialization | **Rule ID:** C7
**Severity:** CRITICAL in rulebook, WARNING here (functionally equivalent)

**Issue:** Five instances across `WorkerThreadProc`, `ResetWorkerThreadProc`, and `TrayThreadProc` guard `CoUninitialize` with:

```cpp
if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();
```

instead of the canonical pattern:

```cpp
if (SUCCEEDED(hrCo)) CoUninitialize();  // handles S_OK + S_FALSE
```

While **functionally correct** in this code (the `FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE` guard on lines 759–762 ensures that only `S_OK` or `S_FALSE` reach the CoUninitialize call), the non-canonical guard is **fragile**. If someone removes or modifies the early-return guard (e.g., changing it to not return on failure), the CoUninitialize could be called with `RPC_E_CHANGED_MODE` or another failure `HRESULT`, causing undefined behavior.

**Failure scenario:** A future refactor changes line 760 from `return 1;` to a softer error path. Now `CoUninitialize` could be called with `RPC_E_CHANGED_MODE` — which must not be uninitialized (MSDN).

**Fix:** Replace with `SUCCEEDED(hrCo)` at all 5 locations:

```cpp
// Before:
if (hrCo != RPC_E_CHANGED_MODE) CoUninitialize();

// After:
if (SUCCEEDED(hrCo)) CoUninitialize();
```

---

### W-02 — Cross-thread `g_shutdownEvent` read without atomic guard (T8)

**Location:** `net-toggle.wh.cpp:931`
**Category:** 5 — Threading & Concurrency | **Rule ID:** T8
**Severity:** WARNING

**Issue:** `DnsPingWorkerProc` reads `g_shutdownEvent` directly as a cross-thread variable:

```cpp
if (g_shutdownEvent && WaitForSingleObject(g_shutdownEvent, 0) == WAIT_OBJECT_0) break;
```

`g_shutdownEvent` is a `HANDLE` (pointer-sized) written by `WhTool_ModInit` on the main/Windhawk thread and read by `DnsPingWorkerProc` on a background thread. While pointer reads on x86/x64 are naturally atomic, ARM64 pointer reads _can_ tear if the pointer is not 8-byte aligned — and the compiler is free to cache or reorder a non-volatile, non-atomic read.

**Why existing guards don't fully catch it:** The `IsWindow(g_trayHwnd)` guard on the next line (937) checks the window, not the shutdown event. The shutdown event read at line 931 is used to early-exit the loop, but an incorrect value (stale null) could cause the thread to miss the shutdown signal and continue probing, delaying uninit.

**Failure scenario (ARM64, theoretical):** The compiler emits a torn read of `g_shutdownEvent` that returns a non-null value even after `WhTool_ModUninit` has closed the handle (line 1473). The worker thread then calls `WaitForSingleObject` on a closed handle, which has undefined behavior (may return immediately, may crash, may hang).

**Fix:** Use `InterlockedCompareExchangePointer` to read `g_shutdownEvent` atomically:

```cpp
HANDLE shutdownEvent = (HANDLE)InterlockedCompareExchangePointer((PVOID*)&g_shutdownEvent, nullptr, nullptr);
if (shutdownEvent && WaitForSingleObject(shutdownEvent, 0) == WAIT_OBJECT_0) break;
```

---

### W-03 — Icon destroyed before Shell_NotifyIconW call confirmed (R3)

**Location:** `net-toggle.wh.cpp:693–696, 729–749`
**Category:** 6 — Memory & Resource Cleanup | **Rule ID:** R3
**Severity:** WARNING

**Issue:** In `AddOrUpdateTrayIcon`, the _old_ icon handle (`g_currentIcon`) is destroyed _before_ the new `Shell_NotifyIconW` call is verified:

```cpp
// Line 693–696: Old icon DESTROYED first
if (g_currentIcon) {
    DestroyIcon(g_currentIcon);
    g_currentIcon = nullptr;
}

// ... (nid setup) ...

// Line 744–748: Shell_NotifyIconW called second
if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
    DestroyIcon(hNewIcon);  // New icon destroyed on failure
    return;                 // Old icon already gone — g_currentIcon is null
}

// Line 751: Only on success
g_currentIcon = hNewIcon;
```

If `Shell_NotifyIconW(NIM_MODIFY, &nid)` fails (e.g., transient system error, Explorer crash during update), the old icon handle was already destroyed, and `g_currentIcon` is left as `nullptr`. The tray notification area may still display the old icon using its own cached copy, but the mod's tracking of the icon handle is lost. On the next state update, the mod calls `AddOrUpdateTrayIcon` with `isAdd=FALSE` (since `g_trayIconInstalled=1`), attempting NIM_MODIFY with a stale `hWnd` identity while the tray may have lost the callback association.

**Failure scenario:** A transient NIM_MODIFY failure during rapid state changes (e.g., DNS status flapping between OK/DEGRADED) could cause the tray icon to stop updating until Explorer restarts or the mod reloads.

**Fix:** Swap the order — assign the new icon first, then only destroy the old icon after confirmed success:

```cpp
HICON hOldIcon = g_currentIcon;
g_currentIcon = hNewIcon;

if (isAdd) {
    if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
        g_currentIcon = hOldIcon;  // Rollback
        DestroyIcon(hNewIcon);
        return;
    }
    // ... NIM_SETVERSION ...
} else {
    if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        g_currentIcon = hOldIcon;  // Rollback
        DestroyIcon(hNewIcon);
        return;
    }
}

if (hOldIcon) DestroyIcon(hOldIcon);  // Safe: old icon no longer used
```

---

## INFO (2 findings)

### I-01 — Stylistic inconsistency: `g_lastClickTime` access pattern (T8)

**Location:** `net-toggle.wh.cpp:856, 861, 889, 894`
**Category:** 5 — Threading & Concurrency | **Rule ID:** T8 (INFO)

`g_lastClickTime` is written via `InterlockedExchange` (line 861, 894) but read via direct access (line 856, 889). All accesses are from the same thread (tray thread), so this is correct in practice — but the mixed pattern is confusing to reviewers. Either remove the `InterlockedExchange` wrappers (since writes are also same-thread) or use `InterlockedOr` for reads for consistency.

### I-02 — Settings block `$description` ordering inconsistency (S7)

**Location:** `net-toggle.wh.cpp:21–28, 36–43`
**Category:** 2 — Settings Block | **Rule ID:** S7 (INFO)

`dnsProbe` has `$description` before `$options`, while `dnsProbe2` has `$description` after `$options`. Minor formatting inconsistency within the same settings block.

---

## Summary

| Category | Verdict | Findings |
|----------|---------|----------|
| 1 — Metadata | ✅ PASS | 0 |
| 2 — Settings | ✅ PASS | 1 (INFO) |
| 3 — Boilerplate | ✅ PASS | 0 |
| 4 — COM Initialization | ⚠️ WARNING | 1 (W-01) |
| 5 — Threading & Concurrency | ⚠️ WARNING | 1 (W-02) + 1 (INFO) |
| 6 — Memory & Resource Cleanup | ⚠️ WARNING | 1 (W-03) |
| 7 — Win32 API Correctness | ✅ PASS | 0 |
| 8 — Windhawk API Usage | ✅ PASS | 0 |
| 9 — Documentation | ✅ PASS | 0 |
| 10 — Safety | ✅ PASS | 0 |
| 11 — Versioning & Changelog | ✅ PASS | 0 |

**Overall Verdict:** PASS-WITH-WARNINGS

### What's done right

- ✅ Excellent metadata: all required fields present, `@id` matches filename, valid semver, `@license MIT`, `@architecture x86 x86-64`
- ✅ Thread-safe design: `InterlockedExchange`/`InterlockedOr`/`InterlockedExchangePointer` used consistently for cross-thread state
- ✅ All `PostMessageW` calls guarded with `IsWindow()` (T16/W9)
- ✅ `WM_CLOSE` → `DestroyWindow` → `PostQuitMessage` shutdown chain (B3/T5)
- ✅ `GetModuleFileNameW` truncation check with `case 0:` / `case ARRAYSIZE:` (B5)
- ✅ `ExitProcess(0)` at end of `Wh_ModUninit` (B4)
- ✅ `TaskbarCreated` message handler (W14)
- ✅ `NIF_SHOWTIP` in tray flags (W13)
- ✅ AUMID via `SHGetPropertyStoreForWindow` (W12)
- ✅ `NIF_GUID` with dedicated static GUID (W3)
- ✅ `NIM_SETVERSION(NOTIFYICON_VERSION_4)` (W2)
- ✅ RAII `StringSetting` for all setting reads (A7/R1)
- ✅ Worker shutdown via `SetEvent(g_shutdownEvent)` + `WaitForMultipleObjects` with timeout (T15)
- ✅ Delta-only changelog (V3)
- ✅ Readme with allowlisted image URLs from `i.imgur.com` (D2)
- ✅ Thread handle management with `InterlockedExchangePointer` + `CloseHandle` (T9)

### 3 WARNING findings

All three WARNING issues are subtle and unlikely to manifest on x86/x64 under normal conditions:

1. **W-01** is a canonical-pattern deviation that's functionally correct today but fragile
2. **W-02** is a theoretical ARM64 tearing risk in an edge-case read path
3. **W-03** is a resource-management ordering issue that recovers on the next icon update

**Recommendation:** Fix W-01 and W-03 before PR submission (straightforward one-line changes). W-02 is acceptable as-is for x86-64 targets but should be noted for ARM64.
