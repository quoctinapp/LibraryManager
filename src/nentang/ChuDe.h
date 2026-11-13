#ifndef CHUDE_H
#define CHUDE_H

// ============================================================================
//  ChuDe.h - "Chủ đề" giao diện: bảng màu, phông chữ, co giãn theo DPI
//      - Bảng màu hiện đại (nền xám nhạt, thẻ trắng, màu nhấn xanh dương).
//      - Phông Segoe UI nhiều cỡ + phông biểu tượng Segoe MDL2 / Fluent Icons (có sẵn trong Windows 10/11, không cần file ảnh).
//      - Dp(x): đổi kích thước thiết kế ở 100% sang kích thước thật theo DPI (màn hình đặt tỉ lệ 125%, 150% thì mọi thứ to lên tương ứng).
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Giống "bộ nhận diện thương hiệu" của một công ty: mọi cửa sổ đều lấy màu và chữ từ đây, nên giao diện đồng bộ và muốn đổi màu chỉ cần sửa một chỗ.
//  COLORREF = một màu, viết bằng RGB(đỏ, xanh lá, xanh dương), mỗi số 0..255.
//  HFONT = một kiểu chữ (phông + cỡ + độ đậm) mà Windows đã tạo sẵn để vẽ.
// ----------------------------------------------------------------------------

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

enum class LoaiChu {
    THUONG = 0,         // chữ nội dung 10pt
    DAM,                // chữ nội dung đậm
    NHO,                // chú thích 9pt
    NHO_DAM,
    TIEU_DE,            // tiêu đề trang 15pt
    TIEU_DE_LON,        // tiêu đề lớn 19pt
    SO_LON,             // con số trên thẻ thống kê 22pt
    BIEU_TUONG_NHO,     // phông biểu tượng
    BIEU_TUONG,
    BIEU_TUONG_LON,
    SO_LOAI_CHU
};

class ChuDe {
private:
    static int dpi;
    static HFONT phongChu[static_cast<int>(LoaiChu::SO_LOAI_CHU)];
    static int chieuCaoDong[static_cast<int>(LoaiChu::SO_LOAI_CHU)];

    static HFONT TaoPhong(const wchar_t* tenPhong, int coChu, int doDam);
    static bool CoPhongChu(const wchar_t* tenPhong);

public:
    // ---- Bảng màu ----
    static constexpr COLORREF NEN = RGB(241, 245, 249);
    static constexpr COLORREF THE = RGB(255, 255, 255);
    static constexpr COLORREF VIEN = RGB(226, 232, 240);
    static constexpr COLORREF VIEN_DAM = RGB(203, 213, 225);
    static constexpr COLORREF CHU = RGB(15, 23, 42);
    static constexpr COLORREF CHU_PHU = RGB(100, 116, 139);
    static constexpr COLORREF DONG_LE = RGB(248, 250, 252);

    static constexpr COLORREF THANH_BEN = RGB(15, 23, 42);
    static constexpr COLORREF THANH_BEN_TRO = RGB(30, 41, 59);
    static constexpr COLORREF THANH_BEN_CHU = RGB(203, 213, 225);
    static constexpr COLORREF THANH_BEN_CHU_MO = RGB(100, 116, 139);

    static constexpr COLORREF NHAN = RGB(37, 99, 235);
    static constexpr COLORREF NHAN_DAM = RGB(29, 78, 216);
    static constexpr COLORREF NHAN_NHAT = RGB(219, 234, 254);
    static constexpr COLORREF TIM = RGB(124, 58, 237);
    static constexpr COLORREF TIM_NHAT = RGB(237, 233, 254);
    static constexpr COLORREF XANH_LA = RGB(22, 163, 74);
    static constexpr COLORREF XANH_LA_DAM = RGB(21, 128, 61);
    static constexpr COLORREF XANH_LA_NHAT = RGB(220, 252, 231);
    static constexpr COLORREF CAM = RGB(217, 119, 6);
    static constexpr COLORREF CAM_NHAT = RGB(254, 243, 199);
    static constexpr COLORREF DO = RGB(220, 38, 38);
    static constexpr COLORREF DO_DAM = RGB(185, 28, 28);
    static constexpr COLORREF DO_NHAT = RGB(254, 226, 226);
    static constexpr COLORREF TRANG = RGB(255, 255, 255);

    // ---- Mã biểu tượng trong phông Segoe MDL2 Assets / Segoe Fluent Icons ----
    static constexpr wchar_t BT_TRANG_CHU = 0xE80F;
    static constexpr wchar_t BT_THU_VIEN = 0xE8F1;
    static constexpr wchar_t BT_TAI_LIEU = 0xE8A5;
    static constexpr wchar_t BT_NGUOI = 0xE716;
    static constexpr wchar_t BT_LIEN_HE = 0xE77B;
    static constexpr wchar_t BT_KHOA = 0xE72E;
    static constexpr wchar_t BT_THEM = 0xE710;
    static constexpr wchar_t BT_XOA = 0xE74D;
    static constexpr wchar_t BT_TIM = 0xE721;
    static constexpr wchar_t BT_TAI_XUONG = 0xE896;
    static constexpr wchar_t BT_DONG_Y = 0xE73E;
    static constexpr wchar_t BT_DONG = 0xE711;
    static constexpr wchar_t BT_CANH_BAO = 0xE7BA;
    static constexpr wchar_t BT_LICH = 0xE787;
    static constexpr wchar_t BT_TOI = 0xE72A;
    static constexpr wchar_t BT_LUI = 0xE72B;
    static constexpr wchar_t BT_THOAT = 0xE7E8;
    static constexpr wchar_t BT_SACH_MO = 0xE82D;

    static void KhoiTao();
    static void GiaiPhong();

    static int Dp(int giaTri);                  // co giãn theo DPI
    static HFONT Phong(LoaiChu loai);
    static int ChieuCaoDong(LoaiChu loai);      // chiều cao 1 dòng chữ (pixel)

    // Pha trộn 2 màu: tiLe = 0 -> mau1, tiLe = 1 -> mau2
    static COLORREF TronMau(COLORREF mau1, COLORREF mau2, double tiLe);
};

#endif
