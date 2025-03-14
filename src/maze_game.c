/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2024-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include <stdlib.h>     // Required for: malloc(), free()

#define MAZE_WIDTH          64
#define MAZE_HEIGHT         64
#define MAZE_SCALE          10.0f

#define MAX_MAZE_ITEMS      16

// Declare new data type: Point
typedef struct Point {
    int x;
    int y;
} Point;

// Generate procedural maze image, using grid-based algorithm
// NOTE: Functions defined as static are internal to the module
static Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");
    InitAudioDevice();

    Sound sfx = LoadSound("resources/PointSFX.wav");
    Sound song = LoadSound("resources/BackgroundMusic.wav");

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    int seed = 92683;
    SetRandomSeed(seed);

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If imMaze pixel data is modified, texMaze needs to be re-loaded
    Texture texMaze = LoadTextureFromImage(imMaze);

    // Player start-position and end-position initialization
    Point startCell = { 2, 2 };
    Point endCell = { imMaze.width - 2, imMaze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 mazePosition = {
        GetScreenWidth()/2 - texMaze.width*MAZE_SCALE/2,
        GetScreenHeight()/2 - texMaze.height*MAZE_SCALE/2
    };

    // Define player position and size
    Rectangle player = { mazePosition.x + startCell.x * MAZE_SCALE, mazePosition.y + startCell.y * MAZE_SCALE, 1, 1 };

    Vector2 position = {
        screenWidth / 2 - imMaze.width * MAZE_SCALE / 2,
        screenHeight / 2 - imMaze.height * MAZE_SCALE / 2
    };

    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 5.0f;

    // Mouse selected cell for maze editing
    Point selectedCell = { 0 };

    // Maze items position and state
    Point mazeItems[MAX_MAZE_ITEMS] = { 0 };
    bool mazeItemPicked[MAX_MAZE_ITEMS] = { 0 };
    
    // Define textures to be used as our "biomes"
    Texture texBiomes[4] = { 0 };
    texBiomes[0] = LoadTexture("resources/maze_biome01.png");
    texBiomes[1] = LoadTexture("resources/maze_biome02.png");
    //texBiomes[2] = LoadTexture("resources/maze_biome03.png");

    // TODO: Load additional textures for different biomes
    int currentBiome = 0;

    // TODO: Define all variables required for game UI elements (sprites, fonts...)
    Texture texPlayer = LoadTexture("resources/player.png");
    Rectangle sourceRec = { 6, 2, 26, 31 };
    int playerScore = 0;

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // 
    //Reproductor Musica
    PlaySound(song);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired

        

        if (IsKeyPressed(KEY_SPACE)) currentMode = !currentMode; // Toggle mode: 0-Game, 1-Editor

        if (currentMode == 0) // Game mode
        {
            // TODO: [2p] Player 2D movement from predefined Start-point to End-point
            // Implement maze 2D player movement logic (cursors || WASD)
            
            float velocity = 1 * MAZE_SCALE;
            Vector2 direction = { 0 };

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) direction = (Vector2) { velocity, 0 };
            else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) direction = (Vector2){ -velocity, 0 };
            else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) direction = (Vector2){ 0, velocity };
            else if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) direction = (Vector2){ 0, -velocity };
            
            // Use imMaze pixel information to check collisions

            if (!ColorIsEqual(GetImageColor(imMaze,
                (int)((player.x + direction.x - position.x) / MAZE_SCALE),
                (int)((player.y + direction.y - position.y) / MAZE_SCALE)), WHITE))
            {
                player.x += direction.x;
                player.y += direction.y;
            }

            // Detect if current playerCell == endCell to finish game

            if (player.x == endCell.x && player.y == endCell.y)
            {
                // TODO: Finish game
                CloseWindow();
            }

            // TODO: [1p] Camera 2D system following player movement around the map
            // Update Camera2D parameters as required to follow player and zoom control
            // Update camera target position with new player position
            camera.target = (Vector2){ player.x + 2, player.y + 2 };
            camera.zoom += ((float)GetMouseWheelMove()*0.05f);

            // TODO: [2p] Maze items pickup logic
            if (ColorIsEqual(GetImageColor(imMaze, player.x, player.y), RED)) {
                playerScore += 1;
                PlaySound(sfx);
                /*for (int i = 0; i < MAX_MAZE_ITEMS; i++) {
                    Point Item = mazeItems[i];
                    if (Item.x == player.x && Item.y == player.y) {
                        mazeItemPicked[i] = false;
                    }
                }*/
            }
        }
        else if (currentMode == 1) // Editor mode
        {
            if (IsKeyPressed(KEY_R))
            {
                seed += GetRandomValue(1, 99);
                UnloadImage(imMaze);
                UnloadTexture(texMaze);
                imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.5f);
                texMaze = LoadTextureFromImage(imMaze);
            }

            // TODO: [2p] Maze editor mode, edit image pixels with mouse.
            
            // Implement logic to select image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels

            // WARNING: Remember that when imMaze changes, texMaze must be also updated!

            // Set Walkable Position
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Vector2 mousePos = GetMousePosition();

                if ((mousePos.x >= position.x) && (mousePos.y >= position.y))
                {
                    Point mapCoord = {
                        (int)((mousePos.x - position.x) / MAZE_SCALE),
                        (int)((mousePos.y - position.y) / MAZE_SCALE),
                    };

                    ImageDrawPixel(&imMaze, mapCoord.x, mapCoord.y, BLACK);

                    UnloadTexture(texMaze);
                    texMaze = LoadTextureFromImage(imMaze);
                }
            }


            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                // Set End-Point Position
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    Vector2 mousePos = GetMousePosition();

                    if ((mousePos.x >= position.x) && (mousePos.y >= position.y))
                    {
                        Point mapCoord = {
                            (int)((mousePos.x - position.x) / MAZE_SCALE),
                            (int)((mousePos.y - position.y) / MAZE_SCALE),
                        };

                        ImageDrawPixel(&imMaze, mapCoord.x, mapCoord.y, GREEN);

                        UnloadTexture(texMaze);
                        texMaze = LoadTextureFromImage(imMaze);
                    }
                }
                // Set Wall Position
                else
                {
                    Vector2 mousePos = GetMousePosition();

                    if ((mousePos.x >= position.x) && (mousePos.y >= position.y))
                    {
                        Point mapCoord = {
                            (int)((mousePos.x - position.x) / MAZE_SCALE),
                            (int)((mousePos.y - position.y) / MAZE_SCALE),
                        };

                        ImageDrawPixel(&imMaze, mapCoord.x, mapCoord.y, WHITE);

                        UnloadTexture(texMaze);
                        texMaze = LoadTextureFromImage(imMaze);
                    }
                }
            }

            // TODO: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]
            //if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
            if (IsKeyPressed(KEY_ENTER))
            {
                Vector2 mousePos = GetMousePosition();

                if ((mousePos.x >= position.x) && (mousePos.y >= position.y))
                { 
                    Point mapCoord = {
                        (int)((mousePos.x - position.x) / MAZE_SCALE),
                        (int)((mousePos.y - position.y) / MAZE_SCALE),
                    };

                    for (int i = 0; i < MAX_MAZE_ITEMS; i++) {
                        if (mazeItems[i].x == 0 && mazeItems[i].y) {
                            mazeItems[i] = mapCoord;
                            mazeItemPicked[i] = false;
                            ImageDrawPixel(&imMaze, mapCoord.x, mapCoord.y, RED);

                            UnloadTexture(texMaze);
                            texMaze = LoadTextureFromImage(imMaze);
                            break;
                        }
                    }
                }
            }
        }

        // TODO: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  
        if (IsKeyPressed(KEY_ONE)) currentBiome = 0;
        if (IsKeyPressed(KEY_TWO)) currentBiome = 1;

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (currentMode == 0) // Game mode
            {
                // Draw maze using camera2d (for automatic positioning and scale)
                BeginMode2D(camera);

                // TODO: Draw maze walls and floor using current texture biome 
                for (int y = 0; y < imMaze.height; y++)
                {
                    for (int x = 0; x < imMaze.width; x++)
                    {
                        if (ColorIsEqual(GetImageColor(imMaze, x, y), WHITE))
                        {
                            DrawTexturePro(texBiomes[currentBiome],
                                (Rectangle) {
                                0 * (texBiomes[currentBiome].width / 2), 0 * (texBiomes[currentBiome].height / 2), texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2
                            },
                                (Rectangle) {
                                position.x + x * MAZE_SCALE, position.y + y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE
                            },
                                (Vector2) {
                                0
                            },
                                0.0f, WHITE);
                        }
                        else if (ColorIsEqual(GetImageColor(imMaze, x, y), BLACK))
                        {
                            DrawTexturePro(texBiomes[currentBiome],
                                (Rectangle) {
                                1 * (texBiomes[currentBiome].width / 2), 0 * (texBiomes[currentBiome].height / 2), texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2
                            },
                                (Rectangle) {
                                position.x + x * MAZE_SCALE, position.y + y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE
                            },
                                (Vector2) {
                                0
                            },
                                0.0f, WHITE);
                        }
                        else if (ColorIsEqual(GetImageColor(imMaze, x, y), RED))
                        {
                            DrawTexturePro(texBiomes[currentBiome],
                                (Rectangle) {
                                0 * (texBiomes[currentBiome].width / 2), 1 * (texBiomes[currentBiome].height / 2), texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2
                            },
                                (Rectangle) {
                                position.x + x * MAZE_SCALE, position.y + y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE
                            },
                                (Vector2) {
                                0
                            },
                                0.0f, WHITE);
                        }
                        else if (ColorIsEqual(GetImageColor(imMaze, x, y), GREEN))
                        {
                            DrawTexturePro(texBiomes[currentBiome],
                                (Rectangle) {
                                1 * (texBiomes[currentBiome].width / 2), 1 * (texBiomes[currentBiome].height / 2), texBiomes[currentBiome].width / 2, texBiomes[currentBiome].height / 2
                            },
                                (Rectangle) {
                                position.x + x * MAZE_SCALE, position.y + y * MAZE_SCALE, MAZE_SCALE, MAZE_SCALE
                            },
                                (Vector2) {
                                0
                            },
                                0.0f, WHITE);
                        }
                    }
                }

                // TODO: Draw player rectangle or sprite at player position
                
                //DrawTextureEx(texPlayer, (Vector2) { player.x, player.y }, 0.0f, 1.0f, WHITE);

                DrawTexturePro(texPlayer, sourceRec,
                    (Rectangle) { player.x, player.y, MAZE_SCALE, MAZE_SCALE },
                    (Vector2) { 0 }, 0.0f, WHITE);
                
                // TODO: Draw maze items 2d (using sprite texture?)
                        
                for (int i = 0; i < MAX_MAZE_ITEMS; i++) {
                    Point Item = mazeItems[i];
                    ImageDrawPixel(&imMaze, Item.x, Item.y, RED);
                }
                    
                EndMode2D();

                // TODO: Draw game UI (score, time...) using custom sprites/fonts
                // NOTE: Game UI does not receive the camera2d transformations,
                // it is drawn in screen space coordinates directly

                DrawText(TextFormat("SCORE: %i", playerScore), 10, 76, 10, RED);
                DrawText("[SPACE] TOGGLE MODE: EDITOR/GAME", 10, GetScreenHeight() - 20, 10, WHITE);
                
                DrawFPS(10, 10);

            }
            else if (currentMode == 1) // Editor mode
            {
                // Draw generated maze texture, scaled and centered on screen 
                DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

                // Draw lines rectangle over texture, scaled and centered on screen 
                DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH*MAZE_SCALE, MAZE_HEIGHT*MAZE_SCALE, RED);

                // TODO: Draw player using a rectangle, consider maze screen coordinates!
                DrawRectangle(player.x, player.y, player.width * MAZE_SCALE, player.height * MAZE_SCALE, RED);

                // TODO: Draw editor UI required elements
                DrawText("[R] GENERATE NEW RANDOM SEQUENCE", 10, 36, 10, LIGHTGRAY);
                DrawText(TextFormat("SEED: %i", seed), 10, 56, 10, YELLOW);
                DrawText("[SPACE] TOGGLE MODE: EDITOR/GAME", 10, GetScreenHeight() - 20, 10, WHITE);
            }

            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texMaze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(imMaze);        // Unload maze image from RAM (CPU)
    UnloadSound(sfx);
    UnloadSound(song);

    // TODO: Unload all loaded resources
    
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    //Image imMaze = { 0 };
    
    // TODO: [1p] Implement maze image generation algorithm
    Image imMaze = GenImageColor(width, height, BLACK);

    // TODO: Generate procedural maze image, using grid-based algorithm
    // NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable
    // TODO 1.2: Draw image border
    for (int y = 0; y < imMaze.height; y++)
    {
        for (int x = 0; x < imMaze.width; x++)
        {
            if ((x == 0) || (x == (imMaze.width - 1)) ||
                (y == 0) || (y == (imMaze.height - 1)))
            {
                ImageDrawPixel(&imMaze, x, y, WHITE);
            }
        }
    }

    // STEP 2: Set some random point in image at specific row-column distances

    // TODO 2.1: Define an array of point used for maze generation
    // NOTE A: Static array allocation, memory allocated in STACK (MAX: 1MB)
    // NOTE B: Dynamic array allocation, memory allocated in HEAP (MAX: Available RAM)
   
	//Point mazePoints[64] = { 0 }; //Static array 
	Point* mazePoints = malloc(256 * sizeof(Point)); //Dynamic array
    int mazePointCounter = 0;
 
    // TODO 2.2: Store specific points, at specific row-column distances
    for (int y = 0; y < imMaze.height; y++)
    {
        for (int x = 0; x < imMaze.width; x++)
        {
            if ((x == 0) || (x == (imMaze.width - 1)) ||
                (y == 0) || (y == (imMaze.height - 1)))
            {
                ImageDrawPixel(&imMaze, x, y, WHITE);
            }
            else
            {
                if ((x % 4 == 0) && (y % 4 == 0))
                {
                    if (GetRandomValue(0, 100) <= 80) {
                        mazePoints[mazePointCounter] = (Point){ x, y };
                        mazePointCounter++;
                    }
                }
            }
        }
    }

    // TODO 2.3: Draw our points in image
    for (int i = 0; i < mazePointCounter; i++) {
        ImageDrawPixel(&imMaze, mazePoints[i].x, mazePoints[i].y, WHITE);
    }

    // STEP 3: Draw lines from every point in a random direction

    // TODO 3.1: Define an array of 4 directions for convenience
    Point directions[4] = {
        { 0, -1 }, // North
        { 1, 0 },  // East
        { 0, 1 },  // South
        { -1, 0 }  // West
    };

    // TODO 3.2: Load a random sequence of points, to be used as indices, so,
    // we can access maze-points randomly indexed, instead of following the order we stored them
    int* pointOrder = LoadRandomSequence(mazePointCounter, 0, mazePointCounter - 1);

    // TODO 3.3: Process every random maze point, drawing pixels in one random direction,
    // until we collision with another wall (WHITE pixel)
    for (int i = 0; i < mazePointCounter; i++)
    {
        Point currentPoint = mazePoints[pointOrder[i]];
        Point currentDir = directions[GetRandomValue(0, 3)];
        Point nextPoint = { currentPoint.x + currentDir.x, currentPoint.y + currentDir.y };

        while (ColorIsEqual(GetImageColor(imMaze, nextPoint.x, nextPoint.y), BLACK))
        {
            ImageDrawPixel(&imMaze, nextPoint.x, nextPoint.y, WHITE);
            nextPoint.x += currentDir.x;
            nextPoint.y += currentDir.y;
        }
    }
    
    return imMaze;
}
