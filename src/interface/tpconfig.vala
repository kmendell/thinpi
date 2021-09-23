using Gtk;

int showConfig (string[] args) {
    Gtk.init (ref args);

    try {
        var builder = new Builder ();
        builder.add_from_file ("/thinpi/Interface/configmanager.glade");
        //  builder.connect_signals (null);
        var window3 = builder.get_object ("configWindow") as Window;
        window3.destroy.connect (window3.close);
        var mainLabel = builder.get_object("mainLabel") as Label;
        var configList = builder.get_object("configServerList") as ComboBoxText;
        var serverNameText = builder.get_object("serverNameTextbox") as Entry;
        var serverIPText = builder.get_object("ipaddressTextbox") as Entry;
        var serverDomainText = builder.get_object("domainNameTextbox") as Entry;
        var serverScreenText = builder.get_object("screenResTextbox") as Entry;
        var usbCheck = builder.get_object("usbCheckbox") as CheckButton;
        var printerCheck = builder.get_object("printerCheckbox") as CheckButton;
        var homeCheck = builder.get_object("homeCheckbox") as CheckButton;
        var editButton = builder.get_object("addButton") as Button;
        var removeButton = builder.get_object("removeButton") as Button;

        ConfigUtils.setupLoad(0, serverNameText, serverIPText, serverDomainText, serverScreenText, configList);
        
        
        //  window.show_all ();

        configList.changed.connect (() => {
            int id = configList.active;
            if (configList.get_active_text() == "<Add New>") {
                ConfigUtils.clearFields(serverNameText, serverIPText, serverDomainText, serverScreenText);
            } else {
                ConfigUtils.loadServerFields(id, serverNameText, serverIPText, serverDomainText, serverScreenText);
            }
            
        });
        
        ConfigUtils.loadServers(configList);

        configList.set_active(0);
        window3.show();
    } catch (Error e) {
        stderr.printf ("Could not load ThinPi UI: %s\n", e.message);
        return 1;
    }

    return 0;
}