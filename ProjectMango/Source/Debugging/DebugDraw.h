#pragma once

enum DebugDrawType;

namespace DebugDraw 
{
	void Shape(DebugDrawType renderType, RectF rect, SColour colour);

	void Point(VectorF point, SColour colour, float size = 8.0f);
	void Line(VectorF pointA, VectorF pointB, SColour colour);
	void RectOutline(const RectF& rect, SColour colour);
	void RectFill(const RectF& rect, SColour colour);
	//void debugDrawRects(std::vector<RectF> rects, SColour colour);
	void Quad(Quad2D<float> quad, SColour colour);
	//void Text(const BasicString text, int ptSize, VectorF position, SColour colour, const char* alignment = "Center");
}