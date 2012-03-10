package foragingblind04;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import javax.swing.JPanel;

/**
 *
 * @author ric
 */
public class ABMPanel extends JPanel {

    Image backImg1;
    Graphics2D backGr1;
    Image backImg2;
    Graphics2D backGr2;
    int displayWidth, displayHeight;
    int xOffset = 10;
    public double magnification = 2.0;

    public void setDisplay() {
        displayWidth = this.getSize().width - 2;
        displayHeight = this.getSize().height - 2;
        backImg1 = createImage(displayWidth, displayHeight);
        backGr1 = (Graphics2D) backImg1.getGraphics();
        backImg2 = createImage(displayWidth, displayHeight);
        backGr2 = (Graphics2D) backImg2.getGraphics();
    }

    public void fillOval(double xPos, double yPos, Color colour) {
        int x = (int) ((displayWidth) / 2.0 + (xPos * magnification) - (5 * magnification));
        int y = (int) ((displayHeight) / 2.0 + (yPos * magnification) - (5 * magnification));
        int xx = (int) (10 * magnification);
        if (xx < 2) {
            xx = 2;
        }
        int yy = (int) (10 * magnification);
        if (yy < 2) {
            yy = 2;
        }
        if ((x > 0) && (y > 0) && (x < displayWidth) && (y < displayHeight)) {
            backGr1.setColor(colour);
            backGr1.fillOval(x, y, xx, yy);
        }
    }

    public void clearDisplay() {
        backGr1.setColor(Color.BLACK);
        backGr1.fillRect(0, 0, displayWidth, displayHeight);
    }

    public void drawOval(double xPos, double yPos, Color colour) {
         int x = (int) ((displayWidth) / 2.0 + (xPos * magnification) - (5 * magnification));
        int y = (int) ((displayHeight) / 2.0 + (yPos * magnification) - (5 * magnification));
        int xx = (int) (10 * magnification);
        if (xx < 2) {
            xx = 2;
        }
        int yy = (int) (10 * magnification);
        if (yy < 2) {
            yy = 2;
        }if ((x > 0) && (y > 0) && (x < displayWidth) && (y < displayHeight)) {
            backGr1.setColor(colour);
            backGr1.drawOval(x, y, xx, yy);
        }
    }

    public void drawLine(double xPos, double yPos, double xTail, double yTail, Color colour) {
        int x = (int) ((displayWidth) / 2.0 + (xPos * magnification));
        int y = (int) ((displayHeight) / 2.0 + (yPos * magnification));
        int xx = (int) ((displayWidth) / 2.0 + (xTail * magnification));
        int yy = (int) ((displayHeight) / 2.0 + (yTail * magnification));
        if ((x > 0) && (y > 0) && (x < displayWidth) && (y < displayHeight)) {
            backGr1.setColor(colour);
            backGr1.drawLine((int) x, (int) y, (int) xx, (int) yy);
        }
    }

    public void updateDisplay() {
        backGr2.drawImage(backImg1, 0, 0, displayWidth, displayHeight, 0, 0,
                displayWidth, displayHeight, this);
        repaint();
    }

    @Override
    public void paint(Graphics g) {
        if ((backImg2 != null) && (g != null)) {
            g.drawImage(backImg2, 1, 1, this.getSize().width - 2, this.getSize().height - 2, 0, 0, displayWidth, displayHeight,
                    this);
        }
    }
}

