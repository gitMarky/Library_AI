/*
	Assert.c

	@author: Marky
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
