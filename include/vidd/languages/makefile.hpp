#define E_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define E_NUMBERS "0123456789"
const Language makefile {
	"makefile",
	{ "makefile", "Makefile" },
	{
		true,
/*hglt consts*/      true,
		false,
		false,
		false,
		{ "'", "\"", "`" },
		{},
		{ "#" },
		{ "|", ">", "<", "&", "$", "@", ":", "=" },
		{
			{ { Syntax::Range::Type::While, true, "-", E_CHARACTERS "-_+" }, StyleType::Type },
			{ { Syntax::Range::Type::While, false, "    ", E_CHARACTERS "-_" }, StyleType::KeyWord },
			{ { Syntax::Range::Type::Until, true, "$(", ")" }, StyleType::Macro },
		},
		{}
	}
};

#undef E_CHARACTERS
#undef E_NUMBERS
