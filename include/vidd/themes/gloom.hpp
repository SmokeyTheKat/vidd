#define BGCOLOR Color(15, 18, 15)
#define FGCOLOR Color(216, 222, 180)

const Theme gloom {
/*name*/          "gloom",
/*fg*/            Style(FGCOLOR, {}),
/*bg*/            Style({}, BGCOLOR),
/*text*/          Style(FGCOLOR, BGCOLOR),
/*important*/     Color(80, 80, 255),
/*highlight*/     Style(Style::reverse),
/*topbarinactive*/Style(FGCOLOR, Color("#232323"), Style::underline),
/*topbaractive*/  Style(FGCOLOR, Color("#000"), Style::underline),
/*modeformat*/    "[{}]",
/*baractive*/     Style(FGCOLOR, Color(0)),
/*barinactive*/   Style(BGCOLOR, Color(90)),
/*numberformat*/  " {:>{}} ",
/*numberstyle*/   Style(Color(38, 46, 38), Color(3, 4, 3)),
/*nolineformat*/  "~{:>{}} ",
/*nolinestyle*/   Style(Color(255), BGCOLOR),
	{ /*syntax highlighting*/
/*KeyWord*/       Style(Color(38, 166, 166), BGCOLOR),
/*Operator*/      Style(Color(174, 208, 44), BGCOLOR),
/*Macro*/         Style(Color(0, 255, 255), BGCOLOR),
/*Type*/          Style(Color(255, 93, 57), BGCOLOR),
/*Comment*/       Style(Color(79, 110, 97), BGCOLOR),
/*String*/        Style(Color(174, 208, 44), BGCOLOR),
/*Const*/         Style(Color(174, 208, 44), BGCOLOR),
/*Number*/        Style(Color(184, 101, 223), BGCOLOR),
/*Function*/      Style(Color(117, 225, 238), BGCOLOR),
	}
};

#undef BGCOLOR
#undef FGCOLOR
