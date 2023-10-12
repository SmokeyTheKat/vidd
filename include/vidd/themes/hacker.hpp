#define BGCOLOR Color("#111")
#define FGCOLOR Color("#29cd2e")

const Theme hacker {
/*name*/          "hacker",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color("#fff"),
/*highlight*/     Style(Color(), Color("#000a83")),
/*topbarinactive*/Style(FGCOLOR, Color("#232323"), Style::underline),
/*topbaractive*/  Style(FGCOLOR, Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(BGCOLOR, Color("#03cb03")),
/*barinactive*/   Style(FGCOLOR, Color("#1f321f")),
/*numberformat*/  " {:>{}}  ",
/*numberstyle*/   Style(Color("#20731e"), BGCOLOR),
/*nolineformat*/  "~{:>{}}  ",
/*nolinestyle*/   Style(FGCOLOR, BGCOLOR),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color("#2085de"), BGCOLOR),
/*Operator*/      Style(FGCOLOR, BGCOLOR),
/*Macro*/         Style(Color("#34a7d3"), BGCOLOR),
/*Type*/          Style(Color("#03e3eb"), BGCOLOR),
/*Comment*/       Style(Color("#035d2f"), BGCOLOR),
/*String*/        Style(Color("#03e3eb"), BGCOLOR),
/*Const*/         Style(Color("#a302da"), BGCOLOR),
/*Number*/        Style(Color("#d5d706"), BGCOLOR),
/*Function*/      Style(Color("#cf6109"), BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
