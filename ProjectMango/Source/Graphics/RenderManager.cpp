#include "pch.h"
#include "Graphics/RenderManager.h"

#include "Texture.h"
#include "Renderer.h"
#include "System/Window.h"
#include "Game/Camera/Camera.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

// WARNING: dont remove this, it doesnt complain except remove all the imgi stuff.. weird
#include "Debugging/ImGui/ImGuiMenu.h"


RenderManager::RenderManager() { }

RenderManager* RenderManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.configs != nullptr, "Render manager has no been set up yet");
	return gd.renderManager;
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
	DebugMenu::SendRenderLayerInfo(mRenderPackets);

	SDL_Renderer* renderer = Renderer::Get()->sdlRenderer();

	// handle window scaling
	int width = -1;
	int height = -1;
	SDL_GetWindowSize(GameData::Get().window->get(), &width, &height);
	const VectorF real_window_size = VectorF((float)width, (float)height);
	const VectorF fake_window_size = GameData::Get().window->size();
	const float render_scale = real_window_size.x / fake_window_size.x;
	Renderer::Get()->setScale(render_scale);


	VectorF camera_shift = Camera::Get()->GetRect().TopLeft() * -1.0f;

	// clear screen
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

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

				render_packs[i].rect.Translate(camera_shift);
				render_packs[i].texture->renderSubTexture(render_packs[i].rect, render_packs[i].subRect, render_packs[i].rotation, render_packs[i].flipPoint, render_packs[i].flip);
			}
			else
			{
				render_packs[i].rect.Translate(camera_shift);
				render_packs[i].texture->render(render_packs[i].rect, render_packs[i].flip);
			}
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
			const Vector2D<int> A = (rect.TopLeft() + camera_shift).toInt();
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
			SDL_Rect renderQuadb = { static_cast<int>(rect.x1 + camera_shift.x),
				static_cast<int>(rect.y1 + camera_shift.y),
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
			SDL_Rect renderQuadb = {	static_cast<int>(rect.x1 + camera_shift.x),
										static_cast<int>(rect.y1 + camera_shift.y),
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
	// a hack due to how im handling scaling, im pretending the game is build for a 1024x1024 window size.
	// all sizes and positions are relative to this and then i'm scaling to the real window size. but imgui is getting
	// the real window size and THEN scaling so its all messed up, i dont know how to get it to think its drawing on a 
	// 1024x1024 window size so just pretend its not scaling, then it at least draws to a 1024 screen and its useable
	Renderer::Get()->setScale(1);
	DebugMenu::Draw();
	Renderer::Get()->setScale(render_scale);
#endif

	// update window surface
	SDL_RenderPresent(renderer);
}
