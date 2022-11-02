import sys
import os
import argparse

try:
    import graphviz                     
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--file')
    args = parser.parse_args()

    fileName = args.file

    def importFile(fileName):
        text_file = open(fileName, "r") 
        data = text_file.read()
        text_file.close()
        return data

    print('===== reading dot file at ' + fileName + ' =====')
    dotGraphSource = importFile(fileName)

    fileNamePrefix, fileNameBase = os.path.split(fileName)
    dotGraph = graphviz.Source(dotGraphSource)
    dotGraph.render('Rendered/' + fileNameBase, view=True).replace('\\', '/')

    input("Press Enter to continue...")

except ImportError:
    print("==================================================================================================================================")
    print("===== !!!!!! graphviz was not found on your system. Install it with 'pip install graphviz' or paste the below content into https://dreampuf.github.io/GraphvizOnline/ \n" +
        dotGraphSource
    )
    print("==================================================================================================================================") 
