# Quản lý Thư viện — Project 3 (C++17, Win32 GUI)

Đồ án môn **Cấu trúc dữ liệu & Giải thuật**, mã đề **PRO_CTDL_3**. Đây là ứng dụng Windows có giao diện đồ hoạ, quản lý các hoạt động hằng ngày của một thư viện:

- đăng nhập nhân viên;
- quản lý sách, bạn đọc, phiếu mượn;
- mượn và trả sách;
- thống kê và xuất báo cáo.

Mọi thông tin được lưu dưới dạng file `.txt` (UTF-8, tiếng Việt có dấu).

**Công nghệ:**

- **Hướng đối tượng và template:**
  - `DanhSachLienKet<T>`: danh sách liên kết đơn tự cài đặt, có **merge sort**.
  - `KhoDuLieu<T>`: đọc / ghi file.
  - `XuatCsv<T>`, `ChuyenSangSo<T>`, `TachChuoi<N>`: các hàm template.
  - Hai cây kế thừa có hàm ảo: `DoiTuongLuuTru` và `CuaSoCoSo` (xem `src/GIAI_THICH.md`).
- **Không dùng thư viện ngoài.** Chỉ dùng STL và Windows API có sẵn: user32 / gdi32 (cửa sổ, vẽ), comctl32 (bảng, ô chọn ngày, TaskDialog), msimg32, comdlg32, uxtheme, shell32.
- **Một file exe duy nhất.** Icon, manifest (giao diện hiện đại, nét trên màn hình DPI cao) và dữ liệu mẫu đều được nhúng vào exe. Lần đầu chạy, exe tự tạo thư mục `data\` bên cạnh nó.

---

## 1. Cách sử dụng

Tài khoản mẫu (trong `data/Admin.txt`):

| User | Password |
|---|---|
| `user1` | `12345678` |
| `admin` | `admin123` |

| Màn hình | Chức năng |
|---|---|
| **Đăng nhập** | Mật khẩu hiện dấu `*`. Sai thì báo lỗi, cửa sổ rung và cho nhập lại; **sai 3 lần thì thoát**. **ESC** hoặc nút ✕ để thoát. |
| **Tổng quan** | 5 thẻ thống kê, biểu đồ cột lượt mượn 6 tháng, biểu đồ vòng tình trạng kho, bảng phiếu quá hạn (nhấp đúp để trả sách). |
| **Quản lý sách** | Hiển thị đủ 9 thông tin của sách. **Thêm sách**, **Xóa sách** (chọn dòng hoặc nhập mã), **Cho mượn**. Tìm kiếm tức thì, không phân biệt dấu. Xuất CSV. |
| **Quản lý phiếu mượn** | Hiển thị phiếu mượn; lọc Đang mượn / Quá hạn / Đã trả. **Mượn sách** (nhập mã sách và mã bạn đọc). **Trả sách** (nhập số phiếu, xem trước thông tin phiếu). Xuất CSV. |
| **Bạn đọc** | Danh sách, số sách đang mượn và tổng lượt mượn của từng người. Thêm / xóa bạn đọc. Xuất CSV. |

**Mẹo thao tác:**

- Bấm tiêu đề cột để **sắp xếp** tăng / giảm.
- Phím tắt: `Ctrl+1..4` chuyển trang, `Ctrl+F` tìm kiếm, `F5` làm mới, `Delete` xóa dòng đang chọn.
- Trong hộp thoại: `Enter` để đồng ý, `ESC` để hủy.
- Khi có lỗi (nhập sai, vi phạm quy định, lỗi ghi file), chương trình **báo lỗi, hủy chức năng** rồi quay lại để người dùng chọn chức năng khác.
- Đóng cửa sổ chính thì chương trình hỏi xác nhận thoát; nút **Đăng xuất** đưa về màn hình đăng nhập.

---

## 2. Cài đặt trình biên dịch

Mở terminal và gõ `g++ --version`. Nếu báo `'g++' is not recognized`, cài WinLibs bằng lệnh dưới đây. Lệnh này cài cả `g++`, `mingw32-make` và `windres`, đồng thời tự thêm vào PATH.

```
winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e
```

> Sau khi cài đặt, **phải thoát hẳn VS Code (File → Exit) rồi mở lại**. Nếu không, VS Code vẫn dùng PATH cũ.

---

## 3. Biên dịch, chạy, đóng gói

**Trong VS Code:**

- **Ctrl+Shift+B**: build ra `build\QuanLyThuVien.exe`.
- **F5**: build rồi chạy kèm trình gỡ lỗi gdb.
- *Terminal → Run Task*:
  - `Run: QuanLyThuVien`: mở chương trình.
  - `Test: KiemThu`: chạy kiểm thử tự động.
  - `Phát hành: 1 file exe để nộp`: tạo bản nộp.

**Bằng dòng lệnh** (đứng ở thư mục gốc dự án):

```bat
mingw32-make -j16              :: build/QuanLyThuVien.exe (bản gỡ lỗi)
mingw32-make -j16 test         :: kiểm thử tự động (74 ca)
mingw32-make phathanh          :: release/QuanLyThuVien.exe (khoảng 1,7 MB)
mingw32-make clean             :: xóa build/
```

**Gửi hoặc nộp bài:** chỉ cần file **`release\QuanLyThuVien.exe`**.

- Máy chạy Windows 10 hoặc 11 bản 64-bit là chạy được, không phải cài thêm gì. Exe chỉ dùng các DLL có sẵn của Windows.
- Lần đầu chạy, exe tự tạo thư mục `data\` bên cạnh nó với dữ liệu mẫu.
- Nếu Windows hiện bảng *"Windows protected your PC"*, bấm *More info* rồi *Run anyway*. Bảng này hiện vì exe tự biên dịch, chưa có chữ ký số.

---

## 4. Cấu trúc dự án

```
CTDL&GT/
├── src/
│   ├── main.cpp, ChuongTrinh.h/.cpp Điểm bắt đầu (wWinMain) và điều phối chương trình
│   ├── cautrucdulieu/               DanhSachLienKet.h                                   (lõi)
│   ├── thucthe/                     DoiTuongLuuTru, Sach, BanDoc, PhieuMuon, TaiKhoan   (lõi)
│   ├── luutru/                      KhoDuLieu.h, XuatCsv.h                              (lõi)
│   ├── nghiepvu/                    QuanLyThuVien                                       (lõi)
│   ├── tienich/                     CauHinh, NgoaiLe, NgayThang, TienIch                (lõi)
│   ├── nentang/                     CuaSoCoSo, ChuDe, BoVe, DieuKhien, BangDuLieu, HopThongBao, TaiNguyenNhung
│   └── giaodien/                    CuaSoDangNhap, CuaSoChinh, Trang..., HopThoai...
├── data/        Admin.txt, Sach.txt, BanDoc.txt, PhieuMuon.txt, icon.ico
├── tests/       KiemThu.cpp                        (kiểm thử phần lõi)
├── release/     Bản exe đóng gói để nộp
├── .vscode/     Cấu hình VS Code
├── Makefile     Công thức biên dịch
├── TaiNguyen.rc Danh sách tài nguyên nhúng vào exe 
└── QuanLyThuVien.manifest
```

Mỗi thư mục có một file `GIAI_THICH.md` mô tả chi tiết.

---

## 5. Ràng buộc dữ liệu

Mọi ràng buộc được kiểm tra trong lớp `QuanLyThuVien` (phần lõi), nên dù thao tác từ màn hình nào cũng bị kiểm tra như nhau. Phần lõi được kiểm thử tự động.

| Dữ liệu | Ràng buộc |
|---|---|
| Mã sách, mã bạn đọc | Duy nhất. Chỉ gồm chữ cái không dấu và chữ số, tối đa 10 ký tự, tự đổi thành CHỮ HOA. |
| Tên sách / tác giả / NXB / họ tên | Không rỗng, không chứa `|` hoặc xuống dòng. Tối đa 50 / 30 / 30 / 40 ký tự. |
| Giá bán | Từ 1 đến 100.000.000 đồng. |
| Năm phát hành | Từ 1900 đến năm hiện tại. |
| Số trang | Từ 1 đến 10.000. |
| Ngày nhập kho | Không sau hôm nay, không trước năm phát hành. |
| Ngày đăng ký | Không sau hôm nay. |
| Tình trạng sách | Sách mới luôn có tình trạng 0. Khi được mượn, tình trạng = số phiếu mượn. Khi trả, tình trạng về 0. |
| Xóa sách | Sách phải tồn tại và có tình trạng bằng 0; nếu không thì báo "sách đang được mượn". |
| Xóa bạn đọc | Bạn đọc không được còn phiếu đang mượn. |
| Mượn sách | Sách phải có tình trạng bằng 0, mã bạn đọc phải tồn tại. Phiếu mới được tạo như sau:<br>• số phiếu = số lớn nhất + 1<br>• ngày mượn = ngày hệ thống<br>• ngày phải trả = ngày mượn + 7<br>• tình trạng phiếu = 1 |
| Trả sách | Phiếu phải tồn tại và có tình trạng khác 0. Khi trả, tình trạng phiếu và tình trạng sách đều về 0. |
| Ghi file | Lỗi ghi file thì khôi phục dữ liệu như trước khi thao tác. |
| Đọc file | Dòng sai định dạng hoặc trùng mã bị bỏ qua, kèm cảnh báo. |

---

## 6. Quy ước viết code (C++ Coding Standard)

Đặt tên:

| Loại | Quy ước | Ví dụ |
|---|---|---|
| Lớp, enum, hàm thành viên | `PascalCase` | `QuanLyThuVien`, `ThemSach`, `GetMaSach` |
| Biến, thuộc tính | `camelCase` | `maSach`, `dsPhieuMuon` |
| Hằng số, giá trị enum | `CHU_HOA_GACH_DUOI` | `SO_NGAY_MUON` |
| Tên file | Trùng tên lớp | `Sach.h` / `Sach.cpp` |

Cấu trúc và cách viết:

- Mỗi lớp nằm trong một cặp `.h`/`.cpp`, có header guard và chú thích đầu file kèm phần `[GIẢI THÍCH DỄ HIỂU]`.
- Thuộc tính để `private`, truy cập qua `Get...` / `Set...`. Dùng `const` cho hàm chỉ đọc, `override` cho hàm ghi đè.
- Không dùng `using namespace std`. Không có biến toàn cục. Không dùng số "cứng" trong code (gom về `CauHinh.h`).
- `DanhSachLienKet` tuân thủ quy tắc ba (rule of three).
- Tài nguyên Windows (bút, cọ, bộ đệm vẽ) được trả lại ngay sau khi dùng; `BoDemVe` dùng kỹ thuật RAII.
