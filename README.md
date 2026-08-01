# FrankEngine - starter skeleton

A minimal, plugin-based C++ game engine:

- **Engine** — static lib (`Engine.lib`), linked into both **Editor.exe** and **Runtime.exe**.
- **Editor.exe** — ImGui/GLFW/OpenGL editor.
- **Runtime.exe** — the player. Links Engine statically, but loads the game
  itself (`Game.dll`) dynamically via `LoadLibrary`, so a project's game code
  can be rebuilt and re-run without rebuilding the engine.
- **SDK** (`IGame.h`, `EngineAPI.h`) — the only headers a `Game.dll` needs.
  This is the contract across the DLL boundary.

## Folder layout

```
build/<Debug|Release>/...        compiler output (created by tasks)
deps/glfw-3.4.bin.WIN32/         put the GLFW binary release here
ext/imgui/                       put the Dear ImGui source here
include/SDK/                     IGame.h, EngineAPI.h
include/Engine/core/             ISubsystem, Application, IDraw, object
include/Engine/utility/          DeltaTimer
include/Editor/core/             Editor.h (App/Editor classes)
include/Editor/system/           EditorGUISystem.h
include/Editor/UI/Widget/        EditorWidget base + sample widgets
src/engine/                      Engine static lib sources
src/editor/                      Editor.exe sources
src/Kernel/                      GameLauncher.cpp -> Runtime.exe
games/SampleGame/                example Game.dll implementing IGame
.vscode/                         tasks.json, launch.json, c_cpp_properties.json
```

`deps/` and `ext/imgui/` are **not** included here — drop in the real GLFW
3.4 Windows binaries and the Dear ImGui source (both referenced by your
original tree) at those exact paths and everything else lines up.

## Building

Requires MSVC (`cl.exe`, `lib.exe`) on PATH — open the project from a
**Developer Command Prompt / Developer PowerShell for VS 2022**, or run
`vcvars64.bat` first, then use VS Code's **Terminal → Run Build Task**
(`Ctrl+Shift+B`), which runs `Build All (Debug)`. That chain:

1. `Prepare Build Folders` — creates the `build/Debug/...` tree
2. `Compile Engine (Debug)` → `Lib Engine (Debug)` — produces `Engine.lib`
3. `Build Runtime (Debug)` — links `GameLauncher.cpp` + `Engine.lib` → `Runtime.exe`
4. `Build Editor (Debug)` — compiles ImGui + GLFW backend + editor sources,
   links `Engine.lib`, `glfw3.lib`, `opengl32.lib` → `Editor.exe`
5. `Build SampleGame (Debug)` — compiles `games/SampleGame` as `Game.dll`
6. `Copy SampleGame -> Runtime` — drops `Game.dll` next to `Runtime.exe` so it runs

Run/debug via **Run and Debug** (`F5`): "Debug Editor" or "Debug Runtime".

## What was fixed from the first draft

- **`Editor.h`** had a stray free function (`Initiation()`) referencing
  `MyUI`/`this` outside of any class — didn't compile. Removed.
- **`App::run()`** called `GetUI()->init()/render()/shutdown()`, none of
  which existed (`EditorGUISystem` has `Initialize()/Update()/Shutdown()`
  from `ISubsystem`, and they were `private`). `App` now calls the
  `ISubsystem` names, and they're `public` on `EditorGUISystem`.
- **`EditorGUISystem.h`** included `"imGUI.h"` (wrong case/name) — fixed to
  `<imgui.h>`, and `EditorWidget.h` (referenced but never provided) was
  added under `include/Editor/UI/Widget/Base/`.
- **`DeltaTimer.h`** was missing `#pragma once`.
- **`GameLauncher.cpp`**: since Engine is now a static lib linked directly
  into `Runtime.exe`, the `LoadLibrary("EngineRuntime.dll")` step for the
  engine itself is gone — `GetEngineAPI()` is just a normal function call.
  Only `Game.dll` is still loaded dynamically. Its include path
  (`"SDK/IGame.h"`) now matches the real `include/SDK/IGame.h` location.
  Its loop also used `Sleep(1000)` (1 update/sec) — changed to ~60/sec.
- **`EngineAPI.h`**'s `__declspec(dllexport/dllimport)` macro assumed
  Engine was a DLL. Since it's a static lib now, that's gone — no export
  boundary needed for a statically-linked function.

## Next steps worth doing

- Add a real `RenderSystem`/`IDraw` pipeline instead of just clearing the
  screen in `EditorGUISystem::Render()`.
- Give the Editor a "New Project" flow that copies `Runtime.exe` +
  `Engine.lib`'s dependencies into a project folder, per your original plan.
- Add a "Play" button in the editor that rebuilds the current project's
  `Game.dll` and launches `Runtime.exe` next to it.
