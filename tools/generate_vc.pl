#!/usr/bin/env perl

use Data::Dumper;

use strict;

my $type;
my $guid;
my $path;
my $name;
my $ptype;
my $eguid;

my %prjs;
my @prjlst;

my %files;

my @modes = qw(Debug DebugDLL Release ReleaseDLL);
my @archs = qw(x64 x86);
my %parchs;
$parchs{'x64'}='x64';
$parchs{'x86'}='Win32';

sub isguid($)
{ return $_[0] =~ /^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$/;
}

sub processargs
{ my $i = 0;
  for my $a (@ARGV)
  { $i++;
    last if $i == 5;
    $type = $a if $i == 1;
    $guid = $a if $i == 2;
    $path = $a if $i == 3;
    $ptype = $a if $i == 4;
  }
  die("First argument has to be the type of what shall be generated.") unless $type =~ /^sln|vcproj|vcfilter$/;
  die("Second argument has to be the GUID of what shall be generated.") unless isguid($guid);
  die("Third argument has to be the path where what is generated is stored.") unless $path ne '';
  if ($type eq 'sln')
  { $eguid = $ptype;
    die("Fourth argument has to be the secondary GUID of the solution.") unless isguid($eguid);
  }
  elsif ($type eq 'vcproj')
  { die("Fourth argument has to be the type of project (lib,slib,app).") unless $ptype =~ /^(lib|slib|app)$/
  }
  $name = $path;
  $name =~ s/^.*\///;
  $name =~ s/\.[^\.]*$//;
  $path =~ s/\//\\/g;
}

sub readslninput
{ my $cprj = '';
  my $line = 0;
  while (<STDIN>)
  { $line++;
    if (/^project\s*=\s*(.*)$/)
    { my $tmp = $1;
      die("Invalid GUID [$tmp] specified at line $line of input.") unless isguid($tmp);
      $cprj = $tmp;
      push @prjlst, $tmp;
    }
    elsif (/^([-a-zA-Z0-9]+)\s*=\s*(.*)$/)
    { die("No project defined yet, but property encountered.") unless $cprj ne '';
      my $prop = $1;
      my $val = $2;
      $prjs{$cprj}{$prop} = $val;
    }
    else
    { die("Invalid line $line found in the input.");
    }
  }
}

sub verifyslninput
{ for my $p (keys %prjs)
  { die("Invalid path [$prjs{$p}{'path'}] of project [$p].") unless $prjs{$p}{'path'} ne '';
    my $n = $prjs{$p}{'path'};
    $n =~ s/^.*\///;
    $n =~ s/\.[^\.]*$//;
    die("Path [$prjs{$p}{'path'}] of project [$p] contains empty name part.") if $n eq '';
    $prjs{$p}{'name'} = $n;
    $prjs{$p}{'path'} =~ s/\//\\/g;
  }
}

sub readprjinput
{ my $csec = '';
  my $line = 0;
  while (<STDIN>)
  { $line++;
    next if /^\s*(#.*)?$/;
    if (/^(includes|sources|properties|includedirs|defines|references|libraries|libdirs):$/)
    { $csec = $1;
    }
    elsif ($csec eq '')
    { die("No section yet on line $line.");
    }
    elsif ($csec =~ /^(includes|sources)$/)
    { chomp;
      $files{$csec}{$_} = 1;
    }
    elsif ($csec eq 'properties')
    { chomp;
      if (/^\s*([*=])\s+(.*?)\s*$/)
      { $files{'props'}{$2} = $1;
      }
      else
      { die("Invalid property syntax at line $line.");
      }
    }
    elsif ($csec eq 'references')
    { chomp;
      if (/^\s*([-0-9a-fA-F]+)\s+(.*?)\s*$/)
      { my $guid = $1;
        my $prj = $2;
        die("Invalid guid at line $line.") unless isguid($guid);
        $files{'refs'}{$guid} = $prj;
      }
      else
      { die("Invalid reference syntax at line $line.");
      }
    }
    elsif ($csec =~ /^(includedirs|defines|libraries|libdirs)$/)
    { chomp;
      my %entry;
      if (/^(.*)\|(.*)$/)
      { my $cnd = $1;
        $entry{'v'} = $2;
        my @cnds = split(/,\s*/, $cnd);
        my @mc = ();
        for my $cx (@cnds)
        { next if $cx =~ /^\s*$/;
          #print STDERR "XXX '$cx'\n";
          if ($cx =~ /^(m|mode|o|os|a|arch|l|link|r|rtlink)\s*(=|==|~|=~|!=|!~)\s*(.*)$/)
          { my $what = $1;
            my $op = $2;
            my $v = $3;
            $what = 'mode' if $what eq 'm';
            $what = 'os' if $what eq 'o';
            $what = 'arch' if $what eq 'a';
            $what = 'link' if $what eq 'l';
            $what = 'rtlink' if $what eq 'r';
            $op = '=' if $op eq '==';
            $op = '~' if $op eq '~=';
            my %tmp;
            $tmp{'p'} = $what;
            $tmp{'o'} = $op;
            $tmp{'v'} = $v;
            push @mc, \%tmp;
          }
          else
          { die("Invalid condition syntax in $cnd at line $line.");
          }
        }
        @{$entry{'c'}} = @mc;
      }
      else
      { @{$entry{'c'}} = ();
        $entry{'v'} = $_;
      }
      @{$files{$csec}} = () unless exists $files{$csec};
      push @{$files{$csec}}, \%entry;
    }
    else
    { # unknown section, ignore
    }
  }
}

sub indent($)
{ my $tmp = '';
  my $i = $_[0];
  while ($i > 0)
  { $tmp.='  ';
    $i--;
  }
  return $tmp;
}

sub sln
{ print "\nMicrosoft Visual Studio Solution File, Format Version 12.00\n";
  print "# Visual Studio 15\n";
  print "VisualStudioVersion = 15.0.27004.2009\n";
  print "MinimumVisualStudioVersion = 10.0.40219.1\n";
  for my $p (@prjlst)
  { print "Project(\"{$eguid}\") = \"$prjs{$p}{'name'}\", \"$prjs{$p}{'path'}\", \"{$p}\"\n";
    print "EndProject\n";
  }
  print "Global\n";
  print "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
  for my $m (@modes)
  { for my $a (@archs)
    { print "\t\t$m|$a = $m|$a\n";
    }
  }
  print "\tEndGlobalSection\n";
  print "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
  for my $p (@prjlst)
  { for my $m (@modes)
    { for my $a (@archs)
      { print "\t\t{$p}.$m|$a.ActiveCfg = $m|$parchs{$a}\n";
        print "\t\t{$p}.$m|$a.Build.0 = $m|$parchs{$a}\n";
      }
    }
  }
  print "\tEndGlobalSection\n";
  print "\tGlobalSection(SolutionProperties) = preSolution\n";
  print "\t\tHideSolutionNode = FALSE\n";
  print "\tEndGlobalSection\n";
  print "\tGlobalSection(ExtensibilityGlobals) = postSolution\n";
  print "\t\tSolutionGuid = {$guid}\n";
  print "\tEndGlobalSection\n";
  print "EndGlobal\n";
}

sub propfilename($$$$)
{ my $x = shift;
  my $t = shift;
  my $m = shift;
  my $a = shift;
  if ($t eq '=')
  {
  }
  elsif ($t eq '*')
  { $x.='dll' if $m =~ /DLL$/;
    $x.='64' if $a eq 'x64';
    $x.='32' if $a eq 'x86';
    $x.='d' if $m =~/^Debug/;
    $x.='r' if $m =~/^Release/;
  }
  $x.='.props';
  return $x;
}

sub evaluatevalue($$$)
{ my $a = shift;
  my $o = shift;
  my $v = shift;
  #print STDERR "$a $o $v\n";
  return $a =~ /^$v$/i if $o eq '=';
  return $a !~ /^$v$/i if $o eq '!=';
  return $a =~ /$v/i if $o eq '~';
  return $a !~ /$v/i if $o eq '!~';
  die("Invalid operator '$o'");
}

sub evaluateinput($$$$)
{ my $sec = shift;
  my $sep = shift;
  my $m = shift;
  my $a = shift;
  my $rl = ($m =~ /DLL$/?'dll':'static');
  $m =~ s/DLL$//;
  my $x = '';
  for my $entry (@{$files{$sec}})
  { my %e = %{$entry};
    my @cnds = @{$e{'c'}};
    my $r = 1; # matching unless non-matching found
    for my $cnd (@cnds)
    { my %c = %{$cnd};
      if ($c{'p'} eq 'mode')
      { #print STDERR "mode $m $c{'o'} $c{'v'}\n";
        $r = evaluatevalue($m,$c{'o'},$c{'v'});
      }
      elsif ($c{'p'} eq 'os')
      { # nothing, not implemented
      }
      elsif ($c{'p'} eq 'arch')
      { #print STDERR "arch $a $c{'o'} $c{'v'}\n";
        $r = evaluatevalue($a,$c{'o'},$c{'v'});
      }
      elsif ($c{'p'} eq 'link')
      { # nothing, not implemented
      }
      elsif ($c{'p'} eq 'rtlink')
      { #print STDERR "rtlink $rl $c{'o'} $c{'v'}\n";
        $r = evaluatevalue($rl,$c{'o'},$c{'v'});
      }
      else
      { die("Invalid property '$c{'p'}'.");
      }
      last if $r == 0;
    }
    next if $r == 0;
    $x.=$e{'v'}.$sep;
  }
  return $x;
}

sub projecttype($)
{ return 'StaticLibrary' if $_[0] eq 'lib';
  return 'DynamicLibrary' if $_[0] eq 'slib';
  return 'Application' if $_[0] eq 'app';
  die("Invalid project type '$_[0]'.");
}

sub vcproj
{ print '<?xml version="1.0" encoding="utf-8"?>'."\n";
  print '<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">'."\n";
  print indent(1).'<ItemGroup Label="ProjectConfigurations">'."\n";
  for my $m (@modes)
  { for my $a (keys %parchs)
    { print indent(2)."<ProjectConfiguration Include=\"$m|$parchs{$a}\">\n";
      print indent(3)."<Configuration>$m</Configuration>\n";
      print indent(3)."<Platform>$parchs{$a}</Platform>\n";
      print indent(2)."</ProjectConfiguration>\n";
    }
  }
  print indent(1).'</ItemGroup>'."\n";
  print indent(1).'<PropertyGroup Label="Globals">'."\n";
  print indent(2).'<VCProjectVersion>15.0</VCProjectVersion>'."\n";
  print indent(2)."<ProjectGuid>{$guid}</ProjectGuid>\n";
  print indent(2)."<Keyword>Win32Proj</Keyword>\n";
  print indent(2)."<RootNamespace>$name</RootNamespace>\n";
  print indent(2)."<WindowsTargetPlatformVersion>10.0.16299.0</WindowsTargetPlatformVersion>\n";
  print indent(1)."</PropertyGroup>\n";
  print indent(1).'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />'."\n";
  my $ct = projecttype($ptype);
  for my $m (@modes)
  { for my $a (keys %parchs)
    { print indent(1)."<PropertyGroup Condition=\"'\$(Configuration)|\$(Platform)'=='$m|$parchs{$a}'\" Label=\"Configuration\">\n";
      print indent(2)."<ConfigurationType>$ct</ConfigurationType>\n";
      print indent(2).'<UseDebugLibraries>'.($m =~ /^Release/?'false':'true').'</UseDebugLibraries>'."\n";
      print indent(2)."<PlatformToolset>v141</PlatformToolset>\n";
      print indent(2)."<WholeProgramOptimization>true</WholeProgramOptimization>\n" if $m =~ /^Release/;
      print indent(2)."<CharacterSet>Unicode</CharacterSet>\n";
      print indent(1)."</PropertyGroup>\n";
    }
  }
  print indent(1).'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />'."\n";
  print indent(1).'<ImportGroup Label="ExtensionSettings">'."\n";
  print indent(1).'</ImportGroup>'."\n";
  print indent(1).'<ImportGroup Label="Shared">'."\n";
  print indent(1).'</ImportGroup>'."\n";
  for my $m (@modes)
  { for my $a (keys %parchs)
    { print indent(1)."<ImportGroup Label=\"PropertySheets\" Condition=\"'\$(Configuration)|\$(Platform)'=='$m|$parchs{$a}'\">\n";
      print indent(2)."<Import Project=\"\$(UserRootDir)\\Microsoft.Cpp.\$(Platform).user.props\" Condition=\"exists('\$(UserRootDir)\\Microsoft.Cpp.\$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n";
      for my $p (sort keys %{$files{'props'}})
      { print indent(2)."<Import Project=\"".propfilename($p, $files{'props'}{$p}, $m, $a)."\" />\n";
      }
      print indent(1)."</ImportGroup>\n";
    }
  }
  print indent(1)."<PropertyGroup Label=\"UserMacros\" />\n";
  for my $m (@modes)
  { for my $a (keys %parchs)
    { print indent(1)."<PropertyGroup Condition=\"'\$(Configuration)|\$(Platform)'=='$m|$parchs{$a}'\">\n";
      print indent(2)."<LinkIncremental>".($m =~ /^Release/?'false':'true')."</LinkIncremental>\n";
      print indent(2)."<OutDir>\$(SolutionDir).bin\\\$(Configuration).\$(Platform)\\</OutDir>\n";
      print indent(2)."<IntDir>\$(SolutionDir).bin\\\$(Configuration).\$(Platform)\\\$(ProjectName)\\</IntDir>\n";
      print indent(1)."</PropertyGroup>\n";
    }
  }
  for my $m (@modes)
  { for my $a (keys %parchs)
    { print indent(1)."<ItemDefinitionGroup Condition=\"'\$(Configuration)|\$(Platform)'=='$m|$parchs{$a}'\">\n";
      print indent(2)."<ClCompile>\n";
      if (0 < scalar(keys %{$files{'includes'}}))
      { print indent(3)."<PrecompiledHeader>\n";
        print indent(3)."</PrecompiledHeader>\n";
      }
      print indent(3)."<WarningLevel>Level3</WarningLevel>\n";
      if ($m =~ /^Debug/)
      { print indent(3)."<Optimization>Disabled</Optimization>\n";
      }
      else
      { print indent(3)."<Optimization>MaxSpeed</Optimization>\n";
        print indent(3)."<FunctionLevelLinking>true</FunctionLevelLinking>\n";
        print indent(3)."<IntrinsicFunctions>true</IntrinsicFunctions>\n";
      }
      print indent(3)."<SDLCheck>true</SDLCheck>\n";
      my $ed = evaluateinput('defines',';',$m,$a);
      print indent(3)."<PreprocessorDefinitions>${ed}%(PreprocessorDefinitions)</PreprocessorDefinitions>\n";
      my $ad = evaluateinput('includedirs',';',$m,$a);
      print indent(3)."<AdditionalIncludeDirectories>$ad%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>\n";
      my $runtime = 'MultiThreaded';
      $runtime.='Debug' if $m =~ /^Debug/;
      $runtime.='DLL' if $m =~ /DLL$/;
      print indent(3)."<RuntimeLibrary>$runtime</RuntimeLibrary>\n";
      print indent(2)."</ClCompile>\n";
      print indent(2)."<Link>\n";
      print indent(3)."<SubSystem>Windows</SubSystem>\n";
      if ($m =~ /^Release/)
      { print indent(3)."<EnableCOMDATFolding>true</EnableCOMDATFolding>\n";
        print indent(3)."<OptimizeReferences>true</OptimizeReferences>\n";
      }
      print indent(3)."<GenerateDebugInformation>true</GenerateDebugInformation>\n";
      if ($ptype eq 'app')
      { my $libs = evaluateinput('libraries',';',$m,$a);
        print indent(3)."<AdditionalDependencies>$libs%(AdditionalDependencies)</AdditionalDependencies>\n";
        my $ldirs = evaluateinput('libdirs',';',$m,$a);
        print indent(3)."<AdditionalLibraryDirectories>$ldirs%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n";
      }
      print indent(2)."</Link>\n";
      print indent(1)."</ItemDefinitionGroup>\n";
    }
  }
  if (0 < scalar(keys %{$files{'includes'}}))
  { print indent(1)."<ItemGroup>\n";
    for my $f (sort keys %{$files{'includes'}})
    { print indent(2)."<ClInclude Include=\"$f\" />\n";
    }
    print indent(1)."</ItemGroup>\n";
  }
  if (0 < scalar(keys %{$files{'sources'}}))
  { print indent(1)."<ItemGroup>\n";
    for my $f (sort keys %{$files{'sources'}})
    { print indent(2)."<ClCompile Include=\"$f\" />\n";
    }
    print indent(1)."</ItemGroup>\n";
  }
  if (0 < scalar(keys %{$files{'refs'}}))
  { print indent(1)."<ItemGroup>\n";
    for my $r (sort keys %{$files{'refs'}})
    { print indent(2)."<ProjectReference Include=\"$files{'refs'}{$r}\">\n";
      print indent(3)."<Project>{$r}</Project>\n";
      print indent(2)."</ProjectReference>\n";
    }
    print indent(1)."</ItemGroup>\n";
  }
  print indent(1)."<Import Project=\"\$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n";
  print indent(1)."<ImportGroup Label=\"ExtensionTargets\">\n";
  print indent(1)."</ImportGroup>\n";
  print '</Project>';
}

sub vcfilter
{ print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  print "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
  print indent(1)."<ItemGroup>\n";
  print indent(2)."<Filter Include=\"Source Files\">\n";
  print indent(3)."<UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>\n";
  print indent(3)."<Extensions>cpp;c;cc;cxx;def;odl;idl;hpj;bat;asm;asmx</Extensions>\n";
  print indent(2)."</Filter>\n";
  print indent(2)."<Filter Include=\"Header Files\">\n";
  print indent(3)."<UniqueIdentifier>{93995380-89BD-4b04-88EB-625FBE52EBFB}</UniqueIdentifier>\n";
  print indent(3)."<Extensions>h;hh;hpp;hxx;hm;inl;inc;xsd</Extensions>\n";
  print indent(2)."</Filter>\n";
  print indent(2)."<Filter Include=\"Resource Files\">\n";
  print indent(3)."<UniqueIdentifier>{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}</UniqueIdentifier>\n";
  print indent(3)."<Extensions>rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav;mfcribbon-ms</Extensions>\n";
  print indent(2)."</Filter>\n";
  print indent(1)."</ItemGroup>\n";
  if (0 < scalar(keys %{$files{'includes'}}))
  { print indent(1)."<ItemGroup>\n";
    for my $f (sort keys %{$files{'includes'}})
    { print indent(2)."<ClInclude Include=\"$f\">\n";
      print indent(3)."<Filter>Header Files</Filter>\n";
      print indent(2)."</ClInclude>\n";
    }
    print indent(1)."</ItemGroup>\n";
  }
  if (0 < scalar(keys %{$files{'sources'}}))
  { print indent(1)."<ItemGroup>\n";
    for my $f (sort keys %{$files{'sources'}})
    { print indent(2)."<ClCompile Include=\"$f\">\n";
      print indent(3)."<Filter>Source Files</Filter>\n";
      print indent(2)."</ClCompile>\n";
    }
    print indent(1)."</ItemGroup>\n";
  }
  print "</Project>";
}

processargs;

if ($type eq 'sln')
{ readslninput;
  verifyslninput;
  sln;
}
elsif ($type eq 'vcproj')
{ readprjinput;
  vcproj;
}
elsif ($type eq 'vcfilter')
{ readprjinput;
  vcfilter;
}
