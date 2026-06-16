#ifndef SCREEN1_H
#define SCREEN1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>

LV_IMG_DECLARE(ui_img_fond01_png);    // assets/fond01.png
LV_IMG_DECLARE(ui_img_button_rec_v3_off_png);    // assets/button_rec_V3_OFF.png
LV_IMG_DECLARE(ui_img_button_rec_v3_on_png);    // assets/button_rec_V3_ON.png
LV_IMG_DECLARE(ui_img_home_on_v2_png);    // assets/Home_ON_V2.png
LV_IMG_DECLARE(ui_img_home_off_v2_png);    // assets/Home_OFF_V2.png

void screen1_create(void);
void screen2_create(void);

void screen1_setLabelUSBState(const char*);
void screen1_setLabelRecState(const char*);
void screen1_changeUSBSwitchStatus(lv_state_t);
void screen1_changeRecStatusButton(lv_state_t);
void screen1_setLabelSaveState(const char*);
void stopperEnregistrement();
void screen1_USBDisconnected();
void screen1_enableUSBSwitch();
void screen1_disableUSBSwitch();


#ifdef __cplusplus
}
#endif

#endif