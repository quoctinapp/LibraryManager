#ifndef SACH_H
#define SACH_H

// ============================================================================
//  Sach.h - Lớp Sách (kế thừa DoiTuongLuuTru)
//  Thông tin: Mã sách (duy nhất), Tên sách, Tác giả, Nhà xuất bản, Giá bán,
//             Năm phát hành, Số trang, Ngày nhập kho, Tình trạng.
//  Tình trạng: = 0 khi sách mới thêm / đã trả; = số phiếu mượn khi đang mượn.
//  Một dòng trong data/Sach.txt:
//    S001|Cau truc du lieu va giai thuat|Do Xuan Loi|NXB KHKT|95000|2019|320|10/01/2025|0
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mỗi đối tượng Sach là một "thẻ sách" trong thư viện. Các thông tin để private (tính đóng gói): bên ngoài muốn đọc phải gọi GetMaSach(), GetTenSach()...
//  và chỉ được sửa duy nhất tình trạng qua SetTinhTrang() (khi mượn / trả).
// ----------------------------------------------------------------------------

#include "DoiTuongLuuTru.h"
#include "NgayThang.h"
#include <cstddef>
#include <string>

class Sach : public DoiTuongLuuTru {
private:
    std::string maSach;
    std::string tenSach;
    std::string tacGia;
    std::string nhaXuatBan;
    long long giaBan;
    int namPhatHanh;
    int soTrang;
    NgayThang ngayNhapKho;
    int tinhTrang;

public:
    static const std::size_t SO_TRUONG = 9;     // số ô trên một dòng của Sach.txt

    Sach();
    Sach(const std::string& maSach, const std::string& tenSach, const std::string& tacGia, const std::string& nhaXuatBan, long long giaBan, int namPhatHanh, int soTrang, const NgayThang& ngayNhapKho);

    std::string GetMaSach() const;
    std::string GetTenSach() const;
    std::string GetTacGia() const;
    std::string GetNhaXuatBan() const;
    long long GetGiaBan() const;
    int GetNamPhatHanh() const;
    int GetSoTrang() const;
    NgayThang GetNgayNhapKho() const;
    int GetTinhTrang() const;

    void SetTinhTrang(int tinhTrangMoi);
    bool ChuaDuocMuon() const;      // tình trạng == 0

    std::string ChuyenThanhDong() const override;
    bool DocTuDong(const std::string& dong) override;
    std::string LayKhoa() const override;
};

#endif
