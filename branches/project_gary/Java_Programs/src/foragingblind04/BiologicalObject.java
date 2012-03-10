package foragingblind04;

import java.awt.Color;
import java.util.Random;

/**
 *
 * @author ric
 */
public class BiologicalObject {

    double radianVector;
    double xPos, yPos;
    double xTail, yTail;
    double speed = 1.0;
    Color colour;
    double score;
    Random rand = new Random();
    boolean reorientation;
    CellType type;
    int changeVectorChoice;
    int reorientationChoice;

    public BiologicalObject(CellType type, double xPos, double yPos, Color colour) {
        this.type = type;
        this.xPos = xPos;
        this.yPos = yPos;
        this.colour = colour;
        radianVector = Math.random() * Math.PI * 2;
        score = 0;
        reorientation = rand.nextBoolean();
        changeVectorChoice = type.setChangeReorientationDirection;
        reorientationChoice = type.setChangeToDirectional;
    }

    private void changeRadianVector() {
        double delta;
        if ((reorientationChoice == 0)
                || (rand.nextInt(reorientationChoice) == 0)) {
            reorientation = !reorientation;
        }
        if (reorientation) {
            delta = type.setReorientationAngle;
            changeVectorChoice = type.setChangeReorientationDirection;
            reorientationChoice = type.setChangeToDirectional;
            speed = type.setSpeedReorientation;
            colour = Color.RED;
        } else {
            delta = type.setDirectionalAngle;
            changeVectorChoice = type.setChangeDirectionalDirection;
            reorientationChoice = type.setChangeToReorientation;
            speed = type.setSpeedDirectional;
            colour = Color.BLUE;
        }
        radianVector += (rand.nextDouble() * delta * 2.0) - delta;
        if (radianVector < 0) {
            radianVector = Math.PI * 2 + radianVector;
        }
        if (radianVector >= Math.PI * 2) {
            radianVector = radianVector - Math.PI * 2;
        }
    }

    public void move() {
        if ((changeVectorChoice == 0)
                || (rand.nextInt(changeVectorChoice) == 0)) {
            changeRadianVector();
        }
        xPos += Math.sin(radianVector) * speed;
        yPos += Math.cos(radianVector) * speed;
        xTail = xPos + (Math.sin(radianVector) * -10.0);
        yTail = yPos + (Math.cos(radianVector) * -10.0);
    }
}

