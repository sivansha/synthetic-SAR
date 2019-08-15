### Synthetic SAR data set

Estimation of DEM from SAR require conventially at least two passes.<br>
in the [paper](https://arxiv.org/pdf/1803.05387.pdf) "Towards Monocular Digital Elevation Model (DEM) Estimation by Convolutional Neural Networks - Application on Synthetic Aperture Radar Images"<br>
The authors managed to achive good results by training a CNN to estimate DEM from single SAR. However, in order to imptrove those results more data is needed.<br>
This code generates data set of synthetic DEM and SAR couples in order to train a CNN.<br><br>

### details:
- DEM is created
- The DEM is reflectd to emulate radar reflection
- The DEM and the reflection are projected to satellite coordinates.
- speckle is added. <br>


The projected DEM and the projected reflection are the data pair, the final goal is to train CNN predict the DEM from the SAR. 
<br>

The Perlin noise module is from: [Solarian Programmer](https://solarianprogrammer.com/2012/07/18/perlin-noise-cpp-11/) and it is under GPL 3 license. 