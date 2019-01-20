# this include contains the macros for defining targets for generating Visual Studio solutions and projects
# check define_generate_vssln/define_generate_vsproj below to see the usage

# evaluating this macro call will create a target vssln_<yourname> that generates a sln
# <yourname> (above and below) refers to a string identifying your solution (or project)
# the macro assumes that following values are defined:
#   VSSLN_GUID1_<yourname> ... the main GUID of the solution
#   VSSLN_GUID2_<yourname> ... the secondary GUID in the solution
#   VSSLN_PROJS_<yourname> ... a space separated list of projects in the solution, each name is an id of your project (the <yourname> in your project)
# the following macros can be additionally defined
#   VSSLN_FOLDERS_<yourname> ... gives the list of symbolic names for the following FOLDERDEFS, each describing virtual folders in the solution
#   VSSLN_FOLDERDEFS_NAME_<yourname>_<foldername> ... the name (visible in VS) of the folder (=the path with the solution if nested)
#   VSSLN_FOLDERDEFS_GUID_<yourname>_<foldername> ... the GUID associated with the folder item
# for any of the projects referenced in VSSLN_PROJS_<yourname> the following values shall be defined
#   VS_GUID_<yourname>, SRCDIR_<yourname>
# and the following can be defined
#   VSNAME_<yourname>, VSFOLDER_<yourname>
# see define_generate_vsproj for their description
#(call define_generate_vssln,name,dir)
define define_generate_vssln
VSSLN_TMPINPUT_$(1) ?= ${TMPDIR}/vssln_$(1).input
VSSLN_RESULT_$(1) ?= $(realpath $(2))/$(1).sln

vssln_$(1): | ${TMPDIR}
	$$(call showlabel,$${COLOR_SUPPORT}=== Creating generator input file $${COLOR_HL}$${VSSLN_TMPINPUT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX){ $(foreach proj,${VSSLN_PROJS_$(1)},echo project=$${VSGUID_$(proj)} ; echo path='$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ; echo folder='$${VSFOLDER_$(proj)}' ;) } > "$${VSSLN_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX)echo 'folders' >> $${VSSLN_TMPINPUT_$(1)}
	$(COMMAND_HIDE_PREFIX){ $(foreach fold,${VSSLN_FOLDERS_$(1)},echo '$${VSSLN_FOLDERDEFS_NAME_$(1)_$(fold)}|$${VSSLN_FOLDERDEFS_GUID_$(1)_$(fold)}' ;) } >> "$${VSSLN_TMPINPUT_$(1)}"
	$$(call showlabel,$${COLOR_SUPPORT}=== Generating solution file $${COLOR_HL}$${VSSLN_RESULT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX)tools/generate_vc.pl sln $${VSSLN_GUID1_$(1)} "$(realpath $(2))/$(1).sln" $${VSSLN_GUID2_$(1)} < "$${VSSLN_TMPINPUT_$(1)}" > "$${VSSLN_RESULT_$(1)}"
	$(COMMAND_HIDE_PREFIX)${TOOL_RM} $${VSSLN_TMPINPUT_$(1)}

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
#   VSINCDIRS_<yourname> ... gives all the additional include paths (build conf. variants)
#   VSDEFINES_<yourname> ... gives all extra defines (build conf. variants)
#   VSLIBS_<yourname> ... gives all (static) libraries to link with (build conf. variants)
#   VSLIBDIRS_<yourname> ... gives all additional library paths to search for libs (build conf. variants)
#(call define_generate_vsproj,name)
define define_generate_vsproj
VSNAME_$(1) ?= $(1)
VSPRJ_TMPINPUT_$(1) ?= ${TMPDIR}/vsprj_$(1).input
VSPRJ_RESULT_$(1) ?= $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj
VSFLT_TMPINPUT_$(1) ?= ${TMPDIR}/vsprjf_$(1).input
VSFLT_RESULT_$(1) ?= $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters

vsproj_$(1): | ${TMPDIR}
	$$(call showlabel,$${COLOR_SUPPORT}=== Creating generator input file $${COLOR_HL}$${VSPRJ_TMPINPUT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX){ echo 'includedirs:' ; $(foreach var,${VSINCDIRS_$(1)},echo '${var}' ;) } > "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'defines:' ; $(foreach var,${VSDEFINES_$(1)},echo '${var}' ;) } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'libraries:' ; $(foreach var,${VSLIBS_$(1)},echo '${var}' ;) } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'libdirs:' ; $(foreach var,${VSLIBDIRS_$(1)},echo '${var}' ;) } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'sources:' ; for sf in $${SOURCE_$(1)} ; do echo "$$$$sf" ; done ; } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'includes:' ; for sf in $${VSHEADER_$(1)} ; do echo "$$$$sf" ; done ; } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'references:' ; $(foreach proj,${VSREFS_$(1)},echo $${VSGUID_$(proj)} '$${SRCDIR_$(proj)}/$${VSNAME_$(proj)}.vcxproj' ;) } >> "$${VSPRJ_TMPINPUT_$(1)}"
	$$(call showlabel,$${COLOR_SUPPORT}=== Generating project file $${COLOR_HL}$${VSPRJ_RESULT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX)tools/generate_vc.pl vcproj $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj" $${VSTYPE_$(1)} < "$${VSPRJ_TMPINPUT_$(1)}" > "$${VSPRJ_RESULT_$(1)}"
	$$(call showlabel,$${COLOR_SUPPORT}=== Creating generator input file $${COLOR_HL}$${VSFLT_TMPINPUT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX){ echo 'sources:' ; for sf in $${SOURCE_$(1)} ; do echo "$$$$sf" ; done ; } >> "$${VSFLT_TMPINPUT_$(1)}"
	$(COMMAND_HIDE_PREFIX){ echo 'includes:' ; for sf in $${VSHEADER_$(1)} ; do echo "$$$$sf" ; done ; } >> "$${VSFLT_TMPINPUT_$(1)}"
	$$(call showlabel,$${COLOR_SUPPORT}=== Generating filters file $${COLOR_HL}$${VSFLT_RESULT_$(1)}$${COLOR_0})
	$(COMMAND_HIDE_PREFIX)tools/generate_vc.pl vcfilter $${VSGUID_$(1)} "$$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters" < "$${VSFLT_TMPINPUT_$(1)}" > "$${VSFLT_RESULT_$(1)}"
	$(COMMAND_HIDE_PREFIX)${TOOL_RM} "$${VSPRJ_TMPINPUT_$(1)}" "$${VSFLT_TMPINPUT_$(1)}"

vsall: vsproj_$(1)

vsprojlist_$(1):
	@echo "vsproj_$(1) -> $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj and $$(realpath $${SRCDIR_$(1)})/$${VSNAME_$(1)}.vcxproj.filters"

vslist: vsprojlist_$(1)

.PHONY: vsproj_$(1) vsprojlist_$(1)
endef

.PHONY: vslist vsall
