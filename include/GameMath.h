#pragma once

#include <glm/vec3.hpp>

enum lineMode
{
	LERP,
	CATMULLROM,
	BEZIER
};

namespace Math
{
	//// Linear interpolation
	template <typename T>
	T lerp(T d0, T d1, float t)
	{
		return (1 - t) * d0 + d1 * t;
	}

	// inverse lerp
	template <typename T>
	float invLerp(T d, T d0, T d1)
	{
		return (d - d0) / (d1 - d0);
	}

	//// Catmull-Rom interpolation

	// Returns a point between p1 and p2
	template <typename T>
	T catmull(T p0, T p1, T p2, T p3, float t)
	{
		return	((p1 * 2.0f) + (-p0 + p2) * t +
			((p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3) * (t * t) +
			(-p0 + (p1 * 3.0f) - (p2 * 3.0f) + p3) * (t * t * t)) * 0.5f;
	}

	//// Bezier

	// Base case
	template <typename T>
	T bezier(T p0, float t)
	{
		return p0;
	}

	// Two points (Lerp)
	template <typename T>
	T bezier(T p0, T p1, float t)
	{
		return Math::lerp(Math::bezier(p0, t), Math::bezier(p1, t), t);
	}

	// Three points (lerping lerp)
	template <typename T>
	T bezier(T p0, T p1, T p2, float t)
	{
		return Math::lerp(Math::bezier(p0, p1, t),
			Math::bezier(p1, p2, t), t);
	}

	// Four points (lerp skerp)
	template <typename T>
	T bezier(T p0, T p1, T p2, T p3, float t)
	{
		return Math::lerp(Math::bezier(p0, p1, p2, t),
			Math::bezier(p1, p2, p3, t), t);
	}
}