#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QPushButton>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    connect(ui->btnLogin, &QPushButton::clicked, [=]() {
        QString username = ui->txtUsername->text();
        QString password = ui->txtPassword->text();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Cảnh báo", "Vui lòng nhập đầy đủ tên đăng nhập và mật khẩu!");
            return;
        }

        if (username == "admin" && password == "123456") {
            QMessageBox::information(this, "Thành công", "Đăng nhập hệ thống thành công!");
        } else {
            QMessageBox::warning(this, "Lỗi", "Sai tên đăng nhập hoặc mật khẩu!");
        }
    });
}

LoginWindow::~LoginWindow()
{
    delete ui;
}