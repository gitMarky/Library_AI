/**
 
 */

public func Agent_HasItem(object agent, object item)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(item);

	return agent == item->Contained() || _inherited(agent, item);
}


public func Agent_TakeItem(object agent, object item)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(item);
	
	agent->Collect(item);
}


public func Agent_CanTakeItem(object agent, object item)
{
	return ObjectDistance(agent, item) < 10;
}


public func Agent_FindItemType(object agent, def type)
{
	return agent->FindContents(type) ?? _inherited(agent, type);
}