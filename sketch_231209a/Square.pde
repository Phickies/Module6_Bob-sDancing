class Square {
  int x, y; // Position of the square
  int size; // Size of the square

  boolean isActive; // If the square is currently active (colored)
  boolean isRightActive;
  boolean isLeftActive;

  int timer; // Timer for how long the square stays active
  color activeColor; // Color when the square is active

  // Constructor
  Square(int x, int y, int size, color activeColor) {
    this.x = x;
    this.y = y;
    this.size = size;
    this.isActive      = false;
    this.isRightActive = false;
    this.isLeftActive  = false;
    this.timer = 0;
    this.activeColor = activeColor;
  }

  // Method to display the square
  void display() {
    if (isActive) {
      if (isRightActive) {
        fill(activeColor);
        rect(x+size/2, y, size/2, size);
        fill(200);
        rect(x, y, size/2, size);
        return;
      } else if (isLeftActive) {
        fill(activeColor);
        rect(x, y, size/2, size);
        fill(200);
        rect(x+size/2, y, size/2, size);
        return;
      }
      fill(activeColor); // Active squares are assigned with specific color
    } else {
      fill(200); // Inactive squares are grey
    }
    rect(x, y, size, size);
  }

  // Method to check if the square is clicked
  boolean isClicked(int mouseX, int mouseY) {
    return mouseX >= x && mouseX < x + size && mouseY >= y && mouseY < y + size;
  }

  // Method to activate or deactivate the square
  void setActive(boolean active, int time) {
    isActive = active;
    if (active) {
      timer = time; // Set the timer if activated
    }
  }

  // Method to update the square each frame
  void update() {
    if (isActive && timer > 0) {
      timer--; // Decrease the timer
      if (timer <= 0) {
        isActive = false; // Deactivate if timer reaches zero
      }
    }
  }

  // Additional method to check if the square is inactive
  boolean isInactive() {
    return !isActive;
  }
}
