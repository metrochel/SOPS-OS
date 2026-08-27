/*
 * iostream.hpp - Методы классов istream, ostream, iostream
 *
 * Реализует методы классов istream, ostream и iostream.
 *
 */

#include "../include/iostream.hpp"

/* === istream === */

void istream::put([[maybe_unused]] dword ch) {
    // TODO: бросать исключение
    // Так как istream - поток ввода, вывод в него запрещён.
}

void istream::flush() {
    // TODO: бросать исключение
    // Так как istream - поток ввода, вывод в него запрещён, а значит, "сливать" также нельзя.
}

#define right_shift_op_decl(type)                   \
    istream& istream::operator>>(type& number) {    \
        number = read_uint<type>();                 \
        return *this;                               \
    }

right_shift_op_decl(byte)
right_shift_op_decl(word)
right_shift_op_decl(dword)
right_shift_op_decl(qword)
right_shift_op_decl(size_t)

#undef right_shift_op_decl

istream& istream::operator>>(void *&ptr) {
    ptrint ptr_val = read_uint<ptrint>();
    ptr = (void*)ptr_val;
    return *this;
}

istream& istream::operator>>(char &ch) {
    ch = (char)read_char();
    return *this;
}

istream& istream::operator>>(string& str) {
    str = read_str();
    return *this;
}

istream& istream::operator>>(intmod mod) {
    set_int_modifier(mod);
    return *this;
}

istream& istream::operator=(const istream&& ref) {
    return *this;
}

/* === ostream === */

dword ostream::get() {
    // TODO: бросать исключение
    // Так как ostream - поток вывода, то чтение из него запрещено.
    return maxdword;
}

#define left_shift_op_decl(type)                \
    ostream& ostream::operator<<(type num) {    \
        write_uint(num);                        \
        return *this;                           \
    }

left_shift_op_decl(byte)
left_shift_op_decl(word)
left_shift_op_decl(dword)
left_shift_op_decl(qword)
left_shift_op_decl(size_t)

#undef left_shift_op_decl

ostream& ostream::operator<<(int num) {
    stream::write_int(num);
    return *this;
}

ostream& ostream::operator<<(intmod mod) {
    stream::set_int_modifier(mod);
    return *this;
}

ostream& ostream::operator<<(char ch) {
    write_char(ch);
    return *this;
}

ostream& ostream::operator<<(const char *str) {
    write_str(str);
    return *this;
}

ostream& ostream::operator<<(const string& str) {
    write_str(str);
    return *this;
}

ostream& ostream::operator<<(void *ptr) {
    intmod md = modifier.int_mod;
    set_int_modifier(intmod::hex);
    write_uint((ptrint)ptr);
    set_int_modifier(md);
    return *this;
}

ostream& ostream::operator=(const ostream&& ref) {
    return *this;
}

/* === iostream === */

iostream& iostream::operator=(const iostream&& ref) {
    return *this;
}