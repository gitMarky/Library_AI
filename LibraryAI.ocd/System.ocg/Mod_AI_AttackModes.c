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


/*-- Editor Properties --*/

// Callback from the Definition()-call
public func OnDefineAI(proplist def)
{
	_inherited(def);
	
	def.FxAI.SetAttackMode = this.EditorDelegate_SetAttackMode;
	
	// Set the other options
	def->DefinitionAttackModes(def);
}


func EditorDelegate_SetAttackMode(proplist attack_mode)
{
	// Called by editor delegate when attack mode is changed.
	// For now, attack mode parameter delegates are not supported. Just set by name.
	return this.control->SetAttackMode(this.Target, attack_mode.Identifier);
}
