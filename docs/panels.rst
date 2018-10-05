.. _panels:

Panels
======

SPIERSalign uses *Panels* (dockable windows) for much of its interface (see SPIERSedit and SPIERSview manuals). Available panels are *Markers*, *Crop*, *Auto-Align* and *Info*, shortcut keys F1, F2, F3 and F9 respectively.

Info Panel
----------

The *Info* panel (see Figs 2,3) shows the user what mode they are in - e.g. *Normal Align*, *Automated Align*, *Semi-automated Align*, *Propagate*, *Lock*, as well as the cursor's position in pixels from the top left corner, the file name of the current image, and the image's dimensions. It also provides a 'text box' in which the user can enter notes regarding the aligning process; these notes are automatically saved along with the settings file.

Markers Panel
-------------

See Figure 2. The Markers panel provides a list of all markers, plus facilities to add, remove and edit them.

The *Add* and *Remove Marker* buttons allow you to vary the number of markers (there is a lower limit of five). The *Red*, *Green* and *Blue* dials dictate the colour of the markers; this should be chosen to maximise contrast with the images. Note that the currently selected marker is always the inverse colour of the other markers. The marker size spin box allows you to change the size of the circular markers, while the line thickness changes the width of the line for both linear and circular markers.

Markers can be selected by clicking close to them on the image. They can then be dragged into position. They can also be selected by clicking on the markers list in the markers dialogue, which shows the selected marker highlighted in grey. The toggle shape button transforms the marker from a circle to a line or back again. Lines can be moved by clicking and dragging near the middle of the line; clicking and dragging towards the end of the line allows you to position the endpoints as necessary to match your fiduciary markings.

The *Lock Markers* button stops accidental movement of markers; they can be unlocked with a second click of the button.

Automarkers buttons (*Auto Markers*, *AM: Align* and *AM: Edit Grid*) are dealt with under the *Semi-Automated Align* section below.

Crop Panel
----------

*Crop mode*: Bring the *Crop* Panel up puts SPIERSalign into crop mode (see Fig. 3). A white rectangle is overlaid on the image; this represents the ‘crop box'. A message will appear in the status bar with the dimensions of the crop area; this is also provided in spin-boxes on the Crop panel, where it can be entered precisely. The crop-box defines the region of interest. Everything outside of this box will be removed from the finalised files that SPIERSalign creates in the 'cut' directory (i.e. the ones that SPIERSedit will work with). Users should ensure that all object(s) of interest are is contained inside the crop-box **on every slice**. To check this navigate backwards and forwards in the normal way whilst in crop mode. The crop box can be both resized and moved using the mouse, but for fine adjustments shortcut keys can be used. Crop-box colour can be set in the Crop panel to maximise contrast with the image.

*Cropping a Dataset*: Once all images have been aligned and a crop-box has been set, the Crop button on the Crop panel initiates the cropping procedure. SPIERSalign will ask for first the starting and then the finishing slices of the crop (this allows users to exclude regions at the start or end of a dataset that do not contain the object(s) of interest). SPIERSalign then asks for the file format to generate; this can be JPEG, PNG, BMP or TIFF. Use of PNGs of BMPs is recommended in most circumstances; PNG files are relatively small and are compressed with a lossless algorithm. Saving as a JPEG produces extremely small files but involves ’lossy’ compression which may subtly damage data; while artefacts are likely to be minimal, use of JPEGS is not recommended unless disk-space is an overriding concern. BMPs are saved in uncompressed format, and are typically over three times the size of PNGs; they may, however, load faster in SPIERSedit on systems with high-speed storage systems as they do not need to be decompressed. Thus they are the best choice in some circumstances. TIFFs cannot be imported into SPIERSedit, but may be required by other tomographic software. Once the format has been selected the program will work through the dataset, cropping the images to the required size and placing them in a ‘cut’ folder in the working directory. This process may take a few minutes. The images can then be checked and loaded into SPIERSedit as a new dataset. If there are any problems with the images, crop can be performed again until you are happy with the dataset.
Compressing A Dataset: Cropping will not alter the original image stack, remove the temporary .xxx images, or delete the settings file. This allows a revised alignment and second crop, or definition of a second region of interest. If you wish to remove these files once you have finished with the dataset (the temporary images can take up a lot of disk space), use the Compress Dataset command on the File menu. This command removes the temporary images (but not the settings file) and also converts the original images to PNG format. The PNG images will be exactly the same resolution and quality as the originals, but occupy less storage space. The settings file can be reapplied at a later date if further changes are required.

Auto Align Panel
----------------

See *True Automated Align* section