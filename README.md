# DX11 Engine

## How to build on Windows

### Prerequisite

- CMake
- vcpkg (tho not necessary yet)

### Command to build demo

```shell
# Release
cmake --preset windows-msvc
cmake --build --preset windows-msvc-build

# Debug
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug-build
```

> There are still some unreferenced symbol warnings. But you should be good to go.

## Engine Structure

```
Game
├── InputManager
├── Window
├── DxEngine
│   ├── DxSwapchain
│   └── DxDeviceManager
├── Camera (base) : Camera Buffer
│   ├── FirstPersonCamera
│   └── ThirdPersonCamera
└── GameResourceManager
    ├── Entities
    │   ├── Entity (base): Model Buffer
    │   │   ├── ControllableEntity : Move/Rotate Control
    │   └── RenderComponent
    │       ├── Mesh : Vertex Buffer
    │       └── Material
    │           ├── Shader
    │           └── Texture : Texture Buffer
    └── Lights : Light Buffer
```

## Demo Key Bindings

```
Camera Controls:
    F1: First-Person View (Default);
    F2: Third-Person View;

View Controls (Third-Person Camera):
    Mouse Drag: Rotate camera angle
    Mouse Scroll: Zoom in/out

Spaceship States:
    0: Free Flight;
    1: Landed on Earth;
    2: Landed on Moon;
    3: Orbiting Earth;
    4: Orbiting Moon;

Spaceship Controls:
    Free Flight Mode:
        W: Move forward; S: Move backward;
        A: Strafe left; D: Strafe right;
        Q: Ascend (up); E: Descend (down);
        ← (Left Arrow): Turn left (yaw); → (Right Arrow): Turn right (yaw);
        ↑ (Up Arrow): Pitch up; ↓ (Down Arrow): Pitch down;
    Landed Mode:
        W: Move forward along latitude; S: Move backward along latitude;
        A: Move up along longitude; D: Move down along longitude;

General:
        ESC: Exit / Quit;
```

## Credit

- [Checker Texture 1](https://polycount.com/discussion/186513/free-checker-pattern-texture)
- [Checker Texture 2](https://www.oxpal.com/uv-checker-texture.html)
- [Solar Textures](https://www.solarsystemscope.com/textures/)
