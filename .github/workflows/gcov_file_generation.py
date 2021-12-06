# import required module
import os
 
def is_gcno_file(file_name):
    return file_name.endswith('.gcno')

exluded_folders = ['_deps']
def is_not_excluded_folder(folder_name):
    for exluded in exluded_folders:
        if(exluded == folder_name):
            return False
    return True

def build_gcov_files(directory):
    # iterate over files in that directory
    for filename in os.listdir(directory):
        f = os.path.join(directory, filename)
        # checking if it is a file
        if os.path.isfile(f):
            if is_gcno_file(f):
                command = "gcov " + f
                print(command)
                os.system(command)
        elif is_not_excluded_folder(f):
            build_gcov_files(f)

root_directory = './'
build_gcov_files(root_directory)
