#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <random>
#include <fstream>


void checkRoot(){

		if (geteuid() != 0){

			std::cout << "\n\tRun as ROOT!\n" << std::endl;;
			std:: exit(1);

		} else {

			std::cout << "\n";

		}
}

void createDiretories(){

		int statusCode;

		if ((statusCode = system("mkdir /tmp/duppy")) != 0 ){

			std::cout << "Create dir in /tmp: \e[31mFAILED!!\e[0m";
			std::exit(1);

		}

		if ((statusCode = system("mkdir /tmp/duppy/known_ap")) != 0 ){

			std::cout << "Create dir in /tmp/duppy: \e[31mFAILED!!\e[0m" << std::endl;
			std::exit(1);

		}
}

void removeDirectories(){

		int statusCode;

		if ((statusCode = system("rm -rf /tmp/duppy")) != 0 ) {

			std::cout << "Remove the /tmp/duppy: \e[31mFAILED!!\e[0m" << std::endl;

		}
}

void stopNetworkManager(){

		int statusCode;

		std::cout << "Stop network-manager: \t\t";

		if ((statusCode = std::system("service network-manager stop")) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}
}

void startNetworkManager(){

		int statusCode;

		std::cout << "Start network-manager: \t\t";

		if ((statusCode = std::system("service network-manager start")) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void ifconfigDown(std::string interfaceName){

		int statusCode;
		std::string command = "ifconfig " + interfaceName + " down";


		std::cout << "Interface " << interfaceName << " set down: \t";

		if ((statusCode = std::system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}
}

void ifconfigUp(std::string interfaceName){

		int statusCode;
		std::string command = "ifconfig " + interfaceName + " up";


		std::cout << "Interface " << interfaceName << " set up: \t";

		if ((statusCode = std::system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

std::string checkInterface(std::string interfaceName){

		std::string line, full;
		std::ifstream myfile ("/proc/net/dev");

		if (myfile.is_open()){

			while ( getline (myfile,line) ){
				full += line;
			}
			myfile.close();
		} else {

			std::cout << "Open /proc/net/dev to check interface: \e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}

		if (full.find(interfaceName) == std::string::npos){

			std::cout << "Not a valid interface: " << interfaceName << std::endl;
			std::cout << "Check \"iwconfig\" !\n" << std::endl;
			std::exit(1);
		}

		return interfaceName;
}

int randomNumber(double min, double max){

		int num;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<double> dist(min, max);

		num = (int)dist(mt);

		return num;
}

std::string getRandomHostname(){

		std::string RandomHostname;


		for (int i = 0; i < 10; i++){

			RandomHostname += 'a' + randomNumber(0.0, 26.0);

		}

		return RandomHostname;
}

void saveDefaultHostname(){

		int statusCode;
		std::string defaultHostname;
		std::string command = "hostname > /tmp/duppy/defaulthostname";

		std::cout << "Save default hostname: \t\t";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}
}

std::string getDefaultHostname(){

		std::ifstream myfile ("/tmp/duppy/defaulthostname");
		std::string defaultHostname;


		if (myfile.is_open()){

			getline(myfile, defaultHostname);
			myfile.close();

		} else {

			std::cout << "Open /tmp/duppy/defaulthostname: \e[31mFAILED!\e[0m" << std::endl;;

		}

		return defaultHostname;
}

std::string getActualHostname(){

		int statuscode;
		std::string actualhostname;
		std::string commandWrite = "hostname > /tmp/actualhostname";
		std::string commandRemove = "rm -f /tmp/actualhostname";


		if ((statuscode = system(commandWrite.c_str())) != 0 ){

			std::cout << "Save the actual hostname: \e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}

		//If dont declare here myfile, the open fail
		std::ifstream myfile ("/tmp/actualhostname");

		if (myfile.is_open()) {

			getline(myfile, actualhostname);

		} else {

			std::cout << "Open \"/tmp/actualhostname\": \e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}

		if ((statuscode =  system(commandRemove.c_str())) != 0 ){

			std::cout << "Remove \"/tmp/actualhostname\": \e[31mFAILED!\e[0m" << std::endl;

		}

		return actualhostname;
}

void setHostname(std::string hostname, std::string initCommand){

	int statuscodeKernelHostname;
	int statuscodeEtcHostname;
	int statuscodeHosts;
	int statuscodeXauth;


	std::string commandKernelHostname = "echo " + hostname + " > /proc/sys/kernel/hostname";
	std::string commandEtcHostname = "echo " + hostname + " > /etc/hostname";
	std::string commandHosts = "sed -i \'s/127.0.1.1.*/127.0.1.1\\t\'\"" + hostname + "\"\'/g\' /etc/hosts";
	std::string commandXauthAdd = "su -c \"xauth add $(xauth -b list | sed \'s/^.*\\//\'\"" + hostname + "\"\'\\//g\' | awk \'NR==1 {sub($1,\"\\\"&\\\"\"); print}\')\"";
	std::string commandXauthRemove = "xauth remove " + getActualHostname() + "/unix:0";

	//switch from start/stop

	if (initCommand == "start"){

		if ((statuscodeXauth =  system(commandXauthAdd.c_str())) != 0 ){

			std::cout << "Add Xauth entry: \t\t\e[31mFAILED!\e[0m" << std::endl;

		}

	} else if (initCommand == "stop") {

		if ((statuscodeXauth =  system(commandXauthRemove.c_str())) != 0 ){

			std::cout << "Remove Xauth entry: \t\t\e[31mFAILED!\e[0m" << std::endl;

		}
	}

	if ((statuscodeKernelHostname =  system(commandKernelHostname.c_str())) != 0 ){

		std::cout << "Write to /proc/sys/kernel/hostname: \e[31mFAILED!\e[0m" << std::endl;

	}

	if ((statuscodeEtcHostname =  system(commandEtcHostname.c_str())) != 0 ){

		std::cout << "Write /etc/hostname: \t\e[31mFAILED!\e[0m" << std::endl;

	}

	if ((statuscodeHosts =  system(commandHosts.c_str())) != 0 ){

		std::cout << "Write to /etc/hosts: \t\e[31mFAILED!\e[0m" << std::endl;

	}


	int changeResult = statuscodeKernelHostname + statuscodeEtcHostname + statuscodeHosts + statuscodeXauth;
	std::cout << "Set hostname to \"" << hostname << "\":\t";

	if (changeResult == 0) {

		std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

	} else {

		std::cout << "\e[31mFAILED!\e[0m" << std::endl;

	}
}

void saveDefaultMac(std::string interface){

		int statusCode;
		std::string command = "cat /sys/class/net/" + interface + "/address > /tmp/duppy/defaultmac";


		std::cout << "Save default mac address: \t";

		if ((statusCode = system(command.c_str())) == 0 ){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

std::string getDefaultMac(){

		std::ifstream myfile ("/tmp/duppy/defaultmac");
		std::string defaultMac;


		if (myfile.is_open()){

			getline(myfile, defaultMac);
			myfile.close();

		} else {

			std::cout << "Open /tmp/duppy/defaultmac: \e[31mFAILED!\e[0m";

		}

		return defaultMac;
}

std::string getActualMac(std::string interface){

		int statuscode;
		std::string actualmac;
		std::string commandWrite = "cat /sys/class/net/" + interface + "/address > /tmp/actualmac";
		std::string commandRemove = "rm -f /tmp/actualmac";

		if ((statuscode = system(commandWrite.c_str())) != 0 ){

			std::cout << "Save the actual mac: \e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}

		//If dont declare here myfile, the open fail
		std::ifstream myfile ("/tmp/actualmac");


		if (myfile.is_open()) {

			getline(myfile, actualmac);

		} else {

			std::cout << "Open \"/tmp/actualmac\": \e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}

		if ((statuscode =  system(commandRemove.c_str())) != 0 ){

			std::cout << "Remove \"/tmp/actualhostname\": \e[31mFAILED!\e[0m" << std::endl;

		}

		return actualmac;
}

void setMac(std::string interface, std::string mac){

		int statusNumber;
		std::string command = "ifconfig " + interface + " hw ether " + mac;

		std::cout << "Mac address set : \t\t";

		if ((statusNumber = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void setDuppyNetworkRandom(){

		int statusCode;
		std::string command = "cp duppynetworkRandom.conf /etc/NetworkManager/conf.d/";

		std::cout << "Insert duppynetworkRandom.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void setDuppyNetworkPreserve(){

		int statusCode;
		std::string command = "cp duppynetworkPreserve.conf /etc/NetworkManager/conf.d/";

		std::cout << "Insert duppynetworkRandom.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void deleteDuppynetworkRandom(){

		int statusCode;
		std::string command = "rm -f /etc/NetworkManager/conf.d/duppynetworkRandom.conf";

		std::cout << "Remove duppynetworkRandom.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void deleteDuppynetworkPreserve(){

		int statusCode;
		std::string command = "rm -f /etc/NetworkManager/conf.d/duppynetworkPreserve.conf";

		std::cout << "Remove duppynetworkRandom.conf: ";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void ignoreArp(std::string interface){

		int statusCode;
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf." + interface + ".arp_ignore=8";


		std::cout << "Ignoring ARP: \t\t\t";

		if ((statusCode = system(commandIgnore.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void restrictArpAnnounce(std::string interface){

		int statusCode;
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf." + interface + ".arp_announce=2";


		std::cout << "Restrict ARP announce: \t\t";

		if ((statusCode = system(commandAnnounce.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void allowArp(std::string interface){

		int statusCode;
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf." + interface + ".arp_ignore=0";


		std::cout << "Enable ARP: \t\t\t";

		if ((statusCode = system(commandIgnore.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void enableArpAnnounce(std::string interface){

		int statusCode;
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf." + interface + ".arp_announce=0";


		std::cout << "Enable ARP announce: \t\t";

		if ((statusCode = system(commandAnnounce.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;
			std::exit(1);

		}
}

void disableICMP(){

		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=1";

		std::cout << "Disable ping response: \t\t";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void enableICMP(){

		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=0";

		std::cout << "Enable ping response: \t\t";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void removeKnownAP(){

		int statusCode;
		std::string command = "mv -f /etc/NetworkManager/system-connections/* /tmp/duppy/known_ap/";

		std::cout << "Temporary remove known APs: \t";

		if ((statusCode = system(command.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void setBackKnownAP(){

		int statusCode;
		std::string commandMove = "mv -f /tmp/duppy/known_ap/* /etc/NetworkManager/system-connections/";
		std::string commandRm = "rm -f /etc/NetworkManager/system-connections/*";

		std::cout << "Remove the used AP: \t\t";

		if ((statusCode = system(commandRm.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}

		sleep(1);

		std::cout << "Set back known APs: \t\t";

		if ((statusCode = system(commandMove.c_str())) == 0){

			std::cout << "\e[32mSUCCESS!\e[0m" << std::endl;

		} else {

			std::cout << "\e[31mFAILED!\e[0m" << std::endl;

		}
}

void notify(std::string text){

		int statusCode;
		std::string command = "/usr/bin/notify-send \"Duppy\" \"" + text + "\"";

		if ((statusCode = system(command.c_str())) != 0) {

			std::cout << text << std::endl;

		}
}

void printHelp(){

		std::cout << "                start" << std::endl;
		std::cout << "Usage: ./duppy  stop   <interface>" << std::endl;
		std::cout << "                check\n" << std::endl;
		exit(1);

}

std::string checkDuppyStatus(){

		std::ifstream myfile ("/tmp/duppy/defaulthostname");


		if (myfile.is_open()){

			return "\e[32mENABLED\e[0m";

		} else {

			return "\e[31mDISABLED\e[0m";

		}
}

int main(int argc, char* argv[]){


	std::string start = "start";
	std::string stop = "stop";
	std::string check = "check";
	std::string initCommand;
	std::string selectedInterface;
	std::string selectedMac;

	checkRoot();
	sleep(1);

	//Solve the "Segmentation fault" error if no arg
	if (argc == 3){

		initCommand = argv[1];
		selectedInterface = checkInterface(argv[2]);

	} else if (argc == 4) {

		initCommand = argv[1];
		selectedMac = argv[2];
		selectedInterface = checkInterface(argv[3]);

	} else {

		printHelp();

	}


	if (initCommand == "start" ){

		//start Duppy

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

		setDuppyNetworkRandom();
		sleep(1);

		setHostname(getRandomHostname(), initCommand);
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

		notify("Started");
		sleep(1);

		notify("Dont worry about the 'sudo: unable to resolve host...' error");
		sleep(1);

	} else if (initCommand  == "stop" ){

		//stop duppy

		std::string defaultMac = getDefaultMac();
		std::string defaultHostname = getDefaultHostname();

		stopNetworkManager();
		sleep(1);

		ifconfigDown(selectedInterface.c_str());
		sleep(1);

		setHostname(defaultHostname, initCommand);
		sleep(1);

		deleteDuppynetworkRandom();
		sleep(1);

		setMac(selectedInterface.c_str(), defaultMac);
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

		notify("Stopped\nMAC: " + defaultMac + "\nHostname: " + defaultHostname);
		sleep(1);

	} else if (initCommand == "check" ){

		//check

		std::cout << "Duppy:\t\t" << checkDuppyStatus() << std::endl;
		sleep(1);

		std::cout << "Hostname:\t" << getActualHostname() << std::endl;
		sleep(1);

		std::cout << "Mac:\t\t" << getActualMac(selectedInterface) << std::endl;
		sleep(1);

	} else {

		printHelp();

	}
}
