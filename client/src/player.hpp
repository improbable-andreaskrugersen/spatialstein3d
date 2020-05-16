#pragma once

#include "camera.hpp"
#include <Eigen/Dense>

class Player {
public:
  Player(const Eigen::Vector2d& pos, const Eigen::Vector2d& dir, Camera& camera);

  const Eigen::Vector2d& pos() const {
    return m_pos;
  }
  const Eigen::Vector2d& dir() const {
    return m_dir;
  }
  const Camera& camera() const {
    return m_camera;
  }

  Eigen::Vector2d posPlusX(double delta) const;
  Eigen::Vector2d posPlusY(double delta) const;

  void move(const Eigen::Vector2d& delta);
  void rotate(double angle);

private:
  Eigen::Vector2d m_pos;
  Eigen::Vector2d m_dir;
  Camera& m_camera;
};
