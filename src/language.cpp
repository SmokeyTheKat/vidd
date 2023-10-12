#include <vidd/language.hpp>

namespace {

const Language none {
	"",
	{},
	{
		false,
		false,
		false,
		false,
		false,
		{},
		{},
		{},
		{},
		{},
		{}
	}
};

#include <vidd/languages/cpp.hpp>
#include <vidd/languages/c.hpp>
#include <vidd/languages/zig.hpp>
#include <vidd/languages/yoke.hpp>
#include <vidd/languages/javascript.hpp>
#include <vidd/languages/html.hpp>
#include <vidd/languages/python.hpp>
#include <vidd/languages/escc.hpp>
#include <vidd/languages/makefile.hpp>
#include <vidd/languages/csharp.hpp>
#include <vidd/languages/ocaml.hpp>
#include <vidd/languages/funs.hpp>
const std::vector<const Language*> languages {
	&none,
	&cpp,
	&c,
	&zig,
	&yoke,
	&javascript,
	&html,
	&python,
	&escc,
	&makefile,
	&csharp,
	&ocaml,
	&funs,
};

}; // namespace

const Language* getLanguageByName(std::string_view name) {
	for (const auto* lang : languages) {
		if (lang->name == name) return lang;
	}
	return &none;
}

const Language* getLanguageFromFileName(std::string_view name) {
	for (const auto* lang : languages) {
		for (auto langExt : lang->extensions) {
			if (
				name.length() >= langExt.length() &&
				langExt == name.substr(name.length() - langExt.length())
			) return lang;
		}
	}
	return &none;
}

std::vector<const Language*> getAllLanguages(void) {
	return languages;
}
