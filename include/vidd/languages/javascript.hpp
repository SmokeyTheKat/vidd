const Language javascript {
	"javascript",
	{ ".js", ".jsx", ".json" },
	{
		true,
/*hglt consts*/      true,
		true,
		false,
		true,
		{ "'", "\"", "`" },
/*multi strings*/    {},
		{ { "/*", "*/" } },
		{ "//" },
		{ "+", "-", "*", "/", "=", "!", "~", "<", ">", "?", "&", "%", "|", "^", "$" },
		{},
		{ 
			{ "abstract", StyleType::KeyWord },
			{ "async", StyleType::KeyWord },
			{ "case", StyleType::KeyWord },
			{ "debugger", StyleType::KeyWord },
			{ "enum", StyleType::KeyWord },
			{ "finally", StyleType::KeyWord },
			{ "implements", StyleType::KeyWord },
			{ "private", StyleType::KeyWord },
			{ "super", StyleType::KeyWord },
			{ "transient", StyleType::KeyWord },
			{ "volatile", StyleType::KeyWord },
			{ "arguments", StyleType::KeyWord },
			{ "catch", StyleType::KeyWord },
			{ "default", StyleType::KeyWord },
			{ "eval", StyleType::KeyWord },
			{ "float", StyleType::KeyWord },
			{ "import", StyleType::KeyWord },
			{ "long", StyleType::KeyWord },
			{ "protected", StyleType::KeyWord },
			{ "switch", StyleType::KeyWord },
			{ "while", StyleType::KeyWord },
			{ "await", StyleType::KeyWord },
			{ "char", StyleType::KeyWord },
			{ "delete", StyleType::KeyWord },
			{ "export", StyleType::KeyWord },
			{ "for", StyleType::KeyWord },
			{ "in", StyleType::KeyWord },
			{ "native", StyleType::KeyWord },
			{ "public", StyleType::KeyWord },
			{ "synchronized", StyleType::KeyWord },
			{ "try", StyleType::KeyWord },
			{ "with", StyleType::KeyWord },
			{ "boolean", StyleType::KeyWord },
			{ "class", StyleType::KeyWord },
			{ "do", StyleType::KeyWord },
			{ "extends", StyleType::KeyWord },
			{ "function", StyleType::KeyWord },
			{ "instanceof", StyleType::KeyWord },
			{ "new", StyleType::KeyWord },
			{ "return", StyleType::KeyWord },
			{ "this", StyleType::KeyWord },
			{ "typeof", StyleType::KeyWord },
			{ "yield", StyleType::KeyWord },
			{ "break", StyleType::KeyWord },
			{ "double", StyleType::KeyWord },
			{ "goto", StyleType::KeyWord },
			{ "int", StyleType::KeyWord },
			{ "short", StyleType::KeyWord },
			{ "throw", StyleType::KeyWord },
			{ "byte", StyleType::KeyWord },
			{ "continue", StyleType::KeyWord },
			{ "else", StyleType::KeyWord },
			{ "final", StyleType::KeyWord },
			{ "if", StyleType::KeyWord },
			{ "interface", StyleType::KeyWord },
			{ "package", StyleType::KeyWord },
			{ "static", StyleType::KeyWord },
			{ "throws", StyleType::KeyWord },
			{ "void", StyleType::KeyWord },
			{ "FILE", StyleType::Type },
			{ "true", StyleType::Const },
			{ "false", StyleType::Const },
			{ "undefined", StyleType::Const },
			{ "null", StyleType::Const },
			{ "let", StyleType::Type },
			{ "const", StyleType::Type },
			{ "var", StyleType::Type },
		}
	}
};
