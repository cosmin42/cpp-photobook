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


def get_lipo_commands(folder1, folder2, dstFolder):
    (common_files, different_files) = list_common_files(folder1, folder2)

    if len(different_files) > 0:
        exit(1)

    if common_files:
        command_list = []
        for file in common_files:
            command_content = ("lipo -create -output " + os.path.join(dstFolder, (file.replace(".a", "") + "-macos.a")) + " ")
            command_content = command_content + (" " + os.path.join(folder1, file) + " ")
            command_content = command_content + (" " + os.path.join(folder2, file))

            command_list.append(command_content)
        return command_list
    else:
        raise


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="List files with the same names in two folders.")
    parser.add_argument("folder1", type=str, help="Path to the first folder")
    parser.add_argument("folder2", type=str, help="Path to the second folder")
    parser.add_argument("folder3", type=str, help="Destination folder name")
    

    args = parser.parse_args()
    command_list = get_lipo_commands(args.folder1, args.folder2, args.folder3)

    print("mkdir pb/build")
    os.system("mkdir pb/build")

    for command_content in command_list:
        print(command_content)
        os.system(command_content)

    main_command = get_lipo_commands("pb/x64-macos", "pb/arm64-macos", args.folder3)

    print(main_command[0])
    os.system(main_command[0])
    