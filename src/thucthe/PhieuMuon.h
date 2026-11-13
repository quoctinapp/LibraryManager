#ifndef PHIEUMUON_H
#define PHIEUMUON_H

// ============================================================================
//  PhieuMuon.h - Lớp Phiếu mượn (kế thừa DoiTuongLuuTru)
//  Thông tin: Số phiếu mượn, Mã bạn đọc, Mã sách, Ngày mượn (ngày hệ thống),
//             Ngày phải trả (= ngày mượn + 7), Tình trạng.
//  Tình trạng: phiếu mới tạo = 1 (đang mượn); khi trả sách = 0.
//  Một dòng trong data/PhieuMuon.txt:   2|BD002|S003|05/11/2026|12/11/2026|1
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Phiếu mượn là "biên nhận": ai (mã bạn đọc) mượn cuốn nào (mã sách), ngày
//  nào, hạn trả ngày nào. Constructor tự điền ngày phải trả = ngày mượn + 7
//  và tình trạng = 1, nên không thể tạo nhầm một phiếu sai quy định.
// ----------------------------------------------------------------------------

#include "DoiTuongLuuTru.h"
#include "NgayThang.h"

#include <cstddef>
#include <string>

class PhieuMuon : public DoiTuongLuuTru {
private:
    int soPhieu;
    std::string maBanDoc;
    std::string maSach;
    NgayThang ngayMuon;
    NgayThang ngayPhaiTra;
    int tinhTrang;

public:
    static const std::size_t SO_TRUONG = 6;

    PhieuMuon();
    PhieuMuon(int soPhieu, const std::string& maBanDoc, const std::string& maSach,
              const NgayThang& ngayMuon);

    int GetSoPhieu() const;
    std::string GetMaBanDoc() const;
    std::string GetMaSach() const;
    NgayThang GetNgayMuon() const;
    NgayThang GetNgayPhaiTra() const;
    int GetTinhTrang() const;

    bool DangMuon() const;                              // tình trạng != 0
    bool QuaHan(const NgayThang& homNay) const;         // đang mượn và đã quá ngày phải trả
    void DanhDauDaTra();                                // tình trạng = 0

    std::string ChuyenThanhDong() const override;
    bool DocTuDong(const std::string& dong) override;
    std::string LayKhoa() const override;
};

#endif
