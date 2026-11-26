# Final SH-4 programs use the compiled exports of the resident actually
# loaded beside them. The separate relocatable censuses also work with
# partially recovered images.
ifeq ($(MAIN_LAYOUT),)
$(error dc-images needs the resident startup objects and layout)
endif

DC_EXEC := build/dc/images
DC_RESIDENT := $(DC_EXEC)/$(DC_MAIN).elf
DC_OVERLAYS := $(filter-out $(DC_MAIN),$(DC_IMAGES))
DC_OVERLAY_TOOLS := tools/dc_overlay.py tools/census.py tools/sh_elf.py tools/chao_banks.py tools/config.py tools/tool_process.py

$(DC_RESIDENT): $(call dc_terminal,$(DC_MAIN)) $(MAIN_CENSUS).txt config/1st_read/image.ld config/images.mk $(LD_ID)
	@$(call status,ld,$@)
	$(Q)if ! cmp -s /dev/null $(MAIN_CENSUS).txt; then cat $(MAIN_CENSUS).txt >&2; exit 1; fi
	$(Q)mkdir -p $(@D)
	$(Q)$(SH_ELF)/bin/sh-elf-ld -EL --no-undefined --no-warn-rwx-segments -z max-page-size=4096 \
		-T config/1st_read/image.ld -o $@ $<

define dc_overlay
$(DC_EXEC)/$1.elf: $(call dc_terminal,$1) $(DC_RESIDENT) $(DC_OVERLAY_TOOLS) config/images.mk \
		$(if $(filter chao,$1),config/chao/image.ld,config/overlay.ld) \
		$(if $(filter $1,$(CHAO_AREAS)),$(DC_EXEC)/chao.elf config/chao/symbols.txt config/$1/symbols.txt) $(LD_ID)
	@$$(call status,ld,$$@)
	$$(Q)python3 tools/dc_overlay.py $1 $$< $(DC_RESIDENT) $$@ --toolchain $(SH_ELF) \
		$(if $(filter $1,$(CHAO_AREAS)),--chao $(DC_EXEC)/chao.elf)
endef
$(foreach image,$(DC_OVERLAYS),$(eval $(call dc_overlay,$(image))))

# njCnkSetMaterial reads a material chunk's colors from its polygon list as
# a word, which faults on the SH-4 off one, and SHC's image puts every
# chunk's colors on one. make dc compiles each datum of a body source that
# holds a polygon list on a word (dc.mk's dc_body). tools/chunk_colors.py
# holds each linked image to that: it reads every polygon list of the
# image's content and of its bodies, fails on a material chunk whose colors
# lie off a word, and names it. An image that passes gets its count of the
# chunks read, and its pack waits on that
DC_COLORS := $(DC_IMAGES:%=$(DC_EXEC)/%.colors.txt)
$(DC_COLORS): $(DC_EXEC)/%.colors.txt: $(DC_EXEC)/%.elf tools/chunk_colors.py tools/polygon_lists.py tools/sh_elf.py \
		$(CONTENT_TOOLS)
	@$(call status,colors,$@)
	$(Q)python3 tools/chunk_colors.py $* $< $@

# tools/dc_memory.py holds each image to the memory contract, and writes the
# owners of RAM live beside it to the image's report. A Chao area's state
# holds CHAO as well
DC_MEMORY := $(DC_IMAGES:%=build/dc/memory/%.txt)
DC_MEMORY_SOURCES := src/system/areas.h sdk/shinobi/strt2.c src/system/sbinit.c modules/chao/garden.c
$(DC_MEMORY): build/dc/memory/%.txt: $(DC_EXEC)/%.elf $(DC_RESIDENT) $(DC_MEMORY_SOURCES) tools/dc_memory.py \
		$(DC_OVERLAY_TOOLS) tools/csource.py tools/inline.py tools/layout.py config/images.mk
	@$(call status,memory,$@)
	$(Q)python3 tools/dc_memory.py $* build/dc
$(CHAO_AREAS:%=build/dc/memory/%.txt): $(DC_EXEC)/chao.elf

dc-images: $(DC_RESIDENT) $(addprefix $(DC_EXEC)/,$(addsuffix .elf,$(DC_OVERLAYS))) $(DC_COLORS) $(DC_MEMORY)
	@printf '  %-6s 0 of %s material chunks with colors off a word, in %s images\n' \
		colors "$$(cat $(DC_COLORS) | awk '{ n += $$1 } END { print n + 0 }')" $(words $(DC_IMAGES))

# Emit each program at its optical path. Its receipt binds those bytes to
# the exact ELF and to the decoded bytes, including an overlay's BSS. A pack
# waits on its image's colors, and on the report of each state that holds
# its image. Every state holds the main executable, and each Chao area's
# holds CHAO
DC_STATES.$(DC_MAIN) := $(DC_MEMORY)
DC_STATES.chao := $(CHAO_AREAS:%=build/dc/memory/%.txt)
define dc_program
build/dc/$2 build/dc/receipts/$1.txt &: $(DC_EXEC)/$1.elf $(DC_EXEC)/$1.colors.txt build/dc/memory/$1.txt \
		$(DC_STATES.$1) tools/dc_publish.py $(DC_OVERLAY_TOOLS) config/$1/splits.txt config/images.mk build/prskit
	@$$(call status,pack,$2)
	$$(Q)python3 tools/dc_publish.py $1 build/dc
endef
$(foreach i,$(IMAGES),$(eval $(call dc_program,$(firstword $(subst :, ,$i)),$(lastword $(subst :, ,$i)))))

DC_MANIFEST := build/dc/manifest.txt
$(DC_MANIFEST): $(addprefix build/dc/,$(FILES)) $(DC_IMAGES:%=build/dc/receipts/%.txt) tools/dc_manifest.py tools/config.py tools/tool_process.py config/images.mk
	$(Q)python3 tools/dc_manifest.py $@

dc-files: $(DC_MANIFEST)

dc-disc: $(DC_MANIFEST) build/gdi tools/disc.py tools/tool_process.py $(if $(DISC_FILES),tools/dc_extracted.py tools/dc_iso.py config/jp-disc.sha256)
	@$(call status,gdi,build/dc/disc)
ifneq ($(strip $(DISC_FILES)),)
	$(Q)python3 tools/dc_extracted.py $(DC_MANIFEST) $(call quoted,$(DISC_FILES)) build/dc/disc
else
	$(Q)python3 tools/disc.py $(DC_MANIFEST) $(call quoted,$(or $(GDI),orig)) build/dc/disc
endif
