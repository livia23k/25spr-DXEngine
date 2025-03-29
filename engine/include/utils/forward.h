#pragma once

#include "utils/logger.h"
#include "utils/keycode.h"
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

// Forward class declarations

class Game;
class InputManager;
class WindowWin;

class GameResourceManager;

class DXEngine;
class DXDeviceManager;
class DXSwapchain;

class CameraBase;
class FirstPersonCamera;
class ThirdPersonCamera;

class Light;

class EntityBase;
class ControllableEntity;

class Mesh;
class MaterialBase;
class LambertianMaterial;
class TextureBase;
class ConstantTexture;
class ImageTexture;
class Shader;
class RenderComponent;

struct Vertex;
struct CameraBuffer;
struct ModelBuffer;
// struct MaterialBuffer;
// struct LightBuffer;
// struct LightArrayBuffer;

std::string MatrixToString(const DirectX::XMMATRIX &matrix);