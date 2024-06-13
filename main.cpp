#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;
Color ciano = {0, 255, 255, 255};
Color darkBlue = {0, 0, 139, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;
double lastFoodUpdateTime = 0;

enum Difficulty { EASY, MEDIUM, HARD };

struct GameSettings {
    Difficulty difficulty;
    float updateInterval;
};

bool ElementInDeque(Vector2 element, deque<Vector2> *deque) /*uso de ponteiro*/
{
    for (unsigned int i = 0; i < deque->size(); i++)
    {
        if (Vector2Equals((*deque)[i], element))
        {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval, double &lastTime) /*uso de ponteiro*/
{
    double currentTime = GetTime();
    if (currentTime - lastTime >= interval)
    {
        lastTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> *body; /*uso de ponteiro*/
    Vector2 *direction; /*uso de ponteiro*/
    bool addSegment;

    Snake()
    {
        body = new deque<Vector2>({Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}}); /*uso de ponteiro*/
        direction = new Vector2{1, 0}; /*uso de ponteiro*/
        addSegment = false;
    }

    ~Snake()
    {
        delete body; /*uso de ponteiro*/
        delete direction; /*uso de ponteiro*/
    }

    void Draw()
    {
        for (unsigned int i = 0; i < body->size(); i++)
        {
            float x = (*body)[i].x;
            float y = (*body)[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkBlue);
        }
    }

    void Update()
    {
        body->push_front(Vector2Add((*body)[0], *direction)); /*uso de ponteiro*/
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body->pop_back(); /*uso de ponteiro*/
        }
    }

    void Reset()
    {
        *body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        *direction = {1, 0}; /*uso de ponteiro*/
    }
};

class Food
{
public:
    Vector2 *position; /*uso de ponteiro*/
    Vector2 *direction; /*uso de ponteiro*/
    Texture2D *texture; /*uso de ponteiro*/

    Food(deque<Vector2> *snakeBody) /*uso de ponteiro*/
    {
        Image image = LoadImage("Graphics/food.png");
        texture = new Texture2D(LoadTextureFromImage(image)); /*uso de ponteiro*/
        UnloadImage(image);
        position = new Vector2(GenerateRandomPos(snakeBody)); /*uso de ponteiro*/
        direction = new Vector2{GetRandomValue(-1, 1), GetRandomValue(-1, 1)}; /*uso de ponteiro*/
    }

    ~Food()
    {
        UnloadTexture(*texture); /*uso de ponteiro*/
        delete texture; /*uso de ponteiro*/
        delete position; /*uso de ponteiro*/
        delete direction; /*uso de ponteiro*/
    }

    void Draw()
    {
        DrawTexture(*texture, offset + position->x * cellSize, offset + position->y * cellSize, WHITE); /*uso de ponteiro*/
    }

    void Update()
    {
        *position = Vector2Add(*position, *direction); /*uso de ponteiro*/
        if (position->x >= cellCount || position->x < 0) direction->x *= -1; /*uso de ponteiro*/
        if (position->y >= cellCount || position->y < 0) direction->y *= -1; /*uso de ponteiro*/
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> *snakeBody) /*uso de ponteiro*/
    {
        Vector2 pos = GenerateRandomCell();
        while (ElementInDeque(pos, snakeBody)) /*uso de ponteiro*/
        {
            pos = GenerateRandomCell();
        }
        return pos;
    }
};

class Game
{
public:
    Snake *snake; /*uso de ponteiro*/
    deque<Food*> foods; /*uso de ponteiro*/
    bool running;
    int score;
    Sound *eatSound; /*uso de ponteiro*/
    Sound *wallSound; /*uso de ponteiro*/
    GameSettings settings;

    Game(GameSettings settings) : settings(settings)
    {
        snake = new Snake(); /*uso de ponteiro*/
        running = true;
        score = 0;
        InitAudioDevice();
        eatSound = new Sound(LoadSound("Sounds/eat.mp3")); /*uso de ponteiro*/
        wallSound = new Sound(LoadSound("Sounds/wall.mp3")); /*uso de ponteiro*/

        for (int i = 0; i < 3; i++) {
            foods.push_back(new Food(snake->body)); /*uso de ponteiro*/
        }
    }

    ~Game()
    {
        UnloadSound(*eatSound); /*uso de ponteiro*/
        UnloadSound(*wallSound); /*uso de ponteiro*/
        delete eatSound; /*uso de ponteiro*/
        delete wallSound; /*uso de ponteiro*/
        for (Food* food : foods) {
            delete food; /*uso de ponteiro*/
        }
        delete snake; /*uso de ponteiro*/
        CloseAudioDevice();
    }

    void Draw()
    {
        for (Food* food : foods) { /*uso de ponteiro*/
            food->Draw();
        }
        snake->Draw(); /*uso de ponteiro*/
    }

    void Update()
    {
        if (running)
        {
            snake->Update(); /*uso de ponteiro*/
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();

            if (EventTriggered(settings.updateInterval * 1.7, lastFoodUpdateTime)) {
                for (Food* food : foods) { /*uso de ponteiro*/
                    food->Update();
                }
            }
        }
    }

    void CheckCollisionWithFood()
    {
        for (Food* food : foods) /*uso de ponteiro*/
        {
            if (Vector2Equals((*snake->body)[0], *food->position)) /*uso de ponteiro*/
            {
                *food->position = food->GenerateRandomPos(snake->body); /*uso de ponteiro*/
                snake->addSegment = true; /*uso de ponteiro*/
                score++;
                PlaySound(*eatSound); /*uso de ponteiro*/
            }
        }
    }

    void CheckCollisionWithEdges()
    {
        if ((*snake->body)[0].x == cellCount || (*snake->body)[0].x == -1) /*uso de ponteiro*/
        {
            GameOver();
        }
        if ((*snake->body)[0].y == cellCount || (*snake->body)[0].y == -1) /*uso de ponteiro*/
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake->Reset(); /*uso de ponteiro*/
        for (Food* food : foods) /*uso de ponteiro*/
        {
            *food->position = food->GenerateRandomPos(snake->body); /*uso de ponteiro*/
        }
        running = false;
        score = 0;
        PlaySound(*wallSound); /*uso de ponteiro*/
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = *(snake->body); /*uso de ponteiro*/
        headlessBody.pop_front();
        if (ElementInDeque((*snake->body)[0], &headlessBody)) /*uso de ponteiro*/
        {
            GameOver();
        }
    }
};

void DrawMenu(Difficulty selectedDifficulty)
{
    ClearBackground(ciano);
    DrawText("Select Difficulty", offset, offset - 40, 40, darkBlue);

    const char* difficulties[] = {"EASY", "MEDIUM", "HARD"};
    for (int i = 0; i < 3; i++)
    {
        if (i == selectedDifficulty)
            DrawText(TextFormat("> %s <", difficulties[i]), offset, offset + i * 40, 30, darkBlue);
        else
            DrawText(difficulties[i], offset, offset + i * 40, 30, darkBlue);
    }
}

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "MOVING-SNAKE");
    SetTargetFPS(60);

    Difficulty selectedDifficulty = EASY;
    bool inMenu = true;
    Game* game = nullptr; /*uso de ponteiro*/

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        if (inMenu)
        {
            if (IsKeyPressed(KEY_DOWN))
                selectedDifficulty = (Difficulty)((selectedDifficulty + 1) % 3);
            if (IsKeyPressed(KEY_UP))
                selectedDifficulty = (Difficulty)((selectedDifficulty + 2) % 3);
            if (IsKeyPressed(KEY_ENTER))
            {
                inMenu = false;
                GameSettings settings;
                settings.difficulty = selectedDifficulty;
                if (selectedDifficulty == EASY)
                    settings.updateInterval = 0.3;
                else if (selectedDifficulty == MEDIUM)
                    settings.updateInterval = 0.2;
                else
                    settings.updateInterval = 0.1;

                game = new Game(settings); /*uso de ponteiro*/
            }
            DrawMenu(selectedDifficulty);
        }
        else
        {
            if (EventTriggered(game->settings.updateInterval, lastUpdateTime)) /*uso de ponteiro*/
            {
                allowMove = true;
                game->Update(); /*uso de ponteiro*/
            }

            if (IsKeyPressed(KEY_UP) && game->snake->direction->y != 1 && allowMove) /*uso de ponteiro*/
            {
                *(game->snake->direction) = {0, -1}; /*uso de ponteiro*/
                game->running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && game->snake->direction->y != -1 && allowMove) /*uso de ponteiro*/
            {
                *(game->snake->direction) = {0, 1}; /*uso de ponteiro*/
                game->running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && game->snake->direction->x != 1 && allowMove) /*uso de ponteiro*/
            {
                *(game->snake->direction) = {-1, 0}; /*uso de ponteiro*/
                game->running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_RIGHT) && game->snake->direction->x != -1 && allowMove) /*uso de ponteiro*/
            {
                *(game->snake->direction) = {1, 0}; /*uso de ponteiro*/
                game->running = true;
                allowMove = false;
            }

            // Drawing
            ClearBackground(ciano);
            DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkBlue);
            DrawText("MOVING-SNAKE", offset - 5, 20, 40, darkBlue);
            DrawText(TextFormat("%i", game->score), offset - 5, offset + cellSize * cellCount + 10, 40, darkBlue);
            game->Draw(); /*uso de ponteiro*/
        }

        EndDrawing();
    }

    if (game != nullptr) /*uso de ponteiro*/
        delete game; /*uso de ponteiro*/

    CloseWindow();
    return 0;
}
