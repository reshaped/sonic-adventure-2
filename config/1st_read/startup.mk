# Whole-image LTO still checks the program and SDK declarations together.
# Materialize the SDK members separately so GNU ld can assign their mutable
# objects to the same startup families as the original image.
MAIN_CENSUS := $(call dc_base,1st_read)
MAIN_SDK := $(MAIN_CENSUS)-sdk.o
MAIN_CACHE := $(MAIN_CENSUS)-cache.o
MAIN_STORAGE := $(MAIN_CENSUS)-storage.o
MAIN_LAYOUT := $(MAIN_CENSUS)-layout.o

main_members := $(call dc_members,1st_read)
main_sg_sources := $(call from_config,sorted({r.writer for r in config.Image("1st_read").placements if r.writer != config.DISC and r.section[:3] in "PSG CSG DSG BSG".split()}))
main_cache_sources := $(call from_config,sorted({r.writer for r in config.Image("1st_read").placements if r.writer != config.DISC and r.section in "BSG32 DSG32".split()}))
main_cache_members := $(filter $(addprefix build/dc/,$(addsuffix .o,$(basename $(main_cache_sources)))),$(main_members))
# storage.ld opens the resident head with strt1.o's .text, which it finds by
# file name, so strt1.o stays out of the SDK's partial link.
main_sdk_members := $(filter-out $(main_cache_members) build/dc/sdk/shinobi/strt1.o,$(filter $(addprefix build/dc/,$(addsuffix .o,$(basename $(main_sg_sources)))),$(main_members)))
main_program_members := $(filter-out $(main_cache_members) $(main_sdk_members),$(main_members))
ifneq ($(dc_checking),)
main_cache_members := $(call dc_checked_objects,$(main_cache_members))
main_sdk_members := $(call dc_checked_objects,$(main_sdk_members))
main_program_members := $(call dc_checked_objects,$(main_program_members))
endif

$(call keep,$(MAIN_CACHE:.o=.in),$(main_cache_members))
$(call keep,$(MAIN_SDK:.o=.in),$(main_sdk_members))
$(call keep,$(MAIN_STORAGE:.o=.in),$(main_program_members) $(MAIN_SDK) $(MAIN_CACHE))
# override keeps these links' settings where the command line sets the variable.
$(MAIN_CACHE) $(MAIN_SDK): private override DC_SH4ZAM_LIB :=
$(MAIN_CACHE) $(MAIN_SDK): private override LIBGCC :=
$(MAIN_CACHE): $(main_cache_members)
$(MAIN_SDK): $(main_sdk_members)
$(MAIN_CACHE) $(MAIN_SDK): %.o: %.in $(MAIN_CENSUS).cmd $(LD_ID) config/1st_read/startup.mk
	@$(call status,lto,$@)
	$(Q)$(dc_census)
$(MAIN_STORAGE): private override DC_RELOCATABLE += -Xlinker -T -Xlinker config/1st_read/storage.ld
$(MAIN_STORAGE): $(MAIN_CENSUS).o $(MAIN_CACHE) $(MAIN_SDK) $(MAIN_STORAGE:.o=.in) config/1st_read/storage.ld
	@$(call status,lto,$@)
	$(Q)$(call dc_census,1st_read)
$(MAIN_LAYOUT): $(MAIN_STORAGE) config/1st_read/startup.ld $(LD_ID)
	@$(call status,ld,$@)
	$(Q)$(SH_ELF)/bin/sh-elf-ld -EL -r -T config/1st_read/startup.ld -o $@ $<
area_bound.$(MAIN_CENSUS) := $(MAIN_LAYOUT)
