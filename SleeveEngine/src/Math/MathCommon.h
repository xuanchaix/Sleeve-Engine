#pragma once
#include <concepts>
#include <cmath>

//constexpr float PI = 3.14159265359f;
//constexpr float INV_SQRT_2 = 0.70710678118654752440084436210485f;
constexpr float PI = 3.1415926535897932384626433f;
constexpr float INV_SQRT_2 = 0.70710678118654752440084436210485f;

// Vectors can only have template type of float or double
template<typename T>
concept FloatingPointType = std::is_same_v<T, float> || std::is_same_v<T, double>;