#pragma once

enum DebugDrawType;

namespace DebugDraw 
{
	void Shape(DebugDrawType renderType, RectF rect, Colour colour);

	void Point(VectorF point, float size, Colour colour);
	void Line(VectorF pointA, VectorF pointB, Colour colour);
	void RectOutline(const RectF& rect, Colour colour);
	void RectFill(const RectF& rect, Colour colour);
	//void debugDrawRects(std::vector<RectF> rects, Colour colour);
	//void debugDrawQuad(Quad2D<float> quad, Colour colour);
	//void Text(const BasicString text, int ptSize, VectorF position, Colour colour, const char* alignment = "Center");
}