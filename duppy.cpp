#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <random>
#include <fstream>


void checkRoot(){

		if (geteuid() != 0){

			std::cout << "Run as root!\n" << std::endl;;
			std:: exit(1);

		}
}

void createDiretories(){

		int statusCode;

		if ((statusCode = system("mkdir /tmp/duppy")) != 0 ){

			std::cout << "Create dir in /tmp: \e[31mFAILED!";
			std::exit(1);

		}

		if ((statusCode = system("mkdir /tmp/duppy/known_ap")) != 0 ){

			std::cout << "Create dir in /tmp/duppy: \e[31mFAILED!!" << std::endl;
			std::exit(1);

		}
}

void removeDirectories(){

		int statusCode;

		if ((statusCode = system("rm -rf /tmp/duppy")) != 0 ) {

			std::cout << "Remove the /tmp/duppy: \e[31mFAILED!! " << std::endl;

		}
}

void stopNetworkManager(){

		int statusCode;

		std::cout << "Stop network-manager: ";

		if ((statusCode = std::system("service network-manager stop")) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;
			std::exit(1);

		}
}

void startNetworkManager(){

		int statusCode;

		std::cout << "Start network-manager: ";

		if ((statusCode = std::system("service network-manager start")) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void ifconfigDown(const char *interfaceName){

		int statusCode;
		std::string command = "ifconfig ";
		command.append(interfaceName);
		command.append(" down");

		std::cout << "Interface " << interfaceName << " set down: ";

		if ((statusCode = std::system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;
			std::exit(1);

		}
}

void ifconfigUp(const char *interfaceName){

		int statusCode;

		std::string command = "ifconfig ";
		command.append(interfaceName);
		command.append(" up");

		std::cout << "Interface " << interfaceName << " set up: ";

		if ((statusCode = std::system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

std::string checkInterface(const char *interface){

		std::string line, e;
		std::ifstream myfile ("/proc/net/dev");

		if (myfile.is_open()){

			while ( getline (myfile,line) ){
				e += line;
			}
			myfile.close();
		} else {

			std::cout << "Open /proc/net/dev to check interface: \e[31mFAILED! " << std::endl;
			std::exit(1);

		}


		std::size_t found = e.find(interface);
		if (found == std::string::npos){

			std::cout << "Not a valid interface: " << interface << std::endl;
			std::cout << "Check \"iwconfig\" !\n" << std::endl;
			std::exit(1);
		}

		return interface;
}

int randomNumber(double min, double max){

		int num;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(min, max);

		num = (int)dist(mt);

		return num;
}

void setRandomHostname(){

		int statusCode;
		std::string hostname;
		std::string command = "hostnamectl set-hostname ";

		for (int i = 0; i < 10; i++){

			hostname += 'a' + randomNumber(0.0, 26.0);

		}

		command.append(hostname);

		std::cout << "Hostname change: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS! ";
			std::cout << "(New hostname: " << hostname << ")" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void saveDefaultHostname(){

		int statusCode;
		std::string defaultHostname;
		std::string command = "hostname > /tmp/duppy/hostname";

		std::cout << "Save default hostname: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;
			std::exit(1);

		}
}

void restoreDefaultHostname(){

		std::ifstream myfile ("/tmp/duppy/hostname");
		std::string defaultHostname;
		std::string command = "hostnamectl set-hostname ";
		int statusNumber;

		if (myfile.is_open()){

			getline(myfile, defaultHostname);
			myfile.close();

		} else {

			std::cout << "Open /tmp/duppy/hostname: \e[31mFAILED! " << std::endl;;

		}

		command += defaultHostname;

		std::cout << "Hostname set to default: ";

		if ((statusNumber = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void saveDefaultMac(const char *interface){

		int statusCode;
		std::string command = "cat /sys/class/net/";
		command.append(interface);
		command.append("/address > /tmp/duppy/defaultmac");

		std::cout << "Save default mac address: ";

		if ((statusCode = system(command.c_str())) == 0 ){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void restoreDefaultMac(const char *interface){

		int statusNumber;
		std::ifstream myfile ("/tmp/duppy/defaultmac");
		std::string defaultMac;
		std::string command = "ifconfig ";
		command.append(interface);
		command.append(" hw ether ");


		if (myfile.is_open()){

			getline(myfile, defaultMac);
			myfile.close();

		} else {

			std::cout << "Open /tmp/duppy/defaultmac: \e[31mFAILED!";

		}

		command += defaultMac;

		std::cout << "Mac address set to default: ";

		if ((statusNumber = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void setDuppyNetwork(){

		int statusCode;
		std::string command = "cp duppynetwork.conf /etc/NetworkManager/conf.d/";

		std::cout << "Insert duppynetwork.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void deleteDuppynetwork(){

		int statusCode;
		std::string command = "rm -f /etc/NetworkManager/conf.d/duppynetwork.conf";

		std::cout << "Remove duppynetwork.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void ignoreArp(const char *interface){

		int statusCode;
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf.";
		commandIgnore.append(interface);
		commandIgnore.append(".arp_ignore=8");

		std::cout << "Ignoring ARP: ";

		if ((statusCode = system(commandIgnore.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void restrictArpAnnounce(const char *interface){

		int statusCode;
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf.";
		commandAnnounce.append(interface);
		commandAnnounce.append(".arp_announce=2");

		std::cout << "Restrict ARP announce: ";

		if ((statusCode = system(commandAnnounce.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}




void allowArp(const char *interface){

		int statusCode;
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf.";
		commandIgnore.append(interface);
		commandIgnore.append(".arp_ignore=0");

		std::cout << "Enable ARP: ";

		if ((statusCode = system(commandIgnore.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void enableArpAnnounce(const char *interface){

		int statusCode;
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf.";
		commandAnnounce.append(interface);
		commandAnnounce.append(".arp_announce=0");

		std::cout << "Enable ARP announce: ";

		if ((statusCode = system(commandAnnounce.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;
			std::exit(1);

		}
}

void disableICMP(){

		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=1";

		std::cout << "Disable ping response: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!";
			std::cout << " (WARNING!!!! This affect every interface!)" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void enableICMP(){

		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=0";

		std::cout << "Enable ping response: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void removeKnownAP(){

		int statusCode;
		std::string command = "mv -f /etc/NetworkManager/system-connections/* /tmp/duppy/known_ap/";

		std::cout << "Temporary remove known APs: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void setBackKnownAP(){

		int statusCode;
		std::string commandMove = "mv -f /tmp/duppy/known_ap/* /etc/NetworkManager/system-connections/";
		std::string commandRm = "rm -f /etc/NetworkManager/system-connections/*";

		std::cout << "Remove the used AP: ";

		if ((statusCode = system(commandRm.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}

		std::cout << "Set back known APs: ";

		if ((statusCode = system(commandMove.c_str())) == 0){

			std::cout << "SUCCESS!" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!" << std::endl;

		}
}

void notify(std::string text){

	int statusCode;
	std::string command = "/usr/bin/notify-send \"" + text + "\"";

	if ((statusCode = system(command.c_str())) != 0) {

		std::cout << text << std::endl;
		std::cout << command << std::endl;

	}
}

int main(int argc, char* argv[]){

	std::cout << "\n";

	std::string start = "start";
	std::string stop = "stop";
	std::string initCommand;

	//check user id
	checkRoot();
	sleep(1);

	//Solve the "Segmentation fault" error if no arg
	if (argc != 3){

		std::cout << "Usage: ./duppy stop|start <interface> \n" << std::endl;
		exit(1);

	}

	//start Duppy
	if ((initCommand = argv[1]) == start.c_str()){


		std::string selectedInterface = checkInterface(argv[2]);
		sleep(1);

		createDiretories();
		sleep(1);

		saveDefaultHostname();
		sleep(1);

		saveDefaultMac(selectedInterface.c_str());
		sleep(1);

		stopNetworkManager();
		sleep(1);

		ifconfigDown(selectedInterface.c_str());
		sleep(1);

		setDuppyNetwork();
		sleep(1);

		setRandomHostname();
		sleep(1);

		ignoreArp(selectedInterface.c_str());
		sleep(1);

		restrictArpAnnounce(selectedInterface.c_str());
		sleep(1);

		disableICMP();
		sleep(1);

		removeKnownAP();
		sleep(1);

		ifconfigUp(selectedInterface.c_str());
		sleep(1);

		startNetworkManager();
		sleep(1);

		std::cout << "\nDont worry about the \"sudo: unable to resolve host...\" error.\n" << std::endl;
		notify("Duppy has started!");


	} else if ((initCommand = argv[1]) == stop.c_str()){

		//stop duppy

		std::string selectedInterface = checkInterface(argv[2]);

		stopNetworkManager();
		sleep(1);

		ifconfigDown(selectedInterface.c_str());
		sleep(1);

		restoreDefaultHostname();
		sleep(1);

		deleteDuppynetwork();
		sleep(1);

		restoreDefaultMac(selectedInterface.c_str());
		sleep(1);

		allowArp(selectedInterface.c_str());
		sleep(1);

		enableArpAnnounce(selectedInterface.c_str());
		sleep(1);

		enableICMP();
		sleep(1);

		setBackKnownAP();
		sleep(1);

		ifconfigUp(selectedInterface.c_str());
		sleep(1);

		startNetworkManager();
		sleep(1);

		removeDirectories();
		sleep(1);

		notify("Duppy has stopped!");

	} else {

		std::cout << "\nUsage: ./duppy stop|start <interface> " << std::endl;

	}
}
