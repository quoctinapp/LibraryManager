// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của HopThoaiThemBanDoc (mục lục ở HopThoaiThemBanDoc.h).
// ============================================================================

#include "HopThoaiThemBanDoc.h"
#include "CauHinh.h"
#include "DieuKhien.h"
#include "MaDieuKhien.h"

HopThoaiThemBanDoc::HopThoaiThemBanDoc(GiaoTiepTrang& giaoTiep)
    : HopThoaiCoSo(giaoTiep, L"Đăng ký bạn đọc mới",
                   L"Bạn đọc mới được ghi nối vào cuối file BanDoc.txt",
                   ChuDe::BT_LIEN_HE, L"Thêm bạn đọc"),
      oMa(nullptr), chonNgayDangKy(nullptr), oHoTen(nullptr)
{
}

void HopThoaiThemBanDoc::TaoNoiDung()
{
    oMa = ThemONhap(L"Mã bạn đọc", MA_TRUONG_1, ES_UPPERCASE,
                    static_cast<int>(CauHinh::DO_DAI_MA_TOI_DA), ViTriTruong::TRAI);
    DieuKhien::DatChu(oMa, Utf8SangUtf16(giaoTiep.GetQuanLy().GoiYMaBanDocMoi()));
    chonNgayDangKy = ThemChonNgay(L"Ngày đăng ký", MA_TRUONG_2, NgayThang::HomNay(), ViTriTruong::PHAI);
    oHoTen = ThemONhap(L"Họ và tên", MA_TRUONG_3, 0, static_cast<int>(CauHinh::DO_DAI_HO_TEN_TOI_DA));
}

bool HopThoaiThemBanDoc::ThucHien()
{
    const BanDoc banDocMoi(VietHoa(LayChuUtf8(oMa)), LayChuUtf8(oHoTen), LayNgay(chonNgayDangKy));

    giaoTiep.GetQuanLy().ThemBanDoc(banDocMoi);
    giaoTiep.DuLieuDaThayDoi();
    giaoTiep.BaoTrangThai(L"Đã thêm bạn đọc " + Utf8SangUtf16(banDocMoi.GetMaBanDoc()) + L" – "
                          + Utf8SangUtf16(banDocMoi.GetHoTen()) + L" (ghi nối vào cuối BanDoc.txt)");
    return true;
}
