// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của HopThoaiMuonSach (mục lục ở HopThoaiMuonSach.h).
// ============================================================================

#include "HopThoaiMuonSach.h"
#include "CauHinh.h"
#include "HopThongBao.h"
#include "MaDieuKhien.h"

#include <commctrl.h>

HopThoaiMuonSach::HopThoaiMuonSach(GiaoTiepTrang& giaoTiep, const std::string& maSachChonSan)
    : HopThoaiCoSo(giaoTiep, L"Mượn sách",
                   L"Nhập mã sách và mã bạn đọc. Ngày phải trả = ngày mượn + 7 ngày",
                   ChuDe::BT_TOI, L"Tạo phiếu mượn"),
      maSachChonSan(maSachChonSan), hopSach(nullptr), hopBanDoc(nullptr)
{
}

void HopThoaiMuonSach::TaoNoiDung()
{
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();

    hopSach = ThemHopChon(L"Mã sách (gõ mã hoặc chọn sách đang có sẵn)", MA_TRUONG_1, true);
    for (const Sach& sach : quanLy.GetDanhSachSach()) {
        if (sach.ChuaDuocMuon()) {
            const std::wstring dong = Utf8SangUtf16(sach.GetMaSach()) + L"  –  "
                                      + Utf8SangUtf16(sach.GetTenSach());
            SendMessageW(hopSach, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(dong.c_str()));
        }
    }
    SendMessageW(hopSach, CB_LIMITTEXT, 80, 0);

    hopBanDoc = ThemHopChon(L"Mã bạn đọc (gõ mã hoặc chọn)", MA_TRUONG_2, true);
    for (const BanDoc& banDoc : quanLy.GetDanhSachBanDoc()) {
        const std::wstring dong = Utf8SangUtf16(banDoc.GetMaBanDoc()) + L"  –  "
                                  + Utf8SangUtf16(banDoc.GetHoTen());
        SendMessageW(hopBanDoc, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(dong.c_str()));
    }

    // Sách được chọn sẵn từ trang Sách (nút "Cho mượn")
    if (!maSachChonSan.empty()) {
        const std::wstring tienTo = Utf8SangUtf16(maSachChonSan) + L" ";
        const LRESULT viTri = SendMessageW(hopSach, CB_FINDSTRING, static_cast<WPARAM>(-1),
                                           reinterpret_cast<LPARAM>(tienTo.c_str()));
        if (viTri != CB_ERR) {
            SendMessageW(hopSach, CB_SETCURSEL, static_cast<WPARAM>(viTri), 0);
        } else {
            SetWindowTextW(hopSach, Utf8SangUtf16(maSachChonSan).c_str());
        }
    }

    const NgayThang homNay = NgayThang::HomNay();
    ThemOThongTin(L"Ngày mượn", Utf8SangUtf16(homNay.ChuyenThanhChuoi()), ChuDe::CHU, ViTriTruong::TRAI);
    ThemOThongTin(L"Ngày phải trả",
                  Utf8SangUtf16(homNay.CongNgay(CauHinh::SO_NGAY_MUON).ChuyenThanhChuoi()),
                  ChuDe::CAM, ViTriTruong::PHAI);
    ThemOThongTin(L"Số phiếu mượn sẽ tạo",
                  L"Phiếu số " + std::to_wstring(quanLy.LaySoPhieuTiepTheo())
                      + L"   ·   tình trạng phiếu = 1 (đang mượn)",
                  ChuDe::NHAN);
}

bool HopThoaiMuonSach::ThucHien()
{
    const std::string maSach = LayMaDauChuoi(hopSach);
    const std::string maBanDoc = LayMaDauChuoi(hopBanDoc);
    if (maSach.empty()) {
        throw LoiNhapLieu("Chưa nhập mã sách.");
    }
    if (maBanDoc.empty()) {
        throw LoiNhapLieu("Chưa nhập mã bạn đọc.");
    }

    QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    const PhieuMuon phieu = quanLy.MuonSach(maSach, maBanDoc);
    giaoTiep.DuLieuDaThayDoi();

    const Sach* sach = quanLy.TimSach(maSach);
    const BanDoc* banDoc = quanLy.TimBanDoc(maBanDoc);
    const std::wstring soPhieu = std::to_wstring(phieu.GetSoPhieu());
    giaoTiep.BaoTrangThai(L"Đã tạo phiếu mượn số " + soPhieu + L" – cập nhật Sach.txt và PhieuMuon.txt");
    HopThongBao::ThongTin(GetHwnd(), L"Tạo phiếu mượn số " + soPhieu + L" thành công",
                          L"Bạn đọc:  " + Utf8SangUtf16(maBanDoc) + L" – "
                              + (banDoc != nullptr ? Utf8SangUtf16(banDoc->GetHoTen()) : L"")
                              + L"\nSách:  " + Utf8SangUtf16(maSach) + L" – "
                              + (sach != nullptr ? Utf8SangUtf16(sach->GetTenSach()) : L"")
                              + L"\nNgày mượn:  " + Utf8SangUtf16(phieu.GetNgayMuon().ChuyenThanhChuoi())
                              + L"\nNgày phải trả:  " + Utf8SangUtf16(phieu.GetNgayPhaiTra().ChuyenThanhChuoi())
                              + L"\n\nTình trạng sách đã cập nhật = " + soPhieu + L".");
    return true;
}
