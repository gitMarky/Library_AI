/*
	Functions.c

	Functions for copying functions;

	@author: Marky
 */




global func AddFunction(func call, bool override)
{
	var name = GetFunctionName(call);

	if (this[name] && !override)
	{
		FatalError(Format("Cannot override function %s", name));
	}
	
	this[name] = call;
}


global func AddFunctions(array functions, bool override)
{
	for (var call in functions)
	{
		if (call)
		{
			AddFunction(call, override);
		}
	}
}


global func GetFunctionName(func call)
{
	return RegexReplace(Format("%v", call), "(.+)\\.(.+)", "$2");
}
