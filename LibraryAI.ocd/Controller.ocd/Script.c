/**
	AI
	Controls bots.

	@author Marky
	@credits Original AI structure/design by Sven2, Maikel
*/

static const SAVESCEN_ID_AI = "AI";

/*-- Public interface --*/

// Change whether target Clonk has an AI (used by editor).
public func SetAI(object clonk, bool should_have_ai)
{
	if (should_have_ai)
	{
		return AddAI(clonk);
	}
	else
	{
		return RemoveAI(clonk);
	}
}


// Add AI execution timer to target Clonk.
public func AddAI(object clonk)
{
	AssertDefinitionContext(Format("AddAI(%v)", clonk));

	var fx_ai = GetAI(clonk) ?? clonk->CreateEffect(FxAI, 1, 1, this);
	return fx_ai;
}


// Remove the AI execution timer
public func RemoveAI(object clonk)
{
	AssertDefinitionContext(Format("RemoveAI(%v)", clonk));

	var fx_ai = GetAI(clonk);
	if (fx_ai)
	{
		fx_ai->Remove();
	}
}


public func GetAI(object clonk)
{
	AssertDefinitionContext(Format("GetAI(%v)", clonk));

	if (clonk)
	{
		return clonk->~GetAI();
	}
	else
	{
		return nil;
	}
}


// Set active state: Enables/Disables timer
public func SetActive(object clonk, bool active)
{
	AssertDefinitionContext(Format("SetActive(%v, %v)", clonk, active));

	var fx_ai = GetAI(clonk);
	if (fx_ai)
	{
		if (!active)
		{
			// Inactive: Stop any activity.
			clonk->SetCommand("None");
			clonk->SetComDir(COMD_Stop);
		}
		return fx_ai->SetActive(active);
	}
	else
	{
		return false;
	}
}


/*-- AI Effect --*/

// The AI effect stores a lot of information about the AI clonk. This includes its state, enemy target, alertness etc.
// Each of the functions which are called in the AI definition pass the effect and can then access these variables.
// The most important variables are:
// fx.Target     - The AI clonk.
// fx.target     - The current target the AI clonk will attack.
// fx.alert      - Whether the AI clonk is alert and aware of enemies around it.
// fx.weapon     - Currently selected weapon by the AI clonk.
// fx.ammo_check - Function that is called to check ammunition for fx.weapon.
// fx.commander  - Is commanded by another AI clonk.
// fx.control    - Definition controlling this AI, all alternative AIs should include the basic AI.

local FxAI = new Effect
{
	Construction = func(id control_def)
	{
		// Execute AI every 3 frames.
		this.Interval = control_def->~GetTimerInterval() ?? 1;
		// Store the definition that controls this AI.
		this.control = control_def;
		// Callback to the controller
		this.control->~OnAddAI(this);
		// Give the AI a helper function to get the AI control effect.
		this.Target.ai = this;
		this.Target.GetAI = this.GetAI;
		return FX_OK;
	},
	
	GetAI = func()
	{
		return this.ai;
	},

	Timer = func(int time)
	{
		// Execute the AI in the clonk.
		this.control->Execute(this, time);
		return FX_OK;
	},

	Destruction = func(int reason)
	{
		// Callback to the controller
		this.control->~OnRemoveAI(this, reason);
		// Remove AI reference.
		if (Target && Target.ai == this)
			Target.ai = nil;
		return FX_OK;	
	},
	
	Damage = func(int dmg, int cause)
	{
		// AI takes damage: Make sure we're alert so evasion and healing scripts are executed!
		// It might also be feasible to execute encounter callbacks here (in case an AI is shot from a position it cannot see).
		// However, the attacking clonk is not known and the callback might be triggered e.g. by an unfortunate meteorite or lightning blast.
		// So let's just keep it at alert state for now.
		if (dmg < 0) 
			this.alert = this.time;
		return dmg;
	},
	SetActive = func(bool active)
	{
		this.Interval = (this.control->~GetTimerInterval() ?? 1) * active;	
	},
	GetActive = func()
	{
		return this.Interval != 0;	
	},
	EditorProps = {
	},
	// Save this effect and the AI for scenarios.
	SaveScen = func(proplist props)
	{
		if (this.Target)
		{
			props->AddCall(SAVESCEN_ID_AI, this.control, "AddAI", this.Target);
			if (!this.Interval)
			{
				props->AddCall(SAVESCEN_ID_AI, this.control, "SetActive", this.Target, false);
			}
			this.control->~OnSaveScenarioAI(this, props);
			return true;
		}
		else
		{
			return false;
		}
	}	
};


/*-- AI Execution --*/

public func Execute(effect fx, int time) // TODO: Adjust
{
	return this->Call(fx.strategy, fx);
}


// Selects an item the clonk is about to use.
public func SelectItem(effect fx, object item)
{
	if (!item)
		return;
	if (item->Contained() != fx.Target)
		return;
	fx.Target->SetHandItemPos(0, fx.Target->GetItemPos(item));
}

/*-- Editor Properties --*/

// TODO

/*-- Properties --*/

local Plane = 300;

/*-- Callbacks --*/


// Callback from the effect Construction()-call
public func OnAddAI(proplist fx_ai)
{
	// called by the effect Construction()
}


// Callback from the effect Destruction()-call
public func OnRemoveAI(proplist fx_ai, int reason)
{
	// called by the effect Destruction()
}


// Callback from the effect SaveScen()-call
public func OnSaveScenarioAI(proplist fx_ai, proplist props)
{
	// called by the effect SaveScen()
}
