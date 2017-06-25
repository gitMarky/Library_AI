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

		var item;
		if (GetType(GetItem()) == C4V_C4Object)
		{
			item = GetItem();
		}
		else if (GetType(GetItem()) == C4V_Def)
		{
			item = logic->Agent_FindItem(agent, Find_ID(GetItem()));
		}
		else
		{
			item = logic->Agent_FindItem(agent, Find_Now(GetItem()));
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
	if (GetType(item) == C4V_Def || GetType(item) == C4V_C4Object || GetType(item) == C4V_PropList)
	{
		this.TaskItem = item;
		return this;
	}
	else if (GetType(item) == C4V_Array)
	{
		if (GetLength(item) == 0)
		{
			FatalError("The array must not be empty");
		}
		for (var content in item)
		{
			var type = GetType(content);
			if (type != C4V_PropList)
			{
				FatalError("The parameters in the array need to come from Find_Later");
			}
		}

		this.TaskItem = item;
		return this;
	}
	else
	{
		FatalError(Format("The parameter needs to be a definition, an object, or a single call/array of calls of Find_Later"));
	}
}


public func GetItem()
{
	return this.TaskItem;
}
