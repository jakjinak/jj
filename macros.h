#ifndef JJ_MACROS_H
#define JJ_MACROS_H

/*! Takes two arguments (unary operator and a operand) and converts these to a stream of the arguments followed by the value of the arguments. */
#define jjOO(op, o1) jjT(#op) jjT(#o1) jjT(" / ") jjT(#op) jjT("[") << o1 << jjT("]")
/*! Takes three arguments (first operand, operator and second operand) and converts these to a stream of the arguments followed by the values of the arguments. */
#define jjOOO(o1, op, o2) jjT(#o1) jjT(#op) jjT(#o2) jjT(" / [") << o1 << jjT("]") jjT(#op) jjT("[") << o2 << jjT("]")

#endif // JJ_MACROS_H
