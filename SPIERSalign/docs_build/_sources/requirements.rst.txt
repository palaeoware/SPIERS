.. _requirements:

Requirements
============

System
------

SPIERSalign has no minimum requirements as such, and will run on most systems; it has, however, not been tested on versions of Windows older than Windows XP. The Mac OSX version requires OSX 10.5. Performance will benefit from high processor speed, a fast disk system, and a large high resolution display. A minimum of 512Mb of memory is recommended on Windows XP, 1Gb+ on Vista/7. Memory in SPIERSalign will act as a file-cache, enabling images to load almost instantaneously after the first time they are viewed; A large amount of RAM will enable more files to be cached, which may speed aligning work. A 64-bit version is available; unlike SPIERSedit and SPIERSview however, there are unlikely to be performance or memory usage problems with the 32-bit version on any dataset; the 64-bit version is provided simply to allow a clean 64-bit install of the entire SPIERS suite. Several gigabytes of free space is recommended. A working dataset can be large; for example, high resolution serial photograph datasets can reach 3 Gb in size, and modern CT datasets can be in excess of 15 Gb.

Data
----

SPIERSalign works with windows bitmap format files (ending with a .bmp extension), PNGs (.png), TIFFs (.tif/.tiff) and jpegs (.jpg/.jpeg). Files can be either 24-bit RGB or 8-bit greyscale, and of any resolution. The dataset can also comprise images of different resolutions (although this is not recommended). There are no restrictions on filenames: an identifier followed by a three or four digit number (e.g. mydata0001.bmp) is recommended for clarity. Note that the program will order image files alphabetically, and hence that leading zeroes are required on filenames (e.g. image0001.bmp rather than image1.bmp). All images must be placed in a single directory.
