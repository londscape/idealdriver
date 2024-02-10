#pragma once

#include <numbers>
#include <cmath>
#include <vector>

//#include "globals.h"

FILE* allocConsole()
{
	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);

	HWND consoleWindow = GetConsoleWindow();
	if (consoleWindow != nullptr)
	{
		DWORD consoleProcessId;
		GetWindowThreadProcessId(consoleWindow, &consoleProcessId);
		if (consoleProcessId != GetCurrentProcessId())
		{
			FreeConsole();
			AttachConsole(consoleProcessId);
			freopen_s(&fp, "CONOUT$", "w", stdout);
		}
	}
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	return fp;
}

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3
{
	// constructor
	constexpr Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) { }

	// operator overloads
	constexpr const Vector3& operator-(const Vector3& other) const noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	constexpr const Vector3& operator+(const Vector3& other) const noexcept
	{
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept
	{
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept
	{
		return Vector3{ x * factor, y * factor, z * factor };
	}

	constexpr const bool operator>(const Vector3& other) const noexcept {
		return x > other.x && y > other.y && z > other.z;
	}

	constexpr const bool operator>=(const Vector3& other) const noexcept {
		return x >= other.x && y >= other.y && z >= other.z;
	}

	constexpr const bool operator<(const Vector3& other) const noexcept {
		return x < other.x && y < other.y && z < other.z;
	}

	constexpr const bool operator<=(const Vector3& other) const noexcept {
		return x <= other.x && y <= other.y && z <= other.z;
	}

	// utils
	constexpr const Vector3& ToAngle() const noexcept
	{
		return Vector3{
			std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
			std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>),
			0.0f };
	}

	float calculate_distance(const Vector3& point) {
		float dx = point.x - x;
		float dy = point.y - y;
		float dz = point.z - z;

		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	float length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	float length2d() const {
		return std::sqrt(x * x + y * y);
	}

	constexpr const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	Vector3 w2s(view_matrix_t matrix) const {
		float _x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3];
		float _y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3];

		float w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3];

		float inv_w = 1.f / w;
		_x *= inv_w;
		_y *= inv_w;

		// Check the values at this point
		//std::cout << "_x: " << _x << ", _y: " << _y << ", w: " << w << std::endl;

		float screen_x = (1920 * 0.5f);
		//float screen_x = (globals::windowWidth * 0.5f);
		float screen_y = (1080 * 0.5f);
		//float screen_y = (globals::windowHeight * 0.5f);

		screen_x += 0.5f * _x * 1920 + 0.5f;
		//screen_x += 0.5f * _x * globals::windowWidth + 0.5f;
		screen_y -= 0.5f * _y * 1080 + 0.5f;
		//screen_y -= 0.5f * _y * globals::windowHeight + 0.5f;

		// Check the final screen coordinates
		//std::cout << "screen_x: " << screen_x << ", screen_y: " << screen_y << std::endl;

		return { screen_x, screen_y, w };
	}

	// struct data
	float x, y, z;
};