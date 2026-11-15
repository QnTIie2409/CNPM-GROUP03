#include "UiLauncher.h"
#include "LoginWindow.h"
#include "shell/AppShell.h"
#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QStringList>

UiLauncher::UiLauncher(QObject *parent) : QObject(parent) {}

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
            [this](const QString &account, const QString &) { openShellFor(account); });
    m_login->show();
}

// TAM THOI: chon vai tro theo tien to tai khoan de xem du 3 khung giao dien.
//   gv...    -> Lecturer, admin... -> Admin, con lai -> Student
// Phan backend se thay bang xac thuc that.
void UiLauncher::openShellFor(const QString &account) {
    AppShell::Role role = AppShell::Role::Student;
    QString name = "Nguyễn Minh Anh";
    QStringList pages = {"Overview", "Timetable", "My attendance"};

    if (account.startsWith("gv", Qt::CaseInsensitive)) {
        role = AppShell::Role::Lecturer;
        name = "Trần Minh Quân";
        pages = {"Overview", "Timetable", "Open session", "Live session",
                 "Attendance report", "Excuse requests"};
    } else if (account.startsWith("admin", Qt::CaseInsensitive)) {
        role = AppShell::Role::Admin;
        name = "Lê Hoàng Phúc";
        pages = {"Accounts", "Courses", "Timetables"};
    }

    delete m_shell;
    m_shell = new AppShell(role, name);
    m_shell->setAttribute(Qt::WA_DeleteOnClose);

    for (const QString &p : pages) {
        auto *placeholder = new QLabel(p + " (todo)");
        placeholder->setMargin(32);
        placeholder->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        m_shell->addPage(p, placeholder);
    }

    connect(m_shell, &AppShell::signOutRequested, this, [this] {
        if (m_shell) m_shell->close();
        if (m_login) m_login->show();
    });

    m_shell->resize(1440, 900);
    if (m_login) m_login->hide();
    m_shell->show();
}
