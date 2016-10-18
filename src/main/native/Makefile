SUBDIRS = ftz compression pairhmm

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):;$(MAKE) -C $@

.PHONY: clean

clean: ;rm -f *.so