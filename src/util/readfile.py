# coding=utf-8
import getopt
import os
import sys
import time
from multiprocessing import Pool

threadsNum = int(1)
output = "output.csv"
filePath = ""
fileDir = str()  # unused


def iter_count(file_name):
    from itertools import (takewhile, repeat)
    buffer = 1024 * 1024
    with open(file_name) as f:
        buf_gen = takewhile(lambda x: x, (f.read(buffer)
                                          for _ in repeat(None)))
        return sum(buf.count('\n') for buf in buf_gen)


def usage():
    helpText = "usage:python check.py [-h | --help] [-f | --file] [-o | --output] [-t | --threads] \n" \
               "option description\n" \
               "   -h print help message and exit\n" \
               "   -f csv file path\n" \
               "   -o output file path. default output.csv\n" \
               "   -t threads number default 1\n" \
        # "   -d csv files directory\n" \

    print(helpText)


def parse_option():
    global output, filePath, threadsNum, fileDir

    try:
        opts, args = getopt.getopt(sys.argv[1:], "ho:t:d:f:", [
            "help", "output=", "threads=", "dir=", "file="])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    for opt, value in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-o", "--output"):
            output = value
        elif opt in ("-t", "--threads"):
            threadsNum = int(value)
        elif opt in ("-d", "--dir"):
            fileDir = value
        elif opt in ("-f", "--file"):
            filePath = value
        else:
            assert False, "unhandled option"

    assert len(output) > 0, "add output filepath with -o option"
    assert len(filePath) > 0, "add csv filepath with -f option"


def check(line, out):
    try:
        line = line.split(',')
        # s5-s1 b1-b5
        idx = [38, 34, 30, 26, 22, 20, 24, 28, 32, 36]
        prices = [float(line[x].replace('\t', "").replace(" ", "")) for x in idx]
        for i in range(len(prices) - 1):
            if prices[i] <= 0:
                continue
            if prices[i] <= prices[i + 1]:
                out.write(",".join(line))
                return
    except Exception:
        print("check line failed!")


def check_multithread(pid, filepath, fp1, fp2):
    try:
        outputFile = open(output, "a+")
    except IOError:
        print("open file:%s error!" % output)
        exit(2)
    n = 0
    with open(filepath, "rb") as f:
        if fp1:
            f.seek(fp1 - 1)
            while b"\n" not in f.read(1):
                pass
        while True:
            line = f.readline()
            n += 1
            check(bytes.decode(line), outputFile)
            pos = f.tell()
            if pos >= fp2:
                break
    return n


def main():
    parse_option()

    fileSize = os.path.getsize(filePath)
    perSize = fileSize / threadsNum

    pool = Pool(threadsNum)
    resList = []
    for i in range(threadsNum):
        p1 = int(perSize * i)
        p2 = int(perSize * (i + 1))
        args = [i, filePath, p1, p2]
        res = pool.apply_async(func=check_multithread, args=args)
        resList.append(res)

    pool.close()
    pool.join()

    for x in resList:
        print(x.get())

    return fileSize


if __name__ == "__main__":
    start = time.time()
    fileSize = main()
    end = time.time()
    print("ProcessNum:%d Filesize: %d Total Cost Time: %d" % (threadsNum, fileSize, end - start))
