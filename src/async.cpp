#include <vector>
#include <thread>
#include "CommandReader.h"
#include "AtomicBlockQueue.h"
#include "FileOutputThread.h"
#include "async.h"

class LibInitializer
{
public:
	std::vector<std::unique_ptr<CommandReader>> mCommandReaders;
	std::vector<std::thread> mOutThreads;
	AtomicQueue<CommandBlock> mFileOutputQueue;
	AtomicQueue<CommandBlock> mScreenOutputQueue;
	int mContextCounter = 0;
private:
	void outputThreadsStop()
	{
		mFileOutputQueue.reset();
		mScreenOutputQueue.reset();
	}
public:
	LibInitializer()
	{
		mCommandReaders.reserve(5);
		mOutThreads.reserve(3);
		mOutThreads.push_back(std::thread(&FileOutputThreadMain, std::ref(mFileOutputQueue)));
		mOutThreads.push_back(std::thread(&FileOutputThreadMain, std::ref(mFileOutputQueue)));
		mOutThreads.push_back(std::thread(&ScreenOutputThreadMain, std::ref(mScreenOutputQueue)));
	}

	int addNewReader(int blockSize)
	{
		mCommandReaders.push_back(std::make_unique<CommandReader>(mFileOutputQueue, mScreenOutputQueue, blockSize));
		return mContextCounter++;
	}

	void receive(int context, const char* buffer, int size)
	{
		auto& it = mCommandReaders.at(context);
		if (!it)
			throw std::runtime_error("invalid context");
		it->receive(buffer, size);
	}

	void disconnectReader(int context)
	{
		auto& it = mCommandReaders.at(context);
		if (!it)
			throw std::runtime_error("invalid context");
		it->stop();
		it.reset();
		stop();
	}

	void stop()
	{
		/*
			���������, ���� ��� ������ ��������� ���� ������, �� ���� ������ ������
		*/
		for (auto & it : mCommandReaders)
			if (it) return;

		while (mFileOutputQueue.size() != 0);
		outputThreadsStop();
	}

	~LibInitializer()
	{
		stop();
		for(auto & it : mOutThreads)
			it.join();
	}
};

static LibInitializer libInitializer;

int connect(int block_size)
{
	return libInitializer.addNewReader(block_size);
}

void receive(int context, const char* buffer, int size)
{
	libInitializer.receive(context, buffer, size);
}

void disconnect(int context)
{
	libInitializer.disconnectReader(context);
}