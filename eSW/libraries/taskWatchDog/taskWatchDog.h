#ifndef TASKWATCHDOG
#define TASKWATCHDOG

#include <Arduino.h>
#include <frt.h>

template <typename TaskType>
class TaskWatchDog
{
private:

	struct TaskInfo
	{
		TaskType* task;
		const char* name;
		uint32_t lastAliveTime;
		uint32_t timeoutMs;
		bool enabled;
	};
	
	static const uint8_t MAX_MONITORED_TASKS = 4;
	TaskInfo monitoredTasks[MAX_MONITORED_TASKS];
	uint8_t numMonitoredTasks = 0;
	frt::Mutex mutex;
	
public:

	TaskWatchDog()
	{
		for (auto& task : monitoredTasks)
		{
			task.task = nullptr;
			task.enabled = false;
		}
	}
	
	bool registerTask(TaskType& task, const char* name, uint32_t timeoutMs)
	{
		LockGuard lock(mutex);
		
		if (numMonitoredTasks >= MAX_MONITORED_TASKS)
		{
			return false;
		}
		
		monitoredTasks[numMonitoredTasks].task = &task;
		monitoredTasks[numMonitoredTasks].name = name;
		monitoredTasks[numMonitoredTasks].timeoutMs = timeoutMs;
		monitoredTasks[numMonitoredTasks].lastAliveTime = millis();
		monitoredTasks[numMonitoredTasks].enabled = true;
		
		numMonitoredTasks++;
		return true;
	}
	
	void taskAlive(TaskType& task)
	{
		LockGuard lock(mutex);
		
		for (uint8_t i = 0; i < numMonitoredTasks; i++)
		{
			if (monitoredTasks[i].task == &task && monitoredTasks[i].enabled)
			{
				monitoredTasks[i].lastAliveTime = millis();
				break;
			}
		}
	}
	
	void checkTasks()
	{
		const uint32_t currentTime = millis();
		LockGuard lock(mutex);
		
		for (uint8_t i = 0; i < numMonitoredTasks; i++)
		{
			if (!monitoredTasks[i].enabled) continue;
			
			if (monitoredTasks[i].task->isRunning() && (currentTime - monitoredTasks[i].lastAliveTime > monitoredTasks[i].timeoutMs))
			{
				monitoredTasks[i].task->stop();
				monitoredTasks[i].lastAliveTime = currentTime;
				monitoredTasks[i].enabled = false;
			}
		}
	}
};


#endif // TASKWATCHDOG