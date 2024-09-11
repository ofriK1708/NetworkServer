#include "inOutHelper.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>

#define HEAD 0
#define GET 1
#define OPTIONS 2
#define POST 3
#define PUT 4
#define DELETE 5
#define TRACE 6

void createResponse(const string& status, const string& content_type, char* sendResponse, int method, int content_size, const string& body)
{
	time_t timer;
	time(&timer);
	string response = "HTTP/1.1 " + status + "\r\n";
	response += "Date: " + string(ctime(&timer)) + "\r\n";
	if (method == GET || method == HEAD)
	{
		response += "Content-Type: " + content_type + "\r\n";
		response += "Server: WebServer/1.0\r\n";
		response += "Content-Length: " + std::to_string(content_size) + "\r\n";
	}
	else if (method == OPTIONS)
	{
		response += "Allow: GET, HEAD, OPTIONS, PUT, DELETE, TRACE\r\n";
		response += "Server: WebServer/1.0\r\n";
	}
	else
	{
		response += "Server: WebServer/1.0\r\n";
	}
	response += "\r\n";
	response += body;
	sendResponse = (char*)response.c_str();
}

void GET_HEAD_request(string& path, char* response, int method,string& acceptLangugeHeader)
{
	string full_path = "./HTML_FILES"; // Base directory for the files
	if(!checkLangQuery(path, acceptLangugeHeader))
	{
		createResponse("406 Not Acceptable", "text/html", response, GET);
		return;
	}
	full_path += path;
	std::ifstream file(full_path,std::ios::in);
	// Read the file content
	if (!file.is_open())
	{
		createResponse("404 Not Found", "text/html", response, GET); // here it doesnt matter if its head or get because file is not found or bad 
		return;
	}
	if (file.bad())
	{
		createResponse("500 Internal Server Error", "text/html", response,GET);
		return;
	}
	file.seekg(0, file.end);
	long int file_size = file.tellg();
	file.seekg(0, file.beg);
	if (method == GET)
	{
		string body;
		body.resize(file_size);
		file.read(&body[0], file_size);
		createResponse("200 OK", "text/html", response, GET, file_size, body);
	}
	else
	{
		createResponse("200 OK", "text/html", response, HEAD, file_size);
	}
	file.close();
}

bool checkLangQuery(string& path,string& acceptLangugeHeader)
{
	string language;
	bool foundAvailableLang = false;
	// first we check if there is a language query in the path
	size_t index = path.find('?');
	if (index != string::npos)
	{
		path = path.substr(0, i);
		language = path.substr(i + 6); // 6 is the length of the query string "lang="
		foundAvailableLang = true;
	}
	else // we check if the accept-language header is present or find the first available language and return false there isnt one 
	{
		foundAvailableLang = findAvailableLang(language, acceptLangugeHeader);
	}
	if (foundAvailableLang)
	{
		parseHeaderPath(path, language, acceptLangugeHeader);
	}
	return foundAvailableLang;
}
void parseHeaderPath(string& path, string& language, string& acceptLangugeHeader)
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '.')
		{
			path = path.substr(0, i + 1) + language + path.substr(i); // "0 - i+1 is the name of file and dot, then add lang then i - n is . and file ending  
			break;
		}
	}
}
bool findAvailableLang(string& language, string& acceptLangugeHeader)
{

	if (acceptLangugeHeader.empty())
	{
		language = "en";
		return;
	}
	if (isLanguageAccepted(acceptLangugeHeader, "en")) 
	{
		language = "en"; // Prefer English if present
	}
	else if (isLanguageAccepted(acceptLangugeHeader, "he"))
	{
		language = "he";  // Then prefer Hebrew if present
	}
	else if (isLanguageAccepted(acceptLangugeHeader, "fr")) 
	{
		language = "fr";  // Then prefer French if present
	}
	return language.empty(); // If no language was found, return false
}

bool isLanguageAccepted(const std::string& header, const std::string& lang) {
	// Convert header to lowercase for case-insensitive comparison
	std::string lowerHeader = header;
	std::transform(lowerHeader.begin(), lowerHeader.end(), lowerHeader.begin(), ::tolower);

	return lowerHeader.find(lang) != std::string::npos;
}