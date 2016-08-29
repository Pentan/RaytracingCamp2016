#ifndef R1H_COMMANDQUEUE_H
#define R1H_COMMANDQUEUE_H

#include <queue>
#include <mutex>

#include "framebuffer.h"

namespace r1h {

class RenderCommandQueue {
public:
    
    enum CommandType {
        kNoOp   = 0,
        kTile,
        kSleep,
		kYield,
        kFinish
    };
    
    struct Command {
        CommandType type;
        union {
            FrameBuffer::Tile tile;
            long usec;
        };
        Command():type(kNoOp) {}
    };
    
    RenderCommandQueue();
    ~RenderCommandQueue();
    
    void defineTernimeter(Command cmd);
    
    std::mutex* getMutex();
    // no lock
    void pushTileCommand(FrameBuffer::Tile tile);
    void pushSleepCommand(unsigned long usec);
    // with lock
    void pushTileCommandWithLock(FrameBuffer::Tile tile);
    void pushSleepCommandWithLock(unsigned long usec);
    
    Command popCommand();
    
	bool isQueueEmpty();
	size_t getRemainCommandCount();
    
private:
    std::queue<Command> cmdQueue;
    std::mutex cmdMutex;
    Command cmdTerminater;
};

}

#endif
