// MyEmotiv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "IEmoStateDLL.h"
#include "Iedk.h"
#include "IedkErrorCode.h"

#ifdef _WIN32
#include <conio.h>
#pragma comment(lib, "Ws2_32.lib")
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0700
#define _UNICODE
#define _AFXDLL

#include <afxwin.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <propvarutil.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")
#pragma once
#endif
#if __linux__ || __APPLE__
#include <unistd.h>
#include <termios.h>
#include <thread>
#endif

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace std;
using boost::asio::ip::tcp;

bool isCooldown;
bool isFrown;
ofstream file;

#ifdef _WIN32
UINT cooldown(LPVOID pParam)
{
	int* time = (int*)pParam;
	isCooldown = true;
	Sleep(*time);
	isCooldown = false;
	return 0;
}
#endif

char **argvt;
#ifdef _WIN32
UINT startClient(LPVOID pParam)
{
	boost::asio::io_service io_service;

	tcp::resolver resolver(io_service);
	tcp::resolver::query query(argvt[1], "daytime");
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::resolver::iterator end;

	tcp::socket socket(io_service);
	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_iterator != end)
	{
		socket.close();
		socket.connect(*endpoint_iterator++, error);
	}
	if (error)
		throw boost::system::system_error(error);

	for (;;)
	{
		boost::array<char, 128> buf;
		boost::system::error_code error;

		size_t len = socket.read_some(boost::asio::buffer(buf), error);

		if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
		else if (error)
			throw boost::system::system_error(error); // Some other error
	}
}
#endif

#if __linux__ || __APPLE__
void cooldown(int time)
{
	isCooldown = true;
	usleep(time);
	isCooldown = false;
}
#endif

void runBackgroundCoolDown(int* cooldownTime) {
#ifdef _WIN32
	AfxBeginThread(cooldown, (LPVOID)cooldownTime);
#endif
#if __linux__ || __APPLE__
	cooldown(*cooldownTime);
#endif
}

void writeToFile(string status) {
	file.open("status.txt", ios::out);
	file.clear();
	file << status;
	file.close();
}

int main(int argc, char **argv)
{
	argvt = argv;
	AfxBeginThread(startClient, (LPVOID)argv);
	string receiverHost = "localhost";
	if (argc > 2) {
		cout << "Usage: " << argv[0] << " <hostname>" << endl;
		cout << "The arguments specify the host of the head model "
			"(Default: localhost)" << endl;
		return 1;
	}
	if (argc > 1) {
		receiverHost = string(argv[1]);
	}

	EmoEngineEventHandle eEvent = IEE_EmoEngineEventCreate();
	EmoStateHandle eState = IEE_EmoStateCreate();
	unsigned int userID = 0;
	int cooldownTime = 2000;
	int* cooldownPtr = &cooldownTime;
	const int CONTROL_PANEL_PORT = 3008;
	try {
		if (IEE_EngineConnect() != EDK_OK) {
			throw runtime_error("Emotiv Driver start up failed.");
		}
		else {
			cout << "Emotiv Driver started!" << endl;
		}

		int startSendPort = 30000;
		cout << "Prepare to scan for frowns" << endl;
		while (true) {
			int state = IEE_EngineGetNextEvent(eEvent);
			if (isCooldown) continue;
			if (state == EDK_OK) {
				IEE_Event_t eventType = IEE_EmoEngineEventGetType(eEvent);
				IEE_EmoEngineEventGetUserId(eEvent, &userID);
				switch (eventType) {
				case IEE_UserAdded:
				{
					cout << endl << "New user " << userID
						<< " added, sending FacialExpression animation to ";
					cout << receiverHost << ":" << startSendPort << "..."
						<< endl;
					++startSendPort;
					break;
				}
				case IEE_UserRemoved:
				{
					cout << endl << "User " << userID
						<< " has been removed." << endl;
					break;
				}
				case IEE_EmoStateUpdated:
				{
					IEE_EmoEngineEventGetEmoState(eEvent, eState);

					IEE_FacialExpressionAlgo_t upperFaceType = IS_FacialExpressionGetUpperFaceAction(eState);
					float upperFaceVol = IS_FacialExpressionGetUpperFaceActionPower(eState);
					IEE_FacialExpressionAlgo_t lowerFaceType = IS_FacialExpressionGetLowerFaceAction(eState);
					float lowerFaceVol = IS_FacialExpressionGetLowerFaceActionPower(eState);

					if (!IS_FacialExpressionIsBlink(eState) && !isFrown && upperFaceVol>0.5) {
						if (upperFaceType == FE_FROWN) {
							runBackgroundCoolDown(cooldownPtr);
							cout << "Frown " << upperFaceVol << endl;
							//sendMessage("1");
							//Change Brightness + video speed
							isFrown = true;
						}
					} else if (isFrown && lowerFaceType==FE_SMILE && lowerFaceVol>0.0) {
						isFrown = false;
						cout << "Back to normal" << endl;
						//sendMessage("0");
						//Change settings back to normal
					}
					break;
				}
				case IEE_FacialExpressionEvent: {}
				default:
					break;
				}
			}
			else if (state != EDK_NO_EVENT) {
				cout << endl << "Internal error in Emotiv Engine!"
					<< endl;
				break;
			}
		}
	}
	catch (const runtime_error& e) {
		cerr << e.what() << endl;
		cout << "Press 'Enter' to exit..." << endl;
		getchar();
	}


	system("pause");
	return 0;
}

