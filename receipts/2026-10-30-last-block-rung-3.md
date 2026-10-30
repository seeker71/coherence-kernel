# Last-block rung 3: twenty Adam steps, two rows held out

The gradient rung 2 checked was a mean square on `blk.63.ffn_down`. This rung
steps it. Eight token-rows of the 23-id capture train; the next two are held
out and do not enter the gradient. Adam is the same update as `nlb_adam`,
learning rate 1e-4, twenty steps.

A rate of 1 was the first attempt. Every entry moved by about one, the square
sum left the fixed-point range, and the loss read negative. That reading is
overflow. The door now answers `nothing()` for a negative reconstruction, and
the band asks for a non-negative descent across all twenty steps.

Witness, two runs, the same digits: `form/form-stdlib/tests/qwen-last-block-step-band.bml` → 7.
Train mean 3.1072222511585776 → 0.27509687245846803. Held-out mean
12.42888900463431 → 1.1003874898338721. Last gradient norm 5.005819393980161.
The steps took 185 ms and 192 ms. The head's cross-entropy is still unbuilt.
