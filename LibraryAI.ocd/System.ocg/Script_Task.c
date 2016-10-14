/**
	Prototype for task prototypes.
*/

static const Task = new Effect 
{
	SetPriority = func (int priority)
	{
		this.task_data.priority = priority;
	},
	
	GetPriority = func ()
	{
		return this.task_data.priority;
	},
	
	IsTransferrable = func ()
	{
		return this.task_data.transferrable;
	},

	task_data = {
		priority = 0,
		transferrable = false,
	},
};
