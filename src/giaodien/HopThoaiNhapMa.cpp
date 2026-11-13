// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của HopThoaiNhapMa (mục lục ở HopThoaiNhapMa.h).
// ============================================================================

#include "HopThoaiNhapMa.h"
#include "CauHinh.h"
#include "MaDieuKhien.h"

HopThoaiNhapMa::HopThoaiNhapMa(GiaoTiepTrang& giaoTiep, const std::wstring& tieuDe,
                               const std::wstring& moTa, const std::wstring& nhanTruong,
                               wchar_t bieuTuong, const std::wstring& chuNut)
    : HopThoaiCoSo(giaoTiep, tieuDe, moTa, bieuTuong, chuNut), nhanTruong(nhanTruong), oMa(nullptr)
{
}

void HopThoaiNhapMa::TaoNoiDung()
{
    oMa = ThemONhap(nhanTruong, MA_TRUONG_1, ES_UPPERCASE, static_cast<int>(CauHinh::DO_DAI_MA_TOI_DA));
}

bool HopThoaiNhapMa::ThucHien()
{
    const std::string ma = VietHoa(LayChuUtf8(oMa));
    if (ma.empty()) {
        throw LoiNhapLieu("Mã không được để trống.");
    }
    if (!LaMaHopLe(ma)) {
        throw LoiNhapLieu("Mã '" + ma + "' không hợp lệ (chỉ gồm chữ cái không dấu và chữ số).");
    }
    maDaNhap = ma;
    return true;
}
