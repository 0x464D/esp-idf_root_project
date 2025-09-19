#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *link_view;
    lv_obj_t *halo_view;
    lv_obj_t *link_zelda;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_LINK_VIEW = 1,
    SCREEN_ID_HALO_VIEW = 2,
};

void create_screen_link_view();
void tick_screen_link_view();

void create_screen_halo_view();
void tick_screen_halo_view();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/