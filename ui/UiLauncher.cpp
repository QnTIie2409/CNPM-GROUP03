#include "UiLauncher.h"
#include "LoginWindow.h"
#include "shell/AppShell.h"
#include "MiniCalendarWidget.h"
#include "WeeklyTimetableWidget.h"
#include "../src/services/AuthService.h"
#include "../src/repositories/CourseRepository.h"
#include "../src/repositories/SessionRepository.h"
#include "../src/repositories/UserRepository.h"
#include "../src/services/AccountService.h"
#include "../src/services/CourseService.h"
#include "../src/services/CheckInService.h"
#include "../src/services/SessionService.h"
#include "../src/services/TimetableService.h"
#include "../src/services/AttendanceService.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateEdit>
#include <QTimeEdit>
#include <QCalendarWidget>
#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QStringList>

UiLauncher::UiLauncher(QObject *parent) : QObject(parent) {}

namespace {


QWidget* createDayScheduleCard(const std::vector<ScheduleEntry> &entries, const QDate &date) {
    auto *card = new QFrame;
    card->setObjectName("overviewCard");
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    const QString dateStr = date.toString("dddd, dd/MM/yyyy");
    auto *title = new QLabel(QString("📋 Lịch ngày: %1").arg(dateStr));
    title->setObjectName("overviewCardTitle");
    layout->addWidget(title);

    const int dow = date.dayOfWeek();
    int count = 0;
    for (const auto &entry : entries) {
        if (entry.dayOfWeek == dow && date >= entry.startDate && date <= entry.endDate) {
            auto *item = new QFrame;
            item->setObjectName("scheduleItemCard");
            auto *itemLayout = new QVBoxLayout(item);
            itemLayout->setContentsMargins(12, 10, 12, 10);
            itemLayout->setSpacing(4);

            auto *rowTop = new QHBoxLayout;
            auto *cName = new QLabel(entry.courseName);
            cName->setStyleSheet("font-weight: bold; font-size: 13px; color: #112D55; background: transparent;");
            rowTop->addWidget(cName);
            rowTop->addStretch();
            auto *cCode = new QLabel(entry.courseCode);
            cCode->setStyleSheet("font-size: 11px; color: #64748B; background: transparent;");
            rowTop->addWidget(cCode);
            itemLayout->addLayout(rowTop);

            auto *rowBottom = new QHBoxLayout;
            auto *cTime = new QLabel(QString("⏰ %1 - %2")
                .arg(entry.startTime.toString("HH:mm"))
                .arg(entry.endTime.toString("HH:mm")));
            cTime->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            rowBottom->addWidget(cTime);

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(entry.room.isEmpty() ? "Chưa xếp phòng" : entry.room));
            cRoom->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            rowBottom->addWidget(cRoom);
            rowBottom->addStretch();
            itemLayout->addLayout(rowBottom);

            layout->addWidget(item);
            ++count;
        }
    }

    if (count == 0) {
        auto *emptyLbl = new QLabel("Không có ca học nào trong ngày này.");
        emptyLbl->setStyleSheet("color: #64748B; font-style: italic; padding: 12px 0; background: transparent;");
        layout->addWidget(emptyLbl);
    }

    return card;
}

QWidget *lecturerOverviewPage(int userId) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    const auto currentUser = AuthService::instance().currentUser();
    const QString lecturerName = currentUser.has_value() ? currentUser->fullName : "Giảng viên";

    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel(QString("Xin chào, %1 👨‍🏫").arg(lecturerName));
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel(QString("Hôm nay là %1 | Học kỳ 1 (2026 - 2027)")
        .arg(QDate::currentDate().toString("dddd, dd/MM/yyyy")));
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(24);
    contentRow->setAlignment(Qt::AlignTop);

    // LEFT COLUMN (Main: Today's Schedule + Active Sessions)
    auto *leftCol = new QVBoxLayout;
    leftCol->setSpacing(18);

    const auto lecturerEntries = TimetableService::instance().getLecturerTimetable(userId);
    const QDate today = QDate::currentDate();

    // 1. Ca dạy hôm nay card
    auto *todayCard = new QFrame;
    todayCard->setObjectName("overviewCard");
    auto *todayLayout = new QVBoxLayout(todayCard);
    todayLayout->setContentsMargins(18, 16, 18, 16);
    todayLayout->setSpacing(12);

    auto *todayCardTitle = new QLabel("📅 Lịch dạy hôm nay");
    todayCardTitle->setObjectName("overviewCardTitle");
    todayLayout->addWidget(todayCardTitle);

    int todayCount = 0;
    const int todayDow = today.dayOfWeek();
    for (const auto &entry : lecturerEntries) {
        if (entry.dayOfWeek == todayDow && today >= entry.startDate && today <= entry.endDate) {
            auto *item = new QFrame;
            item->setObjectName("scheduleItemCard");
            auto *itemLayout = new QVBoxLayout(item);
            itemLayout->setContentsMargins(12, 10, 12, 10);
            itemLayout->setSpacing(4);

            auto *r1 = new QHBoxLayout;
            auto *cName = new QLabel(entry.courseName);
            cName->setStyleSheet("font-weight: 700; font-size: 14px; color: #112D55; background: transparent;");
            r1->addWidget(cName);
            r1->addStretch();
            auto *cCode = new QLabel(entry.courseCode);
            cCode->setStyleSheet("font-size: 12px; color: #64748B; background: transparent;");
            r1->addWidget(cCode);
            itemLayout->addLayout(r1);

            auto *r2 = new QHBoxLayout;
            auto *cTime = new QLabel(QString("⏰ %1 - %2")
                .arg(entry.startTime.toString("HH:mm"))
                .arg(entry.endTime.toString("HH:mm")));
            cTime->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            r2->addWidget(cTime);

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(entry.room.isEmpty() ? "Chưa xếp phòng" : entry.room));
            cRoom->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            r2->addWidget(cRoom);
            r2->addStretch();
            itemLayout->addLayout(r2);

            todayLayout->addWidget(item);
            todayCount++;
        }
    }
    if (todayCount == 0) {
        auto *emptyLbl = new QLabel("Hôm nay bạn không có lịch dạy nào trên thời khóa biểu.");
        emptyLbl->setStyleSheet("color: #64748B; font-style: italic; padding: 12px 0; background: transparent;");
        todayLayout->addWidget(emptyLbl);
    }
    leftCol->addWidget(todayCard);

    // 2. Trạng thái các phiên điểm danh đang mở
    auto *sessionCard = new QFrame;
    sessionCard->setObjectName("overviewCard");
    auto *sessionLayout = new QVBoxLayout(sessionCard);
    sessionLayout->setContentsMargins(18, 16, 18, 16);
    sessionLayout->setSpacing(12);

    auto *sessionTitle = new QLabel("⚡ Phiên điểm danh đang mở");
    sessionTitle->setObjectName("overviewCardTitle");
    sessionLayout->addWidget(sessionTitle);

    CourseRepository courses;
    const auto taught = courses.findByLecturer(userId);
    int openCount = 0;
    for (const auto &course : taught) {
        auto open = SessionService::instance().getOpenSession(course.courseId);
        if (open.has_value()) {
            openCount++;
            auto *sBox = new QFrame;
            sBox->setStyleSheet("background-color: #F8FAFC; border: 1px solid #CBD5E1; border-radius: 8px; padding: 12px;");
            auto *sLayout = new QVBoxLayout(sBox);
            sLayout->setSpacing(6);

            auto *cTitle = new QLabel(QString("Môn: %1 - %2").arg(course.code, course.name));
            cTitle->setStyleSheet("font-weight: bold; color: #112D55; font-size: 13px; background: transparent;");
            sLayout->addWidget(cTitle);

            auto *codeLbl = new QLabel(QString("Mã điểm danh hiện tại: %1 (tự đổi sau mỗi 60s)").arg(open->sessionCode));
            codeLbl->setStyleSheet("color: #0D7377; font-weight: bold; font-size: 14px; background: transparent;");
            sLayout->addWidget(codeLbl);

            sessionLayout->addWidget(sBox);
        }
    }
    if (openCount == 0) {
        auto *noOpen = new QLabel("Chưa có phiên điểm danh nào đang mở. Chuyển sang mục 'Open session' để mở phiên điểm danh mới.");
        noOpen->setStyleSheet("color: #64748B; font-style: italic; padding: 8px 0; background: transparent;");
        sessionLayout->addWidget(noOpen);
    }

    leftCol->addWidget(sessionCard);
    leftCol->addStretch();
    contentRow->addLayout(leftCol, 3);

    // RIGHT COLUMN (Mini Calendar + Selected Date Schedule)
    auto *rightCol = new QVBoxLayout;
    rightCol->setSpacing(18);

    auto *miniCal = new MiniCalendarWidget;
    miniCal->setScheduleEntries(lecturerEntries);
    rightCol->addWidget(miniCal);

    auto *selectedDateContainer = new QWidget;
    auto *selectedDateLayout = new QVBoxLayout(selectedDateContainer);
    selectedDateLayout->setContentsMargins(0, 0, 0, 0);

    auto updateSelectedDateView = [selectedDateLayout, lecturerEntries](const QDate &d) {
        while (auto *child = selectedDateLayout->takeAt(0)) {
            if (child->widget()) delete child->widget();
            delete child;
        }
        selectedDateLayout->addWidget(createDayScheduleCard(lecturerEntries, d));
    };

    updateSelectedDateView(today);
    QObject::connect(miniCal, &MiniCalendarWidget::dateSelected, page, updateSelectedDateView);

    rightCol->addWidget(selectedDateContainer);
    rightCol->addStretch();
    contentRow->addLayout(rightCol, 2);

    pageLayout->addLayout(contentRow, 1);
    return page;
}

QWidget *lecturerOpenSessionPage(int userId) {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);

    auto *title = new QLabel("Open attendance session");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    CourseRepository courses;
    const auto taught = courses.findByLecturer(userId);
    auto *courseBox = new QComboBox;
    for (const auto &course : taught)
        courseBox->addItem(course.code + " - " + course.name, course.courseId);
    layout->addWidget(new QLabel("Course"));
    layout->addWidget(courseBox);

    auto *duration = new QSpinBox;
    duration->setRange(1, 600);
    duration->setValue(80);
    auto *present = new QSpinBox;
    present->setRange(1, 600);
    present->setValue(20);
    auto *late = new QSpinBox;
    late->setRange(1, 600);
    late->setValue(80);

    auto addField = [layout](const QString &label, QSpinBox *spin) {
        layout->addWidget(new QLabel(label));
        layout->addWidget(spin);
    };
    addField("Duration (minutes)", duration);
    addField("Present window (minutes)", present);
    addField("Late window end (minutes)", late);

    auto *openButton = new QPushButton("Open session");
    auto *closeButton = new QPushButton("Close session");
    closeButton->setEnabled(false);
    auto *status = new QLabel;
    status->setWordWrap(true);
    auto *codeLabel = new QLabel("Current code: -");
    codeLabel->setObjectName("lblHeading");
    layout->addWidget(openButton);
    layout->addWidget(codeLabel);
    layout->addWidget(closeButton);
    layout->addWidget(status);
    layout->addStretch();

    QObject::connect(openButton, &QPushButton::clicked, page,
        [page, userId, courseBox, duration, present, late, openButton,
         closeButton, codeLabel, status] {
            const auto result = SessionService::instance().openSession(
                courseBox->currentData().toInt(), userId, duration->value(),
                present->value(), late->value());
            status->setText(result.message);
            if (!result.ok) return;

            openButton->setEnabled(false);
            closeButton->setEnabled(true);
            const auto session = SessionService::instance().getSessionById(result.id);
            if (session.has_value())
                codeLabel->setText("Current code: " + session->sessionCode);

            QObject::connect(&SessionService::instance(), &SessionService::codeRotated,
                             page, [result, codeLabel](int sessionId, const QString &code) {
                if (sessionId == result.id)
                    codeLabel->setText("Current code: " + code);
            });
        });

    QObject::connect(closeButton, &QPushButton::clicked, page,
        [courseBox, openButton, closeButton, codeLabel, status] {
            const auto session = SessionService::instance().getOpenSession(
                courseBox->currentData().toInt());
            if (!session.has_value()) {
                status->setText("No open session found.");
                return;
            }
            const auto result = SessionService::instance().closeSession(session->sessionId);
            status->setText(result.message);
            if (result.ok) {
                openButton->setEnabled(true);
                closeButton->setEnabled(false);
                codeLabel->setText("Current code: -");
            }
        });
    return page;
}

QWidget *lecturerReportPage(int userId) {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(12);
    auto *title = new QLabel("Attendance report");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    CourseRepository courses;
    SessionRepository sessions;
    const auto taught = courses.findByLecturer(userId);
    for (const auto &course : taught) {
        auto *box = new QGroupBox(course.code + " - " + course.name);
        auto *boxLayout = new QVBoxLayout(box);
        const auto courseSessions = sessions.findByCourse(course.courseId);
        for (const auto &session : courseSessions) {
            const auto records = AttendanceService::instance().getSessionRecords(session.sessionId);
            boxLayout->addWidget(new QLabel(QString("Session %1 (%2): %3 records")
                .arg(session.sessionId)
                .arg(session.startTime.toString("yyyy-MM-dd HH:mm"))
                .arg(records.size())));
            auto *table = new QTableWidget(records.size(), 5);
            table->setHorizontalHeaderLabels({"Student", "Status", "Check-in time", "New status", "Action"});
            table->horizontalHeader()->setStretchLastSection(true);
            table->setEditTriggers(QAbstractItemView::NoEditTriggers);
            for (int i = 0; i < static_cast<int>(records.size()); ++i) {
                const auto record = records[i];
                table->setItem(i, 0, new QTableWidgetItem(record.studentName));
                table->setItem(i, 1, new QTableWidgetItem(
                    AttendanceRecord::statusToString(record.status)));
                table->setItem(i, 2, new QTableWidgetItem(
                    record.checkInTime.isValid()
                        ? record.checkInTime.toString("yyyy-MM-dd HH:mm")
                        : "-"));
                auto *statusBox = new QComboBox;
                statusBox->addItems({"PRESENT", "LATE", "ABSENT", "EXCUSED"});
                statusBox->setCurrentText(AttendanceRecord::statusToString(record.status));
                auto *save = new QPushButton("Save");
                table->setCellWidget(i, 3, statusBox);
                table->setCellWidget(i, 4, save);
                QObject::connect(save, &QPushButton::clicked, page,
                    [table, save, statusBox, record, userId] {
                        const auto newStatus = AttendanceRecord::stringToStatus(statusBox->currentText());
                        if (AttendanceService::instance().editRecordStatus(
                                record.recordId, newStatus, userId, "Updated from lecturer report")) {
                            const int row = table->indexAt(save->pos()).row();
                            if (row >= 0)
                                table->item(row, 1)->setText(statusBox->currentText());
                            save->setText("Saved");
                            save->setEnabled(false);
                        }
                    });
            }
            boxLayout->addWidget(table);
        }
        if (courseSessions.empty())
            boxLayout->addWidget(new QLabel("No attendance sessions yet."));
        layout->addWidget(box);
    }
    layout->addStretch();
    return page;
}


QWidget *weeklyTimetablePage(const std::vector<ScheduleEntry> &entries,
                             const QString &heading) {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(16);

    auto *title = new QLabel(heading);
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    auto *weeklyWidget = new WeeklyTimetableWidget;
    weeklyWidget->setScheduleEntries(entries);
    layout->addWidget(weeklyWidget, 1);

    return page;
}

QWidget *lecturerTimetablePage(int userId) {
    return weeklyTimetablePage(
        TimetableService::instance().getLecturerTimetable(userId),
        "Thời khóa biểu giảng dạy");
}

QWidget *adminAccountsPage() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    auto *title = new QLabel("Account management");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    auto *form = new QGroupBox("Create Student or Lecturer");
    auto *formLayout = new QVBoxLayout(form);
    auto *role = new QComboBox; role->addItems({"STUDENT", "LECTURER"});
    auto *code = new QLineEdit; code->setPlaceholderText("Student/Lecturer code");
    auto *name = new QLineEdit; name->setPlaceholderText("Full name");
    auto *email = new QLineEdit; email->setPlaceholderText("Email");
    auto *password = new QLineEdit; password->setPlaceholderText("Initial password");
    auto *create = new QPushButton("Create account");
    auto *result = new QLabel; result->setWordWrap(true);
    formLayout->addWidget(role); formLayout->addWidget(code); formLayout->addWidget(name);
    formLayout->addWidget(email); formLayout->addWidget(password);
    formLayout->addWidget(create); formLayout->addWidget(result);
    layout->addWidget(form);

    auto *list = new QTableWidget;
    list->setColumnCount(5);
    list->setHorizontalHeaderLabels({"ID", "Role", "Code", "Name", "Active"});
    list->horizontalHeader()->setStretchLastSection(true);
    UserRepository users;
    const auto all = users.findAll();
    list->setRowCount(static_cast<int>(all.size()));
    for (int i = 0; i < static_cast<int>(all.size()); ++i) {
        list->setItem(i, 0, new QTableWidgetItem(QString::number(all[i].userId)));
        list->setItem(i, 1, new QTableWidgetItem(User::roleToString(all[i].role)));
        list->setItem(i, 2, new QTableWidgetItem(all[i].code));
        list->setItem(i, 3, new QTableWidgetItem(all[i].fullName));
        list->setItem(i, 4, new QTableWidgetItem(all[i].isActive ? "Yes" : "No"));
    }
    layout->addWidget(list);
    QObject::connect(create, &QPushButton::clicked, page,
        [role, code, name, email, password, result] {
            const auto r = AccountService::createAccount(role->currentText(), code->text(),
                name->text(), email->text(), password->text());
            result->setText(r.message);
        });
    return page;
}

QWidget *adminCoursesPage() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    auto *title = new QLabel("Course management");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    UserRepository users;
    const auto lecturers = users.findAll("LECTURER");
    auto *code = new QLineEdit; code->setPlaceholderText("Course code");
    auto *name = new QLineEdit; name->setPlaceholderText("Course name");
    auto *lecturer = new QComboBox;
    for (const auto &u : lecturers)
        lecturer->addItem(u.code + " - " + u.fullName, u.userId);
    auto *create = new QPushButton("Create course");
    auto *result = new QLabel; result->setWordWrap(true);
    layout->addWidget(code); layout->addWidget(name); layout->addWidget(lecturer);
    layout->addWidget(create); layout->addWidget(result);

    auto *list = new QTableWidget;
    list->setColumnCount(4);
    list->setHorizontalHeaderLabels({"ID", "Code", "Name", "Lecturer"});
    list->horizontalHeader()->setStretchLastSection(true);
    CourseRepository courses;
    const auto all = courses.findAll();
    list->setRowCount(static_cast<int>(all.size()));
    for (int i = 0; i < static_cast<int>(all.size()); ++i) {
        list->setItem(i, 0, new QTableWidgetItem(QString::number(all[i].courseId)));
        list->setItem(i, 1, new QTableWidgetItem(all[i].code));
        list->setItem(i, 2, new QTableWidgetItem(all[i].name));
        list->setItem(i, 3, new QTableWidgetItem(all[i].lecturerName));
    }
    layout->addWidget(list);
    QObject::connect(create, &QPushButton::clicked, page,
        [code, name, lecturer, result] {
            if (lecturer->currentIndex() < 0) {
                result->setText("Create a Lecturer account first.");
                return;
            }
            const auto r = CourseService::createCourse(code->text(), name->text(),
                                                        lecturer->currentData().toInt());
            result->setText(r.message);
        });
    return page;
}

QWidget *adminTimetablePage() {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    auto *title = new QLabel("Timetable management");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    CourseRepository courses;
    const auto allCourses = courses.findAll();
    auto *course = new QComboBox;
    for (const auto &c : allCourses)
        course->addItem(c.code + " - " + c.name, c.courseId);
    auto *type = new QComboBox; type->addItems({"CLASS", "STUDY"});
    auto *day = new QSpinBox; day->setRange(1, 7); day->setValue(1);
    auto *start = new QTimeEdit(QTime(8, 0));
    auto *end = new QTimeEdit(QTime(9, 30));
    auto *room = new QLineEdit; room->setPlaceholderText("Room");
    auto *startDate = new QDateEdit(QDate::currentDate()); startDate->setCalendarPopup(true);
    auto *endDate = new QDateEdit(QDate::currentDate().addMonths(4)); endDate->setCalendarPopup(true);
    auto *add = new QPushButton("Add schedule entry");
    auto *result = new QLabel; result->setWordWrap(true);
    layout->addWidget(course); layout->addWidget(type);
    layout->addWidget(new QLabel("Day of week (1 = Monday, 7 = Sunday)")); layout->addWidget(day);
    layout->addWidget(start); layout->addWidget(end); layout->addWidget(room);
    layout->addWidget(startDate); layout->addWidget(endDate);
    layout->addWidget(add); layout->addWidget(result);
    layout->addStretch();

    QObject::connect(add, &QPushButton::clicked, page,
        [course, type, day, start, end, room, startDate, endDate, result] {
            ScheduleEntry e;
            e.courseId = course->currentData().toInt();
            e.type = type->currentText() == "CLASS" ? ScheduleType::CLASS : ScheduleType::STUDY;
            e.dayOfWeek = day->value(); e.startTime = start->time(); e.endTime = end->time();
            e.room = room->text().trimmed(); e.startDate = startDate->date(); e.endDate = endDate->date();
            result->setText(TimetableService::instance().addSchedule(e).message);
        });
    return page;
}

QWidget *studentOverviewPage(int userId) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    const auto currentUser = AuthService::instance().currentUser();
    const QString studentName = currentUser.has_value() ? currentUser->fullName : "Sinh viên";

    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel(QString("Xin chào, %1 👋").arg(studentName));
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel(QString("Hôm nay là %1 | Học kỳ 1 (2026 - 2027)")
        .arg(QDate::currentDate().toString("dddd, dd/MM/yyyy")));
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(24);
    contentRow->setAlignment(Qt::AlignTop);

    // LEFT COLUMN (Main: Today's Schedule + Active Attendance Sessions)
    auto *leftCol = new QVBoxLayout;
    leftCol->setSpacing(18);

    const auto studentEntries = TimetableService::instance().getStudentTimetable(userId);
    const QDate today = QDate::currentDate();

    // 1. Ca học hôm nay card
    auto *todayCard = new QFrame;
    todayCard->setObjectName("overviewCard");
    auto *todayLayout = new QVBoxLayout(todayCard);
    todayLayout->setContentsMargins(18, 16, 18, 16);
    todayLayout->setSpacing(12);

    auto *todayCardTitle = new QLabel("📅 Ca học hôm nay");
    todayCardTitle->setObjectName("overviewCardTitle");
    todayLayout->addWidget(todayCardTitle);

    int todayCount = 0;
    const int todayDow = today.dayOfWeek();
    for (const auto &entry : studentEntries) {
        if (entry.dayOfWeek == todayDow && today >= entry.startDate && today <= entry.endDate) {
            auto *item = new QFrame;
            item->setObjectName("scheduleItemCard");
            auto *itemLayout = new QVBoxLayout(item);
            itemLayout->setContentsMargins(12, 10, 12, 10);
            itemLayout->setSpacing(4);

            auto *r1 = new QHBoxLayout;
            auto *cName = new QLabel(entry.courseName);
            cName->setStyleSheet("font-weight: 700; font-size: 14px; color: #112D55; background: transparent;");
            r1->addWidget(cName);
            r1->addStretch();
            auto *cCode = new QLabel(entry.courseCode);
            cCode->setStyleSheet("font-size: 12px; color: #64748B; background: transparent;");
            r1->addWidget(cCode);
            itemLayout->addLayout(r1);

            auto *r2 = new QHBoxLayout;
            auto *cTime = new QLabel(QString("⏰ %1 - %2")
                .arg(entry.startTime.toString("HH:mm"))
                .arg(entry.endTime.toString("HH:mm")));
            cTime->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            r2->addWidget(cTime);

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(entry.room.isEmpty() ? "Chưa xếp phòng" : entry.room));
            cRoom->setStyleSheet("font-size: 12px; color: #334155; background: transparent;");
            r2->addWidget(cRoom);
            r2->addStretch();
            itemLayout->addLayout(r2);

            todayLayout->addWidget(item);
            todayCount++;
        }
    }
    if (todayCount == 0) {
        auto *emptyLbl = new QLabel("Hôm nay bạn không có lịch học nào trên thời khóa biểu.");
        emptyLbl->setStyleSheet("color: #64748B; font-style: italic; padding: 12px 0; background: transparent;");
        todayLayout->addWidget(emptyLbl);
    }
    leftCol->addWidget(todayCard);

    // 2. Điểm danh buổi học (Active Check-In sessions)
    auto *checkInCard = new QFrame;
    checkInCard->setObjectName("overviewCard");
    auto *checkInLayout = new QVBoxLayout(checkInCard);
    checkInLayout->setContentsMargins(18, 16, 18, 16);
    checkInLayout->setSpacing(12);

    auto *checkInTitle = new QLabel("🔑 Điểm danh buổi học");
    checkInTitle->setObjectName("overviewCardTitle");
    checkInLayout->addWidget(checkInTitle);

    CourseRepository courses;
    SessionRepository sessions;
    const auto enrolled = courses.findByStudent(userId);

    int openSessionsCount = 0;
    for (const auto &course : enrolled) {
        auto open = sessions.findOpenSessionForCourse(course.courseId);
        if (open.has_value()) {
            openSessionsCount++;
            auto *sessionBox = new QFrame;
            sessionBox->setStyleSheet("background-color: #F8FAFC; border: 1px solid #CBD5E1; border-radius: 8px; padding: 12px;");
            auto *sLayout = new QVBoxLayout(sessionBox);
            sLayout->setSpacing(8);

            auto *cTitle = new QLabel(QString("Môn: %1 - %2 (Phiên #%3)")
                .arg(course.code, course.name).arg(open->sessionId));
            cTitle->setStyleSheet("font-weight: bold; color: #112D55; font-size: 13px; background: transparent;");
            sLayout->addWidget(cTitle);

            auto *info = new QLabel(QString("Thời gian đúng giờ: %1 phút | Đi muộn: đến %2 phút.")
                .arg(open->presentWindowMinutes).arg(open->lateWindowMinutes));
            info->setStyleSheet("color: #64748B; font-size: 12px; background: transparent;");
            sLayout->addWidget(info);

            auto *row = new QHBoxLayout;
            auto *code = new QLineEdit;
            code->setPlaceholderText("Nhập mã 6 chữ số");
            code->setMaxLength(6);
            code->setFixedWidth(180);
            auto *submit = new QPushButton("Xác nhận điểm danh");
            submit->setCursor(Qt::PointingHandCursor);
            auto *result = new QLabel;
            result->setStyleSheet("font-size: 12px; font-weight: bold; background: transparent;");

            row->addWidget(code);
            row->addWidget(submit);
            row->addWidget(result);
            row->addStretch();
            sLayout->addLayout(row);

            QObject::connect(submit, &QPushButton::clicked, page,
                [sessionId = open->sessionId, userId, code, result] {
                    const auto response = CheckInService::instance().submitCheckIn(sessionId, userId, code->text());
                    result->setText(response.message);
                    if (response.code == CheckInResultCode::SUCCESS_PRESENT) {
                        result->setStyleSheet("color: #16A34A; font-weight: bold; font-size: 12px;");
                        code->clear();
                        code->setEnabled(false);
                    } else if (response.code == CheckInResultCode::SUCCESS_LATE) {
                        result->setStyleSheet("color: #D97706; font-weight: bold; font-size: 12px;");
                        code->clear();
                        code->setEnabled(false);
                    } else {
                        result->setStyleSheet("color: #DC2626; font-weight: bold; font-size: 12px;");
                    }
                });

            checkInLayout->addWidget(sessionBox);
        }
    }

    if (openSessionsCount == 0) {
        auto *noOpen = new QLabel("Hiện tại không có phiên điểm danh nào đang mở cho các môn học của bạn.");
        noOpen->setStyleSheet("color: #64748B; font-style: italic; padding: 8px 0; background: transparent;");
        checkInLayout->addWidget(noOpen);
    }

    leftCol->addWidget(checkInCard);
    leftCol->addStretch();
    contentRow->addLayout(leftCol, 3);

    // RIGHT COLUMN (Mini Calendar + Selected Date Schedule)
    auto *rightCol = new QVBoxLayout;
    rightCol->setSpacing(18);

    auto *miniCal = new MiniCalendarWidget;
    miniCal->setScheduleEntries(studentEntries);
    rightCol->addWidget(miniCal);

    auto *selectedDateContainer = new QWidget;
    auto *selectedDateLayout = new QVBoxLayout(selectedDateContainer);
    selectedDateLayout->setContentsMargins(0, 0, 0, 0);

    auto updateSelectedDateView = [selectedDateLayout, studentEntries](const QDate &d) {
        while (auto *child = selectedDateLayout->takeAt(0)) {
            if (child->widget()) delete child->widget();
            delete child;
        }
        selectedDateLayout->addWidget(createDayScheduleCard(studentEntries, d));
    };

    updateSelectedDateView(today);
    QObject::connect(miniCal, &MiniCalendarWidget::dateSelected, page, updateSelectedDateView);

    rightCol->addWidget(selectedDateContainer);
    rightCol->addStretch();
    contentRow->addLayout(rightCol, 2);

    pageLayout->addLayout(contentRow, 1);
    return page;
}

QWidget *studentTimetablePage(int userId) {
    return weeklyTimetablePage(
        TimetableService::instance().getStudentTimetable(userId),
        "Thời khóa biểu học tập cá nhân");
}

QWidget *studentAttendancePage(int userId) {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(10);

    auto *title = new QLabel("My attendance");
    title->setObjectName("lblHeading");
    layout->addWidget(title);

    CourseRepository courses;
    const auto enrolled = courses.findByStudent(userId);
    if (enrolled.empty()) {
        layout->addWidget(new QLabel("No attendance data found."));
    } else {
        for (const auto &course : enrolled) {
            const auto stats = AttendanceService::instance().getStudentStats(
                userId, course.courseId);
            layout->addWidget(new QLabel(QString("%1 - %2: %3% (%4 completed sessions)")
                .arg(course.code)
                .arg(course.name)
                .arg(stats.attendanceRate, 0, 'f', 1)
                .arg(stats.totalCompletedSessions)));
        }
    }
    layout->addStretch();
    return page;
}

}

UiLauncher::~UiLauncher() {
    delete m_login;
    delete m_shell;
}

void UiLauncher::start() {
    QFile style(":/style.qss");
    if (style.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(QString::fromUtf8(style.readAll()));
    }

    m_login = new LoginWindow;
    connect(m_login, &LoginWindow::loginRequested, this,
            [this](const QString &account, const QString &password) {
                if (AuthService::instance().login(account, password)) {
                    openShellFor(account);
                } else {
                    m_login->showError(AuthService::instance().lastError());
                }
            });
    m_login->show();
}

void UiLauncher::openShellFor(const QString &) {
    const auto user = AuthService::instance().currentUser();
    if (!user.has_value()) return;

    AppShell::Role role = AppShell::Role::Student;
    QStringList pages = {"Overview", "Timetable", "My attendance"};

    if (user->role == UserRole::LECTURER) {
        role = AppShell::Role::Lecturer;
        pages = {"Overview", "Timetable", "Open session", "Live session",
                 "Attendance report"};
    } else if (user->role == UserRole::ADMIN) {
        role = AppShell::Role::Admin;
        pages = {"Accounts", "Courses", "Timetables"};
    }

    delete m_shell;
    m_shell = new AppShell(role, user->fullName);
    m_shell->setAttribute(Qt::WA_DeleteOnClose);

    for (const QString &p : pages) {
        if (user->role == UserRole::LECTURER && p == "Overview") {
            m_shell->addPage(p, lecturerOverviewPage(user->userId));
        } else if (user->role == UserRole::LECTURER && p == "Timetable") {
            m_shell->addPage(p, lecturerTimetablePage(user->userId));
        } else if (user->role == UserRole::LECTURER && p == "Open session") {
            m_shell->addPage(p, lecturerOpenSessionPage(user->userId));
        } else if (user->role == UserRole::LECTURER && p == "Attendance report") {
            m_shell->addPage(p, lecturerReportPage(user->userId));
        } else if (user->role == UserRole::ADMIN && p == "Accounts") {
            m_shell->addPage(p, adminAccountsPage());
        } else if (user->role == UserRole::ADMIN && p == "Courses") {
            m_shell->addPage(p, adminCoursesPage());
        } else if (user->role == UserRole::ADMIN && p == "Timetables") {
            m_shell->addPage(p, adminTimetablePage());
        } else if (user->role == UserRole::STUDENT && p == "Overview") {
            m_shell->addPage(p, studentOverviewPage(user->userId));
        } else if (user->role == UserRole::STUDENT && p == "Timetable") {
            m_shell->addPage(p, studentTimetablePage(user->userId));
        } else if (user->role == UserRole::STUDENT && p == "My attendance") {
            m_shell->addPage(p, studentAttendancePage(user->userId));
        } else {
            auto *placeholder = new QLabel(p + " (todo)");
            placeholder->setMargin(32);
            placeholder->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            m_shell->addPage(p, placeholder);
        }
    }

    connect(m_shell, &AppShell::signOutRequested, this, [this] {
        AuthService::instance().logout();
        if (m_shell) m_shell->close();
        if (m_login) m_login->show();
    });

    m_shell->resize(1440, 900);
    if (m_login) m_login->hide();
    m_shell->show();
}
