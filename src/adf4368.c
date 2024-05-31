#include "adf4368.h"

#define MOD1_BIT_WIDTH 25
#define MOD2_BIT_WIDTH 23

#define ERROR_CODE(CODE) [(CODE)]=#CODE,


const uint64_t mod1_word = 1<<MOD1_BIT_WIDTH; // Modulo one factor.
const uint64_t mod2_word = 1<<MOD2_BIT_WIDTH; // Modulo two factor.

const error_msg adf4368_error_msg = {
    ERROR_CODE(ADF4368_ERR_OK)
    ERROR_CODE(ADF4368_ERR_UNKOWN)
};


adf4368_err _calc_frac_values(uint64_t freq_hz, _freq_params *p){
    p->frac.mod2 = mod2_word;
    uint64_t num = (freq_hz * p->R);
    uint32_t denom = p->D*p->ref_clk;
    p->frac.n_int = num / denom;
    uint64_t residual1 = num % denom;
    p->frac.n_frac1 = (residual1 * mod1_word) / denom;
    uint64_t residual2 = (residual1 * mod1_word) % denom;
    p->frac.n_frac2 = (residual2 * mod2_word) / denom;
    return ADF4368_ERR_OK;
};

/* Calculate the devisor (N) from the fractional values.**********************/
double _calc_N(_fracn f){
    return (double)(f.n_int                                         \
                    + (double)f.n_frac1/(double)mod1_word           \
                    + (double)f.n_frac2/(double)(mod1_word*mod2_word));
};

/*Caclulate the frequency emitted by the synthesiser. ************************/
double _calc_freq_hz(_freq_params p){
    double num = (p.ref_clk * p.D * _calc_N(p.frac))/p.R;
    return num;
};

/*Calculate the frequency seen by the phase detector. ************************/
double _calc_freq_phase_detect_hz(_freq_params p){
    return (double)(p.D * p.ref_clk) / p.R;
};

//* Calculate the frequency seen by the NCO. *********************************/
double _calc_freq_vco_hz(_freq_params p){
    return (double)(p.ref_clk * _calc_N(p.frac) * p.O);
};

adf4368_err _check_N(_freq_params p){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};

adf4368_err _check_O(_freq_params p){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};

adf4368_err _check_R(_freq_params p){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};

adf4368_err _check_D(_freq_params p){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};

adf4368_err _check_vco(_freq_params p){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};

adf4368_err _check_pfd(uint8_t O, uint32_t ref_clk, double N){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};


adf4368_err adf4368_validate_freq_settings(adf4368_err *state){
    return ADF4368_ERR_NOT_IMPLEMENTED; // TODO
};
