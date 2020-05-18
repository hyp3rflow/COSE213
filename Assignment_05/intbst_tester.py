'''
201R COSE213 Assignment_05 Tester
writer: hyperflow@kakao.com

to avoid Unicode problem,
    $ export PYTHONIOENCODING=UTF-8
'''

from pwn import *
from ctypes import *
from filecmp import cmp
import random
import sys

matchcnt = 0
epochsize = 50

for _ in range(epochsize):
    number = 30
    output_1 = open("./output1.txt", "w")
    output_2 = open("./output2.txt", "w")

    input_list = []
    random.shuffle(input_list)

    c = CDLL("/lib/x86_64-linux-gnu/libc.so.6")
    p = process("./intbst", stdout=output_1)
    t = process("./intbst")
    t.sendline(str(number))
    p.sendline(str(number))
    randkey = c.time(0)

    result = str(t.recvline())
    t.close()
    result = result.split()
    for i in result:
        if i.isdigit():
            input_list.append(int(i))

    for i in range(random.randint(0, 10)):
        input_list.append(random.randint(0, number*3))

    random.shuffle(input_list)

    for i in input_list:
        p.sendline(str(i))
    sleep(5)
    p.close()
    output_1.close()

    t = process("./randtest.o", stdout=output_2)
    t.sendline(str(randkey))
    t.sendline(str(number))

    for i in input_list:
        t.sendline(str(i))
    sleep(5)
    t.close()
    output_2.close()

    if cmp('./output1.txt', './output2.txt'):
        matchcnt += 1
        print("\n" + "=" * 30 + "\n")
        print(f"\n\tMatching! {matchcnt} / {epochsize}\n")
        print("\n" + "=" * 30 + "\n")
    else:
        print("\n$ Doesn't Matching!\n")
        break

print(f"\n\tepoch evaluation complete, {matchcnt} / {epochsize}\n\n")
