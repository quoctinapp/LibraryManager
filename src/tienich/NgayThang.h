#ifndef NGAYTHANG_H
#define NGAYTHANG_H

// ============================================================================
//  NgayThang.h - Lớp biểu diễn ngày / tháng / năm
//   - Lấy ngày hiện tại của hệ thống (ngày mượn sách).
//   - Cộng ngày (ngày phải trả = ngày mượn + 7), kể cả qua tháng / năm nhuận.
//   - Kiểm tra ngày hợp lệ, so sánh hai ngày, đổi qua lại với chuỗi dd/mm/yyyy.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Để cộng ngày cho đúng (31/12 + 7 = 07/01 năm sau, 28/02 năm nhuận + 1 =
//  29/02), lớp này đổi ngày thành "số thứ tự của ngày" (đếm từ 01/01/1970),
//  cộng / trừ trên con số đó, rồi đổi ngược lại thành ngày / tháng / năm.
//  Nạp chồng toán tử (==, <, >...) giúp viết "ngayNhap > homNay" như so sánh số.
// ----------------------------------------------------------------------------

#include <string>

class NgayThang {
private:
    int ngay;
    int thang;
    int nam;

    // Số thứ tự của ngày tính từ 01/01/1970 (thuật toán lịch Gregory)
    long long SoThuTuNgay() const;
    static NgayThang TuSoThuTuNgay(long long soThuTu);

public:
    NgayThang();
    NgayThang(int ngay, int thang, int nam);

    static NgayThang HomNay();
    static bool LaNamNhuan(int nam);
    static int SoNgayTrongThang(int thang, int nam);

    // Đọc ngày từ chuỗi "dd/mm/yyyy" (chấp nhận cả dấu '-'). Sai -> false.
    static bool DocTuChuoi(const std::string& chuoi, NgayThang& ketQua);

    int GetNgay() const;
    int GetThang() const;
    int GetNam() const;

    bool HopLe() const;
    NgayThang CongNgay(int soNgay) const;
    long long SoNgayDen(const NgayThang& ngayKhac) const;   // = ngayKhac - ngày này
    int ThuTrongTuan() const;                               // 0 = Chủ nhật, 1 = Thứ Hai ... 6 = Thứ Bảy
    std::string ChuyenThanhChuoi() const;                   // "dd/mm/yyyy"

    bool operator==(const NgayThang& khac) const;
    bool operator!=(const NgayThang& khac) const;
    bool operator<(const NgayThang& khac) const;
    bool operator<=(const NgayThang& khac) const;
    bool operator>(const NgayThang& khac) const;
    bool operator>=(const NgayThang& khac) const;
};

#endif
