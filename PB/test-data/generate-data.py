import os
import sys

def createAlphabetList(start, end):
    it = start
    alphabetList = []
    while(it != end):
        alphabetList.append(it)
        it = chr(ord(it)+1)
    
    return alphabetList

root = sys.argv[1] +  os.sep + "file-mapping-test"
root = root.replace("/", os.sep)
root = root.replace("\\", os.sep)

try:
    os.mkdir(root)
except:
    pass



topList = createAlphabetList('A', 'Z')

for folderName in topList:
    try:
        os.mkdir(root + os.sep + folderName)
    except:
        pass

    for i in range(12, 17):
        f = open(root + os.sep + folderName + os.sep + str(i) +  ".JPG", 'w')
        f.close()


for i in range(20):
    f = open(root + os.sep + str(i) + ".JPG", 'w')
    f.close()


level2List = createAlphabetList('A', 'K')

for folderName in level2List:
    try:
        os.mkdir(root + os.sep + "A" + os.sep + folderName)
    except:
        pass

for i in range(5, 20):
    for folderName in ['A', 'B', 'C']:
        f = open(root + os.sep + "A" + os.sep + folderName + os.sep + str(i) + ".JPG", 'w')
        f.close()

    f = open(root + os.sep + "A" + os.sep + str(i) + ".JPG", 'w')
    f.close()