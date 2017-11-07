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

public func IsWaypoint(){ return true; }

/* -- Engine callbacks -- */


private func Construction()
{
	paths = {};
}


/* -- Public interface -- */

/**
 Finds a path from one waypoint to another.
 Supports the following formats:
 - FindPath(object, object): Finds a path from an object or a waypoint to another object or waypoint
 - FindPath({x, y}, {x, y}): Finds a path from a proplist position to another proplist position
 - FindPath(x1, y1, x2, y2): Finds a path from start coordinates to end coordinates
 
 
 @par start the starting waypoint
 @par goal the destination waypoint
 
 @return array the path to the goal waypoint, as an array,
               or {@code nil} if no path was found.
 */
public func FindPath(start, goal)
{
	AssertNotNil(start);
	AssertNotNil(goal);

	// all integer parameters: start_x, start_y, end_x, end_y
	if (Par(2) && Par(3))
	{
		return FindPath({x = start, y = goal}, {x = Par(2), y = Par(3)});
	}

	return AStarWaypointMap->FindPath(start, goal); //FindWaypoint(start), FindWaypoint(goal));
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


private func FindWaypoint(a, b)
{
	if (GetType(a) == C4V_C4Object) // object as first parameter
	{
		if (a->~IsWaypoint()) // waypoint? return it
		{
			return a;
		}
		else
		{
			return FindWaypoint(a->GetX(), a->GetY()) ?? a; // TODO: Test fallback option
		}
	}
	else if (GetType(a) == C4V_PropList) // intepret a proplist
	{
		return FindWaypoint(a.x, a.y);
	}
	else if (GetType(a) == C4V_Int && GetType(b) == C4V_Int) // interpret integers as a = x, b = y
	{
		var waypoints = FindObjects(Find_Func("IsWaypoint"), Sort_Distance(a, b));
		
		for (var waypoint in waypoints)
		{
			if (PathFree(a, b, waypoint->GetX(), waypoint->GetY()))
			{
				return waypoint;
			}
		}
	
		return nil;
	}
	else
	{
		FatalError(Format("Unsupported parameters: a = %v, b = %v; Supported: FindWaypoint(object), FindWaypoint({x, y}), FindWaypoint(int, int)", a, b));
	}
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


// Proplist that saves neighboring waypoints for arbitrary points on the landscape, in a 10pixel grid
static Map_Waypoint_PathCache;


// Search algorithm
local AStarWaypointMap = new AStar
{
	// This function is used as a heuristic for estimating the distance: node to the goal
	distance = func(object node, object goal)
	{
		if (node->~IsWaypoint())
		{
			return node->EstimateDistanceToWaypoint(goal);
		}
		else
		{
			return ObjectDistance(node, goal);
		}
	},

	// Cost between two neighboring nodes. 
	cost = func(object origin, object neighbor)
	{
		if (origin->~IsWaypoint())
		{
			return origin->GetDistanceToWaypoint(neighbor);
		}
		else
		{
			return ObjectDistance(origin, neighbor);
		}
	},

	// Returns all neighboring nodes.
	successors = func(object node)
	{
		if (node->~IsWaypoint())
		{
			return node->GetNeighbors();
		}
		else
		{
			return neighbor_waypoints(node->GetX(), node->GetY());
		}
	},
	
	// Proplist that saves neighboring waypoints for arbitrary points on the landscape, in a 10pixel grid
	// neighbor_cache = { can_write = false, },
	
	// Gets up to 3 waypoints in the vincinity of a point, on a 10pixel grid
	neighbor_waypoints = func(int x, int y)
	{
		/*if (!this.neighbor_cache.can_write)
		{
			this.neighbor_cache = MakePropertyWritable("neighbor_cache", this);
			this.neighbor_cache.can_write = true;
		}*/
		if (!Map_Waypoint_PathCache)
		{
			Map_Waypoint_PathCache = {};
		}
	
		var key = neighbor_key(x,y);
		var cached = Map_Waypoint_PathCache[key]; //neighbor_cache[key];
		if (cached)
		{
			return cached;
		}
		else
		{
			var waypoints = FindObjects(Find_Func("IsWaypoint"), Sort_Distance(x, y));
			var neighbors = [];

			for (var waypoint in waypoints)
			{
				if (PathFree(x, y, waypoint->GetX(), waypoint->GetY()))
				{
					PushBack(neighbors, waypoint);
				}
				if (GetLength(neighbors) >= 3)
				{
					break;
				}
			}
			
			//neighbor_cache[key] = neighbors;
			Map_Waypoint_PathCache[key] = neighbors;
			return neighbors;
		}
	},
	
	neighbor_key = func (int x, int y)
	{
		return Format("x%d_y%d", x / 10, y / 10);
	},
};
