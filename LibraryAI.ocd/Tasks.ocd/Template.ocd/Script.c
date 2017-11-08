
public func AddTo(object agent, int priority, proplist parent)
{
	AssertDefinitionContext();

	var controller = AssertHasAI(agent);

	var task = CreateTask();
	task->SetTaskType(this);

	if (task->GetTaskID())
	{
		FatalError(Format("Newly created task already has ID %d", task->GetTaskID()));
	}
	else
	{
		++Debug_TaskCounter;
		task->SetTaskID(Debug_TaskCounter);
		AI_Debugging->LogAI_Info(controller, Format("Created task with ID %d", task->GetTaskID()));
	}

	controller->AddTask(task, priority);
	
	if (parent)
	{
		task->SetParentTask(parent);
	}

	return task;
}

private func CreateTask()
{
	return new Task
	{
		Execute = this.Execute,
	};
}
