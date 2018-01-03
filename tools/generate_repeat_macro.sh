#!/bin/bash

cnt="$1"
[[ $cnt -lt 1 ]] && { echo "Invalid count '$cnt'" ; exit 1 ; }
shift

function generate_selector()
{ echo -n '#define JJ_PP_SELECTOR('
  while [[ $i -lt $cnt ]]
  do
    echo -n "p$i, "
    i=$((i+1))
  done
  echo 'actual, ...) actual'
}

function generate_merge()
{ while [[ $i -lt $cnt ]]
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
  echo -n '#define jjM(...) JJ_PP_SELECTOR(__VA_ARGS__'
  while [[ $i -gt 0 ]]
  do
    echo -n ", jjM$i"
    i=$((i-1))
  done
  echo ')'
}

function generate_test_stuff()
{ cat << 'ENDOFBLOCK'
#define JJ___TEST_CLASS_CALL(name, no, args) \
    static void jjM2(JJ_TEST_CLASS_VARIANT_,no)(){ name inst args; JJ_THIS_TESTCLASS::JJ_TESTCASE_holder_t::instance().run(inst); } \
    static int jjM2(JJ_TEST_CLASS_REGISTRAR_,no)() { jj::test::db_t::instance().register_testclass(&name::jjM2(JJ_TEST_CLASS_VARIANT_,no), #name, #args); return no; }
ENDOFBLOCK
  i=1
  while [[ $i -lt $cnt ]]
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
  echo -n "#define JJ___TEST_CLASS_CALLS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__"
  j=$((cnt-1))
  while [[ $j -gt 0 ]]
  do
    echo -n ", JJ___TEST_CLASS_${j}CALLS"
    j=$((j-1))
  done
  echo ')(name, __VA_ARGS__)'
  echo
  echo '#define JJ___TEST_CLASS_REG(no) jjM2(JJ_TEST_CLASS_REGISTRAR_,no)();'
  i=1
  while [[ $i -lt $cnt ]]
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
  echo -n '#define JJ___TEST_CLASS_REGS(...) JJ_PP_SELECTOR(__VA_ARGS__'
  i=$((cnt-1))
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CLASS_${i}REGS"
    i=$((i-1))
  done
  echo ")(__VA_ARGS__)"
  echo
  echo '#define JJ___TEST_CASE_DEF(classname, testname) classname::jjM2(registrar_,testname)();'
  i=1
  while [[ $i -lt $cnt ]]
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
  i=$((cnt-1))
  echo -n '#define JJ___TEST_CASE_DEFS(classname, ...) JJ_PP_SELECTOR(__VA_ARGS__'
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CASE_${i}DEFS"
    i=$((i-1))
  done
  echo ")(classname, __VA_ARGS__)"
  echo
  cat << 'ENDOFBLOCK'
#define JJ___TEST_CASE_CALL(name, no, args) \
    static void jjM3(runner,no,name)(JJ_THIS_TESTCLASS& inst) { (inst.*&JJ_THIS_TESTCLASS::name) args; }
ENDOFBLOCK
  i=1
  while [[ $i -lt $cnt ]]
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
  j=$((cnt-1))
  echo -n '#define JJ___TEST_CASE_CALLS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__'
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
  while [[ $i -lt $cnt ]]
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
  i=$((cnt-1))
  echo -n '#define JJ___TEST_CASE_REGS(name, ...) JJ_PP_SELECTOR(__VA_ARGS__'
  while [[ $i -gt 0 ]]
  do
    echo -n ", JJ___TEST_CASE_${i}REGS"
    i=$((i-1))
  done
  echo ')(name, __VA_ARGS__)'
}

while [[ $# -gt 0 ]]
do
  i=1
  case "$1" in
    selector)
      generate_selector
      ;;
    merge)
      generate_merge
      ;;
    test)
      generate_test_stuff
      ;;
    *)
      echo "Unknown argument '$1'"
      exit 1
  esac
  shift
done
