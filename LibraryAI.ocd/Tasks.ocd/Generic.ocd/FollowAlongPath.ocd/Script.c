#include Task_Template

private func CreateTask()
{
	var task = inherited();
	task->AddFunctions([this.SetTarget, this.GetTarget, this.SucceedOnArrival]);
	return task;
}


public func Execute(proplist controller, object agent)
{
	if (this.TaskConfigured)
	{
		var logic = controller->GetAgent();
		
		var find_criteria = GetTarget();
		if (!find_criteria) // Target can vanish after configuration
		{
			return TASK_EXECUTION_FAILURE;
		}

		var target;
		if (GetType(find_criteria) == C4V_C4Object)
		{
			target = find_criteria;
		}
		else if (GetType(find_criteria) == C4V_Def)
		{
			target = logic->Agent_FindItem(agent, Find_ID(find_criteria));
		}
		else
		{
			target = logic->Agent_FindItem(agent, Find_Now(find_criteria));
		}
		
		if (target)
		{
			var move_to = target->Contained() ?? target;

			if (logic->Agent_IsNear(agent, move_to))
			{
				if (this.TaskMove)
				{
					this.TaskMove = nil;
				}
				
				if (this.TaskSuccessOnArrival)
				{
					return TASK_EXECUTION_SUCCESS;
				}
			}
			else
			{
				if (PathFree(agent->GetX(), agent->GetY(), move_to->GetX(), move_to->GetY()))
				{
					logic->Agent_MoveTo(agent, move_to);
				}
				else if (!this.TaskMove)
				{
					this.TaskMove = Task_MoveAlongPath->AddTo(agent, 1, this)->SetStart(agent)->SetDestination(move_to);
				}
			}
			
			return TASK_EXECUTION_IN_PROGRESS;
		}
		else
		{
			return TASK_EXECUTION_FAILURE;
		}
	}
	else
	{
		FatalError("Task definition error: Cannot execute task, because there is no target");
	}
}


public func SucceedOnArrival()
{
	this.TaskSuccessOnArrival = true;
}


public func SetTarget(target)
{
	if (GetType(target) == C4V_Def || GetType(target) == C4V_C4Object || GetType(target) == C4V_PropList)
	{
		this.TaskTarget = target;
		this.TaskConfigured = true;
		return this;
	}
	else if (GetType(target) == C4V_Array)
	{
		if (GetLength(target) == 0)
		{
			FatalError("The array must not be empty");
		}
		for (var content in target)
		{
			var type = GetType(content);
			if (type != C4V_PropList)
			{
				FatalError("The parameters in the array need to come from Find_Later");
			}
		}

		this.TaskTarget = target;
		this.TaskConfigured = true;
		return this;
	}
	else
	{
		FatalError(Format("The parameter needs to be a definition, an object, or a single call/array of calls of Find_Later"));
	}
}


public func GetTarget()
{
	return this.TaskTarget;
}
