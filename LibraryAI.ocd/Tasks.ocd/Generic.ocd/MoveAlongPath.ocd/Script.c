#include Task_Template

private func CreateTask()
{
	var task = inherited();
	task->AddFunctions([this.SetStart, this.SetDestination, this.OnTaskFailure]);
	return task;
}


public func Execute(proplist controller, object agent)
{
	if (!this.PathStart || !this.PathDestination)
	{
		FatalError("Task definition error: There is not start and no destination");
	}
	else
	{
		this.PathInfo = this.PathInfo ?? Map_Waypoint->FindPath(this.PathStart, this.PathDestination);
	}

	if (this.PathInfo)
	{
		// Determine where to move to
		var move_to = this.PathInfo[this.PathProgress];
		if (!move_to)
		{
			return TASK_EXECUTION_FAILURE;
		}

		// Takes too long?
		this.PathTimeout = this.PathTimeout ?? (FrameCounter() + Max(100, ObjectDistance(agent, move_to) * 2)); // Timeout of at least 100 frames, or 200 frames per 100 pixels distance
		if (FrameCounter() > this.PathTimeout)
		{
			return TASK_EXECUTION_FAILURE;
		}

		// Arrived?
		var logic = controller->GetAgent();
		if (logic->Agent_IsNear(agent, move_to))
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
				return TASK_EXECUTION_IN_PROGRESS;
			}
		}
		else // Move there!
		{
			logic->Agent_MoveTo(agent, move_to);
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
	ResetPath();
	return this;
}


public func SetDestination(object destination)
{
	this.PathDestination = destination;
	ResetPath();
	return this;
}

private func ResetPath()
{
	this.PathProgress = 0;
	this.PathInfo = nil;
	this.PathTimeout = nil;
}
