/**
	Waypoint
	
	@author Marky
	@version 0.2.0
*/


/* -- Properties -- */

local Name = "$Name$";
local Description = "$Description$";
local Plane = 10000;
local Visibility = VIS_Editor;

local paths;


/* -- Engine callbacks -- */


private func Construction()
{
	paths = {};
}


/* -- Public interface -- */

/**
 Finds a path from one waypoint to another
 
 @par start the starting waypoint
 @par goal the destination waypoint
 
 @return array the path to the goal waypoint, as an array,
               or {@code nil} if no path was found.
 */
public func FindPath(start, goal)
{
	return AStarWaypointMap->FindPath(start, goal);
}


/**
 Adds a path to another waypoint.
 
 @par target the other waypoint.
 @par reconnect if this is set to {@code true} then the other waypoint
                will have a connection to this path, too. Otherwise,
                the path is one-way only.
 */
public func AddPath(object target, bool reconnect)
{
	AssertNotNil(target);

	if (GetPath(target))
	{
		FatalError(Format("This waypoint already has a path to %v", target));
	}

	paths[GetKey(target)] = new Map_Waypoint_Path
	{
		target = target,
	};
	
	Map_PathVisualization->Create(this, target); //, flag, jetpdelay, path_id, dir);

	if (reconnect)
	{
		target->AddPath(this, false);
	}
}


/**
 Removes the path to a target.
 
 @par target the other waypoint.
 @return bool {@code true} if the path was removed.
              Otherwise, there was no path anyway. 
 */
public func RemovePath(object target)
{
	if (GetPath(target))
	{
		paths[GetKey(target)] = nil;
		return true;
	}
	return false;
}


/* -- Waypoint network interface -- */

private func GetNeighbors()
{
	var neighbors = [];
	for (var key in GetProperties(paths))
	{
		PushBack(neighbors, paths[key]->GetTarget());
	}
	return neighbors;
}


private func EstimateDistanceToWaypoint(object node)
{
	// In case that we know the goal, then just use the known distance
	if (GetPath(node))
	{
		return GetDistanceToWaypoint(node);
	}
	else // use the Manhattan distance for unknown nodes, because we do not know whether there is a direct path
	{
		return Abs(this->GetX() - node->GetX()) + Abs(this->GetY() - node->GetY());
	}
}


private func GetDistanceToWaypoint(object node)
{
	var path = GetPath(node);

	if (nil == path)
	{
		FatalError("This function works only for nodes that this waypoint is connected to.");
	}
	else
	{
	 	// Use a custom cost? This is intended for teleporters, jump pads, etc.
	 	// Otherwise use the direct distance, because usually the nodes are connected by a straight line
		return path->GetCost() ?? ObjectDistance(this, path->GetTarget());
	}
}


/* -- Internals -- */

private func GetKey(object node)
{
	return Format("wp%d", node->ObjectNumber());
}


private func GetPath(object node)
{
	return paths[GetKey(node)];
}

/* -- Internals - additional structures -- */

// Description for a path
static const Map_Waypoint_Path = new Global
{
	target = nil,
	cost = nil,

	GetTarget = func ()
	{
		return this.target;
	},

	GetCost = func ()
	{
		return this.cost;
	},
};


// Search algorithm
local AStarWaypointMap = new AStar
{
	// This function is used as a heuristic for estimating the distance: node to the goal
	distance = func(object node, object goal)
	{
		return node->EstimateDistanceToWaypoint(goal);
	},

	// Cost between two neighboring nodes. 
	cost = func(object origin, object neighbor)
	{
		return origin->GetDistanceToWaypoint(neighbor);
	},

	// Returns all neighboring nodes.
	successors = func(object node)
	{
		return node->GetNeighbors();
	},
};
