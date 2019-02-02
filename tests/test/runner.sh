#!/usr/bin/env bash

fail()
{ local code=$1
  shift
  echo "$@" >&2
  exit $code
}

BINARY="$1"
shift
[[ -z "$BINARY" ]] && fail 1 "Test binary not specified"
[[ -x "$BINARY" ]] || fail 1 "Test binary not found '$BINARY'"

if [[ "${VERBOSITY_colors}" -gt 0 ]]
then
  COLOR_0="\e[0m"
  COLOR_HL="\e[1m"
  COLOR_PASS="\e[32m"
  COLOR_FAIL="\e[31m"
  COLOR_BACK="\e[39m"
  COLOR_TEST="\e[1;34m"
else
  COLOR_0=''
  COLOR_HL=''
  COLOR_PASS=''
  COLOR_FAIL=''
  COLOR_BACK=''
  COLOR_TEST=''
fi

COUNT_GOOD=0
COUNT_BAD=0

perform()
{ local name="$1" ; shift
  local result="$1" ; shift
  local output="$1" ; shift
  [[ ${VERBOSITY_tests} -gt 1 ]] && {
    echo -e "${COLOR_TEST}$name${COLOR_0}"
    [[ ${VERBOSITY_commands} -gt 0 ]] && echo -e "$@"
  }

  # run the test
  local out
  out="$("$@")"
  local res=$?
  #echo "$out"

  # verify that command returned the expected value
  if [[ $res -eq $result ]]
  then
    :
  else
    COUNT_BAD=$((COUNT_BAD+1))
    echo -e "${COLOR_FAIL}exited with $res ($result expected)${COLOR_0}"
    return # code did not match, failed, no more checks
  fi

  # otherwise check the output of command
  if [[ -z "$output" ]]
  then
    :
  elif [[ $(type -t "$output") == function ]]
  then
    echo "$out" | $output
    res=$?
    [[ $res -eq 0 ]] || {
      COUNT_BAD=$((COUNT_BAD+1))
      return
    }
  elif [[ "$out" != "$output" ]]
  then
    COUNT_BAD=$((COUNT_BAD+1))
    echo -e "${COLOR_FAIL}Output does not match the expected one.${COLOR_0}"
    return
  fi
  COUNT_GOOD=$((COUNT_GOOD+1))
}

perform 'passed_reportedok' 0 '1/0' "$BINARY" -S=short +t testTests_t/passes_reportsok --results=none
perform 'none_reportedok' 0 '1/0' "$BINARY" -S=short +t testTests_t/notests_reportsok --results=none
perform 'warning_reportedok' 0 '1/0' "$BINARY" -S=short +t testTests_t/warning_reportsok --results=none
perform 'error_reportsfail' 1 '0/1' "$BINARY" -S=short +t testTests_t/error_reportsfail --results=none
perform 'fatal4case_reportsfailcontinues' 1 '0/2' "$BINARY" -S=short +t testTests_t/error_endscase +t testTests_t/error_endscase2 --results=none
perform 'fatal4class_reportsfailstops1' 1 '0/0' "$BINARY" -S=short +t testTests_t/error_endsclass +t testTests_t/error_endsclass2 --results=none
perform 'fatal4class_reportsfailstops2' 1 '0/0' "$BINARY" -S=short +t testTests_t/error_endsclass2 +t testTests_t/error_endsclass --results=none
perform 'fatal4class_reportsfailstops3' 1 '1/0' "$BINARY" -S=short +t testTests_t/notests_reportsok +t testTests_t/error_endsclass --results=none
perform 'fatal4class_reportsfailstops4' 1 '0/1' "$BINARY" -S=short +t testTests_t/error_endscase +t testTests_t/error_endsclass --results=none
perform 'variants_nospec' 1 '3/3' "$BINARY" -S=short +t varclass/ --results=none
perform 'variants_class1' 1 '2/1' "$BINARY" -S=short +t 'varclass(1)/' --results=none
perform 'variants_class2' 1 '1/2' "$BINARY" -S=short +t 'varclass(2)/' --results=none
perform 'variants_classandtest1' 1 '1/1' "$BINARY" -S=short +t 'varclass(1)/vartest' --results=none
perform 'variants_classandtest2' 1 '1/1' "$BINARY" -S=short +t 'varclass/vartest(1)' --results=none
perform 'variants_classandtest3' 0 '1/0' "$BINARY" -S=short +t 'varclass(1)/vartest(1)' --results=none
perform 'variants_classandtest4' 1 '2/2' "$BINARY" -S=short +t varclass/vartest --results=none

checkoutputlines()
{ local enter=0
  local leave=0
  local pass=0
  local fail=0
  local res=0
  while read line
  do
    [[ "$line" =~ 'entering' ]] && ((++enter))
    [[ "$line" =~ 'leaving' ]] && ((++leave))
    [[ "$line" =~ 'passed' ]] && ((++pass))
    [[ "$line" =~ 'failed' ]] && ((++fail))
  done
  [[ "$enter" -eq "$1" ]] || { echo -e "${COLOR_FAIL}Number of 'entering' does not match.${COLOR_0}" ; res=1 ; }
  [[ "$leave" -eq "$2" ]] || { echo -e "${COLOR_FAIL}Number of 'leaving' does not match.${COLOR_0}" ; res=1 ; }
  [[ "$pass" -eq "$3" ]] || { echo -e "${COLOR_FAIL}Number of 'passed' does not match.${COLOR_0}" ; res=1 ; }
  [[ "$fail" -eq "$4" ]] || { echo -e "${COLOR_FAIL}Number of 'failed' does not match.${COLOR_0}" ; res=1 ; }
  return $res
}

checkoutputall() { checkoutputlines 2 2 3 3 ; }
checkoutputfails() { checkoutputlines 2 2 0 3 ; }
checkoutputfatals() { checkoutputlines 2 2 0 0 ; }
checkoutputtestsall() { checkoutputlines 0 0 3 3 ; }
checkoutputtestsnone() { checkoutputlines 0 0 0 0 ; }

perform 'outputall' 1 'checkoutputall' "$BINARY" -S=none -cn +t varclass/ --results=all
perform 'outputfails' 1 'checkoutputfails' "$BINARY" -S=none -cn +t varclass/ --results=fails
perform 'outputfatals' 1 'checkoutputfatals' "$BINARY" -S=none -cn +t varclass/ --results=fatals
perform 'outputtestsall' 1 'checkoutputtestsall' "$BINARY" -S=none +t varclass/ --results=all
perform 'outputtestsnone' 1 'checkoutputtestsnone' "$BINARY" -S=none +t varclass/ --results=none

[[ ${COUNT_BAD} -eq 0 ]] && { COLOR_PASS='' ; COLOR_FAIL='' ; COLOR_BACK='' ; }
if [[ VERBOSITY_tests -gt 1 ]]
then
  echo -e "${COLOR_HL}SUMMARY"
  echo -e "Successful tests:   ${COLOR_PASS}${COUNT_GOOD}${COLOR_BACK}"
  echo -e "Failed tests:       ${COLOR_FAIL}${COUNT_BAD}${COLOR_BACK}"
  echo -e "Tests run in total: $((COUNT_GOOD+COUNT_BAD))${COLOR_0}"
elif [[ VERBOSITY_tests -gt 0 ]]
then
  echo -e "${COLOR_HL}${COLOR_PASS}${COUNT_GOOD}${COLOR_BACK}/${COLOR_FAIL}${COUNT_BAD}${COLOR_0}"
fi

[[ $COUNT_BAD -eq 0 ]]
