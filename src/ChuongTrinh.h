#ifndef CHUONGTRINH_H
#define CHUONGTRINH_H

// ============================================================================
//  ChuongTrinh.h - Lớp điều phối toàn bộ chương trình
//   1) Khởi tạo thư viện giao diện của Windows, phông chữ, lớp cửa sổ.
//   2) Chuẩn bị thư mục dữ liệu (tự giải nén dữ liệu mẫu nhúng trong exe).
//   3) Nạp dữ liệu từ 4 file; có dòng lỗi thì hiện cảnh báo.
//   4) Màn hình đăng nhập -> cửa sổ chính. Đăng xuất thì quay lại đăng nhập.
//  main() chỉ tạo một ChuongTrinh và gọi Chay() -> không có biến toàn cục.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là "người đạo diễn": không tự vẽ hay tính toán gì, chỉ sắp xếp thứ tự
//  các màn: chuẩn bị sân khấu -> mời đăng nhập -> mở cửa sổ chính -> hạ màn.
// ----------------------------------------------------------------------------

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

class ChuongTrinh {
public:
    int Chay(HINSTANCE hInstance);
};

#endif
