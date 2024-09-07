#include "inOutHelper.h"

string createResponse(const string& status, const string& content_type, const string& body = "", bool isOptions = false)
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
	return response;
}
// TODO - FIX IT 
void GET_request(const char* path, char* response)
{
	char full_path[BUFFER_SIZE] = "./HTML_FILES"; // Base directory for the files
	strcat(full_path, path); // Construct the full file path

	FILE* file = fopen(full_path, "r");
	if (file == NULL)
	{
		buildResponse("404 Not Found", "text/plain", NULL, response);
		return;
	}

	// Read the file content
	fseek(file, 0, SEEK_END);
	long int file_size = ftell(file);
	char* fileContent = (char*)malloc(sizeof(char) * file_size);
	if (fileContent == NULL)
	{
		perror("Failed to allocate memory for file content");
		response = NULL;
		return;
	}
	fread(fileContent, sizeof(char), BUFFER_SIZE, file);
	fclose(file);

	// Send the response with the file content
	buildResponse("200 OK", "text/html", fileContent, response);
}