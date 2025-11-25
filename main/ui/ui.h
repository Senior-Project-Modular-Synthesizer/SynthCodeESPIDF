/**
 * @file Project_gen.h
 */

#ifndef PROJECT_GEN_H
#define PROJECT_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#include "../../managed_components/lvgl__lvgl/src/misc/lv_types.h"
#include "esp_log.h"
/*********************
 *      DEFINES
 *********************/



/**********************
 *      TYPEDEFS
 **********************/



/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/
extern lv_obj_t * home;

/*----------------
 * Global styles
 *----------------*/


/*----------------
 * Fonts
 *----------------*/
extern lv_font_t * font_title;
extern lv_font_t * font_subtitle;

/*----------------
 * Images
 *----------------*/
extern const void * img_wifi;
extern const void * img_bluetooth;
extern const void * img_bell;

/*----------------
 * Subjects
 *----------------*/
extern lv_subject_t hours;
extern lv_subject_t mins;
extern lv_subject_t age;
extern lv_subject_t bluetooth_on;
extern lv_subject_t wifi_on;
extern lv_subject_t notification_on;
extern lv_subject_t hour_edited;
extern lv_subject_t min_edited;
extern lv_subject_t arc1;
extern lv_subject_t arc2;
extern lv_subject_t slide;
extern lv_subject_t check;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/
void arc_changed(lv_event_t * e);
void slider_changed(lv_event_t * e);

/**
 * Initialize the component library
 */

void gui_init();

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widget and components of this library*/
#include "components/arc/arc_gen.h"
#include "components/checkbox/checkbox_gen.h"
#include "components/column/column_gen.h"
#include "components/effectslider/effectslider_gen.h"
#include "components/grid/grid_gen.h"
#include "components/header/header_gen.h"
#include "components/icon/icon_gen.h"
#include "components/row/row_gen.h"
#include "components/subtitle/subtitle_gen.h"
#include "components/title/title_gen.h"
#include "screens/effect/effect_gen.h"
#include "screens/home/home_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*PROJECT_GEN_H*/