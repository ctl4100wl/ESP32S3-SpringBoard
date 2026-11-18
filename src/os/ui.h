#ifndef UI_H
#define UI_H

#include "lvgl/lvgl.h"

#define UI_BG       0x000000
#define UI_GROUP    0x1C1C1E
#define UI_SEP      0x2C2C2E
#define UI_TEXT     0xF2F2F7
#define UI_DIM      0x8E8E93
#define UI_BLUE     0x0A84FF
#define UI_RED      0xFF453A
#define UI_GREEN    0x30D158
#define UI_ORANGE   0xFF9F0A

void ui_icon_style(lv_obj_t * obj, uint32_t color);
lv_obj_t * ui_box(lv_obj_t * parent);
lv_obj_t * ui_label(lv_obj_t * parent, const char * text, const lv_font_t * font, uint32_t color);
const lv_font_t * ui_font_vietnamese(const lv_font_t * base);
lv_obj_t * ui_app_scaffold(lv_obj_t * root, const char * title);
lv_obj_t * ui_card(lv_obj_t * parent);
lv_obj_t * ui_row(lv_obj_t * card, const char * sym, uint32_t accent, const char * text);
lv_obj_t * ui_button(lv_obj_t * parent, const char * text, uint32_t bg, lv_event_cb_t cb, void * user_data);
lv_obj_t * ui_keyboard(lv_obj_t * root, lv_obj_t * textarea);
void ui_keyboard_show(lv_obj_t * kb);
void ui_keyboard_hide(lv_obj_t * kb);
bool ui_keyboard_dismiss_visible(lv_obj_t * root);

void ui_glass(lv_obj_t * obj, uint32_t tint, lv_opa_t opa, int32_t blur);

void ui_glass_text_mode(int mode);
lv_draw_buf_t * ui_glass_text(lv_obj_t * bg, const char * text, const lv_font_t * font, int32_t pad, int32_t ox,
                              int32_t oy, uint32_t tint, int32_t bezel, int32_t strength, bool chroma);
void ui_lens_settings_get(int32_t * bezel, int32_t * strength, bool * chroma);
void ui_lens_settings_set(int32_t bezel, int32_t strength, bool chroma);

void ui_lens(lv_obj_t * obj, int32_t bezel, int32_t strength, bool chroma);

void ui_glass_set(lv_obj_t * obj, float p, int32_t max_blur, lv_opa_t opa);

lv_draw_buf_t * ui_text_halo(const char * text, const lv_font_t * font, int32_t radius, int32_t pad);

lv_obj_t * ui_nav_push(lv_obj_t * root, const char * title, const char * back);
lv_obj_t * ui_nav_top(lv_obj_t * root);
bool ui_nav_pop(lv_obj_t * root);
lv_obj_t * ui_nav_row(lv_obj_t * card, const char * sym, uint32_t accent, const char * text, const char * value,
                      lv_event_cb_t cb, void * user_data);
lv_obj_t * ui_switch_row(lv_obj_t * card, const char * sym, uint32_t accent, const char * text, bool on,
                         lv_event_cb_t cb, void * user_data);
lv_obj_t * ui_section(lv_obj_t * body, const char * caption);

#endif
