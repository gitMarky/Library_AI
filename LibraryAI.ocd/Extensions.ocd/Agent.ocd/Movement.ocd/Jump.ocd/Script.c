/**
	Jump
 */

public func Agent_Jump(object agent)
{
	if (_inherited(agent)) return true;

	// Jump if standing on floor.
	if (agent->GetProcedure() == "WALK")
	{
		if (agent->~ControlJump())
		{
			return true; // For clonks.
		}
		else
		{
			return agent->Jump(); // For others.
		}
	}
	return false;
}