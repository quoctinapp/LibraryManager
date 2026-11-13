// ============================================================================
//  [GIẢI THÍCH DỄ HIỂU]
//  Phần "cách làm" của lớp QuanLyThuVien (mục lục ở QuanLyThuVien.h).
//  Mẫu chung của mọi thao tác sửa dữ liệu:
//      1) Kiểm tra ràng buộc / quy định -> sai thì throw, chưa đụng vào dữ liệu.
//      2) Sao lưu danh sách (nhờ hàm tạo sao chép của DanhSachLienKet).
//      3) Sửa trong bộ nhớ rồi ghi file.
//      4) Ghi file lỗi -> trả danh sách về bản sao lưu, báo lỗi cho người dùng.
// ============================================================================

#include "QuanLyThuVien.h"
#include "NgoaiLe.h"
#include "TienIch.h"
#include <cstdio>

// ============================================================================
//  Constructor / nạp dữ liệu
// ============================================================================

QuanLyThuVien::QuanLyThuVien(const std::filesystem::path& thuMucDuLieu) : khoTaiKhoan(thuMucDuLieu / CauHinh::FILE_TAI_KHOAN), khoSach(thuMucDuLieu / CauHinh::FILE_SACH), khoBanDoc(thuMucDuLieu / CauHinh::FILE_BAN_DOC), khoPhieuMuon(thuMucDuLieu / CauHinh::FILE_PHIEU_MUON) {}

template <typename T>
void QuanLyThuVien::NapDanhSach(const KhoDuLieu<T>& kho, DanhSachLienKet<T>& danhSach, DanhSachLienKet<std::string>& canhBao) {
    DanhSachLienKet<T> daDoc;

    int soDongLoi = 0;
    if (!kho.DocTatCa(daDoc, soDongLoi)) {
        canhBao.ThemCuoi("Không tìm thấy file '" + kho.GetTenFile() + "'. Danh sách tương ứng sẽ rỗng.");
    }

    if (soDongLoi > 0) {
        canhBao.ThemCuoi("File '" + kho.GetTenFile() + "' có " + std::to_string(soDongLoi) + " dòng sai định dạng, đã bỏ qua.");
    }

    // [GIẢI THÍCH] Mã phải DUY NHẤT: nếu file có 2 dòng cùng mã thì giữ dòng xuất hiện trước, bỏ dòng sau và báo cảnh báo.
    danhSach.XoaHet();
    for (const T& doiTuong : daDoc) {
        const std::string khoa = doiTuong.LayKhoa();
        const T* daCo = danhSach.TimDauTien([&khoa](const T& khac) {
            return khac.LayKhoa() == khoa;
        });

        if (daCo == nullptr) {
            danhSach.ThemCuoi(doiTuong);
        } else {
            canhBao.ThemCuoi("File '" + kho.GetTenFile() + "': mã '" + khoa + "' bị trùng, đã bỏ qua bản ghi sau.");
        }
    }
}

DanhSachLienKet<std::string> QuanLyThuVien::NapDuLieu() {
    DanhSachLienKet<std::string> canhBao;
    NapDanhSach(khoTaiKhoan, dsTaiKhoan, canhBao);
    NapDanhSach(khoSach, dsSach, canhBao);
    NapDanhSach(khoBanDoc, dsBanDoc, canhBao);
    NapDanhSach(khoPhieuMuon, dsPhieuMuon, canhBao);

    if (dsTaiKhoan.Rong()) {
        canhBao.ThemCuoi("Không có tài khoản hợp lệ nào trong file '" + khoTaiKhoan.GetTenFile() + "'.");
    }

    return canhBao;
}

// ============================================================================
//  Kiểm tra ràng buộc dữ liệu
// ============================================================================

void QuanLyThuVien::KiemTraChu(const std::string& giaTri, const std::string& tenTruong, std::size_t doDaiToiDa) {
    if (CatKhoangTrang(giaTri).empty()) {
        throw LoiNhapLieu(tenTruong + " không được để trống.");
    }

    // [GIẢI THÍCH] Dấu '|' dùng để ngăn các ô trong file, xuống dòng dùng để ngăn các bản ghi -> không cho có trong dữ liệu, kẻo đọc file bị lệch.
    if (giaTri.find_first_of("|\r\n") != std::string::npos) {
        throw LoiNhapLieu(tenTruong + " không được chứa ký tự '|' hoặc xuống dòng.");
    }

    if (DemKyTuUtf8(giaTri) > doDaiToiDa) {
        throw LoiNhapLieu(tenTruong + " dài tối đa " + std::to_string(doDaiToiDa) + " ký tự.");
    }
}

void QuanLyThuVien::KiemTraSach(const Sach& sach) {
    if (!LaMaHopLe(sach.GetMaSach())) {
        throw LoiNhapLieu("Mã sách '" + sach.GetMaSach() + "' không hợp lệ (chỉ gồm chữ cái không dấu và chữ số, tối đa " + std::to_string(CauHinh::DO_DAI_MA_TOI_DA) + " ký tự).");
    }

    KiemTraChu(sach.GetTenSach(), "Tên sách", CauHinh::DO_DAI_TEN_SACH_TOI_DA);
    KiemTraChu(sach.GetTacGia(), "Tác giả", CauHinh::DO_DAI_TEN_TOI_DA);
    KiemTraChu(sach.GetNhaXuatBan(), "Nhà xuất bản", CauHinh::DO_DAI_TEN_TOI_DA);

    if (sach.GetGiaBan() < CauHinh::GIA_BAN_TOI_THIEU || sach.GetGiaBan() > CauHinh::GIA_BAN_TOI_DA) {
        throw LoiNhapLieu("Giá bán phải từ " + DinhDangTien(CauHinh::GIA_BAN_TOI_THIEU) + " đến " + DinhDangTien(CauHinh::GIA_BAN_TOI_DA) + " đồng.");
    }

    const NgayThang homNay = NgayThang::HomNay();
    if (sach.GetNamPhatHanh() < CauHinh::NAM_PHAT_HANH_TOI_THIEU || sach.GetNamPhatHanh() > homNay.GetNam()) {
        throw LoiNhapLieu("Năm phát hành phải từ " + std::to_string(CauHinh::NAM_PHAT_HANH_TOI_THIEU) + " đến năm hiện tại (" + std::to_string(homNay.GetNam()) + ").");
    }

    if (sach.GetSoTrang() < CauHinh::SO_TRANG_TOI_THIEU || sach.GetSoTrang() > CauHinh::SO_TRANG_TOI_DA) {
        throw LoiNhapLieu("Số trang phải từ " + std::to_string(CauHinh::SO_TRANG_TOI_THIEU) + " đến " + std::to_string(CauHinh::SO_TRANG_TOI_DA) + ".");
    }

    const NgayThang ngayNhap = sach.GetNgayNhapKho();
    if (!ngayNhap.HopLe()) {
        throw LoiNhapLieu("Ngày nhập kho không hợp lệ.");
    }

    if (ngayNhap > homNay) {
        throw LoiNhapLieu("Ngày nhập kho (" + ngayNhap.ChuyenThanhChuoi() + ") không được sau ngày hiện tại (" + homNay.ChuyenThanhChuoi() + ").");
    }

    if (ngayNhap.GetNam() < sach.GetNamPhatHanh()) {
        throw LoiNhapLieu("Ngày nhập kho không được trước năm phát hành (" + std::to_string(sach.GetNamPhatHanh()) + ").");
    }
}

void QuanLyThuVien::KiemTraBanDoc(const BanDoc& banDoc) {
    if (!LaMaHopLe(banDoc.GetMaBanDoc())) {
        throw LoiNhapLieu("Mã bạn đọc '" + banDoc.GetMaBanDoc() + "' không hợp lệ (chỉ gồm chữ cái không dấu và chữ số, tối đa " + std::to_string(CauHinh::DO_DAI_MA_TOI_DA) + " ký tự).");
    }

    KiemTraChu(banDoc.GetHoTen(), "Họ tên", CauHinh::DO_DAI_HO_TEN_TOI_DA);

    const NgayThang ngayDangKy = banDoc.GetNgayDangKy();
    if (!ngayDangKy.HopLe()) {
        throw LoiNhapLieu("Ngày đăng ký không hợp lệ.");
    }

    if (ngayDangKy > NgayThang::HomNay()) {
        throw LoiNhapLieu("Ngày đăng ký không được sau ngày hiện tại.");
    }
}

// ============================================================================
//  Đăng nhập / truy vấn
// ============================================================================

bool QuanLyThuVien::KiemTraDangNhap(const std::string& tenDangNhap, const std::string& matKhau) const {
    const TaiKhoan* taiKhoan = dsTaiKhoan.TimDauTien([&](const TaiKhoan& tk) {
        return tk.KhopVoi(tenDangNhap, matKhau);
    });
    return taiKhoan != nullptr;
}

const DanhSachLienKet<Sach>& QuanLyThuVien::GetDanhSachSach() const { return dsSach; }
const DanhSachLienKet<BanDoc>& QuanLyThuVien::GetDanhSachBanDoc() const { return dsBanDoc; }
const DanhSachLienKet<PhieuMuon>& QuanLyThuVien::GetDanhSachPhieuMuon() const { return dsPhieuMuon; }

const Sach* QuanLyThuVien::TimSach(const std::string& maSach) const {
    return dsSach.TimDauTien([&maSach](const Sach& s) { return s.GetMaSach() == maSach; });
}

const BanDoc* QuanLyThuVien::TimBanDoc(const std::string& maBanDoc) const {
    return dsBanDoc.TimDauTien([&maBanDoc](const BanDoc& bd) { return bd.GetMaBanDoc() == maBanDoc; });
}

const PhieuMuon* QuanLyThuVien::TimPhieuMuon(int soPhieu) const {
    return dsPhieuMuon.TimDauTien([soPhieu](const PhieuMuon& pm) { return pm.GetSoPhieu() == soPhieu; });
}

Sach* QuanLyThuVien::TimSachDeSua(const std::string& maSach) {
    return dsSach.TimDauTien([&maSach](const Sach& s) { return s.GetMaSach() == maSach; });
}

PhieuMuon* QuanLyThuVien::TimPhieuMuonDeSua(int soPhieu) {
    return dsPhieuMuon.TimDauTien([soPhieu](const PhieuMuon& pm) { return pm.GetSoPhieu() == soPhieu; });
}

DanhSachLienKet<Sach> QuanLyThuVien::TimKiemSach(const std::string& tuKhoa) const {
    return dsSach.Loc([&tuKhoa](const Sach& s) {
        return ChuaTuKhoa(s.GetMaSach(), tuKhoa) || ChuaTuKhoa(s.GetTenSach(), tuKhoa) || ChuaTuKhoa(s.GetTacGia(), tuKhoa) || ChuaTuKhoa(s.GetNhaXuatBan(), tuKhoa);
    });
}

int QuanLyThuVien::LaySoPhieuTiepTheo() const {
    int soLonNhat = 0;
    for (const PhieuMuon& pm : dsPhieuMuon) {
        if (pm.GetSoPhieu() > soLonNhat) {
            soLonNhat = pm.GetSoPhieu();
        }
    }

    return soLonNhat + 1;
}

// [GIẢI THÍCH] Gợi ý mã bạn đọc kế tiếp: tìm số lớn nhất trong các mã dạng "BDxxx" rồi cộng 1, ví dụ đang có BD008 -> gợi ý BD009.
std::string QuanLyThuVien::GoiYMaBanDocMoi() const {
    int soLonNhat = 0;
    for (const BanDoc& bd : dsBanDoc) {
        const std::string ma = bd.GetMaBanDoc();
        int so = 0;
        if (ma.size() > 2 && ma.compare(0, 2, "BD") == 0 && ChuyenSangSo(ma.substr(2), so) && so > soLonNhat) {
            soLonNhat = so;
        }
    }

    char ketQua[16];
    std::snprintf(ketQua, sizeof(ketQua), "BD%03d", soLonNhat + 1);
    return ketQua;
}

int QuanLyThuVien::DemPhieuDangMuonCua(const std::string& maBanDoc) const {
    return static_cast<int>(dsPhieuMuon.DemNeu([&maBanDoc](const PhieuMuon& pm) {
        return pm.GetMaBanDoc() == maBanDoc && pm.DangMuon();
    }));
}

int QuanLyThuVien::DemLuotMuonCua(const std::string& maBanDoc) const {
    return static_cast<int>(dsPhieuMuon.DemNeu([&maBanDoc](const PhieuMuon& pm) {
        return pm.GetMaBanDoc() == maBanDoc;
    }));
}

ThongKe QuanLyThuVien::LayThongKe(const NgayThang& homNay) const {
    ThongKe tk{};
    tk.tongSoSach = static_cast<int>(dsSach.KichThuoc());
    tk.sachDangMuon = static_cast<int>(dsSach.DemNeu([](const Sach& s) { return !s.ChuaDuocMuon(); }));
    tk.sachCoSan = tk.tongSoSach - tk.sachDangMuon;
    tk.soBanDoc = static_cast<int>(dsBanDoc.KichThuoc());
    tk.tongSoPhieu = static_cast<int>(dsPhieuMuon.KichThuoc());
    tk.phieuDangMuon = static_cast<int>(dsPhieuMuon.DemNeu([](const PhieuMuon& pm) { return pm.DangMuon(); }));
    tk.phieuQuaHan = static_cast<int>(dsPhieuMuon.DemNeu([&homNay](const PhieuMuon& pm) {
        return pm.QuaHan(homNay);
    }));

    // [GIẢI THÍCH] Lùi dần từ tháng hiện tại về 5 tháng trước; tháng 1 lùi tiếp thì thành tháng 12 của năm trước.
    int thang = homNay.GetThang();
    int nam = homNay.GetNam();
    for (int i = CauHinh::SO_THANG_THONG_KE - 1; i >= 0; --i) {
        tk.thang[i] = thang;
        tk.nam[i] = nam;
        tk.soPhieuTheoThang[i] = static_cast<int>(dsPhieuMuon.DemNeu([thang, nam](const PhieuMuon& pm) {
            return pm.GetNgayMuon().GetThang() == thang && pm.GetNgayMuon().GetNam() == nam;
        }));
        if (--thang == 0) {
            thang = 12;
            --nam;
        }
    }

    return tk;
}

// ============================================================================
//  Quản lý sách
// ============================================================================

// [GIẢI THÍCH] Dữ liệu sai -> LoiNhapLieu; trùng mã -> báo "đã tồn tại", không thêm.
// Thêm thành công -> ghi NỐI đúng 1 dòng vào cuối Sach.txt.
void QuanLyThuVien::ThemSach(const Sach& sachMoi) {
    KiemTraSach(sachMoi);
    if (TimSach(sachMoi.GetMaSach()) != nullptr) {
        throw LoiNghiepVu("Sách có mã '" + sachMoi.GetMaSach() + "' đã tồn tại. Không thêm sách!");
    }

    Sach sach = sachMoi;
    sach.SetTinhTrang(CauHinh::SACH_CHUA_MUON);     // sách mới luôn có tình trạng 0

    dsSach.ThemCuoi(sach);
    try {
        khoSach.GhiNoi(sach);
    } catch (const LoiFile&) {
        const std::string ma = sach.GetMaSach();
        dsSach.XoaDauTien([&ma](const Sach& s) { return s.GetMaSach() == ma; });
        throw;
    }
}

// [GIẢI THÍCH] Chỉ xóa được khi sách CÓ trong danh sách và tình trạng = 0.
// Xóa thành công -> ghi ĐÈ toàn bộ danh sách vào Sach.txt.
void QuanLyThuVien::XoaSach(const std::string& maSach) {
    const Sach* sach = TimSach(maSach);
    if (sach == nullptr) {
        throw LoiNghiepVu("Không tìm thấy sách có mã '" + maSach + "' trong danh sách.");
    }

    if (!sach->ChuaDuocMuon()) {
        throw LoiNghiepVu("Không thể xóa sách do sách đang được mượn (phiếu mượn số " + std::to_string(sach->GetTinhTrang()) + ").");
    }

    const DanhSachLienKet<Sach> saoLuu = dsSach;
    dsSach.XoaDauTien([&maSach](const Sach& s) { return s.GetMaSach() == maSach; });

    try {
        khoSach.GhiDeTatCa(dsSach);
    } catch (const LoiFile&) {
        dsSach = saoLuu;
        throw;
    }
}

// ============================================================================
//  Quản lý bạn đọc
// ============================================================================

void QuanLyThuVien::ThemBanDoc(const BanDoc& banDocMoi) {
    KiemTraBanDoc(banDocMoi);
    if (TimBanDoc(banDocMoi.GetMaBanDoc()) != nullptr) {
        throw LoiNghiepVu("Bạn đọc có mã '" + banDocMoi.GetMaBanDoc() + "' đã tồn tại.");
    }

    dsBanDoc.ThemCuoi(banDocMoi);
    try {
        khoBanDoc.GhiNoi(banDocMoi);
    } catch (const LoiFile&) {
        const std::string ma = banDocMoi.GetMaBanDoc();
        dsBanDoc.XoaDauTien([&ma](const BanDoc& bd) { return bd.GetMaBanDoc() == ma; });
        throw;
    }
}

// [GIẢI THÍCH] Bạn đọc còn giữ sách (có phiếu đang mượn) thì không được xóa, nếu không phiếu mượn sẽ trỏ tới một người không còn tồn tại.
void QuanLyThuVien::XoaBanDoc(const std::string& maBanDoc) {
    if (TimBanDoc(maBanDoc) == nullptr) {
        throw LoiNghiepVu("Không tìm thấy bạn đọc có mã '" + maBanDoc + "'.");
    }

    const int soDangMuon = DemPhieuDangMuonCua(maBanDoc);
    if (soDangMuon > 0) {
        throw LoiNghiepVu("Không thể xóa bạn đọc '" + maBanDoc + "' vì đang mượn " + std::to_string(soDangMuon) + " cuốn sách chưa trả.");
    }

    const DanhSachLienKet<BanDoc> saoLuu = dsBanDoc;
    dsBanDoc.XoaDauTien([&maBanDoc](const BanDoc& bd) { return bd.GetMaBanDoc() == maBanDoc; });

    try {
        khoBanDoc.GhiDeTatCa(dsBanDoc);
    } catch (const LoiFile&) {
        dsBanDoc = saoLuu;
        throw;
    }
}

// ============================================================================
//  Quản lý phiếu mượn
// ============================================================================

// [GIẢI THÍCH] Điều kiện mượn: sách tồn tại và tình trạng = 0, mã bạn đọc có trong danh sách. Phiếu mới: số phiếu = số lớn nhất + 1, ngày mượn = hôm nay, ngày phải trả = hôm nay + 7, tình trạng = 1. Tình trạng sách = số phiếu.
PhieuMuon QuanLyThuVien::MuonSach(const std::string& maSach, const std::string& maBanDoc) {
    Sach* sach = TimSachDeSua(maSach);
    if (sach == nullptr) {
        throw LoiNghiepVu("Không tìm thấy sách có mã '" + maSach + "'.");
    }

    if (!sach->ChuaDuocMuon()) {
        throw LoiNghiepVu("Sách '" + maSach + "' đang được mượn (phiếu mượn số " + std::to_string(sach->GetTinhTrang()) + "). Không thể tạo phiếu mượn!");
    }

    if (TimBanDoc(maBanDoc) == nullptr) {
        throw LoiNghiepVu("Mã bạn đọc '" + maBanDoc + "' không có trong danh sách bạn đọc.");
    }

    const DanhSachLienKet<Sach> saoLuuSach = dsSach;
    const DanhSachLienKet<PhieuMuon> saoLuuPhieu = dsPhieuMuon;

    const PhieuMuon phieuMoi(LaySoPhieuTiepTheo(), maBanDoc, maSach, NgayThang::HomNay());
    dsPhieuMuon.ThemCuoi(phieuMoi);
    sach->SetTinhTrang(phieuMoi.GetSoPhieu());

    GhiSachVaPhieuMuon(saoLuuSach, saoLuuPhieu);
    return phieuMoi;
}

// [GIẢI THÍCH] Điều kiện trả: phiếu tồn tại và tình trạng phiếu khác 0.
// Trả thành công: tình trạng phiếu = 0, tình trạng sách trong phiếu = 0.
PhieuMuon QuanLyThuVien::TraSach(int soPhieu) {
    PhieuMuon* phieu = TimPhieuMuonDeSua(soPhieu);
    if (phieu == nullptr) {
        throw LoiNghiepVu("Không tìm thấy phiếu mượn số " + std::to_string(soPhieu) + ".");
    }

    if (!phieu->DangMuon()) {
        throw LoiNghiepVu("Phiếu mượn số " + std::to_string(soPhieu) + " đã được trả trước đó (tình trạng = 0).");
    }

    const DanhSachLienKet<Sach> saoLuuSach = dsSach;
    const DanhSachLienKet<PhieuMuon> saoLuuPhieu = dsPhieuMuon;

    phieu->DanhDauDaTra();
    Sach* sach = TimSachDeSua(phieu->GetMaSach());
    if (sach != nullptr) {
        sach->SetTinhTrang(CauHinh::SACH_CHUA_MUON);
    }

    const PhieuMuon phieuDaTra = *phieu;
    GhiSachVaPhieuMuon(saoLuuSach, saoLuuPhieu);
    return phieuDaTra;
}

void QuanLyThuVien::GhiSachVaPhieuMuon(const DanhSachLienKet<Sach>& saoLuuSach, const DanhSachLienKet<PhieuMuon>& saoLuuPhieu) {
    try {
        khoSach.GhiDeTatCa(dsSach);
        khoPhieuMuon.GhiDeTatCa(dsPhieuMuon);
    } catch (const LoiFile&) {
        // Khôi phục bộ nhớ và cố ghi lại trạng thái cũ để 2 file không lệch nhau
        dsSach = saoLuuSach;
        dsPhieuMuon = saoLuuPhieu;
        try {
            khoSach.GhiDeTatCa(dsSach);
            khoPhieuMuon.GhiDeTatCa(dsPhieuMuon);
        } catch (const LoiFile&) {
            // Bỏ qua: lỗi gốc bên dưới sẽ được báo cho người dùng
        }

        throw;
    }
}
