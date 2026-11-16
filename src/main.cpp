#include "loginwindow.h"
#include "repositories/DatabaseManager.h"
#include "utils/Logger.h"
#include <QApplication>
#include <QFile>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::init("app.log");
    Logger::log("Student Attendance System starting...");

    DatabaseManager::instance().initFromConfig();

    QString stylePath = "resources/style.qss";
    if (!QFileInfo::exists(stylePath)) {
        stylePath = a.applicationDirPath() + "/resources/style.qss";
    }
    QFile styleFile(stylePath);
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        a.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    LoginWindow w;
    w.show();
    return a.exec();
}
