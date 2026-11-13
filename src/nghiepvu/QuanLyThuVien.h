#ifndef QUANLYTHUVIEN_H
#define QUANLYTHUVIEN_H

// ============================================================================
//  QuanLyThuVien.h - Lớp xử lý NGHIỆP VỤ của thư viện (không vẽ gì ra màn hình)
//   - Nạp dữ liệu từ 4 file, kiểm tra đăng nhập.
//   - Sách: thêm (ghi nối Sach.txt), xóa (ghi đè Sach.txt), tìm kiếm.
//   - Bạn đọc: thêm (ghi nối BanDoc.txt), xóa (ghi đè BanDoc.txt).
//   - Mượn / trả sách (ghi lại Sach.txt và PhieuMuon.txt).
//   - Thống kê cho trang Tổng quan.
//  Dữ liệu sai ràng buộc -> ném LoiNhapLieu; vi phạm quy định -> LoiNghiepVu;
//  lỗi ghi file -> LoiFile và dữ liệu trong bộ nhớ được khôi phục như cũ.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là "thủ thư": giữ 4 cuốn sổ (danh sách liên kết) và 4 người thủ kho
//  (KhoDuLieu) chép sổ ra file. Thủ thư lo mọi việc ĐÚNG QUY ĐỊNH, ví dụ:
//    - Giá bán phải > 0, ngày nhập kho không được ở tương lai.
//    - Sách đang có người mượn thì không được xóa, không được cho mượn tiếp.
//    - Phiếu đã trả rồi thì không trả lại lần nữa.
//  Mọi kiểm tra nằm Ở ĐÂY chứ không nằm ở giao diện, nên dù nhập từ cửa sổ
//  nào thì dữ liệu sai cũng bị chặn. Nhờ tách riêng như vậy, có thể kiểm thử
//  tự động thủ thư (tests/KiemThu.cpp) mà không cần ai ngồi bấm chuột.
// ----------------------------------------------------------------------------

#include "BanDoc.h"
#include "CauHinh.h"
#include "DanhSachLienKet.h"
#include "KhoDuLieu.h"
#include "PhieuMuon.h"
#include "Sach.h"
#include "TaiKhoan.h"

#include <filesystem>
#include <string>

// [GIẢI THÍCH] Các con số hiển thị ở trang Tổng quan
struct ThongKe {
    int tongSoSach;
    int sachDangMuon;
    int sachCoSan;
    int soBanDoc;
    int tongSoPhieu;
    int phieuDangMuon;
    int phieuQuaHan;
    // Số phiếu mượn của từng tháng, phần tử cuối là tháng hiện tại
    int thang[CauHinh::SO_THANG_THONG_KE];
    int nam[CauHinh::SO_THANG_THONG_KE];
    int soPhieuTheoThang[CauHinh::SO_THANG_THONG_KE];
};

class QuanLyThuVien {
private:
    KhoDuLieu<TaiKhoan> khoTaiKhoan;
    KhoDuLieu<Sach> khoSach;
    KhoDuLieu<BanDoc> khoBanDoc;
    KhoDuLieu<PhieuMuon> khoPhieuMuon;

    DanhSachLienKet<TaiKhoan> dsTaiKhoan;
    DanhSachLienKet<Sach> dsSach;
    DanhSachLienKet<BanDoc> dsBanDoc;
    DanhSachLienKet<PhieuMuon> dsPhieuMuon;

    // Đọc một file vào danh sách, bỏ các bản ghi trùng khóa, ghi lại cảnh báo
    template <typename T>
    static void NapDanhSach(const KhoDuLieu<T>& kho, DanhSachLienKet<T>& danhSach,
                            DanhSachLienKet<std::string>& canhBao);

    // Kiểm tra một trường chữ: không rỗng, không chứa '|', không quá dài
    static void KiemTraChu(const std::string& giaTri, const std::string& tenTruong,
                           std::size_t doDaiToiDa);
    static void KiemTraSach(const Sach& sach);
    static void KiemTraBanDoc(const BanDoc& banDoc);

    Sach* TimSachDeSua(const std::string& maSach);
    PhieuMuon* TimPhieuMuonDeSua(int soPhieu);

    // Ghi Sach.txt + PhieuMuon.txt; lỗi thì khôi phục bản sao lưu rồi ném lại lỗi
    void GhiSachVaPhieuMuon(const DanhSachLienKet<Sach>& saoLuuSach,
                            const DanhSachLienKet<PhieuMuon>& saoLuuPhieu);

public:
    // thuMucDuLieu: thư mục chứa 4 file .txt (kiểm thử dùng thư mục riêng)
    explicit QuanLyThuVien(const std::filesystem::path& thuMucDuLieu);

    // Trả về danh sách cảnh báo (thiếu file, dòng sai định dạng, trùng mã...)
    DanhSachLienKet<std::string> NapDuLieu();

    bool KiemTraDangNhap(const std::string& tenDangNhap, const std::string& matKhau) const;

    // ---------------- Truy vấn ----------------
    const DanhSachLienKet<Sach>& GetDanhSachSach() const;
    const DanhSachLienKet<BanDoc>& GetDanhSachBanDoc() const;
    const DanhSachLienKet<PhieuMuon>& GetDanhSachPhieuMuon() const;

    const Sach* TimSach(const std::string& maSach) const;
    const BanDoc* TimBanDoc(const std::string& maBanDoc) const;
    const PhieuMuon* TimPhieuMuon(int soPhieu) const;

    // Tìm theo mã / tên / tác giả / NXB, không phân biệt hoa thường và dấu
    DanhSachLienKet<Sach> TimKiemSach(const std::string& tuKhoa) const;

    int LaySoPhieuTiepTheo() const;                                 // = số phiếu lớn nhất + 1
    std::string GoiYMaBanDocMoi() const;                            // BD001, BD002... mã kế tiếp
    int DemPhieuDangMuonCua(const std::string& maBanDoc) const;
    int DemLuotMuonCua(const std::string& maBanDoc) const;
    ThongKe LayThongKe(const NgayThang& homNay) const;

    // ---------------- Quản lý sách ----------------
    void ThemSach(const Sach& sachMoi);
    void XoaSach(const std::string& maSach);

    // ---------------- Quản lý bạn đọc ----------------
    void ThemBanDoc(const BanDoc& banDocMoi);
    void XoaBanDoc(const std::string& maBanDoc);

    // ---------------- Quản lý phiếu mượn ----------------
    PhieuMuon MuonSach(const std::string& maSach, const std::string& maBanDoc);
    PhieuMuon TraSach(int soPhieu);
};

#endif
