#include "raylib.h"
#include "raymath.h"
#include <array>

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int screenWidth = 1600;
constexpr int screenHeight = 1600;

enum class TextureAsset { Tilemap };
enum class SoundAsset { FootGrass, FootStone, Attack, Death, Coins };
enum class MusicAsset { LightAmbience, DarkAmbience };
enum class Zone { All, World, Dungeon };
enum class TileType { Dirt, Grass, Tree };

struct Tile {
    int x;
    int y;
    TileType type;
};

struct Entity {
    int x;
    int y;
    Zone zone;
    bool isAlive;
    bool isPassable;
    int health;
    int damage;
    int money;
    int experience;
};

class Timer {
public:
    double startTime;
    double lifeTime;
    bool isActive = false;

    void Start(double lifetime) {
        startTime = GetTime();
        lifeTime = lifetime;
        isActive = true;
    }

    bool IsDone() const {
        return GetTime() - startTime >= lifeTime;
    }

    double GetElapsed() const {
        return GetTime() - startTime;
    }
};

class Game {
private:
    static constexpr int MAX_TEXTURES = 1;
    static constexpr int MAX_SOUNDS = 5;
    static constexpr int MAX_MUSIC = 2;
    static constexpr int WORLD_WIDTH = 30;
    static constexpr int WORLD_HEIGHT = 30;

    std::array<Texture2D, MAX_TEXTURES> textures;
    std::array<Sound, MAX_SOUNDS> sounds;
    std::array<Music, MAX_MUSIC> music;

    Tile world[WORLD_WIDTH][WORLD_HEIGHT];
    Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
    Camera2D camera;

    Entity player;
    Entity dungeon_gate;
    Entity orc;
    Entity chest;

    Timer combatTextTimer;

public:
    Game() {
        camera.target = { 0, 0 };
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 3.0f;
    }

    void Startup() {
// enable sound output
        InitAudioDevice();

        Image image = LoadImage("assets/bit_packed.png");
        textures[static_cast<int>(TextureAsset::Tilemap)] = LoadTextureFromImage(image);
        UnloadImage(image);

        for (int i = 0; i < WORLD_WIDTH; ++i) {
            for (int j = 0; j < WORLD_HEIGHT; ++j) {
                world[i][j] = { i, j, static_cast<TileType>(GetRandomValue(0, 2)) };
                dungeon[i][j] = { i, j, TileType::Dirt };
            }
        }

        player = {
            24 * TILE_WIDTH, 1 * TILE_HEIGHT, Zone::World, true,false, 100, 0, 1000, 0
        };

        dungeon_gate = { 
            10 * TILE_WIDTH, 10 * TILE_HEIGHT, Zone::All, false,true, 0,0,0,0
        };

        orc = {
            5 * TILE_WIDTH, 5 * TILE_HEIGHT, Zone::Dungeon,true, false, 100, 0, 0, GetRandomValue(10, 100)
        };

        chest = { 0 };

        camera.target = { static_cast<float>(player.x), static_cast<float>(player.y) };

        sounds[static_cast<int>(SoundAsset::FootGrass)] = LoadSound("assets/Grass1.wav");
        sounds[static_cast<int>(SoundAsset::FootStone)] = LoadSound("assets/Concrete1.wav");
        sounds[static_cast<int>(SoundAsset::Attack)] = LoadSound("assets/07_human_atk_sword_2.wav");
        sounds[static_cast<int>(SoundAsset::Death)] = LoadSound("assets/24_orc_death_spin.wav");
        sounds[static_cast<int>(SoundAsset::Coins)] = LoadSound("assets/handleCoins.ogg");

        music[static_cast<int>(MusicAsset::LightAmbience)] = LoadMusicStream("assets/light-ambience.mp3");
        music[static_cast<int>(MusicAsset::DarkAmbience)] = LoadMusicStream("assets/dark-ambience.mp3");

        PlayMusicStream(music[static_cast<int>(MusicAsset::LightAmbience)]);
    }

    void Update() {
        if (player.zone == Zone::World) {
            UpdateMusicStream(music[static_cast<int>(MusicAsset::LightAmbience)]);
        }
        else if (player.zone == Zone::Dungeon) {
            UpdateMusicStream(music[static_cast<int>(MusicAsset::DarkAmbience)]);
        }

        float x = player.x;
        float y = player.y;
        bool hasKeyBeenPressed = false;

        if (IsKeyPressed(KEY_LEFT)) { x -= TILE_WIDTH; hasKeyBeenPressed = true; }
        if (IsKeyPressed(KEY_RIGHT)) { x += TILE_WIDTH; hasKeyBeenPressed = true; }
        if (IsKeyPressed(KEY_UP)) { y -= TILE_HEIGHT; hasKeyBeenPressed = true; }
        if (IsKeyPressed(KEY_DOWN)) { y += TILE_HEIGHT; hasKeyBeenPressed = true; }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            camera.zoom += wheel * 0.125f;
            camera.zoom = Clamp(camera.zoom, 3.0f, 8.0f);
        }

        if (player.zone == orc.zone && orc.isAlive && orc.x == x && orc.y == y) {
            int damage = GetRandomValue(2, 20);
            orc.health -= damage;
            orc.damage = damage;

            if (!combatTextTimer.isActive) {
                combatTextTimer.Start(0.50);
            }

            if (orc.health <= 0) {
                PlaySound(sounds[static_cast<int>(SoundAsset::Death)]);
                orc.isAlive = false;
                player.experience += orc.experience;

                chest = { orc.x, orc.y, orc.zone, true, true, 0, 0, GetRandomValue(10, 100), 0 };
            }
            else {
                PlaySound(sounds[static_cast<int>(SoundAsset::Attack)]);
            }
        }
        else {
            if (hasKeyBeenPressed) {
                SoundAsset sound = (player.zone == Zone::World) ? SoundAsset::FootGrass : SoundAsset::FootStone;
                PlaySound(sounds[static_cast<int>(sound)]);
            }
            player.x = x;
            player.y = y;
            camera.target = { static_cast<float>(player.x), static_cast<float>(player.y) };
        }

        if (IsKeyPressed(KEY_E) && player.x == dungeon_gate.x && player.y == dungeon_gate.y) {
            if (player.zone == Zone::World) {
                player.zone = Zone::Dungeon;
                StopMusicStream(music[static_cast<int>(MusicAsset::LightAmbience)]);
                PlayMusicStream(music[static_cast<int>(MusicAsset::DarkAmbience)]);
            }
            else {
                player.zone = Zone::World;
                StopMusicStream(music[static_cast<int>(MusicAsset::DarkAmbience)]);
                PlayMusicStream(music[static_cast<int>(MusicAsset::LightAmbience)]);
            }
        }

        if (IsKeyPressed(KEY_G) && player.x == chest.x && player.y == chest.y && chest.isAlive) {
            player.money += chest.money;
            PlaySound(sounds[static_cast<int>(SoundAsset::Coins)]);
            chest.isAlive = false;
        }

        if (combatTextTimer.IsDone()) {
            combatTextTimer.isActive = false;
        }
    }

    void Render() {
        BeginMode2D(camera);

        for (int i = 0; i < WORLD_WIDTH; ++i) {
            for (int j = 0; j < WORLD_HEIGHT; ++j) {
                Tile tile = (player.zone == Zone::World) ? world[i][j] : dungeon[i][j];
                int tx = 4, ty = 4;

                switch (tile.type) {
                case TileType::Grass: tx = 5; ty = 4; break;
                case TileType::Tree:  tx = 5; ty = 5; break;
                default: break;
                }
                DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, tx, ty);
            }
        }

        DrawTile(dungeon_gate.x, dungeon_gate.y, 8, 9);

        if (orc.zone == player.zone) {
            if (orc.isAlive) DrawTile(orc.x, orc.y, 11, 0);
            if (combatTextTimer.isActive) {
                DrawText(TextFormat("%d", orc.damage), orc.x, orc.y - 10, 9, YELLOW);
            }
            if (chest.isAlive) DrawTile(chest.x, chest.y, 9, 3);
        }

        DrawTile(player.x, player.y, 24, 0);
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
        for (auto& sound : sounds) UnloadSound(sound);
        for (auto& m : music) { StopMusicStream(m); UnloadMusicStream(m); }
        CloseAudioDevice();
    }

private:
    void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y) {
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
        ClearBackground(GRAY);
        game.Render();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}