#pragma once
#include <QObject>
#include <QPointer>

class LoginWindow;
class AppShell;

// Dieu phoi giao dien: nap style, hien Login, dang nhap xong thi mo khung chinh.
// main.cpp chi can: UiLauncher launcher; launcher.start();
class UiLauncher : public QObject {
    Q_OBJECT
public:
    explicit UiLauncher(QObject *parent = nullptr);
    ~UiLauncher() override;

    void start();

private:
    void openShellFor(const QString &account);

    QPointer<LoginWindow> m_login;
    QPointer<AppShell> m_shell;
};
