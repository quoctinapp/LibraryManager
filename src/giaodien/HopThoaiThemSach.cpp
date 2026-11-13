// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của HopThoaiThemSach (mục lục ở HopThoaiThemSach.h).
// ============================================================================

#include "HopThoaiThemSach.h"
#include "CauHinh.h"
#include "MaDieuKhien.h"

HopThoaiThemSach::HopThoaiThemSach(GiaoTiepTrang& giaoTiep) : HopThoaiCoSo(giaoTiep, L"Thêm sách mới", L"Sách mới luôn có tình trạng 0 và được ghi nối vào cuối file Sach.txt", ChuDe::BT_THEM, L"Thêm sách"), oMa(nullptr), oTen(nullptr), oTacGia(nullptr), oNhaXuatBan(nullptr), oGia(nullptr), oNam(nullptr), oSoTrang(nullptr), chonNgayNhap(nullptr) {}

void HopThoaiThemSach::TaoNoiDung() {
    oMa = ThemONhap(L"Mã sách (chữ và số, tối đa 10 ký tự)", MA_TRUONG_1, ES_UPPERCASE, static_cast<int>(CauHinh::DO_DAI_MA_TOI_DA), ViTriTruong::TRAI);
    oNam = ThemONhap(L"Năm phát hành", MA_TRUONG_2, ES_NUMBER, 4, ViTriTruong::PHAI);
    oTen = ThemONhap(L"Tên sách", MA_TRUONG_3, 0, static_cast<int>(CauHinh::DO_DAI_TEN_SACH_TOI_DA));
    oTacGia = ThemONhap(L"Tác giả", MA_TRUONG_4, 0, static_cast<int>(CauHinh::DO_DAI_TEN_TOI_DA), ViTriTruong::TRAI);
    oNhaXuatBan = ThemONhap(L"Nhà xuất bản", MA_TRUONG_5, 0, static_cast<int>(CauHinh::DO_DAI_TEN_TOI_DA), ViTriTruong::PHAI);
    oGia = ThemONhap(L"Giá bán (đồng)", MA_TRUONG_6, ES_NUMBER, 9, ViTriTruong::TRAI);
    oSoTrang = ThemONhap(L"Số trang", MA_TRUONG_7, ES_NUMBER, 5, ViTriTruong::PHAI);
    chonNgayNhap = ThemChonNgay(L"Ngày nhập kho", MA_TRUONG_8, NgayThang::HomNay(), ViTriTruong::TRAI);
    ThemOThongTin(L"Tình trạng", L"0  –  Chưa có người mượn", ChuDe::XANH_LA, ViTriTruong::PHAI);
}

bool HopThoaiThemSach::ThucHien() {
    // Đọc lần lượt từng trường (trường sai đầu tiên sẽ được báo lỗi)
    const int namPhatHanh = LaySo<int>(oNam, "Năm phát hành");
    const long long giaBan = LaySo<long long>(oGia, "Giá bán");
    const int soTrang = LaySo<int>(oSoTrang, "Số trang");
    const Sach sachMoi(VietHoa(LayChuUtf8(oMa)), LayChuUtf8(oTen), LayChuUtf8(oTacGia), LayChuUtf8(oNhaXuatBan), giaBan, namPhatHanh, soTrang, LayNgay(chonNgayNhap));

    giaoTiep.GetQuanLy().ThemSach(sachMoi);     // kiểm tra ràng buộc + ghi nối Sach.txt
    giaoTiep.DuLieuDaThayDoi();
    giaoTiep.BaoTrangThai(L"Đã thêm sách " + Utf8SangUtf16(sachMoi.GetMaSach()) + L" – " + Utf8SangUtf16(sachMoi.GetTenSach()) + L" (ghi nối vào cuối Sach.txt)");
    return true;
}
