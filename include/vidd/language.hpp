#ifndef __VIDD_LANGUAGE_HPP__
#define __VIDD_LANGUAGE_HPP__

#include <vidd/syntax.hpp>

#include <vector>
#include <string_view>

struct Language {
	std::string_view name;
	std::vector<std::string_view> extensions;
	Syntax syntax;
};

const Language* getLanguageByName(std::string_view name);
const Language* getLanguageFromFileName(std::string_view name);
std::vector<const Language*> getAllLanguages(void);

#endif
