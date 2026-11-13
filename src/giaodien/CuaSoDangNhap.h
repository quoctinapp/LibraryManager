#ifndef CUASODANGNHAP_H
#define CUASODANGNHAP_H

// ============================================================================
//  CuaSoDangNhap.h - Màn hình "ĐĂNG NHẬP HỆ THỐNG" (kế thừa CuaSoCoSo)
//   - Chỉ cho vào hệ thống khi user / password có trong file Admin.txt.
//   - Mật khẩu hiển thị thành dấu '*'.
//   - Sai thì báo và cho nhập lại; sai 3 lần thì thoát khỏi hệ thống.
//   - Nhấn ESC (hoặc nút ✕) để thoát khỏi hệ thống.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Cửa sổ không có thanh tiêu đề của Windows (WS_POPUP) mà tự vẽ toàn bộ:
//  dải màu chuyển sắc phía trên có biểu tượng cuốn sách, phía dưới là 2 ô
//  nhập và nút Đăng nhập. Muốn kéo cửa sổ thì giữ chuột ở dải màu: ta trả lời
//  Windows rằng vùng đó là "thanh tiêu đề" (HTCAPTION) trong WM_NCHITTEST.
//  Nhập sai: cửa sổ rung nhẹ + dòng báo lỗi màu đỏ + số lần thử còn lại.
// ----------------------------------------------------------------------------

#include "CuaSoCoSo.h"
#include "QuanLyThuVien.h"

#include <string>

enum class KetQuaDangNhap {
    THANH_CONG,
    SAI_QUA_SO_LAN,
    THOAT
};

class CuaSoDangNhap : public CuaSoCoSo {
private:
    const QuanLyThuVien& quanLy;
    int soLanConLai;
    std::wstring thongBao;
    bool thongBaoLaLoi;
    KetQuaDangNhap ketQua;
    std::wstring tenDangNhap;
    HWND oTen;
    HWND oMatKhau;
    HWND nutDangNhap;
    HWND nutDong;

    void TaoDieuKhien();
    void ThuDangNhap();
    void RungCuaSo();
    void Ve(HDC hdc, const RECT& vung) const;

protected:
    LRESULT XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) override;

public:
    explicit CuaSoDangNhap(const QuanLyThuVien& quanLy);

    KetQuaDangNhap Chay();
    std::wstring GetTenDangNhap() const { return tenDangNhap; }
};

#endif
