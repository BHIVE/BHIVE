package foragingblind04;


import foragingblind04.BiologicalObject;
import java.awt.Color;
import java.util.ArrayList;
import java.util.Random;

/**
 *
 * @author ric
 */
public class Experiment {

    public ArrayList<Target> targets;
    public ArrayList<BiologicalObject> cells;
    Random rand = new Random();

    public Experiment(int distance, int numberOfTargets, int numberOfCells, CellType type ) {
        Target t;
        BiologicalObject b;
        double x, y;
        targets = new ArrayList<Target>();
        cells = new ArrayList<BiologicalObject>();
        for (int i = 0; i < numberOfTargets; i++) {
            x = (rand.nextDouble() * distance * 2) - distance;
            y = (rand.nextDouble() * distance * 2) - distance;
            t = new Target(x, y, Color.ORANGE);
            targets.add(t);
        }

        for (int i = 0; i < numberOfCells; i++) {
            x = 0;
            y = 0;
            b = new BiologicalObject(type,x, y, Color.BLUE);
            cells.add(b);
        }
    }

    public void scoreToTargets(BiologicalObject c) {
        double distanceX, distanceY, distanceSq;
        for (Target t : targets) {
            distanceX = c.xPos - t.xPos;
            distanceY = c.yPos - t.yPos;
            distanceSq = (distanceX * distanceX) + (distanceY * distanceY);
            c.score += 1 / distanceSq;
        }
    }

    public void iterate() {
        for (BiologicalObject c : cells) {
            c.move();
            scoreToTargets(c);
        }
    }

    public double getAverageResource(){
        double sum = 0.0;
        for (BiologicalObject c : cells) {
            sum +=c.score;
        }
        return sum/cells.size();
    }
}
