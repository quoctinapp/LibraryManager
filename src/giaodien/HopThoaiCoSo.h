#ifndef HOPTHOAICOSO_H
#define HOPTHOAICOSO_H

// ============================================================================
//  HopThoaiCoSo.h - Lớp cơ sở TRỪU TƯỢNG cho các hộp thoại nhập liệu (modal)
//
//      HopThoaiCoSo (trừu tượng)
//        ├── HopThoaiThemSach
//        ├── HopThoaiThemBanDoc
//        ├── HopThoaiMuonSach
//        └── HopThoaiTraSach
//
//      - Tự bố trí các trường theo hàng: một trường chiếm cả dòng, hoặc 2 trường chia đôi dòng (TRAI / PHAI). Nhãn được vẽ phía trên mỗi trường.
//      - Nút [Hủy] (phím ESC) và nút [Đồng ý] (phím Enter) ở chân hộp thoại.
//      - Bấm Đồng ý -> gọi ThucHien() của lớp con. Nếu có lỗi: báo lỗi, HỦY chức năng và đóng hộp thoại (đúng yêu cầu đề bài).
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  "Modal" = hộp thoại chặn cửa sổ phía sau: phải bấm Đồng ý hoặc Hủy mới quay lại được cửa sổ chính. Cách làm: tạm khóa cửa sổ chính (EnableWindow FALSE) rồi chạy một vòng lặp thông điệp riêng cho tới khi hộp thoại đóng.
//  Lớp cơ sở lo khung, bố cục, vòng lặp; lớp con chỉ khai báo các trường (TaoNoiDung) và việc cần làm khi bấm Đồng ý (ThucHien).
// ----------------------------------------------------------------------------

#include "CuaSoCoSo.h"
#include "NgayThang.h"
#include "TrangNoiDung.h"
#include <string>
#include <vector>

enum class ViTriTruong {
    TOAN_DONG,      // trường chiếm cả chiều ngang
    TRAI,           // nửa trái (trường tiếp theo nên là PHAI)
    PHAI            // nửa phải, xong thì xuống dòng
};

class HopThoaiCoSo : public CuaSoCoSo {
private:
    struct Nhan {
        std::wstring chu;
        RECT vung;
    };
    struct OThongTin {
        std::wstring chu;
        RECT vung;
        COLORREF mau;
    };

    HWND cuaSoChu;
    std::wstring tieuDe;
    std::wstring moTa;
    wchar_t bieuTuong;
    std::wstring chuNutDongY;
    bool daDongY;
    int rongNoiDung;
    int yHienTai;
    std::vector<Nhan> dsNhan;
    std::vector<OThongTin> dsOThongTin;

    RECT CapViTri(const std::wstring& nhan, ViTriTruong viTri, int caoDieuKhien);
    void DongY();
    void Dong();

protected:
    GiaoTiepTrang& giaoTiep;

    // ---- Lớp con khai báo nội dung ----
    virtual void TaoNoiDung() = 0;
    virtual bool ThucHien() = 0;    // trả về true để đóng hộp thoại
    virtual void XuLyLenhThem(int id, int maThongBao);
    virtual void VeThem(HDC hdc);

    LRESULT XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) override;

    // ---- Các hàm thêm trường (gọi trong TaoNoiDung) ----
    HWND ThemONhap(const std::wstring& nhan, int id, DWORD kieuThem, int doDaiToiDa, ViTriTruong viTri = ViTriTruong::TOAN_DONG);
    HWND ThemHopChon(const std::wstring& nhan, int id, bool choPhepGo, ViTriTruong viTri = ViTriTruong::TOAN_DONG);
    HWND ThemChonNgay(const std::wstring& nhan, int id, const NgayThang& macDinh, ViTriTruong viTri = ViTriTruong::TOAN_DONG);
    int ThemOThongTin(const std::wstring& nhan, const std::wstring& giaTri, COLORREF mau, ViTriTruong viTri = ViTriTruong::TOAN_DONG);
    RECT ThemVungTuVe(int chieuCao);
    void DatOThongTin(int chiSo, const std::wstring& giaTri, COLORREF mau);

    // ---- Đọc giá trị ----
    static std::string LayChuUtf8(HWND dieuKhien);              // đã cắt khoảng trắng
    static NgayThang LayNgay(HWND chonNgay);
    template <typename T>
    static T LaySo(HWND oNhap, const std::string& tenTruong) {
        const std::string chuoi = LayChuUtf8(oNhap);
        if (chuoi.empty()) {
            throw LoiNhapLieu(tenTruong + " không được để trống.");
        }

        T giaTri{};
        if (!ChuyenSangSo(chuoi, giaTri)) {
            throw LoiNhapLieu(tenTruong + " phải là số hợp lệ (đã nhập: '" + chuoi + "').");
        }

        return giaTri;
    }

    // Lấy mã ở đầu chuỗi hộp chọn: "S001 – Cấu trúc dữ liệu" -> "S001"
    static std::string LayMaDauChuoi(HWND hopChon);

public:
    HopThoaiCoSo(GiaoTiepTrang& giaoTiep, const std::wstring& tieuDe, const std::wstring& moTa, wchar_t bieuTuong, const std::wstring& chuNutDongY);

    // Hiện hộp thoại và chờ tới khi đóng. Trả về true nếu thao tác đã thực hiện thành công.
    bool ChayModal();
};

#endif
