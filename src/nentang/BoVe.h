#ifndef BOVE_H
#define BOVE_H

// ============================================================================
//  BoVe.h - Các hàm vẽ bằng GDI của Windows và bộ đệm vẽ chống nháy
//      - BoVe     : hình chữ nhật bo góc, nền chuyển màu, hình tròn, chữ, biểu tượng,
//                   vòng tròn khuyết (biểu đồ donut) có khử răng cưa.
//      - BoDemVe  : vẽ mọi thứ lên một "tấm ảnh ẩn" rồi chép lên màn hình 1 lần (double-buffering) -> giao diện không bị nháy khi vẽ lại.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  GDI là bộ "bút và cọ" có sẵn trong Windows. HDC là "tờ giấy" đang vẽ lên.
//  Nếu vẽ thẳng lên màn hình từng nét một, mắt người sẽ thấy hình nhấp nháy.
//  Vì vậy BoDemVe tạo một tờ giấy nháp vô hình cùng kích thước, vẽ hết lên đó, xong mới "dán" cả tờ lên màn hình bằng BitBlt -> hình hiện ra mượt mà.
//  BoDemVe dùng kỹ thuật RAII: tạo đối tượng = bắt đầu vẽ, đối tượng bị hủy (ra khỏi khối { }) = tự động dán lên màn hình và dọn dẹp.
// ----------------------------------------------------------------------------

#include "ChuDe.h"
#include <string>

class BoVe {
public:
    BoVe() = delete;    // chỉ chứa hàm tĩnh

    static void ToNen(HDC hdc, const RECT& vung, COLORREF mau);
    static void ToBoGoc(HDC hdc, const RECT& vung, int banKinh, COLORREF mauNen, COLORREF mauVien);
    static void ToChuyenMau(HDC hdc, const RECT& vung, COLORREF mauDau, COLORREF mauCuoi, bool theoChieuNgang);
    static void ToHinhTron(HDC hdc, int tamX, int tamY, int banKinh, COLORREF mau);
    static void VeDuong(HDC hdc, int x1, int y1, int x2, int y2, COLORREF mau, int doDay = 1);

    static void VietChu(HDC hdc, const std::wstring& chu, const RECT& vung, LoaiChu loai, COLORREF mau, UINT dinhDang = DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    static SIZE DoChu(HDC hdc, const std::wstring& chu, LoaiChu loai);
    static void VeBieuTuong(HDC hdc, wchar_t bieuTuong, const RECT& vung, LoaiChu loai, COLORREF mau);

    // Vòng tròn khuyết (donut): phần tiLe (0..1) tô mauChinh, phần còn lại tô mauPhu, bắt đầu từ vị trí 12 giờ theo chiều kim đồng hồ. Vẽ to gấp 3 rồi thu nhỏ để mịn.
    static void VeDonut(HDC hdc, const RECT& vung, double tiLe, COLORREF mauChinh, COLORREF mauPhu, COLORREF mauNen, int doDayVong);
};

class BoDemVe {
private:
    HWND hwnd;
    PAINTSTRUCT thongTinVe;
    HDC hdcManHinh;
    HDC hdcDem;
    HBITMAP anhDem;
    HGDIOBJ anhCu;
    RECT vung;

public:
    explicit BoDemVe(HWND hwnd);
    ~BoDemVe();
    BoDemVe(const BoDemVe&) = delete;
    BoDemVe& operator=(const BoDemVe&) = delete;

    HDC GetHdc() const { return hdcDem; }
    const RECT& GetVung() const { return vung; }
};

#endif
