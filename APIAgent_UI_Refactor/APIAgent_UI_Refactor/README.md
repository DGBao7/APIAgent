# APIAgent UI refactor

This folder contains a UI-only refactor of PR #17. It keeps the Qt Widgets interface independent from the APIAgent console executable.

## Structure

```text
UI/
├── CMakeLists.txt
├── main.cpp
└── ui/
    ├── theme/
    │   ├── Color.h
    │   └── Color.cpp
    ├── Conversation.h
    ├── Conversation.cpp
    ├── History.h
    ├── History.cpp
    ├── InputBar.h
    ├── InputBar.cpp
    ├── MainWindow.h
    └── MainWindow.cpp
```

## Responsibilities

- `MainWindow`: lays out the application and connects child widgets.
- `InputBar`: accepts text and emits `message_submitted` when Enter is pressed. It has no microphone, file, or send-button features.
- `Conversation`: displays user and AI messages. The AI/backend response is not wired yet.
- `History`: static visual placeholder; clicking entries does not load sessions.
- `theme/Color`: central QSS theme.

## Build

Run from this folder in an environment with Qt 6 Widgets installed:

```sh
cmake -S . -B build
cmake --build build
```

This is a standalone UI target named `app_main`; it does not replace the APIAgent console `main.cpp` at the repository root. `MainWindow` exposes the `user_message_submitted(QString)` signal as the integration point for the orchestration/backend layer.
