/**
	Prototype for task prototypes.
*/

// return values

static const TASK_EXECUTION_FAILURE = -1;
static const TASK_EXECUTION_IN_PROGRESS = 0;
static const TASK_EXECUTION_SUCCESS = 1;

// task prototype

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
	
	SetDescription = func (int description)
	{
		this.task_data.description = description;
	},
	
	GetDescription = func ()
	{
		return this.task_data.description;
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
		type = nil,
		priority = 0,
		transferrable = false,
		description = "unknown",
	},
	
	GetTaskType = func ()
	{
		return this.task_data.type;
	},
	
	SetTaskType = func (id type)
	{
		this.task_data.type = type;
	},

	/* -- Execution -- */
	
	Execute = func (proplist controller, object agent)
	{
	},
};
