#include Task_Template

private func CreateTask()
{
	var task = inherited();
	task->AddFunctions([this.SetStart, this.SetDestination, this.OnTaskFailure]);
	return task;
}


public func Execute(proplist controller, object agent)
{
	if (!this.PathStartConfigured || !this.PathStartConfigured)
	{
		FatalError("Task definition error: There is not start and no destination");
	}
	if (!this.PathStart || !this.PathDestination) // Target or destination vanished?
	{
			return TASK_EXECUTION_FAILURE;
	}
	else
	{
		FindPath();
	}

	if (this.PathInfo)
	{
		// Determine where to move to
		if (!this.PathMoveTo)
		{
			return TASK_EXECUTION_FAILURE;
		}

		// Takes too long?
		if (this.PathTimeout && (FrameCounter() > this.PathTimeout))
		{
			return TASK_EXECUTION_FAILURE;
		}

		// Arrived?
		var logic = controller->GetAgent();
		if (logic->Agent_IsNear(agent, this.PathMoveTo))
		{
			// The next step!
			this.PathProgress += 1;
			this.PathTimeout = nil;

			// The end?
			if (this.PathProgress >= GetLength(this.PathInfo))
			{
				return TASK_EXECUTION_SUCCESS;
			}
			else
			{
				this.PathMoveFrom = this.PathMoveTo;
				this.PathMoveTo = this.PathInfo[this.PathProgress];
				this.PathTimeout = this.PathTimeout ?? (FrameCounter() + Max(100, ObjectDistance(agent, this.PathMoveTo) * 2)); // Timeout of at least 100 frames, or 200 frames per 100 pixels distance
				return TASK_EXECUTION_IN_PROGRESS;
			}
		}
		else // Move there!
		{
			MoveAlongPath(logic, agent, this.PathMoveFrom, this.PathMoveTo);
			return TASK_EXECUTION_IN_PROGRESS;
		}
	}
	else
	{
		return TASK_EXECUTION_FAILURE;
	}
}


public func OnTaskFailure(proplist controller, object agent)
{
	// Stand still
	agent->SetCommand("None");
	agent->SetComDir(COMD_None);
}


public func SetStart(object start)
{
	this.PathStart = start;
	this.PathMoveFrom = start;
	this.PathMoveTo = start;
	this.PathStartConfigured = true;
	ResetPath();
	if (this.PathDestination)
	{
		FindPath();
	}
	return this;
}


public func SetDestination(object destination)
{
	this.PathDestination = destination;
	this.PathDestinationConfigured = true;
	ResetPath();
	if (this.PathStart)
	{
		FindPath();
	}
	return this;
}


private func ResetPath()
{
	this.PathProgress = 0;
	this.PathInfo = nil;
	this.PathTimeout = nil;
}


private func FindPath()
{
	this.PathInfo = this.PathInfo ?? Map_Waypoint->FindPath(this.PathStart, this.PathDestination);
}


private func MoveAlongPath(proplist logic, object agent, object move_from, object move_to)
{
	var path = move_from->~GetPath(move_to);
	if (path && path.OnMoveTo)
	{
		path->OnMoveTo(logic, agent, move_from, move_to);
	}
	else
	{
		logic->Agent_MoveTo(agent, move_to);
	}
}
