//
// Created by Ondra on 8.4.2020.
//

#ifndef OCTISSIMO_GITHUBSTATUS_H
#define OCTISSIMO_GITHUBSTATUS_H

struct Status {
	const char *icon;
	const char *message;
	const char *timestamp;
};

class GithubStatus {
	Status status;

	[[nodiscard]] const Status &getStatus() const {return status;}

	public:
	void update(void (*callback)(const Status &));
};


#endif //OCTISSIMO_GITHUBSTATUS_H
