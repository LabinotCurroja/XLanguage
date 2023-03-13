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

## Features 
- [X] supports 32-bit floats
- [X] supports strings 
- [X] supports booleans
- [X] supports integers 
- [X] supports strings 
- [X] arithmetic expressions
- [X] arrays 
- [ ] structs


## Simple program example
```C#
float a = 2.4; 
float b = 1.9;
float c = a+b;

string d = "Hello World!";
print(d);
```
### Produces this abstract syntax tree: 
```
   ASTNode   code_block   -> |   0x0  
      ASTNode   assign_float   -> |   0x0  
          ASTNode   symbol   -> |   0x0  
          ASTNode   float_literal   -> |   0x4019999a  
   ASTNode   code_block   -> |   0x0  
      ASTNode   assign_float   -> |   0x1  
          ASTNode   symbol   -> |   0x1  
          ASTNode   float_literal   -> |   0x3ff33333  
   ASTNode   code_block   -> |   0x0  
      ASTNode   assign_float   -> |   0x2  
          ASTNode   symbol   -> |   0x2  
          ASTNode   bin_add   -> |   0x0  
              ASTNode   identifier   -> |   0x0  
              ASTNode   identifier   -> |   0x1  
   ASTNode   code_block   -> |   0x0  
      ASTNode   assign_str   -> |   0x3  
          ASTNode   identifier   -> |   0x3  
          ASTNode   str_literal   -> |   0x4  
   ASTNode   code_block   -> |   0x0  
      ASTNode   print   -> |   0x0  
          ASTNode   identifier   -> |   0x3  
```

This project is not production ready in any way, but rather a long term commitment to create a language that is perfect. At least in my eyes.
