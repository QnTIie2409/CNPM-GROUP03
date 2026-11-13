#include "UserRepository.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

std::optional<User> UserRepository::findByEmail(const QString& email) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT user_id, role, code, full_name, email, password_hash, is_active, created_at "
                  "FROM users WHERE email = :email");
    query.bindValue(":email", email);

    if (!query.exec()) {
        Logger::logError("UserRepository::findByEmail failed: " + query.lastError().text());
        return std::nullopt;
    }

    if (query.next()) {
        User u;
        u.userId = query.value("user_id").toInt();
        u.role = User::stringToRole(query.value("role").toString());
        u.code = query.value("code").toString();
        u.fullName = query.value("full_name").toString();
        u.email = query.value("email").toString();
        u.passwordHash = query.value("password_hash").toString();
        u.isActive = query.value("is_active").toBool();
        u.createdAt = query.value("created_at").toDateTime();
        return u;
    }
    return std::nullopt;
}

std::optional<User> UserRepository::findById(int userId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT user_id, role, code, full_name, email, password_hash, is_active, created_at "
                  "FROM users WHERE user_id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        User u;
        u.userId = query.value("user_id").toInt();
        u.role = User::stringToRole(query.value("role").toString());
        u.code = query.value("code").toString();
        u.fullName = query.value("full_name").toString();
        u.email = query.value("email").toString();
        u.passwordHash = query.value("password_hash").toString();
        u.isActive = query.value("is_active").toBool();
        u.createdAt = query.value("created_at").toDateTime();
        return u;
    }
    return std::nullopt;
}

std::optional<User> UserRepository::findByCode(const QString& code) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT user_id, role, code, full_name, email, password_hash, is_active, created_at "
                  "FROM users WHERE code = :code");
    query.bindValue(":code", code);

    if (query.exec() && query.next()) {
        User u;
        u.userId = query.value("user_id").toInt();
        u.role = User::stringToRole(query.value("role").toString());
        u.code = query.value("code").toString();
        u.fullName = query.value("full_name").toString();
        u.email = query.value("email").toString();
        u.passwordHash = query.value("password_hash").toString();
        u.isActive = query.value("is_active").toBool();
        u.createdAt = query.value("created_at").toDateTime();
        return u;
    }
    return std::nullopt;
}

std::vector<User> UserRepository::findAll(const QString& roleFilter, const QString& search) {
    std::vector<User> list;
    QString sql = "SELECT user_id, role, code, full_name, email, password_hash, is_active, created_at FROM users WHERE 1=1";
    if (!roleFilter.isEmpty()) {
        sql += " AND role = :role";
    }
    if (!search.isEmpty()) {
        sql += " AND (full_name LIKE :search OR email LIKE :search OR code LIKE :search)";
    }

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare(sql);
    if (!roleFilter.isEmpty()) query.bindValue(":role", roleFilter);
    if (!search.isEmpty()) query.bindValue(":search", "%" + search + "%");

    if (query.exec()) {
        while (query.next()) {
            User u;
            u.userId = query.value("user_id").toInt();
            u.role = User::stringToRole(query.value("role").toString());
            u.code = query.value("code").toString();
            u.fullName = query.value("full_name").toString();
            u.email = query.value("email").toString();
            u.passwordHash = query.value("password_hash").toString();
            u.isActive = query.value("is_active").toBool();
            u.createdAt = query.value("created_at").toDateTime();
            list.push_back(u);
        }
    }
    return list;
}

bool UserRepository::createUser(const User& user) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO users (role, code, full_name, email, password_hash, is_active, created_at) "
                  "VALUES (:role, :code, :full_name, :email, :password_hash, :is_active, NOW())");
    query.bindValue(":role", User::roleToString(user.role));
    query.bindValue(":code", user.code.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : user.code);
    query.bindValue(":full_name", user.fullName);
    query.bindValue(":email", user.email);
    query.bindValue(":password_hash", user.passwordHash);
    query.bindValue(":is_active", user.isActive);

    if (!query.exec()) {
        Logger::logError("UserRepository::createUser failed: " + query.lastError().text());
        return false;
    }
    return true;
}

bool UserRepository::updateUser(const User& user) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE users SET full_name = :full_name, email = :email, code = :code WHERE user_id = :id");
    query.bindValue(":full_name", user.fullName);
    query.bindValue(":email", user.email);
    query.bindValue(":code", user.code);
    query.bindValue(":id", user.userId);
    return query.exec();
}

bool UserRepository::setActiveStatus(int userId, bool isActive) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE users SET is_active = :is_active WHERE user_id = :id");
    query.bindValue(":is_active", isActive);
    query.bindValue(":id", userId);
    return query.exec();
}
