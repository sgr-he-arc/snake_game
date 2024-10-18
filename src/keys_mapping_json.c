# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "cJSON.h"
# include "keys_mapping_json.h"

KeyMapping* game_keys = NULL;
int num_commands = 0;

void clear_datas(char* data, cJSON* json) {
    free(data);
    cJSON_Delete(json);
}

int map_key(const char* key) {
    if (strcmp(key, "ArrowUp") == 0) {
        return 72;  // Code pour la flèche haut
    } else if (strcmp(key, "ArrowDown") == 0) {
        return 80;  // Code pour la flèche bas
    } else if (strcmp(key, "ArrowLeft") == 0) {
        return 75;  // Code pour la flèche gauche
    } else if (strcmp(key, "ArrowRight") == 0) {
        return 77;  // Code pour la flèche droite
    } else if (strlen(key) == 1) {
        return key[0];  // Retourner un caractère normal
    }
    else {
        return -1;
    }
}

int read_key(cJSON* controls, const char* key_name, int* key_var) {
    cJSON* key = cJSON_GetObjectItemCaseSensitive(controls, key_name);
    if (cJSON_IsString(key) && (key->valuestring != NULL)) {
        *key_var = map_key(key->valuestring);
        if (*key_var == -1) {
            return 1;
        }
        return 0;
    }
    else {
        printf("Erreur lors de la lecture de la touche %s\n", key_name);
        return 1;
    }
}

int read_config_file(void) {
    FILE* file = fopen("../assets/config/settings.json", "rb"); // lit le fichier en binaire
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier settings.json. Vérifiez \n");
        return 1;
    }

    // Trouver la taille du fichier
    fseek(file, 0, SEEK_END); // place le curseur à la fin du fichier
    long length = ftell(file); // récupère la taille du fichier du début jusqu'au curseur
    fseek(file, 0, SEEK_SET); // place le curseur au début du fichier

    // mets le fichier dans la mémoire avec un tampon (liste de C)
    char* data = (char*)malloc(length + 1);
    if (data == NULL) {
        printf("Erreur d'allocation mémoire\n");
        fclose(file);
        return 1;
    }
    fread(data, 1, length, file); // lit length fois 1 char et le met dans data
    fclose(file); // ferme le fichier
    data[length] = '\0'; // pour dire que c'est la fin de la liste
    
    cJSON* json = cJSON_Parse(data);
    if (json == NULL) {
        printf("Erreur lors du parse du json\n");
        free(data);
        return 1;
    }

    cJSON* controls = cJSON_GetObjectItemCaseSensitive(json, "controls");
    if (controls && cJSON_IsArray(controls)) {
        num_commands = cJSON_GetArraySize(controls);
        game_keys = (KeyMapping*)malloc(num_commands * sizeof(KeyMapping));
        if (game_keys == NULL) {
            printf("Memory alloc. error\n");
            return 1;
        }

        int index = 0;
        cJSON* control = NULL;
        cJSON_ArrayForEach(control, controls) {
            cJSON* label = cJSON_GetObjectItemCaseSensitive(control, "label");
            cJSON* key = cJSON_GetObjectItemCaseSensitive(control, "key");
            if (cJSON_IsString(label) && cJSON_IsString(key) && label->valuestring != NULL && key->valuestring != NULL) {
                game_keys[index].label = strdup(label->valuestring);
                game_keys[index].key = map_key(key->valuestring);
                index++;
            }
        }
    }
    else {
        printf("Section 'controls' manquante dans le fichier JSON\n");
        clear_datas(data, json);
        return 1;
    }
    clear_datas(data, json);
    return 0;
}

void save_config_file(void) {
    cJSON *json = cJSON_CreateObject();
    cJSON *controls = cJSON_CreateArray();

    for (int i = 0; i < num_commands; i++) {
        cJSON *command = cJSON_CreateObject();
        cJSON_AddStringToObject(command, "label", game_keys[i].label);

        char keyStr[32];

        // Gérer les flèches et autres touches spéciales
        switch (game_keys[i].key) {
            case 72:  // Flèche haut
                strcpy(keyStr, "ArrowUp");
                break;
            case 80:  // Flèche bas
                strcpy(keyStr, "ArrowDown");
                break;
            case 75:  // Flèche gauche
                strcpy(keyStr, "ArrowLeft");
                break;
            case 77:  // Flèche droite
                strcpy(keyStr, "ArrowRight");
                break;
            default:
                // Pour les autres touches, on convertit simplement le caractère
                snprintf(keyStr, sizeof(keyStr), "%c", game_keys[i].key);
                break;
        }

        cJSON_AddStringToObject(command, "key", keyStr);
        cJSON_AddItemToArray(controls, command);
    }

    FILE* file = fopen("../assets/config/settings.json", "w");
    if (file == NULL) {
        printf("unable to open settings file");
        cJSON_Delete(json);
    }

    cJSON_AddItemToObject(json, "controls", controls);

    char *json_string = cJSON_Print(json);
    if (json_string) {
        fprintf(file, "%s", json_string);
        free(json_string);
    }

    fclose(file);

    cJSON_Delete(json);
}
