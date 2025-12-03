/**
 * @file Project_gen.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui.h"

#if LV_USE_XML
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/
lv_obj_t * home;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/
const lv_font_t * font_title;
extern uint8_t Inter_SemiBold_ttf_data[];
extern size_t Inter_SemiBold_ttf_data_size;
const lv_font_t * font_subtitle;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/
lv_subject_t subjects[6];
lv_observer_t * observers[6];
void * pointers[6];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void gui_init()
{
    ESP_LOGI("GUI", "Initializing GUI");
    char buf[256];

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Fonts
     *----------------*/
    /* create tiny ttf font 'font_title' from C array */
    font_title = &lv_font_montserrat_28;
    /* create tiny ttf font 'font_subtitle' from C array */
    font_subtitle = &lv_font_montserrat_20;

    /*----------------
     * Subjects
     *----------------*/
    for (int i = 0; i < 6; i++){
      pointers[i] = NULL;
    }

    /*--------------------
    *  Permanent screens
    *-------------------*/
    home = home_create();

    ESP_LOGI("GUI", "GUI Initialized");
}

/* callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) arc_changed(lv_event_t * e)
{
   LV_UNUSED(e);
   LV_LOG("arc_changed was called\n");
}
void __attribute__((weak)) slider_changed(lv_event_t * e)
{
   LV_UNUSED(e);
   LV_LOG("slider_changed was called\n");
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

void arc_changed(lv_event_t * e) {

}

void slider_changed(lv_event_t * e) {

}

void check_changed(lv_event_t * e) {
   lv_obj_t * obj = lv_event_get_target_obj(e);
   int i = lv_obj_get_index(obj);
   if (lv_obj_has_state(obj, LV_STATE_CHECKED))
      lv_obj_add_state(obj, LV_STATE_DISABLED);
   else
      lv_obj_add_state(obj, LV_STATE_CHECKED);

    *(int*)pointers[i] = !(*(int*)pointers[i]);
}

void observer_cb(lv_observer_t * observer, lv_subject_t * subject) {
   void * ptr = lv_observer_get_user_data(observer);
   if (subject->type == LV_SUBJECT_TYPE_INT)
      *(int *)(ptr) = lv_subject_get_int(subject);
   else
      *(float *)(ptr) = lv_subject_get_float(subject);
}

void update_subjects() {
   for (int i = 0; i < 6; i++) {
      if (pointers[i] != NULL && observers[i] == NULL) {
         if (subjects[i].type == LV_SUBJECT_TYPE_INT)
            lv_subject_set_int(&subjects[i], *((int *)(pointers[i])));
         else
            lv_subject_set_float(&subjects[i], *((float *)(pointers[i])));
      }
   }
}

void destroy_effect() {
   for (int i = 0; i < 6; i++){
      lv_subject_deinit(&subjects[i]);
      pointers[i] = NULL;
      // lv_observer_remove(observers[i]); // Already done by deinit
    }
}
