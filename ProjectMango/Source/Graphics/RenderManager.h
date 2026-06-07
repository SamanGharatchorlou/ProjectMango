#pragma once

struct STexture;
struct Font;

// todo - make this adjust to render layers better?
constexpr u32 c_RenderLayers = 13 + 1;

struct RenderPack
{
	RenderPack() { }
	RenderPack(STexture* tex, u32 renderLayer) : texture(tex), layer(renderLayer) {}

	ECS::Entity entity = ECS::EntityInvalid;

	STexture* texture = nullptr;
	const Font* font = nullptr;

	RectF rect;
	RectF subRect = InvalidRectF;
	RectF clippingRect = InvalidRectF;
	VectorF flipPoint;
	double rotation = 0.0;
	u32 layer = -1;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SColour colourMod;
};

namespace DebugRender
{
	enum DrawType
	{
		Point,
		Line,
		RectOutline,
		RectFill,
		Quad,
		Count
	};

	struct RenderPack
	{
		RectF rect;
		SColour colour;
		DrawType type = DrawType::Point;
	};
}

class RenderManager
{
public:
	RenderManager();

	static RenderManager* Get();

	void render();

	void AddRenderPacket(RenderPack renderPacket);
	void AddDebugRenderPacker(const DebugRender::RenderPack& renderPack);

private:
	// renderlayers + the lowest
	std::vector<RenderPack> mRenderPackets[c_RenderLayers];
	std::vector<DebugRender::RenderPack> mDebugRenders;
};
