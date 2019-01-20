#ifndef JJ_MACROS_H
#define JJ_MACROS_H

#if defined(JJ_COMPILER_MSVC)
// thank you boost
#define JJ__PP_ENFORCE3(d,enforced) enforced
#define JJ__PP_ENFORCE2(a,b) JJ__PP_ENFORCE3(~, a ## b)
#define JJ__PP_ENFORCE1(a) JJ__PP_ENFORCE2(a,)

#define JJ__COUNT3(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25, actual, ...) actual
#define JJ__COUNT2(...) JJ__PP_ENFORCE1(JJ__COUNT3(__VA_ARGS__))
#else
#define JJ__COUNT2(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25, actual, ...) actual
#endif

#define JJ__COUNT1(...) JJ__COUNT2(__VA_ARGS__)

/*! Expands to the number of arguments given to the macro */
#define JJ_COUNT(...) \
    JJ__COUNT1(__VA_ARGS__, 25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
    
#if defined(JJ_COMPILER_MSVC)
#define JJ__VARG_N2(prefix, n, ...) JJ__PP_ENFORCE1(prefix ## n (__VA_ARGS__))
#define JJ__VARG_N12(prefix, n, p1, ...) JJ__PP_ENFORCE1(prefix ## n (p1, __VA_ARGS__))
#else
#define JJ__VARG_N2(prefix, n, ...) prefix ## n (__VA_ARGS__)
#define JJ__VARG_N12(prefix, n, p1, ...) prefix ## n (p1, __VA_ARGS__)
#endif
#define JJ__VARG_N1(prefix, n, ...) JJ__VARG_N2(prefix, n, __VA_ARGS__)
#define JJ__VARG_N11(prefix, n, p1, ...) JJ__VARG_N12(prefix, n, p1, __VA_ARGS__)

/*! Expands to prefixN(args) where prefix is given by first argument, N is the number of
remaining arguments and args are the remaining arguments. */
#define JJ_VARG_N(prefix, ...) JJ__VARG_N1(prefix, JJ_COUNT(__VA_ARGS__), __VA_ARGS__)
#define JJ_VARG_N1(prefix, p1, ...) JJ__VARG_N11(prefix, JJ_COUNT(__VA_ARGS__), p1, __VA_ARGS__)

#define jjM1(p1) p1
/*! Merges given arguments into one symbol. */
#define jjM2(p1,p2) p1##p2
/*! Merges given arguments into one symbol. */
#define jjM3(p1,p2,p3) p1##p2##p3
/*! Merges given arguments into one symbol. */
#define jjM4(p1,p2,p3,p4) p1##p2##p3##p4
/*! Merges given arguments into one symbol. */
#define jjM(...) JJ_VARG_N(__VA_ARGS__, jjM4, jjM3, jjM2, jjM1)

/*! Takes two arguments (unary operator and a operand) and converts these to a stream of the arguments followed by the value of the arguments. */
#define jjOO(op, o1) jjT(#op) jjT(#o1) jjT(" / ") jjT(#op) jjT("[") << o1 << jjT("]")
/*! Takes three arguments (first operand, operator and second operand) and converts these to a stream of the arguments followed by the values of the arguments. */
#define jjOOO(o1, op, o2) jjT(#o1) jjT(#op) jjT(#o2) jjT(" / [") << o1 << jjT("]") jjT(#op) jjT("[") << o2 << jjT("]")

#endif // JJ_MACROS_H
