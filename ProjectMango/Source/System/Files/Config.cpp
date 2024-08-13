#include "pch.h"
#include "Config.h"

void GameSettingsConfig::Read(XMLParser& parser)
{
	settings.fillValues(parser.rootNode());
}

void PopupConfig::Read(XMLParser& parser)
{
	const XMLNode node = parser.rootNode();
	title = node.child("Title").value();
	body = node.child("Body").value();
}