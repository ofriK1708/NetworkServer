#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
#define GOOD "200 OK"
#define EMPTY_BODY "204 No Content"
#define CREATED "201 Created"
#define NOT_FOUND "404 Not Found"
#define SERVER_ERROR "500 Internal Server Error"
#define HTTP_TYPE "text/html"
using std::string;

void createResponse(const string& status, const string& content_type,char * sendResponse, const string& body = "", bool isOptions = false);
void GET_request(string& path, char* response);
void POST_request(char* response, string& body);
string putRequestFileManager(string& path, string& body);
void PUT_request(string& path, string& body, char* response);
