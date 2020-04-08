//
// Created by Ondra on 8.4.2020.
//

#include "GithubStatus.h"

void GithubStatus::update(void (*callback)(const Status &s)) {
	status.icon = "normal";
	status.message = "All Systems Operational";
	status.timestamp = "???";
	callback(status);
}
