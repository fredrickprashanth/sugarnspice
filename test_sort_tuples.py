#!/usr/bin/python
from operator import itemgetter

my_input = [
(1, 2, "SE"),
(2, 3, "NE"),
(3, 5, "E"),
(6, 6, "E"),
(0, 9, "NW"),
(7, 1, "SW"),
(1, 1, "NE")
]

my_input = sorted (my_input, key=itemgetter(1), reverse=True)
my_input = sorted (my_input, key=itemgetter(0))

print(my_input)
 
