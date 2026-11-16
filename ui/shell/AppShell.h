#pragma once
#include <QWidget>
#include <QVector>

class QStackedWidget;
class QLabel;
class QButtonGroup;
class QVBoxLayout;

class AppShell : public QWidget {
    Q_OBJECT
public:
    enum class Role { Student, Lecturer, Admin };

    AppShell(Role role, const QString &userName, QWidget *parent = nullptr);

    // Them mot trang vao menu ben trai (trang dau tien duoc chon san)
    void addPage(const QString &navText, QWidget *page);

signals:
    void signOutRequested();
    void changePasswordRequested();

private:
    QWidget *buildSidebar(Role role);
    QWidget *buildTopBar(const QString &userName);
    QWidget *buildFooter();
    void showPage(int index);

    QStackedWidget *m_stack = nullptr;
    QVBoxLayout *m_navLayout = nullptr;
    QButtonGroup *m_navGroup = nullptr;
    QLabel *m_breadcrumb = nullptr;
    QVector<QString> m_titles;
};
