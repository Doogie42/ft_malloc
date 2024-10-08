#ifndef FT_PRINTF_H
#define FT_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define CONV_IS_SPECIAL_SPACE 1

void ft_itoa(int n, char buffer[64]);
void u_ft_itoa(uint64_t n, char buffer[64]);

int ft_strlen(const char *s);

int ft_printf(const char *s, ...);
int print_arg(const char *s, int *index, va_list *current_arg);

int print_non_special_char(const char *s, int *index);

int print_hexa(va_list *current_arg, int maj);
void int_to_hex(uint64_t n, char s[], int len_number);
int get_len_hexa(uint64_t n);
int print_custom_nbr_zero(char s[], int len_number, int n_zero);

int print_single_char(va_list *current_arg);
int print_string(va_list *current_arg);
int print_decimal(va_list *current_arg);
int print_hexa(va_list *current_arg, int maj);
int print_pointer(va_list *va_arg);
int print_unsigned(va_list *current_arg);
int print_char(char c);

#endif
