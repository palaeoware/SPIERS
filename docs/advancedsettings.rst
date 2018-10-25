.. _advancedsettings:

Advanced Settings File Usage
============================

Loading Backups
---------------

A backed-up settings file (see above) can be loaded and applied to the dataset using the Load Settings File command on the File menu. This operation overwrites the current settings file (which should be backed-up beforehand), and applies the settings to the image list. This will only work with the original dataset; if there have been any changes to the file names or any additions to or deletions from the dataset, it will fail.

Constant Autosave
-----------------

If SPIERSalign crashes after a long period of work, it is possible that changes since the last save will be lost. This loss may not be apparent until you next try and transform any given image, at which point the new transformation will be applied to the original image, not the altered .xxx file which will have been loaded on initial viewing. To guard against this, the Constant Autosave option in the File menu is provided; with this selected SPIERSalign will resave settings.txt after every transformation, removing the risk of losing unsaved work. This should make no noticeable difference in performance on all but the oldest PCs, and so is checked as standard.