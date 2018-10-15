.. _requirements:

System Requirements and Triangle Count
======================================

System Requirments
------------------

SPIERSview has no minimum requirements as such, and will run simple
models on most systems. Performance will benefit from high processor
speed, a large amount of system RAM, and a fast graphics card with a
large amount of onboard RAM.

*Linux*: [tba]

*Windows*: Windows 7 or higher is recommended, and **a 64-bit version of
the operating system is required**.

*Mac* *OSX:* OSX 10.5 or higher is required. SPIERSview makes use of the
right mouse-button; on single-button Mac OSX systems use the
control-click combination to simulate right-clicking.

Triangle Count
--------------

SPIERSview measures the complexity of models in thousands of triangles
(KTr). At the time of writing, typical workstation systems with 4Gb of
RAM can comfortably cope with models of up to 25-30,000 KTr, while basic
laptop systems may struggle with models of over 3,000 KTr. Difficulty
with a model can manifest itself as either slow responses (a slow frame
rate, see bottom left of status bar – anything less than 5 frames per
second is slow), or crashing. Approaches to the reduction of triangle
count in SPIERSedit exports include avoiding ‘speckly’ data (see
SPIERSedit manual), the use of restricted numbers of slices, and the use
of downsampling or ‘binning’. SPIERSview itself can simplify models
using one of two ‘fidelity reduction’ algorithms (see below); using
these it may be possible to use a powerful system to convert a difficult
model to a lower triangle count model that can be handled by
lower-powered systems.