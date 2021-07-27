#include "Networker.h"

TCPNetworker::TCPNetworker(Connection_Type type)
{
	this->connectionType = type;
}
TCPNetworker::~TCPNetworker()
{
	std::cout << "Shutting down the networker...\n";

	this->socket.disconnect();

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

	if (socket.getRemoteAddress() == sf::IpAddress::None)
	{
		std::cout << "Timeout error! - Failed to connect to the server in " << this->secondsToTimeout << " seconds.\n";
		exit(NULL);
	}
	else
		std::cout << "Successfully connected to " << socket.getRemoteAddress() << "\n";
}
void TCPNetworker::disconnect()
{
	sf::Packet disconnect_packet;
	std::string disconnect_message = "(Networker): User disconnected!";
	disconnect_packet << disconnect_message;
	this->socket.send(disconnect_packet);

	this->socket.disconnect();

	if(this->socket.Disconnected)
		std::cout << "\nSuccessfully disconnected from the server! \n";
	else
	{ 
		std::cout << "Failed to disconnect from the server!\n";
		exit(NULL);
	}
}

void TCPNetworker::startListening(unsigned int network_port)
{
	if (this->connectionType == SERVER)
	{
		std::cout << "\nListening for the client... \n";

		this->listener.listen(network_port);
		this->listener.accept(this->socket);

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