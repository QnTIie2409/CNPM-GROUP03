-- ============================================================
-- Student Attendance System - Seed Data (Dữ liệu mẫu)
-- Group 03 - Software Engineering (CNPM)
-- Mật khẩu mặc định cho tất cả các tài khoản là: 123456
-- SHA-256 hash của "123456": pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b
-- ============================================================

USE student_attendance;

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
(2, '012008010322', 'Tư duy thiết kế và đổi mới sáng tạo', 3),
(3, '012012103105', 'Lập trình Web', 2),
(4, '012012210504', 'Công nghệ phần mềm', 2),
(5, '012000510612', 'Kinh tế chính trị Mác - Lênin', 3);

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
(1, 5, 'CLASS', 1, '09:25:00', '11:55:00', 'E301 - CS1 (P.Thạnh Mỹ Tây, TP.HCM)', '2026-10-12', '2026-12-06'),

-- Thứ 3: Công nghệ phần mềm (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(2, 4, 'CLASS', 2, '09:25:00', '11:55:00', 'B003 - CS1 (P.Thạnh Mỹ Tây, TP.HCM)', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(3, 4, 'STUDY', 2, '17:30:00', '19:30:00', 'Online (Hệ thống: Courses.ut.edu.vn)', '2026-10-26', '2026-11-29'),

-- Thứ 4: Lập trình Web (3 tín)
--  + Lớp học trên giảng đường (Ca 4: 14:50 - 17:20, suốt 8 tuần)
(4, 3, 'CLASS', 3, '14:50:00', '17:20:00', 'A205 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học bài tập LMS (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(5, 3, 'STUDY', 3, '17:30:00', '19:30:00', 'ONLINE (Tự học & Bài tập LMS)', '2026-10-26', '2026-11-29'),

-- Thứ 5: Tư duy thiết kế và đổi mới sáng tạo (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(6, 2, 'CLASS', 4, '09:25:00', '11:55:00', 'B102 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(7, 2, 'STUDY', 4, '17:30:00', '19:30:00', 'Online (Hệ thống: Courses.ut.edu.vn)', '2026-10-26', '2026-11-29'),

-- Thứ 6: Phân tích thiết kế giải thuật (3 tín)
--  + Lớp học trên giảng đường (Ca 2: 09:25 - 11:55, suốt 8 tuần)
(8, 1, 'CLASS', 5, '09:25:00', '11:55:00', 'C201 - CS1', '2026-10-12', '2026-12-06'),
--  + E-learning tự học (Ca 5: 17:30 - 19:30, 5 tuần từ tuần 3 đến tuần 7)
(9, 1, 'STUDY', 5, '17:30:00', '19:30:00', 'Online (Hệ thống: Courses.ut.edu.vn)', '2026-10-26', '2026-11-29');

-- 5. Nạp phiên điểm danh đang mở thử nghiệm (attendance_sessions)
-- Mở phiên điểm danh cho môn học hôm nay: Kinh tế chính trị Mác - Lênin
INSERT INTO attendance_sessions (session_id, course_id, user_id, start_time, duration_minutes, present_window_minutes, late_window_minutes, session_code, code_generated_at, status) VALUES
(1, 5, 3, NOW(), 80, 20, 80, '528419', NOW(), 'OPEN');
