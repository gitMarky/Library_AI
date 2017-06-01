#appendto AI_Vehicles

/*-- Public interface --*/

// Set controlled vehicle
public func SetVehicle(object clonk, object new_vehicle)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetVehicle(%v, %v) not called from definition context but from %v", clonk, new_vehicle, this);
	var fx_ai = this->GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.vehicle = new_vehicle;
	return true;
}


/*-- Callbacks --*/

// Callback from the effect Construction()-call
public func OnAddAI(proplist fx_ai)
{
	_inherited(fx_ai);
	
	// Store the vehicle the AI is using.
	if (fx_ai.Target->GetProcedure() == "PUSH")
		fx_ai.vehicle = fx_ai.Target->GetActionTarget();
}
