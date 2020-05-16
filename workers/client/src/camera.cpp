#include "camera.hpp"

Camera::Camera(const Eigen::Vector2d& playerDir, double fov) : m_fov(fov) {
  updateFromPlayerDir(playerDir);
}

void Camera::updateFromPlayerDir(const Eigen::Vector2d& dir) {
  m_plane.x() = dir.y() * m_fov;
  m_plane.y() = -dir.x() * m_fov;

  // calculate inverse camera matrix
  m_inverseMatrix << dir.y(), -dir.x(), -m_plane.y(), m_plane.x();
  const double invDet = 1.0 / (m_plane.x() * dir.y() - dir.x() * m_plane.y());
  m_inverseMatrix *= invDet;
}
