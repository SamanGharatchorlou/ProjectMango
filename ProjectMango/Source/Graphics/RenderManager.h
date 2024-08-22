#pragma once

class Texture;

constexpr u32 c_RenderLayers = 10;

struct RenderPack
{
	RenderPack(const Texture* tex, RectF box, u32 renderLayer) : texture(tex), rect(box), layer(renderLayer) 
	{
		// override if required
		flipPoint = box.Size() * 0.5f;
	}

	const Texture* texture = nullptr;
	RectF rect;
	RectF subRect = InvalidRectF;
	VectorF flipPoint;
	double rotation = 0.0;
	u32 layer = c_RenderLayers;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
};

enum DebugDrawType
{
	Point,
	Line,
	RectOutline,
	RectFill,
	Quad,
	Count
};

struct DebugRenderPack
{
	RectF rect;
	Colour colour;
    DebugDrawType type;
};

class RenderManager
{
public:
	RenderManager();

	static RenderManager* Get();

	void render();

	void AddRenderPacket(RenderPack renderPacket);
	void AddDebugRenderPacker(const DebugRenderPack& renderPack);

private:
	// renderlayers + the lowest
	std::vector<RenderPack> mRenderPackets[c_RenderLayers];
	std::vector<DebugRenderPack> mDebugRenders;
};
