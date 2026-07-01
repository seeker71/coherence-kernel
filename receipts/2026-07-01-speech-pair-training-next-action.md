# Speech pair training next action

The neural pair coverage report says the current neural route is still zero.
This receipt turns that fact into the next executable movement.

Current state:

```text
neural pair windows: 0
Form-native seeded pair windows: 7
captured live wavs: 211
required live wav floor: 12000
native Form: true
neural ready: false
```

Chosen next action:

```text
id: next-form-pair-window-0008
pair: en<->fr
route: expand-form-native-pair-window-before-neural
reason: neural-pairs-zero-and-corpus-under-floor
Form pair windows: 7 -> 8
neural pair windows: 0 -> 0
capture state: 211/12000
```

Witness:

```sh
cat learn/speech-neural-pair-coverage.fk \
    learn/speech-pair-training-next-action.fk \
    learn/tests/speech-pair-training-next-action-band.fk > /tmp/speech-pair-training-next-action.fk
./fkwu --src /tmp/speech-pair-training-next-action.fk
```

```text
32767
```

Meaning: the next honest move is to run another Form-native reciprocal pair
window and keep capturing consentful audio. It is not time to claim
`A=>neural=>B`; the planned neural count remains `0 -> 0`.
