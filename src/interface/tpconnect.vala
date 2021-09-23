using Gtk;

int main (string[] args) {
    Gtk.init (ref args);
    startconfig();

    bool DEBUG = true;

    try {
        var builder = new Builder ();
        builder.add_from_file ("/thinpi/Interface/connect-manager-debug.glade");
        var serverList = builder.get_object("serverSelect") as ComboBoxText;
        var wrongLabel = builder.get_object("wrongLabel") as Label;
        var settingsButton = builder.get_object("settingsButton") as Button;
        var configButton = builder.get_object("openConfigButton") as Button;
        var connectButton = builder.get_object("connect") as Button;
        var infoLabel = builder.get_object("copyrightLabel") as Label;
        
        builder.connect_signals (null);
        var window = builder.get_object ("thinpiMain") as Window;
        window.destroy.connect (Gtk.main_quit);
        connectButton.set_label("Close (Temp)");
        
        if (DEBUG) {
            infoLabel.set_label("THINPI 0.3.0 (UNSTABLE BUILD) Vala 0.48.19 - DEBUG = ON");
        } else {
            infoLabel.set_label("THINPI 0.3.0 (UNSTABLE BUILD) Vala 0.48.19 - DEBUG = OFF");
        }
        

        var ls_stdout = "";
	    var ls_stderr = "";
	    var ls_status = 0;

        connectButton.clicked.connect (() => {
                //  Process.spawn_command_line_sync ("/thinpi/thinpi-config", out ls_stdout, out ls_stderr, out ls_status);
                Gtk.main_quit();
        });

        settingsButton.clicked.connect (() => {
            showSettings(args);
            
        });

        configButton.clicked.connect (() => {
            showConfig(args);
        });

        
        window.show_all ();
        window.fullscreen();
        
        wrongLabel.hide();
        ThinPiPublic.seeServers();
        foreach (TPServer? i in ThinPiPublic.publicServerArray) {
            serverList.append_text(i.serverName);
        }
        
        serverList.set_active(0);

        Gtk.main ();
    } catch (Error e) {
        stderr.printf ("Could not load ThinPi UI: %s\n", e.message);
        return 1;
    }

    return 0;
}