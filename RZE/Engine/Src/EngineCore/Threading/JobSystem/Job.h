#pragma once

#include <EngineCore/Threading/Threading.h>

#include <Utils/Functor.h>

namespace Threading
{
	class Job
	{
	public:
		using Task = Functor<void>;

	public:
		Job();
		explicit Job(Task task);
		Job(const Job& other);
		Job(Job&& other);
		~Job();

		void Run();

		Task GetTask() const;

		void operator=(const Job& other)
		{
			mTask = other.mTask;
			mOnFinished = other.mOnFinished;
		}

	private:
		Task mTask;
		Task mOnFinished;
	};
}