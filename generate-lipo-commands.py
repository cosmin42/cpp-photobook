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

def run_commands(command_list):
    for command_content in command_list:
        print(command_content)
        os.system(command_content)


if __name__ == "__main__":
    print("mkdir pb/build")
    os.system("mkdir pb/build")

    print("mkdir pb/skia-fat")
    os.system("mkdir pb/skia-fat")

    command_list = get_lipo_commands("vcpkg/installed/arm64-ios/debug/lib", "vcpkg/installed/x64-osx/debug/lib", "pb/build")
    run_commands(command_list)

    command_list = get_lipo_commands("pb/x64-macos", "pb/arm64-macos", "pb/build")
    run_commands(command_list)

    command_list = get_lipo_commands("PB/third-party/osx/skia/arm64", "PB/third-party/osx/skia/x86_64", "PB/skia-fat")
    run_commands(command_list)
    