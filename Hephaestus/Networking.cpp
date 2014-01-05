#include "stdafx.h"
#include "Networking.h"

#include "SelectCommand.h"
BOOST_CLASS_EXPORT(SelectCommand)
#include "MoveCommand.h"
BOOST_CLASS_EXPORT(MoveCommand)
#include "AttackCommand.h"
BOOST_CLASS_EXPORT(AttackCommand)
#include "AttackMoveCommand.h"
BOOST_CLASS_EXPORT(AttackMoveCommand)

using boost::asio::ip::tcp;

NetworkConnection::NetworkConnection() { }

NetworkConnection::~NetworkConnection() {
	Disconnect();
}

void NetworkConnection::AcceptClient(int port) {
	is_host_ = true;
	port_ = port;
	tcp::acceptor acceptor(io_service_, tcp::endpoint(tcp::v4(), port));
	acceptor.accept(*tcp_stream_.rdbuf());
}

void NetworkConnection::Connect(const std::string &hostname,
								const std::string &port) {
	is_host_ = false;
	hostname_ = hostname;
	service_ = port;
	tcp_stream_.connect(hostname, port);
}

void NetworkConnection::Disconnect() {
	boost::system::error_code error;
	tcp_stream_.close();
}

void NetworkConnection::Reconnect() {
	Disconnect();
	if (is_host_) {
		AcceptClient(port_);
	} else {
		Connect(hostname_, service_);
	}
}

void NetworkConnection::SendCommands(const CommandTurn *commands) {	
	boost::archive::text_oarchive serializer(tcp_stream_);
	serializer << const_cast<const CommandTurn * const>(commands);
	tcp_stream_.flush();
}

const CommandTurn *NetworkConnection::ReceiveCommands() {
	boost::archive::text_iarchive deserializer(tcp_stream_);
	CommandTurn *commands;
	deserializer >> commands;
	return commands;
}