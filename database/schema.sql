-- ============================================================
-- Student Attendance System - Database Schema (MySQL 8.0)
-- Group 03 - Software Engineering (CNPM)
-- ============================================================

CREATE DATABASE IF NOT EXISTS student_attendance
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE student_attendance;

-- Tắt kiểm tra khóa ngoại tạm thời để dọn dẹp nếu tạo lại
SET FOREIGN_KEY_CHECKS = 0;
DROP TABLE IF EXISTS attendance_change_logs;
DROP TABLE IF EXISTS attendance_records;
DROP TABLE IF EXISTS attendance_sessions;
DROP TABLE IF EXISTS schedule_entries;
DROP TABLE IF EXISTS enrollments;
DROP TABLE IF EXISTS courses;
DROP TABLE IF EXISTS users;
SET FOREIGN_KEY_CHECKS = 1;

-- ------------------------------------------------------------
-- 1. Bảng users: Lưu tài khoản Sinh viên, Giảng viên, Admin
-- ------------------------------------------------------------
CREATE TABLE users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    role ENUM('STUDENT', 'LECTURER', 'ADMIN') NOT NULL,
    code VARCHAR(20) UNIQUE NULL, -- Mã SV hoặc mã GV (Admin có thể để NULL)
    full_name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    is_active BOOLEAN NOT NULL DEFAULT TRUE,
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 2. Bảng courses: Môn học do 1 Giảng viên phụ trách
-- ------------------------------------------------------------
CREATE TABLE courses (
    course_id INT AUTO_INCREMENT PRIMARY KEY,
    code VARCHAR(20) UNIQUE NOT NULL, -- Mã môn (VD: CE201-01)
    name VARCHAR(150) NOT NULL,
    user_id INT NOT NULL, -- Giảng viên phụ trách
    CONSTRAINT fk_courses_lecturer FOREIGN KEY (user_id) 
        REFERENCES users(user_id) ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 3. Bảng enrollments: Sinh viên ghi danh vào môn học
-- ------------------------------------------------------------
CREATE TABLE enrollments (
    enrollment_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL, -- Sinh viên
    course_id INT NOT NULL,
    enrolled_at DATE NOT NULL,
    CONSTRAINT fk_enrollments_student FOREIGN KEY (user_id) 
        REFERENCES users(user_id) ON DELETE RESTRICT,
    CONSTRAINT fk_enrollments_course FOREIGN KEY (course_id) 
        REFERENCES courses(course_id) ON DELETE CASCADE,
    CONSTRAINT uq_enrollment UNIQUE (user_id, course_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 4. Bảng attendance_sessions: Các phiên điểm danh mở bởi GV
-- ------------------------------------------------------------
CREATE TABLE attendance_sessions (
    session_id INT AUTO_INCREMENT PRIMARY KEY,
    course_id INT NOT NULL,
    user_id INT NOT NULL, -- Giảng viên mở phiên
    start_time DATETIME NOT NULL,
    duration_minutes INT NOT NULL,
    present_window_minutes INT NOT NULL DEFAULT 20,
    late_window_minutes INT NOT NULL DEFAULT 80,
    session_code VARCHAR(10) NOT NULL, -- Mã xoay vòng 60 giây
    code_generated_at DATETIME NOT NULL,
    status ENUM('OPEN', 'CLOSED') NOT NULL DEFAULT 'OPEN',
    closed_at DATETIME NULL,
    CONSTRAINT fk_sessions_course FOREIGN KEY (course_id) 
        REFERENCES courses(course_id) ON DELETE RESTRICT,
    CONSTRAINT fk_sessions_lecturer FOREIGN KEY (user_id) 
        REFERENCES users(user_id) ON DELETE RESTRICT,
    CONSTRAINT chk_session_windows CHECK (
        present_window_minutes > 0 AND 
        present_window_minutes <= late_window_minutes AND 
        late_window_minutes <= duration_minutes
    )
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 5. Bảng attendance_records: Kết quả điểm danh của từng SV
-- ------------------------------------------------------------
CREATE TABLE attendance_records (
    record_id INT AUTO_INCREMENT PRIMARY KEY,
    session_id INT NOT NULL,
    user_id INT NOT NULL, -- Sinh viên
    status ENUM('PRESENT', 'LATE', 'ABSENT', 'EXCUSED') NOT NULL,
    check_in_time DATETIME NULL,
    CONSTRAINT fk_records_session FOREIGN KEY (session_id) 
        REFERENCES attendance_sessions(session_id) ON DELETE CASCADE,
    CONSTRAINT fk_records_student FOREIGN KEY (user_id) 
        REFERENCES users(user_id) ON DELETE RESTRICT,
    CONSTRAINT uq_session_student UNIQUE (session_id, user_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 6. Bảng attendance_change_logs: Lịch sử giảng viên sửa điểm danh
-- ------------------------------------------------------------
CREATE TABLE attendance_change_logs (
    log_id INT AUTO_INCREMENT PRIMARY KEY,
    record_id INT NOT NULL,
    old_status ENUM('PRESENT', 'LATE', 'ABSENT', 'EXCUSED') NOT NULL,
    new_status ENUM('PRESENT', 'LATE', 'ABSENT', 'EXCUSED') NOT NULL,
    reason VARCHAR(255) NULL,
    changed_at DATETIME NOT NULL,
    user_id INT NOT NULL, -- Giảng viên thực hiện thay đổi
    CONSTRAINT fk_logs_record FOREIGN KEY (record_id) 
        REFERENCES attendance_records(record_id) ON DELETE CASCADE,
    CONSTRAINT fk_logs_lecturer FOREIGN KEY (user_id) 
        REFERENCES users(user_id) ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- 7. Bảng schedule_entries: Lịch học cố định theo tuần
-- ------------------------------------------------------------
CREATE TABLE schedule_entries (
    schedule_id INT AUTO_INCREMENT PRIMARY KEY,
    course_id INT NOT NULL,
    type ENUM('CLASS', 'STUDY') NOT NULL,
    day_of_week TINYINT NOT NULL, -- 1 = Thứ 2, 7 = Chủ nhật
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    room VARCHAR(50) NOT NULL,
    start_date DATE NOT NULL,
    end_date DATE NOT NULL,
    CONSTRAINT fk_schedules_course FOREIGN KEY (course_id) 
        REFERENCES courses(course_id) ON DELETE CASCADE,
    CONSTRAINT chk_schedule_rules CHECK (
        day_of_week BETWEEN 1 AND 7 AND 
        start_time < end_time AND 
        start_date <= end_date
    )
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ------------------------------------------------------------
-- Indexes tối ưu hóa tốc độ truy vấn (Mục 7.6)
-- ------------------------------------------------------------
CREATE INDEX idx_sessions_course_status ON attendance_sessions(course_id, status);
CREATE INDEX idx_records_student ON attendance_records(user_id);
CREATE INDEX idx_enrollments_student ON enrollments(user_id);
CREATE INDEX idx_courses_lecturer ON courses(user_id);
CREATE INDEX idx_schedules_course_date ON schedule_entries(course_id, start_date, end_date);
