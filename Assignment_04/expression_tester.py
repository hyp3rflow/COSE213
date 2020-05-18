import random
import os
import time
import sys

sys.setrecursionlimit(10000)
            
def process(length):
    result = ""
    cnt = 0
    oper = ['+', '-', '*', '/']
    result += str(random.randint(0, 9))
    for i in range(length):
        if cnt == 0:
            result += str(random.randint(0, 3))
            cnt += 1
        else:
            if random.randint(0, 1) == 0:
                result += str(random.randint(0, 9))
                cnt += 1
            else:
                result += oper[random.randint(0, 3)]
                cnt -= 1
    
    for i in range(cnt):
        result += oper[random.randint(0, 3)]
        
    return result

f = open('./input_test.txt', "w")

for i in range(100):
    result = process(random.randint(40, 100))
    result += "\n"

    f.write(result)
    
f.close()