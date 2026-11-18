#ifndef OS_H
#define OS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "lvgl/lvgl.h"
#include "fx.h"

/*********************
 *      DEFINES
 *********************/

#define OS_MAX_APPS         24
#define OS_GESTURE_H        22
#define OS_WALLPAPER_COUNT  6
#define OS_WALL_CUSTOM      0xFF
#define OS_LOCK_STYLES      6
#define OS_LOCK_COLOR_AUTO  8
#define OS_NAME             "S3-Springboard"

#define HOME_PAGES          2
#define HOME_COLS           4
#define HOME_ROWS           4
#define HOME_DOCK           4
#define HOME_SLOTS          (HOME_COLS * HOME_ROWS)
#define HOME_LOC_DOCK0      (HOME_PAGES * HOME_SLOTS)
#define HOME_LOC_COUNT      (HOME_LOC_DOCK0 + HOME_DOCK)
#define HOME_WIDGETS        6
#define HOME_FOLDER_ITEMS   8

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    const char * symbol;
    uint32_t color;
    void (*create)(lv_obj_t * root);
} app_desc_t;

typedef struct {
    bool wifi;
    bool bluetooth;
    bool airplane;
    uint8_t brightness;
    uint8_t wallpaper;
    uint8_t volume;
    bool dnd;
    bool flashlight;
    bool cellular;
    bool pill;
    bool quick_switch;
    bool previews;
    uint8_t lock_style;
    uint8_t lock_color;
    uint8_t lock_wall;
    uint8_t about_style;
    bool edge_back;
    uint8_t edge_size;
    int8_t lock_widget[4];
    uint8_t lock_widget_span[4];
    bool low_power;
} os_settings_t;

typedef struct {
    int8_t widget;
    int8_t page;
    int8_t cell;
    int8_t size;
} home_widget_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const app_desc_t * const os_apps[];
extern const int os_app_count;
extern const int os_dock_start;

extern os_settings_t os_settings;
void os_load(void);
void os_save(void);

void shell_init(void);
void shell_settings_apply(void);
void shell_wallpaper_colors(int idx, uint32_t * top, uint32_t * bottom);
void shell_statusbar_set_visible(bool visible);
void shell_statusbar_raise(void);
bool shell_home_idle(void);
void shell_launch_from_home(int id, const lv_area_t * icon_area);
void shell_launch_from_widget(int id, lv_obj_t * widget_box);
void shell_open_app(int id);
void shell_open_app_from(int id, const lv_area_t * from);
void shell_lock_changed(bool visible);
void shell_clock_shade_progress(float progress);
void shell_go_home_instant(void);
void shell_recents_open(void);
lv_draw_buf_t * shell_wallpaper_image(bool lock);
void shell_wallpaper_colors_lock(int idx, uint32_t * top, uint32_t * bottom);

lv_obj_t * statusbar_icons_create(lv_obj_t * parent);
void statusbar_icons_update(void);
void statusbar_icons_set_color(lv_color_t c);

void lock_init(void);
void lock_now(void);
bool lock_visible(void);
void lock_refresh(void);
void notify_post(int app_id, const char * title, const char * body);
int notify_count(void);
void notify_clear(void);
void lock_debug_unlock(void);
void lock_debug_shade(float p);

void import_init(void);
void import_offer(const char * path);
bool os_drop_poll(char * path_out, size_t n);

void cc_init(void);
void cc_set_enabled(bool en);
bool cc_visible(void);
void cc_close(void);
void cc_debug_pose(float p);
void cc_debug_pop(void);

void home_create(lv_obj_t * parent);
bool home_get_icon_area(int id, lv_area_t * out);
lv_obj_t * home_get_icon_object(int id);
void home_set_icon_hidden(int id, bool hidden);
void home_edit_enter(void);
void home_debug_resize(int k, int size);
void home_debug_wres(int k, int axis, int dx, int dy);
void home_debug_wdrag(int k, int x, int y);
void home_debug_wdrop(void);
void home_debug_cmd(const char * cmd);
bool notify_banner_active(void);
void shell_debug_home(void);
int home_widget_count(int widget);
bool home_widget_add(int widget);
bool home_widget_remove(int widget);
int home_widget_free_slots(void);
void home_card_sync(void);
void home_edit_exit(void);
bool home_in_edit(void);
void home_reset_layout(void);
void home_layout_export(int8_t out[HOME_LOC_COUNT]);
void home_layout_import(const int8_t in[HOME_LOC_COUNT]);
void home_widgets_export(home_widget_t out[HOME_WIDGETS]);
void home_widgets_import(const home_widget_t in[HOME_WIDGETS]);
void home_folders_export(int8_t members[OS_MAX_APPS][HOME_FOLDER_ITEMS], uint8_t counts[OS_MAX_APPS]);
void home_folders_import(const int8_t members[OS_MAX_APPS][HOME_FOLDER_ITEMS], const uint8_t counts[OS_MAX_APPS]);
bool home_app_installed(int id);
bool home_app_install(int id);
void home_unlock_anim(void);
void home_lock_veil(bool on);
void home_set_hidden(bool hidden);

#endif
