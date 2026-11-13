#ifndef HOPTHOAITHEMSACH_H
#define HOPTHOAITHEMSACH_H

// ============================================================================
//  HopThoaiThemSach.h - Hộp thoại nhập thông tin sách mới (kế thừa HopThoaiCoSo)
//  Các trường: Mã sách, Năm phát hành, Tên sách, Tác giả, Nhà xuất bản, Giá bán, Số trang, Ngày nhập kho (ô chọn ngày), Tình trạng (luôn = 0).
//  Ràng buộc dữ liệu được kiểm tra trong QuanLyThuVien::ThemSach.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Ô giá / năm / số trang có kiểu ES_NUMBER: Windows chỉ cho gõ chữ số, gõ chữ cái sẽ hiện bong bóng nhắc nhở. Ô mã sách có kiểu ES_UPPERCASE: gõ "s001" tự thành "S001". Ngày nhập kho chọn từ lịch nên không thể sai định dạng.
// ----------------------------------------------------------------------------

#include "HopThoaiCoSo.h"

class HopThoaiThemSach : public HopThoaiCoSo {
private:
    HWND oMa;
    HWND oTen;
    HWND oTacGia;
    HWND oNhaXuatBan;
    HWND oGia;
    HWND oNam;
    HWND oSoTrang;
    HWND chonNgayNhap;

protected:
    void TaoNoiDung() override;
    bool ThucHien() override;

public:
    explicit HopThoaiThemSach(GiaoTiepTrang& giaoTiep);
};

#endif
