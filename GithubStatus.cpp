//
// Created by Ondra on 8.4.2020.
//

#include "GithubStatus.h"
#include "win.h"
#include "json.hpp"
#include <Winhttp.h>

using json = nlohmann::json;

GithubStatus::GithubStatus() {

	hSession = WinHttpOpen(L"Octissimo/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
								  WINHTTP_NO_PROXY_BYPASS, 0);
	// Specify an HTTP server.
	if (hSession) {
		hConnect = WinHttpConnect(hSession, url, INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

}

GithubStatus::~GithubStatus() {

}

void GithubStatus::update(void (*callback)(const Status &s)) {

	try {
		std::string response;

		BOOL bResults = FALSE;
		WinHttpHandle hRequest;
		if (hConnect) {
			hRequest = WinHttpOpenRequest(
					  hConnect, L"GET", endpoint, NULL,
					  WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE
			);
		}
		if (hRequest) {
			bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		}
		if (bResults) {
			bResults = WinHttpReceiveResponse(hRequest, NULL);/**/
			// Keep checking for data until there is nothing left.
			DWORD dwSize = 0;
			do {
				// Check for available data.
				dwSize = 0;
				if (WinHttpQueryDataAvailable(hRequest, &dwSize)) {
					auto pszOutBuffer = std::vector<unsigned char>(dwSize + 1);
					ZeroMemory(pszOutBuffer.data(), dwSize + 1);
					DWORD dwDownloaded = 0;
					if (!WinHttpReadData(hRequest, (LPVOID) pszOutBuffer.data(), dwSize, &dwDownloaded) ) {
						break; // strange - we were told to expect some data
					} else {
						// we have the json now
						response += (const char *)pszOutBuffer.data();
					}
				}
			} while (dwSize > 0);
		}

		auto result = json::parse(response);

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
	} catch (...) {

	}
}


