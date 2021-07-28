#include "Networker.h"

TCPNetworker::TCPNetworker(Connection_Type type)
{
	this->connectionType = type;
}
TCPNetworker::~TCPNetworker()
{
	std::cout << "Shutting down the networker...\n";

	this->disconnect(true);

	this->request_thread.join();

	std::cout << "Networker is off!\n";
}

sf::Socket::Status TCPNetworker::socketStatus()
{
	if (this->socket.Done)
		return sf::Socket::Status::Done;

	else if (this->socket.NotReady)
		return sf::Socket::Status::NotReady;

	else if (this->socket.Partial)
		return sf::Socket::Status::Partial;

	else if (this->socket.Disconnected)
		return sf::Socket::Status::Disconnected;

	else if (this->socket.Error)
		return sf::Socket::Status::Error;
}

void TCPNetworker::setBlockingMode(bool mode)
{
	this->socket.setBlocking(mode);
}
bool TCPNetworker::getBlockingMode()
{
	return this->socket.isBlocking();
}

bool TCPNetworker::isConnected()
{
	if (this->socket.getRemoteAddress() == sf::IpAddress::None)
		return false;
	else
		return true;
}

void TCPNetworker::connect(sf::IpAddress target_ip, unsigned int network_port)
{
	std::cout << "Attempting to connect to " << target_ip << "\n";

	this->socket.connect(target_ip, network_port, sf::seconds(this->secondsToTimeout));
	this->request_socket.connect(target_ip, network_port + 128, sf::seconds(this->secondsToTimeout));

	if(!is_responding)
		this->request_thread = std::thread(&TCPNetworker::request_check_loop, this);

	is_responding = true;

	if (socket.getRemoteAddress() == sf::IpAddress::None || request_socket.getRemoteAddress() == sf::IpAddress::None)
	{
		std::cout << "Timeout error! - Failed to connect to the server in " << this->secondsToTimeout << " seconds.\n";
		this->~TCPNetworker();
		exit(NULL);
	}
	else
		std::cout << "Successfully connected to " << socket.getRemoteAddress() << "\n";
}
void TCPNetworker::disconnect(bool send_disconnect_request)
{
	if (send_disconnect_request)
	{
		sf::Packet disconnect_message;
		std::string disconnect_string = "(Networker) : User disconnected!";
		disconnect_message << disconnect_string;

		this->sendPacket(disconnect_message);
		request("Disconnect");
	}

	this->socket.disconnect();
	this->request_socket.disconnect();

	if (this->socket.Disconnected)
		std::cout << "\nDisconnected! \n";
	else
	{
		std::cout << "Failed to disconnect!\n";
		exit(NULL);
	}
}

void TCPNetworker::request(std::string request_name)
{
	sf::Packet request_packet;
	request_packet << request_name;
	this->request_socket.send(request_packet);
}

//You can add your request-response's down here:
void TCPNetworker::respond()
{
	std::string request_name = "\0";

	sf::Packet request_packet;
	this->request_socket.receive(request_packet);
	request_packet >> request_name;

	if (request_name == "Disconnect")
		this->disconnect(false);

	//Add your new requests down here:
	//else if(request_name "your_request_name")
	//
}

void TCPNetworker::request_check_loop()
{
	while (this->isConnected())
	{
		respond();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void TCPNetworker::startListening(unsigned int network_port)
{
	if (this->connectionType == SERVER)
	{
		std::cout << "\nListening for the client... \n";

		this->listener.listen(network_port);
		this->listener.accept(this->socket);

		this->request_listener.listen(network_port + 128);
		this->request_listener.accept(this->request_socket);

		if(!is_responding)
			this->request_thread = std::thread(&TCPNetworker::request_check_loop, this);

		is_responding = true;

		std::cout << "Connected to the client!\n";
	}
	else
		std::cout << "Can't start listening for packets in CLIENT mode!\n To begin listening, Networker must be in SERVER mode.\n";
}
void TCPNetworker::stopListening()
{
	if (this->connectionType == SERVER)
	{
		this->listener.close();
		this->request_listener.close();
		std::cout << "Server stopped listening for the client's packets!\n";
	}
	else
		std::cout << "Can't stop listening for packets in CLIENT mode!\n To stop listening, Networker must be in SERVER mode.\n";
}

void TCPNetworker::sendPacket(sf::Packet &packet)
{
	this->socket.send(packet);
}
void TCPNetworker::receivePacket(sf::Packet &receive_to)
{
	this->socket.receive(receive_to);
}