#ifndef DIEUKHIEN_H
#define DIEUKHIEN_H

// ============================================================================
//  DieuKhien.h - Tạo các "điều khiển" (control) chuẩn của Windows với giao diện
//  hiện đại: nút bấm tự vẽ (bo góc, có biểu tượng, đổi màu khi rê chuột),
//  ô nhập không viền đặt trong khung bo góc, hộp chọn (combobox), ô chọn ngày.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Nút bấm mặc định của Windows trông khá "cổ". Nút ở đây có kiểu BS_OWNERDRAW:
//  Windows vẫn lo việc bấm, Tab, Enter... nhưng giao việc VẼ cho chương trình
//  (thông điệp WM_DRAWITEM) -> ta tự vẽ nút bo góc, có màu và biểu tượng.
//  Thông tin kiểu nút (màu, biểu tượng, đang rê chuột hay không) được cất ngay
//  trong cửa sổ nút (GWLP_USERDATA) nên không cần biến toàn cục.
//  Ô nhập cũng không có viền: cửa sổ cha vẽ một khung bo góc bao quanh, khung
//  đổi sang màu xanh khi ô đang được nhập (xem CuaSoCoSo::VeKhungCacONhap).
// ----------------------------------------------------------------------------

#include "ChuDe.h"

#include <string>

enum class KieuNut {
    CHINH = 0,      // nền xanh, chữ trắng (hành động chính)
    PHU,            // nền trắng, viền xám
    NGUY_HIEM,      // nền đỏ (xóa)
    THANH_CONG,     // nền xanh lá (trả sách)
    TRONG_SUOT      // chỉ có biểu tượng màu trắng (nút đóng trên nền màu)
};

class DieuKhien {
private:
    static LRESULT CALLBACK ThuTucNut(HWND hwndNut, UINT thongDiep, WPARAM wParam, LPARAM lParam,
                                      UINT_PTR maMoc, DWORD_PTR duLieu);

public:
    DieuKhien() = delete;

    static HWND TaoNut(HWND cha, int id, const std::wstring& chu, wchar_t bieuTuong, KieuNut kieu,
                       COLORREF mauNenCha);
    // Ô nhập một dòng. leTrai = khoảng trống bên trái trong khung (chừa chỗ cho biểu tượng)
    static HWND TaoONhap(HWND cha, int id, DWORD kieuThem, int doDaiToiDa, wchar_t bieuTuong = 0);
    static HWND TaoHopChon(HWND cha, int id, bool choPhepGo);
    static HWND TaoChonNgay(HWND cha, int id);

    // Đặt ô nhập sao cho KHUNG bao quanh nó nằm đúng vị trí (x, y, rong, cao)
    static void DatViTriONhap(HWND oNhap, int x, int y, int rong, int cao);
    static RECT KhungCuaONhap(HWND oNhap);              // khung bo góc (tọa độ trong cửa sổ cha)
    static wchar_t BieuTuongCuaONhap(HWND oNhap);

    static void DatChu(HWND dieuKhien, const std::wstring& chu);
    static std::wstring LayChu(HWND dieuKhien);
    static RECT VungTrongCha(HWND con);

    // Vẽ nút tự vẽ. Trả về false nếu không phải nút do DieuKhien tạo.
    static bool VeNut(const DRAWITEMSTRUCT* thongTin);
};

#endif
