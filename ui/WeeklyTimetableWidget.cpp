#include "WeeklyTimetableWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>
#include <QScrollArea>
#include <QFrame>

WeeklyTimetableWidget::WeeklyTimetableWidget(QWidget *parent)
    : QWidget(parent) {
    
    // Default to current week's Monday
    const QDate today = QDate::currentDate();
    m_weekMonday = today.addDays(1 - today.dayOfWeek());

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(14);

    // Top control bar
    auto *topBar = new QHBoxLayout;
    topBar->setSpacing(10);

    // Left date edit
    m_dateEdit = new QDateEdit(today);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("dd/MM/yyyy");
    m_dateEdit->setFixedHeight(36);
    m_dateEdit->setFixedWidth(140);
    m_dateEdit->setStyleSheet(
        "QDateEdit {"
        "  border: 1px solid #1B4F96;"
        "  border-radius: 6px;"
        "  padding: 4px 10px;"
        "  font-weight: 600;"
        "  color: #112D55;"
        "  background: #FFFFFF;"
        "}"
    );
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &WeeklyTimetableWidget::onDateChanged);
    topBar->addWidget(m_dateEdit);

    topBar->addStretch();

    // Right navigation buttons: [ ← ], [ 📅 HIỆN TẠI ], [ → ]
    auto *btnPrev = new QPushButton("←");
    btnPrev->setCursor(Qt::PointingHandCursor);
    btnPrev->setFixedSize(38, 36);
    btnPrev->setStyleSheet(
        "QPushButton {"
        "  background-color: #1B4F96;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  font-size: 15px;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #163F7A; }"
    );
    connect(btnPrev, &QPushButton::clicked, this, &WeeklyTimetableWidget::onPrevWeek);
    topBar->addWidget(btnPrev);

    auto *btnCurrent = new QPushButton("📅 HIỆN TẠI");
    btnCurrent->setCursor(Qt::PointingHandCursor);
    btnCurrent->setFixedHeight(36);
    btnCurrent->setStyleSheet(
        "QPushButton {"
        "  background-color: #1B4F96;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  font-size: 13px;"
        "  padding: 6px 16px;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #163F7A; }"
    );
    connect(btnCurrent, &QPushButton::clicked, this, &WeeklyTimetableWidget::onCurrentWeek);
    topBar->addWidget(btnCurrent);

    auto *btnNext = new QPushButton("→");
    btnNext->setCursor(Qt::PointingHandCursor);
    btnNext->setFixedSize(38, 36);
    btnNext->setStyleSheet(
        "QPushButton {"
        "  background-color: #1B4F96;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  font-size: 15px;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover { background-color: #163F7A; }"
    );
    connect(btnNext, &QPushButton::clicked, this, &WeeklyTimetableWidget::onNextWeek);
    topBar->addWidget(btnNext);

    mainLayout->addLayout(topBar);

    // Scroll Area for the Grid Table
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; }");

    m_gridContainer = new QWidget;
    m_gridContainer->setStyleSheet(
        "QWidget#timetableContainer {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #1B4F96;"
        "  border-radius: 8px;"
        "}"
    );
    m_gridContainer->setObjectName("timetableContainer");

    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(1, 1, 1, 1);
    m_gridLayout->setSpacing(1);

    m_scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(m_scrollArea, 1);

    renderGrid();
}

void WeeklyTimetableWidget::setScheduleEntries(const std::vector<ScheduleEntry> &entries) {
    m_entries = entries;
    renderGrid();
}

void WeeklyTimetableWidget::setWeekOfDate(const QDate &date) {
    m_weekMonday = date.addDays(1 - date.dayOfWeek());
    if (m_dateEdit->date() != date) {
        m_dateEdit->blockSignals(true);
        m_dateEdit->setDate(date);
        m_dateEdit->blockSignals(false);
    }
    renderGrid();
}

void WeeklyTimetableWidget::onPrevWeek() {
    setWeekOfDate(m_weekMonday.addDays(-7));
}

void WeeklyTimetableWidget::onNextWeek() {
    setWeekOfDate(m_weekMonday.addDays(7));
}

void WeeklyTimetableWidget::onCurrentWeek() {
    setWeekOfDate(QDate::currentDate());
}

void WeeklyTimetableWidget::onDateChanged(const QDate &date) {
    setWeekOfDate(date);
}

int WeeklyTimetableWidget::getShiftIndex(const QTime &time) const {
    if (time < QTime(9, 20)) {
        return 0; // Ca 1
    } else if (time < QTime(12, 15)) {
        return 1; // Ca 2
    } else if (time < QTime(14, 45)) {
        return 2; // Ca 3
    } else if (time < QTime(17, 30)) {
        return 3; // Ca 4
    } else {
        return 4; // Ca 5
    }
}

QString WeeklyTimetableWidget::getShiftName(int shiftIndex) const {
    switch (shiftIndex) {
        case 0: return "Ca 1";
        case 1: return "Ca 2";
        case 2: return "Ca 3";
        case 3: return "Ca 4";
        case 4: return "Ca 5";
        default: return "";
    }
}

QString WeeklyTimetableWidget::getPeriodText(int shiftIndex) const {
    switch (shiftIndex) {
        case 0: return "Tiết: 1 - 3";
        case 1: return "Tiết: 4 - 6";
        case 2: return "Tiết: 7 - 9";
        case 3: return "Tiết: 10 - 12";
        case 4: return "Tiết: 13 - 15";
        default: return "";
    }
}

QWidget* WeeklyTimetableWidget::createCourseCard(const ScheduleEntry &entry, int shiftIndex) {
    auto *card = new QFrame;
    card->setObjectName("timetableCourseCard");
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    const bool isElearning = (entry.type == ScheduleType::STUDY);

    QString bgColor = "#EEF2FF";
    QString accentColor = "#3B82F6";
    if (isElearning) {
        bgColor = "#F0F9FF";
        accentColor = "#0284C7";
    } else if (entry.courseId % 3 == 1) {
        bgColor = "#EFF6FF";
        accentColor = "#2563EB";
    } else if (entry.courseId % 3 == 2) {
        bgColor = "#F5F3FF";
        accentColor = "#8B5CF6";
    } else {
        bgColor = "#FFF7ED";
        accentColor = "#EA580C";
    }

    card->setStyleSheet(QString(
        "QFrame#timetableCourseCard {"
        "  background-color: %1;"
        "  border: 1px solid #E2E8F0;"
        "  border-left: 3px solid %2;"
        "  border-radius: 6px;"
        "  padding: 6px;"
        "}"
    ).arg(bgColor, accentColor));

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(3);

    const QString displayName = isElearning ? (entry.courseName + " (E-learning)") : entry.courseName;
    auto *lblName = new QLabel(displayName);
    lblName->setWordWrap(true);
    lblName->setStyleSheet("font-weight: 700; font-size: 11px; color: #1E293B; background: transparent;");
    layout->addWidget(lblName);

    auto *lblCode = new QLabel(entry.courseCode);
    lblCode->setStyleSheet("font-size: 10px; color: #64748B; background: transparent;");
    layout->addWidget(lblCode);

    const QString periodStr = isElearning ? "💻 Tiết: 13 - 15 (E-learning)" : ("📗 " + getPeriodText(shiftIndex));
    auto *lblPeriod = new QLabel(periodStr);
    lblPeriod->setStyleSheet("font-size: 10px; color: #334155; background: transparent;");
    layout->addWidget(lblPeriod);

    auto *lblTime = new QLabel(QString("⏰ %1 - %2")
        .arg(entry.startTime.toString("HH:mm"))
        .arg(entry.endTime.toString("HH:mm")));
    lblTime->setStyleSheet("font-size: 10px; color: #334155; background: transparent;");
    layout->addWidget(lblTime);

    auto *lblRoom = new QLabel(QString("📍 Phòng: %1").arg(entry.room.isEmpty() ? "Chưa xếp phòng" : entry.room));
    lblRoom->setStyleSheet("font-size: 10px; color: #334155; background: transparent;");
    layout->addWidget(lblRoom);

    if (isElearning) {
        auto *lblNotes = new QLabel("🌐 Tự học bài tập trên LMS");
        lblNotes->setStyleSheet("font-size: 9px; color: #0284C7; font-weight: 600; background: transparent;");
        layout->addWidget(lblNotes);

        auto *lblLms = new QLabel("🎓 LMS UTH");
        lblLms->setStyleSheet("font-size: 9px; color: #0284C7; font-weight: bold; background: transparent;");
        layout->addWidget(lblLms);
    } else {
        auto *lblNotes = new QLabel("📖 Đề cương chi tiết học phần");
        lblNotes->setStyleSheet("font-size: 9px; color: #DC2626; background: transparent;");
        layout->addWidget(lblNotes);

        auto *lblLms = new QLabel("🎓 LMS");
        lblLms->setStyleSheet("font-size: 9px; color: #DC2626; font-weight: bold; background: transparent;");
        layout->addWidget(lblLms);
    }

    return card;
}

void WeeklyTimetableWidget::renderGrid() {
    // Clear layout
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    const QDate today = QDate::currentDate();

    // 1. Top-left Header: "Ca học" spanning Column 0 and Column 1
    auto *lblCaHeader = new QLabel("Ca học");
    lblCaHeader->setAlignment(Qt::AlignCenter);
    lblCaHeader->setFixedHeight(46);
    lblCaHeader->setStyleSheet(
        "background-color: #1B4F96;"
        "color: #FFFFFF;"
        "font-weight: bold;"
        "font-size: 13px;"
        "border-top-left-radius: 7px;"
    );
    m_gridLayout->addWidget(lblCaHeader, 0, 0, 1, 2);

    // 2. Day Headers for the 7 columns (Thứ 2 to Chủ nhật)
    for (int d = 0; d < 7; ++d) {
        const QDate dayDate = m_weekMonday.addDays(d);
        const bool isToday = (dayDate == today);
        const QString dayName = (d == 6) ? "Chủ nhật" : QString("Thứ %1").arg(d + 2);
        const QString dateStr = dayDate.toString("dd/MM/yyyy");

        auto *headerLabel = new QLabel(QString("%1\n%2").arg(dayName, dateStr));
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setFixedHeight(46);

        QString style = "color: #FFFFFF; font-weight: bold; font-size: 12px; ";
        if (isToday) {
            style += "background-color: #F59E0B; "; // Highlight today in orange
        } else {
            style += "background-color: #1B4F96; "; // Primary UTH Blue
        }
        if (d == 6) {
            style += "border-top-right-radius: 7px; ";
        }
        headerLabel->setStyleSheet(style);

        m_gridLayout->addWidget(headerLabel, 0, d + 2);
        m_gridLayout->setColumnStretch(d + 2, 1);
    }

    m_gridLayout->setColumnStretch(0, 0);
    m_gridLayout->setColumnStretch(1, 0);

    // 3. Shift Column (Sáng and Chiều)
    // Sáng covers Ca 1 and Ca 2 (rows 1 and 2)
    auto *lblSang = new QLabel("Sáng");
    lblSang->setAlignment(Qt::AlignCenter);
    lblSang->setFixedWidth(65);
    lblSang->setStyleSheet(
        "background-color: #1B4F96;"
        "color: #FFFFFF;"
        "font-weight: bold;"
        "font-size: 13px;"
        "border-right: 1px solid #163F7A;"
    );
    m_gridLayout->addWidget(lblSang, 1, 0, 2, 1);

    // Chiều covers Ca 3, Ca 4, Ca 5 (rows 3, 4, 5)
    auto *lblChieu = new QLabel("Chiều");
    lblChieu->setAlignment(Qt::AlignCenter);
    lblChieu->setFixedWidth(65);
    lblChieu->setStyleSheet(
        "background-color: #1B4F96;"
        "color: #FFFFFF;"
        "font-weight: bold;"
        "font-size: 13px;"
        "border-right: 1px solid #163F7A;"
        "border-bottom-left-radius: 7px;"
    );
    m_gridLayout->addWidget(lblChieu, 3, 0, 3, 1);

    // 4. Ca Names and 5x7 Cells
    for (int shiftIdx = 0; shiftIdx < 5; ++shiftIdx) {
        const int gridRow = shiftIdx + 1;

        // Ca item
        auto *lblCaItem = new QLabel(getShiftName(shiftIdx));
        lblCaItem->setAlignment(Qt::AlignCenter);
        lblCaItem->setFixedWidth(55);
        lblCaItem->setStyleSheet(
            "background-color: #2058A8;"
            "color: #FFFFFF;"
            "font-weight: bold;"
            "font-size: 12px;"
            "border-right: 1px solid #163F7A;"
        );
        m_gridLayout->addWidget(lblCaItem, gridRow, 1);

        // Day cells
        for (int d = 0; d < 7; ++d) {
            const int col = d + 2;
            const QDate cellDate = m_weekMonday.addDays(d);
            const bool isToday = (cellDate == today);
            const int dayOfWeek = d + 1; // 1 = Monday ... 7 = Sunday

            auto *cellFrame = new QFrame;
            cellFrame->setMinimumHeight(135);
            cellFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

            QString cellStyle = "border: 1px solid #E2E8F0; ";
            if (isToday) {
                cellStyle += "background-color: #EFF6FF; "; // Light soft ice blue for today
            } else {
                cellStyle += "background-color: #FFFFFF; ";
            }
            if (shiftIdx == 4 && d == 6) {
                cellStyle += "border-bottom-right-radius: 7px; ";
            }
            cellFrame->setStyleSheet(cellStyle);

            auto *cellLayout = new QVBoxLayout(cellFrame);
            cellLayout->setContentsMargins(6, 6, 6, 6);
            cellLayout->setSpacing(6);

            // Populate courses matching this day and shift
            for (const auto &entry : m_entries) {
                if (entry.dayOfWeek == dayOfWeek &&
                    cellDate >= entry.startDate && cellDate <= entry.endDate &&
                    getShiftIndex(entry.startTime) == shiftIdx) {
                    cellLayout->addWidget(createCourseCard(entry, shiftIdx));
                }
            }

            cellLayout->addStretch();
            m_gridLayout->addWidget(cellFrame, gridRow, col);
        }
        m_gridLayout->setRowStretch(gridRow, 1);
    }
}
