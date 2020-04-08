//
// Created by Ondra on 8.4.2020.
//

#include "GithubStatus.h"
#include "win.h"

#include <Winhttp.h>

GithubStatus::GithubStatus() {
	hSession = WinHttpOpen(L"Octissimo/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
								  WINHTTP_NO_PROXY_BYPASS, 0);
	// Specify an HTTP server.
	if (hSession) {
		hConnect = WinHttpConnect(hSession, L"status.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

}

GithubStatus::~GithubStatus() {

}

void GithubStatus::update(void (*callback)(const Status &s)) {

	try {
		//Variables
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPBYTE pszOutBuffer;


		BOOL bResults = FALSE;
		WinHttpHandle hRequest;
		if (hConnect)
			hRequest = WinHttpOpenRequest(
					  hConnect, L"GET", L"/xyz/1.txt", NULL,
					  WINHTTP_NO_REFERER, NULL, WINHTTP_FLAG_SECURE
			);
		if (hRequest)
			bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		if (bResults) {
			bResults = WinHttpReceiveResponse(hRequest, NULL);/**/
			// Keep checking for data until there is nothing left.
			do {
				// Check for available data.
				dwSize = 0;
				if (WinHttpQueryDataAvailable(hRequest, &dwSize)) {
					// Allocate space for the buffer.
					pszOutBuffer = new unsigned char[dwSize + 1];
					// Read the Data.
					ZeroMemory(pszOutBuffer, dwSize + 1);
					if (!WinHttpReadData(hRequest, (LPVOID) pszOutBuffer, dwSize, &dwDownloaded) || dwDownloaded != dwSize) {
						throw "Error";
					} else {
						// we have the json now
					}
					// Free the memory allocated to the buffer.
					delete[] pszOutBuffer;
				}
			} while (dwSize > 0);
		}

		status.icon = "normal";
		status.message = "All Systems Operational";
		status.timestamp = "???";
		callback(status);
	} catch (...) {

	}
}


