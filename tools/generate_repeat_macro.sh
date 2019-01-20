#!/bin/bash

# 1 ... prefix
# 2 ... suffix
# 3 ... separator
# 4 ... from (inclusive)
# 5 ... to (inclusive)
function generate_inc()
{ local x="$4"
  local max="$5"
  local sep="$3"
  local pf="$1"
  local sf="$2"
  local fst=1
  while [[ "$x" -le "$max" ]]
  do
    [[ $fst -ne 1 ]] && echo -n "$sep"
    echo -n "$pf$x$sf"
    fst=0
    x=$((x+1))
  done
}

# 1 ... prefix
# 2 ... suffix
# 3 ... separator
# 4 ... from (inclusive)
# 5 ... to (inclusive)
function generate_dec()
{ local x="$4"
  local min="$5"
  local sep="$3"
  local pf="$1"
  local sf="$2"
  local fst=1
  while [[ "$x" -ge "$min" ]]
  do
    [[ $fst -ne 1 ]] && echo -n "$sep"
    echo -n "$pf$x$sf"
    fst=0
    x=$((x-1))
  done
}

function generate_varg()
{ cnt="$1"
  varg="$2"
  [[ $cnt -lt 1 || $varg -lt 1 ]] && { echo "Invalid counts '$cnt' '$varg'" ; exit 1 ; }
  local countpars="$(generate_inc p '' , 1 $cnt)"
  local count1pars="$(generate_dec '' '' , $cnt 0)"
  cat << END_OF_BLOCK
#if defined(JJ_COMPILER_MSVC)
// thank you boost
#define JJ__PP_ENFORCE3(d,enforced) enforced
#define JJ__PP_ENFORCE2(a,b) JJ__PP_ENFORCE3(~, a ## b)
#define JJ__PP_ENFORCE1(a) JJ__PP_ENFORCE2(a,)

#define JJ__COUNT3($countpars, actual, ...) actual
#define JJ__COUNT2(...) JJ__PP_ENFORCE1(JJ__COUNT3(__VA_ARGS__))
#else
#define JJ__COUNT2($countpars, actual, ...) actual
#endif

#define JJ__COUNT1(...) JJ__COUNT2(__VA_ARGS__)

/*! Expands to the number of arguments given to the macro */
#define JJ_COUNT(...) \\
    JJ__COUNT1(__VA_ARGS__, $count1pars)

END_OF_BLOCK
  echo '#if defined(JJ_COMPILER_MSVC)'
  echo '#define JJ__VARG_N2(prefix, n, ...) JJ__PP_ENFORCE1(prefix ## n (__VA_ARGS__))'
  i=1
  while [[ $i -le $varg ]]
  do
    local argpars="$(generate_inc p '' ', ' 1 $varg)"
    echo "#define JJ__VARG_N${i}2(prefix, n, $argpars, ...) JJ__PP_ENFORCE1(prefix ## n ($argpars, __VA_ARGS__))"
    i=$((i+1))
  done
  echo '#else // defined(JJ_COMPILER_MSVC)'
  echo '#define JJ__VARG_N2(prefix, n, ...) prefix ## n (__VA_ARGS__)'
  i=1
  while [[ $i -le $varg ]]
  do
    local argpars="$(generate_inc p '' ', ' 1 $varg)"
    echo "#define JJ__VARG_N12(prefix, n, $argpars, ...) prefix ## n ($argpars, __VA_ARGS__)"
    i=$((i+1))
  done
  echo '#endif // defined(JJ_COMPILER_MSVC)'
  echo '#define JJ__VARG_N1(prefix, n, ...) JJ__VARG_N2(prefix, n, __VA_ARGS__)'
  i=1
  while [[ $i -le $varg ]]
  do
    local argpars="$(generate_inc p '' ', ' 1 $varg)"
    echo "#define JJ__VARG_N11(prefix, n, $argpars, ...) JJ__VARG_N12(prefix, n, $argpars, __VA_ARGS__)"
    i=$((i+1))
  done
  echo

  cat << 'END_OF_BLOCK'
/*! Expands to prefixN(args) where prefix is given by first argument, N is the number of
remaining arguments and args are the remaining arguments. */
#define JJ_VARG_N(prefix, ...) JJ__VARG_N1(prefix, JJ_COUNT(__VA_ARGS__), __VA_ARGS__)
END_OF_BLOCK
  i=1
  while [[ $i -le $varg ]]
  do
    local argpars="$(generate_inc p '' ', ' 1 $varg)"
    echo "/*! Same as JJ_VARG_N except that it passes $argpars to the final macro along with the variadic parameters. */"
    echo "#define JJ_VARG_N1(prefix, $argpars, ...) JJ__VARG_N11(prefix, JJ_COUNT(__VA_ARGS__), $argpars, __VA_ARGS__)"
    i=$((i+1))
  done
}

function generate_merge()
{ cnt="$1"
  [[ $cnt -lt 1 ]] && { echo "Invalid count '$cnt'" ; exit 1 ; }
  i=1
  while [[ $i -le $cnt ]]
  do
    echo '/*! Merges given arguments into one symbol. */'
    echo -n "#define jjM$i(p1"
    j=2
    while [[ $j -le $i ]]
    do
      echo -n ",p$j"
      j=$((j+1))
    done
    echo -n ') p1'
    j=2
    while [[ $j -le $i ]]
    do
      echo -n "##p$j"
      j=$((j+1))
    done
    echo
    i=$((i+1))
  done
  i=$((cnt-1))
  echo '/*! Merges given arguments into one symbol. */'
  echo '#define jjM(...) JJ_VARG_N(jjM, __VA_ARGS__)'
}

function generate_test()
{ clsv="$1"
  [[ $clsv -lt 1 ]] && { echo "Invalid count '$clsv'" ; exit 1 ; }
  casv="$2"
  [[ $casv -lt 1 ]] && { echo "Invalid count '$casv'" ; exit 1 ; }
  casincls="$3"
  [[ $casincls -lt 1 ]] && { echo "Invalid count '$casincls'" ; exit 1 ; }
  i=1
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CLASS_CALL(name, no, args) \
    static void jjM2(JJ_TEST_CLASS_VARIANT_,no)(jj::test::statistics_t& stats, const jj::test::AUX::filter_refs_t& filters){ name inst args; JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().run(inst, stats, filters); } \
    static int jjM2(JJ_TEST_CLASS_REGISTRAR_,no)() { jj::test::db_t::instance().register_testclass(&name::jjM2(JJ_TEST_CLASS_VARIANT_,no), jjT(#name), jjT(#args)); return no; }
ENDOFBLOCK
  s=$((clsv-1))
  i=1
  while [[ $i -lt $clsv ]]
  do
    echo -n "#define JJ___TEST_CLASS_CALLS${i}(name"
    j=1
    while [[ $j -le $i ]]
    do
      echo -n ", p$j"
      j=$((j+1))
    done
    echo -n ')'
    j=1
    while [[ $j -le $i ]]
    do
      echo -n " JJ___TEST_CLASS_CALL(name, $j, p$j)"
      j=$((j+1))
    done
    echo
    i=$((i+1))
  done
  echo "#define JJ___TEST_CLASS_CALLS(name, ...) JJ_VARG_N1(JJ___TEST_CLASS_CALLS, name, __VA_ARGS__)"
  echo
  echo '#define JJ___TEST_CLASS_REG(no) jjM2(JJ_TEST_CLASS_REGISTRAR_,no)();'
  i=1
  while [[ $i -lt $clsv ]]
  do
    echo -n "#define JJ___TEST_CLASS_REGS${i}(dummy, p1"
    j=2
    while [[ $j -le $i ]]
    do
      echo -n ", p$j"
      j=$((j+1))
    done
    echo -n ')'
    j=1
    while [[ $j -le $i ]]
    do
      echo -n " JJ___TEST_CLASS_REG($j)"
      j=$((j+1))
    done
    i=$((i+1))
    echo
  done
  echo "#define JJ___TEST_CLASS_REGS(dummy, ...) JJ_VARG_N1(JJ___TEST_CLASS_REGS, dummy, __VA_ARGS__)"
  echo '// note: the dummy parameter above is not required except for MSVC which cannot have empty macro argument list ie, MACRO()'
  echo
  echo '#define JJ___TEST_CASE_DEF(classname, testname) classname::jjM2(registrar_,testname)();'
  i=1
  s=$((casv-1))
  while [[ $i -lt $casv ]]
  do
    echo -n "#define JJ___TEST_CASE_DEFS${i}(classname"
    j=1
    while [[ $j -le $i ]]
    do
      echo -n ", p$j"
      j=$((j+1))
    done
    echo -n ')'
    j=1
    while [[ $j -le $i ]]
    do
      echo -n " JJ___TEST_CASE_DEF(classname, p$j)"
      j=$((j+1))
    done
    echo
    i=$((i+1))
  done
  i=$((casv-1))
  echo "#define JJ___TEST_CASE_DEFS(classname, ...) JJ_VARG_N1(JJ___TEST_CASE_DEFS, classname, __VA_ARGS__)"
  echo
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CASE_CALL(name, no, args) \
    static void jjM3(runner,no,name)(JJ_THIS_TESTCLASS& inst, jj::test::statistics_t& stats) { inst.JJ_TEST_CASE_Statistics.reset(); (inst.*&JJ_THIS_TESTCLASS::name) args; stats = inst.JJ_TEST_CASE_Statistics; }
ENDOFBLOCK
  i=1
  while [[ $i -lt $casv ]]
  do
    echo -n "#define JJ___TEST_CASE_CALLS${i}(name"
    j=1
    while [[ $j -le $i ]]
    do
      echo -n ", p$j"
      j=$((j+1))
    done
    echo -n ')'
    j=1
    while [[ $j -le $i ]]
    do
      echo -n " JJ___TEST_CASE_CALL(name, $j, p$j)"
      j=$((j+1))
    done
    echo
    i=$((i+1))
  done
  j=$((casv-1))
  echo "#define JJ___TEST_CASE_CALLS(name, ...) JJ_VARG_N1(JJ___TEST_CASE_CALLS, name, __VA_ARGS__)"
  echo
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CASE_REG(name, no, args) \
    JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().register_testcase(&JJ_THIS_TESTCLASS::jjM3(runner,no,name), jjT(#name), jjT(#args));
ENDOFBLOCK
  i=1
  s=$(($casincls-1))
  while [[ $i -lt $casincls ]]
  do
    echo -n "#define JJ___TEST_CASE_REGS${i}(name"
    j=1
    while [[ $j -le $i ]]
    do
      echo -n ", p$j"
      j=$((j+1))
    done
    echo -n ')'
    j=1
    while [[ $j -le $i ]]
    do
      echo -n " JJ___TEST_CASE_REG(name, $j, p$j)"
      j=$((j+1))
    done
    echo
    i=$((i+1))
  done
  i=$((casincls-1))
  echo "#define JJ___TEST_CASE_REGS(name, ...) JJ_VARG_N1(JJ___TEST_CASE_REGS, name, __VA_ARGS__)"
}

# main program starts here
what="$1"
shift
case "$what" in
  -h|--help|help)
     cat << 'END_OF_BLOCK'
Use this to generate various macro variants. Usage:
generate_repeat_macro.sh type count...
where type can be one of the following and number of count parameters differs per-type.
  - varg nocount novarg ... JJ_COUNT and JJ_VARG_N stuff
  - merge count ... generates the jjM macro
  - test noclsv nocasv nocasincls ... generates all the test framework macros, where:
      noclsv is the maximum number of class variants for a class
      nocasv is the maximum number of case variants for a case
      nocasincls is the maximum number of cases in a class (variants do not matter)
END_OF_BLOCK
    exit
    ;;
  varg|merge|test)
    generate_"$what" "$@"
    ;;
  *)
    echo "Don't know how to generate '$what'"
    exit 1
esac
