// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của các hàm tiện ích (mục lục ở TienIch.h).
//  Các hàm về tiếng Việt nhờ Windows làm giúp:
//   - MultiByteToWideChar / WideCharToMultiByte: đổi UTF-8 <-> UTF-16.
//   - FoldStringW(MAP_COMPOSITE): tách "ễ" thành "e" + dấu mũ + dấu ngã,
//     sau đó chỉ việc bỏ các dấu đi là được chữ không dấu.
//   - CompareStringEx("vi-VN"): so sánh theo thứ tự bảng chữ cái tiếng Việt.
// ============================================================================

#include "TienIch.h"
#include "CauHinh.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cctype>

std::string CatKhoangTrang(const std::string& chuoi)
{
    const char* khoangTrang = " \t\r\n";
    const std::size_t dau = chuoi.find_first_not_of(khoangTrang);
    if (dau == std::string::npos) {
        return "";
    }

    const std::size_t cuoi = chuoi.find_last_not_of(khoangTrang);
    return chuoi.substr(dau, cuoi - dau + 1);
}

std::string VietHoa(const std::string& chuoi)
{
    std::string ketQua = chuoi;
    for (char& kyTu : ketQua) {
        if (kyTu >= 'a' && kyTu <= 'z') {
            kyTu = static_cast<char>(kyTu - 'a' + 'A');
        }
    }
    return ketQua;
}

std::string VietThuong(const std::string& chuoi)
{
    std::string ketQua = chuoi;
    for (char& kyTu : ketQua) {
        if (kyTu >= 'A' && kyTu <= 'Z') {
            kyTu = static_cast<char>(kyTu - 'A' + 'a');
        }
    }
    return ketQua;
}

bool LaMaHopLe(const std::string& ma)
{
    if (ma.empty() || ma.size() > CauHinh::DO_DAI_MA_TOI_DA) {
        return false;
    }

    for (char kyTu : ma) {
        const bool laChuSo = kyTu >= '0' && kyTu <= '9';
        const bool laChuCai = (kyTu >= 'a' && kyTu <= 'z') || (kyTu >= 'A' && kyTu <= 'Z');
        if (!laChuSo && !laChuCai) {
            return false;
        }
    }
    return true;
}

std::string DinhDangTien(long long giaTri)
{
    const bool laSoAm = giaTri < 0;
    const std::string chuSo = std::to_string(laSoAm ? -giaTri : giaTri);

    // [GIẢI THÍCH] Duyệt từ phải sang trái, cứ đủ 3 chữ số thì chèn dấu phẩy.
    std::string ketQua;
    int dem = 0;
    for (std::size_t i = chuSo.size(); i > 0; --i) {
        ketQua.insert(ketQua.begin(), chuSo[i - 1]);
        ++dem;
        if (dem % 3 == 0 && i > 1) {
            ketQua.insert(ketQua.begin(), ',');
        }
    }

    return laSoAm ? "-" + ketQua : ketQua;
}

// ============================================================================
//  UTF-8 <-> UTF-16
// ============================================================================

// [GIẢI THÍCH] Gọi Windows 2 lần: lần 1 hỏi "cần bao nhiêu chỗ", lần 2 đổi thật.
std::wstring Utf8SangUtf16(const std::string& chuoi)
{
    if (chuoi.empty()) {
        return L"";
    }
    const int doDai = MultiByteToWideChar(CP_UTF8, 0, chuoi.data(),
                                          static_cast<int>(chuoi.size()), nullptr, 0);
    if (doDai <= 0) {
        return L"";
    }
    std::wstring ketQua(static_cast<std::size_t>(doDai), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, chuoi.data(), static_cast<int>(chuoi.size()),
                        &ketQua[0], doDai);
    return ketQua;
}

std::string Utf16SangUtf8(const std::wstring& chuoi)
{
    if (chuoi.empty()) {
        return "";
    }
    const int doDai = WideCharToMultiByte(CP_UTF8, 0, chuoi.data(),
                                          static_cast<int>(chuoi.size()), nullptr, 0,
                                          nullptr, nullptr);
    if (doDai <= 0) {
        return "";
    }
    std::string ketQua(static_cast<std::size_t>(doDai), '\0');
    WideCharToMultiByte(CP_UTF8, 0, chuoi.data(), static_cast<int>(chuoi.size()),
                        &ketQua[0], doDai, nullptr, nullptr);
    return ketQua;
}

// [GIẢI THÍCH] Trong UTF-8, byte "nối tiếp" luôn có dạng 10xxxxxx. Chỉ đếm
// các byte KHÔNG phải byte nối tiếp là ra số ký tự.
std::size_t DemKyTuUtf8(const std::string& chuoi)
{
    std::size_t dem = 0;
    for (char kyTu : chuoi) {
        if ((static_cast<unsigned char>(kyTu) & 0xC0) != 0x80) {
            ++dem;
        }
    }
    return dem;
}

// ============================================================================
//  Tìm kiếm / so sánh tiếng Việt
// ============================================================================

std::wstring BoDauVaVietThuong(const std::wstring& chuoi)
{
    if (chuoi.empty()) {
        return L"";
    }

    // 1) Tách chữ có dấu thành chữ gốc + các dấu: "ễ" -> "e" + "^" + "~"
    std::wstring daTach = chuoi;
    const int doDai = FoldStringW(MAP_COMPOSITE, chuoi.c_str(), static_cast<int>(chuoi.size()),
                                  nullptr, 0);
    if (doDai > 0) {
        daTach.assign(static_cast<std::size_t>(doDai), L'\0');
        FoldStringW(MAP_COMPOSITE, chuoi.c_str(), static_cast<int>(chuoi.size()),
                    &daTach[0], doDai);
    }

    // 2) Bỏ các dấu (mã U+0300..U+036F), đổi đ/Đ thành d, viết thường
    std::wstring ketQua;
    ketQua.reserve(daTach.size());
    for (wchar_t kyTu : daTach) {
        if (kyTu >= 0x0300 && kyTu <= 0x036F) {
            continue;
        }
        if (kyTu == L'đ' || kyTu == L'Đ') {     // đ, Đ
            kyTu = L'd';
        }
        ketQua.push_back(kyTu);
    }
    if (!ketQua.empty()) {
        CharLowerBuffW(&ketQua[0], static_cast<DWORD>(ketQua.size()));
    }
    return ketQua;
}

bool ChuaTuKhoa(const std::string& chuoi, const std::string& tuKhoa)
{
    const std::wstring tuKhoaChuan = BoDauVaVietThuong(Utf8SangUtf16(CatKhoangTrang(tuKhoa)));
    if (tuKhoaChuan.empty()) {
        return true;    // từ khóa rỗng = khớp tất cả
    }
    return BoDauVaVietThuong(Utf8SangUtf16(chuoi)).find(tuKhoaChuan) != std::wstring::npos;
}

int SoSanhTiengViet(const std::string& a, const std::string& b)
{
    const std::wstring wa = Utf8SangUtf16(a);
    const std::wstring wb = Utf8SangUtf16(b);
    const int ketQua = CompareStringEx(L"vi-VN", LINGUISTIC_IGNORECASE | SORT_DIGITSASNUMBERS,
                                       wa.c_str(), static_cast<int>(wa.size()),
                                       wb.c_str(), static_cast<int>(wb.size()),
                                       nullptr, nullptr, 0);
    if (ketQua == 0) {
        return wa.compare(wb);      // Windows không so sánh được -> so sánh mã ký tự
    }
    return ketQua - CSTR_EQUAL;     // CSTR_LESS_THAN(1) -> -1, CSTR_EQUAL(2) -> 0, CSTR_GREATER_THAN(3) -> 1
}
