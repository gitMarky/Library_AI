/*
	Functions.c

	Functions for copying functions;

	@author Marky
 */


/**
 Adds a function from one thing to another.
 This can be called on a proplist, effect,
 definition, or objeffects.note When using this function be sure that
       you know what you are doing. Replacing
       functions can have unintended side effects.

 @par call This is the function that should be added.
 @par override {@code false} means, that overriding
               an existing function is not allowed
               and will lead to a {@link Global#FatalError}

 @version 0.2.0
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


/**
 Adds several functions, calling {@link Global#AddFunction} on all items in the array.

 @par functions These are the functions that should be added.
 @par override {@code false} means, that overriding
               an existing function is not allowed
               and will lead to a {@link Global#FatalError}
 @version 0.2.0
*/
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


/**
 Gets the name of a function.
 This is somewhat  trivial, however such a function  did not exist.

 @return string {@code function_name} in {@code source_type.function_name} of {@code call}.
 @version 0.2.0
*/
global func GetFunctionName(func call)
{
	return RegexReplace(Format("%v", call), "(.+)\\.(.+)", "$2");
}
