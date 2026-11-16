-- ============================================================
-- Student Attendance System - Seed Data (Dữ liệu mẫu)
-- Group 03 - Software Engineering (CNPM)
-- Mật khẩu mặc định cho tất cả các tài khoản là: 123456
-- SHA-256 hash của "123456": pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b
-- ============================================================

USE student_attendance;

-- 1. Nạp tài khoản users
INSERT INTO users (user_id, role, code, full_name, email, password_hash, is_active, created_at) VALUES
(1, 'ADMIN', 'admin', 'Lê Hoàng Phúc', 'admin@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(2, 'LECTURER', 'GV0042', 'Dr. Trần Minh Quân', 'quan.tran@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(3, 'LECTURER', 'GV0057', 'Nguyễn Thu Hà', 'ha.nguyen@uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(4, 'STUDENT', '2451010001', 'Nguyễn Minh Anh', 'minhanh.nguyen@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(5, 'STUDENT', '2451010002', 'Phạm Quốc Bảo', 'bao.pham@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(6, 'STUDENT', '2451010003', 'Võ Tấn Lộc', 'loc.vo@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW()),
(7, 'STUDENT', '2451010004', 'Trần Thảo Vy', 'vy.tran@sv.uth.edu.vn', 'pbkdf2_sha256$100000$00112233445566778899aabbccddeeff$1d44f879854c5e777dd01c8275d58b1e976e7038ae2860fd82d69d1d2f3d610b', TRUE, NOW());

-- 2. Nạp khóa học (courses)
INSERT INTO courses (course_id, code, name, user_id) VALUES
(1, 'CE201-01', 'Transport Planning', 2),
(2, 'MA201-02', 'Applied Mathematics', 3);

-- 3. Nạp danh sách ghi danh (enrollments)
INSERT INTO enrollments (enrollment_id, user_id, course_id, enrolled_at) VALUES
(1, 4, 1, '2026-09-01'),
(2, 5, 1, '2026-09-01'),
(3, 6, 1, '2026-09-01'),
(4, 7, 1, '2026-09-01'),
(5, 4, 2, '2026-09-01'),
(6, 5, 2, '2026-09-01');

-- 4. Nạp thời khóa biểu (schedule_entries)
INSERT INTO schedule_entries (schedule_id, course_id, type, day_of_week, start_time, end_time, room, start_date, end_date) VALUES
(1, 1, 'CLASS', 1, '08:00:00', '09:30:00', 'B.402', '2026-09-01', '2026-12-30'),
(2, 1, 'CLASS', 3, '13:00:00', '14:30:00', 'B.405', '2026-09-01', '2026-12-30'),
(3, 2, 'CLASS', 1, '10:00:00', '11:30:00', 'A.203', '2026-09-01', '2026-12-30'),
(4, 2, 'CLASS', 4, '14:55:00', '17:15:00', 'B.301', '2026-09-01', '2026-12-30'),
(5, 1, 'CLASS', 5, '09:25:00', '11:55:00', 'E.301', '2026-09-01', '2026-12-30'),
(6, 2, 'CLASS', 3, '09:25:00', '11:55:00', 'C.202', '2026-09-01', '2026-12-30');

-- 5. Nạp phiên điểm danh đang mở thử nghiệm (attendance_sessions)
INSERT INTO attendance_sessions (session_id, course_id, user_id, start_time, duration_minutes, present_window_minutes, late_window_minutes, session_code, code_generated_at, status) VALUES
(1, 1, 2, NOW(), 80, 20, 80, '528419', NOW(), 'OPEN');
