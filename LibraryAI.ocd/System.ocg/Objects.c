
global func GetNearestObject(array objects)
{
	AssertObjectContext();
	
	var nearest;
	for (var object in objects)
	{
		if (!nearest || ObjectDistance(object) < ObjectDistance(nearest))
		{
			nearest = object;
		}
	}
	
	return nearest;
}
