.. _datadownsampling:

Dataset Downsampling
====================

Concept
-------

Dataset downsampling is explained in the Basic Concepts section (above).

Changing dataset downsampling
-----------------------------

The current dataset downsampling levels can be changed using the *Change
downsampling* command on the *File* menu. This opens a dialog that
prompts for a new Z (sparsity) factor, and a new XY factor. Either or
both can be changed. If the Z factor is decreased, previously unused
slices will be brought into play. The user has the option of whether or
not to interpolate masks (and locks/selects) and curves onto these
slices; if the appropriate box is selected, masks, locks and curves are
created for new slices by copying from one of the existing adjacent
slices. If not, these are left blank (or any pre-existing data used,
e.g. from before a downsample had taken place). If in doubt, it is best
to interpolate.

Changing downsampling on the fly like this is allowed for, but is not to
be taken lightly – it is a major operation, which takes some time to
complete, and will result in either the discarding of data
(downsampling) or potential differences in editing of areas (upsampling
with later editing of certain regions).

Dataset downsampling for rough masking
--------------------------------------

As mentioned in Basic Concepts, an entire dataset can be masked while
downsampled to some low resolution, and then the dataset upsampled. This
will result in blocky-edged masks, but so long as these edges are in
space not on the specimen this will not matter. After doing this all
slices will need to be re-generated at the new (higher) resolution, so
all threshold corrections done with brightening or segment drawing will
be lost. *If a user intends to upsample at any point prior to outputting
the finished model, they should put off any such editing until AFTER
upsampling.*