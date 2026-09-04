#!/bin/sh
# the school runs itself: harvest when a training ends, then emit and train again, until the stop file
cd "$(dirname "$0")/.." || exit 1
while [ ! -f .form-lora-voice-native/school.stop ]; do
  if [ -f .form-lora-voice-native/train.rc ]; then
    printf '.form-lora-voice-native\n\n' | ./fkwu observe/voice-school-run.fk >> .form-lora-voice-native/school.log 2>&1
    rm -f .form-lora-voice-native/train.rc; mv .form-lora-voice-native/train.log .form-lora-voice-native/train.prev.log 2>/dev/null
    printf 'emit\n' | ./fkwu observe/lora-voice-run.fk >> .form-lora-voice-native/school.log 2>&1
    printf 'train\n' | ./fkwu observe/lora-voice-run.fk >> .form-lora-voice-native/school.log 2>&1
  fi
  sleep 60
done
