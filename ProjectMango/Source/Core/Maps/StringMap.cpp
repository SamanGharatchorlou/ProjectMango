#include "pch.h"
#include "StringMap.h"
#include "Core/Helpers.h"

void StringMap32::fillValues(const XMLNode& node)
{
	XMLNode childNode = node.child();
	while (childNode)
	{
		mData[childNode.name()] = childNode.value();
		childNode = childNode.next();
	}
}

void StringMap32::fillAtributes(const XMLNode& node)
{
	for (XMLNode::Attribute attr = node.attribute(); attr; attr = attr->next_attribute())
	{
		mData[attr->name()] = attr->value();
	}
}

const char* StringMap32::getString(const char* key) const
{
	return mData.at(key).c_str();
}

bool StringMap32::getBool(const char* key) const
{
	return toBool(mData.at(key).c_str());
}

float StringMap32::getFloat(const char* key) const
{
	return toFloat(mData.at(key).c_str());
}

int StringMap32::getInt(const char* key) const
{
	if (mData.contains(key))
		return toInt(mData.at(key).c_str());
	return 0;
}

VectorF StringMap32::getVectorF(const char* x, const char* y) const
{
	if( mData.contains(x) && mData.contains(x))
		return toVectorF(mData.at(x).c_str(), mData.at(y).c_str());
	return VectorF();
}

VectorI StringMap32::getVectorI(const char* x, const char* y) const
{
	if (mData.contains(x) && mData.contains(x))
		return toVectorI(mData.at(x).c_str(), mData.at(y).c_str());
	return VectorI();
}