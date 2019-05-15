Introduction:

Welcome to this little playthrough of Alice in Wonderland.
We set the scene by starting off in the room in which Alice 
sees the white rabbit dissapearing through a door. The only 
problem is that the door is the size of a mouse and Alice can't 
get through! Help her find a way out of the room or she'll be trapped
here forever. 

Good luck. 

Controls: WASD + spacebar to move, left mouse button to interact with the 
environment and right mouse click to rotate the camera view. 

*********************************************************************

Compilation:

Unzip the file into a folder that will be the home directory. 
In this directory, run **premake4 gmake** and **make**.
Then enter the A5-FinalProject directory and once again, 
run **premake4 gmake** and **make**. 
The executable can then be run using **./Final**, followed by the 
commandline argument **Assets/a5ProjectScene.lua**. 


This project was tested in the graphics lab on gl39.


*********************************************************************

Manual:

On the top left corner, we have a window that can be toggled between
three different displays. In the Interactions tab, we have messages 
popping up that come from clicking the different items in the room. 
Some of these are hints to help get through the game while others 
are present for atmospheric purposes. Next we have the Inventory 
tab which lists the items acquired while in the room. Finally we have
the Options tab which allows toggling of certain graphical/physics
techniques in the game. 

Changes were made to ObjFileDecoder and mesh consolidator in the cs488-framework
file to make texture mapping and collision detection possible.

Textures for the various objects in the room are present in the textures
folder. The program requires loading and assigning variables to map new 
textures so if new textures are desired, the program must be adjusted for 
accordingly. Also note for 3D objects, texture coordinates dictate how 
textures get mapped. For nearly all of the textures used, a special 
form of editing was done to get a picture to map onto a single face as 
opposed to the whole object. 

For the cube and room, a different obj file was created since the two 
have opposing normals (cubes face out, room faces in). To map textures 
to this, a 2:3 image was created that had the same picture in all 6 
quadrants. Alternatively, if multiple faces were visible to the viewer, 
a "hidden" colour was chosen such that if this colour was detected by the
fragment shader, it would render the mesh's colour beneath. We see this
with the room. 

Three objectives were not completed: motion blur, reflection map
and particle system. 
Friction was attempted but was not completed. The final result caused
the player to snap back to their starting point as soon as they let 
go of the direction arrow. The code for this is present in Final.cpp
in lines 784 - 887. The idea was to first obtain the velocity of 
the player in both directions. Keeping track of the present node 
we are standing on, the next step was to incorperate the coefficient
of friction and deceleration values form the node. I then tried calculating the 
location the player should move to by using various motion equations described
in the final document. 


*********************************************************************

Objectives: 
1:  A scene has been modelled.
2:  A user interface has been implemented.
3:  A texture mapping was used in the scene.
4:  Shadows have been implemented using a z-buffer, 
	allowing objects to cast shadows on other objects.
5:  A static collision effect occurs between two or more objects.
6:  A post-processing method of motion blur is demonstrated 
	on an object or a scene.
7:  Alpha-blended transparency can be seen on an object.
8:  A reflection map has been created.
9:  A particle system is demonstrated in the scene.
10: A physics engine has been implemented to mimic gravity and friction
