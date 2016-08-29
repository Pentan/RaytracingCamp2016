
#include "commandqueue.h"

using namespace r1h;

RenderCommandQueue::RenderCommandQueue()
{
    cmdTerminater.type = kFinish;
};
RenderCommandQueue::~RenderCommandQueue() {};

void RenderCommandQueue::defineTernimeter(Command cmd) {
    cmdTerminater = cmd;
}

std::mutex* RenderCommandQueue::getMutex() {
    return &cmdMutex;
}

void RenderCommandQueue::pushTileCommand(FrameBuffer::Tile tile) {
    Command cmd;
    cmd.type = kTile;
    cmd.tile = tile;
    cmdQueue.push(cmd);
}

void RenderCommandQueue::pushSleepCommand(unsigned long usec) {
    Command cmd;
    cmd.type = kSleep;
    cmd.usec = usec;
    cmdQueue.push(cmd);
}

void RenderCommandQueue::pushTileCommandWithLock(FrameBuffer::Tile tile) {
    std::lock_guard<std::mutex> lock(cmdMutex);
    pushTileCommand(tile);
}

void RenderCommandQueue::pushSleepCommandWithLock(unsigned long usec) {
    std::lock_guard<std::mutex> lock(cmdMutex);
    pushSleepCommand(usec);
}

RenderCommandQueue::Command RenderCommandQueue::popCommand() {
    std::lock_guard<std::mutex> lock(cmdMutex);
    if(cmdQueue.size() > 0) {
        Command cmd = cmdQueue.front();
        cmdQueue.pop();
        return cmd;
    } else {
        // ternimater
        return cmdTerminater;
    }
}

bool RenderCommandQueue::isQueueEmpty() {
	return (getRemainCommandCount() <= 0);
}

size_t RenderCommandQueue::getRemainCommandCount() {
	return cmdQueue.size();
}
