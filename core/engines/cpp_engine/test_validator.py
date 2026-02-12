#!/usr/bin/env python3
"""
Test script for SandboxComparator validation endpoints
"""
import requests
import json
import sys
from datetime import datetime

BASE_URL = "http://127.0.0.1:3004"

def print_header(msg):
    print(f"\n{'='*60}")
    print(f"  {msg}")
    print('='*60)

def test_validate_filter():
    print_header("Test 1: POST /validate - Validate Filter Output")
    try:
        payload = {
            "type": "filter",
            "filter_type": "blur",
            "input_file": "/path/to/input.jpg",
            "output_file": "/path/to/output.jpg"
        }
        resp = requests.post(
            f"{BASE_URL}/validate",
            json=payload,
            timeout=10
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_validate_effect():
    print_header("Test 2: POST /validate - Validate Effect Output")
    try:
        payload = {
            "type": "effect",
            "effect_type": "edge_detect",
            "input_file": "/path/to/input.jpg",
            "output_file": "/path/to/output.jpg"
        }
        resp = requests.post(
            f"{BASE_URL}/validate",
            json=payload,
            timeout=10
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_validate_images():
    print_header("Test 3: POST /validate - Compare Images")
    try:
        payload = {
            "type": "images",
            "expected_path": "/path/to/expected.jpg",
            "actual_path": "/path/to/actual.jpg",
            "perceptual": True
        }
        resp = requests.post(
            f"{BASE_URL}/validate",
            json=payload,
            timeout=10
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_batch_validate():
    print_header("Test 4: POST /validate/batch - Batch Validation")
    try:
        payload = {
            "validations": [
                {
                    "type": "filter",
                    "filter_type": "grayscale",
                    "input_file": "/path/to/input1.jpg",
                    "output_file": "/path/to/output1.jpg"
                },
                {
                    "type": "effect",
                    "effect_type": "posterize",
                    "input_file": "/path/to/input2.jpg",
                    "output_file": "/path/to/output2.jpg"
                }
            ]
        }
        resp = requests.post(
            f"{BASE_URL}/validate/batch",
            json=payload,
            timeout=10
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_validate_video():
    print_header("Test 5: POST /validate - Validate Video Output")
    try:
        payload = {
            "type": "video",
            "input_file": "/path/to/input.mp4",
            "output_file": "/path/to/output.mp4",
            "expected_config": {
                "width": 1920,
                "height": 1080,
                "fps": 30
            }
        }
        resp = requests.post(
            f"{BASE_URL}/validate",
            json=payload,
            timeout=10
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def main():
    print(f"\n🧪 Testing SandboxComparator Validation Endpoints")
    print(f"Base URL: {BASE_URL}")
    print(f"Time: {datetime.now().isoformat()}")
    
    # First test health
    print_header("Health Check")
    try:
        resp = requests.get(f"{BASE_URL}/health", timeout=5)
        if resp.status_code != 200:
            print("❌ Server not responding! Make sure it's running.")
            print(f"   Start with: ./build/bin/cpp_engine_server")
            return 1
        print(f"✅ Server healthy: {resp.json()}")
    except Exception as e:
        print(f"❌ Cannot reach server: {e}")
        return 1
    
    results = []
    
    # Run validation tests
    results.append(("Validate Filter", test_validate_filter()))
    results.append(("Validate Effect", test_validate_effect()))
    results.append(("Compare Images", test_validate_images()))
    results.append(("Batch Validation", test_batch_validate()))
    results.append(("Validate Video", test_validate_video()))
    
    # Summary
    print_header("Test Summary")
    for name, passed in results:
        status = "✅ PASS" if passed else "❌ FAIL"
        print(f"{status} - {name}")
    
    total = len(results)
    passed = sum(1 for _, p in results if p)
    print(f"\nTotal: {passed}/{total} tests passed")
    
    return 0 if passed == total else 1

if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(1)
