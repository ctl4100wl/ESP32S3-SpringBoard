#ifndef FX_H
#define FX_H

#include "lvgl/lvgl.h"

#ifndef OS_FX_BLUR
  #ifdef ESP_PLATFORM
    #define OS_FX_BLUR  0
  #else
    #define OS_FX_BLUR  1
  #endif
#endif
#ifndef OS_FX_HEAVY
  #define OS_FX_HEAVY OS_FX_BLUR
#endif

#ifndef OS_FX_LENS
  #define OS_FX_LENS  OS_FX_BLUR
#endif

typedef enum {
    FX_X,
    FX_Y,
    FX_SCALE,
    FX_OPA,
    FX_DW,
    FX_DH,
    FX_PROP_COUNT
} fx_prop_t;

typedef void (*fx_done_cb_t)(lv_obj_t * obj);

#define FX_SNAPPY   0.30f, 0.86f
#define FX_BOUNCY   0.45f, 0.68f
#define FX_SOFT     0.55f, 0.90f

void fx_set(lv_obj_t * obj, fx_prop_t prop, float value);
void fx_to(lv_obj_t * obj, fx_prop_t prop, float target, float response, float damping, uint32_t delay_ms,
           fx_done_cb_t done);
void fx_kick(lv_obj_t * obj, fx_prop_t prop, float velocity);
float fx_get(lv_obj_t * obj, fx_prop_t prop);
bool fx_busy(lv_obj_t * obj, fx_prop_t prop);
void fx_stop(lv_obj_t * obj);

#endif
