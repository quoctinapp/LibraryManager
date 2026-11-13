#ifndef HOPTHOAINHAPMA_H
#define HOPTHOAINHAPMA_H

// ============================================================================
//  HopThoaiNhapMa.h - Hộp thoại nhỏ chỉ có MỘT ô để nhập mã (kế thừa HopThoaiCoSo)
//  Dùng cho chức năng "Xóa sách: nhập mã sách để xóa" và "Xóa bạn đọc" khi
//  người dùng chưa chọn dòng nào trong bảng.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Hộp thoại này không tự xóa gì cả: nó chỉ kiểm tra mã có đúng dạng (chữ + số)
//  rồi trả mã về cho trang đã mở nó. Trang sẽ tự hỏi xác nhận và xóa.
// ----------------------------------------------------------------------------

#include "HopThoaiCoSo.h"

#include <string>

class HopThoaiNhapMa : public HopThoaiCoSo {
private:
    std::wstring nhanTruong;
    HWND oMa;
    std::string maDaNhap;

protected:
    void TaoNoiDung() override;
    bool ThucHien() override;

public:
    HopThoaiNhapMa(GiaoTiepTrang& giaoTiep, const std::wstring& tieuDe, const std::wstring& moTa,
                   const std::wstring& nhanTruong, wchar_t bieuTuong, const std::wstring& chuNut);

    std::string GetMa() const { return maDaNhap; }
};

#endif
