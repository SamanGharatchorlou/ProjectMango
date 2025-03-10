#pragma once

struct Screen
{
	std::vector<ECS::Entity> elements;

};

struct UIManager
{
	Screen hud;
};