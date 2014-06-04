# LeapBoxTransform

Paraview Leap Motion and vtkTransform Integregration

-----------------

More specificially this is a Paraview "Filter Plugin" which allows full control
of the vtkTransformFilter through Gestures as well as Mouse Interaction.

# Details

Leap Motion realeased a second version of their tracking software which is much
more precise then the version being used now. This Plugin uses version 1 or the 
tracker software. An update should happen fairly soon.

------------------

The Leap Directory contains the leap dynamic library being used

# Compilation

First of all go into linkExtra and add the path to the LeapMotion dynamic library

------------------

Next: mkdir build; cd build; ccmake ../; During this step you will need to add the
path to your paraview build directory.

Next: make; then go back one directory; cd ../; and run the addExtraLink script; ./addExtraLink.sh

