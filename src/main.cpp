#include "../ui/UiLauncher.h"
#include "repositories/DatabaseManager.h"
#include "utils/Logger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger::init("app.log");
    Logger::log("Student Attendance System starting...");

    DatabaseManager::instance().initFromConfig();

    UiLauncher launcher;
    launcher.start();
    return a.exec();
}
