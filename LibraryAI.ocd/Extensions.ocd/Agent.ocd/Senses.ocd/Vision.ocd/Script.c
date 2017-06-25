/**
	Vision for clonk type NPCs.
 */


public func Agent_Initialize(object agent)
{
	_inherited(agent);

	agent->Agent_Properties()->AddFunctions([this.SetVisionRange, this.GetVisionRange]);
	agent->Agent_Properties()->SetVisionRange();
}


public func Agent_FindItemType(object agent, def type)
{
	var found = _inherited(agent, type);
	var range = agent->Agent_Properties()->GetVisionRange();
	var candidates = agent->FindObjects(Find_ID(type), Find_NoContainer(), Find_Distance(range));

	if (found) PushBack(candidates, found);

	return agent->GetNearestObject(candidates);
}


public func Agent_FindItem(object agent, array find_criteria)
{
	var found = _inherited(agent, find_criteria);
	var range = agent->Agent_Properties()->GetVisionRange();
	var candidates = agent->FindObjects(find_criteria, Find_NoContainer(), Find_Distance(range));

	if (found) PushBack(candidates, found);

	return agent->GetNearestObject(candidates);
}

/* -- Internals that are added to the agent logic -- */

private func SetVisionRange(int range)
{
	this.agent_vision_range = range ?? 850;
}


private func GetVisionRange()
{
	return this.agent_vision_range ?? 1;
}
