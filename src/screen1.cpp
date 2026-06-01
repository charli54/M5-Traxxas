#include "screen1.h"
#include "SD_init.h"
#include "config.h"

#include <lvgl.h>

const char* text = "Aquisition";
const char * recStatus = "REC";
static lv_obj_t * labelUSB = nullptr;
static lv_obj_t * swUSBStatus = nullptr;
static lv_obj_t * ui_Button1 = nullptr;
static lv_obj_t * saveButton  = nullptr;
static lv_obj_t * btRecStatus = nullptr;
static lv_obj_t * labelRecStatus = nullptr;
static lv_obj_t * labelSaveButtonStatus_p = nullptr;
static lv_obj_t * bar_recStatus = nullptr;
static lv_obj_t * symbol_rec = nullptr;
static lv_anim_t anim_bar;

//---------------------------------------------------ANIMATIONS-------------------------------------------------
//-------------------------------------------ANIMATION DE LA BARRE D'ENREGISTREMENT-----------------------------
void demarrer_animation_barre() {
    // 1. On remet d'abord la barre à zéro
    lv_bar_set_value(bar_recStatus, 0, LV_ANIM_OFF);

    // 2. Initialisation de l'animation
    lv_anim_init(&anim_bar);
    lv_anim_set_var(&anim_bar, bar_recStatus); // L'objet à animer (ta barre)
    
    // 3. Définition des valeurs (de 0 à BUFFER_SIZE)
    lv_anim_set_values(&anim_bar, 0, BUFFER_SIZE); 
    
    // 4. Définition du temps (10 secondes = 10000 millisecondes)
    lv_anim_set_time(&anim_bar, DUREE_SEC*1000); 
    
    // 5. On indique à LVGL quelle fonction native utiliser pour modifier la valeur de la barre
    lv_anim_set_exec_cb(&anim_bar, (lv_anim_exec_xcb_t)lv_bar_set_value);
    
    // 6. On lance l'animation
    lv_anim_start(&anim_bar);
}

void stopper_animation_barre() {
    // Arrête l'animation en cours sur la barre
    lv_anim_del(bar_recStatus, (lv_anim_exec_xcb_t)lv_bar_set_value);
}
//------------------------------------------------------------------------------------------------------

//----------------------------------------------------FONCTIONS EXTERNES---------------------------------
extern "C" {
    LV_FONT_DECLARE(metal_rocks_24);
}

extern "C" {
    LV_FONT_DECLARE(super_crown_14);
}

extern "C" {
    LV_FONT_DECLARE(montserrat_regular_18);
}

extern "C" {
    void setRecordingState(bool active);
}

extern "C" {
    void executerSauvegardeSD();
}

extern "C" {
    bool isUSBReady();
}

//---------------------------------------------SETTER POUR LE LABEL_USB-----------------------------
void screen1_setLabelUSBState(const char * text) {
    if (labelUSB != nullptr) {
        lv_label_set_text(labelUSB, text);
    }
}

void screen1_changeUSBSwitchStatus(lv_state_t state){
    lv_obj_clear_state(swUSBStatus, state);
}

void screen1_changeRecStatusButton(lv_state_t state){
    lv_obj_clear_state(btRecStatus, state);
}

void screen1_setLabelRecState(const char * text){
    lv_label_set_text(labelRecStatus, text);
}

void screen1_setLabelSaveState(const char* text){
    lv_label_set_text(labelSaveButtonStatus_p, text);
}

void stopperEnregistrement(){
    setRecordingState(0);
    lv_label_set_text(labelRecStatus, "REC");
    lv_obj_clear_state(btRecStatus, LV_STATE_CHECKED);
    lv_obj_set_style_radius(symbol_rec, LV_RADIUS_CIRCLE, 0); // Rayon maximal = cercle
    stopper_animation_barre();
    lv_obj_clear_state(saveButton, LV_STATE_DISABLED);
}

void screen1_USBDisconnected(){
    lv_obj_clear_state(swUSBStatus, LV_STATE_CHECKED);
    lv_label_set_text(labelUSB, "Aquisition");
    lv_obj_clear_state(ui_Button1, LV_STATE_DISABLED);
    lv_obj_clear_state(saveButton, LV_STATE_DISABLED);
}

void screen1_enableUSBSwitch(){
    lv_obj_clear_state(swUSBStatus, LV_STATE_DISABLED);
    lv_label_set_text(labelUSB, "Activer pour transferer");
}

void screen1_disableUSBSwitch(){
    lv_obj_add_state(swUSBStatus, LV_STATE_DISABLED);
    lv_label_set_text(labelUSB, "USB non connect\xC3\xA9");
}
// ──------------- EVENTS Callbacks ────────────────────────────────────────────────────
//----------------------------------EVENT POUR LE TRANSFERT USB---------------------------------------
static void switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);

    // On récupère le label qui a été attaché à ce switch spécifique
    lv_obj_t *target_label = (lv_obj_t *)lv_event_get_user_data(e);


    //Serial.printf("Switch: %s\n", lv_obj_has_state(sw, LV_STATE_CHECKED) ? "ON" : "OFF");
    if(lv_obj_has_state(sw, LV_STATE_CHECKED)){
        lv_label_set_text(target_label, "Transfert USB");
        lv_obj_add_state(ui_Button1, LV_STATE_DISABLED);
        lv_obj_add_state(saveButton, LV_STATE_DISABLED);
        lv_obj_add_state(swUSBStatus, LV_STATE_DISABLED);
        // 2. FORCE LE RAFRAÎCHISSEMENT PHYSIQUE DE L'ÉCRAN ICI
        // L'écran se met à jour instantanément à l'état "figé/chargement"
        lv_refr_now(NULL);

        enableUsbMode();
    }
    else{
        disableUsbMode();
        lv_label_set_text(target_label, "Aquisition");
        lv_obj_clear_state(ui_Button1, LV_STATE_DISABLED);
        lv_obj_clear_state(saveButton, LV_STATE_DISABLED);
    }
}

//------------------------------EVENT POUR L'ENREGISTREMENMT-------------------------------------------
void ui_event_Button1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * target_symbol = (lv_obj_t *)lv_event_get_user_data(e);

    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED)) {
        (e);
        setRecordingState(LV_STATE_CHECKED);
        lv_obj_add_state(saveButton, LV_STATE_DISABLED);
        lv_label_set_text(labelRecStatus, "STOP");
        lv_obj_set_style_radius(target_symbol, 4, 0); // Rayon maximal = cercle
        demarrer_animation_barre();
    }
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED)) {
        (e);
        stopperEnregistrement();
    }
}

//-------------------------------EVENT POUR LA SAUVEGARDE--------------------------------------------------
void ui_event_ButtonSave(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_CLICKED) {
        executerSauvegardeSD(); // Déclenche la chaîne de sauvegarde
    }
}

// ── Helpers styles ───────────────────────────────────────────────
static void apply_switch_style(lv_obj_t *sw) {
    // Fond OFF (gris)
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa  (sw, LV_OPA_COVER,           LV_PART_MAIN | LV_STATE_DEFAULT);

    // Indicateur OFF transparent
    lv_obj_set_style_bg_opa(sw, LV_OPA_TRANSP, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Fond ON (rouge → change en 0x00FF00 pour vert)
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa  (sw, LV_OPA_COVER,           LV_PART_INDICATOR | LV_STATE_CHECKED);

    // Bordure
    lv_obj_set_style_border_color(sw, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(sw, 2,                       LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(sw, 0,                       LV_PART_MAIN | LV_STATE_DEFAULT);

    // Knob blanc
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
}

// ── Point d'entrée public ────────────────────────────────────────
void screen1_create(void) {
    lv_obj_t *scr = lv_scr_act();

    // Titre

    lv_obj_t *labelTitre = lv_label_create(scr);
    lv_label_set_text(labelTitre, "TRAXXAS");
    lv_obj_align(labelTitre, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(labelTitre, &metal_rocks_24, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_text_font (labelTitre, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelTitre, lv_color_hex(0xFF0000),  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa  (labelTitre, LV_OPA_COVER,            LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, text);
    lv_obj_set_width(label, 175);
    lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_set_style_text_font (label, &montserrat_regular_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF),  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa  (label, LV_OPA_COVER,            LV_PART_MAIN | LV_STATE_DEFAULT);
    labelUSB = label;


    // Switch 2 — grand, activé par défaut
    lv_obj_t *sw2 = lv_switch_create(scr);
    lv_obj_set_size(sw2, 100, 55);
    lv_obj_align(sw2, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
    apply_switch_style(sw2);
    lv_obj_add_state(sw2, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(sw2, switch_event_cb, LV_EVENT_VALUE_CHANGED, (void*)label);
    swUSBStatus = sw2;

    //--------------------------------BOUTON TOGGLE POUR L'ENREGISTREMENT----------------------------------
    ui_Button1 = lv_btn_create(scr);
    lv_obj_set_width(ui_Button1, 125);
    lv_obj_set_height(ui_Button1, 54);
    //lv_obj_set_x(ui_Button1, -50);
    //lv_obj_set_y(ui_Button1, 0);
    lv_obj_align(ui_Button1, LV_ALIGN_CENTER, -70, 0);
    //lv_obj_set_align(ui_Button1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Button1, lv_color_hex(0xCECECE), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui_Button1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Button1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    btRecStatus = ui_Button1;

    // 1. Création du rond pour le Record
    symbol_rec = lv_obj_create(ui_Button1); // ui_screen1 est ton parent (l'écran)
    // 2. Définition de la taille (un carré)
    lv_obj_set_size(symbol_rec, 30, 30); // Un cercle de 20 pixels de diamètre
    // 3. Positionnement (par exemple, en haut à gauche ou à côté de ton bouton)
    lv_obj_align(symbol_rec, LV_ALIGN_CENTER, -30, 0);
    // 4. Application du style pour en faire un cercle rouge
    lv_obj_set_style_radius(symbol_rec, LV_RADIUS_CIRCLE, 0); // Rayon maximal = cercle
    lv_obj_set_style_bg_color(symbol_rec, lv_color_hex(0xFF0000), 0); // Rouge vif
    lv_obj_set_style_bg_opa(symbol_rec, LV_OPA_COVER, 0); // Opacité totale
    lv_obj_set_style_border_opa(symbol_rec, LV_OPA_0, 0); // Pas de bordure

    lv_obj_t *label_rec = lv_label_create(ui_Button1);
    lv_label_set_text(label_rec, recStatus);
    lv_obj_align(label_rec, LV_ALIGN_CENTER, 20, 0);
    lv_obj_set_style_text_font (label_rec, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_rec, lv_color_hex(0xFF0000),  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa  (label_rec, LV_OPA_COVER,            LV_PART_MAIN | LV_STATE_DEFAULT);
    labelRecStatus = label_rec;

                        //-----------BARRE DE PROGRESION---------------------------------------------
    bar_recStatus = lv_bar_create(scr);
    lv_bar_set_range(bar_recStatus, 0, 200); //faudrait passer BUFFER_SIZE parla suite
    lv_bar_set_value(bar_recStatus, 0, LV_ANIM_OFF);
    lv_bar_set_start_value(bar_recStatus, 0, LV_ANIM_OFF);
    lv_obj_set_width(bar_recStatus, 125);
    lv_obj_set_height(bar_recStatus, 10);
    lv_obj_set_x(bar_recStatus, -70);
    lv_obj_set_y(bar_recStatus, -50);
    lv_obj_set_align(bar_recStatus, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, (void*)symbol_rec);
    //--------------------------BOUTON SAUVEGARDE------------------------------------------------------
    saveButton = lv_btn_create(scr);
    lv_obj_set_size(saveButton, 125, 55);
    lv_obj_align(saveButton, LV_ALIGN_CENTER, 70, 0);
    lv_obj_add_event_cb(saveButton, ui_event_ButtonSave, LV_EVENT_ALL, NULL);

    lv_obj_t * labelSaveButton = lv_label_create(saveButton);
    lv_label_set_text(labelSaveButton, "SAVE TO SD");
    lv_obj_align(labelSaveButton, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font (labelSaveButton, &montserrat_regular_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelSaveButton, lv_color_hex(0xFFFFFF),  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa  (labelSaveButton, LV_OPA_COVER,            LV_PART_MAIN | LV_STATE_DEFAULT);    

    lv_obj_t * labelSaveButtonStatus =lv_label_create(scr);
    lv_label_set_text(labelSaveButtonStatus, "vide");
    lv_obj_align(labelSaveButtonStatus, LV_ALIGN_CENTER, 70, -50);
    lv_obj_set_style_text_font (labelSaveButtonStatus, &montserrat_regular_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelSaveButtonStatus, lv_color_hex(0xFFFFFF),  LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa  (labelSaveButtonStatus, LV_OPA_COVER,            LV_PART_MAIN | LV_STATE_DEFAULT); 
    labelSaveButtonStatus_p = labelSaveButtonStatus;  

}