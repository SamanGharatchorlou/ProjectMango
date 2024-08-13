#pragma once


struct Renderable
{
	Renderable();
	~Renderable();

	virtual void render() = 0;
};