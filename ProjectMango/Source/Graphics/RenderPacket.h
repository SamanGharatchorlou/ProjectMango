#pragma once

class Texture;


struct RenderPacket
{
	Texture* texture;
	RectF rect;
};


struct RenderableObject
{
	virtual RenderPacket buildRenderPacket() const = 0;
};