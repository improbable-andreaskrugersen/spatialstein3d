#pragma once

#include "sdl.hpp"
#include <Eigen/Dense>
#include <vector>

class Player;
class WorldMap;

struct Sprite {
  Eigen::Vector2d pos;
  std::size_t texIndex;
  double distance;
};

class RayCasterRenderer {
public:
  RayCasterRenderer(SDL_Window* pWindow, TTF_Font* pFont, int screenWidth, int screenHeight,
                    int texWidth, int texHeight);
  ~RayCasterRenderer();

  const SDL_PixelFormat* getPixelFormat() const;

  void addTexture(SDL_Surface* pTexture);
  void setFloorTextureIndex(std::size_t index);
  void setCeilingTextureIndex(std::size_t index);

  // Renders the scene to the back buffer
  void render(const WorldMap& map, const Player& player, const std::vector<Sprite>& sprites) const;

  // Renders text directly to the back buffer
  void renderText(const char* pText, int x, int y, SDL_Color color) const;

  // Copies the back buffer to the window
  void present() const;

private:
  SDL_Surface* createDarkTexture(SDL_Surface* pSource) const;

  void renderWalls(const WorldMap& world, const Player& player) const;
  void renderFloorAndCeilling(const Player& player) const;
  void renderSprites(const Player& player, const std::vector<Sprite>& sprites) const;

  SDL_Window* m_pWindow;
  TTF_Font* m_pFont;
  SDL_Surface* m_pScreenSurface;
  SDL_Surface* m_pBackSurface;
  int m_screenWidth;
  int m_screenHeight;
  int m_texWidth;
  int m_texHeight;
  std::vector<SDL_Surface*> m_textures;
  std::vector<SDL_Surface*> m_darkTextures;
  std::size_t m_floorTextureIndex = 0;
  std::size_t m_ceilingTextureIndex = 0;

  mutable std::vector<double> m_zBuffer;
};
