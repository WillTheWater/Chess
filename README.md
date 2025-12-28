# ♟️ Chess — Built using Water Engine v1.0.0

A **2D Chess game** using my **Water Engine**.  
This project is currently a **Work in Progress (WIP)**.

---

## 🌊 Overview

- Integration of custom engine systems  
- 2D rendering with SFML  
- Responsive interaction and board logic

---

## 🛠️ The Water Engine Foundation

The Chess project is built on top of the Water Engine's core systems:

| Component | Responsibility | Status in Project |
|----------|----------------|------------------|
| **Actor** | Base for all game objects: rendering, position, bounds checking | Essential for board/pieces |
| **World** | Handles game state, object spawning, update & render cycles | Central game orchestrator |
| **Input System** | Provides frame-by-frame mouse position queries | Fully integrated and active |
| **Integrated UI** | Allows native full-screen using Windows API | Fully integrated and active |

---



## 📌 Project Status
Currently 90% complete. Need to allow users to choose promotion piece and integrate it into the UI. On promotion needs to recheck for wins/check/stalemate.
