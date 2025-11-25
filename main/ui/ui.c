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
lv_font_t * font_title;
extern uint8_t Inter_SemiBold_ttf_data[];
extern size_t Inter_SemiBold_ttf_data_size;
lv_font_t * font_subtitle;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/
lv_subject_t hours;
lv_subject_t mins;
lv_subject_t age;
lv_subject_t bluetooth_on;
lv_subject_t wifi_on;
lv_subject_t notification_on;
lv_subject_t hour_edited;
lv_subject_t min_edited;
lv_subject_t arc1;
lv_subject_t arc2;
lv_subject_t slide;
lv_subject_t check;

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
    font_title = &lv_font_montserrat_20;
    /* create tiny ttf font 'font_subtitle' from C array */
    font_subtitle = &lv_font_montserrat_14;

    /*----------------
     * Images
     *----------------*/


    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&hours, 17);
    lv_subject_init_int(&mins, 45);
    lv_subject_init_int(&age, 17);
    lv_subject_init_int(&bluetooth_on, 0);
    lv_subject_init_int(&wifi_on, 0);
    lv_subject_init_int(&notification_on, 0);
    lv_subject_init_int(&hour_edited, 0);
    lv_subject_init_int(&min_edited, 0);
    lv_subject_init_int(&arc1, 50);
    lv_subject_init_int(&arc2, 50);
    lv_subject_init_int(&slide, 50);
    lv_subject_init_int(&check, 0);

    /*--------------------
    *  Permanent screens
    *-------------------*/
    // home = home_create();

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