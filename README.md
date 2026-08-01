# FrankEngine

## Layout

```
sdk/include/            IGame.h, EngineAPI.h  - the ONLY thing a project may include
engine/include/Engine/  ISubsystem, Application, IDraw, object, DeltaTimer
engine/src/             EngineAPI.cpp (implements GetEngineAPI())
editor/include/Editor/  Editor.h, EditorGUISystem.h, EditorWidget + sample widgets
editor/src/             Editor.cpp, EditorGUISystem.cpp
runtime/src/            GameLauncher.cpp -> Runtime.exe
projects/SampleGame/    Source/SampleGame.cpp, deployed bin/Game.dll + bin/SampleGame.exe
deps/                   prebuilt third-party binaries (GLFW) - not included, drop in yourself
ext/                    third-party source we compile ourselves (Dear ImGui) - drop in yourself
build/                  100% generated - obj/ (intermediates) + bin/ (Engine.lib/Runtime.exe/Editor.exe)
```

## Building

Open from a **Developer Command Prompt / Developer PowerShell for VS 2022**
(needs `cl.exe`/`lib.exe` on PATH), then `Ctrl+Shift+B` runs `Build All (Debug)`:

1. `Prepare Build Folders`
2. `Compile Engine (Debug)` -> `Lib Engine (Debug)` — `build/Debug/bin/Engine/Engine.lib`
3. `Build Runtime (Debug)` — `build/Debug/bin/Runtime/Runtime.exe`
4. `Build Editor (Debug)` — `build/Debug/bin/Editor/Editor.exe`
5. `Build SampleGame (Debug)` — `projects/SampleGame/bin/Game.dll`
6. `Deploy Runtime -> SampleGame` — copies `Runtime.exe` into
   `projects/SampleGame/bin/SampleGame.exe`

Debug via `F5`: **Debug Editor** or **Debug SampleGame**.

## Why the /I include paths per task matter

Each compile task in `tasks.json` only adds the `/I` roots that module is
actually allowed to see:

| Task | `/I` roots | Can reach `engine/include`? |
|---|---|---|
| Engine | `sdk/include` | no (doesn't need it) |
| Runtime | `sdk/include` | no (talks to Engine only via `EngineAPI*`) |
| Editor | `editor/include`, `engine/include` | yes (not crossing the Game.dll ABI boundary) |
| SampleGame (project) | `sdk/include` | **no - and this is enforced by the build, not just convention** |

That last row is the point of the restructure: a project physically cannot
`#include "Engine/core/Application.h"` and have it compile, because
`engine/include` was never added to its task. If it needs more capability
from the Engine, the fix is to grow `EngineAPI` in `sdk/include/EngineAPI.h`
— see `project-include-boundaries.md` from earlier for why.

Note: `c_cpp_properties.json` (IntelliSense only) includes every module's
headers for convenient browsing/autocomplete — that's just editor tooling
and doesn't affect what actually compiles; the real boundary is enforced
per-task in `tasks.json`.
