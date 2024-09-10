#include "inOutHelper.h"
#include <fstream> 
using std::ifstream;
void createResponse(const string& status, const string& content_type,char * sendResponse, const string& body="", bool isOptions = false)
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
		createResponse(NOT_FOUND, HTTP_TYPE, response);
		return;
	}
	if (file.bad())
	{
		createResponse(SERVER_ERROR, HTTP_TYPE, response);
		return;
	}
	file.seekg(0, file.end);
	long int file_size = file.tellg();
	file.seekg(0, file.beg);
	string body;
	body.resize(file_size);
	file.read(&body[0], file_size);
	file.close();
	createResponse(GOOD, HTTP_TYPE, response, body);
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
	createResponse(status, HTTP_TYPE, response, body);
}

void PUT_request(string& path,string& body, char* response) {
	string status = putRequestFileManager(path, body);
	createResponse(status, HTTP_TYPE, response, body);
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