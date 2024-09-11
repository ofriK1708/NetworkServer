#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
using std::string;

void createResponse(const string& status, const string& content_type, char* sendResponse,int method, int content_size = 0, const string& body = "");
void GET_HEAD_request(string& path, char* response, int method);
void checkLangQuery(string& path, string& acceptLangugeHeader);
void parseHeaderPath(string& path, string& language, string& acceptLangugeHeader); 
bool findAvailabelLang(string& language, string& acceptLangugeHeader);
bool isLanguageAccepted(const std::string& header, const std::string& lang);