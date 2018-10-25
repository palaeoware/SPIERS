.. _startingspiersview:

Starting SPIERSview
===================

Opening a file
--------------

SPIERSview uses a single-document model – it only views one file at
once. To view another files you simply open multiple copies of the
program, and to close a file you close the program.

SPIERSview can be launched in one of two ways

1) Launching the SPIERSview program directly: this will bring up a
dialog allowing you to select the file you want to load. If you cancel
the load dialog, the program will close.

2) (Recommended) Double-clicking a ‘.spv’, ‘.spvf’, ‘.vaxml’ or ‘.sp2’
file will launch SPIERSview on this data.

Initial processing
------------------

When launched, SPIERSview first processes the data to generate viewable
objects, first calculating ‘isosurfaces’ (3D triangle-meshes) from
slice-based data using the Marching Cubes algorithm, and then by the
conversion of these into a viewable format; this entire process may take
anything from a second or two to several minutes, and depends on the
type of file opened (see above) and the complexity of the objects. Note
that for VAXML files, these tasks may include the downloading of STL or
PLY data from a remote server, which may be slow. Progress bars show the
progress of this work; the top bar represents overall progress, and the
lower bar the progress of each subtask. Note that the user can begin
interacting with the model before once the first object has been
loading; subsequent objects will appear as they become available.