#pragma once

#include <Eigen/Dense>

class Camera {
public:
  explicit Camera(const Eigen::Vector2d& playerDir, double fov);

  const Eigen::Vector2d& plane() const {
    return m_plane;
  }
  const Eigen::Matrix2d& inverseMatrix() const {
    return m_inverseMatrix;
  }

  void updateFromPlayerDir(const Eigen::Vector2d& dir);

private:
  Eigen::Vector2d m_plane;
  Eigen::Matrix2d m_inverseMatrix;
  double m_fov;
};
