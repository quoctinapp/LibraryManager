#ifndef CAUHINH_H
#define CAUHINH_H

// ============================================================================
//  CauHinh.h - Các hằng số cấu hình dùng chung cho toàn chương trình
//      - Tên thư mục / tên các file dữ liệu.
//      - Quy tắc nghiệp vụ: số lần đăng nhập, số ngày mượn, tình trạng.
//      - Giới hạn độ dài và ràng buộc giá trị của dữ liệu.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là "bảng nội quy" của thư viện, gom về MỘT chỗ.
//  Muốn đổi số ngày mượn từ 7 thành 14, hay đổi số lần nhập sai mật khẩu, chỉ cần sửa ở file này, không phải đi tìm từng con số rải rác trong code (tránh "số ma thuật").
// ----------------------------------------------------------------------------

#include <cstddef>

namespace CauHinh {

// ---- Thư mục và tên các file dữ liệu ----
constexpr const char* THU_MUC_DU_LIEU = "data";
constexpr const char* FILE_TAI_KHOAN = "Admin.txt";
constexpr const char* FILE_SACH = "Sach.txt";
constexpr const char* FILE_BAN_DOC = "BanDoc.txt";
constexpr const char* FILE_PHIEU_MUON = "PhieuMuon.txt";

// ---- Định dạng file: các trường trên một dòng ngăn nhau bởi dấu | ----
constexpr char KY_TU_PHAN_CACH = '|';

// ---- Quy tắc nghiệp vụ ----
constexpr int SO_LAN_DANG_NHAP_TOI_DA = 3;
constexpr int SO_NGAY_MUON = 7;
constexpr int SACH_CHUA_MUON = 0;       // tình trạng sách khi chưa có ai mượn
constexpr int PHIEU_DA_TRA = 0;         // tình trạng phiếu mượn đã trả
constexpr int PHIEU_DANG_MUON = 1;      // tình trạng phiếu mượn mới tạo
constexpr int SO_THANG_THONG_KE = 6;    // biểu đồ lượt mượn của 6 tháng gần nhất

// ---- Giới hạn độ dài (tính theo SỐ KÝ TỰ, chữ có dấu tính là 1 ký tự) ----
constexpr std::size_t DO_DAI_MA_TOI_DA = 10;
constexpr std::size_t DO_DAI_TEN_SACH_TOI_DA = 50;
constexpr std::size_t DO_DAI_TEN_TOI_DA = 30;         // tác giả, nhà xuất bản
constexpr std::size_t DO_DAI_HO_TEN_TOI_DA = 40;      // họ tên bạn đọc
constexpr std::size_t DO_DAI_TAI_KHOAN_TOI_DA = 20;
constexpr std::size_t DO_DAI_MAT_KHAU_TOI_DA = 20;

// ---- Ràng buộc giá trị ----
constexpr long long GIA_BAN_TOI_THIEU = 1;
constexpr long long GIA_BAN_TOI_DA = 100000000;
constexpr int NAM_PHAT_HANH_TOI_THIEU = 1900;
constexpr int SO_TRANG_TOI_THIEU = 1;
constexpr int SO_TRANG_TOI_DA = 10000;
constexpr int NAM_TOI_THIEU = 1900;
constexpr int NAM_TOI_DA = 9999;

}  // namespace CauHinh

#endif
