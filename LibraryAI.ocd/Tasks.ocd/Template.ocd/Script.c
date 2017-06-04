
public func AddTo(object agent)
{
	AssertDefinitionContext();

	var controller = AssertHasAI(agent);

	var task = CreateTask();

	controller->AddTask(task);
	
	return task;
}

private func CreateTask()
{
	return new Task
	{
			Execute = this.Execute,
	};
}
