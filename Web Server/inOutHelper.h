#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
using std::string;

string createResponse(const string& status, const string& content_type, const string& body = "", bool isOptions = false);
