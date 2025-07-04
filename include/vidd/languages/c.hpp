const Language c {
	"c",
	{ ".c", ".h" },
	{
		true,
/*hglt consts*/      true,
		true,
		false,
		true,
		{ "'", "\"" },
/*multi strings*/    {},
		{ { "/*", "*/" } },
		{ "//" },
		{ "+", "-", "*", "/", "=", "!", "~", "<", ">", "?", ":", "&", "%", "|", "^" },
		{
			{ { Syntax::Range::Type::UntilLineEnd, true, "#", "" }, StyleType::Macro },
		},
		{ 
			{ "if", StyleType::KeyWord },
			{ "break", StyleType::KeyWord },
			{ "else", StyleType::KeyWord },
			{ "for", StyleType::KeyWord },
			{ "continue", StyleType::KeyWord },
			{ "while", StyleType::KeyWord },
			{ "do", StyleType::KeyWord },
			{ "switch", StyleType::KeyWord },
			{ "case", StyleType::KeyWord },
			{ "default", StyleType::KeyWord },
			{ "inline", StyleType::KeyWord },
			{ "volatile", StyleType::KeyWord },
			{ "goto", StyleType::KeyWord },
			{ "sizeof", StyleType::KeyWord },
			{ "typeof", StyleType::KeyWord },
			{ "return", StyleType::KeyWord },
			{ "using", StyleType::KeyWord },
			{ "union", StyleType::Type },
			{ "struct", StyleType::Type },
			{ "typedef", StyleType::KeyWord },
			{ "enum", StyleType::Type },
			{ "register", StyleType::KeyWord },
			{ "auto", StyleType::Type },
			{ "inline", StyleType::KeyWord },
			{ "static", StyleType::KeyWord },
			{ "const", StyleType::KeyWord },
			{ "extern", StyleType::KeyWord },
			{ "void", StyleType::Type },
			{ "char", StyleType::Type },
			{ "byte", StyleType::Type },
			{ "bool", StyleType::Type },
			{ "short", StyleType::Type },
			{ "int", StyleType::Type },
			{ "long", StyleType::Type },
			{ "float", StyleType::Type },
			{ "double", StyleType::Type },
			{ "signed", StyleType::Type },
			{ "unsigned", StyleType::Type },
			{ "size_t", StyleType::Type },
			{ "wchar_t", StyleType::Type },
			{ "char8_t", StyleType::Type },
			{ "char16_t", StyleType::Type },
			{ "char32_t", StyleType::Type },
			{ "int8_t", StyleType::Type },
			{ "int16_t", StyleType::Type },
			{ "int32_t", StyleType::Type },
			{ "int64_t", StyleType::Type },
			{ "uint8_t", StyleType::Type },
			{ "uint16_t", StyleType::Type },
			{ "uint32_t", StyleType::Type },
			{ "uint64_t", StyleType::Type },
			{ "uintmax_t", StyleType::Type },
			{ "intmax_t", StyleType::Type },
			{ "FILE", StyleType::Type },
			{ "true", StyleType::Const },
			{ "false", StyleType::Const },
			{ "stdin", StyleType::Const },
			{ "stdout", StyleType::Const },
			{ "stderr", StyleType::Const },
		}
	}
};
