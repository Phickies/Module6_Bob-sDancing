// Sketch for dancing with Bob.
// Documents and code are written by Phicks.

/*
  Using your mouse to click to the colored square to get the score.
  If you missed clicked it, you will get the minus 1 heart.
    
  Bob will be very disapointed if you missed it.
  
  The current program is run at a random systematic rhythm.
  For more detail, please visit the rythmManager class.
  
  Will implement the feature so that the user could feed customized pattern, rhythm
  to this, like DDR.
*/

int gridSize = 3;

Square[][] squares;
GameManager gameManager;
ScreenManager screenManager;

void setup() {
  size(700, 700);
  gameManager = new GameManager(gridSize, width, height);
  screenManager = new ScreenManager(width, height, gameManager);
  squares = gameManager.initializeSquares();
  frameRate(60);
}

void draw() {
  gameManager.updateSquares(squares); // Update game logic, including rhythmic patterns
  screenManager.displayScreen(squares); // Display the updated state of the game
}

void mousePressed() {
  gameManager.handleMouseClick(mouseX, mouseY, squares);
}
