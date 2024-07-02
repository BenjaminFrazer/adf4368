#include "adf4368_params.h"

#define LUT_UINT(hdl, off, r, w) \
    [ADF4368_P_ ## hdl] = (adf4368_param){.offset=off, .reg=r, .width=w, .type=ADF4368_T_UINT, .desc=#hdl, .init_val=0x0}

#define LUT_CONST(off, r, w, i) \
    [ADF4368_P_CONST_ ## r ## _ ## off] = (adf4368_param){.offset=off, .reg=r, .width=w, .type=ADF4368_T_RES_CONST, .desc="CONST" #r "_" #off, .init_val=(i)}

const adf4368_param _param_lut[ADF4368_N_PARAMS] = {
LUT_UINT(PARAM_NOT_DEF,        0, 0x0,  0),
/*REG 0x0 Param Handle *********off, reg, width******************************/
LUT_UINT(SOFT_RESET,           0, 0x0,  1),
LUT_UINT(LSB_FIRST,            1, 0x0,  1),
LUT_UINT(ADDRESS_ASCENSION,    2, 0x0,  1),
LUT_UINT(SDO_ACTIVE,           3, 0x0,  1),
LUT_UINT(SDO_ACTIVE_R,         4, 0x0,  1),
LUT_UINT(ADDRESS_ASCENSION_R,  5, 0x0,  1),
LUT_UINT(LSB_FIRST_R,          6, 0x0,  1),
LUT_UINT(SOFT_RESET_R,         7, 0x0,  1),
/*REG 0x1 Param Handle *********off, reg, width******************************/
LUT_UINT(RESERVED_X01_0,       0, 0x1,  1),
LUT_UINT(REG01_RSV0,           1, 0x1,  1),
LUT_UINT(REG01_RSV1,           2, 0x1,  1),
LUT_UINT(RESERVED_X01_3,       3, 0x1,  1),
LUT_UINT(REG01_RSV4,           4, 0x1,  1),
LUT_UINT(MAIN_READBACK_CONTROL,5, 0x1,  1),
LUT_UINT(REG01_RSV6,           6, 0x1,  1),
LUT_UINT(SINGLE_INSTRUCTION,   7, 0x1,  1),
/*REG 0x2 Param Handle *********off, reg, width******************************/
LUT_UINT(CHIP_STATUS,          0, 0x2,  4),
LUT_UINT(RESERVED_X02,         4, 0x2,  4),
/*REG 0x3 Param Handle *********off, reg, width******************************/
LUT_UINT(CHIP_TYPE,            0, 0x3,  4),
LUT_UINT(RESERVED_X03,         4, 0x3,  4),
/*REG 0x4-5 Param Handle *********off, reg, width******************************/
LUT_UINT(PRODUCT_ID,           0, 0x4, 16),
/*REG 0x6 Param Handle *********off, reg, width******************************/
LUT_UINT(DEVICE_REVISION,      0, 0x6,  4),
LUT_UINT(PRODUCT_GRADE,        4, 0x6,  4),
/*REG 0x7 Param Handle *********off, reg, width******************************/
/*REG 0x8 Param Handle *********off, reg, width******************************/
/*REG 0x9 Param Handle *********off, reg, width******************************/
/*REG 0xA Param Handle *********off, reg, width******************************/
LUT_UINT(SCRATCHPAD,           0, 0xA,  8),
/*REG 0xB Param Handle *********off, reg, width******************************/
LUT_UINT(SPI_REVISION,         0, 0xB,  8),
/*REG 0xC-D Param Handle *********off, reg, width******************************/
LUT_UINT(VENDOR_ID,            0, 0xC,  16),
/*REG 0xE Param Handle *********off, reg, width******************************/
/*REG 0xF Param Handle *********off, reg, width******************************/
LUT_CONST(0, 0xF,  1, 0),
LUT_UINT(RESERVED_X0F,         1, 0xF,  7),
/*REG 0x10 Param Handle ********off, reg, width******************************/
LUT_UINT(NINT,                 0, 0x10, 12),
/*REG 0x11 Param Handle ********off, reg, width******************************/
LUT_UINT(INV_CLKOUT,           4, 0x11, 1),
LUT_UINT(INT_MODE,             5, 0x11, 1),
LUT_UINT(CLKOUT_DIV,           6, 0x11, 2),
/*REG 0x12 Param Handle ********off, reg, width******************************/
LUT_UINT(FRAC1WORD,            0, 0x12, 25),
/*REG 0x13 Param Handle ********off, reg, width******************************/
/*REG 0x14 Param Handle ********off, reg, width******************************/
/*REG 0x15 Param Handle ********off, reg, width******************************/
LUT_UINT(CMOS_OV,              1, 0x15, 1),
LUT_UINT(M_VCO_BIAS,           2, 0x15, 4),
LUT_UINT(M_VCO_CORE,           6, 0x15, 2),
/*REG 0x16 Param Handle ********off, reg, width******************************/
LUT_UINT(M_VCO_BAND,           0, 0x16, 8),
/*REG 0x17 Param Handle ********off, reg, width******************************/
LUT_UINT(FRAC2WORD,            0, 0x17, 24),
/*REG 0x18 Param Handle ********off, reg, width******************************/
/*REG 0x19 Param Handle ********off, reg, width******************************/
/*REG 0x1A Param Handle ********off, reg, width******************************/
LUT_UINT(MOD2WORD,             0, 0x1A, 24),
/*REG 0x1B Param Handle ********off, reg, width******************************/
/*REG 0x1C Param Handle ********off, reg, width******************************/
/*REG 0x1D Param Handle ********off, reg, width******************************/
LUT_UINT(BLEED_I,              0, 0x1D, 13),
/*REG 0x1E Param Handle ********off, reg, width******************************/
LUT_UINT(TIME_SYNC,            5, 0x1E, 1),
LUT_UINT(EN_REF_RST,           6, 0x1E, 1),
LUT_UINT(EN_PHASE_RESY_NC,     7, 0x1E, 1),
/*REG 0x1F Param Handle ********off, reg, width******************************/
LUT_UINT(CP_I,                 0, 0x1F, 4),
LUT_UINT(EN_BLEED,             4, 0x1F, 1),
LUT_UINT(BLEED_POL,            5, 0x1F, 1),
LUT_UINT(PHASE_ADJ,            6, 0x1F, 1),
LUT_UINT(SW_SYNC,              7, 0x1F, 1),
/*REG 0x20 Param Handle ********off, reg, width******************************/
LUT_UINT(R_DIV,                0, 0x20, 6),
LUT_UINT(EN_RDBLR,             6, 0x20, 1),
LUT_UINT(EN_AUTOCAL,           7, 0x20, 1),
/*REG 0x21 Param Handle ********off, reg, width******************************/
LUT_UINT(PHASE_WORD,           0, 0x21, 24),
/*REG 0x22 Param Handle ********off, reg, width******************************/
/*REG 0x23 Param Handle ********off, reg, width******************************/
/*REG 0x24 Param Handle ********off, reg, width******************************/
LUT_UINT(PHASE_ADJUSTMENT,     0, 0x24, 8),
/*REG 0x25 Param Handle ********off, reg, width******************************/
LUT_UINT(RESYNC_WAIT,          0, 0x25, 24),
/*REG 0x26 Param Handle ********off, reg, width******************************/
/*REG 0x27 Param Handle ********off, reg, width******************************/
/*REG 0x28 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x28, 5, 0),
LUT_UINT(VAR_MODE_EN,          5, 0x28, 1),
LUT_UINT(LSB_P1,               6, 0x28, 1),
LUT_CONST(                     7, 0x28, 1, 0),
/*REG 0x29 Param Handle ********off, reg, width******************************/
LUT_UINT(CLK1_OPWR,            0, 0x29, 4),
LUT_UINT(CLK2_OPWR,            4, 0x29, 4),
/*REG 0x2A Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x2A, 2, 0),
LUT_UINT(PD_RDET,              2, 0x2A, 1),
LUT_CONST(                     3, 0x2A, 1, 0),
LUT_UINT(PD_SYNC,              4, 0x2A, 1),
LUT_CONST(                     5, 0x2A, 1, 0),
LUT_UINT(PHASE_ADJ_POL,        6, 0x2A, 1),
LUT_CONST(                     7, 0x2A, 1, 0),
/*REG 0x2B Param Handle ********off, reg, width******************************/
LUT_UINT(PD_CLKOUT2,           0, 0x2B, 1),
LUT_UINT(PD_CLKOUT1,           1, 0x2B, 1),
LUT_CONST(                     2, 0x2B, 1, 0),
LUT_UINT(PD_LD,                3, 0x2B, 1),
LUT_CONST(                     4, 0x2B, 3, 0),
LUT_UINT(PD_ALL,               7, 0x2B, 1),
/*REG 0x2C Param Handle ********off, reg, width******************************/
LUT_UINT(LD_COUNT,             0, 0x2C, 5),
LUT_UINT(LDWIN_PW,             5, 0x2C, 3),
/*REG 0x2D Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x2D, 2, 0b01),
LUT_UINT(EN_RST_LD,            2, 0x2D, 1),
LUT_CONST(                     3, 0x2D, 1, 0),
LUT_UINT(EN_LDWIN,             4, 0x2D, 1),
LUT_UINT(EN_LOL,               5, 0x2D, 1),
LUT_UINT(EN_DRCLK,             6, 0x2D, 1),
LUT_UINT(EN_DNCLK,             7, 0x2D, 1),
/*REG 0x2E Param Handle ********off, reg, width******************************/
LUT_UINT(CP_UP,                0, 0x2E, 1),
LUT_UINT(CP_DOWN,              1, 0x2E, 1),
LUT_UINT(EN_CPTEST,            2, 0x2E, 1),
LUT_CONST(                     3, 0x2E, 1, 0),
LUT_UINT(MUXOUT,               4, 0x2E, 4),
/*REG 0x2F Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x2F, 5, 0b00111),
LUT_UINT(REF_SEL,              5, 0x2F, 1),
LUT_UINT(FILT_REF,             6, 0x2F, 1),
LUT_UINT(BST_REF,              7, 0x2F, 1),
/*REG 0x30 Param Handle ********off, reg, width******************************/
LUT_UINT(DNCLK_DEL,            0, 0x30, 3),
LUT_UINT(DRCLK_DEL,            3, 0x30, 3),
LUT_CONST(                     6, 0x30, 1, 0),
LUT_UINT(MUTE_NCLK,            7, 0x30, 1),
/*REG 0x31 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x31, 3, 0b001),
LUT_UINT(EN_ADC_CLK,           3, 0x31, 1),
LUT_UINT(RESET_SYS,            4, 0x31, 1),
LUT_UINT(SYNC_DEL,             5, 0x31, 3),
/*REG 0x32 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x32, 8, 0b11010011),
/*REG 0x33 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x33, 8, 0b00110010),
/*REG 0x34 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x34, 8, 0b10011000),
/*REG 0x35 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x35, 2, 0b00),
LUT_UINT(DCLK_MODE,            2, 0x35, 1),
LUT_CONST(                     3, 0x35, 5, 0b00000),
/*REG 0x36 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x36, 6, 0b010110),
LUT_UINT(DLK_DIV_DB,           6, 0x36, 1),
LUT_UINT(CLKODIV_DB,           7, 0x36, 1),
/*REG 0x37 Param Handle ********off, reg, width******************************/
LUT_UINT(VCO_BAND_DIV,         0, 0x37, 8),
/*REG 0x38 Param Handle ********off, reg, width******************************/
LUT_UINT(SYNTH_LOCK_TIMEOUT,   0, 0x38, 15),
/*REG 0x39 Param Handle ********off, reg, width******************************/
LUT_UINT(O_VCO_DB,             7, 0x39, 1),
/*REG 0x3A Param Handle ********off, reg, width******************************/
LUT_UINT(VCO_ALC_TIMEOUT,      0, 0x3A, 15),
/*REG 0x3B Param Handle ********off, reg, width******************************/
LUT_UINT(DEL_CTRL_DB,          7, 0x3B, 1),
/*REG 0x3C Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x3C, 8, 0b00000000),
/*REG 0x3D Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x3D, 8, 0b11000000),
/*REG 0x3E Param Handle ********off, reg, width******************************/
LUT_UINT(ADC_CLK_DIV,          0, 0x3E, 8),
/*REG 0x3F Param Handle ********off, reg, width******************************/
LUT_UINT(ADC_A_CONV,           0, 0x3F, 1),
LUT_UINT(EN_ADC,               1, 0x3F, 1),
LUT_CONST(                     2, 0x3F, 5, 0b00000),
LUT_UINT(EN_ADC_CNV,           7, 0x3F, 1),
/*REG 0x40 Param Handle ********off, reg, width******************************/
LUT_UINT(MUTE_CLKOUT1,         0, 0x40, 3),
LUT_UINT(MUTE_CLKOUT2,         3, 0x40, 3),
LUT_CONST(                     6, 0x40, 2, 0b00),
/*REG 0x41 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x41, 8, 0b00000000),
/*REG 0x42 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x42, 8, 0b11000000),
/*REG 0x43 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x43, 6, 0b001001),
LUT_UINT(ADC_CLK_SEL,          6, 0x43, 1),
LUT_CONST(                     7, 0x43, 1, 0b0),
/*REG 0x44 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x44, 8, 0b00011000),
/*REG 0x45 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x45, 8, 0b00001000),
/*REG 0x46 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x46, 8, 0b00000000),
/*REG 0x47 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x47, 8, 0),
/*REG 0x48 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x48, 8, 0),
/*REG 0x49 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x49, 8, 0),
/*REG 0x4A Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4A, 8, 0),
/*REG 0x4B Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4B, 8, 0b01011101),
/*REG 0x4C Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4C, 8, 0b00101011),
/*REG 0x4D Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4D, 8, 0),
/*REG 0x4E Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4E, 1, 0),
LUT_UINT(O_VCO_BIAS,           1, 0x4E, 1),
LUT_UINT(O_VCO_CORE,           2, 0x4E, 1),
LUT_UINT(O_VCO_BAND,           3, 0x4E, 1),
LUT_UINT(DCLK_DIV1,            4, 0x4E, 2),
LUT_CONST(                     6, 0x4E, 2, 0),
/*REG 0x4F Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x4F, 8, 0),
/*REG 0x50 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x50, 8, 0),
/*REG 0x51 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x51, 8, 0),
/*REG 0x52 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x52, 8, 0),
/*REG 0x53 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x53, 4, 0b0101),
LUT_UINT(RST_SYNC_MON,         4, 0x53, 1),
LUT_UINT(SYNC_SEL,             5, 0x53, 1),
LUT_UINT(PD_SYNC_MON,          6, 0x53, 1),
LUT_CONST(                     7, 0x53, 1, 0),
/*REG 0x54 Param Handle ********off, reg, width******************************/
LUT_UINT(ADC_ST_CNV,           0, 0x54, 1),
LUT_UINT(RESERVED_X54,         1, 0x54, 7),
/*REG 0x55 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x55, 8, 0),
/*REG 0x56 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x56, 8, 0),
/*REG 0x57 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x57, 8, 0),
/*REG 0x58 Param Handle ********off, reg, width******************************/
LUT_UINT(LOCKED,               0, 0x58, 1),
LUT_UINT(FSM_BUSY,             1, 0x58, 1),
LUT_UINT(ADC_BUSY,             2, 0x58, 1),
LUT_UINT(REF_OK,               3, 0x58, 1),
LUT_CONST(                     4, 0x58, 1, 0),
LUT_UINT(SYNC_OK,              5, 0x58, 1),
LUT_UINT(EN_CLK1,              6, 0x58, 1),
LUT_UINT(EN_CLK2,              7, 0x58, 1),
/*REG 0x59 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x59, 8, 0),
/*REG 0x5A Param Handle ********off, reg, width******************************/
LUT_UINT(VCO_CORE,             0, 0x5A, 2),
LUT_UINT(RESERVED_X5A,         2, 0x5A, 6),
/*REG 0x5B Param Handle ********off, reg, width******************************/
LUT_UINT(CHIP_TEMP,            0, 0x5B, 9),
/*REG 0x5C Param Handle ********off, reg, width******************************/
LUT_UINT(RESERVED_X5C,         1, 0x5C, 7),
/*REG 0x5D Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x5D, 8, 0),
/*REG 0x5E Param Handle ********off, reg, width******************************/
LUT_UINT(VCO_BAND,             0, 0x5E, 8),
/*REG 0x5F Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x5F, 8, 0),
/*REG 0x60 Param Handle ********off, reg, width******************************/
LUT_UINT(VCO_BIAS,             0, 0x60, 4),
LUT_UINT(RESERVED_X60,         4, 0x60, 4),
/*REG 0x61 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x61, 4, 0),
LUT_UINT(RESERVED_X61,         4, 0x61, 4),
/*REG 0x62 Param Handle ********off, reg, width******************************/
LUT_CONST(                     0, 0x62, 3, 0),
LUT_UINT(RESERVED_X62,         3, 0x62, 5),
/*REG 0x63 Param Handle ********off, reg, width******************************/
LUT_UINT(VERSION,              0, 0x63, 8),
/*Test parameters.*********************************************************/
LUT_UINT(TST_1,                0, ADF4368_TEST_REG_OFFSET+0x00,  1),
LUT_UINT(TST_2,                1, ADF4368_TEST_REG_OFFSET+0x00,  1),
LUT_UINT(TST_3,                2, ADF4368_TEST_REG_OFFSET+0x00,  2),
LUT_UINT(TST_4,                4, ADF4368_TEST_REG_OFFSET+0x01,  8),
LUT_UINT(TST_5,                1, ADF4368_TEST_REG_OFFSET+0x03,  7),
LUT_UINT(TST_6_DEADBEEF,       4, ADF4368_TEST_REG_OFFSET+0x04, 32),
LUT_UINT(TST_7_DEAF,           4, ADF4368_TEST_REG_OFFSET+0x09, 16),
/*END of LUT******************************************************************/
[ADF4368_P_PARAM_END] = (adf4368_param){.type = ADF4368_TYPE_NOT_SET, .desc="END", .width = 0, .reg = 0, .offset = 0},
};
