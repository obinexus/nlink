#!/usr/bin/env python3
"""
NexusLink Include Path Standardizer

This script standardizes include paths across the NexusLink codebase to
follow a consistent pattern. It addresses circular dependencies and ensures
proper hierarchical include structure for both src/cli and src/core components.

Usage:
  python standardize_nlink_includes.py --project-root /path/to/nexuslink [OPTIONS]

Options:
  --project-root DIR     Project root directory (required)
  --dry-run              Show what would be changed without modifying files
  --verbose              Enable verbose output
  --backup               Create backup of modified files
  --report-file FILE     Write detailed report to specified file
"""

import os
import re
import sys
import argparse
import logging
import json
import shutil
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from datetime import datetime
from collections import defaultdict

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('nlink-include-standardizer')

class NLinkIncludeStandardizer:
    """Standardizes include paths in the NexusLink codebase."""
    
    def __init__(self, project_root: str, dry_run: bool = False, 
                 verbose: bool = False, backup: bool = False,
                 report_file: Optional[str] = None):
        self.project_root = Path(project_root)
        self.src_dir = self.project_root / "src"
        self.include_dir = self.project_root / "include"
        self.dry_run = dry_run
        self.backup = backup
        self.report_file = report_file
        
        if verbose:
            logger.setLevel(logging.DEBUG)
        
        # Statistics
        self.files_processed = 0
        self.files_modified = 0
        self.includes_processed = 0
        self.includes_modified = 0
        
        # Tracking data
        self.modifications_by_file = defaultdict(list)
        
        # Backup directory
        self.backup_dir = None
        if self.backup and not self.dry_run:
            self._create_backup()
        
        # Build index of header files
        self.header_index = self._build_header_index()
        
        # Initialize standardization patterns
        self._init_patterns()
    
    def _create_backup(self):
        """Create backup of source files before modification."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.backup_dir = self.project_root / f"backup_{timestamp}"
        self.backup_dir.mkdir(exist_ok=True)
        
        logger.info(f"Creating backup in {self.backup_dir}")
        
        # Backup source files
        self._backup_directory(self.src_dir)
        
        # Backup include files
        self._backup_directory(self.include_dir)
        
        logger.info(f"Backup completed")
    
    def _backup_directory(self, directory: Path):
        """Create backup of files in a directory while preserving structure."""
        if not directory.exists():
            return
            
        for file_ext in ['.c', '.h']:
            for file_path in directory.glob(f"**/*{file_ext}"):
                rel_path = file_path.relative_to(self.project_root)
                backup_path = self.backup_dir / rel_path
                backup_path.parent.mkdir(parents=True, exist_ok=True)
                
                try:
                    shutil.copy2(file_path, backup_path)
                except Exception as e:
                    logger.error(f"Error backing up {file_path}: {e}")
    
    def _build_header_index(self) -> Dict[str, List[str]]:
        """
        Build an index of all header files in the project.
        Maps header filename to canonical include paths.
        """
        index = {}
        
        # Process include directory
        if self.include_dir.exists():
            for header_file in self.include_dir.glob("**/*.h"):
                # Get the canonical include path (relative to include directory)
                rel_path = header_file.relative_to(self.include_dir)
                canonical_path = str(rel_path)
                filename = header_file.name
                
                if filename not in index:
                    index[filename] = []
                
                index[filename].append(canonical_path)
                logger.debug(f"Indexed header: {filename} -> {canonical_path}")
        
        # Also index headers in src directory (for implementation-specific headers)
        if self.src_dir.exists():
            for header_file in self.src_dir.glob("**/*.h"):
                # For src headers, we'll use a src/ prefix
                rel_path = header_file.relative_to(self.src_dir)
                src_path = f"src/{rel_path}"
                filename = header_file.name
                
                if filename not in index:
                    index[filename] = []
                
                index[filename].append(src_path)
                logger.debug(f"Indexed src header: {filename} -> {src_path}")
        
        logger.info(f"Built index of {sum(len(paths) for paths in index.values())} unique header paths")
        return index
    
    def _init_patterns(self):
        """Initialize standardization patterns for NexusLink include structure."""
        # Define canonical module paths
        self.module_paths = {
            # Core components
            "core": "nlink/core",
            "common": "nlink/core/common",
            "metadata": "nlink/core/metadata",
            "minimizer": "nlink/core/minimizer",
            "symbols": "nlink/core/symbols",
            "versioning": "nlink/core/versioning",
            "schema": "nlink/core/schema",
            
            # CLI components
            "cli": "nlink/cli",
            "commands": "nlink/cli/commands",
        }
        
        # Common transformations for include paths
        self.transformations = [
            # Fix duplicate segments
            (r"nlink/nlink/", r"nlink/"),
            (r"core/core/", r"core/"),
            (r"cli/cli/", r"cli/"),
            
            # Fix nested structure issues
            (r"^nlink/core/core/", r"nlink/core/"),
            (r"^src/core/core/", r"src/core/"),
            
            # Convert direct includes to proper hierarchical structure
            (r"^(nexus_[^/]+\.h)$", r"nlink/core/common/\1"),
            (r"^(okpala_[^/]+\.h)$", r"nlink/core/minimizer/\1"),
            
            # Convert module-based includes to full paths
            (r"^core/([^/]+)/(.+)$", r"nlink/core/\1/\2"),
            (r"^core/(.+)$", r"nlink/core/\1"),
            (r"^cli/(.+)$", r"nlink/cli/\1"),
            
            # Fix "lazy" includes specifically
            (r"^lazy\.h$", r"nlink/core/common/lazy.h"),
            (r"^lazy_versioned\.h$", r"nlink/core/versioning/lazy_versioned.h"),
            (r"^lazy_legacy\.h$", r"nlink/core/common/lazy_legacy.h"),
            
            # Fix common direct includes
            (r"^json\.h$", r"nlink/core/common/json.h"),
            (r"^command\.h$", r"nlink/core/common/command.h"),
            (r"^types\.h$", r"nlink/core/common/types.h"),
            (r"^result\.h$", r"nlink/core/common/result.h"),
        ]
        
        # Direct mappings for special files
        self.direct_mappings = {
            "nlink.h": "nlink/core/nlink.h",
            "command.h": "nlink/core/common/command.h",
            "types.h": "nlink/core/common/types.h",
            "result.h": "nlink/core/common/result.h",
            "json.h": "nlink/core/common/json.h",
            "lazy.h": "nlink/core/common/lazy.h",
            "lazy_legacy.h": "nlink/core/common/lazy_legacy.h",
            "lazy_versioned.h": "nlink/core/versioning/lazy_versioned.h",
            "nlink_cli.h": "nlink/cli/nlink_cli.h",
            "nexus_core.h": "nlink/core/common/nexus_core.h",
            "nexus_json.h": "nlink/core/common/nexus_json.h",
            "nexus_loader.h": "nlink/core/common/nexus_loader.h",
            "nexus_version.h": "nlink/core/versioning/nexus_version.h",
            "nexus_symbols.h": "nlink/core/symbols/nexus_symbols.h",
            "forward_decl.h": "nlink/core/common/forward_decl.h",
        }
    
    def find_all_source_files(self) -> List[Path]:
        """Find all C source and header files in the project."""
        source_files = []
        
        # Find C source files in src directory
        if self.src_dir.exists():
            for ext in ['.c', '.h']:
                source_files.extend(self.src_dir.glob(f"**/*{ext}"))
        
        # Find headers in include directory
        if self.include_dir.exists():
            for ext in ['.h']:
                source_files.extend(self.include_dir.glob(f"**/*{ext}"))
        
        return sorted(source_files)
    
    def standardize_include_path(self, include_path: str, file_path: Optional[Path] = None) -> str:
        """
        Standardize an include path to follow NexusLink conventions.
        
        Args:
            include_path: The include path to standardize
            file_path: The source file containing the include (for context)
            
        Returns:
            The standardized include path
        """
        # System includes should not be modified
        if include_path.startswith('<') and include_path.endswith('>'):
            return include_path
        
        # Direct mappings take precedence
        if include_path in self.direct_mappings:
            return self.direct_mappings[include_path]
        
        # Apply transformations in sequence
        standardized = include_path
        for pattern, replacement in self.transformations:
            if re.match(pattern, standardized):
                old_path = standardized
                standardized = re.sub(pattern, replacement, standardized)
                if old_path != standardized:
                    logger.debug(f"Applied transformation: {old_path} → {standardized}")
                    break  # Apply one transformation at a time
        
        # If path is just a filename and not standardized yet, try to find in header index
        if '/' not in standardized and standardized in self.header_index:
            paths = self.header_index[standardized]
            if len(paths) == 1:
                # If there's only one occurrence, use that path
                standardized = paths[0]
                logger.debug(f"Resolved via header index: {include_path} → {standardized}")
            elif len(paths) > 1 and file_path:
                # If multiple matches, try to determine best match based on file context
                file_module = self._get_module_from_path(file_path)
                if file_module:
                    # Try to find a path in the same module
                    for path in paths:
                        if file_module in path:
                            standardized = path
                            logger.debug(f"Resolved via context: {include_path} → {standardized}")
                            break
        
        return standardized
    
    def _get_module_from_path(self, file_path: Path) -> Optional[str]:
        """
        Determine the module a file belongs to based on its path.
        Returns module name or None if undetermined.
        """
        if not file_path:
            return None
            
        # Check if path contains src or include
        if 'src' in file_path.parts:
            # Get path relative to src directory
            try:
                rel_path = file_path.relative_to(self.src_dir)
                if len(rel_path.parts) > 0:
                    return rel_path.parts[0]  # First directory is the module
            except ValueError:
                pass
                
        elif 'include' in file_path.parts:
            # Get path relative to include directory
            try:
                rel_path = file_path.relative_to(self.include_dir)
                if len(rel_path.parts) > 1 and rel_path.parts[0] == 'nlink':
                    return rel_path.parts[1]  # Second directory after nlink is the module
            except ValueError:
                pass
        
        return None
    
    def process_file(self, file_path: Path) -> bool:
        """
        Process a single file to standardize its include paths.
        Returns True if the file was modified.
        """
        self.files_processed += 1
        logger.debug(f"Processing file: {file_path}")
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
            
            original_content = content
            
            # Find all include statements
            include_pattern = re.compile(r'#\s*include\s+"([^"]+)"')
            
            # Process all includes
            matches = include_pattern.findall(content)
            self.includes_processed += len(matches)
            
            modifications = []
            for include_path in matches:
                standardized = self.standardize_include_path(include_path, file_path)
                
                if standardized != include_path:
                    # Create pattern with proper escaping for replacement
                    old_include = f'#include "{include_path}"'
                    new_include = f'#include "{standardized}"'
                    
                    # Apply the replacement
                    if old_include in content:
                        content = content.replace(old_include, new_include)
                        self.includes_modified += 1
                        
                        # Record the modification
                        modifications.append({
                            'original': include_path,
                            'standardized': standardized
                        })
                        
                        logger.debug(f"Standardized: {include_path} → {standardized}")
            
            # Save changes if the file was modified
            if content != original_content and not self.dry_run:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                
                self.files_modified += 1
                logger.info(f"Updated {len(modifications)} includes in {file_path}")
            
            # Record modifications for reporting
            if modifications:
                rel_path = str(file_path.relative_to(self.project_root))
                self.modifications_by_file[rel_path] = modifications
                
                # Log in dry-run mode
                if self.dry_run:
                    logger.info(f"Would standardize {len(modifications)} includes in {file_path}")
                    for mod in modifications:
                        logger.info(f"  {mod['original']} → {mod['standardized']}")
            
            return content != original_content
            
        except Exception as e:
            logger.error(f"Error processing {file_path}: {e}")
            return False
    
    def run(self) -> bool:
        """Run the standardization process on all files."""
        logger.info(f"Starting include path standardization in {'dry-run' if self.dry_run else 'actual'} mode")
        source_files = self.find_all_source_files()
        logger.info(f"Found {len(source_files)} source files to process")
        
        for file_path in source_files:
            self.process_file(file_path)
            
            # Log progress for large codebases
            if self.files_processed % 100 == 0:
                logger.info(f"Processed {self.files_processed}/{len(source_files)} files")
        
        # Generate report
        self.print_summary()
        
        if self.report_file:
            self.write_report()
        
        return True
    
    def print_summary(self):
        """Print a summary of the standardization process."""
        logger.info("\n=== Include Path Standardization Summary ===")
        logger.info(f"Files processed: {self.files_processed}")
        logger.info(f"Files modified: {self.files_modified}")
        logger.info(f"Includes processed: {self.includes_processed}")
        logger.info(f"Includes standardized: {self.includes_modified}")
        
        if self.dry_run:
            logger.info("\nThis was a DRY RUN. No files were actually modified.")
            if self.includes_modified > 0:
                logger.info(f"Would have standardized {self.includes_modified} includes in {len(self.modifications_by_file)} files.")
    
    def write_report(self):
        """Write a detailed report to a file."""
        try:
            with open(self.report_file, 'w', encoding='utf-8') as f:
                f.write("# NexusLink Include Path Standardization Report\n\n")
                f.write(f"- Files processed: {self.files_processed}\n")
                f.write(f"- Files modified: {len(self.modifications_by_file)}\n")
                f.write(f"- Includes processed: {self.includes_processed}\n")
                f.write(f"- Includes standardized: {self.includes_modified}\n\n")
                
                if self.modifications_by_file:
                    f.write("## Files with Standardized Includes\n\n")
                    
                    for file_path, modifications in sorted(self.modifications_by_file.items()):
                        f.write(f"### {file_path}\n\n")
                        
                        for mod in modifications:
                            f.write(f"- `{mod['original']}` → `{mod['standardized']}`\n")
                        
                        f.write("\n")
                    
                    # Add standardization rules for reference
                    f.write("\n## Standardization Rules\n\n")
                    
                    f.write("### Module Paths\n\n")
                    for module, path in sorted(self.module_paths.items()):
                        f.write(f"- {module}: `{path}`\n")
                    
                    f.write("\n### Direct Mappings\n\n")
                    for orig, std in sorted(self.direct_mappings.items()):
                        f.write(f"- `{orig}` → `{std}`\n")
                    
                    f.write("\n### Transformation Patterns\n\n")
                    for pattern, replacement in self.transformations:
                        f.write(f"- `{pattern}` → `{replacement}`\n")
            
            logger.info(f"Detailed report written to {self.report_file}")
            
        except Exception as e:
            logger.error(f"Error writing report: {e}")

def main():
    parser = argparse.ArgumentParser(description="NexusLink Include Path Standardizer")
    parser.add_argument("--project-root", required=True, help="Project root directory")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be changed without modifying files")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--backup", action="store_true", help="Create backup of modified files")
    parser.add_argument("--report-file", help="Write detailed report to specified file")
    
    args = parser.parse_args()
    
    standardizer = NLinkIncludeStandardizer(
        project_root=args.project_root,
        dry_run=args.dry_run,
        verbose=args.verbose,
        backup=args.backup,
        report_file=args.report_file
    )
    
    result = standardizer.run()
    return 0 if result else 1

if __name__ == "__main__":
    sys.exit(main())