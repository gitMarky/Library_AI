/**
	Basic functionality for agents
 
	Tasks define what to do, such as: Get an item, attack an enemy.

	Agents on the other hand, define if the object can do that
	and how it does that: one type of AI might fly there, and the
	other type walks, but the logic what they do (the task) stays
	the same.

	This separation allows for a flexible approach - you can keep
	tasks fairly simple and reuse them, and you can quickly
	implement new types of agent logic.

	Furthermore, agent logic should be modular, so that you
	can create a new agent simply by including the desired
	components.

	@author Marky
	@version 0.2.0
 */
 
/* -- Callbacks -- */

/**
 Callback from {@link AI_TaskExecutor#SetAgent}.
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


/* -- Public interface -- */


/* -- Internals --*/


/**
 Is added to the actual agent.
 */
private func Agent_Properties()
{
	return this.agent_property_list;
}
