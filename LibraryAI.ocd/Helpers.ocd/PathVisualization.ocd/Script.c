/*-- Path visualization --*/


local so, eo, sx, sy, ex, ey, flag, jtpk, path, dir;

func Initialize()
{
	AddTimer(this.UpdatePath, 10);
}

func UpdatePath()
{
	/*if (so && so->GetPath(path)->GetTarget())
	{
		// change?
		if (so->GetX() != sx || so->GetY() != sy || eo->GetX() != ex || eo->GetY() != ey || so->GetPathFlag(path) != flag || so->GetPathJetpack(path) != jtpk || so->GetPathTarget(path) != eo || so->GetPathDir(path) != dir)
		{
			//DebugLog("Path changed");
			//Set(so, so->GetPathTarget(path), so->GetPathFlag(path), so->GetPathJetpack(path), path, so->GetPathDir(path));
		}
	}
	else
	{
		RemoveObject();
	}*/
}

func Create(object start, object end, int flags, int jetpack, int iPath, int iDir)
{
	var line = CreateObject(Map_PathVisualization);
	line->Set(start, end, flags, jetpack, iPath, iDir);
	return line;
}

func Set(object start, object end, int flags, int jetpack, int iPath, int iDir)
{
	so = start;
	sx = start->GetX();
	sy = start->GetY();
	eo = end;
	ex = end->GetX();
	ey = end->GetY();
	flag = flags;
	jtpk = jetpack;
	path = iPath;
	dir = iDir ?? 1;
	
	SetObjectBlitMode(GFX_BLIT_Additive);
	SetClrModulation(MyColor(flags, jetpack, start));

	var cl = 1000 * Distance(sx, sy, ex, ey) / 256;
	var w = 650;
	var r = Angle(sx, sy, ex, ey) - 180;
	
	var fsin = -Sin(r, 1000), fcos = Cos(r, 1000);
	
	var xoff = -4;
	var yoff = 0;
	
	var width = +fcos * w / 1000, height = +fcos * cl / 1000;
	var xskew = +fsin * cl / 1000, yskew = -fsin * w / 1000;
	
	var xadjust = +fcos * xoff + fsin * yoff;
	var yadjust = -fsin * xoff + fcos * yoff;
	
	// set matrix values
	SetObjDrawTransform(-width * dir, xskew, xadjust, -yskew * dir, height, yadjust);
}

func MyColor(int flags, int jetpack, object start)
{
	SetAction("Move", start);
	if (jetpack)
	{
		SetAction("Script", start);
		return RGB(255, 0, 0);
	}
	else
	{
		//if (flags == Path_MoveTo)
		//{
			return RGB(0, 255, 0);
		/*}
		if (flags == Path_Jump)
		{
			return RGB(255, 255, 0);
		}
		if (flags == Path_Backflip)
		{
			SetAction("Script", start);
			return RGB(255, 255, 0);
		}
		SetAction("Transport", start);
		if (flags == Path_Lift)
		{
			return RGB(255, 0, 255);
		}*/
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// properties

local Visibility = VIS_Editor;
local Plane = 10000;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// actions

local ActMap = {
Move = {
	Prototype = Action,
	Name = "Move",
	Procedure = DFA_ATTACH,
	Length = 1,
	X = 0,
	Y = 0,
	Wdt = 32,
	Hgt = 256,
	OffX = -16,
	OffY = 0,
	NextAction = "Hold",
},

Transport = {
	Prototype = Action,
	Name = "Transport",
	Procedure = DFA_ATTACH,
	Length = 1,
	X = 32,
	Y = 0,
	Wdt = 32,
	Hgt = 256,
	OffX = -16,
	OffY = 0,
	NextAction = "Hold",
},

Script = {
	Prototype = Action,
	Name = "Script",
	Procedure = DFA_ATTACH,
	Length = 1,
	X = 64,
	Y = 0,
	Wdt = 32,
	Hgt = 256,
	OffX = -16,
	OffY = 0,
	NextAction = "Hold",
},

};
