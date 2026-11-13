// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp PhieuMuon (mục lục ở PhieuMuon.h).
// ============================================================================

#include "PhieuMuon.h"
#include "CauHinh.h"
#include "TienIch.h"

#include <sstream>

// ============================================================================
//  Constructor
// ============================================================================

PhieuMuon::PhieuMuon()
    : soPhieu(0), tinhTrang(CauHinh::PHIEU_DA_TRA)
{
}

// [GIẢI THÍCH] Phiếu mới: ngày phải trả = ngày mượn + 7, tình trạng = 1 (đang mượn).
PhieuMuon::PhieuMuon(int soPhieu, const std::string& maBanDoc, const std::string& maSach,
                     const NgayThang& ngayMuon)
    : soPhieu(soPhieu), maBanDoc(maBanDoc), maSach(maSach), ngayMuon(ngayMuon),
      ngayPhaiTra(ngayMuon.CongNgay(CauHinh::SO_NGAY_MUON)),
      tinhTrang(CauHinh::PHIEU_DANG_MUON)
{
}

// ============================================================================
//  Getter / trạng thái
// ============================================================================

int PhieuMuon::GetSoPhieu() const { return soPhieu; }
std::string PhieuMuon::GetMaBanDoc() const { return maBanDoc; }
std::string PhieuMuon::GetMaSach() const { return maSach; }
NgayThang PhieuMuon::GetNgayMuon() const { return ngayMuon; }
NgayThang PhieuMuon::GetNgayPhaiTra() const { return ngayPhaiTra; }
int PhieuMuon::GetTinhTrang() const { return tinhTrang; }

bool PhieuMuon::DangMuon() const
{
    return tinhTrang != CauHinh::PHIEU_DA_TRA;
}

bool PhieuMuon::QuaHan(const NgayThang& homNay) const
{
    return DangMuon() && homNay > ngayPhaiTra;
}

void PhieuMuon::DanhDauDaTra()
{
    tinhTrang = CauHinh::PHIEU_DA_TRA;
}

// ============================================================================
//  Lưu / đọc file
// ============================================================================

std::string PhieuMuon::ChuyenThanhDong() const
{
    const char ngan = CauHinh::KY_TU_PHAN_CACH;
    std::ostringstream luong;
    luong << soPhieu << ngan << maBanDoc << ngan << maSach << ngan
          << ngayMuon.ChuyenThanhChuoi() << ngan << ngayPhaiTra.ChuyenThanhChuoi() << ngan
          << tinhTrang;
    return luong.str();
}

bool PhieuMuon::DocTuDong(const std::string& dong)
{
    std::string o[SO_TRUONG];
    if (!TachChuoi(dong, CauHinh::KY_TU_PHAN_CACH, o)) {
        return false;
    }

    int so = 0;
    int tinhTrangDoc = 0;
    NgayThang muon;
    NgayThang tra;
    const std::string maBd = VietHoa(CatKhoangTrang(o[1]));
    const std::string maS = VietHoa(CatKhoangTrang(o[2]));

    const bool hopLe =
        ChuyenSangSo(o[0], so) && so > 0
        && LaMaHopLe(maBd) && LaMaHopLe(maS)
        && NgayThang::DocTuChuoi(o[3], muon)
        && NgayThang::DocTuChuoi(o[4], tra) && tra >= muon
        && ChuyenSangSo(o[5], tinhTrangDoc)
        && (tinhTrangDoc == CauHinh::PHIEU_DA_TRA || tinhTrangDoc == CauHinh::PHIEU_DANG_MUON);

    if (!hopLe) {
        return false;
    }

    soPhieu = so;
    maBanDoc = maBd;
    maSach = maS;
    ngayMuon = muon;
    ngayPhaiTra = tra;
    tinhTrang = tinhTrangDoc;
    return true;
}

std::string PhieuMuon::LayKhoa() const
{
    return std::to_string(soPhieu);
}
