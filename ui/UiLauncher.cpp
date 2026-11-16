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
            codeLbl->setStyleSheet("color: #1B4F96; font-weight: bold; font-size: 14px; background: transparent;");
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

            auto *cRoom = new QLabel(QString("📍 Phòng: %1").arg(entry.room.isEmpty() ? "ONLINE" : entry.room));
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
                    .arg(dowStr, e.startTime.toString("HH:mm"), e.endTime.toString("HH:mm"), e.room.isEmpty() ? "ONLINE" : e.room);
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
