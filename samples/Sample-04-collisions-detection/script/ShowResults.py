import sys
import os
import argparse
import math
import json
import matplotlib.pyplot as plt
import matplotlib.path as mpath
import matplotlib.patches as mpatches

def import_json(name):
    with open(name) as json_file:
        return json.load(json_file)

class AxisBox:
    def __init__(self):
        self.range=[0,0]

    def extend(self, val):
        if(self.range[1] < val):
            self.range[1] = val
        if(self.range[0] > val):
            self.range[0] = val

    def get(self):
        delta = 0.1* (self.range[1] - self.range[0])
        return [self.range[0] - delta, self.range[1] + delta]

class BoundingBox:
    def __init__(self):
        self.interval_x=AxisBox()
        self.interval_y=AxisBox()

    def extend(self, point):
        self.interval_x.extend(point[0])
        self.interval_y.extend(point[1])

    def printCorners(self, ax):
        range_x = self.interval_x.get()
        range_y = self.interval_y.get()
        ax.plot([range_x[0]],[range_y[0]], 'r')
        ax.plot([range_x[1]],[range_y[1]], 'r')

limits = BoundingBox()

def print_line(ax, start, end):
    segment_x = [start['x'], end['x']]
    segment_y = [start['y'], end['y']]
    ax.plot(segment_x, segment_y, '--', c='k', linewidth=0.5)
    limits.extend([start['x'], start['y']])
    limits.extend([end['x'], end['y']])

def print_patch(ax, vertices, color):
    for vertex in vertices:
        limits.extend(vertex)
    path = mpath.Path(vertices)
    patch = mpatches.PathPatch(path, facecolor=color, alpha=1.0)
    ax.add_patch(patch)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', required=True)
    args = parser.parse_args()

    to_print = import_json(args.file)

    fig, ax = plt.subplots()
    fig.suptitle(args.file, fontsize=14)
    for shape_name in to_print['Shapes']:
        shape = to_print['Shapes'][shape_name]
        print_patch(ax, shape['vertices'], shape['color'])
        # TODO print name of polygons close to them in the figure
    for line in to_print['Lines']:
        print_line(ax, line['diffA'], line['diffB'])

    limits.printCorners(ax)

    ax.axis('equal')
    plt.show()
