/**
	Open Clonk AI implemented with this library.
*/

// Timer interval for the effect
public func GetTimerInterval(){	return 3;}


// Callback from the effect Construction()-call
public func OnAddAI(proplist fx_ai)
{
	// Set the timer interval (the controller has an interval of 1)
	fx_ai.Interval = 3;
	// Store the vehicle the AI is using.
	if (fx_ai.Target->GetProcedure() == "PUSH")
		fx_ai.vehicle = fx_ai.Target->GetActionTarget();
	// Store whether the enemy is controlled by a commander.
	this.commander = this.Target.commander;

	// Add AI default settings.	
	SetAttackMode(fx_ai.Target, "Default"); // also binds inventory
	SetHome(fx_ai.Target);
	SetGuardRange(fx_ai.Target, fx_ai.home_x - this.GuardRangeX, fx_ai.home_y - this.GuardRangeY, this.GuardRangeX * 2, this.GuardRangeY * 2);
	SetMaxAggroDistance(fx_ai.Target, this.MaxAggroDistance);
	SetAutoSearchTarget(fx_ai.Target, true);
}


// Callback from the effect Destruction()-call
public func OnRemoveAI(proplist fx_ai, int reason)
{
	// Remove weapons on death.
	if (reason == FX_Call_RemoveDeath)
	{
		if (fx_ai.bound_weapons)
			for (var obj in fx_ai.bound_weapons)
				if (obj && obj->Contained() == fx_ai.Target)
					obj->RemoveObject();
	}
}
