#ifndef SCREEN1_H
#define SCREEN1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>


void screen1_create(void);
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