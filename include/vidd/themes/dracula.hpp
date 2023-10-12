#define BGCOLOR Color("#282a36")
#define FGCOLOR Color("#f8f8f2")

const Theme dracula {
/*name*/          "dracula",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color("#ff5555"),
/*highlight*/     Style(Color(), Color("#44475a")),
/*topbarinactive*/Style(FGCOLOR, Color("#3e3456")),
/*topbaractive*/  Style(FGCOLOR, Color("#1e2132")),
/*modeformat*/    "[{}]",
/*baractive*/     Style(FGCOLOR, Color("#3e3456")),
/*barinactive*/   Style(FGCOLOR, Color("#1e2132")),
/*numberformat*/  " {:>{}}  ",
/*numberstyle*/   Style(Color("#44475a"), BGCOLOR),
/*nolineformat*/  "~{:>{}}  ",
/*nolinestyle*/   Style(FGCOLOR, BGCOLOR),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color("#ff79c6"), BGCOLOR),
/*Operator*/      Style(Color("#ff5555"), BGCOLOR),
/*Macro*/         Style(Color("#34a7d3"), BGCOLOR),
/*Type*/          Style(Color("#8be9fd"), BGCOLOR),
/*Comment*/       Style(Color("#6272a4"), BGCOLOR),
/*String*/        Style(Color("#f1fa8c"), BGCOLOR),
/*Const*/         Style(Color("#bd93f9"), BGCOLOR),
/*Number*/        Style(Color("#bd93f9"), BGCOLOR),
/*Function*/      Style(Color("#50fa7b"), BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
