//
// Created by Ondra on 9.4.2020.
//

#include "Request.h"
#include <vector>
#include <algorithm>

std::wstring widestring(std::string s) {
	std::wstring ws(s.size(), L' '); // Overestimate number of code points.
	ws.resize(std::mbstowcs(&ws[0], s.c_str(), s.size())); // Shrink to fit.
	return ws;
}


Request::Request(const char *url) {

	hSession = WinHttpOpen(L"Octissimo/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
								  WINHTTP_NO_PROXY_BYPASS, 0);
	// Specify an HTTP server.
	if (hSession) {
		hConnect = WinHttpConnect(hSession, widestring(url).c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

}

void Request::update(const char *endpoint, std::function<void(const std::string &)> callback, string headers) {
	std::string response;

	BOOL bResults = FALSE;
	WinHttpHandle hRequest;
	if (hConnect) {
		const wchar_t *typeJson = L"application/json";
		const wchar_t *acceptTypes[] = {typeJson, nullptr};
		hRequest = WinHttpOpenRequest(hConnect, L"GET", widestring(endpoint).c_str(), NULL, WINHTTP_NO_REFERER, acceptTypes, WINHTTP_FLAG_SECURE);
	}
	if (hRequest) {
		bResults = WinHttpSendRequest(hRequest, headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : widestring(headers).c_str(), 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
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

	callback(response);

}
