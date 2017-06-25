/**
 Basic functionality for agents
 
 
 */
 
public func Agent_Initialize(object agent)
{
	AssertDefinitionContext();
	
	if (!agent.agent_property_list)
	{
		// Initialize empty proplist
		agent.agent_property_list = {};
		// Add functions
		agent->AddFunction(this.Agent_Properties);
		// Give it functions for adding functions
		agent->Agent_Properties().AddFunction = Global.AddFunction;
		agent->Agent_Properties().AddFunctions = Global.AddFunctions;
	}
}


/**
 Is added to the actual agent.
 */
private func Agent_Properties()
{
	return this.agent_property_list;
}
