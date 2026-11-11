# Hệ Thống Điểm Danh Sinh Viên (Student Attendance System)
> **Môn học:** Công nghệ phần mềm (Software Engineering) - Nhóm 03  
> **Repository:** [https://github.com/QnTIie2409/CNPM-GROUP03](https://github.com/QnTIie2409/CNPM-GROUP03)  
> **Công nghệ:** C++ (Qt 6.12.0 MinGW 64-bit), MySQL 8.0, CMake, Ninja, VS Code.

---

## 📁 Cấu Trúc Dự Án (Kiến Trúc 3 Tầng Chuẩn)

```text
CNPM-GROUP03/
├── database/
│   ├── schema.sql           # Script khởi tạo 7 bảng MySQL & Indexes
│   └── seed.sql             # Script nạp dữ liệu mẫu (Admin, GV, SV, Lớp, Môn)
├── resources/
│   └── style.qss            # Theme màu sắc, giao diện chuẩn GUI 2.0 (UTH Navy)
├── src/
│   ├── main.cpp             # Khởi động ứng dụng Qt
│   ├── mainwindow.h/.cpp    # Cửa sổ chính & test kết nối
│   ├── mainwindow.ui        # File giao diện Qt Designer
│   ├── models/              # Định nghĩa thực thể dữ liệu (User, Course, Session,...)
│   ├── repositories/        # Tầng Data Access (Chạy SQL qua QSqlQuery prepared)
│   ├── services/            # Tầng Business Logic (Nghiệp vụ điểm danh, xoay mã,...)
│   ├── ui/                  # Tầng Presentation (Các màn hình .ui theo từng Role)
│   └── utils/               # Tiện ích đọc config.ini & ghi app.log
├── config.ini               # Cấu hình IP, Port, DB, User, Pass của MySQL
├── CMakeLists.txt           # File cấu hình build CMake
└── .gitignore               # Chặn thư mục build/, file .exe rác đẩy lên Git
```

---

## 👥 Bảng Phân Công Nhiệm Vụ 6 Thành Viên (Deadline: 16/11)

| Thành viên | Vai trò | Trách nhiệm chính | File phụ trách |
| :--- | :--- | :--- | :--- |
| **TV 1** | **Database & Data Access** | Quản lý MySQL schema, dữ liệu mẫu seed, viết 5 Repositories (`QSqlQuery`), `config.ini`, Logger | `database/`, `src/repositories/` |
| **TV 2** | **Backend 1** | Đăng nhập/Đăng xuất (`AuthService`), Quản lý tài khoản SV/GV & Môn học (`AccountService`, `CourseService`) | `src/services/AuthService.*`, `AccountService.*`, `CourseService.*` |
| **TV 3** | **Backend 2** | Mở/đóng phiên điểm danh (`SessionService`), Timer xoay mã 60s, Xử lý check-in SV (`CheckInService`) | `src/services/SessionService.*`, `CheckInService.*` |
| **TV 4** | **Backend 3** | Tính % Chuyên cần, Báo cáo & Giảng viên sửa điểm danh (`AttendanceService`), Thời khóa biểu (`TimetableService`) | `src/services/AttendanceService.*`, `TimetableService.*` |
| **TV 5** | **UI/UX 1** | Màn hình Đăng nhập (`UI-01`), Cụm màn hình Sinh viên (`UI-02` -> `UI-06`), CSS theme `style.qss` | `src/ui/auth/`, `src/ui/student/`, `resources/` |
| **TV 6** | **UI/UX 2** | Cụm màn hình Giảng viên (`UI-07` -> `UI-11`), Admin (`UI-12` -> `UI-14`), Dialogs xác nhận (`UI-15`) | `src/ui/lecturer/`, `src/ui/admin/`, `src/ui/dialogs/` |

---

## 🚀 Hướng Dẫn Cài Đặt & Chạy Trên Máy Cá Nhân

### Bước 1: Khởi tạo Cơ sở dữ liệu MySQL
1. Bật **MySQL** trên máy bạn (qua XAMPP, Laragon hoặc MySQL Community Server).
2. Mở MySQL Workbench hoặc chạy lệnh import:
   * Chạy file `database/schema.sql` (tạo database và 7 bảng).
   * Chạy file `database/seed.sql` (nạp data mẫu).
3. Mở file `config.ini` ở thư mục gốc kiểm tra lại cổng và mật khẩu MySQL của máy bạn (nếu dùng XAMPP thì user là `root`, password để trống).

> **Tài khoản mẫu để đăng nhập thử (Mật khẩu chung: `123456`):**
> * **Admin:** `admin@uth.edu.vn`
> * **Giảng viên:** `quan.tran@uth.edu.vn`
> * **Sinh viên:** `minhanh.nguyen@sv.uth.edu.vn`

### Bước 2: Mở trong VS Code và Biên dịch
1. Mở thư mục dự án bằng VS Code (`File > Open Folder...`).
2. Nếu hiện bảng chọn Kit, chọn: **`Qt 6.12.0 MinGW 64-bit`**.
3. Bấm phím **`F5`** (hoặc nút Play ở thanh dưới) để Build và Run chương trình.

---

## 🌿 Quy Tắc Làm Việc Trên Git (Tránh Xung Đột)

1. **Trước khi bắt đầu code mỗi ngày:**
   ```bash
   git pull origin main
   ```
2. **Tạo nhánh riêng khi làm tính năng:**
   ```bash
   git checkout -b feature/<ten-chuc-nang>
   # Ví dụ: git checkout -b feature/auth-service
   ```
3. **Commit & Push lên nhánh riêng:**
   ```bash
   git add .
   git commit -m "feat: mo ta cong viec da lam"
   git push origin feature/<ten-chuc-nang>
   ```
4. **Không push file rác:** Thư mục `build/` đã được chặn trong `.gitignore`, tuyệt đối không xóa file `.gitignore`.
