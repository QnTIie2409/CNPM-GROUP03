#include "DatabaseManager.h"
#include "../utils/ConfigReader.h"
#include "../utils/Logger.h"
#include <QDebug>

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initFromConfig() {
    ConfigReader& cfg = ConfigReader::instance();
    return connectToMySQL(cfg.dbHost(), cfg.dbPort(), cfg.dbName(), cfg.dbUser(), cfg.dbPassword());
}

QStringList DatabaseManager::getAvailableDrivers() const {
    return QSqlDatabase::drivers();
}

bool DatabaseManager::connectToMySQL(const QString& host, int port, const QString& dbName,
                                     const QString& user, const QString& password) {
    QString driver = "QMYSQL";
    if (!QSqlDatabase::isDriverAvailable(driver)) {
        if (QSqlDatabase::isDriverAvailable("QODBC")) {
            driver = "QODBC";
        }
    }

    if (QSqlDatabase::contains("qt_mysql_main")) {
        m_db = QSqlDatabase::database("qt_mysql_main");
    } else {
        m_db = QSqlDatabase::addDatabase(driver, "qt_mysql_main");
    }

    if (driver == "QODBC") {
        QString connStr = QString("DRIVER={MySQL ODBC 8.0 Unicode Driver};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                              .arg(host).arg(port).arg(dbName).arg(user).arg(password);
        m_db.setDatabaseName(connStr);
    } else {
        m_db.setHostName(host);
        m_db.setPort(port);
        m_db.setDatabaseName(dbName);
        m_db.setUserName(user);
        m_db.setPassword(password);
    }

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        Logger::logError("Database connection failed: " + m_lastError);
        return false;
    }

    Logger::log("Database connected successfully using driver: " + driver);
    return true;
}

QString DatabaseManager::getLastError() const {
    return m_lastError;
}

bool DatabaseManager::isConnected() const {
    return m_db.isOpen();
}

QSqlDatabase DatabaseManager::getDatabase() {
    return m_db;
}
