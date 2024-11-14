import os
import argparse

def list_common_files(folder1, folder2):
    # Get list of files in both folders
    files1 = set(list(filter(lambda x: x.endswith(".a"), os.listdir(folder1))))
    files2 = set(list(filter(lambda x: x.endswith(".a"), os.listdir(folder2))))

    # Find common files
    common_files = files1.intersection(files2)

    # Filter out directories (only include files)
    common_files = [file for file in common_files if os.path.isfile(os.path.join(folder1, file)) and os.path.isfile(os.path.join(folder2, file))]

    different_files = [file for file in common_files if not os.path.isfile(os.path.join(folder1, file)) and os.path.isfile(os.path.join(folder2, file))]

    return (common_files, different_files)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="List files with the same names in two folders.")
    parser.add_argument("folder1", type=str, help="Path to the first folder")
    parser.add_argument("folder2", type=str, help="Path to the second folder")
    parser.add_argument("folder3", type=str, help="Destination folder name")
    

    args = parser.parse_args()

    (common_files, different_files) = list_common_files(args.folder1, args.folder2)

    if len(different_files) > 0:
        exit(1)

    if common_files:
        for file in common_files:
            print("lipo -create -output " + os.path.join(args.folder3, (file.replace(".a", "") + "-macos.a")) + " \\")
            print("    " + os.path.join(args.folder1, file) + " \\")
            print("    " + os.path.join(args.folder2, file))
            print()

    print("mkdir pb/build")

    print("lipo -create -output pb/build/libpblib-macos.a \\")
    print("    pb/x64-macos/libpblib-Darwin.a \\")
    print("    pb/arm64-macos/libpblib-Darwin.a")
)
    
    else:
        print("No common files found.")