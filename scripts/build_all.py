import os
import subprocess
from sys import platform

def search_src(project_root):
    regenerate_cmake = 0
    src_dir = "src"
    cmake_file = project_root + "/CMakeLists.txt"
    print(f"Looking for new source files in {project_root}/{src_dir}")
    # Get the current list of cpp files from SOURCE_FILES
    with open(cmake_file, "r") as f:
        lines = f.readlines()
        for line in lines:
            if line.startswith("set(SOURCE_FILES"):
                source_files_line = line
                source_files = set(f.strip() for f in line.split()[0:-1] if f.endswith(".cpp"))

    # Find all cpp files in the src directory (recursively)
    cpp_files = set()
    src_folder = project_root + "/" + src_dir
    for dirpath, _, filenames in os.walk(src_folder):
        for f in filenames:
            if f.endswith(".cpp"):
                cpp_files.add(os.path.join(dirpath, f).replace("\\", "/").removeprefix(project_root + "/"))

    # Check if any files were removed
    files_removed = False
    for sf in source_files:
        found = False
        for cf in cpp_files:
            if sf == cf:
                found = True
                break
        if not found:
            files_removed = True
            break
    # if it was detected that a file was removed, to make things simple, just go ahead and rewrite the whole list
    # That way it goes ahead and adds any new source files 
    if files_removed:
        with open(cmake_file, "w") as f:
            for line in lines:
                if line == source_files_line:
                    f.write("set(SOURCE_FILES ")
                    f.write(" ".join(sorted(cpp_files)))
                    f.write(" )\n")
                else:
                    f.write(line)
            print("Source files had been removed, adding everything to SOURCE_FILES.")
            regenerate_cmake = 1
    else:
        # Add any new cpp files to SOURCE_FILES
        new_files = cpp_files - source_files
        if new_files:
            with open(cmake_file, "w") as f:
                for line in lines:
                    if line == source_files_line:
                        f.write("set(SOURCE_FILES ")
                        f.write(" ".join(sorted(source_files.union(new_files))))
                        f.write(" )\n")
                    else:
                        f.write(line)
                print(f"Added {len(new_files)} new cpp file{'s' if len(new_files) > 1 else ''} to SOURCE_FILES.")
                regenerate_cmake = 1
        else:
            print("No new cpp files found.")
    return regenerate_cmake

should_regenerate = 0
should_regenerate += search_src("engine")
should_regenerate += search_src("sandbox")
# testbed += search_src("testbed")


config_workspace_dir = "D:/Skyborn"

ret_code = 0

if should_regenerate > 0:
    print(f"Running CMake...")
    cmake_cmd = ["echo", "Failed to detect platform"]
    if platform == "win32":
        cmake_cmd = ["cmake", "--no-warn-unused-cli", "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE", "-DCMAKE_BUILD_TYPE:STRING=Debug", 
                    "-DCMAKE_C_COMPILER:FILEPATH=C:\\Program Files\\LLVM\\bin\\clang.exe", "-DCMAKE_CXX_COMPILER:FILEPATH=C:\\Program Files\\LLVM\\bin\\clang++.exe", 
                    f"-S{config_workspace_dir}", "-Bbuild", "-G", "Unix Makefiles"]
    str = ""
    for c in cmake_cmd:
        str += c + " "
    print(f"Running: {str}")
    ret_code = subprocess.check_call(cmake_cmd, stderr=subprocess.STDOUT, shell=True)

if ret_code == 0:
    print(f"Building all...")
    cmake_cmd = ["cmake", "--build", "build", "--config", "Debug", "--target", "all", "-j", f"{os.cpu_count() - 4}", "--"]
    str = ""
    for c in cmake_cmd:
        str += c + " "
    print(f"Running: {str}")
    ret_code = subprocess.check_call(cmake_cmd, stderr=subprocess.STDOUT, shell=True)

if ret_code != 0:
    print(f"ERROR: {ret_code}")
else:
    print("Done.")