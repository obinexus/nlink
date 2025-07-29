#!/bin/bash
# bootstrap_nlink_project.sh - Self-contained implementation

# Environment detection and adaptation
if [ -d "/mnt/c" ]; then
  echo "WSL environment detected - adapting paths"
  PROJECT_ROOT="/mnt/c/Users/OBINexus/Projects/nlink-examples"
else
  PROJECT_ROOT="$(pwd)"
fi

PROJECT_NAME="my-nlinked-sdl-project"
PROJECT_PATH="${PROJECT_ROOT}/${PROJECT_NAME}"

# Create directory structure with platform-appropriate paths
mkdir -p "${PROJECT_PATH}"/{sound-processing,game-windows}/{include,src,nlink}
mkdir -p "${PROJECT_PATH}"/include
mkdir -p "${PROJECT_PATH}"/src
mkdir -p "${PROJECT_PATH}"/build/{bin,obj,lib}

echo "Created project structure at: 
${PROJECT_PATH}"
