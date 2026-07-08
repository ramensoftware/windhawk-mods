# Audit: net-toggle.wh.cpp

> **Date:** 2026-07-08 · **Mod version:** 2.1.1 · **Lines:** 1678 · **Skill:** windhawk-master v1.0
> **Catalog version:** 2.1.0 (public) · **Delta:** patch bump (2.1.0 → 2.1.1) ✓

---

## CRITICAL (0 findings)

No critical issues found.

---

## HIGH (0 findings)

No high-severity issues found.

---

## WARNING (1 finding)

### W-01 — Non-atomic read of `g_shutdownEvent` in `WaitForMultipleObjects` call

**Location:** `net-toggle.wh.cpp:1206`
**Category:** `5 — Threading & Concurrency` | **Rule ID:** `T8`

**Issue:**
```cpp
HANDLE waits[2] = { hEvent, g_shutdownEvent };
DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
```
`g_shutdownEvent` is read directly (inheriting a stale or torn pointer on ARM64) instead of being read atomically via `InterlockedCompareExchangePointer`. While `WhTool_ModUninit` signals the event before clearing the handle (creating a happens-before), there is a theoretical ARM64 tearing window if `WhTool_ModUninit` clears `g_shutdownEvent` concurrently with this read. A torn pointer could produce an invalid HANDLE value, causing `WaitForMultipleObjects` to crash with `WAIT_FAILED` / `ERROR_INVALID_HANDLE`.

**Severity rationale:** WARNING — extremely low probability on x86-64 (naturally atomic pointer reads), theoretical on ARM64. The shutdown event is signaled BEFORE `g_shutdownEvent` is cleared, so in practice the thread exits via the event signal before the handle is cleared. This is a defensive hardening issue.

**Fix:** Read atomically:
```cpp
HANDLE hShutdown = (HANDLE)InterlockedCompareExchangePointer((PVOID*)&g_shutdownEvent, nullptr, nullptr);
HANDLE waits[2] = { hEvent, hShutdown };
```

---

## INFO (2 findings)

### I-01 — Redundant `volatile` on several globals

**Location:** `net-toggle.wh.cpp:190,192,197,198,199,201`
**Category:** `5 — Threading & Concurrency` | **Rule ID:** `T8`

Several globals are declared `volatile` but are only accessed through `Interlocked*` functions, which already provide the necessary memory ordering and compiler barriers:
- `g_trayHwnd` (line 190) — written via `InterlockedExchangePointer`, read directly (volatile prevents register caching)
- `g_lastClickTime` (line 192) — written via `InterlockedExchange`, **read directly** (this is the only actual risk; see I-02)
- `g_dnsIp[2]`, `g_dnsProbe[2]`, `g_dnsUp[2]` (lines 197-199) — read via `InterlockedOr`, written via `InterlockedExchange`
- `g_dnsWorkerRunning` (line 201) — read/written via `InterlockedCompareExchange` / `InterlockedExchange`

The `volatile` qualifier is redundant with `Interlocked*` access but not harmful. The changelog reports that `volatile` was removed from `g_isProcessingClick`, `g_trayIconInstalled`, and `g_networkIsUp` — these correctly use `Interlocked*` without `volatile`. The remaining `volatile` variables could be cleaned up similarly for consistency.

### I-02 — `g_lastClickTime` read non-atomically

**Location:** `net-toggle.wh.cpp:859,892`
**Category:** `5 — Threading & Concurrency` | **Rule ID:** `T8`

`g_lastClickTime` is written atomically via `InterlockedExchange` (lines 864, 897) but read directly (lines 859, 892):
```cpp
DWORD now = GetTickCount();
if (now - g_lastClickTime < CLICK_DEBOUNCE_MS) {  // direct read, not atomic
```

On ARM64, a 32-bit read of a `volatile DWORD` is atomic if naturally aligned, but the pattern is inconsistent with the `InterlockedExchange` writes. For consistency and clarity, reads should also use `InterlockedExchange` (read-with-write) or `InterlockedCompareExchange` (read-only):
```cpp
if (now - (DWORD)InterlockedExchange(&g_lastClickTime, 0) < CLICK_DEBOUNCE_MS) {
```

**Severity rationale:** INFO — on x86-64 and ARM64, a properly-aligned 32-bit read is naturally atomic. The inconsistency is stylistic rather than a correctness risk.

---

## Previous Warnings — Status Check

| ID | Issue | Status |
|----|-------|--------|
| **W-01** | `CoUninitialize` guard used `hrCo != RPC_E_CHANGED_MODE` instead of `SUCCEEDED(hrCo)` | **✅ FIXED** — All 3 COM-using threads (Worker, ResetWorker, Tray) now correctly use `SUCCEEDED(hrCo)` (lines 795, 847, 1328) |
| **W-02** | `g_shutdownEvent` read directly instead of via `InterlockedCompareExchangePointer` | **✅ FIXED** — `DnsPingWorkerProc` (line 934) and `NetWatchThreadProc` polling loop (line 1186) now read atomically. Only the `WaitForMultipleObjects` call at line 1206 remains (new W-01). |
| **W-03** | Old icon destroyed before NIM_MODIFY (leaked on failure, dangling on success) | **✅ FIXED** — `AddOrUpdateTrayIcon` correctly saves old icon, restores it on NIM_MODIFY failure (line 748), destroys on success (line 751) |

---

## Summary

| Category | Verdict | Findings |
|----------|---------|----------|
| 1 — Metadata | ✅ PASS | 0 |
| 2 — Settings | ✅ PASS | 0 |
| 3 — Boilerplate | ✅ PASS | 0 |
| 4 — COM Initialization | ✅ PASS | 0 |
| 5 — Threading & Concurrency | ⚠️ WARNING | 1 (W-01) + 2 INFO |
| 6 — Memory & Resource Cleanup | ✅ PASS | 0 |
| 7 — Win32 API Correctness | ✅ PASS | 0 |
| 8 — Windhawk API Usage | ✅ PASS | 0 |
| 9 — Documentation | ✅ PASS | 0 |
| 10 — Safety | ✅ PASS | 0 |
| 11 — Versioning & Changelog | ✅ PASS | 0 |

**Overall Verdict:** ✅ PASS-WITH-WARNINGS
(1 WARNING: non-atomic g_shutdownEvent read in WaitForMultipleObjects)

**The 3 previous WARNING findings from the prior audit are all confirmed fixed.** One new WARNING and two INFO items were identified in this pass. None are CRITICAL or HIGH severity. The mod is in strong shape for PR submission.
