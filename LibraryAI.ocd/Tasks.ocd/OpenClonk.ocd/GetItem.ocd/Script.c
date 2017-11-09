/**
	Get item

	This task makes the agent collect an item, see {@link Task_GetItem#Execute}.

	{@example
		{@code Task_GetItem->AddTo(clonk, priority)->SetTarget(Metal)}
		Makes {@code clonk} collect a {@code Metal} object.
	}
	
	Requires the following functions to be implemented in the agent logic:
	- {@code Agent_FindItem(agent, GetItem())}
	- {@code Agent_HasItem(agent, item)}
    - {@code Agent_IsNear(agent, item)}
    - {@code Agent_TakeItem(agent, item)}
    - {@code Agent_MoveTo(agent, item)}
    
	@author Marky
	@version 0.2.0
*/
#include Task_Template


private func CreateTask()
{
	var task = inherited();
	task->AddFunctions([this.SetItem, this.GetItem]);
	return task;
}


/**
 <ol>
 <li>Finds an item, as defined by {@link Task_GetItem#SetItem}.</li>
 <li>Succeeds if the agent has such an item</li>
 <li>Otherwise,
     <ul>
     <li>if the agent is near the item it collects the item</li>
     <li>else the agent moves to the item</li>
     </ul>
 </li>
 </ol>
*/
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


/**
 Defines what kind of item to get.

 @par item The agent should get this item. This depends on the input type:
<table>
<tr><td>C4V_Def</td><td>Defines the type of the item.</td></tr>
<tr><td>C4V_C4Object</td><td>Defines a specific item.</td></tr>
<tr><td>C4V_Proplist, C4V_Array</td><td>Defines a one, or several search criteria that each have to be defined with {@link Global#Find_Later}.</td></tr>
</table>
*/
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


/**
 Find out what kind of item should be fetched.
 
 @return The item type.
 @version 0.2.0
 */
public func GetItem()
{
	return this.TaskItem;
}
