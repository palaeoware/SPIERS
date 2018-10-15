.. _advancednavigation:

Advanced Navigation
===================

Hiding Images
-------------

Occasionally users may find an image in the sequence that they wish to hide temporarily (for example they wish to visually compare images 10 and 12 by flicking between them, without 11 always appearing in the way). To hide the currently viewed image use the *Hide Image* command on the *Navigate* menu. The image will remain on screen (the status bar notes that it is hidden), but if the user navigates forward and then back again with the ‘.’ and ‘,’ keys they will find that the hidden image is skipped. Using *Ctrl-Shift-'.'* and *Ctrl-Shift-','* to navigate will move to the next/previous image whether or not it is hidden; hidden images can also be accessed using both the spin box and horizontal file slider. To unhide the currently viewed image use the *Hide Image* command on the *Navigate* menu. An *Unhide All* command is also available on the *Navigate* menu.

Hidden images are still processed during cropping and propagation; it is advisable to perform an Unhide All operation before cropping to ensure that objects of interest do not extend outside the crop-box on hidden images.

Propagation
-----------

SPIERSalign provides a facility to record changes made to one image, and then apply those changes to all subsequent or previous images in the sequence. This feature is useful, for instance, where one image has been omitted during alignment and all of the changes made to this need to be applied to the subsequent (aligned) images. To use this mode select the Propagation Mode item on the Locking/Propagation menu **while viewing the image from which you want to apply the changes**. SPIERSalign will ask you whether you want to propagate forwards (to the end of the dataset) or backwards (to the beginning). Propagation mode limits navigation to the propagation image and one either before it (forwards propagation) or after it (backwards propagation). Only the propagation image can be rotated, scaled or shifted. Once all changes to the propagation image have been made, the *Apply Progagation* command on the *Locking/Propagation* menu will then apply the changes to the required images.

Propagation mode (forward flavour) can be used on the first image of a dataset to make changes to the entire dataset; for instance to adjust size or orientation.

File Locking
------------

SPIERSalign provides a 'file locking' feature (*Lock Forward* or *Lock Backward* on the *Locking/Propagation* menu). When activated, this mode restricts navigation to just two images, and only enables rotations, scales or shifts to be applied to one of these. *Forward Locking* allows changes to be performed on the second image of the pair, and *Backward Locking* on the first. The *Move Forward/Back* command on the *Locking/Propagation* menu moves the pair either forward or back in the dataset, depending on the mode. This mode is intended as an aid to alignment, enabling a user to work through the dataset one slice at a time without accidentally moving the wrong image. Note that when viewing the locked image of the pair, the Lock File option in the Locking/Propagation menu will be ticked; unticking this temporarily overrides the locking.

Swapping Files
--------------

Occasionally you may find that two images are the wrong way round in the sequence, and need to be swapped over. The *Swap Image* command on the *Transform* menu will swap the current image with the next one. Note that it is possible to move images to any desired position in the sequence by successive swap operations.