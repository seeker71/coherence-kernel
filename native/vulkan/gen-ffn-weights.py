import struct, random
random.seed(11)
INDIM,HID,OUTD=4,8,3
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
C8=rf(0.7978845608028654);C4=rf(0.044715)
def fgelu(v):
    z=FM(C8,FA(v,FM(C4,FM(v,FM(v,v)))));e=fexp(FM(2.0,z))
    th=FD(FS(e,1.0),FA(e,1.0));return FM(FM(0.5,v),FA(1.0,th))
def rnd(): return rf(round(random.uniform(-1.2,1.2),3))
W1=[[rnd() for _ in range(INDIM)] for _ in range(HID)]
B1=[rnd() for _ in range(HID)]
W2=[[rnd() for _ in range(HID)] for _ in range(OUTD)]
B2=[rnd() for _ in range(OUTD)]
X=[rnd() for _ in range(INDIM)]
def pack(vals):
    b=bytearray()
    for v in vals: b+=struct.pack('<f',rf(v))
    return bytes(b)
open('W1.bin','wb').write(pack([w for row in W1 for w in row]))
open('b1.bin','wb').write(pack(B1))
open('W2.bin','wb').write(pack([w for row in W2 for w in row]))
open('b2.bin','wb').write(pack(B2))
open('xf.bin','wb').write(pack(X))
# reference: downward fold matches shader
def dot_down(wrow,xv):
    acc=0.0
    for j in range(len(xv)-1,-1,-1): acc=FA(FM(wrow[j],xv[j]),acc)
    return acc
A=[fgelu(FA(dot_down(W1[k],X),B1[k])) for k in range(HID)]
Y=[FA(dot_down(W2[i],A),B2[i]) for i in range(OUTD)]
def bts(x):return struct.unpack('<I',struct.pack('<f',rf(x)))[0]
print("EXPECT_Y_BITS", " ".join(str(bts(y)) for y in Y))
