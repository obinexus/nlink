#!/usr/bin/env python3
# component_structure_normalizer.py

import os
import shutil
import re
from pathlib import Path

def normalize_component_structure(src_dir):
    """Normalize the component directory structure by removing recursive build directories"""
    # Find all component directories
    component_dirs = [d for d in Path(src_dir).iterdir() if d.is_dir()]
    
    # Pattern to identify problematic build directories (with multiple _build suffixes)
    build_pattern = re.compile(r'(.+?)(_build){2,}$')
    
    for dir_path in component_dirs:
        dir_name = dir_path.name
        match = build_pattern.match(dir_name)
        
        if match:
            # Extract base component name
            base_component = match.group(1)
            correct_name = f"{base_component}_build"
            
            # Remove problematic directory
            print(f"Removing recursive directory: {dir_path}")
            shutil.rmtree(dir_path)
            
    # Create clean build directories for each core component
    for dir_path in component_dirs:
        if not dir_path.name.endswith("_build") and not dir_path.name.startswith("."):
            build_dir = Path(src_dir) / f"{dir_path.name}_build"
            if not build_dir.exists():
                build_dir.mkdir(exist_ok=True)
                print(f"Created clean build directory: {build_dir}")
                
                # Create minimal CMakeLists.txt for component build directory
                with open(build_dir / "CMakeLists.txt", "w") as f:
                    f.write(f"""# {dir_path.name}_build/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project({dir_path.name}_build LANGUAGES C)

# Import build system utilities
include(${{CMAKE_SOURCE_DIR}}/cmake/ComponentSystem.cmake)

# Register component with build system
file(GLOB COMPONENT_SOURCES "${{CMAKE_SOURCE_DIR}}/src/core/{dir_path.name}/*.c")
nlink_register_component(
    NAME {dir_path.name}
    SOURCES ${{COMPONENT_SOURCES}}
    ENABLE_STATIC TRUE
)
""")

if __name__ == "__main__":
    normalize_component_structure("src/core")
    print("Component directory structure normalized")
