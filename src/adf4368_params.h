#ifndef ADF4368_PARAMS_H_
#define ADF4368_PARAMS_H_
#include <stdint.h>

#define P_PFX(hdl) ADF4368_P_ ## hdl
#define ADF4368_N_PARAMS (190)
#define ADF4368_DESC_WIDTH (24)
#define ADF4368_TEST_REG_OFFSET (0x64)
#define ADF4368_N_TEST_REG (12)

typedef enum
{
P_PFX(PARAM_NOT_DEF)=0,
P_PFX(SOFT_RESET),
P_PFX(LSB_FIRST),
P_PFX(ADDRESS_ASCENSION),
P_PFX(SDO_ACTIVE),
P_PFX(SDO_ACTIVE_R),
P_PFX(ADDRESS_ASCENSION_R),
P_PFX(LSB_FIRST_R),
P_PFX(SOFT_RESET_R),
P_PFX(RESERVED_X01_0),
P_PFX(REG01_RSV0),
P_PFX(REG01_RSV1),
P_PFX(RESERVED_X01_3),
P_PFX(REG01_RSV4),
P_PFX(MAIN_READBACK_CONTROL),
P_PFX(REG01_RSV6),
P_PFX(SINGLE_INSTRUCTION),
P_PFX(CHIP_STATUS),
P_PFX(RESERVED_X02),
P_PFX(CHIP_TYPE),
P_PFX(RESERVED_X03),
P_PFX(PRODUCT_ID),
P_PFX(DEVICE_REVISION),
P_PFX(PRODUCT_GRADE),
P_PFX(SCRATCHPAD),
P_PFX(SPI_REVISION),
P_PFX(VENDOR_ID),
P_PFX(RESERVED_X0F),
P_PFX(NINT),
P_PFX(CLKOUT_DIV),
P_PFX(INT_MODE),
P_PFX(INV_CLKOUT),
P_PFX(FRAC1WORD),
P_PFX(M_VCO_CORE),
P_PFX(M_VCO_BIAS),
P_PFX(CMOS_OV),
P_PFX(M_VCO_BAND),
P_PFX(FRAC2WORD),
P_PFX(MOD2WORD),
P_PFX(BLEED_I),
P_PFX(EN_PHASE_RESY_NC),
P_PFX(EN_REF_RST),
P_PFX(TIME_SYNC),
P_PFX(SW_SYNC),
P_PFX(PHASE_ADJ),
P_PFX(BLEED_POL),
P_PFX(EN_BLEED),
P_PFX(CP_I),
P_PFX(EN_AUTOCAL),
P_PFX(EN_RDBLR),
P_PFX(R_DIV),
P_PFX(PHASE_WORD),
P_PFX(PHASE_ADJUSTMENT),
P_PFX(RESYNC_WAIT),
P_PFX(LSB_P1),
P_PFX(VAR_MODE_EN),
P_PFX(CLK2_OPWR),
P_PFX(CLK1_OPWR),
P_PFX(PHASE_ADJ_POL),
P_PFX(PD_SYNC),
P_PFX(PD_RDET),
P_PFX(PD_ALL),
P_PFX(PD_LD),
P_PFX(PD_CLKOUT1),
P_PFX(PD_CLKOUT2),
P_PFX(LDWIN_PW),
P_PFX(LD_COUNT),
P_PFX(EN_DNCLK),
P_PFX(EN_DRCLK),
P_PFX(EN_LOL),
P_PFX(EN_LDWIN),
P_PFX(EN_RST_LD),
P_PFX(MUXOUT),
P_PFX(EN_CPTEST),
P_PFX(CP_DOWN),
P_PFX(CP_UP),
P_PFX(BST_REF),
P_PFX(FILT_REF),
P_PFX(REF_SEL),
P_PFX(MUTE_NCLK),
P_PFX(DRCLK_DEL),
P_PFX(DNCLK_DEL),
P_PFX(SYNC_DEL),
P_PFX(RESET_SYS),
P_PFX(EN_ADC_CLK),
P_PFX(DCLK_MODE),
P_PFX(CLKODIV_DB),
P_PFX(DLK_DIV_DB),
P_PFX(VCO_BAND_DIV),
P_PFX(SYNTH_LOCK_TIMEOUT),
P_PFX(O_VCO_DB),
P_PFX(VCO_ALC_TIMEOUT),
P_PFX(DEL_CTRL_DB),
P_PFX(ADC_CLK_DIV),
P_PFX(EN_ADC_CNV),
P_PFX(EN_ADC),
P_PFX(ADC_A_CONV),
P_PFX(MUTE_CLKOUT2),
P_PFX(MUTE_CLKOUT1),
P_PFX(ADC_CLK_SEL),
P_PFX(DCLK_DIV1),
P_PFX(O_VCO_BAND),
P_PFX(O_VCO_CORE),
P_PFX(O_VCO_BIAS),
P_PFX(PD_SYNC_MON),
P_PFX(SYNC_SEL),
P_PFX(RST_SYNC_MON),
P_PFX(RESERVED_X54),
P_PFX(ADC_ST_CNV),
P_PFX(EN_CLK2),
P_PFX(EN_CLK1),
P_PFX(SYNC_OK),
P_PFX(REF_OK),
P_PFX(ADC_BUSY),
P_PFX(FSM_BUSY),
P_PFX(LOCKED),
P_PFX(RESERVED_X5A),
P_PFX(VCO_CORE),
P_PFX(CHIP_TEMP),
P_PFX(RESERVED_X5C),
P_PFX(VCO_BAND),
P_PFX(RESERVED_X60),
P_PFX(VCO_BIAS),
P_PFX(RESERVED_X61),
P_PFX(RESERVED_X62),
P_PFX(VERSION),
/*DEBUG parameters.*******************************************************/
P_PFX(TST_1),
P_PFX(TST_2),
P_PFX(TST_3),
P_PFX(TST_4),
P_PFX(TST_5),
P_PFX(TST_6_DEADBEEF),
P_PFX(TST_7_DEAF),
ADF4368_P_CONST_0xF_0,
ADF4368_P_CONST_0x28_0,
ADF4368_P_CONST_0x28_7,
ADF4368_P_CONST_0x2A_0,
ADF4368_P_CONST_0x2A_3,
ADF4368_P_CONST_0x2A_5,
ADF4368_P_CONST_0x2A_7,
ADF4368_P_CONST_0x2B_2,
ADF4368_P_CONST_0x2B_4,
ADF4368_P_CONST_0x2D_0,
ADF4368_P_CONST_0x2D_3,
ADF4368_P_CONST_0x2E_3,
ADF4368_P_CONST_0x2F_0,
ADF4368_P_CONST_0x30_6,
ADF4368_P_CONST_0x31_0,
ADF4368_P_CONST_0x32_0,
ADF4368_P_CONST_0x33_0,
ADF4368_P_CONST_0x34_0,
ADF4368_P_CONST_0x35_0,
ADF4368_P_CONST_0x35_3,
ADF4368_P_CONST_0x36_0,
ADF4368_P_CONST_0x3C_0,
ADF4368_P_CONST_0x3D_0,
ADF4368_P_CONST_0x3F_2,
ADF4368_P_CONST_0x40_6,
ADF4368_P_CONST_0x41_0,
ADF4368_P_CONST_0x42_0,
ADF4368_P_CONST_0x43_0,
ADF4368_P_CONST_0x43_7,
ADF4368_P_CONST_0x44_0,
ADF4368_P_CONST_0x45_0,
ADF4368_P_CONST_0x46_0,
ADF4368_P_CONST_0x47_0,
ADF4368_P_CONST_0x48_0,
ADF4368_P_CONST_0x49_0,
ADF4368_P_CONST_0x4A_0,
ADF4368_P_CONST_0x4B_0,
ADF4368_P_CONST_0x4C_0,
ADF4368_P_CONST_0x4D_0,
ADF4368_P_CONST_0x4E_0,
ADF4368_P_CONST_0x4E_6,
ADF4368_P_CONST_0x4F_0,
ADF4368_P_CONST_0x50_0,
ADF4368_P_CONST_0x51_0,
ADF4368_P_CONST_0x52_0,
ADF4368_P_CONST_0x53_0,
ADF4368_P_CONST_0x53_7,
ADF4368_P_CONST_0x55_0,
ADF4368_P_CONST_0x56_0,
ADF4368_P_CONST_0x57_0,
ADF4368_P_CONST_0x58_4,
ADF4368_P_CONST_0x59_0,
ADF4368_P_CONST_0x5D_0,
ADF4368_P_CONST_0x5F_0,
ADF4368_P_CONST_0x61_0,
ADF4368_P_CONST_0x62_0,
P_PFX(PARAM_END),
} adf4368_param_hdl;


typedef enum
{
    ADF4368_SPI_MODE_3WIRE=0,
    ADF4368_SPI_MODE_4WIRE=1,
} adf4368_spi_mode;

typedef enum
{
ADF4368_OV_1V8=0,
ADF4368_OV_3V3=1,
} adf4368_vout;

typedef enum
{
    ADF4368_SYC_IV_CML = 0,
    ADF4368_SYC_IV_LVDS = 1, // we use LVDS
} adf4368_sync_input_mode;

typedef enum
{
    ADF4368_STREAMING_EN=0,  // streaming mode
    ADF4368_STREAMING_DIS=1, // single instruction mode
} adf4368_instruction_mode;

typedef enum
{
    ADF4368_RBC_SUBORINATE_REG =0,
    ADF4368_RBC_MAIN_REG =1,
} adf4368_main_readback_control_mode;

/* Datatype of param. */
typedef enum // TODO: change this to be RW  vs R
{
    ADF4368_TYPE_NOT_SET = 0, 
    ADF4368_T_INT, // signed integer
    ADF4368_T_UINT, // unsigned integer
    ADF4368_T_RES_CONST, // Reserved must be set to default on init
} adf4368_dtyp;

/* ADF4368 Configuration Parameter. */
typedef struct
{
    adf4368_dtyp type; // type of data held
    uint8_t reg; // register number of param start
    uint8_t width; // bitwidth of the register
    uint8_t offset;  // bit ofset within register
    uint8_t init_val; //
    char desc[ADF4368_DESC_WIDTH];
} adf4368_param;

typedef struct
{
    adf4368_param_hdl hdl;
    uint32_t val;
} adf4368_cfg;

extern const adf4368_param _param_lut[ADF4368_N_PARAMS];

#endif
