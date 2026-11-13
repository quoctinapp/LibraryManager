// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp BanDoc (mục lục ở BanDoc.h).
// ============================================================================

#include "BanDoc.h"
#include "CauHinh.h"
#include "TienIch.h"

BanDoc::BanDoc()
{
}

BanDoc::BanDoc(const std::string& maBanDoc, const std::string& hoTen, const NgayThang& ngayDangKy)
    : maBanDoc(maBanDoc), hoTen(hoTen), ngayDangKy(ngayDangKy)
{
}

std::string BanDoc::GetMaBanDoc() const { return maBanDoc; }
std::string BanDoc::GetHoTen() const { return hoTen; }
NgayThang BanDoc::GetNgayDangKy() const { return ngayDangKy; }

std::string BanDoc::ChuyenThanhDong() const
{
    const std::string ngan(1, CauHinh::KY_TU_PHAN_CACH);
    return maBanDoc + ngan + hoTen + ngan + ngayDangKy.ChuyenThanhChuoi();
}

bool BanDoc::DocTuDong(const std::string& dong)
{
    std::string o[SO_TRUONG];
    if (!TachChuoi(dong, CauHinh::KY_TU_PHAN_CACH, o)) {
        return false;
    }

    const std::string ma = VietHoa(CatKhoangTrang(o[0]));
    const std::string ten = CatKhoangTrang(o[1]);
    NgayThang ngay;
    if (!LaMaHopLe(ma) || ten.empty() || !NgayThang::DocTuChuoi(o[2], ngay)) {
        return false;
    }

    maBanDoc = ma;
    hoTen = ten;
    ngayDangKy = ngay;
    return true;
}

std::string BanDoc::LayKhoa() const
{
    return maBanDoc;
}
