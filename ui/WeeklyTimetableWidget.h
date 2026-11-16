#ifndef WEEKLYTIMETABLEWIDGET_H
#define WEEKLYTIMETABLEWIDGET_H

#include <QWidget>
#include <QDate>
#include <vector>
#include "../src/models/ScheduleEntry.h"

class QLabel;
class QGridLayout;
class QDateEdit;
class QPushButton;
class QScrollArea;

class WeeklyTimetableWidget : public QWidget {
    Q_OBJECT
public:
    explicit WeeklyTimetableWidget(QWidget *parent = nullptr);

    void setScheduleEntries(const std::vector<ScheduleEntry> &entries);
    void setWeekOfDate(const QDate &date);
    QDate currentWeekMonday() const { return m_weekMonday; }

private slots:
    void onPrevWeek();
    void onNextWeek();
    void onCurrentWeek();
    void onDateChanged(const QDate &date);

private:
    void renderGrid();
    QWidget* createCourseCard(const ScheduleEntry &entry, int shiftIndex);
    int getShiftIndex(const QTime &time) const;
    QString getShiftName(int shiftIndex) const;
    QString getPeriodText(int shiftIndex) const;

    QDate m_weekMonday;
    std::vector<ScheduleEntry> m_entries;

    QDateEdit *m_dateEdit = nullptr;
    QWidget *m_gridContainer = nullptr;
    QGridLayout *m_gridLayout = nullptr;
    QScrollArea *m_scrollArea = nullptr;
};

#endif // WEEKLYTIMETABLEWIDGET_H
