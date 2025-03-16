import os
import argparse
import shutil

def list_common_files(folder1, folder2):
    # Get list of files in both folders
    files1 = set(list(filter(lambda x: x.endswith(".a") or x.endswith(".dylib"), os.listdir(folder1))))
    files2 = set(list(filter(lambda x: x.endswith(".a") or x.endswith(".dylib"), os.listdir(folder2))))

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
            if file.endswith(".dylib"):
                command_content = ("lipo -create -output " + os.path.join(dstFolder, file) + " ")
            else:
                command_content = ("lipo -create -output " + os.path.join(dstFolder, (file.replace(".a", "") + "-macos.a")) + " ")
            command_content = command_content + (" " + os.path.join(folder1, file) + " ")
            command_content = command_content + (" " + os.path.join(folder2, file))

            command_list.append(command_content)
        return command_list
    else:
        print(folder1)
        print(folder2)
        raise

def run_commands(command_list):
    for command_content in command_list:
        print(command_content.split(" ")[-1])
        os.system(command_content)

if __name__ == "__main__":
    print("mkdir osx/PhotoBook/macos")
    os.system("mkdir osx/PhotoBook/macos")

    print("mkdir osx/PhotoBook/macos-debug")
    os.system("mkdir osx/PhotoBook/macos-debug")

    print("mkdir osx/PhotoBook/macos-release")
    os.system("mkdir osx/PhotoBook/macos-release")

    command_list = get_lipo_commands("vcpkg/installed/arm64-osx/debug/lib", "vcpkg/installed/x64-osx/debug/lib", "osx/PhotoBook/macos-debug")
    run_commands(command_list)

    command_list = get_lipo_commands("vcpkg/installed/arm64-osx/lib", "vcpkg/installed/x64-osx/lib", "osx/PhotoBook/macos-release")
    run_commands(command_list)

    command_list = get_lipo_commands("pb/Debug/x64-macos", "pb/Debug/arm64-macos", "osx/PhotoBook/macos-debug")
    run_commands(command_list)

    command_list = get_lipo_commands("pb/Release/x64-macos", "pb/Release/arm64-macos", "osx/PhotoBook/macos-release")
    run_commands(command_list)

    command_list = get_lipo_commands("PB/third-party/osx/skia/arm64", "PB/third-party/osx/skia/x86_64", "osx/PhotoBook/macos")
    run_commands(command_list)

    