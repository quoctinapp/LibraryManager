#ifndef KHODULIEU_H
#define KHODULIEU_H

// ============================================================================
//  KhoDuLieu.h - Lớp template đọc / ghi một danh sách đối tượng xuống file
//   - DocTatCa  : đọc từng dòng của file thành đối tượng, bỏ qua dòng sai.
//   - GhiDeTatCa: ghi ĐÈ toàn bộ danh sách (dùng khi xóa, mượn, trả).
//   - GhiNoi    : ghi NỐI một đối tượng vào cuối file (dùng khi thêm).
//  Kiểu T bắt buộc kế thừa DoiTuongLuuTru (kiểm tra lúc biên dịch).
//  Đường dẫn dùng std::filesystem::path nên mở được cả thư mục có dấu
//  (ví dụ "D:\Đồ án\data\Sach.txt").
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  "Kho dữ liệu" = người thủ kho chuyên chép sổ sách ra file và đọc vào lại.
//  Nhờ template, chỉ cần viết MỘT người thủ kho cho cả 4 loại sổ:
//    KhoDuLieu<Sach>      <-> data/Sach.txt
//    KhoDuLieu<BanDoc>    <-> data/BanDoc.txt
//    KhoDuLieu<PhieuMuon> <-> data/PhieuMuon.txt
//    KhoDuLieu<TaiKhoan>  <-> data/Admin.txt
//  Người thủ kho không cần biết bên trong Sách có gì; chỉ cần gọi hai hàm
//  ChuyenThanhDong / DocTuDong mà mọi DoiTuongLuuTru đều có.
// ----------------------------------------------------------------------------

#include "DanhSachLienKet.h"
#include "DoiTuongLuuTru.h"
#include "NgoaiLe.h"
#include "TienIch.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <type_traits>

template <typename T>
class KhoDuLieu {
    static_assert(std::is_base_of<DoiTuongLuuTru, T>::value,
                  "KhoDuLieu<T>: T phai ke thua DoiTuongLuuTru");

private:
    std::filesystem::path duongDan;

    // File do Notepad lưu dạng "UTF-8 có BOM" sẽ có 3 byte lạ ở đầu -> bỏ đi
    static void BoKyTuBom(std::string& dong)
    {
        static const std::string BOM_UTF8 = "\xEF\xBB\xBF";
        if (dong.compare(0, BOM_UTF8.size(), BOM_UTF8) == 0) {
            dong.erase(0, BOM_UTF8.size());
        }
    }

    // [GIẢI THÍCH] Nếu dòng cuối của file chưa có dấu xuống dòng (do ai đó sửa
    // tay bằng Notepad) thì khi ghi nối phải xuống dòng trước, kẻo dính 2 dòng.
    bool KetThucBangXuongDong() const
    {
        std::ifstream file(duongDan, std::ios::binary | std::ios::ate);
        if (!file.is_open() || file.tellg() <= 0) {
            return true;
        }
        file.seekg(-1, std::ios::end);
        char kyTuCuoi = '\0';
        file.get(kyTuCuoi);
        return kyTuCuoi == '\n';
    }

public:
    explicit KhoDuLieu(const std::filesystem::path& duongDan) : duongDan(duongDan) {}

    const std::filesystem::path& GetDuongDan() const { return duongDan; }

    // Tên file để hiện trong thông báo, ví dụ "Sach.txt"
    std::string GetTenFile() const { return duongDan.filename().u8string(); }

    // [GIẢI THÍCH] Trả về false nếu không mở được file.
    // soDongLoi = số dòng sai định dạng đã bị bỏ qua.
    bool DocTatCa(DanhSachLienKet<T>& danhSach, int& soDongLoi) const
    {
        danhSach.XoaHet();
        soDongLoi = 0;

        std::ifstream file(duongDan);
        if (!file.is_open()) {
            return false;
        }

        std::string dong;
        bool laDongDau = true;
        while (std::getline(file, dong)) {
            if (laDongDau) {
                BoKyTuBom(dong);
                laDongDau = false;
            }
            if (CatKhoangTrang(dong).empty()) {
                continue;   // bỏ qua dòng trống
            }

            T doiTuong;
            if (doiTuong.DocTuDong(dong)) {
                danhSach.ThemCuoi(doiTuong);
            } else {
                ++soDongLoi;
            }
        }
        return true;
    }

    void GhiDeTatCa(const DanhSachLienKet<T>& danhSach) const
    {
        std::ofstream file(duongDan, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!file.is_open()) {
            throw LoiFile("Không thể mở file '" + GetTenFile() + "' để ghi.");
        }

        for (const T& doiTuong : danhSach) {
            file << doiTuong.ChuyenThanhDong() << "\r\n";
        }

        if (!file) {
            throw LoiFile("Lỗi khi ghi dữ liệu vào file '" + GetTenFile() + "'.");
        }
    }

    void GhiNoi(const T& doiTuong) const
    {
        const bool canXuongDong = !KetThucBangXuongDong();

        std::ofstream file(duongDan, std::ios::out | std::ios::app | std::ios::binary);
        if (!file.is_open()) {
            throw LoiFile("Không thể mở file '" + GetTenFile() + "' để ghi nối.");
        }

        if (canXuongDong) {
            file << "\r\n";
        }
        file << doiTuong.ChuyenThanhDong() << "\r\n";

        if (!file) {
            throw LoiFile("Lỗi khi ghi nối dữ liệu vào file '" + GetTenFile() + "'.");
        }
    }
};

#endif
