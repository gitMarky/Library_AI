
public func AddTo(object agent, int priority)
{
	AssertDefinitionContext();

	var controller = AssertHasAI(agent);

	var task = CreateTask();

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
