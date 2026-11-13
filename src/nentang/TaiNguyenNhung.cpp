// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp TaiNguyenNhung (mục lục ở TaiNguyenNhung.h).
//  Đọc tài nguyên nhúng: FindResource (tìm) -> LoadResource (nạp) ->
//  LockResource (lấy con trỏ tới các byte) -> ghi nguyên văn các byte ra file.
// ============================================================================

#include "TaiNguyenNhung.h"
#include "CauHinh.h"
#include "NgoaiLe.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

struct TepNhung {
    const wchar_t* tenTaiNguyen;    // tên trong TaiNguyen.rc
    const char* tenFile;
};

static const TepNhung CAC_TEP_NHUNG[] = {
    { L"DL_ADMIN", CauHinh::FILE_TAI_KHOAN },
    { L"DL_SACH", CauHinh::FILE_SACH },
    { L"DL_BANDOC", CauHinh::FILE_BAN_DOC },
    { L"DL_PHIEUMUON", CauHinh::FILE_PHIEU_MUON },
};

fs::path TaiNguyenNhung::ThuMucChuaExe() {
    wchar_t duongDan[MAX_PATH * 4] = L"";
    GetModuleFileNameW(nullptr, duongDan, static_cast<DWORD>(MAX_PATH * 4));
    return fs::path(duongDan).parent_path();
}

bool TaiNguyenNhung::CoDuLieu(const fs::path& thuMuc) {
    std::error_code loi;
    return fs::exists(thuMuc / CauHinh::FILE_TAI_KHOAN, loi);
}

bool TaiNguyenNhung::GhiTaiNguyenRaFile(const wchar_t* tenTaiNguyen, const fs::path& dich) {
    HRSRC timThay = FindResourceW(nullptr, tenTaiNguyen, MAKEINTRESOURCEW(10));     // 10 = RT_RCDATA
    if (timThay == nullptr) {
        return false;
    }

    HGLOBAL daNap = LoadResource(nullptr, timThay);
    const DWORD kichThuoc = SizeofResource(nullptr, timThay);
    const void* duLieu = daNap != nullptr ? LockResource(daNap) : nullptr;
    if (duLieu == nullptr) {
        return false;
    }

    std::ofstream file(dich, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file.write(static_cast<const char*>(duLieu), static_cast<std::streamsize>(kichThuoc));
    return static_cast<bool>(file);
}

// [GIẢI THÍCH] Chỉ giải nén file nào CÒN THIẾU, không bao giờ ghi đè dữ liệu đang có.
bool TaiNguyenNhung::GiaiNenNeuThieu(const fs::path& thuMuc) {
    std::error_code loi;
    fs::create_directories(thuMuc, loi);
    if (!fs::is_directory(thuMuc, loi)) {
        return false;
    }

    for (const TepNhung& tep : CAC_TEP_NHUNG) {
        const fs::path dich = thuMuc / tep.tenFile;
        if (!fs::exists(dich, loi) && !GhiTaiNguyenRaFile(tep.tenTaiNguyen, dich)) {
            return false;
        }
    }

    return true;
}

fs::path TaiNguyenNhung::ChuanBiThuMucDuLieu() {
    const fs::path canhExe = ThuMucChuaExe() / CauHinh::THU_MUC_DU_LIEU;
    if (CoDuLieu(canhExe)) {
        GiaiNenNeuThieu(canhExe);
        return canhExe;
    }

    std::error_code loi;
    const fs::path hienHanh = fs::current_path(loi) / CauHinh::THU_MUC_DU_LIEU;
    if (!loi && CoDuLieu(hienHanh)) {
        return hienHanh;
    }

    if (GiaiNenNeuThieu(canhExe)) {
        return canhExe;
    }

    wchar_t appData[MAX_PATH * 4] = L"";
    if (GetEnvironmentVariableW(L"LOCALAPPDATA", appData, static_cast<DWORD>(MAX_PATH * 4)) > 0) {
        const fs::path duPhong = fs::path(appData) / L"QuanLyThuVien" / CauHinh::THU_MUC_DU_LIEU;
        if (GiaiNenNeuThieu(duPhong)) {
            return duPhong;
        }
    }

    throw LoiFile("Không thể tạo thư mục dữ liệu cạnh file chương trình.");
}
