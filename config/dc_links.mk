# dc.mk reads these rules for all production images, then for the images
# whose disc-free check takes layout-only data. dc_base gives them separate
# link products, including response files and logs. Both a single worker
# and the jobserver fallback pass -flto=1. Record which policy ran the link.
ifneq ($(DC_GOALS),)
$(foreach i,$(dc_scope),$(call keep,$(call dc_base,$i).in,$(call dc_image_obj,$i))$(call \
	keep,$(call dc_base,$i).cmd,DC_LTO_JOBS=$(DC_LTO_JOBS) $(GCC_LTO_LINK) $(DC_RELOCATABLE) $(DC_SCRIPT.$i) \
	$(call unexpanded,dc_census dc_lto)))
endif
# Kindergarten resets the game's BSS and the SDK's BSG separately. The
# whole-image LTO link still takes both groups, so GCC can report a C type
# that differs across that boundary.
# For the loadable layout, compile the SG members together so GNU ld retains
# their identity when it places the two ranges. Select them by the original
# image's output sections, including assembly members and data-only objects.
ifneq ($(and $(DC_GOALS),$(filter chaostgkinder,$(dc_scope))),)
KINDER_CENSUS := $(call dc_base,chaostgkinder)
KINDER_SG := $(KINDER_CENSUS)-sdk.o
KINDER_LAYOUT := $(KINDER_CENSUS)-layout.o
KINDER_BSS := config/chaostgkinder/bss.ld
kinder_sg_sources := $(call from_config,sorted({r.writer for r in config.Image("chaostgkinder").placements \
    if r.writer != config.DISC and r.section in "PSG CSG DSG BSG DSG_INIT_ DSG_END_".split()}))
kinder_sg_objects := $(filter $(addsuffix .o,$(addprefix build/dc/,$(basename $(kinder_sg_sources)))),$(call dc_members,chaostgkinder))
ifneq ($(dc_checking),)
kinder_sg_objects := $(call dc_checked_objects,$(kinder_sg_objects))
endif
kinder_layout_inputs := $(filter-out $(kinder_sg_objects),$(call dc_image_obj,chaostgkinder)) $(KINDER_SG)
$(call keep,$(KINDER_SG:.o=.in),$(kinder_sg_objects))
$(call keep,$(KINDER_LAYOUT:.o=.in),$(kinder_layout_inputs))
$(call keep,$(KINDER_LAYOUT:.o=.cmd),DC_LTO_JOBS=$(DC_LTO_JOBS) $(GCC_LTO_LINK) $(DC_RELOCATABLE) \
	-Xlinker -T -Xlinker $(KINDER_BSS))

area_bound.$(KINDER_CENSUS) := $(KINDER_LAYOUT)
$(KINDER_SG): private DC_SH4ZAM_LIB :=
$(KINDER_SG): private LIBGCC :=
$(KINDER_SG): $(kinder_sg_objects) $(KINDER_SG:.o=.in) $(KINDER_CENSUS).cmd $(LD_ID)
	@$(call status,lto,$@)
	$(Q)$(dc_census)
$(KINDER_LAYOUT): private DC_RELOCATABLE += -Xlinker -T -Xlinker $(KINDER_BSS)
$(KINDER_LAYOUT): $(KINDER_CENSUS).o $(KINDER_SG) $(KINDER_LAYOUT:.o=.in) $(KINDER_LAYOUT:.o=.cmd) $(KINDER_BSS)
	@$(call status,ld,$@)
	$(Q)$(dc_census)
endif

ifneq ($(and $(DC_GOALS),$(filter $(DC_MAIN),$(dc_scope)),$(filter build/dc/sdk/shinobi/strt1.o,$(call dc_members,$(DC_MAIN)))),)
include config/1st_read/startup.mk
endif

ifneq ($(and $(DC_GOALS),$(filter chao,$(DC_IMAGES)),$(filter chao $(CHAO_AREAS),$(dc_scope))),)
include config/chao/areas.mk
endif

# The terrain compiler writes into free RAM following its own overlay. Place
# the complete image before taking that boundary, including the imports the
# resident Chao garden supplies to its child. The ordinary LTO object remains
# available with one section per function for type and reachability checks.
define dc_terrain
$(call dc_base,$1)-terrain.o: $(call dc_bound,$1) $(DC_TERRAIN_LAYOUT) $(LD_ID)
	@$$(call status,ld,$$@)
	$$(Q)$(SH_ELF)/bin/sh-elf-ld -EL -r -T $(DC_TERRAIN_LAYOUT) -o $$@ $$<
endef
$(foreach i,$(filter $(dc_scope),$(DC_TERRAIN)),$(eval $(call dc_terrain,$i)))

# DC_TAKEN is what tools/census.py takes for an image other than the main
# executable: the main executable's relocatable object and its symbols.txt
DC_TAKEN := $(call dc_bound,$(DC_MAIN)) config/$(DC_MAIN)/symbols.txt
define dc_image
$(call dc_base,$1).o: $(call dc_image_obj,$1) $(call dc_base,$1).in $(call dc_base,$1).cmd $(DC_SH4ZAM_LIB) \
		$(LD_ID) $(DC_SCRIPT.$1)
	@$$(call status,lto,$$@)
	$$(Q)$$(call dc_census,$1)
endef
$(foreach i,$(dc_scope),$(eval $(call dc_image,$i)))

define dc_census_report
$(call dc_report,$1): $(call dc_terminal,$1) \
		tools/census.py tools/sh_elf.py tools/config.py $(if $(dc_checking),$(dc_content_inputs.$1)) \
		$(if $(filter-out $(DC_MAIN),$1),$(DC_TAKEN))
	$$(Q)python3 tools/census.py $$< $$@$(if $(filter-out $(DC_MAIN),$1), --main $(word 1,$(DC_TAKEN)) --symbols \
		$(word 2,$(DC_TAKEN)))$(if $(and $(dc_checking),$(filter $1,$(CONTENT_IMAGES))), --content $1)
endef
$(foreach i,$(dc_reports),$(eval $(call dc_census_report,$i)))
