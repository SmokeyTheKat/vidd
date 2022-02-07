#ifndef __VIDD_STYLE_H__
#define __VIDD_STYLE_H__

#define FRGB(r, g, b) "\x1b[38;2;" r ";" g ";" b "m"
#define BRGB(r, g, b) "\x1b[48;2;" r ";" g ";" b "m"
#define FGSC(s) "\x1b[38;2;" s ";" s ";" s  "m"
#define BGSC(s) "\x1b[48;2;" s ";" s ";" s  "m"

#define NOSTYLE "\x1b[0m"
#define STYLE_UNDERLINE "\x1b[4m"
#define STYLE_REVERSE "\x1b[7m"
#define STYLE_ITALIC "\x1b[3m"
#define STYLE_BOLD "\x1b[1m"

#define FG_BLACK "\x1b[38;2;0;0;0m"
#define FG_WHITE "\x1b[38;2;255;255;255m"
#define FG_RED "\x1b[38;2;255;0;0m"
#define FG_GREEN "\x1b[38;2;0;255;0m"
#define FG_BLUE "\x1b[38;2;0;0;255m"
#define FG_YELLOW "\x1b[38;2;255;255;0m"
#define FG_MAGENTA "\x1b[38;2;255;0;255m"
#define FG_CYAN "\x1b[38;2;0;255;255m"
#define FG_LIGHT_CYAN "\x1b[38;2;55;255;255m"
#define FG_DARK_CYAN "\x1b[38;2;0;160;160m"

#define FG_ORANGE "\x1b[38;2;255;165;0m"
#define FG_DARK_ORANGE "\x1b[38;2;200;110;00m"
#define FG_LIGHT_ORANGE "\x1b[38;2;255;175;20m"

#define BG_BLACK "\x1b[48;2;0;0;0m"
#define BG_WHITE "\x1b[48;2;255;255;255m"
#define BG_RED "\x1b[48;2;255;0;0m"
#define BG_GREEN "\x1b[48;2;0;255;0m"
#define BG_BLUE "\x1b[48;2;0;0;255m"
#define BG_YELLOW "\x1b[48;2;255;255;0m"
#define BG_MAGENTA "\x1b[48;2;255;0;255m"
#define BG_CYAN "\x1b[48;2;0;255;255m"
#define BG_ORANGE "\x1b[48;2;255;165;0m"
#define BG_LIGHT_ORANGE "\x1b[48;2;255;175;20m"

#endif