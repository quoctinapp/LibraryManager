#ifndef HOPTHONGBAO_H
#define HOPTHONGBAO_H

// ============================================================================
//  HopThongBao.h - Các hộp thông báo / hỏi xác nhận (TaskDialog của Windows)
//  TaskDialog là hộp thoại hiện đại có tiêu đề chính in đậm, nội dung bên dưới và biểu tượng lỗi / cảnh báo / thông tin.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Thay vì mỗi chỗ tự gọi API dài dòng, mọi thông báo đều đi qua lớp này:
//    HopThongBao::Loi(cuaSo, L"Không thể xóa sách", L"Sách đang được mượn..."); if (HopThongBao::XacNhan(cuaSo, L"Xóa sách?", L"...")) { ... }
// ----------------------------------------------------------------------------

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <string>

class HopThongBao {
private:
    static int Hien(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung, PCWSTR bieuTuong, int cacNut, int nutMacDinh);

public:
    HopThongBao() = delete;

    static void Loi(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung);
    static void CanhBao(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung);
    static void ThongTin(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung);

    // Hỏi Có / Không. nguyHiem = true: nút mặc định là "Không" (tránh bấm Enter nhầm)
    static bool XacNhan(HWND cha, const std::wstring& tieuDeChinh, const std::wstring& noiDung, bool nguyHiem = false);
};

#endif
