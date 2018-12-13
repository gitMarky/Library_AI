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
		this.start_x = this.Target->GetX();
		this.start_y = this.Target->GetY();
	},

	Timer = func (int time)
	{
		// Cancel the whole thing?
		if (!this.destination || !this.path)
		{
			return FX_Execute_Kill;
		}

		var waypoint = this.path[this.progress];
		var logic = this.Target->GetAI()->GetAgent();
		var agent = this.Target;

		// Something changed? Update the path, but only every 35 frames (displaced by 18, so that it does not recalculate instantly)
		var interval_ok = (time % 35) == 18;
		var destination_changed = this.x != this.destination->GetX() || this.y != this.destination->GetY();

		var dist_path = Distance(this.start_x, this.start_y, waypoint->GetX(), waypoint->GetY());
		var dist_agent = Distance(this.start_x, this.start_y, agent->GetX(), agent->GetY());
		var dist_wp = ObjectDistance(agent, waypoint);

		var off_path = dist_wp >= (6 * dist_path / 5); // logic->Agent_IsBetween(agent->GetX(), agent->GetY(), this.start_x, this.start_y, waypoint->GetX(), waypoint->GetY());
		if (interval_ok && (destination_changed || off_path))
		{
			agent->Message("Recalculating path %d", FrameCounter());
			// Cancel current command, so that move to works correctly
			agent->SetCommand("None");
			// Calculate the path again
			logic->Agent_MoveTo(agent, this.destination);
			return FX_OK;
		}

		// Wait while there is a command, otherwise move
		if (logic->Agent_IsReadyForCommand(agent))
		{
			// Reached the waypoint?
			if (logic->Agent_IsNear(agent, waypoint))
			{
				// Update progress and position
				this.progress += 1;
				this.start_x = this.Target->GetX();
				this.start_y = this.Target->GetY();

				// Arrived?
				if (this.progress >= GetLength(this.path))
				{
					if (this.destination)
					{
						agent->SetCommand("MoveTo", this.destination, nil, nil, nil, nil, 3);
					}
					return FX_Execute_Kill;
				}
			}
			else // Move to the waypoint
			{
				agent->SetCommand("MoveTo", waypoint, nil, nil, nil, nil, 3);
			}
		}
		return FX_OK;
	},
};
