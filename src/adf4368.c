#include "adf4368.h"
#include "adf4368_priv.h"
#include "adf4368_debug.h"
#include "adf4368_params.h"
#include <math.h>
#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MOD1_BIT_WIDTH 25
#define MOD2_BIT_WIDTH 23

const uint64_t mod1_word = 1 << MOD1_BIT_WIDTH; // Modulo one factor.
const uint64_t mod2_word = 1 << MOD2_BIT_WIDTH; // Modulo two factor.
//


adf4368_err _calc_frac_values(uint64_t freq_hz, const ad4368_constants params,
                              fracn *f) {
  //_ASSERT_MSG(params.ref_clk>0, ADF4368_ERR_PARAM_ERR_REFCLK); // would cause
  //a divide by zero error
  f->mod1 = mod1_word;
  f->mod2 = mod2_word;
  uint64_t num = (freq_hz * params.R);
  uint64_t denom = params.D * params.ref_clk;
  f->n_int = num / denom;
  uint64_t residual1 = num % denom;
  f->n_frac1 = (residual1 * mod1_word) / denom;
  uint64_t residual2 = (residual1 * mod1_word) % denom;
  f->n_frac2 = (residual2 * mod2_word) / denom;
  return ADF4368_ERR_OK;
};

// unsafe version of get_staged_param (no checking needed)
uint32_t adf4368_get_param(const adf4368_state *state, adf4368_param_hdl hdl) {
  uint32_t ret;
  adf4368_err err = _get_staged_param(&state->registers, hdl, &ret);
  if(err!=ADF4368_ERR_OK){
    info.err = err;
    return 0;
  };
  return ret;
};


/* Calculate the devisor (N) from the fractional values.**********************/
double _calc_N(fracn f) {
  return (double)((f.n_int + (double)f.n_frac1 / (double)f.mod1) +
                  ((double)f.n_frac2 / ((double)f.mod1 * (double)f.mod2)));
};

/*Caclulate the frequency emitted by the synthesiser. ************************/
double _calc_freq_hz(ad4368_constants c, fracn f) {
  double num = (c.ref_clk * c.D * _calc_N(f)) / c.R;
  return num;
};

/*Calculate the frequency seen by the phase detector. ************************/
double _calc_freq_phase_detect_hz(ad4368_constants c) {
  return (double)(c.D * c.ref_clk) / c.R;
};

//* Calculate the frequency seen by the NCO. *********************************/
double _calc_freq_vco_hz(ad4368_constants c, fracn f) {
  return (double)(_calc_freq_phase_detect_hz(c) * _calc_N(f) * c.O);
};

//* Calculate f_DIV_RCL . *********************************/
uint32_t _calc_fdiv_rclk(ad4368_constants c, adf4368_dclk_div dclk_div) {
  return INT_CEIL((uint32_t)_calc_freq_phase_detect_hz(c),
                  dclk_div.dclk_div * (1u << dclk_div.dclk_mode));
};

uint32_t _calc_synth_lock_timeout(ad4368_constants c,
                                  adf4368_dclk_div dclk_div) {
  return INT_CEIL((uint64_t)200 * _calc_fdiv_rclk(c, dclk_div), M(1));
};

uint32_t _calc_vco_alc_timeout(ad4368_constants c, adf4368_dclk_div dclk_div) {
  return INT_CEIL((uint64_t)50 * _calc_fdiv_rclk(c, dclk_div), M(1));
};

uint32_t _calc_vco_band_div(ad4368_constants c, adf4368_dclk_div dclk_div) {
  return INT_CEIL((uint64_t)15 * _calc_fdiv_rclk(c, dclk_div),
                  (M(1) * 16 * (1u << dclk_div.dclk_mode)));
};

uint32_t _calc_adc_clk_div(ad4368_constants c, adf4368_dclk_div dclk_div) {
  uint32_t num = INT_CEIL(_calc_fdiv_rclk(c, dclk_div), K(400)) - 2;
  return INT_CEIL(num, 4);
};

adf4368_err _check_N(const adf4360_freq_params *p) {
  double N = _calc_N(p->f);
  _ASSERT_MSG(N >= (double)19, ADF4368_ERR_N_LOW);
  _ASSERT_MSG(N <= (double)4095, ADF4368_ERR_N_HIGH);
  return ADF4368_ERR_OK;
};

/*Check output doubler value**************************************************/
adf4368_err _check_O(const adf4360_freq_params *p) {
  _ASSERT_MSG(
      ((p->c.O == 1) || (p->c.O == 2) || (p->c.O == 4) || (p->c.O == 8)),
      ADF4368_ERR_O_INVALID);
  return ADF4368_ERR_OK;
};

/*Check reference Divider value.**********************************************/
adf4368_err _check_R(const adf4360_freq_params *p) {
  _ASSERT_MSG(p->c.R <= 63, ADF4368_ERR_R_HIGH);
  _ASSERT_MSG(p->c.R > 0, ADF4368_ERR_R_LOW);
  return ADF4368_ERR_OK;
};

/*Check the frequency doubler value (should be 0 or 1).***********************/
adf4368_err _check_D(const adf4360_freq_params *p) {
  _ASSERT_MSG(p->c.D <= 2, ADF4368_ERR_D_HIGH);
  _ASSERT_MSG(p->c.D > 0, ADF4368_ERR_D_LOW);
  return ADF4368_ERR_OK;
};

adf4368_err _check_vco(const adf4360_freq_params *p) {
  double vco_f = _calc_freq_vco_hz(p->c, p->f);
  _ASSERT_MSG(vco_f >= (double)G(6.4), ADF4368_ERR_VCO_F_LOW);
  _ASSERT_MSG(vco_f <= (double)G(12.8), ADF4368_ERR_VCO_F_HIGH);
  return ADF4368_ERR_OK;
};

/*Check the phase/frequency detector frequency.*******************************/
adf4368_err _check_pfd(const adf4360_freq_params *p) {
  // FIXME assumes fractional mode with phase re-sync
  // f_out < 3GHz constrained to :75MHz<f_pfd<250MHz
  // depending on application this may be conservative,
  // check the datasheet.
  // TODO integer mode where 3-625MHz
  uint64_t lower_lim = M(75);
  if (p->c.band_hz[0] >= G(3)) {
    lower_lim = M(3);
  }
  double f_pfd = _calc_freq_phase_detect_hz(p->c);
  _ASSERT_MSG(f_pfd > (double)lower_lim, ADF4368_ERR_PFD_F_LOW);
  _ASSERT_MSG(f_pfd < (double)M(250), ADF4368_ERR_PFD_F_HIGH);
  return ADF4368_ERR_OK;
};

adf4368_err _check_reff_clk(const adf4360_freq_params *p) {
  _ASSERT_MSG(p->c.ref_clk >= M(10), ADF4368_ERR_F_REF_LOW);
  _ASSERT_MSG(p->c.ref_clk <= M(4000), ADF4368_ERR_F_REF_HIGH);
  return ADF4368_ERR_OK;
};


adf4368_err _check_d_clk_div(const adf4360_freq_params *p) {
  _ASSERT_MSG((p->dclk_div.dclk_div == 1) || (p->dclk_div.dclk_div == 2) ||
                  (p->dclk_div.dclk_div == 4),
              ADF4368_ERR_DCLK_INVALID);
  return ADF4368_ERR_OK;
};

adf4368_err _check_f_div_rclk(const adf4360_freq_params *p) {
  double fdiv_rclk = _calc_fdiv_rclk(p->c, p->dclk_div);
  _ASSERT_MSG(fdiv_rclk <= M(125), ADF4368_ERR_FDIV_RCLK_TOO_HIGH);
  _ASSERT_MSG((fdiv_rclk < M(80)) || (p->dclk_div.dclk_mode == 1),
              ADF4368_ERR_FDIV_DCLK_MODE_UNSET);
  return ADF4368_ERR_OK;
}

_freq_param_check *_checks[] = {
    &_check_D,          &_check_N,
    &_check_O,          &_check_R,
    &_check_vco,        &_check_pfd,
    &_check_reff_clk,   &_check_d_clk_div,
    &_check_f_div_rclk
};

adf4368_err _check_freq_settings(adf4360_freq_params p) {
  int len = sizeof(_checks) / sizeof(_freq_param_check *);
  for (int i = 0; i < len; i++) {
    _freq_param_check *check_funptr = _checks[i];
    _ASSERT_ERR(check_funptr(&p));
  }
  return ADF4368_ERR_OK;
};

adf4368_err _check_rf_band_achievable(ad4368_constants c) {
  for (int i = 0; i < sizeof(c.band_hz) / sizeof(c.band_hz[0]); i++) {
    uint64_t target_frequency = c.band_hz[i];
    adf4360_freq_params p;
    p.c = c;
    _ASSERT_ERR(_calc_frac_values(target_frequency, c, &p.f));
    p.dclk_div = _calc_dclk_div(c);
    //double freq_calc = _calc_freq_hz(c, p.f);
    //_ASSERT_MSG(fabs((double)target_frequency - freq_calc) <
    //                ADF4368_FREQ_TOLLERANCE_HZ,
    //            ADF4368_ERR_RF_BAND0_UNACHIEVABLE + i);
    _ASSERT_ERR(_check_freq_settings(p));
  }
  return ADF4368_ERR_OK;
};

adf4368_err adf4368_validate_freq_settings(const adf4368_state *state){
  adf4360_freq_params p = _get_freq_params(state);
  _ASSERT_ERR(info.err);
  return _check_freq_settings(p);
}

double adf4368_get_freq(const adf4368_state* state){
  adf4360_freq_params p = _get_freq_params(state);
  return _calc_freq_hz(p.c, p.f);
};

adf4368_err adf4368_read_all(adf4368_state *state) {
  for (int i = 0; i < ADF4368_N_REGS; i++) {
    uint8_t val;
    _ASSERT_ERR(state->read_fcn(i, &val));
    state->registers[i] = val;
  }
  return ADF4368_ERR_OK;
};

// write to and read from the scratchpad to test spi interface
adf4368_err _adf4368_if_test(adf4368_state *state) {
  uint32_t vals2test[] = {0xDE, 0xAD, 0x0};
  uint32_t val;
  for (uint8_t i = 0; i < sizeof(vals2test) / sizeof(uint32_t); i++) {
    _ASSERT_ERR(
        adf4368_stage_param(state, ADF4368_P_SCRATCHPAD, vals2test[i]));
    _ASSERT_ERR(_write_staged_param_reg(state, ADF4368_P_SCRATCHPAD));
    _ASSERT_ERR(adf4368_read_uint_param(state, ADF4368_P_SCRATCHPAD, &val));
    _ASSERT_MSG(val == vals2test[i], ADF4368_ERR_IF_TEST_FAILED);
  }
  return ADF4368_ERR_OK;
};

adf4368_dclk_div _get_dclk_div(const adf4368_state *state) {
  adf4368_dclk_div ret;
  ret.dclk_mode = (uint8_t)adf4368_get_param(state, ADF4368_P_DCLK_MODE);
  ret.dclk_div = (uint8_t)1u
                 << adf4368_get_param(state, ADF4368_P_DCLK_DIV1);
  return ret;
};

fracn _get_fracn(const adf4368_state *state) {
  fracn f;
  f.mod2 = adf4368_get_param(state, ADF4368_P_MOD2WORD);
  f.mod1 = mod1_word;
  f.n_frac1 = adf4368_get_param(state, ADF4368_P_FRAC1WORD);
  f.n_frac2 = adf4368_get_param(state, ADF4368_P_FRAC2WORD);
  f.n_int = adf4368_get_param(state, ADF4368_P_NINT);
  return f;
};

adf4368_vco_cal_params adf4368_get_cal_values(const adf4368_state *state) {
  adf4368_vco_cal_params ret;
  ret.vco_band = adf4368_get_param(state, ADF4368_P_VCO_BAND);
  ret.vco_bias = adf4368_get_param(state, ADF4368_P_VCO_BIAS);
  ret.vco_core = adf4368_get_param(state, ADF4368_P_VCO_CORE);
  return ret;
};

adf4368_vco_cal_params _get_cal_written(const adf4368_state *state) {
  adf4368_vco_cal_params ret;
  ret.vco_band = adf4368_get_param(state, ADF4368_P_M_VCO_BAND);
  ret.vco_bias = adf4368_get_param(state, ADF4368_P_M_VCO_BIAS);
  ret.vco_core = adf4368_get_param(state, ADF4368_P_M_VCO_CORE);
  return ret;
};

adf4368_err _stage_fracn(adf4368_state *state, fracn f) {
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_MOD2WORD, f.mod2));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_FRAC1WORD, f.n_frac1));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_FRAC2WORD, f.n_frac2));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_NINT, f.n_int));
  return ADF4368_ERR_OK;
};

adf4368_err _stage_dclk_div(adf4368_state *state, adf4368_dclk_div dclk) {
  const uint8_t log2lutfloor[] = {[0] = 0, [1] = 0, [2] = 1, [3] = 1, [4] = 2,
                                  [5] = 2, [6] = 2, [7] = 2, [8] = 3};
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_DCLK_DIV1,
                                       log2lutfloor[dclk.dclk_div]));
  _ASSERT_ERR(
      adf4368_stage_param(state, ADF4368_P_DCLK_MODE, dclk.dclk_mode));
  return ADF4368_ERR_OK;
};

ad4368_constants _get_const_vals(const adf4368_state *state) {
  ad4368_constants ret;
  ret.D = (uint8_t)1u << adf4368_get_param(state, ADF4368_P_EN_RDBLR); //  1: D=2; 0: D=1;
  ret.R = (uint8_t)adf4368_get_param(state, ADF4368_P_R_DIV);
  ret.O = (uint8_t)1u << adf4368_get_param(state, ADF4368_P_CLKOUT_DIV);
  ret.ref_clk = state->constants.ref_clk;
  memcpy(&ret.band_hz, &state->constants.band_hz,
         sizeof(state->constants.band_hz));
  return ret;
};

adf4360_freq_params _get_freq_params(const adf4368_state *state) {
  adf4360_freq_params ret;
  ret.c = _get_const_vals(state);
  ret.dclk_div = _get_dclk_div(state);
  ret.f = _get_fracn(state);
  return ret;
};

adf4368_err _stage_const_vals(adf4368_state *state,
                                      ad4368_constants consts) {
  const uint8_t log2lutfloor[] = {[0] = 0, [1] = 0, [2] = 1, [3] = 1, [4] = 2,
                                  [5] = 2, [6] = 2, [7] = 2, [8] = 3};
  _ASSERT_ERR(adf4368_stage_param(
      state, ADF4368_P_EN_RDBLR, consts.D >> 1)); // EN_RDBLR: 1: D=2; 0: D=1;
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_R_DIV, consts.R));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_CLKOUT_DIV,
                                       log2lutfloor[consts.O]));
  return ADF4368_ERR_OK;
};

adf4368_err _stage_freq_settings(adf4368_state *state, adf4360_freq_params p) {
  _ASSERT_ERR(_stage_fracn(state, p.f));           // EN_RDBLR: 1: D=2; 0: D=1;
  _ASSERT_ERR(_stage_dclk_div(state, p.dclk_div)); // EN_RDBLR: 1: D=2; 0: D=1;
  _ASSERT_ERR(_stage_const_vals(state, p.c)); // EN_RDBLR: 1: D=2; 0: D=1;
  return ADF4368_ERR_OK;
};

adf4368_err _stage_manual_cal_values(adf4368_state *state, adf4368_vco_cal_params cal) {
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_M_VCO_CORE, cal.vco_core));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_M_VCO_BAND, cal.vco_band));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_M_VCO_BIAS, cal.vco_bias));
  return ADF4368_ERR_OK;
};

adf4368_err adf4368_write_staged_regs_decending(adf4368_state *state) {
  for (uint8_t i = ADF4368_N_REGS; i > 0; i--) {
    if (state->reg_modified[i] == 1u) {
      _ASSERT_ERR(state->write_fcn(i, state->registers[i]));
      state->reg_modified[i] = 0u;
    }
  }
  return ADF4368_ERR_OK;
};

adf4368_dclk_div _calc_dclk_div(ad4368_constants c) {
  double f_PDF = _calc_freq_phase_detect_hz(c);
  // configure frequency params for autocal
  // Table 17 in ADF4368 datasheet
  adf4368_dclk_div dck_div;
  if (f_PDF < M(160)) {
    dck_div.dclk_div = 1u;
  } else if (f_PDF < M(320)) {
    dck_div.dclk_div = 4u;
  } else {
    dck_div.dclk_div = 8u;
  }
  dck_div.dclk_mode = 1u;
  return dck_div;
};

adf4368_err _stage_timeouts(adf4368_state *state, ad4368_constants c,
                            adf4368_dclk_div dck_div) {
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_SYNTH_LOCK_TIMEOUT,
                                       _calc_synth_lock_timeout(c, dck_div)));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_VCO_ALC_TIMEOUT,
                                       _calc_vco_alc_timeout(c, dck_div)));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_VCO_BAND_DIV,
                                       _calc_vco_band_div(c, dck_div)));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_ADC_CLK_DIV,
                                       _calc_adc_clk_div(c, dck_div)));
  return ADF4368_ERR_OK;
};

adf4368_err _adf4368_stage_autocal_mode(adf4368_state *state,
                                        adf4368_calibration_mode mode) {
  uint32_t autocal_en = (mode == ADF4368_MODE_AUTOCAL);
  // these should be on when in autocall
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_EN_AUTOCAL, autocal_en));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_EN_DRCLK, autocal_en));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_EN_DNCLK, autocal_en));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_EN_ADC_CLK, autocal_en));
  // these should be off when in autocall
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_O_VCO_CORE, !autocal_en));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_O_VCO_BAND, !autocal_en));
  _ASSERT_ERR(adf4368_stage_param(state, ADF4368_P_O_VCO_BIAS, !autocal_en));

  return ADF4368_ERR_OK;
};

adf4368_err adf4368_standard_initialisation(adf4368_state *state, uint64_t freq){
  _ASSERT_ERR(_check_rf_band_achievable(state->constants));
  _ASSERT_MSG(state->wait_fcn != NULL, ADF4368_ERR_WAIT_FCN_NULL);
  _ASSERT_MSG(state->write_fcn!= NULL, ADF4368_ERR_WRITE_FCN_NULL);
  _ASSERT_MSG(state->read_fcn!= NULL, ADF4368_ERR_READ_FCN_NULL);
  // clear reg structs
  for (int i=0; i<ADF4368_N_REGS; i++){
    state->registers[i] = 0;
    state->reg_modified[i] = 0;
  };
  // figure out frequency settings
  // caculate fracn values
  adf4360_freq_params p;
  p.c = state->constants;
  p.dclk_div = _calc_dclk_div(p.c);
  // p.dclk_div.dclk_mode = 1;
  // p.dclk_div.dclk_div = 1;
  _ASSERT_ERR(_calc_frac_values(freq, state->constants, &p.f));
  _ASSERT_ERR(_stage_freq_settings(state, p));
  _ASSERT_ERR(_stage_timeouts(state, p.c, p.dclk_div));
  _ASSERT_ERR(_adf4368_stage_autocal_mode(state, ADF4368_MODE_AUTOCAL));
  _ASSERT_ERR(_check_freq_settings(p));


  const adf4368_cfg default_params[] = {

    // Populate register 0x0 with explicit defaults since 0x0 is always written
    {ADF4368_P_SOFT_RESET,            0}, // No soft reset
    {ADF4368_P_SOFT_RESET_R,          0}, // No soft reset
    {ADF4368_P_LSB_FIRST,             0}, // bit order  set to msb first 
    {ADF4368_P_LSB_FIRST_R,           0}, // bit order  set to msb first
    {ADF4368_P_ADDRESS_ASCENSION,     0}, // Address assention set to auto decrement
    {ADF4368_P_ADDRESS_ASCENSION_R,   0}, // Repeat of above
    {ADF4368_P_SDO_ACTIVE,            ADF4368_SPI_MODE_3WIRE}, // Default to 3 wire mode
    {ADF4368_P_SDO_ACTIVE_R,          ADF4368_SPI_MODE_3WIRE}, // Default to 3 wire mode
    //{ADF4368_P_EN_ADC_CLK,            1}, // Enable the ADC clock (enabled by autocal mode)
    {ADF4368_P_EN_ADC,                1}, // Enable the ADC itself
    {ADF4368_P_EN_ADC_CNV,            1},  // Enable ADC conversion
    //{ADF4368_P_ADC_ST_CNV,            0},  // write this to run a single ADC conversion
    {ADF4368_P_ADC_A_CONV,            1}, // Run conversion automatically on vco cal
    /* Register :0x2B power down settings ----------------------------------------------*/
    {ADF4368_P_PD_LD,                 0}, // enable lock detect disabled (pd by default)
    {ADF4368_P_PD_ALL,                0}, // enable all (pd by default)
    {ADF4368_P_PD_CLKOUT1,            0}, // enable RF output 1 (pd by default)
    {ADF4368_P_PD_CLKOUT2,            0}, // enable RF output 2 (pd by default)
  };
  // load default params defaults
  for (int i = 0; i < sizeof(default_params)/sizeof(adf4368_cfg); i++) {
    adf4368_cfg entry = default_params[i];
    if (entry.hdl == ADF4368_P_PARAM_NOT_DEF)
      break;
    _ASSERT_ERR(adf4368_stage_param(state, entry.hdl, entry.val));
  }
  // user overwrites
  for (int i = 0; i < ADF4368_N_USER_OVERWRITES; i++) {
    adf4368_cfg entry = state->init_param_overwrites[i];
    if (entry.hdl == ADF4368_P_PARAM_NOT_DEF)
      break;
    _ASSERT_ERR(adf4368_stage_param(state, entry.hdl, entry.val));
  }
  // stage all const parameters
  for (uint8_t i = 0; i < ADF4368_N_PARAMS; i++) {
    if ((_param_lut[i].type == ADF4368_T_RES_CONST) && (_param_lut[i].init_val !=0)) {
      _ASSERT_ERR(adf4368_stage_param(state, i, _param_lut[i].init_val));
    }
  }

  //  3. a) et the SDO_ACTIVE and CMOS_OV bits to a desired state for future
  //  readback operations. Choose Between 3-Wire or 4-Wire Operation. 0x0 R/W 0:
  //  3-wire 1: 4-wire SPI (enables SDO and SDIO becomes an input only
  //  only call once since both are in same reg
  _ASSERT_ERR(_write_staged_param_reg(state, ADF4368_P_SDO_ACTIVE)); 

  // logic High Voltage for MUXOUT, LKDET, SDO, SDIO. 0x0 R/W0: 1.8 V Logic.
  // 1: 3.3 V Logi
  _ASSERT_ERR(_write_staged_param_reg(state, ADF4368_P_CMOS_OV));

  // check that the spi interface is working by writing to the scratchpad.
  _ASSERT_ERR(_adf4368_if_test(state))
  // TODO read in the current parameter state of all registers maybe not needed
  //_ASSERT_ERR(_adf4368_sync_all(state)); //
  // 3. b) Program all register addresses in descending order, REG0053 to
  // REG0010. There are several required reserved register field settings
  // provided in Table 19 that are required for proper device operation Set
  // reserverved registers

  // loop through and write all the configured registers in descending order
  for (uint8_t i = 0x53; i > 0x11; i--) {
    if (state->reg_modified[i] == 1u) {
      _ASSERT_ERR(state->write_fcn(i, state->registers[i]));
      state->reg_modified[i] = 0u;
    }
  }
  state->cal_mode = ADF4368_MODE_AUTOCAL;
  // "4. The ADF4368 remains in power-down mode until the PD_ALL bit is
  // programmed to 0. After PD_ALL is disabled, wait at least 10 us for the VCO
  // calibration circuitry and other circuit blocks to settle before starting a
  // VCO calibration"
  // VCO calibration starts when register 0x10 is written, so delay is applied 
  // before writing remaining registers
  _ASSERT_ERR(state->wait_fcn(10));
  _ASSERT_ERR(adf4368_write_staged_regs_decending(state));

  return ADF4368_ERR_OK;
};

adf4368_calibration_mode adf4368_get_calibration_mode(adf4368_state *state) {
  int sum = (adf4368_get_param(state, ADF4368_P_EN_AUTOCAL)) +
            (adf4368_get_param(state, ADF4368_P_EN_DRCLK)) +
            (adf4368_get_param(state, ADF4368_P_EN_DNCLK)) +
            (adf4368_get_param(state, ADF4368_P_EN_ADC_CLK)) +
            (!adf4368_get_param(state, ADF4368_P_O_VCO_CORE)) +
            (!adf4368_get_param(state, ADF4368_P_O_VCO_BAND));
  switch (sum) {
  case 0:
    return ADF4368_MODE_MANUAL;
  case 6:
    return ADF4368_MODE_AUTOCAL;
  default:
    return ADF4368_MODE_NDEF;
  }
};

adf4368_err adf4368_switch_calibration_mode(adf4368_state *state,
                                            adf4368_calibration_mode mode) {
  _ASSERT_MSG((mode == ADF4368_MODE_AUTOCAL) || (mode == ADF4368_MODE_MANUAL),
              ADF4368_ERR_PARAM_ERR)
  _ASSERT_ERR(_adf4368_stage_autocal_mode(state, mode));
  if (mode == ADF4368_MODE_AUTOCAL) {
    ad4368_constants c = _get_const_vals(state);
    adf4368_dclk_div dclk_div = _calc_dclk_div(c);
    _ASSERT_ERR(info.err);
    _ASSERT_ERR(_stage_timeouts(state, c, dclk_div));
    _ASSERT_ERR(adf4368_write_staged_regs_decending(state));
  }
  state->cal_mode = mode;
  return ADF4368_ERR_OK;
};

adf4368_err adf4368_switch_freq_autocal(adf4368_state *state, uint64_t freq_hz) {
  adf4360_freq_params p;

  // Check frequency is in band
  _ASSERT_MSG(freq_hz >= state->constants.band_hz[0] && 
             freq_hz <= state->constants.band_hz[1], ADF4368_ERR_FREQ_OUT_OF_BAND);
  // check the device is in the correct calibration mode
  _ASSERT_MSG(state->cal_mode==ADF4368_MODE_AUTOCAL, ADF4368_ERR_INCORRECT_CAL_MODE);

  p.c = _get_const_vals(state);
  p.dclk_div = _calc_dclk_div(p.c);

  _ASSERT_ERR(_calc_frac_values(freq_hz, p.c, &p.f));
  _ASSERT_ERR(_check_freq_settings(p));
  _ASSERT_ERR(_stage_fracn(state, p.f));
  _ASSERT_ERR(adf4368_write_staged_regs_decending(state));

  // check the ADC and FSM status to confirm hop is complete
  int found = 0;
  for (int i = 0; i<9; i++){
    _ASSERT_ERR(_sync_param_cache(state, ADF4368_P_ADC_BUSY));
    _ASSERT_ERR(_sync_param_cache(state, ADF4368_P_FSM_BUSY));
    uint32_t adc_busy = adf4368_get_param(state, ADF4368_P_ADC_BUSY);
    uint32_t fsm_busy = adf4368_get_param(state, ADF4368_P_FSM_BUSY);
    found = !adc_busy && !fsm_busy;
    if (found)
      break;
  };
  _ASSERT_MSG(found, ADF4368_ERR_FREQ_SWITCH_TIMEOUT);
  return ADF4368_ERR_OK;
};


adf4368_err adf4368_switch_freq_mancal(adf4368_state *state, uint64_t freq_hz,
                                       adf4368_vco_cal_params cal){
  adf4360_freq_params p;
  //
  // check freq is in band
  _ASSERT_MSG(freq_hz >= state->constants.band_hz[0] && 
             freq_hz <= state->constants.band_hz[1], ADF4368_ERR_FREQ_OUT_OF_BAND);
  _ASSERT_MSG(state->cal_mode==ADF4368_MODE_MANUAL, ADF4368_ERR_INCORRECT_CAL_MODE);

  // calculate freq parameters
  p.c = _get_const_vals(state); // retrieve the device's current const values
  p.dclk_div = _calc_dclk_div(p.c);

  // compute the fractionsl values
  _ASSERT_ERR(_calc_frac_values(freq_hz, p.c, &p.f));
  _ASSERT_ERR(_check_freq_settings(p));
  _ASSERT_ERR(_stage_fracn(state, p.f));
  _ASSERT_ERR(_stage_manual_cal_values(state, cal));

  // write freq hop params in decending order
  _ASSERT_ERR(adf4368_write_staged_regs_decending(state));

  return ADF4368_ERR_OK;
};

adf4368_err _sync_param_cache(adf4368_state *state,
                                      adf4368_param_hdl p_hdl) {
  adf4368_param param = _param_lut[p_hdl];
  adf4368_err err = ADF4368_ERR_OK;
  uint8_t val;
  if (state->read_fcn == NULL)
    return ADF4368_ERR_READ_FCN_NULL;
  for (int i = 0; i < N_REGS(param); i++) {
    err = state->read_fcn(param.reg + i, &val); // TODO
    state->registers[param.reg + i] = val;
  };
  return err;
};

/*Write the cached value of a parameter to the device LUT. Usefull for multiple
 * writes.*/
adf4368_err _write_staged_param_reg(adf4368_state *state,
                                            adf4368_param_hdl p_hdl) {
  adf4368_param param = _param_lut[p_hdl];
  adf4368_err err = ADF4368_ERR_OK;
  if (state->write_fcn == NULL)
    return ADF4368_ERR_WRITE_FCN_NULL;
  for (int i = 0; i < N_REGS(param); i++) {
    err = state->write_fcn(param.reg + i, state->registers[param.reg + i]);
    state->reg_modified[param.reg + i] = 0u;
  };
  return err;
};

adf4368_err adf4368_write_param(adf4368_state *state,
                                     adf4368_param_hdl p_hdl, uint32_t val) {
  _ASSERT_ERR(_sync_param_cache(state, p_hdl));
  _ASSERT_ERR(adf4368_stage_param(state, p_hdl, val));
  _ASSERT_ERR(_write_staged_param_reg(state, p_hdl));
  return ADF4368_ERR_OK;
};

adf4368_err adf4368_stage_param(adf4368_state *state,
                                     adf4368_param_hdl p_hdl, uint32_t val) {
  adf4368_param param = _param_lut[p_hdl];
  uint32_t param_max_val = (((uint64_t)1u << param.width) - 1);
  // if (param.type != ADF4368_T_UINT)
  //     return ADF4368_ERR_PARAM_INVALID_TYPE;
  if ((param.width + param.offset) > 64)
    return ADF4368_ERR_PARAM_ERR;
  if (val > param_max_val)
    return ADF4368_ERR_PARAM_TOO_LARGE;
  // get pointer to start of registers
  uint64_t *start_ptr = (uint64_t *)((uint8_t *)state->registers + param.reg);
  // generate bigmask used to clear variables 0000010 when setting bit 1 for
  // example
  uint64_t mask = (uint64_t)param_max_val << param.offset;
  // clear section of register state
  *start_ptr &= ~mask;
  // set data
  uint64_t data = val;
  *start_ptr |= (mask & (data << param.offset));

  // mark the registers as modified
  for (int i = 0; i < (N_REGS(param)); i++) {
    state->reg_modified[param.reg + i] = 1u;
  };
  return ADF4368_ERR_OK;
};

adf4368_err _get_staged_param(const adf4368_regs *reg,
                                          adf4368_param_hdl p_hdl,
                                          uint32_t *val) {
  adf4368_param param = _param_lut[p_hdl];
  // if (param.type != ADF4368_T_UINT)
  // return ADF4368_ERR_PARAM_INVALID_TYPE;
  if ((param.width + param.offset) > 64) {
    return ADF4368_ERR_PARAM_ERR;
  }
  // get pointer to start of registers
  uint8_t *start_ptr = (uint8_t *)reg + param.reg;
  // generate bigmask used to clear variables 1111101 when setting bit 1 for
  // example
  uint64_t mask = (((uint64_t)1u << param.width) - 1) << param.offset;
  // clear section of register state
  *val = (mask & *((uint64_t *)start_ptr)) >> param.offset;
  return ADF4368_ERR_OK;
};

adf4368_err adf4368_read_uint_param(adf4368_state *state,
                                    adf4368_param_hdl p_hdl, uint32_t *val) {
  _ASSERT_ERR(_sync_param_cache(state, p_hdl));
  _ASSERT_ERR(_get_staged_param(&state->registers, p_hdl, val));
  return ADF4368_ERR_OK;
};
