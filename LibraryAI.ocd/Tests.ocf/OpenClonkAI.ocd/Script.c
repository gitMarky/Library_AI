/**
	Open Clonk AI implemented with this library.
*/

// Include the different parts of the AI.
#include AI_Appearance
#include AI_Debugging
#include AI_HelperFunctions
#include AI_MeleeWeapons
#include AI_Movement
#include AI_Protection
#include AI_RangedWeapons
#include AI_TargetFinding
#include AI_Vehicles
#include AI_AttackModes
// Include the basic functionality
#include AI_Controller
// Include additional components
#include AI_Helper_Clonk

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


/*-- AI Execution --*/

public func Execute(effect fx, int time)
{
	fx.time = time;
	// Evasion, healing etc. if alert.
	if (fx.alert)
		if (this->ExecuteProtection(fx))
			return true;
	// Current command override.
	if (fx.command)
	{
		if (this->Call(fx.command, fx))
			return true;
		else
			fx.command = nil;
	}
	// Find something to fight with.
	if (!fx.weapon) 
	{
		fx.can_attack_structures = false;
		this->CancelAiming(fx);
		if (!this->ExecuteArm(fx))
			return this->ExecuteIdle(fx);
		else if (!fx.weapon)
			return true;
	}
	// Weapon out of ammo?
	if (fx.ammo_check && !this->Call(fx.ammo_check, fx, fx.weapon))
	{
		fx.weapon = nil;
		this->LogAI(fx, Format("weapon %v is out of ammo, AI won't do anything.", fx.weapon));
		return false;
	}
	// Find an enemy.
	if (fx.target) 
		if ((fx.target->GetCategory() & C4D_Living && !fx.target->GetAlive()) || (!fx.ranged && fx.Target->ObjectDistance(fx.target) >= fx.max_aggro_distance))
			fx.target = nil;
	if (!fx.target)
	{
		this->CancelAiming(fx);
		if (!fx.auto_search_target || !(fx.target = this->FindTarget(fx)))
			return ExecuteIdle(fx);
		// First encounter callback. might display a message.
		if (fx.encounter_cb)
			if (GameCall(fx.encounter_cb, fx.Target, fx.target))
				fx.encounter_cb = nil;
		// Wake up nearby allies.
		if (fx.ally_alert_range)
		{
			var ally_fx;
			for (var ally in fx.Target->FindObjects(Find_Distance(fx.ally_alert_range), Find_Exclude(fx.Target), Find_OCF(OCF_CrewMember), Find_Owner(fx.Target->GetOwner())))
				if (ally_fx = this->GetAI(ally))
					if (!ally_fx.target)
					{
						ally_fx.target = fx.target;
						ally_fx.alert = ally_fx.time;
						if (ally_fx.encounter_cb) 
							if (GameCall(ally_fx.encounter_cb, ally, fx.target))
								ally_fx.encounter_cb = nil;
					}
			// Do some messages.
			this->ExecuteIntruderMessage(fx);		
			// Waking up works only once. after that, AI might have moved and wake up clonks it shouldn't.
			fx.ally_alert_range = nil;
		}
	}
	// Do stuff on the appearance of the enemy like displaying a message.
	this->ExecuteAppearance(fx);
	// Attack it!
	if (!this->IsWeaponForTarget(fx))
		this->LogAI(fx, Format("weapon of type %i is not fit to attack %v (type: %i).", fx.weapon->GetID(), fx.target, fx.target->GetID()));
	return this->Call(fx.strategy, fx);
}


public func ExecuteThrow(effect fx)
{
	// Still carrying the weapon to throw?
	if (fx.weapon->Contained() != fx.Target)
	{
		fx.weapon = nil;
		return false;
	}
	// Path to target free?
	var x = fx.Target->GetX(), y = fx.Target->GetY(), tx = fx.target->GetX(), ty = fx.target->GetY();
	if (PathFree(x, y, tx, ty))
	{
		var throw_speed = fx.Target.ThrowSpeed;
		var rx = (throw_speed * throw_speed) / (100 * GetGravity()); // horizontal range for 45 degree throw if enemy is on same height as we are
		var ry = throw_speed * 7 / (GetGravity() * 10); // vertical range of 45 degree throw
		var dx = tx - x, dy = ty - y + 15 * fx.Target->GetCon() / 100; // distance to target. Reduce vertical distance a bit because throwing exit point is not at center
		// Check range
		// Could calculate the optimal parabulum here, but that's actually not very reliable on moving targets
		// It's usually better to throw straight at the target and only throw upwards a bit if the target stands on high ground or is far away
		// Also ignoring speed added by own velocity, etc...
		if (Abs(dx) * ry - Min(dy) * rx <= rx * ry)
		{
			// We're in range. Can throw?
			if (!this->CheckHandsAction(fx))
				return true;
			// OK. Calc throwing direction.
			dy -= dx * dx / rx;
			// And throw!
			fx.Target->SetCommand("None");
			fx.Target->SetComDir(COMD_Stop);
			this->SelectItem(fx, fx.weapon);
			return fx.Target->ControlThrow(fx.weapon, dx, dy);
		}
	}
	// Can't reach target yet. Walk towards it.
	if (!fx.Target->GetCommand() || !Random(3))
		fx.Target->SetCommand("MoveTo", fx.target);
	return true;
}


public func ExecuteArm(effect fx)
{
	// Find shield.
	fx.shield = fx.Target->FindContents(Shield);
	// Vehicle control overrides all other weapons
	if (fx.weapon = fx.vehicle)
	{
		if (this->CheckVehicleAmmo(fx, fx.weapon))
		{
			fx.strategy = this.ExecuteVehicle;
			fx.ranged = true;
			fx.aim_wait = 20;
			fx.ammo_check = this.CheckVehicleAmmo;
			return true;
		}
		else
			fx.weapon = nil;
	}
	// Find a weapon. Depends on attack mode
	if (Call(fx.attack_mode.FindWeapon, fx))
	{
		// Select unless it's e.g. a vehicle or a spell
		SelectItem(fx, fx.weapon);
		return true;
	}
	// No weapon.
	return false;
}


public func FindInventoryWeapon(effect fx)
{
	// Find weapon in inventory, mark it as equipped and set according strategy, etc.
	fx.ammo_check = nil;
	fx.ranged = false;
	if (FindInventoryWeaponGrenadeLauncher(fx)) return true;
	if (FindInventoryWeaponBlunderbuss(fx)) return true;
	if (FindInventoryWeaponBow(fx)) return true;
	if (FindInventoryWeaponJavelin(fx)) return true;
	// Throwing weapons.
	if ((fx.weapon = fx.Target->FindContents(Firestone)) || (fx.weapon = fx.Target->FindContents(Rock)) || (fx.weapon = fx.Target->FindContents(Lantern))) 
	{
		fx.can_attack_structures = fx.weapon->~HasExplosionOnImpact();
		fx.strategy = this.ExecuteThrow;
		return true;
	}
	// Melee weapons.
	if ((fx.weapon = fx.Target->FindContents(Sword)) || (fx.weapon = fx.Target->FindContents(Axe))) // Sword attacks aren't 100% correct for Axe, but work well enough
	{
		fx.strategy = this.ExecuteMelee;
		return true;
	}
	if ((fx.weapon = fx.Target->FindContents(PowderKeg)))
	{
		fx.strategy = this.ExecuteBomber;
		return true;
	}
	if ((fx.weapon = fx.Target->FindContents(Club)))
	{
		fx.strategy = this.ExecuteClub;
		return true;
	}
	// No weapon.
	return false;
}


private func FindInventoryWeaponGrenadeLauncher(effect fx)
{
	if (fx.weapon = fx.Target->FindContents(GrenadeLauncher))
	{
		if (this->HasBombs(fx, fx.weapon))
		{
			fx.strategy = this.ExecuteRanged;
			fx.projectile_speed = 75;
			fx.ammo_check = this.HasBombs;
			fx.ranged = true;
			fx.can_attack_structures = true;
			return true;
		}
		else
			fx.weapon = nil;
	}
}


private func FindInventoryWeaponBlunderbuss(effect fx)
{
	if (fx.weapon = fx.Target->FindContents(Blunderbuss))
	{
		if (this->HasAmmo(fx, fx.weapon))
		{
			fx.strategy = this.ExecuteRanged;
			fx.projectile_speed = 200;
			fx.ammo_check = this.HasAmmo;
			fx.ranged = true;
			fx.ranged_direct = true;
			return true;
		}
		else
			fx.weapon = nil;
	}
}


private func FindInventoryWeaponBow(effect fx)
{
	if (fx.weapon = fx.Target->FindContents(Bow))
	{
		if (this->HasArrows(fx, fx.weapon))
		{
			fx.strategy = this.ExecuteRanged;
			fx.projectile_speed = 100;
			fx.ammo_check = this.HasArrows;
			fx.ranged = true;
			var arrow = fx.weapon->Contents(0) ?? FindObject(Find_Container(fx.Target), Find_Func("IsArrow"));
			fx.can_attack_structures = arrow && arrow->~IsExplosive();
			return true;
		}
		else
			fx.weapon = nil;
	}
}


private func FindInventoryWeaponJavelin(effect fx)
{
	if (fx.weapon = fx.Target->FindContents(Javelin)) 
	{
		fx.strategy = this.ExecuteRanged;
		fx.projectile_speed = fx.Target.ThrowSpeed * fx.weapon.shooting_strength / 100;
		fx.ranged=true;
		return true;
	}
}

// -------------------------------------------------------------------------------------------------

// Enemy spawn definition depends on this
local DefinitionPriority = 50;

// AI Settings.
local MaxAggroDistance = 200; // Lose sight to target if it is this far away (unless we're ranged - then always guard the range rect).
local GuardRangeX = 300; // Search targets this far away in either direction (searching in rectangle).
local GuardRangeY = 150; // Search targets this far away in either direction (searching in rectangle).

/*-- Public interface --*/

// Set the current inventory to be removed when the clonk dies. Only works if clonk has an AI.
public func BindInventory(object clonk)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: BindInventory(%v) not called from definition context but from %v", clonk, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	var cnt = clonk->ContentsCount();
	fx_ai.bound_weapons = CreateArray(cnt);
	for (var i = 0; i < cnt; ++i)
		fx_ai.bound_weapons[i] = clonk->Contents(i);
	return true;
}


// Set the home position the Clonk returns to if he has no target.
public func SetHome(object clonk, int x, int y, int dir)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetHome(%v, %d, %d, %d) not called from definition context but from %v", clonk, x, y, dir, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	// nil/nil defaults to current position.
	if (!GetType(x))
		x = clonk->GetX();
	if (!GetType(y))
		y = clonk->GetY();
	if (!GetType(dir))
		dir = clonk->GetDir();
	fx_ai.home_x = x;
	fx_ai.home_y = y;
	fx_ai.home_dir = dir;
	return true;
}


// Enable/disable auto-searching of targets.
public func SetAutoSearchTarget(object clonk, bool new_auto_search_target)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetAutoSearchTarget(%v, %v) not called from definition context but from %v", clonk, new_auto_search_target, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.auto_search_target = new_auto_search_target;
	return true;
}


// Set the guard range to the provided rectangle.
public func SetGuardRange(object clonk, int x, int y, int wdt, int hgt)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetGuardRange(%v, %d, %d, %d, %d) not called from definition context but from %v", clonk, x, y, wdt, hgt, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	// Clip to landscape size.
	if (x < 0)
	{
		wdt += x;
		x = 0;
	}
	if (y < 0)
	{
		hgt += y;
		y = 0;
	}
	wdt = Min(wdt, LandscapeWidth() - x);
	hgt = Min(hgt, LandscapeHeight() - y);
	fx_ai.guard_range = {x = x, y = y, wdt = wdt, hgt = hgt};
	return true;
}


// Set the maximum distance the enemy will follow an attacking clonk.
public func SetMaxAggroDistance(object clonk, int max_dist)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetMaxAggroDistance(%v, %d) not called from definition context but from %v", clonk, max_dist, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.max_aggro_distance = max_dist;
	return true;
}


// Set range in which, on first encounter, allied AI clonks get the same aggro target set.
public func SetAllyAlertRange(object clonk, int new_range)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetAllyAlertRange(%v, %d) not called from definition context but from %v", clonk, new_range, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.ally_alert_range = new_range;
	return true;
}


// Set callback function name to be called in game script when this AI is first encountered
// Callback function first parameter is (this) AI clonk, second parameter is player clonk.
// The callback should return true to be cleared and not called again. Otherwise, it will be called every time a new target is found.
public func SetEncounterCB(object clonk, string cb_fn)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetEncounterCB(%v, %s) not called from definition context but from %v", clonk, cb_fn, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.encounter_cb = cb_fn;
	return true;
}


// Set attack path
public func SetAttackPath(object clonk, array new_attack_path)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetAttackPath(%v, %v) not called from definition context but from %v", clonk, new_attack_path, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.attack_path = new_attack_path;
	return true;
}


// Set controlled vehicle
public func SetVehicle(object clonk, object new_vehicle)
{
	if (GetType(this) != C4V_Def)
		Log("WARNING: SetVehicle(%v, %v) not called from definition context but from %v", clonk, new_vehicle, this);
	var fx_ai = GetAI(clonk);
	if (!fx_ai)
		return false;
	fx_ai.vehicle = new_vehicle;
	return true;
}


local FxAI_OC = new Effect
{	
	SetAttackMode = func(proplist attack_mode)
	{
		// Called by editor delegate when attack mode is changed.
		// For now, attack mode parameter delegates are not supported. Just set by name.
		return this.control->SetAttackMode(this.Target, attack_mode.Identifier);
	},
	SetAttackPath = func(array attack_path)
	{
		// Called by editor delegate when attack path is changed.
		return this.control->SetAttackPath(this.Target, attack_path);
	},
	EditorProps = {
		guard_range = { Name = "$GuardRange$", Type = "rect", Storage = "proplist", Color = 0xff00, Relative = false },
		ignore_allies = { Name = "$IgnoreAllies$", Type = "bool" },
		max_aggro_distance = { Name = "$MaxAggroDistance$", Type = "circle", Color = 0x808080 },
		active = { Name = "$Active$", EditorHelp = "$ActiveHelp$", Type = "bool", Priority = 50, AsyncGet = "GetActive", Set = "SetActive" },
		auto_search_target = { Name = "$AutoSearchTarget$", EditorHelp = "$AutoSearchTargetHelp$", Type = "bool" },
		attack_path = { Name = "$AttackPath$", EditorHelp = "$AttackPathHelp$", Type = "enum", Set = "SetAttackPath", Options = [
			{ Name="$None$" },
			{ Name="$AttackPath$", Type=C4V_Array, Value = [{X = 0, Y = 0}], Delegate =
				{ Name="$AttackPath$", EditorHelp="$AttackPathHelp$", Type="polyline", StartFromObject=true, DrawArrows=true, Color=0xdf0000, Relative=false }
			}
		] }
	},
};


/*-- Editor Properties --*/

// Callback from the Definition()-call
public func OnDefineAI(proplist def)
{
	_inherited(def);
	
	// Can be added to Clonk
	AddEditorProp_AISelection(Clonk, AI_OpenClonk);

	def->DefinitionAttackModes(def);
	// Add AI user actions.
	var enemy_evaluator = UserAction->GetObjectEvaluator("IsClonk", "$Enemy$", "$EnemyHelp$");
	enemy_evaluator.Priority = 100;
	UserAction->AddEvaluator("Action", "Clonk", "$SetAIActivated$", "$SetAIActivatedHelp$", "ai_set_activated", [def, def.EvalAct_SetActive], 
		{
			Enemy = nil,
			AttackTarget = {
				Function= "triggering_clonk"
			},
			Status = {
				Function = "bool_constant",
				Value = true
			}
		},
		{
			Type = "proplist",
			Display = "{{Enemy}}: {{Status}} ({{AttackTarget}})",
			EditorProps = {
				Enemy = enemy_evaluator,
				AttackTarget = UserAction->GetObjectEvaluator("IsClonk", "$AttackTarget$", "$AttackTargetHelp$"),
				Status = new UserAction.Evaluator.Boolean { Name = "$Status$" }
			}
		}
	);
	UserAction->AddEvaluator("Action", "Clonk", "$SetAINewHome$", "$SetAINewHomeHelp$", "ai_set_new_home", [def, def.EvalAct_SetNewHome],
		{
			Enemy = nil,
			HomePosition = nil,
			Status = {
				Function = "bool_constant",
				Value = true
			}
		},
		{
			Type = "proplist",
			Display = "{{Enemy}} -> {{NewHome}}",
			EditorProps = {
				Enemy = enemy_evaluator,
				NewHome = new UserAction.Evaluator.Position {
					Name = "$NewHome$",
					EditorHelp = "$NewHomeHelp$"
				},
				NewHomeDir = {
					Type = "enum",
					Name = "$NewHomeDir$",
					EditorHelp = "$NewHomeDirHelp$",
					Options = [
						{ Name = "$Unchanged$" },
						{ Name = "$Left$", Value = DIR_Left },
						{ Name = "$Right$", Value = DIR_Right }
					]
				},
			}
		}
	);
}

public func EvalAct_SetActive(proplist props, proplist context)
{
	// User action: Activate enemy AI.
	var enemy = UserAction->EvaluateValue("Object", props.Enemy, context);
	var attack_target = UserAction->EvaluateValue("Object", props.AttackTarget, context);
	var status = UserAction->EvaluateValue("Boolean", props.Status, context);
	if (!enemy)
		return;
	// Ensure enemy AI exists
	var fx = GetAI(enemy);
	if (!fx)
	{
		// Deactivated? Then we don't need an AI effect.
		if (!status)
			return;
		fx = AddAI(enemy);
		if (!fx || !enemy)
			return;
	}
	// Set activation.
	fx->SetActive(status);
	// Set specific target if desired.
	if (attack_target)
		fx.target = attack_target;
}

public func EvalAct_SetNewHome(proplist props, proplist context)
{
	// User action: Set new home.
	var enemy = UserAction->EvaluateValue("Object", props.Enemy, context);
	var new_home = UserAction->EvaluatePosition(props.NewHome, context);
	var new_home_dir = props.NewHomeDir;
	if (!enemy)
		return;
	// Ensure enemy AI exists.
	var fx = GetAI(enemy);
	if (!fx)
	{
		fx = AddAI(enemy);
		if (!fx || !enemy)
			return;
		// Create without attack command.
		SetAutoSearchTarget(enemy, false);
	}
	fx.command = this.ExecuteIdle;
	fx.home_x = new_home[0];
	fx.home_y = new_home[1];
	if (GetType(new_home_dir))
		fx.home_dir = new_home_dir;
}
