// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của TrangTongQuan (mục lục ở TrangTongQuan.h).
//  Bố cục:   [thẻ][thẻ][thẻ][thẻ][thẻ]
//            [ biểu đồ cột (62%) ][ biểu đồ vòng ]
//            [ bảng phiếu quá hạn ..................... ]
// ============================================================================

#include "TrangTongQuan.h"
#include "BoVe.h"
#include "HopThoaiTraSach.h"
#include "MaDieuKhien.h"
#include <cmath>

TrangTongQuan::TrangTongQuan(GiaoTiepTrang& giaoTiep) : TrangNoiDung(giaoTiep), thongKe{}, vungThe{}, vungBieuDoCot{}, vungBieuDoVong{} {}

std::wstring TrangTongQuan::GetTieuDe() const {
    return L"Tổng quan";
}

std::wstring TrangTongQuan::GetMoTa() const {
    return L"Tình hình thư viện hôm nay";
}

void TrangTongQuan::TaoDieuKhien() {
    bang.Tao(hwnd, MA_BANG);
    bang.ThemCot(L"Số phiếu", 80, LVCFMT_CENTER);
    bang.ThemCot(L"Bạn đọc", 200);
    bang.ThemCot(L"Sách", 260);
    bang.ThemCot(L"Ngày phải trả", 120, LVCFMT_CENTER);
    bang.ThemCot(L"Số ngày trễ", 120);
    bang.DatCotNhanManh(4);
    bang.DatChuKhiRong(L"Tuyệt vời! Không có phiếu mượn nào quá hạn.");
}

void TrangTongQuan::BoTri(int rong, int cao) {
    const int le = ChuDe::Dp(24);
    const int khoang = ChuDe::Dp(16);
    int y = ChuDe::Dp(20);

    // Hàng 1: 5 thẻ thống kê
    const int caoThe = ChuDe::Dp(112);
    const int rongThe = (rong - 2 * le - 4 * khoang) / 5;
    for (int i = 0; i < 5; ++i) {
        const int x = le + i * (rongThe + khoang);
        vungThe[i] = { x, y, x + rongThe, y + caoThe };
    }

    y += caoThe + ChuDe::Dp(18);

    // Hàng 2: biểu đồ cột + biểu đồ vòng
    const int caoBieuDo = ChuDe::Dp(236);
    const int rongCot = (rong - 2 * le - khoang) * 62 / 100;
    vungBieuDoCot = { le, y, le + rongCot, y + caoBieuDo };
    vungBieuDoVong = { le + rongCot + khoang, y, rong - le, y + caoBieuDo };
    y += caoBieuDo + ChuDe::Dp(18);

    // Hàng 3: thẻ chứa bảng phiếu quá hạn (có dòng tiêu đề cao 44)
    vungTheBang = { le, y, rong - le, cao - ChuDe::Dp(20) };
    if (vungTheBang.bottom < vungTheBang.top + ChuDe::Dp(120)) {
        vungTheBang.bottom = vungTheBang.top + ChuDe::Dp(120);
    }

    const int dem = ChuDe::Dp(6);
    bang.DatViTri(vungTheBang.left + dem, vungTheBang.top + ChuDe::Dp(44), vungTheBang.right - vungTheBang.left - 2 * dem, vungTheBang.bottom - vungTheBang.top - ChuDe::Dp(44) - dem);
}

void TrangTongQuan::LamMoi() {
    const QuanLyThuVien& quanLy = giaoTiep.GetQuanLy();
    const NgayThang homNay = NgayThang::HomNay();
    thongKe = quanLy.LayThongKe(homNay);

    // Phiếu quá hạn, trễ lâu nhất xếp trên cùng (sắp xếp trộn trên danh sách liên kết)
    DanhSachLienKet<PhieuMuon> quaHan = quanLy.GetDanhSachPhieuMuon().Loc([&homNay](const PhieuMuon& p) { return p.QuaHan(homNay); });
    quaHan.SapXep([](const PhieuMuon& a, const PhieuMuon& b) { return a.GetNgayPhaiTra() < b.GetNgayPhaiTra(); });

    bang.BatDauCapNhat();
    for (const PhieuMuon& phieu : quaHan) {
        const BanDoc* banDoc = quanLy.TimBanDoc(phieu.GetMaBanDoc());
        const Sach* sach = quanLy.TimSach(phieu.GetMaSach());
        bang.ThemDong({ std::to_wstring(phieu.GetSoPhieu()), Utf8SangUtf16(phieu.GetMaBanDoc()) + L" – " + (banDoc != nullptr ? Utf8SangUtf16(banDoc->GetHoTen()) : L""), Utf8SangUtf16(phieu.GetMaSach()) + L" – " + (sach != nullptr ? Utf8SangUtf16(sach->GetTenSach()) : L""), Utf8SangUtf16(phieu.GetNgayPhaiTra().ChuyenThanhChuoi()), L"Trễ " + std::to_wstring(phieu.GetNgayPhaiTra().SoNgayDen(homNay)) + L" ngày" }, ChuDe::DO);
    }

    bang.KetThucCapNhat();
    InvalidateRect(hwnd, nullptr, FALSE);
}

bool TrangTongQuan::XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) {
    if (thongBao->hwndFrom == bang.GetHwnd() && thongBao->code == NM_DBLCLK) {
        int soPhieu = 0;
        if (ChuyenSangSo(Utf16SangUtf8(bang.LayChuDongChon(0)), soPhieu)) {
            HopThoaiTraSach hopThoai(giaoTiep, soPhieu);
            hopThoai.ChayModal();
        }

        ketQua = 0;
        return true;
    }

    return false;
}

// ============================================================================
//  Vẽ
// ============================================================================

void TrangTongQuan::Ve(HDC hdc, const RECT&) {
    const int tongSach = thongKe.tongSoSach;
    VeTheThongKe(hdc, vungThe[0], L"TỔNG SỐ SÁCH", tongSach, L"đầu sách trong thư viện", ChuDe::BT_THU_VIEN, ChuDe::NHAN, ChuDe::NHAN_NHAT);
    VeTheThongKe(hdc, vungThe[1], L"CÒN TRONG KHO", thongKe.sachCoSan, L"sẵn sàng cho mượn", ChuDe::BT_DONG_Y, ChuDe::XANH_LA, ChuDe::XANH_LA_NHAT);
    VeTheThongKe(hdc, vungThe[2], L"ĐANG MƯỢN", thongKe.phieuDangMuon, L"phiếu chưa trả sách", ChuDe::BT_TOI, ChuDe::CAM, ChuDe::CAM_NHAT);
    VeTheThongKe(hdc, vungThe[3], L"PHIẾU QUÁ HẠN", thongKe.phieuQuaHan, thongKe.phieuQuaHan > 0 ? L"cần nhắc bạn đọc trả" : L"không có phiếu trễ", ChuDe::BT_CANH_BAO, ChuDe::DO, ChuDe::DO_NHAT);
    VeTheThongKe(hdc, vungThe[4], L"BẠN ĐỌC", thongKe.soBanDoc, L"đã đăng ký thẻ", ChuDe::BT_NGUOI, ChuDe::TIM, ChuDe::TIM_NHAT);

    VeBieuDoCot(hdc);
    VeBieuDoVong(hdc);

    // Thẻ bảng phiếu quá hạn + tiêu đề
    BoVe::ToBoGoc(hdc, vungTheBang, ChuDe::Dp(10), ChuDe::THE, ChuDe::VIEN);
    const RECT vungTieuDe = { vungTheBang.left + ChuDe::Dp(20), vungTheBang.top,
                              vungTheBang.right - ChuDe::Dp(20), vungTheBang.top + ChuDe::Dp(44) };
    BoVe::VietChu(hdc, L"Phiếu mượn quá hạn", vungTieuDe, LoaiChu::DAM, ChuDe::CHU);
    BoVe::VietChu(hdc, L"Nhấp đúp một phiếu để trả sách", vungTieuDe, LoaiChu::NHO, ChuDe::CHU_PHU, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
}

void TrangTongQuan::VeTheThongKe(HDC hdc, const RECT& vung, const std::wstring& nhan, int giaTri, const std::wstring& ghiChu, wchar_t bieuTuong, COLORREF mau, COLORREF mauNhat) const {
    BoVe::ToBoGoc(hdc, vung, ChuDe::Dp(12), ChuDe::THE, ChuDe::VIEN);
    // Vạch màu nhấn ở cạnh trái của thẻ
    const RECT vach = { vung.left, vung.top + ChuDe::Dp(18), vung.left + ChuDe::Dp(4), vung.bottom - ChuDe::Dp(18) };
    BoVe::ToNen(hdc, vach, mau);

    const int le = ChuDe::Dp(20);
    const RECT vungNhan = { vung.left + le, vung.top + ChuDe::Dp(14), vung.right - ChuDe::Dp(60),
                            vung.top + ChuDe::Dp(34) };
    BoVe::VietChu(hdc, nhan, vungNhan, LoaiChu::NHO_DAM, ChuDe::CHU_PHU, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    const RECT vungSo = { vung.left + le, vung.top + ChuDe::Dp(34), vung.right - ChuDe::Dp(12),
                          vung.top + ChuDe::Dp(76) };
    BoVe::VietChu(hdc, std::to_wstring(giaTri), vungSo, LoaiChu::SO_LON, ChuDe::CHU);
    const RECT vungGhiChu = { vung.left + le, vung.top + ChuDe::Dp(76), vung.right - ChuDe::Dp(12),
                              vung.bottom - ChuDe::Dp(12) };
    BoVe::VietChu(hdc, ghiChu, vungGhiChu, LoaiChu::NHO, mau, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // Biểu tượng trong hình tròn màu nhạt ở góc phải
    const int banKinh = ChuDe::Dp(20);
    const int tamX = vung.right - ChuDe::Dp(16) - banKinh;
    const int tamY = vung.top + ChuDe::Dp(16) + banKinh;
    BoVe::ToHinhTron(hdc, tamX, tamY, banKinh, mauNhat);
    const RECT vungBieuTuong = { tamX - banKinh, tamY - banKinh, tamX + banKinh, tamY + banKinh };
    BoVe::VeBieuTuong(hdc, bieuTuong, vungBieuTuong, LoaiChu::BIEU_TUONG, mau);
}

// [GIẢI THÍCH] Biểu đồ cột: trục đứng chia 4 vạch; mỗi tháng là một cột, cột của tháng hiện tại tô màu đậm, các tháng trước tô màu nhạt hơn.
void TrangTongQuan::VeBieuDoCot(HDC hdc) const {
    const RECT& the = vungBieuDoCot;
    BoVe::ToBoGoc(hdc, the, ChuDe::Dp(12), ChuDe::THE, ChuDe::VIEN);

    int tong = 0;
    int lonNhat = 1;
    for (int so : thongKe.soPhieuTheoThang) {
        tong += so;
        if (so > lonNhat) {
            lonNhat = so;
        }
    }

    const RECT vungTieuDe = { the.left + ChuDe::Dp(20), the.top + ChuDe::Dp(12), the.right - ChuDe::Dp(20),
                              the.top + ChuDe::Dp(40) };
    BoVe::VietChu(hdc, L"Lượt mượn 6 tháng gần nhất", vungTieuDe, LoaiChu::DAM, ChuDe::CHU);
    BoVe::VietChu(hdc, L"Tổng: " + std::to_wstring(tong) + L" lượt", vungTieuDe, LoaiChu::NHO, ChuDe::CHU_PHU, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    // Thang chia: làm tròn lên bội số của 4 cho đẹp
    const int SO_VACH = 4;
    const int dinhThang = ((lonNhat + SO_VACH - 1) / SO_VACH) * SO_VACH;
    const RECT vungVe = { the.left + ChuDe::Dp(48), the.top + ChuDe::Dp(56), the.right - ChuDe::Dp(20),
                          the.bottom - ChuDe::Dp(36) };
    const int caoVe = vungVe.bottom - vungVe.top;

    for (int i = 0; i <= SO_VACH; ++i) {
        const int y = vungVe.bottom - caoVe * i / SO_VACH;
        BoVe::VeDuong(hdc, vungVe.left, y, vungVe.right, y, i == 0 ? ChuDe::VIEN_DAM : ChuDe::VIEN);
        const RECT vungNhan = { the.left + ChuDe::Dp(8), y - ChuDe::Dp(10), vungVe.left - ChuDe::Dp(10),
                                y + ChuDe::Dp(10) };
        BoVe::VietChu(hdc, std::to_wstring(dinhThang * i / SO_VACH), vungNhan, LoaiChu::NHO, ChuDe::CHU_PHU, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }

    const int soCot = CauHinh::SO_THANG_THONG_KE;
    const int rongO = (vungVe.right - vungVe.left) / soCot;
    const int rongCot = rongO * 45 / 100;
    for (int i = 0; i < soCot; ++i) {
        const int giaTri = thongKe.soPhieuTheoThang[i];
        const int xGiua = vungVe.left + rongO * i + rongO / 2;
        const int caoCot = caoVe * giaTri / dinhThang;
        const bool laThangNay = (i == soCot - 1);
        const COLORREF mau = laThangNay ? ChuDe::NHAN : ChuDe::TronMau(ChuDe::NHAN, ChuDe::THE, 0.55);

        if (caoCot > 0) {
            const RECT cot = { xGiua - rongCot / 2, vungVe.bottom - caoCot, xGiua + rongCot / 2, vungVe.bottom + 1 };
            BoVe::ToBoGoc(hdc, cot, ChuDe::Dp(5), mau, mau);
            const RECT vungNhanCot = { cot.left - ChuDe::Dp(10), cot.top - ChuDe::Dp(22), cot.right + ChuDe::Dp(10),
                                       cot.top - ChuDe::Dp(2) };
            BoVe::VietChu(hdc, std::to_wstring(giaTri), vungNhanCot, LoaiChu::NHO_DAM, laThangNay ? ChuDe::NHAN : ChuDe::CHU_PHU, DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
        }

        wchar_t nhanThang[16];
        swprintf(nhanThang, 16, L"T%d/%02d", thongKe.thang[i], thongKe.nam[i] % 100);
        const RECT vungThang = { xGiua - rongO / 2, vungVe.bottom + ChuDe::Dp(6), xGiua + rongO / 2,
                                 vungVe.bottom + ChuDe::Dp(28) };
        BoVe::VietChu(hdc, nhanThang, vungThang, laThangNay ? LoaiChu::NHO_DAM : LoaiChu::NHO, laThangNay ? ChuDe::CHU : ChuDe::CHU_PHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void TrangTongQuan::VeBieuDoVong(HDC hdc) const {
    const RECT& the = vungBieuDoVong;
    BoVe::ToBoGoc(hdc, the, ChuDe::Dp(12), ChuDe::THE, ChuDe::VIEN);
    const RECT vungTieuDe = { the.left + ChuDe::Dp(20), the.top + ChuDe::Dp(12), the.right - ChuDe::Dp(20),
                              the.top + ChuDe::Dp(40) };
    BoVe::VietChu(hdc, L"Tình trạng kho sách", vungTieuDe, LoaiChu::DAM, ChuDe::CHU);

    const double tiLe = thongKe.tongSoSach > 0 ? static_cast<double>(thongKe.sachCoSan) / thongKe.tongSoSach : 0.0;
    const int canh = ChuDe::Dp(150);
    const int x = the.left + ChuDe::Dp(24);
    const int y = the.top + ChuDe::Dp(56) + (the.bottom - the.top - ChuDe::Dp(56) - canh - ChuDe::Dp(12)) / 2;
    const RECT vungVong = { x, y, x + canh, y + canh };
    BoVe::VeDonut(hdc, vungVong, tiLe, ChuDe::XANH_LA, ChuDe::CAM, ChuDe::THE, ChuDe::Dp(22));

    const int phanTram = static_cast<int>(std::lround(tiLe * 100));
    RECT vungSo = { x, y + canh / 2 - ChuDe::Dp(26), x + canh, y + canh / 2 + ChuDe::Dp(8) };
    BoVe::VietChu(hdc, std::to_wstring(phanTram) + L"%", vungSo, LoaiChu::TIEU_DE_LON, ChuDe::CHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    RECT vungChu = { x, y + canh / 2 + ChuDe::Dp(6), x + canh, y + canh / 2 + ChuDe::Dp(26) };
    BoVe::VietChu(hdc, L"còn trong kho", vungChu, LoaiChu::NHO, ChuDe::CHU_PHU, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Chú thích bên phải
    struct ChuThich {
        COLORREF mau;
        std::wstring chu;
        int giaTri;
    };
    const ChuThich dsChuThich[] = {
        { ChuDe::XANH_LA, L"Còn trong kho", thongKe.sachCoSan },
        { ChuDe::CAM, L"Đang được mượn", thongKe.sachDangMuon },
        { ChuDe::VIEN_DAM, L"Tổng số sách", thongKe.tongSoSach },
    };
    // 3 dòng chú thích chia đều theo chiều cao của vòng tròn
    const int xChu = x + canh + ChuDe::Dp(24);
    const int buocDong = canh / 3;
    int yChu = y + (buocDong - ChuDe::Dp(46)) / 2;
    for (const ChuThich& ct : dsChuThich) {
        BoVe::ToHinhTron(hdc, xChu + ChuDe::Dp(6), yChu + ChuDe::Dp(10), ChuDe::Dp(6), ct.mau);
        const RECT vungTen = { xChu + ChuDe::Dp(20), yChu, the.right - ChuDe::Dp(20), yChu + ChuDe::Dp(20) };
        BoVe::VietChu(hdc, ct.chu, vungTen, LoaiChu::NHO, ChuDe::CHU_PHU);
        const RECT vungGiaTri = { xChu + ChuDe::Dp(20), yChu + ChuDe::Dp(20), the.right - ChuDe::Dp(20),
                                  yChu + ChuDe::Dp(46) };
        BoVe::VietChu(hdc, std::to_wstring(ct.giaTri) + L" cuốn", vungGiaTri, LoaiChu::TIEU_DE, ChuDe::CHU);
        yChu += buocDong;
    }
}
