#include "stdafx.h"
#include "CommandSink.h"

CommandSink* CommandSink::Null = new CommandSink::NullCommandSink();