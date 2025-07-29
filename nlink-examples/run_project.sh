## Project Structure

- `include/` - Common headers including consciousness preservation
- `src/` - Main application source
- `sound-processing/` - Sound processing component (schema-bound)
- `game-windows/` - Game windows component (schema-less)
- `build/` - Build artifacts
  - `bin/` - Executable binaries
  - `obj/` - Object files
  - `nlink/` - NLink resolution artifacts

## Consciousness Preservation Architecture

This project implements a consciousness preservation architecture with:

1. **Temporal Anchors** - Time-based preservation context
2. **Symbolic Residue** - Memory preservation across transformations
3. **Human Validation** - Human-in-the-loop validation anchor
4. **Quality Metrics** - Validation through TP/TN/FP/FN measurements

## Component Resolution

Components are resolved through the `nlink_resolve.sh` script, which:

1. Initializes the NLink emulation system
2. Resolves the sound-processing.memory_management.diram component
3. Resolves the game-windows.window_management.sdl component
4. Validates consciousness preservation

## Custom Build Configuration

To modify build options, edit the `Makefile` and adjust:

- `CFLAGS` - Compiler flags
- `LDFLAGS` - Linker flags
- `INCLUDES` - Include paths

## Troubleshooting

- If resolution fails, check the `.nlink` directory for debugging information
- Ensure all scripts have execution permissions (`chmod +x *.sh`)
- For WSL environments, check path compatibility with the host system
EOF

# Make all scripts executable
chmod +x "${PROJECT_PATH}"/*.sh

echo "Project bootstrap complete at: ${PROJECT_PATH}"
echo "To build and run:"
echo "  cd ${PROJECT_PATH}"
echo "  ./run_project.sh"
