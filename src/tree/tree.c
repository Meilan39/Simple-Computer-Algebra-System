#include "tree.h"

void n_simplify(Node* this) {
    n_print(this, "./src/meta/pt.txt"); // print parse tree
    n_compress(this);
    n_print(this, "./src/meta/ast.txt"); // print abstract syntax tree
}

void n_compress(Node* this) {
    n_compress_suffix(this);
    n_compress_chain(this);
    for(int i = 0; i < this->length; i++)
        n_compress(this->next[i]);  
    // n_refactor(this); // refactoring is depth first
}

// void n_refactor(Node* this) {
//     /* refactor multiplication */
//     if(this->type == nt_multiplicative_expression) {
//         Node* next;
//         int divide = 0, idx = 1;
//         /* start from 1 to avoid multiplicative sign */
//         for(int i = 1; i < this->length; i++) {
//             next = this->next[i];
//             if(next->type == lt_dot || next->type == lt_slash) {
//                 divide = (next->type == lt_slash); 
//                 n_free(next);
//                 continue;
//             }
//             next->subtype = divide ? 2 : 1;
//             this->next[idx++] = this->next[i];
//             if((idx-1) != i) this->next[i] = NULL;
//         }
//         this->length = idx;
//         this->next = realloc(this->next, sizeof(Node*) * this->length);
//     }    
//     /* refactor addition */
//     if(this->type == nt_additive_expression) {
//         Node* next;
//         int minus = 0, idx = 0;
//         for(int i = 0; i < this->length; i++) {
//             next = this->next[i];
//             if(next->type == lt_plus || next->type == lt_minus) {
//                 minus = (next->type == lt_minus); 
//                 n_free(next);
//                 continue;
//             }
//             if(next->next[0]->type == lt_minus) minus = !minus;
//             next->next[0]->type = minus ? lt_minus : lt_plus;
//             this->next[idx++] = this->next[i];
//             if((idx-1) != i) this->next[i] = NULL;
//         }
//         this->length = idx;
//         this->next = realloc(this->next, sizeof(Node*) * this->length);
//     }
// }

Node* n_free(Node* head) {
    if(!head) return NULL;
    if(head->length) {
        for(int i = 0; i < head->length; i++)
            if(head->next[i]) n_free(head->next[i]);
        if(head->next) free(head->next);
    }
    free(head);
    return NULL;
}

Node* n_reset(Node* head) {
    if(!head) return NULL;
    if(head->length) {
        for(int i = 0; i < head->length; i++)
            n_free(head->next[i]);
        if(head->next) free(head->next);
    }
    head->next = NULL;
    head->length = 0;
    head->subtype = 0;
    head->value = 0;
    return head; 
}

Node* n_construct(int type, double value) {
    Node* this = malloc(sizeof(Node));
    if(!this) return NULL;
    this->next = NULL;
    this->length = 0;
    this->type = type;
    this->subtype = 0;
    this->value = value;
    return this;
}

Node* n_pop(Node* this, int index) {return NULL;}

Node* n_pick(Node* this, int index) {return NULL;}

Node* n_copy(Node* this) {return NULL;}

Node** n_findd(Node* this, int type) {
    Node** ref = NULL;
    while(this->type != type){
        if(this->length > 2) return NULL;
        if(this->length == 2) {
            if(ct_terminator >= this->next[0]->type || this->next[0]->type >= lt_terminator)
                return NULL;
            this = this->next[1];
            ref = &(this->next[1]);           
        } else {
            this = this->next[0];
            ref = &(this->next[0]);            
        }
    } return this->length == 1 ? NULL : ref;
}

Node** n_findb(Node* this, int type) {
    /* todo */
    // while(this->type != type){
    //     if(this->length != 1) return NULL;
    //     this = this->next[0];
    // } return this;
}

int n_push(Node* this, Node* node) {
    if(!node) return 0;
    if((this->length)++) this->next = realloc(this->next, this->length * sizeof(Node*));
    else this->next = malloc(this->length * sizeof(Node*));
    this->next[this->length - 1] = node;   
    return 1;
}

int n_pushfront(Node* this, Node* node) {
    if(!node) return 0;
    if((this->length)++) {
        Node** temp = malloc(this->length * sizeof(Node*));
        for(int i = 0; i < this->length - 1; i++)
            temp[i+1] = this->next[i];
        free(this->next);
        this->next = temp;
    } else this->next = malloc(this->length * sizeof(Node*));
    this->next[0] = node;
    return 1;
}

int n_delete(Node* this, int n, ...) {
    if(!(n > 0)) return 0;
    va_list args;
    va_start(args, n);
    int idx = 0, next = va_arg(args, int);
    for(int i = 0; i < this->length; i++) {
        if(next == i) {
            n_free(this->next[i]);
            next = (--n) ? va_arg(args, int) : next;
            continue;
        }
        this->next[idx++] = this->next[i];
    }
    this->length = idx + 1;
    va_end(args);
    return 1;
}

int n_emplace(Node* this, int length) {
    if(this->next) {
        if(this->length > length) goto E;
        this->next = realloc(this->next, length * sizeof(Node*));
        for(int i = this->length; i < length; i++)
            this->next[i] = NULL;
        this->length = length;
    } else {
        this->length = length;
        this->next = calloc(sizeof(Node*), this->length);
    }
    return 0;
E:  printf("...emplace failed\n");
    return -1;
}

int n_replace(Node** this, Node* new) {
    return 0;
}

void n_print(Node* this, const char* path) {
    s_abstract_syntax_tree = fopen(path, "w");
    if(!s_abstract_syntax_tree) {printf("unable to open %s\n", path); return;}
    int state[100] = {0};
    n_helper(this, 0, 0, state);
    fclose(s_abstract_syntax_tree);
} 

void n_helper(Node* this, int depth, int edge, int state[]) {
    if(depth > 0) {
        for(int i = 1; i < depth; i++)
            fprintf(s_abstract_syntax_tree, state[i] ? "│   " : "    ");
        fprintf(s_abstract_syntax_tree, "%s", (edge ? "└── " : "├── "));
    }
    /* print to file */
    fprintf(s_abstract_syntax_tree, "%s", n_typtostr(this->type));
    if(c_types(this->type)){
        fprintf(s_abstract_syntax_tree, " {%lf}", this->value);
    }
    if(this->type == nt_multiplicative_expression) {
        if(this->subtype == 1) fprintf(s_abstract_syntax_tree, " |%s|", "positive");
        if(this->subtype == 2) fprintf(s_abstract_syntax_tree, " |%s|", "negative");
    }
    if(this->type == nt_exponential_expression) {
        if(this->subtype == 1) fprintf(s_abstract_syntax_tree, " |%s|", "product");
        if(this->subtype == 2) fprintf(s_abstract_syntax_tree, " |%s|", "quotient");
    }
    fprintf(s_abstract_syntax_tree, "\n");
    /* edge processing */
    if(this->length > 0) {
        state[depth] = !edge;
        for (int i = 0; i < this->length; i++) {
            n_helper(this->next[i], depth + 1, i == (this->length - 1), state);
        }
        state[depth] = 0;
    }
}

void n_compress_chain(Node* this) {
    if(this->length==0) return;
    for(int i = 0; i < this->length; i++) {
        while(1) {
            Node* next = this->next[i];
            if(n_chain_exception(next->type)) break;
            if(next->length!=1) break;
            this->next[i] = next->next[0];
            free(next->next);
            free(next);                 
        }
    }
}

void n_compress_suffix(Node* this) {
    if(this->length==0) return;
    if( ! n_suffix_exception(this->type)) return;
    for(int i = 0; i < this->length; i++) {
        Node* next = this->next[i];
        if(this->type + 1 != next->type) continue;
        this->length += next->length - 1;
        Node** temp = malloc(this->length * sizeof(Node*));
        int j = 0, k = 0, l = 0;
        for(; j < this->length; j++) {
            if(j < i || j >= i + next->length) {
                temp[j] = this->next[k++];
                continue;
            } else temp[j] = next->next[l++];
            if(j==i) k++;
        }
        free(this->next);
        this->next = temp;
        free(next->next);
        free(next);
    }
}

const char* n_typtostr(int type) {
    if(type < 1 || nt_terminator <= type) return n_typtostr_map[0];
    return n_typtostr_map[type] ? n_typtostr_map[type] : n_typtostr_map[0];
}
int n_suffix_exception(int type) {
    if(type < 1 || nt_terminator <= type) return 0;
    return n_suffix_exception_map[type];
}
int n_chain_exception(int type) {
    if(type < 1 || nt_terminator <= type) return 0;
    return n_chain_exception_map[type];
}

const char* const n_typtostr_map[nt_terminator] = {
    [0] = "",
    /* constants */
    [ct_number] = "number",
    [ct_decimal] = "decimal",
    [ct_zero] = "zero",
    /* lex */
    [lt_plus] = "plus",
    [lt_minus] = "minus",
    [lt_dot] = "dot", 
    [lt_slash] = "slash",
    [lt_caret] = "caret",
    [lt_equal] = "equal",
    [lt_comma] = "comma",
    [lt_scientific] = "scientific",
    [lt_h_parenthesis] = "head parenthesis",
    [lt_t_parenthesis] = "tail parenthesis",
    [lt_h_bracket] = "head bracket",
    [lt_t_bracket] = "tail bracket",
    [lt_e] = "e",
    [lt_pi] = "pi",
    [lt_x] = "x",
    [lt_zeros] = "zeros",
    [lt_root] = "root",   
    [lt_sqrt] = "square root",  
    [lt_log] = "logarithm",   
    [lt_ln] = "natural logarithm",  
    [lt_sin] = "sin",  
    [lt_cos] = "cos",   
    [lt_tan] = "tan",  
    [lt_asin] = "arcsin",  
    [lt_acos] = "arccos",   
    [lt_atan] = "arctan",  
    [lt_sinh] = "hyperbolic sin",  
    [lt_cosh] = "hyperbolic cos",   
    [lt_tanh] = "hyperbolic tan",  
    /* syn */
    [nt_command] = "command",
    [nt_nonvariable] = "non variable",
    [nt_zeros] = "zeros",
    [nt_polynomial] = "polynomial",
    [nt_polynomial_suffix] = "polynomial suffix",
    [nt_polynomial_term] = "polynomial term",
    [nt_expression] = "expression",    
    [nt_additive_expression] = "additive expression",
    [nt_additive_expression_suffix] = "additive expression suffix",
    [nt_multiplicative_expression] = "multiplicative expression",
    [nt_multiplicative_expression_suffix] = "multiplicative expression suffix",
    [nt_exponential_expression] = "exponential expression",
    [nt_exponential_expression_suffix] = "exponential expression suffix",
    [nt_primary_expression] = "primary expression",
    [nt_parenthesis] = "parenthesis",
    [nt_functions] = "functions",   
    [nt_root] = "root",   
    [nt_sqrt] = "square root",  
    [nt_log] = "logarithm",   
    [nt_ln] = "natural logarithm",  
    [nt_sin] = "sine",  
    [nt_cos] = "cosine",   
    [nt_tan] = "tangent",  
    [nt_asin] = "arcsin",  
    [nt_acos] = "arccos",   
    [nt_atan] = "arctan",  
    [nt_sinh] = "hyperbolic sin",  
    [nt_cosh] = "hyperbolic cos",   
    [nt_tanh] = "hyperbolic tan",  
    [nt_real_number] = "real number",
    [nt_irrational] = "irrational number",
    [nt_rational] = "rational number",
    [nt_scientific] = "scientific number",    
    [nt_sign] = "sign",
    [nt_variable] = "variable",
    [nt_x] = "x",
};

const char n_chain_exception_map[] = {
    [nt_additive_expression] = 1,
    [nt_multiplicative_expression] = 1,
    [nt_exponential_expression] = 1,
    [nt_parenthesis] = 1,
    [nt_polynomial_term] = 1,
    [nt_nonvariable] = 1,    
    [nt_zeros] = 1,
    [nt_irrational] = 1,
    [nt_rational] = 1,
    [nt_variable] = 1,
};

const char n_suffix_exception_map[] = {
    [nt_additive_expression] = 1,
    [nt_multiplicative_expression] = 1,
    [nt_exponential_expression] = 1,
    [nt_polynomial] = 1,
};
