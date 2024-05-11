import os
import sys

def add_file_to_folder(folder_path, file_name):
    if not os.path.exists(folder_path):
        print(f"The folder '{folder_path}' does not exist.")
        return
    
    file_path = os.path.join(folder_path, file_name)
    
    if os.path.exists(file_path):
        print(f"The file '{file_name}' already exists in the folder.")
        return
    
    # Create an empty file
    with open(file_path, 'w') as file:
        pass
    
    print(f"File '{file_name}' added.")

def fill_header(file_path, namespace_name, class_name):
    with open(file_path, 'a') as file:
            file.write("#pragma once\n")
            file.write("\n")
            file.write("namespace "+ namespace_name + "\n")
            file.write("{\n")
            file.write("class " + class_name + "\n")
            file.write("{\n")
            file.write("};\n")
            file.write("}\n")

def fill_src(file_path, namespace_name, class_name):
    with open(file_path, 'a') as file:
            file.write("#include <pb/" + class_name + ".h>\n")
            file.write("\n")
            file.write("\n")
            file.write("namespace " + namespace_name + "\n")
            file.write("{\n")
            file.write("}\n")

if __name__ == "__main__":
    # Check if at least two arguments (folder path and file name) are provided
    if len(sys.argv) < 3:
        print("Usage: python script.py <class_name> <namespace_name> [--only-header]")
        sys.exit(1)

    only_header = False
    if "--only-header" in sys.argv:
        only_header = True
    
    class_name = sys.argv[1]
    namespace_name = sys.argv[2]

    h_file = class_name + ".h"
    cpp_file = class_name + ".cpp"
    
    add_file_to_folder(".", "PB\\include\\pb\\" + h_file)
    fill_header("PB\\include\\pb\\" + h_file, namespace_name, class_name)

    if not only_header:
        add_file_to_folder(".", "PB\\src\\" + cpp_file)
        fill_src("PB\\src\\" + cpp_file, namespace_name, class_name)