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
static const Map_WayPoint_Path = new Global
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

