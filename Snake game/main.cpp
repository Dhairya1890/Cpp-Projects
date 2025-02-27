#include <iostream>
#include <raylib.h>
#include<deque>
#include<raymath.h>

using namespace std;
//Defining the board colors:
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
//Creating cells for movement logic ease:
int cellSize = 30;
int cellCount = 25;
int offset = 70;

double lastUpdateTime = 0;

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}
bool ElementInDeque(Vector2 element, deque<Vector2> deque){

for(unsigned int i = 0; i< deque.size(); i++){
    if(Vector2Equals(deque[i], element)){
        return true;
    }
}
return false;
}

class Snake {
    public:
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9} , Vector2{4,9}};
    Vector2 direction = {1,0};

    bool addSegment = false;


    void Draw(){
        for(unsigned int i = 0; i<body.size(); i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }
    void update(){
        body.push_front(Vector2Add(body[0] ,direction));
        if(addSegment == true){
            addSegment = false;
        }
        else{
            body.pop_back();
        }

        
    }
    void Reset(){
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1, 0};
    }
};

class food{
    public:
    //Raylib library to create co-ordinates for food:
    Vector2 position = {5,6};//These are the food co-ordinates cells by which food can be accessed(x,y):
    Texture2D texture;
    food(deque<Vector2> snakeBody){//Food Constructor: to load the texture
        Image image = LoadImage("food.png");

        texture = LoadTextureFromImage(image);

        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }
    //Destructor: to unload the texture.
    ~food(){
        UnloadTexture(texture);
    }
    void Draw(){
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y*cellSize, WHITE);
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCount-1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
        
        Vector2 position  = GenerateRandomCell();
        while(ElementInDeque(position, snakeBody)){
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game{
public:
Snake snake = Snake();
food Food  = food(snake.body);
bool running = true;
int score = 0;

void Draw(){
    Food.Draw();
    snake.Draw();
}
void update(){
    if(running){
        snake.update();
    CheckCollisionWithFood();
    CheckCollisionWithEdge();
    CheckCollisionWithTail();
    }
}

void CheckCollisionWithFood() {
    printf("Snake Head: (%.2f, %.2f), Food: (%.2f, %.2f)\n",
           snake.body[0].x, snake.body[0].y,
           Food.position.x, Food.position.y);

    if (Vector2Equals(snake.body[0], Food.position)) {
        Food.position = Food.GenerateRandomPos(snake.body);
        snake.addSegment = true;
        score++;
    }
}
void CheckCollisionWithEdge(){
    if(snake.body[0].x == cellCount || snake.body[0].x == -1){
        GameOver();
    }
    if(snake.body[0].y == cellCount || snake.body[0].y == -1){
        GameOver();
    }
}

void GameOver(){
    snake.Reset();
    Food.position = Food.GenerateRandomPos(snake.body);
    running = false;
    score = 0;
}

void CheckCollisionWithTail(){
    deque<Vector2> headlessBody = snake.body;
    headlessBody.pop_front();
    if(ElementInDeque(snake.body[0], headlessBody)){
        GameOver();
    }
}

};
int main () {
cout<<"Starting the game..."<<endl; 

//Making a board for the game:
InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount,"Retro Snake");


//Defining the speed of frame updation during the loop:
SetTargetFPS(60);

Game game = Game();

Snake snake = Snake();

while(WindowShouldClose() == false){//defining the loop condition, the loop will run till the window is closed.
    //Function to draw on the board:
    BeginDrawing();
    if(eventTriggered(0.2)){
        game.update();
    }
    if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
        game.snake.direction = {0 , -1};
        game.running = true;
    }
    if(IsKeyPressed(KEY_DOWN)&& game.snake.direction.y != -1){
        game.snake.direction = {0, 1};
        game.running = true;

    }
    if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
        game.snake.direction = {-1, 0};
        game.running = true;

    }
    if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
        game.snake.direction = {1, 0};
        game.running = true;

    }
    game.CheckCollisionWithFood();
    game.CheckCollisionWithEdge();
    game.CheckCollisionWithTail();
    //calling the color:green.
    ClearBackground(green);
    DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, darkGreen);
    DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
    DrawText(TextFormat("%i",game.score), offset-5, offset+cellSize*cellCount+10, 40, darkGreen);
    game.Draw();
    EndDrawing();
}
//Closing the window:
CloseWindow();
}