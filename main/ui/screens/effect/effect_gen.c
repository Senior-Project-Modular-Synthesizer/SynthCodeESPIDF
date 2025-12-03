/**
 * @file effect_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "effect_gen.h"
#include "../../ui.h"


/***********************
 *  STATIC VARIABLES
 **********************/
static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * effect_create(UIElement* elements)
{
    LV_TRACE_OBJ_CREATE("begin");
    ESP_LOGI("GUI", "Creating Effect");

    static lv_style_t main;
    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&main);
        lv_style_set_text_font(&main, font_subtitle);
        
        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_style_pad_hor(lv_obj_0, 5, 0);
    lv_obj_set_style_pad_top(lv_obj_0, 5, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_t * title_0 = title_create(lv_obj_0, "Gain");
    lv_obj_set_align(title_0, LV_ALIGN_TOP_LEFT);

    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_0);
    lv_obj_set_height(lv_button_0, 30);
    lv_obj_set_style_text_font(lv_button_0, font_subtitle, 0);
    lv_obj_set_align(lv_button_0, LV_ALIGN_TOP_RIGHT);

    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_0, "Back");
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);

    lv_obj_add_screen_load_event(lv_button_0, LV_EVENT_CLICKED, home, LV_SCR_LOAD_ANIM_NONE, 0, 0);
    lv_obj_add_event_cb(lv_button_0, destroy_effect, LV_EVENT_CLICKED, NULL);

    lv_obj_t * grid = lv_obj_create(lv_obj_0);
    lv_obj_set_size(grid, 480, 280);
    lv_obj_set_style_align(grid, LV_ALIGN_BOTTOM_MID, 0);
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
    lv_obj_set_flag(grid, LV_OBJ_FLAG_SCROLLABLE, false);

    for (int i = 0; i < 6; i++) {
        int row = i / 3;
        int col = i % 3;
        UIElement element = elements[i];
        if (element.data == NULL)
            continue;

        lv_obj_t * comp = NULL;
        switch (element.type) {
            case CHECKBOX:
                lv_subject_init_int(&subjects[i], *(int *)element.data);
                comp = checkbox_create(grid, &subjects[i], element.name, 1);
                observers[i] = lv_subject_add_observer(&subjects[i], observer_cb, element.data);
                break;
            case SLIDER:
                lv_subject_init_float(&subjects[i], *(float *)element.data);
                comp = effectslider_create(grid, element.name, &subjects[i], element.min, element.max, *(float *)element.data);
                observers[i] = lv_subject_add_observer(&subjects[i], observer_cb, element.data);
                break;
            case ARC1:
            case ARC2:
                lv_subject_init_float(&subjects[i], *(float *)element.data);
                comp = arc_create(grid, element.name, &subjects[i], element.min, element.max, *(float *)element.data);
                observers[i] = lv_subject_add_observer(&subjects[i], observer_cb, element.data);
                break;
            case NUMBER:
                lv_subject_init_float(&subjects[i], *(float *)element.data);
                comp = number_create(grid, element.name, &subjects[i]);
                break;
            case LIGHT:
                lv_subject_init_int(&subjects[i], *(int *)element.data);
                comp = checkbox_create(grid, &subjects[i], element.name, 0);
                break;
            default: // (empty)
                break;
        }
        if (comp != NULL) {
            lv_obj_set_grid_cell(comp, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
            pointers[i] = element.data;
        }
    }

    LV_TRACE_OBJ_CREATE("finished");

    ESP_LOGI("GUI", "Effect created");
    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

    // lv_obj_t * checkbox_0 = checkbox_create(grid, "Check", &check);
    // lv_obj_set_grid_cell(checkbox_0, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    // lv_obj_t * effect_slider_0 = effectslider_create(grid, "Test", &arc1);
    // lv_obj_set_grid_cell(effect_slider_0, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    // lv_obj_t * arc_0 = arc_create(grid, "GAIN", &arc1);
    // lv_obj_set_align(arc_0, LV_ALIGN_BOTTOM_LEFT);
    // lv_obj_set_grid_cell(arc_0, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    // lv_obj_t * arc_1 = arc_create(grid, "GAIN", &arc2);
    // lv_obj_set_align(arc_1, LV_ALIGN_BOTTOM_RIGHT);
    // lv_obj_set_grid_cell(arc_1, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);