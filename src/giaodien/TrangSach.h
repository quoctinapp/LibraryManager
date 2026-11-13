#ifndef TRANGSACH_H
#define TRANGSACH_H

// ============================================================================
//  TrangSach.h - Trang "Quản lý sách" (kế thừa TrangNoiDung)
//   - Hiển thị thông tin sách: bảng 9 cột, bấm tiêu đề cột để sắp xếp.
//   - Tìm kiếm tức thì theo mã / tên / tác giả / NXB (không phân biệt dấu).
//   - Thêm sách, Xóa sách (chọn dòng hoặc nhập mã), Cho mượn, Xuất CSV.
//   - Nhấp đúp một dòng để xem chi tiết; phím Delete để xóa dòng đang chọn.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mỗi lần LamMoi(): lấy danh sách sách khớp từ khóa (DanhSachLienKet<Sach>),
//  sắp xếp bằng merge sort theo cột đang chọn, rồi đổ từng dòng vào bảng.
//  Cột tình trạng tô xanh nếu = 0 (còn trong kho), tô đỏ nếu đang được mượn.
// ----------------------------------------------------------------------------

#include "TrangNoiDung.h"

class TrangSach : public TrangNoiDung {
private:
    HWND nutThem;
    HWND nutXoa;
    HWND nutMuon;
    HWND nutXuat;
    DanhSachLienKet<Sach> dsHienThi;    // các sách đang hiện trong bảng (đã lọc + sắp xếp)

    static int SoSanh(const Sach& a, const Sach& b, int cot);
    void ThemSach();
    void XoaSach();
    void ChoMuon();
    void XuatBaoCao();
    void XemChiTiet();

protected:
    void TaoDieuKhien() override;
    void BoTri(int rong, int cao) override;
    void XuLyLenh(int id, int maThongBao) override;
    bool XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua) override;

public:
    explicit TrangSach(GiaoTiepTrang& giaoTiep);

    std::wstring GetTieuDe() const override;
    std::wstring GetMoTa() const override;
    void LamMoi() override;
};

#endif
