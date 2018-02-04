#!/usr/bin/env perl

# this tool will convert the output of make -d into xml based on the indentation of individual lines

use strict;

my $inrecipe = 0;
my $recipetxt = '';
my $level = 0;

print "<xml>\n";
while (<STDIN>)
{ if ($inrecipe == 1)
  { if (/^Removing child /)
    { $inrecipe = 0;
      $recipetxt.=$_;
      $recipetxt =~ s/</&lt;/g;
      $recipetxt =~ s/>/&gt;/g;
      print "<text>$recipetxt</text>\n";
      next;
    }
    $recipetxt.=$_;
  }
  else
  { if (/^Invoking recipe from /)
    { $inrecipe = 1;
      $recipetxt = $_;
      next;
    }
    chomp;
    /^(\s*)/;
    my $cnt = length($1);
    s/^(\s*)//;
    while ($level<$cnt)
    { print "<sub>\n";
      $level++;
    }
    while ($level>$cnt)
    { print "</sub>\n";
      $level--;
    }
    s/</&lt;/g;
    s/>/&gt;/g;
    print "<text>$_</text>\n";
  }
}
print "</xml>\n";
