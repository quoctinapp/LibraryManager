// ============================================================================
//  ĐỀ TÀI: PROJECT 3 - CHƯƠNG TRÌNH QUẢN LÝ THƯ VIỆN
//  Môn: Cấu trúc dữ liệu & Giải thuật - C++17 - Win32 API (không thư viện ngoài)
//
//  main chỉ tạo đối tượng chương trình và chạy. Toàn bộ logic nằm trong các lớp:
//      DanhSachLienKet<T> (template)   - cấu trúc dữ liệu (có sắp xếp trộn)
//      KhoDuLieu<T>, XuatCsv<T>        - đọc / ghi file, xuất CSV (template)
//      DoiTuongLuuTru (trừu tượng)     <- Sach, BanDoc, PhieuMuon, TaiKhoan
//      QuanLyThuVien                   - nghiệp vụ
//      CuaSoCoSo (trừu tượng)          <- CuaSoDangNhap, CuaSoChinh, TrangNoiDung (<- 4 trang), HopThoaiCoSo (<- 5 hộp thoại)
// ============================================================================

// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là "cửa chính" của chương trình: bấm đúp QuanLyThuVien.exe thì máy tính bắt đầu chạy từ hàm wWinMain ở file này (chương trình có cửa sổ của Windows dùng wWinMain thay cho main; chữ "w" = hỗ trợ tiếng Việt Unicode).
// ===========================================================================

#include "ChuongTrinh.h"
#include "TienIch.h"
#include <exception>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
    try {
        ChuongTrinh chuongTrinh;
        return chuongTrinh.Chay(hInstance);
    } catch (const std::exception& loi) {
        // [GIẢI THÍCH] Lưới an toàn cuối cùng: lỗi không lường trước được thì báo ra rồi thoát, thay vì để chương trình "chết" im lặng.
        MessageBoxW(nullptr, Utf8SangUtf16(loi.what()).c_str(), L"Lỗi nghiêm trọng", MB_ICONERROR | MB_OK);
        return 1;
    }
}
