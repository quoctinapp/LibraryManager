// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp CuaSoCoSo (mục lục ở CuaSoCoSo.h).
// ============================================================================

#include "CuaSoCoSo.h"
#include "BoVe.h"
#include "DieuKhien.h"

HINSTANCE CuaSoCoSo::hInstance = nullptr;
const wchar_t* CuaSoCoSo::TEN_LOP = L"QuanLyThuVien.CuaSo";
const wchar_t* CuaSoCoSo::TEN_LOP_CO_BONG = L"QuanLyThuVien.CuaSoCoBong";

// [GIẢI THÍCH] Đăng ký 2 "loại cửa sổ" với Windows: loại thường và loại có
// bóng đổ (CS_DROPSHADOW, dùng cho màn hình đăng nhập không viền).
void CuaSoCoSo::DangKyLop(HINSTANCE hInst)
{
    hInstance = hInst;

    WNDCLASSEXW lop{};
    lop.cbSize = sizeof(lop);
    lop.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    lop.lpfnWndProc = ThuTucChung;
    lop.hInstance = hInst;
    lop.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    lop.hbrBackground = nullptr;    // tự vẽ nền trong WM_PAINT
    lop.hIcon = LoadIconW(hInst, L"IDI_UNG_DUNG");
    lop.hIconSm = static_cast<HICON>(LoadImageW(hInst, L"IDI_UNG_DUNG", IMAGE_ICON,
                                                GetSystemMetrics(SM_CXSMICON),
                                                GetSystemMetrics(SM_CYSMICON), 0));
    lop.lpszClassName = TEN_LOP;
    RegisterClassExW(&lop);

    lop.style |= CS_DROPSHADOW;
    lop.lpszClassName = TEN_LOP_CO_BONG;
    RegisterClassExW(&lop);
}

HINSTANCE CuaSoCoSo::GetInstance()
{
    return hInstance;
}

CuaSoCoSo::CuaSoCoSo()
    : hwnd(nullptr), daDong(false)
{
}

// [GIẢI THÍCH] Đối tượng bị hủy trước cửa sổ -> gỡ liên kết rồi đóng cửa sổ,
// để Windows không gọi vào một đối tượng không còn tồn tại.
CuaSoCoSo::~CuaSoCoSo()
{
    if (hwnd != nullptr && IsWindow(hwnd)) {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        DestroyWindow(hwnd);
    }
}

bool CuaSoCoSo::TaoCuaSo(DWORD kieuMoRong, DWORD kieu, const std::wstring& tieuDe, int x, int y,
                         int rong, int cao, HWND cha, bool coBong)
{
    // Truyền con trỏ "this" qua tham số cuối; ThuTucChung nhận lại ở WM_NCCREATE
    CreateWindowExW(kieuMoRong, coBong ? TEN_LOP_CO_BONG : TEN_LOP, tieuDe.c_str(), kieu,
                    x, y, rong, cao, cha, nullptr, hInstance, this);
    return hwnd != nullptr;
}

LRESULT CALLBACK CuaSoCoSo::ThuTucChung(HWND hwndCuaSo, UINT thongDiep, WPARAM wParam, LPARAM lParam)
{
    CuaSoCoSo* doiTuong = nullptr;
    if (thongDiep == WM_NCCREATE) {
        const CREATESTRUCTW* thongTinTao = reinterpret_cast<CREATESTRUCTW*>(lParam);
        doiTuong = static_cast<CuaSoCoSo*>(thongTinTao->lpCreateParams);
        SetWindowLongPtrW(hwndCuaSo, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(doiTuong));
        doiTuong->hwnd = hwndCuaSo;
    } else {
        doiTuong = reinterpret_cast<CuaSoCoSo*>(GetWindowLongPtrW(hwndCuaSo, GWLP_USERDATA));
    }

    if (doiTuong == nullptr) {
        return DefWindowProcW(hwndCuaSo, thongDiep, wParam, lParam);
    }

    const LRESULT ketQua = doiTuong->XuLyThongDiep(thongDiep, wParam, lParam);
    if (thongDiep == WM_NCDESTROY) {
        // Thông điệp cuối cùng của cửa sổ: đánh dấu đã đóng
        SetWindowLongPtrW(hwndCuaSo, GWLP_USERDATA, 0);
        doiTuong->hwnd = nullptr;
        doiTuong->daDong = true;
    }
    return ketQua;
}

LRESULT CuaSoCoSo::XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam)
{
    switch (thongDiep) {
    case WM_ERASEBKGND:
        return 1;   // không xóa nền: WM_PAINT sẽ vẽ đè toàn bộ (chống nháy)
    case WM_DRAWITEM: {
        const DRAWITEMSTRUCT* thongTin = reinterpret_cast<const DRAWITEMSTRUCT*>(lParam);
        if (DieuKhien::VeNut(thongTin)) {
            return TRUE;
        }
        break;
    }
    default:
        break;
    }
    return DefWindowProcW(hwnd, thongDiep, wParam, lParam);
}

bool CuaSoCoSo::TienXuLy(MSG&)
{
    return false;
}

void CuaSoCoSo::ChayVongLapThongDiep()
{
    MSG thongDiep;
    while (!daDong) {
        const BOOL ketQua = GetMessageW(&thongDiep, nullptr, 0, 0);
        if (ketQua <= 0) {
            if (ketQua == 0) {
                PostQuitMessage(static_cast<int>(thongDiep.wParam));   // để vòng lặp ngoài cũng thoát
            }
            break;
        }
        if (TienXuLy(thongDiep)) {
            continue;
        }
        // [GIẢI THÍCH] IsDialogMessage lo phím Tab (chuyển ô), Enter (= nút OK),
        // ESC (= nút Hủy) giống như một hộp thoại chuẩn của Windows.
        if (hwnd != nullptr && IsDialogMessageW(hwnd, &thongDiep)) {
            continue;
        }
        TranslateMessage(&thongDiep);
        DispatchMessageW(&thongDiep);
    }
}

void CuaSoCoSo::VeKhungCacONhap(HDC hdc, COLORREF mauNen) const
{
    const HWND oDangChon = GetFocus();
    for (HWND oNhap : dsONhap) {
        if (!IsWindowVisible(oNhap)) {
            continue;
        }
        const RECT khung = DieuKhien::KhungCuaONhap(oNhap);
        const bool dangChon = (oDangChon == oNhap);
        const bool dangTat = !IsWindowEnabled(oNhap);

        BoVe::ToBoGoc(hdc, khung, ChuDe::Dp(6),
                      dangTat ? ChuDe::DONG_LE : mauNen,
                      dangChon ? ChuDe::NHAN : ChuDe::VIEN_DAM);
        if (dangChon) {
            // Viền thứ 2 bên trong -> viền dày 2px khi đang nhập
            RECT trong = khung;
            InflateRect(&trong, -1, -1);
            BoVe::ToBoGoc(hdc, trong, ChuDe::Dp(6) - 1, dangTat ? ChuDe::DONG_LE : mauNen, ChuDe::NHAN);
        }

        const wchar_t bieuTuong = DieuKhien::BieuTuongCuaONhap(oNhap);
        if (bieuTuong != 0) {
            RECT vungBieuTuong = khung;
            vungBieuTuong.right = vungBieuTuong.left + ChuDe::Dp(36);
            BoVe::VeBieuTuong(hdc, bieuTuong, vungBieuTuong, LoaiChu::BIEU_TUONG_NHO,
                              dangChon ? ChuDe::NHAN : ChuDe::CHU_PHU);
        }
    }
}

void CuaSoCoSo::XuLyTieuDiemONhap(WPARAM wParam, LPARAM lParam)
{
    const int maThongBao = HIWORD(wParam);
    if (maThongBao != EN_SETFOCUS && maThongBao != EN_KILLFOCUS) {
        return;
    }
    const HWND oNhap = reinterpret_cast<HWND>(lParam);
    for (HWND o : dsONhap) {
        if (o == oNhap) {
            RECT khung = DieuKhien::KhungCuaONhap(oNhap);
            InflateRect(&khung, 2, 2);
            InvalidateRect(hwnd, &khung, FALSE);
            break;
        }
    }
}
