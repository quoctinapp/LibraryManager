#ifndef HOPTHOAITHEMBANDOC_H
#define HOPTHOAITHEMBANDOC_H

// ============================================================================
//  HopThoaiThemBanDoc.h - Hộp thoại đăng ký bạn đọc mới (kế thừa HopThoaiCoSo)
//  Các trường: Mã bạn đọc (gợi ý sẵn mã kế tiếp), Ngày đăng ký, Họ tên.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mã bạn đọc được điền sẵn mã kế tiếp (đang có BD008 thì gợi ý BD009) để nhân viên đỡ phải nghĩ, nhưng vẫn sửa được. Trùng mã sẽ bị từ chối.
// ----------------------------------------------------------------------------

#include "HopThoaiCoSo.h"

class HopThoaiThemBanDoc : public HopThoaiCoSo {
private:
    HWND oMa;
    HWND chonNgayDangKy;
    HWND oHoTen;

protected:
    void TaoNoiDung() override;
    bool ThucHien() override;

public:
    explicit HopThoaiThemBanDoc(GiaoTiepTrang& giaoTiep);
};

#endif
