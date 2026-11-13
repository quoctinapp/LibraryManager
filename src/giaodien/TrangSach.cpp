// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của TrangSach (mục lục ở TrangSach.h).
// ============================================================================

#include "TrangSach.h"
#include "DieuKhien.h"
#include "HopThoaiMuonSach.h"
#include "HopThoaiNhapMa.h"
#include "HopThoaiThemSach.h"
#include "MaDieuKhien.h"
#include "XuatCsv.h"

TrangSach::TrangSach(GiaoTiepTrang& giaoTiep)
    : TrangNoiDung(giaoTiep), nutThem(nullptr), nutXoa(nullptr), nutMuon(nullptr), nutXuat(nullptr)
{
}

std::wstring TrangSach::GetTieuDe() const
{
    return L"Quản lý sách";
}

std::wstring TrangSach::GetMoTa() const
{
    return L"Hiển thị, thêm, xóa, tìm kiếm sách và cho mượn";
}

void TrangSach::TaoDieuKhien()
{
    TaoOTimKiem(L"Tìm theo mã, tên sách, tác giả, nhà xuất bản...  (Ctrl+F)");
    nutThem = DieuKhien::TaoNut(hwnd, MA_NUT_THEM, L"Thêm sách", ChuDe::BT_THEM, KieuNut::CHINH, ChuDe::NEN);
    nutMuon = DieuKhien::TaoNut(hwnd, MA_NUT_MUON, L"Cho mượn", ChuDe::BT_TOI, KieuNut::PHU, ChuDe::NEN);
    nutXoa = DieuKhien::TaoNut(hwnd, MA_NUT_XOA, L"Xóa sách", ChuDe::BT_XOA, KieuNut::NGUY_HIEM, ChuDe::NEN);
    nutXuat = DieuKhien::TaoNut(hwnd, MA_NUT_XUAT, L"Xuất CSV", ChuDe::BT_TAI_XUONG, KieuNut::PHU, ChuDe::NEN);

    bang.Tao(hwnd, MA_BANG);
    bang.ThemCot(L"Mã sách", 70);
    bang.ThemCot(L"Tên sách", 205);
    bang.ThemCot(L"Tác giả", 150);
    bang.ThemCot(L"Nhà xuất bản", 160);
    bang.ThemCot(L"Giá bán", 80, LVCFMT_RIGHT);
    bang.ThemCot(L"Năm PH", 72, LVCFMT_CENTER);
    bang.ThemCot(L"Số trang", 70, LVCFMT_RIGHT);
    bang.ThemCot(L"Ngày nhập kho", 118, LVCFMT_CENTER);
    bang.ThemCot(L"Tình trạng", 150);
    bang.DatCotNhanManh(8);
    bang.DatChuKhiRong(L"Không có cuốn sách nào phù hợp");
}

void TrangSach::BoTri(int rong, int cao)
{
    BoTriChuan(rong, cao, { nutXuat, nutXoa, nutMuon, nutThem });
}

// [GIẢI THÍCH] So sánh 2 cuốn sách theo cột thứ "cot" của bảng.
int TrangSach::SoSanh(const Sach& a, const Sach& b, int cot)
{
    switch (cot) {
    case 0: return SoSanhTiengViet(a.GetMaSach(), b.GetMaSach());
    case 1: return SoSanhTiengViet(a.GetTenSach(), b.GetTenSach());
    case 2: return SoSanhTiengViet(a.GetTacGia(), b.GetTacGia());
    case 3: return SoSanhTiengViet(a.GetNhaXuatBan(), b.GetNhaXuatBan());
    case 4: return SoSanhGiaTri(a.GetGiaBan(), b.GetGiaBan());
    case 5: return SoSanhGiaTri(a.GetNamPhatHanh(), b.GetNamPhatHanh());
    case 6: return SoSanhGiaTri(a.GetSoTrang(), b.GetSoTrang());
    case 7: return SoSanhGiaTri(a.GetNgayNhapKho(), b.GetNgayNhapKho());
    case 8: return SoSanhGiaTri(a.GetTinhTrang(), b.GetTinhTrang());
    default: return 0;
    }
}

void TrangSach::LamMoi()
{
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    const std::wstring maDangChon = bang.LayChuDongChon(0);

    dsHienThi = quanLy.TimKiemSach(Utf16SangUtf8(DieuKhien::LayChu(oTimKiem)));
    const int cot = bang.GetCotSapXep();
    if (cot >= 0) {
        dsHienThi.SapXep([this, cot](const Sach& a, const Sach& b) { return DungTruoc(SoSanh(a, b, cot)); });
    }

    bang.BatDauCapNhat();
    for (const Sach& sach : dsHienThi) {
        const bool coSan = sach.ChuaDuocMuon();
        const std::wstring tinhTrang = coSan ? L"0  ·  Có sẵn"
                                             : std::to_wstring(sach.GetTinhTrang()) + L"  ·  Đang mượn";
        bang.ThemDong({ Utf8SangUtf16(sach.GetMaSach()), Utf8SangUtf16(sach.GetTenSach()),
                        Utf8SangUtf16(sach.GetTacGia()), Utf8SangUtf16(sach.GetNhaXuatBan()),
                        Utf8SangUtf16(DinhDangTien(sach.GetGiaBan())), std::to_wstring(sach.GetNamPhatHanh()),
                        std::to_wstring(sach.GetSoTrang()), Utf8SangUtf16(sach.GetNgayNhapKho().ChuyenThanhChuoi()),
                        tinhTrang },
                      coSan ? ChuDe::XANH_LA : ChuDe::DO);
    }
    bang.KetThucCapNhat();
    bang.ChonDongCoChu(maDangChon);

    const DanhSachLienKet<Sach>& tatCa = quanLy.GetDanhSachSach();
    const std::size_t dangMuon = tatCa.DemNeu([](const Sach& s) { return !s.ChuaDuocMuon(); });
    DatTomTat(L"Hiển thị " + std::to_wstring(dsHienThi.KichThuoc()) + L" / "
              + std::to_wstring(tatCa.KichThuoc()) + L" cuốn sách   ·   "
              + std::to_wstring(tatCa.KichThuoc() - dangMuon) + L" còn trong kho   ·   "
              + std::to_wstring(dangMuon) + L" đang được mượn   ·   Tình trạng: 0 = chưa mượn, "
                L"khác 0 = số phiếu mượn");
}

void TrangSach::XuLyLenh(int id, int maThongBao)
{
    switch (id) {
    case MA_TIM_KIEM:
        if (maThongBao == EN_CHANGE) {
            LamMoi();
        }
        break;
    case MA_NUT_THEM:
        ThemSach();
        break;
    case MA_NUT_XOA:
        XoaSach();
        break;
    case MA_NUT_MUON:
        ChoMuon();
        break;
    case MA_NUT_XUAT:
        XuatBaoCao();
        break;
    default:
        break;
    }
}

bool TrangSach::XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua)
{
    if (thongBao->hwndFrom != bang.GetHwnd()) {
        return false;
    }
    if (thongBao->code == NM_DBLCLK) {
        XemChiTiet();
        ketQua = 0;
        return true;
    }
    if (thongBao->code == LVN_KEYDOWN
        && reinterpret_cast<const NMLVKEYDOWN*>(thongBao)->wVKey == VK_DELETE) {
        XoaSach();
        ketQua = 0;
        return true;
    }
    return false;
}

void TrangSach::ThemSach()
{
    HopThoaiThemSach hopThoai(giaoTiep);
    hopThoai.ChayModal();
}

// [GIẢI THÍCH] Đề bài: "Nhập mã sách để xóa". Nếu đã chọn một dòng trong bảng
// thì lấy mã của dòng đó; nếu chưa thì mở hộp thoại cho nhập mã.
void TrangSach::XoaSach()
{
    std::string maSach = Utf16SangUtf8(bang.LayChuDongChon(0));
    if (maSach.empty()) {
        HopThoaiNhapMa hopThoai(giaoTiep, L"Xóa sách", L"Chỉ xóa được sách có tình trạng = 0 (chưa có người mượn)",
                                L"Mã sách cần xóa", ChuDe::BT_XOA, L"Tiếp tục");
        if (!hopThoai.ChayModal()) {
            return;
        }
        maSach = hopThoai.GetMa();
    }

    ThucHienAnToan(L"Xóa sách", [&]() {
        QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
        const Sach* sach = quanLy.TimSach(maSach);
        if (sach == nullptr) {
            throw LoiNghiepVu("Không tìm thấy sách có mã '" + maSach + "' trong danh sách.");
        }
        if (!sach->ChuaDuocMuon()) {
            throw LoiNghiepVu("Không thể xóa sách do sách đang được mượn (phiếu mượn số "
                              + std::to_string(sach->GetTinhTrang()) + ").");
        }
        if (!HopThongBao::XacNhan(giaoTiep.GetCuaSoChinh(), L"Xóa sách " + Utf8SangUtf16(maSach) + L"?",
                                  L"“" + Utf8SangUtf16(sach->GetTenSach()) + L"” – " + Utf8SangUtf16(sach->GetTacGia())
                                      + L"\n\nSách sẽ bị xóa khỏi danh sách và file Sach.txt được ghi đè lại.",
                                  true)) {
            giaoTiep.BaoTrangThai(L"Đã hủy thao tác xóa sách.");
            return;
        }
        quanLy.XoaSach(maSach);
        giaoTiep.DuLieuDaThayDoi();
        giaoTiep.BaoTrangThai(L"Đã xóa sách " + Utf8SangUtf16(maSach) + L" và ghi đè lại file Sach.txt");
    });
}

void TrangSach::ChoMuon()
{
    HopThoaiMuonSach hopThoai(giaoTiep, Utf16SangUtf8(bang.LayChuDongChon(0)));
    hopThoai.ChayModal();
}

void TrangSach::XuatBaoCao()
{
    const std::filesystem::path duongDan = HoiNoiLuuCsv(L"DanhSachSach");
    if (duongDan.empty()) {
        return;
    }
    ThucHienAnToan(L"Xuất CSV", [&]() {
        XuatCsv(duongDan, "Mã sách,Tên sách,Tác giả,Nhà xuất bản,Giá bán,Năm phát hành,Số trang,Ngày nhập kho,Tình trạng",
                dsHienThi, [](const Sach& s) {
                    return NoiCsv({ s.GetMaSach(), s.GetTenSach(), s.GetTacGia(), s.GetNhaXuatBan(),
                                    std::to_string(s.GetGiaBan()), std::to_string(s.GetNamPhatHanh()),
                                    std::to_string(s.GetSoTrang()), s.GetNgayNhapKho().ChuyenThanhChuoi(),
                                    std::to_string(s.GetTinhTrang()) });
                });
        HoanTatXuatCsv(duongDan, dsHienThi.KichThuoc());
    });
}

void TrangSach::XemChiTiet()
{
    const Sach* sach = giaoTiep.GetQuanLy().TimSach(Utf16SangUtf8(bang.LayChuDongChon(0)));
    if (sach == nullptr) {
        return;
    }
    const std::wstring tinhTrang = sach->ChuaDuocMuon()
                                       ? L"0 – Chưa có người mượn"
                                       : std::to_wstring(sach->GetTinhTrang()) + L" – Đang được mượn theo phiếu số "
                                             + std::to_wstring(sach->GetTinhTrang());
    HopThongBao::ThongTin(giaoTiep.GetCuaSoChinh(), Utf8SangUtf16(sach->GetTenSach()),
                          L"Mã sách:  " + Utf8SangUtf16(sach->GetMaSach())
                              + L"\nTác giả:  " + Utf8SangUtf16(sach->GetTacGia())
                              + L"\nNhà xuất bản:  " + Utf8SangUtf16(sach->GetNhaXuatBan())
                              + L"\nGiá bán:  " + Utf8SangUtf16(DinhDangTien(sach->GetGiaBan())) + L" đ"
                              + L"\nNăm phát hành:  " + std::to_wstring(sach->GetNamPhatHanh())
                              + L"\nSố trang:  " + std::to_wstring(sach->GetSoTrang())
                              + L"\nNgày nhập kho:  " + Utf8SangUtf16(sach->GetNgayNhapKho().ChuyenThanhChuoi())
                              + L"\nTình trạng:  " + tinhTrang);
}
