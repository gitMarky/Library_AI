
/**
 Defines a Find_* function to be called later. The functions can be called with Find_Now().

 This is useful if you want to define certain criteria for finding objects, but you
 need them in a different function call.
 
 @par call This Find_* function that will be called.
           Should be a function pointer to one of the Global.Find_* functions.
           Up to 9 additional parameters for the function call are possible.
 */
global func Find_Later(func call)
{
	var function_name = GetFunctionName(call);
	if (RegexMatch(function_name, "Find_.+"))
	{
		if (function_name == "Find_Later" || function_name == "Find_Now")
		{
			FatalError("Find_Later and Find_Now may not be called later");
		}
	
		var find_criteria = { FunctionCall = call, FunctionParameters = []};
		
		var parameter;
		for (var i = 1; parameter = Par(i); ++i)
		{
			find_criteria.FunctionParameters[i-1] = parameter;
		}
		return find_criteria;
	}
	else
	{
		FatalError(Format("Find_Delayed can only call Find_* functions, passed %s", function_name));
	}
}


/**
 Evaluates a Find_Later() function.
 
 @par find_criteria Can either be criteria defined by Find_Later or
                    an array of criteria defined by Find_Later.
                    In case of an array, the criteria will be
                    joined by Find_And().
 */
global func Find_Now(find_criteria)
{
	if (GetType(find_criteria) == C4V_PropList)
	{
		if (find_criteria.FunctionCall && find_criteria.FunctionParameters)
		{
			return Call(find_criteria.FunctionCall,
			            find_criteria.FunctionParameters[0],
			            find_criteria.FunctionParameters[1],
			            find_criteria.FunctionParameters[2],
			            find_criteria.FunctionParameters[3],
			            find_criteria.FunctionParameters[4],
			            find_criteria.FunctionParameters[5],
			            find_criteria.FunctionParameters[6],
			            find_criteria.FunctionParameters[7],
			            find_criteria.FunctionParameters[8]);
		}
		else
		{
			FatalError(Format("Parameter %v does not come from Find_Delayed", find_criteria));
		}
	}
	else if (GetType(find_criteria) == C4V_Array)
	{
		var found = nil;
		for (var call in find_criteria)
		{
			found = Find_And(Find_Now(call), found);
		}
		return found;
	}
}
