// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp Sach (mục lục ở Sach.h).
// ============================================================================

#include "Sach.h"
#include "CauHinh.h"
#include "TienIch.h"

#include <sstream>

// ============================================================================
//  Constructor
// ============================================================================

Sach::Sach()
    : giaBan(0), namPhatHanh(0), soTrang(0), tinhTrang(CauHinh::SACH_CHUA_MUON)
{
}

// [GIẢI THÍCH] Sách mới tạo LUÔN có tình trạng = 0 (theo đề bài).
Sach::Sach(const std::string& maSach, const std::string& tenSach, const std::string& tacGia,
           const std::string& nhaXuatBan, long long giaBan, int namPhatHanh, int soTrang,
           const NgayThang& ngayNhapKho)
    : maSach(maSach), tenSach(tenSach), tacGia(tacGia), nhaXuatBan(nhaXuatBan),
      giaBan(giaBan), namPhatHanh(namPhatHanh), soTrang(soTrang), ngayNhapKho(ngayNhapKho),
      tinhTrang(CauHinh::SACH_CHUA_MUON)
{
}

// ============================================================================
//  Getter / Setter
// ============================================================================

std::string Sach::GetMaSach() const { return maSach; }
std::string Sach::GetTenSach() const { return tenSach; }
std::string Sach::GetTacGia() const { return tacGia; }
std::string Sach::GetNhaXuatBan() const { return nhaXuatBan; }
long long Sach::GetGiaBan() const { return giaBan; }
int Sach::GetNamPhatHanh() const { return namPhatHanh; }
int Sach::GetSoTrang() const { return soTrang; }
NgayThang Sach::GetNgayNhapKho() const { return ngayNhapKho; }
int Sach::GetTinhTrang() const { return tinhTrang; }

void Sach::SetTinhTrang(int tinhTrangMoi)
{
    tinhTrang = (tinhTrangMoi < 0) ? CauHinh::SACH_CHUA_MUON : tinhTrangMoi;
}

bool Sach::ChuaDuocMuon() const
{
    return tinhTrang == CauHinh::SACH_CHUA_MUON;
}

// ============================================================================
//  Lưu / đọc file
// ============================================================================

std::string Sach::ChuyenThanhDong() const
{
    const char ngan = CauHinh::KY_TU_PHAN_CACH;
    std::ostringstream luong;
    luong << maSach << ngan << tenSach << ngan << tacGia << ngan << nhaXuatBan << ngan
          << giaBan << ngan << namPhatHanh << ngan << soTrang << ngan
          << ngayNhapKho.ChuyenThanhChuoi() << ngan << tinhTrang;
    return luong.str();
}

// [GIẢI THÍCH] Đọc vào các biến TẠM trước, kiểm tra hết mới gán vào sách.
// Nhờ vậy dòng sai sẽ không làm hỏng dữ liệu đang có của đối tượng.
bool Sach::DocTuDong(const std::string& dong)
{
    std::string o[SO_TRUONG];
    if (!TachChuoi(dong, CauHinh::KY_TU_PHAN_CACH, o)) {
        return false;
    }
    for (std::string& truong : o) {
        truong = CatKhoangTrang(truong);
    }

    const std::string ma = VietHoa(o[0]);
    long long gia = 0;
    int nam = 0;
    int trang = 0;
    int tinhTrangDoc = 0;
    NgayThang ngayNhap;

    const bool hopLe =
        LaMaHopLe(ma)
        && !o[1].empty() && !o[2].empty() && !o[3].empty()
        && ChuyenSangSo(o[4], gia)
        && gia >= CauHinh::GIA_BAN_TOI_THIEU && gia <= CauHinh::GIA_BAN_TOI_DA
        && ChuyenSangSo(o[5], nam) && nam >= CauHinh::NAM_PHAT_HANH_TOI_THIEU
        && ChuyenSangSo(o[6], trang)
        && trang >= CauHinh::SO_TRANG_TOI_THIEU && trang <= CauHinh::SO_TRANG_TOI_DA
        && NgayThang::DocTuChuoi(o[7], ngayNhap)
        && ChuyenSangSo(o[8], tinhTrangDoc) && tinhTrangDoc >= CauHinh::SACH_CHUA_MUON;

    if (!hopLe) {
        return false;
    }

    maSach = ma;
    tenSach = o[1];
    tacGia = o[2];
    nhaXuatBan = o[3];
    giaBan = gia;
    namPhatHanh = nam;
    soTrang = trang;
    ngayNhapKho = ngayNhap;
    tinhTrang = tinhTrangDoc;
    return true;
}

std::string Sach::LayKhoa() const
{
    return maSach;
}
