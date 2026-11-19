# TỔNG HỢP LÝ THUYẾT CÔNG NGHỆ PHẦN MỀM (SOFTWARE ENGINEERING)

---

## CHƯƠNG 1: TỔNG QUAN VỀ CÔNG NGHỆ PHẦN MỀM (Overview of Software Engineering)

### 1. Các khái niệm cơ bản
* **Phần mềm (Software):** $\text{Software} = \text{Program} + \text{Document}$ (Chương trình + Tài liệu).
* **Sản phẩm phần mềm (Software Product):** $\text{Software Product} = \text{Program} + \text{Document} + \text{Support}$ (Chương trình + Tài liệu + Hỗ trợ).
* **Phân loại sản phẩm phần mềm:**
  * **Generic Product (Sản phẩm đại trà):** Các hệ thống độc lập được tiếp thị và bán rộng rãi cho bất kỳ khách hàng nào có nhu cầu.
  * **Bespoke Product (Sản phẩm đặt hàng/riêng biệt):** Phần mềm được phát triển theo yêu cầu đặt hàng riêng của một khách hàng cụ thể.
* **Lý do chủ quan gây ra vấn đề trong phát triển phần mềm:**
  * Phát triển phần mềm chưa có tính chuyên nghiệp cao, thường mang tính nghệ thuật (tự phát) hơn là khoa học.
  * Quy trình phát triển chưa được thống nhất.
  * Chưa có tiêu chuẩn thống nhất để đo lường phần mềm.
  * *Giải pháp:* Cần có một hoặc nhiều quy trình chuyên nghiệp để nâng cao hiệu quả ngành công nghiệp phần mềm.

### 2. Công nghệ Phần mềm (Software Engineering - SE)
* **Định nghĩa:** Là một ngành kỹ thuật (engineering discipline) quan tâm đến mọi khía cạnh của việc sản xuất phần mềm, từ giai đoạn đầu đặc tả hệ thống cho đến khi bảo trì hệ thống sau khi đưa vào sử dụng.
  * *Tính kỹ thuật:* Sử dụng lý thuyết và phương pháp phù hợp để giải quyết vấn đề trong giới hạn về tổ chức và tài chính.
  * *Mọi khía cạnh:* Không chỉ là kỹ thuật lập trình mà còn bao gồm quản lý dự án, phát triển công cụ, phương pháp hỗ trợ.
* **Tầm quan trọng của SE:** Giúp tạo ra các hệ thống đáng tin cậy một cách nhanh chóng và kinh tế. Về lâu dài, áp dụng SE rẻ hơn nhiều so với lập trình tự phát vì phần lớn chi phí phần mềm nằm ở giai đoạn **thay đổi và bảo trì sau khi sử dụng**.
* **3 yếu tố cấu thành SE:**
  1. **Quy trình / Thủ tục (Process/Procedure):** Xác định các giai đoạn, thứ tự thực hiện, tài liệu/sản phẩm bàn giao (deliverables) và các cột mốc (milestones).
  2. **Phương pháp (Method):** Cách thức cụ thể để hoàn thành từng giai đoạn (phương pháp đặc tả, thiết kế, lập trình, kiểm thử, bảo trì...).
  3. **Công cụ (Tool):** Các công cụ hỗ trợ tự động hoặc bán tự động (CASE tools - *Computer Aided Software Engineering*), ví dụ: IDE (editor, compiler, debugger, testing, version control...).
* **Các nguyên tắc cơ bản của SE:**
  * Hiểu và quản lý tốt quy trình xây dựng phần mềm (mỗi loại phần mềm dùng quy trình khác nhau).
  * Độ tin cậy (reliability) và hiệu năng (performance) luôn quan trọng.
  * Hiểu và quản lý chặt chẽ đặc tả và yêu cầu phần mềm.
  * Ưu tiên tái sử dụng (reuse) phần mềm có sẵn thay vì viết mới hoàn toàn.

### 3. Công nghệ phần mềm trên nền Web (Web-based SE)
* Web trở thành nền tảng chạy ứng dụng; các dịch vụ web (Web services) cho phép truy cập chức năng qua mạng.
* **Điện toán đám mây (Cloud Computing / SaaS):** Người dùng không mua đứt phần mềm mà mua dịch vụ và trả phí theo mức độ sử dụng (*Software as a Service*).
* **Đặc điểm phát triển:**
  * **Tái sử dụng phần mềm (Software reuse):** Lắp ráp từ các thành phần/hệ thống có sẵn là hướng tiếp cận chủ đạo.
  * **Phát triển tăng trưởng và linh hoạt (Incremental & Agile development):** Không thể xác định toàn bộ yêu cầu từ trước, cần phát triển và phân phối theo từng phần tăng dần.

### 4. Quy trình phát triển phần mềm (Software Development Process - SDP)
* **5 giai đoạn chính:**
  1. **Đặc tả / Phân tích (Specification/Analysis):** Xác định yêu cầu và ràng buộc của hệ thống (hiểu rõ bài toán).
  2. **Thiết kế (Design):** Đưa ra giải pháp giải quyết bài toán (kiến trúc, cơ sở dữ liệu, giao diện, thành phần).
  3. **Cài đặt / Lập trình (Implementation/Programming):** Chuyển thiết kế thành chương trình thực thi và gỡ lỗi (debugging).
  4. **Kiểm thử (Testing):** Kiểm tra xem hệ thống có hoạt động đúng như khách hàng mong muốn không (Component testing $\rightarrow$ System testing $\rightarrow$ Acceptance testing).
  5. **Bảo trì / Tiến hóa (Maintenance/Evolution):** Sửa lỗi, thay đổi và nâng cấp hệ thống theo nhu cầu mới.
* **Các mô hình quy trình phần mềm (Software Process Models):**
  * **Mô hình Thác nước (Waterfall model):** Hướng kế hoạch (plan-driven), các giai đoạn phân tách rõ ràng và tuần tự.
  * **Mô hình Tăng trưởng / Lặp (Incremental/Iterative model):** Đặc tả, thiết kế, lập trình, kiểm thử được thực hiện qua nhiều vòng lặp. Có thể theo lịch trình hoặc linh hoạt.
  * **Mô hình Tích hợp và Tái sử dụng (Integration and Reuse model):** Hệ thống được lắp ráp từ các thành phần có sẵn.
  * *Thực tế:* Các hệ thống lớn thường kết hợp cả 3 mô hình trên.

### 5. Đạo đức trong Công nghệ Phần mềm (Ethics in SE)
* Hành vi đạo đức không chỉ là tuân thủ pháp luật mà còn phải tuân theo các nguyên tắc đạo đức nghề nghiệp.
* **Các vấn đề trách nhiệm nghề nghiệp:**
  * *Quyền sở hữu trí tuệ (Intellectual property rights):* Tuân thủ luật bản quyền, bằng sáng chế; bảo vệ tài sản trí tuệ của công ty và khách hàng.
  * *Lạm dụng máy tính (Computer misuse):* Không dùng kỹ năng kỹ thuật để lạm dụng máy tính người khác (từ việc nhỏ như chơi game trên máy công ty đến việc nghiêm trọng như phát tán virus).
* **Bộ quy tắc đạo đức ACM/IEEE (8 nguyên tắc):**
  1. **PUBLIC (Cộng đồng):** Hành động nhất quán với lợi ích cộng đồng.
  2. **CLIENT AND EMPLOYER (Khách hàng & Nhà tuyển dụng)**
  3. **PRODUCT (Sản phẩm)**
  4. **JUDGMENT (Phán đoán)**
  5. **MANAGEMENT (Quản lý)**
  6. **PROFESSION (Nghề nghiệp):** Nâng cao tính chính trực và uy tín của nghề phù hợp với lợi ích cộng đồng.
  7. **COLLEAGUES (Đồng nghiệp):** Công bằng và hỗ trợ đồng nghiệp.
  8. **SELF (Bản thân):** Học tập suốt đời về thực hành nghề nghiệp và thúc đẩy cách tiếp cận có đạo đức.

---

## CHƯƠNG 2: PHÂN TÍCH VÀ ĐẶC TẢ YÊU CẦU (Requirements Analysis & Specification)

### 1. Khái niệm về Yêu cầu và Các bên liên quan
* **Yêu cầu (Requirement):** Dao động từ một tuyên bố trừu tượng mức cao về dịch vụ/ràng buộc hệ thống cho đến một đặc tả chức năng toán học chi tiết.
  * *Chức năng kép:* Dùng để các nhà thầu đấu thầu hợp đồng, đồng thời dùng làm định nghĩa hệ thống chi tiết giữa nhà thầu và khách hàng.
* **Phân tích yêu cầu (Requirements Analysis):** Là bước kỹ thuật đầu tiên, nơi các bên liên quan hợp tác để tìm ra chi tiết những gì cần làm.
* **Các bên liên quan (Stakeholders):** Bất kỳ cá nhân hoặc tổ chức nào bị ảnh hưởng bởi hệ thống và có lợi ích hợp pháp (người dùng cuối, người quản trị hệ thống, chủ sở hữu hệ thống, người quản lý nghiệp vụ...).

### 2. Phân loại Yêu cầu
* **Yêu cầu chức năng (Functional requirements):** Mô tả các dịch vụ hệ thống phải cung cấp, cách hệ thống phản ứng với đầu vào cụ thể và hành vi trong các tình huống cụ thể.
  * *Lưu ý:* Sự mơ hồ (imprecision) trong yêu cầu chức năng dễ dẫn đến hiểu nhầm giữa người dùng và lập trình viên. Yêu cầu cần đảm bảo tính **đầy đủ (completeness)** và **nhất quán (consistency)**.
* **Yêu cầu phi chức năng (Non-functional requirements):** Thường đặt ra ràng buộc cho hệ thống đang phát triển hoặc quy trình phát triển (ví dụ: hiệu năng, độ tin cậy, tính khả chuyển - portability, bảo mật...). Thường liên quan đến các thuộc tính tổng thể (emergent properties) của toàn hệ thống.

### 3. Quy trình Kỹ thuật Yêu cầu (Requirements Engineering - RE)
Là một quy trình lặp (dạng xoắn ốc - spiral) gồm 4 hoạt động chung:

#### a. Khai phá và Phân tích yêu cầu (Requirements Elicitation & Analysis)
Gồm 4 bước lặp:
1. *Khám phá yêu cầu (Discovery):* Tương tác với stakeholders để tìm hiểu nghiệp vụ và yêu cầu miền ứng dụng.
2. *Phân loại và tổ chức (Classification & organization):* Nhóm các yêu cầu liên quan thành các cụm mạch lạc.
3. *Sắp xếp ưu tiên và thương lượng (Prioritization & negotiation):* Xác định độ ưu tiên và giải quyết xung đột yêu cầu.
4. *Tài liệu hóa (Documentation):* Ghi nhận yêu cầu làm đầu vào cho vòng lặp tiếp theo.

* **Các kỹ thuật khai phá:**
  * **Phỏng vấn (Interviewing):** Phỏng vấn đóng (câu hỏi chuẩn bị sẵn) và phỏng vấn mở (thảo luận tự do các vấn đề).
  * **Quan sát / Dân tộc học (Observation / Ethnography):** Quan sát người dùng làm việc thực tế để thấy công cụ và quy trình họ sử dụng.
  * **Kịch bản (Scenarios) & Ca sử dụng (Use cases) / User stories:** Mô tả trình tự tương tác, điều kiện tiên quyết (preconditions), hậu điều kiện (postconditions), luồng chính và các ngoại lệ (extensions) khi có lỗi xảy ra.

#### b. Đặc tả yêu cầu (Requirements Specification)
* **Yêu cầu người dùng (User requirements):** Viết dễ hiểu cho khách hàng/người dùng cuối không có nền tảng kỹ thuật.
* **Yêu cầu hệ thống (System requirements):** Chi tiết hơn, chứa nhiều thông tin kỹ thuật hơn cho đội ngũ phát triển.
* **Các phương pháp viết đặc tả:**
  * *Ngôn ngữ tự nhiên (Natural language):* Biểu đạt tốt, trực quan, phổ biến nhưng dễ gây mơ hồ (thường kết hợp biểu đồ, bảng).
  * *Ngôn ngữ mô tả thiết kế (Design description languages):* Giống ngôn ngữ lập trình mức trừu tượng (ít dùng, chủ yếu cho đặc tả giao diện).
  * *Ký hiệu đồ họa (Graphical notations):* Mô hình đồ họa kèm chú thích văn bản (UML Use Case, Sequence diagram...).
  * *Đặc tả toán học (Mathematical specifications):* Dựa trên máy trạng thái hữu hạn, tập hợp; không mơ hồ nhưng khách hàng rất khó hiểu.
* **Yêu cầu và Thiết kế:** Về nguyên tắc, Yêu cầu nói **WHAT** (làm gì), Thiết kế nói **HOW** (làm như thế nào). Trên thực tế, hai phần này khó tách rời hoàn toàn.
* **Tài liệu đặc tả yêu cầu phần mềm (SRS - Software Requirements Specification):** Tuyên bố chính thức về những gì yêu cầu đối với nhà phát triển. Cấu trúc chuẩn (như chuẩn IEEE) thường gồm: *Lời nói đầu (Preface), Giới thiệu (Introduction), Bảng thuật ngữ (Glossary), Định nghĩa yêu cầu người dùng, Kiến trúc hệ thống, Đặc tả yêu cầu hệ thống, Mô hình hệ thống, Tiến hóa hệ thống, Phụ lục (Appendices), Chỉ mục (Index).*

#### c. Thẩm định yêu cầu (Requirements Validation)
* Chứng minh rằng các yêu cầu mô tả đúng hệ thống khách hàng thực sự muốn.
* **Tầm quan trọng:** Chi phí sửa lỗi yêu cầu sau khi bàn giao có thể **cao gấp 100 lần** so với sửa lỗi cài đặt.
* **5 tiêu chí kiểm tra:**
  1. **Tính hợp lệ (Validity):** Hệ thống có cung cấp các chức năng hỗ trợ tốt nhất nhu cầu khách hàng không?
  2. **Tính nhất quán (Consistency):** Có xung đột giữa các yêu cầu không?
  3. **Tính đầy đủ (Completeness):** Đã bao gồm mọi chức năng khách hàng cần chưa?
  4. **Tính khả thi/thực tế (Realism):** Có thể thực hiện với ngân sách và công nghệ hiện có không?
  5. **Tính có thể kiểm chứng (Verifiability):** Có thể kiểm tra/kiểm thử yêu cầu đó không?
* **Kỹ thuật thẩm định:** Đánh giá yêu cầu (Requirements reviews), Làm mẫu thử (Prototyping), Sinh ca kiểm thử (Test-case generation).

#### d. Quản lý yêu cầu (Requirements Management)
* **Nguyên nhân thay đổi yêu cầu:** Môi trường kinh doanh/kỹ thuật luôn thay đổi; người trả tiền (khách hàng) và người sử dụng thường không phải cùng một đối tượng; hệ thống lớn có nhiều nhóm người dùng với ưu tiên xung đột nhau.
* **Nhiệm vụ:** Theo dõi từng yêu cầu, duy trì liên kết giữa các yêu cầu phụ thuộc để đánh giá tác động khi có thay đổi, và thiết lập quy trình chính thức cho đề xuất thay đổi.

### 4. Mô hình hóa hệ thống (System Modeling)
Sử dụng các ký hiệu đồ họa để xây dựng các mô hình trừu tượng thể hiện nhiều góc nhìn khác nhau của hệ thống:
* **FDD (Functional Decomposition Diagram):** Sơ đồ phân rã chức năng.
* **DFD (Data Flow Diagram):** Sơ đồ luồng dữ liệu.
* **ERD (Entity-Relationship Diagram):** Sơ đồ thực thể - liên kết.
* **Data Dictionaries:** Từ điển dữ liệu.
* **UML (Unified Modeling Language):** Biểu đồ Use-case, Class, Sequence...

---

## CHƯƠNG 3: THIẾT KẾ PHẦN MỀM (Software Design)

### 1. Tổng quan và các hoạt động thiết kế
4 hoạt động thiết kế chính:
1. **Thiết kế kiến trúc (Architectural Design):** Xác định cấu trúc tổng thể của hệ thống, các thành phần chính (subsystems/modules) và mối quan hệ giữa chúng.
2. **Thiết kế cơ sở dữ liệu (Database Design):** Thiết kế cấu trúc dữ liệu và cách biểu diễn trong CSDL.
3. **Thiết kế giao diện (Interface Design):** Xác định giao diện giữa các thành phần hệ thống và giao diện người dùng.
4. **Lựa chọn và thiết kế thành phần (Component Selection & Design):** Tìm kiếm thành phần tái sử dụng; nếu không có sẵn thì tiến hành thiết kế mới.

### 2. Các nguyên tắc thiết kế
* Không giới hạn trong một góc nhìn hẹp $\rightarrow$ cân nhắc nhiều giải pháp khác nhau.
* Không làm lại những giải pháp đã có $\rightarrow$ tái sử dụng thiết kế tối đa.
* Biểu diễn thiết kế phải nhất quán và tích hợp cao để nhiều người cùng làm việc và giao tiếp được.
* Cấu trúc phải linh hoạt, dễ thay đổi $\rightarrow$ cần **module hóa (modularization)**.
* Đánh giá thiết kế ngay trong quá trình tạo lập (thông qua **Cohesion** và **Coupling**).
* Thẩm định và kiểm tra lại để tránh lỗi hệ thống (thiếu chức năng, mâu thuẫn với SRS).

### 3. Module hóa (Modularization) và Thước đo chất lượng thiết kế
* **Nguyên tắc Chia để trị (Divide and Conquer):**
  * Độ phức tạp: $C(p_1 + p_2) > C(p_1) + C(p_2)$
  * Công sức thực hiện: $E(p_1 + p_2) > E(p_1) + E(p_2)$
* **Lợi ích:** Giảm độ phức tạp, dễ nâng cấp/sửa đổi, hỗ trợ phát triển song song, tăng khả năng tái sử dụng.
* **Số lượng module tối ưu:** Cần cân bằng giữa *chi phí phát triển module* (giảm khi tăng số module) và *chi phí tích hợp module* (tăng khi tăng số module), dựa trên khái niệm **độc lập chức năng (functional independence)**.
* **2 thước đo chất lượng thiết kế:**
  * **Coupling (Độ ghép nối / phụ thuộc):** Mức độ phụ thuộc lẫn nhau giữa các module.
    * Mức độ từ Tệ (Tight - chặt) $\rightarrow$ Tốt (Loose - lỏng): `Content` $\rightarrow$ `Common` $\rightarrow$ `Control` $\rightarrow$ `Stamp` $\rightarrow$ `Data`.
    * *Mục tiêu:* **Low/Loose Coupling (Phụ thuộc lỏng)**.
  * **Cohesion (Độ kết dính):** Mức độ liên quan mật thiết giữa các thành phần bên trong cùng một module.
    * *Mục tiêu:* **High Cohesion (Kết dính cao)**.

### 4. Thiết kế kiến trúc (Architectural Design)
* Là giai đoạn sớm nhất trong thiết kế. Việc tái cấu trúc kiến trúc (refactoring) về sau cực kỳ tốn kém vì ảnh hưởng đến rất nhiều thành phần.
* **Mẫu kiến trúc (Architectural Pattern):** Là các thực hành thiết kế tốt đã được kiểm chứng qua nhiều môi trường, giúp biểu diễn, chia sẻ và tái sử dụng tri thức.
* **Mô hình MVC (Model - View - Controller):** Tách biệt việc hiển thị và tương tác dữ liệu thành 3 thành phần logic:
  * **Model:** Quản lý dữ liệu hệ thống và các thao tác gắn liền với dữ liệu đó (CRUD với Database).
  * **View:** Định nghĩa và quản lý cách dữ liệu được trình bày/hiển thị cho người dùng.
  * **Controller:** Quản lý tương tác của người dùng, điều phối thao tác với Model và gửi dữ liệu sang View.

### 5. Thiết kế giao diện người dùng (User Interface Design)
* **Các nguyên tắc vàng:**
  * *Đặt người dùng vào vị trí kiểm soát:* Tối ưu tương tác theo trình độ người dùng, ẩn đi các chi tiết kỹ thuật bên trong, hỗ trợ tương tác trực tiếp với đối tượng trên màn hình.
  * *Giảm tải trí nhớ cho người dùng (Reduce Memory Load):* Giảm yêu cầu nhớ ngắn hạn, thiết lập giá trị mặc định hợp lý, tạo phím tắt trực quan, dùng phép ẩn dụ đời thực (real world metaphor), hiển thị thông tin theo từng cấp độ (progressive disclosure).
  * *Đảm bảo tính nhất quán (Consistency):* Đặt tác vụ hiện tại trong ngữ cảnh rõ ràng, giữ nhất quán trên toàn bộ họ ứng dụng, không thay đổi các mô hình tương tác quen thuộc nếu không có lý do thuyết phục.
* **4 Mô hình thiết kế giao diện:**
  1. *User model:* Hồ sơ về tất cả người dùng cuối của hệ thống.
  2. *Design model:* Hiện thực hóa thiết kế của user model.
  3. *Mental model (System perception):* Hình dung trong tâm trí người dùng về giao diện đó.
  4. *Implementation model:* Giao diện thực tế ("look and feel") khi cài đặt.
* **Phân tích và Mô hình hóa tác vụ (Task Analysis & Modeling):** Gồm Use-cases (tương tác cơ bản), Task elaboration (làm mịn tác vụ), Object elaboration (xác định đối tượng giao diện), Workflow analysis (luồng công việc nhiều vai trò).
* **4 bước thiết kế giao diện:**
  1. Xác định đối tượng và hành động giao diện.
  2. Xác định các sự kiện (hành động người dùng) làm thay đổi trạng thái giao diện và mô hình hóa hành vi này.
  3. Mô tả từng trạng thái giao diện đúng như cách người dùng cuối sẽ nhìn thấy.
  4. Chỉ ra cách người dùng diễn giải trạng thái hệ thống từ thông tin giao diện cung cấp.
* **Các vấn đề thiết kế cần lưu ý:** Thời gian phản hồi (Response time), Hệ thống trợ giúp (Help facilities), Xử lý lỗi (Error handling), Nhãn menu và lệnh (Menu/command labeling).
* **Nguyên tắc thiết kế giao diện Web/Mobile App:** Tự chủ có kiểm soát (*Controlled autonomy*), Hiệu quả cho người dùng (*Efficiency*), Tập trung (*Focus*), Bảo toàn dữ liệu công việc đang làm (*Maintain work product integrity* - tự động lưu), Dễ đọc (*Readability*), Lưu vết trạng thái (*Track state*).

---

## CHƯƠNG 4: CÀI ĐẶT / LẬP TRÌNH (Implementation / Programming)

### 1. Thiết kế, Cài đặt và Quyết định "Xây dựng hay Mua"
* Thiết kế và cài đặt luôn đan xen lẫn nhau. Cài đặt là quá trình hiện thực hóa thiết kế thành chương trình thực thi được.
* **Build or Buy (Xây dựng hay Mua):** Có thể mua các hệ thống thương mại có sẵn (**COTS** - *Commercial Off-The-Shelf*) rồi cấu hình, điều chỉnh cho phù hợp yêu cầu để tiết kiệm thời gian và chi phí.
* **Mục tiêu của cài đặt:**
  * Chuyển đổi thiết kế sang ngôn ngữ máy tính.
  * Giảm chi phí cho các giai đoạn sau (kiểm thử và bảo trì).
  * Làm cho chương trình dễ đọc (readable) $\rightarrow$ dễ bảo trì hơn.

### 2. Các phương pháp lập trình & Độ phức tạp thuật toán
* **4 phương pháp lập trình (Programming Methodologies):**
  1. *Lập trình thủ tục (Procedural):* Chia bài toán thành các thủ tục/hàm (VD: cộng, trừ, nhân, chia).
  2. *Lập trình hướng đối tượng (Object-oriented):* Giải pháp xoay quanh các thực thể/đối tượng của bài toán (VD: Nhân viên, Bảng lương).
  3. *Lập trình hàm (Functional):* Chia giải pháp thành các đơn vị chức năng độc lập.
  4. *Lập trình logic (Logical):* Dựa trên các luật và suy diễn logic.
* **Độ phức tạp thuật toán (Algorithm Complexity):** Đánh giá hiệu quả qua 2 yếu tố:
  * **Thời gian (Time):** Thời gian thực thi thuật toán (phép toán, rẽ nhánh, vòng lặp).
  * **Không gian (Space):** Dung lượng bộ nhớ cần để lưu biến (đầu vào, biến tạm, đầu ra).

### 3. Các vấn đề quan trọng trong Cài đặt (Implementation Issues)

#### a. Tái sử dụng phần mềm (Reuse)
* **4 cấp độ tái sử dụng:**
  1. *Cấp độ trừu tượng (Abstraction level):* Không dùng lại mã nguồn trực tiếp mà dùng lại kiến thức/mẫu thiết kế thành công.
  2. *Cấp độ đối tượng (Object level):* Tái sử dụng trực tiếp các đối tượng/hàm từ thư viện.
  3. *Cấp độ thành phần (Component level):* Tái sử dụng tập hợp các lớp/đối tượng (VD: Frameworks).
  4. *Cấp độ hệ thống (System level):* Tái sử dụng toàn bộ hệ thống ứng dụng.
* **Chi phí tái sử dụng:** Chi phí tìm kiếm & đánh giá; chi phí mua bản quyền; chi phí điều chỉnh & cấu hình; chi phí tích hợp với mã nguồn mới.

#### b. Quản lý cấu hình (Configuration Management - CM)
* Quá trình quản lý sự thay đổi của một hệ thống phần mềm đang tiến hóa, giúp nhiều lập trình viên truy cập mã nguồn/tài liệu có kiểm soát (công cụ phổ biến: **Git**).
* **3 hoạt động chính:**
  1. *Quản lý phiên bản (Version management):* Theo dõi các phiên bản khác nhau của thành phần phần mềm và điều phối làm việc nhóm.
  2. *Tích hợp hệ thống (System integration):* Xác định phiên bản nào của các thành phần được dùng để biên dịch, liên kết và đóng gói (build) thành một phiên bản hệ thống.
  3. *Theo dõi sự cố (Problem tracking):* Cho phép báo cáo lỗi (bugs), theo dõi ai đang xử lý và khi nào hoàn tất.

#### c. Phát triển Máy chủ - Máy đích (Host-Target Development)
* Phần mềm thường được phát triển trên một máy tính (**Host** - môi trường phát triển, sử dụng IDE) nhưng lại được triển khai và chạy trên một máy tính/nền tảng khác (**Target** - môi trường đích).

### 4. Phát triển Mã nguồn mở (Open-Source Development)
* Là cách tiếp cận công khai mã nguồn hệ thống và mời cộng đồng/tình nguyện viên cùng tham gia phát triển.
* **Quản lý giấy phép (License Management):** Cần thiết lập hệ thống theo dõi các thành phần mã nguồn mở đang dùng, hiểu rõ từng loại giấy phép trước khi sử dụng, nắm được lộ trình phát triển của thành phần đó, đào tạo nhân sự, có hệ thống kiểm toán (auditing) và tham gia vào cộng đồng mã nguồn mở.

---

## CHƯƠNG 5: KIỂM THỬ PHẦN MỀM (Software Testing)

### 1. Tổng quan về Kiểm thử
* **Mục đích:** Thực thi chương trình với dữ liệu nhân tạo để kiểm tra xem phần mềm có làm đúng chức năng mong muốn không và phát hiện lỗi (defects) trước khi đưa vào sử dụng.
* **Nguyên lý cốt lõi:** Kiểm thử chỉ có thể **chứng minh sự tồn tại của lỗi, KHÔNG thể chứng minh sự vắng mặt của lỗi**.
* **2 mục tiêu kiểm thử:**
  * *Validation testing (Kiểm thử thẩm định):* Chứng minh phần mềm đáp ứng các yêu cầu đặt ra.
  * *Defect testing (Kiểm thử tìm lỗi):* Tìm ra các tình huống phần mềm hoạt động sai, không mong muốn (crash, sai tính toán, hỏng dữ liệu...).

### 2. Xác minh (Verification) vs. Thẩm định (Validation) & Thanh tra (Inspections)
* **Verification (Xác minh):** *"Are we building the product right?"* (Chúng ta có đang xây dựng sản phẩm đúng cách không?) $\rightarrow$ Kiểm tra sự tuân thủ đặc tả.
* **Validation (Thẩm định):** *"Are we building the right product?"* (Chúng ta có đang xây dựng đúng sản phẩm không?) $\rightarrow$ Kiểm tra xem phần mềm có đáp ứng đúng nhu cầu thực tế của người dùng không.
* **Mô hình chữ V (V-Model):** Liên kết trực tiếp các giai đoạn phát triển (Verification) với các cấp độ kiểm thử tương ứng (Validation):
  * Business Req. Spec $\leftrightarrow$ Acceptance Testing
  * System Req. Spec $\leftrightarrow$ System Integration Testing
  * High Level Design $\leftrightarrow$ Component Testing
  * Low Level Design $\leftrightarrow$ Unit Testing
* **Thanh tra phần mềm (Software Inspections - Kiểm thử tĩnh) vs. Kiểm thử động (Testing):**
  * *Inspections:* Con người trực tiếp đọc/kiểm tra tài liệu, mã nguồn mà **không cần chạy chương trình**. Có thể áp dụng ở mọi giai đoạn; một lỗi không che khuất (mask) lỗi khác.
  * *Hạn chế của Inspections:* Không kiểm tra được sự phù hợp với nhu cầu thực sự của khách hàng và không kiểm tra được các thuộc tính phi chức năng (hiệu năng, tính khả dụng).
  * $\rightarrow$ Inspections và Testing **bổ trợ cho nhau** trong quá trình V&V.

### 3. Ba giai đoạn kiểm thử chính

#### Giai đoạn 1: Kiểm thử phát triển (Development Testing)
Do chính đội ngũ phát triển thực hiện nhằm tìm lỗi (bugs/defects), gồm 3 cấp độ:
1. **Kiểm thử đơn vị (Unit Testing):**
   * Kiểm thử từng hàm, phương thức hoặc lớp đối tượng một cách cô lập (và các chuỗi chuyển đổi trạng thái).
   * Nên sử dụng **Kiểm thử tự động (Automated testing)** gồm 3 phần: *Setup* (dữ liệu đầu vào & kết quả kỳ vọng) $\rightarrow$ *Call* (gọi hàm cần test) $\rightarrow$ *Assertion* (so sánh kết quả thực tế với kỳ vọng).
   * **2 loại Test case:** Hoạt động bình thường (normal operation) và Đầu vào bất thường (abnormal inputs).
   * **Chiến lược chọn Test case:**
     * *Partition testing (Phân hoạch tương đương - Equivalence partitioning):* Chia dữ liệu đầu vào thành các lớp/miền có hành vi xử lý tương đương nhau, chọn đại diện từ mỗi miền.
     * *Guideline-based testing (Dựa trên hướng dẫn/kinh nghiệm):* Với chuỗi/mảng (test rỗng, 1 phần tử, nhiều kích thước, phần tử đầu/giữa/cuối); nói chung (ép sinh mọi thông báo lỗi, tràn bộ đệm, lặp đầu vào nhiều lần, giá trị quá lớn/quá nhỏ).
2. **Kiểm thử thành phần (Component Testing):**
   * Tích hợp nhiều đơn vị thành thành phần phức hợp (composite components).
   * Trọng tâm: Kiểm thử **giao diện thành phần (component interfaces)**.
3. **Kiểm thử hệ thống (System Testing):**
   * Tích hợp các thành phần (cả mới viết và tái sử dụng/COTS) thành hệ thống hoàn chỉnh.
   * Trọng tâm: Kiểm thử **sự tương tác giữa các thành phần** và hành vi tổng thể (emergent behaviour). Thường dùng **Use-case testing** (dựa trên biểu đồ tuần tự - sequence diagram).
* **Kiểm thử hồi quy (Regression Testing):** Chạy lại toàn bộ các bài test sau mỗi lần thay đổi mã nguồn để đảm bảo sửa đổi mới không làm hỏng các tính năng vốn đang chạy tốt.

#### Giai đoạn 2: Kiểm thử phát hành (Release Testing)
* Do một **đội ngũ kiểm thử độc lập** (không tham gia phát triển) thực hiện trên một phiên bản hoàn chỉnh trước khi giao cho người dùng.
* Thường là **kiểm thử hộp đen (Black-box testing)**, dựa hoàn toàn vào đặc tả hệ thống.
* Khác với System testing (tập trung tìm bug - defect testing), Release testing tập trung vào **Validation testing** (chứng minh hệ thống đủ tốt để sử dụng bên ngoài).
* **Các hình thức:**
  * *Requirements-based testing:* Xây dựng test case cho từng yêu cầu.
  * *Scenario testing:* Dựa trên các kịch bản sử dụng điển hình.
  * *Performance testing:* Tăng dần tải trọng (load) để kiểm tra hiệu năng và độ tin cậy.
  * *Stress testing:* Một dạng của performance testing, cố tình làm quá tải hệ thống để kiểm tra cách hệ thống xử lý khi sập/quá tải.

#### Giai đoạn 3: Kiểm thử người dùng (User Testing)
* Rất cần thiết vì môi trường làm việc thực tế của người dùng có nhiều yếu tố ảnh hưởng đến độ tin cậy, hiệu năng, tính khả dụng mà môi trường test không thể mô phỏng hết.
* **3 loại hình:**
  1. **Alpha testing:** Người dùng làm việc cùng đội phát triển để kiểm thử ngay tại địa điểm của nhà phát triển.
  2. **Beta testing:** Phát hành phiên bản dùng thử cho người dùng tự trải nghiệm và báo lỗi về cho nhà phát triển.
  3. **Acceptance testing (Kiểm thử chấp nhận):** Khách hàng tự kiểm thử (chủ yếu cho phần mềm đặt hàng - custom systems) để quyết định chấp nhận hay từ chối đưa vào vận hành.
     * *6 bước:* Xác định tiêu chí chấp nhận $\rightarrow$ Lập kế hoạch $\rightarrow$ Xây dựng bài test $\rightarrow$ Chạy test $\rightarrow$ Thương lượng kết quả $\rightarrow$ Chấp nhận/Từ chối hệ thống.
     * *Trong Agile:* Không có giai đoạn Acceptance testing tách biệt; khách hàng là một phần của nhóm phát triển, tự định nghĩa test và tích hợp chạy tự động liên tục (nhược điểm: người dùng đại diện chưa chắc đã đại diện cho mọi stakeholders).