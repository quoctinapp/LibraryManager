#ifndef CUASOCOSO_H
#define CUASOCOSO_H

// ============================================================================
//  CuaSoCoSo.h - Lớp cơ sở TRỪU TƯỢNG cho mọi cửa sổ Win32 của chương trình
//
//      CuaSoCoSo
//        ├── CuaSoDangNhap
//        ├── CuaSoChinh
//        ├── TrangNoiDung (trừu tượng) <- TrangTongQuan, TrangSach, TrangPhieuMuon, TrangBanDoc
//        └── HopThoaiCoSo (trừu tượng) <- HopThoaiThemSach, HopThoaiMuonSach, HopThoaiTraSach, HopThoaiThemBanDoc
//
//  Thủ tục cửa sổ (WndProc) bắt buộc là hàm tĩnh; con trỏ tới đối tượng được
//  gắn vào cửa sổ bằng GWLP_USERDATA rồi gọi hàm ẢO XuLyThongDiep -> mỗi lớp
//  con tự xử lý thông điệp theo cách riêng (đa hình), không cần biến toàn cục.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Windows giao tiếp với cửa sổ bằng cách gửi "thông điệp" (message):
//  WM_PAINT = "hãy vẽ lại", WM_SIZE = "cửa sổ vừa đổi kích thước",
//  WM_COMMAND = "có nút vừa được bấm"... Windows chỉ biết gọi MỘT hàm tĩnh
//  (ThuTucChung). Hàm này tìm ra đối tượng C++ đang sở hữu cửa sổ rồi chuyển
//  thông điệp cho hàm XuLyThongDiep của đúng đối tượng đó.
// ----------------------------------------------------------------------------

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <string>
#include <vector>

class CuaSoCoSo {
private:
    static HINSTANCE hInstance;

    static LRESULT CALLBACK ThuTucChung(HWND hwndCuaSo, UINT thongDiep, WPARAM wParam, LPARAM lParam);

protected:
    HWND hwnd;
    bool daDong;
    std::vector<HWND> dsONhap;      // các ô nhập cần vẽ khung bo góc

    // Hàm ảo: lớp con ghi đè để xử lý thông điệp riêng, phần còn lại gọi lại hàm này
    virtual LRESULT XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam);

    // Hàm "móc" cho vòng lặp thông điệp (ví dụ phím tắt). Trả về true nếu đã xử lý.
    virtual bool TienXuLy(MSG& thongDiep);

    bool TaoCuaSo(DWORD kieuMoRong, DWORD kieu, const std::wstring& tieuDe, int x, int y,
                  int rong, int cao, HWND cha, bool coBong = false);

    // Vẽ khung bo góc cho mọi ô nhập trong dsONhap (gọi trong WM_PAINT)
    void VeKhungCacONhap(HDC hdc, COLORREF mauNen) const;
    // Ô nhập vừa nhận / mất tiêu điểm -> vẽ lại khung (đổi màu viền)
    void XuLyTieuDiemONhap(WPARAM wParam, LPARAM lParam);

public:
    static const wchar_t* TEN_LOP;
    static const wchar_t* TEN_LOP_CO_BONG;

    static void DangKyLop(HINSTANCE hInst);
    static HINSTANCE GetInstance();

    CuaSoCoSo();
    virtual ~CuaSoCoSo();
    CuaSoCoSo(const CuaSoCoSo&) = delete;
    CuaSoCoSo& operator=(const CuaSoCoSo&) = delete;

    HWND GetHwnd() const { return hwnd; }
    bool DaDong() const { return daDong; }

    // Lặp nhận / chuyển thông điệp cho tới khi cửa sổ này bị đóng
    void ChayVongLapThongDiep();
};

#endif
