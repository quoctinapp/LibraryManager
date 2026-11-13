#ifndef TRANGBANDOC_H
#define TRANGBANDOC_H

// ============================================================================
//  TrangBanDoc.h - Trang "Bạn đọc" (kế thừa TrangNoiDung)
//   - Hiển thị bạn đọc kèm số sách đang mượn và tổng số lượt đã mượn.
//   - Thêm bạn đọc, Xóa bạn đọc (không xóa được nếu còn giữ sách), Xuất CSV.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Hai cột "Đang mượn" và "Tổng lượt mượn" không lưu trong BanDoc.txt mà được
//  ĐẾM từ danh sách phiếu mượn mỗi lần làm mới (QuanLyThuVien::DemPhieuDangMuonCua).
// ----------------------------------------------------------------------------

#include "TrangNoiDung.h"

class TrangBanDoc : public TrangNoiDung {
private:
    HWND nutThem;
    HWND nutXoa;
    HWND nutXuat;
    DanhSachLienKet<BanDoc> dsHienThi;

    int SoSanh(const BanDoc& a, const BanDoc& b, int cot) const;
    void ThemBanDoc();
    void XoaBanDoc();
    void XuatBaoCao();

protected:
    void TaoDieuKhien() override;
    void BoTri(int rong, int cao) override;
    void XuLyLenh(int id, int maThongBao) override;
    bool XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) override;

public:
    explicit TrangBanDoc(GiaoTiepTrang& giaoTiep);

    std::wstring GetTieuDe() const override;
    std::wstring GetMoTa() const override;
    void LamMoi() override;
};

#endif
