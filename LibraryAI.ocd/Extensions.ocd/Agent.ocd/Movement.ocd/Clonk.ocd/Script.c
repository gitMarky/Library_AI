/**
 
 */

public func Agent_IsNear(object agent, object target)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(target);

	return ObjectDistance(agent, target) <= 10;
}


public func Agent_MoveTo(object agent, object target)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(target);

	if (!agent->GetAI()->GetAgent()->Agent_IsReadyForCommand(agent)) return;
	
	if (target->GetSpeed() <= 2)
	{
		agent->SetCommand("MoveTo", target);
	}
	else
	{
		agent->SetCommand("Follow", target);
	}
}
