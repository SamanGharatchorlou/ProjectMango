#pragma once

class Texture;
struct Renderable;

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

class RenderManager //: public Observer
{
public:
	RenderManager();

	static RenderManager* Get();

	void render();

	void AddRenderPacket(RenderPack renderPacket);
	//void AddRenderPacketList(RenderPack renderPacket) { mRenderPackets[renderPacket.layer].push_back(renderPacket); }

	void AddDebugRenderPacker(const DebugRenderPack& renderPack);

	//void handleEvent(EventData& data) override;
	void addRenderable(Renderable* renderable);
	void removeRenderable(Renderable* renderable);

private:
	void renderPacketRange(int start, int end);
	//void renderPackets(RenderLayer layer);


private:
	std::vector<Renderable*> mRenderables;
	//std::vector<RenderPack> mRenderPackets;

	// renderlayers + the lowest
	std::vector<RenderPack> mRenderPackets[c_RenderLayers];
	std::vector<DebugRenderPack> mDebugRenders;
};
