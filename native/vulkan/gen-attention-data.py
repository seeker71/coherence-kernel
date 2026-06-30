import struct, random
random.seed(23); S=4; D=4; INV=0.5
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
def rnd():return rf(round(random.uniform(-1.2,1.2),3))
Q=[rnd() for _ in range(S*D)];K=[rnd() for _ in range(S*D)];V=[rnd() for _ in range(S*D)]
for nm,a in [("q",Q),("k",K),("v",V)]: open(nm+'a.bin','wb').write(b''.join(struct.pack('<f',z) for z in a))
Y=[0.0]*(S*D)
for i in range(S):
    sc=[0.0]*S
    for j in range(S):
        acc=0.0
        for t in range(D-1,-1,-1): acc=FA(FM(Q[i*D+t],K[j*D+t]),acc)
        sc[j]=FM(acc,INV)
    m=sc[0]
    for j in range(1,S):
        if sc[j]>m:m=sc[j]
    ssum=0.0
    for j in range(S-1,-1,-1): ssum=FA(fexp(FS(sc[j],m)),ssum)
    for t in range(D):
        o=0.0
        for j in range(S-1,-1,-1): o=FA(FM(FD(fexp(FS(sc[j],m)),ssum),V[j*D+t]),o)
        Y[i*D+t]=o
def bts(x):return struct.unpack('<I',struct.pack('<f',rf(x)))[0]
print("ATT_Y", " ".join(str(bts(y)) for y in Y))
