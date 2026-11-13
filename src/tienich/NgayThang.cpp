// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp NgayThang (mục lục ở NgayThang.h).
// ============================================================================

#include "NgayThang.h"
#include "CauHinh.h"
#include "TienIch.h"

#include <ctime>
#include <iomanip>
#include <sstream>

// ============================================================================
//  Constructor
// ============================================================================

NgayThang::NgayThang()
    : ngay(1), thang(1), nam(CauHinh::NAM_TOI_THIEU)
{
}

NgayThang::NgayThang(int ngay, int thang, int nam)
    : ngay(ngay), thang(thang), nam(nam)
{
}

// ============================================================================
//  Hàm tĩnh
// ============================================================================

// [GIẢI THÍCH] Hỏi đồng hồ của máy tính để biết hôm nay là ngày nào.
NgayThang NgayThang::HomNay()
{
    const std::time_t bayGio = std::time(nullptr);
    std::tm thoiGian{};
#ifdef _WIN32
    localtime_s(&thoiGian, &bayGio);
#else
    localtime_r(&bayGio, &thoiGian);
#endif
    return NgayThang(thoiGian.tm_mday, thoiGian.tm_mon + 1, thoiGian.tm_year + 1900);
}

bool NgayThang::LaNamNhuan(int nam)
{
    return (nam % 4 == 0 && nam % 100 != 0) || (nam % 400 == 0);
}

int NgayThang::SoNgayTrongThang(int thang, int nam)
{
    static const int SO_NGAY[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (thang < 1 || thang > 12) {
        return 0;
    }
    if (thang == 2 && LaNamNhuan(nam)) {
        return 29;
    }
    return SO_NGAY[thang - 1];
}

bool NgayThang::DocTuChuoi(const std::string& chuoi, NgayThang& ketQua)
{
    std::string chuan = CatKhoangTrang(chuoi);
    for (char& kyTu : chuan) {
        if (kyTu == '-') {
            kyTu = '/';
        }
    }

    std::string phan[3];
    if (!TachChuoi(chuan, '/', phan)) {
        return false;
    }

    int ngayDoc = 0;
    int thangDoc = 0;
    int namDoc = 0;
    if (!ChuyenSangSo(phan[0], ngayDoc) || !ChuyenSangSo(phan[1], thangDoc)
        || !ChuyenSangSo(phan[2], namDoc)) {
        return false;
    }

    const NgayThang ngayMoi(ngayDoc, thangDoc, namDoc);
    if (!ngayMoi.HopLe()) {
        return false;
    }

    ketQua = ngayMoi;
    return true;
}

// ============================================================================
//  Getter / kiểm tra / tính toán
// ============================================================================

int NgayThang::GetNgay() const { return ngay; }
int NgayThang::GetThang() const { return thang; }
int NgayThang::GetNam() const { return nam; }

// [GIẢI THÍCH] Tháng 1..12, ngày không vượt số ngày của tháng (tính cả năm nhuận).
bool NgayThang::HopLe() const
{
    if (nam < CauHinh::NAM_TOI_THIEU || nam > CauHinh::NAM_TOI_DA) {
        return false;
    }
    if (thang < 1 || thang > 12) {
        return false;
    }
    return ngay >= 1 && ngay <= SoNgayTrongThang(thang, nam);
}

NgayThang NgayThang::CongNgay(int soNgay) const
{
    return TuSoThuTuNgay(SoThuTuNgay() + soNgay);
}

long long NgayThang::SoNgayDen(const NgayThang& ngayKhac) const
{
    return ngayKhac.SoThuTuNgay() - SoThuTuNgay();
}

// [GIẢI THÍCH] Ngày 01/01/1970 (số thứ tự 0) là Thứ Năm (= 4). Cứ mỗi 7 ngày
// lại lặp lại thứ cũ, nên lấy (số thứ tự + 4) chia lấy dư cho 7.
int NgayThang::ThuTrongTuan() const
{
    const long long du = (SoThuTuNgay() + 4) % 7;
    return static_cast<int>(du < 0 ? du + 7 : du);
}

std::string NgayThang::ChuyenThanhChuoi() const
{
    std::ostringstream luong;
    luong << std::setfill('0') << std::setw(2) << ngay << '/'
          << std::setw(2) << thang << '/'
          << std::setw(4) << nam;
    return luong.str();
}

// ============================================================================
//  Nạp chồng toán tử so sánh
// ============================================================================

bool NgayThang::operator==(const NgayThang& khac) const
{
    return ngay == khac.ngay && thang == khac.thang && nam == khac.nam;
}

bool NgayThang::operator!=(const NgayThang& khac) const { return !(*this == khac); }
bool NgayThang::operator<(const NgayThang& khac) const { return SoThuTuNgay() < khac.SoThuTuNgay(); }
bool NgayThang::operator<=(const NgayThang& khac) const { return !(khac < *this); }
bool NgayThang::operator>(const NgayThang& khac) const { return khac < *this; }
bool NgayThang::operator>=(const NgayThang& khac) const { return !(*this < khac); }

// ============================================================================
//  Đổi ngày <-> số thứ tự ngày (thuật toán "days from civil" của lịch Gregory)
// ============================================================================

// [GIẢI THÍCH] Coi năm bắt đầu từ tháng 3 để ngày 29/02 (nếu có) rơi vào cuối
// năm; nhờ vậy công thức đếm ngày không phải xử lý riêng năm nhuận.
long long NgayThang::SoThuTuNgay() const
{
    const long long namTinh = thang <= 2 ? nam - 1 : nam;
    const long long chuKy = (namTinh >= 0 ? namTinh : namTinh - 399) / 400;
    const long long namTrongChuKy = namTinh - chuKy * 400;
    const long long thangTinh = thang > 2 ? thang - 3 : thang + 9;
    const long long ngayTrongNam = (153 * thangTinh + 2) / 5 + ngay - 1;
    const long long ngayTrongChuKy =
        namTrongChuKy * 365 + namTrongChuKy / 4 - namTrongChuKy / 100 + ngayTrongNam;
    return chuKy * 146097 + ngayTrongChuKy - 719468;
}

NgayThang NgayThang::TuSoThuTuNgay(long long soThuTu)
{
    const long long z = soThuTu + 719468;
    const long long chuKy = (z >= 0 ? z : z - 146096) / 146097;
    const long long ngayTrongChuKy = z - chuKy * 146097;
    const long long namTrongChuKy = (ngayTrongChuKy - ngayTrongChuKy / 1460
                                     + ngayTrongChuKy / 36524 - ngayTrongChuKy / 146096) / 365;
    const long long ngayTrongNam =
        ngayTrongChuKy - (365 * namTrongChuKy + namTrongChuKy / 4 - namTrongChuKy / 100);
    const long long thangTinh = (5 * ngayTrongNam + 2) / 153;
    const long long ngayKetQua = ngayTrongNam - (153 * thangTinh + 2) / 5 + 1;
    const long long thangKetQua = thangTinh < 10 ? thangTinh + 3 : thangTinh - 9;
    const long long namKetQua = namTrongChuKy + chuKy * 400 + (thangKetQua <= 2 ? 1 : 0);
    return NgayThang(static_cast<int>(ngayKetQua), static_cast<int>(thangKetQua),
                     static_cast<int>(namKetQua));
}
