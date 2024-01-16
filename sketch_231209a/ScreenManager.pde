class ScreenManager {
    int width, height;
    GameManager gameManager;
    int margin = 20;

    ScreenManager(int width, int height, GameManager gameManager) {
        this.width = width;
        this.height = height;
        this.gameManager = gameManager;
    }

    void displayScreen(Square[][] squares) { //<>//
        
        // Display gameOver screen
        if (gameManager.isGameOver()) {
            displayGameOverScreen();
            return;
        }
        
        // Display normal gameplay screen
        background(250);
        for (int i = 0; i < squares.length; i++) {
            for (int j = 0; j < squares[i].length; j++) {
                squares[i][j].display();
            }
        }
        //displayHeader();
    }

    //void displayHeader() {
    //    fill(0);
    //    textSize(32);
    //    text("Score: " + gameManager.getScore(), margin, 40);
    //    text("Hearts: " + gameManager.getHearts(), margin, 80);
    //}

    void displayGameOverScreen() {
        background(0);
        fill(255);
        textSize(64);
        textAlign(CENTER, CENTER);
        text("You are out of hearts", width / 2, height / 3);
        text("Bob is disappointed in you", width / 2, height /2);
    }
}
