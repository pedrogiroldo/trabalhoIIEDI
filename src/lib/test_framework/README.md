# Test Framework

A lightweight unit testing framework for C99 projects.

## Features

- Simple test registration and execution
- Assertion macros for common test cases
- Clean test output with pass/fail reporting
- Test summary statistics
- Reusable across multiple test files

## Usage

### 1. Include the Framework

```c
#include "../test_framework/test_framework.h"
```

### 2. Write Test Functions

Test functions must return `bool` (true for pass, false for fail):

```c
bool test_example(void) {
    int result = my_function(5);
    ASSERT_EQUAL(result, 10);
    return true;
}
```

### 3. Register and Run Tests

```c
int main(void) {
    // Initialize framework
    test_framework_init();
    
    // Optionally print section headers
    test_print_section("Testing My Module");
    
    // Register tests
    test_register("test_example", test_example);
    test_register("test_another", test_another);
    
    // Run all tests
    int result = test_run_all();
    
    // Cleanup
    test_framework_cleanup();
    
    return result;
}
```

## Assertion Macros

The framework provides several assertion macros:

| Macro | Description |
|-------|-------------|
| `ASSERT_TRUE(condition)` | Asserts that condition is true |
| `ASSERT_FALSE(condition)` | Asserts that condition is false |
| `ASSERT_NULL(ptr)` | Asserts that pointer is NULL |
| `ASSERT_NOT_NULL(ptr)` | Asserts that pointer is not NULL |
| `ASSERT_EQUAL(a, b)` | Asserts that a equals b |
| `ASSERT_NOT_EQUAL(a, b)` | Asserts that a does not equal b |
| `ASSERT_STR_EQUAL(str1, str2)` | Asserts that two strings are equal |

## Example

See [args_handler.spec.c](file:///home/pedrogiroldo/Documentos/Projetos/trabalhoIIEDI/src/lib/args_handler/args_handler.spec.c) for a complete example.

### Compilation

```bash
gcc -std=c99 -fstack-protector-all \
    -I../test_framework \
    -o my_test \
    my_module.spec.c \
    my_module.c \
    ../test_framework/test_framework.c
```

### Running Tests

```bash
./my_test
```

### Expected Output

```
--- Testing My Module ---
========================================
Running Tests
========================================

Running: test_example
  ✓ PASSED
Running: test_another
  ✓ PASSED

========================================
Test Summary:
  Total:  2
  Passed: 2
  Failed: 0
========================================
```

## Limitations

- Maximum of 100 tests per test file (configurable in `test_framework.c`)
- No test fixtures or setup/teardown hooks (yet)
- No parameterized tests
- Tests that call `exit()` cannot be properly tested

## Best Practices

1. **One assertion per test**: Keep tests focused on a single behavior
2. **Descriptive names**: Use clear test function names like `test_function_name_scenario`
3. **Group related tests**: Use `test_print_section()` to organize test output
4. **Return true**: Always return `true` at the end of successful tests
5. **Early return on failure**: Assertion macros return `false` automatically
