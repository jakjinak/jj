# this include contains the macros for defining targets for generating Visual Studio solutions and projects
# check define_generate_vssln/define_generate_vsproj below to see the usage

# evaluating this macro call will create a target vssln_<yourname> that generates a sln
# <yourname> (above and below) refers to a string identifying your solution (or project)
# the macro assumes that following values are defined:
#   VSSLN_GUID1_<yourname> ... the main GUID of the solution
#   VSSLN_GUID2_<yourname> ... the secondary GUID in the solution
#   VSSLN_PROJS_<yourname> ... a space separated list of projects in the solution, each name is an id of your project (the <yourname> in your project)
# the following macro can be defined (using define to allow for newlines)
#   VSSLN_FOLDERS_<yourname> ... gives description on folders in the solution to organize projects
#        each line gives one folder definition in form <foldername>|<GUID>
#        ie. the name (and id) of the folder (=the path with the solution if nested), then a pipe '|', then a GUID identifying the folder
# for any of the projects referenced in VSSLN_PROJS_<yourname> the following values shall be defined
#   VS_GUID_<yourname>, SRCDIR_<yourname>
# and the following can be defined
#   VSNAME_<yourname>, VSFOLDER_<yourname>
# see define_generate_vsproj for their description
#(call define_generate_vssln,name,dir)
define define_generate_vssln
.ONESHELL: vssln_$(1)

vssln_$(1):
	{
		$(foreach proj,${VSSLN_PROJS_$(1)},echo project=$${VSGUID_$(proj)} ; echo path='$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ; echo folder='$${VSFOLDER_$(proj)}' ;)
		echo 'folders'
		cat << 'END_OF_BLOCK'
		$${VSSLN_FOLDERS_$(1)}
		END_OF_BLOCK
	} | tools/generate_vc.pl sln $${VSSLN_GUID1_$(1)} "$(realpath $(2))/$(1).sln" $${VSSLN_GUID2_$(1)} > "$(realpath $(2))/$(1).sln"

vsall: vssln_$(1)

vsslnlist_$(1):
	@echo "vssln_$(1) -> $(realpath $(2))/$(1).sln"

vslist: vsslnlist_$(1)

.PHONY: vssln_$(1) vsslnlist_$(1)
endef

# evaluating this macro call will create a target vsproj_<yourname> that generates a vcxproj and vcxproj.filters files
# <yourname> (above and below) refers to a string identifying your project
# the macro assumes that following values are defined:
#   VSTYPE_<yourname> ... can be one of lib/capp/gapp meaning static library/console application/gui application
#   VSGUID_<yourname> ... the GUID used to identify the project in Visual Studio
#   SRCDIR_<yourname>, SOURCE_<yourname> ... see build.mk for these ones
# the macro can take the following if defined
#   VSNAME_<yourname> ... overrides the default (equal to first parameter) name of the project in Visual Studio and also the project filename stem
#   VSHEADER_<yourname> ... a space separated list of header files in the project
#   VSREFS_<yourname> ... gives a space separated list of other project <yourname>s in case there are 'Project dependencies' between such projects
# the following macro shall be defined (using define to allow for newlines)
#   VSINPUT_<yourname> ... gives definitions for additional include directories, defines, linked libraries, additional library directories in a format accepted by tools/generate_vc.pl
#(call define_generate_vsproj,name)
define define_generate_vsproj
VSNAME_$(1) ?= $(1)

.ONESHELL: vsproj_$(1)

vsproj_$(1):
	{
		echo '$${VSINPUT_$(1)}'
		echo 'sources:'
		for sf in $${SOURCE_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'includes:'
		for sf in $${VSHEADER_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'references:'
		$(foreach proj,${VSREFS_$(1)},echo $${VSGUID_$(proj)} '$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ;)
	} | tools/generate_vc.pl vcproj $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj" $${VSTYPE_$(1)} > "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj"
	{
		echo 'sources:'
		for sf in $${SOURCE_$(1)}
		do
			echo "$$$$sf"
		done
		echo 'includes:'
		for sf in $${VSHEADER_$(1)}
		do
			echo "$$$$sf"
		done
	} | tools/generate_vc.pl vcfilter $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters" > "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters"

vsall: vsproj_$(1)

vsprojlist_$(1):
	@echo "vsproj_$(1) -> $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj and $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters"

vslist: vsprojlist_$(1)

.PHONY: vsproj_$(1) vsprojlist_$(1)
endef

.PHONY: vslist vsall
