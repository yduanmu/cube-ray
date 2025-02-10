Melissa Duanmu mduanmu@u.rochester.edu
Last modified: 09 Feb 2025
cube-ray README.txt

To compile:
gcc -o Main -std=c99 -Wall -Werror -fsanitize=address main.c

To run:
./main

Put in coordinates like: a b c
  must have space between each number, but no extra space.

For length, must be like: a
  and no extra space, either.

When done, enter: stop
  when it asks the coordinates of the FLD vertex.

"What does FLD mean?"
  Where the front, left, and down plane intersect. See diagram.png

Sample output:
------------------------------------------------------------------
x y z coordinates of the FLD vertex (eg, 0 0 0 for origin): 0 0 0
Length of cube: 3
x y z coordinates of ray origin: 3 6 -2
x y z coordinates of ray direction: 1 1 1
Intersection at U face: 1.800000 3.000000 -0.200000
------------------------------------------------------------------

Arbitrary limitations (easy to change):
- The total characters for the coordinates must not exceed 14 chars.

Other limitations (moderate to change):
- The cube is only at one angle in 3D space. That is, standing flat on the x-axis. I could add 
  another parameter to initialize the cube at a different rotation, and it would require
  minimally (if any all) edits to the calculation process, which is the most important anyways.