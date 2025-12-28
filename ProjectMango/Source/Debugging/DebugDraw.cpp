#pragma once
#include "pch.h"
#include "DebugDraw.h"

#include "Game/Camera/Camera.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderManager.h"

namespace DebugDraw 
{
	void Shape(u32 renderType, RectF rect, SColour colour) 
	{
		DebugRender::DrawType type = (DebugRender::DrawType)renderType
;		switch (type) 
			{
				case DebugRender::DrawType::Point:
				{
					RectFill(rect, colour);
					break;
				}
				case DebugRender::DrawType::Line: 
				{
					RectFill(rect, colour);
					break;
				}
				case DebugRender::DrawType::RectOutline: 
				{
					RectOutline(rect, colour);
					break;
				}
				case DebugRender::DrawType::RectFill: 
				{
					RectFill(rect, colour);
					break;
				}
				default:
					break;
			}
	}

	void Point(VectorF point, SColour colour, float size)
	{
		RectF rect;
		rect.SetSize(size, size);
		rect.SetCenter(point);

		RectFill(rect, colour);
	}

	void Line(VectorF pointA, VectorF pointB, SColour colour)
	{
		DebugRender::RenderPack pack;
		pack.colour = colour;
		pack.type = DebugRender::DrawType::Line;
		pack.rect = RectF(pointA, pointB);
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}


	void RectOutline(const RectF& rect, SColour colour)
	{
		DebugRender::RenderPack pack;
		pack.colour = colour;
		pack.type = DebugRender::DrawType::RectOutline;
		pack.rect = rect;
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}


	void RectFill(const RectF& rect, SColour colour)
	{
		DebugRender::RenderPack pack;
		pack.colour = colour;
		pack.type = DebugRender::DrawType::RectFill;
		pack.rect = rect;
		RenderManager::Get()->AddDebugRenderPacker(pack);
	}

	void Quad(Quad2D<float> quad, SColour colour)
	{
		for (unsigned int i = 0; i < quad.sides(); i++)
		{
			int j = i + 1 >= quad.sides() ? 0 : i + 1;
	
			VectorF pointA = quad[i];
			VectorF pointB = quad[j];
			Line(pointA, pointB, colour);
		}
	}
}