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
#include <QDialog>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <functional>

UiLauncher::UiLauncher(QObject *parent) : QObject(parent) {}

namespace {

static QString cleanRoomName(const QString &raw) {
    if (raw.trimmed().isEmpty()) return "ONLINE";
    int idx = raw.indexOf('(');
    if (idx != -1) {
        return raw.left(idx).trimmed();
    }
    return raw.trimmed();
}

QFrame *createKpiCard(const QString &title, const QString &value, const QString &subtext, const QString &valueColor = "") {
    auto *card = new QFrame;
    card->setObjectName("kpiCard");
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(18, 16, 18, 16);
    lay->setSpacing(6);

    auto *lblTitle = new QLabel(title);
    lblTitle->setObjectName("kpiTitle");
    lay->addWidget(lblTitle);

    auto *lblVal = new QLabel(value);
    lblVal->setObjectName("kpiValue");
    if (!valueColor.isEmpty()) {
        lblVal->setStyleSheet(QString("color: %1;").arg(valueColor));
    }
    lay->addWidget(lblVal);

    auto *lblSub = new QLabel(subtext);
    lblSub->setObjectName("kpiSubtext");
    lay->addWidget(lblSub);

    return card;
}

QWidget *createStatusBadgeWidget(AttendanceStatus status) {
    auto *container = new QWidget;
    auto *lay = new QHBoxLayout(container);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setAlignment(Qt::AlignCenter);

    auto *badge = new QLabel;
    switch (status) {
    case AttendanceStatus::PRESENT:
        badge->setObjectName("badgePresent");
        badge->setText("Present");
        break;
    case AttendanceStatus::LATE:
        badge->setObjectName("badgeLate");
        badge->setText("Late");
        break;
    case AttendanceStatus::ABSENT:
        badge->setObjectName("badgeAbsent");
        badge->setText("Absent");
        break;
    case AttendanceStatus::EXCUSED:
        badge->setObjectName("badgeExcused");
        badge->setText("Excused");
        break;
    }
    lay->addWidget(badge);
    return container;
}

void showExcuseRequestDialog(QWidget *parent, const QString &courseTitle, const QString &sessionDate) {
    auto *dialog = new QDialog(parent);
    dialog->setWindowTitle("Gửi đơn xin phép vắng học (Excuse Request)");
    dialog->setFixedWidth(480);
    dialog->setStyleSheet("QDialog { background-color: #FFFFFF; color: #1E293B; }");

    auto *lay = new QVBoxLayout(dialog);
    lay->setContentsMargins(24, 22, 24, 22);
    lay->setSpacing(14);

    auto *lblTitle = new QLabel("📝 Đơn xin phép vắng học");
    lblTitle->setStyleSheet("font-size: 17px; font-weight: 700; color: #112D55;");
    lay->addWidget(lblTitle);

    auto *lblInfo = new QLabel(QString("Môn học: <b>%1</b><br>Buổi học: %2").arg(courseTitle, sessionDate));
    lblInfo->setStyleSheet("color: #475569; font-size: 13px; line-height: 1.4;");
    lay->addWidget(lblInfo);

    auto *lblReason = new QLabel("Lý do vắng học *:");
    lblReason->setStyleSheet("font-weight: 600; color: #1E293B; font-size: 13px;");
    lay->addWidget(lblReason);

    auto *editReason = new QTextEdit;
    editReason->setPlaceholderText("Vui lòng ghi rõ lý do (ví dụ: bị ốm, có giấy khám bệnh, việc gia đình cấp bách)...");
    editReason->setStyleSheet("QTextEdit { background: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 6px; padding: 8px; font-size: 13px; color: #0F172A; } QTextEdit:focus { border: 1px solid #1B4F96; }");
    editReason->setFixedHeight(90);
    lay->addWidget(editReason);

    auto *lblProof = new QLabel("Minh chứng kèm theo (giấy tờ y tế / xác nhận):");
    lblProof->setStyleSheet("font-weight: 600; color: #1E293B; font-size: 13px;");
    lay->addWidget(lblProof);

    auto *fileRow = new QHBoxLayout;
    auto *lblFileName = new QLabel("Chưa chọn tệp");
    lblFileName->setStyleSheet("color: #64748B; font-style: italic; font-size: 12px;");
    auto *btnChoose = new QPushButton("Chọn tệp...");
    btnChoose->setObjectName("btnOutline");
    btnChoose->setCursor(Qt::PointingHandCursor);
    fileRow->addWidget(btnChoose);
    fileRow->addWidget(lblFileName, 1);
    lay->addLayout(fileRow);

    QObject::connect(btnChoose, &QPushButton::clicked, dialog, [dialog, lblFileName] {
        const QString path = QFileDialog::getOpenFileName(dialog, "Chọn tệp minh chứng", "", "Tài liệu (*.pdf *.png *.jpg *.jpeg)");
        if (!path.isEmpty()) {
            lblFileName->setText(QFileInfo(path).fileName());
            lblFileName->setStyleSheet("color: #15803D; font-weight: 600; font-size: 12px;");
        }
    });

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    auto *btnCancel = new QPushButton("Hủy");
    btnCancel->setObjectName("btnOutline");
    btnCancel->setCursor(Qt::PointingHandCursor);

    auto *btnSubmit = new QPushButton("Gửi đơn xin phép");
    btnSubmit->setStyleSheet("QPushButton { background-color: #1B4F96; color: #FFFFFF; font-weight: bold; border-radius: 6px; padding: 8px 18px; border: none; } QPushButton:hover { background-color: #163F7A; }");
    btnSubmit->setCursor(Qt::PointingHandCursor);

    btnRow->addWidget(btnCancel);
    btnRow->addWidget(btnSubmit);
    lay->addLayout(btnRow);

    QObject::connect(btnCancel, &QPushButton::clicked, dialog, &QDialog::reject);
    QObject::connect(btnSubmit, &QPushButton::clicked, dialog, [dialog, editReason] {
        if (editReason->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(dialog, "Thiếu thông tin", "Vui lòng nhập lý do vắng học trước khi gửi đơn.");
            return;
        }
        QMessageBox::information(dialog, "Đã gửi đơn", "Đơn xin phép vắng học của bạn đã được gửi thành công đến giảng viên phụ trách môn học để duyệt.");
        dialog->accept();
    });

    dialog->exec();
}


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

            const QString typeBadge = (entry.type == ScheduleType::STUDY) ? "🌐 E-Learning" : "🏫 Trực tiếp";
            const QString typeStyle = (entry.type == ScheduleType::STUDY)
                ? "color: #0369A1; background: #E0F2FE; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;"
                : "color: #15803D; background: #DCFCE7; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;";
            auto *badge = new QLabel(typeBadge);
            badge->setStyleSheet(typeStyle);
            rowTop->addWidget(badge);

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

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(cleanRoomName(entry.room)));
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

QWidget *createActiveBadgeWidget(bool isActive) {
    auto *container = new QWidget;
    auto *lay = new QHBoxLayout(container);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setAlignment(Qt::AlignCenter);

    auto *badge = new QLabel(isActive ? "Active" : "Inactive");
    badge->setObjectName(isActive ? "badgeActive" : "badgeInactive");
    lay->addWidget(badge);
    return container;
}

QWidget *lecturerOverviewPage(int userId, std::function<void(const QString&)> navCallback = nullptr, AppShell *shell = nullptr) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    const auto currentUser = AuthService::instance().currentUser();
    const QString lecturerName = currentUser.has_value() ? currentUser->fullName : "Giảng viên";

    // 1. Header Row
    auto *headerRow = new QHBoxLayout;
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel(QString("Xin chào, %1 👨‍🏫").arg(lecturerName));
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel(QString("Hôm nay là %1 · Học kỳ 1 (2026 - 2027)")
        .arg(QDate::currentDate().toString("dddd, dd/MM/yyyy")));
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    headerRow->addLayout(headerBox, 1);

    auto *btnOpenQuick = new QPushButton("Mở phiên điểm danh →");
    btnOpenQuick->setObjectName("btnOutline");
    btnOpenQuick->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(btnOpenQuick);
    pageLayout->addLayout(headerRow);

    if (navCallback) {
        QObject::connect(btnOpenQuick, &QPushButton::clicked, page, [navCallback] {
            navCallback("Open session");
        });
    }

    // 2. Compute stats across taught courses
    CourseRepository courses;
    SessionRepository sessionRepo;
    AttendanceRepository attRepo;
    const auto taught = courses.findByLecturer(userId);

    int totalCourses = static_cast<int>(taught.size());
    int totalEnrolledStudents = 0;
    int totalCompletedSessions = 0;
    int totalSessionRecords = 0;
    int creditedRecords = 0;
    int activeOpenSessions = 0;
    std::optional<AttendanceSession> firstOpenSession;
    QString firstOpenCourseTitle;

    for (const auto &c : taught) {
        const auto enrolled = courses.getEnrolledStudents(c.courseId);
        totalEnrolledStudents += static_cast<int>(enrolled.size());

        auto open = sessionRepo.findOpenSessionForCourse(c.courseId);
        if (open.has_value()) {
            activeOpenSessions++;
            if (!firstOpenSession.has_value()) {
                firstOpenSession = open;
                firstOpenCourseTitle = QString("%1 - %2").arg(c.code, c.name);
            }
        }

        const auto pastSessions = sessionRepo.findByCourse(c.courseId);
        for (const auto &s : pastSessions) {
            if (s.status == SessionStatus::CLOSED) {
                totalCompletedSessions++;
                const auto recs = attRepo.findBySession(s.sessionId);
                for (const auto &r : recs) {
                    totalSessionRecords++;
                    if (r.status == AttendanceStatus::PRESENT || r.status == AttendanceStatus::LATE || r.status == AttendanceStatus::EXCUSED) {
                        creditedRecords++;
                    }
                }
            }
        }
    }

    const double termRate = totalSessionRecords > 0 ? ((double)creditedRecords / totalSessionRecords) * 100.0 : 100.0;

    // 4 Top KPI Cards
    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(16);

    kpiRow->addWidget(createKpiCard(
        "Lớp học phần (Assigned courses)",
        QString::number(totalCourses),
        QString("%1 sinh viên đăng ký").arg(totalEnrolledStudents)));

    kpiRow->addWidget(createKpiCard(
        "Tỷ lệ chuyên cần (Term attendance)",
        QString("%1%").arg(termRate, 0, 'f', 1),
        QString("%1 / %2 lượt ghi nhận").arg(creditedRecords).arg(totalSessionRecords)));

    kpiRow->addWidget(createKpiCard(
        "Buổi dạy đã học (Completed)",
        QString("%1 buổi").arg(totalCompletedSessions),
        "Đã kết thúc điểm danh"));

    kpiRow->addWidget(createKpiCard(
        "Phiên đang mở (Active sessions)",
        QString("%1 phiên").arg(activeOpenSessions),
        activeOpenSessions > 0 ? "Đang mở điểm danh" : "Chưa có phiên mở",
        activeOpenSessions > 0 ? "#15803D" : "#112D55"));

    pageLayout->addLayout(kpiRow);

    // 3. Main Content: 2 Columns
    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(24);
    contentRow->setAlignment(Qt::AlignTop);

    // LEFT COLUMN (Active Session Alert, Today's Timetable, Guidelines)
    auto *leftCol = new QVBoxLayout;
    leftCol->setSpacing(18);

    auto *liveAlertCard = new QFrame;
    liveAlertCard->setStyleSheet("background-color: #EFF6FF; border: 1.5px solid #1B4F96; border-radius: 12px; padding: 16px;");
    auto *lAlertLay = new QVBoxLayout(liveAlertCard);
    lAlertLay->setSpacing(8);

    auto *aHead = new QHBoxLayout;
    auto *aBadge = new QLabel("● PHIÊN ĐIỂM DANH ĐANG MỞ");
    aBadge->setStyleSheet("background: #DCFCE7; color: #15803D; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 2px 8px;");
    aHead->addWidget(aBadge);
    aHead->addStretch();
    auto *btnGoLive = new QPushButton("Vào phòng trực tiếp →");
    btnGoLive->setObjectName("btnPrimary");
    btnGoLive->setCursor(Qt::PointingHandCursor);
    aHead->addWidget(btnGoLive);

    auto *btnEndNow = new QPushButton("🛑 Kết thúc buổi học ngay");
    btnEndNow->setObjectName("btnDanger");
    btnEndNow->setCursor(Qt::PointingHandCursor);
    aHead->addWidget(btnEndNow);
    lAlertLay->addLayout(aHead);

    auto *lblCourse = new QLabel;
    lblCourse->setStyleSheet("font-size: 15px; font-weight: bold; color: #112D55; background: transparent;");
    lAlertLay->addWidget(lblCourse);

    auto *lblCodeNow = new QLabel;
    lblCodeNow->setStyleSheet("font-size: 13px; color: #334155; background: transparent;");
    lAlertLay->addWidget(lblCodeNow);

    leftCol->addWidget(liveAlertCard);

    if (navCallback) {
        QObject::connect(btnGoLive, &QPushButton::clicked, page, [navCallback] {
            navCallback("Live session");
        });
    }

    auto activeSessionId = std::make_shared<int>(0);
    auto activeCourseId = std::make_shared<int>(0);

    auto updateOverviewAlert = std::make_shared<std::function<void()>>();
    *updateOverviewAlert = [=]() {
        CourseRepository cr;
        SessionRepository sr;
        const auto tCourses = cr.findByLecturer(userId);
        std::optional<AttendanceSession> openSess;
        Course openC;
        for (const auto &c : tCourses) {
            auto op = sr.findOpenSessionForCourse(c.courseId);
            if (op.has_value()) {
                openSess = op;
                openC = c;
                break;
            }
        }
        if (openSess.has_value()) {
            *activeSessionId = openSess->sessionId;
            *activeCourseId = openC.courseId;
            lblCourse->setText(QString("%1 - %2 · Phiên #%3").arg(openC.code, openC.name).arg(openSess->sessionId));
            lblCodeNow->setText(QString("Mã 6 chữ số: <span style='font-size: 22px; font-weight: 800; color: #1B4F96; letter-spacing: 4px;'>%1</span> (tự xoay sau 60s)")
                .arg(openSess->sessionCode));
            liveAlertCard->setVisible(true);
        } else {
            *activeSessionId = 0;
            *activeCourseId = 0;
            liveAlertCard->setVisible(false);
        }
    };
    (*updateOverviewAlert)();

    QObject::connect(btnEndNow, &QPushButton::clicked, page, [=, updateOverviewAlert] {
        const int sessId = *activeSessionId;
        if (sessId <= 0) return;
        const int cid = *activeCourseId;
        const auto recs = AttendanceRepository{}.findBySession(sessId);
        const auto enrolled = CourseRepository{}.getEnrolledStudents(cid);
        int checkedCount = 0;
        for (const auto &r : recs) {
            if (r.status == AttendanceStatus::PRESENT || r.status == AttendanceStatus::LATE)
                checkedCount++;
        }
        int absentCount = static_cast<int>(enrolled.size()) - checkedCount;
        if (absentCount < 0) absentCount = 0;

        const QString msg = QString(
            "Xác nhận kết thúc buổi học & đóng phiên điểm danh #%1 ngay lập tức?\n\n"
            "• Sĩ số lớp: %2 sinh viên\n"
            "• Đã điểm danh: %3 sinh viên\n"
            "• %4 sinh viên chưa điểm danh sẽ tự động được ghi nhận là VẮNG (ABSENT).\n\n"
            "Bạn có muốn kết thúc ca học ngay?")
            .arg(sessId).arg(enrolled.size()).arg(checkedCount).arg(absentCount);

        const auto reply = QMessageBox::question(page, "Xác nhận kết thúc buổi học",
            msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            const auto res = SessionService::instance().closeSession(sessId);
            if (res.ok) {
                QMessageBox::information(page, "Đã kết thúc buổi học",
                    QString("Buổi điểm danh #%1 đã kết thúc thành công.\n%2 sinh viên được ghi nhận vắng.")
                        .arg(sessId).arg(absentCount));
                if (updateOverviewAlert && *updateOverviewAlert) (*updateOverviewAlert)();
            } else {
                QMessageBox::warning(page, "Lỗi", res.message);
            }
        }
    });

    QObject::connect(&SessionService::instance(), &SessionService::sessionOpened, page, [=, updateOverviewAlert](int) {
        if (updateOverviewAlert && *updateOverviewAlert) (*updateOverviewAlert)();
    });
    QObject::connect(&SessionService::instance(), &SessionService::codeRotated, page, [=, updateOverviewAlert](int, const QString&) {
        if (updateOverviewAlert && *updateOverviewAlert) (*updateOverviewAlert)();
    });
    QObject::connect(&SessionService::instance(), &SessionService::sessionClosed, page, [=, updateOverviewAlert](int) {
        if (updateOverviewAlert && *updateOverviewAlert) (*updateOverviewAlert)();
    });

    auto *overviewTimer = new QTimer(page);
    overviewTimer->setInterval(2000);
    QObject::connect(overviewTimer, &QTimer::timeout, page, [=, updateOverviewAlert]() {
        if (page->isVisible() && updateOverviewAlert && *updateOverviewAlert) {
            (*updateOverviewAlert)();
        }
    });
    overviewTimer->start();

    if (shell) {
        QObject::connect(shell, &AppShell::pageChanged, page, [=, updateOverviewAlert](const QString &cur) {
            if (cur == "Overview" && updateOverviewAlert && *updateOverviewAlert) {
                (*updateOverviewAlert)();
            }
        });
    }

    const auto lecturerEntries = TimetableService::instance().getLecturerTimetable(userId);
    const QDate today = QDate::currentDate();

    // Ca dạy hôm nay card
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

            const QString typeBadge = (entry.type == ScheduleType::STUDY) ? "🌐 E-Learning" : "🏫 Trực tiếp";
            const QString typeStyle = (entry.type == ScheduleType::STUDY)
                ? "color: #0369A1; background: #E0F2FE; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;"
                : "color: #15803D; background: #DCFCE7; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;";
            auto *badge = new QLabel(typeBadge);
            badge->setStyleSheet(typeStyle);
            r1->addWidget(badge);
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

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(cleanRoomName(entry.room)));
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

    // Hướng dẫn nghiệp vụ giảng viên
    auto *notesCard = new QFrame;
    notesCard->setObjectName("overviewCard");
    auto *notesLayout = new QVBoxLayout(notesCard);
    notesLayout->setContentsMargins(18, 14, 18, 14);
    notesLayout->setSpacing(6);
    auto *notesTitle = new QLabel("📌 Hướng dẫn & Quy định điểm danh UTH");
    notesTitle->setStyleSheet("font-weight: 700; color: #112D55; font-size: 14px;");
    notesLayout->addWidget(notesTitle);
    auto *n1 = new QLabel("• Mã 6 số được sinh ngẫu nhiên và tự động xoay vòng sau mỗi 60 giây để chống sinh viên gửi mã ra ngoài.");
    n1->setStyleSheet("color: #475569; font-size: 12px;");
    auto *n2 = new QLabel("• Sinh viên điểm danh sau cửa sổ đúng giờ sẽ được ghi nhận là 'Đi muộn' (vẫn được tính chuyên cần).");
    n2->setStyleSheet("color: #475569; font-size: 12px;");
    auto *n3 = new QLabel("• Khi đóng phiên điểm danh, tất cả sinh viên chưa check-in sẽ tự động được hệ thống ghi nhận là 'Vắng (Absent)'.");
    n3->setStyleSheet("color: #475569; font-size: 12px;");
    notesLayout->addWidget(n1);
    notesLayout->addWidget(n2);
    notesLayout->addWidget(n3);
    leftCol->addWidget(notesCard);

    leftCol->addStretch();
    contentRow->addLayout(leftCol, 3);

    // RIGHT COLUMN (Mini Calendar + Selected Date Schedule + Assigned Courses Summary)
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

    // Assigned Courses Summary Card
    auto *coursesCard = new QFrame;
    coursesCard->setObjectName("overviewCard");
    auto *coursesLayout = new QVBoxLayout(coursesCard);
    coursesLayout->setContentsMargins(18, 16, 18, 16);
    coursesLayout->setSpacing(10);

    auto *coursesCardHeader = new QHBoxLayout;
    auto *coursesCardTitle = new QLabel("📚 Lớp học phần phụ trách");
    coursesCardTitle->setObjectName("overviewCardTitle");
    coursesCardHeader->addWidget(coursesCardTitle);
    coursesCardHeader->addStretch();
    coursesLayout->addLayout(coursesCardHeader);

    for (const auto &c : taught) {
        const auto enrolled = courses.getEnrolledStudents(c.courseId);
        auto *item = new QFrame;
        item->setStyleSheet("QFrame { background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 8px; padding: 10px 12px; } QFrame:hover { background: #F1F5F9; }");
        auto *iLay = new QHBoxLayout(item);
        iLay->setContentsMargins(0, 0, 0, 0);

        auto *cText = new QLabel(QString("%1\n%2 · %3 sinh viên").arg(c.name, c.code).arg(enrolled.size()));
        cText->setStyleSheet("font-size: 12px; font-weight: 600; color: #112D55; background: transparent; border: none;");
        iLay->addWidget(cText, 1);

        if (navCallback) {
            auto *btnRep = new QPushButton("Báo cáo →");
            btnRep->setObjectName("linkPrimary");
            btnRep->setCursor(Qt::PointingHandCursor);
            iLay->addWidget(btnRep);
            QObject::connect(btnRep, &QPushButton::clicked, page, [navCallback] {
                navCallback("Attendance report");
            });
        }
        coursesLayout->addWidget(item);
    }
    if (taught.empty()) {
        auto *emptyTaught = new QLabel("Chưa được phân công giảng dạy môn nào trong học kỳ này.");
        emptyTaught->setStyleSheet("color: #64748B; font-style: italic; font-size: 12px;");
        coursesLayout->addWidget(emptyTaught);
    }

    rightCol->addWidget(coursesCard);
    rightCol->addStretch();
    contentRow->addLayout(rightCol, 2);

    pageLayout->addLayout(contentRow, 1);
    return page;
}

QWidget *lecturerOpenSessionPage(int userId, std::function<void(const QString&)> navCallback = nullptr, AppShell *shell = nullptr) {
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    // 1. Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Mở phiên điểm danh mới (Open Session)");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Học kỳ 1 (2026 - 2027) · Chọn lớp học phần và thiết lập cửa sổ điểm danh");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    layout->addLayout(headerBox);

    CourseRepository courses;
    const auto taught = courses.findByLecturer(userId);

    // Main Card
    auto *card = new QFrame;
    card->setObjectName("overviewCard");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // Row 1: Course Selection
    auto *courseLabel = new QLabel("Lớp học phần (Course)");
    courseLabel->setStyleSheet("font-weight: 700; color: #112D55; font-size: 13px;");
    cardLayout->addWidget(courseLabel);

    auto *courseBox = new QComboBox;
    courseBox->setFixedHeight(38);
    for (const auto &course : taught) {
        courseBox->addItem(QString("%1 - %2").arg(course.code, course.name), course.courseId);
    }
    cardLayout->addWidget(courseBox);

    // Active session alert box if already open
    auto *alertBox = new QFrame;
    alertBox->setStyleSheet("QFrame { background-color: #FEF3C7; border: 1px solid #FDE68A; border-radius: 8px; padding: 12px; }");
    auto *alertLay = new QHBoxLayout(alertBox);
    auto *alertText = new QLabel;
    alertText->setStyleSheet("color: #92400E; font-size: 13px; font-weight: 600; background: transparent; border: none;");
    alertLay->addWidget(alertText, 1);

    auto *btnJumpLive = new QPushButton("Vào phòng trực tiếp →");
    btnJumpLive->setObjectName("btnOutline");
    btnJumpLive->setCursor(Qt::PointingHandCursor);
    alertLay->addWidget(btnJumpLive);

    auto *btnEndSessionNow = new QPushButton("🛑 Kết thúc buổi học ngay (End Course Session)");
    btnEndSessionNow->setObjectName("btnDanger");
    btnEndSessionNow->setCursor(Qt::PointingHandCursor);
    alertLay->addWidget(btnEndSessionNow);

    alertBox->setVisible(false);
    cardLayout->addWidget(alertBox);

    if (navCallback) {
        QObject::connect(btnJumpLive, &QPushButton::clicked, page, [navCallback] {
            navCallback("Live session");
        });
    }

    // Row 2: Presets
    auto *presetLabel = new QLabel("Thiết lập nhanh (Presets)");
    presetLabel->setStyleSheet("font-weight: 700; color: #112D55; font-size: 13px;");
    cardLayout->addWidget(presetLabel);

    auto *presetRow = new QHBoxLayout;
    presetRow->setSpacing(12);

    auto *btnStandard = new QPushButton("⏱ Chuẩn (Standard): 80p / 20p / 80p");
    btnStandard->setObjectName("btnPresetActive");
    btnStandard->setCursor(Qt::PointingHandCursor);
    presetRow->addWidget(btnStandard);

    auto *btnQuick = new QPushButton("⚡ Nhanh (Quick): 5p / 5p / 5p");
    btnQuick->setObjectName("btnPreset");
    btnQuick->setCursor(Qt::PointingHandCursor);
    presetRow->addWidget(btnQuick);
    presetRow->addStretch();
    cardLayout->addLayout(presetRow);

    // Row 3: Windows SpinBoxes (Duration, Present, Late)
    auto *spinRow = new QHBoxLayout;
    spinRow->setSpacing(18);

    auto makeSpinBox = [](int defaultVal) {
        auto *sp = new QSpinBox;
        sp->setRange(1, 600);
        sp->setValue(defaultVal);
        sp->setFixedHeight(38);
        return sp;
    };

    auto *boxDuration = new QVBoxLayout;
    boxDuration->setSpacing(4);
    auto *lblDur = new QLabel("Thời lượng phiên (Phút)");
    lblDur->setStyleSheet("font-size: 12px; color: #475569; font-weight: 600;");
    auto *spinDuration = makeSpinBox(80);
    boxDuration->addWidget(lblDur);
    boxDuration->addWidget(spinDuration);
    spinRow->addLayout(boxDuration, 1);

    auto *boxPresent = new QVBoxLayout;
    boxPresent->setSpacing(4);
    auto *lblPres = new QLabel("Cửa sổ đúng giờ (Phút)");
    lblPres->setStyleSheet("font-size: 12px; color: #475569; font-weight: 600;");
    auto *spinPresent = makeSpinBox(20);
    boxPresent->addWidget(lblPres);
    boxPresent->addWidget(spinPresent);
    spinRow->addLayout(boxPresent, 1);

    auto *boxLate = new QVBoxLayout;
    boxLate->setSpacing(4);
    auto *lblLate = new QLabel("Cửa sổ đi muộn tối đa (Phút)");
    lblLate->setStyleSheet("font-size: 12px; color: #475569; font-weight: 600;");
    auto *spinLate = makeSpinBox(80);
    boxLate->addWidget(lblLate);
    boxLate->addWidget(spinLate);
    spinRow->addLayout(boxLate, 1);

    cardLayout->addLayout(spinRow);

    // Validation Feedback Label
    auto *valLabel = new QLabel;
    valLabel->setWordWrap(true);
    cardLayout->addWidget(valLabel);

    // Action button
    auto *btnOpen = new QPushButton("Bắt đầu phiên điểm danh");
    btnOpen->setFixedHeight(40);
    btnOpen->setObjectName("btnPrimary");
    btnOpen->setCursor(Qt::PointingHandCursor);
    cardLayout->addWidget(btnOpen);

    auto *statusLabel = new QLabel;
    statusLabel->setWordWrap(true);
    cardLayout->addWidget(statusLabel);

    layout->addWidget(card);

    // Preset handlers
    QObject::connect(btnStandard, &QPushButton::clicked, page, [=] {
        btnStandard->setObjectName("btnPresetActive");
        btnStandard->setStyle(btnStandard->style());
        btnQuick->setObjectName("btnPreset");
        btnQuick->setStyle(btnQuick->style());
        spinDuration->setValue(80);
        spinPresent->setValue(20);
        spinLate->setValue(80);
    });

    QObject::connect(btnQuick, &QPushButton::clicked, page, [=] {
        btnQuick->setObjectName("btnPresetActive");
        btnQuick->setStyle(btnQuick->style());
        btnStandard->setObjectName("btnPreset");
        btnStandard->setStyle(btnStandard->style());
        spinDuration->setValue(5);
        spinPresent->setValue(5);
        spinLate->setValue(5);
    });

    // Real-time validation
    auto validateInputs = [=]() -> bool {
        const int dur = spinDuration->value();
        const int pres = spinPresent->value();
        const int lt = spinLate->value();

        if (pres <= 0 || lt < pres || dur < lt) {
            valLabel->setText("⚠️ Quy định: 0 < Cửa sổ đúng giờ ≤ Cửa sổ đi muộn ≤ Thời lượng phiên.");
            valLabel->setStyleSheet("color: #DC2626; font-size: 13px; font-weight: 600;");
            btnOpen->setEnabled(false);
            return false;
        }

        valLabel->setText(QString("✓ Thiết lập hợp lệ: Đúng giờ trong 0–%1p, Đi muộn trong %1–%2p, Phiên đóng sau %3p.")
            .arg(pres).arg(lt).arg(dur));
        valLabel->setStyleSheet("color: #15803D; font-size: 13px; font-weight: 600;");
        btnOpen->setEnabled(true);
        return true;
    };

    QObject::connect(spinDuration, QOverload<int>::of(&QSpinBox::valueChanged), page, [=](int) { validateInputs(); });
    QObject::connect(spinPresent, QOverload<int>::of(&QSpinBox::valueChanged), page, [=](int) { validateInputs(); });
    QObject::connect(spinLate, QOverload<int>::of(&QSpinBox::valueChanged), page, [=](int) { validateInputs(); });

    // Check open session for currently selected course
    auto checkCourseOpenSession = std::make_shared<std::function<void()>>();
    *checkCourseOpenSession = [=, checkCourseOpenSession]() {
        if (courseBox->currentIndex() < 0) return;
        const int cid = courseBox->currentData().toInt();
        auto existing = SessionRepository{}.findOpenSessionForCourse(cid);
        if (existing.has_value()) {
            alertText->setText(QString("⚠️ Lớp này đang có phiên điểm danh mở (#%1). Hãy vào phòng trực tiếp để điều hành hoặc kết thúc phiên trước khi mở mới.")
                .arg(existing->sessionId));
            alertBox->setVisible(true);
            btnOpen->setEnabled(false);
        } else {
            alertBox->setVisible(false);
            validateInputs();
        }
    };

    QObject::connect(btnEndSessionNow, &QPushButton::clicked, page, [=, checkCourseOpenSession] {
        if (courseBox->currentIndex() < 0) return;
        const int cid = courseBox->currentData().toInt();
        auto existing = SessionRepository{}.findOpenSessionForCourse(cid);
        if (!existing.has_value()) return;

        const auto enrolled = CourseRepository{}.getEnrolledStudents(cid);
        const auto recs = AttendanceRepository{}.findBySession(existing->sessionId);
        int checkedCount = 0;
        for (const auto &r : recs) {
            if (r.status == AttendanceStatus::PRESENT || r.status == AttendanceStatus::LATE)
                checkedCount++;
        }
        int absentWillBe = static_cast<int>(enrolled.size()) - checkedCount;
        if (absentWillBe < 0) absentWillBe = 0;

        const QString confirmMsg = QString(
            "Xác nhận kết thúc buổi điểm danh cho môn học này ngay lập tức?\n\n"
            "• Phiên điểm danh: #%1\n"
            "• Sĩ số lớp: %2 sinh viên\n"
            "• Đã điểm danh: %3 sinh viên\n"
            "• %4 sinh viên chưa điểm danh sẽ tự động được ghi nhận là VẮNG (ABSENT).\n\n"
            "Bạn có chắc chắn muốn kết thúc buổi học ngay bây giờ?")
            .arg(existing->sessionId).arg(enrolled.size()).arg(checkedCount).arg(absentWillBe);

        const auto reply = QMessageBox::question(page, "Xác nhận kết thúc buổi học ngay",
            confirmMsg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            const auto res = SessionService::instance().closeSession(existing->sessionId);
            if (res.ok) {
                QMessageBox::information(page, "Đã kết thúc buổi học",
                    QString("Buổi điểm danh #%1 đã kết thúc thành công.\n%2 sinh viên được ghi nhận vắng.")
                        .arg(existing->sessionId).arg(absentWillBe));
                if (checkCourseOpenSession && *checkCourseOpenSession) (*checkCourseOpenSession)();
            } else {
                QMessageBox::warning(page, "Lỗi", res.message);
            }
        }
    });

    QObject::connect(courseBox, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=, checkCourseOpenSession](int) {
        if (checkCourseOpenSession && *checkCourseOpenSession) (*checkCourseOpenSession)();
    });

    QObject::connect(&SessionService::instance(), &SessionService::sessionOpened, page, [=, checkCourseOpenSession](int) {
        if (checkCourseOpenSession && *checkCourseOpenSession) (*checkCourseOpenSession)();
    });
    QObject::connect(&SessionService::instance(), &SessionService::sessionClosed, page, [=, checkCourseOpenSession](int) {
        if (checkCourseOpenSession && *checkCourseOpenSession) (*checkCourseOpenSession)();
    });

    if (shell) {
        QObject::connect(shell, &AppShell::pageChanged, page, [=, checkCourseOpenSession](const QString &cur) {
            if (cur == "Open session" && checkCourseOpenSession && *checkCourseOpenSession) {
                (*checkCourseOpenSession)();
            }
        });
    }

    (*checkCourseOpenSession)();

    // Open button logic
    QObject::connect(btnOpen, &QPushButton::clicked, page, [=] {
        if (!validateInputs()) return;
        const int cid = courseBox->currentData().toInt();
        const auto res = SessionService::instance().openSession(
            cid, userId, spinDuration->value(), spinPresent->value(), spinLate->value());
        if (!res.ok) {
            statusLabel->setText("❌ " + res.message);
            statusLabel->setStyleSheet("color: #DC2626; font-weight: bold; font-size: 13px;");
            return;
        }

        statusLabel->setText("✓ " + res.message);
        statusLabel->setStyleSheet("color: #15803D; font-weight: bold; font-size: 13px;");
        if (navCallback) {
            navCallback("Live session");
        }
    });

    layout->addStretch();
    return page;
}

QWidget *lecturerLiveSessionPage(int userId, std::function<void(const QString&)> navCallback = nullptr, AppShell *shell = nullptr) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    // Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Phòng điểm danh trực tiếp (Live Session) 📡");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Mã 6 chữ số tự động đổi sau mỗi 60 giây. Sinh viên nhập mã này trên thiết bị để điểm danh.");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    auto *liveContainer = new QWidget;
    auto *liveLayout = new QVBoxLayout(liveContainer);
    liveLayout->setContentsMargins(0, 0, 0, 0);
    liveLayout->setSpacing(16);
    pageLayout->addWidget(liveContainer, 1);

    auto currentSessionId = std::make_shared<int>(0);
    auto activeCourseCode = std::make_shared<QString>();
    auto activeCourseName = std::make_shared<QString>();

    auto lblCodeNumberPtr = std::make_shared<QLabel*>(nullptr);
    auto lblBadgeLivePtr = std::make_shared<QLabel*>(nullptr);
    auto btnCloseSessionPtr = std::make_shared<QPushButton*>(nullptr);
    auto cardPresentValPtr = std::make_shared<QLabel*>(nullptr);
    auto cardLateValPtr = std::make_shared<QLabel*>(nullptr);
    auto cardPendingValPtr = std::make_shared<QLabel*>(nullptr);
    auto studentTablePtr = std::make_shared<QTableWidget*>(nullptr);
    auto enrolledStudentsList = std::make_shared<std::vector<User>>();

    auto refreshLiveTable = std::make_shared<std::function<void()>>();
    *refreshLiveTable = [=]() {
        const int sessId = *currentSessionId;
        if (sessId <= 0 || !*studentTablePtr) return;

        const auto recs = AttendanceRepository{}.findBySession(sessId);
        int pres = 0, lt = 0;
        for (const auto &r : recs) {
            if (r.status == AttendanceStatus::PRESENT) pres++;
            else if (r.status == AttendanceStatus::LATE) lt++;
        }
        const int totalEnrolled = static_cast<int>(enrolledStudentsList->size());
        int pend = totalEnrolled - (pres + lt);
        if (pend < 0) pend = 0;

        if (*cardPresentValPtr) (*cardPresentValPtr)->setText(QString::number(pres));
        if (*cardLateValPtr) (*cardLateValPtr)->setText(QString::number(lt));
        if (*cardPendingValPtr) (*cardPendingValPtr)->setText(QString::number(pend));

        auto *stTable = *studentTablePtr;
        stTable->setRowCount(totalEnrolled);
        for (int i = 0; i < totalEnrolled; ++i) {
            const auto &st = (*enrolledStudentsList)[i];
            auto *itemIdx = new QTableWidgetItem(QString::number(i + 1));
            itemIdx->setTextAlignment(Qt::AlignCenter);
            stTable->setItem(i, 0, itemIdx);

            auto *itemCode = new QTableWidgetItem(st.code);
            itemCode->setTextAlignment(Qt::AlignCenter);
            stTable->setItem(i, 1, itemCode);

            auto *itemName = new QTableWidgetItem(st.fullName);
            itemName->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            stTable->setItem(i, 2, itemName);

            bool checked = false;
            for (const auto &r : recs) {
                if (r.studentUserId == st.userId) {
                    checked = true;
                    stTable->setCellWidget(i, 3, createStatusBadgeWidget(r.status));
                    auto *itemTime = new QTableWidgetItem(r.checkInTime.isValid() ? r.checkInTime.toString("HH:mm:ss") : "—");
                    itemTime->setTextAlignment(Qt::AlignCenter);
                    stTable->setItem(i, 4, itemTime);
                    break;
                }
            }
            if (!checked) {
                auto *badgeUnchecked = new QLabel("Chưa điểm danh");
                badgeUnchecked->setStyleSheet("background: #F1F5F9; color: #64748B; font-size: 11px; font-weight: 600; border-radius: 4px; padding: 2px 8px;");
                auto *w = new QWidget;
                auto *l = new QHBoxLayout(w);
                l->setContentsMargins(0, 0, 0, 0);
                l->setAlignment(Qt::AlignCenter);
                l->addWidget(badgeUnchecked);
                stTable->setCellWidget(i, 3, w);

                auto *itemTime = new QTableWidgetItem("—");
                itemTime->setTextAlignment(Qt::AlignCenter);
                stTable->setItem(i, 4, itemTime);
            }
        }
    };

    auto renderView = std::make_shared<std::function<void(int)>>();
    *renderView = [=, renderView, refreshLiveTable](int targetSessionId) {
        while (auto *child = liveLayout->takeAt(0)) {
            if (child->widget()) delete child->widget();
            delete child;
        }
        *lblCodeNumberPtr = nullptr;
        *lblBadgeLivePtr = nullptr;
        *btnCloseSessionPtr = nullptr;
        *cardPresentValPtr = nullptr;
        *cardLateValPtr = nullptr;
        *cardPendingValPtr = nullptr;
        *studentTablePtr = nullptr;
        enrolledStudentsList->clear();

        CourseRepository courseRepo;
        SessionRepository sessionRepo;
        const auto taught = courseRepo.findByLecturer(userId);

        std::optional<AttendanceSession> activeSession;
        Course activeCourse;

        if (targetSessionId > 0) {
            auto sOpt = SessionService::instance().getSessionById(targetSessionId);
            if (sOpt.has_value()) {
                activeSession = sOpt;
                for (const auto &c : taught) {
                    if (c.courseId == sOpt->courseId) {
                        activeCourse = c;
                        break;
                    }
                }
            }
        }

        if (!activeSession.has_value()) {
            for (const auto &c : taught) {
                auto open = sessionRepo.findOpenSessionForCourse(c.courseId);
                if (open.has_value()) {
                    activeSession = open;
                    activeCourse = c;
                    break;
                }
            }
        }

        if (!activeSession.has_value()) {
            *currentSessionId = 0;
            auto *emptyCard = new QFrame;
            emptyCard->setObjectName("overviewCard");
            auto *emptyLay = new QVBoxLayout(emptyCard);
            emptyLay->setContentsMargins(32, 36, 32, 36);
            emptyLay->setSpacing(16);
            emptyLay->setAlignment(Qt::AlignCenter);

            auto *lblIcon = new QLabel("🕒");
            lblIcon->setStyleSheet("font-size: 48px; background: transparent;");
            lblIcon->setAlignment(Qt::AlignCenter);
            emptyLay->addWidget(lblIcon);

            auto *lblEmptyTitle = new QLabel("Hiện không có phiên điểm danh nào đang mở");
            lblEmptyTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #112D55; background: transparent;");
            lblEmptyTitle->setAlignment(Qt::AlignCenter);
            emptyLay->addWidget(lblEmptyTitle);

            auto *lblEmptySub = new QLabel("Hãy chọn lớp và mở phiên điểm danh mới để bắt đầu cấp mã cho sinh viên.");
            lblEmptySub->setStyleSheet("font-size: 13px; color: #64748B; background: transparent;");
            lblEmptySub->setAlignment(Qt::AlignCenter);
            emptyLay->addWidget(lblEmptySub);

            if (navCallback) {
                auto *btnGoOpen = new QPushButton("Mở phiên điểm danh ngay →");
                btnGoOpen->setObjectName("btnPrimary");
                btnGoOpen->setCursor(Qt::PointingHandCursor);
                btnGoOpen->setFixedWidth(240);
                emptyLay->addWidget(btnGoOpen, 0, Qt::AlignCenter);
                QObject::connect(btnGoOpen, &QPushButton::clicked, page, [navCallback] {
                    navCallback("Open session");
                });
            }
            liveLayout->addWidget(emptyCard);
            liveLayout->addStretch();
            return;
        }

        const int sessId = activeSession->sessionId;
        *currentSessionId = sessId;
        *activeCourseCode = activeCourse.code;
        *activeCourseName = activeCourse.name;

        *enrolledStudentsList = courseRepo.getEnrolledStudents(activeSession->courseId);
        const int totalEnrolled = static_cast<int>(enrolledStudentsList->size());

        // 1. Session Info & Giant Rotating Code Box
        auto *sessionCard = new QFrame;
        sessionCard->setObjectName("overviewCard");
        auto *sLay = new QVBoxLayout(sessionCard);
        sLay->setContentsMargins(24, 20, 24, 20);
        sLay->setSpacing(14);

        auto *cHeaderRow = new QHBoxLayout;
        auto *cTitle = new QLabel(QString("Môn: %1 - %2 · Phiên #%3").arg(activeCourse.code, activeCourse.name).arg(sessId));
        cTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #112D55; background: transparent;");
        cHeaderRow->addWidget(cTitle);
        cHeaderRow->addStretch();

        auto *badgeLive = new QLabel(activeSession->status == SessionStatus::OPEN ? "● ĐANG DIỄN RA" : "● ĐÃ KẾT THÚC");
        badgeLive->setStyleSheet(activeSession->status == SessionStatus::OPEN
            ? "background: #DCFCE7; color: #15803D; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 4px 10px;"
            : "background: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 4px 10px;");
        cHeaderRow->addWidget(badgeLive);
        *lblBadgeLivePtr = badgeLive;
        sLay->addLayout(cHeaderRow);

        auto *timeInfo = new QLabel(QString("Bắt đầu lúc: %1 | Đúng giờ: 0–%2p | Đi muộn: đến %3p | Thời lượng: %4p")
            .arg(activeSession->startTime.toString("HH:mm:ss dd/MM/yyyy"))
            .arg(activeSession->presentWindowMinutes)
            .arg(activeSession->lateWindowMinutes)
            .arg(activeSession->durationMinutes));
        timeInfo->setStyleSheet("color: #64748B; font-size: 12px; background: transparent;");
        sLay->addWidget(timeInfo);

        // GIANT ROTATING CODE BOX
        auto *codeBox = new QFrame;
        codeBox->setObjectName("liveCodeBox");
        auto *codeBoxLay = new QVBoxLayout(codeBox);
        codeBoxLay->setContentsMargins(20, 18, 20, 18);
        codeBoxLay->setSpacing(8);
        codeBoxLay->setAlignment(Qt::AlignCenter);

        auto *codeNumber = new QLabel(activeSession->status == SessionStatus::OPEN ? activeSession->sessionCode : "CLOSED");
        codeNumber->setObjectName("liveCodeNumber");
        codeNumber->setAlignment(Qt::AlignCenter);
        codeBoxLay->addWidget(codeNumber);
        *lblCodeNumberPtr = codeNumber;

        auto *codeHint = new QLabel(activeSession->status == SessionStatus::OPEN
            ? "🔄 Mã 6 số tự động cập nhật sau mỗi 60 giây. Sinh viên dùng mã này để xác nhận điểm danh."
            : "⚠️ Phiên điểm danh này đã kết thúc.");
        codeHint->setObjectName("liveCodeHint");
        codeHint->setAlignment(Qt::AlignCenter);
        codeBoxLay->addWidget(codeHint);

        sLay->addWidget(codeBox);
        liveLayout->addWidget(sessionCard);

        // 2. Real-time Live Stats KPIs
        auto *kpiRow = new QHBoxLayout;
        kpiRow->setSpacing(16);

        auto *cardPresent = createKpiCard("Đúng giờ (Present)", "0", "Sinh viên check-in đúng giờ", "#15803D");
        auto *cardLate = createKpiCard("Đi muộn (Late)", "0", "Check-in trong cửa sổ muộn", "#D97706");
        auto *cardPending = createKpiCard("Chưa điểm danh (Pending)", QString::number(totalEnrolled), "Sẽ ghi nhận Vắng nếu hết giờ", "#B91C1C");
        auto *cardTotal = createKpiCard("Sĩ số (Enrolled)", QString::number(totalEnrolled), "Tổng sinh viên lớp học phần", "#112D55");

        *cardPresentValPtr = cardPresent->findChild<QLabel*>("kpiValue");
        *cardLateValPtr = cardLate->findChild<QLabel*>("kpiValue");
        *cardPendingValPtr = cardPending->findChild<QLabel*>("kpiValue");

        kpiRow->addWidget(cardPresent);
        kpiRow->addWidget(cardLate);
        kpiRow->addWidget(cardPending);
        kpiRow->addWidget(cardTotal);
        liveLayout->addLayout(kpiRow);

        // 3. Student Check-in Status Table
        auto *tableCard = new QFrame;
        tableCard->setObjectName("overviewCard");
        auto *tLay = new QVBoxLayout(tableCard);
        tLay->setContentsMargins(20, 18, 20, 18);
        tLay->setSpacing(12);

        auto *tHeader = new QHBoxLayout;
        auto *tTitle = new QLabel("Danh sách sinh viên & Trạng thái check-in");
        tTitle->setObjectName("overviewCardTitle");
        tHeader->addWidget(tTitle);
        tHeader->addStretch();
        tLay->addLayout(tHeader);

        auto *studentTable = new QTableWidget;
        studentTable->setColumnCount(5);
        studentTable->setHorizontalHeaderLabels({"STT", "Mã SV", "Họ và tên", "Trạng thái", "Giờ check-in"});
        studentTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        studentTable->setColumnWidth(0, 50);
        studentTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        studentTable->setColumnWidth(1, 130);
        studentTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        studentTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
        studentTable->setColumnWidth(3, 140);
        studentTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
        studentTable->setColumnWidth(4, 150);
        studentTable->verticalHeader()->setVisible(false);
        studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        studentTable->setMinimumHeight(240);
        tLay->addWidget(studentTable);
        *studentTablePtr = studentTable;

        // 4. Action Buttons (Close Session with Absent Confirmation)
        auto *btnRow = new QHBoxLayout;
        btnRow->setSpacing(14);

        auto *btnCloseSession = new QPushButton("🛑 Đóng phiên điểm danh (Kết thúc)");
        btnCloseSession->setObjectName("btnDanger");
        btnCloseSession->setCursor(Qt::PointingHandCursor);
        btnCloseSession->setEnabled(activeSession->status == SessionStatus::OPEN);
        btnRow->addWidget(btnCloseSession);
        *btnCloseSessionPtr = btnCloseSession;

        if (navCallback) {
            auto *btnReport = new QPushButton("Xem báo cáo chi tiết →");
            btnReport->setObjectName("btnOutline");
            btnReport->setCursor(Qt::PointingHandCursor);
            btnRow->addWidget(btnReport);
            QObject::connect(btnReport, &QPushButton::clicked, page, [navCallback] {
                navCallback("Attendance report");
            });
        }
        btnRow->addStretch();
        tLay->addLayout(btnRow);

        QObject::connect(btnCloseSession, &QPushButton::clicked, page, [=, refreshLiveTable] {
            const auto recs = AttendanceRepository{}.findBySession(sessId);
            int checkedCount = 0;
            for (const auto &r : recs) {
                if (r.status == AttendanceStatus::PRESENT || r.status == AttendanceStatus::LATE)
                    checkedCount++;
            }
            int absentWillBe = totalEnrolled - checkedCount;
            if (absentWillBe < 0) absentWillBe = 0;

            const QString confirmMsg = QString(
                "Bạn có chắc chắn muốn đóng phiên điểm danh #%1?\n\n"
                "• Sĩ số lớp: %2 sinh viên\n"
                "• Đã điểm danh: %3 sinh viên\n"
                "• Chưa điểm danh: %4 sinh viên\n\n"
                "⚠️ HỆ THỐNG SẼ TỰ ĐỘNG GHI NHẬN VẮNG (ABSENT) CHO %4 SINH VIÊN CHƯA CHECK-IN.")
                .arg(sessId).arg(totalEnrolled).arg(checkedCount).arg(absentWillBe);

            const auto reply = QMessageBox::question(page, "Xác nhận đóng phiên điểm danh",
                confirmMsg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                const auto res = SessionService::instance().closeSession(sessId);
                if (res.ok) {
                    QMessageBox::information(page, "Đã đóng phiên điểm danh",
                        QString("Phiên điểm danh #%1 đã đóng thành công.\n%2 sinh viên đã được ghi nhận vắng.")
                            .arg(sessId).arg(absentWillBe));
                    if (*lblBadgeLivePtr) {
                        (*lblBadgeLivePtr)->setText("● ĐÃ KẾT THÚC");
                        (*lblBadgeLivePtr)->setStyleSheet("background: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 4px 10px;");
                    }
                    if (*lblCodeNumberPtr) (*lblCodeNumberPtr)->setText("CLOSED");
                    if (*btnCloseSessionPtr) (*btnCloseSessionPtr)->setEnabled(false);
                    if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
                    if (navCallback) {
                        navCallback("Attendance report");
                    }
                } else {
                    QMessageBox::warning(page, "Lỗi", res.message);
                }
            }
        });

        liveLayout->addWidget(tableCard);
        liveLayout->addStretch();

        // Initial table load
        if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
    };

    // Initial render
    (*renderView)(0);

    // Signal connections:
    QObject::connect(&SessionService::instance(), &SessionService::sessionOpened, page, [=, renderView](int newSessId) {
        CourseRepository cr;
        const auto taught = cr.findByLecturer(userId);
        auto sessOpt = SessionService::instance().getSessionById(newSessId);
        if (sessOpt.has_value()) {
            for (const auto &c : taught) {
                if (c.courseId == sessOpt->courseId) {
                    (*renderView)(newSessId);
                    break;
                }
            }
        }
    });

    QObject::connect(&SessionService::instance(), &SessionService::codeRotated, page,
        [=, refreshLiveTable](int rotatedSessId, const QString &newCode) {
            if (*currentSessionId == rotatedSessId) {
                if (*lblCodeNumberPtr) (*lblCodeNumberPtr)->setText(newCode);
                if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
            }
        });

    QObject::connect(&SessionService::instance(), &SessionService::sessionClosed, page,
        [=, refreshLiveTable](int closedSessId) {
            if (*currentSessionId == closedSessId) {
                if (*lblBadgeLivePtr) {
                    (*lblBadgeLivePtr)->setText("● ĐÃ KẾT THÚC");
                    (*lblBadgeLivePtr)->setStyleSheet("background: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 4px 10px;");
                }
                if (*lblCodeNumberPtr) (*lblCodeNumberPtr)->setText("CLOSED");
                if (*btnCloseSessionPtr) (*btnCloseSessionPtr)->setEnabled(false);
                if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
            }
        });

    // 1500ms Polling Timer
    auto *livePollTimer = new QTimer(page);
    livePollTimer->setInterval(1500);
    QObject::connect(livePollTimer, &QTimer::timeout, page, [=, renderView, refreshLiveTable]() {
        if (!page->isVisible()) return;

        if (*currentSessionId > 0) {
            const auto sOpt = SessionService::instance().getSessionById(*currentSessionId);
            if (!sOpt.has_value() || sOpt->status == SessionStatus::CLOSED) {
                if (*lblBadgeLivePtr && (*lblBadgeLivePtr)->text() != "● ĐÃ KẾT THÚC") {
                    (*lblBadgeLivePtr)->setText("● ĐÃ KẾT THÚC");
                    (*lblBadgeLivePtr)->setStyleSheet("background: #FEE2E2; color: #DC2626; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 4px 10px;");
                }
                if (*lblCodeNumberPtr && (*lblCodeNumberPtr)->text() != "CLOSED") {
                    (*lblCodeNumberPtr)->setText("CLOSED");
                }
                if (*btnCloseSessionPtr && (*btnCloseSessionPtr)->isEnabled()) {
                    (*btnCloseSessionPtr)->setEnabled(false);
                }
                if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
            } else {
                if (*lblCodeNumberPtr && (*lblCodeNumberPtr)->text() != sOpt->sessionCode) {
                    (*lblCodeNumberPtr)->setText(sOpt->sessionCode);
                }
                if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
            }
        } else {
            CourseRepository cr;
            SessionRepository sr;
            const auto taught = cr.findByLecturer(userId);
            for (const auto &c : taught) {
                auto open = sr.findOpenSessionForCourse(c.courseId);
                if (open.has_value()) {
                    (*renderView)(open->sessionId);
                    break;
                }
            }
        }
    });
    livePollTimer->start();

    if (shell) {
        QObject::connect(shell, &AppShell::pageChanged, page, [=, renderView, refreshLiveTable](const QString &cur) {
            if (cur == "Live session") {
                if (*currentSessionId > 0) {
                    const auto sOpt = SessionService::instance().getSessionById(*currentSessionId);
                    if (!sOpt.has_value() || sOpt->status == SessionStatus::CLOSED) {
                        (*renderView)(0);
                    } else {
                        if (refreshLiveTable && *refreshLiveTable) (*refreshLiveTable)();
                    }
                } else {
                    (*renderView)(0);
                }
            }
        });
    }

    return page;
}

QWidget *lecturerReportPage(int userId, AppShell *shell = nullptr) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    // Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Báo cáo chuyên cần & Điều chỉnh học vụ (Attendance Report)");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Học kỳ 1 (2026 - 2027) · Chọn lớp học phần và phiên học để xem và điều chỉnh điểm danh");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    CourseRepository courses;
    const auto taught = courses.findByLecturer(userId);

    if (taught.empty()) {
        auto *emptyMsg = new QLabel("Bạn chưa được phân công giảng dạy môn học nào trong học kỳ này.");
        emptyMsg->setStyleSheet("color: #64748B; font-style: italic; padding: 24px;");
        pageLayout->addWidget(emptyMsg);
        pageLayout->addStretch();
        return page;
    }

    // Controls Row: Course Combo, Session Combo, Search Edit, Audit Log button
    auto *controlsCard = new QFrame;
    controlsCard->setObjectName("overviewCard");
    auto *cLay = new QHBoxLayout(controlsCard);
    cLay->setContentsMargins(18, 14, 18, 14);
    cLay->setSpacing(12);

    auto *lblCourse = new QLabel("Lớp HP:");
    lblCourse->setStyleSheet("font-weight: 700; color: #112D55;");
    cLay->addWidget(lblCourse);

    auto *courseCombo = new QComboBox;
    courseCombo->setFixedHeight(36);
    for (const auto &c : taught) {
        courseCombo->addItem(QString("%1 - %2").arg(c.code, c.name), c.courseId);
    }
    cLay->addWidget(courseCombo, 2);

    auto *lblSession = new QLabel("Phiên học:");
    lblSession->setStyleSheet("font-weight: 700; color: #112D55;");
    cLay->addWidget(lblSession);

    auto *sessionCombo = new QComboBox;
    sessionCombo->setFixedHeight(36);
    cLay->addWidget(sessionCombo, 2);

    auto *searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("🔍 Tìm theo tên hoặc mã SV...");
    searchEdit->setFixedHeight(36);
    searchEdit->setStyleSheet("QLineEdit { background: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 6px; padding: 4px 10px; font-size: 13px; }");
    cLay->addWidget(searchEdit, 2);

    auto *btnAuditLogs = new QPushButton("📜 Lịch sử chỉnh sửa");
    btnAuditLogs->setObjectName("btnOutline");
    btnAuditLogs->setFixedHeight(36);
    btnAuditLogs->setCursor(Qt::PointingHandCursor);
    cLay->addWidget(btnAuditLogs);

    pageLayout->addWidget(controlsCard);

    // 4 KPI Cards for the selected session
    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(16);

    auto *cardEnrolled = createKpiCard("Sĩ số (Enrolled)", "0", "Sinh viên đăng ký lớp");
    auto *cardPresent = createKpiCard("Đúng giờ (Present)", "0", "Check-in đúng giờ", "#15803D");
    auto *cardLate = createKpiCard("Đi muộn (Late)", "0", "Check-in cửa sổ muộn", "#D97706");
    auto *cardAbsentExcused = createKpiCard("Vắng / Phép (Absent)", "0", "Vắng mặt hoặc có phép", "#B91C1C");

    kpiRow->addWidget(cardEnrolled);
    kpiRow->addWidget(cardPresent);
    kpiRow->addWidget(cardLate);
    kpiRow->addWidget(cardAbsentExcused);
    pageLayout->addLayout(kpiRow);

    // Student Records Table
    auto *tableCard = new QFrame;
    tableCard->setObjectName("overviewCard");
    auto *tLay = new QVBoxLayout(tableCard);
    tLay->setContentsMargins(20, 18, 20, 18);
    tLay->setSpacing(12);

    auto *table = new QTableWidget;
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"STT", "Mã SV", "Họ và tên", "Trạng thái", "Giờ check-in", "Thao tác"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->setColumnWidth(0, 50);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->setColumnWidth(1, 130);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    table->setColumnWidth(3, 140);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->setColumnWidth(4, 150);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(5, 130);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setMinimumHeight(280);
    tLay->addWidget(table);
    pageLayout->addWidget(tableCard);

    // Refresh function
    auto refreshSessionData = std::make_shared<std::function<void()>>();
    *refreshSessionData = [=, refreshSessionData] {
        if (sessionCombo->currentIndex() < 0) {
            table->setRowCount(0);
            return;
        }
        const int currentSessionId = sessionCombo->currentData().toInt();
        const auto records = AttendanceService::instance().getSessionRecords(currentSessionId);

        int pres = 0, lt = 0, ab = 0, exc = 0;
        for (const auto &r : records) {
            if (r.status == AttendanceStatus::PRESENT) pres++;
            else if (r.status == AttendanceStatus::LATE) lt++;
            else if (r.status == AttendanceStatus::ABSENT) ab++;
            else if (r.status == AttendanceStatus::EXCUSED) exc++;
        }

        auto findValLabel = [](QWidget *cd) -> QLabel* {
            return cd->findChild<QLabel*>("kpiValue");
        };
        if (auto *l = findValLabel(cardEnrolled)) l->setText(QString::number(records.size()));
        if (auto *l = findValLabel(cardPresent)) l->setText(QString::number(pres));
        if (auto *l = findValLabel(cardLate)) l->setText(QString::number(lt));
        if (auto *l = findValLabel(cardAbsentExcused)) l->setText(QString("%1 / %2").arg(ab).arg(exc));

        const QString filterText = searchEdit->text().trimmed().toLower();
        std::vector<AttendanceRecord> filtered;
        for (const auto &r : records) {
            if (filterText.isEmpty() || r.studentName.toLower().contains(filterText) || r.studentCode.toLower().contains(filterText)) {
                filtered.push_back(r);
            }
        }

        table->setRowCount(static_cast<int>(filtered.size()));
        for (int i = 0; i < static_cast<int>(filtered.size()); ++i) {
            const auto &r = filtered[i];
            auto *itemIdx = new QTableWidgetItem(QString::number(i + 1));
            itemIdx->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 0, itemIdx);

            auto *itemCode = new QTableWidgetItem(r.studentCode);
            itemCode->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 1, itemCode);

            auto *itemName = new QTableWidgetItem(r.studentName);
            itemName->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            table->setItem(i, 2, itemName);

            table->setCellWidget(i, 3, createStatusBadgeWidget(r.status));

            auto *itemTime = new QTableWidgetItem(r.checkInTime.isValid() ? r.checkInTime.toString("HH:mm:ss") : "—");
            itemTime->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 4, itemTime);

            auto *btnEdit = new QPushButton("Sửa trạng thái");
            btnEdit->setObjectName("linkPrimary");
            btnEdit->setCursor(Qt::PointingHandCursor);
            auto *wBtn = new QWidget;
            auto *lBtn = new QHBoxLayout(wBtn);
            lBtn->setContentsMargins(0, 0, 0, 0);
            lBtn->setAlignment(Qt::AlignCenter);
            lBtn->addWidget(btnEdit);
            table->setCellWidget(i, 5, wBtn);

            QObject::connect(btnEdit, &QPushButton::clicked, page, [=, refreshSessionData, recordId = r.recordId, curStatus = r.status, sName = r.studentName, sCode = r.studentCode] {
                auto *dlg = new QDialog(page);
                dlg->setWindowTitle("Điều chỉnh trạng thái điểm danh");
                dlg->setFixedWidth(420);
                auto *dLay = new QVBoxLayout(dlg);
                dLay->setSpacing(14);
                dLay->setContentsMargins(20, 20, 20, 20);

                auto *lblTarget = new QLabel(QString("Sinh viên: %1 (%2)\nTrạng thái hiện tại: %3")
                    .arg(sName, sCode, AttendanceRecord::statusToString(curStatus)));
                lblTarget->setStyleSheet("font-size: 13px; font-weight: 600; color: #112D55;");
                dLay->addWidget(lblTarget);

                dLay->addWidget(new QLabel("Trạng thái mới:"));
                auto *newStatusCombo = new QComboBox;
                newStatusCombo->addItem("PRESENT (Đúng giờ)", static_cast<int>(AttendanceStatus::PRESENT));
                newStatusCombo->addItem("LATE (Đi muộn)", static_cast<int>(AttendanceStatus::LATE));
                newStatusCombo->addItem("ABSENT (Vắng mặt)", static_cast<int>(AttendanceStatus::ABSENT));
                newStatusCombo->addItem("EXCUSED (Có phép)", static_cast<int>(AttendanceStatus::EXCUSED));
                newStatusCombo->setCurrentIndex(static_cast<int>(curStatus));
                dLay->addWidget(newStatusCombo);

                dLay->addWidget(new QLabel("Lý do điều chỉnh (Bắt buộc lưu audit log):"));
                auto *reasonEdit = new QTextEdit;
                reasonEdit->setPlaceholderText("Nhập lý do thay đổi (ví dụ: Sinh viên có giấy phép y tế, lỗi thiết bị...)");
                reasonEdit->setFixedHeight(80);
                dLay->addWidget(reasonEdit);

                auto *btnBox = new QHBoxLayout;
                btnBox->addStretch();
                auto *btnCancel = new QPushButton("Hủy bỏ");
                btnCancel->setObjectName("btnOutline");
                btnBox->addWidget(btnCancel);
                auto *btnSave = new QPushButton("Lưu thay đổi");
                btnSave->setObjectName("btnPrimary");
                btnBox->addWidget(btnSave);
                dLay->addLayout(btnBox);

                QObject::connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
                QObject::connect(btnSave, &QPushButton::clicked, dlg, [=, refreshSessionData] {
                    const QString reason = reasonEdit->toPlainText().trimmed();
                    if (reason.isEmpty()) {
                        QMessageBox::warning(dlg, "Thiếu thông tin", "Vui lòng nhập lý do điều chỉnh để lưu vào lịch sử kiểm toán học vụ.");
                        return;
                    }
                    const auto selectedStatus = static_cast<AttendanceStatus>(newStatusCombo->currentData().toInt());
                    const bool ok = AttendanceService::instance().editRecordStatus(recordId, selectedStatus, userId, reason);
                    if (ok) {
                        dlg->accept();
                        if (refreshSessionData && *refreshSessionData) (*refreshSessionData)();
                    } else {
                        QMessageBox::warning(dlg, "Lỗi", "Không thể cập nhật trạng thái điểm danh.");
                    }
                });

                dlg->exec();
            });
        }
    };

    // Populate sessions when course changes
    auto populateSessionsForCourse = std::make_shared<std::function<void(int, int)>>();
    *populateSessionsForCourse = [=, populateSessionsForCourse, refreshSessionData](int courseId, int preferredSessionId) {
        const int oldId = (preferredSessionId > 0) ? preferredSessionId : sessionCombo->currentData().toInt();
        sessionCombo->blockSignals(true);
        sessionCombo->clear();
        const auto sessions = SessionRepository{}.findByCourse(courseId);
        int selectIdx = -1;
        for (int i = 0; i < static_cast<int>(sessions.size()); ++i) {
            const auto &s = sessions[i];
            const QString stStr = (s.status == SessionStatus::OPEN) ? "ĐANG MỞ" : "ĐÃ ĐÓNG";
            sessionCombo->addItem(QString("Phiên #%1 · %2 (%3)")
                .arg(s.sessionId).arg(s.startTime.toString("dd/MM/yyyy HH:mm")).arg(stStr), s.sessionId);
            if (s.sessionId == oldId) {
                selectIdx = i;
            }
        }
        sessionCombo->blockSignals(false);
        if (selectIdx != -1) {
            sessionCombo->setCurrentIndex(selectIdx);
        } else if (sessionCombo->count() > 0) {
            sessionCombo->setCurrentIndex(0);
        }
        if (refreshSessionData && *refreshSessionData) (*refreshSessionData)();
    };

    QObject::connect(courseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=, populateSessionsForCourse](int) {
        if (populateSessionsForCourse && *populateSessionsForCourse) {
            (*populateSessionsForCourse)(courseCombo->currentData().toInt(), 0);
        }
    });

    QObject::connect(sessionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=, refreshSessionData](int) {
        if (refreshSessionData && *refreshSessionData) (*refreshSessionData)();
    });

    QObject::connect(searchEdit, &QLineEdit::textChanged, page, [=, refreshSessionData](const QString&) {
        if (refreshSessionData && *refreshSessionData) (*refreshSessionData)();
    });

    QObject::connect(&SessionService::instance(), &SessionService::sessionOpened, page, [=, populateSessionsForCourse](int newSessId) {
        if (courseCombo->currentIndex() >= 0 && populateSessionsForCourse && *populateSessionsForCourse) {
            (*populateSessionsForCourse)(courseCombo->currentData().toInt(), newSessId);
        }
    });
    QObject::connect(&SessionService::instance(), &SessionService::sessionClosed, page, [=, populateSessionsForCourse](int closedSessId) {
        if (courseCombo->currentIndex() >= 0 && populateSessionsForCourse && *populateSessionsForCourse) {
            (*populateSessionsForCourse)(courseCombo->currentData().toInt(), closedSessId);
        }
    });

    if (shell) {
        QObject::connect(shell, &AppShell::pageChanged, page, [=, populateSessionsForCourse](const QString &cur) {
            if (cur == "Attendance report" && courseCombo->currentIndex() >= 0 && populateSessionsForCourse && *populateSessionsForCourse) {
                (*populateSessionsForCourse)(courseCombo->currentData().toInt(), 0);
            }
        });
    }

    // Audit logs viewer dialog
    QObject::connect(btnAuditLogs, &QPushButton::clicked, page, [=] {
        if (sessionCombo->currentIndex() < 0) return;
        const int sessId = sessionCombo->currentData().toInt();
        const auto logs = AttendanceService::instance().getChangeLogsForSession(sessId);

        auto *dlg = new QDialog(page);
        dlg->setWindowTitle(QString("Lịch sử kiểm toán chỉnh sửa - Phiên #%1").arg(sessId));
        dlg->resize(760, 420);
        auto *lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(20, 20, 20, 20);
        lay->setSpacing(12);

        auto *dlgTitle = new QLabel(QString("Lịch sử thay đổi điểm danh (Phiên #%1)").arg(sessId));
        dlgTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #112D55;");
        lay->addWidget(dlgTitle);

        auto *logTable = new QTableWidget;
        logTable->setColumnCount(6);
        logTable->setHorizontalHeaderLabels({"Thời gian", "Mã SV", "Sinh viên", "Thay đổi", "Người sửa", "Lý do"});
        logTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        logTable->setColumnWidth(0, 130);
        logTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        logTable->setColumnWidth(1, 110);
        logTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
        logTable->setColumnWidth(2, 130);
        logTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
        logTable->setColumnWidth(3, 110);
        logTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
        logTable->setColumnWidth(4, 120);
        logTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
        logTable->verticalHeader()->setVisible(false);
        logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        logTable->setRowCount(static_cast<int>(logs.size()));

        for (int i = 0; i < static_cast<int>(logs.size()); ++i) {
            const auto &lg = logs[i];
            logTable->setItem(i, 0, new QTableWidgetItem(lg.changedAt.toString("dd/MM/yyyy HH:mm:ss")));
            logTable->setItem(i, 1, new QTableWidgetItem(lg.studentCode));
            logTable->setItem(i, 2, new QTableWidgetItem(lg.studentName));
            logTable->setItem(i, 3, new QTableWidgetItem(QString("%1 → %2").arg(lg.oldStatus, lg.newStatus)));
            logTable->setItem(i, 4, new QTableWidgetItem(lg.lecturerName.isEmpty() ? "Giảng viên" : lg.lecturerName));
            logTable->setItem(i, 5, new QTableWidgetItem(lg.reason));
        }

        lay->addWidget(logTable);
        if (logs.empty()) {
            auto *empty = new QLabel("Chưa có lượt điều chỉnh nào được ghi nhận cho phiên học này.");
            empty->setStyleSheet("color: #64748B; font-style: italic;");
            lay->addWidget(empty);
        }

        auto *btnClose = new QPushButton("Đóng");
        btnClose->setObjectName("btnOutline");
        QObject::connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
        lay->addWidget(btnClose, 0, Qt::AlignRight);

        dlg->exec();
    });

    if (!taught.empty()) {
        if (populateSessionsForCourse && *populateSessionsForCourse) {
            (*populateSessionsForCourse)(taught.front().courseId, 0);
        }
    }

    pageLayout->addStretch();
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
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    // Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Quản lý tài khoản người dùng (Account Management)");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Quản lý tài khoản Sinh viên, Giảng viên, kích hoạt hoặc vô hiệu hóa tài khoản");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    // 4 KPI Cards
    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(16);
    auto *cardTotal = createKpiCard("Tổng tài khoản", "0", "Hệ thống");
    auto *cardStudents = createKpiCard("Sinh viên", "0", "Đang theo học", "#1E40AF");
    auto *cardLecturers = createKpiCard("Giảng viên", "0", "Đang công tác", "#0F766E");
    auto *cardActive = createKpiCard("Đang kích hoạt", "0", "Active accounts", "#15803D");

    kpiRow->addWidget(cardTotal);
    kpiRow->addWidget(cardStudents);
    kpiRow->addWidget(cardLecturers);
    kpiRow->addWidget(cardActive);
    pageLayout->addLayout(kpiRow);

    // Controls Row: Search & Role Filter
    auto *filterCard = new QFrame;
    filterCard->setObjectName("overviewCard");
    auto *fLay = new QHBoxLayout(filterCard);
    fLay->setContentsMargins(18, 14, 18, 14);
    fLay->setSpacing(12);

    auto *searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("🔍 Tìm theo tên, mã hoặc email...");
    searchEdit->setFixedHeight(36);
    searchEdit->setStyleSheet("QLineEdit { background: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 6px; padding: 4px 10px; font-size: 13px; }");
    fLay->addWidget(searchEdit, 3);

    auto *lblRole = new QLabel("Vai trò:");
    lblRole->setStyleSheet("font-weight: 700; color: #112D55;");
    fLay->addWidget(lblRole);

    auto *roleCombo = new QComboBox;
    roleCombo->setFixedHeight(36);
    roleCombo->addItems({"Tất cả vai trò", "STUDENT", "LECTURER", "ADMIN"});
    fLay->addWidget(roleCombo, 1);

    auto *lblStatus = new QLabel("Trạng thái:");
    lblStatus->setStyleSheet("font-weight: 700; color: #112D55;");
    fLay->addWidget(lblStatus);

    auto *statusCombo = new QComboBox;
    statusCombo->setFixedHeight(36);
    statusCombo->addItems({"Tất cả trạng thái", "Đang kích hoạt (Active)", "Vô hiệu hóa (Inactive)"});
    fLay->addWidget(statusCombo, 1);

    pageLayout->addWidget(filterCard);

    // Accounts Table
    auto *tableCard = new QFrame;
    tableCard->setObjectName("overviewCard");
    auto *tLay = new QVBoxLayout(tableCard);
    tLay->setContentsMargins(20, 18, 20, 18);
    tLay->setSpacing(12);

    auto *table = new QTableWidget;
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Mã", "Họ và tên", "Email", "Vai trò", "Trạng thái", "Thao tác"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->setColumnWidth(0, 50);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->setColumnWidth(1, 130);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->setColumnWidth(4, 110);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(5, 110);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->setColumnWidth(6, 140);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setMinimumHeight(240);
    tLay->addWidget(table);
    pageLayout->addWidget(tableCard);

    // Create Account Form
    auto *createCard = new QFrame;
    createCard->setObjectName("overviewCard");
    auto *cLay = new QVBoxLayout(createCard);
    cLay->setContentsMargins(20, 18, 20, 18);
    cLay->setSpacing(12);

    auto *cTitle = new QLabel("➕ Tạo tài khoản mới (Create Account)");
    cTitle->setObjectName("overviewCardTitle");
    cLay->addWidget(cTitle);

    auto *formGrid = new QHBoxLayout;
    formGrid->setSpacing(12);

    auto *inRole = new QComboBox;
    inRole->addItems({"STUDENT", "LECTURER"});
    inRole->setFixedHeight(36);
    formGrid->addWidget(inRole, 1);

    auto *inCode = new QLineEdit;
    inCode->setPlaceholderText("Mã SV / GV (Code)");
    inCode->setFixedHeight(36);
    formGrid->addWidget(inCode, 1);

    auto *inName = new QLineEdit;
    inName->setPlaceholderText("Họ và tên đầy đủ");
    inName->setFixedHeight(36);
    formGrid->addWidget(inName, 2);

    auto *inEmail = new QLineEdit;
    inEmail->setPlaceholderText("Email (vd: user@uth.edu.vn)");
    inEmail->setFixedHeight(36);
    formGrid->addWidget(inEmail, 2);

    auto *inPassword = new QLineEdit;
    inPassword->setPlaceholderText("Mật khẩu khởi tạo");
    inPassword->setEchoMode(QLineEdit::Password);
    inPassword->setFixedHeight(36);
    formGrid->addWidget(inPassword, 1);

    auto *btnCreate = new QPushButton("Tạo tài khoản");
    btnCreate->setObjectName("btnPrimary");
    btnCreate->setFixedHeight(36);
    btnCreate->setCursor(Qt::PointingHandCursor);
    formGrid->addWidget(btnCreate, 1);

    cLay->addLayout(formGrid);

    auto *lblCreateMsg = new QLabel;
    lblCreateMsg->setWordWrap(true);
    cLay->addWidget(lblCreateMsg);

    pageLayout->addWidget(createCard);

    // Refresh function
    auto refreshAccounts = std::make_shared<std::function<void()>>();
    *refreshAccounts = [=, refreshAccounts]() {
        const auto all = UserRepository{}.findAll();
        int stu = 0, lec = 0, act = 0;
        for (const auto &u : all) {
            if (u.role == UserRole::STUDENT) stu++;
            else if (u.role == UserRole::LECTURER) lec++;
            if (u.isActive) act++;
        }

        auto findValLabel = [](QWidget *cd) -> QLabel* {
            return cd->findChild<QLabel*>("kpiValue");
        };
        if (auto *l = findValLabel(cardTotal)) l->setText(QString::number(all.size()));
        if (auto *l = findValLabel(cardStudents)) l->setText(QString::number(stu));
        if (auto *l = findValLabel(cardLecturers)) l->setText(QString::number(lec));
        if (auto *l = findValLabel(cardActive)) l->setText(QString::number(act));

        const QString filterText = searchEdit->text().trimmed().toLower();
        const QString roleFilter = roleCombo->currentText();
        const int statusFilterIdx = statusCombo->currentIndex(); // 0: All, 1: Active, 2: Inactive

        std::vector<User> filtered;
        for (const auto &u : all) {
            if (!filterText.isEmpty() && !u.fullName.toLower().contains(filterText) &&
                !u.code.toLower().contains(filterText) && !u.email.toLower().contains(filterText)) {
                continue;
            }
            if (roleFilter != "Tất cả vai trò" && User::roleToString(u.role) != roleFilter) {
                continue;
            }
            if (statusFilterIdx == 1 && !u.isActive) continue;
            if (statusFilterIdx == 2 && u.isActive) continue;

            filtered.push_back(u);
        }

        table->setRowCount(static_cast<int>(filtered.size()));
        for (int i = 0; i < static_cast<int>(filtered.size()); ++i) {
            const auto &u = filtered[i];
            table->setItem(i, 0, new QTableWidgetItem(QString::number(u.userId)));
            table->setItem(i, 1, new QTableWidgetItem(u.code));
            table->setItem(i, 2, new QTableWidgetItem(u.fullName));
            table->setItem(i, 3, new QTableWidgetItem(u.email));
            table->setItem(i, 4, new QTableWidgetItem(User::roleToString(u.role)));

            table->setCellWidget(i, 5, createActiveBadgeWidget(u.isActive));

            auto *btnAction = new QPushButton(u.isActive ? "Vô hiệu hóa" : "Kích hoạt lại");
            btnAction->setObjectName(u.isActive ? "btnDangerOutline" : "btnOutline");
            btnAction->setCursor(Qt::PointingHandCursor);

            auto *wBtn = new QWidget;
            auto *lBtn = new QHBoxLayout(wBtn);
            lBtn->setContentsMargins(0, 0, 0, 0);
            lBtn->setAlignment(Qt::AlignCenter);
            lBtn->addWidget(btnAction);
            table->setCellWidget(i, 6, wBtn);

            QObject::connect(btnAction, &QPushButton::clicked, page, [=, refreshAccounts, targetId = u.userId, isActive = u.isActive, targetName = u.fullName] {
                if (isActive) {
                    const auto reply = QMessageBox::question(page, "Xác nhận vô hiệu hóa",
                        QString("Bạn có chắc chắn muốn vô hiệu hóa tài khoản '%1'? Người dùng này sẽ không thể đăng nhập vào hệ thống.").arg(targetName),
                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                    if (reply == QMessageBox::Yes) {
                        const auto res = AccountService::deactivate(targetId);
                        if (res.ok) {
                            if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
                        } else {
                            QMessageBox::warning(page, "Lỗi", res.message);
                        }
                    }
                } else {
                    const auto reply = QMessageBox::question(page, "Xác nhận kích hoạt lại",
                        QString("Kích hoạt lại tài khoản '%1'?").arg(targetName),
                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                    if (reply == QMessageBox::Yes) {
                        const auto res = AccountService::reactivate(targetId);
                        if (res.ok) {
                            if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
                        } else {
                            QMessageBox::warning(page, "Lỗi", res.message);
                        }
                    }
                }
            });
        }
    };

    QObject::connect(searchEdit, &QLineEdit::textChanged, page, [=, refreshAccounts] {
        if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
    });
    QObject::connect(roleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=, refreshAccounts] {
        if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
    });
    QObject::connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=, refreshAccounts] {
        if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
    });

    // Create Account action
    QObject::connect(btnCreate, &QPushButton::clicked, page, [=, refreshAccounts] {
        const QString roleStr = inRole->currentText();
        const QString codeStr = inCode->text().trimmed();
        const QString nameStr = inName->text().trimmed();
        const QString emailStr = inEmail->text().trimmed();
        const QString pwStr = inPassword->text().trimmed();

        if (codeStr.isEmpty() || nameStr.isEmpty() || emailStr.isEmpty() || pwStr.isEmpty()) {
            lblCreateMsg->setText("⚠️ Vui lòng điền đầy đủ tất cả các trường.");
            lblCreateMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
            return;
        }

        const auto res = AccountService::createAccount(roleStr, codeStr, nameStr, emailStr, pwStr);
        if (res.ok) {
            lblCreateMsg->setText("✓ " + res.message);
            lblCreateMsg->setStyleSheet("color: #15803D; font-weight: 600;");
            inCode->clear();
            inName->clear();
            inEmail->clear();
            inPassword->clear();
            if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
        } else {
            lblCreateMsg->setText("❌ " + res.message);
            lblCreateMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
        }
    });

    if (refreshAccounts && *refreshAccounts) (*refreshAccounts)();
    pageLayout->addStretch();
    return page;
}

QWidget *adminCoursesPage() {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    // Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Quản lý học phần & Danh sách sinh viên (Course Management)");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Thiết lập môn học, phân công giảng viên và quản lý danh sách sinh viên ghi danh");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    // 3 KPI Cards
    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(16);
    auto *cardTotal = createKpiCard("Tổng học phần", "0", "Học kỳ 1 (2026-2027)");
    auto *cardEnrolled = createKpiCard("Tổng lượt ghi danh", "0", "Sinh viên đang học", "#1E40AF");
    auto *cardSessions = createKpiCard("Phiên điểm danh", "0", "Đã tổ chức", "#0F766E");

    kpiRow->addWidget(cardTotal);
    kpiRow->addWidget(cardEnrolled);
    kpiRow->addWidget(cardSessions);
    pageLayout->addLayout(kpiRow);

    // Courses Directory Table
    auto *tableCard = new QFrame;
    tableCard->setObjectName("overviewCard");
    auto *tLay = new QVBoxLayout(tableCard);
    tLay->setContentsMargins(20, 18, 20, 18);
    tLay->setSpacing(12);

    auto *tHeader = new QLabel("Danh mục học phần");
    tHeader->setObjectName("overviewCardTitle");
    tLay->addWidget(tHeader);

    auto *table = new QTableWidget;
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Mã môn", "Tên học phần", "Giảng viên", "Sinh viên", "Số phiên", "Thao tác"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->setColumnWidth(0, 50);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->setColumnWidth(1, 130);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->setColumnWidth(4, 90);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(5, 90);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->setColumnWidth(6, 210);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setMinimumHeight(240);
    tLay->addWidget(table);
    pageLayout->addWidget(tableCard);

    // Create Course Card
    auto *createCard = new QFrame;
    createCard->setObjectName("overviewCard");
    auto *cLay = new QVBoxLayout(createCard);
    cLay->setContentsMargins(20, 18, 20, 18);
    cLay->setSpacing(12);

    auto *cTitle = new QLabel("➕ Mở môn học phần mới (Create Course)");
    cTitle->setObjectName("overviewCardTitle");
    cLay->addWidget(cTitle);

    auto *formRow = new QHBoxLayout;
    formRow->setSpacing(12);

    auto *inCode = new QLineEdit;
    inCode->setPlaceholderText("Mã môn (vd: 012012210501)");
    inCode->setFixedHeight(36);
    formRow->addWidget(inCode, 1);

    auto *inName = new QLineEdit;
    inName->setPlaceholderText("Tên môn học (vd: Lập trình Web)");
    inName->setFixedHeight(36);
    formRow->addWidget(inName, 2);

    auto *inLecturer = new QComboBox;
    inLecturer->setFixedHeight(36);
    const auto lecturers = UserRepository{}.findAll("LECTURER");
    for (const auto &l : lecturers) {
        inLecturer->addItem(QString("%1 - %2").arg(l.code, l.fullName), l.userId);
    }
    formRow->addWidget(inLecturer, 2);

    auto *btnCreate = new QPushButton("Tạo môn học");
    btnCreate->setObjectName("btnPrimary");
    btnCreate->setFixedHeight(36);
    btnCreate->setCursor(Qt::PointingHandCursor);
    formRow->addWidget(btnCreate, 1);

    cLay->addLayout(formRow);

    auto *lblCreateMsg = new QLabel;
    lblCreateMsg->setWordWrap(true);
    cLay->addWidget(lblCreateMsg);

    pageLayout->addWidget(createCard);

    // Refresh function
    auto refreshCourses = std::make_shared<std::function<void()>>();
    *refreshCourses = [=, refreshCourses]() {
        const auto allCourses = CourseRepository{}.findAll();
        int totalEnroll = 0;
        int totalSess = 0;

        for (const auto &c : allCourses) {
            totalEnroll += static_cast<int>(CourseRepository{}.getEnrolledStudents(c.courseId).size());
            totalSess += static_cast<int>(SessionRepository{}.findByCourse(c.courseId).size());
        }

        auto findValLabel = [](QWidget *cd) -> QLabel* {
            return cd->findChild<QLabel*>("kpiValue");
        };
        if (auto *l = findValLabel(cardTotal)) l->setText(QString::number(allCourses.size()));
        if (auto *l = findValLabel(cardEnrolled)) l->setText(QString::number(totalEnroll));
        if (auto *l = findValLabel(cardSessions)) l->setText(QString::number(totalSess));

        table->setRowCount(static_cast<int>(allCourses.size()));
        for (int i = 0; i < static_cast<int>(allCourses.size()); ++i) {
            const auto &c = allCourses[i];
            const auto enrolledList = CourseRepository{}.getEnrolledStudents(c.courseId);
            const auto sessList = SessionRepository{}.findByCourse(c.courseId);

            table->setItem(i, 0, new QTableWidgetItem(QString::number(c.courseId)));
            table->setItem(i, 1, new QTableWidgetItem(c.code));
            table->setItem(i, 2, new QTableWidgetItem(c.name));
            table->setItem(i, 3, new QTableWidgetItem(c.lecturerName.isEmpty() ? "Chưa phân công" : c.lecturerName));

            auto *itemEnrolled = new QTableWidgetItem(QString::number(enrolledList.size()));
            itemEnrolled->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 4, itemEnrolled);

            auto *itemSess = new QTableWidgetItem(QString::number(sessList.size()));
            itemSess->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 5, itemSess);

            // Actions: Manage Students + Delete Course
            auto *btnManage = new QPushButton("Quản lý SV");
            btnManage->setObjectName("linkPrimary");
            btnManage->setCursor(Qt::PointingHandCursor);

            auto *btnDelete = new QPushButton("Xóa môn");
            btnDelete->setObjectName("btnDangerOutline");
            btnDelete->setCursor(Qt::PointingHandCursor);

            auto *actBox = new QWidget;
            auto *actLay = new QHBoxLayout(actBox);
            actLay->setContentsMargins(0, 0, 0, 0);
            actLay->setSpacing(6);
            actLay->setAlignment(Qt::AlignCenter);
            actLay->addWidget(btnManage);
            actLay->addWidget(btnDelete);
            table->setCellWidget(i, 6, actBox);

            // Manage Students Dialog
            QObject::connect(btnManage, &QPushButton::clicked, page, [=, refreshCourses, targetCourseId = c.courseId, courseTitle = QString("%1 - %2").arg(c.code, c.name)] {
                auto *dlg = new QDialog(page);
                dlg->setWindowTitle("Quản lý danh sách sinh viên - " + courseTitle);
                dlg->resize(650, 480);
                auto *dlgLay = new QVBoxLayout(dlg);
                dlgLay->setContentsMargins(20, 20, 20, 20);
                dlgLay->setSpacing(14);

                auto *dlgHead = new QLabel("Danh sách sinh viên ghi danh: " + courseTitle);
                dlgHead->setStyleSheet("font-size: 15px; font-weight: 700; color: #112D55;");
                dlgLay->addWidget(dlgHead);

                // Add student row
                auto *addRow = new QHBoxLayout;
                auto *studentCombo = new QComboBox;
                studentCombo->setFixedHeight(36);

                const auto allStudents = UserRepository{}.findAll("STUDENT");
                auto currentEnrolled = CourseRepository{}.getEnrolledStudents(targetCourseId);

                for (const auto &s : allStudents) {
                    bool already = false;
                    for (const auto &e : currentEnrolled) {
                        if (e.userId == s.userId) { already = true; break; }
                    }
                    if (!already) {
                        studentCombo->addItem(QString("%1 - %2 (%3)").arg(s.code, s.fullName, s.email), s.userId);
                    }
                }

                auto *btnAddStudent = new QPushButton("➕ Ghi danh sinh viên");
                btnAddStudent->setObjectName("btnPrimary");
                btnAddStudent->setFixedHeight(36);
                btnAddStudent->setCursor(Qt::PointingHandCursor);
                addRow->addWidget(studentCombo, 3);
                addRow->addWidget(btnAddStudent, 1);
                dlgLay->addLayout(addRow);

                auto *sTable = new QTableWidget;
                sTable->setColumnCount(4);
                sTable->setHorizontalHeaderLabels({"STT", "Mã SV", "Họ và tên", "Thao tác"});
                sTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
                sTable->setColumnWidth(0, 50);
                sTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
                sTable->setColumnWidth(1, 130);
                sTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
                sTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
                sTable->setColumnWidth(3, 120);
                sTable->verticalHeader()->setVisible(false);
                sTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
                dlgLay->addWidget(sTable);

                auto refreshDlgTable = std::make_shared<std::function<void()>>();
                *refreshDlgTable = [=, refreshDlgTable, refreshCourses]() mutable {
                    const auto updatedEnrolled = CourseRepository{}.getEnrolledStudents(targetCourseId);
                    sTable->setRowCount(static_cast<int>(updatedEnrolled.size()));
                    for (int j = 0; j < static_cast<int>(updatedEnrolled.size()); ++j) {
                        const auto &st = updatedEnrolled[j];
                        sTable->setItem(j, 0, new QTableWidgetItem(QString::number(j + 1)));
                        sTable->setItem(j, 1, new QTableWidgetItem(st.code));
                        sTable->setItem(j, 2, new QTableWidgetItem(st.fullName));

                        auto *btnUnenroll = new QPushButton("Hủy ghi danh");
                        btnUnenroll->setObjectName("btnDangerOutline");
                        btnUnenroll->setCursor(Qt::PointingHandCursor);
                        auto *w = new QWidget;
                        auto *lw = new QHBoxLayout(w);
                        lw->setContentsMargins(0, 0, 0, 0);
                        lw->setAlignment(Qt::AlignCenter);
                        lw->addWidget(btnUnenroll);
                        sTable->setCellWidget(j, 3, w);

                        QObject::connect(btnUnenroll, &QPushButton::clicked, dlg, [=, refreshDlgTable, refreshCourses, sId = st.userId, sName = st.fullName] {
                            const auto reply = QMessageBox::question(dlg, "Xác nhận hủy ghi danh",
                                QString("Xác nhận hủy ghi danh sinh viên '%1' khỏi môn học này?").arg(sName),
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                            if (reply == QMessageBox::Yes) {
                                CourseService::unenrollStudent(targetCourseId, sId);
                                if (refreshDlgTable && *refreshDlgTable) (*refreshDlgTable)();
                                if (refreshCourses && *refreshCourses) (*refreshCourses)();
                            }
                        });
                    }
                };

                if (refreshDlgTable && *refreshDlgTable) (*refreshDlgTable)();

                QObject::connect(btnAddStudent, &QPushButton::clicked, dlg, [=, refreshDlgTable, refreshCourses]() mutable {
                    if (studentCombo->currentIndex() < 0) return;
                    const int sId = studentCombo->currentData().toInt();
                    const auto res = CourseService::enrollStudent(targetCourseId, sId);
                    if (res.ok) {
                        studentCombo->removeItem(studentCombo->currentIndex());
                        if (refreshDlgTable && *refreshDlgTable) (*refreshDlgTable)();
                        if (refreshCourses && *refreshCourses) (*refreshCourses)();
                    } else {
                        QMessageBox::warning(dlg, "Lỗi", res.message);
                    }
                });

                auto *btnCloseDlg = new QPushButton("Đóng");
                btnCloseDlg->setObjectName("btnOutline");
                QObject::connect(btnCloseDlg, &QPushButton::clicked, dlg, &QDialog::accept);
                dlgLay->addWidget(btnCloseDlg, 0, Qt::AlignRight);

                dlg->exec();
            });

            // Delete Course logic
            QObject::connect(btnDelete, &QPushButton::clicked, page, [=, refreshCourses, targetCourseId = c.courseId, courseTitle = QString("%1 - %2").arg(c.code, c.name)] {
                if (CourseRepository{}.hasSessions(targetCourseId)) {
                    QMessageBox::warning(page, "Không thể xóa học phần",
                        QString("Học phần '%1' đã có các phiên điểm danh trong hệ thống.\n\n"
                                "Theo quy định bảo toàn dữ liệu học vụ, không thể xóa học phần này.")
                            .arg(courseTitle));
                    return;
                }

                const auto reply = QMessageBox::question(page, "Xác nhận xóa học phần",
                    QString("Bạn có chắc chắn muốn xóa học phần '%1'? Hành động này không thể hoàn tác.").arg(courseTitle),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    const auto res = CourseService::deleteCourse(targetCourseId);
                    if (res.ok) {
                        if (refreshCourses && *refreshCourses) (*refreshCourses)();
                    } else {
                        QMessageBox::warning(page, "Lỗi", res.message);
                    }
                }
            });
        }
    };

    // Create course button logic
    QObject::connect(btnCreate, &QPushButton::clicked, page, [=, refreshCourses] {
        const QString codeStr = inCode->text().trimmed();
        const QString nameStr = inName->text().trimmed();
        if (codeStr.isEmpty() || nameStr.isEmpty()) {
            lblCreateMsg->setText("⚠️ Vui lòng nhập đầy đủ mã và tên học phần.");
            lblCreateMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
            return;
        }
        if (inLecturer->currentIndex() < 0) {
            lblCreateMsg->setText("⚠️ Hãy tạo tài khoản Giảng viên trước khi tạo môn học.");
            lblCreateMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
            return;
        }
        const int lecturerId = inLecturer->currentData().toInt();
        const auto res = CourseService::createCourse(codeStr, nameStr, lecturerId);
        if (res.ok) {
            lblCreateMsg->setText("✓ " + res.message);
            lblCreateMsg->setStyleSheet("color: #15803D; font-weight: 600;");
            inCode->clear();
            inName->clear();
            if (refreshCourses && *refreshCourses) (*refreshCourses)();
        } else {
            lblCreateMsg->setText("❌ " + res.message);
            lblCreateMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
        }
    });

    if (refreshCourses && *refreshCourses) (*refreshCourses)();
    pageLayout->addStretch();
    return page;
}

QWidget *adminTimetablePage() {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    // Header
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel("Quản lý Thời khóa biểu & Lịch giảng dạy (Timetable Management)");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel("Thiết lập lịch học trên lớp (Class) và lịch tự học E-learning cho các lớp học phần");
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    pageLayout->addLayout(headerBox);

    const auto allCourses = CourseRepository{}.findAll();

    // Schedule Entries Table Card
    auto *tableCard = new QFrame;
    tableCard->setObjectName("overviewCard");
    auto *tLay = new QVBoxLayout(tableCard);
    tLay->setContentsMargins(20, 18, 20, 18);
    tLay->setSpacing(12);

    auto *tHeader = new QLabel("Lịch học phần hiện hữu");
    tHeader->setObjectName("overviewCardTitle");
    tLay->addWidget(tHeader);

    auto *table = new QTableWidget;
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels({"ID", "Học phần", "Loại", "Thứ", "Thời gian", "Phòng", "Khoảng ngày", "Thao tác"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    table->setColumnWidth(0, 50);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    table->setColumnWidth(2, 100);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    table->setColumnWidth(3, 90);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    table->setColumnWidth(4, 130);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(5, 110);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->setColumnWidth(6, 170);
    table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    table->setColumnWidth(7, 100);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setMinimumHeight(260);
    tLay->addWidget(table);
    pageLayout->addWidget(tableCard);

    // Add Schedule Entry Form Card
    auto *addCard = new QFrame;
    addCard->setObjectName("overviewCard");
    auto *aLay = new QVBoxLayout(addCard);
    aLay->setContentsMargins(20, 18, 20, 18);
    aLay->setSpacing(12);

    auto *aTitle = new QLabel("➕ Thêm lịch học phần mới (Add Schedule)");
    aTitle->setObjectName("overviewCardTitle");
    aLay->addWidget(aTitle);

    auto *formRow1 = new QHBoxLayout;
    formRow1->setSpacing(12);

    auto *courseBox = new QComboBox;
    courseBox->setFixedHeight(36);
    for (const auto &c : allCourses) {
        courseBox->addItem(QString("%1 - %2").arg(c.code, c.name), c.courseId);
    }
    formRow1->addWidget(courseBox, 3);

    auto *typeBox = new QComboBox;
    typeBox->setFixedHeight(36);
    typeBox->addItem("🏫 Lớp trực tiếp (CLASS)", static_cast<int>(ScheduleType::CLASS));
    typeBox->addItem("🌐 Tự học E-learning (STUDY)", static_cast<int>(ScheduleType::STUDY));
    formRow1->addWidget(typeBox, 2);

    auto *dayBox = new QComboBox;
    dayBox->setFixedHeight(36);
    dayBox->addItem("Thứ 2", 1);
    dayBox->addItem("Thứ 3", 2);
    dayBox->addItem("Thứ 4", 3);
    dayBox->addItem("Thứ 5", 4);
    dayBox->addItem("Thứ 6", 5);
    dayBox->addItem("Thứ 7", 6);
    dayBox->addItem("Chủ nhật", 7);
    formRow1->addWidget(dayBox, 1);

    auto *roomEdit = new QLineEdit;
    roomEdit->setPlaceholderText("Phòng học (vd: B003 - CS1)");
    roomEdit->setFixedHeight(36);
    formRow1->addWidget(roomEdit, 2);

    aLay->addLayout(formRow1);

    auto *formRow2 = new QHBoxLayout;
    formRow2->setSpacing(12);

    auto *startEdit = new QTimeEdit(QTime(9, 25));
    startEdit->setFixedHeight(36);
    formRow2->addWidget(new QLabel("Bắt đầu:"));
    formRow2->addWidget(startEdit);

    auto *endEdit = new QTimeEdit(QTime(11, 55));
    endEdit->setFixedHeight(36);
    formRow2->addWidget(new QLabel("Kết thúc:"));
    formRow2->addWidget(endEdit);

    auto *startDateEdit = new QDateEdit(QDate::currentDate());
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setFixedHeight(36);
    formRow2->addWidget(new QLabel("Từ ngày:"));
    formRow2->addWidget(startDateEdit);

    auto *endDateEdit = new QDateEdit(QDate::currentDate().addMonths(2));
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setFixedHeight(36);
    formRow2->addWidget(new QLabel("Đến ngày:"));
    formRow2->addWidget(endDateEdit);

    auto *btnAdd = new QPushButton("Thêm lịch học");
    btnAdd->setObjectName("btnPrimary");
    btnAdd->setFixedHeight(36);
    btnAdd->setCursor(Qt::PointingHandCursor);
    formRow2->addWidget(btnAdd);

    aLay->addLayout(formRow2);

    auto *lblAddMsg = new QLabel;
    lblAddMsg->setWordWrap(true);
    aLay->addWidget(lblAddMsg);

    pageLayout->addWidget(addCard);

    // Refresh function
    auto refreshSchedules = std::make_shared<std::function<void()>>();
    *refreshSchedules = [=, refreshSchedules]() {
        const auto entries = TimetableService::instance().getAllSchedules();
        table->setRowCount(static_cast<int>(entries.size()));

        for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
            const auto &e = entries[i];
            table->setItem(i, 0, new QTableWidgetItem(QString::number(e.scheduleId)));
            table->setItem(i, 1, new QTableWidgetItem(QString("%1 - %2").arg(e.courseCode, e.courseName)));

            const QString typeStr = (e.type == ScheduleType::STUDY) ? "E-Learning" : "Trực tiếp";
            table->setItem(i, 2, new QTableWidgetItem(typeStr));

            const QString dowStr = (e.dayOfWeek == 1) ? "Thứ 2" : (e.dayOfWeek == 2) ? "Thứ 3"
                                : (e.dayOfWeek == 3) ? "Thứ 4" : (e.dayOfWeek == 4) ? "Thứ 5"
                                : (e.dayOfWeek == 5) ? "Thứ 6" : (e.dayOfWeek == 6) ? "Thứ 7" : "Chủ nhật";
            table->setItem(i, 3, new QTableWidgetItem(dowStr));

            table->setItem(i, 4, new QTableWidgetItem(QString("%1 - %2")
                .arg(e.startTime.toString("HH:mm")).arg(e.endTime.toString("HH:mm"))));

            table->setItem(i, 5, new QTableWidgetItem(cleanRoomName(e.room)));

            table->setItem(i, 6, new QTableWidgetItem(QString("%1 → %2")
                .arg(e.startDate.toString("dd/MM/yyyy")).arg(e.endDate.toString("dd/MM/yyyy"))));

            auto *btnDel = new QPushButton("Xóa");
            btnDel->setObjectName("btnDangerOutline");
            btnDel->setCursor(Qt::PointingHandCursor);
            auto *w = new QWidget;
            auto *lw = new QHBoxLayout(w);
            lw->setContentsMargins(0, 0, 0, 0);
            lw->setAlignment(Qt::AlignCenter);
            lw->addWidget(btnDel);
            table->setCellWidget(i, 7, w);

            QObject::connect(btnDel, &QPushButton::clicked, page, [=, refreshSchedules, sId = e.scheduleId, cName = e.courseName, dow = dowStr] {
                const auto reply = QMessageBox::question(page, "Xác nhận xóa lịch học",
                    QString("Bạn có chắc chắn muốn xóa lịch '%1' (%2)?").arg(cName, dow),
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    TimetableService::instance().deleteSchedule(sId);
                    if (refreshSchedules && *refreshSchedules) (*refreshSchedules)();
                }
            });
        }
    };

    // Add schedule button logic
    QObject::connect(btnAdd, &QPushButton::clicked, page, [=, refreshSchedules] {
        if (courseBox->currentIndex() < 0) return;
        if (startEdit->time() >= endEdit->time()) {
            lblAddMsg->setText("⚠️ Giờ bắt đầu phải nhỏ hơn giờ kết thúc.");
            lblAddMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
            return;
        }
        if (startDateEdit->date() > endDateEdit->date()) {
            lblAddMsg->setText("⚠️ Ngày bắt đầu không thể sau ngày kết thúc.");
            lblAddMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
            return;
        }

        ScheduleEntry entry;
        entry.courseId = courseBox->currentData().toInt();
        entry.type = static_cast<ScheduleType>(typeBox->currentData().toInt());
        entry.dayOfWeek = dayBox->currentData().toInt();
        entry.startTime = startEdit->time();
        entry.endTime = endEdit->time();
        entry.room = roomEdit->text().trimmed();
        entry.startDate = startDateEdit->date();
        entry.endDate = endDateEdit->date();

        const auto res = TimetableService::instance().addSchedule(entry);
        if (res.ok) {
            lblAddMsg->setText("✓ " + res.message);
            lblAddMsg->setStyleSheet("color: #15803D; font-weight: 600;");
            roomEdit->clear();
            if (refreshSchedules && *refreshSchedules) (*refreshSchedules)();
        } else {
            lblAddMsg->setText("❌ " + res.message);
            lblAddMsg->setStyleSheet("color: #DC2626; font-weight: 600;");
        }
    });

    if (refreshSchedules && *refreshSchedules) (*refreshSchedules)();
    pageLayout->addStretch();
    return page;
}

QWidget *studentOverviewPage(int userId, std::function<void(const QString&)> navCallback = nullptr) {
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    const auto currentUser = AuthService::instance().currentUser();
    const QString studentName = currentUser.has_value() ? currentUser->fullName : "Sinh viên";

    // 1. Header Row
    auto *headerRow = new QHBoxLayout;
    auto *headerBox = new QVBoxLayout;
    headerBox->setSpacing(4);
    auto *title = new QLabel(QString("Xin chào, %1 👋").arg(studentName));
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel(QString("Hôm nay là %1 · Học kỳ 1 (2026 - 2027)")
        .arg(QDate::currentDate().toString("dddd, dd/MM/yyyy")));
    subTitle->setStyleSheet("color: #64748B; font-size: 14px;");
    headerBox->addWidget(title);
    headerBox->addWidget(subTitle);
    headerRow->addLayout(headerBox, 1);

    auto *btnViewAtt = new QPushButton("Xem chuyên cần →");
    btnViewAtt->setObjectName("btnOutline");
    btnViewAtt->setCursor(Qt::PointingHandCursor);
    headerRow->addWidget(btnViewAtt);
    pageLayout->addLayout(headerRow);

    if (navCallback) {
        QObject::connect(btnViewAtt, &QPushButton::clicked, page, [navCallback] {
            navCallback("My attendance");
        });
    }

    CourseRepository courses;
    const auto enrolled = courses.findByStudent(userId);

    // 2. Top 4 KPI Stat Cards (GUI 2.0 Page 2)
    int totalCompleted = 0;
    int totalCredited = 0;
    int totalAbsences = 0;
    int totalExcused = 0;
    for (const auto &c : enrolled) {
        const auto st = AttendanceService::instance().getStudentStats(userId, c.courseId);
        totalCompleted += st.totalCompletedSessions;
        totalCredited += (st.presentCount + st.lateCount + st.excusedCount);
        totalAbsences += st.absentCount;
        totalExcused += st.excusedCount;
    }
    const double overallRate = totalCompleted > 0 ? ((double)totalCredited / totalCompleted) * 100.0 : 100.0;

    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(16);
    kpiRow->addWidget(createKpiCard(
        "Tỷ lệ chuyên cần (Overall attendance)",
        QString("%1%").arg(overallRate, 0, 'f', 1),
        QString("%1 đã tính / %2 buổi hoàn thành").arg(totalCredited).arg(totalCompleted)));

    kpiRow->addWidget(createKpiCard(
        "Học phần đăng ký (Enrolled courses)",
        QString::number(enrolled.size()),
        "Học kỳ 1 (2026–2027)"));

    kpiRow->addWidget(createKpiCard(
        "Số buổi vắng (Absences)",
        QString::number(totalAbsences),
        "Trên các buổi đã hoàn thành",
        totalAbsences > 0 ? "#B91C1C" : "#112D55"));

    kpiRow->addWidget(createKpiCard(
        "Đơn xin phép (Excuse requests)",
        QString("%1 đã duyệt").arg(totalExcused),
        "Hồ sơ chuyên cần đã cập nhật"));

    pageLayout->addLayout(kpiRow);

    // 3. Main Content: 2 Columns
    auto *contentRow = new QHBoxLayout;
    contentRow->setSpacing(24);
    contentRow->setAlignment(Qt::AlignTop);

    // LEFT COLUMN (Check-In Sessions, Today's Timetable, Helpful Guidelines)
    auto *leftCol = new QVBoxLayout;
    leftCol->setSpacing(18);

    const auto studentEntries = TimetableService::instance().getStudentTimetable(userId);
    const QDate today = QDate::currentDate();

    // Active Check-In sessions
    SessionRepository sessions;
    int openSessionsCount = 0;
    for (const auto &course : enrolled) {
        auto open = sessions.findOpenSessionForCourse(course.courseId);
        if (open.has_value()) {
            openSessionsCount++;
            auto *sessionBox = new QFrame;
            sessionBox->setObjectName("sessionBox");
            sessionBox->setStyleSheet("QFrame#sessionBox { background-color: #F8FAFC; border: 1px solid #CBD5E1; border-radius: 12px; padding: 16px; }");
            auto *sLayout = new QVBoxLayout(sessionBox);
            sLayout->setSpacing(10);

            auto *badgeRow = new QHBoxLayout;
            auto *badgeOpen = new QLabel("● ĐANG MỞ ĐIỂM DANH");
            badgeOpen->setStyleSheet("background: #DCFCE7; color: #15803D; font-size: 11px; font-weight: 700; border-radius: 4px; padding: 2px 8px;");
            auto *sessIdLbl = new QLabel(QString("Phiên #%1").arg(open->sessionId));
            sessIdLbl->setStyleSheet("color: #64748B; font-size: 12px; font-weight: 600;");
            badgeRow->addWidget(badgeOpen);
            badgeRow->addWidget(sessIdLbl);
            badgeRow->addStretch();
            sLayout->addLayout(badgeRow);

            auto *cTitle = new QLabel(QString("%1 - %2").arg(course.code, course.name));
            cTitle->setStyleSheet("font-weight: bold; color: #112D55; font-size: 15px; background: transparent; border: none;");
            sLayout->addWidget(cTitle);

            auto *info = new QLabel(QString("Thời gian đúng giờ: %1 phút | Đi muộn: đến %2 phút.")
                .arg(open->presentWindowMinutes).arg(open->lateWindowMinutes));
            info->setStyleSheet("color: #64748B; font-size: 12px; background: transparent; border: none;");
            sLayout->addWidget(info);

            auto *row = new QHBoxLayout;
            auto *code = new QLineEdit;
            code->setPlaceholderText("Nhập mã 6 chữ số");
            code->setMaxLength(6);
            code->setFixedWidth(180);
            code->setStyleSheet("QLineEdit { background: #FFFFFF; border: 1px solid #CBD5E1; border-radius: 6px; padding: 8px 12px; color: #112D55; font-size: 14px; font-weight: 600; } QLineEdit:focus { border: 1px solid #1B4F96; }");

            auto *submit = new QPushButton("Xác nhận điểm danh");
            submit->setObjectName("btnSubmitCheckIn");
            submit->setCursor(Qt::PointingHandCursor);
            submit->setStyleSheet(
                "QPushButton {"
                "  background-color: #1B4F96;"
                "  color: #FFFFFF;"
                "  font-weight: bold;"
                "  font-size: 13px;"
                "  padding: 8px 18px;"
                "  border: none;"
                "  border-radius: 6px;"
                "}"
                "QPushButton:hover { background-color: #163F7A; }"
                "QPushButton:disabled { background-color: #94A3B8; color: #FFFFFF; }"
            );

            auto *result = new QLabel;
            result->setStyleSheet("font-size: 12px; font-weight: bold; background: transparent; border: none;");

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

            leftCol->addWidget(sessionBox);
        }
    }

    // Ca học hôm nay card
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

            const QString typeBadge = (entry.type == ScheduleType::STUDY) ? "🌐 E-Learning" : "🏫 Trực tiếp";
            const QString typeStyle = (entry.type == ScheduleType::STUDY)
                ? "color: #0369A1; background: #E0F2FE; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;"
                : "color: #15803D; background: #DCFCE7; font-size: 10px; font-weight: 700; padding: 2px 6px; border-radius: 4px;";
            auto *badge = new QLabel(typeBadge);
            badge->setStyleSheet(typeStyle);
            r1->addWidget(badge);

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

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(cleanRoomName(entry.room)));
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

    // Helpful guidelines card
    auto *notesCard = new QFrame;
    notesCard->setObjectName("overviewCard");
    auto *notesLayout = new QVBoxLayout(notesCard);
    notesLayout->setContentsMargins(18, 14, 18, 14);
    notesLayout->setSpacing(6);
    auto *notesTitle = new QLabel("📌 Thông tin & Lưu ý chuyên cần");
    notesTitle->setStyleSheet("font-weight: 700; color: #112D55; font-size: 14px;");
    notesLayout->addWidget(notesTitle);
    auto *n1 = new QLabel("• Giờ học tính theo múi giờ ICT (UTC+7). Phiên điểm danh được mở trực tiếp bởi giảng viên tại lớp.");
    n1->setStyleSheet("color: #475569; font-size: 12px;");
    auto *n2 = new QLabel("• Tỷ lệ chuyên cần = (Đúng giờ + Đi muộn + Có phép) ÷ Tổng số buổi đã học. Đi muộn vẫn được tính chuyên cần.");
    n2->setStyleSheet("color: #475569; font-size: 12px;");
    auto *n3 = new QLabel("• Nếu vắng học có lý do chính đáng, sinh viên vui lòng vào mục 'My attendance' để gửi đơn xin phép kèm minh chứng.");
    n3->setStyleSheet("color: #475569; font-size: 12px;");
    notesLayout->addWidget(n1);
    notesLayout->addWidget(n2);
    notesLayout->addWidget(n3);
    leftCol->addWidget(notesCard);

    leftCol->addStretch();
    contentRow->addLayout(leftCol, 3);

    // RIGHT COLUMN (Mini Calendar + Selected Date Schedule + My Courses Summary)
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

    // My courses quick summary card
    auto *coursesCard = new QFrame;
    coursesCard->setObjectName("overviewCard");
    auto *coursesLayout = new QVBoxLayout(coursesCard);
    coursesLayout->setContentsMargins(18, 16, 18, 16);
    coursesLayout->setSpacing(10);

    auto *coursesCardHeader = new QHBoxLayout;
    auto *coursesCardTitle = new QLabel("📚 Học phần của tôi");
    coursesCardTitle->setObjectName("overviewCardTitle");
    coursesCardHeader->addWidget(coursesCardTitle);
    coursesCardHeader->addStretch();

    if (navCallback) {
        auto *btnViewAll = new QPushButton("Tất cả →");
        btnViewAll->setObjectName("linkPrimary");
        btnViewAll->setCursor(Qt::PointingHandCursor);
        coursesCardHeader->addWidget(btnViewAll);
        QObject::connect(btnViewAll, &QPushButton::clicked, page, [navCallback] {
            navCallback("My attendance");
        });
    }
    coursesLayout->addLayout(coursesCardHeader);

    for (const auto &c : enrolled) {
        const auto st = AttendanceService::instance().getStudentStats(userId, c.courseId);
        auto *item = new QFrame;
        item->setStyleSheet("QFrame { background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 8px; padding: 8px 12px; } QFrame:hover { background: #F1F5F9; }");
        auto *iLay = new QHBoxLayout(item);
        iLay->setContentsMargins(0, 0, 0, 0);

        auto *cText = new QLabel(QString("%1\n%2").arg(c.name, c.code));
        cText->setStyleSheet("font-size: 12px; font-weight: 600; color: #112D55; background: transparent; border: none;");
        iLay->addWidget(cText, 1);

        const QString badgeColor = st.attendanceRate >= 80.0 ? "background: #DCFCE7; color: #15803D;"
                                 : (st.attendanceRate >= 50.0 ? "background: #FEF3C7; color: #B45309;" : "background: #FEE2E2; color: #B91C1C;");
        auto *rateBadge = new QLabel(QString("%1%").arg(st.attendanceRate, 0, 'f', 1));
        rateBadge->setStyleSheet(QString("%1 font-size: 11px; font-weight: 700; border-radius: 6px; padding: 3px 8px; border: none;").arg(badgeColor));
        iLay->addWidget(rateBadge);

        coursesLayout->addWidget(item);
    }

    rightCol->addWidget(coursesCard);
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
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 28);
    pageLayout->setSpacing(20);

    CourseRepository courseRepo;
    const auto enrolled = courseRepo.findByStudent(userId);

    // 1. Top Header Row: Title & Subtitle on left, Course Selector Combo on right
    auto *headerRow = new QHBoxLayout;
    auto *leftHeader = new QVBoxLayout;
    leftHeader->setSpacing(4);

    auto *title = new QLabel("My course attendance");
    title->setObjectName("lblHeading");
    auto *subTitle = new QLabel;
    subTitle->setStyleSheet("color: #64748B; font-size: 13px;");
    leftHeader->addWidget(title);
    leftHeader->addWidget(subTitle);
    headerRow->addLayout(leftHeader, 1);

    auto *courseCombo = new QComboBox;
    courseCombo->setObjectName("courseSelectCombo");
    courseCombo->setCursor(Qt::PointingHandCursor);
    for (const auto &c : enrolled) {
        courseCombo->addItem(QString("%1 - %2").arg(c.code, c.name), c.courseId);
    }
    headerRow->addWidget(courseCombo);
    pageLayout->addLayout(headerRow);

    if (enrolled.empty()) {
        auto *emptyMsg = new QLabel("Bạn chưa đăng ký môn học nào trong học kỳ này.");
        emptyMsg->setStyleSheet("color: #64748B; font-style: italic; padding: 24px;");
        pageLayout->addWidget(emptyMsg);
        pageLayout->addStretch();
        return page;
    }

    // 2. Top 3 KPI Cards (GUI 2.0 Page 3)
    auto *kpiRow = new QHBoxLayout;
    kpiRow->setSpacing(20);

    auto *cardRate = new QFrame; cardRate->setObjectName("kpiCard");
    auto *cardRateLay = new QVBoxLayout(cardRate);
    auto *kpiTitle1 = new QLabel("Attendance rate");
    kpiTitle1->setObjectName("kpiTitle");
    cardRateLay->addWidget(kpiTitle1);
    auto *lblRateVal = new QLabel("0.0%");
    lblRateVal->setObjectName("kpiValue");
    auto *lblRateSub = new QLabel("0 credited / 0 completed sessions");
    lblRateSub->setObjectName("kpiSubtext");
    cardRateLay->addWidget(lblRateVal);
    cardRateLay->addWidget(lblRateSub);
    kpiRow->addWidget(cardRate);

    auto *cardPresentLate = new QFrame; cardPresentLate->setObjectName("kpiCard");
    auto *cardPresentLateLay = new QVBoxLayout(cardPresentLate);
    auto *kpiTitle2 = new QLabel("Present / Late");
    kpiTitle2->setObjectName("kpiTitle");
    cardPresentLateLay->addWidget(kpiTitle2);
    auto *lblPresentLateVal = new QLabel("0 / 0");
    lblPresentLateVal->setObjectName("kpiValue");
    lblPresentLateVal->setStyleSheet("color: #15803D;");
    auto *lblPresentLateSub = new QLabel("Late sessions count toward attendance");
    lblPresentLateSub->setObjectName("kpiSubtext");
    cardPresentLateLay->addWidget(lblPresentLateVal);
    cardPresentLateLay->addWidget(lblPresentLateSub);
    kpiRow->addWidget(cardPresentLate);

    auto *cardAbsentExcused = new QFrame; cardAbsentExcused->setObjectName("kpiCard");
    auto *cardAbsentExcusedLay = new QVBoxLayout(cardAbsentExcused);
    auto *kpiTitle3 = new QLabel("Absent / Excused");
    kpiTitle3->setObjectName("kpiTitle");
    cardAbsentExcusedLay->addWidget(kpiTitle3);
    auto *lblAbsentExcusedVal = new QLabel("0 / 0");
    lblAbsentExcusedVal->setObjectName("kpiValue");
    lblAbsentExcusedVal->setStyleSheet("color: #B91C1C;");
    auto *lblAbsentExcusedSub = new QLabel("0 absences across completed sessions");
    lblAbsentExcusedSub->setObjectName("kpiSubtext");
    cardAbsentExcusedLay->addWidget(lblAbsentExcusedVal);
    cardAbsentExcusedLay->addWidget(lblAbsentExcusedSub);
    kpiRow->addWidget(cardAbsentExcused);

    pageLayout->addLayout(kpiRow);

    // 3. Session History Card (GUI 2.0 Page 3)
    auto *historyCard = new QFrame;
    historyCard->setObjectName("overviewCard");
    auto *historyLayout = new QVBoxLayout(historyCard);
    historyLayout->setContentsMargins(20, 18, 20, 18);
    historyLayout->setSpacing(14);

    auto *histHeaderRow = new QHBoxLayout;
    auto *histTitle = new QLabel("Session history");
    histTitle->setObjectName("overviewCardTitle");
    auto *histScheduleInfo = new QLabel;
    histScheduleInfo->setStyleSheet("color: #64748B; font-size: 13px;");
    histHeaderRow->addWidget(histTitle);
    histHeaderRow->addStretch();
    histHeaderRow->addWidget(histScheduleInfo);
    historyLayout->addLayout(histHeaderRow);

    auto *table = new QTableWidget;
    table->setObjectName("historyTable");
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Session / date", "Check-in", "Result", "Request"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    table->setColumnWidth(1, 140);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    table->setColumnWidth(2, 140);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    table->setColumnWidth(3, 160);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setMinimumHeight(240);
    historyLayout->addWidget(table);

    // Bottom Legend and Note
    auto *legendRow = new QHBoxLayout;
    auto *lblCountInfo = new QLabel;
    lblCountInfo->setStyleSheet("color: #64748B; font-size: 12px; font-weight: 500;");
    legendRow->addWidget(lblCountInfo);
    legendRow->addStretch();

    auto *legendDots = new QLabel("● Present   ● Late   ● Absent   ● Excused");
    legendDots->setStyleSheet("color: #475569; font-size: 12px; font-weight: 600;");
    legendRow->addWidget(legendDots);
    historyLayout->addLayout(legendRow);

    auto *ruleNote = new QLabel("Rate = (Present + Late + Excused) ÷ completed sessions. An approved request changes its related Absent result to Excused.");
    ruleNote->setStyleSheet("color: #94A3B8; font-size: 12px; font-style: italic;");
    ruleNote->setWordWrap(true);
    historyLayout->addWidget(ruleNote);

    pageLayout->addWidget(historyCard);

    // Update handler
    auto updateCourseData = [=](int courseId) {
        // Find course in enrolled
        Course currentCourse;
        for (const auto &c : enrolled) {
            if (c.courseId == courseId) {
                currentCourse = c;
                break;
            }
        }

        const QString lecturer = currentCourse.lecturerName.isEmpty() ? "Giảng viên bộ môn" : currentCourse.lecturerName;
        subTitle->setText(QString("%1 · %2 · %3 · Cập nhật %4 ICT · Học kỳ 1, 2026–27")
            .arg(currentCourse.code, currentCourse.name, lecturer, QTime::currentTime().toString("HH:mm:ss")));

        // Find schedule info
        const auto entries = TimetableService::instance().getStudentTimetable(userId);
        QString schedText = "Lịch học: Xem thời khóa biểu";
        for (const auto &e : entries) {
            if (e.courseId == courseId && e.type == ScheduleType::CLASS) {
                const QString dowStr = (e.dayOfWeek == 1) ? "Thứ 2" : (e.dayOfWeek == 2) ? "Thứ 3"
                                    : (e.dayOfWeek == 3) ? "Thứ 4" : (e.dayOfWeek == 4) ? "Thứ 5"
                                    : (e.dayOfWeek == 5) ? "Thứ 6" : (e.dayOfWeek == 6) ? "Thứ 7" : "Chủ nhật";
                schedText = QString("%1 · %2–%3 · Phòng %4")
                    .arg(dowStr, e.startTime.toString("HH:mm"), e.endTime.toString("HH:mm"), cleanRoomName(e.room));
                break;
            }
        }
        histScheduleInfo->setText(schedText);

        // Stats
        const auto stats = AttendanceService::instance().getStudentStats(userId, courseId);
        lblRateVal->setText(QString("%1%").arg(stats.attendanceRate, 0, 'f', 1));
        const int credited = stats.presentCount + stats.lateCount + stats.excusedCount;
        lblRateSub->setText(QString("%1 credited / %2 completed sessions").arg(credited).arg(stats.totalCompletedSessions));
        lblPresentLateVal->setText(QString("%1 / %2").arg(stats.presentCount).arg(stats.lateCount));
        lblAbsentExcusedVal->setText(QString("%1 / %2").arg(stats.absentCount).arg(stats.excusedCount));
        lblAbsentExcusedSub->setText(QString("%1 absence%2 across completed sessions")
            .arg(stats.absentCount).arg(stats.absentCount == 1 ? "" : "s"));

        // Records & Active Session
        const auto records = AttendanceService::instance().getStudentRecords(userId, courseId);
        SessionRepository sRepo;
        auto openSession = sRepo.findOpenSessionForCourse(courseId);

        const int totalRows = static_cast<int>(records.size()) + (openSession.has_value() ? 1 : 0);
        table->setRowCount(totalRows);

        int rowIdx = 0;
        if (openSession.has_value()) {
            auto *itemSess = new QTableWidgetItem(QString("Phiên #%1 · Hôm nay (Đang mở)").arg(openSession->sessionId));
            itemSess->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            table->setItem(rowIdx, 0, itemSess);

            auto *itemCheckIn = new QTableWidgetItem("Đang diễn ra");
            itemCheckIn->setTextAlignment(Qt::AlignCenter);
            table->setItem(rowIdx, 1, itemCheckIn);

            auto *badgeOpen = new QLabel("Session open");
            badgeOpen->setObjectName("badgeOpen");
            auto *wOpen = new QWidget;
            auto *lOpen = new QHBoxLayout(wOpen);
            lOpen->setContentsMargins(0, 0, 0, 0);
            lOpen->setAlignment(Qt::AlignCenter);
            lOpen->addWidget(badgeOpen);
            table->setCellWidget(rowIdx, 2, wOpen);

            auto *itemReq = new QTableWidgetItem("Not final");
            itemReq->setTextAlignment(Qt::AlignCenter);
            itemReq->setForeground(QColor("#64748B"));
            table->setItem(rowIdx, 3, itemReq);

            rowIdx++;
        }

        for (const auto &r : records) {
            const QString dateStr = r.sessionStartTime.isValid()
                ? r.sessionStartTime.toString("dd/MM/yyyy · HH:mm") : "Buổi học";
            auto *itemSess = new QTableWidgetItem(QString("Phiên #%1 · %2").arg(r.sessionId).arg(dateStr));
            itemSess->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            table->setItem(rowIdx, 0, itemSess);

            const QString checkInStr = ((r.status == AttendanceStatus::PRESENT || r.status == AttendanceStatus::LATE) && r.checkInTime.isValid())
                ? r.checkInTime.toString("HH:mm:ss") : "—";
            auto *itemCheckIn = new QTableWidgetItem(checkInStr);
            itemCheckIn->setTextAlignment(Qt::AlignCenter);
            table->setItem(rowIdx, 1, itemCheckIn);

            table->setCellWidget(rowIdx, 2, createStatusBadgeWidget(r.status));

            if (r.status == AttendanceStatus::EXCUSED) {
                auto *badgeApp = new QLabel("Approved");
                badgeApp->setObjectName("badgeExcused");
                auto *wApp = new QWidget;
                auto *lApp = new QHBoxLayout(wApp);
                lApp->setContentsMargins(0, 0, 0, 0);
                lApp->setAlignment(Qt::AlignCenter);
                lApp->addWidget(badgeApp);
                table->setCellWidget(rowIdx, 3, wApp);
            } else if (r.status == AttendanceStatus::ABSENT) {
                auto *btnExcuse = new QPushButton("Gửi đơn");
                btnExcuse->setObjectName("btnOutline");
                btnExcuse->setCursor(Qt::PointingHandCursor);
                btnExcuse->setStyleSheet("padding: 2px 10px; font-size: 11px;");
                auto *wBtn = new QWidget;
                auto *lBtn = new QHBoxLayout(wBtn);
                lBtn->setContentsMargins(0, 0, 0, 0);
                lBtn->setAlignment(Qt::AlignCenter);
                lBtn->addWidget(btnExcuse);
                table->setCellWidget(rowIdx, 3, wBtn);

                QObject::connect(btnExcuse, &QPushButton::clicked, page, [page, currentCourse, dateStr] {
                    showExcuseRequestDialog(page, QString("%1 - %2").arg(currentCourse.code, currentCourse.name), dateStr);
                });
            } else {
                auto *itemDash = new QTableWidgetItem("—");
                itemDash->setTextAlignment(Qt::AlignCenter);
                itemDash->setForeground(QColor("#94A3B8"));
                table->setItem(rowIdx, 3, itemDash);
            }

            rowIdx++;
        }

        lblCountInfo->setText(QString("Hiển thị %1 buổi học · %2 đã hoàn thành%3")
            .arg(totalRows).arg(stats.totalCompletedSessions).arg(openSession.has_value() ? ", 1 đang mở" : ""));
    };

    QObject::connect(courseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), page, [=](int) {
        updateCourseData(courseCombo->currentData().toInt());
    });

    if (!enrolled.empty()) {
        updateCourseData(enrolled.front().courseId);
    }

    pageLayout->addStretch();
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
            m_shell->addPage(p, lecturerOverviewPage(user->userId, [this](const QString &target) {
                if (m_shell) m_shell->setCurrentPage(target);
            }, m_shell));
        } else if (user->role == UserRole::LECTURER && p == "Timetable") {
            m_shell->addPage(p, lecturerTimetablePage(user->userId));
        } else if (user->role == UserRole::LECTURER && p == "Open session") {
            m_shell->addPage(p, lecturerOpenSessionPage(user->userId, [this](const QString &target) {
                if (m_shell) m_shell->setCurrentPage(target);
            }, m_shell));
        } else if (user->role == UserRole::LECTURER && p == "Live session") {
            m_shell->addPage(p, lecturerLiveSessionPage(user->userId, [this](const QString &target) {
                if (m_shell) m_shell->setCurrentPage(target);
            }, m_shell));
        } else if (user->role == UserRole::LECTURER && p == "Attendance report") {
            m_shell->addPage(p, lecturerReportPage(user->userId, m_shell));
        } else if (user->role == UserRole::ADMIN && p == "Accounts") {
            m_shell->addPage(p, adminAccountsPage());
        } else if (user->role == UserRole::ADMIN && p == "Courses") {
            m_shell->addPage(p, adminCoursesPage());
        } else if (user->role == UserRole::ADMIN && p == "Timetables") {
            m_shell->addPage(p, adminTimetablePage());
        } else if (user->role == UserRole::STUDENT && p == "Overview") {
            m_shell->addPage(p, studentOverviewPage(user->userId, [this](const QString &target) {
                if (m_shell) m_shell->setCurrentPage(target);
            }));
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
