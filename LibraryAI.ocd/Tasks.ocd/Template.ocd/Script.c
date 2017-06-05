
public func AddTo(object agent, int priority)
{
	AssertDefinitionContext();

	var controller = AssertHasAI(agent);

	var task = CreateTask();
	
	if (task.TaskID)
	{
		Log("Newly created task already has ID %d", task.TaskID);
	}
	else
	{
		++Debug_TaskCounter;
		task.TaskID = Debug_TaskCounter;
		Log("Created task with ID %d", task.TaskID);
	}

	controller->AddTask(task, priority);
	
	return task;
}

private func CreateTask()
{
	return new Task
	{
			Execute = this.Execute,
	};
}
