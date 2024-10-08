# ft_malloc

This is a custom implementation of the `malloc` function in C. It provides a way to dynamically allocate memory on the heap. It can be used as a drop in replacement to malloc.

## Features
- Implementation of malloc, free, realloc, calloc.
- Efficient memory allocation and deallocation.
- Support for different allocation sizes.
- Thread-safe implementation.

## Getting Started

To use this custom malloc implementation in your project, follow these steps:
```
git clone https://github.com/Doogie42/ft_malloc.git
cd ft_malloc
make
```

## Usage with LD_PRELOAD


```
LD_PRELOAD=./libft_malloc.so ./yourprogram
```
or
```
LD_PRELOAD=$(pwd)/libft_malloc.so ./yourprogram
```