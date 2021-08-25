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
package com.ti.msp430.HelpBroker;

import java.util.*;
import java.util.List;
import java.awt.*;
import java.io.*;
import java.net.*;

import org.jdom2.*;
import org.jdom2.input.*;

import com.sun.xml.internal.txw2.Document;

/**
 * This class is used to display html help files.
 *
 * Individual html files are mapped to id's using a "idMap.xml" file that is assumed to be
 * in the same directory as the html files.
 *
 * The example files in the helpfiles directory illustrate how doxygen can be used to
 * generate a rich help file environment.
 * 
 * Note that the Desktop.browse method will not correctly extract URL that are in jar files
 * so you must specify an basePath to this class that will exist outside the jar.
 *
 * @author a0182881
 */
public class HelpBroker {

    public String TOP = "top";
    public String MAP = "idMap.xml";
    private String baseHelpPath;
    private TreeMap<String, String> idMap;

    /**
     * Constructor.
     *
     * @param url - path to the location of the html files
     */
    public HelpBroker(String helpPath) {
        idMap = null;

        this.baseHelpPath = helpPath;
        this.idMap = new TreeMap<String, String>();
        buildIdMap();
    }

    /**
     * Private function to create a URL to a file in the baseHelpPath directory.
     *
     * If null is provided as an argument, then a URL to the baseHelpPath directory itself is
     * created.
     *
     * @param element
     * @return
     */
    private URL getUrl(String element) throws MalformedURLException {
        if (element != null) {
            return new URL("file://" + baseHelpPath + "/" + element);
        }
        return new URL("file://" + baseHelpPath);
    }

    /**
     * Read the idMap.xml file and build the id to html map tree.
     */
    private void buildIdMap() {

        URL thebaseHelpPath;
        try {
            thebaseHelpPath = getUrl(null);
        } catch (MalformedURLException ex) {
            System.out.println(ex.toString());
           
        }

        SAXBuilder builder = new SAXBuilder();

        String xmlPath = baseHelpPath + "/" + MAP;
        File xmlFile = new File(xmlPath);
        if (xmlFile.exists()) {


            try {
                //Document document = builder.build(input);
                Document document = builder.build(xmlFile);
                org.jdom2.Element rootNode = document.getRootElement();

                List<org.jdom2.Element> mapIdList = rootNode.getChildren("mapID");
                for (int k = 0; k < mapIdList.size(); k++) {
                    org.jdom2.Element aMap = mapIdList.get(k);
                    String aTarget = aMap.getAttribute("target").getValue();
                    String aUrl = aMap.getAttribute("url").getValue();


                    try {
                        URL theUrl = getUrl(aUrl);
                        if (theUrl != null) {
                            idMap.put(aTarget, baseHelpPath + "/" + aUrl);
                        }
                        else {
                        }
                    } catch (MalformedURLException ex) {
                    }
                }
            } catch (JDOMException ex) {
		System.out.println(ex.toString());
            } catch (IOException ex) {
		System.out.println(ex.toString());
            }
        } else {
        }
    }

    /**
     * Look up the given map id and display it
     *
     * @param helpID
     */
    public void setCurrentId(String helpID) {
        Desktop desktop = Desktop.getDesktop();

        String urlStr = idMap.get(helpID);
        if (urlStr != null) {
            File idUrl = new File(urlStr);
            String path = idUrl.getAbsolutePath().replaceAll("\\\\", "/");
            
            try {
                desktop.browse(new URI("file://" + path));
            } catch (URISyntaxException ex) {
		    System.out.println(ex.toString());
            } catch (IOException ex) {
		    System.out.println(ex.toString());
            }
        }
    }

    /**
     * Return a JLabel link that when clicked, will take you to the helpId
     * 
     * @param labeltext
     * @param helpID
     * @return
     */
    public HtmlLinkLabel getHtmlLinkLabel(String labeltext, String helpID) {
        HtmlLinkLabel linkLabel = null;

        String urlStr = idMap.get(helpID);
        if (urlStr != null) {
            File idUrl = new File(urlStr);
            String path = idUrl.getAbsolutePath().replaceAll("\\\\", "/");
            linkLabel = new HtmlLinkLabel(labeltext, path);
        }
        return linkLabel;
    }

    /**
     * Displays the top help page
     */
    public void setTopId() {
        setCurrentId(TOP);
    }

    /**
     * Test main
     *
     * @param args
     */
    public static void main(String args[]) {

        // Remember to go into the "helpfiles" directory and type "make" to generate the 
        // html directory contents

        String baseHelpPath = "helpfiles/html";
        HelpBroker helpBroker = new HelpBroker(baseHelpPath);

        helpBroker.setTopId();
        helpBroker.setCurrentId("chapter2");

    }
}
