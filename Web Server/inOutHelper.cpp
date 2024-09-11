#include "inOutHelper.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>



void createResponse(const string& status, const string& content_type, char* sendResponse, int method, size_t content_size, const string& body)
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
		createResponse(NOT_FOUND, HTTP_TYPE, response,GET);
		return;
	}
	if (file.bad())
	{
		createResponse(SERVER_ERROR, HTTP_TYPE, response,GET);
		return;
	}
	file.seekg(0, file.end);
	size_t file_size = file.tellg();
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
	size_t i = path.find('?');
	if (i != string::npos)
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
		return true;
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

// Creating the response that will insert it into 'response' and it will be printed in send massage func
void POST_request(string& body, char* response) {
	string status;
	if (body.empty())
	{
		status = EMPTY_BODY;
	}
	else {
		status = GOOD;
	}
	createResponse(status, HTTP_TYPE, response,POST,body.size(),body);
}

void PUT_request(string& path,string& body, char* response) {
	string status = putRequestFileManager(path, body);
	createResponse(status, HTTP_TYPE, response, POST, body.size(), body);
}

//need to understand if the file name is only in the path or if it can be in the body of put request or both
//WE CAN DECIDE THAT FILE NAME WILL BE SENT ONLY IN THE PATH HEADER
//if file is already existed need to change last-modified header, need to think if we ahould create another response func
string putRequestFileManager(string& path, string& body) {
	string fileName;
	// find last slach in the path
	auto lastSlash = path.find_last_of('/');
	// check that there was last slash
	if (lastSlash != string::npos) {
		// get the file name after the last slash
		fileName = path.substr(lastSlash + 1);
	}
	else {
		// no last slash, get the whole path as the file name
		fileName = path;
	}

	std::ifstream file(fileName);

	// file does not exist
	if (!file) {
		file.close();
		std::ofstream newFile(fileName);
		// if file was created get the status and insert the data into the file
		if (newFile.is_open()) {
			newFile << body;
			newFile.close();
			return CREATED;
		}
	}
	
	// file exists
	else {
		file.close();
		if (body.empty()) {
			// need to check if no content should do nothing or erase the content
			return EMPTY_BODY;
		}
		else {
			// if body is not enpty override the data of the existing file
			std::ofstream existingFile(fileName);
			if (existingFile.is_open()) {
				existingFile << body;
				existingFile.close();
				return GOOD;
			}
		}
	}
	return SERVER_ERROR;
}
void DELETE_request(string& path, char* response) 
{
	if (remove(path.c_str()) == 0) 
	{
		createResponse(GOOD, HTTP_TYPE, response, DELETE_);
	}
	else 
	{
		createResponse(NOT_FOUND, HTTP_TYPE, response, DELETE_);
	}
}