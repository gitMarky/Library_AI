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
		Log("Executing GetItem");
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
			if (item->Contained() == agent)
			{
				Log("Agent has the item, do nothing");
				return;
			}
	
			if (CanAddCommand(agent))
			{
				agent->SetCommand("Get", item);
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
	// look in the agent first
	var found = FindObject(Find_ID(type), Find_Container(agent)) ?? FindObject(Find_ID(type), Find_NoContainer(), agent->Find_Distance(250));
	
	return found;
}
