// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp TrangNoiDung (mục lục ở TrangNoiDung.h).
// ============================================================================

#include "TrangNoiDung.h"
#include "BoVe.h"
#include "DieuKhien.h"
#include "MaDieuKhien.h"
#include "NgayThang.h"
#include <commdlg.h>
#include <shellapi.h>

TrangNoiDung::TrangNoiDung(GiaoTiepTrang& giaoTiep) : giaoTiep(giaoTiep), oTimKiem(nullptr), vungTomTat{}, vungTheBang{} {}

bool TrangNoiDung::Tao(HWND cha) {
    // WS_EX_CONTROLPARENT: phím Tab đi vào được các ô / nút bên trong trang
    if (!TaoCuaSo(WS_EX_CONTROLPARENT, WS_CHILD | WS_CLIPCHILDREN, L"", 0, 0, 10, 10, cha)) {
        return false;
    }

    TaoDieuKhien();
    LamMoi();
    return true;
}

void TrangNoiDung::DatTieuDiemTimKiem() {
    if (oTimKiem != nullptr) {
        SetFocus(oTimKiem);
        SendMessageW(oTimKiem, EM_SETSEL, 0, -1);
    }
}

void TrangNoiDung::TaoOTimKiem(const std::wstring& goiY) {
    oTimKiem = DieuKhien::TaoONhap(hwnd, MA_TIM_KIEM, 0, 50, ChuDe::BT_TIM);
    SendMessageW(oTimKiem, EM_SETCUEBANNER, TRUE, reinterpret_cast<LPARAM>(goiY.c_str()));
    dsONhap.push_back(oTimKiem);
}

int TrangNoiDung::DoRongNut(HWND nut) {
    HDC hdc = GetDC(nut);
    const SIZE coChu = BoVe::DoChu(hdc, DieuKhien::LayChu(nut), LoaiChu::DAM);
    ReleaseDC(nut, hdc);
    return coChu.cx + ChuDe::Dp(18 + 8 + 36);
}

void TrangNoiDung::BoTriChuan(int rong, int cao, const std::vector<HWND>& cacNut, HWND hopLoc) {
    const int le = ChuDe::Dp(24);
    const int yCongCu = ChuDe::Dp(20);
    const int caoCongCu = ChuDe::Dp(38);
    const int khoang = ChuDe::Dp(10);

    // Các nút xếp từ phải sang trái (nút cuối danh sách nằm sát lề phải)
    int x = rong - le;
    for (auto it = cacNut.rbegin(); it != cacNut.rend(); ++it) {
        const int rongNut = DoRongNut(*it);
        x -= rongNut;
        MoveWindow(*it, x, yCongCu, rongNut, caoCongCu, TRUE);
        x -= khoang;
    }

    // Ô tìm kiếm (và hộp lọc) chiếm phần bên trái còn lại
    const int rongHopLoc = hopLoc != nullptr ? ChuDe::Dp(170) : 0;
    int rongTim = x - le - (hopLoc != nullptr ? rongHopLoc + khoang : 0);
    if (rongTim > ChuDe::Dp(380)) {
        rongTim = ChuDe::Dp(380);
    }

    if (rongTim < ChuDe::Dp(160)) {
        rongTim = ChuDe::Dp(160);
    }

    if (oTimKiem != nullptr) {
        DieuKhien::DatViTriONhap(oTimKiem, le, yCongCu, rongTim, caoCongCu);
    }

    if (hopLoc != nullptr) {
        MoveWindow(hopLoc, le + rongTim + khoang, yCongCu + 1, rongHopLoc, ChuDe::Dp(300), TRUE);
    }

    // Dòng tóm tắt + khung bảng
    const int yTomTat = yCongCu + caoCongCu + ChuDe::Dp(12);
    vungTomTat = { le, yTomTat, rong - le, yTomTat + ChuDe::Dp(24) };
    vungTheBang = { le, vungTomTat.bottom + ChuDe::Dp(8), rong - le, cao - ChuDe::Dp(20) };
    const int dem = ChuDe::Dp(6);
    bang.DatViTri(vungTheBang.left + dem, vungTheBang.top + dem, vungTheBang.right - vungTheBang.left - 2 * dem, vungTheBang.bottom - vungTheBang.top - 2 * dem);
}

void TrangNoiDung::DatTomTat(const std::wstring& chu) {
    chuTomTat = chu;
    InvalidateRect(hwnd, &vungTomTat, FALSE);
}

void TrangNoiDung::Ve(HDC hdc, const RECT&) {
    BoVe::VietChu(hdc, chuTomTat, vungTomTat, LoaiChu::NHO, ChuDe::CHU_PHU);
    if (vungTheBang.right > vungTheBang.left) {
        BoVe::ToBoGoc(hdc, vungTheBang, ChuDe::Dp(10), ChuDe::THE, ChuDe::VIEN);
    }
}

void TrangNoiDung::XuLyLenh(int, int) {}

bool TrangNoiDung::XuLyThongBaoKhac(const NMHDR*, LRESULT&) {
    return false;
}

LRESULT TrangNoiDung::XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) {
    switch (thongDiep) {
    case WM_SIZE:
        BoTri(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_PAINT: {
        BoDemVe boDem(hwnd);
        BoVe::ToNen(boDem.GetHdc(), boDem.GetVung(), ChuDe::NEN);
        VeKhungCacONhap(boDem.GetHdc(), ChuDe::THE);
        Ve(boDem.GetHdc(), boDem.GetVung());
        return 0;
    }

    case WM_COMMAND:
        XuLyTieuDiemONhap(wParam, lParam);
        XuLyLenh(LOWORD(wParam), HIWORD(wParam));
        return 0;

    case WM_NOTIFY: {
        const NMHDR* thongBao = reinterpret_cast<const NMHDR*>(lParam);
        LRESULT ketQua = 0;
        bool daBamCot = false;
        if (bang.XuLyThongBao(thongBao, ketQua, daBamCot)) {
            if (daBamCot) {
                LamMoi();   // sắp xếp lại theo cột vừa bấm
            }

            return ketQua;
        }

        if (XuLyThongBaoKhac(thongBao, ketQua)) {
            return ketQua;
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
        // Nền của ô nhập chỉ đọc / bị khóa
        SetBkColor(reinterpret_cast<HDC>(wParam), ChuDe::THE);
        return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));

    default:
        break;
    }

    return CuaSoCoSo::XuLyThongDiep(thongDiep, wParam, lParam);
}

std::filesystem::path TrangNoiDung::HoiNoiLuuCsv(const std::wstring& tenMacDinh) const {
    const NgayThang homNay = NgayThang::HomNay();
    wchar_t ngay[16];
    swprintf(ngay, 16, L"_%02d-%02d-%04d", homNay.GetNgay(), homNay.GetThang(), homNay.GetNam());

    wchar_t tenFile[MAX_PATH * 2] = L"";
    lstrcpynW(tenFile, (tenMacDinh + ngay + L".csv").c_str(), MAX_PATH * 2);

    OPENFILENAMEW hop{};
    hop.lStructSize = sizeof(hop);
    hop.hwndOwner = giaoTiep.GetCuaSoChinh();
    hop.lpstrFilter = L"Bảng tính CSV (*.csv)\0*.csv\0Tất cả các file\0*.*\0";
    hop.lpstrFile = tenFile;
    hop.nMaxFile = MAX_PATH * 2;
    hop.lpstrDefExt = L"csv";
    hop.lpstrTitle = L"Xuất báo cáo CSV";
    hop.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&hop)) {
        return std::filesystem::path();
    }

    return std::filesystem::path(tenFile);
}

void TrangNoiDung::HoanTatXuatCsv(const std::filesystem::path& duongDan, std::size_t soDong) {
    giaoTiep.BaoTrangThai(L"Đã xuất " + std::to_wstring(soDong) + L" dòng ra file " + duongDan.filename().wstring());
    if (HopThongBao::XacNhan(giaoTiep.GetCuaSoChinh(), L"Xuất báo cáo thành công", L"Đã ghi " + std::to_wstring(soDong) + L" dòng vào:\n" + duongDan.wstring() + L"\n\nBạn có muốn mở file ngay bây giờ không?")) {
        ShellExecuteW(giaoTiep.GetCuaSoChinh(), L"open", duongDan.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }
}
