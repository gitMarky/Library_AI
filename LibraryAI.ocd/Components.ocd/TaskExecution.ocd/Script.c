/**
	AI Component: Task execution
	
	Use this component in your AI if you want it to be able to solve/execute tasks.
*/

/* -- AI Execution -- */

public func Execute(effect controller, int time)
{
	if (this->~QueryCallExecute(controller, controller.Target, time))
	{
		return FX_OK;
	}

	// handle all parallel tasks

	for (var task in controller->GetParallelTasks())
	{
		task->~Execute(controller, controller.Target);
	}
	
	// handle the active prioritized task - probably easier by passing the effect to the function
	
	var current_task = controller->GetCurrentTask() ?? controller->FindCurrentTask();
	if (current_task)
	{
		var status = current_task->~Execute(controller, controller.Target);
		
		if (status == TASK_EXECUTION_SUCCESS)
		{
			controller->~OnTaskSuccess(current_task);
			controller->FinishCurrentTask();
		}
		else if (status == TASK_EXECUTION_FAILURE)
		{
			controller->~OnTaskFailure(current_task);
			controller->FinishCurrentTask();
		}
	}

	return FX_OK;
}

/* -- Callbacks -- */

// Callback from the Definition()-call
public func OnDefineAI(proplist controller)
{
	_inherited(controller);
	
	// add functions to the AI
	controller->GetControlEffect()->AddFunctions(
	[
		// public interface
		this.AddTask,
		this.GetParallelTasks,
		this.GetPriorityTasks,
		this.GetCurrentTask,
		this.HasParallelTask,
		this.HasPriorityTask,
		this.GetAgent,
		this.SetAgent,
		// internal interface
		this.SetCurrentTask,
		this.FindCurrentTask,
		this.FinishCurrentTask,
		this.ResetCurrentTask
	]);

	// add properties to the AI
	var properties = {
		Tasks = {
			Parallel = [], // tasks that can be executed parallel
			Priority = [], // tasks that have to be executed by priority
			Current = nil, // task that has currently has the highest priority
			Agent = nil,   // definition that controls the behavior
		},
	};

	AddProperties(controller.FxAI, properties);
}


public func OnAddAI(proplist controller)
{
	_inherited(controller);

	// Reset the tasks
	controller.Tasks = {
			Parallel = [], // tasks that can be executed parallel
			Priority = [], // tasks that have to be executed by priority
			Current = nil, // task that has currently has the highest priority
			Agent = nil,   // definition that controls the behavior
	};
}


/* -- Public (AI Controller Effect) Interface -- */
// should not be called from definition context, but only from the AI itself

public func AddTask(proplist task, int priority)
{
	AssertNotNil(task);

	task->~SetPriority(priority);
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

public func HasParallelTask(id type)
{
	for (var task in this->GetParallelTasks())
	{
		if (type == task->GetTaskType())
		{
			return true;
		}
	}
	return false;
}

public func GetPriorityTasks()
{
	return this.Tasks.Priority;
}

public func HasPriorityTask(id type)
{
	for (var task in this->GetPriorityTasks())
	{
		if (type == task->GetTaskType())
		{
			return true;
		}
	}
	return false;
}

public func GetCurrentTask()
{
	return this.Tasks.Current;
}

public func GetAgent()
{
	return this.Tasks.Agent;
}


public func SetAgent(def agent)
{
	AssertNotNil(agent);

	this.Tasks.Agent = agent;
	// possibly leaves old agent properties if the agent changes
	agent->~Agent_Initialize(this.Target);
	return this;
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


private func ResetCurrentTask()
{
	// TODO: At the moment this simply overrides the current task
	// Should probably have some kind of handshake-mechanism, so
	// that the old task can perform a cleanup

	this.Tasks.Current = nil;
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
		for (var i = 0; i < GetLength(GetPriorityTasks()); ++i)
		{
			// update task index
			var task = GetPriorityTasks()[i];
			task.PriorityIndex = i;
		
			// potential candidate
			if (!current_task || current_task->GetPriority() < task->GetPriority())
			{
				current_task = task;
			}
		}
		
		// update actual current task
		if (GetCurrentTask() != current_task)
		{
			SetCurrentTask(current_task);
		}

		return task;
	}
}


private func FinishCurrentTask()
{
	RemoveArrayIndex(GetPriorityTasks(), GetCurrentTask().PriorityIndex);
	ResetCurrentTask();
}
