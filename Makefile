all: 96boards-uart.zip 96boards-uart-seeed.csv

%-Edge_Cuts.gko: %-Edge_Cuts.gm1
	cp $< $@

%.zip: %-B_Cu.gbl %-B_Mask.gbs %-B_SilkS.gbo %-B_Paste.gbp %-F_Cu.gtl %-F_Mask.gts %-F_SilkS.gto %-F_Paste.gtp %-Edge_Cuts.gko %.drl %-NPTH.drl %-all.pos %-ft230x.xml %.csv
	zip $@ $^

%-seeed.csv: %.csv
	awk 'BEGIN { FS=","; OFS=","; } { print $$5,1,$$1; }' $< > $@

