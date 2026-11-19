# Project Status and FR Checklist

## Mục đích

File này ghi lại trạng thái hiện tại của các Functional Requirements để nhóm kiểm tra giao diện, kết nối backend và chuẩn bị test thủ công. Đây là checklist làm việc; một chức năng chỉ được đánh dấu Pass sau khi đã chạy thử với database thật.

## Phần đã có nền tảng hoạt động

### FR-01 Login

- Login bằng email hoặc mã tài khoản.
- Kiểm tra mật khẩu bằng salted PBKDF2 hash cho dữ liệu mới.
- Từ chối tài khoản inactive.
- Lấy role thật từ database để mở workspace Student, Lecturer hoặc Admin.
- Có thông báo lỗi đăng nhập chung để không tiết lộ email hay password sai.

### FR-02 Logout

- Có nút Sign out trên AppShell.
- Xóa user hiện tại trong `AuthService`.
- Quay lại màn hình login.

### FR-04, FR-05, FR-06 Attendance session

- Lecturer chỉ mở được session cho course mình phụ trách.
- Kiểm tra `Present window <= Late window <= Duration`.
- Tạo mã session 6 chữ số.
- Lưu session và mã vào MySQL.
- Xoay mã sau 60 giây.
- Hiển thị mã hiện tại cho Lecturer.
- Đóng session thủ công.
- Tự đóng session khi hết duration.
- Tạo record `ABSENT` cho Student chưa check-in khi session đóng.
- Các thao tác đóng session và đánh dấu Absent được thực hiện trong transaction.

### FR-10, FR-11 Student check-in

- Student phải được enroll trong course.
- Kiểm tra session đang mở.
- Kiểm tra mã hiện tại.
- Từ chối check-in trùng.
- Tính Present/Late theo thời gian session.
- Lưu thời gian check-in bằng `NOW()` của MySQL trong thao tác lưu record.
- Database có unique key `(session_id, user_id)` để chặn record trùng.

### FR-07, FR-08 Attendance report and correction

- Lecturer có thể đọc attendance records theo session.
- Có bảng Student, status và check-in time.
- Có thể sửa status thành Present, Late, Absent hoặc Excused.
- Mỗi lần sửa tạo `attendance_change_logs` với old status, new status, reason, thời gian và Lecturer.
- Update record và insert change log nằm trong transaction.

### FR-09, FR-13 Timetable

- Có `TimetableService`.
- Student đọc timetable theo enrollment.
- Lecturer đọc timetable theo course mình dạy.
- Có hiển thị dạng lịch với ngày, giờ, course, loại Class/Study và phòng.
- Schedule entry có khoảng ngày áp dụng.

### FR-12 Student attendance

- Student đọc records theo course.
- Tính attendance rate theo công thức:

```text
(Present + Late + Excused) / completed sessions
```

### FR-14, FR-15, FR-16, FR-17 Admin backend

- Có tạo tài khoản Student/Lecturer.
- Có deactivate account.
- Có tạo, sửa, xóa course.
- Có gán Lecturer cho course.
- Có enroll và unenroll Student.
- Có thêm và xóa schedule entry.
- Database có constraint cho email, code, enrollment trùng và session/course.

## Những phần UI cần kiểm tra hoặc bổ sung

### FR-03 Phân quyền

- Student chỉ thấy Overview, Timetable và My attendance.
- Lecturer chỉ thấy chức năng course mình dạy, session, attendance và timetable.
- Admin chỉ thấy account, course, enrollment và timetable management.
- Admin không được mở attendance report.
- Guest không được thấy dữ liệu trước khi login.
- Backend Service cũng phải tự kiểm tra quyền, không chỉ ẩn menu trên UI.

### FR-04 Mở session

- Bổ sung hoặc kiểm tra preset Standard:
  - Present 20 phút.
  - Late đến phút 80.
- Bổ sung hoặc kiểm tra preset Quick:
  - 5 phút.
  - Không có Late window.
- Hiển thị lỗi ngay tại form khi window không hợp lệ.

### FR-06 Đóng session

- Có confirmation trước khi đóng session.
- Confirmation nên hiển thị số Student sẽ bị đánh dấu Absent.
- Sau khi đóng, UI phải ngừng hiển thị mã hiện tại.
- Trang kết quả cuối session phải hiển thị được.

### FR-07, FR-08 Attendance UI

- Chọn course rồi chọn session.
- Hiển thị tổng Present, Late, Absent, Excused.
- Tìm Student theo tên hoặc mã.
- Dialog xác nhận trước khi sửa status.
- Nhập reason khi sửa.
- Hiển thị audit history sau khi sửa.

### FR-09, FR-13 Timetable UI

- Kiểm tra day view.
- Kiểm tra week view.
- Kiểm tra month view.
- Kiểm tra các buổi Class và Study.
- Kiểm tra lọc đúng theo enrollment của Student.
- Kiểm tra Lecturer không thấy timetable của course người khác.

### FR-14 Account UI

- Thêm màn hình edit account.
- Hiển thị trạng thái active/inactive.
- Có confirmation khi deactivate.
- Kiểm tra email format và code trùng.
- Nếu nhóm hỗ trợ reactivate thì thêm nút reactivate và cập nhật requirement tương ứng.

### FR-15, FR-16 Course and enrollment UI

- Course list cần hiển thị Lecturer, số Student và số session.
- Không cho xóa course đã có session.
- Quản lý danh sách Student trong course.
- Confirmation khi xóa course.
- Confirmation khi xóa enrollment nếu UI cần hành động nguy hiểm.

### FR-17 Schedule UI

- Thêm schedule entry.
- Sửa schedule entry.
- Xóa schedule entry với confirmation.
- Kiểm tra giờ bắt đầu nhỏ hơn giờ kết thúc.
- Kiểm tra ngày bắt đầu không sau ngày kết thúc.
- Kiểm tra lịch bị overlap.

## Các vấn đề backend cần xử lý sau khi hoàn thiện UI chính

- Một số Service vẫn chứa SQL trực tiếp; cần chuyển SQL về Repository để đạt NFR-16.
- `AttendanceService` cần kiểm tra Lecturer sở hữu course/session trước khi đọc hoặc sửa attendance.
- `CheckInService` không nên fallback sang giờ máy nếu truy vấn giờ MySQL thất bại; nên trả lỗi hệ thống.
- Một số lỗi query dùng `qWarning()` thay vì ghi đầy đủ vào `app.log`.
- `TimetableService` còn thiếu update schedule và overlap validation.
- `AccountService` còn thiếu edit account và có thể thiếu reactivate tùy quyết định của nhóm.
- Cần kiểm tra lại lỗi thời gian biên: Present/Late window và thời điểm session hết hạn.

## Test FR chính cần chạy

| ID | Kịch bản | Kết quả cần đạt |
|---|---|---|
| FR-T01 | Login bằng email đúng | Mở đúng workspace theo role |
| FR-T02 | Login bằng mã đúng | Mở đúng workspace theo role |
| FR-T03 | Sai password | Hiện lỗi chung, không lộ thông tin |
| FR-T04 | Lecturer mở course của mình | Tạo session và mã 6 số |
| FR-T05 | Lecturer mở course người khác | Bị từ chối |
| FR-T06 | Mã session sau 60 giây | Mã đổi và UI nhận mã mới |
| FR-T07 | Student đã enroll check-in | Record Present hoặc Late được tạo |
| FR-T08 | Student chưa enroll check-in | Bị từ chối |
| FR-T09 | Student nhập mã cũ | Bị từ chối |
| FR-T10 | Student check-in lần hai | Bị từ chối, chỉ có một record |
| FR-T11 | Đóng session thủ công | Student chưa check-in thành Absent |
| FR-T12 | Session hết duration | Tự đóng và tạo Absent |
| FR-T13 | Lecturer xem report | Chỉ thấy course mình dạy |
| FR-T14 | Lecturer sửa attendance | Record đổi và có change log |
| FR-T15 | Student xem attendance | Chỉ thấy records của mình |
| FR-T16 | Student xem timetable | Chỉ thấy course đã enroll |
| FR-T17 | Lecturer xem timetable | Chỉ thấy course mình dạy |
| FR-T18 | Admin tạo account/course | Dữ liệu được lưu đúng |
| FR-T19 | Admin enroll/unenroll | Enrollment thay đổi, records cũ vẫn còn |
| FR-T20 | Admin thêm/sửa/xóa schedule | Timetable cập nhật đúng |

## Cách đánh giá trạng thái

- **Pass:** chạy đúng với database thật và đúng kết quả mong đợi.
- **Partial:** backend hoặc UI đã có nhưng chưa hoàn chỉnh, hoặc chưa kiểm thử đủ.
- **Fail:** chạy sai, thiếu chức năng, hoặc vi phạm rule trong Requirement Document.
- **Blocked:** chưa thể test vì MySQL, dữ liệu seed, môi trường build hoặc máy test chưa sẵn sàng.

## Về lỗi phần mềm

Không có sản phẩm thực tế nào có thể chứng minh là hoàn toàn không còn lỗi. Kiểm thử chỉ giúp tìm lỗi trong phạm vi các kịch bản đã kiểm tra. Mục tiêu của nhóm là kiểm soát lỗi bằng cách:

1. Xác định rõ requirement và tiêu chí Pass.
2. Viết test case theo từng FR/NFR.
3. Ưu tiên sửa lỗi ảnh hưởng đến dữ liệu, bảo mật, phân quyền và attendance.
4. Chạy regression test sau mỗi thay đổi lớn.
5. Ghi lại lỗi, nguyên nhân, cách sửa và kết quả kiểm tra.
6. Đo NFR bằng số liệu thay vì chỉ nói hệ thống chạy tốt.

Một sản phẩm được xem là ổn định khi các chức năng quan trọng đã có test case, lỗi nghiêm trọng đã được xử lý, dữ liệu không bị sai hoặc mất, và nhóm có bằng chứng kiểm thử cho các yêu cầu chính. Không cần chờ đến khi đạt trạng thái không có lỗi tuyệt đối mới được xem là sản phẩm tốt.

