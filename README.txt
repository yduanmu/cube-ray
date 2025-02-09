Melissa Duanmu mduanmu@u.rochester.edu
Last modified: 9 Feb 2025
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

Arbitrary limitations (easy to change):
- The total characters for the coordinates must not exceed 14 chars.

Other limitations (moderate to change):
- The cube is only at one angle in 3D space. That is, standing flat on the x-axis. I could add 
  another parameter to initialize the cube at a different rotation, and it would minimally (if
  at all) affect the calculation process, which is the most important anyways.