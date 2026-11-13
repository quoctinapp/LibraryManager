#ifndef DANHSACHLIENKET_H
#define DANHSACHLIENKET_H

// ============================================================================
//  DanhSachLienKet.h - CẤU TRÚC DỮ LIỆU: danh sách liên kết đơn dạng template
//      - Lưu danh sách Sách, Bạn đọc, Phiếu mượn, Tài khoản.
//      - Thao tác: ThemCuoi O(1), TimDauTien O(n), XoaDauTien O(n), Loc, DemNeu, SapXep O(n log n) bằng thuật toán sắp xếp trộn (merge sort).
//      - Có bộ duyệt (iterator) để viết: for (const Sach& s : dsSach) {...}
//      - Tuân thủ quy tắc ba: hàm hủy, hàm tạo sao chép, toán tử gán sao chép.
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Danh sách liên kết giống một ĐOÀN TÀU: mỗi toa (Nut) chở một món dữ liệu và móc vào toa phía sau (con trỏ tiep). Ta giữ toa đầu (dau) và toa cuối (cuoi) nên thêm toa mới vào cuối rất nhanh, không phải đi hết đoàn tàu.
//  Muốn xóa một toa, chỉ cần cho toa đứng trước móc thẳng sang toa đứng sau.
//
//  "template <typename T>" = viết MỘT lần, dùng cho MỌI kiểu dữ liệu:
//  DanhSachLienKet<Sach>, DanhSachLienKet<BanDoc>, DanhSachLienKet<std::string>...
//
//  Các hàm Tim / Xoa / Loc nhận vào một "điều kiện" (thường là lambda), ví dụ:
//    dsSach.TimDauTien([&](const Sach& s) { return s.GetMaSach() == "S001"; });
// ----------------------------------------------------------------------------

#include <cstddef>
#include <utility>

template <typename T>
class DanhSachLienKet {
private:
    // [GIẢI THÍCH] Một "toa tàu": dữ liệu + con trỏ tới toa kế tiếp.
    struct Nut {
        T duLieu;
        Nut* tiep;

        explicit Nut(const T& giaTri) : duLieu(giaTri), tiep(nullptr) {}
    };

    Nut* dau;
    Nut* cuoi;
    std::size_t soPhanTu;

    void HoanDoi(DanhSachLienKet& khac) noexcept {
        std::swap(dau, khac.dau);
        std::swap(cuoi, khac.cuoi);
        std::swap(soPhanTu, khac.soPhanTu);
    }

    // [GIẢI THÍCH] TRỘN hai đoàn tàu ĐÃ SẮP XẾP thành một đoàn đã sắp xếp:
    // so sánh toa đầu của hai đoàn, toa nào "nhỏ hơn" thì tháo ra móc vào đoàn kết quả. Khi bằng nhau luôn lấy bên trái trước -> giữ nguyên thứ tự cũ của các phần tử bằng nhau (sắp xếp ỔN ĐỊNH).
    // "duoi" trỏ tới ô con trỏ cần móc toa tiếp theo vào (tránh phải tạo toa giả).
    template <typename SoSanh>
    static Nut* TronHaiDanhSach(Nut* trai, Nut* phai, SoSanh& nhoHon) {
        Nut* ketQua = nullptr;
        Nut** duoi = &ketQua;

        while (trai != nullptr && phai != nullptr) {
            if (nhoHon(phai->duLieu, trai->duLieu)) {
                *duoi = phai;
                phai = phai->tiep;
            } else {
                *duoi = trai;
                trai = trai->tiep;
            }

            duoi = &((*duoi)->tiep);
        }

        *duoi = (trai != nullptr) ? trai : phai;
        return ketQua;
    }

    // [GIẢI THÍCH] SẮP XẾP TRỘN (merge sort) trên danh sách liên kết:
    //   1) Chia đôi đoàn tàu: con trỏ "cham" đi 1 bước, "nhanh" đi 2 bước; khi nhanh tới cuối thì cham đứng ở giữa.
    //   2) Sắp xếp từng nửa (gọi đệ quy).
    //   3) Trộn hai nửa đã sắp xếp.
    // Độ phức tạp O(n log n), không cần mảng phụ, chỉ đổi lại các móc nối.
    template <typename SoSanh>
    static Nut* SapXepTron(Nut* dauDanhSach, SoSanh& nhoHon) {
        if (dauDanhSach == nullptr || dauDanhSach->tiep == nullptr) {
            return dauDanhSach;     // 0 hoặc 1 phần tử: đã sắp xếp
        }

        Nut* cham = dauDanhSach;
        Nut* nhanh = dauDanhSach->tiep;
        while (nhanh != nullptr && nhanh->tiep != nullptr) {
            cham = cham->tiep;
            nhanh = nhanh->tiep->tiep;
        }

        Nut* nuaSau = cham->tiep;
        cham->tiep = nullptr;       // cắt đôi đoàn tàu

        Nut* trai = SapXepTron(dauDanhSach, nhoHon);
        Nut* phai = SapXepTron(nuaSau, nhoHon);
        return TronHaiDanhSach(trai, phai, nhoHon);
    }

public:
    // [GIẢI THÍCH] Bộ duyệt: đi lần lượt từng toa từ đầu đến cuối.
    // Tên begin / end / operator++ là quy ước bắt buộc của C++ cho vòng for-each.
    class BoDuyet {
    private:
        Nut* nutHienTai;

    public:
        explicit BoDuyet(Nut* nut) : nutHienTai(nut) {}
        T& operator*() const { return nutHienTai->duLieu; }
        T* operator->() const { return &nutHienTai->duLieu; }
        BoDuyet& operator++() {
            nutHienTai = nutHienTai->tiep;
            return *this;
        }

        bool operator==(const BoDuyet& khac) const { return nutHienTai == khac.nutHienTai; }
        bool operator!=(const BoDuyet& khac) const { return nutHienTai != khac.nutHienTai; }
    };

    // Bộ duyệt chỉ đọc (dùng cho danh sách const)
    class BoDuyetHang {
    private:
        const Nut* nutHienTai;

    public:
        explicit BoDuyetHang(const Nut* nut) : nutHienTai(nut) {}
        const T& operator*() const { return nutHienTai->duLieu; }
        const T* operator->() const { return &nutHienTai->duLieu; }
        BoDuyetHang& operator++() {
            nutHienTai = nutHienTai->tiep;
            return *this;
        }

        bool operator==(const BoDuyetHang& khac) const { return nutHienTai == khac.nutHienTai; }
        bool operator!=(const BoDuyetHang& khac) const { return nutHienTai != khac.nutHienTai; }
    };

    // ========================================================================
    //  Constructor / Destructor / Sao chép
    // ========================================================================

    DanhSachLienKet() : dau(nullptr), cuoi(nullptr), soPhanTu(0) {}

    // [GIẢI THÍCH] Sao chép = tạo đoàn tàu MỚI có các toa giống hệt (sao chép sâu), để hai danh sách không dùng chung toa (xóa bên này không ảnh hưởng bên kia).
    DanhSachLienKet(const DanhSachLienKet& khac) : DanhSachLienKet() {
        for (const T& giaTri : khac) {
            ThemCuoi(giaTri);
        }
    }

    DanhSachLienKet& operator=(const DanhSachLienKet& khac) {
        if (this != &khac) {
            DanhSachLienKet banSao(khac);
            HoanDoi(banSao);
        }

        return *this;
    }

    ~DanhSachLienKet() {
        XoaHet();
    }

    // ========================================================================
    //  Thao tác cơ bản
    // ========================================================================

    // [GIẢI THÍCH] Móc toa mới vào sau toa cuối - O(1).
    void ThemCuoi(const T& giaTri) {
        Nut* nutMoi = new Nut(giaTri);
        if (cuoi == nullptr) {
            dau = nutMoi;
            cuoi = nutMoi;
        } else {
            cuoi->tiep = nutMoi;
            cuoi = nutMoi;
        }

        ++soPhanTu;
    }

    // [GIẢI THÍCH] Tháo và giải phóng từng toa để không rò rỉ bộ nhớ.
    void XoaHet() {
        Nut* hienTai = dau;
        while (hienTai != nullptr) {
            Nut* nutSau = hienTai->tiep;
            delete hienTai;
            hienTai = nutSau;
        }

        dau = nullptr;
        cuoi = nullptr;
        soPhanTu = 0;
    }

    std::size_t KichThuoc() const { return soPhanTu; }
    bool Rong() const { return soPhanTu == 0; }

    // ========================================================================
    //  Tìm kiếm / xóa / lọc theo điều kiện
    // ========================================================================

    // [GIẢI THÍCH] Tìm tuần tự phần tử ĐẦU TIÊN thỏa điều kiện - O(n).
    // Trả về con trỏ tới phần tử (để đọc / sửa), hoặc nullptr nếu không có.
    template <typename DieuKien>
    T* TimDauTien(DieuKien dieuKien) {
        for (Nut* hienTai = dau; hienTai != nullptr; hienTai = hienTai->tiep) {
            if (dieuKien(hienTai->duLieu)) {
                return &hienTai->duLieu;
            }
        }

        return nullptr;
    }

    template <typename DieuKien>
    const T* TimDauTien(DieuKien dieuKien) const {
        for (const Nut* hienTai = dau; hienTai != nullptr; hienTai = hienTai->tiep) {
            if (dieuKien(hienTai->duLieu)) {
                return &hienTai->duLieu;
            }
        }

        return nullptr;
    }

    // [GIẢI THÍCH] Xóa phần tử ĐẦU TIÊN thỏa điều kiện: toa đứng trước (truoc) móc thẳng sang toa đứng sau. Trả về true nếu có xóa.
    template <typename DieuKien>
    bool XoaDauTien(DieuKien dieuKien) {
        Nut* truoc = nullptr;
        Nut* hienTai = dau;

        while (hienTai != nullptr) {
            if (dieuKien(hienTai->duLieu)) {
                if (truoc == nullptr) {
                    dau = hienTai->tiep;        // xóa toa đầu
                } else {
                    truoc->tiep = hienTai->tiep;
                }

                if (hienTai == cuoi) {
                    cuoi = truoc;               // xóa toa cuối
                }

                delete hienTai;
                --soPhanTu;
                return true;
            }

            truoc = hienTai;
            hienTai = hienTai->tiep;
        }

        return false;
    }

    // Tạo danh sách MỚI gồm các phần tử thỏa điều kiện (dùng cho tìm kiếm sách)
    template <typename DieuKien>
    DanhSachLienKet Loc(DieuKien dieuKien) const {
        DanhSachLienKet ketQua;
        for (const T& giaTri : *this) {
            if (dieuKien(giaTri)) {
                ketQua.ThemCuoi(giaTri);
            }
        }

        return ketQua;
    }

    template <typename DieuKien>
    std::size_t DemNeu(DieuKien dieuKien) const {
        std::size_t dem = 0;
        for (const T& giaTri : *this) {
            if (dieuKien(giaTri)) {
                ++dem;
            }
        }

        return dem;
    }

    // [GIẢI THÍCH] Sắp xếp danh sách theo hàm so sánh nhoHon(a, b) = "a đứng trước b". Ví dụ sắp xếp sách theo giá tăng dần:
    //   dsSach.SapXep([](const Sach& a, const Sach& b) { return a.GetGiaBan() < b.GetGiaBan(); });
    // Dùng khi người dùng bấm vào tiêu đề cột của bảng.
    template <typename SoSanh>
    void SapXep(SoSanh nhoHon) {
        dau = SapXepTron(dau, nhoHon);

        // Sau khi đổi móc nối, đi tìm lại toa cuối
        cuoi = dau;
        while (cuoi != nullptr && cuoi->tiep != nullptr) {
            cuoi = cuoi->tiep;
        }
    }

    // ========================================================================
    //  Bộ duyệt cho vòng for-each
    // ========================================================================

    BoDuyet begin() { return BoDuyet(dau); }
    BoDuyet end() { return BoDuyet(nullptr); }
    BoDuyetHang begin() const { return BoDuyetHang(dau); }
    BoDuyetHang end() const { return BoDuyetHang(nullptr); }
};

#endif
