#ifndef __VIDD_SYNTAX_H__
#define __VIDD_SYNTAX_H__

#include "vidd.h"
#include "line.h"
#include "buffer.h"

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

#define SYNTAX_DEFINE(name) \
	extern char* name[]; extern int name##_length;

#define SYNTAXES_ADD(syntax) \
	(char**)syntax,

#define SYNTAX_NEW(name) \
	char* name[] = {

#define SYNTAX_EXT_COUNT(count) \
	(char*)count,

#define SYNTAX_ADD_EXT(ext) \
	ext,

#define SYNTAX_ADD(name) \
	name,

#define SYNTAX_ADD_RANGE(start, end) \
	SSID_RANGE start "\0" end,

#define SYNTAX_ADD_RANGE_NI(start, end) \
	SSID_RANGE_NI start "\0" end,

#define SYNTAX_END(name) \
	0 }; int name##_length = sizeof(name) / sizeof(name[0]);

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line);
void vidd_syntax_ftdetect(struct vidd_client* client);
void vidd_syntax_ftdetect_cstring(struct vidd_client* client, char* rfile_name);

SYNTAX_DEFINE(syntax_c);

extern char** syntaxes[];
extern int syntaxes_length;

#endif