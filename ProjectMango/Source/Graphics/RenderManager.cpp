#include "pch.h"
#include "Graphics/RenderManager.h"

#include "STexture.h"
#include "UI/Text/Font.h"
#include "Renderer.h"
#include "System/Window.h"
#include "Game/Camera/Camera.h"
#include "Core/Helpers.h"

// WARNING: dont remove this, it doesnt complain except remove all the imgi stuff.. weird
#include "Debugging/ImGui/ImGuiMenu.h"

namespace DebugMenu
{
	void SendRenderLayerInfo(const std::vector<RenderPack>* render_packs);
}

RenderManager::RenderManager() { }

RenderManager* RenderManager::Get()
{
	GameData& gd = GameData::Get();
	ASSERT(gd.configs != nullptr, "Render manager has no been set up yet");
	return gd.renderManager;
}

void RenderManager::AddDebugRenderPacker(const DebugRender::RenderPack& renderPack)
{
	mDebugRenders.push_back(renderPack);
}

void RenderManager::AddRenderPacket(RenderPack renderPacket) 
{
	mRenderPackets[renderPacket.layer].push_back(renderPacket); 
}

void RenderManager::render()
{
	DebugMenu::SendRenderLayerInfo(mRenderPackets);

	Renderer* renderer = Renderer::Get();
	SDL_Renderer* sdl_renderer = renderer->sdlRenderer();

	// handle window scaling
	int width = -1;
	int height = -1;
	SDL_GetWindowSize(GameData::Get().window->get(), &width, &height);
	const VectorF real_window_size = VectorF((float)width, (float)height);
	const VectorF fake_window_size = GameData::Get().window->size();
	const float render_scale = real_window_size.x / fake_window_size.x;
	renderer->setScale(render_scale);

	VectorF camera_shift = Camera::Get()->GetRect().TopLeft() * -1.0f;

	// clear screen
	SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(sdl_renderer);

	// render all the packs we received in the layer order
	for (u32 l = 0; l < c_RenderLayers; l++)
	{
		std::vector<RenderPack>& render_packs = mRenderPackets[l];
		for (u32 i = 0; i < render_packs.size(); i++)
		{
			// debug break point
			if (IsSelectedDebugEntity(render_packs[i].entity))
				int a = 4;
			
			if (render_packs[i].clippingRect.isValid())
			{
				render_packs[i].clippingRect.Translate(camera_shift);
				SDL_Rect clip = render_packs[i].clippingRect.toSDLRect();
				SDL_RenderSetClipRect(sdl_renderer, &clip);
			}

			if(render_packs[i].flip == SDL_FLIP_HORIZONTAL)
			{
				// same distance but flipped over to the other side, so x2 the diff between rect center and the flip point
				VectorF diff = render_packs[i].rect.Size() - (render_packs[i].flipPoint * 2.0f);
				render_packs[i].rect.Translate(diff * -1);
			}

			render_packs[i].rect.Translate(camera_shift);

			if(render_packs[i].texture)
			{
				render_packs[i].texture->colourModThisFrame = render_packs[i].colourMod;

				if (render_packs[i].subRect.isValid())
				{
					render_packs[i].texture->renderSubTexture(sdl_renderer, render_packs[i].rect, render_packs[i].subRect, render_packs[i].rotation, render_packs[i].flipPoint, render_packs[i].flip);
				}
				else
				{
					render_packs[i].texture->render(sdl_renderer, render_packs[i].rect, render_packs[i].flip, render_packs[i].rotation, render_packs[i].flipPoint);
				}
			}

			if(render_packs[i].font)
			{
				render_packs[i].font->Render(renderer, render_packs[i].rect.TopLeft());
			}

			if (render_packs[i].clippingRect.isValid())
				SDL_RenderSetClipRect(sdl_renderer, nullptr);
		}

		render_packs.clear();

	}

	// always debug draw last
	for (u32 i = 0; i < mDebugRenders.size(); i++)
	{
		switch (mDebugRenders[i].type)
		{
		case DebugRender::DrawType::Line:
		{
			SColour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(sdl_renderer, colour.r, colour.g, colour.b, colour.a);

			const RectF& rect = mDebugRenders[i].rect;
			const Vector2D<int> A = (rect.TopLeft() + camera_shift).toInt();
			const Vector2D<int> B = (rect.Size() + camera_shift).toInt();

			SDL_RenderDrawLine(sdl_renderer, A.x, A.y, B.x, B.y);
			break;
		}
		case DebugRender::DrawType::RectOutline:
		{
			SColour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(sdl_renderer, colour.r, colour.g, colour.b, colour.a);
			SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

			RectF& rect = mDebugRenders[i].rect;
			SDL_Rect renderQuadb = { static_cast<int>(rect.x1 + camera_shift.x),
				static_cast<int>(rect.y1 + camera_shift.y),
				static_cast<int>(rect.Width()),
				static_cast<int>(rect.Height()) };

			SDL_RenderDrawRect(sdl_renderer, &renderQuadb);
			break;
		}
		case DebugRender::DrawType::Point:
		case DebugRender::DrawType::RectFill:
		{
			SColour colour = mDebugRenders[i].colour;
			SDL_SetRenderDrawColor(sdl_renderer, colour.r, colour.g, colour.b, colour.a);
			
			RectF& rect = mDebugRenders[i].rect;
			SDL_Rect renderQuadb = {	static_cast<int>(rect.x1 + camera_shift.x),
										static_cast<int>(rect.y1 + camera_shift.y),
										static_cast<int>(rect.Width()),
										static_cast<int>(rect.Height()) };

			SDL_RenderFillRect(sdl_renderer, &renderQuadb);
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
	renderer->setScale(1);
	DebugMenu::Draw();
	renderer->setScale(render_scale);
#endif

	// update window surface
	SDL_RenderPresent(sdl_renderer);
}
