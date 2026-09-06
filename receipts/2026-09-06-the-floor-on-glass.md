# The floor on glass

Urs: "all optimized using the hardware bandwidth as floor." Then the floor has to be visible, in
the same frame as the measurement, or it is a sentence rather than a standard.
`form/form-stdlib/floor-lens.bml` and `observe/floor-lens-run.fk` measure the bandwidth here and
now (a kernel reads a real model blob through the handle door; the bytes it moved over the seconds
it took), then run each lane warm and show, per lane, the bytes one pass must move, the floor at
that bandwidth, the measured time, and the distance between them.

This Mac gives 330 to 348 GB/s through the door, and the lanes stand:

| lane | bytes a pass moves | floor | measured | distance |
|---|---|---|---|---|
| whisper encode, 8 s window | 83 MB (15 MB weights, the rest activation) | 0.23 ms | 3.75 ms | 15.7x |
| whisper decode token | 59 MB, the decoder's own tensors | 0.17 ms | 0.64 ms | 3.8x |
| dense llama3.2:1b token | 1321 MB, the blob read once | 3.80 ms | 15.4 ms | 4.1x |

The floors are the model's own arithmetic: the encoder's and decoder's tensor bytes summed from
the npz table by name, the blob's size for a dense token, and the encoder's activation traffic
named term by term as it is dispatched. `form/form-stdlib/tests/floor-lens-band.fk` = 31 proves
the lens without a GPU: a gigabyte at a thousand gigabytes a second is one millisecond, and a
lane faster than a millisecond a pass is not measured as zero.

The surprise: the first lens said the encoder was 85x its floor, and it was flattering the floor,
not the lane — counting only the weights when the pass moves four times more activation than
weight. Naming each term brought it to 15.7x, which is the real room. Discomfort turned gold: the
whisper token measured 0 ms, and that zero was the lens's wound (whole milliseconds divided by a
count) rather than the lane's virtue; totals and counts as floats made it 0.64 ms, and the band
now pins that so no lane can ever again read as free.
