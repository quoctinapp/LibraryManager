// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của TrangPhieuMuon (mục lục ở TrangPhieuMuon.h).
// ============================================================================

#include "TrangPhieuMuon.h"
#include "DieuKhien.h"
#include "HopThoaiMuonSach.h"
#include "HopThoaiTraSach.h"
#include "MaDieuKhien.h"
#include "XuatCsv.h"

// Thứ tự các mục trong hộp lọc
enum LocPhieu {
    LOC_TAT_CA = 0,
    LOC_DANG_MUON,
    LOC_QUA_HAN,
    LOC_DA_TRA
};

TrangPhieuMuon::TrangPhieuMuon(GiaoTiepTrang& giaoTiep) : TrangNoiDung(giaoTiep), hopLoc(nullptr), nutMuon(nullptr), nutTra(nullptr), nutXuat(nullptr) {}

std::wstring TrangPhieuMuon::GetTieuDe() const {
    return L"Quản lý phiếu mượn";
}

std::wstring TrangPhieuMuon::GetMoTa() const {
    return L"Hiển thị phiếu mượn, mượn sách và trả sách";
}

void TrangPhieuMuon::TaoDieuKhien() {
    TaoOTimKiem(L"Tìm theo số phiếu, bạn đọc, sách...  (Ctrl+F)");
    hopLoc = DieuKhien::TaoHopChon(hwnd, MA_HOP_LOC, false);
    for (const wchar_t* muc : { L"Tất cả phiếu", L"Đang mượn", L"Quá hạn", L"Đã trả" }) {
        SendMessageW(hopLoc, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(muc));
    }

    SendMessageW(hopLoc, CB_SETCURSEL, LOC_TAT_CA, 0);

    nutMuon = DieuKhien::TaoNut(hwnd, MA_NUT_MUON, L"Mượn sách", ChuDe::BT_TOI, KieuNut::CHINH, ChuDe::NEN);
    nutTra = DieuKhien::TaoNut(hwnd, MA_NUT_TRA, L"Trả sách", ChuDe::BT_LUI, KieuNut::THANH_CONG, ChuDe::NEN);
    nutXuat = DieuKhien::TaoNut(hwnd, MA_NUT_XUAT, L"Xuất CSV", ChuDe::BT_TAI_XUONG, KieuNut::PHU, ChuDe::NEN);

    bang.Tao(hwnd, MA_BANG);
    bang.ThemCot(L"Số phiếu", 75, LVCFMT_CENTER);
    bang.ThemCot(L"Mã bạn đọc", 95);
    bang.ThemCot(L"Họ tên bạn đọc", 170);
    bang.ThemCot(L"Mã sách", 80);
    bang.ThemCot(L"Tên sách", 220);
    bang.ThemCot(L"Ngày mượn", 105, LVCFMT_CENTER);
    bang.ThemCot(L"Ngày phải trả", 110, LVCFMT_CENTER);
    bang.ThemCot(L"Tình trạng", 160);
    bang.DatCotNhanManh(7);
    bang.DatChuKhiRong(L"Không có phiếu mượn nào phù hợp");
}

void TrangPhieuMuon::BoTri(int rong, int cao) {
    BoTriChuan(rong, cao, { nutXuat, nutTra, nutMuon }, hopLoc);
}

std::string TrangPhieuMuon::TenBanDoc(const std::string& maBanDoc) const {
    const BanDoc* banDoc = giaoTiep.GetQuanLy().TimBanDoc(maBanDoc);
    return banDoc != nullptr ? banDoc->GetHoTen() : "(không rõ)";
}

std::string TrangPhieuMuon::TenSach(const std::string& maSach) const {
    const Sach* sach = giaoTiep.GetQuanLy().TimSach(maSach);
    return sach != nullptr ? sach->GetTenSach() : "(đã bị xóa)";
}

int TrangPhieuMuon::SoSanh(const PhieuMuon& a, const PhieuMuon& b, int cot) const {
    switch (cot) {
    case 0: return SoSanhGiaTri(a.GetSoPhieu(), b.GetSoPhieu());
    case 1: return SoSanhTiengViet(a.GetMaBanDoc(), b.GetMaBanDoc());
    case 2: return SoSanhTiengViet(TenBanDoc(a.GetMaBanDoc()), TenBanDoc(b.GetMaBanDoc()));
    case 3: return SoSanhTiengViet(a.GetMaSach(), b.GetMaSach());
    case 4: return SoSanhTiengViet(TenSach(a.GetMaSach()), TenSach(b.GetMaSach()));
    case 5: return SoSanhGiaTri(a.GetNgayMuon(), b.GetNgayMuon());
    case 6: return SoSanhGiaTri(a.GetNgayPhaiTra(), b.GetNgayPhaiTra());
    case 7: {
        // Thứ tự mức độ cần chú ý: quá hạn -> đang mượn -> đã trả
        const NgayThang homNay = NgayThang::HomNay();
        auto mucDo = [&homNay](const PhieuMuon& p) { return p.QuaHan(homNay) ? 0 : (p.DangMuon() ? 1 : 2); };
        return SoSanhGiaTri(mucDo(a), mucDo(b));
    }
    default: return 0;
    }
}

void TrangPhieuMuon::LamMoi() {
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    const std::wstring soDangChon = bang.LayChuDongChon(0);
    const std::string tuKhoa = Utf16SangUtf8(DieuKhien::LayChu(oTimKiem));
    const int loc = static_cast<int>(SendMessageW(hopLoc, CB_GETCURSEL, 0, 0));
    const NgayThang homNay = NgayThang::HomNay();

    // Lọc theo tình trạng + từ khóa
    dsHienThi = quanLy.GetDanhSachPhieuMuon().Loc([&](const PhieuMuon& p) {
        if ((loc == LOC_DANG_MUON && !p.DangMuon()) || (loc == LOC_QUA_HAN && !p.QuaHan(homNay)) || (loc == LOC_DA_TRA && p.DangMuon())) {
            return false;
        }

        return ChuaTuKhoa(std::to_string(p.GetSoPhieu()), tuKhoa) || ChuaTuKhoa(p.GetMaBanDoc(), tuKhoa) || ChuaTuKhoa(TenBanDoc(p.GetMaBanDoc()), tuKhoa) || ChuaTuKhoa(p.GetMaSach(), tuKhoa) || ChuaTuKhoa(TenSach(p.GetMaSach()), tuKhoa);
    });
    const int cot = bang.GetCotSapXep();
    if (cot >= 0) {
        dsHienThi.SapXep([this, cot](const PhieuMuon& a, const PhieuMuon& b) {
            return DungTruoc(SoSanh(a, b, cot));
        });
    }

    bang.BatDauCapNhat();
    for (const PhieuMuon& phieu : dsHienThi) {
        std::wstring tinhTrang = L"0  ·  Đã trả";
        COLORREF mau = ChuDe::XANH_LA;
        if (phieu.QuaHan(homNay)) {
            tinhTrang = L"1  ·  Quá hạn " + std::to_wstring(phieu.GetNgayPhaiTra().SoNgayDen(homNay)) + L" ngày";
            mau = ChuDe::DO;
        } else if (phieu.DangMuon()) {
            tinhTrang = L"1  ·  Đang mượn";
            mau = ChuDe::CAM;
        }

        bang.ThemDong({ std::to_wstring(phieu.GetSoPhieu()), Utf8SangUtf16(phieu.GetMaBanDoc()), Utf8SangUtf16(TenBanDoc(phieu.GetMaBanDoc())), Utf8SangUtf16(phieu.GetMaSach()), Utf8SangUtf16(TenSach(phieu.GetMaSach())), Utf8SangUtf16(phieu.GetNgayMuon().ChuyenThanhChuoi()), Utf8SangUtf16(phieu.GetNgayPhaiTra().ChuyenThanhChuoi()), tinhTrang }, mau);
    }

    bang.KetThucCapNhat();
    bang.ChonDongCoChu(soDangChon);

    const DanhSachLienKet<PhieuMuon>& tatCa = quanLy.GetDanhSachPhieuMuon();
    const std::size_t dangMuon = tatCa.DemNeu([](const PhieuMuon& p) { return p.DangMuon(); });
    const std::size_t quaHan = tatCa.DemNeu([&homNay](const PhieuMuon& p) { return p.QuaHan(homNay); });
    DatTomTat(L"Hiển thị " + std::to_wstring(dsHienThi.KichThuoc()) + L" / " + std::to_wstring(tatCa.KichThuoc()) + L" phiếu   ·   " + std::to_wstring(dangMuon) + L" đang mượn   ·   " + std::to_wstring(quaHan) + L" quá hạn   ·   Tình trạng: 1 = đang mượn, 0 = đã trả   ·   Nhấp đúp phiếu để trả sách");
}

void TrangPhieuMuon::XuLyLenh(int id, int maThongBao) {
    switch (id) {
    case MA_TIM_KIEM:
        if (maThongBao == EN_CHANGE) {
            LamMoi();
        }
        break;
    case MA_HOP_LOC:
        if (maThongBao == CBN_SELCHANGE) {
            LamMoi();
        }
        break;
    case MA_NUT_MUON:
        MuonSach();
        break;
    case MA_NUT_TRA:
        TraSach();
        break;
    case MA_NUT_XUAT:
        XuatBaoCao();
        break;
    default:
        break;
    }
}

bool TrangPhieuMuon::XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) {
    if (thongBao->hwndFrom == bang.GetHwnd() && thongBao->code == NM_DBLCLK) {
        TraSach();
        ketQua = 0;
        return true;
    }

    return false;
}

void TrangPhieuMuon::MuonSach() {
    HopThoaiMuonSach hopThoai(giaoTiep, "");
    hopThoai.ChayModal();
}

// [GIẢI THÍCH] Đề bài: "Nhập số phiếu mượn để yêu cầu trả sách". Hộp thoại luôn cho nhập số phiếu; nếu đang chọn một phiếu thì điền sẵn số của phiếu đó.
void TrangPhieuMuon::TraSach() {
    int soPhieu = 0;
    ChuyenSangSo(Utf16SangUtf8(bang.LayChuDongChon(0)), soPhieu);
    HopThoaiTraSach hopThoai(giaoTiep, soPhieu);
    hopThoai.ChayModal();
}

void TrangPhieuMuon::XuatBaoCao() {
    const std::filesystem::path duongDan = HoiNoiLuuCsv(L"DanhSachPhieuMuon");
    if (duongDan.empty()) {
        return;
    }

    ThucHienAnToan(L"Xuất CSV", [&]() {
        XuatCsv(duongDan, "Số phiếu,Mã bạn đọc,Họ tên bạn đọc,Mã sách,Tên sách,Ngày mượn,Ngày phải trả,Tình trạng", dsHienThi, [this](const PhieuMuon& p) {
                    return NoiCsv({ std::to_string(p.GetSoPhieu()), p.GetMaBanDoc(), TenBanDoc(p.GetMaBanDoc()), p.GetMaSach(), TenSach(p.GetMaSach()), p.GetNgayMuon().ChuyenThanhChuoi(), p.GetNgayPhaiTra().ChuyenThanhChuoi(), std::to_string(p.GetTinhTrang()) });
                });
        HoanTatXuatCsv(duongDan, dsHienThi.KichThuoc());
    });
}
