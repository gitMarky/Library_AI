
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

global func Test1_OnStart(int player){ return true;}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	Log("Test the behaviour of ...");

	if (doTest("Something", true, true))
	{
		return PassTest();
	}

	return Wait();
}
