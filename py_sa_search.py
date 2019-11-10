from ctypes import *
import os
import timeit

base = os.path.dirname(os.path.abspath(__file__))
SA = cdll.LoadLibrary(os.path.join(base, "out/sa.so"))

def genSuffixArray(path):
    cpath = create_string_buffer(path.encode("utf-8", "ignore"))
    SA.genSA(cpath)
    output = "%s.sa" % path
    if not os.path.isfile(output):
        print("Error ocurred while generating SA");
        return None
    return output

def search(path, word, cnt):
    output = "%s.sa" % path
    if not os.path.isfile(output):
        print("SA should be generated before searching");
        return None

    cpath = create_string_buffer(path.encode("utf-8", "ignore"))
    cword = create_string_buffer(word.encode("utf-8", "ignore"))
    res = (c_int*cnt)()
    SA.search(cpath, cword, res, cnt)
    indices = [x for x in res]

    ret = {}
    with open(path, "rb") as f:
        text = f.read();
        for index in indices:
            if index == -1:
                break
            s = text.rfind(b"\n", 0, index)+1
            e = text.find(b"\n", index)
            if not s in ret:
                ret[s] = text[s:e]
    return sorted(ret.items())

def prepareSA():
    path = "test/kernel.log"
    genSuffixArray(path)

def testSA():
    path = "test/kernel.log"
    ret = search(path, "@!@", 30)

def testLinear():
    path = "test/kernel.log"
    word = b"@!@"
    ret = {}
    with open(path, "rb") as f:
        text = f.read();
        indices = []
        pos = 0
        while len(indices) < 30:
            pos = text.find(word, pos+1)
            if pos == -1: break
            indices.append(pos)
        for index in indices:
            if index == -1:
                break
            s = text.rfind(b"\n", 0, index)+1
            e = text.find(b"\n", index)
            if not s in ret:
                ret[s] = text[s:e]

# prepareSA()
print("Linear", timeit.timeit(testLinear, number=1))
print("SA", timeit.timeit(testSA, number=1))

