#include <array>

#include "raylib.h"
#include "raymath.h"

#include "main.hpp"

class Game {
private:
    static constexpr int MAX_TEXTURES = 1;
    static constexpr int WORLD_WIDTH = 25;
    static constexpr int WORLD_HEIGHT = 20;

    std::array<Texture2D, MAX_TEXTURES> textures;

// world stored in this 2D arry
    Tile world[WORLD_WIDTH][WORLD_HEIGHT];
    Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
    Camera2D camera;

    Entity player;
    Entity dungeon_gate;
    Entity orc;
    Entity chest;

    Timer combatTextTimer;
    Timer playerTimer;

    Audio audio;
    int player_sprite_toggle = 0;
    int bound = 8;


    std::vector<Projectile> projectiles;
    const float projectileSpeed = 5.0f;
    const float projectileRadius = 2.0f;


public:
    Game() {
        camera.target = { 0, 0 };
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 3.7f;
    }

    void Startup() {
    // enable sound output
        InitAudioDevice();
        audio = Audio();
        Image image = LoadImage("assets/bit_packed.png");
        textures[static_cast<int>(TextureAsset::Tilemap)] = LoadTextureFromImage(image);
        UnloadImage(image);

        // randomly pick tiles in world
        for (int i = 0; i < WORLD_WIDTH; ++i) {
            for (int j = 0; j < WORLD_HEIGHT; ++j) {
                // world[i][j] = { i, j, static_cast<TileType>(GetRandomValue(0, 2)) };
                world[i][j] = { i, j, static_cast<TileType>(1)};
                dungeon[i][j] = { i, j, TileType::Dirt };
            }
        }

        player = {
            12 * TILE_WIDTH, 6 * TILE_HEIGHT, Zone::World, true,false, 100, 0, 1000, 0
        };

        dungeon_gate = { 
            10 * TILE_WIDTH, 10 * TILE_HEIGHT, Zone::All, false,true, 0,0,0,0
        };

        orc = {
            5 * TILE_WIDTH, 5 * TILE_HEIGHT, Zone::Dungeon,true, false, 100, 0, 0, GetRandomValue(10, 100)
        };

        chest = { 0 };

        TraceLog(LOG_INFO, "Application started!--------------------------------------------------------------------------------------------------------");
        audio.play_music(MusicAsset::LightAmbience); 
    }

    void Update() {
        if (player.zone == Zone::World) {
            audio.update_music(MusicAsset::LightAmbience);
        }
        else if (player.zone == Zone::Dungeon) {
            audio.update_music(MusicAsset::DarkAmbience);
        }

        float x = player.x;
        float y = player.y;

        Vector2 mouseScreen = GetMousePosition();
        Vector2 mousePos = GetScreenToWorld2D(mouseScreen, camera);

        // Shoot when left mouse button is pressed

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // PlaySound(sounds[static_cast<int>(SoundAsset::Laser)]);
            audio.play_sound(SoundAsset::Laser);
            //TraceLog(LOG_INFO, "mouse pressed x=%f, y=%f", mousePos.x, mousePos.y);
            //TraceLog(LOG_INFO, "1. Position: x=%f, y=%f", x, y);
            Vector2 direction = Vector2Normalize(Vector2Subtract(mousePos, Vector2{x+8,y+8}));

            // Create new projectile
            Projectile newProjectile = {
                Vector2{x+8,y+8},
                direction,
                projectileSpeed,
                true
            };

            // Add to projectiles list
            projectiles.push_back(newProjectile);
        }

        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const Projectile& p) { return !p.active; }), projectiles.end());
        //remove_projectiles()


        bool hasKeyBeenPressed = false;

        if (IsKeyPressed(KEY_LEFT))
        { 
            x -= TILE_WIDTH; 
            hasKeyBeenPressed = true; 
        }
        if (IsKeyPressed(KEY_RIGHT))
        { 
            x += TILE_WIDTH; 
            hasKeyBeenPressed = true; 
        }
        if (IsKeyPressed(KEY_UP)) { y -= TILE_HEIGHT; hasKeyBeenPressed = true; }
        if (IsKeyPressed(KEY_DOWN)) { y += TILE_HEIGHT; hasKeyBeenPressed = true; }

//       float wheel = GetMouseWheelMove();
//        if (wheel != 0) {
//            camera.zoom += wheel * 0.125f;
//            camera.zoom = Clamp(camera.zoom, 3.0f, 8.0f);
//        }

        int wx = x / TILE_WIDTH;
        int wy = y / TILE_HEIGHT;

        Tile target_tile = (player.zone == Zone::World) ? world[wx][wy] : dungeon[wx][wy];

        // TraceLog(LOG_INFO, "1. Position: x=%f, y=%f", x, y);
        if (target_tile.type == TileType::Boundary) {
            // TraceLog(LOG_INFO, "1. Position: x=%f, y=%f", x, y);
            x = player.x;
            y = player.y;
        }

        if (player.zone == orc.zone && orc.isAlive && orc.x == x && orc.y == y) {
            int damage = GetRandomValue(2, 20);
            orc.health -= damage;
            orc.damage = damage;

            if (!combatTextTimer.isActive) {
                combatTextTimer.Start(0.50);
            }

            if (orc.health <= 0) {
                audio.play_sound(SoundAsset::Death);
                orc.isAlive = false;
                player.experience += orc.experience;

                chest = { orc.x, orc.y, orc.zone, true, true, 0, 0, GetRandomValue(10, 100), 0 };
            }
            else {
                audio.play_sound(SoundAsset::Attack);
            }
        }
        else {
            if (hasKeyBeenPressed) {
                SoundAsset sound = (player.zone == Zone::World) ? SoundAsset::FootGrass : SoundAsset::FootStone;
                audio.play_sound(sound);
            }
            player.x = x;
            player.y = y;
            int camera_x = x;
            int camera_y = y;
            camera.target = { static_cast<float>(player.x), static_cast<float>(player.y) };
            //std::cout << "player x" << player.x << std::endl;
            
            // variable camera control
            if (player.x < bound * TILE_WIDTH) {
                camera_x = bound * TILE_WIDTH;
            }
            else if (player.x > (TILE_WIDTH * (WORLD_WIDTH - bound))) {
                camera_x = (WORLD_WIDTH - bound) * TILE_WIDTH;
            }
            if (player.y < bound * TILE_HEIGHT) {
                camera_y = bound * TILE_HEIGHT;
            }
            else if (player.y > (TILE_HEIGHT * (WORLD_HEIGHT - bound))) {
                camera_y = (WORLD_HEIGHT - bound) * TILE_HEIGHT;
            }
            camera.target = { static_cast<float>(camera_x), static_cast<float>(camera_y) };

            /* else if (player.y < 2) {
                camera.target = { static_cast<float>(player.x), static_cast<float>(3)};
            }
            else {
                camera.target = { static_cast<float>(player.x), static_cast<float>(player.y) };
            }*/
        }

        if (IsKeyPressed(KEY_E) && player.x == dungeon_gate.x && player.y == dungeon_gate.y) {
            if (player.zone == Zone::World) {
                player.zone = Zone::Dungeon;
                audio.stop_music(MusicAsset::LightAmbience);
                audio.play_music(MusicAsset::DarkAmbience);
            }
            else {
                player.zone = Zone::World;
                audio.stop_music(MusicAsset::DarkAmbience);
                audio.play_music(MusicAsset::LightAmbience);
            }
        }

        if (IsKeyPressed(KEY_G) && player.x == chest.x && player.y == chest.y && chest.isAlive) {
            player.money += chest.money;
            audio.play_sound(SoundAsset::Coins);
            chest.isAlive = false;
        }

        if (combatTextTimer.IsDone()) {
            combatTextTimer.isActive = false;
        }

        // Update projectiles
        for (auto& projectile : projectiles) {
            update_projectile(projectile);
        }
        collisions(projectiles);
        
    }

    void Render() {
        BeginMode2D(camera);

        for (int i = 0; i < WORLD_WIDTH; ++i) {
            for (int j = 0; j < WORLD_HEIGHT; ++j) {
                Tile tile = (player.zone == Zone::World) ? world[i][j] : dungeon[i][j];
                int tx = 4, ty = 4;

                switch (tile.type) {
                    case TileType::Grass: tx = 6; ty = 0; break;
                    case TileType::Tree:  tx = 5; ty = 2; break;
                    case TileType::Dirt:  tx = 2; ty = 0; break;
                default: break;
                    
                }
                if (i < 3 || j < 3 || i >(WORLD_WIDTH - 4) || j >(WORLD_HEIGHT - 4)) {
                    world[i][j] = Tile{ i, j, TileType::Boundary };
                    tx = 0; ty = 13;
                }
                DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, tx, ty);
            }
        }

        DrawTile(dungeon_gate.x, dungeon_gate.y, 3, 3);

        if (orc.zone == player.zone) {
            if (orc.isAlive) DrawTile(orc.x, orc.y, 11, 0);
            if (combatTextTimer.isActive){
                DrawText(TextFormat("%d", orc.damage), orc.x, orc.y - 10, 9, YELLOW);
            }
            if (chest.isAlive) DrawTile(chest.x, chest.y, 9, 3);
        }


        if (!playerTimer.isActive) {
            playerTimer.Start(0.3);
        }

        if (playerTimer.IsDone()) {
            playerTimer.isActive = false;
            player_sprite_toggle ^= 1;
        }

        if (player_sprite_toggle == 0){
            DrawTile(player.x, player.y, 19, 8);
        }
        else{
            DrawTile(player.x, player.y, 22, 8);
        }

        // Draw projectiles
        for (const auto& projectile : projectiles) {
            if (projectile.active) {
                DrawCircleV(projectile.position, projectileRadius, RED);
            }
        }
        DrawCircleV(Vector2{80,80}, 5.0f, RED);

        EndMode2D();

        DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(5, 5, 330, 120, BLUE);

        DrawText(TextFormat("Camera Target: (%06.2f, %06.2f", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
        DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
        DrawText(TextFormat("Player Health: %d", player.health), 15, 50, 14, YELLOW);
        DrawText(TextFormat("Player XP: %d", player.experience), 15, 70, 14, YELLOW);
        DrawText(TextFormat("Player Money: %d", player.money), 15, 90, 14, YELLOW);
        if (orc.isAlive) DrawText(TextFormat("Orc Health: %d", orc.health), 15, 110, 14, YELLOW);
    }

    void Shutdown() {
        for (auto& texture : textures) UnloadTexture(texture);
        audio.shutdown_audio();
        CloseAudioDevice();
        
    }

private:
    void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0) {
        Rectangle source = {
            static_cast<float>(TILE_WIDTH * texture_index_x),
            static_cast<float>(TILE_HEIGHT * texture_index_y),
            static_cast<float>(TILE_WIDTH),
            static_cast<float>(TILE_HEIGHT)
        };
        Rectangle dest = {
            static_cast<float>(pos_x),
            static_cast<float>(pos_y),
            static_cast<float>(TILE_WIDTH),
            static_cast<float>(TILE_HEIGHT)
        };
        DrawTexturePro(textures[static_cast<int>(TextureAsset::Tilemap)], source, dest, { 0,0 }, 0, WHITE);
    }
};

int main() {
    InitWindow(screenWidth, screenHeight, "Raylib 2D RPG");
    SetTargetFPS(60);

    Game game;
    game.Startup();

    while (!WindowShouldClose()) {
        game.Update();
        BeginDrawing();
        ClearBackground(BLACK);
        game.Render();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}