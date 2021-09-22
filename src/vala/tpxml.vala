public static void printConfig (Xml.Node* node, string node_name) {
	assert (node->name == node_name);

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.TEXT_NODE) {
			print ("%s   - %s\n", node_name, iter->get_content ());
		} else {
			print ("Unexpected element %s\n", iter->name);
		}
	}

}

public static string addToServer(Xml.Node* node, string node_name) {
	assert (node->name == node_name);

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.TEXT_NODE) {
			//  print ("%s   - %s\n", node_name, iter->get_content ());
			return iter->get_content();
		} else {
			print ("Unexpected element %s\n", iter->name);
			return "";
		}
	}
	return "";
}



public static TPServer readConfigFile (Xml.Node* node) {
	assert (node->name == "server");
	var setServer = TPServer();
	//  print (" * Server:\n");

	string? id = node->get_prop ("id");
	if (id != null) {
		//  print ("   - %s\n", id);
	} else {
		print ("Expected: <server id=...\n");
	}

	

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.ELEMENT_NODE) {
			switch (iter->name) {
			case "title":
				//  printConfig (iter, "title");
				string sTitle = addToServer(iter, "title");
				setServer.serverName = sTitle;
				//  print("%s", sTitle);
				break;

			case "ip":
				//  printConfig (iter, "ip");
				string sIP = addToServer(iter, "ip");
				setServer.serverIP = sIP;
				//  print("%s", sIP);
				break;

            case "domain":
				//  printConfig (iter, "domain");
				string sDomain = addToServer(iter, "domain");
				setServer.serverDomain = sDomain;
				//  print("%s", sDomain);
				break;

			case "usb":
				//  printConfig (iter, "usb");
				string sUsb = addToServer(iter, "usb");
				setServer.usbPass = sUsb;
				//  print("%s", sUsb);
				break;

			case "printer":
				//  printConfig (iter, "printer");
				string sPrinter = addToServer(iter, "printer");
				setServer.printerPass = sPrinter;
				//  print("%s", sPrinter);
				break;

			case "home":
				//  printConfig (iter, "home");
				string sHome = addToServer(iter, "home");
				setServer.homePass = sHome;
				//  print("%s", sHome);
				break;

			default:
				print ("Unexpected element %s\n", iter->name);
				break;
			}
			
		}
		
	}
	return setServer;
}

public static void print_books (Xml.Node* node) {
	assert (node->name == "connections");

	print ("ThinPi Connections:\n");
	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.ELEMENT_NODE) {
			if (iter->name == "server") {
				var temp = readConfigFile (iter);
				ThinPiPublic.addToArray(temp);
				
			} else {
				print ("Unexpected element %s\n", iter->name);
			}
		}
	}
	ThinPiPublic.seeServers();
}

public static int startconfig () {
	// Parse the document from path
	Xml.Doc* doc = Xml.Parser.parse_file ("src/vala/thinpi.xml");
	if (doc == null) {
		print ("File 'books.xml' not found or permissions missing\n");
		return 0;
	}

	Xml.Node* root = doc->get_root_element ();
	if (root == null) {
		print ("WANTED! root\n");
		delete doc;
		return 0;
	}

	if (root->name == "connections") {
		print_books (root);
	} else {
		print ("Unexpected element %s\n", root->name);
	}

	delete doc;
	return 0;
}
