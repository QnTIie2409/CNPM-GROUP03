#pragma once
#include <QWidget>

class QAction;

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

public slots:
    void showError(const QString &message);   // backend goi khi dang nhap that bai
    void clearError();

signals:
    void loginRequested(const QString &account, const QString &password);

private slots:
    void onSignInClicked();

private:
    Ui::LoginWindow *ui;
    QAction *m_togglePassword = nullptr;
};
