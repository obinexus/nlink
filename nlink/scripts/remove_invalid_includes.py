#!/usr/bin/env python3
"""
NexusLink Invalid Include Remover

This script scans the codebase for #include directives that reference non-existent
files and either removes them or reports them depending on configuration.

Usage:
  python remove_invalid_includes.py --project-root /path/to/nexuslink [OPTIONS]

Options:
  --project-root DIR     Project root directory (required)
  --dry-run              Report issues without making changes
  --verbose              Enable verbose output
  --report-file FILE     Write detailed report to specified file
  --strict               Apply stricter validation (check that includes match actual file paths)
"""

import os
import re
import sys
import argparse
import logging
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('nlink-include-validator')

class InvalidIncludeRemover:
    """Finds and removes invalid includes in the NexusLink codebase."""
    
    def __init__(self, project_root: str, dry_run: bool = False, 
                 verbose: bool = False, report_file: Optional[str] = None,
                 strict: bool = False):
        self.project_root = Path(project_root)
        self.src_dir = self.project_root / "src"
        self.include_dir = self.project_root / "include"
        self.dry_run = dry_run
        self.strict = strict
        self.report_file = report_file
        
        if verbose:
            logger.setLevel(logging.DEBUG)
        
        # Statistics
        self.files_processed = 0
        self.files_modified = 0
        self.invalid_includes_found = 0
        self.includes_removed = 0
        
        # Tracking data
        self.invalid_by_file = {}
        
        # Build a catalog of available headers
        self.available_headers = self._build_header_catalog()
    
    def _build_header_catalog(self) -> Dict[str, List[str]]:
        """
        Build a catalog of all available headers in the project.
        Maps header filename to full paths.
        """
        catalog = {}
        
        # Scan include directory
        if self.include_dir.exists():
            for header_file in self.include_dir.glob("**/*.h"):
                filename = header_file.name
                rel_path = str(header_file.relative_to(self.include_dir))
                
                if filename not in catalog:
                    catalog[filename] = []
                
                catalog[filename].append(f"include/{rel_path}")
                logger.debug(f"Cataloged header: {filename} at include/{rel_path}")
        
        # Scan src directory
        if self.src_dir.exists():
            for header_file in self.src_dir.glob("**/*.h"):
                filename = header_file.name
                rel_path = str(header_file.relative_to(self.src_dir))
                
                if filename not in catalog:
                    catalog[filename] = []
                
                catalog[filename].append(f"src/{rel_path}")
                logger.debug(f"Cataloged header: {filename} at src/{rel_path}")
        
        logger.info(f"Built catalog of {sum(len(paths) for paths in catalog.values())} headers in {len(catalog)} unique files")
        return catalog
    
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
    
    def is_valid_include(self, include_path: str) -> bool:
        """
        Check if an include path is valid (file exists).
        
        Args:
            include_path: The include path to validate
            
        Returns:
            True if the include path is valid, False otherwise
        """
        # System includes are always considered valid
        if include_path.startswith('<') and include_path.endswith('>'):
            return True
        
        # In non-strict mode, check if the header name exists
        if not self.strict:
            filename = os.path.basename(include_path)
            if filename in self.available_headers:
                return True
        
        # In strict mode, check if the full path exists
        else:
            # Check in include directory
            if (self.include_dir / include_path).exists():
                return True
            
            # Check in src directory
            if (self.src_dir / include_path).exists():
                return True
            
            # Special case for nlink/... includes - check in include directory
            if include_path.startswith('nlink/'):
                if (self.include_dir / include_path).exists():
                    return True
        
        return False
    
    def process_file(self, file_path: Path) -> bool:
        """
        Process a single file to find and optionally remove invalid includes.
        Returns True if the file was modified.
        """
        self.files_processed += 1
        logger.debug(f"Processing file: {file_path}")
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
                lines = f.readlines()
            
            # Track modifications
            invalid_includes = []
            modified_lines = []
            file_modified = False
            
            # Find include statements
            include_pattern = re.compile(r'#\s*include\s+"([^"]+)"')
            
            for line_num, line in enumerate(lines, 1):
                include_match = include_pattern.search(line)
                
                if include_match:
                    include_path = include_match.group(1)
                    
                    if not self.is_valid_include(include_path):
                        # Invalid include found
                        invalid_includes.append({
                            'line_num': line_num,
                            'include_path': include_path,
                            'line': line.strip()
                        })
                        self.invalid_includes_found += 1
                        
                        # Skip this line when rebuilding the file (effectively removing it)
                        file_modified = True
                        continue
                
                # Keep valid line
                modified_lines.append(line)
            
            # Update file if modified and not in dry-run mode
            if file_modified and not self.dry_run:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.writelines(modified_lines)
                self.files_modified += 1
                self.includes_removed += len(invalid_includes)
                logger.info(f"Removed {len(invalid_includes)} invalid includes from {file_path}")
            
            # Store invalid includes for reporting
            if invalid_includes:
                rel_path = file_path.relative_to(self.project_root)
                self.invalid_by_file[str(rel_path)] = invalid_includes
                
                # Log in dry-run mode
                if self.dry_run:
                    logger.info(f"Found {len(invalid_includes)} invalid includes in {file_path}")
                    for inc in invalid_includes:
                        logger.info(f"  Line {inc['line_num']}: {inc['include_path']}")
            
            return file_modified
            
        except Exception as e:
            logger.error(f"Error processing {file_path}: {e}")
            return False
    
    def run(self) -> bool:
        """Run the invalid include remover on all files."""
        logger.info(f"Starting invalid include removal in {'dry-run' if self.dry_run else 'actual'} mode")
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
        
        return self.invalid_includes_found == 0
    
    def print_summary(self):
        """Print a summary of the removal process."""
        logger.info("\n=== Invalid Include Removal Summary ===")
        logger.info(f"Files processed: {self.files_processed}")
        logger.info(f"Files modified: {self.files_modified}")
        logger.info(f"Invalid includes found: {self.invalid_includes_found}")
        logger.info(f"Includes removed: {self.includes_removed}")
        
        if self.dry_run:
            logger.info("\nThis was a DRY RUN. No files were actually modified.")
            if self.invalid_includes_found > 0:
                logger.info(f"Would have removed {self.invalid_includes_found} invalid includes from {len(self.invalid_by_file)} files.")
    
    def write_report(self):
        """Write a detailed report to a file."""
        try:
            with open(self.report_file, 'w', encoding='utf-8') as f:
                f.write("# NexusLink Invalid Include Report\n\n")
                f.write(f"- Files processed: {self.files_processed}\n")
                f.write(f"- Files with invalid includes: {len(self.invalid_by_file)}\n")
                f.write(f"- Total invalid includes: {self.invalid_includes_found}\n\n")
                
                if self.invalid_includes_found > 0:
                    f.write("## Files with Invalid Includes\n\n")
                    
                    for file_path, includes in sorted(self.invalid_by_file.items()):
                        f.write(f"### {file_path}\n\n")
                        
                        for inc in includes:
                            f.write(f"- Line {inc['line_num']}: `{inc['include_path']}`\n")
                            f.write(f"  ```\n  {inc['line']}\n  ```\n")
                        
                        f.write("\n")
                    
                    # Add catalog information for reference
                    f.write("\n## Header Catalog\n\n")
                    f.write("Available headers in the project:\n\n")
                    
                    for header, paths in sorted(self.available_headers.items()):
                        f.write(f"- `{header}`\n")
                        for path in paths:
                            f.write(f"  - {path}\n")
            
            logger.info(f"Detailed report written to {self.report_file}")
            
        except Exception as e:
            logger.error(f"Error writing report: {e}")

def main():
    parser = argparse.ArgumentParser(description="NexusLink Invalid Include Remover")
    parser.add_argument("--project-root", required=True, help="Project root directory")
    parser.add_argument("--dry-run", action="store_true", help="Report issues without making changes")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--report-file", help="Write detailed report to specified file")
    parser.add_argument("--strict", action="store_true", help="Apply stricter validation")
    
    args = parser.parse_args()
    
    remover = InvalidIncludeRemover(
        project_root=args.project_root,
        dry_run=args.dry_run,
        verbose=args.verbose,
        report_file=args.report_file,
        strict=args.strict
    )
    
    result = remover.run()
    return 0 if result else 1

if __name__ == "__main__":
    sys.exit(main())