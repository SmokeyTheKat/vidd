#define E_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define E_NUMBERS "0123456789"
const Language escc {
	"escc",
	{ ".esc" },
	{
		false,
/*hglt consts*/      false,
		false,
		false,
		false,
		{ "\"" },
		{},
		{ ";" },
		{},
		{
			{ { Syntax::Range::Type::While, true, ".", E_CHARACTERS "._" }, StyleType::KeyWord },
			{ { Syntax::Range::Type::While, true, "*", E_CHARACTERS "._" }, StyleType::KeyWord },
			{ { Syntax::Range::Type::While, true, "@", E_CHARACTERS "._" }, StyleType::Const  },
			{ { Syntax::Range::Type::While, true, "%", E_CHARACTERS "._" }, StyleType::Type  },
			{ { Syntax::Range::Type::While, true, "$", E_NUMBERS "xabcdefXABCDEF_" }, StyleType::Number  },
			{ { Syntax::Range::Type::While, true, "!", E_CHARACTERS }, StyleType::Macro  },
			{ { Syntax::Range::Type::While, true, "#", E_CHARACTERS "._" }, StyleType::Macro  },
			{ { Syntax::Range::Type::While, true, "'", E_CHARACTERS }, StyleType::String  },
		},
		{}
	}
};
#undef E_CHARACTERS
#undef E_NUMBERS
