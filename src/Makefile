
VALAFLAGS=--pkg gtk+-3.0 --pkg glib-2.0 --pkg gee-0.8 --pkg libxml-2.0 
INTERFACE=interface/tpconnect.vala interface/tpsettings.vala interface/tpconfig.vala
CLASSES=classes/tppublic.vala classes/tpserverclass.vala
UTILS=utils/tpxml.vala utils/tpconfig-utils.vala
VALA=valac -q

vala:
	$(VALA) $(INTERFACE) $(CLASSES) $(UTILS) $(VALAFLAGS) -o build/tpc
	build/tpc