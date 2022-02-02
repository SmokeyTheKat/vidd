#ifndef __VIDD_CONFIG_SYNTAX_H__
#define __VIDD_CONFIG_SYNTAX_H__

#define ACTIVE_CLIENT_COLOR BRGB("255", "255", "0") FRGB("0", "0", "0")
#define INACTIVE_CLIENT_COLOR BRGB("255", "255", "255") FRGB("0", "0", "0")

#define SSID_KEYWORD "#"
#define SSID_OPERATOR "+"
#define SSID_MACRO ")"
#define SSID_TYPE "%"
#define SSID_COMMENT "/"
#define SSID_CONST "*"
#define SSID_STRING "\""
#define SSID_CHARACTER_LITERAL "'"
#define SSID_RANGE "~"
#define SSID_RANGE_NI "`"
#define SSID_NUMBER "$"

#define SCID_KEYWORD '#'
#define SCID_OPERATOR '+'
#define SCID_MACRO ')'
#define SCID_TYPE '%'
#define SCID_COMMENT '/'
#define SCID_CONST '*'
#define SCID_STRING '"'
#define SCID_CHARACTER_LITERAL '\''
#define SCID_RANGE '~'
#define SCID_RANGE_NI '`'
#define SCID_NUMBER '$'

#define SYNTAX_KEYWORD_COLOR FRGB("255", "255", "0")
#define SYNTAX_OPERATOR_COLOR FRGB("255", "255", "0")
#define SYNTAX_MACRO_COLOR FRGB("0", "255", "255")
#define SYNTAX_TYPE_COLOR FRGB("255", "0", "0")
#define SYNTAX_STRING_COLOR FRGB("255", "0", "255")
#define SYNTAX_COMMENT_COLOR STYLE_ITALIC FRGB("0", "0", "255")
#define SYNTAX_CONST_COLOR STYLE_ITALIC FRGB("50", "100", "255")
#define SYNTAX_CHARACTER_LITERAL_COLOR FRGB("155", "100", "0")
#define SYNTAX_NUMBER_COLOR FRGB("0", "255", "0")

#define SYNTAX_DEFINE(name) \
	extern char* name[]; extern int name##_length;

SYNTAX_DEFINE(syntax_c);

extern char* syntax_colors[];
extern int syntax_colors_length;

extern char** syntaxes[];
extern int syntaxes_length;

#endif