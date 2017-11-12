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
	
	if (goal->~IsWaypoint())
	{
		return AStarWaypointMap->FindPath(start, goal);
	}
	else
	{
		// Find nearest waypoints at the goal
		var x, y;
		if (GetType(goal) == C4V_C4Object)
		{
			x = goal->GetX();
			y = goal->GetY();
		}
		else
		{
			x = goal.x;
			y = goal.y;
		}
		var neighbors = AStarWaypointMap->neighbor_waypoints(x, y);
		
		// Look up all (up to three) paths and choose the best one
		var min_cost = nil;
		var min_path = nil;
		for (var wp in neighbors)
		{
			var path = AStarWaypointMap->FindPath(start, wp);
			if (path)
			{
				var path_cost = AStarWaypointMap->total_cost(path);
				if (!min_cost || path_cost < min_cost)
				{
					min_cost = path_cost;
					min_path = path;
				}
			}
		}
		
		if (min_path) PushBack(min_path, goal);
		return min_path;
	}
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

	RemovePath(target);
	paths[GetKey(target)] = new Map_Waypoint_Path
	{
		target = target,
		EditorInfo = 
		{
			Color =  {R = 0, G = 255, B = 0},
			Gfx = "Move",
		},
	};
	
	Map_PathVisualization->Create(this, target); //, flag, jetpdelay, dir);

	if (reconnect)
	{
		target->AddPath(this, false);
	}
	
	return GetPath(target);
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


/* -- Waypoint movement functions -- */

public func OnMoveTo_Jump(proplist logic, object agent, object move_to)
{
	// Face the target
	if (logic.Agent_LookAt)
	{
		logic->~Agent_LookAt(agent, move_to);
	}
	else
	{
		var dir;
		if (move_to->GetX() < agent->GetX())
		{
			dir = 0;	
		}
		else
		{
			dir = 1;
		}
		agent->SetDir(dir);
	}
	// Clear agent command TODO
	agent->SetCommand("None");
	
	// Jump
	logic->Agent_Jump(agent);
	
	// Then move somewhere
	logic->Agent_MoveTo(agent, move_to);
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
	
	SetMoveTo = func (func call)
	{
		this.OnMoveTo = call;
		return this;
	},
	
	OnMoveTo = func (proplist logic, object agent, object move_to)
	{
		logic->Agent_MoveTo(agent, move_to);
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
	
	// Gets up to 3 waypoints in the vincinity of a point, on a 10pixel grid
	neighbor_waypoints = func(int x, int y)
	{
		if (!Map_Waypoint_PathCache)
		{
			Map_Waypoint_PathCache = {};
		}
	
		var key = neighbor_key(x,y);
		var cached = Map_Waypoint_PathCache[key];
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

			Map_Waypoint_PathCache[key] = neighbors;
			return neighbors;
		}
	},
	
	neighbor_key = func (int x, int y)
	{
		return Format("x%d_y%d", x / 10, y / 10);
	},


	total_cost = func (array path)
	{
		var value = 0;
		for (var index = 0; index < GetLength(path) - 1; ++index)
		{
			value += cost(path[index], path[index + 1]);
		}
		return value;
	}
};
