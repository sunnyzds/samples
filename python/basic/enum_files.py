import sys,os


def file_name(file_dir):
    """
    Find the all file path with the directory
    Args:
    file_dir: The source file directory
    Returns:
    files_path: all the file path into a list
    """
    files_path = []

    for t1, t2, files in os.walk(file_dir):
        print("root[%s] dir[%s] files[%s]"%(t1, t2, files))
        for name in files:
            file_path = t1 + "/" + name
            print(file_path)
            files_path.append(file_path)

    return files_path

file_name("./")
