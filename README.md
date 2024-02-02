# t_string
type string implementation with string.h compatibility

The ambition of this project is to give an idea on how to possibly implement some extra functionality that could interfere with standard C libraries.
We all know that C is considered an unsafe language, and one of the main reason is strings are treated as null terminating sequence of characters. This could lead to some nasty behaviour, like buffer overflow or segfault.

## **index:**
  - [Declare your struct](#declare-your-struct)
  - [Initialize the string](#initialize-the-string)
  - [Polymorphism](#polymorphism)
  - [Considerations](#considerations)
  - [Special Mention](#special-mention)
  - [Conclusion](#conclusion)

### Declare your struct
First thing first, we need to declare our string struct. It's just a struct with a size, a capacity, and the actual char array. It's important to notice that I've declared the array as a char[] without any size. This is called 'Flexible array member'. For more info about this, you can check the wikipedia page: https://en.wikipedia.org/wiki/Flexible_array_member
Basically the idea is that you declare an empty array as the last element of the struct. When you allocating memory, any extra space is accessible using the char[].
I've choosed this option over the plain char * because this permit to allocate memory for the string in one go without the need to double malloc the struct.
It's faster and cleaner.
I've also adding 2 char pointers. These should be use as iterators, if we wanna iterate the array using pointers instead of index. Note that there's no actually any benefit to use pointers instead of index, but I'm planning to add range loops as well, and using pointers is much more safer.
```
typedef char * iterator;
typedef struct s_string {
	size_t size;
	size_t capacity;
	iterator begin;
	iterator end;
	char buffer[]; // flexible array member
}	t_string;
typedef t_string * string;
```

### Initialize the string
```
string string_init(char *str) {
	size_t len = strlen(str);
	size_t to_alloc = len < SMALL_CHUNK ? SMALL_CHUNK : len < MEDIUM_CHUNK ? MEDIUM_CHUNK : len < BIG_CHUNK ? BIG_CHUNK : len + SMALL_CHUNK;
	string s = (string)malloc(sizeof(t_string) + to_alloc + 1);
	if (!s) {
		return &(t_string){.error=HEAP_ALLOC_ERROR};
	}
	s->capacity = to_alloc;
	s->size = len;
	s->begin = &s->buffer[0];
	s->end = &s->buffer[s->size];
	for (size_t i = 0; i < len; i++) {
		s->buffer[i] = str[i];
	}
	s->buffer[len] = 0;
	return s;
}
```
Nothing to fency here. I decided to allocate fixed amount of memory for our array, so it will be faster to perform some operation like concatenation ecc.
Note that like c++ strings, the end pointer point to the element after the last.

### Polymorphism
So, after many attempts spent trying to overload standard libraries functions, it appeared the  easiest way was the right way.
Using C11 feature _Generic, we can define a macro that choose the correct function based on the parameters passed.
What was even cooler, _Generic works at comptime, while usually macro works inside the preprocessor.
This is very important, because it allows to #define an existing function as a _Generic macro.
Let's make an example to clear this statement:
Let's overload the simplest string.h function, strlen, using _Generic macro:
```
#define strlen(a) _Generic((a), string  : length_ptr, default : strlen) (a)
```
this is the equivalent of:
calling a preprocessor macro that substitute strlen with
```
_Generic((a), string  : length, default : strlen) (a)
```
Since we passed the preprocessor phase, now during comptime _Generic will check '(a)', the argument passed to the function. If the argument is a string, it will call our function; if not, it will call the standard strlen.
Our length function will look like:
```
size_t length(string s) { return s->size; }
```
This overload is a great example on how this could benefit a program. Image you need to calculate the length of a very long string. The computation will take O(n). Using string it will be O(1), no matter the size of the string. Also, we can assume that the function will work in any circumstances, while for a char * someone could forget to put the terminating null char.

### Considerations
Checking the library you can easly understand how this could easly work and seriously benefit the code in many situations. It even has big backward compatibility. I'm seriously planning on increasing the amount of functions to overload and increasing the stability and safety of the library.
Using some C23 functionanlity I was also able to overload read function passing 2 arguments instead of 3, utilizing __VA_OPT__ and switching function and number of parameters based on __VA_ARGS__.
This gave me so many ideas to extend C functionality and build something a little more modern.

### Special Mention
I don't know how many are aware of this, but whiler implementing this struct I discovered you can add specs to printf, and passing the function explaining how to print your custom spec.
```
register_printf_specifier('T', printf_output_T, printf_arginfo_T);
```
This is the function to call to add your specifier. In this case 'T' is the specifier to print my string, printf_output_T the function that explain how to print it, and printf_arginfo_T the function that explain how many arguments takes the specifier (1 in our case, a pointer to t_string). What is cool is that I can check before printing if the string will segfault and crash the program, reading the size, and I can skip the print in case it's 0.
The really big backfire is that compiler will emit a warning. This can't basically be avoided.
But I'm going to try to build a library to print using new C functionality.

### Conclusion
That's all for the moment. I appreciate the time you spent reading this. I hope it could be helpful to someone, and any suggestion is highly appreciated. I will keep updating this library until it could be used for real projects.
I'm planning on building a library that simulate range loops and iterator dynamics, to allow easy and clean operation on arrays. Unfortunately, I haven't found any solution to simulate lambdas, so I need to find another way to pass the expression to apply to each element of a range.
