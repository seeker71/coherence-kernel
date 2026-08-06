# concept-semantics-10000-project.awk — Princeton WordNet 3.1 projection.
#
# Inputs, in order: index.sense; four index.POS; four POS exceptions; four
# data.POS; anchors.tsv (`id<TAB>label`). LC_ALL=C is required: widths are bytes.
# Outputs are supplied with -v index_out=... -v payload_out=... -v stats_out=...
#
# Mapping law:
#   exact lemma > WordNet exception. Context-free suffix detachment is refused:
#   without POS it can map `his` to noun `hi`, which is morphology-shaped but
#   semantically false.
#   Each POS contributes its WordNet sense 1 (first index offset). The primary
#   POS is the candidate with greatest tagged count; zero/ties keep n,v,a,r.

function hexval(c) {
  if (c >= "0" && c <= "9") return c + 0
  c = tolower(c)
  return 10 + index("abcdef", c) - 1
}

function hexint(s,    i,v) {
  v = 0
  for (i = 1; i <= length(s); i++) v = v * 16 + hexval(substr(s,i,1))
  return v
}

function filepos(name) {
  if (name ~ /noun/) return "n"
  if (name ~ /verb/) return "v"
  if (name ~ /adj/) return "a"
  return "r"
}

function sensepos(code) {
  if (code == "1") return "n"
  if (code == "2") return "v"
  if (code == "3" || code == "5") return "a"
  return "r"
}

function haslemma(lemma) {
  return ((lemma SUBSEP "n") in first || (lemma SUBSEP "v") in first ||
          (lemma SUBSEP "a") in first || (lemma SUBSEP "r") in first)
}

function pospriority(p) {
  if (p == "n") return 0
  if (p == "v") return 1
  if (p == "a") return 2
  return 3
}

function total_senses(lemma) {
  return (sensecnt[lemma SUBSEP "n"] + sensecnt[lemma SUBSEP "v"] + sensecnt[lemma SUBSEP "a"] + sensecnt[lemma SUBSEP "r"])
}

function total_pos(lemma,    n) {
  n = 0
  if ((lemma SUBSEP "n") in first) n++
  if ((lemma SUBSEP "v") in first) n++
  if ((lemma SUBSEP "a") in first) n++
  if ((lemma SUBSEP "r") in first) n++
  return n
}

function reset_best() {
  B_FOUND=0; B_LEMMA=""; B_POS=""; B_OFFSET=""; B_TAG=-1; B_ORDER=999999
}

function consider(lemma,p,order,    k,off,t) {
  k = lemma SUBSEP p
  if (!(k in first)) return
  off = first[k]
  t = tagcnt[lemma SUBSEP p SUBSEP off] + 0
  if (!B_FOUND || t > B_TAG || (t == B_TAG && order < B_ORDER) ||
      (t == B_TAG && order == B_ORDER && pospriority(p) < pospriority(B_POS))) {
    B_FOUND=1; B_LEMMA=lemma; B_POS=p; B_OFFSET=off; B_TAG=t; B_ORDER=order
  }
}

function consider_all_pos(lemma,order) {
  consider(lemma,"n",order); consider(lemma,"v",order)
  consider(lemma,"a",order); consider(lemma,"r",order)
}

function consider_exception(form,p,order,    k,n,a,i) {
  k = form SUBSEP p
  if (!(k in exceptions)) return order
  n = split(exceptions[k],a," ")
  for (i=1; i<=n; i++) { consider(a[i],p,order); order++ }
  return order
}

function emit_miss() {
  printf "%035d", 0 >> index_out
  miss++
}

function emit_map(method,    dk,stype,g,rels,rc,ts,pc,payload,plen,idxrow) {
  dk = B_POS SUBSEP B_OFFSET
  stype = datatype[dk]
  g = gloss[dk]
  rels = relations[dk]
  rc = relcount[dk] + 0
  ts = total_senses(B_LEMMA)
  pc = total_pos(B_LEMMA)
  payload = sprintf("%02d%s%04d%s%s",length(B_LEMMA),B_LEMMA,length(g),g,rels)
  plen = length(payload)
  idxrow = sprintf("%1d%08d%05d%s%s%03d%1d%05d%03d",
                   method,payload_bytes,plen,stype,B_OFFSET,ts,pc,B_TAG,rc)
  if (length(idxrow) != 35) {
    print "bad index width for " B_LEMMA ": " length(idxrow) > "/dev/stderr"
    exit 2
  }
  printf "%s",idxrow >> index_out
  printf "%s",payload >> payload_out
  payload_bytes += plen
  mapped++; method_count[method]++
  pos_count[stype]++
  if (ts > 1) poly++
  if (rc > 0) relation_rows++
  relations_total += rc
  if (length(g) > max_gloss) max_gloss=length(g)
  if (plen > max_payload) max_payload=plen
}

BEGIN {
  printf "" > index_out; close(index_out)
  printf "" > payload_out; close(payload_out)
  printf "" > stats_out; close(stats_out)
}

# WordNet sense index: sense_key synset_offset sense_number tag_count
FILENAME ~ /index\.sense$/ {
  if ($3 == 1) {
    split($1,k,"%")
    p=sensepos(substr(k[2],1,1))
    tagcnt[k[1] SUBSEP p SUBSEP $2]=$4+0
  }
  next
}

# WordNet POS indices. Header/license rows begin with spaces and have no lemma shape.
FILENAME ~ /index\.(noun|verb|adj|adv)$/ {
  if ($0 ~ /^  / || NF < 6) next
  p=filepos(FILENAME); pc=$4+0
  sensefield=5+pc; firstfield=7+pc
  first[$1 SUBSEP p]=$(firstfield)
  sensecnt[$1 SUBSEP p]=$(sensefield)+0
  next
}

# Authoritative irregular morphology exceptions.
FILENAME ~ /(noun|verb|adj|adv)\.exc$/ {
  p=filepos(FILENAME)
  bases=""
  for (i=2;i<=NF;i++) bases = bases (bases=="" ? "" : " ") $i
  exceptions[$1 SUBSEP p]=bases
  next
}

# WordNet data rows: retain gloss and every typed pointer relation.
FILENAME ~ /data\.(noun|verb|adj|adv)$/ {
  if ($0 ~ /^  / || $1 !~ /^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]$/) next
  p=filepos(FILENAME); off=$1; wc=hexint($4); pi=5+2*wc; pn=$(pi)+0
  rel=""
  for (j=0;j<pn;j++) {
    sym=$(pi+1+4*j); toff=$(pi+2+4*j); tp=$(pi+3+4*j)
    rel = rel sprintf("%-2s%s%s",sym,tp,toff)
  }
  bar=index($0," | ")
  g=(bar>0 ? substr($0,bar+3) : "")
  sub(/[ \r]+$/, "", g)
  gloss[p SUBSEP off]=g
  datatype[p SUBSEP off]=$3
  relations[p SUBSEP off]=rel
  relcount[p SUBSEP off]=pn
  next
}

# Anchors: id<TAB>surface-label. All source tables have already been loaded.
FILENAME ~ /concept-semantics-anchors\.tsv$/ {
  form=$2
  reset_best(); consider_all_pos(form,0)
  if (B_FOUND) { emit_map(1); next }

  reset_best(); order=0
  order=consider_exception(form,"n",order)
  order=consider_exception(form,"v",order)
  order=consider_exception(form,"a",order)
  order=consider_exception(form,"r",order)
  if (B_FOUND) { emit_map(2); next }

  emit_miss()
  next
}

END {
  print "anchors=" mapped+miss > stats_out
  print "mapped=" mapped >> stats_out
  print "miss=" miss >> stats_out
  print "exact=" method_count[1] >> stats_out
  print "exception=" method_count[2] >> stats_out
  print "detached=" (method_count[3]+0) >> stats_out
  print "polysemous=" poly >> stats_out
  print "relation_rows=" relation_rows >> stats_out
  print "relations_total=" relations_total >> stats_out
  print "primary_n=" pos_count["n"] >> stats_out
  print "primary_v=" pos_count["v"] >> stats_out
  print "primary_a=" pos_count["a"] >> stats_out
  print "primary_s=" pos_count["s"] >> stats_out
  print "primary_r=" pos_count["r"] >> stats_out
  print "payload_bytes=" payload_bytes >> stats_out
  print "max_gloss_bytes=" max_gloss >> stats_out
  print "max_payload_bytes=" max_payload >> stats_out
}
