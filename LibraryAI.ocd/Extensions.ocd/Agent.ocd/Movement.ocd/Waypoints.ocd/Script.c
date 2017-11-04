/**
	Move along waypoints
	
	Might be changed to a task, though, because that is essentially what it is (macro command in Hazard).
 */


public func Agent_MoveTo(object agent, object destination)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(destination);

	if (!agent->GetAI()->GetAgent()->Agent_IsReadyForCommand(agent)) return;
	
	var path = Map_Waypoint->FindPath(agent, destination);
	
	if (path)
	{
		var fx = GetEffect(FxMoveAlongPath.Name, agent) ?? agent->CreateEffect(FxMoveAlongPath, 1, 1);
		fx->SetPath(path, destination);
	}
	else
	{
		_inherited(agent, destination);
	}
}


local FxMoveAlongPath = new Effect
{
	SetPath = func (array path, destination)
	{
		AssertNotNil(path);
		AssertNotEmpty(path);
		AssertNotNil(destination);
		this.path = path;
		this.progress = 0;
		this.destination = destination;
		this.x = destination->GetX();
		this.y = destination->GetY();
	},
	
	Timer = func (int time)
	{
		// Cancel the whole thing?
		if (!this.destination || !this.path)
		{
			return FX_Execute_Kill;
		}
	
		var logic = this.Target->GetAI()->GetAgent();
		var agent = this.Target;	

		// Something changed? Update the path, but only every 35 frames
		if (!(time % 35) && (this.x != this.destination->GetX() || this.y != this.destination->GetY()))
		{
			logic->Agent_MoveTo(agent, this.destination);
			return FX_OK;
		}
	
		// Wait while there is a command, otherwise move
		if (logic->Agent_IsReadyForCommand(agent))
		{			
			// The logic
			var waypoint = this.path[this.progress];

			// Reached the waypoint?
			if (logic->Agent_IsNear(agent, waypoint))
			{
				this.progress += 1;
				
				// Arrived?
				if (this.progress >= GetLength(this.path))
				{
					if (this.destination)
					{
						agent->SetCommand("MoveTo", this.destination);
					}
					return FX_Execute_Kill;
				}
			}
			else // Move to the waypoint
			{
				agent->SetCommand("MoveTo", waypoint);
			}
		}
		return FX_OK;
	},
};
