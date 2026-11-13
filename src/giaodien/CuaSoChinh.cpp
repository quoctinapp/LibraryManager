// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của CuaSoChinh (mục lục ở CuaSoChinh.h).
//
//    ┌──────────┬──────────────────────────────────────────┐
//    │  LOGO    │  Tên trang                     Ngày      │ đầu trang cao 72
//    │ ▸ Tổng.. ├──────────────────────────────────────────┤
//    │   Sách   │                                          │
//    │   Phiếu  │          vùng nội dung (1 trang)         │
//    │   Bạn đọc│                                          │
//    │  (user)  │                                          │
//    │  Đ.xuất  │                                          │
//    ├──────────┴──────────────────────────────────────────┤
//    │ thanh trạng thái                                     │
//    └──────────────────────────────────────────────────────┘
// ============================================================================

#include "CuaSoChinh.h"
#include "BoVe.h"
#include "HopThongBao.h"
#include "MaDieuKhien.h"
#include "NgayThang.h"
#include "TrangBanDoc.h"
#include "TrangPhieuMuon.h"
#include "TrangSach.h"
#include "TrangTongQuan.h"
#include <commctrl.h>

static const int RONG_THANH_BEN = 236;
static const int CAO_DAU_TRANG = 72;
static const int Y_MUC_DAU = 128;
static const int CAO_MUC = 46;
static const int KHOANG_MUC = 6;

static const wchar_t* const TEN_THU[] = { L"Chủ nhật", L"Thứ Hai", L"Thứ Ba", L"Thứ Tư",
                                          L"Thứ Năm", L"Thứ Sáu", L"Thứ Bảy" };

CuaSoChinh::CuaSoChinh(QuanLyThuVien& quanLy, const std::filesystem::path& thuMucDuLieu, const std::wstring& nguoiDung) : quanLy(quanLy), thuMucDuLieu(thuMucDuLieu), nguoiDung(nguoiDung), trangHienTai(0), mucDangRe(-1), dangTheoDoiChuot(false), thanhTrangThai(nullptr), bangPhimTat(nullptr), ketQua(KetQuaCuaSoChinh::THOAT) {}

CuaSoChinh::~CuaSoChinh() {
    if (bangPhimTat != nullptr) {
        DestroyAcceleratorTable(bangPhimTat);
    }
}

// ============================================================================
//  GiaoTiepTrang
// ============================================================================

QuanLyThuVien& CuaSoChinh::GetQuanLy() {
    return quanLy;
}

HWND CuaSoChinh::GetCuaSoChinh() const {
    return hwnd;
}

void CuaSoChinh::BaoTrangThai(const std::wstring& thongBao) {
    if (thanhTrangThai != nullptr) {
        SendMessageW(thanhTrangThai, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>((L"  " + thongBao).c_str()));
    }
}

void CuaSoChinh::DuLieuDaThayDoi() {
    for (auto& trang : cacTrang) {
        if (trang) {
            trang->LamMoi();
        }
    }
}

// ============================================================================
//  Tạo cửa sổ và chạy
// ============================================================================

KetQuaCuaSoChinh CuaSoChinh::Chay() {
    // Kích thước mặc định 1360x820 (theo DPI), không vượt quá vùng làm việc của màn hình
    RECT vungLamViec;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &vungLamViec, 0);
    int rong = ChuDe::Dp(1360);
    int cao = ChuDe::Dp(820);
    const int rongToiDa = vungLamViec.right - vungLamViec.left;
    const int caoToiDa = vungLamViec.bottom - vungLamViec.top;
    if (rong > rongToiDa) {
        rong = rongToiDa;
    }

    if (cao > caoToiDa) {
        cao = caoToiDa;
    }

    const int x = vungLamViec.left + (rongToiDa - rong) / 2;
    const int y = vungLamViec.top + (caoToiDa - cao) / 2;

    if (!TaoCuaSo(WS_EX_CONTROLPARENT, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, L"Quản lý thư viện – Project 3 CTDL&GT", x, y, rong, cao, nullptr)) {
        return KetQuaCuaSoChinh::THOAT;
    }

    // Thanh trạng thái: [thông báo][thư mục dữ liệu]
    thanhTrangThai = CreateWindowExW(0, STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, nullptr, GetInstance(), nullptr);
    SendMessageW(thanhTrangThai, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::NHO)), FALSE);

    // 4 trang (đa hình: cùng kiểu con trỏ TrangNoiDung, mỗi trang một lớp con)
    cacTrang[0] = std::make_unique<TrangTongQuan>(*this);
    cacTrang[1] = std::make_unique<TrangSach>(*this);
    cacTrang[2] = std::make_unique<TrangPhieuMuon>(*this);
    cacTrang[3] = std::make_unique<TrangBanDoc>(*this);
    for (auto& trang : cacTrang) {
        trang->Tao(hwnd);
    }

    // Phím tắt
    ACCEL phimTat[] = {
        { FCONTROL | FVIRTKEY, '1', MA_PHIM_TRANG_1 },
        { FCONTROL | FVIRTKEY, '2', MA_PHIM_TRANG_2 },
        { FCONTROL | FVIRTKEY, '3', MA_PHIM_TRANG_3 },
        { FCONTROL | FVIRTKEY, '4', MA_PHIM_TRANG_4 },
        { FCONTROL | FVIRTKEY, 'F', MA_PHIM_TIM_KIEM },
        { FVIRTKEY, VK_F5, MA_PHIM_LAM_MOI },
    };
    bangPhimTat = CreateAcceleratorTableW(phimTat, static_cast<int>(sizeof(phimTat) / sizeof(phimTat[0])));

    BoTri();
    ChuyenTrang(0);
    BaoTrangThai(L"Xin chào " + nguoiDung + L"!   Phím tắt: Ctrl+1..4 chuyển trang  ·  Ctrl+F tìm kiếm  ·  F5 làm mới  ·  Delete xóa dòng đang chọn");

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    ChayVongLapThongDiep();
    return ketQua;
}

bool CuaSoChinh::TienXuLy(MSG& thongDiep) {
    return bangPhimTat != nullptr && hwnd != nullptr && TranslateAcceleratorW(hwnd, bangPhimTat, &thongDiep);
}

// ============================================================================
//  Bố cục, chuyển trang
// ============================================================================

void CuaSoChinh::BoTri() {
    if (hwnd == nullptr || thanhTrangThai == nullptr) {
        return;
    }

    RECT vung;
    GetClientRect(hwnd, &vung);

    SendMessageW(thanhTrangThai, WM_SIZE, 0, 0);
    RECT vungTrangThai;
    GetWindowRect(thanhTrangThai, &vungTrangThai);
    const int caoTrangThai = vungTrangThai.bottom - vungTrangThai.top;
    int phan[2] = { vung.right - ChuDe::Dp(460), -1 };
    SendMessageW(thanhTrangThai, SB_SETPARTS, 2, reinterpret_cast<LPARAM>(phan));
    const std::wstring thuMuc = L"  Dữ liệu: " + thuMucDuLieu.wstring();
    SendMessageW(thanhTrangThai, SB_SETTEXTW, 1, reinterpret_cast<LPARAM>(thuMuc.c_str()));

    const int x = ChuDe::Dp(RONG_THANH_BEN);
    const int y = ChuDe::Dp(CAO_DAU_TRANG);
    for (auto& trang : cacTrang) {
        if (trang) {
            MoveWindow(trang->GetHwnd(), x, y, vung.right - x, vung.bottom - y - caoTrangThai, TRUE);
        }
    }
}

void CuaSoChinh::ChuyenTrang(int chiSo) {
    if (chiSo < 0 || chiSo >= SO_TRANG) {
        return;
    }

    trangHienTai = chiSo;
    for (int i = 0; i < SO_TRANG; ++i) {
        ShowWindow(cacTrang[i]->GetHwnd(), i == chiSo ? SW_SHOW : SW_HIDE);
    }

    cacTrang[chiSo]->LamMoi();
    InvalidateRect(hwnd, nullptr, FALSE);
}

void CuaSoChinh::DangXuat() {
    if (HopThongBao::XacNhan(hwnd, L"Đăng xuất?", L"Bạn sẽ quay lại màn hình đăng nhập.")) {
        ketQua = KetQuaCuaSoChinh::DANG_XUAT;
        DestroyWindow(hwnd);
    }
}

RECT CuaSoChinh::VungMuc(int chiSo) const {
    const int le = ChuDe::Dp(12);
    const int rong = ChuDe::Dp(RONG_THANH_BEN);
    if (chiSo < SO_TRANG) {
        const int y = ChuDe::Dp(Y_MUC_DAU) + chiSo * ChuDe::Dp(CAO_MUC + KHOANG_MUC);
        return { le, y, rong - le, y + ChuDe::Dp(CAO_MUC) };
    }

    // Mục "Đăng xuất" ở đáy thanh bên (phía trên thanh trạng thái)
    RECT vung;
    GetClientRect(hwnd, &vung);
    const int duoi = vung.bottom - ChuDe::Dp(40);
    return { le, duoi - ChuDe::Dp(CAO_MUC), rong - le, duoi };
}

int CuaSoChinh::MucTaiDiem(int x, int y) const {
    const POINT diem = { x, y };
    for (int i = 0; i <= SO_TRANG; ++i) {
        const RECT vung = VungMuc(i);
        if (PtInRect(&vung, diem)) {
            return i;
        }
    }

    return -1;
}

// ============================================================================
//  Vẽ
// ============================================================================

void CuaSoChinh::Ve(HDC hdc, const RECT& vung) const {
    VeThanhBen(hdc, vung);
    VeDauTrang(hdc, vung);
}

void CuaSoChinh::VeThanhBen(HDC hdc, const RECT& vung) const {
    const int rong = ChuDe::Dp(RONG_THANH_BEN);
    const RECT thanhBen = { 0, 0, rong, vung.bottom };
    BoVe::ToNen(hdc, thanhBen, ChuDe::THANH_BEN);

    // Logo
    const int coLogo = ChuDe::Dp(44);
    const RECT logo = { ChuDe::Dp(20), ChuDe::Dp(22), ChuDe::Dp(20) + coLogo, ChuDe::Dp(22) + coLogo };
    BoVe::ToBoGoc(hdc, logo, ChuDe::Dp(12), ChuDe::NHAN, ChuDe::NHAN);
    BoVe::VeBieuTuong(hdc, ChuDe::BT_THU_VIEN, logo, LoaiChu::BIEU_TUONG, ChuDe::TRANG);
    const RECT vungTen = { logo.right + ChuDe::Dp(12), ChuDe::Dp(22), rong - ChuDe::Dp(8), ChuDe::Dp(46) };
    BoVe::VietChu(hdc, L"THƯ VIỆN", vungTen, LoaiChu::TIEU_DE, ChuDe::TRANG);
    const RECT vungPhu = { logo.right + ChuDe::Dp(12), ChuDe::Dp(46), rong - ChuDe::Dp(8), ChuDe::Dp(66) };
    BoVe::VietChu(hdc, L"Quản lý mượn – trả sách", vungPhu, LoaiChu::NHO, ChuDe::THANH_BEN_CHU_MO);

    const RECT nhanMenu = { ChuDe::Dp(24), ChuDe::Dp(96), rong, ChuDe::Dp(118) };
    BoVe::VietChu(hdc, L"MENU", nhanMenu, LoaiChu::NHO_DAM, ChuDe::THANH_BEN_CHU_MO);

    // Các mục điều hướng
    const wchar_t bieuTuong[SO_TRANG] = { ChuDe::BT_TRANG_CHU, ChuDe::BT_THU_VIEN, ChuDe::BT_TAI_LIEU, ChuDe::BT_NGUOI };
    const wchar_t* const ten[SO_TRANG] = { L"Tổng quan", L"Quản lý sách", L"Quản lý phiếu mượn", L"Bạn đọc" };
    for (int i = 0; i < SO_TRANG; ++i) {
        const RECT muc = VungMuc(i);
        const bool dangChon = (i == trangHienTai);
        COLORREF mauChu = ChuDe::THANH_BEN_CHU;
        if (dangChon) {
            BoVe::ToBoGoc(hdc, muc, ChuDe::Dp(10), ChuDe::NHAN, ChuDe::NHAN);
            mauChu = ChuDe::TRANG;
        } else if (i == mucDangRe) {
            BoVe::ToBoGoc(hdc, muc, ChuDe::Dp(10), ChuDe::THANH_BEN_TRO, ChuDe::THANH_BEN_TRO);
            mauChu = ChuDe::TRANG;
        }

        const RECT vungBieuTuong = { muc.left + ChuDe::Dp(10), muc.top, muc.left + ChuDe::Dp(42), muc.bottom };
        BoVe::VeBieuTuong(hdc, bieuTuong[i], vungBieuTuong, LoaiChu::BIEU_TUONG, mauChu);
        const RECT vungChu = { muc.left + ChuDe::Dp(50), muc.top, muc.right - ChuDe::Dp(8), muc.bottom };
        BoVe::VietChu(hdc, ten[i], vungChu, dangChon ? LoaiChu::DAM : LoaiChu::THUONG, mauChu, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }

    // Khối người dùng + mục Đăng xuất
    const RECT dangXuat = VungMuc(SO_TRANG);
    const int yNguoiDung = dangXuat.top - ChuDe::Dp(70);
    BoVe::VeDuong(hdc, ChuDe::Dp(20), yNguoiDung - ChuDe::Dp(10), rong - ChuDe::Dp(20), yNguoiDung - ChuDe::Dp(10), ChuDe::THANH_BEN_TRO);
    const int banKinh = ChuDe::Dp(20);
    const int tamX = ChuDe::Dp(24) + banKinh;
    const int tamY = yNguoiDung + ChuDe::Dp(28);
    BoVe::ToHinhTron(hdc, tamX, tamY, banKinh, ChuDe::TIM);
    const std::wstring chuDau = nguoiDung.empty() ? L"?" : std::wstring(1, static_cast<wchar_t>(towupper(nguoiDung[0])));
    const RECT vungAnh = { tamX - banKinh, tamY - banKinh, tamX + banKinh, tamY + banKinh };
    BoVe::VietChu(hdc, chuDau, vungAnh, LoaiChu::TIEU_DE, ChuDe::TRANG, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    const RECT vungTenNguoi = { tamX + banKinh + ChuDe::Dp(12), tamY - ChuDe::Dp(20), rong - ChuDe::Dp(12), tamY };
    BoVe::VietChu(hdc, nguoiDung, vungTenNguoi, LoaiChu::DAM, ChuDe::TRANG, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_END_ELLIPSIS);
    const RECT vungVaiTro = { tamX + banKinh + ChuDe::Dp(12), tamY, rong - ChuDe::Dp(12), tamY + ChuDe::Dp(20) };
    BoVe::VietChu(hdc, L"Nhân viên thư viện", vungVaiTro, LoaiChu::NHO, ChuDe::THANH_BEN_CHU_MO, DT_LEFT | DT_TOP | DT_SINGLELINE);

    COLORREF mauDangXuat = ChuDe::THANH_BEN_CHU;
    if (mucDangRe == SO_TRANG) {
        BoVe::ToBoGoc(hdc, dangXuat, ChuDe::Dp(10), ChuDe::DO_DAM, ChuDe::DO_DAM);
        mauDangXuat = ChuDe::TRANG;
    }

    const RECT vungBieuTuongDX = { dangXuat.left + ChuDe::Dp(10), dangXuat.top, dangXuat.left + ChuDe::Dp(42), dangXuat.bottom };
    BoVe::VeBieuTuong(hdc, ChuDe::BT_THOAT, vungBieuTuongDX, LoaiChu::BIEU_TUONG, mauDangXuat);
    const RECT vungChuDX = { dangXuat.left + ChuDe::Dp(50), dangXuat.top, dangXuat.right, dangXuat.bottom };
    BoVe::VietChu(hdc, L"Đăng xuất", vungChuDX, LoaiChu::THUONG, mauDangXuat);
}

void CuaSoChinh::VeDauTrang(HDC hdc, const RECT& vung) const {
    const int x = ChuDe::Dp(RONG_THANH_BEN);
    const int cao = ChuDe::Dp(CAO_DAU_TRANG);
    const RECT dau = { x, 0, vung.right, cao };
    BoVe::ToNen(hdc, dau, ChuDe::THE);
    BoVe::VeDuong(hdc, x, cao - 1, vung.right, cao - 1, ChuDe::VIEN);

    const TrangNoiDung& trang = *cacTrang[trangHienTai];
    const RECT vungTieuDe = { x + ChuDe::Dp(28), ChuDe::Dp(10), vung.right - ChuDe::Dp(300), ChuDe::Dp(42) };
    BoVe::VietChu(hdc, trang.GetTieuDe(), vungTieuDe, LoaiChu::TIEU_DE, ChuDe::CHU);
    const RECT vungMoTa = { x + ChuDe::Dp(28), ChuDe::Dp(40), vung.right - ChuDe::Dp(300), ChuDe::Dp(62) };
    BoVe::VietChu(hdc, trang.GetMoTa(), vungMoTa, LoaiChu::NHO, ChuDe::CHU_PHU);

    // Ô ngày hôm nay ở góc phải
    const NgayThang homNay = NgayThang::HomNay();
    const std::wstring ngay = std::wstring(TEN_THU[homNay.ThuTrongTuan()]) + L", " + Utf8SangUtf16(homNay.ChuyenThanhChuoi());
    const SIZE coChu = BoVe::DoChu(hdc, ngay, LoaiChu::DAM);
    const int rongO = coChu.cx + ChuDe::Dp(56);
    const RECT oNgay = { vung.right - ChuDe::Dp(28) - rongO, ChuDe::Dp(16), vung.right - ChuDe::Dp(28),
                         ChuDe::Dp(56) };
    BoVe::ToBoGoc(hdc, oNgay, ChuDe::Dp(20), ChuDe::NEN, ChuDe::VIEN);
    const RECT vungLich = { oNgay.left + ChuDe::Dp(10), oNgay.top, oNgay.left + ChuDe::Dp(38), oNgay.bottom };
    BoVe::VeBieuTuong(hdc, ChuDe::BT_LICH, vungLich, LoaiChu::BIEU_TUONG_NHO, ChuDe::NHAN);
    const RECT vungNgay = { oNgay.left + ChuDe::Dp(40), oNgay.top, oNgay.right - ChuDe::Dp(12), oNgay.bottom };
    BoVe::VietChu(hdc, ngay, vungNgay, LoaiChu::DAM, ChuDe::CHU);
}

// ============================================================================
//  Thông điệp
// ============================================================================

LRESULT CuaSoChinh::XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) {
    switch (thongDiep) {
    case WM_SIZE:
        BoTri();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO* thongTin = reinterpret_cast<MINMAXINFO*>(lParam);
        thongTin->ptMinTrackSize.x = ChuDe::Dp(1120);
        thongTin->ptMinTrackSize.y = ChuDe::Dp(700);
        return 0;
    }

    case WM_PAINT: {
        BoDemVe boDem(hwnd);
        BoVe::ToNen(boDem.GetHdc(), boDem.GetVung(), ChuDe::NEN);
        if (cacTrang[0]) {
            Ve(boDem.GetHdc(), boDem.GetVung());
        }

        return 0;
    }

    case WM_MOUSEMOVE: {
        if (!dangTheoDoiChuot) {
            TRACKMOUSEEVENT theoDoi = { sizeof(theoDoi), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&theoDoi);
            dangTheoDoiChuot = true;
        }

        const int muc = MucTaiDiem(static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)));
        if (muc != mucDangRe) {
            mucDangRe = muc;
            RECT thanhBen = { 0, 0, ChuDe::Dp(RONG_THANH_BEN), 0 };
            RECT vung;
            GetClientRect(hwnd, &vung);
            thanhBen.bottom = vung.bottom;
            InvalidateRect(hwnd, &thanhBen, FALSE);
        }

        return 0;
    }

    case WM_MOUSELEAVE:
        dangTheoDoiChuot = false;
        if (mucDangRe != -1) {
            mucDangRe = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
        }

        return 0;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            POINT diem;
            GetCursorPos(&diem);
            ScreenToClient(hwnd, &diem);
            if (MucTaiDiem(diem.x, diem.y) >= 0) {
                SetCursor(LoadCursorW(nullptr, IDC_HAND));
                return TRUE;
            }
        }
        break;

    case WM_LBUTTONDOWN: {
        const int muc = MucTaiDiem(static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)));
        if (muc >= 0 && muc < SO_TRANG) {
            ChuyenTrang(muc);
        } else if (muc == SO_TRANG) {
            DangXuat();
        }

        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case MA_PHIM_TRANG_1:
        case MA_PHIM_TRANG_2:
        case MA_PHIM_TRANG_3:
        case MA_PHIM_TRANG_4:
            ChuyenTrang(LOWORD(wParam) - MA_PHIM_TRANG_1);
            return 0;
        case MA_PHIM_LAM_MOI:
            DuLieuDaThayDoi();
            BaoTrangThai(L"Đã làm mới dữ liệu.");
            return 0;
        case MA_PHIM_TIM_KIEM:
            cacTrang[trangHienTai]->DatTieuDiemTimKiem();
            return 0;
        default:
            return 0;   // IDOK / IDCANCEL do IsDialogMessage gửi: bỏ qua
        }

    case WM_CLOSE:
        // Đề bài: người dùng chọn kết thúc chương trình -> hỏi lại cho chắc
        if (HopThongBao::XacNhan(hwnd, L"Thoát chương trình?", L"Mọi thay đổi đã được lưu vào file.")) {
            ketQua = KetQuaCuaSoChinh::THOAT;
            DestroyWindow(hwnd);
        }

        return 0;

    default:
        break;
    }

    return CuaSoCoSo::XuLyThongDiep(thongDiep, wParam, lParam);
}
