// ============================================================================
//  KiemThu.cpp - Kiểm thử tự động (chương trình console riêng, không thuộc chương trình chính)
//  Biên dịch và chạy từ thư mục gốc dự án:
//    mingw32-make -j16 test
//  Dữ liệu mẫu trong data/ được SAO CHÉP sang một thư mục tên CÓ DẤU trong build/ rồi mới thử thêm / xóa / mượn / trả, nên file thật trong data/ không bị thay đổi. Mã thoát 0 = tất cả đạt.
// ============================================================================

// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Đây là chương trình "tự chấm bài". Nó gọi thủ thư (QuanLyThuVien) với dữ liệu biết trước đáp án, rồi so sánh kết quả. Đúng in [DAT], sai in [SAI].
//  Ví dụ: sách S003 đang được mượn thì lệnh xóa S003 PHẢI bị từ chối.
//  Mỗi lần sửa code chỉ cần chạy lại file này để biết có làm hỏng gì không.
// ============================================================================

#include "DanhSachLienKet.h"
#include "NgayThang.h"
#include "NgoaiLe.h"
#include "QuanLyThuVien.h"
#include "TienIch.h"
#include "XuatCsv.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

// Thư mục kiểm thử có dấu tiếng Việt -> kiểm tra luôn việc mở file đường dẫn Unicode
static const fs::path THU_MUC_KIEM_THU = fs::path(L"build") / L"kiểm_thử_dữ_liệu";

// [GIẢI THÍCH] "Bộ chấm điểm": đếm số ca đã kiểm tra và số ca sai.
class BoKiemThu {
private:
    int soCa;
    int soLoi;

    template <typename LoaiLoi, typename HamThu>
    void PhaiBaoLoi(const std::string& moTa, HamThu ham) {
        bool coLoi = false;
        try {
            ham();
        } catch (const LoaiLoi&) {
            coLoi = true;
        }

        KiemTra(moTa, coLoi);
    }

public:
    BoKiemThu() : soCa(0), soLoi(0) {}

    void KiemTra(const std::string& moTa, bool dat) {
        soCa++;
        if (!dat) {
            soLoi++;
        }

        std::cout << (dat ? "  [DAT] " : "  [SAI] ") << moTa << '\n';
    }

    // Đạt khi hàm truyền vào ném đúng loại lỗi
    template <typename HamThu>
    void PhaiBaoLoiNghiepVu(const std::string& moTa, HamThu ham) {
        PhaiBaoLoi<LoiNghiepVu>(moTa, ham);
    }

    template <typename HamThu>
    void PhaiBaoLoiNhapLieu(const std::string& moTa, HamThu ham) {
        PhaiBaoLoi<LoiNhapLieu>(moTa, ham);
    }

    void Nhom(const std::string& tenNhom) {
        std::cout << "\n== " << tenNhom << " ==\n";
    }

    int KetThuc() const {
        std::cout << "\nKet qua: " << (soCa - soLoi) << "/" << soCa << " ca dat.\n";
        return soLoi == 0 ? 0 : 1;
    }
};

static std::string DocFile(const fs::path& duongDan) {
    std::ifstream file(duongDan, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

static bool ChuanBiDuLieu() {
    std::error_code loi;
    fs::remove_all(THU_MUC_KIEM_THU, loi);
    fs::create_directories(THU_MUC_KIEM_THU, loi);
    for (const char* ten : { CauHinh::FILE_TAI_KHOAN, CauHinh::FILE_SACH, CauHinh::FILE_BAN_DOC, CauHinh::FILE_PHIEU_MUON }) {
        fs::copy_file(fs::path(CauHinh::THU_MUC_DU_LIEU) / ten, THU_MUC_KIEM_THU / ten, fs::copy_options::overwrite_existing, loi);
        if (loi) {
            std::cout << "Khong sao chep duoc data/" << ten << " (hay chay tu thu muc goc du an)\n";
            return false;
        }
    }

    return true;
}

// Phần tử dùng để kiểm tra sắp xếp ỔN ĐỊNH: cùng khóa thì giữ thứ tự ban đầu
struct CapSo {
    int khoa;
    int thuTuBanDau;
};

int main() {
    if (!ChuanBiDuLieu()) {
        return 1;
    }

    BoKiemThu bo;
    const NgayThang homNay = NgayThang::HomNay();

    // ------------------------------------------------------------------------
    bo.Nhom("1. NgayThang");
    bo.KiemTra("28/02/2024 + 1 = 29/02/2024 (nam nhuan)", NgayThang(28, 2, 2024).CongNgay(1) == NgayThang(29, 2, 2024));
    bo.KiemTra("28/12/2026 + 7 = 04/01/2027 (qua nam moi)", NgayThang(28, 12, 2026).CongNgay(7) == NgayThang(4, 1, 2027));
    NgayThang ngay;
    bo.KiemTra("29/02/2025 khong hop le", !NgayThang::DocTuChuoi("29/02/2025", ngay));
    bo.KiemTra("31/04/2025 khong hop le", !NgayThang::DocTuChuoi("31/04/2025", ngay));
    bo.KiemTra("'abc' khong hop le", !NgayThang::DocTuChuoi("abc", ngay));
    bo.KiemTra("05-11-2026 doc duoc", NgayThang::DocTuChuoi("05-11-2026", ngay) && ngay == NgayThang(5, 11, 2026));
    bo.KiemTra("13/11/2026 la Thu Sau, 01/01/1970 la Thu Nam", NgayThang(13, 11, 2026).ThuTrongTuan() == 5 && NgayThang(1, 1, 1970).ThuTrongTuan() == 4);

    // ------------------------------------------------------------------------
    bo.Nhom("2. TienIch (chuoi, tieng Viet)");
    int so = 0;
    bo.KiemTra("ChuyenSangSo: '12abc' bi tu choi", !ChuyenSangSo("12abc", so));
    bo.KiemTra("ChuyenSangSo: '1.5' (kieu int) bi tu choi", !ChuyenSangSo("1.5", so));
    bo.KiemTra("DinhDangTien(1250000) = 1,250,000", DinhDangTien(1250000) == "1,250,000");
    bo.KiemTra("LaMaHopLe: 'S 01' va 'SÁCH' sai, 'S001' dung", !LaMaHopLe("S 01") && !LaMaHopLe("SÁCH") && LaMaHopLe("S001"));
    bo.KiemTra("UTF-8 -> UTF-16 -> UTF-8 giu nguyen 'Nguyễn Đức'", Utf16SangUtf8(Utf8SangUtf16("Nguyễn Đức")) == "Nguyễn Đức");
    bo.KiemTra("'Nguyễn' (8 byte) dem duoc 6 ky tu", DemKyTuUtf8("Nguyễn") == 6);
    bo.KiemTra("Tim khong dau: 'nguyen duc' khop 'Nguyễn Đức Nghĩa'", ChuaTuKhoa("Nguyễn Đức Nghĩa", "nguyen duc"));
    bo.KiemTra("Tim co dau, khac hoa thuong: 'LẬP TRÌNH' khop 'Nhập môn lập trình'", ChuaTuKhoa("Nhập môn lập trình", "LẬP TRÌNH"));
    bo.KiemTra("So sanh tieng Viet: 'An' < 'Ánh' < 'Bình'", SoSanhTiengViet("An", "Ánh") < 0 && SoSanhTiengViet("Ánh", "Bình") < 0);
    bo.KiemTra("So sanh tieng Viet: 'S2' < 'S10' (so trong chuoi)", SoSanhTiengViet("S2", "S10") < 0);
    bo.KiemTra("OCsv boc o co dau phay / ngoac kep", OCsv("a,b") == "\"a,b\"" && OCsv("x\"y") == "\"x\"\"y\"" && OCsv("abc") == "abc");

    // ------------------------------------------------------------------------
    bo.Nhom("3. DanhSachLienKet (thao tac + sap xep tron)");
    DanhSachLienKet<int> ds;
    for (int i = 1; i <= 5; i++) {
        ds.ThemCuoi(i);
    }

    ds.XoaDauTien([](int x) { return x == 1; });     // xóa đầu
    ds.XoaDauTien([](int x) { return x == 5; });     // xóa cuối
    ds.ThemCuoi(6);                                    // con trỏ cuoi vẫn đúng sau khi xóa cuối
    DanhSachLienKet<int> banSao = ds;
    banSao.XoaHet();
    int tong = 0;
    for (int x : ds) {
        tong += x;
    }

    bo.KiemTra("Xoa dau / xoa cuoi / them cuoi: {2,3,4,6}", ds.KichThuoc() == 4 && tong == 15);
    bo.KiemTra("Sao chep sau: xoa ban sao khong anh huong ban goc", ds.KichThuoc() == 4);

    DanhSachLienKet<int> canSapXep;
    for (int x : { 5, 3, 9, 1, 7, 2, 8, 2 }) {
        canSapXep.ThemCuoi(x);
    }

    canSapXep.SapXep([](int a, int b) { return a < b; });
    std::string ketQuaSapXep;
    for (int x : canSapXep) {
        ketQuaSapXep += std::to_string(x);
    }

    bo.KiemTra("SapXep tang dan: 12235789", ketQuaSapXep == "12235789");
    canSapXep.ThemCuoi(0);      // con trỏ cuoi phải đúng sau khi sắp xếp
    bo.KiemTra("Sau SapXep, ThemCuoi van gan dung vao cuoi", canSapXep.KichThuoc() == 9);

    DanhSachLienKet<CapSo> dsOnDinh;
    for (int i = 0; i < 6; i++) {
        dsOnDinh.ThemCuoi(CapSo{ i % 2, i });          // khóa 0,1,0,1,0,1
    }

    dsOnDinh.SapXep([](const CapSo& a, const CapSo& b) { return a.khoa < b.khoa; });
    std::string thuTu;
    for (const CapSo& c : dsOnDinh) {
        thuTu += std::to_string(c.thuTuBanDau);
    }

    bo.KiemTra("SapXep on dinh: cung khoa giu thu tu cu (024135)", thuTu == "024135");

    DanhSachLienKet<int> dsRong;
    dsRong.SapXep([](int a, int b) { return a < b; });
    DanhSachLienKet<int> dsMot;
    dsMot.ThemCuoi(42);
    dsMot.SapXep([](int a, int b) { return a < b; });
    bo.KiemTra("SapXep danh sach rong / 1 phan tu khong loi", dsRong.Rong() && dsMot.KichThuoc() == 1 && *dsMot.begin() == 42);

    // ------------------------------------------------------------------------
    bo.Nhom("4. Nap du lieu va dang nhap (thu muc co dau)");
    QuanLyThuVien ql(THU_MUC_KIEM_THU);
    const DanhSachLienKet<std::string> canhBao = ql.NapDuLieu();
    bo.KiemTra("Du lieu mau khong co canh bao", canhBao.Rong());
    bo.KiemTra("Dang nhap user1/12345678 dung", ql.KiemTraDangNhap("user1", "12345678"));
    bo.KiemTra("Dang nhap sai mat khau bi tu choi", !ql.KiemTraDangNhap("user1", "sai"));
    bo.KiemTra("Doc du 12 sach, 8 ban doc, 12 phieu", ql.GetDanhSachSach().KichThuoc() == 12 && ql.GetDanhSachBanDoc().KichThuoc() == 8 && ql.GetDanhSachPhieuMuon().KichThuoc() == 12);
    bo.KiemTra("Doc dung tieng Viet co dau", ql.TimBanDoc("BD006") != nullptr && ql.TimBanDoc("BD006")->GetHoTen() == "Đặng Quốc Huy");
    bo.KiemTra("So phieu tiep theo = 13", ql.LaySoPhieuTiepTheo() == 13);
    bo.KiemTra("Goi y ma ban doc moi = BD009", ql.GoiYMaBanDocMoi() == "BD009");

    // ------------------------------------------------------------------------
    bo.Nhom("5. Quan ly sach");
    bo.PhaiBaoLoiNghiepVu("Them sach trung ma S001 bi tu choi", [&] {
        ql.ThemSach(Sach("S001", "x", "y", "z", 1000, 2020, 10, homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them sach gia = 0 bi tu choi", [&] {
        ql.ThemSach(Sach("S100", "Ten", "Tac gia", "NXB", 0, 2020, 100, homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them sach nam phat hanh > nam hien tai bi tu choi", [&] {
        ql.ThemSach(Sach("S100", "Ten", "Tac gia", "NXB", 1000, homNay.GetNam() + 1, 100, homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them sach ngay nhap kho o tuong lai bi tu choi", [&] {
        ql.ThemSach(Sach("S100", "Ten", "Tac gia", "NXB", 1000, 2020, 100, homNay.CongNgay(1)));
    });
    bo.PhaiBaoLoiNhapLieu("Them sach ten chua ky tu '|' bi tu choi", [&] {
        ql.ThemSach(Sach("S100", "Ten|loi", "Tac gia", "NXB", 1000, 2020, 100, homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them sach ma co dau cach bi tu choi", [&] {
        ql.ThemSach(Sach("S 100", "Ten", "Tac gia", "NXB", 1000, 2020, 100, homNay));
    });

    ql.ThemSach(Sach("S013", "Giải tích 2", "Nguyễn Đình Trí", "NXB Giáo dục", 72000, 2019, 310, homNay));
    bo.KiemTra("Them S013: tinh trang = 0", ql.TimSach("S013") != nullptr && ql.TimSach("S013")->GetTinhTrang() == 0);
    bo.KiemTra("Them S013: da ghi noi vao Sach.txt (co dau)", DocFile(THU_MUC_KIEM_THU / CauHinh::FILE_SACH).find("S013|Giải tích 2") != std::string::npos);
    bo.PhaiBaoLoiNghiepVu("Xoa sach dang muon (S003) bi tu choi", [&] { ql.XoaSach("S003"); });
    bo.PhaiBaoLoiNghiepVu("Xoa sach khong ton tai bi tu choi", [&] { ql.XoaSach("S999"); });
    ql.XoaSach("S013");
    bo.KiemTra("Xoa S013: da ghi de Sach.txt", ql.TimSach("S013") == nullptr && DocFile(THU_MUC_KIEM_THU / CauHinh::FILE_SACH).find("S013") == std::string::npos);
    bo.KiemTra("Tim kiem khong dau 'lap trinh' ra 3 sach", ql.TimKiemSach("lap trinh").KichThuoc() == 3);
    bo.KiemTra("Tim kiem theo NXB 'giao duc' ra 4 sach", ql.TimKiemSach("giao duc").KichThuoc() == 4);

    // ------------------------------------------------------------------------
    bo.Nhom("6. Quan ly ban doc");
    bo.PhaiBaoLoiNghiepVu("Them ban doc trung ma BD001 bi tu choi", [&] {
        ql.ThemBanDoc(BanDoc("BD001", "Ai Do", homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them ban doc ho ten rong bi tu choi", [&] {
        ql.ThemBanDoc(BanDoc("BD099", "   ", homNay));
    });
    bo.PhaiBaoLoiNhapLieu("Them ban doc ngay dang ky o tuong lai bi tu choi", [&] {
        ql.ThemBanDoc(BanDoc("BD099", "Ai Do", homNay.CongNgay(3)));
    });
    ql.ThemBanDoc(BanDoc("BD009", "Trương Mỹ Linh", homNay));
    bo.KiemTra("Them BD009: ghi noi vao BanDoc.txt", DocFile(THU_MUC_KIEM_THU / CauHinh::FILE_BAN_DOC).find("BD009|Trương Mỹ Linh") != std::string::npos);
    bo.PhaiBaoLoiNghiepVu("Xoa ban doc dang muon sach (BD002) bi tu choi", [&] { ql.XoaBanDoc("BD002"); });
    bo.PhaiBaoLoiNghiepVu("Xoa ban doc khong ton tai bi tu choi", [&] { ql.XoaBanDoc("BD999"); });
    ql.XoaBanDoc("BD009");
    bo.KiemTra("Xoa BD009: da ghi de BanDoc.txt", ql.TimBanDoc("BD009") == nullptr && DocFile(THU_MUC_KIEM_THU / CauHinh::FILE_BAN_DOC).find("BD009") == std::string::npos);
    bo.KiemTra("BD001 da muon 2 luot, dang muon 0", ql.DemLuotMuonCua("BD001") == 2 && ql.DemPhieuDangMuonCua("BD001") == 0);

    // ------------------------------------------------------------------------
    bo.Nhom("7. Muon / tra sach");
    bo.PhaiBaoLoiNghiepVu("Muon sach dang duoc muon bi tu choi", [&] { ql.MuonSach("S003", "BD001"); });
    bo.PhaiBaoLoiNghiepVu("Muon voi ma ban doc khong ton tai bi tu choi", [&] { ql.MuonSach("S001", "BD999"); });
    bo.PhaiBaoLoiNghiepVu("Muon sach khong ton tai bi tu choi", [&] { ql.MuonSach("S999", "BD001"); });

    const PhieuMuon phieu = ql.MuonSach("S001", "BD004");
    bo.KiemTra("Phieu moi: so 13, tinh trang 1", phieu.GetSoPhieu() == 13 && phieu.GetTinhTrang() == 1);
    bo.KiemTra("Phieu moi: ngay muon = hom nay, ngay phai tra = hom nay + 7", phieu.GetNgayMuon() == homNay && phieu.GetNgayPhaiTra() == homNay.CongNgay(7));
    bo.KiemTra("Tinh trang sach S001 = so phieu 13", ql.TimSach("S001")->GetTinhTrang() == 13);
    bo.KiemTra("PhieuMuon.txt da ghi phieu 13", DocFile(THU_MUC_KIEM_THU / CauHinh::FILE_PHIEU_MUON).find("13|BD004|S001|" + homNay.ChuyenThanhChuoi()) != std::string::npos);

    bo.PhaiBaoLoiNghiepVu("Tra phieu da tra (so 1) bi tu choi", [&] { ql.TraSach(1); });
    bo.PhaiBaoLoiNghiepVu("Tra phieu khong ton tai bi tu choi", [&] { ql.TraSach(99); });
    ql.TraSach(13);
    bo.KiemTra("Tra phieu 13: phieu = 0, sach S001 = 0", ql.TimPhieuMuon(13)->GetTinhTrang() == 0 && ql.TimSach("S001")->GetTinhTrang() == 0);

    // ------------------------------------------------------------------------
    bo.Nhom("8. Thong ke");
    const ThongKe tk = ql.LayThongKe(NgayThang(13, 11, 2026));
    bo.KiemTra("12 sach: 4 dang muon, 8 co san", tk.tongSoSach == 12 && tk.sachDangMuon == 4 && tk.sachCoSan == 8);
    bo.KiemTra("Ngay 13/11/2026: 4 phieu dang muon, 2 qua han", tk.phieuDangMuon == 4 && tk.phieuQuaHan == 2);
    bo.KiemTra("Bieu do: thang cuoi = 11/2026, thang dau = 6/2026", tk.thang[5] == 11 && tk.nam[5] == 2026 && tk.thang[0] == 6);
    const int luotThang11 = 3 + (homNay.GetThang() == 11 && homNay.GetNam() == 2026 ? 1 : 0);
    bo.KiemTra("Bieu do: thang 6 co 2 luot, thang 11 co du so luot", tk.soPhieuTheoThang[0] == 2 && tk.soPhieuTheoThang[5] == luotThang11);
    const ThongKe tkDauNam = ql.LayThongKe(NgayThang(15, 2, 2027));
    bo.KiemTra("Bieu do lui qua nam: 02/2027 -> bat dau tu 09/2026", tkDauNam.thang[0] == 9 && tkDauNam.nam[0] == 2026 && tkDauNam.thang[5] == 2);

    // ------------------------------------------------------------------------
    bo.Nhom("9. Xuat CSV");
    const fs::path fileCsv = THU_MUC_KIEM_THU / L"Danh sách sách.csv";
    XuatCsv(fileCsv, "Mã sách,Tên sách,Giá bán", ql.GetDanhSachSach(), [](const Sach& s) {
        return NoiCsv({ s.GetMaSach(), s.GetTenSach(), std::to_string(s.GetGiaBan()) });
    });
    const std::string noiDungCsv = DocFile(fileCsv);
    int soDong = 0;
    for (char kyTu : noiDungCsv) {
        if (kyTu == '\n') {
            soDong++;
        }
    }

    bo.KiemTra("File CSV bat dau bang BOM UTF-8", noiDungCsv.compare(0, 3, "\xEF\xBB\xBF") == 0);
    bo.KiemTra("File CSV co 1 dong tieu de + 12 dong du lieu", soDong == 13);
    bo.KiemTra("File CSV chua ten sach co dau", noiDungCsv.find("S003,Nhập môn lập trình,80000") != std::string::npos);

    // ------------------------------------------------------------------------
    bo.Nhom("10. Doc lai tu file");
    QuanLyThuVien ql2(THU_MUC_KIEM_THU);
    ql2.NapDuLieu();
    bo.KiemTra("Phieu 13 va sach S001 luu dung trang thai", ql2.TimPhieuMuon(13) != nullptr && ql2.TimPhieuMuon(13)->GetTinhTrang() == 0 && ql2.TimSach("S001")->GetTinhTrang() == 0);
    bo.KiemTra("Sach S003 van dang muon theo phieu 9", ql2.TimSach("S003")->GetTinhTrang() == 9);
    bo.KiemTra("So phieu tiep theo = 14", ql2.LaySoPhieuTiepTheo() == 14);

    {
        std::ofstream file(THU_MUC_KIEM_THU / CauHinh::FILE_BAN_DOC, std::ios::app);
        file << "BD001|Trùng mã|01/01/2025\ndòng sai định dạng\n";
    }

    QuanLyThuVien ql3(THU_MUC_KIEM_THU);
    const DanhSachLienKet<std::string> canhBao3 = ql3.NapDuLieu();
    bo.KiemTra("Dong trung ma va dong sai bi bo qua (2 canh bao, van 8 ban doc)", canhBao3.KichThuoc() == 2 && ql3.GetDanhSachBanDoc().KichThuoc() == 8);

    return bo.KetThuc();
}
