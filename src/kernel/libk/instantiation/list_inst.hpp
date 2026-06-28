/*
 * list_inst.hpp - Инстанцирование шаблона класса list
 *
 * Содержит перечень явных инстанцирований для шаблона list<T>.
 */

#ifndef _LIST_INST_INCL
#define _LIST_INST_INCL

#include "../include/list.hpp"
#include "io/com/com.hpp"

template class list<com::com_port>;

#endif //_LIST_INST_INCL
