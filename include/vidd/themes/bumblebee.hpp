#define BGCOLOR Color(0)
#define FGCOLOR Color(255)

const Theme bumblebee {
/*name*/          "bumblebee",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color(80, 80, 255),
/*highlight*/     Style(Style::reverse),
/*topbarinactive*/Style(Color("#fff"), Color("#232323"), Style::underline),
/*topbaractive*/  Style(Color("#fff"), Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(Color(0), Color(255, 255, 0)),
/*barinactive*/   Style(Color(0), Color(255)),
/*numberformat*/  "{:>{}}│",
/*numberstyle*/   Style(Color(255, 255, 0), Color(0)),
/*nolineformat*/  "~{:>{}} ",
/*nolinestyle*/   Style(Color(255, 255, 0), Color(0)),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color(255, 255, 0), BGCOLOR),
/*Operator*/      Style(Color(255, 255, 0), BGCOLOR),
/*Macro*/         Style(Color(0, 255, 255), BGCOLOR),
/*Type*/          Style(Color(255, 0, 0), BGCOLOR),
/*Comment*/       Style(Color(0, 0, 255), BGCOLOR),
/*String*/        Style(Color(255, 0, 255), BGCOLOR),
/*Const*/         Style(Color(50, 100, 255), BGCOLOR),
/*Number*/        Style(Color(0, 255, 0), BGCOLOR),
/*Function*/      Style(FGCOLOR, BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
