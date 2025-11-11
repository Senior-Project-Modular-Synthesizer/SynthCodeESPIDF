/**
 * @file grid_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "grid_gen.h"
#include "ui.h"

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

lv_obj_t * grid_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_style_layout(lv_obj_0, LV_LAYOUT_GRID, 0);

    lv_obj_t * lv_checkbox_0 = lv_checkbox_create(lv_obj_0);
    lv_checkbox_set_text(lv_checkbox_0, "Check");
    lv_obj_bind_checked(lv_checkbox_0, &check);
    lv_obj_set_style_grid_cell_row_pos(lv_checkbox_0, 0, 0);
    lv_obj_set_style_grid_cell_column_pos(lv_checkbox_0, 0, 0);


    lv_obj_t * effectslider_0 = effectslider_create(lv_obj_0, "GAIN", &arc1);
    lv_obj_set_style_grid_cell_row_pos(effectslider_0, 1, 0);
    lv_obj_set_style_grid_cell_column_pos(effectslider_0, 1, 0);



    LV_TRACE_OBJ_CREATE("finished");

    lv_obj_set_name(lv_obj_0, "grid_#");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/