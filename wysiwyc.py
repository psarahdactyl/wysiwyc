#!/usr/bin/env

# pipeline script - a thing to put the pieces together
'''
Sarah Kushner
September 2018
'''
import os.path
from os import name
import sys
from threading import Thread
from subprocess import Popen, PIPE, call, check_output
from queue import Queue, Empty

import matplotlib.pyplot as plt
import matplotlib.image as mpimg

# taken from https://stackoverflow.com/a/7616401/4293336
def enqueue_output(out, queue):
    for line in iter(out.readline, b''):
        line = line.decode(sys.stdout.encoding)
        queue.put(line)
    out.close()

def run_visualization(dir_name, file_name, line_width):
    # get full path for OverlapView directory
    full_path_overlap_view = os.path.join(dir_name, 'OverlapView')
    print(full_path_overlap_view)

    # get script path and run
    full_path_script = os.path.join(full_path_overlap_view, 'OverlapView.py')
    call(['python', full_path_script, file_name, line_width])

    # get output path and display it
    full_path_output = os.path.join(full_path_overlap_view, 'output.png')
    img = mpimg.imread(full_path_output)
    imgplot = plt.imshow(img)
    plt.show()


def run_exploder(dir_name, file_name):
    # get full path for exploder directory
    full_path_exploder = os.path.join(dir_name, 'exploder_strokes_fills')

    # get script path and run
    full_path_script = os.path.join(full_path_exploder, 'explode_strokes_fills.py')
    call(['python', full_path_script, file_name])

    # get output path
    new_file_name = file_name[:-4] + ('-exploded.svg')
    full_path_output = os.path.join(full_path_exploder, new_file_name)

    return full_path_output


def run_flatten(dir_name, file_name):
    # get full path for flatten directory
    full_path_flatten = os.path.join(dir_name, 'flattening')

    # get executable path and run
    full_path_script = os.path.join(full_path_flatten, 'visible_strokes', 'build')
    if os.name == 'nt':
        os.chdir(full_path_script)
        call(['cmake', '../..', '-T', 'host=x64', '-A', 'x64'])
        os.chdir(dir_name)
        full_path_script = os.path.join(full_path_script, 'flattening.exe')
        
    else:
        os.chdir(full_path_script)
        call(['cmake', '../..'])
        os.chdir(dir_name)
        full_path_script = os.path.join(full_path_script, 'flattening')
    
    # run executable
    p = Popen([full_path_script, file_name], stdout=PIPE, stderr=PIPE, bufsize=64)  
    q = Queue()
    t = Thread(target=enqueue_output, args=(p.stdout, q))
    t.daemon = True # thread dies with the program
    t.start()

    # print output
    while not p.stdout.closed or not q.empty():
        try:
            line = q.get_nowait()
        except Empty:
            continue
        else:
            print(line, end='')

    # get output path
    new_file_name = file_name + ('_flattened.svg')
    full_path_output = os.path.join(full_path_flatten, new_file_name)

    return new_file_name


if __name__ == '__main__':
    # Get the directory name this file is in
    dir_path = os.path.dirname(os.path.realpath(__file__))

    # Read the file
    if sys.argv[1]:
        file_name = sys.argv[1]
        #run_visualization(dir_path, file_name, 5)
        exploded_file_name = run_exploder(dir_path, file_name)
        flattened_file_name = run_flatten(dir_path, file_name)#exploded_file_name)
        #run_visualization(dir_path, file_name, 5)
    else:
        print('You must run this script with one argument, the name of the file you want to see.')