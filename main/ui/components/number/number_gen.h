/**
 * @file number_gen.h
 */

#ifndef NUMBER_H
#define NUMBER_H

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


lv_obj_t * number_create(lv_obj_t * parent, const char * text, lv_subject_t * subject);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUMBER_H*/