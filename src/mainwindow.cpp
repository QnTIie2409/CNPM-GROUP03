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

    Logger::init("app.log");
    Logger::log("Student Attendance System dashboard initialized.");

    QStringList drivers = DatabaseManager::instance().getAvailableDrivers();
    ui->txtDrivers->setPlainText("Available Qt SQL Drivers:\n - " + drivers.join("\n - ") +
                                 "\n\n3-Tier Architecture (Models, Repositories, Services, UI) is ready!");

    if (DatabaseManager::instance().isConnected()) {
        ui->lblStatus->setStyleSheet("color: #15803D; font-weight: bold;");
        ui->lblStatus->setText("MySQL Connection: SUCCESSFUL (Ready for queries)");
    } else {
        ui->lblStatus->setStyleSheet("color: #B91C1C; font-weight: bold;");
        ui->lblStatus->setText("Status: Not Connected");
    }

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

    ui->lblStatus->setText("Connecting to database...");

    bool ok = DatabaseManager::instance().connectToMySQL(host, port, db, user, pass);
    if (ok) {
        ui->lblStatus->setStyleSheet("color: #15803D; font-weight: bold;");
        ui->lblStatus->setText("MySQL Connection SUCCESSFUL! Ready for queries.");
        QMessageBox::information(this, "Success", "Successfully connected to MySQL database!\nDatabase: " + db);
    } else {
        QString err = DatabaseManager::instance().getLastError();
        ui->lblStatus->setStyleSheet("color: #B91C1C; font-weight: bold;");
        ui->lblStatus->setText("Connection FAILED!");
        QMessageBox::warning(this, "Connection Error", "Cannot connect to MySQL database:\n" + err);
    }
}
