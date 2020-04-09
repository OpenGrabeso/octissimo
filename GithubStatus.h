//
// Created by Ondra on 8.4.2020.
//

#ifndef OCTISSIMO_GITHUBSTATUS_H
#define OCTISSIMO_GITHUBSTATUS_H

#include "win.h"
#include <string>
#if defined __MINGW32__ && !defined __MINGW64__
#include <mingw.thread.h>
#include <mingw.condition_variable.h>
#include <mingw.mutex.h>
#else
#include <thread>
#endif
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h" // https://github.com/yhirose/cpp-httplib

struct Status {
	std::string icon;
	std::string message;
	std::string timestamp;
};

class GithubStatus {
	Status status;

	httplib::SSLClient cli;

	[[nodiscard]] const Status &getStatus() const {return status;}

	static constexpr auto url = "kctbh9vrtdwd.statuspage.io";
	static constexpr auto endpoint = "/api/v2/status.json";

	public:
	GithubStatus();
	~GithubStatus();

	void update(void (*callback)(const Status &));
};


#endif //OCTISSIMO_GITHUBSTATUS_H
