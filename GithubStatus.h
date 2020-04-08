//
// Created by Ondra on 8.4.2020.
//

#ifndef OCTISSIMO_GITHUBSTATUS_H
#define OCTISSIMO_GITHUBSTATUS_H

#include "win.h"
#include <Winhttp.h>
#include <string>

struct Status {
	std::string icon;
	std::string message;
	std::string timestamp;
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

	static constexpr auto url = L"kctbh9vrtdwd.statuspage.io";
	static constexpr auto endpoint = L"/api/v2/status.json";

	public:
	GithubStatus();
	~GithubStatus();

	void update(void (*callback)(const Status &));
};


#endif //OCTISSIMO_GITHUBSTATUS_H
