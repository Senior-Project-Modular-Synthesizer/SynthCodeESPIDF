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

/*----------------
 * Subjects
 *----------------*/
extern lv_subject_t subjects[6];
extern lv_observer_t * observers[6];
extern void* pointers[6];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/
void arc_changed(lv_event_t * e);
void slider_changed(lv_event_t * e);
void check_changed(lv_event_t * e);
void observer_cb(lv_observer_t * observer, lv_subject_t * subject);

void update_subjects();
void destroy_effect();

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
#include "components/effectslider/effectslider_gen.h"
#include "components/subtitle/subtitle_gen.h"
#include "components/title/title_gen.h"
#include "screens/effect/effect_gen.h"
#include "screens/home/home_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*PROJECT_GEN_H*/