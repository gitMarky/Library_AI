#appendto AI_AttackModes

/*-- Callbacks --*/

// Callback from the effect Construction()-call
public func OnAddAI(proplist fx_ai)
{
	_inherited(fx_ai);

	// Add AI default settings.	
	SetAttackMode(fx_ai.Target, "Default"); // also binds inventory
}


// Callback from the effect SaveScen()-call
public func OnSaveScenarioAI(proplist fx_ai, proplist props)
{
	_inherited(fx_ai, props);

	if (fx_ai.attack_mode.Identifier != "Default")
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAttackMode", fx_ai.Target, Format("%v", fx_ai.attack_mode.Identifier));
}
