// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp HopThongBao (mục lục ở HopThongBao.h).
//  TaskDialogIndirect nhận một "phiếu yêu cầu" (TASKDIALOGCONFIG) mô tả hộp thoại cần hiện; Windows vẽ hộp thoại và trả về nút người dùng đã bấm.
// ============================================================================

#include "HopThongBao.h"
#include <commctrl.h>

static const wchar_t* TIEU_DE_CUA_SO = L"Quản lý thư viện";

int HopThongBao::Hien(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung, PCWSTR bieuTuong, int cacNut, int nutMacDinh) {
    TASKDIALOGCONFIG cauHinh{};
    cauHinh.cbSize = sizeof(cauHinh);
    cauHinh.hwndParent = cha;
    cauHinh.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_POSITION_RELATIVE_TO_WINDOW;
    cauHinh.dwCommonButtons = static_cast<TASKDIALOG_COMMON_BUTTON_FLAGS>(cacNut);
    cauHinh.pszWindowTitle = TIEU_DE_CUA_SO;
    cauHinh.pszMainIcon = bieuTuong;
    cauHinh.pszMainInstruction = tieuDeChinh.c_str();
    cauHinh.pszContent = noiDung.empty() ? nullptr : noiDung.c_str();
    cauHinh.nDefaultButton = nutMacDinh;

    int nutDaBam = IDCANCEL;
    if (FAILED(TaskDialogIndirect(&cauHinh, &nutDaBam, nullptr, nullptr))) {
        // Dự phòng nếu TaskDialog không khả dụng
        const std::wstring toanBo = tieuDeChinh + L"\n\n" + noiDung;
        UINT kieu = MB_OK | MB_ICONINFORMATION;
        if (cacNut & TDCBF_YES_BUTTON) {
            kieu = MB_YESNO | MB_ICONQUESTION;
        }

        nutDaBam = MessageBoxW(cha, toanBo.c_str(), TIEU_DE_CUA_SO, kieu);
    }

    return nutDaBam;
}

void HopThongBao::Loi(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung) {
    MessageBeep(MB_ICONHAND);
    Hien(cha, tieuDeChinh, noiDung, TD_ERROR_ICON, TDCBF_OK_BUTTON, IDOK);
}

void HopThongBao::CanhBao(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung) {
    Hien(cha, tieuDeChinh, noiDung, TD_WARNING_ICON, TDCBF_OK_BUTTON, IDOK);
}

void HopThongBao::ThongTin(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung) {
    Hien(cha, tieuDeChinh, noiDung, TD_INFORMATION_ICON, TDCBF_OK_BUTTON, IDOK);
}

bool HopThongBao::XacNhan(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung, bool nguyHiem) {
    return Hien(cha, tieuDeChinh, noiDung, nguyHiem ? TD_WARNING_ICON : TD_INFORMATION_ICON, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, nguyHiem ? IDNO : IDYES) == IDYES;
}
