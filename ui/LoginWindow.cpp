#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QAction>
#include <QIcon>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);

    // Icon
    ui->btnSignIn->setIcon(QIcon(":/icons/arrow-right.svg"));
    ui->btnSignIn->setIconSize(QSize(18, 18));
    ui->lblErrorIcon->setPixmap(QIcon(":/icons/info.svg").pixmap(18, 18));

    // Nut an/hien mat khau nam trong o nhap
    m_togglePassword = ui->editPassword->addAction(
        QIcon(":/icons/eye.svg"), QLineEdit::TrailingPosition);
    m_togglePassword->setCheckable(true);
    connect(m_togglePassword, &QAction::toggled, this, [this](bool show) {
        ui->editPassword->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
        m_togglePassword->setIcon(QIcon(show ? ":/icons/eye-off.svg" : ":/icons/eye.svg"));
    });

    ui->errorBanner->hide();

    connect(ui->btnSignIn, &QPushButton::clicked, this, &LoginWindow::onSignInClicked);
    connect(ui->editAccount, &QLineEdit::returnPressed, this,
            [this] { ui->editPassword->setFocus(); });
    connect(ui->editPassword, &QLineEdit::returnPressed, this, &LoginWindow::onSignInClicked);
    connect(ui->editAccount, &QLineEdit::textChanged, this, &LoginWindow::clearError);
    connect(ui->editPassword, &QLineEdit::textChanged, this, &LoginWindow::clearError);
}

LoginWindow::~LoginWindow() { delete ui; }

void LoginWindow::onSignInClicked() {
    const QString account = ui->editAccount->text().trimmed();
    const QString password = ui->editPassword->text();

    if (account.isEmpty() || password.isEmpty()) {
        showError("Please enter your account ID or email and your password.");
        return;
    }
    emit loginRequested(account, password);
}

void LoginWindow::showError(const QString &message) {
    ui->lblErrorText->setText(message);
    ui->errorBanner->show();
}

void LoginWindow::clearError() {
    ui->errorBanner->hide();
}
