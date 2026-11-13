#ifndef HOPTHOAIMUONSACH_H
#define HOPTHOAIMUONSACH_H

// ============================================================================
//  HopThoaiMuonSach.h - Hộp thoại tạo phiếu mượn (kế thừa HopThoaiCoSo)
//      - Nhập (hoặc chọn từ danh sách) MÃ SÁCH và MÃ BẠN ĐỌC, đúng như đề bài.
//      - Hiện trước: số phiếu sẽ tạo, ngày mượn (hôm nay), ngày phải trả (+7).
//  Điều kiện mượn được kiểm tra trong QuanLyThuVien::MuonSach.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Hộp chọn kiểu CBS_DROPDOWN vừa cho GÕ mã trực tiếp, vừa cho thả xuống để chọn ("S001 – Cấu trúc dữ liệu..."). Chương trình chỉ lấy phần mã ở đầu.
//  Danh sách sách chỉ gồm sách đang có sẵn (tình trạng 0); nếu gõ mã một cuốn đang được mượn, thủ thư sẽ từ chối và báo lỗi.
// ----------------------------------------------------------------------------

#include "HopThoaiCoSo.h"
#include <string>

class HopThoaiMuonSach : public HopThoaiCoSo {
private:
    std::string maSachChonSan;
    HWND hopSach;
    HWND hopBanDoc;

protected:
    void TaoNoiDung() override;
    bool ThucHien() override;

public:
    HopThoaiMuonSach(GiaoTiepTrang& giaoTiep, const std::string& maSachChonSan);
};

#endif
