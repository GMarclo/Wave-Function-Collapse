# Wave-Function-Collapse
This project is my C++ implementation of the Wave Function Collapse (WFC) algorithm using the SFML graphics library. It generates patterns on a 2D grid by progressively collapsing tile possibilities while respecting adjacency rules, similar to procedural texture or map generation.

🚀 Features

- Tile-based procedural generation using WFC.

- Interactive controls:

    - Press Enter to advance the collapse step-by-step or resume after interruptions.

    - Click & drag with the left mouse button to reset/renew tiles manually.

    - Press Escape to close the application.

- Weighted randomness: Tile chances are read from tile_probabilities.txt so different tiles can appear more or less frequently.

- Dynamic rendering: Tiles are drawn in real time with a gradually increasing framerate as the collapse progresses.

- Customizable tile set: Comes with 41 tile variants split across multiple colors.

🖼️ How it Works

- The grid starts with all possible tiles available for each cell.

- A priority queue selects the next tile to collapse, weighted by entropy and probability.

- Once a tile is chosen, neighbor constraints propagate through the grid, reducing possibilities.

- This continues until the grid is fully collapsed into a valid tiling.

📂 Project Structure

- WFC.cpp – Main program loop and SFML rendering.

- wfc_header.h – Core WFC logic, data structures, and utility functions.

- tile_probabilities.txt – Tile definitions and probability weights.

- tiles.png – Sprite sheet containing tile textures (required).
