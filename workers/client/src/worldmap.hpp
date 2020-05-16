#pragma once

#include <Eigen/Dense>

class WorldMap {
public:
  static constexpr int kMapWidth = 24;
  static constexpr int kMapHeight = 24;

  int at(const Eigen::Vector2i& pos) const;
  int at(int x, int y) const;

  bool isEmpty(const Eigen::Vector2i& pos) const;

private:
  static const int m_map[kMapWidth][kMapHeight];
};
