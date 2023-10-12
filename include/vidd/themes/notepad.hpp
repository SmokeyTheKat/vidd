#define BGCOLOR Color(255, 255, 255)
#define FGCOLOR Color(0, 2, 0)

const Theme notepad {
/*name*/          "notepad",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color(255, 0, 0),
/*highlight*/     Style(BGCOLOR, Color(51, 152, 255)),
/*topbarinactive*/Style(Color("#fff"), Color("#232323"), Style::underline),
/*topbaractive*/  Style(Color("#fff"), Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(Color(180), Color(1, 43, 53)),
/*barinactive*/   Style(Color(20), Color(90)),
/*numberformat*/  " {:>{}} ",
/*numberstyle*/   Style(Color(160), Color(228)),
/*nolineformat*/  " {:>{}} ",
/*nolinestyle*/   Style(Color(160), Color(228)),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color(38, 15, 255), BGCOLOR),
/*Operator*/      Style(Color(7, 12, 149), BGCOLOR),
/*Macro*/         Style(Color(153, 99, 47), BGCOLOR),
/*Type*/          Style(Color(137, 46, 254), BGCOLOR),
/*Comment*/       Style(Color(94, 170, 80), BGCOLOR),
/*String*/        Style(Color(184, 136, 9), BGCOLOR),
/*Const*/         Style(FGCOLOR, BGCOLOR),
/*Number*/        Style(Color(250, 171, 63), BGCOLOR),
/*Function*/      Style(FGCOLOR, BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
