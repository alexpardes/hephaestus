#include "stdafx.h"
#include "CommandBuffer.h"
#include "CommandTurn.h"

CommandBuffer::CommandBuffer() {
  buffer = std::make_shared<CommandTurn>();
}

std::shared_ptr<CommandTurn> CommandBuffer::TakeCommandTurn() {
  auto turn = buffer;

  // Makes sure that no more commands will be added to the old buffer.
  lock.lock();
  buffer = std::make_shared<CommandTurn>();
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

void CommandBuffer::AddCommand(std::shared_ptr<Command> command) {
  lock.lock();
  buffer->Add(command);
  lock.unlock();
}

void CommandBuffer::AddCommands(std::shared_ptr<CommandTurn> commands) {
  lock.lock();
  for (const std::shared_ptr<const Command> command : *commands) {
    buffer->Add(command);
  }
}

void CommandBuffer::RegisterCommandSink(CommandSink* sink) {
  sinks.push_back(sink);
}

void CommandBuffer::SetGameHash(size_t gameHash) {
  lock.lock();
  buffer->SetHash(gameHash);
  lock.unlock();
}