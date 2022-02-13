#include <vidd/syntax.h>

#include <vidd/languages/c>
#include <vidd/languages/cpp>
#include <vidd/languages/python>
#include <vidd/languages/javascript>
#include <vidd/languages/java>
#include <vidd/languages/css>
#include <vidd/languages/html>
#include <vidd/languages/sast>
#include <vidd/languages/clature>
#include <vidd/languages/rust>
#include <vidd/languages/go>

char** syntaxes[] = {
	SYNTAXES_ADD(syntax_c)
	SYNTAXES_ADD(syntax_cpp)
	SYNTAXES_ADD(syntax_py)
	SYNTAXES_ADD(syntax_js)
	SYNTAXES_ADD(syntax_java)
	SYNTAXES_ADD(syntax_css)
	SYNTAXES_ADD(syntax_html)
	SYNTAXES_ADD(syntax_sast)
	SYNTAXES_ADD(syntax_cl)
	SYNTAXES_ADD(syntax_rs)
	SYNTAXES_ADD(syntax_go)
};
int syntaxes_length = sizeof(syntaxes) / sizeof(syntaxes[0]);
