#include "MiniCalendarWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

MiniCalendarWidget::MiniCalendarWidget(QWidget *parent)
    : QWidget(parent),
      m_displayMonth(QDate::currentDate().year(), QDate::currentDate().month(), 1),
      m_selectedDate(QDate::currentDate()) {

    setObjectName("miniCalendarCard");
    setAttribute(Qt::WA_StyledBackground, true);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 14, 16, 16);
    mainLayout->setSpacing(10);

    // Header: "Lịch theo tháng" and Navigation
    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(6);

    auto *lblTitle = new QLabel("Lịch theo tháng");
    lblTitle->setObjectName("miniCalTitle");
    headerLayout->addWidget(lblTitle);
    headerLayout->addStretch();

    auto *btnPrev = new QPushButton("<");
    btnPrev->setObjectName("miniCalNavBtn");
    btnPrev->setFixedSize(28, 28);
    connect(btnPrev, &QPushButton::clicked, this, &MiniCalendarWidget::onPrevMonth);
    headerLayout->addWidget(btnPrev);

    m_lblMonthYear = new QLabel;
    m_lblMonthYear->setObjectName("miniCalMonthBadge");
    headerLayout->addWidget(m_lblMonthYear);

    auto *btnNext = new QPushButton(">");
    btnNext->setObjectName("miniCalNavBtn");
    btnNext->setFixedSize(28, 28);
    connect(btnNext, &QPushButton::clicked, this, &MiniCalendarWidget::onNextMonth);
    headerLayout->addWidget(btnNext);

    mainLayout->addLayout(headerLayout);

    // Weekdays header: CN, T2, T3, T4, T5, T6, T7
    auto *weekdaysLayout = new QHBoxLayout;
    weekdaysLayout->setSpacing(4);
    const QStringList days = {"CN", "T2", "T3", "T4", "T5", "T6", "T7"};
    for (int i = 0; i < days.size(); ++i) {
        auto *lbl = new QLabel(days[i]);
        lbl->setAlignment(Qt::AlignCenter);
        if (i == 0) {
            lbl->setObjectName("miniCalColHeaderSun");
        } else {
            lbl->setObjectName("miniCalColHeader");
        }
        weekdaysLayout->addWidget(lbl, 1);
    }
    mainLayout->addLayout(weekdaysLayout);

    // Divider line
    auto *divider = new QFrame;
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setObjectName("miniCalDivider");
    mainLayout->addWidget(divider);

    // Days Grid Container
    m_daysGridWidget = new QWidget;
    m_daysGrid = new QGridLayout(m_daysGridWidget);
    m_daysGrid->setContentsMargins(0, 0, 0, 0);
    m_daysGrid->setSpacing(4);
    mainLayout->addWidget(m_daysGridWidget);

    updateCalendar();
}

void MiniCalendarWidget::setScheduleEntries(const std::vector<ScheduleEntry> &entries) {
    m_entries = entries;
    updateCalendar();
}

void MiniCalendarWidget::setSelectedDate(const QDate &date) {
    if (m_selectedDate != date) {
        m_selectedDate = date;
        if (m_displayMonth.year() != date.year() || m_displayMonth.month() != date.month()) {
            m_displayMonth = QDate(date.year(), date.month(), 1);
        }
        updateCalendar();
    }
}

void MiniCalendarWidget::onPrevMonth() {
    m_displayMonth = m_displayMonth.addMonths(-1);
    updateCalendar();
}

void MiniCalendarWidget::onNextMonth() {
    m_displayMonth = m_displayMonth.addMonths(1);
    updateCalendar();
}

void MiniCalendarWidget::onResetToday() {
    m_selectedDate = QDate::currentDate();
    m_displayMonth = QDate(m_selectedDate.year(), m_selectedDate.month(), 1);
    updateCalendar();
    emit dateSelected(m_selectedDate);
}

int MiniCalendarWidget::countClassesOnDate(const QDate &date) const {
    int count = 0;
    const int dow = date.dayOfWeek();
    for (const auto &e : m_entries) {
        if (e.dayOfWeek == dow && date >= e.startDate && date <= e.endDate) {
            ++count;
        }
    }
    return count;
}

void MiniCalendarWidget::updateCalendar() {
    m_lblMonthYear->setText(QString("tháng %1 %2 📅")
        .arg(m_displayMonth.month())
        .arg(m_displayMonth.year()));

    // Clear previous items
    QLayoutItem *child;
    while ((child = m_daysGrid->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    const int daysInMonth = m_displayMonth.daysInMonth();
    const QDate firstDay(m_displayMonth.year(), m_displayMonth.month(), 1);
    // Sunday is col 0, Monday is 1, ..., Saturday is 6
    const int startCol = firstDay.dayOfWeek() % 7;

    int row = 0;
    int col = startCol;

    const QDate today = QDate::currentDate();

    for (int day = 1; day <= daysInMonth; ++day) {
        const QDate curDate(m_displayMonth.year(), m_displayMonth.month(), day);
        const int classCount = countClassesOnDate(curDate);
        const bool isToday = (curDate == today);
        const bool isSelected = (curDate == m_selectedDate);
        const bool isSunday = (col == 0);

        auto *btn = new QPushButton;
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setFixedHeight(38);

        // Build HTML text with day number and dot indicators
        QString dotHtml = "";
        if (classCount == 1) {
            dotHtml = "<br><span style='color:#16A34A; font-size:14px; line-height:8px;'>•</span>";
        } else if (classCount == 2) {
            dotHtml = "<br><span style='color:#0D9488; font-size:14px; line-height:8px;'>••</span>";
        } else if (classCount > 2) {
            dotHtml = "<br><span style='color:#EA580C; font-size:14px; line-height:8px;'>•••</span>";
        }

        const QString numColor = isSunday ? "#EA580C" : "#1E293B";
        btn->setText(QString::number(day));

        // Styling based on state
        QString style = "QPushButton { border-radius: 8px; font-weight: 600; font-size: 13px; ";
        if (isSunday) {
            style += "color: #EA580C; ";
        } else {
            style += "color: #1E293B; ";
        }

        if (classCount > 0) {
            style += "background-color: #B5E2E0; ";
        } else {
            style += "background-color: transparent; ";
        }

        if (isToday) {
            style += "border: 2px solid #0D7377; ";
        } else if (isSelected) {
            style += "border: 2px dashed #0D7377; ";
        } else {
            style += "border: 1px solid transparent; ";
        }

        style += "} QPushButton:hover { background-color: #99D5D3; }";
        btn->setStyleSheet(style);

        // Put a small container with day number & dot
        auto *dayLayout = new QVBoxLayout(btn);
        dayLayout->setContentsMargins(0, 3, 0, 2);
        dayLayout->setSpacing(0);
        dayLayout->setAlignment(Qt::AlignCenter);

        auto *lblNum = new QLabel(QString::number(day));
        lblNum->setAlignment(Qt::AlignCenter);
        lblNum->setStyleSheet(QString("color: %1; font-weight: 600; font-size: 13px; background: transparent; border: none;").arg(numColor));
        dayLayout->addWidget(lblNum);

        auto *lblDot = new QLabel;
        lblDot->setAlignment(Qt::AlignCenter);
        lblDot->setStyleSheet("background: transparent; border: none;");
        if (classCount == 1) {
            lblDot->setText("•");
            lblDot->setStyleSheet("color: #16A34A; font-size: 15px; font-weight: bold; background: transparent; border: none; margin-top: -6px;");
        } else if (classCount == 2) {
            lblDot->setText("••");
            lblDot->setStyleSheet("color: #0D9488; font-size: 15px; font-weight: bold; background: transparent; border: none; margin-top: -6px;");
        } else if (classCount > 2) {
            lblDot->setText("•••");
            lblDot->setStyleSheet("color: #EA580C; font-size: 15px; font-weight: bold; background: transparent; border: none; margin-top: -6px;");
        } else {
            lblDot->setText(" ");
            lblDot->setFixedHeight(6);
        }
        dayLayout->addWidget(lblDot);

        connect(btn, &QPushButton::clicked, this, [this, curDate] {
            m_selectedDate = curDate;
            updateCalendar();
            emit dateSelected(curDate);
        });

        m_daysGrid->addWidget(btn, row, col);

        col++;
        if (col > 6) {
            col = 0;
            row++;
        }
    }
}
