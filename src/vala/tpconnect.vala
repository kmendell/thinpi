using Gtk;

int main (string[] args) {
    Gtk.init (ref args);

    try {
        var builder = new Builder ();
        builder.add_from_file ("connect-manager.glade");
        var serverList = builder.get_object("serverSelect") as ComboBoxText;
        var wrongLabel = builder.get_object("wrongLabel") as Label;
        var settingsButton = builder.get_object("settingsButton") as Button;
        builder.connect_signals (null);
        var window = builder.get_object ("thinpiMain") as Window;
        window.destroy.connect (Gtk.main_quit);

        settingsButton.clicked.connect (() => {
            Gtk.main_quit();
        });

        wrongLabel.hide();
        //  window.show_all ();
        window.fullscreen();
        Gtk.main ();
    } catch (Error e) {
        stderr.printf ("Could not load ThinPi UI: %s\n", e.message);
        return 1;
    }

    return 0;
}