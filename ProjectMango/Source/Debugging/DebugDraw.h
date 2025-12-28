#pragma once

namespace DebugDraw 
{
	void Shape(u32 renderType, RectF rect, SColour colour);

	void Point(VectorF point, SColour colour, float size = 8.0f);
	void Line(VectorF pointA, VectorF pointB, SColour colour);
	void RectOutline(const RectF& rect, SColour colour);
	void RectFill(const RectF& rect, SColour colour);
	void Quad(Quad2D<float> quad, SColour colour);
}