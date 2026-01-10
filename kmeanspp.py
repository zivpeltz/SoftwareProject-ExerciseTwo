import numpy as np
import pandas as pd
import sys




def main():
    #reading user CMD arguments
    if len(sys.argv) == 1 or len(sys.argv) > 6:
        print("An Error Has Occurred")
        raise SystemExit(1)

    k = sys.argv[1]
    if not k.isdigit():
        print("Incorrect number of clusters!")
        raise SystemExit(1)

    k = int(k)

    if len(sys.argv)==5:
        iter = sys.argv[2]
        if not iter.isdigit():
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
        iter = int(iter)
        if iter < 2 or iter > 799:
            print("Incorrect maximum iteration!")
            raise SystemExit(1)
    else:
        iter = 400
    eps=argv[3]


if __name__=="__main__":
    main()