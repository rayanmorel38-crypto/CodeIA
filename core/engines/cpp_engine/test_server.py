#!/usr/bin/env python3
"""
Script de test pour valider les endpoints du control server
"""
import requests
import json
import time
import sys
from datetime import datetime

BASE_URL = "http://127.0.0.1:3004"

def print_header(msg):
    print(f"\n{'='*60}")
    print(f"  {msg}")
    print('='*60)

def test_health():
    print_header("Test 1: GET /health")
    try:
        resp = requests.get(f"{BASE_URL}/health", timeout=5)
        print(f"Status: {resp.status_code}")
        print(f"Response:\n{json.dumps(resp.json(), indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_process_submission():
    print_header("Test 2: POST /process - Submit task")
    try:
        payload = {
            "command": ["--help"],
            "timeout": 30
        }
        resp = requests.post(
            f"{BASE_URL}/process",
            json=payload,
            timeout=5
        )
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        
        if resp.status_code == 200 and "task_id" in data:
            return data["task_id"]
        return None
    except Exception as e:
        print(f"❌ Error: {e}")
        return None

def test_status(task_id):
    print_header(f"Test 3: GET /status/{task_id}")
    try:
        resp = requests.get(f"{BASE_URL}/status/{task_id}", timeout=5)
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_tasks_list():
    print_header("Test 4: GET /tasks")
    try:
        resp = requests.get(f"{BASE_URL}/tasks?limit=5", timeout=5)
        print(f"Status: {resp.status_code}")
        data = resp.json()
        print(f"Response:\n{json.dumps(data, indent=2)}")
        return resp.status_code == 200
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_invalid_request():
    print_header("Test 5: POST /process - Invalid request (bad JSON)")
    try:
        resp = requests.post(
            f"{BASE_URL}/process",
            data="invalid json",
            timeout=5
        )
        print(f"Status: {resp.status_code}")
        print(f"Should be 400: {resp.status_code == 400}")
        return resp.status_code == 400
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def main():
    print(f"\n🧪 Testing CPP Engine Control Server (v1.1.0)")
    print(f"Base URL: {BASE_URL}")
    print(f"Time: {datetime.now().isoformat()}")
    
    results = []
    
    # Test health
    results.append(("Health Check", test_health()))
    
    if not results[0][1]:
        print("\n❌ Server not responding! Make sure it's running.")
        print(f"   Start with: ./build/bin/cpp_engine_server")
        return 1
    
    # Test process submission
    task_id = test_process_submission()
    results.append(("Process Submission", task_id is not None))
    
    if task_id:
        # Wait for task to complete
        print("\n⏳ Waiting 2s for task to complete...")
        time.sleep(2)
        
        # Test status
        results.append(("Get Status", test_status(task_id)))
    
    # Test tasks list
    results.append(("List Tasks", test_tasks_list()))
    
    # Test invalid request
    results.append(("Invalid Request Handling", test_invalid_request()))
    
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
