//
// Created by Ondra on 9.4.2020.
//

#include "Request.h"
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

std::wstring widestring(std::string s) {
	std::wstring ws(s.size(), L' '); // Overestimate number of code points.
	ws.resize(std::mbstowcs(&ws[0], s.c_str(), s.size())); // Shrink to fit.
	return ws;
}

std::string stringFromWide(std::wstring s) {
	std::string rs(s.size() * 4, ' '); // Overestimate number of code points.
	rs.resize(std::wcstombs(&rs[0], s.c_str(), s.size())); // Shrink to fit.
	return rs;
}


Request::Request(const char *url) {

	hSession = WinHttpOpen(L"Octissimo/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
								  WINHTTP_NO_PROXY_BYPASS, 0);
	// Specify an HTTP server.
	if (hSession) {
		hConnect = WinHttpConnect(hSession, widestring(url).c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
	}

}

std::vector<string> splitString(string s, char delim) {
	std::stringstream ss(s);
	std::vector<string> ret;
	string to;
	while (std::getline(ss, to, delim)) {
		if (!to.empty() && to[to.size() - 1] == '\r') to.erase(to.size() - 1);
		ret.push_back(to);
	}
	return ret;
}

std::string ltrim(std::string str, const std::string& chars = "\t\n\v\f\r ") {
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

std::string rtrim(std::string str, const std::string& chars = "\t\n\v\f\r ") {
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

std::string trim(std::string str, const std::string& chars = "\t\n\v\f\r ") {
	return ltrim(rtrim(str, chars), chars);
}

std::string tolower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
	return str;
}

void Request::update(
		  const char *endpoint,
		  std::function<void(const std::string &, const std::map<string, string> &)> callback,
		  string headers
		  ) {
	string response;
	std::map<string, string> responseHeaders;

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

	if (bResults) {
		DWORD dwSize;
		WinHttpQueryHeaders( hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
									WINHTTP_HEADER_NAME_BY_INDEX, NULL,
									&dwSize, WINHTTP_NO_HEADER_INDEX);

		if( GetLastError( ) == ERROR_INSUFFICIENT_BUFFER ) {
			WCHAR *lpOutBuffer = new WCHAR[dwSize/sizeof(WCHAR)];

			// Now, use WinHttpQueryHeaders to retrieve the header.
			bResults = WinHttpQueryHeaders( hRequest,
													  WINHTTP_QUERY_RAW_HEADERS_CRLF,
													  WINHTTP_HEADER_NAME_BY_INDEX,
													  lpOutBuffer, &dwSize,
													  WINHTTP_NO_HEADER_INDEX);

			string headersString = stringFromWide(lpOutBuffer);

		  	delete[] lpOutBuffer;

			std::stringstream ss(headersString);
			std::string to;

			auto rawLines = splitString(headersString, '\n');
			std::vector<string> lines;
			for (auto l: rawLines) {
				auto pos = l.find(':');
				if (pos != string::npos) {
					auto name = l.substr(0, pos);
					auto value = l.substr(pos + 1);
					responseHeaders.insert({tolower(trim(name)), trim(value)});
				}
			}
		}
	}


	callback(response, responseHeaders);

}
