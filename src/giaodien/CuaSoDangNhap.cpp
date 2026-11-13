// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của CuaSoDangNhap (mục lục ở CuaSoDangNhap.h).
// ============================================================================

#include "CuaSoDangNhap.h"
#include "BoVe.h"
#include "CauHinh.h"
#include "DieuKhien.h"
#include "HopThongBao.h"
#include "MaDieuKhien.h"
#include "TienIch.h"
#include <commctrl.h>

static const int RONG_CUA_SO = 440;
static const int CAO_CUA_SO = 580;
static const int CAO_DAI_MAU = 200;
static const int LE = 40;
static const COLORREF MAU_DAI_TREN = RGB(30, 64, 175);
static const COLORREF MAU_DAI_DUOI = RGB(91, 33, 182);

CuaSoDangNhap::CuaSoDangNhap(const QuanLyThuVien& quanLy) : quanLy(quanLy), soLanConLai(CauHinh::SO_LAN_DANG_NHAP_TOI_DA), thongBaoLaLoi(false), ketQua(KetQuaDangNhap::THOAT), oTen(nullptr), oMatKhau(nullptr), nutDangNhap(nullptr), nutDong(nullptr) {}

KetQuaDangNhap CuaSoDangNhap::Chay() {
    const int rong = ChuDe::Dp(RONG_CUA_SO);
    const int cao = ChuDe::Dp(CAO_CUA_SO);
    RECT vungLamViec;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &vungLamViec, 0);
    const int x = vungLamViec.left + (vungLamViec.right - vungLamViec.left - rong) / 2;
    const int y = vungLamViec.top + (vungLamViec.bottom - vungLamViec.top - cao) / 2;

    if (!TaoCuaSo(WS_EX_APPWINDOW | WS_EX_CONTROLPARENT, WS_POPUP | WS_CLIPCHILDREN | WS_MINIMIZEBOX, L"Đăng nhập – Quản lý thư viện", x, y, rong, cao, nullptr, true)) {
        return KetQuaDangNhap::THOAT;
    }

    TaoDieuKhien();
    thongBao = L"Còn " + std::to_wstring(soLanConLai) + L" lần thử   ·   Nhấn ESC để thoát";

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(oTen);
    ChayVongLapThongDiep();
    return ketQua;
}

void CuaSoDangNhap::TaoDieuKhien() {
    const int le = ChuDe::Dp(LE);
    const int rong = ChuDe::Dp(RONG_CUA_SO) - 2 * le;
    const int caoO = ChuDe::Dp(44);

    oTen = DieuKhien::TaoONhap(hwnd, MA_O_TEN_DANG_NHAP, 0, static_cast<int>(CauHinh::DO_DAI_TAI_KHOAN_TOI_DA), ChuDe::BT_LIEN_HE);
    DieuKhien::DatViTriONhap(oTen, le, ChuDe::Dp(296), rong, caoO);
    SendMessageW(oTen, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(L"Nhập tên đăng nhập"));

    oMatKhau = DieuKhien::TaoONhap(hwnd, MA_O_MAT_KHAU, ES_PASSWORD, static_cast<int>(CauHinh::DO_DAI_MAT_KHAU_TOI_DA), ChuDe::BT_KHOA);
    DieuKhien::DatViTriONhap(oMatKhau, le, ChuDe::Dp(376), rong, caoO);
    SendMessageW(oMatKhau, EM_SETPASSWORDCHAR, L'*', 0);     // đề bài: mật khẩu hiện dấu *
    SendMessageW(oMatKhau, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(L"Nhập mật khẩu"));

    dsONhap.push_back(oTen);
    dsONhap.push_back(oMatKhau);

    nutDangNhap = DieuKhien::TaoNut(hwnd, IDOK, L"Đăng nhập", ChuDe::BT_TOI, KieuNut::CHINH, ChuDe::THE);
    MoveWindow(nutDangNhap, le, ChuDe::Dp(464), rong, ChuDe::Dp(46), FALSE);

    const int coNutDong = ChuDe::Dp(36);
    nutDong = DieuKhien::TaoNut(hwnd, IDCANCEL, L"", ChuDe::BT_DONG, KieuNut::TRONG_SUOT, MAU_DAI_TREN);
    MoveWindow(nutDong, ChuDe::Dp(RONG_CUA_SO) - coNutDong - ChuDe::Dp(8), ChuDe::Dp(8), coNutDong, coNutDong, FALSE);
}

void CuaSoDangNhap::ThuDangNhap() {
    const std::wstring ten = DieuKhien::LayChu(oTen);
    const std::wstring matKhau = DieuKhien::LayChu(oMatKhau);
    if (CatKhoangTrang(Utf16SangUtf8(ten)).empty()) {
        thongBao = L"Vui lòng nhập tên đăng nhập.";
        thongBaoLaLoi = true;
        InvalidateRect(hwnd, nullptr, FALSE);
        SetFocus(oTen);
        return;
    }

    const std::string tenUtf8 = CatKhoangTrang(Utf16SangUtf8(ten));
    if (quanLy.KiemTraDangNhap(tenUtf8, Utf16SangUtf8(matKhau))) {
        tenDangNhap = Utf8SangUtf16(tenUtf8);
        ketQua = KetQuaDangNhap::THANH_CONG;
        DestroyWindow(hwnd);
        return;
    }

    --soLanConLai;
    if (soLanConLai <= 0) {
        ketQua = KetQuaDangNhap::SAI_QUA_SO_LAN;
        DestroyWindow(hwnd);
        return;
    }

    // Xóa ô mật khẩu TRƯỚC khi đặt thông báo lỗi: việc xóa sinh ra EN_CHANGE, mà EN_CHANGE lại đưa dòng thông báo về trạng thái bình thường.
    DieuKhien::DatChu(oMatKhau, L"");
    thongBao = L"Sai tên đăng nhập hoặc mật khẩu!  Còn " + std::to_wstring(soLanConLai) + L" lần thử.";
    thongBaoLaLoi = true;
    InvalidateRect(hwnd, nullptr, FALSE);
    MessageBeep(MB_ICONHAND);
    RungCuaSo();
    SetFocus(oMatKhau);
}

// [GIẢI THÍCH] Hiệu ứng "lắc đầu" khi nhập sai: dịch cửa sổ trái / phải vài lần.
void CuaSoDangNhap::RungCuaSo() {
    RECT vung;
    GetWindowRect(hwnd, &vung);
    const int dichChuyen[] = { -10, 10, -8, 8, -5, 5, -2, 2, 0 };
    for (int dx : dichChuyen) {
        SetWindowPos(hwnd, nullptr, vung.left + ChuDe::Dp(dx), vung.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        UpdateWindow(hwnd);
        Sleep(22);
    }
}

void CuaSoDangNhap::Ve(HDC hdc, const RECT& vung) const {
    BoVe::ToNen(hdc, vung, ChuDe::THE);

    // Dải màu chuyển sắc phía trên
    const int caoDai = ChuDe::Dp(CAO_DAI_MAU);
    const RECT dai = { 0, 0, vung.right, caoDai };
    BoVe::ToChuyenMau(hdc, dai, MAU_DAI_TREN, MAU_DAI_DUOI, false);

    // Biểu tượng cuốn sách trong ô vuông bo góc màu trắng
    const int coLogo = ChuDe::Dp(68);
    const RECT logo = { (vung.right - coLogo) / 2, ChuDe::Dp(34), (vung.right + coLogo) / 2, ChuDe::Dp(34) + coLogo };
    BoVe::ToBoGoc(hdc, logo, ChuDe::Dp(18), ChuDe::TRANG, ChuDe::TRANG);
    BoVe::VeBieuTuong(hdc, ChuDe::BT_THU_VIEN, logo, LoaiChu::BIEU_TUONG_LON, MAU_DAI_TREN);

    const RECT vungTen = { 0, ChuDe::Dp(114), vung.right, ChuDe::Dp(150) };
    BoVe::VietChu(hdc, L"QUẢN LÝ THƯ VIỆN", vungTen, LoaiChu::TIEU_DE_LON, ChuDe::TRANG, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    const RECT vungPhu = { 0, ChuDe::Dp(150), vung.right, ChuDe::Dp(176) };
    BoVe::VietChu(hdc, L"Project 3  ·  Cấu trúc dữ liệu & Giải thuật", vungPhu, LoaiChu::NHO, ChuDe::TronMau(ChuDe::TRANG, MAU_DAI_DUOI, 0.25), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Phần thân: tiêu đề + nhãn + ô nhập
    const int le = ChuDe::Dp(LE);
    const RECT vungTieuDe = { le, ChuDe::Dp(218), vung.right - le, ChuDe::Dp(250) };
    BoVe::VietChu(hdc, L"ĐĂNG NHẬP HỆ THỐNG", vungTieuDe, LoaiChu::TIEU_DE, ChuDe::CHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    const RECT nhanTen = { le, ChuDe::Dp(268), vung.right - le, ChuDe::Dp(292) };
    BoVe::VietChu(hdc, L"User", nhanTen, LoaiChu::NHO_DAM, ChuDe::CHU);
    const RECT nhanMatKhau = { le, ChuDe::Dp(348), vung.right - le, ChuDe::Dp(372) };
    BoVe::VietChu(hdc, L"Password", nhanMatKhau, LoaiChu::NHO_DAM, ChuDe::CHU);
    VeKhungCacONhap(hdc, ChuDe::THE);

    const RECT vungThongBao = { le, ChuDe::Dp(428), vung.right - le, ChuDe::Dp(454) };
    BoVe::VietChu(hdc, thongBao, vungThongBao, thongBaoLaLoi ? LoaiChu::NHO_DAM : LoaiChu::NHO, thongBaoLaLoi ? ChuDe::DO : ChuDe::CHU_PHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    const RECT vungChan = { le, ChuDe::Dp(528), vung.right - le, ChuDe::Dp(556) };
    BoVe::VietChu(hdc, L"Enter: đăng nhập   ·   Tab: chuyển ô   ·   ESC: thoát", vungChan, LoaiChu::NHO, ChuDe::VIEN_DAM, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Viền mảnh quanh cửa sổ (cửa sổ không có khung của Windows)
    HBRUSH khongTo = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    HPEN but = CreatePen(PS_SOLID, 1, ChuDe::VIEN_DAM);
    HGDIOBJ coCu = SelectObject(hdc, khongTo);
    HGDIOBJ butCu = SelectObject(hdc, but);
    Rectangle(hdc, 0, 0, vung.right, vung.bottom);
    SelectObject(hdc, coCu);
    SelectObject(hdc, butCu);
    DeleteObject(but);
}

LRESULT CuaSoDangNhap::XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) {
    switch (thongDiep) {
    case WM_PAINT: {
        BoDemVe boDem(hwnd);
        Ve(boDem.GetHdc(), boDem.GetVung());
        return 0;
    }

    case WM_COMMAND:
        XuLyTieuDiemONhap(wParam, lParam);
        if (LOWORD(wParam) == IDOK) {
            ThuDangNhap();          // nút Đăng nhập hoặc phím Enter
        } else if (LOWORD(wParam) == IDCANCEL) {
            ketQua = KetQuaDangNhap::THOAT;     // nút ✕ hoặc phím ESC
            DestroyWindow(hwnd);
        } else if (HIWORD(wParam) == EN_CHANGE && thongBaoLaLoi) {
            // Bắt đầu gõ lại -> chuyển dòng báo lỗi về màu thường
            thongBaoLaLoi = false;
            thongBao = L"Còn " + std::to_wstring(soLanConLai) + L" lần thử   ·   Nhấn ESC để thoát";
            InvalidateRect(hwnd, nullptr, FALSE);
        }

        return 0;

    case WM_NCHITTEST: {
        // Giữ chuột ở dải màu phía trên để kéo cửa sổ
        POINT diem = { static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
        ScreenToClient(hwnd, &diem);
        if (diem.y >= 0 && diem.y < ChuDe::Dp(CAO_DAI_MAU)) {
            return HTCAPTION;
        }

        return HTCLIENT;
    }

    case WM_CLOSE:
        ketQua = KetQuaDangNhap::THOAT;
        DestroyWindow(hwnd);
        return 0;

    default:
        break;
    }

    return CuaSoCoSo::XuLyThongDiep(thongDiep, wParam, lParam);
}
