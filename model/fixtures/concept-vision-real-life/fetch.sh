#!/bin/sh
set -eu

# Reproduce the committed real-image fixture set from attributed Wikimedia
# Commons derivatives. No concept metadata is supplied to the vision carrier;
# this script is fixture transport and checksum verification only.

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
curl_bin=/usr/bin/curl

fetch_one() {
  name=$1
  expected=$2
  url=$3
  target="$fixture_dir/$name"
  "$curl_bin" -LfsS -A 'coherence-kernel fixture reproduction; github.com/seeker71' "$url" -o "$target"
  observed=$(shasum -a 256 "$target" | awk '{print $1}')
  if [ "$observed" != "$expected" ]; then
    printf 'checksum mismatch for %s: expected %s observed %s\n' "$name" "$expected" "$observed" >&2
    exit 1
  fi
}

fetch_one airplane.jpg 139c0fd483736ea3ff55e22d30ee85835cf69ba30154a731a82751df155cdf3e \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/e/ef/B17g_and_b52h_in_flight.jpg/1280px-B17g_and_b52h_in_flight.jpg'
fetch_one banana.jpg a20a8ed87002f1c72c6627d4852555139f85e5c2f90acbff71a9277ba02b4194 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/4/4c/Bananas.jpg/1280px-Bananas.jpg'
fetch_one bridge.jpg f2f1708810c37de6a093d2ee51008dc5c68a9df815ef970b6a807b2de5df96b2 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/b/b9/Canberra_%28AU%29%2C_Commonwealth_Avenue_Bridge_--_2019_--_1811.jpg/1280px-Canberra_%28AU%29%2C_Commonwealth_Avenue_Bridge_--_2019_--_1811.jpg'
fetch_one cat.jpg 7d4106eaa1fb4b3c9c0301d292991e354fc5c2e1c0e6cdaf65c34463d2c2fe16 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/7/72/Cat_playing_with_a_lizard.jpg/1280px-Cat_playing_with_a_lizard.jpg'
fetch_one coffee.jpg bbbb4ab6f72899d296c263fc8b6f27a1ed11aeb6b53c36062afe8a33629b46c5 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/4/45/A_small_cup_of_coffee.JPG/1280px-A_small_cup_of_coffee.JPG'
fetch_one dog.jpg 7e735cfe66005797b4d08e67fbf86a6b29589ba1a38b79989caa479862ca7686 \
  'https://upload.wikimedia.org/wikipedia/commons/d/dd/Golden_Retriever_Hund_Dog.JPG'
fetch_one guitar.jpg 741b7b2925c1d083c2ff06a495b9b9b00b4b67f80d329a349042bc4ee65862ea \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/e/e0/Man_playing_an_acoustic_brazilian_guitar_%28Viol%C3%A3o%29_on_Marco_Zero_Square%2C_Refice%2C_Pernambuco%2C_Brazil.jpg/1280px-Man_playing_an_acoustic_brazilian_guitar_%28Viol%C3%A3o%29_on_Marco_Zero_Square%2C_Refice%2C_Pernambuco%2C_Brazil.jpg'
fetch_one pizza.jpg 062e7c2b19e72258d006573e350c430bd4621a80619dd205a078d6081efe5dc5 \
  'https://upload.wikimedia.org/wikipedia/commons/a/a3/Eq_it-na_pizza-margherita_sep2005_sml.jpg'
fetch_one sunflower.jpg 109b1304d93923fbb94ef878eaa6b9b37d38bdafb06420097d38ef6d9befed25 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/c/c0/Sunflower_head_2015_G1.jpg/1280px-Sunflower_head_2015_G1.jpg'
fetch_one train.jpg 23ed51991c5de1657fa8ef9f740e6fdedc458da87d8a502109c60d1f3f410f65 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/a/aa/Locomotive_ChS4-072_2011_G1.jpg/1280px-Locomotive_ChS4-072_2011_G1.jpg'

printf '10 verified real-image fixtures\n'
