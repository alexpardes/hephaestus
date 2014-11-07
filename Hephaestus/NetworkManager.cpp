#include "stdafx.h"
#include "NetworkManager.h"
#include "NetworkConnection.h"

#include <boost/exception/diagnostic_information.hpp> 

using boost::asio::ip::tcp;

NetworkManager::NetworkManager() : work(ioService) {
  acceptor = nullptr;
  thread = new std::thread([this]() {
    ioService.run();
  });
  socket = new tcp::socket(ioService);
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
      result = new NetworkConnection(socket);
    }
    try {handler(result); }
    catch (boost::system::system_error& e) { std::cout << e.what(); }
  };
}

void NetworkManager::AcceptClient(int port, ConnectionHandler handler) {
	acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), port));
	acceptor->async_accept(*socket, CreateHandler(handler));
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

  tcp::resolver resolver(ioService);
  tcp::resolver::query query(hostname, port);
  tcp::resolver::iterator endpoints = resolver.resolve(query);
  boost::asio::async_connect(*socket, endpoints, CreateJoinHandler(handler));
}

void NetworkManager::Reset() {
  socket->close();
}