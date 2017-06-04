/**
	Readiness of clonk type NPCs.
 */


func AppendTo(def type)
{
	type->AddFunction(this.Agent_IsReadyForCommand);
}

public func Agent_IsReadyForCommand()
{
	if (GetCommand() && GetCommand() != "Wait") return false;
	
	return GetProcedure() == DFA_WALK
	    || GetProcedure() == DFA_SCALE
	    || GetProcedure() == DFA_HANGLE;
}
