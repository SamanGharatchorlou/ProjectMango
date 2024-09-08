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

namespace rapidjson
{
	enum kJsonType { Null, False, True, Object, Array, String, Number };
	static const char* kTypeNames[] { "Null", "False", "True", "Object", "Array", "String", "Number" };
};