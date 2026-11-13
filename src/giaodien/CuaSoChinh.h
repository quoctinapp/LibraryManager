#ifndef CUASOCHINH_H
#define CUASOCHINH_H

// ============================================================================
//  CuaSoChinh.h - Cửa sổ chính của chương trình (kế thừa CuaSoCoSo + GiaoTiepTrang)
//   - Thanh bên trái (sidebar): Tổng quan, Quản lý sách, Quản lý phiếu mượn,
//     Bạn đọc, và nút Đăng xuất ở dưới cùng.
//   - Đầu trang: tên trang đang mở, mô tả, ngày hôm nay.
//   - Vùng nội dung: hiện 1 trong 4 trang (TrangNoiDung).
//   - Thanh trạng thái: báo kết quả thao tác gần nhất + thư mục dữ liệu.
//   - Phím tắt: Ctrl+1..4 chuyển trang, Ctrl+F tìm kiếm, F5 làm mới.
//   - Đóng cửa sổ -> hỏi xác nhận thoát chương trình.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Cửa sổ chính là "khung nhà": nó tự vẽ thanh bên và đầu trang, còn phần
//  giữa là chỗ "treo" 4 trang. Bấm mục nào ở thanh bên thì ẩn trang cũ, hiện
//  trang mới. Cửa sổ chính cũng là "người đưa tin": khi một trang vừa thêm /
//  xóa / mượn / trả, nó gọi DuLieuDaThayDoi() để MỌI trang cùng cập nhật lại.
// ----------------------------------------------------------------------------

#include "CuaSoCoSo.h"
#include "TrangNoiDung.h"

#include <filesystem>
#include <memory>
#include <string>

enum class KetQuaCuaSoChinh {
    THOAT,
    DANG_XUAT
};

class CuaSoChinh : public CuaSoCoSo, public GiaoTiepTrang {
private:
    static const int SO_TRANG = 4;

    QuanLyThuVien& quanLy;
    std::filesystem::path thuMucDuLieu;
    std::wstring nguoiDung;
    std::unique_ptr<TrangNoiDung> cacTrang[SO_TRANG];
    int trangHienTai;
    int mucDangRe;              // mục sidebar đang rê chuột (-1 = không có)
    bool dangTheoDoiChuot;
    HWND thanhTrangThai;
    HACCEL bangPhimTat;
    KetQuaCuaSoChinh ketQua;

    RECT VungMuc(int chiSo) const;      // chiSo 0..3 = các trang, 4 = Đăng xuất
    int MucTaiDiem(int x, int y) const;
    void ChuyenTrang(int chiSo);
    void BoTri();
    void Ve(HDC hdc, const RECT& vung) const;
    void VeThanhBen(HDC hdc, const RECT& vung) const;
    void VeDauTrang(HDC hdc, const RECT& vung) const;
    void DangXuat();

protected:
    LRESULT XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) override;
    bool TienXuLy(MSG& thongDiep) override;

public:
    CuaSoChinh(QuanLyThuVien& quanLy, const std::filesystem::path& thuMucDuLieu, const std::wstring& nguoiDung);
    ~CuaSoChinh() override;

    KetQuaCuaSoChinh Chay();

    // ---- GiaoTiepTrang ----
    QuanLyThuVien& GetQuanLy() override;
    HWND GetCuaSoChinh() const override;
    void BaoTrangThai(const std::wstring& thongBao) override;
    void DuLieuDaThayDoi() override;
};

#endif
