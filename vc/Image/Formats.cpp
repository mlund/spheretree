/*************************************************************************\

                             C O P Y R I G H T

  Copyright 2003 Image Synthesis Group, Trinity College Dublin, Ireland.
                        All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following paragraphs appear in all copies.


                             D I S C L A I M E R

  IN NO EVENT SHALL TRININTY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
  BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
  AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGES.

  TRINITY COLLEGE DUBLIN DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
  COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
  ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted at the following e-mail addresses:

          Gareth_Bradshaw@yahoo.co.uk    isg@cs.tcd.ie

  Further information about the ISG and it's project can be found at the ISG
  web site :

          isg.cs.tcd.ie

\**************************************************************************/

#include "Formats.h"

const char *formatsRead =
    "All Files (*.*)|*.*|"
    "Alias/Wavefront RLE (*.pix, *.rla)|*.pix; *.rla|"
    "Amiga (*.iff, *.ilbm)|*.iff; *;ilbm|"
    "Apple QuickDraw (*.pic, *.pict)|*.pic; *.pict|"
    "Compuserve Graphics Interchange (*.gif)|*.gif|"
    "Computer Graphics Metafile (*.cgm)|*.cgm|"
    "FlashPix (*.fpx)|*.fpx|"
    "Flexible Image Transfer System (*.fits)|*.fits|"
    "Irix RGB (*.sgi)|*.sgi|"
    "Joint Bi-level Image Expert Group (*.jbig)|*.jbig|"
    "Joint Picture Expert Group (*.jpg, *.jif, *.jpeg)|*.jpg; *.jif; *.jpeg|"
    "Khoros Visualisation (*.vif, *.viff)|*.vif; *.viff|"
    "Microsoft Windows Bitmap (*.bmp, *.dib)|*.bmp; *.dib|"
    "PSX TIM (*.tim)|*.tim|"
    "Photo CD (*.pcd)|*.pcd|"
    "Pilot Image (*.pdb)|*.pdb|"
    "Portable Anymap (*.pnm)|*.pnm|"
    "Portable Bitmap (*.pbm)|*.pbm|"
    "Portable Greymap (*.pgm)|*.pgm|"
    "Portable Network Graphics (*.png, *.mng)|*.png; *.mng|"
    "Portable Pixelmap (*.ppm)|*.ppm|"
    "Radiance (*.rad)|*.rad|"
    "SciTex Continuous Tone (*.sct)|*.sct|"
    "Seattle File Works (*.pwp, *.sfw)| *.pwp; *.sfw|"
    "SUN Rasterfile (*.sun)|*.sun|"
    "Tagged Image File (*.tif, *.ptif)|*.tif; *.ptif|"
    "Truevision Targa (*.tga)|*.tga|"
    "Utah RLE (*.rle)|*.rle|"
    "Windows Metafile (*.wmf)|*.wmf|"
    "Word Perfect Graphic (*.wpg)|*.wpg|"
    "X-Windows Pixmap (*.xpm)|*.xpm|"
    "X-Windows Window Dump (*.xwd)|*.xwd|"
    "ZSoft IBM PC Paintbrush (*.pcx,*.dcx)|*.pcx; *.dcx||";

const char *formatsWrite =
    "All Files (*.*)|*.*|"
    "Amiga (*.iff, *.ilbm)|*.iff; *;ilbm|"
    "Apple QuickDraw (*.pic, *.pict)|*.pic; *.pict|"
    "Compuserve Graphics Interchange (*.gif)|*.gif|"
    "Computer Graphics Metafile (*.cgm)|*.cgm|"
    "FlashPix (*.fpx)|*.fpx|"
    "Flexible Image Transfer System (*.fits)|*.fits|"
    "Irix RGB (*.sgi)|*.sgi|"
    "Joint Bi-level Image Expert Group (*.jbig)|*.jbig|"
    "Joint Picture Expert Group (*.jpg, *.jif, *.jpeg)|*.jpg; *.jif; *.jpeg|"
    "Khoros Visualisation (*.vif, *.viff)|*.vif; *.viff|"
    "Microsoft Windows Bitmap (*.bmp, *.dib)|*.bmp; *.dib|"
    "PSX TIM (*.tim)|*.tim|"
    "Photo CD (*.pcd)|*.pcd|"
    "Portable Anymap (*.pnm)|*.pnm|"
    "Portable Bitmap (*.pbm)|*.pbm|"
    "Portable Greymap (*.pgm)|*.pgm|"
    "Portable Network Graphics (*.png, *.mng)|*.png; *.mng|"
    "Portable Pixelmap (*.ppm)|*.ppm|"
    "Radiance (*.rad)|*.rad|"
    "SciTex Continuous Tone (*.sct)|*.sct|"
    "Seattle File Works (*.pwp, *.sfw)| *.pwp; *.sfw|"
    "SUN Rasterfile (*.sun)|*.sun|"
    "Tagged Image File (*.tif, *.ptif)|*.tif; *.ptif|"
    "Truevision Targa (*.tga)|*.tga|"
    "Windows Metafile (*.wmf)|*.wmf|"
    "Word Perfect Graphic (*.wpg)|*.wpg|"
    "X-Windows Pixmap (*.xpm)|*.xpm|"
    "X-Windows Window Dump (*.xwd)|*.xwd|"
    "ZSoft IBM PC Paintbrush (*.pcx,*.dcx)|*.pcx; *.dcx||";
