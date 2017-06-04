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
		var item;
		if (GetType(GetItem()) == C4V_Def)
		{
			item = FindItem(agent, GetItem());
		}
		else
		{
			item = GetItem();
		}
		
		if (item)
		{
			if (item->Contained() == agent) // got it
			{
				Log("Agent has the item, do nothing");
				return;
			}
			else
			{
				var move_to = item->Contained() ?? item;

				if (ObjectDistance(agent, move_to) < 10)
				{
					agent->Collect(item);
				}
				else if (agent->~Agent_IsReadyForCommand())
				{
					agent->SetCommand("Follow", move_to);
				}
			}
		}
		else
		{
			Log("No item found");
		}
	}
	else
	{
		FatalError("Cannot execute task, because there is no item");
	}
}


public func SetItem(item)
{
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
	return this.TaskItem;
}


public func FindItem(object agent, def type)
{
	return agent->FindContents(type) ?? agent->~Agent_FindItem(type);
}
