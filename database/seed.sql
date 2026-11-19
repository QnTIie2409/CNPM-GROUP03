-- ============================================================
-- Student Attendance System - Seed Data (Dữ liệu mẫu)
-- Group 03 - Software Engineering (CNPM)
-- Mật khẩu mặc định cho tất cả các tài khoản là: 123456
-- SHA-256 hash của "123456": pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b
-- ============================================================

USE student_attendance;

-- This file is UTF-8. Declare the import connection encoding explicitly.
SET NAMES utf8mb4;

SET FOREIGN_KEY_CHECKS = 0;
TRUNCATE TABLE attendance_change_logs;
TRUNCATE TABLE attendance_records;
TRUNCATE TABLE attendance_sessions;
TRUNCATE TABLE schedule_entries;
TRUNCATE TABLE enrollments;
TRUNCATE TABLE courses;
TRUNCATE TABLE users;
SET FOREIGN_KEY_CHECKS = 1;

-- 1. Nạp tài khoản users (Admin, Giảng viên, Sinh viên)
INSERT INTO users (user_id, role, code, full_name, email, password_hash, is_active, created_at) VALUES
(1, 'ADMIN', 'admin', 'Lê Hoàng Phúc', 'admin@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(2, 'LECTURER', 'GV0042', 'Dr. Trần Minh Quân', 'quan.tran@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(3, 'LECTURER', 'GV0057', 'Nguyễn Thu Hà', 'ha.nguyen@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(4, 'STUDENT', '2451010001', 'Nguyễn Minh Anh', 'minhanh.nguyen@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(5, 'STUDENT', '2451010002', 'Phạm Quốc Bảo', 'bao.pham@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(6, 'STUDENT', '2451010003', 'Võ Tấn Lộc', 'loc.vo@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(7, 'STUDENT', '2451010004', 'Trần Thảo Vy', 'vy.tran@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW());

-- 2. Nạp khóa học (courses)
-- 5 môn học theo chương trình:
-- 1. Phân tích thiết kế giải thuật (3 tín)
-- 2. Tư duy thiết kế và đổi mới sáng tạo (3 tín)
-- 3. Lập trình Web (3 tín)
-- 4. Công nghệ phần mềm (3 tín)
-- 5. Kinh tế chính trị Mác - Lênin (2 tín)
INSERT INTO courses (course_id, code, name, user_id) VALUES
(1, '012012210501', 'Phân tích thiết kế giải thuật', 2),
(2, '012008010322', 'Tư duy thiết kế và đổi mới sáng tạo', 2),
(3, '012012103105', 'Lập trình Web', 2),
(4, '012012210504', 'Công nghệ phần mềm', 2),
(5, '012000510612', 'Kinh tế chính trị Mác - Lênin', 2);

-- 3. Nạp danh sách ghi danh (enrollments)
INSERT INTO enrollments (enrollment_id, user_id, course_id, enrolled_at) VALUES
-- Sinh viên 4: Nguyễn Minh Anh
(1, 4, 1, '2026-10-12'),
(2, 4, 2, '2026-10-12'),
(3, 4, 3, '2026-10-12'),
(4, 4, 4, '2026-10-12'),
(5, 4, 5, '2026-10-12'),
-- Sinh viên 5: Phạm Quốc Bảo
(6, 5, 1, '2026-10-12'),
(7, 5, 2, '2026-10-12'),
(8, 5, 3, '2026-10-12'),
(9, 5, 4, '2026-10-12'),
(10, 5, 5, '2026-10-12'),
-- Sinh viên 6: Võ Tấn Lộc
(11, 6, 1, '2026-10-12'),
(12, 6, 2, '2026-10-12'),
(13, 6, 3, '2026-10-12'),
(14, 6, 4, '2026-10-12'),
(15, 6, 5, '2026-10-12'),
-- Sinh viên 7: Trần Thảo Vy
(16, 7, 1, '2026-10-12'),
(17, 7, 2, '2026-10-12'),
(18, 7, 3, '2026-10-12'),
(19, 7, 4, '2026-10-12'),
(20, 7, 5, '2026-10-12');

-- 4. Nạp thời khóa biểu (schedule_entries)
-- Học kỳ 8 tuần (~2 tháng): từ 12/10/2026 đến 06/12/2026
-- Quy tắc:
--  + Môn 2 tín (Kinh tế chính trị): Học trực tiếp trên lớp suốt 8 tuần (không có e-learning).
--  + Các môn 3 tín:
--      * Có buổi học trên lớp ban ngày (suốt 8 tuần).
--      * VÀ từ tuần 3 đến tuần 7 (5 tuần: 26/10 đến 29/11): có thêm buổi E-learning / Tự học bài tập LMS buổi tối (Ca 5: 17:30 - 19:30).
INSERT INTO schedule_entries (schedule_id, course_id, type, day_of_week, start_time, end_time, room, start_date, end_date) VALUES
-- Thứ 2: Kinh tế chính trị Mác - Lênin (2 tín) - Ca 2 (09:25 - 11:55) - Trực tiếp 8 tuần (Không e-learning)
(1, 5, 'CLASS', 1, '09:25:00', '11:55:00', 'E301 - CS1', '2026-10-12', '2026-12-06'),

-- Thứ 3: Công nghệ phần mềm (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(2, 4, 'CLASS', 2, '09:25:00', '11:55:00', 'B003 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(3, 4, 'STUDY', 2, '17:30:00', '19:30:00', 'ONLINE', '2026-10-26', '2026-11-29'),

-- Thứ 4: Lập trình Web (3 tín)
--  + Lớp học trên giảng đường (Ca 4: 14:50 - 17:20, suốt 8 tuần)
(4, 3, 'CLASS', 3, '14:50:00', '17:20:00', 'A205 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học bài tập LMS (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(5, 3, 'STUDY', 3, '17:30:00', '19:30:00', 'ONLINE', '2026-10-26', '2026-11-29'),

-- Thứ 5: Tư duy thiết kế và đổi mới sáng tạo (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(6, 2, 'CLASS', 4, '09:25:00', '11:55:00', 'B102 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(7, 2, 'STUDY', 4, '17:30:00', '19:30:00', 'ONLINE', '2026-10-26', '2026-11-29'),

-- Thứ 6: Phân tích thiết kế giải thuật (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(8, 1, 'CLASS', 5, '09:25:00', '11:55:00', 'C201 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(9, 1, 'STUDY', 5, '17:30:00', '19:30:00', 'ONLINE', '2026-10-26', '2026-11-29');

-- 5. Nạp phiên điểm danh & kết quả điểm danh mẫu (attendance_sessions & attendance_records)
-- Phiên đang mở hôm nay: Môn 5 (Kinh tế chính trị Mác - Lênin)
INSERT INTO attendance_sessions (session_id, course_id, user_id, start_time, duration_minutes, present_window_minutes, late_window_minutes, session_code, code_generated_at, status, closed_at) VALUES
(1, 5, 3, NOW(), 80, 20, 80, '528419', NOW(), 'OPEN', NULL),
-- Các phiên đã hoàn thành ở các tuần trước (Môn 5):
(2, 5, 3, '2026-10-19 09:25:00', 80, 20, 80, '112233', '2026-10-19 09:25:00', 'CLOSED', '2026-10-19 10:45:00'),
(3, 5, 3, '2026-10-26 09:25:00', 80, 20, 80, '223344', '2026-10-26 09:25:00', 'CLOSED', '2026-10-26 10:45:00'),
(4, 5, 3, '2026-11-02 09:25:00', 80, 20, 80, '334455', '2026-11-02 09:25:00', 'CLOSED', '2026-11-02 10:45:00'),
(5, 5, 3, '2026-11-09 09:25:00', 80, 20, 80, '445566', '2026-11-09 09:25:00', 'CLOSED', '2026-11-09 10:45:00'),
-- Các phiên đã hoàn thành (Môn 4 - Công nghệ phần mềm):
(6, 4, 2, '2026-10-20 09:25:00', 80, 20, 80, '556677', '2026-10-20 09:25:00', 'CLOSED', '2026-10-20 10:45:00'),
(7, 4, 2, '2026-10-27 09:25:00', 80, 20, 80, '667788', '2026-10-27 09:25:00', 'CLOSED', '2026-10-27 10:45:00'),
(8, 4, 2, '2026-11-03 09:25:00', 80, 20, 80, '778899', '2026-11-03 09:25:00', 'CLOSED', '2026-11-03 10:45:00'),
(9, 4, 2, '2026-11-10 09:25:00', 80, 20, 80, '889900', '2026-11-10 09:25:00', 'CLOSED', '2026-11-10 10:45:00'),
-- Các phiên đã hoàn thành (Môn 1 - Phân tích thiết kế giải thuật):
(10, 1, 2, '2026-10-15 09:25:00', 80, 20, 80, '123456', '2026-10-15 09:25:00', 'CLOSED', '2026-10-15 10:45:00'),
(11, 1, 2, '2026-10-22 09:25:00', 80, 20, 80, '234567', '2026-10-22 09:25:00', 'CLOSED', '2026-10-22 10:45:00'),
(12, 1, 2, '2026-10-29 09:25:00', 80, 20, 80, '345678', '2026-10-29 09:25:00', 'CLOSED', '2026-10-29 10:45:00'),
(13, 1, 2, '2026-11-05 09:25:00', 80, 20, 80, '456789', '2026-11-05 09:25:00', 'CLOSED', '2026-11-05 10:45:00'),
(14, 1, 2, '2026-11-12 09:25:00', 80, 20, 80, '567890', '2026-11-12 09:25:00', 'CLOSED', '2026-11-12 10:45:00'),
-- Các phiên đã hoàn thành (Môn 3 - Lập trình Web):
(15, 3, 2, '2026-10-21 14:50:00', 80, 20, 80, '678901', '2026-10-21 14:50:00', 'CLOSED', '2026-10-21 16:10:00'),
(16, 3, 2, '2026-10-28 14:50:00', 80, 20, 80, '789012', '2026-10-28 14:50:00', 'CLOSED', '2026-10-28 16:10:00'),
(17, 3, 2, '2026-11-04 14:50:00', 80, 20, 80, '890123', '2026-11-04 14:50:00', 'CLOSED', '2026-11-04 16:10:00'),
(18, 3, 2, '2026-11-11 14:50:00', 80, 20, 80, '901234', '2026-11-11 14:50:00', 'CLOSED', '2026-11-11 16:10:00'),
-- Các phiên đã hoàn thành (Môn 2 - Tư duy thiết kế):
(19, 2, 3, '2026-10-23 09:25:00', 80, 20, 80, '012345', '2026-10-23 09:25:00', 'CLOSED', '2026-10-23 10:45:00'),
(20, 2, 3, '2026-10-30 09:25:00', 80, 20, 80, '135790', '2026-10-30 09:25:00', 'CLOSED', '2026-10-30 10:45:00'),
(21, 2, 3, '2026-11-06 09:25:00', 80, 20, 80, '246801', '2026-11-06 09:25:00', 'CLOSED', '2026-11-06 10:45:00'),
(22, 2, 3, '2026-11-13 09:25:00', 80, 20, 80, '357912', '2026-11-13 09:25:00', 'CLOSED', '2026-11-13 10:45:00');

-- 6. Nạp kết quả điểm danh của sinh viên 4 (Nguyễn Minh Anh) và các sinh viên khác
INSERT INTO attendance_records (record_id, session_id, user_id, status, check_in_time) VALUES
-- Môn 5 cho Sinh viên 4 (Nguyễn Minh Anh):
(1, 2, 4, 'PRESENT', '2026-10-19 09:28:15'),
(2, 3, 4, 'PRESENT', '2026-10-26 09:31:04'),
(3, 4, 4, 'LATE', '2026-11-02 09:47:20'),
(4, 5, 4, 'PRESENT', '2026-11-09 09:26:40'),
-- Môn 5 cho Sinh viên 5 (Phạm Quốc Bảo):
(5, 2, 5, 'PRESENT', '2026-10-19 09:27:00'),
(6, 3, 5, 'ABSENT', NULL),
(7, 4, 5, 'PRESENT', '2026-11-02 09:35:10'),
(8, 5, 5, 'PRESENT', '2026-11-09 09:30:20'),
-- Môn 4 cho Sinh viên 4 (Nguyễn Minh Anh):
(9, 6, 4, 'PRESENT', '2026-10-20 09:29:10'),
(10, 7, 4, 'PRESENT', '2026-10-27 09:33:05'),
(11, 8, 4, 'PRESENT', '2026-11-03 09:27:50'),
(12, 9, 4, 'EXCUSED', NULL),
-- Môn 1 cho Sinh viên 4 (Nguyễn Minh Anh):
(13, 10, 4, 'PRESENT', '2026-10-15 09:28:10'),
(14, 11, 4, 'PRESENT', '2026-10-22 09:26:45'),
(15, 12, 4, 'LATE', '2026-10-29 09:48:30'),
(16, 13, 4, 'ABSENT', NULL),
(17, 14, 4, 'PRESENT', '2026-11-12 09:29:05'),
-- Môn 3 cho Sinh viên 4 (Nguyễn Minh Anh):
(18, 15, 4, 'PRESENT', '2026-10-21 14:52:10'),
(19, 16, 4, 'PRESENT', '2026-10-28 14:51:30'),
(20, 17, 4, 'PRESENT', '2026-11-04 14:53:00'),
(21, 18, 4, 'LATE', '2026-11-11 15:12:40'),
-- Môn 2 cho Sinh viên 4 (Nguyễn Minh Anh):
(22, 19, 4, 'PRESENT', '2026-10-23 09:27:00'),
(23, 20, 4, 'PRESENT', '2026-10-30 09:26:15'),
(24, 21, 4, 'PRESENT', '2026-11-06 09:30:10'),
(25, 22, 4, 'PRESENT', '2026-11-13 09:28:40');
