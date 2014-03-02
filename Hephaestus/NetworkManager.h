#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "Command.h"
#include "Util.h"

class NetworkConnection;
typedef std::function<void(NetworkConnection*)> ConnectionHandler;

class NetworkManager {
	public:
		NetworkManager();
		~NetworkManager();
		void SendCommands(const CommandTurn *commands);
		const CommandTurn *ReceiveCommands();
    void AcceptClient(int port, ConnectionHandler handler);
    void Cancel();

		void Connect(const std::string &hostname, const std::string &port,
        ConnectionHandler handler);

		void Disconnect();
    void Reset();

	private:
    static void Run(NetworkManager* manager);
    typedef std::function<void(const boost::system::error_code&)> AsioHandler;
    typedef std::function<void(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator iterator)> AsioJoinHandler;
    AsioHandler CreateHandler(ConnectionHandler handler);
    AsioJoinHandler CreateJoinHandler(ConnectionHandler handler);
		boost::asio::io_service io_service_;
    boost::asio::io_service::work work;
		boost::asio::ip::tcp::iostream tcp_stream_;
    std::thread* thread;    
    boost::asio::ip::tcp::acceptor* acceptor;
    boost::asio::ip::tcp::socket* socket;
};