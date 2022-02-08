#include <iostream>
#include "CommandReader.h"
#include "AtomicBlockQueue.h"
#include "FileOutputThread.h"
#include "async.h"
#include <thread>
#include <chrono>
int main(int argc, char* argv[])
{
    int desc = connect(3);
    std::string commands("cmd1\ncmd2\n{\ncmd3\ncmd4\n}\ncmd5");
    //receive(desc, commands.c_str(), commands.size());
   // receive(desc, commands.c_str(), commands.size());
    //receive(desc, commands.c_str(), commands.size());

    int desc2 = connect(2);
    receive(desc2, commands.c_str(), commands.size());
    receive(desc2, commands.c_str(), commands.size());
    receive(desc2, commands.c_str(), commands.size());
    disconnect(desc);
    std::cout<<"disconnect"<<std::endl;
    disconnect(desc2);
    std::cout<<"disconnect2"<<std::endl;
    return 0;
}