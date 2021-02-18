.. _basicaligningprocedure:

Basic Aligning Procedure
========================

A SPIERSalign session in which you wish to align a set of 100 unregistered images will normally involve the following stages:

1) Open a series of images and select one (arbitrarily) to act as the datum – all other images will be rotated/shifted/scaled to line up with this one. Typically, the datum image is near the centre of the run; this example will assume that image 50 out of 100 has been selected.
2) Set up lines and/or circles on image 50 to mark the fiduciary points (see Markers Panel below). These markers don’t actually do anything, but provide visual reference points that can be used as a guide for alignment.
3) Move to image 51 (see *Navigating Datasets*), and perform rotate, shift and if necessary rescale operations until the image position fits the fiduciary markings (see *Shifting, Rotating and Scaling Images*). It is advisable also to flick repeatedly between image 50 and 51 to check visually that alignment has been achieved.
4) Repeat step 3 for image 52 (checking against image 51), and continue until the end of the run (image 100) has been reached.
5) Return to the datum image (50), and repeat steps 3 and 4, this time going backwards through the sequence (aligning 49 to 50, 48 to 49 etc.)
6) When happy with alignment (it is advisable to run through the entire sequence to ensure that all movement is as smooth as possible), enter crop mode (see *Crop Panel*) and resize it, ensuring that the object of interest is contained within the box on every image.
7) Finalise this process by selecting crop. SPIERSalign will process the images, producing sub-directory with the aligned and cropped images ready for SPIERSedit.