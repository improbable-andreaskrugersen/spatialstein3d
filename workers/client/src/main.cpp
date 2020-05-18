#include "camera.hpp"
#include "player.hpp"
#include "renderer.hpp"
#include "sdl.hpp"
#include "utils.hpp"
#include "worldmap.hpp"
#include <Eigen/Dense>
#include <improbable/worker.h>
#include <iostream>
#include <string>

using namespace Eigen;

namespace {
static constexpr int kScreenWidth = 1920;
static constexpr int kScreenHeight = 1080;
static constexpr int kTexWidth = 64;
static constexpr int kTexHeight = 64;
static constexpr double kFov = 1;
static constexpr double kMoveSpeed = 5.0;
static constexpr double kTurnSpeed = 3.0;
static const Vector2d kStartPos = Vector2d{22, 11.5};
static const Vector2d kStartDir = Vector2d{0, 1};

static const std::string kTexturePaths[] = {
    "assets/eagle.png",     "assets/redbrick.png",   "assets/purplestone.png",
    "assets/greystone.png", "assets/bluestone.png",  "assets/mossy.png",
    "assets/wood.png",      "assets/colorstone.png", "assets/barrel.png",
    "assets/pillar.png",    "assets/greenlight.png"};

static const std::string kFontPath = "assets/VT323-Regular.ttf";
static const SDL_Color kTextColor{255, 255, 255, 255};

// clang-format off
static std::vector<Sprite> kSprites = {
  // green lights in every room
  {Vector2d{20.5, 11.5}, 10, 0},
  {Vector2d{18.5, 4.5}, 10, 0},
  {Vector2d{10, 4.5}, 10, 0},
  {Vector2d{10, 12.5}, 10, 0},
  {Vector2d{3.5, 6.5}, 10, 0},
  {Vector2d{3.5, 20.5}, 10, 0},
  {Vector2d{3.5, 14.5}, 10, 0},
  {Vector2d{14.5, 20.5}, 10, 0},

  // row of pillars in front of wall
  {Vector2d{18.5, 10.5}, 9, 0},
  {Vector2d{18.5, 11.5}, 9, 0},
  {Vector2d{18.5, 12.5}, 9, 0},

  //some barrels around the map
  {Vector2d{21.5, 1.5}, 8, 0},
  {Vector2d{15.5, 1.5}, 8, 0},
  {Vector2d{16.0, 1.8}, 8, 0},
  {Vector2d{16.2, 1.2}, 8, 0},
  {Vector2d{3.5,  2.5}, 8, 0},
  {Vector2d{9.5, 15.5}, 8, 0},
  {Vector2d{10.0, 15.1}, 8, 0},
  {Vector2d{10.5, 15.8}, 8, 0},
};
// clang-format on
}  // namespace

struct Input {
  bool quit;

  bool forward;
  bool back;
  bool left;
  bool right;
};

RayCasterRenderer* init() {
  SDL_Window* pWindow;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    return nullptr;
  } else {
    pWindow = SDL_CreateWindow("Spatialstein3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN);
    if (!pWindow) {
      std::cout << "Could not create window: " << SDL_GetError() << std::endl;
      return nullptr;
    }
  }

  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    std::cout << "Could not initialize PNG library: " << IMG_GetError() << std::endl;
    SDL_DestroyWindow(pWindow);
    return nullptr;
  }

  if (TTF_Init() != 0) {
    std::cout << "Could not initialize TTF library: " << TTF_GetError() << std::endl;
    SDL_DestroyWindow(pWindow);
    return nullptr;
  }

  TTF_Font* pFont = TTF_OpenFont(kFontPath.c_str(), 24);
  if (!pFont) {
    std::cout << "Could not load font '" << kFontPath << "': " << TTF_GetError() << std::endl;
    SDL_DestroyWindow(pWindow);
    return nullptr;
  }

  return new RayCasterRenderer(pWindow, pFont, kScreenWidth, kScreenHeight, kTexWidth, kTexHeight);
}

SDL_Surface* loadImageFromFile(const std::string& filename, const SDL_PixelFormat& format) {
  SDL_Surface* pSurface = nullptr;

  SDL_Surface* pLoaded = IMG_Load(filename.c_str());
  if (!pLoaded) {
    std::cout << "Could not load texture '" << filename << "'" << std::endl;
    return nullptr;
  }

  if (pLoaded->w != kTexWidth || pLoaded->h != kTexHeight) {
    std::cout << "Invalid texture size for '" << filename << "', must be " << kTexWidth << " * "
              << kTexHeight << std::endl;
    SDL_FreeSurface(pLoaded);
    return nullptr;
  }

  pSurface = SDL_ConvertSurface(pLoaded, &format, 0);
  if (!pSurface) {
    std::cout << "Could not optimize texture '" << filename
              << "' to screen format: " << SDL_GetError() << std::endl;
  }

  SDL_FreeSurface(pLoaded);

  return pSurface;
}

bool loadTextures(RayCasterRenderer& renderer) {
  for (std::size_t i = 0; i < count_of(kTexturePaths); ++i) {
    SDL_Surface* pTexture = loadImageFromFile(kTexturePaths[i], *renderer.getPixelFormat());
    if (pTexture) {
      renderer.addTexture(pTexture);
    } else {
      return false;
    }
  }
  return true;
}

// Sort sprites from farthest to nearest
void sortSprites(std::vector<Sprite>& sprites, const Vector2d& playerPos) {
  for (Sprite& sprite : sprites) {
    sprite.distance = (playerPos - sprite.pos).squaredNorm();
  }
  std::sort(sprites.begin(), sprites.end(),
            [](const Sprite& lhs, const Sprite& rhs) { return lhs.distance > rhs.distance; });
}

void pollInput(Input& input) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      input.quit = true;
    } else {
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        input.quit = true;
        break;

      case SDLK_w:
      case SDLK_UP: {
        input.forward = event.type == SDL_KEYDOWN;
        break;
      }

      case SDLK_s:
      case SDLK_DOWN: {
        input.back = event.type == SDL_KEYDOWN;
        break;
      }

      case SDLK_a:
      case SDLK_LEFT: {
        input.left = event.type == SDL_KEYDOWN;
        break;
      }

      case SDLK_d:
      case SDLK_RIGHT: {
        input.right = event.type == SDL_KEYDOWN;
        break;
      }

      default:
        break;
      }
    }
  }
}

using AllComponents = worker::Components<>;

int main() {

  RayCasterRenderer* pRenderer = init();
  if (!pRenderer) {
    SDL_Quit();
    return -1;
  }

  // :TODO: validate that we have loaded the same amount of textures as required by the map
  if (!loadTextures(*pRenderer)) {
    delete pRenderer;
    SDL_Quit();
    return -1;
  }

  pRenderer->setFloorTextureIndex(3);
  pRenderer->setCeilingTextureIndex(6);

  AllComponents allComponents{};

  worker::LogsinkParameters logsink;
  logsink.Type = worker::LogsinkType::kStdout;
  logsink.FilterParameters.Categories = worker::LogCategory::kApi;
  logsink.FilterParameters.Level = worker::LogLevel::kInfo;

  worker::ConnectionParameters params;
  params.WorkerType = "client";
  params.Network.ConnectionType = worker::NetworkConnectionType::kModularKcp;
  params.Network.UseExternalIp = false;
  params.Logsinks = {logsink};
  params.EnableLoggingAtStartup = true;

  worker::Connection connection =
      worker::Connection::ConnectAsync(allComponents, "localhost", 7777,
                                       "client", params)
          .Get();
  if (connection.GetConnectionStatusCode() ==
      worker::ConnectionStatusCode::kSuccess) {
    std::cout << "Connection successful" << std::endl;
  }

  WorldMap world;

  Camera camera{kStartDir, kFov};
  Player player{kStartPos, kStartDir, camera};

  Uint32 time = 0;
  Uint32 oldTime = 0;

  Input input{};

  char fpsBuffer[15];

  bool quit = false;
  while (!quit) {
    oldTime = time;
    time = SDL_GetTicks();
    double frameTime = static_cast<double>(time - oldTime) / 1000.0;
    double fps = 1.0 / frameTime;
    snprintf(fpsBuffer, count_of(fpsBuffer), "FPS: %.2f", fps);

    double moveSpeed = frameTime * kMoveSpeed;
    double rotSpeed = frameTime * kTurnSpeed;

    pollInput(input);
    if (input.quit) {
      quit = true;
    } else {
      if (input.forward) {
      } else if (input.back) {
        moveSpeed *= -1;
      } else {
        moveSpeed = 0;
      }

      if (moveSpeed != 0.0) {
        Vector2d delta{0, 0};
        Vector2d inc{player.dir().x() * moveSpeed, player.dir().y() * moveSpeed};
        if (world.isEmpty(player.posPlusX(inc.x()).cast<int>())) {
          delta.x() = inc.x();
        }
        if (world.isEmpty(player.posPlusY(inc.y()).cast<int>())) {
          delta.y() = inc.y();
        }
        player.move(delta);
      }

      if (input.left) {
        player.rotate(rotSpeed);
      } else if (input.right) {
        player.rotate(-rotSpeed);
      }
    }

    sortSprites(kSprites, player.pos());

    pRenderer->render(world, player, kSprites);
    pRenderer->renderText(fpsBuffer, 10, 10, kTextColor);
    pRenderer->present();
  }

  delete pRenderer;
  SDL_Quit();

  return 0;
}
