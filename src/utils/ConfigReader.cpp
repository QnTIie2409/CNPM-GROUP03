#include "ConfigReader.h"
#include <QFileInfo>
#include <QCoreApplication>

ConfigReader::ConfigReader() {
    load();
}

ConfigReader& ConfigReader::instance() {
    static ConfigReader inst;
    return inst;
}

void ConfigReader::load(const QString& filePath) {
    QString path = filePath;
    if (!QFileInfo::exists(path)) {
        // Try relative to application directory
        path = QCoreApplication::applicationDirPath() + "/" + filePath;
    }

    if (QFileInfo::exists(path)) {
        QSettings settings(path, QSettings::IniFormat);
        m_host = settings.value("Database/host", "127.0.0.1").toString();
        m_port = settings.value("Database/port", 3306).toInt();
        m_dbName = settings.value("Database/dbname", "student_attendance").toString();
        m_user = settings.value("Database/username", "root").toString();
        m_password = settings.value("Database/password", "").toString();
    }
}

QString ConfigReader::dbHost() const { return m_host; }
int ConfigReader::dbPort() const { return m_port; }
QString ConfigReader::dbName() const { return m_dbName; }
QString ConfigReader::dbUser() const { return m_user; }
QString ConfigReader::dbPassword() const { return m_password; }
