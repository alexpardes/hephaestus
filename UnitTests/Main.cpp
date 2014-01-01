#define CATCH_CONFIG_MAIN

#include <Hephaestus/stdafx.h>
#include <Hephaestus/Commands.h>
BOOST_CLASS_EXPORT(SelectCommand)
BOOST_CLASS_EXPORT(MoveCommand)
BOOST_CLASS_EXPORT(AttackCommand)
BOOST_CLASS_EXPORT(AttackMoveCommand)

#include "catch.hpp"

using std::list;

TEST_CASE("Command Stack Serialization") {
	list<unitId> targets;
	targets.push_back(3);
	targets.push_back(4);
	const SelectCommand stack_select = SelectCommand(targets);

	std::stringstream stream;
	boost::archive::text_oarchive oarchive(stream);
	oarchive << stack_select;

	SelectCommand stream_select;
	boost::archive::text_iarchive iarchive(stream);
	iarchive >> stream_select;

	REQUIRE(stream_select.targets().front() == 3);
	REQUIRE(stream_select.targets().back() == 4);
}

TEST_CASE("Command Heap Serialization") {
	MoveCommand* heap_move = new MoveCommand(Vector2i(2, 3));
	Command* heap_command = (Command*) heap_move;

	std::stringstream stream;
	boost::archive::text_oarchive oarchive(stream);
	oarchive << heap_command;
	delete heap_move;

	Command* stream_command = NULL;
	boost::archive::text_iarchive iarchive(stream);
	iarchive >> stream_command;
	MoveCommand* stream_move = dynamic_cast<MoveCommand*>(stream_command);
	REQUIRE(stream_move != NULL);

	REQUIRE(stream_move->location().x == 2);
	REQUIRE(stream_move->location().y == 3);
}