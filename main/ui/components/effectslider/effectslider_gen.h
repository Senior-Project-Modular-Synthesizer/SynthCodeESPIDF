/**
 * @file effectslider_gen.h
 */

#ifndef EFFECTSLIDER_H
#define EFFECTSLIDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


lv_obj_t * effectslider_create(lv_obj_t * parent, const char * title, lv_subject_t * variable, short min, short max, short start);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*EFFECTSLIDER_H*/