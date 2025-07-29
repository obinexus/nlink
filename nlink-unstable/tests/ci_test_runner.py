#!/usr/bin/env python3
"""
NexusLink CI Test Runner
Cross-platform test runner for CI environments with JUnit XML output
Copyright © 2025 OBINexus Computing
"""

import os
import sys
import argparse
import subprocess
import platform
import multiprocessing
import xml.etree.ElementTree as ET
from datetime import datetime

# ANSI color codes for terminals that support it
class Colors:
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    CYAN = '\033[96m'
    END = '\033[0m'

    @staticmethod
    def colored(text, color):
        if sys.stdout.isatty():  # Only apply colors when terminal supports it
            return f"{color}{text}{Colors.END}"
        return text

def print_banner():
    banner = f"""
  _   _ _     _       _    
 | \\ | | |   (_)     | |   
 |  \\| | |    _ _ __ | | __
 | . ` | |   | | '_ \\| |/ /
 | |\\  | |___| | | | |   < 
 |_| \\_|\\_____/_|_| |_|_|\\_\\
                           
{Colors.colored("NexusLink CI Test Runner", Colors.BLUE)}
{Colors.colored("Copyright © 2025 OBINexus Computing", Colors.CYAN)}
"""
    print(banner)

def section(title):
    print(f"\n{Colors.colored(f'==== {title} ====', Colors.BLUE)}")

def success(message):
    print(f"{Colors.colored('✓', Colors.GREEN)} {message}")

def warning(message):
    print(f"{Colors.colored('!', Colors.YELLOW)} {message}")

def error(message):
    print(f"{Colors.colored('✗', Colors.RED)} {message}")

def get_project_root():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(script_dir)

def get_cpu_count():
    try:
        return multiprocessing.cpu_count()
    except:
        return 2  # Default to 2 if we can't detect

def configure_project(build_dir, project_root, build_type, cmake_options):
    section("Configuring Project")
    
    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
        success(f"Created build directory: {build_dir}")
    
    # Build the CMake command
    cmake_cmd = ["cmake"]
    cmake_cmd.append(f"-DCMAKE_BUILD_TYPE={build_type}")
    
    # Add any additional CMake options
    for option in cmake_options:
        cmake_cmd.append(option)
    
    cmake_cmd.append(project_root)
    
    # Run CMake
    print(f"Running: {' '.join(cmake_cmd)}")
    try:
        subprocess.run(cmake_cmd, cwd=build_dir, check=True)
        success(f"Project configured with build type: {build_type}")
    except subprocess.CalledProcessError as e:
        error(f"Configuration failed with exit code {e.returncode}")
        sys.exit(1)

def build_project(build_dir, build_type, parallel):
    section("Building Tests")
    
    cpu_count = get_cpu_count() if parallel else 1
    
    # Create build command based on platform
    if platform.system() == "Windows":
        build_cmd = ["cmake", "--build", ".", "--config", build_type, "--parallel", str(cpu_count)]
    else:
        build_cmd = ["cmake", "--build", ".", "--config", build_type, "--parallel", str(cpu_count)]
    
    # Run build
    print(f"Running: {' '.join(build_cmd)}")
    try:
        subprocess.run(build_cmd, cwd=build_dir, check=True)
        success("Tests built successfully")
    except subprocess.CalledProcessError as e:
        error(f"Build failed with exit code {e.returncode}")
        sys.exit(1)

def run_tests(build_dir, test_filter, parallel, verbose, junit_xml):
    section("Running Tests")
    
    # Prepare CTest command
    ctest_cmd = ["ctest"]
    
    if parallel:
        ctest_cmd.extend(["-j", str(get_cpu_count())])
    
    if verbose:
        ctest_cmd.append("-V")
    else:
        ctest_cmd.append("--output-on-failure")
    
    if test_filter:
        ctest_cmd.extend(["-R", test_filter])
        warning(f"Running tests matching: {test_filter}")
    else:
        warning("Running all tests")
    
    # Add JUnit XML output if requested
    output_file = "test_output.log"
    if junit_xml:
        ctest_cmd.extend(["--output-junit", junit_xml])
    
    # Show tests that will be run
    try:
        test_list = subprocess.check_output(["ctest", "--show-only"], cwd=build_dir, text=True)
        for line in test_list.splitlines():
            if "Test #" in line:
                print(line.strip())
    except subprocess.CalledProcessError:
        warning("Could not get test list")
    
    # Run tests
    print(f"\nRunning: {' '.join(ctest_cmd)}")
    with open(os.path.join(build_dir, output_file), "w") as log_file:
        try:
            subprocess.run(ctest_cmd, cwd=build_dir, stdout=log_file, stderr=subprocess.STDOUT)
            # Note: We don't check=True here because we want to handle test failures ourselves
        except Exception as e:
            error(f"Error running tests: {e}")
            sys.exit(1)
    
    # Parse and display results
    try:
        with open(os.path.join(build_dir, output_file), "r") as log_file:
            log_content = log_file.read()
        
        # Count passed and failed tests
        pass_count = log_content.count("Test Passed")
        fail_count = log_content.count("Test Failed")
        total_count = pass_count + fail_count
        
        section("Test Summary")
        print(f"Passed: {pass_count}")
        print(f"Failed: {fail_count}")
        print(f"Total: {total_count}")
        
        # Print details of failed tests
        if fail_count > 0:
            section("Failed Tests")
            # Extract and display failure information
            lines = log_content.splitlines()
            failed_section = False
            for i, line in enumerate(lines):
                if "The following tests FAILED:" in line:
                    failed_section = True
                    print(line)
                    # Print the next several lines with failure details
                    for j in range(1, min(10, len(lines) - i)):
                        if lines[i + j].strip():
                            print(lines[i + j])
                    break
            
            error("Some tests failed.")
            return False
        else:
            success("All tests passed!")
            return True
    except Exception as e:
        error(f"Error parsing test results: {e}")
        return False

def parse_arguments():
    parser = argparse.ArgumentParser(description="NexusLink CI Test Runner")
    parser.add_argument("--build-dir", default="build", help="Build directory (default: 'build')")
    parser.add_argument("--build-type", default="Debug", choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"], 
                      help="Build type (default: Debug)")
    parser.add_argument("--filter", default="", help="Test filter regex pattern")
    parser.add_argument("--sequential", action="store_true", help="Run tests sequentially")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    parser.add_argument("--cmake-option", action="append", default=[], 
                      help="Additional CMake options (can be used multiple times)")
    parser.add_argument("--junit-xml", default="", help="Output test results in JUnit XML format")
    return parser.parse_args()

if __name__ == "__main__":
    print_banner()
    
    args = parse_arguments()
    
    project_root = get_project_root()
    build_dir = os.path.abspath(os.path.join(project_root, args.build_dir))
    
    # Configure, build, and run tests
    configure_project(build_dir, project_root, args.build_type, args.cmake_option)
    build_project(build_dir, args.build_type, not args.sequential)
    tests_passed = run_tests(build_dir, args.filter, not args.sequential, args.verbose, args.junit_xml)
    
    sys.exit(0 if tests_passed else 1)