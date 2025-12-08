#pragma once

class STexture;
struct Font;

constexpr u32 c_RenderLayers = 10;

struct RenderPack
{
	RenderPack() { }
	RenderPack(STexture* tex, RectF box, u32 renderLayer) : texture(tex), rect(box), layer(renderLayer) 
	{
		// override if required
		flipPoint = box.Size() * 0.5f;
	}
	//RenderPack(Font* _font, RectF box, u32 renderLayer) : font(_font), rect(box), layer(renderLayer) 
	//{
	//	// override if required
	//	flipPoint = box.Size() * 0.5f;
	//}

	ECS::Entity entity;

	STexture* texture = nullptr;
	const Font* font = nullptr;

	RectF rect;
	RectF subRect = InvalidRectF;
	VectorF flipPoint;
	double rotation = 0.0;
	u32 layer = c_RenderLayers;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SColour colourMod;
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
	SColour colour;
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
