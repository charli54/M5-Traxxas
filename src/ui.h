#ifndef _UI_H
#define _UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
///////////////////// SCREENS ////////////////////

#include "screen1.h"

///////////////////// VARIABLES ////////////////////


// EVENTS

extern lv_obj_t * ui____initial_actions0;

// IMAGES AND IMAGE SETS
LV_IMG_DECLARE(ui_img_plan_de_travail_101_png);    // assets/Plan de travail 101.png
LV_IMG_DECLARE(ui_img_bouton_rec01_png);    // assets/bouton_rec01.png
LV_IMG_DECLARE(ui_img_fond01_png);    // assets/fond01.png
LV_IMG_DECLARE(ui_img_button_rec_v3_off_png);    // assets/button_rec_V3_OFF.png
LV_IMG_DECLARE(ui_img_button_rec_v3_on_png);    // assets/button_rec_V3_ON.png
LV_IMG_DECLARE(ui_img_button_save_png);    // assets/button_save.png
LV_IMG_DECLARE(ui_img_button_transfert_ok_png);    // assets/button_transfert_OK.png
LV_IMG_DECLARE(ui_img_button_transfert_png);    // assets/button_transfert.png
LV_IMG_DECLARE(ui_img_home_on_png);    // assets/Home_ON.png
LV_IMG_DECLARE(ui_img_home_png);    // assets/Home.png
LV_IMG_DECLARE(ui_img_graph_off_png);    // assets/Graph_OFF.png
LV_IMG_DECLARE(ui_img_graph_on_png);    // assets/Graph_ON.png

// UI INIT

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif