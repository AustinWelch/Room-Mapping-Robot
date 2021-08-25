/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
package com.ti.msp430.imagereformer;


import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.FocusListener;
import java.awt.image.*;
import java.text.NumberFormat;
import java.util.*;

import javax.swing.*;
import javax.swing.border.Border;

public class ImageDisplay extends JPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = -6519689355657597475L;

	BufferedImage thisImage;




	JLabel xlabel = new JLabel();
	JLabel ylabel = new JLabel();
	TextField xsize = new TextField();;
	TextField ysize = new TextField();

	JPanel imageSize = new JPanel();
	JLabel depth = new JLabel();
	JLabel colorSpace = new JLabel();
	JPanel compressionPanel = new JPanel();
	ColorPalette paletteBar;
	JRadioButton uncompressedButton = new JRadioButton("Off");
	JRadioButton compressedButton = new JRadioButton("On");
	//JRadioButton compressedRLE8SpeedButton = new JRadioButton("RLE8 Speed");
	//JRadioButton compressedRLE8SizeButton = new JRadioButton("RLE8 Size");

	Panel stats = new Panel();
	Panel topLine = new Panel();
	Panel bottomLine = new Panel();
	
	String title;

	class ComparibleColor extends Color implements Comparable {

		/**
		 * 
		 */
		private static final long serialVersionUID = 346418231120947948L;

		public ComparibleColor(int arg0, int arg1, int arg2) {
			super(arg0, arg1, arg2);
		}

		public int compareTo(Object arg0) {
			Color c = (Color) arg0;
			if (c.getRGB() > getRGB()) {
				return (-1);
			} else if (c.getRGB() < getRGB()) {
				return (1);
			}
			return 0;
		}

	}

	class ColorPalette extends Button {

		/**
		 * 
		 */
		private static final long serialVersionUID = 6539790657871467337L;
		Dimension dimension;
		byte[] rr;
		byte[] bb;
		byte[] gg;
		int numColorsInPalette;
		int width;

		public ColorPalette(int width) {
			super();
			setEnabled(false);
			this.width = width;
		}

		public Dimension getPreferredSize() {
			return new Dimension(width, 15);
		}

		public Dimension getMinimumSize() {
			return getPreferredSize();
		}

		public void paint(Graphics g) {
			super.paint(g);

			if (thisImage != null) {
				dimension = getSize();
				ColorModel cm = thisImage.getColorModel();
				if (cm instanceof IndexColorModel) {
					IndexColorModel icm = (IndexColorModel) cm;
					numColorsInPalette = icm.getMapSize();

					rr = new byte[numColorsInPalette];
					bb = new byte[numColorsInPalette];
					gg = new byte[numColorsInPalette];
					icm.getReds(rr);
					icm.getGreens(gg);
					icm.getBlues(bb);
					int offset = 1;
					Vector v = new Vector();
					for (int i = 0; i < numColorsInPalette; i++) {
						try {
							ComparibleColor myColor = new ComparibleColor(
									rr[i] & 0xff, gg[i] & 0xff, bb[i] & 0xff);

							v.addElement(myColor);

						} catch (Exception e) {
							System.out.println(e + " r: " + rr[i] + " g: "
									+ gg[i] + " b: " + bb[i]);
						}
					}
					Collections.sort(v);
					float barSize = (float) dimension.width / (float) v.size();
					for (int i = 0; i < v.size(); i++) {

						int intBarSize = (int) Math.ceil(barSize);
						ComparibleColor myColor = (ComparibleColor) v
								.elementAt(i);
						g.setColor(myColor);
						g.fillRect(intBarSize * i, 0, intBarSize * offset,
								dimension.height);
						g.setColor(Color.BLACK);
						g.drawRect(intBarSize * i, 0, intBarSize * offset,
								dimension.height);
						offset++;
					}
				}
			}
		}
	}

	public ImageDisplay() {
		setLayout(new BorderLayout());
	}

	public ImageDisplay(BufferedImage name,String title) {
		this();
		this.thisImage = name;
		this.title=title;
		ImageWindow my = new ImageWindow(name);

		add(my, BorderLayout.CENTER);

		xlabel.setText(" Width: ");
		if (name != null)
			xsize.setText(Integer.toString(name.getWidth()));
		xsize.setEditable(true);

		ylabel.setText(" Height: ");
		if (name != null)
			ysize.setText(Integer.toString(name.getHeight()));

		imageSize.setLayout(new GridLayout(1, 4));
		imageSize.add(xlabel);
		imageSize.add(xsize);
		imageSize.add(ylabel);
		imageSize.add(ysize);

		if (thisImage != null)
			depth.setText("Color Depth: " + name.getColorModel().getPixelSize());
		if (thisImage != null)
			colorSpace.setText("Color Space: "
					+ System.getProperty("java.version"));

			ButtonGroup group = new ButtonGroup();
			group.add(uncompressedButton);
			group.add(compressedButton);

			compressionPanel.setLayout(new GridLayout(1, 4));
			compressionPanel.add(new Label("Compression"));

			compressionPanel.add(uncompressedButton);
			compressionPanel.add(compressedButton);
			uncompressedButton.setSelected(true);

		stats.setLayout(new GridLayout(2, 1));
		topLine.setLayout(new GridLayout(1,3));
		
		
		topLine.add(imageSize);
		
		Button t=new Button(title);
		t.setEnabled(false);
		//t.setForeground(Color.RED);
		
		topLine.add(t);
		topLine.add(depth);
		
		//line1.add(colorSpace);
		stats.add(topLine);
		// stats.add(transparency);
		
		stats.add(compressionPanel);

		Border loweredbevel = BorderFactory.createLoweredBevelBorder();
		Border raisedbevel = BorderFactory.createRaisedBevelBorder();
		Border compound = BorderFactory.createCompoundBorder(raisedbevel,
				loweredbevel);

		imageSize.setBorder(loweredbevel);
		depth.setBorder(loweredbevel);
		colorSpace.setBorder(loweredbevel);
		// transparency.setBorder(loweredbevel);
		compressionPanel.setBorder(loweredbevel);

		// stats.setBorder(compound);
		this.setBorder(compound);
		if (this.thisImage!=null) { 
			paletteBar = new ColorPalette(this.thisImage.getWidth());
			add(paletteBar, BorderLayout.SOUTH);
		}

	    

		add(stats, BorderLayout.NORTH);
		validate();
	}

	public void addActionListener(ActionListener l) {
		xsize.addActionListener(l);
		ysize.addActionListener(l);
		//uncompressedButton.addActionListener(l);
		//compressedButton.addActionListener(l);
	}

	public void addFocusListener(FocusListener l) {
		xsize.addFocusListener(l);
		ysize.addFocusListener(l);
		//uncompressedButton.addActionListener(l);
		//compressedButton.addActionListener(l);
	}
	public void allowChange(boolean allow) {
		xsize.setEditable(allow);
		ysize.setEditable(allow);
		uncompressedButton.setEnabled(allow);
		compressedButton.setEnabled(allow);
		//compressedRLE8SpeedButton.setEnabled(allow);
		//compressedRLE8SizeButton.setEnabled(allow); 

		stats.setEnabled(allow); 
		topLine.setEnabled(allow); 
		bottomLine.setEnabled(allow); 
	}

	public int getXsize() {
		String val = xsize.getText();
		try {
			return (Integer.parseInt(val));
		} catch (NumberFormatException e) {
			return (1);
		}
	}

	public int getYsize() {
		String val = ysize.getText();
		try {
			return (Integer.parseInt(val));
		} catch (NumberFormatException e) {
			return (1);
		}
	}

	public BufferedImage getThisImage() {
		return thisImage;
	}

	public void setThisImage(BufferedImage thisImage) {
		this.thisImage = thisImage;
	}
	
	public int getCompression() {
		if (uncompressedButton.isSelected()) {
			return (Output.UNCOMPRESSED);
		} else if (compressedButton.isSelected()) {
			return (Output.COMPRESSED);
		/*} else if (compressedRLE8SpeedButton.isSelected()) {
			return (Output.COMPRESSED_RLE8_SPEED);
		} else if (compressedRLE8SizeButton.isSelected()) {
			return (Output.COMPRESSED_RLE8_SIZE);*/
		} else
			return (-1);

	}

}
