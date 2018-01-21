#!/bin/bash

function generate_selector()
{ cnt="$1"
  [[ $cnt -lt 1 ]] && { echo "Invalid count '$cnt'" ; exit 1 ; }
  i=1
  while [[ $i -lt $cnt ]]
  do
    echo -n "#define JJ_PP_SELECTOR$i("
    j=1
    while [[ $j -le $i ]]
    do
      echo -n "p$j, "
      j=$((j+1))
    done
    echo 'actual, ...) actual'
    i=$((i+1))
  done
}

function generate_merge()
{ cnt="$1"
  [[ $cnt -lt 1 ]] && { echo "Invalid count '$cnt'" ; exit 1 ; }
  i=1
  while [[ $i -lt $cnt ]]
  do
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
  echo -n "#define jjM(...) JJ_PP_SELECTOR$i(__VA_ARGS__"
  while [[ $i -gt 0 ]]
  do
    echo -n ", jjM$i"
    i=$((i-1))
  done
  echo ')'
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
    static int jjM2(JJ_TEST_CLASS_REGISTRAR_,no)() { jj::test::db_t::instance().register_testclass(&name::jjM2(JJ_TEST_CLASS_VARIANT_,no), #name, #args); return no; }
ENDOFBLOCK
  s=$((clsv-1))
  i=1
  while [[ $i -lt $clsv ]]
  do
    echo -n "#define JJ___TEST_CLASS_${i}CALLS(name"
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
  echo -n "#define JJ___TEST_CLASS_CALLS(name, ...) JJ_PP_SELECTOR$s(__VA_ARGS__"
  j=$((clsv-1))
  while [[ $j -gt 0 ]]
  do
    echo -n ", JJ___TEST_CLASS_${j}CALLS"
    j=$((j-1))
  done
  echo ')(name, __VA_ARGS__)'
  echo
  echo '#define JJ___TEST_CLASS_REG(no) jjM2(JJ_TEST_CLASS_REGISTRAR_,no)();'
  i=1
  while [[ $i -lt $clsv ]]
  do
    echo -n "#define JJ___TEST_CLASS_${i}REGS(p1"
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
  echo -n "#define JJ___TEST_CLASS_REGS(...) JJ_PP_SELECTOR$s(__VA_ARGS__"
  i=$((clsv-1))
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CLASS_${i}REGS"
    i=$((i-1))
  done
  echo ")(__VA_ARGS__)"
  echo
  echo '#define JJ___TEST_CASE_DEF(classname, testname) classname::jjM2(registrar_,testname)();'
  i=1
  s=$((casv-1))
  while [[ $i -lt $casv ]]
  do
    echo -n "#define JJ___TEST_CASE_${i}DEFS(classname"
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
  echo -n "#define JJ___TEST_CASE_DEFS(classname, ...) JJ_PP_SELECTOR$s(__VA_ARGS__"
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CASE_${i}DEFS"
    i=$((i-1))
  done
  echo ")(classname, __VA_ARGS__)"
  echo
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CASE_CALL(name, no, args) \
    static void jjM3(runner,no,name)(JJ_THIS_TESTCLASS& inst, jj::test::statistics_t& stats) { inst.JJ_TEST_CASE_Statistics.reset(); (inst.*&JJ_THIS_TESTCLASS::name) args; stats = inst.JJ_TEST_CASE_Statistics; }
ENDOFBLOCK
  i=1
  while [[ $i -lt $casv ]]
  do
    echo -n "#define JJ___TEST_CASE_${i}CALLS(name"
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
  echo -n "#define JJ___TEST_CASE_CALLS(name, ...) JJ_PP_SELECTOR$s(__VA_ARGS__"
  while [[ $j -gt 0 ]]
  do
    echo -n ", JJ___TEST_CASE_${j}CALLS"
    j=$((j-1))
  done
  echo ')(name, __VA_ARGS__)'
  echo
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CASE_REG(name, no, args) \
    JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().register_testcase(&JJ_THIS_TESTCLASS::jjM3(runner,no,name), #name, #args);
ENDOFBLOCK
  i=1
  s=$(($casincls-1))
  while [[ $i -lt $casincls ]]
  do
    echo -n "#define JJ___TEST_CASE_${i}REGS(name"
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
  echo -n "#define JJ___TEST_CASE_REGS(name, ...) JJ_PP_SELECTOR$s(__VA_ARGS__"
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CASE_${i}REGS"
    i=$((i-1))
  done
  echo ')(name, __VA_ARGS__)'
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
  - selector count ... generates the JJ_PP_SELECTOR macro
  - merge count ... generates the jjM macro
  - test noclsv nocasv nocasincls ... generates all the test framework macros, where:
      noclsv is the maximum number of class variants for a class
      nocasv is the maximum number of case variants for a case
      nocasincls is the maximum number of cases in a class (variants do not matter)
END_OF_BLOCK
    exit
    ;;
  selector|merge|test)
    generate_"$what" "$@"
    ;;
  *)
    echo "Don't know how to generate '$what'"
    exit 1
esac
