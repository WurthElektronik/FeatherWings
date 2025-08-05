import os
import shutil
Import("env")

def copy_c_cpp_properties(*args, **kwargs):
    source_file = os.path.join('.vscode', 'c_cpp_properties.json')
    destination_folder = '../../Common/.vscode'
    destination_file = os.path.join(destination_folder, 'c_cpp_properties.json')

    try:
        if not os.path.isfile(source_file):
            print(f"[Post-Build] Source file not found: {source_file}")
            return

        os.makedirs(destination_folder, exist_ok=True)

        # Delete the destination file if it already exists
        if os.path.exists(destination_file):
            os.remove(destination_file)

        shutil.copy2(source_file, destination_file)
        print(f"[Post-Build] File copied to {destination_file}")

    except Exception as e:
        print(f"[Post-Build] Error: {e}")

env.AddPostAction("buildprog", copy_c_cpp_properties)
