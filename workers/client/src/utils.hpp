#pragma once

#include <cstddef>

template <typename T, std::size_t N>
constexpr std::size_t count_of(const T (&)[N]) noexcept {
  return N;
}
