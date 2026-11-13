#ifndef TAINGUYENNHUNG_H
#define TAINGUYENNHUNG_H

// ============================================================================
//  TaiNguyenNhung.h - Tìm / tạo thư mục dữ liệu cho chương trình
//  4 file dữ liệu mẫu (Admin.txt, Sach.txt, BanDoc.txt, PhieuMuon.txt) được
//  NHÚNG vào bên trong file exe lúc biên dịch (xem TaiNguyen.rc). Thứ tự chọn
//  thư mục dữ liệu:
//    1) <thư mục chứa exe>\data\      nếu đã có Admin.txt
//    2) <thư mục hiện hành>\data\     nếu đã có Admin.txt (chạy từ VS Code)
//    3) Tạo <thư mục chứa exe>\data\ và giải nén dữ liệu mẫu từ exe ra.
//       Không ghi được (ví dụ exe nằm trong Program Files) thì dùng
//       %LOCALAPPDATA%\QuanLyThuVien\data\.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Nhờ lớp này, chỉ cần gửi MỘT file exe: lần đầu chạy, exe tự "đẻ" ra thư
//  mục data bên cạnh nó với dữ liệu mẫu. Những lần sau, chương trình đọc và
//  ghi vào chính thư mục data đó (sửa được bằng Notepad nếu muốn).
// ----------------------------------------------------------------------------

#include <filesystem>

class TaiNguyenNhung {
private:
    static std::filesystem::path ThuMucChuaExe();
    static bool CoDuLieu(const std::filesystem::path& thuMuc);
    static bool GiaiNenNeuThieu(const std::filesystem::path& thuMuc);
    static bool GhiTaiNguyenRaFile(const wchar_t* tenTaiNguyen, const std::filesystem::path& dich);

public:
    TaiNguyenNhung() = delete;

    // Trả về thư mục dữ liệu đã sẵn sàng; ném LoiFile nếu không tạo được ở đâu cả
    static std::filesystem::path ChuanBiThuMucDuLieu();
};

#endif
