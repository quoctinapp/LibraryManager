# Thư mục `data/` — dữ liệu của thư viện

Đề bài yêu cầu mọi thông tin phải được lưu dưới dạng file. Mỗi file `.txt` dưới đây là một "cuốn sổ", lưu theo chuẩn **UTF-8**, nên có thể ghi tiếng Việt có dấu.

Mỗi dòng trong file là một bản ghi. Các ô trên một dòng ngăn nhau bằng dấu `|`, giống một hàng trong bảng Excel.

| File | Ý nghĩa | Định dạng một dòng |
|---|---|---|
| `Admin.txt` | Tài khoản nhân viên được phép đăng nhập | `user|password` |
| `Sach.txt` | Danh sách sách | `MaSach|TenSach|TacGia|NhaXuatBan|GiaBan|NamPhatHanh|SoTrang|NgayNhapKho|TinhTrang` |
| `BanDoc.txt` | Danh sách bạn đọc | `MaBanDoc|HoTen|NgayDangKy` |
| `PhieuMuon.txt` | Danh sách phiếu mượn | `SoPhieu|MaBanDoc|MaSach|NgayMuon|NgayPhaiTra|TinhTrang` |
| `icon.ico` | Biểu tượng cuốn sách của chương trình (nhiều cỡ từ 16 đến 256 px) | |

Ví dụ một dòng trong `Sach.txt`:

```
S003|Nhập môn lập trình|Nguyễn Thanh Thủy|NXB ĐHQG Hà Nội|80000|2018|280|15/02/2025|9
```

Dòng này nghĩa là: sách mã `S003`, giá 80.000 đ, xuất bản năm 2018, dày 280 trang, nhập kho ngày 15/02/2025. Ô cuối là tình trạng. Ở đây tình trạng bằng 9, tức là sách đang được mượn theo **phiếu mượn số 9**. Sách chưa có ai mượn thì có tình trạng bằng 0.

Ở `PhieuMuon.txt`, tình trạng bằng 1 nghĩa là đang mượn, bằng 0 nghĩa là đã trả.

## Khi nào file được ghi lại

| Thao tác | File bị ghi |
|---|---|
| Thêm sách | Ghi **nối** đúng 1 dòng vào cuối `Sach.txt` |
| Xóa sách | Ghi **đè** lại toàn bộ `Sach.txt` |
| Mượn sách, trả sách | Ghi **đè** lại cả `Sach.txt` và `PhieuMuon.txt` |
| Thêm bạn đọc | Ghi **nối** vào cuối `BanDoc.txt` |
| Xóa bạn đọc | Ghi **đè** lại `BanDoc.txt` |

`Admin.txt` chỉ được đọc; muốn thêm tài khoản thì sửa bằng Notepad.

## Dữ liệu nhúng trong exe

Cả 4 file `.txt` và `icon.ico` được **nhúng vào bên trong** `QuanLyThuVien.exe` khi biên dịch (xem `TaiNguyen.rc` ở thư mục gốc).

Khi chạy, chương trình tìm thư mục dữ liệu theo thứ tự sau (code ở `src/nentang/TaiNguyenNhung.cpp`):

1. `data\` nằm cạnh file exe, nếu đã có `Admin.txt`.
2. `data\` trong thư mục hiện hành. Đây là trường hợp chạy bằng F5 trong VS Code; khi đó chương trình dùng chính thư mục này.
3. Nếu cả hai đều chưa có, chương trình tạo `data\` cạnh exe và **giải nén dữ liệu mẫu** ra đó. Nếu không ghi được ở đó (ví dụ exe nằm trong `Program Files`), nó dùng `%LOCALAPPDATA%\QuanLyThuVien\data\`.

Chương trình chỉ giải nén những file **còn thiếu**, không bao giờ ghi đè dữ liệu đang có.

## Dữ liệu sai

Khi mở chương trình, những dòng sai định dạng (thiếu ô, ngày không có thật, giá là chữ...) và những dòng trùng mã sẽ bị **bỏ qua**, kèm thông báo cảnh báo. Nhờ vậy một dòng hỏng không làm hỏng cả chương trình.

Code đọc và ghi các file này là lớp template `KhoDuLieu<T>` (file `src/luutru/KhoDuLieu.h`).

> Dữ liệu mẫu có phiếu số 9 và số 10 **quá hạn** (tính theo ngày 13/11/2026), để minh họa phần cảnh báo màu đỏ ở trang Tổng quan và trang Phiếu mượn.
