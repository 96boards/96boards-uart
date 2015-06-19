all: 96boards-uart.zip 96boards-uart-seeed.csv

%.zip: %-B_Cu.gbl %-B_Mask.gbs %-B_SilkS.gbo %-F_Cu.gtl %-F_Mask.gts %-F_SilkS.gto %-Edge_Cuts.gbr %.drl %-NPTH.drl
	zip $@ $^

%-seeed.csv: %.csv
	awk 'BEGIN { FS=","; OFS=","; } { print $$4,$$2,$$3; }' $< > $@

