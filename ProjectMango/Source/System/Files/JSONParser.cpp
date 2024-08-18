#include "pch.h"
#include "JSONParser.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>

using namespace rapidjson;

JSONParser::JSONParser(const char* filePath)
{
	ASSERT(fs::exists(filePath), "File path %s does not exist, cannot parse xml file", filePath);

	if(FILE* fp = fopen(filePath, "rb"))
	{
		ASSERT(fp, "failed to read file %s", filePath);

		const u32 size = (u32)std::filesystem::file_size( fs::path(filePath) );

		// cant be sure how big this might be so new a buffer so its on the heap
		char* buffer = new char[size];
		FileReadStream is(fp, buffer, size);

		document.ParseStream(is);

		fclose(fp);

		delete[] buffer;
		buffer = nullptr;
	}
}

void JSONParser::Print()
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
 
	const char* output = buffer.GetString();

	printf(output);
}


void JSONParser::DoTest()
{
	Value::MemberIterator levels = document.FindMember("levels");

	bool has_mem = levels != document.MemberEnd();
	bool arr = levels->value.IsArray();

	Value& level_0 = levels->value[0];


	Value::MemberIterator yep = level_0.FindMember("layerInstances");
	bool has_memb = yep != document.MemberEnd();

	bool is_layers = yep->value.IsArray();

    for (SizeType i = 0; i < yep->value.Size(); i++)
	{
		Value& layer = yep->value[i];

		const char* id = layer["__identifier"].GetString();
		const int def_id = layer["layerDefUid"].GetInt();

		Value& grid = layer["gridTiles"];
		bool has_grid = layer.HasMember("gridTiles");

		if( grid.IsArray() )
		{
			const Value::Array& grid_array = grid.GetArray();

			for( u32 i = 0; i < grid.Size(); i++ )
			{
				Value& entry = grid[i];

				bool has_member_px = entry.HasMember("px");
				Value& px = entry["px"];
				
				bool is_arry = px.IsArray();


				int x = px[0].GetInt();
				int y = px[1].GetInt();

				int b = 10;
			}
		}

		int c = 4;
	}

	int a = 4;
}



