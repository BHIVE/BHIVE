/*
 * NewApplication.java
 *
 * Created on Jun 9, 2010, 3:11:30 PM
 */
package foragingblind04;

/**
 *
 * @author ric
 */
public class JFrameControl extends javax.swing.JFrame {

    ExperimentWindow investigation;

    /** Creates new form NewApplication */
    public JFrameControl() {

        initComponents();
        this.setSize(800,300);
    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jPanel2 = new javax.swing.JPanel();
        jLabelNT = new javax.swing.JLabel();
        jSliderTargets = new javax.swing.JSlider();
        jLabelNC = new javax.swing.JLabel();
        jSliderCells = new javax.swing.JSlider();
        jLabelTD = new javax.swing.JLabel();
        jSliderDistance = new javax.swing.JSlider();
        jPanel5 = new javax.swing.JPanel();
        jLabel8 = new javax.swing.JLabel();
        jLabel9 = new javax.swing.JLabel();
        jLabel11 = new javax.swing.JLabel();
        jFormattedTextFieldVCA = new javax.swing.JFormattedTextField();
        jPanel7 = new javax.swing.JPanel();
        jLabel10 = new javax.swing.JLabel();
        jLabel15 = new javax.swing.JLabel();
        jLabel16 = new javax.swing.JLabel();
        jFormattedTextFieldRCA = new javax.swing.JFormattedTextField();
        jPanel3 = new javax.swing.JPanel();
        jPanel6 = new javax.swing.JPanel();
        jLabel12 = new javax.swing.JLabel();
        jLabel13 = new javax.swing.JLabel();
        jLabel14 = new javax.swing.JLabel();
        jFormattedTextFieldVCB = new javax.swing.JFormattedTextField();
        jPanel8 = new javax.swing.JPanel();
        jLabel17 = new javax.swing.JLabel();
        jLabel18 = new javax.swing.JLabel();
        jLabel19 = new javax.swing.JLabel();
        jFormattedTextFieldRCB = new javax.swing.JFormattedTextField();
        jPanel11 = new javax.swing.JPanel();
        jLabel25 = new javax.swing.JLabel();
        jLabel26 = new javax.swing.JLabel();
        jLabel27 = new javax.swing.JLabel();
        jFormattedTextFieldSA = new javax.swing.JFormattedTextField();
        jPanel10 = new javax.swing.JPanel();
        jLabel22 = new javax.swing.JLabel();
        jLabel23 = new javax.swing.JLabel();
        jLabel24 = new javax.swing.JLabel();
        jFormattedTextFieldSB = new javax.swing.JFormattedTextField();
        jPanel9 = new javax.swing.JPanel();
        jLabel5 = new javax.swing.JLabel();
        jLabel20 = new javax.swing.JLabel();
        jLabel21 = new javax.swing.JLabel();
        jFormattedTextFieldDA = new javax.swing.JFormattedTextField();
        jPanel4 = new javax.swing.JPanel();
        jLabel3 = new javax.swing.JLabel();
        jLabel6 = new javax.swing.JLabel();
        jLabel7 = new javax.swing.JLabel();
        jFormattedTextFieldDB = new javax.swing.JFormattedTextField();
        menuBar = new javax.swing.JMenuBar();
        fileMenu = new javax.swing.JMenu();
        exitMenuItem = new javax.swing.JMenuItem();
        editMenu = new javax.swing.JMenu();
        jMenuItemRunExp = new javax.swing.JMenuItem();
        helpMenu = new javax.swing.JMenu();
        contentsMenuItem = new javax.swing.JMenuItem();
        aboutMenuItem = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setLayout(new java.awt.GridLayout(1, 2));

        jPanel2.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                jPanel2MousePressed(evt);
            }
        });
        jPanel2.addMouseMotionListener(new java.awt.event.MouseMotionAdapter() {
            public void mouseDragged(java.awt.event.MouseEvent evt) {
                jPanel2MouseDragged(evt);
            }
        });
        jPanel2.setLayout(new java.awt.GridLayout(5, 2));

        jLabelNT.setText(" Number of targets 1000");
        jPanel2.add(jLabelNT);

        jSliderTargets.setMaximum(2000);
        jSliderTargets.setPaintLabels(true);
        jSliderTargets.setPaintTicks(true);
        jSliderTargets.setValue(1000);
        jSliderTargets.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSliderTargetsStateChanged(evt);
            }
        });
        jSliderTargets.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
            public void propertyChange(java.beans.PropertyChangeEvent evt) {
                jSliderTargetsPropertyChange(evt);
            }
        });
        jPanel2.add(jSliderTargets);

        jLabelNC.setText(" Number of cells 50");
        jPanel2.add(jLabelNC);

        jSliderCells.setMaximum(1000);
        jSliderCells.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSliderCellsStateChanged(evt);
            }
        });
        jPanel2.add(jSliderCells);

        jLabelTD.setText(" Target distance 1000");
        jPanel2.add(jLabelTD);

        jSliderDistance.setMaximum(2000);
        jSliderDistance.setMinorTickSpacing(10);
        jSliderDistance.setValue(1000);
        jSliderDistance.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                jSliderDistanceStateChanged(evt);
            }
        });
        jPanel2.add(jSliderDistance);

        jPanel5.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel5.setLayout(new java.awt.GridLayout(3, 1));

        jLabel8.setText(" Probability of change");
        jPanel5.add(jLabel8);

        jLabel9.setText(" of direction when in ");
        jPanel5.add(jLabel9);

        jLabel11.setText(" Reorientation phase");
        jPanel5.add(jLabel11);

        jPanel2.add(jPanel5);

        jFormattedTextFieldVCA.setText("0");
        jFormattedTextFieldVCA.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jFormattedTextFieldVCAActionPerformed(evt);
            }
        });
        jPanel2.add(jFormattedTextFieldVCA);

        jPanel7.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel7.setLayout(new java.awt.GridLayout(3, 1));

        jLabel10.setText(" Probability of change");
        jPanel7.add(jLabel10);

        jLabel15.setText(" from Reorentation");
        jPanel7.add(jLabel15);

        jLabel16.setText(" to Directional phase");
        jPanel7.add(jLabel16);

        jPanel2.add(jPanel7);

        jFormattedTextFieldRCA.setText("10");
        jFormattedTextFieldRCA.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jFormattedTextFieldRCAActionPerformed(evt);
            }
        });
        jPanel2.add(jFormattedTextFieldRCA);

        jPanel1.add(jPanel2);

        jPanel3.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jPanel3.setLayout(new java.awt.GridLayout(6, 2));

        jPanel6.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel6.setLayout(new java.awt.GridLayout(3, 1));

        jLabel12.setText(" Probability of change");
        jPanel6.add(jLabel12);

        jLabel13.setText(" of direction when in ");
        jPanel6.add(jLabel13);

        jLabel14.setText(" Directional phase");
        jPanel6.add(jLabel14);

        jPanel3.add(jPanel6);

        jFormattedTextFieldVCB.setText("5");
        jPanel3.add(jFormattedTextFieldVCB);

        jPanel8.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel8.setLayout(new java.awt.GridLayout(3, 1));

        jLabel17.setText(" Probability of change");
        jPanel8.add(jLabel17);

        jLabel18.setText(" from Directioan");
        jPanel8.add(jLabel18);

        jLabel19.setText(" to Reorientation phase");
        jPanel8.add(jLabel19);

        jPanel3.add(jPanel8);

        jFormattedTextFieldRCB.setText("10");
        jPanel3.add(jFormattedTextFieldRCB);

        jPanel11.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel11.setLayout(new java.awt.GridLayout(3, 1));

        jLabel25.setText(" Speed when");
        jPanel11.add(jLabel25);

        jLabel26.setText(" in Reorientation phase ");
        jPanel11.add(jLabel26);

        jLabel27.setText(" ");
        jPanel11.add(jLabel27);

        jPanel3.add(jPanel11);

        jFormattedTextFieldSA.setText("1.0");
        jPanel3.add(jFormattedTextFieldSA);

        jPanel10.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel10.setLayout(new java.awt.GridLayout(3, 1));

        jLabel22.setText(" Speed when");
        jPanel10.add(jLabel22);

        jLabel23.setText(" in Directional phase");
        jPanel10.add(jLabel23);

        jLabel24.setText(" ");
        jPanel10.add(jLabel24);

        jPanel3.add(jPanel10);

        jFormattedTextFieldSB.setText("1.0");
        jPanel3.add(jFormattedTextFieldSB);

        jPanel9.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel9.setLayout(new java.awt.GridLayout(3, 1));

        jLabel5.setText(" Fractional");
        jPanel9.add(jLabel5);

        jLabel20.setText(" radius change");
        jPanel9.add(jLabel20);

        jLabel21.setText(" in Reorientation phase");
        jPanel9.add(jLabel21);

        jPanel3.add(jPanel9);

        jFormattedTextFieldDA.setText("0.5");
        jPanel3.add(jFormattedTextFieldDA);

        jPanel4.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
        jPanel4.setLayout(new java.awt.GridLayout(3, 1));

        jLabel3.setText(" Fractional");
        jPanel4.add(jLabel3);

        jLabel6.setText(" radius change");
        jPanel4.add(jLabel6);

        jLabel7.setText(" in Directional phase");
        jPanel4.add(jLabel7);

        jPanel3.add(jPanel4);

        jFormattedTextFieldDB.setText("0.1");
        jPanel3.add(jFormattedTextFieldDB);

        jPanel1.add(jPanel3);

        getContentPane().add(jPanel1, java.awt.BorderLayout.CENTER);

        fileMenu.setText("File");

        exitMenuItem.setText("Exit");
        exitMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                exitMenuItemActionPerformed(evt);
            }
        });
        fileMenu.add(exitMenuItem);

        menuBar.add(fileMenu);

        editMenu.setText("Experiment");

        jMenuItemRunExp.setText("Run");
        jMenuItemRunExp.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItemRunExpActionPerformed(evt);
            }
        });
        editMenu.add(jMenuItemRunExp);

        menuBar.add(editMenu);

        helpMenu.setText("Help");

        contentsMenuItem.setText("Contents");
        helpMenu.add(contentsMenuItem);

        aboutMenuItem.setText("About");
        helpMenu.add(aboutMenuItem);

        menuBar.add(helpMenu);

        setJMenuBar(menuBar);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void exitMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_exitMenuItemActionPerformed
        System.exit(0);
}//GEN-LAST:event_exitMenuItemActionPerformed

    private void jMenuItemRunExpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItemRunExpActionPerformed
        CellType cType = new CellType();
        cType.setChangeReorientationDirection = Integer.parseInt(this.jFormattedTextFieldVCA.getText());
        cType.setChangeToDirectional = Integer.parseInt(this.jFormattedTextFieldRCA.getText());
        cType.setChangeDirectionalDirection = Integer.parseInt(this.jFormattedTextFieldVCB.getText());
        cType.setChangeToReorientation = Integer.parseInt(this.jFormattedTextFieldRCB.getText());
        cType.setSpeedReorientation = Double.parseDouble(this.jFormattedTextFieldSA.getText());
        cType.setSpeedDirectional = Double.parseDouble(this.jFormattedTextFieldSB.getText());
        cType.setReorientationAngle = Math.PI * Double.parseDouble(this.jFormattedTextFieldDA.getText());
        cType.setDirectionalAngle = Math.PI * Double.parseDouble(this.jFormattedTextFieldDB.getText());
        int targetDistance = this.jSliderDistance.getValue();
        int targets = this.jSliderTargets.getValue();
        int cells = this.jSliderCells.getValue();
        investigation = new ExperimentWindow();
        investigation.setExperiment(targetDistance, targets, cells, cType);
        investigation.start();
    }//GEN-LAST:event_jMenuItemRunExpActionPerformed

    private void jSliderTargetsPropertyChange(java.beans.PropertyChangeEvent evt) {//GEN-FIRST:event_jSliderTargetsPropertyChange
        
    }//GEN-LAST:event_jSliderTargetsPropertyChange

    private void jSliderTargetsStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSliderTargetsStateChanged
        this.jLabelNT.setText(" Number of targets " + Integer.toString(this.jSliderTargets.getValue()));
    }//GEN-LAST:event_jSliderTargetsStateChanged

    private void jSliderCellsStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSliderCellsStateChanged
        this.jLabelNC.setText(" Number of cells " + Integer.toString(this.jSliderCells.getValue()));
    }//GEN-LAST:event_jSliderCellsStateChanged

    private void jSliderDistanceStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_jSliderDistanceStateChanged
        this.jLabelTD.setText(" Target distance " + Integer.toString(this.jSliderDistance .getValue()));
    }//GEN-LAST:event_jSliderDistanceStateChanged

    private void jPanel2MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanel2MousePressed
     
    }//GEN-LAST:event_jPanel2MousePressed

    private void jPanel2MouseDragged(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jPanel2MouseDragged
        // TODO add your handling code here:
    }//GEN-LAST:event_jPanel2MouseDragged

    private void jFormattedTextFieldVCAActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jFormattedTextFieldVCAActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jFormattedTextFieldVCAActionPerformed

    private void jFormattedTextFieldRCAActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jFormattedTextFieldRCAActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_jFormattedTextFieldRCAActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JMenuItem aboutMenuItem;
    private javax.swing.JMenuItem contentsMenuItem;
    private javax.swing.JMenu editMenu;
    private javax.swing.JMenuItem exitMenuItem;
    private javax.swing.JMenu fileMenu;
    private javax.swing.JMenu helpMenu;
    private javax.swing.JFormattedTextField jFormattedTextFieldDA;
    private javax.swing.JFormattedTextField jFormattedTextFieldDB;
    private javax.swing.JFormattedTextField jFormattedTextFieldRCA;
    private javax.swing.JFormattedTextField jFormattedTextFieldRCB;
    private javax.swing.JFormattedTextField jFormattedTextFieldSA;
    private javax.swing.JFormattedTextField jFormattedTextFieldSB;
    private javax.swing.JFormattedTextField jFormattedTextFieldVCA;
    private javax.swing.JFormattedTextField jFormattedTextFieldVCB;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel11;
    private javax.swing.JLabel jLabel12;
    private javax.swing.JLabel jLabel13;
    private javax.swing.JLabel jLabel14;
    private javax.swing.JLabel jLabel15;
    private javax.swing.JLabel jLabel16;
    private javax.swing.JLabel jLabel17;
    private javax.swing.JLabel jLabel18;
    private javax.swing.JLabel jLabel19;
    private javax.swing.JLabel jLabel20;
    private javax.swing.JLabel jLabel21;
    private javax.swing.JLabel jLabel22;
    private javax.swing.JLabel jLabel23;
    private javax.swing.JLabel jLabel24;
    private javax.swing.JLabel jLabel25;
    private javax.swing.JLabel jLabel26;
    private javax.swing.JLabel jLabel27;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JLabel jLabel9;
    private javax.swing.JLabel jLabelNC;
    private javax.swing.JLabel jLabelNT;
    private javax.swing.JLabel jLabelTD;
    private javax.swing.JMenuItem jMenuItemRunExp;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel10;
    private javax.swing.JPanel jPanel11;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JPanel jPanel4;
    private javax.swing.JPanel jPanel5;
    private javax.swing.JPanel jPanel6;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JSlider jSliderCells;
    private javax.swing.JSlider jSliderDistance;
    private javax.swing.JSlider jSliderTargets;
    private javax.swing.JMenuBar menuBar;
    // End of variables declaration//GEN-END:variables
}
