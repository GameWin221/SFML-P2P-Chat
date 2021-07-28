#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>

enum Connection_Type {
	CLIENT,
	SERVER
};

class TCPNetworker
{
private:
	sf::TcpSocket socket;
	sf::TcpListener listener;

	std::thread request_thread;

	sf::TcpSocket request_socket;
	sf::TcpListener request_listener;

	bool is_responding = false;

	//Connection type is either "server" or "client"
	Connection_Type connectionType;

	void request_check_loop();

public:
	float secondsToTimeout = 2.0f;

	sf::Socket::Status socketStatus();

	void setBlockingMode(bool mode);
	bool getBlockingMode();

	bool isConnected();

	void connect(sf::IpAddress target_ip, unsigned int network_port);
	void disconnect(bool send_disconnect_request);

	void request(std::string request_name);
	void respond();

	void startListening(unsigned int network_port);
	void stopListening();

	void sendPacket(sf::Packet &packet);
	void receivePacket(sf::Packet &receive_to);

	TCPNetworker(Connection_Type type);
	~TCPNetworker();
};