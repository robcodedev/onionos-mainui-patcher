#ifndef TWEAKS_MAIN_MENU_H__
#define TWEAKS_MAIN_MENU_H__

// SPDX-License-Identifier: GPL-3.0-only
//
// Tweaks controls for the menu and SELECT-menu visibility handled by the
// optional patched MainUI main-menu.json reader. Existing custom actions,
// unknown/unsupported root fields and recognized item order are kept. The
// root hotkey setting is editable separately from the context-menu entries.

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "components/list.h"
#include "utils/json.h"

#define MAINMENU_CONFIG_DIR "/mnt/SDCARD/.tmp_update/config"
#define MAINMENU_CONFIG_PATH MAINMENU_CONFIG_DIR "/main-menu.json"
#define MAINMENU_CONFIG_MAX_SIZE (128 * 1024)

#define MAINMENU_MENU_COUNT 6
#define MAINMENU_CONTEXT_COUNT 14
#define MAINMENU_CONTEXT_LIST_COUNT (MAINMENU_CONTEXT_COUNT + 1)

static const char *mainmenu_menu_keys[MAINMENU_MENU_COUNT] = {
    "recents", "favorites", "games", "apps", "settings", "expert"};

static const char *mainmenu_menu_labels[MAINMENU_MENU_COUNT] = {
    "Recents", "Favorites", "Games", "Apps", "Settings", "Expert"};

static const char *mainmenu_context_keys[MAINMENU_CONTEXT_COUNT] = {
    "shutdown", "refresh", "search", "recents", "favorites", "games",
    "apps", "settings", "expert", "themes", "tweaks", "custom1",
    "custom2", "custom3"};

static const char *mainmenu_context_labels[MAINMENU_CONTEXT_COUNT] = {
    "Shutdown", "Refresh all roms", "Search", "Recents", "Favorites", "Games",
    "Apps", "Settings", "Expert", "Themes", "Tweaks", "Custom 1",
    "Custom 2", "Custom 3"};

static void mainmenu_set_default_menu(bool states[MAINMENU_MENU_COUNT])
{
    memset(states, 0, sizeof(bool) * MAINMENU_MENU_COUNT);
    states[1] = true; // Favorites
    states[2] = true; // Games
    states[3] = true; // Apps
    states[4] = true; // Settings
}

static void mainmenu_set_default_context(bool states[MAINMENU_CONTEXT_COUNT])
{
    memset(states, 0, sizeof(bool) * MAINMENU_CONTEXT_COUNT);
    states[1] = true;  // Refresh all roms
    states[2] = true;  // Search
    states[10] = true; // Tweaks
}

static int mainmenu_menu_key_index(const char *key)
{
    if (key == NULL)
        return -1;
    if (strcmp(key, "recents") == 0 || strcmp(key, "recent") == 0)
        return 0;
    if (strcmp(key, "favorites") == 0 || strcmp(key, "favorite") == 0 ||
        strcmp(key, "favourite") == 0 || strcmp(key, "favourites") == 0 ||
        strcmp(key, "favs") == 0)
        return 1;
    for (int i = 2; i < MAINMENU_MENU_COUNT; i++) {
        if (strcmp(key, mainmenu_menu_keys[i]) == 0)
            return i;
    }
    return -1;
}

static int mainmenu_context_key_index(const char *key)
{
    if (key == NULL)
        return -1;
    if (strcmp(key, "recents") == 0 || strcmp(key, "recent") == 0)
        return 3;
    if (strcmp(key, "favorites") == 0 || strcmp(key, "favorite") == 0 ||
        strcmp(key, "favourite") == 0 || strcmp(key, "favourites") == 0 ||
        strcmp(key, "favs") == 0)
        return 4;
    for (int i = 0; i < MAINMENU_CONTEXT_COUNT; i++) {
        if (i == 3 || i == 4)
            continue;
        if (strcmp(key, mainmenu_context_keys[i]) == 0)
            return i;
    }
    return -1;
}

static bool mainmenu_ensure_config_dir(void)
{
    if (mkdir(MAINMENU_CONFIG_DIR, 0755) == 0)
        return true;
    if (errno != EEXIST)
        return false;

    struct stat st;
    return stat(MAINMENU_CONFIG_DIR, &st) == 0 && S_ISDIR(st.st_mode);
}

static char *mainmenu_read_config(bool *exists_out)
{
    *exists_out = false;

    struct stat st;
    if (stat(MAINMENU_CONFIG_PATH, &st) != 0)
        return NULL;
    *exists_out = true;
    if (!S_ISREG(st.st_mode) || st.st_size < 0 ||
        st.st_size > MAINMENU_CONFIG_MAX_SIZE)
        return NULL;

    FILE *fp = fopen(MAINMENU_CONFIG_PATH, "rb");
    if (fp == NULL)
        return NULL;

    size_t size = (size_t)st.st_size;
    char *text = malloc(size + 1);
    if (text == NULL) {
        fclose(fp);
        return NULL;
    }

    bool ok = fread(text, 1, size, fp) == size;
    if (fclose(fp) != 0)
        ok = false;
    if (!ok) {
        free(text);
        return NULL;
    }
    text[size] = '\0';
    return text;
}

static void mainmenu_relax_json(char *text)
{
    bool in_string = false;
    bool escaped = false;
    bool line_comment = false;
    bool block_comment = false;

    for (size_t i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        char next = text[i + 1];

        if (line_comment) {
            if (ch == '\n' || ch == '\r')
                line_comment = false;
            else
                text[i] = ' ';
            continue;
        }
        if (block_comment) {
            if (ch == '*' && next == '/') {
                text[i] = ' ';
                text[i + 1] = ' ';
                i++;
                block_comment = false;
            }
            else if (ch != '\n' && ch != '\r') {
                text[i] = ' ';
            }
            continue;
        }
        if (in_string) {
            if (escaped)
                escaped = false;
            else if (ch == '\\')
                escaped = true;
            else if (ch == '"')
                in_string = false;
            continue;
        }
        if (ch == '"') {
            in_string = true;
            continue;
        }
        if (ch == '#') {
            text[i] = ' ';
            line_comment = true;
            continue;
        }
        if (ch == '/' && next == '/') {
            text[i] = ' ';
            text[i + 1] = ' ';
            i++;
            line_comment = true;
            continue;
        }
        if (ch == '/' && next == '*') {
            text[i] = ' ';
            text[i + 1] = ' ';
            i++;
            block_comment = true;
        }
    }

    in_string = false;
    escaped = false;
    for (size_t i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        if (in_string) {
            if (escaped)
                escaped = false;
            else if (ch == '\\')
                escaped = true;
            else if (ch == '"')
                in_string = false;
            continue;
        }
        if (ch == '"') {
            in_string = true;
            continue;
        }
        if (ch != ',')
            continue;

        size_t next = i + 1;
        while (text[next] != '\0' && isspace((unsigned char)text[next]))
            next++;
        if (text[next] == '}' || text[next] == ']')
            text[i] = ' ';
    }
}

static cJSON *mainmenu_load_root(bool *exists_out, bool *valid_out)
{
    char *text = mainmenu_read_config(exists_out);
    if (!*exists_out) {
        *valid_out = true;
        return cJSON_CreateObject();
    }
    if (text == NULL) {
        *valid_out = false;
        return NULL;
    }

    mainmenu_relax_json(text);
    cJSON *root = cJSON_Parse(text);
    free(text);
    if (!cJSON_IsObject(root)) {
        cJSON_Delete(root);
        *valid_out = false;
        return NULL;
    }

    *valid_out = true;
    return root;
}

static void mainmenu_read_menu_states(cJSON *root,
                                      bool states[MAINMENU_MENU_COUNT])
{
    memset(states, 0, sizeof(bool) * MAINMENU_MENU_COUNT);
    cJSON *menu = cJSON_GetObjectItemCaseSensitive(root, "menu");
    if (cJSON_IsObject(menu)) {
        for (cJSON *item = menu->child; item != NULL; item = item->next) {
            int index = mainmenu_menu_key_index(item->string);
            if (index >= 0)
                states[index] = cJSON_IsTrue(item);
        }
    }

    bool any = false;
    for (int i = 0; i < MAINMENU_MENU_COUNT; i++)
        any = any || states[i];
    if (!any)
        mainmenu_set_default_menu(states);
}


static bool mainmenu_menu_object_has_enabled(cJSON *menu)
{
    if (!cJSON_IsObject(menu))
        return false;
    for (cJSON *item = menu->child; item != NULL; item = item->next) {
        if (mainmenu_menu_key_index(item->string) >= 0 && cJSON_IsTrue(item))
            return true;
    }
    return false;
}

static void mainmenu_read_context_states(cJSON *root,
                                         bool states[MAINMENU_CONTEXT_COUNT])
{
    memset(states, 0, sizeof(bool) * MAINMENU_CONTEXT_COUNT);
    cJSON *context = cJSON_GetObjectItemCaseSensitive(root, "context");
    if (context == NULL) {
        mainmenu_set_default_context(states);
        return;
    }

    if (cJSON_IsObject(context)) {
        for (cJSON *item = context->child; item != NULL; item = item->next) {
            int index = mainmenu_context_key_index(item->string);
            if (index >= 0)
                states[index] = cJSON_IsTrue(item);
        }
    }
    else if (cJSON_IsArray(context)) {
        int count = cJSON_GetArraySize(context);
        for (int i = 0; i < count; i++) {
            cJSON *item = cJSON_GetArrayItem(context, i);
            if (!cJSON_IsString(item))
                continue;
            int index = mainmenu_context_key_index(item->valuestring);
            if (index >= 0)
                states[index] = true;
        }
    }
}

static bool mainmenu_read_hotkey_state(cJSON *root)
{
    cJSON *hotkey = cJSON_GetObjectItemCaseSensitive(root, "hotkey");
    return hotkey == NULL || cJSON_IsTrue(hotkey);
}

static bool mainmenu_load_hotkey_state(void)
{
    bool exists = false;
    bool valid = false;
    cJSON *root = mainmenu_load_root(&exists, &valid);
    if (!valid || root == NULL) {
        cJSON_Delete(root);
        return true;
    }

    bool enabled = mainmenu_read_hotkey_state(root);
    cJSON_Delete(root);
    return enabled;
}

static void mainmenu_load_states(bool menu_states[MAINMENU_MENU_COUNT],
                                 bool context_states[MAINMENU_CONTEXT_COUNT])
{
    bool exists = false;
    bool valid = false;
    cJSON *root = mainmenu_load_root(&exists, &valid);
    if (!valid || root == NULL) {
        mainmenu_set_default_menu(menu_states);
        mainmenu_set_default_context(context_states);
        cJSON_Delete(root);
        return;
    }

    mainmenu_read_menu_states(root, menu_states);
    mainmenu_read_context_states(root, context_states);
    cJSON_Delete(root);
}

static bool mainmenu_write_root(cJSON *root)
{
    if (root == NULL || !mainmenu_ensure_config_dir())
        return false;

    char *output = cJSON_Print(root);
    if (output == NULL)
        return false;

    size_t length = strlen(output);
    if (length > MAINMENU_CONFIG_MAX_SIZE) {
        cJSON_free(output);
        return false;
    }

    char temporary[256];
    if (snprintf(temporary, sizeof(temporary), "%s.tmp", MAINMENU_CONFIG_PATH) >=
        (int)sizeof(temporary)) {
        cJSON_free(output);
        return false;
    }

    FILE *fp = fopen(temporary, "wb");
    if (fp == NULL) {
        cJSON_free(output);
        return false;
    }

    bool ok = fwrite(output, 1, length, fp) == length;
    if (ok)
        ok = fwrite("\n", 1, 1, fp) == 1;
    if (ok)
        ok = fflush(fp) == 0;
    if (ok)
        ok = fsync(fileno(fp)) == 0;
    if (fclose(fp) != 0)
        ok = false;
    cJSON_free(output);

    if (!ok) {
        remove(temporary);
        return false;
    }
    if (rename(temporary, MAINMENU_CONFIG_PATH) != 0) {
        remove(temporary);
        return false;
    }
    return true;
}

static bool mainmenu_replace_root_section(cJSON *root, const char *name,
                                          cJSON *replacement)
{
    cJSON *old = cJSON_GetObjectItemCaseSensitive(root, name);
    if (old != NULL)
        return cJSON_ReplaceItemInObjectCaseSensitive(root, name, replacement);
    return cJSON_AddItemToObject(root, name, replacement);
}

static bool mainmenu_set_object_alias(cJSON *object, int target_index,
                                      bool enabled, bool context)
{
    bool matched = false;
    for (cJSON *item = object->child; item != NULL;) {
        cJSON *next = item->next;
        int index = context ? mainmenu_context_key_index(item->string)
                            : mainmenu_menu_key_index(item->string);
        if (index == target_index) {
            cJSON *value = cJSON_CreateBool(enabled);
            if (value == NULL)
                return false;

            char *key = item->string;
            item->string = NULL;
            value->string = key;
            if (!cJSON_ReplaceItemViaPointer(object, item, value)) {
                value->string = NULL;
                item->string = key;
                cJSON_Delete(value);
                return false;
            }
            matched = true;
        }
        item = next;
    }

    if (matched)
        return true;
    const char *key = context ? mainmenu_context_keys[target_index]
                              : mainmenu_menu_keys[target_index];
    return cJSON_AddBoolToObject(object, key, enabled) != NULL;
}

static bool mainmenu_set_array_context(cJSON *array, int target_index,
                                       bool enabled)
{
    bool found = false;
    for (int i = cJSON_GetArraySize(array) - 1; i >= 0; i--) {
        cJSON *item = cJSON_GetArrayItem(array, i);
        if (!cJSON_IsString(item) ||
            mainmenu_context_key_index(item->valuestring) != target_index)
            continue;
        found = true;
        if (!enabled)
            cJSON_DeleteItemFromArray(array, i);
    }
    if (enabled && !found) {
        cJSON *value = cJSON_CreateString(mainmenu_context_keys[target_index]);
        if (value == NULL)
            return false;
        if (!cJSON_AddItemToArray(array, value)) {
            cJSON_Delete(value);
            return false;
        }
    }
    return true;
}

static bool mainmenu_add_default_sections(cJSON *root)
{
    bool menu_states[MAINMENU_MENU_COUNT];
    bool context_states[MAINMENU_CONTEXT_COUNT];
    mainmenu_set_default_menu(menu_states);
    mainmenu_set_default_context(context_states);

    cJSON *menu = cJSON_CreateObject();
    cJSON *context = cJSON_CreateObject();
    if (menu == NULL || context == NULL) {
        cJSON_Delete(menu);
        cJSON_Delete(context);
        return false;
    }

    for (int i = 0; i < MAINMENU_MENU_COUNT; i++) {
        if (cJSON_AddBoolToObject(menu, mainmenu_menu_keys[i], menu_states[i]) == NULL) {
            cJSON_Delete(menu);
            cJSON_Delete(context);
            return false;
        }
    }
    for (int i = 0; i < MAINMENU_CONTEXT_COUNT; i++) {
        if (!context_states[i])
            continue;
        if (cJSON_AddBoolToObject(context, mainmenu_context_keys[i], true) == NULL) {
            cJSON_Delete(menu);
            cJSON_Delete(context);
            return false;
        }
    }

    if (cJSON_AddBoolToObject(root, "hotkey", true) == NULL) {
        cJSON_Delete(menu);
        cJSON_Delete(context);
        return false;
    }
    if (!cJSON_AddItemToObject(root, "menu", menu)) {
        cJSON_Delete(menu);
        cJSON_Delete(context);
        return false;
    }
    if (!cJSON_AddItemToObject(root, "context", context)) {
        cJSON_Delete(context);
        return false;
    }
    return true;
}

static bool mainmenu_store_menu_item(int target_index, bool enabled)
{
    bool exists = false;
    bool valid = false;
    cJSON *root = mainmenu_load_root(&exists, &valid);
    if (!valid || root == NULL)
        return false;

    if (!exists && !mainmenu_add_default_sections(root)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *menu = cJSON_GetObjectItemCaseSensitive(root, "menu");
    bool states[MAINMENU_MENU_COUNT];
    mainmenu_read_menu_states(root, states);
    if (!cJSON_IsObject(menu)) {
        cJSON *replacement = cJSON_CreateObject();
        if (replacement == NULL) {
            cJSON_Delete(root);
            return false;
        }
        for (int i = 0; i < MAINMENU_MENU_COUNT; i++) {
            if (cJSON_AddBoolToObject(replacement, mainmenu_menu_keys[i], states[i]) == NULL) {
                cJSON_Delete(replacement);
                cJSON_Delete(root);
                return false;
            }
        }
        if (!mainmenu_replace_root_section(root, "menu", replacement)) {
            cJSON_Delete(replacement);
            cJSON_Delete(root);
            return false;
        }
        menu = cJSON_GetObjectItemCaseSensitive(root, "menu");
    }
    else if (!mainmenu_menu_object_has_enabled(menu)) {
        for (int i = 0; i < MAINMENU_MENU_COUNT; i++) {
            if (!mainmenu_set_object_alias(menu, i, states[i], false)) {
                cJSON_Delete(root);
                return false;
            }
        }
    }

    bool ok = mainmenu_set_object_alias(menu, target_index, enabled, false) &&
              mainmenu_write_root(root);
    cJSON_Delete(root);
    return ok;
}

static bool mainmenu_store_context_item(int target_index, bool enabled)
{
    bool exists = false;
    bool valid = false;
    cJSON *root = mainmenu_load_root(&exists, &valid);
    if (!valid || root == NULL)
        return false;

    if (!exists && !mainmenu_add_default_sections(root)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *context = cJSON_GetObjectItemCaseSensitive(root, "context");
    bool ok = false;
    if (cJSON_IsObject(context)) {
        ok = mainmenu_set_object_alias(context, target_index, enabled, true);
    }
    else if (cJSON_IsArray(context)) {
        ok = mainmenu_set_array_context(context, target_index, enabled);
    }
    else {
        bool states[MAINMENU_CONTEXT_COUNT];
        mainmenu_read_context_states(root, states);
        states[target_index] = enabled;

        cJSON *replacement = cJSON_CreateObject();
        if (replacement != NULL) {
            ok = true;
            for (int i = 0; i < MAINMENU_CONTEXT_COUNT; i++) {
                if (!states[i])
                    continue;
                if (cJSON_AddBoolToObject(replacement,
                                          mainmenu_context_keys[i], true) == NULL) {
                    ok = false;
                    break;
                }
            }
            if (ok)
                ok = mainmenu_replace_root_section(root, "context", replacement);
        }
        if (!ok)
            cJSON_Delete(replacement);
    }

    if (ok)
        ok = mainmenu_write_root(root);
    cJSON_Delete(root);
    return ok;
}

static bool mainmenu_store_hotkey(bool enabled)
{
    bool exists = false;
    bool valid = false;
    cJSON *root = mainmenu_load_root(&exists, &valid);
    if (!valid || root == NULL)
        return false;

    if (!exists && !mainmenu_add_default_sections(root)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *value = cJSON_CreateBool(enabled);
    if (value == NULL) {
        cJSON_Delete(root);
        return false;
    }

    cJSON *hotkey = cJSON_GetObjectItemCaseSensitive(root, "hotkey");
    bool ok = hotkey != NULL
                  ? cJSON_ReplaceItemInObjectCaseSensitive(root, "hotkey", value)
                  : cJSON_AddItemToObject(root, "hotkey", value);
    if (!ok)
        cJSON_Delete(value);
    else
        ok = mainmenu_write_root(root);

    cJSON_Delete(root);
    return ok;
}

static void mainmenu_action_hotkey(void *pt)
{
    ListItem *item = (ListItem *)pt;
    if (!mainmenu_store_hotkey(item->value != 0))
        item->value = mainmenu_load_hotkey_state();
}

static void mainmenu_action_menu_item(void *pt)
{
    ListItem *item = (ListItem *)pt;
    int index = item->action_id;
    if (index < 0 || index >= MAINMENU_MENU_COUNT)
        return;
    if (!mainmenu_store_menu_item(index, item->value != 0)) {
        bool menu_states[MAINMENU_MENU_COUNT];
        bool context_states[MAINMENU_CONTEXT_COUNT];
        mainmenu_load_states(menu_states, context_states);
        item->value = menu_states[index];
    }
}

static void mainmenu_action_context_item(void *pt)
{
    ListItem *item = (ListItem *)pt;
    int index = item->action_id;
    if (index < 0 || index >= MAINMENU_CONTEXT_COUNT)
        return;
    if (!mainmenu_store_context_item(index, item->value != 0)) {
        bool menu_states[MAINMENU_MENU_COUNT];
        bool context_states[MAINMENU_CONTEXT_COUNT];
        mainmenu_load_states(menu_states, context_states);
        item->value = context_states[index];
    }
}

void menu_mainMenuItems(void *_)
{
    (void)_;
    if (!_menu_main_menu_items._created) {
        bool menu_states[MAINMENU_MENU_COUNT];
        bool context_states[MAINMENU_CONTEXT_COUNT];
        mainmenu_load_states(menu_states, context_states);
        _menu_main_menu_items = list_createWithTitle(
            MAINMENU_MENU_COUNT, LIST_SMALL, "Main menu items");
        for (int i = 0; i < MAINMENU_MENU_COUNT; i++) {
            list_addItem(&_menu_main_menu_items,
                         (ListItem){
                             .label = "",
                             .item_type = TOGGLE,
                             .value = menu_states[i],
                             .action_id = i,
                             .action = mainmenu_action_menu_item});
            strcpy(_menu_main_menu_items.items[i].label, mainmenu_menu_labels[i]);
        }
    }
    menu_stack[++menu_level] = &_menu_main_menu_items;
    header_changed = true;
}

void menu_mainContextItems(void *_)
{
    (void)_;
    if (!_menu_main_context_items._created) {
        bool menu_states[MAINMENU_MENU_COUNT];
        bool context_states[MAINMENU_CONTEXT_COUNT];
        mainmenu_load_states(menu_states, context_states);
        _menu_main_context_items = list_createWithTitle(
            MAINMENU_CONTEXT_LIST_COUNT, LIST_SMALL, "Context menu items");
        for (int i = 0; i < MAINMENU_CONTEXT_COUNT; i++) {
            list_addItem(&_menu_main_context_items,
                         (ListItem){
                             .label = "",
                             .item_type = TOGGLE,
                             .value = context_states[i],
                             .action_id = i,
                             .action = mainmenu_action_context_item});
            strcpy(_menu_main_context_items.items[i].label,
                   mainmenu_context_labels[i]);
        }
        list_addItemWithInfoNote(
            &_menu_main_context_items,
            (ListItem){
                .label = "Activate hotkey",
                .item_type = TOGGLE,
                .value = mainmenu_load_hotkey_state(),
                .action = mainmenu_action_hotkey},
            "Enable the MainUI hotkey that opens the\n"
            "context menu. This root setting is not a\n"
            "context-menu item. Default: on.");
    }
    menu_stack[++menu_level] = &_menu_main_context_items;
    header_changed = true;
}

void menu_mainMenu(void *_)
{
    (void)_;
    if (!_menu_main_menu._created) {
        _menu_main_menu = list_createWithTitle(2, LIST_SMALL, "Main menu");
        list_addItemWithInfoNote(
            &_menu_main_menu,
            (ListItem){.label = "Main menu items...", .action = menu_mainMenuItems},
            "Choose which top-level MainUI sections\n"
            "are visible. Restart MainUI after changes.");
        list_addItemWithInfoNote(
            &_menu_main_menu,
            (ListItem){.label = "Context menu items...", .action = menu_mainContextItems},
            "Choose which SELECT-menu entries are visible\n"
            "and whether its activation hotkey is enabled.\n"
            "Existing custom definitions are preserved.");
    }
    menu_stack[++menu_level] = &_menu_main_menu;
    header_changed = true;
}

#endif // TWEAKS_MAIN_MENU_H__
