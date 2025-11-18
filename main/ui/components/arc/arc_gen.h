/**
 * @file arc_gen.h
 */

#ifndef ARC_H
#define ARC_H

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


lv_obj_t * arc_create(lv_obj_t * parent, const char * title, lv_subject_t * variable);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*ARC_H*/