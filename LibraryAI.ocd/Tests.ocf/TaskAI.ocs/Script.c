
protected func InitializePlayer(int player)
{
	// Set zoom and move player to the middle of the scenario.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	SetFoW(false, player);
	GetCrew(player)->SetPosition(120, 190);
	GetCrew(player)->MakeInvincible();
	
	// Add test control effect.
	LaunchTest(1);
	return true;
}

/*-- Helpers --*/

global func OnTaskSuccess(proplist task)
{
	CurrentTest().TaskSuccess = true;
}

global func OnTestFinished()
{
	// Default cleanup
	if (CurrentTest().Bot) CurrentTest().Bot->RemoveObject();
	if (CurrentTest().Item) CurrentTest().Item->RemoveObject();
	CurrentTest().AddedAI = false;
	CurrentTest().TaskSuccess = false;
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
		CurrentTest().AddedAI = true;
	
		var ai = AI_Local->AddAI(CurrentTest().Bot);
		ai->SetAgent(AI_Agent_Local);
		ai.OnTaskSuccess = Global.OnTaskSuccess;
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
		CurrentTest().AddedAI = true;
	
		var ai = AI_Local->AddAI(CurrentTest().Bot);
		ai->SetAgent(AI_Agent_Local);
		ai.OnTaskSuccess = Global.OnTaskSuccess;
		Task_GetItem->AddTo(CurrentTest().Bot, 1)->SetItem(Rock);
		return Wait();
	}
}

global func Test2_OnFinished()
{
	OnTestFinished();
}
