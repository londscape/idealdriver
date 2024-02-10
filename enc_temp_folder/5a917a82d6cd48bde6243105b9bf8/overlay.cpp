#include "../pch.h"
#include "../../src/vector.h"
#include "../../src/memory.h"
#include "../../src/offsets.h"
#include "../hex/customFont.h"
#include "../../src/hacks/hacks.h"
#include "../../src/globals.h"
#include "../hex/icons/font_awesome.h"
#include "../hex/icons/font_awesome2.h"
#include <numbers>
#include <cmath>
#include <chrono>
#include <vector>
#include <windows.h>
#include <streambuf>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <string>

//char inputBuffer[256] = "";
ImFont* h1Font;
ImFont* h2Font;
ImFont* h3Font;
ImFont* icons_font;
bool pressedPanic = false;
bool memAttached = false;
bool startedThreads;
Memory mem{ "cs2.exe" };

auto lastTriggerTime = std::chrono::high_resolution_clock::now();

template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p) {
		p->Release();
		p = NULL;
	}
}

BOOL c_overlay::init_device() {
	allocConsole();
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
	ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
	vSwapChainDesc.BufferCount = 2;
	vSwapChainDesc.BufferDesc.Width = 0;
	vSwapChainDesc.BufferDesc.Height = 0;
	vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	vSwapChainDesc.OutputWindow = window_handle;
	vSwapChainDesc.SampleDesc.Count = 1;
	vSwapChainDesc.SampleDesc.Quality = 0;
	vSwapChainDesc.Windowed = TRUE;
	vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL vFeatureLevel;
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &vSwapChainDesc, &swap_chain, &d3d_device, &vFeatureLevel, &device_context) != S_OK)
		return false;

	init_render_target();
	
	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	return true;
}

VOID c_overlay::dest_device() {
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowTitleAlign.x = 0.50f;
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 0.0f;
	style.FramePadding = ImVec2(1, 1);
	style.FrameRounding = 0.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 0.0f;

	style.Colors[ImGuiCol_Text] = ImColor(213, 213, 213, 255);
	style.Colors[ImGuiCol_TextDisabled] = ImColor(125, 125, 125, 255);
	style.Colors[ImGuiCol_WindowBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_ChildBg] = ImColor(8, 8, 8, 255);
	style.Colors[ImGuiCol_PopupBg] = ImColor(65, 65, 65, 255);
	style.Colors[ImGuiCol_Border] = ImColor(10, 10, 10, 255);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImColor(32, 32, 38, 255);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(32, 32, 38, 255);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(8, 8, 8, 255);
	style.Colors[ImGuiCol_TitleBg] = ImColor(38, 31, 71, 255);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(38, 31, 71, 255);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(38, 31, 71, 255);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImColor(45, 45, 45, 255);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(65, 65, 65);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(75, 75, 75, 255);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(75, 75, 75, 255);
	style.Colors[ImGuiCol_CheckMark] = ImColor(150, 189, 99, 255);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(150, 189, 99, 255);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(160, 200, 100, 255);
	style.Colors[ImGuiCol_Button] = ImColor(28, 28, 28, 255);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(32, 32, 32, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(35, 35, 35, 255);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.043f, 0.043f, 0.043f, 0.73f);

	ImGuiIO& io = ImGui::GetIO();
	//font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18.0f);
	//font->font_size = 24.f;
	font->im_font = io.Fonts->AddFontFromMemoryTTF(customFont, sizeof(customFont), 18.f );
	h1Font = io.Fonts->AddFontFromMemoryTTF(customFont, sizeof(customFont), 16.f);
	h2Font = io.Fonts->AddFontFromMemoryTTF(customFont, sizeof(customFont), 15.f);
	h3Font = io.Fonts->AddFontFromMemoryTTF(customFont1, sizeof(customFont1), 8.5f);
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	static const ImWchar icon_ranges[]{ 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = true;
	icons_config.OversampleV = true;

	icons_font = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.5f, &icons_config, icon_ranges);

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer)
		d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
	if (!render_target_view)
		return;

	render_target_view->Release();
	render_target_view = NULL;
}

c_overlay::c_overlay() :
	ready(false),
	exit(false),
	window_handle(nullptr),
	window_class({}),
	window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
	window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
	font(nullptr),
	d3d_device(nullptr),
	device_context(nullptr),
	swap_chain(nullptr),
	render_target_view(nullptr),
	draw_list(nullptr),
	breath(NULL),
	menu_ticks(NULL)
{
	window_handle = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay");
	HWND cs2hwnd = FindWindowA(NULL, "Counter-Strike 2");
	auto getInfo = GetWindowLongA(window_handle, -20);
	auto changeAttributes = SetWindowLongA(window_handle, -20, (LONG_PTR)(getInfo | 0x20));
	LONG_PTR windowStyle = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
	SetWindowLongPtr(window_handle, GWL_EXSTYLE, windowStyle & ~WS_EX_TRANSPARENT);

	SetFocus(cs2hwnd);
	SetFocus(window_handle);

	MARGINS margins = { -1 };
	margins.cyBottomHeight = margins.cyTopHeight = margins.cxLeftWidth = margins.cxRightWidth = -1;
	DwmExtendFrameIntoClientArea(window_handle, &margins);
	SetLayeredWindowAttributes(window_handle, 0x000000, 0xFF, 0x02);
	SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);

	if (!window_handle)
		return;

	font = new s_font();

	if (!init_device())
		return;
	
	init_imgui();

	std::thread(&c_overlay::input_handler, this).detach();
	std::thread(&c_overlay::anim_handler, this).detach();
}

c_overlay::~c_overlay() {
	dest_imgui();
	dest_device();

	delete font;
}

const void c_overlay::render(FLOAT width, FLOAT height) {
	if (exit) {
		ready = true;
		return;
	}

	if (pressedPanic) {
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();

	globals::base_module = mem.GetModuleAddress("client.dll");

	if (startedThreads == false) {
		startedThreads = true;
	}

	init_draw_list();

	double menu_anim = sin((M_PI / 2.0) * (((menu_ticks == 1 ? 0 : 1) * pow(menu_ticks, 2)) / ((double)pow(menu_anim_time - 1, 2))));
	style.Alpha = menu_anim;
	style.Colors[ImGuiCol_Border] = ImVec4(0.76f, 0.28f, 0.28f, ((breath / (float)breath_anim_time) * .7f) + .3f);

	style.Colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, ((breath / (float)breath_anim_time) * .7f) + .3f);

	float outerRadius = config::aim::fovSlider;
	float innerRadius = outerRadius - 1.0f;
	ImU32 circleColor = ImColor(255, 255, 255);
	ImU32 backcolor = ImColor(0, 0, 0, 50);

	//if (config::esp::bBoxes || config::esp::bHeadCircles || config::esp::bHealthBar || config::esp::bHealthInt || config::esp::bSkeleton || config::esp::bSnaplines || config::esp::bTeam) {
	if (gui::isRunning) {
		const uintptr_t localPlayer = mem.Read<uintptr_t>(globals::base_module + offsets::dwLocalPlayer);
		const uintptr_t entity_list = mem.Read<uintptr_t>(globals::base_module + offsets::dwEntityList);
		const uintptr_t m_bSpotted = mem.Read<uintptr_t>(globals::base_module + offsets::m_bSpotted);
		const bool entityDormant = mem.Read<bool>(entity_list + offsets::m_bDormant);


		if (!localPlayer || !entity_list)
			return;

		const int8_t localTeam = mem.Read<int>(localPlayer + offsets::m_iTeamNum);
		const int8_t crosshairid = mem.Read<int>(localPlayer + offsets::iIDEntIndex);


		if (config::aim::bFovCircle) {
			ImU32 borderColor = ImColor(0, 0, 0);

			ImGui::GetBackgroundDrawList()->AddCircle({ static_cast<float>(globals::windowWidth) / 2.0f, static_cast<float>(globals::windowHeight) / 2.0f }, outerRadius, borderColor, 64, 1.0f);
			ImGui::GetBackgroundDrawList()->AddCircle({ static_cast<float>(globals::windowWidth) / 2.0f, static_cast<float>(globals::windowHeight) / 2.0f }, innerRadius, circleColor, 64, 1.0f);
		}

		if (config::aim::bFillc) {
			ImGui::GetBackgroundDrawList()->AddCircleFilled({ static_cast<float>(globals::windowWidth) / 2.0f, static_cast<float>(globals::windowHeight) / 2.0f }, innerRadius, backcolor, 64);
		}

		if (config::misc::bCros) {
			auto draw = ImGui::GetBackgroundDrawList();
			draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), config::esp::crossSize, IM_COL32(255, 255, 255, 255), 100, 0.0f);
			ImGui::End();
		}

		if (config::esp::bRadar) {

		}


		if (config::esp::bDemo) {
			ImGui::ShowDemoWindow();
		}



		const view_matrix_t view_matrix = mem.Read<view_matrix_t>(globals::base_module + offsets::dwViewMatrix);
		const std::uint32_t localPlayerPawn = mem.Read<std::uint32_t>(localPlayer + offsets::m_hPlayerPawn);

		if (!localPlayerPawn) // a 0 is here, prolly offsets, commented the return out for the time being, filtering teams doesnt work now XD
			;
		//return;

		const uintptr_t localList_entry2 = mem.Read<uintptr_t>(entity_list + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
		const uintptr_t localpCSPlayerPawn = mem.Read<uintptr_t>(localList_entry2 + 120 * (localPlayerPawn & 0x1FF));

		if (!localpCSPlayerPawn)
			return;

		Vector3 localOrigin = mem.Read<Vector3>(localpCSPlayerPawn + offsets::m_vecOrigin);
		int playerIndex = 0;
		uintptr_t list_entry;

		for (int playerIndex = 0; playerIndex < 32; ++playerIndex) {
			list_entry = mem.Read<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
			if (!list_entry)
				break;
			const uintptr_t player = mem.Read<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));
			if (!player)
				continue;
			const int playerTeam = mem.Read<int>(player + offsets::m_iTeamNum);
			if (!config::esp::bShowTeam) {
				if (playerTeam == localTeam) {
					continue;
				}
			}

			const std::uint32_t playerPawn = mem.Read<std::uint32_t>(player + offsets::m_hPlayerPawn);
			const uintptr_t list_entry2 = mem.Read<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

			if (!list_entry2)
				continue;

			const uintptr_t pCSPlayerPawn = mem.Read<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));

			if (!pCSPlayerPawn)
				continue;

			const int playerHealth = mem.Read<int>(pCSPlayerPawn + offsets::m_iHealth);

			if (playerHealth <= 0 || playerHealth > 100)
				continue;

			if (pCSPlayerPawn == localPlayer)
				continue;

			uint64_t gamescene = mem.Read<uint64_t>(pCSPlayerPawn + offsets::m_pGameSceneNode);
			uint64_t bonearray = mem.Read<uint64_t>(gamescene + offsets::modelState + 0x80);

			Vector3 origin = mem.Read<Vector3>(pCSPlayerPawn + offsets::m_vecOrigin);
			Vector3 headPos = mem.Read<Vector3>(bonearray + 6 * 32);
			Vector3 cou = mem.Read<Vector3>(bonearray + 5 * 32);
			Vector3 cock = mem.Read<Vector3>(bonearray + 0 * 32);

			if ((localOrigin - origin).length2d() < 10)
				continue;

			Vector3 feetPos = origin.w2s(view_matrix);

			Vector3 screenHead = (headPos + Vector3{ 0.f, 0.f, 11.f }).w2s(view_matrix);
			Vector3 trueScreenHead = headPos.w2s(view_matrix);

			Vector3 screenHou = (cou + Vector3{ 0.f, 0.f, 11.f }).w2s(view_matrix);
			Vector3 TrueScreenHou = cou.w2s(view_matrix);

			Vector3 screenCock = (cock + Vector3{ 0.f, 0.f, 11.f }).w2s(view_matrix);
			Vector3 TrueScreenCock = cock.w2s(view_matrix);

			Vector3 screenCenter = { static_cast<float>(globals::windowWidth) / 2.0f, static_cast<float>(globals::windowHeight) / 2.0f, 0.0f };
			float distance = localOrigin.calculate_distance(origin);
			float roundedDist = std::round(distance) / 10.f;
			int distInMeters = static_cast<int>(roundedDist);

			const uintptr_t localPlayer = mem.Read<uintptr_t>(globals::base_module + offsets::dwLocalPlayer);
			const uintptr_t entity_list = mem.Read<uintptr_t>(globals::base_module + offsets::dwEntityList);

			float distFromCross;
			float maxDist = FLT_MAX;

			if (config::aim::bAimbot && GetAsyncKeyState(VK_XBUTTON2) & 0x8000) {
				distFromCross = std::sqrt((trueScreenHead.x - screenCenter.x) * (trueScreenHead.x - screenCenter.x) + (trueScreenHead.y - screenCenter.y) * (trueScreenHead.y - screenCenter.y));

				if (distFromCross < config::aim::fovSlider) {
					if (distFromCross < maxDist) {
						maxDist = distFromCross;
						float interpolationFactor = 1.0f - std::pow(1.0f - config::aim::smoothness, ImGui::GetIO().DeltaTime * 60.0f);

						int moveX = static_cast<int>((trueScreenHead.x - screenCenter.x) * interpolationFactor);
						int moveY = static_cast<int>((trueScreenHead.y - screenCenter.y) * interpolationFactor);

						mouse_event(MOUSEEVENTF_MOVE, moveX, moveY, 0, 0);
					}
				}
			}

			if (config::aim::bTrigger && (GetKeyState(VK_XBUTTON1) & 0x8000)) {
				distFromCross = std::sqrt((trueScreenHead.x - screenCenter.x) * (trueScreenHead.x - screenCenter.x) + (trueScreenHead.y - screenCenter.y) * (trueScreenHead.y - screenCenter.y));

				if (distFromCross < config::aim::fovtrigger) {
					if (distFromCross < maxDist) {
						maxDist = distFromCross;

						auto currentTime = std::chrono::high_resolution_clock::now();
						auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTriggerTime).count();

						if (elapsedMilliseconds >= config::aim::triggerdelay * 1000) {
							mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

							trueScreenHead = headPos.w2s(view_matrix);
							lastTriggerTime = std::chrono::high_resolution_clock::now();
						}
					}
				}
			}

			float healthPercentage = static_cast<float>(playerHealth) / static_cast<float>(100);

			ImDrawList* drawList = ImGui::GetBackgroundDrawList();

			ImGui::PushFont(h3Font);

			if (feetPos.z >= 0.5f) {
				float height = feetPos.y - screenHead.y;
				float width = height / 3.0f;
				if (config::esp::bBoxes) {
					float rounding = 4.0f;
					ImGui::GetBackgroundDrawList()->AddRect({ screenHead.x - width, screenHead.y }, { screenHead.x + width, feetPos.y }, ImColor(255, 255, 255), config::esp::bRounding, 0, 1.0f);
				}

				ImGui::PushFont(h3Font);
				if (config::esp::bSnaplines) {
					ImVec2 lineTop = { feetPos.x, feetPos.y };
					ImVec2 lineBottom = { 1920 / 2, 1080 };

					drawList->AddLine(lineTop, lineBottom, IM_COL32(0, 0, 0, 255), config::esp::bLineout);
					drawList->AddLine(lineTop, lineBottom, IM_COL32(255, 255, 255, 255));
				}
				if (config::esp::bHealthBar) {
					ImVec2 healthBarMin = { screenHead.x - width - 7.0f, screenHead.y + (feetPos.y - screenHead.y) * (1.0f - healthPercentage) };
					ImVec2 healthBarMax = { screenHead.x - width - 5.7f, feetPos.y };


					float borderThickness = 2.3f;

					ImGui::GetBackgroundDrawList()->AddRect(healthBarMin, healthBarMax, ImColor(0, 0, 0), 0.0f, 0, borderThickness);
					ImGui::GetBackgroundDrawList()->AddRectFilled(healthBarMin, healthBarMax, ImColor((255 - playerHealth), (55 + playerHealth * 2), 75), 3.0f);

				}

				Vector3 head = mem.Read<Vector3>(bonearray + 6 * 32);
				Vector3 cou = mem.Read<Vector3>(bonearray + 5 * 32);
				Vector3 shoulderR = mem.Read<Vector3>(bonearray + 8 * 32);
				Vector3 shoulderL = mem.Read<Vector3>(bonearray + 13 * 32);
				Vector3 brasR = mem.Read<Vector3>(bonearray + 9 * 32);
				Vector3 brasL = mem.Read<Vector3>(bonearray + 14 * 32);
				Vector3 handR = mem.Read<Vector3>(bonearray + 11 * 32);
				Vector3 handL = mem.Read<Vector3>(bonearray + 16 * 32);
				Vector3 cock = mem.Read<Vector3>(bonearray + 0 * 32);
				Vector3 kneesR = mem.Read<Vector3>(bonearray + 23 * 32);
				Vector3 kneesL = mem.Read<Vector3>(bonearray + 26 * 32);
				Vector3 feetR = mem.Read<Vector3>(bonearray + 24 * 32);
				Vector3 feetL = mem.Read<Vector3>(bonearray + 27 * 32);

				if (config::esp::bSkeleton) {

					ImGui::GetBackgroundDrawList()->AddLine({ (head.w2s(view_matrix)).x, (head.w2s(view_matrix)).y }, { (cou.w2s(view_matrix)).x, (cou.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (cou.w2s(view_matrix)).x, (cou.w2s(view_matrix)).y }, { (shoulderR.w2s(view_matrix)).x, (shoulderR.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (shoulderR.w2s(view_matrix)).x, (shoulderR.w2s(view_matrix)).y }, { (brasR.w2s(view_matrix)).x, (brasR.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (brasR.w2s(view_matrix)).x, (brasR.w2s(view_matrix)).y }, { (handR.w2s(view_matrix)).x, (handR.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (cou.w2s(view_matrix)).x, (cou.w2s(view_matrix)).y }, { (shoulderL.w2s(view_matrix)).x, (shoulderL.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (shoulderL.w2s(view_matrix)).x, (shoulderL.w2s(view_matrix)).y }, { (brasL.w2s(view_matrix)).x, (brasL.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (brasL.w2s(view_matrix)).x, (brasL.w2s(view_matrix)).y }, { (handL.w2s(view_matrix)).x, (handL.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (cou.w2s(view_matrix)).x, (cou.w2s(view_matrix)).y }, { (cock.w2s(view_matrix)).x, (cock.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (cock.w2s(view_matrix)).x, (cock.w2s(view_matrix)).y }, { (kneesR.w2s(view_matrix)).x, (kneesR.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (kneesR.w2s(view_matrix)).x, (kneesR.w2s(view_matrix)).y }, { (feetR.w2s(view_matrix)).x, (feetR.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (cock.w2s(view_matrix)).x, (cock.w2s(view_matrix)).y }, { (kneesL.w2s(view_matrix)).x, (kneesL.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
					ImGui::GetBackgroundDrawList()->AddLine({ (kneesL.w2s(view_matrix)).x, (kneesL.w2s(view_matrix)).y }, { (feetL.w2s(view_matrix)).x, (feetL.w2s(view_matrix)).y }, ImColor(255, 255, 255), 1.f);
				}
				if (config::esp::bHeadCircles) {
					ImGui::GetBackgroundDrawList()->AddRect({ (head.w2s(view_matrix)).x, (head.w2s(view_matrix)).y }, { (head.w2s(view_matrix)).x, (head.w2s(view_matrix)).y }, ImColor(255, 0, 0), 0.0f, 1.f);
				}

				if (config::esp::bHealthInt) {
					ImGui::GetBackgroundDrawList()->AddText({ screenHead.x + width + 5.f, screenHead.y + 0.f }, ImColor(255, 255, 255), (std::to_string(playerHealth) + "hp").c_str());
				}


				ImGui::PopFont();
			}
			ImGui::PopFont();
		}
	}

	ImGui::SetNextWindowSize(ImVec2(390, 400));
	ImGui::PushFont(h3Font);
	ImGui::Begin("Idealdriver", nullptr, ImGuiWindowFlags_NoResize);
	ImGui::PopFont();
	ImGui::PushFont(icons_font);

	if (gui::isRunning) {
			ImGui::BeginChild("Sidebar", ImVec2(100, 0), true);
			ImGui::PushFont(h1Font);

			if (ImGui::Button(" WH")) {
				gui::showVisuals = true;
				gui::showMisc = false;
				gui::showAim = false;
				gui::showConfig = false;
			}

			if (ImGui::Button(" AIM")) {
				gui::showVisuals = false;
				gui::showMisc = false;
				gui::showAim = true;
				gui::showConfig = false;
			}

			if (ImGui::Button(" MISC")) {
				gui::showVisuals = false;
				gui::showMisc = true;
				gui::showAim = false;
				gui::showConfig = false;
			}

			if (ImGui::Button(" SAVE")) {
				gui::showVisuals = false;
				gui::showMisc = false;
				gui::showAim = false;
				gui::showConfig = true;
			}

			ImGui::EndChild();
	}
	ImGui::PopFont();

	ImGui::SameLine();

	if (gui::isRunning) {
			if (gui::showVisuals) {
				ImGui::BeginChild("esp main", ImVec2(250, 0), true);
				ImGui::PushFont(h2Font);
				ImGui::Text("Main");
				ImGui::Separator();
				ImGui::Checkbox("Enable", &config::esp::bEnable);
				if (config::esp::bEnable) {
					ImGui::Checkbox("Boxes", &config::esp::bBoxes);
					if (config::esp::bBoxes) {
						ImGui::Text("Box rounding");
						ImGui::SliderFloat("Box rounding", &config::esp::bRounding, 1.0f, 80.0f);
					}
					ImGui::Text("Bone");
					ImGui::Separator();
					ImGui::Checkbox("Head Box", &config::esp::bHeadCircles);
					ImGui::Checkbox("Skeleton", &config::esp::bSkeleton);
					ImGui::Text("Other");
					ImGui::Separator();
					ImGui::Checkbox("Show Team", &config::esp::bShowTeam);
					ImGui::Checkbox("Health", &config::esp::bHealthInt);
					ImGui::Checkbox("Health Bar", &config::esp::bHealthBar);
					ImGui::Checkbox("Snaplines", &config::esp::bSnaplines);
					if (config::esp::bSnaplines) {
						ImGui::Text("Snaplines outline");
						ImGui::SliderFloat("Outline", &config::esp::bLineout, 1.0f, 20.0f);
					}
				}
				ImGui::PopFont();
				ImGui::SetNextItemWidth(120.f);
				ImGui::EndChild();
			}

			if (gui::showAim) {
				ImGui::BeginChild("aim", ImVec2(250, 0), true);
				ImGui::PushFont(h2Font);
				ImGui::Text("Aimbot");
				ImGui::Separator();
				ImGui::Checkbox("Enabled", &config::aim::bAimbot);
				if (config::aim::bAimbot) {
					ImGui::Checkbox("Aim radius", &config::aim::bFovCircle);
					if (config::aim::bFovCircle) {
						ImGui::Checkbox("Filled circle", &config::aim::bFillc);
					}
					ImGui::Checkbox("Autofire on aim radius", &config::aim::bAutofire);
					ImGui::Text("Smooth");
					ImGui::SameLine();
					ImGui::SliderFloat("Smoothness", &config::aim::smoothness, 0.2f, 1.0f);
					ImGui::SetNextItemWidth(120.f);
					ImGui::Text("Fov radius");
					ImGui::SameLine();
					ImGui::SliderFloat("FOV", &config::aim::fovSlider, 1.f, 80.f);
				}
				ImGui::Text("Trigger");
				ImGui::Separator();
				ImGui::Checkbox("Enabled trigger", &config::aim::bTrigger);
				if (config::aim::bTrigger) {
					ImGui::Text("Radius");
					ImGui::SameLine();
					ImGui::SliderFloat("Trigger radius", &config::aim::fovtrigger, 1.f, 20.f);
					ImGui::Text("Delay");
					ImGui::SameLine();
					ImGui::SliderFloat("Trigger delay", &config::aim::triggerdelay, 1.f, 3.f);
				}

				ImGui::PopFont();
				ImGui::EndChild();
			}

			if (gui::showMisc) {
				ImGui::BeginChild("misc", ImVec2(250, 0), true);
				ImGui::PushFont(h2Font);
				ImGui::Text("Miscellaneous");
				ImGui::Checkbox("Crosshair", &config::misc::bCros);
				if (config::misc::bCros) {
					ImGui::SliderFloat("Crosshair Size", &config::esp::crossSize, 1.0f, 10.0f);
				}
				ImGui::Checkbox("Radarhack", &config::esp::bRadar);
				ImGui::Checkbox("Imgui demo", &config::esp::bDemo);

				ImGui::Checkbox("Size mode function", &config::aim::bSemirage);
				if (config::aim::bSemirage) {

					/*ImGui::InputText("Code", inputBuffer, sizeof(inputBuffer));

					if (ImGui::Button("1")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "1");
					}
					ImGui::SameLine();
					if (ImGui::Button("2")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "2");
					}
					ImGui::SameLine();
					if (ImGui::Button("3")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "3");
					}

					if (ImGui::Button("4")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "4");
					}
					ImGui::SameLine();
					if (ImGui::Button("5")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "5");
					}
					ImGui::SameLine();
					if (ImGui::Button("6")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "6");
					}

					if (ImGui::Button("7")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "7");
					}
					ImGui::SameLine();
					if (ImGui::Button("8")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "8");
					}
					ImGui::SameLine();
					if (ImGui::Button("9")) {
						strcat_s(inputBuffer, sizeof(inputBuffer), "9");
					}

					if (ImGui::Button("Back")) {
						size_t length = strlen(inputBuffer);
						if (length > 0) {
							inputBuffer[length - 1] = '\0';
						}
					}

					if (std::atoi(inputBuffer) == 1299) {
						ImGui::Separator();
						ImGui::Text("Aim fov");
						ImGui::SameLine();
						ImGui::SliderFloat("Aim fov", &config::aim::fovSlider, 1.f, 1000.f);

						ImGui::Text("Crpsshair size");
						ImGui::SameLine();
						ImGui::SliderFloat("Crosshair Size", &config::esp::crossSize, 1.0f, 1000.0f);

						ImGui::Text("Line outline size");
						ImGui::SameLine();
						ImGui::SliderFloat("Outline", &config::esp::bLineout, 1.0f, 100.0f);

						ImGui::Text("Box rounding");
						ImGui::SameLine();
						ImGui::SliderFloat("Box rounding", &config::esp::bRounding, 1.0f, 200.0f);
					}*/

				}

				ImGui::PopFont();
				ImGui::EndChild();
			}


			if (gui::showConfig) {
				ImGui::BeginChild("configs", ImVec2(250, 0), true);
				ImGui::PushFont(h2Font);
				ImGui::Text("Configuration");
				ImGui::Separator();

				if (ImGui::Button("Close process")) {

				}
				ImGui::PopFont();
				ImGui::EndChild();
			}

			if (!config::esp::bEnable) {
				config::esp::bBoxes = false;
				config::esp::bSnaplines = false;
				config::esp::bLineout = false;
				config::esp::bHealthBar = false;
				config::esp::bHealthInt = false;
				config::esp::bHeadCircles = false;
				config::esp::bRounding = false;
				config::esp::bShowTeam = false;
				config::esp::bSkeleton = false;
			}

			if (!config::aim::bAimbot) {
				config::aim::bFovCircle = false;
				config::aim::bFillc = false;
				config::aim::bAutofire = false;
			}

			ImGui::End();
	}

	if (menu_ticks == 1)
			return;
}

BOOL c_overlay::msg_loop()
{
	if (window_handle) {
		MSG msg{ 0 };
		ImVec4 clear_clr = { 0,0,0,0 };

		HWND cs2hwnd = FindWindowA(NULL, "Counter-Strike 2");
		HWND cs2hwndActive = GetForegroundWindow();

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		render(window_width, window_height);

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(1, 0);

	}
	return false;
}

void c_overlay::input_handler() {
	for (; !exit; Sleep(1)) {
		ImGuiIO& io = ImGui::GetIO();

		POINT p{};
		GetCursorPos(&p);
		io.MousePos = ImVec2((float)p.x, (float)p.y);

		io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
	}
}

void c_overlay::anim_handler() {
	breath_timer.start();
	menu_timer.start();

	for (;!exit;) {
		RECT rect{ 0 };
		POINT point{ 0 };
		GetClientRect(window_handle, &rect);
		SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
			if (gui::isOpen) {
				LONG_PTR windowStyle = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
				SetWindowLongPtr(window_handle, GWL_EXSTYLE, windowStyle | WS_EX_TRANSPARENT);
			}
			// idfk
			gui::isOpen = false;
			gui::isRunning = false;
			config::esp::bEnable = false;
			config::esp::bBoxes = false;
			config::esp::bSnaplines = false;
			config::esp::bLineout = false;
			config::esp::bHealthBar = false;
			config::esp::bHealthInt = false;
			config::esp::bHeadCircles = false;
			config::esp::bRounding = false;
			config::esp::bShowTeam = false;
			config::esp::bSkeleton = false;
			config::esp::bWeapon = false;
			config::misc::bCros = false;
			config::esp::crossSize = false;
			config::esp::bDemo = false;
			config::esp::bRadar = false;
			config::aim::bAimbot = false;
			config::aim::bFovCircle = false;
			config::aim::bSemirage = false;
			config::aim::bFillc = false;
			config::aim::bTrigger = false;
			config::aim::bAutofire = false;
			config::aim::smoothingSlider = false;
			menu_ticks = menu_anim_time;
			menu_timer.start();
			pressedPanic = true;

		}
		
		int breath_tmr = breath_timer.end();
		if (breath_tmr >= breath_anim_time * 2) {
			breath = breath_anim_time;
			breath_timer.start();
		}
		else if (breath_tmr > breath_anim_time)
			breath = breath_anim_time - (breath_tmr - breath_anim_time);
		else if (breath_tmr <= breath_anim_time)
			breath = breath_tmr;

		int menu_tmr = menu_timer.end();
		if (GetAsyncKeyState(VK_INSERT) & 0x8000 && menu_tmr > menu_anim_time) {
			if (gui::isOpen) {
				LONG_PTR windowStyle = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
				SetWindowLongPtr(window_handle, GWL_EXSTYLE, windowStyle | WS_EX_TRANSPARENT);
			}
			else {
				LONG_PTR windowStyle = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
				SetWindowLongPtr(window_handle, GWL_EXSTYLE, windowStyle & ~WS_EX_TRANSPARENT);
			}
			gui::isOpen = !gui::isOpen;
			menu_ticks = menu_anim_time;
			menu_timer.start();
		}
		else if (menu_tmr < menu_anim_time) {
			menu_ticks = (gui::isOpen ? menu_tmr : menu_anim_time - menu_tmr);
		}
	}
}

VOID c_overlay::init_draw_list() {
	draw_list = ImGui::GetBackgroundDrawList();
}

VOID c_overlay::crosshair(const FLOAT aSize, ImU32 color) {
	draw_list->AddLine({ window_width / 2,window_height / 2 - (aSize + 1) }, { window_width / 2 ,window_height / 2 + (aSize + 1) }, color, 2);
	draw_list->AddLine({ window_width / 2 - (aSize + 1),window_height / 2 }, { window_width / 2 + (aSize + 1), window_height / 2 }, color, 2);
}

VOID c_overlay::box(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width) {
	std::array<ImVec2, 4> box_lines{ pos,ImVec2{pos.x + width,pos.y},ImVec2{pos.x + width,pos.y + height},ImVec2{pos.x,pos.y + height} };
	draw_list->AddPolyline(box_lines.data(), box_lines.size(), color, true, 2);
}

VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width) {
	draw_list->AddLine(point1, point2, color, line_width);
}

VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircleFilled(point, radius, color, 200); // num segments is how circular the circle actually is
}

VOID c_overlay::text(const ImVec2& pos, const std::string& text, ImU32 color, bool outline) {
	if (outline) {
		draw_list->AddText(font->im_font, 18.f, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font->im_font, 18.f, pos, color, text.c_str());
}

VOID c_overlay::radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out) {
	if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
		return;

	draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
	const int count = draw_list->_Path.Size - 1;

	unsigned int vtx_base = draw_list->_VtxCurrentIdx;
	draw_list->PrimReserve(count * 3, count + 1);

	const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
	draw_list->PrimWriteVtx(center, uv, col_in);
	for (int n = 0; n < count; n++)
		draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);
	
	for (int n = 0; n < count; n++) {
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	draw_list->_Path.Size = 0;
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}