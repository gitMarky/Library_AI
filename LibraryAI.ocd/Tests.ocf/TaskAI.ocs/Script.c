
protected func InitializePlayer(int player)
{
	// Set zoom and move player to the middle of the scenario.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	SetFoW(false, player);
	GetCrew(player)->SetPosition(120, 190);
	GetCrew(player)->MakeInvincible();
	
	// Add test control effect.
	LaunchTest(2);
	return true;
}

/*-- Helpers --*/

global func OnTaskSuccess(proplist task)
{
	CurrentTest().TaskSuccess = true;
}

global func SetupAgent(object bot)
{
	bot = bot ?? CurrentTest().Bot;
	CurrentTest().AddedAI = true;

	var ai = AI_Local->AddAI(bot);
	ai->SetAgent(AI_Agent_Local);
	ai.OnTaskSuccess = Global.OnTaskSuccess;
	
	Log("After creating the AI in %v:", bot);
	Log("- Current task: %v", ai->GetCurrentTask());
	Log("- Priority task: %v", ai->GetPriorityTasks());
	Log("- Parallel task: %v", ai->GetParallelTasks());
}

global func OnTestFinished()
{
	// Default cleanup
	if (CurrentTest().Bot) CurrentTest().Bot->RemoveObject();
	if (CurrentTest().Bot2) CurrentTest().Bot2->RemoveObject();
	if (CurrentTest().Item) CurrentTest().Item->RemoveObject();
	CurrentTest().AddedAI = false;
	CurrentTest().TaskSuccess = false;
	
	if (CurrentTest().AdditionalObjects)
	{
		for (var obj in CurrentTest().AdditionalObjects)
			if (obj)
				obj->RemoveObject();
	}
}

/*-- Tests --*/

///////////////////////////////////////////////////////////////////////////////////////////////////

global func Test1_OnStart(int player)
{
	Log("-- Test 1: Run a GetItem-Task, item is in inventory --");

	CurrentTest().Bot = CreateObject(Clonk, LandscapeWidth() / 2, 260, NO_OWNER);
	CurrentTest().Item = CurrentTest().Bot->CreateContents(Rock);
	Wait(10); // wait a little afterwards so that the test does not start right away
	return true;
}

global func Test1_Execute()
{
	if (CurrentTest().AddedAI)
	{
		if (CurrentTest().TaskSuccess)
		{
			return PassTest();
		}
		else
		{
			return Wait();
		}
	}
	else
	{
		SetupAgent();
		
		Task_GetItem->AddTo(CurrentTest().Bot, 1)->SetItem(Rock);
		return Wait();
	}
}

global func Test1_OnFinished()
{
	OnTestFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

global func Test2_OnStart(int player)
{
	Log("-- Test 2: Run a GetItem-Task, item is in the open --");

	CurrentTest().Bot = CreateObject(Clonk, LandscapeWidth() / 2, 260, NO_OWNER);
	CurrentTest().Item = CreateObject(Rock, LandscapeWidth() - 20, 190, NO_OWNER);
	Wait(10); // wait a little afterwards so that the test does not start right away
	return true;
}

global func Test2_Execute()
{
	if (CurrentTest().AddedAI)
	{
		if (CurrentTest().TaskSuccess)
		{
			return PassTest();
		}
		else
		{
			return Wait();
		}
	}
	else
	{
		SetupAgent();
		
		Task_GetItem->AddTo(CurrentTest().Bot, 1)->SetItem(Rock);
		return Wait();
	}
}

global func Test2_OnFinished()
{
	OnTestFinished();
}


///////////////////////////////////////////////////////////////////////////////////////////////////

global func Test3_OnStart(int player)
{
	Log("-- Test 3: Run a GetItem-Task, item is a specific item --");

	CurrentTest().Bot = CreateObject(Clonk, LandscapeWidth() / 2, 260, NO_OWNER);
	CurrentTest().Item = CreateObject(Rock, CurrentTest().Bot->GetX() - 50, 265, NO_OWNER);
	CurrentTest().AdditionalObjects = [];
	PushBack(CurrentTest().AdditionalObjects, CreateObject(Rock, CurrentTest().Bot->GetX() - 20, 265, NO_OWNER));
	PushBack(CurrentTest().AdditionalObjects, CreateObject(Rock, CurrentTest().Bot->GetX() + 10, 265, NO_OWNER));
	Wait(50); // wait a little afterwards so that the test does not start right away
	return true;
}

global func Test3_Execute()
{
	if (CurrentTest().AddedAI)
	{
		if (CurrentTest().TaskSuccess)
		{
			if (CurrentTest().Item->Contained() == CurrentTest().Bot)
			{
				return PassTest("The agent collected the specified item");
			}
			else
			{
				return FailTest("The agent collected another item, instead of the specified item");
			}
		}
		else
		{
			return Wait();
		}
	}
	else
	{
		SetupAgent();

		var task = Task_GetItem->AddTo(CurrentTest().Bot, 1);
		Log("%v", task->GetItem());
		task->SetItem(CurrentTest().Item); // set a specific item
		Log("%v", task->GetItem());
		return Wait();
	}
}

global func Test3_OnFinished()
{
	OnTestFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

global func Test4_OnStart(int player)
{
	Log("-- Test 4: Run a GetItem-Task, with two clonks --");

	CurrentTest().Bot = CreateObject(Clonk, +10, 190, NO_OWNER);
	CurrentTest().Bot2 = CreateObject(Clonk, LandscapeWidth() - 10, 190, NO_OWNER);
	CurrentTest().AdditionalObjects = [];
	PushBack(CurrentTest().AdditionalObjects, CreateObject(Wood, -20 + LandscapeWidth() / 2, 265, NO_OWNER));
	PushBack(CurrentTest().AdditionalObjects, CreateObject(Metal, +20 + LandscapeWidth() / 2, 265, NO_OWNER));
	Wait(10); // wait a little afterwards so that the test does not start right away
	return true;
}

global func Test4_Execute()
{
	if (CurrentTest().AddedAI)
	{
		if (CurrentTest().TaskSuccess)
		{
			var wood = FindObject(Find_ID(Wood))->Contained() == CurrentTest().Bot2;
			var metal = FindObject(Find_ID(Metal))->Contained() == CurrentTest().Bot;
			if (wood && metal)
			{
				return PassTest("The agent collected the specified item");
			}
			else
			{
				return FailTest("The agent collected another item, instead of the specified item");
			}
		}
		else
		{
			return Wait();
		}
	}
	else
	{
		SetupAgent(CurrentTest().Bot);
		SetupAgent(CurrentTest().Bot2);

		var task = Task_GetItem->AddTo(CurrentTest().Bot, 2);
		Log("%v", task->GetItem());
		task->SetItem(Metal);
		Log("%v", task->GetItem());

		var task = Task_GetItem->AddTo(CurrentTest().Bot2, 2);
		Log("%v", task->GetItem());
		task->SetItem(Wood);
		Log("%v", task->GetItem());

		return Wait();
	}
}

global func Test4_OnFinished()
{
	OnTestFinished();
}
