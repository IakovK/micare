#include "Builder.h"

Task::Task(Target* target)
	:m_Target(target)
{
}

void Task::AddDependency(Task &dep)
{
	m_Dependencies.push_back(&dep);
}

void Task::ExecuteDependencies(std::list<const Task*> dependents, ITaskExecutor *exec) const
{
	m_Target->m_nActiveDeps = m_Dependencies.size();
	for (auto d : m_Dependencies)
	{
		dependents.push_back(this);
		d->Execute(dependents, [=]
			{
				if (m_Target->m_nActiveDeps > 0)
					m_Target->m_nActiveDeps--;
				if (m_Target->m_nActiveDeps == 0)
				{
				}
			}, exec);
	}
}

void Task::Execute(std::list<const Task*> dependents, std::function<void()> callback, ITaskExecutor *exec) const
{
	if (std::find(dependents.begin(), dependents.end(), this) != dependents.end())
	{
		throw std::exception("circular dependency found");
	}
	ExecuteDependencies(dependents, exec);
	exec->ExecuteTask(m_Target->task, callback);
}

void BuildGraph::AddTarget(Target &t)
{
	auto r = m_Tasks.insert({ t.id, Task(&t) });
	if (!r.second)
		throw std::exception("duplicate task id");
}

void BuildGraph::AddDependency(size_t idTarget, size_t idSrc)
{
	auto it = m_Tasks.find(idTarget);
	if (it == m_Tasks.end())
		throw std::exception("unknown target id");
	auto it1 = m_Tasks.find(idSrc);
	if (it1 == m_Tasks.end())
		throw std::exception("unknown src id");

	it->second.AddDependency(it1->second);
}

const Task *BuildGraph::FindTask(size_t target_id) const
{
	auto it = m_Tasks.find(target_id);
	if (it == m_Tasks.end())
		throw std::exception("unknown target id");
	return &(it->second);
}

void InvokeFunc(std::function<void()> func)
{
	if (func)
		func();
}

Executor::Executor(size_t num_threads)
{
	for (int j = 0; j < num_threads; j++)
		m_threads.push_back(std::thread([=] {threadFunc(); }));
}

Executor::~Executor()
{
	Wait();
}

void Executor::SubmitWork(std::function<void()> task, std::function<void()> callback)
{
	WorkItem wi{ task, callback, false };
	m_queue.push(wi);
}

void Executor::Stop()
{
	WorkItem wi{ std::function<void()>(), std::function<void()>(), true };
	for (int j = 0; j < m_threads.size(); j++)
		m_queue.push(wi);
}

void Executor::Wait()
{
	for (auto &t : m_threads)
	{
		if (t.joinable())
			t.join();
	}
}

void Executor::threadFunc()
{
	while (true)
	{
		auto wi = m_queue.pop();
		if (wi.stop)
			break;
		InvokeFunc(wi.task);
		InvokeFunc(wi.callback);
	}
}

Builder::Builder(size_t num_threads)
	: m_ex(num_threads)
{
}

void Builder::execute(const BuildGraph& build_graph, size_t target_id)
{
	auto task = build_graph.FindTask(target_id);
	task->Execute({}, nullptr, this);
	m_ex.Stop();
	m_ex.Wait();
}

void Builder::ExecuteTask(std::function<void()> task, std::function<void()> callback)
{
	m_ex.SubmitWork(task, callback);
}
