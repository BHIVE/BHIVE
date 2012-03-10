package foragingblind04;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.Random;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 *
 * @author ric
 */
public class ExperimentWindow extends JFrame implements Runnable {

    Random rand = new Random();
    Thread runner;
    boolean running;
    int displaySize;
    ABMPanel abmDisplay;
    JSlider magnificationSlider;
    JButton stopButton;
    JLabel statusLabel;
    public Experiment investigation;

    public ExperimentWindow() {
        displaySize = 500;
        setSize(displaySize + 2, displaySize + 50);

        abmDisplay = new ABMPanel();
        this.getContentPane().add(BorderLayout.CENTER, abmDisplay);
        abmDisplay.addMouseMotionListener(new abmMouseMoved());

        magnificationSlider = new JSlider();
        magnificationSlider.setMaximum(100);
        magnificationSlider.addChangeListener(new manification1Listener());

        JLabel zoomLabel = new JLabel("Zoom ");
        stopButton = new JButton("Stop");
        stopButton.addActionListener(new stopButtonListener());

        statusLabel = new JLabel("Average resource : 0.0");
        JPanel controlPanel = new JPanel();
        controlPanel.add(BorderLayout.WEST, stopButton);
        controlPanel.add(BorderLayout.EAST, zoomLabel);
        controlPanel.add(BorderLayout.CENTER, magnificationSlider);
        this.getContentPane().add(BorderLayout.SOUTH, controlPanel);
        this.getContentPane().add(BorderLayout.NORTH, statusLabel);

        setVisible(true);
        abmDisplay.setDisplay();
    }

    public void setExperiment(int distance, int numberOfTargets, int numberOfCells, CellType type) {
        investigation = new Experiment(distance, numberOfTargets, numberOfCells, type);
    }

    public void drawCA() {
        int score;
        this.abmDisplay.clearDisplay();
        for (Target t : investigation.targets) {
            this.abmDisplay.fillOval(t.xPos, t.yPos, t.colour);
        }
        for (BiologicalObject b : investigation.cells) {
            score = (int) b.score * 10;
            if (score > 255) {
                score = 255;
            }
            this.abmDisplay.fillOval(b.xPos, b.yPos, new Color(score, score,
                    score, 100));
            this.abmDisplay.drawOval(b.xPos, b.yPos, b.colour);
            this.abmDisplay.drawLine(b.xPos, b.yPos, b.xTail, b.yTail, b.colour);
        }
        this.abmDisplay.updateDisplay();
    }

    public void start() {
        if (runner == null) {
            runner = new Thread(this);
            running = true;
            this.setVisible(true);
        }
        runner.start();
    }

    public void run() {
        while (runner == Thread.currentThread()) {
            if (running) {
                this.investigation.iterate();
            }
            drawCA();
            this.statusLabel.setText("Average resource : "+Double.toString(this.investigation.getAverageResource()));
        }
    }

    class manification1Listener implements ChangeListener {

        @Override
        public void stateChanged(ChangeEvent e) {
            abmDisplay.magnification = magnificationSlider.getValue() / 30.0;
        }
    }
    class abmMouseMoved implements MouseMotionListener{
        public void mouseDragged(MouseEvent me) {
            System.out.println(me.getX());
        }

        public void mouseMoved(MouseEvent me) {
          //  throw new UnsupportedOperationException("Not supported yet.");
        }

    }

    class stopButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent ae) {
            if (running) {
                running = false;
                stopButton.setText("Resume");
            } else {
                running = true;
                stopButton.setText("Stop");
            }
        }
    }
}
