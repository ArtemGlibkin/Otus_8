#include "AtomicBlockQueue.h"

void FileOutputThreadMain(AtomicQueue<CommandBlock>& queue)
{
	while (true)
	{
		try
		{
			queue.pop().filePrint();
		}
		catch(AtomicQueueResetException& e)
		{
			return;
		}
	}
}

void ScreenOutputThreadMain(AtomicQueue<CommandBlock>& queue)
{
	while (true)
	{
		try
		{
			queue.pop().displayPrint();
		}
		catch(AtomicQueueResetException& e)
		{
			return;
		}
	}
}