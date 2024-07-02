#ifndef ADF4368_DEBUG_H_
#define ADF4368_DEBUG_H_
#include <memory.h>
#include "adf4368.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define ADF4368_ERROR_MSG_WIDTH 34

/*****************************************************************************/
/*                                   Utils                                   */
/*****************************************************************************/

typedef struct {
    size_t len;
    char * val;
} str;

/*****************************************************************************/
/*                                   Debug                                   */
/*****************************************************************************/

/*Create a debug string for frequency parameters.*****************************/
str adf4368_str_freq_parms(adf4360_freq_params p);

/*Create a debug string for the interal frequencies.**************************/
str adf4368_str_internal_feqs(adf4360_freq_params p);

/*Create debug string of registers*/

str adf4368_str_dump_regs(adf4368_state* s);

/* Create a debug string for a device parameter. */

str adf4368_str_param_info(adf4368_param_hdl hdl);

/* Descriptive error message of fist error post clear.*/
str adf4368_str_err(void);


str adf4368_str_cal_params(adf4368_vco_cal_params cal);

str adf4368_str_dclkdiv_params(adf4368_dclk_div dclkdiv);

/*****************************************************************************/
/*                                   Error handling                          */
/*****************************************************************************/

void update_err(adf4368_err err, const char* filename, const int line_number);

#define _ASSERT_MSG(cond, err)\
    if (!(cond)){ \
        update_err((err), __FILENAME__, __LINE__); \
        return err;} \

#define _ASSERT(cond)\
    if (!(cond)){ \
        update_err(ADF4368_ERR_UNKONW , __FILENAME__, __LINE__); \
        return (err);} \

#define _ASSERT_ERR(err)\
    if ((err)!=ADF4368_ERR_OK){ \
        update_err(err, __FILENAME__, __LINE__); \
        return (err);} \

typedef struct {
    adf4368_err err;
    int lineno;
    char file_name[100];
}err_info;

typedef char error_msg[100][ADF4368_ERROR_MSG_WIDTH];

extern const error_msg adf4368_error_msg;

extern err_info info;


#endif // ADF4368_ERR_H_
