from ctypes import *
import os

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

def testSA():
    path = "test/kernel.log"
    genSuffixArray(path)
    ret = search(path, "microcode", 5)
    import pprint
    pprint.pprint(ret, width=200)

testSA()
