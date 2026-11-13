// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của BoVe và BoDemVe (mục lục ở BoVe.h).
//  Quy tắc của GDI: mượn bút (pen) / cọ (brush) bằng CreateXxx, chọn vào tờ giấy bằng SelectObject, vẽ xong phải chọn lại đồ cũ và DeleteObject để trả lại cho Windows, nếu không chương trình sẽ "rò rỉ" tài nguyên.
// ============================================================================

#include "BoVe.h"
#include <cmath>

// ============================================================================
//  BoVe
// ============================================================================

void BoVe::ToNen(HDC hdc, const RECT& vung, COLORREF mau) {
    HBRUSH co = CreateSolidBrush(mau);
    FillRect(hdc, &vung, co);
    DeleteObject(co);
}

void BoVe::ToBoGoc(HDC hdc, const RECT& vung, int banKinh, COLORREF mauNen, COLORREF mauVien) {
    HBRUSH co = CreateSolidBrush(mauNen);
    HPEN but = CreatePen(PS_SOLID, 1, mauVien);
    HGDIOBJ coCu = SelectObject(hdc, co);
    HGDIOBJ butCu = SelectObject(hdc, but);
    RoundRect(hdc, vung.left, vung.top, vung.right, vung.bottom, banKinh * 2, banKinh * 2);
    SelectObject(hdc, coCu);
    SelectObject(hdc, butCu);
    DeleteObject(co);
    DeleteObject(but);
}

// [GIẢI THÍCH] GradientFill (thư viện msimg32 của Windows) tô một hình chữ nhật chuyển dần từ màu đầu sang màu cuối. Mỗi màu ở đây dùng thang 16 bit.
void BoVe::ToChuyenMau(HDC hdc, const RECT& vung, COLORREF mauDau, COLORREF mauCuoi, bool theoChieuNgang) {
    TRIVERTEX dinh[2] = {};
    dinh[0].x = vung.left;
    dinh[0].y = vung.top;
    dinh[0].Red = static_cast<COLOR16>(GetRValue(mauDau) << 8);
    dinh[0].Green = static_cast<COLOR16>(GetGValue(mauDau) << 8);
    dinh[0].Blue = static_cast<COLOR16>(GetBValue(mauDau) << 8);
    dinh[1].x = vung.right;
    dinh[1].y = vung.bottom;
    dinh[1].Red = static_cast<COLOR16>(GetRValue(mauCuoi) << 8);
    dinh[1].Green = static_cast<COLOR16>(GetGValue(mauCuoi) << 8);
    dinh[1].Blue = static_cast<COLOR16>(GetBValue(mauCuoi) << 8);

    GRADIENT_RECT hinh = { 0, 1 };
    GradientFill(hdc, dinh, 2, &hinh, 1, theoChieuNgang ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
}

void BoVe::ToHinhTron(HDC hdc, int tamX, int tamY, int banKinh, COLORREF mau) {
    HBRUSH co = CreateSolidBrush(mau);
    HPEN but = CreatePen(PS_SOLID, 1, mau);
    HGDIOBJ coCu = SelectObject(hdc, co);
    HGDIOBJ butCu = SelectObject(hdc, but);
    Ellipse(hdc, tamX - banKinh, tamY - banKinh, tamX + banKinh + 1, tamY + banKinh + 1);
    SelectObject(hdc, coCu);
    SelectObject(hdc, butCu);
    DeleteObject(co);
    DeleteObject(but);
}

void BoVe::VeDuong(HDC hdc, int x1, int y1, int x2, int y2, COLORREF mau, int doDay) {
    HPEN but = CreatePen(PS_SOLID, doDay, mau);
    HGDIOBJ butCu = SelectObject(hdc, but);
    MoveToEx(hdc, x1, y1, nullptr);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, butCu);
    DeleteObject(but);
}

void BoVe::VietChu(HDC hdc, const std::wstring& chu, const RECT& vung, LoaiChu loai, COLORREF mau, UINT dinhDang) {
    RECT vungViet = vung;
    HGDIOBJ phongCu = SelectObject(hdc, ChuDe::Phong(loai));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, mau);
    DrawTextW(hdc, chu.c_str(), static_cast<int>(chu.size()), &vungViet, dinhDang | DT_NOPREFIX);
    SelectObject(hdc, phongCu);
}

SIZE BoVe::DoChu(HDC hdc, const std::wstring& chu, LoaiChu loai) {
    SIZE kichThuoc{};
    HGDIOBJ phongCu = SelectObject(hdc, ChuDe::Phong(loai));
    GetTextExtentPoint32W(hdc, chu.c_str(), static_cast<int>(chu.size()), &kichThuoc);
    SelectObject(hdc, phongCu);
    return kichThuoc;
}

void BoVe::VeBieuTuong(HDC hdc, wchar_t bieuTuong, const RECT& vung, LoaiChu loai, COLORREF mau) {
    VietChu(hdc, std::wstring(1, bieuTuong), vung, loai, mau, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// [GIẢI THÍCH] GDI vẽ hình tròn bị "răng cưa". Mẹo: vẽ to gấp 3 lần lên tấm ảnh phụ, rồi thu nhỏ về kích thước thật bằng chế độ HALFTONE (lấy trung bình màu các điểm ảnh) -> mép tròn trở nên mịn.
void BoVe::VeDonut(HDC hdc, const RECT& vung, double tiLe, COLORREF mauChinh, COLORREF mauPhu, COLORREF mauNen, int doDayVong) {
    const int HE_SO = 3;
    const int rong = vung.right - vung.left;
    const int cao = vung.bottom - vung.top;
    const int rongLon = rong * HE_SO;
    const int caoLon = cao * HE_SO;

    HDC hdcLon = CreateCompatibleDC(hdc);
    HBITMAP anhLon = CreateCompatibleBitmap(hdc, rongLon, caoLon);
    HGDIOBJ anhCu = SelectObject(hdcLon, anhLon);

    const RECT toanBo = { 0, 0, rongLon, caoLon };
    ToNen(hdcLon, toanBo, mauNen);

    const int banKinh = (rongLon < caoLon ? rongLon : caoLon) / 2 - HE_SO;
    const int tamX = rongLon / 2;
    const int tamY = caoLon / 2;
    ToHinhTron(hdcLon, tamX, tamY, banKinh, mauPhu);

    if (tiLe >= 0.999) {
        ToHinhTron(hdcLon, tamX, tamY, banKinh, mauChinh);
    } else if (tiLe > 0.001) {
        // Quạt tròn từ 12 giờ, quay theo chiều kim đồng hồ một góc = tiLe * 360 độ
        const double PI = 3.14159265358979323846;
        const double goc = tiLe * 2.0 * PI;
        const int xCuoi = tamX + static_cast<int>(banKinh * std::sin(goc));
        const int yCuoi = tamY - static_cast<int>(banKinh * std::cos(goc));

        HBRUSH co = CreateSolidBrush(mauChinh);
        HPEN but = CreatePen(PS_SOLID, 1, mauChinh);
        HGDIOBJ coCu = SelectObject(hdcLon, co);
        HGDIOBJ butCu = SelectObject(hdcLon, but);
        SetArcDirection(hdcLon, AD_CLOCKWISE);
        Pie(hdcLon, tamX - banKinh, tamY - banKinh, tamX + banKinh, tamY + banKinh, tamX, tamY - banKinh, xCuoi, yCuoi);
        SelectObject(hdcLon, coCu);
        SelectObject(hdcLon, butCu);
        DeleteObject(co);
        DeleteObject(but);
    }

    ToHinhTron(hdcLon, tamX, tamY, banKinh - doDayVong * HE_SO, mauNen);    // khoét lỗ giữa

    SetStretchBltMode(hdc, HALFTONE);
    SetBrushOrgEx(hdc, 0, 0, nullptr);
    StretchBlt(hdc, vung.left, vung.top, rong, cao, hdcLon, 0, 0, rongLon, caoLon, SRCCOPY);

    SelectObject(hdcLon, anhCu);
    DeleteObject(anhLon);
    DeleteDC(hdcLon);
}

// ============================================================================
//  BoDemVe
// ============================================================================

BoDemVe::BoDemVe(HWND hwnd) : hwnd(hwnd), thongTinVe{}, hdcManHinh(nullptr), hdcDem(nullptr), anhDem(nullptr), anhCu(nullptr), vung{} {
    hdcManHinh = BeginPaint(hwnd, &thongTinVe);
    GetClientRect(hwnd, &vung);
    hdcDem = CreateCompatibleDC(hdcManHinh);
    anhDem = CreateCompatibleBitmap(hdcManHinh, vung.right > 0 ? vung.right : 1, vung.bottom > 0 ? vung.bottom : 1);
    anhCu = SelectObject(hdcDem, anhDem);
}

BoDemVe::~BoDemVe() {
    BitBlt(hdcManHinh, 0, 0, vung.right, vung.bottom, hdcDem, 0, 0, SRCCOPY);
    SelectObject(hdcDem, anhCu);
    DeleteObject(anhDem);
    DeleteDC(hdcDem);
    EndPaint(hwnd, &thongTinVe);
}
