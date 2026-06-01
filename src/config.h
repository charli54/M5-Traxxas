#ifndef CONFIG_H
#define CONFIG_H

// --- PARAMÈTRES DE L'ENREGISTREMENT ---
#define FREQUENCE_HZ     20
#define DUREE_SEC        10

// Calcul automatique de la taille du tableau : 20 * 60 = 1200
#define BUFFER_SIZE      (FREQUENCE_HZ * DUREE_SEC) 

// --- PARAMÈTRES MATÉRIELS (Optionnel, mais propre) ---
#define SD_CS_PIN        4  // Remplace par ta vraie broche CS si besoin

// Broche GPIO sur laquelle est branché le switch magnétique (Ex: GPIO 1)
// /!\ Choisis une broche libre sur le connecteur du CoreS3
#define MAGNET_SWITCH_PIN 7

#endif // CONFIG_H