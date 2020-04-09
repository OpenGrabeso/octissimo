//
// Created by Ondra on 9.4.2020.
//

#ifndef OCTISSIMO_REQUEST_H
#define OCTISSIMO_REQUEST_H

#include <functional>
#include "win.h"
#include <Winhttp.h>
#include <string>

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
	Request(const wchar_t *url);

	void update(const wchar_t *endpoint, std::function<void(const std::string &)> callback);
};


#endif //OCTISSIMO_REQUEST_H
