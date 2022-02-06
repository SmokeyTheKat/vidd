#ifndef __VIDD_CONFIG_H__
#define __VIDD_CONFIG_H__

#include "vidd.h"

#define KEY_CTRL(c) ((int)(c-96))
#define DEFAULT_FILE PREFIX "/share/vidd/default"

extern struct vidd_keybind vidd_normal_mode_keybinds[];
extern int vidd_normal_mode_keybinds_length;

extern struct vidd_keybind vidd_window_move_mode_keybinds[];
extern int vidd_window_move_mode_keybinds_length;

extern struct vidd_keybind vidd_select_mode_keybinds[];
extern int vidd_select_mode_keybinds_length;

extern struct vidd_keybind vidd_insert_mode_keybinds[];
extern int vidd_insert_mode_keybinds_length;

extern struct command vidd_commands[];
extern int vidd_commands_length;

extern void(*vidd_editor_keybinds[])(struct vidd_client*);
extern int vidd_editor_keybinds_length;

#endif