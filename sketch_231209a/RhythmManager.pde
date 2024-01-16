class RhythmManager {

  int currentPatternIndex = 0; // Index of the current pattern

  int patternSwitchInterval; // Frames after which the pattern switches
  int[][] rhythmicPatterns; // Array of interval-duration pairs
  
  int scaleTime = 8;

  RhythmManager(int switchInterval) {

    this.patternSwitchInterval = switchInterval;

    // Define rhythmic patterns [interval, duration]
    this.rhythmicPatterns = new int[][]{
      {15*scaleTime, 15*scaleTime}, // Pattern 2: Interval .5s, Duration 1s
      {20*scaleTime, 20*scaleTime}, // Pattern 3: Interval 1s, Duration 1s
      {30*scaleTime, 30*scaleTime}  // Pattern 4: Interval 1.5s, Duration 1.5s
      // Add more patterns as needed
    };
  }

  int[] getCurrentPattern() {
    return rhythmicPatterns[currentPatternIndex];
  }

  void updatePattern(int frameCount) {
    /*
      Method to switch to different pattern.
      (Currently set to random)
     */
     
    if (frameCount % patternSwitchInterval == 0) {
      //currentPatternIndex = (currentPatternIndex + 1) % rhythmicPatterns.length;
      currentPatternIndex = int(random(0, rhythmicPatterns.length-1));
    }
  }
}
