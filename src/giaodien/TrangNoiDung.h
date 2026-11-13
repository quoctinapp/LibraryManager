#ifndef TRANGNOIDUNG_H
#define TRANGNOIDUNG_H

// ============================================================================
//  TrangNoiDung.h - Lớp cơ sở TRỪU TƯỢNG cho các trang của cửa sổ chính
//
//      TrangNoiDung (trừu tượng)
//        ├── TrangTongQuan     thẻ thống kê, biểu đồ, phiếu quá hạn
//        ├── TrangSach         hiển thị / thêm / xóa / tìm / cho mượn / xuất CSV
//        ├── TrangPhieuMuon    hiển thị / lọc / mượn / trả / xuất CSV
//        └── TrangBanDoc       hiển thị / thêm / xóa / tìm / xuất CSV
//
//  GiaoTiepTrang: "hợp đồng" mà cửa sổ chính cung cấp cho các trang (lấy thủ
//  thư, báo lên thanh trạng thái, yêu cầu mọi trang làm mới dữ liệu).
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Mỗi trang là một cửa sổ con nằm trong vùng nội dung của cửa sổ chính, chỉ
//  một trang được hiện tại một thời điểm. Lớp cơ sở lo phần GIỐNG NHAU:
//  thanh công cụ (ô tìm kiếm + các nút), dòng tóm tắt, khung thẻ trắng chứa
//  bảng, bắt lỗi khi thực hiện chức năng. Phần KHÁC NHAU (cột nào, nút nào,
//  bấm nút thì làm gì) do từng lớp con tự viết qua các hàm ảo.
//
//  Cửa sổ chính chỉ cần giữ 4 con trỏ TrangNoiDung* và gọi LamMoi() / GetTieuDe()
//  mà không cần biết đó là trang nào -> đây chính là TÍNH ĐA HÌNH.
// ----------------------------------------------------------------------------

#include "BangDuLieu.h"
#include "CuaSoCoSo.h"
#include "HopThongBao.h"
#include "NgoaiLe.h"
#include "QuanLyThuVien.h"
#include "TienIch.h"

#include <filesystem>
#include <string>
#include <vector>

class GiaoTiepTrang {
public:
    virtual ~GiaoTiepTrang() = default;
    virtual QuanLyThuVien& GetQuanLy() = 0;
    virtual HWND GetCuaSoChinh() const = 0;
    virtual void BaoTrangThai(const std::wstring& thongBao) = 0;
    virtual void DuLieuDaThayDoi() = 0;         // làm mới mọi trang sau khi thêm / xóa / mượn / trả
};

class TrangNoiDung : public CuaSoCoSo {
protected:
    GiaoTiepTrang& giaoTiep;
    BangDuLieu bang;
    HWND oTimKiem;
    std::wstring chuTomTat;     // dòng tóm tắt phía trên bảng, ví dụ "Hiển thị 12 / 12 sách"
    RECT vungTomTat;
    RECT vungTheBang;           // khung thẻ trắng bao quanh bảng

    // ---- Các hàm lớp con phải / có thể viết lại ----
    virtual void TaoDieuKhien() = 0;
    virtual void BoTri(int rong, int cao) = 0;
    virtual void Ve(HDC hdc, const RECT& vung);
    virtual void XuLyLenh(int id, int maThongBao);
    virtual bool XuLyThongBaoKhac(const NMHDR* thongBao, LRESULT& ketQua);

    LRESULT XuLyThongDiep(UINT thongDiep, WPARAM wParam, LPARAM lParam) override;

    // ---- Tiện ích dùng chung cho các trang ----
    void TaoOTimKiem(const std::wstring& goiY);
    // Bố trí thanh công cụ: ô tìm kiếm (+ hộp lọc) bên trái, các nút xếp bên phải; sau đó là
    // dòng tóm tắt và khung bảng chiếm phần còn lại
    void BoTriChuan(int rong, int cao, const std::vector<HWND>& cacNut, HWND hopLoc = nullptr);
    static int DoRongNut(HWND nut);
    void DatTomTat(const std::wstring& chu);

    // So sánh 2 giá trị bất kỳ có toán tử < : trả về -1 / 0 / 1 (dùng khi sắp xếp bảng)
    template <typename T>
    static int SoSanhGiaTri(const T& a, const T& b)
    {
        if (a < b) {
            return -1;
        }
        return (b < a) ? 1 : 0;
    }

    // Đổi kết quả so sánh -1/0/1 thành "a đứng trước b" theo chiều tăng / giảm của bảng
    bool DungTruoc(int ketQuaSoSanh) const
    {
        return bang.GetTangDan() ? ketQuaSoSanh < 0 : ketQuaSoSanh > 0;
    }

    // Chạy một chức năng; nếu có lỗi thì báo lỗi, HỦY chức năng và quay lại (đúng yêu cầu đề bài)
    template <typename Ham>
    void ThucHienAnToan(const std::wstring& tenChucNang, Ham ham)
    {
        try {
            ham();
        } catch (const LoiChuongTrinh& loi) {
            HopThongBao::Loi(giaoTiep.GetCuaSoChinh(), tenChucNang + L" không thành công",
                             Utf8SangUtf16(loi.what()) + L"\n\nChức năng đã bị hủy.");
            giaoTiep.BaoTrangThai(L"Lỗi: " + Utf8SangUtf16(loi.what()));
        } catch (const std::exception& loi) {
            HopThongBao::Loi(giaoTiep.GetCuaSoChinh(), L"Lỗi không xác định",
                             Utf8SangUtf16(loi.what()) + L"\n\nChức năng đã bị hủy.");
        }
    }

    // Hộp thoại "Lưu thành..." để chọn nơi lưu file CSV. Trả về rỗng nếu người dùng hủy.
    std::filesystem::path HoiNoiLuuCsv(const std::wstring& tenMacDinh) const;
    void HoanTatXuatCsv(const std::filesystem::path& duongDan, std::size_t soDong);

public:
    explicit TrangNoiDung(GiaoTiepTrang& giaoTiep);

    bool Tao(HWND cha);
    virtual std::wstring GetTieuDe() const = 0;
    virtual std::wstring GetMoTa() const = 0;
    virtual void LamMoi() = 0;
    virtual void DatTieuDiemTimKiem();
};

#endif
