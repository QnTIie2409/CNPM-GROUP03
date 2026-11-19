#include "src/services/SessionService.h"
#include "src/repositories/DatabaseManager.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <cstdlib>
#include <cstdio>

static void require(bool condition, const char *message) {
    std::printf("%s: %s\n", condition ? "PASS" : "FAIL", message);
    std::fflush(stdout);
    if (!condition) std::exit(1);
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    require(DatabaseManager::instance().initFromConfig(), "MySQL connection");
    auto db = DatabaseManager::instance().getDatabase();
    auto sql = [&](const QString &statement) {
        QSqlQuery q(db);
        if (!q.exec(statement)) {
            std::fprintf(stderr, "%s\n", qPrintable(q.lastError().text()));
            std::exit(1);
        }
    };
    // Temporary tables shadow the real tables only on this connection.
    // They disappear on disconnect; no application rows are modified.
    for (const auto *table : {"users", "courses", "enrollments",
                             "attendance_sessions", "attendance_records"}) {
        sql(QString("CREATE TEMPORARY TABLE fixture_%1 LIKE %1").arg(table));
        sql(QString("ALTER TABLE fixture_%1 RENAME TO %1").arg(table));
    }
    sql("INSERT INTO users (user_id,role,full_name,email,password_hash,is_active) "
        "VALUES (1,'LECTURER','Teacher','teacher@test.invalid','unused',1),"
        "(2,'STUDENT','Student','student@test.invalid','unused',1)");
    sql("INSERT INTO courses (course_id,code,name,user_id) VALUES "
        "(1,'TEST1','Test one',1),(2,'TEST2','Test two',1)");
    sql("INSERT INTO enrollments (user_id,course_id,enrolled_at) "
        "VALUES (2,1,CURDATE()),(2,2,CURDATE())");

    auto &service = SessionService::instance();
    int openedCount = 0, rotations = 0;
    QObject::connect(&service, &SessionService::sessionOpened, &app,
                     [&](int) { ++openedCount; });
    QObject::connect(&service, &SessionService::codeRotated, &app,
                     [&](int, const QString &) { ++rotations; });
    const auto first = service.openSession(1, 1, 5, 5, 5);
    const auto second = service.openSession(2, 1, 5, 5, 5);
    require(first.ok && second.ok && openedCount == 2, "open two sessions and notify UI");
    require(!service.openSession(1, 1, 5, 5, 5).ok, "reject duplicate open session");
    const auto oldCode = service.getSessionById(second.id)->sessionCode;
    sql(QString("UPDATE attendance_sessions SET code_generated_at = "
                "DATE_SUB(NOW(), INTERVAL 61 SECOND) WHERE session_id = %1").arg(second.id));
    require(service.closeSession(first.id).ok, "close first session");
    QEventLoop loop;
    QTimer::singleShot(2200, &loop, &QEventLoop::quit);
    loop.exec();
    require(rotations == 1, "other session still rotates via real QTimer after first closes");
    require(service.getSessionById(second.id)->sessionCode != oldCode,
            "new code differs and is persisted");
    SessionRepository repo;
    require(!repo.updateSessionCode(second.id, "123456"), "second client cannot rotate early");
    require(!repo.updateSessionCode(first.id, "123456"), "closed session code cannot rotate");
    sql(QString("UPDATE attendance_sessions SET start_time = "
                "DATE_SUB(NOW(), INTERVAL 6 MINUTE) WHERE session_id = %1").arg(second.id));
    QTimer::singleShot(2200, &loop, &QEventLoop::quit);
    loop.exec();
    require(service.getSessionById(second.id)->status == SessionStatus::CLOSED,
            "timer closes expired session");
    QSqlQuery count(db);
    require(count.exec("SELECT COUNT(*) FROM attendance_records WHERE status = 'ABSENT'") &&
            count.next() && count.value(0).toInt() == 2, "unchecked students marked Absent");
    const auto third = service.openSession(1, 1, 5, 5, 5);
    require(third.ok, "monitor remains available after all sessions closed");
    sql(QString("UPDATE attendance_sessions SET code_generated_at = "
                "DATE_SUB(NOW(), INTERVAL 61 SECOND) WHERE session_id = %1").arg(third.id));
    QTimer::singleShot(2200, &loop, &QEventLoop::quit);
    loop.exec();
    require(rotations == 2, "new session rotates without restarting app");
    return 0;
}
