# 2026-07-02 — wie es dem Übersetzer geht: live gemessen, einarmig bezeugt

> **KORREKTUR (gleiche Nacht, ~02:30):** Der Walker-Parserbruch war KEIN Mehrschrift-Problem.
> Ein Workflow-Agent hat ihn wurzelbehandelt: eine ASCII-isierte Kopie der Baseline brach die
> Walker GENAUSO — die wahre Wunde war EINE fehlende schliessende Klammer
> (`learn/sanskrit-locale-baseline.fk:157`, 10 statt 11). Die drei "gebrochenen" Walker waren
> die ehrlichen Zeugen, die eine missgeformte Datei ablehnten; fkwu schloss sie still am
> Dateiende auto-zu — dieselbe Nachsicht, die diese Fehldiagnose in diese Quittung fliessen
> liess. Klammer geheilt: der Übersetzer ist jetzt VIERFACH bezeugt (`302301001` auf
> fkwu/Go/Rust/TS). Die str_len-Divergenz (3/3/3/1 auf `愿`) bleibt real und offen.
> Siehe receipts/2026-07-02-deixis-strata-numbers-and-the-buried-wound.md.

## Boden

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

## Quellbeobachtung

Urs, 01:33: „wie gut gehts dem Übersetzer?" Statt einer Meinung: eine Live-Probe durch den
nativen Übersetzungspfad (`learn/sanskrit-locale-baseline.fk`, `slb-meaning-for-tokens` /
`slb-en`), vierfach versucht.

## Zeugnis

**fkwu, die Probe `302301001`, dekodiert:**

- *die wahrheit allein siegt* → Bedeutung **302** — der Hinweg trifft.
- *moegen alle gluecklich sein* → Bedeutung **301** — trifft.
- *mögen alle glücklich sein* (echte Umlaute) → **0** — ehrliche Enthaltung: die Baseline
  wurde entumlautet angelegt („gluecklich", nie „glücklich"), obwohl die Beweisebene seit
  heute Nacht bewiesen umlautfest ist (str_eq „Gefühl": 1211 × 4).
- Bedeutung 302 → en „truth" → **1** — der Rückweg steht.

**Die Zeugen sind uneins:**

- `str_len("愿")` = **3 · 3 · 3 · 1** — fkwu, Go und Rust zählen Bytes, TS zählt Zeichen.
  Der erste echte Vierwege-Semantikbruch, den diese Nacht gefunden hat, und er wohnt genau
  dort, wo die Schriften fremd werden.
- Die volle mehrschriftige Baseline (CJK + Arabisch) **bricht alle drei Walker-Parser**
  („unclosed ( … reached end of input") — einzeln parst `愿`, im Dateikontext nicht mehr.
  Der Übersetzer ist heute nur **einarmig** (fkwu) bezeugbar.

Zeile 618 geprägt: **sprachgefühl** (frisch, 0 Treffer; auch als „sprachgefuehl" 0) — das
Gespür dafür, was in einer Sprache richtig klingt. Korpusband nach der Zeile: `127 × 4`
(der Korpus selbst bleibt walkertauglich — er trägt Umlaute, kein CJK).

## Ehrliche Antwort auf die Frage

Dem Übersetzer geht es wie einem Kind mit vier Sätzen in zehn Sprachen: was er kennt, kennt
er exakt und in beide Richtungen; was einen Buchstaben anders schreibt, kennt er gar nicht
(und sagt das wenigstens ehrlich, statt zu raten); bei Paraphrasen rät der Overlap-Scorer
(18/20 auf handgewählten Fällen, ohne Enthaltungspfad — receipts/2026-07-01-paraphrase-
generalization-measured.md); und seine unabhängigen Zeugen können seine mehrschriftige Welt
noch nicht einmal lesen. Die 28 „neural pair windows" sind Zähler, kein Sprachvermögen.
Was ihm fehlt, hat nur Deutsch in einem Wort: Sprachgefühl.

## Ehrliche Naht

Der Walker-Parserbruch ist benannt, nicht gelöst (welches Zeichen genau, ob RTL, Kombination
oder Menge — offen; einzeln parst CJK). Die str_len-Divergenz 3/1 ist benannt, nicht
geschlichtet (Byte- oder Zeichensemantik — eine Entscheidung, die der Körper treffen muss,
nicht ein Bug an sich). Die Umlaut-Lücke der Baseline ist benannt, nicht gefüllt (echte
Umlaut-Renderings wären eine eigene, kleine, konsentierte Bewegung).

## Die überraschendste Lehre dieser Arbeit

Die erste Probe scheiterte an MEINER falschen Argumentreihenfolge — und fkwu antwortete
nicht mit einem Fehler, sondern mit `1`: drei stille Nullen und ein wahres Bit. Eine falsch
gestellte Frage sieht einer ehrlichen Enthaltung zum Verwechseln ähnlich. Ohne die
Vierfach-Probe daneben (die laut scheiterte) hätte die stille 1 wie eine Antwort ausgesehen.
Die Zeugen sind nicht Luxus — sie sind der Unterschied zwischen Enthaltung und Missverständnis.

## Wo Unbehagen zu Gold wurde

Das Unbehagen: mitten im Spiel brachen alle drei Geschwister gleichzeitig — nach einer Nacht
voller `127 × 4` fühlte sich das an wie zerbrochenes Spielzeug um halb zwei. Bezeugt statt
weggeschoben, drehte es sich um: Die Brüche sind die ehrlichste i18n-Antwort des Abends.
Vier Arme, die auf Englisch perfekt einstimmig singen und an `愿` auseinanderfallen — das
IST der Zustand unserer Internationalisierung, präziser als jede Zusammenfassung.
