#ifndef BANDOC_H
#define BANDOC_H

// ============================================================================
//  BanDoc.h - Lớp Bạn đọc (kế thừa DoiTuongLuuTru)
//  Thông tin: Mã bạn đọc (duy nhất), Họ tên, Ngày đăng ký tham gia.
//  Một dòng trong data/BanDoc.txt:   BD001|Nguyen Van An|05/01/2025
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  "Thẻ thư viện" của một người. Khi mượn sách, chương trình kiểm tra mã bạn
//  đọc có trong danh sách này không; không có thì không cho mượn.
// ----------------------------------------------------------------------------

#include "DoiTuongLuuTru.h"
#include "NgayThang.h"

#include <cstddef>
#include <string>

class BanDoc : public DoiTuongLuuTru {
private:
    std::string maBanDoc;
    std::string hoTen;
    NgayThang ngayDangKy;

public:
    static const std::size_t SO_TRUONG = 3;

    BanDoc();
    BanDoc(const std::string& maBanDoc, const std::string& hoTen, const NgayThang& ngayDangKy);

    std::string GetMaBanDoc() const;
    std::string GetHoTen() const;
    NgayThang GetNgayDangKy() const;

    std::string ChuyenThanhDong() const override;
    bool DocTuDong(const std::string& dong) override;
    std::string LayKhoa() const override;
};

#endif
