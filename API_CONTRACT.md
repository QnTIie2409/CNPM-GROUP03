# Session backend and Live session integration

## Password change

```cpp
bool AuthService::changePassword(const QString &currentPassword,
                                 const QString &newPassword);
```

The logged-in user must provide the current password. The service requires a
new password of at least six characters, rejects an unchanged password,
creates a new salted PBKDF2 hash, updates the active user's row and refreshes
the in-memory user hash. The UI should show `AuthService::lastError()` when
the operation fails.

## Excuse requests

The current Requirement Document excludes student excuse requests. FR-08
allows a Lecturer to set `EXCUSED` directly while correcting attendance. The
existing Student excuse dialog is therefore informational only and must not
claim that a request was submitted or notify a Lecturer. A real request flow
requires a new FR, a request table and repository/service, request status and
reason fields, Lecturer notification/list UI, approval/rejection rules and
new test cases.

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

## Additional UI findings from source review (2026-11-19)

- `lecturerOverviewPage` renders the first open session's code once. Its
  `lblCodeNow` is not subscribed to code changes. Refresh this label too.
- `lecturerOpenSessionPage` captures the local `std::function`
  `checkCourseOpenSession` by reference in stored callbacks. That variable is
  destroyed when the factory returns. Calling it on a later click/course
  change is undefined behavior. Move refresh logic into a page QObject slot
  (preferred); do not capture local function variables by reference.
- The same lifetime bug occurs with `refreshSessionData` in
  `lecturerReportPage` and `refreshAccounts` in account management. Audit all
  stored callbacks using `&refresh...` and replace them with page-owned slots.
- Report session choices are fetched at construction and course changes only.
  Re-fetch on page entry and sessionOpened/sessionClosed; preserve the selected
  session ID and select the newly opened/closed session when navigating from
  Live session. The repository already returns both OPEN and CLOSED sessions.
- Closing a live session currently navigates to an existing report widget;
  navigation alone does not reload either page. On success disable the old
  session controls and refresh Live, Open session, Overview and Report.
- Poll visible live data every 1-2 seconds for other app instances. Refresh
  code/status/roster independently; keep UI changes on the GUI thread and
  attach timers/connections to the page lifetime.

Observed local DB evidence: session #23 code generation timestamp advanced
from 21:27:28 to 21:28:28 and its code changed. The screen retaining the old
code is therefore a UI synchronization problem in this reproduction.
The row was still OPEN when inspected; the close action's actual session ID
and return value need to be checked rather than assuming a successful close.

## Lecturer assignment data inconsistency

Current seed assigns all five courses to lecturer user_id=2 (Tran Minh Quan).
Lecturer user_id=3 (Nguyen Thu Ha) consequently has no course timetable.
Nine historical/sample sessions belong to user_id=3 on courses currently
assigned to user_id=2. This may represent reassignment, but seed needs an
explicit consistent teaching allocation for testing. Confirm the intended
course allocation before changing current lecturer ownership; do not reset
seed or rewrite attendance history to hide this mismatch.

## Manual acceptance sequence for UI owner

1. Login Lecturer, open a course session and store the returned session ID.
2. Navigate to Live: same ID/code as MySQL, no stale empty state.
3. Wait a full 60-second code interval: code changes in MySQL and on screen.
4. Student checks in from a second instance: roster refreshes within polling
   interval, independently of code rotation.
5. Close the displayed ID: inspect ServiceResult; on success MySQL is CLOSED,
   old code is hidden and controls are disabled.
6. Navigate to Report: the same session appears as CLOSED with final records.
7. Navigate to Open session: course is unblocked; create a different session
   ID and repeat without logout/restart.

## Running the backend regression test

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
