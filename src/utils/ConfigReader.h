#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QString>
#include <QSettings>

class ConfigReader {
public:
    static ConfigReader& instance();

    void load(const QString& filePath = "config.ini");

    QString dbHost() const;
    int dbPort() const;
    QString dbName() const;
    QString dbUser() const;
    QString dbPassword() const;

private:
    ConfigReader();
    QString m_host = "127.0.0.1";
    int m_port = 3306;
    QString m_dbName = "student_attendance";
    QString m_user = "root";
    QString m_password = "";
};

#endif // CONFIGREADER_H
