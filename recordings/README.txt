the folder first_half, second_half and interleave refers to the sign inversion when the input is split to the delay lines. 

For 4 delay lines.
First half =  [*-1, *-1, unchanged, unchanged]
Second half = [unchanged, unchanged, *-1, *-1]
Interleaved = [unchanged, *-1, unchanged, *-1]

The comb filtering effect from the all pass is quite different for each of these.

