#ifndef __BUILTIN_H
#define __BUILTIN_H

#include "tueslisp.h"

#define DECL(ID) Any *ID(Any*, Any*, Any*);

DECL(builtin_nil_p);     
DECL(builtin_symbol_p);  
DECL(builtin_pair_p);    
DECL(builtin_number_p);  
DECL(builtin_quote);     
DECL(builtin_quasiquote);
DECL(builtin_cons);      
DECL(builtin_car);       
DECL(builtin_cdr);       
DECL(builtin_cat);       
DECL(builtin_defsym);    
DECL(builtin_set);       
DECL(builtin_lambda);    
DECL(builtin_macro);     
DECL(builtin_plus);      
DECL(builtin_minus);     
DECL(builtin_multiply);  
DECL(builtin_divide);    
DECL(builtin_equal);     
DECL(builtin_less_than);     
DECL(builtin_greater_than);     
DECL(builtin_whole_part);
DECL(builtin_print);     
DECL(builtin_import);
DECL(builtin_extract_function); 
DECL(builtin_exit); 

#undef DECL

#endif
