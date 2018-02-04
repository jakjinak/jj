#!/usr/bin/env perl

use Cwd;
use strict;

sub mode($)
{ return $ENV{'MODE'} =~ /^$_[0]$/i;
}

sub trim
{ my $s = shift;
  $s =~ s/^\s+|\s+$//g;
  return $s
}

print "digraph Makefile {\n" if mode('dot');

my %files;

while (<STDIN>)
{ next unless /^[-_\.\/a-zA-Z0-9]+:/;
  my $cwd = Cwd::abs_path(getcwd());
  s/$cwd\//.../g;
  s/\.bin\/[^\/]+\///g;
  if (mode('dot'))
  { chomp;
    /^([^:]+):(.*)/;
    my $tgt = $1;
    my $d = $2;
    my @deps = split(/ /, $d);
    $tgt = trim($tgt);
    next if $tgt eq '.PHONY' or $tgt eq '.SUFFIXES';
    for my $i (@deps)
    { next if $i =~ /^\s*$/;
      next if $i =~ /dircreated$/;
      next if $ENV{'SKIPHEADERS'} and $i =~ /\.h$/i;
      print "\"$tgt\" -> \"".trim($i)."\"\n";
      if ($i =~ /\.(c|cpp|h|o|a|so)$/i)
      { my $suf = $1;
        $suf =~ y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/;
        $files{$suf}{$i} = 1;
      }
      elsif ($i =~ /^\.\.\./)
      { $files{''}{$i} = 1;
      }
    }
  }
  else
  { print $_;
  }
}

sub printnodes($$)
{ my @nodes = keys %{$files{$_[0]}};
  for my $n (@nodes)
  { print "\"$n\" [shape=\"box\" style=\"filled,rounded\" fillcolor=\"$_[1]\"]\n";
  }
}

if (mode('dot'))
{ printnodes('c', '#DDFFDD');
  printnodes('cpp', '#DDFFDD');
  printnodes('h', '#DDDDFF');
  printnodes('o', '#FFFFDD');
  printnodes('a', '#FFDDDD');
  printnodes('so', '#FFDDDD');
  printnodes('', '#FFAAAA');
  print "}\n";
}
