#appendto Clonk

public func Definition(proplist def)
{
	inherited(def);
	
	AgentExtension_Ready_Clonk->AppendTo(def);
	AgentExtension_Senses_RangedVision->AppendTo(def);
}
