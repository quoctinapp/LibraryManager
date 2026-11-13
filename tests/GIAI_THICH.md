# Thư mục `tests/` — kiểm thử tự động

| File | Ý nghĩa |
|---|---|
| `KiemThu.cpp` | Chương trình "tự chấm bài" chạy trong console, không phải chương trình chính. Nó chỉ dùng phần **lõi** (không có giao diện). |
| `build/KiemThu.exe` | File chạy được, tạo trong thư mục `build/` ở gốc dự án sau khi biên dịch `KiemThu.cpp`. |

**Cách hoạt động:** chương trình gọi lớp nghiệp vụ `QuanLyThuVien` với dữ liệu biết trước đáp án, rồi so sánh kết quả. Ví dụ: sách S003 đang được mượn thì lệnh xóa S003 phải bị từ chối. Ca đúng in `[DAT]`, ca sai in `[SAI]`, cuối cùng in tổng số ca đạt (hiện tại **74/74**).

Trước khi chạy, dữ liệu trong `data/` được **sao chép** sang `build/kiểm_thử_dữ_liệu/`. Tên thư mục cố ý có dấu, để thử luôn việc mở file trong đường dẫn tiếng Việt. Nhờ sao chép, file thật trong `data/` không bị thay đổi.

Các nhóm kiểm tra:

1. **Ngày tháng:** cộng ngày qua năm nhuận và qua năm mới; nhận diện ngày không có thật; tính thứ trong tuần.
2. **Tiện ích tiếng Việt:** đổi UTF-8 ↔ UTF-16; đếm ký tự có dấu; tìm kiếm không dấu ("nguyen" khớp "Nguyễn"); so sánh theo bảng chữ cái tiếng Việt; bọc ô CSV.
3. **Danh sách liên kết:** xóa đầu / cuối; sao chép sâu; **sắp xếp trộn** cho kết quả đúng thứ tự và **ổn định**; danh sách rỗng hoặc 1 phần tử không gây lỗi.
4. **Nạp dữ liệu và đăng nhập:** đúng / sai, đọc đủ số bản ghi, đọc đúng chữ có dấu.
5. **Quản lý sách:** thêm trùng mã; các ràng buộc dữ liệu (giá, năm, ngày nhập, ký tự `|`, mã có dấu cách); ghi nối file; xóa sách đang mượn; ghi đè file; tìm kiếm.
6. **Quản lý bạn đọc:** thêm trùng mã; họ tên rỗng; ngày ở tương lai; xóa bạn đọc còn giữ sách.
7. **Mượn / trả:**
   - các điều kiện từ chối;
   - số phiếu = số lớn nhất + 1;
   - ngày phải trả = ngày mượn + 7;
   - cập nhật tình trạng sách và phiếu.
8. **Thống kê:** số sách đang mượn; số phiếu quá hạn; biểu đồ 6 tháng (kể cả khi lùi qua năm cũ).
9. **Xuất CSV:** có BOM UTF-8, đúng số dòng, đúng chữ có dấu.
10. **Đọc lại từ file:** dữ liệu được lưu đúng; dòng sai và dòng trùng mã bị bỏ qua.

**Cách chạy trong VS Code:** chọn *Terminal → Run Task → Test: KiemThu*, hoặc gõ `mingw32-make -j16 test`.
