#appendto AI_Movement


/*-- Public interface --*/

// Set attack path
public func SetAttackPath(object clonk, array new_attack_path)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetAttackPath(%v, %v) not called from definition context but from %v", clonk, new_attack_path, this);
	var fx_ai = this->GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.attack_path = new_attack_path;
	return true;
}


/*-- Callbacks --*/

// Callback from the effect SaveScen()-call
public func OnSaveScenarioAI(proplist fx_ai, proplist props)
{
	_inherited(fx_ai, props);

	if (fx_ai.attack_path)
		props->AddCall(SAVESCEN_ID_AI, fx_ai.control, "SetAttackPath", fx_ai.Target, fx_ai.attack_path);

}


/*-- Editor Properties --*/

// Callback from the Definition()-call
public func OnDefineAI(proplist def)
{
	_inherited(def);
	
	def.FxAI.SetAttackPath = this.EditorDelegate_SetAttackPath;
	
	// Set the additional editor properties
	var additional_props =
	{
		ignore_allies = { Name = "$IgnoreAllies$", Type = "bool" },
		active = { Name = "$Active$", EditorHelp = "$ActiveHelp$", Type = "bool", Priority = 50, AsyncGet = "GetActive", Set = "SetActive" },
		attack_path = { Name = "$AttackPath$", EditorHelp = "$AttackPathHelp$", Type = "enum", Set = "SetAttackPath", Options = [
			{ Name="$None$" },
			{ Name="$AttackPath$", Type=C4V_Array, Value = [{X = 0, Y = 0}], Delegate =
				{ Name="$AttackPath$", EditorHelp="$AttackPathHelp$", Type="polyline", StartFromObject=true, DrawArrows=true, Color=0xdf0000, Relative=false }
			}
		] },
	};
	
	AddProperties(def.FxAI.EditorProps, additional_props);
}


func EditorDelegate_SetAttackPath(array attack_path)
{
	// Called by editor delegate when attack mode is changed.
	// For now, attack mode parameter delegates are not supported. Just set by name.
	return this.control->SetAttackPath(this.Target, attack_path);
}
