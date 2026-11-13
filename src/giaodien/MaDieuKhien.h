#ifndef MADIEUKHIEN_H
#define MADIEUKHIEN_H

// ============================================================================
//  MaDieuKhien.h - Mã số (ID) của các điều khiển và phím tắt trong giao diện
// ============================================================================

// ----------------------------------------------------------------------------
//  [GIẢI THÍCH DỄ HIỂU]
//  Khi một nút được bấm, Windows báo cho cửa sổ cha bằng WM_COMMAND kèm MÃ SỐ của nút. Gom các mã về một chỗ để không bị trùng và dễ đọc hơn số trần.
//  IDOK (1) và IDCANCEL (2) là mã chuẩn của Windows cho nút Đồng ý / Hủy:
//  phím Enter tự bấm IDOK, phím ESC tự bấm IDCANCEL.
// ----------------------------------------------------------------------------

enum MaDieuKhien {
    // ---- Trang nội dung ----
    MA_BANG = 1001,
    MA_TIM_KIEM,
    MA_HOP_LOC,
    MA_NUT_THEM,
    MA_NUT_XOA,
    MA_NUT_MUON,
    MA_NUT_TRA,
    MA_NUT_XUAT,

    // ---- Màn hình đăng nhập ----
    MA_O_TEN_DANG_NHAP = 1101,
    MA_O_MAT_KHAU,

    // ---- Các trường trong hộp thoại ----
    MA_TRUONG_1 = 1201,
    MA_TRUONG_2,
    MA_TRUONG_3,
    MA_TRUONG_4,
    MA_TRUONG_5,
    MA_TRUONG_6,
    MA_TRUONG_7,
    MA_TRUONG_8,

    // ---- Phím tắt cửa sổ chính ----
    MA_PHIM_TRANG_1 = 1301,     // Ctrl+1 .. Ctrl+4
    MA_PHIM_TRANG_2,
    MA_PHIM_TRANG_3,
    MA_PHIM_TRANG_4,
    MA_PHIM_LAM_MOI,            // F5
    MA_PHIM_TIM_KIEM            // Ctrl+F
};

#endif
