#include "main.hpp"

Game::Game() : enemy(100, 7, 5 * TILE_WIDTH, 5 * TILE_HEIGHT), protagonist(100, 7, 5 * TILE_WIDTH, 5 * TILE_HEIGHT, Zone::World)
{
    camera.target = { 0, 0 };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 3.7f;
}

// load assets and initialize game state
void Game::Startup() {
    
// enable sound output
    InitAudioDevice();
    audio = Audio();
    Image image = LoadImage("assets/bit_packed.png");
    textures[static_cast<int>(TextureAsset::Tilemap)] = LoadTextureFromImage(image);
    image = LoadImage("assets/player_sprite/2_walk.png");
    textures[static_cast<int>(TextureAsset::Player)] = LoadTextureFromImage(image);   
    image = LoadImage("assets/dungeon_pack/tileset/Dungeon_Tileset.png");
    textures[static_cast<int>(TextureAsset::Dungeon)] = LoadTextureFromImage(image);


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
    Player protagonist = Player(100, 7, 5 * TILE_WIDTH, 5 * TILE_HEIGHT, Zone::World);

    dungeon_gate = { 
        10 * TILE_WIDTH, 10 * TILE_HEIGHT, Zone::All, false,true, 0,0,0,0
    };
    enemy = Mob(100, 7, 5 * TILE_WIDTH, 5 * TILE_HEIGHT );

    chest = { 0 };

    TraceLog(LOG_INFO, "Application started!--------------------------------------------------------------------------------------------------------");
    audio.play_music(MusicAsset::LightAmbience); 
}

void Game::Update() {
    if (protagonist.zone == Zone::World) {
        audio.update_music(MusicAsset::LightAmbience);
    }
    else if (protagonist.zone == Zone::Dungeon) {
        audio.update_music(MusicAsset::DarkAmbience);
    }

    float x = protagonist.x;
    float y = protagonist.y;

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
        player_projectiles.push_back(newProjectile);
    }
    // Remove unused projectiles
    remove_projectiles(player_projectiles);
    remove_projectiles(enemy_projectiles);


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

    int wx = x / TILE_WIDTH;
    int wy = y / TILE_HEIGHT;

    Tile target_tile = (protagonist.zone == Zone::World) ? world[wx][wy] : dungeon[wx][wy];

    // do not allow players to move to pass the boundary
    if (target_tile.type == TileType::Boundary) {
        // TraceLog(LOG_INFO, "1. Position: x=%f, y=%f", x, y);
        x = protagonist.x;
        y = protagonist.y;
    }
    if (enemy.isAlive) 
    {
        if (!combatTextTimer.isActive) {
            enemy.random_move();
            //TraceLog(LOG_INFO, "2. Position: x=%f, y=%f", enemy.x, enemy.y);
            enemy_projectiles.push_back(enemy.attack(protagonist.x, protagonist.y));
            combatTextTimer.Start(0.50);
        }

        else if (enemy.health <= 0)
        {
            audio.play_sound(SoundAsset::Death);
            enemy.isAlive = false;
        }
    }

    
    if (hasKeyBeenPressed) {
        SoundAsset sound = (protagonist.zone == Zone::World) ? SoundAsset::FootGrass : SoundAsset::FootStone;
        audio.play_sound(sound);
    }
    
    protagonist.x = x;
    protagonist.y = y;
    int camera_x = x;
    int camera_y = y;
    camera.target = { x, y };   
    //std::cout << "player x" << protagonist.x << std::endl;
    
    // variable camera control
    if (protagonist.x < bound * TILE_WIDTH) {
        camera_x = bound * TILE_WIDTH;
    }
    else if (protagonist.x > (TILE_WIDTH * (WORLD_WIDTH - bound))) {
        camera_x = (WORLD_WIDTH - bound) * TILE_WIDTH;
    }
    if (protagonist.y < bound * TILE_HEIGHT) {
        camera_y = bound * TILE_HEIGHT;
    }
    else if (protagonist.y > (TILE_HEIGHT * (WORLD_HEIGHT - bound))) {
        camera_y = (WORLD_HEIGHT - bound) * TILE_HEIGHT;
    }
    camera.target = { static_cast<float>(camera_x), static_cast<float>(camera_y) };

    /* else if (protagonist.y < 2) {
        camera.target = { static_cast<float>(protagonist.x), static_cast<float>(3)};
    }
    else {
        camera.target = { static_cast<float>(protagonist.x), static_cast<float>(protagonist.y) };
    }*/


    if (IsKeyPressed(KEY_E) && protagonist.x == dungeon_gate.x && protagonist.y == dungeon_gate.y) {
        if (protagonist.zone == Zone::World) {
            protagonist.zone = Zone::Dungeon;
            audio.stop_music(MusicAsset::LightAmbience);
            audio.play_music(MusicAsset::DarkAmbience);
        }
        else {
            protagonist.zone = Zone::World;
            audio.stop_music(MusicAsset::DarkAmbience);
            audio.play_music(MusicAsset::LightAmbience);
        }
    }

    if (IsKeyPressed(KEY_G) && protagonist.x == chest.x && protagonist.y == chest.y && chest.isAlive) {
        protagonist.money += chest.money;
        audio.play_sound(SoundAsset::Coins);
        chest.isAlive = false;
    }

    if (combatTextTimer.IsDone()) {
        combatTextTimer.isActive = false;
    }

    // Update projectiles
    for (auto& projectile : player_projectiles) {
        update_projectile(projectile);
    }
    for (auto& projectile : enemy_projectiles) {
        update_projectile(projectile);
    }
    
    collisions(player_projectiles, enemy);
    //collisions(enemy_projectiles, player);
}

void Game::Render() {
    BeginMode2D(camera);

    for (int i = 0; i < WORLD_WIDTH; ++i) {
        for (int j = 0; j < WORLD_HEIGHT; ++j) {
            Tile tile = (protagonist.zone == Zone::World) ? world[i][j] : dungeon[i][j];
            int tx = 4, ty = 4;

            switch (tile.type) {
                case TileType::Grass: tx = 2; ty = 2; break;
                case TileType::Tree:  tx = 3; ty = 2; break;
                case TileType::Dirt:  tx = 3; ty = 3; break;
            default: break;
                
            }
            // set boundary on map
            if (i < 1 || j < 1 || i >(WORLD_WIDTH - 2) || j >(WORLD_HEIGHT - 2)) {
                world[i][j] = Tile{ i, j, TileType::Boundary };
                tx = 0; ty = 1;
            }
            if (j < 1){
                world[i][j] = Tile{ i, j, TileType::Boundary };
                tx = 1; ty = 0;
            }
            if (j >(WORLD_HEIGHT - 2)) {
                world[i][j] = Tile{ i, j, TileType::Boundary };
                tx = 1; ty = 4;
            }
            DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, tx, ty);
            //DrawTile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, 0, 1);
        }
    }

    DrawTile(dungeon_gate.x, dungeon_gate.y, 9, 4);

    //DrawText(TextFormat("%d", orc.damage), orc.x, orc.y - 10, 9, YELLOW);

    if (enemy.isAlive) {
        DrawTile(enemy.getX(), enemy.getY(), 0, 9);
    }


    if (!playerTimer.isActive) {
        playerTimer.Start(0.3);
    }

    if (playerTimer.IsDone()) {
        playerTimer.isActive = false;
        player_sprite_toggle ^= 1;
    }
    // Draw player sprite
    if (player_sprite_toggle == 0){
        DrawPlayerTile(protagonist.x, protagonist.y, 0, 1);
    }
    else{
        DrawPlayerTile(protagonist.x, protagonist.y, 0, 3);
    }

    // Draw projectiles
    for (const auto& projectile : player_projectiles) {
        //TraceLog(LOG_INFO, "Projectile : x=%f, y=%f", projectile.position.x, projectile.position.y);

        if (projectile.active) 
        {
            // Draw the projectile
            //TraceLog(LOG_INFO, "Projectile position: x=%f, y=%f", projectile.position.x, projectile.position.y);
            DrawCircleV(projectile.position, projectileRadius, RED);
        }
    }

        // Draw projectiles
    for (const auto& projectile : enemy_projectiles) {
        //TraceLog(LOG_INFO, "Projectile : x=%f, y=%f", projectile.position.x, projectile.position.y);

        if (projectile.active) 
        {
            // Draw the projectile
            //TraceLog(LOG_INFO, "Projectile position: x=%f, y=%f", projectile.position.x, projectile.position.y);
            DrawCircleV(projectile.position, projectileRadius, BLUE);
        }
    }

    EndMode2D();

    //DrawRectangle(5, 5, 330, 140, Fade(SKYBLUE, 1.0f));
    DrawRectangleLines(5, 5, 330, 120, BLUE);
    DrawText(TextFormat("Camera Target: (%06.2f, %06.2f", camera.target.x, camera.target.y), 15, 10, 14, YELLOW);
    DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 30, 14, YELLOW);
    DrawText(TextFormat("Player Health: %d", player.health), 15, 50, 14, YELLOW);
    DrawText(TextFormat("Player Money: %d", player.money), 15, 90, 14, YELLOW);
    DrawText(TextFormat("player x y: %d %d", protagonist.x/16, protagonist.y/16), 15, 110, 14, YELLOW);
}

void Game::Shutdown() {
    for (auto& texture : textures) UnloadTexture(texture);
    audio.shutdown_audio();
    CloseAudioDevice();
}

// private:
void Game::DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip) {
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
    DrawTexturePro(textures[static_cast<int>(TextureAsset::Dungeon)], source, dest, { 0,0 }, 0, WHITE);
}

void Game::DrawPlayerTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip) {
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
    DrawTexturePro(textures[static_cast<int>(TextureAsset::Player)], source, dest, { 0,0 }, 0, WHITE);
}


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