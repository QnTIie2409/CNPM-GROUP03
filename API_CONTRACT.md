# Backend API Contract

This contract describes the C++ service and repository APIs used by the Qt UI.
The backend owns validation, authorization and database writes. UI code should
call these methods and display the returned `ServiceResult` or response data.

## Attendance sessions and rotating codes

### `SessionService::openSession`

```cpp
ServiceResult openSession(int courseId,
                          int lecturerUserId,
                          int durationMin = 80,
                          int presentMin = 20,
                          int lateMin = 80);
```

- Requires an active Lecturer who teaches `courseId`.
- Requires `0 < presentMin <= lateMin <= durationMin`.
- Rejects a second non-expired open session for the same course.
- Creates the session using the MySQL server clock.
- Generates a six digit code and stores its generation time.
- Returns the new `session_id` in `ServiceResult::id`.
- Returns the initial code in `ServiceResult::message`.
- A background timer rotates every open session code after 60 seconds.
- The background timer closes sessions when `durationMin` has elapsed and
  creates `ABSENT` records for enrolled Students who did not check in.

### `SessionService::getOpenSession`

```cpp
std::optional<AttendanceSession> getOpenSession(int courseId);
```

- Returns the current open session for the course.
- Automatically closes and returns empty for a session whose duration has
  expired, using the MySQL server clock.
- `AttendanceSession::sessionCode` is the current code to display to the
  Lecturer or validate through `CheckInService`.

### `SessionService::getSessionById`

```cpp
std::optional<AttendanceSession> getSessionById(int sessionId);
```

Returns the session snapshot, including status, windows, current code and
server-recorded timestamps.

### `SessionService::closeSession`

```cpp
ServiceResult closeSession(int sessionId);
```

- Closes an open session once.
- Runs the status update and automatic `ABSENT` insertion in one transaction.
- Returns the number of automatically absent Students in the message.
- Emits `sessionClosed(sessionId)` after a successful commit.

### Signals

```cpp
void codeRotated(int sessionId, const QString &newCode);
void sessionClosed(int sessionId);
```

The Lecturer UI should subscribe to `codeRotated` and update the displayed
code only when the signal's `sessionId` matches its live session.

### `SessionService::generateRandomCode`

```cpp
static QString generateRandomCode();
```

Returns a six digit numeric code. The UI must not generate or persist session
codes itself.

## Session repository support

The UI does not call repositories directly. `SessionRepository` provides the
following service support methods:

```cpp
std::vector<AttendanceSession> findOpenSessions();
bool lecturerTeachesCourse(int lecturerUserId, int courseId);
qint64 serverSecondsSince(const QDateTime &startTime);
```

`serverSecondsSince` uses `NOW()` in MySQL so expiration and code rotation do
not depend on a user's computer clock.

## Check-in integration

```cpp
CheckInResponse CheckInService::submitCheckIn(
    int sessionId, int studentUserId, const QString &enteredCode);
```

The Student UI sends the entered code to this method. It must not compare the
code locally. The service checks session status, enrollment, duplicate records,
the current code and the Present/Late windows.
