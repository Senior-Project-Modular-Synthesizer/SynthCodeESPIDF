/**
 * @file home_gen.c
 * @description Template source file for LVGL objects
 */

#include "home_gen.h"
#include "../../ui.h"


/***********************
 *  STATIC VARIABLES
 **********************/
static const char * btnm_map[] = {"1", "2", "3", "4", "5", "\n",
                                  "6", "7", "8", "9", "0", "\n",
                                  ""
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
        LV_UNUSED(txt);
        ESP_LOGI("GUI", "%s was pressed\n", txt);
    }
}

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t cell;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&cell);
        lv_style_set_border_side(&cell, LV_BORDER_SIDE_FULL);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);

    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(lv_obj_0);
    lv_buttonmatrix_set_map(btnm1, btnm_map);
    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/