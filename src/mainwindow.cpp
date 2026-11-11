#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repositories/DatabaseManager.h"
#include "services/AuthService.h"
#include "services/SessionService.h"
#include "services/CheckInService.h"
#include "utils/Logger.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Khởi tạo logger và nạp cấu hình Database
    Logger::init("app.log");
    Logger::log("Student Attendance System started.");

    // Kiểm tra drivers SQL sẵn có
    QStringList drivers = DatabaseManager::instance().getAvailableDrivers();
    ui->txtDrivers->setPlainText("Danh sách SQL Drivers hiện có trong Qt:\n - " + drivers.join("\n - ") +
                                 "\n\nCấu trúc 3 tầng (Models, Repositories, Services, UI) đã được nạp sẵn sàng!");

    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onConnectButtonClicked() {
    QString host = ui->editHost->text().trimmed();
    int port = ui->editPort->text().toInt();
    QString db = ui->editDatabase->text().trimmed();
    QString user = ui->editUser->text().trimmed();
    QString pass = ui->editPassword->text();

    ui->lblStatus->setText("Đang kết nối cơ sở dữ liệu...");

    bool ok = DatabaseManager::instance().connectToMySQL(host, port, db, user, pass);
    if (ok) {
        ui->lblStatus->setStyleSheet("color: #15803D; font-weight: bold;");
        ui->lblStatus->setText("Kết nối MySQL THÀNH CÔNG! Đã sẵn sàng truy vấn.");
        QMessageBox::information(this, "Thành công", "Đã kết nối thành công tới cơ sở dữ liệu MySQL!\nDatabase: " + db);
    } else {
        QString err = DatabaseManager::instance().getLastError();
        ui->lblStatus->setStyleSheet("color: #B91C1C; font-weight: bold;");
        ui->lblStatus->setText("Kết nối THẤT BẠI!");
        QMessageBox::warning(this, "Lỗi kết nối", "Không thể kết nối MySQL:\n" + err);
    }
}
