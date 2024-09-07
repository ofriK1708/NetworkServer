#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
using std::string;

void createResponse(const string& status, const string& content_type,char * sendResponse, const string& body = "", bool isOptions = false);
