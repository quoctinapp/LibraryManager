#ifndef NGOAILE_H
#define NGOAILE_H

// ============================================================================
//  NgoaiLe.h - Các lớp ngoại lệ (exception) của chương trình
//
//      std::runtime_error
//        └── LoiChuongTrinh            (lớp cơ sở)
//              ├── LoiNhapLieu         dữ liệu nhập sai định dạng / ngoài khoảng
//              ├── LoiNghiepVu         vi phạm quy tắc: trùng mã, sách đang mượn...
//              ├── LoiFile             không đọc / ghi được file
//              └── HuyThaoTac          người dùng nhấn ESC
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Ngoại lệ giống "còi báo động". Khi gặp lỗi ở sâu bên trong (ví dụ nhập giá bán là "abc"), code chỉ cần "thổi còi" bằng lệnh throw. 
//  Còi được nghe thấy ở ChuongTrinh::ThucHienChucNang: ở đó in thông báo lỗi, HỦY chức năng đang làm và quay về menu, đúng như yêu cầu của đề bài.
// ----------------------------------------------------------------------------

#include <stdexcept>
#include <string>

class LoiChuongTrinh : public std::runtime_error {
public:
    explicit LoiChuongTrinh(const std::string& thongBao) : std::runtime_error(thongBao) {}
};

class LoiNhapLieu : public LoiChuongTrinh {
public:
    explicit LoiNhapLieu(const std::string& thongBao) : LoiChuongTrinh(thongBao) {}
};

class LoiNghiepVu : public LoiChuongTrinh {
public:
    explicit LoiNghiepVu(const std::string& thongBao) : LoiChuongTrinh(thongBao) {}
};

class LoiFile : public LoiChuongTrinh {
public:
    explicit LoiFile(const std::string& thongBao) : LoiChuongTrinh(thongBao) {}
};

class HuyThaoTac : public LoiChuongTrinh {
public:
    explicit HuyThaoTac(const std::string& thongBao) : LoiChuongTrinh(thongBao) {}
};

#endif
