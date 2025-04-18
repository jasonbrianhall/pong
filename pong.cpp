#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Paddle dimensions
const int PADDLE_WIDTH = 15;
const int PADDLE_HEIGHT = 90;
const int PADDLE_SPEED = 5;

// Ball dimensions
const int BALL_SIZE = 15;
const int BALL_SPEED = 5;

// Difficulty levels
enum class Difficulty { EASY, MEDIUM, HARD };

// Font handling
const std::vector<std::string> FONT_PATHS = {
    // Common system fonts
    "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",  // Ubuntu
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Dejavu
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",  // Liberation Sans
    
    // More generic paths
    "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
    "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
    
    // Additional potential locations
    "/usr/share/fonts/truetype/droid/DroidSans.ttf",
    "/usr/share/fonts/google-droid/DroidSans.ttf",
    
    // Fallback generic names
    "FreeSans.ttf",
    "DejaVuSans.ttf",
    "LiberationSans-Regular.ttf"
    "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
    "/usr/share/fonts/dejavu-sans-mono-fonts/DejaVuSansMono.ttf",
    "/usr/share/fonts/dejavu-serif-fonts/DejaVuSerif.ttf",
    
    // Fallback to full paths from previous attempts
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
    
    // Absolute paths and generic names
    "DejaVuSans.ttf",
    "arial.ttf"

};

TTF_Font *loadBestFont(int fontSize) {
  TTF_Font *font = nullptr;

  for (const auto &fontPath : FONT_PATHS) {
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (font) {
      std::cout << "Successfully loaded font: " << fontPath << std::endl;
      return font;
    }
  }

  std::cerr << "Error: Could not load any font. Text rendering will fail."
            << std::endl;
  return nullptr;
}

class Paddle {
public:
  SDL_Rect rect;
  int yVel = 0;

  Paddle(int x, int y) {
    rect.x = x;
    rect.y = y;
    rect.w = PADDLE_WIDTH;
    rect.h = PADDLE_HEIGHT;
  }

  void move() {
    rect.y += yVel;

    // Keep paddle within screen bounds
    if (rect.y < 0)
      rect.y = 0;
    if (rect.y + rect.h > SCREEN_HEIGHT)
      rect.y = SCREEN_HEIGHT - rect.h;
  }

  int centerY() const { return rect.y + rect.h / 2; }
};

class Ball {
public:
  SDL_Rect rect;
  int xVel, yVel;

  Ball() {
    rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    rect.w = BALL_SIZE;
    rect.h = BALL_SIZE;
    resetBall();
  }

  void move() {
    rect.x += xVel;
    rect.y += yVel;

    // Bounce off top and bottom walls
    if (rect.y <= 0 || rect.y + rect.h >= SCREEN_HEIGHT) {
      yVel = -yVel;
    }
  }

  void resetBall() {
    rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;

    // Randomize initial ball direction
    xVel = (rand() % 2 == 0 ? 1 : -1) * BALL_SPEED;
    yVel = (rand() % 2 == 0 ? 1 : -1) * BALL_SPEED;
  }

  int predictedYPosition() const {
    if (xVel == 0)
      return SCREEN_HEIGHT / 2;

    // Calculate intersection point with right side of screen
    int timeToReach = (SCREEN_WIDTH - rect.x) / abs(xVel);
    int predictedY = rect.y + yVel * timeToReach;

    // Bounce prediction off vertical walls
    while (predictedY < 0 || predictedY + BALL_SIZE > SCREEN_HEIGHT) {
      if (predictedY < 0) {
        predictedY = -predictedY;
      }
      if (predictedY + BALL_SIZE > SCREEN_HEIGHT) {
        predictedY = 2 * (SCREEN_HEIGHT - BALL_SIZE) - predictedY;
      }
    }

    return predictedY + BALL_SIZE / 2;
  }
};

class Pong {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  bool running;
  bool isPlayerTwoAI;
  Difficulty aiDifficulty;

  Paddle leftPaddle;
  Paddle rightPaddle;
  Ball ball;

  int leftScore;
  int rightScore;

  void moveAIPaddle() {
    int targetY = ball.predictedYPosition();
    int paddleCenter = rightPaddle.centerY();

    // Difficulty-based AI behavior
    switch (aiDifficulty) {
    case Difficulty::EASY:
      // Move randomly and slowly
      if (rand() % 10 < 3) {
        rightPaddle.yVel =
            (targetY > paddleCenter) ? PADDLE_SPEED / 2 : -PADDLE_SPEED / 2;
      } else {
        rightPaddle.yVel = 0;
      }
      break;

    case Difficulty::MEDIUM:
      // Moderate tracking with some error
      if (abs(targetY - paddleCenter) > PADDLE_HEIGHT / 4) {
        rightPaddle.yVel =
            (targetY > paddleCenter) ? PADDLE_SPEED : -PADDLE_SPEED;
      } else {
        rightPaddle.yVel = 0;
      }
      break;

    case Difficulty::HARD:
      // Near-perfect tracking
      if (abs(targetY - paddleCenter) > 5) {
        rightPaddle.yVel =
            (targetY > paddleCenter) ? PADDLE_SPEED : -PADDLE_SPEED;
      } else {
        rightPaddle.yVel = 0;
      }
      break;
    }
  }

public:
  Pong(bool playerTwoAI = true, Difficulty difficulty = Difficulty::MEDIUM)
      : window(nullptr), renderer(nullptr), font(nullptr), running(true),
        isPlayerTwoAI(playerTwoAI), aiDifficulty(difficulty),
        leftPaddle(50, SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2),
        rightPaddle(SCREEN_WIDTH - 50 - PADDLE_WIDTH,
                    SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2),
        leftScore(0), rightScore(0) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
                << std::endl;
      return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
      std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: "
                << TTF_GetError() << std::endl;
      return;
    }

    // Create window
    window =
        SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
                << std::endl;
      return;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
      std::cerr << "Renderer could not be created! SDL Error: "
                << SDL_GetError() << std::endl;
      return;
    }

    // Load font
    font = loadBestFont(24);
  }

  void handleEvents() {
    SDL_Event event;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // Process all events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }

      // Difficulty selection
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_1:
          aiDifficulty = Difficulty::EASY;
          std::cout << "Difficulty set to EASY" << std::endl;
          break;
        case SDLK_2:
          aiDifficulty = Difficulty::MEDIUM;
          std::cout << "Difficulty set to MEDIUM" << std::endl;
          break;
        case SDLK_3:
          aiDifficulty = Difficulty::HARD;
          std::cout << "Difficulty set to HARD" << std::endl;
          break;
        }
      }
    }

    // Left paddle movement (W and S keys)
    leftPaddle.yVel = 0;
    if (keystate[SDL_SCANCODE_W]) {
      leftPaddle.yVel = -PADDLE_SPEED;
    }
    if (keystate[SDL_SCANCODE_S]) {
      leftPaddle.yVel = PADDLE_SPEED;
    }

    // Right paddle movement
    if (isPlayerTwoAI) {
      // AI controlled paddle
      moveAIPaddle();
    } else {
      // Human controlled paddle (Up and Down arrow keys)
      rightPaddle.yVel = 0;
      if (keystate[SDL_SCANCODE_UP]) {
        rightPaddle.yVel = -PADDLE_SPEED;
      }
      if (keystate[SDL_SCANCODE_DOWN]) {
        rightPaddle.yVel = PADDLE_SPEED;
      }
    }
  }

  void update() {
    leftPaddle.move();
    rightPaddle.move();
    ball.move();

    // Ball collision with paddles
    if (SDL_HasIntersection(&ball.rect, &leftPaddle.rect) ||
        SDL_HasIntersection(&ball.rect, &rightPaddle.rect)) {

      // Determine which paddle was hit
      SDL_Rect *hitPaddle = SDL_HasIntersection(&ball.rect, &leftPaddle.rect)
                                ? &leftPaddle.rect
                                : &rightPaddle.rect;

      // Reverse horizontal direction
      ball.xVel = -ball.xVel;

      // Adjust vertical velocity based on where the ball hits the paddle
      int paddleCenter = hitPaddle->y + hitPaddle->h / 2;
      int ballCenter = ball.rect.y + ball.rect.h / 2;

      // Calculate the offset from the paddle's center
      int offset = ballCenter - paddleCenter;

      // Modify y velocity based on hit location
      // This creates a more dynamic bouncing effect
      ball.yVel += offset / 10;

      // Prevent the ball from getting stuck inside the paddle
      if (ball.rect.x < SCREEN_WIDTH / 2) {
        // Left paddle hit
        ball.rect.x = hitPaddle->x + hitPaddle->w;
      } else {
        // Right paddle hit
        ball.rect.x = hitPaddle->x - ball.rect.w;
      }
    }

    // Scoring
    if (ball.rect.x <= 0) {
      rightScore++;
      ball.resetBall();
    }
    if (ball.rect.x + ball.rect.w >= SCREEN_WIDTH) {
      leftScore++;
      ball.resetBall();
    }
  }
  void render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render paddles
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &leftPaddle.rect);
    SDL_RenderFillRect(renderer, &rightPaddle.rect);

    // Render ball
    SDL_RenderFillRect(renderer, &ball.rect);

    // Render scores
    renderScore();

    // Render center line
    renderCenterLine();

    // Render difficulty instructions
    renderDifficultyInstructions();

    // Present renderer
    SDL_RenderPresent(renderer);
  }

  void renderScore() {
    if (!font)
      return;

    // Render left score
    SDL_Color white = {255, 255, 255, 255};
    std::string leftScoreText = std::to_string(leftScore);
    SDL_Surface *leftSurface =
        TTF_RenderText_Solid(font, leftScoreText.c_str(), white);

    if (!leftSurface) {
      std::cerr << "Unable to render left score text! SDL_ttf Error: "
                << TTF_GetError() << std::endl;
      return;
    }

    SDL_Texture *leftTexture =
        SDL_CreateTextureFromSurface(renderer, leftSurface);

    SDL_Rect leftScoreRect;
    leftScoreRect.x = SCREEN_WIDTH / 4;
    leftScoreRect.y = 20;
    SDL_QueryTexture(leftTexture, NULL, NULL, &leftScoreRect.w,
                     &leftScoreRect.h);
    SDL_RenderCopy(renderer, leftTexture, NULL, &leftScoreRect);

    // Render right score
    std::string rightScoreText = std::to_string(rightScore);
    SDL_Surface *rightSurface =
        TTF_RenderText_Solid(font, rightScoreText.c_str(), white);

    if (!rightSurface) {
      std::cerr << "Unable to render right score text! SDL_ttf Error: "
                << TTF_GetError() << std::endl;
      SDL_FreeSurface(leftSurface);
      SDL_DestroyTexture(leftTexture);
      return;
    }

    SDL_Texture *rightTexture =
        SDL_CreateTextureFromSurface(renderer, rightSurface);

    SDL_Rect rightScoreRect;
    rightScoreRect.x = 3 * SCREEN_WIDTH / 4;
    rightScoreRect.y = 20;
    SDL_QueryTexture(rightTexture, NULL, NULL, &rightScoreRect.w,
                     &rightScoreRect.h);
    SDL_RenderCopy(renderer, rightTexture, NULL, &rightScoreRect);

    // Clean up
    SDL_FreeSurface(leftSurface);
    SDL_FreeSurface(rightSurface);
    SDL_DestroyTexture(leftTexture);
    SDL_DestroyTexture(rightTexture);
  }

  void renderCenterLine() {
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
      SDL_Rect centerLine;
      centerLine.x = SCREEN_WIDTH / 2 - 2;
      centerLine.y = y;
      centerLine.w = 4;
      centerLine.h = 10;
      SDL_RenderFillRect(renderer, &centerLine);
    }
  }

  void renderDifficultyInstructions() {
    if (!font)
      return;

    SDL_Color white = {255, 255, 255, 255};
    std::string difficultyText = "AI Difficulty: 1-Easy, 2-Medium, 3-Hard";

    SDL_Surface *difficultySurface =
        TTF_RenderText_Solid(font, difficultyText.c_str(), white);

    if (!difficultySurface) {
      std::cerr << "Unable to render difficulty text! SDL_ttf Error: "
                << TTF_GetError() << std::endl;
      return;
    }

    SDL_Texture *difficultyTexture =
        SDL_CreateTextureFromSurface(renderer, difficultySurface);

    SDL_Rect difficultyRect;
    difficultyRect.x = SCREEN_WIDTH / 2 - difficultySurface->w / 2;
    difficultyRect.y = SCREEN_HEIGHT - 40;
    difficultyRect.w = difficultySurface->w;
    difficultyRect.h = difficultySurface->h;
    SDL_RenderCopy(renderer, difficultyTexture, NULL, &difficultyRect);

    // Clean up
    SDL_FreeSurface(difficultySurface);
    SDL_DestroyTexture(difficultyTexture);
  }

  void run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (running) {
      frameStart = SDL_GetTicks();

      handleEvents();
      update();
      render();

      frameTime = SDL_GetTicks() - frameStart;
      if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
      }
    }
  }

  ~Pong() {
    // Clean up resources
    if (font)
      TTF_CloseFont(font);
    if (renderer)
      SDL_DestroyRenderer(renderer);
    if (window)
      SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
  }
};

int main(int argc, char *argv[]) {
  // Seed random number generator
  srand(SDL_GetTicks());

  // Allow command-line configuration of game mode and difficulty
  bool isPlayerTwoAI = true;
  Difficulty difficulty = Difficulty::MEDIUM;

  // Parse command-line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--2player" || arg == "-2") {
      isPlayerTwoAI = false;
    } else if (arg == "--easy" || arg == "-e") {
      difficulty = Difficulty::EASY;
    } else if (arg == "--hard" || arg == "-h") {
      difficulty = Difficulty::HARD;
    } else if (arg == "--help") {
      std::cout << "Pong Game Usage:" << std::endl;
      std::cout << "  --2player, -2   : Two-player mode" << std::endl;
      std::cout << "  --easy, -e      : Set AI difficulty to Easy" << std::endl;
      std::cout << "  --hard, -h      : Set AI difficulty to Hard" << std::endl;
      std::cout << "  --help          : Show this help message" << std::endl;
      std::cout << "\nControls:" << std::endl;
      std::cout << "  Left Paddle (Player 1): W/S keys" << std::endl;
      std::cout << "  Right Paddle: "
                << (isPlayerTwoAI ? "AI (Adjust difficulty with 1-3 keys)"
                                  : "Up/Down arrow keys")
                << std::endl;
      return 0;
    }
  }

  // Create and run the game
  Pong game(isPlayerTwoAI, difficulty);
  game.run();

  return 0;
}
