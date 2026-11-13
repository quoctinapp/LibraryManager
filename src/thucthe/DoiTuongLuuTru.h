#ifndef DOITUONGLUUTRU_H
#define DOITUONGLUUTRU_H

// ============================================================================
//  DoiTuongLuuTru.h - Lớp cơ sở TRỪU TƯỢNG cho mọi đối tượng lưu được ra file
//
//      DoiTuongLuuTru (trừu tượng)
//        ├── Sach
//        ├── BanDoc
//        ├── PhieuMuon
//        └── TaiKhoan
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là "bản thiết kế dở dang": nó chỉ quy định rằng đối tượng nào muốn được
//  lưu ra file thì PHẢI biết 3 việc:
//    1) ChuyenThanhDong: tự viết mình thành 1 dòng chữ, ví dụ "BD001|Nguyen Van An|05/01/2025"
//    2) DocTuDong     : tự đọc lại mình từ 1 dòng chữ (dòng sai -> trả về false)
//    3) LayKhoa       : cho biết "mã định danh" để kiểm tra trùng
//  Còn làm THẾ NÀO thì mỗi lớp con tự viết (chữ "= 0" = bắt buộc lớp con viết).
//  Nhờ vậy lớp KhoDuLieu<T> đọc / ghi được mọi loại đối tượng theo cùng một cách.
// ----------------------------------------------------------------------------

#include <string>

class DoiTuongLuuTru {
public:
    virtual ~DoiTuongLuuTru() = default;

    virtual std::string ChuyenThanhDong() const = 0;
    virtual bool DocTuDong(const std::string& dong) = 0;
    virtual std::string LayKhoa() const = 0;
};

#endif
