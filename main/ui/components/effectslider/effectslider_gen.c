/**
 * @file effectslider_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "effectslider_gen.h"
#include "../../ui.h"

/*********************
 *      DEFINES
 *********************/



/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * effectslider_create(lv_obj_t * parent, const char * title, lv_subject_t * variable, short min, short max, short start)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t slider;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&slider);
        lv_style_set_bg_opa(&slider, 0);
        lv_style_set_border_width(&slider, 0);
        lv_style_set_width(&slider, LV_SIZE_CONTENT);
        lv_style_set_height(&slider, 60);
        lv_style_set_flex_flow(&slider, LV_FLEX_FLOW_COLUMN);
        lv_style_set_text_font(&slider, font_subtitle);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(lv_obj_0, &slider, 0);

    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, title);
    lv_obj_set_style_pad_bottom(lv_label_0, 45, 0);


    lv_obj_t * lv_slider_0 = lv_slider_create(lv_obj_0);
    lv_slider_bind_value(lv_slider_0, variable);
    lv_obj_set_width(lv_slider_0, 150);
    lv_obj_set_align(lv_slider_0, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(lv_slider_0, slider_changed, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_range(lv_slider_0, min, max);
    lv_slider_set_value(lv_slider_0, start, LV_ANIM_OFF);

    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_1, variable, NULL);lv_obj_set_style_pad_top(lv_label_1, 45, 0);



    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/