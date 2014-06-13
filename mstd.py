from math import *

def fact(n):
    if n <= 1:
        return 1
    return n * fact(n-1)
    

def choose(n, k):
    return fact(n) // (fact(k) * fact(n - k))

def decode(s,n):
    for i in range(n):
        if (s >> i) & 1 == 1:
            yield i

def sumset(s):
    for x in s:
        for y in s:
            yield x + y

def diffset(s):
    for x in s:
        for y in s:
            yield x - y

def print_pt(p):
    print ("\\draw[fill=black] (%s,%s) circle (2pt);" % p)

def print_line(p,q):    
    print ("\\draw (%s,%s) -- (%s,%s);" % (p[0],p[1],q[0],q[1]))

def compute(l, n):
    s = 0
    for i in range(1,n + 1):
        s += (n - i + 1) * l[i-1]
    return s

def scatter(l):
    print("\\documentclass{article}")
    print("\\usepackage{tikz}")
    print("\\usepackage[margin=1in]{geometry}")
    print("\\pagestyle{empty}")
    print("\\begin{document}")
    print("\\begin{tikzpicture}[yscale=4,xscale=0.5]")

    print("\\draw[gray,->] (-0.5, 4) -- (%d, 4);" % len(l))
    print("\\draw[gray,->] (0, 3.5) -- (0, %d);" % (ceil(max(l))))

    for p in enumerate(l):
        print_pt(p)

    print("\\end{tikzpicture}")
    print("\\end{document}")

def main():
    print("\\documentclass{article}")
    print("\\usepackage{tikz}")
    print("\\usepackage[margin=1in]{geometry}")
    print("\\pagestyle{empty}")
    print("\\begin{document}")

    n = 15
    A = set(decode(29239, n))

    print("\\begin{tikzpicture}")
    print("\\draw[gray,->] (-0.5, 0) -- (%d, 0);" % n)
    print("\\draw[gray,->] (0, -0.5) -- (0, %d);" % n)

    for x in A:
        for y in A:
            print_pt((x,y))
    for s in sumset(A):
        if s < n:
            print_line((s,0), (0, s))
        else:
            print_line((n-1,s - n + 1), (s - n +1, n-1))
        #print_pt((s+1, -1))
    #print("\\end{tikzpicture}\\\\[1cm]")
    #
    #print("\\begin{tikzpicture}[scale=.5]")
    #print("\\draw[gray,->] (-0.5, 0) -- (%d, 0);" % n)
    #print("\\draw[gray,->] (0, -0.5) -- (0, %d);" % n)
    #
    #for x in A:
    #    for y in A:
    #        print_pt((x,y))
    for d in diffset(A):
        if d < 0:
            print_line((0,-d), (n + d - 1, n-1))
        else:
            print_line((d,0), (n-1, n-d-1))
        #print_pt((d-1, -1))

    print("\\end{tikzpicture}")

    print("\\end{document}")

if __name__ == '__main__':
    #main()
    l = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 14, 16, 42, 92, 208, 382, 834, 1748, 3568, 7066, 14914, 28618, 60712, 120872, 240102, 483328, 992812, 1948804, 3975364, 7933368, 15876692, 31759760, 64035868, 126968066, 255821994, 510650874, 1019984988, 2037991854, 4087847100, 8137581604]
    print ([compute(l, n) for n in range(1, 47)])
     # scatter(list(filter(lambda x: x >= 4, [(compute(l, n) / 2 ** n) * 10**4 for n in range(1, 47)])))

