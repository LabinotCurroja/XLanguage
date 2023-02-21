# X Language

> A programming language i wrote because i would love a language that is C-like, but is a little more productive than C. It is written without any third party dependencies and runs in its own virtual machine, which means it is portable as well. 
>
> Languages today are too subjective in my opinion, meaning the authors typically engrave their footprint into them. 
> Take Rust for example
> ```Rust 
> fn is_divisible_by(lhs: u32, rhs: u32) -> bool {}
> ```
> - The arguments in the function are denoted as (param : type)
> Theres no reason for this. All it does is add an extra character ":" - also "fn" which is the keyword for a function, which in this case returns -> bool - This is too subjective. 
> 
> The function could be rewritten as follows
> ```C
> bool is_divisible_by(int lhs, int rhs){}
> ``` 
> This is simpler and more readable. There is no need for "->" or ":" or writing "fn" or any other syntax sugar that one might come up with.
> 
> I believe C is still the best language ever written, if it could just be a little more productive. Simple things such as string manipulation is "hard". I also want to remove the need for pointers and memory allocation/freeing. 
