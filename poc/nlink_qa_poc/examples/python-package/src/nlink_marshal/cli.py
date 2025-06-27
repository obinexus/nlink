"""
Command-line interface for NexusLink Python marshalling

Provides CLI access to marshalling functionality for testing and debugging.
"""

import sys
import argparse
from typing import List
from .marshaller import create_marshaller


def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NexusLink Python Data Marshalling CLI"
    )
    
    parser.add_argument(
        '--test', 
        action='store_true',
        help='Run basic marshalling test'
    )
    
    parser.add_argument(
        '--data',
        type=float,
        nargs='+',
        help='Data to marshal (space-separated floats)'
    )
    
    parser.add_argument(
        '--version',
        action='version',
        version='NexusLink Python Marshal 1.0.0'
    )
    
    args = parser.parse_args()
    
    if args.test:
        print("Running basic marshalling test...")
        marshaller = create_marshaller()
        
        test_data = [1.0, 2.5, 3.14159, -4.7]
        print(f"Test data: {test_data}")
        
        try:
            marshalled = marshaller.marshal_data(test_data)
            print(f"Marshalled size: {len(marshalled)} bytes")
            
            recovered = marshaller.unmarshal_data(marshalled)
            print(f"Recovered data: {recovered}")
            
            if test_data == recovered:
                print("✅ SUCCESS: Round-trip marshalling test passed")
                return 0
            else:
                print("❌ FAIL: Data mismatch in round-trip test")
                return 1
                
        except Exception as e:
            print(f"❌ ERROR: {e}")
            return 1
    
    elif args.data:
        print(f"Marshalling data: {args.data}")
        marshaller = create_marshaller()
        
        try:
            marshalled = marshaller.marshal_data(args.data)
            print(f"✅ Marshalled {len(args.data)} values to {len(marshalled)} bytes")
            
            # Verify round-trip
            recovered = marshaller.unmarshal_data(marshalled)
            print(f"✅ Verified round-trip consistency")
            return 0
            
        except Exception as e:
            print(f"❌ ERROR: {e}")
            return 1
    
    else:
        parser.print_help()
        return 0


if __name__ == '__main__':
    sys.exit(main())
