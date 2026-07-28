# Real-life open-label vision fixtures

Ten attributable photographs exercise the pretrained-label → Form → ranked
10,000-concept bridge. Nine are positive observations; the guitar photograph
is deliberately retained as a hard negative because the visible instrument is
not named in the carrier's top twenty labels on the witnessed host.

`PROVENANCE.tsv` records every Commons file page, author, license, checksum,
byte size, and actual derivative dimensions. `fetch.sh` reproduces and verifies
the committed bytes. The files are unmodified Commons derivatives; changes to
this repository remain under the repository license, while each photograph
retains the license named in the provenance table.

No filenames, expected labels, concept ids, page descriptions, or attribution
rows are passed to the classifier. The live Form cell supplies only an opaque
image path to the Swift carrier and receives only confidence/label rows.
