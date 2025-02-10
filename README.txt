Melissa Duanmu mduanmu@u.rochester.edu
Last modified: 10 Feb 2025
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

"What is a valid intersection?"
- The ray is not within the plane.
- The ray does not enter vertex and enter vertex.
    - Vertices are not a "face", and thus not valid intersections.
- The ray intersects with a plane.

Sample output:
------------------------------------------------------------------
x y z coordinates of the FLD vertex (eg, 0 0 0 for origin): 0 0 0
Length of cube: 3
x y z coordinates of ray origin: 3 6 -2
x y z coordinates of ray direction: 1 1 1
Closest intersection is at U face: 1.800000 3.000000 -0.200000
------------------------------------------------------------------

Arbitrary limitations (quick change):
- The total characters for the coordinates must not exceed 14 chars.

Other limitations (slow change):
- The cube is only at one angle in 3D space. That is, standing flat
  on the x-axis. I believe the calculation part doesn't care what
  rotation the cube is at, but the cube initialization part would
  have to be edited in order to solve this limitation. Tentatively,
  I would say that the algorithm is complete, but not the program.