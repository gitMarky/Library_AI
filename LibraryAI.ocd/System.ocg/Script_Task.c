/**
	Definition for tasks.

	@author Marky
	@version 0.1.0
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


	/**
	 Gets the priority of the task.

	 @return int The priority, and if the task has
	         a parent task, the parent priority
	         is added to the value.
	 @version 0.2.0
	 */
	GetPriority = func ()
	{
		var from_parent = 0;
		if (GetParentTask())
		{
			from_parent = GetParentTask()->GetPriority();
		}
		return this.task_data.priority + from_parent;
	},

	SetDescription = func (int description)
	{
		this.task_data.description = description;
	},

	GetDescription = func ()
	{
		return this.task_data.description;
	},

	/**
	 Sets the parent task. The parent task is a reference to the actual parent task.
	 @version 0.2.0
	 */
	SetParentTask = func(proplist parent)
	{
		this.task_parent = parent;
	},

	/**
	 Gets the parent task. The parent task is a reference to the original parent task.
	 @version 0.2.0
	 */
	GetParentTask = func()
	{
		return this.task_parent; // relies on proplists being passed as a reference, cannot be saved in task_data or else all tasks have the same parent
	},

	/**
	 Sets the task ID. This can be done only once.
	 @par ID The task ID.
	 @version 0.2.0
	 */
	SetTaskID = func(int ID)
	{
		if (GetTaskID())
		{
			FatalError(Format("Task already has ID %d, cannot assign new ID %d", GetTaskID(), ID));
		}
		else
		{
			this.task_id = ID;
		}
	},


	/**
	 Gets a unique numeric identifier for the task, if it was assigned.
	 @version 0.2.0
	 */
	GetTaskID = func()
	{
		return this.task_id;
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
