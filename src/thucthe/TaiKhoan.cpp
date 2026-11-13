// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp TaiKhoan (mục lục ở TaiKhoan.h).
// ============================================================================

#include "TaiKhoan.h"
#include "CauHinh.h"
#include "TienIch.h"

TaiKhoan::TaiKhoan()
{
}

TaiKhoan::TaiKhoan(const std::string& tenDangNhap, const std::string& matKhau)
    : tenDangNhap(tenDangNhap), matKhau(matKhau)
{
}

std::string TaiKhoan::GetTenDangNhap() const
{
    return tenDangNhap;
}

bool TaiKhoan::KhopVoi(const std::string& tenDangNhapNhap, const std::string& matKhauNhap) const
{
    return tenDangNhap == tenDangNhapNhap && matKhau == matKhauNhap;
}

std::string TaiKhoan::ChuyenThanhDong() const
{
    return tenDangNhap + CauHinh::KY_TU_PHAN_CACH + matKhau;
}

bool TaiKhoan::DocTuDong(const std::string& dong)
{
    std::string o[SO_TRUONG];
    if (!TachChuoi(dong, CauHinh::KY_TU_PHAN_CACH, o)) {
        return false;
    }

    const std::string ten = CatKhoangTrang(o[0]);
    const std::string mk = CatKhoangTrang(o[1]);
    if (ten.empty() || mk.empty()) {
        return false;
    }

    tenDangNhap = ten;
    matKhau = mk;
    return true;
}

std::string TaiKhoan::LayKhoa() const
{
    return tenDangNhap;
}
