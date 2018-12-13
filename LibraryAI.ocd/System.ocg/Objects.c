/**
 Goes through an array of objects and returns that with the least distance to the calling object.

 @par objects The objects.
 @return object The nearest object.
 @version 0.2.0
*/
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
