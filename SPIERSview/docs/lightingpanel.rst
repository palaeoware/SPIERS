.. _lightingpanel:

Lighting Panel
==============

SPIERSview 4 introduced an improved lighting and rendering system, capable of rendering models with shadows, and 
of using multiple light sources. The lighting panel provides controls for this system.

Lighting is split into three light sources:

1) 'main' light, by default placed in north-west, casting soft shadows

2) 'secondary' light, by default slightly weaker than main, and placed east, without shadows

3) 'headlight', always placed at the camera position and pointing forwards. This never casts shadows, and is off by default

Note that previous versions of SPIERSview only implemented a headlight; the main and secondary lights are new to version 4.

Colour
------
All lights by default emit white light. Click the colour box if you wish to modify this to coloured light. Some bizarre effects are possible, though not recommended.

Light positioning
-----------------
Main and secondary lights have position controls. Use the XY dial in conjunction with the Z slider to control the position of the light. The XY dial rotates the light position around the direction of view (i.e. around the Z direction). The Z slider controls the distance of the light from the model in the Z direction - set the slider to the top of the range for a light positioned to the side of the model, and to the bottom of the range for a light positioned near the view position.

Shadows
-------
Main and secondary lights can optionally cast shadows. Depending on the nature of the model, these may either assist or interfere with interpretation. Shadows can be set to 'off', 'hard shadows', or 'soft shadows'. Soft shadows a more realistic and avoid hard-edge shadow pixellation artefacts, but are more computationally expensive. Any shadows introduce computational cost - if the frame rate in SPIERSview is low, consider turning shadows off for all lights.

Light power
-----------
The strength of each light source can be controlled with the power slider. Use this if your model is too bright or too dark under current lighting.

Light activation
----------------
The secondary light and headlight can be switched on and off - the main light is always active. Switching a light off entirely removes its computation cost, so use this approach (rather than just dropping light power) if you need to improve rendering performance.

