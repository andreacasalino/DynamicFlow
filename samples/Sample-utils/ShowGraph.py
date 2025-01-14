import os, argparse

def print_graph(path):
    with open(path, 'r') as stream:
        data = stream.read()
    
    try:
        import graphviz        
        _, fileNameBase = os.path.split(path)
        dotGraph = graphviz.Source(data)
        dotGraph.render('Rendered/' + fileNameBase, view=True).replace('\\', '/')

    except ImportError:
        print("==================================================================================================================================")
        print("===== !!!!!! graphviz was not found on your system. Install it with 'pip install graphviz' or paste the below content into https://dreampuf.github.io/GraphvizOnline/")
        print(data)
        print("==================================================================================================================================") 

    input("Press Enter to continue...")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--filename', required=True)
    args = parser.parse_args()

    print_graph(args.filename)
