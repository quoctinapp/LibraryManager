# Thư mục `.vscode/` — cấu hình cho VS Code

Thư mục có tên bắt đầu bằng dấu chấm thường là thư mục cấu hình. Thư mục này chỉ phục vụ phần mềm soạn code VS Code; chương trình không đọc nó.

| File | Ý nghĩa |
|---|---|
| `tasks.json` | Định nghĩa các "nút bấm". **Build** (Ctrl+Shift+B) dịch mã nguồn thành `build\QuanLyThuVien.exe`. **Run** mở chương trình. **Test** chạy kiểm thử tự động. **Phát hành** tạo `release\QuanLyThuVien.exe`, một file duy nhất để nộp bài. Mọi nút đều gọi `mingw32-make` đọc file `Makefile` ở thư mục gốc. |
| `launch.json` | Cấu hình phím **F5**: build rồi chạy chương trình kèm trình gỡ lỗi gdb. Thư mục làm việc là thư mục gốc dự án, nên chương trình dùng `data/` của dự án. |
| `c_cpp_properties.json` | Cho phần gợi ý code biết các file `.h` nằm trong `src/` và các thư mục con, để không gạch đỏ dòng `#include`. |

Các file này đều gọi trình biên dịch `g++`. Máy phải cài `g++` trước (xem `README.md` mục 2).
