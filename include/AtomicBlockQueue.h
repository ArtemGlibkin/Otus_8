#pragma once
#include <queue>
#include "CommandBlock.h"
#include <memory>
#include <mutex>
#include <iostream>
#include <condition_variable>


class AtomicQueueResetException : public std::exception
{
public:
	AtomicQueueResetException()
	{

	}
};

template<typename T>
class AtomicQueue
{
	std::queue<T> mQueue;
	std::mutex mtx;
	std::condition_variable cv;
	bool mStop = false;
public:
	void push(T& block)
	{
		std::unique_lock<std::mutex> lock(mtx);
		mQueue.push(block);
		cv.notify_all();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [&] {return !mQueue.empty() || mStop; });
		if (mStop)
			throw AtomicQueueResetException();
		T block = mQueue.front();
		mQueue.pop();
		return block;
	}

	void reset()
	{
		mStop = true;
		cv.notify_all();
	}

	size_t size()
	{
		return mQueue.size();
	}
};