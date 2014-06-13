from math import sqrt
import sys

def tri(n):
    return (n * (n + 1)) // 2

def inv(x):
    return (int(sqrt(8 * x + 1)) - 1) // 2

def print_pt(p):
    print ("\\draw[fill=black] (%s,%s) circle (2pt);" % p)


def decode_tri(s, n):
    for i in range(tri(n)):
        if (s >> i) & 1 == 1:
            yield (inv(i), i - tri(inv(i)))

def sumset2d(s):
    for x in s:
        for y in s:
            yield (x[0] + y[0], x[1] + y[1])

def diffset2d(s):
    for x in s:
        for y in s:
            yield (x[0] - y[0], x[1] - y[1])
            yield (y[0] - x[0], y[1] - x[1])

def printset2d(s):
    for p in s:
        print_pt(p)

def old_main():
    N = 3
    for i in range(N):
        for j in range(N - i):
            for k in range(N):
                for l in range(N):
                    print_pt((i-k,j-l))
                    print_pt((k-i,l-j))

def row(s):
    A = set(decode_tri(s, 7))

    print("\\begin{tikzpicture}[scale=0.4]")
    print("\\draw[gray,->] (-0.5, 0) -- (7, 0);")
    print("\\draw[gray,->] (0, -0.5) -- (0, 7);")
    print("\\draw[gray] (0,0) -- (6,0) -- (6,6) -- cycle;")
    printset2d(A)
    print("\\end{tikzpicture}")

    print("&")

    print("\\begin{tikzpicture}[scale=0.4]")
    print("\\draw[gray,->] (-0.5, 0) -- (13, 0);")
    print("\\draw[gray,->] (0, -0.5) -- (0, 13);")
    printset2d(set(sumset2d(A)))
    print("\\end{tikzpicture}")

    print("&")

    print("\\begin{tikzpicture}[scale=0.4]")
    print("\\draw[gray,<->] (-6, 0) -- (6, 0);")
    print("\\draw[gray,<->] (0, -6) -- (0, 6);")
    printset2d(set(diffset2d(A)))
    print("\\end{tikzpicture}")

    print("\\\\")

def main():
    print("\\documentclass{article}")
    print("\\usepackage{tikz}")
    print("\\usepackage[paperheight=19in,margin=1in]{geometry}")
    print("\\pagestyle{empty}")
    print("\\begin{document}")
    print("\\setlength{\\tabcolsep}{1.5em}")

    print("\\begin{tabular}{ccc}")
    print("$A$ & $A + A$ & $A - A$ \\\\")
    print("\\noalign{\\bigskip}")
    row(-1)
    print("\\noalign{\\bigskip}\\hline\\noalign{\\bigskip}")
    for s in [29468518, 29786750, 97642304, 123260608, 118882942, 119032526]:
        row(s)
        print("\\noalign{\\bigskip}")


    print("\\end{tabular}")
    print("\\end{document}")

if __name__ == '__main__':
    main()
