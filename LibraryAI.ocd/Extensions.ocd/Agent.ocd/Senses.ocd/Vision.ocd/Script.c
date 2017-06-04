/**
	Vision for clonk type NPCs.
 */


func AppendTo(def type)
{
	type->AddFunction(this.Agent_FindItem);
}

public func Agent_FindItem(def type)
{
	return FindObject(Find_ID(type), Find_NoContainer(), Find_Distance(250));
}
