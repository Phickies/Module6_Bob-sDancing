// GameManager Class
class GameManager {

  int score  = 0;
  int hearts = 3;

  boolean gameOver = false;

  int gridSize;
  int squareSize;

  int headerHeight = 0;
  int margin       = 0;
  int gridWidth, gridHeight;

  int activationInterval; // Time between activations
  int activationDuration; // Duration of each square's activation

  RhythmManager rhythmManager;

  GameManager(int gridSize, int width, int height) {
    this.gridSize = gridSize;
    gridWidth = width - 2 * margin;
    gridHeight = height - headerHeight - 2 * margin;
    squareSize = gridHeight / gridSize;

    // Ryhthmic pattern setup
    int patternSwitchInterval = 180; // Switch pattern every 3 seconds (at 60 FPS)
    rhythmManager = new RhythmManager(patternSwitchInterval);
  }

  Square[][] initializeSquares() {
    Square[][] squares = new Square[gridSize][gridSize];
    for (int i = 0; i < gridSize; i++) {
      for (int j = 0; j < gridSize; j++) {
        color activeColor = color(0); // Default black
        if (i == 0 && j == 0) activeColor = color(165, 42, 42);   // Top right brown
        if (i == 1 && j == 0) activeColor = color(255, 0, 0);     // Top middle red
        if (i == 2 && j == 0) activeColor = color(255, 165, 0);   // Top left orange
        if (i == 0 && j == 2) activeColor = color(255, 192, 203); // Bottom left pink
        if (i == 1 && j == 2) activeColor = color(255, 255, 0);   // Bottom middle yellow
        if (i == 2 && j == 2) activeColor = color(0, 255, 0);     // Bottom right green
        if (i == 0 && j == 1) activeColor = color(0, 0, 255);     // Middle left blue
        if (i == 1 && j == 1) activeColor = color(0);             // Middle middle black
        if (i == 2 && j == 1) activeColor = color(30, 255, 255);  // Middle right bold green

        squares[i][j] = new Square(margin + i * squareSize, headerHeight + margin + j * squareSize, squareSize, activeColor);
      }
    }
    return squares;
  }

  void handleMouseClick(int mouseX, int mouseY, Square[][] squares) {
    /*
      handle mouse click and game mechanic
     */

    if (gameOver) return;
    boolean clickedOnActiveSquare = false;
    for (int i = 0; i < gridSize; i++) {
      for (int j = 0; j < gridSize; j++) {
        if (squares[i][j].isClicked(mouseX, mouseY)) {
          if (squares[i][j].isActive) {
            score++;
            squares[i][j].setActive(false, 0);
            clickedOnActiveSquare = true;
          } else if (squares[i][j].isInactive()) {
            hearts--;
            if (hearts <= 0) gameOver = true;
            return;
          }
        }
      }
    }
    if (!clickedOnActiveSquare && hearts > 0) hearts--;
  }

  void updateSquares(Square[][] squares) {

    // Update rhythm
    rhythmManager.updatePattern(frameCount);
    int[] currentPattern = rhythmManager.getCurrentPattern();
    activationInterval = currentPattern[0];
    activationDuration = currentPattern[1];

    for (int i = 0; i < gridSize; i++) {
      for (int j = 0; j < gridSize; j++) {
        squares[i][j].update();
      }
    }
    if (frameCount % activationInterval == 0) {
      activateRandomSquare(squares);
    }
  }

  void activateRandomSquare(Square[][] squares) {
    int randX, randY;
    //do {
    randX = int(random(gridSize));
    randY = int(random(gridSize));
    //} while ((randX == 1 && randY == 1) || squares[randX][randY].isActive); // Avoid middle square and active squares

    Square middleSquare = squares[1][1];
    float r = random(1);
    if (r < 0.5) {
      middleSquare.isRightActive = true;
      middleSquare.isLeftActive = false;
    } else {
      middleSquare.isRightActive = false;
      middleSquare.isLeftActive = true;
    }

    squares[randX][randY].setActive(true, activationDuration);
  }

  boolean isGameOver() {
    return gameOver;
  }

  int getScore() {
    return score;
  }

  int getHearts() {
    return hearts;
  }
}
