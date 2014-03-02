#include "stdafx.h"
#include "NetworkManager.h"
#include "NetworkConnection.h"

#include <boost/exception/diagnostic_information.hpp> 

using boost::asio::ip::tcp;

NetworkManager::NetworkManager() : work(io_service_) {
  acceptor = nullptr;
  thread = new std::thread([this]() {
    io_service_.run();
  });
  socket = new tcp::socket(io_service_);
}

NetworkManager::~NetworkManager() {
	Reset();
}

void NetworkManager::Cancel() {
  socket->close();
}

NetworkManager::AsioHandler
NetworkManager::CreateHandler(ConnectionHandler handler) {
  return [this, handler] (const boost::system::error_code& e) {
    NetworkConnection* result = nullptr;
    if (!e) {
      result = new NetworkConnection(socket /*&tcp_stream_*/);
    }
    try {handler(result); }
    catch (boost::system::system_error& e) { std::cout << e.what(); }
  };
}

void NetworkManager::AcceptClient(int port, ConnectionHandler handler) {
	acceptor = new tcp::acceptor(io_service_, tcp::endpoint(tcp::v4(), port));
  //socket = new tcp::socket(io_service_);
	acceptor->async_accept(*socket /**tcp_stream_.rdbuf()*/, CreateHandler(handler));
}

NetworkManager::AsioJoinHandler
NetworkManager::CreateJoinHandler(ConnectionHandler handler) {
  return [this, handler](const boost::system::error_code& e,
      boost::asio::ip::tcp::resolver::iterator it) {
    NetworkConnection* result = nullptr;
    if (!e) {
      result = new NetworkConnection(socket);
    }
    handler(result);
  };
}

void NetworkManager::Connect(const std::string &hostname,
                             const std::string &port,
                             ConnectionHandler handler) {

  tcp::resolver resolver(io_service_);
  tcp::resolver::query query(hostname, port);
  tcp::resolver::iterator endpoints = resolver.resolve(query);

  //try {
  //socket = new tcp::socket(io_service_);
  boost::asio::async_connect(*socket, endpoints, CreateJoinHandler(handler));
  //} catch (boost::system::system_error& e) {
  //  std::ofstream log("errorlog.txt");
  //  log << e.what() << std::endl;
  //  log << boost::diagnostic_information(e);
  //}

  //socket->async_connect(endpoint, [this, handler, socket](const boost::system::error_code& e) {
  //    tcp_stream_.rdbuf()->assign(tcp::v4(), socket->native());
  //    handler(new NetworkConnection(tcp_stream_));
  //});
  //boost::asio::async_connect(*tcp_stream_.rdbuf(), endpoints, Handler);
}

void NetworkManager::Reset() {
  socket->close();
}

void NetworkManager::Disconnect() {
	boost::system::error_code error;
	tcp_stream_.close();
}

//void NetworkManager::SendCommands(const CommandTurn *commands) {	
//	boost::archive::text_oarchive serializer(tcp_stream_);
//	serializer << const_cast<const CommandTurn * const>(commands);
//	tcp_stream_.flush();
//}
//
//const CommandTurn *NetworkManager::ReceiveCommands() {
//	boost::archive::text_iarchive deserializer(tcp_stream_);
//	CommandTurn *commands;
//	deserializer >> commands;
//	return commands;
//}