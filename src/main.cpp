#include <Arduino.h>
#include <M5Unified.h>
#include <lvgl.h>

#include "screen_init.h"
#include "screen1.h"
#include "SD_init.h"
#include "config.h"

//------------------------------ENCODEUR----------------------------------------

// ── VARIABLES PARTAGÉES (INDICATIONS ARDUINO/FREERTOS) ─────────────────
// "volatile" est obligatoire pour les variables modifiées dans une interruption
volatile uint32_t compteurCoups = 0;
// Tableau pour stocker les échantillons (Ex: taille 100)
//#define BUFFER_SIZE 200
uint32_t tableauSauvegarde[BUFFER_SIZE];
int indexTableau = 0;
bool lastUSBStateConnected = true;

// Fréquence d'échantillonnage (20 Hz = 50 ms)
const uint32_t frequenceHz = FREQUENCE_HZ;
const uint32_t periodeMs = 1000 / frequenceHz; 

TaskHandle_t TaskBackgroundHandle = NULL;

// ── ROUTINE D'INTERRUPTION (ISR) ───────────────────────────────────────
// Cette fonction ultra-rapide s'exécute à CHAQUE front (montant et descendant)
void IRAM_ATTR onSwitchChange() {
    compteurCoups++;
}

static volatile bool enregistrementActif = false;

// Créer des SETTER et GETTER pour activer l'Enregistrement
// Le Setter : seule passerelle pour modifier le flag
extern "C" void setRecordingState(bool active) {
    enregistrementActif = active;
}

// La fonction intermédiaire appelée par l'écran (screen1.cpp)
extern "C" void executerSauvegardeSD() {
    
    // Le main a le droit de passer son propre tableau privé en argument !
    sauvegarderDonnees(tableauSauvegarde, BUFFER_SIZE);
    
    // On réinitialise l'index une fois la copie terminée
    indexTableau = 0;
}

extern "C" {
    bool isUSBReady();
}


// ── LA TÂCHE DE FOND (THREAED SUR CŒUR 0) ──────────────────────────────
void MaTacheDeFond(void * pvParameters) {
    //USBSerial.printf("[Thread] Démarré sur le cœur : %d à %d Hz\n", xPortGetCoreID(), frequenceHz);

    for(;;) {

        if(enregistrementActif){
            // --- 1. Sauvegarde de la valeur actuelle du compteur ---
            // On copie la valeur dans le tableau
            tableauSauvegarde[indexTableau] = compteurCoups;

            // Débug : Affiche la valeur enregistrée
            //USBSerial.printf("[Sauvegarde] Index %d = %d coups\n", indexTableau, tableauSauvegarde[indexTableau]);

            // --- 2. Gestion de l'index du tableau (Boucle circulaire) ---
            indexTableau++;
            if (indexTableau >= BUFFER_SIZE) {
                screen1_changeRecStatusButton(LV_STATE_CHECKED);
                screen1_setLabelRecState("REC");
                stopperEnregistrement();
                enregistrementActif = 0;
                indexTableau = 0; // On recommence au début (ou tu traites tes données ici)
                //USBSerial.println("[Buffer] Le tableau est plein, reset de l'index.");
            }
        }
        else{
            indexTableau = 0;
        }
        // --- 3. Timing précis à 20 Hz (50 ms) ---
        // pdMS_TO_TICKS convertit automatiquement les millisecondes en cycles FreeRTOS
        vTaskDelay(pdMS_TO_TICKS(periodeMs)); 
    }
}


//---------------------------------------------------------------------------------


void setup() {
    Serial.begin(115200);
    screen_init();    // init hardware + LVGL
    screen1_create(); // construction de l'UI
    sdInit();
    disableUsbMode();
    // 1.INPUT tout court, car la résistance de 10K externe fait déjà le travail
    pinMode(MAGNET_SWITCH_PIN, INPUT);

    // 2. Attacher l'interruption sur la broche
    // CHANGE : Déclenche l'interruption sur front montant ET descendant
    attachInterrupt(digitalPinToInterrupt(MAGNET_SWITCH_PIN), onSwitchChange, CHANGE);

    // 3. Création de la tâche FreeRTOS bloquée sur le Cœur 0
    xTaskCreatePinnedToCore(
        MaTacheDeFond,          // Fonction
        "TaskBackground",       // Nom
        4096,                   // Pile (Stack)
        NULL,                   // Paramètres
        1,                      // Priorité
        &TaskBackgroundHandle,  // Handle
        0                       // Cœur 0
    );

}

void loop() {
    M5.update();
    lv_timer_handler();

    // 1. On lit l'état actuel directement
    bool USBConnected = isUSBReady();

    // 2. Le Latch : On vérifie s'il y a eu un CHANGEMENT d'état
    if (USBConnected != lastUSBStateConnected) {
        lastUSBStateConnected = USBConnected; // On mémorise le nouvel état immédiatement

        // 3. On applique l'action graphique une seule fois selon le cas
        if (USBConnected) {
            screen1_enableUSBSwitch();
        } else {
            screen1_disableUSBSwitch();
        }
    }

    if (isUsbModeActive() && usbEjected){

        disableUsbMode();

        // 2. On met à jour l'interface graphique pour avertir l'utilisateur
        //screen1_setLabelUSBState("Aquisition");
        //screen1_changeUSBSwitchStatus(LV_STATE_CHECKED);
        screen1_USBDisconnected();
        screen1_enableUSBSwitch();

    }
/*
    if(usbPretEtMonte){
        usbPretEtMonte = false;
        screen1_enableUSBSwitch();
        //screen1_setLabelSaveState("PRET");
    }*/

    delay(5);
}