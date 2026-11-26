# Include bodies are named relative to the source's top-level include
# directory (sdk/, src/ or modules/). The full build reads one placing
# image's bytes. Links for every image independently check their contents.
body_root = $2/$(body_directory.$1)
body_includes = $(addprefix $(call body_root,$1,$2)/,$(body_files.$1))
body_binding_inputs = build/body-inputs/$(basename $1).id $(wildcard offsets/$(basename $1).bindings)
STUB_OBJ := $(BODY_C:%.c=build/stub/obj/%.obj)
BINDING_IDS := $(BODY_C:%.c=build/body-inputs/%.id)
BODY_LINKS := $(foreach s,$(BODY_C),$(filter %/$(s:.c=.bin),$(LINKS)))
STUB_LINKS := $(BODY_LINKS:build/link/%=build/stub/link/%)
CHECK_LINKS := $(filter-out $(BODY_LINKS),$(LINKS)) $(STUB_LINKS)
EXPECTED += $(STUB_LINKS)

# tools/body_data.py writes every include of a body in one run. Its rule
# names the body's first include alone, and each other include waits on
# that one. A grouped rule would give each include a list of all the
# others, so a body would cost make the square of its count, and one body's
# land has thousands. The rule runs again when this file, which writes its
# command, changes. Where any include is missing, the rule takes FORCE, so
# removing one regenerates the body. These rules also order the first
# dependency scan, before a .d exists
BODY_TOOLS := tools/body_bindings.py tools/body_data.py tools/csource.py tools/layout.py tools/config.py \
              tools/decls.py tools/kinds.py tools/split.py tools/guards.py tools/inline.py
define body_rules
$(firstword $(call body_includes,$1,$2)): $1 $(call body_binding_inputs,$1) $(if $4,offsets/$(basename $1).txt) \
        $(body_headers.$1) config/$3/splits.txt \
        $(wildcard config/$3/symbols.txt config/1st_read/symbols.txt) $(BODY_TOOLS) config/bodies.mk $4 \
        $(if $(filter-out $(wildcard $(call body_includes,$1,$2)),$(call body_includes,$1,$2)),FORCE)
	@$$(call status,data,$1)
	$$(Q)python3 tools/body_data.py $3 $1 $2 $(if $4,--original $(firstword $4),--stub)
$(wordlist 2,$(words $(body_files.$1)),$(call body_includes,$1,$2)): | $(firstword $(call body_includes,$1,$2)) ;
endef

# Find the placing images, source headers and generated includes together so
# shared compiler headers are parsed in one Python process.
body_input_rules := build/body-inputs/sources.mk
$(shell $(python) tools/body_inputs.py $(body_input_rules) $(BODY_C))
ifneq ($(.SHELLSTATUS),0)
$(error tools/body_inputs.py: initializer dependency scan failed)
endif
include $(body_input_rules)

# A variant compiles its provider's declarations with its own image data.
define body_source
$(eval $(call body_rules,$1,build/stub,$(body_image.$1)))
$(if $(COMPILING),$(eval $(call body_rules,$1,build/gen,$(body_image.$1),$(call original,$(patsubst $(body_image.$1):%,%,$(filter $(body_image.$1):%,$(IMAGES)))))))
$(eval build/offsets/$(basename $1).txt: $(filter %.bindings,$(body_headers.$1)) $(wildcard offsets/$(basename $1).txt offsets/$(basename $1).bindings))
$(eval build/lint/$1.tidy: private TIDY_BODY_ROOT := $(call body_root,$1,build/stub))
$(eval build/lint/$1.tidy: $(call body_includes,$1,build/stub))
$(eval build/obj/$(basename $1).obj: $(call body_includes,$1,build/gen) config/bodies.mk)
$(eval build/stub/obj/$(basename $1).obj: private body_listing := $(or $(body_listing.$1),object))
$(eval build/stub/obj/$(basename $1).obj: $(call body_includes,$1,build/stub) config/bodies.mk)
$(foreach r,$(ROUTES),$(if $(filter $1,$($r.sources)),\
    $(eval build/obj/$(basename $1).obj: private $r.includes := $($r.includes),$(call body_root,$1,build/gen))\
    $(eval build/stub/obj/$(basename $1).obj: private $r.includes := $($r.includes),$(call body_root,$1,build/stub))))
endef
$(foreach source,$(BODY_C),$(call body_source,$(source)))

# Reuse the full route, including inline assembly and patches, with its source
# list restricted to body sources and its object targets under build/stub/.
# Both variants still take the same flags and command stamp.
stub_route = $(subst call shc,call stub_shc,$(subst build/obj/%.obj,build/stub/obj/%.obj,\
             $(subst $$($1.sources),$(filter $(BODY_C),$($1.sources)),$(call route,$1))))
# An automatic initializer also needs its C source lines to distinguish equal-sized templates.
# A machine-code stub asks SHC for its listing in the same invocation. Some
# Kamui source and option paths exceed SHC's parameter buffer, so the
# options use a response file. Inline and patched stubs already keep asmsh's
# listing of the instructions it actually assembled.
stub_shc = $(if $(filter -code=machinecode,$4),printf '%s\n' $4 '-listfile=$(@:.obj=.lis)' "-show=$(body_listing)" > $@.sub && \
    $(call shc,$1,$2,$3,-sub=$@.sub) && touch $@,$(call shc,$1,$2,$3,$4))
$(STUB_OBJ): private shc_tmp = $(@:build/stub/obj/%.obj=build/stub/tmp/%)
stub_object = build/stub/obj/$(patsubst $(call image,$1)/%,%,$1).obj
.SECONDEXPANSION:
$(STUB_LINKS): build/stub/link/%.bin: $$(call stub_object,$$*) \
        build/stub/link/%.rows tools/link.py tools/sysrof.py tools/config.py $(R32_ID)
	@$(call status,link,$@)
	$(Q)python3 tools/link.py --stub $< $(call image,$*)
$(STUB_LINKS:.bin=.rows): ;


# Losing a listing forces its stub to be recompiled. The measured table
# comes from this object, independent of the coordinates in offsets/.
MEASURED := $(BODY_C:%.c=build/offsets/%.txt)
$(STUB_OBJ): %.obj: %.lis
$(STUB_OBJ:.obj=.lis): ;
$(MEASURED): build/offsets/%.txt: build/stub/obj/%.obj tools/body_offsets.py tools/shc_listing.py tools/sysrof.py $(BODY_TOOLS)
	@$(call status,offset,$*.c)
	$(Q)python3 tools/body_offsets.py $*.c $(<:.obj=.lis) $@
$(MEASURED:.txt=.ok): build/offsets/%.ok: build/offsets/%.txt offsets/%.txt
	$(Q)cmp $< offsets/$*.txt || { [ $$? = 1 ] && echo "offsets/$*.txt disagrees with its native stub; run make offsets"; false; }
	$(Q)touch $@
