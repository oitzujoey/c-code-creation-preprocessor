# C Code Creation Preprocessor

CCCP generates C code using Lua scripts that are embedded in the C source files. An example of a generate fragment is shown below.

```c
/*cccp generate
    -- Lua code goes here.
    return '\n\tputs("Hello, world!");\n'
*/
/*cccp begin*/
    // Generated code goes here. This comment will be deleted when cccp is run.
    puts("Hello, world!");
/*cccp end */
```

Anything in the `cccp generate` comment will be executed in a Lua interpreter. The generated code is returned to cccp as a string which is then inserted between `cccp begin` and `cccp end`. Unless otherwise described, this syntax must be followed exactly, otherwise the program will throw errors, and the fragment will not be generated.

An important note is that the space on the end of `cccp end` is currently required to successfully run cccp. The reason for this is simply bad coding practice, but its purpose is to make room for a checksum at the end. Anything between `cccp end` (after the space) and the closing `*/` will be interpreted as a checksum. When cccp is run, it first checks for the checksum. If it is present, it checks to see if the generated C code was modified. If it was, the program skips generation of that fragment. If no checksum exists (like above), operation will proceed as if a good checksum exists. This can be used to override bad checksums.

The goals of this project are to remain fully compatible with most versions of C and to prevent it from becoming a dependency that code will not compile without. This is why cccp commands are commented out, and both generator and generated code is left in the source file.

While this program was written to make C easier to write in certain situations, there is no reason that it couldn't be modified to work with other languages as well.
