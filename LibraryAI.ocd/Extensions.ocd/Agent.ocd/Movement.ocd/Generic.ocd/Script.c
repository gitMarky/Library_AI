/**

 */


public func Agent_IsBetween(int current_x, int current_y, int start_x, int start_y, int end_x, int end_y)
{
	if (_inherited(current_x, current_y, start_x, start_y, end_x, end_y))
	{
		return true;
	}
	else // is it between start and end?
	{
		var angle_to_start = Angle(current_x, current_y, start_x, start_y);
		var angle_to_end = Angle(current_x, current_y, end_x, end_y);

		var angle = Abs(Normalize(angle_to_start - angle_to_end, 0));

		return angle > 90;
	}
}
