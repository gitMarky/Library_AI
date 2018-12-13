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

	var com_name = agent->GetCommand(0);
	var com_target = agent->GetCommand(1);



	if (!agent->GetAI()->GetAgent()->Agent_IsReadyForCommand(agent)
	 && !(com_name == "MoveTo" && com_target == target)) return;


	if (target->GetSpeed() <= 2)
	{
		agent->SetCommand("MoveTo", target);
	}
	else
	{
		agent->SetCommand("Follow", target);
	}
}
