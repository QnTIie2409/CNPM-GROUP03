#ifndef MINICALENDARWIDGET_H
#define MINICALENDARWIDGET_H

#include <QWidget>
#include <QDate>
#include <vector>
#include "../src/models/ScheduleEntry.h"

class QLabel;
class QGridLayout;
class QPushButton;
class QFrame;

class MiniCalendarWidget : public QWidget {
    Q_OBJECT
public:
    explicit MiniCalendarWidget(QWidget *parent = nullptr);

    void setScheduleEntries(const std::vector<ScheduleEntry> &entries);
    void setSelectedDate(const QDate &date);
    QDate selectedDate() const { return m_selectedDate; }

signals:
    void dateSelected(const QDate &date);

private slots:
    void onPrevMonth();
    void onNextMonth();
    void onResetToday();

private:
    void updateCalendar();
    int countClassesOnDate(const QDate &date) const;

    QDate m_displayMonth;
    QDate m_selectedDate;
    std::vector<ScheduleEntry> m_entries;

    QLabel *m_lblMonthYear = nullptr;
    QWidget *m_daysGridWidget = nullptr;
    QGridLayout *m_daysGrid = nullptr;
};

#endif // MINICALENDARWIDGET_H
