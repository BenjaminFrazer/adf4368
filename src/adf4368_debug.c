#include <string.h>
#include <stdio.h>
#include "adf4368_debug.h"
#include "adf4368_priv.h"


#define ADF4368_CHECK_ERRS // comment this out to avoid error checking

#define ERROR_CODE(CODE) [(CODE)]=#CODE,

// parse out just the base name of the file

err_info info = {.err = ADF4368_ERR_OK, .file_name = "", .lineno = 0}; // declare the internal held error info object

const error_msg adf4368_error_msg = {
    ERROR_CODE(ADF4368_ERR_OK)
    ERROR_CODE(ADF4368_ERR_UNKOWN)
    ERROR_CODE(ADF4368_ERR_NOT_IMPLEMENTED)  // functionality not implemented
    ERROR_CODE(ADF4368_ERR_VCO_F_HIGH)  // VCO frequency too high
    ERROR_CODE(ADF4368_ERR_VCO_F_LOW)  // VCO frequency too low
    ERROR_CODE(ADF4368_ERR_O_INVALID)  // invalid value of O
    ERROR_CODE(ADF4368_ERR_R_HIGH)  // R too high
    ERROR_CODE(ADF4368_ERR_R_LOW)  // R too high
    ERROR_CODE(ADF4368_ERR_N_LOW)  // Divisor (N) too low
    ERROR_CODE(ADF4368_ERR_N_HIGH)  // Divisor (N) too high
    ERROR_CODE(ADF4368_ERR_F_REF_HIGH) // Refference frequency too high
    ERROR_CODE(ADF4368_ERR_F_REF_LOW) // Refference frequency too low
    ERROR_CODE(ADF4368_ERR_PFD_F_HIGH)  // phase detector frequency too high
    ERROR_CODE(ADF4368_ERR_PFD_F_LOW) // Phase detector frequency too low
    ERROR_CODE(ADF4368_ERR_D_HIGH)  // doubler (D) is too high.
    ERROR_CODE(ADF4368_ERR_D_LOW)
    ERROR_CODE(ADF4368_ERR_PARAM_INVALID_TYPE)
    ERROR_CODE(ADF4368_ERR_PARAM_TOO_LARGE)
    ERROR_CODE(ADF4368_ERR_SPI_WRITE_ERR)// attempted to set a parameter exceeds param width
    ERROR_CODE(ADF4368_ERR_SPI_READ_ERR) // attempted to set a parameter exceeds param width
    ERROR_CODE(ADF4368_ERR_WRITE_FCN_NULL)
    ERROR_CODE(ADF4368_ERR_READ_FCN_NULL)
    ERROR_CODE(ADF4368_ERR_IF_TEST_FAILED)
    ERROR_CODE(ADF4368_ERR_MAPPING)
    ERROR_CODE(ADF4368_ERR_FDIV_DCLK_MODE_UNSET)
    ERROR_CODE(ADF4368_ERR_FDIV_RCLK_TOO_HIGH)
    ERROR_CODE(ADF4368_ERR_FREQUENCY_INIT_PARAMS)
    ERROR_CODE(ADF4368_ERR_PARAM_ERR_REFCLK) // generic parameter error TODO more specific??
    ERROR_CODE(ADF4368_ERR_RF_BAND0_UNACHIEVABLE)
    ERROR_CODE(ADF4368_ERR_RF_BAND1_UNACHIEVABLE)
    ERROR_CODE(ADF4368_ERR_DCLK_INVALID)
    ERROR_CODE(ADF4368_ERR_WAIT_FCN_NULL)
    ERROR_CODE(ADF4368_ERR_WAIT_FCN_FAIL)
    ERROR_CODE(ADF4368_ERR_FREQ_SWITCH_TIMEOUT)
    ERROR_CODE(ADF4368_ERR_INCORRECT_CAL_MODE)
    ERROR_CODE(ADF4368_ERR_FREQ_OUT_OF_BAND)
};

void update_err(adf4368_err err, const char* filename, const int line_number){
#ifdef BREAK_ON_ERR
    raise(SIGTRAP);
#endif 
    if (info.err==ADF4368_ERR_OK){
        info.err = err;
        strncpy(info.file_name, filename, sizeof(info.file_name));
        info.lineno = line_number;
    }
};

str adf4368_str_const_params(ad4368_constants c) {
  static char buff[300];
  str ret;
  ret.val = &buff[0];
  ret.len = sprintf(
      buff, "D=%u,O=%u, R=%u, refclk=%lu, band[0]=%llu, band[1]=%llu", c.D, c.O,
      c.R, (unsigned long)c.ref_clk, (unsigned long long)c.band_hz[0],
      (unsigned long long)c.band_hz[1]);
  return ret;
};

str adf4368_str_fracn_params(fracn f) {
  static char buff[300];
  str ret;
  ret.val = &buff[0];
  double N = _calc_N(f);
  ret.len = sprintf(
      buff, "N=%e, N_INT=%u, Frac1=%lu, Mod1=%lu, Frac2=%lu, Mod2=%lu",
      (float)N, (unsigned)f.n_int, (unsigned long)f.n_frac1,
      (unsigned long)f.mod1, (unsigned long)f.n_frac2, (unsigned long)f.mod2);
  return ret;
};

str adf4368_str_dclkdiv_params(adf4368_dclk_div dclkdiv) {
  static char buff[300];
  str ret;
  ret.val = &buff[0];
  ret.len = sprintf(buff, "dclk_div=%u, dclk_mode=%u", dclkdiv.dclk_div,
                    dclkdiv.dclk_mode);
  return ret;
};

str adf4368_str_cal_params(adf4368_vco_cal_params cal) {
  str ret;
  static char buff[300];
  ret.val = &buff[0];
  ret.len = sprintf(buff, "vco_band=%lu, vco_bias%lu, vco_core%lu",
                    (unsigned long)cal.vco_band, (unsigned long)cal.vco_bias,
                    (unsigned long)cal.vco_core);
  return ret;
};

str adf4368_str_freq_parms(adf4360_freq_params p) {
  static char buff[300];
  str ret;
  ret.val = &buff[0];
  ret.len = sprintf(
      buff, "fracn={%s}\n const={%s}\n dclkdiv={%s}",
      adf4368_str_fracn_params(p.f).val, adf4368_str_const_params(p.c).val,
      adf4368_str_dclkdiv_params(p.dclk_div).val);
  return ret;
};

str adf4368_str_internal_feqs(adf4360_freq_params p) {
  static char buff[100];
  str ret;
  double freq_pfd = _calc_freq_phase_detect_hz(p.c);
  double freq_vco = _calc_freq_vco_hz(p.c, p.f);
  ret.len = sprintf(buff, "F_pfd=%e,F_vco=%e", freq_pfd, freq_vco);
  if ((ret.len > 100) || (ret.len < 0)) {
    ret.val = NULL;
  } else {
    ret.val = &buff[0];
  }
  return ret;
};

//TODO: move to debug.c
str adf4368_str_dump_regs(adf4368_state *s) {
#define _ROW_WIDTH 16
#define _N_ROWS (25 + 2)
#define _BUFF_SIZE (_ROW_WIDTH * _N_ROWS + 1) // for null termination
  static char buff[_BUFF_SIZE];
  char row_buff[_ROW_WIDTH + 1];
  str ret;
  char *ptr = (char *)&buff;
  //"OFF 00 01 02 03\n"
  //"-----------------"
  //"000 DE AD BE EF\n"
  //"003 DE AD BE EF\n"
  /*Populate table header.**************************************************/
  sprintf(buff, "OFF 00 01 02 03\n---------------\n");
  ptr += _ROW_WIDTH * 2;
  /*Populate the table.*****************************************************/
  for (unsigned int i = 0; i < (100 / 4); i++) {
    sprintf(row_buff, "%03u %02X %02X %02X %02X\n", i * 4, s->registers[i * 4],
            s->registers[i * 4 + 1], s->registers[i * 4 + 2],
            s->registers[i * 4 + 3]);
    memcpy(ptr, &row_buff, _ROW_WIDTH);
    ptr += _ROW_WIDTH;
  }
  ret.len = _BUFF_SIZE;
  ret.val = &buff[0];
  return ret;
}

str adf4368_str_param_info(adf4368_param_hdl hdl) {
  str ret;
  static char buff[70];
  ret.val = &buff[0];
  ret.len = sprintf(buff, "%-21s[%3d] reg=0x%2X, offset=%u, width=%2u",
                    _param_lut[hdl].desc, hdl, _param_lut[hdl].reg,
                    _param_lut[hdl].offset, _param_lut[hdl].width);
  return ret;
};

str adf4368_str_err(void) {
  str ret;
  static char buff[150];
  ret.val = &buff[0];
  ret.len = sprintf(buff, "ERR:[%s] %s:%d", adf4368_error_msg[info.err],
                    info.file_name, info.lineno);
  return ret;
};
