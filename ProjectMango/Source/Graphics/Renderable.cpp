#include "pch.h"

#include "Renderable.h"
#include "Game/Data/GameData.h"
#include "Graphics/RenderManager.h"


Renderable::Renderable()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.renderManager, "Render manager has no been set up yet, cannot link Renderable object to it. will not render");

	gd.renderManager->addRenderable(this);

	DebugPrint(Log, "Adding to renderables");
}

Renderable::~Renderable()
{
	GameData& gd = GameData::Get();
	gd.renderManager->removeRenderable(this);
}