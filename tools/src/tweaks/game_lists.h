#ifndef TWEAKS_GAME_LISTS_H__
#define TWEAKS_GAME_LISTS_H__

// SPDX-License-Identifier: GPL-3.0-only
//
// User-friendly Onion Tweaks controls for the runtime configuration files
// consumed by the optional MainUI game-list patches.

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "system/settings.h"
#include "utils/config.h"

#include "components/list.h"

#define GAMELISTS_CONFIG_DIR "/mnt/SDCARD/.tmp_update/config"
#define GAMELISTS_ROWS_PATH GAMELISTS_CONFIG_DIR "/.romListRows"
#define GAMELISTS_FONT_PATH GAMELISTS_CONFIG_DIR "/.romListFontSize"
#define GAMELISTS_SCROLL_PATH GAMELISTS_CONFIG_DIR "/.romListTitleScroll"
#define GAMELISTS_REPEAT_PATH GAMELISTS_CONFIG_DIR "/.mainUIKeyRepeat"
#define GAMELISTS_SORT_PATH GAMELISTS_CONFIG_DIR "/.romListCaseSensitiveSort"
#define GAMELISTS_DYNAMIC_FAV_PATH GAMELISTS_CONFIG_DIR "/.romListDynamicFavPos"
#define GAMELISTS_ROMWINIDX_PATH "/appconfigs/romwinidx.json"
#ifndef GAMELISTS_THEME_RESCALE_SCRIPT
#define GAMELISTS_THEME_RESCALE_SCRIPT "/mnt/SDCARD/.tmp_update/script/rescale_theme_list_icons.sh"
#endif
#ifndef GAMELISTS_CACHE_CLEAR_COMMAND
#define GAMELISTS_CACHE_CLEAR_COMMAND \
    "rm -f /mnt/SDCARD/Roms/*/*_cache* " \
    "/mnt/SDCARD/Roms/PORTS/Shortcuts/Shortcuts_cache6.db"
#endif
#define GAMELISTS_PREF_REBUILD_CACHE "gameLists/rebuildCacheAfterCaseChange"
#define GAMELISTS_PREF_RESCALE_ICONS "gameLists/rescaleThemeListIcons"

static bool gamelists_rescale_pending = false;


static const int gamelists_scroll_delays[] = {
    -1, 0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1250,
    1500, 1750, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 15000,
    20000, 30000};

static const int gamelists_repeat_intervals[] = {
    30, 40, 50, 60, 70, 80, 90, 100, 120, 150, 175, 200, 250, 300,
    400, 500};

#define GAMELISTS_ARRAY_COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))

static int gamelists_clamp(int value, int minimum, int maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

static bool gamelists_parse_int(const char *text, int *value_out)
{
    if (text == NULL || value_out == NULL)
        return false;

    while (isspace((unsigned char)*text))
        text++;
    if (*text == '\0')
        return false;

    errno = 0;
    char *end = NULL;
    long parsed = strtol(text, &end, 10);
    if (errno != 0 || end == text || parsed < INT_MIN || parsed > INT_MAX)
        return false;

    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0')
        return false;

    *value_out = (int)parsed;
    return true;
}

static bool gamelists_read_int(const char *path, int *value_out)
{
    char buffer[64];
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return false;

    bool ok = fgets(buffer, sizeof(buffer), fp) != NULL;
    if (fclose(fp) != 0)
        ok = false;
    if (!ok)
        return false;

    return gamelists_parse_int(buffer, value_out);
}

static bool gamelists_read_pair(const char *path, int *first_out, int *second_out)
{
    char buffer[96];
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        return false;

    bool ok = fgets(buffer, sizeof(buffer), fp) != NULL;
    if (fclose(fp) != 0)
        ok = false;
    if (!ok)
        return false;

    char *cursor = buffer;
    while (isspace((unsigned char)*cursor))
        cursor++;

    errno = 0;
    char *end = NULL;
    long first = strtol(cursor, &end, 10);
    if (errno != 0 || end == cursor || first < INT_MIN || first > INT_MAX)
        return false;

    cursor = end;
    while (isspace((unsigned char)*cursor))
        cursor++;
    if (*cursor == ',') {
        cursor++;
        while (isspace((unsigned char)*cursor))
            cursor++;
    }
    else if (cursor == end) {
        return false;
    }

    errno = 0;
    long second = strtol(cursor, &end, 10);
    if (errno != 0 || end == cursor || second < INT_MIN || second > INT_MAX)
        return false;

    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0')
        return false;

    *first_out = (int)first;
    *second_out = (int)second;
    return true;
}

static bool gamelists_ensure_config_dir(void)
{
    if (mkdir(GAMELISTS_CONFIG_DIR, 0755) == 0)
        return true;
    if (errno != EEXIST)
        return false;

    struct stat st;
    return stat(GAMELISTS_CONFIG_DIR, &st) == 0 && S_ISDIR(st.st_mode);
}

static bool gamelists_write_text(const char *path, const char *text)
{
    if (!gamelists_ensure_config_dir())
        return false;

    char temporary[256];
    if (snprintf(temporary, sizeof(temporary), "%s.tmp", path) >= (int)sizeof(temporary))
        return false;

    FILE *fp = fopen(temporary, "w");
    if (fp == NULL)
        return false;

    size_t length = strlen(text);
    bool ok = fwrite(text, 1, length, fp) == length;
    if (ok)
        ok = fflush(fp) == 0;
    if (ok)
        ok = fsync(fileno(fp)) == 0;
    if (fclose(fp) != 0)
        ok = false;

    if (!ok) {
        remove(temporary);
        return false;
    }

    if (rename(temporary, path) != 0) {
        remove(temporary);
        return false;
    }
    return true;
}

static bool gamelists_write_int(const char *path, int value)
{
    char buffer[48];
    snprintf(buffer, sizeof(buffer), "%d\n", value);
    return gamelists_write_text(path, buffer);
}

static bool gamelists_write_pair(const char *path, int first, int second)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d,%d\n", first, second);
    return gamelists_write_text(path, buffer);
}

static int gamelists_nearest_index(const int *values, int count, int target)
{
    int best_index = 0;
    long best_distance = labs((long)target - values[0]);
    for (int i = 1; i < count; i++) {
        long distance = labs((long)target - values[i]);
        if (distance < best_distance) {
            best_index = i;
            best_distance = distance;
        }
    }
    return best_index;
}

static void gamelists_load_scroll(int *delay_out, int *speed_out)
{
    int delay = -1;
    int speed = 120;
    if (gamelists_read_pair(GAMELISTS_SCROLL_PATH, &delay, &speed)) {
        if (delay < 0 || speed <= 0) {
            delay = -1;
            speed = gamelists_clamp(speed > 0 ? speed : 120, 5, 400);
        }
        else {
            delay = gamelists_clamp(delay, 0, 30000);
            speed = gamelists_clamp(speed, 5, 400);
        }
    }
    *delay_out = delay;
    *speed_out = speed;
}

static void gamelists_store_scroll(int delay, int speed)
{
    speed = gamelists_clamp(speed, 5, 400);
    if (delay < 0 && speed == 120) {
        remove(GAMELISTS_SCROLL_PATH);
        return;
    }
    gamelists_write_pair(GAMELISTS_SCROLL_PATH, delay, speed);
}

static void gamelists_load_repeat(int *delay_out, int *interval_out)
{
    int delay = 500;
    int interval = 100;
    if (!gamelists_read_pair(GAMELISTS_REPEAT_PATH, &delay, &interval) ||
        delay <= 0 || interval <= 0) {
        delay = 500;
        interval = 100;
    }
    *delay_out = gamelists_clamp(delay, 100, 2000);
    *interval_out = gamelists_clamp(interval, 30, 500);
}

static void gamelists_store_repeat(int delay, int interval)
{
    delay = gamelists_clamp(delay, 100, 2000);
    interval = gamelists_clamp(interval, 30, 500);
    if (delay == 500 && interval == 100) {
        remove(GAMELISTS_REPEAT_PATH);
        return;
    }
    gamelists_write_pair(GAMELISTS_REPEAT_PATH, delay, interval);
}

static int gamelists_value_rows(void)
{
    int rows = 6;
    if (!gamelists_read_int(GAMELISTS_ROWS_PATH, &rows))
        rows = 6;
    return gamelists_clamp(rows, 6, 20) - 6;
}

static int gamelists_value_font(void)
{
    int size = 0;
    if (!gamelists_read_int(GAMELISTS_FONT_PATH, &size) || size <= 0)
        return 0;
    return gamelists_clamp(size, 8, 60) - 7;
}

static int gamelists_value_scroll_delay(void)
{
    int delay, speed;
    gamelists_load_scroll(&delay, &speed);
    (void)speed;
    return gamelists_nearest_index(
        gamelists_scroll_delays,
        GAMELISTS_ARRAY_COUNT(gamelists_scroll_delays),
        delay);
}

static int gamelists_value_scroll_speed(void)
{
    int delay, speed;
    gamelists_load_scroll(&delay, &speed);
    (void)delay;
    return (gamelists_clamp(speed, 5, 400) - 5 + 2) / 5;
}

static int gamelists_value_repeat_delay(void)
{
    int delay, interval;
    gamelists_load_repeat(&delay, &interval);
    (void)interval;
    return (delay - 100 + 25) / 50;
}

static int gamelists_value_repeat_interval(void)
{
    int delay, interval;
    gamelists_load_repeat(&delay, &interval);
    (void)delay;
    return gamelists_nearest_index(
        gamelists_repeat_intervals,
        GAMELISTS_ARRAY_COUNT(gamelists_repeat_intervals),
        interval);
}

static int gamelists_value_sort(void)
{
    return access(GAMELISTS_SORT_PATH, F_OK) == 0;
}

static int gamelists_value_fixed_favorite_position(void)
{
    return access(GAMELISTS_DYNAMIC_FAV_PATH, F_OK) != 0;
}

static int gamelists_value_default_enabled(const char *key)
{
    int value = 1;
    if (!config_get(key, CONFIG_INT, &value))
        return 1;
    return value != 0;
}

static int gamelists_value_rebuild_cache(void)
{
    return gamelists_value_default_enabled(GAMELISTS_PREF_REBUILD_CACHE);
}

static int gamelists_value_rescale_icons(void)
{
    return gamelists_value_default_enabled(GAMELISTS_PREF_RESCALE_ICONS);
}

static bool gamelists_remove_file(const char *path)
{
    if (remove(path) == 0)
        return true;
    return errno == ENOENT;
}

static int gamelists_row_height_for_rows(int rows)
{
    rows = gamelists_clamp(rows, 6, 20);
    return 360 / rows;
}

static bool gamelists_run_theme_rescale(int rows)
{
    if (settings.theme[0] == '\0')
        return false;

    char height[16];
    snprintf(height, sizeof(height), "%d", gamelists_row_height_for_rows(rows));

    pid_t pid = fork();
    if (pid < 0)
        return false;
    if (pid == 0) {
        char row_count[16];
        snprintf(row_count, sizeof(row_count), "%d", rows);
        execl("/bin/sh", "sh", GAMELISTS_THEME_RESCALE_SCRIPT,
              settings.theme, height, row_count, (char *)NULL);
        _exit(127);
    }

    int status = 0;
    while (waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR)
            return false;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

static void gamelists_clear_rom_caches(void)
{
    system(GAMELISTS_CACHE_CLEAR_COMMAND);
}

static void gamelists_formatter_rows(void *pt, char *out_label)
{
    int rows = ((ListItem *)pt)->value + 6;
    if (rows == 6)
        strcpy(out_label, "6 (stock)");
    else
        snprintf(out_label, STR_MAX, "%d rows", rows);
}

static void gamelists_formatter_font(void *pt, char *out_label)
{
    int value = ((ListItem *)pt)->value;
    if (value == 0)
        strcpy(out_label, "Theme default");
    else
        snprintf(out_label, STR_MAX, "%d px", value + 7);
}

static void gamelists_formatter_scroll_delay(void *pt, char *out_label)
{
    int index = ((ListItem *)pt)->value;
    int delay = gamelists_scroll_delays[index];
    if (delay < 0)
        strcpy(out_label, "Off");
    else if (delay == 0)
        strcpy(out_label, "Immediate");
    else
        snprintf(out_label, STR_MAX, "%d ms", delay);
}

static void gamelists_formatter_scroll_speed(void *pt, char *out_label)
{
    int speed = 5 + ((ListItem *)pt)->value * 5;
    snprintf(out_label, STR_MAX, "%d px/s", speed);
}

static void gamelists_formatter_repeat_delay(void *pt, char *out_label)
{
    int delay = 100 + ((ListItem *)pt)->value * 50;
    if (delay == 500)
        snprintf(out_label, STR_MAX, "%d ms (stock)", delay);
    else
        snprintf(out_label, STR_MAX, "%d ms", delay);
}

static void gamelists_formatter_repeat_interval(void *pt, char *out_label)
{
    int interval = gamelists_repeat_intervals[((ListItem *)pt)->value];
    if (interval == 100)
        snprintf(out_label, STR_MAX, "%d ms (stock)", interval);
    else
        snprintf(out_label, STR_MAX, "%d ms", interval);
}

static void gamelists_action_rows(void *pt)
{
    int rows = ((ListItem *)pt)->value + 6;
    int old_rows = gamelists_value_rows() + 6;
    if (rows == old_rows)
        return;

    bool stored;
    if (rows == 6)
        stored = gamelists_remove_file(GAMELISTS_ROWS_PATH);
    else
        stored = gamelists_write_int(GAMELISTS_ROWS_PATH, rows);

    if (stored) {
        gamelists_remove_file(GAMELISTS_ROMWINIDX_PATH);
        gamelists_rescale_pending = true;
    }
}

static void gamelists_action_rescale_icons(void *pt)
{
    int enabled = ((ListItem *)pt)->value != 0;
    config_setNumber(GAMELISTS_PREF_RESCALE_ICONS, enabled);
    if (enabled)
        gamelists_rescale_pending = true;
}

static void gamelists_action_font(void *pt)
{
    int value = ((ListItem *)pt)->value;
    if (value == 0)
        remove(GAMELISTS_FONT_PATH);
    else
        gamelists_write_int(GAMELISTS_FONT_PATH, value + 7);
}

static void gamelists_action_scroll_delay(void *pt)
{
    int delay, speed;
    gamelists_load_scroll(&delay, &speed);
    delay = gamelists_scroll_delays[((ListItem *)pt)->value];
    gamelists_store_scroll(delay, speed);
}

static void gamelists_action_scroll_speed(void *pt)
{
    int delay, speed;
    gamelists_load_scroll(&delay, &speed);
    speed = 5 + ((ListItem *)pt)->value * 5;
    gamelists_store_scroll(delay, speed);
}

static void gamelists_action_repeat_delay(void *pt)
{
    int delay, interval;
    gamelists_load_repeat(&delay, &interval);
    delay = 100 + ((ListItem *)pt)->value * 50;
    gamelists_store_repeat(delay, interval);
}

static void gamelists_action_repeat_interval(void *pt)
{
    int delay, interval;
    gamelists_load_repeat(&delay, &interval);
    interval = gamelists_repeat_intervals[((ListItem *)pt)->value];
    gamelists_store_repeat(delay, interval);
}

static void gamelists_action_sort(void *pt)
{
    int value = ((ListItem *)pt)->value != 0;
    int old_value = gamelists_value_sort();
    if (value == old_value)
        return;

    bool stored;
    if (value == 0)
        stored = gamelists_remove_file(GAMELISTS_SORT_PATH);
    else
        stored = gamelists_write_text(GAMELISTS_SORT_PATH, "");

    if (stored && gamelists_value_rebuild_cache())
        gamelists_clear_rom_caches();
}

static void gamelists_action_rebuild_cache(void *pt)
{
    int enabled = ((ListItem *)pt)->value != 0;
    config_setNumber(GAMELISTS_PREF_REBUILD_CACHE, enabled);
}

static void gamelists_action_fixed_favorite_position(void *pt)
{
    int enabled = ((ListItem *)pt)->value != 0;
    if (enabled)
        gamelists_remove_file(GAMELISTS_DYNAMIC_FAV_PATH);
    else
        gamelists_write_text(GAMELISTS_DYNAMIC_FAV_PATH, "");
}

static void gamelists_on_menu_exit(void)
{
    if (!gamelists_rescale_pending)
        return;

    if (!gamelists_value_rescale_icons()) {
        gamelists_rescale_pending = false;
        return;
    }

    if (gamelists_run_theme_rescale(gamelists_value_rows() + 6))
        gamelists_rescale_pending = false;
}

void menu_gameLists(void *_)
{
    (void)_;
    if (!_menu_game_lists._created) {
        _menu_game_lists = list_createWithTitle(10, LIST_SMALL, "Game lists");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Rows",
                .item_type = MULTIVALUE,
                .value_max = 14,
                .value = gamelists_value_rows(),
                .value_formatter = gamelists_formatter_rows,
                .action = gamelists_action_rows},
            "Number of visible rows in ROM, Favorites,\n"
            "Recent and Search game lists.\n"
            "Requires the patched MainUI row feature.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Rescale theme list icons to fit",
                .item_type = TOGGLE,
                .value = gamelists_value_rescale_icons(),
                .action = gamelists_action_rescale_icons},
            "After changing Rows, resize when leaving this\n"
            "menu. Three icons use one-time .bak originals;\n"
            "the row background is generated separately.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Font override",
                .item_type = MULTIVALUE,
                .value_max = 53,
                .value = gamelists_value_font(),
                .value_formatter = gamelists_formatter_font,
                .action = gamelists_action_font},
            "Override the theme font size only for game\n"
            "lists. Theme default removes the override.\n"
            "Valid patched-MainUI range: 8-60 px.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Title scroll delay",
                .item_type = MULTIVALUE,
                .value_max = GAMELISTS_ARRAY_COUNT(gamelists_scroll_delays) - 1,
                .value = gamelists_value_scroll_delay(),
                .value_formatter = gamelists_formatter_scroll_delay,
                .action = gamelists_action_scroll_delay},
            "Delay before a selected long title starts\n"
            "scrolling. Off disables title scrolling.\n"
            "Recommended starting point: 700 ms.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Title scroll speed",
                .item_type = MULTIVALUE,
                .value_max = 79,
                .value = gamelists_value_scroll_speed(),
                .value_formatter = gamelists_formatter_scroll_speed,
                .action = gamelists_action_scroll_speed},
            "Horizontal title movement in pixels per\n"
            "second. Patched-MainUI range: 5-400.\n"
            "Recommended starting point: 120 px/s.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Repeat delay",
                .item_type = MULTIVALUE,
                .value_max = 38,
                .value = gamelists_value_repeat_delay(),
                .value_formatter = gamelists_formatter_repeat_delay,
                .action = gamelists_action_repeat_delay},
            "Initial delay before a held key repeats.\n"
            "Stock is 500 ms; 300 ms is a useful\n"
            "quicker starting point.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Repeat interval",
                .item_type = MULTIVALUE,
                .value_max = GAMELISTS_ARRAY_COUNT(gamelists_repeat_intervals) - 1,
                .value = gamelists_value_repeat_interval(),
                .value_formatter = gamelists_formatter_repeat_interval,
                .action = gamelists_action_repeat_interval},
            "Time between repeats while a key remains\n"
            "held. Stock is 100 ms; 50 ms is a useful\n"
            "quicker starting point.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Sorting",
                .item_type = MULTIVALUE,
                .value_max = 1,
                .value_labels = {"Case-insensitive", "Case-sensitive"},
                .value = gamelists_value_sort(),
                .action = gamelists_action_sort},
            "Case-insensitive is the patched default.\n"
            "Restart MainUI after changing this option.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Rebuild caches after case change",
                .item_type = TOGGLE,
                .value = gamelists_value_rebuild_cache(),
                .action = gamelists_action_rebuild_cache},
            "Delete ROM cache databases automatically when\n"
            "the Sorting option changes, so MainUI rebuilds\n"
            "them with matching sort indexes. Default: on.");

        list_addItemWithInfoNote(
            &_menu_game_lists,
            (ListItem){
                .label = "Fixed favorite position",
                .item_type = TOGGLE,
                .value = gamelists_value_fixed_favorite_position(),
                .action = gamelists_action_fixed_favorite_position},
            "Keep the favorite icon in a fixed position.\n"
            "Turn off to let it follow the title. Default: on.");
    }

    menu_stack[++menu_level] = &_menu_game_lists;
    header_changed = true;
}

#endif // TWEAKS_GAME_LISTS_H__
