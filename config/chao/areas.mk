# CHAO stays in the main stage slot while one child occupies the area slot.
# Preserve each image's own whole-image link. Check cross-image call and
# object types with the actual resident members that the child references.

CHAO_CENSUS := $(call dc_base,chao)
CHAO_PLACED := $(CHAO_CENSUS)-placed.o
CHAO_ARCHIVE := $(CHAO_CENSUS)-members.a

ifneq ($(dc_checking),1)

$(CHAO_BANKS).c $(CHAO_BANKS).ld: $(CHAO_BANKS).%: tools/chao_banks.py tools/config.py \
		config/chao/symbols.txt config/chaostgrace/symbols.txt config/chao/areas.mk
	$(Q)python3 tools/chao_banks.py $* $@
$(CHAO_BANKS).o: $(CHAO_BANKS).c sdk/include/ninja.h sdk/include/sg_xpt.h $(GCC_ID) config/gcc.mk
	@$(call status,gcc,$<)
	$(Q)$(call compile,$(call gcc_flags,$<,) -Isdk/include,$<)
-include $(CHAO_BANKS).d
endif

ifneq ($(filter chao,$(dc_scope)),)
area_bound.$(CHAO_CENSUS) := $(CHAO_PLACED)

$(CHAO_PLACED): $(CHAO_CENSUS).o $(CHAO_BANKS).ld config/chao/resident.ld $(LD_ID) config/chao/areas.mk
	@$(call status,ld,$@)
	$(Q)$(SH_ELF)/bin/sh-elf-ld -EL -r -T config/chao/resident.ld -o $@ $<
$(CHAO_ARCHIVE): $(call dc_image_obj,chao) $(CHAO_CENSUS).in $(LD_ID) config/chao/areas.mk
	@$(call status,ar,$@)
	$(Q)rm -f $@ && $(SH_ELF)/bin/sh-elf-ar rcsDP $@ @$(@:%-members.a=%.in)
endif

define chao_area
area_bound.$(call dc_base,$1) := $(call dc_base,$1)-resident.o
$(call dc_base,$1)-interface.o: $(call dc_image_obj,$1) $(call dc_base,$1).in $(call dc_base,$1).cmd \
		$(CHAO_ARCHIVE) $(DC_SH4ZAM_LIB) $(LD_ID) config/chao/areas.mk
	@$$(call status,lto,$$@)
	$$(Q)$$(dc_lto) $(DC_RELOCATABLE) -o $$@ @$(call dc_base,$1).in \
		$(CHAO_ARCHIVE) $(DC_SH4ZAM_LIB) $(LIBGCC) > $$(@:.o=.log) 2>&1 || (cat $$(@:.o=.log); false)
$(call dc_base,$1)-resident.o: $(if $(filter chaostgkinder,$1),$(KINDER_LAYOUT),$(call dc_base,$1).o) $(CHAO_PLACED) \
		$(call dc_base,$1)-interface.o tools/chao_area.py tools/dc_overlay.py tools/census.py tools/sh_elf.py tools/chao_banks.py tools/config.py tools/tool_process.py \
		config/chao/symbols.txt config/$1/symbols.txt $(LD_ID)
	@$$(call status,ld,$$@)
	$$(Q)python3 tools/chao_area.py $1 $$< $(CHAO_PLACED) $$@ --toolchain $(SH_ELF)
endef
$(foreach image,$(filter $(dc_scope),$(CHAO_AREAS)),$(eval $(call chao_area,$(image))))
