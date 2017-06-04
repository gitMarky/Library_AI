/**
	Readiness of clonk type NPCs.
 */

public func Agent_IsReadyForCommand(object agent)
{
	AssertDefinitionContext();
	AssertNotNil(agent);

	if (agent->GetCommand() && agent->GetCommand() != "Wait") return false;
	
	return agent->GetProcedure() == DFA_WALK
	    || agent->GetProcedure() == DFA_SCALE
	    || agent->GetProcedure() == DFA_HANGLE;
}
