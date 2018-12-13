/*
	Assert.c

	@author Marky
	@version 0.2.0
 */


/**
 Throws a fatal error if the object has no AI.

 @par agent The object that is being controlled by the AI.
 @par description [optional] A description for the error.
*/
global func AssertHasAI(object agent, string description)
{
	AssertDefinitionContext(description);

	var controller = this->~GetAI(agent) ?? agent->~GetAI();

	if (!controller)
	{
		FatalError(Format("%s: %v has no AI!", description ?? "Cannot call this function", agent));
	}
	else
	{
		return controller;
	}
}
