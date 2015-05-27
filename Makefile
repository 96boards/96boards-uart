all: 96boards-uart.zip

%.zip: %-B_Cu.gbl %-B_Mask.gbs %-B_SilkS.gbo %-F_Cu.gtl %-F_Mask.gts %-F_SilkS.gto %-Edge_Cuts.gbr %.drl
	zip $@ $^
