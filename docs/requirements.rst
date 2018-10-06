.. _requirements:

Requirements
============

System Requirements
-------------------

SPIERSedit has no minimum requirements as such, and will run on most
systems; it has, however, not been tested on versions of Windows older
than Windows XP. Performance will benefit from high processor speed and
a large amount of system RAM; 2Gb is a recommended minimum. As of v2.20,
only 64-bit operating systems are supported. Graphics card speed is not
relevant (although it is to SPIERSview). A fast hard disk is
recommended; SPIERSedit is I/O intensive, and hard-disk speed will be
the principle constraint on speed for many bulk-file operations. Large
datasets can easily exceed 5Gb in size, so hard disk size may also be an
issue.

*Mac* *OSX:* OSX 10.5 or higher is required. SPIERSview makes use of the
right mouse-button; on single-button Mac OSX systems use the
control-click combination to simulate right-clicking.

A high quality optical mouse or digitising tablet is strongly
recommended for editing work.

Data requirements
-----------------

SPIERSedit can work with any dataset consisting of serial ‘slice’
images, so long as the resolution and scale of each image is the same,
and no images are missing from the sequence. Datasets should comprise
registered (aligned) serial images, ideally in uncompressed Windows
Bitmap (.bmp) format. SPIERSedit can also work with datasets in Portable
Network Graphic (.png) or JPEG (.jpg) format; these take up less
hard-disk space, but will normally result in slower processing; JPEG
datasets will be considerably more compact than Portable Network Graphic
datasets, but are not recommended as JPEG compression causes artefacts
which can degrade data and hence final models.

*Format*: Images can be 8-bit grayscale (.png, .bmp) or 24-bit colour
(.bmp, .png, .jpg). Images that lack colour information (e.g. CT data)
should be converted to an 8-bit rather than 24-bit format – you will not
see a difference visually, but the software will run substantially
faster.

*Names and directory:* Although SPIERSedit will load any file sequence
in alphabetical/numerical order, for clarity it is recommended that
images should be serially numbered with three or four digits before the
file extension (e.g. 0001.bmp, 0002.bmp etc). Images must all be in the
same directory (called the *working directory* subsequently). If the
dataset has been prepared using SPIERSalign, this will be the ‘cut’
directory underlying the SPIERSalign working directory. Note that
changing filenames once the dataset has been created will effectively
corrupt SPIERSedit’s data.

*Size and cropping:* Images can be any size (resolution), but a key
restriction is that *all images* in the sequence *must be the same
resolution*. The speed of many SPIERSedit operations will be
proportional to resolution; cropping of the image sequence before
loading into SPIERSedit is hence strongly recommended. SPIERSalign can
be used to perform this croppping, and hence it is normal to pass even
pre-registered data (e.g. from a CT scanner) through SPIERSalign before
loading into SPIERSedit. Note that once editing work in SPIERSedit has
begun it is not possible to re-crop images without all work being lost.