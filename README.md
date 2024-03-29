DISCLAIMER: This library should not be used for anything malicious or with malicious intent. I will not be held responsible for any damages caused by the mis-use of this library.

This is a useful library for Reverse Engineering enthusiasts and anyone else interested in the Windows API. There are several functions which are rather self-explanatory to use:

MemTools() is a constructor which automatically opens a handle to a process with the inputted access rights.

GetModuleBase() retrieves the module base address of a module inputted by a wide string.

Resolve() adds one offset to the specified address.

ResolveDMA() resolves a pointer chain (several offsets).

ReadSized() reads a specific size instead of using the size of a data type.

Read() reads from an address using the size of the data type in the template for the read size.

Write() writes to an address using the size of the data type in the template for the write size.

Patch() patches several bytes at a specific address.

Nop() patches several nops at a specific address.

PatternScan() uses an IDA-style string to scan for a signature in a process. Returns an offset from the base address.

Example implementation:

```
MemTools mem = MemTools(L"process.exe", PROCESS_ALL_ACCESS);
mem.Write<int>(uintptr_t address, int value);
mem.Patch(uintptr_t address, {0x90, 0x90});
```

Of course do include some error handling. In most cases, if a function fails it will either return NULL, nullptr or false. It will also output a debug string.
