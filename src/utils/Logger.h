#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger {
public:
    static void init(const QString& logFile = "app.log");
    static void log(const QString& message, const QString& level = "INFO");
    static void logError(const QString& error);
};

#endif // LOGGER_H
