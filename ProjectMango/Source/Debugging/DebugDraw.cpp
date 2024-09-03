#pragma once
#include "pch.h"
#include "DebugDraw.h"

#include "Game/Camera/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderManager.h"

namespace DebugDraw 
{
	void Shape(DebugDrawType renderType, RectF rect, Colour colour) 
	{
	  switch (renderType) 
	  {
          case DebugDrawType::Point:
		  {
				RectFill(rect, colour);
				break;
		  }
          case DebugDrawType::Line: 
		  {
				RectFill(rect, colour);
				break;
          }
          case DebugDrawType::RectOutline: 
		  {
				RectOutline(rect, colour);
				break;
          }
          case DebugDrawType::RectFill: 
		  {
				RectFill(rect, colour);
				break;
          }

		default:
			break;
	  }
	}

	void Point(VectorF point, Colour colour, float size)
	{
		RectF rect;
		rect.SetSize(size, size);
		rect.SetCenter(point);

		RectFill(rect, colour);
	}

	void Line(VectorF pointA, VectorF pointB, Colour colour)
	{
		DebugRenderPack pack;
		pack.colour = colour;
		pack.type = DebugDrawType::Line;
		pack.rect = RectF(pointA, pointB);
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}


	void RectOutline(const RectF& rect, Colour colour)
	{
		DebugRenderPack pack;
		pack.colour = colour;
		pack.type = DebugDrawType::RectOutline;
		pack.rect = rect;
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}


	void RectFill(const RectF& rect, Colour colour)
	{
		DebugRenderPack pack;
		pack.colour = colour;
		pack.type = DebugDrawType::RectFill;
		pack.rect = rect;
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}


	//void debugDrawRects(const std::vector<RectF>& rects, Colour colour)
	//{
	//	SDL_SetRenderDrawColor(Renderer::Get()->sdlRenderer(), colour.r, colour.g, colour.b, colour.a);
	//	
	//	for (unsigned int i = 0; i < rects.size(); i++)
	//	{
	//		RectF rect = Camera::Get()->toCameraCoords(rects[i]);
	//		SDL_Rect renderQuadb = { static_cast<int>(rect.x1),
	//						static_cast<int>(rect.y1),
	//						static_cast<int>(rect.Width()),
	//						static_cast<int>(rect.Height()) };
	//
	//		SDL_RenderFillRect(Renderer::Get()->sdlRenderer(), &renderQuadb);
	//	}
	//}
	//
	//
	//void debugDrawQuad(Quad2D<float> quad, Colour colour)
	//{
	//	SDL_SetRenderDrawColor(Renderer::Get()->sdlRenderer(), colour.r, colour.g, colour.b, colour.a);
	//
	//	for (unsigned int i = 0; i < quad.sides(); i++)
	//	{
	//		quad[i] = Camera::Get()->toCameraCoords(quad[i]);
	//	}
	//
	//	for (unsigned int i = 0; i < quad.sides(); i++)
	//	{
	//		int j = i + 1 >= quad.sides() ? 0 : i + 1;
	//
	//		Vector2D<int> pointA = quad.at(i).toInt();
	//		Vector2D<int> pointB = quad.at(j).toInt();
	//		SDL_RenderDrawLine(Renderer::Get()->sdlRenderer(), pointA.x, pointA.y, pointB.x, pointB.y);
	//	}
	//}

//	void Text(const BasicString text, int ptSize, VectorF position, Colour colour, const char* alignment)
//	{
//		UITextBox textBox;
//		textBox.setAlignment(alignment);
//		textBox.mText.init(text, "default", ptSize, colour.toSDL());
//		textBox.setRect(RectF(position + VectorF(5,5), VectorF::zero()));
//		textBox.render();
//	}
}