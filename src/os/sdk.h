#ifndef SDK_H
#define SDK_H

#include "os.h"
#include "ui.h"

#define APP(var, name, symbol, color)                                       \
    static void var##_create(lv_obj_t * root);                              \
    const app_desc_t var = { name, symbol, color, var##_create };           \
    static void var##_create(lv_obj_t * root)

typedef enum { WIDGET_SMALL, WIDGET_MEDIUM, WIDGET_LARGE } widget_size_t;

typedef struct {
    const char * name;
    const app_desc_t * app;
    widget_size_t size;
    void (*create)(lv_obj_t * box);
    void (*lock_create)(lv_obj_t * slot);
    bool resizable;
} widget_desc_t;

#define WIDGET(var, name, app, size, lock_fn)                                     \
    static void var##_create(lv_obj_t * box);                                     \
    const widget_desc_t var = { name, app, size, var##_create, lock_fn };         \
    static void var##_create(lv_obj_t * box)

#define WIDGET_RESIZABLE(var, name, app, size, lock_fn)                           \
    static void var##_create(lv_obj_t * box);                                     \
    const widget_desc_t var = { name, app, size, var##_create, lock_fn, true };   \
    static void var##_create(lv_obj_t * box)

widget_size_t sdk_widget_size(void);
void sdk_widget_size_set(widget_size_t size);
int sdk_widget_inst(void);
void sdk_widget_inst_set(int k);

extern const widget_desc_t * const os_widgets[];
extern const int os_widget_count;

lv_timer_t * sdk_timer(lv_obj_t * owner, lv_timer_cb_t cb, uint32_t period_ms, void * user_data);

int sdk_get_int(const char * key, int def);

void sdk_ensure_dir(const char * path);
void sdk_set_int(const char * key, int value);
const char * sdk_get_str(const char * key, const char * def);
void sdk_set_str(const char * key, const char * value);

typedef void (*sdk_http_cb_t)(bool ok, const char * body, void * user_data);

void sdk_http_get(const char * url, sdk_http_cb_t cb, void * user_data, lv_obj_t * owner);

typedef void (*sdk_http_data_cb_t)(bool ok, const uint8_t * data, size_t len, void * user_data);
void sdk_http_get_data(const char * url, sdk_http_data_cb_t cb, void * user_data, lv_obj_t * owner);

void sdk_http_get_data_ex(const char * url, sdk_http_data_cb_t cb, void * user_data, lv_obj_t * owner, size_t max_bytes);

size_t sdk_http_fetch_sync(const char * url, char * buf, size_t cap);

typedef void (*sdk_job_fn_t)(void * user_data);
typedef void (*sdk_job_done_t)(void * user_data, bool ok);
void sdk_job_run(sdk_job_fn_t work, sdk_job_done_t done, void * user_data, lv_obj_t * owner, uint32_t timeout_ms);
bool sdk_online(void);

const char * sdk_json_find(const char * json, const char * key);
bool sdk_json_num(const char * json, const char * key, double * out);
int sdk_json_num_array(const char * json, const char * key, double * out, int max);

void sdk_notify(const app_desc_t * app, const char * title, const char * body);

typedef void (*sdk_refresh_cb_t)(lv_obj_t * body, void * user_data);
void sdk_pull_refresh(lv_obj_t * body, sdk_refresh_cb_t cb, void * user_data);
void sdk_pull_refresh_done(lv_obj_t * body);

bool sdk_pick_file(const char * title, const char * filter, char * path_out, size_t n);
bool sdk_pick_image(char * path_out, size_t n);

typedef void (*sdk_pick_cb_t)(const char * path, void * user_data);
void sdk_pick_file_async(const char * title, const char * filter, sdk_pick_cb_t cb, void * user_data);
lv_draw_buf_t * sdk_image_load(const char * path, int32_t w, int32_t h);

lv_draw_buf_t * sdk_image_decode_mem(const uint8_t * data, size_t len, int32_t max_w, int32_t max_h);

void sdk_toast(const char * title, const char * text);
void sdk_open_app(const app_desc_t * app);
int sdk_app_index(const app_desc_t * app);

#endif
