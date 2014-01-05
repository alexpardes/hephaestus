#ifndef NETWORKING_
#define NETWORKING_

#include "Command.h"
#include "Util.h"

class NetworkConnection {
	public:
		NetworkConnection();
		~NetworkConnection();
		void SendCommands(const CommandTurn *commands);
		const CommandTurn *ReceiveCommands();
		void AcceptClient(int port);
		void Connect(const std::string &hostname, const std::string &port);
		void Disconnect();
		void Reconnect();

	private:
		boost::asio::io_service io_service_;
		boost::asio::ip::tcp::iostream tcp_stream_;
		bool is_host_;
		int port_;
		std::string hostname_, service_;
};

#endif