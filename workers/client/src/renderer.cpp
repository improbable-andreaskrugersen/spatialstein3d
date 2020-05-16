#include "renderer.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "worldmap.hpp"
#include <iostream>

using namespace Eigen;

namespace {
inline void setSurfacePixel(SDL_Surface* pSurface, int x, int y, Uint32 color) {
  Uint32* pTargetPixel = reinterpret_cast<Uint32*>(static_cast<Uint8*>(pSurface->pixels) +
                                                   y * pSurface->pitch + x * sizeof(*pTargetPixel));
  *pTargetPixel = color;
}

inline Uint32 getSurfacePixel(SDL_Surface* pSurface, int x, int y) {
  Uint32* pTargetPixel = reinterpret_cast<Uint32*>(static_cast<Uint8*>(pSurface->pixels) +
                                                   y * pSurface->pitch + x * sizeof(*pTargetPixel));
  return *pTargetPixel;
}
}  // namespace

RayCasterRenderer::RayCasterRenderer(SDL_Window* pWindow, TTF_Font* pFont, int screenWidth,
                                     int screenHeight, int texWidth, int texHeight)
: m_pWindow(pWindow)
, m_pFont(pFont)
, m_screenWidth(screenWidth)
, m_screenHeight(screenHeight)
, m_texWidth(texWidth)
, m_texHeight(texHeight) {
  m_pScreenSurface = SDL_GetWindowSurface(pWindow);
  m_pBackSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0x00ff0000, 0x0000ff00,
                                        0x000000ff, 0xff000000);
  m_zBuffer.resize(m_screenWidth);
}

RayCasterRenderer::~RayCasterRenderer() {
  for (SDL_Surface* pTexture : m_darkTextures) {
    SDL_FreeSurface(pTexture);
  }
  for (SDL_Surface* pTexture : m_textures) {
    SDL_FreeSurface(pTexture);
  }
  if (m_pBackSurface) {
    SDL_FreeSurface(m_pBackSurface);
  }
  if (m_pWindow) {
    SDL_DestroyWindow(m_pWindow);
  }
}

const SDL_PixelFormat* RayCasterRenderer::getPixelFormat() const {
  return m_pScreenSurface->format;
}

void RayCasterRenderer::addTexture(SDL_Surface* pTexture) {
  m_textures.push_back(pTexture);
  m_darkTextures.push_back(createDarkTexture(pTexture));
}

void RayCasterRenderer::setFloorTextureIndex(std::size_t index) {
  m_floorTextureIndex = index;
}

void RayCasterRenderer::setCeilingTextureIndex(std::size_t index) {
  m_ceilingTextureIndex = index;
}

void RayCasterRenderer::render(const WorldMap& world, const Player& player,
                               const std::vector<Sprite>& sprites) const {
  renderFloorAndCeilling(player);
  renderWalls(world, player);
  renderSprites(player, sprites);
}

void RayCasterRenderer::renderText(const char* pText, int x, int y, SDL_Color color) const {
  SDL_Surface* pSurface = TTF_RenderText_Solid(m_pFont, pText, color);
  if (!pSurface) {
    std::cout << "Text render failed: " << TTF_GetError() << std::endl;
    return;
  }
  SDL_Rect rect{};
  rect.x = x;
  rect.y = y;
  SDL_BlitSurface(pSurface, nullptr, m_pBackSurface, &rect);
  SDL_FreeSurface(pSurface);
}

void RayCasterRenderer::present() const {
  SDL_BlitSurface(m_pBackSurface, nullptr, m_pScreenSurface, nullptr);
  SDL_UpdateWindowSurface(m_pWindow);
}

SDL_Surface* RayCasterRenderer::createDarkTexture(SDL_Surface* pSource) const {
  SDL_Surface* pDark = SDL_DuplicateSurface(pSource);
  for (int x = 0; x < pDark->w; ++x) {
    for (int y = 0; y < pDark->h; ++y) {
      Uint8 r, g, b;
      Uint32 color = getSurfacePixel(pDark, x, y);
      SDL_GetRGB(color, pDark->format, &r, &g, &b);
      color = SDL_MapRGB(pDark->format, r >> 1, g >> 1, b >> 1);
      setSurfacePixel(pDark, x, y, color);
    }
  }
  return pDark;
}

void RayCasterRenderer::renderWalls(const WorldMap& world, const Player& player) const {
  Vector2d pos = player.pos();

  for (int x = 0; x < m_screenWidth; ++x) {
    double cameraX =
        2 * x / static_cast<double>(m_screenWidth) - 1;  // x-coordinate in camera space
    Vector2d rayDir = player.dir() + player.camera().plane() * cameraX;

    // which cell of the map we're in
    Vector2i map = pos.cast<int>();

    // length of ray from current position to next x or y side
    Vector2d sideDist;

    // length of ray from one x or y side to next x or y side
    // potential div/0 is safe as infinity will be correctly handled
    Vector2d deltaDist{std::abs(1 / rayDir.x()), std::abs(1 / rayDir.y())};
    double perpWallDist;

    // what direction to step in x or y direction (either +1 or -1)
    int stepX, stepY;

    if (rayDir.x() < 0) {
      stepX = -1;
      sideDist.x() = (pos.x() - map.x()) * deltaDist.x();
    } else {
      stepX = 1;
      sideDist.x() = (map.x() + 1.0 - pos.x()) * deltaDist.x();
    }
    if (rayDir.y() < 0) {
      stepY = -1;
      sideDist.y() = (pos.y() - map.y()) * deltaDist.y();
    } else {
      stepY = 1;
      sideDist.y() = (map.y() + 1.0 - pos.y()) * deltaDist.y();
    }

    bool hit = false;
    bool sideHit = false;
    while (!hit) {
      if (sideDist.x() <= sideDist.y()) {
        sideDist.x() += deltaDist.x();
        map.x() += stepX;
        sideHit = false;
      } else {
        sideDist.y() += deltaDist.y();
        map.y() += stepY;
        sideHit = true;
      }

      if (world.at(map)) {
        hit = true;
      }
    }

    // calculate distance projected on camera direction (Euclidean distance will give fisheye
    // effect!)
    if (sideHit) {
      perpWallDist = (map.y() - pos.y() + (1 - stepY) / 2) / rayDir.y();
    } else {
      perpWallDist = (map.x() - pos.x() + (1 - stepX) / 2) / rayDir.x();
    }

    // calculate height of line to draw on screen
    int lineHeight = static_cast<int>(m_screenHeight / perpWallDist);

    // calculate lowest and highest pixel to fill in current stripe
    int drawStart = std::max(0, -lineHeight / 2 + m_screenHeight / 2);
    int drawEnd = std::min(m_screenHeight - 1, lineHeight / 2 + m_screenHeight / 2);

    int texIndex = world.at(map) - 1;

    // :TODO: remove when we have texture count validation
    if (texIndex >= (int)m_textures.size()) {
      texIndex = 0;
    }

    // where was the wall hit exactly
    double wallX =
        sideHit ? pos.x() + perpWallDist * rayDir.x() : pos.y() + perpWallDist * rayDir.y();
    wallX -= std::floor(wallX);

    // x coordinate of the texture
    int u = static_cast<int>(wallX * static_cast<double>(m_texWidth));
    if ((!sideHit && rayDir.x() > 0) || (sideHit && rayDir.y() < 0)) {
      u = m_texWidth - u - 1;
    }

    // how much to increase the texture coordinate per screen pixel
    double step = 1.0 * m_texHeight / lineHeight;

    // starting texture coordinate
    double texPos = (drawStart - m_screenHeight / 2 + lineHeight / 2) * step;
    for (int y = drawStart; y < drawEnd; ++y) {
      // cast the texture coordinate to integer and mask with (m_texHeight - 1) in case of overflow
      int v = static_cast<int>(texPos) & (m_texHeight - 1);
      texPos += step;

      Uint32 color =
          getSurfacePixel(sideHit ? m_darkTextures[texIndex] : m_textures[texIndex], u, v);

      setSurfacePixel(m_pBackSurface, x, y, color | 0xff000000);
    }

    m_zBuffer[x] = perpWallDist;
  }
}

void RayCasterRenderer::renderFloorAndCeilling(const Player& player) const {
  for (int y = 0; y < m_screenHeight; ++y) {
    Vector2d rayDirLeft = player.dir() - player.camera().plane();
    Vector2d rayDirRight = player.dir() + player.camera().plane();

    // vertical position of the camera
    const double posZ = 0.5 * m_screenHeight;

    // current y position compared to the center of the screen (horizon)
    const int p = y - static_cast<int>(posZ);

    // horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    const double rowDistance = posZ / p;

    // calculate the real world step vector we have to add for each x (parallel to the camera
    // plane). adding step by step avoids multiplications with a weight in the inner loop.
    const Vector2d floorStep = rowDistance * (rayDirRight - rayDirLeft) / m_screenWidth;

    // real world coordinates of the leftmost column
    Vector2d floor = player.pos() + rowDistance * rayDirLeft;

    Vector2i texSize{m_texWidth, m_texHeight};

    for (int x = 0; x < m_screenWidth; ++x) {
      Vector2i cell = floor.cast<int>();

      // texture coordinate
      int u = static_cast<int>(m_texWidth * (floor.x() - cell.x())) & (m_texWidth - 1);
      int v = static_cast<int>(m_texHeight * (floor.y() - cell.y())) & (m_texHeight - 1);

      floor += floorStep;

      // floor
      Uint32 color = getSurfacePixel(m_darkTextures[m_floorTextureIndex], u, v);
      setSurfacePixel(m_pBackSurface, x, y, color | 0xff000000);

      // ceiling
      color = getSurfacePixel(m_darkTextures[m_ceilingTextureIndex], u, v);
      setSurfacePixel(m_pBackSurface, x, m_screenHeight - y - 1, color | 0xff000000);
    }
  }
}

void RayCasterRenderer::renderSprites(const Player& player,
                                      const std::vector<Sprite>& sprites) const {
  for (const Sprite& sprite : sprites) {
    Vector2d toSprite = sprite.pos - player.pos();
    Vector2d transform = player.camera().inverseMatrix() * toSprite;

    // behind the camera?
    if (transform.y() <= 0) {
      continue;
    }

    int spriteScreenX = static_cast<int>(m_screenWidth / 2 * (1 + transform.x() / transform.y()));
    int spriteHeight = std::abs(static_cast<int>(m_screenHeight / transform.y()));

    // calculate lowest and highest pixel to fill in current stripe
    int drawStartY = std::max(0, -spriteHeight / 2 + m_screenHeight / 2);
    int drawEndY = std::min(m_screenHeight - 1, spriteHeight / 2 + m_screenHeight / 2);

    int spriteWidth = spriteHeight;
    int drawStartX = std::max(0, -spriteWidth / 2 + spriteScreenX);
    int drawEndX = std::min(m_screenWidth - 1, spriteWidth / 2 + spriteScreenX);

    for (int stripe = drawStartX; stripe < drawEndX; ++stripe) {
      int u = static_cast<int>(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * m_texWidth /
                               spriteWidth) /
          256;

      if (stripe > 0 && stripe < m_screenWidth && transform.y() < m_zBuffer[stripe]) {
        for (int y = drawStartY; y < drawEndY; ++y) {
          int d = y * 256 - m_screenHeight * 128 +
              spriteHeight * 128;  // 256 and 128 factors to avoid floats
          int v = ((d * m_texHeight) / spriteHeight) / 256;

          Uint32 color = getSurfacePixel(m_textures[sprite.texIndex], u, v);
          if (color & 0x00ffffff) {
            setSurfacePixel(m_pBackSurface, stripe, y, color | 0xff000000);
          }
        }
      }
    }
  }
}
