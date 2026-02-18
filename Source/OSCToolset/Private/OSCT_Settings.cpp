// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved.
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "OSCT_Settings.h"

UOSCT_Settings::UOSCT_Settings()
{
	ServerAddress = "0.0.0.0";
	ServerPort = 9999;
	ClientAddress = "127.0.0.1";
	ClientPort = 9998;
	MulticastLoopback = false;
	UseLocalIPV4 = false;
}
