// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp BangDuLieu (mục lục ở BangDuLieu.h).
//  Tô màu từng ô bằng cơ chế "custom draw": trước khi vẽ mỗi ô, ListView hỏi cửa sổ cha (NM_CUSTOMDRAW) "ô này muốn màu gì?" -> ta trả lời bằng màu chữ, màu nền và phông chữ mong muốn.
// ============================================================================

#include "BangDuLieu.h"
#include "CuaSoCoSo.h"
#include <commctrl.h>
#include <uxtheme.h>
#include <cwchar>

BangDuLieu::BangDuLieu() : hwnd(nullptr), soCot(0), cotSapXep(-1), tangDan(true), cotNhanManh(-1), chuKhiRong(L"Không có dữ liệu") {}

void BangDuLieu::Tao(HWND cha, int id) {
    hwnd = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS, 0, 0, 10, 10, cha, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), CuaSoCoSo::GetInstance(), nullptr);
    ListView_SetExtendedListViewStyle(hwnd, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
    SetWindowTheme(hwnd, L"Explorer", nullptr);     // giao diện chọn dòng giống File Explorer
    SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::THUONG)), FALSE);
    SendMessageW(ListView_GetHeader(hwnd), WM_SETFONT, reinterpret_cast<WPARAM>(ChuDe::Phong(LoaiChu::NHO_DAM)), FALSE);

    // [GIẢI THÍCH] Mẹo tăng chiều cao dòng: gắn một danh sách ảnh "rỗng" cao 32px, ListView sẽ kéo cao mọi dòng cho vừa ảnh -> bảng thoáng hơn.
    HIMAGELIST anhRong = ImageList_Create(1, ChuDe::Dp(32), ILC_COLOR32, 1, 1);
    ListView_SetImageList(hwnd, anhRong, LVSIL_SMALL);
}

void BangDuLieu::ThemCot(const std::wstring& tenCot, int rong, int canLe) {
    LVCOLUMNW cot{};
    cot.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    cot.fmt = canLe;
    cot.cx = ChuDe::Dp(rong);
    cot.pszText = const_cast<wchar_t*>(tenCot.c_str());
    ListView_InsertColumn(hwnd, soCot, &cot);
    rongThietKe.push_back(cot.cx);
    ++soCot;
}

// [GIẢI THÍCH] Chia bề ngang bảng cho các cột theo đúng TỈ LỆ độ rộng thiết kế:
// cửa sổ to thì cột rộng ra, cửa sổ nhỏ thì cột hẹp lại -> không bao giờ phải cuộn ngang. Chừa sẵn chỗ cho thanh cuộn dọc; cột cuối nhận phần dư.
void BangDuLieu::CanDoRongCot() {
    if (soCot == 0) {
        return;
    }

    RECT vung;
    GetClientRect(hwnd, &vung);
    const int rongCo = vung.right - vung.left - GetSystemMetrics(SM_CXVSCROLL);
    int tongThietKe = 0;
    for (int rong : rongThietKe) {
        tongThietKe += rong;
    }

    if (rongCo <= 0 || tongThietKe <= 0) {
        return;
    }

    int daDung = 0;
    for (int cot = 0; cot < soCot; ++cot) {
        int rong = MulDiv(rongThietKe[static_cast<std::size_t>(cot)], rongCo, tongThietKe);
        if (cot == soCot - 1) {
            rong = rongCo - daDung;
        }

        ListView_SetColumnWidth(hwnd, cot, rong);
        daDung += rong;
    }
}

void BangDuLieu::DatCotNhanManh(int cot) {
    cotNhanManh = cot;
}

void BangDuLieu::DatChuKhiRong(const std::wstring& chu) {
    chuKhiRong = chu;
}

void BangDuLieu::DatViTri(int x, int y, int rong, int cao) {
    MoveWindow(hwnd, x, y, rong, cao, TRUE);
    CanDoRongCot();
}

void BangDuLieu::BatDauCapNhat() {
    SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(hwnd);
    mauNhanManh.clear();
}

void BangDuLieu::ThemDong(std::initializer_list<std::wstring> cacO, COLORREF mauCotNhanManh) {
    const int dong = ListView_GetItemCount(hwnd);
    int cot = 0;
    for (const std::wstring& o : cacO) {
        if (cot == 0) {
            LVITEMW muc{};
            muc.mask = LVIF_TEXT;
            muc.iItem = dong;
            muc.pszText = const_cast<wchar_t*>(o.c_str());
            ListView_InsertItem(hwnd, &muc);
        } else {
            ListView_SetItemText(hwnd, dong, cot, const_cast<wchar_t*>(o.c_str()));
        }

        ++cot;
    }

    mauNhanManh.push_back(mauCotNhanManh);
}

void BangDuLieu::KetThucCapNhat() {
    CapNhatMuiTenSapXep();
    SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwnd, nullptr, TRUE);
}

int BangDuLieu::SoDong() const {
    return ListView_GetItemCount(hwnd);
}

std::wstring BangDuLieu::LayChuDongChon(int cot) const {
    const int dong = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
    if (dong < 0) {
        return L"";
    }

    wchar_t boDem[512] = L"";
    ListView_GetItemText(hwnd, dong, cot, boDem, 512);
    return boDem;
}

void BangDuLieu::ChonDongCoChu(const std::wstring& chu, int cot) {
    if (chu.empty()) {
        return;
    }

    const int soDong = ListView_GetItemCount(hwnd);
    for (int dong = 0; dong < soDong; ++dong) {
        wchar_t boDem[512] = L"";
        ListView_GetItemText(hwnd, dong, cot, boDem, 512);
        if (chu == boDem) {
            ListView_SetItemState(hwnd, dong, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(hwnd, dong, FALSE);
            return;
        }
    }
}

void BangDuLieu::CapNhatMuiTenSapXep() {
    HWND tieuDe = ListView_GetHeader(hwnd);
    for (int cot = 0; cot < soCot; ++cot) {
        HDITEMW muc{};
        muc.mask = HDI_FORMAT;
        Header_GetItem(tieuDe, cot, &muc);
        muc.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
        if (cot == cotSapXep) {
            muc.fmt |= tangDan ? HDF_SORTUP : HDF_SORTDOWN;
        }

        Header_SetItem(tieuDe, cot, &muc);
    }
}

bool BangDuLieu::XuLyThongBao(const NMHDR* thongBao, LRESULT& ketQua, bool& daBamCot) {
    daBamCot = false;
    if (thongBao == nullptr || thongBao->hwndFrom != hwnd) {
        return false;
    }

    switch (thongBao->code) {
    case LVN_COLUMNCLICK: {
        // Bấm lại cột đang sắp xếp -> đảo chiều; bấm cột khác -> sắp tăng dần theo cột đó
        const NMLISTVIEW* thongTin = reinterpret_cast<const NMLISTVIEW*>(thongBao);
        if (thongTin->iSubItem == cotSapXep) {
            tangDan = !tangDan;
        } else {
            cotSapXep = thongTin->iSubItem;
            tangDan = true;
        }

        CapNhatMuiTenSapXep();
        daBamCot = true;
        ketQua = 0;
        return true;
    }
    case LVN_GETEMPTYMARKUP: {
        NMLVEMPTYMARKUP* thongTin = reinterpret_cast<NMLVEMPTYMARKUP*>(const_cast<NMHDR*>(thongBao));
        thongTin->dwFlags = EMF_CENTERED;
        wcsncpy(thongTin->szMarkup, chuKhiRong.c_str(), L_MAX_URL_LENGTH - 1);
        thongTin->szMarkup[L_MAX_URL_LENGTH - 1] = L'\0';
        ketQua = TRUE;
        return true;
    }
    case NM_CUSTOMDRAW: {
        NMLVCUSTOMDRAW* ve = reinterpret_cast<NMLVCUSTOMDRAW*>(const_cast<NMHDR*>(thongBao));
        switch (ve->nmcd.dwDrawStage) {
        case CDDS_PREPAINT:
            ketQua = CDRF_NOTIFYITEMDRAW;
            return true;
        case CDDS_ITEMPREPAINT:
            ketQua = CDRF_NOTIFYSUBITEMDRAW;
            return true;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM: {
            const std::size_t dong = static_cast<std::size_t>(ve->nmcd.dwItemSpec);
            ve->clrTextBk = (dong % 2 == 1) ? ChuDe::DONG_LE : ChuDe::THE;
            if (ve->iSubItem == cotNhanManh && dong < mauNhanManh.size()) {
                ve->clrText = mauNhanManh[dong];
                SelectObject(ve->nmcd.hdc, ChuDe::Phong(LoaiChu::DAM));
            } else {
                ve->clrText = ChuDe::CHU;
                SelectObject(ve->nmcd.hdc, ChuDe::Phong(LoaiChu::THUONG));
            }

            ketQua = CDRF_NEWFONT;
            return true;
        }
        default:
            ketQua = CDRF_DODEFAULT;
            return true;
        }
    }
    default:
        return false;
    }
}
