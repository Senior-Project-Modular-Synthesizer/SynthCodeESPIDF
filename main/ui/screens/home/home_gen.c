/**
 * @file home_gen.c
 * @description Template source file for LVGL objects
 */

#include "home_gen.h"
#include "../../ui.h"


/***********************
 *  STATIC VARIABLES
 **********************/
static const char * btnm_map[] = {"High Pass", "Low Pass", "Band", "\n",
                                  "LFO", "Env", "VCA", "\n",
                                  "Sine", "Triangle", "Square", ""
                                 };

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        const char * txt = lv_buttonmatrix_get_button_text(obj, id);
        ESP_LOGI("GUI", "Pushed id: %d, txt: %s\n", id, txt);
        LV_UNUSED(txt);
        // lv_obj_t * effect_scr = effect_create();
        // lv_screen_load(effect_scr);
    }
}

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");
    ESP_LOGI("GUI", "Creating home");

    static lv_style_t matrix;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_set_width(&matrix, 480);
        lv_style_set_height(&matrix, 320);
        lv_style_set_text_font(&matrix, font_title);
        
        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);

    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(lv_obj_0);
    lv_obj_add_style(lv_buttonmatrix_0, &matrix, 0);

    lv_buttonmatrix_set_map(lv_buttonmatrix_0, btnm_map);
    
    lv_obj_align(lv_buttonmatrix_0, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(lv_buttonmatrix_0, event_handler, LV_EVENT_ALL, NULL);

    LV_TRACE_OBJ_CREATE("finished");
    ESP_LOGI("GUI", "Created home");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/