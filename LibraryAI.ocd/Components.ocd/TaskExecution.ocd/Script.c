/**
	AI Component: Task execution
	
	Use this component in your AI if you want it to be able to solve/execute tasks.

	@author Marky
	@version 0.2.0
*/

/* -- AI Execution -- */


/**
 This is the AI execution logic. Does nothing if the callback
 {@code QueryCallExecute} returns {@code true}.

 Execution order is as follows:
 <ol>
 <li>All parallel tasks get handled - these are usually used for ongoing behavior, such as avoiding danger.</li>
 <li>The currently active task gets handled - this is usually used for an action that can be completed, such as getting an item.
    <ul><li>The callbacks {@code OnTaskSuccess(effect controller, object agent)} and {@code OnTaskSuccess(proplist task)} are issued to the task and AI effect respectively</li>
        <li>The callbacks {@code OnTaskFailure(effect controller, object agent)} and {@code OnTaskFailure(proplist task)} are issued to the task and AI effect respectively</li>
    </ul>
 </li>
 </ol>

 @par controller This is the AI effect that is added via {@link AI_Controller#AddAI}.
 @par time The effect running time.

 @version 0.2.0
*/
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
			current_task->~OnTaskSuccess(controller, controller.Target);
			controller->~OnTaskSuccess(current_task);
			controller->FinishCurrentTask();
		}
		else if (status == TASK_EXECUTION_FAILURE)
		{
			current_task->~OnTaskFailure(controller, controller.Target);
			controller->~OnTaskFailure(current_task);
			controller->FinishCurrentTask();
		}
	}

	return FX_OK;
}

/* -- Callbacks -- */

/**
 Callback from the Definition()-call of the AI. Defines the internal
 structure and functions of the AI effect.

 @par controller This is the AI control effect.

 @version 0.2.0
*/
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


/**
 Callback from the AI effect.

 @par controller This is the AI control effect.

 @version 0.2.0
*/
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


/**
 Adds a task. Usually you do not call this method yourself, but use
 the task definition to do so.

 @par task This task is added.
 @par priority This is the priority of the task. A priority of 0 means
               that the task will be executed as a parallel task, that
               is at the same time as other tasks.
               Otherwise, the tasks will be executed in order of priority,
               where those with a higher priority value are executed first.
               This also changes the current task if the new task has a
               higher priority.

 @version 0.2.0
*/
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


/**
 Gets the tasks without priority for this AI.

 @return array All parallel tasks, or an empty array.

 @version 0.2.0
*/
public func GetParallelTasks()
{
	return this.Tasks.Parallel;
}


/**
 Finds out whether this AI has a task of a given type without priority.

 @par type The task type.
 @return {@code true} if it has such a task.
 @version 0.2.0
*/
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


/**
 Gets the tasks with a priority of this AI.

 @return array All tasks with priority, or an empty array.

 @version 0.2.0
*/
public func GetPriorityTasks()
{
	return this.Tasks.Priority;
}


/**
 Finds out whether this AI has a task of a given type with priority.

 @par type The task type.
 @return {@code true} if it has such a task.
 @version 0.2.0
*/
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


/**
 Gets the task with priority that is currently being executed.

 @return proplist The task.
 @version 0.2.0
*/
public func GetCurrentTask()
{
	return this.Tasks.Current;
}


/**
 Gets a task with a specific ID. Each task ID is unique.
 
 @par nr The task ID.
 @return proplist The task, or {@code nil} if this controller
         has no such task.

 @version 0.2.0
 */
public func GetTaskByID(int nr)
{
	for (var task in GetPriorityTasks())
	{
	}
}


/**
 Gets the definition that defines the logic and capabilities
 of how tasks are executed.

 @see {@link AI_TaskExecutor#SetAgent}.
 @return object The agent logic definition.
 @version 0.2.0
*/
public func GetAgent()
{
	return this.Tasks.Agent;
}


/**
 Changes the logic that executes the tasks.

 @par agent The agent logic definition. While a task 
            contains the logic what to do, the agent
            contains the logic if and how to do it.
 @return {@code true} if it has such a task.
 @version 0.2.0
*/
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
