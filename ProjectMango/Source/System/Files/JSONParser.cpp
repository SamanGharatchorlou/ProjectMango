#include "pch.h"
#include "JSONParser.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>

using namespace rapidjson;

JSONParser::JSONParser(const char* filePath)
{
	Parse(filePath);
}

bool JSONParser::Parse(const char* full_path)
{
	if(FILE* fp = fopen(full_path, "rb"))
	{
		ASSERT(fp, "failed to read file %s", full_path);

		const u32 size = (u32)std::filesystem::file_size( fs::path(full_path) );

		// cant be sure how big this might be so new a buffer so its on the heap
		char* buffer = new char[size];
		FileReadStream is(fp, buffer, size);

		document.ParseStream(is);

		fclose(fp);

		delete[] buffer;
		buffer = nullptr;
	}
		
	if(!document.IsObject())
	{
		DebugPrint(PriorityLevel::Warning, "Invalid json document: %s", full_path);
		return false;
	}

	return true;
}

bool JSONParser::IsValid() const
{
	return document.IsObject();
}

void JSONParser::Print()
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
 
	const char* output = buffer.GetString();

	printf(output);
}

bool PopulateSettingByType(const rapidjson::Value& value, const char* id, kJsonType type, Settings& settings)
{
	switch( type )
	{	
		case kJsonType::Number:
		{
			settings.values[id] = value.GetFloat();
			return true;
		}
		case kJsonType::True:
		{
			settings.values[id] = true;
			return true;
		}
		case kJsonType::False:
		{
			settings.values[id] = false;
			return true;
		}
		case kJsonType::String:
		{
			settings.strings[id] = value.GetString();
			return true;
		} 
		case kJsonType::Array:
		{			
			const Value::ConstArray& array = value.GetArray();
			if(array.Size() > 0 && array.begin()->GetType() == kJsonType::Number)
			{
				if(array.Size() <= 2)
				{
					VectorF vector;
					if( array.Size() > 0 )
						vector.x = array[0].GetFloat();
					if( array.Size() > 1 )
						vector.y = array[1].GetFloat();

					settings.vectors[id] = vector;
				}
				else
				{
					std::vector<float>& float_array = settings.floatArrays.data[id];
					for( u32 i = 0; i < array.Size(); i++ )
					{
						float_array.push_back(array[i].GetFloat());
					}
				}
			}
			return true;
		}
		default:
			return false;
	}
}