
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

private func CanAddCommand(object agent)
{
	if (agent->GetCommand()) return false;
	if (agent->GetAction() == "Walk") return true;
	
	return true;
}
