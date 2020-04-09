//
// Created by Ondra on 8.4.2020.
//

#include "GithubStatus.h"
#include "win.h"
#include "json.hpp"

using json = nlohmann::json;

GithubStatus::GithubStatus():cli(url, 443) {
}

GithubStatus::~GithubStatus() {

}

void GithubStatus::update(void (*callback)(const Status &s)) {
	//try {
		auto res = cli.Get(endpoint);

		if (res) {

			auto result = json::parse(res->body);

			// https://www.githubstatus.com/api/#status
			/*
			{
			  "page":{
				 "id":"kctbh9vrtdwd",
				 "name":"GitHub",
				 "url":"https://www.githubstatus.com",
				 "time_zone":"Etc/UTC",
				 "updated_at":"2020-04-08T08:09:34.236Z"
			  },
			  "status":{
				 "indicator":"none",
				 "description":"All Systems Operational"
			  }
			}
			*/

			status.icon = result["status"]["indicator"].get<std::string>();
			status.message = result["status"]["description"].get<std::string>();
			status.timestamp = result["page"]["updated_at"].get<std::string>();
			callback(status);
		}
		/*
	} catch (...) {

	}*/
}


