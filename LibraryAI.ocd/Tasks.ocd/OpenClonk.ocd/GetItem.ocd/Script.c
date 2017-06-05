#include Task_Template

private func CreateTask()
{
	var task = inherited();
	task->AddFunctions([this.SetItem, this.GetItem]);
	return task;
}


public func Execute(proplist controller, object agent)
{
	if (GetItem())
	{
		var logic = controller->GetAgent();
		
		
		
		Log("Controller has tasks: ");
		for (var task in controller->GetPriorityTasks())
		{
			Log("- %v", task);
		}

		var item;
		if (GetType(GetItem()) == C4V_Def)
		{
			item = logic->Agent_FindItemType(agent, GetItem());
		}
		else
		{
			item = GetItem();
		}
		
		if (item)
		{
			if (logic->Agent_HasItem(agent, item)) // got it
			{
				return TASK_EXECUTION_SUCCESS;
			}
			else
			{
				var move_to = item->Contained() ?? item;

				if (logic->Agent_IsNear(agent, move_to))
				{
					logic->Agent_TakeItem(agent, item);
				}
				else
				{
					logic->Agent_MoveTo(agent, move_to);
				}
				
				return TASK_EXECUTION_IN_PROGRESS;
			}
		}
		else
		{
			return TASK_EXECUTION_FAILURE;
		}
	}
	else
	{
		FatalError("Task definition error: Cannot execute task, because there is no item");
	}
}


public func SetItem(item)
{
	Log("SetItem(%v) called from %v", item, this);

	if (GetType(item) == C4V_Def || GetType(item) == C4V_C4Object)
	{
		this.TaskItem = item;
		return this;
	}
	else
	{
		FatalError(Format("The parameter needs to be a definition or an object"));
	}
}


public func GetItem()
{
	Log("GetItem called from %v", this);

	return this.TaskItem;
}
