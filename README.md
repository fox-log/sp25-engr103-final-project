# sp25-engr103-final-project
This is the final project for my Spring 2025 Engeering 103H class, meant to played on an Adafruit Circuit Playground Express.
It is a simple reflex game where the player must stop the spinning wheel on a designated pixel. A win (stopping the wheel in the correct spot) will result in a win tune being played and an increase to your score. A loss will play the loosing tone. Score is displayed at the end of each match (regardless of a win or loss) by the number of active pixels around the edge of the Circuit Playground.

As your score increases, the wheel's size will shrink and its speed will increase, making the game more difficult.

## Inputs
### Pause Button
The slidder switch can be used to stop any gameplay and clear the board. Unpausing will cause a reset to the game.

### Double Click
When on the score screen, clicking both buttons simultaniously will start a new match.

### Single Click
Clicking either button while in the game screen will cause a "catch", ending the game.

## Outputs
### Game Screen
While a game round is active, the neopixels on the Circuit Playground will be used to display the spinning wheel (in pink) and the spot where the player needs to stop it (in white)

### Score Screen
While in the score screen between games, the neopixels will present your score, using the number active (between 0-9) to show your score in the 'ones' place (this is done by activating `score % 10` pixels), and the color of the pixels will represent your scrore in the tens place (done with `map(score/10, 0, 10, 1,8)`)
