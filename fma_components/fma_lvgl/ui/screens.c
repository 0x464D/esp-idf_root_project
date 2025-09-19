#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_link_view() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.link_view = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 135);
    {
        lv_obj_t *parent_obj = obj;
        {
            // link_zelda
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.link_zelda = obj;
            lv_obj_set_pos(obj, 56, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_link_zelda);
        }
    }
    
    tick_screen_link_view();
}

void tick_screen_link_view() {
}

void create_screen_halo_view() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.halo_view = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 135);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 59, 0);
            lv_obj_set_size(obj, 119, 135);
            lv_image_set_src(obj, &img_halo);
        }
    }
    
    tick_screen_halo_view();
}

void tick_screen_halo_view() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_link_view,
    tick_screen_halo_view,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_link_view();
    create_screen_halo_view();
}
