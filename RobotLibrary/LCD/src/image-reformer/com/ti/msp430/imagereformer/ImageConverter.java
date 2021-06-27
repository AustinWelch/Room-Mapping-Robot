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
import java.awt.color.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import java.awt.event.*;
import java.net.*;

import javax.imageio.*;
import javax.swing.*;
import javax.swing.filechooser.FileFilter;


/**
 * @author a0194920 Chris Tsongas
 * 
 */
public class ImageConverter extends Panel implements ActionListener,FocusListener {

	String imageName;
	JButton bpp1 = new JButton(">>> 2 Color >>>");
	JButton bpp2 = new JButton(">>> 4 Color >>>");
	JButton bpp4 = new JButton(">>> 16 Color >>>");
	JButton bpp8 = new JButton(">>> 256 Color >>>");
	JButton grayScale = new JButton("Gray Scale >>>");
	static JFrame jf;
	static File lastDir=null;
	// take out for release
	Button write = new Button("create");
	JButton reset = new JButton("Reset");
	BufferedImage image;
	BufferedImage destination;
	
	static final String original="Original";
	static final String modified="Modified";
	

	MyPanel imagePanel;
	private static final long serialVersionUID = 1232211803609446210L;

	ImageDisplay originalPanel = new ImageDisplay();
	ImageDisplay modifiedPanel = new ImageDisplay();
	Panel buttons = new MyPanel();
	
	
	class MyPanel extends Panel {
		
		public MyPanel() {
			setPreferredSize(new Dimension(110,60));
		}
		
	}

	/**
	 * This class converts the input image to a bit palette image. Then
	 * processes the color map to platteSize palette components from the input
	 * image
	 * 
	 * This is a best guess conversion of the image by averaging the colors in
	 * 256 color palette inito paletteSize entries. Pure black[0], red[1],
	 * green[2], blue[3], white[paletteSize-1] are preserved in the palatte.
	 * 
	 * @author a0194920
	 * 
	 */
	class AdaptiveColorPalette {

		class FuzzyHashtable extends Hashtable {
			/**
			 * 
			 */
			int range = 1;
			private static final long serialVersionUID = -7991404974611369290L;

			/* 
			 * 
			 */
			public FuzzyHashtable(int colors) {
				super();
				this.range = 10;
			}

			public boolean inRange(int val, int target) {

				/*
				 * int red = (val & 0x00ff0000) >>> 16; int green = (val &
				 * 0x0000ff00) >>> 8; int blue = val & 0x000000ff;
				 * 
				 * int targetRed = (target & 0x00ff0000) >>> 16; int targetGreen
				 * = (target & 0x0000ff00) >>> 8; int targetBlue = target &
				 * 0x000000ff;
				 * 
				 * int minR=red-range; int maxR=red+range;
				 * 
				 * int minG=green-range; int maxG=green+range;
				 * 
				 * int minB=blue-range; int maxB=blue+range;
				 */
				int max = target + range;
				int min = target - range;

				if ((val >= min) && (val <= max)) {
					return (true);
				}
				return (false);
			}

			public Object get(Object key) {
				Object val = super.get(key);

				if (val == null) {
					Color keyC = (Color) key;
					int kRed = keyC.getRed();
					int kGreen = keyC.getGreen();
					int kBlue = keyC.getBlue();
					// Ok so it was not found, lets see if we have something
					// close

					for (Enumeration e = colors.keys(); e.hasMoreElements();) {
						Color c = (Color) e.nextElement();

						// Date dte=new Date();
						// System.out.println("Before inrange "+dte.getTime());
						int cRed = c.getRed();
						int cGreen = c.getGreen();
						int cBlue = c.getBlue();
						if (inRange(cRed, kRed) && inRange(cGreen, kGreen)
								&& inRange(cBlue, kBlue)) {
							/*
							 * Integer I= (Integer)super.get(o); int
							 * weight=I.intValue(); super.remove(o);
							 * 
							 * // Add this value and average... int
							 * wPlus1=weight+1;
							 * 
							 * int newRed=((cRed*weight)+kRed)/wPlus1; int
							 * newGreen=((cGreen*weight)+kGreen)/wPlus1; int
							 * newBlue=((cBlue*weight)+kBlue)/wPlus1; Color
							 * newC=new Color(newRed,newGreen,newBlue); I=new
							 * Integer(wPlus1); super.put(newC,I); return(I);
							 */
							return (super.get(c));
						}
						// dte=new Date();
						// System.out.println("After inrange "+dte.getTime());
					}
				}
				return (val);

			}
		}

		byte r[];
		byte g[];
		byte b[];
		Image bi;
		int size;
		FuzzyHashtable colors;

		/**
		 * 
		 * @param i
		 * @param paletteSize
		 */
		public AdaptiveColorPalette(Image i, int paletteSize) {
			this.bi = i;
			this.size = paletteSize;
			colors = new FuzzyHashtable(size);
			createPalette();
		}

		private void createPalette() {
			byte ff = (byte) 0xff;
			BufferedImage myImage = ImageConverter.toBufferedImage(bi);

			// Convert image to 8 bit with color map
			BufferedImage destination = new BufferedImage(myImage.getWidth(),
					myImage.getHeight(), BufferedImage.TYPE_BYTE_INDEXED);
			// ColorModel colorMod =destination.getColorModel();

			if (myImage.getColorModel() instanceof IndexColorModel) {
				destination = myImage;
			} else {
				Graphics2D gr = destination.createGraphics();
				RenderingHints hints = new RenderingHints(null);
				RescaleOp op = new RescaleOp(1f, 0f, hints);
				gr.drawImage(myImage, op, 0, 0);
			}

			IndexColorModel cm = (IndexColorModel) destination.getColorModel();

			int initialSize = cm.getMapSize();

			byte[] rr = new byte[initialSize];
			byte[] bb = new byte[initialSize];
			byte[] gg = new byte[initialSize];
			cm.getReds(rr);
			cm.getBlues(bb);
			cm.getGreens(gg);
			r = new byte[size];
			g = new byte[size];
			b = new byte[size];

			Date dte = new Date();
			int yoffset = 1;
			int xoffset = 1;

			int width = myImage.getWidth();
			int height = myImage.getHeight();
			if (height < 300) {
				yoffset = 4;
			} else if (height < 1000) {
				yoffset = 6;
			} else if (height < 1500) {
				yoffset = 8;
			} else {
				yoffset = 10;
			}

			if (width < 400) {
				xoffset = 1;
			} else if (width < 1500) {
				xoffset = 2;
			} else if (width < 2000) {
				xoffset = 4;
			} else {
				xoffset = 8;
			}
			System.out.println("Before loop " + dte.getTime());
			for (int y = 0; y < height; y += yoffset) {
				for (int x = 0; x < width; x += xoffset) {
					Color c = new Color(myImage.getRGB(x, y));
					// Date dte1=new Date();
					// System.out.println("Before colors.get "+dte1.getTime());
					Integer I = (Integer) colors.get(c);
					if (I == null) {
						colors.put(c, new Integer(1));
					} else {
						int i = I.intValue();
						i++;
						colors.put(c, i);
					}
					// dte1=new Date();
					// System.out.println("after colors.get "+dte1.getTime());
				}
			}

			Date dte1 = new Date();
			System.out.println("Before loop " + dte1.getTime());
			System.out.println(dte1.getTime() - dte.getTime());

			// Now take the most common colors and create a
			// hashtable based on the color frequency
			Hashtable ht = new Hashtable();
			for (Enumeration e = colors.keys(); e.hasMoreElements();) {
				Object k = e.nextElement();
				Object v = colors.get(k);
				ht.put(v, k);
			}

			Vector v = new Vector(ht.keySet());
			Collections.reverse(v);
			Iterator it = v.iterator();
			Vector vv = new Vector();
			while (it.hasNext()) {
				Object element = it.next();
				vv.addElement(ht.get(element));
			}

			for (int j = 0; j < size; j++) {
				if (vv.size() > j) {
					Color c = (Color) vv.elementAt(j);
					r[j] = (byte) c.getRed();
					b[j] = (byte) c.getBlue();
					g[j] = (byte) c.getGreen();
				}

			}

			boolean blackFound = false;
			boolean whiteFound = false;
			for (int i = 0; i < size; i++) {
				if ((r[i] == ff) && (g[i] == ff) && (b[i] == ff)) {
					whiteFound = true;
				}
				if ((r[i] == 0) && (g[i] == 0) && (b[i] == 0)) {
					blackFound = true;
				}

			}

			if (!whiteFound)
				r[size - 1] = g[size - 1] = b[size - 1] = ff;
			if (!blackFound)
				r[0] = g[0] = b[0] = 0;

		}

		public byte[] getReds() {
			return (r);
		}

		public byte[] getGreens() {
			return (g);
		}

		public byte[] getBlues() {
			return (b);
		}
	}

	class OutputFilter extends javax.swing.filechooser.FileFilter {

		public OutputFilter() {

		}

		public boolean accept(File file) {

			if (file.getName().endsWith(".c")) {
				return (true);
			}
			return (false);
		}

		public String getDescription() {
			return ("*.c");
		}

	}
	
	
	class ImageFilter extends javax.swing.filechooser.FileFilter {

		public ImageFilter() {

		}

		public boolean accept(File file) {

			if (file.getName().endsWith(".jpg") ||
				file.getName().endsWith(".bmp") ||
				file.getName().endsWith(".gif") ||	
				file.getName().endsWith(".jpeg") ||	
				file.getName().endsWith(".png") ||
				file.getName().endsWith(".tif") ||
				file.isDirectory()) {
				return (true);
			}
			return (false);
		}

		public String getDescription() {
			return ("*.jpg, *.bmp, *.gif, *.jpeg, *.png, *.tif");
		}

	}

	class OutputActionListner implements  ActionListener  { 
		
		
		public void	 actionPerformed(ActionEvent e) { 
			     if (imageName==null) {
			    	 JOptionPane.showMessageDialog(jf, "No image in buffer.");
			    	 return;
			     }
			     String fileName=imageName+".c";
		         
				 JFileChooser fc=new JFileChooser(lastDir);
				 fc.addChoosableFileFilter(new OutputFilter()); 
				 fc.setSelectedFile(new File(fileName)); 
				 int returnVal = fc.showSaveDialog(new JFrame()); 
				 
				 
				 if (returnVal == JFileChooser.APPROVE_OPTION) { 
					 File file = fc.getSelectedFile();
                     Output o=new Output(destination,imageName,modifiedPanel.getCompression(),jf);				 
				     o.write(fileName); 
				 } else { 
					 
					 System.out.println("canceled"); 
			     }
			    
				
		 }
		
	}
		
	
	class OpenActionListener implements ActionListener {
		
		public void actionPerformed(ActionEvent e) {
			JFileChooser fileChooser = new JFileChooser();
			if (lastDir==null) {
			   fileChooser.setCurrentDirectory(new File(System.getProperty("user.home")));
			} else {
				fileChooser.setCurrentDirectory(lastDir);
			}
			fileChooser.setFileFilter(new ImageFilter());
			int result = fileChooser.showOpenDialog(ImageConverter.this);
			if (result == JFileChooser.APPROVE_OPTION) {
			    File selectedFile = fileChooser.getSelectedFile();
			    lastDir=selectedFile.getParentFile();
			    System.out.println("Selected file: " + selectedFile.getAbsolutePath());
			    loadImage(selectedFile.getAbsolutePath());
			    reset();
			}
		}
	
	}
	
	public ImageConverter() {

		final SplashScreen splash = SplashScreen.getSplashScreen();
		if (splash == null) {
			System.out.println("SplashScreen.getSplashScreen() returned null");
			return;
		}
		Graphics2D g = splash.createGraphics();
		if (g == null) {
			System.out.println("g is null");
			return;
		}
		for (int i = 0; i < 10; i++) {
			renderSplashFrame(g, i);
			splash.update();
			try {
				Thread.sleep(90);
			} catch (InterruptedException e) {
			}
		}
		splash.close();
		
		
		setLayout(new BorderLayout());
        //transform.setEnabled(false);
		buttons.setLayout(new GridLayout(5, 1));
		//buttons.add(transform);
		buttons.add(bpp1);
		buttons.add(bpp2);
		buttons.add(bpp4);
		buttons.add(bpp8);
		// buttons.add(grayScale);
		// take out for release
		//buttons.add(write);
		buttons.add(reset);
		// write.setEnabled(false);
       

		bpp1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				bpp1();
			}
		});
		bpp2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				bpp2();
			}
		});
		bpp4.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				bpp4();
			}
		});
		bpp8.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				bpp8();
			}
		});
		grayScale.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				grayScale();
			}
		});

		// moved to rcp application menu but this is good to have
		// around for stand alone debug
		// take out for release
		write.addActionListener(new OutputActionListner());
		
			
		reset.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					System.out.println("reset called");
					reset();
				}
		});
		
	}

	
	public void renderSplashFrame(Graphics2D g, int frame) {
		final String[] displayItems = { "JVM", "Color Palette", "Button Images" };
		g.setComposite(AlphaComposite.Clear);
		g.fillRect(120, 140, 200, 40);
		g.setPaintMode();
		g.setColor(Color.WHITE);
		g.drawString("Loading " + displayItems[(frame / 5) % 3] + "...", 120, 150);
	}

	public void createMenuBar(final JFrame jf) {
		this.jf=jf;
		jf.setIconImage(new ImageIcon(loadURLImage("images/images.gif")).getImage());
		JMenuBar mb = new JMenuBar();
		System.out.println(System.getProperty("user.dir"));
		
		
		ImageIcon openImage = new ImageIcon(loadURLImage("images/open.gif"));
		ImageIcon buildImage = new ImageIcon(loadURLImage("images/gear_icon.gif"));
		JButton open = new JButton(openImage);
        JButton generate = new JButton(buildImage);
        
        
        JToolBar toolBar = new JToolBar("Conversion");
        toolBar.add(open);
        open.addActionListener(new OpenActionListener());
        
        
        toolBar.add(generate);    
        generate.addActionListener(new OutputActionListner());
       

			
   
     
		jf.setJMenuBar(mb);
		JMenu fileMenu = new JMenu("File");
		mb.add(fileMenu);
		
		JMenuItem openAction = new JMenuItem("Open",KeyEvent.VK_O);
		KeyStroke ctrlXKeyStroke = KeyStroke.getKeyStroke("control O");
		openAction.setAccelerator(ctrlXKeyStroke);
		openAction.addActionListener(new OpenActionListener());

	    
	    JMenuItem generateAction = new JMenuItem("Generate",KeyEvent.VK_G);
	    ctrlXKeyStroke = KeyStroke.getKeyStroke("control G");
	    generateAction.setAccelerator(ctrlXKeyStroke);
	    generateAction.addActionListener(new OutputActionListner());
	    
	    JMenuItem exitAction = new JMenuItem("Exit",KeyEvent.VK_X);
	    ctrlXKeyStroke = KeyStroke.getKeyStroke("control G");
	    exitAction.setAccelerator(ctrlXKeyStroke);
	    exitAction.addActionListener(new ActionListener() {
	    	public void actionPerformed(ActionEvent e) {
	    		System.exit(0);
	    	}
	    });
	    
	    fileMenu.add(openAction);
        fileMenu.add(generateAction);
      
        fileMenu.addSeparator();
        fileMenu.add(exitAction);
        
        
        JMenu helpMenu = new JMenu("Help");
        mb.add(helpMenu);
		JMenuItem aboutAction = new JMenuItem("About",KeyEvent.VK_A);
		ctrlXKeyStroke = KeyStroke.getKeyStroke("control A");
		aboutAction.setAccelerator(ctrlXKeyStroke);
		aboutAction.addActionListener(new ActionListener() {
		    	public void actionPerformed(ActionEvent e) {
		    		ImageIcon aboutImage = new ImageIcon(loadURLImage("images/about_430.gif"));
		    		JOptionPane.showMessageDialog(jf, "Image Reformer v1.10\n"	    				
		    				+ "MSP430 Software Applications",
		    				"Image Reformer",JOptionPane.PLAIN_MESSAGE,aboutImage);	
		    	}
		});
	    
	    JMenuItem helpAction = new JMenuItem("Help",KeyEvent.VK_F1);
	    ctrlXKeyStroke = KeyStroke.getKeyStroke("F1");
	    helpAction.setAccelerator(ctrlXKeyStroke);
            helpAction.addActionListener(new ActionListener() {
                       public void actionPerformed(ActionEvent e) {
                          JEditorPane infodex = new JEditorPane();
                          JDialog helpDialog = new JDialog();

	                  helpDialog.setModal(true);
                          URL url = null;
                          File manual=null;
                          try {
                              //url = getClass().getResource("/images/help.html");
                              url = new URL("jar:file:ImageReformer.jar!/images/help.html");
                              //manual = new File(getClass().getResource("/images/help.html").toURI());
                              //url=manual.toURL(); 
                          } catch (Exception e1) {
                              e1.printStackTrace();
                          }
                          System.out.println("URL: "+url); 
                          try {
                              infodex.setPage(url);
                          } catch (IOException ex) {
                              ex.printStackTrace();
                          }
              

                          helpDialog.getContentPane().add(new JScrollPane(infodex));
                          helpDialog.setBounds(400,200,900,600);
                          helpDialog.show();
                          infodex.setEditable(false);
                       }
           });

	    helpMenu.add(aboutAction);
	    helpMenu.add(helpAction);

	    
		//jf.getContentPane().setLayout(new BorderLayout());
		jf.getContentPane().add(toolBar, BorderLayout.NORTH);
		
		
	}
	
	
	public BufferedImage getDestination() {
		return destination;
	}

	public void setDestination(BufferedImage destination) {
		this.destination = destination;
	}

	public String getImageName() {
		return imageName;
	}

	public void setImageName(String imageName) {
		this.imageName = imageName;
	}

	public void createDisplay() {
		if (imagePanel != null)
			remove(imagePanel);
		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		//imagePanel.setLayout(new BoxLayout(imagePanel, BoxLayout.LINE_AXIS));
		//imagePanel.add(Box.createRigidArea(new Dimension(5,0)));
		
		// Create the images and add them to the display
		BufferedImage bi = toBufferedImage(image);
		originalPanel = new ImageDisplay(bi,original);
		originalPanel.allowChange(false);
	
		
	
		modifiedPanel = new ImageDisplay(toBufferedImage(image),original);
		modifiedPanel.addActionListener(this);
		modifiedPanel.addFocusListener(this);
		Panel left=new Panel();
		Panel right=new Panel();
		imagePanel.add(left);
		imagePanel.add(right);
		
		left.setLayout(new BorderLayout());
		right.setLayout(new BorderLayout());
		
		
		
		left.add(originalPanel,BorderLayout.CENTER);
		left.add(buttons,BorderLayout.EAST);
		right.add(modifiedPanel,BorderLayout.CENTER);
		//imagePanel.add(Box.createRigidArea(new Dimension(5,0)));
		
		this.add(imagePanel, BorderLayout.CENTER);

		destination = bi;
		//jf.pack();
	    jf.setVisible(true);
		//validate();
	}

	public void loadImage(String name) {
		this.imageName = name;
		try {
			image = ImageIO.read(new File(name));

		} catch (IOException e) {
		}
		BufferedImage bi = toBufferedImage(image);
		// If we got a Palette image convert it to direct color model so the
		// rest of
		// of the app only needs to deal with 1 type.
		if (bi.getColorModel() instanceof IndexColorModel) {
			int height = bi.getHeight();
			int width = bi.getWidth();
			BufferedImage newBi = new BufferedImage(width, height,
					BufferedImage.TYPE_INT_RGB);
			;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int rgb = bi.getRGB(x, y);
					newBi.setRGB(x, y, rgb);

				}
			}
			image = newBi;
		}
		createDisplay();
		validateTree();

	}

	public void reset() {
	   createDisplay();
	}

	public void grayScale() {
		imagePanel.remove(modifiedPanel);

		BufferedImageOp op = new ColorConvertOp(
				ColorSpace.getInstance(ColorSpace.CS_GRAY), null);

		destination = op.filter(image, null);

		originalPanel = new ImageDisplay(image,original);
		originalPanel.allowChange(false);
		modifiedPanel = new ImageDisplay(destination,modified);

		imagePanel.add(modifiedPanel);

		validateTree();
	}
	
	public int ImagePreamble() {
		int comp=-1;
		if (imagePanel != null) {
			if (modifiedPanel!=null) {
				comp=modifiedPanel.getCompression();
				
			}
			remove(imagePanel);
		}
		return(comp);
	}

	public void imagePost(int comp) {
		originalPanel = new ImageDisplay(toBufferedImage(image),original);
		originalPanel.allowChange(false);
		
		modifiedPanel = new ImageDisplay(destination,modified);
		modifiedPanel.addActionListener(this);
		modifiedPanel.addFocusListener(this);
		
		Panel left=new Panel();
		Panel right=new Panel();
		imagePanel.add(left);
		imagePanel.add(right);
		
		left.setLayout(new BorderLayout());
		right.setLayout(new BorderLayout());
		
		
		
		left.add(originalPanel,BorderLayout.CENTER);
		left.add(buttons,BorderLayout.EAST);
		right.add(modifiedPanel,BorderLayout.CENTER);
		if (comp!=-1) {
			if (comp==Output.COMPRESSED)	modifiedPanel.compressedButton.setSelected(true);
		}
		// write.setEnabled(true);
		validate();
	}
	
	public void bpp1() {

		BufferedImage bi = toBufferedImage(image);
		int comp=ImagePreamble();

		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		this.add(imagePanel, BorderLayout.CENTER);
		
		if (bi != null) {
			byte ff = (byte) 0xff;

			byte[] r = { 0, ff };
			byte[] g = { 0, ff };
			byte[] b = { 0, ff };
			IndexColorModel cm = new IndexColorModel(1, 2, r, g, b);
			destination = new BufferedImage(modifiedPanel.getXsize(),
					modifiedPanel.getYsize(), BufferedImage.TYPE_BYTE_INDEXED,
					cm);
			bi = resize(bi, modifiedPanel.getXsize(), modifiedPanel.getYsize());

			Graphics2D gr = destination.createGraphics();
			RenderingHints hints = new RenderingHints(null);

			RescaleOp op = new RescaleOp(1f, 0f, hints);
			gr.drawImage(bi, op, 0, 0);


			imagePost(comp);

		}
	}

	public void bpp2() {

		BufferedImage bi = toBufferedImage(image);
		int comp=ImagePreamble();
		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		this.add(imagePanel, BorderLayout.CENTER);

		if (bi != null) {
			/*
			 * byte ff = (byte) 0xff; byte qt = (byte) 0x44; byte[] r ={ 0, ff,
			 * 0, ff }; byte[] g ={ 0, 0, ff, ff}; byte[] b = {0, qt, 0, ff};
			 */

			// Derive the palette from the image
			AdaptiveColorPalette mp = new AdaptiveColorPalette(image, 4);
			byte[] r = mp.getReds();
			byte[] g = mp.getGreens();
			byte[] b = mp.getBlues();

			IndexColorModel cm = new IndexColorModel(2, 4, r, g, b);

			System.out.println("Num color comp " + cm.getNumColorComponents());

			destination = new BufferedImage(modifiedPanel.getXsize(),
					modifiedPanel.getYsize(), BufferedImage.TYPE_BYTE_INDEXED,
					cm);

			bi = resize(bi, modifiedPanel.getXsize(), modifiedPanel.getYsize());
			Graphics2D gr = destination.createGraphics();
			RenderingHints hints = new RenderingHints(null);

			RescaleOp op = new RescaleOp(1f, 0f, hints); // do nothing!
			gr.drawImage(bi, op, 0, 0);
			imagePost(comp);
			

		}
	}

	public void bpp4() {

		BufferedImage bi = toBufferedImage(image);
		int comp=ImagePreamble();
		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		this.add(imagePanel, BorderLayout.CENTER);
		
		if (bi != null) {
			/*
			 * byte ff = (byte) 0xff; byte half = (byte) 0x88; byte qt = (byte)
			 * 0x44; // static pretty good 16 color palette byte[] r = { 0, ff,
			 * 0, 0, half, 0, 0, qt, 0, 0, ff, 0, ff, half, half, ff }; byte[] g
			 * = { 0, 0, ff, 0, 0, half, 0, 0, qt, 0, ff, ff, 0, ff, half, ff };
			 * byte[] b = { 0, 0, 0, ff, 0, 0, half, 0, 0, qt, 0, half, half,
			 * qt, half, ff };
			 */

			byte[] r;
			byte[] g;
			byte[] b;
			// Derive the palette from the image
			AdaptiveColorPalette mp = new AdaptiveColorPalette(image, 16);
			r = mp.getReds();
			g = mp.getGreens();
			b = mp.getBlues();

			IndexColorModel cm = new IndexColorModel(4, 16, r, g, b);

			System.out.println("Num color comp " + cm.getNumColorComponents());

			destination = new BufferedImage(modifiedPanel.getXsize(),
					modifiedPanel.getYsize(), BufferedImage.TYPE_BYTE_INDEXED,cm);

			bi = resize(bi, modifiedPanel.getXsize(), modifiedPanel.getYsize());
			Graphics2D gr = destination.createGraphics();
			RenderingHints hints = new RenderingHints(null);

			RescaleOp op = new RescaleOp(1f, 0f, hints); // do nothing!
			gr.drawImage(bi, op, 0, 0);

			imagePost(comp);
			

		}
	}

	public void bpp8() {

		BufferedImage bi = toBufferedImage(image);
		int comp=ImagePreamble();
		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		this.add(imagePanel, BorderLayout.CENTER);
		if (bi != null) {
			/*
			 * byte[] r ; byte[] g ; byte[] b ; // Derive the palette from the
			 * image AdaptiveColorPalette mp = new AdaptiveColorPalette(image,
			 * 256); r = mp.getReds(); g = mp.getGreens(); b = mp.getBlues();
			 **/ 

			/*byte[] r;
			byte[] g;
			byte[] b;
			// Derive the palette from the image
			AdaptiveColorPalette mp = new AdaptiveColorPalette(image, 256);
			r = mp.getReds();
			g = mp.getGreens();
			b = mp.getBlues();

			IndexColorModel cm = new IndexColorModel(8, 256, r, g, b);*/			 

			destination = new BufferedImage(modifiedPanel.getXsize(),
					modifiedPanel.getYsize(), BufferedImage.TYPE_BYTE_INDEXED);
			bi = resize(bi, modifiedPanel.getXsize(), modifiedPanel.getYsize());

			Graphics2D gr = destination.createGraphics();
			RenderingHints hints = new RenderingHints(null);

			RescaleOp op = new RescaleOp(1f, 0f, hints);
			gr.drawImage(bi, op, 0, 0);

			imagePost(comp);

		}
	}

	public void justRisize() {
		BufferedImage bi = toBufferedImage(image);
		int numPixels=destination.getColorModel().getPixelSize();
		int comp=ImagePreamble();
		
		
		//Create the panel to display the images and buttons
		imagePanel = new MyPanel();
		imagePanel.setLayout(new GridLayout(1,2));
		this.add(imagePanel, BorderLayout.CENTER);
		
		if (bi != null) {
			
			destination = new BufferedImage(modifiedPanel.getXsize(),
					modifiedPanel.getYsize(), bi.getType());

			bi = resize(bi, modifiedPanel.getXsize(), modifiedPanel.getYsize());

			Graphics2D g = destination.createGraphics();
			RenderingHints hints = new RenderingHints(null);

			RescaleOp op = new RescaleOp(1f, 0f, hints);
			g.drawImage(bi, op, 0, 0);

            if (numPixels==1) {
            	bpp1();
            } else if (numPixels==2) {
            	bpp2();
            } else if (numPixels==4) {
            	bpp4();
            } else if (numPixels==8) {
            	bpp8();
            } else {
    			imagePost(comp);
            }

		}
	}

	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub

		System.out.println("action even " + arg0);
		// ColorModel cm=destination.getColorModel();
		if (destination != null) {
			int numPix = destination.getColorModel().getPixelSize();
			if ((numPix == 1)) {
				bpp1();
			} else if (numPix == 2) {
				bpp2();
			} else if (numPix == 4) {
				bpp4();
			} else if (numPix == 8) {
				bpp8();
			} else {
				justRisize();
			}
		}

	}

	 public void	focusGained(FocusEvent e) {}
     
	 public void	focusLost(FocusEvent e) {
		 justRisize();
	 }
     
     
     
     
	private static BufferedImage resize(BufferedImage image, int width,
			int height) {
		int type = image.getType() == 0 ? BufferedImage.TYPE_INT_ARGB : image
				.getType();
		BufferedImage resizedImage = new BufferedImage(width, height, type);
		Graphics2D g = resizedImage.createGraphics();
		g.setComposite(AlphaComposite.Src);

		g.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
				RenderingHints.VALUE_INTERPOLATION_BILINEAR);

		g.setRenderingHint(RenderingHints.KEY_RENDERING,
				RenderingHints.VALUE_RENDER_QUALITY);

		g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
				RenderingHints.VALUE_ANTIALIAS_ON);

		g.drawImage(image, 0, 0, width, height, null);
		g.dispose();
		return resizedImage;
	}

	public static BufferedImage toBufferedImage(Image image) {
		if (image == null)
			return (null);
		if (image instanceof BufferedImage) {
			return (BufferedImage) image;
		}

		image = new ImageIcon(image).getImage();
		BufferedImage bimage = null;
		GraphicsEnvironment ge = GraphicsEnvironment
				.getLocalGraphicsEnvironment();
		try {
			int transparency = Transparency.OPAQUE;
			// Create the buffered image
			GraphicsDevice gs = ge.getDefaultScreenDevice();
			GraphicsConfiguration gc = gs.getDefaultConfiguration();
			bimage = gc.createCompatibleImage(image.getWidth(null),
					image.getHeight(null), transparency);
		} catch (HeadlessException e) {
			System.out.println(e.toString());
		}

		if (bimage == null) {
			bimage = new BufferedImage(image.getWidth(null),
					image.getHeight(null), BufferedImage.TYPE_INT_RGB);
		}

		Graphics g = bimage.createGraphics();

		// Paint the image onto the buffered image
		g.drawImage(image, 0, 0, null);
		g.dispose();

		return bimage;
	}

        public Image loadURLImage(String path) {
           Image img = null;
           try {
               //ClassLoader cl = this.getClass().getClassLoader();
               InputStream url = getClass().getClassLoader().getResourceAsStream(path);
               System.out.println("URL:" + url);
               img = ImageIO.read(url); // Null argument exception
           } catch (IOException ex) {
               System.out.println(ex.toString());
           }
           return(img);
       }

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		JFrame jf = new JFrame();
		jf.setName("GRLib GUI");

		ImageConverter iv = new ImageConverter();
		iv.createMenuBar(jf);
		jf.getContentPane().add(iv);
		

		jf.setSize(1300, 800);
		jf.setVisible(true);
		jf.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {

				System.exit(0);
			}
		});

	}

}
