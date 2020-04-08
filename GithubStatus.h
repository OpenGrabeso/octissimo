//
// Created by Ondra on 8.4.2020.
//

#ifndef OCTISSIMO_GITHUBSTATUS_H
#define OCTISSIMO_GITHUBSTATUS_H

#include "win.h"
#include <Winhttp.h>

struct Status {
	const char *icon;
	const char *message;
	const char *timestamp;

	Status() {
		icon = nullptr;
		message = nullptr;
		timestamp = nullptr;
	}
};

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

class GithubStatus {
	Status status;

	WinHttpHandle hSession;
	WinHttpHandle hConnect;

	[[nodiscard]] const Status &getStatus() const {return status;}

	public:
	GithubStatus();
	~GithubStatus();

	void update(void (*callback)(const Status &));
};


#endif //OCTISSIMO_GITHUBSTATUS_H
