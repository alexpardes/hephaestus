#include "stdafx.h"
#include "CommandBuffer.h"

CommandBuffer::CommandBuffer() {
  buffer = new CommandTurn();
}

CommandTurn* CommandBuffer::TakeCommandTurn() {
  CommandTurn* turn = buffer;

  // Makes sure that no more commands will be added to the old buffer.
  lock.lock();
  buffer = new CommandTurn();
  lock.unlock();

  // Is this safe if another thread adds a sink during iteration?
  for (CommandSink* sink : sinks) {
    sink->AddCommands(turn);
  }
  return turn;
}

void CommandBuffer::CreateTurnDelay(int nTurns) {
  for (int i = 0; i < nTurns; ++i) {
    TakeCommandTurn();
  }
}

void CommandBuffer::AddCommand(Command* command) {
  lock.lock();
  buffer->push_back(command);
  lock.unlock();
}

void CommandBuffer::AddCommands(CommandTurn* commands) {
  lock.lock();
  for (const Command* command : *commands) {
    buffer->push_back(command);
  }
}

void CommandBuffer::RegisterCommandSink(CommandSink* sink) {
  sinks.push_back(sink);
}