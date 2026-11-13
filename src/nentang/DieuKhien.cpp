// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp DieuKhien (mục lục ở DieuKhien.h).
//
//  Cách cất thông tin nút vào một số nguyên 64 bit (GWLP_USERDATA):
//    bit  0..15 : mã biểu tượng          bit 24 : dấu hiệu "nút của chương trình"
//    bit 16..23 : kiểu nút (KieuNut)     bit 25 : chuột đang rê trên nút
//    bit 32..55 : màu nền của cửa sổ cha (để tô 4 góc ngoài phần bo tròn)
// ============================================================================

#include "DieuKhien.h"
#include "BoVe.h"
#include "CuaSoCoSo.h"
#include <commctrl.h>

static_assert(sizeof(LONG_PTR) >= 8, "Chuong trinh can bien dich 64-bit");

static const LONG_PTR CO_NUT_RIENG = static_cast<LONG_PTR>(1) << 24;
static const LONG_PTR CO_DANG_RE = static_cast<LONG_PTR>(1) << 25;

// Ô nhập: cất khoảng lề trái (bit 0..15) và mã biểu tượng (bit 16..31)
static const int CHIEU_CAO_KHUNG_MAC_DINH = 38;

// ============================================================================
//  Nút bấm
// ============================================================================

HWND DieuKhien::TaoNut(HWND cha, int id, const std::wstring& chu, wchar_t bieuTuong, KieuNut kieu, COLORREF mauNenCha) {
    HWND nut = CreateWindowExW(0, L"BUTTON", chu.c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW, 0, 0, 10, 10, cha, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), CuaSoCoSo::GetInstance(), nullptr);
    const LONG_PTR duLieu = static_cast<LONG_PTR>(bieuTuong) | (static_cast<LONG_PTR>(kieu) << 16) | CO_NUT_RIENG | (static_cast<LONG_PTR>(mauNenCha & 0xFFFFFF) << 32);
    SetWindowLongPtrW(nut, GWLP_USERDATA, duLieu);
    SendMessageW(nut, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::DAM)), FALSE);

    // [GIẢI THÍCH] "Móc" thêm một hàm xử lý vào nút (subclass) để biết khi nào chuột rê vào / rời khỏi nút -> vẽ lại nút với màu đậm hơn.
    SetWindowSubclass(nut, ThuTucNut, 1, 0);
    return nut;
}

LRESULT CALLBACK DieuKhien::ThuTucNut(HWND hwndNut, UINT thongDiep, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    switch (thongDiep) {
    case WM_MOUSEMOVE: {
        const LONG_PTR duLieu = GetWindowLongPtrW(hwndNut, GWLP_USERDATA);
        if ((duLieu & CO_DANG_RE) == 0) {
            SetWindowLongPtrW(hwndNut, GWLP_USERDATA, duLieu | CO_DANG_RE);
            TRACKMOUSEEVENT theoDoi = { sizeof(theoDoi), TME_LEAVE, hwndNut, 0 };
            TrackMouseEvent(&theoDoi);
            InvalidateRect(hwndNut, nullptr, FALSE);
        }
        break;
    }
    case WM_MOUSELEAVE: {
        const LONG_PTR duLieu = GetWindowLongPtrW(hwndNut, GWLP_USERDATA);
        SetWindowLongPtrW(hwndNut, GWLP_USERDATA, duLieu & ~CO_DANG_RE);
        InvalidateRect(hwndNut, nullptr, FALSE);
        break;
    }
    case WM_SETCURSOR:
        SetCursor(LoadCursorW(nullptr, IDC_HAND));
        return TRUE;
    case WM_NCDESTROY:
        RemoveWindowSubclass(hwndNut, ThuTucNut, 1);
        break;
    default:
        break;
    }

    return DefSubclassProc(hwndNut, thongDiep, wParam, lParam);
}

bool DieuKhien::VeNut(const DRAWITEMSTRUCT* thongTin) {
    if (thongTin == nullptr || thongTin->CtlType != ODT_BUTTON) {
        return false;
    }

    const LONG_PTR duLieu = GetWindowLongPtrW(thongTin->hwndItem, GWLP_USERDATA);
    if ((duLieu & CO_NUT_RIENG) == 0) {
        return false;
    }

    const wchar_t bieuTuong = static_cast<wchar_t>(duLieu & 0xFFFF);
    const KieuNut kieu = static_cast<KieuNut>((duLieu >> 16) & 0xFF);
    const COLORREF nenCha = static_cast<COLORREF>((duLieu >> 32) & 0xFFFFFF);
    const bool dangRe = (duLieu & CO_DANG_RE) != 0;
    const bool dangNhan = (thongTin->itemState & ODS_SELECTED) != 0;
    const bool biTat = (thongTin->itemState & ODS_DISABLED) != 0;
    const bool coTieuDiem = (thongTin->itemState & ODS_FOCUS) != 0 && (thongTin->itemState & ODS_NOFOCUSRECT) == 0;

    // Chọn màu theo kiểu nút
    COLORREF nen = ChuDe::NHAN;
    COLORREF vien = ChuDe::NHAN;
    COLORREF chu = ChuDe::TRANG;
    switch (kieu) {
    case KieuNut::CHINH:
        nen = ChuDe::NHAN;
        break;
    case KieuNut::NGUY_HIEM:
        nen = ChuDe::DO;
        break;
    case KieuNut::THANH_CONG:
        nen = ChuDe::XANH_LA;
        break;
    case KieuNut::PHU:
        nen = ChuDe::THE;
        vien = ChuDe::VIEN_DAM;
        chu = ChuDe::CHU;
        break;
    case KieuNut::TRONG_SUOT:
        nen = nenCha;
        break;
    }

    if (kieu != KieuNut::PHU) {
        vien = nen;
    }

    if (dangNhan) {
        nen = ChuDe::TronMau(nen, kieu == KieuNut::PHU ? ChuDe::NHAN : RGB(0, 0, 0), 0.18);
    } else if (dangRe) {
        nen = ChuDe::TronMau(nen, kieu == KieuNut::PHU ? ChuDe::NHAN : RGB(0, 0, 0), kieu == KieuNut::PHU ? 0.06 : 0.10);
    }

    if (kieu == KieuNut::TRONG_SUOT) {
        vien = nen;
    }

    if (biTat) {
        nen = ChuDe::VIEN;
        vien = ChuDe::VIEN;
        chu = ChuDe::CHU_PHU;
    }

    // Vẽ lên bộ đệm rồi chép ra (chống nháy)
    const RECT& vung = thongTin->rcItem;
    const int rong = vung.right - vung.left;
    const int cao = vung.bottom - vung.top;
    HDC hdc = CreateCompatibleDC(thongTin->hDC);
    HBITMAP anh = CreateCompatibleBitmap(thongTin->hDC, rong, cao);
    HGDIOBJ anhCu = SelectObject(hdc, anh);

    const RECT toanBo = { 0, 0, rong, cao };
    BoVe::ToNen(hdc, toanBo, nenCha);
    BoVe::ToBoGoc(hdc, toanBo, ChuDe::Dp(7), nen, vien);
    if (coTieuDiem && kieu != KieuNut::TRONG_SUOT) {
        RECT trong = toanBo;
        InflateRect(&trong, -ChuDe::Dp(3), -ChuDe::Dp(3));
        HBRUSH khongTo = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        HPEN but = CreatePen(PS_DOT, 1, kieu == KieuNut::PHU ? ChuDe::NHAN : ChuDe::TRANG);
        HGDIOBJ coCu = SelectObject(hdc, khongTo);
        HGDIOBJ butCu = SelectObject(hdc, but);
        RoundRect(hdc, trong.left, trong.top, trong.right, trong.bottom, ChuDe::Dp(8), ChuDe::Dp(8));
        SelectObject(hdc, coCu);
        SelectObject(hdc, butCu);
        DeleteObject(but);
    }

    // Biểu tượng + chữ nằm giữa nút
    const std::wstring noiDung = LayChu(thongTin->hwndItem);
    const SIZE coChu = BoVe::DoChu(hdc, noiDung, LoaiChu::DAM);
    const int rongBieuTuong = bieuTuong != 0 ? ChuDe::Dp(18) : 0;
    const int khoangCach = (bieuTuong != 0 && !noiDung.empty()) ? ChuDe::Dp(8) : 0;
    const int tongRong = rongBieuTuong + khoangCach + (noiDung.empty() ? 0 : coChu.cx);
    int x = (rong - tongRong) / 2;
    if (bieuTuong != 0) {
        const RECT vungBieuTuong = { x, 0, x + rongBieuTuong, cao };
        BoVe::VeBieuTuong(hdc, bieuTuong, vungBieuTuong, kieu == KieuNut::TRONG_SUOT ? LoaiChu::BIEU_TUONG : LoaiChu::BIEU_TUONG_NHO, chu);
        x += rongBieuTuong + khoangCach;
    }

    if (!noiDung.empty()) {
        const RECT vungChu = { x, 0, x + coChu.cx + 2, cao };
        BoVe::VietChu(hdc, noiDung, vungChu, LoaiChu::DAM, chu);
    }

    BitBlt(thongTin->hDC, vung.left, vung.top, rong, cao, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, anhCu);
    DeleteObject(anh);
    DeleteDC(hdc);
    return true;
}

// ============================================================================
//  Ô nhập, hộp chọn, ô chọn ngày
// ============================================================================

HWND DieuKhien::TaoONhap(HWND cha, int id, DWORD kieuThem, int doDaiToiDa, wchar_t bieuTuong) {
    HWND oNhap = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | kieuThem, 0, 0, 10, 10, cha, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), CuaSoCoSo::GetInstance(), nullptr);
    SendMessageW(oNhap, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::THUONG)), FALSE);
    if (doDaiToiDa > 0) {
        SendMessageW(oNhap, EM_SETLIMITTEXT, static_cast<WPARAM>(doDaiToiDa), 0);
    }

    const int leTrai = bieuTuong != 0 ? ChuDe::Dp(36) : ChuDe::Dp(12);
    SetWindowLongPtrW(oNhap, GWLP_USERDATA, static_cast<LONG_PTR>(leTrai) | (static_cast<LONG_PTR>(bieuTuong) << 16));
    return oNhap;
}

HWND DieuKhien::TaoHopChon(HWND cha, int id, bool choPhepGo) {
    HWND hop = CreateWindowExW(0, WC_COMBOBOXW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | (choPhepGo ? CBS_DROPDOWN | CBS_AUTOHSCROLL : CBS_DROPDOWNLIST), 0, 0, 10, ChuDe::Dp(300), cha, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), CuaSoCoSo::GetInstance(), nullptr);
    SendMessageW(hop, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::THUONG)), FALSE);
    // Ô hiển thị cao bằng các ô nhập khác; mỗi dòng trong danh sách thả xuống cao hơn cho dễ chọn
    SendMessageW(hop, CB_SETITEMHEIGHT, static_cast<WPARAM>(-1), ChuDe::Dp(CHIEU_CAO_KHUNG_MAC_DINH - 8));
    SendMessageW(hop, CB_SETITEMHEIGHT, 0, ChuDe::Dp(26));
    return hop;
}

HWND DieuKhien::TaoChonNgay(HWND cha, int id) {
    HWND chonNgay = CreateWindowExW(0, DATETIMEPICK_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | DTS_SHORTDATEFORMAT, 0, 0, 10, 10, cha, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), CuaSoCoSo::GetInstance(), nullptr);
    SendMessageW(chonNgay, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::THUONG)), FALSE);
    SendMessageW(chonNgay, DTM_SETFORMATW, 0, reinterpret_cast<LPARAM>(L"dd/MM/yyyy"));
    return chonNgay;
}

// [GIẢI THÍCH] Ô nhập được đặt ở giữa khung theo chiều dọc, cách lề trái một khoảng (lớn hơn nếu có biểu tượng), cách lề phải 12px.
void DieuKhien::DatViTriONhap(HWND oNhap, int x, int y, int rong, int cao) {
    const int leTrai = static_cast<int>(GetWindowLongPtrW(oNhap, GWLP_USERDATA) & 0xFFFF);
    const int caoDong = ChuDe::ChieuCaoDong(LoaiChu::THUONG) + 2;
    MoveWindow(oNhap, x + leTrai, y + (cao - caoDong) / 2, rong - leTrai - ChuDe::Dp(12), caoDong, TRUE);
}

RECT DieuKhien::KhungCuaONhap(HWND oNhap) {
    const int leTrai = static_cast<int>(GetWindowLongPtrW(oNhap, GWLP_USERDATA) & 0xFFFF);
    const RECT o = VungTrongCha(oNhap);
    const int caoKhung = ChuDe::Dp(CHIEU_CAO_KHUNG_MAC_DINH);
    const int caoDong = o.bottom - o.top;
    RECT khung;
    khung.left = o.left - leTrai;
    khung.right = o.right + ChuDe::Dp(12);
    khung.top = o.top - (caoKhung - caoDong) / 2;
    khung.bottom = khung.top + caoKhung;
    return khung;
}

wchar_t DieuKhien::BieuTuongCuaONhap(HWND oNhap) {
    return static_cast<wchar_t>((GetWindowLongPtrW(oNhap, GWLP_USERDATA) >> 16) & 0xFFFF);
}

// ============================================================================
//  Tiện ích chung
// ============================================================================

void DieuKhien::DatChu(HWND dieuKhien, const std::wstring& chu) {
    SetWindowTextW(dieuKhien, chu.c_str());
}

std::wstring DieuKhien::LayChu(HWND dieuKhien) {
    const int doDai = GetWindowTextLengthW(dieuKhien);
    std::wstring chu(static_cast<std::size_t>(doDai) + 1, L'\0');
    GetWindowTextW(dieuKhien, &chu[0], doDai + 1);
    chu.resize(static_cast<std::size_t>(doDai));
    return chu;
}

RECT DieuKhien::VungTrongCha(HWND con) {
    RECT vung;
    GetWindowRect(con, &vung);
    MapWindowPoints(HWND_DESKTOP, GetParent(con), reinterpret_cast<POINT*>(&vung), 2);
    return vung;
}
