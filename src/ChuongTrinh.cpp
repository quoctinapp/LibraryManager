// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp ChuongTrinh (mục lục ở ChuongTrinh.h).
// ============================================================================

#include "ChuongTrinh.h"
#include "ChuDe.h"
#include "CuaSoChinh.h"
#include "CuaSoCoSo.h"
#include "CuaSoDangNhap.h"
#include "HopThongBao.h"
#include "NgoaiLe.h"
#include "QuanLyThuVien.h"
#include "TaiNguyenNhung.h"
#include "TienIch.h"
#include <commctrl.h>
#include <filesystem>
#include <string>

int ChuongTrinh::Chay(HINSTANCE hInstance){
    // [GIẢI THÍCH] Chỉ cho chạy MỘT cửa sổ chương trình cùng lúc: hai cửa sổ cùng ghi vào một file dữ liệu sẽ làm hỏng dữ liệu của nhau.
    HANDLE khoaDuyNhat = CreateMutexW(nullptr, TRUE, L"QuanLyThuVien.CTDL.Project3");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HopThongBao::ThongTin(nullptr, L"Chương trình đang chạy", L"Quản lý thư viện đã được mở ở một cửa sổ khác.");
        CloseHandle(khoaDuyNhat);
        return 0;
    }

    INITCOMMONCONTROLSEX thuVien = { sizeof(thuVien), ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_STANDARD_CLASSES | ICC_BAR_CLASSES };
    InitCommonControlsEx(&thuVien);
    ChuDe::KhoiTao();
    CuaSoCoSo::DangKyLop(hInstance);

    int maThoat = 0;
    try {
        const std::filesystem::path thuMucDuLieu = TaiNguyenNhung::ChuanBiThuMucDuLieu();
        QuanLyThuVien quanLy(thuMucDuLieu);

        const DanhSachLienKet<std::string> canhBao = quanLy.NapDuLieu();
        if (!canhBao.Rong()) {
            std::wstring noiDung;
            for (const std::string& dong : canhBao) {
                noiDung += L"•  " + Utf8SangUtf16(dong) + L"\n";
            }
            HopThongBao::CanhBao(nullptr, L"Cảnh báo khi đọc dữ liệu", noiDung);
        }

        // Đăng nhập -> cửa sổ chính; đăng xuất thì lặp lại từ đầu
        while (true) {
            CuaSoDangNhap dangNhap(quanLy);
            const KetQuaDangNhap ketQua = dangNhap.Chay();
            if (ketQua == KetQuaDangNhap::SAI_QUA_SO_LAN) {
                HopThongBao::Loi(nullptr, L"Đăng nhập thất bại", L"Bạn đã nhập sai user / password " + std::to_wstring(CauHinh::SO_LAN_DANG_NHAP_TOI_DA) + L" lần. Hệ thống sẽ thoát.");
                break;
            }
            if (ketQua == KetQuaDangNhap::THOAT) {
                break;
            }

            CuaSoChinh cuaSoChinh(quanLy, thuMucDuLieu, dangNhap.GetTenDangNhap());
            if (cuaSoChinh.Chay() == KetQuaCuaSoChinh::THOAT) {
                break;
            }
        }
    } catch (const LoiChuongTrinh& loi) {
        HopThongBao::Loi(nullptr, L"Không thể khởi động chương trình", Utf8SangUtf16(loi.what()));
        maThoat = 1;
    }

    ChuDe::GiaiPhong();
    CloseHandle(khoaDuyNhat);
    return maThoat;
}
