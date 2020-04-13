//
// Created by Ondra on 8.4.2020.
//

#ifndef OCTISSIMO_GITHUBSTATUS_H
#define OCTISSIMO_GITHUBSTATUS_H

#include <functional>
#include "Request.h"

struct Status {
	std::string icon;
	std::string message;
	std::string timestamp;
};

class GithubStatus {
	Status status;
	Request request;

	[[nodiscard]] const Status &getStatus() const {return status;}

	static constexpr auto url = "kctbh9vrtdwd.statuspage.io";
	static constexpr auto endpoint = "/api/v2/status.json";

	public:
	GithubStatus();
	~GithubStatus();

	void update(std::function<void(const Status &)> callback);
};


#endif //OCTISSIMO_GITHUBSTATUS_H
