#ifndef TIENICH_H
#define TIENICH_H

// ============================================================================
//  TienIch.h - Các hàm tiện ích xử lý chuỗi dùng chung
//      - Cắt khoảng trắng, đổi hoa / thường (cho mã), kiểm tra mã hợp lệ.
//      - Đổi qua lại UTF-8 (lưu file) <-> UTF-16 (Windows hiển thị), đếm ký tự.
//      - Tìm kiếm KHÔNG DẤU ("nguyen" tìm ra "Nguyễn"), so sánh theo tiếng Việt.
//      - Template: ChuyenSangSo<T> (chuỗi -> số), TachChuoi<N> (tách "a|b|c").
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  "Tiện ích" = hộp đồ nghề nhỏ dùng chung cho nhiều file khác.
//
//  Vì sao có 2 kiểu chuỗi?
//      - File .txt lưu chữ tiếng Việt theo chuẩn UTF-8 (std::string): chữ "ễ" chiếm 3 byte. Đây là chuẩn phổ biến, Notepad mở được.
//      - Cửa sổ Windows hiển thị chữ theo chuẩn UTF-16 (std::wstring): mỗi chữ tiếng Việt là 1 ký tự wchar_t.
//  Hai hàm Utf8SangUtf16 / Utf16SangUtf8 là "phiên dịch viên" giữa hai bên.
//
//  Hai hàm template ở cuối file viết MỘT lần nhưng dùng được cho NHIỀU kiểu:
//      - ChuyenSangSo<int>, ChuyenSangSo<long long>... đổi chuỗi thành số, và chỉ chấp nhận khi TOÀN BỘ chuỗi là số ("12abc" -> sai).
//      - TachChuoi<9> tách một dòng trong file Sach.txt thành đúng 9 ô.
// ----------------------------------------------------------------------------

#include <cstddef>
#include <sstream>
#include <string>

// Bỏ khoảng trắng / xuống dòng thừa ở hai đầu: "  abc  " -> "abc"
std::string CatKhoangTrang(const std::string& chuoi);

// Đổi hoa / thường các chữ cái tiếng Anh (dùng cho mã sách, mã bạn đọc)
std::string VietHoa(const std::string& chuoi);
std::string VietThuong(const std::string& chuoi);

// Mã hợp lệ: không rỗng, tối đa DO_DAI_MA_TOI_DA ký tự, chỉ gồm chữ cái và chữ số (không dấu)
bool LaMaHopLe(const std::string& ma);

// 125000 -> "125,000"
std::string DinhDangTien(long long giaTri);

// ---- Chuyển đổi bảng mã ----
std::wstring Utf8SangUtf16(const std::string& chuoi);
std::string Utf16SangUtf8(const std::wstring& chuoi);

// Số KÝ TỰ của chuỗi UTF-8 ("Nguyễn" = 6 ký tự dù chiếm 8 byte)
std::size_t DemKyTuUtf8(const std::string& chuoi);

// ---- Tìm kiếm / so sánh tiếng Việt ----
// Bỏ dấu và viết thường: "Nguyễn Đức" -> "nguyen duc"
std::wstring BoDauVaVietThuong(const std::wstring& chuoi);

// Tìm từ khóa không phân biệt hoa / thường và không phân biệt dấu
bool ChuaTuKhoa(const std::string& chuoi, const std::string& tuKhoa);

// So sánh theo thứ tự từ điển tiếng Việt: < 0 nếu a đứng trước b, 0 nếu bằng, > 0 nếu sau
int SoSanhTiengViet(const std::string& a, const std::string& b);

// [GIẢI THÍCH] Đổi chuỗi sang số kiểu T. Trả về false nếu chuỗi rỗng, có ký tự
// thừa ("12abc", "1.5" với kiểu int) hoặc số quá lớn so với kiểu T.
template <typename T>
bool ChuyenSangSo(const std::string& chuoi, T& ketQua){
    const std::string daCat = CatKhoangTrang(chuoi);
    if (daCat.empty()) {
        return false;
    }

    std::istringstream luong(daCat);
    T giaTri{};
    luong >> giaTri;
    if (luong.fail() || !luong.eof()) {
        return false;
    }

    ketQua = giaTri;
    return true;
}

// [GIẢI THÍCH] Tách chuỗi theo ký tự phân cách vào mảng có ĐÚNG N phần tử.
// Ví dụ TachChuoi("a|b|c", '|', mang3) -> mang3 = {"a", "b", "c"}, trả về true.
// Nếu số ô khác N (thiếu hoặc thừa) thì trả về false.
template <std::size_t N>
bool TachChuoi(const std::string& chuoi, char kyTuPhanCach, std::string (&ketQua)[N]){
    std::size_t soO = 0;
    std::size_t batDau = 0;

    while (true) {
        if (soO >= N) {
            return false;
        }

        const std::size_t viTri = chuoi.find(kyTuPhanCach, batDau);
        if (viTri == std::string::npos) {
            ketQua[soO++] = chuoi.substr(batDau);
            break;
        }

        ketQua[soO++] = chuoi.substr(batDau, viTri - batDau);
        batDau = viTri + 1;
    }

    return soO == N;
}

#endif
