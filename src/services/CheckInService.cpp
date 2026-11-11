#include "CheckInService.h"
#include <QDateTime>

CheckInService& CheckInService::instance() {
    static CheckInService inst;
    return inst;
}

CheckInResponse CheckInService::submitCheckIn(int sessionId, int studentUserId, const QString& enteredCode) {
    CheckInResponse resp;

    // 1. Kiểm tra session có tồn tại và đang mở không
    auto sOpt = m_sessionRepo.findById(sessionId);
    if (!sOpt.has_value() || sOpt->status != SessionStatus::OPEN) {
        resp.code = CheckInResultCode::ERROR_SESSION_CLOSED;
        resp.message = "Phiên điểm danh đã đóng hoặc không tồn tại.";
        return resp;
    }

    const AttendanceSession& session = sOpt.value();

    // 2. Kiểm tra sinh viên có ghi danh môn học này không
    if (!m_courseRepo.isStudentEnrolled(studentUserId, session.courseId)) {
        resp.code = CheckInResultCode::ERROR_NOT_ENROLLED;
        resp.message = "Bạn không có tên trong danh sách ghi danh môn học này.";
        return resp;
    }

    // 3. Kiểm tra sinh viên đã điểm danh phiên này chưa
    if (m_attendanceRepo.findRecord(sessionId, studentUserId).has_value()) {
        resp.code = CheckInResultCode::ERROR_ALREADY_CHECKED_IN;
        resp.message = "Bạn đã hoàn thành điểm danh cho phiên này rồi.";
        return resp;
    }

    // 4. Kiểm tra mã 6 số
    if (enteredCode.trimmed() != session.sessionCode.trimmed()) {
        resp.code = CheckInResultCode::ERROR_WRONG_CODE;
        resp.message = "Mã điểm danh không chính xác hoặc đã hết hạn (xoay mã 60s).";
        return resp;
    }

    // 5. Xác định Present hay Late theo thời gian mở phiên
    QDateTime now = QDateTime::currentDateTime();
    qint64 elapsedMin = session.startTime.secsTo(now) / 60;

    if (elapsedMin > session.lateWindowMinutes || elapsedMin > session.durationMinutes) {
        resp.code = CheckInResultCode::ERROR_SESSION_CLOSED;
        resp.message = "Thời gian điểm danh (Present & Late window) cho phiên này đã kết thúc.";
        return resp;
    }

    AttendanceStatus status = (elapsedMin <= session.presentWindowMinutes) ? AttendanceStatus::PRESENT : AttendanceStatus::LATE;

    // 6. Lưu vào cơ sở dữ liệu
    if (m_attendanceRepo.saveCheckIn(sessionId, studentUserId, status)) {
        resp.status = status;
        if (status == AttendanceStatus::PRESENT) {
            resp.code = CheckInResultCode::SUCCESS_PRESENT;
            resp.message = "Điểm danh THÀNH CÔNG (Đúng giờ - Present)!";
        } else {
            resp.code = CheckInResultCode::SUCCESS_LATE;
            resp.message = "Điểm danh THÀNH CÔNG (Đi muộn - Late)!";
        }
    } else {
        resp.code = CheckInResultCode::ERROR_SYSTEM;
        resp.message = "Có lỗi xảy ra khi lưu bản ghi điểm danh.";
    }

    return resp;
}
