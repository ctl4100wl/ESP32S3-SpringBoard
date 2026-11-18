#include "hal.h"
#include <SDL.h>
#include <string.h>

static char s_drop[512];
static volatile int s_drop_pending;

static int drop_watch(void * ud, SDL_Event * e)
{
  (void)ud;
  if(e->type == SDL_DROPFILE && e->drop.file) {
    strncpy(s_drop, e->drop.file, sizeof(s_drop) - 1);
    s_drop[sizeof(s_drop) - 1] = '\0';
    s_drop_pending = 1;
    SDL_free(e->drop.file);
    e->drop.file = NULL;
  }
  return 1;
}

bool os_drop_poll(char * path_out, size_t n)
{
  if(!s_drop_pending) return false;
  strncpy(path_out, s_drop, n - 1);
  path_out[n - 1] = '\0';
  s_drop_pending = 0;
  return true;
}

lv_display_t * sdl_hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  SDL_AddEventWatch(drop_watch, NULL);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon);
  lv_obj_t * cursor_obj;

  cursor_obj = lv_image_create(lv_screen_active());

  lv_image_set_src(cursor_obj, &mouse_cursor_icon);

  lv_indev_set_cursor(mouse, cursor_obj);

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}
