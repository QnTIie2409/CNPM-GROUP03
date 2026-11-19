# Session backend and Live session integration

## 2026-11-19: Open session and code rotation

Existing method signatures remain unchanged:

```cpp
ServiceResult SessionService::openSession(int courseId, int lecturerUserId,
    int durationMin = 80, int presentMin = 20, int lateMin = 80);
std::optional<AttendanceSession> SessionService::getOpenSession(int courseId);
std::optional<AttendanceSession> SessionService::getSessionById(int sessionId);
ServiceResult SessionService::closeSession(int sessionId);
```

`openSession` returns the new session ID in `ServiceResult.id`. Store this ID
and use it to load Live session. The initial code in the success message is a
snapshot; it does not change when the code rotates.

Signals (same app process only):

```cpp
void sessionOpened(int sessionId); // NEW: emitted after successful insert
void codeRotated(int sessionId, const QString &newCode);
void sessionClosed(int sessionId);
```

The backend monitor starts when `SessionService::instance()` is first called.
It continues polling open sessions every second, including sessions already
in MySQL before app startup. Closing one session does not stop other sessions.
Time calculations use MySQL NOW(), not the computer clock. SQL only rotates
OPEN, unexpired sessions whose previous code is at least 60 seconds old.
Concurrent rotation attempts from other app processes affect zero rows after
the winning update. Every successfully generated new code differs from the
previous code.

## UI handoff: why Live session stays empty

Observed in `ui/UiLauncher.cpp`: all pages are constructed at login.
`lecturerLiveSessionPage` queries once and returns an empty page when no session
exists. Switching the stacked page after `openSession` does not rebuild it.
The early return also skips connecting `codeRotated`.

The UI owner should implement these changes:

1. On successful `openSession`, pass `result.id` into the Live session view.
2. Load/reload the live view on navigation or `sessionOpened`. The empty state
   must also subscribe to session changes; do not leave a stale empty widget.
3. Subscribe to `codeRotated` and update only the matching session ID.
4. Poll `getSessionById(sessionId)` while the live page is visible (for example
   every 1-2 seconds). Qt signals do not cross processes or Radmin VPN; polling
   is needed for changes made by a different app instance.
5. Refresh the roster independently of code rotation, so check-ins appear
   promptly. Clear the code and disable actions when status becomes CLOSED.
6. Handle `sessionClosed` locally and verify status through polling remotely.
7. On failed reads show an error/retry state, not a claim that a session closed.

No files under `ui/` were edited in this backend change. UI handoff is required
to fix the stale Live session screen completely.

## Backend regression test

```powershell
cmake -S database/tests -B build/session-tests -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64
cmake --build build/session-tests
.\build\session-tests\session_lifecycle.exe
```

Run from the repo root with Qt/MySQL runtime DLLs on PATH, and MySQL running.
Tests use the config.ini connection and create TEMPORARY tables that shadow
real tables only in the test connection. They do not edit application rows.
They verify timer rotation, persisted codes, closed/early rotation rejection,
multi-session monitoring, automatic expiry and Absent insertion. Timestamps in
temporary fixtures are advanced to exercise 60-second/5-minute boundaries
without long waits.
