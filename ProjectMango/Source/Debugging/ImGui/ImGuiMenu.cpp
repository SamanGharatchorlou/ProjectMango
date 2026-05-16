#include "pch.h"
#include "ImGuiMenu.h"

#include "SDL_render.h"
#include "imgui-master/imgui.h"
#include "imgui-master/src/imgui_impl_sdl2.h"
#include "imgui-master/src/imgui_impl_sdlrenderer2.h"
#include "imgui_sdl-master/imgui_sdl.h"

#include "Graphics/Renderer.h"
#include "System/Window.h"
#include "Debugging/AnimationEditor.h"

#if IMGUI
namespace DebugMenu
{
	void DoEntityPartSystemWindow(bool entity_view);
	void DoTweakerWindow();
	void DoGameStateWindow();
	void DoInputWindow();
	void DoTransformWindow();

	struct DebugState
	{
		bool entitySystemWindow = false;
		bool partViewerWindow = false;
		bool inputWindow = false;
		bool transformWindow = false;
		bool gameStateWindow = false;
		bool tweakerWindow = false;
		bool editorWindow = false;
		bool demoWindow = false;

		bool hidden = false;
	};

	static DebugState s_debugState;

	SharedState& DebugMenu::GetSharedState()
	{
		static SharedState s_state;
		return s_state;
	}

	u32 GetSelectedEntity() { return GetSharedState().selectedEntity; }
	void DebugMenu::SelectEntity(ECS::Entity entity) { GetSharedState().selectedEntity = entity; }


	void ToggleShow()
	{
		s_debugState.hidden = !s_debugState.hidden;

		if(!s_debugState.hidden)
		{
			SDL_ShowCursor(SDL_ENABLE);
		}
		else
		{
			SDL_ShowCursor(SDL_DISABLE);
		}
	}

	void Init()
	{
		SDL_Renderer* renderer = Renderer::Get()->sdlRenderer();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		SDL_ShowCursor(SDL_DISABLE);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForSDLRenderer(GameData::Get().window->get(), renderer);
		ImGui_ImplSDLRenderer2_Init(renderer);
	}


	bool HandleInput(SDL_Event& event)
	{
		if(s_debugState.hidden)
			return false;

		ImGui_ImplSDL2_ProcessEvent(&event);

		// consume game inputs when over gui window
		bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
		return is_window_hovered;
	}

	void OpenEditorWindow()
	{
		s_debugState.editorWindow = true;
		s_debugState.hidden = false;
	}

	void Draw()
	{
		if(s_debugState.hidden)
			return;

		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("MainWindow", 0, ImGuiWindowFlags_MenuBar);
		if (ImGui::Checkbox("Entity System", &s_debugState.entitySystemWindow))
			if (s_debugState.entitySystemWindow) s_debugState.partViewerWindow = false;
		ImGui::SameLine();
		if (ImGui::Checkbox("Part Viewer", &s_debugState.partViewerWindow))
			if (s_debugState.partViewerWindow) s_debugState.entitySystemWindow = false;
		ImGui::SameLine();
		ImGui::Checkbox("Input", &s_debugState.inputWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Transforms", &s_debugState.transformWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Game State", &s_debugState.gameStateWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Tweakers", &s_debugState.tweakerWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Demo Window", &s_debugState.demoWindow);
		ImGui::SameLine();
		ImGui::End();

		// these two are the same panel
		if (s_debugState.entitySystemWindow || s_debugState.partViewerWindow)
		{
			DoEntityPartSystemWindow(s_debugState.entitySystemWindow);
		}

		if (s_debugState.inputWindow)
		{
			DoInputWindow();
		}

		if(s_debugState.transformWindow) 
		{
			DoTransformWindow();
		}

		if(s_debugState.gameStateWindow)
		{
			DoGameStateWindow();
		}

		if(s_debugState.tweakerWindow)
		{
			DoTweakerWindow();
		}

		if(s_debugState.editorWindow)
		{
			AnimationEditor::DoEditor();
		}

		if(s_debugState.demoWindow)
			ImGui::ShowDemoWindow();

		ImGui::Render();

		SDL_Renderer* renderer = Renderer::Get()->sdlRenderer();
		ImGuiIO& io = ImGui::GetIO();
		//SDL_RenderSetScale(renderer, 0.5f, 0.5f);// io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	}
}
#endif