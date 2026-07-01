# Speech pair training next action

The neural pair coverage report says the current neural route is still zero.
This receipt turns that fact into the next executable movement.

Current state:

```text
neural pair windows: 1
Form-native seeded pair windows: 8
captured live wavs: 211
required live wav floor: 12000
native Form: true
neural ready: false
```

Chosen next action:

```text
id: next-native-neural-pair-window-0002
pair: en<->pt-br
route: train-native-neural-pair-window
reason: neural-micro-pair-training-enabled
Form pair windows: 8 -> 8
neural pair windows: 1 -> 2
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

Meaning: the next honest move is to run the second native neural micro-pair
window and keep capturing consentful audio. The planned neural count moves
`1 -> 2`; full open ASR/TTS authority still waits for open receipts.
