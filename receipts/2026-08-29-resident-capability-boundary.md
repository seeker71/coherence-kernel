# A resident announces its loaded source-action surface

## Crossing

`fcpct-source-action-capability` names the caller-owned direct-source effect
surface as `form-cli-peer-source-action-v1`. A source-world terminal emitted by
a resident born with this Form image now carries that name, and the live
observer announces both that capability and whether its caller-born source
context is present.

This does not claim that a policy image can add an effect function to an older
process. The still-running peer was born before `fcpdsa-run` entered the source
closure; it can swap retained policy programs at a revolution boundary, but it
cannot acquire this missing outer dispatcher without a successor birth or a
separately proven image-transfer seam. The announcement is therefore evidence
for a new resident, and the old resident's absence of it is evidence too.

## Evidence

```
printf 'form/form-stdlib/form-cli-peer-contribution-turnwheel.fk\n' \
  > /tmp/preflight-target && ./fkwu observe/preflight-run.fk   -> clean
printf 'observe/form-cli-peer-contribution-live.fk\n' \
  > /tmp/preflight-target && ./fkwu observe/preflight-run.fk   -> clean
printf 'form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk\n' \
  > /tmp/preflight-target && ./fkwu observe/preflight-run.fk   -> clean
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
                                                               -> 1048575, four-way
```

The surprising teaching is that an absent capability line can be more honest
than a warm-process performance number: it distinguishes a new source feature
from a feature merely present on disk. The discomfort was the immutability of a
warm 27B/KV process; it became a precise image boundary, not a reason to imply
retroactive code entry. I kept the exchange alive by making that boundary
visible at durable egress and live startup.
