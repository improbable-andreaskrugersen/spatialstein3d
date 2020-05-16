#include "player.hpp"

using namespace Eigen;

Player::Player(const Vector2d& pos, const Vector2d& dir, Camera& camera)
: m_pos(pos), m_dir(dir), m_camera(camera) {}

Eigen::Vector2d Player::posPlusX(double delta) const {
  return Vector2d{m_pos.x() + delta, m_pos.y()};
}

Eigen::Vector2d Player::posPlusY(double delta) const {
  return Vector2d{m_pos.x(), m_pos.y() + delta};
}

void Player::move(const Eigen::Vector2d& delta) {
  m_pos += delta;
}

void Player::rotate(double angle) {
  Rotation2Dd rot{angle};
  m_dir = rot.toRotationMatrix() * m_dir;
  m_camera.updateFromPlayerDir(m_dir);
}
