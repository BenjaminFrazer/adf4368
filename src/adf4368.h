#ifndef ADF4368_H_
#define ADF4368_H_
#include <inttypes.h>

#define ADF4368_N_REGISTERS 100
#define ADF4368_N_PARAMS 100
#define ADF4368_ERROR_MSG_WIDTH 30

/*****************************************************************************/
/*                               Public Members                              */
/*****************************************************************************/


/* Read/write mode of ADF4368 SPI transaction. */
typedef enum
{
        ADF4368_R,
        ADF4368_W,
} adf4368_rw_mode;

typedef int *(adf4368_write_fcn)(adf4368_rw_mode mode, uint8_t register, uint8_t payload);

/* Datatype of param. */
typedef enum
{
    ADF4368_UNDEFINED_DTYPE = 0,
    ADF4368_INT, // signed integer
    ADF4368_UINT, // unsigned integer
} adf4368_dtyp;

typedef enum
{
    NDEF = 0,
    ADC_CLK,
} adf4368_param_hdl;
/* All known ADF4368 errors */
typedef enum
{
    ADF4368_ERR_OK = 0,
    ADF4368_ERR_UNKOWN = 1,
    ADF4368_ERR_NOT_IMPLEMENTED
} adf4368_err;

/* ADF4368 Configuration Parameter. */
typedef struct
{
    adf4368_dtyp type; // type of data held
    uint8_t reg; // register number of param start
    uint8_t width; // bitwidth of the register
    uint8_t offset;  // bit ofset within register

} adf4368_param;

typedef struct
{
    uint8_t registers[ADF4368_N_REGISTERS];
    uint64_t ref_clk_hz;
    adf4368_write_fcn *write_fcn;
}adf4368_state;

typedef enum
{
    ADF4368_FREQ_HOP_MODE_UNKOWN = 0,
    ADF4368_FREQ_HOP_MODE_AUTOCAL,
    ADF4368_FREQ_HOP_MODE_FASTHOP,
} adf4368_freq_hop_mode;

typedef char error_msg[100][ADF4368_ERROR_MSG_WIDTH];

/*Check selected frequency values are valid. *********************************/
adf4368_err adf4368_validate_freq_settings(adf4368_err *state);

/* Initialise  the ADF4368. */
adf4368_err adf4368_init(adf4368_state *state);

/* Set an unsigned integer parameter. */
adf4368_err set_uint_param(adf4368_state *state, adf4368_param_hdl id, unsigned int val);

/* Get an unsigned integer parameter. */
adf4368_err get_uint_param(const adf4368_state *state, adf4368_param_hdl id, unsigned int val);

/* Set a signed integer parameter. */
adf4368_err set_int_param(adf4368_state *state, adf4368_param_hdl id, int *val);

/* Set a signed integer parameter. */
adf4368_err get_int_param(const adf4368_state *state, adf4368_param_hdl id, int *val);

/* Determine the STATE of the ADF4368. */
adf4368_err adf4368_get_freq_hop_mode(const adf4368_state *state, adf4368_freq_hop_mode *mode);

/*****************************************************************************/
/*                              Private Members                              */
/*****************************************************************************/

/* Fractional values. */
typedef struct
{
    uint16_t n_int;
    uint32_t mod2;
    uint32_t n_frac1;
    uint32_t n_frac2;
}_fracn;

typedef struct
{
    uint8_t D;
    uint8_t R;
    uint8_t O;
    uint32_t ref_clk;
    _fracn frac;
}_freq_params;

/* Calculate Divisor (N) from fractional values.*/
double _calc_N(_fracn f);

/* Calculate fractional for a given target frequency values based on a known D, R and Reference clock. */
adf4368_err _calc_frac_values(uint64_t freq_hz, _freq_params *p);

/* Calculate the devisor (N) from the fractional values.**********************/
double _calc_N(_fracn f);

/*Caclulate the frequency emitted by the synthesiser. ************************/
double _calc_freq_hz(_freq_params p);

/*Calculate the frequency seen by the phase detector. ************************/
double _calc_freq_phase_detect_hz(_freq_params p);

//* Calculate the frequency seen by the NCO. *********************************/
double _calc_freq_vco_hz(_freq_params p);

#endif // ADF4368_H_
