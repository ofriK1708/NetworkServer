#include "inOutHelper.h"
#include <fstream>
#include <iostream>
#include <ctime>


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

void GET_HEAD_request(string& path, char* response, int method)
{
	string full_path = "./HTML_FILES"; // Base directory for the files
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
