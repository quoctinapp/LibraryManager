#ifndef TAIKHOAN_H
#define TAIKHOAN_H

// ============================================================================
//  TaiKhoan.h - Lớp Tài khoản của nhân viên quản lý thư viện (kế thừa DoiTuongLuuTru)
//  Thông tin: user, password.  Một dòng trong data/Admin.txt:   user1|12345678
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mật khẩu để private và KHÔNG có hàm GetMatKhau(): bên ngoài chỉ được hỏi "user / password này có khớp không?" qua hàm KhopVoi(), không đọc trộm được.
// ----------------------------------------------------------------------------

#include "DoiTuongLuuTru.h"
#include <cstddef>
#include <string>

class TaiKhoan : public DoiTuongLuuTru {
private:
    std::string tenDangNhap;
    std::string matKhau;

public:
    static const std::size_t SO_TRUONG = 2;

    TaiKhoan();
    TaiKhoan(const std::string& tenDangNhap, const std::string& matKhau);

    std::string GetTenDangNhap() const;
    bool KhopVoi(const std::string& tenDangNhapNhap, const std::string& matKhauNhap) const;

    std::string ChuyenThanhDong() const override;
    bool DocTuDong(const std::string& dong) override;
    std::string LayKhoa() const override;
};

#endif
