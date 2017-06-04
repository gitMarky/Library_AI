/**
	AI Component: Task execution
	
	Use this component in your AI if you want it to be able to solve/execute tasks.
*/

/* -- AI Execution -- */

public func Execute(effect controller, int time)
{
	// handle all parallel tasks

	for (var task in controller->GetParallelTasks())
	{
		task->~Execute(controller, controller.Target);
	}
	
	// handle the active prioritized task - probably easier by passing the effect to the function
	
	var current_task = controller->GetCurrentTask() ?? controller->FindCurrentTask();
	if (current_task)
	{
		current_task->~Execute(controller, controller.Target);
	}

	return FX_OK;
}

/* -- Callbacks -- */

// Callback from the Definition()-call
public func OnDefineAI(proplist controller)
{
	_inherited(controller);
	
	// add functions to the AI <- should not be necessary
	controller->GetControlEffect().AddTask = this.AddTask;
	controller->GetControlEffect().GetParallelTasks = this.GetParallelTasks;
	controller->GetControlEffect().GetPriorityTasks = this.GetPriorityTasks;
	controller->GetControlEffect().GetCurrentTask = this.GetCurrentTask;

	// internal interface
	controller->GetControlEffect().SetCurrentTask = this.SetCurrentTask;
	controller->GetControlEffect().FindCurrentTask = this.FindCurrentTask;

	// add prop->GetAI()es to the AI
	var properties = {
		Tasks = {
			Parallel = [], // tasks that can be executed parallel
			Priority = [], // tasks that have to be executed by priority
			Current = nil, // task that has currently has the highest priority
		},
	};

	AddProperties(controller.FxAI, properties);
}


/* -- Public (AI Controller Effect) Interface -- */
// should not be called from definition context, but only from the AI itself

public func AddTask(proplist task)
{
	Log("Calling 'AddTask'; Context: %v in %v", GetType(this), this);

	AssertNotNil(task);

	var priority = task->~GetPriority();
	if (priority > 0)
	{
		PushBack(GetPriorityTasks(), task);

		// update the current task
		if (GetCurrentTask() && GetCurrentTask()->~GetPriority() < priority)
		{
			SetCurrentTask(task);
		}
	}
	else
	{
		PushBack(GetParallelTasks(), task);
	}
}

public func GetParallelTasks()
{
	return this.Tasks.Parallel;
}

public func GetPriorityTasks()
{
	return this.Tasks.Priority;
}

public func GetCurrentTask()
{
	return this.Tasks.Current;
}

/* -- Internals -- */

private func SetCurrentTask(proplist task)
{
	AssertNotNil(task);
	
	// TODO: At the moment this simply overrides the current task
	// Should probably have some kind of handshake-mechanism, so
	// that the old task can perform a cleanup

	this.Tasks.Current = task;
}


private func FindCurrentTask()
{
	if (GetLength(GetPriorityTasks()) == 0)
	{
		return nil;
	}
	else
	{
		RemoveHoles(GetPriorityTasks());
		
		// find
		var current_task;
		for (var task in GetPriorityTasks())
		{
			if (!current_task || current_task->GetPriority() < task->GetPriority())
			{
				current_task = task;
			}
		}
		
		// update
		if (GetCurrentTask() != current_task)
		{
			SetCurrentTask(current_task);
		}

		return task;
	}
}
