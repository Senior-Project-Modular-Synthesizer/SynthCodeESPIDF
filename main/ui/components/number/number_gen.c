/**
 * @file number_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "number_gen.h"
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

lv_obj_t * number_create(lv_obj_t * parent, const char * text, lv_subject_t * subject)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t box;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&box);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_align(lv_obj_0, LV_ALIGN_CENTER);
    lv_obj_set_height(lv_obj_0, 60);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_obj_set_style_pad_bottom(lv_label_0, 20, 0);
    lv_label_set_text(lv_label_0, text);
    lv_obj_set_style_text_font(lv_label_0, font_title, 0);

    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_obj_set_style_pad_top(lv_label_1, 25, 0);
    lv_label_bind_text(lv_label_1, subject, NULL);
    lv_obj_set_style_text_font(lv_label_1, font_subtitle, 0);


    LV_TRACE_OBJ_CREATE("finished");


    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/