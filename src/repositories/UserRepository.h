#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../models/User.h"
#include <optional>
#include <vector>

class UserRepository {
public:
    std::optional<User> findByEmail(const QString& email);
    std::optional<User> findById(int userId);
    std::optional<User> findByCode(const QString& code);
    std::vector<User> findAll(const QString& roleFilter = "", const QString& search = "");
    bool createUser(const User& user);
    bool updateUser(const User& user);
    bool setActiveStatus(int userId, bool isActive);
};

#endif
