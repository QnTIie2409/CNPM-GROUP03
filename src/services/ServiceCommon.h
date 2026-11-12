#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

// Kết quả chung cho các thao tác ghi dữ liệu của service.
struct ServiceResult
{
    bool ok = false;
    QString message;   // thông báo hiển thị cho người dùng
    qint64 id = 0;     // id bản ghi vừa tạo (nếu có)

    static ServiceResult success(const QString &msg = QString(), qint64 newId = 0)
    {
        ServiceResult r;
        r.ok = true;
        r.message = msg;
        r.id = newId;
        return r;
    }

    static ServiceResult failure(const QString &msg)
    {
        ServiceResult r;
        r.ok = false;
        r.message = msg;
        return r;
    }
};

// Kết nối CSDL dùng chung cho mọi service.
// Mặc định dùng kết nối mặc định của Qt (QSqlDatabase::addDatabase không đặt tên).
// Nếu nhóm đặt tên kết nối riêng thì chỉ cần sửa ở đây.
inline QSqlDatabase serviceDb()
{
    return QSqlDatabase::database();
}

// true nếu lỗi do vi phạm ràng buộc UNIQUE (MySQL error 1062).
inline bool isDuplicateKeyError(const QSqlError &e)
{
    return e.nativeErrorCode() == QLatin1String("1062")
           || e.text().contains(QLatin1String("Duplicate entry"), Qt::CaseInsensitive);
}

// true nếu lỗi do vi phạm khóa ngoại (MySQL error 1451/1452).
inline bool isForeignKeyError(const QSqlError &e)
{
    return e.nativeErrorCode() == QLatin1String("1451")
           || e.nativeErrorCode() == QLatin1String("1452")
           || e.text().contains(QLatin1String("foreign key"), Qt::CaseInsensitive);
}
