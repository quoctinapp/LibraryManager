#ifndef TRANGPHIEUMUON_H
#define TRANGPHIEUMUON_H

// ============================================================================
//  TrangPhieuMuon.h - Trang "Quản lý phiếu mượn" (kế thừa TrangNoiDung)
//      - Hiển thị thông tin các phiếu mượn (kèm họ tên bạn đọc, tên sách).
//      - Lọc: Tất cả / Đang mượn / Quá hạn / Đã trả; tìm kiếm tức thì.
//      - Mượn sách, Trả sách (nhập số phiếu), Xuất CSV.
//      - Nhấp đúp một phiếu đang mượn để trả nhanh.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Màu cột tình trạng: VÀNG = đang mượn, ĐỎ = quá hạn trả, XANH = đã trả.
//  Phiếu quá hạn = đang mượn mà ngày hôm nay đã vượt ngày phải trả.
// ----------------------------------------------------------------------------

#include "TrangNoiDung.h"

class TrangPhieuMuon : public TrangNoiDung {
private:
    HWND hopLoc;
    HWND nutMuon;
    HWND nutTra;
    HWND nutXuat;
    DanhSachLienKet<PhieuMuon> dsHienThi;

    std::string TenBanDoc(const std::string& maBanDoc) const;
    std::string TenSach(const std::string& maSach) const;
    int SoSanh(const PhieuMuon& a, const PhieuMuon& b, int cot) const;
    void MuonSach();
    void TraSach();
    void XuatBaoCao();

protected:
    void TaoDieuKhien() override;
    void BoTri(int rong, int cao) override;
    void XuLyLenh(int id, int maThongBao) override;
    bool XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) override;

public:
    explicit TrangPhieuMuon(GiaoTiepTrang& giaoTiep);

    std::wstring GetTieuDe() const override;
    std::wstring GetMoTa() const override;
    void LamMoi() override;
};

#endif
