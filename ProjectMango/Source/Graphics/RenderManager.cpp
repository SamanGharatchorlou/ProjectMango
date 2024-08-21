#include "pch.h"
#include "Graphics/RenderManager.h"

#include "Texture.h"
#include "Renderable.h"
#include "Renderer.h"

#include "Debugging/ImGui/ImGuiMenu.h"


RenderManager::RenderManager() { }

RenderManager* RenderManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.configs != nullptr, "Render manager has no been set up yet");
	return gd.renderManager;
}

void RenderManager::addRenderable(Renderable* renderable)
{
	mRenderables.push_back(renderable);
}

void RenderManager::removeRenderable(Renderable* renderable)
{
	for (auto iter = mRenderables.begin(); iter != mRenderables.end(); iter++)
	{
		if (*iter == renderable)
		{
			mRenderables.erase(iter);
			return;
		}
	}
}

void RenderManager::AddDebugRenderPacker(const DebugRenderPack& renderPack)
{
	mDebugRenders.push_back(renderPack);
}


void RenderManager::AddRenderPacket(RenderPack renderPacket) 
{ 

	ASSERT(renderPacket.texture != nullptr, "RenderPacket has no texture");
	mRenderPackets[renderPacket.layer].push_back(renderPacket); 
}

void RenderManager::render()
{
	SDL_Renderer* renderer = Renderer::Get()->sdlRenderer();

	// SDL_GetRendererInfo

	//SDL_RenderSetLogicalSize(renderer, 512, 512);
	//Renderer::Get()->setScale(2);
	//VectorF scale = 

	//SDL_RenderSetScale(renderer, 0.5, 0.5);

	// clear screen
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	// todo: remove
	for (uint32_t i = 0; i < mRenderables.size(); i++)
	{
		mRenderables[i]->render();
	}

	// render all the packs we received in the layer order
	for (u32 i = 0; i < c_RenderLayers; i++)
	{
		std::vector<RenderPack>& render_packs = mRenderPackets[i];
		for (u32 i = 0; i < render_packs.size(); i++)
		{
			if (render_packs[i].subRect.isValid())
			{
				if(render_packs[i].flip == SDL_FLIP_HORIZONTAL)
				{
					// same distance but flipped over to the other side, so x2 the diff between rect center and the flip point
					VectorF diff = render_packs[i].rect.Size() - (render_packs[i].flipPoint * 2.0f);
					render_packs[i].rect.Translate(diff * -1);
				}

				render_packs[i].texture->renderSubTexture(render_packs[i].rect, render_packs[i].subRect, render_packs[i].rotation, render_packs[i].flipPoint, render_packs[i].flip);
			}
			else
				render_packs[i].texture->render(render_packs[i].rect, render_packs[i].flip);
		}

		render_packs.clear();
	}

	// always debug draw last
	for (u32 i = 0; i < mDebugRenders.size(); i++)
	{
		switch (mDebugRenders[i].type)
		{
		case DebugDrawType::Line:
		{
			Colour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

			const RectF& rect = mDebugRenders[i].rect;
			const Vector2D<int> A = rect.TopLeft().toInt();
			const Vector2D<int> B = rect.Size().toInt();

			SDL_RenderDrawLine(renderer, A.x, A.y, B.x, B.y);
			break;
		}
		case DebugDrawType::RectOutline:
		{
			Colour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

			RectF& rect = mDebugRenders[i].rect;
			SDL_Rect renderQuadb = { static_cast<int>(rect.x1),
				static_cast<int>(rect.y1),
				static_cast<int>(rect.Width()),
				static_cast<int>(rect.Height()) };

			SDL_RenderDrawRect(renderer, &renderQuadb);
			break;
		}
		case DebugDrawType::Point:
		case DebugDrawType::RectFill:
		{
			Colour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
			
			RectF& rect = mDebugRenders[i].rect;
			SDL_Rect renderQuadb = { static_cast<int>(rect.x1),
							static_cast<int>(rect.y1),
							static_cast<int>(rect.Width()),
							static_cast<int>(rect.Height()) };

			SDL_RenderFillRect(renderer, &renderQuadb);
			break;
		}
		default:
			break;
		}
	}

	mDebugRenders.clear();

#if IMGUI
	DebugMenu::Draw();
#endif

	// update window surface
	SDL_RenderPresent(renderer);
}
