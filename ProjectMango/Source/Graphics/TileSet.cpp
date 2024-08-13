#include "pch.h"
#include "Tileset.h"
#include "Texture.h"


// --- Tile --- //
void Tile::render(RectF targetRect) const
{
	texture->renderSubTexture(targetRect, rect);
}

void Tile::render(RectF targetRect, SDL_RendererFlip flip) const
{
	texture->renderSubTexture(targetRect, rect, flip);
}


void Tile::render(RectF targetRect, SDL_RendererFlip flip, Uint8 alpha)
{
	texture->renderSubTexture(targetRect, rect, flip, alpha);
}




// --- Tileset --- //
Tileset::Tileset(TilesetData& data) :
	mTexture(data.texture),
	tileDimentions(data.tileSize),
	mTiles(Grid<Tile>(data.tileCount, Tile()))
{
	for (int y = 0; y < mTiles.yCount(); y++)
	{
		for (int x = 0; x < mTiles.xCount(); x++)
		{
			VectorI index(x, y);
			Tile& tile = mTiles[index];

			RectF rect(tileDimentions * index.toFloat(), tileDimentions);

			tile.setTileRect(rect);
			tile.setTexture(mTexture);
		}
	}
}


Tile& Tileset::getTile(VectorI index)
{
	ASSERT(mTiles.inBounds(index),
		"Animation index (%d, %d) out of sprite sheet bounds (%d, %d)",
		index.x, index.y, mTiles.xCount(), mTiles.yCount());

	return mTiles[index];
}
