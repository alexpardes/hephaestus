#include "stdafx.h"
#include "CommandBuffer.h"
#include "CommandTurn.h"

// TODO: Ensure thread safety.
CommandBuffer::CommandBuffer() {
  bufferTurn = std::make_shared<CommandTurn>();
}

std::shared_ptr<CommandTurn> CommandBuffer::TakeCommandTurn() {
  EmptyBuffer();
  auto turnToExecute = turnQueue.front();
  turnQueue.pop();
  return turnToExecute;
}

void CommandBuffer::CreateTurnDelay(int nTurns) {
  for (int i = 0; i < nTurns; ++i) {
    EmptyBuffer();
  }
}

void CommandBuffer::AddCommand(std::shared_ptr<Command> command) {
  lock.lock();
  bufferTurn->Add(command);
  lock.unlock();
}

void CommandBuffer::AddCommands(std::shared_ptr<CommandTurn> commands) {
  turnQueue.push(commands);
  SendCommands(commands);
}

void CommandBuffer::RegisterCommandSink(CommandSink* sink) {
  sinks.push_back(sink);
}

void CommandBuffer::SetGameHash(size_t gameHash) {
  lock.lock();
  bufferTurn->SetHash(gameHash);
  lock.unlock();
}

void CommandBuffer::SendCommands(std::shared_ptr<CommandTurn> commands) const {
  for (CommandSink* sink : sinks) {
    sink->AddCommands(commands);
  }
}

void CommandBuffer::EmptyBuffer() {
  // Makes sure that no more commands will be added to the old buffer.
  lock.lock();
  AddCommands(bufferTurn);
  bufferTurn = std::make_shared<CommandTurn>();
  lock.unlock();
}