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
    Logger::log("Student Attendance System started.");

    QStringList drivers = DatabaseManager::instance().getAvailableDrivers();
    ui->txtDrivers->setPlainText("Danh sĂ¡ch SQL Drivers hiá»‡n cĂ³ trong Qt:\n - " + drivers.join("\n - ") +
                                 "\n\nCáº¥u trĂºc 3 táº§ng (Models, Repositories, Services, UI) Ä‘Ă£ Ä‘Æ°á»£c náº¡p sáºµn sĂ ng!");

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

    ui->lblStatus->setText("Äang káº¿t ná»‘i cÆ¡ sá»Ÿ dá»¯ liá»‡u...");

    bool ok = DatabaseManager::instance().connectToMySQL(host, port, db, user, pass);
    if (ok) {
        ui->lblStatus->setStyleSheet("color: #15803D; font-weight: bold;");
        ui->lblStatus->setText("Káº¿t ná»‘i MySQL THĂ€NH CĂ”NG! ÄĂ£ sáºµn sĂ ng truy váº¥n.");
        QMessageBox::information(this, "ThĂ nh cĂ´ng", "ÄĂ£ káº¿t ná»‘i thĂ nh cĂ´ng tá»›i cÆ¡ sá»Ÿ dá»¯ liá»‡u MySQL!\nDatabase: " + db);
    } else {
        QString err = DatabaseManager::instance().getLastError();
        ui->lblStatus->setStyleSheet("color: #B91C1C; font-weight: bold;");
        ui->lblStatus->setText("Káº¿t ná»‘i THáº¤T Báº I!");
        QMessageBox::warning(this, "Lá»—i káº¿t ná»‘i", "KhĂ´ng thá»ƒ káº¿t ná»‘i MySQL:\n" + err);
    }
}
