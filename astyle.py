import os
import subprocess

def format_cpp_files_with_astyle(src_folder, options_file):
    # Get all .cpp files in the src_folder
    cpp_files = []
    for root, dirs, files in os.walk(src_folder):
        for file in files:
            if file.endswith('.cpp'):
                cpp_files.append(os.path.join(root, file))

    # Apply AStyle to each .cpp file
    for cpp_file in cpp_files:
        command = ['astyle', '--options=' + options_file, cpp_file]
        try:
            subprocess.run(command, check=True)
            print(f'Formatted: {cpp_file}')
        except subprocess.CalledProcessError as e:
            print(f'Error formatting {cpp_file}: {e}')

if __name__ == '__main__':
    src_folder = 'src'  # Folder containing .cpp files
    options_file = 'astyle-options.txt'  # AStyle options file

    format_cpp_files_with_astyle(src_folder, options_file)
    