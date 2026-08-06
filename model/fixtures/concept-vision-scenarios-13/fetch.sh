#!/bin/sh
set -eu

# Reproduce the committed scenario photographs from attributed Wikimedia
# Commons derivatives. The classifier never receives this metadata.

fixture_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
curl_bin=/usr/bin/curl

fetch_one() {
  name=$1
  expected=$2
  url=$3
  target="$fixture_dir/$name"
  "$curl_bin" -LfsS -A 'coherence-kernel fixture reproduction; github.com/seeker71' "$url" -o "$target"
  observed=$(/usr/bin/shasum -a 256 "$target" | /usr/bin/awk '{print $1}')
  if [ "$observed" != "$expected" ]; then
    printf 'checksum mismatch for %s: expected %s observed %s\n' "$name" "$expected" "$observed" >&2
    exit 1
  fi
}

fetch_one fire.jpg c9514f09b90d4750a7d1eaf5fd3d3a42351814390f1534ac6d2e25999ffa2eaf \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/1/19/Fire_Extinguisher_01.jpg/1280px-Fire_Extinguisher_01.jpg'
fetch_one firstaid.jpg 54cd233bbacd1216ef9576c2ac4e71a3cd4e281bdeae637bab95bb52bc6a348f \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/3/30/First-aid_travel_kit_1.jpg/1280px-First-aid_travel_kit_1.jpg'
fetch_one stethoscope.jpg 53e6545eb63cf77f97910687a47dae6fe46610dd084d9eb3a519b25743bdbbd7 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/b/bc/Stethoscope_A.jpg/1280px-Stethoscope_A.jpg'
fetch_one bus.jpg b439bf51c21d5111a454193da4cdcdf657628929916ae03a3e4b4c14d3939197 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/5/58/Bus_801_Limoilou.jpg/1280px-Bus_801_Limoilou.jpg'
fetch_one bicycle.jpg f24ad7d58d2021260aa94d5070d69f8f32d778b805b16826d78f3ab3063535ed \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/2/2d/Bicycle_on_Kings_Lane%2C_Cambridge_-_geograph.org.uk_-_5985166.jpg/1280px-Bicycle_on_Kings_Lane%2C_Cambridge_-_geograph.org.uk_-_5985166.jpg'
fetch_one worker.jpg c50e49c1a870afc741d5a912c5aec25132725382dfd842be2e7488b0f8a1f3b9 \
  'https://upload.wikimedia.org/wikipedia/commons/d/d2/Hard_Hat_Worker_HHW01.JPG'
fetch_one kitchen.jpg 49f1ab1e9b32965d07f40cfbc5d247c46208cba8692d668400c32b837b79b2f7 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/4/47/Carlton_%28Kitchen_interior_with_woman_and_three_children%29._State_Library_Victoria.jpg/1280px-Carlton_%28Kitchen_interior_with_woman_and_three_children%29._State_Library_Victoria.jpg'
fetch_one house.jpg 5588d15d9943da95a4e5eed6c07ba91eb5c71b924fc2c764b40f6322be7de1a7 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/7/77/Russian_peasant_girls_in_front_of_a_traditional_wooden_house_in_Kirillov.jpg/1280px-Russian_peasant_girls_in_front_of_a_traditional_wooden_house_in_Kirillov.jpg'
fetch_one umbrella.jpg cc3044c38be51e1f4216e8a6de97bf5dc3190e54207874032f2447b303067d4c \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/6/64/Guy_with_an_umbrella_battling_rain_and_snow_on_a_bridge.jpg/1280px-Guy_with_an_umbrella_battling_rain_and_snow_on_a_bridge.jpg'
fetch_one snow.jpg 365cef60b2f54342f43d6cc0957eee9a5e2cc830bc4876a8f64c9f8e809404da \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/6/68/Orchards_in_snow%2C_Sangla%2C_Himachal_Pradesh%2C_India.jpg/1280px-Orchards_in_snow%2C_Sangla%2C_Himachal_Pradesh%2C_India.jpg'
fetch_one bread.jpg 06b9fad868452e5feb1ac8ffb5f454f0b07861d633a2f1a038539d297bf8d526 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/4/45/A_Moldy_Loaf_of_Bread.jpg/1280px-A_Moldy_Loaf_of_Bread.jpg'
fetch_one apple.jpg 637e6863e11856dd42203f4185a6d9558a9f9bfc11239dc4c3024126eab13f34 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/0/0f/Golden_Delicious_apples.jpg/1280px-Golden_Delicious_apples.jpg'
fetch_one park.jpg 8db1fdfcecf780709686cc509c913e0b8f22c5d7b604c75fc76fdcc789e6f681 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/b/bd/Boston_Public_Garden_%2836008p%29.jpg/1280px-Boston_Public_Garden_%2836008p%29.jpg'
fetch_one playground.jpg cb04e977d147495d09d3ccc6b40644d27dbeac7a22e0f9d709a8cd608384f7ba \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/e/e6/Children%27s_playground_-_geograph.org.uk_-_5606737.jpg/1280px-Children%27s_playground_-_geograph.org.uk_-_5606737.jpg'
fetch_one library.jpg d82530b2be9e0dfca44896b0cbfc3edd569ffe4e88a92d6b3b996a978b2cc252 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/a/a8/LOC_Main_Reading_Room_Highsmith.jpg/1280px-LOC_Main_Reading_Room_Highsmith.jpg'
fetch_one crossing.jpg 91bb43b7080be0c9ecbf4a5df9049c211801da7bb027c3df78887dba482486f8 \
  'https://upload.wikimedia.org/wikipedia/commons/thumb/c/c5/Tokyo_Shibuya_Scramble_Crossing_2018-10-09.jpg/1280px-Tokyo_Shibuya_Scramble_Crossing_2018-10-09.jpg'

printf '16 verified real-life scenario photographs\n'
