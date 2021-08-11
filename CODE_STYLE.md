# Code Style Guidelines
This is a fairly 'adhered-to' set of guidelines for code style within the project. Some outdated code, notable exceptions or oversights to this code style may exist in the project, if in doubt ask about why a bit of code may not follow the code style - but unintuitive exceptions should be remarked in some way.

With that said, this is just a guide to stop things getting too messy and needn't be _too_ rigidly followed.

## `clang-format`
A `.clang-format` file is included in the repository which all code should follow. Be sure to use **at least** `clang-format-11` which can be installed on Debian/Ubuntu with:

```bash
# Try the main clang-format package first -
$ sudo apt install clang-format

# if it suggests that it will install version 10, cancel
# with Ctrl+C and install clang-format-11 forcefully:
$ sudo apt install clang-format-11
```

Then, be sure to enable `clang-format` in your IDE of choice.

---

## General Formatting
Most/all of these should be enforced by the `clang-format` file, but they're here for posterity;

- Use the "west `const`" placement:
  ```c
  const int a = 42; // Yes! West const :)
  int const a = 42; // No... gross!
  ```
- Use left pointer alignment (pointer by the type, not the variable):
  ```c
  const char* myVar = "Hello, world!"; // Yes! :)
  const char *myVar = "Hello, world!"; // No... why?!
  ```
---

## Kernel Naming Conventions
The Thornhill project has three distinct naming conventions that should be followed in this order:

1. **Common functions** - unless there is good cause to rename well-known functions (e.g. different behavior or added functionality), functions should be named according to popular convention.

2. **Low-level C code** - particularly C functions that are exported, such as the GDT/IDT methods, should be named with `lower_snake_case`.

3. **New Thornhill code** - in other words, OS-specific code - should be named as follows:
    - Classes, namespaces or structs should be named with `UpperCamelCase`.
    - Fields, methods, getters/setters, variables and parameters should favor `lowerCamelCase`.
    - Macro or struct type definitions should be `lower_snake_case` and end with `_t` per the POSIX standard, with a notable exception being the additional lowercase `null` type which equates to `0`.

## Kernel Structuring and Organization
Standard methods for either the POSIX/std or Thornhill libraries should be organized under _namespaces_ so they can be easily included at will:
- The POSIX/std library should be organized under the `std` namespace.
- The Thornhill library should be organized under the `Thornhill` namespace.

Internal kernel methods should be organized under the `ThornhillKernel` namespace. **These are not expected to and should not be exposed to user-space applications as ordinary API - they are free to be changed at any time.**

> TODO: finish this section.

---

## Bootloader Naming Conventions
The bootloader follows the UEFI naming conventions:
- Uppercase types such as `UINTN`, `UINT32`, etc., should be used.
- Parameters and variable names should be `UpperCamelCase`.
- Method names should be prefixed with `THB` (**T**horn**h**ill **B**oot).

## Bootloader Structuring and Organization
The `main.c` file for the bootloader should **only** contain the `efi_main` method, with all additional methods being organized into header/class files under relevant directories.