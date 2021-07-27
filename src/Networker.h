#pragma once

#include <SFML/Network.hpp>
#include <iostream>

enum Connection_Type {
	CLIENT,
	SERVER
};

class TCPNetworker
{
private:
	sf::TcpSocket socket;
	sf::TcpListener listener;

	//Connection type is either "server" or "client"
	Connection_Type connectionType;

public:
	float secondsToTimeout = 5.0f;

	sf::Socket::Status socketStatus();

	void setBlockingMode(bool mode);
	bool getBlockingMode();

	bool isConnected();

	void connect(sf::IpAddress target_ip, unsigned int network_port);
	void disconnect();

	void startListening(unsigned int network_port);
	void stopListening();

	void sendPacket(sf::Packet &packet);
	void receivePacket(sf::Packet &receive_to);

	TCPNetworker(Connection_Type type);
	~TCPNetworker();
};