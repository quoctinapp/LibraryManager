# ============================================================================
#  [GIẢI THÍCH DỄ HIỂU]
#  Makefile = "công thức nấu" để biên dịch chương trình, chạy bằng lệnh: mingw32-make
#  Mỗi file .cpp được dịch riêng thành file .o trong thư mục build/. Lần sau
#  chỉ những file .cpp VỪA SỬA (hoặc file .h nó dùng vừa sửa) mới phải dịch
#  lại -> build nhanh hơn. Dùng thêm "-j16" để dịch nhiều file cùng lúc.
#
#  Mã nguồn chia 2 phần:
#   - LÕI (không có giao diện): cautrucdulieu, thucthe, luutru, nghiepvu, tienich
#     -> dùng chung cho chương trình chính và chương trình kiểm thử.
#   - GIAO DIỆN Win32: src (main, ChuongTrinh), nentang, giaodien.
#
#  Các lệnh:
#    mingw32-make -j16          dịch -> build/QuanLyThuVien.exe (bản gỡ lỗi, F5 dùng)
#    mingw32-make run           dịch rồi mở chương trình
#    mingw32-make -j16 test     dịch và chạy chương trình kiểm thử tự động
#    mingw32-make phathanh      tạo release/QuanLyThuVien.exe: MỘT file duy nhất để nộp / gửi
#    mingw32-make clean         xóa thư mục build/ để dịch lại từ đầu
# ============================================================================

# Luôn chạy các lệnh bằng cmd.exe của Windows (kể cả khi gọi make từ Git Bash)
SHELL    = cmd.exe
CXX      = g++
WINDRES  = windres

THU_MUC_LOI = src/cautrucdulieu src/thucthe src/luutru src/nghiepvu src/tienich
THU_MUC_GUI = src src/nentang src/giaodien
THU_MUC     = $(THU_MUC_LOI) $(THU_MUC_GUI)

# -DUNICODE: dùng bản "W" (Unicode) của Windows API -> hiển thị tiếng Việt có dấu
# -MMD -MP : tự ghi lại file .cpp nào dùng file .h nào (để biết cần dịch lại gì)
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O1 -DUNICODE -D_UNICODE \
           -finput-charset=UTF-8 -fexec-charset=UTF-8 -MMD -MP $(addprefix -I,$(THU_MUC))
# Thư viện có sẵn của Windows (không phải thư viện ngoài):
#   comctl32 (bảng, thanh trạng thái, ô chọn ngày, TaskDialog), gdi32 (vẽ), msimg32 (tô chuyển màu),
#   comdlg32 (hộp thoại "Lưu thành"), uxtheme (giao diện Explorer), shell32 (mở file CSV)
LDLIBS   = -lcomctl32 -lgdi32 -lmsimg32 -lcomdlg32 -luxtheme -lshell32
# -mwindows: ứng dụng cửa sổ (không mở console); -municode: điểm vào wWinMain;
# -static: exe chạy được ở máy khác mà không cần DLL của MinGW
# -B...    : MinGW tự gắn sẵn một manifest mặc định, trùng với manifest của chương trình
#            (xem TaiNguyen.rc). Trỏ GCC tới một default-manifest.o RỖNG để chỉ còn manifest của ta.
KHONG_MANIFEST = build/khongmanifest/default-manifest.o
LDFLAGS  = -static -mwindows -municode -Bbuild/khongmanifest/

EXE      = build/QuanLyThuVien.exe
EXE_PH   = release/QuanLyThuVien.exe
TEST_EXE = build/KiemThu.exe

SRC_LOI  = $(foreach d,$(THU_MUC_LOI),$(wildcard $(d)/*.cpp))
SRC_GUI  = $(foreach d,$(THU_MUC_GUI),$(wildcard $(d)/*.cpp))
OBJ_LOI  = $(addprefix build/,$(notdir $(SRC_LOI:.cpp=.o)))
OBJ_GUI  = $(addprefix build/,$(notdir $(SRC_GUI:.cpp=.o)))
OBJ      = $(OBJ_LOI) $(OBJ_GUI)
vpath %.cpp $(THU_MUC)

# Icon + manifest + dữ liệu mẫu được nhúng vào exe (xem TaiNguyen.rc)
RES      = build/TaiNguyen.o
TAI_NGUYEN = TaiNguyen.rc QuanLyThuVien.manifest data/icon.ico data/Admin.txt data/Sach.txt \
             data/BanDoc.txt data/PhieuMuon.txt

all: $(EXE)

$(EXE): $(OBJ) $(RES) $(KHONG_MANIFEST)
	$(CXX) $(OBJ) $(RES) -o $@ $(LDFLAGS) $(LDLIBS)

$(RES): $(TAI_NGUYEN) | build
	$(WINDRES) TaiNguyen.rc -O coff -o $@

$(KHONG_MANIFEST): | build
	if not exist build\khongmanifest mkdir build\khongmanifest
	echo // doi tuong rong thay cho default-manifest.o cua MinGW> build\khongmanifest\rong.c
	gcc -c build/khongmanifest/rong.c -o $@

# Dịch từng file .cpp (tìm trong các thư mục src/...) thành build/<tên>.o
build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/KiemThu.o: tests/KiemThu.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	if not exist build mkdir build

run: $(EXE)
	start "" build\QuanLyThuVien.exe

# Chương trình kiểm thử (console) chỉ dùng phần LÕI, không cần giao diện
$(TEST_EXE): build/KiemThu.o $(OBJ_LOI)
	$(CXX) build/KiemThu.o $(OBJ_LOI) -o $@ -static

test: $(TEST_EXE)
	build\KiemThu.exe

# Bản phát hành: MỘT file exe duy nhất (bỏ thông tin gỡ lỗi -s, tối ưu -O2 khi liên kết)
phathanh: $(OBJ) $(RES) $(KHONG_MANIFEST)
	if not exist release mkdir release
	$(CXX) $(OBJ) $(RES) -o $(EXE_PH) $(LDFLAGS) -s $(LDLIBS)

clean:
	if exist build rmdir /s /q build

.PHONY: all run test clean phathanh

-include $(OBJ:.o=.d) build/KiemThu.d
