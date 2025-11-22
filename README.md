# ♟️ Chess Game — Built on the Water Engine

A **2D Chess game** developed as a **university project**, built entirely on my custom C++ framework: **The Water Engine**.  
This project is currently a **Work in Progress (WIP)**.

---

## 🌊 Overview

- Integration of custom engine systems  
- 2D rendering with SFML  
- Responsive interaction and board logic  
- Clean object-oriented structure tailored for game development

---

## ✨ Current Features (Rendering & Interaction Layer)

### ✔️ Pixel-Perfect Board Rendering
- The entire 8×8 chessboard is drawn precisely.
- Each square and piece aligns perfectly with the visual grid.

### ✔️ Initial Piece Placement
- All chess pieces are loaded from a **single sprite sheet**.
- The custom `ChessPiece` Actor initializes pieces in their correct starting positions.

### ✔️ Hover Highlighting
- The engine checks mouse position against each piece’s bounds.
- Hovered pieces are subtly tinted for instant visual feedback.

### ✔️ Engine Integration
- Uses Water Engine components such as:
  - `World` (object manager & update cycle)
  - `Actor` (base for all renderable objects)
  - Global Tick system (continuous input/update loop)

---

## 🛠️ The Water Engine Foundation

The Chess project is built on top of the Water Engine's core systems:

| Component | Responsibility | Status in Project |
|----------|----------------|------------------|
| **Actor** | Base for all game objects: rendering, position, bounds checking | Essential for board/pieces |
| **World** | Handles game state, object spawning, update & render cycles | Central game orchestrator |
| **Input System** | Provides frame-by-frame mouse position queries | Fully integrated and active |

---

## 🚀 Future Development Goals

The next phases of development will introduce full gameplay:

### 🧩 Piece Selection & Movement
- Mouse-based selection
- Drag-and-drop movement across the board

### ✔️ Move Validation
- Implement full chess rules (legal moves, captures, special rules)

### ♜ Game State Management
- Turn system
- Check/checkmate detection
- Move history
---

## ⚙️ Setup & Dependencies

This project uses **C++** and **SFML**.

### Requirements
- **Water Engine** source & libraries (must be linked)
- **SFML** modules:
  - `sfml-graphics`
  - `sfml-window`
- **Assets**
  - Board texture
  - Piece sprite sheet

Place all required images in the project’s `resources/` directory.

---

## 📌 Project Status
This is an early-stage but functional architecture, designed to be expanded into a complete chess experience.  
Core rendering and interaction are complete — move logic and game flow are next.

