#pragma once

#include <QString>

#include "ServiceCommon.h"

class AccountService
{
public:
    static ServiceResult createAccount(const QString &role, const QString &code,
                                       const QString &fullName, const QString &email,
                                       const QString &password);

    static ServiceResult deactivate(int userId);
};
