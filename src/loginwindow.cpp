#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include "services/AuthService.h"
#include "utils/Logger.h"
#include <QMessageBox>
#include <QPushButton>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    connect(ui->btnLogin, &QPushButton::clicked, this, [this]() {
        QString username = ui->txtUsername->text().trimmed();
        QString password = ui->txtPassword->text();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter both username and password!");
            return;
        }

        bool ok = AuthService::instance().login(username, password);
        Logger::log(QString("Login attempt for '%1': %2").arg(username, ok ? "SUCCESS" : "FAILED (" + AuthService::instance().lastError() + ")"));
        if (ok) {
            auto userOpt = AuthService::instance().currentUser();
            QString roleStr = userOpt.has_value() ? User::roleToString(userOpt->role) : "";
            QString nameStr = userOpt.has_value() ? userOpt->fullName : username;

            QMessageBox::information(this, "Success", 
                QString("Login successful!\nWelcome: %1\nRole: %2").arg(nameStr, roleStr));

            MainWindow *mainWin = new MainWindow();
            mainWin->show();
            this->hide();
        } else {
            QString err = AuthService::instance().lastError();
            if (err.isEmpty()) {
                err = "Invalid username or password!";
            }
            QMessageBox::warning(this, "Login Failed", err);
        }
    });
}

LoginWindow::~LoginWindow()
{
    delete ui;
}
