.. _appendixavtkusage:

Appendix A – VTK Usage
======================

Introduction
------------

VTK (the Visualisation Toolkit; http://www.vtk.org/) is a an
open-source, freely available software system for 3D computer graphics,
image processing and visualisation. SPIERSview makes use of VTK version
8.1 for some data-processing purposes (although not for visualisation,
which uses custom code to increase speed and memory efficiency). Certain
VTK classes used are documented here, where knowing details of
algorithms used may help a user understand exactly what SPIERSview is
doing with their data. Interested users are referred to the VTK
documentation to interpret the notes below.

Fidelity Reduction
------------------

SPIERSview default ‘fast and safe’ fidelity reduction uses the
*vtkDecimatePro* filter. *AccumulateError* is set to ‘off’,
*PreserveTopology* is set to on; no other settings are changed from
defaults.

SPIERSview ‘Quadric’ fidelity reduction uses the *vtkQuadricDecimation*
filter. No settings are changed from defaults.

Island Removal
--------------

Island removal uses the *vtkPolyDataConnectivityFilter* class.
*ColorRegions* is set to off; no other settings are changed from
defaults. ‘All’ mode uses *SetExtractionModeToLargestRegion*; otherwise
the filter is run in *SetExtractionModeToAllRegions* mode. Tiny, Small,
Medium and Large settings remove regions of up to 20, 100, 600 and 4000
triangles in size respectively.

Smoothing
---------

Smoothing uses multiple passes of the *vtkWindowedSincPolyDataFilter*
filter. *FeatureEdgeSmoothing*, *BoundarySmoothing*,
*GenerateErrorScalars* and *GenerateErrorVectors* are set to off, and
PassBand is set to 0.05. Very Weak, Weak, Medium, Strongish, Strong, and
Strongest settings use 5, 10, 20, 40, 60, and 100 iterations
respectively.

STL reader
----------

SPIERSview uses the *vtkSTLreader* class to import STL triangle meshes
during VAXML import; its restrictions and capabalities are thus those of
this VTK class.

PLY reader
----------

SPIERSview uses the *vtkPLYreader* class to import PLY triangle meshes
and surface colour information during VAXML import; it’s restrictions
and capabalities are thus those of this VTK class; in particular colour
information input is restricted to ‘red’, ‘green’ and ‘blue’ properties
of vertex elements.