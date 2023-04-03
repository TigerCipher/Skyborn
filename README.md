# Skyborn
---
A 3D Vulkan engine being developed mostly for experimental and research purposes

## Dependencies
---
- Vulkan SDK (Currently 1.2.x)

## Conventions
---
- Code owned by Skyborn should be `snake_case`
    - This includes classes, structs, namespaces, functions, variables, etc
    - Exception is template parameters. They should be UpperCamelCase. For example, `template <typename... Args>`
- File names should be UpperCamelCase
    - Headers will be `.h`
    - Source will be `.cpp`
    - No `.inl` files
- Private class attributes should be prefixed with `m_`
- If a function parameter is a pointer, it should be prefixed with `p`
    - *This is more of a recommendation than a requirement*
- Direct initialization should be used, regardless if it is a member, local, global, or constexpr variable
    - `u32 some_variable{ 32 };` not `u32 some_variable = 32;`
    - Excpetions:
        - For loop variables should use `=`... `for(u32 i = 0...`
        - Enums should also use `=`
- Primitive types should be used with the aliases in `Skyborn/src/Defines.h`
    - Exception being the main function, but as this is defined in `Skyborn/src/Skyborn.h` this is a redundant exception
- f32 values ending with no fractional part should be written as `#.0f` and not `#.f`
- Use of classes is generally avoided unless it makes a lot more sense
    - For example, `sky::utl::vector` makes more sense as a class
    - What would be static classes / singletons, like the platform interface, memory interface, etc, make more sense as semi-C style functions in namespaces rather than having classes.
        - Exception being the game class of course, as the user inherits that to create their game
            - *Although this may change to be function pointers instead of being a class*
- `sky::utl::vector` should be used instead of `std::vector`
- `scope<T>` and `ref<T>` should be used instead of `std::unique_ptr<T>` and `std::shared_ptr<T>`
- Unless it is a class being passed (like `utl::vector`) parameters that will be modified should be pointers instead of being passed by reference
    - Valid Examples:
        - `bool some_func(utl::vector<u32>& indices) // Class passed by reference`
        - `bool some_func(vulkan_context* pcontext) // Struct passed as pointer`
    - Reason for this is that for readability I like to be explicit with what I'm doing.
        - If I see `func(a);` and it's a function belonging to Skyborn, then I assume `a` is passed either by value or const reference and not modified
        - Likewise, if I see `func(*a)` then I know the value of pointer `a` is passed by value or const reference and not modified
        - Likewise, if I see `func(&a)` then I know `a` will be modified
- More to come...