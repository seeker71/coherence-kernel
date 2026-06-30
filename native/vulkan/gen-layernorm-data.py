import struct, random
random.seed(17); N=8; EPS=1e-5
def rf(x): return struct.unpack('<f',struct.pack('<f',x))[0]
def FM(a,b):return rf(a*b)
def FA(a,b):return rf(a+b)
def FS(a,b):return rf(a-b)
def FD(a,b):return rf(a/b)
def FQ(a):return rf(struct.unpack('<f',struct.pack('<f',a**0.5))[0])
X=[rf(round(random.uniform(-3,3),3)) for _ in range(N)]
G=[rf(round(random.uniform(0.5,1.5),3)) for _ in range(N)]
B=[rf(round(random.uniform(-0.5,0.5),3)) for _ in range(N)]
for nm,v in [("xl",X),("gl",G),("bl",B)]: open(nm+'.bin','wb').write(b''.join(struct.pack('<f',z) for z in v))
s=0.0
for i in range(N-1,-1,-1): s=FA(X[i],s)
mean=FD(s,rf(float(N)))
vs=0.0
for i in range(N-1,-1,-1):
    d=FS(X[i],mean); vs=FA(FM(d,d),vs)
inv=FD(rf(1.0), FQ(FA(FD(vs,rf(float(N))), rf(EPS))))
Y=[FA(FM(FM(FS(X[i],mean),inv),G[i]),B[i]) for i in range(N)]
def bts(x):return struct.unpack('<I',struct.pack('<f',rf(x)))[0]
print("EPS_BITS", bts(EPS))
print("LN_Y", " ".join(str(bts(y)) for y in Y))
