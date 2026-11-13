// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của HopThoaiTraSach (mục lục ở HopThoaiTraSach.h).
// ============================================================================

#include "HopThoaiTraSach.h"
#include "BoVe.h"
#include "DieuKhien.h"
#include "HopThongBao.h"
#include "MaDieuKhien.h"

#include <climits>

HopThoaiTraSach::HopThoaiTraSach(GiaoTiepTrang& giaoTiep, int soPhieuChonSan)
    : HopThoaiCoSo(giaoTiep, L"Trả sách",
                   L"Nhập số phiếu mượn. Phiếu và sách sẽ được cập nhật tình trạng = 0",
                   ChuDe::BT_LUI, L"Xác nhận trả"),
      soPhieuChonSan(soPhieuChonSan), oSoPhieu(nullptr), vungXemTruoc{}
{
}

void HopThoaiTraSach::TaoNoiDung()
{
    oSoPhieu = ThemONhap(L"Số phiếu mượn", MA_TRUONG_1, ES_NUMBER, 9);
    if (soPhieuChonSan > 0) {
        DieuKhien::DatChu(oSoPhieu, std::to_wstring(soPhieuChonSan));
    }
    vungXemTruoc = ThemVungTuVe(ChuDe::Dp(150));
}

void HopThoaiTraSach::XuLyLenhThem(int id, int maThongBao)
{
    if (id == MA_TRUONG_1 && maThongBao == EN_CHANGE) {
        InvalidateRect(hwnd, &vungXemTruoc, FALSE);
    }
}

// [GIẢI THÍCH] Vẽ thẻ "xem trước" thông tin phiếu ứng với số đang nhập.
void HopThoaiTraSach::VeThem(HDC hdc)
{
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    int soPhieu = 0;
    const PhieuMuon* phieu = nullptr;
    if (ChuyenSangSo(LayChuUtf8(oSoPhieu), soPhieu)) {
        phieu = quanLy.TimPhieuMuon(soPhieu);
    }

    COLORREF mauNen = ChuDe::DONG_LE;
    COLORREF mauVien = ChuDe::VIEN;
    if (phieu != nullptr && phieu->DangMuon()) {
        mauNen = phieu->QuaHan(NgayThang::HomNay()) ? ChuDe::DO_NHAT : ChuDe::XANH_LA_NHAT;
        mauVien = mauNen;
    }
    BoVe::ToBoGoc(hdc, vungXemTruoc, ChuDe::Dp(8), mauNen, mauVien);

    RECT vung = vungXemTruoc;
    InflateRect(&vung, -ChuDe::Dp(16), -ChuDe::Dp(12));
    if (phieu == nullptr) {
        BoVe::VietChu(hdc, LayChuUtf8(oSoPhieu).empty() ? L"Nhập số phiếu để xem thông tin phiếu mượn."
                                                         : L"Không tìm thấy phiếu mượn có số này.",
                      vung, LoaiChu::THUONG, ChuDe::CHU_PHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }

    const Sach* sach = quanLy.TimSach(phieu->GetMaSach());
    const BanDoc* banDoc = quanLy.TimBanDoc(phieu->GetMaBanDoc());
    const NgayThang homNay = NgayThang::HomNay();

    std::wstring trangThai;
    COLORREF mauTrangThai = ChuDe::XANH_LA_DAM;
    if (!phieu->DangMuon()) {
        trangThai = L"Phiếu này ĐÃ TRẢ (tình trạng = 0) – không thể trả lại.";
        mauTrangThai = ChuDe::CHU_PHU;
    } else if (phieu->QuaHan(homNay)) {
        trangThai = L"Đang mượn – QUÁ HẠN " + std::to_wstring(phieu->GetNgayPhaiTra().SoNgayDen(homNay))
                    + L" ngày";
        mauTrangThai = ChuDe::DO_DAM;
    } else {
        trangThai = L"Đang mượn – còn " + std::to_wstring(homNay.SoNgayDen(phieu->GetNgayPhaiTra()))
                    + L" ngày tới hạn trả";
    }

    const int caoDong = ChuDe::Dp(25);
    const std::wstring dong[] = {
        L"Bạn đọc:   " + Utf8SangUtf16(phieu->GetMaBanDoc()) + L" – "
            + (banDoc != nullptr ? Utf8SangUtf16(banDoc->GetHoTen()) : L"(không rõ)"),
        L"Sách:   " + Utf8SangUtf16(phieu->GetMaSach()) + L" – "
            + (sach != nullptr ? Utf8SangUtf16(sach->GetTenSach()) : L"(đã bị xóa)"),
        L"Ngày mượn:   " + Utf8SangUtf16(phieu->GetNgayMuon().ChuyenThanhChuoi())
            + L"      Ngày phải trả:   " + Utf8SangUtf16(phieu->GetNgayPhaiTra().ChuyenThanhChuoi()),
    };
    RECT vungDong = { vung.left, vung.top, vung.right, vung.top + caoDong };
    for (const std::wstring& noiDung : dong) {
        BoVe::VietChu(hdc, noiDung, vungDong, LoaiChu::THUONG, ChuDe::CHU,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        OffsetRect(&vungDong, 0, caoDong);
    }
    OffsetRect(&vungDong, 0, ChuDe::Dp(6));
    BoVe::VietChu(hdc, trangThai, vungDong, LoaiChu::DAM, mauTrangThai);
}

bool HopThoaiTraSach::ThucHien()
{
    const int soPhieu = LaySo<int>(oSoPhieu, "Số phiếu mượn");
    if (soPhieu <= 0) {
        throw LoiNhapLieu("Số phiếu mượn phải lớn hơn 0.");
    }

    const PhieuMuon phieu = giaoTiep.GetQuanLy().TraSach(soPhieu);
    giaoTiep.DuLieuDaThayDoi();

    const NgayThang homNay = NgayThang::HomNay();
    const std::wstring soPhieuChu = std::to_wstring(soPhieu);
    giaoTiep.BaoTrangThai(L"Đã trả sách theo phiếu số " + soPhieuChu
                          + L" – cập nhật Sach.txt và PhieuMuon.txt");
    if (homNay > phieu.GetNgayPhaiTra()) {
        HopThongBao::CanhBao(GetHwnd(), L"Đã trả sách – phiếu số " + soPhieuChu,
                             L"Bạn đọc trả sách TRỄ " + std::to_wstring(phieu.GetNgayPhaiTra().SoNgayDen(homNay))
                                 + L" ngày so với ngày phải trả ("
                                 + Utf8SangUtf16(phieu.GetNgayPhaiTra().ChuyenThanhChuoi()) + L").\n\n"
                                 L"Tình trạng phiếu và tình trạng sách đã được cập nhật về 0.");
    } else {
        HopThongBao::ThongTin(GetHwnd(), L"Trả sách thành công – phiếu số " + soPhieuChu,
                              L"Sách " + Utf8SangUtf16(phieu.GetMaSach())
                                  + L" đã được trả đúng hạn.\nTình trạng phiếu và tình trạng sách đã cập nhật về 0.");
    }
    return true;
}
