#ifndef ADF4368_PRIV_H_
#define ADF4368_PRIV_H_
#include "adf4368.h"

/*****************************************************************************/
/*                              Private Members                              */
/*****************************************************************************/

adf4368_err _sync_param_cache(adf4368_state *state, adf4368_param_hdl p_hdl);

adf4368_err _check_freq_settings(adf4360_freq_params p);

adf4368_err _get_staged_param(const adf4368_regs *reg, adf4368_param_hdl id, uint32_t *val);

/* Write the register(s) coresponding to param p_hdl.*/
adf4368_err _write_staged_param_reg(adf4368_state *state, adf4368_param_hdl p_hdl);

/* Calculate Divisor (N) from fractional values.*/
double _calc_N(fracn f);

/* Calculate fractional for a given target frequency values based on a known D, R and Reference clock. */
adf4368_err _calc_frac_values(uint64_t freq_hz, const ad4368_constants p, fracn* f);

/* Calculate the devisor (N) from the fractional values.**********************/
double _calc_N(fracn f);

/*Caclulate the frequency emitted by the synthesiser. ************************/
double _calc_freq_hz(ad4368_constants c, fracn f);

/*Calculate the frequency seen by the phase detector. ************************/
double _calc_freq_phase_detect_hz(ad4368_constants c);

//* Calculate the frequency seen by the NCO. *********************************/
double _calc_freq_vco_hz(ad4368_constants c, fracn f);

adf4368_err _stage_freq_settings(adf4368_state *state, const adf4360_freq_params p);

ad4368_constants _get_const_vals(const adf4368_state *state);

fracn _get_fracn(const adf4368_state *state);

adf4368_dclk_div _get_dclk_div(const adf4368_state *state);

adf4368_vco_cal_params _get_cal_written(const adf4368_state *state);

adf4360_freq_params _get_freq_params(const adf4368_state *state);

adf4368_err _stage_fracn(adf4368_state *state, fracn f);

adf4368_err _stage_manual_cal_values(adf4368_state *state, adf4368_vco_cal_params cal);

adf4368_err _stage_const_vals(adf4368_state *state, ad4368_constants consts);

adf4368_dclk_div _calc_dclk_div(ad4368_constants c); // forward declaration

uint32_t _calc_fdiv_rclk(ad4368_constants c, adf4368_dclk_div dclk_div);
uint32_t _calc_synth_lock_timeout(ad4368_constants c, adf4368_dclk_div dclk_div);
uint32_t _calc_vco_alc_timeout(ad4368_constants c, adf4368_dclk_div dclk_div);
uint32_t _calc_vco_band_div(ad4368_constants c, adf4368_dclk_div dclk_div);
uint32_t _calc_adc_clk_div(ad4368_constants c, adf4368_dclk_div dclk_div);


/*Function signature for frequency parameter checking functions.**************/
typedef adf4368_err _freq_param_check(const adf4360_freq_params* p);

_freq_param_check _check_N;
_freq_param_check _check_O;
_freq_param_check _check_R;
_freq_param_check _check_D;
_freq_param_check _check_vco;
_freq_param_check _check_pfd;
_freq_param_check _check_reff_clk;


adf4368_err _sync_param_cache(adf4368_state* state, adf4368_param_hdl p_hdl);

#endif
