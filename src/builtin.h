#ifndef __BUILTIN_H
#define __BUILTIN_H

#include "tueslisp.h"

Any *builtin_nil_p(Any*, Any*, Any*);     
Any *builtin_symbol_p(Any*, Any*, Any*);  
Any *builtin_pair_p(Any*, Any*, Any*);    
Any *builtin_number_p(Any*, Any*, Any*);  
Any *builtin_quote(Any*, Any*, Any*);     
Any *builtin_quasiquote(Any*, Any*, Any*);
Any *builtin_cons(Any*, Any*, Any*);      
Any *builtin_car(Any*, Any*, Any*);       
Any *builtin_cdr(Any*, Any*, Any*);       
Any *builtin_cat(Any*, Any*, Any*);       
Any *builtin_defsym(Any*, Any*, Any*);    
Any *builtin_set(Any*, Any*, Any*);       
Any *builtin_lambda(Any*, Any*, Any*);    
Any *builtin_macro(Any*, Any*, Any*);     
Any *builtin_plus(Any*, Any*, Any*);      
Any *builtin_minus(Any*, Any*, Any*);     
Any *builtin_multiply(Any*, Any*, Any*);  
Any *builtin_divide(Any*, Any*, Any*);    
Any *builtin_equal(Any*, Any*, Any*);     
Any *builtin_whole_part(Any*, Any*, Any*);
Any *builtin_print(Any*, Any*, Any*);     
Any *builtin_open_file(Any*, Any*, Any*); 
Any *builtin_eval_file(Any*, Any*, Any*); 

#endif
