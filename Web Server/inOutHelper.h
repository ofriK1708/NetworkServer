#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <ctime>
#define GOOD "200 OK"
#define EMPTY_BODY "204 No Content"
#define CREATED "201 Created"
#define BAD_REQUEST "400 Bad Request"
#define NOT_FOUND "404 Not Found"
#define NOT_ALLWOED "405 Not Allowed"
#define SERVER_ERROR "500 Internal Server Error"
#define TEXT_HTML_TYPE "text/html"
#define MESSAGE_HTTP_TYPE "message/http"
#define CheckAllocation(c) if(c==NULL){ \
printf("Memory Allocation Failed!!!");   \
exit(1);                                \
}
constexpr int HEAD = 0;
constexpr int GET = 1;
constexpr int OPTIONS = 2;
constexpr int POST = 3;
constexpr int PUT = 4;
constexpr int DELETE_ = 5;
constexpr int TRACE = 6;
using std::string;
const string full_path = "C:/temp"; // Base directory for the files

void createResponse(const string& status, const string& content_type, char** sendResponse,int method = -1, size_t content_size = 0, const string& body = "");
void GET_HEAD_request(string& path, char** response, int method, string& acceptLangugeHeader);
bool checkLangQuery(string& path, string& acceptLangugeHeader);
void parseHeaderPath(string& path, string& language, string& acceptLangugeHeader); 
bool findAvailableLang(string& language, string& acceptLangugeHeader);
bool isLanguageAccepted(const std::string& header, const std::string& lang);
void POST_request(string& body, char** response);
string putRequestFileManager(string& path, string& body);
void PUT_request(string& path, string& body, char** response);
void DELETE_request(string& path, char** response);
void getFilePath(string & path);