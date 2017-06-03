/**
	Prototype for task prototypes.
*/

static const Task = new Effect 
{
	/* -- Interface --*/

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
	
	/* -- Callbacks -- */
	
	Construction = func()
	{
		this.Interval = 0; // never calls a timer
		
		if (this.Priority) // delegate the priority to the internal stuff, too
		{
			this->SetPriority(this.Priority);
		}

		return FX_OK;
	},
	
	/* -- Internals -- */

	task_data = {
		priority = 0,
		transferrable = false,
	},
	
	/* -- Execution -- */
	
	Execute = func (proplist controller, object agent)
	{
	},
};
