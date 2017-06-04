/*
	Functions.c

	Functions for copying functions;

	@author: Marky
 */
 
global func AddFunctions(array functions, bool override)
{
	for (var function in functions)
	{
		if (function)
		{
			AddFunction(function, override);
		}
	}
}


global func AddFunction(func function, bool override)
{
	var name = GetFunctionName(function);

	if (this[name] && !override)
	{
		FatalError(Format("Cannot override function %s", name));
	}
	
	this[name] = function;
}

global func GetFunctionName(func function)
{
	return RegexReplace(Format("%v", function), "(.+)\\.(.+)", "$2");
}
