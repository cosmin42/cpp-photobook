import os


try:
    os.mkdir("file-mapping-test")
except:
    pass

def createAlphabetList(start, end):
    it = start
    alphabetList = []
    while(it != end):
        alphabetList.append(it)
        it = chr(ord(it)+1)
    
    return alphabetList

topList = createAlphabetList('A', 'Z')

for folderName in topList:
    try:
        os.mkdir("file-mapping-test/" + folderName)
    except:
        pass

    for i in range(12, 17):
        f = open("file-mapping-test/" + folderName + "/" + str(i) +  ".JPG", 'w')
        f.close()


for i in range(20):
    f = open("file-mapping-test/" + str(i) + ".JPG", 'w')
    f.close()


level2List = createAlphabetList('A', 'K')

for folderName in level2List:
    try:
        os.mkdir("file-mapping-test/A/" + folderName)
    except:
        pass

for i in range(5, 20):
    for folderName in ['A', 'B', 'C']:
        f = open("file-mapping-test/A/" + folderName + "/" + str(i) + ".JPG", 'w')
        f.close()

    f = open("file-mapping-test/A/" + str(i) + ".JPG", 'w')
    f.close()