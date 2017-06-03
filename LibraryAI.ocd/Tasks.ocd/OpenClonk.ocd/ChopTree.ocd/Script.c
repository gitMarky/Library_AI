
public func AddTo(object agent)
{
	AssertDefinitionContext();
	
	var controller = AssertHasAI(agent);
	
	var chop_tree = new Task
	{
		Execute = this.Execute,
	};

	controller->AddTask(chop_tree);
}


public func Execute(proplist controller, object agent)
{
	agent->Message("I want to chop a tree, dumdidum");
}
