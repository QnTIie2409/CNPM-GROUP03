#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <QStringList>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initFromConfig();
    bool connectToMySQL(const QString& host, int port, const QString& dbName,
                        const QString& user, const QString& password);

    QStringList getAvailableDrivers() const;
    QString getLastError() const;
    bool isConnected() const;
    QSqlDatabase getDatabase();

private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
