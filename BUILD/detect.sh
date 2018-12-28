#!/bin/sh

# behaves same way as the actual which binary
# prints full path and returns 0 if first arg is executable,
# does not print anything and returns 1 otherwise
which()
{ [ -z "$1" ] && return 1
  case "$1" in
    /*)
      # absolute path (starts with /)
      [ -d "$1" ] && return 1
      [ -x "$1" ] && { echo "$1" ; return 0 ; }
      return 1
      ;;
    */*)
      # relative path (contains /)
      [ -d "$1" ] && return 1
      [ -x "$1" ] && { echo "`pwd`/$1" ; return 0 ; }
      ;;
    *)
      # no / present
      # note: subshell below, thus exit instead of return
      ( IFS=:
        for d in $PATH ; do
          [ -d "$1" ] && continue
          [ -x "$d/$1" ] && { echo "$d/$1" ; exit 0 ; }
        done
        exit 1
      )
      ;;
  esac
}

showhelp()
{ cat << 'END_OF_BLOCK'
detect.sh [OPTION]

OPTIONS
-h
--help ... Show this help.
-s
--os ... Print the name of the operating system, eg. linux.
-r
--arch ... Print the architecture of the operating system (normalized).
    Possible output values:
    x86 ...... for any of x86, i386, i486, i586, i686, athlon
    x86_64 ... for any of x86_64, x64
END_OF_BLOCK
}

# process input arguments
# note: actually only 1 always expected
case "$1" in
  -h|--help|help)
    showhelp
    exit
    ;;
  -s|--os)
    case "`uname -s`" in
      [Ll][Ii][Nn][Uu][Xx])
        echo linux
        ;;
      *)
        echo "`uname -s`"
        ;;
    esac
    ;;
  -r|--arch)
    which arch > /dev/null && out="`arch`" || out="`uname -p`"
    case "$out" in
      [Xx]86|[Ii][3456]86|[Aa][Tt][Hh][Ll][Oo][Nn])
        echo x86
        ;;
      [xX]86_64|[Xx]64)
        echo x86_64
        ;;
      *)
        echo "$out"
        ;;
    esac
    ;;
  *)
    echo "Invalid argument '$1'!"
    exit 1
    ;;
esac
