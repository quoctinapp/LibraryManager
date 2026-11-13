#ifndef TRANGTONGQUAN_H
#define TRANGTONGQUAN_H

// ============================================================================
//  TrangTongQuan.h - Trang "Tổng quan" / dashboard (kế thừa TrangNoiDung)
//   - 5 thẻ thống kê: tổng số sách, còn trong kho, đang mượn, quá hạn, bạn đọc.
//   - Biểu đồ cột: số lượt mượn của 6 tháng gần nhất (vẽ bằng GDI).
//   - Biểu đồ vòng (donut): tỉ lệ sách còn trong kho / đang được mượn.
//   - Bảng các phiếu mượn quá hạn (nhấp đúp để trả sách).
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Các con số lấy từ QuanLyThuVien::LayThongKe(). Biểu đồ không dùng thư viện
//  nào: mỗi cột là một hình chữ nhật bo góc có chiều cao tỉ lệ với số lượt
//  mượn; biểu đồ vòng là một hình quạt (Pie) đè lên hình tròn rồi khoét giữa.
// ----------------------------------------------------------------------------

#include "TrangNoiDung.h"

class TrangTongQuan : public TrangNoiDung {
private:
    ThongKe thongKe;
    RECT vungThe[5];
    RECT vungBieuDoCot;
    RECT vungBieuDoVong;

    void VeTheThongKe(HDC hdc, const RECT& vung, const std::wstring& nhan, int giaTri,
                      const std::wstring& ghiChu, wchar_t bieuTuong, COLORREF mau, COLORREF mauNhat) const;
    void VeBieuDoCot(HDC hdc) const;
    void VeBieuDoVong(HDC hdc) const;

protected:
    void TaoDieuKhien() override;
    void BoTri(int rong, int cao) override;
    void Ve(HDC hdc, const RECT& vung) override;
    bool XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) override;

public:
    explicit TrangTongQuan(GiaoTiepTrang& giaoTiep);

    std::wstring GetTieuDe() const override;
    std::wstring GetMoTa() const override;
    void LamMoi() override;
};

#endif
