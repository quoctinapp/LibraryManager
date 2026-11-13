#ifndef BANGDULIEU_H
#define BANGDULIEU_H

// ============================================================================
//  BangDuLieu.h - Bảng hiển thị dữ liệu dạng lưới (bọc điều khiển ListView)
//      - Thêm cột, thêm dòng, lấy dòng đang chọn.
//      - Dòng chẵn / lẻ tô màu xen kẽ; một cột "nhấn mạnh" được tô màu riêng theo từng dòng (ví dụ tình trạng: xanh = có sẵn, đỏ = đang mượn).
//      - Bấm tiêu đề cột để sắp xếp tăng / giảm (hiện mũi tên ▲▼ trên cột).
//      - Hiện dòng chữ "Không có dữ liệu" khi bảng rỗng.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  ListView là bảng có sẵn của Windows (giống bảng trong File Explorer).
//  Lớp này chỉ "bọc" lại cho dễ dùng: trang nào cần bảng thì tạo một BangDuLieu, gọi ThemCot / ThemDong là xong.
//  Bảng KHÔNG tự sắp xếp: khi người dùng bấm tiêu đề cột, bảng chỉ ghi nhớ "đang sắp theo cột nào, tăng hay giảm". Trang chứa bảng sẽ sắp xếp danh sách liên kết bằng DanhSachLienKet::SapXep (merge sort) rồi đổ lại vào bảng.
// ----------------------------------------------------------------------------

#include "ChuDe.h"
#include <commctrl.h>
#include <initializer_list>
#include <string>
#include <vector>

class BangDuLieu {
private:
    HWND hwnd;
    int soCot;
    int cotSapXep;          // -1 = chưa sắp xếp
    bool tangDan;
    int cotNhanManh;        // -1 = không có
    std::vector<COLORREF> mauNhanManh;
    std::vector<int> rongThietKe;   // độ rộng mong muốn của từng cột (đã nhân DPI)
    std::wstring chuKhiRong;

    void CapNhatMuiTenSapXep();
    void CanDoRongCot();            // co giãn các cột theo tỉ lệ cho vừa khít bề ngang bảng

public:
    BangDuLieu();

    void Tao(HWND cha, int id);
    HWND GetHwnd() const { return hwnd; }

    void ThemCot(const std::wstring& tenCot, int rong, int canLe = LVCFMT_LEFT);
    void DatCotNhanManh(int cot);
    void DatChuKhiRong(const std::wstring& chu);
    void DatViTri(int x, int y, int rong, int cao);

    // Cập nhật nội dung: BatDau -> ThemDong ... -> KetThuc (không nháy khi đổ nhiều dòng)
    void BatDauCapNhat();
    void ThemDong(std::initializer_list<std::wstring> cacO, COLORREF mauCotNhanManh = ChuDe::CHU);
    void KetThucCapNhat();

    int SoDong() const;
    std::wstring LayChuDongChon(int cot = 0) const;     // rỗng nếu chưa chọn dòng nào
    void ChonDongCoChu(const std::wstring& chu, int cot = 0);

    int GetCotSapXep() const { return cotSapXep; }
    bool GetTangDan() const { return tangDan; }

    // Xử lý WM_NOTIFY của bảng. Trả về true nếu đã xử lý (ketQua = giá trị trả về).
    // daBamCot = true khi người dùng vừa bấm tiêu đề cột (cần sắp xếp lại).
    bool XuLyThongBao(const NMHDR* thongBao, LRESULT& ketQua, bool& daBamCot);
};

#endif
