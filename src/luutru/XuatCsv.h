#ifndef XUATCSV_H
#define XUATCSV_H

// ============================================================================
//  XuatCsv.h - Xuất một danh sách ra file .csv để mở bằng Excel
//      - File ghi theo UTF-8 CÓ BOM: Excel nhận ra và hiển thị đúng tiếng Việt.
//      - Ô chứa dấu phẩy / ngoặc kép / xuống dòng được bọc trong "..." theo chuẩn CSV (RFC 4180), ngoặc kép bên trong được nhân đôi.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  CSV = "bảng tính dạng chữ": mỗi dòng là một hàng, các ô ngăn nhau bằng dấu phẩy. Hàm template XuatCsv dùng được cho MỌI loại danh sách: chỉ cần đưa vào một hàm "tạo dòng" cho biết mỗi phần tử thành dòng CSV như thế nào.
//  Ví dụ xuất danh sách sách:
//    XuatCsv(duongDan, "Mã sách,Tên sách", dsSach, [](const Sach& s) {
//        return NoiCsv({ s.GetMaSach(), s.GetTenSach() });
//    });
// ----------------------------------------------------------------------------

#include "DanhSachLienKet.h"
#include "NgoaiLe.h"
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <string>

// Bọc một ô theo chuẩn CSV nếu cần: Clean Code, "Bob" -> "Clean Code, ""Bob"""
inline std::string OCsv(const std::string& noiDung) {
    if (noiDung.find_first_of(",\"\r\n") == std::string::npos) {
        return noiDung;
    }

    std::string ketQua = "\"";
    for (char kyTu : noiDung) {
        if (kyTu == '"') {
            ketQua += "\"\"";
        } else {
            ketQua += kyTu;
        }
    }

    return ketQua + "\"";
}

// Nối nhiều ô thành một dòng CSV
inline std::string NoiCsv(std::initializer_list<std::string> cacO) {
    std::string dong;
    bool laODau = true;
    for (const std::string& o : cacO) {
        if (!laODau) {
            dong += ',';
        }

        dong += OCsv(o);
        laODau = false;
    }

    return dong;
}

template <typename T, typename HamTaoDong>
void XuatCsv(const std::filesystem::path& duongDan, const std::string& dongTieuDe, const DanhSachLienKet<T>& danhSach, HamTaoDong taoDong) {
    std::ofstream file(duongDan, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file.is_open()) {
        throw LoiFile("Không thể tạo file CSV (file có thể đang được mở bằng Excel).");
    }

    file << "\xEF\xBB\xBF";                 // BOM UTF-8 cho Excel
    file << dongTieuDe << "\r\n";
    for (const T& phanTu : danhSach) {
        file << taoDong(phanTu) << "\r\n";
    }

    if (!file) {
        throw LoiFile("Lỗi khi ghi file CSV.");
    }
}

#endif
