//
// Created by Ondra on 9.4.2020.
//

#ifndef OCTISSIMO_REQUEST_H
#define OCTISSIMO_REQUEST_H

#include <functional>
#include "win.h"
#include <Winhttp.h>
#include <string>
#include <map>

using std::map;
using std::string;

std::wstring widestring(std::string s);

class WinHttpHandle {
	HINTERNET handle;

	public:
	WinHttpHandle() {
		handle = nullptr;
	}

	~WinHttpHandle() {
		if (handle) WinHttpCloseHandle(handle);
	}

	WinHttpHandle &operator = (HINTERNET h) {handle = h;return *this;}
	operator HINTERNET () const {return handle;}
	operator bool () const {return handle != nullptr;}
};



class Request {
	WinHttpHandle hSession;
	WinHttpHandle hConnect;


	public:
	Request(const char *url);

	void update(const char *endpoint, std::function<void(const std::string &, const std::map<string, string> &)> callback, string headers = "");
};


#endif //OCTISSIMO_REQUEST_H
