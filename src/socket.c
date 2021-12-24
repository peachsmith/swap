/*
	Functions for performing socket operations
	Copyright (C) 2017 John Powell

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "swap/socket.h"
#include <stdio.h>

#if defined(__unix__) || defined(__linux__)

// static int last_error = 0;

#endif // __unix__ || __linux__

int jep_socket_init()
{
	int result = 0;

#ifdef _WIN32
	WSADATA wsaData;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	return result;
}

int jep_socket_quit()
{
	int result = 0;
#ifdef _WIN32
	result = WSACleanup();
#endif
	return result;
}

int jep_get_addr_info(jep_addrinf **inf, const char* address, const char* port)
{
	int result = 0;

#ifdef _WIN32
	jep_addrinf hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	result = getaddrinfo(address, port, &hints, inf);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	jep_addrinf hints;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	result = getaddrinfo(address, port, &hints, inf);
#endif

	return result;
}

void jep_free_addrinf(jep_addrinf* inf)
{
#ifdef _WIN32
	freeaddrinfo(inf);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	freeaddrinfo(inf);
#endif
}

jep_socket jep_socket_create(jep_addrinf *inf)
{
	jep_socket s = JEP_INVALID_SOCKET;

#ifdef _WIN32
	s = socket(inf->ai_family, inf->ai_socktype, inf->ai_protocol);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	s = socket(inf->ai_family, inf->ai_socktype, inf->ai_protocol);
#endif
	return s;
}

int jep_socket_bind(jep_socket s, jep_addrinf *inf)
{
	int result = 0;

#ifdef _WIN32
	result = bind(s, inf->ai_addr, (int)inf->ai_addrlen);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = bind(s, inf->ai_addr, inf->ai_addrlen);
#endif

	return result;
}

int jep_socket_listen(jep_socket s, int backlog)
{
	int result = 0;

#ifdef _WIN32
	result = listen(s, backlog);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = listen(s, backlog);
#endif

	return result;
}

int jep_socket_connect(jep_socket s, jep_addrinf* inf)
{
	int result = 0;

#ifdef _WIN32
	result = connect(s, inf->ai_addr, (int)inf->ai_addrlen);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = connect(s, inf->ai_addr, inf->ai_addrlen);
#endif

	return result;
}

jep_socket jep_socket_accept(jep_socket s, jep_addr* addr, int* addrlen)
{
	jep_socket result = 0;

#ifdef _WIN32
	result = accept(s, addr, addrlen);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = accept(s, addr, (socklen_t*)addrlen);
#endif

	return result;
}

int jep_socket_send(jep_socket s, char* buffer, size_t len, int flags)
{
	int result = 0;

#ifdef _WIN32
	result = send(s, buffer, len, flags);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = write(s, buffer, len);
#endif

	return result;
}

int jep_socket_receive(jep_socket s, unsigned char* buffer, size_t len, int flags)
{
	int result = 0;

#ifdef _WIN32
	result = recv(s, buffer, len, flags);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = read(s, buffer, len);
#endif

	return result;
}

int jep_socket_close(jep_socket s)
{
#ifdef _WIN32
	return closesocket(s);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	return close(s);
#endif
}

int jep_socket_shutdown(jep_socket s, int how)
{
	int result = 0;

#ifdef _WIN32
	result = shutdown(s, how);
#elif defined(__unix__) || defined(__linux__) || defined(__MACH__)
	result = shutdown(s, how);
#endif

	return result;
}

int jep_socket_get_error()
{
	int err = 0;

#ifdef _WIN32
	err = WSAGetLastError();
#endif

	return err;
}
