#pragma once
#include <functional>
#include <map>
#include <list>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

struct Target
{
	size_t id;
	std::function<void()> task;
	std::atomic<size_t> m_nActiveDeps;	// number of dependencies not yet completed
};

class ITaskExecutor
{
public:
	virtual void ExecuteTask(std::function<void()> task, std::function<void()> callback) = 0;
};

class Task
{
	Target* m_Target;
	std::list<Task*> m_Dependencies;
public:
	Task(Target* target);
	void AddDependency(Task &dep);
	void Execute(std::list<const Task*> dependents, std::function<void()> callback, ITaskExecutor *exec) const;
private:
	void ExecuteDependencies(std::list<const Task*> dependents, ITaskExecutor *exec) const;
};

class BuildGraph
{
	std::map<size_t, Task> m_Tasks;
public:
	void AddTarget(Target &t);
	void AddDependency(size_t idSrc, size_t idTarget);
	const Task *FindTask(size_t target_id) const;
};

struct WorkItem
{
	std::function<void()> task;
	std::function<void()> callback;
	bool stop;
};

// Thread-safe queue
// taken from https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
template <typename T>
class TSQueue {
private:
	// Underlying queue
	std::queue<T> m_queue;

	// mutex for thread synchronization
	std::mutex m_mutex;

	// Condition variable for signaling
	std::condition_variable m_cond;

public:
	// Pushes an element to the queue
	void push(T item)
	{

		// Acquire lock
		std::unique_lock<std::mutex> lock(m_mutex);

		// Add item
		m_queue.push(item);

		// Notify one thread that
		// is waiting
		m_cond.notify_one();
	}

	// Pops an element off the queue
	T pop()
	{

		// acquire lock
		std::unique_lock<std::mutex> lock(m_mutex);

		// wait until queue is not empty
		m_cond.wait(lock,
			[this]() { return !m_queue.empty(); });

		// retrieve item
		T item = m_queue.front();
		m_queue.pop();

		// return item
		return item;
	}
};

class Executor
{
	std::vector<std::thread> m_threads;
	TSQueue<WorkItem> m_queue;
	void threadFunc();
public:
	Executor(size_t num_threads);
	~Executor();
	void SubmitWork(std::function<void()> task, std::function<void()> callback);
	void Stop();
	void Wait();
};

class Builder : public ITaskExecutor
{
	Executor m_ex;
	size_t m_numThreads;
public:
	explicit Builder(size_t num_threads);
	void execute(const BuildGraph& build_graph, size_t target_id);
	// ITaskExecutor
	virtual void ExecuteTask(std::function<void()> task, std::function<void()> callback);
};

