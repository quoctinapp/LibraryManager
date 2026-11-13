# Thư mục `src/` — toàn bộ mã nguồn C++

Mã nguồn được chia theo **vai trò**: mỗi thư mục con là một nhóm việc.

Năm thư mục **lõi** không dính gì tới giao diện. Chương trình kiểm thử (`tests/`) chỉ dùng năm thư mục này:

| Thư mục | Chứa gì | Ví dụ đời thường |
|---|---|---|
| `cautrucdulieu/` | `DanhSachLienKet<T>`: **danh sách liên kết đơn dạng template**, có sắp xếp trộn (merge sort) | Đoàn tàu chở dữ liệu |
| `thucthe/` | `DoiTuongLuuTru` (lớp trừu tượng) và 4 lớp con: `Sach`, `BanDoc`, `PhieuMuon`, `TaiKhoan` | Thẻ sách, thẻ bạn đọc, biên nhận |
| `luutru/` | `KhoDuLieu<T>` đọc / ghi file `.txt`; `XuatCsv<T>` xuất báo cáo cho Excel | Thủ kho |
| `nghiepvu/` | `QuanLyThuVien`: mọi quy định và ràng buộc dữ liệu (thêm / xóa sách, bạn đọc, mượn / trả, thống kê) | Thủ thư |
| `tienich/` | `CauHinh` (hằng số), `NgoaiLe` (các lớp lỗi), `NgayThang` (ngày tháng), `TienIch` (chuỗi, tiếng Việt) | Hộp dụng cụ |

Các thư mục **giao diện** (Win32 API, chỉ dùng thư viện có sẵn của Windows):

| Thư mục | Chứa gì | Ví dụ đời thường |
|---|---|---|
| (gốc `src/`) | `main.cpp` (điểm bắt đầu `wWinMain`) và `ChuongTrinh` (điều phối: dữ liệu → đăng nhập → cửa sổ chính) | Đạo diễn |
| `nentang/` | Phần "nói chuyện với Windows": `CuaSoCoSo` (lớp cửa sổ cơ sở), `ChuDe` (màu, phông, DPI), `BoVe` (vẽ GDI), `DieuKhien` (nút, ô nhập), `BangDuLieu` (bảng ListView), `HopThongBao` (TaskDialog), `TaiNguyenNhung` (giải nén dữ liệu mẫu từ exe) | Ánh sáng, sân khấu |
| `giaodien/` | Các màn hình: `CuaSoDangNhap`, `CuaSoChinh`, 4 trang (`TrangTongQuan`, `TrangSach`, `TrangPhieuMuon`, `TrangBanDoc`), 5 hộp thoại (`HopThoaiThemSach`, `HopThoaiThemBanDoc`, `HopThoaiMuonSach`, `HopThoaiTraSach`, `HopThoaiNhapMa`) | Các cảnh diễn |

Mỗi phần thường có 2 file: `.h` là mục lục (có những gì), `.cpp` là nội dung (làm thế nào). Riêng các lớp template (`DanhSachLienKet`, `KhoDuLieu`, `XuatCsv`) phải viết trọn trong file `.h`, vì trình biên dịch cần thấy toàn bộ code để sinh ra phiên bản cho từng kiểu.

Khi một file viết `#include "Sach.h"`, trình biên dịch tự tìm trong tất cả các thư mục trên (cấu hình trong `Makefile`), nên không cần ghi đường dẫn thư mục.

## Cây kế thừa (tính đa hình)

```
DoiTuongLuuTru (trừu tượng)   <- Sach, BanDoc, PhieuMuon, TaiKhoan
CuaSoCoSo (trừu tượng)
   <- CuaSoDangNhap
   <- CuaSoChinh  (+ GiaoTiepTrang)
   <- TrangNoiDung (trừu tượng)  <- TrangTongQuan, TrangSach, TrangPhieuMuon, TrangBanDoc
   <- HopThoaiCoSo (trừu tượng)  <- HopThoaiThemSach, HopThoaiThemBanDoc,
                                    HopThoaiMuonSach, HopThoaiTraSach, HopThoaiNhapMa
```

## Luồng chạy

```
wWinMain -> ChuongTrinh::Chay()
   ├── TaiNguyenNhung::ChuanBiThuMucDuLieu()   tìm data\ hoặc giải nén dữ liệu mẫu từ exe
   ├── QuanLyThuVien::NapDuLieu()               đọc 4 file vào 4 DanhSachLienKet
   ├── CuaSoDangNhap::Chay()                    sai 3 lần -> thoát; ESC -> thoát
   └── CuaSoChinh::Chay()                       sidebar + 4 trang; Đăng xuất -> quay lại đăng nhập
          └── Trang... -> HopThoai...::ChayModal() -> QuanLyThuVien::ThemSach / MuonSach / TraSach ...
                 lỗi -> báo lỗi (TaskDialog), HỦY chức năng, quay lại màn hình trước
```
