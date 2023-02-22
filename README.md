# X Language

> A programming language i wrote because i would love a language that is C-like, but is a little more productive than C. It is written without any third party dependencies and runs in its own virtual machine, which means it is portable as well. 
>
> Languages today are too subjective in my opinion, meaning the authors typically engrave their footprint into them. 
> Take Rust for example
> ```Rust 
> fn is_divisible_by(lhs: u32, rhs: u32) -> bool {}
> ```
> The arguments in the function are denoted as (param : type)
> 
> All it does is add an extra character ":" - also "fn" which is the keyword for a function, which in this case returns a -> bool - This is too subjective. 
> 
> The function could be rewritten as follows
> ```C
> bool is_divisible_by(int lhs, int rhs){}
> ``` 
> This is simpler and more readable. There is no need for "->" or ":" or writing "fn" or any other syntax sugar that one might come up with.
> 
> I believe C is still the best language ever written, if it could just be a little more productive. Simple things such as string manipulation is "hard". I also want to remove the need for pointers and memory allocation/freeing and let the compiler handle that part.
>
> My intention is to create a language that is fast, readable and objective. A language that achieves the goal in the simplest way possible, without adding any fancy syntax.
> This is still work in progress, as i only work on it in my spare time, which does not allow me to put too much effort into it for now. I haven't decided on a name for the language yet. Thats how early it is, but i still believe its beautiful, even though its young. 

#### Features 
- [X] supports floats
- [X] supports strings 
- [X] supports booleans
- [X] supports integers 
- [ ] supports structs
