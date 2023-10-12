#define BGCOLOR Color(1, 19, 26)
#define FGCOLOR Color(133, 151, 158)

const Theme strg {
/*name*/          "strg",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color(255, 255, 0),
/*highlight*/     Style(Style::reverse),
/*topbarinactive*/Style(Color("#fff"), Color("#232323"), Style::underline),
/*topbaractive*/  Style(Color("#fff"), Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(Color(180), Color(1, 43, 53)),
/*barinactive*/   Style(Color(20), Color(90)),
/*numberformat*/  " {:>{}} ",
/*numberstyle*/   Style(Color(72, 110, 122), Color(1, 43, 53)),
/*nolineformat*/  "~{:>{}} ",
/*nolinestyle*/   Style(Color(72, 110, 122), BGCOLOR),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color(184, 136, 9), BGCOLOR),
/*Operator*/      Style(FGCOLOR, BGCOLOR),
/*Macro*/         Style(Color(192, 77, 33), BGCOLOR),
/*Type*/          Style(Color(184, 136, 9), BGCOLOR),
/*Comment*/       Style(Color(79, 98, 105), BGCOLOR),
/*String*/        Style(Color(184, 136, 9), BGCOLOR),
/*Const*/         Style(Color(60, 124, 168), BGCOLOR),
/*Number*/        Style(Color(184, 101, 223), BGCOLOR),
/*Function*/      Style(FGCOLOR, BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
