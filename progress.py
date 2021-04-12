from random import random
from math import log, exp

DATA = [
10000,
10000,
10000,
20000,
30000,
10000,
10000,
30000,
10000,
30000,
20000,
10000,
10000,
10000,
20000,
10000,
30000,
20000,
30000,
80000,
10000,
20000,
10000,
10000,
30000,
10000,
10000,
90000,
100000,
20000,
10000,
40000,
80000,
10000,
10000,
10000,
10000,
10000,
30000,
90000,
10000,
40000,
360000,
10000,
20000,
10000,
100000,
350000,
30000,
380000,
10000,
10000,
600000,
20000,
]

DATA = [x/1000000 for x in DATA]

# https://mathworld.wolfram.com/LeastSquaresFittingExponential.html

class Acc:
    def __init__(self):
        self.xxy = 0
        self.ylogy = 0
        self.xy = 0
        self.xylogy = 0
        self.sumy = 0

    def __call__(self, x, y):
        xy1 = x*y
        ylogy1 = y*log(y)
        self.xxy += x*xy1
        self.ylogy += ylogy1
        self.xy += xy1
        self.xylogy += x*ylogy1
        self.sumy += y
        divisor = self.sumy * self.xxy - pow(self.xy, 2)
        try:
            a = (self.xxy * self.ylogy - self.xy * self.xylogy) / divisor 
            b = (self.sumy * self.xylogy - self.xy * self.ylogy) / divisor
            return exp(b*x+a)
        except:
            return 0


def prog(i):
    X = tuple(enumerate(DATA[:i+1]))

    xxy = sum(x*x*y for x, y in X)
    ylogy = sum(y*log(y) for _, y in X)
    xy = sum(x*y for x, y in X)
    xylogy = sum(x*y*log(y) for x, y in X)
    sumy = sum(y for _, y in X)
    divisor = sumy * xxy - pow(xy, 2)

    a = (xxy * ylogy - xy * xylogy) / divisor 
    b = (sumy * xylogy - xy * ylogy) / divisor
    return exp(b*i+a)

acc = Acc()
p = 0
for i in range(len(DATA)):
    p = max(acc(i,DATA[i]), p)
    print(i, DATA[i], p, sep=',')

f = [
acc.xxy,
acc.ylogy,
acc.xy,
acc.xylogy,
acc.sumy,
]
for i in f:
    print(i)

