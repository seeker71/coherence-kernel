import struct, random
random.seed(13); N=8
def rf(x): return struct.unpack('<f',struct.pack('<f',x))[0]
def FM(a,b):return rf(a*b)
def FA(a,b):return rf(a+b)
def FS(a,b):return rf(a-b)
def FD(a,b):return rf(a/b)
def fexp(v):
    s=v;k=0
    while abs(s)>0.5:s=FM(s,0.5);k+=1
    n=1.0;term=1.0;acc=1.0
    for q in range(14):term=FM(term,FD(s,n));acc=FA(acc,term);n=FA(n,1.0)
    while k>0:acc=FM(acc,acc);k-=1
    return acc
X=[rf(round(random.uniform(-3,3),3)) for _ in range(N)]
open('xs.bin','wb').write(b''.join(struct.pack('<f',v) for v in X))
m=X[0]
for v in X[1:]:
    if v>m: m=v
s=0.0
for i in range(N-1,-1,-1): s=FA(fexp(FS(X[i],m)),s)
Y=[FD(fexp(FS(X[i],m)),s) for i in range(N)]
def bts(x):return struct.unpack('<I',struct.pack('<f',rf(x)))[0]
print("SM_Y", " ".join(str(bts(y)) for y in Y))
