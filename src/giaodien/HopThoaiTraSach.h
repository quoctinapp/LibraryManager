#ifndef HOPTHOAITRASACH_H
#define HOPTHOAITRASACH_H

// ============================================================================
//  HopThoaiTraSach.h - Hộp thoại trả sách theo SỐ PHIẾU MƯỢN (kế thừa HopThoaiCoSo)
//  Gõ số phiếu -> hộp thoại hiện ngay thông tin phiếu (bạn đọc, sách, hạn trả, số ngày trễ hoặc "phiếu đã trả") để nhân viên kiểm tra trước khi xác nhận.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mỗi lần nội dung ô số phiếu thay đổi (thông báo EN_CHANGE), hộp thoại tra phiếu trong danh sách và vẽ lại vùng "xem trước". Việc trả sách thật sự (đổi tình trạng phiếu và sách về 0, ghi file) do QuanLyThuVien::TraSach làm.
// ----------------------------------------------------------------------------

#include "HopThoaiCoSo.h"

class HopThoaiTraSach : public HopThoaiCoSo {
private:
    int soPhieuChonSan;
    HWND oSoPhieu;
    RECT vungXemTruoc;

protected:
    void TaoNoiDung() override;
    bool ThucHien() override;
    void XuLyLenhThem(int id, int maThongBao) override;
    void VeThem(HDC hdc) override;

public:
    HopThoaiTraSach(GiaoTiepTrang& giaoTiep, int soPhieuChonSan);
};

#endif
