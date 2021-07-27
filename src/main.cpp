#include <SFML/Graphics.hpp>

#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono> 

#include "Networker.h"

TCPNetworker* network;

std::vector<sf::Text> lines_of_text;
sf::Font font;

std::string nickname = "";

const int max_messages = 10;

//----Chat-functions----
void AddText(std::string text)
{
	if (lines_of_text.size() + 1 > max_messages)
	{
		lines_of_text.erase(lines_of_text.begin());
		for (int i = 0; i < lines_of_text.size(); i++)
			lines_of_text[i].move(0, -40);
	}

	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	std::string time_str;

	sf::Text text_line;
	text_line.setFont(font);
	text_line.setPosition(5, (40 * lines_of_text.size()));
	text_line.setString(time_str + text);
	lines_of_text.push_back(text_line);
}
void UpdateChat()
{
	sf::Packet incoming;
	while (network->isConnected())
	{
		network->receivePacket(incoming);

		if (incoming.getDataSize() > 0)
		{
			std::string incoming_text = "";
			incoming >> incoming_text;

			AddText(incoming_text);

			if (incoming_text == "(Networker): User disconnected!")
				network->disconnect();
		}

		incoming.clear();
	}
}
//----------------------

int main()
{
	//-----Input-field-----
	std::string message = "_";

	sf::Packet message_packet;

	font.loadFromFile("Assets/Lato.ttf");

	sf::Text inputField;
	inputField.setFont(font);
	inputField.setPosition(10, 450);
	inputField.setString(message);
	//---------------------

	//--Selecting-chat-type--
	std::cout << "Enter 's' for server, 'c' for client!\n";
	char selected_type = ' ';
	std::cin >> selected_type;
	//-----------------------

	if (selected_type == 's')
	{
		std::cout << "\nYour server's ip: " << sf::IpAddress::getLocalAddress() << "\n";
		network = new TCPNetworker(SERVER);
		network->startListening(2000);
	}
	else if (selected_type == 'c')
	{
		std::string ip_string = "";
		sf::IpAddress server_ip;

		std::cout << "Type in the server's ip:\n";
		std::cin >> ip_string;

		server_ip = (ip_string == "local") ? sf::IpAddress::getLocalAddress() : ip_string;

		network = new TCPNetworker(CLIENT);
		network->connect(server_ip, 2000);
	}

	std::cout << "Type in your nickname:\n";
	std::cin >> nickname;

	sf::RenderWindow window(sf::VideoMode(1200, 500), "LAN Chat User '" +nickname+ "' (" + sf::IpAddress::getLocalAddress().toString() + ")", sf::Style::Close);

	network->setBlockingMode(true);
	std::thread packet_update(UpdateChat);

	while (window.isOpen())
	{
		sf::Event ev;
		while (window.pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed) 
			{
				network->disconnect();
				window.close();
				return 0;
			}

			
			if (ev.type == sf::Event::TextEntered)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))			//Sending message packet
				{
					//Removing the '_'
					message.erase(message.length() - 1);

					message_packet << ("(" + nickname + "): " + message);
					network->sendPacket(message_packet);
					message_packet.clear();

					AddText("("+ nickname + "): " + message);
					message = "_";
					inputField.setString(message);
				}
				else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))  //Adding characters to the message
				{
					//Removing the '_'
					message.erase(message.length() - 1);

					message += ev.text.unicode;
					message += "_";
					inputField.setString(message);
				}
				else															//Erasing last character from the message
				{
					if (message.length() > 1)
					{
						message.erase(message.length() - 2, 1);
						inputField.setString(message);
					}
				}
			}
		}
		
		//----Drawing-text----
		window.clear(sf::Color(45, 45, 55, 255));

		window.draw(inputField);
		for (int i = 0; i < lines_of_text.size(); i++)
			window.draw(lines_of_text[i]);

		window.display();
		//------------------
	}
	
	packet_update.join();
	network->~TCPNetworker();

	system("pause");

	return 0;
}