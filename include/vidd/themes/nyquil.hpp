#define BGCOLOR Color(0, 0, 0)
#define FGCOLOR Color(249)

const Theme nyquil {
/*name*/          "nyquil",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color(255, 0, 255),
/*highlight*/     Style(Color(), Color(31, 36, 48)),
/*topbarinactive*/Style(Color("#fff"), Color("#232323"), Style::underline),
/*topbaractive*/  Style(Color("#fff"), Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(Color(180), Color(10)),
/*barinactive*/   Style(Color(20), Color(90)),
/*numberformat*/  " {:>{}} ",
/*numberstyle*/   Style(Color(25, 27, 31), BGCOLOR),
/*nolineformat*/  "~{:>{}} ",
/*nolinestyle*/   Style(Color(255), BGCOLOR),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color(246, 106, 49), BGCOLOR),
/*Operator*/      Style(Color(249, 255, 107), BGCOLOR),
/*Macro*/         Style(Color(0, 255, 255), BGCOLOR),
/*Type*/          Style(Color(60, 115, 211), BGCOLOR),
/*Comment*/       Style(Color(205, 10, 113), BGCOLOR),
/*String*/        Style(Color(213, 55, 202), BGCOLOR),
/*Const*/         Style(Color(174, 208, 44), BGCOLOR),
/*Number*/        Style(Color(184, 101, 223), BGCOLOR),
/*Function*/      Style(Color(133, 255, 102), BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
