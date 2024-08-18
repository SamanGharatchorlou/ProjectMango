#pragma once

#include "rapidjson/document.h"

struct JSONParser
{
	JSONParser(const char* filePath);
	~JSONParser() { }

	void Print();
	void DoTest();

	rapidjson::Document document;
};