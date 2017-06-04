/**
	Vision for clonk type NPCs.
 */

public func Agent_FindItemType(object agent, def type)
{
	var found = _inherited(agent, type);

	var candidates = agent->FindObjects(Find_ID(type), Find_NoContainer(), Find_Distance(250));

	if (found) PushBack(candidates, found);

	return agent->GetNearestObject(candidates);
}
