#ifndef SD_INIT_H
#define SD_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool usbEjected;
extern volatile bool usbPretEtMonte;

void sdInit(void);
void enableUsbMode();
void disableUsbMode();
bool isUsbModeActive();
void sauvegarderDonnees(uint32_t *tableau, int taille); 

#ifdef __cplusplus
}
#endif

#endif