const Language funs {
/*name*/    "funs",
/*exts*/    { ".funs" },
/*syntax*/  {
/*hglt types*/       true,
/*hglt consts*/      false,
/*hglt func calls*/  false,
/*fcall any braket*/ false,
/*hglt nums*/        true,
/*string tokens*/    {},
/*multi comments*/   {},
/*single comments*/  { ";" },
/*operators*/        { "+", "-", "*", "/", "=", "!", "~", "<", ">", "?", ":", "&", "%", "|", "^", ";" },
/*syntax ranges*/    {},
/*syntax words*/     {
						 { "plotFx", StyleType::KeyWord },
						 { "plotFxy", StyleType::KeyWord },
					 }
			}
};
