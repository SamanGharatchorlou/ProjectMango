#pragma once

#include "rapidjson/document.h"

struct JSONParser
{
	JSONParser() { }
	JSONParser(const char* filePath);
	~JSONParser() { }

	bool Parse(const char* file_path);
	void Print();
	void DoTest();

	bool IsValid() const;

	rapidjson::Document document;
};

namespace rapidjson
{
	enum kJsonType { Null, False, True, Object, Array, String, Number };
	static const char* kTypeNames[] { "Null", "False", "True", "Object", "Array", "String", "Number" };
};