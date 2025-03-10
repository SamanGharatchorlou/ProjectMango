#include "pch.h"
#include "Config.h"

#include "System/Files/JSONParser.h"
#include "System/Files/XMLParser.h"

//static void JSONReadData(const rapidjson::Value& doc_values, Settings& settings)
//{
//	for (rapidjson::Value::ConstMemberIterator itr = doc_values.MemberBegin(); itr != doc_values.MemberEnd(); ++itr)
//	{
//		if(itr->value.GetType() == rapidjson::kJsonType::Number)
//		{
//			settings.values[itr->name.GetString()] = itr->value.GetFloat();
//		}
//		else if(itr->value.GetType() == rapidjson::kJsonType::True)
//		{
//			settings.values[itr->name.GetString()] = true;
//		}
//		else if(itr->value.GetType() == rapidjson::kJsonType::False)
//		{
//			settings.values[itr->name.GetString()] = false;
//		}
//		else if (itr->value.GetType() == rapidjson::kJsonType::String)
//		{
//			settings.strings[itr->name.GetString()] = itr->value.GetString();
//		}
//	}
//}
//
//static void XMLReadStrings(const XMLNode& node, Settings& out_string)
//{
//	XMLNode childNode = node.child();
//	while (childNode)
//	{
//		//if(childNode.)
//
//		char* p = nullptr;
//		const char* in_value = childNode.value();
//		long converted = strtol(in_value, &p, 10);
//		if (*p) 
//		{
//			// conversion failed because the input wasn't a number
//			bool is_true = strncmp(in_value, "true", strlen("true") ) == 0;
//			bool is_false = strncmp(in_value, "false", strlen("false") ) == 0;
//			if( is_true || is_false )
//			{
//				out_string.values[childNode.name()] = (int)is_true;
//			}
//			else
//			{
//				out_string.strings[childNode.name()] = childNode.value();
//			}	
//		}
//		else 
//		{
//			// use converted
//			
//			out_string.values[childNode.name()] = converted;
//		}
//
//		//out_string[childNode.name()] = childNode.value();
//		childNode = childNode.next();
//	}
//}

//
//void Config::Read(const char* path)
//{
//	if(type == XML)
//	{
//		XMLParser* parser = new XMLParser;
//		parser->parseXML(path);
//
//		XMLReadStrings(parser->rootNode(), data);
//
//		delete parser;
//	}
//	else
//	{
//		JSONParser parser(path);
//
//		if (!parser.document.IsObject())
//			return;
//
//		JSONReadData(parser.document, data);
//	}
//		
//	parsed = true;
//}
