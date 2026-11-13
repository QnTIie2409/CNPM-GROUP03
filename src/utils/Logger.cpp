#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

static QString g_logFilePath = "app.log";

void Logger::init(const QString& logFile) {
    g_logFilePath = logFile;
}

void Logger::log(const QString& message, const QString& level) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString line = QString("[%1] [%2] %3\n").arg(timestamp, level, message);

    qDebug().noquote() << line.trimmed();

    QFile file(g_logFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << line;
    }
}

void Logger::logError(const QString& error) {
    log(error, "ERROR");
}
