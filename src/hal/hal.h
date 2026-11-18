#ifndef LV_VSCODE_HAL_H
#define LV_VSCODE_HAL_H

#include "lvgl/lvgl.h"
#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * sdl_hal_init(int32_t w, int32_t h);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif
