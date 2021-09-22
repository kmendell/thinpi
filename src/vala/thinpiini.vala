using Gee;

int readIni (string path, owned TPServer[] arr) {
    // A reference to our file
    //  var file = File.new_for_path ("thinpi.ini");
    var file = File.new_for_path (path);
    var list = new ArrayList<TPServer?> ();

    if (!file.query_exists ()) {
        stderr.printf ("File '%s' doesn't exist.\n", file.get_path ());
        return 1;
    }

    try {
        // Open file for reading and wrap returned FileInputStream into a
        // DataInputStream, so we can read line by line
        var dis = new DataInputStream (file.read ());
        string line;
        string tempServer;
        int numServers = -1;
        // Read lines until end of file (null) is reached
        var  tmps = TPServer();
        while ((line = dis.read_line (null)) != null) {
            bool conRes = line.contains("[connection]");
            bool endRes = line.contains("[connection end]");
            bool nameRes = line.contains("serverName = ");
            bool ipRes = line.contains("serverIP = ");
            bool domainRes = line.contains("serverDomain = ");
            bool screenRes = line.contains("serverScreen = ");
            
            if (conRes) {
                numServers++;
                stdout.printf ("%s %d\n", line, numServers);
            } else if (endRes) {
                stdout.printf ("%s\n", line);
            }
            if (numServers >= 0) {
                if (nameRes) {
                    tmps.serverName = line;
                } else if (ipRes) {
                    tmps.serverIP = line;
                } else if (domainRes) {
                    tmps.serverDomain = line;
                } else if (screenRes) {
                    tmps.serverScreen = line;
                }
                //  TPConfig.servers[0] = tmps;
                list.add(tmps);
            }
            
            
        }
        foreach (TPServer i in list) {
            stdout.printf ("%s\n", i.serverIP);
        }
        //  stdout.printf ("%s, %s, %s, %s\n", tmps.serverName, tmps.serverIP, tmps.serverDomain, tmps.serverScreen);
        //  stdout.printf ("%s, %s, %s, %s\n", tmps1.serverName, tmps1.serverIP, tmps1.serverDomain, tmps1.serverScreen);
        //  stdout.printf("%s\n", TPConfig.servers[0].serverName);
    } catch (Error e) {
        error ("%s", e.message);
    }

    return 0;
}

int main () {
    TPServer[] servers = {};

    readIni("src/vala/thinpi.ini", servers);
    //  stdout.printf ("%s\n", servers[0].serverName);
    return 0;
}