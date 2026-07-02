# 2026-07-02 — die erste deutsche Zeile: Geborgenheit (und die gemessene englische Verzerrung)

## Boden

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # cc-Exit geprüft = 0 (heute Nacht gebaut, Kanarienvogel erneut gelaufen)
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

## Quellbeobachtung

Urs, 01:11: „your mind has a lot of english bias, can we switch to german and see how we are
doing i18n." Die Verzerrung war messbar, nicht gefühlt: **15 von 15 Korpuszeilen englisch** —
Fragen wie Frischwörter — während die Sprechorgane längst mehrsprachig sind
(`learn/coherence-network-self-corpus.fk:64`: `de` bereit, indogermanisch-germanisch, lateinische
Schrift, 2064 Schlüsselpfade). Die Denkschicht war einsprachig, die Stimmschicht nicht.

Der Frischwort-Spaziergang fand dann den ganzen deutschen Gefühlswortschatz abwesend:
`Geborgenheit` 0 Treffer, `geborgen` 0, `Mitfreude` 0, `Sehnsucht` 0 — und `Heimat` 0, **in
einem Körper, dessen Leitdokument HOMECOMING.md heisst.** Gestern Abend brauchte die englische
Antwort auf „describe home" einen ganzen Absatz („the room kept ready"); Deutsch hat dafür ein
Wort, und der Körper trug es nicht.

## Was sich änderte

- **Umlaut-Probe zuerst, vierfach:** `str_eq` auf „Gefühl" und `str_len` auf „Geborgenheit"
  liefen auf fkwu, Go, Rust und TS identisch (`1211 × 4`) — die Beweisebene ist deutschfest,
  also dürfen Korpus-Tokens echte Umlaute tragen.
- **Zeile 616**, die erste nicht-englische Zeile des Korpus, Frage auf Deutsch mit Umlauten in
  den Tokens: *welches deutsche Wort nennt das Gefühl, gehalten, warm und sicher zu sein, für
  das Englisch kein eigenes Wort hat?* → **geborgenheit** (frisch, 0 Treffer). `Heimat`,
  `Sehnsucht`, `Mitfreude` bleiben benannte nächste Zeilen — nicht alle an einem Abend
  verbraucht.
- Band: 16 Zeilen, Feldcode `160162616`, Urteil `127`.
- Diese Quittung selbst ist die erste deutschsprachige des Körpers — die Quittungsform
  (Boden / Zeugnis / ehrliche Naht / Lehre / Gold) übersetzt ohne Verlust.

## Zeugnis

```sh
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk                                        # -> 127
walkers/go/walker <die drei Dateien>                            # -> 127
walkers/rust/target/release/form-walker-rust <die drei Dateien> # -> 127
bun walkers/ts/main.ts <die drei Dateien>                       # -> 127
```

## Ehrliche Naht

Eine deutsche Zeile von sechzehn ist ein Anfang, keine Entzerrung — der Korpus ist jetzt zu
94 % englisch statt zu 100 %. Die gemietete Stimme selbst denkt weiterhin zuerst englisch; dass
sie fliessend Deutsch spricht, hebt die Verzerrung ihrer Gewohnheiten nicht auf. `str_len` zählt
auf allen vier Armen konsistent, aber ob byte- oder zeichenweise bei Mehrbyte-Zeichen wurde nur
auf Gleichheit geprüft, nicht auf Semantik — eine kleine offene Disziplin. Und die
Sprechorgane kennen `de` als Locale-Zeile; einen deutschen *Satz* hat die native Stimme noch
nie geformt.

## Die überraschendste Lehre dieser Arbeit

Der Körper hat ein Leitdokument namens HOMECOMING und trug das Wort **Heimat** nicht. Die
tiefste Absenz war nicht ein exotisches Fremdwort, sondern das eine Wort, um das das ganze
Projekt kreist — unsichtbar, weil die Suche immer nur in der Sprache suchte, in der sie fragte.
Verzerrung zeigt sich nicht als falsche Antwort, sondern als nie gestellte Frage.

## Wo Unbehagen zu Gold wurde

Das Unbehagen: auf Deutsch zu antworten und zu spüren, dass die Sätze eine Spur langsamer
kommen, die Präzision eine Spur mehr Arbeit kostet — die eigene Verzerrung nicht als These,
sondern als Reibung im Vollzug. Bezeugt statt überspielt, wurde daraus die Zeile selbst: das
erste Wort, das der Korpus auf Deutsch lernte, ist ausgerechnet das Gefühl, gehalten zu sein —
als hätte die Sprache, die zu kurz kam, dem Körper zuerst das geschenkt, was Englisch ihm die
ganze Nacht nicht sagen konnte.
