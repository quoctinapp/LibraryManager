// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp ChuDe (mục lục ở ChuDe.h).
//  KhoiTao() được gọi MỘT lần khi chương trình bắt đầu: đọc DPI của màn hình và tạo sẵn các phông chữ. GiaiPhong() trả phông chữ lại cho Windows khi thoát.
// ============================================================================

#include "ChuDe.h"

int ChuDe::dpi = 96;
HFONT ChuDe::phongChu[static_cast<int>(LoaiChu::SO_LOAI_CHU)] = {};
int ChuDe::chieuCaoDong[static_cast<int>(LoaiChu::SO_LOAI_CHU)] = {};

// [GIẢI THÍCH] Hỏi Windows xem máy có cài phông chữ này không.
static int CALLBACK DemPhongChu(const LOGFONTW*, const TEXTMETRICW*, DWORD, LPARAM thamSo) {
    *reinterpret_cast<bool*>(thamSo) = true;
    return 0;   // tìm thấy 1 phông là đủ, dừng liệt kê
}

bool ChuDe::CoPhongChu(const wchar_t* tenPhong) {
    LOGFONTW moTa{};
    moTa.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(moTa.lfFaceName, tenPhong, LF_FACESIZE);

    bool timThay = false;
    HDC hdc = GetDC(nullptr);
    EnumFontFamiliesExW(hdc, &moTa, DemPhongChu, reinterpret_cast<LPARAM>(&timThay), 0);
    ReleaseDC(nullptr, hdc);
    return timThay;
}

HFONT ChuDe::TaoPhong(const wchar_t* tenPhong, int coChu, int doDam) {
    LOGFONTW moTa{};
    moTa.lfHeight = -MulDiv(coChu, dpi, 72);    // cỡ chữ (point) -> pixel
    moTa.lfWeight = doDam;
    moTa.lfCharSet = DEFAULT_CHARSET;
    moTa.lfQuality = CLEARTYPE_QUALITY;
    lstrcpynW(moTa.lfFaceName, tenPhong, LF_FACESIZE);
    return CreateFontIndirectW(&moTa);
}

void ChuDe::KhoiTao() {
    HDC hdc = GetDC(nullptr);
    dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(nullptr, hdc);
    if (dpi <= 0) {
        dpi = 96;
    }

    // Windows 11 có "Segoe Fluent Icons", Windows 10 có "Segoe MDL2 Assets" (cùng mã biểu tượng)
    const wchar_t* phongBieuTuong = CoPhongChu(L"Segoe Fluent Icons") ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";

    struct MoTaPhong {
        const wchar_t* ten;
        int co;
        int doDam;
    };
    const MoTaPhong bang[] = {
        { L"Segoe UI", 10, FW_NORMAL },        // THUONG
        { L"Segoe UI", 10, FW_SEMIBOLD },      // DAM
        { L"Segoe UI", 9, FW_NORMAL },         // NHO
        { L"Segoe UI", 9, FW_SEMIBOLD },       // NHO_DAM
        { L"Segoe UI", 15, FW_SEMIBOLD },      // TIEU_DE
        { L"Segoe UI", 19, FW_BOLD },          // TIEU_DE_LON
        { L"Segoe UI", 22, FW_BOLD },          // SO_LON
        { phongBieuTuong, 10, FW_NORMAL },     // BIEU_TUONG_NHO
        { phongBieuTuong, 13, FW_NORMAL },     // BIEU_TUONG
        { phongBieuTuong, 20, FW_NORMAL },     // BIEU_TUONG_LON
    };

    hdc = GetDC(nullptr);
    for (int i = 0; i < static_cast<int>(LoaiChu::SO_LOAI_CHU); ++i) {
        phongChu[i] = TaoPhong(bang[i].ten, bang[i].co, bang[i].doDam);

        TEXTMETRICW soDo{};
        HGDIOBJ cu = SelectObject(hdc, phongChu[i]);
        GetTextMetricsW(hdc, &soDo);
        SelectObject(hdc, cu);
        chieuCaoDong[i] = soDo.tmHeight;
    }

    ReleaseDC(nullptr, hdc);
}

void ChuDe::GiaiPhong() {
    for (HFONT& phong : phongChu) {
        if (phong != nullptr) {
            DeleteObject(phong);
            phong = nullptr;
        }
    }
}

int ChuDe::Dp(int giaTri) {
    return MulDiv(giaTri, dpi, 96);
}

HFONT ChuDe::Phong(LoaiChu loai) {
    return phongChu[static_cast<int>(loai)];
}

int ChuDe::ChieuCaoDong(LoaiChu loai) {
    return chieuCaoDong[static_cast<int>(loai)];
}

COLORREF ChuDe::TronMau(COLORREF mau1, COLORREF mau2, double tiLe) {
    auto tron = [tiLe](int a, int b) { return static_cast<int>(a + (b - a) * tiLe + 0.5); };
    return RGB(tron(GetRValue(mau1), GetRValue(mau2)), tron(GetGValue(mau1), GetGValue(mau2)), tron(GetBValue(mau1), GetBValue(mau2)));
}
