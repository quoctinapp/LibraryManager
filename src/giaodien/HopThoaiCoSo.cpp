// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp HopThoaiCoSo (mục lục ở HopThoaiCoSo.h).
//  Bố cục (đơn vị đã nhân DPI):
//     ┌───────────────────────────────────────┐
//     │ (biểu tượng)  Tiêu đề                 │  phần đầu cao 84
//     │               Mô tả ngắn              │
//     ├───────────────────────────────────────┤
//     │ Nhãn trường                           │  mỗi hàng: nhãn 22 + ô 38 + khoảng 14
//     │ [ ô nhập ............................]│
//     │ Nhãn trái           Nhãn phải         │
//     │ [ ô trái ]          [ ô phải ]        │
//     ├───────────────────────────────────────┤
//     │                       [ Hủy ][Đồng ý] │  chân cao 72
//     └───────────────────────────────────────┘
// ============================================================================

#include "HopThoaiCoSo.h"
#include "BoVe.h"
#include "DieuKhien.h"
#include "HopThongBao.h"
#include "MaDieuKhien.h"

#include <commctrl.h>

static const int RONG_HOP_THOAI = 580;
static const int CAO_PHAN_DAU = 84;
static const int CAO_PHAN_CHAN = 72;
static const int LE = 28;
static const int CAO_NHAN = 24;
static const int CAO_O = 38;
static const int KHOANG_HANG = 14;
static const int KHOANG_COT = 16;

HopThoaiCoSo::HopThoaiCoSo(GiaoTiepTrang& giaoTiep, const std::wstring& tieuDe,
                           const std::wstring& moTa, wchar_t bieuTuong,
                           const std::wstring& chuNutDongY)
    : cuaSoChu(nullptr), tieuDe(tieuDe), moTa(moTa), bieuTuong(bieuTuong),
      chuNutDongY(chuNutDongY), daDongY(false), rongNoiDung(0), yHienTai(0), giaoTiep(giaoTiep)
{
}

// ============================================================================
//  Bố cục các trường
// ============================================================================

// [GIẢI THÍCH] Trả về vị trí cho điều khiển, đồng thời ghi lại nhãn cần vẽ và
// dịch con trỏ "yHienTai" xuống hàng tiếp theo khi hàng đã đầy.
RECT HopThoaiCoSo::CapViTri(const std::wstring& nhan, ViTriTruong viTri, int caoDieuKhien)
{
    const int le = ChuDe::Dp(LE);
    const int khoangCot = ChuDe::Dp(KHOANG_COT);
    const int rongNua = (rongNoiDung - 2 * le - khoangCot) / 2;

    int x = le;
    int rong = rongNoiDung - 2 * le;
    if (viTri == ViTriTruong::TRAI) {
        rong = rongNua;
    } else if (viTri == ViTriTruong::PHAI) {
        x = le + rongNua + khoangCot;
        rong = rongNua;
    }

    const int caoNhan = ChuDe::Dp(CAO_NHAN);
    dsNhan.push_back({ nhan, { x, yHienTai, x + rong, yHienTai + caoNhan } });
    const RECT vung = { x, yHienTai + caoNhan, x + rong, yHienTai + caoNhan + caoDieuKhien };

    if (viTri != ViTriTruong::TRAI) {
        yHienTai += caoNhan + caoDieuKhien + ChuDe::Dp(KHOANG_HANG);
    }
    return vung;
}

HWND HopThoaiCoSo::ThemONhap(const std::wstring& nhan, int id, DWORD kieuThem, int doDaiToiDa,
                             ViTriTruong viTri)
{
    const RECT vung = CapViTri(nhan, viTri, ChuDe::Dp(CAO_O));
    HWND oNhap = DieuKhien::TaoONhap(hwnd, id, kieuThem, doDaiToiDa);
    DieuKhien::DatViTriONhap(oNhap, vung.left, vung.top, vung.right - vung.left, vung.bottom - vung.top);
    dsONhap.push_back(oNhap);
    return oNhap;
}

HWND HopThoaiCoSo::ThemHopChon(const std::wstring& nhan, int id, bool choPhepGo, ViTriTruong viTri)
{
    const RECT vung = CapViTri(nhan, viTri, ChuDe::Dp(CAO_O));
    HWND hop = DieuKhien::TaoHopChon(hwnd, id, choPhepGo);
    MoveWindow(hop, vung.left, vung.top, vung.right - vung.left, ChuDe::Dp(320), TRUE);
    return hop;
}

HWND HopThoaiCoSo::ThemChonNgay(const std::wstring& nhan, int id, const NgayThang& macDinh,
                                ViTriTruong viTri)
{
    const RECT vung = CapViTri(nhan, viTri, ChuDe::Dp(CAO_O));
    HWND chonNgay = DieuKhien::TaoChonNgay(hwnd, id);
    MoveWindow(chonNgay, vung.left, vung.top, vung.right - vung.left, vung.bottom - vung.top, TRUE);

    SYSTEMTIME ngay{};
    ngay.wDay = static_cast<WORD>(macDinh.GetNgay());
    ngay.wMonth = static_cast<WORD>(macDinh.GetThang());
    ngay.wYear = static_cast<WORD>(macDinh.GetNam());
    DateTime_SetSystemtime(chonNgay, GDT_VALID, &ngay);

    // Giới hạn chọn ngày từ 01/01/1900 tới hôm nay (không chọn được ngày tương lai)
    const NgayThang homNay = NgayThang::HomNay();
    SYSTEMTIME gioiHan[2] = {};
    gioiHan[0].wDay = 1;
    gioiHan[0].wMonth = 1;
    gioiHan[0].wYear = static_cast<WORD>(CauHinh::NAM_TOI_THIEU);
    gioiHan[1].wDay = static_cast<WORD>(homNay.GetNgay());
    gioiHan[1].wMonth = static_cast<WORD>(homNay.GetThang());
    gioiHan[1].wYear = static_cast<WORD>(homNay.GetNam());
    DateTime_SetRange(chonNgay, GDTR_MIN | GDTR_MAX, gioiHan);
    return chonNgay;
}

int HopThoaiCoSo::ThemOThongTin(const std::wstring& nhan, const std::wstring& giaTri, COLORREF mau,
                                ViTriTruong viTri)
{
    const RECT vung = CapViTri(nhan, viTri, ChuDe::Dp(CAO_O));
    dsOThongTin.push_back({ giaTri, vung, mau });
    return static_cast<int>(dsOThongTin.size()) - 1;
}

RECT HopThoaiCoSo::ThemVungTuVe(int chieuCao)
{
    const int le = ChuDe::Dp(LE);
    const RECT vung = { le, yHienTai, rongNoiDung - le, yHienTai + chieuCao };
    yHienTai += chieuCao + ChuDe::Dp(KHOANG_HANG);
    return vung;
}

void HopThoaiCoSo::DatOThongTin(int chiSo, const std::wstring& giaTri, COLORREF mau)
{
    if (chiSo < 0 || chiSo >= static_cast<int>(dsOThongTin.size())) {
        return;
    }
    dsOThongTin[static_cast<std::size_t>(chiSo)].chu = giaTri;
    dsOThongTin[static_cast<std::size_t>(chiSo)].mau = mau;
    InvalidateRect(hwnd, &dsOThongTin[static_cast<std::size_t>(chiSo)].vung, FALSE);
}

// ============================================================================
//  Đọc giá trị
// ============================================================================

std::string HopThoaiCoSo::LayChuUtf8(HWND dieuKhien)
{
    return CatKhoangTrang(Utf16SangUtf8(DieuKhien::LayChu(dieuKhien)));
}

NgayThang HopThoaiCoSo::LayNgay(HWND chonNgay)
{
    SYSTEMTIME ngay{};
    DateTime_GetSystemtime(chonNgay, &ngay);
    return NgayThang(ngay.wDay, ngay.wMonth, ngay.wYear);
}

std::string HopThoaiCoSo::LayMaDauChuoi(HWND hopChon)
{
    const std::string chuoi = LayChuUtf8(hopChon);
    const std::size_t viTri = chuoi.find(' ');
    return VietHoa(viTri == std::string::npos ? chuoi : chuoi.substr(0, viTri));
}

// ============================================================================
//  Chạy hộp thoại
// ============================================================================

bool HopThoaiCoSo::ChayModal()
{
    cuaSoChu = giaoTiep.GetCuaSoChinh();
    rongNoiDung = ChuDe::Dp(RONG_HOP_THOAI);

    const DWORD kieu = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN;
    const DWORD kieuMoRong = WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT;
    if (!TaoCuaSo(kieuMoRong, kieu, tieuDe, 0, 0, rongNoiDung, ChuDe::Dp(400), cuaSoChu)) {
        return false;
    }

    // 1) Lớp con thêm các trường
    yHienTai = ChuDe::Dp(CAO_PHAN_DAU) + ChuDe::Dp(20);
    TaoNoiDung();
    const int caoNoiDung = yHienTai + ChuDe::Dp(6) + ChuDe::Dp(CAO_PHAN_CHAN);

    // 2) Nút Hủy / Đồng ý ở chân hộp thoại
    const int le = ChuDe::Dp(LE);
    const int caoNut = ChuDe::Dp(40);
    const int yNut = caoNoiDung - (ChuDe::Dp(CAO_PHAN_CHAN) + caoNut) / 2;
    const int rongNutDongY = ChuDe::Dp(150);
    const int rongNutHuy = ChuDe::Dp(100);
    HWND nutDongY = DieuKhien::TaoNut(hwnd, IDOK, chuNutDongY, ChuDe::BT_DONG_Y, KieuNut::CHINH,
                                      ChuDe::DONG_LE);
    HWND nutHuy = DieuKhien::TaoNut(hwnd, IDCANCEL, L"Hủy", 0, KieuNut::PHU, ChuDe::DONG_LE);
    MoveWindow(nutDongY, rongNoiDung - le - rongNutDongY, yNut, rongNutDongY, caoNut, FALSE);
    MoveWindow(nutHuy, rongNoiDung - le - rongNutDongY - ChuDe::Dp(10) - rongNutHuy, yNut,
               rongNutHuy, caoNut, FALSE);

    // 3) Đổi kích thước cửa sổ cho vừa nội dung và đặt giữa cửa sổ chính
    RECT khung = { 0, 0, rongNoiDung, caoNoiDung };
    AdjustWindowRectEx(&khung, kieu, FALSE, kieuMoRong);
    const int rongCuaSo = khung.right - khung.left;
    const int caoCuaSo = khung.bottom - khung.top;
    RECT vungChu;
    GetWindowRect(cuaSoChu, &vungChu);
    SetWindowPos(hwnd, nullptr, vungChu.left + (vungChu.right - vungChu.left - rongCuaSo) / 2,
                 vungChu.top + (vungChu.bottom - vungChu.top - caoCuaSo) / 2, rongCuaSo, caoCuaSo,
                 SWP_NOZORDER);

    // 4) Khóa cửa sổ chính, hiện hộp thoại, chờ tới khi đóng
    EnableWindow(cuaSoChu, FALSE);
    ShowWindow(hwnd, SW_SHOW);
    HWND oDau = GetNextDlgTabItem(hwnd, nullptr, FALSE);
    if (oDau != nullptr) {
        SetFocus(oDau);
    }
    ChayVongLapThongDiep();
    return daDongY;
}

void HopThoaiCoSo::Dong()
{
    // Mở khóa cửa sổ chính TRƯỚC khi hủy hộp thoại để Windows trả tiêu điểm về đúng chỗ
    EnableWindow(cuaSoChu, TRUE);
    SetActiveWindow(cuaSoChu);
    if (hwnd != nullptr) {
        DestroyWindow(hwnd);
    }
}

void HopThoaiCoSo::DongY()
{
    try {
        if (ThucHien()) {
            daDongY = true;
            Dong();
        }
    } catch (const LoiChuongTrinh& loi) {
        HopThongBao::Loi(hwnd, tieuDe + L" không thành công",
                         Utf8SangUtf16(loi.what()) + L"\n\nChức năng đã bị hủy.");
        giaoTiep.BaoTrangThai(L"Lỗi: " + Utf8SangUtf16(loi.what()));
        Dong();
    } catch (const std::exception& loi) {
        HopThongBao::Loi(hwnd, L"Lỗi không xác định", Utf8SangUtf16(loi.what()));
        Dong();
    }
}

void HopThoaiCoSo::XuLyLenhThem(int, int)
{
}

void HopThoaiCoSo::VeThem(HDC)
{
}

LRESULT HopThoaiCoSo::XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam)
{
    switch (thongDiep) {
    case WM_COMMAND:
        XuLyTieuDiemONhap(wParam, lParam);
        if (LOWORD(wParam) == IDOK) {
            DongY();
        } else if (LOWORD(wParam) == IDCANCEL) {
            Dong();
        } else {
            XuLyLenhThem(LOWORD(wParam), HIWORD(wParam));
        }
        return 0;

    case WM_CLOSE:
        Dong();
        return 0;

    case WM_CTLCOLORSTATIC:
        SetBkColor(reinterpret_cast<HDC>(wParam), ChuDe::THE);
        return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));

    case WM_PAINT: {
        BoDemVe boDem(hwnd);
        HDC hdc = boDem.GetHdc();
        const RECT& vung = boDem.GetVung();
        BoVe::ToNen(hdc, vung, ChuDe::THE);

        // Phần đầu: biểu tượng tròn + tiêu đề + mô tả
        const int caoDau = ChuDe::Dp(CAO_PHAN_DAU);
        const RECT vungDau = { 0, 0, vung.right, caoDau };
        BoVe::ToChuyenMau(hdc, vungDau, ChuDe::NHAN_NHAT, ChuDe::THE, true);
        BoVe::VeDuong(hdc, 0, caoDau - 1, vung.right, caoDau - 1, ChuDe::VIEN);
        const int le = ChuDe::Dp(LE);
        const int banKinh = ChuDe::Dp(22);
        BoVe::ToHinhTron(hdc, le + banKinh, caoDau / 2, banKinh, ChuDe::NHAN);
        const RECT vungBieuTuong = { le, caoDau / 2 - banKinh, le + 2 * banKinh, caoDau / 2 + banKinh };
        BoVe::VeBieuTuong(hdc, bieuTuong, vungBieuTuong, LoaiChu::BIEU_TUONG, ChuDe::TRANG);
        const int xChu = le + 2 * banKinh + ChuDe::Dp(16);
        const RECT vungTieuDe = { xChu, ChuDe::Dp(14), vung.right - le, ChuDe::Dp(46) };
        BoVe::VietChu(hdc, tieuDe, vungTieuDe, LoaiChu::TIEU_DE, ChuDe::CHU);
        const RECT vungMoTa = { xChu, ChuDe::Dp(46), vung.right - le, ChuDe::Dp(70) };
        BoVe::VietChu(hdc, moTa, vungMoTa, LoaiChu::NHO, ChuDe::CHU_PHU,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

        // Nhãn, ô nhập, ô thông tin
        for (const Nhan& nhan : dsNhan) {
            BoVe::VietChu(hdc, nhan.chu, nhan.vung, LoaiChu::NHO_DAM, ChuDe::CHU,
                          DT_LEFT | DT_TOP | DT_SINGLELINE);
        }
        VeKhungCacONhap(hdc, ChuDe::THE);
        for (const OThongTin& o : dsOThongTin) {
            BoVe::ToBoGoc(hdc, o.vung, ChuDe::Dp(6), ChuDe::DONG_LE, ChuDe::VIEN);
            RECT vungChu = o.vung;
            vungChu.left += ChuDe::Dp(12);
            vungChu.right -= ChuDe::Dp(8);
            BoVe::VietChu(hdc, o.chu, vungChu, LoaiChu::DAM, o.mau,
                          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }
        VeThem(hdc);

        // Phần chân
        const int caoChan = ChuDe::Dp(CAO_PHAN_CHAN);
        const RECT vungChan = { 0, vung.bottom - caoChan, vung.right, vung.bottom };
        BoVe::ToNen(hdc, vungChan, ChuDe::DONG_LE);
        BoVe::VeDuong(hdc, 0, vungChan.top, vung.right, vungChan.top, ChuDe::VIEN);
        return 0;
    }

    default:
        break;
    }
    return CuaSoCoSo::XuLyThongDiep(thongDiep, wParam, lParam);
}
