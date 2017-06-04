
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

/*-- Tests --*/

global func Test1_OnStart(int player)
{
	Log("-- Test 1: Task execution AI can execute a task --");

	CurrentTest().Bot = CreateObject(Clonk, 80, 190, NO_OWNER);
	CurrentTest().Item = CreateObject(Rock, 100, 190, NO_OWNER);
	
	AI_Local->AddAI(CurrentTest().Bot);
	Task_GetItem->AddTo(CurrentTest().Bot)->SetItem(Rock)->SetPriority(1);
	return true;
}

global func Test1_Execute()
{
	return Wait();
}

global func Test1_OnFinished()
{
	CurrentTest().Bot->RemoveObject();
	CurrentTest().ITem->RemoveObject();
}
