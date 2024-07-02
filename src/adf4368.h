#ifndef ADF4368_H_
#define ADF4368_H_
#include "adf4368_params.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

/*****************************************************************************/
/*                                  Defines                                  */
/*****************************************************************************/

#define ADF4368_N_REGS (100)
#define ADF4368_N_CACHE_REGS (ADF4368_N_REGS + ADF4368_N_TEST_REG)
#define ADF4368_FREQ_TOLLERANCE_HZ 0.01 // allowed frequency deviation
//
#define ADF4368_N_USER_OVERWRITES 20

#define K(val) ((val) * (uint64_t)1000)
#define M(val) ((val) * (uint64_t)1000000)
#define G(val) ((val) * (uint64_t)1000000000)
#define INT_CEIL(x, y) (((x) + (y) - 1) / (y))
#define N_REGS(param) INT_CEIL(param.width + param.offset, 8)
/*****************************************************************************/
/*                               Public Members                              */
/*****************************************************************************/

typedef enum
{
    ADF4368_ERR_OK = 0,  // no error
    ADF4368_ERR_UNKOWN = 1,  // error not defined
    ADF4368_ERR_NOT_IMPLEMENTED,  // functionality not implemented
    ADF4368_ERR_VCO_F_HIGH,  // VCO frequency too high
    ADF4368_ERR_VCO_F_LOW,  // VCO frequency too low
    ADF4368_ERR_O_INVALID,  // invalid value of O
    ADF4368_ERR_R_HIGH,  // R too high
    ADF4368_ERR_R_LOW,
    ADF4368_ERR_N_LOW,  // Divisor (N) too low
    ADF4368_ERR_N_HIGH,  // Divisor (N) too high
    ADF4368_ERR_F_REF_HIGH, // Refference frequency too high
    ADF4368_ERR_F_REF_LOW, // Refference frequency too low
    ADF4368_ERR_PFD_F_HIGH,  // phase detector frequency too high
    ADF4368_ERR_PFD_F_LOW, // Phase detector frequency too low
    ADF4368_ERR_D_HIGH,  // doubler (D) is too high.
    ADF4368_ERR_D_LOW,  // doubler (D) is too high.
    // Target frequency doesn't match computed freq
    ADF4368_ERR_RF_BAND0_UNACHIEVABLE,
    ADF4368_ERR_RF_BAND1_UNACHIEVABLE,
    ADF4368_ERR_PARAM_INVALID_TYPE,
    ADF4368_ERR_PARAM_ERR, // generic parameter error TODO: more specific??
    ADF4368_ERR_PARAM_TOO_LARGE, // attempted to set a parameter exceeds param width
    ADF4368_ERR_SPI_WRITE_ERR, // attempted to set a parameter exceeds param width
    ADF4368_ERR_SPI_READ_ERR, // attempted to set a parameter exceeds param width
    ADF4368_ERR_WRITE_FCN_NULL,
    ADF4368_ERR_READ_FCN_NULL,
    ADF4368_ERR_IF_TEST_FAILED,
    ADF4368_ERR_MAPPING,
    ADF4368_ERR_FDIV_DCLK_MODE_UNSET,
    ADF4368_ERR_FDIV_RCLK_TOO_HIGH,
    ADF4368_ERR_FREQUENCY_INIT_PARAMS,
    ADF4368_ERR_PARAM_ERR_REFCLK, // generic parameter error TODO: more specific??
    ADF4368_ERR_DCLK_INVALID,
    ADF4368_ERR_WAIT_FCN_NULL,
    ADF4368_ERR_WAIT_FCN_FAIL,
    ADF4368_ERR_FREQ_SWITCH_TIMEOUT,
  ADF4368_ERR_INCORRECT_CAL_MODE,
  ADF4368_ERR_FREQ_OUT_OF_BAND,
} adf4368_err;

/* Read/write mode of ADF4368 SPI transaction. */
typedef enum {
  ADF4368_R,
  ADF4368_W,
} adf4368_rw_mode;

/*Prototype of spi read/write handler to be provided by user.*****************/
typedef adf4368_err(adf4368_write_fcn)(uint8_t register, uint8_t payload);
typedef adf4368_err(adf4368_read_fcn)(uint8_t register, uint8_t *payload);

/*Prototype of wait function provided by user.********************************/
typedef adf4368_err(adf4368_wait_fcn)(unsigned long time_us);

typedef adf4368_err(adf4368_write_burst)(uint8_t register_start,
                                         uint8_t payload,
                                         size_t n); // TODO implement

/* Fractional values. */
typedef struct {
  uint16_t n_int;
  uint32_t mod1;
  uint32_t mod2;
  uint32_t n_frac1;
  uint32_t n_frac2;
} fracn;

/* VCO Calibration paramiters */
typedef struct {
  uint32_t vco_core;
  uint32_t vco_band;
  uint32_t vco_bias;
} adf4368_vco_cal_params;

typedef enum {
  ADF4368_MODE_NDEF = 0,
  ADF4368_MODE_INIT,
  ADF4368_MODE_MANUAL,
  ADF4368_MODE_AUTOCAL,
} adf4368_calibration_mode;

typedef struct {
  uint8_t O;
  uint8_t R;
  uint8_t D;
  uint32_t ref_clk;
  uint64_t band_hz[2];
} ad4368_constants;

typedef struct {
  uint8_t dclk_mode;
  uint8_t dclk_div;
} adf4368_dclk_div;

typedef struct {
  uint32_t synth_lock_timeout;
  uint32_t vco_alc_timeout;
  uint32_t vco_band_div;
  uint32_t adc_clk_div;
} adf4368_timeouts;

/* Frequency settings. */
typedef struct {
  fracn f;
  ad4368_constants c;
  adf4368_dclk_div dclk_div;
  adf4368_timeouts timeouts;
} adf4360_freq_params;

typedef uint8_t adf4368_regs[ADF4368_N_CACHE_REGS];

typedef struct {
  ad4368_constants constants;
  adf4368_calibration_mode cal_mode;
  adf4368_regs registers;
  uint8_t reg_modified[ADF4368_N_CACHE_REGS];
  adf4368_cfg init_param_overwrites[ADF4368_N_USER_OVERWRITES];
  adf4368_write_fcn *write_fcn; // user provided function to write spi payload
  adf4368_read_fcn *read_fcn;   // user provided function to read spi payload
  adf4368_wait_fcn *wait_fcn;
} adf4368_state;

typedef enum {
  ADF4368_FREQ_HOP_MODE_UNKOWN = 0,
  ADF4368_FREQ_HOP_MODE_AUTOCAL,
  ADF4368_FREQ_HOP_MODE_FASTHOP,
} adf4368_freq_hop_mode;

/*Check current frequency parameters are valid. */
adf4368_err adf4368_validate_freq_settings(const adf4368_state *state);

/* Get the device's current frequency from parameter cache.*/
double adf4368_get_freq(const adf4368_state *state);

/* Preform a frequency switch using auto-calibration mode*/
adf4368_err adf4368_switch_freq_autocal(adf4368_state *state, uint64_t freq_hz);

/* Preform a frequency switch using pre-computed calibration parameters.*/
adf4368_err adf4368_switch_freq_mancal(adf4368_state *state, uint64_t freq_hz,
                                       adf4368_vco_cal_params cal);

/* initialise device in auto calibration mode and power on output.*/
adf4368_err adf4368_standard_initialisation(adf4368_state *state,
                                            uint64_t freq);

/* Modify a parameter in the local cache without writing. */
adf4368_err adf4368_stage_param(adf4368_state *state, adf4368_param_hdl id,
                                uint32_t val);

/* Get an unsigned integer parameter from local cache. */
uint32_t adf4368_get_param(const adf4368_state *state, adf4368_param_hdl hdl);

/* Read parameter from the ADF438's registers. */
adf4368_err adf4368_read_uint_param(adf4368_state *state, adf4368_param_hdl id,
                                    uint32_t *val);

/* Write an unsigned integer parameter to the ADF4368's registers. Will sync
 * parameter register(s) before writing.*/
adf4368_err adf4368_write_param(adf4368_state *state, adf4368_param_hdl p_hdl,
                                uint32_t val);

/* Switch between auto/manual calibration mode. */
adf4368_err adf4368_switch_calibration_mode(adf4368_state *state,
                                            adf4368_calibration_mode mode);

/* Duplicate all device registers to the local cache. All staged changes which
 * have not been written will be lost. */
adf4368_err adf4368_read_all(adf4368_state *state);

/* Write all staged parameters to the device in register descending order. */
adf4368_err adf4368_write_staged_regs_decending(adf4368_state *state);

/* Retrieve calibration params from the device's memory*/
adf4368_vco_cal_params adf4368_get_cal_values(const adf4368_state *state);

#endif // ADF4368_H_
