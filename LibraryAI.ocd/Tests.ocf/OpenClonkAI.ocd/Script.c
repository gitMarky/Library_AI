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
	fx_ai.commander = fx_ai.Target.commander;

	// Add AI default settings.	
	SetAttackMode(fx_ai.Target, "Default"); // also binds inventory
	SetHome(fx_ai.Target);
	SetGuardRange(fx_ai.Target, fx_ai.home_x - fx_ai.GuardRangeX, fx_ai.home_y - fx_ai.GuardRangeY, fx_ai.GuardRangeX * 2, fx_ai.GuardRangeY * 2);
	SetMaxAggroDistance(fx_ai.Target, fx_ai.MaxAggroDistance);
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


// Callback from the effect SaveScen()-call
public func OnSaveScenarioAI(proplist fx_ai, proplist props)
{
	if (fx_ai.attack_mode.Identifier != "Default")
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAttackMode", fx_ai.Target, Format("%v", fx_ai.attack_mode.Identifier));
	if (fx_ai.attack_path)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAttackPath", fx_ai.Target, fx_ai.attack_path);
	if (fx_ai.home_x != fx_ai.Target->GetX() || fx_ai.home_y != fx_ai.Target->GetY() || fx_ai.home_dir != fx_ai.Target->GetDir())
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetHome", fx_ai.Target, fx_ai.home_x, fx_ai.home_y, GetConstantNameByValueSafe(fx_ai.home_dir, "DIR_"));
	props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetGuardRange", fx_ai.Target, fx_ai.guard_range.x, fx_ai.guard_range.y, fx_ai.guard_range.wdt, fx_ai.guard_range.hgt);
	if (fx_ai.max_aggro_distance != fx_ai.control.MaxAggroDistance)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetMaxAggroDistance", fx_ai.Target, fx_ai.max_aggro_distance);
	if (fx_ai.ally_alert_range)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAllyAlertRange", fx_ai.Target, fx_ai.ally_alert_range);
	if (!fx_ai.auto_search_target)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAutoSearchTarget", fx_ai.Target, false);
	if (fx_ai.encounter_cb)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetEncounterCB", fx_ai.Target, Format("%v", fx_ai.encounter_cb));
}

