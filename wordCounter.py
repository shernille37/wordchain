def isFloat(num):
    try:
        float(num)
        return True
    except ValueError:
        return False

def wordCount():
    cnt = 0
    with open("output.txt", mode="r", encoding='utf-8') as F:
        for row in F: cnt += len(row.split())
    
    print("Count", cnt)

def checkProbs():
    with open("output.csv", mode="r", encoding='utf-8') as F:
        for row in F:
            probs = 0
            line = row.split(',')

            for elt in line:
                if isFloat(elt):
                    probs += float(elt)
            
            print(probs)
            

wordCount()