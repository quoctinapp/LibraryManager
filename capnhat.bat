@echo off
REM ============================================================================
REM  capnhat.bat - Cap nhat code len GitHub chi bang MOT lenh
REM
REM  Cach dung (mo terminal o thu muc goc du an):
REM     capnhat                        loi nhan mac dinh: "Cap nhat <ngay> <gio>"
REM     capnhat "Them chuc nang X"     loi nhan tu dat
REM  Hoac trong VS Code: Terminal > Run Task > "Git: Cap nhat len GitHub"
REM
REM  [GIAI THICH] Cac buoc lam lan luot, buoc nao loi thi DUNG LAI (khong day
REM  code hong len GitHub):
REM     1) Dong goi lai release\QuanLyThuVien.exe tu code moi nhat
REM     2) Chay kiem thu tu dong (tests\KiemThu.cpp)
REM     3) git add + git commit
REM     4) git push len https://github.com/quoctinapp/LibraryManager
REM ============================================================================
chcp 65001 >nul
setlocal
cd /d "%~dp0"

where git >nul 2>nul || (echo [LOI] Chua cai Git hoac Git khong co trong PATH. & exit /b 1)
where mingw32-make >nul 2>nul || (echo [LOI] Khong tim thay mingw32-make ^(xem README muc 2^). & exit /b 1)

REM ---- Co thay doi nao khong? ----
set "CO_THAY_DOI="
for /f "delims=" %%i in ('git status --porcelain') do set "CO_THAY_DOI=1"
if not defined CO_THAY_DOI (
    echo Khong co thay doi nao de cap nhat. Moi thu da giong tren GitHub.
    exit /b 0
)

REM ---- Chuong trinh dang mo thi khong ghi de duoc file exe ----
tasklist /fi "imagename eq QuanLyThuVien.exe" 2>nul | find /i "QuanLyThuVien.exe" >nul
if not errorlevel 1 (
    echo [LOI] QuanLyThuVien.exe dang chay. Hay dong chuong trinh roi chay lai lenh capnhat.
    exit /b 1
)

REM ---- Loi nhan commit ----
set "LOI_NHAN=%~1"
if "%LOI_NHAN%"=="" set "LOI_NHAN=Cap nhat %date% %time:~0,5%"

echo.
echo [1/4] Dong goi release\QuanLyThuVien.exe ...
mingw32-make -j16 phathanh >nul
if errorlevel 1 (echo [LOI] Bien dich that bai - xem loi o tren. Da dung lai. & exit /b 1)

echo [2/4] Chay kiem thu tu dong ...
if not exist build mkdir build
mingw32-make -j16 test > build\ketqua_kiemthu.txt 2>&1
if errorlevel 1 (
    type build\ketqua_kiemthu.txt
    echo [LOI] Kiem thu KHONG DAT - xem cac dong [SAI] o tren. Da dung lai.
    exit /b 1
)
for /f "delims=" %%i in ('findstr /c:"Ket qua:" build\ketqua_kiemthu.txt') do echo       %%i

echo [3/4] Commit: "%LOI_NHAN%"
git add -A
git commit -q -m "%LOI_NHAN%"
if errorlevel 1 (echo [LOI] Commit that bai. & exit /b 1)

echo [4/4] Day len GitHub ...
git push -q
if errorlevel 1 (echo [LOI] Push that bai - kiem tra mang / dang nhap GitHub. Commit van con o may. & exit /b 1)

echo.
for /f "delims=" %%i in ('git log -1 --format^="%%h  %%s"') do echo XONG! Da cap nhat: %%i
echo Xem tai: https://github.com/quoctinapp/LibraryManager
endlocal
