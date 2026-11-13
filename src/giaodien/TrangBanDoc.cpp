// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của TrangBanDoc (mục lục ở TrangBanDoc.h).
// ============================================================================

#include "TrangBanDoc.h"
#include "DieuKhien.h"
#include "HopThoaiNhapMa.h"
#include "HopThoaiThemBanDoc.h"
#include "MaDieuKhien.h"
#include "XuatCsv.h"

TrangBanDoc::TrangBanDoc(GiaoTiepTrang& giaoTiep)
    : TrangNoiDung(giaoTiep), nutThem(nullptr), nutXoa(nullptr), nutXuat(nullptr)
{
}

std::wstring TrangBanDoc::GetTieuDe() const
{
    return L"Bạn đọc";
}

std::wstring TrangBanDoc::GetMoTa() const
{
    return L"Danh sách bạn đọc đã đăng ký thẻ thư viện";
}

void TrangBanDoc::TaoDieuKhien()
{
    TaoOTimKiem(L"Tìm theo mã hoặc họ tên bạn đọc...  (Ctrl+F)");
    nutThem = DieuKhien::TaoNut(hwnd, MA_NUT_THEM, L"Thêm bạn đọc", ChuDe::BT_THEM, KieuNut::CHINH, ChuDe::NEN);
    nutXoa = DieuKhien::TaoNut(hwnd, MA_NUT_XOA, L"Xóa bạn đọc", ChuDe::BT_XOA, KieuNut::NGUY_HIEM, ChuDe::NEN);
    nutXuat = DieuKhien::TaoNut(hwnd, MA_NUT_XUAT, L"Xuất CSV", ChuDe::BT_TAI_XUONG, KieuNut::PHU, ChuDe::NEN);

    bang.Tao(hwnd, MA_BANG);
    bang.ThemCot(L"Mã bạn đọc", 110);
    bang.ThemCot(L"Họ và tên", 260);
    bang.ThemCot(L"Ngày đăng ký", 130, LVCFMT_CENTER);
    bang.ThemCot(L"Đang mượn", 140);
    bang.ThemCot(L"Tổng lượt mượn", 140, LVCFMT_CENTER);
    bang.DatCotNhanManh(3);
    bang.DatChuKhiRong(L"Không có bạn đọc nào phù hợp");
}

void TrangBanDoc::BoTri(int rong, int cao)
{
    BoTriChuan(rong, cao, { nutXuat, nutXoa, nutThem });
}

int TrangBanDoc::SoSanh(const BanDoc& a, const BanDoc& b, int cot) const
{
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    switch (cot) {
    case 0: return SoSanhTiengViet(a.GetMaBanDoc(), b.GetMaBanDoc());
    case 1: return SoSanhTiengViet(a.GetHoTen(), b.GetHoTen());
    case 2: return SoSanhGiaTri(a.GetNgayDangKy(), b.GetNgayDangKy());
    case 3: return SoSanhGiaTri(quanLy.DemPhieuDangMuonCua(a.GetMaBanDoc()),
                                quanLy.DemPhieuDangMuonCua(b.GetMaBanDoc()));
    case 4: return SoSanhGiaTri(quanLy.DemLuotMuonCua(a.GetMaBanDoc()), quanLy.DemLuotMuonCua(b.GetMaBanDoc()));
    default: return 0;
    }
}

void TrangBanDoc::LamMoi()
{
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    const std::wstring maDangChon = bang.LayChuDongChon(0);
    const std::string tuKhoa = Utf16SangUtf8(DieuKhien::LayChu(oTimKiem));

    dsHienThi = quanLy.GetDanhSachBanDoc().Loc([&tuKhoa](const BanDoc& bd) {
        return ChuaTuKhoa(bd.GetMaBanDoc(), tuKhoa) || ChuaTuKhoa(bd.GetHoTen(), tuKhoa);
    });
    const int cot = bang.GetCotSapXep();
    if (cot >= 0) {
        dsHienThi.SapXep([this, cot](const BanDoc& a, const BanDoc& b) { return DungTruoc(SoSanh(a, b, cot)); });
    }

    int tongDangMuon = 0;
    bang.BatDauCapNhat();
    for (const BanDoc& banDoc : dsHienThi) {
        const int dangMuon = quanLy.DemPhieuDangMuonCua(banDoc.GetMaBanDoc());
        tongDangMuon += dangMuon;
        bang.ThemDong({ Utf8SangUtf16(banDoc.GetMaBanDoc()), Utf8SangUtf16(banDoc.GetHoTen()),
                        Utf8SangUtf16(banDoc.GetNgayDangKy().ChuyenThanhChuoi()),
                        dangMuon > 0 ? std::to_wstring(dangMuon) + L" cuốn" : L"Không",
                        std::to_wstring(quanLy.DemLuotMuonCua(banDoc.GetMaBanDoc())) },
                      dangMuon > 0 ? ChuDe::CAM : ChuDe::XANH_LA);
    }
    bang.KetThucCapNhat();
    bang.ChonDongCoChu(maDangChon);

    DatTomTat(L"Hiển thị " + std::to_wstring(dsHienThi.KichThuoc()) + L" / "
              + std::to_wstring(quanLy.GetDanhSachBanDoc().KichThuoc()) + L" bạn đọc   ·   "
              + std::to_wstring(tongDangMuon) + L" cuốn sách đang được giữ");
}

void TrangBanDoc::XuLyLenh(int id, int maThongBao)
{
    switch (id) {
    case MA_TIM_KIEM:
        if (maThongBao == EN_CHANGE) {
            LamMoi();
        }
        break;
    case MA_NUT_THEM:
        ThemBanDoc();
        break;
    case MA_NUT_XOA:
        XoaBanDoc();
        break;
    case MA_NUT_XUAT:
        XuatBaoCao();
        break;
    default:
        break;
    }
}

bool TrangBanDoc::XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua)
{
    if (thongBao->hwndFrom == bang.GetHwnd() && thongBao->code == LVN_KEYDOWN
        && reinterpret_cast<const NMLVKEYDOWN*>(thongBao)->wVKey == VK_DELETE) {
        XoaBanDoc();
        ketQua = 0;
        return true;
    }
    return false;
}

void TrangBanDoc::ThemBanDoc()
{
    HopThoaiThemBanDoc hopThoai(giaoTiep);
    hopThoai.ChayModal();
}

void TrangBanDoc::XoaBanDoc()
{
    std::string maBanDoc = Utf16SangUtf8(bang.LayChuDongChon(0));
    if (maBanDoc.empty()) {
        HopThoaiNhapMa hopThoai(giaoTiep, L"Xóa bạn đọc", L"Chỉ xóa được bạn đọc không còn giữ sách chưa trả",
                                L"Mã bạn đọc cần xóa", ChuDe::BT_XOA, L"Tiếp tục");
        if (!hopThoai.ChayModal()) {
            return;
        }
        maBanDoc = hopThoai.GetMa();
    }

    ThucHienAnToan(L"Xóa bạn đọc", [&]() {
        QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
        const BanDoc* banDoc = quanLy.TimBanDoc(maBanDoc);
        if (banDoc == nullptr) {
            throw LoiNghiepVu("Không tìm thấy bạn đọc có mã '" + maBanDoc + "'.");
        }
        if (!HopThongBao::XacNhan(giaoTiep.GetCuaSoChinh(), L"Xóa bạn đọc " + Utf8SangUtf16(maBanDoc) + L"?",
                                  Utf8SangUtf16(banDoc->GetHoTen())
                                      + L"\n\nBạn đọc sẽ bị xóa khỏi danh sách và file BanDoc.txt được ghi đè lại.",
                                  true)) {
            giaoTiep.BaoTrangThai(L"Đã hủy thao tác xóa bạn đọc.");
            return;
        }
        quanLy.XoaBanDoc(maBanDoc);     // còn phiếu đang mượn -> ném LoiNghiepVu
        giaoTiep.DuLieuDaThayDoi();
        giaoTiep.BaoTrangThai(L"Đã xóa bạn đọc " + Utf8SangUtf16(maBanDoc) + L" và ghi đè lại file BanDoc.txt");
    });
}

void TrangBanDoc::XuatBaoCao()
{
    const std::filesystem::path duongDan = HoiNoiLuuCsv(L"DanhSachBanDoc");
    if (duongDan.empty()) {
        return;
    }
    ThucHienAnToan(L"Xuất CSV", [&]() {
        const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
        XuatCsv(duongDan, "Mã bạn đọc,Họ và tên,Ngày đăng ký,Đang mượn,Tổng lượt mượn", dsHienThi,
                [&quanLy](const BanDoc& bd) {
                    return NoiCsv({ bd.GetMaBanDoc(), bd.GetHoTen(), bd.GetNgayDangKy().ChuyenThanhChuoi(),
                                    std::to_string(quanLy.DemPhieuDangMuonCua(bd.GetMaBanDoc())),
                                    std::to_string(quanLy.DemLuotMuonCua(bd.GetMaBanDoc())) });
                });
        HoanTatXuatCsv(duongDan, dsHienThi.KichThuoc());
    });
}
