#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
using std::string;

void createResponse(const string& status, const string& content_type, char* sendResponse, const string& body = "", int content_size = 0, bool isHead = false, bool isOptions = false);
void GET_HEAD_request(string& path, char* response, bool isHead = false);