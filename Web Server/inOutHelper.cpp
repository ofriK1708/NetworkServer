#include "inOutHelper.h"
#include <fstream>
using std::ifstream;
void createResponse(const string& status, const string& content_type,char * sendResponse, const string& body, bool isOptions)
{
	time_t timer;
	time(&timer);
	string response = "HTTP/1.1 " + status + "\r\n";
	response += "Date: " + string(ctime(&timer)) + "\r\n";
	if (!body.empty())
	{
		response += "Content-Type: " + content_type + "\r\n";
		response += "Server: WebServer/1.0\r\n";
		response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
	}
	else if (isOptions)
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
// TODO - FIX IT 

void GET_request(string& path, char* response)
{
	string full_path = "./HTML_FILES"; // Base directory for the files
	full_path += path;
    
	ifstream file(full_path,std::ios::in);
	// Read the file content
	if (!file.is_open())
	{
		createResponse("404 Not Found", "text/html", response);
		return;
	}
	if (file.bad())
	{
		createResponse("500 Internal Server Error", "text/html", response);
		return;
	}
	file.seekg(0, file.end);
	long int file_size = file.tellg();
	file.seekg(0, file.beg);
	string body;
	body.resize(file_size);
	file.read(&body[0], file_size);
	file.close();
	createResponse("200 OK", "text/html", response, body);
}
