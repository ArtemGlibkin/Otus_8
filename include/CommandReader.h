#pragma once
#include <iostream>
#include <string>
#include "CommandBlock.h"
#include <istream>
#include <AtomicBlockQueue.h>

class CommandReader
{
	int blockSize = 0;
	bool exitFlag = false;
	bool dynamicBlock = false;
	int dynamicBlockCounter = 0;
	std::stringstream inputStream;
	std::unique_ptr<CommandBlock> currientBlock;
	AtomicQueue<CommandBlock>& mFileOutputQueue;
	AtomicQueue<CommandBlock>& mScreenOutputQueue;

	bool isExit(const Command& cmd)
	{
		static const std::string eof = std::string("EOF");
		return exitFlag = (cmd == eof);
	}
	
	bool isBeginBlock(const Command& cmd)
	{
		static const std::string beginBlock = std::string("{");
		return cmd == beginBlock;
	}

	bool isEndBlock(const Command& cmd)
	{
		static const std::string endBlock = std::string("}");
		return cmd == endBlock;
	}

	void beginBlockHandler()
	{
		dynamicBlock = true;
		dynamicBlockCounter++;
	}

	void endBlockHandler()
	{
		if (!dynamicBlock)
			return;

		dynamicBlockCounter--;
		if (dynamicBlockCounter == 0)
			dynamicBlock = false;
	}

	bool inputHandler(const Command& input, CommandBlock& block)
	{
		if (isExit(input))
			return false;
		if (isEndBlock(input))
		{
			endBlockHandler();
			return dynamicBlock;
		}
		if (isBeginBlock(input))
		{
			beginBlockHandler();
			return !(dynamicBlock && (dynamicBlockCounter == 1));
		}

		block.add(input);
		return true;
	}

	void writeBlock(CommandBlock & block)
	{
		mFileOutputQueue.push(*currientBlock);
		mScreenOutputQueue.push(*currientBlock);
	}

public:
	CommandReader(AtomicQueue<CommandBlock>& fileQueue, AtomicQueue<CommandBlock>& screenQueue, int n) : blockSize(n), 
																										 mFileOutputQueue(fileQueue), 
																										 mScreenOutputQueue(screenQueue){};

	void readBlock(std::stringstream& inputstream)
	{
		if (!currientBlock)
			currientBlock = std::make_unique<CommandBlock>();

		for (std::string input; std::getline(inputstream, input); )
		{
			if (!inputHandler(input, *currientBlock) || (currientBlock->size() == blockSize) && !dynamicBlock)
			{
				writeBlock(*currientBlock);
				currientBlock = std::make_unique<CommandBlock>();
			}
		}
	}

	void stop()
	{
		if (currientBlock)
			writeBlock(*currientBlock);
	}

	void receive(const char * data, int size)
	{
		std::stringstream ss;
		ss.write(data, size);
		readBlock(ss);
	}
};