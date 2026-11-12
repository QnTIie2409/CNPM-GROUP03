#pragma once

#include <QString>

#include "ServiceCommon.h"

// FR-14 | UC-13
class AccountService
{
public:
    // Tạo tài khoản sinh viên / giảng viên.
    // role: "STUDENT" hoặc "LECTURER". Mã (code) và email phải duy nhất.
    // Thành công: ServiceResult::id là user_id mới.
    static ServiceResult createAccount(const QString &role, const QString &code,
                                       const QString &fullName, const QString &email,
                                       const QString &password);

    // Vô hiệu hóa tài khoản (is_active = false), không xóa dữ liệu.
    static ServiceResult deactivate(int userId);
};
