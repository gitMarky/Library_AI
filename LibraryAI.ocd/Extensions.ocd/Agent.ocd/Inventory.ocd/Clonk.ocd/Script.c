/**
	Agent inventory logic for a Clonk 

	@author Marky
	@version 0.2.0
 */


/**
 Does the agent have an item?

 @par agent The object that is acting - the Clonk.
 @par item The desired item.

 @return {@code true} if the item is contained in the agent,
         or the value of the {@link Global#_inherited} function.

 @version 0.2.0
 */
public func Agent_HasItem(object agent, object item)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(item);

	return agent == item->Contained() || _inherited(agent, item);
}


/**
 Defines how the agent can take an item.

 @par agent The object that is acting - the Clonk.
 @par item The desired item.

 @note Calls {@link Global#Collect} in the item. 

 @version 0.2.0
 */
public func Agent_TakeItem(object agent, object item)
{
	AssertDefinitionContext();
	AssertNotNil(agent);
	AssertNotNil(item);

	agent->Collect(item);
}


/**
 Find out if the agent can take an item.

 @par agent The object that is acting - the Clonk.
 @par item The desired item.

 @return bool {@code true} if the distance between
              item and agent is less than 10 pixels. 

 @version 0.2.0
 */
public func Agent_CanTakeItem(object agent, object item)
{
	return ObjectDistance(agent, item) < 10 || _inherited(agent, item);
}


/**
 Defines how the agent finds an item of a specific type.

 @par agent The object that is acting - the Clonk.
 @par type The desired item definition.

 @return object An object of the desired type.
         This is {@link Global#FindContents} of that type,
         or the {@link Global#_inherited} function. 

 @version 0.2.0
 */
public func Agent_FindItemType(object agent, def type)
{
	return agent->FindContents(type) ?? _inherited(agent, type);
}


/**
 Defines how the agent can find an item.

 @par agent The object that is acting - the Clonk.
 @par find_criteria A list of criteria for {@link Global#FindObject},
      such as {@link Global#Find_OCF}.

 @return object An object matching the desired criteria. 
                This is {@link Global#FindObject} with
                {@link Global#Find_Container} where the
                agent is the container,
                or the {@link Global#_inherited} function.

 @version 0.2.0
 */
public func Agent_FindItem(object agent, array find_criteria)
{
	return agent->FindObject(Find_Container(agent), find_criteria) ?? _inherited(agent, find_criteria);
}


/**
 Defines how the agent selects an item, so that it is
 the item that he is going to use.

 @par agent The object that is acting - the Clonk.
 @par item The desired item.

 @note Calls {@link Library_ClonkInventoryControl#SetHandItemPos} in the item. 

 @version 0.2.0
 */
public func Agent_SelectItem(object agent, object item)
{
	if (item && agent == item->Contained())
	{
		agent->SetHandItemPos(0, agent->GetItemPos(item));
	}
}
